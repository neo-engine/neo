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
namespace font {
#define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))
#define NUMFONTS 2
    const u8 SCREEN_TOP = 1;
    const u8 SCREEN_BOTTOM = 0;
    const u8 FONT_WIDTH = 16;
    const u8 FONT_HEIGHT = 16;

    void putrec( int p_x1,
                 int p_y1,
                 int p_x2,
                 int p_y2,
                 bool p_bottom = true,
                 bool p_striped = false,
                 int p_color = 42 );

    class Font {
    public:
        // Constructors
        Font( u8 *p_fontData, u8 *p_characterWidths, void( *p_shiftchar )( u16& val ) );
        ~Font( );

        // Accessors
        void setColor( color p_newColor, int p_num ) {
            _color[ p_num ] = p_newColor;
        }
        color getColor( int p_num ) const {
            return _color[ p_num ];
        }

        // Methods
        void print_char( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom );
        void print_string( const char *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void print_string_center( const char *p_string, bool p_bottom );
        void print_string_d( const char *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void print_string_center_d( const char *p_string, bool p_bottom );
        void print_number( s32 p_num, s16 p_x, s16 p_y, bool p_bottom );
        u32 string_width( const char *p_string ) const;

        void print_string( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void print_string_center( const wchar_t *p_string, bool p_bottom );
        void print_string_d( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom );
        void print_string_center_d( const wchar_t *p_string, bool p_bottom );
        u32 string_width( const wchar_t *p_string ) const;

    private:
        u8 *_data;
        u8 *_widths;
        void( *_shiftchar )( u16& val );
        color _color[ 5 ];
    };
}

void top_screen_darken( );
void btm_screen_darken( );
void top_screen_plot( u8 p_x, u8 p_y, color p_color );
void btm_screen_plot( u8 p_x, u8 p_y, color p_color );