/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : battle.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2015
    Philip Wellnitz (RedArceus)

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

#include <nds.h>

#include "move.h"
#include "script.h"
#include "ability.h"
#include "sprite.h"
#include "print.h"
#include "pokemon.h"
#include "defines.h"

namespace ITEMS {
    class item;
}

extern OAMTable *Oam;
extern SpriteInfo spriteInfo[ SPRITE_COUNT ];

extern OAMTable *OamTop;
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

extern FONT::font cust_font;
extern FONT::font cust_font2;

namespace BATTLE {
    extern u8 firstMoveSwitchTarget;

    typedef battleScript::command               cmd;
    typedef battleScript::command::condition    con;
    typedef battleScript::command::value        val;


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

    class battleUI {
        battle* _battle;
        s8      _oldPKMNStats[ 6 ][ 2 ][ 10 ];

    public:
        static void displayHP( u16 HPstart, u16 HP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, bool big = false ); //HP in %
        static void displayHP( u16 HPstart, u16 HP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, u8 innerR, u8 outerR, bool p_sub = false ); //HP in %
        static void displayEP( u16 EPstart, u16 EP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, u8 innerR = 14, u8 outerR = 15, bool p_sub = false );

        static void initLogScreen( );
        static void clearLogScreen( );
        static void setLogTextColor( u16 p_color );
        static void setLogText2Color( u16 p_color );
        static void writeLogText( const std::wstring& p_message );
        static void waitForTouchUp( );
        static void waitForKeyUp( int p_key );

        void    init( );
        void    trainerIntro( );

        bool    declareBattleMove( u8 p_pokemonPos, bool p_showBack );

        u16     chooseAttack( u8 p_pokemonPos );
        u8      chooseAttackTarget( u8 p_pokemonPos, u16 p_moveNo );
        u16     chooseItem( u8 p_pokemonPos );
        u8      choosePKMN( bool p_firstIsChosen, bool p_back = true );
        void    useNav( );

        void    showAttack( bool p_opponent, u8 p_pokemonPos );
        void    updateHP( bool p_opponent, u8 p_pokemonPos );
        void    showStatus( bool p_opponent, u8 p_pokemonPos );
        void    updateStatus( bool p_opponent, u8 p_pokemonPos );
        void    applyEXPChanges( bool p_opponent, u8 p_pokemonPos, u32 p_gainedExp );
        void    updateStats( bool p_opponent, u8 p_pokemonPos, bool p_move = true );

        void    hidePKMN( bool p_opponent, u8 p_pokemonPos );
        void    sendPKMN( bool p_opponent, u8 p_pokemonPos );
        void    evolvePKMN( bool p_opponent, u8 p_pokemonPos );

        void    learnMove( u8 p_pokemonPos, u16 p_move );

        void    showEndScreen( );

        void    dinit( );

        battleUI( ) { }
        battleUI( battle* p_battle )
            :_battle( p_battle ) { }
    };

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
    public:
        enum acStatus {
            OK = 0,
            STS = 1,
            KO = 2,
            NA = 3,
            SELECTED = 4
        };
    private:
        u16 _round,
            _maxRounds,
            _AILevel;
        const battleTrainer
            *_player,
            *_opponent;

        u8 _acPkmnPosition[ 6 ][ 2 ]; //me; opp; maps the Pkmn's positions in the teams to their real in-battle positions

        bool _battleSpotOccupied[ 2 ][ 2 ];

        enum battleStatus {
            NO_BATTLE_STATUS = 0,
            DIVE,
            FLY,
            DEFAULTED, //Pokemon used eg Hyperbeam last Turn
            PROTECTED
            //Todo ...
        };

        enum battleTerrain {
            NO_BATTLE_TERRAIN = 0,
            TRICK_ROOM = 1,
            //Todo
        };
        battleTerrain   _battleTerrain;

        struct battleMove {
        public:
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

            void clear( ) {
                m_target = 0;
                m_type = (type)0;
                m_value = 0;
            }
        };
        struct battlePokemon {
        public:
            POKEMON::pokemon*   m_pokemon;
            s8                  m_acStatChanges[ 10 ];
            Type                m_Types[ 3 ];
            battle::acStatus    m_acStatus;
            move::ailment       m_ailments;
            u8                  m_ailmentCount;
            battleStatus        m_battleStatus;
            u8                  m_toxicCount;
        };

        battleMove  _battleMoves[ 2 ][ 2 ];
        u8          _moveOrder[ 2 ][ 2 ];
        u16         _lstOwnMove;
        u16         _lstOppMove;
        u16         _lstMove;

        std::map<POKEMON::pokemon*, u8 > _participatedPKMN;

        //Current turn's current move's "consequences"
        s16         _acDamage[ 2 ][ 2 ];
        u8          _critical[ 2 ][ 2 ];
        u8          _criticalChance[ 2 ][ 2 ];
        float       _effectivity[ 2 ][ 2 ];
        s8          _acStatChange[ 2 ][ 2 ][ 10 ];
        bool        _currentMoveIsOpp;
        bool        _currentMoveIsSnd;

