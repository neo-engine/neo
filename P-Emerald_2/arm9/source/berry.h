/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : berry.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file (item.cpp) for details.

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

#include <string>
#include <nds.h>
#include "item.h"
#include "type.h"

class berry
    : public item {
public:
    enum berryGueteType {
        SUPER_SOFT = 0,
        VERY_SOFT = 1,
        SOFT = 2,
        HARD = 3,
        VERY_HARD = 4,
        SUPER_HARD = 5
    };

    enum berryTaste {
        SPICY,
        DRY,
        SWEET,
        BITTER,
        SOUR
    };

    //std::string dscrpt2;
    short           m_berrySize;
    berryGueteType  m_berryGuete;
    Type            m_naturalGiftType;  //Type that Natural Gift becomes when holding this berry
    u8   m_naturalGiftStrength;
    u8   m_berryTaste[ 5 ];       //Spicy, Dry, Sweet, Bitter, Sour
    u8   m_hoursPerGrowthStage,
        m_minBerries,
        m_maxBerries;

    std::string     getDescription2( bool p_new = false );

    short           getMass( );

    short           getSize( );

    berryGueteType  getGuete( );

    std::pair < Type, u8 >
        getNaturalGiftStats( );

    u8   getTaste( berryTaste p_taste );

    u8   getHoursPerGrowthStage( );

    u8   getMinBerries( );

    u8   getMaxBerries( );

    bool load( );

    berry( const std::string& p_name )
        : item( p_name ) {
        this->m_itemType = BERRIES;
    }
};