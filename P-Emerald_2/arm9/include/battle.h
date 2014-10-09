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

namespace POKEMON {
    class PKMN;
}
class item;
struct SpriteInfo;

namespace BATTLE {
    void displayHP( int HPstart, int HP, int x, int y, int freecolor1, int freecolor2, bool delay, bool big = false ); //HP in %
    void displayHP( int HPstart, int HP, int x, int y, int freecolor1, int freecolor2, bool delay, int innerR, int outerR ); //HP in %
    void displayEP( int EPstart, int EP, int x, int y, int freecolor1, int freecolor2, bool delay, int innerR = 14, int outerR = 15 );

    int calcDamage( const move& atk, const POKEMON::PKMN& atg, const POKEMON::PKMN& def, int rndVal );

    class battleTrainer {
    public:
        enum trainerClass {
            PKMN_TRAINER
        };

        const char*         m_battleTrainerName;
        trainerClass        m_trainerClass;
        std::vector < POKEMON::PKMN >
            *m_pkmnTeam;
    private:
        std::vector<item>   *_items;
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
                       std::vector<POKEMON::PKMN>* p_pkmnTeam,
                       std::vector<item>* p_items,
                       trainerClass p_trainerClass = PKMN_TRAINER )
                       : m_battleTrainerName( p_battleTrainerName ),
                       m_trainerClass( p_trainerClass ),
                       m_pkmnTeam( p_pkmnTeam ),
                       _items( p_items ),
                       _msg1( p_msg1 ),
                       _msg2( p_msg2 ),
                       _msg3( p_msg3 ),
                       _msg4( p_msg4 ) { }

        POKEMON::PKMN&      sendNewPKMN( bool p_choice = true );
        item& useItem( bool choice = true );

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
        int _round,
            _maxRounds,
            _AILevel;
        const battleTrainer
            *_player,
            *_opponent;

        int _acPkmnPosition[ 6 ][ 2 ]; //me; opp; maps the Pkmn's positions in the teams to their real in-battle positions
        enum acStatus {
            OK = 0,
            STS = 1,
            KO = 2,
            NA = 3
        }   _acPkmnStatus[ 6 ][ 2 ];

        move::ailment _acPkmnAilments[ 6 ][ 2 ];
        int _acPkmnAilmentCounts[ 6 ][ 2 ];

    public:
        enum weather {
            NONE = 0,
            RAIN = 1,
            HAIL = 2,
            FOG = 3,
            SANDSTORM = 4,
            SUN = 5
        };

        enum battleMode {
            SINGLE = 0,
            MULTI = 1,
            DOUBLE = 2
        };

        bool        m_distributeEXP;

        weather     m_weather;
        battleMode  m_battleMode;

        battle( battleTrainer* p_player,
                battleTrainer* p_opponent,
                int p_maxRounds,
                int p_AILevel = 5,
                battleMode p_battlemode = SINGLE );

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