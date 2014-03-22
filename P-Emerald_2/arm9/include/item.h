/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : item.h
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
#include <map>

extern const char ITEM_PATH[];

class item{
public:
    enum EFFEKT{
        NONE = 0,
        IN_BATTLE = 1,
        HOLD = 2,
        OUT_OF_BATTLE = 4,
        USE_ON_PKMN = 8
    };
    enum ITEM_TYPE{
        GOODS,
        KEY_ITEM,
        TM_HM,
        MAILS,
        MEDICINE,
        BERRIES,
        POKE_BALLS,
        BATTLE_ITEM
    };

    std::string Name;//, displayName, dscrpt;
    //EFFEKT effekt;
    ITEM_TYPE itemtype;
    //std::string effekt_script;
    //int price;

    bool load;
    std::string getDisplayName(bool new_ = false);
    std::string getDescription(bool new_ = false);
    std::string getShortDescription(bool new_ = false);
    EFFEKT getEffekt();
    ITEM_TYPE getItemType();
    int getPrice();
    int getID();

    virtual bool _load();

    item(const std::string& name): Name(name){/* load = false;*/}
    item():Name("Null"){ /*load = false;*/ }

    void use(...);
};

class ball : public item{public: ball(const std::string& Name):item(Name){ this->itemtype = POKE_BALLS;}};
class medicine : public item{public:  medicine(const std::string& Name):item(Name){ this->itemtype = MEDICINE;}};
class TM : public item{public:  TM(const std::string& Name):item(Name){ this->itemtype = TM_HM;}};
class battle_item : public item{public:  battle_item(const std::string& Name):item(Name){ this->itemtype = BATTLE_ITEM;} };
class key_item : public item{public:  key_item(const std::string& Name):item(Name){ this->itemtype = KEY_ITEM;} };
class mail : public item{public:  mail(const std::string& Name):item(Name){ this->itemtype = MAILS;} };

extern item ItemList[700];