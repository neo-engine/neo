/*
Pokémon Emerald 2 Version
------------------------------

file        : box.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <nds.h>
#include <cstring>

#include "pokemon.h"

namespace BOX {
    struct box {
        char m_name[ 15 ];
#define MAX_PKMN_PER_BOX 18
        pokemon::boxPokemon m_pokemon[ MAX_PKMN_PER_BOX ];
        u8 m_wallpaper;

        s8 getFirstFreeSpot( ) const {
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( !( m_pokemon + i )->m_speciesId )
                    return i;
            return -1;
        }
        u8 count( ) const {
            u8 res = 0;
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( ( m_pokemon + i )->m_speciesId )
                    res++;
            return res;
        }
        u8 count( u16 p_pkmnIdx ) const {
            u8 res = 0;
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( !( m_pokemon + i )->isEgg( )
                    && ( m_pokemon + i )->m_speciesId == p_pkmnIdx )
                    res++;
            return res;
        }
        void clear( u8 p_pos ) {
            memset( m_pokemon + p_pos, 0, sizeof( pokemon::boxPokemon ) );
        }
        pokemon::boxPokemon& operator[]( u8 p_pos ) {
            return m_pokemon[ p_pos ];
        }
        const pokemon::boxPokemon& operator[]( u8 p_pos ) const {
            return m_pokemon[ p_pos ];
        }
    };
}
