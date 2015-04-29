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