#pragma once
#include <vector>
#include <nds.h>
#include "attack.h"

namespace POKEMON{
    class PKMN;
}
class item;
struct SpriteInfo;

namespace BATTLE{
    void displayHP(int HPstart,int HP,int x,int y,int freecolor1,int freecolor2,bool delay,bool big = false); //HP in %
    void displayHP(int HPstart,int HP,int x,int y,int freecolor1,int freecolor2,bool delay,int innerR,int outerR); //HP in %
    void displayEP(int EPstart,int EP,int x,int y,int freecolor1,int freecolor2,bool delay,int innerR = 14,int outerR = 15);

    int calcDamage(const attack& atk, const POKEMON::PKMN& atg, const POKEMON::PKMN& def,int rndVal);

    class battle_trainer{
    public:
        const char* Name;
        enum TrainerClass{
            PKMN_TRAINER
        }trainer_class;
        std::vector<POKEMON::PKMN>* pkmn_team;
    private:
        std::vector<item>* items;
        int money_earned;
        const char* Msg1,*Msg2,*Msg3,*Msg4;


    public:
        battle_trainer(const char* Name, const char* Msg1,const char* Msg2,const char* Msg3,const char* Msg4,std::vector<POKEMON::PKMN>* pkmnTeam,std::vector<item>* Items,TrainerClass t = PKMN_TRAINER)
            : Name(Name),trainer_class(t),pkmn_team(pkmnTeam),items(Items),Msg1(Msg1),Msg2(Msg2),Msg3(Msg3),Msg4(Msg4) { }

        POKEMON::PKMN& sendNewPKMN(bool choice = true);
        item& useItem(bool choice = true);

        const char* getLooseMsg() const { return Msg4; }
        int getLooseMoney() const { return money_earned; }
        const char* getWinMsg() const { return Msg3; }
        const char* getCriticalMsg() const { return Msg2; }
        const char* getInitMsg() const { return Msg1; }
    };
    class battle{
    private:
        int round, max_round, AI_level;
        const battle_trainer* player, *opponent;

        int acpokpos[6][2]; //me; opp
        enum acsts{
            OK = 0,
            STS = 1,
            KO = 2,
            NA = 3
        } acpoksts[6][2];

        attack::Ailment ailments[6][2];
        int ailmentcnt[6][2];

    public:
        bool distributeEXP;

        enum Weather{
            NONE = 0,
            RAIN = 1,
            HAIL = 2,
            FOG = 3,
            SANDSTORM = 4,
            SUN = 5
        }weather;
        enum BattleMode{
            SINGLE = 0,
            MULTI = 1,
            DOUBLE = 2
        }battlemode;
        battle(battle_trainer* player, battle_trainer* opponent,int max_round,int AI_level = 5,BattleMode battlemode = SINGLE);

        void initBattleScreen();
        int start(int battle_back,Weather weather); //Runs battle; returns -1 if opponent wins, 1 otherwise
        void switchOppPkmn(int newPok,int toSwitch = 0);
        void switchOwnPkmn(int newPok,int toSwitch = 0);
        
        int getChoice(int);
        int getTarget(int,int);
        int getSwitchPkmn(int&,int&,int&,bool);
        void printAttackChoiceScreen(int,int&,int&,int&);
        void printTargetChoiceScreen(int,int,int&,int&,int&);
        void printPKMNSwitchScreen(int& os2,int& pS2,int& ts2);
    private:
        void initBattleScene(int battle_back,Weather weather);
    };
}