/*
Pokémon Emerald 2 Version
------------------------------

file        : statusScreen.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "statusScreen.h"
#include "uio.h"
#include "messageBox.h"
#include "defines.h"
#include "item.h"
#include "move.h"
#include "saveGame.h"
#include "dex.h"
#include "ribbon.h"
#include "bagViewer.h"
#include "defines.h"

namespace STS {
    statusScreen::statusScreen( u8 p_pkmnIdx ) {
        _page = 0;
        _pkmnIdx = p_pkmnIdx;
        _stsUI = new regStsScreenUI( );
    }
    statusScreen::~statusScreen( ) {
        delete _stsUI;
    }

    move* statusScreen::run( ) {
        _stsUI->init( _pkmnIdx );
        auto tg = _stsUI->draw( _pkmnIdx, true );
        auto rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
        touchPosition touch;

#define DEFAULT_MODE 0
#define VIEW_DETAILS 1
#define MOVE_DETAILS 2
#define RIBBON_DETAILS 3
        u8 mode = DEFAULT_MODE;
        u8 modeVal = 0;
        u8 selectedIdx = 42;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            int pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_X ) )
                break;
            else if( mode <= VIEW_DETAILS ) {
                if( GET_AND_WAIT( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                    _pkmnIdx = ( _pkmnIdx + 1 ) % FS::SAV->getTeamPkmnCount( );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    }
                } else if( GET_AND_WAIT( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                    _pkmnIdx = ( _pkmnIdx + FS::SAV->getTeamPkmnCount( ) - 1 ) % FS::SAV->getTeamPkmnCount( );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    }
                }
            }

            for( u8 i = 0; i < FS::SAV->getTeamPkmnCount( ); ++i )
                if( GET_AND_WAIT_C( 16 - 2 * i, 40 + 24 * i, 12 ) ) {
                    _pkmnIdx = i;
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    }
                }
            for( u8 i = 0; i < 5; ++i ) {
                if( FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_individualValues.m_isEgg && i != 2 )
                    continue;
                if( GET_AND_WAIT_C( 62 + 32 * i, 14 - 2 * i, 14 ) ) {
                    _page = i;
                    _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, mode == DEFAULT_MODE );
                    mode = VIEW_DETAILS;
                }
            }
            for( s8 i = 0; i < s8( tg.size( ) ) - 2; ++i )
                if( IN_RANGE_I( touch, tg[ i ] ) && IO::waitForInput( tg[ i ] ) ) {
                    u8 u = 0, o;
                    for( o = 0; o < 4 && u <= i; ++o )
                        if( AttackList[ FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->m_isFieldAttack )
                            u++;
                    o--;
                    consoleSelect( &IO::Bottom );
                    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    if( AttackList[ FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->possible( ) ) {

                        char buffer[ 50 ];
                        sprintf( buffer, "%s setzt %s\nein!", FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_name,
                                 AttackList[ FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->m_moveName.c_str( ) );
                        IO::messageBox a( buffer );
                        IO::NAV->draw( );

                        //shoUseAttack( (*_pokemon)[_pkmnIdx ].m_boxdata.m_speciesId,
                        //              (*_pokemon)[_pkmnIdx ].m_boxdata.m_isFemale, (*_pokemon)[_pkmnIdx ].m_boxdata.isShiny( ) );

                        return AttackList[ FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_moves[ o ] ];
                    } else {
                        IO::messageBox( "Diese Attacke kann jetzt\nnicht eingesetzt werden.", "PokéNav" );
                        _stsUI->init( _pkmnIdx, false );
                        tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                        if( mode != DEFAULT_MODE ) {
                            mode = VIEW_DETAILS;
                            rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                            _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                        }
                    }
                    break;
                }

            if( tg.size( ) >= 2 ) {
                if( IN_RANGE_I( touch, tg[ tg.size( ) - 2 ] ) && IO::waitForInput( tg[ tg.size( ) - 2 ] ) ) {
                    if( FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_holdItem ) { //take item
                        char buffer[ 50 ];
                        item acI = *ItemList[ FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_holdItem ];
                        FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_holdItem = 0;
                        consoleSelect( &IO::Bottom );
                        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                        consoleClear( );

                        sprintf( buffer, "%s von %s\nim Beutel verstaut.", acI.getDisplayName( true ).c_str( ), FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_name );
                        IO::NAV->draw( );
                        IO::messageBox a( buffer );
                        FS::SAV->m_bag.insert( BAG::toBagType( acI.m_itemType ), acI.getItemId( ), 1 );
                    } else { //give item
                        BAG::bagViewer bv;
                        UPDATE_TIME = false;
                        u16 itm = bv.getItem( BAG::bagViewer::GIVE_TO_PKMN );
                        UPDATE_TIME = true;
                        DRAW_TIME = true;
                        if( itm ) {
                            if( FS::SAV->m_pkmnTeam[ _pkmnIdx ].getItem( ) ) {
                                auto curItm = ItemList[ FS::SAV->m_pkmnTeam[ _pkmnIdx ].getItem( ) ];
                                FS::SAV->m_bag.insert( BAG::toBagType( curItm->m_itemType ), curItm->getItemId( ), 1 );
                            }
                            FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_holdItem = itm;
                        }
                    }
                    _stsUI->init( _pkmnIdx );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    }
                }
                if( IN_RANGE_I( touch, tg.back( ) ) && IO::waitForInput( tg.back( ) ) ) {
                    DEX::dex( DEX::dex::SHOW_SINGLE, -1 ).run( FS::SAV->m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_speciesId );

                    _stsUI->init( _pkmnIdx );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs = ribbon::getRibbons( FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    }
                }
            }

            if( mode == DEFAULT_MODE ) {
                if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                    break;
                } else if( GET_AND_WAIT( KEY_A ) ) {
                    _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    mode = VIEW_DETAILS;
                } else if( GET_AND_WAIT( KEY_SELECT ) ) {
                    if( selectedIdx == 42 ) {
                        selectedIdx = _pkmnIdx;
                        continue;
                    } else if( selectedIdx != _pkmnIdx ) {
                        std::swap( FS::SAV->m_pkmnTeam[ selectedIdx ], FS::SAV->m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->init( _pkmnIdx );
                        tg = _stsUI->draw( _pkmnIdx, true );
                    }
                    selectedIdx = 42;
                }
            } else {
                if( GET_AND_WAIT( KEY_RIGHT ) ) {
                    mode = VIEW_DETAILS;
                    _page = ( _page + 1 ) % _stsUI->m_pagemax;
                    _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, false );
                } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                    mode = VIEW_DETAILS;
                    _page = ( _page + _stsUI->m_pagemax - 1 ) % _stsUI->m_pagemax;
                    _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, false );
                }

                if( mode == VIEW_DETAILS ) {
                    if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                        mode = DEFAULT_MODE;
                        _stsUI->init( _pkmnIdx );
                        tg = _stsUI->draw( _pkmnIdx, true );
                    } else if( _page == 3 && GET_AND_WAIT( KEY_A ) ) {
                        mode = MOVE_DETAILS;
                        if( !_stsUI->drawMove( FS::SAV->m_pkmnTeam[ _pkmnIdx ], modeVal ) ) {
                            mode = VIEW_DETAILS;
                            modeVal = 0;
                        }
                    } else if( _page == 4 && !rbs.empty( ) && GET_AND_WAIT( KEY_A ) ) {
                        mode = RIBBON_DETAILS;
                        modeVal = 0;
                        if( !_stsUI->drawRibbon( FS::SAV->m_pkmnTeam[ _pkmnIdx ], rbs[ modeVal ] ) ) {
                            mode = VIEW_DETAILS;
                            modeVal = 0;
                        }
                    }
                } else if( mode == MOVE_DETAILS ) {
                    if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                        mode = VIEW_DETAILS;
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    } else if( GET_AND_WAIT( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                        do modeVal = ( modeVal + 1 ) % 4;
                        while( !_stsUI->drawMove( FS::SAV->m_pkmnTeam[ _pkmnIdx ], modeVal ) );
                    } else if( GET_AND_WAIT( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                        do modeVal = ( modeVal + 3 ) % 4;
                        while( !_stsUI->drawMove( FS::SAV->m_pkmnTeam[ _pkmnIdx ], modeVal ) );
                    }
                } else if( mode == RIBBON_DETAILS ) {
                    if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                        mode = VIEW_DETAILS;
                        _stsUI->draw( FS::SAV->m_pkmnTeam[ _pkmnIdx ], _page, true );
                    } else if( GET_AND_WAIT( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                        modeVal = ( modeVal + 1 ) % rbs.size( );
                        _stsUI->drawRibbon( FS::SAV->m_pkmnTeam[ _pkmnIdx ], rbs[ modeVal ] );
                    } else if( GET_AND_WAIT( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                        modeVal = ( modeVal + rbs.size( ) - 1 ) % rbs.size( );
                        _stsUI->drawRibbon( FS::SAV->m_pkmnTeam[ _pkmnIdx ], rbs[ modeVal ] );
                    }
                }
            }
        }
        return 0;
    }
}
