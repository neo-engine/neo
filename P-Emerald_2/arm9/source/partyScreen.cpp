/*
Pokémon neo
------------------------------

file        : partyScreen.cpp
author      : Philip Wellnitz
description : Run the pkmn party screen

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

#include "partyScreen.h"

namespace STS {
    partyScreen::partyScreen( pokemon p_team[ 6 ], u8 p_teamLength, bool p_allowMoves,
                              u8 p_toSelect ) {
        _curState           = state::GENERAL;
        _team               = p_team;
        _teamLength         = p_teamLength;
        _allowMoveSelection = p_allowMoves;
        _toSelect           = p_toSelect;
        _currentSelection   = 0;
        _partyUI            = new partyScreenUI( p_team, p_teamLength );
    }

    partyScreen::~partyScreen( ) {
        delete _partyUI;
    }

    bool partyScreen::checkReturnCondition( ) {
        u8 numsel = 0;

        for( u8 i = 0; i < _teamLength; i++ ) {
            if( _currentMarksOrMove.m_selectedPkmn & ( 1 << i ) ) { ++numsel; }
        }
        return numsel == _toSelect;
    }

    bool partyScreen::confirmSelection( ) {
        // TODO
        return true;
    }

    void partyScreen::focus( u8 p_selectedIdx ) {
        // TODO
    }

    void partyScreen::select( u8 p_selectedIdx ) {
        if( _currentSelection != p_selectedIdx ) {
            _currentSelection = p_selectedIdx;
            _partyUI->select( p_selectedIdx );
        }
    }

    void partyScreen::mark( u8 p_markIdx ) {
        // TODO
    }

    void partyScreen::unmark( u8 p_markIdx ) {
        // TODO
    }

    void partyScreen::swap( u8 p_idx1, u8 p_idx2 ) {
        // TODO
    }

    partyScreen::result partyScreen::run( u8 p_initialSelection ) {
        _currentSelection = p_initialSelection;
        _partyUI->init( _currentSelection );
        _frame = 0;

        int           pressed, held;
        touchPosition touch;
        u8            cooldown = 1;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) || ( pressed & KEY_B ) ) break;
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                select( ( _currentSelection + 1 ) % _teamLength );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                select( ( _currentSelection + _teamLength - 1 ) % _teamLength );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                if( _currentSelection + 2 >= _teamLength ) {
                    select( _currentSelection & 1 );
                } else {
                    select( _currentSelection + 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                if( _currentSelection < 2 ) {
                    select( _teamLength / 2 * 2 - ( _currentSelection & 1 ) );
                } else {
                    select( _currentSelection - 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                cooldown = COOLDOWN_COUNT;
                focus( _currentSelection );
                if( checkReturnCondition( ) ) {
                    // Make the player confirm the set of selected pkmn, but not the selected move
                    if( !_toSelect || confirmSelection( ) ) {
                        break;
                    } else {
                        unmark( _currentSelection );
                    }
                }
            }

            swiWaitForVBlank( );
        }

        return _currentMarksOrMove;
    }
} // namespace STS