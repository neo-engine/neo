/*
Pokémon neo
------------------------------

file        : pokemon.cpp
author      : Philip Wellnitz
description : The main Pokémon engine

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

#include "itemNames.h"
#include "mapDrawer.h"
#include "move.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "saveGame.h"

boxPokemon::boxPokemon( u16 p_pkmnId, u16 p_level, u8 p_forme, const char* p_name, u8 p_shiny,
                        bool p_hiddenAbility, bool p_isEgg, u8 p_ball, u8 p_pokerus,
                        bool p_fatefulEncounter, pkmnData* p_data )
    : boxPokemon::boxPokemon(
          nullptr, p_pkmnId, p_name, p_level, SAVE::SAV->getActiveFile( ).m_id,
          SAVE::SAV->getActiveFile( ).m_sid, SAVE::SAV->getActiveFile( ).m_playername,
          !SAVE::SAV->getActiveFile( ).m_isMale, p_shiny, p_hiddenAbility, p_fatefulEncounter,
          p_isEgg, MAP::curMap->getCurrentLocationId( ), p_ball, p_pokerus, p_forme, p_data ) {
}

boxPokemon::boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id,
                        u16 p_sid, const char* p_oT, bool p_oTFemale, u8 p_shiny,
                        bool p_hiddenAbility, bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace,
                        u8 p_ball, u8 p_pokerus, u8 p_forme, pkmnData* p_data ) {
    pkmnData data;
    if( p_data == nullptr ) {
        data = getPkmnData( p_pkmnId, p_forme );
    } else {
        data = *p_data;
    }

    m_oTId  = p_id;
    m_oTSid = p_sid;
    m_pid   = rand( );
    if( p_shiny == 2 )
        while( !isShiny( ) ) m_pid = rand( );
    else if( p_shiny == 1 )
        while( isShiny( ) ) m_pid = rand( );
    else if( p_shiny ) { // Try p_shiny - 2 additional times to generate a shiny PId
        for( u8 i = 0; i < p_shiny - 2 && !isShiny( ); ++i ) m_pid = rand( );
    }
    m_speciesId = p_pkmnId;

    if( data.m_baseForme.m_items[ 3 ] )
        m_heldItem = data.m_baseForme.m_items[ 3 ];
    else {
        u8 m_b1 = rand( ) % 100;
        if( m_b1 < 5 && data.m_baseForme.m_items[ 0 ] )
            m_heldItem = data.m_baseForme.m_items[ 0 ];
        else if( m_b1 < 20 && data.m_baseForme.m_items[ 1 ] )
            m_heldItem = data.m_baseForme.m_items[ 1 ];
        else if( m_b1 < 80 && data.m_baseForme.m_items[ 2 ] )
            m_heldItem = data.m_baseForme.m_items[ 2 ];
    }

    if( !p_isEgg ) m_experienceGained = EXP[ p_level - 1 ][ data.getExpType( ) ];

    if( p_isEgg ) {
        m_steps          = data.m_eggCycles;
        m_gotDate[ 0 ]   = acday;
        m_gotDate[ 1 ]   = acmonth;
        m_gotDate[ 2 ]   = acyear % 100;
        m_gotPlace       = p_gotPlace;
        m_hatchDate[ 0 ] = m_hatchDate[ 1 ] = m_hatchDate[ 2 ] = m_hatchPlace = 0;
    } else {
        m_steps          = data.m_baseFriend;
        m_hatchDate[ 0 ] = acday;
        m_hatchDate[ 1 ] = acmonth;
        m_hatchDate[ 2 ] = acyear % 100;
        m_gotPlace       = p_gotPlace;
    }

    m_ability = ( p_hiddenAbility && data.m_baseForme.m_abilities[ 2 ] )
                    ? ( ( ( m_pid & 1 ) || !data.m_baseForme.m_abilities[ 3 ] )
                            ? data.m_baseForme.m_abilities[ 2 ]
                            : data.m_baseForme.m_abilities[ 3 ] )
                    : ( ( ( m_pid & 1 ) || !data.m_baseForme.m_abilities[ 1 ] )
                            ? data.m_baseForme.m_abilities[ 0 ]
                            : data.m_baseForme.m_abilities[ 1 ] );
    m_origLang = 5;

    if( p_moves )
        memcpy( m_moves, p_moves, sizeof( m_moves ) );
    else
        getLearnMoves( p_pkmnId, 0, p_level, 4, m_moves );
    for( u8 i = 0; i < 4; ++i ) {
        MOVE::moveData mdata = MOVE::getMoveData( m_moves[ i ] );
        m_acPP[ i ]          = mdata.m_pp;
    }

    for( u8 i = 0; i < 6; ++i ) IVset( i, rand( ) & 31 );
    setIsNicknamed( !!p_name );
    setIsEgg( p_isEgg );
    m_fateful = p_fatefulEncounter;

    pkmnGenderType A = data.m_baseForme.m_genderRatio;
    if( A == MALE )
        m_isFemale = m_isGenderless = false;
    else if( A == FEMALE )
        m_isFemale = true, m_isGenderless = false;
    else if( A == GENDERLESS )
        m_isFemale = false, m_isGenderless = true;
    else if( ( m_pid & 255 ) >= A )
        m_isFemale = m_isGenderless = false;
    else
        m_isFemale = true, m_isGenderless = false;

    m_altForme = p_forme;
    if( p_name ) {
        strcpy( m_name, p_name );
        setIsNicknamed( true );
    } else {
        getDisplayName( p_pkmnId, m_name, CURRENT_LANGUAGE );
        setIsNicknamed( false );
    }
    m_hometown = 4;
    strcpy( m_oT, p_oT );
    m_pokerus    = p_pokerus;
    m_ball       = p_ball;
    m_gotLevel   = p_level;
    m_oTisFemale = p_oTFemale;
    recalculateForme( );
}

bool boxPokemon::isShiny( ) const {
    return !( ( ( ( m_oTId ^ m_oTSid ) >> 3 )
                ^ ( ( ( m_pid >> 16 ) ^ ( m_pid % ( 1 << 16 ) ) ) ) >> 3 ) );
}

/*
 * @brief: Returns the gender. -1 for female, 0 for genderless, and 1 for male
 */
