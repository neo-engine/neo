/*
Pokémon neo
------------------------------

file        : mapPlayerMovement.cpp
author      : Philip Wellnitz
description : Map drawing engine: functions related to moving the player

Copyright (C) 2022 - 2022
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
#include "battle/battleDefines.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/abilityNames.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace MAP {
    bool mapDrawer::checkTrainerEye( u16 p_globX, u16 p_globY ) {
        bool hadBattle = false;
        if( !_scriptRunning ) {
            // Check for trainer
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

                if( o.second.m_event.m_type == EVENT_TRAINER ) [[unlikely]] {
                    // Check if trainer can see player

                    if( std::abs( p_globX - o.second.m_pos.m_posX ) > o.second.m_range )
                        [[likely]] {
                        continue;
                    }
                    if( std::abs( p_globY - o.second.m_pos.m_posY ) > o.second.m_range )
                        [[likely]] {
                        continue;
                    }
                    if( std::abs( p_globY - o.second.m_pos.m_posY )
                        && std::abs( p_globX - o.second.m_pos.m_posX ) ) [[likely]] {
                        continue;
                    }

                    direction trainerDir = UP;
                    direction playerDir  = DOWN;
                    if( p_globY > o.second.m_pos.m_posY ) {
                        trainerDir = DOWN;
                        playerDir  = UP;
                    }
                    if( p_globX < o.second.m_pos.m_posX ) {
                        trainerDir = LEFT;
                        playerDir  = RIGHT;
                    }
                    if( p_globX > o.second.m_pos.m_posX ) {
                        trainerDir = RIGHT;
                        playerDir  = LEFT;
                    }

                    if( trainerDir != o.second.m_direction ) { continue; }

                    // Check if anything is blocking the path between trainer and player
                    bool pathblocked = false;
                    auto stpos       = o.second.m_pos;
                    for( u8 d = 1;
                         d < dist( p_globX, p_globY, o.second.m_pos.m_posX, o.second.m_pos.m_posY );
                         ++d ) {
                        if( !canMove( stpos, trainerDir, WALK, true ) ) {
                            pathblocked = true;
                            break;
                        }
                        stpos.m_posX += dir[ trainerDir ][ 0 ];
                        stpos.m_posY += dir[ trainerDir ][ 1 ];
                        if( _pkmnFollowsPlayer && stpos.m_posX == _followPkmn.m_pos.m_posX
                            && stpos.m_posY == _followPkmn.m_pos.m_posY ) {
                            pathblocked = true;
                            break;
                        }
                    }

                    if( pathblocked ) [[unlikely]] { continue; }
                    // Check for exclamation mark / music change
                    if( !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED(
                            o.second.m_event.m_data.m_trainer.m_trainerId ) ) ) [[likely]] {
                        // player did not defeat the trainer yet
                        auto tr
                            = FS::getBattleTrainer( o.second.m_event.m_data.m_trainer.m_trainerId );

                        // Check if the battle would be a double battle; if so and if the
                        // player has only a single pkmn, the battle is optional
                        if( !BATTLE::isDoubleBattleTrainerClass( tr.m_data.m_trainerClass )
                            || SAVE::SAV.getActiveFile( ).countAlivePkmn( ) >= 2 ) {

                            SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].second.m_movement
                                = NO_MOVEMENT;
                            showExclamationAboveMapObject( i );
                            SOUND::playBGM(
                                SOUND::BGMforTrainerEncounter( tr.m_data.m_trainerClass ) );

                            // walk trainer to player
                            redirectPlayer( playerDir, false );

                            while( dist( p_globX, p_globY, o.second.m_pos.m_posX,
                                         o.second.m_pos.m_posY )
                                   > 1 ) {
                                for( u8 j = 0; j < 16; ++j ) {
                                    moveMapObject( i, { trainerDir, j } );
                                    swiWaitForVBlank( );
                                }
                            }
                            runEvent( o.second.m_event, i );
                            hadBattle = true;
                        }
                    }
                }
            }
        }
        return hadBattle;
    }

    void mapDrawer::stepOff( u16 p_globX, u16 p_globY, bool p_isPlayer, direction p_enterDir,
                             direction p_exitDir ) {
        clearFieldAnimation( p_globX, p_globY );
        animateExitField( p_globX, p_globY, p_isPlayer, p_enterDir, p_exitDir );
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        // Check for things that activate upon leaving a tile

        switch( behave ) {
        default: break;
        }
    }

    void mapDrawer::unfadeScreen( ) {
        IO::fadeScreen( IO::UNFADE );
        if( REG_BLDALPHA ) {
            REG_BLDCNT = WEATHER_BLEND;
        } else {
            REG_BLDCNT = BLEND_NONE;
        }
        bgUpdate( );
    }

    void mapDrawer::stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn, bool p_unfade,
                            bool p_runScripts ) {
        animateField( p_globX, p_globY );
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        auto curLocId = getCurrentLocationId( );
        for( const auto& fn : _newLocationCallbacks ) { fn( curLocId, false ); }

        // Check for things that activate upon stepping on a tile

        switch( behave ) {
        case BEH_GRASS_ASH: { // Add ash to the soot bag
            if( SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( BAG::ITEMTYPE_KEYITEM ),
                                                        I_SOOT_SACK ) ) {
                SAVE::SAV.getActiveFile( ).m_ashCount++;
                if( SAVE::SAV.getActiveFile( ).m_ashCount > 999'999'999 ) {
                    SAVE::SAV.getActiveFile( ).m_ashCount = 999'999'999;
                }
            }
            break;
        }
        default: break;
        }

        if( p_unfade ) { unfadeScreen( ); }

        if( p_allowWildPkmn && !_scriptRunning ) {
            if( !checkTrainerEye( p_globX, p_globY ) ) { handleWildPkmn( p_globX, p_globY ); }
        }

        if( p_runScripts && !_scriptRunning ) { handleEvents( p_globX, p_globY, p_z ); }
    }

    bool mapDrawer::canMove( position p_start, direction p_direction, moveMode p_moveMode,
                             bool p_events ) {
        u16 nx = p_start.m_posX + dir[ p_direction ][ 0 ];
        u16 ny = p_start.m_posY + dir[ p_direction ][ 1 ];

#ifdef DESQUID
        // Walk through walls for desquid purposes.
        if( keysHeld( ) & KEY_R ) { return true; }
#endif

        if( p_events ) {
            // Check if any event is occupying the target block
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
                if( ( o.second.m_pos.m_posX == nx && o.second.m_pos.m_posY == ny )
                    || ( o.second.m_currentMovement.m_frame
                         && o.second.m_pos.m_posX
                                    + dir[ o.second.m_currentMovement.m_direction ][ 0 ]
                                == nx
                         && o.second.m_pos.m_posY
                                    + dir[ o.second.m_currentMovement.m_direction ][ 1 ]
                                == ny ) ) {
                    switch( o.second.m_event.m_type ) {
                    case EVENT_HMOBJECT:
                        if( o.second.m_event.m_data.m_hmObject.m_hmType
                            == mapSpriteManager::SPR_STRENGTH ) {
                            // Check if the boulder could be moved by using strength
                            if( p_moveMode == STRENGTH
                                || !canMove( { nx, ny, p_start.m_posZ }, p_direction, STRENGTH ) ) {
                                return false;
                            }
                            // Check if the player has actually used strength
                            if( !_strengthUsed ) { return false; }
                            break;
                        }
                        if( o.second.m_event.m_data.m_hmObject.m_hmType ) { return false; }
                        break;
                    case EVENT_ITEM:
                        if( o.second.m_event.m_data.m_item.m_itemType ) {
                            return false;
                        } // item is not hidden
                        break;
                    case EVENT_NPC:
                    case EVENT_NPC_MESSAGE:
                    case EVENT_TRAINER:
                    case EVENT_OW_PKMN:
                    case EVENT_BERRYTREE: return false;
                    case EVENT_GENERIC:
                        if( o.second.m_event.m_trigger & TRIGGER_INTERACT ) { return false; }
                    default: break;
                    }
                }
            }
        }

        // Gather data about the source block
        u8 lstMoveData, lstBehave;
        lstMoveData = atom( p_start.m_posX, p_start.m_posY ).m_movedata;

        auto lstblock = at( p_start.m_posX, p_start.m_posY );
        lstBehave     = lstblock.m_bottombehave;

        // Gather data about the destination block
        u8 curMoveData, curBehave;
        curMoveData = atom( nx, ny ).m_movedata;

        auto curblock = at( nx, ny );
        curBehave     = curblock.m_bottombehave;

        // Check for special block attributes
        switch( lstBehave ) {
        case BEH_BLOCK_RIGHT:
            if( p_direction == RIGHT ) return false;
            break;
        case BEH_BLOCK_LEFT:
            if( p_direction == LEFT ) return false;
            break;
        case BEH_BLOCK_UP:
            if( p_direction == UP ) return false;
            break;
        case BEH_BLOCK_DOWN:
            if( p_direction == DOWN ) return false;
            break;
        case BEH_BLOCK_DOWN_LEFT:
            if( p_direction == DOWN || p_direction == LEFT ) return false;
            break;
        case BEH_BLOCK_DOWN_RIGHT:
            if( p_direction == DOWN || p_direction == RIGHT ) return false;
            break;
        case BEH_BLOCK_UP_LEFT:
            if( p_direction == UP || p_direction == LEFT ) return false;
            break;
        case BEH_BLOCK_UP_RIGHT:
            if( p_direction == UP || p_direction == RIGHT ) return false;
            break;

        case BEH_WALK_ONLY:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case BEH_BLOCK_UP_DOWN:
            if( ( p_direction & 1 ) == 0 ) return false;
            break;
        case BEH_BLOCK_LEFT_RIGHT:
            if( p_direction & 1 ) return false;
            break;
        case BEH_WARP_ON_WALK_RIGHT:
            if( p_direction == RIGHT ) return true;
            break;
        case BEH_WARP_ON_WALK_LEFT:
            if( p_direction == LEFT ) return true;
            break;
        case BEH_WARP_ON_WALK_UP:
            if( p_direction == UP ) return true;
            break;
        case BEH_WARP_ON_WALK_DOWN:
        case BEH_WARP_ON_WALK_DOWN_DIVE:
            if( p_direction == DOWN ) return true;
            break;

        case BEH_BIKE_BRIDGE_VERTICAL: // Bike stuff
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == LEFT || p_direction == RIGHT ) {
                if( ( curBehave != BEH_BIKE_BRIDGE_VERTICAL
                      && curBehave != BEH_BIKE_BRIDGE_VERTICAL_NO_JUMP )
                    || p_moveMode != ACRO_BIKE || !( held & KEY_B ) ) {
                    return false;
                }
            }
            break;
        case BEH_BIKE_BRIDGE_VERTICAL_NO_JUMP:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == LEFT || p_direction == RIGHT ) { return false; }
            break;
        case BEH_BIKE_BRIDGE_HORIZONTAL:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == DOWN || p_direction == UP ) {
                if( ( curBehave != BEH_BIKE_BRIDGE_HORIZONTAL
                      && curBehave != BEH_BIKE_BRIDGE_HORIZONTAL_NO_JUMP )
                    || p_moveMode != ACRO_BIKE || !( held & KEY_B ) ) {
                    return false;
                }
            }
            break;
        case BEH_BIKE_BRIDGE_HORIZONTAL_NO_JUMP:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == DOWN || p_direction == UP ) { return false; }
            break;

        default: break;
        }

        switch( curBehave ) {
        // Jumpy stuff
        case BEH_JUMP_RIGHT: return p_direction == RIGHT;
        case BEH_JUMP_LEFT: return p_direction == LEFT;
        case BEH_JUMP_UP: return p_direction == UP;
        case BEH_JUMP_DOWN: return p_direction == DOWN;

        case BEH_WALK_ONLY:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case BEH_BLOCK_UP_DOWN:
            if( ( p_direction & 1 ) == 0 ) return false;
            break;
        case BEH_BLOCK_LEFT_RIGHT:
            if( p_direction & 1 ) return false;
            break;
        case BEH_WATERFALL: return false;
        case BEH_OBSTACLE: return false;

        // door-y stuff
        case BEH_DOOR: // Player can move to a door if there is a corresponding warp at the target
                       // position
            if( currentData( nx, ny ).hasEvent( EVENT_WARP, nx % SIZE, ny % SIZE,
                                                p_start.m_posZ ) ) {
                return true;
            } else {
                return false;
            }

        case BEH_BIKE_BRIDGE_VERTICAL: // Bike stuff
        case BEH_BIKE_BRIDGE_VERTICAL_NO_JUMP:
            if( !( p_moveMode & BIKE ) ) { return false; }
            break;
        case BEH_BIKE_BRIDGE_HORIZONTAL:
        case BEH_BIKE_BRIDGE_HORIZONTAL_NO_JUMP:
            if( !( p_moveMode & BIKE ) ) { return false; }
            break;

        default: break;
        }

        if( ( p_moveMode & BIKE ) && !canBike( { nx, ny, p_start.m_posZ } ) ) { return false; }

        // Check for movedata stuff
        if( curMoveData % 4 == 1 ) { return false; }
        if( lstMoveData == MVD_SIT ) { // Stand up (only possible for the player)
            return p_direction == SAVE::SAV.getActiveFile( ).m_player.m_direction;
        }
        if( curMoveData == MVD_SIT ) { // Sit down
            return ( p_moveMode == WALK );
        }
        if( curMoveData == MVD_SURF && !( p_moveMode & SURF ) ) {
            return false;
        } else if( curMoveData == MVD_SURF ) {
            return true;
        }

        if( curMoveData == MVD_WALK && lstMoveData == MVD_SURF ) { return true; }
        if( !curMoveData || !lstMoveData ) { return true; }
        if( curMoveData == MVD_BRIDGE ) { return true; }

        return curMoveData % 4 == 0 && curMoveData / 4 == p_start.m_posZ;
    }

    void mapDrawer::movePlayer( direction p_direction, bool p_fast ) {
        u16 curx        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u8  curz        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        u16 nx          = curx + dir[ p_direction ][ 0 ];
        u16 ny          = cury + dir[ p_direction ][ 1 ];
        u8  newMoveData = atom( nx, ny ).m_movedata;
        u8  lstMoveData = atom( curx, cury ).m_movedata;
        u8  newBehave   = at( nx, ny ).m_bottombehave;
        u8  lstBehave   = at( curx, cury ).m_bottombehave;

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement != moveMode::WALK ) {
            p_fast = false; // Running is only possible when the player is actually walking
        }

        // Check if any event is occupying the target block and push it if necessary
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.second.m_pos.m_posX == nx && o.second.m_pos.m_posY == ny ) {
                switch( o.second.m_event.m_type ) {
                case EVENT_HMOBJECT:
                    if( o.second.m_event.m_data.m_hmObject.m_hmType
                        == mapSpriteManager::SPR_STRENGTH ) {
                        // Check if the boulder could be moved by using strength
                        if( !canMove( { nx, ny, curz }, p_direction, STRENGTH ) ) { continue; }
                        // Check if the player has actually used strength
                        if( !_strengthUsed ) { continue; }

                        // push the boulder one block in the current direction
                        SOUND::playSoundEffect( SFX_HM_STRENGTH );
                        for( u8 f = 0; f < 16; ++f ) {
                            _mapSprites.moveSprite( o.first, p_direction, 1 );
                            swiWaitForVBlank( );
                        }
                        o.second.m_pos.m_posX += dir[ p_direction ][ 0 ];
                        o.second.m_pos.m_posY += dir[ p_direction ][ 1 ];
                    }
                    break;
                default: break;
                }
            }
        }

        bool reinit = false, moving = true, hadjump = false;
        while( moving ) {
            // Check for end of surf, stand up and sit down
            if( lstMoveData == MVD_SIT
                && newMoveData != MVD_SIT ) { // Stand up (only possible for the player)
                if( p_direction != SAVE::SAV.getActiveFile( ).m_player.m_direction ) return;

                standUpPlayer( p_direction );
                stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
                return;
            } else if( lstMoveData == MVD_SIT ) {
                _fastBike = false;
                return;
            }
            if( newMoveData == MVD_SIT ) { // Sit down
                if( SAVE::SAV.getActiveFile( ).m_player.m_movement != WALK ) return;
                SAVE::SAV.getActiveFile( ).m_player.m_direction
                    = direction( ( u8( p_direction ) + 2 ) % 4 );
                _mapSprites.setFrameD( _playerSprite,
                                       SAVE::SAV.getActiveFile( ).m_player.m_direction );
                sitDownPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction, SIT );
                _fastBike = false;
                return;
            }
            if( newMoveData == MVD_WALK && lstMoveData == MVD_SURF ) { // End of surf
                standUpPlayer( p_direction );
                stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
                _fastBike = false;
                return;
            }

            if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, p_direction,
                          SAVE::SAV.getActiveFile( ).m_player.m_movement ) ) {
                _fastBike = false;
                moving    = false;
                stopPlayer( p_direction );
                return;
            }

            // Check for jumps/slides/...
            switch( newBehave ) {
            // First check for jumps
            case BEH_JUMP_RIGHT:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( RIGHT );
                p_direction = RIGHT;
                break;
            case BEH_JUMP_LEFT:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( LEFT );
                p_direction = LEFT;
                break;
            case BEH_JUMP_UP:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( UP );
                p_direction = UP;
                break;
            case BEH_JUMP_DOWN:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( DOWN );
                p_direction = DOWN;
                break;

            case BEH_BIKE_BRIDGE_VERTICAL: // Bike stuff
                if( p_direction == LEFT || p_direction == RIGHT ) {
                    bikeJumpPlayer( p_direction );
                    stopPlayer( p_direction );
                    return;
                }
                goto NO_BREAK;
            case BEH_BIKE_BRIDGE_HORIZONTAL:
                if( p_direction == UP || p_direction == DOWN ) {
                    bikeJumpPlayer( p_direction );
                    stopPlayer( p_direction );
                    return;
                }
                goto NO_BREAK;

            case BEH_BREAKABLE_TILE: {
                // breakable floor
                if( _fastBike > FASTBIKE_SPEED_NO_TILE_BREAK ) goto NEXT_PASS;
                walkPlayer( p_direction, p_fast );
                stopPlayer( p_direction );
                return;
            }

            case BEH_MUD_SLIDE:
                if( p_direction == DOWN ) {
                    removeFollowPkmn( );
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    reinit      = true;
                    break;
                } else {
                    if( _fastBike > FASTBIKE_SPEED_NO_TILE_BREAK ) goto NEXT_PASS;
                    walkPlayer( p_direction, p_fast );
                    removeFollowPkmn( );
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    reinit      = true;
                    break;
                }

            // Warpy stuff
            case BEH_WARP_ON_WALK_RIGHT:
                if( p_direction == RIGHT ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case BEH_WARP_ON_WALK_LEFT:
                if( p_direction == LEFT ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case BEH_WARP_ON_WALK_UP:
                if( p_direction == UP ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case BEH_WARP_ON_WALK_DOWN_DIVE:
                if( p_direction == DOWN ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( EMERGE_WATER );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case BEH_WARP_ON_WALK_DOWN:
                if( p_direction == DOWN ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            NO_BREAK:
            default:
                // If no jump has to be done, check for other stuff
                hadjump = false;
                switch( lstBehave ) {
                case BEH_SLIDE_ON_ICE:
                case BEH_SLIDE_CONTINUE: slidePlayer( p_direction ); break;
                // These change the direction of movement
                case BEH_WALK_RIGHT:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case BEH_WALK_LEFT:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case BEH_WALK_UP:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP );
                    p_direction = UP;
                    break;
                case BEH_WALK_DOWN:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case BEH_SLIDE_RIGHT:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case BEH_SLIDE_LEFT:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case BEH_SLIDE_UP:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( UP );
                    p_direction = UP;
                    break;
                case BEH_SLIDE_DOWN:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case BEH_RUN_RIGHT:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT, true );
                    p_direction = RIGHT;
                    break;
                case BEH_RUN_LEFT:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT, true );
                    p_direction = LEFT;
                    break;
                case BEH_RUN_UP:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP, true );
                    p_direction = UP;
                    break;
                case BEH_RUN_DOWN:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN, true );
                    p_direction = DOWN;
                    break;
                case BEH_WARP_ON_WALK_RIGHT:
                    if( p_direction == RIGHT ) {
                        redirectPlayer( RIGHT, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case BEH_WARP_ON_WALK_LEFT:
                    if( p_direction == LEFT ) {
                        redirectPlayer( LEFT, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case BEH_WARP_ON_WALK_UP:
                    if( p_direction == UP ) {
                        redirectPlayer( UP, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case BEH_WARP_ON_WALK_DOWN_DIVE:
                    if( p_direction == DOWN ) {
                        redirectPlayer( DOWN, p_fast );
                        handleWarp( EMERGE_WATER );
                        return;
                    }
                    goto NEXT_PASS;
                case BEH_WARP_ON_WALK_DOWN:
                    if( p_direction == DOWN ) {
                        redirectPlayer( DOWN, p_fast );
                        handleWarp( NO_SPECIAL );
                        return;
                    }
                    goto NEXT_PASS;

                case BEH_MUD_SLIDE:
                    if( _fastBike > FASTBIKE_SPEED_NO_TILE_BREAK && p_direction != DOWN )
                        goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                NEXT_PASS:
                default:
                    if( reinit ) {
                        _mapSprites.setFrameD( _playerSprite, p_direction );
                        _fastBike = false;
                        return;
                    }
                    switch( newBehave ) {
                    case BEH_SLIDE_ON_ICE:
                    case BEH_SLIDE_CONTINUE: walkPlayer( p_direction, p_fast ); break;

                    // Check for warpy stuff
                    case BEH_WARP_CAVE_ENTRY:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( CAVE_ENTRY );
                        return;
                    case BEH_WARP_NO_SPECIAL:
                    case BEH_WARP_NO_SPECIAL_2:
                    case BEH_WARP_THEN_WALK_UP:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( NO_SPECIAL );
                        return;
                    case BEH_FALL_THROUGH:
                        walkPlayer( p_direction, p_fast );
                        _fastBike = false;
                        stopPlayer( p_direction );
                        return;
                    case BEH_DOOR:
                        redirectPlayer( p_direction, p_fast );
                        openDoor( nx, ny );
                        walkPlayer( p_direction, p_fast );
                        handleWarp( DOOR );
                        break;
                    case BEH_WARP_DIVE:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( EMERGE_WATER );
                        break;
                    case BEH_WARP_TELEPORT:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( TELEPORT );
                        break;

                    // These change the direction of movement
                    case BEH_WALK_RIGHT:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case BEH_WALK_LEFT:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case BEH_WALK_UP:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case BEH_WALK_DOWN:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;

                    case BEH_SLIDE_RIGHT:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case BEH_SLIDE_LEFT:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case BEH_SLIDE_UP:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case BEH_SLIDE_DOWN:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;

                    case BEH_RUN_RIGHT:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case BEH_RUN_LEFT:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case BEH_RUN_UP:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case BEH_RUN_DOWN:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;
                    default: moving = false; continue;
                    }
                }
            }
            reinit = true;
            newMoveData
                = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_movedata;
            lstMoveData = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata;

            newBehave
                = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_bottombehave;
            lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                            .m_bottombehave;
        }
        walkPlayer( p_direction, p_fast );
        auto movedt = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        if( movedt > 4 && movedt != MVD_BRIDGE && movedt != MVD_SIT ) {
            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ = movedt / 4;
        }
    }

    void mapDrawer::fallthroughPlayer( ) {
        removeFollowPkmn( );

        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        _mapSprites.setVisibility( _playerSprite, true );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        IO::fadeScreen( IO::CLEAR_DARK );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        redirectPlayer( DOWN, false );
        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY += 32;
        draw( );
    }

    void mapDrawer::divePlayer( ) {
        // set move mode to "DIVE", change map bank to <current mapbank + DIVE_MAP>

        IO::fadeScreen( IO::CLEAR_DARK );
        changeMoveMode( DIVE );
        loadNewBank( SAVE::SAV.getActiveFile( ).m_currentMap );
        draw( OBJPRIORITY_2, false );
    }

    void mapDrawer::resurfacePlayer( ) {
        IO::fadeScreen( IO::CLEAR_DARK );
        changeMoveMode( SURF );
        loadNewBank( SAVE::SAV.getActiveFile( ).m_currentMap );
        draw( OBJPRIORITY_2, false );
    }

    void mapDrawer::flyPlayer( warpPos p_target ) {
        redirectPlayer( DOWN, false );
        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 5;

        u16 curx      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        for( u8 i = 0; i < 5; ++i ) {
            _mapSprites.drawFrame( _playerSprite, i, false, true );
            for( u8 j = 0; j < 5; ++j ) swiWaitForVBlank( );
        }

        changeMoveMode( WALK );
        warpPlayer( FLY, p_target );
    }

    void mapDrawer::warpPlayer( warpType p_type, warpPos p_target ) {
        u8   oldMapType = u8( currentData( ).m_mapType );
        bool checkPos   = false;
        _fastBike       = 0;
        _playerIsFast   = false;
        removeFollowPkmn( );

        bool crossbank = false;
        if( p_target.first != SAVE::SAV.getActiveFile( ).m_currentMap ) {
            SAVE::SAV.getActiveFile( ).m_mapObjectCount = 0;
        }
        if( !FSDATA.isOWMap( p_target.first )
            && FSDATA.isOWMap( SAVE::SAV.getActiveFile( ).m_currentMap ) ) {
            SAVE::SAV.getActiveFile( ).m_lastOWPos = { SAVE::SAV.getActiveFile( ).m_currentMap,
                                                       SAVE::SAV.getActiveFile( ).m_player.m_pos };

            checkPos = true;
        }

        if( p_target.first != SAVE::SAV.getActiveFile( ).m_currentMap ) { crossbank = true; }

        loadNewBank( p_target.first );

        mapData ndata;
        FS::readMapSliceAndData( _currentBank, nullptr, &ndata, p_target.second.m_posX / SIZE,
                                 p_target.second.m_posY / SIZE );
        u8 newMapType = u8( ndata.m_mapType );

        if( checkPos ) {
            auto curL = ndata.m_locationIds[ ( p_target.second.m_posY % SIZE ) / 8 ]
                                           [ ( p_target.second.m_posX % SIZE ) / 8 ];
            auto tmpPos = MAP_LOCATIONS.getOWPosForLocation( curL );
            if( tmpPos != mapLocation::DUMMY_POSITION ) {
                SAVE::SAV.getActiveFile( ).m_lastOWPos
                    = { SAVE::SAV.getActiveFile( ).m_currentMap, tmpPos };
            }
        }

        bool entryCave
            = ( !( oldMapType & CAVE ) && ( newMapType & CAVE ) && !( newMapType & INSIDE ) );
        if( entryCave ) {
            SAVE::SAV.getActiveFile( ).m_lastCaveEntry
                = { SAVE::SAV.getActiveFile( ).m_currentMap,
                    SAVE::SAV.getActiveFile( ).m_player.m_pos };
        }
        bool hidePlayer = true;
        bool exitCave
            = ( ( oldMapType & CAVE ) && !( oldMapType & INSIDE ) && !( newMapType & CAVE ) );
        if( exitCave ) { SAVE::SAV.getActiveFile( ).m_lastCaveEntry = { 255, { 0, 0, 0 } }; }

        if( !( oldMapType & INSIDE ) && ( newMapType & INSIDE ) ) { hidePlayer = false; }
        if( newMapType & CAVE ) { hidePlayer = false; }
        if( exitCave && p_type != DOOR && p_type != SLIDING_DOOR ) { hidePlayer = false; }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement == DIVE ) { hidePlayer = false; }

        switch( p_type ) {
        case TELEPORT:
            SOUND::playSoundEffect( SFX_WARP );
            for( u8 j = 0; j < 2; ++j ) {
                redirectPlayer( RIGHT, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( UP, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( LEFT, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( DOWN, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
            }
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        case EMERGE_WATER:
        case CAVE_ENTRY:
            SOUND::playSoundEffect( SFX_CAVE_WARP );
            if( entryCave ) {
                IO::fadeScreen( IO::CAVE_ENTRY );
            } else if( exitCave ) {
                IO::fadeScreen( IO::CAVE_EXIT );
            } else {
                IO::fadeScreen( IO::CLEAR_DARK );
            }
            break;
        case LAST_VISITED:
        case DOOR:
        case SLIDING_DOOR:
        default:
            SOUND::playSoundEffect( SFX_CAVE_WARP );
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        case NO_SPECIAL: break;
        }
        swiWaitForVBlank( );
        swiWaitForVBlank( );

        if( ( ( oldMapType & INSIDE ) && ( newMapType & INSIDE ) && p_type == CAVE_ENTRY ) ) {
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            stopPlayer( DOWN );
            stopPlayer( );
        }

        if( crossbank ) { resetMapSprites( ); }

        SAVE::SAV.getActiveFile( ).m_player.m_pos = p_target.second;
        //        if( SAVE::SAV.getActiveFile( ).m_currentMap != p_target.first ) {
        SAVE::SAV.getActiveFile( ).m_currentMap = p_target.first;

        auto oldw = SAVE::SAV.getActiveFile( ).m_currentMapWeather;
        if( ndata.m_mapType & mapType::DARK ) {
            if( ndata.m_mapType & mapType::FLASHABLE ) {
                if( SAVE::SAV.getActiveFile( ).m_currentMapWeather != DARK_FLASH_USED ) {
                    SAVE::SAV.getActiveFile( ).m_currentMapWeather = DARK_FLASHABLE;
                }
            } else {
                SAVE::SAV.getActiveFile( ).m_currentMapWeather = DARK_PERMANENT;
            }
        } else {
            SAVE::SAV.getActiveFile( ).m_currentMapWeather = (mapWeather) ndata.m_weather;
        }
        if( oldw != SAVE::SAV.getActiveFile( ).m_currentMapWeather ) { initWeather( ); }

        // hide player, may need to open a door first
        bool oldsc     = _scriptRunning;
        _scriptRunning = true;

        draw( OBJPRIORITY_2, hidePlayer );
        for( const auto& fn : _newBankCallbacks ) { fn( SAVE::SAV.getActiveFile( ).m_currentMap ); }
        auto curLocId = getCurrentLocationId( );

        if( curLocId == L_POKEMON_CENTER && oldMapType != newMapType && p_type == SLIDING_DOOR ) {
            // Register a new faint position (only if the PC was just entered)
            SAVE::SAV.getActiveFile( ).m_lastPokeCenter = p_target;
            SAVE::SAV.getActiveFile( ).m_lastPokeCenter.second.m_posY -= 4;
        }

        for( const auto& fn : _newLocationCallbacks ) { fn( curLocId, true ); }

        for( u8 i{ 0 }; i < 25; ++i ) { swiWaitForVBlank( ); }

        auto posx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        auto posy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u8 behave = at( posx, posy ).m_bottombehave;

        if( behave == BEH_DOOR ) {
            // a door, open it
            openDoor( posx, posy );
        }
        if( hidePlayer ) { drawPlayer( OBJPRIORITY_2 ); }

        if( ( currentData( ).m_mapType & INSIDE )
            && ( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::MACH_BIKE
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::ACRO_BIKE
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::BIKE ) ) {
            // Don't bike in buildings
            changeMoveMode( MAP::WALK );
        }

        if( ( ( oldMapType & INSIDE ) && ( newMapType & INSIDE ) && p_type == CAVE_ENTRY ) ) {
            if( behave != BEH_WARP_ON_WALK_DOWN ) { stopPlayer( DOWN ); }
        }

        bool oldforce  = _forceNoFollow;
        _forceNoFollow = true;
        switch( behave ) {
        case BEH_WARP_THEN_WALK_UP: walkPlayer( UP, false ); break;
        case BEH_WARP_CAVE_ENTRY: walkPlayer( DOWN, false ); break;
        case BEH_DOOR: {
            walkPlayer( DOWN, false );
            closeDoor( posx, posy );
            break;
        }

        default: break;
        }
        _forceNoFollow = oldforce;
        unfadeScreen( );
        _scriptRunning = oldsc;

        handleEvents( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
    }

    void mapDrawer::redirectPlayer( direction p_direction, bool p_fast, bool p_force ) {
        // Check if redirecting is allowed
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;

        if( ( lstBehave == BEH_BIKE_BRIDGE_VERTICAL || lstBehave == BEH_BIKE_BRIDGE_HORIZONTAL
              || lstBehave == BEH_BIKE_BRIDGE_VERTICAL_NO_JUMP
              || lstBehave == BEH_BIKE_BRIDGE_HORIZONTAL_NO_JUMP )
            && p_direction % 2 != SAVE::SAV.getActiveFile( ).m_player.m_direction % 2 ) {
            return;
        }

        // Check if the player's direction changed
        if( p_direction != SAVE::SAV.getActiveFile( ).m_player.m_direction || p_force ) {
            if( !_mapSprites.getVisibility( _playerPlatSprite ) ) {
                _mapSprites.setFrameD( _playerPlatSprite, p_direction, false );
            }
            _mapSprites.setFrame( _playerSprite, ( p_fast * 20 ) + getFrame( p_direction ) );
            SAVE::SAV.getActiveFile( ).m_player.m_direction = p_direction;
        }
    }

    void mapDrawer::standUpPlayer( direction p_direction ) {
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            stepOff( gx, gy, true, p_direction, p_direction );
        }

        redirectPlayer( p_direction, false );
        bool remPlat = SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                       || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB;

        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        _mapSprites.moveSprite( _playerSprite, DOWN, 1 );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) / 3 + 12 );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        if( remPlat ) { _mapSprites.destroySprite( _playerPlatSprite ); }
        moveCamera( p_direction, true );
        _mapSprites.moveSprite( _playerSprite, DOWN, 2 );

        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        moveCamera( p_direction, true );

        _mapSprites.moveSprite( _playerSprite, DOWN, 1 );
        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }

        changeMoveMode( WALK );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }

        stepOff( gx, gy, true, p_direction, p_direction );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::updatePlayer( ) {
        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX = _cx;
        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY = _cy;
    }

    void mapDrawer::sitDownPlayer( direction p_direction, moveMode p_newMoveMode ) {
        direction di = ( ( p_newMoveMode == SIT ) ? direction( ( u8( p_direction ) + 2 ) % 4 )
                                                  : p_direction );

        removeFollowPkmn( );
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;
        u16 gx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            stepOff( gx, gy, true, _lastPlayerMove, p_direction );
        }

        if( p_newMoveMode == SURF || p_newMoveMode == ROCK_CLIMB ) {
            // Load the Pkmn
            _playerPlatSprite
                = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPR_PLATFORM );
            _mapSprites.setFrame( _playerPlatSprite, getFrame( p_direction ) );
        }

        for( u8 i = 0; i < 7; ++i ) {
            if( i == 3 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            moveCamera( di, true );
            swiWaitForVBlank( );
        }
        changeMoveMode( p_newMoveMode );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) / 3 + 12 );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( di, true );
            if( i % 2 ) swiWaitForVBlank( );
        }
        _mapSprites.moveSprite( _playerSprite, UP, 2 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );
        moveCamera( di, true );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) );
        _mapSprites.moveSprite( _playerSprite, UP, 1 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );

        stepOff( gx, gy, true, _lastPlayerMove, p_direction );
    }

    void mapDrawer::slidePlayer( direction p_direction ) {
        removeFollowPkmn( );
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            stepOff( gx, gy, true, _lastPlayerMove, p_direction );
        }

        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
            _mapSprites.nextFrame( _playerSprite );
        }

        // check for reflection
        bool disableRefl = true;
        u8   behave
            = at( gx + dir[ p_direction ][ 0 ], gy + dir[ p_direction ][ 1 ] + 1 ).m_bottombehave;
        if( isReflective( behave ) ) {
            _mapSprites.enableReflection( _playerSprite );
            disableRefl = false;
        }
        behave
            = at( gx + dir[ p_direction ][ 0 ], gy + dir[ p_direction ][ 1 ] + 2 ).m_bottombehave;
        if( isReflective( behave ) ) {
            _mapSprites.enableReflection( _playerSprite );
            disableRefl = false;
        }

        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 ) { _mapSprites.currentFrame( _playerSprite ); }
            swiWaitForVBlank( );
        }
        stepOff( gx, gy, true, _lastPlayerMove, p_direction );

        if( disableRefl ) { _mapSprites.disableReflection( _playerSprite ); }

        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::walkPlayer( direction p_direction, bool p_fast ) {
        u16 gx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16 gz   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        u16 nx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ];
        u16 ny   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ];
        u16 ox   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX - dir[ _lastPlayerMove ][ 0 ];
        u16 oy   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY - dir[ _lastPlayerMove ][ 1 ];
        u8  anim = getTileAnimation( nx, ny );
        if( anim == mapSpriteManager::SPR_HOT_SPRING_WATER ) {
            anim = 0;
            animateField( nx, ny );
        }
        if( anim == mapSpriteManager::SPR_LONG_GRASS ) { animateField( nx, ny ); }

        if( p_direction == DOWN && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_HOT_SPRING_WATER
            && at( gx, gy ).m_bottombehave != BEH_PACIFIDLOG_LOG_VERTICAL_TOP ) {
            if( !_pkmnFollowsPlayer && !SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                stepOff( gx, gy, true, _lastPlayerMove, p_direction );
            } else {
                // stepOff( gx, gy, true, _lastPlayerMove, p_direction );
                stepOff( ox, oy, true, _lastFollowPkmnMove, _lastPlayerMove );
            }
        }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement != WALK ) p_fast = false;

        // movement for attached objects
        auto olddir         = _lastPlayerMove;
        auto oldpdir        = _lastFollowPkmnMove;
        auto oldprio        = _lastPlayerPriority;
        _lastPlayerMove     = p_direction;
        _lastPlayerPriority = _mapSprites.getPriority( _playerSprite );
        _mapSprites.setPriority( _playerFollowPkmnSprite, oldprio );

        redirectPlayer( p_direction, p_fast );

        if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ > 3
            && SAVE::SAV.getActiveFile( ).m_player.m_movement != SURF ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_1 );
        }

        if( p_fast != _playerIsFast ) {
            _playerIsFast = p_fast;
            _mapSprites.setFrame( _playerSprite,
                                  ( p_fast * PLAYER_FAST ) + getFrame( p_direction ) );
        }

        // check for reflection
        bool disableRefl = true;
        u8   behave      = at( nx, ny + 1 ).m_bottombehave;
        if( isReflective( behave ) ) {
            _mapSprites.enableReflection( _playerSprite );
            disableRefl = false;
        }
        behave = at( nx, ny + 2 ).m_bottombehave;
        if( isReflective( behave ) ) {
            _mapSprites.enableReflection( _playerSprite );
            disableRefl = false;
        }

        u8 sid = 255;
        for( u8 i = 0; i < 16; ++i ) {
            if( anim && i == 0 ) { sid = animateField( nx, ny, anim, 0 ); }
            if( anim && sid < 255 && i == 5 ) { animateField( nx, ny, sid, 2 ); }
            if( anim && sid < 255 && i == 12 ) { animateField( nx, ny, sid, 3 ); }

            moveCamera( p_direction, true );
            if( i == 8 ) { _mapSprites.nextFrame( _playerSprite ); }
            if( ( !p_fast || i % 3 ) && !_fastBike ) swiWaitForVBlank( );
            if( i % ( _fastBike / 3 + 2 ) == 0 && _fastBike ) swiWaitForVBlank( );

            if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx, { olddir, i }, false,
                               DOWN, false );
            } else if( _pkmnFollowsPlayer ) {
                _lastFollowPkmnMove = _followPkmn.m_direction;
                moveMapObject( _followPkmn, _playerFollowPkmnSprite, { olddir, i }, false, DOWN,
                               false );
            }
        }

        if( disableRefl ) { _mapSprites.disableReflection( _playerSprite ); }

        // check if the object following the player got somehow detached (due to a jump,  etc)
        if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            auto pos = SAVE::SAV.getActiveFile( )
                           .m_mapObjects[ SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx ]
                           .second.m_pos;
            if( std::abs( pos.m_posX - nx ) + std::abs( pos.m_posY - ny ) > 1 ) {
                for( u8 i = 0; i < 16; ++i ) {
                    moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx, { olddir, i },
                                   false, DOWN, false );
                    if( i % 3 ) swiWaitForVBlank( );
                }
            }
        } else if( _pkmnFollowsPlayer ) {
            if( std::abs( _followPkmn.m_pos.m_posX - nx )
                    + std::abs( _followPkmn.m_pos.m_posY - ny )
                > 1 ) {
                for( u8 i = 0; i < 16; ++i ) {
                    moveMapObject( _followPkmn, _playerFollowPkmnSprite, { olddir, i }, false, DOWN,
                                   false );
                    if( i % 3 ) swiWaitForVBlank( );
                }
            }
        }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement == WALK && !_pkmnFollowsPlayer ) {
            spawnFollowPkmn( gx, gy, gz, olddir );
        }

        if( sid < 255 ) { _mapSprites.destroySprite( sid, false ); }
        _mapSprites.drawFrame( _playerSprite, ( p_fast * PLAYER_FAST ) + getFrame( p_direction ) );
        if( ( SAVE::SAV.getActiveFile( ).m_player.m_movement & BIKE )
            || SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF ) {
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == ACRO_BIKE ) {
                _fastBike = std::min( _fastBike + 1, 4 );
            } else if( SAVE::SAV.getActiveFile( ).m_player.m_movement == MACH_BIKE ) {
                _fastBike = std::min( _fastBike + 1, 12 );
            } else if( SAVE::SAV.getActiveFile( ).m_player.m_movement == BIKE ) {
                _fastBike = std::min( _fastBike + 1, 8 );
            } else {
                _fastBike = std::min( _fastBike + 1, 6 );
            }
        } else
            _fastBike = false;

        if( atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == MVD_BRIDGE
            && ( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ <= 3
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_3 );

            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) {
                _mapSprites.setPriority( _playerPlatSprite, OBJPRIORITY_3 );
            }
        } else if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ <= 3 ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_2 );
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) {
                _mapSprites.setPriority( _playerPlatSprite, OBJPRIORITY_2 );
            }
        }
        if( !_pkmnFollowsPlayer && !SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            stepOff( gx, gy, true, olddir, p_direction );
        } else {
            // stepOff( gx, gy, true, olddir, p_direction );
            stepOff( ox, oy, true, oldpdir, olddir );
        }

        if( !updateTracerChain( p_direction ) ) { resetTracerChain( true ); }
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        if( _pkmnFollowsPlayer || SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            stepOn( gx, gy, SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ, false );
        }
        SAVE::SAV.getActiveFile( ).stepIncrease( );
        _mapSprites.reorderSprites( true );
    }

    void mapDrawer::bikeJumpPlayer( direction p_direction ) {
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            stepOff( gx, gy, true, _lastPlayerMove, p_direction );
        }

        SOUND::playSoundEffect( SFX_JUMP );

        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i < 3 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            if( i > 13 ) { _mapSprites.moveSprite( _playerSprite, DOWN, 3 ); }
            if( i % 2 ) swiWaitForVBlank( );
        }
        stepOff( gx, gy, true, _lastPlayerMove, p_direction );
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::jumpPlayer( direction p_direction ) {
        SOUND::playSoundEffect( SFX_JUMP );
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16 nx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + 2 * dir[ p_direction ][ 0 ];
        u16 ny = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + 2 * dir[ p_direction ][ 1 ];
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            stepOff( gx, gy, true, _lastPlayerMove, p_direction );
        }

        // movement for attached objects
        auto olddir         = _lastPlayerMove;
        auto oldprio        = _lastPlayerPriority;
        _lastPlayerMove     = p_direction;
        _lastPlayerPriority = _mapSprites.getPriority( _playerSprite );
        _mapSprites.setPriority( _playerFollowPkmnSprite, oldprio );

        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
        }

        bool followlong = false;
        if( _pkmnFollowsPlayer
            && std::abs( _followPkmn.m_pos.m_posX - nx ) + std::abs( _followPkmn.m_pos.m_posY - ny )
                   > 3 ) {
            followlong          = true;
            _lastFollowPkmnMove = _followPkmn.m_direction;
        }

        for( u8 i = 0; i < 32; ++i ) {
            moveCamera( p_direction, true );
            if( i < 6 && i % 2 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            if( i % 8 == 0 ) { _mapSprites.nextFrame( _playerSprite ); }
            if( i > 28 && i % 2 ) { _mapSprites.moveSprite( _playerSprite, DOWN, 3 ); }
            if( i % 4 ) swiWaitForVBlank( );

            if( i < 16 || followlong ) {
                if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                    moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx,
                                   { olddir, u8( i & 15 ) }, false, DOWN, false );
                } else if( _pkmnFollowsPlayer ) {
                    moveMapObject( _followPkmn, _playerFollowPkmnSprite, { olddir, u8( i & 15 ) },
                                   false, DOWN, false );
                }
            }
        }

        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) );
        stepOff( gx, gy, true, _lastPlayerMove, p_direction );
        if( !updateTracerChain( p_direction ) ) { resetTracerChain( true ); }
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::stopPlayer( ) {
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;
        if( lstBehave == BEH_FALL_THROUGH ) {
            // fall through
            _playerIsFast = false;
            _fastBike     = false;
            _mapSprites.setFrame( _playerSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            fallthroughPlayer( );
            return;
        }

        while( _fastBike ) {
            _fastBike = std::max( 0, (s8) _fastBike - 3 );
            if( canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos,
                         SAVE::SAV.getActiveFile( ).m_player.m_direction, BIKE ) )
                movePlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            _fastBike = std::max( 0, (s8) _fastBike - 1 );
        }
        _playerIsFast = false;
        _fastBike     = false;
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
    }

    void mapDrawer::stopPlayer( direction p_direction ) {
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;
        if( lstBehave == BEH_FALL_THROUGH ) {
            // fall through
            _playerIsFast = false;
            _fastBike     = false;
            _mapSprites.setFrame( _playerSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            fallthroughPlayer( );
            return;
        }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SIT
            && ( ( p_direction % 2 == SAVE::SAV.getActiveFile( ).m_player.m_direction % 2 )
                 || atom(
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                            .m_movedata
                        != atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                                     + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 0 ],
                                 SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                                     + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 1 ] )
                               .m_movedata ) ) {
            return;
        }
        _fastBike = false;
        redirectPlayer( p_direction, false );
        _playerIsFast = false;
        _mapSprites.nextFrame( _playerSprite );
    }

    void mapDrawer::changeMoveMode( moveMode p_newMode, bool p_hidden ) {
        bool change  = SAVE::SAV.getActiveFile( ).m_player.m_movement != p_newMode;
        u8   basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        u16  nx      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  ny      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _fastBike    = false;
        bool surfing = false;
        u8   ydif    = 0;
        SAVE::SAV.getActiveFile( ).m_player.m_movement = p_newMode;

        switch( p_newMode ) {
        case WALK: SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic; break;
        case SURF:
        case DIVE:
        case ROCK_CLIMB:
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 3;
            surfing                                      = true;
            if( p_newMode != DIVE ) { resetTracerChain( true ); }
            break;
        case BIKE:
        case MACH_BIKE:
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 1;
            resetTracerChain( );
            break;
        case ACRO_BIKE:
            //    SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 2;
            //    TODO
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 1;
            resetTracerChain( );
            break;
        case SIT:
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 3;
            ydif                                         = 2;
            resetTracerChain( );
            break;
        default: break;
        }

        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite
            = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                      SAVE::SAV.getActiveFile( ).m_player.sprite( ), p_hidden );

        // check for reflection
        bool disableRefl = true;
        u8   behave      = at( nx, ny + 1 ).m_bottombehave;
        if( isReflective( behave ) ) {
            _mapSprites.enableReflection( _playerSprite );
            disableRefl = false;
        }
        behave = at( nx, ny + 2 ).m_bottombehave;
        if( isReflective( behave ) ) {
            _mapSprites.enableReflection( _playerSprite );
            disableRefl = false;
        }

        if( disableRefl ) { _mapSprites.disableReflection( _playerSprite ); }

        if( ydif ) { _mapSprites.moveSprite( _playerSprite, UP, ydif, true ); }
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );

        if( surfing ) {
            _playerPlatSprite
                = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPR_PLATFORM, p_hidden );
            _mapSprites.setFrame( _playerPlatSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            if( !change ) { _mapSprites.moveSprite( _playerSprite, UP, 3 ); }
        }

        for( const auto& fn : _newMoveModeCallbacks ) { fn( p_newMode ); }
    }

    bool mapDrawer::canFish( position p_start, direction p_direction ) {
        // allow fishing on water tiles, as long as they are not a bridge (fishing under
        // bridges creates visual oddities.
        if( atom( p_start.m_posX + dir[ p_direction ][ 0 ],
                  p_start.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == MVD_SURF
            && atom( p_start.m_posX, p_start.m_posY ).m_movedata != MVD_BRIDGE ) {
            return true;
        }

        // allow fishing on sand tiles
        if( at( p_start.m_posX + dir[ p_direction ][ 0 ], p_start.m_posY + dir[ p_direction ][ 1 ] )
                .m_bottombehave
            == BEH_SAND_WITH_ENCOUNTER_AND_FISH ) {
            return true;
        }

        return false;
    }

    void mapDrawer::fishPlayer( direction p_direction, u8 p_rodType ) {
        PLAYER_IS_FISHING = true;
        u16 curx          = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury          = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 6;
        bool surfing  = ( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF );
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        _mapSprites.moveSprite( _playerSprite, 8 * dir[ p_direction ][ 0 ],
                                8 * ( p_direction == DOWN ), true );

        u8 frame = 0;
        if( p_direction == UP ) frame = 4;
        if( p_direction == DOWN ) frame = 8;

        for( u8 i = 0; i < 4; ++i ) {
            _mapSprites.drawFrame( _playerSprite, frame + i, p_direction == RIGHT, true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        u8          rounds = rand( ) % 5;
        bool        failed = false;
        std::string msg    = "";
        IO::printMessage( 0, MSG_NOCLOSE );
        for( u8 i = 0; i < rounds + 1; ++i ) {
            u8 cr = rand( ) % 7;
            msg   = "";
            IO::printMessage( 0, MSG_NOCLOSE );
            for( u8 j = 0; j < cr + 5; ++j ) {
                msg += ". ";
                IO::printMessage( msg.c_str( ), MSG_NOCLOSE, true );
                for( u8 k = 0; k < 30; ++k ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    int pressed = keysDown( );
                    if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_B ) ) {
                        failed = true;
                        goto OUT;
                    }
                }
            }
            FRAME_COUNT = 0;
            IO::printMessage( GET_STRING( IO::STR_MAP_FISH_SUCCESS ), MSG_NOCLOSE );
            if( FRAME_COUNT > 180 ) {
                failed = true;
                break;
            }
        }

    OUT:
        if( failed ) {
            IO::printMessage( GET_STRING( IO::STR_MAP_FISH_FAIL ) );
        } else {
            IO::printMessage( 0 );
        }
        for( s8 i = 2; i >= 0; --i ) {
            _mapSprites.drawFrame( _playerSprite, frame + i, p_direction == RIGHT, true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        changeMoveMode( surfing ? SURF : WALK );
        if( !failed ) {
            // Check if the player's leading Pokémon has sucion cups or sticky hold
            bool forceEncounter
                = ( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].isEgg( )
                    && ( SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                             == A_SUCTION_CUPS
                         || SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                                == A_STICKY_HOLD ) );

            // Start wild PKMN battle here
            switch( p_rodType ) {
            default:
            case 0: handleWildPkmn( OLD_ROD, forceEncounter ); break;
            case 1: handleWildPkmn( GOOD_ROD, forceEncounter ); break;
            case 2: handleWildPkmn( SUPER_ROD, forceEncounter ); break;
            }
        }

        PLAYER_IS_FISHING = false;
    }

    void mapDrawer::faintPlayer( ) {
        removeFollowPkmn( );
        SAVE::SAV.getActiveFile( ).increaseVar( SAVE::V_NUM_FAINTED );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        ANIMATE_MAP = false;
        videoSetMode( MODE_5_2D );
        IO::initVideo( true );
        IO::clearScreen( true, true, true );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        SOUND::setVolume( 0 );
        bgUpdate( );

        auto tgpos = SAVE::SAV.getActiveFile( ).m_lastPokeCenter;
        if( !SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first
            || SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first == 255 ) {
            SAVE::printTextAndWait( GET_STRING( IO::STR_MAP_FAINT_TO_HOME ) );
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                // TODO: move to FSINFO
                tgpos = { 20, { 0x2b, 0x89, 3 } };
            } else {
                // TODO: move to FSINFO
                tgpos = { 20, { 0x31, 0xa9, 3 } };
            }
        } else {
            SAVE::printTextAndWait( GET_STRING( IO::STR_MAP_FAINT_TO_POKE_CENTER ) );
        }
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_2 );

        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        bgUpdate( );

        auto teamcnt = SAVE::SAV.getActiveFile( ).getTeamPkmnCount( );
        for( u8 i = 0; i < teamcnt; ++i ) {
            auto pkmn = SAVE::SAV.getActiveFile( ).getTeamPkmn( i );
            if( pkmn == nullptr ) [[unlikely]] { continue; }
            pkmn->heal( );
        }

        changeMoveMode( MAP::WALK );
        SOUND::setVolume( 0 );
        IO::initVideoSub( );
        IO::resetScale( true, false );
        IO::init( );
        redirectPlayer( DOWN, false );
        warpPlayer( NO_SPECIAL, tgpos );
        ANIMATE_MAP = true;
    }
} // namespace MAP
