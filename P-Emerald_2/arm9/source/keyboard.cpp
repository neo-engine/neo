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
#include "Back.h"

namespace IO {
    const u16 height = 18, width = 12;
    const u16 x = 16, y = 70;
    const u16 margin = 2;
    const u16 charsPerRow = 13;

    keyboard::keyboard( ) {
        _ind = 0;
        //Initialize the keyboard's characters

        std::string spchars[ ] = { "ÄÖÜ .!?()[]{}", "äöüßé §$%&/\\#" };
        for( wchar_t c = L'A'; c <= L'Z'; ++c ) {
            u16 idx = u16( c - L'A' );

            u16 xpos = x + ( ( idx % charsPerRow ) * ( width + margin ) );
            u16 ypos = y + ( ( idx / charsPerRow ) * ( height * margin ) );

            _chars[ { (u8) 0, { xpos, ypos } } ] = c;
            _chars[ { (u8) 1, { xpos, ypos } } ] = L'a' + idx;
            if( idx < 10 )
                _chars[ { (u8) 2, { xpos, ypos } } ] = L'0' + idx;

            if( idx < charsPerRow ) {
                ypos += u16( 3 * ( height * margin ) );

                _chars[ { (u8) 0, { xpos, ypos } } ] = spchars[ 0 ][ idx ];
                _chars[ { (u8) 1, { xpos, ypos } } ] = spchars[ 1 ][ idx ];
            }
        }
    }

    std::wstring keyboard::getText( u16 p_length, const char* p_msg ) {

        initOAMTable( true );
        u16 nextAvailableTileIdx = 16;

        nextAvailableTileIdx = loadSprite( A_ID, 0, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                                           ATiles, ATilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = loadSprite( FWD_ID, 1, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 24, SCREEN_HEIGHT - 28 - 22, 32, 32, ForwardPal,
                                           ForwardTiles, ForwardTilesLen, false, false, false, OBJPRIORITY_1, true );
        nextAvailableTileIdx = loadSprite( BWD_ID, 2, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28 - 18, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                           BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_1, true );

        updateOAM( true );

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

    void keyboard::draw( const char* p_msg, std::wstring& p_currSel, u16 p_length ) {
        BG_PALETTE_SUB[ COLOR_IDX ] = RGB( 30 * ( _ind == 0 ), 30 * ( _ind == 1 ), 30 * ( _ind == 2 ) );

        drawSub( );
        initTextField( );
        if( p_msg != 0 )
            regularFont->printString( p_msg, 8, 4, true );
        for( u8 i = 0; i < p_length; ++i ) {
            printRectangle( 8 + i * ( width + margin ), 4 + ( height + margin ) * !!p_msg,
                            8 + i * ( width + margin ) + width, 4 + ( height + margin ) * !!p_msg + height,
                            true, false, GRAY_IDX );

            u16 currChar = '_';
            if( i < p_currSel.length( ) )
                currChar = p_currSel[ i ];

            printChar( regularFont, currChar, 8 + i * ( width + margin ) + 2, 4 + ( height + margin ) * !!p_msg + 2, true );
        }

        for( auto i : _chars ) {
            if( i.first.first != _ind )
                continue;

            printChoiceBox( i.first.second.first,
                            i.first.second.second,
                            i.first.second.first + width,
                            i.first.second.second + height,
                            2,
                            COLOR_IDX,
                            false );
            printChar( regularFont, i.second,
                       i.first.second.first + 2,
                       i.first.second.second + 2, true );
        }
    }
}