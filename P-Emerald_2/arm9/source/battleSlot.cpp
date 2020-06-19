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

#include "battle.h"
#include "battleUI.h"
#include "battleSlot.h"
#include "battleSide.h"
#include "battleField.h"
#include "defines.h"

namespace BATTLE {
    void slot::age( battleUI* p_ui ) {
        // TODO
    }

    boosts slot::addBoosts( boosts p_boosts, bool p_allowAbilities ) {
        if( getPkmn( ) == nullptr ) { return boosts( ); }

        if( p_allowAbilities && getPkmn( )->getAbility( ) == A_CONTRARY ) {
            p_boosts.invert( );
        }

        return _boosts.addBoosts( p_boosts );
    }

    bool slot::resetBoosts( battleUI* p_ui ) {
        // TODO
        return false;
    }

    u16 slot::computeRecoil( u16 p_damage ) {
        // TODO
        return 0;
    }

    bool slot::absorbesMove( battleMove p_move, u16 p_baseDamage ) {
        // TODO
        return false;
    }
} // namespace BATTLE
