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
#define MAX_SCRIPT_SIZE 128
    // opcode : u8, param1 : u8, param2 : u8, param3 : u8
    // opcode : u8, paramA : u12, paramB : u12
    u32 SCRIPT_INS[ MAX_SCRIPT_SIZE ];
#define OPCODE( p_ins ) ( ( p_ins ) >> 24 )
#define PARAM1( p_ins ) ( ( ( p_ins ) >> 16 ) & 0xFF )
#define PARAM2( p_ins ) ( ( ( p_ins ) >> 8 ) & 0xFF )
#define PARAM3( p_ins ) ( (p_ins) &0xFF )
#define PARAMA( p_ins ) ( ( ( p_ins ) >> 12 ) & 0xFFF )
#define PARAMB( p_ins ) ( (p_ins) &0xFFF )

    // special functions
    // 1 heal entire team
    // 2 run constructed poke mart
    // 3 get badge count (param1: region [0: Hoenn, 1: battle frontier])
    //

    enum opCode : u8 {
        EOP = 0,  // end of program
        SMO = 1,  // set map object
        MMO = 2,  // move map object
        DMO = 3,  // destroy map object
        CFL = 4,  // check flag
        SFL = 5,  // set flag
        CRG = 6,  // check register
        SRG = 7,  // set register
        MRG = 8,  // move register value
        JMP = 9,  // jump
        JMB = 10, // jump backwards

        SMOR = 11, // set map object
        MMOR = 12, // move map object
        DMOR = 13, // destroy map object
        CFLR = 14, // check flag
        SFLR = 15, // set flag

        CRGL = 16, // check register lower
        CRGG = 17, // check register greater
        CRGN = 18, // check register not equal
        MPL  = 19, // move player
        CMO  = 20, // current map object id to reg 0

        ATT = 90, // Attach player
        REM = 91, // Remove player

        BTR = 100, // Battle trainer

        PMO = 117, // Play music oneshot
        SMC = 118, // Set music
        SLC = 119, // Set location
        SWT = 120, // Set weather
        WAT = 121, // Wait
        MBG = 122, // Pokemart description begin
        MIT = 123, // Mart item

        YNM = 125, // yes no message
        CLL = 126, // Call special function
        MSG = 127, // message
    };

    std::string parseLogCmd( const std::string& p_cmd ) {
        u16 tmp = 0;

        if( p_cmd == "PLAYER" ) { return SAVE::SAV.getActiveFile( ).m_playername; }
        if( p_cmd == "RIVAL" ) {
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                return GET_STRING( 461 );
            } else {
                return GET_STRING( 460 );
            }
        }
        if( sscanf( p_cmd.c_str( ), "CRY:%hu", &tmp ) && tmp ) {
            SOUND::playCry( tmp );
            return "";
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
                if( p_style == MSG_NORMAL ) {
                    NAV::printMessage( res.c_str( ), MSG_NORMAL_CONT );
                } else if( p_style == MSG_INFO ) {
                    NAV::printMessage( res.c_str( ), MSG_INFO_CONT );
                } else {
                    NAV::printMessage( res.c_str( ), p_style );
                }
                res = "";
                continue;
            }
            res += p_text[ i ];
        }
        NAV::printMessage( res.c_str( ), p_style );
    }

    void mapDrawer::executeScript( u16 p_scriptId, u8 p_mapObject ) {
        FILE* f = FS::openScript( p_scriptId );
        if( !f ) { return; }
        fread( SCRIPT_INS, sizeof( u32 ), MAX_SCRIPT_SIZE, f );
        FS::close( f );
        auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ p_mapObject ];

        u8 pc = 0;

        std::vector<std::pair<u16, u32>> martItems;

        u16 registers[ 10 ] = { 0 };

        u8   pmartCurr = 0;
        bool martSell  = true;

        bool playerAttachedToObject = false;

        while( SCRIPT_INS[ pc ] ) {
            u16  curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
            u16  cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
            u16  mapX = curx / SIZE, mapY = cury / SIZE;
            auto ins  = opCode( OPCODE( SCRIPT_INS[ pc ] ) );
            u8   par1 = PARAM1( SCRIPT_INS[ pc ] );
            u8   par2 = PARAM2( SCRIPT_INS[ pc ] );
            u8   par3 = PARAM3( SCRIPT_INS[ pc ] );

#ifdef DESQUID_MORE
            NAV::printMessage( ( std::to_string( ins ) + " ( " + std::to_string( par1 ) + " , "
                                 + std::to_string( par2 ) + " , " + std::to_string( par3 ) + ")" )
                                   .c_str( ) );
#endif
            u16 parA = PARAMA( SCRIPT_INS[ pc ] );
            u16 parB = PARAMB( SCRIPT_INS[ pc ] );

            switch( ins ) {
            case ATT: playerAttachedToObject = true; break;
            case REM: playerAttachedToObject = false; break;
            case EOP: return;
            case SMO: {
                mapObject obj         = mapObject( );
                obj.m_pos             = { u16( mapX * SIZE + par2 ), u16( mapY * SIZE + par3 ), 3 };
                obj.m_picNum          = par1;
                obj.m_movement        = NO_MOVEMENT;
                obj.m_range           = 0;
                obj.m_direction       = DOWN;
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                std::pair<u8, mapObject> cur = { 0, obj };
                loadMapObject( cur );

                // Check if there is some unused map object

                u8 found = 255;
                for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                    if( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].first == UNUSED_MAPOBJECT ) {
                        found = i;
                        break;
                    }
                }

                if( found < 255 ) {
                    registers[ 0 ] = found;
                } else {
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_mapObjectCount++;
                }
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ 0 ] ] = cur;
                break;
            }
            case MPL: {
                redirectPlayer( direction( par2 ), false );
                for( u8 j = 0; j < par3; ++j ) { movePlayer( direction( par2 ) ); }
                break;
            }
            case MMO: {
                movement m = { direction( par2 ), 0 };
                _mapSprites.setFrame( par1, getFrame( direction( par2 ) ) );

                for( u8 j = 0; j < par3; ++j ) {
                    for( u8 i = 0; i < 16; ++i ) {
                        moveMapObject( par1, m, playerAttachedToObject,
                                       SAVE::SAV.getActiveFile( ).m_player.m_direction );
                        m.m_frame = ( m.m_frame + 1 ) & 15;
                        swiWaitForVBlank( );
                    }
                    if( playerAttachedToObject ) {
                        SAVE::SAV.getActiveFile( ).m_player.m_direction = direction( par2 );
                    }
                }
                break;
            }
            case DMO: {
                _mapSprites.destroySprite( SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].first );
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ]
                    = { UNUSED_MAPOBJECT, mapObject( ) };
                break;
            }
            case CFL: {
                if( SAVE::SAV.getActiveFile( ).checkFlag( par1 ) == par2 ) { pc += par3; }
                break;
            }
            case SFL: {
                SAVE::SAV.getActiveFile( ).setFlag( par1, par2 );
                break;
            }
            case CRG: {
                if( registers[ par1 ] == par2 ) { pc += par3; }
                break;
            }
            case SRG: registers[ par1 ] = par2; break;
            case MRG: registers[ par2 ] = registers[ par1 ]; break;
            case JMP:
                if( pc + par1 < MAX_SCRIPT_SIZE ) {
                    pc += par1;
                } else {
                    return;
                }
                break;
            case JMB:
                if( par1 > pc ) {
                    pc = 0;
                } else {
                    pc -= par1;
                }
                break;
            case SMOR: {
                mapObject obj         = mapObject( );
                obj.m_pos             = { u16( mapX * SIZE + par2 ), u16( mapY * SIZE + par3 ), 3 };
                obj.m_picNum          = registers[ par1 ];
                obj.m_movement        = NO_MOVEMENT;
                obj.m_range           = 0;
                obj.m_direction       = DOWN;
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                std::pair<u8, mapObject> cur = { 0, obj };
                loadMapObject( cur );

                // Check if there is some unused map object

                u8 found = 255;
                for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                    if( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].first == UNUSED_MAPOBJECT ) {
                        found = i;
                        break;
                    }
                }

                if( found < 255 ) {
                    registers[ 0 ] = found;
                } else {
                    NAV::printMessage( "HERE" );
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_mapObjectCount++;
                }
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ 0 ] ] = cur;
                break;
            }
            case MMOR: {
#ifdef DESQUID_MORE
                NAV::printMessage(
                    ( std::to_string(
                          SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first )
                      + " ( " + std::to_string( registers[ par1 ] ) + " , " + std::to_string( par2 )
                      + " , " + std::to_string( par3 ) + ")" )
                        .c_str( ) );
#endif
                movement m = { direction( par2 ), 0 };
                _mapSprites.setFrame(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first,
                    getFrame( direction( par2 ) ) );

                for( u8 j = 0; j < par3; ++j ) {
                    for( u8 i = 0; i < 16; ++i ) {
                        moveMapObject( registers[ par1 ], m, playerAttachedToObject,
                                       SAVE::SAV.getActiveFile( ).m_player.m_direction );
                        m.m_frame = ( m.m_frame + 1 ) & 15;
                        swiWaitForVBlank( );
                    }

                    if( playerAttachedToObject ) {
                        SAVE::SAV.getActiveFile( ).m_player.m_direction = direction( par2 );
                    }
                }
                break;
            }
            case DMOR: {
                _mapSprites.destroySprite(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first );
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ]
                    = { UNUSED_MAPOBJECT, mapObject( ) };
                break;
            }
            case CFLR: {
                if( SAVE::SAV.getActiveFile( ).checkFlag( par1 ) == registers[ par2 ] ) {
                    pc += par3;
                }
                break;
            }
            case SFLR: {
                SAVE::SAV.getActiveFile( ).setFlag( par1, registers[ par2 ] );
                break;
            }

            case CRGL:
                if( registers[ par1 ] < par2 ) { pc += par3; }
                break;
            case CRGG:
                if( registers[ par1 ] > par2 ) { pc += par3; }
                break;
            case CRGN:
                if( registers[ par1 ] != par2 ) { pc += par3; }
                break;
            case CMO: registers[ 0 ] = o.first; break;
            case BTR:
                // TODO
                break;
            case PMO: {
                SOUND::playBGMOneshot( parA );
                for( u16 i = 0; i < parB; ++i ) { swiWaitForVBlank( ); }
                SOUND::restartBGM( );
                break;
            }
            case SMC:
                // TODO
                break;
            case SLC:
                // TODO
                break;
            case SWT:
                // TODO
                break;
            case WAT:
                for( u8 i = 0; i < parA; ++i ) { swiWaitForVBlank( ); }
                break;
            case MBG:
                pmartCurr = par1;
                martSell  = par2;
                martItems.clear( );
                break;
            case MIT: martItems.push_back( { parA, parB } ); break;
            case CLL:
                switch( par1 ) {
                case 1: { // heal pkmn team
                    for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
                        SAVE::SAV.getActiveFile( ).getTeamPkmn( i )->heal( );
                    }
                    break;
                }
                case 2: { // run pokemart
                    runPokeMart( martItems, 0, martSell, pmartCurr );
                    break;
                }
                case 3: {
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).getBadgeCount( par1 );
                    break;
                }

                default: break;
                }
                break;
            case YNM: {
                registers[ 0 ]
                    = IO::yesNoBox::YES
                      == IO::yesNoBox( ).getResult( GET_MAP_STRING( parA ), (style) parB );
                NAV::init( );
                break;
            }
            case MSG: printMapMessage( GET_MAP_STRING( parA ), (style) parB ); break;
            default: break;
            }
            ++pc;
        }
    }

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

    void mapDrawer::runEvent( mapData::event p_event, u8 p_objectId ) {
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
                    return;
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
        case EVENT_NPC: {
            executeScript( p_event.m_data.m_npc.m_scriptId, p_objectId );
            break;
        }
        case EVENT_GENERIC: {
            executeScript( p_event.m_data.m_generic.m_scriptId );
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
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.second.m_pos.m_posX != p_globX || o.second.m_pos.m_posY != p_globY
                || o.second.m_pos.m_posZ != z ) {
                continue;
            }

            auto old = o.second.m_movement;
            // rotate sprite to player
            if( ( o.second.m_picNum & 0xff ) <= 240 ) {
                o.second.m_movement = NO_MOVEMENT;
                _mapSprites.setFrame( o.first, getFrame( direction( ( u8( p_dir ) + 2 ) % 4 ) ) );
            }

            runEvent( o.second.m_event, i );
            o.second.m_movement = old;
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
