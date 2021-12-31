/*
Pokémon neo
------------------------------

file        : nav.h
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
#include "uio.h"
#include "yesNoBox.h"

namespace NAV {

    extern const u16 ARR_X_SPR_PAL[ 16 ];

    constexpr u8 MAX_NAV_APPS = 3;

    constexpr u16 SPR_MSGTEXT_OAM = 108;
    constexpr u16 SPR_MSGCONT_OAM = 112;
    constexpr u16 SPR_MSGBOX_OAM  = 113;

    constexpr u16 SPR_MSG_GFX     = 348;
    constexpr u16 SPR_MSG_EXT_GFX = 220;
    constexpr u16 SPR_MSGBOX_GFX  = 476;
    constexpr u16 SPR_MSGCONT_GFX = 508;

    constexpr u16 SPR_MENU_OAM_SUB( u16 p_idx ) {
        return ( 0 + ( p_idx ) );
    }
    constexpr u16 SPR_ITEM_OAM_SUB( u16 p_i ) {
        return ( 0 + ( p_i ) );
    }
    constexpr u16 SPR_MENU_SEL_OAM_SUB = 6;
    constexpr u16 SPR_CHOICE_START_OAM_SUB( u16 p_pos ) {
        return ( 7 + 8 * ( p_pos ) );
    }
    constexpr u16 SPR_X_OAM_SUB = 56;
    constexpr u16 SPR_ARROW_UP_OAM_SUB( u16 p_i ) {
        return ( 57 + ( p_i ) );
    }
    constexpr u16 SPR_ARROW_DOWN_OAM_SUB( u16 p_i ) {
        return ( 63 + ( p_i ) );
    }
    constexpr u16 SPR_MSGBOX_OAM_SUB = 70;
    constexpr u16 SPR_NAV_APP_ICON_SUB( u16 p_i ) {
        return ( 80 + ( p_i ) );
    }
    constexpr u16 SPR_PAGE_BG_OAM_SUB = 84;
    constexpr u16 SPR_NAV_APP_RSV_SUB = 90;

    constexpr u16 SPR_MENU_PAL_SUB( u16 p_idx ) {
        return ( 0 + ( p_idx ) );
    }
    constexpr u16 SPR_ITEM_PAL_SUB( u16 p_idx ) {
        return ( 0 + ( p_idx ) );
    }
    constexpr u16 SPR_MENU_SEL_PAL_SUB = 6;
    constexpr u16 SPR_BOX_PAL_SUB      = 7;
    constexpr u16 SPR_BOX_SEL_PAL_SUB  = 8;
    constexpr u16 SPR_X_PAL_SUB        = 9;
    constexpr u16 SPR_MSGBOX_PAL_SUB   = 10;
    constexpr u16 SPR_NAV_APP_ICON_PAL_SUB( u16 p_i ) {
        return ( 11 + ( p_i ) );
    }
    constexpr u16 SPR_NAV_APP_RSV1_PAL_SUB = 14;
    constexpr u16 SPR_NAV_APP_RSV2_PAL_SUB = 15;

    extern u8  LOCATION_TIMER;
    extern u16 TEXT_BUF[ 64 * 256 ];

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

    struct backgroundSet {
        std::string           m_name;
        const unsigned int*   m_mainMenu;
        const unsigned short* m_mainMenuPal;
        bool                  m_loadFromRom;
        bool                  m_allowsOverlay;
    };

    /*
     * @brief: Shows a message box informing the player that they obtained the specified
     * item. (Also adds the specified item to the player's bag)
     */
    void giveItemToPlayer( u16 p_itemId, u16 p_amount = 1 );

    /*
     * @brief: Shows a message informing the player that they lost the specified item.
     * (also removes the specified item from the bag)
     */
    void takeItemFromPlayer( u16 p_itemId, u16 p_amount = 1 );

    /*
     * @brief: Shows a message informing the player that they lost the specified item
     * because the item was used.
     * (also removes the specified item from the bag)
     */
    void useItemFromPlayer( u16 p_itemId, u16 p_amount = 1 );

    /*
     * @brief: Prints the given message. An empty message clears the message box.
     */
    void printMessage( const char* p_message, style p_style = MSG_NORMAL, bool p_noDelay = false );

    inline void printMessage( const std::string& p_message, style p_style = MSG_NORMAL,
                              bool p_noDelay = false ) {
        printMessage( p_message.c_str( ), p_style, p_noDelay );
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection = 255,
                    bool p_showMoney = false );

    inline std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const std::string& p_message, style p_style, u8 p_selection = 255,
                    bool p_showMoney = false ) {
        return printYNMessage( p_message.c_str( ), p_style, p_selection, p_showMoney );
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const char* p_message, style p_style, const std::vector<u16>& p_choices,
                        u8 p_selection = 255 );

    inline std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const std::string& p_message, style p_style,
                        const std::vector<u16>& p_choices, u8 p_selection = 255 ) {
        return printChoiceMessage( p_message.c_str( ), p_style, p_choices, p_selection );
    }

    std::vector<std::pair<IO::inputTarget, s32>> drawCounter( s32 p_min, s32 p_max );
    void updateCounterValue( s32 p_newValue, u8 p_selectedDigit, u8 p_numDigs );
    void hoverCounterButton( s32 p_min, s32 p_max, s32 p_button );

    /*
     * @brief: Initializes the bottom screen with the main menu
     */
    void init( bool p_noPic = false, bool p_bottom = true );

    void handleInput( const char* p_path );

    void buyItem( const std::vector<std::pair<u16, u32>>& p_offeredItems, u8 p_paymentMethod );

    /*
     * @brief: Make the player select a pkmn from the (up to 2) currently stored in
     * daycare p_daycare. Also lists some basic stats and how many levels the pkmn have
     * grown.
     */
    u8 chooseDaycarePkmn( u8 p_daycare );

    void showNewLocation( u16 p_newLocation );
    void hideLocation( u8 p_remTime = 0 );
} // namespace NAV
