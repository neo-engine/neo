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
#include "move.h"
#include "saveGame.h"
#include "mapDrawer.h"
#include "pokemonNames.h"

bool boxPokemon::isShiny( ) const {
    return !( ( ( ( m_oTId ^ m_oTSid ) >> 3 )
                ^ ( ( ( m_pid >> 16 ) ^ ( m_pid % ( 1 << 16 ) ) ) ) >> 3 ) );
}

/*
*@ Returns the gender. -1 for female, 0 for genderless, and 1 for male
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
            m_altForme = ( m_holdItem == I_GRISEOUS_ORB );
            return;
        }
        case PKMN_ARCEUS: {
            if( m_holdItem >= I_FLAME_PLATE && m_holdItem <= I_IRON_PLATE ) {
                m_altForme = m_holdItem - I_FLAME_PLATE + 1;
            } else if( m_holdItem == I_NULL_PLATE ) {
                m_altForme = 17;
            } else if( m_holdItem == I_PIXIE_PLATE ) {
                m_altForme = 18;
            } else if( m_holdItem >= I_FIRIUM_Z && m_holdItem <= I_STEELIUM_Z ) {
                m_altForme = m_holdItem - I_FIRIUM_Z + 1;
            } else if( m_holdItem == I_FAIRIUM_Z ) {
                m_altForme = 18;
            } else if( m_holdItem >= I_FIRIUM_Z2 && m_holdItem <= I_STEELIUM_Z2 ) {
                m_altForme = m_holdItem - I_FIRIUM_Z2 + 1;
            } else if( m_holdItem == I_FAIRIUM_Z2 ) {
                m_altForme = 18;
            } else {
                m_altForme = 0;
            }
            return;
        }
        case PKMN_GENESECT: {
            if( m_holdItem >= I_DOUSE_DRIVE && m_holdItem <= I_CHILL_DRIVE ) {
                m_altForme = m_holdItem - I_DOUSE_DRIVE + 1;
            } else {
                m_altForme = 0;
            }
            return;
        }
        case PKMN_SILVALLY: {
            if( m_holdItem >= I_FIGHTING_MEMORY && m_holdItem <= I_FAIRY_MEMORY ) {
                m_altForme = m_holdItem - I_FIGHTING_MEMORY + 1;
            } else {
                m_altForme = 0;
            }
            return;
        }

    }
}

bool pokemon::heal( ) {
    bool change = m_stats.m_acHP < m_stats.m_maxHP;
    m_stats.m_acHP = m_stats.m_maxHP;

    change |= !!m_statusint;
    m_statusint = 0;

    for( u8 i = 0; i < 4; ++i ) {
        if( m_boxdata.m_moves[ i ] ) {
            auto mx = s8( AttackList[ m_boxdata.m_moves[ i ] ]->m_movePP
                    * ( ( 5 + m_boxdata.PPupget( i ) ) / 5.0 ) );
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
    auto HPdif      = m_stats.m_maxHP - m_stats.m_acHP;
    m_stats         = calcStats( m_boxdata, m_level, &p_data );
    m_stats.m_acHP  = m_stats.m_maxHP - HPdif;
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

bool boxPokemon::setNature( pkmnNatures p_newNature ) {
    if( getNature( ) == p_newNature ) {
        return false;
    }
    bool shiny = isShiny( );
    for( ; getNature( ) != p_newNature || isShiny( ) != shiny; m_pid = rand( ) );
    return true;
}

bool pokemon::setNature( pkmnNatures p_newNature ) {
    if( !m_boxdata.setNature( p_newNature ) ) {
        return false;
    }
    recalculateStats( );
    return true;
}

bool pokemon::setLevel( u8 p_newLevel ) {
    if( p_newLevel > 100 || p_newLevel == m_level ) {
        return false;
    }
    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );

    m_boxdata.m_experienceGained = EXP[ p_newLevel - 1 ][ data.getExpType( ) ];
    m_level = calcLevel( m_boxdata, &data );
    recalculateStats( data );

    return true;
}

bool pokemon::gainExperience( u32 p_amount ) {
    if( m_level == 100 ) {
        return false;
    }

    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );

    m_boxdata.m_experienceGained = std::min( m_boxdata.m_experienceGained + p_amount,
        EXP[ 99 ][ data.getExpType( ) ] );
    m_level = calcLevel( m_boxdata, &data );
    recalculateStats( data );
    return true;
}

void pokemon::setStatus( u8 p_status, u8 p_value ) {
    switch( p_status ) {
        case 0:
            m_status.m_isAsleep = p_value; break;
        case 1:
            m_status.m_isPoisoned = p_value; break;
        case 2:
            m_status.m_isBurned = p_value; break;
        case 3:
            m_status.m_isFrozen = p_value;
            if( p_value && m_boxdata.m_speciesId == PKMN_SHAYMIN && getForme( ) == 1 ) {
                setForme( 0 );
            }
            break;
        case 4:
            m_status.m_isParalyzed = p_value; break;
        case 5:
            m_status.m_isBadlyPoisoned = p_value; break;
        default:
            break;
    }
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

boxPokemon::boxPokemon( u16 p_pkmnId, u16 p_level, u8 p_forme, const char* p_name,
                                 u8 p_shiny, bool p_hiddenAbility, bool p_isEgg,
                                 u8 p_ball, u8 p_pokerus, bool p_fatefulEncounter,
                                 pkmnData* p_data ) :
    boxPokemon::boxPokemon( nullptr, p_pkmnId, p_name, p_level,
            SAVE::SAV->getActiveFile( ).m_id, SAVE::SAV->getActiveFile( ).m_sid,
            SAVE::SAV->getActiveFile( ).m_playername, !SAVE::SAV->getActiveFile( ).m_isMale,
            p_shiny, p_hiddenAbility, p_fatefulEncounter, p_isEgg,
            MAP::curMap->getCurrentLocationId( ), p_ball, p_pokerus, p_forme, p_data ) { }

boxPokemon::boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name,
                                 u16 p_level, u16 p_id, u16 p_sid, const char* p_oT,
                                 bool p_oTFemale, u8 p_shiny, bool p_hiddenAbility,
                                 bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace,
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
        m_holdItem = data.m_baseForme.m_items[ 3 ];
    else {
        u8 m_b1 = rand( ) % 100;
        if( m_b1 < 5 && data.m_baseForme.m_items[ 0 ] )
            m_holdItem = data.m_baseForme.m_items[ 0 ];
        else if( m_b1 < 20 && data.m_baseForme.m_items[ 1 ] )
            m_holdItem = data.m_baseForme.m_items[ 1 ];
        else if( m_b1 < 80 && data.m_baseForme.m_items[ 2 ] )
            m_holdItem = data.m_baseForme.m_items[ 2 ];
    }

    if( !p_isEgg )
        m_experienceGained = EXP[ p_level - 1 ][ data.getExpType( ) ];

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
        getLearnMoves( p_pkmnId, p_level, 0, 1, 4, m_moves );
    for( u8 i = 0; i < 4; ++i ) m_acPP[ i ] = ( u8 )( AttackList[ m_moves[ i ] ]->m_movePP );

    for( u8 i = 0; i < 6; ++i )
        IVset( i, rand( ) & 31 );
    setIsNicknamed( !!p_name );
    setIsEgg( p_isEgg );
    m_fateful                     = p_fatefulEncounter;

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

pokemon::stats calcStats( const boxPokemon& p_boxdata, u8 p_level,
                          const pkmnData* p_data ) {
    pokemon::stats res;
    u16            pkmnId = p_boxdata.m_speciesId;
    if( pkmnId != PKMN_SHEDINJA )
        res.m_acHP = res.m_maxHP = ( ( p_boxdata.IVget( 0 ) + 2 *
                p_data->m_baseForme.m_bases[ 0 ]
                                       + ( p_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )
                                     * p_level / 100 )
                                   + 10;
    else
        res.m_acHP = res.m_maxHP = 1;
    pkmnNatures nature = p_boxdata.getNature( );

    for( u8 i = 1; i < 6; ++i ) {
        res.setStat( i, ( ( ( p_boxdata.IVget( i ) + 2
                    * p_data->m_baseForme.m_bases[ i ]
                      + ( p_boxdata.m_effortValues[ i ] >> 2 ) )
                    * p_level / 100.0 )
                  + 5 ) * NatMod[ nature ][ i - 1 ] );
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
    pkmnData data = getPkmnData( m_boxdata.m_speciesId, m_boxdata.getForme( ) );
    auto oldHP    = m_stats.m_maxHP - m_stats.m_acHP;
    m_stats       = calcStats( m_boxdata, m_level, &data );
    m_stats.m_acHP  = m_stats.m_maxHP - oldHP;
}
u16 pokemon::takeItem( ) {
    u16 res       = m_boxdata.takeItem( );
    pkmnData data = getPkmnData( m_boxdata.m_speciesId, m_boxdata.getForme( ) );
    auto oldHP    = m_stats.m_maxHP - m_stats.m_acHP;
    m_stats       = calcStats( m_boxdata, m_level, &data );
    m_stats.m_acHP  = m_stats.m_maxHP - oldHP;
    return res;
}


pokemon::pokemon( boxPokemon& p_boxPokemon ) : m_boxdata( p_boxPokemon ) {
    pkmnData data       = getPkmnData( p_boxPokemon.m_speciesId, p_boxPokemon.getForme( ) );
    m_level             = calcLevel( p_boxPokemon, &data );
    m_stats             = calcStats( m_boxdata, m_level, &data );
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
    m_statusint = 0;
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
            && m_boxdata.m_holdItem != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_moves[ j ] );
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType
                       == AttackList[ m_boxdata.m_moves[ j ] ]->m_moveType );
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty
            && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_contestStats[ 1 ] )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
            bool b = false;
            for( int j = 0; j < 6; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember
                       == SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId );
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
            && m_boxdata.m_holdItem != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_moves[ j ] );
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType
                       == AttackList[ m_boxdata.m_moves[ j ] ]->m_moveType );
            if( !b ) continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty
            && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_contestStats[ 1 ] )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
            bool b = false;
            for( int j = 0; j < 6; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember
                       == SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId );
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
        strcpy( m_boxdata.m_name, getDisplayName( m_boxdata.m_speciesId, CURRENT_LANGUAGE ).c_str( ) );
}

void boxPokemon::hatch( ) {
    setIsEgg( false );
    m_hatchPlace               = MAP::curMap->getCurrentLocationId( );
    m_hatchDate[ 0 ]           = acday;
    m_hatchDate[ 1 ]           = acmonth + 1;
    m_hatchDate[ 2 ]           = ( acyear + 1900 ) % 100;
}

