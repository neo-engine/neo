/*
Pokémon Emerald 2 Version
------------------------------

file        : map2d.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2014
Philip Wellnitz (RedArceus)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:


1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
is required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/

#pragma once
#include <nds.h>
#include <nds/ndstypes.h>
#include <vector>

#include <stdio.h>

namespace map2d {
    extern int bgs[ 4 ];


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
        std::vector< Tile >
            m_animationTiles;
    };

    class Map {
    public:
        u32         m_sizex, m_sizey;

        Palette     m_pals[ 16 ];
        std::vector< std::vector < MapBlockAtom > >
            m_blocks;
        TileSet     m_tileset;
        BlockSet    m_blockSets;
        std::vector< Anbindung >
            m_anbindungen;
        u16         m_rand[ 2 ][ 2 ];
        std::vector< Animation >
            m_animations;

        //WildePKMN, Events...
        Map( ) { }

        Map( const char* p_path, const char* p_name );

        void            draw( int p_bx, int p_by, bool p_init = false );
        void            movePlayer( int p_direction );
    private:
        void            fill( u16* mapMemory[ 4 ], int p_xmin, int p_x, int p_xmax, int p_ymin, int p_y, int p_ymax, int p_c );
    };
}