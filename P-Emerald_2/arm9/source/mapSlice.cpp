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

#define assert( a ) if( !(a) ) IO::messageBox( "assert failed o.O" )
#endif

namespace MAP {
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
            char buffer[ 50 ];
            snprintf( buffer, 49, "Map %d/%d,%d does not exist.", p_map, p_y, p_x );
            IO::messageBox m( buffer );
            IO::NAV->draw( true );
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
            IO::NAV->draw( true );
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
        IO::NAV->draw( true );
        swiWaitForVBlank( );
#endif

        //Read the wild Pokémon data
        mapF = FS::open( MAP_PATH,
            ( toString( p_map )
              + "/" + toString( p_y )
              + "_" + toString( p_x ) ).c_str( ),
                         ".enc" );
        FS::read( mapF, res->m_pokemon, sizeof( std::pair<u16, u16> ), 3 * 5 * 5 );
        FS::close( mapF );

        //Read the first tileset
        mapF = FS::open( TILESET_PATH, tsidx1, ".ts" );
        FS::readTiles( mapF, res->m_tileSet.m_tiles );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx1, ".bvd" );
        FS::readBlocks( mapF, res->m_blockSet.m_blocks );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx1, ".p2l" );
        FS::readPal( mapF, res->m_pals );
        FS::close( mapF );

        // TODO: FIX THIS!
        mapF = FS::open( TILESET_PATH, tsidx1, ".anm" );
        res->m_tileSet.m_animationCount1 = FS::readAnimations( mapF, res->m_tileSet.m_animations );
        FS::close( mapF );

        //Read the second tileset

        mapF = FS::open( TILESET_PATH, tsidx2, ".ts" );
        FS::readTiles( mapF, res->m_tileSet.m_tiles, 512 );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx2, ".bvd" );
        FS::readBlocks( mapF, res->m_blockSet.m_blocks, 512 );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx2, ".p2l" );
        FS::readPal( mapF, res->m_pals + 6 );
        FS::close( mapF );

        mapF = FS::open( TILESET_PATH, tsidx2, ".anm" );
        res->m_tileSet.m_animationCount2 = FS::readAnimations( mapF, res->m_tileSet.m_animations + MAX_ANIM_PER_TILE_SET );
        FS::close( mapF );
        return res;
    }
}
