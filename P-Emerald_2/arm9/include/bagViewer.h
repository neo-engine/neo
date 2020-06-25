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
            NO_SPECIAL_CONTEXT,
            BATTLE,       // Allow only items usable in-battle
            WILD_BATTLE,  // Allow only items usable in-battle and pokeballs
            GIVE_TO_PKMN, // Allow only items, medicine and berries
            // TODO: USE_ON_PKMN,  // Allow only evo items, medicine, berries, and forme change
        };

      private:
        /*
         * @brief: Returns true if the current context allows the gaven item to be
         * displayed.
         */
        bool isAllowed( u16 p_itemId );

        void initView( );

        std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> _ranges;

        std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>> _view;

        bagUI* _bagUI;
        bool   _hasSprite;
        u8     _currSelectedIdx;

        pokemon* _playerTeam;

        u16 _currentViewStart;
        u16 _currentViewEnd;

        context _context;

        void initUI( );

        bool confirmChoice( u16 p_targetItem );

        inline std::pair<std::pair<u16, u16>, ITEM::itemData>
            currentItem( ) const {
            return _view[ _currSelectedIdx ];
        }

        /*
         * @brief: Uses the specified item on the given pkmn.
         * @returns: true if the item needs to be removed from the bag, false otherwise.
         */
        bool useItemOnPkmn( pokemon& p_pokemon, u16 p_item, ITEM::itemData* p_data );

        /*
         * @brief: Gives the specified item to the given pkmn.
         * @returns: True iff the item needs to bo removed from the bag.
         */
        bool giveItemToPkmn( pokemon& p_pokemon, u16 p_item );

        /*
         * @brief: Removes any item the given pkmn holds and adds it to bag.
         */
        void takeItemFromPkmn( pokemon& p_pokemon );

        u16  handleSelection( );
        bool handleSomeInput( bool p_allowSort = true );

      public:
        bagViewer( pokemon* p_playerTeam, context p_context = NO_SPECIAL_CONTEXT );
        ~bagViewer( ) {
            delete _bagUI;
        }

        /*
         * @brief: Runs the bag viewer. May return an item that is to be used in the OW
         * (e.g. Honey or Repel).
         */
        u16 run( );

        /*
         * @brief: Runs the bag viewer to make the player select an item from the bag that
         * fits the specified context.
         */
        u16 getItem( context p_context );
    };
} // namespace BAG
