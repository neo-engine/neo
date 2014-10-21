/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : bag.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Bag engine.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
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
