/*
Pokémon neo
------------------------------

file        : battleSide.cpp
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

#include "battleSide.h"
#include "battle.h"
#include "battleField.h"
#include "battleSlot.h"
#include "battleUI.h"
#include "defines.h"

namespace BATTLE {
    void side::age( battleUI* p_ui ) {
        for( u8 i = 0; i < 2; ++i ) { _slots[ i ].age( p_ui ); }

        for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
            if( _sideConditionCounter[ i ] && _sideConditionCounter[ i ] < 250 ) {
                if( !--_sideConditionCounter[ i ] ) {
                    _sideConditionCounter[ i ] = 1;
                    removeSideCondition( p_ui, sideCondition( 1 << i ) );
                }
            }
        }
    }
} // namespace BATTLE
