/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : savgm.h
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
#include <vector>
#include "bag.h"

namespace POKEMON{
    class PKMN;
}
class BOX;
enum SavMod{
    _NDS,
    _GBA
};
class savgm
{
public:	
    typedef std::pair<int,int> pii;	

    //General Player Data
    std::wstring PLAYERNAME;		
    bool IsMale;
    int ID;	
    int SID;	
    union{
        u32 PLAYTIME;
        struct{
            u16 hours;
            u8 mins;
            u8 secs;

        }pt;
    };
    int HOENN_Badges;
    int KANTO_Badges;
    int JOHTO_Badges;
    short Orden;
    int Dex;
    std::vector<bool> inDex;

    int SavTyp;

    int Money;

    bool hasPKMN;
    bool activatedPNav;

    //Bag
    bag Bag;

    //Team
    std::vector<POKEMON::PKMN> PKMN_team;

    u16 owIdx;

    u16 flags[500];

    int acposx, acposy, acposz;
    int acMapIdx;
    int acMoveMode;
    char acMapName[100];

    bool EXPShareEnabled;
    bool EvolveInBattle;

    //GameFlags
    bool hasGDex;

    bool good; //FailBit

    struct {
        u16 gameid;
    }gba;

    savgm():PLAYERNAME(),IsMale(true),ID(0),SID(0)
    {
        inDex = std::vector<bool>(650,false);
        good = true;
    }
    savgm(std::wstring P_Name):PLAYERNAME(P_Name),IsMale(true),ID(0),SID(0)
    {
        inDex = std::vector<bool>(650,false);
        good = true;
    }
    savgm(void Func(int));
    ~savgm(){}

    bool save(void Func(int));
    std::wstring getName() { return this->PLAYERNAME; }
    void setName(const std::wstring& val) { this->PLAYERNAME = val; }

    bool checkflag(int idx){
        return flags[idx >> 3] & ( 1 << (idx % 8));
    }
    void setflag(int idx, bool value){
        if(value != checkflag(idx))
            flags[idx >> 3] ^= ( 1 << (idx % 8));
        return;
    }
};	