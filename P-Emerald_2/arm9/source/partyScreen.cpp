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
#include "bagViewer.h"
#include "item.h"
#include "saveGame.h"
#include "sound.h"

namespace STS {
    partyScreen::partyScreen( pokemon p_team[ 6 ], u8 p_teamLength, bool p_allowMoves,
                              bool p_allowItems, bool p_allowDex, u8 p_toSelect,
                              bool p_confirmSelection, bool p_faintSelect, bool p_eggSelect ) {
        _team               = p_team;
        _teamLength         = p_teamLength;
        _allowMoveSelection = p_allowMoves;
        _allowItems         = p_allowItems;
        _allowDex           = p_allowDex;
        _toSelect           = p_toSelect;
        _selectConfirm      = p_confirmSelection;
        _faintSelect        = p_faintSelect;
        _eggSelect          = p_eggSelect;
        _currentSelection   = 0;
        _selectedCnt        = 0;
        _partyUI            = new partyScreenUI( p_team, p_teamLength, _toSelect );
    }

    partyScreen::~partyScreen( ) {
        delete _partyUI;
    }

#ifdef DESQUID
    const u16 partyScreen::getTextForDesquidChoice( const desquidChoice p_choice ) {
        switch( p_choice ) {
        case DESQUID_SPECIES:
            return DESQUID_STRING + 1;
        case DESQUID_STATUS:
            return DESQUID_STRING + 2;
        case DESQUID_ABILITY:
            return DESQUID_STRING + 3;
        case DESQUID_NATURE:
            return DESQUID_STRING + 4;
        case DESQUID_ITEM:
            return DESQUID_STRING + 5;
        case DESQUID_MOVES:
            return DESQUID_STRING + 6;
        case DESQUID_LEVEL:
            return DESQUID_STRING + 7;
        case DESQUID_SHINY:
            return DESQUID_STRING + 8;
        case DESQUID_DUPLICATE:
            return DESQUID_STRING + 9;
        case DESQUID_DELETE:
            return DESQUID_STRING + 10;
        case DESQUID_EGG:
            return DESQUID_STRING + 11;

        case DESQUID_CANCEL:
            return 330;
        }

        return 0;
    }
#endif

    const u16 partyScreen::getTextForChoice( const choice p_choice ) {
        switch( p_choice ) {
        case SELECT:
            return 323;
        case UNSELECT:
            return 331;
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
#ifdef DESQUID
        case _DESQUID:
            return DESQUID_STRING + 0;
#endif
        }
        return 0;
    }

    bool partyScreen::checkReturnCondition( ) {
        return ( _toSelect && _selectedCnt == _toSelect )
               || ( _allowMoveSelection && _currentMarksOrMove.m_selectedMove );
    }

    bool partyScreen::confirmSelection( ) {
        // TODO
        return true;
    }

    void partyScreen::selectChoice( u8 p_choice, u8 p_numChoices ) {
        if( p_numChoices == 255 ) { p_numChoices = _currentChoices.size( ); }
        bool secondPage         = p_choice >= 6;
        _currentChoiceSelection = p_choice;
        _partyUI->drawPartyPkmnChoice( _currentSelection, 0,
                                       std::min( 6, p_numChoices - ( 6 * secondPage ) ),
                                       !secondPage && p_numChoices > 6, secondPage, p_choice % 6 );
    }

#ifdef DESQUID
    std::vector<partyScreen::desquidChoice> partyScreen::computeDesquidChoices( ) {
        std::vector<partyScreen::desquidChoice> res = std::vector<partyScreen::desquidChoice>( );

        for( u8 i = 0; i < 12; i++ ) {
            if( _teamLength <= 1
                && partyScreen::desquidChoice( i + DESQUID_SPECIES ) == DESQUID_DELETE ) {
                continue;
            }
            if( _teamLength >= 6
                && partyScreen::desquidChoice( i + DESQUID_SPECIES ) == DESQUID_DUPLICATE ) {
                continue;
            }
            res.push_back( partyScreen::desquidChoice( i + DESQUID_SPECIES ) );
        }

        return res;
    }

