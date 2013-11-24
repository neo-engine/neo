#pragma once

#include "type.h"
#include <string>

class attack
{
public:
	std::string Name;
	

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
		Name(_Name),Effect(_Effect),Base_Power(_Base_Power),type(_type),Accuracy(_Accuracy),PP(_PP),
		Effect_accuracy(_Effect_accuracy),Affects_whom(_Affects_whom),Priority(_Priority),Flag(_Flag), HitType(_HitType) { }
	~attack() { }
};