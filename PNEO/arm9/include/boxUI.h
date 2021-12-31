/*
Pokémon neo
------------------------------

file        : boxUI.h
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
#include <vector>
#include "box.h"
#include "uio.h"

namespace BOX {
#define PAGE_ICON_START 5
    class boxUI {
      private:
        bool       _showTeam;
        u8         _currentSelection;
        boxPokemon _heldPkmn;
        u16        _outlineColor = 0xF4A0;

        void initTop( );
        void initSub( );

        /*
         * @brief: Draws some basic info of the pkmn to the top screen (clears screen
         * if p_pokemon is nullptr)
         */
        void drawPkmnInfoTop( pokemon* p_pokemon );

        /*
         * @brief: Updates the buttons for the given pkmn. (Hides all buttons if
         * p_pokemon is nullptr)
         */
        void drawPkmnInfoSub( boxPokemon* p_pokemon );
        void writeLineTop( const char* p_string, u8 p_line, u8 p_color = 252,
                           bool p_bottom = false );

      public:
        enum button {
            BUTTON_LEFT,
            BUTTON_RIGHT,
            BUTTON_BOX_NAME,
            BUTTON_BACK,
            BUTTON_PKMN,
            BUTTON_PARTY,
            BUTTON_MODE_MOVE,
            BUTTON_MODE_SELECT,
            BUTTON_PKMN_MOVE,
            BUTTON_PKMN_STATUS,
            BUTTON_PKMN_RELEASE,
            BUTTON_PKMN_GIVE_ITEM,
            BUTTON_PKMN_TAKE_ITEM,
            BUTTON_PKMN_CANCEL
        };

        /*
         * @brief: Sets the current movement mode (changes arrow and outline colors)
         */
        void setMode( u8 p_newMode );

        /*
         * @brief: Describes buttons in the UI.
         */
        struct interact {
            IO::inputTarget m_touch;
            button          m_buttonType;
            u8              m_param; // pkmn id
        };

        /*
         * @brief: returns the current buttons.
         */
        std::vector<interact> getInteractions( );

        /*
         * @brief: Initializes the box UI.
         */
        void init( );

        /*
         * @brief: Draw the specified box.
         */
        void draw( box* p_box );

        /*
         * @brief: Selects the given button.
         */
        void selectButton( button p_selectedButton, bool p_touched = false );

        /*
         * @brief: Selects the given pkmn; shows the corresponding buttons
         */
        void selectPkmn( boxPokemon* p_pokemon, u8 p_index, bool p_touched = false );

        /*
         * @brief: Shows the currently held pkmn at the specified index.
         */
        void updateHeldPkmn( u8 p_index );

        /*
         * @brief: Sets the given pkmn as the currently held pkmn.
         * (if p_pokemon is nullptr, the held pkmn is hidden)
         */
        void setNewHeldPkmn( boxPokemon* p_pokemon, u8 p_index );

        /*
         * @brief: Hovers over a pkmn.
         */
        void hoverPkmn( boxPokemon* p_pokemon, u8 p_index, bool p_redraw = true );

        /*
         * @brief: Shows the pkmn party window
         */
        void showParty( box* p_box, pokemon* p_party, u8 p_partyLen );

        /*
         * @brief: Hides the party window
         */
        void hideParty( box* p_box );

        /*
         * @brief: Replaces the pkmn at position p_index with the given pkmn.
         */
        void updatePkmn( boxPokemon* p_newPkmn, u8 p_index );
    };
} // namespace BOX
