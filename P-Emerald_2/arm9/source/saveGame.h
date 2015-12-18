/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.h
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

#include <nds.h>
#include <string>
#include <vector>
#include "bag.h"
#include "box.h"
#include "pokemon.h"
#include "mapObject.h"
#include "battle.h"

namespace FS {
    enum SavMod {
        _NDS,
        _GBA
    };
    extern SavMod savMod;

    extern std::vector<pokemon> tmp;
    struct saveGame {
        //general stuff
        wchar_t     m_playername[ 12 ];
        u8          m_isMale : 1;
        u16         m_id;
        u16         m_sid;
        union {
            u32     m_playtime;
            struct {
                u16 m_hours;
                u8  m_mins;
                u8  m_secs;

            }       m_pt;
        };
        u8          m_HOENN_Badges;
        u8          m_KANTO_Badges;
        u8          m_JOHTO_Badges;
        u8          m_savTyp : 3;
        u8          m_inDex[ 1 + MAX_PKMN / 8 ];
        u32         m_money;

        //Bag stuff
        BAG::bag*   m_bag; //Be VERY CAREFUL when deleting savegames or when just using them!
        u8          m_lstBag;
        u8          m_lstBagItem;

        u8          m_repelSteps;

        pokemon     m_pkmnTeam[ 6 ];

        //Stored Pkmn
        BOX::box*   m_storedPokemon; //And I really mean careful

        //Map stuff
        MAP::mapObject m_player;
        u8          m_currentMap;

        u8          m_EXPShareEnabled : 1;
        u8          m_evolveInBattle : 1;
        u8          m_bgIdx;

        u16         m_flags[ 500 ];

        u8          m_hasGDex : 1;
        u8          m_activatedPNav;

        bool        checkflag( u8 p_idx ) {
            return m_flags[ p_idx >> 3 ] & ( 1 << ( p_idx % 8 ) );
        }
        void        setflag( u8 p_idx, bool p_value ) {
            if( p_value != checkflag( p_idx ) )
                m_flags[ p_idx >> 3 ] ^= ( 1 << ( p_idx % 8 ) );
            return;
        }
        void        stepIncrease( );
        u8          getEncounterLevel( u8 p_tier );
        u8          getBadgeCount( ) {
            u8 cnt = 0;
            for( u8 i = 0; i < 8; ++i ) {
                cnt += !!( m_HOENN_Badges & ( 1 << i ) );
                cnt += !!( m_KANTO_Badges & ( 1 << i ) );
                cnt += !!( m_JOHTO_Badges & ( 1 << i ) );
            }
            return cnt;
        }
        BATTLE::battleTrainer* getBattleTrainer( ) {
            tmp.clear( );
            for( u8 i = 0; i < 6; ++i )
                if( m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                    tmp.push_back( m_pkmnTeam[ i ] );
                else
                    break;
            char buffer[ 30 ];
            sprintf( buffer, "%ls", m_playername );

            static BATTLE::battleTrainer res( std::string(buffer), "", "", "", "", tmp, m_bag->getBattleItems( ) );
            return &res;
        }
        void updateTeam( ) {
            for( u8 i = 0; i < tmp.size( ); ++i )
                m_pkmnTeam[ i ] = tmp[ i ];
        }
    };

    saveGame* readSave( );
    bool writeSave( saveGame* p_saveGame );

    extern saveGame* SAV;
}