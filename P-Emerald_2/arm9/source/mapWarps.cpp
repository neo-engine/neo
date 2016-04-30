/*
Pokémon Emerald 2 Version
------------------------------

file        : mapWarps.cpp
author      : Philip Wellnitz
description : Map warps.

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

#include "mapWarps.h"
#include "mapDrawer.h"

namespace MAP {
    bool operator==( position p_l, position p_r ) {
        return p_l.m_posX == p_r.m_posX && p_l.m_posY == p_r.m_posY && p_l.m_posZ == p_r.m_posZ;
    }
    bool operator<( position p_l, position p_r ) {
        return p_l.m_posX < p_r.m_posX || ( p_l.m_posX == p_r.m_posX && p_l.m_posY < p_r.m_posY )
            || ( p_l.m_posX == p_r.m_posX && p_l.m_posY == p_r.m_posY && p_l.m_posZ < p_r.m_posZ );
    }

    std::map<u8, std::pair<u16, u16>> mapInfo = {
        { MAP_HOENN, { 2005, 0 } }, //Hoenn

        { MAP_METEOR_FALLS, { 298, 0 } }, //Meteor Falls
        { MAP_DESERT_RUINS, { 317, 0 } }, //Desert Ruin
        { MAP_ISLAND_CAVE, { 316, 0 } }, //Island Caves
        { MAP_ANCIENT_TOMB, { 318, 0 } }, //Ancient Tomb


        { MAP_KANTO, { 2003, 0 } }, //Kanto

        { MAP_TOHJO_FALLS, { 223, 0 } }, //Tohjo Falls
        { MAP_KANTO_VICTORY_ROAD, { 221, 0 } }, //Victory Road
        { MAP_VIRIDIAN_FOREST, { 224, 8 } }, //Viridian Forest
        { MAP_SEAFOAM_ISLANDS, { 203, 16 } }, //Seaform Islands
        { MAP_MT_MOON, { 198, 0 } }, //Mt Moon
        { MAP_DIGLETTS_CAVE, { 197, 0 } }, //Diglett's Cave
        { MAP_ROCK_TUNNEL, { 200, 0 } }, //Rock Tunnel
        { MAP_CERULEAN_CAVE, { 199, 10 } }, //Cerulean Cave


        { MAP_JOHTO, { 2004, 0 } }, //Johto

        { MAP_DARK_CAVE, { 220, 10 } }, //Dark Cave
        { MAP_SPROUT_TOWER, { 204, 18 } }, //Sprout Tower
        { MAP_RUINS_OF_ALPH, { 209, 0 } }, //Ruins Of Alph
        { MAP_UNION_CAVE, { 210, 0 } }, //Union Cave
        { MAP_SLOWPOKE_WELL, { 211, 30 } }, //Slowpoke Well
        { MAP_ILEX_FOREST, { 214, 8 } }, //Ilex Forest
        { MAP_NATIONAL_PARK, { 207, 14 } }, //National Park
        { MAP_BURNED_TOWER, { 206, 18 } }, //Burned Tower
        { MAP_BELL_TOWER, { 205, 18 } }, //Bell Tower
        { MAP_WHIRL_ISLANDS, { 218, 30 } }, //Whirl Islands
        { MAP_MT_MOON, { 216, 0 } }, //Mt Mortar
        { MAP_ICE_PATH, { 217, 16 } }, //Ice Path
        { MAP_DRAGONS_DEN, { 222, 30 } }, //Dragon's Den
    };

    std::map<u8, std::pair<mapType, mapWeather>> mapTypes = {
        { MAP_HOENN,{ OUTSIDE, REGULAR } },

        { MAP_METEOR_FALLS,{ CAVE, NOTHING } }, //Meteor Falls
        { MAP_DESERT_RUINS,{ CAVE, NOTHING } },
        { MAP_ISLAND_CAVE,{ CAVE, NOTHING } },
        { MAP_ANCIENT_TOMB,{ CAVE, NOTHING } },

        { MAP_KANTO,{ OUTSIDE, REGULAR } },
        { MAP_TOHJO_FALLS,{ CAVE, NOTHING } }, //Tohjo Falls
        { MAP_KANTO_VICTORY_ROAD,{ mapType( CAVE | DARK | FLASHABLE ), NOTHING } }, //Victory Road
        { MAP_VIRIDIAN_FOREST,{ OUTSIDE, CLOUDY } }, //Viridian Forest
        { MAP_SEAFOAM_ISLANDS,{ CAVE, NOTHING } }, //Seaform Islands
        { MAP_MT_MOON,{ mapType( CAVE | DARK | FLASHABLE ), NOTHING } }, //Mt Moon
        { MAP_DIGLETTS_CAVE,{ mapType( CAVE | DARK | FLASHABLE ), NOTHING } }, //Diglett's Cave
        { MAP_ROCK_TUNNEL,{ mapType( CAVE | DARK | FLASHABLE ), NOTHING } }, //Rock Tunnel
        { MAP_CERULEAN_CAVE,{ mapType( CAVE | DARK ), NOTHING } }, //Cerulean Cave

        { MAP_JOHTO,{ OUTSIDE, REGULAR } },
        { MAP_DARK_CAVE,{ mapType( CAVE | DARK ), NOTHING } }, //Dark Cave
        { MAP_SPROUT_TOWER,{ CAVE, NOTHING } }, //Sprout Tower
        { MAP_RUINS_OF_ALPH,{ CAVE, NOTHING } }, //Ruins Of Alph
        { MAP_UNION_CAVE,{ mapType( CAVE | DARK | FLASHABLE ), NOTHING } }, //Union Cave
        { MAP_SLOWPOKE_WELL,{ CAVE, NOTHING } }, //Slowpoke Well
        { MAP_ILEX_FOREST,{ CAVE, CLOUDY } }, //Ilex Forest
        { MAP_NATIONAL_PARK,{ OUTSIDE, SUNNY } }, //National Park
        { MAP_BURNED_TOWER,{ CAVE, NOTHING } }, //Burned Tower
        { MAP_BELL_TOWER,{ CAVE, NOTHING } }, //Bell Tower
        { MAP_WHIRL_ISLANDS,{ CAVE, NOTHING } }, //Whirl Islands
        { MAP_MT_MOON,{ CAVE, NOTHING } }, //Mt Mortar
        { MAP_ICE_PATH,{ CAVE, NOTHING } }, //Ice Path
        { MAP_DRAGONS_DEN,{ CAVE, NOTHING } }, //Dragon's Den
    };

#define MAP_POS( p_mapX, p_mapY, p_x, p_y, p_z ) { ( p_mapY ) * 32 + ( p_x ), ( p_mapX ) * 32 + ( p_y ), ( p_z ) }
#define POS( p_x, p_y, p_z ) { ( p_x ), ( p_y ), ( p_z ) }
#define WARP( p_map1, p_pos1, p_map2, p_pos2 ) { { p_map1, p_pos1 }, { p_map2, p_pos2 } },\
                                { { p_map2, p_pos2 }, { p_map1, p_pos1 } }

    std::map<std::pair<u8, position>, std::pair<u8, position>> warpList = {
        //Meteor Falls
        WARP( MAP_HOENN, POS( 104, 119, 5 ), MAP_METEOR_FALLS, MAP_POS( 1, 3, 0x1c, 0x1d, 4 ) ),
        WARP( MAP_HOENN, POS( 83, 133, 3 ), MAP_METEOR_FALLS, MAP_POS( 1, 3, 0x07, 0x32, 3 ) ),

        WARP( MAP_METEOR_FALLS, MAP_POS( 3, 1, 0x05, 0x06, 4 ), MAP_METEOR_FALLS, MAP_POS( 1, 1, 0x04, 0x0e, 3 ) ),
        WARP( MAP_METEOR_FALLS, MAP_POS( 3, 1, 0x07, 0x0b, 5 ), MAP_METEOR_FALLS, MAP_POS( 1, 1, 0x08, 0x13, 3 ) ),
        WARP( MAP_METEOR_FALLS, MAP_POS( 3, 1, 0x12, 0x0e, 4 ), MAP_METEOR_FALLS, MAP_POS( 1, 1, 0x15, 0x17, 3 ) ),

        WARP( MAP_METEOR_FALLS, MAP_POS( 3, 1, 0x03, 0x17, 5 ), MAP_METEOR_FALLS, MAP_POS( 1, 3, 0x06, 0x0f, 3 ) ),
        WARP( MAP_METEOR_FALLS, MAP_POS( 3, 1, 0x14, 0x1e, 3 ), MAP_METEOR_FALLS, MAP_POS( 1, 3, 0x1b, 0x27, 3 ) ),

        WARP( MAP_METEOR_FALLS, MAP_POS( 3, 1, 0x11, 0x03, 3 ), MAP_METEOR_FALLS, MAP_POS( 2, 2, 0x05, 0x0f, 3 ) ),
        WARP( MAP_METEOR_FALLS, MAP_POS( 1, 1, 0x0a, 0x1d, 3 ), MAP_METEOR_FALLS, MAP_POS( 1, 3, 0x0b, 0x0e, 3 ) )
    };

#undef MAP_POS
#undef POS
#undef WARP
}