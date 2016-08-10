/*
Pokémon Emerald 2 Version
------------------------------

file        : bagUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2016
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
#define MAX_ITEMS_PER_PAGE 9
    class bagUI {
    public:
        struct targetInfo {
            u16 m_item;
            bool m_isHeld;
        };
    private:
        std::vector<std::pair<IO::inputTarget, targetInfo>>
            drawPkmn( item* p_currentSelection );
    public:
        u16 drawPkmnIcons( );
        void init( );

        std::vector<std::pair<IO::inputTarget, targetInfo>>
            drawBagPage( bag::bagType, u16 p_firstDisplayedItem );
        void selectItem( u8 p_idx, std::pair<u16, u16> p_item, bool p_pressed = false );
        void unselectItem( bag::bagType p_page, u8 p_idx, u16 p_item );

        bool getSprite( u8 p_idx, std::pair<u16, u16> p_item );
        void updateSprite( touchPosition p_touch );
        void dropSprite( bag::bagType p_page, u8 p_idx, std::pair<u16, u16> p_item );
    };
}
