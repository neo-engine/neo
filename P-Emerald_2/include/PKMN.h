#pragma once

#include <nds.h>
#include <nds\ndstypes.h>
#include <string>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "attack.h"
#include "ability.h"
#include "item.h"
#include "berry.h"
#include "savgm.h"
#include "print.h"
#include "font_data.h"
#include <time.h>
#include <map>

#undef RAND_MAX
#define RAND_MAX 4294967295

#define MAXPKMN 649
#define MAXSTOREDPKMN 800

class berry;

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
extern savgm SAV;

extern PrintConsole Top,Bottom;
extern font::Font cust_font;
extern font::Font cust_font2;

extern item ItemList[700];
extern ablty abilities[164];

extern attack* AttackList[560];
extern std::string NatureList[25];
extern std::string TypeList[19];
extern std::string PersonalityList[30];
extern std::map<int,std::string> Locations;

enum Type;//defined in "berry.h"
namespace POKEMON{
    enum LevelUp_Type{
        MEDIUM_FAST=0,
        ERRATIC=1,
        FLUCTUATING=2,
        MEDIUM_SLOW=3,
        FAST=4,
        SLOW=5
    };
    enum Color_Type{
        RED=0,
        BLUE=1,
        YELLOW=2,
        GREEN=3,
        BLACK=4,
        BROWN=5,
        PURPLE=6,
        GRAY=7,
        WHITE=8,
        PINK=9
    };
    enum Gender_Type{
        GENDERLESS=255,
        FEMALE=254,
    //   MALE_125=223,
        MALE_250=191,
        MALE_500=127,
        MALE_750=63,
        MALE_875=31,
        MALE=0
    };
    enum Egg_Type
    {
        NONE_ = 0,
        PFLANZE_=8,
        KAEFER_ = 4,
        FLUG_ = 5,
        HUMANOTYP_ = 9,
        MINERAL_ = 11,
        AMORPH_ = 12,
        FELD_=6,
        WASSER1_ = 2,
        WASSER2_ = 13,
        WASSER3_ = 10,
        MONSTER_ = 1,
        FEE_ = 7,
        DRACHE_ = 15,
        DITTO_ = 14,
        UNBEKANNT_ = 16
    };

    enum Natures{
        Robust = 0,	Einsam = 1,	Mutig = 2,	
        Hart = 3,	Frech = 4,	Kuehn =5,
        Sanft = 6,	Locker = 7,	Pfiffig = 8,
        Lasch=9,	Scheu=10,	Hastig =11,
        Ernst=12,	Froh=13,	Naiv=14,
        Maessig = 15,	Mild=16,	Ruhig =17,
        Zaghaft=18,	Hitzig = 19,	Still = 20,
        Zart = 21,	Forsch = 22,	Sacht = 23,
        Kauzig = 24
    };

    namespace PKMNDATA{
        #define GREEN RGB(0,24,0)
        #define RED RGB(24,0,0)
        #define BLUE RGB(0,0,24)
        #define WHITE RGB(31,31,31)
        #define NORMAL_ RGB(27,27,27)
        #define BLACK RGB(0,0,0)
        #define YELLOW RGB(24,24,0)
        #define PURPLE RGB(24,0,24)
        #define TURQOISE RGB(0,24,24)
        #define ICE RGB(15,31,31)
        #define FAIRY RGB(31,15,31)
        #define GROUND RGB(31,31,15)
        #define POISON RGB(31,0,15)
        #define ORANGE RGB(31,15,0)
        #define GHOST RGB(15,0,31)
        #define ROCK RGB(28,23,7)    
        #define BUG RGB(15,28,7)    
        #define STEEL RGB(24,24,24)
        #define DRAGON RGB(7,7,24)
        #define UNKNOWN RGB(0,42,42)
        struct PKMNDATA
        {
            Type Types[2];
            short Bases[6];
            short catchrate;
            unsigned short Items[4];
            Gender_Type gender;
            Egg_Type eggT[2];
            short eggcyc;
            short baseFriend;
            short EVYield[6];
            short EXPYield;
            short formecnt;
        };
        Type getType(int pkmnind,int type);
        short getBase(int pkmnind,int base);
        short getCatchRate(int pkmnind);
        const char* getDisplayName(int pkmnind);
        const wchar_t* getWDisplayName(int pkmnind);
        void getWDisplayName(int pkmnind,wchar_t* Name);
        void getHoldItems(int pkmnind,unsigned short* Items);
        Gender_Type getGenderType(int pkmnind);
        const char* getSpecies(int pkmnind);
        u16 getColor(Type T1);
        const char* getDexEntry(int pkmnind);
        short getForme(int pkmn,int formeId,const char* ret_forme_name);
        void getAll(int pkmnind,PKMNDATA& out);
    }

