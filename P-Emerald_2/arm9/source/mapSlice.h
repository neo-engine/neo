/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSlice.h
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

#pragma once
#include <nds/ndstypes.h>
#include <vector>
#include <memory>

namespace MAP {
    const char* const MAP_PATH = "nitro:/MAPS/";
    const char* const TILESET_PATH = "nitro:/MAPS/TILESETS/";
    const u16 SIZE = 32;
    const s16 dir[ 4 ][ 2 ] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };

    typedef struct {
        u16 m_pal[ 16 ];
    } Palette;
    typedef struct {
        u8 m_tile[ 32 ];
    } Tile;
    typedef u8 PalNo;
    //typedef struct{
    //    u16 tileidx      : 10;
    //    u8 vflip         :  1;
    //    u8 hflip         :  1;
    //    PalNo palno      :  4;
    //}BlockAtom;
    typedef u16 BlockAtom;

    class Block {
    public:
        BlockAtom   m_top[ 2 ][ 2 ];
        u8          m_topbehave;

        BlockAtom   m_bottom[ 2 ][ 2 ];
        u8          m_bottombehave;
    };

    typedef struct {
        Tile        m_blocks[ 1024 ];
    } TileSet;
    typedef struct {
        Block       m_blocks[ 1024 ];
    } BlockSet;

    typedef struct {
        u16         m_blockidx : 10;
        u8          m_movedata : 6;
    } MapBlockAtom;

    struct mapSlice {
        struct position {
            u16 m_posX; //Global
            u16 m_posY; //Global
            u8 m_posZ;
        };
        enum direction {
            UP,
            RIGHT,
            DOWN,
            LEFT
        };
        enum moveMode {
            //Player modes
            WALK = 0,
            SURF = 1,
            BIKE = 2,
            //NPC modes
            NOTHING = 4,

        };

        Palette     m_pals[ 16 ];
        TileSet     m_tileSet;
        BlockSet    m_blockSet;
        MapBlockAtom m_blocks[ 32 ][ 32 ]; // [ y ][ x ]
        u8          m_map;
        u16         m_x, m_y;
        u16         m_location;

        bool canMove( position p_start, direction p_direction, moveMode p_moveMode = WALK );
    };
    std::unique_ptr<mapSlice> constructSlice( u8 p_map, u16 p_x, u16 p_y );
}