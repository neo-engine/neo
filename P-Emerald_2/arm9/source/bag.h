/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : bag.h
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
#include <map>
#include <nds.h>
#include <string>
#include <map>
#include "item.h"

namespace BAG {
#define MAXITEMCOUNT 999

    extern std::string bagnames[ 5 ];
    
    class bag {
    private:
        std::map<u16, u16> _items[ 5 ];
    public:
        enum bagType {
            ITEMS,
            MEDICINE,
            TM_HM,
            BERRIES,
            KEY_ITEMS
        };

        /*
         * Adds cnt items with no. item_id to the bag.
         */
        void insert( bagType, u16 p_itemId, u16 p_cnt );

        /*
         * Removes cnt items with no. item_id from the bag.
         * cnt == -1: removes all items of desired kind
         */
        void erase( bagType, u16 p_itemId, u16 p_cnt = -1 );

        /*
         * Returnes the number of items with no. item_id in the bag.
         */
        u16 count( bagType, u16 p_itemId );

        /*
         * Returnes true if the specified bag is empty.
         */
        bool empty( bagType );

        /*
         * Clears the specified bag.
         */
        void clear( bagType );

        /*
         * Returnes the number of items in the specified bag.
         */
        std::size_t size( bagType );

        std::map<u16, u16> element( bagType );

        std::vector<item>& getBattleItems( ) {
            static std::vector<item> res;
            return res;
        }
    };

    bag::bagType toBagType( item::itemType p_itemType );
}