/*
Pokémon Emerald 2 Version
------------------------------

file        : boxViewer.h
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

#include "box.h"
#include "boxUI.h"

namespace BOX {
    class boxViewer {
    private:
        box* _box;
        boxUI* _boxUI;
        u8 _currPos;
        std::pair<u16, u16> _currPage[ 30 ];

        std::vector<IO::inputTarget> _ranges;
        u8 _atHandOam;

        u16 nextNonEmptyBox( u16 p_start );
        u16 previousNonEmptyBox( u16 p_start );
        void generateNextPage( );
        void generatePreviousPage( );
    public:
        boxViewer( box* p_box, boxUI* p_boxUI, u16 p_currPkmn )
            : _box( p_box ), _boxUI( p_boxUI ), _currPos( 0 ) {
            for( u8 i = 0; i < 30; ++i )
                _currPage[ i ] = { 0, 0 };
            generateNextPage( );
        }

        void run( bool p_allowTakePkmn = false );
    };
}