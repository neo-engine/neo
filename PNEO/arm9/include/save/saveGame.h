/*
    Pokémon neo
    ------------------------------

    file        : saveGame.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2023
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

#include <bit>
#include <memory>
#include <string>
#include <vector>
#include <nds.h>
#include "bag/bag.h"
#include "bag/pokeblock.h"
#include "box/box.h"
#include "map/mapObject.h"
#include "pokemon.h"
#include "save/saveOptions.h"
#include "save/wonderCard.h"

namespace SAVE {
#ifdef FLASHCARD
    constexpr u8 MAX_SAVE_FILES = 2;
#else
    constexpr u8 MAX_SAVE_FILES = 3;
#endif
    constexpr u8 MAX_SPECIAL_EPISODES = 0;

    constexpr u8 POKEBLOCK_TYPES = 24;

    constexpr u8  NUM_PARTY_SLOTS = 6;
    constexpr u16 NUM_FLAGS       = 256;
    constexpr u16 NUM_VARS        = 256;

    constexpr u32 GOOD_MAGIC1 = 0x01234567;
    constexpr u32 GOOD_MAGIC2 = 0xFEDCBA98;

    constexpr u8  MAX_STORED_WC          = 12;
    constexpr u16 MAX_WC_IDX             = 256; // wc idx 0 to 255
    constexpr u8  MAX_BOXES              = 45;
    constexpr u8  MAX_REGISTERED_FLY_POS = 75;
    constexpr u16 MAX_MAPOBJECT          = 256;
    constexpr u8  BERRY_SLOTS            = 150;

    constexpr u16 F_MEGA_EVOLUTION    = 1;
    constexpr u16 F_NAV_OBTAINED      = 2;
    constexpr u16 F_DEX_OBTAINED      = 3;
    constexpr u16 F_NAT_DEX_OBTAINED  = 4;
    constexpr u16 F_RIVAL_APPEARANCE  = 5; // set if rival has appearance 1
    constexpr u16 F_HOENN_DAYCARE_EGG = 190;
    constexpr u16 F_SCND_DAYCARE_EGG  = 191;
    constexpr u16 F_THRD_DAYCARE_EGG  = 192;

    constexpr u16 F_SLATEPORT_BATTLE_TENT_STREAK_ONGOING = 194;
    constexpr u16 F_BATTLE_FACTORY_50_STREAK_ONGOING     = 221;
    constexpr u16 F_BATTLE_FACTORY_100_STREAK_ONGOING    = 222;

    constexpr u16 F_ICAVE_LAYER_CLEARED = 327;
    constexpr u16 F_ICAVE_LAYER_ITEM1   = 328;
    constexpr u16 F_ICAVE_LAYER_ITEM2   = 329;
    constexpr u16 F_ICAVE_LAYER_ITEM3   = 330;
    constexpr u16 F_ICAVE_LAYER_ITEM4   = 331;
    constexpr u16 F_ICAVE_LAYER_ITEM5   = 332;
    constexpr u16 F_ICAVE_LAYER_ITEM6   = 333;
    constexpr u16 F_ICAVE_LAYER_ITEM7   = 334;
    constexpr u16 F_ICAVE_LAYER_ITEM8   = 335;
    constexpr u16 F_ICAVE_LAYER_ITEM9   = 336;
    constexpr u16 F_ICAVE_LAYER_ITEM10  = 337;
    constexpr u16 F_ICAVE_POKE_CLEARED  = 338;

    constexpr u16 F_UNCOLLECTED_MYSTERY_EVENT = 351;

    constexpr u16 F_GAME_CLEAR = 385;

    constexpr u16 F_TRAINER_BATTLED( u8 p_trainer ) {
        return p_trainer + 2048;
    }

    constexpr u8 V_INITIAL_PKMN_CHOICE          = 0; // 1: TREECKO, 2: TORCHIC, 3: MUDKIP, 0: NONE
    constexpr u8 V_NUM_FAINTED                  = 1; // how often the player fainted
    constexpr u8 V_SLATEPORT_BATTLE_TENT_STREAK = 2;
    constexpr u8 V_UNUSED_BATTLE_TENT_STREAK_1  = 3;
    constexpr u8 V_UNUSED_BATTLE_TENT_STREAK_2  = 4;

    constexpr u8 V_CRYSTAL_CAVERN_1 = 10;
    constexpr u8 V_CRYSTAL_CAVERN_2 = 11;
    constexpr u8 V_CRYSTAL_CAVERN_3 = 12;
    constexpr u8 V_CRYSTAL_CAVERN_4 = 13;

    constexpr u8 V_BATTLE_FACTORY_50_STREAK                   = 20;
    constexpr u8 V_BATTLE_FACTORY_100_STREAK                  = 21;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_1_50_STREAK         = 22;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_1_100_STREAK        = 23;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_2_50_STREAK         = 24;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_2_100_STREAK        = 25;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_3_50_STREAK         = 26;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_3_100_STREAK        = 27;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_4_50_STREAK         = 28;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_4_100_STREAK        = 29;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_5_50_STREAK         = 30;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_5_100_STREAK        = 31;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_6_50_STREAK         = 32;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_6_100_STREAK        = 33;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_6_50_DOUBLE_STREAK  = 34;
    constexpr u8 V_UNUSED_BATTLE_FACILITY_6_100_DOUBLE_STREAK = 35;
    constexpr u8 V_SHOAL_CAVE                                 = 36;
    constexpr u8 V_HALL_OF_FAME_VICTORIES                     = 37;
    constexpr u8 V_INFINITY_CAVE_MAX_LAYER                    = 38;
    constexpr u8 V_INFINITY_CAVE_CURRENT_LAYER                = 39;

    enum gameType : u8 {
        UNUSED   = 0,
        NORMAL   = 1,
        TRANSFER = 2,

        SPECIAL = 3
    };

    struct time {
        u16 m_hours;
        u8  m_mins;
        u8  m_secs;

        constexpr bool canIncrease( ) const {
            if( m_hours == 999 && m_mins == 59 && m_secs == 59 ) { return false; }
            return true;
        }

        auto operator<=>( const time& ) const = default;
    };

    extern time CURRENT_TIME;

    struct date {
        u8 m_year;
        u8 m_month;
        u8 m_day;

        auto operator<=>( const date& ) const = default;
    };

    extern date CURRENT_DATE;

    struct saveGame {
        struct playerInfo {
            static constexpr u8 ACHIEVEMENT_HALL_OF_FAME    = 1 << 0; // entering the hall of fame
            static constexpr u8 ACHIEVEMENT_DEX             = 1 << 1; // completing the hoenn dex
            static constexpr u8 ACHIEVEMENT_BATTLE_FRONTIER = 1 << 2; // obtain all gold symbols
            static constexpr u8 ACHIEVEMENT_CONTEST         = 1 << 3; // obtain all paintings
            static constexpr u8 ACHIEVEMENT_BERRY           = 1 << 4; // collect all vars of berries
            static constexpr u8 ACHIEVEMENT_GAME_CLEAR      = 1 << 5; // tbd

            u32 m_good1 = 0;

            gameType m_gameType;
            u8       m_achievements; // Hall of Fame / Dex / Battle Frontier /
                                     // Contest / Berries / Game clear
            u8   m_appearance;       // sprite the player currently uses
            char m_playername[ OTLENGTH ];
            u16  m_id;
            u16  m_sid;
            time m_playTime;
            date m_startDate;
            date m_lastSaveDate;
            time m_lastSaveTime;
            u16  m_lastSaveLocation;
            date m_lastAchievementDate; // Time player won last badge / entered hof
            u8   m_HOENN_Badges;
            u16  m_FRONTIER_Badges;
            u8   m_KANTO_Badges;
            u8   m_JOHTO_Badges;
            u8   m_RESERVED_Badges;
            u8   m_lastAchievementEvent; // id of a string to be printed on the second line of the
                                         // trainer's card.
            u32            m_money;
            u16            m_coins;
            u16            m_battlePoints;
            u32            m_ashCount = 0; // ash collected on Route 113
            MAP::mapPlayer m_player;
            u8             m_currentMap;
            u8             m_stepCount;
            MAP::warpPos   m_lastWarp;
            u8             m_curBox;
            u16            m_lstDex;
            u16            m_lstLocalDexPage;
            u16            m_lstLocalDexSlot;
            u16            m_lstViewedItem[ 5 ]; // Last viewed item in the bag

            ObjPriority m_playerPriority;

            u16         m_registeredItem; // Item registered for fast-use
            u8          m_lstBag;         // Last sub-bag used by the player
            u16         m_lstUsedItem;    // Most recently used bag item
            s16         m_repelSteps;     // Steps remaining of the currently active repel
            saveOptions m_options;        // Various options and settings
            pokemon     m_pkmnTeam[ NUM_PARTY_SLOTS ];
            u16         m_vars[ NUM_VARS ]; // variables to be set by map scripts etc.
            u16 m_flags[ NUM_FLAGS ];       // flags tracking the progress of the player's adventure

            u8   m_berryTrees[ BERRY_SLOTS ];       // berry indices for each berry tree
            u8   m_berryHealth[ BERRY_SLOTS ];      // health of the berry (255 for default)
            date m_berryPlantedDate[ BERRY_SLOTS ]; // unused
            time m_berryPlantedTime[ BERRY_SLOTS ]; // play time when the berry was planted

            MAP::warpPos m_lastPokeCenter; // position where the player is teleported to if all of
                                           // their pkmn are fainted
            MAP::warpPos m_lastCaveEntry; // position where a player is teleported to if they use an
                                          // escape rope in a cave
            MAP::warpPos    m_lastOWPos;  // last position of the player on an outside-type map
            MAP::mapWeather m_currentMapWeather; // current map weather

            std::pair<u8, MAP::mapObject> m_mapObjects[ MAX_MAPOBJECT ];
            u8                            m_mapObjectCount;

            u16 m_initGameItemCount = 0; // Items accessible during begin game PC check
            u16 m_initGameItems[ 5 ];

            u8 m_objectAttached    = 0; // true if there is a mapobject following the player
            u8 m_mapObjAttachedIdx = 0; // idx of the map object following the player

            u8 m_dayCareDepositLevel[ 6 ] = { 0 }; // level of pkmn in day care when deposited

            u8 m_forcedMovement = 0;

            u8          m_numRegisteredFlyPos                        = 0;
            MAP::flyPos m_registeredFlyPos[ MAX_REGISTERED_FLY_POS ] = { };

            u32 m_reserved[ 88 ] = { 0 }; // reserved for future things that need to be stored

            u16 m_pokeblockCount[ POKEBLOCK_TYPES ] = { 0 };

            BAG::bag m_bag;

            boxPokemon m_dayCarePkmn[ 6 ] = { boxPokemon( ) };
            boxPokemon m_dayCareEgg[ 3 ]  = { boxPokemon( ) };

            BOX::box m_storedPokemon[ MAX_BOXES ]; // pkmn in the storage system

            boxPokemon m_traderPokemon; // pkmn currently owned by pkmn self-trader
            boxPokemon m_unusedPkmn[ 5 ];

            u8 m_caughtPkmn[ 250 ]; // The pkmn the player has caught
            u8 m_seenPkmn[ 250 ];   // The pkmn the player has seen
            u8 m_reserved2[ 300 ];

            u8 m_route = 0; // current story route

            wonderCard m_storedWonderCards[ MAX_STORED_WC ];
            u8         m_collectedWonderCards[ MAX_WC_IDX / 8 ];

            u32 m_good2 = 0;

            /*
             * @brief: tries to register a new fly pos; returns true on success.
             */
            bool registerFlyPos( MAP::flyPos p_flyPos ) {
                for( u8 i = 0; i < m_numRegisteredFlyPos; ++i ) {
                    if( m_registeredFlyPos[ i ].location( ) == p_flyPos.location( ) ) {
                        m_registeredFlyPos[ i ] = p_flyPos;
                        return true;
                    }
                }
                if( m_numRegisteredFlyPos >= MAX_REGISTERED_FLY_POS ) { return false; }
                m_registeredFlyPos[ m_numRegisteredFlyPos++ ] = p_flyPos;
                return true;
            }

            inline bool hasFlyPos( ) const {
                return !!m_numRegisteredFlyPos;
            }

            inline auto getFlyPosList( ) const {
                std::vector<MAP::flyPos> res{ };
                for( u8 i = 0; i < m_numRegisteredFlyPos; ++i ) {
                    if( m_registeredFlyPos[ i ].location( ) ) {
                        res.push_back( m_registeredFlyPos[ i ] );
                    }
                }
                return res;
            }

            std::vector<u16> getFlyPosLocationListForCurrentOW( ) const;

            inline auto getFlyPos( u8 p_idx ) const {
                if( p_idx >= m_numRegisteredFlyPos ) { return MAP::flyPos{ }; }
                return m_registeredFlyPos[ p_idx ];
            }

            inline auto getFlyPosForLocation( u16 p_location ) const {
                for( u8 i = 0; i < m_numRegisteredFlyPos; ++i ) {
                    if( m_registeredFlyPos[ i ].location( ) == p_location ) {
                        return m_registeredFlyPos[ i ];
                    }
                }
                return MAP::flyPos{ };
            }

            /*
             * @brief: returns whether the player can receive badges/symbols of the
             * specified type.
             */
            constexpr bool hasBadgeCase( u8 p_case ) const {
                if( p_case == 0 ) { return true; }

                if( p_case == 1 ) { return !!m_FRONTIER_Badges; }

                return false;
            }

            /*
             * @brief: returns the number of stars on the trainer's card
             */
            constexpr u8 getAchievementCount( ) const {
                u8 res = 0;
                for( u8 i = 0; i < 6; ++i ) {
                    if( ( 1 << i ) & m_achievements ) { ++res; }
                }
                return res;
            }

            /*
             * @brief: Returns the number of seen pkmn.
             */
            inline u16 getSeenCount( ) const {
                if( !checkFlag( F_NAT_DEX_OBTAINED ) ) { return getLocalSeenCount( ); }
                u16 res = 0;
                for( u8 i = 0; i < 185; ++i ) {
                    // Count the number of set bits
                    res += std::popcount( m_seenPkmn[ i ] );
                }
                return std::max( res, getCaughtCount( ) );
            }

            /*
             * @brief: Returns the number of seen pkmn in the local pkdex.
             */
            u16 getLocalSeenCount( ) const;

            /*
             * @brief: Returns the number of caught pkmn.
             */
            inline u16 getCaughtCount( ) const {
                if( !checkFlag( F_NAT_DEX_OBTAINED ) ) { return getLocalCaughtCount( ); }
                u16 res = 0;
                for( u8 i = 0; i < 185; ++i ) {
                    // Count the number of set bits
                    res += std::popcount( m_caughtPkmn[ i ] );
                }
                return res;
            }

            /*
             * @brief: Returns the number of caught pkmn in the local pkdex.
             */
            u16 getLocalCaughtCount( ) const;

            /*
             * @brief: Returns a local dex no if the player has no national dex; otherwise
             * returns national dex no.
             */
            u16 getPkmnDisplayDexId( u16 p_pokemon ) const;

            bool dexCompleted( ) const;

            bool dexSeenCompleted( ) const;

            bool localDexCompleted( ) const;

            bool localDexSeenCompleted( ) const;

            constexpr bool collectedWC( u16 p_cardId ) const {
                return !!( m_collectedWonderCards[ p_cardId / 8 ] & ( 1 << ( p_cardId % 8 ) ) );
            }

            constexpr void registerCollectedWC( u16 p_cardId ) {
                m_collectedWonderCards[ p_cardId / 8 ] |= 1 << ( p_cardId % 8 );
            }

            constexpr bool seen( u16 p_pokemon ) const {
                return !!( m_seenPkmn[ p_pokemon / 8 ] & ( 1 << ( p_pokemon % 8 ) ) );
            }

            constexpr bool caught( u16 p_pokemon ) const {
                return !!( m_caughtPkmn[ p_pokemon / 8 ] & ( 1 << ( p_pokemon % 8 ) ) );
            }

            constexpr void registerSeenPkmn( u16 p_pokemon ) {
                m_seenPkmn[ p_pokemon / 8 ] |= 1 << ( p_pokemon % 8 );
            }

            inline void registerCaughtPkmn( u16 p_pokemon ) {
                registerSeenPkmn( p_pokemon );
                m_caughtPkmn[ p_pokemon / 8 ] |= 1 << ( p_pokemon % 8 );

                if( localDexCompleted( ) ) { m_achievements |= ACHIEVEMENT_DEX; }
            }

            inline void registerAchievement( u8 p_achievement ) {
                m_achievements |= p_achievement;
            }

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
            bool checkFlag( u16 p_idx ) const;

            /*
             * @brief: Sets the specified flag to the specified value.
             */
            void setFlag( u16 p_idx, bool p_value );

            /*
             * @brief: Returns the value of the specified variable
             */
            u16 getVar( u8 p_idx );

            /*
             * @brief: Sets the specified var to the specified value.
             */
            void setVar( u8 p_idx, u16 p_value );

            /*
             * @brief: Shortcut for accessing the variable storing the max layer achieved
             * in the infinity cave.
             */
            inline u16& infinityCaveMaxLayer( ) {
                return m_vars[ V_INFINITY_CAVE_MAX_LAYER ];
            }

            /*
             * @brief: Shortcut for accessing the variable storing the max layer achieved
             * in the infinity cave.
             */
            inline const u16& infinityCaveMaxLayer( ) const {
                return m_vars[ V_INFINITY_CAVE_MAX_LAYER ];
            }

            /*
             * @brief: Shortcut for accessing the variable storing the max layer achieved
             * in the infinity cave.
             */
            inline u16& infinityCaveCurrentLayer( ) {
                return m_vars[ V_INFINITY_CAVE_CURRENT_LAYER ];
            }

            /*
             * @brief: Shortcut for accessing the variable storing the max layer achieved
             * in the infinity cave.
             */
            inline const u16& infinityCaveCurrentLayer( ) const {
                return m_vars[ V_INFINITY_CAVE_CURRENT_LAYER ];
            }

            /*
             * @brief: Increases the variable; does nothing if an overflow would occur.
             */
            void increaseVar( u8 p_idx );

            /*
             * @brief: Increases the number of steps the player has walked by 1.
             * Needs to be called whenever the player makes a step in the ow.
             */
            void stepIncrease( );

            /*
             * @brief: Returns a suitable level for wild pkmn encounters for the current
             * number of badges that the player currently has.
             */
            u8 getEncounterLevel( u8 p_tier );

            /*
             * @brief: Returns the number of badges the player has obtained so far.
             */
            u8 getBadgeCount( u8 p_region = -1 );

            /*
             * @brief: Returns the number of pkmn currently in the player's party.
             */
            u8 getTeamPkmnCount( );

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
                for( u8 i = 0; i < NUM_PARTY_SLOTS; ++i ) { res += m_pkmnTeam[ i ].canBattle( ); }
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

            /*
             * @brief: Draws the trainer's id card to the specified screen.
             * @param p_dummy: Show an empty trainer's card
             */
            void drawTrainersCard( bool p_bottom = false, bool p_dummy = false );

            /*
             * @brief: Increases the time by 1 second.
             */
            constexpr void increaseTime( ) {
                if( m_playTime.canIncrease( ) ) {
                    if( ++m_playTime.m_secs == 60 ) {
                        m_playTime.m_secs = 0;
                        if( ++m_playTime.m_mins == 60 ) {
                            m_playTime.m_mins = 0;
                            ++m_playTime.m_hours;
                        }
                    }
                }
            }

            u32 berryTimeSincePlanted( u8 p_berrySlot ) const;

            /*
             * @brief: Checks whether the specified berry plant is still alive.
             */
            bool berryIsAlive( u8 p_berrySlot ) const;

            void waterBerry( u8 p_berrySlot );

            /*
             * @brief: Returns the number of berries the specified plant yields when
             * harvested.
             */
            u8 getBerryYield( u8 p_berrySlot ) const;

            /*
             * @brief: Returns the growth stage of the berry at the specified slot
             * @returns: 0 if no berry is present, ow stage from [1, 4].
             * (1 - just planted, 2 - sprouting, 3 - blooming, 4 - yielding)
             */
            u8 getBerryStage( u8 p_berrySlot ) const;

            inline u8 getBerry( u8 p_berrySlot ) const {
                return m_berryTrees[ p_berrySlot ];
            }

            /*
             * @brief: Removes any berry in the specified berry slot.
             */
            inline void harvestBerry( u8 p_berrySlot ) {
                m_berryHealth[ p_berrySlot ] = 0;
                m_berryTrees[ p_berrySlot ]  = 0;
            }

            /*
             * @brief: Plants the specified berry (given by itemid) in the specified slot.
             */
            void plantBerry( u8 p_berrySlot, u16 p_berry );
        } m_saveFile[ MAX_SAVE_FILES ];

        u8  m_activeFile;      // The save file the player is currently using.
        u32 m_version;         // The game version the save was created with.
        u16 m_specialEpisodes; // Unlocked special episodes

        /*
         * @brief: Returns which special episodes the player has unlocked so far.
         */
        inline std::vector<u8> getSpecialEpisodes( ) const {
            auto res = std::vector<u8>( );
            for( u8 i = 0; i < MAX_SPECIAL_EPISODES; ++i ) {
                if( m_specialEpisodes & ( 1 << i ) ) { res.push_back( i + 1 ); }
            }
#ifdef DESQUID
            // special desquid episode used for testing purposes.
            // res.push_back( 0 );
#endif
            return res;
        }

        /*
         * @brief: Returns whether the save is valid.
         */
        bool isGood( );

        /*
         * @brief: Clears the save file.
         */
        void clear( );

        inline playerInfo& getActiveFile( ) {
            return m_saveFile[ m_activeFile ];
        }
    };

    bool initNewGame( );

    extern saveGame SAV;
} // namespace SAVE
