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

#include "pokemon.h"
#include "itemNames.h"
#include "mapDrawer.h"
#include "move.h"
#include "pokemonNames.h"
#include "saveGame.h"

pokemon::pokemon( boxPokemon& p_boxPokemon ) : m_boxdata( p_boxPokemon ) {
    pkmnData data       = getPkmnData( p_boxPokemon.m_speciesId, p_boxPokemon.getForme( ) );
    m_level             = calcLevel( p_boxPokemon, &data );
    m_stats             = calcStats( m_boxdata, m_level, &data );
    m_battleForme = 0;
    m_statusint = 0;
}
pokemon::pokemon( u16 p_pkmnId, u16 p_level, u8 p_forme, const char* p_name, u8 p_shiny,
                  bool p_hiddenAbility, bool p_isEgg, u8 p_ball, u8 p_pokerus,
                  bool p_fatefulEncounter ) {
    pkmnData data       = getPkmnData( p_pkmnId, p_forme );
    m_boxdata           = boxPokemon( p_pkmnId, p_level, p_forme, p_name, p_shiny,
            p_hiddenAbility, p_isEgg, p_ball, p_pokerus,  p_fatefulEncounter, &data );
    m_level             = p_level;
    m_stats             = calcStats( m_boxdata, p_level, &data );
    m_battleForme = 0;
    m_statusint = 0;
}
pokemon::pokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
                  const char* p_oT, bool p_oTFemale, u8 p_shiny, bool p_hiddenAbility,
                  bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace, u8 p_ball, u8 p_pokerus,
                  u8 p_forme ) {
    pkmnData data       = getPkmnData( p_pkmnId, p_forme );
    m_boxdata           = boxPokemon( p_moves, p_pkmnId, p_name, p_level, p_id,
            p_sid, p_oT, p_oTFemale, p_shiny, p_hiddenAbility, p_fatefulEncounter, p_isEgg,
            p_gotPlace, p_ball, p_pokerus, p_forme, &data );
    m_level             = p_level;
    m_stats             = calcStats( m_boxdata, p_level, &data );
    m_battleForme = 0;
    m_statusint = 0;
}
pokemon::pokemon( trainerPokemon& p_trainerPokemon ) {
    pkmnData data = getPkmnData( p_trainerPokemon.m_speciesId, p_trainerPokemon.m_forme );
    m_level       = p_trainerPokemon.m_level;
    m_boxdata     = boxPokemon( p_trainerPokemon.m_speciesId, m_level, p_trainerPokemon.m_forme, 0,
                            p_trainerPokemon.m_shiny, false, false, 0, 0, false, &data );

    m_boxdata.m_ability  = p_trainerPokemon.m_ability;
    m_boxdata.m_heldItem = p_trainerPokemon.m_heldItem;
    memcpy( m_boxdata.m_moves, p_trainerPokemon.m_moves, sizeof( m_boxdata.m_moves ) );
    memcpy( m_boxdata.m_effortValues, p_trainerPokemon.m_ev, sizeof( m_boxdata.m_effortValues ) );
    for( u8 i = 0; i < 6; ++i ) {
        m_boxdata.IVset( i, p_trainerPokemon.m_iv[ i ] );
    }
    m_stats             = calcStats( m_boxdata, m_level, &data );
    m_battleForme = 0;
    m_statusint = 0;
}

bool pokemon::heal( ) {
    bool change    = m_stats.m_acHP < m_stats.m_maxHP;
    m_stats.m_acHP = m_stats.m_maxHP;

    change |= !!m_statusint;
    m_statusint = 0;

    for( u8 i = 0; i < 4; ++i ) {
        if( m_boxdata.m_moves[ i ] ) {
            MOVE::moveData mdata = MOVE::getMoveData( m_boxdata.m_moves[ i ] );
            auto           mx    = s8( mdata.m_pp * ( ( 5 + m_boxdata.PPupget( i ) ) / 5.0 ) );
            change |= m_boxdata.m_acPP[ i ] < mx;
            m_boxdata.m_acPP[ i ] = mx;
        }
    }

    return change;
}

bool pokemon::canBattleTransform( ) const {
    // TODO: Check whether pkmn can mega evolve etc

    return false;
}

void pokemon::battleTransform( ) {
    // TODO: Perform mega evolution etc
}

void pokemon::revertBattleTransform( ) {
    m_battleForme = 0;
}

void pokemon::recalculateStats( ) {
    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );
    recalculateStats( data );
}

