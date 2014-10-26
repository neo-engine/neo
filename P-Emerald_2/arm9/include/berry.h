/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : berry.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file (item.cpp) for details.

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

#pragma once

#include <string>
#include <nds.h>
#include "item.h"
#include "type.h"

namespace ITEMS {
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
}

void initLocations( );