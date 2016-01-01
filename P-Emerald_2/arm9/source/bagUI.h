/*
Pokémon Emerald 2 Version
------------------------------

file        : bagUI.h
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
#include <nds.h>

#include "bag.h"
#include "uio.h"

namespace BAG {
#define GIVE_ITEM 1
#define TAKE_ITEM 2
#define MOVE_ITEM 3

#define MOVE_BAG  4

#define BAG_CNT 5
    class bagUI {
        friend class bagViewer;
        std::vector<std::pair<IO::inputTarget, u16>> _ranges;
        std::vector<std::pair<u16, u16>> _bag[ BAG_CNT ];

        u8 _currSelectedIdx = 0;
        u8 _currPage;
        u16 _currItemIdx;

        u8 drawPkmn( item* p_item );
        void init( );

        void updateAtHand( touchPosition p_touch, u8 p_oamIdx );

        void updateSelectedIdx( u8 p_newIdx );
        std::vector<IO::inputTarget> drawBagPage( u8 p_page, u16 p_itemIdx );
        u8 getSprite( u8 p_rangeIdx, touchPosition p_currPos );
        u32 acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ); //First 10 bits: type, remaining: value
        u8 acceptTouch( u8 p_rangeIdx );
    };
}