void pokemon::recalculateStats( pkmnData& p_data ) {
    auto HPdif     = m_stats.m_maxHP - m_stats.m_acHP;
    m_stats        = calcStats( m_boxdata, m_level, &p_data );
    if( m_stats.m_maxHP < HPdif ) {
        m_stats.m_acHP = 0;
    } else {
        m_stats.m_acHP = m_stats.m_maxHP - HPdif;
    }
}

void pokemon::setForme( u8 p_newForme ) {
    m_boxdata.setForme( p_newForme );
    recalculateStats( );
}

void pokemon::setBattleForme( u8 p_newForme ) {
    m_boxdata.setForme( 0 );
    m_battleForme = p_newForme;
    recalculateStats( );
}

bool pokemon::setNature( pkmnNatures p_newNature ) {
    if( !m_boxdata.setNature( p_newNature ) ) { return false; }
    recalculateStats( );
    return true;
}

bool pokemon::setLevel( u8 p_newLevel ) {
    if( p_newLevel > 100 || p_newLevel == m_level ) { return false; }
    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );

    m_boxdata.m_experienceGained = EXP[ p_newLevel - 1 ][ data.getExpType( ) ];
    m_level                      = calcLevel( m_boxdata, &data );
    recalculateStats( data );

    return true;
}

bool pokemon::gainExperience( u32 p_amount ) {
    if( m_level == 100 ) { return false; }

    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );

    m_boxdata.m_experienceGained
        = std::min( m_boxdata.m_experienceGained + p_amount, EXP[ 99 ][ data.getExpType( ) ] );
    m_level = calcLevel( m_boxdata, &data );
    recalculateStats( data );
    return true;
}

void pokemon::setStatus( u8 p_status, u8 p_value ) {
    switch( p_status ) {
    case 0:
        m_status.m_isAsleep = p_value;
        break;
    case 1:
        m_status.m_isPoisoned = p_value;
        break;
    case 2:
        m_status.m_isBurned = p_value;
        break;
    case 3:
        m_status.m_isFrozen = p_value;
        if( p_value && m_boxdata.m_speciesId == PKMN_SHAYMIN && getForme( ) == 1 ) {
            setForme( 0 );
        }
        break;
    case 4:
        m_status.m_isParalyzed = p_value;
        break;
    case 5:
        m_status.m_isBadlyPoisoned = p_value;
        break;
    default:
        break;
    }
}

