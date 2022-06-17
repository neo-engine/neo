/*
Pokémon neo
------------------------------

file        : mapSlice.h
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

#pragma once
#include <cstring>
#include <vector>
#include <nds/ndstypes.h>
#include "map/mapDefines.h"

namespace MAP {
    constexpr u8  MAX_ANIM_PER_TILE_SET   = 32;
    constexpr u16 MAX_TILES_PER_TILE_SET  = 512;
    constexpr u16 MAX_BLOCKS_PER_TILE_SET = 512;

    const char* const MAP_PATH      = "nitro:/MAPS/";
    const char* const TILESET_PATH  = "nitro:/MAPS/TILESETS/";
    const char* const BLOCKSET_PATH = "nitro:/MAPS/BLOCKSETS/";
    const char* const PALETTE_PATH  = "nitro:/MAPS/PALETTES/";
    constexpr u16     SIZE          = 32;
    constexpr s16     dir[ 4 ][ 2 ] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    constexpr u8 COLORS_PER_PAL    = 16;
    constexpr u8 PALS_PER_BLOCKSET = 8;

    struct palette {
        u16 m_pal[ COLORS_PER_PAL ];
    };

    struct tile {
        u8 m_tile[ 32 ];
    };

    // struct blockAtom {
    //    u16 m_tileidx   : 10;
    //    u8 m_vflip      :  1;
    //    u8 m_hflip      :  1;
    //    u8 m_palno      :  4;
    // };
    typedef u16 blockAtom;

    class block {
      public:
        blockAtom m_top[ 2 ][ 2 ];
        u8        m_topbehave;

        blockAtom m_bottom[ 2 ][ 2 ];
        u8        m_bottombehave;
    };

    struct animation {
        u16       m_tileIdx;
        u16       m_size;
        u8        m_tileSetIdx;
        u8        m_acFrame;
        u8        m_maxFrame;
        u8        m_speed;
        const u8* m_tileData;
    };

    struct door {
        u8  m_tileset : 8;
        u8  m_doorIdx : 8;
        u8  m_palette : 4;
        u16 m_blockIdx : 12;
    };

    constexpr u8     TILE_ANIMATION_COUNT = 60;
    extern animation TILE_ANIMATIONS[ TILE_ANIMATION_COUNT ];

    constexpr u8 DOOR_ANIMATION_COUNT = 50;
    extern door  DOOR_ANIMATIONS[ DOOR_ANIMATION_COUNT ];

    struct tileSet {
        tile m_tiles[ 2 * MAX_TILES_PER_TILE_SET ];
    };

    struct blockSet {
        block m_blocks[ 2 * MAX_BLOCKS_PER_TILE_SET ];
    };

    struct mapBlockAtom {
        u16 m_blockidx : 10;
        u8  m_movedata : 6;
    };

    struct mapSliceData {
        u8 m_sizeX = SIZE; // ignored / assumed to be SIZE
        u32 : 24;
        u8 m_sizeY = SIZE; // ignored / assumed to be SIZE
        u32 : 24;

        u8 m_tIdx1 = 0;
        u32 : 24;
        u8 m_tIdx2 = 0;
        u32 : 24;

        u8 m_borderSizeX = 0; // ignored / assumed to be 0
        u8 m_borderSizeY = 0; // ignored / assumed to be 0
        u32 : 16;

        mapBlockAtom m_blocks[ SIZE ][ SIZE ] = { { mapBlockAtom( ) } }; // [ y ][ x ]
    };

    struct mapSlice {
        bool         m_loaded = false;
        u16          m_x = 0, m_y = 0;
        palette      m_pals[ 2 * PALS_PER_BLOCKSET * DAYTIMES ];
        tileSet      m_tileSet;
        blockSet     m_blockSet;
        mapSliceData m_data;
    };
    void constructSlice( FILE* p_bank, FILE* p_tileset, u8 p_map, u16 p_x, u16 p_y,
                         mapSlice* p_result, mapData* p_resultData,
                         mapSlice p_cache[ 2 ][ 2 ] = 0 );
} // namespace MAP
