/*
Pokémon neo
------------------------------

file        : mapBattleFacilities.cpp
author      : Philip Wellnitz
description : Map drawing engine: implementations of the various battle facilities.

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

#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "fs/data.h"
#include "fs/fs.h"
#include "gen/trainerClassNames.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/uio.h"
#include "map/mapBattleFacilityDefines.h"
#include "map/mapDrawer.h"
#include "sound/sound.h"
#include "sts/partyScreen.h"

namespace MAP {
    const ruleSet FACILITY_RULE_SETS[ NUM_RULESETS ] = {
        { RSID_SINGLE_LV50, 50, 3, true, 7, BATTLE::BM_SINGLE },
        { RSID_SINGLE_LV100, 100, 3, true, 7, BATTLE::BM_SINGLE },
        { RSID_SINGLE_LV30, 30, 3, false, 3, BATTLE::BM_SINGLE },  // battle tents
        { RSID_DOUBLE_LV50, 50, 4, true, 7, BATTLE::BM_DOUBLE },   // battle tower
        { RSID_DOUBLE_LV100, 100, 4, true, 7, BATTLE::BM_DOUBLE }, // battle tower
    };

    constexpr u8 MAX_BF_TRAINER_CLASS = 40;

    /*
     * @brief: Possible trainer classes for (random) opponents in any of the battle
     * facilities.
     */
    constexpr u8 BATTLE_FACILITY_TRAINER_CLASSES[ MAX_BF_TRAINER_CLASS ] = { TC_AROMA_LADY,
                                                                             TC_BATTLE_GIRL,
                                                                             TC_BEAUTY,
                                                                             TC_BIRD_KEEPER,
                                                                             TC_BLACK_BELT,
                                                                             TC_BUG_CATCHER,
                                                                             TC_BUG_MANIAC,
                                                                             TC_CAMPER,
                                                                             TC_COLLECTOR,
                                                                             TC_ACE_TRAINER,
                                                                             TC_DRAGON_TAMER,
                                                                             TC_EXPERT,
                                                                             TC_FISHERMAN,
                                                                             TC_GENTLEMAN,
                                                                             TC_GUITARIST,
                                                                             TC_HEX_MANIAC,
                                                                             TC_HIKER,
                                                                             TC_KINDLER,
                                                                             TC_LADY,
                                                                             TC_LASS,
                                                                             TC_NINJA_BOY,
                                                                             TC_PARASOL_LADY,
                                                                             TC_PICNICKER,
                                                                             TC_POKE_FAN,
                                                                             TC_POKE_MANIAC,
                                                                             TC_POKEMON_BREEDER,
                                                                             TC__POKEMON_BREEDER,
                                                                             TC_POKEMON_RANGER,
                                                                             TC_PSYCHIC,
                                                                             TC_RICH_BOY,
                                                                             TC_RUIN_MANIAC,
                                                                             TC_SAILOR,
                                                                             TC_SCHOOLBOY,
                                                                             TC_SCHOOLGIRL,
                                                                             TC_SWIMMER,
                                                                             TC__SWIMMER,
                                                                             TC_TRIATHLETE,
                                                                             TC_TUBER,
                                                                             TC__TUBER,
                                                                             TC_YOUNGSTER };

    constexpr u16 MAX_PKMN_PER_CLASS = 200;

    constexpr u16 BF_PKMN_IDX_FOR_TCLASS[ MAX_BF_TRAINER_CLASS ][ MAX_PKMN_PER_CLASS ] = {
        // AROMA_LADY
        { },
        // BATTLE_GIRL
        { },
        // BEAUTY
        { },
        // BIRD_KEEPER
        { },
        // BLACK_BELT
        { },
        // BUG_CATCHER
        { },
        // BUG_MANIAC
        { },
        // CAMPER
        { },
        // COLLECTOR
        { },
        // ACE_TRAINER
        { },
        // DRAGON_TAMER
        { },
        // EXPERT
        { },
        // FISHERMAN
        { },
        // GENTLEMAN
        { },
        // GUITARIST
        { },
        // HEX_MANIAC
        { },
        // HIKER
        { },
        // KINDLER
        { },
        // LADY
        { },
        // LASS
        { },
        // NINJA_BOY
        { },
        // PARASOL_LADY
        { },
        // PICNICKER
        { },
        // POKE_FAN
        { },
        // POKE_MANIAC
        { },
        // POKEMON_BREEDER
        { },
        // _POKEMON_BREEDER
        { },
        // POKEMON_RANGER
        { },
        // PSYCHIC
        { },
        // RICH_BOY
        { },
        // RUIN_MANIAC
        { },
        // SAILOR
        { },
        // SCHOOLBOY
        { },
        // SCHOOLGIRL
        { },
        // SWIMMER
        { },
        // _SWIMMER
        { },
        // TRIATHLETE
        { },
        // TUBER
        { },
        // _TUBER
        { },
        // YOUNGSTER
        { },
    };

    constexpr u16 bfPkmnForClassAndStreak( u8 p_trainerClass, u16 p_streak ) {
        u16 choice = 0;
        while( !choice ) {
            choice = rand( ) % MAX_PKMN_PER_CLASS;
            choice = BF_PKMN_IDX_FOR_TCLASS[ p_trainerClass ][ choice ];
            if( choice > MAX_BF_PKMN_FOR_STREAK[ p_streak ]
                || choice < MIN_BF_PKMN_FOR_STREAK[ p_streak ] ) {
                choice = 0;
            }
        }
        return choice;
    }

    pokemon           PLAYER_TEMP_TEAM[ 6 ];
    BATTLE::bfTrainer NEXT_OPPONENT;
    bfPokemon         NEXT_OPPONENT_TEAM[ 6 ];

    bool createNextOpponentTrainer( const ruleSet& p_rules, u16 p_streak,
                                    bool p_ignoreClassLimits ) {
        if( p_streak > IV_MAX_STREAK ) { p_streak = IV_MAX_STREAK; }
        std::memset( &NEXT_OPPONENT, 0, sizeof( BATTLE::bfTrainer ) );
        std::memset( &NEXT_OPPONENT_TEAM, 0, 6 * sizeof( bfPokemon ) );

        NEXT_OPPONENT.m_trainerNameIdx = bfTrainerForStreak( p_streak );
        // TODO
        // if( !FS::loadBFTrainer( &NEXT_OPPONENT, NEXT_OPPONENT.m_trainerNameIdx ) ) { return
        // false; }

        auto numPokemon = p_rules.m_battleMode == BATTLE::BM_SINGLE ? 3 : 4;

        auto found = 0;

        while( found < numPokemon ) {
            u16 idx = 0;
            if( !p_ignoreClassLimits ) {
                // just pick numPokemon different pkmn
                idx = bfPkmnForClassAndStreak( NEXT_OPPONENT.m_trainerClass, p_streak );
            } else {
                // pick according to tclass
                idx = bfPkmnForStreak( p_streak );
            }

            // load pkmn
            FS::loadBFPokemon( &NEXT_OPPONENT_TEAM[ found ], idx );

            bool bad = false;
            // check that pkmn is different from the remaining pkmn and does not violate
            // the item rule
            for( auto i = 0; i < found; ++i ) {
                if( NEXT_OPPONENT_TEAM[ found ].m_speciesId == NEXT_OPPONENT_TEAM[ i ].m_speciesId
                    || NEXT_OPPONENT_TEAM[ found ].m_heldItem
                           == NEXT_OPPONENT_TEAM[ i ].m_heldItem ) {
                    bad = true;
                    break;
                }
            }
            if( p_ignoreClassLimits ) {
                // also check that the pkmn is different from the current pkmn of the
                // player
                for( auto i = 0; i < 6; ++i ) {
                    if( NEXT_OPPONENT_TEAM[ found ].m_speciesId
                            == PLAYER_TEMP_TEAM[ i ].getSpecies( )
                        || NEXT_OPPONENT_TEAM[ found ].m_heldItem
                               == PLAYER_TEMP_TEAM[ i ].getItem( ) ) {
                        bad = true;
                        break;
                    }
                }
            }
            if( !bad ) { found++; }
        }

        return true;
    }
} // namespace MAP
