/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : berry.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file (item.cpp) for details.

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

    struct berryData {
        u16             m_berrySize;
        berryGueteType  m_berryGuete;
        type            m_naturalGiftType;  //Type that Natural Gift becomes when holding this berry
        u8   m_naturalGiftStrength;
        u8   m_berryTaste[ 5 ];       //Spicy, Dry, Sweet, Bitter, Sour
        u8   m_hoursPerGrowthStage;
        u8   m_minBerries;
        u8   m_maxBerries;
    } m_berryData;

    bool load( ) override;

    berry( const std::string& p_name )
        : item( p_name ) {
        this->m_itemType = BERRIES;
    }
};