    std::string partyScreen::desquidItem::getString( ) {
        if( m_hasCounterName ) {
            snprintf( BUFFER, 49, GET_STRING( m_string ),
                    m_nameForValue( m_currentValue( ) ).c_str( ) );
        } else {
            snprintf( BUFFER, 49, GET_STRING( m_string ) );
        }
        return std::string( BUFFER );
    }

    std::vector<partyScreen::desquidItem> partyScreen::getDesquidItemsForChoice(
            const partyScreen::desquidChoice p_choice ) {
        auto res = std::vector<partyScreen::desquidItem>( );
        switch( p_choice ) {
            case DESQUID_SPECIES: {
                // Species
                res.push_back(partyScreen::desquidItem({
                            DESQUID_STRING + 12, true, MAX_PKMN, 1,
                            [&]() {
                                return _team[ _currentSelection ].m_boxdata.m_speciesId;
                            },
                            [&]( u16 p_newValue ) {
                                _team[ _currentSelection ].m_boxdata.m_speciesId = p_newValue;
                                _team[ _currentSelection ].setForme( 0 );
                                 if( !_team[ _currentSelection ].m_boxdata.isNicknamed( ) ) {
                                     getDisplayName( p_newValue,
                                             _team[ _currentSelection ].m_boxdata.m_name,
                                             CURRENT_LANGUAGE );
                                 }
                            },
                            []( u16 p_value ) {
                                return getDisplayName( p_value );
                            }
                        }));
                // Forme
                res.push_back(partyScreen::desquidItem({
                            DESQUID_STRING + 13, true, 31, 0,
                            [&]() {
                                return _team[ _currentSelection ].getForme( );
                            },
                            [&]( u16 p_newValue ) {
                                _team[ _currentSelection ].setForme( p_newValue );
                            },
                            [&]( u16 p_value ) {
                                return getDisplayName(
                                        _team[ _currentSelection ].m_boxdata.m_speciesId, p_value );
                            }
                        }));

                break;
            }

            default: break;
        }

        return res;
    }

#define UPDATE_VALUE( p_newValue ) do {\
    choices[ selectedLine ].m_counterUpdate( p_newValue ); \
    data = getPkmnData( p_newValue ); \
} while( false );

    bool partyScreen::desquidWindow( desquidChoice p_choice ) {
        pkmnData data = getPkmnData( _team[ _currentSelection ].m_boxdata.m_speciesId );
        snprintf( BUFFER, 49, "%s: %s", GET_STRING( DESQUID_STRING ),
                GET_STRING( getTextForDesquidChoice( p_choice ) ) );
        _partyUI->select( _currentSelection, BUFFER );
        _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );
        swiWaitForVBlank( );
        //_partyUI->drawPartyPkmnSub( _currentSelection, true, false, GET_STRING( DESQUID_STRING ) );
        _partyUI->showDesquidWindow( );

        int           pressed, held;
        touchPosition touch;
        u8            cooldown = COOLDOWN_COUNT;
        bool          redraw   = false;

        auto choices       = getDesquidItemsForChoice( p_choice );
        u8   selectedLine  = 0;
        u8   selectedDigit = 0;
        bool editing       = false;
        u32  oldval        = 0;

        for( u8 i = 0; i < choices.size( ); ++i ) {
            _partyUI->drawDesquidItem( i, choices[ i ].getString( ).c_str( ),
                                       choices[ i ].m_currentValue( ), choices[ i ].m_maxValue );
        }
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            pressed = keysUp( );
            held    = keysHeld( );

            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( editing ) {
                    redraw = false;;
                    editing = false;
                    UPDATE_VALUE( oldval );
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                                               choices[ selectedLine ].m_currentValue( ),
                                               choices[ selectedLine ].m_maxValue, true );
                    cooldown = COOLDOWN_COUNT;
                } else {
                    break;
                }
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                if( !editing ) {
                    SOUND::playSoundEffect( SFX_CHOOSE );
                    oldval = choices[ selectedLine ].m_currentValue( );
                    editing       = true;
                    selectedDigit = 0;
                    _partyUI->drawDesquidItem( selectedLine,
                            choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue, true, selectedDigit );
                } else {
                    redraw = oldval != choices[ selectedLine ].m_currentValue( );;
                    SOUND::playSoundEffect( SFX_SAVE );
                    _team[ _currentSelection ].recalculateStats( );
                    editing = false;
                    snprintf( BUFFER, 49, "%s: %s", GET_STRING( DESQUID_STRING ),
                            GET_STRING( getTextForDesquidChoice( p_choice ) ) );
                    _partyUI->select( _currentSelection, BUFFER );

                    for( u8 i = 0; i < choices.size( ); ++i ) {
                        _partyUI->drawDesquidItem( i, choices[ i ].getString( ).c_str( ),
                                choices[ i ].m_currentValue( ), choices[ i ].m_maxValue,
                                i == selectedLine );
                    }
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( editing ) {
                    u32 change = 1;
                    for( u8 i = 0; i < selectedDigit; ++i, change *= 10 );
                    if( choices[ selectedLine ].m_currentValue( ) == choices[ selectedLine ].m_minValue ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_maxValue );
                    } else if( choices[ selectedLine ].m_currentValue( ) <
                            change + choices[ selectedLine ].m_minValue ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_minValue );
                    } else {
                        UPDATE_VALUE( choices[ selectedLine ].m_currentValue( ) - change );
                    }
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue, true,
                            selectedDigit );
                } else {
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue );
                    selectedLine = ( selectedLine + 1 ) % choices.size( );
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue, true );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( editing ) {
                    u32 change = 1;
                    for( u8 i = 0; i < selectedDigit; ++i, change *= 10 );
                    if( choices[ selectedLine ].m_currentValue( ) == choices[ selectedLine ].m_maxValue ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_minValue );
                    } else if( choices[ selectedLine ].m_currentValue( ) >
                            choices[ selectedLine ].m_maxValue - change ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_maxValue );
                    } else {
                        UPDATE_VALUE( choices[ selectedLine ].m_currentValue( ) + change );
                    }
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue, true,
                            selectedDigit );
                } else {
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue );
                    selectedLine = ( selectedLine + choices.size( ) - 1 ) % choices.size( );
                    _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                            choices[ selectedLine ].m_currentValue( ),
                            choices[ selectedLine ].m_maxValue, true );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( editing && GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                u8 numDig = 0;
                for( u32 cur = choices[ selectedLine ].m_maxValue; cur > 0; ++numDig, cur /= 10 );
                selectedDigit = ( selectedDigit + 1 ) % numDig;
                _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                                           choices[ selectedLine ].m_currentValue( ),
                                           choices[ selectedLine ].m_maxValue, true, selectedDigit );

                cooldown = COOLDOWN_COUNT;
            } else if( editing && GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                u8 numDig = 0;
                for( u32 cur = choices[ selectedLine ].m_maxValue; cur > 0; ++numDig, cur /= 10 );
                selectedDigit = ( selectedDigit + numDig - 1 ) % numDig;
                _partyUI->drawDesquidItem( selectedLine, choices[ selectedLine ].getString( ).c_str( ),
                                           choices[ selectedLine ].m_currentValue( ),
                                           choices[ selectedLine ].m_maxValue, true, selectedDigit );

                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }


        _partyUI->hideDesquidWindow( );
        return redraw;
    }

