/*
Pokémon neo
------------------------------

file        : ability.h
author      : Philip Wellnitz
description :

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

#include <string>
#include <nds.h>

#include "abilityNames.h"

bool        getAbilityName( u16 p_abilityId, u8 p_language, char* p_out );
std::string getAbilityName( u16 p_abilityId, u8 p_language );
std::string getAbilityName( u16 p_abilityId );

bool        getAbilityDescr( u16 p_abilityId, u8 p_language, char* p_out );
std::string getAbilityDescr( u16 p_abilityId, u8 p_language );
std::string getAbilityDescr( u16 p_abilityId );

constexpr bool allowsReplace( u16 p_ability ) {
    switch( p_ability ) {
        case A_COMATOSE:
        case A_MULTITYPE:
        case A_DISGUISE:
        case A_GULP_MISSILE:
        case A_ICE_FACE:
        case A_ILLUSION:
        case A_POWER_CONSTRUCT:
        case A_RKS_SYSTEM:
        case A_SCHOOLING:
        case A_SHIELDS_DOWN:
        case A_STANCE_CHANGE:
        case A_WONDER_GUARD:
        case A_ZEN_MODE:
            return false;
        default:
            return true;
    }
}

constexpr bool allowsCopy( u16 p_ability ) {
    switch( p_ability ) {
        case A_COMATOSE:
        case A_MULTITYPE:
        case A_DISGUISE:
        case A_FLOWER_GIFT:
        case A_FORECAST:
        case A_GULP_MISSILE:
        case A_ICE_FACE:
        case A_IMPOSTER:
        case A_ILLUSION:
        case A_POWER_CONSTRUCT:
        case A_POWER_OF_ALCHEMY:
        case A_RECEIVER:
        case A_RKS_SYSTEM:
        case A_SCHOOLING:
        case A_SHIELDS_DOWN:
        case A_STANCE_CHANGE:
        case A_TRACE:
        case A_WONDER_GUARD:
        case A_ZEN_MODE:
            return false;
        default:
            return true;
    }
}
