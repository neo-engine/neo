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
#include "dextop3.h"

#define SPR_X_OAM_SUB 0

#define SPR_X_PAL_SUB       0
#define SPR_BOX_PAL_SUB     7
#define SPR_BOX_SEL_PAL_SUB 8

namespace DEX {
    void dexUI::init( ) {
    }

    std::vector<std::pair<IO::inputTarget, u8>> dexUI::drawModeChoice( bool p_showLocalDex,
                                                                       bool p_showNationalDex ) {
        char buffer[ 10 ];

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

        p_showNationalDex = true;

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
        return res;
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
    }

    void dexUI::selectNationalIndex( u16 p_pkmnIdx, bool p_bottom ) {
    }

    void dexUI::selectLocalPageSlot( u16 p_page, u8 p_slot, bool p_bottom ) {
    }

    void dexUI::highlightButton( u8 p_button, bool p_bottom ) {
    }

    std::vector<std::pair<IO::inputTarget, u8>> dexUI::getTouchPositions( bool p_bottom ) {
        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        return res;
    }
} // namespace DEX
