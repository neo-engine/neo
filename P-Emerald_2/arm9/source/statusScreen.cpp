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
#include "bagViewer.h"
#include "defines.h"
#include "dex.h"
#include "item.h"
#include "messageBox.h"
#include "move.h"
#include "ribbon.h"
#include "saveGame.h"
#include "screenFade.h"
#include "uio.h"

namespace STS {
    statusScreen::statusScreen( u8 p_pkmnIdx ) {
        _page    = 0;
        _pkmnIdx = p_pkmnIdx;
        _stsUI   = new regStsScreenUI( );
    }
    statusScreen::~statusScreen( ) {
        delete _stsUI;
    }

    u8 statusScreen::run( ) {
        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
        _stsUI->init( _pkmnIdx );
        auto tg  = _stsUI->draw( _pkmnIdx, true );
        auto rbs = ribbon::getRibbons( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
        touchPosition touch;
        IO::fadeScreen( IO::fadeType::UNFADE_FAST );

#define DEFAULT_MODE 0
#define VIEW_DETAILS 1
#define MOVE_DETAILS 2
#define RIBBON_DETAILS 3
        u8 mode        = DEFAULT_MODE;
        u8 modeVal     = 0;
        u8 selectedIdx = 42;
        u8 frame       = 0;

        int pressed, held;
        u8  cooldown = 1;
        loop( ) {
            _stsUI->animate( frame++, mode );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( held & KEY_X )
                break;
            else if( mode <= VIEW_DETAILS ) {
                if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                    _pkmnIdx = ( _pkmnIdx + 1 ) % SAVE::SAV->getActiveFile( ).getTeamPkmnCount( );
                    tg       = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );
                    cooldown = COOLDOWN_COUNT;

                    if( mode != DEFAULT_MODE ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = VIEW_DETAILS;
                        rbs  = ribbon::getRibbons(
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    }
                } else if( GET_KEY_COOLDOWN( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                    _pkmnIdx = ( _pkmnIdx + SAVE::SAV->getActiveFile( ).getTeamPkmnCount( ) - 1 )
                               % SAVE::SAV->getActiveFile( ).getTeamPkmnCount( );
                    tg       = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );
                    cooldown = COOLDOWN_COUNT;

                    if( mode != DEFAULT_MODE ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = VIEW_DETAILS;
                        rbs  = ribbon::getRibbons(
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    }
                }
            }

            for( u8 i = 0; i < SAVE::SAV->getActiveFile( ).getTeamPkmnCount( ); ++i )
                if( GET_AND_WAIT_C( 16 - 2 * i, 40 + 24 * i, 12 ) ) {
                    _pkmnIdx = i;
                    tg       = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = VIEW_DETAILS;
                        rbs  = ribbon::getRibbons(
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    }
                }
            for( u8 i = 0; i < 5; ++i ) {
                if( SAVE::SAV->getActiveFile( )
                        .m_pkmnTeam[ _pkmnIdx ].isEgg( )
                    && i != 2 )
                    continue;
                if( GET_AND_WAIT_C( 62 + 32 * i, 14 - 2 * i, 14 ) ) {
                    _page = i;
                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                  mode == DEFAULT_MODE );
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    mode = VIEW_DETAILS;
                }
            }
            for( s8 i = 0; i < s8( tg.size( ) ) - 2; ++i )
                if( IN_RANGE_I( touch, tg[ i ] ) && IO::waitForInput( tg[ i ] ) ) {
                    u8 u = 0, o;
                    for( o = 0; o < 4 && u <= i; ++o )
                        if( MOVE::isFieldMove( SAVE::SAV->getActiveFile( )
                                            .m_pkmnTeam[ _pkmnIdx ]
                                            .m_boxdata.m_moves[ o ] ) )
                            u++;
                    o--;
                    consoleSelect( &IO::Bottom );
                    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    for( u8 param = 0; param < 2; ++param )
                        if( MOVE::possible( SAVE::SAV->getActiveFile( )
                                    .m_pkmnTeam[ _pkmnIdx ]
                                    .m_boxdata.m_moves[ o ], param ) ) {
                            char buffer[ 50 ] = {0};
                            snprintf(
                                    buffer, 49, GET_STRING( 99 ),
                                    SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx
                                    ].m_boxdata.m_name,
                                    MOVE::getMoveName( SAVE::SAV->getActiveFile( )
                                        .m_pkmnTeam[ _pkmnIdx ]
                                        .m_boxdata.m_moves[ o ],
                                        CURRENT_LANGUAGE ).c_str( ) );
                            IO::messageBox a( buffer );
                            NAV::draw( );

                            // shoUseAttack( (*_pokemon)[_pkmnIdx ].m_boxdata.m_speciesId,
                            //              (*_pokemon)[_pkmnIdx ].m_boxdata.m_isFemale,
                            //              (*_pokemon)[_pkmnIdx ].m_boxdata.isShiny( ) );

                            return ( param << 15 ) | SAVE::SAV->getActiveFile( )
                                .m_pkmnTeam[ _pkmnIdx ]
                                .m_boxdata.m_moves[ o ];
                        }

                    IO::messageBox( GET_STRING( 100 ), GET_STRING( 91 ) );
                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    _stsUI->init( _pkmnIdx, false );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs  = ribbon::getRibbons(
                                SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                true );
                    }
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    break;
                }

