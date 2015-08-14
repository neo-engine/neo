/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.cpp
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
        if( !mapF ) mapF = FS::open( MAP_PATH,
                                     ( toString( p_map )
                                     + "/BORDER/" + toString( p_y )
                                     + "_" + toString( p_x ) ).c_str( ),
                                     ".map" );
        if( !mapF ) {
#ifdef DEBUG
            sprintf( buffer, "Map %d/%d,%d does not exist.", p_map, p_y, p_x );
            IO::messageBox m( buffer );
            IO::drawSub( true );
#endif
            //mapF = FS::open( MAP_PATH, "empty", ".map" );
            //if( !mapF )
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

        u8 b1, b2;
        fread( &b1, sizeof( u8 ), 1, mapF );
        fread( &b2, sizeof( u8 ), 1, mapF );
        FS::readNop( mapF, 2 );

        fread( res->m_blocks, sizeof( MapBlockAtom ), b1 * b2, mapF ); //Border blocks

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
}