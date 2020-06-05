/*
Pokémon neo
------------------------------

file        : statusScreen.h
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

#include "pokemon.h"
#include "statusScreenUI.h"

namespace STS {
    class statusScreen {
      private:
        pokemon*        _pokemon;
        statusScreenUI* _ui;

        u8 _currentPage;
        u8 _frame;

        bool _allowKeyUp;
        bool _allowKeyDown;

        void select( u8 p_newPage );

      public:
        enum result { BACK, NEXT_PKMN, PREV_PKMN, EXIT };

        /*
         * @brief Creates a new status screen. Does nothing else
         * @param p_allowKeyUp: Handle KEY_UP
         * @param p_allowKeyDown: Handle KEY_DOWN
         */
        statusScreen( pokemon* p_pokemon, bool p_allowKeyUp = true, bool p_allowKeyDown = true,
                      statusScreenUI* p_ui = nullptr );

        inline ~statusScreen( ) {
            if( _ui ) { delete _ui; }
        }

        /*
         * @brief Runs the status screen. Destroys anything that was previously on the screen.
         * @param p_initialPage: initially selected page
         * @returns an action the player wishes to do
         */
        result run( u8 p_initialPage = 0 );

        /*
         * @brief Returns the current page.
         */
        inline u8 currentPage( ) const {
            return _currentPage;
        }
    };
} // namespace STS
