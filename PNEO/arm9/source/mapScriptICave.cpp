/*
Pokémon neo
------------------------------

file        : mapScriptICave.cpp
author      : Philip Wellnitz
description : Map script engine

Copyright (C) 2012 - 2026
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

#include <algorithm>

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "dex/dex.h"
#include "fs/fs.h"
#include "gen/locationNames.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/simpleWidget.h"
#include "io/sprite.h"
#include "io/util.h"
#include "map/mapDrawer.h"
#include "map/mapScript.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"

namespace MAP {
    constexpr u8 ICAVE_TSYO_POKE_1 = 24;
    constexpr u8 ICAVE_TSYO_POKE_2 = 48;
    constexpr u8 ICAVE_TSYO_POKE_3 = 72;
    constexpr u8 ICAVE_TSYO_POKE_4 = 96;

    FILE* icavef           = nullptr;
    u8    infinityCaveType = 0; // (1 + evtype)
    u32   infinityCaveReqs = 0; // 8 bit per shard type

    constexpr u8 MAX_INFINITY_POKE = 5;

    std::pair<u16, u8>              infinityCavePoke[ MAX_INFINITY_POKE ];
    std::vector<std::pair<u16, u8>> infinityCaveSpecials;

    void mapDrawer::initInfinityCave( u8 p_icavetype ) {
        // load bst_ev list for ev stat p_icavetype
        // reset counter

        infinityCaveType = 1 + p_icavetype;
        infinityCaveReqs = 0;

        SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) = 0;

        for( u8 i = 0; i < MAX_INFINITY_POKE; ++i ) { infinityCavePoke[ i ] = { 0, 0 }; }
        infinityCaveSpecials.clear( );

        if( icavef ) { fclose( icavef ); }
        icavef = FS::openInfinityCave( p_icavetype );
    }

    bool mapDrawer::continueInfinityCave( u16& p_resultX, u16& p_resultY ) {
        if( infinityCaveReqs ) {
            // Not all reqs cleared, warp to exit
            p_resultX = 1;
            p_resultY = 1;
            return false;
        }
        SAVE::CURRENT_FILE->infinityCaveCurrentLayer( )
            = std::min( 100, SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) + 1 );

        SAVE::CURRENT_FILE->infinityCaveMaxLayer( )
            = std::max( SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ),
                        SAVE::CURRENT_FILE->infinityCaveMaxLayer( ) );

        if( SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
            && SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_1 ) {
            p_resultX        = 3;
            p_resultY        = 1;
            infinityCaveReqs = 0;
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 1 );
        } else if( SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
                   && SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_2 ) {
            p_resultX        = 5;
            p_resultY        = 1;
            infinityCaveReqs = 0;
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 1 );
        } else if( SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
                   && SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_3 ) {
            p_resultX        = 7;
            p_resultY        = 1;
            infinityCaveReqs = 0;
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 1 );
        } else if( SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
                   && SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_4 ) {
            p_resultX        = 9;
            p_resultY        = 1;
            infinityCaveReqs = 0;
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 1 );
        } else {
            p_resultX = 2 * ( rand( ) % 5 ) + 1;
            p_resultY = 2 * ( rand( ) % 4 ) + 3;

            u8 start = rand( ) % 4;
            u8 rem   = SAVE::CURRENT_FILE->infinityCaveCurrentLayer( );
            for( u8 i = 1; i < 4; ++i ) {
                u8 shift = ( start + i ) % 4;
                u8 nx    = rand( ) % rem;
                infinityCaveReqs |= nx << ( 8 * shift );
                rem -= nx;
            }
            infinityCaveReqs |= rem << ( 8 * start );
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 0 );
        }

        // reset items
        for( u16 i = SAVE::F_ICAVE_LAYER_ITEM1; i <= SAVE::F_ICAVE_LAYER_ITEM10; ++i ) {
            SAVE::CURRENT_FILE->setFlag( i, 0 );
        }
        SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_POKE_CLEARED, 0 );
        return true;
    }

    bool mapDrawer::mapEnterInfinityCave( ) {
        auto& cD   = currentData( );
        u16   curx = SAVE::CURRENT_FILE->m_player.m_pos.m_posX;
        u16   cury = SAVE::CURRENT_FILE->m_player.m_pos.m_posY;
        u16   mapX = curx / SIZE, mapY = cury / SIZE;

        if( !SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) ) {
            infinityCaveReqs = 1;
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 0 );
            cD.m_pokemon[ 0 ] = { PKMN_ZUBAT, 0, INFINITY_CAVE, 1, 15, 20 };
            return false;
        }
        if( infinityCaveReqs ) { SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 0 ); }

        // update stored pkmn
        u8  base_level = SAVE::CURRENT_FILE->infinityCaveCurrentLayer( );
        u16 bst_upper  = base_level * base_level / 2 + 300;
        if( SAVE::CURRENT_FILE->m_options.m_difficulty == 3 ) {
            base_level = std::max( 1, base_level / 2 );
            bst_upper  = base_level * base_level / 2 + 250;
        } else if( SAVE::CURRENT_FILE->m_options.m_difficulty < 3 ) {
            base_level = std::max( 1, base_level / 3 );
            bst_upper  = base_level * base_level / 2 + 225;
            bst_upper  = 5 * base_level + 220;
        }

        if( SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_1
            || SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_2
            || SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_3
            || SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) == ICAVE_TSYO_POKE_4 ) {
            // spawn a random special pkmn
            if( infinityCaveSpecials.empty( )
                || SAVE::CURRENT_FILE->checkFlag( SAVE::F_ICAVE_POKE_CLEARED ) ) {
                return true;
            }
            u16 idx = rand( ) % ( infinityCaveSpecials.size( ) );
            // create wildPoke
            if( cD.m_events[ 19 ].m_type == EVENT_OW_PKMN ) {
                cD.m_events[ 19 ].m_data.m_owPkmn.m_speciesId = infinityCaveSpecials[ idx ].first;
                cD.m_events[ 19 ].m_data.m_owPkmn.m_forme     = infinityCaveSpecials[ idx ].second;
                resetMapSprites( );
                constructAndAddNewMapObjects( cD, mapX, mapY );
            }
        } else {
            u16 nxt_bst  = 0;
            u16 nxt_idx  = 0;
            u8  nxt_form = 0;
            u8  nxt_ev   = 0;

            loop( ) {
                if( !icavef ) { break; }

                nxt_idx  = 0;
                nxt_form = 0;
                nxt_bst  = 0;
                nxt_ev   = 0;

                if( !fread( &nxt_bst, sizeof( u16 ), 1, icavef ) ) { break; }
                fread( &nxt_idx, sizeof( u16 ), 1, icavef );
                fread( &nxt_form, sizeof( u8 ), 1, icavef );
                if( !fread( &nxt_ev, sizeof( u8 ), 1, icavef ) ) { break; }

                if( hasBattleTransform( nxt_idx ) ) { nxt_form = 0; }
                if( SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
                    && ( isLegendary( nxt_idx ) || isUltraBeast( nxt_idx ) )
                    && !isSpecial( nxt_idx ) ) {
                    if( nxt_idx == PKMN_SILVALLY && nxt_form ) { continue; }
                    infinityCaveSpecials.push_back( std::pair<u16, u8>{ nxt_idx, nxt_form } );
                    continue;
                }
                if( isLegendary( nxt_idx ) || isMythical( nxt_idx ) || isUltraBeast( nxt_idx )
                    || isParadox( nxt_idx ) || isSecret( nxt_idx ) ) {
                    continue;
                }
                if( !SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
                    && isTradeEvolution( nxt_idx ) ) {
                    infinityCaveSpecials.push_back( std::pair<u16, u8>{ nxt_idx, nxt_form } );
                    continue;
                }
                if( !SAVE::CURRENT_FILE->checkFlag( SAVE::F_NAT_DEX_OBTAINED )
                    && SAVE::CURRENT_FILE->getPkmnDisplayDexId( nxt_idx ) == u16( -1 ) ) {
                    continue;
                }

                if( infinityCaveSpecials.empty( ) ) {
                    infinityCaveSpecials.push_back( std::pair<u16, u8>{ nxt_idx, nxt_form } );
                }

                u8 free = MAX_INFINITY_POKE + 1;
                for( u8 i = 0; i < MAX_INFINITY_POKE; ++i ) {
                    if( !infinityCavePoke[ i ].first ) {
                        free = i;
                        break;
                    }
                }

                if( free < MAX_INFINITY_POKE ) {
                    infinityCavePoke[ free ] = { nxt_idx, nxt_form };
                } else {
                    free = rand( ) % ( 2 * MAX_INFINITY_POKE );
                    if( free < MAX_INFINITY_POKE ) {
                        infinityCavePoke[ free ] = { nxt_idx, nxt_form };
                    }
                }

                if( nxt_bst >= bst_upper ) { break; }
            }

            cD.m_pokemon[ 0 ] = { PKMN_ZUBAT, 0, INFINITY_CAVE, base_level, 15, 20 };

            for( u8 i = 0; i < MAX_INFINITY_POKE; ++i ) {
                if( infinityCavePoke[ i ].first ) {
                    cD.m_pokemon[ i ] = { infinityCavePoke[ i ].first,
                                          infinityCavePoke[ i ].second,
                                          INFINITY_CAVE,
                                          base_level,
                                          15,
                                          20 };
                }
            }
        }
        return true;
    }

    bool mapDrawer::gateCheckInfinityCave( ) {
        u16                   curx = SAVE::CURRENT_FILE->m_player.m_pos.m_posX;
        u16                   cury = SAVE::CURRENT_FILE->m_player.m_pos.m_posY;
        u16                   mapX = curx / SIZE, mapY = cury / SIZE;
        std::array<char, 200> buffer{ };

        if( !SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) ) {
            IO::printMessage( GET_MAP_STRING( IO::STR_MAP_ICAVE_REPORT_REQS_MET_AFTER ), MSG_INFO );
            return false;
        }
        if( !infinityCaveReqs || SAVE::CURRENT_FILE->checkFlag( SAVE::F_ICAVE_LAYER_CLEARED ) ) {
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 1 );
            infinityCaveReqs = 0;
            IO::printMessage( GET_MAP_STRING( IO::STR_MAP_ICAVE_REPORT_REQS_MET_AFTER ), MSG_INFO );
            return false;
        }

        // report requirements
        u8 blue   = infinityCaveReqs & 255;
        u8 red    = ( infinityCaveReqs >> 8 ) & 255;
        u8 green  = ( infinityCaveReqs >> 16 ) & 255;
        u8 yellow = ( infinityCaveReqs >> 24 ) & 255;

        snprintf( buffer.data( ), buffer.size( ), GET_MAP_STRING( IO::STR_MAP_ICAVE_REPORT_REQS ),
                  red, blue, yellow, green );
        IO::printMessage( buffer.data( ), MSG_INFO );

        // check if player has required shards
        bool clear = blue <= SAVE::CURRENT_FILE->m_bag.count(
                         BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ), I_BLUE_SHARD );
        clear &= red <= SAVE::CURRENT_FILE->m_bag.count(
                     BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ), I_RED_SHARD );
        clear &= green <= SAVE::CURRENT_FILE->m_bag.count(
                     BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ), I_GREEN_SHARD );
        clear &= yellow <= SAVE::CURRENT_FILE->m_bag.count(
                     BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ), I_YELLOW_SHARD );

        if( !clear ) {
            IO::printMessage( GET_MAP_STRING( IO::STR_MAP_ICAVE_REPORT_REQS_NOT_MET ), MSG_INFO );
            return false;
        }

        // ask if they want to pay the shards (if possible)
        IO::simpleYesNoBox yn;
        if( yn.getResult( GET_MAP_STRING( IO::STR_MAP_ICAVE_REPORT_REQS_MET_Q ), MSG_INFO_NOCLOSE )
            == IO::yesNoBox::YES ) {
            IO::init( );
            // remove shards from bag
            SAVE::CURRENT_FILE->m_bag.erase( BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ),
                                             I_BLUE_SHARD, blue );
            SAVE::CURRENT_FILE->m_bag.erase( BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ),
                                             I_RED_SHARD, red );
            SAVE::CURRENT_FILE->m_bag.erase( BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ),
                                             I_GREEN_SHARD, green );
            SAVE::CURRENT_FILE->m_bag.erase( BAG::toBagType( BAG::ITEMTYPE_COLLECTIBLE ),
                                             I_YELLOW_SHARD, yellow );

            // set reqs to 0
            SAVE::CURRENT_FILE->setFlag( SAVE::F_ICAVE_LAYER_CLEARED, 1 );
            infinityCaveReqs = 0;
            // eq
            earthquake( );

            runLevelScripts( _data[ _curX ][ _curY ], mapX, mapY );
            // set lamps to green
            IO::printMessage( GET_MAP_STRING( IO::STR_MAP_ICAVE_REPORT_REQS_MET_AFTER ), MSG_INFO );
        } else {
            IO::init( );
            return false;
        }

        return true;
    }
} // namespace MAP
