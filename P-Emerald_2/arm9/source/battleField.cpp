/*
Pokémon neo
------------------------------

file        : battleField.cpp
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
    field::field( weather p_initialWeather, pseudoWeather p_initialPseudoWeather,
                  terrain p_initialTerrain ) {
        // TODO
    }

    void field::init( battleUI p_ui ) {
        // TODO
    }

    void field::age( battleUI p_ui ) {
        // TODO
    }

    bool    field::setWeather( battleUI p_ui, weather p_newWeather ) {
        // TODO
        return false;
    }

    bool    field::removeWeather( battleUI p_ui ) {
        // TODO
        return false;
    }

    weather field::getWeather( ) {
        // TODO
        return weather( 0 );
    }

    bool          field::setPseudoWeather( battleUI p_ui, pseudoWeather p_newPseudoWeather ) {
        // TODO
        return false;
    }

    bool          field::removePseudoWeather( battleUI p_ui ) {
        // TODO
        return false;
    }

    pseudoWeather field::getPseudoWeather( ) {
        // TODO
        return pseudoWeather( 0 );
    }

    bool    field::setTerrain( battleUI p_ui, terrain p_newTerrainr ) {
        // TODO
        return false;
    }

    bool    field::removeTerrain( battleUI p_ui ) {
        // TODO
        return false;
    }

    terrain field::getTerrain( ) {
        // TODO
        return terrain( 0 );
    }

    bool field::addSideCondition( battleUI p_ui, u8 p_side, sideCondition p_sideCondition,
                                  u8 p_duration ) {
        // TODO
        return false;
    }

    bool field::removeSideCondition( battleUI p_ui, u8 p_side, sideCondition p_sideCondition ) {
        // TODO
        return false;
    }

    std::vector<battleMove>
    field::computeSortedBattleMoves( const std::vector<battleMoveSelection>& p_selectedMoves ) {
        // TODO
        return { };
    }

    void field::executeBattleMove( battleUI p_ui, battleMove p_move,
                                   std::vector<battleMove> p_targetsMoves,
                                   std::vector<battleMove> p_tergetedMoves ) {
        // TODO
    }
}
