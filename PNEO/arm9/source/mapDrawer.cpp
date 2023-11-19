/*
Pokémon neo
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine: core rendering functions

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

#ifdef DESQUID_MORE
#include <cassert>
#endif

#include "defines.h"
#include "fs/fs.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/saveGame.h"

namespace MAP {
    mapLocation MAP_LOCATIONS;

    mapDrawer*   curMap = nullptr;
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    mapDrawer::mapDrawer( ) : _curX( 0 ), _curY( 0 ), _playerIsFast( false ) {
        _mapSprites.init( );
    }

    void mapDrawer::loadNewBank( u8 p_bank ) {
        if( _currentBank != nullptr ) { fclose( _currentBank ); }
        _currentBank
            = FS::openBank( p_bank, SAVE::SAV.getActiveFile( ).m_player.m_movement == DIVE );
    }

    const mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            .m_data.m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    const block& mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           .m_data.m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ].m_blockSet.m_blocks[ blockidx ];
    }

    mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            .m_data.m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    block& mapDrawer::at( u16 p_x, u16 p_y ) {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           .m_data.m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ].m_blockSet.m_blocks[ blockidx ];
    }

    const mapData& mapDrawer::currentData( ) const {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        return currentData( curx, cury );
    }
    const mapData& mapDrawer::currentData( u16 p_x, u16 p_y ) const {
        u16 curx = p_x;
        u16 cury = p_y;

        bool x = ( curx / SIZE != CUR_SLICE.m_x ), y = ( cury / SIZE != CUR_SLICE.m_y );
        return _data[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ];
    }

    u16* mapMemory[ 4 ];

    void mapDrawer::loadBlock( block p_curblock, u32 p_memPos ) {
        // TODO: get rid of magic constants
        u8   toplayer = 1, bottomlayer = 3;
        bool elevateTopLayer = p_curblock.m_topbehave == TBEH_ELEVATE_TOP_LAYER;

        mapMemory[ toplayer ][ p_memPos ]      = !elevateTopLayer * p_curblock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 1 ]  = !elevateTopLayer * p_curblock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer ][ p_memPos + 32 ] = !elevateTopLayer * p_curblock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 33 ] = !elevateTopLayer * p_curblock.m_top[ 1 ][ 1 ];

        mapMemory[ toplayer + 1 ][ p_memPos ]      = elevateTopLayer * p_curblock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 1 ]  = elevateTopLayer * p_curblock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 32 ] = elevateTopLayer * p_curblock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 33 ] = elevateTopLayer * p_curblock.m_top[ 1 ][ 1 ];

        mapMemory[ bottomlayer ][ p_memPos ]      = p_curblock.m_bottom[ 0 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 1 ]  = p_curblock.m_bottom[ 0 ][ 1 ];
        mapMemory[ bottomlayer ][ p_memPos + 32 ] = p_curblock.m_bottom[ 1 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 33 ] = p_curblock.m_bottom[ 1 ][ 1 ];
    }

    void mapDrawer::loadBlock( block p_curblock, u8 p_scrnX, u8 p_scrnY ) {
        // TODO: get rid of magic constants
        u32 c = 64 * u32( p_scrnY ) + 2 * ( u32( p_scrnX ) % 16 );
        c += ( u32( p_scrnX ) / 16 ) * 1024;
        loadBlock( p_curblock, c );
    }

    void mapDrawer::registerOnBankChangedHandler( std::function<void( u8 )> p_handler ) {
        _newBankCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnLocationChangedHandler( std::function<void( u16, bool )> p_handler ) {
        _newLocationCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnMoveModeChangedHandler( std::function<void( moveMode )> p_handler ) {
        _newMoveModeCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnWeatherChangedHandler( std::function<void( mapWeather )> p_handler ) {
        _newWeatherCallbacks.push_back( p_handler );
    }
    // Drawing of Maps and stuff

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            ANIMATE_MAP = false;

            videoSetMode( MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE
                          | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                          | ( ( DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
                                | DISPLAY_SPR_1D_BMP_SIZE_128 | ( 5 << 28 ) | 2 )
                              & 0xffffff0 ) );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            FADE_TOP_DARK( );
            dmaFillHalfWords( 0, BG_PALETTE, 512 );
            bgUpdate( );

            _curX = _curY = 0;

            u16 mx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                my = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
            constructSlice( _currentBank, _tileset, SAVE::SAV.getActiveFile( ).m_currentMap,
                            mx / SIZE, my / SIZE, &_slices[ _curX ][ _curY ],
                            &_data[ _curX ][ _curY ], _slices );
            constructSlice( _currentBank, _tileset, SAVE::SAV.getActiveFile( ).m_currentMap,
                            mx / SIZE + currentHalf( mx ), my / SIZE,
                            &_slices[ _curX ^ 1 ][ _curY ], &_data[ _curX ^ 1 ][ _curY ], _slices );
            constructSlice( _currentBank, _tileset, SAVE::SAV.getActiveFile( ).m_currentMap,
                            mx / SIZE, my / SIZE + currentHalf( my ),
                            &_slices[ _curX ][ _curY ^ 1 ], &_data[ _curX ][ _curY ^ 1 ], _slices );
            constructSlice( _currentBank, _tileset, SAVE::SAV.getActiveFile( ).m_currentMap,
                            mx / SIZE + currentHalf( mx ), my / SIZE + currentHalf( my ),
                            &_slices[ _curX ^ 1 ][ _curY ^ 1 ], &_data[ _curX ^ 1 ][ _curY ^ 1 ],
                            _slices );

            for( u8 i = 1; i < 4; ++i ) {
                bgInit( i - 1, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetScroll( i - 1, 120, 40 );
            }
            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
            dmaCopy( CUR_SLICE.m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );

            // for palettes, the unchanged day-time pal comes first
            u8 currDT = ( getCurrentDaytime( ) + 3 ) % 5;
            if( ( currentData( ).m_mapType & INSIDE ) || ( currentData( ).m_mapType & CAVE ) ) {
                currDT = 0;
            }
            BG_PALETTE[ 0 ] = 0;

            for( u8 i = 1; i < 4; ++i ) {
                mapMemory[ i ] = (u16*) BG_MAP_RAM( 2 * i - 1 );
                bgSetPriority( i - 1, i );
            }
            // reset frame animation of objects
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto& o                            = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
                o.second.m_currentMovement.m_frame = 0;
            }

            for( u8 i = 0; i < 4; ++i ) {
                constructAndAddNewMapObjects( _data[ i % 2 ][ i / 2 ],
                                              _slices[ i % 2 ][ i / 2 ].m_x,
                                              _slices[ i % 2 ][ i / 2 ].m_y );
            }

            dmaCopy( CUR_SLICE.m_pals + currDT * 16, BG_PALETTE, 512 - 32 );
            initWeather( );
            BG_PALETTE[ 0 ] = 0;

            _mapSprites.reset( );
            // Restore the map objects
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                loadMapObject( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] );
            }
            if( _pkmnFollowsPlayer ) {
                if( updateFollowPkmn( ) ) {
                    std::pair<u8, mapObject> cur = { _playerFollowPkmnSprite, _followPkmn };
                    loadMapObject( cur );
                    _followPkmn             = cur.second;
                    _playerFollowPkmnSprite = cur.first;
                } else {
                    removeFollowPkmn( );
                }
            }
            if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                attachMapObjectToPlayer( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx );
            }

            runLevelScripts( _data[ _curX ][ _curY ], mx / SIZE, my / SIZE );
            runLevelScripts( _data[ _curX ^ 1 ][ _curY ], mx / SIZE + currentHalf( mx ),
                             my / SIZE );
            runLevelScripts( _data[ _curX ][ _curY ^ 1 ], mx / SIZE,
                             my / SIZE + currentHalf( my ) );
            runLevelScripts( _data[ _curX ^ 1 ][ _curY ^ 1 ], mx / SIZE + currentHalf( mx ),
                             my / SIZE + currentHalf( my ) );
        }

        _lastrow = NUM_ROWS - 1;
        _lastcol = NUM_COLS - 1;

        _cx = p_globX;
        _cy = p_globY;

        u16 mny = p_globY - 8;
        u16 mnx = p_globX - 15;

        ANIMATE_MAP = false;
        for( u16 y = 0; y < NUM_ROWS; y++ )
            for( u16 x = 0; x < NUM_COLS; x++ ) { loadBlock( at( mnx + x, mny + y ), x, y ); }

        bgUpdate( );
        ANIMATE_MAP = true;
    }

    void mapDrawer::draw( ObjPriority, bool p_playerHidden, bool p_init ) {
        draw( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
              SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY, true ); // Draw the map
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ, false, false, false );

        if( p_init ) { unfadeScreen( ); }

        drawPlayer( SAVE::SAV.getActiveFile( ).m_playerPriority,
                    p_playerHidden ); // Draw the player

        unfadeScreen( );

        for( const auto& fn : _newBankCallbacks ) { fn( SAVE::SAV.getActiveFile( ).m_currentMap ); }
        auto curLocId = getCurrentLocationId( );
        for( const auto& fn : _newLocationCallbacks ) { fn( curLocId, false ); }

        if( !_scriptRunning ) {
            handleEvents( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                          SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                          SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        }
    }

    void mapDrawer::setBlock( u16 p_globX, u16 p_globY, u16 p_newBlock ) {
        if( p_newBlock > 2 * MAX_BLOCKS_PER_TILE_SET ) [[unlikely]] { return; }
        atom( p_globX, p_globY ).m_blockidx = p_newBlock;

        auto curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        auto cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        if( std::abs( curx - p_globX ) >= NUM_COLS / 2
            || std::abs( cury - p_globY ) > NUM_ROWS / 2 ) {
            // A non-visible block got changed, we should not draw it on-screen.
            return;
        }

        loadBlock( at( p_globX, p_globY ), ( _lastcol + NUM_COLS / 2 - curx + p_globX ) % NUM_COLS,
                   ( _lastrow + NUM_ROWS / 2 + 1 - cury + p_globY ) % NUM_ROWS );
        bgUpdate( );
    }

    void mapDrawer::setMovement( u16 p_globX, u16 p_globY, u16 p_newMovement ) {
        if( p_newMovement > 0xff ) [[unlikely]] { return; }
        atom( p_globX, p_globY ).m_movedata = p_newMovement;
    }

    void mapDrawer::loadNewRow( direction p_direction, bool p_updatePlayer ) {
        _cx += dir[ p_direction ][ 0 ];
        _cy += dir[ p_direction ][ 1 ];
#ifdef DESQUID_MORE
        assert( _cx != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                || _cy != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY );
#endif
        if( p_updatePlayer ) { updatePlayer( ); }

        // Check if a new slice should be loaded
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 32 == 16 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 32 == 15 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 32 == 16 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 32 == 15 ) ) {

            // Hacky optimization: Don't load new slices on inside maps.
            if( ( currentData( ).m_mapType & CAVE ) || !( currentData( ).m_mapType & INSIDE ) ) {
                ANIMATE_MAP = false;
                DRAW_TIME   = false;
                loadSlice( p_direction );
                ANIMATE_MAP = true;
                DRAW_TIME   = true;
            }
#ifdef DESQUID_MORE
            IO::printMessage( "Load Slice" );
#endif
        }

        // Check if map objects should be updated
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 16 == 8 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 16 == 7 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 16 == 8 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 16 == 7 ) ) {
            ANIMATE_MAP = false;
            for( u8 i = 0; i < 4; ++i ) {
                constructAndAddNewMapObjects( _data[ i % 2 ][ i / 2 ],
                                              _slices[ i % 2 ][ i / 2 ].m_x,
                                              _slices[ i % 2 ][ i / 2 ].m_y );
            }
            ANIMATE_MAP = true;
        }

        // Check if a new slice got stepped onto
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 32 == 0 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 32 == 31 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 32 == 0 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 32 == 31 ) ) {

            u8 oldts1 = CUR_SLICE.m_data.m_tIdx1;
            u8 oldts2 = CUR_SLICE.m_data.m_tIdx2;

            _curX = ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1;
            _curY = ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1;
            // Update tileset, block and palette data

            u8 newts1 = CUR_SLICE.m_data.m_tIdx1;
            u8 newts2 = CUR_SLICE.m_data.m_tIdx2;

            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );

            ANIMATE_MAP = false;
            if( oldts1 != newts1 && oldts2 != newts2 ) {
                dmaCopy( CUR_SLICE.m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );
            } else if( oldts2 != newts2 ) {
                dmaCopy( &CUR_SLICE.m_tileSet.m_tiles[ MAX_TILES_PER_TILE_SET ],
                         &tileMemory[ MAX_TILES_PER_TILE_SET * 32 ], MAX_TILES_PER_TILE_SET * 32 );
            }

            // for palettes, the unchanged day-time pal comes first
            u8 currDT = ( getCurrentDaytime( ) + 3 ) % 5;
            if( ( currentData( ).m_mapType & INSIDE ) || ( currentData( ).m_mapType & CAVE ) ) {
                currDT = 0;
            }
            dmaCopy( CUR_SLICE.m_pals + currDT * 16, BG_PALETTE, 512 - 32 );
            BG_PALETTE[ 0 ] = 0;
            ANIMATE_MAP     = true;

#ifdef DESQUID_MORE
            char buffer[ 100 ];
            snprintf( buffer, 99, "Switch Slice to (%d, %d)", _curX, _curY );
            IO::printMessage( buffer );
#endif
        }

        switch( p_direction ) {
        case UP: {
            u16 ty  = _cy - 8;
            u16 mnx = _cx - 15;
            for( u16 x = ( _lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, _lastrow );
            _lastrow = ( _lastrow + NUM_ROWS - 1 ) % NUM_ROWS;
            break;
        }
        case LEFT: {
            u16 tx  = _cx - 15;
            u16 mny = _cy - 8;
            for( u16 y = ( _lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), _lastcol, y );
            _lastcol = ( _lastcol + NUM_COLS - 1 ) % NUM_COLS;
            break;
        }
        case DOWN: {
            _lastrow = ( _lastrow + 1 ) % NUM_ROWS;
            u16 ty   = _cy + 7;
            u16 mnx  = _cx - 15;
            for( u16 x = ( _lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, _lastrow );
            break;
        }
        case RIGHT: {
            _lastcol = ( _lastcol + 1 ) % NUM_COLS;
            u16 tx   = _cx + 16;
            u16 mny  = _cy - 8;
            for( u16 y = ( _lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), _lastcol, y );
            break;
        }
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows ) {
        for( u8 i = 0; i < 4; ++i ) {
            if( i == IO::bg3 && !_weatherFollow ) { continue; }
            bgScroll( i, dir[ p_direction ][ 0 ], dir[ p_direction ][ 1 ] );
        }
        bgUpdate( );
        _mapSprites.moveCamera( p_direction, 1, !p_updatePlayer );
        if( p_autoLoadRows
            && ( ( dir[ p_direction ][ 0 ]
                   && ( ( bgState[ 1 ].scrollX >> 8 ) - !!dir[ p_direction ][ 0 ] + 9 ) % 16 == 0 )
                 || ( dir[ p_direction ][ 1 ]
                      && ( ( bgState[ 1 ].scrollY >> 8 ) - !!dir[ p_direction ][ 1 ] + 9 ) % 16
                             == 0 ) ) ) {
            loadNewRow( p_direction, p_updatePlayer );
        }
    }

    void mapDrawer::loadSlice( direction p_direction ) {
        auto mx = CUR_SLICE.m_x + dir[ p_direction ][ 0 ],
             my = CUR_SLICE.m_y + dir[ p_direction ][ 1 ];

        constructSlice( _currentBank, _tileset, SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                        &_slices[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                                [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                        &_data[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                              [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                        _slices );
        runLevelScripts( _data[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                              [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                         mx, my );

        auto& neigh = _slices[ ( _curX + !dir[ p_direction ][ 0 ] ) & 1 ]
                             [ ( _curY + !dir[ p_direction ][ 1 ] ) & 1 ];
        mx = neigh.m_x + dir[ p_direction ][ 0 ];
        my = neigh.m_y + dir[ p_direction ][ 1 ];
        constructSlice( _currentBank, _tileset, SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                        &_slices[ _curX ^ 1 ][ _curY ^ 1 ], &_data[ _curX ^ 1 ][ _curY ^ 1 ],
                        _slices );
        runLevelScripts( _data[ _curX ^ 1 ][ _curY ^ 1 ], mx, my );
    }

    u16 mapDrawer::getCurrentLocationId( ) const {
        if( FSDATA.isOWMap( SAVE::SAV.getActiveFile( ).m_currentMap ) ) [[likely]] {
            if( MAP_LOCATIONS.m_bank != SAVE::SAV.getActiveFile( ).m_currentMap
                || !MAP_LOCATIONS.m_good ) {
                FS::loadLocationData( SAVE::SAV.getActiveFile( ).m_currentMap );
            }

            return MAP_LOCATIONS.get(
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY / mapLocation::MAP_LOCATION_RES,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX / mapLocation::MAP_LOCATION_RES );
        }

        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;

        const mapData& mdata = currentData( );
        return mdata.m_locationIds[ cury / 8 ][ curx / 8 ];
    }
} // namespace MAP
