/*
Pokémon neo
------------------------------

file        : fontUtil.cpp
author      : Philip Wellnitz
description : helper functions to draw and set pixels on-screen

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

#include <algorithm>
#include <nds.h>

#include "io/font.h"
#include "io/fontUtil.h"

namespace IO {
    void initColors( bool p_sub, bool p_top, bool p_font ) {
        if( p_font ) {
            regularFont->setColor( 0, 0 );
            regularFont->setColor( IO::BLACK_IDX, 1 );
            regularFont->setColor( IO::GRAY_IDX, 2 );
            boldFont->setColor( 0, 0 );
            boldFont->setColor( IO::GRAY_IDX, 1 );
            boldFont->setColor( IO::WHITE_IDX, 2 );
        }
        if( p_sub ) {
            BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
            BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
            BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
            BG_PALETTE_SUB[ IO::RED_IDX ]   = IO::RED;
            BG_PALETTE_SUB[ IO::BLUE_IDX ]  = IO::BLUE;
        }
        if( p_top ) {
            BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
            BG_PALETTE[ IO::GRAY_IDX ]  = IO::GRAY;
            BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
            BG_PALETTE[ IO::RED_IDX ]   = IO::RED;
            BG_PALETTE[ IO::BLUE_IDX ]  = IO::BLUE;
        }
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

    void drawLine( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, u8 p_color, u8 p_layer ) {
        if( p_x1 == p_x2 ) { // vertical line
            if( p_y2 < p_y1 ) { std::swap( p_y1, p_y2 ); }
            for( u16 y = p_y1; y <= p_y2; ++y ) { setPixel( p_x1, y, p_bottom, p_color, p_layer ); }
            return;
        }
        if( p_y1 == p_y2 ) { // horizontal line
            if( p_x2 < p_x1 ) { std::swap( p_x1, p_x2 ); }
            for( u16 x = p_x1; x <= p_x2; ++x ) { setPixel( x, p_y1, p_bottom, p_color, p_layer ); }
            return;
        }

        if( p_x2 < p_x1 ) {
            std::swap( p_x1, p_x2 );
            std::swap( p_y1, p_y2 );
        }

        auto dx = p_x2 - p_x1;
        auto dy = -std::abs( p_y2 - p_y1 );
        auto sy = p_y2 < p_y1 ? -1 : 1;
        auto D  = dx + dy;
        auto y  = p_y1;
        auto x  = p_x1;

        while( true ) {
            setPixel( x, y, p_bottom, p_color, p_layer );
            if( x == p_x2 && y == p_y2 ) { break; }
            auto D2 = 2 * D;
            if( D2 >= dy ) {
                if( x == p_x2 ) { break; }
                D += dy;
                x++;
            }
            if( D2 <= dx ) {
                if( y == p_y2 ) { break; }
                D += dx;
                y += sy;
            }
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
} // namespace IO
