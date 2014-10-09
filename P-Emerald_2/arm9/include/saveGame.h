/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.h
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

#include <string>
#include <vector>
#include "bag.h"

namespace POKEMON {
    class PKMN;
}
class BOX;
enum SavMod {
    _NDS,
    _GBA
};
class saveGame {
public:
    typedef std::pair<int, int> pii;

    //General Player Data
    std::wstring        m_playername;
    bool                m_isMale;
    int                 m_Id;
    int                 m_Sid;
    union {
        u32         m_playtime;
        struct {
            u16 m_hours;
            u8  m_mins;
            u8  m_secs;

        }           m_pt;
    };
    int                 m_HOENN_Badges;
    int                 m_KANTO_Badges;
    int                 m_JOHTO_Badges;
    short               m_badges;
    int                 m_dex;
    std::vector<bool>   m_inDex;

    int                 m_savTyp;

    int                 m_money;

    bool                m_hasPKMN;
    bool                m_activatedPNav;

    //Bag
    bag                 m_bag;

    //Team
    std::vector < POKEMON::PKMN >
        m_PkmnTeam;

    u16                 m_overWorldIdx;

    u16                 m_flags[ 500 ];

    int                 m_acposx,
        m_acposy,
        m_acposz;
    int                 m_acMapIdx;
    int                 m_acMoveMode;
    char                m_acMapName[ 100 ];

    bool                m_EXPShareEnabled;
    bool                m_evolveInBattle;

    //GameFlags
    bool                m_hasGDex;

    bool                m_good; //FailBit

    struct {
        u16 m_gameid;
    }                   m_gba;

    saveGame( ) :
        m_playername( ), m_isMale( true ), m_Id( 0 ), m_Sid( 0 ) {
        m_inDex = std::vector<bool>( 650, false );
        m_good = true;
    }
    saveGame( std::wstring p_playername )
        : m_playername( p_playername ), m_isMale( true ), m_Id( 0 ), m_Sid( 0 ) {
        m_inDex = std::vector<bool>( 650, false );
        m_good = true;
    }
    saveGame( void p_func( int ) );
    ~saveGame( ) { }

    bool                save( void p_func( int ) );

    std::wstring        getName( ) {
        return m_playername;
    }
    void                setName( const std::wstring& p_playername ) {
        m_playername = p_playername;
    }

    bool                checkflag( int p_idx ) {
        return m_flags[ p_idx >> 3 ] & ( 1 << ( p_idx % 8 ) );
    }
    void                setflag( int p_idx, bool p_value ) {
        if( p_value != checkflag( p_idx ) )
            m_flags[ p_idx >> 3 ] ^= ( 1 << ( p_idx % 8 ) );
        return;
    }
};