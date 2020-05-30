/*
Pokémon neo
------------------------------

file        : bag.cpp
author      : Philip Wellnitz
description :

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

#include "bag.h"
#include "pokemon.h"
#include "saveGame.h"

#include <algorithm>
#include <nds.h>

namespace BAG {
    void bag::insert( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        for( u16 i = _startIdx[ p_bagType ]; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ i ].first == p_itemId ) {
                _items[ i ].second += p_cnt;
                return;
            }
        if( _nextFree[ p_bagType ] >= MAX_ITEMS_IN_BAG
            || _nextFree[ p_bagType ] == _startIdx[ 1 + p_bagType ] ) // Insert failed.
            return;
        _items[ _nextFree[ p_bagType ]++ ] = {p_itemId, p_cnt};
    }

    void bag::erase( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        for( u16 i = _startIdx[ p_bagType ]; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ i ].first == p_itemId ) {
                if( p_cnt <= _items[ i ].second )
                    _items[ i ].second -= p_cnt;
                else
                    _items[ i ].second = 0;
                if( !_items[ i ].second ) {
                    for( u16 j = i; j < _nextFree[ p_bagType ]; ++j ) _items[ j ] = _items[ j + 1 ];
                    --_nextFree[ p_bagType ];
                }
                return;
            }
    }

    u16 bag::count( bagType p_bagType, u16 p_itemId ) {
        for( u16 i = _startIdx[ p_bagType ]; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ i ].first == p_itemId ) return _items[ i ].second;
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
    const std::pair<u16, u16>* bag::cbegin( bagType p_bagType ) const {
        return _items + _startIdx[ p_bagType ];
    }
    std::pair<u16, u16>* bag::end( bagType p_bagType ) {
        return _items + _nextFree[ p_bagType ];
    }
    const std::pair<u16, u16>* bag::cend( bagType p_bagType ) const {
        return _items + _nextFree[ p_bagType ];
    }

    void bag::clear( bagType p_bagType ) {
        memset( _items + _startIdx[ p_bagType ], 0,
                _startIdx[ p_bagType + 1 ] - _startIdx[ p_bagType ] + 1 );
        _nextFree[ p_bagType ] = _startIdx[ p_bagType ];
    }

    void bag::swap( bagType p_bagType, u16 p_idx1, u16 p_idx2 ) {
        std::swap( begin( p_bagType )[ p_idx1 ], begin( p_bagType )[ p_idx2 ] );
    }

} // namespace BAG