#undef UPDATE_VALUE

    bool partyScreen::executeDesquidChoice( desquidChoice p_choice ) {
        switch( p_choice ) {
            case DESQUID_DUPLICATE:
                if( _teamLength < 6 ) {
                    std::memcpy( &_team[ _teamLength++ ], &_team[ _currentSelection ],
                                 sizeof( pokemon ) );
                    _partyUI->updateTeamLength( _teamLength );
                    return true;
                } else {
                    return false;
                }
            case DESQUID_DELETE:
                if( _teamLength <= 1 ) { return false; }
                if( _currentSelection + 1 < _teamLength ) {
                    std::memmove( &_team[ _currentSelection ],
                                  &_team[ _currentSelection + 1 ],
                                  ( _teamLength - _currentSelection - 1 ) * sizeof( pokemon ) );
                }
                std::memset( &_team[ --_teamLength ], 0, sizeof( pokemon ) );
                _partyUI->updateTeamLength( _teamLength );
                return true;
            default:
                break;
        }
        return desquidWindow( p_choice );
    }
#endif

    bool partyScreen::focus( u8 p_selectedIdx ) {
        u16 c[ 12 ] = {0};
        for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
            c[ i ] = getTextForChoice( _currentChoices[ i ] );
        }
        if( _currentChoiceSelection >= 6 ) {
            _partyUI->drawPartyPkmnChoice( p_selectedIdx, c + 6,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 6 ),
                                           false, true, _currentChoiceSelection % 6 );
        } else {
            _partyUI->drawPartyPkmnChoice(
                p_selectedIdx, c, std::min( size_t( 6 ), _currentChoices.size( ) ),
                _currentChoices.size( ) > 6, false, _currentChoiceSelection % 6 );
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
            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                break; }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( ( _currentChoiceSelection & 1 ) && _currentChoiceSelection < 6
                    && _currentChoices.size( ) > 6 ) {
                    // Switch to second page
                    _currentChoiceSelection += 5;
                    if( _currentChoiceSelection >= _currentChoices.size( ) ) {
                        _currentChoiceSelection = 6;
                    }
                    _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
                    _partyUI->drawPartyPkmnChoice(
                        p_selectedIdx, c + 6, std::min( size_t( 6 ), _currentChoices.size( ) - 6 ),
                        false, true, _currentChoiceSelection % 6 );
                } else if( ( _currentChoiceSelection ^ 1 ) < _currentChoices.size( ) ) {
                    selectChoice( _currentChoiceSelection ^ 1 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( !( _currentChoiceSelection & 1 ) && _currentChoiceSelection >= 6 ) {
                    // Switch to first page
                    _currentChoiceSelection -= 5;
                    _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
                    _partyUI->drawPartyPkmnChoice( p_selectedIdx, c, 6, true, false,
                                                   _currentChoiceSelection % 6 );
                } else {
                    selectChoice( _currentChoiceSelection ^ 1 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _currentChoiceSelection >= 6
                    && size_t( _currentChoiceSelection + 2 ) >= _currentChoices.size( ) ) {
                    selectChoice( ( _currentChoiceSelection & 1 ) + 6 );
                } else if( _currentChoiceSelection < 6
                           && size_t( _currentChoiceSelection + 2 )
                                  >= std::min( size_t( 6 ), _currentChoices.size( ) ) ) {
                    selectChoice( _currentChoiceSelection & 1 );
                } else {
                    selectChoice( _currentChoiceSelection + 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
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
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                ex       = executeChoice( _currentChoices[ _currentChoiceSelection ] );
                break;
            }
            swiWaitForVBlank( );
        }

        for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
            c[ i ] = getTextForChoice( _currentChoices[ i ] );
        }
        if( _currentChoices.size( ) <= 6 || _currentChoiceSelection >= 6 ) {
            _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
        }
        // Selecting CANCEL should result in a reset of the saved poesition
        _currentChoiceSelection %= _currentChoices.size( ) - 1;
        if( _currentChoiceSelection >= 6 && _currentChoices.size( ) > 7 ) {
            _partyUI->drawPartyPkmnChoice( p_selectedIdx, c + 6,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 6 - 1 ),
                                           false, true, 255 );
        } else {
            _partyUI->drawPartyPkmnChoice( p_selectedIdx, c,
                                           std::min( size_t( 6 ), _currentChoices.size( ) - 1 ),
                                           _currentChoices.size( ) > 7, false, 255 );
        }
        return ex;
    }

