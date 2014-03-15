#pragma once

#include "type.h"
#include <string>

#define MAXATTACK 560

class attack
{
public:
    enum Ailment{
        none,
        Paralysis,
        Sleep,
        Freeze,
        Burn,
        Poison,
        Confusion,
        Infatuation,
        Trap,
        Nightmare,
        Torment,
        Disable,
        Yawn,
        Heal_Block,
        No_type_immunity,
        Leech_Seed,
        Embargo,
        Perish_Song,
        Ingrain
    };

    bool isFieldAttack;
    std::string Name;

    char Effect;
    char Base_Power;
    Type type;
    char Accuracy;
    unsigned char PP;
    char Effect_accuracy;
    enum AffectsTypes
    {
        SELECTED = 0,
        DEPENDS_ON_ATTACK = 1,
        OWN_FIELD = 2,
        RANDOM = 4,
        BOTH_FOES = 8,
        USER = 16,
        BOTH_FOES_AND_PARTNER = 32,
        OPPONENTS_FIELD = 64
    } Affects_whom;
    char Priority;
    enum Flags{
        MAKES_CONTACT = 1,
        PROTECT = 2,
        MAGIC_COAT = 4,
        SNATCH = 8,
        MIRROR_MOVE = 16,
        KINGS_ROCK = 32,
        SOUNDPROOF = 64
    } Flag;

    enum HitTypes{
        PHYS = 0,
        SPEC = 1,
        STAT = 2
    }HitType;

    attack() { }
    attack(const std::string _Name,char _Effect,char _Base_Power,Type _type,char _Accuracy,
        char _PP,char _Effect_accuracy,AffectsTypes _Affects_whom,char _Priority,Flags _Flag,HitTypes _HitType):
    isFieldAttack(false), Name(_Name),Effect(_Effect),Base_Power(_Base_Power),type(_type),Accuracy(_Accuracy),PP(_PP),
        Effect_accuracy(_Effect_accuracy),Affects_whom(_Affects_whom),Priority(_Priority),Flag(_Flag), HitType(_HitType) { }
    ~attack() { }

    virtual bool possible() { return false; }
    virtual void use() { }
    virtual const char* text() { return 0;}
};


#define C (char)
#define A (attack::AffectsTypes)
#define F (attack::Flags)
#define H (attack::HitTypes)

