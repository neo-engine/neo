/*
Pokémon neo
------------------------------

file        : yesNoBox.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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
#include "defines.h"
#include "io/uio.h"
#include "nav/nav.h"
#include "sound/sound.h"

namespace IO {
    yesNoBox::selection yesNoBox::getResult(
        std::function<std::vector<std::pair<inputTarget, selection>>( )> p_drawFunction,
        std::function<void( selection )> p_selectFunction, selection p_initialSelection,
        std::function<void( )> p_tick ) {
        // initialize the ynbox
        auto choices = p_drawFunction( );
        auto sel     = p_initialSelection;
        p_selectFunction( sel );
        cooldown = COOLDOWN_COUNT;

        // loop until player made a decision
        loop( ) {
            p_tick( );
            // check for new input from the player
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_A ) {
                // player selects current choice
                if( sel == yesNoBox::YES ) {
                    SOUND::playSoundEffect( SFX_CHOOSE );
                } else {
                    SOUND::playSoundEffect( SFX_CANCEL );
                }
                cooldown = COOLDOWN_COUNT;
                break;
            }
            if( pressed & KEY_B ) {
                // player cancels, i.e., selects "NO"
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown = COOLDOWN_COUNT;
                sel      = yesNoBox::NO;
                p_selectFunction( sel );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) || GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                // player selects other possible option
                SOUND::playSoundEffect( SFX_SELECT );

                if( sel == yesNoBox::YES ) {
                    sel = yesNoBox::NO;
                } else if( sel == yesNoBox::NO ) {
                    sel = yesNoBox::YES;
                }

                p_selectFunction( sel );

                cooldown = COOLDOWN_COUNT;
            }

            // touch input
            for( auto i : choices ) {
                if( i.first.inRange( touch ) ) {
                    // player touched on a position that corresponds to a possible selection
                    sel = i.second;
                    p_selectFunction( sel );
                    bool bad = false;
                    while( touch.px || touch.py ) {
                        // check that the player was serious about their choice, i.e., the
                        // touch-up/release happens at a "valid" position for the current
                        // selection
                        swiWaitForVBlank( );
                        if( !i.first.inRange( touch ) ) {
                            // touch release, but outside of valid selection range
                            bad = true;
                            break;
                        }
                        p_tick( );

                        // update input
                        scanKeys( );
                        touchRead( &touch );
                        swiWaitForVBlank( );
                    }
                    if( !bad ) {
                        // was a valid selection
                        if( sel == yesNoBox::YES ) {
                            SOUND::playSoundEffect( SFX_CHOOSE );
                        } else {
                            SOUND::playSoundEffect( SFX_CANCEL );
                        }
                        return sel;
                    }
                }
            }

            // wait for a vertical blank
            swiWaitForVBlank( );
        }
        return sel;
    }

    yesNoBox::selection yesNoBox::getResult( const char* p_message, style p_style,
                                             bool p_showMoney ) {
        return getResult(
            [ & ]( ) { return NAV::printYNMessage( p_message, p_style, 255, p_showMoney ); },
            [ & ]( yesNoBox::selection p_selection ) {
                NAV::printYNMessage( 0, p_style, p_selection == IO::yesNoBox::NO, p_showMoney );
            } );
    }
} // namespace IO
