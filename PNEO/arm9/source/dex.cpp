/*
Pokémon neo
------------------------------

file        : dex.cpp
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

#include "dex.h"
#include "defines.h"
#include "dexUI.h"
#include "saveGame.h"
#include "uio.h"

#include <cmath>

namespace DEX {
    u16 nextEntry( u16 p_startIdx ) {
        if( p_startIdx > MAX_PKMN ) return MAX_PKMN + 1;
        for( u16 i = p_startIdx + 1; i <= MAX_PKMN; ++i )
            if( IN_DEX( i ) ) return i;
        return MAX_PKMN + 1;
    }

    u16 previousEntry( u16 p_startIdx ) {
        if( p_startIdx > MAX_PKMN ) p_startIdx = MAX_PKMN + 1;
        if( !p_startIdx ) return 0;
        for( u16 i = p_startIdx - 1; i && i <= MAX_PKMN; --i )
            if( IN_DEX( i ) ) return i;
        return 0;
    }

    dex::dex( ) {
        _dexUI = new dexUI( );
    }

    void dex::changeMode( mode p_newMode, u16 p_startPkmn ) {
    }

    void dex::selectNational( u16 p_idx ) {
    }

    void dex::selectLocal( u16 p_page, u8 p_slot ) {
    }

    void dex::runModeChoice( ) {
    }

    void dex::runDex( ) {
    }

    void dex::run( ) {
    }

    void dex::run( u16 p_pkmnIdx ) {
    }
} // namespace DEX
