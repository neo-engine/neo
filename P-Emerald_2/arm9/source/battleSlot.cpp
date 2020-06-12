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
#include "defines.h"

namespace BATTLE {
    void slot::age( battleUI* p_ui ) {
        // TODO
    }

    pokemon* slot::recallPokemon( battleUI* p_ui, bool p_keepVolatileStatus ) {
        // TODO
        return nullptr;
    }

    bool slot::sendPokemon( battleUI* p_ui, pokemon* p_pokemon ) {
        // TODO
        return false;
    }

    bool slot::damagePokemon( battleUI* p_ui, u16 p_damage ) {
        // TODO
        return false;
    }

    bool slot::healPokemon( battleUI* p_ui, u16 p_heal ) {
        // TODO
        return false;
    }

    bool slot::faintPokemon( battleUI* p_ui ) {
        // TODO
        return false;
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

    bool slot::addVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus ) {
        // TODO
        return false;
    }

    bool slot::addSlotCondition( battleUI* p_ui, slotCondition p_slotCondition ) {
        // TODO
        return false;
    }

    bool slot::useMove( battleUI* p_ui, u16 p_moveId ) {
        // TODO
        return false;
    }

    void slot::hitByMove( battleUI* p_ui, u16 p_moveId ) {
        // TODO
    }

    battleMove slot::computeBattleMove( battleMoveSelection               p_usersSelection,
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
} // namespace BATTLE
