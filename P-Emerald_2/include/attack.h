#pragma once

#include "type.h"
#include <string>


class field_attack {
public:
    virtual void use() {  }
    virtual bool possible() { return false; }
};

class attack : public field_attack
{
public:
    std::string Name;
    
    bool isFieldAttack;
    char Effect;
    char Base_Power;
    Type type;
    char Accuracy;
    char PP;
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
        KINGS_ROCK = 32
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

    bool possible() { return false; }
    void use() { }
};
class cut : public attack{                                                //VM01
public: 
    void use(); bool possible();
    cut() : attack(std::string("Zerschneider"),(char)0,(char) 60,NORMAL, (char)100, (char)15,
        (char)0, SELECTED, (char)1, (Flags)(MAKES_CONTACT | PROTECT | KINGS_ROCK),PHYS) {
        this->isFieldAttack = true;
    }
};         
class rock_smash : public attack{                                                //VM02
public: 
    void use(); bool possible();
    rock_smash() : attack(std::string("Zertr\x81""mmerer"),(char)1,(char) 60,KAMPF, (char)100, (char)15,
        (char)30, SELECTED, (char)1, (Flags)(MAKES_CONTACT | PROTECT | KINGS_ROCK),PHYS) {
        this->isFieldAttack = true;
    }
};            
class fly : public attack{                                                //VM03
public: 
    void use(); bool possible();
    fly() : attack(std::string("Fliegen"),(char)2,(char) 90,FLUG, (char)100, (char)15,
        (char)100, SELECTED, (char)1, (Flags)(MAKES_CONTACT | PROTECT | KINGS_ROCK),PHYS) {
        this->isFieldAttack = true;
    }
};         
class flash : public attack{                                                //VM04
public: 
    void use(); bool possible();
    flash() : attack(std::string("Blitz"),(char)3,(char) 0,ELEKTRO, (char)80, (char)15,
        (char)100, SELECTED, (char)1, (Flags)(PROTECT | MAGIC_COAT),STAT) {
        this->isFieldAttack = true;
    }
};        
class whirlpool : public attack{                                                //VM05
public: 
    void use(); bool possible();
    whirlpool() : attack(std::string("Whirlpool"),(char)4,(char) 35,WASSER, (char)85, (char)15,
        (char)100, SELECTED, (char)1, (Flags)(PROTECT | KINGS_ROCK),SPEC) {
        this->isFieldAttack = true;
    }
};      
class surf : public attack{                                                //VM06
public: 
    void use(); bool possible();
    surf() : attack(std::string("Surfer"),(char)0,(char) 95,WASSER, (char)95, (char)15,
        (char)100, BOTH_FOES_AND_PARTNER, (char)1, (Flags)(PROTECT | KINGS_ROCK),SPEC) {
        this->isFieldAttack = true;
    }
}; 
class dive : public attack{public: void use(){} bool possible(){return false;} };         //VM07
class defog : public attack{public: void use(){} bool possible(){return false;} };        //VM08
class strength : public attack{public: void use(){} bool possible(){return false;} };     //VM09
class rock_climb : public attack{public: void use(){} bool possible(){return false;} };   //VM10
class kaskade : public attack{public: void use(){} bool possible(){return false;} };      //VM11

class teleport : public attack{public: void use(){} bool possible(){return false;} };
class headbutt : public attack{public: void use(){} bool possible(){return false;} };