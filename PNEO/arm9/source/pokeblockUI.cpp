/*
Pokémon neo
------------------------------

file        : pokeblockUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2023 - 2023
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

#include <cstdio>

#include "bag/pokeblockUI.h"
#include "bag/pokeblockViewer.h"
#include "defines.h"
#include "fs/data.h"
#include "io/screenFade.h"
#include "io/strings.h"

// Sprites
#include "NoItem.h"

namespace BAG {
    constexpr u8 MAX_PARTY_PKMN          = SAVE::NUM_PARTY_SLOTS;
    constexpr u8 SPRITES_PER_CB_WINDOW   = 6;
    constexpr u8 NUM_CB_CHOICES          = 6;
    constexpr u8 SPRITES_PER_ITEM_WINDOW = 5;

    constexpr u8 SPR_TRANSFER_OAM_SUB = 0;
    constexpr u8 SPR_MSG_BOX_OAM_SUB  = 1;
    constexpr u8 SPR_CHOICE_START_OAM_SUB( u8 p_pos ) {
        return 14 + SPRITES_PER_CB_WINDOW * p_pos;
    }
    constexpr u8 SPR_ARROW_DOWN_OAM_SUB = 52;
    constexpr u8 SPR_ARROW_BACK_OAM_SUB = 53;
    constexpr u8 SPR_ARROW_UP_OAM_SUB   = 54;
    constexpr u8 SPR_ITEM_WINDOW_OAM_SUB( u8 p_window ) {
        return 55 + SPRITES_PER_ITEM_WINDOW * p_window;
    }
    constexpr u8 SPR_PKMN_START_OAM_SUB    = 100;
    constexpr u8 SPR_MSG_PKMN_SEL_OAM_SUB  = 106;
    constexpr u8 SPR_MSG_BOX_SMALL_OAM_SUB = 115;
    constexpr u8 SPR_BAG_ICON_SEL_OAM_SUB  = 120;
    constexpr u8 SPR_BAG_ICON_OAM_SUB      = 121;

    constexpr u8 SPR_BOX_PAL_SUB          = 0;
    constexpr u8 SPR_SELECTED_PAL_SUB     = 1;
    constexpr u8 SPR_PKMN_PAL_SUB         = 2;
    constexpr u8 SPR_BAG_ICON_PAL_SUB     = 8;
    constexpr u8 SPR_BAG_ICON_SEL_PAL_SUB = 9;
    // constexpr u8 SPR_ITEM_ICON_PAL_SUB    = 10;
    constexpr u8 SPR_ARROW_X_PAL_SUB  = 11;
    constexpr u8 SPR_BACK_PAL_SUB     = 12;
    constexpr u8 SPR_DOWN_PAL_SUB     = 13;
    constexpr u8 SPR_TRANSFER_PAL_SUB = 15;
    // constexpr u8 SPR_TYPE_PAL_SUB( u8 p_type ) {
    //     return 10 + ( p_type )
    // }

    u16 pokeblockUI::drawPkmnIcons( ) {
        return 0;
    }

    std::vector<std::pair<IO::inputTarget, u8>> pokeblockUI::getTouchPositions( ) {
        return { };
    }

    void pokeblockUI::init( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::initOAMTable( false );
        // Don't init anything else for the top screen here
        IO::initOAMTable( true );
        IO::vramSetup( );
        swiWaitForVBlank( );
        IO::clearScreen( true, true );
        IO::resetScale( true, true );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        u16 tileCnt = drawPkmnIcons( );

        tileCnt = IO::loadSprite( "BG/Back", SPR_ARROW_BACK_OAM_SUB, SPR_BACK_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 26, 32, 32, false, false,
                                  false, OBJPRIORITY_3, true );

        tileCnt = IO::loadSprite( "BG/Down", SPR_ARROW_DOWN_OAM_SUB, SPR_DOWN_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 60, SCREEN_HEIGHT - 26, 32, 32, false, false,
                                  false, OBJPRIORITY_3, true );
        tileCnt = IO::loadSprite( "BG/Up", SPR_ARROW_UP_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26, 32, 32, false, false,
                                  false, OBJPRIORITY_3, true );
    }

    void pokeblockUI::initBlockView( ) {
    }

    void pokeblockUI::initPkmnView( ) {
    }

    void pokeblockUI::animateFeedBlockToPkmn( u8 p_pkmnIdx, u8 p_blockType ) {
    }

    void pokeblockUI::selectBlock( u8 p_blockType ) {
    }

    void pokeblockUI::selectPkmn( u8 p_pkmnIdx, u8 p_blockType ) {
    }
} // namespace BAG
