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

#include <algorithm>

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
          nullptr, p_pkmnId, p_name, p_level, SAVE::SAV.getActiveFile( ).m_id,
          SAVE::SAV.getActiveFile( ).m_sid, SAVE::SAV.getActiveFile( ).m_playername,
          p_shiny, p_hiddenAbility, p_fatefulEncounter,
          p_isEgg, MAP::curMap->getCurrentLocationId( ), p_ball, p_pokerus, p_forme, p_data ) {
}

boxPokemon::boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id,
                        u16 p_sid, const char* p_oT, u8 p_shiny,
                        bool p_hiddenAbility, bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace,
                        u8 p_ball, u8 p_pokerus, u8 p_forme, pkmnData* p_data ) {
    pkmnData data;
    if( p_data == nullptr ) {
        data = getPkmnData( p_pkmnId, p_forme );
    } else {
        data = *p_data;
    }

    m_rand = rand( ) & 0xF;

    m_oTId  = p_id;
    m_oTSid = p_sid;
    m_pid   = rand( );
    if( p_shiny == 2 ) {
        while( !isShiny( ) ) { m_pid = rand( ); }
        m_shinyType = 2;
    } else if( p_shiny == 1 ) {
        while( isShiny( ) ) m_pid = rand( );
    } else if( p_shiny ) { // Try p_shiny - 2 additional times to generate a shiny PId
        for( u8 i = 0; i < p_shiny - 2 && !isShiny( ); ++i ) {
            m_pid       = rand( );
            m_shinyType = 1;
        }
        if( SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ),
                                                    I_SHINY_CHARM ) ) {
            for( u8 i = 0; i < p_shiny - 2 && !isShiny( ); ++i ) {
                m_pid       = rand( );
                m_shinyType = 1;
            }
        }
    }

    m_experienceGained = EXP[ p_level - 1 ][ data.getExpType( ) ];

    if( data.m_baseForme.m_items[ 3 ] )
        m_heldItem = data.m_baseForme.m_items[ 3 ];
    else {
        u8 m_b1 = rand( ) % 100;
        if( m_b1 < 1 && data.m_baseForme.m_items[ 0 ] )
            m_heldItem = data.m_baseForme.m_items[ 0 ];
        else if( m_b1 < 6 && data.m_baseForme.m_items[ 1 ] )
            m_heldItem = data.m_baseForme.m_items[ 1 ];
        else if( m_b1 < 56 && data.m_baseForme.m_items[ 2 ] )
            m_heldItem = data.m_baseForme.m_items[ 2 ];
    }

    if( p_isEgg ) {
        m_steps          = data.m_eggCycles;
        m_gotDate[ 0 ]   = SAVE::CURRENT_DATE.m_day;
        m_gotDate[ 1 ]   = SAVE::CURRENT_DATE.m_month;
        m_gotDate[ 2 ]   = SAVE::CURRENT_DATE.m_year % 100;
        m_gotPlace       = p_gotPlace;
        m_hatchDate[ 0 ] = m_hatchDate[ 1 ] = m_hatchDate[ 2 ] = m_hatchPlace = 0;
    } else {
        m_steps          = data.m_baseFriend;
        m_hatchDate[ 0 ] = SAVE::CURRENT_DATE.m_day;
        m_hatchDate[ 1 ] = SAVE::CURRENT_DATE.m_month;
        m_hatchDate[ 2 ] = SAVE::CURRENT_DATE.m_year % 100;
        m_hatchPlace     = p_gotPlace;
    }
    m_origLang = 5;

    if( p_moves )
        memcpy( m_moves, p_moves, sizeof( m_moves ) );
    else
        getLearnMoves( p_pkmnId, 0, p_level, 4, m_moves );
    for( u8 i = 0; i < 4; ++i ) {
        MOVE::moveData mdata = MOVE::getMoveData( m_moves[ i ] );
        m_curPP[ i ]         = mdata.m_pp;
    }

    for( u8 i = 0; i < 6; ++i ) IVset( i, rand( ) & 31 );

    if( p_fatefulEncounter ) {
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
        IVset( rand( ) % 6, 31 );
    }

    setIsNicknamed( !!p_name );
    setIsEgg( p_isEgg );
    m_fateful = p_fatefulEncounter;

    setForme( p_forme );
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

    m_abilitySlot = 2 * p_hiddenAbility + ( m_pid & 1 );
    setSpecies( p_pkmnId, &data );
}

bool boxPokemon::isFullyEvolved( ) const {
    auto edata = getPkmnEvolveData( getSpecies( ), getForme( ) );
    return !!edata.m_evolutionCount;
}

bool boxPokemon::isForeign( ) const {
    if( std::strcmp( m_oT, SAVE::SAV.getActiveFile( ).m_playername ) ) { return true; }
    if( m_oTId != SAVE::SAV.getActiveFile( ).m_id ) { return true; }
    if( m_oTSid != SAVE::SAV.getActiveFile( ).m_sid ) { return true; }
    return false;
}

void boxPokemon::setSpecies( u16 p_newSpecies, pkmnData* p_data ) {
    m_speciesId = p_newSpecies;
    setForme( 0 );
    pkmnData data;
    if( p_data == nullptr ) {
        data = getPkmnData( getSpecies( ), getForme( ) );
    } else {
        data = *p_data;
    }

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

    setAbility( getAbilitySlot( ), &data );
    recalculateForme( );
}

