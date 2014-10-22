/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : battle.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */

#pragma once
#include <vector>

#include <nds.h>

#include "move.h"
#include "script.h"
#include "ability.h"


namespace POKEMON {
    class pokemon;
}
namespace ITEMS {
    class item;
}

struct SpriteInfo;

namespace BATTLE {
    typedef battleScript::command               cmd;
    typedef battleScript::command::condition    con;
    typedef battleScript::command::value        val;

    void displayHP( u16 HPstart, u16 HP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, bool big = false ); //HP in %
    void displayHP( u16 HPstart, u16 HP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, u8 innerR, u8 outerR ); //HP in %
    void displayEP( u16 EPstart, u16 EP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, u8 innerR = 14, u8 outerR = 15 );

    u16 calcDamage( const move& atk, const POKEMON::pokemon& atg, const POKEMON::pokemon& def, u8 rndVal );

    class battleTrainer {
    public:
        enum trainerClass {
            PKMN_TRAINER
        };

        const char*         m_battleTrainerName;
        trainerClass        m_trainerClass;
        std::vector < POKEMON::pokemon >
            *m_pkmnTeam;
    private:
        std::vector<ITEMS::item>   *_items;
        int                 _moneyEarned;
        const char          *_msg1,
            *_msg2,
            *_msg3,
            *_msg4;

    public:
        battleTrainer( const char* p_battleTrainerName,
                       const char* p_msg1,
                       const char* p_msg2,
                       const char* p_msg3,
                       const char* p_msg4,
                       std::vector<POKEMON::pokemon>* p_pkmnTeam,
                       std::vector<ITEMS::item>* p_items,
                       trainerClass p_trainerClass = PKMN_TRAINER )
                       : m_battleTrainerName( p_battleTrainerName ),
                       m_trainerClass( p_trainerClass ),
                       m_pkmnTeam( p_pkmnTeam ),
                       _items( p_items ),
                       _msg1( p_msg1 ),
                       _msg2( p_msg2 ),
                       _msg3( p_msg3 ),
                       _msg4( p_msg4 ) { }

        POKEMON::pokemon&      sendNewPKMN( bool p_choice = true );
        ITEMS::item& useItem( bool choice = true );

        const char*         getLooseMsg( ) const {
            return _msg4;
        }
        int                 getLooseMoney( ) const {
            return _moneyEarned;
        }
        const char*         getWinMsg( ) const {
            return _msg3;
        }
        const char*         getCriticalMsg( ) const {
            return _msg2;
        }
        const char*         getInitMsg( ) const {
            return _msg1;
        }
    };

    class battle {
    private:
        u16 _round,
            _maxRounds,
            _AILevel;
        const battleTrainer
            *_player,
            *_opponent;

        u8 _acPkmnPosition[ 6 ][ 2 ]; //me; opp; maps the Pkmn's positions in the teams to their real in-battle positions
        enum acStatus {
            OK = 0,
            STS = 1,
            KO = 2,
            NA = 3,
            SELECTED = 4
        }   _acPkmnStatus[ 6 ][ 2 ];

        move::ailment _acPkmnAilments[ 6 ][ 2 ];
        u8 _acPkmnAilmentCounts[ 6 ][ 2 ];

        bool _battleSpotOccupied[ 2 ][ 2 ];

#define ATK 0
#define DEF 1
#define SPD 2
#define SATK 3
#define SDEF 4
#define ACCURACY 5

        u8 _acPkmnStatChanges[ 6 ][ 2 ][ 10 ];

        struct battleMove {
        public:
            enum type {
                ATTACK,
                SWITCH,
                USE_ITEM,
                USE_NAV
            };
            type    m_type;
            u16     m_value;
        };

        battleMove  _battleMoves[ 2 ][ 2 ];
        u8          _moveOrder[ 2 ][ 2 ];
        u16         _lstOwnMove;
        u16         _lstOppMove;
        u16         _lstMove;

        battleScript _undoScript[ 6 ][ 2 ]; //script to undo changes done to the own pkmn

        bool        _restoreItem;
        u8          _weatherLength;

