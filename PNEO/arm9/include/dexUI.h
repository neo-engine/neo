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
        u8 _mode;

        u16 _nationalSelectedIdx;
        u8  _nationalOAMStart;

        u16 _localSelectedPage;

        std::vector<std::pair<IO::touchInputTarget, u16>> _touchPositions;
        std::pair<IO::touchInputTarget, u16>              _backChoice;

        /*
         * @brief: Loads the specified pkmn and its corresponding bg to the specified
         * position on-screen. Assumes that the gfx for the bg sprites are loaded already
         * via init.
         */
        void loadPkmnEntry( u16 p_pkmnIdx, u8 p_pkmnForme, u8 p_OAMstart, bool p_isHidden, u16 p_x,
                            u16 p_y, bool p_bottom = true );

        /*
         * @brief: Loads the (bottom screen) sprites for a single national dex entry into
         * the specified slot (relative to _nationalOAMStart).
         */
        void nationalLoadPkmnEntry( u16 p_pkmnIdx, u8 p_OAMslot, bool p_bottom );

        /*
         * @brief: Initializes and redraws the bottom screen in the national mode, entry
         * no. p_centerPkmnIdx will appear vertically centered.
         */
        void nationalInitSub( u16 p_centerPkmnIdx, u16 p_pkmnIdxUB = 0, bool p_bottom = true );

        /*
         * @brief: Rotate the national dex view one entry forward by shifting the
         * corresponding sprites onscreen, deleting non-visible sprites and preloading
         * required new sprites.
         */
        void nationalRotateForward( u16 p_pkmnIdxUB = 0, bool p_bottom = true );

        /*
         * @brief: Rotate the national dex view one entry backward by shifting the
         * corresponding sprites onscreen, deleting non-visible sprites and preloading
         * required new sprites.
         */
        void nationalRotateBackward( u16 p_pkmnIdxUB = 0, bool p_bottom = true );

        /*
         * @brief: draws a page of the local pokedex.
         */
        void localDrawPage( u16 p_page, u16 p_pageUB = 0, bool p_inverted = false,
                            bool p_bottom = true );

        /*
         * @brief: Initializes and redraws the bottom screen in the local dex mode.
         */
        void localInitSub( bool p_bottom = true );

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
         * @param p_pkmnIdxUB: Entries above this value will be hidden.
         */
        void nationalSelectIndex( u16 p_pkmnIdx, u16 p_pkmnIdxUB = 0, bool p_bottom = true,
                                  u8 p_forme = 0, bool p_shiny = false, bool p_female = false );

        /*
         * @brief: Draws the bottom screen of the local pokedex with the specified pkmn
         * selected. (bottom screen of the local dex is organized a horizontal list of
         * columns with up to 3 icons per column.)
         * @param p_pageUB: Pages above this value will be hidden.
         */
        void localSelectPageSlot( u16 p_page, u8 p_slot, u16 p_pageUB = 0, bool p_bottom = true,
                                  u8 p_forme = 0, bool p_shiny = false, bool p_female = false );

        /*
         * @brief: Returns information for the current buttons on the screen.
         */
        std::vector<std::pair<IO::touchInputTarget, u16>> getTouchPositions( u8 p_mode );
    };
} // namespace DEX
