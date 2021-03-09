/*
Pokémon neo
------------------------------

file        : mapDrawer.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2021
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

#include "battle.h"
#include "mapDefines.h"
#include "mapObject.h"
#include "mapSlice.h"
#include "mapSprite.h"
#include "move.h"

namespace MAP {
    class mapDrawer {
      private:
        mapSpriteManager _mapSprites;

        ObjPriority _lastPlayerPriority = OBJPRIORITY_1;
        direction   _lastPlayerMove     = DOWN;

        mapSlice _slices[ 2 ][ 2 ]; //[x][y]
        u8       _curX, _curY;      // Current main slice from the _slices array

        s8   _weatherScrollX = 0;
        s8   _weatherScrollY = 0;
        bool _weatherFollow  = false;

        mapData _data[ 2 ][ 2 ];

        std::map<position, u8> _tileAnimations;

        std::set<u16> _fixedMapObjects;

        constexpr u16 dist( u16 p_globX1, u16 p_globY1, u16 p_globX2, u16 p_globY2 ) {
            return std::max( std::abs( p_globX1 - p_globX2 ), std::abs( p_globY1 - p_globY2 ) );
        }

        u8        _playerSprite           = 255; // id of the player sprite
        u8        _playerPlatSprite       = 255; // id of the player platform sprite
        u8        _playerFollowPkmnSprite = 255; // id of the pkmn ow sprite that follows the player
        bool      _pkmnFollowsPlayer      = false;
        mapObject _followPkmn;
        bool      _forceNoFollow = false;

        bool _strengthUsed; // Player has used HM Strength and can move boulders

        u16 _lastrow, // Row to be filled when extending the map to the top
            _lastcol; // Column to be filled when extending the map to the left

        u16 _cx, _cy; // Cameras's pos

        /*
         * @brief: Updates the pkmn species etc following the player. Returns false on
         * failure.
         */
        bool updateFollowPkmn( );

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
         */
        u8 getTileAnimation( u16 p_globX, u16 p_globY );

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

        bool _playerIsFast;

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

        void updatePlayer( );

        void initWeather( );

        void draw( u16 p_globX, u16 p_globY, bool p_init );
        void drawPlayer( ObjPriority p_playerPrio = OBJPRIORITY_2 );

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

        void handleWildPkmn( u16 p_globX, u16 p_globY );
        bool handleWildPkmn( wildPkmnType p_type, bool p_forceEncounter = false );

        BATTLE::battlePolicy getBattlePolicy( bool               p_isWildBattle,
                                              BATTLE::battleMode p_mode          = BATTLE::SINGLE,
                                              bool               p_distributeEXP = true );

        std::pair<bool, mapData::event::data> getWarpData( u16 p_globX, u16 p_globY, u8 p_z = 3 );

        /*
         * @brief: Runs all events with an "on map enter" trigger.
         * Called "level script" for historic reasons.
         */
        void runLevelScripts( const mapData& p_data, u16 p_mapX, u16 p_mapY );

      public:
        const block&        at( u16 p_x, u16 p_y ) const;
        block&              at( u16 p_x, u16 p_y );
        const mapBlockAtom& atom( u16 p_x, u16 p_y ) const;
        mapBlockAtom&       atom( u16 p_x, u16 p_y );
        const mapData&      currentData( ) const;
        const mapData&      currentData( u16 p_x, u16 p_y ) const;

        mapDrawer( );

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
                // Long grass
            case 0x03:
                // ladder
            case 0x0a:
                //
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
                return false;
                // bridge in forrtree
            case 0x78: return p_start.m_posZ <= 3;
            default: break;
            }

            return true;
        }

        /*
         * @brief: Checks if the player can get off their bike at the specified position.
         */
        inline bool canGetOffBike( position p_start ) const {
            auto lstblock  = at( p_start.m_posX, p_start.m_posY );
            u8   lstBehave = lstblock.m_bottombehave;

            switch( lstBehave ) {
            case 0xd3:
            case 0xd4:
            case 0xd5:
            case 0xd6: return false;
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

        void changeWeather( mapWeather p_newWeather );

        void draw( ObjPriority p_playerPrio = OBJPRIORITY_2 );

        void interact( );

        void animateMap( u8 p_frame );

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
        void changeMoveMode( moveMode p_newMode );

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
        void usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny, u8 p_forme );

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
        u16 getCurrentLocationId( u8 p_file ) const;
    };
    extern mapDrawer* curMap;
} // namespace MAP
