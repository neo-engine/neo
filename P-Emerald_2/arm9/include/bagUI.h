/*
Pokémon neo
------------------------------

file        : bagUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2020
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

#pragma once
#include <nds.h>

#include "bag.h"
#include "uio.h"

namespace BAG {
    const u8 GIVE_ITEM = 1;
    const u8 TAKE_ITEM = 2;
    const u8 MOVE_ITEM = 3;
    const u8 MOVE_BAG  = 4;

    const u8 BAG_CNT = 5;

    const u8 MAX_ITEMS_PER_PAGE = 9;
    class bagUI {
      public:
        struct targetInfo {
            u16  m_item;
            bool m_isHeld;
        };

      private:
        std::vector<std::pair<IO::inputTarget, targetInfo>> drawPkmn( u16             p_itemId,
                                                                      ITEM::itemData& p_data );

      public:
        u16  drawPkmnIcons( );
        void init( );

        std::vector<std::pair<IO::inputTarget, targetInfo>> drawBagPage( bag::bagType,
                                                                         u16 p_firstDisplayedItem );
        void selectItem( u8 p_idx, std::pair<u16, u16> p_item, ITEM::itemData& p_data,
                         bool p_pressed = false );
        void unselectItem( bag::bagType p_page, u8 p_idx, u16 p_item, ITEM::itemData& p_data );

        bool getSprite( u8 p_idx, std::pair<u16, u16> p_item, ITEM::itemData& p_data );
        void updateSprite( touchPosition p_touch );
        void dropSprite( bag::bagType p_page, u8 p_idx, std::pair<u16, u16> p_item,
                         ITEM::itemData& p_data );
    };
} // namespace BAG
