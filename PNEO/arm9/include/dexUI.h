/*
Pokémon neo
------------------------------

file        : dexUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2021
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
#include <nds.h>

#include "uio.h"

namespace DEX {
    class dexUI {
      private:
        std::vector<std::pair<IO::inputTarget, u8>> _touchPostiions;
        u8                                          _mode;

      public:
        /*
         * @brief: Creates a new dexUI. Does not destroy any screens.
         */
        dexUI( ) {
        }

        /*
         * @brief: Initializes the general dex UI. Destroys everything on both screens.
         */
        void init( );

        /*
         * @brief: draws the dex mode choice. Destroys everything on both screens.
         */
        std::vector<std::pair<IO::inputTarget, u8>>
        drawModeChoice( bool p_showLocalDex = true, bool p_showNationalDex = false );

        /*
         * @brief: selects a mode during the mode selection phase.
         */
        void selectMode( u8 p_newMode );

        /*
         * @brief: initializes the specified mode for the main dex.
         */
        void changeMode( u8 p_newMode );

        /*
         * @brief: Prints the dex information.
         * @param p_page: 1-general information; 2-forms; 3-habitats
         */
        void drawPkmnInfo( u16 p_pkmnId, u8 p_page, u8 p_forme = 0, bool p_shiny = false,
                           bool p_female = false, bool p_bottom = false );

        /*
         * @brief: Draws the bottom screen of the national pokedex with the specified pkmn
         * selected. (bottom screen of the national dex is organized as a vertical list of
         * entries)
         */
        void selectNationalIndex( u16 p_pkmnIdx, bool p_bottom = true );

        /*
         * @brief: Draws the bottom screen of the local pokedex with the specified pkmn
         * selected. (bottom screen of the local dex is organized a horizontal list of
         * columns with up to 3 icons per column.)
         */
        void selectLocalPageSlot( u16 p_page, u8 p_slot, bool p_bottom = true );

        /*
         * @brief: Highlights the specified button or none if p_button is -1.
         */
        void highlightButton( u8 p_button = 255, bool p_bottom = true );

        /*
         * @brief: Returns information for the current buttons on the screen.
         */
        std::vector<std::pair<IO::inputTarget, u8>> getTouchPositions( bool p_bottom = true );
    };
} // namespace DEX
