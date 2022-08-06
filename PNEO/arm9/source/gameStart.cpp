/*
Pokémon neo
------------------------------

file        : gameStart.cpp
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

#include "save/gameStart.h"
#include "bag/bag.h"
#include "fs/data.h"
#include "fs/fs.h"
#include "gen/itemNames.h"
#include "gen/pokemonNames.h"
#include "io/choiceBox.h"
#include "io/keyboard.h"
#include "io/menuUI.h"
#include "io/screenFade.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "save/saveGame.h"
#include "save/startScreen.h"
#include "sound/sound.h"

namespace SAVE {
    constexpr u16 EP_INTRO_TEXT_START[] = { IO::STR_UI_EP0_TEXT_START };
    constexpr u8  EP_INTRO_TEXT_LEN[] = { IO::STR_UI_EP0_TEXT_END - IO::STR_UI_EP0_TEXT_START + 1 };

    void printTextAndWait( const char* p_text ) {
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        IO::clearScreen( true, true, true );
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GRAY;
        u8 nw                           = 0;
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
        printTextAndWait( GET_STRING( IO::STR_UI_INIT_GAME_TEXT0 ) );

        IO::clearScreen( true, true, true );
        SOUND::playBGM( BGM_ROUTE_123 );
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "intro", 240 * 2, 256 * 192,
                             true );

        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "Border", 64, 192,
                             192 * 256, true );
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

        for( u8 i = 0; i < 4; ++i ) {
            printMBoxTextAndWait( GET_STRING( IO::STR_UI_INIT_GAME_TEXT1 + i ) );
        }

        // poke ball demo animation
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

        // pball opened, load pkmn sprite
        pkmnSpriteInfo pinfo
            = { PKMN_LOTAD, 0, false, !( rand( ) & 31 ), false, DEFAULT_SPRITE_PID };
        IO::loadPKMNSprite( pinfo, 40, 36, 23, 3, tileCnt, true );
        for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        SOUND::playCry( PKMN_LOTAD );
        for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 11; ++i ) {
            printMBoxTextAndWait( GET_STRING( IO::STR_UI_INIT_GAME_TEXT2 + i ) );
        }

        // run chara creation
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

            IO::regularFont->printBreakingStringC(
                GET_STRING( IO::STR_UI_INIT_GAME_CHOOSE_APPEARANCE ), 8, 8, 240, true,
                IO::font::LEFT, 16 );

            IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_LEFT_RIGHT );

            // make player pick an appearance
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

            SAV.getActiveFile( ).setFlag( F_RIVAL_APPEARANCE,
                                          1 - SAV.getActiveFile( ).m_appearance );

            FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "tbg_s" );
            SAV.getActiveFile( ).drawTrainersCard( false, true );
            IO::loadTrainerSprite( SAV.getActiveFile( ).m_appearance, 33, 45, 0, 0, 0, false );
            IO::updateOAM( false );

            // make player pick a name
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 50 );
            IO::regularFont->printBreakingStringC( GET_STRING( IO::STR_UI_INIT_GAME_CHOOSE_NAME ),
                                                   8, 4, 40, true, IO::font::LEFT, 13 );

            auto name = IO::keyboard( ).getText( 10 );
            if( name == "" ) {
                name = GET_STRING( IO::STR_UI_INIT_GAME_DEFAULT_NAME0
                                   + SAV.getActiveFile( ).m_appearance );
            }
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
            // init sprites for yes/no box
            for( u8 i = 0; i < 3; i++ ) {
                u8 pos = 2 * i;

                if( !i ) {
                    tileCnt = IO::loadSprite( "SEL/noselection_96_32_1",
                                              SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                              tileCnt, 29, 42 + i * 36, 16, 32, false, false, true,
                                              OBJPRIORITY_3, true, OBJMODE_BLENDED );
                    tileCnt = IO::loadSprite( "SEL/noselection_96_32_2",
                                              SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                              tileCnt, 29 + 16, 42 + i * 36, 16, 32, false, false,
                                              true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                } else {
                    IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                    IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                                    29, 42 + i * 36, 16, 32, 0, 0, 0, false, false, true,
                                    OBJPRIORITY_3, true, OBJMODE_BLENDED );
                    IO::loadSprite(
                        SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16,
                        42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
                }
                for( u8 j = 2; j < 7; j++ ) {
                    IO::loadSprite(
                        SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                        29 + j * 11, 42 + i * 36, 16, 32, 0, 0, 0, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                                29 + 5 * 16, 42 + i * 36, 16, 32, 0, 0, 0, true, true, true,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }

            for( u8 i = 0; i < 3; i++ ) {
                u8 pos = 2 * i + 1;
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_BLENDED );
                for( u8 j = 1; j < 7; j++ ) {
                    IO::loadSprite(
                        SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                        131 + j * 11, 42 + i * 36, 16, 32, 0, 0, 0, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                                131 + 5 * 16, 42 + i * 36, 16, 32, 0, 0, 0, true, true, true,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }

            IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );

#undef SPR_BOX_PAL_SUB
#undef SPR_BOX_SEL_PAL_SUB
#undef SPR_CHOICE_START_OAM_SUB
        } while( IO::yesNoBox( ).getResult( // make player confirm their chara choice
                     [ & ]( ) {
                         auto res = IO::printYNMessage( 0, MSG_NORMAL, 253 );

                         IO::regularFont->printBreakingStringC(
                             GET_STRING( IO::STR_UI_INIT_GAME_CHARACTER_OK ), 8, 8, 240, true,
                             IO::font::LEFT, 16 );
                         return res;
                     },
                     [ & ]( IO::yesNoBox::selection p_selection ) {
                         IO::printYNMessage( 0, MSG_NORMAL, p_selection == IO::yesNoBox::NO );
                     } )
                 == IO::yesNoBox::NO );

        // re-init sprites for initial game conversation/explanation
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::clearScreen( true, true, true );
        IO::fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "intro", 240 * 2, 256 * 192,
                             true );

        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "Border", 64, 192,
                             192 * 256, true );
        tileCnt = 0;
        tileCnt = IO::loadTrainerSprite( 240, 80, 32, 0, 0, tileCnt, true );
        tileCnt = IO::loadPlatform( 10, 48, 16 + 48, 1, 1, tileCnt, true );

        // load mbox

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

        auto p1 = std::string( GET_STRING( IO::STR_UI_INIT_GAME_TEXT3 ) );
        p1 += std::string( SAV.getActiveFile( ).m_playername );
        p1 += std::string( GET_STRING( IO::STR_UI_INIT_GAME_TEXT4 ) );
        printMBoxTextAndWait( p1.c_str( ) );

        for( u8 i = 0; i < 4; ++i ) {
            printMBoxTextAndWait( GET_STRING( IO::STR_UI_INIT_GAME_TEXT5 + i ) );
        }

        // initialize player data / send player to a reasonable start position on the map

        SAV.getActiveFile( ).m_currentMap = 10;
        SAV.getActiveFile( ).m_player     = MAP::mapPlayer(
                { u16( 0xb4 + ( 9 * !!SAV.getActiveFile( ).m_appearance ) ), 0x15c, 3 },
                u16( 10 * SAV.getActiveFile( ).m_appearance ), MAP::moveMode::WALK );
        SAVE::SAV.getActiveFile( ).m_player.m_direction = MAP::RIGHT;
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

        SOUND::playBGM( BGM_ROUTE_123 );
        printEpisodeInfo( p_episode );

        switch( p_episode ) {
        case 0:
            // Initialize character and send them to starting towm
            std::strncpy( SAV.getActiveFile( ).m_playername, "Test", 11 );
            SAV.getActiveFile( ).m_appearance = 1;
            SAV.getActiveFile( ).setFlag( F_RIVAL_APPEARANCE,
                                          1 - SAV.getActiveFile( ).m_appearance );
            SAV.getActiveFile( ).m_currentMap = 10;
            SAV.getActiveFile( ).m_player     = MAP::mapPlayer(
                    { u16( 0xb4 + ( 9 * !!SAV.getActiveFile( ).m_appearance ) ), 0x15c, 3 },
                    u16( 10 * SAV.getActiveFile( ).m_appearance ), MAP::moveMode::WALK );
            SAVE::SAV.getActiveFile( ).m_player.m_direction = MAP::RIGHT;

            // hand out useful items
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_MACH_BIKE, 1 );
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_ACRO_BIKE, 1 );
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_POKE_RADAR, 1 );
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_EXP_ALL, 1 );
            SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_SUPER_ROD, 1 );
            SAV.getActiveFile( ).m_currentMap     = 10;
            SAV.getActiveFile( ).m_registeredItem = I_MACH_BIKE;
            for( u8 i = 10; i; --i ) {
                SOUND::setVolume( 0x10 * i );
                swiWaitForVBlank( );
            }
            // at most 4 init game items
            SAVE::SAV.getActiveFile( ).m_initGameItemCount  = 2;
            SAVE::SAV.getActiveFile( ).m_initGameItems[ 0 ] = I_WISHING_CHARM;
            SAVE::SAV.getActiveFile( ).m_initGameItems[ 1 ] = I_SHINY_CHARM;

            return true;
        default: SAV.getActiveFile( ).m_gameType = UNUSED; return false;
        }
    }

    bool startScreen::transferGame( ) {
        // TODO
        return false;
    }
} // namespace SAVE
