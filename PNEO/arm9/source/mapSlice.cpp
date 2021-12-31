/*
Pokémon neo
------------------------------

file        : mapSlice.cpp
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#include <cstring>
#include <map>
#include <string>
#include "mapSlice.h"
#include "defines.h"
#include "fs.h"
#include "nav.h"

#ifdef DESQUID
#include "uio.h"
#endif

namespace MAP {
    void constructSlice( FILE* p_f, FILE* p_tileset, u8 p_map, u16 p_x, u16 p_y, mapSlice* p_result,
                         mapData* p_resultData, mapSlice p_cache[ 2 ][ 2 ] ) {
        bool mapExists = true;
        if( !p_f ) { p_f = FS::openBank( p_map ); }
        if( !p_tileset ) { p_tileset = FS::openTileSet( ); }

#ifdef DESQUID
        if( !p_result ) {
            NAV::printMessage( "Bad things happened, aborting", MSG_INFO );
            return;
        }
#endif

        if( !p_f || !p_tileset ) {
#ifdef DESQUID
            char buffer[ 50 ];
            snprintf( buffer, 49, "Map %d/%d,%d does not exist or TS broken", p_map, p_y, p_x );
            NAV::printMessage( buffer, MSG_INFO );
            swiWaitForVBlank( );
#endif
            mapExists = false;
            std::memset( p_result->m_data.m_blocks, 0, SIZE * SIZE * sizeof( mapBlockAtom ) );

            p_result->m_data.m_tIdx1 = 255;
            p_result->m_data.m_tIdx2 = 255;
        }

        u16  oldts1   = p_result->m_data.m_tIdx1;
        u16  oldts2   = p_result->m_data.m_tIdx2;
        bool reloadTs = false;
        if( !p_result->m_loaded ) { reloadTs = mapExists; }

        if( mapExists ) {
            auto res           = FS::readMapSliceAndData( p_f, p_result, p_resultData, p_x, p_y );
            p_result->m_loaded = !res;
        } else {
            return;
        }

        u16 tsidx1 = p_result->m_data.m_tIdx1;
        u16 tsidx2 = p_result->m_data.m_tIdx2;

        p_result->m_data.m_tIdx1 = oldts1;
        p_result->m_data.m_tIdx2 = oldts2;

        // Read the first tileset
        if( reloadTs || tsidx1 != oldts1 ) {
            bool found = false;
            if( !reloadTs && p_cache ) {
                for( u8 i = 0; i < 2; ++i )
                    for( u8 j = 0; j < 2; ++j )
                        if( p_cache[ i ][ j ].m_loaded
                            && p_cache[ i ][ j ].m_data.m_tIdx1 == tsidx1 ) {
                            found = true;
                            memcpy( p_result->m_tileSet.m_tiles,
                                    p_cache[ i ][ j ].m_tileSet.m_tiles,
                                    MAX_TILES_PER_TILE_SET * sizeof( tile ) );
                            memcpy( p_result->m_blockSet.m_blocks,
                                    p_cache[ i ][ j ].m_blockSet.m_blocks,
                                    MAX_BLOCKS_PER_TILE_SET * sizeof( block ) );

                            for( u8 k = 0; k < 5; ++k ) {
                                memcpy( p_result->m_pals + k * 16,
                                        p_cache[ i ][ j ].m_pals + k * 16, 6 * sizeof( palette ) );
                            }
                            break;
                        }
            }
            if( !found ) {
                if( FS::seekTileSet( p_tileset, tsidx1 ) ) {
                    FS::readTiles( p_tileset, p_result->m_tileSet.m_tiles );
                    FS::readBlocks( p_tileset, p_result->m_blockSet.m_blocks );
                    for( u8 i = 0; i < DAYTIMES; ++i ) {
                        FS::readPal( p_tileset, p_result->m_pals + i * 16, 8 );
                    }
                }
            }
        }
        // Read the second tileset
        if( reloadTs || tsidx2 != oldts2 ) {
            bool found = false;
            if( !reloadTs && p_cache ) {
                for( u8 i = 0; i < 2; ++i )
                    for( u8 j = 0; j < 2; ++j )
                        if( p_cache[ i ][ j ].m_loaded
                            && p_cache[ i ][ j ].m_data.m_tIdx2 == tsidx2 ) {
                            found = true;
                            memcpy( p_result->m_tileSet.m_tiles + MAX_TILES_PER_TILE_SET,
                                    p_cache[ i ][ j ].m_tileSet.m_tiles + MAX_TILES_PER_TILE_SET,
                                    MAX_TILES_PER_TILE_SET * sizeof( tile ) );
                            memcpy( p_result->m_blockSet.m_blocks + MAX_BLOCKS_PER_TILE_SET,
                                    p_cache[ i ][ j ].m_blockSet.m_blocks + MAX_BLOCKS_PER_TILE_SET,
                                    MAX_BLOCKS_PER_TILE_SET * sizeof( block ) );

                            for( u8 k = 0; k < 5; ++k ) {
                                memcpy( p_result->m_pals + 6 + k * 16,
                                        p_cache[ i ][ j ].m_pals + 6 + k * 16,
                                        8 * sizeof( palette ) );
                            }
                            break;
                        }
            }
            if( !found ) {
                if( FS::seekTileSet( p_tileset, tsidx2 ) ) {
                    FS::readTiles( p_tileset, p_result->m_tileSet.m_tiles, 512 );
                    FS::readBlocks( p_tileset, p_result->m_blockSet.m_blocks, 512 );
                    for( u8 i = 0; i < DAYTIMES; ++i ) {
                        FS::readPal( p_tileset, p_result->m_pals + 6 + i * 16, 8 );
                    }
                }
            }
        }
        p_result->m_data.m_tIdx1 = tsidx1;
        p_result->m_data.m_tIdx2 = tsidx2;
    }
} // namespace MAP
