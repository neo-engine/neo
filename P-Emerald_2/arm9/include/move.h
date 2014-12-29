/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : move.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */

#pragma once

#include "type.h"
#include "script.h"
#include <string>
#include <nds/ndstypes.h>

#define MAXATTACK 560

class move {
public:
    enum ailment {
        NONE = 0,
        PARALYSIS,
        SLEEP,
        FREEZE,
        BURN,
        POISON,
        CONFUSION,
        INFATUATION,
        TRAP,
        NIGHTMARE,
        TORMENT,
        DISABLE,
        YAWN,
        HEAL_BLOCK,
        NO_TYPE_IMMUNITY,
        LEECH_SEED,
        EMBARGO,
        PERISH_SONG,
        INGRAIN
    };

    enum moveAffectsTypes {
        SELECTED = 0,
        DEPENDS_ON_ATTACK = 1,
        OWN_FIELD = 2,
        RANDOM = 4,
        BOTH_FOES = 8,
        USER = 16,
        BOTH_FOES_AND_PARTNER = 32,
        OPPONENTS_FIELD = 64
    };

    enum moveFlags {
        MAKES_CONTACT = 1,
        PROTECT = 2,
        MAGIC_COAT = 4,
        SNATCH = 8,
        MIRROR_MOVE = 16,
        KINGS_ROCK = 32,
        SOUNDPROOF = 64,
        WHILE_ASLEEP = 128
    };

    enum moveHitTypes {
        PHYS = 0,
        SPEC = 1,
        STAT = 2
    };

    bool            m_isFieldAttack;

    std::string     m_moveName;
    BATTLE::battleScript    m_moveEffect;
    char            m_moveBasePower;
    Type            m_moveType;
    char            m_moveAccuracy;
    u8              m_movePP;
    char            m_moveEffectAccuracy;
    moveAffectsTypes
        m_moveAffectsWhom;
    char            m_movePriority;
    moveFlags       m_moveFlags;
    moveHitTypes    m_moveHitType;

    //Constructrs

    move( ) { }

    move( const std::string p_moveName,
          char p_moveEffect,
          char p_moveBasePower,
          Type p_moveType,
          char p_moveAccuracy,
          char p_movePP,
          char p_moveEffectAccuracy,
          moveAffectsTypes p_moveAffectsWhom,
          char p_movePriority,
          moveFlags p_moveFlags,
          moveHitTypes p_moveHitType )
          : m_isFieldAttack( false ),
          m_moveName( p_moveName ),
          m_moveEffect( BATTLE::battleScript( { BATTLE::battleScript::command( L"Attackeneffekt.[A]" ) } ) ), // <-- TODO 
          m_moveBasePower( p_moveBasePower ),
          m_moveType( p_moveType ),
          m_moveAccuracy( p_moveAccuracy ),
          m_movePP( p_movePP ),
          m_moveEffectAccuracy( p_moveEffectAccuracy ),
          m_moveAffectsWhom( p_moveAffectsWhom ),
          m_movePriority( p_movePriority ),
          m_moveFlags( p_moveFlags ),
          m_moveHitType( p_moveHitType ) { }

    ~move( ) { }

    virtual bool possible( ) {
        return false;
    }

    virtual void use( ) { }

    virtual const char* text( ) {
        return 0;
    }
};

#define M_FOULPLAY 42
#define M_CHIP_AWAY 43
#define M_PSYSHOCK 44
#define M_PSYSTRIKE 45
#define M_SECRET_SWORD 46
#define M_STRUGGLE 47