        battleScript _weatherEffects[ 6 ] = {
            battleScript( ),
            //Rain
            battleScript( std::vector<cmd>( {
                cmd( L"Es regnet.[A]" ),

                //Hydration
                cmd( std::vector<con>( { con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::EQUALS, A_HYDRATION ), con( cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::NOT_EQUALS, 0 ) } ),
                cmd::OWN1, cmd::PKMN_STATUS, cmd::SET, 0, L"[OWN1:ABILITY] von\n[OWN1] wirkt![A][CLEAR][OWN1] heilt sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::EQUALS, A_HYDRATION ), con( cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::NOT_EQUALS, 0 ) } ),
                cmd::OWN2, cmd::PKMN_STATUS, cmd::SET, 0, L"[OWN2:ABILITY] von\n[OWN2] wirkt![A][CLEAR][OWN2] heilt sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::EQUALS, A_HYDRATION ), con( cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::NOT_EQUALS, 0 ) } ),
                cmd::OPPONENT1, cmd::PKMN_STATUS, cmd::SET, 0, L"[OPP1:ABILITY] von\n[OPP1] wirkt![A][CLEAR][OPP1] heilt sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::EQUALS, A_HYDRATION ), con( cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::NOT_EQUALS, 0 ) } ),
                cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::SET, 0, L"[OPP2:ABILITY] von\n[OPP2] wirkt![A][CLEAR][OPP2] heilt sich.[A]" ),

                //Dry Skin
                cmd( std::vector<con>( { con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::EQUALS, A_DRY_SKIN ), con( cmd::OWN1, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 8 ), L"[OWN1:ABILITY] von\n[OWN1] wirkt![A][CLEAR]KP von [OWN1]\nregenerieren sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::EQUALS, A_DRY_SKIN ), con( cmd::OPPONENT2, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OWN2, cmd::PKMN_STATUS, cmd::SET, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 8 ), L"[OWN2:ABILITY] von\n[OWN2] wirkt![A][CLEAR]KP von [OWN2]\nregenerieren sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::EQUALS, A_DRY_SKIN ), con( cmd::OPPONENT2, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OPPONENT1, cmd::PKMN_STATUS, cmd::SET, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 8 ), L"[OPP1:ABILITY] von\n[OPP1] wirkt![A][CLEAR]KP von [OPP1]\nregenerieren sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::EQUALS, A_DRY_SKIN ), con( cmd::OPPONENT2, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::SET, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 8 ), L"[OPP2:ABILITY] von\n[OPP2] wirkt![A][CLEAR]KP von [OPP2]\nregenerieren sich.[A]" ),

                //Rain Dish
                cmd( std::vector<con>( { con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::EQUALS, A_RAIN_DISH ), con( cmd::OWN1, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 16 ), L"[OWN1:ABILITY] von\n[OWN1] wirkt![A][CLEAR]KP von [OWN1]\nregenerieren sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::EQUALS, A_RAIN_DISH ), con( cmd::OPPONENT2, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OWN2, cmd::PKMN_STATUS, cmd::SET, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 16 ), L"[OWN2:ABILITY] von\n[OWN2] wirkt![A][CLEAR]KP von [OWN2]\nregenerieren sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OPPONENT2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::EQUALS, A_RAIN_DISH ), con( cmd::OPPONENT2, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OPPONENT1, cmd::PKMN_STATUS, cmd::SET, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 16 ), L"[OPP1:ABILITY] von\n[OPP1] wirkt![A][CLEAR]KP von [OPP1]\nregenerieren sich.[A]" ),
                cmd( std::vector<con>( { con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_AIR_LOCK ), con( cmd::OWN1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OPPONENT1, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ), con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::NOT_EQUALS, A_CLOUD_NINE ),
                con( cmd::OWN2, cmd::PKMN_ABILITY, cmd::EQUALS, A_RAIN_DISH ), con( cmd::OPPONENT2, cmd::PKMN_HP_PERCENT, cmd::NOT_EQUALS, 100 ) } ),
                cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::SET, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 16 ), L"[OPP2:ABILITY] von\n[OPP2] wirkt![A][CLEAR]KP von [OPP2]\nregenerieren sich.[A]" ),
            } ) ),
            //Hail

            //Fog

            //Sandstorm

            //SUN
        };
        std::wstring _weatherMessage[ 6 ] = {
            L"",
            L"Es regnet.[A]",
            L"Es hagelt.[A]",
            L"Es herrscht dichter Nebel...[A]",
            L"Der Sandsturm wirkt.[A]",
            L"Das Sonnenlicht ist stark.[A]"
        };
        std::wstring _weatherEndMessage[ 6 ] = {
            L"",
            L"Der Regen stoppte.[A]",
            L"Der Hagel stoppte.[A]",
            L"Der Nebel verzog sich.[A]",
            L"Der Sandsturm verzog sich.[A]",
            L"Das Sonnenlicht wurde wieder normal.[A]"
        };

    public:
#define OPPONENT 1
#define PLAYER 0

