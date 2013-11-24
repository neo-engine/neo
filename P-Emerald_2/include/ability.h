#include <string>

#ifndef __ABIL__
#define __ABIL__

class ablty{
public:
	enum type{
		ATTACK=1,
		BEFORE_BATTLE=2,
		AFTER_BATTLE=4,
		GRASS=8,
		BEFORE_ATTACK=16,
		AFTER_ATTACK=32
	}; 
	const std::string Name;
	std::string FlavourText;
	type T;
	ablty(){}
	ablty(const std::string S):Name(S){}
	ablty(const std::string S,int t):Name(S),T((type)t){}
	ablty(const std::string S,const std::string FText,int t):Name(S),FlavourText(FText),T(type(t)){}
};

#endif