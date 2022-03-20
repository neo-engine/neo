/*
Pokémon neo
------------------------------

file        : bagUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include "bag/bag.h"
#include "io/uio.h"
#include "io/yesNoBox.h"

namespace BAG {
    constexpr u8 GIVE_ITEM = 1;
    constexpr u8 TAKE_ITEM = 2;
    constexpr u8 MOVE_ITEM = 3;
    constexpr u8 MOVE_BAG  = 4;

    constexpr u8 BAG_CNT = 5;

    constexpr u8 MAX_ITEMS_PER_PAGE = 8;
    class bagUI {
      private:
        pokemon* _playerTeam;

        u8           _selectedIdx = 255;
        bag::bagType _currentPage;

        std::pair<u16, std::string> _teamItemCache[ 6 ];
        std::pair<u16, std::string> _itemCache[ MAX_ITEMS_PER_PAGE ];

        void drawPkmn( u16 p_itemId, const itemData* p_data );

        u8 _lastPkmnItemType = 255; // itemtype of the last item for which the pkmn info was drawn

        void drawItemSub( u16 p_itemId, const itemData* p_data, u16 p_idx );

      public:
        bagUI( pokemon* p_playerTeam ) : _playerTeam( p_playerTeam ) {
        }

        u16 drawPkmnIcons( );

        /*
         * @brief: Returns positions and types of all buttons currently visible on the
         * screen.
         */
        std::vector<std::pair<IO::inputTarget, u8>> getTouchPositions( );

        /*
         * @brief: Initializes the bag UI. Destroys anything on the screens.
         */
        void init( );

        /*
         * @brief: Redraws the specified page and displays the given items.
         */
        void drawBagPage( bag::bagType,
                          const std::vector<std::pair<std::pair<u16, u16>, itemData>>& p_items,
                          u8 p_selection = 0 );

        /*
         * @brief: Selects and highlights the specified item.
         */
        void selectItem( u8 p_idx, std::pair<u16, u16> p_item, const itemData* p_data );

        /*
         * @brief: Draws a counter with the specified message. Moves spirtes for choice
         * box item 0
         */
        std::vector<std::pair<IO::inputTarget, s32>> drawCounter( u16 p_message, s32 p_min,
                                                                  s32 p_max );

        /*
         * @brief: Updates the value and/or selected digit of a couter that is currently being
         * shown.
         */
        void updateCounter( s32 p_newValue, u8 p_selectedDigit, u8 p_numDigs );

        /*
         * @brief: Highlights the button a user is currently hovering over.
         */
        void hoverCounterButton( s32 p_min, s32 p_max, s32 p_button );

        /*
         * @brief: Restores any changes done to sprite positions done by drawing a
         * counter.
         */
        void destroyCounter( );

        /*
         * @brief: Draws a choiceBox for the specified item.
         */
        std::vector<std::pair<IO::inputTarget, u8>> drawChoice( u16 p_item, const itemData* p_data,
                                                                const std::vector<u16>& p_texts );

        /*
         * @brief: Draws a choice box for the moves of the pokemon.
         */
        std::vector<std::pair<IO::inputTarget, u8>> drawMoveChoice( const boxPokemon* p_pokemon,
                                                                    u16 p_extraMove = 0 );

        /*
         * @brief: Returns inputTargets for each teamPkmn
         */
        std::vector<std::pair<IO::inputTarget, u8>> getPkmnInputTarget( ) const;

        /*
         * @brief: Returns an input target for the specified button.
         */
        std::pair<IO::inputTarget, u8> getButtonInputTarget( u8 p_button ) const;

        void drawPkmnChoice( );

        void undrawPkmnChoice( );

        /*
         * @brief: Selects the specified pkmn.
         */
        void selectPkmn( u8 p_selection );

        /*
         * @brief: Draws a yesNoBox.
         */
        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
        printYNMessage( const char* p_message, u8 p_selection, bool p_bottom = true );

        /*
         * @brief: Prints the specified message.
         */
        void printMessage( const char* p_message );

        /*
         * @brief: Selects the specified choice in the currently displayed choiceBox.
         */
        void selectChoice( u8 p_selection );

        /*
         * @brief: Selects a move choice.
         */
        void selectMoveChoice( u8 p_selection );

        /*
         * @brief: Attaches the sprite corresponding to the specified item to the player's
         * touch input.
         */
        bool getSprite( u8 p_idx, std::pair<u16, u16> p_item, const itemData* p_data );

        /*
         * @brief: Updates the current position of the sprite at hand.
         */
        void updateSprite( touchPosition p_touch );

        /*
         * @brief: Removes any sprite attached from the player's touch input.
         */
        void dropSprite( u8 p_idx, std::pair<u16, u16> p_item, const itemData* p_data );
    };
} // namespace BAG
