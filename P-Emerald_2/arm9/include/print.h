/*
Pokémon Emerald 2 Version
------------------------------

file        : print.h
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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


#pragma once
#include <nds.h>
#include "defines.h"

typedef u16 color;
namespace FONT {
#define NUMFONTS 2
    const u8 SCREEN_TOP = 1;
    const u8 SCREEN_BOTTOM = 0;
    const u8 FONT_WIDTH = 16;
    const u8 FONT_HEIGHT = 16;

    void putrec( u8 p_x1,
                 u8 p_y1,
                 u8 p_x2,
                 u8 p_y2,
                 bool p_bottom = true,
                 bool p_striped = false,
                 u8 p_color = 42 );

    class font {
    public:
        // Constructors
        font( u8 *p_fontData, u8 *p_characterWidths, void( *p_shiftchar )( u16& val ) );
        ~font( );

        // Accessors
        void setColor( color p_newColor, int p_num ) {
            _color[ p_num ] = p_newColor;
        }
        color getColor( int p_num ) const {
            return _color[ p_num ];
        }

        // Methods
        void printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom );
        void printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16 );
        void printStringCenter( const char *p_string, bool p_bottom );
        void printStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void printStringCenterD( const char *p_string, bool p_bottom );
        void printNumber( s32 p_num, s16 p_x, s16 p_y, bool p_bottom );
        u32 stringWidth( const char *p_string ) const;

        void printString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16 );
        void printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, u8 p_updateTimePar = 0 );
        void printMBString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, u8 p_updateTimePar = 0 );
        void printMBStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, u8 p_updateTimePar = 0 );
        void printMBStringD( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, u8 p_updateTimePar = 0 );
        void printStringCenter( const wchar_t *p_string, bool p_bottom );
        void printStringD( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void printStringCenterD( const wchar_t *p_string, bool p_bottom );
        u32 stringWidth( const wchar_t *p_string ) const;

    private:
        u8 *_data;
        u8 *_widths;
        void( *_shiftchar )( u16& val );
        color _color[ 5 ];
    };

    extern u8 ASpriteOamIndex;
}

void topScreenDarken( );
void btmScreenDarken( );
void topScreenPlot( u8 p_x, u8 p_y, color p_color );
void btmScreenPlot( u8 p_x, u8 p_y, color p_color );