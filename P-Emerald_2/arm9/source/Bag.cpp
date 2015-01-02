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

#include <algorithm>
#include <nds/ndstypes.h>

void bag::addItem( bagtype p_bagType, u16 p_itemId, u16 p_cnt ) {
    for( auto I = m_bags[ p_bagType ].begin( );
         I != m_bags[ p_bagType ].end( ); ++I ) {
        if( I->first == p_itemId ) {
            if( p_cnt + I->second <= MAXITEMCOUNT ) {
                I->second += p_cnt;
                return;
            }
            I->second = MAXITEMCOUNT;
            p_cnt -= MAXITEMCOUNT - I->second;
        }
    }
    m_bags[ p_bagType ].push_back( std::pair<u16, u16>( p_itemId, p_cnt ) );
    std::sort( m_bags[ p_bagType ].begin( ), m_bags[ p_bagType ].end( ) );
}

void bag::removeItem( bagtype p_bagType, u16 p_itemId, u16 p_cnt ) {
    for( auto I = m_bags[ p_bagType ].begin( );
         I != m_bags[ p_bagType ].end( ); ++I ) {
        if( I->first == p_itemId ) {
            u16 num = I->second;
            if( p_cnt && ( ( I->second - p_cnt ) > 0 ) ) {
                I->second -= p_cnt;
                std::sort( m_bags[ p_bagType ].begin( ), m_bags[ p_bagType ].end( ) );
                return;
            } else {
                m_bags[ p_bagType ].erase( I );
                p_cnt -= num;
                break;
            }
        }
    }
    std::sort( m_bags[ p_bagType ].begin( ), m_bags[ p_bagType ].end( ) );
}

u16 bag::countItem( bagtype p_bagType, u16 p_itemId ) {
    u16 cnt = 0;
    for( auto I = m_bags[ p_bagType ].begin( );
         I != m_bags[ p_bagType ].end( ); ++I )
         if( I->first == p_itemId )
             cnt += I->second;
    return cnt;
}

bool bag::empty( bagtype p_bagType ) {
    return m_bags[ p_bagType ].empty( );
}

std::size_t bag::size( bagtype p_bagType ) {
    return m_bags[ p_bagType ].size( );
}

std::pair<u16, u16> bag::elementAt( bagtype p_bagType, u16 index ) {
    return m_bags[ u16( p_bagType ) ][ index ];
}

void bag::clear( bagtype p_bagType ) {
    m_bags[ p_bagType ].clear( );
}
