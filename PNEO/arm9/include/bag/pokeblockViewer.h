/*
Pokémon neo
------------------------------

file        : pokeblockViewer.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#pragma once
#include <nds.h>

#include "bag/pokeblock.h"
#include "bag/pokeblockUI.h"
#include "pokemon.h"

namespace BAG {
    /*
     * View 1 (default):
     *  top: pokeblock case, description of selected block
     *  bottom: pkmn overview, 3x4 pkblk types on 2 pages, message/button "Feed"
     *
     * View 2 (select pkmn)
     *  top: pokeblock case, description of selected block
     *  bottom: pkmn overview, status condition overview
     *
     * View 3 (feeding animation, top only, ones-shot)
     */
    class pokeblockViewer {
      private:
        pokeblockUI* _ui;

        u8 _view          = 0;
        u8 _selectedPkmn  = 0;
        u8 _selectedBlock = 0; // 0 - 11: page 1 of blocks, 12 - 23 page 2

        pokemon* _playerTeam;
        u16*     _pokeblockCount;

        void selectView( u8 p_view );

        void selectBlock( u8 p_blockIdx );

        void selectPkmn( u8 p_pkmnIdx );

        /*
         * @brief: Handles all touch input related things.
         * @returns: 1 if the player pressed back.; 2 if the player pressed A
         */
        u8 handleTouch( );

      public:
        /*
         * @brief: Initializes a new pokeblockViewer.
         * @param p_playerTeam: Player's party which is to be displayed in the bag.
         */
        pokeblockViewer( pokemon* p_playerTeam, u16* p_pokeblockCount );

        ~pokeblockViewer( ) {
            delete _ui;
        }

        /*
         * @brief: Runs the pokeblock viewer.
         */
        void run( );
    };
} // namespace BAG
