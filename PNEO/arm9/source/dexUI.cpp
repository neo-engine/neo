/*
Pokémon neo
------------------------------

file        : dexUI.cpp
author      : Philip Wellnitz
description :

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

#include <algorithm>
#include <cstdio>

#include "choiceBox.h"
#include "defines.h"
#include "dex.h"
#include "dexUI.h"
#include "fs.h"
#include "nav.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

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
#define SPR_DX3_OAM_SUB       1
#define SPR_DX1_OAM_SUB       2
#define SPR_DX2_OAM_SUB       3
#define SPR_CAUGHT_OAM_SUB    4
#define SPR_BOX_START_OAM_SUB 5

#define SPR_START_GFX_SUB 240

#define PKMN_BOX_HG_I 32
#define PKMN_BOX_WD_I 32
#define PKMN_BOX_HG   ( PKMN_BOX_HG_I + 8 )
#define PKMN_BOX_WD   ( PKMN_BOX_WD_I + 8 )

// 10 slots per entry (1 spr window, 1 pkmn/number sprite, 2 pkmn name, 1 caught, 5 name_bg)
#define SPR_NAT_DX_START_OAM_SUB 20
#define SPR_NAT_DX_START_GFX_SUB SPR_START_GFX_SUB
#define SPR_NAT_DX_SLOT_SIZE     10
#define SPR_NAT_DX_SLOT_GFX_SIZE 56
#define SPR_NAT_DX_SLOT_COUNT    7
#define SPR_NAT_DX_HIDDEN( p_slot ) \
    ( ( ( p_slot ) == 0 || ( p_slot ) == SPR_NAT_DX_SLOT_COUNT - 1 ) )
#define SPR_NAT_DX_BG_POS_X( p_slot ) 40
#define SPR_NAT_DX_BG_POS_Y( p_slot ) ( -64 + PKMN_BOX_HG * ( p_slot ) )

#define SPR_LOC_DX_START_OAM_SUB              20
#define SPR_LOC_DX_START_GFX_SUB              SPR_START_GFX_SUB
#define SPR_LOC_DX_SLOT_SIZE                  4
#define SPR_LOC_DX_SLOT_GFX_SIZE              32
#define SPR_LOC_DX_SLOT_ROW_COUNT             3
#define SPR_LOC_DX_SLOT_COL_COUNT             6
#define SPR_LOC_DX_BG_POS_X( p_page, p_slot ) ( 8 + PKMN_BOX_WD * ( p_page ) )
#define SPR_LOC_DX_BG_POS_Y( p_page, p_slot ) ( 24 + PKMN_BOX_HG * ( p_slot ) )

#define SPR_X_PAL_SUB 0

namespace DEX {

    const u16 TEXT_PAL[ 16 ] = { 0, IO::BLACK, IO::GRAY, IO::WHITE, IO::BLUE, IO::BLUE };

    void initSubSprites( ) {
        IO::initOAMTable( true );
        SpriteRotation* oamM = IO::Oam->matrixBuffer;
        SpriteEntry*    oam  = IO::Oam->oamBuffer;

        oamM[ 0 ].hdx = ( 1LLU << 8 ) | ( 1 << 6 );
        oamM[ 0 ].hdy = ( 0 << 8 );
        oamM[ 0 ].vdx = ( 0 << 8 );
        oamM[ 0 ].vdy = ( 1LLU << 8 ) | ( 1 << 6 );

        oamM[ 1 ].hdx = ( 1LLU << 8 ) | ( 1LLU << 7 );
        oamM[ 1 ].hdy = ( 0 << 8 );
        oamM[ 1 ].vdx = ( 0 << 8 );
        oamM[ 1 ].vdy = ( 1LLU << 8 ) | ( 1LLU << 7 );

        // Bottom screen sprites
        u16 tileCnt = 0;

        // x
        tileCnt = IO::loadSpriteB( "UI/x_16_16", SPR_X_OAM_SUB, tileCnt, 236, 172, 16, 16, false,
                                   false, false, OBJPRIORITY_2, true );

        // no entry
        tileCnt = IO::loadSpriteB( "DX/dexsp1", SPR_DX1_OAM_SUB, tileCnt, 64, 64, 32, 64, false,
                                   false, true, OBJPRIORITY_3, true );
        tileCnt = IO::loadSpriteB( "DX/dexsp2", SPR_DX2_OAM_SUB, tileCnt, 96, 64, 32, 64, false,
                                   false, true, OBJPRIORITY_3, true );
        tileCnt = IO::loadSpriteB( "DX/dexsp1", SPR_DX3_OAM_SUB, tileCnt, 96, 64, 32, 64, false,
                                   false, true, OBJPRIORITY_3, true, true, 0b1001'0100'0001'1101 );

        // caught ball
        tileCnt = IO::loadSpriteB( "DX/caught", SPR_CAUGHT_OAM_SUB, tileCnt, 20, 112, 16, 16, false,
                                   false, true, OBJPRIORITY_3, true );

        // pkmn name box sub
        tileCnt = IO::loadSpriteB( "SEL/noselection_64_20", SPR_BOX_START_OAM_SUB, tileCnt, 18, 0,
                                   32, 32, false, false, false, OBJPRIORITY_3, true );
        for( u8 i = 0; i < 10; ++i ) {
            oam[ SPR_BOX_START_OAM_SUB + i ] = oam[ SPR_BOX_START_OAM_SUB ];
            oam[ SPR_BOX_START_OAM_SUB + i ].x += 24 * i;
        }

        dmaFillHalfWords( 0, &SPRITE_PALETTE[ SPR_SHADOW_PAL * 16 ], 32 );
        IO::updateOAM( true );
    }

    void moveSpriteBlock( u8 p_OAMStart, u8 p_blockSize, s16 p_x, s16 p_y, bool p_bottom ) {
        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;
        for( u8 i = 0; i < p_blockSize; ++i ) {
            oam[ p_OAMStart + i ].x += p_x;
            oam[ p_OAMStart + i ].y += p_y;
        }
    }

    void dexUI::init( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );

        IO::initOAMTable( false );
        _nationalSelectedIdx = 0;
        _localSelectedPage   = -1;

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

        FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/DEX/", "dexsub2", 0, 49152,
                             true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/DEX/", "dexsub3", 2 * 200,
                             49152, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/DEX/", "dextop2", 0, 49152,
                             false );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/DEX/", "dexsub3", 2 * 200, 49152,
                             false );

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );
        //        SpriteEntry* oam = IO::Oam->oamBuffer;

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

        initSubSprites( );

        // top screen sprites
        u16 tileCnt = 0;

        // window name bg ("hoenn dex" / "national dex")
        for( u8 i = 0; i < 4; ++i ) {
            IO::loadSprite( SPR_WINDOW_NAME_OAM + 4 - i, SPR_BOX_PAL, tileCnt, 0 + 22 * i, 0, 64,
                            32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false );
        }
        tileCnt
            = IO::loadSprite( "SEL/noselection_128_32_2", SPR_WINDOW_NAME_OAM, SPR_BOX_PAL, tileCnt,
                              4 + 22 * 3, 0, 64, 32, false, false, false, OBJPRIORITY_3, false );

        // pkmn sprite
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM ].gfxIndex = tileCnt;
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM ].palette  = SPR_PKMN_PAL;
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM ].x        = 128 - 48;
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM ].x        = 24;
        tileCnt += 144;

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
        tileCnt = IO::loadSprite( "DX/caught", SPR_CAUGHT_OAM, SPR_CAUGHT_PAL, tileCnt, 20, 112, 16,
                                  16, false, false, true, OBJPRIORITY_0, false );

        IO::updateOAM( true );
        IO::updateOAM( false );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );

        _backChoice     = { IO::touchInputTarget( IO::Oam->oamBuffer[ SPR_X_OAM_SUB ].x,
                                                  IO::Oam->oamBuffer[ SPR_X_OAM_SUB ].y,
                                                  IO::Oam->oamBuffer[ SPR_X_OAM_SUB ].x + 32,
                                                  IO::Oam->oamBuffer[ SPR_X_OAM_SUB ].y + 32 ),
                        0 };
        _touchPositions = std::vector<std::pair<IO::touchInputTarget, u16>>( );
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
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/DEX/", "dexsub1", 2 * 200,
                             49152, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/DEX/", "dextop1", 2 * 200, 49152,
                             false );

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
                tileCnt = IO::loadSprite( "DX/crown3", 20, 3, tileCnt, 255 - x + 6, y + 3, 16, 16,
                                          false, false, false, OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).localDexCompleted( ) ) {
                tileCnt = IO::loadSprite( "DX/crown2", 20, 3, tileCnt, 255 - x + 6, y + 3, 16, 16,
                                          false, false, false, OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).localDexSeenCompleted( ) ) {
                tileCnt = IO::loadSprite( "DX/crown1", 20, 3, tileCnt, 255 - x + 6, y + 3, 16, 16,
                                          false, false, false, OBJPRIORITY_3, true );
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
                tileCnt = IO::loadSprite( "DX/crown3", 21, 4, tileCnt, 255 - x + 6, y + 3, 16, 16,
                                          false, false, false, OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).dexCompleted( ) ) {
                tileCnt = IO::loadSprite( "DX/crown2", 21, 4, tileCnt, 255 - x + 6, y + 3, 16, 16,
                                          false, false, false, OBJPRIORITY_3, true );
            } else if( SAVE::SAV.getActiveFile( ).dexSeenCompleted( ) ) {
                tileCnt = IO::loadSprite( "DX/crown1", 21, 4, tileCnt, 255 - x + 6, y + 3, 16, 16,
                                          false, false, false, OBJPRIORITY_3, true );
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

        char buffer[ 90 ];

        initSubSprites( );
        IO::printRectangle( 0, 6, 97, 22, false, 0 );
        IO::printRectangle( 20, 0, 235, 22, true, 0 );
        if( p_newMode == 0 ) { // local dex
            IO::regularFont->printStringC( GET_STRING( 577 ), 94 / 2 + 1, 5, false,
                                           IO::font::CENTER );
            localInitSub( true );
            std::snprintf( buffer, 89, GET_STRING( 583 ),
                           SAVE::SAV.getActiveFile( ).getLocalSeenCount( ) );
            IO::regularFont->printStringC( buffer, 74, 2, true, IO::font::CENTER );
            std::snprintf( buffer, 89, GET_STRING( 584 ),
                           SAVE::SAV.getActiveFile( ).getLocalCaughtCount( ) );
            IO::regularFont->printStringC( buffer, 54 + 128, 2, true, IO::font::CENTER );

            _nationalSelectedIdx = 0;
            _localSelectedPage   = -1;
        } else if( p_newMode == 1 ) { // national dex
            IO::regularFont->printStringC( GET_STRING( 578 ), 94 / 2 + 1, 5, false,
                                           IO::font::CENTER );
            std::snprintf( buffer, 89, GET_STRING( 583 ),
                           SAVE::SAV.getActiveFile( ).getSeenCount( ) );
            IO::regularFont->printStringC( buffer, 74, 2, true, IO::font::CENTER );
            std::snprintf( buffer, 89, GET_STRING( 584 ),
                           SAVE::SAV.getActiveFile( ).getCaughtCount( ) );
            IO::regularFont->printStringC( buffer, 54 + 128, 2, true, IO::font::CENTER );

            _nationalSelectedIdx = 0;
            _localSelectedPage   = -1;
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

    void dexUI::drawPkmnInfo( const pkmnSpriteInfo& p_pkmn, u8 p_page, bool p_bottom ) {
        auto     oamTop = !p_bottom ? IO::OamTop->oamBuffer : IO::Oam->oamBuffer;
        pkmnData data   = getPkmnData( p_pkmn.m_pkmnIdx, p_pkmn.m_forme );

        bool seen   = SAVE::SAV.getActiveFile( ).seen( p_pkmn.m_pkmnIdx );
        bool caught = SAVE::SAV.getActiveFile( ).caught( p_pkmn.m_pkmnIdx );

        char buffer[ 100 ];

        if( p_page == 0 ) {
            // load pkmn sprite
            pkmnSpriteInfo pinfo = p_pkmn;
            if( !seen && !caught ) { pinfo = { 0, 0, false, false, false }; }
            IO::loadPKMNSprite( pinfo, SPR_PKMN_X, SPR_PKMN_Y - 96 + IO::pkmnSpriteHeight( pinfo ),
                                SPR_PKMN_START_OAM, SPR_PKMN_PAL,
                                oamTop[ SPR_PKMN_START_OAM ].gfxIndex, p_bottom, seen && !caught );

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
            if( seen || caught ) {
                if( !p_pkmn.m_forme ) {
                    IO::regularFont->printStringC(
                        getDisplayName( p_pkmn.m_pkmnIdx, p_pkmn.m_forme ).c_str( ), 128, 112,
                        p_bottom, IO::font::CENTER );
                } else {
                    IO::regularFont->printStringC(
                        getDisplayName( p_pkmn.m_pkmnIdx, p_pkmn.m_forme ).c_str( ),
                        SPR_TYPE_X( 0 ) - 4, 112, p_bottom, IO::font::RIGHT );
                }
            } else {
                IO::regularFont->printStringC( GET_STRING( 581 ), 128, 112, p_bottom,
                                               IO::font::CENTER );
            }

            // dex no
            snprintf( buffer, 99, "%03hu%c",
                      _mode ? p_pkmn.m_pkmnIdx : getDexNo( p_pkmn.m_pkmnIdx ),
                      p_pkmn.m_forme ? 'a' + p_pkmn.m_forme - 1 : 0 );
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
                          getSpeciesName( p_pkmn.m_pkmnIdx, p_pkmn.m_forme ).c_str( ),
                          data.m_baseForme.m_size / 10.0, data.m_baseForme.m_weight / 10.0 );
                IO::regularFont->setColor( IO::GRAY_IDX, 1 );
                IO::regularFont->printStringC( buffer, 128, 128, p_bottom, IO::font::CENTER );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->printBreakingStringC(
                    getDexEntry( p_pkmn.m_pkmnIdx, p_pkmn.m_forme ).c_str( ), 128, 141, 214,
                    p_bottom, IO::font::CENTER, 12 );

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

    void dexUI::loadPkmnEntry( u16 p_pkmnIdx, u8 p_pkmnForme, u8 p_OAMstart, bool p_isHidden,
                               u16 p_x, u16 p_y, bool p_bottom ) {
        bool ispkmn = p_pkmnIdx && p_pkmnIdx <= MAX_PKMN;

        bool seen   = ispkmn && SAVE::SAV.getActiveFile( ).seen( p_pkmnIdx );
        bool caught = ispkmn && SAVE::SAV.getActiveFile( ).caught( p_pkmnIdx );

        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;

        // sprite bg
        oam[ p_OAMstart + 2 ]          = ispkmn ? oam[ SPR_DX2_OAM_SUB ] : oam[ SPR_DX1_OAM_SUB ];
        oam[ p_OAMstart + 2 ].isHidden = p_isHidden;
        oam[ p_OAMstart + 2 ].x        = p_x;
        oam[ p_OAMstart + 2 ].y        = p_y;
        oam[ p_OAMstart + 2 ].priority = OBJPRIORITY_3;

        pkmnSpriteInfo pinfo = { p_pkmnIdx, p_pkmnForme, false, false, false, DEFAULT_SPRITE_PID };

        // pkmn icon
        if( caught ) {
            IO::loadPKMNIconB( pinfo, p_x, p_y + 16, p_OAMstart + 1, oam[ p_OAMstart + 1 ].gfxIndex,
                               p_bottom );
            oam[ p_OAMstart + 1 ].priority = OBJPRIORITY_3;
            if( p_isHidden ) {
                oam[ p_OAMstart + 1 ].isRotateScale = false;
                oam[ p_OAMstart + 1 ].isHidden      = true;
            } else {
                oam[ p_OAMstart + 1 ].isHidden      = false;
                oam[ p_OAMstart + 1 ].isRotateScale = true;
                oam[ p_OAMstart + 1 ].rotationIndex = 0;
            }

            //   oam[ p_OAMstart ]          = oam[ SPR_CAUGHT_OAM_SUB ];
            // oam[ p_OAMstart ].isHidden = p_isHidden;
            // if( !p_isHidden ) {
            //     oam[ p_OAMstart ].isRotateScale = true;
            //     oam[ p_OAMstart ].rotationIndex = 1;
            //  }
            //  oam[ p_OAMstart ].x        = p_x - 2;
            //  oam[ p_OAMstart ].y        = p_y + 6;
            //  oam[ p_OAMstart ].priority = OBJPRIORITY_3;
        } else if( seen ) {
            IO::loadPKMNIconB( pinfo, p_x, p_y + 16, p_OAMstart + 1, oam[ p_OAMstart + 1 ].gfxIndex,
                               p_bottom, false, 0, true );
            oam[ p_OAMstart + 1 ].priority = OBJPRIORITY_3;
            if( p_isHidden ) {
                oam[ p_OAMstart + 1 ].isRotateScale = false;
                oam[ p_OAMstart + 1 ].isHidden      = true;
            } else {
                oam[ p_OAMstart + 1 ].isHidden      = false;
                oam[ p_OAMstart + 1 ].isRotateScale = true;
                oam[ p_OAMstart + 1 ].rotationIndex = 0;
            }

            oam[ p_OAMstart ].isRotateScale = false;
            oam[ p_OAMstart ].isHidden      = true;
        } else {
            if( !_mode && !p_isHidden && ispkmn ) {
                // load dex no (only in local dex mode
                char buffer[ 20 ];
                snprintf( buffer, 99, "%03hu%c", _mode ? p_pkmnIdx : getDexNo( p_pkmnIdx ),
                          p_pkmnForme ? 'a' + p_pkmnForme - 1 : 0 );

                std::memset( NAV::TEXT_BUF, 0, sizeof( NAV::TEXT_BUF ) );

                IO::regularFont->setColor( 1, 1 );
                IO::regularFont->setColor( 0, 2 );
                IO::regularFont->printStringBC( buffer, TEXT_PAL, NAV::TEXT_BUF, 32,
                                                IO::font::CENTER, 0, 32 );
                IO::loadSpriteB( p_OAMstart + 1, oam[ p_OAMstart + 1 ].gfxIndex, p_x - 1, p_y + 25,
                                 32, 32, NAV::TEXT_BUF, 32 * 32 / 2, false, false, false,
                                 OBJPRIORITY_3, p_bottom );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            } else if( _mode && !p_isHidden && ispkmn ) {
                pinfo = { 0, 0, false, false, false };
                IO::loadPKMNIconB( pinfo, p_x, p_y + 16, p_OAMstart + 1,
                                   oam[ p_OAMstart + 1 ].gfxIndex, p_bottom );
                oam[ p_OAMstart + 1 ].priority = OBJPRIORITY_3;
                if( p_isHidden ) {
                    oam[ p_OAMstart + 1 ].isRotateScale = false;
                    oam[ p_OAMstart + 1 ].isHidden      = true;
                } else {
                    oam[ p_OAMstart + 1 ].isHidden      = false;
                    oam[ p_OAMstart + 1 ].isRotateScale = true;
                    oam[ p_OAMstart + 1 ].rotationIndex = 0;
                }
            } else {
                oam[ p_OAMstart + 1 ].isRotateScale = false;
                oam[ p_OAMstart + 1 ].isHidden      = true;
            }
        }

        oam[ p_OAMstart ].isRotateScale    = false;
        oam[ p_OAMstart ].isHidden         = true;
        oam[ p_OAMstart + 3 ].isSizeDouble = oam[ p_OAMstart + 4 ].isSizeDouble = false;
        oam[ p_OAMstart + 3 ].isRotateScale = oam[ p_OAMstart + 4 ].isRotateScale = false;
        oam[ p_OAMstart + 3 ].isHidden = oam[ p_OAMstart + 4 ].isHidden = true;
        IO::updateOAM( p_bottom );
    }

    void dexUI::nationalLoadPkmnEntry( u16 p_pkmnIdx, u8 p_OAMslot, bool p_bottom ) {
        u8 currRot  = ( _nationalOAMStart - SPR_NAT_DX_START_OAM_SUB ) / SPR_NAT_DX_SLOT_SIZE;
        u8 oamStart = SPR_NAT_DX_START_OAM_SUB
                      + ( ( currRot + p_OAMslot ) % SPR_NAT_DX_SLOT_COUNT ) * SPR_NAT_DX_SLOT_SIZE;

        bool         seen   = SAVE::SAV.getActiveFile( ).seen( p_pkmnIdx );
        bool         caught = SAVE::SAV.getActiveFile( ).caught( p_pkmnIdx );
        SpriteEntry* oam    = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;

        char buffer[ 10 ];

        // load pkmn icon w/ corresponding bg
        loadPkmnEntry( p_pkmnIdx, 0, oamStart, SPR_NAT_DX_HIDDEN( p_OAMslot ),
                       SPR_NAT_DX_BG_POS_X( p_OAMslot ), SPR_NAT_DX_BG_POS_Y( p_OAMslot ),
                       p_bottom );

        if( !p_pkmnIdx || p_pkmnIdx > MAX_PKMN ) {
            oam[ oamStart + 5 ].isHidden = true;
            oam[ oamStart + 6 ].isHidden = true;
            oam[ oamStart + 7 ].isHidden = true;
            oam[ oamStart + 8 ].isHidden = true;
        } else {
            // load pkmn name
            std::memset( NAV::TEXT_BUF, 0, sizeof( NAV::TEXT_BUF ) );

            IO::regularFont->setColor( 1, 1 );
            IO::regularFont->setColor( 0, 2 );
            snprintf( buffer, 9, "%03hu", p_pkmnIdx );
            IO::regularFont->printStringBC( buffer, TEXT_PAL, NAV::TEXT_BUF, 32, IO::font::LEFT, 0,
                                            32 );
            IO::loadSpriteB( oamStart + 5, oam[ oamStart + 5 ].gfxIndex,
                             SPR_NAT_DX_BG_POS_X( p_OAMslot ) + 40,
                             SPR_NAT_DX_BG_POS_Y( p_OAMslot ) + 32, 32, 16, NAV::TEXT_BUF,
                             16 * 32 / 2, false, false, false, OBJPRIORITY_3, p_bottom );

            std::memset( NAV::TEXT_BUF, 0, sizeof( NAV::TEXT_BUF ) );
            if( seen || caught ) {
                IO::regularFont->printStringBC( getDisplayName( p_pkmnIdx ).c_str( ), TEXT_PAL,
                                                NAV::TEXT_BUF, 96, IO::font::LEFT, 15, 32, 16 );
            } else {
                IO::regularFont->printStringBC( GET_STRING( 581 ), TEXT_PAL, NAV::TEXT_BUF, 96,
                                                IO::font::LEFT, 15, 32, 16 );
            }
            IO::loadSpriteB( oamStart + 6, oam[ oamStart + 6 ].gfxIndex,
                             SPR_NAT_DX_BG_POS_X( p_OAMslot ) + 64,
                             SPR_NAT_DX_BG_POS_Y( p_OAMslot ) + 32, 32, 16, NAV::TEXT_BUF,
                             32 * 16 / 2, false, false, false, OBJPRIORITY_3, p_bottom );
            IO::loadSpriteB( oamStart + 7, oam[ oamStart + 7 ].gfxIndex,
                             SPR_NAT_DX_BG_POS_X( p_OAMslot ) + 64 + 32,
                             SPR_NAT_DX_BG_POS_Y( p_OAMslot ) + 32, 32, 16, NAV::TEXT_BUF + 32 * 16,
                             32 * 16 / 2, false, false, false, OBJPRIORITY_3, p_bottom );
            IO::loadSpriteB( oamStart + 8, oam[ oamStart + 8 ].gfxIndex,
                             SPR_NAT_DX_BG_POS_X( p_OAMslot ) + 64 + 64,
                             SPR_NAT_DX_BG_POS_Y( p_OAMslot ) + 32, 32, 16, NAV::TEXT_BUF + 32 * 32,
                             32 * 16 / 2, false, false, false, OBJPRIORITY_3, p_bottom );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        }
    }

    void dexUI::nationalRotateBackward( u16 p_pkmnIdxUB, bool p_bottom ) {
        if( p_pkmnIdxUB && _nationalSelectedIdx >= p_pkmnIdxUB ) { return; }

        u8 currRot       = ( _nationalOAMStart - SPR_NAT_DX_START_OAM_SUB ) / SPR_NAT_DX_SLOT_SIZE;
        u8 nextRot       = ( currRot + SPR_NAT_DX_SLOT_COUNT - 1 ) % SPR_NAT_DX_SLOT_COUNT;
        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;

        _nationalOAMStart = SPR_NAT_DX_START_OAM_SUB + nextRot * SPR_NAT_DX_SLOT_SIZE;

        _nationalSelectedIdx--;
        u16 pidx = ( _nationalSelectedIdx >= SPR_NAT_DX_SLOT_COUNT / 3 )
                       ? ( _nationalSelectedIdx - SPR_NAT_DX_SLOT_COUNT / 3 - 1 )
                       : 0;

        if( p_pkmnIdxUB && pidx > p_pkmnIdxUB ) { pidx = 0; }
        nationalLoadPkmnEntry( pidx, 0, p_bottom );

        for( u8 i = 1; i < SPR_NAT_DX_SLOT_COUNT; ++i ) {
            u8 oamStart = SPR_NAT_DX_START_OAM_SUB
                          + ( ( nextRot + i ) % SPR_NAT_DX_SLOT_COUNT ) * SPR_NAT_DX_SLOT_SIZE;
            moveSpriteBlock( oamStart, SPR_NAT_DX_SLOT_SIZE, 0, PKMN_BOX_HG, p_bottom );
        }

        for( u8 i = 0; i < SPR_NAT_DX_SLOT_COUNT; ++i ) {
            u8 oamStart = SPR_NAT_DX_START_OAM_SUB
                          + ( ( nextRot + i ) % SPR_NAT_DX_SLOT_COUNT ) * SPR_NAT_DX_SLOT_SIZE;
            pidx = ( _nationalSelectedIdx + i > SPR_NAT_DX_SLOT_COUNT / 3 - 1 )
                       ? _nationalSelectedIdx + i - SPR_NAT_DX_SLOT_COUNT / 3 - 1
                       : 0;
            if( p_pkmnIdxUB && pidx > p_pkmnIdxUB ) { pidx = 0; }

            bool isHidden                = SPR_NAT_DX_HIDDEN( i );
            oam[ oamStart + 2 ].isHidden = isHidden;
            isHidden |= !pidx;
            oam[ oamStart + 1 ].isHidden       = isHidden;
            oam[ oamStart + 1 ].isRotateScale  = !isHidden;
            oam[ oamStart + 5 ].isHidden       = oam[ oamStart + 6 ].isHidden
                = oam[ oamStart + 7 ].isHidden = oam[ oamStart + 8 ].isHidden = isHidden;
        }

        IO::updateOAM( p_bottom );
    }

    void dexUI::nationalRotateForward( u16 p_pkmnIdxUB, bool p_bottom ) {
        if( !_nationalSelectedIdx ) { return; }

        u8 currRot       = ( _nationalOAMStart - SPR_NAT_DX_START_OAM_SUB ) / SPR_NAT_DX_SLOT_SIZE;
        u8 nextRot       = ( currRot + 1 ) % SPR_NAT_DX_SLOT_COUNT;
        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;

        _nationalOAMStart = SPR_NAT_DX_START_OAM_SUB + nextRot * SPR_NAT_DX_SLOT_SIZE;

        u16 pidx = SPR_NAT_DX_SLOT_COUNT + _nationalSelectedIdx - SPR_NAT_DX_SLOT_COUNT / 3 - 1;
        _nationalSelectedIdx++;

        if( p_pkmnIdxUB && pidx > p_pkmnIdxUB ) { pidx = 0; }
        nationalLoadPkmnEntry( pidx, SPR_NAT_DX_SLOT_COUNT - 1, p_bottom );

        for( u8 i = 0; i < SPR_NAT_DX_SLOT_COUNT - 1; ++i ) {
            u8 oamStart = SPR_NAT_DX_START_OAM_SUB
                          + ( ( nextRot + i ) % SPR_NAT_DX_SLOT_COUNT ) * SPR_NAT_DX_SLOT_SIZE;
            moveSpriteBlock( oamStart, SPR_NAT_DX_SLOT_SIZE, 0, -PKMN_BOX_HG, p_bottom );
        }
        for( u8 i = 0; i < SPR_NAT_DX_SLOT_COUNT; ++i ) {
            u8 oamStart = SPR_NAT_DX_START_OAM_SUB
                          + ( ( nextRot + i ) % SPR_NAT_DX_SLOT_COUNT ) * SPR_NAT_DX_SLOT_SIZE;
            pidx = ( _nationalSelectedIdx + i > SPR_NAT_DX_SLOT_COUNT / 3 - 1 )
                       ? _nationalSelectedIdx + i - SPR_NAT_DX_SLOT_COUNT / 3 - 1
                       : 0;
            if( p_pkmnIdxUB && pidx > p_pkmnIdxUB ) { pidx = 0; }

            bool isHidden                = SPR_NAT_DX_HIDDEN( i );
            oam[ oamStart + 2 ].isHidden = isHidden;
            isHidden |= !pidx;
            oam[ oamStart + 1 ].isHidden       = isHidden;
            oam[ oamStart + 1 ].isRotateScale  = !isHidden;
            oam[ oamStart + 5 ].isHidden       = oam[ oamStart + 6 ].isHidden
                = oam[ oamStart + 7 ].isHidden = oam[ oamStart + 8 ].isHidden = isHidden;
        }

        IO::updateOAM( p_bottom );
    }

    void dexUI::nationalInitSub( u16 p_centerPkmnIdx, u16 p_pkmnIdxUB, bool p_bottom ) {
        // draw bottom screen, vertical list in national dex mode

        // clear anything on the screen
        IO::printRectangle( 30, 30, 220, 160, p_bottom, 0 );

        _nationalSelectedIdx = p_centerPkmnIdx;
        _nationalOAMStart    = SPR_NAT_DX_START_OAM_SUB;

        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;

        oam[ SPR_DX3_OAM_SUB ].isHidden = false;
        oam[ SPR_DX3_OAM_SUB ].x        = SPR_NAT_DX_BG_POS_X( 3 );
        oam[ SPR_DX3_OAM_SUB ].y        = SPR_NAT_DX_BG_POS_Y( 3 );

        for( u8 i = 0; i < SPR_NAT_DX_SLOT_COUNT; ++i ) {
            u8 oamStart                  = _nationalOAMStart + i * SPR_NAT_DX_SLOT_SIZE;
            oam[ oamStart + 1 ].gfxIndex = SPR_NAT_DX_START_GFX_SUB + i * SPR_NAT_DX_SLOT_GFX_SIZE;
            oam[ oamStart + 5 ].gfxIndex
                = SPR_NAT_DX_START_GFX_SUB + i * SPR_NAT_DX_SLOT_GFX_SIZE + 16;
            oam[ oamStart + 6 ].gfxIndex
                = SPR_NAT_DX_START_GFX_SUB + i * SPR_NAT_DX_SLOT_GFX_SIZE + 16 + 16;
            oam[ oamStart + 7 ].gfxIndex
                = SPR_NAT_DX_START_GFX_SUB + i * SPR_NAT_DX_SLOT_GFX_SIZE + 16 + 24;
            oam[ oamStart + 8 ].gfxIndex
                = SPR_NAT_DX_START_GFX_SUB + i * SPR_NAT_DX_SLOT_GFX_SIZE + 16 + 32;
        }

        for( u8 i = 0; i < SPR_NAT_DX_SLOT_COUNT; ++i ) {
            u16 pidx = ( p_centerPkmnIdx + i >= SPR_NAT_DX_SLOT_COUNT / 3 )
                           ? ( p_centerPkmnIdx - SPR_NAT_DX_SLOT_COUNT / 3 + i - 1 )
                           : 0;
            if( p_pkmnIdxUB && pidx > p_pkmnIdxUB ) { pidx = 0; }
            nationalLoadPkmnEntry( pidx, i, p_bottom );
        }
        IO::updateOAM( p_bottom );
    }

    void dexUI::nationalSelectIndex( u16 p_pkmnIdx, u16 p_pkmnIdxUB, bool p_bottom, u8 p_forme,
                                     bool p_shiny, bool p_female ) {
        pkmnSpriteInfo pinfo = { p_pkmnIdx, p_forme, p_female, p_shiny, false, DEFAULT_SPRITE_PID };
        drawPkmnInfo( pinfo, 0, !p_bottom );
        if( !_nationalSelectedIdx || _nationalSelectedIdx == p_pkmnIdx ) {
            nationalInitSub( p_pkmnIdx, p_pkmnIdxUB, p_bottom );
        } else if( _nationalSelectedIdx > p_pkmnIdx ) {
            while( _nationalSelectedIdx > p_pkmnIdx ) {
                nationalRotateBackward( p_pkmnIdxUB, p_bottom );
            }
        } else if( _nationalSelectedIdx < p_pkmnIdx ) {
            while( _nationalSelectedIdx < p_pkmnIdx ) {
                nationalRotateForward( p_pkmnIdxUB, p_bottom );
            }
        }

        // update touch positions
        _touchPositions = std::vector<std::pair<IO::touchInputTarget, u16>>( );
        _touchPositions.push_back( _backChoice );
        for( u8 i = 0; i < SPR_NAT_DX_SLOT_COUNT; ++i ) {
            u16 pidx = ( _nationalSelectedIdx + i >= SPR_NAT_DX_SLOT_COUNT / 3 )
                           ? ( _nationalSelectedIdx - SPR_NAT_DX_SLOT_COUNT / 3 + i - 1 )
                           : 0;
            if( p_pkmnIdxUB && pidx > p_pkmnIdxUB ) { pidx = 0; }
            if( pidx && !SPR_NAT_DX_HIDDEN( i ) ) {
                _touchPositions.push_back( { IO::touchInputTarget( IO::touchVerticalRhombus(
                                                 SPR_NAT_DX_BG_POS_X( i ), SPR_NAT_DX_BG_POS_Y( i ),
                                                 PKMN_BOX_WD_I, PKMN_BOX_HG_I ) ),
                                             pidx } );
            }
        }
    }

    void dexUI::localDrawPage( u16 p_page, u16 p_pageUB, bool p_inverted, bool p_bottom ) {
        _touchPositions = std::vector<std::pair<IO::touchInputTarget, u16>>( );
        _touchPositions.push_back( _backChoice );

        for( s8 x = 0; x < SPR_LOC_DX_SLOT_COL_COUNT; ++x ) {
            for( u8 y = 0; y < SPR_LOC_DX_SLOT_ROW_COUNT; ++y ) {
                s8  ix = p_inverted ? x : SPR_LOC_DX_SLOT_COL_COUNT - x - 1;
                u16 pg = p_page + ix;

                u16 pkmnIdx   = LOCAL_DEX_PAGES[ pg ][ y ];
                u16 pkmnForme = 0;

                if( p_pageUB && pg > p_pageUB ) { pkmnIdx = pkmnForme = 0; }

                u16 linearizedIdx = ix * SPR_LOC_DX_SLOT_ROW_COUNT + y;

                u8 oamStart = SPR_LOC_DX_START_OAM_SUB + linearizedIdx * SPR_LOC_DX_SLOT_SIZE;

                loadPkmnEntry( pkmnIdx, pkmnForme, oamStart, false, SPR_LOC_DX_BG_POS_X( ix, y ),
                               SPR_LOC_DX_BG_POS_Y( ix, y ), p_bottom );

                if( pkmnIdx ) {
                    _touchPositions.push_back(
                        { IO::touchInputTarget( IO::touchVerticalRhombus(
                              SPR_LOC_DX_BG_POS_X( ix, y ), SPR_LOC_DX_BG_POS_Y( ix, y ),
                              PKMN_BOX_WD_I, PKMN_BOX_HG_I ) ),
                          pkmnIdx } );
                }
            }
        }
    }

    void dexUI::localInitSub( bool p_bottom ) {
        // draw bottom screen, 3 rows, horizontal scrolling list for local mode

        // clear anything on the screen
        IO::printRectangle( 30, 30, 220, 160, p_bottom, 0 );

        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;

        for( u8 i = 0; i < SPR_LOC_DX_SLOT_ROW_COUNT * SPR_LOC_DX_SLOT_COL_COUNT; ++i ) {
            u8 oamStart                  = SPR_LOC_DX_START_OAM_SUB + i * SPR_LOC_DX_SLOT_SIZE;
            oam[ oamStart + 1 ].gfxIndex = SPR_LOC_DX_START_GFX_SUB + i * SPR_LOC_DX_SLOT_GFX_SIZE;
        }
        _localSelectedPage = -1;
    }

    void dexUI::localSelectPageSlot( u16 p_page, u8 p_slot, u16 p_pageUB, bool p_bottom, u8 p_forme,
                                     bool p_shiny, bool p_female ) {
        pkmnSpriteInfo pinfo = { LOCAL_DEX_PAGES[ p_page ][ p_slot ],
                                 p_forme,
                                 p_female,
                                 p_shiny,
                                 false,
                                 DEFAULT_SPRITE_PID };
        drawPkmnInfo( pinfo, 0, !p_bottom );
        SpriteEntry* oam   = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;
        u16          dpage = ( p_page - 1 ) / 4 * 4;

        oam[ SPR_DX3_OAM_SUB ].isHidden = false;
        oam[ SPR_DX3_OAM_SUB ].x        = SPR_LOC_DX_BG_POS_X( p_page - dpage, p_slot );
        oam[ SPR_DX3_OAM_SUB ].y        = SPR_LOC_DX_BG_POS_Y( p_page - dpage, p_slot );

        IO::updateOAM( p_bottom );

        bool inv = _localSelectedPage < dpage;
        if( _localSelectedPage == u16( -1 ) ) { inv = true; }

        if( _localSelectedPage != dpage ) {
            localDrawPage( dpage, p_pageUB, inv, p_bottom );
            _localSelectedPage = dpage;
        }
    }

    std::vector<std::pair<IO::touchInputTarget, u16>> dexUI::getTouchPositions( u8 p_mode ) {
        if( p_mode == _mode ) { return _touchPositions; }

        // disable every choice except "back/exit"
        return { _backChoice };
    }
} // namespace DEX