void boxPokemon::setAbility( u8 p_newAbilitySlot, pkmnData* p_data ) {
    pkmnData data;
    if( p_data == nullptr ) {
        data = getPkmnData( getSpecies( ), getForme( ) );
    } else {
        data = *p_data;
    }

    m_abilitySlot = p_newAbilitySlot;

    m_ability = ( p_newAbilitySlot >= 2 && data.m_baseForme.m_abilities[ 2 ] )
                    ? ( ( ( p_newAbilitySlot & 1 ) || !data.m_baseForme.m_abilities[ 3 ] )
                            ? data.m_baseForme.m_abilities[ 2 ]
                            : data.m_baseForme.m_abilities[ 3 ] )
                    : ( ( ( p_newAbilitySlot & 1 ) || !data.m_baseForme.m_abilities[ 1 ] )
                            ? data.m_baseForme.m_abilities[ 0 ]
                            : data.m_baseForme.m_abilities[ 1 ] );
}

void boxPokemon::recalculateForme( ) {
    switch( m_speciesId ) {
    case PKMN_GIRATINA: {
        setForme( ( m_heldItem == I_GRISEOUS_ORB ) );
        return;
    }
    case PKMN_ARCEUS: {
        if( m_heldItem >= I_FLAME_PLATE && m_heldItem <= I_IRON_PLATE ) {
            setForme( m_heldItem - I_FLAME_PLATE + 1 );
        } else if( m_heldItem == I_NULL_PLATE ) {
            setForme( 17 );
        } else if( m_heldItem == I_PIXIE_PLATE ) {
            setForme( 18 );
        } else if( m_heldItem >= I_FIRIUM_Z && m_heldItem <= I_STEELIUM_Z ) {
            setForme( m_heldItem - I_FIRIUM_Z + 1 );
        } else if( m_heldItem == I_FAIRIUM_Z ) {
            setForme( 18 );
        } else if( m_heldItem >= I_FIRIUM_Z2 && m_heldItem <= I_STEELIUM_Z2 ) {
            setForme( m_heldItem - I_FIRIUM_Z2 + 1 );
        } else if( m_heldItem == I_FAIRIUM_Z2 ) {
            setForme( 18 );
        } else {
            setForme( 0 );
        }
        return;
    }
    case PKMN_GENESECT: {
        if( m_heldItem >= I_DOUSE_DRIVE && m_heldItem <= I_CHILL_DRIVE ) {
            setForme( m_heldItem - I_DOUSE_DRIVE + 1 );
        } else {
            setForme( 0 );
        }
        return;
    }
    case PKMN_SILVALLY: {
        if( m_heldItem >= I_FIGHTING_MEMORY && m_heldItem <= I_FAIRY_MEMORY ) {
            setForme( m_heldItem - I_FIGHTING_MEMORY + 1 );
        } else {
            setForme( 0 );
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
    auto old = m_ability;
    setAbility( m_abilitySlot ^ 1 );
    if( old == m_ability ) {
        m_abilitySlot ^= 1;
        return false;
    }
    return true;
}

void boxPokemon::hatch( ) {
    setIsEgg( false );
    m_hatchPlace     = MAP::curMap->getCurrentLocationId( );
    m_hatchDate[ 0 ] = SAVE::CURRENT_DATE.m_day;
    m_hatchDate[ 1 ] = SAVE::CURRENT_DATE.m_month;
    m_hatchDate[ 2 ] = SAVE::CURRENT_DATE.m_year % 100;
}

bool boxPokemon::learnMove( u16 p_move, std::function<void( const char* )> p_message,
                            std::function<u8( boxPokemon*, u16 )> p_getMove,
                            std::function<bool( const char* )>    p_yesNoMessage ) {
    char buffer[ 50 ];
    if( p_move == m_moves[ 0 ] || p_move == m_moves[ 1 ] || p_move == m_moves[ 2 ]
        || p_move == m_moves[ 3 ] ) {
        snprintf( buffer, 49, GET_STRING( 102 ), m_name, MOVE::getMoveName( p_move ).c_str( ) );
        p_message( buffer );
        return false;
    } else if( canLearn( m_speciesId, p_move, LEARN_TM ) ) {
        auto mdata    = MOVE::getMoveData( p_move );
        bool freeSpot = false;
        for( u8 i = 0; i < 4; ++i )
            if( !m_moves[ i ] ) {
                m_moves[ i ] = p_move;
                m_curPP[ i ] = std::min( m_curPP[ i ], mdata.m_pp );

                snprintf( buffer, 49, GET_STRING( 103 ), m_name,
                          MOVE::getMoveName( p_move ).c_str( ) );
                p_message( buffer );
                freeSpot = true;
                break;
            }
        if( !freeSpot ) {
            snprintf( buffer, 49, GET_STRING( 104 ), m_name );
            if( p_yesNoMessage( buffer ) ) {
                u8 res = p_getMove( this, p_move );
                if( res < 4 ) {
                    if( MOVE::isFieldMove( m_moves[ res ] ) ) {
                        snprintf( buffer, 49, GET_STRING( 106 ), m_name,
                                  MOVE::getMoveName( m_moves[ res ] ).c_str( ) );
                        p_message( buffer );
                        return false;
                    } else {
                        m_moves[ res ] = p_move;
                        m_curPP[ res ] = std::min( m_curPP[ res ], mdata.m_pp );
                    }
                    return true;
                }
            }
            snprintf( buffer, 49, GET_STRING( 403 ), m_name, MOVE::getMoveName( p_move ).c_str( ) );
            p_message( buffer );
            return false;
        }
    } else {
        snprintf( buffer, 49, GET_STRING( 107 ), m_name, MOVE::getMoveName( p_move ).c_str( ) );
        p_message( buffer );
        return false;
    }
    return true;
}
