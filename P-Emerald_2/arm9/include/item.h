/*
    Pokémon neo
    ------------------------------

    file        : item.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

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

#pragma once
#include <functional>
#include <string>
#include "pokemon.h"

namespace ITEM {
    const u8 ITEMTYPE_POKEBALL    = 1;
    const u8 ITEMTYPE_MEDICINE    = 2;
    const u8 ITEMTYPE_BATTLEITEM  = 3;
    const u8 ITEMTYPE_COLLECTIBLE = 4;
    const u8 ITEMTYPE_USABLE      = 5;
    const u8 ITEMTYPE_EVOLUTION   = 6;
    const u8 ITEMTYPE_FORMECHANGE = 8;
    const u8 ITEMTYPE_KEYITEM     = 9;
    const u8 ITEMTYPE_TM          = 12;
    const u8 ITEMTYPE_APRICORN    = 13;
    const u8 ITEMTYPE_BERRY       = 16;
    const u8 ITEMTYPE_HOLD        = 32;

    struct itemData {
        u8  m_itemType;
        u8  m_effect; // Effect index
        u16 m_param1; // Effect param 1
        u16 m_param2; // Effect param 2
        u16 m_param3; // Effect param 3
        u16 m_sellPrice;
        u16 m_buyPrice;
    };

    /*
     * @brief: Compute itemtype character.
     */
    constexpr u16 getItemChar( const u8 p_itemType ) {
        if( p_itemType & ITEMTYPE_BERRY ) { return 473; }

        auto itemType = ( p_itemType & 15 );

        switch( itemType ) {
        case ITEMTYPE_POKEBALL:
            return 474;
        case ITEMTYPE_BATTLEITEM:
            return 475;
        case ITEMTYPE_COLLECTIBLE:
        case ITEMTYPE_USABLE:
        case ITEMTYPE_EVOLUTION:
            return 468;
        case ITEMTYPE_MEDICINE:
            return 472;
        case ITEMTYPE_FORMECHANGE:
        case ITEMTYPE_KEYITEM:
            return 469;
        case ITEMTYPE_TM:
            return 470;
        default:
            return ' ';
        }
    }

    bool        getItemName( const u16 p_itemId, const u8 p_language, char* p_out );
    std::string getItemName( const u16 p_itemId, const u8 p_language );
    std::string getItemName( const u16 p_itemId );
    itemData    getItemData( const u16 p_itemId );
    bool        getItemData( const u16 p_itemId, itemData* p_out );

    bool isUsable( const u16 p_itemId );

    /*
     * @brief: Uses a usable item/ key item.
     */
    bool use( const u16 p_itemId, const bool p_dryRun = false );

    /*
     * @brief: Use specified item on given pokemon. (Item needs to be either medicine,
     * formeChange).
     */
    bool use( const u16 p_itemId, const itemData& p_data, pokemon& p_pokemon,
              std::function<u8( u8 )> p_callback, bool p_inbattle = false );
} // namespace ITEM
