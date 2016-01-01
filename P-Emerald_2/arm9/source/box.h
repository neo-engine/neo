/*
Pokémon Emerald 2 Version
------------------------------

file        : box.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <nds.h>
#include <list>
#include <algorithm>

#include "pokemon.h"

namespace BOX {
    class box {
    private:
        std::list<pokemon::boxPokemon> _storedPkmn[ 1 + MAX_PKMN ]; //An extra box for eggs
    public:
        void insert( pokemon p_pokemon ) {
            if( !p_pokemon.m_boxdata.m_individualValues.m_isEgg )
                _storedPkmn[ p_pokemon.m_boxdata.m_speciesId - 1 ].push_back( p_pokemon.m_boxdata );
            else
                _storedPkmn[ MAX_PKMN ].push_back( p_pokemon.m_boxdata );
        }
        void insert( pokemon::boxPokemon p_pokemon ) {
            if( !p_pokemon.m_individualValues.m_isEgg )
                _storedPkmn[ p_pokemon.m_speciesId - 1 ].push_back( p_pokemon );
            else
                _storedPkmn[ MAX_PKMN ].push_back( p_pokemon );
        }
        void erase( pokemon p_pokemon ) {
            if( !p_pokemon.m_boxdata.m_individualValues.m_isEgg )
                _storedPkmn[ p_pokemon.m_boxdata.m_speciesId - 1 ].remove( p_pokemon.m_boxdata );
            else
                _storedPkmn[ MAX_PKMN ].remove( p_pokemon.m_boxdata );
        }
        void erase( pokemon::boxPokemon p_pokemon ) {
            if( !p_pokemon.m_individualValues.m_isEgg )
                _storedPkmn[ p_pokemon.m_speciesId - 1 ].remove( p_pokemon );
            else
                _storedPkmn[ MAX_PKMN ].remove( p_pokemon );
        }

        u16 count( u16 p_pkmnSpecies ) {
            if( !p_pkmnSpecies )
                return 0;
            return _storedPkmn[ p_pkmnSpecies - 1 ].size( );
        }

        bool empty( u16 p_pkmnSpecies ) {
            if( !p_pkmnSpecies )
                return 0;
            return _storedPkmn[ p_pkmnSpecies - 1 ].empty( );
        }

        std::list<pokemon::boxPokemon>& operator[]( u16 p_pkmnSpecies ) {
            return _storedPkmn[ p_pkmnSpecies - 1 ];
        }
        std::list<pokemon::boxPokemon> operator[]( u16 p_pkmnSpecies ) const {
            return _storedPkmn[ p_pkmnSpecies - 1 ];
        }
    };
}