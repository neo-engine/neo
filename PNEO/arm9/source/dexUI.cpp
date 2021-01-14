/*
Pokémon neo
------------------------------

file        : dexUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2021
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

#include <algorithm>
#include <cstdio>

#include "choiceBox.h"
#include "defines.h"
#include "dex.h"
#include "dexUI.h"
#include "fs.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

#include "dexsub1.h"
#include "dexsub2.h"
#include "dexsub3.h"
#include "dexsub4.h"
#include "dextop1.h"
#include "dextop2.h"

#include "dexsp1.h"
#include "dexsp2.h"

#include "crown1.h"
#include "crown2.h"
#include "crown3.h"
#include "crown4.h"

#include "caught.h"

#define SPR_WINDOW_NAME_OAM    0
#define SPR_PKMN_START_OAM     6
#define SPR_PKMN2_START_OAM    10
#define SPR_TEXTBOX_OAM        14
#define SPR_NAMEBOX_OAM        20
#define SPR_TYPE_OAM( p_type ) ( 30 + ( p_type ) )
#define SPR_STAR_START_OAM     32
#define SPR_CAUGHT_OAM         70

#define SPR_PKMN_X           ( 128 - 48 )
#define SPR_PKMN_Y           ( 108 )
#define SPR_TYPE_X( p_type ) ( 256 - 20 - 64 + 32 * ( p_type ) )
#define SPR_TYPE_Y( p_type ) ( 112 )

#define SPR_BOX_PAL            0
#define SPR_PKMN_PAL           1
#define SPR_PKMN2_PAL          2
#define SPR_TEXTBOX_PAL        3
#define SPR_NAMEBOX_PAL        4
#define SPR_STAR_PAL           5
#define SPR_TYPE_PAL( p_type ) ( 6 + ( p_type ) )
#define SPR_SHADOW_PAL         8
#define SPR_CAUGHT_PAL         9

#define SPR_X_OAM_SUB         0
#define SPR_DX1_OAM_SUB       1
#define SPR_DX2_OAM_SUB       2
#define SPR_DX3_OAM_SUB       3
#define SPR_CAUGHT_OAM_SUB    4
#define SPR_BOX_START_OAM_SUB 5

#define SPR_NAT_DX_START_OAM_SUB     20
#define SPR_NAT_PKMN_START_OAM_SUB   30
#define SPR_NAT_CAUGHT_START_OAM_SUB 38

#define SPR_X_PAL_SUB       0
#define SPR_BOX_PAL_SUB     7
#define SPR_BOX_SEL_PAL_SUB 8

namespace DEX {
    void dexUI::init( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        _touchPostiions = std::vector<std::pair<IO::inputTarget, u8>>( );

        IO::initOAMTable( true );
        IO::initOAMTable( false );

        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        vramSetBankD( VRAM_D_SUB_SPRITE );
        videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                         | ( ( DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
                               | DISPLAY_SPR_1D_BMP_SIZE_128 | ( 5 << 28 ) | 2 )
                             & 0xffffff0 ) );

        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );

        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

        REG_BLDCNT     = BLEND_NONE;
        REG_BLDCNT_SUB = BLEND_NONE;
        bgUpdate( );

        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );

        dmaCopy( dexsub2Bitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaCopy( dextop2Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaCopy( dexsub3Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );
        dmaCopy( dexsub3Bitmap, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaCopy( dextop1Pal, BG_PALETTE, 200 * 2 );
        dmaCopy( dextop1Pal, BG_PALETTE_SUB, 200 * 2 );

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );
        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = IO::BG_PAL( i );
            pal[ 0 ]             = 0;
            pal[ IO::COLOR_IDX ] = IO::RGB( 22, 22, 22 );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = IO::RGB( 16, 16, 16 );
            pal[ IO::BLACK_IDX ] = IO::RGB( 8, 8, 8 );
            pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );
        }

        // Bottom screen sprites
        u16 tileCnt = 0;

        // x
        tileCnt = IO::loadSpriteB( "UI/x_16_16", SPR_X_OAM_SUB, tileCnt, 236, 172, 16, 16, false,
                                   false, false, OBJPRIORITY_2, true );

        // dex entry bg

        // no entry
        tileCnt = IO::loadSpriteB( SPR_DX1_OAM_SUB, tileCnt, 64, 64, 32, 64, dexsp1Pal, dexsp1Tiles,
                                   dexsp1TilesLen, false, false, true, OBJPRIORITY_3, true );
        tileCnt = IO::loadSpriteB( SPR_DX2_OAM_SUB, tileCnt, 96, 64, 32, 64, dexsp2Pal, dexsp2Tiles,
                                   dexsp2TilesLen, false, false, true, OBJPRIORITY_3, true );
        tileCnt = IO::loadSpriteB( SPR_DX3_OAM_SUB, tileCnt, 96, 64, 32, 64, dexsp1Pal, dexsp1Tiles,
                                   dexsp1TilesLen, false, false, true, OBJPRIORITY_0, true, true,
                                   0b1001'0100'0001'1101 );

        // top screen sprites
        tileCnt = 0;

        // window name bg ("hoenn dex" / "national dex")
        for( u8 i = 0; i < 4; ++i ) {
            IO::loadSprite( SPR_WINDOW_NAME_OAM + 4 - i, SPR_BOX_PAL, tileCnt, 0 + 22 * i, 0, 64,
                            32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false );
        }
        tileCnt
            = IO::loadSprite( "SEL/noselection_128_32_2", SPR_WINDOW_NAME_OAM, SPR_BOX_PAL, tileCnt,
                              4 + 22 * 3, 0, 64, 32, false, false, false, OBJPRIORITY_3, false );

        // pkmn sprite
        tileCnt = IO::loadPKMNSprite( 0, 128 - 48, 24, SPR_PKMN_START_OAM, SPR_PKMN_PAL, tileCnt,
                                      false, false, false, false, false, 0 );
        IO::OamTop->oamBuffer[ SPR_PKMN2_START_OAM ].gfxIndex = tileCnt;
        IO::OamTop->oamBuffer[ SPR_PKMN2_START_OAM ].palette  = SPR_PKMN2_PAL;
        tileCnt += 144;

        // pkmn dex entry box
        for( u8 i = 1; i < 4; ++i ) {
            IO::loadSprite( SPR_TEXTBOX_OAM + i, SPR_TEXTBOX_PAL, tileCnt, 12 + 60 * i, 192 - 64,
                            64, 64, 0, 0, 0, false, false, false, OBJPRIORITY_3, false );
        }
        tileCnt = IO::loadSprite( "UI/pg3", SPR_TEXTBOX_OAM, SPR_TEXTBOX_PAL, tileCnt, 12, 192 - 64,
                                  64, 64, false, false, false, OBJPRIORITY_3, false );

        // pkmn name box
        for( u8 i = 1; i < 9; ++i ) {
            IO::loadSprite( SPR_NAMEBOX_OAM + i, SPR_NAMEBOX_PAL, tileCnt, 12 + 24 * i,
                            192 - 64 - 18, 32, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3,
                            false );
        }
        tileCnt = IO::loadSprite( "SEL/noselection_64_20", SPR_NAMEBOX_OAM, SPR_NAMEBOX_PAL,
                                  tileCnt, 12, 192 - 64 - 18, 32, 32, false, false, false,
                                  OBJPRIORITY_3, false );

        // pkmn base stat stars
        for( u8 i = 0; i < 30; ++i ) {
            u8 x = 73 - 13 * ( i % 10 );
            if( i % 10 >= 5 ) x = 170 + 13 * ( i % 5 );
            IO::loadSprite( SPR_STAR_START_OAM + i, SPR_STAR_PAL, tileCnt, x, 42 + 26 * ( i / 10 ),
                            16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_2, false );
        }
        tileCnt = IO::loadSprite( "UI/star", SPR_STAR_START_OAM, SPR_STAR_PAL, tileCnt,
                                  IO::OamTop->oamBuffer[ SPR_STAR_START_OAM ].x,
                                  IO::OamTop->oamBuffer[ SPR_STAR_START_OAM ].y, 16, 16, false,
                                  false, true, OBJPRIORITY_2, false );

        // pkmn types

        IO::OamTop->oamBuffer[ SPR_TYPE_OAM( 0 ) ].gfxIndex = tileCnt;
        tileCnt += 8;
        IO::OamTop->oamBuffer[ SPR_TYPE_OAM( 1 ) ].gfxIndex = tileCnt;
        tileCnt += 8;

        // init caught ball
        tileCnt = IO::loadSprite( SPR_CAUGHT_OAM, SPR_CAUGHT_PAL, tileCnt, 20, 112, 16, 16,
                                  caughtPal, caughtTiles, caughtTilesLen, false, false, true,
                                  OBJPRIORITY_0, false );

        IO::updateOAM( true );
        IO::updateOAM( false );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
    }

    std::vector<std::pair<IO::inputTarget, u8>> dexUI::drawModeChoice( bool p_showLocalDex,
                                                                       bool p_showNationalDex ) {
        char buffer[ 10 ];
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        IO::initOAMTable( true );
        IO::initOAMTable( false );

        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );

        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

        REG_BLDCNT     = BLEND_NONE;
        REG_BLDCNT_SUB = BLEND_NONE;
        bgUpdate( );

        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaCopy( dexsub1Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );
        dmaCopy( dextop1Bitmap, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaCopy( dextop1Pal, BG_PALETTE, 200 * 2 );
        dmaCopy( dextop1Pal, BG_PALETTE_SUB, 200 * 2 );

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );
        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = IO::BG_PAL( i );
            pal[ 0 ]             = 0;
            pal[ IO::COLOR_IDX ] = IO::RGB( 22, 22, 22 );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = IO::RGB( 16, 16, 16 );
            pal[ IO::BLACK_IDX ] = IO::RGB( 8, 8, 8 );
            pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );
        }

        // top screen
        u16 tileCnt = 0;
        u16 y       = 80;
        u16 x       = 128 - 72;

        tileCnt = IO::loadSprite( "SEL/noselection_96_32_1", 0, 0, tileCnt, x, y, 16, 32, false,
                                  false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "SEL/noselection_96_32_2", 1, 0, tileCnt, x + 16, y, 16, 32,
                                  false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        for( u8 j = 2; j < 8; j++ ) {
            IO::loadSprite( 8 + 1 - j, 0, IO::OamTop->oamBuffer[ 1 ].gfxIndex, x + j * 16, y, 16,
                            32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
        }
        IO::loadSprite( 8, 0, IO::OamTop->oamBuffer[ 0 ].gfxIndex, x + 8 * 16, y, 16, 32, 0, 0, 0,
                        true, true, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );

        IO::boldFont->printString( GET_STRING( 576 ), 128, y + 7, false, IO::font::CENTER );

        // bottom screen
        tileCnt = 0;

        // Back/exit choice

        tileCnt = IO::loadSprite( "UI/x_16_16", SPR_X_OAM_SUB, SPR_X_PAL_SUB, tileCnt, 236, 172, 16,
                                  16, false, false, false, OBJPRIORITY_1, true, OBJMODE_NORMAL );

        y       = 48;
        tileCnt = IO::loadSprite( "SEL/noselection_64_20", 1, 1, tileCnt, x, y, 32, 32, false,
                                  false, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        if( p_showLocalDex ) {
            if( p_showNationalDex ) { y = 16; }

            for( u8 j = 0; j < 6; j++ ) {
                IO::loadSprite( j + 1, 1, IO::Oam->oamBuffer[ 1 ].gfxIndex, x + j * 16, y, 32, 32,
                                0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }
            IO::loadSprite( 7, 1, IO::Oam->oamBuffer[ 1 ].gfxIndex, x + 7 * 16, y - 12, 32, 32, 0,
                            0, 0, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            IO::regularFont->printStringC( GET_STRING( 577 ), 128, y + 2, true, IO::font::CENTER );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( GET_STRING( 579 ), 128 - 32, y + 21, true,
                                           IO::font::LEFT );

            IO::regularFont->printStringC( GET_STRING( 580 ), 128 - 32, y + 21 + 14, true,
                                           IO::font::LEFT );

            snprintf( buffer, 9, "%03hu", SAVE::SAV.getActiveFile( ).getLocalSeenCount( ) );
            IO::regularFont->printStringC( buffer, 128 + 31, y + 21, true, IO::font::RIGHT );
            snprintf( buffer, 9, "%03hu", SAVE::SAV.getActiveFile( ).getLocalCaughtCount( ) );
            IO::regularFont->printStringC( buffer, 128 + 31, y + 21 + 14, true, IO::font::RIGHT );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );

            res.push_back( std::pair( IO::inputTarget( x, y, 255 - x, y + 20 ), 0 ) );

            // crown sprites for completed dex
            if( SAVE::SAV.getActiveFile( ).getLocalCaughtCount( ) == LOCAL_DEX_SIZE ) {
                tileCnt = IO::loadSprite( 20, 3, tileCnt, 255 - x + 6, y + 3, 16, 16, crown3Pal,
                                          crown3Tiles, crown3TilesLen, false, false, false,
                                          OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).localDexCompleted( ) ) {
                tileCnt = IO::loadSprite( 20, 3, tileCnt, 255 - x + 6, y + 3, 16, 16, crown2Pal,
                                          crown2Tiles, crown2TilesLen, false, false, false,
                                          OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).localDexSeenCompleted( ) ) {
                tileCnt = IO::loadSprite( 20, 3, tileCnt, 255 - x + 6, y + 3, 16, 16, crown1Pal,
                                          crown1Tiles, crown1TilesLen, false, false, false,
                                          OBJPRIORITY_3, true );
            }
        } else {
            res.push_back(
                std::pair( IO::inputTarget( 1, 1, 0, 0 ), IO::choiceBox::DISABLED_CHOICE ) );
        }
        if( p_showNationalDex ) {
            if( p_showLocalDex ) { y = 76; }

            for( u8 j = 0; j < 6; j++ ) {
                IO::loadSprite( j + 8, 1, IO::Oam->oamBuffer[ 1 ].gfxIndex, x + j * 16, y, 32, 32,
                                0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }
            IO::loadSprite( 14, 1, IO::Oam->oamBuffer[ 1 ].gfxIndex, x + 7 * 16, y - 12, 32, 32, 0,
                            0, 0, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            IO::regularFont->printStringC( GET_STRING( 578 ), 128, y + 2, true, IO::font::CENTER );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( GET_STRING( 579 ), 128 - 32, y + 21, true,
                                           IO::font::LEFT );
            IO::regularFont->printStringC( GET_STRING( 580 ), 128 - 32, y + 21 + 14, true,
                                           IO::font::LEFT );

            snprintf( buffer, 9, "%03hu", SAVE::SAV.getActiveFile( ).getSeenCount( ) );
            IO::regularFont->printStringC( buffer, 128 + 31, y + 21, true, IO::font::RIGHT );
            snprintf( buffer, 9, "%03hu", SAVE::SAV.getActiveFile( ).getCaughtCount( ) );
            IO::regularFont->printStringC( buffer, 128 + 31, y + 21 + 14, true, IO::font::RIGHT );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );

            res.push_back( std::pair( IO::inputTarget( x, y, 255 - x, y + 20 ), 1 ) );

            // crown sprites for completed dex
            if( SAVE::SAV.getActiveFile( ).getCaughtCount( ) == MAX_PKMN ) {
                tileCnt = IO::loadSprite( 21, 4, tileCnt, 255 - x + 6, y + 3, 16, 16, crown3Pal,
                                          crown3Tiles, crown3TilesLen, false, false, false,
                                          OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).dexCompleted( ) ) {
                tileCnt = IO::loadSprite( 21, 4, tileCnt, 255 - x + 6, y + 3, 16, 16, crown2Pal,
                                          crown2Tiles, crown2TilesLen, false, false, false,
                                          OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).dexSeenCompleted( ) ) {
                tileCnt = IO::loadSprite( 21, 4, tileCnt, 255 - x + 6, y + 3, 16, 16, crown1Pal,
                                          crown1Tiles, crown1TilesLen, false, false, false,
                                          OBJPRIORITY_3, true );
            }

        } else {
            res.push_back(
                std::pair( IO::inputTarget( 1, 1, 0, 0 ), IO::choiceBox::DISABLED_CHOICE ) );
        }

        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::EXIT_CHOICE ) );
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::BACK_CHOICE ) );

        IO::copySpritePal( IO::SELECTED_SPR_PAL, 2, 0, 2 * 8, true );
        IO::updateOAM( false );
        IO::updateOAM( true );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
        return res;
    }

    void dexUI::changeMode( u8 p_newMode ) {
        if( p_newMode > 1 ) { return; }
        _mode = p_newMode;

        IO::printRectangle( 0, 6, 97, 22, false, 0 );
        if( p_newMode == 0 ) { // local dex
            IO::regularFont->printStringC( GET_STRING( 577 ), 94 / 2 + 1, 5, false,
                                           IO::font::CENTER );
        } else if( p_newMode == 1 ) { // national dex
            IO::regularFont->printStringC( GET_STRING( 578 ), 94 / 2 + 1, 5, false,
                                           IO::font::CENTER );
        }
    }

    void dexUI::selectMode( u8 p_newMode ) {
        if( p_newMode > 1 ) { return; }

        for( u8 j = 0; j < 2 * 7; j++ ) { IO::Oam->oamBuffer[ 1 + j ].palette = 1; }
        for( u8 j = 7 * p_newMode; j < 7 + 7 * p_newMode; j++ ) {
            IO::Oam->oamBuffer[ 1 + j ].palette = 2;
        }
        IO::updateOAM( true );
    }

    void dexUI::drawPkmnInfo( u16 p_pkmnId, u8 p_page, u8 p_forme, bool p_shiny, bool p_female,
                              bool p_bottom ) {
        auto     oamTop = !p_bottom ? IO::OamTop->oamBuffer : IO::Oam->oamBuffer;
        pkmnData data   = getPkmnData( p_pkmnId, p_forme );

        bool seen   = SAVE::SAV.getActiveFile( ).seen( p_pkmnId );
        bool caught = SAVE::SAV.getActiveFile( ).caught( p_pkmnId );

        char buffer[ 100 ];

        if( p_page == 0 ) {
            // load pkmn sprite
            IO::loadPKMNSprite( seen ? p_pkmnId : 0, SPR_PKMN_X,
                                SPR_PKMN_Y - 96 + IO::pkmnSpriteHeight( seen ? p_pkmnId : 0 ),
                                SPR_PKMN_START_OAM, SPR_PKMN_PAL,
                                oamTop[ SPR_PKMN_START_OAM ].gfxIndex, p_bottom, p_shiny, p_female,
                                false, false, seen ? p_forme : 0 );
            if( seen && !caught ) {
                for( u8 i = 0; i < 4; ++i ) {
                    oamTop[ SPR_PKMN_START_OAM + i ].palette = SPR_SHADOW_PAL;
                }
            }
            // load types
            if( caught ) {
                IO::loadTypeIcon( data.m_baseForme.m_types[ 1 ], SPR_TYPE_X( 1 ), SPR_TYPE_Y( 1 ),
                                  SPR_TYPE_OAM( 1 ), SPR_TYPE_PAL( 1 ),
                                  oamTop[ SPR_TYPE_OAM( 1 ) ].gfxIndex, p_bottom,
                                  CURRENT_LANGUAGE );
                if( data.m_baseForme.m_types[ 0 ] != data.m_baseForme.m_types[ 1 ] ) {
                    IO::loadTypeIcon( data.m_baseForme.m_types[ 0 ], SPR_TYPE_X( 0 ),
                                      SPR_TYPE_Y( 0 ), SPR_TYPE_OAM( 0 ), SPR_TYPE_PAL( 0 ),
                                      oamTop[ SPR_TYPE_OAM( 0 ) ].gfxIndex, p_bottom,
                                      CURRENT_LANGUAGE );
                } else {
                    oamTop[ SPR_TYPE_OAM( 0 ) ].isHidden = true;
                }
            } else {
                IO::loadTypeIcon( UNKNOWN, SPR_TYPE_X( 1 ), SPR_TYPE_Y( 1 ), SPR_TYPE_OAM( 1 ),
                                  SPR_TYPE_PAL( 1 ), oamTop[ SPR_TYPE_OAM( 1 ) ].gfxIndex, p_bottom,
                                  CURRENT_LANGUAGE );
                oamTop[ SPR_TYPE_OAM( 0 ) ].isHidden = true;
            }

            IO::printRectangle( 20, 32, 235, 192, false, 0 );
            // pkmn name
            if( seen ) {
                if( !p_forme ) {
                    IO::regularFont->printStringC( getDisplayName( p_pkmnId, p_forme ).c_str( ),
                                                   128, 112, p_bottom, IO::font::CENTER );
                } else {
                    IO::regularFont->printStringC( getDisplayName( p_pkmnId, p_forme ).c_str( ),
                                                   SPR_TYPE_X( 0 ) - 4, 112, p_bottom,
                                                   IO::font::RIGHT );
                }
            } else {
                IO::regularFont->printStringC( GET_STRING( 581 ), 128, 112, p_bottom,
                                               IO::font::CENTER );
            }

            // dex no
            snprintf( buffer, 99, "%03hu%c", _mode ? p_pkmnId : getDexNo( p_pkmnId ),
                      p_forme ? 'a' + p_forme - 1 : 0 );
            IO::regularFont->printStringC( buffer, 36, 112, p_bottom, IO::font::LEFT );

            // pokeball icon if pkmn caught
            if( caught ) {
                oamTop[ SPR_CAUGHT_OAM ].isHidden = false;
                oamTop[ SPR_CAUGHT_OAM ].palette  = SPR_CAUGHT_PAL;
            } else if( seen ) {
                oamTop[ SPR_CAUGHT_OAM ].isHidden = false;
                oamTop[ SPR_CAUGHT_OAM ].palette  = SPR_SHADOW_PAL;
            } else {
                oamTop[ SPR_CAUGHT_OAM ].isHidden = true;
            }

            // dex entry
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            if( caught ) {
                snprintf( buffer, 99, GET_STRING( 582 ),
                          getSpeciesName( p_pkmnId, p_forme ).c_str( ),
                          data.m_baseForme.m_size / 10.0, data.m_baseForme.m_weight / 10.0 );
                IO::regularFont->setColor( IO::GRAY_IDX, 1 );
                IO::regularFont->printStringC( buffer, 128, 128, p_bottom, IO::font::CENTER );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->printBreakingStringC( getDexEntry( p_pkmnId, p_forme ).c_str( ),
                                                       128, 141, 214, p_bottom, IO::font::CENTER,
                                                       12 );

                // base stats

                for( u8 i = 0; i < 30; ++i ) {
                    u8 bs;
                    if( i / 5 == 0 ) {
                        // HP
                        bs = data.m_baseForme.m_bases[ 0 ];
                    } else if( i / 5 == 1 ) {
                        // Speed
                        bs = data.m_baseForme.m_bases[ 5 ];
                    } else {
                        bs = data.m_baseForme.m_bases[ i / 5 - 1 ];
                    }

                    oamTop[ i + SPR_STAR_START_OAM ].palette
                        = ( bs < 30 + 30 * ( i % 5 ) ) ? SPR_SHADOW_PAL : SPR_STAR_PAL;
                    oamTop[ i + SPR_STAR_START_OAM ].isHidden = false;
                }
                IO::regularFont->printStringC( GET_STRING( 126 ), 86, 49 - 20, false,
                                               IO::font::RIGHT );
                IO::regularFont->printStringC( GET_STRING( 127 ), 86, 74 - 21, false,
                                               IO::font::RIGHT );
                IO::regularFont->printStringC( GET_STRING( 129 ), 86, 99 - 20, false,
                                               IO::font::RIGHT );

                IO::regularFont->printStringC( GET_STRING( 131 ), 170, 49 - 20, false );
                IO::regularFont->printStringC( GET_STRING( 128 ), 170, 74 - 21, false );
                IO::regularFont->printStringC( GET_STRING( 130 ), 170, 99 - 20, false );
            } else {
                for( u8 i = 0; i < 30; ++i ) { oamTop[ i + SPR_STAR_START_OAM ].isHidden = true; }
                IO::regularFont->printStringC( GET_STRING( 0 ), 128, 150, p_bottom,
                                               IO::font::CENTER );
            }
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        }

        IO::updateOAM( p_bottom );
    }

    void dexUI::selectNationalIndex( u16 p_pkmnIdx, bool p_bottom ) {
        drawPkmnInfo( p_pkmnIdx, 0, 0, false, false, !p_bottom );

        // draw bottom screen, vertical list in national dex mode
        IO::printRectangle( 30, 30, 220, 160, true, 0 );
    }

    void dexUI::selectLocalPageSlot( u16 p_page, u8 p_slot, bool p_bottom ) {
        drawPkmnInfo( LOCAL_DEX_PAGES[ p_page ][ p_slot ] % ALOLAN_FORME, 0,
                      LOCAL_DEX_PAGES[ p_page ][ p_slot ] > ALOLAN_FORME, false, false, !p_bottom );

        IO::printRectangle( 30, 30, 220, 160, true, 0 );
    }

    void dexUI::highlightButton( u8 p_button, bool p_bottom ) {
        (void) p_button;
        (void) p_bottom;
    }

    std::vector<std::pair<IO::inputTarget, u8>> dexUI::getTouchPositions( bool p_bottom ) {
        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        return res;
    }
} // namespace DEX
