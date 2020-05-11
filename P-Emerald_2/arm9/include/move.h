/*
Pokémon neo
------------------------------

file        : pokemon.h
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

#include <cstring>
#include <string>
#include <nds/ndstypes.h>
#include "defines.h"
#include "battleDefines.h"
#include "type.h"

namespace MOVE {
    struct moveData {
        type        m_type   = UNKNOWN; // ???
        contestType m_contestType = NO_CONTEST_TYPE; // Clever, Smart, ...
        u8       m_basePower = 0;
        u8       m_pp        = 1;

        moveHitTypes        m_category  = (moveHitTypes) 0;
        moveHitTypes        m_defensiveCategory = (moveHitTypes) 0; // category used for defending pkmn
        u8       m_accuracy  = 0; // 255: always hit
        s8       m_priority  = 0;

        BATTLE::sideCondition   m_sideCondition = BATTLE::NO_SIDE_CONDITION; // side introduced by the move (reflect, etc)

        BATTLE::weather         m_weather   = BATTLE::NO_WEATHER; // weather introduced by the move
        BATTLE::pseudoWeather   m_pseudoWeather = BATTLE::NO_PSEUDO_WEATHER; // pseudo weather introduced by the move
        BATTLE::terrain         m_terrain   = BATTLE::NO_TERRAIN; // terrain introduced by the move
        u8                      m_status = 0;

        BATTLE::slotCondition   m_slotCondition = (BATTLE::slotCondition) 0; // stuff introduced on the slot (wish, etc)
        u8                      m_fixedDamage = 0;
        target                  m_target    = (target) 0;
        target                  m_pressureTarget = (target) 0; // restrictions are computed based on different target than resulting effect

        u8       m_heal = 0; // as m_heal / 240
        u8       m_recoil = 0; // as dealt damage * m_recoil / 240
        u8       m_drain = 0; // as dealt damage * m_recoil / 240
        u8       m_multiHit = 0; // as ( min << 8 ) | max

        u8       m_critRatio = 1;
        u8       m_secondaryChance = 0; // chance that the secondary effect triggers
        u8       m_secondaryStatus = 0;
        u8       m_unused = 0;

        BATTLE::volatileStatus m_volatileStatus = (BATTLE::volatileStatus) 0; // confusion, etc
        BATTLE::volatileStatus m_secondaryVolatileStatus = (BATTLE::volatileStatus) 0; // confusion, etc

        BATTLE::boosts   m_boosts = { 0 }; // Status ``boosts'' for the target
        BATTLE::boosts   m_selfBoosts = { 0 }; // Status ``boosts'' for the user (if target != user)
        BATTLE::boosts   m_secondaryBoosts = { 0 }; // Stat ``boosts'' for the target
        BATTLE::boosts   m_secondarySelfBoosts = { 0 }; // Stat ``boosts'' for the user (if target != user)

        moveFlags m_flags     = (moveFlags) 0;
    };

    bool getMoveName( const u16 p_moveId, const u8 p_language, char* p_out );
    std::string getMoveName( const u16 p_moveId, const u8 p_language );

    bool getMoveData( const u16 p_moveId, moveData* p_out );
    moveData getMoveData( const u16 p_moveId );

    /*
     * @brief: Returns the text (id) displayed in OW
     */
    u16  text( const u16 p_moveId, u8 p_param );
    bool isFieldMove( const u16 p_moveId );
    bool possible( const u16 p_moveId, u8 p_param );
    void use( const u16 p_moveId, u8 p_param );
}
