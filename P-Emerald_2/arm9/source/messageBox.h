/*
Pokémon Emerald 2 Version
------------------------------

file        : messageBox.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
#include <string>
#include <cstdio>

#include "item.h"

namespace IO {
#define MAXLINES 6

    class messageBox {
    public:
        enum sprite_type {
            no_sprite = 0,
            sprite_pkmn = 1,
            sprite_trainer = 2
        };

        messageBox( item* p_item, const u16 p_count );

        messageBox( const char* p_text, bool p_remsprites = true );
        messageBox( const char* p_text, const char* p_name, bool p_a = true, bool p_remsprites = true, sprite_type p_sprt = no_sprite, u16 p_sprind = 0 );
        void put( const char* p_text, bool p_a = true );
        void carriageReturn( );
        void clear( );
        void clearButName( );

        const char* m_isNamed;
    };
}