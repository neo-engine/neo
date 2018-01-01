/*
Pokémon Emerald 2 Version
------------------------------

file        : type.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2018
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include "type.h"

std::string TypeList[ 19 ] = {"\xc2\xc3\xc4\xc5",
                              "\xc6\xc7\xc8\xc9",
                              "\xca\xcb\xcc\xcd",
                              "\xce\xcf\xd0\xd1",
                              "\xd2\xd3\xd4\xd5",
                              "\xb6\xb7\xb8\xb9",
                              "\x95\x96\x97\x98",
                              "\xae\xaf\xb0\xb1",
                              "\xb2\xb3\xb4\xb5",
                              "\xa6\xa7\xa8\xa9",
                              "\x89\x8a\x8b\x8c",
                              "\x85\x86\x87\x88",
                              "\xaa\xab\xac\xad",
                              "\x9e\x9f\xa0\xa1",
                              "\xba\xbb\xbc\xbd",
                              "\x8f\x90\x91\x92",
                              "\xa2\xa3\xa4\xa5",
                              "\xbe\xbf\xc0\xc1",
                              "[FEE]"};

std::string toString( type p_type ) {
    return TypeList[ p_type ];
}

float TypeEffectiveness[ 19 ][ 19 ] = {
    {1, 1, 1, 1, 1, .5, 1, 0, .5, .75, 1, 1, 1, 1, 1, 1, 1, 1, 1},      // Normal
    {2, 1, .5, .5, 1, 2, .5, 0, 2, .75, 1, 1, 1, 1, .5, 2, 1, 2, .5},   // Fight
    {1, 2, 1, 1, 1, .5, 2, 1, .5, .75, 1, 1, 2, .5, 1, 1, 1, 1, 1},     // Flying
    {1, 1, 1, .5, .5, .5, 1, .5, 0, .75, 1, 1, 2, 1, 1, 1, 1, 1, 2},    // Poison
    {1, 1, 0, 2, 1, 2, .5, 1, 2, .75, 1, 2, .5, 2, 1, 1, 1, 1, 1},      // Ground
    {1, .5, 2, 1, .5, 1, 2, 1, .5, .75, 1, 2, 1, 1, 1, 2, 1, 1, 1},     // Rock
    {1, .5, .5, .5, 1, 1, 1, .5, .5, .75, 1, .5, 2, 1, 2, 1, 1, 2, .5}, // Bug
    {0, 1, 1, 1, 1, 1, 1, 2, .5, .75, 1, 1, 1, 1, 2, 1, 1, .5, 1},      // Ghost
    {1, 1, 1, 1, 1, 2, 1, 1, .5, .75, .5, .5, 1, .5, 1, 2, 1, 1, 2},    // Steel
    {1.33, 1.33, 1.33, 1.33, 1.33, 1.33, 1.33, 1.33, 1.33, 1, 1.33, 1.33, 1.33, 1.33, 1.33, 1.33,
     1.33, 1.33, 1.33},                                                 //???
    {1, 1, 1, 1, 2, 2, 1, 1, 1, .75, .5, 2, .5, 1, 1, 1, .5, 1, 1},     // Water
    {1, 1, 1, 1, 1, .5, 2, 1, 2, .75, .5, .5, 2, 1, 1, 2, .5, 1, 1},    // Fire
    {1, 1, .5, .5, 2, 2, .5, 1, .5, .75, 2, .5, .5, 1, 1, 1, .5, 1, 1}, // Grass
    {1, 1, 2, 1, 0, 1, 1, 1, 1, .75, 2, 1, .5, .5, 1, 1, .5, 1, 1},     // Electric
    {1, 2, 1, 2, 1, 1, 1, 1, .5, .75, 1, 1, 1, 1, .5, 1, 1, 0, 1},      // Psychic
    {1, 1, 2, 1, 2, 1, 1, 1, .5, .75, .5, .5, 2, 1, 1, .5, 2, 1, 1},    // Ice
    {1, 1, 1, 1, 1, 1, 1, 1, .5, .75, 1, 1, 1, 1, 1, 1, 2, 1, 0},       // Dragon
    {1, .5, 1, 1, 1, 1, 1, 2, .5, .75, 1, 1, 1, 1, 2, 1, 1, .5, .5},    // Dark
    {1, 2, 1, .5, 1, 1, 1, 1, .5, .75, 1, .5, 1, 1, 1, 1, 2, 2, 1}      // Fairy
};

float getEffectiveness( const type& p_t1, const type& p_t2 ) { // t1 is moving
    return TypeEffectiveness[ (int) p_t1 ][ (int) p_t2 ];
}
