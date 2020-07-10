/*
Pokémon neo
------------------------------

file        : startScreen.cpp
author      : Philip Wellnitz
description : Header file. See corresponding source file for details.

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

#include <nds/system.h>

#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "gameStart.h"
#include "mapDrawer.h"
#include "nav.h"
#include "pokemonNames.h"
#include "screenFade.h"
#include "sound.h"
#include "startScreen.h"
#include "uio.h"
#include "yesNoBox.h"

#include "arrow.h"
#include "noselection_32_64.h"
#include "noselection_64_20.h"
#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "noselection_96_32_4.h"

#ifdef DESQUID
// BEGIN TEST
#include "animations.h"
#include "bagViewer.h"
#include "battle.h"
#include "boxViewer.h"
#include "itemNames.h"
#include "moveNames.h"
#include "partyScreen.h"
#include "specials.h"
#include "statusScreen.h"
#include "keyboard.h"
// END TEST
#endif

namespace SAVE {

#define SPR_SMALL_CHOICE_OAM_SUB 0
#define SPR_LARGE_CHOICE_OAM_SUB 1
#define SPR_ARROW_LEFT_OAM_SUB 5
#define SPR_ARROW_RIGHT_OAM_SUB 6
#define SPR_PAGE_LEFT_OAM_SUB 7
#define SPR_PAGE_RIGHT_OAM_SUB 8
#define SPR_MSG_BOX_OAM_SUB 9
#define SPR_CHOICE_OAM_SUB( p_choice ) ( 30 + 10 * ( p_choice ) )
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 30 + 6 * ( p_pos ) )

#define SPR_BOX_PAL_SUB 0
#define SPR_SELECTED_PAL_SUB 1
#define SPR_ARROW_X_PAL_SUB 2

    constexpr language translate( u8 p_dSRegion ) {
        switch( p_dSRegion ) {
        // case 0:
        //    return SAVE::JPN;
        default:
        case 1:
            return SAVE::EN;
        // case 2:
        //    return SAVE::FRE;
        case 3:
            return SAVE::GER;
            // case 4:
            //    return SAVE::ITA;
            // case 5:
            //    return SAVE::SPA;
            // case 6:
            //    return SAVE::CHS;
            // case 7:
            //    return SAVE::KOR;
        }
    }

    void startScreen::drawSplash( ) {
        IO::initOAMTable( false );
        IO::initOAMTable( true );
        IO::clearScreen( true, true, false );
        consoleSelect( &IO::Top );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Title" );

        BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE[ IO::COLOR_IDX ] = IO::CHOICE_COLOR;
        BG_PALETTE[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE[ IO::RED_IDX ]   = IO::GREEN;

        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::WHITE_IDX, 1 );
        IO::boldFont->setColor( IO::BLACK_IDX, 2 );

#ifdef DESQUID
        // BEGIN TEST

        // auto kb = IO::keyboard( );
        // kb.getText( 12 );

        /*
                pokemon testTeam[ 6 ];
                for( u16 i = 0; i < 6; i++ ) {
                    testTeam[ i ]                 = pokemon( PKMN_GYARADOS, 100, 0, 0, 255, false, i
           == 3 ); testTeam[ i ].m_stats.m_curHP = testTeam[ i ].m_stats.m_maxHP * i / 6;
                    SAVE::SAV.getActiveFile( ).storePkmn( pokemon( 1 + ( 3 * i ) % MAX_PKMN,
                                                                   1 + rand( ) % 100, 0, 0, i, rand(
           ) % 2, 3 * i == 490, rand( ) % 20 ) );

                    testTeam[ i ].m_boxdata.m_moves[ 3 ] = M_FOCUS_PUNCH;
                    testTeam[ i ].m_boxdata.m_moves[ 1 ] = M_DIVE;
                    testTeam[ i ].m_boxdata.m_heldItem   = I_GYARADOSITE;
                }
        */
        //        BAG::bagViewer
        //        btv = BAG::bagViewer( testTeam );
        //        btv.run( );
        //        btv = BAG::bagViewer( testTeam, BAG::bagViewer::BATTLE );
        //        btv.run( );
        //        btv = BAG::bagViewer( testTeam, BAG::bagViewer::WILD_BATTLE );
        //        btv.run( );
        //       btv = BAG::bagViewer( testTeam, BAG::bagViewer::GIVE_TO_PKMN );
        //       btv.run( );

        //    BOX::boxViewer bxv;
        //  bxv.run( );

        //      STS::partyScreen sts = STS::partyScreen( testTeam, 5, false, true, true, 2, true );
        //        sts.run( );

        //        STS::statusScreen psts = STS::statusScreen( SAVE::SAV.getActiveFile(
        //        ).getTeamPkmn( 1 ) ); psts.run( );

        //       SOUND::playBGM( MOD_SURFING );

        //        BATTLE::battle bt
        //            = BATTLE::battle( testTeam, 6, pokemon( 1 + rand( ) % MAX_PKMN, 100, 0, 0, 2
        //            ),
        //                              rand( ) % 46, rand( ) % 46, rand( ) % 58 );

        //        bt.start( );

        // END TEST
