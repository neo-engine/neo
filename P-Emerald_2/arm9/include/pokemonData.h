/*
Pokémon neo
------------------------------

file        : pokemon.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#pragma once

#include <map>
#include <string>
#include <vector>
#include <nds.h>
#include "type.h"

#undef RAND_MAX
#define RAND_MAX 4294967295

enum pkmnLevelUpType {
    MEDIUM_FAST = 0,
    ERRATIC     = 1,
    FLUCTUATING = 2,
    MEDIUM_SLOW = 3,
    FAST        = 4,
    SLOW        = 5
};
enum pkmnColorType {
    RED_    = 0,
    BLUE_   = 1,
    YELLOW_ = 2,
    GREEN_  = 3,
    BLACK_  = 4,
    BROWN_  = 5,
    PURPLE_ = 6,
    GRAY_   = 7,
    WHITE_  = 8,
    PINK_   = 9
};
enum pkmnGenderType {
    GENDERLESS = 255,
    FEMALE     = 254,
    MALE_125   = 223,
    MALE_250   = 191,
    MALE_500   = 127,
    MALE_750   = 63,
    MALE_875   = 31,
    MALE       = 0
};
enum pkmnEggType {
    NONE_      = 0,
    MONSTER_   = 1,
    WASSER1_   = 2,
    WATER1_    = 2,
    KAEFER_    = 4,
    BUG_       = 4,
    FLUG_      = 5,
    FLYING_    = 5,
    FELD_      = 6,
    FIELD_     = 6,
    FEE_       = 7,
    FAIRY_     = 7,
    PFLANZE_   = 8,
    GRASS_     = 8,
    HUMANOTYP_ = 9,
    HUMANLIKE_ = 9,
    WASSER3_   = 10,
    WATER3_    = 10,
    MINERAL_   = 11,
    AMORPH_    = 12,
    WASSER2_   = 13,
    WATER2_    = 13,
    DITTO_     = 14,
    DRACHE_    = 15,
    DRAGON_    = 15,
    UNBEKANNT_ = 16,
    UNDISCOVERED_ = 16
};

enum pkmnNatures {
    ROBUST  = 0,
    HARDY   = 0,
    EINSAM  = 1,
    LONELY  = 1,
    MUTIG   = 2,
    BRAVE   = 2,
    HART    = 3,
    ADAMANT = 3,
    FRECH   = 4,
    NAUGHY  = 4,
    KUEHN   = 5,
    BOLD    = 5,
    SANFT   = 6,
    DOCILE  = 6,
    LOCKER  = 7,
    RELAXED = 7,
    PFIFFIG = 8,
    IMPISH  = 8,
    LASCH   = 9,
    LAX     = 9,
    SCHEU   = 10,
    TIMID   = 10,
    HASTIG  = 11,
    NASTY   = 11,
    ERNST   = 12,
    SERIOUS = 12,
    FROH    = 13,
    JOLLY   = 13,
    NAIV    = 14,
    NAIVE   = 14,
    MAESSIG = 15,
    MODEST  = 15,
    MILD    = 16,
    RUHIG   = 17,
    QUIET   = 17,
    ZAGHAFT = 18,
    BASHFUL = 18,
    HITZIG  = 19,
    RASH    = 19,
    STILL   = 20,
    CALM    = 20,
    ZART    = 21,
    GENTLE  = 21,
    FORSCH  = 22,
    SASSY   = 22,
    SACHT   = 23,
    CAREFUL = 23,
    KAUZIG  = 24,
    QUIRKY  = 24
};

struct pkmnFormeData {
    type            m_types[ 2 ];
    u16             m_abilities[ 4 ]; // abilities
    u8              m_bases[ 6 ]; // base values (hp, atk, def, satk, sdef, spd)
    u16             m_expYield;
    pkmnGenderType  m_genderRatio; // from pkmnGenderType
    u8              m_size; // in dm
    u16             m_weight; // in 100g
    u8              m_colorShape; // (color << 4) | shape
    u8              m_evYield[ 6 ]; // (hp, atk, def, satk, sdef, spd)
    u16             m_items[ 4 ]; // possible held items: 1%, 5%, 50%, 100%
    u8              m_eggGroups; // (eg1 << 4) | eg2;
};

struct pkmnData {
    pkmnFormeData   m_baseForme;
    u8              m_expTypeFormeCnt; // (ExpType << 5) | FormeCnt
    u8              m_eggCycles;
    u8              m_catchrate;
    u8              m_baseFriend;

    u8 getExpType( ) const {
        return m_expTypeFormeCnt >> 5;
    }
};


// deprecated
struct pokemonData {
    type           m_types[ 2 ];
    u16            m_bases[ 6 ];
    u16            m_catchrate;
    u16            m_items[ 4 ];
    pkmnGenderType m_gender;
    pkmnEggType    m_eggT[ 2 ];
    u16            m_eggcyc;
    u16            m_baseFriend;
    u16            m_EVYield[ 6 ];
    u16            m_EXPYield;
    u16            m_formecnt;
    u16            m_size;
    u16            m_weight;
    u16            m_expType;
    u16            m_abilities[ 4 ];

    union {
        struct evolvestruct {
            s16 m_evolvesInto;
            s16 m_evolutionTrigger;
            s16 m_evolveItem;
            s16 m_evolveLevel;
            s16 m_evolveGender;
            s16 m_evolveLocation;
            s16 m_evolveHeldItem;
            s16 m_evolveDayTime;
            s16 m_evolveKnownMove;
            s16 m_evolveKnownMoveType;
            s16 m_evolveFriendship;
            s16 m_evolveMinimumBeauty;
            s16 m_evolveAtkDefRelation; // 1: >; 2: ==; 3 <
            s16 m_evolveAdditionalPartyMember;
            s16 m_evolveTradeSpecies;
        } m_e;
        s16 m_evolveData[ 15 ];
    } m_evolutions[ 7 ];
    char m_displayName[ 15 ];
    char m_species[ 50 ];
    char m_dexEntry[ 200 ];

    u16  m_formeIdx[ 30 ];
    char m_formeName[ 30 ][ 15 ];

    u16 m_preEvolution;
    u8  m_stage; // 0: Basic, 1: Stage 1, 2: Stage 2, 3 Restored (m_preEvolution: itemIdx)
};

std::string getDisplayName( u16 p_pkmnId, u8 p_language, u8 p_forme = 0 );
bool        getDisplayName( u16 p_pkmnId, char* p_name, u8 p_language, u8 p_forme = 0 );

pkmnData    getPkmnData( const u16 p_pkmnId, const u8 p_forme = 0 );
bool        getPkmnData( const u16 p_pkmnId, pkmnData* p_out );
bool        getPkmnData( const u16 p_pkmnId, const u8 p_forme, pkmnData* p_out );

[[deprecated]]
bool        getAll( u16 p_pkmnId, pokemonData& out, u8 p_forme = 0 );

const u16 LEARN_TM = 200;
const u16 LEARN_TUTOR = 201;
const u16 LEARN_EGG = 202;

void getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_num, u16* p_res );
bool canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_maxLevel );

extern double NatMod[ 25 ][ 5 ];
extern u32 EXP[ 100 ][ 6 ];
