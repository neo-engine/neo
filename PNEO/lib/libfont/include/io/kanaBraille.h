/*
Pokémon neo
------------------------------

file        : kanaBraille.h
author      : Philip Wellnitz
description : Generated kana braille bank.

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

#include "io/kana.h"

namespace IO::KANA {
    constexpr u8 NONE       = 0xFF;
    constexpr u8 DAKUTEN    = 1 << 4;
    constexpr u8 HANDAKUTEN = 1 << 5;
    constexpr u8 YOON       = 1 << 3;

    constexpr u8 CELLS[ NUM_DIGRAPHS ][ 2 ] = {
        { 0x01, NONE }, { 0x01, NONE }, { 0x03, NONE }, { 0x03, NONE }, { 0x09, NONE },
        { 0x09, NONE }, { 0x0B, NONE }, { 0x0B, NONE }, { 0x0A, NONE }, { 0x0A, NONE },
        { 0x21, NONE }, { 0x10, 0x21 }, { 0x23, NONE }, { 0x10, 0x23 }, { 0x29, NONE },
        { 0x10, 0x29 }, { 0x2B, NONE }, { 0x10, 0x2B }, { 0x2A, NONE }, { 0x10, 0x2A },
        { 0x31, NONE }, { 0x10, 0x31 }, { 0x33, NONE }, { 0x10, 0x33 }, { 0x39, NONE },
        { 0x10, 0x39 }, { 0x3B, NONE }, { 0x10, 0x3B }, { 0x3A, NONE }, { 0x10, 0x3A },
        { 0x15, NONE }, { 0x10, 0x15 }, { 0x17, NONE }, { 0x10, 0x17 }, { 0x02, NONE },
        { 0x1D, NONE }, { 0x10, 0x1D }, { 0x1F, NONE }, { 0x10, 0x1F }, { 0x1E, NONE },
        { 0x10, 0x1E }, { 0x05, NONE }, { 0x07, NONE }, { 0x0D, NONE }, { 0x0F, NONE },
        { 0x0E, NONE }, { 0x25, NONE }, { 0x10, 0x25 }, { 0x20, 0x25 }, { 0x27, NONE },
        { 0x10, 0x27 }, { 0x20, 0x27 }, { 0x2D, NONE }, { 0x10, 0x2D }, { 0x20, 0x2D },
        { 0x2F, NONE }, { 0x10, 0x2F }, { 0x20, 0x2F }, { 0x2E, NONE }, { 0x10, 0x2E },
        { 0x20, 0x2E }, { 0x35, NONE }, { 0x37, NONE }, { 0x3D, NONE }, { 0x3F, NONE },
        { 0x3E, NONE }, { 0x08, NONE }, { 0x0C, NONE }, { 0x08, NONE }, { 0x2C, NONE },
        { 0x08, NONE }, { 0x1C, NONE }, { 0x11, NONE }, { 0x13, NONE }, { 0x19, NONE },
        { 0x1B, NONE }, { 0x1A, NONE }, { 0x04, NONE }, { 0x14, NONE }, { 0x34, NONE },
        { 0x01, NONE }, { 0x01, NONE }, { 0x03, NONE }, { 0x03, NONE }, { 0x09, NONE },
        { 0x09, NONE }, { 0x0B, NONE }, { 0x0B, NONE }, { 0x0A, NONE }, { 0x0A, NONE },
        { 0x21, NONE }, { 0x10, 0x21 }, { 0x23, NONE }, { 0x10, 0x23 }, { 0x29, NONE },
        { 0x10, 0x29 }, { 0x2B, NONE }, { 0x10, 0x2B }, { 0x2A, NONE }, { 0x10, 0x2A },
        { 0x31, NONE }, { 0x10, 0x31 }, { 0x33, NONE }, { 0x10, 0x33 }, { 0x39, NONE },
        { 0x10, 0x39 }, { 0x3B, NONE }, { 0x10, 0x3B }, { 0x3A, NONE }, { 0x10, 0x3A },
        { 0x15, NONE }, { 0x10, 0x15 }, { 0x17, NONE }, { 0x10, 0x17 }, { 0x02, NONE },
        { 0x1D, NONE }, { 0x10, 0x1D }, { 0x1F, NONE }, { 0x10, 0x1F }, { 0x1E, NONE },
        { 0x10, 0x1E }, { 0x05, NONE }, { 0x07, NONE }, { 0x0D, NONE }, { 0x0F, NONE },
        { 0x0E, NONE }, { 0x25, NONE }, { 0x10, 0x25 }, { 0x20, 0x25 }, { 0x27, NONE },
        { 0x10, 0x27 }, { 0x20, 0x27 }, { 0x2D, NONE }, { 0x10, 0x2D }, { 0x20, 0x2D },
        { 0x2F, NONE }, { 0x10, 0x2F }, { 0x20, 0x2F }, { 0x2E, NONE }, { 0x10, 0x2E },
        { 0x20, 0x2E }, { 0x35, NONE }, { 0x37, NONE }, { 0x3D, NONE }, { 0x3F, NONE },
        { 0x3E, NONE }, { 0x08, NONE }, { 0x0C, NONE }, { 0x08, NONE }, { 0x2C, NONE },
        { 0x08, NONE }, { 0x1C, NONE }, { 0x11, NONE }, { 0x13, NONE }, { 0x19, NONE },
        { 0x1B, NONE }, { 0x1A, NONE }, { 0x04, NONE }, { 0x14, NONE }, { 0x34, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { 0x16, NONE }, { 0x22, NONE },
        { 0x30, NONE }, { 0x32, NONE }, { NONE, NONE }, { 0x02, NONE }, { NONE, NONE },
        { 0x24, NONE }, { 0x24, NONE }, { NONE, NONE }, { NONE, NONE }, { 0x36, NONE },
        { 0x36, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { 0x12, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE }, { NONE, NONE },
        { NONE, NONE },
    };

    constexpr bool hasCells( u16 p_code ) {
        return isKana( p_code ) && CELLS[ valToKana( p_code, 0 ) ][ 0 ] != NONE;
    }

    // Yoon merge
    constexpr u8 YOON_COL[ 26 ]
        = { NONE, 0x24, NONE, 0x14, NONE, NONE, 0x20, 0x24, NONE, NONE, 0x20, NONE, 0x34,
            0x04, NONE, 0x24, NONE, 0x10, 0x30, 0x14, NONE, NONE, NONE, NONE, NONE, 0x30 };
    constexpr u8 YOON_MARK[ 26 ]
        = { NONE, 0x18, NONE, 0x18, NONE, NONE, 0x18, 0x08, NONE, NONE, 0x08, NONE, 0x08,
            0x08, NONE, 0x28, NONE, 0x08, 0x08, 0x08, NONE, NONE, NONE, NONE, NONE, 0x18 };

    constexpr u8 yoonVowel( char p_char ) {
        switch( p_char ) {
        case 'a':
        case 'A': return 0x01;
        case 'o':
        case 'O': return 0x09;
        case 'u':
        case 'U': return 0x0A;
        default: return NONE;
        }
    }

    constexpr bool isSmallYoon( char p_c1, char p_c2 ) {
        return ( p_c1 == 'y' && ( p_c2 == 'A' || p_c2 == 'U' || p_c2 == 'O' ) )
               || ( p_c1 == 'Y' && ( p_c2 == 'a' || p_c2 == 'u' || p_c2 == 'o' ) );
    }

    constexpr bool yoonExpand( char p_c1, char p_c3, char p_c4, u8& p_cell0, u8& p_cell1 ) {
        if( !isSmallYoon( p_c3, p_c4 ) ) { return false; }

        char k = isUpper( p_c1 ) ? (char) ( p_c1 - 'A' + 'a' ) : p_c1;
        if( !isLower( k ) ) { return false; }
        u8 col = YOON_COL[ k - 'a' ];
        if( col == NONE ) { return false; }

        u8 vow = yoonVowel( p_c4 );
        if( vow == NONE ) { return false; }

        p_cell0 = YOON_MARK[ k - 'a' ];
        p_cell1 = (u8) ( col | vow );
        return true;
    }
} // namespace IO::KANA
