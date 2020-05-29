/*
Pokémon neo
------------------------------

file        : statusScreen.cpp
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

#include "statusScreen.h"
#include "defines.h"
#include "sound.h"
#include "uio.h"

namespace STS {
    statusScreen::statusScreen( pokemon* p_pokemon, bool p_allowKeyUp, bool p_allowKeyDown,
                                statusScreenUI* p_ui ) {
        _pokemon      = p_pokemon;
        _allowKeyUp   = p_allowKeyUp;
        _allowKeyDown = p_allowKeyDown;
        if( p_ui ) {
            _ui = p_ui;
        } else {
            _ui = new statusScreenUI( );
        }
    }

    void statusScreen::select( u8 p_newPage ) {
        if( _currentPage != p_newPage ) {
            _currentPage = p_newPage;
            _ui->draw( _pokemon, _currentPage );
        }
    }

    statusScreen::result statusScreen::run( u8 p_initialPage ) {
        _ui->init( _pokemon, p_initialPage, _allowKeyUp, _allowKeyDown );
        _frame   = 0;
        cooldown = COOLDOWN_COUNT;
        _currentPage = p_initialPage;

        loop( ) {
            _ui->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return result::EXIT;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return result::BACK;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( ( _currentPage + 1 ) % _ui->getPageCount( ) );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( ( _currentPage + _ui->getPageCount( ) - 1 ) % _ui->getPageCount( ) );
                cooldown = COOLDOWN_COUNT;
            } else if( _allowKeyDown && GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                return result::NEXT_PKMN;
            } else if( _allowKeyUp && GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                return result::PREV_PKMN;
            }
            swiWaitForVBlank( );
        }

        return result::EXIT;
    }

} // namespace STS
