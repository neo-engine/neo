/*
Pokémon neo
------------------------------

file        : boxViewer.h
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

#include <nds.h>

#include "box.h"
#include "boxUI.h"
#include "statusScreenUI.h"

namespace BOX {
    class boxViewer {
      private:
        enum mode {
            STATUS = 0, // pressing A on a pkmn shows menu
            MOVE   = 1, // pressing A on a pkmn picks it up
        };

        mode _mode;

        std::vector<IO::inputTarget> _ranges;
        u8                           _selectedIdx;
        std::pair<u8, u8>            _heldPkmnPos; //(box, pos in box); (_, 31..36) for team pkmn
        pokemon                      _heldPkmn;
        bool                         _showParty;
        boxUI                        _boxUI;

        /*
         * @brief: Selects the pkmn/button at p_index.
         */
        void select( u8 p_index );

        /*
         * @brief: Takes and holds the pkmn at p_index.
         * @param p_continuousSwap: When "taking" a pkmn while already holding a pkmn, allow
         * to swap these pkmn and to directly "take" the new pkmn.
         */
        void takePkmn( u8 p_index, bool p_continuousSwap );

        /*
         * @brief: Returns the currently held pkmn where it came from.
         */
        void returnPkmn( );

        /*
         * @brief: Shows the party and runs everything related.
         * @returns: True if the player pressed X
         */
        bool runParty( );

        /*
         * @brief: Runs the status choice for a pkmn (show status, move, release, ...)
         * Returns 1 if the pkmn has to be picked up, 0 otherwise.
         * (Picking up the pkmn may require placing the currently held pkmn.)
         */
        u8 runStatusChoice( );

        /*
         * @brief: Runs the choice screen for the various box settings. (rename,
         * wallpaper, sort)
         */
        void runBoxSettings( );

        /*
         * @brief: Shows the current pkmn's status.
         * @returns: 255 if the user pressed x; 0 otherwise
         */
        u8 showPkmnStatus( );

        /*
         * @brief: Returns the pokemon at the given position.
         * if p_position is larger than MEX_PKMN_PER_BOX, a party pkmn is returned.
         */
        boxPokemon* getPkmn( u8 p_position );
        boxPokemon* getPkmn( std::pair<u8, u8> p_position );

        /*
         * @brief: Overrides the pokemon at the given position with the given pokemon.
         */
        void setPkmn( u8 p_position, boxPokemon* p_pokemon );
        void setPkmn( u8 p_position, pokemon* p_pokemon );
        void setPkmn( std::pair<u8, u8> p_position, boxPokemon* p_pokemon );
        void setPkmn( std::pair<u8, u8> p_position, pokemon* p_pokemon );

      public:
        /*
         * @brief: Runs the box interface. Bottom screen requires complete re-init
         * afterwards (including setMode)
         */
        void run( );
    };
} // namespace BOX
