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
#include "script.h"
#include "type.h"

class move {
  public:
    enum moveAffectsTypes : u8 {
        SELECTED              = 0,
        DEPENDS_ON_ATTACK     = 1,
        OWN_FIELD             = 2,
        RANDOM                = 4,
        BOTH_FOES             = 8,
        USER                  = 16,
        BOTH_FOES_AND_PARTNER = 32,
        OPPONENTS_FIELD       = 64
    };

    enum moveFlags : u8 {
        MAKES_CONTACT = 1,
        PROTECT       = 2,
        MAGIC_COAT    = 4,
        SNATCH        = 8,
        MIRROR_MOVE   = 16,
        KINGS_ROCK    = 32,
        SOUNDPROOF    = 64,
        WHILE_ASLEEP  = 128
    };

    enum moveHitTypes : u8 { PHYS = 0, SPEC = 1, STAT = 2 };

    bool m_isFieldAttack;

    std::string          m_moveName;
    BATTLE::battleScript m_moveEffect;
    char                 m_moveBasePower;
    type                 m_moveType;
    char                 m_moveAccuracy;
    u8                   m_movePP;
    char                 m_moveEffectAccuracy;
    moveAffectsTypes     m_moveAffectsWhom;
    char                 m_movePriority;
    moveFlags            m_moveFlags;
    moveHitTypes         m_moveHitType;

    char m_description[ 300 ] = {0};

    // Constructrs

    move( ) {
    }

    move( const std::string p_moveName, BATTLE::battleScript p_moveEffect, char p_moveBasePower,
          type p_moveType, char p_moveAccuracy, char p_movePP, char p_moveEffectAccuracy,
          moveAffectsTypes p_moveAffectsWhom, char p_movePriority, moveFlags p_moveFlags,
          moveHitTypes p_moveHitType,
          const char*  p_description = "Keine genaueren Informationen verfügbar." )
        : m_isFieldAttack( false ), m_moveName( p_moveName ), m_moveEffect( p_moveEffect ),
          m_moveBasePower( p_moveBasePower ), m_moveType( p_moveType ),
          m_moveAccuracy( p_moveAccuracy ), m_movePP( p_movePP ),
          m_moveEffectAccuracy( p_moveEffectAccuracy ), m_moveAffectsWhom( p_moveAffectsWhom ),
          m_movePriority( p_movePriority ), m_moveFlags( p_moveFlags ),
          m_moveHitType( p_moveHitType ) {
        strcpy( m_description, p_description );
    }

    ~move( ) {
    }

    virtual bool possible( ) {
        return false;
    }

    virtual void use( ) {
    }

    virtual const char* text( ) {
        return "N/A";
    }
    virtual const char* description( ) {
        return m_description;
    }
};

extern move* AttackList[ MAX_ATTACK ];