            if( tg.size( ) >= 2 ) {
                if( IN_RANGE_I( touch, tg[ tg.size( ) - 2 ] )
                    && IO::waitForInput( tg[ tg.size( ) - 2 ] ) ) {
                    if( SAVE::SAV->getActiveFile( )
                            .m_pkmnTeam[ _pkmnIdx ].getItem( ) ) { // take item
                        char buffer[ 50 ];
                        u16 acI = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ].takeItem( );
                        consoleSelect( &IO::Bottom );
                        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                        consoleClear( );

                        sprintf(buffer, GET_STRING( 101 ),
                                ITEM::getItemName( acI, CURRENT_LANGUAGE ).c_str( ),
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ].m_boxdata.m_name );
                        NAV::draw( );
                        IO::messageBox a( buffer );
                        SAVE::SAV->getActiveFile( ).m_bag.insert( BAG::toBagType(
                                    ITEM::getItemData( acI).m_itemType ), acI, 1 );
                    } else { // give item
                        BAG::bagViewer bv;
                        UPDATE_TIME = false;
                        u16 itm     = bv.getItem( BAG::bagViewer::GIVE_TO_PKMN );
                        UPDATE_TIME = true;
                        DRAW_TIME   = true;
                        if( itm ) {
                            if( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ].getItem( ) ) {
                                auto curItm = SAVE::SAV->getActiveFile( )
                                                            .m_pkmnTeam[ _pkmnIdx ]
                                                            .getItem( );
                                SAVE::SAV->getActiveFile( ).m_bag.insert(
                                    BAG::toBagType( ITEM::getItemData( curItm ).m_itemType ),
                                    curItm, 1 );
                            }
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ].giveItem( itm );
                        }
                    }
                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    _stsUI->init( _pkmnIdx );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs  = ribbon::getRibbons(
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                    }
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                }
                if( IN_RANGE_I( touch, tg.back( ) ) && IO::waitForInput( tg.back( ) ) ) {
                    DEX::dex( DEX::dex::SHOW_SINGLE, -1 )
                        .run( SAVE::SAV->getActiveFile( )
                                  .m_pkmnTeam[ _pkmnIdx ]
                                  .m_boxdata.m_speciesId );

                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    _stsUI->init( _pkmnIdx );
                    tg = _stsUI->draw( _pkmnIdx, mode == DEFAULT_MODE );

                    if( mode != DEFAULT_MODE ) {
                        mode = VIEW_DETAILS;
                        rbs  = ribbon::getRibbons(
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                    }
                    swiWaitForVBlank( );
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                }
            }

            if( mode == DEFAULT_MODE ) {
                if( GET_KEY_COOLDOWN( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                    break;
                } else if( GET_KEY_COOLDOWN( KEY_A ) ) {
                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page, true );
                    mode = VIEW_DETAILS;
                    swiWaitForVBlank( );
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                } else if( GET_KEY_COOLDOWN( KEY_SELECT ) ) {
                    if( selectedIdx == 42 ) {
                        selectedIdx = _pkmnIdx;
                        continue;
                    } else if( selectedIdx != _pkmnIdx ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        std::swap( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ selectedIdx ],
                                   SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ] );
                        _stsUI->init( _pkmnIdx );
                        tg = _stsUI->draw( _pkmnIdx, true );
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    }
                    selectedIdx = 42;
                }
            } else {
                if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    mode  = VIEW_DETAILS;
                    _page = ( _page + 1 ) % _stsUI->m_pagemax;
                    _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                  false );
                    swiWaitForVBlank( );
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                    IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                    mode  = VIEW_DETAILS;
                    _page = ( _page + _stsUI->m_pagemax - 1 ) % _stsUI->m_pagemax;
                    _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                  false );
                    swiWaitForVBlank( );
                    IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                }

                if( mode == VIEW_DETAILS ) {
                    if( GET_KEY_COOLDOWN( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = DEFAULT_MODE;
                        _stsUI->init( _pkmnIdx );
                        tg = _stsUI->draw( _pkmnIdx, true );
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    } else if( _page == 3 && GET_AND_WAIT( KEY_A ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = MOVE_DETAILS;
                        if( !_stsUI->drawMove( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ],
                                               modeVal ) ) {
                            mode    = VIEW_DETAILS;
                            modeVal = 0;
                        }
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    } else if( _page == 4 && !rbs.empty( ) && GET_AND_WAIT( KEY_A ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode    = RIBBON_DETAILS;
                        modeVal = 0;
                        if( !_stsUI->drawRibbon( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ],
                                                 rbs[ modeVal ] ) ) {
                            mode    = VIEW_DETAILS;
                            modeVal = 0;
                        }
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    }
                } else if( mode == MOVE_DETAILS ) {
                    if( GET_KEY_COOLDOWN( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = VIEW_DETAILS;
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    } else if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                        do {
                            modeVal = ( modeVal + 1 ) % 4;
                            IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                            bool res = _stsUI->drawMove(
                                SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], modeVal );
                            swiWaitForVBlank( );
                            IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                            if( res ) break;
                        }
                        loop( );
                    } else if( GET_KEY_COOLDOWN( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                        do {
                            modeVal = ( modeVal + 3 ) % 4;
                            IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                            bool res = _stsUI->drawMove(
                                SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], modeVal );
                            swiWaitForVBlank( );
                            IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                            if( res ) break;
                        }
                        loop( );
                    }
                } else if( mode == RIBBON_DETAILS ) {
                    if( GET_KEY_COOLDOWN( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        mode = VIEW_DETAILS;
                        _stsUI->draw( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ], _page,
                                      true );
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    } else if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        modeVal = ( modeVal + 1 ) % rbs.size( );
                        _stsUI->drawRibbon( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ],
                                            rbs[ modeVal ] );
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    } else if( GET_KEY_COOLDOWN( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                        IO::fadeScreen( IO::fadeType::CLEAR_DARK_FAST );
                        modeVal = ( modeVal + rbs.size( ) - 1 ) % rbs.size( );
                        _stsUI->drawRibbon( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _pkmnIdx ],
                                            rbs[ modeVal ] );
                        swiWaitForVBlank( );
                        IO::fadeScreen( IO::fadeType::UNFADE_FAST );
                    }
                }
            }
        }
        return 0;
    }
} // namespace STS
