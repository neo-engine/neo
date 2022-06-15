/*
    Pokémon neo
    ------------------------------

    file        : bag.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2022
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
#include <map>
#include <string>
#include <nds.h>
#include "bag/item.h"
#include "defines.h"

namespace BAG {
    class bag {
      private:
        static const u16 ITEM_START      = 0;
        static const u16 MEDICINE_START  = 500;
        static const u16 TM_HM_START     = 625;
        static const u16 BERRIES_START   = 850;
        static const u16 KEY_ITEMS_START = 950;

        u16                 _startIdx[ 6 ] = { ITEM_START,    MEDICINE_START,  TM_HM_START,
                               BERRIES_START, KEY_ITEMS_START, MAX_ITEMS_IN_BAG };
        std::pair<u16, u16> _items[ MAX_ITEMS_IN_BAG ];
        u16                 _nextFree[ 5 ];

      public:
        enum bagType { ITEMS, MEDICINE, TM_HM, BERRIES, KEY_ITEMS };
        bag( ) {
            for( u8 i = 0; i < 5; ++i ) _nextFree[ i ] = _startIdx[ i ];
        }

        /*
         * @brief: Sorts the specified bag.
         */
        void sort( bagType );

        /*
         * @brief: Adds cnt items with no. item_id to the bag.
         */
        void insert( bagType, u16 p_itemId, u16 p_cnt );

        /*
         * @brief: Removes cnt items with no. item_id from the bag.
         * cnt == -1: removes all items of desired kind
         */
        void erase( bagType, u16 p_itemId, u16 p_cnt = -1 );

        /*
         * @brief: Returns the number of items with no. item_id in the bag.
         */
        u16 count( bagType, u16 p_itemId );

        /*
         * @brief: Returns true if the specified bag is empty.
         */
        bool empty( bagType );

        /*
         * @brief: Clears the specified bag.
         */
        void clear( bagType );

        /*
         * @brief: Swaps the items at the specified positions.
         */
        void swap( bagType, u16 p_idx1, u16 p_idx2 );

        /*
         * @brief: Returns the number of items in the specified bag.
         */
        std::size_t size( bagType );

        /*
         * @brief: Returns the pointer to the first element in the bag.
         */
        std::pair<u16, u16>* begin( bagType );

        /*
         * @brief: Returns the pointer to the first element in the bag.
         */
        const std::pair<u16, u16>* cbegin( bagType ) const;

        /*
         * @brief: Returns the pointer after the last element of the bag.
         */
        std::pair<u16, u16>* end( bagType );

        /*
         * @brief: Returns the pointer after the last element of the bag.
         */
        const std::pair<u16, u16>* cend( bagType ) const;

        std::pair<u16, u16>& operator( )( bagType p_bagType, u16 p_idx ) {
            return begin( p_bagType )[ p_idx ];
        }

        const std::pair<u16, u16>& operator( )( bagType p_bagType, u16 p_idx ) const {
            return cbegin( p_bagType )[ p_idx ];
        }

        std::pair<u16, u16>* getItems( ) {
            return _items;
        }
    };

    constexpr bag::bagType toBagType( u8 p_itemType ) {
        if( p_itemType & ITEMTYPE_BERRY ) { return bag::bagType::BERRIES; }

        if( ( p_itemType & 15 ) == ITEMTYPE_MEDICINE ) { return bag::bagType::MEDICINE; }

        switch( p_itemType ) {
        case ITEMTYPE_FORMECHANGE:
        case ITEMTYPE_KEYITEM: return bag::bagType::KEY_ITEMS;
        case ITEMTYPE_TM: return bag::bagType::TM_HM;
        default: break;
        }
        return bag::bagType::ITEMS;
    }
} // namespace BAG
