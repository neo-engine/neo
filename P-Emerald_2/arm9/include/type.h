#pragma once

enum Type{
    NORMAL=0,	KAMPF=1,	FLUG=2,     GIFT=3,	    BODEN=4,
    GESTEIN=5,	KAEFER=6,	GEIST=7,	STAHL=8,	UNBEKANNT=9,
    WASSER=10,	FEUER=11,	PFLANZE=12,	ELEKTRO=13,	PSYCHO=14,
    EIS=15,     DRACHE=16,	UNLICHT=17, FEE = 18
};

float getEffectiveness(const Type& t1, const Type& t2);