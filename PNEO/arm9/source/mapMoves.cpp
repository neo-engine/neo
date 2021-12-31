/*
Pokémon neo
------------------------------

file        : mapMoves.cpp
author      : Philip Wellnitz
description :

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
    u16 text( const u16 p_moveId, u8 p_param ) {
        if( p_param == 1 ) {
            switch( p_moveId ) {
            case M_DIVE: return 322;
            default: return 0;
            }
        }
        switch( p_moveId ) {
        case M_CUT: return 313;
        case M_ROCK_SMASH: return 314;
        case M_WHIRLPOOL: return 315;
        case M_SURF: return 316;
        case M_DIVE: return 317;
        case M_STRENGTH:
            if( MAP::curMap->strengthEnabled( ) ) {
                return 558;
            } else {
                return 318;
            }
        case M_ROCK_CLIMB: return 319;
        case M_WATERFALL: return 320;
        case M_HEADBUTT: return 321;
        default: return 0;
        }
    }

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
            // Check for badge 1
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 0 ) ) ) { return false; }

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
            // Check for badge 3
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 2 ) ) ) { return false; }

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
            // Check for badge 4
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 3 ) ) ) { return false; }

            // Check if strength has already been used
            if( MAP::curMap->strengthEnabled( ) ) { return false; }

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

        case M_FLY: {
            // Check for badge 6
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 5 ) ) ) { return false; }
            return false;
        }
        case M_FLASH: {
            return SAVE::SAV.getActiveFile( ).m_currentMapWeather == MAP::DARK_FLASHABLE;
        }
        case M_WHIRLPOOL: {
            // Check for badge 7
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 6 ) ) ) { return false; }
            return false;
        }
        case M_SURF:
            // Check for badge 5
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 4 ) ) ) { return false; }

            return ( SAVE::SAV.getActiveFile( ).m_player.m_movement != MAP::SURF )
                   && MAP::curMap->atom( tx, ty ).m_movedata == 0x4
                   && MAP::curMap
                              ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata
                          == 0xc;
        case M_DIVE: {
            // Check for badge 7
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 6 ) ) ) { return false; }
            return false;
        }
        case M_DEFOG: {
            // Check for badge 6
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 5 ) ) ) { return false; }
            return false;
        }
        case M_ROCK_CLIMB: {
            // Check for badge 8
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 7 ) ) ) { return false; }
            return MAP::curMap->at( tx, ty ).m_bottombehave == 0x12;
        }
        case M_WATERFALL: {
            // Check for badge 8
            if( !( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << 7 ) ) ) { return false; }
            return ( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::SURF )
                   && MAP::curMap->at( tx, ty ).m_bottombehave == 0x13;
        }
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
        case M_FLASH: {
            if( SAVE::SAV.getActiveFile( ).m_currentMapWeather == MAP::DARK_FLASHABLE ) {
                bgSetScale( IO::bg3, 1 << 7 | 1 << 6 | 1 << 5, 1 << 7 | 1 << 6 | 1 << 5 );
                bgSetScroll( IO::bg3, 112 - 96, 84 - 72 );
                bgUpdate( );
                for( u8 i = 0; i < 8; ++i ) { swiWaitForVBlank( ); }
                bgSetScale( IO::bg3, 1 << 7 | 1 << 6, 1 << 7 | 1 << 6 );
                bgSetScroll( IO::bg3, 96 - 64, 72 - 48 );
                bgUpdate( );
                for( u8 i = 0; i < 8; ++i ) { swiWaitForVBlank( ); }
                bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
                bgSetScroll( IO::bg3, 64, 48 );
                bgUpdate( );
                SAVE::SAV.getActiveFile( ).m_currentMapWeather = MAP::DARK_FLASH_USED;
            }
            return;
        }
        case M_WHIRLPOOL: return;
        case M_SURF: {
            MAP::direction d  = SAVE::SAV.getActiveFile( ).m_player.m_direction;
            MAP::direction fp = MAP::curMap->getFollowPkmnDirection( );
            MAP::curMap->sitDownPlayer( d, MAP::SURF );
            if( p_param == 2 ) {
                // move got used by follow pkmn, make player climb in correct direction
                d = fp;
                MAP::curMap->redirectPlayer( d, false );
                MAP::curMap->walkPlayer( d );
            }

            return;
        }
        case M_DIVE: return;
        case M_DEFOG: return;
        case M_ROCK_CLIMB: {
            MAP::direction d  = SAVE::SAV.getActiveFile( ).m_player.m_direction;
            MAP::direction fp = MAP::curMap->getFollowPkmnDirection( );

            MAP::curMap->sitDownPlayer( d, MAP::ROCK_CLIMB );
            if( p_param == 2 ) {
                // move got used by follow pkmn, make player climb in correct direction
                d = fp;
                MAP::curMap->redirectPlayer( d, false );
            }

            while( possible( M_ROCK_CLIMB, 0 ) ) MAP::curMap->walkPlayer( d );
            MAP::curMap->standUpPlayer( d );
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
                       != 0x0a ) {
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ
                    = MAP::curMap
                          ->atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                  SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata
                      / 4;
            }
            return;
        }
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