pokemon::stats calcStats( const boxPokemon& p_boxdata, u8 p_level, const pkmnData* p_data ) {
    pokemon::stats res;
    u16            pkmnId = p_boxdata.m_speciesId;
    if( pkmnId != PKMN_SHEDINJA )
        res.m_acHP = res.m_maxHP = ( ( p_boxdata.IVget( 0 ) + 2 * p_data->m_baseForme.m_bases[ 0 ]
                                       + ( p_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )
                                     * p_level / 100 )
                                   + 10;
    else
        res.m_acHP = res.m_maxHP = 1;
    pkmnNatures nature = p_boxdata.getNature( );

    for( u8 i = 1; i < 6; ++i ) {
        res.setStat( i, ( ( ( p_boxdata.IVget( i ) + 2 * p_data->m_baseForme.m_bases[ i ]
                              + ( p_boxdata.m_effortValues[ i ] >> 2 ) )
                            * p_level / 100.0 )
                          + 5 )
                            * NatMod[ nature ][ i - 1 ] );
    }
    return res;
}
pokemon::stats calcStats( const boxPokemon& p_boxdata, const pkmnData* p_data ) {
    return calcStats( p_boxdata, calcLevel( p_boxdata, p_data ), p_data );
}
u16 calcLevel( const boxPokemon& p_boxdata, const pkmnData* p_data ) {
    for( u16 i = 2; i < 101; ++i )
        if( EXP[ i - 1 ][ p_data->getExpType( ) ] > p_boxdata.m_experienceGained ) return ( i - 1 );
    return 100;
}

void pokemon::giveItem( u16 p_newItem ) {
    m_boxdata.giveItem( p_newItem );
    pkmnData data  = getPkmnData( m_boxdata.m_speciesId, m_boxdata.getForme( ) );
    auto     oldHP = m_stats.m_maxHP - m_stats.m_acHP;
    m_stats        = calcStats( m_boxdata, m_level, &data );
    m_stats.m_acHP = m_stats.m_maxHP - oldHP;
}
u16 pokemon::takeItem( ) {
    u16      res   = m_boxdata.takeItem( );
    pkmnData data  = getPkmnData( m_boxdata.m_speciesId, m_boxdata.getForme( ) );
    auto     oldHP = m_stats.m_maxHP - m_stats.m_acHP;
    m_stats        = calcStats( m_boxdata, m_level, &data );
    m_stats.m_acHP = m_stats.m_maxHP - oldHP;
    return res;
}

bool pokemon::canEvolve( u16 p_item, u16 p_method ) {
    if( isEgg( ) ) return false;
    if( getItem( ) == I_EVERSTONE ) return false;

    pokemonData data;
    getAll( m_boxdata.m_speciesId, data, getForme( ) );

    for( int i = 0; i < 7; ++i ) {
        if( m_level < data.m_evolutions[ i ].m_e.m_evolveLevel ) continue;
        if( m_boxdata.m_steps < data.m_evolutions[ i ].m_e.m_evolveFriendship ) continue;
        if( data.m_evolutions[ i ].m_e.m_evolveItem
            && p_item != data.m_evolutions[ i ].m_e.m_evolveItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveDayTime != -1
            && getCurrentDaytime( ) != data.m_evolutions[ i ].m_e.m_evolveDayTime )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolvesInto == 0 ) continue;
        if( data.m_evolutions[ i ].m_e.m_evolveGender
            && m_boxdata.gender( ) != data.m_evolutions[ i ].m_e.m_evolveGender )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveLocation
            && MAP::curMap->getCurrentLocationId( ) != data.m_evolutions[ i ].m_e.m_evolveLocation )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveHeldItem
            && m_boxdata.m_heldItem != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_moves[ j ] );
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
            bool b = false;
            for( int j = 0; j < 4; ++j ) {
                MOVE::moveData mdata = MOVE::getMoveData( m_boxdata.m_moves[ j ] );
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType == mdata.m_category );
            }
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty
            && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_contestStats[ 1 ] )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
            bool b = false;
            for( int j = 0; j < 6; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember
                       == SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId );
            if( !b ) continue;
        }
        if( p_method != data.m_evolutions[ i ].m_e.m_evolutionTrigger ) continue;
        return true;
    }
    return false;
}
void pokemon::evolve( u16 p_item, u16 p_method ) {
    if( isEgg( ) ) return;
    if( getItem( ) == I_EVERSTONE ) return;

    pokemonData data;
    getAll( m_boxdata.m_speciesId, data );

    int into = 0;

    for( int i = 0; i < 7; ++i ) {
        if( m_level < data.m_evolutions[ i ].m_e.m_evolveLevel ) continue;
        if( m_boxdata.m_steps < data.m_evolutions[ i ].m_e.m_evolveFriendship ) continue;
        if( data.m_evolutions[ i ].m_e.m_evolveItem
            && p_item != data.m_evolutions[ i ].m_e.m_evolveItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveDayTime != -1
            && getCurrentDaytime( ) != data.m_evolutions[ i ].m_e.m_evolveDayTime )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolvesInto == 0 ) continue;
        if( data.m_evolutions[ i ].m_e.m_evolveGender
            && m_boxdata.gender( ) != data.m_evolutions[ i ].m_e.m_evolveGender )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveLocation
            && MAP::curMap->getCurrentLocationId( ) != data.m_evolutions[ i ].m_e.m_evolveLocation )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveHeldItem
            && m_boxdata.m_heldItem != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_moves[ j ] );
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
            bool b = false;
            for( int j = 0; j < 4; ++j ) {
                MOVE::moveData mdata = MOVE::getMoveData( m_boxdata.m_moves[ j ] );
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType == mdata.m_category );
            }
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty
            && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_contestStats[ 1 ] )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
            bool b = false;
            for( int j = 0; j < 6; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember
                       == SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId );
            if( !b ) continue;
        }
        if( p_method != data.m_evolutions[ i ].m_e.m_evolutionTrigger ) continue;
        into = data.m_evolutions[ i ].m_e.m_evolvesInto;
        break;
    }
    if( into == 0 ) return;

    m_boxdata.m_speciesId = into;
    recalculateStats( );

    if( !m_boxdata.isNicknamed( ) )
        strcpy( m_boxdata.m_name,
                getDisplayName( m_boxdata.m_speciesId, CURRENT_LANGUAGE ).c_str( ) );
}
