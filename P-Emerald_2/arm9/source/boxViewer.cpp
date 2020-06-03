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
            } else if( pressed & KEY_A ) {
                if( _selectedIdx == u8( -1 ) ) {
                    select( 0 );
                } else if( _selectedIdx < MAX_PKMN_PER_BOX ) { // player selects a pkmn
                    if( ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )
                            [ _selectedIdx ].getSpecies( ) ) {
                        if( _mode != MOVE ) {
                            u8 res = runStatusChoice( );
                            if( !res ) {
                                cooldown = COOLDOWN_COUNT;
                                continue;
                            }
                            if( res == 255 ) { // player pressed X
                                if( !_heldPkmn.getSpecies( ) ) {
                                    return;
                                } else {
                                    continue;
                                }
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
        pokemon curSel;
        if( _selectedIdx < MAX_PKMN_PER_BOX ) {
            curSel = ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ];
            _boxUI.selectPkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ],
                    _selectedIdx );
        } else {
            _boxUI.selectPkmn( nullptr, 0 );
            return 0;
        }

        u8 selectedBtn = 0;
        std::vector<boxUI::button> btns = std::vector<boxUI::button>( );

        btns.push_back( boxUI::BUTTON_PKMN_MOVE );
        btns.push_back( boxUI::BUTTON_PKMN_STATUS );
        if( !curSel.isEgg( ) ) {
            btns.push_back( boxUI::BUTTON_PKMN_RELEASE );
            btns.push_back( boxUI::BUTTON_PKMN_GIVE_ITEM );
            if( curSel.getItem( ) ) {
                btns.push_back( boxUI::BUTTON_PKMN_TAKE_ITEM );
            }
        }
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
                        STS::statusScreen::result stsres;
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
                                    curSel =
                                        ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ];
                                } else if( stsres == STS::statusScreen::PREV_PKMN ) {
                                    _selectedIdx = SAVE::SAV.getActiveFile( )
                                        .getCurrentBox( )->getPrevFilledSpot( _selectedIdx );
                                    curSel =
                                        ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ];
                                }
                            } while( stsres != STS::statusScreen::BACK
                                    && stsres != STS::statusScreen::EXIT );
                        } else {
                            // TODO team pkmn
                        }

                        if( stsres == STS::statusScreen::EXIT ) { return 255; }
                        _boxUI.init( );
                        _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                        _boxUI.hoverPkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[
                                _selectedIdx ], _selectedIdx );
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
                            ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ]
                                .giveItem( itm );
                        }
                        _boxUI.init( );
                        _boxUI.draw( SAVE::SAV.getActiveFile( ).getCurrentBox( ) );
                        _boxUI.hoverPkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[
                                _selectedIdx ], _selectedIdx );
                        break;
                    }
                    case boxUI::BUTTON_PKMN_TAKE_ITEM: {
                        u16 acI = ( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ]
                                .takeItem( );
                        SAVE::SAV.getActiveFile( ).m_bag.insert(
                                BAG::toBagType( ITEM::getItemData( acI ).m_itemType ), acI, 1 );

                        _boxUI.updatePkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[
                                _selectedIdx ], _selectedIdx );
                        _boxUI.selectPkmn( nullptr, 0 );
                        _boxUI.hoverPkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[
                                _selectedIdx ], _selectedIdx );
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
        _boxUI.hoverPkmn( &( *SAVE::SAV.getActiveFile( ).getCurrentBox( ) )[ _selectedIdx ],
                _selectedIdx, false );
        return 0;
    }

    void boxViewer::runBoxSettings( ) {
        // TODO
    }

} // namespace BOX