s8 boxPokemon::gender( ) const {
    if( m_isGenderless )
        return 0;
    else if( m_isFemale )
        return -1;
    return 1;
}

u8 boxPokemon::getForme( ) {
    return m_altForme;
}

void boxPokemon::recalculateForme( ) {
    switch( m_speciesId ) {
    case PKMN_GIRATINA: {
        m_altForme = ( m_heldItem == I_GRISEOUS_ORB );
        return;
    }
    case PKMN_ARCEUS: {
        if( m_heldItem >= I_FLAME_PLATE && m_heldItem <= I_IRON_PLATE ) {
            m_altForme = m_heldItem - I_FLAME_PLATE + 1;
        } else if( m_heldItem == I_NULL_PLATE ) {
            m_altForme = 17;
        } else if( m_heldItem == I_PIXIE_PLATE ) {
            m_altForme = 18;
        } else if( m_heldItem >= I_FIRIUM_Z && m_heldItem <= I_STEELIUM_Z ) {
            m_altForme = m_heldItem - I_FIRIUM_Z + 1;
        } else if( m_heldItem == I_FAIRIUM_Z ) {
            m_altForme = 18;
        } else if( m_heldItem >= I_FIRIUM_Z2 && m_heldItem <= I_STEELIUM_Z2 ) {
            m_altForme = m_heldItem - I_FIRIUM_Z2 + 1;
        } else if( m_heldItem == I_FAIRIUM_Z2 ) {
            m_altForme = 18;
        } else {
            m_altForme = 0;
        }
        return;
    }
    case PKMN_GENESECT: {
        if( m_heldItem >= I_DOUSE_DRIVE && m_heldItem <= I_CHILL_DRIVE ) {
            m_altForme = m_heldItem - I_DOUSE_DRIVE + 1;
        } else {
            m_altForme = 0;
        }
        return;
    }
    case PKMN_SILVALLY: {
        if( m_heldItem >= I_FIGHTING_MEMORY && m_heldItem <= I_FAIRY_MEMORY ) {
            m_altForme = m_heldItem - I_FIGHTING_MEMORY + 1;
        } else {
            m_altForme = 0;
        }
        return;
    }
    }
}

bool boxPokemon::setNature( pkmnNatures p_newNature ) {
    if( getNature( ) == p_newNature ) { return false; }
    bool shiny = isShiny( );
    for( ; getNature( ) != p_newNature || isShiny( ) != shiny; m_pid = rand( ) )
        ;
    return true;
}

bool boxPokemon::swapAbilities( ) {
    pkmnData data = getPkmnData( m_speciesId, getForme( ) );

    if( m_ability == data.m_baseForme.m_abilities[ 2 ] ) {
        if( !data.m_baseForme.m_abilities[ 3 ] ) {
            return false;
        } else {
            m_ability = data.m_baseForme.m_abilities[ 3 ];
            return true;
        }
    }
    if( m_ability == data.m_baseForme.m_abilities[ 3 ] ) {
        m_ability = data.m_baseForme.m_abilities[ 2 ];
        return true;
    }
    if( m_ability == data.m_baseForme.m_abilities[ 0 ] ) {
        if( !data.m_baseForme.m_abilities[ 1 ] ) {
            return false;
        } else {
            m_ability = data.m_baseForme.m_abilities[ 1 ];
            return true;
        }
    }
    if( m_ability == data.m_baseForme.m_abilities[ 1 ] ) {
        m_ability = data.m_baseForme.m_abilities[ 0 ];
        return true;
    }
    return false;
}

void boxPokemon::hatch( ) {
    setIsEgg( false );
    m_hatchPlace     = MAP::curMap->getCurrentLocationId( );
    m_hatchDate[ 0 ] = acday;
    m_hatchDate[ 1 ] = acmonth + 1;
    m_hatchDate[ 2 ] = ( acyear + 1900 ) % 100;
}
