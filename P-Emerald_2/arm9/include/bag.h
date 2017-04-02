/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : bag.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2017
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
#include "item.h"
#include "defines.h"

namespace BAG {
    class bag {
    private:
        enum {
            ITEM_START = 0,
            MEDICINE_START = 550,
            TM_HM_START = 600,
            BERRIES_START = 725,
            KEY_ITEMS_START = 825
        };
        const u16 _startIdx[ 6 ] = { ITEM_START, MEDICINE_START, TM_HM_START,
            BERRIES_START, KEY_ITEMS_START, MAX_ITEMS_IN_BAG };
        std::pair<u16, u16> _items[ MAX_ITEMS_IN_BAG ];
        u16 _nextFree[ 5 ];
    public:
        enum bagType {
            ITEMS,
            MEDICINE,
            TM_HM,
            BERRIES,
            KEY_ITEMS
        };
        bag( ) {
            for( u8 i = 0; i < 5; ++i )
                _nextFree[ i ] = _startIdx[ i ];
        }

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
        * Swaps the items at the specified positions.
        */
        void swap( bagType, u16 p_idx1, u16 p_idx2 );

        /*
         * Returnes the number of items in the specified bag.
         */
        std::size_t size( bagType );

        /*
        * Returns the pointer to the first element in the bag.
        */
        std::pair<u16, u16>* begin( bagType );

        /*
        * Returns the pointer to the first element in the bag.
        */
        const std::pair<u16, u16>* cbegin( bagType ) const;

        /*
        * Returns the pointer after the last element of the bag.
        */
        std::pair<u16, u16>* end( bagType );

        /*
        * Returns the pointer after the last element of the bag.
        */
        const std::pair<u16, u16>* cend( bagType ) const;

        std::pair<u16, u16>& operator()( bagType p_bagType, u16 p_idx ) {
            return begin( p_bagType )[ p_idx ];
        }
        const std::pair<u16, u16>& operator()( bagType p_bagType, u16 p_idx ) const {
            return cbegin( p_bagType )[ p_idx ];
        }

        std::pair<u16, u16>* getItems( ) {
            return _items;
        }
    };

    bag::bagType toBagType( item::itemType p_itemType );
}
