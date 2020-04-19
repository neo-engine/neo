/*
Pokémon neo
------------------------------

file        : bagViewer.h
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
#include <functional>
#include <vector>
#include <nds.h>

#include "bag.h"
#include "bagUI.h"
#include "item.h"
#include "uio.h"

namespace BAG {
    class bagViewer {
      public:
        enum context {
            BATTLE,      // Allow only items usable in-battle
            GIVE_TO_PKMN // Allow only items, medicine and berries
        };

      private:
        std::function<bool( std::pair<u16, u16>, std::pair<u16, u16> )> _currCmp;

        std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> _ranges;

        bagUI* _bagUI;
        bool   _hasSprite;
        u8     _currSelectedIdx;

        void initUI( );

        bool confirmChoice( context p_context, u16 p_targetItem );
        bool useItemOnPkmn( pokemon& p_pokemon, u16 p_item );
        bool giveItemToPkmn( pokemon& p_pokemon, u16 p_item );
        void takeItemFromPkmn( pokemon& p_pokemon );

        u16  handleSelection( );
        bool handleSomeInput( touchPosition p_touch, int p_pressed );

      public:
        bagViewer( );
        ~bagViewer( ) {
            delete _bagUI;
        }
        // Returns an item which shall be used
        u16 run( );

        u16 getItem( context p_context );
    };
} // namespace BAG
