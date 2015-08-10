/*
Pokémon Emerald 2 Version
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine

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

#include "mapDrawer.h"
#include "uio.h"
#include "sprite.h"

namespace MAP {
    mapDrawer* curMap;
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) { }

    void mapDrawer::moveCamera( mapSlice::direction p_direction ) { }

    void mapDrawer::loatSlice( u8 p_x, u8 p_y, u16 p_sliceX, u16 p_sliceY ) { }
    void mapDrawer::loadSlice( mapSlice::direction p_direction ) { }

    void mapDrawer::handleWarp( ) { }
    void mapDrawer::handleWildPkmn( ) { }
    void mapDrawer::handleTrainer( ) { }

    mapDrawer::mapDrawer( u8 p_currentMap, mapObject& p_player )
        :m_player( p_player ) {
        _curX = _curY = 0;

        u16 mx = p_player.m_pos.m_posX, my = p_player.m_pos.m_posY;
        _slices[ _curX ][ _curY ] = mapSlice( p_currentMap, mx / SIZE, my / SIZE );
        _slices[ 1 - _curX ][ _curY ] = mapSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE );
        _slices[ _curX ][ 1 - _curY ] = mapSlice( p_currentMap, mx / SIZE, my / SIZE + currentHalf( my ) );
        _slices[ 1 - _curX ][ 1 - _curY ] = mapSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE + currentHalf( my ) );

    }

    void mapDrawer::draw( ) { }

    bool mapDrawer::canMove( mapSlice::position p_start,
                             mapSlice::direction p_direction,
                             mapSlice::moveMode p_moveMode ) {
        return false;
    }
    void mapDrawer::movePlayer( mapSlice::direction p_direction ) { }
    void mapDrawer::stopPlayer( mapSlice::direction p_direction ) { }
    void mapDrawer::changeMoveMode( mapSlice::moveMode p_newMode ) { }

    u16  mapDrawer::getCurrentLocationId( ) {
        return _slices[ _curX ][ _curY ].m_location;
    }
}