#ifdef DESQUID
    bool STS::partyScreen::desquid( u8 p_selectedIdx ) {
        u16  c[ 12 ] = {0};
        auto choices = computeDesquidChoices( );
        _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
        _currentChoiceSelection = 0;

        for( u8 i = 0; i < choices.size( ); i++ ) {
            c[ i ] = getTextForDesquidChoice( choices[ i ] );
        }
        _partyUI->drawPartyPkmnChoice( p_selectedIdx, c, std::min( size_t( 6 ), choices.size( ) ),
                                       choices.size( ) > 6, false, 0 );

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
            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( ( _currentChoiceSelection & 1 ) && _currentChoiceSelection < 6
                    && choices.size( ) > 6 ) {
                    // Switch to second page
                    _currentChoiceSelection += 5;
                    if( _currentChoiceSelection >= choices.size( ) ) {
                        _currentChoiceSelection = 6;
                    }
                    _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
                    _partyUI->drawPartyPkmnChoice( p_selectedIdx, c + 6,
                                                   std::min( size_t( 6 ), choices.size( ) - 6 ),
                                                   false, true, _currentChoiceSelection % 6 );
                } else if( ( _currentChoiceSelection ^ 1 ) < choices.size( ) ) {
                    selectChoice( _currentChoiceSelection ^ 1, choices.size( ) );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( !( _currentChoiceSelection & 1 ) && _currentChoiceSelection >= 6 ) {
                    // Switch to first page
                    _currentChoiceSelection -= 5;
                    _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
                    _partyUI->drawPartyPkmnChoice( p_selectedIdx, c, 6, true, false,
                                                   _currentChoiceSelection % 6 );
                } else {
                    selectChoice( _currentChoiceSelection ^ 1, choices.size( ) );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _currentChoiceSelection >= 6
                    && size_t( _currentChoiceSelection + 2 ) >= choices.size( ) ) {
                    selectChoice( ( _currentChoiceSelection & 1 ) + 6, choices.size( ) );
                } else if( _currentChoiceSelection < 6
                           && size_t( _currentChoiceSelection + 2 )
                                  >= std::min( size_t( 6 ), choices.size( ) ) ) {
                    selectChoice( _currentChoiceSelection & 1, choices.size( ) );
                } else {
                    selectChoice( _currentChoiceSelection + 2, choices.size( ) );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _currentChoiceSelection >= 6 && _currentChoiceSelection < 8 ) {
                    selectChoice(
                        6 + ( choices.size( ) - 7 - ( _currentChoiceSelection & 1 ) ) / 2 * 2
                            + ( _currentChoiceSelection & 1 ),
                        choices.size( ) );
                } else if( _currentChoiceSelection < 2 ) {
                    selectChoice( ( std::min( size_t( 6 ), choices.size( ) ) - 1
                                    - ( _currentChoiceSelection & 1 ) )
                                          / 2 * 2
                                      + ( _currentChoiceSelection & 1 ),
                                  choices.size( ) );
                } else {
                    selectChoice( _currentChoiceSelection - 2, choices.size( ) );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                if( executeDesquidChoice( choices[ _currentChoiceSelection ] ) ) { break; }
                _partyUI->select( _currentSelection );

                if( _currentChoiceSelection >= 6 ) {
                    _partyUI->drawPartyPkmnChoice( p_selectedIdx, c + 6,
                                                   std::min( size_t( 6 ), choices.size( ) - 6 ),
                                                   false, true, _currentChoiceSelection % 6 );
                } else {
                    _partyUI->drawPartyPkmnChoice(
                        p_selectedIdx, c, std::min( size_t( 6 ), choices.size( ) ),
                        choices.size( ) > 6, false, _currentChoiceSelection % 6 );
                }
            }
            swiWaitForVBlank( );
        }
        _currentChoiceSelection = 0;
        return ex;
    }
