/*
Pokémon Emerald 2 Version
------------------------------

file        : pokemon.h
author      : Philip Wellnitz (RedArceus)
description :

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


#include "bag.h"
#include "item.h"

#include <algorithm>
#include <nds.h>

namespace BAG {
    std::string bagnames[ 5 ] = { "Items", "Medizin", "TM/VM", "Beeren", "Basis-Items" };

    void bag::insert( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        _items[ u16( p_bagType ) ][ p_itemId ] += p_cnt;
    }

    void bag::erase( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        _items[ u16( p_bagType ) ][ p_itemId ] = std::max( 0, _items[ size_t( p_bagType ) ][ p_itemId ] - p_cnt );
    }

    u16 bag::count( bagType p_bagType, u16 p_itemId ) {
        return _items[ u16( p_bagType ) ][ p_itemId ];
    }

    bool bag::empty( bagType p_bagType ) {
        return _items[ u16( p_bagType ) ].empty( );
    }

    std::size_t bag::size( bagType p_bagType ) {
        return _items[ u16( p_bagType ) ].size( );
    }

    std::map<u16, u16> bag::element( bagType p_bagType ) {
        return _items[ u16( p_bagType ) ];
    }

    void bag::clear( bagType p_bagType ) {
        _items[ u16( p_bagType ) ].clear( );
    }

    bag::bagType toBagType( item::itemType p_itemType ) {
        switch( p_itemType ) {
            case item::GOODS:
            case item::MAILS:
            case item::POKE_BALLS:
            case item::BATTLE_ITEM:
                return bag::bagType::ITEMS;
            case item::KEY_ITEM:
                return bag::bagType::KEY_ITEMS;
            case item::TM_HM:
                return bag::bagType::TM_HM;
            case item::MEDICINE:
                return bag::bagType::MEDICINE;
            case item::BERRIES:
                return bag::bagType::BERRIES;
            default:
                break;
        }
        return bag::bagType::ITEMS;
    }
}