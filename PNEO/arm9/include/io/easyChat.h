/*
Pokémon neo
------------------------------

file        : easyChat.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2023 - 2023
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
#pragma once

#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#include "defines.h"

namespace IO {
    struct easyChatMessage {
        u32 p_sentenceId : 8;
        u32 p_word1Id : 12;
        u32 p_word2Id : 12;

        std::string construct( );
    };

    constexpr u16 EC_MOVE_WORD = 512;
    constexpr u16 EC_PKMN_WORD = 2048;
} // namespace IO
