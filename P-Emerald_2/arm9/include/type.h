/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : type.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
        Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        is required.

    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
        distribution.
*/

#pragma once

enum Type{
    NORMAL=0,	KAMPF=1,	FLUG=2,     GIFT=3,	    BODEN=4,
    GESTEIN=5,	KAEFER=6,	GEIST=7,	STAHL=8,	UNBEKANNT=9,
    WASSER=10,	FEUER=11,	PFLANZE=12,	ELEKTRO=13,	PSYCHO=14,
    EIS=15,     DRACHE=16,	UNLICHT=17, FEE = 18
};

float getEffectiveness(const Type& t1, const Type& t2);