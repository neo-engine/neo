/*
Pokémon Emerald 2 Version
------------------------------

file        : dex.h
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

#include <nds.h>
#include "dexUI.h"

namespace DEX {
    u16 nextEntry( u16 p_current );
    u16 previousEntry( u16 p_current );

    class dex {
    public:
        enum mode {
            SHOW_CAUGHT = 0,
            SHOW_ALL = 1,
            SHOW_SINGLE = 2
        };
    private:
        dexUI* _dexUI;
        u16 _maxPkmn; //set this to -1 to show only a single dex entry

        u8 _page;
        u16 _curPkmn[ 32 ];
        u8 _curPkmnStart;

        u8 _selectedIdx;

        mode _mode;

        void changeMode( mode p_newMode, u16 p_startIdx );
        void select( u8 p_idx );

        void rotateForward( );
        void rotateBackward( );
    public:

        dex( mode p_mode, u16 p_maxPkmn );
        ~dex( ) {
            delete _dexUI;
        }

        void run( u16 p_pkmnIdx );
    };
}