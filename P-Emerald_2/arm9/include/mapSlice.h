/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.h
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

#pragma once
#include <cstring>
#include <memory>
#include <vector>
#include <nds/ndstypes.h>

namespace MAP {
    const u8  MAX_ANIM_PER_TILE_SET   = 32;
    const u16 MAX_TILES_PER_TILE_SET  = 512;
    const u16 MAX_BLOCKS_PER_TILE_SET = 512;

    const char* const MAP_PATH      = "nitro:/MAPS/";
    const char* const TILESET_PATH  = "nitro:/MAPS/TILESETS/";
    const u16         SIZE          = 32;
    const s16         dir[ 4 ][ 2 ] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

    typedef struct { u16 m_pal[ 16 ]; } palette;
    typedef struct { u8 m_tile[ 32 ]; } tile;
    // typedef struct{
    //    u16 tileidx      : 10;
    //    u8 vflip         :  1;
    //    u8 hflip         :  1;
    //    PalNo palno      :  4;
    //}blockAtom;
    typedef u16 blockAtom;

    class block {
      public:
        blockAtom m_top[ 2 ][ 2 ];
        u8        m_topbehave;

        blockAtom m_bottom[ 2 ][ 2 ];
        u8        m_bottombehave;
    };

    struct tileSet {
        struct animation {
            u16  m_tileIdx;
            u8   m_acFrame;
            u8   m_maxFrame;
            u8   m_speed;
            tile m_tiles[ 16 ];
        };
        u8        m_animationCount1, m_animationCount2;
        animation m_animations[ 2 * MAX_ANIM_PER_TILE_SET ];
        tile      m_tiles[ 2 * MAX_TILES_PER_TILE_SET ];
    };
    struct blockSet {
        block m_blocks[ 2 * MAX_BLOCKS_PER_TILE_SET ];
    };

    struct mapBlockAtom {
        u16 m_blockidx : 10;
        u8  m_movedata : 6;
    };

    struct mapSlice {
        palette      m_pals[ 16 ];
        tileSet      m_tileSet;
        blockSet     m_blockSet;
        mapBlockAtom m_blocks[ 32 ][ 32 ]; // [ y ][ x ]
        u8           m_evtCnt[ 32 ][ 32 ]; // [ y ][ x ]
        u8           m_map;
        u16          m_x, m_y;
        u8           m_tIdx1, m_tIdx2;

        std::pair<u16, u16> m_pokemon[ 3 * 5 * 5 ];
    };
    void constructSlice( u8 p_map, u16 p_x, u16 p_y, std::unique_ptr<mapSlice>& p_result,
                         std::unique_ptr<mapSlice> p_cache[ 2 ][ 2 ] = 0 );
}
