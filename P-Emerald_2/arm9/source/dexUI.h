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
#include "pokemon.h"

namespace DEX {
    class dexUI {
        friend class dex;
    private:
        bool _useInDex;
        u16 _currPkmn;
        u16 _maxPkmn;
        u8 _currPage;
        u8 _currForme;

        void init( );
        void drawPage( bool p_newPok, bool p_newPage = false );
        void drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName );
        void undrawFormes( u16 p_formeIdx );
    public:
        dexUI( bool p_useInDex, u16 p_currPkmn, u16 p_maxPkmn )
            : _useInDex( p_useInDex ), _currPkmn( p_currPkmn ), _maxPkmn( p_maxPkmn ), _currPage( 0 ) { }
        u16 currPkmn( ) {
            return _currPkmn;
        }
    };
}