#define ACPOS(i,p) _acPkmnPosition[ i ][ p ]
#define ACPKMNSTS(i,p) _acPkmnStatus[ ACPOS((i),(p)) ][ p ]
#define ACPKMNAIL(i,p) _acPkmnAilments[ ACPOS((i),(p)) ][ p ]
#define ACPKMNAILCNT(i,p) _acPkmnAilmentCounts[ ACPOS((i),(p)) ][ p ]
#define ACPKMN(i,p) (((p) == OPPONENT) ? (( *_opponent->m_pkmnTeam )[ ACPOS( (i), OPPONENT ) ]) : (( *_player->m_pkmnTeam )[ ACPOS( (i), PLAYER ) ]))

#define ACPOS2(b,i,p) (b)._acPkmnPosition[ i ][ p ]
#define ACPKMN2(b,i,p) (((p) == OPPONENT) ? (( *(b)._opponent->m_pkmnTeam )[ ACPOS2((b), (i), OPPONENT ) ]) : (( *(b)._player->m_pkmnTeam )[ ACPOS2( (b), (i), PLAYER ) ]))
#define ACPKMNUNDO2(b,i,p) (b)._undoScript[ ACPOS2((b),(i),(p)) ][ p ]
#define ACPKMNSTATCHG2(b,i,p) (b)._acPkmnStatChanges[ ACPOS2((b),(i),(p)) ][ p ]

        friend class battleScript;
        friend std::wstring parseLogCmd( const battle& p_battle, const std::wstring& p_cmd );
        friend int getTargetSpecifierValue( const battle& p_battle, 
                                            const POKEMON::pokemon& p_target, 
                                            bool p_targetIsOpp,
                                            u8 p_targetPosition, 
                                            const battleScript::command::targetSpecifier& p_targetSpecifier );
        friend int getTargetSpecifierValue( const battle& p_target, 
                                            const battleScript::command::targetSpecifier& p_targetSpecifier );

        enum weather {
            NO_WEATHER = 0,
            RAIN = 1,
            HAIL = 2,
            FOG = 3,
            SANDSTORM = 4,
            SUN = 5
        };

        enum battleMode {
            SINGLE = 0,
            DOUBLE = 1
        };

        enum battleEndReason {
            ROUND_LIMIT = 0,
            OPPONENT_WON = -1,
            PLAYER_WON = 1,
            NONE = 2
        };

        bool        m_distributeEXP;

        weather     m_weather;
        battleMode  m_battleMode;

        battle( battleTrainer* p_player,
                battleTrainer* p_opponent,
                int p_maxRounds,
                int p_AILevel = 5,
                battleMode p_battlemode = SINGLE );

        void        log( const std::wstring& p_message );

        s8          start( ); //Runs battle; returns -1 if opponent wins, 0 if tie, 1 otherwise

        void        initBattle( );
        void        trainerIntro( );

        void        refillBattleSpots( bool p_choice );
        u8          getNextPKMN( bool p_opponent );
        void        orderPKMN( ); //orders PKMN according to their speed, their move's priority, ... and stores result in _moveOrder
        void        hidePKMN( bool p_opponent, u8 p_pokemonPos );
        void        sendPKMN( bool p_opponent, u8 p_pokemonPos );
        void        switchPKMN( bool p_opponent, u8 p_toSwitch, u8 p_newPokemonPos );

        void        doAbilities( ability::abilityType p_situation );

        bool        canMove( u8 p_pokemonPos );
        void        declareBattleMove( u8 p_pokemonPos );
        void        declareBattleMoveChoose( );
        void        chooseAttack( );
        void        chooseItem( );
        void        run( );
        u8          choosePKMN( );
        void        useNav( );

        void        doMoves( );
        void        doMove( bool p_opponent, u8 p_pokemonPos );

        void        doWeather( );

        bool        endConditionHit( battleEndReason& p_battleEndReason );
        void        endBattle( battleEndReason p_battleEndReason );


        //old battle methods (deprecated)
        void        initBattleScreen( );

        int         start( int p_battleBack, weather p_weather ); //Runs battle; returns -1 if opponent wins, 1 otherwise

        void        switchOppPkmn( int p_newPkmn, int p_toSwitch = 0 );

        void        switchOwnPkmn( int p_newPkmn, int p_toSwitch = 0 );

        int         getChoice( int );
        int         getTarget( int, int );
        int         getSwitchPkmn( int&, int&, int&, bool );
        void        printAttackChoiceScreen( int, int&, int&, int& );
        void        printTargetChoiceScreen( int, int, int&, int&, int& );
        void        printPKMNSwitchScreen( int& os2, int& pS2, int& ts2 );

    private:
        void        initBattleScene( int p_battleBack, weather p_weather );
    };
}