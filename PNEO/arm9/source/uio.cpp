/*
Pokémon neo
------------------------------

file        : uio.cpp
author      : Philip Wellnitz
description : User IO

Copyright (C) 2012 - 2026
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

#include "battle/type.h"
#include "defines.h"
#include "fs/data.h"
#include "io/util.h"
#include "save/saveGame.h"

namespace IO {
    ConsoleFont *consoleFont = new ConsoleFont( );

    OAMTable  *Oam = new OAMTable( );
    SpriteInfo spriteInfo[ SPRITE_COUNT ];

    OAMTable  *OamTop = new OAMTable( );
    SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    PrintConsole Top, Bottom;

    int bg3sub;
    int bg2sub;
    int bg3;
    int bg2;

    void initVideo( bool p_noFade ) {
        vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
        //        vramSetBankB( VRAM_B_MAIN_BG_0x06020000 );

        vramSetBankB( VRAM_B_MAIN_SPRITE );

        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        // set up our top bitmap background
        bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( bg3, 3 );
        bgSetPriority( bg2, 2 );

        if( !p_noFade ) {
            if( SCREENS_SWAPPED ) {
                REG_BLDCNT   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
                REG_BLDALPHA = TRANSPARENCY_COEFF;
            } else {
                REG_BLDCNT = BLEND_NONE;
            }
        }
    }
    void initVideoSub( bool p_noFade ) {
        // vramSetBankC( VRAM_C_SUB_BG_0x06200000 );
        vramSetBankC( VRAM_C_SUB_BG_0x06200000 );
        vramSetBankD( VRAM_D_SUB_SPRITE );

        /*  Set the video mode on the main screen. */
        videoSetModeSub( MODE_5_2D |          // Set the graphics mode to Mode 5
                         DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                         DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                         DISPLAY_SPR_ACTIVE | // Enable sprites for display
                         DISPLAY_SPR_1D       // Enable 1D tiled sprites
        );
        if( !p_noFade ) {
            if( !SCREENS_SWAPPED ) {
                REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
                REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
            } else {
                REG_BLDCNT_SUB = BLEND_NONE;
            }
        }
    }
    void vramSetup( bool p_noFade ) {
        initVideo( p_noFade );
        initVideoSub( p_noFade );
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

    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, bool p_big ) {
        if( p_big ) {
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 20, 24 );
        } else {
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 9, 12 );
        }
    }
    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        p_HP = std::max( std::min( (u16) 101, p_HP ), u16( 0 ) );

        if( 3 * ( p_HPstart ) < 75 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = RED;
            BG_PAL( p_sub )[ p_freecolor2 ] = RED2;
        } else if( 3 * p_HPstart < 150 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
            BG_PAL( p_sub )[ p_freecolor2 ] = YELLOW2;
        } else {
            BG_PAL( p_sub )[ p_freecolor1 ] = GREEN;
            BG_PAL( p_sub )[ p_freecolor2 ] = GREEN2;
        }

        if( p_HPstart < p_HP ) {
            // heal
            for( u16 phi = 3 * p_HPstart; phi <= 3 * p_HP; phi++ ) {
                if( phi > 294 || phi < 6 ) { continue; }
                s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {

                    s16 nx = p_x + 16 + ( ( (s32) j * x ) >> 12 );
                    s16 ny = p_y + 15 - ( ( (s32) j * y ) >> 12 );

                    if( j == p_outerR || j == p_innerR ) {
                        setPixel( nx, ny, p_sub, p_freecolor2 );
                    } else {
                        setPixel( nx, ny, p_sub, p_freecolor1 );
                    }

                    if( phi >= 75 ) {
                        BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
                        BG_PAL( p_sub )[ p_freecolor2 ] = YELLOW2;
                    }
                    if( phi >= 150 ) {
                        BG_PAL( p_sub )[ p_freecolor1 ] = GREEN;
                        BG_PAL( p_sub )[ p_freecolor2 ] = GREEN2;
                    }
                }
                if( p_delay && ( phi & 1 ) ) swiWaitForVBlank( );
            }
        } else if( p_HPstart > p_HP ) {
            // damage
            for( u16 phi = 3 * p_HPstart; phi > 3 * p_HP; phi-- ) {
                if( phi > 294 || phi < 6 ) { continue; }
                s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    s16 nx = p_x + 16 + ( ( (s32) j * x ) >> 12 );
                    s16 ny = p_y + 15 - ( ( (s32) j * y ) >> 12 );

                    setPixel( nx, ny, p_sub, 0 );
                    if( phi < 150 ) {
                        BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
                        BG_PAL( p_sub )[ p_freecolor2 ] = YELLOW2;
                    }
                    if( phi < 75 ) {
                        BG_PAL( p_sub )[ p_freecolor1 ] = RED;
                        BG_PAL( p_sub )[ p_freecolor2 ] = RED2;
                    }
                }
                if( p_delay && ( phi & 1 ) ) swiWaitForVBlank( );
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
                    s16 nx = p_x + 16 + ( ( (s32) j * x ) >> 12 );
                    s16 ny = p_y + 15 - ( ( (s32) j * y ) >> 12 );
                    if( nx == p_x + 16 + j ) --nx;
                    ( (color *) BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                        = ( ( (u8) p_freecolor1 ) << 8 ) | (u8) p_freecolor1;
                }
            }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = ICE_COLOR;
            for( u16 phi = 3 * p_EPstart; phi <= 3 * p_EP; ++phi ) {
                s16 x = isin( degreesToAngle( ( 210 + phi ) % 360 ) );
                s16 y = isin( degreesToAngle( ( 120 + phi ) % 360 ) );
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    s16 nx = p_x + 16 + ( ( (s32) j * x ) >> 12 );
                    s16 ny = p_y + 15 - ( ( (s32) j * y ) >> 12 );
                    if( nx == p_x + 16 + j ) --nx;
                    ( (color *) BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ]
                        = ( ( (u8) p_freecolor2 ) << 8 ) | (u8) p_freecolor2;
                }
                if( p_delay ) swiWaitForVBlank( );
            }
        }
    }

    std::string formatDate( SAVE::date p_date, u8 p_language ) {
        u8 tmp[ 3 ] = { p_date.m_day, p_date.m_month, u8( p_date.m_year % 100 ) };
        return formatDate( tmp, p_language );
    }

    std::string formatDate( SAVE::date p_date ) {
        return formatDate( p_date, CURRENT_LANGUAGE );
    }

    std::string formatDate( u8 p_date[ 3 ], u8 p_language ) {
        std::array<char, 20> buffer{ };
        switch( p_language ) {
        default:
        case 0: // EN
            // I know that this will "break" in 2100.
            snprintf( buffer.data( ), buffer.size( ), "%s %hhu, 20%02hhu",
                      FS::MONTHS[ p_date[ 1 ] ][ p_language ], p_date[ 0 ] + 1, p_date[ 2 ] % 100 );
            break;
        case 1: // DE
            snprintf( buffer.data( ), buffer.size( ), "%hhu. %s 20%02hhu", p_date[ 0 ] + 1,
                      FS::MONTHS[ p_date[ 1 ] ][ p_language ], p_date[ 2 ] );
            break;
        }
        return std::string( buffer.data( ) );
    }

    std::string formatDate( u8 p_date[ 3 ] ) {
        return formatDate( p_date, CURRENT_LANGUAGE );
    }
} // namespace IO
