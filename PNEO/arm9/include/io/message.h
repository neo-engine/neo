/*
Pokémon neo
------------------------------

file        : message.h
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

#include "defines.h"
#include "io/uio.h"

namespace IO {
    constexpr u16 SPR_MSGTEXT_OAM     = 108;
    constexpr u16 SPR_MSGTEXT_OAM_LEN = 4;
    constexpr u16 SPR_MSGCONT_OAM     = 112;
    constexpr u16 SPR_MSGCONT_OAM_LEN = 1;
    constexpr u16 SPR_MSGBOX_OAM      = 113;
    constexpr u16 SPR_MSGBOX_OAM_LEN  = 14;

    constexpr u16 SPR_MSG_GFX     = 348;
    constexpr u16 SPR_MSG_EXT_GFX = 220;
    constexpr u16 SPR_MSGBOX_GFX  = 476;
    constexpr u16 SPR_MSGCONT_GFX = 508;

    extern u8  LOCATION_TIMER;
    extern u16 TEXT_BUF[ 64 * 256 ];

    void hideMessageBox( );

    void animateMB( u8 p_frame );

    void waitForInteract( );

    void doPrintMessage( const char* p_message, style p_style, u16 p_item = 0,
                         const BAG::itemData* p_data = 0, bool p_noDelay = false );

    /*
     * @brief: Prints the given message. An empty message clears the message box.
     */
    void printMessage( const char* p_message, style p_style, bool p_noDelay = false );

    inline void printMessage( const std::string& p_message, style p_style = MSG_NORMAL,
                              bool p_noDelay = false ) {
        printMessage( p_message.c_str( ), p_style, p_noDelay );
    }

    inline void printMessage( const char* p_message ) {
        printMessage( p_message, MSG_NORMAL, false );
    }

    /*
     * @brief: Shows a message informing the player that they lost the specified item
     * because the item was used.
     * (also removes the specified item from the bag)
     */
    void useItemFromPlayer( u16 p_itemId, u16 p_amount = 1 );

    /*
     * @brief: Shows a message informing the player that they lost the specified item.
     * (also removes the specified item from the bag)
     */
    void takeItemFromPlayer( u16 p_itemId, u16 p_amount = 1 );

    /*
     * @brief: Shows a message box informing the player that they obtained the specified
     * item. (Also adds the specified item to the player's bag)
     */
    void giveItemToPlayer( u16 p_itemId, u16 p_amount = 1 );

    void showNewLocation( u16 p_newLocation, bool );
    void hideLocation( u8 p_remTime = 0 );
} // namespace IO
