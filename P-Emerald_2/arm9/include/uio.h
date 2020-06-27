/*
Pokémon neo
------------------------------

file        : uio.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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
#pragma once
#include <string>
#include <nds.h>

#include "font.h"
#include "nav.h"
#include "sprite.h"

namespace IO {

#define TOUCH_UP ( !touch.px && !touch.py )
#define GET_KEY_COOLDOWN( p_key ) \
    ( ( pressed & ( p_key ) ) || ( ( held & ( p_key ) ) && !( --cooldown ) ) )
#define GET_AND_WAIT( p_key ) \
    ( ( pressed & ( p_key ) ) && IO::waitForInput( IO::inputTarget( p_key ) ) )

    constexpr u16 RGB( u8 p_r, u8 p_g, u8 p_b ) {
        return RGB15( p_r, p_g, p_b ) | BIT( 15 );
    }

    constexpr u16 COMPL( u16 p_color ) {
        return RGB( 31 - ( p_color >> 10 ) % 32, 31 - ( p_color >> 5 ) % 32, 31 - p_color % 32 );
    }

    constexpr u8 RED2_IDX  = 247;
    constexpr u8 BLUE2_IDX = 248;
    constexpr u8 COLOR_IDX = 249;
    constexpr u8 WHITE_IDX = 250;
    constexpr u8 GRAY_IDX  = 251;
    constexpr u8 BLACK_IDX = 252;
    constexpr u8 RED_IDX   = 253;
    constexpr u8 BLUE_IDX  = 254;

    constexpr u16 CHOICE_COLOR = RGB( 16, 25, 19 );

    constexpr u16 GREEN         = RGB( 12, 30, 12 );
    constexpr u16 GREEN2        = RGB( 3, 23, 4 );
    constexpr u16 RED           = RGB( 30, 15, 12 );
    constexpr u16 RED2          = RGB( 20, 7, 7 );
    constexpr u16 BLUE          = RGB( 0, 10, 31 );
    constexpr u16 BLUE2         = RGB( 0, 0, 15 );
    constexpr u16 WHITE         = RGB( 30, 30, 30 );
    constexpr u16 GRAY          = RGB( 15, 15, 15 );
    constexpr u16 NORMAL_COLOR  = RGB( 27, 27, 27 );
    constexpr u16 BLACK         = RGB( 0, 0, 0 );
    constexpr u16 YELLOW        = RGB( 30, 30, 12 );
    constexpr u16 YELLOW2       = RGB( 23, 23, 5 );
    constexpr u16 PURPLE        = RGB( 24, 0, 24 );
    constexpr u16 TURQOISE      = RGB( 0, 24, 24 );
    constexpr u16 ICE_COLOR     = RGB( 15, 31, 31 );
    constexpr u16 FAIRY_COLOR   = RGB( 31, 15, 31 );
    constexpr u16 GROUND_COLOR  = RGB( 31, 31, 15 );
    constexpr u16 POISON_COLOR  = RGB( 31, 0, 15 );
    constexpr u16 ORANGE        = RGB( 31, 15, 0 );
    constexpr u16 GHOST_COLOR   = RGB( 15, 0, 31 );
    constexpr u16 ROCK_COLOR    = RGB( 28, 23, 7 );
    constexpr u16 BUG_COLOR     = RGB( 15, 28, 7 );
    constexpr u16 STEEL_COLOR   = RGB( 24, 24, 24 );
    constexpr u16 DRAGON_COLOR  = RGB( 7, 7, 24 );
    constexpr u16 UNKNOWN_COLOR = RGB( 0, 42, 42 );

    inline u16* BG_PAL( bool p_bottom ) {
        return p_bottom ? BG_PALETTE_SUB : BG_PALETTE;
    }
    inline u16* BG_BMP( bool p_bottom ) {
        return p_bottom ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 );
    }

    constexpr u8 SCREEN_TOP    = 1;
    constexpr u8 SCREEN_BOTTOM = 0;
    constexpr u8 FONT_WIDTH    = 16;
    constexpr u8 FONT_HEIGHT   = 16;

    extern font*        regularFont;
    extern font*        boldFont;
    extern font*        smallFont;
    extern ConsoleFont* consoleFont;

    extern u8 ASpriteOamIndex;

    extern OAMTable*  Oam;
    extern SpriteInfo spriteInfo[ SPRITE_COUNT ];

    extern OAMTable*  OamTop;
    extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    extern u8 TEXTSPEED;

    extern int          bg2;
    extern int          bg3;
    extern int          bg2sub;
    extern int          bg3sub;
    extern PrintConsole Bottom;
    extern PrintConsole Top;

    void initVideo( bool p_noFade = false );
    void initVideoSub( bool p_noFade = false );
    void vramSetup( bool p_noFade = false );

    void swapScreens( );
    void animateBG( u8 p_frame, int p_bg );

    void setDefaultConsoleTextColors( u16* p_palette, u8 p_start = 1 );

    struct inputTarget {
        enum inputType { BUTTON, TOUCH, TOUCH_CIRCLE };
        inputType   m_inputType;
        KEYPAD_BITS m_keys;
        u16         m_targetX1 = 0;
        u16         m_targetY1 = 0;
        u16         m_targetX2 = 300;
        u16         m_targetY2 = 300;
        u16         m_targetR  = 16;

        inputTarget( KEYPAD_BITS p_keys ) : m_inputType( BUTTON ), m_keys( p_keys ) {
        }
        inputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 )
            : m_inputType( TOUCH ), m_targetX1( p_targetX1 ), m_targetY1( p_targetY1 ),
              m_targetX2( p_targetX2 ), m_targetY2( p_targetY2 ) {
        }
        inputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetR )
            : m_inputType( TOUCH_CIRCLE ), m_targetX1( p_targetX1 ), m_targetY1( p_targetY1 ),
              m_targetR( p_targetR ) {
        }

        constexpr bool inRange( touchPosition& p_touch ) const {
            if( m_inputType == TOUCH ) {
                return p_touch.px >= m_targetX1 && p_touch.py >= m_targetY1
                    && p_touch.px <= m_targetX2 && p_touch.py <= m_targetY2;
            } else if( m_inputType == TOUCH_CIRCLE ) {
                return sq( p_touch.px - m_targetX1 ) + sq( p_touch.py - m_targetY1 )
                    <= sq( m_targetR );
            } else {
                return false;
            }
        }
    };

    bool waitForTouchUp( u16 p_targetX1 = 0, u16 p_targetY1 = 0, u16 p_targetX2 = 300,
                         u16 p_targetY2 = 300 );
    bool waitForTouchUp( inputTarget p_inputTarget );

    bool waitForKeysUp( KEYPAD_BITS p_keys );
    bool waitForKeysUp( inputTarget p_inputTarget );

    bool waitForInput( inputTarget p_inputTarget );

    void initTextField( );

    void setPixel( u8 p_x, u8 p_y, bool p_bottom, u8 p_color, u8 p_layer = 1 );

    void printRectangle( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, u8 p_color,
                         u8 p_layer = 1 );

    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, bool p_big = false ); // HP in %
    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub = false ); // HP in %
    void displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR = 14, u8 p_outerR = 15, bool p_sub = false );

    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_colorIdx,
                         bool p_pressed, bool p_bottom = true );
    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_borderWidth2,
                         u8 p_colorIdx, bool p_pressed, bool p_bottom = true );

    std::string formatDate( u8 p_date[ 3 ] );
    std::string formatDate( u8 p_date[ 3 ], u8 p_language );

    constexpr u16 getColor( type p_type ) {
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
