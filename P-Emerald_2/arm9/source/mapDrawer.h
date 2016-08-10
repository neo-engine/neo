/*
Pokémon Emerald 2 Version
------------------------------

file        : mapDrawer.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2016
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

#include "mapDefines.h"
#include "mapSlice.h"
#include "mapObject.h"
#include "mapSprite.h"
#include "hmMoves.h"

namespace MAP {
    class mapDrawer {
    private:
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
        void drawPlayer( ObjPriority p_playerPrio = OBJPRIORITY_2 );
        void drawObjects( );

        void moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows = true );

        void loadNewRow( direction p_direction, bool p_updatePlayer );
        void loadSlice( direction p_direction ); //dir: dir that needs to be extended

        void animateField( u16 p_globX, u16 p_globY );

        void handleWarp( warpType p_type );
        void handleWildPkmn( u16 p_globX, u16 p_globY );
        bool handleWildPkmn( wildPkmnType p_type, u8 p_rodType = 0, bool p_forceEncounter = false );
        void handleTrainer( );

    public:
        block& at( u16 p_x, u16 p_y ) const;
        mapBlockAtom& atom( u16 p_x, u16 p_y ) const;

        mapDrawer( );

        void draw( ObjPriority p_playerPrio = OBJPRIORITY_2 );

        void animateMap( u8 p_frame );

        bool canMove( position p_start,
                      direction p_direction,
                      moveMode p_moveMode = WALK );
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

        bool canFish( position p_start,
                      direction p_direction );
        void fishPlayer( direction p_direction, u8 p_rodType = 0 );

        void usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny );

        void disablePkmn( s16 p_steps = -1 );
        void enablePkmn( );

        bool requestWildPkmn( bool p_forceHighGrass = false );

        u16  getCurrentLocationId( ) const;
    };
    extern mapDrawer* curMap;
}