class cut : public attack{                                                //VM01
public: 
    void use() override; bool possible() override;
    cut() : attack(std::string("Zerschneider"),(char)0,(char) 60,NORMAL, (char)100, (char)15,
        (char)0, SELECTED, (char)1, (Flags)(MAKES_CONTACT | PROTECT | KINGS_ROCK),PHYS) {
            this->isFieldAttack = true;
    }
    const char* text() override { return "Ein kleiner Baum."; }
};         
class rock_smash : public attack{                                                //VM02
public: 
    void use() override; bool possible() override;
    rock_smash() : attack(std::string("Zertr\x81""mmerer"),(char)1,(char) 60,KAMPF, (char)100, (char)15,
        (char)30, SELECTED, (char)1, (Flags)(MAKES_CONTACT | PROTECT | KINGS_ROCK),PHYS) {
            this->isFieldAttack = true;
    }
    const char* text() override { return "Ein kleiner Felsen"; }
};            
class fly : public attack{                                                //VM03
public: 
    void use() override; bool possible() override;
    fly() : attack(std::string("Fliegen"),(char)2,(char) 90,FLUG, (char)100, (char)15,
        (char)100, SELECTED, (char)1, (Flags)(MAKES_CONTACT | PROTECT | KINGS_ROCK),PHYS) {
            this->isFieldAttack = true;
    }
    const char* text() override { return "Freier Himmel."; }
};         
class flash : public attack{                                                //VM04
public: 
    void use() override; bool possible() override;
    flash() : attack(std::string("Blitz"),(char)3,(char) 0,ELEKTRO, (char)80, (char)15,
        (char)100, SELECTED, (char)1, (Flags)(PROTECT | MAGIC_COAT),STAT) {
            this->isFieldAttack = true;
    }
    const char* text() override { return "Eine dunkle Höhle."; }
};        
class whirlpool : public attack{                                                //VM05
public: 
    void use() override; bool possible() override;
    whirlpool() : attack(std::string("Whirlpool"),(char)4,(char) 35,WASSER, (char)85, (char)15,
        (char)100, SELECTED, (char)1, (Flags)(PROTECT | KINGS_ROCK),SPEC) {
            this->isFieldAttack = true;
    }
    const char* text() override { return "Ein Strudel."; }
};      
class surf : public attack{                                                //VM06
public: 
    void use() override; bool possible() override;
    surf() : attack(std::string("Surfer"),(char)0,(char) 95,WASSER, (char)95, (char)15,
        (char)100, BOTH_FOES_AND_PARTNER, (char)1, (Flags)(PROTECT | KINGS_ROCK),SPEC) {
            this->isFieldAttack = true;
    }
    const char* text() override { return "Das Wasser ist tiefblau."; }
}; 
class dive : public attack{public: 
dive():attack("Taucher",    C 0,    C 80,   WASSER,    C 100,     C 10,   C 0,        A 0,    C 1,    F 35,   H 0){ 
            this->isFieldAttack = true;}
void use(){} bool possible(){return false;} 
const char* text() override { return "Das Meer scheint hier\nbesonders tief."; }};         //VM07
class defog : public attack{public:
defog():attack("Auflockern",    C 0,    C 0,   FLUG,    C 0,     C 15,   C 0,        A 0,    C 1,    F 6,   H 2){ 
            this->isFieldAttack = true;}
void use(){} bool possible(){return false;}
const char* text() override { return "Dichter Nebel."; } };        //VM08
class strength : public attack{public: 
strength() : attack("Stärke",    C 0,    C 80,   NORMAL,    C 100,     C 15,   C 0,        A 0,    C 1,    F 35,   H 0) {
            this->isFieldAttack = true;}
void use(){} bool possible(){return false;} 
const char* text() override { return "Ein großer Felsen."; }};     //VM09
class rock_climb : public attack{public: 
rock_climb() : attack("Kraxler",    C 0,    C 90,   NORMAL,    C 85,     C 20,   C 0,        A 0,    C 1,    F 35,   H 0){
            this->isFieldAttack = true; }
void use(){} bool possible(){return false;} 
const char* text() override { return "Eine steile Felswand."; }};   //VM10
class kaskade : public attack{public:
kaskade() : attack("Kaskade",    C 0,    C 80,   WASSER,    C 100,     C 15,   C 0,        A 0,    C 1,    F 35,   H 0){
            this->isFieldAttack = true;}
void use(){} bool possible(){return false;}
const char* text() override { return "Ein Wasselfall."; } };      //VM11

class teleport : public attack{public: 
teleport() : attack("Teleport",    C 0,    C 0,   PSYCHO,    C 100,     C 20,   C 0,        A 0,    C 1,    F 0,   H 2) { 
            this->isFieldAttack = true;}
void use(){} bool possible(){return false;} 
const char* text() override { return "Zu gefährlich hier?"; }};
class headbutt : public attack{public: headbutt():
    attack("Kopfnuss",    C 0,    C 70,   NORMAL,    C 100,     C 15,   C 0,        A 0,    C 1,    F 3,   H 0){
            this->isFieldAttack = true;}
void use(){} bool possible(){return false;} 
const char* text() override { return "Da hat sich etwas bewegt!"; }};
class lockduft : public attack{
public: 
    lockduft() : attack("Lockduft", C 0, C 0, NORMAL, C 100, C 20, C 0, A 0, C 1, F 6, H 2) {
            this->isFieldAttack = true;}
    void use(){} bool possible(){return false;} 
    const char* text() override { return "Der Geruch wilder Pokémon\nliegt in der Luft."; }
};

#undef C
#undef A
#undef F
#undef H