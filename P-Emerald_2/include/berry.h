#pragma once

#include <string>
#include <nds.h>
#include "item.h"
#include "type.h"

class berry : public item
{
public:
	enum Guete_Type{
        Super_Weich = 0,
        Sehr_Weich = 1,
		Weich = 2,
        Hart = 3,
        Sehr_Hart = 4,
        Super_Hart = 5
	};
    enum Tastes{
        Spicy,
        Dry,
        Sweet,
        Bitter,
        Sour
    };

	//std::string dscrpt2;
	short  size;
	Guete_Type Guete;
	Type BeerenKr_Type;
	unsigned char BeerenKr_Str;
	unsigned char Taste[5];//Spicy,Dry,Sweet,Bitter,Sour
	unsigned char HoursPerStage, minBerries, maxBerries;

    std::string getDescription2();
    short getMass();
    short getSize();
    Guete_Type getGuete();
    std::pair<Type,unsigned char> getBeerenkraefteStats();
    unsigned char getTaste(Tastes taste);
    unsigned char getHoursPerStage();
    unsigned char getMinBerries();
    unsigned char getMaxBerries();

    bool _load();

    berry(const std::string& Name):item(Name){ this->itemtype = BERRIES;}
};
void initLocs();