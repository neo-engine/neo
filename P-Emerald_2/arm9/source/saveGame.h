/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.h
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

#include <nds.h>
#include <string>
#include <vector>
#include "bag.h"
#include "box.h"
#include "mapObject.h"
#include "saveOptions.h"

struct pokemon;

namespace BATTLE {
    class battleTrainer;
}

namespace SAVE {
#define MAX_SAVE_FILES 3
    extern std::vector<pokemon> tmp;

    enum gameType {
        NORMAL = 0,
        TRANSFER = 1,

        SPECIAL = 2
    };

    struct saveGame {
        struct playerInfo {
            u8          m_used : 1;

            // general stuff
            gameType    m_gameType : 6;

            u8          m_isMale : 1;
            char        m_playername[ 12 ];
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
            u32         m_money;
            u32         m_coins;
            u32         m_battlePoints;

            // Map stuff
            MAP::mapObject m_player;
            u8          m_currentMap;

            u8          m_curBox;

            u16         m_lstDex;
            u8          m_activatedPNav;

            u16         m_lstUsedItems[ 5 ];
            u8          m_lstUsedItemsIdx;
            u16         m_registeredItem;

            u8          m_lstBag;
            u16         m_lstBagItem;

            s16         m_repelSteps;

            saveOptions m_options;

            pokemon     m_pkmnTeam[ 6 ];

            u16         m_flags[ 500 ];

            //Bag stuff
            BAG::bag    m_bag;

            // Methods 'n' stuff
            bool        checkflag( u8 p_idx );
            void        setflag( u8 p_idx, bool p_value );
            void        stepIncrease( );
            u8          getEncounterLevel( u8 p_tier );
            u8          getBadgeCount( );
            u8          getTeamPkmnCount( );

            BATTLE::battleTrainer* getBattleTrainer( );
            void        updateTeam( );
        } m_saveFile[ MAX_SAVE_FILES ];

        u8          m_activeFile;

        // Things shared among all save files

        // Stored Pkmn
#define MAX_BOXES 42
        BOX::box    m_storedPokemon[ MAX_BOXES ];
        pokemon::boxPokemon m_clipboard[ 6 ];

        u8          m_caughtPkmn[ 1 + MAX_PKMN / 8 ];
        u8          m_seenPkmn[ 1 + MAX_PKMN / 8 ];

        u8          m_transfersRemaining; // Times a GBA save file can be copied
        
        // Return the idx of the resulting Box
        s8          storePkmn( const pokemon::boxPokemon& p_pokemon );
        s8          storePkmn( const pokemon& p_pokemon );

        u16         countPkmn( u16 p_pkmnIdx );

        BOX::box*   getCurrentBox( );

        playerInfo&  getActiveFile( ) {
            return m_saveFile[ m_activeFile ];
        }
    };

    extern saveGame* SAV;
}
