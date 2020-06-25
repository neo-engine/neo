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
#include <string>
#include <nds.h>

#include "bag.h"
#include "uio.h"

namespace BAG {
    constexpr u8 GIVE_ITEM = 1;
    constexpr u8 TAKE_ITEM = 2;
    constexpr u8 MOVE_ITEM = 3;
    constexpr u8 MOVE_BAG  = 4;

    constexpr u8 BAG_CNT = 5;

    constexpr u8 MAX_ITEMS_PER_PAGE = 8;
    class bagUI {
      public:
        struct targetInfo {
            u16  m_item;
            bool m_isHeld;
        };

      private:
        pokemon* _playerTeam;

        u8       _selectedIdx = 255;
        bag::bagType  _currentPage;

        std::pair<u16, std::string> _teamItemCache[ 6 ];
        std::pair<u16, std::string> _itemCache[ MAX_ITEMS_PER_PAGE ];

        std::vector<std::pair<IO::inputTarget, targetInfo>> drawPkmn( u16             p_itemId,
                                                                      const ITEM::itemData* p_data );

        u8 _lastPkmnItemType = 255; // itemtype of the last item for which the pkmn info was drawn

        void drawItemSub( u16 p_itemId, const ITEM::itemData* p_data, u16 p_idx, bool p_selected,
                bool p_pressed, bool p_clearOnly = false );
      public:
        bagUI( pokemon* p_playerTeam ) : _playerTeam( p_playerTeam ) { }

        u16  drawPkmnIcons( );

        /*
         * @brief: Initializes the bag UI. Destroys anything on the screens.
         */
        void init( );

        /*
         * @brief: Redraws the specified page and displays the given items.
         */
        std::vector<std::pair<IO::inputTarget, targetInfo>>
            drawBagPage( bag::bagType,
                    const std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>>& p_items,
                    u8 p_selection = 0 );

        /*
         * @brief: Selects and highlights the specified item.
         */
        void selectItem( u8 p_idx, std::pair<u16, u16> p_item, const ITEM::itemData* p_data,
                         bool p_pressed = false );

        /*
         * @brief: Attaches the sprite corresponding to the specified item to the player's
         * touch input.
         */
        bool getSprite( u8 p_idx, std::pair<u16, u16> p_item, const ITEM::itemData* p_data );

        /*
         * @brief: Updates the current position of the sprite at hand.
         */
        void updateSprite( touchPosition p_touch );

        /*
         * @brief: Removes any sprite attached from the player's touch input.
         */
        void dropSprite( u8 p_idx, std::pair<u16, u16> p_item, const ITEM::itemData* p_data );
    };
} // namespace BAG