    extern double NatMod[25][5];
    extern LevelUp_Type Pkmn_LevelUpTypes[669];
    extern char Pkmn_Abilities[669][4];
    extern char Pkmn_SafariCatchRate[669];
    extern Color_Type Pkmn_Color[669];

    extern int EXP[100][13];

    bool operator==(touchPosition R,touchPosition L);

    extern unsigned int LastPID;
    extern int page;

    const short OTLENGTH = 8;
    const short PKMN_NAMELENGTH = 11;
    struct PKMN
    {
    public:
        struct BOX_PKMN{
            unsigned int PID                : 32;
            unsigned short int b1           : 16;
            unsigned short int checksum     : 16;
            //BLOCKA{
            unsigned short int SPEC         : 16; 
            unsigned short int Item         : 16;
            unsigned short int ID           : 16;
            unsigned short int SID          : 16;
            unsigned int exp                : 32;
            unsigned char steps             : 8; //StepstoHatch/256 // Happiness 
            unsigned char ability           : 8;
            union {
                struct {
                    u8 circle:1;
                    u8 triangle:1;
                    u8 square:1;
                    u8 heart:1;
                    u8 star:1;
                    u8 diamond:1;
                    u8 xbit:2; // unused
                } mark;
                u8 markings;
            };
            u8 orig_lang;
            unsigned char EV[6];	//HP,Attack,Defense,SAttack,SDefense,Speed
            unsigned char ConStats[6]; //Cool, Beauty, Cute, Smart, Tough, Sheen
            unsigned char ribbons1[4];
            //}

            //BLOCKB{
            unsigned short Attack[4];
            unsigned char AcPP[4]; //
            union {
                struct{
                    u8 Up1 : 2;
                    u8 Up2 : 2;
                    u8 Up3 : 2;
                    u8 Up4 : 2;
                }ppup;
                u8 PPUps;
            };

            union{
                struct{
                    unsigned char HP         : 5;
                    unsigned char Attack     : 5;
                    unsigned char Defense    : 5;
                    unsigned char Speed      : 5;
                    unsigned char SAttack    : 5;
                    unsigned char SDefense   : 5;
                    bool isNicked           : 1;
                    bool isEgg              : 1;
                } IV; //HP,Attack,Defense,Speed,SAttack,SDefense
                u32 IVint;
            };
            unsigned char ribbons0[4];
            bool fateful                : 1;
            bool isFemale               : 1;
            bool isGenderless           : 1;
            unsigned char alt_forme     : 5;
            bool cloned                 : 1;
            unsigned short int          : 15;
            unsigned short hatchPlace   : 16; //PT-like
            unsigned short gotPlace     : 16; //PT-like
            //}

            //BLOCKC{
            wchar_t Name[PKMN_NAMELENGTH];
            unsigned short hometown     : 16;
            unsigned char ribbons2[4];
            unsigned int                : 32;
            //}

            //BLOCKD{
            wchar_t OT[OTLENGTH];
            unsigned char gotDate[3];//(EGG)
            unsigned char hatchDate[3]; //gotDate for nonEgg
            unsigned int                : 32;
            unsigned char PKRUS         : 8;//
            unsigned char Ball          : 8;//
            unsigned short int gotLevel : 7;
            bool OTisFemale             : 1;
            enum enc : char {
                EGG=0x0,
                GRASS = 0x2,
                DIA_PAL = 0x4,
                CAVE = 0x5,
                SURF = 0x7,
                BUILDING = 0x9,
                SAFZONE = 0xA,
                STARTER = 0xC
            }encounter                  : 8;
            unsigned char HGSSBall;
            unsigned char               : 8;
            //}

            Natures getNature() { return (Natures)(this->PID %25); }
            int getAbility()    { return this->ability; }
            bool isShiny();
            bool isCloned();
            int gender();

