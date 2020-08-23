/*
Pokémon neo
------------------------------

file        : nav.h
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

#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#include "defines.h"
#include "uio.h"
#include "yesNoBox.h"

namespace NAV {
#define MAXBG    13
#define START_BG 0

    extern u8 LOCATION_TIMER;

    enum menuOption {
        VIEW_PARTY = 0,
        VIEW_DEX   = 1,
        VIEW_BAG   = 2,
        VIEW_ID    = 3,
        SAVE       = 4,
        SETTINGS   = 5,
    };

    struct backgroundSet {
        std::string           m_name;
        const unsigned int*   m_mainMenu;
        const unsigned short* m_mainMenuPal;
        bool                  m_loadFromRom;
        bool                  m_allowsOverlay;
    };

    /*
     * @brief: Shows a message box informing the player that he obtained the specified
     * item. (Also adds the specified item to the player's bag)
     */
    void giveItemToPlayer( u16 p_itemId, u16 p_amount = 1 );

    /*
     * @brief: Prints the given message. An empty message clears the message box.
     */
    void printMessage( const char* p_message, style p_style = MSG_NORMAL );

    inline void printMessage( const std::string& p_message, style p_style = MSG_NORMAL ) {
        printMessage( p_message.c_str( ), p_style );
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection = 255 );

    inline std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const std::string& p_message, style p_style, u8 p_selection = 255 ) {
        return printYNMessage( p_message.c_str( ), p_style, p_selection );
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const char* p_message, style p_style, const std::vector<u16>& p_choices,
                        u8 p_selection = 255 );

    inline std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const std::string& p_message, style p_style,
                        const std::vector<u16>& p_choices, u8 p_selection = 255 ) {
        return printChoiceMessage( p_message.c_str( ), p_style, p_choices, p_selection );
    }

    /*
     * @brief: Initializes the bottom screen with the main menu
     */
    void init( bool p_noPic = false, bool p_bottom = true );

    void handleInput( const char* p_path );

    void buyItem( const std::vector<std::pair<u16, u32>>& p_offeredItems, u8 p_paymentMethod );

    void showNewLocation( u16 p_newLocation );
    void hideLocation( u8 p_remTime = 0 );
} // namespace NAV
