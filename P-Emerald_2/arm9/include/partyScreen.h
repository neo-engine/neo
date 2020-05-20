/*
Pokémon neo
------------------------------

file        : partyScreen.h
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

#include <vector>

#include "move.h"
#include "partyScreenUI.h"
#include "pokemon.h"

namespace STS {
    class partyScreen {
      public:
        union result {
            u8  m_selectedPkmn;
            u16 m_selectedMove;
        };

      private:
        enum state {
            GENERAL, // no pkmn has focus
            SINGLE   // Single Pkmn has focus
        };

        enum choice {
            SELECT = 0,
            STATUS,
            GIVE_ITEM,
            TAKE_ITEM,
            USE_ITEM,
            FIELD_MOVE_1,
            FIELD_MOVE_2,
            FIELD_MOVE_3,
            FIELD_MOVE_4,
            SWAP,
            DEX_ENTRY,
            CANCEL
        };

        const u16 getTextForChoice( const choice p_choice );

        std::vector<choice> _currentChoices; // Possible choices for the currently selected pkmn
        u8                  _currentChoiceSelection; // Current choice selected (SINGLE mode)

        state    _curState;
        pokemon* _team;               // Pkmn to display
        u8       _teamLength;         // Num Pkmn in team
        u8       _currentSelection;   // cur selected pkmn (always exactly one)
        result   _currentMarksOrMove; // cur marked pkmn / cur selected move
        u8       _toSelect;           // num of pokemon to select
        u8       _allowMoveSelection; // allow player to select field moves of pkmn
        u8       _allowItems;         // allow player to use items
        u8       _allowDex;           // allow player to use the pokedex

        u8             _frame;
        partyScreenUI* _partyUI;

        /*
         * @brief Checks if enough pkmn are marked.
         */
        bool checkReturnCondition( );

        /*
         * @brief Checks whether player is satisfied with the set of selected Pkmn.
         */
        bool confirmSelection( );

        /*
         * @brief Selects the specified choice window
         */
        void selectChoice( u8 p_choice );

        /*
         * @brief Focusses the pkmn at position p_selectedIdx. Returns when pkmn leaves focus.
         */
        bool focus( u8 p_selectedIdx );

        /*
         * @brief Changes the selection to p_selectedIdx. Unselects old selected idx.
         */
        void select( u8 p_selectedIdx );

        /*
         * @brief Adds p_markIdx to the marked indices. Does nothing for already marked indices.
         */
        void mark( u8 p_markIdx );

        /*
         * @brief Removes p_markIdx from the marked indices. Does nothing for unmarked indices.
         */
        void unmark( u8 p_markIdx );

        /*
         * @brief Swaps two Pkmn.
         */
        void swap( u8 p_idx1, u8 p_idx2 );

        /*
         * @brief Computes the possible selections for the currently selected pkmn.
         */
        void computeSelectionChoices( );

      public:
        /*
         * @brief Creates a new party screen; does nothing else.
         * @param p_team: Pkmn party to display
         * @param p_teamLenth: Num Pkmn in team (max 6)
         * @param p_allowMoves: Allow to select a field move
         * @param p_allowItems: Allow to interact with items
         * @param p_allowMoves: Allow to show dex entry
         * @param p_toSelect: Number of Pkmn the player has to select. Any value > 0 makes field
         * moves unselectable
         */
        partyScreen( pokemon p_team[ 6 ], u8 p_teamLength, bool p_allowMoves = true,
                     bool p_allowItems = true, bool p_allowDex = true, u8 p_toSelect = 0 );

        ~partyScreen( );

        /*
         * @brief Runs the party screen. Destroys anything that was previously on the screen.
         * @param p_initialSelection: initially selected Pkmn
         * @returns Either a move selected or a (bitmask) of selected indices
         */
        result run( u8 p_initialSelection = 0 );
    };
} // namespace STS
