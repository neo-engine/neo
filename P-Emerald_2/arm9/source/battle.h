/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : battle.h
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
#include <vector>
#include <map>
#include <set>
#include <string>

#include <nds.h>

#include "battleUI.h"
#include "battlePokemon.h"
#include "battleWeather.h"
#include "script.h"
#include "defines.h"
#include "type.h"
#include "ability.h"
#include "pokemon.h"

class move;

namespace BATTLE {
    class battleTrainer;

    extern u8 firstMoveSwitchTarget;
    extern std::string weatherMessage[ 9 ];
    extern std::string weatherEndMessage[ 9 ];

    u16 calcDamage( const move& p_atk, const pokemon& p_atg, const pokemon& p_def, u8 p_rndVal );

#define MAX_STATS 8
#define ATK 0
#define DEF 1
#define SPD 2
#define SATK 3
#define SDEF 4
#define ACCURACY 5
#define EVASION 6
#define ATTACK_BLOCKED 7

    class battle {
    private:
        u16 _round,
            _maxRounds,
            _AILevel;
        battleTrainer
            *_player,
            *_opponent;

        u8 _acPkmnPosition[ 6 ][ 2 ]; //me; opp; maps the Pkmn's positions in the teams to their real in-battle positions

        bool _battleSpotOccupied[ 2 ][ 2 ];

        enum battleTerrain {
            NO_BATTLE_TERRAIN = 0,
            TRICK_ROOM = 1,
            //Todo
        };
        battleTerrain   _battleTerrain;

        struct battleMove {
            enum type {
                ATTACK,
                MEGA_ATTACK, //Mega evolve and attack
                SWITCH,
                USE_ITEM,
                USE_NAV,
                RUN
            };
            type    m_type;
            u16     m_value;
            u8      m_target;  //Bitflag: 0 Own1, 1 Own2, 2 Opp1, 3 Opp2, 4 Own Field, 5 OppField, m_target == 0 -> default target

            u32     m_newItemEffect;

            void clear( ) {
                m_target = 0;
                m_type = (type) 0;
                m_value = 0;
                m_newItemEffect = 0;
            }
        };

        battleMove  _battleMoves[ 2 ][ 2 ];
        u8          _moveOrder[ 2 ][ 2 ];
        u16         _lstOwnMove;
        u16         _lstOppMove;
        u16         _lstMove;

        u8          _participatedPKMN[ 6 ][ 2 ];

        //Current turn's current move's "consequences"
        s16         _acDamage[ 2 ][ 2 ];
        u8          _critical[ 2 ][ 2 ];
        u8          _criticalChance[ 2 ][ 2 ];
        float       _effectivity[ 2 ][ 2 ];
        s8          _acStatChange[ 2 ][ 2 ][ 10 ];
        bool        _currentMoveIsOpp;
        bool        _currentMoveIsSnd;

        bool        _restoreItem;
        bool        _allowMegaEvolution;
        u8          _weatherLength;

        bool        _endBattle = false;

        u16         _storedHP[ 2 ][ 2 ];
        battlePokemon _pkmns[ 6 ][ 2 ];
        battlePokemon _storedPkmns[ 6 ][ 2 ];
        battlePokemon _wildPokemon;

        static battleUI*    _battleUI;

