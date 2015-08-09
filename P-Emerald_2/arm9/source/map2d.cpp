///*
//Pokémon Emerald 2 Version
//------------------------------
//
//file        : map2d.cpp
//author      : Philip Wellnitz 
//description :
//
//Copyright (C) 2012 - 2015
//Philip Wellnitz 
//
//This file is part of Pokémon Emerald 2 Version.
//
//Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
//*/
//
//
//
//#include "map2d.h"
//
//#include "buffer.h"
//#include "uio.h"
//
//#include <fstream>
//#include <vector>
//#include <fat.h>
//#include <nds.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <math.h>
//
//namespace MAP {
//#define PLAYERSPRITE 0
//
//
//    int tcnt = 0;
//    int bgs[ 4 ];
//
//    int lastrow, lastcol;
//    int lastbx, lastby;
//
//    u16* mapMemory[ 4 ];
//
//    void Map::fill( u16* p_mapMemory[ 4 ], s16 p_xmin, s16 p_x, s16 p_xmax, s16 p_ymin, s16 p_y, s16 p_ymax, s16 p_c ) {
//        u8 toplayer = 1, betw = 2, bottomlayer = 3;
//
//        Block acBlock;
//        if( p_x < 0 || p_y < 0 || p_x >= (s16)m_sizex + 20 || p_y >= (s16)m_sizey + 20 )
//            acBlock = m_blockSets.m_blocks[ m_rand[ p_x % 2 ][ p_y % 2 ] ];
//        else if( ( p_x < 10 || p_y < 10 || p_x >= (s16)m_sizex + 10 || p_y >= (s16)m_sizey + 10 ) && m_blocks[ p_x ][ p_y ].m_blockidx == 0 )
//            acBlock = m_blockSets.m_blocks[ m_rand[ p_x % 2 ][ p_y % 2 ] ];
//        else
//            acBlock = m_blockSets.m_blocks[ m_blocks[ p_x ][ p_y ].m_blockidx ];
//
//        if( acBlock.m_topbehave != 0x10 )
//            std::swap( toplayer, betw );
//
//        if( p_x > p_xmin && p_y > p_ymin ) {
//            p_mapMemory[ toplayer ][ p_c - 33 ] = 0;
//            p_mapMemory[ betw ][ p_c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
//            p_mapMemory[ bottomlayer ][ p_c - 33 ] = acBlock.m_bottom[ 0 ][ 0 ];
//        }
//        if( p_x > p_xmin && p_y < p_ymax - 1 ) {
//            p_mapMemory[ toplayer ][ p_c - 32 ] = 0;
//            p_mapMemory[ betw ][ p_c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
//            p_mapMemory[ bottomlayer ][ p_c - 32 ] = acBlock.m_bottom[ 0 ][ 1 ];
//        }
//        if( p_x < p_xmax - 1 && p_y > p_ymin ) {
//            p_mapMemory[ toplayer ][ p_c - 1 ] = 0;
//            p_mapMemory[ betw ][ p_c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
//            p_mapMemory[ bottomlayer ][ p_c - 1 ] = acBlock.m_bottom[ 1 ][ 0 ];
//        }
//        if( p_x < p_xmax - 1 && p_y < p_ymax - 1 ) {
//            p_mapMemory[ toplayer ][ p_c ] = 0;
//            p_mapMemory[ betw ][ p_c ] = acBlock.m_top[ 1 ][ 1 ];
//            p_mapMemory[ bottomlayer ][ p_c ] = acBlock.m_bottom[ 1 ][ 1 ];
//        }
//    }
//
//    void Map::movePlayer( int p_direction ) {
//        int c = 0;
//
//        int bx = lastbx, by = lastby;
//
//        s16 xmin = 0,
//            xmax = 0,
//            ymin = 0,
//            ymax = 0,
//            plsval = 0;
//        int lc = lastcol, lr = lastrow;
//
//        switch( p_direction ) {
//            case 1:
//                lastrow = ( lastrow + 1 ) % 32;
//                c = lastrow * 2;
//                lastbx++;
//                bx += 16;
//
//                xmin = lastby;
//                xmax = lastby + 17;
//                ymin = lastbx + 15;
//                ymax = lastbx + 17;
//                plsval = 60;
//
//                break;
//            case 3:
//                c = lastrow * 2;
//                lastrow = ( lastrow + 31 ) % 32;
//                lastbx--;
//                bx += 14;
//
//                xmin = lastby;
//                xmax = lastby + 17;
//                ymin = lastbx;
//                ymax = lastbx + 2;
//                plsval = 60;
//
//                break;
//            case 2:
//                lastcol = ( lastcol + 1 ) % 16;
//                c = lastcol * 64;
//                lastby++;
//                by += 16;
//
//                xmin = lastby + 15;
//                xmax = lastby + 17;
//                ymin = lastbx;
//                ymax = lastbx + 17;
//                plsval = 32;
//
//                break;
//            case 4:
//                c = lastcol * 64;
//                lastcol = ( lastcol + 15 ) % 16;
//
//                lastby--;
//                by += 14;
//
//                xmin = lastby;
//                xmax = lastby + 2;
//                ymin = lastbx;
//                ymax = lastbx + 17;
//                plsval = 32;
//
//        }
//        int c2 = c;
//        if( p_direction == 1 ) {
//            ymin += 16;
//            ymax += 16;
//        }
//        for( u8 i = 0; i < 2; ++i ) {
//            if( ( p_direction % 2 == 0 ) || ( i == ( ( lastrow + ( p_direction == 3 ? 1 : 0 ) ) % 32 ) / 16 ) ) {
//                for( s16 x = xmin; x < xmax; x++ ) {
//                    for( s16 y = ymin; y < ymax; y++ ) {
//
//                        fill( mapMemory, xmin, x, xmax, ymin, y, ymax, c );
//
//                        if( ( p_direction % 2 == 1 && y < bx + 16 * ( 2 + i ) ) || ( p_direction % 2 == 0 && y < bx + 16 * ( 1 + i ) ) )
//                            c += 2;
//                    }
//                    c += plsval;
//                }
//            }
//            c = c2 + 1024 - 32;
//            if( p_direction % 2 == 0 ) {
//                c += 32;
//                ymin += 16;
//                ymax += 16;
//            }
//        }
//        switch( p_direction ) {
//            case 2: case 4:
//            {
//                u16 q[ 64 ];
//                for( u8 i = 1; i < 4; ++i ) {
//                    for( u8 g = 0; g < 32; ++g ) {
//                        q[ g ] = mapMemory[ i ][ c2 + g ];
//                        q[ g + 32 ] = mapMemory[ i ][ c2 + g + 1024 ];
//                    }
//                    for( u8 o = 0; o < 32; ++o ) {
//                        mapMemory[ i ][ c2 + o ] = q[ ( o + 64 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
//                        mapMemory[ i ][ c2 + o + 1024 ] = q[ ( o + 96 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
//                    }
//                }
//                c2 += 32;
//                for( u8 i = 1; i < 4; ++i ) {
//                    for( u8 g = 0; g < 32; ++g ) {
//                        q[ g ] = mapMemory[ i ][ c2 + g ];
//                        q[ g + 32 ] = mapMemory[ i ][ c2 + g + 1024 ];
//                    }
//                    for( u8 o = 0; o < 32; ++o ) {
//                        mapMemory[ i ][ c2 + o ] = q[ ( o + 64 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
//                        mapMemory[ i ][ c2 + o + 1024 ] = q[ ( o + 96 - 2 * ( ( lr + 1 ) % 32 ) ) % 64 ];
//                    }
//                }
//                break;
//            }
//            case 1: case 3:
//            {
//                c2 += ( ( ( lastrow + ( p_direction == 3 ? 1 : 0 ) ) % 32 ) / 16 ) * ( 1024 - 32 );
//
//                u16 q[ 32 ];
//                for( u8 i = 1; i < 4; ++i ) {
//                    for( u8 g = 0; g < 32; ++g )
//                        q[ g ] = mapMemory[ i ][ c2 + 32 * g ];
//                    for( u8 o = 0; o < 32; ++o )
//                        mapMemory[ i ][ c2 + 32 * o ] = q[ ( o + 32 - 2 * ( lc + 1 ) ) % 32 ];
//                }
//                c2++;
//                for( u8 i = 1; i < 4; ++i ) {
//                    for( u8 g = 0; g < 32; ++g )
//                        q[ g ] = mapMemory[ i ][ c2 + 32 * g ];
//                    for( u8 o = 0; o < 32; ++o )
//                        mapMemory[ i ][ c2 + 32 * o ] = q[ ( o + 32 - 2 * ( lc + 1 ) ) % 32 ];
//                }
//                break;
//            }
//        }
//    }
//
//    void Map::draw( s16 p_bx, s16 p_by, bool p_init ) {
//        if( p_init ) {
//            videoSetMode( MODE_0_2D /*| DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE |
//                          DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
//            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
//
//            //REG_BG0CNT = BG_32x32 | BG_COLOR_16 |  BG_MAP_BASE(0) | BG_TILE_BASE(1)|  BG_PRIORITY(0);
//            //REG_BG1CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(1) | BG_TILE_BASE(1)| BG_PRIORITY(1);
//            //REG_BG2CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(2) | BG_TILE_BASE(1)| BG_PRIORITY(2);
//            //REG_BG3CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(3) | BG_TILE_BASE(1)| BG_PRIORITY(3);
//
//            for( u8 i = 1; i < 4; ++i ) {
//                bgs[ i ] = bgInit( i, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
//                bgSetPriority( bgs[ i ], i );
//                bgScroll( bgs[ i ], 128, 32 );
//            }
//
//            //Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 5, true ,true);
//
//            u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
//
//            for( u16 i = 0; i < 1024; ++i )
//                swiCopy( m_tileset.m_blocks[ i ].m_tile, tileMemory + i * 32, 16 );
//
//            dmaCopy( m_pals, BG_PALETTE, 512 );
//            for( u8 i = 1; i < 4; ++i )  mapMemory[ i ] = (u16*)BG_MAP_RAM( 2 * i - 1 );
//        }
//
//        lastrow = 31;
//        lastcol = 15;
//
//        int c = 0;
//        p_bx += 0;
//        p_by += 10;
//
//
//        lastbx = p_bx;
//        lastby = p_by;
//        for( u8 i = 0; i < 2; ++i ) {
//            for( s16 x = p_by; x < p_by + 17; x++ ) {
//                for( s16 y = p_bx; y < p_bx + 17; y++ ) {
//                    u8 toplayer = 1, bottomlayer = 3;
//
//                    Block acBlock = m_blockSets.m_blocks[ m_blocks[ x ][ y ].m_blockidx ];
//
//                    //if(tile_deb_test){
//                    //    acBlock = b.blocks[tcnt];
//                    //    tcnt = (tcnt +1)%1024;
//                    //}
//                    if( x < 0 || y < 0 || x >= (s16)m_sizex + 20 || y >= (s16)m_sizey + 20 ) {
//                        acBlock = m_blockSets.m_blocks[ m_rand[ x % 2 ][ y % 2 ] ];
//                    } else if( ( x < 10 || y < 10 || x >= (s16)m_sizex + 10 || y >= (s16)m_sizey + 10 ) && m_blocks[ x ][ y ].m_blockidx == 0 ) {
//                        acBlock = m_blockSets.m_blocks[ m_rand[ x % 2 ][ y % 2 ] ];
//                    } else {
//                        //consoleSelect(&Top);
//                        //consoleSetWindow(&Top,2*(y-p_bx)-1,2*(x-p_by)-1,2,2);
//                        //printf("%i",acBlock.topbehave);
//                    }
//
//                    if( acBlock.m_topbehave == 0x10 ) {
//                        if( x > p_by && y > p_bx )
//                            mapMemory[ toplayer + 1 ][ c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
//                        if( x > p_by && y < p_bx + 16 )
//                            mapMemory[ toplayer + 1 ][ c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
//                        if( x < p_by + 16 && y > p_bx )
//                            mapMemory[ toplayer + 1 ][ c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
//                        if( x < p_by + 16 && y < p_bx + 16 )
//                            mapMemory[ toplayer + 1 ][ c ] = acBlock.m_top[ 1 ][ 1 ];
//
//                        if( x > p_by && y > p_bx )
//                            mapMemory[ toplayer ][ c - 33 ] = 0;
//                        if( x > p_by && y < p_bx + 16 )
//                            mapMemory[ toplayer ][ c - 32 ] = 0;
//                        if( x < p_by + 16 && y > p_bx )
//                            mapMemory[ toplayer ][ c - 1 ] = 0;
//                        if( x < p_by + 16 && y < p_bx + 16 )
//                            mapMemory[ toplayer ][ c ] = 0;
//                    } else {
//                        if( x > p_by && y > p_bx )
//                            mapMemory[ toplayer ][ c - 33 ] = ( acBlock.m_top[ 0 ][ 0 ] );
//                        if( x > p_by && y < p_bx + 16 )
//                            mapMemory[ toplayer ][ c - 32 ] = acBlock.m_top[ 0 ][ 1 ];
//                        if( x < p_by + 16 && y > p_bx )
//                            mapMemory[ toplayer ][ c - 1 ] = acBlock.m_top[ 1 ][ 0 ];
//                        if( x < p_by + 16 && y < p_bx + 16 )
//                            mapMemory[ toplayer ][ c ] = acBlock.m_top[ 1 ][ 1 ];
//
//                        if( x > p_by && y > p_bx )
//                            mapMemory[ toplayer + 1 ][ c - 33 ] = 0;
//                        if( x > p_by && y < p_bx + 16 )
//                            mapMemory[ toplayer + 1 ][ c - 32 ] = 0;
//                        if( x < p_by + 16 && y > p_bx )
//                            mapMemory[ toplayer + 1 ][ c - 1 ] = 0;
//                        if( x < p_by + 16 && y < p_bx + 16 )
//                            mapMemory[ toplayer + 1 ][ c ] = 0;
//                    }
//
//                    if( x > p_by && y > p_bx )
//                        mapMemory[ bottomlayer ][ c - 33 ] = acBlock.m_bottom[ 0 ][ 0 ];
//                    if( x > p_by && y < p_bx + 16 )
//                        mapMemory[ bottomlayer ][ c - 32 ] = acBlock.m_bottom[ 0 ][ 1 ];
//                    if( x < p_by + 16 && y > p_bx )
//                        mapMemory[ bottomlayer ][ c - 1 ] = acBlock.m_bottom[ 1 ][ 0 ];
//                    if( x < p_by + 16 && y < p_bx + 16 )
//                        mapMemory[ bottomlayer ][ c ] = acBlock.m_bottom[ 1 ][ 1 ];
//
//                    if( y < p_bx + 16 )
//                        c += 2;
//                }
//                c += 32;
//            }
//            c -= 64;
//            p_bx += 16;
//        }
//        bgUpdate( );
//        //consoleSelect( &Bottom );
//        //swiWaitForVBlank();
//    }
//
//    Map::pos Map::getFlyPos( const char* p_path, const char* p_name ) {
//        sprintf( buffer, "%s%s.fp", p_path, p_name );
//        FILE* mapF = fopen( buffer, "rb" );
//        if( !mapF )
//            return{ -1, -1, -1 };
//
//        u16 x, y;
//        u8 z;
//        fscanf( mapF, "%hu %hu %hhu", &x, &y, &z );
//        fclose( mapF );
//        return{ x, y, z };
//    }
//}
//
////
////void showNewMap( u16 p_mapIdx ) {
////    //for( u8 i = 0; i < 3; ++i ) {
////    //    for( u8 j = 0; j < 75; ++j ) {
////    //        MapRegionPos m = MapLocations[ i ][ j ];
////    //        if( m.m_ind != p_mapIdx )
////    //            continue;
////    //        acMapRegion = Region( i + 1 );
////    //        printMapLocation( m );
////    //        IO::Oam->oamBuffer[ SQCH_ID ].x = IO::Oam->oamBuffer[ SQCH_ID + 1 ].x = ( m.m_lx + m.m_rx ) / 2 - 8;
////    //        IO::Oam->oamBuffer[ SQCH_ID ].y = IO::Oam->oamBuffer[ SQCH_ID + 1 ].y = ( m.m_ly + m.m_ry ) / 2 - 8;
////    //        IO::Oam->oamBuffer[ SQCH_ID ].isHidden = IO::Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = false;
////    //        updateOAM( true );
////
////    FS::SAV->m_acMapIdx = p_mapIdx;
////
////    //        swiWaitForIRQ( );
////    //        swiWaitForVBlank( );
////    //        return;
////    //    }
////    //}
////}
////
////bool left = false;
////void loadframe( IO::SpriteInfo* p_si, int p_idx, int p_frame, bool p_big = false ) {
////    char buffer[ 50 ];
////    sprintf( buffer, "%i/%i", p_idx, p_frame );
////    if( !p_big )
////        FS::readSpriteData( p_si, "nitro:/PICS/SPRITES/OW/", buffer, 64, 16 );
////    else
////        FS::readSpriteData( p_si, "nitro:/PICS/SPRITES/OW/", buffer, 128, 16 );
////}
////
////void animateHero( int p_dir, int p_frame, bool p_runDisable = false ) {
////    heroIsBig = false;
////
////    left = !left;
////    bool bike = ( MAP::MoveMode )FS::SAV->m_acMoveMode == MAP::MoveMode::BIKE, run = ( keysHeld( ) & KEY_B ) && !p_runDisable;
////    if( p_frame == 0 ) {
////        switch( p_dir ) {
////            case 0:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
////                IO::updateOAM( false );
////                swiWaitForVBlank( );
////                swiWaitForVBlank( );
////                swiWaitForIRQ( );
////                return;
////            case 1:
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                IO::OamTop->oamBuffer[ 0 ].hFlip = true;
////                if( !run ) {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 7, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 8, heroIsBig );
////                } else {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 16, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 17, heroIsBig );
////                }
////
////                IO::updateOAM( false );
////                swiWaitForVBlank( );
////                swiWaitForIRQ( );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                swiWaitForVBlank( );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                return;
////            case 2:
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run ) {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 3, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 4, heroIsBig );
////                } else {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 12, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 13, heroIsBig );
////                }
////                IO::updateOAM( false );
////                swiWaitForVBlank( );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                swiWaitForVBlank( );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                return;
////            case 3:
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run ) {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 7, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 8, heroIsBig );
////                } else {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 16, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 17, heroIsBig );
////                }
////                IO::updateOAM( false );
////                swiWaitForVBlank( );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                swiWaitForVBlank( );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                return;
////            case 4:
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run ) {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 5, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 6, heroIsBig );
////                } else {
////                    if( left )
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 14, heroIsBig );
////                    else
////                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 15, heroIsBig );
////                }
////                IO::updateOAM( false );
////                {swiWaitForVBlank( );  }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                {swiWaitForVBlank( );  }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                return;
////            default:
////                break;
////        }
////    }
////    if( p_frame == 1 ) {
////        switch( p_dir ) {
////            case 0:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
////                IO::updateOAM( false );
////                return;
////            case 1:
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                IO::OamTop->oamBuffer[ 0 ].hFlip = true;
////                IO::updateOAM( false );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
////                else
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 11, heroIsBig );
////                IO::updateOAM( false );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                {swiWaitForVBlank( );  }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 2, 0 );
////                bgUpdate( );
////                return;
////            case 2:
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                IO::updateOAM( false );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
////                else
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 9, heroIsBig );
////                IO::updateOAM( false );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                {swiWaitForVBlank( );  }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, 2 );
////                bgUpdate( );
////                return;
////            case 3:
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                IO::updateOAM( false );
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
////                else
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 11, heroIsBig );
////                IO::updateOAM( false );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                {swiWaitForVBlank( );  }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], -2, 0 );
////                bgUpdate( );
////                return;
////            case 4:
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                if( !run ) {
////                    swiWaitForVBlank( );
////                }
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 1, heroIsBig );
////                else
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 10, heroIsBig );
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                IO::updateOAM( false );
////                bgUpdate( );
////                {swiWaitForVBlank( );  }
////                for( u8 i = 1; i < 4; ++i )
////                    bgScroll( MAP::bgs[ i ], 0, -2 );
////                bgUpdate( );
////                return;
////            default:
////                break;
////        }
////    }
////    if( p_frame == 2 ) {
////        switch( p_dir ) {
////            case 0:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0 );
////                IO::updateOAM( false );
////                return;
////            case 1:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = true;
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
////                //else
////                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,11,heroIsBig);
////                IO::updateOAM( false );
////                return;
////            case 2:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
////                //else
////                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,9,heroIsBig);
////                IO::updateOAM( false );
////                return;
////            case 3:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
////                //else
////                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,11,heroIsBig);
////                IO::updateOAM( false );
////                return;
////            case 4:
////                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
////                if( !run )
////                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 1, heroIsBig );
////                //else
////                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,10,heroIsBig);
////                IO::updateOAM( false );
////                return;
////            default:
////                break;
////        }
////    }
////}
////
////
////bool movePlayerOnMap( s16 p_x, s16 p_y, s16 p_z, bool p_init /*= true*/ ) {
////    bool WTW = ( gMod == DEVELOPER ) && ( keysHeld( ) & KEY_R );
////
////
////    int movedir = 0;
////    int oldx = FS::SAV->m_acposy / 20 + 10, oldy = FS::SAV->m_acposx / 20 + 10;
////    if( oldy < p_x )
////        movedir = 1;
////    else if( oldy > p_x )
////        movedir = 3;
////    else if( oldx < p_y )
////        movedir = 2;
////    else if( oldx > p_y )
////        movedir = 4;
////
////    if( lastmovedata == 0 && acmovedata % 4 == 0 )
////        FS::SAV->m_acposz = p_z = acmovedata / 4;
////
////    IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_2;
////    if( ( verhalten == 0x70 || lstverhalten == 0x70 ) && p_z >= 4 )
////        IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_1;
////    if( acmovedata == 60 ) {
////        if( p_z <= 3 )
////            IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_3;
////        else
////            IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_1;
////    }
////    if( WTW || ( acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) )
////        animateHero( movedir, 0 );
////    else {
////        animateHero( movedir, 2 );
////        swiWaitForVBlank( );
////        bgUpdate( );
////        return false;
////    }
////
////    if( p_x < 10 ) {
////        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
////            for( auto a : acMap->m_anbindungen ) {
////                if( a.m_direction == 'W' && p_y >= a.m_move + 10 && p_y < a.m_move + a.m_mapsx + 10 ) {
////                    ANIMATE_MAP = false;
////                    showNewMap( a.m_mapidx );
////                    strcpy( FS::SAV->m_acMapName, a.m_name );
////                    FS::SAV->m_acMapIdx = a.m_mapidx;
////                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
////                    p_y -= a.m_move;
////                    p_x = a.m_mapsy + 10;
////                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
////                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
////                    animateHero( movedir, 1 );
////                    acMap->draw( p_x - 17, p_y - 18, true );
////                    animateHero( movedir, 2 );
////                    ANIMATE_MAP = true;
////                    return true;
////                }
////            }
////        }
////        return false;
////    }
////    if( p_y < 10 ) {
////        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
////            for( auto a : acMap->m_anbindungen ) {
////                if( a.m_direction == 'N' && p_x >= a.m_move + 10 && p_x < a.m_move + a.m_mapsy + 10 ) {
////                    ANIMATE_MAP = false;
////                    showNewMap( a.m_mapidx );
////                    strcpy( FS::SAV->m_acMapName, a.m_name );
////                    FS::SAV->m_acMapIdx = a.m_mapidx;
////                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
////                    p_x -= a.m_move;
////                    p_y = a.m_mapsx + 10;
////                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
////                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
////                    animateHero( movedir, 1 );
////                    acMap->draw( p_x - 16, p_y - 19, true );
////                    animateHero( movedir, 2 );
////                    ANIMATE_MAP = true;
////                    return true;
////                }
////            }
////        }
////        return false;
////    }
////    if( p_x >= s32( acMap->m_sizey + 10 ) ) {
////        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
////            for( auto a : acMap->m_anbindungen ) {
////                if( a.m_direction == 'E' && p_y >= a.m_move + 10 && p_y < a.m_move + a.m_mapsx + 10 ) {
////                    ANIMATE_MAP = false;
////                    showNewMap( a.m_mapidx );
////                    strcpy( FS::SAV->m_acMapName, a.m_name );
////                    FS::SAV->m_acMapIdx = a.m_mapidx;
////                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
////                    p_y -= a.m_move;
////                    p_x = 9;
////                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
////                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
////                    animateHero( movedir, 1 );
////                    acMap->draw( p_x - 15, p_y - 18, true );
////                    animateHero( movedir, 2 );
////                    ANIMATE_MAP = true;
////                    return true;
////                }
////            }
////        }
////        return false;
////    }
////    if( p_y >= s32( acMap->m_sizex + 10 ) ) {
////
////        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
////            for( auto a : acMap->m_anbindungen ) {
////                if( a.m_direction == 'S'  && p_x >= a.m_move + 10 && p_x < a.m_move + a.m_mapsy + 10 ) {
////                    ANIMATE_MAP = false;
////                    showNewMap( a.m_mapidx );
////                    strcpy( FS::SAV->m_acMapName, a.m_name );
////                    FS::SAV->m_acMapIdx = a.m_mapidx;
////                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
////                    p_x -= a.m_move;
////                    p_y = 9;
////                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
////                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
////                    animateHero( movedir, 1 );
////                    acMap->draw( p_x - 16, p_y - 17, true );
////                    animateHero( movedir, 2 );
////                    ANIMATE_MAP = true;
////                    return true;
////                }
////            }
////        }
////        return false;
////    }
////    if( p_init )
////        acMap->draw( p_x - 16, p_y - 18, p_init );
////    else
////        movePlayer( movedir );
////
////    animateHero( movedir, 1 );
////    swiWaitForVBlank( );
////    animateHero( movedir, 2 );
////    swiWaitForVBlank( );
////    if( p_init )
////        animateHero( lastdir, 2 );
////
////    IO::updateOAM( false );
////
////    return true;
////}
////
////void animateMap( u8 p_frame ) {
////    u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
////    for( size_t i = 0; i < acMap->m_animations.size( ); ++i ) {
////        MAP::Animation& a = acMap->m_animations[ i ];
////        if( ( p_frame ) % ( a.m_speed ) == 0 || a.m_speed == 1 ) {
////            a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
////            swiCopy( &a.m_animationTiles[ a.m_acFrame ], tileMemory + a.m_tileIdx * 32, 16 );
////        }
////    }
////}
////
////void initMapSprites( ) {
////    IO::initOAMTable( false );
////
////    IO::SpriteInfo * SQCHAInfo = &IO::spriteInfoTop[ 0 ];
////    SpriteEntry * SQCHA = &IO::OamTop->oamBuffer[ 0 ];
////    SQCHAInfo->m_oamId = 0;
////    SQCHAInfo->m_width = 16;
////    SQCHAInfo->m_height = 32;
////    SQCHAInfo->m_angle = 0;
////    SQCHAInfo->m_entry = SQCHA;
////    SQCHA->y = 72;
////    SQCHA->isRotateScale = false;
////    SQCHA->blendMode = OBJMODE_NORMAL;
////    SQCHA->isMosaic = true;
////    SQCHA->colorMode = OBJCOLOR_16;
////    SQCHA->shape = OBJSHAPE_TALL;
////    SQCHA->isHidden = false;
////    SQCHA->x = 120;
////    SQCHA->size = OBJSIZE_32;
////    SQCHA->gfxIndex = 0;
////    SQCHA->priority = OBJPRIORITY_2;
////    SQCHA->palette = 0;
////
////    loadframe( SQCHAInfo, FS::SAV->m_overWorldIdx, 0 );
////
////    SQCHAInfo = &IO::spriteInfoTop[ 1 ];
////    SQCHA = &IO::OamTop->oamBuffer[ 1 ];
////    SQCHAInfo->m_oamId = 1;
////    SQCHAInfo->m_width = 32;
////    SQCHAInfo->m_height = 32;
////    SQCHAInfo->m_angle = 0;
////    SQCHAInfo->m_entry = SQCHA;
////    SQCHA->y = 72;
////    SQCHA->isRotateScale = false;
////    SQCHA->blendMode = OBJMODE_NORMAL;
////    SQCHA->isMosaic = true;
////    SQCHA->colorMode = OBJCOLOR_16;
////    SQCHA->shape = OBJSHAPE_SQUARE;
////    SQCHA->isHidden = true;
////    SQCHA->x = 112;
////    SQCHA->size = OBJSIZE_32;
////    SQCHA->gfxIndex = 16;
////    SQCHA->priority = OBJPRIORITY_2;
////    SQCHA->palette = 0;
////
////    IO::SpriteInfo * B2Info = &IO::spriteInfoTop[ 2 ];
////    SpriteEntry * B2 = &IO::OamTop->oamBuffer[ 2 ];
////    B2Info->m_oamId = 2;
////    B2Info->m_width = 64;
////    B2Info->m_height = 64;
////    B2Info->m_angle = 0;
////    B2Info->m_entry = B2;
////    B2->isRotateScale = false;
////    B2->blendMode = OBJMODE_NORMAL;
////    B2->isMosaic = false;
////    B2->colorMode = OBJCOLOR_16;
////    B2->shape = OBJSHAPE_SQUARE;
////    B2->isHidden = true;
////    B2->size = OBJSIZE_64;
////    B2->gfxIndex = 32;
////    B2->priority = OBJPRIORITY_1;
////    B2->palette = 1;
////    B2->x = 64;
////    B2->y = 32;
////
////    B2 = &IO::OamTop->oamBuffer[ 3 ];
////    B2->isRotateScale = false;
////    B2->blendMode = OBJMODE_NORMAL;
////    B2->isMosaic = false;
////    B2->colorMode = OBJCOLOR_16;
////    B2->shape = OBJSHAPE_SQUARE;
////    B2->isHidden = true;
////    B2->size = OBJSIZE_64;
////    B2->gfxIndex = 32;
////    B2->priority = OBJPRIORITY_1;
////    B2->palette = 1;
////    B2->x = 128;
////    B2->y = 32;
////    B2->hFlip = true;
////
////    B2 = &IO::OamTop->oamBuffer[ 4 ];
////    B2->isRotateScale = false;
////    B2->blendMode = OBJMODE_NORMAL;
////    B2->isMosaic = false;
////    B2->colorMode = OBJCOLOR_16;
////    B2->shape = OBJSHAPE_SQUARE;
////    B2->isHidden = true;
////    B2->size = OBJSIZE_64;
////    B2->gfxIndex = 32;
////    B2->priority = OBJPRIORITY_1;
////    B2->palette = 1;
////    B2->x = 64;
////    B2->y = 96;
////    B2->hFlip = false;
////    B2->vFlip = true;
////
////    B2 = &IO::OamTop->oamBuffer[ 5 ];
////    B2->isRotateScale = false;
////    B2->blendMode = OBJMODE_NORMAL;
////    B2->isMosaic = false;
////    B2->colorMode = OBJCOLOR_16;
////    B2->shape = OBJSHAPE_SQUARE;
////    B2->isHidden = true;
////    B2->size = OBJSIZE_64;
////    B2->gfxIndex = 32;
////    B2->priority = OBJPRIORITY_1;
////    B2->palette = 1;
////    B2->x = 128;
////    B2->y = 96;
////    B2->hFlip = true;
////    B2->vFlip = true;
////
////    dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, BigCirc1Pal, &SPRITE_PALETTE[ 16 ], 32 );
////    dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, BigCirc1Tiles, &SPRITE_GFX[ 32 * 32 / sizeof( SPRITE_GFX[ 0 ] ) ], BigCirc1TilesLen );
////    IO::updateOAM( false );
////}
////
////int stepcnt = 0;
////void stepincrease( ) {
////    stepcnt = ( stepcnt + 1 ) % 256;
////    if( stepcnt == 0 ) {
////        for( size_t s = 0; s < 6; ++s ) {
////            pokemon& ac = FS::SAV->m_pkmnTeam[ s ];
////            if( !ac.m_boxdata.m_speciesId )
////                break;
////
////            if( ac.m_boxdata.m_individualValues.m_isEgg ) {
////                ac.m_boxdata.m_steps--;
////                if( ac.m_boxdata.m_steps == 0 ) {
////                    ac.m_boxdata.m_individualValues.m_isEgg = false;
////                    ac.m_boxdata.m_hatchPlace = curMap->getCurrentLocationId( );
////                    ac.m_boxdata.m_hatchDate[ 0 ] = acday;
////                    ac.m_boxdata.m_hatchDate[ 1 ] = acmonth + 1;
////                    ac.m_boxdata.m_hatchDate[ 2 ] = ( acyear + 1900 ) % 100;
////                    char buffer[ 50 ];
////                    sprintf( buffer, "%ls schüpfte\naus dem Ei!", ac.m_boxdata.m_name );
////                    IO::messageBox M( buffer );
////                }
////            } else
////                ac.m_boxdata.m_steps = std::min( 255, ac.m_boxdata.m_steps + 1 );
////        }
////    }
////}
////
////
////void shoUseAttack( u16 p_pkmIdx, bool p_female, bool p_shiny ) {
////    IO::OamTop->oamBuffer[ 0 ].isHidden = true;
////    IO::OamTop->oamBuffer[ 1 ].isHidden = false;
////    for( u8 i = 0; i < 5; ++i ) {
////        loadframe( &IO::spriteInfoTop[ 1 ], FS::SAV->m_player.m_picNum + 4, i, true );
////        IO::updateOAM( false );
////        swiWaitForVBlank( );
////        swiWaitForVBlank( );
////        swiWaitForVBlank( );
////    }
////    for( u8 i = 0; i < 4; ++i )
////        IO::OamTop->oamBuffer[ 2 + i ].isHidden = false;
////    if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 6, 2, 96, false, p_shiny, p_female ) ) {
////        IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 6, 2, 96, false, p_shiny, !p_female );
////    }
////    IO::updateOAM( false );
////
////    for( u8 i = 0; i < 40; ++i )
////        swiWaitForVBlank( );
////
////    //animateHero(lastdir,2);
////    IO::OamTop->oamBuffer[ 0 ].isHidden = false;
////    IO::OamTop->oamBuffer[ 1 ].isHidden = true;
////    for( u8 i = 0; i < 8; ++i )
////        IO::OamTop->oamBuffer[ 2 + i ].isHidden = true;
////    IO::updateOAM( false );
////}
