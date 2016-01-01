/*
Pokémon Emerald 2 Version
------------------------------

file        : statusScreen.h
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

#include <vector>

#include "pokemon.h"
#include "statusScreenUI.h"

namespace STS {
    class statusScreen {
    protected:
        u8 _page;
        u8 _pkmnIdx;
        std::vector<pokemon>* _pokemon;
        statusScreenUI* _stsUI;
    public:

        virtual s16 drawPage( ) = 0;
        virtual void run( ) = 0;
    };

    class regStsScreen : public statusScreen {
    public:
        regStsScreen( u8 p_pkmnIdx, statusScreenUI* p_stsUI );

        void run( ) override;
        s16 drawPage( ) override;
    };

    //class battleStsScreen : public statusScreen {
    //    s16 run( bool _time, s8 p_timeParameter ) override;
    //    s16 drawPage(  ) override;
    //};
}