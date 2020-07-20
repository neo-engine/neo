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
#include "abilityNames.h"
#include "itemNames.h"
#include "mapDrawer.h"
#include "move.h"
#include "pokemonNames.h"
#include "saveGame.h"

pokemon::pokemon( boxPokemon& p_boxPokemon ) : m_boxdata( p_boxPokemon ) {
    pkmnData data       = getPkmnData( p_boxPokemon.m_speciesId, p_boxPokemon.getForme( ) );
    m_level             = calcLevel( p_boxPokemon, &data );
    m_stats             = calcStats( m_boxdata, m_level, &data );
    m_battleForme       = 0;
    m_battleTimeAbility = 0;
    m_statusint         = 0;
}
pokemon::pokemon( u16 p_pkmnId, u16 p_level, u8 p_forme, const char* p_name, u8 p_shiny,
                  bool p_hiddenAbility, bool p_isEgg, u8 p_ball, u8 p_pokerus,
                  bool p_fatefulEncounter ) {
    pkmnData data = getPkmnData( p_pkmnId, p_forme );
    m_boxdata = boxPokemon( p_pkmnId, p_level, p_forme, p_name, p_shiny, p_hiddenAbility, p_isEgg,
                            p_ball, p_pokerus, p_fatefulEncounter, &data );
    m_level   = p_level;
    m_stats   = calcStats( m_boxdata, p_level, &data );
    m_battleForme       = 0;
    m_battleTimeAbility = 0;
    m_statusint         = 0;
}
pokemon::pokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
                  const char* p_oT, u8 p_shiny, bool p_hiddenAbility, bool p_fatefulEncounter,
                  bool p_isEgg, u16 p_gotPlace, u8 p_ball, u8 p_pokerus, u8 p_forme ) {
    pkmnData data = getPkmnData( p_pkmnId, p_forme );
    m_boxdata     = boxPokemon( p_moves, p_pkmnId, p_name, p_level, p_id, p_sid, p_oT, p_shiny,
                            p_hiddenAbility, p_fatefulEncounter, p_isEgg, p_gotPlace, p_ball,
                            p_pokerus, p_forme, &data );
    m_level       = p_level;
    m_stats       = calcStats( m_boxdata, p_level, &data );
    m_battleForme = 0;
    m_battleTimeAbility = 0;
    m_statusint         = 0;
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
    for( u8 i = 0; i < 6; ++i ) { m_boxdata.IVset( i, p_trainerPokemon.m_iv[ i ] ); }
    m_stats             = calcStats( m_boxdata, m_level, &data );
    m_battleForme       = 0;
    m_battleTimeAbility = 0;
    m_statusint         = 0;
}

bool pokemon::heal( ) {
    bool change     = m_stats.m_curHP < m_stats.m_maxHP;
    m_stats.m_curHP = m_stats.m_maxHP;

    change |= !!m_statusint;
    m_statusint = 0;

    for( u8 i = 0; i < 4; ++i ) {
        if( m_boxdata.m_moves[ i ] ) {
            MOVE::moveData mdata = MOVE::getMoveData( m_boxdata.m_moves[ i ] );
            auto           mx    = s8( mdata.m_pp * ( ( 5 + m_boxdata.PPupget( i ) ) / 5.0 ) );
            change |= m_boxdata.m_curPP[ i ] < mx;
            m_boxdata.m_curPP[ i ] = mx;
        }
    }

    return change;
}

void pokemon::battleTransform( ) {
    if( getSpecies( ) == PKMN_DARMANITAN && getAbility( ) == A_ZEN_MODE ) {
        setBattleForme( 2 * ( getForme( ) / 2 ) + 1 );
        return;
    }
    if( getSpecies( ) == PKMN_WISHIWASHI && getAbility( ) == A_SCHOOLING ) {
        setBattleForme( 1 );
        return;
    }

    if( getSpecies( ) == PKMN_ZYGARDE && getAbility( ) == A_POWER_CONSTRUCT ) {
        setBattleForme( 2 );
        return;
    }

    if( canBattleTransform( ) ) {
        if( getSpecies( ) == PKMN_CHARIZARD ) {
            if( getItem( ) == I_CHARIZARDITE_X ) {
                setBattleForme( 1 );
            } else {
                setBattleForme( 2 );
            }
            return;
        }
        if( getSpecies( ) == PKMN_MEWTWO ) {
            if( getItem( ) == I_MEWTWONITE_X ) {
                setBattleForme( 1 );
            } else {
                setBattleForme( 2 );
            }
            return;
        }
        setBattleForme( 1 );
    }
}