#endif

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
        _currentMarksOrMove.setMark( p_markIdx, ++_selectedCnt );
        if( _selectedCnt < _toSelect || _selectConfirm ) {
            _partyUI->mark( p_markIdx, _selectedCnt );
            _partyUI->select( _currentSelection );
        }
    }

    void partyScreen::unmark( u8 p_markIdx ) {
        u8 oldmark = _currentMarksOrMove.getMark( p_markIdx );
        _currentMarksOrMove.setMark( p_markIdx, 0 );
        _selectedCnt--;
        _partyUI->unmark( p_markIdx );

        // Shift all other marks
        for( u8 i = 0; i < _teamLength; i++ ) {
            if( _currentMarksOrMove.getMark( i ) > oldmark ) {
                _currentMarksOrMove.setMark( i, _currentMarksOrMove.getMark( i ) - 1 );
                _partyUI->unmark( i );
                _partyUI->mark( i, _currentMarksOrMove.getMark( i ) );
            }
        }

        _partyUI->select( _currentSelection );
    }

    void partyScreen::swap( u8 p_idx1, u8 p_idx2 ) {
        std::swap( _team[ p_idx1 ], _team[ p_idx2 ] );
        u8 marks = _currentMarksOrMove.getMark( p_idx1 );
        _currentMarksOrMove.setMark( p_idx1, _currentMarksOrMove.getMark( p_idx2 ) );
        _currentMarksOrMove.setMark( p_idx2, marks );

        _partyUI->swap( p_idx1, p_idx2 );
    }

    void partyScreen::computeSelectionChoices( ) {
        _currentChoices = std::vector<choice>( );
        if( _swapSelection == 255 ) {
            if( _toSelect && !_currentMarksOrMove.getMark( _currentSelection )
                && _team[ _currentSelection ].isEgg( ) == _eggSelect
                && ( _team[ _currentSelection ].m_stats.m_acHP || _faintSelect ) ) {
                _currentChoices.push_back( SELECT );
            }
            if( _toSelect && _currentMarksOrMove.getMark( _currentSelection ) ) {
                _currentChoices.push_back( UNSELECT );
            }
            _currentChoices.push_back( STATUS );
            if( _allowItems && !_team[ _currentSelection ].isEgg( ) ) {
                if( _team[ _currentSelection ].getItem( ) ) {
                    _currentChoices.push_back( TAKE_ITEM );
                }
                _currentChoices.push_back( GIVE_ITEM );
                // TODO: _currentChoices.push_back( USE_ITEM );
            }
            if( _allowMoveSelection && !_team[ _currentSelection ].isEgg( ) ) {
                for( u8 i = 0; i < 4; i++ ) {
                    if( _team[ _currentSelection ].m_boxdata.m_moves[ i ]
                        && MOVE::isFieldMove(
                               _team[ _currentSelection ].m_boxdata.m_moves[ i ] ) ) {
                        for( u8 j = 0; j < 2; ++j ) {
                            if( MOVE::possible( _team[ _currentSelection ].m_boxdata.m_moves[ i ],
                                                j ) ) {
                                _currentChoices.push_back( choice( FIELD_MOVE_1 + i ) );
                                break;
                            }
                        }
                    }
                }
            }
        }
        if( _teamLength > 1 ) { _currentChoices.push_back( SWAP ); }
        if( _swapSelection == 255 ) {
            if( _allowDex && !_team[ _currentSelection ].isEgg( ) ) {
                // TODO _currentChoices.push_back( DEX_ENTRY );
            }
#ifdef DESQUID
            _currentChoices.push_back( _DESQUID );
#endif
        }
        _currentChoices.push_back( CANCEL );
    }

    bool STS::partyScreen::executeChoice( choice p_choice ) {
        switch( p_choice ) {
        case STS::partyScreen::SELECT:
            mark( _currentSelection );
            computeSelectionChoices( );
            break;
        case STS::partyScreen::UNSELECT:
            unmark( _currentSelection );
            computeSelectionChoices( );
            break;
        case STS::partyScreen::STATUS:
            // TODO

            break;
        case STS::partyScreen::GIVE_ITEM: {
            BAG::bagViewer bv;
            u16            itm = bv.getItem( BAG::bagViewer::GIVE_TO_PKMN );
            computeSelectionChoices( );
            _partyUI->init( _currentSelection );
            _frame = 0;
            if( itm ) {
                if( _team[ _currentSelection ].getItem( ) ) {
                    auto curItm = _team[ _currentSelection ].getItem( );
                    SAVE::SAV.getActiveFile( ).m_bag.insert(
                        BAG::toBagType( ITEM::getItemData( curItm ).m_itemType ), curItm, 1 );
                }
                _partyUI->select( _currentSelection );
                _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );

                sprintf( BUFFER, GET_STRING( 334 ),
                         ITEM::getItemName( itm ).c_str( ),
                         _team[ _currentSelection ].m_boxdata.m_name );
                _partyUI->printMessage( BUFFER, itm );
                waitForInteract( );
                _partyUI->hideMessageBox( );
                _team[ _currentSelection ].giveItem( itm );
                computeSelectionChoices( );
                _partyUI->select( _currentSelection );
            }
            break;
        }
        case STS::partyScreen::TAKE_ITEM: {
            u16 acI = _team[ _currentSelection ].takeItem( );
            _partyUI->select( _currentSelection );
            _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );

            sprintf( BUFFER, GET_STRING( 101 ), ITEM::getItemName( acI ).c_str( ),
                     _team[ _currentSelection ].m_boxdata.m_name );
            _partyUI->printMessage( BUFFER, acI );
            SAVE::SAV.getActiveFile( ).m_bag.insert(
                BAG::toBagType( ITEM::getItemData( acI ).m_itemType ), acI, 1 );
            waitForInteract( );
            _partyUI->hideMessageBox( );
            computeSelectionChoices( );
            _partyUI->select( _currentSelection );
            break;
        }
        case STS::partyScreen::USE_ITEM:
            // TODO

            break;
        case STS::partyScreen::FIELD_MOVE_1:
        case STS::partyScreen::FIELD_MOVE_2:
        case STS::partyScreen::FIELD_MOVE_3:
        case STS::partyScreen::FIELD_MOVE_4:
            _currentMarksOrMove.m_selectedMove
                = _team[ _currentSelection ].m_boxdata.m_moves[ p_choice - FIELD_MOVE_1 ];
            break;
        case STS::partyScreen::SWAP:
            if( _swapSelection != 255 ) {
                swap( _currentSelection, _swapSelection );
                _swapSelection = 255;
                computeSelectionChoices( );
            } else {
                _swapSelection = _currentSelection;
                _partyUI->mark( _currentSelection, SWAP_COLOR );
                computeSelectionChoices( );
            }
            break;
        case STS::partyScreen::DEX_ENTRY:
            // TODO
            break;
