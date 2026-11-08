/*
Pokémon neo
------------------------------

file        : font.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2025
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

#include "io/kana.h"

namespace IO {
    typedef u16 color;

    constexpr u8 FONT_WIDTH  = 16;
    constexpr u8 FONT_HEIGHT = 16;
    extern u8    TEXTSPEED;

    constexpr bool isNumeral( u16 p_val ) {
        return '0' <= p_val && p_val <= '9';
    }
    constexpr u16 valToNumeral( u16 p_val, u16 p_numStart ) {
        return ( p_val - '0' ) + p_numStart;
    }

    constexpr bool isUpper( u16 p_val ) {
        return 'A' <= p_val && p_val <= 'Z';
    }
    constexpr u16 valToUpper( u16 p_val, u16 p_upperStart ) {
        return ( p_val - 'A' ) + p_upperStart;
    }

    constexpr bool isLower( u16 p_val ) {
        return 'a' <= p_val && p_val <= 'z';
    }
    constexpr u16 valToLower( u16 p_val, u16 p_lowerStart ) {
        return ( p_val - 'a' ) + p_lowerStart;
    }

    constexpr u16  KANA_CODE_START        = 5000;
    constexpr u16  KANA_CODE_END          = KANA_CODE_START + KANA::NUM_DIGRAPHS - 1;
    constexpr u8   NUM_KANA_BRAILLE_CELLS = 64;
    constexpr bool isKana( u16 p_val ) {
        return KANA_CODE_START <= p_val && p_val <= KANA_CODE_END;
    }
    constexpr u16 valToKana( u16 p_val, u16 p_kanaStart ) {
        return ( p_val - KANA_CODE_START ) + p_kanaStart;
    }

    namespace REGULAR_FONT {
        constexpr auto NUM_CHARS          = 490;
        constexpr auto NUMERAL_START      = 0;
        constexpr auto UPPER_START        = 10;
        constexpr auto LOWER_START        = 36;
        constexpr auto KANA_START         = 195;
        constexpr auto HIRAGANA_START     = 195;
        constexpr auto KATAKANA_START     = 275;
        constexpr auto FULL_NUMERAL_START = 355;
        constexpr auto FULL_UPPER_START   = 365;
        constexpr auto FULL_LOWER_START   = 391;

        void      parseKanaDigraph( const char*, u32&, u16[ 2 ], u8& );
        void      shiftchar( u16& );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    } // namespace REGULAR_FONT
    namespace BOLD_FONT {
        constexpr auto NUM_CHARS          = 490;
        constexpr auto NUMERAL_START      = 0;
        constexpr auto UPPER_START        = 10;
        constexpr auto LOWER_START        = 36;
        constexpr auto KANA_START         = 195;
        constexpr auto HIRAGANA_START     = 195;
        constexpr auto KATAKANA_START     = 275;
        constexpr auto FULL_NUMERAL_START = 355;
        constexpr auto FULL_UPPER_START   = 365;
        constexpr auto FULL_LOWER_START   = 391;

        void      parseKanaDigraph( const char*, u32&, u16[ 2 ], u8& );
        void      shiftchar( u16& );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    } // namespace BOLD_FONT
    namespace SMALL_FONT {
        constexpr auto NUM_CHARS     = 150;
        constexpr auto NUMERAL_START = 0;
        constexpr auto UPPER_START   = 23;
        constexpr auto LOWER_START   = 23;

        void      shiftchar( u16& );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    } // namespace SMALL_FONT
    namespace BRAILLE_FONT {
        constexpr auto NUM_CHARS     = 30 + NUM_KANA_BRAILLE_CELLS;
        constexpr auto NUMERAL_START = NUM_CHARS;
        constexpr auto UPPER_START   = 0;
        constexpr auto LOWER_START   = 0;
        constexpr auto KANA_START    = 30;

        void      parseKanaDigraph( const char*, u32&, u16[ 2 ], u8& );
        void      shiftchar( u16& );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    } // namespace BRAILLE_FONT

    class font {
      private:
        u8* _data;
        u8* _widths;
        void ( *_shiftchar )( u16& );
        color _color[ 5 ];

        void _charDelay( s8 p_textSpeedModifier ) const;

        /*
         * @brief: Interprets p_string[ p_start ] p_string[ p_start + 1] as a kana digraph
         * and returns the corresponding up to two glyph ids to be printed.
         * Advances the position in the string by the num
         */
        void ( *_parseKanaDigraph )( const char*, u32&, u16[ 2 ], u8& );

      public:
        enum alignment { LEFT, RIGHT, CENTER };

        font( u8* p_fontData, u8* p_characterWidths, void ( *p_shiftchar )( u16& ),
              void ( *p_parseKanaDigraph )( const char*, u32&, u16[ 2 ], u8& ) );

        /*
         * @brief: Sets the p_num-th color.
         */
        constexpr void setColor( color p_newColor, int p_num ) {
            _color[ p_num ] = p_newColor;
        }

        /*
         * @brief: Returns the p_num-th color
         */
        constexpr color getColor( int p_num ) const {
            return _color[ p_num ];
        }

        /*
         * @brief: Returns the width in px that the given string has when using the font
         */
        u32 stringWidth( const char* p_string, u8 p_charShift = 0,
                         bool p_startInKanaMode = false ) const;
        /*
         * @brief: Returns the width in px that the given string has when using the
         * (compressed) font of the printStringC variants
         */
        u32 stringWidthC( const char* p_string ) const;
        /*
         * @brief: Returns the width in px that the given string has when using the font
         * or the length of the longest prefix ending before a p_breakChar that is shorter
         * than p_maxwidth
         */
        u32 stringMaxWidth( const char* p_string, u16 p_maxWidth, char p_breakChar,
                            u8 p_charShift = 0, bool p_startInKanaMode = false ) const;
        /*
         * @brief: Returns the width in px that the given string has when using the
         * (compressed) font of the printStringC variants
         * or the length of the longest prefix ending before a p_breakChar that is shorter
         * than p_maxwidth
         */
        u32 stringMaxWidthC( const char* p_string, u16 p_maxWidth, char p_breakChar ) const;

        // Methods to print single characters

        /*
         * @brief: Prints the given character at the given position.
         * @returns: the width (in px) of the printed char.
         */
        u16 printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom, u8 p_layer = 1,
                       bool p_shift = true ) const;

        /*
         * @brief: Prints the given character (as bitmap) in the given buffer.
         * @returns: the width (in px) of the printed char.
         */
        u16 printCharB( u16 p_ch, const u16* p_palette, u16* p_buffer, u16 p_bufferWidth,
                        s16 p_x = 0, s16 p_y = 0, bool p_shift = true ) const;

        /*
         * @brief: Draws the continue triangle for message boxes.
         */
        void drawContinue( u8 p_x, u8 p_y, bool p_bottom = true, u8 p_layer = 1 ) const;

        /*
         * @brief: Un-draws the continue triangle.
         */
        void hideContinue( u8 p_x, u8 p_y, u8 p_color = 250, bool p_bottom = true,
                           u8 p_layer = 1 ) const;

        // Methods to print compound objects

        /*
         * @brief: Prints a counter with the specified value.
         */
        void printCounter( u32 p_value, u8 p_digits, u16 p_x, u16 p_y, u8 p_highlightDigit,
                           u8 p_highlightBG, u8 p_highlightFG, bool p_bottom, u8 p_layer = 1 );

        // Methods to print strings

        /*
         * @brief: Prints the given string at the given position to the screen.
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printString( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                         alignment p_alignment = LEFT, u8 p_yDistance = 15, s8 p_adjustX = 0,
                         u8 p_charShift = 0, bool p_delay = false, u8 p_layer = 1,
                         s8 p_textSpeedModifier = 0 ) const;

        /*
         * @brief: Prints a string with less horizontal space between characters
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printStringC( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                          alignment p_alignment = LEFT, u8 p_yDistance = 15, s8 p_adjustX = 0,
                          bool p_delay = false, u8 p_layer = 1, s8 p_textSpeedModifier = 0 ) const;

        /*
         * @brief: Prints the given string with some delay after every character.
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printStringD( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                          alignment p_alignment = LEFT, u8 p_yDistance = 15, s8 p_adjustX = 0,
                          u8 p_charShift = 0, u8 p_layer = 1, s8 p_textSpeedModifier = 0 ) const;

        /*
         * @brief: Prints the given string to the given buffer.
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printStringB( const char* p_string, const u16* p_palette, u16* p_buffer,
                          u16 p_bufferWidth, alignment p_alignment = LEFT, u8 p_yDistance = 15,
                          u8 p_charShift = 0, u8 p_chunkSize = 64, u16 p_bufferHeight = 32,
                          u16 p_rightPadding = 0 ) const;
        /*
         * @brief: Prints a string in the given buffer with less horizontal space between characters
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printStringBC( const char* p_string, const u16* p_palette, u16* p_buffer,
                           u16 p_bufferWidth, alignment p_alignment = LEFT, u8 p_yDistance = 15,
                           u8 p_chunkSize = 64, u16 p_bufferHeight = 32,
                           u16 p_rightPadding = 0 ) const;

        /*
         * @brief: Prints the given string, where newlines are inserted whenever the
         * current line exceeds the given p_maxWidth.
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printBreakingString( const char* p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                 bool p_bottom, alignment p_alignment = LEFT, u8 p_yDistance = 16,
                                 char p_breakChar = ' ', s8 p_adjustX = 0, u8 p_charShift = 0,
                                 bool p_delay = false, u8 p_layer = 1,
                                 s8 p_textSpeedModifier = 0 ) const;
        /*
         * @brief: Prints the given string, where newlines are inserted whenever the
         * current line exceeds the given p_maxWidth. Uses less horizontal space for each
         * character.
         * @returns: number of lines written (i.e. 1 + number of newlines or other breaks)
         */
        u16 printBreakingStringC( const char* p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                  bool p_bottom, alignment p_alignment = LEFT, u8 p_yDistance = 16,
                                  char p_breakChar = ' ', s8 p_adjustX = 0, bool p_delay = false,
                                  u8 p_layer = 1, s8 p_textSpeedModifier = 0 ) const;

        /*
         * @brief: Prints a string until p_maxX is reached, writes p_breakChar if the
         * limit is hit
         */
        void printMaxString( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                             s16 p_maxX = 256, u16 p_breakChar = L'.', u8 p_charShift = 0,
                             bool p_delay = false, u8 p_layer = 1,
                             s8 p_textSpeedModifier = 0 ) const;
        /*
         * @brief: Prints a string with less horizontal space between characters;
         * prints p_breakChar once p_maxX is reached.
         */
        void printMaxStringC( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                              s16 p_maxX = 256, u16 p_breakChar = L'.', bool p_delay = false,
                              u8 p_layer = 1, s8 p_textSpeedModifier = 0 ) const;
    };

    extern font* regularFont;
    extern font* boldFont;
    extern font* smallFont;
    extern font* brailleFont;
} // namespace IO