void pokemon::revertBattleTransform( ) {
    if( m_battleForme ) { setBattleForme( 0 ); }
}

void pokemon::recalculateStats( ) {
    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );
    recalculateStats( data );
}

void pokemon::recalculateStats( pkmnData& p_data ) {
    auto HPdif = m_stats.m_maxHP - m_stats.m_curHP;
    m_stats    = calcStats( m_boxdata, m_level, &p_data );
    if( m_stats.m_maxHP < HPdif ) {
        m_stats.m_curHP = 0;
    } else {
        m_stats.m_curHP = m_stats.m_maxHP - HPdif;
    }
}

void pokemon::setForme( u8 p_newForme ) {
    m_boxdata.setForme( p_newForme );
    recalculateStats( );
}

void pokemon::setBattleForme( u8 p_newForme ) {
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

bool pokemon::setExperience( u32 p_amount ) {
    if( m_level == 100 ) { return false; }

    pkmnData data = getPkmnData( m_boxdata.m_speciesId, getForme( ) );

    m_boxdata.m_experienceGained = std::min( p_amount, EXP[ 99 ][ data.getExpType( ) ] );
    m_level                      = calcLevel( m_boxdata, &data );
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

pokemon::stats calcStats( boxPokemon& p_boxdata, u8 p_level, const pkmnData* p_data ) {
    pokemon::stats res;
    u16            pkmnId = p_boxdata.m_speciesId;
    if( pkmnId != PKMN_SHEDINJA )
        res.m_curHP = res.m_maxHP = ( ( p_boxdata.IVget( 0 ) + 2 * p_data->m_baseForme.m_bases[ 0 ]
                                        + ( p_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )
                                      * p_level / 100 )
                                    + 10;
    else
        res.m_curHP = res.m_maxHP = 1;
    pkmnNatures nature = p_boxdata.getNature( );

    for( u8 i = 1; i < 6; ++i ) {
        res.setStat( i, ( ( ( p_boxdata.IVget( i ) + 2 * p_data->m_baseForme.m_bases[ i ]
                              + ( p_boxdata.m_effortValues[ i ] >> 2 ) )
                            * p_level / 100.0 )
                          + 5 )
                            * NatMod[ nature ][ i - 1 ] / 10 );
    }
    return res;
}
pokemon::stats calcStats( boxPokemon& p_boxdata, const pkmnData* p_data ) {
    return calcStats( p_boxdata, calcLevel( p_boxdata, p_data ), p_data );
}
u16 calcLevel( boxPokemon& p_boxdata, const pkmnData* p_data ) {
    for( u16 i = 2; i < 101; ++i )
        if( EXP[ i - 1 ][ p_data->getExpType( ) ] > p_boxdata.m_experienceGained ) return ( i - 1 );
    return 100;
}

void pokemon::giveItem( u16 p_newItem ) {
    m_boxdata.giveItem( p_newItem );
    pkmnData data   = getPkmnData( m_boxdata.m_speciesId, m_boxdata.getForme( ) );
    auto     oldHP  = m_stats.m_maxHP - m_stats.m_curHP;
    m_stats         = calcStats( m_boxdata, m_level, &data );
    m_stats.m_curHP = m_stats.m_maxHP - oldHP;
}
u16 pokemon::takeItem( ) {
    u16      res    = m_boxdata.takeItem( );
    pkmnData data   = getPkmnData( m_boxdata.m_speciesId, m_boxdata.getForme( ) );
    auto     oldHP  = m_stats.m_maxHP - m_stats.m_curHP;
    m_stats         = calcStats( m_boxdata, m_level, &data );
    m_stats.m_curHP = m_stats.m_maxHP - oldHP;
    return res;
}

u8 pokemon::canEvolve( u16 p_item, evolutionMethod p_method, pkmnEvolveData* p_edata ) {
    if( isEgg( ) ) return false;
    if( getItem( ) == I_EVERSTONE ) return false;

    pkmnEvolveData edata;
    if( p_edata != nullptr ) {
        edata = *p_edata;
    } else {
        edata = getPkmnEvolveData( getSpecies( ), getForme( ) );
    }

    for( u8 i = 0; i < edata.m_evolutionCount; ++i ) {
        auto cur = edata.m_evolutions[ i ];

        switch( cur.m_type ) {
        case EVOLUTION_LEVEL:
            if( p_method == EVOMETHOD_LEVEL_UP && m_level >= cur.m_param1 ) { return i + 1; }
            break;
        case EVOLUTION_ITEM:
            if( p_method == EVOMETHOD_ITEM && p_item == cur.m_param1 ) { return i + 1; }
            break;
        case EVOLUTION_TRADE:
            if( p_method == EVOMETHOD_TRADE ) { return i + 1; }
            break;
        case EVOLUTION_TRADE_ITEM:
            if( p_method == EVOMETHOD_TRADE && getItem( ) == cur.m_param1 ) { return i + 1; }
            break;
        case EVOLUTION_TRADE_PKMN:
            if( p_method == EVOMETHOD_TRADE && p_item == cur.m_param1 ) { return i + 1; }
            break;

        case EVOLUTION_TIME: {
            if( p_method == EVOMETHOD_LEVEL_UP && cur.m_param1 == getCurrentDaytime( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_PLACE: {
            if( p_method == EVOMETHOD_LEVEL_UP
                && cur.m_param1 == MAP::curMap->getCurrentLocationId( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_MOVE: {
            if( p_method != EVOMETHOD_LEVEL_UP ) { break; }
            for( u8 j = 0; j < 4; ++j ) {
                if( getMove( j ) == cur.m_param1 ) { return i + 1; }
            }
            break;
        }
        case EVOLUTION_FRIEND: {
            if( p_method == EVOMETHOD_LEVEL_UP && m_boxdata.m_steps >= 250 ) { return i + 1; }
            break;
        }
        case EVOLUTION_LEVEL_PLACE: {
            if( p_method == EVOMETHOD_LEVEL_UP && m_level >= cur.m_param1
                && cur.m_param2 == MAP::curMap->getCurrentLocationId( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_FRIEND_TIME: {
            if( p_method == EVOMETHOD_LEVEL_UP && m_boxdata.m_steps >= 250
                && cur.m_param1 == getCurrentDaytime( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_LEVEL_TIME: {
            if( p_method == EVOMETHOD_LEVEL_UP && m_level >= cur.m_param1
                && cur.m_param2 == getCurrentDaytime( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_LEVEL_GENDER: {
            if( p_method == EVOMETHOD_LEVEL_UP && m_level >= cur.m_param1
                && cur.m_param2 - 1 == gender( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_ITEM_HOLD: {
            if( p_method == EVOMETHOD_LEVEL_UP && getItem( ) == cur.m_param1
                && cur.m_param2 == getCurrentDaytime( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_CONTEST: {
            if( p_method == EVOMETHOD_LEVEL_UP
                && m_boxdata.m_contestStats[ cur.m_param1 ] >= 250 ) {
                return i + 1;
            }
            break;
        }

        case EVOLUTION_ITEM_PLACE: {
            if( p_method == EVOMETHOD_ITEM && p_item >= cur.m_param1
                && cur.m_param2 == MAP::curMap->getCurrentLocationId( ) ) {
                return i + 1;
            }
            break;
        }
        case EVOLUTION_ITEM_GENDER: {
            if( p_method == EVOMETHOD_ITEM && p_item == cur.m_param1
                && cur.m_param2 - 1 == gender( ) ) {
                return i + 1;
            }
            break;
        }

        case EVOLUTION_SPECIAL: {
            if( p_method != EVOMETHOD_LEVEL_UP ) { break; }
            switch( getSpecies( ) ) {
            case PKMN_TYROGUE:
                if( m_level < 20 ) { break; }
                if( cur.m_target == PKMN_HITMONLEE && m_stats.m_Atk > m_stats.m_Def ) {
                    return i + 1;
                }
                if( cur.m_target == PKMN_HITMONCHAN && m_stats.m_Atk < m_stats.m_Def ) {
                    return i + 1;
                }
                if( cur.m_target == PKMN_HITMONTOP && m_stats.m_Atk == m_stats.m_Def ) {
                    return i + 1;
                }
                break;
            case PKMN_WURMPLE:
                if( m_level < 7 ) { break; }
                if( cur.m_target == PKMN_SILCOON && ( m_boxdata.m_pid >> 16 ) % 10 <= 4 ) {
                    return i + 1;
                }
                if( cur.m_target == PKMN_CASCOON && ( m_boxdata.m_pid >> 16 ) % 10 > 4 ) {
                    return i + 1;
                }
                break;
            case PKMN_MANTYKE:
                // Check for a Remoraid in the party
                for( u8 q = 0; q < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++q ) {
                    if( SAVE::SAV.getActiveFile( ).getTeamPkmn( q )->getSpecies( )
                        == PKMN_REMORAID ) {
                        return i + 1;
                    }
                }
                break;
            case PKMN_PANCHAM:
                // Check for dark-type pkmn
                for( u8 q = 0; q < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++q ) {
                    auto dt
                        = getPkmnData( SAVE::SAV.getActiveFile( ).getTeamPkmn( q )->getSpecies( ),
                                       SAVE::SAV.getActiveFile( ).getTeamPkmn( q )->getForme( ) );
                    if( dt.m_baseForme.m_types[ 0 ] == DARK
                        || dt.m_baseForme.m_types[ 1 ] == DARK ) {
                        return i + 1;
                    }
                }
                break;
            case PKMN_SLIGGOO:
                if( m_level < 50 ) { break; }
                if( MAP::curMap->getWeather( ) == MAP::mapWeather::RAINY
                    || MAP::curMap->getWeather( ) == MAP::mapWeather::HEAVY_RAIN
                    || MAP::curMap->getWeather( ) == MAP::mapWeather::FOG
                    || MAP::curMap->getWeather( ) == MAP::mapWeather::DENSE_MIST ) {
                    return i + 1;
                }
                break;
            case PKMN_TOXEL:
                if( m_level < 30 ) { break; }

                switch( getNature( ) ) {
                case HARDY:
                case BRAVE:
                case ADAMANT:
                case NAUGHY:
                case DOCILE:
                case IMPISH:
                case LAX:
                case HASTY:
                case JOLLY:
                case NAIVE:
                case RASH:
                case SASSY:
                case QUIRKY:
                    if( cur.m_targetForme == 0 ) { return i + 1; }
                    break;
                default:
                    if( cur.m_targetForme == 1 ) { return i + 1; }
                    break;
                }
                break;

                [[likely]] default : break;
            }
        }

            [[unlikely]] default : break;
        }
    }

    return false;
}

void pokemon::evolve( u16 p_item, evolutionMethod p_method ) {
    if( isEgg( ) ) return;
    if( getItem( ) == I_EVERSTONE ) return;

    auto edata = getPkmnEvolveData( getSpecies( ), getForme( ) );
    u8   tg    = canEvolve( p_item, p_method, &edata );
    if( !( tg-- ) ) { return; }

    m_boxdata.m_altForme = edata.m_evolutions[ tg ].m_targetForme;
    setSpecies( edata.m_evolutions[ tg ].m_target );

    if( !m_boxdata.isNicknamed( ) ) {
        strncpy( m_boxdata.m_name, getDisplayName( edata.m_evolutions[ tg ].m_target ).c_str( ),
                 12 );
    }

    // check for shedinja

    if( getSpecies( ) == PKMN_NINJASK ) {
        if( SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( ITEM::ITEMTYPE_POKEBALL ),
                                                    I_POKE_BALL ) ) {
            if( SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) < 6 ) {
                // Create a Shedinja
                auto shed = *this;
                shed.setSpecies( PKMN_SHEDINJA );
                shed.m_boxdata.m_ball = ITEM::itemToBall( I_POKE_BALL );
                shed.heal( );
                shed.takeItem( );
                strcpy( shed.m_boxdata.m_name,
                        getDisplayName( PKMN_SHEDINJA, CURRENT_LANGUAGE ).c_str( ) );

                SAVE::SAV.getActiveFile( ).m_bag.erase( BAG::toBagType( ITEM::ITEMTYPE_POKEBALL ),
                                                        I_POKE_BALL, 1 );
                SAVE::SAV.getActiveFile( ).setTeamPkmn(
                    SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), &shed );
            }
        }
    }
}
