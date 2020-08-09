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
#include "boxViewer.h"
#include "defines.h"
#include "fs.h"
#include "mapDrawer.h"
#include "nav.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sound.h"
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

    std::string parseLogCmd( const std::string& p_cmd ) {
        if( p_cmd == "PLAYER" ) { return SAVE::SAV.getActiveFile( ).m_playername; }
        if( p_cmd == "RIVAL" ) {
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                return GET_STRING( 461 );
            } else {
                return GET_STRING( 460 );
            }
        }

        return "";
    }

    void printMapMessage( const std::string& p_text, style p_style ) {
        std::string res = "";

        for( size_t i = 0; i < p_text.size( ); ++i ) {
            if( p_text[ i ] == '[' ) {
                std::string accmd = "";
                while( p_text[ ++i ] != ']' ) { accmd += p_text[ i ]; }
                res += parseLogCmd( accmd );
                continue;
            } else if( p_text[ i ] == '\r' ) {
                NAV::printMessage( res.c_str( ), p_style );
                res = "";
                continue;
            }
            res += p_text[ i ];
        }
        NAV::printMessage( res.c_str( ), p_style );
    }

#ifdef false
    bool mapDrawer::executeScript( u8 p_map, u8 p_globX, u8 p_globY, u8 p_z, u8 p_number,
                                   invocationType p_inv ) {
        FILE* sc = FS::openScript( warpPos{ p_map, { p_globX, p_globY, p_z } }, p_number );
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
            case JUMP: pc = FETCH( PARAM2( SCRIPT_INS[ pc ] ) ); break;
            case JUMP_I: pc = ( PARAM2( SCRIPT_INS[ pc ] ) ); break;

            case MSG_I:
                /*
                if( !PARAMB( SCRIPT_INS[ pc ] ) )
                    IO::messageBox( GET_STRING( PARAMA( SCRIPT_INS[ pc ] ) ), true );
                else
                    IO::messageBox( GET_STRING( PARAMA( SCRIPT_INS[ pc ] ) ),
                                    GET_STRING( PARAMB( SCRIPT_INS[ pc ] ) ) );
                                    */
                break;
            case MSG:
                /*
                if( !PARAMB( SCRIPT_INS[ pc ] ) )
                    IO::messageBox( GET_STRING( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) ), true );
                else
                    IO::messageBox( GET_STRING( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) ),
                                    GET_STRING( FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) ) );
                                    */
                break;
            case ITEM_I: {
                /*
   ITEM::itemData itm = ITEM::getItemData( PARAMA( SCRIPT_INS[ pc ] ) );
   IO::messageBox( PARAMA( SCRIPT_INS[ pc ] ), itm, PARAMB( SCRIPT_INS[ pc ] ) );
   */
                break;
            }
            case ITEM: {
                /*
     ITEM::itemData itm = ITEM::getItemData( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ) );
     IO::messageBox( FETCH( PARAM1( SCRIPT_INS[ pc ] ) ), itm,
                     FETCH( PARAM2( SCRIPT_INS[ pc ] ) ) );
                     */
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
            case CLEAR: break;
            default: break;
            }
            ++pc;
        }
        return true;
    }
