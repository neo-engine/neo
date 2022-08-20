/*
Pokémon neo
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine: functions related to animating the map

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

#include <algorithm>

#include "bag/bagViewer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "io/choiceBox.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace MAP {
    void mapDrawer::clearFieldAnimation( u16 p_globX, u16 p_globY ) {
        position p = { p_globX, p_globY, 0 };
        if( !_tileAnimations.count( p ) ) {
            return;
        } else {
            _mapSprites.destroySprite( _tileAnimations[ p ] );
            _tileAnimations.erase( p );
        }
    }

    u8 mapDrawer::getTileAnimation( u16 p_globX, u16 p_globY, bool p_shiny ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        switch( behave ) {
        case BEH_GRASS_UNDERWATER_NO_RESURFACE: // TODO
        case BEH_GRASS_UNDERWATER:              // TODO
        case BEH_GRASS:
        case BEH_GRASS_ASH:
            return p_shiny ? mapSpriteManager::SPR_GRASS_SHINY : mapSpriteManager::SPR_GRASS;
        case BEH_LONG_GRASS: return mapSpriteManager::SPR_LONG_GRASS;
        default: return 0;
        }
    }

    u8 mapDrawer::getTileExitAnimation( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        // TODO
        switch( behave ) {
        default: return 0;
        }
    }

    u8 mapDrawer::animateField( u16 p_globX, u16 p_globY, u8 p_animation, u8 p_frame ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( !p_frame ) {
            auto res = _mapSprites.loadSprite( curx, cury, p_globX, p_globY, p_animation );
            _mapSprites.drawFrame( res, 1 );
            if( p_animation == mapSpriteManager::SPR_LONG_GRASS ) {
                _mapSprites.setPriority( res, OBJPRIORITY_1 );
            }
            return res;
        } else {
            _mapSprites.drawFrame( p_animation, p_frame );
            _mapSprites.setPriority( p_animation, OBJPRIORITY_1 );
            return p_animation;
        }
    }

    void mapDrawer::animateField( u16 p_globX, u16 p_globY, u8 p_animation ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( _tileAnimations.count( { p_globX, p_globY, 0 } ) ) {
            clearFieldAnimation( p_globX, p_globY );
        }
        _tileAnimations[ { p_globX, p_globY, 0 } ]
            = _mapSprites.loadSprite( curx, cury, p_globX, p_globY, p_animation );
    }

    void mapDrawer::animateField( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;
        u8 anim   = getTileAnimation( p_globX, p_globY );

        switch( behave ) {
        case BEH_GRASS:
        case BEH_LONG_GRASS: {
            // tall grass
            animateField( p_globX, p_globY, anim );
            break;
        }
        case BEH_GRASS_ASH: {
            // ashen grass
            animateField( p_globX, p_globY, anim );
            if( CUR_SLICE.m_data.m_tIdx2 == 6 ) {
                setBlock( p_globX, p_globY, TS6_ASH_GRASS_BLOCK );
            } else if( CUR_SLICE.m_data.m_tIdx2 == 7 ) {
                setBlock( p_globX, p_globY, TS7_ASH_GRASS_BLOCK );
            }
            break;
        }
        case BEH_BREAKABLE_TILE: {
            // breakable floor
            // breaks on step on
            setBlock( p_globX, p_globY, BREAKABLE_TILE_BLOCK );
            break;
        }
        case BEH_PACIFIDLOG_LOG_VERTICAL_TOP: {
            setBlock( p_globX, p_globY, TS12_LOG_UPPER_DOWN );
            setBlock( p_globX, p_globY + 1, TS12_LOG_LOWER_DOWN );
            break;
        }
        case BEH_PACIFIDLOG_LOG_VERTICAL_BOTTOM: {
            setBlock( p_globX, p_globY - 1, TS12_LOG_UPPER_DOWN );
            setBlock( p_globX, p_globY, TS12_LOG_LOWER_DOWN );
            break;
        }
        case BEH_PACIFIDLOG_LOG_HORIZONTAL_LEFT: {
            setBlock( p_globX, p_globY, TS12_LOG_LEFT_DOWN );
            setBlock( p_globX + 1, p_globY, TS12_LOG_RIGHT_DOWN );
            break;
        }
        case BEH_PACIFIDLOG_LOG_HORIZONTAL_RIGHT: {
            setBlock( p_globX - 1, p_globY, TS12_LOG_LEFT_DOWN );
            setBlock( p_globX, p_globY, TS12_LOG_RIGHT_DOWN );
            break;
        }
        case BEH_FORRTREE_BRIDGE_BIKE_BELOW: {
            if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ > 3 ) {
                setBlock( p_globX, p_globY, atom( p_globX, p_globY ).m_blockidx | 1 );
            }
            break;
        }

        default: break;
        }

        // TODO
    }

    void mapDrawer::animateExitField( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;
        u8 anim   = getTileExitAnimation( p_globX, p_globY );

        (void) anim;

        switch( behave ) {
        case BEH_PACIFIDLOG_LOG_VERTICAL_TOP: {
            setBlock( p_globX, p_globY, TS12_LOG_UPPER_UP );
            setBlock( p_globX, p_globY + 1, TS12_LOG_LOWER_UP );
            break;
        }
        case BEH_PACIFIDLOG_LOG_VERTICAL_BOTTOM: {
            setBlock( p_globX, p_globY - 1, TS12_LOG_UPPER_UP );
            setBlock( p_globX, p_globY, TS12_LOG_LOWER_UP );
            break;
        }
        case BEH_PACIFIDLOG_LOG_HORIZONTAL_LEFT: {
            setBlock( p_globX, p_globY, TS12_LOG_LEFT_UP );
            setBlock( p_globX + 1, p_globY, TS12_LOG_RIGHT_UP );
            break;
        }
        case BEH_PACIFIDLOG_LOG_HORIZONTAL_RIGHT: {
            setBlock( p_globX - 1, p_globY, TS12_LOG_LEFT_UP );
            setBlock( p_globX, p_globY, TS12_LOG_RIGHT_UP );
            break;
        }
        case BEH_FORRTREE_BRIDGE_BIKE_BELOW: {
            if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ > 3 ) {
                setBlock( p_globX, p_globY, atom( p_globX, p_globY ).m_blockidx & ( ~1 ) );
            }
            break;
        }

        default: break;
        }

        // TODO
    }

    void mapDrawer::animateDoor( u16 p_globX, u16 p_globY, u8 p_z, bool p_close ) {
        u16  curx  = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  cury  = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        auto wdata = getWarpData( p_globX, p_globY, p_z );
        if( !wdata.first ) { return; }

        if( wdata.second.m_warp.m_warpType == DOOR ) {
            SOUND::playSoundEffect( SFX_ENTER_DOOR );
        } else if( wdata.second.m_warp.m_warpType == SLIDING_DOOR ) {
            SOUND::playSoundEffect( SFX_SLIDING_DOOR );
        } else {
            // Not a door?
            return;
        }

        auto block = atom( p_globX, p_globY );
        u8   ts    = 0;

        if( block.m_blockidx < MAX_BLOCKS_PER_TILE_SET ) {
            ts = CUR_SLICE.m_data.m_tIdx1;
        } else {
            ts = CUR_SLICE.m_data.m_tIdx2;
        }

        for( u8 i = 0; i < DOOR_ANIMATION_COUNT; ++i ) {
            auto d = DOOR_ANIMATIONS[ i ];
            if( d.m_tileset != ts || d.m_blockIdx != block.m_blockidx ) { continue; }

            u8 door = _mapSprites.loadDoor( curx, cury, p_globX, p_globY, d.m_doorIdx,
                                            &BG_PALETTE[ 16 * d.m_palette ] );
            if( !p_close ) {
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 1 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 2 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
            } else {
                _mapSprites.drawFrame( door, 2 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 1 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 0 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.destroySprite( door );
            }
            return;
        }

        // no animation found :/
    }

    void mapDrawer::openDoor( u16 p_globX, u16 p_globY, u8 p_z ) {
        animateDoor( p_globX, p_globY, p_z, false );
    }

    void mapDrawer::closeDoor( u16 p_globX, u16 p_globY, u8 p_z ) {
        animateDoor( p_globX, p_globY, p_z, true );
    }

    void mapDrawer::animateMapObjects( u8 p_frame ) {
        bool change = false;
        u16  curx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  cury   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u16 cx2 = curx, cy2 = cury;

        if( _pkmnFollowsPlayer ) {
            cx2 = _followPkmn.m_pos.m_posX;
            cy2 = _followPkmn.m_pos.m_posY;
        }

        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.first == UNUSED_MAPOBJECT ) { continue; }
            if( o.first == 255 ) { continue; }

            if( o.second.m_event.m_type == EVENT_BERRYTREE ) {
                if( ( p_frame & 31 ) == 15 ) { _mapSprites.nextFrame( o.first ); }
                continue;
            }

            if( o.second.m_movement == NO_MOVEMENT ) { continue; }

            if( o.second.m_movement <= 15 || o.second.m_movement == WALK_AROUND_LEFT_RIGHT
                || o.second.m_movement == WALK_AROUND_UP_DOWN
                || o.second.m_movement == WALK_AROUND_SQUARE ) {
                if( ( p_frame & 127 ) == 127 ) {
                    o.second.m_direction = getRandomLookDirection( o.second.m_movement );
                    _mapSprites.setFrameD( o.first, o.second.m_direction, false );
                    change = true;
                }
            }

            if( change ) { continue; }

            if( o.second.m_currentMovement.m_frame ) {
                moveMapObject( i, o.second.m_currentMovement );
                o.second.m_currentMovement.m_frame
                    = ( o.second.m_currentMovement.m_frame + 1 ) & 15;
                continue;
            }

            u8 rndir = rand( ) & 1;

            bool movemnt
                = ( p_frame & 127 ) == 63
                  || ( ( p_frame & 15 ) == 15 && o.second.m_movement == WALK_CONT_LEFT_RIGHT )
                  || ( ( p_frame & 15 ) == 15 && o.second.m_movement == WALK_CONT_UP_DOWN )
                  || ( ( p_frame & 15 ) == 15 && o.second.m_movement == WALK_CONT_FOLLOW_OBJECT );

            if( o.second.m_movement == WALK_CONT_FOLLOW_OBJECT ) {
                // printf( "MO %i - ", i );
                if( movemnt ) {
                    auto curdir = o.second.m_currentMovement.m_direction;
                    auto nxdir  = direction( ( curdir + 1 ) % 4 );

                    // check if the object could do a right turn (ignoring any events)
                    if( canMove( o.second.m_pos, nxdir, WALK, false ) ) {
                        o.second.m_currentMovement = { nxdir, 0 };
                    }

                    auto nox = o.second.m_pos.m_posX
                               + dir[ o.second.m_currentMovement.m_direction ][ 0 ];
                    auto noy = o.second.m_pos.m_posY
                               + dir[ o.second.m_currentMovement.m_direction ][ 1 ];

                    // Don't glitch through the player or other objects
                    if( canMove( o.second.m_pos, o.second.m_currentMovement.m_direction, WALK )
                        && ( nox != curx || noy != cury ) && ( nox != cx2 || noy != cy2 ) ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    } else {
                        o.second.m_currentMovement = { curdir, 0 };
                    }
                }
            }

            if( o.second.m_movement == WALK_AROUND_LEFT_RIGHT
                || o.second.m_movement == WALK_LEFT_RIGHT
                || o.second.m_movement == WALK_CONT_LEFT_RIGHT
                || ( o.second.m_movement == WALK_AROUND_SQUARE && rndir ) ) {
                if( movemnt ) {
                    bool nomove = false;

                    auto nxl = o.second.m_pos.m_posX + dir[ LEFT ][ 0 ];
                    auto nyl = o.second.m_pos.m_posY + dir[ LEFT ][ 1 ];
                    auto nxr = o.second.m_pos.m_posX + dir[ RIGHT ][ 0 ];
                    auto nyr = o.second.m_pos.m_posY + dir[ RIGHT ][ 1 ];
                    auto nox = o.second.m_pos.m_posX
                               + dir[ o.second.m_currentMovement.m_direction ][ 0 ];
                    auto noy = o.second.m_pos.m_posY
                               + dir[ o.second.m_currentMovement.m_direction ][ 1 ];

                    if( o.second.m_pos.m_posX % SIZE == ( o.second.m_event.m_posX + 1 ) % SIZE ) {

                        if( canMove( o.second.m_pos, LEFT, WALK ) && ( nxl != curx || nyl != cury )
                            && ( nxl != cx2 || nyl != cy2 ) ) {
                            o.second.m_currentMovement = { LEFT, 0 };
                        } else {
                            nomove = true;
                        }
                    } else if( ( o.second.m_pos.m_posX + 1 ) % SIZE
                               == o.second.m_event.m_posX % SIZE ) {
                        if( canMove( o.second.m_pos, RIGHT, WALK ) && ( nxr != curx || nyr != cury )
                            && ( nxr != cx2 || nyr != cy2 ) ) {
                            o.second.m_currentMovement = { RIGHT, 0 };
                        } else {
                            nomove = true;
                        }
                    } else {
                        if( o.second.m_currentMovement.m_direction != LEFT
                            && o.second.m_currentMovement.m_direction != RIGHT ) {
                            if( canMove( o.second.m_pos, RIGHT, WALK )
                                && ( nxr != curx || nyr != cury )
                                && ( nxr != cx2 || nyr != cy2 ) ) {
                                o.second.m_currentMovement = { RIGHT, 0 };
                            } else {
                                nomove = true;
                            }
                        } else if( ( nox == curx && noy == cury )
                                   || ( nox == cx2 && noy == cy2 ) ) {
                            nomove = true;
                        }
                    }
                    if( !nomove ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    }
                }
            }
            if( o.second.m_movement == WALK_AROUND_UP_DOWN || o.second.m_movement == WALK_UP_DOWN
                || o.second.m_movement == WALK_CONT_UP_DOWN
                || ( o.second.m_movement == WALK_AROUND_SQUARE && !rndir ) ) {
                if( movemnt ) {
                    bool nomove = false;

                    auto nxu = o.second.m_pos.m_posX + dir[ UP ][ 0 ];
                    auto nyu = o.second.m_pos.m_posY + dir[ UP ][ 1 ];
                    auto nxd = o.second.m_pos.m_posX + dir[ DOWN ][ 0 ];
                    auto nyd = o.second.m_pos.m_posY + dir[ DOWN ][ 1 ];
                    auto nox = o.second.m_pos.m_posX
                               + dir[ o.second.m_currentMovement.m_direction ][ 0 ];
                    auto noy = o.second.m_pos.m_posY
                               + dir[ o.second.m_currentMovement.m_direction ][ 1 ];

                    if( o.second.m_pos.m_posY % SIZE == ( o.second.m_event.m_posY + 1 ) % SIZE ) {
                        if( canMove( o.second.m_pos, UP, WALK ) && ( nxu != curx || nyu != cury )
                            && ( nxu != cx2 || nyu != cy2 ) ) {
                            o.second.m_currentMovement = { UP, 0 };
                        } else {
                            nomove = true;
                        }
                    } else if( ( o.second.m_pos.m_posY + 1 ) % SIZE
                               == o.second.m_event.m_posY % SIZE ) {
                        if( canMove( o.second.m_pos, DOWN, WALK ) && ( nxd != curx || nyd != cury )
                            && ( nxd != cx2 || nyd != cy2 ) ) {
                            o.second.m_currentMovement = { DOWN, 0 };
                        } else {
                            nomove = true;
                        }
                    } else {
                        if( o.second.m_currentMovement.m_direction != DOWN
                            && o.second.m_currentMovement.m_direction != UP ) {
                            if( canMove( o.second.m_pos, DOWN, WALK )
                                && ( nxd != curx || nyd != cury )
                                && ( nxd != cx2 || nyd != cy2 ) ) {
                                o.second.m_currentMovement = { DOWN, 0 };
                            } else {
                                nomove = true;
                            }
                        } else if( ( nox == curx && noy == cury )
                                   || ( nox == cx2 && noy == cy2 ) ) {
                            nomove = true;
                        }
                    }
                    if( !nomove ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    }
                }
            }
        }

        if( change ) { _mapSprites.update( ); }
    }

    void mapDrawer::animateMap( u8 p_frame ) {
        ANIMATE_MAP = false;
        // animate weather
        if( _weatherScrollX || _weatherScrollY ) {
            bgScrollf( IO::bg3, ( _weatherScrollX << 8 ) / 10, ( _weatherScrollY << 8 ) / 10 );
            bgUpdate( );
        }

        // u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        // u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        // animate map objects
        animateMapObjects( p_frame );

        loadAnimatedTiles( p_frame );

        // checkTrainerEye( curx, cury );
        ANIMATE_MAP = true;
    }

    void mapDrawer::loadAnimatedTiles( u8 p_frame ) {
        u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
        for( u8 i = 0; i < TILE_ANIMATION_COUNT; ++i ) {
            if( !TILE_ANIMATIONS[ i ].m_size ) { break; }
            if( CUR_SLICE.m_data.m_tIdx1 == TILE_ANIMATIONS[ i ].m_tileSetIdx ) {
                auto& a = TILE_ANIMATIONS[ i ];
                if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                    a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                    dmaCopy( a.m_tileData + 32 * a.m_size * a.m_acFrame,
                             tileMemory + a.m_tileIdx * 32, sizeof( tile ) * a.m_size );
                }
            }
            if( CUR_SLICE.m_data.m_tIdx2 == TILE_ANIMATIONS[ i ].m_tileSetIdx ) {
                auto& a = TILE_ANIMATIONS[ i ];
                if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                    a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                    dmaCopy( a.m_tileData + 32 * a.m_size * a.m_acFrame,
                             tileMemory + ( a.m_tileIdx + MAX_TILES_PER_TILE_SET ) * 32,
                             sizeof( tile ) * a.m_size );
                }
            }
        }
    }

    void mapDrawer::animateTracer( ) {
        u16 sx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX - TRACER_AREA;
        u16 sy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY - TRACER_AREA;

        u16 ta  = getTileAnimation( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                    SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY );
        u16 tas = getTileAnimation( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                    SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY, true );

        u8 sid[ TRACER_AREA ] = { }, scnt = 0;
        std::memset( sid, 255, sizeof( sid ) );

        for( u8 i = 0; i < 32; ++i ) {
            scnt = 0;
            for( u16 y = 0; y < 2 * TRACER_AREA + 1; ++y ) {
                for( u16 x = 0; x < 2 * TRACER_AREA + 1; ++x ) {
                    if( _tracerPositions[ y ] & ( 1 << x ) ) {
                        u16 nx   = sx + x;
                        u16 ny   = sy + y;
                        u8  slot = dist( x, y, TRACER_AREA, TRACER_AREA );
                        if( !slot || slot > TRACER_AREA ) { continue; }
                        bool shiny = tracerSlotShiny( slot );

                        if( ta && i == 0 ) {
                            sid[ scnt ] = animateField( nx, ny, shiny ? tas : ta, 0 );
                        }

                        if( shiny ) {
                            if( ta && sid[ scnt ] < 255 && ( i == 2 || i == 12 || i == 25 ) ) {
                                animateField( nx, ny, sid[ scnt ], 1 );
                            }
                            if( ta && sid[ scnt ] < 255 && ( i == 5 || i == 15 || i == 28 ) ) {
                                animateField( nx, ny, sid[ scnt ], 2 );
                            }
                            if( ta && sid[ scnt ] < 255 && ( i == 8 || i == 18 || i == 31 ) ) {
                                animateField( nx, ny, sid[ scnt ], 3 );
                            }
                        } else {
                            if( ta && sid[ scnt ] < 255 && ( i == 5 || i == 21 ) ) {
                                animateField( nx, ny, sid[ scnt ], 2 );
                            }
                            if( ta && sid[ scnt ] < 255 && ( i == 12 || i == 28 ) ) {
                                animateField( nx, ny, sid[ scnt ], 3 );
                            }
                        }

                        if( i == 31 ) { _mapSprites.destroySprite( sid[ scnt ] ); }
                        ++scnt;
                    }
                }
            }
            if( i & 3 ) { swiWaitForVBlank( ); }
            swiWaitForVBlank( );
        }
    }
} // namespace MAP
