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

#include <algorithm>

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

    bool battleScript::command::condition::check( battle& p_battle, void* p_self ) const {
        switch( m_target ) {
            case BATTLE::battleScript::command::OPPONENT1:
                return evaluate( p_battle.getTargetSpecifierValue( OPPONENT, 0, m_targetSpecifier, m_value ) );
            case BATTLE::battleScript::command::OPPONENT2:
                return evaluate( p_battle.getTargetSpecifierValue( OPPONENT, 1, m_targetSpecifier, m_value ) );
            case BATTLE::battleScript::command::OWN1:
                return evaluate( p_battle.getTargetSpecifierValue( PLAYER, 0, m_targetSpecifier, m_value ) );
            case BATTLE::battleScript::command::OWN2:
                return evaluate( p_battle.getTargetSpecifierValue( PLAYER, 1, m_targetSpecifier, m_value ) );
            case BATTLE::battleScript::command::BATTLE_:
                return evaluate( p_battle.getTargetSpecifierValue( m_targetSpecifier ) );
            case BATTLE::battleScript::command::SELF_PKMN:
            {
                bool pkmnIsOpp = false;
                bool pkmnIsSnd = ( p_battle.m_battleMode == battle::DOUBLE );
                auto acPkmn = *( (pokemon*) p_self );

                if( acPkmn == CUR_PKMN_2( p_battle, OPPONENT, 0 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = false;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, OPPONENT, 1 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd &= true;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, PLAYER, 0 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = false;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, PLAYER, 1 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd &= true;
                }

                return evaluate( p_battle.getTargetSpecifierValue( pkmnIsOpp, pkmnIsSnd, m_targetSpecifier, m_value ) );
            }
            case BATTLE::battleScript::command::SELF_BATTLE:
                return evaluate( ( (battle*) p_self )->getTargetSpecifierValue( m_targetSpecifier ) );
            case BATTLE::battleScript::command::MOVE:
                return evaluate( *( (int*) p_self ) );
            default:
                break;
        }
        return false;
    }

    int battleScript::command::value::get( battle& p_battle, void* p_self ) const {
        switch( m_target ) {
            case BATTLE::battleScript::command::OPPONENT1:
                return get( p_battle, OPPONENT, 0 );
            case BATTLE::battleScript::command::OPPONENT2:
                return get( p_battle, OPPONENT, 1 );
            case BATTLE::battleScript::command::OWN1:
                return get( p_battle, PLAYER, 0 );
            case BATTLE::battleScript::command::OWN2:
                return get( p_battle, PLAYER, 1 );
            case BATTLE::battleScript::command::BATTLE_:
                return get( p_battle );
            case BATTLE::battleScript::command::SELF_PKMN:
            {
                bool pkmnIsOpp = false;
                bool pkmnIsSnd = ( p_battle.m_battleMode == battle::DOUBLE );
                auto acPkmn = *( (pokemon*) p_self );

                if( acPkmn == CUR_PKMN_2( p_battle, OPPONENT, 0 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = false;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, OPPONENT, 1 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd &= true;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, PLAYER, 0 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = false;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, PLAYER, 1 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd &= true;
                }

                return get( p_battle, pkmnIsOpp, pkmnIsSnd );
            }
            case BATTLE::battleScript::command::SELF_BATTLE:
                return get( *( (battle*) p_self ) );
            case BATTLE::battleScript::command::MOVE:
                return m_additiveConstant + m_multiplier * ( *( (int*) p_self ) ) / 100;
            default:
            case BATTLE::battleScript::command::NO_TARGET:
                return m_additiveConstant;
        }
    }
    int battleScript::command::value::get( battle& p_battle, bool p_targetIsOpp, u8 p_targetPosition ) const {
        return m_additiveConstant + int( m_multiplier * p_battle.getTargetSpecifierValue( p_targetIsOpp, p_targetPosition, m_targetSpecifier ) );
    }
    int battleScript::command::value::get( battle& p_target ) const {
        return m_additiveConstant + int( m_multiplier * p_target.getTargetSpecifierValue( m_targetSpecifier ) );
    }

    void battleScript::command::evaluateOnTargetVal( battle& p_battle, void* p_self, bool p_targetIsOpp, u8 p_targetPosition ) const {
        if( p_battle.m_battleMode != battle::DOUBLE && p_targetPosition )
            return;

        auto target = CUR_PKMN_STR_2( p_battle, p_targetPosition, p_targetIsOpp );
        switch( m_action ) {
            case BATTLE::battleScript::command::ADD:
            case BATTLE::battleScript::command::MULTIPLY:
            case BATTLE::battleScript::command::SET:
                switch( m_targetSpecifier ) {
                    case BATTLE::battleScript::command::PKMN_SPECIES:
                        if( m_action == SET )
                            target.m_pokemon->m_boxdata.m_speciesId = m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_ITEM:
                        if( m_action == SET )
                            target.m_pokemon->m_boxdata.m_holdItem = m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_ABILITY:
                        if( m_action == SET )
                            target.m_pokemon->m_boxdata.m_holdItem = m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_STATUS:
                        if( m_action == SET )
                            target.m_pokemon->m_boxdata.m_holdItem = m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_HP:
                        if( m_action == SET )
                            target.m_pokemon->m_stats.m_acHP = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_pokemon->m_stats.m_acHP += m_value.get( p_battle, p_self );
                        if( m_action == MULTIPLY )
                            target.m_pokemon->m_stats.m_acHP = int( ( target.m_pokemon->m_stats.m_acHP / 100.f ) *  m_value.get( p_battle, p_self ) );

                        target.m_pokemon->m_stats.m_acHP = std::max( (u16) 0, std::min( target.m_pokemon->m_stats.m_maxHP, target.m_pokemon->m_stats.m_acHP ) );

                        break;
                    case BATTLE::battleScript::command::PKMN_ATK:
                        if( m_action == SET )
                            target.m_acStatChanges[ ATK ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_acStatChanges[ ATK ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_DEF:
                        if( m_action == SET )
                            target.m_acStatChanges[ DEF ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_acStatChanges[ DEF ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_SPD:
                        if( m_action == SET )
                            target.m_acStatChanges[ SPD ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_acStatChanges[ SPD ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_SATK:
                        if( m_action == SET )
                            target.m_acStatChanges[ SATK ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_acStatChanges[ SATK ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_SDEF:
                        if( m_action == SET )
                            target.m_acStatChanges[ SDEF ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_acStatChanges[ SDEF ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_ACCURACY:
                        if( m_action == SET )
                            target.m_acStatChanges[ ACCURACY ] = m_value.get( p_battle, p_self );
                        if( m_action == ADD )
                            target.m_acStatChanges[ ACCURACY ] += m_value.get( p_battle, p_self );
                        break;
                    case BATTLE::battleScript::command::PKMN_ATTACK_BLOCKED:
                        if( m_action == SET )
                            target.m_acStatChanges[ ATTACK_BLOCKED ] = !!m_value.get( p_battle, p_self );
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
                if( m_targetSpecifier == BATTLE_WEATHER )
                    p_battle.m_weather = weather( m_value.get( p_battle, p_self ) );
                if( m_targetSpecifier == BATTLE_WEATHER_LENGTH )
                    p_battle._weatherLength = weather( m_value.get( p_battle, p_self ) );
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
                p_battle.storePkmnSts( OPPONENT, 0 );
                evaluateOnTargetVal( p_battle, p_self, OPPONENT, 0 );
                return p_battle.updatePkmnSts( OPPONENT, 0 );
            case BATTLE::battleScript::command::OPPONENT2:
                p_battle.storePkmnSts( OPPONENT, 1 );
                evaluateOnTargetVal( p_battle, p_self, OPPONENT, 1 );
                return p_battle.updatePkmnSts( OPPONENT, 1 );
            case BATTLE::battleScript::command::OWN1:
                p_battle.storePkmnSts( PLAYER, 0 );
                evaluateOnTargetVal( p_battle, p_self, PLAYER, 0 );
                return p_battle.updatePkmnSts( PLAYER, 0 );
            case BATTLE::battleScript::command::OWN2:
                p_battle.storePkmnSts( PLAYER, 1 );
                evaluateOnTargetVal( p_battle, p_self, PLAYER, 1 );
                return p_battle.updatePkmnSts( PLAYER, 1 );
            case BATTLE::battleScript::command::BATTLE_:
                return evaluateOnTargetVal( p_battle, p_self );
            case BATTLE::battleScript::command::SELF_PKMN:
            {
                bool pkmnIsOpp = false;
                bool pkmnIsSnd = false;
                auto acPkmn = *( (pokemon*) p_self );

                if( acPkmn == CUR_PKMN_2( p_battle, OPPONENT, 0 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = false;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, OPPONENT, 1 ) ) {
                    pkmnIsOpp = true;
                    pkmnIsSnd = true;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, PLAYER, 0 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = false;
                }
                if( acPkmn == CUR_PKMN_2( p_battle, PLAYER, 1 ) ) {
                    pkmnIsOpp = false;
                    pkmnIsSnd = true;
                }
                p_battle.storePkmnSts( pkmnIsOpp, pkmnIsSnd );
                evaluateOnTargetVal( p_battle, p_self, pkmnIsOpp, pkmnIsSnd );
                return p_battle.updatePkmnSts( pkmnIsOpp, pkmnIsSnd );
            }
            case BATTLE::battleScript::command::SELF_BATTLE:
                return evaluateOnTargetVal( *( (battle*) p_self ), p_self );

            case BATTLE::battleScript::command::MOVE:
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
        bool lastCondition = false;
        for( auto cmd : _commands ) {
            for( auto cond : cmd.m_conditions ) {
                if( cond.m_asLastCondition && !lastCondition )
                    goto NEXT;
                if( !cond.m_asLastCondition && !( lastCondition = cond.check( p_battle, p_self ) ) )
                    goto NEXT;
            }
            if( cmd.m_log.length( ) )
                p_battle.log( cmd.m_log );
            cmd.execute( p_battle, p_self );
NEXT:
            continue;
        }
    }

}