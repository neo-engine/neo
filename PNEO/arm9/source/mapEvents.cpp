/*
Pokémon neo
------------------------------

file        : mapEvents.cpp
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
#include "box/boxViewer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/locationNames.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace MAP {
    void mapDrawer::runEvent( mapData::event p_event, u8 p_objectId, s16 p_mapX, s16 p_mapY ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16 mapX = curx / SIZE, mapY = cury / SIZE;

        auto oldforce = _forceNoFollow;

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
                ANIMATE_MAP = false;
                DRAW_TIME   = false;
                resetTracerChain( true );
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
                                    BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_WISHING_CHARM )
                                    ? !( rand( ) & 127 )
                                    : !( rand( ) & 2047 );
                bool charm    = SAVE::SAV.getActiveFile( ).m_bag.count(
                    BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_SHINY_CHARM );

                wildPkmnType btType
                    = SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF ? WATER : GRASS;

                prepareBattleWildPkmn( btType, pkmnIdx, luckyenc );

                WILD_PKMN = pokemon( pkmnIdx, level, forme, 0,
                                     shiny ? shiny : ( luckyenc ? 255 : ( charm ? 3 : 0 ) ),
                                     hiddenab || luckyenc, false, 0, 0, fateful || luckyenc );

                SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, 1 );
                auto battleres = battleWildPkmn( btType );
                if( battleres == BATTLE::battle::BATTLE_OPPONENT_WON ) {
                    SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, 0 );
                    return;
                }
                if( battleres != BATTLE::battle::BATTLE_CAPTURE ) {
                    char buffer[ 100 ];
                    snprintf( buffer, 99, GET_STRING( 672 ), WILD_PKMN.m_boxdata.m_name );
                    printMapMessage( std::string( buffer ), MSG_NORMAL );
                }
            }
            break;
        }
        case EVENT_TRAINER: {
            auto tr = FS::getBattleTrainer( p_event.m_data.m_trainer.m_trainerId );
            if( !SAVE::SAV.getActiveFile( ).checkFlag(
                    SAVE::F_TRAINER_BATTLED( p_event.m_data.m_trainer.m_trainerId ) ) ) {
                // player did not defeat the trainer yet

                printMapMessage( tr.m_strings.m_message1, MSG_NORMAL );

                auto playerPrio = _mapSprites.getPriority( _playerSprite );

                ANIMATE_MAP = false;
                SOUND::playBGM( SOUND::BGMforTrainerBattle( tr.m_data.m_trainerClass ) );

                // Check the trainer class, for some classes start a double battle
                auto policy = BATTLE::isDoubleBattleTrainerClass( tr.m_data.m_trainerClass )
                                  ? BATTLE::DEFAULT_DOUBLE_TRAINER_POLICY
                                  : BATTLE::DEFAULT_TRAINER_POLICY;

                FADE_TOP_DARK( );
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
                IO::init( );
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
                    IO::printMessage( GET_STRING( 558 ), MSG_NORMAL );
                } else {
                    IO::printMessage( GET_STRING( 318 ), MSG_NORMAL );
                }
                break;
            case mapSpriteManager::SPR_ROCKSMASH:
                IO::printMessage( GET_STRING( 314 ), MSG_NORMAL );
                break;
            case mapSpriteManager::SPR_CUT:
                IO::printMessage( GET_STRING( 313 ), MSG_NORMAL );
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
            IO::giveItemToPlayer( p_event.m_data.m_item.m_itemId, 1 );
            SAVE::SAV.getActiveFile( ).setFlag( p_event.m_deactivateFlag, 1 );
            constructAndAddNewMapObjects( currentData( ), mapX, mapY );
        }
        default: break;
        }
        _forceNoFollow = oldforce;
    }

    void mapDrawer::handleEvents( u16 p_globX, u16 p_globY, u8 p_z ) {
        u8 x = p_globX % SIZE;
        u8 y = p_globY % SIZE;
        u8 z = p_z;

        auto mdata = currentData( );

        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
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
            if( ( mdata.m_events[ i ].m_route )
                && mdata.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                continue;
            }

            if( mdata.m_events[ i ].m_type == EVENT_FLY_POS ) {
                // register fly pos
                SAVE::SAV.getActiveFile( ).registerFlyPos(
                    flyPos{ mdata.m_events[ i ].m_data.m_flyPos.m_location,
                            SAVE::SAV.getActiveFile( ).m_currentMap, p_z, p_globX, p_globY } );
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

        // check if player moved to different position; may need to check for events at
        // new position
        if( p_globX != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
            || p_globY != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
            || p_z != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ ) {
            handleEvents( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                          SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                          SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
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
        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {

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
            if( ( mdata.m_events[ i ].m_route )
                && mdata.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                continue;
            }

            if( mdata.m_events[ i ].m_type != EVENT_MESSAGE
                && mdata.m_events[ i ].m_type != EVENT_GENERIC
                && mdata.m_events[ i ].m_type != EVENT_BERRYTREE ) {
                // These events have associated map objects
                continue;
            }

            if( mdata.m_events[ i ].m_type == EVENT_BERRYTREE ) {
                if( !SAVE::SAV.getActiveFile( ).berryIsAlive(
                        mdata.m_events[ i ].m_data.m_berryTree.m_treeIdx ) ) {
                    SAVE::SAV.getActiveFile( ).harvestBerry(
                        mdata.m_events[ i ].m_data.m_berryTree.m_treeIdx );
                }

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
                        u16            itm = bv.getItem( true );

                        FADE_TOP_DARK( );
                        FADE_SUB_DARK( );
                        IO::clearScreen( false );
                        videoSetMode( MODE_5_2D );
                        bgUpdate( );

                        if( itm ) {
                            // plant the berry
                            SAVE::SAV.getActiveFile( ).plantBerry(
                                mdata.m_events[ i ].m_data.m_berryTree.m_treeIdx, itm );
                        }

                        FADE_TOP_DARK( );
                        IO::init( );
                        draw( playerPrio );
                        _mapSprites.setPriority( _playerSprite,
                                                 SAVE::SAV.getActiveFile( ).m_playerPriority
                                                 = playerPrio );
                        SOUND::restartBGM( );
                        ANIMATE_MAP = true;
                        SOUND::restoreVolume( );
                        if( itm ) {
                            char buffer[ 100 ];
                            snprintf( buffer, 99, GET_STRING( 572 ),
                                      FS::getItemName( itm ).c_str( ) );
                            IO::printMessage( buffer, MSG_INFO );
                        }
                        return;
                    } else {
                        IO::init( );
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

            if( mdata.m_events[ i ].m_trigger == TRIGGER_NONE ) { continue; }
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

            if( o.second.m_movement != NO_MOVEMENT && o.second.m_currentMovement.m_frame ) {
                continue;
            }

            auto old = o.second.m_movement;

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
                              FS::getItemName( BAG::berryToItem( berryType ) ).c_str( ) );
                    break;
                case 4:
                    snprintf( buffer, 99, GET_STRING( 569 ),
                              FS::getItemName( BAG::berryToItem( berryType ) ).c_str( ), yield );
                    break;
                default: continue;
                }
                IO::printMessage( buffer, MSG_INFO_CONT );
                if( stage == 4 ) {
                    // Berries can be harvested
                    if( IO::yesNoBox( ).getResult( GET_STRING( 570 ), MSG_INFO_NOCLOSE )
                        == IO::yesNoBox::YES ) {
                        IO::init( );
                        IO::giveItemToPlayer( BAG::berryToItem( berryType ), yield );
                        SAVE::SAV.getActiveFile( ).harvestBerry(
                            o.second.m_event.m_data.m_berryTree.m_treeIdx );
                        _mapSprites.destroySprite( o.first );
                        o.first = 255;
                        constructAndAddNewMapObjects( currentData( p_globX, p_globY ), mapX, mapY );
                    }
                    IO::init( );
                } else {
                    // Ask player if they want to water the berry
                    // (only if they have a watering can, though)
                    if( SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_SPRAYDUCK )
                        + SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_SPRINKLOTAD )
                        + SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_WAILMER_PAIL )
                        + SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( BAG::ITEMTYPE_KEYITEM ), I_SQUIRT_BOTTLE ) ) {

                        if( IO::yesNoBox( ).getResult( GET_STRING( 574 ), MSG_INFO_NOCLOSE )
                            == IO::yesNoBox::YES ) {
                            IO::init( );
                            IO::printMessage( GET_STRING( 573 ), MSG_INFO );
                            SAVE::SAV.getActiveFile( ).waterBerry(
                                o.second.m_event.m_data.m_berryTree.m_treeIdx );
                        } else {
                            IO::init( );
                        }
                    } else {
                        IO::init( );
                    }
                }
                continue;
            } else {
                // rotate sprite to player
                if( o.second.m_picNum > 1000 || ( o.second.m_picNum & 0xff ) <= 240 ) {
                    o.second.m_movement = NO_MOVEMENT;
                    _mapSprites.setFrameD( o.first, direction( ( u8( p_dir ) + 2 ) % 4 ) );
                }
            }

            if( o.second.m_event.m_type == EVENT_TRAINER ) {
                // Check for exclamation mark / music change

                if( !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED(
                        o.second.m_event.m_data.m_trainer.m_trainerId ) ) ) {
                    // player did not defeat the trainer yet
                    showExclamationAboveMapObject( i );
                    auto tr = FS::getBattleTrainer( o.second.m_event.m_data.m_trainer.m_trainerId );
                    SOUND::playBGM( SOUND::BGMforTrainerEncounter( tr.m_data.m_trainerClass ) );
                }
            }

            u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
            u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

            runEvent( o.second.m_event, i );
            if( curx == SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                && cury == SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY ) {
                o.second.m_movement = old;
            }
        }
    }

    bool mapData::hasEvent( eventType p_type, u8 p_x, u8 p_y, u8 p_z ) const {
        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
            if( m_events[ i ].m_posX == p_x && m_events[ i ].m_posY == p_y
                && m_events[ i ].m_posZ == p_z && m_events[ i ].m_type == p_type ) {
                if( m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag( m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag( m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }
                if( ( m_events[ i ].m_route )
                    && m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                    continue;
                }

                return true;
            }
        }
        return false;
    }

    void mapDrawer::runLevelScripts( const mapData& p_data, u16 p_mapX, u16 p_mapY ) {
        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
            if( p_data.m_events[ i ].m_trigger != TRIGGER_ON_MAP_ENTER ) { continue; }

            if( p_data.m_events[ i ].m_activateFlag
                && !SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( p_data.m_events[ i ].m_deactivateFlag
                && SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_deactivateFlag ) ) {
                continue;
            }
            if( ( p_data.m_events[ i ].m_route )
                && p_data.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                continue;
            }

            runEvent( p_data.m_events[ i ], u8( 0 ), s16( p_mapX ), s16( p_mapY ) );
        }
    }

    void mapDrawer::executeMoveTriggerScript( u16 p_move ) {
        u16  curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16  curz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        auto data = currentData( curx, cury );

        auto mx = curx / SIZE;
        auto my = cury / SIZE;

        curx %= SIZE;
        cury %= SIZE;

        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
            if( data.m_events[ i ].m_type == EVENT_GENERIC
                && data.m_events[ i ].m_trigger == TRIGGER_ON_MOVE_AT_POS
                && data.m_events[ i ].m_posX == curx && data.m_events[ i ].m_posY == cury
                && data.m_events[ i ].m_posZ == curz
                && data.m_events[ i ].m_data.m_generic.m_triggerMove == p_move ) {
                if( data.m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( data.m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }
                if( ( data.m_events[ i ].m_route )
                    && data.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                    continue;
                }

                executeScript( data.m_events[ i ].m_data.m_generic.m_scriptId, 0, mx, my );
                return;
            }
        }
    }

    std::vector<u16> mapDrawer::getTriggerMovesForCurPos( ) const {
        std::vector<u16> res{ };

        u16  curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16  curz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        auto data = currentData( curx, cury );

        curx %= SIZE;
        cury %= SIZE;

        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
            if( data.m_events[ i ].m_type == EVENT_GENERIC
                && data.m_events[ i ].m_trigger == TRIGGER_ON_MOVE_AT_POS
                && data.m_events[ i ].m_posX == curx && data.m_events[ i ].m_posY == cury
                && data.m_events[ i ].m_posZ == curz ) {
                if( data.m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( data.m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }
                if( ( data.m_events[ i ].m_route )
                    && data.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                    continue;
                }

                res.push_back( data.m_events[ i ].m_data.m_generic.m_triggerMove );
            }
        }
        return res;
    }

    std::vector<u16> mapDrawer::getTriggerBattleMovesForCurPos( ) const {
        std::vector<u16> res{ };

        u16  curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        auto data = currentData( curx, cury );

        curx %= SIZE;
        cury %= SIZE;

        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
            if( data.m_events[ i ].m_type == EVENT_GENERIC
                && data.m_events[ i ].m_trigger == TRIGGER_ON_MOVE_IN_BATTLE ) {
                if( data.m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( data.m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }
                if( ( data.m_events[ i ].m_route )
                    && data.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                    continue;
                }

                res.push_back( data.m_events[ i ].m_data.m_generic.m_triggerMove );
            }
        }
        return res;
    }

    std::pair<bool, mapData::event::data> mapDrawer::getWarpData( u16 p_globX, u16 p_globY,
                                                                  u8 p_z ) {
        auto data = currentData( p_globX, p_globY );
        p_globX %= SIZE;
        p_globY %= SIZE;

        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
            if( data.m_events[ i ].m_type == EVENT_WARP && data.m_events[ i ].m_posX == p_globX
                && data.m_events[ i ].m_posY == p_globY && data.m_events[ i ].m_posZ == p_z ) {
                if( data.m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( data.m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }
                if( ( data.m_events[ i ].m_route )
                    && data.m_events[ i ].m_route != SAVE::SAV.getActiveFile( ).m_route ) {
                    continue;
                }

                return { true, data.m_events[ i ].m_data };
            }
        }
        return { false, mapData::event::data( ) };
    }

    void mapDrawer::handleWarp( warpType p_type, warpPos p_source ) {
        warpPos tg;
        u16     curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16     cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16     curz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;

        auto wdata = getWarpData( curx, cury, curz );
        curx %= SIZE;
        cury %= SIZE;
        if( !wdata.first ) { return; }

        if( wdata.second.m_warp.m_warpType != NO_SPECIAL ) {
            p_type = (warpType) wdata.second.m_warp.m_warpType;
            if( wdata.second.m_warp.m_warpType == SCRIPT ) {
                auto scriptId = wdata.second.m_warp.m_posZ;
                if( !executeWarpScript( scriptId, p_type, tg ) ) {
                    // warp script failed / didn't return
                    return;
                }
                if( p_type == LAST_VISITED ) { tg = SAVE::SAV.getActiveFile( ).m_lastCaveEntry; }
            } else {
                if( wdata.second.m_warp.m_warpType == LAST_VISITED ) {
                    tg = SAVE::SAV.getActiveFile( ).m_lastCaveEntry;
                } else {
                    tg = warpPos(
                        wdata.second.m_warp.m_bank,
                        position( wdata.second.m_warp.m_mapX * SIZE + wdata.second.m_warp.m_posX,
                                  wdata.second.m_warp.m_mapY * SIZE + wdata.second.m_warp.m_posY,
                                  +wdata.second.m_warp.m_posZ ) );
                }
            }
        }

        if( tg.first == WARP_TO_LAST_ENTRY ) { tg = SAVE::SAV.getActiveFile( ).m_lastWarp; }
        if( !tg.first && !tg.second.m_posY && !tg.second.m_posZ && !tg.second.m_posX ) return;

        SAVE::SAV.getActiveFile( ).m_lastWarp = p_source;
        warpPlayer( p_type, tg );
    }

    void mapDrawer::handleWarp( warpType p_type ) {
        warpPos current = warpPos{ SAVE::SAV.getActiveFile( ).m_currentMap,
                                   SAVE::SAV.getActiveFile( ).m_player.m_pos };
        if( p_type == LAST_VISITED ) {
            warpPos target = SAVE::SAV.getActiveFile( ).m_lastCaveEntry;
            if( !target.first && !target.second.m_posX && !target.second.m_posY
                && !target.second.m_posZ )
                return;
            SAVE::SAV.getActiveFile( ).m_lastWarp = current;

            warpPlayer( p_type, target );
        } else {
            handleWarp( p_type, current );
        }
    }

    bool mapDrawer::currentPosAllowsDirectFieldMove( ) const {
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        auto beh = at( gx, gy ).m_bottombehave;

        if( beh == BEH_NO_DIRECT_FIELD_MOVE ) { return false; }

        return true;
    }
} // namespace MAP
