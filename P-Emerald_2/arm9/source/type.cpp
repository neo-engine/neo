/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : type.cpp
    author      : Philip Wellnitz (RedArceus)
    description : PKMN-Type definition and move effectiveness table.

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


#include "type.h"
#include <string>

std::string TypeList[19]=
{
    "\xc2\xc3\xc4\xc5","\xc6\xc7\xc8\xc9","\xca\xcb\xcc\xcd","\xce\xcf\xd0\xd1","\xd2\xd3\xd4\xd5",
    "\xb6\xb7\xb8\xb9","\x95\x96\x97\x98","\xae\xaf\xb0\xb1","\xb2\xb3\xb4\xb5","\xa6\xa7\xa8\xa9",
    "\x89\x8a\x8b\x8c","\x85\x86\x87\x88","\xaa\xab\xac\xad","\x9e\x9f\xa0\xa1","\xba\xbb\xbc\xbd",
    "\x8f\x90\x91\x92","\xa2\xa3\xa4\xa5","\xbe\xbf\xc0\xc1", "[FEE]"
};

std::string toString(Type T){
    return TypeList[T];
}

float TypeEffectiveness[19][19] = { 
    {   1,  1,  1,  1,  1,  .5, 1,  0,  .5, .75,    1,  1,  1,  1,  1,  1,  1,  1,  1   }, //Normal
    {   2,  1,  .5, .5, 1,  2,  .5, 0,  2,  .75,    1,  1,  1,  1,  .5, 2,  1,  2,  .5  }, //Fight
    {   1,  2,  1,  1,  1,  .5, 2,  1,  .5, .75,    1,  1,  2,  .5, 1,  1,  1,  1,  1   }, //Flying
    {   1,  1,  1,  .5, .5, .5, 1,  .5, 0,  .75,    1,  1,  2,  1,  1,  1,  1,  1,  2   }, //Poison
    {   1,  1,  0,  2,  1,  2,  .5, 1,  2,  .75,    1,  2,  .5, 2,  1,  1,  1,  1,  1   }, //Ground
    {   1,  .5, 2,  1,  .5, 1,  2,  1,  .5, .75,    1,  2,  1,  1,  1,  2,  1,  1,  1   }, //Rock
    {   1,  .5, .5, .5, 1,  1,  1,  .5, .5, .75,    1,  .5, 2,  1,  2,  1,  1,  2,  .5  }, //Bug
    {   0,  1,  1,  1,  1,  1,  1,  2,  .5, .75,    1,  1,  1,  1,  2,  1,  1,  .5, 1   }, //Ghost
    {   1,  1,  1,  1,  1,  2,  1,  1,  .5, .75,    .5, .5, 1,  .5, 1,  2,  1,  1,  2   }, //Steel
    {1.33,1.33,1.33,1.33,1.33,1.33,1.33,1.33,1.33,1,1.33,1.33,1.33,1.33,1.33,1.33,1.33,1.33,1.33}, //???
    {   1,  1,  1,  1,  2,  2,  1,  1,  1,  .75,    .5, 2,  .5, 1,  1,  1,  .5, 1,  1   }, //Water
    {   1,  1,  1,  1,  1,  .5, 2,  1,  2,  .75,    .5, .5, 2,  1,  1,  2,  .5, 1,  1   }, //Fire
    {   1,  1,  .5, .5, 2,  2,  .5, 1,  .5, .75,    2,  .5, .5, 1,  1,  1,  .5, 1,  1   }, //Grass
    {   1,  1,  2,  1,  0,  1,  1,  1,  1,  .75,    2,  1,  .5, .5, 1,  1,  .5, 1,  1   }, //Electric
    {   1,  2,  1,  2,  1,  1,  1,  1,  .5, .75,    1,  1,  1,  1,  .5, 1,  1,  0,  1   }, //Psychic
    {   1,  1,  2,  1,  2,  1,  1,  1,  .5, .75,    .5, .5, 2,  1,  1,  .5, 2,  1,  1   }, //Ice
    {   1,  1,  1,  1,  1,  1,  1,  1,  .5, .75,    1,  1,  1,  1,  1,  1,  2,  1,  0   }, //Dragon
    {   1,  .5, 1,  1,  1,  1,  1,  2,  .5, .75,    1,  1,  1,  1,  2,  1,  1,  .5, .5  }, //Dark
    {   1,  2,  1,  .5, 1,  1,  1,  1,  .5, .75,    1,  .5, 1,  1,  1,  1,  2,  2,  1   }  //Fairy
};

float getEffectiveness(const Type& t1, const Type& t2){ //t1 is attacking
    return TypeEffectiveness[(int)t1][(int)t2];
}