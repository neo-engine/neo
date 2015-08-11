/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.cpp
author      : Philip Wellnitz
description : Header file. See corresponding source file for details.

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

#include <string>
#include <map>
#include "mapSlice.h"
#include "fs.h"
#include "defines.h"
#ifdef DEBUG
#include "uio.h"
#include "messageBox.h"
#endif

namespace MAP {
    std::map<u8, Tile*> tiles;
    std::map<u8, Block*> blocks;
    std::map<u8, Palette*> palettes;

    std::unique_ptr<mapSlice> constructSlice( u8 p_map, u16 p_x, u16 p_y ) {
        FILE* mapF = FS::open( MAP_PATH,
                               ( toString( p_map )
                               + "/" + toString( p_y )
                               + "_" + toString( p_x ) ).c_str( ),
                               ".map" );
        if( !mapF ) {
#ifdef DEBUG
            sprintf( buffer, "Map %d/%d,%d does not exist.\n", p_map, p_x, p_y );
            IO::messageBox m( buffer );
            IO::drawSub( );
#endif
            return 0;
        }
        std::unique_ptr<mapSlice> res = std::unique_ptr<mapSlice>( new mapSlice );

        FS::readNop( mapF, 8 );
        res->m_x = p_x;
        res->m_y = p_y;

        u8 tsidx1, tsidx2;
        fread( &tsidx1, sizeof( u8 ), 1, mapF );
        FS::readNop( mapF, 3 );
        fread( &tsidx2, sizeof( u8 ), 1, mapF );
        FS::readNop( mapF, 3 );

        FS::readNop( mapF, 4 );
        FS::readNop( mapF, 8 ); //Border blocks will be ignored
        fread( res->m_blocks, sizeof( MapBlockAtom ), SIZE * SIZE, mapF );
        FS::close( mapF );

        //Read the first tileset
        if( !tiles.count( tsidx1 ) ) {
            mapF = FS::open( TILESET_PATH, tsidx1, ".ts" );
            tiles[ tsidx1 ] = new Tile[ 512 ];
            FS::readTiles( mapF, tiles[ tsidx1 ] );
            FS::close( mapF );
        }
        memcpy( res->m_tileSet.m_blocks, tiles[ tsidx1 ], 512 * sizeof( Tile ) );

        if( !blocks.count( tsidx1 ) ) {
            mapF = FS::open( TILESET_PATH, tsidx1, ".bvd" );
            blocks[ tsidx1 ] = new Block[ 512 ];
            FS::readBlocks( mapF, blocks[ tsidx1 ] );
            FS::close( mapF );
        }
        memcpy( res->m_blockSet.m_blocks, blocks[ tsidx1 ], 512 * sizeof( Block ) );

        if( !palettes.count( tsidx1 ) ) {
            mapF = FS::open( TILESET_PATH, tsidx1, ".p2l" );
            palettes[ tsidx1 ] = new Palette[ 6 ];
            FS::readPal( mapF, palettes[ tsidx1 ] );
            FS::close( mapF );
        }
        memcpy( res->m_pals, palettes[ tsidx1 ], sizeof( Palette ) * 6 );
        //sprintf( buffer, "nitro:/MAPS/TILESETS/%i.anm", tsidx1 );
        //FS::readAnimations( fopen( buffer, "rb" ), m_animations );

        //Read the second tileset

        if( !tiles.count( tsidx2 ) ) {
            mapF = FS::open( TILESET_PATH, tsidx2, ".ts" );
            tiles[ tsidx2 ] = new Tile[ 512 ];
            FS::readTiles( mapF, tiles[ tsidx2 ] );
            FS::close( mapF );
        }
        memcpy( res->m_tileSet.m_blocks + 512, tiles[ tsidx2 ], 512 * sizeof( Tile ) );

        if( !blocks.count( tsidx2 ) ) {
            mapF = FS::open( TILESET_PATH, tsidx2, ".bvd" );
            blocks[ tsidx2 ] = new Block[ 512 ];
            FS::readBlocks( mapF, blocks[ tsidx2 ] );
            FS::close( mapF );
        }
        memcpy( res->m_blockSet.m_blocks + 512, blocks[ tsidx2 ], 512 * sizeof( Block ) );

        if( !palettes.count( tsidx2 ) ) {
            mapF = FS::open( TILESET_PATH, tsidx2, ".p2l" );
            palettes[ tsidx2 ] = new Palette[ 6 ];
            FS::readPal( mapF, palettes[ tsidx2 ] );
            FS::close( mapF );
        }
        memcpy( res->m_pals + 6, palettes[ tsidx2 ], sizeof( Palette ) * 6 );

        //sprintf( buffer, "nitro:/MAPS/TILESETS/%i.anm", tsidx2 );
        //readAnimations( fopen( buffer, "rb" ), m_animations );
        return res;
    }

