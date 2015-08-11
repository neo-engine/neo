/*
Pokémon Emerald 2 Version
------------------------------

file        : mapDrawer.h
author      : Philip Wellnitz
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
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
#include <map>
#include <memory>

#include "mapSlice.h"
#include "mapObject.h"

namespace MAP {
    class mapDrawer {
    private:
        std::unique_ptr<mapSlice> _slices[ 2 ][ 2 ] = { { 0 } };  //[x][y]
        u8          _curX, _curY;       //Current main slice from the _slices array
        std::map<std::pair<u16,u16>, std::vector<mapObject>>
            _mapObjs;
        mapObject& _player;
        
        void draw( u16 p_globX, u16 p_globY, bool p_init );
        
        void moveCamera( mapSlice::direction p_direction, bool p_updatePlayer, bool p_autoLoadRows = true );

        void loadNewRow( mapSlice::direction p_direction, bool p_updatePlayer );
        void loadSlice( mapSlice::direction p_direction ); //dir: dir that needs to be extended

        void handleWarp( );
        void handleWildPkmn( );
        void handleTrainer( );

        MapBlockAtom at( u16 p_x, u16 p_y ) const;
    public:
        mapDrawer( u8 p_currentMap, mapObject& p_player );

        void draw( );

        bool canMove( mapSlice::position p_start,
                      mapSlice::direction p_direction,
                      mapSlice::moveMode p_moveMode = mapSlice::WALK );
        void movePlayer( mapSlice::direction p_direction );
        void stopPlayer( mapSlice::direction p_direction );
        void changeMoveMode( mapSlice::moveMode p_newMode );

        u16  getCurrentLocationId( ) const;
    };

    extern mapDrawer* curMap;
}