/*
Pokémon neo
------------------------------

file        : yesNoBox.cpp
author      : Philip Wellnitz
description :

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

#include "io/yesNoBox.h"

namespace IO {
    yesNoBox::selection yesNoBox::getResult(
        std::function<std::vector<std::pair<inputTarget, selection>>( )> p_drawFunction,
        std::function<void( selection )> p_selectFunction, selection p_initialSelection,
        std::function<void( )> p_tick, std::function<void( )> p_sfxCancel,
        std::function<void( )> p_sfxChoose, std::function<void( )> p_sfxSelect ) {
        // initialize the ynbox
        auto choices = p_drawFunction( );
        auto sel     = p_initialSelection;
        p_selectFunction( sel );
        BTN_COOLDOWN = COOLDOWN_COUNT;

        while( 1 ) {
            swiWaitForVBlank( );
            p_tick( );
            scanKeys( );
            touchRead( &TOUCH );
            BTN_PRESSED = keysUp( );
            BTN_HELD    = keysHeld( );

            if( BTN_PRESSED & KEY_A ) {
                // player selects current choice
                if( sel == yesNoBox::YES ) {
                    p_sfxChoose( );
                } else {
                    p_sfxCancel( );
                }
                BTN_COOLDOWN = COOLDOWN_COUNT;
                break;
            }
            if( BTN_PRESSED & KEY_B ) {
                // player cancels, i.e., selects "NO"
                p_sfxCancel( );
                BTN_COOLDOWN = COOLDOWN_COUNT;
                sel          = yesNoBox::NO;
                p_selectFunction( sel );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) || GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                // player selects other possible option
                p_sfxSelect( );

                if( sel == yesNoBox::YES ) {
                    sel = yesNoBox::NO;
                } else if( sel == yesNoBox::NO ) {
                    sel = yesNoBox::YES;
                }

                p_selectFunction( sel );

                BTN_COOLDOWN = COOLDOWN_COUNT;
            }

            // touch input
            for( auto i : choices ) {
                if( i.first.inRange( TOUCH ) ) {
                    // player touched on a position that corresponds to a possible selection
                    sel = i.second;
                    p_selectFunction( sel );
                    bool bad = false;
                    while( TOUCH.px || TOUCH.py ) {
                        // check that the player was serious about their choice, i.e., the
                        // touch-up/release happens at a "valid" position for the current
                        // selection
                        if( !i.first.inRange( TOUCH ) ) {
                            // touch release, but outside of valid selection range
                            bad = true;
                            break;
                        }
                        swiWaitForVBlank( );
                        p_tick( );

                        // update input
                        scanKeys( );
                        touchRead( &TOUCH );
                    }
                    if( !bad ) {
                        // was a valid selection
                        if( sel == yesNoBox::YES ) {
                            p_sfxChoose( );
                        } else {
                            p_sfxCancel( );
                        }
                        return sel;
                    }
                }
            }
        }
        return sel;
    }
} // namespace IO
