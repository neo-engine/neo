/*
Pokémon neo
------------------------------

file        : mapDefines.h
author      : Philip Wellnitz
description : General map stuff.

Copyright (C) 2012 - 2022
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
#include <map>
#include <string>
#include <vector>
#include <nds.h>

#include "defines.h"

namespace MAP {
    constexpr u8 NUM_ROWS         = 16;
    constexpr u8 NUM_COLS         = 32;
    constexpr u8 UNUSED_MAPOBJECT = 200;

    struct position {
        u16 m_posX; // Global
        u16 m_posY; // Global
        u8  m_posZ;

        constexpr auto operator<=>( const position& ) const = default;
    };

    struct mapLocation {
        static constexpr position DUMMY_POSITION   = { 0, 0, 0 };
        static constexpr u8       MAP_LOCATION_RES = 8;

        u8   m_bank;
        bool m_good; // data has been read from fs
        u8   m_owMapSizeX;
        u8   m_owMapSizeY;
        u8   m_mapImageRes    = 4;
        u8   m_mapImageShiftX = 0;
        u8   m_mapImageShiftY = 0;

        u16 m_defaultLocation; // location to return for out-of-bounds; e.g. L_HOENN

        std::vector<u16> m_locationData;

        inline u16 get( u16 p_y, u16 p_x ) const {
            size_t idx = p_y * m_owMapSizeX + p_x;
            if( idx < m_locationData.size( ) ) {
                return m_locationData[ idx ];
            } else {
                return m_defaultLocation;
            }
        }

        constexpr position getOWPosForLocation( u16 p_location ) const {
            for( u16 y = 0; y < m_owMapSizeY; ++y ) {
                for( u16 x = 0; x < m_owMapSizeX; ++x ) {
                    if( get( y, x ) == p_location ) {
                        return { u16( MAP_LOCATION_RES * x ), u16( MAP_LOCATION_RES * y ), 0 };
                    }
                }
            }
            return DUMMY_POSITION;
        }
    };

    extern mapLocation MAP_LOCATIONS;

    std::string parseLogCmd( const std::string& p_cmd );
    std::string convertMapString( const std::string& p_text, style p_style );
    void        printMapMessage( const std::string& p_text, style p_style );
    void        printMapYNMessage( const std::string& p_text, style p_style );

    enum direction : u8 { UP, RIGHT, DOWN, LEFT };

    constexpr direction movement2Direction( u8 p_move ) {
        switch( p_move ) {
        case 0: return UP;
        case 1: return DOWN;
        case 2: return RIGHT;
        case 3: return LEFT;

        default: return DOWN;
        }
    }

    struct movement {
        direction m_direction;
        u8        m_frame;
    };

    typedef std::pair<u8, position> warpPos;
    struct flyPos {
        // ensure that a flyPos fits into 8 bytes.

        u16 m_targetLocation;
        u8  m_targetBank;
        u8  m_targetZ;

        u16 m_targetX;
        u16 m_targetY;

        /*
         * @brief: returns the ow map this flypos shoul appear on
         */
        constexpr auto location( ) const {
            return m_targetLocation;
        }

        /*
         * @brief: returns the target position this fly pos should warp to.
         */
        constexpr warpPos target( ) const {
            return { m_targetBank, { m_targetX, m_targetY, u8( m_targetZ ) } };
        }

        constexpr auto operator<=>( const flyPos& ) const = default;
    };

    enum moveMode {
        // Player modes
        WALK       = 0,
        BIKE       = ( 1 << 2 ),
        ACRO_BIKE  = ( 1 << 2 ) | ( 1 << 0 ),
        MACH_BIKE  = ( 1 << 2 ) | ( 1 << 1 ),
        BIKE_JUMP  = ( 1 << 2 ) | ( 1 << 0 ) | ( 1 << 1 ),
        SIT        = ( 1 << 3 ),
        DIVE       = ( 1 << 4 ),
        ROCK_CLIMB = ( 1 << 5 ),
        STRENGTH   = ( 1 << 6 ),
        SURF       = ( 1 << 7 ),
        // NPC modes
        NO_MOVEMENT             = 0,
        LOOK_UP                 = 1,
        LOOK_DOWN               = 2,
        LOOK_RIGHT              = 4,
        LOOK_LEFT               = 8,
        WALK_LEFT_RIGHT         = 16,
        WALK_UP_DOWN            = 17,
        WALK_CIRCLE             = 18, // unused
        WALK_AROUND_LEFT_RIGHT  = 19,
        WALK_AROUND_UP_DOWN     = 20,
        WALK_CONT_LEFT_RIGHT    = 21,
        WALK_CONT_UP_DOWN       = 22,
        WALK_AROUND_SQUARE      = 23, // randomly walks around in a 3x3 square
        WALK_CONT_FOLLOW_OBJECT = 24, // walks along the edge of an object, touching w/ right hand
    };

    direction getRandomLookDirection( moveMode p_movement );

    enum eventType : u8 {
        EVENT_NONE        = 0,
        EVENT_MESSAGE     = 1,
        EVENT_ITEM        = 2,
        EVENT_TRAINER     = 3,
        EVENT_OW_PKMN     = 4,
        EVENT_NPC         = 5,
        EVENT_WARP        = 6,
        EVENT_GENERIC     = 7,
        EVENT_HMOBJECT    = 8, // cut, rock smash, strength
        EVENT_BERRYTREE   = 9,
        EVENT_NPC_MESSAGE = 10,
        EVENT_FLY_POS     = 11,
    };

    enum eventTrigger : u8 {
        TRIGGER_NONE              = 0,
        TRIGGER_STEP_ON           = ( 1 << 0 ),
        TRIGGER_INTERACT          = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 ),
        TRIGGER_INTERACT_DOWN     = ( 1 << 1 ),
        TRIGGER_INTERACT_LEFT     = ( 1 << 2 ),
        TRIGGER_INTERACT_UP       = ( 1 << 3 ),
        TRIGGER_INTERACT_RIGHT    = ( 1 << 4 ),
        TRIGGER_ON_MAP_ENTER      = ( 1 << 5 ),
        TRIGGER_ON_MOVE_AT_POS    = ( 1 << 6 ), // only for generic events
        TRIGGER_ON_MOVE_IN_BATTLE = ( 1 << 7 ), // only for generic events
    };

    constexpr eventTrigger dirToEventTrigger( direction p_dir ) {
        switch( p_dir ) {
        case UP: return TRIGGER_INTERACT_DOWN;
        case LEFT: return TRIGGER_INTERACT_RIGHT;
        case DOWN: return TRIGGER_INTERACT_UP;
        case RIGHT: return TRIGGER_INTERACT_LEFT;
        }
        return TRIGGER_NONE;
    }

    enum wildPkmnType : u8 {
        GRASS,
        HIGH_GRASS,
        WATER,
        OLD_ROD,
        GOOD_ROD,
        SUPER_ROD,
        HEADBUTT, // unused
        ROCK_SMASH,
        SWEET_SCENT, // unused
        POKE_TORE,
        SWARM,
        INFINITY_CAVE, // normal grass, but m_slot is the level
    };

    enum mapWeather : u16 {
        NOTHING         = 0, // Inside
        SUNNY           = 1,
        REGULAR         = 2,
        RAINY           = 3,
        SNOW            = 4,
        THUNDERSTORM    = 5,
        MIST            = 6,
        BLIZZARD        = 7,
        SANDSTORM       = 8,
        FOG             = 9,
        DENSE_MIST      = 0xa,
        CLOUDY          = 0xb, // "nice weather" clouds
        HEAVY_SUNLIGHT  = 0xc,
        HEAVY_RAIN      = 0xd,
        UNDERWATER      = 0xe,
        DARK_FLASHABLE  = 0xf,
        DARK_PERMANENT  = 0x10,
        DARK_FLASH_USED = 0x11,
        FOREST_CLOUDS   = 0x12,
        ASH_RAIN        = 0x13, // route 113
        DARK_FLASH_1    = 0x14, // dewford gym defeated 1-2 trainers
        DARK_FLASH_2    = 0x15, // dewford gym defeated 3-4 trainers
    };

    enum mapType : u8 { OUTSIDE = 0, CAVE = 1, INSIDE = 2, DARK = 4, FLASHABLE = 8 };

    enum warpType : u8 {
        NO_SPECIAL,
        CAVE_ENTRY,
        DOOR,
        TELEPORT,
        EMERGE_WATER,
        LAST_VISITED,
        SLIDING_DOOR,
        FLY,
        SCRIPT
    };

    constexpr u8 MAX_PKMN_PER_SLICE   = 30;
    constexpr u8 MAX_EVENTS_PER_SLICE = 64;
    struct mapData {
        u8 m_mapType;
        u8 m_weather;
        u8 m_battleBG;
        u8 m_battlePlat1;

        u8 m_battlePlat2;
        u8 m_surfBattleBG;
        u8 m_surfBattlePlat1;
        u8 m_surfBattlePlat2;

        u32 : 32;

        u16 m_locationIds[ 4 ][ 4 ]; // (y, x), 8x8 blocks each

        struct wildPkmnData {
            u16 m_speciesId;
            u8  m_forme;
            u8  m_encounterType;

            u8 m_slot;  // num req badges for pkmn to show up
            u8 m_daytime;
            u8 m_encounterRate;
        } m_pokemon[ MAX_PKMN_PER_SLICE ];
        struct event {
            u8  m_posX;
            u8  m_posY;
            u8  m_posZ;
            u8  m_route = 0; // event activates only for this route
            u16 m_activateFlag;
            u16 m_deactivateFlag;
            u8  m_type;

            u8 m_trigger;
            union data {
                struct {
                    u8  m_msgType;
                    u16 m_msgId;
                } m_message;
                struct {
                    u8  m_itemType;
                    u16 m_itemId;
                } m_item;
                struct {
                    u16 m_spriteId;
                    u16 m_trainerId;

                    u8 m_movementType;
                    u8 m_sight;
                } m_trainer;
                struct {
                    u16 m_speciesId;
                    u8  m_level;
                    u8  m_forme; // BIT(6) female; BIT(7) genderless

                    u8 m_shiny; // BIT(6) hidden ability, BIT(7) fateful
                } m_owPkmn;
                struct {
                    u16 m_spriteId;
                    u16 m_scriptId;

                    u8 m_movementType;
                    u8 m_scriptType;
                } m_npc;
                struct {
                    u8 m_warpType;
                    u8 m_bank;
                    u8 m_mapX;
                    u8 m_mapY;

                    u8 m_posX;
                    u8 m_posY;
                    u8 m_posZ; // script id if warp type is script.
                } m_warp;
                struct {
                    u16 m_scriptId;
                    u8  m_scriptType;
                    u16 m_triggerMove; // only used when event triggered by a pkmn move
                } m_generic;
                struct {
                    u8 m_hmType;
                } m_hmObject;
                struct {
                    u8 m_treeIdx; // internal id of this berry tree
                } m_berryTree;
                struct {
                    u16 m_location; // location for which this flypos should appear
                } m_flyPos;
            } m_data;
        } m_events[ MAX_EVENTS_PER_SLICE ];

        /*
         * @brief: Checks whether there is a active event of the specified type at the
         * specified position.
         */
        bool hasEvent( eventType p_type, u8 p_x, u8 p_y, u8 p_z ) const;
    };

    constexpr u8 MAPMODE_DEFAULT   = 0;
    constexpr u8 MAPMODE_SCATTERED = 1;
    constexpr u8 MAPMODE_COMBINED  = 2;

    struct bankInfo {
        u8 m_sizeX   = 0;
        u8 m_sizeY   = 0;
        u8 m_mapMode = 0; // 0: normal maps/data in folder, 1: scattered in subfolders, 2: combined
                          // (ignored, assumed to be 2)
        u8 m_isOWMap : 1 = false;    // redundant information (unused); stores whether this bank
                                     // has corresponding location data and an ow map
        u8 m_hasDiveMap : 1 = false; // stores whether there is a seperate map for hm dive
        u8 : 6;
        u16 m_defaultLocation = 0; // default location for this bank (unused)
        u16 : 16;

        constexpr bankInfo( u8 p_sizeX = 0, u8 p_sizeY = 0, u8 p_mapMode = MAPMODE_COMBINED,
                            bool p_isOWMap = false )
            : m_sizeX( p_sizeX ), m_sizeY( p_sizeY ), m_mapMode( p_mapMode ),
              m_isOWMap( p_isOWMap ) {
        }
    };

    struct blockSetBankHeader {
        u8  m_blockSetCount = 0;        // number of block sets
        u8  m_dayTimeCount  = DAYTIMES; // number of extra daytime palettes per palette
        u32 m_1 : 16        = 0;
        u32 m_2 : 32        = 0;
    };

    constexpr direction getDir( int p_dir ) {
        if( p_dir & KEY_DOWN ) { return MAP::direction::DOWN; }
        if( p_dir & KEY_UP ) { return MAP::direction::UP; }
        if( p_dir & KEY_LEFT ) { return MAP::direction::LEFT; }
        if( p_dir & KEY_RIGHT ) { return MAP::direction::RIGHT; }
        return MAP::direction::RIGHT;
    }

    constexpr auto WEATHER_BLEND = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG0 | BLEND_DST_BG2
                                   | BLEND_DST_BG1 | BLEND_DST_SPRITE;
} // namespace MAP
