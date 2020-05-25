/*
Pokémon neo
------------------------------

file        : boxViewer.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2020
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

#include "box.h"
#include "boxUI.h"
#include "statusScreenUI.h"

namespace BOX {
    class boxViewer {
      private:
        std::vector<IO::inputTarget> _ranges;
        u8                           _selectedIdx;
        std::pair<u8, u8>            _heldPokmPos; //(box, pos in box); (_, 19..24) for team pkmn
        pokemon                      _heldPkmn;
        bool                         _showTeam;
        bool                         _topScreenDirty;
        u8                           _curPage;
        boxUI                        _boxUI;

        void select( u8 p_index );
        void takePkmn( u8 p_index );

        bool updateTeam( );

      public:
        boxViewer( ) {
        }
        ~boxViewer( ) {
        }
        void run( bool p_allowTakePkmn = false );
    };
} // namespace BOX