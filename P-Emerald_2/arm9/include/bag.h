/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : bag.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2015
    Philip Wellnitz (RedArceus)

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
#include <vector>
#include "item.h"

#define MAXITEMCOUNT 999
class bag {
public:
    //Goods,Keys,TMs,Mails,Medicine,Berries,PokéBalls,BattleItems
    typedef ITEMS::item::itemType bagtype;

    std::vector<std::pair<u16, u16> > m_bags[ 8 ];

    bag( ) {
        for( u8 i = 0; i < 8; ++i )
            this->m_bags[ i ].clear( );
    }
    ~bag( ) { }

    /*
     * Adds cnt items with mo. item_id to the bag.
     */
    void            addItem( bagtype, u16 p_itemId, u16 p_cnt );

    /*
     * Removes cnt items with no. item_id from the bag.
     * cnt == -1: removes all items of desired kind
     */
    void            removeItem( bagtype, u16 p_itemId, u16 p_cnt = -1 );

    /*
     * Returnes the number of items with no. item_id in the bag.
     */
    u16             countItem( bagtype, u16 p_itemId );

    /*
     * Returnes true if the specified bag is empty.
     */
    bool            empty( bagtype );

    /*
     * Clears the specified bag.
     */
    void            clear( bagtype );

    /*
     * Returnes the number of items in the specified bag.
     */
    std::size_t     size( bagtype );

    std::pair<u16, u16>
        elementAt( bagtype, u16 );

    void            draw( u8 p_startBag, u8 p_startItemIdx );
};