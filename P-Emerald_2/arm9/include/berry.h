/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : berry.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file (item.cpp) for details.

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
#include <nds.h>
#include "item.h"
#include "type.h"

class berry : public item
{
public:
    enum Guete_Type{
        Super_Weich = 0,
        Sehr_Weich = 1,
        Weich = 2,
        Hart = 3,
        Sehr_Hart = 4,
        Super_Hart = 5
    };
    enum Tastes{
        Spicy,
        Dry,
        Sweet,
        Bitter,
        Sour
    };

    //std::string dscrpt2;
    short  size;
    Guete_Type Guete;
    Type BeerenKr_Type;
    unsigned char BeerenKr_Str;
    unsigned char Taste[5];//Spicy,Dry,Sweet,Bitter,Sour
    unsigned char HoursPerStage, minBerries, maxBerries;

    std::string getDescription2(bool new_ = false);
    short getMass();
    short getSize();
    Guete_Type getGuete();
    std::pair<Type,unsigned char> getBeerenkraefteStats();
    unsigned char getTaste(Tastes taste);
    unsigned char getHoursPerStage();
    unsigned char getMinBerries();
    unsigned char getMaxBerries();

    bool _load();

    berry(const std::string& Name):item(Name){ this->itemtype = BERRIES;}
};
void initLocs();