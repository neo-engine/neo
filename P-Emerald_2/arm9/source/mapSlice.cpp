/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.cpp
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2017
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

#include <map>
#include <string>
#include "mapSlice.h"
#include "defines.h"
#include "fs.h"
#ifdef DEBUG
#include "messageBox.h"
#include "uio.h"

#define assert( a ) \
    if( !( a ) ) IO::messageBox( "assert failed o.O" )
#endif

namespace MAP {
    void constructSlice( u8 p_map, u16 p_x, u16 p_y, std::unique_ptr<mapSlice>& p_result ) {
        FILE* mapF = FS::open(
            MAP_PATH,
            ( toString( p_map ) + "/" + toString( p_y ) + "_" + toString( p_x ) ).c_str( ),
            ".map" );
        if( !mapF )
            mapF = FS::open( MAP_PATH, ( toString( p_map ) + "/BORDER/" + toString( p_y ) + "_"
                                         + toString( p_x ) )
                                           .c_str( ),
                             ".map" );
        if( !mapF ) {
#ifdef DEBUG
            char buffer[ 50 ];
            snprintf( buffer, 49, "Map %d/%d,%d does not exist.", p_map, p_y, p_x );
            IO::messageBox m( buffer );
            IO::NAV->draw( true );
            swiWaitForVBlank( );
#endif
            mapF = FS::open( MAP_PATH, "empty", ".map" );
            if( !mapF ) return;
        }
        p_result = std::unique_ptr<mapSlice>( new mapSlice );
#ifdef DEBUG
        if( !p_result ) {
            IO::messageBox( "Not enough memory :(" );
            IO::NAV->draw( true );
        }
#endif

        FS::readNop( mapF, 8 );
        p_result->m_x = p_x;
        p_result->m_y = p_y;

        u8 tsidx1, tsidx2;
        FS::read( mapF, &tsidx1, sizeof( u8 ), 1 );
        FS::readNop( mapF, 3 );
        FS::read( mapF, &tsidx2, sizeof( u8 ), 1 );
        FS::readNop( mapF, 3 );

        u8 b1, b2;
        FS::read( mapF, &b1, sizeof( u8 ), 1 );
        FS::read( mapF, &b2, sizeof( u8 ), 1 );
        FS::readNop( mapF, 2 );

        FS::read( mapF, p_result->m_blocks, sizeof( mapBlockAtom ), b1 * b2 ); // Border blocks

        FS::read( mapF, p_result->m_blocks, sizeof( mapBlockAtom ), SIZE * SIZE );
        FS::close( mapF );

        // Read the wild Pokémon data
        mapF = FS::open(
            MAP_PATH,
            ( toString( p_map ) + "/" + toString( p_y ) + "_" + toString( p_x ) ).c_str( ),
            ".enc" );
        FS::read( mapF, p_result->m_pokemon, sizeof( std::pair<u16, u16> ), 3 * 5 * 5 );
        FS::close( mapF );

        mapF = FS::open(
            MAP_PATH,
            ( toString( p_map ) + "/" + toString( p_y ) + "_" + toString( p_x ) ).c_str( ),
            ".ect" );
        FS::read( mapF, p_result->m_evtCnt, sizeof( u8 ), 32 * 32 );
        FS::close( mapF );

        // Read the first tileset
        mapF = FS::open( TILESET_PATH, tsidx1, ".ts" );
        FS::readTiles( mapF, p_result->m_tileSet.m_tiles );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx1, ".bvd" );
        FS::readBlocks( mapF, p_result->m_blockSet.m_blocks );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx1, ".p2l" );
        FS::readPal( mapF, p_result->m_pals );
        FS::close( mapF );

        // TODO: FIX THIS!
        mapF = FS::open( TILESET_PATH, tsidx1, ".anm" );
        p_result->m_tileSet.m_animationCount1
            = FS::readAnimations( mapF, p_result->m_tileSet.m_animations );
        FS::close( mapF );

        // Read the second tileset

        mapF = FS::open( TILESET_PATH, tsidx2, ".ts" );
        FS::readTiles( mapF, p_result->m_tileSet.m_tiles, 512 );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx2, ".bvd" );
        FS::readBlocks( mapF, p_result->m_blockSet.m_blocks, 512 );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx2, ".p2l" );
        FS::readPal( mapF, p_result->m_pals + 6 );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx2, ".anm" );
        p_result->m_tileSet.m_animationCount2
            = FS::readAnimations( mapF, p_result->m_tileSet.m_animations + MAX_ANIM_PER_TILE_SET );
        FS::close( mapF );
    }
}
