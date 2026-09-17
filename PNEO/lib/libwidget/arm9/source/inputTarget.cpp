/*
Pokémon neo
------------------------------

file        : uio.cpp
author      : Philip Wellnitz
description : User IO

Copyright (C) 2012 - 2026
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

#include <algorithm>
#include <nds.h>

#include "io/inputTarget.h"

namespace IO {
    int           BTN_PRESSED, BTN_HELD, BTN_LAST;
    touchPosition TOUCH;
    u8            BTN_COOLDOWN = COOLDOWN_COUNT;

    bool waitForTouchUp( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 ) {
        return waitForTouchUp( inputTarget( p_targetX1, p_targetY1, p_targetX2, p_targetY2 ) );
    }

    bool waitForTouchUp( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::TOUCH
            || p_inputTarget.m_inputType == inputTarget::inputType::TOUCH_CIRCLE ) {
            while( 1 ) {
                swiWaitForVBlank( );
                scanKeys( );
                touchRead( &TOUCH );
                if( TOUCH_UP ) { return true; }
                if( !p_inputTarget.inRange( TOUCH ) ) { return false; }
            }
        }
        return false;
    }

    bool waitForKeysUp( KEYPAD_BITS p_keys ) {
        return waitForKeysUp( inputTarget( p_keys ) );
    }

    bool waitForKeysUp( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            while( 1 ) {
                swiWaitForVBlank( );
                scanKeys( );
                if( keysUp( ) & p_inputTarget.m_keys ) { return true; }
                if( !( keysHeld( ) & p_inputTarget.m_keys ) ) { return true; }
            }
        }
        return false;
    }

    bool waitForInput( inputTarget p_inputTarget ) {
        if( p_inputTarget.m_inputType == inputTarget::inputType::BUTTON ) {
            return waitForKeysUp( p_inputTarget );
        }
        return waitForTouchUp( p_inputTarget );
    }

    void waitForInteractAB( ) {
        while( 1 ) {
            scanKeys( );
            swiWaitForVBlank( );
            IO::BTN_PRESSED = keysUp( );

            if( IO::BTN_PRESSED & KEY_A ) { break; }
            if( IO::BTN_PRESSED & KEY_B ) { break; }
            swiWaitForVBlank( );
        }
    }

    void waitForInteract( std::function<void( u8 )> p_tick, std::function<void( )> p_sfxChoose ) {
        scanKeys( );
        IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
        u8 frame         = 0;
        while( 1 ) {
            swiWaitForVBlank( );
            p_tick( ++frame );
            scanKeys( );
            touchRead( &IO::TOUCH );
            IO::BTN_PRESSED = keysUp( );
            IO::BTN_HELD    = keysHeld( );

            if( ( IO::BTN_PRESSED & KEY_A ) || ( IO::BTN_PRESSED & KEY_B ) || IO::TOUCH.px
                || IO::TOUCH.py ) {
                while( IO::TOUCH.px || IO::TOUCH.py ) {
                    p_tick( ++frame );
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &IO::TOUCH );
                    swiWaitForVBlank( );
                }
                p_sfxChoose( );
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
                break;
            }
        }
    }

    /*
     * @brief: Finishes the current frame and checks whether the given buttons were
     * pressed.
     */
    bool interruptableFrame( int p_inter ) {
        swiWaitForVBlank( );
        scanKeys( );
        IO::BTN_PRESSED = keysUp( );

        if( IO::BTN_PRESSED & p_inter ) { return true; }
        return false;
    }

} // namespace IO
