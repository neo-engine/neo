/*
Pokémon neo
------------------------------

file        : messageBox.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2020
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

#include <cstdio>
#include <string>
#include <nds.h>

#include "item.h"

namespace IO {
#define MAXLINES 6

    class messageBox {
      public:
        enum sprite_type { no_sprite = 0, sprite_pkmn = 1, sprite_trainer = 2 };

        explicit messageBox( u16 p_itemId, ITEM::itemData& p_data, const u16 p_count );

        explicit messageBox( const char* p_text, bool p_remsprites = true );
        explicit messageBox( const char* p_text, const char* p_name, bool p_a = true,
                             bool p_remsprites = true, sprite_type p_sprt = no_sprite,
                             u16 p_sprind = 0 );
        void put( const char* p_text, bool p_a = true );
        void carriageReturn( );
        void clear( );
        void clearButName( );

        const char* m_isNamed;
    };
}
