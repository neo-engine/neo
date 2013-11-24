#pragma once

#include <string>
#include <vector>
#include "bag.h"

namespace POKEMON{
    class PKMN;
}
class BOX;

class savgm
    {
    public:	
        typedef std::pair<int,int> pii;	

        //General Player Data
        std::wstring PLAYERNAME;		
        bool IsMale;
        int ID;	
        int SID;	
        long long PLAYTIME;
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
        int Loc[5];
        
        //GameFlags
        bool hasGDex;

        bool good; //FailBit

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
    };	