/*
Pokémon neo
------------------------------

file        : battleSide.cpp
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
#include "defines.h"

namespace BATTLE {
    void side::age( battleUI p_ui ) {
        // TODO
    }

    bool side::addSideCondition( battleUI p_ui, sideCondition p_sideCondition, u8 p_duration ) {
        // TODO
        return false;
    }

    bool side::removeSideCondition( battleUI p_ui, sideCondition p_sideCondition ) {
        // TODO
        return false;
    }

    battleMove side::computeBattleMove( u8 p_slot, battleMoveSelection& p_usersSelection,
                                        std::vector<battleMoveSelection>& p_targetsSelecotions ) {
        // TODO
        return battleMove( );
    }

    std::vector<u16> side::computeDamageDealt( u8 p_slot, battleMove p_usersMove,
                                               std::vector<battleMove>& p_targetsMoves,
                                               std::vector<battleMove>& p_targetedMoves ) {
        // TODO
        return {};
    }

    u16 side::computeDamageTaken( u8 p_slot, battleMove p_move, u16 p_baseDamage ) {
        // TODO
        return 0;
    }

    u16 side::computeRecoil( u8 p_slot, u16 p_damage ) {
        // TODO
        return 0;
    }

    bool side::absorbesMove( u8 p_slot, battleMove p_move, u16 p_baseDamage ) {
        // TODO
        return false;
    }
} // namespace BATTLE
