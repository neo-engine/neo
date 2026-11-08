/*
Pokémon neo
------------------------------

file        : kana.h
author      : Philip Wellnitz
description : Utilities for parsing/rendering kana text.

Copyright (C) 2026
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

namespace IO::KANA {
    constexpr char ON  = 0x0E; // KANA-MODE-ON
    constexpr char OFF = 0x0F; // KANA-MODE-OFF

    constexpr bool isModeMarker( char p_c ) {
        return p_c == ON || p_c == OFF;
    }

    constexpr u16 NUM_DIGRAPHS = 286;

    /*
     * @brief: Embeds the given string in kana on/off markers.
     * @param p_kanaOnInitially: true: ON p_s; false: OFF p_s.
     * @param p_kanaOffFinally: true: p_s OFF; false: p_s ON.
     */
    const char* embedKana( const char* p_s, bool p_kanaOnInitially, bool p_kanaOffFinally,
                           char* p_buf, uint32_t p_bufSize );

    char devoice( char p_char );
    char voice( char p_char, char p_half = false );
    bool hasVariant( char p_c1, char p_c2, bool p_half );
    bool toggleVoicedness( char& p_c1, char p_c2, bool p_half );

    // Three different encodings of kana.
    // - as ASCII digraphs (used to store kana strings on disk)
    // - as index in the glyph order used by regular and bold fonts. (1 to ~195)
    // - as intermediate transport: KANA_CODE_START + index in glyph order (used by the fonts
    //   to identify kana characters; gets mapped by shiftChar to the actual index in the
    //   glyph table. KANA_CODE_START + x -> *_FONT::KANA_START + x
    //
    // IO::KANA contains helpers to parse ASCII digraphs into indices in the glyph order.
    // (KANA Braille has a different glyph order.)

    // id -> { digraph hi, lo };
    constexpr char GLYPH_ORDER[ NUM_DIGRAPHS ][ 2 ]
        = { { 'a', 'A' }, { 'a', 'a' }, { 'i', 'I' }, { 'i', 'i' }, { 'u', 'U' }, { 'u', 'u' },
            { 'e', 'E' }, { 'e', 'e' }, { 'o', 'O' }, { 'o', 'o' }, { 'k', 'a' }, { 'g', 'a' },
            { 'k', 'i' }, { 'g', 'i' }, { 'k', 'u' }, { 'g', 'u' }, { 'k', 'e' }, { 'g', 'e' },
            { 'k', 'o' }, { 'g', 'o' }, { 's', 'a' }, { 'z', 'a' }, { 's', 'i' }, { 'z', 'i' },
            { 's', 'u' }, { 'z', 'u' }, { 's', 'e' }, { 'z', 'e' }, { 's', 'o' }, { 'z', 'o' },
            { 't', 'a' }, { 'd', 'a' }, { 't', 'i' }, { 'd', 'i' }, { 't', 'U' }, { 't', 'u' },
            { 'd', 'u' }, { 't', 'e' }, { 'd', 'e' }, { 't', 'o' }, { 'd', 'o' }, { 'n', 'a' },
            { 'n', 'i' }, { 'n', 'u' }, { 'n', 'e' }, { 'n', 'o' }, { 'h', 'a' }, { 'b', 'a' },
            { 'p', 'a' }, { 'h', 'i' }, { 'b', 'i' }, { 'p', 'i' }, { 'h', 'u' }, { 'b', 'u' },
            { 'p', 'u' }, { 'h', 'e' }, { 'b', 'e' }, { 'p', 'e' }, { 'h', 'o' }, { 'b', 'o' },
            { 'p', 'o' }, { 'm', 'a' }, { 'm', 'i' }, { 'm', 'u' }, { 'm', 'e' }, { 'm', 'o' },
            { 'y', 'A' }, { 'y', 'a' }, { 'y', 'U' }, { 'y', 'u' }, { 'y', 'O' }, { 'y', 'o' },
            { 'r', 'a' }, { 'r', 'i' }, { 'r', 'u' }, { 'r', 'e' }, { 'r', 'o' }, { 'w', 'a' },
            { 'w', 'o' }, { 'n', 'n' }, { 'A', 'a' }, { 'A', 'A' }, { 'I', 'i' }, { 'I', 'I' },
            { 'U', 'u' }, { 'U', 'U' }, { 'E', 'e' }, { 'E', 'E' }, { 'O', 'o' }, { 'O', 'O' },
            { 'K', 'A' }, { 'G', 'A' }, { 'K', 'I' }, { 'G', 'I' }, { 'K', 'U' }, { 'G', 'U' },
            { 'K', 'E' }, { 'G', 'E' }, { 'K', 'O' }, { 'G', 'O' }, { 'S', 'A' }, { 'Z', 'A' },
            { 'S', 'I' }, { 'Z', 'I' }, { 'S', 'U' }, { 'Z', 'U' }, { 'S', 'E' }, { 'Z', 'E' },
            { 'S', 'O' }, { 'Z', 'O' }, { 'T', 'A' }, { 'D', 'A' }, { 'T', 'I' }, { 'D', 'I' },
            { 'T', 'u' }, { 'T', 'U' }, { 'D', 'U' }, { 'T', 'E' }, { 'D', 'E' }, { 'T', 'O' },
            { 'D', 'O' }, { 'N', 'A' }, { 'N', 'I' }, { 'N', 'U' }, { 'N', 'E' }, { 'N', 'O' },
            { 'H', 'A' }, { 'B', 'A' }, { 'P', 'A' }, { 'H', 'I' }, { 'B', 'I' }, { 'P', 'I' },
            { 'H', 'U' }, { 'B', 'U' }, { 'P', 'U' }, { 'H', 'E' }, { 'B', 'E' }, { 'P', 'E' },
            { 'H', 'O' }, { 'B', 'O' }, { 'P', 'O' }, { 'M', 'A' }, { 'M', 'I' }, { 'M', 'U' },
            { 'M', 'E' }, { 'M', 'O' }, { 'Y', 'a' }, { 'Y', 'A' }, { 'Y', 'u' }, { 'Y', 'U' },
            { 'Y', 'o' }, { 'Y', 'O' }, { 'R', 'A' }, { 'R', 'I' }, { 'R', 'U' }, { 'R', 'E' },
            { 'R', 'O' }, { 'W', 'A' }, { 'W', 'O' }, { 'N', 'N' }, { 'x', '0' }, { 'x', '1' },
            { 'x', '2' }, { 'x', '3' }, { 'x', '4' }, { 'x', '5' }, { 'x', '6' }, { 'x', '7' },
            { 'x', '8' }, { 'x', '9' }, { 'x', 'A' }, { 'x', 'B' }, { 'x', 'C' }, { 'x', 'D' },
            { 'x', 'E' }, { 'x', 'F' }, { 'x', 'G' }, { 'x', 'H' }, { 'x', 'I' }, { 'x', 'J' },
            { 'x', 'K' }, { 'x', 'L' }, { 'x', 'M' }, { 'x', 'N' }, { 'x', 'O' }, { 'x', 'P' },
            { 'x', 'Q' }, { 'x', 'R' }, { 'x', 'S' }, { 'x', 'T' }, { 'x', 'U' }, { 'x', 'V' },
            { 'x', 'W' }, { 'x', 'X' }, { 'x', 'Y' }, { 'x', 'Z' }, { 'x', 'a' }, { 'x', 'b' },
            { 'x', 'c' }, { 'x', 'd' }, { 'x', 'e' }, { 'x', 'f' }, { 'x', 'g' }, { 'x', 'h' },
            { 'x', 'i' }, { 'x', 'j' }, { 'x', 'k' }, { 'x', 'l' }, { 'x', 'm' }, { 'x', 'n' },
            { 'x', 'o' }, { 'x', 'p' }, { 'x', 'q' }, { 'x', 'r' }, { 'x', 's' }, { 'x', 't' },
            { 'x', 'u' }, { 'x', 'v' }, { 'x', 'w' }, { 'x', 'x' }, { 'x', 'y' }, { 'x', 'z' },
            { 'x', '!' }, { 'x', '?' }, { 'x', ',' }, { 'x', '.' }, { 'x', '|' }, { 'x', '&' },
            { 'x', '/' }, { 'x', '[' }, { 'x', ']' }, { 'x', '{' }, { 'x', '}' }, { 'x', '(' },
            { 'x', ')' }, { 'q', 'm' }, { 'q', 'f' }, { 'x', '+' }, { 'x', '-' }, { 'x', '*' },
            { 'x', '%' }, { 'x', '=' }, { 'x', '~' }, { 'x', ':' }, { 'x', ';' }, { 'X', '.' },
            { 'X', ',' }, { 'q', '1' }, { 'q', '2' }, { 'q', '3' }, { 'q', '4' }, { 'q', '5' },
            { 'q', '6' }, { 'q', '7' }, { 'q', '8' }, { 'q', '9' }, { 'q', '0' }, { 0, 0 },
            { 'Q', '1' }, { 'Q', '2' }, { 'Q', '3' }, { 'Q', '4' }, { 'Q', '5' }, { 'Q', '6' },
            { 'Q', '7' }, { 'Q', '8' }, { 'Q', '9' }, { 'Q', '0' }, { 'Q', 'w' }, { 'Q', 's' },
            { 'q', 'z' }, { 'x', '$' }, { 0, 0 },     { 0, 0 },     { 0, 0 },     { 0, 0 },
            { 0, 0 },     { 0, 0 },     { 0, 0 },     { 0, 0 },     { 'q', 'a' }, { 'q', 'w' },
            { 'q', 's' }, { 'q', 'd' }, { 0, 0 } };

    constexpr u16 NO_CODE = NUM_DIGRAPHS - 1;
    constexpr u16 digraphToGlyphOrderIndex( char p_c1, char p_c2 ) {
        for( u16 i = 0; i < NUM_DIGRAPHS; ++i ) {
            if( GLYPH_ORDER[ i ][ 0 ] == p_c1 && GLYPH_ORDER[ i ][ 1 ] == p_c2 ) { return i; }
        }
        return NO_CODE;
    }
} // namespace IO::KANA
