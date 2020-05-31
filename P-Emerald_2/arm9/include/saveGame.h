/*
    Pokémon neo
    ------------------------------

    file        : saveGame.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2020
    Philip Wellnitz

    This file is part of Pokémon neo.

    Pokémon neo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pokémon neo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
    */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <nds.h>
#include "bag.h"
#include "box.h"
#include "pokemon.h"
#include "mapObject.h"
#include "saveOptions.h"

namespace SAVE {
#define MAX_SAVE_FILES 3
#define MAX_CHAPTERS 12
#define MAX_SPECIAL_EPISODES 1

#define getActiveFile( ) m_saveFile[ SAVE::SAV.m_activeFile ]

#define GOOD_MAGIC1 0x01234567
#define GOOD_MAGIC2 0xFEDCBA98

    const u8 F_MEGA_EVOLUTION = 1;

    extern const char* const CHAPTER_NAMES[ 2 * MAX_CHAPTERS ][ LANGUAGES ];
    extern const char*       EPISODE_NAMES[ LANGUAGES ][ MAX_SPECIAL_EPISODES ];

    enum gameType {
        UNUSED   = 0,
        NORMAL   = 1,
        TRANSFER = 2,

        SPECIAL = 3
    };

    struct saveGame {
        struct playerInfo {
            u32      m_good1 = 0;

            gameType m_gameType;
            u8       m_chapter;
            u8       m_isMale;
            char     m_playername[ 12 ];
            u16      m_id;
            u16      m_sid;
            union {
                u32 m_playtime;
                struct {
                    u16 m_hours;
                    u8  m_mins;
                    u8  m_secs;

                } m_pt;
            };
            u8             m_HOENN_Badges;
            u8             m_KANTO_Badges;
            u8             m_JOHTO_Badges;
            u32            m_money;
            u32            m_coins;
            u32            m_battlePoints;
            MAP::mapObject m_player;
            u8             m_currentMap;
            u8             m_stepCount;
            MAP::warpPos   m_lastWarp;
            u8             m_curBox;
            u16            m_lstDex;
            u16            m_lstUsedItems[ 5 ];
            u8             m_lstUsedItemsIdx;

            u32            m_good2 = 0;

            u16            m_registeredItem;
            u8             m_lstBag;
            u16            m_lstBagItem;
            s16            m_repelSteps;
            saveOptions    m_options;
            pokemon        m_pkmnTeam[ 6 ];
            u16            m_vars[ 256 ];
            u16            m_flags[ 244 ];
            BAG::bag       m_bag;

            // Methods 'n' stuff
            void clear( );
            void initialize( );
            bool checkFlag( u8 p_idx );
            void setFlag( u8 p_idx, bool p_value );
            void stepIncrease( );
            u8   getEncounterLevel( u8 p_tier );
            u8   getBadgeCount( );
            u8   getTeamPkmnCount( );

            constexpr bool isGood( ) {
                return m_good1 == GOOD_MAGIC1 && m_good2 == GOOD_MAGIC2;
            }
        } m_saveFile[ MAX_SAVE_FILES ];

        u8 m_activeFile;

// Things shared among all save files

// Stored Pkmn
#define MAX_BOXES 40
        BOX::box            m_storedPokemon[ MAX_BOXES ];

        u8 m_caughtPkmn[ 1 + MAX_PKMN / 8 ];
        u8 m_seenPkmn[ 1 + MAX_PKMN / 8 ];

        u8 m_transfersRemaining; // Times a GBA save file can be copied

        // Return the idx of the resulting Box
        s8 storePkmn( const boxPokemon& p_pokemon );
        s8 storePkmn( const pokemon& p_pokemon );

        u16 countPkmn( u16 p_pkmnIdx );

        u16 getDexCount( );

        BOX::box* getCurrentBox( );

        bool isGood( );
        void clear( );
    };

    extern saveGame SAV;
}
