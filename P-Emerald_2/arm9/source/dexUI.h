/*
Pokémon Emerald 2 Version
------------------------------

file        : dexUI.h
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
#include <string>

namespace DEX {
    class dexUI {
    private:
        bool _useInDex;
        u16 _maxPkmn;
        u8 _currForme;

        void drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName );
    public:
        dexUI( bool p_useInDex, u16 p_maxPkmn );

        void drawPage( u16 p_pkmnIdx, u8 p_page );
        s8 drawSub( u8 p_mode, u16 p_pkmnIdcs[ 32 ], u16 p_idxStart, u8 p_selectedIdx ); // +1 -> rotateForward, -1 -> rotateBackward

        void changeMode( u8 p_newMode );

        void select( u8 p_idx );
    };
}