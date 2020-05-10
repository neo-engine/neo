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


#include "defines.h"
#include "battle.h"

namespace BATTLE {
    slot::slot( pokemon* p_initialPokemon ) {
        // TODO
    }

    void slot::age( battleUI p_ui ) {
        // TODO
    }

    pokemon* slot::recallPokemon( battleUI p_ui, bool p_keepVolatileStatus ) {
        // TODO
        return nullptr;
    }

    bool slot::sendPokemon( battleUI p_ui, pokemon* p_pokemon ) {
        // TODO
        return false;
    }

    bool slot::damagePokemon( battleUI p_ui, u16 p_damage ) {
        // TODO
        return false;
    }

    bool slot::healPokemon( battleUI p_ui, u16 p_heal ) {
        // TODO
        return false;
    }

    bool slot::faintPokemon( battleUI p_ui ) {
        // TODO
        return false;
    }

    bool slot::addBoosts( battleUI p_ui, boosts p_boosts ) {
        // TODO
        return false;
    }

    bool slot::resetBoosts( battleUI p_ui ) {
        // TODO
        return false;
    }

    boosts slot::getBoosts( ) {
        // TODO
        return boosts( );
    }

    bool slot::addVolatileStatus( battleUI p_ui, volatileStatus p_volatileStatus ) {
        // TODO
        return false;
    }

    volatileStatus slot::getVolatileStatus( ) {
        // TODO
        return volatileStatus( );
    }

    bool slot::addSlotCondition( battleUI p_ui, slotCondition p_slotCondition ) {
        // TODO
        return false;
    }

    slotCondition slot::getSlotCondition( ) {
        // TODO
        return slotCondition( );
    }

    u16 slot::getStat( u8 p_stat ) {
        // TODO
        return 0;
    }

    bool slot::canSelectMove( ) {
        // TODO
        return false;
    }

    bool slot::canSelectMove( u8 p_moveIdx ) {
        // TODO
        return false;
    }

    bool slot::useMove( battleUI p_ui, u16 p_moveId ) {
        // TODO
        return false;
    }

    void slot::hitByMove( battleUI p_ui, u16 p_moveId ) {
        // TODO
    }

    bool slot::canUseItem( ) {
        // TODO
        return false;
    }

    bool slot::canSwitchOut( ) {
        // TODO
        return false;
    }

    battleMove slot::computeBattleMove( battleMoveSelection p_usersSelection,
                                        std::vector<battleMoveSelection>& p_targetsSelecotions ) {
        // TODO
        return battleMove( );
    }

    u16 slot::computeDamageTaken( battleMove p_move, u16 p_baseDamage ) {
        // TODO
        return 0;
    }

    u16 slot::computeRecoil( u16 p_damage ) {
        // TODO
        return 0;
    }

    bool slot::absorbesMove( battleMove p_move, u16 p_baseDamage ) {
        // TODO
        return false;
    }
}
