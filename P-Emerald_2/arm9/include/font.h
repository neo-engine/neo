/*
Pokémon neo
------------------------------

file        : font.h
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
#include <nds.h>

namespace IO {
    typedef u16 color;

    namespace REGULAR_FONT {
        constexpr auto NUM_CHARS = 490;
        void           shiftchar( u16 &val );
        extern u8      fontWidths[ NUM_CHARS ];
        extern u8      fontData[ NUM_CHARS * 256 ];
    } // namespace REGULAR_FONT
    namespace BOLD_FONT {
        constexpr auto NUM_CHARS = 490;
        void           shiftchar( u16 &val );
        extern u8      fontWidths[ NUM_CHARS ];
        extern u8      fontData[ NUM_CHARS * 256 ];
    } // namespace BOLD_FONT
    namespace SMALL_FONT {
        constexpr auto NUM_CHARS = 150;
        void           shiftchar( u16 &val );
        extern u8      fontWidths[ NUM_CHARS ];
        extern u8      fontData[ NUM_CHARS * 256 ];
    } // namespace SMALL_FONT

    class font {
      public:
        // Constructors
        font( u8 *p_fontData, u8 *p_characterWidths, void ( *p_shiftchar )( u16 &val ) );

        /*
         * @brief: Sets the p_num-th color.
         */
        void setColor( color p_newColor, int p_num ) {
            _color[ p_num ] = p_newColor;
        }

        /*
         * @brief: Returns the p_num-th color
         */
        color getColor( int p_num ) const {
            return _color[ p_num ];
        }

        enum alignment { LEFT, RIGHT, CENTER };

        // Methods

        /*
         * @brief: Draws the continue triangle for message boxes.
         */
        void drawContinue( u8 p_x, u8 p_y, bool p_bottom = true, u8 p_layer = 1 ) const;

        /*
         * @brief: Un-draws the continue triangle.
         */
        void hideContinue( u8 p_x, u8 p_y, u8 p_color = 250, bool p_bottom = true,
                           u8 p_layer = 1 ) const;
        /*
         * @brief: Prints the given character (as bitmap) in the given buffer.
         */
        void printCharB( u16 p_ch, u16* p_palette, u16* p_buffer, u16 p_bufferWidth, s16 p_x = 0,
                         s16 p_y = 0 ) const;

        /*
         * @brief: Prints the given character at the given position.
         */
        void printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom, u8 p_layer = 1 ) const;

        /*
         * @brief: Prints the given string, where newlines are inserted whenever the
         * current line exceeds the given p_maxWidth.
         */
        void printBreakingString( const char *p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                  bool p_bottom, alignment p_alignment = LEFT, u8 p_yDistance = 16,
                                  char p_breakChar = ' ', s8 p_adjustX = 0, u8 p_layer = 1 ) const;
        /*
         * @brief: Prints the given string, where newlines are inserted whenever the
         * current line exceeds the given p_maxWidth. Uses less horizontal space for each
         * character.
         */
        void printBreakingStringC( const char *p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                   bool p_bottom, alignment p_alignment = LEFT, u8 p_yDistance = 16,
                                   char p_breakChar = ' ', s8 p_adjustX = 0, u8 p_layer = 1 ) const;

        /*
         * @brief: Prints a string in the given buffer with less horizontal space between characters
         */
        void printStringBC( const char *p_string, u16* p_palette, u16* p_buffer, u16 p_bufferWidth,
                            alignment p_alignment = LEFT, u8 p_yDistance = 15 ) const;
        /*
         * @brief: Prints the given string to the given buffer.
         */
        void printStringB( const char *p_string, u16* p_palette, u16* p_buffer, u16 p_bufferWidth,
                           alignment p_alignment = LEFT, u8 p_yDistance = 15 ) const;

        /*
         * @brief: Prints a string with less horizontal space between characters
         */
        void printStringC( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                           alignment p_alignment = LEFT, u8 p_yDistance = 15, s8 p_adjustX = 0,
                           u8 p_layer = 1 ) const;
        /*
         * @brief: Prints the given string at the given position to the screen.
         */
        void printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                          alignment p_alignment = LEFT, u8 p_yDistance = 15, s8 p_adjustX = 0,
                          u8 p_layer = 1 ) const;
        /*
         * @brief: Prints a string until p_maxX is reached, writes p_breakChar if the
         * limit is hit
         */
        void printMaxString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                             s16 p_maxX = 256, u16 p_breakChar = L'.', u8 p_layer = 1 ) const;
        /*
         * @brief: Prints a string with less horizontal space between characters;
         * prints p_breakChar once p_maxX is reached.
         */
        void printMaxStringC( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                             s16 p_maxX = 256, u16 p_breakChar = L'.', u8 p_layer = 1 ) const;
        /*
         * @brief: Prints the given string with some delay after every character.
         */
        void printStringD( const char *p_string, s16 &p_x, s16 &p_y, bool p_bottom,
                           u8 p_layer = 1 ) const;

        /*
         * @brief: Prints the given string; pauses and waits for KEY_A or KEY_B when a "`" is
         * encountered in the given string.
         */
        void printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                            u8 p_layer = 1 ) const;
        /*
         * @brief: Prints the given string; pauses and waits for KEY_A or KEY_B when a "`" is
         * encountered in the given string. Has additional delay after every character.
         */
        void printMBStringD( const char *p_string, s16 &p_x, s16 &p_y, bool p_bottom,
                             u8 p_layer = 1 ) const;

        /*
         * @brief: Returns the width in px that the given string has when using the font
         */
        u32 stringWidth( const char *p_string ) const;
        /*
         * @brief: Returns the width in px that the given string has when using the
         * (compressed) font of the printStringC variants
         */
        u32 stringWidthC( const char *p_string ) const;
        /*
         * @brief: Returns the width in px that the given string has when using the font
         * or the length of the longest prefix ending before a p_breakChar that is shorter
         * than p_maxwidth
         */
        u32 stringMaxWidth( const char *p_string, u16 p_maxWidth, char p_breakChar ) const;
        /*
         * @brief: Returns the width in px that the given string has when using the
         * (compressed) font of the printStringC variants
         * or the length of the longest prefix ending before a p_breakChar that is shorter
         * than p_maxwidth
         */
        u32 stringMaxWidthC( const char *p_string, u16 p_maxWidth, char p_breakChar ) const;

        /*
         * @brief: Prints a counter with the specified value.
         */
        void printCounter( u32 p_value, u8 p_digits, u16 p_x, u16 p_y, u8 p_highlightDigit,
                           u8 p_highlightBG, u8 p_highlightFG, bool p_bottom, u8 p_layer = 1 );

      private:
        u8 *_data;
        u8 *_widths;
        void ( *_shiftchar )( u16 &val );
        color _color[ 5 ];
    };
} // namespace IO
