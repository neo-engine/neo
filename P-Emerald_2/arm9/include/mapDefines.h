/*
Pokémon Emerald 2 Version
------------------------------

file        : mapDefines.h
author      : Philip Wellnitz
description : General map stuff.

Copyright (C) 2012 - 2017
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

#pragma once
#include <map>
#include <string>
#include <nds/ndstypes.h>

#define MAP_HOENN 10
#define MAP_METEOR_FALLS 11
#define MAP_DESERT_RUINS 12
#define MAP_ISLAND_CAVE 13
#define MAP_ANCIENT_TOMB 14

#define MAP_KANTO 110
#define MAP_TOHJO_FALLS 111
#define MAP_KANTO_VICTORY_ROAD 112
#define MAP_VIRIDIAN_FOREST 113
#define MAP_SEAFOAM_ISLANDS 114
#define MAP_MT_MOON 115
#define MAP_DIGLETTS_CAVE 116
#define MAP_ROCK_TUNNEL 117
#define MAP_CERULEAN_CAVE 118

#define MAP_JOHTO 210
#define MAP_DARK_CAVE 211
#define MAP_SPROUT_TOWER 212
#define MAP_RUINS_OF_ALPH 213
#define MAP_UNION_CAVE 214
#define MAP_SLOWPOKE_WELL 215
#define MAP_ILEX_FOREST 216
#define MAP_NATIONAL_PARK 217
#define MAP_BURNED_TOWER 218
#define MAP_BELL_TOWER 219
#define MAP_WHIRL_ISLANDS 220
#define MAP_MT_MORTAR 221
#define MAP_ICE_PATH 222
#define MAP_DRAGONS_DEN 223

namespace MAP {
    struct position {
        u16 m_posX; // Global
        u16 m_posY; // Global
        u8  m_posZ;
    };
    enum direction : u8 { UP, RIGHT, DOWN, LEFT };
    enum moveMode {
        // Player modes
        WALK      = 0,
        SURF      = 1,
        BIKE      = 2,
        ACRO_BIKE = 3,
        SIT       = 4,
        DIVE      = 5,
        // NPC modes
        NO_MOVEMENT = 0,
    };
    enum wildPkmnType {
        GRASS,       // 5 tiers
        HIGH_GRASS,  // 5 tiers
        WATER,       // 5 tiers
        FISHING_ROD, // 5 tiers
        HEADBUTT,    // 3 tiers
        ROCK_SMASH   // 2 tiers
    };
    enum mapWeather {
        NOTHING        = 0, // Inside
        SUNNY          = 1,
        REGULAR        = 2,
        RAINY          = 3,
        SNOW           = 4,
        THUNDERSTORM   = 5,
        MIST           = 6,
        BLIZZARD       = 7,
        SANDSTORM      = 8,
        FOG            = 9,
        DENSE_MIST     = 0xa,
        CLOUDY         = 0xb, // Dark Forest clouds
        HEAVY_SUNLIGHT = 0xc,
        HEAVY_RAIN     = 0xd,
        UNDERWATER     = 0xe
    };
    enum mapType { OUTSIDE = 0, CAVE = 1, INSIDE = 2, DARK = 4, FLASHABLE = 8 };
    enum warpType { NO_SPECIAL, CAVE_ENTRY, DOOR, TELEPORT, EMERGE_WATER, LAST_VISITED };

    enum scriptType { WARP_SCRIPT = 0, SCRIPT = 1, TRAINER_SCRIPT = 2 };
    enum invocationType { STEP_ONTO = 0, INTERACT = 1, WARP_TILE = 2 };

    typedef std::pair<u8, position> warpPos;

#define MAX_MAP_LOCATIONS 62
    struct bankInfo {
        u8  m_bank;
        u16 m_locationId;
        u8  m_battleBg;
        u8  m_mapType; // mapType << 4 | mapWeather

        struct mapInfo {
            u16 m_upperLeftX;
            u16 m_upperLeftY;
            u16 m_lowerRightX;
            u16 m_lowerRightY;
            u16 m_locationId;
            u8  m_battleBg;
            u8  m_mapType; // mapType << 4 | mapWeather
        } m_data[ MAX_MAP_LOCATIONS ];
    };

    extern bankInfo CURRENT_BANK;
    void loadNewBank( u8 p_bank );
}
