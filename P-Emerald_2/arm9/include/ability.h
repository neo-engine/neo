/*
    Pok�mon Emerald 2 Version
    ------------------------------

    file        : ability.h
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

#include <string>

class ability {
public:
    enum abilityType {
        ATTACK = 1,
        BEFORE_BATTLE = 2,
        AFTER_BATTLE = 4,
        GRASS = 8,
        BEFORE_ATTACK = 16,
        AFTER_ATTACK = 32
    };
    const std::string   m_abilityName;
    std::string         m_flavourText;
    abilityType         m_type;

    ///Constructors
    ability( ) { }

    ability( const std::string p_abilityName )
        : m_abilityName( p_abilityName ) { }

    ability( const std::string p_abilityName, int p_type )
        : m_abilityName( p_abilityName ), m_type( (abilityType)p_type ) { }

    ability( const std::string p_abilityName, const std::string p_flavourText, int p_type )
        : m_abilityName( p_abilityName ), m_flavourText( p_flavourText ), m_type( (abilityType)p_type ) { }

    void run( ... );
};