/*
Pokémon neo
------------------------------

file        : boxUI.h
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
#include "box.h"
#include "uio.h"

namespace BOX {
#define PAGE_ICON_START 5
    class boxUI {
        bool _showTeam;


        public:
            /*
             * @brief: Initializes the box UI.
             */
            void init( );

            /*
             * @brief: Draw the specified box.
             */
            std::vector<IO::inputTarget> draw( box* p_box, bool p_showTeam = false );







            enum button { BUTTON_LEFT, BUTTON_RIGHT, BUTTON_BOX_NAME };
            void buttonChange( button p_button, bool p_pressed );

            std::vector<std::pair<u8, IO::inputTarget>> _ranges;

            void select( u8 p_index );
            void takePkmn( u8 p_index, u16 p_heldPkmnIdx, bool p_isEgg );
            void updateTeam( );

            void drawAllBoxStatus( bool p_bottom = false );
    };
}
