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
#define PAGE_START 1 // + 3
#define FRAME_START_2 10 // + 5
#define PKMN_ICON_START_2 15 // + 5
#define PKMN_ICON_START 32 // + 32
#define FRAME_START 64 // + 32
#define BG_SPR_START 96 // + 2 * 8
    class dexUI {
    private:
        bool _useInDex;
        u16 _maxPkmn;

        void drawFormes( u16 p_pkmnId, u8 p_forme, u8 p_formeCnt, bool p_isFemale, bool p_isGenderless );
    public:
        dexUI( bool p_useInDex, u16 p_maxPkmn );

        void drawPage( u16 p_pkmnIdx, u8 p_page, u8 p_forme );
        void drawSub( u8 p_mode, u16 p_pkmnIdcs[ 32 ], u16 p_idxStart, u8 p_selectedIdx, u8 p_oldIdx = 0 );
    };
}
