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
                              bool p_allowItems, bool p_allowDex, u8 p_toSelect ) {
        _curState           = state::GENERAL;
        _team               = p_team;
        _teamLength         = p_teamLength;
        _allowMoveSelection = p_allowMoves;
        _allowItems         = p_allowItems;
        _allowDex           = p_allowDex;
        _toSelect           = p_toSelect;
        _currentSelection   = 0;
        _partyUI            = new partyScreenUI( p_team, p_teamLength );
    }

    partyScreen::~partyScreen( ) {
        delete _partyUI;
    }

    const u16 partyScreen::getTextForChoice( const choice p_choice ) {
        switch( p_choice ) {
        case SELECT:
            return 323;
        case STATUS:
            return 324;
        case GIVE_ITEM:
            return 325;
        case TAKE_ITEM:
            return 326;
        case USE_ITEM:
            return 327;
        case FIELD_MOVE_1:
            return CHOICE_FIELD_MOVE | 0;
        case FIELD_MOVE_2:
            return CHOICE_FIELD_MOVE | 1;
        case FIELD_MOVE_3:
            return CHOICE_FIELD_MOVE | 2;
        case FIELD_MOVE_4:
            return CHOICE_FIELD_MOVE | 3;
        case SWAP:
            return 328;
        case DEX_ENTRY:
            return 329;
        case CANCEL:
            return 330;
        }
    }

    bool partyScreen::checkReturnCondition( ) {
        u8 numsel = 0;

        for( u8 i = 0; i < _teamLength; i++ ) {
            if( _currentMarksOrMove.m_selectedPkmn & ( 1 << i ) ) { ++numsel; }
        }
        return _toSelect && numsel == _toSelect;
    }

    bool partyScreen::confirmSelection( ) {
        // TODO
        return true;
    }

    void partyScreen::selectChoice( u8 p_choice ) {
        bool secondPage         = p_choice >= 6;
        _currentChoiceSelection = p_choice;
        _partyUI->drawPartyPkmnChoice(
            _currentSelection, 0,
            std::min( size_t( 6 ), _currentChoices.size( ) - ( 6 * secondPage ) ),
            !secondPage && _currentChoices.size( ) > 6, secondPage, p_choice % 6 );
    }

    bool partyScreen::focus( u8 p_selectedIdx ) {
        u16 c[ 12 ] = {0};
        for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
            c[ i ] = getTextForChoice( _currentChoices[ i ] );
        }
        if( _currentChoiceSelection >= 6 ) {
            _partyUI->drawPartyPkmnChoice( _currentSelection, c + 6,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 6 ),
                                           false, true, _currentChoiceSelection % 6 );
        } else {
            _partyUI->drawPartyPkmnChoice(
                _currentSelection, c, std::min( size_t( 6 ), _currentChoices.size( ) ),
                _currentChoices.size( ) > 6, false, _currentChoiceSelection % 66 );
        }
        int           pressed, held;
        touchPosition touch;
        u8            cooldown = COOLDOWN_COUNT;
        bool          ex       = false;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                ex = true;
                break;
            }
            if( pressed & KEY_B ) { break; }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                if( ( _currentChoiceSelection & 1 ) && _currentChoiceSelection < 6
                    && _currentChoices.size( ) > 6 ) {
                    // Switch to second page
                    _currentChoiceSelection += 5;
                    if( _currentChoiceSelection >= _currentChoices.size( ) ) {
                        _currentChoiceSelection = 6;
                    }
                    _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
                    _partyUI->drawPartyPkmnChoice(
                        _currentSelection, c + 6,
                        std::min( size_t( 6 ), _currentChoices.size( ) - 6 ), false, true,
                        _currentChoiceSelection % 6 );
                } else if( ( _currentChoiceSelection ^ 1 ) < _currentChoices.size( ) ) {
                    selectChoice( _currentChoiceSelection ^ 1 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                if( !( _currentChoiceSelection & 1 ) && _currentChoiceSelection >= 6 ) {
                    // Switch to first page
                    _currentChoiceSelection -= 5;
                    _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
                    _partyUI->drawPartyPkmnChoice( _currentSelection, c, 6, true, false,
                                                   _currentChoiceSelection % 6 );
                } else {
                    selectChoice( _currentChoiceSelection ^ 1 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                if( _currentChoiceSelection >= 6
                    && _currentChoiceSelection + 2 >= _currentChoices.size( ) ) {
                    selectChoice( ( _currentChoiceSelection & 1 ) + 6 );
                } else if( _currentChoiceSelection < 6
                           && _currentChoiceSelection + 2
                                  >= std::min( size_t( 6 ), _currentChoices.size( ) ) ) {
                    selectChoice( _currentChoiceSelection & 1 );
                } else {
                    selectChoice( _currentChoiceSelection + 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                if( _currentChoiceSelection >= 6 && _currentChoiceSelection < 8 ) {
                    selectChoice(
                        6
                        + ( _currentChoices.size( ) - 7 - ( _currentChoiceSelection & 1 ) ) / 2 * 2
                        + ( _currentChoiceSelection & 1 ) );
                } else if( _currentChoiceSelection < 2 ) {
                    selectChoice( ( std::min( size_t( 6 ), _currentChoices.size( ) ) - 1
                                    - ( _currentChoiceSelection & 1 ) )
                                      / 2 * 2
                                  + ( _currentChoiceSelection & 1 ) );
                } else {
                    selectChoice( _currentChoiceSelection - 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) { // TODO
            }

            swiWaitForVBlank( );
        }

        if( _currentChoices.size( ) <= 6 || _currentChoiceSelection >= 6 ) {
            _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
        }
        // Selecting CANCEL should result in a reset of the saved poesition
        _currentChoiceSelection %= _currentChoices.size( ) - 1;
        if( _currentChoiceSelection >= 6 && _currentChoices.size( ) > 7 ) {
            _partyUI->drawPartyPkmnChoice( _currentSelection, c + 6,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 6 - 1 ),
                                           false, true, 255 );
        } else {
            _partyUI->drawPartyPkmnChoice( _currentSelection, c,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 1 ),
                                           _currentChoices.size( ) > 7, false, 255 );
        }
        return ex;
    }

    void partyScreen::select( u8 p_selectedIdx ) {
        if( _currentSelection != p_selectedIdx ) {
            _currentSelection = p_selectedIdx;
            _partyUI->select( p_selectedIdx );

            computeSelectionChoices( );
            _currentChoiceSelection = 0;

            u16 c[ 6 ] = {0};
            for( u8 i = 0; i < std::min( size_t( 6 ), _currentChoices.size( ) ); i++ ) {
                c[ i ] = getTextForChoice( _currentChoices[ i ] );
            }

            // CANCEL is only displayed in SINGLE mode
            _partyUI->drawPartyPkmnChoice( _currentSelection, c,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 1 ),
                                           _currentChoices.size( ) - 1 > 6, false );
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

    void partyScreen::computeSelectionChoices( ) {
        _currentChoices = std::vector<choice>( );
        if( _toSelect ) { _currentChoices.push_back( SELECT ); }
        _currentChoices.push_back( STATUS );
        if( _allowItems && !_team[ _currentSelection ].isEgg( ) ) {
            if( _team[ _currentSelection ].getItem( ) ) {
                _currentChoices.push_back( TAKE_ITEM );
            } else {
                _currentChoices.push_back( GIVE_ITEM );
            }
            _currentChoices.push_back( USE_ITEM );
        }
        if( _allowMoveSelection && !_team[ _currentSelection ].isEgg( ) ) {
            for( u8 i = 0; i < 4; i++ ) {
                if( _team[ _currentSelection ].m_boxdata.m_moves[ i ]
                    && MOVE::isFieldMove( _team[ _currentSelection ].m_boxdata.m_moves[ i ] ) ) {
                    _currentChoices.push_back( choice( FIELD_MOVE_1 + i ) );
                }
            }
        }
        _currentChoices.push_back( SWAP );
        if( _allowDex && !_team[ _currentSelection ].isEgg( ) ) {
            _currentChoices.push_back( DEX_ENTRY );
        }
        _currentChoices.push_back( CANCEL );
    }

    partyScreen::result partyScreen::run( u8 p_initialSelection ) {
        _partyUI->init( p_initialSelection );
        _currentSelection       = 255;
        _currentChoiceSelection = 0;
        select( p_initialSelection );
        _frame = 0;

        int           pressed, held;
        touchPosition touch;
        u8            cooldown = COOLDOWN_COUNT;
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
                    select( ( _teamLength - 1 - ( _currentSelection & 1 ) ) / 2 * 2
                            + ( _currentSelection & 1 ) );
                } else {
                    select( _currentSelection - 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                cooldown = COOLDOWN_COUNT;
                if( focus( _currentSelection ) ) { // User pressed X
                    _currentMarksOrMove.m_selectedMove = 0;
                    break;
                }
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