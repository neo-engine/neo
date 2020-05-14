/*
Pokémon neo
------------------------------

file        : mapMoves.cpp
author      : Philip Wellnitz
description :

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

#include "defines.h"
#include "mapDrawer.h"
#include "mapSlice.h"
#include "messageBox.h"
#include "saveGame.h"
#include "uio.h"
#include "moveNames.h"

namespace MOVE {
    u16 text( const u16 p_moveId, u8 p_param ) {
        if( p_param == 1 ) {
            switch( p_moveId ) {
                case M_DIVE:
                    return 322;
                default:
                    return 0;
            }
        }
        switch( p_moveId ) {
            case M_CUT:
                return 313;
            case M_ROCK_SMASH:
                return 314;
            case M_WHIRLPOOL:
                return 315;
            case M_SURF:
                return 316;
            case M_DIVE:
                return 317;
            case M_STRENGTH:
                return 318;
            case M_ROCK_CLIMB:
                return 319;
            case M_WATERFALL:
                return 320;
            case M_HEADBUTT:
                return 321;
            default:
                return 0;
        }
    }

    bool isFieldMove( u16 p_moveId ) {
        switch( p_moveId ) {
            case M_CUT: case M_ROCK_SMASH: case M_FLY: case M_FLASH: case M_WHIRLPOOL:
            case M_SURF: case M_DIVE: case M_DEFOG: case M_STRENGTH: case M_ROCK_CLIMB:
            case M_WATERFALL: case M_TELEPORT: case M_HEADBUTT: case M_SWEET_SCENT: case M_DIG:
                return true;
            default:
                return false;
        }
    }

    bool possible( u16 p_moveId, u8 p_param ) {
        if( p_param == 1 ) {
            switch( p_moveId ) {
                case M_DIVE:
                    return false;
                default:
                    return false;
            }
        }

        switch( p_moveId ) {
            case M_CUT:
                return false;
            case M_ROCK_SMASH:
                return false;
            case M_FLY:
                return false;
            case M_FLASH:
                return false;
            case M_WHIRLPOOL:
                return false;
            case M_SURF:
                return ( SAVE::SAV->getActiveFile( ).m_player.m_movement != MAP::SURF )
                    && MAP::curMap
                    ->atom(
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX
                            + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY
                            + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] )
                    .m_movedata
                    == 0x4
                    && MAP::curMap
                    ->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                    .m_movedata
                    == 0xc;
            case M_DIVE:
                return false;
            case M_DEFOG:
                return false;
            case M_STRENGTH:
                return false;
            case M_ROCK_CLIMB:
                return MAP::curMap
                    ->at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX
                            + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY
                            + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] )
                    .m_bottombehave
                    == 0x12;
            case M_WATERFALL:
                return ( SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::SURF )
                    && MAP::curMap
                    ->at(
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX
                            + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY
                            + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] )
                    .m_bottombehave
                    == 0x13;
            case M_TELEPORT:
                return false;
            case M_HEADBUTT:
                return false;
            case M_SWEET_SCENT: {
                u8 curBehave = MAP::curMap
                    ->at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                    .m_bottombehave;
                return curBehave == 0x2 || curBehave == 0x3;
            }
            case M_DIG:
                return false;
            default:
                return false;
        }
    }

    void use( u16 p_moveId, u8 p_param ) {
        if( p_param == 1 ) {
            switch( p_moveId ) {
                case M_DIVE:
                    return;
                default:
                    return;
            }
        }

        switch( p_moveId ) {
            case M_CUT:
                return;
            case M_ROCK_SMASH:
                return;
            case M_FLY:
                return;
            case M_FLASH:
                return;
            case M_WHIRLPOOL:
                return;
            case M_SURF:
                MAP::curMap->sitDownPlayer( SAVE::SAV->getActiveFile(
                            ).m_player.m_direction, MAP::SURF );
                return;
            case M_DIVE:
                return;
            case M_DEFOG:
                return;
            case M_STRENGTH:
                return;
            case M_ROCK_CLIMB:
                MAP::curMap->sitDownPlayer( SAVE::SAV->getActiveFile(
                            ).m_player.m_direction, MAP::ROCK_CLIMB );
                while( possible( M_ROCK_CLIMB, 0 ) )
                    MAP::curMap->walkPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
                MAP::curMap->standUpPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
                if( MAP::curMap
                        ->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                        .m_movedata
                        > 4
                        && MAP::curMap
                        ->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                        .m_movedata
                        != 0x3c
                        && MAP::curMap
                        ->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                        .m_movedata
                        != 0x0a )
                    SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ
                        = MAP::curMap
                        ->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                        .m_movedata
                        / 4;
                return;
            case M_WATERFALL:
                MAP::curMap->disablePkmn( );
                while( possible( M_WATERFALL, 0 ) )
                    MAP::curMap->walkPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
                MAP::curMap->walkPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
                MAP::curMap->enablePkmn( );
                return;
            case M_TELEPORT:
                return;
            case M_HEADBUTT:
                return;
            case M_SWEET_SCENT:
                if( !possible( M_SWEET_SCENT, 0 ) || !MAP::curMap->requestWildPkmn( true ) ) {
                    IO::messageBox( GET_STRING( 90 ), true );
                    NAV::draw( );
                }
                return;
            case M_DIG:
                return;
            default:
                return;
        }
    }
}
