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
            SELL_ITEM,    // Sell items
        };

      private:
        enum choice {
            DEREGISTER_ITEM,
            REGISTER_ITEM,
            TOSS_ITEM,
            USE_ITEM,
            USE_TM,
            APPLY_ITEM,
            GIVE_ITEM,
            VIEW_DETAILS,
            SELL,
            BACK,
        };

        std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>> _view;

        bagUI* _bagUI;
        bool   _hasSprite;
        u8     _currSelectedIdx;

        pokemon* _playerTeam;

        u16 _currentViewStart;
        u16 _currentViewEnd;

        context _context;

        std::vector<choice> _choices;
        u8                  _currentChoice;

        void initUI( );

        void waitForInteract( );

        constexpr u16 getTextForChoice( const choice p_choice ) const {
            switch( p_choice ) {
            case DEREGISTER_ITEM:
                return 402;
            case REGISTER_ITEM:
                return 46;
            case TOSS_ITEM:
                return 48;
                return 47;
            case GIVE_ITEM:
                return 44;
            case USE_ITEM:
            case USE_TM:
            case APPLY_ITEM:
                return 47;
            case VIEW_DETAILS:
                return 401;
            case BACK:
                return 330;
            case SELL:
                return 483;
            }
            return 0;
        }

        u8 chooseMove( const boxPokemon* p_pokemon, u16 p_extraMove = 0 );

        u16 executeChoice( choice p_choice );

        std::vector<choice> getItemChoices( u16 p_itemId, ITEM::itemData* p_data );

        u8 confirmChoice( u16 p_targetItem, ITEM::itemData* p_data );

        inline std::pair<std::pair<u16, u16>, ITEM::itemData> currentItem( ) const {
            return _view[ _currSelectedIdx ];
        }

        /*
         * @brief: Returns true if the current context allows the gaven item to be
         * displayed.
         */
        bool isAllowed( u16 p_itemId );

        void initView( );

        /*
         * @brief: Selects the item at the specified index. If p_index is negative or
         * larger than _view.size(), new items are loaded into the view.
         */
        void selectItem( s8 p_index );

        /*
         * @brief: Selects and shows the new page.
         */
        void selectPage( u8 p_page );

        /*
         * @brief: Handles all touch input related things.
         * @returns: 1 if the player pressed back.
         */
        u8 handleTouch( );

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

        /*
         * @brief: Handles the interaction of a player with a specific item.
         * @returns: 0 if nothing happened; 1 if the item got consumed and 2 if the bag is exited.
         * If an item has yet to be used the 14 highest bits contain its id
         */
        u16 handleSelection( );

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
         * @brief: Runs the bag viewer to make the player select an item from the bag.
         */
        u16 getItem( );
    };
} // namespace BAG
