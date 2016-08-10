/*
Pokémon Emerald 2 Version
------------------------------

file        : hmMoves.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2016
Philip Wellnitz

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
        { },
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
    const char* description( ) override {
        return "Ein Basisangriff mit Schere oder Klaue. Damit können kleine Bäume gefällt werden.";
    }
};

//HM02
class rockSmash
    : public move {
public:
    rockSmash( )
        : move( std::string( "Zertrümmerer" ),
        { },
                C 60,
                FIGHTING,
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
    const char* description( ) override {
        return "Steinbrechende Attacke, die den Verteidigungs-Wert des Zieles senken kann.";
    }
};

//HM03
class fly
    : public move {
public:
    fly( )
        : move( std::string( "Fliegen" ),
        { },
                C 90,
                FLYING,
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
    const char* description( ) override {
        return "Steigt in Runde 1 empor und trifft das Ziel in Runde 2. Ermöglicht Flug in jede bekannte Stadt.";
    }
};

//HM04
class flash
    : public move {
public:
    flash( ) :
        move( std::string( "Blitz" ),
        { },
              C 0,
              LIGHTNING,
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
    const char* description( ) override {
        return "Erzeugt helles Licht, das die Genauigkeit des Zieles senkt und Höhlen ausleuchten kann.";
    }
};

//HM05
class whirlpool
    : public move {
public:
    whirlpool( )
        : move( std::string( "Whirlpool" ),
        { },
                C 35,
                WATER,
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
    const char* description( ) override {
        return "Das Ziel wird für 4 bis 5 Runden in einer Wasserhose gefangen. Macht Strudel überwindbar.";
    }
};

//HM06
class surf
    : public move {
public:
    surf( )
        : move( std::string( "Surfer" ),
        { },
                C 95,
                WATER,
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
    const char* description( ) override {
        return "Eine Welle bricht über alle Pokémon in der Nähe des Anwenders herein. Macht Surfen möglich.";
    }
};

//HM07
class dive
    : public move {
public:
    dive( )
        : move( "Taucher",
        { },
                C 80,
                WATER,
                C 100,
                C 10,
                C 0,
                A 0,
                C 1,
                F 35,
                H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Das Meer scheint hier\nbesonders tief.";
    }
    const char* description( ) override {
        return "Taucht in Runde 1 ab und greift in Runde 2 aus der Tiefe an. Ermöglicht Tauchgänge zum Meeresgrund.";
    }

};

//HM08
class defog
    : public move {
public:
    defog( )
        : move( "Auflockern",
        { },
                C 0,
                FLYING,
                C 0,
                C 15,
                C 0,
                A 0,
                C 1,
                F 6,
                H 2 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Dichter Nebel.";
    }
    const char* description( ) override {
        return "Starker Wind hebt Attacken wie Reflektor und Lichtschild des Gegners auf. Senkt außerdem den Fluchtwert.";
    }
};

//HM09
class strength
    : public move {
public:
    strength( )
        : move( "Stärke",
        { },
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

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Ein großer Felsen.";
    }
    const char* description( ) override {
        return "Das Ziel wird extrem stark getroffen. Macht Verschieben von Felsen möglich.";
    }
};

//HM10
class rockClimb
    : public move {
public:
    rockClimb( )
        : move( "Kraxler",
        { },
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

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Eine steile Felswand.";
    }
    const char* description( ) override {
        return "Eine stürmische Attacke, die das Ziel eventuell verwirrt.";
    }
};

//HM11
class waterfall
    : public move {
public:
    waterfall( )
        : move( "Kaskade",
        { },
                C 80,
                WATER,
                C 100,
                C 15,
                C 0,
                A 0,
                C 1,
                F 35,
                H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Ein Wasselfall.";
    }
    const char* description( ) override {
        return "Eine mächtige Attacke, durch die das Ziel evtl. zurückschreckt. Wasserfälle können damit erklommen werden.";
    }
};

class teleport
    : public move {
public:
    teleport( )
        : move( "Teleport",
        { },
                C 0,
                PSYCHIC,
                C 100,
                C 20,
                C 0,
                A 0,
                C 1,
                F 0,
                H 2 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Zu gefährlich hier?";
    }
    const char* description( ) override {
        return "Fliehe sofort vor wilden Pokémon. Warp zum letzten Pokémon-Center.";
    }
};

class headbutt
    : public move {
public:
    headbutt( ) :
        move( "Kopfnuss",
        { },
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

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Da hat sich etwas bewegt!";
    }
    const char* description( ) override {
        return "Rammt das Ziel mit einer Kopfnuss. Ziel schreckt eventuell zurück.";
    }
};

class sweetScent
    : public move {
public:
    sweetScent( )
        : move( "Lockduft",
        { },
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

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Der Geruch wilder Pokémon\nliegt in der Luft.";
    }
    const char* description( ) override {
        return "Lockt Gegner an und senkt deren Fluchtwert. Lockt im Gras auch wilde Pokémon an.";
    }
};

class dig
    : public move {
public:
    dig( )
        : move( "Schaufler",
        { },
                C 80,
                GROUND,
                C 100,
                C 10,
                C 0,
                A 0,
                C 1,
                F 35,
                H 0 ) {
        this->m_isFieldAttack = true;
    }

    void use( ) override;
    bool possible( ) override;

    const char* text( ) override {
        return "Zu dunkel hier?";
    }
    const char* description( ) override {
        return "Gräbt sich ein, um dann aus der Erde anzugreifen. Warp zum Höhleneingang.";
    }
};
#undef C
#undef A
#undef F
#undef H