#ifdef DESQUID
        case STS::partyScreen::_DESQUID:
            desquid( _currentSelection );
            computeSelectionChoices( );
            _partyUI->init( _currentSelection );
            _frame = 0;
            break;
#endif
        case STS::partyScreen::CANCEL:
            if( _swapSelection != 255 ) {
                _partyUI->unswap( _swapSelection );
                _swapSelection = 255;
                computeSelectionChoices( );
            }
            break;
        default:
            break;
        }
        return false;
    }

    void STS::partyScreen::waitForInteract( ) {
        int           pressed, held;
        touchPosition touch;
        u8            cooldown = COOLDOWN_COUNT;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( GET_KEY_COOLDOWN( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                break;
            }
        }
    }

    partyScreen::result partyScreen::run( u8 p_initialSelection ) {
        _partyUI->init( p_initialSelection );
        _currentSelection                  = 255;
        _currentChoiceSelection            = 0;
        _currentMarksOrMove.m_selectedMove = 0;
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

            if( pressed & KEY_X ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( _swapSelection != 255 ) {
                    _partyUI->unswap( _swapSelection );
                    _swapSelection = 255;
                    computeSelectionChoices( );
                    _partyUI->select( _currentSelection );
                    u16 c[ 12 ] = {0};
                    for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
                        c[ i ] = getTextForChoice( _currentChoices[ i ] );
                    }
                    _partyUI->drawPartyPkmnChoice( _currentSelection, c,
                                                   std::min( size_t( 6 ), _currentChoices.size( ) ),
                                                   _currentChoices.size( ) > 7, false );

                } else {
                    break;
                }
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( ( _currentSelection + 1 ) % _teamLength );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( ( _currentSelection + _teamLength - 1 ) % _teamLength );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _currentSelection + 2 >= _teamLength ) {
                    select( _currentSelection & 1 );
                } else {
                    select( _currentSelection + 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _currentSelection < 2 ) {
                    select( ( _teamLength - 1 - ( _currentSelection & 1 ) ) / 2 * 2
                            + ( _currentSelection & 1 ) );
                } else {
                    select( _currentSelection - 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                if( focus( _currentSelection ) ) { // User pressed X
                    _currentMarksOrMove.m_selectedMove = 0;
                    break;
                }
                if( checkReturnCondition( ) ) {
                    // Make the player confirm the set of selected pkmn, but not the selected move
                    if( !_toSelect || !_selectConfirm || confirmSelection( ) ) {
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
