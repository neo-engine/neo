/*
Pokémon neo
------------------------------

file        : mapScript.cpp
author      : Philip Wellnitz
description : Map script engine

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

#include <algorithm>

#include "battle.h"
#include "battleTrainer.h"
#include "battleDefines.h"
#include "defines.h"
#include "fs.h"
#include "mapDrawer.h"
#include "messageBox.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

namespace MAP {
#define MAX_SCRIPT_SIZE 64
    // opcode : u8, param1 : u8, param2 : u8, param3 : u8
    // opcode : u8, paramA : u12, paramB : u12
    u32 SCRIPT_INS[ MAX_SCRIPT_SIZE ];
#define OPCODE( p_ins ) ( ( p_ins ) >> 24 )
#define PARAM1( p_ins ) ( ( ( p_ins ) >> 16 ) & 0xFF )
#define PARAM2( p_ins ) ( ( ( p_ins ) >> 8 ) & 0xFF )
#define PARAM3( p_ins ) ( (p_ins) &0xFF )
#define PARAMA( p_ins ) ( ( ( p_ins ) >> 12 ) & 0xFFF )
#define PARAMB( p_ins ) ( (p_ins) &0xFFF )
#define FETCH( p_var )   \
    ( ( ( p_var ) == 0 ) \
          ? 0            \
          : ( ( ( p_var ) == 1 ) ? 1 : SAVE::SAV.getActiveFile( ).m_vars[ p_var ] ) )
#define FETCH_UNSEC( p_var ) ( SAVE::SAV.getActiveFile( ).m_vars[ p_var ] )

    enum opCodes {
        // [param] : immediate
        // $param  : value at reg param
        NOP           = 0x00, // No operation
        MV            = 0x01, // move $param2 to $param1
        BRANCH        = 0x02, // if $param1 > 0 set pc = $param2
        JUMP          = 0x03, // set pc = $param1
        ADD           = 0x04, // $param1 = $param2 + $param3
        CHECK_FLAG    = 0x05, // $param1 = checkFlag( $param2 )
        SET_FLAG      = 0x06, // setFlag( $param1, 1 )
        CLEAR_FLAG    = 0x07, // setFlag( $param1, 0 )
        MV_I          = 0x11, // set $param1 to [param2]
        BRANCH_I      = 0x12, // if $param1 > 0 set pc = [param2]
        JUMP_I        = 0x13, // set pc = [param1]
        ADD_I         = 0x14, // $param1 = $param2 + [param3]
        CHECK_FLAG_I  = 0x15, // $param1 = checkFlag( [param2] )
        SET_FLAG_I    = 0x16, // setFlag( [param1], 1 )
        CLEAR_FLAG_I  = 0x17, // setFlag( [param2], 0 )
        MSG           = 0x90, // Print string $param1. ($param2 is name if nonzero)
        ITEM          = 0x91, // Give player item $param1 $param2 times
        GIVE_PKMN     = 0x92, // Give player pkmn $paramA at level $paramB
        BATTLE_PKMN   = 0x93, // As GIVE_PKMN, but pkmn battles player
        MOVE_PLAYER   = 0x94, // Move player in dir $param1 <= $param2 steps at spd $param3
        MSG_I         = 0xA0, // Print string [paramA]. ([paramB] is name if nonzero)
        ITEM_I        = 0xA1, // Give player item [paramA] [paramB] times
        GIVE_PKMN_I   = 0xA2, // Give player pkmn [paramA] at level [paramB]
        BATTLE_PKMN_I = 0xA3, // As GIVE_PKMN, but pkmn battles player
        MOVE_PLAYER_I = 0xA4, // Move player in dir [param1] <= [param2] steps at spd [param3]
        HEAL_TEAM     = 0xE0, // Heal Pkmn team
        CHOOSE_PKMN   = 0xE1, // Choose Pkmn from team, store result (-1…5) in $param1
        GET_PKMN_STAT = 0xE2, // Store stats of pkmn $param1 starting at $param2
        CLEAR         = 0xFE, // Call IO::drawNav( true )
        EOP           = 0xFF  // Exit the script
    };
    bool mapDrawer::executeScript( u8 p_map, u16 p_globX, u16 p_globY, u8 p_z, u8 p_number,
                                   invocationType p_inv ) {
        FILE* sc = FS::openScript( warpPos{p_map, {p_globX, p_globY, p_z}}, p_number );
        if( !sc ) return false;

        u8 header[ 5 ];
        FS::read( sc, header, sizeof( u8 ), 4 );
        // Check if this script is really a script
        if( header[ 0 ] != scriptType::SCRIPT || header[ 3 ] == 0 ) return true;
        /*
                snprintf( buffer, 40, "%hhu %hhu %hhu %hhu", header[ 0 ], header[ 1 ], header[ 2 ],
                          header[ 3 ] );
                IO::messageBox( buffer, true );
                NAV::draw( );*/
        // Check if it can be executed
        if( header[ 2 ] != p_inv || ( header[ 1 ] < 0xFF && header[ 1 ] != p_z ) ) return true;

        FS::read( sc, SCRIPT_INS, sizeof( u32 ), std::min( (u8) MAX_SCRIPT_SIZE, header[ 3 ] ) );
        FS::close( sc );

        u8 pc = 0; // program counter

        while( pc < std::min( (u8) MAX_SCRIPT_SIZE, header[ 3 ] )
               && OPCODE( SCRIPT_INS[ pc ] ) != EOP ) {
            /*char buffer[ 50 ];
            snprintf( buffer, 40, "Op %lx\n(%lu, %lu, %lu)\n(%lu,%lu)", OPCODE( SCRIPT_INS[ pc ] ),
                      PARAM1( SCRIPT_INS[ pc ] ), PARAM2( SCRIPT_INS[ pc ] ),
                      PARAM3( SCRIPT_INS[ pc ] ), PARAMA( SCRIPT_INS[ pc ] ),
                      PARAMB( SCRIPT_INS[ pc ] ) );
            IO::messageBox( buffer, true );
            NAV::draw( );*/

            switch( OPCODE( SCRIPT_INS[ pc ] ) ) {
            case MV:
                if( PARAM1( SCRIPT_INS[ pc ] ) > 1 )
                    FETCH_UNSEC( PARAM1( SCRIPT_INS[ pc ] ) ) = FETCH( PARAM2( SCRIPT_INS[ pc ] ) );
                break;
            case MV_I:
                if( PARAM1( SCRIPT_INS[ pc ] ) > 1 )
                    FETCH_UNSEC( PARAM1( SCRIPT_INS[ pc ] ) ) = ( PARAM2( SCRIPT_INS[ pc ] ) );
                break;
            case ADD:
                if( PARAM1( SCRIPT_INS[ pc ] ) > 1 )
                    FETCH_UNSEC( PARAM1( SCRIPT_INS[ pc ] ) )
                        = FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) + FETCH( PARAM3( SCRIPT_INS[ pc ] ) );
                break;
            case ADD_I:
                if( PARAM1( SCRIPT_INS[ pc ] ) > 1 )
                    FETCH_UNSEC( PARAM1( SCRIPT_INS[ pc ] ) )
                        = FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) + ( PARAM3( SCRIPT_INS[ pc ] ) );
                break;

            case CHECK_FLAG:
                if( PARAM1( SCRIPT_INS[ pc ] ) > 1 )
                    FETCH_UNSEC( PARAM1( SCRIPT_INS[ pc ] ) )
                        = SAVE::SAV.getActiveFile( ).checkFlag(
                            FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) );
                break;
            case CHECK_FLAG_I:
                if( PARAM1( SCRIPT_INS[ pc ] ) > 1 )
                    FETCH_UNSEC( PARAM1( SCRIPT_INS[ pc ] ) )
                        = SAVE::SAV.getActiveFile( ).checkFlag( ( PARAM2( SCRIPT_INS[ pc ] ) ) );
                break;
            case SET_FLAG:
                SAVE::SAV.getActiveFile( ).setFlag( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ), 1 );
                break;
            case SET_FLAG_I:
                SAVE::SAV.getActiveFile( ).setFlag( ( PARAM1( SCRIPT_INS[ pc ] ) ), 1 );
                break;
            case CLEAR_FLAG:
                SAVE::SAV.getActiveFile( ).setFlag( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ), 0 );
                break;
            case CLEAR_FLAG_I:
                SAVE::SAV.getActiveFile( ).setFlag( ( PARAM1( SCRIPT_INS[ pc ] ) ), 0 );
                break;
            case BRANCH:
                if( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) ) pc = FETCH( PARAM2( SCRIPT_INS[ pc ] ) );
                break;
            case BRANCH_I:
                if( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) ) pc = ( PARAM2( SCRIPT_INS[ pc ] ) );
                break;
            case JUMP:
                pc = FETCH( PARAM2( SCRIPT_INS[ pc ] ) );
                break;
            case JUMP_I:
                pc = ( PARAM2( SCRIPT_INS[ pc ] ) );
                break;

            case MSG_I:
                if( !PARAMB( SCRIPT_INS[ pc ] ) )
                    IO::messageBox( GET_STRING( PARAMA( SCRIPT_INS[ pc ] ) ), true );
                else
                    IO::messageBox( GET_STRING( PARAMA( SCRIPT_INS[ pc ] ) ),
                                    GET_STRING( PARAMB( SCRIPT_INS[ pc ] ) ) );
                break;
            case MSG:
                if( !PARAMB( SCRIPT_INS[ pc ] ) )
                    IO::messageBox( GET_STRING( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) ), true );
                else
                    IO::messageBox( GET_STRING( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) ),
                                    GET_STRING( FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) ) );
                break;
            case ITEM_I: {
                ITEM::itemData itm = ITEM::getItemData( PARAMA( SCRIPT_INS[ pc ] ) );
                IO::messageBox( PARAMA( SCRIPT_INS[ pc ] ), itm, PARAMB( SCRIPT_INS[ pc ] ) );
                break;
            }
            case ITEM: {
                ITEM::itemData itm = ITEM::getItemData( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) );
                IO::messageBox( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ), itm,
                                FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) );
                break;
            }
            case GIVE_PKMN_I: {
                // pokemon pk = pokemon( PARAMA( SCRIPT_INS[ pc ] ), PARAMB( SCRIPT_INS[ pc ] ) );
                // TODO
                // SAVE::SAV.getActiveFile( ).givePkmn( pk );
                break;
            }
            case GIVE_PKMN: {
                // pokemon pk = pokemon( FETCH( PARAMA( SCRIPT_INS[ pc ] ) ),
                //                       FETCH( PARAMB( SCRIPT_INS[ pc ] ) ) );
                // TODO
                // SAVE::SAV.getActiveFile( ).givePkmn( pk );
                break;
            }
            case BATTLE_PKMN_I: {
                // pokemon pk = pokemon( PARAMA( SCRIPT_INS[ pc ] ), PARAMB( SCRIPT_INS[ pc ] ) );
                // TODO
                break;
            }
            case BATTLE_PKMN: {
                // pokemon pk = pokemon( FETCH( PARAMA( SCRIPT_INS[ pc ] ) ),
                //                       FETCH( PARAMB( SCRIPT_INS[ pc ] ) ) );
                // TODO
                break;
            }

            case MOVE_PLAYER_I:
                for( u8 i = 0; i < PARAM2( SCRIPT_INS[ pc ] ); ++i )
                    movePlayer( (MAP::direction) PARAM1( SCRIPT_INS[ pc ] ),
                                PARAM3( SCRIPT_INS[ pc ] ) );
                break;
            case MOVE_PLAYER:
                for( u8 i = 0; i < FETCH( PARAM2( SCRIPT_INS[ pc ] ) ); ++i )
                    movePlayer( (MAP::direction) FETCH( PARAM1( SCRIPT_INS[ pc ] ) ),
                                FETCH( PARAM3( SCRIPT_INS[ pc ] ) ) );
                break;

            case HEAL_TEAM:
                for( u8 i = 0; i < 6; ++i )
                    if( SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                        SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].heal( );
                break;
            case CHOOSE_PKMN:
                // TODO
                break;
            case GET_PKMN_STAT:
                // TODO
                break;
            case CLEAR:
                NAV::draw( true );
                break;
            default:
                break;
            }
            ++pc;
        }
        return true;
    }

    // At most one warp is allowed per block.
    // The corresponding script must have id 0.
    warpPos getWarpPos( warpPos p_source ) {
        FILE* sc = FS::openScript( p_source, 0 );
        if( !sc ) return warpPos{0, {0, 0, 0}};

        u8 header[ 5 ], content[ 7 ];
        FS::read( sc, header, sizeof( u8 ), 4 );
        if( header[ 0 ] != scriptType::WARP_SCRIPT || header[ 2 ] != invocationType::WARP_TILE ) {
            FS::close( sc );
            return warpPos{0, {0, 0, 0}};
        }

        if( header[ 3 ] == 1 ) { // warp specified as local coordinates
            FS::read( sc, content, sizeof( u8 ), 6 );
            FS::close( sc );
            return warpPos{content[ 0 ],
                           {( u16 )( content[ 2 ] * SIZE + content[ 3 ] ),
                            ( u16 )( content[ 1 ] * SIZE + content[ 4 ] ), content[ 5 ]}};
        } else if( header[ 3 ] == 0 ) { // warp specified as global coordinates
            warpPos res;
            FS::read( sc, &res, sizeof( warpPos ), 1 );
            FS::close( sc );
            return res;
        } else {
            FS::close( sc );
            return warpPos{0, {0, 0, 0}};
        }
    }

    void mapDrawer::interact( ) {
        u16  px = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  py = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16  pz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        auto d  = SAVE::SAV.getActiveFile( ).m_player.m_direction;
        handleEvents( px, py, pz, d );
    }

    void mapDrawer::handleEvents( u16 p_globX, u16 p_globY, u8 p_z ) {
        u8 map = SAVE::SAV.getActiveFile( ).m_currentMap;
        u8 i   = 0;
        while( executeScript( map, p_globX, p_globY, p_z, i, invocationType::STEP_ONTO ) ) ++i;
    }

    void mapDrawer::handleEvents( u16 p_globX, u16 p_globY, u8 p_z, direction p_dir ) {
        p_globX += dir[ p_dir ][ 0 ];
        p_globY += dir[ p_dir ][ 1 ];
        u8 map = SAVE::SAV.getActiveFile( ).m_currentMap;
        u8 i   = 0;
        while( executeScript( map, p_globX, p_globY, p_z, i, invocationType::INTERACT ) ) ++i;
    }

    void mapDrawer::handleWarp( warpType p_type, warpPos p_source ) {
        warpPos tg = getWarpPos( p_source );

        if( tg.first == 0xFF ) tg = SAVE::SAV.getActiveFile( ).m_lastWarp;
        if( !tg.first && !tg.second.m_posY && !tg.second.m_posZ && !tg.second.m_posX ) return;

        SAVE::SAV.getActiveFile( ).m_lastWarp = p_source;
        warpPlayer( p_type, tg );
    }
}
