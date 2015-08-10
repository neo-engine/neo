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

namespace MAP {
    mapDrawer* curMap;
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    MapBlockAtom mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = p_x / SIZE == _slices[ _curX ][ _curY ]->m_x,
            y = p_y / SIZE == _slices[ _curX ][ _curY ]->m_y;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]->m_blocks[ p_x % SIZE ][ p_y % SIZE ];
    }

    int bgs[ 4 ];

    int lastrow, lastcol;
    int lastbx, lastby;
    u16* mapMemory[ 4 ];
    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            videoSetMode( MODE_0_2D /*| DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE |
                          DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            //REG_BG0CNT = BG_32x32 | BG_COLOR_16 |  BG_MAP_BASE(0) | BG_TILE_BASE(1)|  BG_PRIORITY(0);
            //REG_BG1CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(1) | BG_TILE_BASE(1)| BG_PRIORITY(1);
            //REG_BG2CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(2) | BG_TILE_BASE(1)| BG_PRIORITY(2);
            //REG_BG3CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(3) | BG_TILE_BASE(1)| BG_PRIORITY(3);

            for( u8 i = 1; i < 4; ++i ) {
                bgs[ i ] = bgInit( i, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetPriority( bgs[ i ], i );
                bgScroll( bgs[ i ], 128, 32 );
            }

            IO::Top = *consoleInit(&IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 5, true ,true);

            u8* tileMemory = (u8*)BG_TILE_RAM( 1 );

            for( u16 i = 0; i < 1024; ++i )
                swiCopy( _slices[ _curX ][ _curY ]->m_tileSet.m_blocks[ i ].m_tile, tileMemory + i * 32, 16 );

            dmaCopy( _slices[ _curX ][ _curY ]->m_pals, BG_PALETTE, 512 );
            for( u8 i = 1; i < 4; ++i )  mapMemory[ i ] = (u16*)BG_MAP_RAM( 2 * i - 1 );
        }

        lastrow = 31;
        lastcol = 15;

        int c = 0;
        p_globX += 0;
        p_globY += 10;


        lastbx = p_globX;
        lastby = p_globY;

        for( u8 i = 0; i < 2; ++i ) {
            for( s16 x = p_globX; x < p_globX + 17; x++ ) {
                for( s16 y = p_globY; y < p_globY + 17; y++ ) {
                    u8 toplayer = 1, bottomlayer = 3;

                    Block acBlock = _slices[ _curX ][ _curY ]->m_blockSet.m_blocks[ at( x, y ).m_blockidx ];
#ifdef DEBUG
                    consoleSelect(&IO::Top);
                    consoleSetWindow( &IO::Top, 2 * ( y - p_globX ) - 1, 2 * ( x - p_globY ) - 1, 2, 2 );
                    printf("%i",acBlock.m_topbehave);
#endif

                    if( acBlock.m_topbehave == 0x10 ) {
                        if( x > p_globY && y > p_globX )
                            mapMemory[ toplayer + 1 ][ c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
                        if( x > p_globY && y < p_globX + 16 )
                            mapMemory[ toplayer + 1 ][ c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
                        if( x < p_globY + 16 && y > p_globX )
                            mapMemory[ toplayer + 1 ][ c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
                        if( x < p_globY + 16 && y < p_globX + 16 )
                            mapMemory[ toplayer + 1 ][ c ] = acBlock.m_top[ 1 ][ 1 ];

                        if( x > p_globY && y > p_globX )
                            mapMemory[ toplayer ][ c - 33 ] = 0;
                        if( x > p_globY && y < p_globX + 16 )
                            mapMemory[ toplayer ][ c - 32 ] = 0;
                        if( x < p_globY + 16 && y > p_globX )
                            mapMemory[ toplayer ][ c - 1 ] = 0;
                        if( x < p_globY + 16 && y < p_globX + 16 )
                            mapMemory[ toplayer ][ c ] = 0;
                    } else {
                        if( x > p_globY && y > p_globX )
                            mapMemory[ toplayer ][ c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
                        if( x > p_globY && y < p_globX + 16 )
                            mapMemory[ toplayer ][ c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
                        if( x < p_globY + 16 && y > p_globX )
                            mapMemory[ toplayer ][ c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
                        if( x < p_globY + 16 && y < p_globX + 16 )
                            mapMemory[ toplayer ][ c ] = acBlock.m_top[ 1 ][ 1 ];

                        if( x > p_globY && y > p_globX )
                            mapMemory[ toplayer + 1 ][ c - 33 ] = 0;
                        if( x > p_globY && y < p_globX + 16 )
                            mapMemory[ toplayer + 1 ][ c - 32 ] = 0;
                        if( x < p_globY + 16 && y > p_globX )
                            mapMemory[ toplayer + 1 ][ c - 1 ] = 0;
                        if( x < p_globY + 16 && y < p_globX + 16 )
                            mapMemory[ toplayer + 1 ][ c ] = 0;
                    }

                    if( x > p_globY && y > p_globX )
                        mapMemory[ bottomlayer ][ c - 33 ] = acBlock.m_bottom[ 0 ][ 0 ];
                    if( x > p_globY && y < p_globX + 16 )
                        mapMemory[ bottomlayer ][ c - 32 ] = acBlock.m_bottom[ 0 ][ 1 ];
                    if( x < p_globY + 16 && y > p_globX )
                        mapMemory[ bottomlayer ][ c - 1 ] = acBlock.m_bottom[ 1 ][ 0 ];
                    if( x < p_globY + 16 && y < p_globX + 16 )
                        mapMemory[ bottomlayer ][ c ] = acBlock.m_bottom[ 1 ][ 1 ];

                    if( y < p_globX + 16 )
                        c += 2;
                }
                c += 32;
            }
            c -= 64;
            p_globX += 16;
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( mapSlice::direction p_direction ) {
        (void)p_direction;
    }

    void mapDrawer::loatSlice( u8 p_x, u8 p_y, u16 p_sliceX, u16 p_sliceY ) {
        (void)p_sliceX;
        (void)p_sliceY;
        (void)p_x;
        (void)p_y;
    }
    void mapDrawer::loadSlice( mapSlice::direction p_direction ) {
        (void)p_direction;
    }

    void mapDrawer::handleWarp( ) { }
    void mapDrawer::handleWildPkmn( ) { }
    void mapDrawer::handleTrainer( ) { }

    mapDrawer::mapDrawer( u8 p_currentMap, mapObject& p_player )
        :m_player( p_player ) {
        _curX = _curY = 0;

        u16 mx = p_player.m_pos.m_posX, my = p_player.m_pos.m_posY;
        _slices[ _curX ][ _curY ] = constructSlice( p_currentMap, mx / SIZE, my / SIZE );
        _slices[ 1 - _curX ][ _curY ] = constructSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE );
        _slices[ _curX ][ 1 - _curY ] = constructSlice( p_currentMap, mx / SIZE, my / SIZE + currentHalf( my ) );
        _slices[ 1 - _curX ][ 1 - _curY ] = constructSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE + currentHalf( my ) );
    }

    void mapDrawer::draw( ) {
        draw( m_player.m_pos.m_posX, m_player.m_pos.m_posY, true );
    }

    bool mapDrawer::canMove( mapSlice::position p_start,
                             mapSlice::direction p_direction,
                             mapSlice::moveMode p_moveMode ) {
        (void)p_start;
        (void)p_direction;
        (void)p_moveMode;
        return false;
    }
    void mapDrawer::movePlayer( mapSlice::direction p_direction ) {
        (void)p_direction;
    }
    void mapDrawer::stopPlayer( mapSlice::direction p_direction ) {
        (void)p_direction;
    }
    void mapDrawer::changeMoveMode( mapSlice::moveMode p_newMode ) {
        m_player.m_movement = p_newMode;
    }

    u16  mapDrawer::getCurrentLocationId( ) const {
        return _slices[ _curX ][ _curY ]->m_location;
    }
}