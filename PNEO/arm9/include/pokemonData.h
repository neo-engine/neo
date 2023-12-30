/*
Pokémon neo
------------------------------

file        : pokemon.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#include <map>
#include <string>
#include <vector>
#include <nds.h>
#include "battle/type.h"
#include "gen/pokemonNames.h"

enum pkmnLevelUpType {
    LEVELUP_TYPE_MEDIUM_FAST = 0,
    LEVELUP_TYPE_ERRATIC     = 1,
    LEVELUP_TYPE_FLUCTUATING = 2,
    LEVELUP_TYPE_MEDIUM_SLOW = 3,
    LEVELUP_TYPE_FAST        = 4,
    LEVELUP_TYPE_SLOW        = 5
};
enum pkmnColorType {
    COLOR_TYPE_RED    = 0,
    COLOR_TYPE_BLUE   = 1,
    COLOR_TYPE_YELLOW = 2,
    COLOR_TYPE_GREEN  = 3,
    COLOR_TYPE_BLACK  = 4,
    COLOR_TYPE_BROWN  = 5,
    COLOR_TYPE_PURPLE = 6,
    COLOR_TYPE_GRAY   = 7,
    COLOR_TYPE_WHITE  = 8,
    COLOR_TYPE_PINK   = 9
};
enum pkmnGenderType {
    GENDER_TYPE_GENDERLESS = 255,
    GENDER_TYPE_FEMALE     = 254,
    GENDER_TYPE_MALE_125   = 223,
    GENDER_TYPE_MALE_250   = 191,
    GENDER_TYPE_MALE_500   = 127,
    GENDER_TYPE_MALE_750   = 63,
    GENDER_TYPE_MALE_875   = 31,
    GENDER_TYPE_MALE       = 0
};
enum pkmnEggType {
    EGG_TYPE_NONE         = 0,
    EGG_TYPE_MONSTER      = 1,
    EGG_TYPE_WATER1       = 2,
    EGG_TYPE_BUG          = 3,
    EGG_TYPE_FLYING       = 4,
    EGG_TYPE_FIELD        = 5,
    EGG_TYPE_FAIRY        = 6,
    EGG_TYPE_GRASS        = 7,
    EGG_TYPE_HUMANLIKE    = 8,
    EGG_TYPE_WATER3       = 9,
    EGG_TYPE_MINERAL      = 10,
    EGG_TYPE_AMORPHOUS    = 11,
    EGG_TYPE_WATER2       = 12,
    EGG_TYPE_DITTO        = 13,
    EGG_TYPE_DRAGON       = 14,
    EGG_TYPE_UNDISCOVERED = 15
};

enum pkmnNatures {
    NATURE_HARDY   = 0,
    NATURE_LONELY  = 1,
    NATURE_BRAVE   = 2,
    NATURE_ADAMANT = 3,
    NATURE_NAUGHTY = 4,
    NATURE_BOLD    = 5,
    NATURE_DOCILE  = 6,
    NATURE_RELAXED = 7,
    NATURE_IMPISH  = 8,
    NATURE_LAX     = 9,
    NATURE_TIMID   = 10,
    NATURE_HASTY   = 11,
    NATURE_SERIOUS = 12,
    NATURE_JOLLY   = 13,
    NATURE_NAIVE   = 14,
    NATURE_MODEST  = 15,
    NATURE_MILD    = 16,
    NATURE_QUIET   = 17,
    NATURE_BASHFUL = 18,
    NATURE_RASH    = 19,
    NATURE_CALM    = 20,
    NATURE_GENTLE  = 21,
    NATURE_SASSY   = 22,
    NATURE_CAREFUL = 23,
    NATURE_QUIRKY  = 24
};

struct pkmnFormeData {
    BATTLE::type   m_types[ 2 ];
    u16            m_abilities[ 4 ]; // abilities
    u8             m_bases[ 6 ];     // base values (hp, atk, def, satk, sdef, spd)
    u16            m_expYield;
    pkmnGenderType m_genderRatio;  // from pkmnGenderType
    u8             m_size;         // in dm
    u16            m_weight;       // in 100g
    u8             m_colorShape;   // (color << 4) | shape
    u8             m_evYield[ 6 ]; // (hp, atk, def, satk, sdef, spd)
    u16            m_items[ 4 ];   // possible held items: 1%, 5%, 50%, 100%
    u8             m_eggGroups;    // (eg1 << 4) | eg2;

    inline pkmnEggType getEggType( u8 p_eggType ) {
        if( p_eggType ) {
            auto res = pkmnEggType( m_eggGroups >> 4 );
            if( res == EGG_TYPE_NONE ) { return pkmnEggType( 0 ); }
            return res;
        }
        return pkmnEggType( m_eggGroups & 0b1111 );
    }
};

struct pkmnData {
    pkmnFormeData m_baseForme;
    u8            m_expTypeFormeCnt; // (ExpType << 5) | FormeCnt
    u8            m_eggCycles;
    u8            m_catchrate;
    u8            m_baseFriend;

    inline u8 getExpType( ) const {
        return m_expTypeFormeCnt >> 5;
    }
    inline u8 getFormeCnt( ) const {
        return m_expTypeFormeCnt & 0xF;
    }
    inline pkmnEggType getEggType( u8 p_eggType ) {
        return m_baseForme.getEggType( p_eggType );
    }
};

enum evolutionMethod : u8 {
    EVOMETHOD_LEVEL_UP = 0,
    EVOMETHOD_ITEM     = 1,
    EVOMETHOD_TRADE    = 2,
};

enum evolutionType : u8 {
    EVOLUTION_NONE       = 0,
    EVOLUTION_LEVEL      = 20, // level up
    EVOLUTION_ITEM       = 1,  // evolution item
    EVOLUTION_TRADE      = 2,  // trade
    EVOLUTION_TRADE_ITEM = 3,  // trade while holding item
    EVOLUTION_TRADE_PKMN = 4,  // trade with specific species

    EVOLUTION_TIME         = 5,  // level up at specific time
    EVOLUTION_PLACE        = 6,  // level up at specific place
    EVOLUTION_MOVE         = 7,  // level up with certain move known
    EVOLUTION_FRIEND       = 8,  // level up with high friendship
    EVOLUTION_LEVEL_PLACE  = 10, // level up at specific place with a minimum level
    EVOLUTION_FRIEND_TIME  = 11, // level up with high friendship at a specific time
    EVOLUTION_LEVEL_TIME   = 12, // level up at a specific time with a minimum level
    EVOLUTION_LEVEL_GENDER = 13, // level up with a specific gender
    EVOLUTION_ITEM_HOLD    = 24, // hold item + level up + time

    EVOLUTION_ITEM_PLACE  = 22, // evolution item at a special place
    EVOLUTION_ITEM_GENDER = 25, // evolution item with special condition

    EVOLUTION_CONTEST = 15, // A specific contest stat

    EVOLUTION_SPECIAL = 16, // totally crazy stuff
};

struct pkmnEvolution {
    evolutionType m_type;
    u8            m_targetForme;
    u16           m_target;
    u16           m_param1;
    u16           m_param2;
};

struct pkmnEvolveData {
    u16           m_evolutionCount;
    u16           m_preEvolution;
    u32           m_baseEvolution; // (forme << 16) | (speciesidx)
    pkmnEvolution m_evolutions[ 10 ];
};

constexpr bool hasBattleTransform( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_VENUSAUR:
    case PKMN_CHARIZARD:
    case PKMN_BLASTOISE:
    case PKMN_ALAKAZAM:
    case PKMN_GENGAR:
    case PKMN_KANGASKHAN:
    case PKMN_PINSIR:
    case PKMN_GYARADOS:
    case PKMN_AERODACTYL:
    case PKMN_MEWTWO:
    case PKMN_AMPHAROS:
    case PKMN_SCIZOR:
    case PKMN_HERACROSS:
    case PKMN_HOUNDOOM:
    case PKMN_TYRANITAR:
    case PKMN_BLAZIKEN:
    case PKMN_GARDEVOIR:
    case PKMN_MAWILE:
    case PKMN_AGGRON:
    case PKMN_MEDICHAM:
    case PKMN_MANECTRIC:
    case PKMN_BANETTE:
    case PKMN_ABSOL:
    case PKMN_GARCHOMP:
    case PKMN_LUCARIO:
    case PKMN_ABOMASNOW:
    case PKMN_BEEDRILL:
    case PKMN_PIDGEOT:
    case PKMN_SLOWBRO:
    case PKMN_STEELIX:
    case PKMN_SCEPTILE:
    case PKMN_SWAMPERT:
    case PKMN_SABLEYE:
    case PKMN_SHARPEDO:
    case PKMN_CAMERUPT:
    case PKMN_ALTARIA:
    case PKMN_GLALIE:
    case PKMN_SALAMENCE:
    case PKMN_METAGROSS:
    case PKMN_LATIAS:
    case PKMN_LATIOS:
    case PKMN_RAYQUAZA:
    case PKMN_LOPUNNY:
    case PKMN_GALLADE:
    case PKMN_AUDINO:
    case PKMN_DIANCIE:
    case PKMN_KYOGRE:
    case PKMN_GROUDON:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr bool isTradeEvolution( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_ALAKAZAM:
    case PKMN_MACHAMP:
    case PKMN_GOLEM:
    case PKMN_GENGAR:
    case PKMN_POLITOED:
    case PKMN_SLOWKING:
    case PKMN_STEELIX:
    case PKMN_SCIZOR:
    case PKMN_KINGDRA:
    case PKMN_PORYGON2:
    case PKMN_HUNTAIL:
    case PKMN_GOREBYSS:
    case PKMN_MILOTIC:
    case PKMN_RHYPERIOR:
    case PKMN_ELECTIVIRE:
    case PKMN_MAGMORTAR:
    case PKMN_DUSKNOIR:
    case PKMN_PORYGON_Z:
    case PKMN_GIGALITH:
    case PKMN_CONKELDURR:
    case PKMN_ESCAVALIER:
    case PKMN_ACCELGOR:
    case PKMN_AROMATISSE:
    case PKMN_SLURPUFF:
    case PKMN_TREVENANT:
    case PKMN_GOURGEIST:
        return true;
    [[likely]] default:
        return false;
    }
}

/*
 * @brief: Extra species not appearing in icave.
 */
