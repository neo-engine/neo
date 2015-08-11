/*
Pokémon Emerald 2 Version
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine

Copyright (C) 2012 - 2015
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mapDrawer.h"
#include "uio.h"
#include "sprite.h"
#include "defines.h"

#ifdef DEBUG
#include "messageBox.h"
#endif

namespace MAP {
#define NUM_ROWS 16
#define NUM_COLS 32
    mapDrawer* curMap;
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    MapBlockAtom mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != _slices[ _curX ][ _curY ]->m_x ),
            y = ( p_y / SIZE != _slices[ _curX ][ _curY ]->m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]->m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }

    int bgs[ 4 ];

    u16 lastrow, //Row to be filled when extending the map to the top
        lastcol; //Column to be filled when extending the map to the left
    u16 cx, cy;
    u16* mapMemory[ 4 ];

    inline void loadBlock( Block p_curBlock, u32 p_memPos ) {
        u8 toplayer = 1, bottomlayer = 3;
        bool elevateTopLayer = p_curBlock.m_topbehave == 0x10;

        mapMemory[ toplayer ][ p_memPos ] = !elevateTopLayer * p_curBlock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 1 ] = !elevateTopLayer * p_curBlock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer ][ p_memPos + 32 ] = !elevateTopLayer * p_curBlock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 33 ] = !elevateTopLayer * p_curBlock.m_top[ 1 ][ 1 ];

        mapMemory[ toplayer + 1 ][ p_memPos ] = elevateTopLayer * p_curBlock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 1 ] = elevateTopLayer *  p_curBlock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 32 ] = elevateTopLayer *  p_curBlock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 33 ] = elevateTopLayer * p_curBlock.m_top[ 1 ][ 1 ];

        mapMemory[ bottomlayer ][ p_memPos ] = p_curBlock.m_bottom[ 0 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 1 ] = p_curBlock.m_bottom[ 0 ][ 1 ];
        mapMemory[ bottomlayer ][ p_memPos + 32 ] = p_curBlock.m_bottom[ 1 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 33 ] = p_curBlock.m_bottom[ 1 ][ 1 ];
    }
    inline void loadBlock( Block p_curBlock, u8 p_scrnX, u8 p_scrnY ) {
        u32 c = 64 * u32( p_scrnY ) + 2 * ( u32( p_scrnX ) % 16 );
        c += ( u32( p_scrnX ) / 16 ) * 1024;
        loadBlock( p_curBlock, c );
    }

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            videoSetMode( MODE_0_2D/* | DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE |
                          DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            for( u8 i = 1; i < 4; ++i ) {
                bgs[ i ] = bgInit( i, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetPriority( bgs[ i ], i );
                bgSetScroll( MAP::bgs[ i ], 120, 40 );
            }
            u8* tileMemory = (u8*)BG_TILE_RAM( 1 );

            for( u16 i = 0; i < 1024; ++i )
                swiCopy( _slices[ _curX ][ _curY ]->m_tileSet.m_blocks[ i ].m_tile, tileMemory + i * 32, 16 );

            dmaCopy( _slices[ _curX ][ _curY ]->m_pals, BG_PALETTE, 512 );
            for( u8 i = 1; i < 4; ++i )
                mapMemory[ i ] = (u16*)BG_MAP_RAM( 2 * i - 1 );
        }

        lastrow = NUM_ROWS - 1;
        lastcol = NUM_COLS - 1;

        cx = p_globX;
        cy = p_globY;

        u32 c = 0;
        for( u8 i = 0; i < 2; ++i, p_globX += 16 ) {
            s16 mny = p_globY - 8, mxy = p_globY + 7;
            s16 mnx = p_globX - 15, mxx = p_globX;

            for( s16 y = mny; y <= mxy; y++, c += 32 )
                for( s16 x = mnx; x <= mxx; x++, c += 2 )
                    loadBlock( _slices[ _curX ][ _curY ]->m_blockSet.m_blocks[ at( x, y ).m_blockidx ], c );
        }
        bgUpdate( );
    }

    void mapDrawer::loadNewRow( mapSlice::direction p_direction, bool p_updatePlayer ) {
        cx += dir[ p_direction ][ 0 ];
        cy += dir[ p_direction ][ 1 ];
        if( p_updatePlayer ) {
            _player.m_pos.m_posX = cx;
            _player.m_pos.m_posY = cy;
        }
        switch( p_direction ) {
            case MAP::mapSlice::UP: {
                u16 ty = cy - 8;
                s16 mnx = cx - 15;
                for( u16 x = 0, xp = mnx; x < NUM_COLS; ++x, ++xp )
                    loadBlock( _slices[ _curX ][ _curY ]->m_blockSet.m_blocks[ at( xp, ty ).m_blockidx ], x, lastrow );
                lastrow = ( lastrow + NUM_ROWS - 1 ) % NUM_ROWS;
                break;
            }
            case MAP::mapSlice::LEFT: {
                u16 tx = cx - 15;
                s16 mny = cy - 8;
                for( u16 y = 0, yp = mny; y < NUM_ROWS; ++y, ++yp )
                    loadBlock( _slices[ _curX ][ _curY ]->m_blockSet.m_blocks[ at( tx, yp ).m_blockidx ], lastcol, y );
                lastcol = ( lastcol + NUM_COLS - 1 ) % NUM_COLS;
                break;
            }
            case MAP::mapSlice::DOWN: {
                lastrow = ( lastrow + 1 ) % NUM_ROWS;
                u16 ty = cy + 7;
                s16 mnx = cx - 15;
                for( u16 x = 0, xp = mnx; x < NUM_COLS; ++x, ++xp )
                    loadBlock( _slices[ _curX ][ _curY ]->m_blockSet.m_blocks[ at( xp, ty ).m_blockidx ], x, lastrow );
                break;
            }
            case MAP::mapSlice::RIGHT: {
                lastcol = ( lastcol + 1 ) % NUM_COLS;
                u16 tx = cx + 16;
                s16 mny = cy - 8;
                for( u16 y = 0, yp = mny; y < NUM_ROWS; ++y, ++yp )
                    loadBlock( _slices[ _curX ][ _curY ]->m_blockSet.m_blocks[ at( tx, yp ).m_blockidx ], lastcol, y );
                break;
            }
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( mapSlice::direction p_direction, bool p_updatePlayer, bool p_autoLoadRows ) {
        for( u8 i = 1; i < 4; ++i )
            bgScroll( MAP::bgs[ i ], dir[ p_direction ][ 0 ], dir[ p_direction ][ 1 ] );
        bgUpdate( );

        if( p_autoLoadRows && ( bgScrollTable[ 1 ]->x + dir[ p_direction ][ 0 ] ) % 16 == 1 )
            loadNewRow( p_direction, p_updatePlayer );
        if( p_autoLoadRows && ( bgScrollTable[ 1 ]->y + dir[ p_direction ][ 1 ] ) % 16 == 1 )
            loadNewRow( p_direction, p_updatePlayer );

    }

    void mapDrawer::loadSlice( mapSlice::direction p_direction ) {
        (void)p_direction;
    }

    void mapDrawer::handleWarp( ) { }
    void mapDrawer::handleWildPkmn( ) { }
    void mapDrawer::handleTrainer( ) { }

    mapDrawer::mapDrawer( u8 p_currentMap, mapObject& p_player )
        :_player( p_player ) {
        _curX = _curY = 0;

        u16 mx = p_player.m_pos.m_posX, my = p_player.m_pos.m_posY;
        _slices[ _curX ][ _curY ] = constructSlice( p_currentMap, mx / SIZE, my / SIZE );
        _slices[ 1 - _curX ][ _curY ] = constructSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE );
        _slices[ _curX ][ 1 - _curY ] = constructSlice( p_currentMap, mx / SIZE, my / SIZE + currentHalf( my ) );
        _slices[ 1 - _curX ][ 1 - _curY ] = constructSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE + currentHalf( my ) );
    }

    void mapDrawer::draw( ) {
        draw( _player.m_pos.m_posX, _player.m_pos.m_posY, true );
    }

    bool mapDrawer::canMove( mapSlice::position p_start,
                             mapSlice::direction p_direction,
                             mapSlice::moveMode p_moveMode ) {
        (void)p_start;
        (void)p_direction;
        (void)p_moveMode;
        return true;
    }
    void mapDrawer::movePlayer( mapSlice::direction p_direction ) {
        moveCamera( p_direction, true );
    }
    void mapDrawer::stopPlayer( mapSlice::direction p_direction ) {
        (void)p_direction;
    }
    void mapDrawer::changeMoveMode( mapSlice::moveMode p_newMode ) {
        _player.m_movement = p_newMode;
    }

    u16  mapDrawer::getCurrentLocationId( ) const {
        return _slices[ _curX ][ _curY ]->m_location;
    }
}