        battleScript _undoScript[ 6 ][ 2 ]; //script to undo changes done to the own pkmn

        bool        _restoreItem;
        bool        _allowMegaEvolution;
        u8          _weatherLength;

        bool        _endBattle = false;

        battleScript _weatherEffects[ 9 ] = {
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
        std::wstring _weatherMessage[ 9 ] = {
            L"",
            L"Es regnet.[A]",
            L"Es hagelt.[A]",
            L"Es herrscht dichter Nebel...[A]",
            L"Der Sandsturm wirkt.[A]",
            L"Das Sonnenlicht ist stark.[A]",
            L"Enormer Regen.[A]",
            L"Extremes Sonnenlicht.[A]",
            L"Starke Winde wehen.[A]"
        };
        std::wstring _weatherEndMessage[ 9 ] = {
            L"",
            L"Der Regen stoppte.[A]",
            L"Der Hagel stoppte.[A]",
            L"Der Nebel verzog sich.[A]",
            L"Der Sandsturm verzog sich.[A]",
            L"Das Sonnenlicht wurde wieder normal.[A]",
            L"Der Regen stoppte.[A]",
            L"Das Sonnenlicht wurde wieder normal.[A]",
            L"Windstille...[A]"
        };

        battlePokemon _pkmns[ 6 ][ 2 ];

        battleUI    _battleUI;
    public:
#define OPPONENT 1
#define PLAYER 0

#define ACPOS( p_pokemonPos, p_opponent ) _acPkmnPosition[ p_pokemonPos ][ p_opponent ]
#define ACPKMNSTS( p_pokemonPos, p_opponent ) (_pkmns[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]).m_acStatus
#define ACPKMNAIL( p_pokemonPos, p_opponent ) _pkmns[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_ailments
#define ACPKMNAILCNT( p_pokemonPos, p_opponent ) _pkmns[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_ailmentCount
#define ACPKMN( p_pokemonPos, p_opponent ) (*(_pkmns[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_pokemon))
#define ACPKMNSTATCHG( p_pokemonPos, p_opponent ) _pkmns[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_acStatChanges
#define ACPKMNSTR( p_pokemonPos, p_opponent ) (_pkmns[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ])

#define ACPOS2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._acPkmnPosition[ p_pokemonPos ][ p_opponent ]
#define ACPKMNSTS2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._pkmns[ ACPOS2( ( p_battle ), ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_acStatus
#define ACPKMN2( p_battle, p_pokemonPos, p_opponent ) (*(( p_battle )._pkmns[ ACPOS2( ( p_battle ), ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_pokemon))
#define ACPKMNSTATCHG2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._pkmns[ ACPOS2( ( p_battle ), ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ].m_acStatChanges

        friend class battleScript;
        friend class battleUI;
        friend std::wstring parseLogCmd( battle& p_battle, const std::wstring& p_cmd );
        friend int getTargetSpecifierValue( const battle& p_battle,
                                            const POKEMON::pokemon& p_target,
                                            bool p_targetIsOpp,
                                            u8 p_targetPosition,
                                            const battleScript::command::targetSpecifier& p_targetSpecifier );
        friend int getTargetSpecifierValue( const battle& p_target,
                                            const battleScript::command::targetSpecifier& p_targetSpecifier );

        friend  u16 initStsBalls( bool p_bottom, battle* p_battle, u16& p_tilecnt );
        friend void loadSpritesSub( battle* p_battle );
        friend void drawPKMNChoiceScreen( battle* p_battle, bool p_firstIsChosen );

        enum weather {
            NO_WEATHER = 0,
            RAIN = 1,
            HAIL = 2,
            FOG = 3,
            SANDSTORM = 4,
            SUN = 5,
            HEAVY_RAIN = 6,
            HEAVY_SUNSHINE = 7,
            HEAVY_WINDS = 8
        };

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

        weather     m_weather;
        battleMode  m_battleMode;

        battle( battleTrainer* p_player,
                battleTrainer* p_opponent,
                int p_maxRounds,
                int p_AILevel = 5,
                battleMode p_battlemode = SINGLE );
        ~battle( );

        void        log( const std::wstring& p_message );

        s8          start( ); //Runs battle; returns -1 if opponent wins, 0 if tie, 1 otherwise

        void        initBattle( );

        void        refillBattleSpots( bool p_choice, bool p_send = true );
        u8          getNextPKMN( bool p_opponent, u8 p_startIdx = 0 );
        void        orderPKMN( bool p_includeMovePriority = false ); //orders PKMN according to their speed, their move's priority, ... and stores result in _moveOrder
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

        void        doWeather( );
        void        handleSpecialConditions( bool p_opponent, u8 p_pokemonPos );
        void        handleFaint( bool p_opponent, u8 p_pokemonPos, bool p_show = true );

        bool        endConditionHit( battleEndReason& p_battleEndReason );
        void        endBattle( battleEndReason p_battleEndReason );

        void        checkForAttackLearn( u8 p_pokemonPos );
        void        checkForEvolution( bool p_opponent, u8 p_pokemonPos );
    };
}