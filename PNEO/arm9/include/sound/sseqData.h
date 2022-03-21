/*
    Pokémon neo
    ------------------------------

    file        : sseqData.h
    author      : Philip Wellnitz
    description : Header file.

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

#pragma once
#ifndef NO_SOUND
#include "defines.h"
#include "gen/bgmNames.h"

namespace SOUND::SSEQ {
    constexpr u8 MAX_SAMPLES_PER_SSEQ = 4;
    constexpr u8 NUM_SSEQ             = 100;
    struct sseqData {
        u16 m_bank; // bank used for sseq
        u16 m_sseqId;
        u16 m_sampleCnt;
        u16 m_samplesId[ MAX_SAMPLES_PER_SSEQ ];
    };

    extern const sseqData SSEQ_LIST[ NUM_SSEQ ];

    constexpr int SSEQ_NONE = -1;

    constexpr int BGMIndexForName( unsigned p_name ) {
        switch( p_name ) {
        case BGM_NONE:
        default: return SSEQ_NONE;
        case BGM_LITTLEROOT_TOWN: return SSEQ_NONE;
        case BGM_OLDALE_TOWN: return SSEQ_NONE;
        case BGM_PETALBURG_CITY: return SSEQ_NONE;
        case BGM_RUSTBORO_CITY: return SSEQ_NONE;
        case BGM_DEWFORD_TOWN: return SSEQ_NONE;
        case BGM_SLATEPORT_CITY: return SSEQ_NONE;
        case BGM_VERDANTURF_TOWN: return SSEQ_NONE;
        case BGM_FALLARBOR_TOWN: return SSEQ_NONE;
        case BGM_FORTREE_CITY: return SSEQ_NONE;
        case BGM_LILYCOVE_CITY: return SSEQ_NONE;
        case BGM_NEW_LILYCOVE_CITY: return SSEQ_NONE;
        case BGM_SOOTOPOLIS_CITY: return SSEQ_NONE;
        case BGM_CLIFFELTA_CITY: return SSEQ_NONE;
        case BGM_EX07: return SSEQ_NONE;
        case BGM_ROUTE_101: return SSEQ_NONE;
        case BGM_ROUTE_104: return SSEQ_NONE;
        case BGM_ROUTE_110: return SSEQ_NONE;
        case BGM_ROUTE_113: return SSEQ_NONE;
        case BGM_ROUTE_119: return SSEQ_NONE;
        case BGM_ROUTE_120: return SSEQ_NONE;
        case BGM_ROUTE_123: return SSEQ_NONE;
        case BGM_ROUTE_135: return SSEQ_NONE;
        case BGM_ROUTE_38: return SSEQ_NONE;
        case BGM_ROUTE_10: return 4;
        case BGM_POKEMON_CENTER: return 2;
        case BGM_POKEMON_GYM: return 1;
        case BGM_POKEMON_MART: return 25;
        case BGM_PROF_BIRCH_LAB: return SSEQ_NONE;
        case BGM_CAVE_FORESTS: return SSEQ_NONE;
        case BGM_EX03: return 0; // floaroma town
        case BGM_TRAINER_SCHOOL: return SSEQ_NONE;
        case BGM_ABANDONED_SHIP: return SSEQ_NONE;
        case BGM_MARINE_SCIENCE_MUSEUM: return SSEQ_NONE;
        case BGM_TRICK_HOUSE: return SSEQ_NONE;
        case BGM_NEW_MAUVILLE: return SSEQ_NONE;
        case BGM_MT_CHIMNEY: return SSEQ_NONE;
        case BGM_METEOR_FALLS: return SSEQ_NONE;
        case BGM_SAFARI_ZONE: return SSEQ_NONE;
        case BGM_LILYCOVE_MUSEUM: return SSEQ_NONE;
        case BGM_MT_PYRE_PEAK: return SSEQ_NONE;
        case BGM_TEAM_AM_HIDEOUT: return SSEQ_NONE;
        case BGM_SHOAL_CAVE: return SSEQ_NONE;
        case BGM_VICTORY_ROAD: return SSEQ_NONE;
        case BGM_SEALED_CHAMBER: return 10;
        case BGM_CRYSTAL_CAVERN: return 5; // (defunct)
        case BGM_BATTLE_FRONTIER: return SSEQ_NONE;
        case BGM_BATTLE_TOWER: return SSEQ_NONE;
        case BGM_BATTLE_ARENA: return SSEQ_NONE;
        case BGM_BATTLE_FACTORY: return SSEQ_NONE;
        case BGM_BATTLE_PALACE: return SSEQ_NONE;
        case BGM_BATTLE_PYRAMID_PEAK: return SSEQ_NONE;
        case BGM_BATTLE_WILD: return SSEQ_NONE;
        case BGM_BATTLE_WILD_ALT: return SSEQ_NONE;
        case BGM_BATTLE_REGI: return 17;
        case BGM_BATTLE_RAYQUAZA: return SSEQ_NONE;
        case BGM_BATTLE_RAIKOU_ENTEI_SUICUNE: return SSEQ_NONE;
        case BGM_BATTLE_EX01: return 15; // sinnoh battle
        case BGM_BATTLE_DIALGA_PALKIA: return 16;
        case BGM_BATTLE_TRAINER: return SSEQ_NONE;
        case BGM_BATTLE_MAY_BRENDAN: return SSEQ_NONE;
        case BGM_BATTLE_TEAM_AM: return SSEQ_NONE;
        case BGM_BATTLE_GYM_LEADER: return 13;
        case BGM_BATTLE_CHAMPION: return 14;
        case BGM_BATTLE_FRONTIER_BRAIN: return SSEQ_NONE;
        case BGM_VICTORY_GYM_LEADER: return SSEQ_NONE;
        case BGM_VICTORY_TEAM_AM_GRUNT: return SSEQ_NONE;
        case BGM_VICTORY_TRAINER: return SSEQ_NONE;
        case BGM_VICTORY_WILD: return SSEQ_NONE;
        case BGM_ENCOUNTER_BRENDAN: return SSEQ_NONE;
        case BGM_ENCOUNTER_MAY: return SSEQ_NONE;
        case BGM_ENCOUNTER_TEAM_AM: return SSEQ_NONE;
        case BGM_ENCOUNTER_ELECTRIC: return SSEQ_NONE;
        case BGM_ENCOUNTER_HIKER: return 12;
        case BGM_ENCOUNTER_SAILOR: return SSEQ_NONE;
        case BGM_ENCOUNTER_TUBER: return SSEQ_NONE;
        case BGM_ENCOUNTER_TWINS: return SSEQ_NONE;
        case BGM_ENCOUNTER_YOUNGSTER: return SSEQ_NONE;
        case BGM_ENCOUNTER_SHIRONA: return 11;
        case BGM_HELP_PROF_BIRCH: return SSEQ_NONE;
        case BGM_SHOW_ME_AROUND: return SSEQ_NONE;
        case BGM_CYCLING: return SSEQ_NONE;
        case BGM_SURFING: return SSEQ_NONE;
        case BGM_SURFING_ALT: return SSEQ_NONE;
        case BGM_SAILING: return SSEQ_NONE;
        case BGM_DIVING: return 24;
        case BGM_UNWAVERING_EMOTIONS: return 18; // (defunct)
        case BGM_DESERT: return SSEQ_NONE;
        case BGM_CABLE_CAR: return SSEQ_NONE;
        case BGM_EVOLVING: return 26;
        case BGM_MYSTERY_GIFT: return 20;
        case BGM_OS_BADGE: return 21;
        case BGM_OS_SYMBOL: return SSEQ_NONE;
        case BGM_OS_TM: return 22;
        case BGM_OS_EVOLVED: return SSEQ_NONE;
        case BGM_OS_HEAL_PKMN: return 23;
        case BGM_OS_LEVEL_UP: return SSEQ_NONE;
        case BGM_OS_PKMN_CAPTURE: return SSEQ_NONE;
        case BGM_EX01: return SSEQ_NONE;
        case BGM_EX02: return SSEQ_NONE;
        case BGM_MT_MOON: return SSEQ_NONE;
        case BGM_ECRUTEAK_CITY: return SSEQ_NONE;
        case BGM_EX04: return SSEQ_NONE;
        case BGM_EX05: return SSEQ_NONE;
        case BGM_LAKE_CAVE: return 6;
        case BGM_DISTORTION_WORLD: return 9;
        case BGM_EX06: return SSEQ_NONE;
        case BGM_POKE_RADAR: return 19;
        case BGM_GHOST_GIRL: return SSEQ_NONE;

        case BGM_UNOVA_GATE: return 3;
        case BGM_ETERNA_FOREST: return 7;
        case BGM_SPRING_PATH: return 8;
        }
    }
} // namespace SOUND::SSEQ
#endif
