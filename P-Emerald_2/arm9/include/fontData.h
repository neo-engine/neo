/*
Pokémon Emerald 2 Version
------------------------------

file        : fontData.h
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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
#include <nds.h>

namespace FONT {
    namespace font1 {
#define NUM_CHARS 490
        void shiftchar( u16& val );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    }
    namespace font2 {
#define NUM_CHARS 490
        void shiftchar( u16& val );
        extern u8 fontWidths[ NUM_CHARS ];
        extern u8 fontData[ NUM_CHARS * 256 ];
    }
}