/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : print.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */

#pragma once
#include <nds.h>

typedef u16 color;
namespace FONT {
#define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))
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
        void printMBString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom );
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
}

void topScreenDarken( );
void btmScreenDarken( );
void topScreenPlot( u8 p_x, u8 p_y, color p_color );
void btmScreenPlot( u8 p_x, u8 p_y, color p_color );