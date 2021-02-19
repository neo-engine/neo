/*
Pokémon neo
------------------------------

file        : mapBankData.cpp
author      : Philip Wellnitz
description : Door animations

Copyright (C) 2012 - 2021
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mapSlice.h"

namespace MAP {
    // tiles generated from *.png (using pal0)  via sprite2raw $i 32 0 0 0 0xa8a3 0x7fff
    // 0x779b 0x7337 0x6ed4 0x4610 0x3d8c 0x3528 0x2cc5 0x7af1 0x76af 0x7af3 0x6e8f 0x622d
    // 0x55cc 0x2eaa

    door DOOR_ANIMATIONS[ DOOR_ANIMATION_COUNT ] = {
        // { TS, No, Pal, Block },
        { 0, 0, 1, 33 },    { 0, 1, 1, 97 },    { 0, 2, 5, 461 },    { 0, 3, 0, 65 },
        { 0, 9, 10, 647 },

        { 1, 4, 10, 584 },  { 1, 5, 8, 585 },   { 1, 14, 1, 475 },

        { 2, 6, 11, 559 },  { 2, 7, 10, 543 },

        { 3, 13, 5, 549 },

        { 4, 12, 1, 732 },  { 4, 42, 1, 915 },

        { 5, 10, 7, 684 },  { 5, 11, 5, 929 },  { 5, 43, 1, 980 },

        { 7, 8, 7, 677 },   { 7, 44, 1, 876 },  { 7, 29, 7, 759 },

        { 9, 24, 5, 780 },  { 9, 25, 9, 813 },  { 9, 15, 8, 582 },   { 9, 16, 5, 654 },

        { 10, 17, 1, 673 }, { 10, 26, 8, 749 },

        { 11, 20, 8, 541 },

        { 12, 21, 9, 538 },

        { 13, 18, 6, 540 }, { 13, 19, 6, 542 },

        { 14, 36, 9, 611 }, { 14, 35, 1, 650 }, { 14, 47, 1, 1020 }, { 14, 48, 1, 918 },

        { 15, 38, 5, 657 }, { 15, 37, 0, 809 }, { 15, 47, 1, 1020 }, { 15, 48, 1, 918 },
    };
} // namespace MAP
