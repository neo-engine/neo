/*
Pokémon neo
------------------------------

file        : uio.cpp
author      : Philip Wellnitz
description : User IO

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <nds.h>

#include <algorithm>
#include <ctime>

#include "defines.h"
#include "fs.h"
#include "saveGame.h"
#include "type.h"
#include "uio.h"

namespace IO {
    font *regularFont
        = new font( REGULAR_FONT::fontData, REGULAR_FONT::fontWidths, REGULAR_FONT::shiftchar );
    font *boldFont = new font( BOLD_FONT::fontData, BOLD_FONT::fontWidths, BOLD_FONT::shiftchar );
    font *smallFont
        = new font( SMALL_FONT::fontData, SMALL_FONT::fontWidths, SMALL_FONT::shiftchar );
    ConsoleFont *consoleFont = new ConsoleFont( );

    u8 ASpriteOamIndex = 0;

    OAMTable * Oam = new OAMTable( );
    SpriteInfo spriteInfo[ SPRITE_COUNT ];

    OAMTable * OamTop = new OAMTable( );
    SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    u8 TEXTSPEED = 50;

    PrintConsole Top, Bottom;

    int bg3sub;
    int bg2sub;
    int bg3;
    int bg2;

#define TRANSPARENCY_COEFF 0x0671

    void initVideo( ) {
        vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
        vramSetBankB( VRAM_B_MAIN_BG_0x06020000 );

        vramSetBankE( VRAM_E_MAIN_SPRITE );

        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        // set up our top bitmap background
        bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( bg3, 3 );
        bgSetPriority( bg2, 2 );

        if( SCREENS_SWAPPED ) {
            REG_BLDCNT   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
            REG_BLDALPHA = TRANSPARENCY_COEFF;
        } else {
            REG_BLDCNT = BLEND_NONE;
        }
        bgUpdate( );
    }
    void initVideoSub( ) {
        vramSetBankC( VRAM_C_SUB_BG_0x06200000 );
        vramSetBankD( VRAM_D_SUB_SPRITE );

        /*  Set the video mode on the main screen. */
        videoSetModeSub( MODE_5_2D |          // Set the graphics mode to Mode 5
                         DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                         DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                         DISPLAY_SPR_ACTIVE | // Enable sprites for display
                         DISPLAY_SPR_1D       // Enable 1D tiled sprites
        );
        if( !SCREENS_SWAPPED ) {
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        } else {
            REG_BLDCNT_SUB = BLEND_NONE;
        }
    }
    void vramSetup( ) {
        initVideo( );
        initVideoSub( );
        vramSetBankG( VRAM_G_LCD );
        vramSetBankH( VRAM_H_LCD );
    }

    void swapScreens( ) {
        if( SCREENS_SWAPPED )
            lcdMainOnTop( );
        else
            lcdMainOnBottom( );
        SCREENS_SWAPPED = !SCREENS_SWAPPED;

        if( !SCREENS_SWAPPED ) {
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
            REG_BLDCNT       = BLEND_NONE;
        } else {
            REG_BLDCNT_SUB = BLEND_NONE;
            REG_BLDCNT     = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
            REG_BLDALPHA   = TRANSPARENCY_COEFF;
        }
    }

    void animateBG( u8 p_frame, int p_bg ) {
        if( p_frame == 255 ) { bgScrollf( p_bg, -( 256 << 6 ), -( 256 << 6 ) ); }
        bgScrollf( p_bg, 1 << 6, 1 << 6 );
    }

    void setDefaultConsoleTextColors( u16 *p_palette, u8 p_start ) {
        p_palette[ ( p_start + 0 ) ] = RGB15( 15, 0, 0 );  // 31 normal red
        p_palette[ ( p_start + 1 ) ] = RGB15( 0, 15, 0 );  // 32 normal green
        p_palette[ ( p_start + 2 ) ] = RGB15( 15, 15, 0 ); // 33 normal yellow

        p_palette[ ( p_start + 3 ) ] = RGB15( 0, 0, 15 );   // 34 normal blue
        p_palette[ ( p_start + 4 ) ] = RGB15( 15, 0, 15 );  // 35 normal magenta
        p_palette[ ( p_start + 5 ) ] = RGB15( 0, 15, 15 );  // 36 normal cyan
        p_palette[ ( p_start + 6 ) ] = RGB15( 24, 24, 24 ); // 37 normal white
    }

    bool waitForTouchUp( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 ) {
        return waitForTouchUp( inputTarget( p_targetX1, p_targetY1, p_targetX2, p_targetY2 ) );
    }
    bool waitForTouchUp( inputTarget p_inputTarget ) {
        touchPosition touch;
        if( p_inputTarget.m_inputType == inputTarget::inputType::TOUCH ) {
            loop( ) {
                swiWaitForVBlank( );
                scanKeys( );
                touchRead( &touch );
                if( TOUCH_UP ) return true;
                if( !IN_RANGE_I( touch, p_inputTarget ) ) return false;
            }
        }
        if( p_inputTarget.m_inputType == inputTarget::inputType::TOUCH_CIRCLE ) {
            loop( ) {
                swiWaitForVBlank( );
                scanKeys( );
                touchRead( &touch );
                if( TOUCH_UP ) return true;
                if( !IN_RANGE_I_C( touch, p_inputTarget ) ) return false;
            }
        }
        return false;
    }

    bool waitForKeysUp( KEYPAD_BITS p_keys ) {
        return waitForKeysUp( inputTarget( p_keys ) );
    }
    bool waitForKeysUp( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            loop( ) {
                scanKeys( );
                swiWaitForVBlank( );
                if( keysUp( ) & p_inputTarget.m_keys ) return true;
                if( !( keysHeld( ) & p_inputTarget.m_keys ) ) return true;
            }
        }
        return false;
    }

    bool waitForInput( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON )
            return waitForKeysUp( p_inputTarget );
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
        BG_PALETTE_SUB[ GRAY_IDX ]  = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ]   = RED;
        BG_PALETTE_SUB[ BLUE_IDX ]  = BLUE;
        printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true, WHITE_IDX );
    }

    /*
     * @brief Sets a pixel to the specified color
     */
    void setPixel( u8 p_x, u8 p_y, bool p_bottom, u8 p_color, u8 p_layer ) {
        if( p_bottom ) {
            color old
                = ( (color *) BG_BMP_RAM_SUB( p_layer ) )[ ( p_x + p_y * (u16) SCREEN_WIDTH ) / 2 ];
            u8 bot = old, top = old >> 8;
            if( p_x & 1 )
                old = ( ( (u8) p_color ) << 8 ) | bot;
            else
                old = ( top << 8 ) | ( (u8) p_color );

            ( (color *) BG_BMP_RAM_SUB( p_layer ) )[ ( p_x + p_y * (u16) SCREEN_WIDTH ) / 2 ] = old;
        } else {
            color old
                = ( (color *) BG_BMP_RAM( p_layer ) )[ ( p_x + p_y * (u16) SCREEN_WIDTH ) / 2 ];
            u8 bot = old, top = old >> 8;
            if( p_x & 1 )
                old = ( ( (u8) p_color ) << 8 ) | bot;
            else
                old = ( top << 8 ) | ( (u8) p_color );

            ( (color *) BG_BMP_RAM( p_layer ) )[ ( p_x + p_y * (u16) SCREEN_WIDTH ) / 2 ] = old;
        }
    }

    /*
     * @brief Prints a rectangle to the screen, all coordinates inclusive
     */
    void printRectangle( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, u8 p_color,
                         u8 p_layer ) {
        for( u16 y = p_y1; y <= p_y2; ++y )
            for( u16 x = p_x1; x <= p_x2; ++x ) { setPixel( x, y, p_bottom, p_color, p_layer ); }
    }

    /*
     * @brief A sine approximation via a third-order cosine approx.
     * @param p_x   angle (with 2^15 units/circle)
     * @return     Sine value (Q12)
     */
    s32 isin( s32 p_x ) {
        const u16 qN = 13, qA = 12, qP = 15, qR = 2 * qN - qP, qS = qN + qP + 1 - qA;
        p_x <<= ( 30 - qN );             // shift to full s32 range (Q13->Q30)
        if( ( p_x ^ ( p_x << 1 ) ) < 0 ) // test for quadrant 1 or 2
            p_x = ( 1 << 31 ) - p_x;
        p_x >>= ( 30 - qN );
        return p_x * ( ( 3 << qP ) - ( p_x * p_x >> qR ) ) >> qS;
    }

    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, bool p_big ) {
        if( p_big )
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 20, 24 );
        else
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 8, 12 );
    }
    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        p_HP = std::max( std::min( (u16) 101, p_HP ), u16( 0 ) );

        if( p_HP > 100 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = GREEN;
            for( u16 phi = 0; phi < 300; phi++ ) {
                s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j ) --nx;
                    ( (color *) ( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                        = ( ( (u8) p_freecolor1 ) << 8 ) | (u8) p_freecolor1;
                    // printf( "%i %i; ", nx, ny );
                }
            }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = NORMAL_COLOR;
            if( 100 - p_HPstart <= p_HP ) {
                for( u16 phi = 3 * ( 100 - p_HPstart ); phi < 3 * p_HP; phi++ ) {
                    s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                    s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                    for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                        u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                        u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                        if( nx == p_x + 16 + j ) --nx;

                        ( (color *) ( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                            = ( ( (u8) p_freecolor2 ) << 8 ) | (u8) p_freecolor2;
                        if( phi >= 150 ) BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
                        if( phi >= 225 ) BG_PAL( p_sub )[ p_freecolor1 ] = RED;
                    }
                    if( p_delay ) swiWaitForVBlank( );
                }
            } else {
                for( u16 phi = 3 * ( 100 - p_HPstart ); phi > 3 * p_HP; phi-- ) {
                    s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                    s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                    for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                        u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                        u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                        if( nx == p_x + 16 + j ) --nx;

                        ( (color *) ( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                            = ( ( (u8) p_freecolor1 ) << 8 ) | (u8) p_freecolor1;
                        if( phi < 225 ) BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
                        if( phi < 150 ) BG_PAL( p_sub )[ p_freecolor1 ] = GREEN;
                    }
                    if( p_delay ) swiWaitForVBlank( );
                }
            }
        }
    }
    void displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        if( p_EPstart >= 100 || p_EP > 100 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = NORMAL_COLOR;
            for( u16 phi = 0; phi < 300; phi++ ) {
                s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j ) --nx;
                    ( (color *) BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                        = ( ( (u8) p_freecolor1 ) << 8 ) | (u8) p_freecolor1;
                    // printf("%i %i; ",nx,ny);
                }
            }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = ICE_COLOR;
            for( u16 phi = 3 * p_EPstart; phi <= 3 * p_EP; ++phi ) {
                s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 - j * ( x / ( 1.0 * ( 1 << 12 ) ) );
                    u16 ny = p_y + 16 - j * ( y / ( 1.0 * ( 1 << 12 ) ) );
                    if( nx == p_x + 16 + j ) --nx;
                    ( (color *) BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                        = ( ( (u8) p_freecolor2 ) << 8 ) | (u8) p_freecolor2;
                }
                if( p_delay ) swiWaitForVBlank( );
            }
        }
    }

    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_colorIdx,
                         bool p_pressed, bool p_bottom ) {
        printChoiceBox( p_x1, p_y1, p_x2, p_y2, p_borderWidth, p_borderWidth, p_colorIdx, p_pressed,
                        p_bottom );
    }

    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_borderWidth2,
                         u8 p_colorIdx, bool p_pressed, bool p_bottom ) {
        if( !p_pressed ) {
            printRectangle( p_x2 - 2, p_y1 + 1, p_x2, p_y2, p_bottom, BLACK_IDX );
            printRectangle( p_x1 + 1, p_y2 - 1, p_x2, p_y2, p_bottom, BLACK_IDX );

            printRectangle( p_x1, p_y1, p_x1 + p_borderWidth, p_y2 - 1, p_bottom, p_colorIdx );
            printRectangle( p_x2 - p_borderWidth2 - 2, p_y1, p_x2 - 2, p_y2 - 1, p_bottom,
                            p_colorIdx );
            printRectangle( p_x1, p_y1, p_x2 - 2, p_y1 + p_borderWidth - 2, p_bottom, p_colorIdx );
            printRectangle( p_x1, p_y2 - p_borderWidth + 3, p_x2 - 2, p_y2 - 1, p_bottom,
                            p_colorIdx );

            printRectangle( p_x2 - p_borderWidth2 - 1, p_y1 + p_borderWidth - 1,
                            p_x2 - p_borderWidth2, p_y2 - p_borderWidth + 4, p_bottom, BLACK_IDX );
            printRectangle( p_x1 + 1 + p_borderWidth, p_y2 - p_borderWidth + 2,
                            p_x2 - p_borderWidth2, p_y2 - p_borderWidth + 4, p_bottom, BLACK_IDX );
            printRectangle( p_x1 + p_borderWidth, p_y1 + p_borderWidth - 2,
                            p_x2 - p_borderWidth2 - 2, p_y2 - p_borderWidth + 3, p_bottom,
                            WHITE_IDX );
        } else {
            printRectangle( p_x1, p_y1, p_x1 + 2, p_y2 - 1, p_bottom, 0 );
            printRectangle( p_x1, p_y1, p_x2 - 1, p_y1 + 1, p_bottom, 0 );

            printRectangle( p_x1 + 2, p_y1 + 1, p_x1 + 2 + p_borderWidth, p_y2, p_bottom,
                            p_colorIdx );
            printRectangle( p_x2 - p_borderWidth2, p_y1 + 1, p_x2, p_y2, p_bottom, p_colorIdx );
            printRectangle( p_x1 + 2, p_y1 + 1, p_x2, p_y1 + p_borderWidth - 1, p_bottom,
                            p_colorIdx );
            printRectangle( p_x1 + 2, p_y2 - p_borderWidth + 3, p_x2, p_y2, p_bottom, p_colorIdx );

            printRectangle( p_x2 - p_borderWidth2 + 1, p_y1 + p_borderWidth,
                            p_x2 - p_borderWidth2 + 2, p_y2 - p_borderWidth + 4, p_bottom,
                            BLACK_IDX );
            printRectangle( p_x1 + 3 + p_borderWidth, p_y2 - p_borderWidth + 4,
                            p_x2 - p_borderWidth2 + 2, p_y2 - p_borderWidth + 4, p_bottom,
                            BLACK_IDX );

            printRectangle( p_x1 + 2 + p_borderWidth, p_y1 + p_borderWidth - 1,
                            p_x2 - p_borderWidth2, p_y2 - p_borderWidth + 3, p_bottom, WHITE_IDX );
        }
    }

    u16 getColor( type p_type ) {
        switch( p_type ) {
        case NORMAL:
            return NORMAL_COLOR;
            break;
        case FIGHTING:
            return RED;
            break;
        case FLYING:
            return TURQOISE;
            break;
        case POISON:
            return POISON_COLOR;
            break;
        case GROUND:
            return GROUND_COLOR;
            break;
        case ROCK:
            return ROCK_COLOR;
            break;
        case BUG:
            return BUG_COLOR;
            break;
        case GHOST:
            return GHOST_COLOR;
            break;
        case STEEL:
            return STEEL_COLOR;
            break;
        case UNKNOWN:
            return UNKNOWN_COLOR;
            break;
        case WATER:
            return BLUE;
            break;
        case FIRE:
            return ORANGE;
            break;
        case GRASS:
            return GREEN;
            break;
        case LIGHTNING:
            return YELLOW;
            break;
        case PSYCHIC:
            return PURPLE;
            break;
        case ICE:
            return ICE_COLOR;
            break;
        case DRAGON:
            return DRAGON_COLOR;
            break;
        case DARKNESS:
            return BLACK;
            break;
        case FAIRY:
            return FAIRY_COLOR;
            break;
        default:
            return DRAGON_COLOR;
            break;
        }
        return WHITE;
    }
} // namespace IO
