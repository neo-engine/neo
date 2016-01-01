/*
Pokémon Emerald 2 Version
------------------------------

file        : boxUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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
#include "box.h"
#include "uio.h"
#include <vector>

namespace BOX {
    class boxUI {
        friend class boxViewer;

        std::vector<std::pair<u8, IO::inputTarget>> _ranges;

        void init( );

        void updateAtHand( touchPosition p_touch, u8 p_oamIdx );

        std::vector<IO::inputTarget> draw( std::pair<u16, u16> p_pokemon[ 30 ], u8 p_pos, box* p_box, u8 p_oldpos = 42, bool p_newPok = true, bool p_showTeam = false );
        u8 getSprite( u8 p_oldIdx, u8 p_rangeIdx );
        u32 acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ); //First 10 bits: type, remaining: value
        u8 acceptTouch( u8 p_oldIdx, u8 p_rangeIdx, bool p_allowTakePkmn );
    };
}