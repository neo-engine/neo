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
#include <nds.h>

#include "defines.h"
#include "keyboard.h"
#include "nav.h"
#include "uio.h"

#include "A.h"
#include "Back.h"
#include "Backward.h"
#include "Forward.h"

namespace IO {
    const u8 height = 16, width = 16;
    const u8 textHeight = 14, textWidth = 12;
    const u8 sx = 16, sy = 68;
    const u8 margin      = 4;
    const u8 charsPerRow = 11;
    const u8 numRows     = 5;

    const char* pages[] = {
        "ABCDEFG  .:"
        "HIJKLMN  ,;"
        "OPQRSTU  !?"
        "VWXYZ   ÄÖÜ"
        "1234567890 ",

        "abcdefg  .:"
        "hijklmn  ,;"
        "opqrstu  !?"
        "vwxyz ß äöü"
        "1234567890 ",

        "ÀÁÂÄÇÈÉÊË ¡"
        "ÌÍÎÏÑÒÓÔÖ ¿"
        "ÙÚÛÜ $ {}"
        "+-*/~()#&×÷"
        "1234567890 ",

        "àáâäåçèéêë "
        "ìíîïñòóôö @"
        "ùúûüß · {}"
        "+-*/~()#&×÷"
        "1234567890 ",
    };

    std::string guardEmptyString( const std::string& p_string ) {
        for( auto i : p_string )
            if( i != ' ' ) return p_string;
        return "";
    }

    std::string keyboard::getText( u8 p_length ) {
        _page = 0;
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

    std::string keyboard::getText( u8 p_length, const char* p_msg ) {
        drawPage( p_msg );
        return getText( p_length );
    }

    void init( ) {
        initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = loadSprite( A_ID, 0, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                              ATiles, ATilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = loadSprite( FWD_ID, 1, tileCnt, SCREEN_WIDTH - 24, SCREEN_HEIGHT - 28 - 24, 32,
                              32, ForwardPal, ForwardTiles, ForwardTilesLen, false, false, false,
                              OBJPRIORITY_1, true );
        tileCnt = loadSprite( BWD_ID, 2, tileCnt, SCREEN_WIDTH - 28 - 24, SCREEN_HEIGHT - 24, 32,
                              32, BackwardPal, BackwardTiles, BackwardTilesLen, false, false, false,
                              OBJPRIORITY_1, true );
        updateOAM( true );
        NAV::draw( );
        initTextField( );
    }

    void keyboard::drawPage( ) {
        if( _page > 4 ) _page = 0;
        // BG_PALETTE_SUB[ COLOR_IDX ]
        //    = RGB( 30 * ( _page == 0 ), 30 * ( _page == 1 ), 30 * ( _page == 2 ) );
        for( u8 y = 0; y < numRows; ++y )
            for( u8 x = 0; x < charsPerRow; ++x ) {
                u8 px = sx + x * ( width + margin );
                u8 py = sy + y * ( height + margin );

                printChoiceBox( px, py, px + width, py + height, 2, COLOR_IDX, false );
                IO::regularFont->printChar( pages[ _page ][ y * charsPerRow + x ], px + 2, py + 2,
                                            true );
            }
    }

    void keyboard::drawPage( const char* p_msg ) {
        init( );
        if( p_msg != 0 ) regularFont->printString( p_msg, 8, 4, true );
        drawPage( );
    }

    void keyboard::drawChar( u8 p_pos, u16 p_char ) {
        IO::regularFont->printChar( p_char, 8 + p_pos * ( textWidth + margin ) + 2,
                                    -12 + ( textHeight + 2 * margin ) + textHeight, true );
    }
    void keyboard::clearChar( u8 p_pos ) {
        printRectangle( 8 + p_pos * ( textWidth + margin ), 1 + textHeight + 2 * margin,
                        8 + p_pos * ( textWidth + margin ) + textWidth,
                        5 + 2 * textHeight + 2 * margin, true,  GRAY_IDX );
        IO::regularFont->printChar( '_', 8 + p_pos * ( textWidth + margin ) + 1,
                                    -9 + 2 * textHeight + 2 * margin, true );
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
                || GET_AND_WAIT_R( 224, 164, 300, 300 ) )
                return '\n';
            if( GET_AND_WAIT( KEY_SELECT ) || GET_AND_WAIT_C( 248, 162, 16 ) ) return '\a';
            if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_C( 220, 184, 16 ) ) return '\b';

            for( u8 y = 0; y < numRows; ++y )
                for( u8 x = 0; x < charsPerRow; ++x ) {
                    u8 px = sx + x * ( width + margin );
                    u8 py = sy + y * ( height + margin );
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
