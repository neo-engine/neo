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
#include "locationNames.h"
#include "mapDrawer.h"
#include "mapSlice.h"
#include "mapSprite.h"
#include "moveNames.h"
#include "nav.h"
#include "saveGame.h"
#include "uio.h"

namespace MOVE {
    bool possible( u16 p_moveId, u8 p_param ) {
        if( p_param == 1 ) {
            switch( p_moveId ) {
            case M_DIVE: return false;
            default: return false;
            }
        }

        u16 tx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                 + MAP::dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 0 ];
        u16 ty = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                 + MAP::dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 1 ];

        switch( p_moveId ) {
        case M_CUT: {
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

                if( o.second.m_pos.m_posX != tx || o.second.m_pos.m_posY != ty ) { continue; }
                if( o.second.m_event.m_type == MAP::EVENT_HMOBJECT
                    && o.second.m_event.m_data.m_hmObject.m_hmType
                           == MAP::mapSpriteManager::SPR_CUT ) {
                    return true;
                }
            }
            return false;
        }
        case M_ROCK_SMASH: {
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

                if( o.second.m_pos.m_posX != tx || o.second.m_pos.m_posY != ty ) { continue; }
                if( o.second.m_event.m_type == MAP::EVENT_HMOBJECT
                    && o.second.m_event.m_data.m_hmObject.m_hmType
                           == MAP::mapSpriteManager::SPR_ROCKSMASH ) {
                    return true;
                }
            }
            return false;
        }
        case M_STRENGTH: {
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

                if( o.second.m_pos.m_posX != tx || o.second.m_pos.m_posY != ty ) { continue; }
                if( o.second.m_event.m_type == MAP::EVENT_HMOBJECT
                    && o.second.m_event.m_data.m_hmObject.m_hmType
                           == MAP::mapSpriteManager::SPR_STRENGTH ) {
                    return true;
                }
            }
            return false;
        }

        case M_FLY: return false;
        case M_FLASH: return false;
        case M_WHIRLPOOL: return false;
        case M_SURF:
            return ( SAVE::SAV.getActiveFile( ).m_player.m_movement != MAP::SURF )
                   && MAP::curMap->atom( tx, ty ).m_movedata == 0x4
                   && MAP::curMap
                              ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata
                          == 0xc;
        case M_DIVE: return false;
        case M_DEFOG: return false;
        case M_ROCK_CLIMB: return MAP::curMap->at( tx, ty ).m_bottombehave == 0x12;
        case M_WATERFALL:
            return ( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::SURF )
                   && MAP::curMap->at( tx, ty ).m_bottombehave == 0x13;
        case M_HEADBUTT: return false;
        case M_SWEET_SCENT: {
            u8 curBehave = MAP::curMap
                               ->at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                     SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                               .m_bottombehave;
            return curBehave == 0x2 || curBehave == 0x3;
        }
        case M_TELEPORT:
            if( MAP::curMap->getCurrentLocationId( ) == L_POKEMON_CENTER ) { return false; }
            return !!SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first
                   && SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first != 255;
        case M_DIG:
            if( !( MAP::curMap->currentData( ).m_mapType & MAP::CAVE ) ) { return false; }
            return !!SAVE::SAV.getActiveFile( ).m_lastCaveEntry.first
                   && SAVE::SAV.getActiveFile( ).m_lastCaveEntry.first != 255;
        default: return false;
        }
    }

    void use( u16 p_moveId, u8 p_param ) {
        if( p_param == 1 ) {
            switch( p_moveId ) {
            case M_DIVE: return;
            default: return;
            }
        }

        u16 tx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                 + MAP::dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 0 ];
        u16 ty = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                 + MAP::dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 1 ];

        switch( p_moveId ) {
        case M_CUT:
        case M_ROCK_SMASH: MAP::curMap->destroyHMObject( tx, ty ); return;
        case M_STRENGTH: MAP::curMap->enableStrength( ); return;
        case M_FLY: return;
        case M_FLASH: return;
        case M_WHIRLPOOL: return;
        case M_SURF:
            MAP::curMap->sitDownPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction,
                                        MAP::SURF );
            return;
        case M_DIVE: return;
        case M_DEFOG: return;
        case M_ROCK_CLIMB:
            MAP::curMap->sitDownPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction,
                                        MAP::ROCK_CLIMB );
            while( possible( M_ROCK_CLIMB, 0 ) )
                MAP::curMap->walkPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            MAP::curMap->standUpPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            if( MAP::curMap
                        ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                        .m_movedata
                    > 4
                && MAP::curMap
                           ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                   SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_movedata
                       != 0x3c
                && MAP::curMap
                           ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                   SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_movedata
                       != 0x0a )
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ
                    = MAP::curMap
                          ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                  SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata
                      / 4;
            return;
        case M_WATERFALL:
            MAP::curMap->disablePkmn( );
            while( possible( M_WATERFALL, 0 ) )
                MAP::curMap->walkPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            MAP::curMap->walkPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            MAP::curMap->enablePkmn( );
            return;
        case M_HEADBUTT: return;
        case M_SWEET_SCENT:
            if( !possible( M_SWEET_SCENT, 0 ) || !MAP::curMap->requestWildPkmn( true ) ) {
                NAV::printMessage( GET_STRING( 90 ) );
            }
            return;
        case M_TELEPORT:
            MAP::curMap->warpPlayer( MAP::TELEPORT, SAVE::SAV.getActiveFile( ).m_lastPokeCenter );
            return;
        case M_DIG:
            MAP::curMap->warpPlayer( MAP::CAVE_ENTRY, SAVE::SAV.getActiveFile( ).m_lastCaveEntry );
            return;
        default: return;
        }
    }
} // namespace MOVE
