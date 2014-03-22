/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : ability.h
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

    void run(...);
};

#endif