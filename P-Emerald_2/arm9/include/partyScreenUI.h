/*
Pokémon neo
------------------------------

file        : partyScreenUI.h
author      : Philip Wellnitz
description : Run the pkmn party screen

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

#include "pokemon.h"

namespace STS {
    class partyScreenUI {
      private:
        pokemon* _team;
        u8       _teamLength;
        u8       _selectedIdx;

        constexpr u16 partyTopScreenPkmnIconPosY( u8 p_pos ) {
            return ( p_pos & 1 ) * 8 + 2 + 61 * ( p_pos >> 1 );
        }

        u16 initTopScreen( bool p_bottom = false );
        u16 initBottomScreen( bool p_bottom = true );

        void drawPartyPkmn( u8 p_pos, bool p_selected, bool p_redraw = true,
                            bool p_bottom = false );
        void animatePartyPkmn( u8 p_frame, bool p_bottom = false );

      public:
        /*
         * @brief Creates a new party screen UI; does nothing else.
         * @param p_team: Pkmn party to display
         * @param p_teamLenth: Num Pkmn in team (max 6)
         * moves unselectable
         */
        partyScreenUI( pokemon p_team[ 6 ], u8 p_teamLength );

        /*
         * @brief Initializes the party screen UI. Destroys anything that was previously on the
         * screen.
         * @param p_initialSelection: initially selected Pkmn
         */
        void init( u8 p_initialSelection );

        /*
         * @brief Draws the p_frame-th frame. Needs to be called every frame.
         */
        void animate( u8 p_frame );

        /*
         * @brief Changes the selection to p_selectedIdx. Unselects old selected idx.
         */
        void select( u8 p_selectedIdx );

        /*
         * @brief Adds p_markIdx to the marked indices. Does nothing for already marked indices.
         * @param p_color: color to use for the mark (out of 4)
         */
        void mark( u8 p_markIdx, u8 p_color );

        /*
         * @brief Removes p_markIdx from the marked indices. Does nothing for unmarked indices.
         */
        void unmark( u8 p_markIdx );

        /*
         * @brief Swaps two Pkmn.
         */
        void swap( u8 p_idx1, u8 p_idx2 );
    };
} // namespace STS
