/*
Pokémon Emerald 2 Version
------------------------------

file        : battlePokemon.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#pragma once

#include <nds/ndstypes.h>
#include "type.h"

struct pokemon;
namespace BATTLE {

    enum acStatus {
        OK = 0,
        STS = 1,
        KO = 2,
        NA = 3,
        SELECTED = 4
    };
    enum battleStatus {
        NO_BATTLE_STATUS = 0,
        DIVE,
        FLY,
        DEFAULTED, //Pokemon used eg Hyperbeam last Turn
        PROTECTED
        //Todo ...
    };

    struct battlePokemon {
        pokemon*            m_pokemon;
        s8                  m_acStatChanges[ 10 ];
        type                m_types[ 3 ];
        acStatus            m_acStatus;
        ailment             m_ailments;
        u8                  m_ailmentCount;
        battleStatus        m_battleStatus;
        u8                  m_toxicCount;
    };
}
