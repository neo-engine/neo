/*
Pokémon Emerald 2 Version
------------------------------

file        : script.h
author      : Philip Wellnitz 
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2015
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

class pokemon;

namespace BATTLE {

    class battle;

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
                SELF_PKMN,
                SELF_BATTLE,
                SELF_DAMAGE
            };
            enum targetSpecifier {
                NONE,
                PKMN_TYPE1,
                PKMN_TYPE2,
                PKMN_TYPE1o2,

                PKMN_SIZE,

                PKMN_WEIGHT,

                PKMN_SPECIES,

                PKMN_ITEM,

                PKMN_ABILITY,

                PKMN_GENDER, //-1: Female, 0: Neutral, 1: Male

                PKMN_STATUS,

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
                BATTLE_OPP_TEAMSIZE
            };

            enum action {
                SET,
                ADD,
                MULTIPLY, //Multiply target by value and divide it by 100
                SWITCH,
                END
            };
            enum comp {
                EQUALS,
                NOT_EQUALS,
                GREATER,
                LESS,
                GEQ,
                LEQ
            };

            struct condition {
            public:
                target          m_target;
                targetSpecifier m_targetSpecifier;
                comp            m_comp;
                int             m_value;

                condition( ) { }
                condition( target p_target,
                           targetSpecifier p_targetSpecifier,
                           comp p_comp,
                           int p_value )
                           : m_target( p_target ),
                           m_targetSpecifier( p_targetSpecifier ),
                           m_comp( p_comp ),
                           m_value( p_value ) { }

                bool            check( battle& p_battle, void* p_self ) const;
                bool            evaluate( int p_other ) const;
                int             getTargetVal( const battle& p_battle, const pokemon& p_target, bool p_targetIsOpp, u8 p_targetPosition ) const;
                int             getTargetVal( const battle& p_target ) const;
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
                int             get( battle& p_battle, pokemon& p_target, bool p_targetIsOpp, u8 p_targetPosition )const;
                int             get( battle& p_target )const;
            };

            std::vector<condition>  m_conditions;
            target                  m_target;
            targetSpecifier         m_targetSpecifier;
            action                  m_action;
            value                   m_value;

            std::wstring            m_log;

            command( std::wstring& p_log )
                : m_conditions( { } ),
                m_target( NO_TARGET ),
                m_targetSpecifier( NONE ),
                m_action( SET ),
                m_value( 0 ),
                m_log( p_log ) { }
            command( const wchar_t* p_log )
                : m_conditions( { } ),
                m_target( NO_TARGET ),
                m_targetSpecifier( NONE ),
                m_action( SET ),
                m_value( 0 ),
                m_log( p_log ) { }

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
                     m_log( L"" ) { }

            command( std::vector<condition>  p_conditions,
                     target                  p_target,
                     targetSpecifier         p_targetSpecifier,
                     action                  p_action,
                     int                     p_value,
                     std::wstring&           p_log )
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
                     const wchar_t*          p_log )
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
                     const wchar_t*          p_log )
                     : m_conditions( p_conditions ),
                     m_target( p_target ),
                     m_targetSpecifier( p_targetSpecifier ),
                     m_action( p_action ),
                     m_value( p_value ),
                     m_log( p_log ) { }


            void                    execute( battle& p_battle, void* p_self )const;
            void                    evaluateOnTargetVal( battle& p_battle, void* p_self, pokemon& p_target, bool p_targetIsOpp, u8 p_targetPosition )const;
            void                    evaluateOnTargetVal( battle& p_battle, void* p_self )const;
        };

        std::vector<command>            _commands;
    public:
        battleScript( ) { }

        battleScript( std::string p_path );

        battleScript( std::vector<command> p_commands )
            : _commands( p_commands ) { }

        void                            execute( battle& p_battle, void* p_self ) const;

        friend int                      getTargetSpecifierValue( const battle&                      p_battle,
                                                                 const pokemon&                        p_target,
                                                                 bool                                           p_targetIsOpp,
                                                                 u8                                             p_targetPosition,
                                                                 const battleScript::command::targetSpecifier&  p_targetSpecifier );
        friend int                      getTargetSpecifierValue( const battle&                                  p_target,
                                                                 const battleScript::command::targetSpecifier&  p_targetSpecifier );
    };
}