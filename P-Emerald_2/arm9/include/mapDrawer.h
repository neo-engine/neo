/*
Pokémon neo
------------------------------

file        : mapDrawer.h
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
        mapWeather       _weather;
        mapSpriteManager _mapSprites;

        mapSlice _slices[ 2 ][ 2 ]; //[x][y]
        u8       _curX, _curY;      // Current main slice from the _slices array

        mapData _data[ 2 ][ 2 ];

        constexpr u16 dist( u16 p_globX1, u16 p_globY1, u16 p_globX2, u16 p_globY2 ) {
            return std::max( std::abs( p_globX1 - p_globX2 ), std::abs( p_globY1 - p_globY2 ) );
        }

        u8 _playerSprite;     // id of the player sprite
        u8 _playerPlatSprite; // id of the player platform sprite

        std::vector<std::pair<u8, mapObject>> _objects;

        bool _strengthUsed; // Player has used HM Strength and can move boulders

        u16 _lastrow, // Row to be filled when extending the map to the top
            _lastcol; // Column to be filled when extending the map to the left

        u16 _cx, _cy; // Cameras's pos

        void loadBlock( block p_curblock, u32 p_memPos );
        void loadBlock( block p_curblock, u8 p_scrnX, u8 p_scrnY );

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

        void draw( u16 p_globX, u16 p_globY, bool p_init );
        void drawPlayer( ObjPriority p_playerPrio = OBJPRIORITY_2 );
        void drawObjects( );

        void moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows = true );

        void loadNewRow( direction p_direction, bool p_updatePlayer );
        void loadSlice( direction p_direction ); // dir: dir that needs to be extended

        void stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn = true );

        void animateField( u16 p_globX, u16 p_globY );

        void runEvent( mapData::event p_event );

        void handleEvents( u8 p_localX, u8 p_localY, u8 p_z );
        void handleEvents( u16 p_localX, u16 p_localY, u8 p_z, direction p_dir );
        void handleWarp( warpType p_type, warpPos p_source );
        void handleWarp( warpType p_type );

        void handleWildPkmn( u16 p_globX, u16 p_globY );
        bool handleWildPkmn( wildPkmnType p_type, bool p_forceEncounter = false );
        void handleTrainer( );

        BATTLE::battlePolicy getBattlePolicy( bool               p_isWildBattle,
                                              BATTLE::battleMode p_mode          = BATTLE::SINGLE,
                                              bool               p_distributeEXP = true );

      public:
        const block&        at( u16 p_x, u16 p_y ) const;
        block&              at( u16 p_x, u16 p_y );
        const mapBlockAtom& atom( u16 p_x, u16 p_y ) const;
        mapBlockAtom&       atom( u16 p_x, u16 p_y );
        const mapData&      currentData( ) const;
        const mapData&      currentData( u16 p_x, u16 p_y ) const;

        mapDrawer( );

        constexpr mapWeather getWeather( ) const {
            return _weather;
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

        void draw( ObjPriority p_playerPrio = OBJPRIORITY_2 );

        void interact( );

        void animateMap( u8 p_frame );

        bool canMove( position p_start, direction p_direction, moveMode p_moveMode = WALK );
        void movePlayer( direction p_direction, bool p_fast = false );

        void jumpPlayer( direction p_direction );
        void slidePlayer( direction p_direction );
        void walkPlayer( direction p_direction, bool p_fast = false );

        void warpPlayer( warpType p_type, warpPos p_target );

        void stopPlayer( );
        void stopPlayer( direction p_direction );
        void changeMoveMode( moveMode p_newMode );

        void redirectPlayer( direction p_direction, bool p_fast );

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
