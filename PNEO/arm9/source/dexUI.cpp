/*
Pokémon neo
------------------------------

file        : dexUI.cpp
author      : Philip Wellnitz
description :

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

#include <algorithm>
#include <cstdio>

#include "defines.h"
#include "dex.h"
#include "dexUI.h"
#include "fs.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

namespace DEX {
    void dexUI::init( ) {
    }

    void dexUI::drawModeChoice( bool p_showLocalDex, bool p_showNationalDex, u8 p_selection ) {
    }

    void dexUI::drawPkmnInfo( u16 p_pkmnId, u8 p_page, u8 p_forme, bool p_shiny, bool p_female,
                              bool p_bottom ) {
    }

    void dexUI::selectNationalIndex( u16 p_pkmnIdx, bool p_bottom ) {
    }

    void dexUI::selectLocalPageSlot( u16 p_page, u8 p_slot, bool p_bottom ) {
    }

    void dexUI::highlightButton( u8 p_button, bool p_bottom ) {
    }

    std::vector<std::pair<IO::inputTarget, u8>> dexUI::getTouchPositions( bool p_bottom ) {
        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        return res;
    }
} // namespace DEX
