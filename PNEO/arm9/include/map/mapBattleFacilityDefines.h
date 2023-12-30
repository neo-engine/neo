/*
Pokémon neo
------------------------------

file        : mapBattleFacilityDefines.h
author      : Philip Wellnitz
description : General map stuff.

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
#include "battle/battleDefines.h"
#include "defines.h"
#include "gen/pokemonNames.h"
#include "pokemon.h"

namespace MAP {
    struct ruleSet {
        u8 m_id;
        u8 m_level;
        u8 m_numPkmn;
        u8 m_hasSpecialBattles; // if set to 1, use fixed battles for 21st and 49th battle that
                                // award symbols
        u8                 m_numBattles;
        BATTLE::battleMode m_battleMode;
    };

    constexpr u8         NUM_RULESETS = 5;
    extern const ruleSet FACILITY_RULE_SETS[ NUM_RULESETS ];

    constexpr int IV_MAX_STREAK                  = 8;
    constexpr u8  IV_FOR_STREAK[ IV_MAX_STREAK ] = { 3, 6, 9, 12, 15, 18, 21, 31 };

    constexpr u16 MAX_BF_PKMN = 1073;
    constexpr u16 MAX_BF_PKMN_FOR_STREAK[ IV_MAX_STREAK ]
        = { 50, 125, 200, 300, 400, 600, 800, MAX_BF_PKMN };
    constexpr u16 MIN_BF_PKMN_FOR_STREAK[ IV_MAX_STREAK ] = { 1, 26, 51, 126, 201, 251, 301, 401 };

    constexpr u16 MAX_BF_TRAINER = 300;
    constexpr u16 MAX_BF_TRAINER_FOR_STREAK[ IV_MAX_STREAK ]
        = { 100, 120, 140, 160, 180, 200, 220, 300 };
    constexpr u16 MIN_BF_TRAINER_FOR_STREAK[ IV_MAX_STREAK ]
        = { 1, 81, 101, 121, 141, 161, 181, 201 };

    constexpr u16 bfTrainerForStreak( u16 p_streak ) {
        return MIN_BF_TRAINER_FOR_STREAK[ p_streak ]
               + ( rand( )
                   % ( MAX_BF_TRAINER_FOR_STREAK[ p_streak ] - MIN_BF_TRAINER_FOR_STREAK[ p_streak ]
                       + 1 ) );
    }

    constexpr u16 bfPkmnForStreak( u16 p_streak ) {
        return MIN_BF_PKMN_FOR_STREAK[ p_streak ]
               + ( rand( )
                   % ( MAX_BF_PKMN_FOR_STREAK[ p_streak ] - MIN_BF_PKMN_FOR_STREAK[ p_streak ]
                       + 1 ) );
    }

} // namespace MAP
