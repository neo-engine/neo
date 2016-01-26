/*
Pokémon Emerald 2 Version
------------------------------

file        : yesNoBox.cpp
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

#include "yesNoBox.h"
#include "uio.h"
#include "defines.h"

namespace IO {

    void yesNoBox::draw( u8 p_pressedIdx ) {
        printChoiceBox( 28, 102, 122, 134, 6, RED_IDX, p_pressedIdx == 0 );
        regularFont->printString( "Ja", 65 + 2 * ( p_pressedIdx == 0 ),
                                  110 + ( p_pressedIdx == 0 ), true );
        printChoiceBox( 134, 102, 228, 134, 6, BLUE_IDX, p_pressedIdx == 1 );
        regularFont->printString( "Nein", 165 + 2 * ( p_pressedIdx == 1 ),
                                  110 + ( p_pressedIdx == 1 ), true );
    }

    yesNoBox::yesNoBox( bool p_initSprites ) {
        initTextField( );
        if( p_initSprites )
            initOAMTable( true );
        _isNamed = false;
    }
    yesNoBox::yesNoBox( const char* p_name, bool p_initSprites ) {
        initTextField( );
        if( p_initSprites )
            initOAMTable( true );
        regularFont->printString( p_name, 8, 8, true );

        swiWaitForVBlank( );
        _isNamed = true;
    }
    yesNoBox::yesNoBox( messageBox p_box, bool p_initSprites ) {
        initTextField( );
        if( p_initSprites )
            initOAMTable( true );
        _isNamed = p_box.m_isNamed;
    }

    bool yesNoBox::getResult( const char* p_text = 0 ) {
        s16 x = 8 + 64 * !!_isNamed;
        s16 y = 8;
        if( p_text )
            regularFont->printStringD( p_text, x, y, true );
        draw( 2 );
        bool result;
        loop( ) {
            swiWaitForVBlank( );
            touchPosition t;
            touchRead( &t );
            if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
                draw( 0 );
                if( !waitForTouchUp( 28, 102, 122, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = true;
                break;
            } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
                draw( 1 );
                if( !waitForTouchUp( 134, 102, 228, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = false;
                break;
            }
        }
        return result;
    }
    bool yesNoBox::getResult( const wchar_t* p_text = 0 ) {
        s16 x = 8 + 64 * !!_isNamed;
        s16 y = 8;
        if( p_text )
            regularFont->printStringD( p_text, x, y, true );

        draw( 2 );
        bool result;
        loop( ) {
            swiWaitForVBlank( );
            touchPosition t;
            touchRead( &t );
            if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
                draw( 0 );
                if( !waitForTouchUp( 28, 102, 122, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = true;
                break;
            } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
                draw( 1 );
                if( !waitForTouchUp( 134, 102, 228, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = false;
                break;
            }
        }
        return result;
    }

}