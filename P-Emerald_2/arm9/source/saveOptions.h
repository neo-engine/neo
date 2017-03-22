/*
Pokémon Emerald 2 Version
------------------------------

file        : options.h
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2017
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

namespace SAVE {
    enum language {
        EN,
        GER
    };
    
    struct saveOptions {
        u8          m_EXPShareEnabled;
        u8          m_evolveInBattle;
        u8          m_bgIdx;
        
        language    m_language;
        s8          m_levelModifier;
        s8          m_encounterRateModifier;
        s8          m_textSpeedModifier;
    };
}