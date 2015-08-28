/*
Pok�mon Emerald 2 Version
------------------------------

file        : mapDrawer.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2015
Philip Wellnitz

This file is part of Pok�mon Emerald 2 Version.

Pok�mon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pok�mon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pok�mon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <map>
#include <memory>

#include "mapDefines.h"
#include "mapSlice.h"
#include "mapObject.h"
#include "mapSprite.h"
#include "hmMoves.h"

namespace MAP {
    class mapDrawer {
    private:
        enum wildPkmnType {
            GRASS,
            HIGH_GRASS,
            WATER,
            CAVE_WALK,
            FISHING_ROD
        };
        enum mapWeather {
            NOTHING, //Inside
            SUNNY,
            REGULAR,
            RAINY,
            SNOW,
            THUNDERSTORM,
            MIST,
            BLIZZARD,
            SANDSTORM,
            MIST_2,
            DENSE_MIST,
            CLOUDY,
            HEAVY_SUNLIGHT,
            HEAVY_RAIN,
            UNDERWATER
        };
        enum mapType {
            OUTSIDE = 0,
            CAVE = 1,
            INSIDE = 2,
            DARK = 4,
            FLASHABLE = 8
        };
        enum warpType {
            NORMAL,
            LAST_VISITED
        };

        std::map<u8, mapType> _mapTypes = { { 10, OUTSIDE } };
        mapWeather _weather;

        std::unique_ptr<mapSlice> _slices[ 2 ][ 2 ] = { { 0 } };  //[x][y]
        u8          _curX, _curY;       //Current main slice from the _slices array
        std::map<std::pair<u16, u16>, std::vector<mapObject>>
            _mapObjs;
        bool _playerIsFast;

        mapSprite _sprites[ 16 ];
        u16 _entriesUsed;
        std::map<u16, u8> _spritePos; //mapObject.id -> index in _sprites

        void draw( u16 p_globX, u16 p_globY, bool p_init );
        void drawPlayer( );
        void drawObjects( );

        void moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows = true );

        void loadNewRow( direction p_direction, bool p_updatePlayer );
        void loadSlice( direction p_direction ); //dir: dir that needs to be extended

        void animateField( u16 p_globX, u16 p_globY );

        void jumpPlayer( direction p_direction );
        void slidePlayer( direction p_direction );
        void walkPlayer( direction p_direction, bool p_fast = false );

        void handleWarp( warpType p_type );
        void handleWildPkmn( wildPkmnType p_type );
        void handleTrainer( );

        block& at( u16 p_x, u16 p_y ) const;
        mapBlockAtom& atom( u16 p_x, u16 p_y ) const;
    public:
        friend bool surf::possible( );


        mapDrawer( );

        void draw( );

        bool canMove( position p_start,
                      direction p_direction,
                      moveMode p_moveMode = WALK );
        void movePlayer( direction p_direction, bool p_fast = false );

        void stopPlayer( );
        void stopPlayer( direction p_direction );
        void changeMoveMode( moveMode p_newMode );

        void redirectPlayer( direction p_direction, bool p_fast );

        void standUpPlayer( direction p_direction );
        void sitDownPlayer( direction p_direction, moveMode p_newMoveMode );

        bool canFish( position p_start,
                      direction p_direction );
        void fishPlayer( direction p_direction );

        void usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny );

        u16  getCurrentLocationId( ) const;
    };
    extern mapDrawer* curMap;
}