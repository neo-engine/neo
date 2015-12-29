/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.h
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

#pragma once
#include <nds/ndstypes.h>
#include <vector>
#include <memory>
#include <cstring>

namespace MAP {
    const char* const MAP_PATH = "nitro:/MAPS/";
    const char* const TILESET_PATH = "nitro:/MAPS/TILESETS/";
    const u16 SIZE = 32;
    const s16 dir[ 4 ][ 2 ] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    typedef struct {
        u16 m_pal[ 16 ];
    } palette;
    typedef struct {
        u8 m_tile[ 32 ];
    } tile;
    //typedef struct{
    //    u16 tileidx      : 10;
    //    u8 vflip         :  1;
    //    u8 hflip         :  1;
    //    PalNo palno      :  4;
    //}blockAtom;
    typedef u16 blockAtom;

    class block {
    public:
        blockAtom   m_top[ 2 ][ 2 ];
        u8          m_topbehave;

        blockAtom   m_bottom[ 2 ][ 2 ];
        u8          m_bottombehave;
    };

    typedef struct {
        tile        *m_tiles1, *m_tiles2;
    } tileSet;
    typedef struct {
        block       *m_blocks1, *m_blocks2;
    } blockSet;

    typedef struct {
        u16         m_blockidx : 10;
        u8          m_movedata : 6;
    } mapBlockAtom;

    struct mapSlice {

        palette     m_pals[ 16 ];
        tileSet     m_tileSet;
        blockSet    m_blockSet;
        mapBlockAtom m_blocks[ 32 ][ 32 ]; // [ y ][ x ]
        u8          m_map;
        u16         m_x, m_y;

        std::pair<u16, u16> m_pokemon[ 3 * 5 * 4 ];
    };
    std::unique_ptr<mapSlice> constructSlice( u8 p_map, u16 p_x, u16 p_y );

    struct sliceCache {
#define MAX_CACHE_SIZE 4
        s8 m_indices[ MAX_CACHE_SIZE ];
        tile* m_tiles[ MAX_CACHE_SIZE ];
        block* m_blocks[ MAX_CACHE_SIZE ];
        palette* m_palettes[ MAX_CACHE_SIZE ];
        u8 m_nextFree;

        sliceCache( ) {
            m_nextFree = 0;
            memset( m_indices, -1, sizeof( m_indices ) );
        }

        u8 set( u8 p_index ) { //Gets the next free index
            u8 res = m_nextFree;
            m_indices[ m_nextFree ] = p_index;
            m_nextFree = ( m_nextFree + 1 ) % MAX_CACHE_SIZE;
            return res;
        }
        s8 get( u8 p_index ) { //Return the index or -1 if not found
            for( u8 i = 0; i < MAX_CACHE_SIZE; ++i )
                if( m_indices[ i ] == p_index )
                    return i;
            return -1;
        }

        void clear( ) {
            m_nextFree = 0;
            memset( m_indices, -1, sizeof( m_indices ) );
            for( u8 i = 0; i < MAX_CACHE_SIZE; ++i ) {
                delete m_tiles[ i ];
                delete m_blocks[ i ];
                delete m_palettes[ i ];
            }
        }
    };
    extern sliceCache cache;
}