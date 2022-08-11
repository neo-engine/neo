/*
Pokémon neo
------------------------------

file        : partyScreen.cpp
author      : Philip Wellnitz
description : Run the pkmn party screen

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

#include "sts/partyScreen.h"
#include "bag/bagViewer.h"
#include "bag/item.h"
#include "dex/dex.h"
#include "fs/data.h"
#include "io/yesNoBox.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "sts/statusScreen.h"

#ifdef DESQUID
#include "battle/ability.h"
#include "fs/fs.h"
#endif

namespace STS {
    partyScreen::partyScreen( pokemon p_team[ 6 ], u8 p_teamLength, bool p_allowMoves,
                              bool p_allowItems, bool p_allowDex, u8 p_toSelect,
                              bool p_confirmSelection, bool p_faintSelect, bool p_eggSelect,
                              bool p_allowCancel, u8 p_inBattle, u8 p_toSwap ) {
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
        _allowCancel        = p_allowCancel;
        _inBattle           = p_inBattle;
        _toSwap             = p_toSwap;
        _partyUI = new partyScreenUI( p_team, p_teamLength, _toSelect, _allowCancel, _inBattle,
                                      _toSwap );
    }

    partyScreen::~partyScreen( ) {
        if( _partyUI ) { delete _partyUI; }
    }

    int           pressed, held;
    touchPosition touch;
    u8            cooldown = COOLDOWN_COUNT;

    bool partyScreen::checkReturnCondition( ) {
        return ( _toSelect && _selectedCnt == _toSelect )
               || ( _allowMoveSelection && _currentMarksOrMove.m_selectedMove );
    }

    bool partyScreen::confirmSelection( ) {
        if( _toSelect > 1 ) {
            _partyUI->select( _currentSelection, GET_STRING( 335 ) );
        } else {
            _partyUI->select( _currentSelection, GET_STRING( 336 ) );
        }
        _ranges = _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );
        IO::yesNoBox yn;
        bool         res = yn.getResult( [ & ]( ) { return _partyUI->printYNMessage( 0, 254 ); },
                                 [ & ]( IO::yesNoBox::selection p_sel ) {
                                     _partyUI->printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                                 },
                                 IO::yesNoBox::YES, [ & ]( ) { _partyUI->animate( _frame++ ); } )
                   == IO::yesNoBox::YES;

        _partyUI->hideYNMessageBox( );
        _currentChoiceSelection = 0;
        return res;
    }

    void partyScreen::selectChoice( u8 p_choice, u8 p_numChoices ) {
        if( p_numChoices == 255 ) { p_numChoices = _currentChoices.size( ); }
        bool secondPage         = p_choice >= 6;
        _currentChoiceSelection = p_choice;
        _ranges                 = _partyUI->drawPartyPkmnChoice(
                            _currentSelection, 0, std::min( 6, p_numChoices - ( 6 * secondPage ) ),
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

    std::string partyScreen::desquidItem::computeString( ) {
        if( m_hasCounterName ) {
            snprintf( BUFFER, 49, GET_STRING( m_string ),
                      m_nameForValue( m_currentValue( ) ).c_str( ) );
        } else {
            snprintf( BUFFER, 49, GET_STRING( m_string ) );
        }
        return std::string( BUFFER );
    }

    std::vector<partyScreen::desquidItem>
    partyScreen::getDesquidItemsForChoice( const partyScreen::desquidChoice p_choice ) {
        auto res = std::vector<partyScreen::desquidItem>( );
        switch( p_choice ) {
        case DESQUID_SPECIES: {
            // Species
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 12, true, MAX_PKMN, 0,
                  [ & ]( ) { return _team[ _currentSelection ].getSpecies( ); },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].setSpecies( p_newValue );
                      if( !_team[ _currentSelection ].m_boxdata.isNicknamed( ) ) {
                          FS::getDisplayName( p_newValue,
                                              _team[ _currentSelection ].m_boxdata.m_name,
                                              CURRENT_LANGUAGE );
                      }
                  },
                  []( u32 p_value ) { return FS::getDisplayName( p_value ); } } ) );
            // Forme
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 13, true, 31, 0,
                  [ & ]( ) { return _team[ _currentSelection ].getForme( ); },
                  [ & ]( u32 p_newValue ) { _team[ _currentSelection ].setForme( p_newValue ); },
                  [ & ]( u32 p_value ) {
                      return FS::getDisplayName( _team[ _currentSelection ].m_boxdata.m_speciesId,
                                                 p_value );
                  } } ) );
            // Ability
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 14, true, 3, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.getAbilitySlot( ); },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.setAbility( p_newValue );
                  },
                  [ & ]( u32 ) {
                      return FS::getAbilityName(
                          _team[ _currentSelection ].m_boxdata.getAbility( ) );
                  } } ) );
            // Shininess
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 23, false, 1, 0,
                  [ & ]( ) { return _team[ _currentSelection ].isShiny( ); },
                  [ & ]( u32 p_newValue ) {
                      while( p_newValue != _team[ _currentSelection ].isShiny( ) ) {
                          _team[ _currentSelection ].m_boxdata.m_pid = rand( );
                      }
                  },
                  []( u32 ) { return ""; } } ) );
            // Level
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 21, false, 100, 1,
                  [ & ]( ) { return _team[ _currentSelection ].m_level; },
                  [ & ]( u32 p_newValue ) { _team[ _currentSelection ].setLevel( p_newValue ); },
                  []( u32 ) { return ""; } } ) );
            // EXP
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 22, false, 1640000, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_experienceGained; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].setExperience( p_newValue );
                  },
                  []( u32 ) { return ""; } } ) );
            break;
        }
        case DESQUID_STATUS: {
            // Sleep
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 15, false, 7, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_status.m_isAsleep; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_status.m_isAsleep = p_newValue & 7;
                  },
                  []( u32 ) { return ""; } } ) );
            // Poison
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 16, false, 1, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_status.m_isPoisoned; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_status.m_isPoisoned = p_newValue & 1;
                  },
                  []( u32 ) { return ""; } } ) );
            // Burn
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 17, false, 8, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_status.m_isBurned; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_status.m_isBurned = p_newValue & 1;
                  },
                  []( u32 ) { return ""; } } ) );
            // Freeze
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 18, false, 8, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_status.m_isFrozen; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_status.m_isFrozen = p_newValue & 1;
                  },
                  []( u32 ) { return ""; } } ) );
            // Paralyzed
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 19, false, 8, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_status.m_isParalyzed; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_status.m_isParalyzed = p_newValue & 1;
                  },
                  []( u32 ) { return ""; } } ) );
            // Toxic
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 20, false, 8, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_status.m_isBadlyPoisoned; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_status.m_isBadlyPoisoned = p_newValue & 1;
                  },
                  []( u32 ) { return ""; } } ) );
            break;
        }
        case DESQUID_EGG: {
            // Egg
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 24, false, 1, 0,
                  [ & ]( ) { return _team[ _currentSelection ].isEgg( ); },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.setIsEgg( p_newValue );
                  },
                  []( u32 ) { return ""; } } ) );
            // Steps/Happiness
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 25, false, 255, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_steps; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.m_steps = ( p_newValue & 0xFF );
                  },
                  []( u32 ) { return ""; } } ) );
            // Got place
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 26, true, 6000, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_gotPlace; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.m_gotPlace = p_newValue & 0xFFFF;
                  },
                  []( u32 p_location ) { return FS::getLocation( p_location ); } } ) );
            // Hatch place
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 27, true, 6000, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_hatchPlace; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.m_hatchPlace = p_newValue & 0xFFFF;
                  },
                  []( u32 p_location ) { return FS::getLocation( p_location ); } } ) );
            // Id
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 28, false, 65535, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_oTId; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.m_oTId = p_newValue & 0xFFFF;
                  },
                  []( u32 ) { return ""; } } ) );
            // S.Id
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 28, false, 65535, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_oTSid; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.m_oTSid = p_newValue & 0xFFFF;
                  },
                  []( u32 ) { return ""; } } ) );
            break;
        }
        case DESQUID_NATURE: {
            // Nature
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 30, true, 24, 0,
                  [ & ]( ) { return u8( _team[ _currentSelection ].getNature( ) ); },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.setNature( pkmnNatures( p_newValue ) );
                  },
                  []( u32 p_value ) { return std::string( GET_STRING( 187 + p_value ) ); } } ) );
            for( u8 i = 0; i < 5; ++i ) {
                res.push_back( partyScreen::desquidItem(
                    { u16( FS::DESQUID_STRING + 31 + i ), true, 2, 0,
                      [ &, i ]( ) {
                          return NatMod[ u8( _team[ _currentSelection ].getNature( ) ) ][ i ] - 9;
                      },
                      [ &, i ]( u32 ) { return; },
                      []( u32 p_value ) {
                          return ( p_value == 0 ? "--" : ( p_value == 1 ? "   " : "++" ) );
                      } } ) );
            }
            break;
        }
        case DESQUID_IV: {
            // IV
            for( u8 i = 0; i < 6; ++i ) {
                res.push_back( partyScreen::desquidItem(
                    { u16( FS::DESQUID_STRING + 36 + i ), true, 31, 0,
                      [ &, i ]( ) { return _team[ _currentSelection ].IVget( i ); },
                      [ &, i ]( u32 p_newValue ) {
                          _team[ _currentSelection ].IVset( i, p_newValue & 31 );
                      },
                      []( u32 ) { return ""; } } ) );
            }
            break;
        }
        case DESQUID_EV: {
            // IV
            for( u8 i = 0; i < 6; ++i ) {
                res.push_back( partyScreen::desquidItem(
                    { u16( FS::DESQUID_STRING + 36 + i ), true, 252, 0,
                      [ &, i ]( ) { return _team[ _currentSelection ].EVget( i ); },
                      [ &, i ]( u32 p_newValue ) {
                          _team[ _currentSelection ].EVset( i, p_newValue );
                      },
                      []( u32 ) { return ""; } } ) );
            }
            break;
        }
        case DESQUID_MOVES: {
            // Moves
            for( u8 i = 0; i < 4; ++i ) {
                res.push_back( partyScreen::desquidItem(
                    { u16( FS::DESQUID_STRING + 42 ), true, 796, 0,
                      [ &, i ]( ) { return _team[ _currentSelection ].m_boxdata.m_moves[ i ]; },
                      [ &, i ]( u32 p_newValue ) {
                          _team[ _currentSelection ].m_boxdata.m_moves[ i ] = p_newValue;
                          _team[ _currentSelection ].PPupset( i, 0 );
                      },
                      []( u32 p_value ) { return FS::getMoveName( p_value ); } } ) );
            }
            // Held Item
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 43, true, 1278, 1,
                  [ & ]( ) { return _team[ _currentSelection ].getItem( ); },
                  [ & ]( u32 p_newValue ) { _team[ _currentSelection ].giveItem( p_newValue ); },
                  []( u32 p_value ) { return FS::getItemName( p_value ); } } ) );

            break;
        }
        case DESQUID_ITEM: {
            // Current HP
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 36, false, 999, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_stats.m_curHP; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_stats.m_curHP = std::min(
                          u16( p_newValue ), _team[ _currentSelection ].m_stats.m_maxHP );
                  },
                  []( u32 ) { return ""; } } ) );
            // Moves
            for( u8 i = 0; i < 4; ++i ) {
                res.push_back( partyScreen::desquidItem(
                    { u16( FS::DESQUID_STRING + 44 ), true, 99, 0,
                      [ &, i ]( ) { return _team[ _currentSelection ].m_boxdata.m_curPP[ i ]; },
                      [ &, i ]( u32 p_newValue ) {
                          BATTLE::moveData mdata = FS::getMoveData(
                              _team[ _currentSelection ].m_boxdata.m_moves[ i ] );
                          u8 dmx = u8( mdata.m_pp * ( 5 + _team[ _currentSelection ].PPupget( i ) )
                                       / 5 );
                          bool boost = !( mdata.m_flags & BATTLE::MF_NOPPBOOST );
                          u8   gmx   = s8( mdata.m_pp * ( 5 + ( boost ? 3 : 0 ) ) / 5 );
                          p_newValue = std::min( u8( p_newValue ), gmx );
                          if( p_newValue > dmx ) { _team[ _currentSelection ].PPupset( i, 3 ); }
                          _team[ _currentSelection ].m_boxdata.m_curPP[ i ] = p_newValue;
                      },
                      [ &, i ]( u32 ) {
                          return FS::getMoveName(
                              _team[ _currentSelection ].m_boxdata.m_moves[ i ] );
                      } } ) );
            }
            // Fateful encounter
            res.push_back( partyScreen::desquidItem(
                { FS::DESQUID_STRING + 45, false, 1, 0,
                  [ & ]( ) { return _team[ _currentSelection ].m_boxdata.m_fateful; },
                  [ & ]( u32 p_newValue ) {
                      _team[ _currentSelection ].m_boxdata.m_fateful = p_newValue;
                  },
                  []( u32 ) { return ""; } } ) );

            break;
        }

        default: break;
        }
        return res;
    }

#define UPDATE_VALUE( p_newValue ) \
    do { choices[ selectedLine ].m_counterUpdate( p_newValue ); } while( false );

    bool partyScreen::desquidWindow( desquidChoice p_choice ) {
        auto s1 = std::string( GET_STRING( FS::DESQUID_STRING ) );
        snprintf( BUFFER, 49, "%s: %s", s1.c_str( ),
                  GET_STRING( getTextForDesquidChoice( p_choice ) ) );
        _partyUI->select( _currentSelection, BUFFER );
        _ranges = _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );
        swiWaitForVBlank( );
        //_partyUI->drawPartyPkmnSub( _currentSelection, true, false, GET_STRING( FS::DESQUID_STRING
        //)
        //);
        _partyUI->showDesquidWindow( );

        auto choices       = getDesquidItemsForChoice( p_choice );
        u8   selectedLine  = 0;
        u8   selectedDigit = 0;
        bool editing       = false;
        u32  oldval        = 0;

        for( u8 i = 0; i < choices.size( ); ++i ) {
            _partyUI->drawDesquidItem( i, choices[ i ].computeString( ).c_str( ),
                                       choices[ i ].m_currentValue( ), choices[ i ].m_maxValue,
                                       i == selectedLine );
        }
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                UPDATE_VALUE( oldval );
                break;
            } else if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( editing ) {
                    editing = false;
                    UPDATE_VALUE( oldval );
                    _partyUI->drawDesquidItem( selectedLine,
                                               choices[ selectedLine ].computeString( ).c_str( ),
                                               choices[ selectedLine ].m_currentValue( ),
                                               choices[ selectedLine ].m_maxValue, true );
                    cooldown = COOLDOWN_COUNT;
                } else {
                    break;
                }
            } else if( pressed & KEY_A ) {
                if( !editing ) {
                    SOUND::playSoundEffect( SFX_CHOOSE );
                    oldval        = choices[ selectedLine ].m_currentValue( );
                    editing       = true;
                    selectedDigit = 0;
                    _partyUI->drawDesquidItem(
                        selectedLine, choices[ selectedLine ].computeString( ).c_str( ),
                        choices[ selectedLine ].m_currentValue( ),
                        choices[ selectedLine ].m_maxValue, true, selectedDigit );
                } else {
                    SOUND::playSoundEffect( SFX_SAVE );
                    _team[ _currentSelection ].recalculateStats( );
                    editing = false;
                    s1      = std::string( GET_STRING( FS::DESQUID_STRING ) );
                    snprintf( BUFFER, 49, "%s: %s", s1.c_str( ),
                              GET_STRING( getTextForDesquidChoice( p_choice ) ) );
                    _partyUI->select( _currentSelection, BUFFER );

                    for( u8 i = 0; i < choices.size( ); ++i ) {
                        _partyUI->drawDesquidItem( i, choices[ i ].computeString( ).c_str( ),
                                                   choices[ i ].m_currentValue( ),
                                                   choices[ i ].m_maxValue, i == selectedLine );
                    }
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( editing ) {
                    u32 change = 1;
                    for( u8 i = 0; i < selectedDigit; ++i, change *= 10 )
                        ;
                    if( choices[ selectedLine ].m_currentValue( )
                        == choices[ selectedLine ].m_minValue ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_maxValue );
                    } else if( choices[ selectedLine ].m_currentValue( )
                               < change + choices[ selectedLine ].m_minValue ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_minValue );
                    } else {
                        UPDATE_VALUE( choices[ selectedLine ].m_currentValue( ) - change );
                    }
                    _partyUI->drawDesquidItem(
                        selectedLine, choices[ selectedLine ].computeString( ).c_str( ),
                        choices[ selectedLine ].m_currentValue( ),
                        choices[ selectedLine ].m_maxValue, true, selectedDigit );
                } else {
                    _partyUI->drawDesquidItem( selectedLine,
                                               choices[ selectedLine ].computeString( ).c_str( ),
                                               choices[ selectedLine ].m_currentValue( ),
                                               choices[ selectedLine ].m_maxValue );
                    selectedLine = ( selectedLine + 1 ) % choices.size( );
                    _partyUI->drawDesquidItem( selectedLine,
                                               choices[ selectedLine ].computeString( ).c_str( ),
                                               choices[ selectedLine ].m_currentValue( ),
                                               choices[ selectedLine ].m_maxValue, true );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( editing ) {
                    u32 change = 1;
                    for( u8 i = 0; i < selectedDigit; ++i, change *= 10 )
                        ;
                    if( choices[ selectedLine ].m_currentValue( )
                        == choices[ selectedLine ].m_maxValue ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_minValue );
                    } else if( choices[ selectedLine ].m_currentValue( )
                               > choices[ selectedLine ].m_maxValue - change ) {
                        UPDATE_VALUE( choices[ selectedLine ].m_maxValue );
                    } else {
                        UPDATE_VALUE( choices[ selectedLine ].m_currentValue( ) + change );
                    }
                    _partyUI->drawDesquidItem(
                        selectedLine, choices[ selectedLine ].computeString( ).c_str( ),
                        choices[ selectedLine ].m_currentValue( ),
                        choices[ selectedLine ].m_maxValue, true, selectedDigit );
                } else {
                    _partyUI->drawDesquidItem( selectedLine,
                                               choices[ selectedLine ].computeString( ).c_str( ),
                                               choices[ selectedLine ].m_currentValue( ),
                                               choices[ selectedLine ].m_maxValue );
                    selectedLine = ( selectedLine + choices.size( ) - 1 ) % choices.size( );
                    _partyUI->drawDesquidItem( selectedLine,
                                               choices[ selectedLine ].computeString( ).c_str( ),
                                               choices[ selectedLine ].m_currentValue( ),
                                               choices[ selectedLine ].m_maxValue, true );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( editing && GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                u8 numDig = 0;
                for( u32 cur = choices[ selectedLine ].m_maxValue; cur > 0; ++numDig, cur /= 10 )
                    ;
                selectedDigit = ( selectedDigit + 1 ) % numDig;
                _partyUI->drawDesquidItem(
                    selectedLine, choices[ selectedLine ].computeString( ).c_str( ),
                    choices[ selectedLine ].m_currentValue( ), choices[ selectedLine ].m_maxValue,
                    true, selectedDigit );

                cooldown = COOLDOWN_COUNT;
            } else if( editing && GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                u8 numDig = 0;
                for( u32 cur = choices[ selectedLine ].m_maxValue; cur > 0; ++numDig, cur /= 10 )
                    ;
                selectedDigit = ( selectedDigit + numDig - 1 ) % numDig;
                _partyUI->drawDesquidItem(
                    selectedLine, choices[ selectedLine ].computeString( ).c_str( ),
                    choices[ selectedLine ].m_currentValue( ), choices[ selectedLine ].m_maxValue,
                    true, selectedDigit );

                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }

        _partyUI->hideDesquidWindow( );
        return false;
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
                std::memmove( &_team[ _currentSelection ], &_team[ _currentSelection + 1 ],
                              ( _teamLength - _currentSelection - 1 ) * sizeof( pokemon ) );
            }
            std::memset( &_team[ --_teamLength ], 0, sizeof( pokemon ) );
            _partyUI->updateTeamLength( _teamLength );
            return true;
        case DESQUID_HEAL: _team[ _currentSelection ].heal( ); return false;
        default: break;
        }
        return desquidWindow( p_choice );
    }
#endif

    bool partyScreen::focus( ) {
        u16 c[ 12 ] = { 0 };
        for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
            c[ i ] = getTextForChoice( _currentChoices[ i ] );
        }
        if( _currentChoiceSelection >= 6 ) {
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c + 6, std::min( size_t( 6 ), _currentChoices.size( ) - 6 ),
                false, true, _currentChoiceSelection % 6 );
        } else {
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c, std::min( size_t( 6 ), _currentChoices.size( ) ),
                _currentChoices.size( ) > 6, false, _currentChoiceSelection % 6 );
        }
        bool ex  = false;
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                ex = _allowCancel;
                break;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown = COOLDOWN_COUNT;
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( ( _currentChoiceSelection & 1 ) && _currentChoiceSelection < 6
                    && _currentChoices.size( ) > 6 ) {
                    // Switch to second page
                    _currentChoiceSelection += 5;
                    if( _currentChoiceSelection >= _currentChoices.size( ) ) {
                        _currentChoiceSelection = 6;
                    }
                    _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
                    _ranges = _partyUI->drawPartyPkmnChoice(
                        _currentSelection, c + 6,
                        std::min( size_t( 6 ), _currentChoices.size( ) - 6 ), false, true,
                        _currentChoiceSelection % 6 );
                } else if( ( _currentChoiceSelection ^ 1 ) < _currentChoices.size( ) ) {
                    selectChoice( _currentChoiceSelection ^ 1 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( !( _currentChoiceSelection & 1 ) && _currentChoiceSelection >= 6 ) {
                    // Switch to first page
                    _currentChoiceSelection -= 5;
                    _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
                    _ranges = _partyUI->drawPartyPkmnChoice( _currentSelection, c, 6, true, false,
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
            } else if( pressed & KEY_A ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                ex = executeChoice( _currentChoices[ _currentChoiceSelection ] ) && _allowCancel;
                cooldown = COOLDOWN_COUNT;
                break;
            }

            if( handleTouch( true ) == 2 ) {
                ex = _allowCancel;
                break;
            }

            swiWaitForVBlank( );
        }

        for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
            c[ i ] = getTextForChoice( _currentChoices[ i ] );
        }
        if( _currentChoices.size( ) <= 6 || _currentChoiceSelection >= 6 ) {
            _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
        }

        // Selecting CANCEL should result in a reset of the saved position
        _currentChoiceSelection %= _currentChoices.size( ) - 1;
        if( _currentChoiceSelection >= 6 && _currentChoices.size( ) > 7 ) {
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c + 6, std::min( size_t( 6 ), _currentChoices.size( ) - 6 - 1 ),
                false, true, 255 );
        } else {
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c, std::min( size_t( 6 ), _currentChoices.size( ) - 1 ),
                _currentChoices.size( ) > 7, false, 255 );
        }
        return ex;
    }

#ifdef DESQUID
    bool STS::partyScreen::desquid( u8 p_selectedIdx ) {
        u16  c[ 12 ] = { 0 };
        auto choices = computeDesquidChoices( );
        _partyUI->drawPartyPkmnSub( p_selectedIdx, true, false );
        _currentChoiceSelection = 0;

        for( u8 i = 0; i < choices.size( ); i++ ) {
            c[ i ] = getTextForDesquidChoice( choices[ i ] );
        }
        _ranges = _partyUI->drawPartyPkmnChoice( p_selectedIdx, c,
                                                 std::min( size_t( 6 ), choices.size( ) ),
                                                 choices.size( ) > 6, false, 0 );

        bool ex  = false;
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                ex = false;
                break;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                _partyUI->select( _currentSelection );
                cooldown = COOLDOWN_COUNT;
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
                    _ranges = _partyUI->drawPartyPkmnChoice(
                        p_selectedIdx, c + 6, std::min( size_t( 6 ), choices.size( ) - 6 ), false,
                        true, _currentChoiceSelection % 6 );
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
                    _ranges = _partyUI->drawPartyPkmnChoice( p_selectedIdx, c, 6, true, false,
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
            } else if( pressed & KEY_A ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                if( ( ex = executeDesquidChoice( choices[ _currentChoiceSelection ] ) ) ) { break; }
                _partyUI->select( _currentSelection );

                if( _currentChoiceSelection >= 6 ) {
                    _ranges = _partyUI->drawPartyPkmnChoice(
                        p_selectedIdx, c + 6, std::min( size_t( 6 ), choices.size( ) - 6 ), false,
                        true, _currentChoiceSelection % 6 );
                } else {
                    _ranges = _partyUI->drawPartyPkmnChoice(
                        p_selectedIdx, c, std::min( size_t( 6 ), choices.size( ) ),
                        choices.size( ) > 6, false, _currentChoiceSelection % 6 );
                }
                cooldown = COOLDOWN_COUNT;
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

            u16 c[ 6 ] = { 0 };
            for( u8 i = 0; i < std::min( size_t( 6 ), _currentChoices.size( ) ); i++ ) {
                c[ i ] = getTextForChoice( _currentChoices[ i ] );
            }

            // CANCEL is only displayed in SINGLE mode
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c, std::min( size_t( 6 ), _currentChoices.size( ) - 1 ),
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
                && ( _team[ _currentSelection ].m_stats.m_curHP || _faintSelect )
                && _currentSelection >= _inBattle ) {
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
                //  _currentChoices.push_back( USE_ITEM );
            }
            if( _allowMoveSelection && !_team[ _currentSelection ].isEgg( ) ) {
                for( u8 i = 0; i < 4; i++ ) {
                    if( _team[ _currentSelection ].m_boxdata.m_moves[ i ]
                        && BATTLE::isFieldMove(
                            _team[ _currentSelection ].m_boxdata.m_moves[ i ] ) ) {
                        for( u8 j = 0; j < 2; ++j ) {
                            if( BATTLE::possible( _team[ _currentSelection ].m_boxdata.m_moves[ i ],
                                                  j ) ) {
                                _currentChoices.push_back( choice( FIELD_MOVE_1 + i ) );
                                break;
                            }
                        }
                    }
                }
            }
        }
        if( _teamLength > 1 && !_inBattle ) { _currentChoices.push_back( SWAP ); }
        if( _swapSelection == 255 ) {
            if( _allowDex && !_team[ _currentSelection ].isEgg( ) ) {
                _currentChoices.push_back( DEX_ENTRY );
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
        case STS::partyScreen::STATUS: {
            statusScreen::result stsres;
            u8                   curStsPage = 0;
            u8                   oldchoice  = _currentSelection;
            do {
                statusScreen sts
                    = statusScreen( _team + _currentSelection, _teamLength > 1, _teamLength > 1 );
                stsres     = sts.run( curStsPage );
                curStsPage = sts.currentPage( );
                if( stsres == statusScreen::NEXT_PKMN ) {
                    _currentSelection = ( _currentSelection + 1 ) % _teamLength;
                } else if( stsres == statusScreen::PREV_PKMN ) {
                    _currentSelection = ( _currentSelection + _teamLength - 1 ) % _teamLength;
                }
            } while( stsres != statusScreen::BACK && stsres != statusScreen::EXIT );

            if( stsres == statusScreen::EXIT && _allowCancel ) {
                return true;
            } else {
                stsres = statusScreen::BACK;
            }
            computeSelectionChoices( );
            _partyUI->init( oldchoice );
            _partyUI->select( _currentSelection );
            _frame = 0;

            break;
        }
        case STS::partyScreen::GIVE_ITEM: {
            BAG::bagViewer bv  = BAG::bagViewer( _team, BAG::bagViewer::GIVE_TO_PKMN );
            u16            itm = bv.getItem( );
            computeSelectionChoices( );
            _partyUI->init( _currentSelection );
            _frame = 0;
            if( itm ) {
                if( _team[ _currentSelection ].getItem( ) ) {
                    auto curItm = _team[ _currentSelection ].getItem( );
                    SAVE::SAV.getActiveFile( ).m_bag.insert(
                        BAG::toBagType( FS::getItemData( curItm ).m_itemType ), curItm, 1 );
                }
                _partyUI->select( _currentSelection );
                _ranges = _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );

                sprintf( BUFFER, GET_STRING( 334 ), FS::getItemName( itm ).c_str( ),
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
            _ranges = _partyUI->drawPartyPkmnChoice( 0, 0, 0, false, false );

            sprintf( BUFFER, GET_STRING( 101 ), FS::getItemName( acI ).c_str( ),
                     _team[ _currentSelection ].m_boxdata.m_name );
            _partyUI->printMessage( BUFFER, acI );
            SAVE::SAV.getActiveFile( ).m_bag.insert(
                BAG::toBagType( FS::getItemData( acI ).m_itemType ), acI, 1 );
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
        case STS::partyScreen::DEX_ENTRY: {
            DEX::dex( ).run(
                _team[ _currentSelection ].getSpecies( ), _team[ _currentSelection ].getForme( ),
                _team[ _currentSelection ].isShiny( ), _team[ _currentSelection ].isFemale( ) );

            computeSelectionChoices( );
            _partyUI->init( _currentSelection );
            _partyUI->select( _currentSelection );
            _frame = 0;
            break;
        }
#ifdef DESQUID
        case STS::partyScreen::_DESQUID:
            if( desquid( _currentSelection ) ) {
                computeSelectionChoices( );
                _partyUI->init( _currentSelection );
                _frame = 0;
            }
            break;
#endif
        case STS::partyScreen::CANCEL:
            if( _swapSelection != 255 ) {
                _partyUI->unswap( _swapSelection );
                _swapSelection = 255;
                computeSelectionChoices( );
            }
            break;
        default: break;
        }
        return false;
    }

    void STS::partyScreen::waitForInteract( ) {
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_A ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    _partyUI->animate( _frame++ );
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                break;
            }
        }
    }

    u8 partyScreen::handleTouch( bool p_autoSel ) {
        u16 c[ 12 ] = { 0 };
        for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
            c[ i ] = getTextForChoice( _currentChoices[ i ] );
        }
        u8 change = false;
        for( auto i : _partyUI->getTouchPositions( ) ) {
            if( i.first.inRange( touch ) ) {
                swiWaitForVBlank( );
                if( i.second < 6 ) {
                    select( i.second );
                    change = 1;
                } else if( i.second == EXIT_TARGET ) {
                    change = 2;
                }
                while( touch.px || touch.py ) {
                    _partyUI->animate( _frame++ );
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }
            }
        }
        if( change ) {
            for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
                c[ i ] = getTextForChoice( _currentChoices[ i ] );
            }
            _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c + ( _currentChoiceSelection / 6 * 6 ),
                std::min( size_t( 6 ), _currentChoices.size( ) - ( _currentChoiceSelection / 6 * 6 )
                                           - !( p_autoSel ) ),
                _currentChoiceSelection < 6 && _currentChoices.size( ) > size_t( 7 + p_autoSel ),
                _currentChoiceSelection >= 6, p_autoSel ? _currentChoiceSelection : 255 );
            return change;
        }

        u8   res = 0;
        bool bad = false;
        for( auto i : _ranges ) {
            if( i.first.inRange( touch ) ) {
                swiWaitForVBlank( );
                if( _currentChoiceSelection >= 6 && i.second < 6 ) {
                    _currentChoiceSelection = i.second + 6;
                } else if( i.second < 6 ) {
                    _currentChoiceSelection = i.second;
                }
                _partyUI->drawPartyPkmnChoice(
                    _currentSelection, c + ( _currentChoiceSelection / 6 * 6 ),
                    std::min( size_t( 6 ), _currentChoices.size( )
                                               - ( _currentChoiceSelection / 6 * 6 )
                                               - !( p_autoSel ) ),
                    _currentChoiceSelection < 6
                        && _currentChoices.size( ) > size_t( 7 + p_autoSel ),
                    _currentChoiceSelection >= 6, i.second );

                while( touch.px || touch.py ) {
                    _partyUI->animate( _frame++ );
                    swiWaitForVBlank( );
                    if( !i.first.inRange( touch ) ) {
                        bad = true;
                        break;
                    }

                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }
                res = 0;
                if( !bad ) {
                    if( i.second < 6 ) {
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        res = 1
                              + ( executeChoice( _currentChoices[ _currentChoiceSelection ] )
                                  && _allowCancel );
                        for( u8 j = 0; j < _currentChoices.size( ); ++j ) {
                            c[ j ] = getTextForChoice( _currentChoices[ j ] );
                        }
                    } else if( i.second == NEXT_PAGE_TARGET ) {
                        _currentChoiceSelection = 6;
                        _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
                        bad = true;
                    } else if( i.second == PREV_PAGE_TARGET ) {
                        _currentChoiceSelection = 0;
                        _partyUI->drawPartyPkmnSub( _currentSelection, true, false );
                        bad = true;
                    }
                }
                break;
            }
        }
        if( bad ) {
            _ranges = _partyUI->drawPartyPkmnChoice(
                _currentSelection, c + ( _currentChoiceSelection / 6 * 6 ),
                std::min( size_t( 6 ), _currentChoices.size( ) - ( _currentChoiceSelection / 6 * 6 )
                                           - !( p_autoSel ) ),
                _currentChoiceSelection < 6 && _currentChoices.size( ) > size_t( 7 + p_autoSel ),
                _currentChoiceSelection >= 6, p_autoSel ? _currentChoiceSelection : 255 );
        }

        if( res ) {
            swiWaitForVBlank( );
            _partyUI->drawPartyPkmnSub( _currentSelection, true, false );

            // Selecting CANCEL should result in a reset of the saved position
            _currentChoiceSelection %= _currentChoices.size( ) - !( p_autoSel );
            if( _currentChoiceSelection >= 6
                && _currentChoices.size( ) > size_t( 6 + p_autoSel ) ) {
                _ranges = _partyUI->drawPartyPkmnChoice(
                    _currentSelection, c + 6,
                    std::min( size_t( 6 ), _currentChoices.size( ) - 6 - !( p_autoSel ) ), false,
                    true, p_autoSel ? _currentChoiceSelection : 255 );
            } else {
                _ranges = _partyUI->drawPartyPkmnChoice(
                    _currentSelection, c,
                    std::min( size_t( 6 ), _currentChoices.size( ) - !( p_autoSel ) ),
                    _currentChoices.size( ) > size_t( 7 + p_autoSel ), false,
                    p_autoSel ? _currentChoiceSelection : 255 );
            }
        }

        return res;
    }

    partyScreen::result partyScreen::run( u8 p_initialSelection ) {
        _partyUI->init( p_initialSelection );
        _currentSelection                  = 255;
        _currentChoiceSelection            = 0;
        _currentMarksOrMove.m_selectedMove = 0;
        select( p_initialSelection );
        _frame   = 0;
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            _partyUI->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );
            u8 tc   = 0;

            if( ( pressed & KEY_X ) && _allowCancel ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                break;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( _swapSelection != 255 ) {
                    _partyUI->unswap( _swapSelection );
                    _swapSelection = 255;
                    computeSelectionChoices( );
                    _partyUI->select( _currentSelection );
                    u16 c[ 12 ] = { 0 };
                    for( u8 i = 0; i < _currentChoices.size( ); i++ ) {
                        c[ i ] = getTextForChoice( _currentChoices[ i ] );
                    }
                    _ranges = _partyUI->drawPartyPkmnChoice(
                        _currentSelection, c, std::min( size_t( 6 ), _currentChoices.size( ) ),
                        _currentChoices.size( ) > 7, false );

                } else if( _allowCancel ) {
                    break;
                }
                cooldown = COOLDOWN_COUNT;
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
            } else if( ( pressed & KEY_A ) || ( tc = handleTouch( false ) ) ) {
                if( pressed & KEY_A ) { SOUND::playSoundEffect( SFX_CHOOSE ); }
                if( ( tc == 2 || ( tc == 0 && focus( ) ) ) && _allowCancel ) { // User pressed X
                    _currentMarksOrMove.m_selectedMove = 0;
                    break;
                }
                if( checkReturnCondition( ) ) {
                    // Make the player confirm the set of selected pkmn, but not the selected move
                    if( !_toSelect || !_selectConfirm || confirmSelection( ) ) {
                        break;
                    } else {
                        unmark( _currentSelection );
                        u8 tmp            = _currentSelection;
                        _currentSelection = 255;
                        select( tmp );
                    }
                }
                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }

        return _currentMarksOrMove;
    }
} // namespace STS