constexpr bool isSecret( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_MIMIKYU:
    case PKMN_ORICORIO:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr bool isMythical( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_MEW:
    case PKMN_CELEBI:
    case PKMN_JIRACHI:
    case PKMN_DEOXYS:
    case PKMN_PHIONE:
    case PKMN_MANAPHY:
    case PKMN_DARKRAI:
    case PKMN_SHAYMIN:
    case PKMN_ARCEUS:
    case PKMN_VICTINI:
    case PKMN_KELDEO:
    case PKMN_MELOETTA:
    case PKMN_GENESECT:
    case PKMN_DIANCIE:
    case PKMN_HOOPA:
    case PKMN_VOLCANION:
    case PKMN_MAGEARNA:
    case PKMN_MARSHADOW:
    case PKMN_ZERAORA:
    case PKMN_MELTAN:
    case PKMN_MELMETAL:
    case PKMN_ZARUDE:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr bool isSpecial( u16 p_speciesId ) {
    if( isMythical( p_speciesId ) ) { return true; }
    switch( p_speciesId ) {
    case PKMN_MEWTWO:
    case PKMN_LUGIA:
    case PKMN_HO_OH:
    case PKMN_KYOGRE:
    case PKMN_GROUDON:
    case PKMN_RAYQUAZA:
    case PKMN_DIALGA:
    case PKMN_PALKIA:
    case PKMN_GIRATINA:
    case PKMN_RESHIRAM:
    case PKMN_ZEKROM:
    case PKMN_KYUREM:
    case PKMN_XERNEAS:
    case PKMN_YVELTAL:
    case PKMN_ZYGARDE:
    case PKMN_COSMOG:
    case PKMN_COSMOEM:
    case PKMN_SOLGALEO:
    case PKMN_LUNALA:
    case PKMN_NECROZMA:
    case PKMN_ZACIAN:
    case PKMN_ZAMAZENTA:
    case PKMN_ETERNATUS:
    case PKMN_KUBFU:
    case PKMN_URSHIFU:
    case PKMN_GLASTRIER:
    case PKMN_SPECTRIER:
    case PKMN_CALYREX:
    case PKMN_MIRAIDON:
    case PKMN_KORAIDON:
    case PKMN_OKIDOGI:
    case PKMN_MUNKIDORI:
    case PKMN_FEZANDIPITI:
    case PKMN_OGERPON:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr bool isLegendary( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_ARTICUNO:
    case PKMN_ZAPDOS:
    case PKMN_MOLTRES:
    case PKMN_MEWTWO:
    case PKMN_RAIKOU:
    case PKMN_ENTEI:
    case PKMN_SUICUNE:
    case PKMN_LUGIA:
    case PKMN_HO_OH:
    case PKMN_REGIROCK:
    case PKMN_REGICE:
    case PKMN_REGISTEEL:
    case PKMN_LATIAS:
    case PKMN_LATIOS:
    case PKMN_KYOGRE:
    case PKMN_GROUDON:
    case PKMN_RAYQUAZA:
    case PKMN_UXIE:
    case PKMN_MESPRIT:
    case PKMN_AZELF:
    case PKMN_DIALGA:
    case PKMN_PALKIA:
    case PKMN_HEATRAN:
    case PKMN_REGIGIGAS:
    case PKMN_GIRATINA:
    case PKMN_CRESSELIA:
    case PKMN_COBALION:
    case PKMN_TERRAKION:
    case PKMN_VIRIZION:
    case PKMN_TORNADUS:
    case PKMN_THUNDURUS:
    case PKMN_RESHIRAM:
    case PKMN_ZEKROM:
    case PKMN_LANDORUS:
    case PKMN_KYUREM:
    case PKMN_XERNEAS:
    case PKMN_YVELTAL:
    case PKMN_ZYGARDE:
    case PKMN_TYPE__NULL:
    case PKMN_SILVALLY:
    case PKMN_TAPU_KOKO:
    case PKMN_TAPU_LELE:
    case PKMN_TAPU_BULU:
    case PKMN_TAPU_FINI:
    case PKMN_COSMOG:
    case PKMN_COSMOEM:
    case PKMN_SOLGALEO:
    case PKMN_LUNALA:
    case PKMN_NECROZMA:
    case PKMN_ZACIAN:
    case PKMN_ZAMAZENTA:
    case PKMN_ETERNATUS:
    case PKMN_KUBFU:
    case PKMN_URSHIFU:
    case PKMN_REGIELEKI:
    case PKMN_REGIDRAGO:
    case PKMN_GLASTRIER:
    case PKMN_SPECTRIER:
    case PKMN_CALYREX:
    case PKMN_ENAMORUS:
    case PKMN_WO_CHIEN:
    case PKMN_CHIEN_PAO:
    case PKMN_TING_LU:
    case PKMN_CHI_YU:
    case PKMN_MIRAIDON:
    case PKMN_KORAIDON:
    case PKMN_OKIDOGI:
    case PKMN_MUNKIDORI:
    case PKMN_FEZANDIPITI:
    case PKMN_OGERPON:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr bool isUltraBeast( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_NIHILEGO:
    case PKMN_BUZZWOLE:
    case PKMN_PHEROMOSA:
    case PKMN_XURKITREE:
    case PKMN_CELESTEELA:
    case PKMN_KARTANA:
    case PKMN_GUZZLORD:
    case PKMN_POIPOLE:
    case PKMN_NAGANADEL:
    case PKMN_STAKATAKA:
    case PKMN_BLACEPHALON:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr bool isParadox( u16 p_speciesId ) {
    switch( p_speciesId ) {
    case PKMN_IRON_TREADS:
    case PKMN_IRON_BUNDLE:
    case PKMN_IRON_HANDS:
    case PKMN_IRON_JUGULIS:
    case PKMN_IRON_MOTH:
    case PKMN_IRON_THORNS:
    case PKMN_IRON_VALIANT:
    case PKMN_IRON_LEAVES:
    case PKMN_GREAT_TUSK:
    case PKMN_SCREAM_TAIL:
    case PKMN_BRUTE_BONNET:
    case PKMN_FLUTTER_MANE:
    case PKMN_SLITHER_WING:
    case PKMN_SANDY_SHOCKS:
    case PKMN_ROARING_MOON:
    case PKMN_WALKING_WAKE:
        return true;
    [[likely]] default:
        return false;
    }
}

constexpr u8 NatMod[ 25 ][ 5 ] = {
    { 10, 10, 10, 10, 10 }, { 11, 9, 10, 10, 10 }, { 11, 10, 10, 10, 9 },  { 11, 10, 9, 10, 10 },
    { 11, 10, 10, 9, 10 },  { 9, 11, 10, 10, 10 }, { 10, 10, 10, 10, 10 }, { 10, 11, 10, 10, 9 },
    { 10, 11, 9, 10, 10 },  { 10, 11, 10, 9, 10 }, { 9, 10, 10, 10, 11 },  { 10, 9, 10, 10, 11 },
    { 10, 10, 10, 10, 10 }, { 10, 10, 9, 10, 11 }, { 10, 10, 10, 9, 11 },  { 9, 10, 11, 10, 10 },
    { 10, 9, 11, 10, 10 },  { 10, 10, 11, 10, 9 }, { 10, 10, 10, 10, 10 }, { 10, 10, 11, 9, 10 },
    { 9, 10, 10, 11, 10 },  { 10, 9, 10, 11, 10 }, { 10, 10, 10, 11, 9 },  { 10, 10, 9, 11, 10 },
    { 10, 10, 10, 10, 10 } };

/*
 * @brief: max amount of exp any pkmn requires to reach lv 100.
 */
constexpr u32 MAX_EXP = 1640000;

/*
 * @brief: lut for speed; min exp required to reach a certain level.
 */
constexpr u32 EXP[ 100 ][ 6 ] = { { 0, 0, 0, 0, 0, 0 },
                                  { 15, 6, 8, 9, 10, 4 },
                                  { 52, 21, 27, 57, 33, 13 },
                                  { 122, 51, 64, 96, 80, 32 },
                                  { 237, 100, 125, 135, 156, 65 },
                                  { 406, 172, 216, 179, 270, 112 },
                                  { 637, 274, 343, 236, 428, 178 },
                                  { 942, 409, 512, 314, 640, 276 },
                                  { 1326, 583, 729, 419, 911, 393 },
                                  { 1800, 800, 1000, 560, 1250, 540 },
                                  { 2369, 1064, 1331, 742, 1663, 745 },
                                  { 3041, 1382, 1728, 973, 2160, 967 },
                                  { 3822, 1757, 2197, 1261, 2746, 1230 },
                                  { 4719, 2195, 2744, 1612, 3430, 1591 },
                                  { 5737, 2700, 3375, 2035, 4218, 1957 },
                                  { 6881, 3276, 4096, 2535, 5120, 2457 },
                                  { 8155, 3930, 4913, 3120, 6141, 3046 },
                                  { 9564, 4665, 5832, 3798, 7290, 3732 },
                                  { 11111, 5487, 6859, 4575, 8573, 4526 },
                                  { 12800, 6400, 8000, 5460, 10000, 5440 },
                                  { 14632, 7408, 9261, 6458, 11576, 6482 },
                                  { 16610, 8518, 10648, 7577, 13310, 7666 },
                                  { 18737, 9733, 12167, 8825, 15208, 9003 },
                                  { 21012, 11059, 13824, 10208, 17280, 10506 },
                                  { 23437, 12500, 15625, 11735, 19531, 12187 },
                                  { 26012, 14060, 17576, 13411, 21970, 14060 },
                                  { 28737, 15746, 19683, 15244, 24603, 16140 },
                                  { 31610, 17561, 21952, 17242, 27440, 18439 },
                                  { 34632, 19511, 24389, 19411, 30486, 20974 },
                                  { 37800, 21600, 27000, 21760, 33750, 23760 },
                                  { 41111, 23832, 29791, 24294, 37238, 26811 },
                                  { 44564, 26214, 32768, 27021, 40960, 30146 },
                                  { 48155, 28749, 35937, 29949, 44921, 33780 },
                                  { 51881, 31443, 39304, 33084, 49130, 37731 },
                                  { 55737, 34300, 42875, 36435, 53593, 42017 },
                                  { 59719, 37324, 46656, 40007, 58320, 46656 },
                                  { 63822, 40522, 50653, 43808, 63316, 50653 },
                                  { 68041, 43897, 54872, 47846, 68590, 55969 },
                                  { 72369, 47455, 59319, 52127, 74148, 60505 },
                                  { 76800, 51200, 64000, 56660, 80000, 66560 },
                                  { 81326, 55136, 68921, 61450, 86151, 71677 },
                                  { 85942, 59270, 74088, 66505, 92610, 78533 },
                                  { 90637, 63605, 79507, 71833, 99383, 84277 },
                                  { 95406, 68147, 85184, 77440, 106480, 91998 },
                                  { 100237, 72900, 91125, 83335, 113906, 98415 },
                                  { 105122, 77868, 97336, 89523, 121670, 107069 },
                                  { 110052, 83058, 103823, 96012, 129778, 114205 },
                                  { 115015, 88473, 110592, 102810, 138240, 123863 },
                                  { 120001, 94119, 117649, 109923, 147061, 131766 },
                                  { 125000, 100000, 125000, 117360, 156250, 142500 },
                                  { 131324, 106120, 132651, 125126, 165813, 151222 },
                                  { 137795, 112486, 140608, 133229, 175760, 163105 },
                                  { 144410, 119101, 148877, 141677, 186096, 172697 },
                                  { 151165, 125971, 157464, 150476, 196830, 185807 },
                                  { 158056, 133100, 166375, 159635, 207968, 196322 },
                                  { 165079, 140492, 175616, 169159, 219520, 210739 },
                                  { 172229, 148154, 185193, 179056, 231491, 222231 },
                                  { 179503, 156089, 195112, 189334, 243890, 238036 },
                                  { 186894, 164303, 205379, 199999, 256723, 250562 },
                                  { 194400, 172800, 216000, 211060, 270000, 267840 },
                                  { 202113, 181584, 226981, 222522, 283726, 281456 },
                                  { 209728, 190662, 238328, 234393, 297910, 300293 },
                                  { 217540, 200037, 250047, 246681, 312558, 315059 },
                                  { 225443, 209715, 262144, 259392, 327680, 335544 },
                                  { 233431, 219700, 274625, 272535, 343281, 351520 },
                                  { 241496, 229996, 287496, 286115, 359370, 373744 },
                                  { 249633, 240610, 300763, 300140, 375953, 390991 },
                                  { 257834, 251545, 314432, 314618, 393040, 415050 },
                                  { 267406, 262807, 328509, 329555, 410636, 433631 },
                                  { 276458, 274400, 343000, 344960, 428750, 459620 },
                                  { 286328, 286328, 357911, 360838, 447388, 479600 },
                                  { 296358, 298598, 373248, 377197, 466560, 507617 },
                                  { 305767, 311213, 389017, 394045, 486271, 529063 },
                                  { 316074, 324179, 405224, 411388, 506530, 559209 },
                                  { 326531, 337500, 421875, 429235, 527343, 582187 },
                                  { 336255, 351180, 438976, 447591, 548720, 614566 },
                                  { 346965, 365226, 456533, 466464, 570666, 639146 },
                                  { 357812, 379641, 474552, 485862, 593190, 673863 },
                                  { 367807, 394431, 493039, 505791, 616298, 700115 },
                                  { 378880, 409600, 512000, 526260, 640000, 737280 },
                                  { 390077, 425152, 531441, 547274, 664301, 765275 },
                                  { 400293, 441094, 551368, 568841, 689210, 804997 },
                                  { 411686, 457429, 571787, 590969, 714733, 834809 },
                                  { 423190, 474163, 592704, 613664, 740880, 877201 },
                                  { 433572, 491300, 614125, 636935, 767656, 908905 },
                                  { 445239, 508844, 636056, 660787, 795070, 954084 },
                                  { 457001, 526802, 658503, 685228, 823128, 987754 },
                                  { 467489, 545177, 681472, 710266, 851840, 1035837 },
                                  { 479378, 563975, 704969, 735907, 881211, 1071552 },
                                  { 491346, 583200, 729000, 762160, 911250, 1122660 },
                                  { 501878, 602856, 753571, 789030, 941963, 1160499 },
                                  { 513934, 622950, 778688, 816525, 973360, 1214753 },
                                  { 526049, 643485, 804357, 844653, 1005446, 1254796 },
                                  { 536557, 664467, 830584, 873420, 1038230, 1312322 },
                                  { 548720, 685900, 857375, 902835, 1071718, 1354652 },
                                  { 560922, 707788, 884736, 932903, 1105920, 1415577 },
                                  { 571333, 730138, 912673, 963632, 1140841, 1460276 },
                                  { 583539, 752953, 941192, 995030, 1176490, 1524731 },
                                  { 591882, 776239, 970299, 1027103, 1212873, 1571884 },
                                  { 600000, 800000, 1000000, 1059860, 1250000, 1640000 } };
