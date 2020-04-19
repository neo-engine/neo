/*
Pokémon neo
------------------------------

file        : mapWarps.cpp
author      : Philip Wellnitz
description : Map warps.

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

#include <cstring>
#include "fs.h"
#include "mapDrawer.h"

namespace MAP {
    bankInfo CURRENT_BANK;

    void loadNewBank( u8 p_bank ) {
        memset( &CURRENT_BANK, 0, sizeof( bankInfo ) );
        CURRENT_BANK.m_bank = p_bank;
        if( !FS::readBankData( p_bank, CURRENT_BANK ) ) CURRENT_BANK.m_locationId = 1234;
    }
    /*
        // locationId, battlebg
        std::map<u8, std::pair<u16, u16>> mapInfo = {
            {MAP_HOENN, {2005, 0}}, // Hoenn

            {MAP_METEOR_FALLS, {298, 0}}, // Meteor Falls
            {MAP_DESERT_RUINS, {317, 0}}, // Desert Ruin
            {MAP_ISLAND_CAVE, {316, 0}},  // Island Caves
            {MAP_ANCIENT_TOMB, {318, 0}}, // Ancient Tomb

         SAVE::SAV->getActiveFile( ).m_currentMap   {MAP_KANTO, {2003, 0}}, // Kanto

            {MAP_TOHJO_FALLS, {223, 0}},        // Tohjo Falls
            {MAP_KANTO_VICTORY_ROAD, {221, 0}}, // Victory Road
            {MAP_VIRIDIAN_FOREST, {224, 8}},    // Viridian Forest
            {MAP_SEAFOAM_ISLANDS, {203, 16}},   // Seaform Islands
            {MAP_MT_MOON, {198, 0}},            // Mt Moon
            {MAP_DIGLETTS_CAVE, {197, 0}},      // Diglett's Cave
            {MAP_ROCK_TUNNEL, {200, 0}},        // Rock Tunnel
            {MAP_CERULEAN_CAVE, {199, 10}},     // Cerulean Cave

            {MAP_JOHTO, {2004, 0}}, // Johto

            {MAP_DARK_CAVE, {220, 10}},     // Dark Cave
            {MAP_SPROUT_TOWER, {204, 18}},  // Sprout Tower
            {MAP_RUINS_OF_ALPH, {209, 0}},  // Ruins Of Alph
            {MAP_UNION_CAVE, {210, 0}},     // Union Cave
            {MAP_SLOWPOKE_WELL, {211, 30}}, // Slowpoke Well
            {MAP_ILEX_FOREST, {214, 8}},    // Ilex Forest
            {MAP_NATIONAL_PARK, {207, 14}}, // National Park
            {MAP_BURNED_TOWER, {206, 18}},  // Burned Tower
            {MAP_BELL_TOWER, {205, 18}},    // Bell Tower
            {MAP_WHIRL_ISLANDS, {218, 30}}, // Whirl Islands
            {MAP_MT_MOON, {216, 0}},        // Mt Mortar
            {MAP_ICE_PATH, {217, 16}},      // Ice Path
            {MAP_DRAGONS_DEN, {222, 30}},   // Dragon's Den
        };

        std::map<u8, std::pair<mapType, mapWeather>> mapTypes = {
            {MAP_HOENN, {OUTSIDE, REGULAR}},

            {MAP_METEOR_FALLS, {CAVE, NOTHING}}, // Meteor Falls
            {MAP_DESERT_RUINS, {CAVE, NOTHING}},
            {MAP_ISLAND_CAVE, {CAVE, NOTHING}},
            {MAP_ANCIENT_TOMB, {CAVE, NOTHING}},

            {MAP_KANTO, {OUTSIDE, REGULAR}},
            {MAP_TOHJO_FALLS, {CAVE, NOTHING}},                                      // Tohjo Falls
            {MAP_KANTO_VICTORY_ROAD, {mapType( CAVE | DARK | FLASHABLE ), NOTHING}}, // Victory Road
            {MAP_VIRIDIAN_FOREST, {OUTSIDE, CLOUDY}},                                // Viridian
       Forest
            {MAP_SEAFOAM_ISLANDS, {CAVE, NOTHING}},                                  // Seaform
       Islands
            {MAP_MT_MOON, {mapType( CAVE | DARK | FLASHABLE ), NOTHING}},            // Mt Moon
            {MAP_DIGLETTS_CAVE, {mapType( CAVE | DARK | FLASHABLE ), NOTHING}},      // Diglett's
       Cave
            {MAP_ROCK_TUNNEL, {mapType( CAVE | DARK | FLASHABLE ), NOTHING}},        // Rock Tunnel
            {MAP_CERULEAN_CAVE, {mapType( CAVE | DARK ), NOTHING}},                  // Cerulean
       Cave

            {MAP_JOHTO, {OUTSIDE, REGULAR}},
            {MAP_DARK_CAVE, {mapType( CAVE | DARK ), NOTHING}},              // Dark Cave
            {MAP_SPROUT_TOWER, {CAVE, NOTHING}},                             // Sprout Tower
            {MAP_RUINS_OF_ALPH, {CAVE, NOTHING}},                            // Ruins Of Alph
            {MAP_UNION_CAVE, {mapType( CAVE | DARK | FLASHABLE ), NOTHING}}, // Union Cave
            {MAP_SLOWPOKE_WELL, {CAVE, NOTHING}},                            // Slowpoke Well
            {MAP_ILEX_FOREST, {CAVE, CLOUDY}},                               // Ilex Forest
            {MAP_NATIONAL_PARK, {OUTSIDE, SUNNY}},                           // National Park
            {MAP_BURNED_TOWER, {CAVE, NOTHING}},                             // Burned Tower
            {MAP_BELL_TOWER, {CAVE, NOTHING}},                               // Bell Tower
            {MAP_WHIRL_ISLANDS, {CAVE, NOTHING}},                            // Whirl Islands
            {MAP_MT_MOON, {CAVE, NOTHING}},                                  // Mt Mortar
            {MAP_ICE_PATH, {CAVE, NOTHING}},                                 // Ice Path
            {MAP_DRAGONS_DEN, {CAVE, NOTHING}},                              // Dragon's Den
        }; */
} // namespace MAP
