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
        u8         m_wallpaper;

        /*
         * @brief: Returns the first non-occupied spot in the box or -1 if the box is
         * full.
         */
        constexpr s8 getFirstFreeSpot( ) {
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( !( m_pokemon + i )->m_speciesId ) return i;
            return -1;
        }

        /*
         * @brief: Searches for the next filled spot after the given position.
         */
        constexpr u8 getNextFilledSpot( u8 p_position ) {
            for( u8 i = p_position + 1; i < MAX_PKMN_PER_BOX; ++i ) {
                if( m_pokemon[ i ].getSpecies( ) ) { return i; }
            }
            for( u8 i = 0; i < p_position; ++i ) {
                if( m_pokemon[ i ].getSpecies( ) ) { return i; }
            }
            return p_position;
        }

        /*
         * @brief: Searches for the last filled spot before the given position.
         */
        constexpr u8 getPrevFilledSpot( u8 p_position ) {
            for( s8 i = p_position - 1; i > 0; --i ) {
                if( m_pokemon[ i ].getSpecies( ) ) { return i; }
            }
            for( u8 i = MAX_PKMN_PER_BOX - 1; i > p_position; --i ) {
                if( m_pokemon[ i ].getSpecies( ) ) { return i; }
            }
            return p_position;
        }

        /*
         * @brief: Returns the number of pokemon in the box.
         */
        constexpr u8 count( ) {
            u8 res = 0;
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( ( m_pokemon + i )->m_speciesId ) res++;
            return res;
        }

        /*
         * @brief: Returns how often the given pokemon appears in the box.
         */
        constexpr u8 count( u16 p_pkmnIdx ) {
            u8 res = 0;
            for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i )
                if( !( m_pokemon + i )->isEgg( ) && ( m_pokemon + i )->m_speciesId == p_pkmnIdx )
                    res++;
            return res;
        }

        /*
         * @brief: Clears the given position of the box.
         */
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
