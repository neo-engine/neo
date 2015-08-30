/*
Pokémon Emerald 2 Version
------------------------------

file        : screenFade.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

namespace IO {
    enum fadeType {
        UNFADE,

        CLEAR_DARK,
        CLEAR_WHITE,
        CAVE_ENTRY,
        CAVE_EXIT,

        BATTLE,
        BATTLE_STRONG_OPPONENT
    };

    void fadeScreen( fadeType p_type );
    void clearScreen( bool p_bottom, bool p_both = false, bool p_dark = true );
    void clearScreenConsole( bool p_bottom, bool p_both = false );
}