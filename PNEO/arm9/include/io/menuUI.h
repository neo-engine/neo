/*
Pokémon neo
------------------------------

file        : menuUI.h
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
#include "io/menu.h"
#include "io/uio.h"
#include "io/yesNoBox.h"

namespace IO {
    constexpr u8 NUM_CB_CHOICES = 6;

    extern const u16 ARR_X_SPR_PAL[ 16 ];

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

    /*
     * @brief: (Re-)Initializes the bottom screen
     */
    void init( bool p_noPic = false, bool p_bottom = true );

    void redraw( bool p_bottom = true );

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection = 255,
                    bool p_showMoney = false );

    inline std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const std::string& p_message, style p_style, u8 p_selection = 255,
                    bool p_showMoney = false ) {
        return printYNMessage( p_message.c_str( ), p_style, p_selection, p_showMoney );
    }

    std::vector<std::pair<IO::inputTarget, u8>> printChoiceMessage( const char* p_message,
                                                                    style p_style, u16 p_moves[ 4 ],
                                                                    u16 p_extraMove,
                                                                    u8  p_selection = 255 );
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

    std::vector<std::pair<IO::inputTarget, menuOption>> getTouchPositions( bool p_bottom = true );
    std::vector<std::pair<IO::inputTarget, u8>>         drawMenu( );

    void selectMenuItem( u8 p_selection );

    std::vector<std::pair<IO::inputTarget, u8>>
    drawItemChoice( const std::vector<std::pair<u16, u32>>& p_offeredItems,
                    const std::vector<std::string>&         p_itemNames,
                    const std::vector<BAG::itemData>& p_data, u8 p_paymentMethod, u8 p_firstItem );

    void selectItem( std::pair<u16, u32> p_item, const BAG::itemData& p_itemData,
                     const std::string& p_descr, u8 p_selection );

    std::vector<std::pair<IO::inputTarget, u8>> drawDaycareChoice( u8 p_daycare );
} // namespace IO
