/*
Pokémon neo
------------------------------

file        : easyChat.cpp
author      : Philip Wellnitz
description : Simple translatable message system.

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
#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#include "fs/data.h"
#include "io/easyChat.h"

namespace IO {
    std::string easyChatMessage::construct( ) const {
        char bufferM[ 200 ];
        char bufferW1[ 30 ];
        char bufferW2[ 30 ];

        snprintf( bufferW1, 29, "%s", FS::getEasyChatWord( m_word1Id ) );
        snprintf( bufferW2, 29, "%s", FS::getEasyChatWord( m_word2Id ) );
        snprintf( bufferM, 199, FS::getEasyChatMessage( m_sentenceId ), bufferW1, bufferW2 );

        return std::string( bufferM );
    }
} // namespace IO
