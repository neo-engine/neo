/*
Pokémon Emerald 2 Version
------------------------------

file        : uio.cpp
author      : Philip Wellnitz
description : User IO

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <nds.h>

#include <ctime>

#include "uio.h"
#include "defines.h"
#include "type.h"
#include "saveGame.h"
#include "fs.h"

namespace IO {
    font* regularFont = new font( REGULAR_FONT::fontData, REGULAR_FONT::fontWidths, REGULAR_FONT::shiftchar );
    font* boldFont = new font( BOLD_FONT::fontData, BOLD_FONT::fontWidths, BOLD_FONT::shiftchar );
    ConsoleFont* consoleFont = new ConsoleFont( );

    u8 ASpriteOamIndex = 0;

    OAMTable *Oam = new OAMTable( );
    SpriteInfo spriteInfo[ SPRITE_COUNT ];

    OAMTable *OamTop = new OAMTable( );
    SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    u8 TEXTSPEED = 50;

    PrintConsole Top, Bottom;

    int bg3sub;
    int bg2sub;
    int bg3;
    int bg2;

    void initVideo( ) {

        vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
        vramSetBankB( VRAM_B_MAIN_BG_0x06020000 );

        vramSetBankE( VRAM_E_MAIN_SPRITE );

        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

        // set up our top bitmap background
        bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( bg3, 3 );
        bgUpdate( );
    }
    void initVideoSub( ) {

        vramSetBankC( VRAM_C_SUB_BG_0x06200000 );
        vramSetBankD( VRAM_D_SUB_SPRITE );


        /*  Set the video mode on the main screen. */
        videoSetModeSub( MODE_5_2D | // Set the graphics mode to Mode 5
                         DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                         DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                         DISPLAY_SPR_ACTIVE | // Enable sprites for display
                         DISPLAY_SPR_1D       // Enable 1D tiled sprites
                         );
    }
    void vramSetup( ) {
        initVideo( );
        initVideoSub( );
        VRAM_F_CR = VRAM_ENABLE | VRAM_F_BG_EXT_PALETTE | VRAM_OFFSET( 1 );
        vramSetBankG( VRAM_G_LCD );
        vramSetBankH( VRAM_H_LCD );
    }

    void setDefaultConsoleTextColors( u16* p_palette, u8 p_start ) {
        p_palette[ p_start * 16 - 1 ] = RGB15( 0, 0, 0 ); //30 normal black
        p_palette[ ( p_start + 1 ) * 16 - 1 ] = RGB15( 15, 0, 0 ); //31 normal red
        p_palette[ ( p_start + 2 ) * 16 - 1 ] = RGB15( 0, 15, 0 ); //32 normal green
        p_palette[ ( p_start + 3 ) * 16 - 1 ] = RGB15( 15, 15, 0 ); //33 normal yellow

        p_palette[ ( p_start + 4 ) * 16 - 1 ] = RGB15( 0, 0, 15 ); //34 normal blue
        p_palette[ ( p_start + 5 ) * 16 - 1 ] = RGB15( 15, 0, 15 ); //35 normal magenta
        p_palette[ ( p_start + 6 ) * 16 - 1 ] = RGB15( 0, 15, 15 ); //36 normal cyan
        p_palette[ ( p_start + 7 ) * 16 - 1 ] = RGB15( 24, 24, 24 ); //37 normal white
    }

    bool waitForTouchUp( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 ) {
        return waitForTouchUp( inputTarget( p_targetX1, p_targetY1, p_targetX2, p_targetY2 ) );
    }
    bool waitForTouchUp( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::TOUCH ) {
            loop( ) {
                swiWaitForVBlank( );
                scanKeys( );
                auto touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    return true;
                if( !IN_RANGE( touch, p_inputTarget ) )
                    return false;
            }
        }
        if( p_inputTarget.m_inputType == inputTarget::inputType::TOUCH_CIRCLE ) {
            loop( ) {
                swiWaitForVBlank( );
                scanKeys( );
                auto touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    return true;
                if( !IN_RANGE_C( touch, p_inputTarget ) )
                    return false;
            }
        }
        return false;
    }

    void waitForKeysUp( KEYPAD_BITS p_keys ) {
        return waitForKeysUp( inputTarget( p_keys ) );
    }
    void waitForKeysUp( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            loop( ) {
                scanKeys( );
                keysCurrent( );
                swiWaitForVBlank( );
                if( keysUp( ) & p_inputTarget.m_keys )
                    return;
            }
        }
    }

    bool waitForInput( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            waitForKeysUp( p_inputTarget );
            return true;
        }
        return waitForTouchUp( p_inputTarget );
    }
    
    void initTextField( ) {
        regularFont->setColor( 0, 0 );
        regularFont->setColor( BLACK_IDX, 1 );
        regularFont->setColor( GRAY_IDX, 2 );
        boldFont->setColor( 0, 0 );
        boldFont->setColor( GRAY_IDX, 1 );
        boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
        BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
        printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true, false, WHITE_IDX );
    }

    void putrec( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, bool p_striped, u8 p_color ) {
        printRectangle( p_x1, p_y1, p_x2, p_y2, p_bottom, p_striped, p_color );
    }
    void printRectangle( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, bool p_striped, u8 p_color ) {
        for( u16 x = p_x1; x <= p_x2; ++x ) for( u16 y = p_y1; y < p_y2; ++y )
            if( p_bottom )
                ( (color *) BG_BMP_RAM_SUB( 1 ) )[ ( x + y * (u16) SCREEN_WIDTH ) / 2 ] = !p_striped ? ( ( (u8) p_color ) << 8 ) | ( (u8) p_color ) : p_color;
            else
                ( (color *) BG_BMP_RAM( 1 ) )[ ( x + y * (u16) SCREEN_WIDTH ) / 2 ] = !p_striped ? ( ( (u8) p_color ) << 8 ) | ( (u8) p_color ) : p_color;
    }

    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, bool p_big ) {
        if( p_big )
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 20, 24 );
        else
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 8, 12 );
    }
    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        p_HP = std::max( std::min( (u16) 101, p_HP ), u16( 0 ) );

        if( p_HP > 100 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = GREEN;
            for( u16 phi = 0; phi < 300; phi++ ) {
                s16 x = cosLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                s16 y = sinLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j )
                        --nx;
                    ( (color *) ( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8) p_freecolor1 ) << 8 ) | (u8) p_freecolor1;
                    //printf( "%i %i; ", nx, ny );
                }
            }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = NORMAL_;
            for( u16 phi = 3 * ( 100 - p_HPstart ); phi < 3 * p_HP; phi++ ) {
                s16 x = cosLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                s16 y = sinLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j )
                        --nx;

                    ( (color *) ( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8) p_freecolor2 ) << 8 ) | (u8) p_freecolor2;
                    if( phi >= 150 )
                        BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
                    if( phi >= 225 )
                        BG_PAL( p_sub )[ p_freecolor1 ] = RED;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }
    void displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        if( p_EPstart >= 100 || p_EP > 100 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = NORMAL_;
            for( u16 phi = 0; phi < 300; phi++ ) {
                s16 x = cosLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                s16 y = sinLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j )
                        --nx;
                    ( (color *) BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8) p_freecolor1 ) << 8 ) | (u8) p_freecolor1;
                    //printf("%i %i; ",nx,ny);
                }
            }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = ICE;
            for( u16 phi = 3 * p_EPstart; phi <= 3 * p_EP; ++phi ) {
                s16 x = cosLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                s16 y = sinLerp( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j )
                        --nx;
                    ( (color *) BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8) p_freecolor2 ) << 8 ) | (u8) p_freecolor2;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }

    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_colorIdx, bool p_pressed ) {
        printChoiceBox( p_x1, p_y1, p_x2, p_y2, p_borderWidth, p_borderWidth, p_colorIdx, p_pressed );
    }

    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_borderWidth2, u8 p_colorIdx, bool p_pressed ) {
        if( !p_pressed ) {
            printRectangle( p_x1 + 1, p_y1 + 1, p_x2, p_y2,
                            true, false, BLACK_IDX );
            printRectangle( p_x1, p_y1, p_x2 - 2, p_y2 - 1,
                            true, false, p_colorIdx );
            printRectangle( p_x1 + 1 + p_borderWidth, p_y1 + p_borderWidth - 1, p_x2 - p_borderWidth2, p_y2 - p_borderWidth + 4,
                            true, false, BLACK_IDX );
            printRectangle( p_x1 + p_borderWidth, p_y1 + p_borderWidth - 2, p_x2 - p_borderWidth2 - 2, p_y2 - p_borderWidth + 3,
                            true, false, WHITE_IDX );
        } else {
            printRectangle( p_x1, p_y1, p_x2 - 1, p_y2 - 1,
                            true, false, 0 );
            printRectangle( p_x1 + 2, p_y1 + 1, p_x2, p_y2,
                            true, false, p_colorIdx );

            printRectangle( p_x1 + 3 + p_borderWidth, p_y1 + p_borderWidth, p_x2 - p_borderWidth2 + 2, p_y2 - p_borderWidth + 4,
                            true, false, BLACK_IDX );

            printRectangle( p_x1 + 2 + p_borderWidth, p_y1 + p_borderWidth - 1, p_x2 - p_borderWidth2, p_y2 - p_borderWidth + 3,
                            true, false, WHITE_IDX );
        }
    }


    void printChar( font* p_font, u16 p_ch, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printChar( p_ch, p_x, p_y, p_bottom );
    }
    void printString( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance ) {
        p_font->printString( p_string, p_x, p_y, p_bottom, p_yDistance );
    }
    void printStringCenter( font* p_font, const char *p_string, bool p_bottom ) {
        p_font->printStringCenter( p_string, p_bottom );
    }
    void printStringD( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printStringD( p_string, p_x, p_y, p_bottom );
    }
    void printStringCenterD( font* p_font, const char *p_string, bool p_bottom ) {
        p_font->printStringCenterD( p_string, p_bottom );
    }
    void printNumber( font* p_font, s32 p_num, s16 p_x, s16 p_y, bool p_bottom );

    void printString( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance ) {
        p_font->printString( p_string, p_x, p_y, p_bottom, p_yDistance );
    }
    void printMBString( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printMBString( p_string, p_x, p_y, p_bottom );
    }
    void printMBString( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printMBString( p_string, p_x, p_y, p_bottom );
    }
    void printMBStringD( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printMBStringD( p_string, p_x, p_y, p_bottom );
    }
    void printMBStringD( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printMBStringD( p_string, p_x, p_y, p_bottom );
    }
    void printStringCenter( font* p_font, const wchar_t *p_string, bool p_bottom ) {
        p_font->printStringCenter( p_string, p_bottom );
    }
    void printStringD( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        p_font->printStringD( p_string, p_x, p_y, p_bottom );
    }
    void printStringCenterD( font* p_font, const wchar_t *p_string, bool p_bottom ) {
        p_font->printStringCenterD( p_string, p_bottom );
    }

    void topScreenPlot( u8 p_x, u8 p_y, color p_color ) {
        if( ( p_color >> 8 ) != 0 && ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *) BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color;
        else if( ( p_color >> 8 ) != 0 )
            ( (color *) BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *) BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] % ( 1 << 8 ) );
        else if( ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *) BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *) BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] << 8 );
    }
    void btmScreenPlot( u8 p_x, u8 p_y, color p_color ) {
        if( ( p_color >> 8 ) != 0 && ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *) BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color;
        else if( ( p_color >> 8 ) != 0 )
            ( (color *) BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *) BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] % ( 1 << 8 ) );
        else if( ( p_color % ( 1 << 8 ) ) != 0 )
            ( (color *) BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *) BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] << 8 );
    }

    u16 getColor( Type p_type ) {
        switch( p_type ) {
            case NORMAL:
                return NORMAL_;
                break;
            case KAMPF:
                return RED;
                break;
            case FLUG:
                return TURQOISE;
                break;
            case GIFT:
                return POISON;
                break;
            case BODEN:
                return GROUND;
                break;
            case GESTEIN:
                return ROCK;
                break;
            case KAEFER:
                return BUG;
                break;
            case GEIST:
                return GHOST;
                break;
            case STAHL:
                return STEEL;
                break;
            case UNBEKANNT:
                return UNKNOWN;
                break;
            case WASSER:
                return BLUE;
                break;
            case FEUER:
                return ORANGE;
                break;
            case PFLANZE:
                return GREEN;
                break;
            case ELEKTRO:
                return YELLOW;
                break;
            case PSYCHO:
                return PURPLE;
                break;
            case EIS:
                return ICE;
                break;
            case DRACHE:
                return DRAGON;
                break;
            case UNLICHT:
                return BLACK;
                break;
            case FEE:
                return FAIRY;
                break;
            default:
                return DRAGON;
                break;
        }
        return WHITE;
    }
}