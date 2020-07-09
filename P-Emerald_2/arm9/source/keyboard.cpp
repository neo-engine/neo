/*
Pokémon neo
------------------------------

file        : keyboard.cpp
author      : Philip Wellnitz
description :

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

#include <cmath>
#include <string>
#include <vector>
#include <nds.h>

#include "defines.h"
#include "fs.h"
#include "keyboard.h"
#include "nav.h"
#include "uio.h"

namespace IO {
    const u8 height = 16, width = 12;
    const u8 sx = 25, sy = 38;
    const u8 marginx     = 4;
    const u8 marginy     = 8;
    const u8 charsPerRow = 13;
    const u8 numRows     = 5;

    const char* pages[] = { "ABCDEFGHIJKLM"
                            "NOPQRSTUVWXYZ"
                            "abcdefghijklm"
                            "nopqrstuvwxyz"
                            "\xc4\xc9\xd6\xdc \xe4\xe9\xf6\xfc\xdf .,",

                            "1234567890 +-"
                            ".,:;         "
                            "             "
                            "             "
                            "             " };

    std::string guardEmptyString( const std::string& p_string ) {
        for( auto i : p_string )
            if( i != ' ' ) return p_string;
        return "";
    }

    std::string keyboard::getText( u8 p_length ) {
        _page = 0;
        init( );
        for( u8 i = 0; i < p_length; ++i ) clearChar( i );

        std::string res = "";
        char        c;
        u8          pos = 0;
        loop( ) {
            c = getNextChar( );
            switch( c ) {
            case '\n':
                return guardEmptyString( res );
            case '\b':
                if( pos ) {
                    clearChar( --pos );
                    res.pop_back( );
                }
                break;
            case '\a':
                _page = ( _page + 1 ) % MAX_KEYBOARD_PAGES;
                drawPage( );
                break;
            default:
                if( pos < p_length ) {
                    res += c;
                    drawChar( pos++, c );
                }
                break;
            }
        }
    }

    void keyboard::init( ) const {
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "keyboard", 240 * 2,
                             256 * 192, true );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( 0, 2 );

        initOAMTable( true );
        updateOAM( true );

        drawPage( );
    }

    u8 keyboard::maxPages( ) const {
        switch( CURRENT_LANGUAGE ) {
        default:
            return 2;
        }
    }

    void keyboard::drawPage( ) const {
        for( u8 y = 0; y < numRows; ++y )
            for( u8 x = 0; x < charsPerRow; ++x ) {
                u8 px = sx + x * ( width + marginx );
                u8 py = sy + y * ( height + marginy );

                IO::boldFont->printChar( pages[ _page ][ y * charsPerRow + x ], px + 2, py + 2,
                                         true );
            }

        IO::smallFont->printString( GET_STRING( 439 ), 135, 158, true, IO::font::CENTER );
        IO::smallFont->printString( GET_STRING( 440 ), 195, 158, true, IO::font::CENTER );
    }

    void keyboard::drawChar( u8 p_pos, u16 p_char ) {
        IO::boldFont->printChar( p_char, 72 + p_pos * width, 10, true );
    }
    void keyboard::clearChar( u8 p_pos ) {
        IO::printRectangle( 70 + p_pos * width, 10, 70 + ( p_pos + 1 ) * width - 1, 15 + 16, true,
                            0 );
        IO::regularFont->printChar( '_', 70 + p_pos * width, 15, true );
    }

    u16 keyboard::getNextChar( ) {
        touchPosition touch;
        int           pressed;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_START ) || GET_AND_WAIT( KEY_A )
                /*|| GET_AND_WAIT_R( 224, 164, 300, 300 ) */ )
                return '\n';
            if( GET_AND_WAIT( KEY_SELECT ) /* || GET_AND_WAIT_C( 248, 162, 16 ) */ ) return '\a';
            if( GET_AND_WAIT( KEY_B ) /* || GET_AND_WAIT_C( 220, 184, 16 ) */ ) return '\b';

            for( u8 y = 0; y < numRows; ++y )
                for( u8 x = 0; x < charsPerRow; ++x ) {
                    u8 px = sx + x * ( width + marginx );
                    u8 py = sy + y * ( height + marginy );
                    if( touch.px >= px && touch.px <= px + width && touch.py >= py
                        && touch.py <= py + height ) {

                        printChoiceBox( px, py, px + width, py + height, 2, COLOR_IDX, true );
                        IO::regularFont->printChar( pages[ _page ][ y * charsPerRow + x ], px + 4,
                                                    py + 3, true );
                        if( waitForTouchUp( px, py, px + width, py + width ) ) {
                            printChoiceBox( px, py, px + width, py + height, 2, COLOR_IDX, false );
                            IO::regularFont->printChar( pages[ _page ][ y * charsPerRow + x ],
                                                        px + 2, py + 2, true );
                            return pages[ _page ][ y * charsPerRow + x ];
                        }
                        printChoiceBox( px, py, px + width, py + height, 2, COLOR_IDX, false );
                        IO::regularFont->printChar( pages[ _page ][ y * charsPerRow + x ], px + 2,
                                                    py + 2, true );
                    }
                }
        }
        return '\n';
    }
} // namespace IO