#endif

    void mapDrawer::interact( ) {
        u16  px = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  py = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16  pz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        auto d  = SAVE::SAV.getActiveFile( ).m_player.m_direction;
        // Check for special blocks

        u16 tx     = px + dir[ d ][ 0 ];
        u16 ty     = py + dir[ d ][ 1 ];
        u8  behave = at( tx, ty ).m_bottombehave;

        switch( behave ) {
        case 0x83: { // PC
            SOUND::playSoundEffect( SFX_PC_OPEN );
            atom( tx, ty ).m_blockidx = 0x5;
            loadBlock( at( tx, ty ), ( _lastcol + NUM_COLS / 2 ) % NUM_COLS,
                       ( _lastrow + NUM_ROWS / 2 ) % NUM_ROWS );
            bgUpdate( );
            for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }
            NAV::printMessage( GET_STRING( 559 ), MSG_INFO );

            BOX::boxViewer bxv;
            ANIMATE_MAP = false;
            videoSetMode( MODE_5_2D );
            bgUpdate( );
            SOUND::dimVolume( );

            bxv.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            IO::initVideoSub( );
            IO::resetScale( true, false );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            NAV::init( );
            draw( );

            SOUND::playSoundEffect( SFX_PC_CLOSE );
            atom( tx, ty ).m_blockidx = 0x4;
            loadBlock( at( tx, ty ), ( _lastcol + NUM_COLS / 2 ) % NUM_COLS,
                       ( _lastrow + NUM_ROWS / 2 ) % NUM_ROWS );
            bgUpdate( );
            for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

            return;
        }
        case 0x85: { // Map
            // TODO
            NAV::printMessage( GET_STRING( 560 ), MSG_NORMAL );
            break;
        }
        case 0x80: { // load script one block behind
            handleEvents( tx, ty, pz, d );
            return;
        }
            [[likely]] default : break;
        }
        handleEvents( px, py, pz, d );
    }

    void mapDrawer::runEvent( mapData::event p_event ) {
        switch( p_event.m_type ) {
        case EVENT_MESSAGE:
            printMapMessage( GET_MAP_STRING( p_event.m_data.m_message.m_msgId ),
                             (style) p_event.m_data.m_message.m_msgType );
            break;
        case EVENT_NPC_MESSAGE:
            printMapMessage( GET_MAP_STRING( p_event.m_data.m_npc.m_scriptId ),
                             ( style )( p_event.m_data.m_npc.m_scriptType & 127 ) );
            if( p_event.m_data.m_npc.m_scriptType & 128 ) {
                SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, true );
            }
            break;
        case EVENT_TRAINER: {
            auto tr = BATTLE::getBattleTrainer( p_event.m_data.m_trainer.m_trainerId );
            if( !SAVE::SAV.getActiveFile( ).checkFlag(
                    SAVE::F_TRAINER_BATTLED( p_event.m_deactivateFlag ) ) ) {
                // player did not defeat the trainer yet

                NAV::printMessage( tr.m_strings.m_message1, MSG_NORMAL );

                auto playerPrio = OBJPRIORITY_0;
                //_sprites[ _spritePos[ SAVE::SAV.getActiveFile( ).m_player.m_id ] ]
                //                  .getPriority( );
                SOUND::playBGM( SOUND::BGMforTrainerBattle( tr.m_data.m_trainerClass ) );
                FADE_TOP_DARK( );
                ANIMATE_MAP = false;
                swiWaitForVBlank( );

                BATTLE::battle bt
                    = BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                      SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), tr );
                if( bt.start( ) == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    faintPlayer( );
                } else {
                    SAVE::SAV.getActiveFile( ).setFlag(
                        SAVE::F_TRAINER_BATTLED( p_event.m_deactivateFlag ), true );
                }
                FADE_TOP_DARK( );
                NAV::init( );
                draw( playerPrio );
                SOUND::restartBGM( );
                ANIMATE_MAP = true;
            }

            NAV::printMessage( tr.m_strings.m_message2, MSG_NORMAL );
            break;
        }
        case EVENT_HMOBJECT: {
            switch( p_event.m_data.m_hmObject.m_hmType ) {
            case mapSpriteManager::SPR_STRENGTH:
                if( _strengthUsed ) {
                    NAV::printMessage( GET_STRING( 558 ), MSG_NORMAL );
                } else {
                    NAV::printMessage( GET_STRING( 308 ), MSG_NORMAL );
                }
                break;
            case mapSpriteManager::SPR_ROCKSMASH:
                NAV::printMessage( GET_STRING( 314 ), MSG_NORMAL );
                break;
            case mapSpriteManager::SPR_CUT:
                NAV::printMessage( GET_STRING( 313 ), MSG_NORMAL );
                break;
            default: break;
            }
            break;
        }
        default: break;
        }
    }

    void mapDrawer::handleEvents( u8 p_globX, u8 p_globY, u8 p_z ) {
        u8 x = p_globX % SIZE;
        u8 y = p_globY % SIZE;
        u8 z = p_z;

        auto mdata = currentData( );

        for( u8 i = 0; i < mdata.m_eventCount; ++i ) {
            if( mdata.m_events[ i ].m_posX != x || mdata.m_events[ i ].m_posY != y
                || mdata.m_events[ i ].m_posZ != z ) {
                continue;
            }
            if( mdata.m_events[ i ].m_activateFlag
                && !SAVE::SAV.getActiveFile( ).checkFlag( mdata.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( mdata.m_events[ i ].m_deactivateFlag
                && SAVE::SAV.getActiveFile( ).checkFlag( mdata.m_events[ i ].m_deactivateFlag ) ) {
                continue;
            }
            if( mdata.m_events[ i ].m_type != EVENT_MESSAGE
                && mdata.m_events[ i ].m_type != EVENT_GENERIC ) {
                // These events have associated map objects
                continue;
            }
            if( mdata.m_events[ i ].m_trigger == TRIGGER_STEP_ON ) {
                runEvent( mdata.m_events[ i ] );
            }
        }
    }

    void mapDrawer::handleEvents( u16 p_globX, u16 p_globY, u8 p_z, direction p_dir ) {
        p_globX += dir[ p_dir ][ 0 ];
        p_globY += dir[ p_dir ][ 1 ];

        u8 x = p_globX % SIZE;
        u8 y = p_globY % SIZE;
        u8 z = p_z;

        auto mdata = currentData( p_globX, p_globY );
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.second.m_pos.m_posX != p_globX || o.second.m_pos.m_posY != p_globY
                || o.second.m_pos.m_posZ != z ) {
                continue;
            }

            runEvent( o.second.m_event );
        }
        for( u8 i = 0; i < mdata.m_eventCount; ++i ) {
            if( mdata.m_events[ i ].m_posX != x || mdata.m_events[ i ].m_posY != y
                || mdata.m_events[ i ].m_posZ != z ) {
                continue;
            }
            if( mdata.m_events[ i ].m_activateFlag
                && !SAVE::SAV.getActiveFile( ).checkFlag( mdata.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( mdata.m_events[ i ].m_type != EVENT_TRAINER && mdata.m_events[ i ].m_deactivateFlag
                && SAVE::SAV.getActiveFile( ).checkFlag( mdata.m_events[ i ].m_deactivateFlag ) ) {
                continue;
            }

            if( mdata.m_events[ i ].m_type != EVENT_MESSAGE
                && mdata.m_events[ i ].m_type != EVENT_GENERIC ) {
                // These events have associated map objects
                continue;
            }

            if( mdata.m_events[ i ].m_trigger & dirToEventTrigger( p_dir ) ) {
                runEvent( mdata.m_events[ i ] );
            }
        }
    }

    bool mapData::hasEvent( eventType p_type, u8 p_x, u8 p_y, u8 p_z ) const {
        for( u8 i = 0; i < m_eventCount; ++i ) {
            if( m_events[ i ].m_posX == p_x && m_events[ i ].m_posY == p_y
                && m_events[ i ].m_posZ == p_z && m_events[ i ].m_type == p_type ) {
                if( m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag( m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag( m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                return true;
            }
        }
        return false;
    }

} // namespace MAP
