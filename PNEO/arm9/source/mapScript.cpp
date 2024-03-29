/*
Pokémon neo
------------------------------

file        : mapScript.cpp
author      : Philip Wellnitz
description : Map script engine

Copyright (C) 2012 - 2023
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

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "dex/dex.h"
#include "fs/fs.h"
#include "gen/locationNames.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/keyboard.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "map/mapScript.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace MAP {
    static u16 CURRENT_SCRIPT  = -1;
    u16        registers[ 10 ] = { 0 };

    bool mapDrawer::executeWarpScript( u16 p_scriptId, warpType& p_targetType,
                                       warpPos& p_targetPos ) {
        // run script
        executeScript( p_scriptId );

        // parse registers as returned data
        p_targetType = warpType( registers[ 0 ] );
        if( !p_targetType ) { return false; }

        p_targetPos = warpPos( registers[ 1 ], position( registers[ 3 ] * SIZE + registers[ 5 ],
                                                         registers[ 2 ] * SIZE + registers[ 4 ],
                                                         +registers[ 6 ] ) );

        if( !p_targetPos.first ) { return false; }
        return true;
    }

    void mapDrawer::executeScript( u16 p_scriptId, u8 p_mapObject, s16 p_mapX, s16 p_mapY ) {
        if( CURRENT_SCRIPT == p_scriptId ) { return; }
        CURRENT_SCRIPT = p_scriptId;

        FILE* f = FS::openScript( p_scriptId );
        if( !f ) {
            CURRENT_SCRIPT = -1;
            return;
        }

        bool srn       = _scriptRunning;
        _scriptRunning = true;

        u32 SCRIPT_INS[ MAX_SCRIPT_SIZE ];
        fread( SCRIPT_INS, sizeof( u32 ), MAX_SCRIPT_SIZE, f );
        FS::close( f );

        u8 pc = 0;

        std::vector<std::pair<u16, u32>> martItems;

        std::vector<u16> choiceBoxItems;
        std::vector<u16> choiceBoxPL;
        u16              choiceBoxMessage = 0;
        u8               choiceBoxMsgType = 0;

        char buffer[ 200 ] = { 0 };

        u8   pmartCurr = 0;
        bool martSell  = true;

        bool playerAttachedToObject = false;

        moveMode tmpmove = NO_MOVEMENT;

        u8 newbnk = 255;
        u8 newz   = 0;

        memset( registers, 0, sizeof( registers ) );

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
            u16  par1x = PARAM1X( SCRIPT_INS[ pc ] );
            u8   par2x = PARAM2X( SCRIPT_INS[ pc ] );
            u8   par3x = PARAM3X( SCRIPT_INS[ pc ] );
            u8   par1s = PARAM1S( SCRIPT_INS[ pc ] );
            u8   par2s = PARAM2S( SCRIPT_INS[ pc ] );
            u16  par3s = PARAM3S( SCRIPT_INS[ pc ] );
            u16  parA  = PARAMA( SCRIPT_INS[ pc ] );
            u16  parB  = PARAMB( SCRIPT_INS[ pc ] );

#ifdef DESQUID_MORE
            IO::printMessage( ( std::to_string( pc ) + ": " + std::to_string( ins ) + " ( "
                                + std::to_string( par1 ) + " , " + std::to_string( par2 ) + " , "
                                + std::to_string( par3 ) + ")" )
                                  .c_str( ) );
#endif

            switch( ins ) {
            case DES: {
                SAVE::SAV.getActiveFile( ).registerSeenPkmn( parA );
                showPkmn( { parA, u8( parB ), false, false, false, DEFAULT_SPRITE_PID }, false );
                changeMoveMode( SAVE::SAV.getActiveFile( ).m_player.m_movement );
                swiWaitForVBlank( );
                break;
            }
            case HPK:
                removeFollowPkmn( );
                _forceNoFollow = true;
                break;
            case HPL: _mapSprites.setVisibility( _playerSprite, true ); break;
            case SPL: _mapSprites.setVisibility( _playerSprite, false ); break;
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
                if( par1 < 10 ) { showExclamationAboveMapObject( registers[ par1 ] ); }
                break;
            }
            case FIXR: {
                if( par1 < 10 ) { registers[ 0 ] = fixMapObject( registers[ par1 ] ); }
                break;
            }
            case UFXR: {
                unfixMapObject( );
                break;
            }
            case ATT: playerAttachedToObject = true; break;
            case REM: playerAttachedToObject = false; break;
            case EOP: {
                CURRENT_SCRIPT = -1;
                _scriptRunning = srn;
                return;
            }
            case GIT: {
                auto idata     = FS::getItemData( parA );
                registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_bag.count(
                    BAG::toBagType( idata.m_itemType ), parA );
                break;
            }
            case SMO: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( mapX * SIZE + par1s ), u16( mapY * SIZE + par2s ), 3 };
                obj.m_picNum    = par3s;
                obj.m_movement  = NO_MOVEMENT;
                obj.m_range     = 0;
                obj.m_direction = DOWN;
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                std::pair<u8, mapObject> cur = { 0, obj };
                if( !loadMapObject( cur ) ) {
#ifdef DESQUID_MORE
                    IO::printMessage( "SMO fail" );
#endif
                }
                // Check if there is some unused map object
                u8 found = 255;
                for( u8 i = _fixedObjectCount; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount;
                     ++i ) {
                    if( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].first == UNUSED_MAPOBJECT ) {
                        found = i;
                        break;
                    }
                }
                if( found < 255 ) {
                    registers[ 0 ] = found;
                } else {
#ifdef DESQUID_MORE
                    IO::printMessage(
                        ( std::to_string( cur.first ) + " "
                          + std::to_string( SAVE::SAV.getActiveFile( ).m_mapObjectCount ) )
                            .c_str( ) );
#endif
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_mapObjectCount++;
                }
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ 0 ] ] = cur;
                break;
            }
            case WPL: {
                redirectPlayer( direction( par2 ), false, true );
                for( u8 j = 0; j < par3; ++j ) { walkPlayer( direction( par2 ) ); }
                break;
            }
            case MPL: {
                redirectPlayer( direction( par2 ), false );
                for( u8 j = 0; j < par3; ++j ) { movePlayer( direction( par2 ) ); }
                break;
            }

            case PRM: {
                auto pkmn = SAVE::SAV.getActiveFile( ).getTeamPkmn( par1s );
                if( pkmn && pkmn->getSpecies( ) == par3s ) { pc += par2s; }
                break;
            }

            case PRMA: {
                u8 cnt = 0;
                for( u8 k = 0; k < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++k ) {
                    auto pkmn = SAVE::SAV.getActiveFile( ).getTeamPkmn( k );
                    if( pkmn && pkmn->getSpecies( ) == par3s ) { cnt++; }
                }
                if( cnt >= par1s ) { pc += par2s; }
                break;
            }

            case PDT: {
                auto pkmn = SAVE::SAV.getActiveFile( ).getTeamPkmn( par2 );
                if( !pkmn ) { break; }
                if( par1 >= PDT_STAT_START && par1 <= PDT_STAT_END ) {
                    registers[ 0 ] = pkmn->getStat( par1 - PDT_STAT_START );
                    break;
                }
                if( par1 >= PDT_EV_START && par1 <= PDT_EV_END ) {
                    registers[ 0 ] = pkmn->m_boxdata.EVget( par1 - PDT_EV_START );
                    break;
                }
                if( par1 >= PDT_IV_START && par1 <= PDT_IV_END ) {
                    registers[ 0 ] = pkmn->m_boxdata.IVget( par1 - PDT_IV_START );
                    break;
                }
                if( par1 >= PDT_CONDITION_START && par1 <= PDT_CONDITION_END ) {
                    registers[ 0 ] = pkmn->m_boxdata.m_contestStats[ par1 - PDT_CONDITION_START ];
                    break;
                }
                if( par1 == PDT_PKRS ) {
                    registers[ 0 ] = pkmn->m_boxdata.m_pokerusActive;
                    break;
                }
                if( par1 == PDT_FRIENDSHIP ) {
                    registers[ 0 ] = pkmn->m_boxdata.m_steps;
                    break;
                }
                if( par1 == PDT_EGG ) {
                    registers[ 0 ] = pkmn->m_boxdata.isEgg( );
                    break;
                }
                break;
            }

            case CMM: {
                if( SAVE::SAV.getActiveFile( ).m_player.m_movement != parA ) {
                    changeMoveMode( moveMode( parA ) );
                }
                break;
            }
            case FMM: {
                SAVE::SAV.getActiveFile( ).m_forcedMovement
                    = SAVE::SAV.getActiveFile( ).m_player.m_movement;
                break;
            }
            case UMM: {
                SAVE::SAV.getActiveFile( ).m_forcedMovement = 0;
                break;
            }
            case GMM: {
                registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_player.m_movement;
                break;
            }

            case MMO: {
                movement m = { direction( par2 ), 0 };
                _mapSprites.setFrameD( par1, direction( par2 ) );

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
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_direction
                    = direction( par2 );
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_movement = tmp;
                break;
            }
            case MFO: {
                movement m = { direction( par2 ), 0 };
                _mapSprites.setFrameD( par1, direction( par2 ) );

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
                SAVE::SAV.getActiveFile( ).m_mapObjects[ par1 ].second.m_direction
                    = direction( par2 );
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
                if( SAVE::SAV.getActiveFile( ).checkFlag( par1x ) == par2x ) { pc += par3x; }
                break;
            }
            case SFL: {
                SAVE::SAV.getActiveFile( ).setFlag( par1x, par2x );
                break;
            }
            case STF: {
                SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_TRAINER_BATTLED( par1x ), par2x );
                break;
            }
            case CTF: {
                if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED( par1x ) )
                    == par2x ) {
                    pc += par3x;
                }
                break;
            }
            case SRT: {
                SAVE::SAV.getActiveFile( ).m_route = par1;
                break;
            }
            case CRT: {
                if( SAVE::SAV.getActiveFile( ).m_route == par1 ) { pc += par2; }
                break;
            }

            case MVT: {
                u16 move    = parA;
                ANIMATE_MAP = false;

                auto oldframe = _mapSprites.getFrame(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ p_mapObject ].first );

                STS::partyScreen sts
                    = STS::partyScreen( move, SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                        SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) );
                SOUND::dimVolume( );

                auto res = sts.run( );
                FADE_TOP_DARK( );
                FADE_SUB_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                IO::resetScale( true, false );
                bgUpdate( );

                SOUND::restoreVolume( );

                IO::init( );
                MAP::curMap->draw( );
                _mapSprites.setFrame( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_mapObject ].first,
                                      oldframe );

                auto selpkmn = res.getSelectedPkmn( );

                if( selpkmn >= SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) ) {
                    registers[ 0 ] = 0;
                    ANIMATE_MAP    = true;
                    break;
                }

                registers[ 0 ] = SAVE::SAV.getActiveFile( ).getTeamPkmn( selpkmn )->learnMove(
                    move,
                    [ & ]( const char* p_message ) {
                        IO::init( );
                        IO::printMessage( p_message, MSG_NORMAL );
                    },
                    [ & ]( boxPokemon* p_pok, u16 p_extraMove ) {
                        IO::choiceBox menu3
                            = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
                        return menu3.getResult( 0, MSG_NOCLOSE, p_pok->m_moves, p_extraMove );
                    },
                    [ & ]( const char* p_message ) {
                        IO::yesNoBox yn;
                        return yn.getResult( p_message, MSG_NOCLOSE ) == IO::yesNoBox::YES;
                    } );
                IO::init( );

                ANIMATE_MAP = true;
                break;
            }

            case CRG: {
                if( registers[ par1 ] == par2 ) { pc += par3; }
                break;
            }
            case MINR: registers[ par3 ] = std::min( registers[ par1 ], registers[ par2 ] ); break;
            case MAXR: registers[ par3 ] = std::max( registers[ par1 ], registers[ par2 ] ); break;
            case SUB: registers[ parA ] -= parB; break;
            case ADD: registers[ parA ] += parB; break;
            case DIV:
                if( parB ) { registers[ parA ] /= parB; }
                break;
            case ARG: {
                registers[ par1 ] += registers[ par2 ];
                break;
            }
            case SUBR: registers[ par1 ] -= registers[ par2 ]; break;
            case DRG:
                if( registers[ par2 ] ) { registers[ par1 ] /= registers[ par2 ]; }
                break;
            case SRG: registers[ parA ] = parB; break;
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
                for( u8 i = _fixedObjectCount; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount;
                     ++i ) {
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
            case MMOR: {
#ifdef DESQUID_MORE
                IO::printMessage(
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
                _mapSprites.setFrameD(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first,
                    direction( par2 ) );

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
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_direction
                    = direction( par2 );
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_movement
                    = tmp;
                break;
            }
            case MFOR: {
#ifdef DESQUID_MORE
                IO::printMessage(
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
                _mapSprites.setFrameD(
                    SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].first,
                    direction( par2 ) );

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
                SAVE::SAV.getActiveFile( ).m_mapObjects[ registers[ par1 ] ].second.m_direction
                    = direction( par2 );
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
                if( SAVE::SAV.getActiveFile( ).checkFlag( par1x ) == registers[ par2x ] ) {
                    pc += par3x;
                }
                break;
            }
            case SFLR: {
                SAVE::SAV.getActiveFile( ).setFlag( par1x, registers[ par2x ] );
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
                break;
            case CVRN:
                if( SAVE::SAV.getActiveFile( ).getVar( par1 ) != par2 ) { pc += par3; }
                break;
            case CVR:
                if( SAVE::SAV.getActiveFile( ).getVar( par1 ) == par2 ) { pc += par3; }
                break;
            case GVR: registers[ parB ] = SAVE::SAV.getActiveFile( ).getVar( parA ); break;
            case SVR: SAVE::SAV.getActiveFile( ).setVar( parA, parB ); break;
            case SVRR: SAVE::SAV.getActiveFile( ).setVar( parA, registers[ parB ] ); break;
            case CMN:
                if( SAVE::SAV.getActiveFile( ).m_money >= parA ) { pc += parB; }
                break;
            case PMN:
                SOUND::playSoundEffect( SFX_BUY_SUCCESSFUL );

                switch( parB ) {
                default:
                case 0:
                    // money
                    if( SAVE::SAV.getActiveFile( ).m_money >= parA ) {
                        SAVE::SAV.getActiveFile( ).m_money -= parA;
                    } else {
                        SAVE::SAV.getActiveFile( ).m_money = 0;
                    }
                    break;
                case 1:
                    // battle points
                    if( SAVE::SAV.getActiveFile( ).m_battlePoints >= parA ) {
                        SAVE::SAV.getActiveFile( ).m_battlePoints -= parA;
                    } else {
                        SAVE::SAV.getActiveFile( ).m_battlePoints = 0;
                    }
                    break;
                case 2:
                    // coins
                    if( SAVE::SAV.getActiveFile( ).m_coins >= parA ) {
                        SAVE::SAV.getActiveFile( ).m_coins -= parA;
                    } else {
                        SAVE::SAV.getActiveFile( ).m_coins = 0;
                    }
                    break;
                case 3:
                    // ash
                    if( SAVE::SAV.getActiveFile( ).m_ashCount >= parA ) {
                        SAVE::SAV.getActiveFile( ).m_ashCount -= parA;
                    } else {
                        SAVE::SAV.getActiveFile( ).m_ashCount = 0;
                    }
                    break;
                }
                break;

                break;
            case RMN: {
                switch( parB ) {
                default:
                case 0:
                    // money
                    SAVE::SAV.getActiveFile( ).m_money += parA;
                    break;
                case 1:
                    // battle points
                    SAVE::SAV.getActiveFile( ).m_battlePoints += parA;
                    break;
                case 2:
                    // coins
                    SAVE::SAV.getActiveFile( ).m_coins += parA;
                    break;
                case 3:
                    // ash
                    SAVE::SAV.getActiveFile( ).m_ashCount += parA;
                    break;
                }
                break;
            }
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
            case MAP: {
                auto playerPrio = _mapSprites.getPriority( _playerSprite );
                FADE_TOP_DARK( );
                ANIMATE_MAP = false;
                swiWaitForVBlank( );

                IO::init( );
                draw( playerPrio );
                _mapSprites.setPriority( _playerSprite,
                                         SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
                ANIMATE_MAP = true;
                break;
            }
            case EQ: {
                earthquake( );
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
                auto tr         = FS::getBattleTrainer( parA );
                auto playerPrio = _mapSprites.getPriority( _playerSprite );

                ANIMATE_MAP = false;
                SOUND::playBGM( SOUND::BGMforTrainerBattle( tr.m_data.m_trainerClass ) );
                FADE_TOP_DARK( );
                swiWaitForVBlank( );

                BATTLE::battle bt
                    = BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                      SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), tr, policy );
                auto result = bt.start( _pkmnFollowsPlayer );

                FADE_TOP_DARK( );
                IO::init( );
                draw( playerPrio );
                _mapSprites.setPriority( _playerSprite,
                                         SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
                SOUND::restartBGM( );
                ANIMATE_MAP = true;

                // set result register here to avoid weird interactions with the draw call
                // and level scripts.
                if( result == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    registers[ 0 ] = 0;
                } else {
                    registers[ 0 ] = 1;
                }
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
                auto res
                    = BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                      SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), wildPkmn,
                                      currentData( ).m_battlePlat1, currentData( ).m_battlePlat2,
                                      currentData( ).m_battleBG, getBattlePolicy( true ) )
                          .start( _pkmnFollowsPlayer );

                FADE_TOP_DARK( );
                draw( playerPrio );
                _mapSprites.setPriority( _playerSprite,
                                         SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
                ANIMATE_MAP = true;
                IO::init( );

                if( res == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    registers[ 0 ] = 0;
                } else {
                    registers[ 0 ] = 1;
                }

                if( res == BATTLE::battle::BATTLE_CAPTURE ) { registers[ 1 ] = 1; }

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
            case GPK: {
                pokemon giftPkmn = pokemon( parA, parB );

                // Player obtained <pkmn>

                snprintf( buffer, 99, GET_STRING( 814 ), SAVE::SAV.getActiveFile( ).m_playername,
                          giftPkmn.m_boxdata.m_name );

                SOUND::playSoundEffect( SFX_CAPTURE_SUCCESSFUL );
                printMapMessage( buffer, MSG_INFO );

                // would you like to nickname <pkmn>?

                IO::yesNoBox yn;
                snprintf( buffer, 99, GET_STRING( 141 ), giftPkmn.m_boxdata.m_name );
                if( IO::yesNoBox::YES
                    == IO::yesNoBox( ).getResult(
                        convertMapString( buffer, MSG_INFO_NOCLOSE ).c_str( ), MSG_INFO ) ) {
                    ANIMATE_MAP = false;
                    IO::init( );
                    IO::keyboard kbd;
                    printMapMessage( GET_STRING( 142 ), MSG_INFO_NOCLOSE );
                    auto nick = kbd.getText( 10 );
                    if( strcmp( nick.c_str( ), giftPkmn.m_boxdata.m_name )
                        && strcmp( "", nick.c_str( ) ) ) {
                        strcpy( giftPkmn.m_boxdata.m_name, nick.c_str( ) );
                        giftPkmn.m_boxdata.setIsNicknamed( true );
                    }
                    ANIMATE_MAP = true;
                }

                IO::init( );

                auto cnt = SAVE::SAV.getActiveFile( ).getTeamPkmnCount( );

                if( cnt < 6 ) {
                    SAVE::SAV.getActiveFile( ).setTeamPkmn( cnt, &giftPkmn );
                    registers[ 0 ] = 1;
                } else {
                    u8 oldbx = SAVE::SAV.getActiveFile( ).m_curBox;
                    u8 nb    = SAVE::SAV.getActiveFile( ).storePkmn( giftPkmn );
                    if( nb != u8( -1 ) ) {
                        snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_SENT_TO_STORAGE ),
                                  giftPkmn.m_boxdata.m_name );
                        printMapMessage( buffer, MSG_INFO );

                        if( oldbx != nb ) {
                            snprintf( buffer, 99, GET_STRING( IO::STR_UI_STORAGE_BOX_FULL ),
                                      SAVE::SAV.getActiveFile( ).m_storedPokemon[ oldbx ].m_name );
                            printMapMessage( buffer, MSG_INFO );
                        }
                        snprintf( buffer, 99, GET_STRING( IO::STR_UI_STORAGE_BOX_PICKED ),
                                  giftPkmn.m_boxdata.m_name,
                                  SAVE::SAV.getActiveFile( ).m_storedPokemon[ nb ].m_name );
                        printMapMessage( buffer, MSG_INFO );
                        registers[ 0 ] = 1;
                    } else {
                        printMapMessage( GET_STRING( IO::STR_UI_STORAGE_ALL_BOXES_FULL ),
                                         MSG_INFO );
                        snprintf( buffer, 99, GET_STRING( IO::STR_UI_PKMN_RELEASED ),
                                  giftPkmn.m_boxdata.m_name );
                        printMapMessage( buffer, MSG_INFO );
                        registers[ 0 ] = 0;
                    }
                }
                break;
            }
            case ITM: IO::giveItemToPlayer( parA, parB ); break;
            case TTM: IO::takeItemFromPlayer( parA, parB ); break;
            case UTM: IO::useItemFromPlayer( parA, parB ); break;
            case ITMR: IO::giveItemToPlayer( registers[ par1 ], registers[ par1 + 1 ] ); break;
            case TTMR: {
                IO::takeItemFromPlayer( registers[ par1 ], registers[ par1 + 1 ] );
                break;
            }
            case UTMR: IO::useItemFromPlayer( registers[ par1 ], registers[ par1 + 1 ] ); break;
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
            case SBCC: {
                setBlock( u16( mapX * SIZE + par1s
                               + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 0 ] ),
                          u16( mapY * SIZE + par2s
                               + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 1 ] ),
                          par3s );
                break;
            }
            case SMM: {
                setMovement( u16( mapX * SIZE + par1s ), u16( mapY * SIZE + par2s ), par3s );
                break;
            }
            case CBG:
                choiceBoxItems.clear( );
                choiceBoxPL.clear( );
                choiceBoxMessage = parA;
                choiceBoxMsgType = parB;
                break;
            case CIT:
                choiceBoxItems.push_back( parA + FS::MAP_STRING );
                choiceBoxPL.push_back( parB );
                break;

            case BTZ: {
                // par 1: battle facility
                // par 2: ruleset (max level, num pkmn, battle mode, num battles, fixed
                // encounters, etc)
                switch( par1 ) {
                case BTZ_BATTLE_FACTORY: {
                    runBattleFactory( FACILITY_RULE_SETS[ par2 ] );
                    break;
                }
                default: break;
                }
                break;
            }

            case MBG:
                pmartCurr = par1;
                martSell  = par2;
                martItems.clear( );
                break;
            case MIT:
                if( pmartCurr == 0 ) {
                    martItems.push_back( { parA, u16( parB ) * 10 } );
                } else {
                    martItems.push_back( { parA, parB } );
                }
                break;
            case CLL:
                switch( par1 ) {
                case CLL_HEAL_ENTIRE_TEAM: { // heal pkmn team
                    for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
                        auto tmp = SAVE::SAV.getActiveFile( ).getTeamPkmn( i );
                        if( tmp ) { tmp->heal( ); }
                    }
                    break;
                }
                case CLL_RUN_POKE_MART: { // run pokemart
                    bool oldAM  = ANIMATE_MAP;
                    ANIMATE_MAP = false;
                    runPokeMart( martItems, 0, martSell, pmartCurr );
                    ANIMATE_MAP = oldAM;
                    break;
                }
                case CLL_GET_BADGE_COUNT: {
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).getBadgeCount( par2 );
                    break;
                }
                case CLL_INIT_GAME_ITEM_COUNT: {
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_initGameItemCount;
                    break;
                }
                case CLL_GET_AND_REMOVE_INIT_GAME_ITEM: {
                    // At most 4 init game items
                    if( par2 <= 4 ) [[likely]] {
                        registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_initGameItems[ par2 ];
                    } else {
                        registers[ 0 ] = 0;
                        break;
                    }
                    // remove first item from list of items yet to be handed out
                    for( u8 i = par2; i < SAVE::SAV.getActiveFile( ).m_initGameItemCount; ++i ) {
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
                case CLL_RUN_INITIAL_PKMN_SELECTION: {
                    // init pkmn
                    SPX::runInitialPkmnSelection( );
                    break;
                }
                case CLL_NAV_INIT: {
                    IO::init( );
                    break;
                }
                case CLL_RUN_CATCHING_TUTORIAL: {
                    SPX::runCatchingTutorial( );
                    break;
                }
                case CLL_AWARD_BADGE: {
                    awardBadge( par2, par3 );
                    break;
                }
                case CLL_RUN_CHOICE_BOX: {
                    IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
                    registers[ 0 ]   = cb.getResult( GET_MAP_STRING( choiceBoxMessage ),
                                                     style( choiceBoxMsgType ), choiceBoxItems );
                    registers[ 1 ]   = choiceBoxPL[ registers[ 0 ] ];
                    IO::init( );
                    break;
                }
                case CLL_GET_CURRENT_DAYTIME: { // get current time
                    registers[ 0 ] = getCurrentDaytime( );
                    break;
                }
                case CLL_DAYCARE_BAA_SAN: {
                    // day care baa san
                    runDayCareLady( par2 );
                    break;
                }
                case CLL_DAYCARE_JII_SAN: {
                    // day care jii san
                    runDayCareGuy( par2 );
                    break;
                }
                case CLL_SAVE_GAME: {
                    // save game, writes 1 to eval reg if successful

                    IO::yesNoBox yn;
                    if( par2 == 1
                        || yn.getResult( GET_STRING( 92 ), MSG_INFO_NOCLOSE )
                               == IO::yesNoBox::YES ) {
                        IO::init( );
                        ANIMATE_MAP = false;
                        u16 lst     = -1;
                        if( FS::writeSave( ARGV[ 0 ], [ & ]( u16 p_perc, u16 p_total ) {
                                u16 stat = p_perc * 18 / p_total;
                                if( stat != lst ) {
                                    lst = stat;
                                    IO::printMessage( 0, MSG_INFO_NOCLOSE );
                                    std::string buf2 = "";
                                    for( u8 i = 0; i < stat; ++i ) {
                                        buf2 += "\x03";
                                        if( i % 3 == 2 ) { buf2 += " "; }
                                    }
                                    for( u8 i = stat; i < 18; ++i ) {
                                        buf2 += "\x04";
                                        if( i % 3 == 2 ) { buf2 += " "; }
                                    }
                                    snprintf( buffer, 99, GET_STRING( 93 ), buf2.c_str( ) );
                                    IO::printMessage( buffer, MSG_INFO_NOCLOSE, true );
                                }
                            } ) ) {
                            IO::printMessage( 0, MSG_INFO_NOCLOSE );
                            SOUND::playSoundEffect( SFX_SAVE );
                            IO::printMessage( GET_STRING( 94 ), MSG_INFO );
                            registers[ 0 ] = 1;
                        } else {
                            IO::printMessage( 0, MSG_INFO_NOCLOSE );
                            IO::printMessage( GET_STRING( 95 ), MSG_INFO );
                            registers[ 0 ] = 0;
                        }
                        ANIMATE_MAP = true;
                    } else {
                        registers[ 0 ] = 0;
                        IO::init( );
                    }
                    break;
                }
                case CLL_HOURS_MOD: {
                    registers[ 0 ] = SAVE::CURRENT_TIME.m_hours % par2;
                    break;
                }
                case CLL_PLAYTIME_HOURS: {
                    registers[ 0 ] = SAVE::SAV.getActiveFile( ).m_playTime.m_hours;
                    break;
                }
                case CLL_HALL_OF_FAME: {
                    removeFollowPkmn( );
                    SPX::runHallOfFame( );
                    return;
                }
                case CLL_INIT_INFINITY_CAVE: {
                    initInfinityCave( par2 );
                    break;
                }
                case CLL_CONTINUE_INFINITY_CAVE: {
                    continueInfinityCave( registers[ 0 ], registers[ 1 ] );
                    break;
                }
                case CLL_MAPENTER_INFINITY_CAVE: {
                    mapEnterInfinityCave( );
                    break;
                }
                case CLL_GATECHECK_INFINITY_CAVE: {
                    gateCheckInfinityCave( );
                    break;
                }
                case CLL_PKMN_SELF_TRADER: {
                    selfTrader( );
                    break;
                }
                case CLL_PKMN_INGAME_TRADE: {
                    ingameTrade( par2, registers[ 0 ], registers[ 1 ], registers[ 2 ],
                                 registers[ 3 ] );
                    break;
                }
                case CLL_MYSTERY_GIFT_CLERK: {
                    mysteryGiftClerk( );
                    break;
                }
                case CLL_POKEBLOCK_BLENDER: {
                    pokeblockBlender( par2, par3 );
                    break;
                }
                case CLL_EFFORT_RIBBON_GIRL: {
                    effortRibbonGirl( );
                    break;
                }
                case CLL_NAME_RATER: {
                    nameRater( );
                    break;
                }
                default: break;
                }
                break;
            case COU: {
                style st       = MSG_INFO_NOCLOSE;
                registers[ 0 ] = IO::counter( 0, parB ).getResult(
                    convertMapString( GET_MAP_STRING( parA ), st ).c_str( ), st );
                IO::init( );
                break;
            }
            case COUR: {
                style st = MSG_INFO_NOCLOSE;
                registers[ 0 ]
                    = IO::counter( 0, registers[ parB ] )
                          .getResult( convertMapString( GET_MAP_STRING( parA ), st ).c_str( ), st );
                IO::init( );
#ifdef DESQUID_MORE
                std::string dstr = "";
                for( u8 q = 0; q < 10; ++q ) { dstr += std::to_string( registers[ q ] ) + " "; }
                IO::printMessage( dstr, MSG_INFO );
#endif
                break;
            }

            case YNM: {
                style st        = (style) parB;
                bool  showMoney = st & MSG_SHOW_MONEY_FLAG;
                st              = (style) ( st & 127 );

                if( st == MSG_NORMAL ) { st = MSG_NOCLOSE; }
                if( st == MSG_INFO ) { st = MSG_INFO_NOCLOSE; }

                registers[ 0 ]
                    = IO::yesNoBox::YES
                      == IO::yesNoBox( ).getResult(
                          convertMapString( GET_MAP_STRING( parA ), st ).c_str( ), st, showMoney );
                IO::init( );
                break;
            }
            case MSG: printMapMessage( GET_MAP_STRING( parA ), (style) parB ); break;
            default: break;
            }
            ++pc;
        }
        CURRENT_SCRIPT = -1;
        _scriptRunning = srn;

#ifdef DESQUID_MORE
        IO::printMessage( "SCRIPT END" );
#endif
    }
} // namespace MAP
