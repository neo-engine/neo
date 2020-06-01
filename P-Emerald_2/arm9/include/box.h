/*
Pokémon neo
------------------------------

file        : box.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <cstring>
#include <nds.h>

#include "pokemon.h"

namespace BOX {
    struct box {
        char m_name[ 12 ];
#define MAX_PKMN_PER_BOX 30
        boxPokemon m_pokemon[ MAX_PKMN_PER_BOX ];
        u8                  m_wallpaper;

        constexpr s8 getFirstFreeSpot( )  {
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( !( m_pokemon + i )->m_speciesId ) return i;
            return -1;
        }
        constexpr u8 count( ) {
            u8 res = 0;
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( ( m_pokemon + i )->m_speciesId ) res++;
            return res;
        }
        constexpr u8 count( u16 p_pkmnIdx ) {
            u8 res = 0;
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( !( m_pokemon + i )->isEgg( ) && ( m_pokemon + i )->m_speciesId == p_pkmnIdx )
                    res++;
            return res;
        }
        void clear( u8 p_pos ) {
            memset( m_pokemon + p_pos, 0, sizeof( boxPokemon ) );
        }
        boxPokemon& operator[]( u8 p_pos ) {
            return m_pokemon[ p_pos ];
        }
        const boxPokemon& operator[]( u8 p_pos ) const {
            return m_pokemon[ p_pos ];
        }
    };
} // namespace BOX
