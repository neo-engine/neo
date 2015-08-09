/*
Pokémon Emerald 2 Version
------------------------------

file        : mapObject.h
author      : Philip Wellnitz
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
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
#include <nds/ndstypes.h>
#include "mapSlice.h"

namespace MAP {
    struct mapObject {
        enum type {
            PLYR,
            SIGN,
            NPC
        } m_type;
        mapSlice::position m_pos;
        u16 m_picNum;
        mapSlice::moveMode m_movement;
        u16 m_range;
        u16 m_id;

        void interact( );
        void move( mapSlice::direction p_direction, u8 p_frame );
        void show( u16 p_currX, u16 p_currY, u8 p_oamIdx, u8 p_palIdx, u16 p_tileIdx );
    };
}