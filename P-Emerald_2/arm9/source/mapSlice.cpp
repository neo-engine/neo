/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.cpp
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2016
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
    sliceCache cache;

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
            swiWaitForVBlank( );
#endif
            //mapF = FS::open( MAP_PATH, "empty", ".map" );
            //if( !mapF )
            return 0;
        }
        std::unique_ptr<mapSlice> res = std::unique_ptr<mapSlice>( new mapSlice );
#ifdef DEBUG
        if( !res ) {
            IO::messageBox( "Not enough memory :(" );
            IO::drawSub( true );
        }
#endif

        FS::readNop( mapF, 8 );
        res->m_x = p_x;
        res->m_y = p_y;

        u8 tsidx1, tsidx2;
        FS::read( mapF, &tsidx1, sizeof( u8 ), 1 );
        FS::readNop( mapF, 3 );
        FS::read( mapF, &tsidx2, sizeof( u8 ), 1 );
        FS::readNop( mapF, 3 );

        u8 b1, b2;
        FS::read( mapF, &b1, sizeof( u8 ), 1 );
        FS::read( mapF, &b2, sizeof( u8 ), 1 );
        FS::readNop( mapF, 2 );

        FS::read( mapF, res->m_blocks, sizeof( mapBlockAtom ), b1 * b2 ); //Border blocks

        FS::read( mapF, res->m_blocks, sizeof( mapBlockAtom ), SIZE * SIZE );
        FS::close( mapF );

#ifdef DEBUG__
        sprintf( buffer, "ts1 %d ts2 %d", tsidx1, tsidx2 );
        IO::messageBox a( buffer );
        IO::drawSub( true );
        swiWaitForVBlank( );
#endif

        //Read the wild Pkoemon data
        mapF = FS::open( MAP_PATH,
                         ( toString( p_map )
                           + "/" + toString( p_y )
                           + "_" + toString( p_x ) ).c_str( ),
                         ".enc" );
        FS::read( mapF, res->m_pokemon, sizeof( std::pair<u16, u16> ), 3 * 5 * 4 );
        FS::close( mapF );

        //Read the first tileset
        s8 ind;
        if( ( ind = cache.get( tsidx1 ) ) == -1 ) {
            ind = cache.set( tsidx1 );
            if( cache.m_tiles[ ind ] )
                delete cache.m_tiles[ ind ];
            mapF = FS::open( TILESET_PATH, tsidx1, ".ts" );
            cache.m_tiles[ ind ] = new tile[ 512 ];
            FS::readTiles( mapF, cache.m_tiles[ ind ] );
            FS::close( mapF );

            if( cache.m_blocks[ ind ] )
                delete cache.m_blocks[ ind ];
            mapF = FS::open( TILESET_PATH, tsidx1, ".bvd" );
            cache.m_blocks[ ind ] = new block[ 512 ];
            FS::readblocks( mapF, cache.m_blocks[ ind ] );
            FS::close( mapF );

            if( cache.m_palettes[ ind ] )
                delete  cache.m_palettes[ ind ];
            mapF = FS::open( TILESET_PATH, tsidx1, ".p2l" );
            cache.m_palettes[ ind ] = new palette[ 6 ];
            FS::readPal( mapF, cache.m_palettes[ ind ] );
            FS::close( mapF );
        }
        res->m_tileSet.m_tiles1 = cache.m_tiles[ ind ];
        res->m_blockSet.m_blocks1 = cache.m_blocks[ ind ];
        memcpy( res->m_pals, cache.m_palettes[ ind ], sizeof( palette ) * 6 );
        //sprintf( buffer, "nitro:/MAPS/TILESETS/%i.anm", tsidx1 );
        //FS::readAnimations( fopen( buffer, "rb" ), m_animations );

        //Read the second tileset

        if( ( ind = cache.get( tsidx2 ) ) == -1 ) {
            ind = cache.set( tsidx2 );
            if( cache.m_tiles[ ind ] )
                delete cache.m_tiles[ ind ];
            mapF = FS::open( TILESET_PATH, tsidx2, ".ts" );
            cache.m_tiles[ ind ] = new tile[ 512 ];
            FS::readTiles( mapF, cache.m_tiles[ ind ] );
            FS::close( mapF );

            if( cache.m_blocks[ ind ] )
                delete cache.m_blocks[ ind ];
            mapF = FS::open( TILESET_PATH, tsidx2, ".bvd" );
            cache.m_blocks[ ind ] = new block[ 512 ];
            FS::readblocks( mapF, cache.m_blocks[ ind ] );
            FS::close( mapF );

            if( cache.m_palettes[ ind ] )
                delete  cache.m_palettes[ ind ];
            mapF = FS::open( TILESET_PATH, tsidx2, ".p2l" );
            cache.m_palettes[ ind ] = new palette[ 6 ];
            FS::readPal( mapF, cache.m_palettes[ ind ] );
            FS::close( mapF );
        }
        res->m_tileSet.m_tiles2 = cache.m_tiles[ ind ];
        res->m_blockSet.m_blocks2 = cache.m_blocks[ ind ];
        memcpy( res->m_pals + 6, cache.m_palettes[ ind ], sizeof( palette ) * 6 );

        //sprintf( buffer, "nitro:/MAPS/TILESETS/%i.anm", tsidx2 );
        //readAnimations( fopen( buffer, "rb" ), m_animations );
        return res;
    }
}