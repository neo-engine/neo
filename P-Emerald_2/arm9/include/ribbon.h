/*
Pokémon Emerald 2 Version
------------------------------

file        : ribbon.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#pragma once

#include <string>
#include <vector>

#include <nds.h>
#include "pokemon.h"

#include "r0.h"
#include "r1.h"
#include "r10.h"
#include "r11.h"
#include "r12.h"
#include "r13.h"
#include "r14.h"
#include "r15.h"
#include "r16.h"
#include "r17.h"
#include "r18.h"
#include "r19.h"
#include "r2.h"
#include "r20.h"
#include "r21.h"
#include "r22.h"
#include "r23.h"
#include "r24.h"
#include "r25.h"
#include "r26.h"
#include "r27.h"
#include "r28.h"
#include "r29.h"
#include "r3.h"
#include "r30.h"
#include "r31.h"
#include "r32.h"
#include "r33.h"
#include "r34.h"
#include "r35.h"
#include "r36.h"
#include "r37.h"
#include "r38.h"
#include "r39.h"
#include "r4.h"
#include "r40.h"
#include "r41.h"
#include "r42.h"
#include "r43.h"
#include "r44.h"
#include "r45.h"
#include "r46.h"
#include "r47.h"
#include "r48.h"
#include "r49.h"
#include "r5.h"
#include "r50.h"
#include "r51.h"
#include "r52.h"
#include "r53.h"
#include "r54.h"
#include "r55.h"
#include "r56.h"
#include "r57.h"
#include "r58.h"
#include "r59.h"
#include "r6.h"
#include "r60.h"
#include "r61.h"
#include "r62.h"
#include "r63.h"
#include "r64.h"
#include "r65.h"
#include "r66.h"
#include "r67.h"
#include "r68.h"
#include "r69.h"
#include "r7.h"
#include "r70.h"
#include "r71.h"
#include "r72.h"
#include "r73.h"
#include "r74.h"
#include "r75.h"
#include "r76.h"
#include "r77.h"
#include "r78.h"
#include "r79.h"
#include "r8.h"
#include "r80.h"
#include "r81.h"
#include "r82.h"
#include "r83.h"
#include "r84.h"
#include "r85.h"
#include "r9.h"

struct ribbon {
    std::string m_name;
    std::string m_description;

    static std::vector<u8> getRibbons( const pokemon& p_pokemon );
};

#define MAX_RIBBONS ( 12 * 8 )
extern ribbon RibbonList[ MAX_RIBBONS ];
