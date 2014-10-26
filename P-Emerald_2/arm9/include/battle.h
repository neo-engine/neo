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
#include <map>
#include <set>

#include <nds.h>

#include "move.h"
#include "script.h"
#include "ability.h"
#include "sprite.h"
#include "print.h"


namespace POKEMON {
    class pokemon;
}
namespace ITEMS {
    class item;
}

extern OAMTable *Oam;
extern SpriteInfo spriteInfo[ SPRITE_COUNT ];

extern OAMTable *OamTop;
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

extern FONT::Font cust_font;
extern FONT::Font cust_font2;


namespace BATTLE {
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

        u16     _usedPals;

        s8      getNextFreePal( ) {
            for( u8 i = 0; i < 16; ++i )if( !( _usedPals & ( 1 << i ) ) ) {
                _usedPals |= ( 1 << i );
                return i;
            }
            return -1;
        }
        void    freePal( u16 p_palNo ) {
            _usedPals &= ~( 1 << p_palNo );
        }



    public:
        static void displayHP( u16 HPstart, u16 HP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, bool big = false ); //HP in %
        static void displayHP( u16 HPstart, u16 HP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, u8 innerR, u8 outerR ); //HP in %
        static void displayEP( u16 EPstart, u16 EP, u8 x, u8 y, u8 freecolor1, u8 freecolor2, bool delay, u8 innerR = 14, u8 outerR = 15 );

        void    initScreen( );
        void    clearScreen( );
        void    setTextColor( u16 p_color );
        void    setText2Color( u16 p_color );
        void    writeText( const std::wstring& p_message );
        void    waitForTouchUp( );
        void    waitForKeyUp( int p_key );

        void    init( );
        void    trainerIntro( );

        void    declareBattleMove( u8 p_pokemonPos );

        void    declareBattleMoveChoose( );
        void    chooseAttack( );
        void    chooseItem( );
        u8      choosePKMN( );
        void    useNav( );

        void    showAttack( u8 p_moveNo );
        void    updateHP( bool p_opponent, u8 p_pokemonPos ); 
        void    applyEXPChanges( );
        void    updateStats( bool p_opponent, u8 p_pokemonPos );

        void    hidePKMN( bool p_opponent, u8 p_pokemonPos );
        void    sendPKMN( bool p_opponent, u8 p_pokemonPos );
        void    evolvePKMN( bool p_opponent, u8 p_pokemonPos );

        void    learnMove( u8 p_pokemonPos, u16 p_move );

        void    dinit( );

        battleUI( ) { }
        battleUI( battle* p_battle )
            :_battle( p_battle ) { }
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

#define MAX_STATS 7
#define ATK 0
#define DEF 1
#define SPD 2
#define SATK 3
#define SDEF 4
#define ACCURACY 5
#define ATTACK_BLOCKED 6

        s8 _acPkmnStatChanges[ 6 ][ 2 ][ 10 ];

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
        };

        battleMove  _battleMoves[ 2 ][ 2 ];
        u8          _moveOrder[ 2 ][ 2 ];
        u16         _lstOwnMove;
        u16         _lstOppMove;
        u16         _lstMove;

        std::map<POKEMON::pokemon&, std::set<POKEMON::pokemon&> > _participatedPKMN;

        //Current turn's current move's "consequences"
        u8          _acMove;
        s16         _acDamage[ 2 ][ 2 ];
        u8          _critical[ 2 ][ 2 ];
        u8          _criticalChance[ 2 ][ 2 ];
        float       _effectivity[ 2 ][ 2 ];
        s8          _acStatChange[ 2 ][ 2 ][ 10 ];

        battleScript _undoScript[ 6 ][ 2 ]; //script to undo changes done to the own pkmn

        bool        _restoreItem;
        bool        _allowMegaEvolution;
        u8          _weatherLength;

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



        battleUI    _battleUI;
    public:
#define OPPONENT 1
#define PLAYER 0

#define ACPOS( p_pokemonPos, p_opponent ) _acPkmnPosition[ p_pokemonPos ][ p_opponent ]
#define ACPKMNSTS( p_pokemonPos, p_opponent ) _acPkmnStatus[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]
#define ACPKMNAIL( p_pokemonPos, p_opponent ) _acPkmnAilments[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]
#define ACPKMNAILCNT( p_pokemonPos, p_opponent ) _acPkmnAilmentCounts[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]
#define ACPKMN( p_pokemonPos, p_opponent ) ( ( ( p_opponent ) == OPPONENT) ? ( ( *_opponent->m_pkmnTeam )[ ACPOS( ( p_pokemonPos ), OPPONENT ) ] ) :\
                     ( ( *_player->m_pkmnTeam )[ ACPOS( ( p_pokemonPos ), PLAYER ) ]))
#define ACPKMNSTATCHG( p_pokemonPos, p_opponent ) _acPkmnStatChanges[ ACPOS( ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]

#define ACPOS2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._acPkmnPosition[ p_pokemonPos ][ p_opponent ]
#define ACPKMN2( p_battle, p_pokemonPos, p_opponent ) ( ( ( p_opponent ) == OPPONENT ) ? ( ( *( p_battle )._opponent->m_pkmnTeam )[ ACPOS2( ( p_battle ), ( p_pokemonPos ), OPPONENT ) ] ) :\
                     ( ( *( p_battle )._player->m_pkmnTeam )[ ACPOS2( ( p_battle ), ( p_pokemonPos ), PLAYER ) ] ) )
#define ACPKMNUNDO2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._undoScript[ ACPOS2( ( p_battle ), ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]
#define ACPKMNSTATCHG2( p_battle, p_pokemonPos, p_opponent ) ( p_battle )._acPkmnStatChanges[ ACPOS2( ( p_battle ), ( p_pokemonPos ), ( p_opponent ) ) ][ p_opponent ]

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
            NONE = 2
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

        void        log( const std::wstring& p_message );

        s8          start( ); //Runs battle; returns -1 if opponent wins, 0 if tie, 1 otherwise

        void        initBattle( );

        void        refillBattleSpots( bool p_choice );
        u8          getNextPKMN( bool p_opponent );
        void        orderPKMN( bool p_includeMovePriority = false ); //orders PKMN according to their speed, their move's priority, ... and stores result in _moveOrder
        void        switchPKMN( bool p_opponent, u8 p_toSwitch, u8 p_newPokemonPos );

        void        doItems( ability::abilityType p_situation );
        void        doItem( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation );

        void        doAbilities( ability::abilityType p_situation );
        void        doAbility( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation );

        void        getAIMoves( );
        bool        canMove( bool p_opponent, u8 p_pokemonPos );
        void        run( );

        void        registerParticipatedPKMN( );
        void        distributeEXP( bool p_opponent, u8 p_pokemonPos );
        void        evolve( bool p_opponent, u8 p_pokemonPos );
        void        megaEvolve( bool p_opponent, u8 p_pokemonPos );
        void        doMoves( );
        void        doMove( bool p_opponent, u8 p_pokemonPos );

        void        calcDamage( u8 p_moveNo, u8 p_randInt );
        void        doAttack( u8 p_moveNo );

        void        doWeather( );

        bool        endConditionHit( battleEndReason& p_battleEndReason );
        void        endBattle( battleEndReason p_battleEndReason );

        void        checkForAttackLearn( u8 p_pokemonPos );
        void        checkForEvolution( bool p_opponent, u8 p_pokemonPos );
    };
}