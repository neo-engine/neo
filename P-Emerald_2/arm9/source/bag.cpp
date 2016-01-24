/*
Pokémon Emerald 2 Version
------------------------------

file        : bag.cpp
author      : Philip Wellnitz
description :

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


#include "bag.h"
#include "item.h"

#include <algorithm>
#include <nds.h>

namespace BAG {
    std::string bagnames[ 5 ] = { "Items", "Medizin", "TM/VM", "Beeren", "Basis-Items" };
    
    void bag::insert( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        for( u16 i = 0; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ _startIdx[ p_bagType ] + i ].first == p_itemId ) {
                _items[ _startIdx[ p_bagType ] + i ].second += p_cnt;
                return;
            }
        if( _nextFree[ p_bagType ] >= MAX_ITEMS_IN_BAG
            || _nextFree[ p_bagType ] == _startIdx[ 1 + p_bagType ] ) //Insert failed.
            return;
        _items[ _nextFree[ p_bagType ]++ ] = { p_itemId, p_cnt };
    }

    void bag::erase( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        for( u16 i = 0; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ _startIdx[ p_bagType ] + i ].first == p_itemId ) {
                if( p_cnt <= _items[ _startIdx[ p_bagType ] + i ].second )
                    _items[ _startIdx[ p_bagType ] + i ].second -= p_cnt;
                else
                    _items[ _startIdx[ p_bagType ] + i ].second = 0;
                return;
            }
    }

    u16 bag::count( bagType p_bagType, u16 p_itemId ) {
        for( u16 i = 0; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ _startIdx[ p_bagType ] + i ].first == p_itemId )
                return _items[ _startIdx[ p_bagType ] + i ].second;
        return -1;
    }

    bool bag::empty( bagType p_bagType ) {
        return !( _nextFree[ p_bagType ] - _startIdx[ p_bagType ] );
    }

    std::size_t bag::size( bagType p_bagType ) {
        return _nextFree[ p_bagType ] - _startIdx[ p_bagType ];
    }

    std::pair<u16, u16>* bag::begin( bagType p_bagType ) {
        return _items + _startIdx[ p_bagType ];
    }
    const std::pair<u16, u16>* bag::begin( bagType p_bagType ) const {
        return _items + _startIdx[ p_bagType ];
    }
    std::pair<u16, u16>* bag::end( bagType p_bagType ) {
        return _items + _nextFree[ p_bagType ];
    }
    const std::pair<u16, u16>* bag::end( bagType p_bagType ) const {
        return _items + _nextFree[ p_bagType ];
    }

    void bag::clear( bagType p_bagType ) {
        memset( _items + _startIdx[ p_bagType ], 0, _startIdx[ p_bagType + 1 ] - _startIdx[ p_bagType ] + 1 );
        _nextFree[ p_bagType ] = _startIdx[ p_bagType ];
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