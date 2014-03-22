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

typedef u16 Color;
namespace font{
#define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))
#define NUMFONTS 2
    const u8 SCREEN_TOP = 1;
    const u8 SCREEN_BOTTOM = 0;
    const u8 FONT_WIDTH = 16;
    const u8 FONT_HEIGHT = 16;

    void putrec(int x1,int y1,int x2,int y2, bool bottom = true, bool striped = false,int color = 42);

    class Font{
    public:
        // Constructors
        Font(u8 *the_data, u8 *the_widths, void (*shiftchar)(u16& val));
        ~Font();

        // Accessors
        void set_color(Color new_color,int num) { color[num] = new_color; }
        Color get_color(int num) const { return color[num]; }

        // Methods
        void print_char(u16 ch, s16 x, s16 y,bool bottom);
        void print_string(const char *string, s16 x, s16 y,bool bottom);
        void print_string_center(const char *string,bool bottom);
        void print_string_d(const char *string, s16 x, s16 y,bool bottom);
        void print_string_center_d(const char *string,bool bottom);
        void print_number(s32 num, s16 x, s16 y,bool bottom);
        u32 string_width(const char *string) const;

        void print_string(const wchar_t *string, s16 x, s16 y,bool bottom);
        void print_string_center(const wchar_t *string,bool bottom);
        void print_string_d(const wchar_t *string, s16 x, s16 y,bool bottom);
        void print_string_center_d(const wchar_t *string,bool bottom);
        u32 string_width(const wchar_t *string) const;

    private:
        u8 *data;
        u8 *widths;
        void (*shiftchar)(u16& val);
        Color color[5];
    };
}

void top_screen_darken();
void btm_screen_darken();
void top_screen_plot(u8 x, u8 y, Color color);
void btm_screen_plot(u8 x, u8 y, Color color);