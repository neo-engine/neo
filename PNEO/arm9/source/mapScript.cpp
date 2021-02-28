/*
Pokémon neo
------------------------------

file        : mapScript.cpp
author      : Philip Wellnitz
description : Map script engine

Copyright (C) 2012 - 2021
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

#include "bagViewer.h"
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
#include "specials.h"
#include "sprite.h"
#include "uio.h"

namespace MAP {
#define MAX_SCRIPT_SIZE 128
    // opcode : u8, param1 : u8, param2 : u8, param3 : u8
    // opcode : u8, paramA : u12, paramB : u12
#define OPCODE( p_ins )  ( ( p_ins ) >> 24 )
#define PARAM1( p_ins )  ( ( ( p_ins ) >> 16 ) & 0xFF )
#define PARAM2( p_ins )  ( ( ( p_ins ) >> 8 ) & 0xFF )
#define PARAM3( p_ins )  ( ( p_ins ) & ( 0xFF ) )
#define PARAM1S( p_ins ) ( ( ( p_ins ) >> 19 ) & 0x1F )
#define PARAM2S( p_ins ) ( ( ( p_ins ) >> 14 ) & 0x1F )
#define PARAM3S( p_ins ) ( ( p_ins ) & ( 0x3FFF ) )
#define PARAMA( p_ins )  ( ( ( p_ins ) >> 12 ) & 0xFFF )
#define PARAMB( p_ins )  ( ( p_ins ) & ( 0xFFF ) )

    // special functions
    // 1 heal entire team
    // 2 run constructed poke mart
    // 3 get badge count (param1: region [0: Hoenn, 1: battle frontier])
    // 4 get init game item count
    // 5 get (and remove) init game item par1
    // 6 Initial pkmn selection
    // 7 NAV::init( )
    // 8 Catching tutorial
    // 9 award badge (p1: region, p2: badge)

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
        GMO  = 21, // Get map object id of object at specified position to reg 0

        CPP = 22, // check player position

        LCKR = 23, // Lock map object
        ULKR = 24, // unlock map object

        BNK = 25, // warp player (bank, z) (needs to be before WRP)
        WRP = 26, // warp player (x, y)

        CVR  = 27, // check variable equal
        CVRN = 28, // check variable not equal
        CVRG = 29, // check variable greater
        CVRL = 30, // check variable lower

        MFO  = 31, // move map object fast
        MFOR = 32, // move map object fast

        GIT = 33, // return how many copies the player has of the specified item

        STF = 34, // set trainer flag
        CTF = 35, // check trainer flag

        EXM  = 87, // Exclamation mark
        EXMR = 88, // Exclamation mark (register)
        RDR  = 89, // Redraw objects
        ATT  = 90, // Attach player
        REM  = 91, // Remove player

        FNT = 99,  // faint player
        BTR = 100, // Battle trainer
        BPK = 101, // Battle pkmn
        ITM = 102, // Give item

        BTRR = 105, // Battle trainer
        BPKR = 106, // Battle pkmn
        ITMR = 107, // Give item

        MSC = 113, // play music (temporary)
        RMS = 114, // Reset music
        CRY = 115, // Play cry
        SFX = 116, // Play sound effect
        PMO = 117, // Play music oneshot
        SMC = 118, // Set music
        SLC = 119, // Set location
        SWT = 120, // Set weather to parA
        WAT = 121, // Wait
        MBG = 122, // Pokemart description begin
        MIT = 123, // Mart item

        YNM = 125, // yes no message
        CLL = 126, // Call special function
        MSG = 127, // message

        SBC = 196, // set block
    };

    std::string parseLogCmd( const std::string& p_cmd ) {
        u16 tmp = -1;

        if( p_cmd == "PLAYER" ) { return SAVE::SAV.getActiveFile( ).m_playername; }
        if( p_cmd == "RIVAL" ) {
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                return GET_STRING( 461 );
            } else {
                return GET_STRING( 460 );
            }
        }
        if( sscanf( p_cmd.c_str( ), "CRY:%hu", &tmp ) && tmp != u16( -1 ) ) {
            SOUND::playCry( tmp );
            return "";
        }
        if( sscanf( p_cmd.c_str( ), "TEAM:%hu", &tmp ) && tmp != u16( -1 ) ) {
            return getDisplayName( SAVE::SAV.getActiveFile( ).getTeamPkmn( tmp )->getSpecies( ) );
        }
        return std::string( "[" ) + p_cmd + "]";
    }

    std::string convertMapString( const std::string& p_text, style p_style ) {
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
        return res;
    }

    void printMapMessage( const std::string& p_text, style p_style ) {
        if( p_style == MSG_SIGN ) {
            // TODO: properly implement signs
            p_style = MSG_INFO;
        }
        NAV::printMessage( convertMapString( p_text, p_style ).c_str( ), p_style );
    }

    void printMapYNMessage( const std::string& p_text, style p_style ) {

        if( p_style == MSG_SIGN ) {
            // TODO: properly implement signs
            p_style = MSG_INFO;
        }
        NAV::printMessage( convertMapString( p_text, p_style ).c_str( ), p_style );
    }

    static u16 CURRENT_SCRIPT = -1;
    void       mapDrawer::executeScript( u16 p_scriptId, u8 p_mapObject, s16 p_mapX, s16 p_mapY ) {
        if( CURRENT_SCRIPT == p_scriptId ) { return; }
        CURRENT_SCRIPT = p_scriptId;

        FILE* f = FS::openScript( p_scriptId );
        if( !f ) {
            CURRENT_SCRIPT = -1;
            return;
        }
        u32 SCRIPT_INS[ MAX_SCRIPT_SIZE ];
        fread( SCRIPT_INS, sizeof( u32 ), MAX_SCRIPT_SIZE, f );
        FS::close( f );

        u8 pc = 0;

        std::vector<std::pair<u16, u32>> martItems;

        u16 registers[ 10 ] = { 0 };

        u8   pmartCurr = 0;
        bool martSell  = true;

        bool playerAttachedToObject = false;

        moveMode tmpmove = NO_MOVEMENT;

        u8 newbnk = 255;
        u8 newz   = 0;

        while( SCRIPT_INS[ pc ] ) {
            u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
            u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
            u16 curz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
            u16 mapX = curx / SIZE, mapY = cury / SIZE;
            if( p_mapX >= 0 ) { mapX = p_mapX; }
            if( p_mapY >= 0 ) { mapY = p_mapY; }
            auto ins   = opCode( OPCODE( SCRIPT_INS[ pc ] ) );
            u8   par1  = PARAM1( SCRIPT_INS[ pc ] );
            u8   par2  = PARAM2( SCRIPT_INS[ pc ] );
            u8   par3  = PARAM3( SCRIPT_INS[ pc ] );
            u8   par1s = PARAM1S( SCRIPT_INS[ pc ] );
            u8   par2s = PARAM2S( SCRIPT_INS[ pc ] );
            u16  par3s = PARAM3S( SCRIPT_INS[ pc ] );
            u16  parA  = PARAMA( SCRIPT_INS[ pc ] );
            u16  parB  = PARAMB( SCRIPT_INS[ pc ] );

#ifdef DESQUID_MORE
            NAV::printMessage( ( std::to_string( pc ) + ": " + std::to_string( ins ) + " ( "
                                 + std::to_string( par1 ) + " , " + std::to_string( par2 ) + " , "
                                 + std::to_string( par3 ) + ")" )
                                   .c_str( ) );
#endif

            switch( ins ) {
            case BNK: {
                newbnk = par1;
                newz   = par2;
                break;
            }
            case WRP: {
                warpPlayer( NO_SPECIAL, { newbnk, { parA, parB, newz } } );
                break;
            }
            case RDR: {
                constructAndAddNewMapObjects( currentData( ), mapX, mapY );
                break;
            }
            case EXM: {
                showExclamationAboveMapObject( par1 );
                break;
            }
            case EXMR: {
                showExclamationAboveMapObject( registers[ par1 ] );
                break;
            }
            case ATT: playerAttachedToObject = true; break;
            case REM: playerAttachedToObject = false; break;
            case EOP: {
                CURRENT_SCRIPT = -1;
                return;
            }
            case GIT: {
                auto idata     = ITEM::getItemData( parA );
                registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_bag.count(
                    BAG::toBagType( idata.m_itemType ), parA );
                break;
            }
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

                auto tmp = SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement;
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement = NO_MOVEMENT;

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
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement = tmp;
                break;
            }
            case MFO: {
                movement m = { direction( par2 ), 0 };
                _mapSprites.setFrame( par1, getFrame( direction( par2 ) ) );

                auto tmp = SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement;
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement = NO_MOVEMENT;

                for( u8 j = 0; j < par3; ++j ) {
                    for( u8 i = 0; i < 16; ++i ) {
                        moveMapObject( par1, m, playerAttachedToObject,
                                       SAVE::SAV.getActiveFile( ).m_player.m_direction );
                        m.m_frame = ( m.m_frame + 1 ) & 15;
                        if( i & 1 ) { swiWaitForVBlank( ); }
                    }
                    if( playerAttachedToObject ) {
                        SAVE::SAV.getActiveFile( ).m_player.m_direction = direction( par2 );
                    }
                }
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement = tmp;
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
            case STF: {
                SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_TRAINER_BATTLED( par1 ), par2 );
                break;
            }
            case CTF: {
                if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED( par1 ) )
                    == par2 ) {
                    pc += par3;
                }
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
                    CURRENT_SCRIPT = -1;
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
                auto tmp = SAVE::SAV.getActiveFile( )
                               .m_mapObjects[ registers[ par1 ] ]
                               .second.m_movement;
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = NO_MOVEMENT;
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
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = tmp;
                break;
            }
            case MFOR: {
#ifdef DESQUID_MORE
                NAV::printMessage(
                    ( std::to_string(
                          SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first )
                      + " ( " + std::to_string( registers[ par1 ] ) + " , " + std::to_string( par2 )
                      + " , " + std::to_string( par3 ) + ")" )
                        .c_str( ) );
#endif
                auto tmp = SAVE::SAV.getActiveFile( )
                               .m_mapObjects[ registers[ par1 ] ]
                               .second.m_movement;
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = NO_MOVEMENT;
                movement m = { direction( par2 ), 0 };
                _mapSprites.setFrame(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first,
                    getFrame( direction( par2 ) ) );

                for( u8 j = 0; j < par3; ++j ) {
                    for( u8 i = 0; i < 16; ++i ) {
                        moveMapObject( registers[ par1 ], m, playerAttachedToObject,
                                       SAVE::SAV.getActiveFile( ).m_player.m_direction );
                        m.m_frame = ( m.m_frame + 1 ) & 15;
                        if( i & 1 ) { swiWaitForVBlank( ); }
                    }

                    if( playerAttachedToObject ) {
                        SAVE::SAV.getActiveFile( ).m_player.m_direction = direction( par2 );
                    }
                }
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = tmp;
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
            case CVRL:
                if( SAVE::SAV.getActiveFile( ).getVar( par1 ) < par2 ) { pc += par3; }
                break;
            case CVRG:
                if( SAVE::SAV.getActiveFile( ).getVar( par1 ) > par2 ) { pc += par3; }
                if( registers[ par1 ] > par2 ) { pc += par3; }
                break;
            case CVRN:
                if( SAVE::SAV.getActiveFile( ).getVar( par1 ) != par2 ) { pc += par3; }
                break;
            case CVR:
                if( SAVE::SAV.getActiveFile( ).getVar( par1 ) == par2 ) { pc += par3; }
                break;

            case CMO: registers[ 0 ] = p_mapObject; break;
            case LCKR: {
                tmpmove = SAVE::SAV.getActiveFile( )
                              .m_mapObjects[ registers[ par1 ] ]
                              .second.m_movement;
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = NO_MOVEMENT;
                break;
            }
            case ULKR: {
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = tmpmove;
                break;
            }
            case GMO: {
                registers[ 0 ] = 255;
                for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                    auto& o2 = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
                    if( o2.second.m_pos.m_posX == par1 + mapX * SIZE
                        && o2.second.m_pos.m_posY == par2 + mapY * SIZE
                        && o2.second.m_pos.m_posZ == par3 ) {
                        registers[ 0 ] = i;
                        break;
                    }
                }
                break;
            }
            case CPP: {
                if( curx % SIZE == par1 && cury % SIZE == par2 && curz % SIZE == par3 ) {
                    registers[ 0 ] = 1;
                } else {
                    registers[ 0 ] = 0;
                }
                break;
            }
            case FNT: {
                faintPlayer( );
                break;
            }
            case BTR: {
                // If the player can't battle, they'll just lose
                if( !SAVE::SAV.getActiveFile( ).countAlivePkmn( ) ) {
                    registers[ 0 ] = 0;
                    break;
                }

                auto policy     = parB == 1 && SAVE::SAV.getActiveFile( ).countAlivePkmn( ) > 1
                                      ? BATTLE::DEFAULT_DOUBLE_TRAINER_POLICY
                                      : BATTLE::DEFAULT_TRAINER_POLICY;
                auto tr         = BATTLE::getBattleTrainer( parA );
                auto playerPrio = _mapSprites.getPriority( _playerSprite );

                SOUND::playBGM( SOUND::BGMforTrainerBattle( tr.m_data.m_trainerClass ) );
                FADE_TOP_DARK( );
                ANIMATE_MAP = false;
                swiWaitForVBlank( );

                BATTLE::battle bt
                    = BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                      SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), tr, policy );
                if( bt.start( ) == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    registers[ 0 ] = 0;
                } else {
                    registers[ 0 ] = 1;
                }

                FADE_TOP_DARK( );
                NAV::init( );
                draw( playerPrio );
                _mapSprites.setPriority( _playerSprite,
                                         SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
                SOUND::restartBGM( );
                ANIMATE_MAP = true;
                break;
            }
            case BTRR:
                // If the player can't battle, they'll just lose
                if( !SAVE::SAV.getActiveFile( ).countAlivePkmn( ) ) {
                    registers[ 0 ] = 0;
                    break;
                }

                // TODO
                break;
            case BPK: {
                // If the player can't battle, they'll just lose
                if( !SAVE::SAV.getActiveFile( ).countAlivePkmn( ) ) {
                    registers[ 0 ] = 0;
                    break;
                }

                pokemon wildPkmn = pokemon( parA, parB );

                auto playerPrio = _mapSprites.getPriority( _playerSprite );
                ANIMATE_MAP     = false;
                DRAW_TIME       = false;
                swiWaitForVBlank( );
                if( BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                    SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), wildPkmn,
                                    currentData( ).m_battlePlat1, currentData( ).m_battlePlat2,
                                    currentData( ).m_battleBG, getBattlePolicy( true ) )
                        .start( )
                    == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    registers[ 0 ] = 0;
                } else {
                    registers[ 0 ] = 1;
                }
                FADE_TOP_DARK( );
                draw( playerPrio );
                _mapSprites.setPriority( _playerSprite,
                                         SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
                ANIMATE_MAP = true;
                NAV::init( );
                break;
            }
            case BPKR:
                // If the player can't battle, they'll just lose
                if( !SAVE::SAV.getActiveFile( ).countAlivePkmn( ) ) {
                    registers[ 0 ] = 0;
                    break;
                }

                // TODO
                break;
            case ITM: NAV::giveItemToPlayer( parA, parB ); break;
            case ITMR: NAV::giveItemToPlayer( registers[ par1 ], registers[ par1 + 1 ] ); break;
            case MSC: {
                SOUND::playBGM( parA, true );
                break;
            }
            case RMS: {
                SOUND::restartBGM( );
                break;
            }
            case CRY: {
                SOUND::playCry( parA, parB );
                break;
            }
            case SFX: {
                SOUND::playSoundEffect( parA );
                break;
            }
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
            case SWT: changeWeather( mapWeather( parA ) ); break;
            case WAT:
                for( u8 i = 0; i < parA; ++i ) { swiWaitForVBlank( ); }
                break;
            case SBC: {
                setBlock( u16( mapX * SIZE + par1s ), u16( mapY * SIZE + par2s ), par3s );
                break;
            }
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
                case 4: {
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_initGameItemCount;
                    break;
                }
                case 5: {
                    if( par1 <= 4 ) [[likely]] {
                        par1++;
                        registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_initGameItems[ par1 - 1 ];
                    } else {
                        registers[ 0 ] = 0;
                        break;
                    }
                    for( u8 i = par1; i < SAVE::SAV.getActiveFile( ).m_initGameItemCount; ++i ) {
                        if( i < 4 ) [[likely]] {
                            SAVE::SAV.getActiveFile( ).m_initGameItems[ i ]
                                = SAVE::SAV.getActiveFile( ).m_initGameItems[ i + 1 ];
                        } else {
                            SAVE::SAV.getActiveFile( ).m_initGameItems[ i ] = 0;
                        }
                    }
                    SAVE::SAV.getActiveFile( ).m_initGameItemCount--;
                    break;
                }
                case 6: {
                    // init pkmn
                    SPX::runInitialPkmnSelection( );
                    break;
                }
                case 7: {
                    NAV::init( );
                    break;
                }
                case 8: {
                    SPX::runCatchingTutorial( );
                    break;
                }
                case 9: {
                    awardBadge( par2, par3 );
                    break;
                }
                default: break;
                }
                break;
            case YNM: {
                style st = (style) parB;
                if( st == MSG_NORMAL ) { st = MSG_NOCLOSE; }
                if( st == MSG_INFO ) { st = MSG_INFO_NOCLOSE; }

                registers[ 0 ]
                    = IO::yesNoBox::YES
                      == IO::yesNoBox( ).getResult(
                          convertMapString( GET_MAP_STRING( parA ), (style) parB ).c_str( ),
                          (style) parB );
                NAV::init( );
                break;
            }
            case MSG: printMapMessage( GET_MAP_STRING( parA ), (style) parB ); break;
            default: break;
            }
            ++pc;
        }
        CURRENT_SCRIPT = -1;
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
            printMapMessage( GET_STRING( 560 ), MSG_NORMAL );
            return;
        }
        case 0x86: { // TV
            printMapMessage( GET_MAP_STRING( 26 ), MSG_NORMAL );
            return;
        }
        case 0xe5: { // Pokemart shelves
            printMapMessage( GET_MAP_STRING( 127 ), MSG_NORMAL );
            break;
        }
        case 0xe0: { // picture books
            printMapMessage( GET_MAP_STRING( 133 ), MSG_NORMAL );
            break;
        }
        case 0xe1: { // picture books
            printMapMessage( GET_MAP_STRING( 134 ), MSG_NORMAL );
            break;
        }
        case 0xe2: { // PokeCenter magazines
            printMapMessage( GET_MAP_STRING( 30 ), MSG_NORMAL );
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

    void mapDrawer::runEvent( mapData::event p_event, u8 p_objectId, s16 p_mapX, s16 p_mapY ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16 mapX = curx / SIZE, mapY = cury / SIZE;

        switch( p_event.m_type ) {
        case EVENT_MESSAGE:
            printMapMessage( GET_MAP_STRING( p_event.m_data.m_message.m_msgId ),
                             (style) p_event.m_data.m_message.m_msgType );
            break;
        case EVENT_NPC_MESSAGE: {
            u8 tp = p_event.m_data.m_npc.m_scriptType & 127;
            if( tp == 10 || tp == 11 ) { tp = 0; }
            printMapMessage( GET_MAP_STRING( p_event.m_data.m_npc.m_scriptId ), (style) tp );
            if( p_event.m_data.m_npc.m_scriptType & 128 ) {
                SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, true );
            }
            break;
        }
        case EVENT_OW_PKMN: {
            if( !SAVE::SAV.getActiveFile( ).checkFlag( p_event.m_deactivateFlag ) ) {
                ANIMATE_MAP  = false;
                DRAW_TIME    = false;
                u16  pkmnIdx = p_event.m_data.m_owPkmn.m_speciesId;
                u8   level   = p_event.m_data.m_owPkmn.m_level;
                u8   forme   = p_event.m_data.m_owPkmn.m_forme & ( ~( ( 1 << 6 ) | ( 1 << 7 ) ) );
                bool female  = !!( p_event.m_data.m_owPkmn.m_forme & ( 1 << 6 ) );
                // bool genderless = !!( p_event.m_data.m_owPkmn.m_forme & ( 1 << 7 ) );

                u8   shiny    = p_event.m_data.m_owPkmn.m_shiny & ( ~( ( 1 << 6 ) | ( 1 << 7 ) ) );
                bool hiddenab = !!( p_event.m_data.m_owPkmn.m_shiny & ( 1 << 6 ) );
                bool fateful  = !!( p_event.m_data.m_owPkmn.m_shiny & ( 1 << 7 ) );

                SOUND::playCry( pkmnIdx, forme, female );
                swiWaitForVBlank( );
                swiWaitForVBlank( );

                bool luckyenc = SAVE::SAV.getActiveFile( ).m_bag.count(
                                    BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_WISHING_CHARM )
                                    ? !( rand( ) & 127 )
                                    : !( rand( ) & 2047 );
                bool charm    = SAVE::SAV.getActiveFile( ).m_bag.count(
                    BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SHINY_CHARM );

                if( luckyenc ) {
                    SOUND::playBGM( MOD_BATTLE_WILD_ALT );
                } else {
                    SOUND::playBGM( SOUND::BGMforWildBattle( pkmnIdx ) );
                }

                IO::fadeScreen( IO::BATTLE );
                IO::BG_PAL( true )[ 0 ] = 0;
                IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
                dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
                dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );

                auto wildPkmn = pokemon( pkmnIdx, level, forme, 0,
                                         shiny ? shiny : ( luckyenc ? 255 : ( charm ? 3 : 0 ) ),
                                         hiddenab || luckyenc, false, 0, 0, fateful || luckyenc );

                u8 platform = 0, plat2 = 0, battleBack = 0;

                if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF ) {
                    battleBack = currentData( ).m_surfBattleBG;
                    platform   = currentData( ).m_surfBattlePlat1;
                    plat2      = currentData( ).m_surfBattlePlat2;
                } else {
                    battleBack = currentData( ).m_battleBG;
                    platform   = currentData( ).m_battlePlat1;
                    plat2      = currentData( ).m_battlePlat2;
                }

                auto playerPrio = _mapSprites.getPriority( _playerSprite );
                swiWaitForVBlank( );
                auto battleres
                    = BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                      SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), wildPkmn,
                                      platform, plat2, battleBack, getBattlePolicy( true ) )
                          .start( );
                if( battleres == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    faintPlayer( );
                    return;
                }
                SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, 1 );
                SOUND::restartBGM( );
                FADE_TOP_DARK( );
                draw( playerPrio );

                _mapSprites.setPriority( _playerSprite,
                                         SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
                NAV::init( );

                ANIMATE_MAP = true;
                DRAW_TIME   = true;
                if( battleres != BATTLE::battle::BATTLE_CAPTURE ) {
                    char buffer[ 100 ];
                    snprintf( buffer, 99, GET_STRING( 672 ), wildPkmn.m_boxdata.m_name );
                    printMapMessage( std::string( buffer ), MSG_NORMAL );
                }
            }
            break;
        }
        case EVENT_TRAINER: {
            auto tr = BATTLE::getBattleTrainer( p_event.m_data.m_trainer.m_trainerId );
            if( !SAVE::SAV.getActiveFile( ).checkFlag(
                    SAVE::F_TRAINER_BATTLED( p_event.m_data.m_trainer.m_trainerId ) ) ) {
                // player did not defeat the trainer yet

                printMapMessage( tr.m_strings.m_message1, MSG_NORMAL );

                auto playerPrio = _mapSprites.getPriority( _playerSprite );

                SOUND::playBGM( SOUND::BGMforTrainerBattle( tr.m_data.m_trainerClass ) );

                // Check the trainer class, for some classes start a double battle
                auto policy = BATTLE::isDoubleBattleTrainerClass( tr.m_data.m_trainerClass )
                                  ? BATTLE::DEFAULT_DOUBLE_TRAINER_POLICY
                                  : BATTLE::DEFAULT_TRAINER_POLICY;

                FADE_TOP_DARK( );
                ANIMATE_MAP = false;
                swiWaitForVBlank( );

                BATTLE::battle bt
                    = BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                      SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), tr, policy );
                if( bt.start( ) == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    faintPlayer( );
                    return;
                } else {
                    SAVE::SAV.getActiveFile( ).setFlag(
                        SAVE::F_TRAINER_BATTLED( p_event.m_data.m_trainer.m_trainerId ), true );
                }
                FADE_TOP_DARK( );
                NAV::init( );
                draw( playerPrio );
                SOUND::restartBGM( );
                ANIMATE_MAP = true;
            } else {
                printMapMessage( tr.m_strings.m_message2, MSG_NORMAL );
            }
            break;
        }
        case EVENT_HMOBJECT: {
            switch( p_event.m_data.m_hmObject.m_hmType ) {
            case mapSpriteManager::SPR_STRENGTH:
                if( _strengthUsed ) {
                    NAV::printMessage( GET_STRING( 558 ), MSG_NORMAL );
                } else {
                    NAV::printMessage( GET_STRING( 318 ), MSG_NORMAL );
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
            executeScript( p_event.m_data.m_generic.m_scriptId, 0, p_mapX, p_mapY );
            break;
        }
        case EVENT_ITEM: {
            NAV::giveItemToPlayer( p_event.m_data.m_item.m_itemId, 1 );
            SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, 1 );
            constructAndAddNewMapObjects( currentData( ), mapX, mapY );
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
            if( mdata.m_events[ i ].m_type == EVENT_GENERIC ) {
                if( mdata.m_events[ i ].m_data.m_generic.m_scriptType == 11
                    && SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
                if( mdata.m_events[ i ].m_data.m_generic.m_scriptType == 10
                    && !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
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

        u16 mapX = p_globX / SIZE, mapY = p_globY / SIZE;

        auto mdata = currentData( p_globX, p_globY );
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
                && mdata.m_events[ i ].m_type != EVENT_GENERIC
                && mdata.m_events[ i ].m_type != EVENT_BERRYTREE ) {
                // These events have associated map objects
                continue;
            }

            if( mdata.m_events[ i ].m_type == EVENT_BERRYTREE ) {
                u8 berryType = SAVE::SAV.getActiveFile( ).getBerry(
                    mdata.m_events[ i ].m_data.m_berryTree.m_treeIdx );

                if( !berryType ) {
                    //  ask if player wants to plant a berry
                    if( IO::yesNoBox( ).getResult( GET_STRING( 571 ), MSG_INFO_NOCLOSE )
                        == IO::yesNoBox::YES ) {

                        FADE_TOP_DARK( );
                        ANIMATE_MAP = false;
                        swiWaitForVBlank( );

                        IO::clearScreen( false );
                        videoSetMode( MODE_5_2D );
                        bgUpdate( );

                        auto playerPrio = _mapSprites.getPriority( _playerSprite );

                        // Make player choose berry
                        SOUND::dimVolume( );
                        BAG::bagViewer bv  = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                            BAG::bagViewer::CHOOSE_BERRY );
                        u16            itm = bv.getItem( );

                        FADE_TOP_DARK( );
                        FADE_SUB_DARK( );
                        IO::clearScreen( false );
                        videoSetMode( MODE_5_2D );
                        bgUpdate( );

                        // plant the berry
                        SAVE::SAV.getActiveFile( ).plantBerry(
                            mdata.m_events[ i ].m_data.m_berryTree.m_treeIdx, itm );

                        FADE_TOP_DARK( );
                        NAV::init( );
                        draw( playerPrio );
                        _mapSprites.setPriority( _playerSprite,
                                                 SAVE::SAV.getActiveFile( ).m_playerPriority
                                                 = playerPrio );
                        SOUND::restartBGM( );
                        ANIMATE_MAP = true;
                        SOUND::restoreVolume( );
                        char buffer[ 100 ];
                        snprintf( buffer, 99, GET_STRING( 572 ),
                                  ITEM::getItemName( itm ).c_str( ) );
                        NAV::printMessage( buffer, MSG_INFO );
                    } else {
                        NAV::init( );
                    }
                    continue;
                }
            }

            if( mdata.m_events[ i ].m_type == EVENT_GENERIC ) {
                if( mdata.m_events[ i ].m_data.m_generic.m_scriptType == 11
                    && SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
                if( mdata.m_events[ i ].m_data.m_generic.m_scriptType == 10
                    && !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
            }

            if( mdata.m_events[ i ].m_trigger & dirToEventTrigger( p_dir ) ) {
                runEvent( mdata.m_events[ i ] );
            }
        }

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

            if( o.second.m_event.m_type == EVENT_BERRYTREE ) {
                u8 stage = SAVE::SAV.getActiveFile( ).getBerryStage(
                    o.second.m_event.m_data.m_berryTree.m_treeIdx );
                u8 berryType = SAVE::SAV.getActiveFile( ).getBerry(
                    o.second.m_event.m_data.m_berryTree.m_treeIdx );
                u8 yield = SAVE::SAV.getActiveFile( ).getBerryYield(
                    o.second.m_event.m_data.m_berryTree.m_treeIdx );

                if( !berryType ) {
                    // handled later
                    continue;
                }

                char buffer[ 100 ];

                switch( stage ) {
                case 0:
                case 1:
                case 2:
                case 3:
                    snprintf( buffer, 99, GET_STRING( 565 + stage ),
                              ITEM::getItemName( ITEM::berryToItem( berryType ) ).c_str( ) );
                    break;
                case 4:
                    snprintf( buffer, 99, GET_STRING( 569 ),
                              ITEM::getItemName( ITEM::berryToItem( berryType ) ).c_str( ), yield );
                    break;
                default: continue;
                }
                NAV::printMessage( buffer, MSG_INFO_CONT );
                if( stage == 4 ) {
                    // Berries can be harvested
                    if( IO::yesNoBox( ).getResult( GET_STRING( 570 ), MSG_INFO_NOCLOSE )
                        == IO::yesNoBox::YES ) {
                        NAV::init( );
                        NAV::giveItemToPlayer( ITEM::berryToItem( berryType ), yield );
                        SAVE::SAV.getActiveFile( ).harvestBerry(
                            o.second.m_event.m_data.m_berryTree.m_treeIdx );
                        _mapSprites.destroySprite( o.first );
                        o.first = 255;
                        constructAndAddNewMapObjects( currentData( p_globX, p_globY ), mapX, mapY );
                    }
                    NAV::init( );
                } else {
                    // Ask player if they want to water the berry
                    // (only if they have a watering can, though)
                    if( SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SPRAYDUCK )
                        + SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SPRINKLOTAD )
                        + SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_WAILMER_PAIL )
                        + SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SQUIRT_BOTTLE ) ) {

                        if( IO::yesNoBox( ).getResult( GET_STRING( 574 ), MSG_INFO_NOCLOSE )
                            == IO::yesNoBox::YES ) {
                            SAVE::SAV.getActiveFile( ).harvestBerry(
                                o.second.m_event.m_data.m_berryTree.m_treeIdx );
                            NAV::init( );
                            NAV::printMessage( GET_STRING( 573 ), MSG_INFO );
                            SAVE::SAV.getActiveFile( ).waterBerry(
                                o.second.m_event.m_data.m_berryTree.m_treeIdx );
                        } else {
                            NAV::init( );
                        }
                    } else {
                        NAV::init( );
                    }
                }
            }

            if( o.second.m_event.m_type == EVENT_TRAINER ) {
                // Check for exclamation mark / music change

                if( !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED(
                        o.second.m_event.m_data.m_trainer.m_trainerId ) ) ) {
                    // player did not defeat the trainer yet
                    showExclamationAboveMapObject( i );
                    auto tr
                        = BATTLE::getBattleTrainer( o.second.m_event.m_data.m_trainer.m_trainerId );
                    SOUND::playBGM( SOUND::BGMforTrainerEncounter( tr.m_data.m_trainerClass ) );
                }
            }

            runEvent( o.second.m_event, i );
            o.second.m_movement = old;
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

    void mapDrawer::runLevelScripts( const mapData& p_data, u16 p_mapX, u16 p_mapY ) {
        for( u8 i = 0; i < p_data.m_eventCount; ++i ) {
            if( p_data.m_events[ i ].m_activateFlag
                && !SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( p_data.m_events[ i ].m_deactivateFlag
                && SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( p_data.m_events[ i ].m_trigger == TRIGGER_ON_MAP_ENTER ) {
                runEvent( p_data.m_events[ i ], u8( 0 ), s16( p_mapX ), s16( p_mapY ) );
            }
        }
    }

} // namespace MAP
