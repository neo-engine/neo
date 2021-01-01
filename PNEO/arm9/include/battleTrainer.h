/*
Pokémon neo
------------------------------

file        : battleTrainer.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2021
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
#include <string>
#include <nds/ndstypes.h>
#include "defines.h"
#include "pokemon.h"

namespace BATTLE {
    struct trainerStrings {
        char m_name[ 16 ];
        char m_message1[ 200 ];
        char m_message2[ 200 ];
        char m_message3[ 200 ];
        char m_message4[ 200 ];
    };

    struct trainerData {
        u8             m_trainerClass;
        u8             m_AILevel;
        u16            m_trainerBG;
        u16            m_battleBG;
        u8             m_battlePlat1;
        u8             m_battlePlat2;
        u8             m_numPokemon;
        u16            m_items[ 5 ];
        u32            m_moneyEarned;
        trainerPokemon m_pokemon[ 6 ];
    };

    struct battleTrainer {
        trainerStrings m_strings;
        trainerData    m_data;

        constexpr u8 getClass( ) const {
            return m_data.m_trainerClass;
        }
    };

    std::string getTrainerClassName( u16 p_trainerClass );
    std::string getTrainerClassName( u16 p_trainerClass, u8 p_language );
    bool        getTrainerClassName( u16 p_trainerClass, u8 p_language, char* p_out );

    battleTrainer getBattleTrainer( u16 p_battleTrainerId );
    battleTrainer getBattleTrainer( u16 p_battleTrainerId, u8 p_language );
    bool          getBattleTrainer( u16 p_battleTrainerId, u8 p_language, battleTrainer* p_out );
} // namespace BATTLE
