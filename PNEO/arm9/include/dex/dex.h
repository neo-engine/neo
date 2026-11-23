/*
Pokémon neo
------------------------------

file        : dex.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2023
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

#include <nds.h>
#include "dex/dexUI.h"
#include "gen/localDex.h"
#include "gen/pokemonNames.h"

namespace DEX {
    u16          nextEntry( u16 p_current );
    u16          previousEntry( u16 p_current );
    constexpr u8 MAX_PAGES = 3;

    /*
     * @brief: Returns the number of the specified pkmn in the local pokédex. Returns
     * u16(-1) if the specified pkmn is not found in the local pokédex
     */
    constexpr u16 getDexNo( u16 p_pkmnIdx ) {
        for( u16 i = 0; i < LOCAL_DEX_SIZE; ++i ) {
            if( LOCAL_DEX[ i ] == p_pkmnIdx ) { return i + 1; }
        }
        return u16( -1 );
    }

    constexpr bool requiredForCompletion( u16 p_pkmnIdx, bool p_localDex = false ) {
        if( isMythical( p_pkmnIdx ) ) { return false; }
        if( p_pkmnIdx == PKMN_ORICORIO ) { return !p_localDex; }
        return true;
    }

    /*
     * @brief: returns the smallest index no of a pkmn in the nat dex that the player has
     * seen or caught.
     */
    u16 getFirstPkmnInNatDex( );

    /*
     * @brief: returns the highest index no of a pkmn in the nat dex that the player has
     * seen or caught.
     */
    u16 getLastPkmnInNatDex( );

    /*
     * @brief: returns the smallest index no of a pkmn in the local dex that the player has
     * seen or caught.
     */
    u16 getFirstPkmnInLocDex( );

    /*
     * @brief: returns the highest index no of a pkmn in the local dex that the player has
     * seen or caught.
     */
    u16 getLastPkmnInLocDex( );

    /*
     * @brief: Returns the pair (page, slot) containing the pkmn specified by p_natIdx or
     * (0, 255) if no such pair exists.
     */
    constexpr std::pair<u16, u8> getLocSlotForNat( u16 p_natIdx ) {
        for( u16 i = 0; i < MAX_LOCAL_DEX_PAGES - 1; ++i ) {
            for( u8 j = 0; j < MAX_LOCAL_DEX_SLOTS; ++j ) {
                if( LOCAL_DEX_PAGES[ i ][ j ] == p_natIdx ) { return { i, j }; }
            }
        }
        return { 0, 255 };
    }

    class dex {
      public:
        enum mode { LOCAL_DEX = 0, NATIONAL_DEX = 1, SHOW_SINGLE = 2 };

      private:
        dexUI* _dexUI;
        mode   _mode         = LOCAL_DEX;
        u8     _currentPage  = 0;
        u8     _currentForme = 0; // only available for the national dex mode

        u16                _natDexUB;
        u16                _natDexLB;
        u16                _locDexUB;
        u16                _locDexLB;
        std::pair<u16, u8> _locFirstPage;
        std::pair<u16, u8> _locLastPage;

        /*
         * @brief: Swaps between the local/national dex modes.
         * @param p_startIdx: pkmn to highlight in the new mode.
         */
        void changeMode( mode p_newMode, u16 p_startIdx );

        /*
         * @brief: Select an entry while in the national dex mode.
         */
        void selectNational( u16 p_pkmnIdx, bool p_forceDraw = false, u8 p_forme = 0,
                             bool p_shiny = false, bool p_female = false );

        /*
         * @brief: Select an entry while in the local dex mode.
         */
        void selectLocal( u16 p_page, u8 p_slot, s8 p_dir, bool p_forceDraw = false, u8 p_forme = 0,
                          bool p_shiny = false, bool p_female = false );

        /*
         * @brief: Run dex welcome screen which shows the progress and an initial dex mode
         * selection (local/national), if the national dex is already unlocked.
         * @returns: true if the player wants to exit the dex.
         */
        bool runModeChoice( );

        /*
         * @brief: Runs the main logic of the dex, handles mode switching and selecting
         * different pkmn/pages.
         */
        void runDex( );

      public:
        dex( );
        ~dex( ) {
            if( _dexUI ) { delete _dexUI; }
        }

        /*
         * @brief: Runs the dex.
         */
        void run( );

        /*
         * @brief: Shows a single dex entry of the specified pkmn, no navigation, formes
         * or habitats. Displays the national dex number iff the player has unlocked the
         * national dex (local dex no otherwise).
         */
        void run( u16 p_pkmnIdx, u8 p_forme = 0, bool p_shiny = false, bool p_female = false );
    };
} // namespace DEX
