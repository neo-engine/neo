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
#include "messageBox.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace MAP {
#define NUM_ROWS 16
#define NUM_COLS 32
#define MAP_BORDER 0x3f
    mapDrawer* curMap;
#define CUR_SLICE _slices[ _curX ][ _curY ]
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }


    MapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE->m_x ),
            y = ( p_y / SIZE != CUR_SLICE->m_y );
        return  _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]->m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    Block& mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE->m_x ),
            y = ( p_y / SIZE != CUR_SLICE->m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]->m_blockSet.m_blocks[ _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]->m_blocks[ p_y % SIZE ][ p_x % SIZE ].m_blockidx ];
    }

    u16 lastrow, //Row to be filled when extending the map to the top
        lastcol; //Column to be filled when extending the map to the left
    u16 cx, cy;  //Cameras's pos
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

    // Drawing of Maps and stuff

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            videoSetMode( MODE_0_2D/* | DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE |
                          DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            for( u8 i = 1; i < 4; ++i ) {
                bgInit( i, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetScroll( i, 120, 40 );
            }
            u8* tileMemory = (u8*)BG_TILE_RAM( 1 );

            for( u16 i = 0; i < 1024; ++i )
                swiCopy( CUR_SLICE->m_tileSet.m_blocks[ i ].m_tile, tileMemory + i * 32, 16 );
            dmaCopy( CUR_SLICE->m_pals, BG_PALETTE, 512 );
            for( u8 i = 1; i < 4; ++i ) {
                mapMemory[ i ] = (u16*)BG_MAP_RAM( 2 * i - 1 );
                bgSetPriority( i, i );
            }
        }

        lastrow = NUM_ROWS - 1;
        lastcol = NUM_COLS - 1;

        cx = p_globX;
        cy = p_globY;

        u16 mny = p_globY - 8;
        u16 mnx = p_globX - 15;

        for( u16 y = 0; y < NUM_ROWS; y++ )
            for( u16 x = 0; x < NUM_COLS; x++ ) {
#ifdef ___DEBUG
                if( ( mnx + x ) % SIZE == 0 || ( mny + y ) % SIZE == 0 )
                    loadBlock( CUR_SLICE->m_blockSet.m_blocks[ 0 ], x, y );
                else
#endif
                    loadBlock( at( mnx + x, mny + y ), x, y );
            }
        bgUpdate( );
    }

    void mapDrawer::draw( ) {
        draw( _player.m_pos.m_posX, _player.m_pos.m_posY, true ); //Draw the map

        IO::initOAMTable( false );
        drawPlayer( ); //Draw the player
        drawObjects( ); //Draw NPCs / stuff
    }

    void mapDrawer::drawPlayer( ) {
        _sprites[ 0 ] = _player.show( 128 - 8, 96 - 24, 0, 0, 0 );
        _spritePos[ _player.m_id ] = 0;
        _entriesUsed |= ( 1 << 0 );
    }

    void mapDrawer::drawObjects( ) {

    }

    void mapDrawer::loadNewRow( mapSlice::direction p_direction, bool p_updatePlayer ) {
        cx += dir[ p_direction ][ 0 ];
        cy += dir[ p_direction ][ 1 ];
#ifdef DEBUG
        assert( cx != _player.m_pos.m_posX || cy != _player.m_pos.m_posY );
#endif
        if( p_updatePlayer ) {
            _player.m_pos.m_posX = cx;
            _player.m_pos.m_posY = cy;
        }

        //Check if a new slice should be loaded
        if( ( dir[ p_direction ][ 0 ] == 1 && cx % 32 == 15 )
            || ( dir[ p_direction ][ 0 ] == -1 && cx % 32 == 14 )
            || ( dir[ p_direction ][ 1 ] == 1 && cy % 32 == 16 )
            || ( dir[ p_direction ][ 1 ] == -1 && cy % 32 == 15 ) ) {
            loadSlice( p_direction );
#ifdef __DEBUG
            IO::messageBox m( "Load Slice" );
            IO::drawSub( );
#endif
        }
        //Check if a new slice got stepped onto
        if( ( dir[ p_direction ][ 0 ] == 1 && cx % 32 == 0 )
            || ( dir[ p_direction ][ 0 ] == -1 && cx % 32 == 31 )
            || ( dir[ p_direction ][ 1 ] == 1 && cy % 32 == 0 )
            || ( dir[ p_direction ][ 1 ] == -1 && cy % 32 == 31 ) ) {
            _curX = ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1;
            _curY = ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1;
            //Update tileset, block and palette data
            u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
            for( u16 i = 0; i < 1024; ++i )
                swiCopy( CUR_SLICE->m_tileSet.m_blocks[ i ].m_tile, tileMemory + i * 32, 16 );
            dmaCopy( CUR_SLICE->m_pals, BG_PALETTE, 512 );

#ifdef __DEBUG
            sprintf( buffer, "Switch Slice to (%d, %d)", _curX, _curY );
            IO::messageBox m( buffer );
            IO::drawSub( );
#endif
        }

        switch( p_direction ) {
            case MAP::mapSlice::UP: {
                u16 ty = cy - 8;
                u16 mnx = cx - 15;
                for( u16 x = ( lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS; x = ( x + 1 ) % NUM_COLS, ++xp )
                    loadBlock( at( xp, ty ), x, lastrow );
                lastrow = ( lastrow + NUM_ROWS - 1 ) % NUM_ROWS;
                break;
            }
            case MAP::mapSlice::LEFT: {
                u16 tx = cx - 15;
                u16 mny = cy - 8;
                for( u16 y = ( lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS; y = ( y + 1 ) % NUM_ROWS, ++yp )
                    loadBlock( at( tx, yp ), lastcol, y );
                lastcol = ( lastcol + NUM_COLS - 1 ) % NUM_COLS;
                break;
            }
            case MAP::mapSlice::DOWN: {
                lastrow = ( lastrow + 1 ) % NUM_ROWS;
                u16 ty = cy + 7;
                u16 mnx = cx - 15;
                for( u16 x = ( lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS; x = ( x + 1 ) % NUM_COLS, ++xp )
                    loadBlock( at( xp, ty ), x, lastrow );
                break;
            }
            case MAP::mapSlice::RIGHT: {
                lastcol = ( lastcol + 1 ) % NUM_COLS;
                u16 tx = cx + 16;
                u16 mny = cy - 8;
                for( u16 y = ( lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS; y = ( y + 1 ) % NUM_ROWS, ++yp )
                    loadBlock( at( tx, yp ), lastcol, y );
                break;
            }
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( mapSlice::direction p_direction, bool p_updatePlayer, bool p_autoLoadRows ) {
        for( u8 i = 1; i < 4; ++i )
            bgScroll( i, dir[ p_direction ][ 0 ], dir[ p_direction ][ 1 ] );
        bgUpdate( );

        if( p_autoLoadRows
            && ( ( dir[ p_direction ][ 0 ] && ( ( bgState[ 1 ].scrollX >> 8 ) - dir[ p_direction ][ 0 ] + 16 ) % 16 == 0 )
            || ( dir[ p_direction ][ 1 ] && ( ( bgState[ 1 ].scrollY >> 8 ) - dir[ p_direction ][ 1 ] + 16 ) % 16 == 0 ) ) )
            loadNewRow( p_direction, p_updatePlayer );
    }

    void mapDrawer::loadSlice( mapSlice::direction p_direction ) {
        _slices[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ][ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ]
            = constructSlice( _curMap, CUR_SLICE->m_x + dir[ p_direction ][ 0 ], CUR_SLICE->m_y + dir[ p_direction ][ 1 ] );

        auto& neigh = _slices[ ( _curX + !dir[ p_direction ][ 0 ] ) & 1 ][ ( _curY + !dir[ p_direction ][ 1 ] ) & 1 ];
        _slices[ _curX ^ 1 ][ _curY ^ 1 ]
            = constructSlice( _curMap, neigh->m_x + dir[ p_direction ][ 0 ], neigh->m_y + dir[ p_direction ][ 1 ] );
    }

    void mapDrawer::handleWarp( ) { }
    void mapDrawer::handleWildPkmn( ) { }
    void mapDrawer::handleTrainer( ) { }

    mapDrawer::mapDrawer( u8 p_currentMap, mapObject& p_player )
        : _curX( 0 ), _curY( 0 ), _curMap( p_currentMap ), _player( p_player ), _entriesUsed( 0 ) {
        u16 mx = p_player.m_pos.m_posX, my = p_player.m_pos.m_posY;
        _slices[ _curX ][ _curY ] = constructSlice( p_currentMap, mx / SIZE, my / SIZE );
        _slices[ _curX ^ 1 ][ _curY ] = constructSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE );
        _slices[ _curX ][ _curY ^ 1 ] = constructSlice( p_currentMap, mx / SIZE, my / SIZE + currentHalf( my ) );
        _slices[ _curX ^ 1 ][ _curY ^ 1 ] = constructSlice( p_currentMap, mx / SIZE + currentHalf( mx ), my / SIZE + currentHalf( my ) );
    }

    // Movement stuff

    bool mapDrawer::canMove( mapSlice::position p_start,
                             mapSlice::direction p_direction,
                             mapSlice::moveMode p_moveMode ) {
        u16 nx = p_start.m_posX + dir[ p_direction ][ 0 ];
        u16 ny = p_start.m_posY + dir[ p_direction ][ 1 ];

        //Gather data about the source block
        u8 lstMoveData, lstBehave;
        if( nx / SIZE != p_start.m_posX / SIZE
            || ny / SIZE != p_start.m_posY / SIZE ) {
            lstMoveData = 0;
            lstBehave = 0;
        } else {
            lstMoveData = atom( p_start.m_posX, p_start.m_posY ).m_movedata;

            auto lstBlock = at( p_start.m_posX, p_start.m_posY );
            lstBehave = lstBlock.m_bottombehave;
        }

        //Gather data about the destination block
        u8 curMoveData, curBehave;
        curMoveData = atom( nx, ny ).m_movedata;

        auto curBlock = at( nx, ny );
        curBehave = curBlock.m_bottombehave;

        //Check for special block attributes
        switch( lstBehave ) {
            case 0x30:
                if( p_direction == mapSlice::direction::RIGHT )
                    return false;
                break;
            case 0x31:
                if( p_direction == mapSlice::direction::LEFT )
                    return false;
                break;
            case 0x32:
                if( p_direction == mapSlice::direction::UP )
                    return false;
                break;
            case 0x33:
                if( p_direction == mapSlice::direction::DOWN )
                    return false;
                break;
            case 0x36:
                if( p_direction == mapSlice::direction::DOWN || p_direction == mapSlice::direction::LEFT )
                    return false;
                break;
            case 0x37:
                if( p_direction == mapSlice::direction::DOWN || p_direction == mapSlice::direction::RIGHT )
                    return false;
                break;
            case 0xa0:
                if( !( p_moveMode & mapSlice::WALK ) )
                    return false;
                break;
            case 0xc0:
                if( p_direction % 2 )
                    return false;
                break;
            case 0xc1:
                if( p_direction % 2 == 0 )
                    return false;
                break;
            default:
                break;
        }
        switch( curBehave ) {
            //Jumpy stuff
            case 0x38: case 0x35:
                return p_direction == mapSlice::direction::RIGHT;
            case 0x39: case 0x34:
                return p_direction == mapSlice::direction::LEFT;
            case 0x3a:
                return p_direction == mapSlice::direction::UP;
            case 0x3b:
                return p_direction == mapSlice::direction::DOWN;

            case 0xa0:
                if( !( p_moveMode & mapSlice::WALK ) )
                    return false;
                break;
            case 0xc0:
                if( p_direction % 2 )
                    return false;
                break;
            case 0xc1:
                if( p_direction % 2 == 0 )
                    return false;
                break;
            case 0xd3: case 0xd4:
            case 0xd5: case 0xd6:
            case 0xd7:
                return false;
            default:
                break;
        }

        //Check for movedata stuff
        if( curMoveData % 4 == 1 )
            return false;
        if( curMoveData == 4 && !( p_moveMode & mapSlice::SURF ) )
            return false;
        else if( curMoveData == 4 )
            return true;
        if( curMoveData == 0x0c && lstMoveData == 4 )
            return true;
        if( !curMoveData || !lstMoveData )
            return true;
        if( curMoveData == 0x3c )
            return true;
        return curMoveData % 4 == 0 && curMoveData / 4 == p_start.m_posZ;

    }
    void mapDrawer::movePlayer( mapSlice::direction p_direction ) {
        if( atom( _player.m_pos.m_posX + dir[ p_direction ][ 0 ],
            _player.m_pos.m_posY + dir[ p_direction ][ 1 ] ).m_movedata == MAP_BORDER ) {
            stopPlayer( mapSlice::direction( ( u8( p_direction ) + 2 ) % 4 ) );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            stopPlayer( );
            IO::messageBox m( "Ende der Kartendaten.\nKehr um, sonst\nverirrst du dich!", "PokéNav" );
            IO::drawSub( true );
            return;
        }
        //Check if the player's direction changed
        if( p_direction != _player.m_direction ) {
            _sprites[ _spritePos[ _player.m_id ] ].setFrame( getFrame( p_direction ) );
            _player.m_direction = p_direction;
        }
        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 )
                _sprites[ _spritePos[ _player.m_id ] ].nextFrame( );
            swiWaitForVBlank( );
        }
        _sprites[ _spritePos[ _player.m_id ] ].drawFrame( getFrame( p_direction ) );
        if( atom( _player.m_pos.m_posX, _player.m_pos.m_posY ).m_movedata > 4
            && atom( _player.m_pos.m_posX, _player.m_pos.m_posY ).m_movedata != 0x3c )
            _player.m_pos.m_posZ = atom( _player.m_pos.m_posX, _player.m_pos.m_posY ).m_movedata / 4;
    }
    void mapDrawer::stopPlayer( ) {
        _sprites[ _spritePos[ _player.m_id ] ].drawFrame( getFrame( _player.m_direction ) );
    }
    void mapDrawer::stopPlayer( mapSlice::direction p_direction ) {
        //Check if the player's direction changed
        if( p_direction != _player.m_direction ) {
            _player.m_direction = p_direction;
            _sprites[ _spritePos[ _player.m_id ] ].setFrame( getFrame( p_direction ) );
        }
        _sprites[ _spritePos[ _player.m_id ] ].nextFrame( );
    }
    void mapDrawer::changeMoveMode( mapSlice::moveMode p_newMode ) {
        _player.m_movement = p_newMode;
    }

    u16  mapDrawer::getCurrentLocationId( ) const {
        //TODO
        return 0;
    }
}