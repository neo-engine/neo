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

#include "sound.h"

#include "saveGame.h"
#include "statusScreen.h"
#include "statusScreenUI.h"

namespace BOX {
#define TRESHOLD 20

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
                if( !_heldPkmn.getSpecies( ) ) {
                    SOUND::playSoundEffect( SFX_CANCEL );
                    return;
                }
            }
            if( pressed & KEY_B ) {
                if( _showParty ) {
                    SOUND::playSoundEffect( SFX_CANCEL );
                    //  _boxUI.hidePkmnTeam( );
                } else {
                    if( !_heldPkmn.getSpecies( ) ) {
                        SOUND::playSoundEffect( SFX_CANCEL );
                        return;
                    }
                }
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
            } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX ) { // player selects a pkmn
                    if( ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )
                            [ _selectedIdx ].getSpecies( ) ) {
                        if( _mode != MOVE ) {
                            if( !runStatusChoice( ) ) {
                                cooldown = COOLDOWN_COUNT;
                                continue;
                            }
                        }
                        takePkmn( _selectedIdx );
                    }
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

        if( _selectedIdx < MAX_PKMN_PER_BOX ) {
            // Pkmn is selected.
            _boxUI.hoverPkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ p_index ],
                    _selectedIdx );
        } else if( _selectedIdx == BOXNAME_BUTTON ) {
            _boxUI.selectButton( boxUI::BUTTON_BOX_NAME );
        } else if( _selectedIdx == PARTY_BUTTON ) {
            _boxUI.selectButton( boxUI::BUTTON_PARTY );
        }
    }

    void boxViewer::takePkmn( u8 p_index ) {
        // TODO
    }

    void boxViewer::returnPkmn( ) {
        // TODO
    }

    void boxViewer::runParty( ) {
        // TODO
    }

    u8 boxViewer::runStatusChoice( ) {
        // TODO
        return 0;
    }

    void boxViewer::runBoxSettings( ) {
        // TODO
    }

} // namespace BOX
