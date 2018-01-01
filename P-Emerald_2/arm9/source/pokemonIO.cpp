/*
Pokémon Emerald 2 Version
------------------------------

file        : pokemonIO.cpp
author      : Philip Wellnitz
description : Methods for pokémon which require user IO o

Copyright (C) 2012 - 2018
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

#include "choiceBox.h"
#include "defines.h"
#include "mapDrawer.h"
#include "messageBox.h"
#include "moveChoiceBox.h"
#include "pokemon.h"
#include "saveGame.h"
#include "uio.h"
#include "yesNoBox.h"

#include <algorithm>
#include <ctime>

pokemonData data;
pokemon::boxPokemon::boxPokemon( u16 p_pkmnId, u16 p_level, const char* p_name, u8 p_shiny,
                                 bool p_hiddenAbility, bool p_isEgg, u8 p_pokerus,
                                 bool p_fatefulEncounter ) {
    getAll( p_pkmnId, data );
    m_pid   = rand( );
    m_oTId  = SAVE::SAV->getActiveFile( ).m_id;
    m_oTSid = SAVE::SAV->getActiveFile( ).m_sid;
    if( p_shiny == 2 )
        while( !isShiny( ) || isCloned( ) ) m_pid = rand( );
    else if( p_shiny == 1 )
        while( isShiny( ) || isCloned( ) ) m_pid = rand( );
    else if( p_shiny ) { // Try p_shiny - 2 additional times to generate a shiny PId
        for( u8 i = 0; i < p_shiny - 2 && !isShiny( ) && isCloned( ); ++i ) m_pid = rand( );
    }
    m_checksum  = 0;
    m_speciesId = p_pkmnId;

    if( data.m_items[ 3 ] )
        m_holdItem = data.m_items[ 3 ];
    else {
        m_b1 = rand( ) % 100;
        if( m_b1 < 5 && data.m_items[ 0 ] )
            m_holdItem = data.m_items[ 0 ];
        else if( m_b1 < 20 && data.m_items[ 1 ] )
            m_holdItem = data.m_items[ 1 ];
        else if( m_b1 < 80 && data.m_items[ 2 ] )
            m_holdItem = data.m_items[ 2 ];
        else
            m_holdItem = 0;
    }

    if( !p_isEgg )
        m_experienceGained = EXP[ p_level - 1 ][ data.m_expType ];
    else
        m_experienceGained = 0;

    if( p_isEgg ) {
        m_steps          = data.m_eggcyc;
        m_gotDate[ 0 ]   = acday;
        m_gotDate[ 1 ]   = acmonth;
        m_gotDate[ 2 ]   = acyear % 100;
        m_gotPlace       = MAP::curMap->getCurrentLocationId( );
        m_hatchDate[ 0 ] = m_hatchDate[ 1 ] = m_hatchDate[ 2 ] = m_hatchPlace = 0;
    } else {
        m_steps        = data.m_baseFriend;
        m_gotDate[ 0 ] = m_gotDate[ 1 ] = m_gotDate[ 2 ] = m_hatchPlace = 0;
        m_hatchDate[ 0 ]                                                = acday;
        m_hatchDate[ 1 ]                                                = acmonth;
        m_hatchDate[ 2 ]                                                = acyear % 100;
        m_gotPlace = MAP::curMap->getCurrentLocationId( );
    }

    m_ability = ( p_hiddenAbility && data.m_abilities[ 2 ] )
                    ? ( ( ( m_pid & 1 ) || !data.m_abilities[ 3 ] ) ? data.m_abilities[ 2 ]
                                                                    : data.m_abilities[ 3 ] )
                    : ( ( ( m_pid & 1 ) || !data.m_abilities[ 1 ] ) ? data.m_abilities[ 0 ]
                                                                    : data.m_abilities[ 1 ] );
    m_markings = 0;

    m_origLang = 5;

    memset( m_effortValues, 0, sizeof( m_effortValues ) );
    memset( m_contestStats, 0, sizeof( m_contestStats ) );
    memset( m_ribbons0, 0, sizeof( m_ribbons0 ) );
    memset( m_ribbons1, 0, sizeof( m_ribbons1 ) );
    memset( m_ribbons2, 0, sizeof( m_ribbons2 ) );
    getLearnMoves( p_pkmnId, p_level, 0, 1, 4, m_moves );
    for( u8 i = 0; i < 4; ++i ) m_acPP[ i ] = ( u8 )( AttackList[ m_moves[ i ] ]->m_movePP );

    m_ppup.m_Up1                  = 0;
    m_ppup.m_Up2                  = 0;
    m_ppup.m_Up3                  = 0;
    m_ppup.m_Up4                  = 0;
    m_individualValues.m_attack   = rand( ) & 31;
    m_individualValues.m_defense  = rand( ) & 31;
    m_individualValues.m_hp       = rand( ) & 31;
    m_individualValues.m_sAttack  = rand( ) & 31;
    m_individualValues.m_sDefense = rand( ) & 31;
    m_individualValues.m_speed    = rand( ) & 31;
    m_individualValues.m_isEgg    = p_isEgg;
    m_fateful                     = p_fatefulEncounter;

    pkmnGenderType A = data.m_gender;
    if( A == MALE )
        m_isFemale = m_isGenderless = false;
    else if( A == FEMALE )
        m_isFemale = true, m_isGenderless = false;
    else if( A == GENDERLESS )
        m_isFemale = false, m_isGenderless = true;
    else if( ( m_pid % 256 ) >= A )
        m_isFemale = m_isGenderless = false;
    else
        m_isFemale = true, m_isGenderless = false;

    m_altForme = 0;
    m_cloned   = false;
    if( p_name ) {
        strcpy( m_name, p_name );
        m_individualValues.m_isNicked = true;
    } else {
        strcpy( m_name, data.m_displayName );
        m_individualValues.m_isNicked = false;
    }
    m_hometown = 4;
    strcpy( m_oT, SAVE::SAV->getActiveFile( ).m_playername );
    m_pokerus    = p_pokerus;
    m_ball       = 0;
    m_gotLevel   = p_level;
    m_oTisFemale = !SAVE::SAV->getActiveFile( ).m_isMale;
    m_encounter  = (encounter) 0;
    m_HGSSBall   = 0;
}
pokemon::boxPokemon::boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level,
                                 u16 p_id, u16 p_sid, const char* p_oT, bool p_oTFemale, u8 p_shiny,
                                 bool p_hiddenAbility, bool p_fatefulEncounter, bool p_isEgg,
                                 u16 p_gotPlace, u8 p_ball, u8 p_pokerus ) {
    getAll( p_pkmnId, data );

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
    m_checksum  = 0;
    m_speciesId = p_pkmnId;

    if( data.m_items[ 3 ] )
        m_holdItem = data.m_items[ 3 ];
    else {
        m_b1 = rand( ) % 100;
        if( m_b1 < 5 && data.m_items[ 0 ] )
            m_holdItem = data.m_items[ 0 ];
        else if( m_b1 < 20 && data.m_items[ 1 ] )
            m_holdItem = data.m_items[ 1 ];
        else if( m_b1 < 80 && data.m_items[ 2 ] )
            m_holdItem = data.m_items[ 2 ];
        else
            m_holdItem = 0;
    }

    if( !p_isEgg )
        m_experienceGained = EXP[ p_level - 1 ][ data.m_expType ];
    else
        m_experienceGained = 0;

    if( p_isEgg ) {
        m_steps          = data.m_eggcyc;
        m_gotDate[ 0 ]   = acday;
        m_gotDate[ 1 ]   = acmonth;
        m_gotDate[ 2 ]   = acyear % 100;
        m_gotPlace       = p_gotPlace;
        m_hatchDate[ 0 ] = m_hatchDate[ 1 ] = m_hatchDate[ 2 ] = m_hatchPlace = 0;
    } else {
        m_steps        = data.m_baseFriend;
        m_gotDate[ 0 ] = m_gotDate[ 1 ] = m_gotDate[ 2 ] = m_hatchPlace = 0;
        m_hatchDate[ 0 ]                                                = acday;
        m_hatchDate[ 1 ]                                                = acmonth;
        m_hatchDate[ 2 ]                                                = acyear % 100;
        m_gotPlace                                                      = p_gotPlace;
    }

    m_ability = ( p_hiddenAbility && data.m_abilities[ 2 ] )
                    ? ( ( ( m_pid & 1 ) || !data.m_abilities[ 3 ] ) ? data.m_abilities[ 2 ]
                                                                    : data.m_abilities[ 3 ] )
                    : ( ( ( m_pid & 1 ) || !data.m_abilities[ 1 ] ) ? data.m_abilities[ 0 ]
                                                                    : data.m_abilities[ 1 ] );
    m_markings = 0;
    m_origLang = 5;

    memset( m_effortValues, 0, sizeof( m_effortValues ) );
    memset( m_contestStats, 0, sizeof( m_contestStats ) );
    memset( m_ribbons0, 0, sizeof( m_ribbons0 ) );
    memset( m_ribbons1, 0, sizeof( m_ribbons1 ) );
    memset( m_ribbons2, 0, sizeof( m_ribbons2 ) );

    if( p_moves )
        memcpy( m_moves, p_moves, sizeof( m_moves ) );
    else
        getLearnMoves( p_pkmnId, p_level, 0, 1, 4, m_moves );
    for( u8 i = 0; i < 4; ++i ) m_acPP[ i ] = ( u8 )( AttackList[ m_moves[ i ] ]->m_movePP );

    m_ppup.m_Up1                  = 0;
    m_ppup.m_Up2                  = 0;
    m_ppup.m_Up3                  = 0;
    m_ppup.m_Up4                  = 0;
    m_individualValues.m_attack   = rand( ) % 32;
    m_individualValues.m_defense  = rand( ) % 32;
    m_individualValues.m_hp       = rand( ) % 32;
    m_individualValues.m_sAttack  = rand( ) % 32;
    m_individualValues.m_sDefense = rand( ) % 32;
    m_individualValues.m_speed    = rand( ) % 32;
    m_individualValues.m_isNicked = !!p_name;
    m_individualValues.m_isEgg    = p_isEgg;
    m_fateful                     = p_fatefulEncounter;

    pkmnGenderType A = data.m_gender;
    if( A == MALE )
        m_isFemale = m_isGenderless = false;
    else if( A == FEMALE )
        m_isFemale = true, m_isGenderless = false;
    else if( A == GENDERLESS )
        m_isFemale = false, m_isGenderless = true;
    else if( ( m_pid % 256 ) >= A )
        m_isFemale = m_isGenderless = false;
    else
        m_isFemale = true, m_isGenderless = false;

    m_altForme = 0;
    if( p_name ) {
        strcpy( m_name, p_name );
        m_individualValues.m_isNicked = true;
    } else {
        strcpy( m_name, data.m_displayName );
        m_individualValues.m_isNicked = false;
    }
    m_hometown = 4;
    strcpy( m_oT, p_oT );
    m_pokerus    = p_pokerus;
    m_ball       = p_ball;
    m_gotLevel   = p_level;
    m_oTisFemale = p_oTFemale;
    m_encounter  = (encounter) 0;
    m_HGSSBall   = 0;
}

pokemon::stats calcStats( const pokemon::boxPokemon& p_boxdata, u8 p_level,
                          const pokemonData& p_data ) {
    pokemon::stats res;
    u16            pkmnId = p_boxdata.m_speciesId;
    if( pkmnId != 292 )
        res.m_acHP = res.m_maxHP = ( ( p_boxdata.m_individualValues.m_hp + 2 * p_data.m_bases[ 0 ]
                                       + ( p_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )
                                     * p_level / 100 )
                                   + 10;
    else
        res.m_acHP = res.m_maxHP = 1;
    pkmnNatures nature = p_boxdata.getNature( );
    res.m_Atk          = ( ( ( p_boxdata.m_individualValues.m_attack + 2 * p_data.m_bases[ 1 ]
                      + ( p_boxdata.m_effortValues[ 1 ] >> 2 ) )
                    * p_level / 100.0 )
                  + 5 )
                * NatMod[ nature ][ 0 ];
    res.m_Def = ( ( ( p_boxdata.m_individualValues.m_defense + 2 * p_data.m_bases[ 2 ]
                      + ( p_boxdata.m_effortValues[ 2 ] >> 2 ) )
                    * p_level / 100.0 )
                  + 5 )
                * NatMod[ nature ][ 1 ];
    res.m_Spd = ( ( ( p_boxdata.m_individualValues.m_speed + 2 * p_data.m_bases[ 5 ]
                      + ( p_boxdata.m_effortValues[ 5 ] >> 2 ) )
                    * p_level / 100.0 )
                  + 5 )
                * NatMod[ nature ][ 2 ];
    res.m_SAtk = ( ( ( p_boxdata.m_individualValues.m_sAttack + 2 * p_data.m_bases[ 3 ]
                       + ( p_boxdata.m_effortValues[ 3 ] >> 2 ) )
                     * p_level / 100.0 )
                   + 5 )
                 * NatMod[ nature ][ 3 ];
    res.m_SDef = ( ( ( p_boxdata.m_individualValues.m_sDefense + 2 * p_data.m_bases[ 4 ]
                       + ( p_boxdata.m_effortValues[ 4 ] >> 2 ) )
                     * p_level / 100.0 )
                   + 5 )
                 * NatMod[ nature ][ 4 ];
    return res;
}
pokemon::stats calcStats( const pokemon::boxPokemon& p_boxdata, const pokemonData& p_data ) {
    return calcStats( p_boxdata, calcLevel( p_boxdata, p_data ), p_data );
}
u16 calcLevel( const pokemon::boxPokemon& p_boxdata, const pokemonData& p_data ) {
    for( u16 i = 2; i < 101; ++i )
        if( EXP[ i - 1 ][ p_data.m_expType ] > p_boxdata.m_experienceGained ) return ( i - 1 );
    return 100;
}

pokemon::pokemon( pokemon::boxPokemon p_boxPokemon ) : m_boxdata( p_boxPokemon ) {
    getAll( p_boxPokemon.m_speciesId, data );
    m_level             = calcLevel( p_boxPokemon, data );
    m_stats             = calcStats( m_boxdata, m_level, data );
    m_status.m_isAsleep = m_status.m_isBurned = m_status.m_isFrozen = m_status.m_isParalyzed
        = m_status.m_isPoisoned = m_status.m_isBadlyPoisoned = false;
}
pokemon::pokemon( u16 p_pkmnId, u16 p_level, const char* p_name, u8 p_shiny, bool p_hiddenAbility,
                  bool p_isEgg, u8 p_pokerus, bool p_fatefulEncounter )
    : m_boxdata( p_pkmnId, p_level, p_name, p_shiny, p_hiddenAbility, p_isEgg, p_pokerus,
                 p_fatefulEncounter ),
      m_level( p_level ) {
    m_stats             = calcStats( m_boxdata, p_level, data );
    m_status.m_isAsleep = m_status.m_isBurned = m_status.m_isFrozen = m_status.m_isParalyzed
        = m_status.m_isPoisoned = m_status.m_isBadlyPoisoned = false;
}
pokemon::pokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
                  const char* p_oT, bool p_oTFemale, u8 p_shiny, bool p_hiddenAbility,
                  bool p_fatefulEncounter, bool p_isEgg, u16 p_gotPlace, u8 p_ball, u8 p_pokerus )
    : m_boxdata( p_moves, p_pkmnId, p_name, p_level, p_id, p_sid, p_oT, p_oTFemale, p_shiny,
                 p_hiddenAbility, p_fatefulEncounter, p_isEgg, p_gotPlace, p_ball, p_pokerus ),
      m_level( p_level ) {
    m_stats             = calcStats( m_boxdata, p_level, data );
    m_status.m_isAsleep = m_status.m_isBurned = m_status.m_isFrozen = m_status.m_isParalyzed
        = m_status.m_isPoisoned = m_status.m_isBadlyPoisoned = false;
}

bool pokemon::canEvolve( u16 p_item, u16 p_method ) {
    if( isEgg( ) ) return false;
    if( getItem( ) == I_EVERSTONE ) return false;

    getAll( m_boxdata.m_speciesId, data );

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

    int HPdif             = m_stats.m_maxHP - m_stats.m_acHP;
    m_boxdata.m_speciesId = into;
    getAll( m_boxdata.m_speciesId, data );
    if( m_boxdata.m_speciesId != 292 )
        m_stats.m_maxHP = ( ( m_boxdata.m_individualValues.m_hp + 2 * data.m_bases[ 0 ]
                              + ( m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )
                            * m_level / 100 )
                          + 10;
    else
        m_stats.m_maxHP = 1;

    if( !m_boxdata.m_individualValues.m_isNicked )
        strcpy( m_boxdata.m_name, getDisplayName( m_boxdata.m_speciesId ).c_str( ) );

    pkmnNatures nature = m_boxdata.getNature( );
    m_stats.m_Atk      = ( ( ( m_boxdata.m_individualValues.m_attack + 2 * data.m_bases[ 1 ]
                          + ( m_boxdata.m_effortValues[ 1 ] >> 2 ) )
                        * m_level / 100.0 )
                      + 5 )
                    * NatMod[ nature ][ 0 ];
    m_stats.m_Def = ( ( ( m_boxdata.m_individualValues.m_defense + 2 * data.m_bases[ 2 ]
                          + ( m_boxdata.m_effortValues[ 2 ] >> 2 ) )
                        * m_level / 100.0 )
                      + 5 )
                    * NatMod[ nature ][ 1 ];
    m_stats.m_Spd = ( ( ( m_boxdata.m_individualValues.m_speed + 2 * data.m_bases[ 3 ]
                          + ( m_boxdata.m_effortValues[ 3 ] >> 2 ) )
                        * m_level / 100.0 )
                      + 5 )
                    * NatMod[ nature ][ 2 ];
    m_stats.m_SAtk = ( ( ( m_boxdata.m_individualValues.m_sAttack + 2 * data.m_bases[ 4 ]
                           + ( m_boxdata.m_effortValues[ 4 ] >> 2 ) )
                         * m_level / 100.0 )
                       + 5 )
                     * NatMod[ nature ][ 3 ];
    m_stats.m_SDef = ( ( ( m_boxdata.m_individualValues.m_sDefense + 2 * data.m_bases[ 5 ]
                           + ( m_boxdata.m_effortValues[ 5 ] >> 2 ) )
                         * m_level / 100.0 )
                       + 5 )
                     * NatMod[ nature ][ 4 ];

    m_stats.m_acHP = m_stats.m_maxHP - HPdif;
}

void pokemon::boxPokemon::hatch( ) {
    m_individualValues.m_isEgg = false;
    m_hatchPlace               = MAP::curMap->getCurrentLocationId( );
    m_hatchDate[ 0 ]           = acday;
    m_hatchDate[ 1 ]           = acmonth + 1;
    m_hatchDate[ 2 ]           = ( acyear + 1900 ) % 100;
}

bool pokemon::boxPokemon::learnMove( u16 p_move ) {
    char buffer[ 50 ];
    if( p_move == m_moves[ 0 ] || p_move == m_moves[ 1 ] || p_move == m_moves[ 2 ]
        || p_move == m_moves[ 3 ] ) {
        snprintf( buffer, 49, GET_STRING( 102 ), m_name,
                  AttackList[ p_move ]->m_moveName.c_str( ) );
        IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
        IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
        IO::messageBox a( buffer, false );
        return false;
    } else if( canLearn( m_speciesId, p_move, 4 ) ) {
        bool freeSpot = false;
        for( u8 i = 0; i < 4; ++i )
            if( !m_moves[ i ] ) {
                m_moves[ i ] = p_move;
                m_acPP[ i ]  = std::min( m_acPP[ i ], AttackList[ p_move ]->m_movePP );

                snprintf( buffer, 49, GET_STRING( 103 ), m_name,
                          AttackList[ p_move ]->m_moveName.c_str( ) );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                IO::messageBox a( buffer, false );

                freeSpot = true;
                break;
            }
        if( !freeSpot ) {
            IO::yesNoBox yn( false );
            snprintf( buffer, 49, GET_STRING( 104 ), m_name );
            if( yn.getResult( buffer ) ) {
                u8 res = IO::moveChoiceBox( *this, p_move ).getResult( false, false, (u8) -1 );
                if( res < 4 ) {
                    if( AttackList[ m_moves[ res ] ]->m_isFieldAttack ) {
                        snprintf( buffer, 49, GET_STRING( 106 ), m_name,
                                  AttackList[ m_moves[ res ] ]->m_moveName.c_str( ) );
                        IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                        IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                        IO::messageBox a( buffer, false );
                        return false;
                    } else {
                        m_moves[ res ] = p_move;
                        m_acPP[ res ]  = std::min( m_acPP[ res ], AttackList[ p_move ]->m_movePP );
                    }
                }
            } else
                return false;
        }
    } else {
        snprintf( buffer, 49, GET_STRING( 107 ), m_name,
                  AttackList[ p_move ]->m_moveName.c_str( ) );
        IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
        IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
        IO::messageBox a( buffer, false );
        return false;
    }
    return true;
}
