/*
Pokémon Emerald 2 Version
------------------------------

file        : map2d.h
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
#include <nds.h>
#include <nds/ndstypes.h>
#include <vector>
#include "mapSlice.h"

#include <stdio.h>

namespace MAP {
    extern int bgs[ 4 ];

    class Anbindung {
    public:
        char        m_name[ 100 ];
        char        m_direction;
        int         m_move;
        int         m_mapidx;

        int         m_mapsx;
        int         m_mapsy;

        Anbindung( ) { }
        Anbindung( const char p_name[ 100 ], char p_dir, int p_mv, int p_mpidx )
            : m_direction( p_dir ),
            m_move( p_mv ),
            m_mapidx( p_mpidx ) {
            memcpy( m_name, p_name, 100 );
        }
    };
    class Animation {
    public:
        u16         m_tileIdx;
        u8          m_acFrame;
        u8          m_maxFrame;
        u8          m_speed;
        std::vector < Tile >
            m_animationTiles;
    };

    class Map {
    public:
        u32         m_sizex, m_sizey;

        Palette     m_pals[ 16 ];
        std::vector < std::vector < MapBlockAtom > >
            m_blocks;
        TileSet     m_tileset;
        BlockSet    m_blockSets;
        std::vector < Anbindung >
            m_anbindungen;
        u16         m_rand[ 2 ][ 2 ];
        std::vector < Animation >
            m_animations;

        //WildePKMN, Events...
        Map( ) { }

        Map( const char* p_path, const char* p_name );

        void            draw( s16 p_bx, s16 p_by, bool p_init = false );
        void            movePlayer( int p_direction );

        struct pos {
            u16 m_posX;
            u16 m_posY;
            u8 m_posZ;


        };
        static pos      getFlyPos( const char* p_path, const char* p_name );
    private:
        void            fill( u16* mapMemory[ 4 ], s16 p_xmin, s16 p_x, s16 p_xmax, s16 p_ymin, s16 p_y, s16 p_ymax, s16 p_c );
    };
    bool operator<( Map::pos p_a, Map::pos p_b );
}