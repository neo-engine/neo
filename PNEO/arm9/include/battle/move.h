/*
Pokémon neo
------------------------------

file        : pokemon.h
author      : Philip Wellnitz
description :

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

#include <string>
#include <nds/ndstypes.h>
#include "battle/battleDefines.h"
#include "gen/moveNames.h"

namespace BATTLE {

    /*
     * @brief: Returns the text (id) displayed in OW
     */
    u16 text( const u16 p_moveId, u8 p_param );

    constexpr bool isFieldMove( u16 p_moveId ) {
        switch( p_moveId ) {
        case M_CUT:
        case M_ROCK_SMASH:
        case M_FLY:
        case M_FLASH:
        case M_WHIRLPOOL:
        case M_SURF:
        case M_DIVE:
        case M_DEFOG:
        case M_STRENGTH:
        case M_ROCK_CLIMB:
        case M_WATERFALL:
        case M_TELEPORT:
        case M_HEADBUTT:
        case M_SWEET_SCENT:
        case M_DIG:
        case M_HIDDEN_POWER:
        case M_SECRET_POWER: return true;
        default: return false;
        }
    }

    bool possible( const u16 p_moveId, u8 p_param );
    void use( const u16 p_moveId, u8 p_param );
} // namespace BATTLE