        int         getTargetSpecifierValue( bool p_targetIsOpp, u8 p_targetPosition,
                                             const battleScript::command::targetSpecifier& p_targetSpecifier, u8 p_targetVal = 0 );
        int         getTargetSpecifierValue( const battleScript::command::targetSpecifier& p_targetSpecifier );
        std::string parseLogCmd( const std::string& p_cmd );
    public:
#define OPPONENT 1
#define PLAYER 0

#define CUR_POS( p_pokemonPos, p_opponent ) _acPkmnPosition[ p_pokemonPos ][ p_opponent ]
#define CUR_PKMN_STR( p_pokemonPos, p_opponent ) ( ( m_isWildBattle && (p_opponent) ) ? _wildPokemon : (_pkmns[ CUR_POS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]) )

#define CUR_PKMN_STS( p_pokemonPos, p_opponent ) CUR_PKMN_STR( p_pokemonPos, p_opponent ).m_acStatus
#define CUR_PKMN( p_pokemonPos, p_opponent ) (*(CUR_PKMN_STR( p_pokemonPos, p_opponent ).m_pokemon))
#define CUR_PKMN_STATCHG( p_pokemonPos, p_opponent ) CUR_PKMN_STR( p_pokemonPos, p_opponent ).m_acStatChanges

#define CUR_POS_2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._acPkmnPosition[ p_pokemonPos ][ p_opponent ]
#define CUR_PKMN_STR_2( p_battle, p_pokemonPos, p_opponent ) ( ( ( p_battle ).m_isWildBattle && (p_opponent) ) ? ( p_battle )._wildPokemon : \
                    ( ( p_battle )._pkmns[ CUR_POS_2( p_battle, p_pokemonPos, p_opponent ) ][ p_opponent ]) )
#define CUR_PKMN_STS_2( p_battle, p_pokemonPos, p_opponent ) CUR_PKMN_STR_2( p_battle, p_pokemonPos, p_opponent ).m_acStatus
#define CUR_PKMN_2( p_battle, p_pokemonPos, p_opponent ) (*(CUR_PKMN_STR_2( p_battle, p_pokemonPos, p_opponent ).m_pokemon) )
#define CUR_PKMN_STATCHG_2( p_battle, p_pokemonPos, p_opponent ) CUR_PKMN_STR_2( p_battle, p_pokemonPos, p_opponent ).m_acStatChanges

        friend class battleScript;
        friend class battleUI;
        
        enum battleMode {
            SINGLE = 0,
            DOUBLE = 1
        };

        enum battleEndReason {
            ROUND_LIMIT = 0,
            OPPONENT_WON = -1,
            PLAYER_WON = 1,
            NONE = 2,
            RUN = 3
        };

        bool        m_distributeEXP;
        bool        m_isWildBattle;

        u8          m_platformId;
        u8          m_backgroundId;

        weather     m_weather;
        battleMode  m_battleMode;

        battle( battleTrainer* p_player,
                battleTrainer* p_opponent,
                int p_maxRounds,
                weather p_weather,
                u8 p_platform,
                u8 p_background,
                int p_AILevel = 5,
                battleMode p_battlemode = SINGLE );
        battle( battleTrainer* p_player,
                pokemon* p_opponent,
                weather p_weather,
                u8 p_platform,
                u8 p_background );

        void        log( const std::string& p_message );

        s8          start( ); //Runs battle; returns -1 if opponent wins, 0 if tie, 1 otherwise

        void        initBattle( );

        void        refillBattleSpots( bool p_choice, bool p_send = true );
        u8          getNextPKMN( bool p_opponent, u8 p_startIdx = 0 );
        void        orderPKMN( bool p_includeMovePriority = false ); //orders PKMN according to their speed, their move's priority, … and stores result in _moveOrder
        void        switchPKMN( bool p_opponent, u8 p_toSwitch, u8 p_newPokemonPos );

        void        doItems( ability::abilityType p_situation );
        void        doItem( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation );

        void        doAbilities( ability::abilityType p_situation );
        void        doAbility( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation );

        void        getAIMoves( );
        bool        canMove( bool p_opponent, u8 p_pokemonPos );
        bool        run( );

        void        registerParticipatedPKMN( );
        void        distributeEXP( bool p_opponent, u8 p_pokemonPos );
        void        evolve( bool p_opponent, u8 p_pokemonPos );
        void        megaEvolve( bool p_opponent, u8 p_pokemonPos );
        void        doMoves( );
        void        doMove( u8 p_moveNo );

        s16         calcDamage( bool p_userIsOpp, u8 p_userPos, bool p_targetIsOpp, u8 p_targetPos );
        void        doAttack( bool p_opponent, u8 p_pokemonPos );

        void        storePkmnSts( bool p_opponent, u8 p_pokemonPos );
        void        updatePkmnSts( bool p_opponent, u8 p_pokemonPos );

        void        doWeather( );
        void        handleSpecialConditions( bool p_opponent, u8 p_pokemonPos );
        void        handleFaint( bool p_opponent, u8 p_pokemonPos, bool p_show = true );

        bool        tryCapture( u16 p_pokeBall );
        void        handleCapture( );

        bool        endConditionHit( battleEndReason& p_battleEndReason );
        void        endBattle( battleEndReason p_battleEndReason );

        void        checkForAttackLearn( u8 p_pokemonPos );
        void        checkForEvolution( bool p_opponent, u8 p_pokemonPos );
    };
}