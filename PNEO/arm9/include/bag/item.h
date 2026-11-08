/*
    Pokémon neo
    ------------------------------

    file        : item.h
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
#include <functional>
#include <string>
#include "gen/ballMap.h"
#include "gen/berryMap.h"
#include "gen/itemNames.h"
#include "pokemon.h"

namespace BAG {
    constexpr u8 ITEMTYPE_POKEBALL    = 1;
    constexpr u8 ITEMTYPE_MEDICINE    = 2;
    constexpr u8 ITEMTYPE_BATTLEITEM  = 3;
    constexpr u8 ITEMTYPE_COLLECTIBLE = 4;
    constexpr u8 ITEMTYPE_USABLE      = 5;
    constexpr u8 ITEMTYPE_EVOLUTION   = 6;
    constexpr u8 ITEMTYPE_FORMECHANGE = 8;
    constexpr u8 ITEMTYPE_KEYITEM     = 9;
    constexpr u8 ITEMTYPE_TM          = 12;
    constexpr u8 ITEMTYPE_APRICORN    = 13;

    constexpr u8 ITEMTYPE_NONFLAG = 15;

    constexpr u8 ITEMTYPE_BERRY = 16;
    constexpr u8 ITEMTYPE_HOLD  = 32;

    struct itemData {
        u8  m_itemType;
        u8  m_effect; // Effect index
        u16 m_param1; // Effect param 1
        u16 m_param2; // Effect param 2
        u16 m_param3; // Effect param 3
        u16 m_sellPrice;
        u16 m_buyPrice;
    };

    constexpr u8 BERRY_STAGES = 4;

    /*
     * @brief: Compute itemtype character.
     */
    constexpr u16 getItemChar( const u8 p_itemType ) {
        if( p_itemType & ITEMTYPE_BERRY ) { return 472; }

        auto itemType = ( p_itemType & ITEMTYPE_NONFLAG );

        switch( itemType ) {
        case ITEMTYPE_POKEBALL: return 473;
        case ITEMTYPE_BATTLEITEM: return 474;
        case ITEMTYPE_COLLECTIBLE:
        case ITEMTYPE_USABLE:
        case ITEMTYPE_EVOLUTION: return 467;
        case ITEMTYPE_MEDICINE: return 471;
        case ITEMTYPE_FORMECHANGE:
        case ITEMTYPE_KEYITEM: return 468;
        case ITEMTYPE_TM: return 469;
        default: return 467;
        }
    }

    /*
     * @brief: Returns true if using the specified item opens a new submenu/interface that
     * destroys screen content.
     */
    constexpr bool hasInterface( u16 p_itemId ) {
        switch( p_itemId ) {
        default: return false;
        case I_POKEBLOCK_CASE: return true;
        }
    }

    bool isUsable( u16 p_itemId );

    /*
     * @brief: Uses a usable item/ key item. Returns false if the item needs to be used in
     * the OW.
     */
    bool use( u16 p_itemId, std::function<void( const char* )> p_message, bool p_dryRun = false );

    /*
     * @brief: Use specified item on given pokemon. (Item needs to be either medicine,
     * formeChange).
     */
    bool use( u16 p_itemId, const itemData& p_data, pokemon& p_pokemon,
              std::function<u8( u8 )> p_callback, bool p_inbattle = false );
} // namespace BAG
