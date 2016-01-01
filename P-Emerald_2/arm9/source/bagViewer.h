/*
Pokémon Emerald 2 Version
------------------------------

file        : bagViewer.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2015
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
#include <vector>
#include <functional>

#include "bag.h"
#include "bagUI.h"
#include "item.h"
#include "uio.h"

namespace BAG {
    class bagViewer {
    private:
        u8 _currPage;
        u16 _currItem;

        std::function<bool( std::pair<u16, u16>, std::pair<u16, u16> )> _currCmp;

        bag* _origBag;

        std::vector<IO::inputTarget> _ranges;
        u8 _atHandOam;

        bagUI* _bagUI;

    public:
        bagViewer( bag* p_bag, bagUI* p_bagUI );
        void run( u8 p_startPage, u16 p_startIdx );
    };
}