            unsigned char IVget(int i){
                switch(i){
                case 0: return this->IV.HP;
                case 1: return this->IV.Attack;
                case 2: return this->IV.Defense;
                case 3: return this->IV.Speed;
                case 4: return this->IV.SAttack;
                case 5: return this->IV.SDefense;
                default: return 0;
                }
            }
            u8 PPupget(int i){
                switch(i){
                case 0: return this->ppup.Up1;
                case 1: return this->ppup.Up2;
                case 2: return this->ppup.Up3;
                case 3: return this->ppup.Up4;
                default: return 0;
                }
            }
            int getPersonality(){
                int counter = 1, i = PID%6;

                short max = i,maxval=IVget(i);
                for(;counter < 6;++counter)
                {
                    i = (i+1)%6;
                    if(IVget(i)>maxval)
                    {
                        maxval = IVget(i);
                        max = i;
                    }
                }

                return (max*5) + (maxval%5);
            }
            std::string getTasteStr(){
                if(NatMod[getNature()][0] == 1.2)
                    return "scharf";
                if(NatMod[getNature()][1] == 1.2)
                    return "saur";
                if(NatMod[getNature()][2] == 1.2)
                    return "s\xFE\xFF";
                if(NatMod[getNature()][3] == 1.2)
                    return "trocken";
                if(NatMod[getNature()][4] == 1.2)
                    return "bitter";
                else
                    return "all";
            }
            int getItem(){ return this->Item; }
            Type getHPType(){
                int a =((IVget(0)&1)+2*(IVget(1)&1)+4*(IVget(2)&1)+8*(IVget(3)&1)+16*(IVget(4)&1)+32*(IVget(5)&1)*15)/63;
                return a<9?(Type)a:Type(a+1);
            }
            int getHPPower(){		
                return 30+((((IVget(0)>>1)&1)+2*((IVget(1)>>1)&1)+4*((IVget(2)>>1)&1)+8*((IVget(3)>>1)&1)+16*((IVget(4)>>1)&1)+32*((IVget(5)>>1)&1)*40)/63);
                }
        
            BOX_PKMN(){}
            BOX_PKMN(char* Attacks,int SPE,const wchar_t* N,short level,unsigned short ID_,unsigned short SID_,const wchar_t* ot,
                bool OTFemale,bool Cloned,bool Shiny,bool h_a = false,bool fatef = false,bool _isEgg = false,short gPlace = 0,char BALL = 0, char Prus = 0);
        
            ~BOX_PKMN() {}

            int draw(){ return 0;}

        }boxdata;

        union{
            struct{
                unsigned char Asleep : 3;
                bool Poisoned       : 1;
                bool Burned         : 1;
                bool Frozen         : 1;
                bool Paralyzed      : 1;
                bool Toxic          : 1;
            } _status;
            u32 statusint;
        };
        unsigned char                   : 8;
        unsigned char                   : 8;
        unsigned char                   : 8;
        unsigned char Level             : 8;
        unsigned char                   : 8;
        struct Stats{
            unsigned short acHP     : 16;	//Aktuelle HP
            unsigned short maxHP    : 16;
            unsigned short Atk      : 16;
            unsigned short Def      : 16;
            unsigned short Spd      : 16;
            unsigned short SAtk     : 16;
            unsigned short SDef     : 16;
        }stats;

        PKMN(){}
        PKMN(char* Attacks,int SPE,const wchar_t* N,short level,unsigned short ID_,unsigned short SID_,const wchar_t* ot,
            bool OTFemale,bool Cloned,bool Shiny,bool h_a,bool fatef,bool _isEgg,short gPlace,char BALL, char Prus);
        //PKMN(const PKMN& pkmn){
        //    this->boxdata = pkmn.boxdata;
        //    this->_status = pkmn._status;
        //    this->Level = pkmn.Level;
        //    this->stats = pkmn.stats;
        //}
        ~PKMN() { }

        void evolve();
        void devolve();

        int draw();
        void drawPage(int Page,PrintConsole* Top,PrintConsole* Bottom,bool newpok);
    
        //std::pair<std::string,std::string> typesToString()
        //{
        //    std::pair<std::string,std::string> ret = std::pair<std::string,std::string>();
        //    ret.first = TypeList[Pkmn_Types[this->SPEC][0]];
        //    if(Pkmn_Types[this->SPEC][0]!=Pkmn_Types[this->SPEC][1])
        //         ret.second = TypeList[Pkmn_Types[this->SPEC][1]];
        //    return ret;
        //}
    };
}