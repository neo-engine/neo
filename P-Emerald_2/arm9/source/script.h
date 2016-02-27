/*
Pokémon Emerald 2 Version
------------------------------

file        : script.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#pragma once

#include <string>
#include <vector>

#include <nds/ndstypes.h>

#include "buffer.h"
#include "type.h"

class pokemon;
class move;

namespace BATTLE {
    class battle;
    struct battlePokemon;

    class battleScript {
    public:
        struct command {
            enum target {
                NO_TARGET,
                OPPONENT1,
                OPPONENT2,
                OWN1,
                OWN2,
                BATTLE_,
                SELF_PKMN, //move user
                SELF_BATTLE,
                MOVE,

                MOVE_TARGET,

                PLAYER,

                RANDOM
            };
            enum targetSpecifier {
                NONE,
                PKMN_TYPE1,
                PKMN_TYPE2,
                PKMN_TYPE3,
                PKMN_TYPE,

                PKMN_SIZE,

                PKMN_WEIGHT,

                PKMN_SPECIES,

                PKMN_ITEM,

                PKMN_ABILITY,

                PKMN_GENDER, //-1: Female, 0: Neutral, 1: Male

                PKMN_AILMENT,   //Burn, ...
                PKMN_STATUS,    //is Flying, etc

                PKMN_HP,
                PKMN_HP_PERCENT,
                PKMN_MAX_HP,
                PKMN_ATK,
                PKMN_DEF,
                PKMN_SPD,
                PKMN_SATK,
                PKMN_SDEF,
                PKMN_ACCURACY,
                PKMN_ATTACK_BLOCKED,
                PKMN_KO, //Ko moves

                PKMN_LEVEL,

                BATTLE_ROUND,
                BATTLE_MODE,
                BATTLE_WEATHER,
                BATTLE_WEATHER_LENGTH,
                BATTLE_LAST_MOVE,
                BATTLE_LAST_OWN_MOVE,
                BATTLE_LAST_OPP_MOVE,
                BATTLE_OWN_FST_MOVE,
                BATTLE_OWN_SND_MOVE,
                BATTLE_OPP_FST_MOVE,
                BATTLE_OPP_SND_MOVE,
                BATTLE_OWN_TEAMSIZE,
                BATTLE_OPP_TEAMSIZE,

                BATTLE_MOVE_TARGET,

                MOVE_DAMAGE,
                MOVE_REPEAT,
                MOVE_CRITICAL_RATIO,
                MOVE_MAX_ROUNDS,
                MOVE_ROUND,

                PLAYER_MONEY,

                RAND_MAX_0,
                RAND_MAX_10 = RAND_MAX_0 + 10,
                RAND_MAX_50 = RAND_MAX_0 + 50,
                RAND_MAX_100 = RAND_MAX_0 + 100
            };

            enum action {
                SET,
                ADD,
                MULTIPLY, //Multiply target by value and divide it by 100
                SWITCH,
                END,
                DEFER,    //Don't use a move
            };
            enum comp {
                EQUALS,
                NOT_EQUALS,
                GREATER,
                LESS,
                GEQ,
                LEQ
            };

            enum ctr {
                IF,
                IFN,
                ELSE,
                WHILE
            };

            struct condition {
            public:
                ctr             m_control;
                target          m_target;
                targetSpecifier m_targetSpecifier;
                comp            m_comp;
                int             m_value;
                bool            m_asLastCondition;

                condition( ctr p_control = IF )
                    : m_control( p_control ),
                    m_asLastCondition( true ) {
                }
                condition( target p_target,
                           targetSpecifier p_targetSpecifier,
                           comp p_comp,
                           int p_value )
                    : m_control( IF ),
                    m_target( p_target ),
                    m_targetSpecifier( p_targetSpecifier ),
                    m_comp( p_comp ),
                    m_value( p_value ),
                    m_asLastCondition( false ) { }

                bool            check( battle& p_battle, void* p_self ) const;
                bool            evaluate( int p_other ) const;
            };

            struct value {
            public:
                target          m_target;
                targetSpecifier m_targetSpecifier;
                float           m_multiplier;
                int             m_additiveConstant;

                value( int p_value )
                    : m_target( NO_TARGET ),
                    m_targetSpecifier( NONE ),
                    m_multiplier( 0 ),
                    m_additiveConstant( p_value ) { }

                value( target          p_target,
                       targetSpecifier p_targetSpecifier,
                       float           p_multiplier )
                    : m_target( p_target ),
                    m_targetSpecifier( p_targetSpecifier ),
                    m_multiplier( p_multiplier ),
                    m_additiveConstant( 0 ) { }

                value( target          p_target,
                       targetSpecifier p_targetSpecifier,
                       float           p_multiplier,
                       int             p_additiveConstant )
                    : m_target( p_target ),
                    m_targetSpecifier( p_targetSpecifier ),
                    m_multiplier( p_multiplier ),
                    m_additiveConstant( p_additiveConstant ) { }

                int             get( battle& p_battle, void* p_self )const;
                int             get( battle& p_battle, bool p_targetIsOpp, u8 p_targetPosition )const;
                int             get( battle& p_target )const;
            };

            std::vector<condition>  m_conditions;
            target                  m_target;
            targetSpecifier         m_targetSpecifier;
            action                  m_action;
            value                   m_value;

            std::string            m_log;

            command( std::string& p_log )
                : m_conditions( { } ),
                m_target( NO_TARGET ),
                m_targetSpecifier( NONE ),
                m_action( SET ),
                m_value( 0 ),
                m_log( p_log ) { }
            command( const char* p_log )
                : m_conditions( { } ),
                m_target( NO_TARGET ),
                m_targetSpecifier( NONE ),
                m_action( SET ),
                m_value( 0 ),
                m_log( p_log ) { }

            command( target                 p_target,
                     targetSpecifier        p_targetSpecifier,
                     value                  p_value )
                : m_target( p_target ),
                m_targetSpecifier( p_targetSpecifier ),
                m_action( SET ),
                m_value( p_value ) {
            }
            command( target                 p_target,
                     targetSpecifier        p_targetSpecifier,
                     action                 p_action,
                     value                  p_value )
                : m_target( p_target ),
                m_targetSpecifier( p_targetSpecifier ),
                m_action( p_action ),
                m_value( p_value ) {
            }

            command( std::vector<condition>  p_conditions,
                     target                  p_target,
                     targetSpecifier         p_targetSpecifier,
                     action                  p_action,
                     int                     p_value )
                : m_conditions( p_conditions ),
                m_target( p_target ),
                m_targetSpecifier( p_targetSpecifier ),
                m_action( p_action ),
                m_value( p_value ),
                m_log( "" ) { }

            command( std::vector<condition>  p_conditions,
                     target                  p_target,
                     targetSpecifier         p_targetSpecifier,
                     action                  p_action,
                     int                     p_value,
                     std::string&            p_log )
                : m_conditions( p_conditions ),
                m_target( p_target ),
                m_targetSpecifier( p_targetSpecifier ),
                m_action( p_action ),
                m_value( p_value ),
                m_log( p_log ) { }
            command( std::vector<condition>  p_conditions,
                     target                  p_target,
                     targetSpecifier         p_targetSpecifier,
                     action                  p_action,
                     int                     p_value,
                     const char*          p_log )
                : m_conditions( p_conditions ),
                m_target( p_target ),
                m_targetSpecifier( p_targetSpecifier ),
                m_action( p_action ),
                m_value( p_value ),
                m_log( p_log ) { }


            command( std::vector<condition>  p_conditions,
                     target                  p_target,
                     targetSpecifier         p_targetSpecifier,
                     action                  p_action,
                     value                   p_value,
                     const char*             p_log = "" )
                : m_conditions( p_conditions ),
                m_target( p_target ),
                m_targetSpecifier( p_targetSpecifier ),
                m_action( p_action ),
                m_value( p_value ),
                m_log( p_log ) { }


            void                    execute( battle& p_battle, void* p_self )const;
            void                    evaluateOnTargetVal( battle& p_battle, void* p_self, bool p_targetIsOpp, u8 p_targetPosition )const;
            void                    evaluateOnTargetVal( battle& p_battle, void* p_self )const;
        };

        std::vector<command>            _commands;
    public:
        battleScript( ) { }

        battleScript( std::vector<command> p_commands )
            : _commands( p_commands ) { }

        void                            execute( battle& p_battle, void* p_self ) const;
    };

    extern battleScript weatherEffects[ 9 ];
    typedef battleScript::command               cmd;
    typedef battleScript::command::condition    con;
    typedef battleScript::command::value        val;

#define NEQ( pkmn, var, val ) ( con( cmd::pkmn, cmd::var, cmd::NOT_EQUALS, val ) )
#define EQ( pkmn, var, val ) ( con( cmd::pkmn, cmd::var, cmd::EQUALS, val ) )
#define GT( pkmn, var, val ) ( con( cmd::pkmn, cmd::var, cmd::GREATER, val ) )
#define LS( pkmn, var, val ) ( con( cmd::pkmn, cmd::var, cmd::LESS, val ) )
#define GEQ( pkmn, var, val ) ( con( cmd::pkmn, cmd::var, cmd::GEQ, val ) )
#define LEQ( pkmn, var, val ) ( con( cmd::pkmn, cmd::var, cmd::LEQ, val ) )
}