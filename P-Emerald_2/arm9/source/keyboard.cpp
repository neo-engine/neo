/*
Pokémon Emerald 2 Version
------------------------------

file        : keyboard.cpp
author      : Philip Wellnitz
description :

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


#include <nds.h>
#include <cmath>
#include <string>

#include "keyboard.h"
#include "uio.h"
#include "defines.h"

#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Back.h"

namespace IO {
    const u8 height = 16, width = 16;
    const u8 textHeight = 14, textWidth = 12;
    const u8 sx = 16, sy = 68;
    const u8 margin = 4;
    const u8 charsPerRow = 11;
    const u8 numRows = 5;

    const char* pages[ ] = {
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
        "ÙÚÛÜŒ $… {}"
        "+-*/~()#&×÷"
        "1234567890 ",

        "àáâäåçèéêë "
        "ìíîïñòóôö @"
        "ùúûüßœ · {}"
        "+-*/~()#&×÷"
        "1234567890 ",
    };

    std::string guardEmptyString( const std::string& p_string ) {
        for( auto i : p_string )
            if( i != ' ' )
                return p_string;
        return "";
    }

    std::string keyboard::getText( u8 p_length ) {
        _page = 0;
        for( u8 i = 0; i < p_length; ++i )
            clearChar( i );

        std::string res = "";
        char c;
        u8 pos = 0;
        loop( ) {
            c = getNextChar( );
            switch( c ) {
                case '\n':
                    return guardEmptyString( res );
                case '\b':
                    if( pos )
                        clearChar( --pos );
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
    std::string keyboard::getText( u8 p_length, const wchar_t* p_msg ) {
        drawPage( p_msg );
        return getText( p_length );
    }
    std::wstring keyboard::getWText( u8 p_length, const char* p_msg ) {
        auto rs = getText( p_length, p_msg );
        std::wstring res = L"";
        for( auto i : rs ) res += i;
        return res;
    }
    std::wstring keyboard::getWText( u8 p_length, const wchar_t* p_msg ) {
        auto rs = getText( p_length, p_msg );
        std::wstring res = L"";
        for( auto i : rs ) res += i;
        return res;
    }

    void init( ) {
        initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = loadSprite( A_ID, 0, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                              ATiles, ATilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = loadSprite( FWD_ID, 1, tileCnt,
                              SCREEN_WIDTH - 24, SCREEN_HEIGHT - 28 - 24, 32, 32, ForwardPal,
                              ForwardTiles, ForwardTilesLen, false, false, false, OBJPRIORITY_1, true );
        tileCnt = loadSprite( BWD_ID, 2, tileCnt,
                              SCREEN_WIDTH - 28 - 24, SCREEN_HEIGHT - 24, 32, 32, BackwardPal,
                              BackwardTiles, BackwardTilesLen, false, false, false, OBJPRIORITY_1, true );
        updateOAM( true );
        drawSub( );
        initTextField( );

    }

    void keyboard::drawPage( ) {
        BG_PALETTE_SUB[ COLOR_IDX ] = RGB( 30 * ( _page == 0 ), 30 * ( _page == 1 ), 30 * ( _page == 2 ) );
        for( u8 y = 0; y < numRows; ++y )
            for( u8 x = 0; x < charsPerRow; ++x ) {
                u8 px = sx + x * ( width + margin );
                u8 py = sy + y * ( height + margin );

                printChoiceBox( px, py, px + width, py + height,
                                2, COLOR_IDX, false );
                printChar( regularFont, pages[ _page ][ y * charsPerRow + x ],
                           px + 2, py + 2, true );
            }
    }

    void keyboard::drawPage( const char* p_msg ) {
        init( );
        if( p_msg != 0 )
            regularFont->printString( p_msg, 8, 4, true );
        drawPage( );
    }
    void keyboard::drawPage( const wchar_t* p_msg ) {
        init( );
        if( p_msg != 0 )
            regularFont->printString( p_msg, 8, 4, true );
        drawPage( );
    }

    void keyboard::drawChar( u8 p_pos, u16 p_char ) {
        printChar( regularFont, p_char, 8 + p_pos * ( textWidth + margin ) + 2,
                   -13 + ( textHeight + 2 * margin ) + textHeight, true );
    }
    void keyboard::clearChar( u8 p_pos ) {
        printRectangle( 8 + p_pos * ( textWidth + margin ), 2 + textHeight + 2 * margin,
                        8 + p_pos * ( textWidth + margin ) + textWidth, 4 + 2 * textHeight + 2 * margin,
                        true, false, GRAY_IDX );
        printChar( regularFont, '_', 8 + p_pos * ( textWidth + margin ) + 1,
                   -10 + 2 * textHeight + 2 * margin, true );
    }

    u16 keyboard::getNextChar( ) {
        touchPosition touch;
        int pressed;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_SELECT ) )
                return '\a';
            if( GET_AND_WAIT( KEY_B ) )
                return '\b';
            if( GET_AND_WAIT( KEY_START ) )
                return '\n';
        }
        return '\n';
    }


    /* std::wstring keyboard::getText( u16 p_length, const char* p_msg ) {


         std::wstring out = L"";
         draw( p_msg, out, p_length );

         touchPosition touch;
         u8 i = 0;
         loop( ) {
             scanKeys( );
             touchRead( &touch );

             u16 keyPosX = ( touch.px - x ) / ( width + margin );
             u16 keyPosY = ( touch.py - y ) / ( height + margin );

             u16 nx = x + keyPosX * ( width + margin ),
                 ny = y + keyPosY * ( width + margin );

             if( i < p_length && _chars.count( { _ind, { nx, ny } } ) ) {
                 printChoiceBox( nx, ny, nx + width, ny + height, 2, COLOR_IDX, true );
                 printChar( regularFont, _chars[ { _ind, { nx, ny } } ], nx + 4, ny + 4, true );
                 bool res = waitForTouchUp( nx, ny, nx + width, ny + height );

                 printChoiceBox( nx, ny, nx + width, ny + height, 2, COLOR_IDX, false );
                 printChar( regularFont, _chars[ { _ind, { nx, ny } } ], nx + 2, ny + 2, true );

                 if( !res )
                     continue;

                 ++i;
                 out += _chars[ { _ind, { nx, ny } } ];

                 printChar( regularFont, _chars[ { _ind, { nx, ny } } ], 8 + i * ( width + margin ) + 2, 4 + ( height + margin ) * !!p_msg + 2, true );
                 swiWaitForVBlank( );
             } else if( GET_AND_WAIT_C( 248, 162, 16 ) ) {
                 _ind = ( _ind + 1 ) % MAXKEYBOARDS;
                 draw( p_msg, out, p_length );
             } else if( GET_AND_WAIT_C( 248, 184, 16 ) ) {
                 return out;
             } else if( GET_AND_WAIT_C( 220, 184, 16 ) ) {
                 out.pop_back( );
                 --i;
                 printRectangle( 8 + i * ( width + margin ), 4 + ( height + margin ) * !!p_msg,
                                 8 + i * ( width + margin ) + width, 4 + ( height + margin ) * !!p_msg + height,
                                 true, false, GRAY_IDX );
                 printChar( regularFont, '_', 8 + i * ( width + margin ) + 2, 4 + ( height + margin ) * !!p_msg + 2, true );
             }
             swiWaitForVBlank( );
         }

         return out;
     }
 */
}