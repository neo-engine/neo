/*
Pokémon Emerald 2 Version
------------------------------

file        : font.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#pragma once
#include <nds.h>

namespace IO {
#define NUMFONTS 2

    typedef u16 color;

    namespace REGULAR_FONT {
#define NUM_CHARS 490
        void shiftchar( u16& val );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    }
    namespace BOLD_FONT {
#define NUM_CHARS 490
        void shiftchar( u16& val );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    }

    class font {
    public:
        // Constructors
        font( u8 *p_fontData, u8 *p_characterWidths, void( *p_shiftchar )( u16& val ) );

        // Accessors
        void setColor( color p_newColor, int p_num ) {
            _color[ p_num ] = p_newColor;
        }
        color getColor( int p_num ) const {
            return _color[ p_num ];
        }

        // Methods
        void printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom );
        void printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16, s8 p_adjustX = 0 );
        void printMaxString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, s16 p_maxX = 256, u16 p_breakChar = L'.' );
        void printStringCenter( const char *p_string, bool p_bottom );
        void printStringD( const char *p_string, s16& p_x, s16& p_y, bool p_bottom );
        void printStringCenterD( const char *p_string, bool p_bottom );
        void printNumber( s32 p_num, s16 p_x, s16 p_y, bool p_bottom );

        void printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void printMBStringD( const char *p_string, s16& p_x, s16& p_y, bool p_bottom );

        u32 stringWidth( const char *p_string ) const;
    private:
        u8 *_data;
        u8 *_widths;
        void( *_shiftchar )( u16& val );
        color _color[ 5 ];
    };
}