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

#include "fs/fs.h"
#include "gen/trainerClassNames.h"
#include "map/mapBattleFacilityDefines.h"
#include "map/mapDrawer.h"

namespace MAP {
    constexpr u8 RSID_SINGLE_LV50  = 0;
    constexpr u8 RSID_SINGLE_LV100 = 1;
    constexpr u8 RSID_SINGLE_LV30  = 2;
    constexpr u8 RSID_DOUBLE_LV50  = 3;
    constexpr u8 RSID_DOUBLE_LV100 = 4;

    const ruleSet FACILITY_RULE_SETS[ NUM_RULESETS ] = {
        { RSID_SINGLE_LV50, 50, 3, true, 7, BATTLE::BM_SINGLE },
        { RSID_SINGLE_LV100, 100, 3, true, 7, BATTLE::BM_SINGLE },
        { RSID_SINGLE_LV30, 30, 3, false, 3, BATTLE::BM_SINGLE },  // battle tents
        { RSID_DOUBLE_LV50, 50, 3, true, 7, BATTLE::BM_DOUBLE },   // battle tower
        { RSID_DOUBLE_LV100, 100, 3, true, 7, BATTLE::BM_DOUBLE }, // battle tower
    };

    constexpr u8 SPECIAL_BATTLE_1 = 21;
    constexpr u8 SPECIAL_BATTLE_2 = 42;

    constexpr u8 BATTLE_FACILITY_NUM_TRAINER_CLASSES = 40;

    /*
     * @brief: Possible trainer classes for (random) opponents in any of the battle
     * facilities.
     */
    constexpr u8 BATTLE_FACILITY_TRAINER_CLASSES[ BATTLE_FACILITY_NUM_TRAINER_CLASSES ]
        = { TC_AROMA_LADY,
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

    pokemon               PLAYER_TEMP_TEAM[ 6 ];
    BATTLE::battleTrainer NEXT_OPPONENT;

    bool createNextOpponentTrainer( const ruleSet& p_rules, u16 p_streak, bool p_randomTeam ) {
        std::memset( &NEXT_OPPONENT, 0, sizeof( BATTLE::battleTrainer ) );

        // choose a random trainer class
        u8 tclass = rand( ) % BATTLE_FACILITY_NUM_TRAINER_CLASSES;

        // load strings for a random trainer of the chosen trainer class
        if( !FS::loadBattleFacilityTrainerStrings( tclass, rand( ) % TRAINERS_PER_CLASS,
                                                   &NEXT_OPPONENT ) ) {
            return false;
        }

        NEXT_OPPONENT.m_data.m_numPokemonEasy = NEXT_OPPONENT.m_data.m_numPokemonNormal
            = NEXT_OPPONENT.m_data.m_numPokemonHard
            = p_rules.m_battleMode == BATTLE::BM_SINGLE ? 3 : 4;
        if( p_randomTeam ) {
            // choose random pkmn
            for( u8 i = 0; i < NEXT_OPPONENT.m_data.m_numPokemonNormal; ++i ) {
                u16 species = 0;
                u8  variant = NO_VARIANT;

                while( !species || variant == NO_VARIANT ) {
                    species = rand( ) % MAX_PKMN;
                    variant = getSpeciesVariantForStreak( species, p_streak );

                    if( variant == NO_VARIANT ) { continue; }

                    if( !FS::loadBattleFacilityPkmn( species, variant, p_rules.m_level, p_streak,
                                                     &NEXT_OPPONENT.m_data.m_pokemon[ i ] ) ) {
                        species = 0;
                        continue;
                    }

                    for( u8 j = 0; j < i; ++j ) {
                        if( NEXT_OPPONENT.m_data.m_pokemon[ j ].getSpecies( ) == species
                            || ( NEXT_OPPONENT.m_data.m_pokemon[ i ].getItem( )
                                 && NEXT_OPPONENT.m_data.m_pokemon[ j ].getItem( )
                                        == NEXT_OPPONENT.m_data.m_pokemon[ i ].getItem( ) ) ) {
                            species = 0;
                            continue;
                        }
                    }
                }
            }
        } else {
            // load team
            if( !FS::loadBattleFacilityTrainerTeam( p_rules, tclass, rand( ) % TEAMS_PER_CLASS,
                                                    p_streak, &NEXT_OPPONENT ) ) {
                return false;
            }
        }

        NEXT_OPPONENT.m_data.m_AILevel = ( 4 + p_streak > 9 ) ? 9 : 4 + p_streak;
        return true;
    }

    void mapDrawer::runBattleFactory( const ruleSet& p_rules ) {
        if( p_rules.m_id != RSID_SINGLE_LV50 && p_rules.m_id != RSID_SINGLE_LV100
            && p_rules.m_id != RSID_SINGLE_LV30 ) {
            return;
        }

        // obtain basic data
        auto streakVar
            = p_rules.m_id == RSID_SINGLE_LV50
                  ? SAVE::V_BATTLE_FACTORY_50_STREAK
                  : ( p_rules.m_id == RSID_SINGLE_LV100 ? SAVE::V_BATTLE_FACTORY_100_STREAK
                                                        : SAVE::V_SLATEPORT_BATTLE_TENT_STREAK );
        auto streakActiveFlag = p_rules.m_id == RSID_SINGLE_LV50
                                    ? SAVE::F_BATTLE_FACTORY_50_STREAK_ONGOING
                                    : ( p_rules.m_id == RSID_SINGLE_LV100
                                            ? SAVE::F_BATTLE_FACTORY_100_STREAK_ONGOING
                                            : SAVE::F_SLATEPORT_BATTLE_TENT_STREAK_ONGOING );
        u16  currentStreak    = SAVE::SAV.getActiveFile( ).getVar( streakVar );
        bool streakOngoing    = SAVE::SAV.getActiveFile( ).checkFlag( streakActiveFlag );
        auto battlePolicy     = getBattlePolicy( false, p_rules.m_battleMode, false );

        if( !streakOngoing ) {
            currentStreak = 0;
            SAVE::SAV.getActiveFile( ).setVar( streakVar, currentStreak );
            SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, true );
        }

        // initial pkmn selection

        for( u8 battle = 0; battle < p_rules.m_numBattles; ++battle ) {
            if( battle ) {
                // in-between battle message, heal pkmn team

                // ask if player wants to continue
                if( false ) {
                    // reset streak
                    SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, true );
                    break;
                }
            }

            // create next opponent
            if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_1 ) {
                // load special battle 1
            } else if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_2 ) {
                // load special battle 2
            } else {
                if( !createNextOpponentTrainer( p_rules, currentStreak / 7, true ) ) {
                    // やばい、次の相手が見つけられなかった

                    // TODO: どうしよう??
                }
            }

            // run battle

            if( true ) {
                // player won

                if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_1 ) {
                    // some message
                    // award siver symbol
                    awardBadge( 1, 11 );
                }
                if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_2 ) {
                    // some message
                    // award gold symbol
                    awardBadge( 1, 12 );
                }

                currentStreak++;
                SAVE::SAV.getActiveFile( ).setVar( streakVar, currentStreak );
                // move player to clerk

            } else {
                // player lost
                // reset streak
                SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, false );
                break;
            }
        }
        // return player to initial position, give back pkmn

        // check if chain is still active
        if( true ) {
            // hand prize(s) to player, depending on the streak
        }
    }
} // namespace MAP
