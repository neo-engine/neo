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
#include <map>
#include <memory>
#include <functional>
#include <vector>

#include "move.h"
#include "mapDefines.h"
#include "mapObject.h"
#include "mapSlice.h"
#include "mapSprite.h"
#include "battle.h"

namespace MAP {
    class mapDrawer {
        private:
            mapWeather _weather;

            std::unique_ptr<mapSlice> _slices[ 2 ][ 2 ] = {{0}}; //[x][y]
            u8                        _curX, _curY; // Current main slice from the _slices array
            std::map<std::pair<u16, u16>, std::vector<mapObject>> _mapObjs;

            bool _playerIsFast;

            mapSprite         _sprites[ 16 ];
            u16               _entriesUsed;
            std::map<u16, u8> _spritePos; // mapObject.id -> index in _sprites

            std::vector<std::function<void( u16 )>>      _newLocationCallbacks
                = std::vector<std::function<void( u16 )>>( ); // Called whenever player makes a step
            std::vector<std::function<void( u8 )>>      _newBankCallbacks
                = std::vector<std::function<void( u8 )>>( ); // Called when a map bank was loaded
            std::vector<std::function<void( moveMode )>> _newMoveModeCallbacks
                = std::vector<std::function<void( moveMode )>>( ); // Called when the player's moveMode changed

            void draw( u16 p_globX, u16 p_globY, bool p_init );
            void drawPlayer( ObjPriority p_playerPrio = OBJPRIORITY_2 );
            void drawObjects( );

            void moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows = true );

            void loadNewRow( direction p_direction, bool p_updatePlayer );
            void loadSlice( direction p_direction ); // dir: dir that needs to be extended

            void stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn = true );

            void animateField( u16 p_globX, u16 p_globY );

            bool executeScript( u8 p_map, u16 p_globX, u16 p_globY, u8 p_z, u8 p_number,
                    invocationType p_inv );

            void handleEvents( u16 p_globX, u16 p_globY, u8 p_z );
            void handleEvents( u16 p_globX, u16 p_globY, u8 p_z, direction p_dir );
            void handleWarp( warpType p_type, warpPos p_source );
            void handleWarp( warpType p_type );

            void handleWildPkmn( u16 p_globX, u16 p_globY );
            bool handleWildPkmn( wildPkmnType p_type, u8 p_rodType = 0, bool p_forceEncounter = false );
            void handleTrainer( );

            BATTLE::battlePolicy getBattlePolicy( bool p_isWildBattle,
                    BATTLE::battleMode p_mode = BATTLE::SINGLE,
                    bool p_distributeEXP = true );
        public:
            block&        at( u16 p_x, u16 p_y ) const;
            mapBlockAtom& atom( u16 p_x, u16 p_y ) const;

            mapDrawer( );

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

            void usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny );

            void disablePkmn( s16 p_steps = -1 );
            void enablePkmn( );

            bool requestWildPkmn( bool p_forceHighGrass = false );

            u16 getCurrentLocationId( ) const;
            u16 getCurrentLocationId( u8 p_file ) const;
    };
    extern mapDrawer* curMap;
} // namespace MAP
