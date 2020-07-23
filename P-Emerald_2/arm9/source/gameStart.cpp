/*
Pokémon neo
------------------------------

file        : gameStart.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
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

#include "gameStart.h"
#include "bag.h"
#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "gen3Parser.h"
#include "itemNames.h"
#include "keyboard.h"
#include "nav.h"
#include "pokemonNames.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sound.h"
#include "startScreen.h"
#include "uio.h"
#include "yesNoBox.h"

#include "Border.h"
#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "noselection_96_32_4.h"

namespace SAVE {
    constexpr u16 EP_INTRO_TEXT_START[] = { 111 };
    constexpr u8  EP_INTRO_TEXT_LEN[]   = { 3 };

    void printTextAndWait( const char* p_text ) {
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        IO::clearScreen( true, true, true );
        BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE[ IO::COLOR_IDX ] = IO::GRAY;
        u8 nw                       = 0;
        for( u8 i = 0; i < strlen( p_text ); ++i )
            if( p_text[ i ] == '\n' ) ++nw;
        IO::regularFont->printString( p_text, 128, 89 - 10 * nw, true, IO::font::CENTER );
        swiWaitForVBlank( );
        IO::fadeScreen( IO::fadeType::UNFADE, true, true );

        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START )
                || ( IO::inputTarget( 1, 1, 256, 192 ).inRange( touch )
                     && IO::waitForInput( IO::inputTarget( 1, 1, 256, 192 ) ) ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                break;
            }
        }
    }

    void printEpisodeInfo( s8 p_episode ) {
        IO::regularFont->setColor( IO::COLOR_IDX, 2 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );

        for( u8 i = 0; i < EP_INTRO_TEXT_LEN[ p_episode ]; ++i )
            printTextAndWait( GET_STRING( EP_INTRO_TEXT_START[ p_episode ] + i ) );
    }

    void printMBoxTextAndWait( const char* p_text ) {
        IO::regularFont->printBreakingStringC( p_text, 23, 192 - 54, 178, true, IO::font::LEFT, 16,
                                               ' ', 0, true );
        u8 frame = 0;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysCurrent( );

            if( ++frame % 64 == 31 ) {
                IO::regularFont->printChar( 172, 196, 192 - 28, true );
            } else if( frame % 64 == 63 ) {
                IO::printRectangle( 196, 192 - 28, 212, 192 - 20, true, 0 );
            }

            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START )
                || ( IO::inputTarget( 1, 1, 256, 192 ).inRange( touch )
                     && IO::waitForInput( IO::inputTarget( 1, 1, 256, 192 ) ) ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                break;
            }
        }

        IO::printRectangle( 22, 192 - 60, 212, 192 - 60 + 40, true, 0 );
    }

    bool initNewGame( ) {
        SAV.getActiveFile( ).initialize( );
        SAV.getActiveFile( ).m_gameType = NORMAL;
        IO::initOAMTable( true );

        // Initial text
        IO::clearScreen( true, true, true );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;

        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }
        SOUND::playSoundEffect( SFX_NAV );
        for( u8 i = 0; i < 40; ++i ) { swiWaitForVBlank( ); }
        SOUND::playSoundEffect( SFX_NAV );
        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

        printTextAndWait( "... ..." );
        printTextAndWait( GET_STRING( 441 ) );

        IO::clearScreen( true, true, true );
        SOUND::playBGM( MOD_ROUTE_123 );
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "intro", 240 * 2, 256 * 192,
                             true );
        dmaCopy( BorderBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaCopy( BorderPal + 192, BG_PALETTE_SUB + 192, 64 );
        u16 tileCnt = 0;
        tileCnt     = IO::loadTrainerSprite( 240, 80, 32, 0, 0, tileCnt, true );
        tileCnt     = IO::loadPlatform( 10, 48, 16 + 48, 1, 1, tileCnt, true );

        // mbox

        IO::loadSprite( "UI/mbox1", 10, 2, tileCnt, 14, 192 - 60, 32, 64, false, false, false,
                        OBJPRIORITY_3, true );

        for( u8 i = 0; i < 10; ++i ) {
            IO::loadSprite( 10 + 11 - i, 2, tileCnt, 35 + 16 * i, 192 - 60, 32, 64, 0, 0, 0, false,
                            true, false, OBJPRIORITY_3, true );
        }
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        IO::updateOAM( true );

        IO::fadeScreen( IO::fadeType::UNFADE, true, true );

        for( u8 i = 0; i < 4; ++i ) { printMBoxTextAndWait( GET_STRING( 442 + i ) ); }

        tileCnt = IO::loadSprite( "PB/3/3_0", 22, 3, tileCnt + 32, 76, 46, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );

        IO::Oam->matrixBuffer[ 0 ].hdx         = 1 << 9;
        IO::Oam->matrixBuffer[ 0 ].vdx         = 0;
        IO::Oam->matrixBuffer[ 0 ].hdy         = 0;
        IO::Oam->matrixBuffer[ 0 ].vdy         = 1 << 9;
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;

        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        IO::Oam->oamBuffer[ 22 ].x -= 2;
        IO::Oam->oamBuffer[ 22 ].y -= 2;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_1", 22, 3, tileCnt + 32, 72, 44, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_2", 22, 3, tileCnt + 32, 72, 45, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_3", 22, 3, tileCnt + 32, 72, 46, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_4", 22, 3, tileCnt + 32, 72, 47, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_5", 22, 3, tileCnt + 32, 72, 48, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_6", 22, 3, tileCnt + 32, 72, 49, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_7", 22, 3, tileCnt + 32, 72, 50, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        SOUND::playSoundEffect( SFX_BATTLE_RECALL );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_8", 22, 3, tileCnt + 32, 72, 52, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_9", 22, 3, tileCnt + 32, 72, 54, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        tileCnt = IO::loadSprite( "PB/3/3_10", 22, 3, tileCnt + 32, 72, 56, 32, 32, false, false,
                                  false, OBJPRIORITY_0, true );
        IO::Oam->oamBuffer[ 22 ].rotationIndex = 0;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = true;
        IO::Oam->oamBuffer[ 0 ].x += 1;
        IO::updateOAM( true );
        for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }

        IO::Oam->oamBuffer[ 22 ].isHidden      = true;
        IO::Oam->oamBuffer[ 22 ].isRotateScale = false;

        IO::loadPKMNSprite( PKMN_LOTAD, 40, 36, 23, 3, tileCnt, true );
        for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        SOUND::playCry( PKMN_LOTAD );
        for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 11; ++i ) { printMBoxTextAndWait( GET_STRING( 446 + i ) ); }

        do {
            std::strncpy( SAV.getActiveFile( ).m_playername, "", 12 );
            FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "tbg_t" );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "tbg_s", 249 * 2,
                                 256 * 192, true );
            SAV.getActiveFile( ).drawTrainersCard( false, true );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            IO::initOAMTable( true );

            tileCnt = 0;
            IO::loadSprite( "UI/mbox2", 0, 2, tileCnt, 2, 2, 32, 64, false, false, false,
                            OBJPRIORITY_3, true );

            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSprite( 13 - i, 2, tileCnt, 30 + 16 * i, 2, 32, 64, 0, 0, 0, false, true,
                                false, OBJPRIORITY_3, true );
            }

            tileCnt = IO::loadTrainerSprite( 0, 32, 88, 14, 0, tileCnt + 32, true );
            tileCnt = IO::loadTrainerSprite( 1, 128 + 32, 88, 15, 1, tileCnt, true );

            IO::loadSprite( "UI/sel_64_64", 50, 4, tileCnt, 32, 88, 64, 64, false, false, false,
                            OBJPRIORITY_3, true );

            IO::updateOAM( true );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );

            IO::regularFont->printBreakingStringC( GET_STRING( 457 ), 8, 8, 240, true,
                                                   IO::font::LEFT, 16 );

            IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_LEFT_RIGHT );

            SAV.getActiveFile( ).m_appearance = cb.getResult(
                [ & ]( u8 ) {
                    std::vector<std::pair<IO::inputTarget, u8>> res
                        = std::vector<std::pair<IO::inputTarget, u8>>( );
                    res.push_back( std::pair( IO::inputTarget( IO::Oam->oamBuffer[ 14 ].x,
                                                               IO::Oam->oamBuffer[ 14 ].y,
                                                               IO::Oam->oamBuffer[ 14 ].x + 64,
                                                               IO::Oam->oamBuffer[ 14 ].y + 64 ),
                                              0 ) );
                    res.push_back( std::pair( IO::inputTarget( IO::Oam->oamBuffer[ 15 ].x,
                                                               IO::Oam->oamBuffer[ 15 ].y,
                                                               IO::Oam->oamBuffer[ 15 ].x + 64,
                                                               IO::Oam->oamBuffer[ 15 ].y + 64 ),
                                              1 ) );
                    return res;
                },
                [ & ]( u8 p_selection ) {
                    IO::Oam->oamBuffer[ 50 ].x = IO::Oam->oamBuffer[ 14 + p_selection ].x;
                    IO::Oam->oamBuffer[ 50 ].y = IO::Oam->oamBuffer[ 14 + p_selection ].y;
                    IO::updateOAM( true );
                } );

            FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "tbg_s" );
            SAV.getActiveFile( ).drawTrainersCard( false, true );
            IO::loadTrainerSprite( SAV.getActiveFile( ).m_appearance, 33, 45, 0, 0, 0, false );
            IO::updateOAM( false );

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 50 );
            IO::regularFont->printBreakingStringC( GET_STRING( 458 ), 8, 4, 40, true,
                                                   IO::font::LEFT, 13 );

            auto name = IO::keyboard( ).getText( 10 );
            if( name == "" ) { name = GET_STRING( 460 + SAV.getActiveFile( ).m_appearance ); }
            std::strncpy( SAV.getActiveFile( ).m_playername, name.c_str( ), 11 );

            FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "tbg_t" );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "tbg_s", 249 * 2,
                                 256 * 192, true );
            SAV.getActiveFile( ).drawTrainersCard( false );

            IO::initOAMTable( true );

            tileCnt = 0;
            IO::loadSprite( "UI/mbox2", 60, 2, tileCnt, 2, 2, 32, 64, false, false, false,
                            OBJPRIORITY_3, true );

            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSprite( 60 + 13 - i, 2, tileCnt, 30 + 16 * i, 2, 32, 64, 0, 0, 0, false,
                                true, false, OBJPRIORITY_3, true );
            }

            tileCnt = 32;
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 7 + 8 * ( p_pos ) )
#define SPR_BOX_PAL_SUB                   7
#define SPR_BOX_SEL_PAL_SUB               8
            for( u8 i = 0; i < 3; i++ ) {
                u8 pos = 2 * i;

                if( !i ) {
                    tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                              tileCnt, 29, 42 + i * 36, 16, 32,
                                              noselection_96_32_1Pal, noselection_96_32_1Tiles,
                                              noselection_96_32_1TilesLen, false, false, true,
                                              OBJPRIORITY_3, true, OBJMODE_BLENDED );
                    tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                              tileCnt, 29 + 16, 42 + i * 36, 16, 32,
                                              noselection_96_32_2Pal, noselection_96_32_2Tiles,
                                              noselection_96_32_2TilesLen, false, false, true,
                                              OBJPRIORITY_3, true, OBJMODE_BLENDED );
                } else {
                    IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                    IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                                    29, 42 + i * 36, 16, 32, noselection_96_32_1Pal,
                                    noselection_96_32_1Tiles, noselection_96_32_1TilesLen, false,
                                    false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                    IO::loadSprite(
                        SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16,
                        42 + i * 36, 16, 32, noselection_96_32_2Pal, noselection_96_32_2Tiles,
                        noselection_96_32_2TilesLen, false, false, true, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
                }
                for( u8 j = 2; j < 7; j++ ) {
                    IO::loadSprite(
                        SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                        29 + j * 11, 42 + i * 36, 16, 32, noselection_96_32_2Pal,
                        noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                                29 + 5 * 16, 42 + i * 36, 16, 32, noselection_96_32_1Pal,
                                noselection_96_32_1Tiles, noselection_96_32_1TilesLen, true, true,
                                true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }

            for( u8 i = 0; i < 3; i++ ) {
                u8 pos = 2 * i + 1;
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131,
                                42 + i * 36, 16, 32, noselection_96_32_1Pal,
                                noselection_96_32_1Tiles, noselection_96_32_1TilesLen, false, false,
                                true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                for( u8 j = 1; j < 7; j++ ) {
                    IO::loadSprite(
                        SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                        131 + j * 11, 42 + i * 36, 16, 32, noselection_96_32_2Pal,
                        noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                                131 + 5 * 16, 42 + i * 36, 16, 32, noselection_96_32_1Pal,
                                noselection_96_32_1Tiles, noselection_96_32_1TilesLen, true, true,
                                true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }

            IO::copySpritePal( noselection_96_32_4Pal, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );

#undef SPR_BOX_PAL_SUB
#undef SPR_BOX_SEL_PAL_SUB
#undef SPR_CHOICE_START_OAM_SUB
        } while( IO::yesNoBox( ).getResult(
                     [ & ]( ) {
                         auto res = NAV::printYNMessage( 0, MSG_NORMAL, 253 );

                         IO::regularFont->printBreakingStringC( GET_STRING( 459 ), 8, 8, 240, true,
                                                                IO::font::LEFT, 16 );
                         return res;
                     },
                     [ & ]( IO::yesNoBox::selection p_selection ) {
                         NAV::printYNMessage( 0, MSG_NORMAL, p_selection == IO::yesNoBox::NO );
                     } )
                 == IO::yesNoBox::NO );

        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::clearScreen( true, true, true );
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "intro", 240 * 2, 256 * 192,
                             true );
        dmaCopy( BorderBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaCopy( BorderPal + 192, BG_PALETTE_SUB + 192, 64 );
        tileCnt = 0;
        tileCnt = IO::loadTrainerSprite( 240, 80, 32, 0, 0, tileCnt, true );
        tileCnt = IO::loadPlatform( 10, 48, 16 + 48, 1, 1, tileCnt, true );

        // mbox

        IO::loadSprite( "UI/mbox1", 10, 2, tileCnt, 14, 192 - 60, 32, 64, false, false, false,
                        OBJPRIORITY_3, true );

        for( u8 i = 0; i < 10; ++i ) {
            IO::loadSprite( 10 + 11 - i, 2, tileCnt, 35 + 16 * i, 192 - 60, 32, 64, 0, 0, 0, false,
                            true, false, OBJPRIORITY_3, true );
        }
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        IO::updateOAM( true );

        IO::fadeScreen( IO::fadeType::UNFADE, true, true );

        printMBoxTextAndWait( ( GET_STRING( 462 ) + std::string( SAV.getActiveFile( ).m_playername )
                                + GET_STRING( 463 ) )
                                  .c_str( ) );

        for( u8 i = 0; i < 4; ++i ) { printMBoxTextAndWait( GET_STRING( 464 + i ) ); }

        // TODO: Proper location
        SAV.getActiveFile( ).m_currentMap = 10;
        SAV.getActiveFile( ).m_player     = { MAP::mapObject::PLYR,
                                          { 0x93, 0x4a, 3 },
                                          u16( 10 * SAV.getActiveFile( ).m_appearance ),
                                          MAP::moveMode::WALK,
                                          0,
                                          0,
                                          MAP::direction::RIGHT };
        IO::clearScreen( true, true, true );
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        for( u8 i = 10; i; --i ) {
            SOUND::setVolume( 0x10 * i );
            swiWaitForVBlank( );
        }
        return true;
    }

    bool initSpecialEpisode( s8 p_episode ) {
        if( p_episode < 0 ) { // New game
            return initNewGame( );
        }

        SOUND::playBGM( MOD_ROUTE_123 );
        printEpisodeInfo( p_episode );

        switch( p_episode ) {
        case 0:
            std::strncpy( SAV.getActiveFile( ).m_playername, "Test", 11 );
            SAV.getActiveFile( ).m_appearance = rand( ) % 2;
            SAV.getActiveFile( ).m_player     = { MAP::mapObject::PLYR,
                                              { 299, 53, 4 },
                                              u16( 10 * SAV.getActiveFile( ).m_appearance ),
                                              MAP::moveMode::WALK,
                                              0,
                                              0,
                                              MAP::direction::RIGHT };
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_BIKE, 1 );
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_MACH_BIKE, 1 );
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_ACRO_BIKE, 1 );
            SAV.getActiveFile( ).m_currentMap     = 10;
            SAV.getActiveFile( ).m_registeredItem = I_BIKE;
            for( u8 i = 10; i; --i ) {
                SOUND::setVolume( 0x10 * i );
                swiWaitForVBlank( );
            }
            return true;
        default: SAV.getActiveFile( ).m_gameType = UNUSED; return false;
        }
    }

    bool startScreen::transferGame( ) {
        /*
        IO::clearScreen( true, false, false );
        char acSlot2Game[ 5 ] = {0};

        sysSetBusOwners( true, true );
        memcpy( acSlot2Game, (char*) 0x080000AC, 4 );

        char cmpgm[ 5 ][ 4 ] = {"BPE", "AXP", "AXV", "BPR", "BPG"};
        s8   acgame          = -1;

        for( u8 i = 0; i < 5; ++i )
            if( !strncmp( cmpgm[ i ], acSlot2Game, 3 ) ) {
                acgame = i;
                break;
            }
        if( acgame == -1 ) {
            char buffer[ 50 ];
            snprintf( buffer, 49, "%s\n(%s)", GET_STRING( 119 ), acSlot2Game );
            IO::messageBox( buffer, true );
            IO::clearScreen( true, false, false );
            return false;
        }

        IO::yesNoBox yn = IO::yesNoBox( );
        if( !yn.getResult( GET_STRING( 114 ) ) ) {
            IO::clearScreen( true, false, false );
            return false;
        }
        IO::clearScreen( true, false, false );
        IO::messageBox( GET_STRING( 115 ), true );
        IO::messageBox( GET_STRING( 116 ), true );
        IO::messageBox( GET_STRING( 117 ), true );
        IO::messageBox( GET_STRING( 118 ), true );
        yn = IO::yesNoBox( );
        if( !yn.getResult( GET_STRING( 120 ) ) ) {
            IO::clearScreen( true, false, false );
            return false;
        }
        IO::clearScreen( true, false, false );
        IO::messageBox( GET_STRING( 121 ), true );
        int loadgame = acgame > 2 ? 1 : 0;

        FS::gen3Parser* save3 = FS::gen3Parser::Instance( );

        if( save3->load( loadgame ) == -1 ) {
            IO::messageBox( GET_STRING( 122 ), true );
            IO::clearScreen( true, false, false );
            return false;
        }
        IO::clearScreen( true, false, false );

        SAV.m_activeFile                = p_slot;
        SAV.getActiveFile( ).m_gameType = TRANSFER;

        char savname[ 8 ] = {0};
        for( int i = 0; i < 7; ++i ) savname[ i ] = FS::getNText( save3->unpackeddata[ i ] );
        strcpy( SAV.getActiveFile( ).m_playername, savname );

        SAV.getActiveFile( ).m_isMale = !save3->unpackeddata[ 8 ];

        SAV.getActiveFile( ).m_id = ( save3->unpackeddata[ 11 ] << 8 ) | save3->unpackeddata[ 10 ];
        SAV.getActiveFile( ).m_sid
            = ( save3->unpackeddata[ 13 ] << 8 ) | save3->unpackeddata[ 12 ];

        SAV.getActiveFile( ).m_pt.m_hours
            = ( save3->unpackeddata[ 15 ] << 8 ) | save3->unpackeddata[ 14 ];
        SAV.getActiveFile( ).m_pt.m_mins = save3->unpackeddata[ 16 ];
        SAV.getActiveFile( ).m_pt.m_secs = save3->unpackeddata[ 17 ];

        // SAV.getActiveFile( ).m_gba.m_gameid = ( save3->unpackeddata[ 0xaf ] << 24 ) | (
        // save3->unpackeddata[ 0xae ] << 16 ) | ( save3->unpackeddata[ 0xad ] << 8 ) |
        // save3->unpackeddata[ 0xac ];

        pkmnData p;
        for( u8 i = 0; i < 6; ++i ) {
            if( save3->pokemon[ i ]->personality ) {

                pokemon&             acPkmn  = SAV.getActiveFile( ).m_pkmnTeam[ i ];
                FS::belt_pokemon_t*& acBeltP = save3->pokemon[ i ];

                acPkmn.m_boxdata.m_pid   = acBeltP->personality;
                acPkmn.m_boxdata.m_oTSid = acBeltP->otid >> 16;
                acPkmn.m_boxdata.m_oTId  = acBeltP->otid % ( 1 << 16 );
                for( u8 j = 0; j < 10; ++j )
                    acPkmn.m_boxdata.m_name[ j ] = FS::getNText( acBeltP->name[ j ] );
                acPkmn.m_boxdata.m_name[ 10 ] = 0;
                acPkmn.m_boxdata.m_hometown   = acBeltP->language;
                for( u8 j = 0; j < 7; ++j )
                    acPkmn.m_boxdata.m_oT[ j ] = FS::getNText( acBeltP->otname[ j ] );
                acPkmn.m_boxdata.m_oT[ 7 ]  = 0;

                acPkmn.m_statusint         = acBeltP->status;
                acPkmn.m_level             = acBeltP->level;
                acPkmn.m_boxdata.m_pokerus = acBeltP->pokerus;

                acPkmn.m_stats.m_curHP  = acBeltP->currentHP;
                acPkmn.m_stats.m_maxHP = acBeltP->maxHP;
                acPkmn.m_stats.m_Atk   = acBeltP->move;
                acPkmn.m_stats.m_Def   = acBeltP->defense;
                acPkmn.m_stats.m_SAtk  = acBeltP->spatk;
                acPkmn.m_stats.m_SDef  = acBeltP->spdef;
                acPkmn.m_stats.m_Spd   = acBeltP->speed;

                FS::gen3Pokemon::pokemon_growth_t*& acBG = save3->pokemon_growth[ i ];
                acPkmn.m_boxdata.m_speciesId             = FS::getNPKMNIdx( acBG->species );
                acPkmn.m_boxdata.m_heldItem              = FS::getNItemIdx( acBG->held );
                acPkmn.m_boxdata.m_experienceGained      = acBG->xp;
                acPkmn.m_boxdata.m_steps                 = acBG->happiness;
                acPkmn.m_boxdata.m_pPUps                 = acBG->ppbonuses;

                FS::gen3Pokemon::pokemon_moves_t*& acBA = save3->pokemon_moves[ i ];
                for( u8 j = 0; j < 4; ++j ) {
                    acPkmn.m_boxdata.m_moves[ j ] = acBA->atk[ j ];
                    acPkmn.m_boxdata.m_curPP[ j ] = acBA->pp[ j ];
                }

                FS::gen3Pokemon::pokemon_effort_t*& acBE = save3->pokemon_effort[ i ];
                for( u8 j = 0; j < 6; ++j ) {
                    acPkmn.m_boxdata.m_effortValues[ j ] = acBE->EV[ j ];
                    acPkmn.m_boxdata.m_contestStats[ j ] = acBE->ConStat[ j ];
                }

                FS::gen3Pokemon::pokemon_misc_t*& acBM = save3->pokemon_misc[ i ];
                acPkmn.m_boxdata.m_iVint               = acBM->IVint;

                p = getPkmnData( acPkmn.getSpecies( ) );
                acPkmn.m_boxdata.m_ability
                    = p.m_baseForme.m_abilities[ acPkmn.isEgg( ) ];
                acPkmn.m_boxdata.setIsEgg( acPkmn.m_boxdata.isNicknamed( ) );
                acPkmn.m_boxdata.m_gotPlace = FS::getNLocation( acBM->locationcaught );

                acPkmn.m_boxdata.m_gotLevel = acBM->levelcaught;

                if( acPkmn.isEgg( ) || acPkmn.m_boxdata.m_gotLevel ) {
                    acPkmn.m_boxdata.m_hatchPlace           = 999;
                    acPkmn.m_boxdata.m_gotLevel             = 5;
                    acPkmn.m_boxdata.m_hatchDate[ 0 ]       = acPkmn.m_boxdata.m_hatchDate[ 1 ]
                        = acPkmn.m_boxdata.m_hatchDate[ 2 ] = 0;
                    acPkmn.m_boxdata.m_gotDate[ 0 ]         = acPkmn.m_boxdata.m_gotDate[ 1 ]
                        = acPkmn.m_boxdata.m_gotDate[ 2 ]   = 1;
                }
                acPkmn.m_boxdata.m_oTisFemale         = acBM->tgender;
                acPkmn.m_boxdata.m_ball               = acBM->pokeball;
                acPkmn.m_boxdata.m_gotDate[ 0 ]       = acPkmn.m_boxdata.m_gotDate[ 1 ]
                    = acPkmn.m_boxdata.m_gotDate[ 2 ] = 0;
            }
        }

        IO::messageBox( GET_STRING( 123 ), true );
        IO::clearScreen( true, false, false );

        SAV.getActiveFile( ).m_player     = {MAP::mapObject::PLYR,
                                          {104, 120, 5},
                                          SAV.getActiveFile( ).m_isMale ? (u16) 0 : (u16) 10,
                                          MAP::moveMode::WALK,
                                          0,
                                          0,
                                          MAP::direction::RIGHT};
        SAV.getActiveFile( ).m_isMale     = true;
        SAV.getActiveFile( ).m_currentMap = 10;

*/
        return false;
    }
} // namespace SAVE
