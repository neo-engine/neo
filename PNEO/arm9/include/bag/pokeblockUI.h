/*
Pokémon neo
------------------------------

file        : pokeblockUI.h
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
#include <string>
#include <nds.h>

#include "bag/bag.h"
#include "io/uio.h"
#include "io/yesNoBox.h"

namespace BAG {
    constexpr u8 BLOCKS_PER_PAGE = 12;
    constexpr u8 BLOCKS_PER_ROW  = 3;
    class pokeblockUI {
      private:
        pokemon* _playerTeam;

      public:
        pokeblockUI( pokemon* p_playerTeam ) : _playerTeam( p_playerTeam ) {
        }

        /*
         * @brief: Returns positions and types of all buttons currently visible on the
         * screen.
         */
        std::vector<std::pair<IO::inputTarget, u8>> getTouchPositions( );

        void init( );

        void initBlockView( );

        void initPkmnView( );

        void animateFeedBlockToPkmn( u8 p_pkmnIdx, u8 p_blockType );

        void selectBlock( u8 p_blockType );

        void selectPkmn( u8 p_pkmnIdx, u8 p_blockType );
    };
} // namespace BAG
