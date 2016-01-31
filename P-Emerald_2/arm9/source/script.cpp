/*
Pokémon Emerald 2 Version
------------------------------

file        : script.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
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


#include "script.h"
#include "battle.h"

namespace BATTLE {
    bool battleScript::command::condition::evaluate( int p_other ) const {
        switch( m_comp ) {
            case BATTLE::battleScript::command::EQUALS:
                return p_other == m_value;
            case BATTLE::battleScript::command::NOT_EQUALS:
                return p_other != m_value;
            case BATTLE::battleScript::command::GREATER:
                return p_other > m_value;
            case BATTLE::battleScript::command::LESS:
                return p_other < m_value;
            case BATTLE::battleScript::command::GEQ:
                return p_other >= m_value;
            case BATTLE::battleScript::command::LEQ:
                return p_other <= m_value;
            default:
                return false;
        }
    }

    u8 targetVal;
    int getTargetSpecifierValue( const battle& p_battle, const pokemon& p_target, bool p_targetIsOpp,
                                 u8 p_targetPosition, const battleScript::command::targetSpecifier& p_targetSpecifier ) {
        pokemonData data; getAll( p_target.m_boxdata.m_speciesId, data );
        switch( p_targetSpecifier ) {
            case BATTLE::battleScript::command::PKMN_TYPE1:
                return int( data.m_types[ 0 ] );
            case BATTLE::battleScript::command::PKMN_TYPE2:
                return int( data.m_types[ 1 ] );
            case BATTLE::battleScript::command::PKMN_TYPE1o2:
            {
                int a = int( data.m_types[ 1 ] );
                if( targetVal == a )
                    return a;
                else
                    return int( data.m_types[ 0 ] );
                break;
            }
            case BATTLE::battleScript::command::PKMN_SIZE:
                return int( data.m_size );
            case BATTLE::battleScript::command::PKMN_WEIGHT:
                return int( data.m_weight );
            case BATTLE::battleScript::command::PKMN_SPECIES:
                return p_target.m_boxdata.m_speciesId;
            case BATTLE::battleScript::command::PKMN_ITEM:
                return p_target.m_boxdata.m_holdItem;
            case BATTLE::battleScript::command::PKMN_ABILITY:
                return p_target.m_boxdata.m_ability;
            case BATTLE::battleScript::command::PKMN_GENDER:
                return ( p_target.m_boxdata.m_isFemale ? -1 : ( p_target.m_boxdata.m_isGenderless ? 0 : 1 ) );
            case BATTLE::battleScript::command::PKMN_STATUS:
                return p_target.m_statusint;
            case BATTLE::battleScript::command::PKMN_HP:
                return p_target.m_stats.m_acHP;
            case BATTLE::battleScript::command::PKMN_MAX_HP:
                return p_target.m_stats.m_maxHP;
            case BATTLE::battleScript::command::PKMN_HP_PERCENT:
                return p_target.m_stats.m_acHP * 100 / p_target.m_stats.m_maxHP;
            case BATTLE::battleScript::command::PKMN_ATK:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ATK ];
            case BATTLE::battleScript::command::PKMN_DEF:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ DEF ];
            case BATTLE::battleScript::command::PKMN_SPD:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SPD ];
            case BATTLE::battleScript::command::PKMN_SATK:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SATK ];
            case BATTLE::battleScript::command::PKMN_SDEF:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SDEF ];
            case BATTLE::battleScript::command::PKMN_ACCURACY:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ACCURACY ];
            case BATTLE::battleScript::command::PKMN_ATTACK_BLOCKED:
                return ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ATTACK_BLOCKED ];
            case BATTLE::battleScript::command::PKMN_LEVEL:
                return p_target.m_level;
            default:
                return -1;
        }
    }
    int getTargetSpecifierValue( const battle& p_target, const battleScript::command::targetSpecifier& p_targetSpecifier ) {
        switch( p_targetSpecifier ) {
            case BATTLE::battleScript::command::BATTLE_ROUND:
                return p_target._round;
            case BATTLE::battleScript::command::BATTLE_MODE:
                return p_target.m_battleMode;
            case BATTLE::battleScript::command::BATTLE_WEATHER:
                return p_target.m_weather;
            case BATTLE::battleScript::command::BATTLE_LAST_MOVE:
                return p_target._lstMove;
            case BATTLE::battleScript::command::BATTLE_LAST_OWN_MOVE:
                return p_target._lstOwnMove;
            case BATTLE::battleScript::command::BATTLE_LAST_OPP_MOVE:
                return p_target._lstOppMove;
            case BATTLE::battleScript::command::BATTLE_OWN_FST_MOVE:
                if( p_target.m_battleMode == battle::SINGLE ||
                    ( p_target._moveOrder[ 1 ][ PLAYER ] > p_target._moveOrder[ 0 ][ PLAYER ] ) )
                    return ( p_target._battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 0 ][ PLAYER ].m_value : -1;
                return ( p_target._battleMoves[ 1 ][ PLAYER ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 1 ][ PLAYER ].m_value : -1;
            case BATTLE::battleScript::command::BATTLE_OWN_SND_MOVE:
                if( p_target.m_battleMode == battle::SINGLE )
                    return -1;
                if( p_target._moveOrder[ 1 ][ PLAYER ] > p_target._moveOrder[ 0 ][ PLAYER ] )
                    return ( p_target._battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 0 ][ PLAYER ].m_value : -1;
                return ( p_target._battleMoves[ 1 ][ PLAYER ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 1 ][ PLAYER ].m_value : -1;
            case BATTLE::battleScript::command::BATTLE_OPP_FST_MOVE:
                if( p_target.m_battleMode == battle::SINGLE ||
                    ( p_target._moveOrder[ 1 ][ OPPONENT ] > p_target._moveOrder[ 0 ][ OPPONENT ] ) )
                    return ( p_target._battleMoves[ 0 ][ OPPONENT ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 0 ][ OPPONENT ].m_value : -1;
                return ( p_target._battleMoves[ 1 ][ OPPONENT ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 1 ][ OPPONENT ].m_value : -1;
            case BATTLE::battleScript::command::BATTLE_OPP_SND_MOVE:
                if( p_target.m_battleMode == battle::SINGLE )
                    return -1;
                if( p_target._moveOrder[ 1 ][ OPPONENT ] < p_target._moveOrder[ 0 ][ OPPONENT ] )
                    return ( p_target._battleMoves[ 0 ][ OPPONENT ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 0 ][ OPPONENT ].m_value : -1;
                return ( p_target._battleMoves[ 1 ][ OPPONENT ].m_type == battle::battleMove::ATTACK ) ? p_target._battleMoves[ 1 ][ OPPONENT ].m_value : -1;
            case BATTLE::battleScript::command::BATTLE_OWN_TEAMSIZE:
                return int( p_target._player->m_pkmnTeam.size( ) );
            case BATTLE::battleScript::command::BATTLE_OPP_TEAMSIZE:
                return int( p_target._opponent->m_pkmnTeam.size( ) );
            default:
                return -1;
        }
    }

    int battleScript::command::condition::getTargetVal( const battle& p_battle, const pokemon& p_target, bool p_targetIsOpp, u8 p_targetPosition )const {
        targetVal = m_value;
        return getTargetSpecifierValue( p_battle, p_target, p_targetIsOpp, p_targetPosition, m_targetSpecifier );
    }
    int battleScript::command::condition::getTargetVal( const battle& p_target ) const {
        return getTargetSpecifierValue( p_target, m_targetSpecifier );
    }

    bool battleScript::command::condition::check( battle& p_battle, void* p_self ) const {
        switch( m_target ) {
            case BATTLE::battleScript::command::OPPONENT1:
                return evaluate( getTargetVal( p_battle, ACPKMN2( p_battle, OPPONENT, 0 ), OPPONENT, 0 ) );
            case BATTLE::battleScript::command::OPPONENT2:
                return evaluate( getTargetVal( p_battle, ACPKMN2( p_battle, OPPONENT, 1 ), OPPONENT, 1 ) );
            case BATTLE::battleScript::command::OWN1:
                return evaluate( getTargetVal( p_battle, ACPKMN2( p_battle, PLAYER, 0 ), PLAYER, 0 ) );
            case BATTLE::battleScript::command::OWN2:
                return evaluate( getTargetVal( p_battle, ACPKMN2( p_battle, PLAYER, 1 ), PLAYER, 1 ) );
            case BATTLE::battleScript::command::BATTLE_:
                return evaluate( getTargetVal( p_battle ) );
            case BATTLE::battleScript::command::SELF_PKMN:
            {
                bool pkmnIsOpp = false;
                bool pkmnIsSnd = ( p_battle.m_battleMode == battle::DOUBLE );
                auto acPkmn = *( (pokemon*) p_self );

                if( acPkmn == ACPKMN2( p_battle, OPPONENT, 0 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = false;
                }
                if( acPkmn == ACPKMN2( p_battle, OPPONENT, 1 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd &= true;
                }
                if( acPkmn == ACPKMN2( p_battle, PLAYER, 0 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = false;
                }
                if( acPkmn == ACPKMN2( p_battle, PLAYER, 1 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd &= true;
                }

                return evaluate( getTargetVal( p_battle, *( (pokemon*) p_self ), pkmnIsOpp, pkmnIsSnd ) );
            }
            case BATTLE::battleScript::command::SELF_BATTLE:
                return evaluate( getTargetVal( *( (battle*) p_self ) ) );
            case BATTLE::battleScript::command::SELF_DAMAGE:
                return evaluate( *( (int*) p_self ) );
            default:
                break;
        }
        return false;
    }

    int battleScript::command::value::get( battle& p_battle, void* p_self ) const {
        switch( m_target ) {
            case BATTLE::battleScript::command::OPPONENT1:
                return get( p_battle, ACPKMN2( p_battle, OPPONENT, 0 ), OPPONENT, 0 );
            case BATTLE::battleScript::command::OPPONENT2:
                return get( p_battle, ACPKMN2( p_battle, OPPONENT, 1 ), OPPONENT, 1 );
            case BATTLE::battleScript::command::OWN1:
                return get( p_battle, ACPKMN2( p_battle, PLAYER, 0 ), PLAYER, 0 );
            case BATTLE::battleScript::command::OWN2:
                return get( p_battle, ACPKMN2( p_battle, PLAYER, 1 ), PLAYER, 1 );
            case BATTLE::battleScript::command::BATTLE_:
                return get( p_battle );
            case BATTLE::battleScript::command::SELF_PKMN:
            {
                bool pkmnIsOpp = false;
                bool pkmnIsSnd = ( p_battle.m_battleMode == battle::DOUBLE );
                auto acPkmn = *( (pokemon*) p_self );

                if( acPkmn == ACPKMN2( p_battle, OPPONENT, 0 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = false;
                }
                if( acPkmn == ACPKMN2( p_battle, OPPONENT, 1 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd &= true;
                }
                if( acPkmn == ACPKMN2( p_battle, PLAYER, 0 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = false;
                }
                if( acPkmn == ACPKMN2( p_battle, PLAYER, 1 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd &= true;
                }

                return get( p_battle, *( (pokemon*) p_self ), pkmnIsOpp, pkmnIsSnd );
            }
            case BATTLE::battleScript::command::SELF_BATTLE:
                return get( *( (battle*) p_self ) );
            case BATTLE::battleScript::command::SELF_DAMAGE:
                return m_additiveConstant + m_multiplier * ( *( (int*) p_self ) ) / 100;
            default:
            case BATTLE::battleScript::command::NO_TARGET:
                return m_additiveConstant;
        }
    }
    int battleScript::command::value::get( battle& p_battle, pokemon& p_target, bool p_targetIsOpp, u8 p_targetPosition ) const {
        return m_additiveConstant + int( m_multiplier *getTargetSpecifierValue( p_battle, p_target, p_targetIsOpp, p_targetPosition, m_targetSpecifier ) );
    }
    int battleScript::command::value::get( battle& p_target ) const {
        return m_additiveConstant + int( m_multiplier * getTargetSpecifierValue( p_target, m_targetSpecifier ) );
    }

    void battleScript::command::evaluateOnTargetVal( battle& p_battle, void* p_self, pokemon& p_target, bool p_targetIsOpp, u8 p_targetPosition ) const {

        //target t;
        //if( p_targetIsOpp && p_targetPosition )
        //    t = OPPONENT2;
        //if( p_targetIsOpp && !p_targetPosition )
        //    t = OPPONENT1;
        //if( !p_targetIsOpp && p_targetPosition )
        //    t = OWN2;
        //if( !p_targetIsOpp && !p_targetPosition )
        //    t = OWN1;

        switch( m_action ) {
            case BATTLE::battleScript::command::ADD:
            case BATTLE::battleScript::command::MULTIPLY:
            case BATTLE::battleScript::command::SET:
                switch( m_targetSpecifier ) {
                    case BATTLE::battleScript::command::PKMN_SPECIES:
                        if( m_action == SET ) {
                            p_target.m_boxdata.m_speciesId = m_value.get( p_battle, p_self );
                        }
                        break;
                    case BATTLE::battleScript::command::PKMN_ITEM:
                        if( m_action == SET ) {
                            p_target.m_boxdata.m_holdItem = m_value.get( p_battle, p_self );
                        }
                        break;
                    case BATTLE::battleScript::command::PKMN_ABILITY:
                        if( m_action == SET ) {
                            p_target.m_boxdata.m_holdItem = m_value.get( p_battle, p_self );
                        }
                        break;
                    case BATTLE::battleScript::command::PKMN_STATUS:
                        if( m_action == SET ) {
                            p_target.m_boxdata.m_holdItem = m_value.get( p_battle, p_self );
                        }
                        break;
                    case BATTLE::battleScript::command::PKMN_HP:
                        if( m_action == SET )
                            p_target.m_stats.m_acHP = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            p_target.m_stats.m_acHP += m_value.get( p_battle, p_self );
                        if( m_action == MULTIPLY )
                            p_target.m_stats.m_acHP = int( ( p_target.m_stats.m_acHP / 100.f ) *  m_value.get( p_battle, p_self ) );

                        p_target.m_stats.m_acHP = std::max( (u16) 0, std::min( p_target.m_stats.m_maxHP, p_target.m_stats.m_acHP ) );

                        break;
                    case BATTLE::battleScript::command::PKMN_ATK:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ATK ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ATK ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_DEF:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ DEF ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ DEF ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_SPD:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SPD ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SPD ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_SATK:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SATK ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SATK ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_SDEF:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SDEF ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ SDEF ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_ACCURACY:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ACCURACY ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ACCURACY ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_ATTACK_BLOCKED:
                        if( m_action == SET )
                            ACPKMNSTATCHG2( p_battle, p_targetPosition, p_targetIsOpp )[ ATTACK_BLOCKED ] = !!m_value.get( p_battle, p_self );
                        break;
                    default:
                        break;
                }
                break;
            case BATTLE::battleScript::command::SWITCH:
                p_battle.switchPKMN( p_targetIsOpp, p_targetPosition, m_value.get( p_battle, p_self ) );
                break;
            default:
                break;
        }
    }
    void battleScript::command::evaluateOnTargetVal( battle& p_battle, void* p_self ) const {
        switch( m_action ) {
            case BATTLE::battleScript::command::SET:
                if( m_targetSpecifier == BATTLE_WEATHER ) {
                    p_battle.m_weather = battle::weather( m_value.get( p_battle, p_self ) );
                }
                if( m_targetSpecifier == BATTLE_WEATHER_LENGTH ) {
                    p_battle._weatherLength = battle::weather( m_value.get( p_battle, p_self ) );
                }
                break;
            case BATTLE::battleScript::command::END:
                p_battle.endBattle( battle::NONE );
                break;
            default:
                break;
        }
    }

    void battleScript::command::execute( battle& p_battle, void* p_self ) const {
        switch( m_target ) {
            case BATTLE::battleScript::command::OPPONENT1:
                return evaluateOnTargetVal( p_battle, p_self, ACPKMN2( p_battle, OPPONENT, 0 ), OPPONENT, 0 );
            case BATTLE::battleScript::command::OPPONENT2:
                return evaluateOnTargetVal( p_battle, p_self, ACPKMN2( p_battle, OPPONENT, 1 ), OPPONENT, 1 );
            case BATTLE::battleScript::command::OWN1:
                return evaluateOnTargetVal( p_battle, p_self, ACPKMN2( p_battle, PLAYER, 0 ), PLAYER, 0 );
            case BATTLE::battleScript::command::OWN2:
                return evaluateOnTargetVal( p_battle, p_self, ACPKMN2( p_battle, PLAYER, 1 ), PLAYER, 1 );
            case BATTLE::battleScript::command::BATTLE_:
                return evaluateOnTargetVal( p_battle, p_self );
            case BATTLE::battleScript::command::SELF_PKMN:
            {
                bool pkmnIsOpp = false;
                bool pkmnIsSnd = ( p_battle.m_battleMode == battle::DOUBLE );
                auto acPkmn = *( (pokemon*) p_self );

                if( acPkmn == ACPKMN2( p_battle, OPPONENT, 0 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = false;
                }
                if( acPkmn == ACPKMN2( p_battle, OPPONENT, 1 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd &= true;
                }
                if( acPkmn == ACPKMN2( p_battle, PLAYER, 0 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = false;
                }
                if( acPkmn == ACPKMN2( p_battle, PLAYER, 1 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd &= true;
                }

                return evaluateOnTargetVal( p_battle, p_self, acPkmn, pkmnIsOpp, pkmnIsSnd );
            }
            case BATTLE::battleScript::command::SELF_BATTLE:
                return evaluateOnTargetVal( *( (battle*) p_self ), p_self );

            case BATTLE::battleScript::command::SELF_DAMAGE:
            {
                switch( m_action ) {
                    case BATTLE::battleScript::command::SET:
                        *( (int*) p_self ) = m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::ADD:
                        *( (int*) p_self ) += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::MULTIPLY:
                        *( (int*) p_self ) = int( *( (int*) p_self ) * m_value.get( p_battle, p_self ) / 100.f );
                        break;
                    default:
                        break;
                }
            }
            default:
                break;
        }
    }

    void battleScript::execute( battle& p_battle, void* p_self ) const {
        for( auto cmd : _commands ) {
            for( auto cond : cmd.m_conditions )
                if( !cond.check( p_battle, p_self ) )
                    goto NEXT;
            if( cmd.m_log.length( ) )
                p_battle.log( cmd.m_log );
            cmd.execute( p_battle, p_self );
NEXT:
            continue;
        }
    }

}