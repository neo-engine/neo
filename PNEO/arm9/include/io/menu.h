/*
Pokémon neo
------------------------------

file        : menu.h
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

#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#include "bag/item.h"
#include "defines.h"

namespace IO {
    constexpr u8 MAX_NAV_APPS = 3;

    enum menuOption {
        VIEW_PARTY = 0,
        VIEW_DEX   = 1,
        VIEW_BAG   = 2,
        VIEW_ID    = 3,
        SAVE       = 4,
        SETTINGS   = 5,

        NAV_APP_START = 10,
        NAV_APP_1     = 10,
        NAV_APP_2     = 11,
        NAV_APP_3     = 12,
    };

#ifdef DESQUID
    enum desquidMenuOption {
        DSQ_SPAWN_DEFAULT_TEAM  = 0,
        DSQ_SPAWN_DEFAULT_ITEMS = 1,
        DSQ_EDIT_FLAGS          = 2,
        DSQ_WARP                = 3,
        DSQ_TIME                = 4,
        DSQ_BATTLE_TRAINER      = 5,
    };
#endif

    extern bool NAV_NEEDS_REDRAW;

    /*
     * @brief: Checks which nav apps should be displayed
     */
    void recomputeNavApps( );

    void handleInput( const char* p_path );

#ifdef DESQUID
    void handleDesquidMenuSelection( desquidMenuOption p_selection, const char* p_path );
    void focusDesquidMenu( const char* p_path );
#endif

    void handleMenuSelection( menuOption p_selection, const char* p_path );
    void focusMenu( const char* p_path );

    s32 getItemCount( std::pair<u16, u32> p_item, const BAG::itemData& p_itemData,
                      const std::string& p_name, u8 p_paymentMethod );

    void buyItem( const std::vector<std::pair<u16, u32>>& p_offeredItems, u8 p_paymentMethod );

    void selectDaycarePkmn( u8 p_daycare, u8 p_selection );

    /*
     * @brief: Make the player select a pkmn from the (up to 2) currently stored in
     * daycare p_daycare. Also lists some basic stats and how many levels the pkmn have
     * grown.
     */
    u8 chooseDaycarePkmn( u8 p_daycare );

} // namespace IO
