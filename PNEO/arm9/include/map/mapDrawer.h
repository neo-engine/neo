/*
Pokémon neo
------------------------------

file        : mapDrawer.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "battle/battle.h"
#include "battle/move.h"
#include "map/mapBattleFacilityDefines.h"
#include "map/mapDefines.h"
#include "map/mapObject.h"
#include "map/mapSlice.h"
#include "map/mapSprite.h"

namespace MAP {
    class mapDrawer {
      private:
        enum tileBehavior : u8 {
            BEH_NONE = 0x00,

            BEH_GRASS      = 0x02,
            BEH_LONG_GRASS = 0x03,

            BEH_SAND_WITH_ENCOUNTER_AND_FISH = 0x06,

            BEH_CAVE_WITH_ENCOUNTER = 0x08,

            BEH_LADDER = 0x0A,

            BEH_WATERFALL = 0x13,

            BEH_SLIDE_ON_ICE = 0x20,

            BEH_GRASS_UNDERWATER = 0x22,

            BEH_GRASS_ASH = 0x24,

            BEH_WARP_TELEPORT = 0x29,

            BEH_BLOCK_RIGHT         = 0x30,
            BEH_BLOCK_LEFT          = 0x31,
            BEH_BLOCK_UP            = 0x32,
            BEH_BLOCK_DOWN          = 0x33,
            BEH_BLOCK_UP_DOWN_RIGHT = 0x34,
            BEH_BLOCK_UP_DOWN_LEFT  = 0x35,
            BEH_BLOCK_DOWN_RIGHT    = 0x36,
            BEH_BLOCK_DOWN_LEFT     = 0x37,
            BEH_JUMP_RIGHT          = 0x38,
            BEH_JUMP_LEFT           = 0x39,
            BEH_JUMP_UP             = 0x3A,
            BEH_JUMP_DOWN           = 0x3B,

            BEH_WALK_RIGHT     = 0x40,
            BEH_WALK_LEFT      = 0x41,
            BEH_WALK_UP        = 0x42,
            BEH_WALK_DOWN      = 0x43,
            BEH_SLIDE_RIGHT    = 0x44,
            BEH_SLIDE_LEFT     = 0x45,
            BEH_SLIDE_UP       = 0x46,
            BEH_SLIDE_DOWN     = 0x47,
            BEH_SLIDE_CONTINUE = 0x48,

            BEH_RUN_RIGHT = 0x50,
            BEH_RUN_LEFT  = 0x51,
            BEH_RUN_UP    = 0x52,
            BEH_RUN_DOWN  = 0x53,

            BEH_WARP_CAVE_ENTRY    = 0x60,
            BEH_WARP_NO_SPECIAL    = 0x61,
            BEH_WARP_ON_WALK_RIGHT = 0x62,
            BEH_WARP_ON_WALK_LEFT  = 0x63,
            BEH_WARP_ON_WALK_UP    = 0x64,
            BEH_WARP_ON_WALK_DOWN  = 0x65,
            BEH_FALL_THROUGH       = 0x66, // fall through ground

            BEH_WARP_NO_SPECIAL_2 = 0x68,
            BEH_DOOR              = 0x69,

            BEH_WARP_EMERGE_WATER   = 0x6C,
            BEH_WARP_ON_WALK_DOWN_2 = 0x6D,
            BEH_WARP_THEN_WALK_UP   = 0x6E,

            BEH_PACIFIDLOG_LOG_VERTICAL_TOP     = 0x74,
            BEH_PACIFIDLOG_LOG_VERTICAL_BOTTOM  = 0x75,
            BEH_PACIFIDLOG_LOG_HORIZONTAL_LEFT  = 0x76,
            BEH_PACIFIDLOG_LOG_HORIZONTAL_RIGHT = 0x77,
            BEH_FORRTREE_BRIDGE_BIKE_BELOW      = 0x78,

            BEH_WALK_ONLY = 0xA0,

            BEH_BLOCK_UP_DOWN    = 0xC0,
            BEH_BLOCK_LEFT_RIGHT = 0xC1,

            BEH_MUD_SLIDE
            = 0xD0, // slide down player unless FASTBIKE_SPEED_NO_TILE_BREAK fast on bike

            BEH_BREAKABLE_TILE                 = 0xD2,
            BEH_BIKE_BRIDGE_VERTICAL           = 0xD3,
            BEH_BIKE_BRIDGE_HORIZONTAL         = 0xD4,
            BEH_BIKE_BRIDGE_VERTICAL_NO_JUMP   = 0xD5,
            BEH_BIKE_BRIDGE_HORIZONTAL_NO_JUMP = 0xD6,
            BEH_OBSTACLE                       = 0xD7,
        };
        enum moveDataType : u8 {
            MVD_ANY    = 0x00,
            MVD_NONE   = 0x01,
            MVD_SURF   = 0x04,
            MVD_SIT    = 0x0A,
            MVD_WALK   = 0x0C,
            MVD_BRIDGE = 0x3C,
            MVD_BORDER = 0x3F,
            // elevated if = 0 mod 4
            // not passable if = 1 mod 4
        };

        static constexpr u8 FASTBIKE_SPEED_NO_TILE_BREAK
            = 9; // min speed with bike to pass over breakable tiles

        static constexpr u8 TBEH_ELEVATE_TOP_LAYER = 0x10;

        static constexpr u16 TS6_ASH_GRASS_BLOCK  = 0x206;
        static constexpr u16 TS7_ASH_GRASS_BLOCK  = 0x212;
        static constexpr u16 BREAKABLE_TILE_BLOCK = 0x206;

        static constexpr u8 WARP_TO_LAST_ENTRY     = 0xFF;
        static constexpr u8 PIKACHU_IS_MIMIKYU_MOD = 0xFF;

        FILE* _currentBank = nullptr;
        FILE* _tileset     = nullptr;
        void  loadNewBank( u8 p_bank );

        mapSpriteManager _mapSprites;

        ObjPriority _lastPlayerPriority = OBJPRIORITY_1;
        direction   _lastPlayerMove     = DOWN;

        mapSlice _slices[ 2 ][ 2 ]; //[x][y]
        u8       _curX, _curY;      // Current main slice from the _slices array
#define CUR_SLICE _slices[ _curX ][ _curY ]

        s8   _weatherScrollX = 0;
        s8   _weatherScrollY = 0;
        bool _weatherFollow  = false;

        bool _scriptRunning; // true while a map script is running.

        mapData _data[ 2 ][ 2 ];

        std::map<position, u8> _tileAnimations;

        std::set<u16> _fixedMapObjects;

#ifdef DESQUID
        static constexpr u8 TRACER_CHARGED = 1;
#else
        static constexpr u8 TRACER_CHARGED = 50;
#endif
        static constexpr u8 TRACER_AREA = 4;

        u8  _tracerCharge = 0; // steps since last usage, charged if >= TRACER_CHARGED
        u16 _tracerPositions[ 2 * TRACER_AREA + 1 ]
            = { 0 };                    // 9x9 bit field for positions activated by tracer
        u8       _tracerLuckyShiny = 0; // bitfield, tracer enc at ring i is lucky/shiny enc
        position _tracerLastPos    = { 0, 0, 0 }; // last pos where tracer successfully used
        u8       _tracerChain      = 0;           // length of current tracer chain; 0 = no chain

        u16 _tracerSpecies = 0; // current tracer pkmn species
        u8  _tracerForme   = 0; // current tracer pkmn forme

        constexpr u16 dist( u16 p_globX1, u16 p_globY1, u16 p_globX2, u16 p_globY2 ) {
            return std::max( std::abs( p_globX1 - p_globX2 ), std::abs( p_globY1 - p_globY2 ) );
        }

        constexpr bool tracerSlotShiny( u8 p_tracerSlot ) const {
            p_tracerSlot--;
            return ( _tracerLuckyShiny & ( 1 << p_tracerSlot ) ) == ( 1 << p_tracerSlot );
        }
        constexpr bool tracerSlotLucky( u8 p_tracerSlot ) const {
            p_tracerSlot--;
            return ( _tracerLuckyShiny & ( 1 << ( TRACER_AREA + p_tracerSlot ) ) )
                   == ( 1 << ( TRACER_AREA + p_tracerSlot ) );
        }

        inline void setTracerSlotShiny( u8 p_tracerSlot ) {
            p_tracerSlot--;
            _tracerLuckyShiny |= ( 1 << p_tracerSlot );
        }
        inline void setTracerSlotLucky( u8 p_tracerSlot ) {
            p_tracerSlot--;
            _tracerLuckyShiny |= ( 1 << ( TRACER_AREA + p_tracerSlot ) );
        }

        u8        _playerSprite           = 255; // id of the player sprite
        u8        _playerPlatSprite       = 255; // id of the player platform sprite
        u8        _playerFollowPkmnSprite = 255; // id of the pkmn ow sprite that follows the player
        bool      _pkmnFollowsPlayer      = false;
        mapObject _followPkmn;
        bool      _forceNoFollow         = false;
        pokemon*  _followPkmnData        = nullptr;
        pkmnData* _followPkmnSpeciesData = nullptr;

        bool _strengthUsed; // Player has used HM Strength and can move boulders

        u16 _lastrow, // Row to be filled when extending the map to the top
            _lastcol; // Column to be filled when extending the map to the left

        u16 _cx, _cy; // Cameras's pos

        bool _playerIsFast = false;
        s8   _fastBike     = false;

        std::vector<std::function<void( u16 )>> _newLocationCallbacks
            = std::vector<std::function<void( u16 )>>( ); // Called whenever player makes a step
        std::vector<std::function<void( u8 )>> _newBankCallbacks
            = std::vector<std::function<void( u8 )>>( ); // Called when a map bank was loaded
        std::vector<std::function<void( moveMode )>> _newMoveModeCallbacks
            = std::vector<std::function<void( moveMode )>>( ); // Called when the player's moveMode
                                                               // changed
        std::vector<std::function<void( mapWeather )>> _newWeatherCallbacks
            = std::vector<std::function<void( mapWeather )>>( ); // Called when the map weather
                                                                 // changed, flash
                                                                 // excluded

        /*
         * @brief: Plays the wild pkmn battle intro and changes the bgm.
         */
        void prepareBattleWildPkmn( wildPkmnType p_type, u16 p_pkmnId, bool p_luckyEnc );

        /*
         * @brief: Runs a wild pkmn battle against WILD_PKMN
         */
        BATTLE::battle::battleEndReason battleWildPkmn( wildPkmnType p_type );

        u8 getWildPkmnLevel( u16 p_rnd );

        bool getWildPkmnSpecies( wildPkmnType p_type, u16& p_pkmnId, u8& p_pkmnForme );

        /*
         * @brief: Resets any existing chain.
         */
        void resetTracerChain( bool p_updateMusic = false );

        /*
         * @brief: Initializes a new tracer chain: Computes pkmn/forme to use for the
         * chain
         */
        bool startTracerChain( );

        static constexpr u8 NO_TRACER_PKMN = 255;
        /*
         * @brief: Returns the index of the tracer pkmn at the specified position.
         * @returns: NO_TRACER_PKMN if no pkmn at the specified position, otherwise index
         * 0 to 3
         */
        u8 getTracerPkmn( u16 p_globX, u16 p_globY );

        /*
         * @brief: Updates chain parameters when player as player takes a step in the
         * specified direction
         * @returns: true iff the chain continues.
         */
        bool updateTracerChain( direction p_dir );

        /*
         * @brief: Tries to continue the current tracer chain.
         * @returns: true on success.
         */
        bool continueTracerChain( );

        /*
         * @brief: Shows rustling grass according to the current tracer state
         */
        void animateTracer( );

        /*
         * @brief: Updates the pkmn species etc following the player. Returns false on
         * failure.
         */
        bool updateFollowPkmn( );

        /*
         * @brief: Player interacts with the pkmn following them.
         */
        void interactFollowPkmn( );

        /*
         * @brief: Spawns the first pkmn in the player's party and makes it follow the
         * player. May fail if there is no corresponding ow sprite or the pkmn is huge and
         * the player is inside.
         */
        void spawnFollowPkmn( u16 p_globX, u16 p_globY, u8 p_z, direction p_direction );

        void removeFollowPkmn( );

        void loadBlock( block p_curblock, u32 p_memPos );
        void loadBlock( block p_curblock, u8 p_scrnX, u8 p_scrnY );

        void loadAnimatedTiles( u8 p_frame );

        /*
         * @brief: Sets the specified block to the specified value.
         */
        void setBlock( u16 p_globX, u16 p_globY, u16 p_newBlock );

        /*
         * @brief: Loads permanent tile animations (e.g. first frame of grass, etc)
         */
        void animateField( u16 p_globX, u16 p_globY, u8 p_animation );

        /*
         * @brief: Loads the specified frame of the animation.
         * @returns: The sprite id returned by the map sprite manager.
         * @param p_animation: If p_frame is 0, the id of the animation, otherwise the
         * sprite id of the animation.
         */
        u8 animateField( u16 p_globX, u16 p_globY, u8 p_animation, u8 p_frame );

        /*
         * @brief: Returns the identifier of a tile animation of the specified position or
         * 0 if the specified position lacks any tile animations.
         * @param p_shiny: special oam_id for poketore shiny grass
         */
        u8 getTileAnimation( u16 p_globX, u16 p_globY, bool p_shiny = false );

        /*
         * @brief: Clears permanent tile animations from the specified field
         */
        void clearFieldAnimation( u16 p_globX, u16 p_globY );

        /*
         * @brief: Reads the given map data and constructs mapObjects for the relevant
         * events. Also creates sprites for these events and makes them appear on the
         * screen.
         */
        void constructAndAddNewMapObjects( const mapData& p_data, u8 p_mapX, u8 p_mapY );

        void loadMapObject( std::pair<u8, mapObject>& p_mapObject );

        void updatePlayer( );

        void initWeather( );

        void draw( u16 p_globX, u16 p_globY, bool p_init );
        void drawPlayer( ObjPriority p_playerPrio = OBJPRIORITY_2, bool p_playerHidden = false );

        void fixMapObject( u8 p_objectId );
        void unfixMapObject( u8 p_objectId );

        void showExclamationAboveMapObject( u8 p_objectId );
        void moveMapObject( mapObject& p_mapObject, u8 p_spriteId, movement p_movement,
                            bool p_movePlayer = false, direction p_playerMovement = DOWN,
                            bool p_adjustAnim = true );
        void moveMapObject( u8 p_objectId, movement p_movement, bool p_movePlayer = false,
                            direction p_playerMovement = DOWN, bool p_adjustAnim = true );

        void moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows = true );

        void loadNewRow( direction p_direction, bool p_updatePlayer );
        void loadSlice( direction p_direction ); // dir: dir that needs to be extended

        void stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn = true,
                     bool p_unfade = false );

        void animateField( u16 p_globX, u16 p_globY );

        void runEvent( mapData::event p_event, u8 p_objectId = 0, s16 p_mapX = -1,
                       s16 p_mapY = -1 );

        void executeScript( u16 p_scriptId, u8 p_mapObject = 0, s16 p_mapX = -1, s16 p_mapY = -1 );

        void handleEvents( u8 p_localX, u8 p_localY, u8 p_z );
        void handleEvents( u16 p_localX, u16 p_localY, u8 p_z, direction p_dir );
        void handleWarp( warpType p_type, warpPos p_source );
        void handleWarp( warpType p_type );

        bool handleTracerPkmn( u8 p_tracerSlot );

        void handleWildPkmn( u16 p_globX, u16 p_globY );
        bool handleWildPkmn( wildPkmnType p_type, bool p_forceEncounter = false );

        BATTLE::battlePolicy getBattlePolicy( bool               p_isWildBattle,
                                              BATTLE::battleMode p_mode = BATTLE::BM_SINGLE,
                                              bool               p_distributeEXP = true );

        std::pair<bool, mapData::event::data> getWarpData( u16 p_globX, u16 p_globY, u8 p_z = 3 );

        /*
         * @brief: Runs all events with an "on map enter" trigger.
         * Called "level script" for historic reasons.
         */
        void runLevelScripts( const mapData& p_data, u16 p_mapX, u16 p_mapY );

        /*
         * @brief: Runs a battle factory challenge, starting at the player standing in the
         * waiting room. Hands out any prizes
         */
        void runBattleFactory( const ruleSet& p_rules );

      public:
        const block&        at( u16 p_x, u16 p_y ) const;
        block&              at( u16 p_x, u16 p_y );
        const mapBlockAtom& atom( u16 p_x, u16 p_y ) const;
        mapBlockAtom&       atom( u16 p_x, u16 p_y );
        const mapData&      currentData( ) const;
        const mapData&      currentData( u16 p_x, u16 p_y ) const;

        mapDrawer( );

        inline ~mapDrawer( ) {
            if( _currentBank != nullptr ) { fclose( _currentBank ); }
            _currentBank = nullptr;
            if( _tileset != nullptr ) { fclose( _tileset ); }
            _tileset = nullptr;
        }

        /*
         * @brief: Returns if the tracer is charged.
         */
        inline bool tracerCharged( ) const {
            return _tracerCharge >= TRACER_CHARGED;
        }

        /*
         * @brief: Returns if tracer could be used at given pos (if it is additionally
         * charged.)
         */
        bool tracerUsable( position p_position ) const {
            if( currentData( ).m_mapType & MAP::INSIDE ) { return false; }

            auto lstblock  = at( p_position.m_posX, p_position.m_posY );
            u8   lstBehave = lstblock.m_bottombehave;

            switch( lstBehave ) {
                // Grass
            case BEH_GRASS:
            case BEH_GRASS_UNDERWATER:
            case BEH_GRASS_ASH: return true;
            default: return false;
            }
        }

        /*
         * @brief: Starts the poketore.
         */
        void useTracer( );

        direction getFollowPkmnDirection( ) const;

        inline mapWeather getWeather( ) const {
            return SAVE::SAV.getActiveFile( ).m_currentMapWeather;
        }

        /*
         * @brief: Makes mapObject[ p_objectId ] mimic the movements of the player. At
         * most 1 mapObject may be attached to the player at once.
         */
        void attachMapObjectToPlayer( u8 p_objectId );

        /*
         * @brief: Removes any mapobjects from the player.
         */
        void removeAttachedObjects( );

        /*
         * @brief: Checks if the player can ride their bike at the specified position.
         */
        inline bool canBike( position p_start ) const {
            if( currentData( ).m_mapType & MAP::INSIDE ) { return false; }

            auto lstblock  = at( p_start.m_posX, p_start.m_posY );
            u8   lstBehave = lstblock.m_bottombehave;

            switch( lstBehave ) {
            case BEH_LONG_GRASS:
            case BEH_LADDER:
            case BEH_PACIFIDLOG_LOG_VERTICAL_TOP:
            case BEH_PACIFIDLOG_LOG_VERTICAL_BOTTOM:
            case BEH_PACIFIDLOG_LOG_HORIZONTAL_LEFT:
            case BEH_PACIFIDLOG_LOG_HORIZONTAL_RIGHT: return false;
            case BEH_FORRTREE_BRIDGE_BIKE_BELOW: return p_start.m_posZ <= 3;
            default: break;
            }

            return true;
        }

        /*
         * @brief: Checks if the player can get off their bike at the specified position.
         */
        inline bool canGetOffBike( position p_start ) const {
            if( SAVE::SAV.getActiveFile( ).m_forcedMovement & BIKE ) { return false; }

            auto lstblock  = at( p_start.m_posX, p_start.m_posY );
            u8   lstBehave = lstblock.m_bottombehave;

            switch( lstBehave ) {
            case BEH_BIKE_BRIDGE_VERTICAL:
            case BEH_BIKE_BRIDGE_HORIZONTAL:
            case BEH_BIKE_BRIDGE_VERTICAL_NO_JUMP:
            case BEH_BIKE_BRIDGE_HORIZONTAL_NO_JUMP: return false;
            default: break;
            }

            return true;
        }

        /*
         * @brief: Faints the player and teleports them to the last visited PC to heal
         * their PKMN.
         */
        void faintPlayer( );

        /*
         * @brief: Makes big boulders movable.
         */
        constexpr void enableStrength( ) {
            _strengthUsed = true;
        }

        constexpr bool strengthEnabled( ) const {
            return _strengthUsed;
        }

        /*
         * @brief: Shows a short animation and destroys the hmobject (breakable rock,
         * small tree) at the specified global position on the current map.
         */
        void destroyHMObject( u16 p_globX, u16 p_globY );

        void registerOnBankChangedHandler( std::function<void( u8 )> p_handler );
        void registerOnLocationChangedHandler( std::function<void( u16 )> p_handler );
        void registerOnMoveModeChangedHandler( std::function<void( moveMode )> p_handler );
        void registerOnWeatherChangedHandler( std::function<void( mapWeather )> p_handler );

        /*
         * @brief: Plays a door opening animation if the specified position is a door that
         * has an animation.
         */
        void openDoor( u16 p_globX, u16 p_globY, u8 p_z = 3 );

        void closeDoor( u16 p_globX, u16 p_globY, u8 p_z = 3 );

        void animateDoor( u16 p_globX, u16 p_globY, u8 p_z = 3, bool p_close = false );

        void changeWeather( mapWeather p_newWeather );

        void draw( ObjPriority p_playerPrio = OBJPRIORITY_2, bool p_playerHidden = false );

        void interact( );

        void animateMap( u8 p_frame );

        void animateMapObjects( u8 p_frame );

        bool checkTrainerEye( u16 p_globPlayerX, u16 p_globPlayerY );

        bool canMove( position p_start, direction p_direction, moveMode p_moveMode = WALK,
                      bool p_events = true );
        void movePlayer( direction p_direction, bool p_fast = false );

        void bikeJumpPlayer( direction p_direction );
        void jumpPlayer( direction p_direction );
        void slidePlayer( direction p_direction );
        void walkPlayer( direction p_direction, bool p_fast = false );

        void warpPlayer( warpType p_type, warpPos p_target );

        /*
         * @brief: Player falls through a hole in the ground to relative position +(0,32,0).
         */
        void fallthroughPlayer( );

        void stopPlayer( );
        void stopPlayer( direction p_direction );
        void changeMoveMode( moveMode p_newMode, bool p_hidden = false );

        void redirectPlayer( direction p_direction, bool p_fast, bool p_force = false );

        void standUpPlayer( direction p_direction );
        void sitDownPlayer( direction p_direction, moveMode p_newMoveMode );

        bool canFish( position p_start, direction p_direction );
        void fishPlayer( direction p_direction, u8 p_rodType = 0 );

        /*
         * @brief: Plays the badge award animation.
         * @param p_type: Type of badge to award (0: HOENN badge, 1: Frontier symbol)
         * :param p_badge: Badge to award (1-8 or 11/12 - 71/72)
         */
        void awardBadge( u8 p_type, u8 p_badge );
        void usePkmn( const pkmnSpriteInfo& p_pkmn );
        void showPkmn( const pkmnSpriteInfo& p_pkmn, bool p_cry );

        /*
         * @brief: Makes the follow pkmn use a hm move.
         * @returns: true if the animation played, false otherwise (e.g. if the follow
         * pkmn is hidden)
         */
        bool useFollowPkmn( );

        /*
         * @brief: Enables the player to buy the specified items using their money.
         * @param p_paymentMethod: 0: money, 1: BP, 2: Coins, 3: Ash
         */
        void runPokeMart( const std::vector<std::pair<u16, u32>>& p_offeredItems,
                          const char* p_message = 0, bool p_allowItemSell = true,
                          u8 p_paymentMethod = 0 );

        /*
         * @brief: Disables wild pkmn for the specified number of steps.
         */
        void disablePkmn( s16 p_steps = -1 );
        void enablePkmn( );

        bool requestWildPkmn( bool p_forceHighGrass = false );

        u16 getCurrentLocationId( ) const;
    };
    extern mapDrawer* curMap;
    extern pokemon    WILD_PKMN;
} // namespace MAP
