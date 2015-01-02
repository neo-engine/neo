/*
Pokémon Emerald 2 Version
------------------------------

file        : hmMoves.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "move.h"

/*
*  Field moves get a derived class each
*/

#define C (char)
#define A (move::moveAffectsTypes)
#define F (move::moveFlags)
#define H (move::moveHitTypes)

//HM01
class cut
    : public move {
public:
    cut( )
        : move( std::string( "Zerschneider" ),
        C 0,
        C 60,
        NORMAL,
        C 100,
        C 15,
        C 0,
        SELECTED,
        C 1,
        F( MAKES_CONTACT | PROTECT | KINGS_ROCK ),
        PHYS ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;

    bool possible( ) override;

    const char* text( ) override {
        return "Ein kleiner Baum.";
    }
};

//HM02
class rockSmash
    : public move {
public:
    rockSmash( )
        : move( std::string( "Zertr\x81""mmerer" ),
        C 1,
        C 60,
        KAMPF,
        C 100,
        C 15,
        C 30,
        SELECTED,
        C 1,
        F( MAKES_CONTACT | PROTECT | KINGS_ROCK ),
        PHYS ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;

    bool possible( ) override;

    const char* text( ) override {
        return "Ein kleiner Felsen";
    }
};

//HM03
class fly
    : public move {
public:
    fly( )
        : move( std::string( "Fliegen" ),
        C 2,
        C 90,
        FLUG,
        C 100,
        C 15,
        C 100,
        SELECTED,
        C 1,
        F( MAKES_CONTACT | PROTECT | KINGS_ROCK ),
        PHYS ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;

    bool possible( ) override;

    const char* text( ) override {
        return "Freier Himmel.";
    }
};

//HM04
class flash
    : public move {
public:
    flash( ) :
        move( std::string( "Blitz" ),
        C 3,
        C 0,
        ELEKTRO,
        C 80,
        C 15,
        C 100,
        SELECTED,
        C 1,
        F( PROTECT | MAGIC_COAT ),
        STAT ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;

    bool possible( ) override;

    const char* text( ) override {
        return "Eine dunkle Höhle.";
    }
};

//HM05
class whirlpool
    : public move {
public:
    whirlpool( )
        : move( std::string( "Whirlpool" ),
        C 4,
        C 35,
        WASSER,
        C 85,
        C 15,
        C 100,
        SELECTED,
        C 1,
        F( PROTECT | KINGS_ROCK ),
        SPEC ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;

    bool possible( ) override;

    const char* text( ) override {
        return "Ein Strudel.";
    }
};

//HM06
class surf
    : public move {
public:
    surf( )
        : move( std::string( "Surfer" ),
        C 0,
        C 95,
        WASSER,
        C 95,
        C 15,
        C 100,
        BOTH_FOES_AND_PARTNER,
        C 1,
        F( PROTECT | KINGS_ROCK ),
        SPEC ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;

    bool possible( ) override;

    const char* text( ) override {
        return "Das Wasser ist tiefblau.";
    }
};

//HM07
class dive 
    : public move {
public:
    dive( ) 
        : move( "Taucher", 
        C 0, 
        C 80, 
        WASSER, 
        C 100,
        C 10,
        C 0, 
        A 0, 
        C 1, 
        F 35,
        H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Das Meer scheint hier\nbesonders tief.";
    }
};

//HM08
class defog 
    : public move {
public:
    defog( ) 
        : move( "Auflockern",
        C 0,
        C 0,
        FLUG,
        C 0,
        C 15,
        C 0,
        A 0,
        C 1,
        F 6,
        H 2 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Dichter Nebel.";
    }
};

//HM09
class strength 
    : public move {
public:
    strength( ) 
        : move( "Stärke",
        C 0,
        C 80, 
        NORMAL,
        C 100, 
        C 15,
        C 0, 
        A 0, 
        C 1,
        F 35, 
        H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Ein großer Felsen.";
    }
};

//HM10
class rockClimb
    : public move {
public:
    rockClimb( ) 
        : move( "Kraxler",
        C 0, 
        C 90,
        NORMAL, 
        C 85,
        C 20, 
        C 0, 
        A 0,
        C 1,
        F 35,
        H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Eine steile Felswand.";
    }
};

//HM11
class kaskade 
    : public move {
public:
    kaskade( ) 
        : move( "Kaskade",
        C 0, 
        C 80,
        WASSER,
        C 100,
        C 15,
        C 0, 
        A 0,
        C 1, 
        F 35,
        H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Ein Wasselfall.";
    }
};

class teleport 
    : public move {
public:
    teleport( ) 
        : move( "Teleport", 
        C 0,
        C 0,
        PSYCHO, 
        C 100, 
        C 20,
        C 0, 
        A 0,
        C 1,
        F 0,
        H 2 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Zu gefährlich hier?";
    }
};

class headbutt
    : public move {
public:
    headbutt( ) :
        move( "Kopfnuss",
        C 0,
        C 70,
        NORMAL,
        C 100,
        C 15,
        C 0,
        A 0,
        C 1,
        F 3,
        H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { }

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Da hat sich etwas bewegt!";
    }
};

class lockduft 
    : public move {
public:
    lockduft( ) 
        : move( "Lockduft",
        C 0,
        C 0,
        NORMAL,
        C 100,
        C 20,
        C 0,
        A 0,
        C 1,
        F 6,
        H 2 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) { } 

    bool possible( ) {
        return false;
    }

    const char* text( ) override {
        return "Der Geruch wilder Pokémon\nliegt in der Luft.";
    }
};

#undef C
#undef A
#undef F
#undef H