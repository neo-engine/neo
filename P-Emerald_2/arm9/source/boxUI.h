/*
Pokémon Emerald 2 Version
------------------------------

file        : boxUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "box.h"
#include "uio.h"
#include <vector>

namespace BOX {
#define PAGE_ICON_START 5
    class boxUI {
        friend class boxViewer;

        enum button {
            BUTTON_LEFT,
            BUTTON_RIGHT,
            BUTTON_BOX_NAME
        };

        void buttonChange( button p_button, bool p_pressed );

        std::vector<std::pair<u8, IO::inputTarget>> _ranges;

        boxUI( );
        ~boxUI( );

        std::vector<IO::inputTarget> draw( bool p_showTeam = false );

        void select( u8 p_index );
        void takePkmn( u8 p_index, u16 p_heldPkmnIdx, bool p_isEgg );
        void updateTeam( );
    private:
        bool _showTeam;

        void drawAllBoxStatus( bool p_bottom = false );
    };
}