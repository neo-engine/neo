/*
Pokémon neo
------------------------------

file        : mapDefines.h
author      : Philip Wellnitz
description : General map stuff.

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
#include <map>
#include <string>
#include <nds.h>

#define MAP_HOENN        10
#define MAP_METEOR_FALLS 11
#define MAP_DESERT_RUINS 12
#define MAP_ISLAND_CAVE  13
#define MAP_ANCIENT_TOMB 14

#define MAP_KANTO              110
#define MAP_TOHJO_FALLS        111
#define MAP_KANTO_VICTORY_ROAD 112
#define MAP_VIRIDIAN_FOREST    113
#define MAP_SEAFOAM_ISLANDS    114
#define MAP_MT_MOON            115
#define MAP_DIGLETTS_CAVE      116
#define MAP_ROCK_TUNNEL        117
#define MAP_CERULEAN_CAVE      118

#define MAP_JOHTO         210
#define MAP_DARK_CAVE     211
#define MAP_SPROUT_TOWER  212
#define MAP_RUINS_OF_ALPH 213
#define MAP_UNION_CAVE    214
#define MAP_SLOWPOKE_WELL 215
#define MAP_ILEX_FOREST   216
#define MAP_NATIONAL_PARK 217
#define MAP_BURNED_TOWER  218
#define MAP_BELL_TOWER    219
#define MAP_WHIRL_ISLANDS 220
#define MAP_MT_MORTAR     221
#define MAP_ICE_PATH      222
#define MAP_DRAGONS_DEN   223

namespace MAP {
    constexpr u8 NUM_ROWS = 16;
    constexpr u8 NUM_COLS = 32;

    struct position {
        u16 m_posX; // Global
        u16 m_posY; // Global
        u8  m_posZ;

        constexpr auto operator<=>( const position& ) const = default;
    };
    enum direction : u8 { UP, RIGHT, DOWN, LEFT };

    struct movement {
        direction m_direction;
        u8        m_frame;
    };

    typedef std::pair<u8, position> warpPos;
    enum moveMode {
        // Player modes
        WALK       = 0,
        SURF       = ( 1 << 0 ),
        BIKE       = ( 1 << 2 ),
        ACRO_BIKE  = ( 1 << 2 ) | ( 1 << 0 ),
        MACH_BIKE  = ( 1 << 2 ) | ( 1 << 1 ),
        BIKE_JUMP  = ( 1 << 2 ) | ( 1 << 0 ) | ( 1 << 1 ),
        SIT        = ( 1 << 3 ),
        DIVE       = ( 1 << 4 ),
        ROCK_CLIMB = ( 1 << 5 ),
        STRENGTH   = ( 1 << 6 ),
        // NPC modes
        NO_MOVEMENT            = 0,
        LOOK_UP                = 1,
        LOOK_DOWN              = 2,
        LOOK_RIGHT             = 4,
        LOOK_LEFT              = 8,
        WALK_LEFT_RIGHT        = 16,
        WALK_UP_DOWN           = 17,
        WALK_CIRCLE            = 18,
        WALK_AROUND_LEFT_RIGHT = 19,
        WALK_AROUND_UP_DOWN    = 20,
    };
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
    };
    enum eventTrigger : u8 {
        TRIGGER_NONE           = 0,
        TRIGGER_STEP_ON        = ( 1 << 0 ),
        TRIGGER_INTERACT       = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 ),
        TRIGGER_INTERACT_DOWN  = ( 1 << 1 ),
        TRIGGER_INTERACT_LEFT  = ( 1 << 2 ),
        TRIGGER_INTERACT_UP    = ( 1 << 3 ),
        TRIGGER_INTERACT_RIGHT = ( 1 << 4 ),
        TRIGGER_ON_MAP_ENTER   = ( 1 << 5 ),
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
        HEADBUTT,
        ROCK_SMASH,
        SWEET_SCENT,
    };
    enum mapWeather : u8 {
        NOTHING        = 0, // Inside
        SUNNY          = 1,
        REGULAR        = 2,
        RAINY          = 3,
        SNOW           = 4,
        THUNDERSTORM   = 5,
        MIST           = 6,
        BLIZZARD       = 7,
        SANDSTORM      = 8,
        FOG            = 9,
        DENSE_MIST     = 0xa,
        CLOUDY         = 0xb, // Dark Forest clouds
        HEAVY_SUNLIGHT = 0xc,
        HEAVY_RAIN     = 0xd,
        UNDERWATER     = 0xe
    };
    enum mapType : u8 { OUTSIDE = 0, CAVE = 1, INSIDE = 2, DARK = 4, FLASHABLE = 8 };
    enum warpType : u8 {
        NO_SPECIAL,
        CAVE_ENTRY,
        DOOR,
        TELEPORT,
        EMERGE_WATER,
        LAST_VISITED,
        SLIDING_DOOR
    };

    constexpr u8 MAX_EVENTS_PER_SLICE = 64;
    struct mapData {
        mapType    m_mapType;
        mapWeather m_weather;
        u8         m_battleBG;
        u8         m_battlePlat1;
        u8         m_battlePlat2;
        u8         m_surfBattleBG;
        u8         m_surfBattlePlat1;
        u8         m_surfBattlePlat2;

        u16 m_baseLocationId;
        u8  m_extraLocationCount;
        struct locationData {
            u8  m_left;
            u8  m_top;
            u8  m_right;
            u8  m_bottom;
            u16 m_locationId;
        } m_extraLocations[ 4 ];
        u8 m_pokemonDescrCount;
        struct wildPkmnData {
            u16          m_speciesId;
            u8           m_forme;
            wildPkmnType m_encounterType;
            u8           m_slot;
            u8           m_daytime;
            u8           m_encounterRate;
        } m_pokemon[ 30 ];
        u8 m_eventCount;
        struct event {
            u8           m_posX;
            u8           m_posY;
            u8           m_posZ;
            u16          m_activateFlag;
            u16          m_deactivateFlag;
            eventType    m_type;
            eventTrigger m_trigger;
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
                    u8  m_movementType;
                    u16 m_spriteId;
                    u16 m_trainerId;
                    u8  m_sight;
                } m_trainer;
                struct {
                    u16 m_speciesId;
                    u8  m_level;
                    u8  m_forme; // BIT(6) female; BIT(7) genderless
                    u8  m_shiny; // BIT(6) hidden ability, BIT(7) fateful
                } m_owPkmn;
                struct {
                    u8  m_movementType;
                    u16 m_spriteId;
                    u16 m_scriptId;
                    u8  m_scriptType;
                } m_npc;
                struct {
                    warpType m_warpType;
                    u8       m_bank;
                    u8       m_mapY;
                    u8       m_mapX;
                    u8       m_posX;
                    u8       m_posY;
                    u8       m_posZ;
                } m_warp;
                struct {
                    u16 m_scriptId;
                    u8  m_scriptType;
                } m_generic;
                struct {
                    u8 m_hmType;
                } m_hmObject;
                struct {
                    u8 m_treeIdx; // internal id of this berry tree
                } m_berryTree;
            } m_data;
        } m_events[ MAX_EVENTS_PER_SLICE ];

        /*
         * @brief: Checks whether there is a active event of the specified type at the
         * specified position.
         */
        bool hasEvent( eventType p_type, u8 p_x, u8 p_y, u8 p_z ) const;
    };

    struct bankInfo {
        u8  m_bank;
        u16 m_locationId;
        u8  m_mapMode; // currently unused, used for different map modes (i.e. 32x32 tiles; large
                       // maps with connections, etc)
    };

    extern bankInfo CURRENT_BANK;
    void            loadNewBank( u8 p_bank );

    constexpr direction getDir( int p_dir ) {
        if( p_dir & KEY_DOWN ) { return MAP::direction::DOWN; }
        if( p_dir & KEY_UP ) { return MAP::direction::UP; }
        if( p_dir & KEY_LEFT ) { return MAP::direction::LEFT; }
        if( p_dir & KEY_RIGHT ) { return MAP::direction::RIGHT; }
        return MAP::direction::RIGHT;
    }

} // namespace MAP
