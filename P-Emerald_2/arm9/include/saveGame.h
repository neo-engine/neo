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
#define MAX_BOXES 40

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

            u16            m_registeredItem; // Item registered for fast-use
            u8             m_lstBag;       // Last sub-bag used by the player
            u16            m_lstBagItem;   // Most recently viewed bag item
            s16            m_repelSteps;   // Steps remaining of the currently active repel
            saveOptions    m_options;       // Various options and settings
            pokemon        m_pkmnTeam[ 6 ];
            u16            m_vars[ 256 ];  // variables to be set by map scripts etc.
            u16            m_flags[ 244 ]; // flags tracking the progress of the player's adventure
            BAG::bag       m_bag;

            BOX::box       m_storedPokemon[ MAX_BOXES ]; // pkmn in the storage system

            u8 m_caughtPkmn[ 1 + MAX_PKMN / 8 ]; // The pkmn the player has caught
            u8 m_seenPkmn[ 1 + MAX_PKMN / 8 ]; // The pkmn the player has seen

            /*
             * @brief: Stores the given pkmn in the storage system at the next available
             * spot in the box the player used most recently (or any subsequent box until
             * a free space is found). Returns the box the given pkmn ended up in or -1 if
             * all boxes are full.
             */
            s8 storePkmn( const boxPokemon& p_pokemon );
            s8 storePkmn( const pokemon& p_pokemon );

            u16 countPkmn( u16 p_pkmnIdx );

            /*
             * @brief: Return the number of seen pkmn.
             */
            u16 getDexCount( );

            /*
             * @brief: Returns the box of the storage system the player used most recently.
             */
            BOX::box* getCurrentBox( );

            /*
             * @brief: Clears the save file.
             */
            void clear( );

            /*
             * @brief: Clears and initializes the save file.
             */
            void initialize( );

            /*
             * @brief: Checks whether the given flag is set.
             */
            bool checkFlag( u8 p_idx );

            /*
             * @brief: Sets the specified flag to the specified value.
             */
            void setFlag( u8 p_idx, bool p_value );

            /*
             * @brief: Increases the number of steps the player has walked by 1.
             * Needs to be called whenever the player makes a step in the ow.
             */
            void stepIncrease( );

            /*
             * @brief: Returns a suitable level for wild pkmn encounters for the current
             * number of badges that the player currently has.
             */
            u8   getEncounterLevel( u8 p_tier );

            /*
             * @brief: Returns the number of badges the player has obtained so far.
             */
            u8   getBadgeCount( );

            /*
             * @brief: Returns the number of pkmn currently in the player's party.
             */
            u8   getTeamPkmnCount( );

            /*
             * @brief: Returns the team pkmn at the specified position or nullptr if there
             * is no pkmn at that position.
             */
            inline pokemon* getTeamPkmn( u8 p_position ) {
                if( m_pkmnTeam[ p_position ].getSpecies( ) )
                    return &m_pkmnTeam[ p_position ];
                else
                    return nullptr;
            }

            /*
             * @brief: Sets the team pkmn at the specified position to the specified
             * value, overwriting any existing pkmn.
             * @param p_pokemon: The new pkmn. If nullptr, deletes the pokemon at the
             * specified position (as long as there is at least 1 other
             * non-fainted/non-egg pkmn in the party.
             * @returns: true iff the operation was successful (i.e. only returns false
             * when trying to delete the last non-fainted, non-egg pkmn)
             */
            bool setTeamPkmn( u8 p_position, pokemon* p_pokemon );
            bool setTeamPkmn( u8 p_position, boxPokemon* p_pokemon );

            /*
             * @brief: Returns the number of party pkmn that can participate in a battle.
             */
            constexpr u8 countAlivePkmn( ) const {
                u8 res = 0;
                for( u8 i = 0; i < 6; ++i ) {
                    res += m_pkmnTeam[ i ].canBattle( );
                }
                return res;
            }

            /*
             * @brief: Removes any empty spots between pkmn in the party.
             * @returns: The first slot where something changed or u8( -1 ) if nothing happened.
             */
            u8 consolidatePkmn( );

            /*
             * @brief: Returns whether the save is valid.
             */
            inline bool isGood( ) const {
                return m_good1 == GOOD_MAGIC1 && m_good2 == GOOD_MAGIC2;
            }
        } m_saveFile[ MAX_SAVE_FILES ];

        u8 m_activeFile; // The save file the player is currently using.
        u32 m_version; // The game version the save was created with.

        /*
         * @brief: Returns whether the save is valid.
         */
        bool isGood( );

        /*
         * @brief: Clears the save file.
         */
        void clear( );
    };

    extern saveGame SAV;
}
