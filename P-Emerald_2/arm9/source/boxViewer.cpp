/*
Pokémon neo
------------------------------

file        : boxViewer.cpp
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

#include "boxViewer.h"
#include "boxUI.h"
#include "defines.h"
#include "keyboard.h"
#include "messageBox.h"
#include "uio.h"
#include "bagViewer.h"

#include "sound.h"

#include "saveGame.h"
#include "statusScreen.h"
#include "statusScreenUI.h"

namespace BOX {
// #define THRESHOLD 20

#define PARTY_BUTTON ( MAX_PKMN_PER_BOX + 10 )
#define BOXNAME_BUTTON ( MAX_PKMN_PER_BOX + 20 )

    void boxViewer::run( ) {
        _boxUI = boxUI( );
        _boxUI.init( );

        _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );

        _selectedIdx = (u8) -1;
        memset( &_heldPkmn, 0, sizeof( pokemon ) );

        _showParty = false;
        _mode = STATUS;

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( _heldPkmn.getSpecies( ) ) { returnPkmn( ); }
                return;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( !_heldPkmn.getSpecies( ) ) {
                    return;
                } else {
                    returnPkmn( );
                    _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx );
                }
            }
            if( pressed & KEY_SELECT ) {
                _mode = mode( u8( _mode ) ^ 1 );
                _boxUI.setMode( u8( _mode ) );
            }

            if( GET_KEY_COOLDOWN( KEY_L ) ) {
                // previous box
                SAVE::SAV.getActiveFile( ).m_curBox
                    = ( SAVE::SAV.getActiveFile( ).m_curBox + MAX_BOXES - 1 ) % MAX_BOXES;
                _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                select( _selectedIdx );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_R ) ) {
                // next box
                SAVE::SAV.getActiveFile( ).m_curBox
                    = ( SAVE::SAV.getActiveFile( ).m_curBox + 1 ) % MAX_BOXES;
                _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                select( _selectedIdx );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx + 6 >= MAX_PKMN_PER_BOX && _selectedIdx < MAX_PKMN_PER_BOX ) {
                    select( PARTY_BUTTON );
                } else if( _selectedIdx == PARTY_BUTTON ) {
                    select( BOXNAME_BUTTON );
                } else if( _selectedIdx == BOXNAME_BUTTON ) {
                    select( 0 );
                } else {
                    select( 6 + _selectedIdx );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < 6 ) {
                    select( BOXNAME_BUTTON );
                } else if( _selectedIdx == BOXNAME_BUTTON ) {
                    select( PARTY_BUTTON );
                } else if( _selectedIdx == PARTY_BUTTON ) {
                    select( MAX_PKMN_PER_BOX - 1 );
                } else {
                    select( _selectedIdx - 6 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX ) {
                    select( ( _selectedIdx + 1 ) % MAX_PKMN_PER_BOX );
                } else if( _selectedIdx == BOXNAME_BUTTON ) {
                    // switch to next box
                    SAVE::SAV.getActiveFile( ).m_curBox
                        = ( SAVE::SAV.getActiveFile( ).m_curBox + 1 ) % MAX_BOXES;
                    _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX ) {
                    select( ( _selectedIdx + MAX_PKMN_PER_BOX - 1 ) % MAX_PKMN_PER_BOX );
                } else if( _selectedIdx == BOXNAME_BUTTON ) {
                    // switch to prev box
                    SAVE::SAV.getActiveFile( ).m_curBox
                        = ( SAVE::SAV.getActiveFile( ).m_curBox + MAX_BOXES - 1 ) % MAX_BOXES;
                    _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( pressed & KEY_A ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX ) { // player selects a pkmn
                    auto tmp = getPkmn( _selectedIdx );
                    if( tmp != nullptr && tmp->getSpecies( ) ) {
                        if( _mode != MOVE && !_heldPkmn.getSpecies( ) ) {
                            u8 res = runStatusChoice( );
                            if( !res ) {
                                cooldown = COOLDOWN_COUNT;
                                continue;
                            }
                            if( res == 255 ) { // player pressed X
                                return;
                            }
                        }
                    }
                    takePkmn( _selectedIdx, _mode == MOVE );
                } else if( _selectedIdx == BOXNAME_BUTTON ) {
                    // Change box name / wall paper etc.
                    runBoxSettings( );
                } else if( _selectedIdx == PARTY_BUTTON ) {
                    runParty( );
                }
                cooldown = COOLDOWN_COUNT;
            }

            /*
            auto ranges = _boxUI.getInteractions( );
            for( u8 j = 0; j < ranges.size( ); ++j ) {
                auto i = ranges[ j ].m_touch;
                if( IN_RANGE_I( touch, i ) ) {
                    u8 c = 0;
                    loop( ) {
                        scanKeys( );
                        swiWaitForVBlank( );
                        touchRead( &touch );
                        if( c++ == TRESHOLD ) {
                            _selectedIdx = j;
                            takePkmn( _selectedIdx );
                            IO::waitForTouchUp( i );
                            break;
                        }
                        if( TOUCH_UP ) {
                            switch( ranges[ j ].m_buttonType ) {
                                case boxUI::BUTTON_PKMN:
                                    select( ranges[ j ].m_param );
                                    if( _mode == MOVE || runStatusChoice( ) ) {
                                        takePkmn( _selectedIdx );
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                        }
                        if( !IN_RANGE_I( touch, i ) ) break;
                    }
                }
            }
            */
            swiWaitForVBlank( );
        }
    }

    void boxViewer::select( u8 p_index ) {
        _selectedIdx = p_index;

        if( _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
            // Pkmn is selected.
            _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx );
        } else if( _selectedIdx == BOXNAME_BUTTON ) {
            _boxUI.selectButton( boxUI::BUTTON_BOX_NAME );
        } else if( _selectedIdx == PARTY_BUTTON ) {
            _boxUI.selectButton( boxUI::BUTTON_PARTY );
        }
    }

    void boxViewer::takePkmn( u8 p_index, bool p_continuousSwap ) {
        pokemon tmp;
        if( p_index < MAX_PKMN_PER_BOX ) {
            if ( getPkmn( p_index ) == nullptr ) { return; }
            tmp = pokemon( *getPkmn( p_index ) );
        } else {
            if ( p_index >= MAX_PKMN_PER_BOX + 6 ) { return; }
            auto ptmp = SAVE::SAV.getActiveFile( ).getTeamPkmn( p_index - MAX_PKMN_PER_BOX );
            if( ptmp == nullptr ) { // slot is empty
                std::memset( &tmp, 0, sizeof( pokemon ) );
            } else {
                tmp = *ptmp;
            }
        }

        if( tmp.getSpecies( ) ) {
            if( _heldPkmn.getSpecies( ) ) {
                // Swap pkmn
                std::swap( tmp, _heldPkmn );
                setPkmn( p_index, &tmp );
                _boxUI.updatePkmn( getPkmn( p_index ), p_index );
                if( !p_continuousSwap ) {
                    // Return new pkmn
                    returnPkmn( );
                } else {
                    _boxUI.setNewHeldPkmn( &_heldPkmn.m_boxdata, p_index );
                }
            } else {
                // Pick up pkmn
                _heldPkmn = tmp;
                setPkmn( p_index, (pokemon*) nullptr );
                _boxUI.updatePkmn( getPkmn( p_index ), p_index );
                _heldPkmnPos = std::pair<u8, u8>( SAVE::SAV.getActiveFile( ).m_curBox, p_index );
                _boxUI.setNewHeldPkmn( &_heldPkmn.m_boxdata, p_index );
            }
            _boxUI.hoverPkmn( getPkmn( p_index ), p_index, false );
        } else {
            // put down pkmn
            setPkmn( p_index, &_heldPkmn );
            if( p_index < MAX_PKMN_PER_BOX ) {
                _boxUI.updatePkmn( getPkmn( p_index ), p_index );
            } else {
                u8 st = SAVE::SAV.getActiveFile( ).consolidatePkmn( );
                if( st == u8( -1 ) ) {
                    _boxUI.updatePkmn( getPkmn( p_index ), p_index );
                } else {
                    for( ; st < 6; ++st ) {
                        _boxUI.updatePkmn( getPkmn( MAX_PKMN_PER_BOX + st ),
                                MAX_PKMN_PER_BOX + st );
                    }
                }
            }
            memset( &_heldPkmn, 0, sizeof( pokemon ) );
            _heldPkmnPos = std::pair<u8, u8>( -1, -1 );
            _boxUI.hoverPkmn( getPkmn( p_index ), p_index );
            _boxUI.setNewHeldPkmn( nullptr, p_index );
        }
    }

    boxPokemon* boxViewer::getPkmn( u8 p_position ) {
        if( p_position < MAX_PKMN_PER_BOX ) {
            return &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ p_position ];
        } else if( p_position < MAX_PKMN_PER_BOX + 6 ) {
            return &SAVE::SAV.getActiveFile( ).getTeamPkmn( p_position - MAX_PKMN_PER_BOX )
                ->m_boxdata;
        }
        return nullptr;
    }

    boxPokemon* boxViewer::getPkmn( std::pair<u8, u8> p_position ) {
        if( p_position.second < MAX_PKMN_PER_BOX ) {
            return &SAVE::SAV.getActiveFile( ).
                m_storedPokemon[ p_position.first ][ p_position.second ];
        } else if( p_position.second < MAX_PKMN_PER_BOX + 6 ) {
            return &SAVE::SAV.getActiveFile( ).getTeamPkmn( p_position.second - MAX_PKMN_PER_BOX )
                ->m_boxdata;
        }
        return nullptr;
    }

    void boxViewer::setPkmn( u8 p_position, boxPokemon* p_pokemon ) {
        if( p_position < MAX_PKMN_PER_BOX ) {
            if( p_pokemon != nullptr ) {
                *getPkmn( p_position ) = *p_pokemon;
            } else {
                std::memset( getPkmn( p_position ), 0, sizeof( boxPokemon ) );
            }
        } else if( p_position < MAX_PKMN_PER_BOX + 6 ) {
            SAVE::SAV.getActiveFile( ).setTeamPkmn( p_position - MAX_PKMN_PER_BOX,
                    p_pokemon );
        }
    }

    void boxViewer::setPkmn( u8 p_position, pokemon* p_pokemon ) {
        if( p_position < MAX_PKMN_PER_BOX ) {
            if( p_pokemon != nullptr ) {
                *getPkmn( p_position ) = p_pokemon->m_boxdata;
            } else {
                std::memset( getPkmn( p_position ), 0, sizeof( boxPokemon ) );
            }
        } else if( p_position < MAX_PKMN_PER_BOX + 6 ) {
            SAVE::SAV.getActiveFile( ).setTeamPkmn( p_position - MAX_PKMN_PER_BOX,
                    p_pokemon );
        }
    }

    void boxViewer::setPkmn( std::pair<u8, u8> p_position, boxPokemon* p_pokemon ) {
        if( p_position.second < MAX_PKMN_PER_BOX ) {
            if( p_pokemon != nullptr ) {
                *getPkmn( p_position ) = *p_pokemon;
            } else {
                std::memset( getPkmn( p_position ), 0, sizeof( boxPokemon ) );
            }
        } else if( p_position.second < MAX_PKMN_PER_BOX + 6 ) {
            SAVE::SAV.getActiveFile( ).setTeamPkmn( p_position.second - MAX_PKMN_PER_BOX,
                    p_pokemon );
        }
    }

    void boxViewer::setPkmn( std::pair<u8, u8> p_position, pokemon* p_pokemon ) {
        if( p_position.second < MAX_PKMN_PER_BOX ) {
            if( p_pokemon != nullptr ) {
                *getPkmn( p_position ) = p_pokemon->m_boxdata;
            } else {
                std::memset( getPkmn( p_position ), 0, sizeof( boxPokemon ) );
            }
        } else if( p_position.second < MAX_PKMN_PER_BOX + 6 ) {
            SAVE::SAV.getActiveFile( ).setTeamPkmn( p_position.second - MAX_PKMN_PER_BOX,
                    p_pokemon );
        }
    }

    void boxViewer::returnPkmn( ) {
        if( !_heldPkmn.getSpecies( ) ) { return; } // Nothing to do.
        setPkmn( _heldPkmnPos, &_heldPkmn );
        std::memset( &_heldPkmn, 0, sizeof( pokemon ) );


        if( _heldPkmnPos.first == SAVE::SAV.getActiveFile( ).m_curBox ) {
             _boxUI.setNewHeldPkmn( nullptr, _heldPkmnPos.second );
             _boxUI.updatePkmn( getPkmn( _heldPkmnPos.second ), _heldPkmnPos.second );
        } else {
             _boxUI.setNewHeldPkmn( nullptr, _selectedIdx );
         }

        _heldPkmnPos = { 0, 0 };
    }

    bool boxViewer::runParty( ) {
        _boxUI.showParty( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) );
        _selectedIdx = MAX_PKMN_PER_BOX;
        select( MAX_PKMN_PER_BOX );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( _heldPkmn.getSpecies( ) ) { returnPkmn( ); }
                return true;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( !_heldPkmn.getSpecies( ) ) {
                    break;
                } else {
                    returnPkmn( );
                    _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx );
                }
            }
            if( pressed & KEY_SELECT ) {
                _mode = mode( u8( _mode ) ^ 1 );
                _boxUI.setMode( u8( _mode ) );
            }

            if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( !_heldPkmn.getSpecies( ) &&
                        _selectedIdx + 2
                        >= MAX_PKMN_PER_BOX + SAVE::SAV.getActiveFile( ).getTeamPkmnCount( )
                        && _selectedIdx
                        < MAX_PKMN_PER_BOX + SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) ) {
                    select( PARTY_BUTTON );
                } else if( _heldPkmn.getSpecies( ) && _selectedIdx + 2 >= MAX_PKMN_PER_BOX + 6
                        && _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
                    select( PARTY_BUTTON );
                } else if( _selectedIdx == PARTY_BUTTON ) {
                    select( MAX_PKMN_PER_BOX );
                } else {
                    select( 2 + _selectedIdx );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX + 2 ) {
                    select( PARTY_BUTTON );
                } else if( !_heldPkmn.getSpecies( ) && _selectedIdx == PARTY_BUTTON ) {
                    select( MAX_PKMN_PER_BOX + SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) - 1 );
                } else if( _heldPkmn.getSpecies( ) && _selectedIdx == PARTY_BUTTON ) {
                    select( MAX_PKMN_PER_BOX + 5 );
                } else {
                    select( _selectedIdx - 2 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( !_heldPkmn.getSpecies( ) && _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
                    select( MAX_PKMN_PER_BOX +
                            ( _selectedIdx - MAX_PKMN_PER_BOX + 1 )
                            % SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) );
                } else if( _heldPkmn.getSpecies( ) && _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
                    select( MAX_PKMN_PER_BOX + ( _selectedIdx - MAX_PKMN_PER_BOX + 1 ) % 6 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( !_heldPkmn.getSpecies( ) && _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
                    select( MAX_PKMN_PER_BOX + ( _selectedIdx - MAX_PKMN_PER_BOX +
                                SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) - 1 )
                            % SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) );
                } else if( _heldPkmn.getSpecies( ) && _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
                    select( MAX_PKMN_PER_BOX + ( _selectedIdx - MAX_PKMN_PER_BOX + 5 ) % 6 );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( pressed & KEY_A ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX + 6 ) { // player selects a pkmn
                    auto tmp = getPkmn( _selectedIdx );
                    if( tmp != nullptr && tmp->getSpecies( ) ) {
                        if( _mode != MOVE && !_heldPkmn.getSpecies( ) ) {
                            u8 res = runStatusChoice( );
                            if( !res ) {
                                cooldown = COOLDOWN_COUNT;
                                continue;
                            }
                            if( res == 255 ) { // player pressed X
                                return true;
                            }
                        }
                    }
                    takePkmn( _selectedIdx, _mode == MOVE );
                } else if( _selectedIdx == PARTY_BUTTON ) {
                    break;
                }
                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }
        _boxUI.hideParty( );
        return false;
    }

    u8 boxViewer::runStatusChoice( ) {
        pokemon curSel;
        auto tmp = getPkmn( _selectedIdx );
        if( _selectedIdx < MAX_PKMN_PER_BOX && tmp != nullptr ) {
            curSel = pokemon( *tmp );
        } else if( _selectedIdx < MAX_PKMN_PER_BOX + 6 && tmp != nullptr ){
            auto ptmp = SAVE::SAV.getActiveFile( ).getTeamPkmn(
                    _selectedIdx - MAX_PKMN_PER_BOX );
            if( ptmp != nullptr ) { curSel = *ptmp; }
        } else {
            _boxUI.selectPkmn( nullptr, 0 );
            return 0;
        }
        _boxUI.selectPkmn( tmp, _selectedIdx );

        u8 selectedBtn = 0;
        std::vector<boxUI::button> btns = { boxUI::BUTTON_PKMN_MOVE, boxUI::BUTTON_PKMN_STATUS };

        if( !curSel.isEgg( ) ) {
            btns.push_back( boxUI::BUTTON_PKMN_RELEASE );
            btns.push_back( boxUI::BUTTON_PKMN_GIVE_ITEM );
            if( curSel.getItem( ) ) {
                btns.push_back( boxUI::BUTTON_PKMN_TAKE_ITEM );
            }
        }
        btns.push_back( boxUI::BUTTON_PKMN_CANCEL );

        _boxUI.selectButton( btns[ selectedBtn ] );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) ) {
                cooldown = COOLDOWN_COUNT;
                if( !_heldPkmn.getSpecies( ) ) {
                    SOUND::playSoundEffect( SFX_CANCEL );
                    _boxUI.selectPkmn( nullptr, 0 );
                    return 255;
                }
            }
            if( pressed & KEY_B ) {
                cooldown = COOLDOWN_COUNT;
                SOUND::playSoundEffect( SFX_CANCEL );
                break;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                _boxUI.selectButton( btns[ ( selectedBtn = ( selectedBtn + 1 ) % btns.size( ) ) ] );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                _boxUI.selectButton( btns[ ( selectedBtn = ( selectedBtn + btns.size( ) - 1 )
                            % btns.size( ) ) ] );
                cooldown = COOLDOWN_COUNT;
            } else if( pressed & KEY_A ) {
                switch( btns[ selectedBtn ] ) {
                    case boxUI::BUTTON_PKMN_MOVE:
                        return 1;
                    case boxUI::BUTTON_PKMN_STATUS: {
                        STS::statusScreen::result stsres = STS::statusScreen::BACK;
                        u8                   curStsPage = 0;

                        if( _selectedIdx < MAX_PKMN_PER_BOX ) {
                            u8 curBoxPkmnCnt =
                                SAVE::SAV.getActiveFile( ).getCurrentBox( )->count( );

                            do {
                                STS::statusScreen sts
                                    = STS::statusScreen( &curSel, curBoxPkmnCnt > 1, curBoxPkmnCnt > 1 );
                                stsres     = sts.run( curStsPage );
                                curStsPage = sts.currentPage( );
                                if( stsres == STS::statusScreen::NEXT_PKMN ) {
                                    _selectedIdx = SAVE::SAV.getActiveFile( )
                                        .getCurrentBox( )->getNextFilledSpot( _selectedIdx );
                                    tmp = getPkmn( _selectedIdx );
                                    if( tmp != nullptr ) { curSel = pokemon( *tmp ); }
                                } else if( stsres == STS::statusScreen::PREV_PKMN ) {
                                    _selectedIdx = SAVE::SAV.getActiveFile( )
                                        .getCurrentBox( )->getPrevFilledSpot( _selectedIdx );
                                    tmp = getPkmn( _selectedIdx );
                                    if( tmp != nullptr ) { curSel = pokemon( *tmp ); }
                                }
                            } while( stsres != STS::statusScreen::BACK
                                    && stsres != STS::statusScreen::EXIT );
                        } else if( _selectedIdx < MAX_PKMN_PER_BOX + 6 ) {
                            u8 curBoxPkmnCnt =
                                SAVE::SAV.getActiveFile( ).getTeamPkmnCount( );

                            do {
                                STS::statusScreen sts
                                    = STS::statusScreen( &curSel, curBoxPkmnCnt > 1, curBoxPkmnCnt > 1 );
                                stsres     = sts.run( curStsPage );
                                curStsPage = sts.currentPage( );
                                if( stsres == STS::statusScreen::NEXT_PKMN ) {
                                    _selectedIdx = MAX_PKMN_PER_BOX +
                                        ( _selectedIdx - MAX_PKMN_PER_BOX + 1 ) % curBoxPkmnCnt;
                                    auto ptmp = SAVE::SAV.getActiveFile( ).getTeamPkmn(
                                            _selectedIdx - MAX_PKMN_PER_BOX );
                                    if( ptmp != nullptr ) { curSel = *ptmp; }
                                } else if( stsres == STS::statusScreen::PREV_PKMN ) {
                                    _selectedIdx = MAX_PKMN_PER_BOX +
                                        ( _selectedIdx - MAX_PKMN_PER_BOX + curBoxPkmnCnt - 1 )
                                        % curBoxPkmnCnt;
                                    auto ptmp = SAVE::SAV.getActiveFile( ).getTeamPkmn(
                                            _selectedIdx - MAX_PKMN_PER_BOX );
                                    if( ptmp != nullptr ) { curSel = *ptmp; }
                                }
                            } while( stsres != STS::statusScreen::BACK
                                    && stsres != STS::statusScreen::EXIT );
                        }

                        if( stsres == STS::statusScreen::EXIT ) { return 255; }
                        _boxUI.init( );
                        _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                        _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx );
                        break;
                    }
                    case boxUI::BUTTON_PKMN_RELEASE: {
                        // TODO: Add confirmation
                        SAVE::SAV.getActiveFile( ).getCurrentBox( )->clear( _selectedIdx );
                        _boxUI.updatePkmn( nullptr, _selectedIdx );
                        _boxUI.selectPkmn( nullptr, 0 );
                        _boxUI.hoverPkmn( nullptr, _selectedIdx );
                        return 0;
                    }
                    case boxUI::BUTTON_PKMN_GIVE_ITEM: {
                        BAG::bagViewer bv;
                        u16            itm = bv.getItem( BAG::bagViewer::GIVE_TO_PKMN );
                        if( itm ) {
                            if( curSel.getItem( ) ) {
                                auto curItm = curSel.getItem( );
                                SAVE::SAV.getActiveFile( ).m_bag.insert(
                                        BAG::toBagType( ITEM::getItemData( curItm ).m_itemType ), curItm, 1 );
                            }
                            tmp = getPkmn( _selectedIdx );
                            if( tmp != nullptr ) { tmp->giveItem( itm ); }
                        }
                        _boxUI.init( );
                        _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                        _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx );
                        break;
                    }
                    case boxUI::BUTTON_PKMN_TAKE_ITEM: {
                        tmp = getPkmn( _selectedIdx );
                        if( tmp != nullptr ) {
                            u16 acI = tmp->takeItem( );
                            SAVE::SAV.getActiveFile( ).m_bag.insert(
                                    BAG::toBagType( ITEM::getItemData( acI ).m_itemType ), acI, 1 );

                            _boxUI.updatePkmn( getPkmn( _selectedIdx ), _selectedIdx );
                            _boxUI.selectPkmn( nullptr, 0 );
                            _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx );
                        }
                        return 0;
                    }
                    default:
                        break;
                }
                break;
            }
            swiWaitForVBlank( );
        }

        _boxUI.selectPkmn( nullptr, 0 );
        _boxUI.hoverPkmn( getPkmn( _selectedIdx ), _selectedIdx, false );
        return 0;
    }

    void boxViewer::runBoxSettings( ) {
        // TODO
    }

} // namespace BOX
