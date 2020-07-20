/*
Pokémon neo
------------------------------

file        : battleSlot.cpp
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

#include "battleSlot.h"
#include "battle.h"
#include "battleField.h"
#include "battleSide.h"
#include "battleUI.h"
#include "defines.h"

namespace BATTLE {
    void slot::age( battleUI* p_ui ) {
        if( _pokemon == nullptr ) [[unlikely]] {
                return;
            }

        for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
            if( _slotConditionCounter[ i ] && _slotConditionCounter[ i ] < 250 ) {
                if( !--_slotConditionCounter[ i ] ) {
                    removeSlotCondition( p_ui, slotCondition( 1 << i ) );
                }
            }
        }
        _turnsInPlay++;
    }
} // namespace BATTLE
