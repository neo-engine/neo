/*
Pokémon Emerald 2 Version
------------------------------

p_file        : map2d.cpp
author      : Philip Wellnitz (RedArceus)
description : Main engine for parsing and handling 2D maps.

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


#include "map2d.h"

#include "screenLoader.h"
#include "buffer.h"

#include <fstream>
#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>


extern PrintConsole Top, Bottom;

namespace map2d {
#define PLAYERSPRITE 0

    inline void readPal( FILE* p_file, Palette* p_palette ) {
        if( p_file == 0 )
            return;
        for( int i = 0; i < 6; ++i )
            fread( p_palette[ i ].m_pal, sizeof( u16 ) * 16, 1, p_file );
        fclose( p_file );
    }
    inline void readTileSet( FILE* p_file, TileSet& p_tileSet, int p_startIdx = 0, int p_size = 512 ) {
        if( p_file == 0 )
            return;
        fread( &p_tileSet.m_blocks[ p_startIdx ], sizeof( Tile )*p_size, 1, p_file );
        fclose( p_file );
    }
    inline void readNop( FILE* p_file, int p_cnt ) {
        if( p_file == 0 )
            return;
        fread( 0, sizeof( u8 )*p_cnt, 1, p_file );
    }
    inline void readBlockSet( FILE* p_file, BlockSet& p_tileSet, int p_startIdx = 0, int p_size = 512 ) {
        if( p_file == 0 )
            return;
        readNop( p_file, 4 );
        for( int i = 0; i < p_size; ++i ) {
            fread( p_tileSet.m_blocks[ p_startIdx + i ].m_bottom, 4 * sizeof( BlockAtom ), 1, p_file );
            fread( p_tileSet.m_blocks[ p_startIdx + i ].m_top, 4 * sizeof( BlockAtom ), 1, p_file );
        }
        for( int i = 0; i < p_size; ++i ) {
            fread( &p_tileSet.m_blocks[ p_startIdx + i ].m_bottombehave, sizeof( u8 ), 1, p_file );
            fread( &p_tileSet.m_blocks[ p_startIdx + i ].m_topbehave, sizeof( u8 ), 1, p_file );
        }
        fclose( p_file );
    }

    inline void readAnimations( FILE* p_file, std::vector<Animation>& p_animations ) {
        if( p_file == 0 )
            return;
        u8 N;
        fread( &N, sizeof( u8 ), 1, p_file );
        for( int i = 0; i < N; ++i ) {
            Animation a;
            fread( &a.m_tileIdx, sizeof( u16 ), 1, p_file );
            fread( &a.m_speed, sizeof( u8 ), 1, p_file );
            fread( &a.m_maxFrame, sizeof( u8 ), 1, p_file );
            a.m_acFrame = 0;
            a.m_animationTiles.assign( a.m_maxFrame, Tile( ) );
            for( int i = 0; i < a.m_maxFrame; ++i )
                fread( &a.m_animationTiles[ i ], sizeof( Tile ), 1, p_file );
            p_animations.push_back( a );
        }
        fclose( p_file );
    }

    Map::Map( const char* p_path, const char* p_name ) {
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 4, 25, 20 );
        consoleClear( );

        sprintf( buffer, "%s%s.m2p", p_path, p_name );
        FILE* mapF = fopen( buffer, "rb" );
        if( mapF == 0 )
            return;

        //printf("TEST1\n");        

        fread( &m_sizey, sizeof( u32 ), 1, mapF );
        fread( &m_sizex, sizeof( u32 ), 1, mapF );

        u8 tsidx1, tsidx2;
        fread( &tsidx1, sizeof( u8 ), 1, mapF );
        readNop( mapF, 3 );
        fread( &tsidx2, sizeof( u8 ), 1, mapF );
        readNop( mapF, 3 );

        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.ts", tsidx1 );
        readTileSet( fopen( buffer, "rb" ), m_tileset );
        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.bvd", tsidx1 );
        readBlockSet( fopen( buffer, "rb" ), m_blockSets );
        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.p2l", tsidx1 );
        readPal( fopen( buffer, "rb" ), m_pals );
        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.anm", tsidx1 );
        readAnimations( fopen( buffer, "rb" ), m_animations );

        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.ts", tsidx2 );
        readTileSet( fopen( buffer, "rb" ), m_tileset, 512 );
        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.bvd", tsidx2 );
        readBlockSet( fopen( buffer, "rb" ), m_blockSets, 512 );
        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.p2l", tsidx2 );
        readPal( fopen( buffer, "rb" ), m_pals + 6 );
        sprintf( buffer, "nitro:/MAPS/TILESETS/%i.anm", tsidx2 );
        readAnimations( fopen( buffer, "rb" ), m_animations );


        readNop( mapF, 4 );
        m_blocks.assign( m_sizex + 20, std::vector<MapBlockAtom>( m_sizey + 20 ) );

        //printf("TEST2\n");

        sprintf( buffer, "%s%s.anb", p_path, p_name );
        FILE* A = fopen( buffer, "r" );
        if( A ) {
            int N; fscanf( A, "%d", &N );
            m_anbindungen = std::vector<Anbindung>( N );
            //printf("TEST3 : %i\n",N);
            for( int i = 0; i < N; ++i ) {
                Anbindung& ac = m_anbindungen[ i ];
                fscanf( A, "%s %c %d %d", ac.m_name, &ac.m_direction, &ac.m_move, &ac.m_mapidx );
                //printf("TEST4 : %s %c %d %d\n",ac.name, ac.p_direction,ac.move,ac.mapidx);

                sprintf( buffer, "%s%s.m2p", p_path, ac.m_name );
                FILE* mapF2 = fopen( buffer, "rb" );
                if( mapF2 == 0 )
                    continue;
                fread( &ac.m_mapsy, sizeof( u32 ), 1, mapF2 );
                fread( &ac.m_mapsx, sizeof( u32 ), 1, mapF2 );

                readNop( mapF2, 12 );

                //TODO: SPEED-UP this Part
                for( int x = 0; x < ac.m_mapsx; ++x ) {
                    for( int y = 0; y < ac.m_mapsy; ++y ) {
                        if( ac.m_direction == 'W' && y >= ac.m_mapsy - 10 && x + ac.m_move + 10 >= 0 && x + ac.m_move < (int)m_sizex + 10 )
                            fread( &( m_blocks[ x + ac.m_move + 10 ][ y - ac.m_mapsy + 10 ] ), sizeof( MapBlockAtom ), 1, mapF2 );

                        else if( ac.m_direction == 'N' && x - ac.m_mapsx + 10 >= 0 && y + ac.m_move >= -10 && y + ac.m_move < (int)m_sizey + 10 )
                            fread( &( m_blocks[ x - ac.m_mapsx + 10 ][ y + ac.m_move + 10 ] ), sizeof( MapBlockAtom ), 1, mapF2 );

                        else if( ac.m_direction == 'E' && y < 10 && x + ac.m_move + 10 >= 0 && x + ac.m_move < (int)m_sizex + 10 )
                            fread( &( m_blocks[ x + ac.m_move + 10 ][ y + m_sizey + 10 ] ), sizeof( MapBlockAtom ), 1, mapF2 );

                        else if( ac.m_direction == 'S' && x < 10 && y + ac.m_move + 10 >= 0 && y + ac.m_move < (int)m_sizey + 10 )
                            fread( &( m_blocks[ x + m_sizex + 10 ][ y + ac.m_move + 10 ] ), sizeof( MapBlockAtom ), 1, mapF2 );
                        else {
                            readNop( mapF2, sizeof( MapBlockAtom ) );
                        }
                    }
                }
                fclose( mapF2 );
            }
            consoleSelect( &Bottom );
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 0; j < 2; ++j ) {
                if( fscanf( A, "%hu", &m_rand[ i ][ j ] ) == EOF )
                    m_rand[ i ][ j ] = 0;
                }
            fclose( A );
        }

        for( u32 x = 0; x < m_sizex; ++x )
            fread( &m_blocks[ x + 10 ][ 10 ], sizeof( MapBlockAtom )*m_sizey, 1, mapF );
        //printf("Test5 %i",anbindungen.p_size());
        fclose( mapF );
    }

    int tcnt = 0;
    int bgs[ 4 ];

    int lastrow, lastcol;
    int lastbx, lastby;

    u16* mapMemory[ 4 ];

    void Map::fill( u16* p_mapMemory[ 4 ], s16 p_xmin, s16 p_x, s16 p_xmax, s16 p_ymin, s16 p_y, s16 p_ymax, s16 p_c ) {
        u8 toplayer = 1, betw = 2, bottomlayer = 3;

        Block acBlock;
        if( p_x < 0 || p_y < 0 || p_x >= (s16)m_sizex + 20 || p_y >= (s16)m_sizey + 20 )
            acBlock = m_blockSets.m_blocks[ m_rand[ p_x % 2 ][ p_y % 2 ] ];
        else if( ( p_x < 10 || p_y < 10 || p_x >= (s16)m_sizex + 10 || p_y >= (s16)m_sizey + 10 ) && m_blocks[ p_x ][ p_y ].m_blockidx == 0 )
            acBlock = m_blockSets.m_blocks[ m_rand[ p_x % 2 ][ p_y % 2 ] ];
        else
            acBlock = m_blockSets.m_blocks[ m_blocks[ p_x ][ p_y ].m_blockidx ];

        if( acBlock.m_topbehave != 0x10 )
            std::swap( toplayer, betw );

        if( p_x > p_xmin && p_y > p_ymin ) {
            p_mapMemory[ toplayer ][ p_c - 33 ] = 0;
            p_mapMemory[ betw ][ p_c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
            p_mapMemory[ bottomlayer ][ p_c - 33 ] = acBlock.m_bottom[ 0 ][ 0 ];
        }
        if( p_x > p_xmin && p_y < p_ymax - 1 ) {
            p_mapMemory[ toplayer ][ p_c - 32 ] = 0;
            p_mapMemory[ betw ][ p_c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
            p_mapMemory[ bottomlayer ][ p_c - 32 ] = acBlock.m_bottom[ 0 ][ 1 ];
        }
        if( p_x < p_xmax - 1 && p_y > p_ymin ) {
            p_mapMemory[ toplayer ][ p_c - 1 ] = 0;
            p_mapMemory[ betw ][ p_c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
            p_mapMemory[ bottomlayer ][ p_c - 1 ] = acBlock.m_bottom[ 1 ][ 0 ];
        }
        if( p_x < p_xmax - 1 && p_y < p_ymax - 1 ) {
            p_mapMemory[ toplayer ][ p_c ] = 0;
            p_mapMemory[ betw ][ p_c ] = acBlock.m_top[ 1 ][ 1 ];
            p_mapMemory[ bottomlayer ][ p_c ] = acBlock.m_bottom[ 1 ][ 1 ];
        }
    }

    void Map::movePlayer( int p_direction ) {
        int c = 0;

        int bx = lastbx, by = lastby;

        s16 xmin = 0,
            xmax = 0,
            ymin = 0,
            ymax = 0,
            plsval = 0;
        int lc = lastcol, lr = lastrow;

        switch( p_direction ) {
            case 1:
                lastrow = ( lastrow + 1 ) % 32;
                c = lastrow * 2;
                lastbx++;
                bx += 16;

                xmin = lastby;
                xmax = lastby + 17;
                ymin = lastbx + 15;
                ymax = lastbx + 17;
                plsval = 60;

                break;
            case 3:
                c = lastrow * 2;
                lastrow = ( lastrow + 31 ) % 32;
                lastbx--;
                bx += 14;

                xmin = lastby;
                xmax = lastby + 17;
                ymin = lastbx;
                ymax = lastbx + 2;
                plsval = 60;

                break;
            case 2:
                lastcol = ( lastcol + 1 ) % 16;
                c = lastcol * 64;
                lastby++;
                by += 16;

                xmin = lastby + 15;
                xmax = lastby + 17;
                ymin = lastbx;
                ymax = lastbx + 17;
                plsval = 32;

                break;
            case 4:
                c = lastcol * 64;
                lastcol = ( lastcol + 15 ) % 16;

                lastby--;
                by += 14;

                xmin = lastby;
                xmax = lastby + 2;
                ymin = lastbx;
                ymax = lastbx + 17;
                plsval = 32;

        }
        int c2 = c;
        if( p_direction == 1 ) {
            ymin += 16;
            ymax += 16;
        }
        for( u8 i = 0; i < 2; ++i ) {
            if( ( p_direction % 2 == 0 ) || ( i == ( ( lastrow + ( p_direction == 3 ? 1 : 0 ) ) % 32 ) / 16 ) ) {
                for( s16 x = xmin; x < xmax; x++ ) {
                    for( s16 y = ymin; y < ymax; y++ ) {

                        fill( mapMemory, xmin, x, xmax, ymin, y, ymax, c );

                        if( ( p_direction % 2 == 1 && y < bx + 16 * ( 2 + i ) ) || ( p_direction % 2 == 0 && y < bx + 16 * ( 1 + i ) ) )
                            c += 2;
                    }
                    c += plsval;
                }
            }
            c = c2 + 1024 - 32;
            if( p_direction % 2 == 0 ) {
                c += 32;
                ymin += 16;
                ymax += 16;
            }
        }
        switch( p_direction ) {
            case 2: case 4:
            {
                u16 q[ 64 ];
                for( u8 i = 1; i < 4; ++i ) {
                    for( u8 g = 0; g < 32; ++g ) {
                        q[ g ] = mapMemory[ i ][ c2 + g ];
                        q[ g + 32 ] = mapMemory[ i ][ c2 + g + 1024 ];
                    }
                    for( u8 o = 0; o < 32; ++o ) {
                        mapMemory[ i ][ c2 + o ] = q[ ( o + 64 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
                        mapMemory[ i ][ c2 + o + 1024 ] = q[ ( o + 96 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
                    }
                }
                c2 += 32;
                for( u8 i = 1; i < 4; ++i ) {
                    for( u8 g = 0; g < 32; ++g ) {
                        q[ g ] = mapMemory[ i ][ c2 + g ];
                        q[ g + 32 ] = mapMemory[ i ][ c2 + g + 1024 ];
                    }
                    for( u8 o = 0; o < 32; ++o ) {
                        mapMemory[ i ][ c2 + o ] = q[ ( o + 64 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
                        mapMemory[ i ][ c2 + o + 1024 ] = q[ ( o + 96 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
                    }
                }
                break;
            }
            case 1: case 3:
            {
                c2 += ( ( ( lastrow + ( p_direction == 3 ? 1 : 0 ) ) % 32 ) / 16 ) * ( 1024 - 32 );

                u16 q[ 32 ];
                for( u8 i = 1; i < 4; ++i ) {
                    for( u8 g = 0; g < 32; ++g )
                        q[ g ] = mapMemory[ i ][ c2 + 32 * g ];
                    for( u8 o = 0; o < 32; ++o )
                        mapMemory[ i ][ c2 + 32 * o ] = q[ ( o + 32 - 2 * ( lc + 1 ) ) % 32 ];
                }
                c2++;
                for( u8 i = 1; i < 4; ++i ) {
                    for( u8 g = 0; g < 32; ++g )
                        q[ g ] = mapMemory[ i ][ c2 + 32 * g ];
                    for( u8 o = 0; o < 32; ++o )
                        mapMemory[ i ][ c2 + 32 * o ] = q[ ( o + 32 - 2 * ( lc + 1 ) ) % 32 ];
                }
                break;
            }
        }
    }

    void Map::draw( s16 p_bx, s16 p_by, bool p_init ) {
        if( p_init ) {
            videoSetMode( MODE_0_2D /*| DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE |
                          DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            //REG_BG0CNT = BG_32x32 | BG_COLOR_16 |  BG_MAP_BASE(0) | BG_TILE_BASE(1)|  BG_PRIORITY(0);
            //REG_BG1CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(1) | BG_TILE_BASE(1)| BG_PRIORITY(1);
            //REG_BG2CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(2) | BG_TILE_BASE(1)| BG_PRIORITY(2);
            //REG_BG3CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(3) | BG_TILE_BASE(1)| BG_PRIORITY(3);

            for( u8 i = 1; i < 4; ++i ) {
                bgs[ i ] = bgInit( i, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetPriority( bgs[ i ], i );
                bgScroll( bgs[ i ], 128, 32 );
            }

            //Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 5, true ,true);

            u8* tileMemory = (u8*)BG_TILE_RAM( 1 );

            for( u16 i = 0; i < 1024; ++i )
                swiCopy( m_tileset.m_blocks[ i ].m_tile, tileMemory + i * 32, 16 );

            dmaCopy( m_pals, BG_PALETTE, 512 );
            for( u8 i = 1; i < 4; ++i )  mapMemory[ i ] = (u16*)BG_MAP_RAM( 2 * i - 1 );
        }

        lastrow = 31;
        lastcol = 15;

        int c = 0;
        p_bx += 0;
        p_by += 10;


        lastbx = p_bx;
        lastby = p_by;
        for( u8 i = 0; i < 2; ++i ) {
            for( s16 x = p_by; x < p_by + 17; x++ ) {
                for( s16 y = p_bx; y < p_bx + 17; y++ ) {
                    u8 toplayer = 1, bottomlayer = 3;

                    Block acBlock = m_blockSets.m_blocks[ m_blocks[ x ][ y ].m_blockidx ];

                    //if(tile_deb_test){
                    //    acBlock = b.blocks[tcnt];
                    //    tcnt = (tcnt +1)%1024;
                    //}
                    if( x < 0 || y < 0 || x >= (s16)m_sizex + 20 || y >= (s16)m_sizey + 20 ) {
                        acBlock = m_blockSets.m_blocks[ m_rand[ x % 2 ][ y % 2 ] ];
                    } else if( ( x < 10 || y < 10 || x >= (s16)m_sizex + 10 || y >= (s16)m_sizey + 10 ) && m_blocks[ x ][ y ].m_blockidx == 0 ) {
                        acBlock = m_blockSets.m_blocks[ m_rand[ x % 2 ][ y % 2 ] ];
                    } else {
                        //consoleSelect(&Top);
                        //consoleSetWindow(&Top,2*(y-p_bx)-1,2*(x-p_by)-1,2,2);
                        //printf("%i",acBlock.topbehave);
                    }

                    if( acBlock.m_topbehave == 0x10 ) {
                        if( x > p_by && y > p_bx )
                            mapMemory[ toplayer + 1 ][ c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
                        if( x > p_by && y < p_bx + 16 )
                            mapMemory[ toplayer + 1 ][ c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
                        if( x < p_by + 16 && y > p_bx )
                            mapMemory[ toplayer + 1 ][ c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
                        if( x < p_by + 16 && y < p_bx + 16 )
                            mapMemory[ toplayer + 1 ][ c ] = acBlock.m_top[ 1 ][ 1 ];

                        if( x > p_by && y > p_bx )
                            mapMemory[ toplayer ][ c - 33 ] = 0;
                        if( x > p_by && y < p_bx + 16 )
                            mapMemory[ toplayer ][ c - 32 ] = 0;
                        if( x < p_by + 16 && y > p_bx )
                            mapMemory[ toplayer ][ c - 1 ] = 0;
                        if( x < p_by + 16 && y < p_bx + 16 )
                            mapMemory[ toplayer ][ c ] = 0;
                    } else {
                        if( x > p_by && y > p_bx )
                            mapMemory[ toplayer ][ c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
                        if( x > p_by && y < p_bx + 16 )
                            mapMemory[ toplayer ][ c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
                        if( x < p_by + 16 && y > p_bx )
                            mapMemory[ toplayer ][ c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
                        if( x < p_by + 16 && y < p_bx + 16 )
                            mapMemory[ toplayer ][ c ] = acBlock.m_top[ 1 ][ 1 ];

                        if( x > p_by && y > p_bx )
                            mapMemory[ toplayer + 1 ][ c - 33 ] = 0;
                        if( x > p_by && y < p_bx + 16 )
                            mapMemory[ toplayer + 1 ][ c - 32 ] = 0;
                        if( x < p_by + 16 && y > p_bx )
                            mapMemory[ toplayer + 1 ][ c - 1 ] = 0;
                        if( x < p_by + 16 && y < p_bx + 16 )
                            mapMemory[ toplayer + 1 ][ c ] = 0;
                    }

                    if( x > p_by && y > p_bx )
                        mapMemory[ bottomlayer ][ c - 33 ] = acBlock.m_bottom[ 0 ][ 0 ];
                    if( x > p_by && y < p_bx + 16 )
                        mapMemory[ bottomlayer ][ c - 32 ] = acBlock.m_bottom[ 0 ][ 1 ];
                    if( x < p_by + 16 && y > p_bx )
                        mapMemory[ bottomlayer ][ c - 1 ] = acBlock.m_bottom[ 1 ][ 0 ];
                    if( x < p_by + 16 && y < p_bx + 16 )
                        mapMemory[ bottomlayer ][ c ] = acBlock.m_bottom[ 1 ][ 1 ];

                    if( y < p_bx + 16 )
                        c += 2;
                }
                c += 32;
            }
            c -= 64;
            p_bx += 16;
        }
        bgUpdate( );
        //consoleSelect( &Bottom );
        //swiWaitForVBlank();
    }
}