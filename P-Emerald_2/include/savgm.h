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

    u8 owIdx;

    u8 flags[1000];

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
        u8 gameid;
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