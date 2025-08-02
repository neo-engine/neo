/*
Pokémon neo
------------------------------

file        : fontUtil.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2025
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
#include <nds.h>

namespace IO {
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
    constexpr u16 GRAY          = RGB( 20, 20, 20 );
    constexpr u16 NORMAL_COLOR  = RGB( 27, 27, 27 );
    constexpr u16 BLACK         = RGB( 2, 2, 4 );
    constexpr u16 BLACK2        = RGB( 5, 5, 5 );
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

    void initColors( bool p_sub = true, bool p_top = true, bool p_font = true );

    void setPixel( u8 p_x, u8 p_y, bool p_bottom, u8 p_color, u8 p_layer = 1 );

    void printRectangle( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, u8 p_color,
                         u8 p_layer = 1 );

    void drawLine( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y5, bool p_bottom, u8 p_color, u8 p_layer = 1 );
} // namespace IO