    bool mapSlice::canMove( position p_start, direction p_direction, moveMode p_moveMode ) {
        u16 nx = p_start.m_posX + dir[ p_direction ][ 0 ];
        u16 ny = p_start.m_posY + dir[ p_direction ][ 1 ];

        //Leaving a map shall always be possible
        if( nx / SIZE != p_start.m_posX / SIZE && nx / SIZE != m_x )
            return true;
        if( ny / SIZE != p_start.m_posY / SIZE && ny / SIZE != m_y )
            return true;

        //Gather data about the source block
        u8 lstMoveData, lstBehave;
        if( nx / SIZE != p_start.m_posX / SIZE
            || ny / SIZE != p_start.m_posY / SIZE ) {
            lstMoveData = 0;
            lstBehave = 0;
        } else {
            lstMoveData = m_blocks[ p_start.m_posX % SIZE ][ p_start.m_posY % SIZE ].m_movedata;

            auto lstBlock = m_blockSet.m_blocks[ m_blocks[ p_start.m_posX % SIZE ][ p_start.m_posY % SIZE ].m_blockidx ];
            lstBehave = lstBlock.m_bottombehave;
        }

        //Gather data about the destination block
        u8 curMoveData, curBehave;
        curMoveData = m_blocks[ nx % SIZE ][ ny % SIZE ].m_movedata;

        auto curBlock = m_blockSet.m_blocks[ m_blocks[ nx % SIZE ][ ny % SIZE ].m_blockidx ];
        curBehave = curBlock.m_bottombehave;

        //Check for special block attributes
        switch( lstBehave ) {
            case 0x30:
                if( p_direction == direction::RIGHT )
                    return false;
                break;
            case 0x31:
                if( p_direction == direction::LEFT )
                    return false;
                break;
            case 0x32:
                if( p_direction == direction::UP )
                    return false;
                break;
            case 0x33:
                if( p_direction == direction::DOWN )
                    return false;
                break;
            case 0x36:
                if( p_direction == direction::DOWN || p_direction == direction::LEFT )
                    return false;
                break;
            case 0x37:
                if( p_direction == direction::DOWN || p_direction == direction::RIGHT )
                    return false;
                break;
            case 0xa0:
                if( !( p_moveMode & WALK ) )
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
                return p_direction == direction::RIGHT;
            case 0x39: case 0x34:
                return p_direction == direction::LEFT;
            case 0x3a:
                return p_direction == direction::UP;
            case 0x3b:
                return p_direction == direction::DOWN;

            case 0xa0:
                if( !( p_moveMode & WALK ) )
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
        if( curMoveData == 4 && !( p_moveMode & SURF ) )
            return false;
        else if( curMoveData == 4 )
            return true;
        if( curMoveData == 0x0c && lstMoveData == 4 )
            return true;
        if( !curMoveData )
            return true;
        return curMoveData % 4 == 0 && curMoveData / 4 == p_start.m_posZ;
    }
}