#endif

        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );

        BG_PALETTE[ 3 ] = BG_PALETTE_SUB[ 3 ] = RGB15( 0, 0, 0 );

        printf( "Free Software, PW 2012 - 2020\n"
                "                             \n"
                "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );

        if( gMod == DEVELOPER )
            printf( "   " __DATE__ " " __TIME__ " v%hhu.%hhu-dev\n", VERSION / 10, VERSION % 10 );
        else if( gMod == BETA )
            printf( "  " __DATE__ " " __TIME__ " v%hhu.%hhu-beta\n", VERSION / 10, VERSION % 10 );
        else if( gMod == ALPHA )
            printf( " " __DATE__ " " __TIME__ " v%hhu.%hhu-alpha\n", VERSION / 10, VERSION % 10 );
        else if( gMod == EMULATOR )
            printf( "   " __DATE__ " " __TIME__ " v%hhu.%hhu-emu\n", VERSION / 10, VERSION % 10 );
        printf( "%32s", VERSION_NAME );

        consoleSetWindow( &IO::Top, 0, 23, 32, 1 );
        consoleSelect( &IO::Top );
        u8 frame = 0;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START )
                || ( ( touch.px || touch.py ) && IO::waitForTouchUp( ) ) ) {
                SOUND::playCry( PKMN_RAYQUAZA );
                for( u8 i = 0; i < 70; ++i ) { swiWaitForVBlank( ); };
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true, false, true );
                IO::fadeScreen( IO::CLEAR_DARK, true, true );

                break;
            }
            ++frame;
            if( !( frame % 120 ) ) {
                IO::boldFont->printString( STRINGS[ 70 ][ _currentLanguage ], 128, 176, false,
                                           IO::font::CENTER );
                frame = 0;
            } else if( ( frame % 120 ) == 60 ) {
                IO::printRectangle( 0, 176, 255, 192, false, 0 );
            }
        }

        IO::clearScreenConsole( true, true );
        consoleSelect( &IO::Top );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x06 << 8 );
        bgUpdate( );
    }

    std::vector<startScreen::choice> startScreen::getMainChoicesForSlot( u8 p_slot ) {
        std::vector<startScreen::choice> res;

        if( SAV.m_saveFile[ p_slot ].m_gameType ) { res.push_back( startScreen::CONTINUE ); }
        res.push_back( startScreen::NEW_GAME );
        if( SAVE::SAV.getSpecialEpisodes( ).size( ) ) {
            res.push_back( startScreen::SPECIAL_EPISODE );
        }
#ifdef DESQUID
        if( gMod == DEVELOPER ) {
            res.push_back( startScreen::TRANSFER_GAME );
            res.push_back( startScreen::MYSTERY_GIFT );
        }
#endif
        return res;
    }

    std::vector<std::pair<IO::inputTarget, startScreen::choice>>
    startScreen::drawMainChoice( const std::vector<choice>& p_choices, u8 p_slot ) {
        std::vector<std::pair<IO::inputTarget, startScreen::choice>> res
            = std::vector<std::pair<IO::inputTarget, startScreen::choice>>( );
        IO::initOAMTable( true );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );

        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x06 << 8 );
        bgUpdate( );

        // Sprites
        SpriteEntry* oam = IO::Oam->oamBuffer;

        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( SPR_LARGE_CHOICE_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, 0, 0, 16, 32,
                                  noselection_96_32_1Pal, noselection_96_32_1Tiles,
                                  noselection_96_32_1TilesLen, false, false, true, OBJPRIORITY_3,
                                  true, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_LARGE_CHOICE_OAM_SUB + 1, SPR_BOX_PAL_SUB, tileCnt, 0, 0, 16,
                                  32, noselection_96_32_2Pal, noselection_96_32_2Tiles,
                                  noselection_96_32_2TilesLen, false, false, true, OBJPRIORITY_3,
                                  true, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_SMALL_CHOICE_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, 0, 0, 32, 32,
                                  noselection_64_20Pal, noselection_64_20Tiles,
                                  noselection_64_20TilesLen, false, false, true, OBJPRIORITY_3,
                                  true, OBJMODE_BLENDED );

        // Arrows
        tileCnt = IO::loadSprite( SPR_ARROW_LEFT_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 4, 76, 16,
                                  16, arrowPal, arrowTiles, arrowTilesLen, false, false, true,
                                  OBJPRIORITY_1, true, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ARROW_RIGHT_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                        oam[ SPR_ARROW_LEFT_OAM_SUB ].gfxIndex, 236, 76, 16, 16, arrowPal,
                        arrowTiles, arrowTilesLen, false, true, true, OBJPRIORITY_1, true,
                        OBJMODE_NORMAL );

        // page windows
        tileCnt = IO::loadSprite( SPR_PAGE_LEFT_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, 0 - 8, 57 - 12,
                                  32, 64, noselection_32_64Pal, noselection_32_64Tiles,
                                  noselection_32_64TilesLen, true, true, true, OBJPRIORITY_2, true,
                                  OBJMODE_BLENDED );
        IO::loadSprite( SPR_PAGE_RIGHT_OAM_SUB, SPR_BOX_PAL_SUB,
                        oam[ SPR_PAGE_LEFT_OAM_SUB ].gfxIndex, 256 - 24, 4 + 28 * 2, 32, 64,
                        noselection_32_64Pal, noselection_32_64Tiles, noselection_32_64TilesLen,
                        false, false, true, OBJPRIORITY_2, true, OBJMODE_BLENDED );

        // Pals
        IO::copySpritePal( arrowPal, SPR_ARROW_X_PAL_SUB, 0, 2 * 4, true );
        IO::copySpritePal( noselection_96_32_4Pal, SPR_SELECTED_PAL_SUB, 0, 2 * 8, true );

        if( _currentSlot != p_slot ) { SAV.m_saveFile[ p_slot ].drawTrainersCard( false ); }

        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x06 << 8 );
        bgUpdate( );

        _currentSlot = p_slot;
        if( SAV.m_saveFile[ p_slot ].m_gameType ) {
            _currentLanguage = SAV.m_saveFile[ p_slot ].m_options.m_language;
        } else {
            _currentLanguage = translate( TWL_CONFIG ? *(u8*) 0x02000406 : PersonalData->language );
        }

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );

        res.push_back( std::pair( IO::inputTarget( 0, 0, 0 ), IO::choiceBox::BACK_CHOICE ) );
        if( p_slot > 0 ) {
            // prev slot button
            res.push_back( std::pair( IO::inputTarget( 1, oam[ SPR_PAGE_LEFT_OAM_SUB ].y + 8, 24,
                                                       oam[ SPR_PAGE_LEFT_OAM_SUB ].y + 48 ),
                                      IO::choiceBox::PREV_PAGE_CHOICE ) );

            oam[ SPR_PAGE_LEFT_OAM_SUB ].isHidden  = false;
            oam[ SPR_ARROW_LEFT_OAM_SUB ].isHidden = false;
        }
        if( p_slot < MAX_SAVE_FILES - 1 ) {
            // next slot button
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_PAGE_RIGHT_OAM_SUB ].x,
                                                       oam[ SPR_PAGE_RIGHT_OAM_SUB ].y + 8,
                                                       oam[ SPR_PAGE_RIGHT_OAM_SUB ].x + 23,
                                                       oam[ SPR_PAGE_RIGHT_OAM_SUB ].y + 48 ),
                                      IO::choiceBox::NEXT_PAGE_CHOICE ) );

            oam[ SPR_PAGE_RIGHT_OAM_SUB ].isHidden  = false;
            oam[ SPR_ARROW_RIGHT_OAM_SUB ].isHidden = false;
        }

        u16 cury = 32;
        u8  ln   = 0;

        for( auto c : p_choices ) {
            if( c == CONTINUE ) {
                IO::regularFont->printStringC(
                    STRINGS[ getTextForMainChoice( c ) ][ _currentLanguage ], 128, cury + 8, true,
                    IO::font::CENTER );

                res.push_back( std::pair( IO::inputTarget( 28, cury, 236, cury + 32 ), ln ) );
                // load sprites

                IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ), SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 48, cury, 16, 32, 0, 0, 0,
                                false, false, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                for( u8 i = 1; i <= 8; ++i ) {
                    IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + i, SPR_BOX_PAL_SUB,
                                    oam[ SPR_LARGE_CHOICE_OAM_SUB + 1 ].gfxIndex, 48 + 16 * i, cury,
                                    16, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                                    OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + 9, SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 48 + 16 * 9, cury, 16, 32,
                                0, 0, 0, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

                cury += 34;
            } else {
                IO::regularFont->printStringC(
                    STRINGS[ getTextForMainChoice( c ) ][ _currentLanguage ], 128, cury + 2, true,
                    IO::font::CENTER );

                res.push_back( std::pair( IO::inputTarget( 28, cury, 236, cury + 20 ), ln ) );

                for( u8 i = 0; i <= 8; ++i ) {
                    IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + i, SPR_BOX_PAL_SUB,
                                    oam[ SPR_SMALL_CHOICE_OAM_SUB ].gfxIndex, 48 + 14 * i, cury, 32,
                                    32, 0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                                    OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + 9, SPR_BOX_PAL_SUB,
                                oam[ SPR_SMALL_CHOICE_OAM_SUB ].gfxIndex, 48 + 16 * 8, cury - 12,
                                32, 32, 0, 0, 0, true, true, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );

                cury += 22;
            }

            ++ln;
        }

        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::boldFont->printString( std::to_string( p_slot + 1 ).c_str( ), 248, 3, false,
                                   IO::font::RIGHT );

        IO::updateOAM( true );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, startScreen::choice>> startScreen::drawEpisodeChoice( ) {

        SpriteEntry* oam = IO::Oam->oamBuffer;
        IO::clearScreen( true, false, true );
        std::vector<std::pair<IO::inputTarget, startScreen::choice>> res
            = std::vector<std::pair<IO::inputTarget, startScreen::choice>>( );

        oam[ SPR_ARROW_LEFT_OAM_SUB ].isHidden  = true;
        oam[ SPR_ARROW_RIGHT_OAM_SUB ].isHidden = true;
        oam[ SPR_PAGE_LEFT_OAM_SUB ].isHidden   = true;
        oam[ SPR_PAGE_RIGHT_OAM_SUB ].isHidden  = true;
        for( u8 j = 0; j < 6; ++j ) {
            for( u8 i = 0; i < 10; ++i ) { oam[ SPR_CHOICE_OAM_SUB( j ) + i ].isHidden = true; }
        }

        auto episodes = SAVE::SAV.getSpecialEpisodes( );
        episodes.push_back( IO::choiceBox::BACK_CHOICE );

        u16 cury = 32;
        u8  ln   = 0;
        for( u8 c = 0; c < episodes.size( ); ++c ) {
            res.push_back( std::pair( IO::inputTarget( 28, cury, 236, cury + 20 ), c ) );
            if( episodes[ c ] != IO::choiceBox::BACK_CHOICE ) {
                IO::regularFont->printStringC( EPISODE_NAMES[ episodes[ c ] ][ _currentLanguage ],
                                               128, cury + 2, true, IO::font::CENTER );
            } else {
                IO::regularFont->printStringC( STRINGS[ 330 ][ _currentLanguage ], 128, cury + 2,
                                               true, IO::font::CENTER );
                res.push_back(
                    std::pair( IO::inputTarget( 0, 0, 0 ), IO::choiceBox::BACK_CHOICE ) );
            }

            for( u8 i = 0; i <= 8; ++i ) {
                IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + i, SPR_BOX_PAL_SUB,
                                oam[ SPR_SMALL_CHOICE_OAM_SUB ].gfxIndex, 48 + 14 * i, cury, 32, 32,
                                0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + 9, SPR_BOX_PAL_SUB,
                            oam[ SPR_SMALL_CHOICE_OAM_SUB ].gfxIndex, 48 + 16 * 8, cury - 12, 32,
                            32, 0, 0, 0, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            cury += 22;
            ++ln;
        }

        IO::updateOAM( true );

        return res;
    }

    void startScreen::selectMainChoice( u8 p_selection ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        oam[ SPR_PAGE_LEFT_OAM_SUB ].palette  = SPR_BOX_PAL_SUB;
        oam[ SPR_PAGE_RIGHT_OAM_SUB ].palette = SPR_BOX_PAL_SUB;
        for( u8 j = 0; j < 6; ++j ) {
            for( u8 i = 0; i < 10; ++i ) {
                oam[ SPR_CHOICE_OAM_SUB( j ) + i ].palette = SPR_BOX_PAL_SUB;
            }
        }

        if( p_selection < 6 ) {
            for( u8 i = 0; i < 10; ++i ) {
                oam[ SPR_CHOICE_OAM_SUB( p_selection ) + i ].palette = SPR_SELECTED_PAL_SUB;
            }
        }
        if( p_selection == IO::choiceBox::NEXT_PAGE_CHOICE ) {
            oam[ SPR_PAGE_RIGHT_OAM_SUB ].palette = SPR_SELECTED_PAL_SUB;
        }
        if( p_selection == IO::choiceBox::PREV_PAGE_CHOICE ) {
            oam[ SPR_PAGE_LEFT_OAM_SUB ].palette = SPR_SELECTED_PAL_SUB;
        }
        IO::updateOAM( true );
    }

    u8 startScreen::runEpisodeChoice( ) {
        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );

        auto res = cb.getResult( [ & ]( u8 ) { return drawEpisodeChoice( ); },
                                 [ & ]( u8 p_choice ) { selectMainChoice( p_choice ); } );

        auto eps = SAVE::SAV.getSpecialEpisodes( );

        if( res == IO::choiceBox::BACK_CHOICE || res >= eps.size( ) ) {
            return IO::choiceBox::BACK_CHOICE;
        } else {
            return eps[ res ];
        }
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    startScreen::printYNMessage( const char* p_message, u8 p_selection ) {

        SpriteEntry* oam = IO::Oam->oamBuffer;

        if( p_message ) {
            oam[ SPR_ARROW_LEFT_OAM_SUB ].isHidden  = true;
            oam[ SPR_ARROW_RIGHT_OAM_SUB ].isHidden = true;
            oam[ SPR_PAGE_LEFT_OAM_SUB ].isHidden   = true;
            oam[ SPR_PAGE_RIGHT_OAM_SUB ].isHidden  = true;
            for( u8 j = 0; j < 6; ++j ) {
                for( u8 i = 0; i < 10; ++i ) { oam[ SPR_CHOICE_OAM_SUB( j ) + i ].isHidden = true; }
            }

            // message box
            IO::loadSprite( SPR_MSG_BOX_OAM_SUB, SPR_BOX_PAL_SUB,
                            oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 29, 80 - 36, 16, 32, 0, 0, 0,
                            false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            for( u8 j = 1; j < 12; j++ ) {
                IO::loadSprite( SPR_MSG_BOX_OAM_SUB + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB + 1 ].gfxIndex, 29 + j * 16, 80 - 36,
                                16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 12, SPR_BOX_PAL_SUB,
                            oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 20 + 12 * 16, 80 - 36, 16, 32,
                            0, 0, 0, true, true, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            // Choice boxes

            for( u8 i = 0; i < 3; i++ ) {
                u8 pos = 2 * i;

                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 29, 80 + i * 36, 16, 32,
                                0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                for( u8 j = 1; j < 5; j++ ) {
                    IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                    oam[ SPR_LARGE_CHOICE_OAM_SUB + 1 ].gfxIndex, 29 + j * 16,
                                    80 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                    true, OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 29 + 5 * 16, 80 + i * 36,
                                16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }

            for( u8 i = 0; i < 3; i++ ) {
                u8 pos = 2 * i + 1;
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 131, 80 + i * 36, 16, 32,
                                0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                for( u8 j = 1; j < 5; j++ ) {
                    IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                    oam[ SPR_LARGE_CHOICE_OAM_SUB + 1 ].gfxIndex, 131 + j * 16,
                                    80 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                    true, OBJMODE_BLENDED );
                }
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                                oam[ SPR_LARGE_CHOICE_OAM_SUB ].gfxIndex, 131 + 5 * 16, 80 + i * 36,
                                16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }
        }

        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> res
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        if( p_message ) {
            for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }

            IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                                256 - oam[ SPR_MSG_BOX_OAM_SUB ].x,
                                oam[ SPR_MSG_BOX_OAM_SUB ].y + 31, true, 0 );

            auto lineCnt = IO::regularFont->printBreakingStringC(
                               p_message, 128, oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, 196, true,
                               IO::font::CENTER, 14, ' ', 0, false, false, -1 )
                           - 1;
            IO::regularFont->printBreakingStringC( p_message, 128,
                                                   oam[ SPR_MSG_BOX_OAM_SUB ].y + 8 - lineCnt * 7,
                                                   196, true, IO::font::CENTER, 14 );
        }

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 6; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( ( i & 1 ) == ( p_selection & 1 ) ) ? SPR_SELECTED_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        if( p_message ) {
            for( u8 i = 0; i < 2; i++ ) {
                for( u8 j = 0; j < 6; j++ ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                }
            }

            IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 95,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 31, true, 0 );
            IO::regularFont->printString(
                STRINGS[ 80 ][ _currentLanguage ], oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 32 ),
                           IO::yesNoBox::YES ) );

            IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 95,
                                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 31, true, 0 );
            IO::regularFont->printString(
                STRINGS[ 81 ][ _currentLanguage ], oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 32 ),
                           IO::yesNoBox::NO ) );
        }
        IO::updateOAM( true );
        return res;
    }

    language startScreen::runLanguageChoice( ) {
        IO::initOAMTable( false );
        IO::clearScreen( true, true, true );
        IO::yesNoBox yn;
        while( yn.getResult(
                   [ & ]( ) { return printYNMessage( STRINGS[ 85 ][ _currentLanguage ], 254 ); },
                   [ & ]( IO::yesNoBox::selection p_sel ) {
                       printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                   } )
               == IO::yesNoBox::YES ) {
            IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
            _currentLanguage = (language) cb.getResult(
                [ & ]( u8 ) {
                    auto         res = std::vector<std::pair<IO::inputTarget, u8>>( );
                    SpriteEntry* oam = IO::Oam->oamBuffer;
                    IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                                        256 - oam[ SPR_MSG_BOX_OAM_SUB ].x,
                                        oam[ SPR_MSG_BOX_OAM_SUB ].y + 31, true, 0 );

                    auto lineCnt = IO::regularFont->printBreakingStringC(
                                       STRINGS[ 109 ][ _currentLanguage ], 128,
                                       oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, 196, true,
                                       IO::font::CENTER, 14, ' ', 0, false, false, -1 )
                                   - 1;
                    IO::regularFont->printBreakingStringC( STRINGS[ 109 ][ _currentLanguage ], 128,
                                                           oam[ SPR_MSG_BOX_OAM_SUB ].y + 8
                                                               - lineCnt * 7,
                                                           196, true, IO::font::CENTER, 14 );

                    for( u8 i = 0; i < LANGUAGES; ++i ) {
                        for( u8 j = 0; j < 6; j++ ) {
                            oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                        }
                        IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 95,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 31, true, 0 );
                        IO::regularFont->printString(
                            LANGUAGE_NAMES[ i ], oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );

                        res.push_back( std::pair(
                            IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                             oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                             oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                             oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                            i ) );
                    }
                    IO::updateOAM( true );
                    return res;
                },
                [ & ]( u8 p_selection ) {
                    SpriteEntry* oam = IO::Oam->oamBuffer;
                    for( u8 i = 0; i < 6; ++i ) {
                        for( u8 j = 0; j < 6; j++ ) {
                            oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette = SPR_BOX_PAL_SUB;
                        }
                    }

                    if( p_selection < 6 ) {
                        for( u8 j = 0; j < 6; j++ ) {
                            oam[ SPR_CHOICE_START_OAM_SUB( p_selection ) + j ].palette
                                = SPR_SELECTED_PAL_SUB;
                        }
                    }
                    IO::updateOAM( true );
                },
                _currentLanguage );
            IO::clearScreen( true, false, true );
        }
        return _currentLanguage;
    }

    void cleanUp( ) {
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::clearScreen( true, true, true );
    }

    bool startScreen::initNewGame( gameType p_type, language p_lang, u8 p_episode ) {
        // Check if this is the first save file
        bool hasSave = false;

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) hasSave |= SAV.m_saveFile[ i ].isGood( );

        if( !hasSave ) { SAV.clear( ); }

        if( SAV.m_saveFile[ _currentSlot ].m_gameType != UNUSED ) {
            // Ask the player if they want to override their save

            IO::yesNoBox yn;
            if( yn.getResult(
                    [ & ]( ) { return printYNMessage( STRINGS[ 79 ][ _currentLanguage ], 254 ); },
                    [ & ]( IO::yesNoBox::selection p_sel ) {
                        printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                    } )
                == IO::yesNoBox::NO ) {

                _currentSlot = 255;
                return false;
            }
        }

        cleanUp( );
        SAV.m_activeFile = _currentSlot;
        SAV.getActiveFile( ).initialize( );
        SAV.getActiveFile( ).m_options.m_language = _currentLanguage;
        SAV.getActiveFile( ).m_gameType           = p_type;
        SAV.getActiveFile( ).m_options.m_bgIdx    = START_BG;

        if( p_type == SPECIAL ) { return initSpecialEpisode( p_episode ); }
        return true;
    }

    void startScreen::run( ) {
        // Check if a valid save is present and use its language.
        // If none is present, use the DS's language.
        if( SAV.getActiveFile( ).m_gameType ) {
            _currentLanguage = SAV.getActiveFile( ).m_options.m_language;
            _currentSlot     = SAV.m_activeFile;
        } else {
            _currentLanguage = translate( TWL_CONFIG ? *(u8*) 0x02000406 : PersonalData->language );
            _currentSlot     = 0;
        }

        loop( ) {
            IO::clearScreen( true, false, false );
            drawSplash( );
            _currentSlot = 255;

            loop( ) {
                IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );

                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "tbg_t" );
                FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "tbg_s", 480,
                                     256 * 192, true );

                auto res = cb.getResult(
                    [ & ]( u8 p_slot ) {
                        return drawMainChoice( getMainChoicesForSlot( p_slot ), p_slot );
                    },
                    [ & ]( u8 p_choice ) { selectMainChoice( p_choice ); }, 0,
                    IO::choiceBox::DEFAULT_TICK,
                    _currentSlot == 255 ? SAVE::SAV.m_activeFile : _currentSlot );

                if( res == IO::choiceBox::BACK_CHOICE ) { break; }
                res = getMainChoicesForSlot( _currentSlot )[ res ];
                switch( res ) {
                case CONTINUE:
                    SAV.m_activeFile = _currentSlot;
                    cleanUp( );
                    return;
                case NEW_GAME:
                    if( !initNewGame( NORMAL, runLanguageChoice( ) ) ) break;
                    return;
                case TRANSFER_GAME:
                    if( !transferGame( ) ) break;
                    return;
                case SPECIAL_EPISODE: {
                    u8 ep = runEpisodeChoice( );
                    if( ep == IO::choiceBox::BACK_CHOICE
                        || !initNewGame( SPECIAL, runLanguageChoice( ), ep ) )
                        break;
                    return;
                }
                default:
                    continue;
                }
            }
        }
    }
} // namespace SAVE
