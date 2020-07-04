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

#include "uio.h"
#include "defines.h"
#include "yesNoBox.h"

namespace NAV {
#define MAXBG 13
#define START_BG 0

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
     * @brief: Prints the given message. An empty message clears the message box.
     */
    void printMessage( const char* p_message, style p_style = MSG_NORMAL );

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection = 255 );

    std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const char* p_message, style p_style, const std::vector<u16>& p_choices,
                        u8 p_selection = 255 );

    /*
     * @brief: Initializes the bottom screen with the main menu
     */
    void init( bool p_bottom = true );

    //    void draw( bool p_initMainSprites = false, u8 p_newIdx = (u8) 255 );
    //    void showNewMap( u8 p_bank );
    //    void updateMap( u16 p_map );
    void handleInput( const char* p_path );
    //    void home( );

    //    extern backgroundSet BGs[ MAXBG ];
} // namespace NAV
