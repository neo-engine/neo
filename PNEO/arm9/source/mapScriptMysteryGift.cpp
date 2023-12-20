/*
Pokémon neo
------------------------------

file        : mapScriptMysteryGift.cpp
author      : Philip Wellnitz
description : Map script engine (mystery gift)

Copyright (C) 2023 - 2023
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
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "map/mapScript.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace MAP {
    void mapDrawer::mysteryGiftClerk( ) {
        char buffer[ 200 ];
        ANIMATE_MAP = false;

        if( !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_UNCOLLECTED_MYSTERY_EVENT ) ) {
            printMapMessage( GET_MAP_STRING( IO::STR_MAP_MY_NEXT_VISIT ), MSG_NORMAL );
            return;
        } else {
            printMapMessage( GET_MAP_STRING( IO::STR_MAP_MY_GIFT ), MSG_NORMAL );
        }

        for( u8 i = 0; i < SAVE::MAX_STORED_WC; ++i ) {
            auto& wc = SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i ];
            if( !SAVE::SAV.getActiveFile( ).collectedWC( wc.m_id ) ) {
                switch( wc.m_type ) {
                case SAVE::WCTYPE_ITEM: {
                    // hand player the items

                    for( u8 j = 0; j < 3; ++j ) {
                        if( wc.m_data.m_item.m_itemId[ j ] && wc.m_data.m_item.m_itemCount[ j ] ) {
                            IO::giveItemToPlayer( wc.m_data.m_item.m_itemId[ j ],
                                                  wc.m_data.m_item.m_itemCount[ j ] );
                        }
                    }
                    SAVE::SAV.getActiveFile( ).registerCollectedWC( wc.m_id );
                    break;
                }
                case SAVE::WCTYPE_PKMN: {
                    // check if there is an empty spot in the team left, hand over pkmn if
                    // there is

                    if( SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) == 6 ) {
                        // No space
                        printMapMessage( GET_MAP_STRING( IO::STR_MAP_MY_MAKE_SPACE ), MSG_NORMAL );
                        return;
                    }

                    boxPokemon giftPkmn{ wc.m_data.m_pkmn.m_moves,
                                         wc.m_data.m_pkmn.m_species,
                                         wc.m_data.m_pkmn.m_name,
                                         wc.m_data.m_pkmn.m_level,
                                         wc.m_data.m_pkmn.m_id,
                                         wc.m_data.m_pkmn.m_sid,
                                         wc.m_data.m_pkmn.m_oT,
                                         wc.m_data.m_pkmn.m_shiny,
                                         wc.m_data.m_pkmn.m_hiddenAbility,
                                         wc.m_data.m_pkmn.m_fatefulEncounter,
                                         wc.m_data.m_pkmn.m_isEgg,
                                         wc.m_data.m_pkmn.m_gotPlace,
                                         wc.m_data.m_pkmn.m_ball,
                                         wc.m_data.m_pkmn.m_pokerus,
                                         wc.m_data.m_pkmn.m_forme };
                    memcpy( giftPkmn.m_ribbons, wc.m_data.m_pkmn.m_ribbons, 12 );

                    u8 ic = 0;
                    for( ; ic < 4; ++ic ) {
                        if( !wc.m_data.m_pkmn.m_items[ ic ] ) { break; }
                    }
                    if( ic ) { giftPkmn.giveItem( wc.m_data.m_pkmn.m_items[ rand( ) % ic ] ); }

                    SAVE::SAV.getActiveFile( ).setTeamPkmn(
                        SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), &giftPkmn );

                    snprintf( buffer, 199, GET_MAP_STRING( IO::STR_MAP_MY_RECEIVE ),
                              FS::getDisplayName( wc.m_data.m_pkmn.m_species ).c_str( ) );
                    printMapMessage( buffer, MSG_INFO );
                    SAVE::SAV.getActiveFile( ).registerCollectedWC( wc.m_id );

                    break;
                }
                default:
                case SAVE::WCTYPE_NONE: {
                    SAVE::SAV.getActiveFile( ).registerCollectedWC( wc.m_id );
                    break;
                }
                }
                break;
            }
        }

        printMapMessage( GET_MAP_STRING( IO::STR_MAP_MY_NEXT_VISIT ), MSG_NORMAL );
        SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_UNCOLLECTED_MYSTERY_EVENT, 0 );

        for( u8 i = 0; i < SAVE::MAX_STORED_WC; ++i ) {
            auto& wc = SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i ];
            if( !SAVE::SAV.getActiveFile( ).collectedWC( wc.m_id ) ) {
                SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_UNCOLLECTED_MYSTERY_EVENT, 1 );
            }
        }
        ANIMATE_MAP = true;
    }
} // namespace MAP
