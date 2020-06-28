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
#include "messageBox.h"
#include "nav.h"
#include "screenFade.h"
#include "sound.h"
#include "startScreen.h"
#include "uio.h"
#include "yesNoBox.h"

// BEGIN TEST
#include "animations.h"
#include "boxViewer.h"
#include "itemNames.h"
#include "moveNames.h"
#include "partyScreen.h"
#include "statusScreen.h"
#include "pokemonNames.h"
#include "bagViewer.h"

#include "battle.h"
// END TEST

namespace SAVE {
    void initColors( ) {
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::CHOICE_COLOR;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::NORMAL_COLOR;
        BG_PALETTE_SUB[ IO::RED_IDX ]   = IO::GREEN;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
    }

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

    void drawSplash( language p_lang ) {
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Title" );
        IO::clearScreen( true, false, false );

        // BEGIN TEST

        pokemon testTeam[ 6 ];
        for( u16 i = 0; i < 6; i++ ) {
            testTeam[ i ] = pokemon( PKMN_GYARADOS, 100, 0, 0, 255, false, i == 3 );
            testTeam[ i ].m_stats.m_curHP = testTeam[ i ].m_stats.m_maxHP * i / 6;
            //            SAVE::SAV.getActiveFile( ).storePkmn(
            //                pokemon( 1 + ( 3 * i ) % MAX_PKMN, 1 + rand( ) % 100, 0, 0, i, rand( )
            //                % 2, 3 * i ==
            //                    490, rand( ) % 20 ) );

            testTeam[ i ].m_boxdata.m_moves[ 3 ] = M_FOCUS_PUNCH;
            testTeam[ i ].m_boxdata.m_moves[ 1 ] = M_DIVE;
            testTeam[ i ].m_boxdata.m_heldItem = I_GYARADOSITE;
        }

//        BAG::bagViewer
//        btv = BAG::bagViewer( testTeam );
//        btv.run( );
//        btv = BAG::bagViewer( testTeam, BAG::bagViewer::BATTLE );
//        btv.run( );
//        btv = BAG::bagViewer( testTeam, BAG::bagViewer::WILD_BATTLE );
//        btv.run( );
 //       btv = BAG::bagViewer( testTeam, BAG::bagViewer::GIVE_TO_PKMN );
 //       btv.run( );

        // BOX::boxViewer bxv;
        // bxv.run( );

       // STS::partyScreen sts = STS::partyScreen( testTeam, 5, false, true, true, 2, true );
       // sts.run( );

        //        STS::statusScreen psts = STS::statusScreen( SAVE::SAV.getActiveFile(
        //        ).getTeamPkmn( 1 ) ); psts.run( );

        //       SOUND::playBGM( MOD_SURFING );

        BATTLE::battle bt
            = BATTLE::battle( testTeam, 6, pokemon( 1 + rand( ) % MAX_PKMN, 100, 0, 0, 2 ),
                    rand( ) % 46, rand( ) % 46, rand( ) % 58 );

        bt.start( );

        // END TEST

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
        u8            frame = 0;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START )
                    || ( ( touch.px || touch.py ) && IO::waitForTouchUp( ) ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true, false, false );
                IO::fadeScreen( IO::CLEAR_WHITE, true, true );
                break;
            }
            ++frame;
            if( !( frame % 120 ) ) {
                printf( STRINGS[ 70 ][ p_lang ] );
                frame = 0;
            } else if( ( frame % 120 ) == 60 )
                consoleClear( );
        }

        IO::clearScreenConsole( true, true );
        consoleSelect( &IO::Top );
    }

    void startScreen::drawMainChoice( language p_lang, std::vector<u8> p_toDraw, s8 p_selected,
                                      s8 p_pressed ) {
        initColors( );
        for( u8 i = 0; i < p_toDraw.size( ); ++i ) {
            IO::printChoiceBox( 4, 4 + 48 * i, 136, 26 + 48 * i, 6,
                                ( i == p_selected ) ? IO::RED_IDX : IO::COLOR_IDX, i == p_pressed );
            IO::regularFont->printString( STRINGS[ p_toDraw[ i ] ][ p_lang ],
                                          70 + 2 * ( i == p_pressed ),
                                          8 + 48 * i + ( i == p_pressed ), true, IO::font::CENTER );
        }
        swiWaitForVBlank( );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::WHITE_IDX, 1 );
        for( u8 i = 0; i < p_toDraw.size( ); ++i ) {
            if( i == p_selected || i == p_pressed )
                IO::boldFont->setColor( IO::BLACK_IDX, 2 );
            else
                IO::boldFont->setColor( IO::GRAY_IDX, 2 );
            IO::boldFont->printString( STRINGS[ p_toDraw[ i ] + 4 ][ p_lang ], 248, 28 + 48 * i,
                                       true, IO::font::RIGHT );
        }
    }

    void fillResume( ) {
        IO::regularFont->setColor( IO::BLUE_IDX, 1 );
        IO::regularFont->setColor( IO::WHITE_IDX, 2 );
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            if( !SAV.m_saveFile[ i ].m_gameType ) continue;

            if( SAV.m_saveFile[ i ].m_isMale )
                BG_PALETTE_SUB[ IO::BLUE_IDX ] = IO::BLUE2;
            else
                BG_PALETTE_SUB[ IO::BLUE_IDX ] = IO::RED2;

            IO::regularFont->printString( SAV.m_saveFile[ i ].m_playername, 8, 30 + 64 * i, true );
            IO::regularFont->printString(
                FS::getLocation( MAP::curMap->getCurrentLocationId( ) ).c_str( ), 248, 30 + 64 * i,
                true, IO::font::RIGHT );

            char buffer[ 50 ];
            snprintf( buffer, 49, "%03d:%02d", SAV.m_saveFile[ i ].m_pt.m_hours,
                      SAV.m_saveFile[ i ].m_pt.m_mins );
            IO::regularFont->printString( buffer, 8, 46 + 64 * i, true );

            snprintf( buffer, 49, STRINGS[ 108 ][ SAV.m_saveFile[ i ].m_options.m_language ],
                      SAV.m_saveFile[ i ].getBadgeCount( ) );
            IO::regularFont->printString( buffer, 110, 46 + 64 * i, true, IO::font::CENTER );

            snprintf( buffer, 49, "PokéDex %03d", SAV.m_saveFile[ i ].getDexCount( ) );
            IO::regularFont->printString( buffer, 248, 46 + 64 * i, true, IO::font::RIGHT );
        }
    }

    void startScreen::drawSlotChoice( language p_current, s8 p_selected, s8 p_pressed ) {
        initColors( );
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            IO::printChoiceBox( 4, 4 + 64 * i, 86, 26 + 64 * i, 6,
                                ( i == p_selected ) ? IO::RED_IDX : IO::COLOR_IDX, i == p_pressed );
            if( !SAV.m_saveFile[ i ].isGood( ) ) { SAV.m_saveFile[ i ].m_gameType = UNUSED; }
            switch( SAV.m_saveFile[ i ].m_gameType ) {
            case UNUSED:
                IO::regularFont->printString(
                    STRINGS[ 82 ][ p_current ], 45 + 2 * ( i == p_pressed ),
                    8 + 64 * i + ( i == p_pressed ), true, IO::font::CENTER );
                break;
            case NORMAL:
                IO::regularFont->printString(
                    CHAPTER_NAMES[ 2 * SAV.m_saveFile[ i ].m_chapter ]
                                 [ SAV.m_saveFile[ i ].m_options.m_language ],
                    45 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true,
                    IO::font::CENTER );
                IO::regularFont->printString(
                    CHAPTER_NAMES[ 2 * SAV.m_saveFile[ i ].m_chapter + 1 ]
                                 [ SAV.m_saveFile[ i ].m_options.m_language ],
                    248, 8 + 64 * i, true, IO::font::RIGHT );
                break;
            case TRANSFER:
                IO::regularFont->printString(
                    STRINGS[ 83 ][ SAV.m_saveFile[ i ].m_options.m_language ],
                    45 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true,
                    IO::font::CENTER );
                break;
            default:
                IO::regularFont->printString(
                    STRINGS[ 84 ][ SAV.m_saveFile[ i ].m_options.m_language ],
                    45 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true,
                    IO::font::CENTER );
                IO::regularFont->printString(
                    EPISODE_NAMES[ SAV.m_saveFile[ i ].m_options.m_language ]
                                 [ SAV.m_saveFile[ i ].m_gameType - (u8) SPECIAL ],
                    248, 8 + 64 * i, true, IO::font::RIGHT );
            }
        }
        swiWaitForVBlank( );
    }

    startScreen::choiceType startScreen::runMainChoice( language p_lang ) {
        std::vector<u8>         vis;
        std::vector<choiceType> res;
        bool                    hasSave = false;

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) hasSave |= !!( SAV.m_saveFile[ i ].m_gameType );

        if( hasSave ) {
            vis.push_back( 71 );
            res.push_back( CONTINUE );
        }
        vis.push_back( 72 );
        res.push_back( NEW_GAME );
        vis.push_back( 73 );
        res.push_back( SPECIAL_EPISODE );
        if( gMod == DEVELOPER ) {
            vis.push_back( 74 );
            res.push_back( TRANSFER_GAME );
        }

        u8 selectedIdx = 0;
        drawMainChoice( p_lang, vis, 0 );

        IO::fadeScreen( IO::UNFADE, true, true );

        touchPosition touch;
        consoleSelect( &IO::Bottom );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            u32 pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true );
                for( u16 i = 1; i < 256; ++i ) BG_PALETTE_SUB[ i ] = IO::WHITE;
                return ABORT;
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                selectedIdx = ( selectedIdx + 1 ) % vis.size( );
                drawMainChoice( p_lang, vis, selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                selectedIdx = ( selectedIdx + vis.size( ) - 1 ) % vis.size( );
                drawMainChoice( p_lang, vis, selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                return res[ selectedIdx ];
            }
            /*
            for( u8 i = 0; i < vis.size( ); ++i )
                if( IN_RANGE_R( 4, 4 + 48 * i, 136, 26 + 48 * i ) ) {
                    selectedIdx = i;
                    drawMainChoice( p_lang, vis, selectedIdx, i );

                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );

                        if( TOUCH_UP ) {
                            SOUND::playSoundEffect( SFX_CHOOSE );
                            return res[ selectedIdx ];
                        }
                        if( !IN_RANGE_R( 4, 4 + 48 * i, 136, 26 + 48 * i ) ) break;
                    }
                    drawMainChoice( p_lang, vis, selectedIdx );
                }
                */
        }
    }

    s8 startScreen::runSlotChoice( language p_lang, bool p_newGameMode ) {
        u8 selectedIdx = 0;
        drawSlotChoice( p_lang, 0 );
        fillResume( );

        touchPosition touch;
        consoleSelect( &IO::Bottom );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            u32 pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true );
                for( u16 i = 1; i < 256; ++i ) BG_PALETTE_SUB[ i ] = IO::WHITE;
                return -1;
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                selectedIdx = ( selectedIdx + 1 ) % MAX_SAVE_FILES;
                drawSlotChoice( p_lang, selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                selectedIdx = ( selectedIdx + MAX_SAVE_FILES - 1 ) % MAX_SAVE_FILES;
                drawSlotChoice( p_lang, selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                if( SAV.m_saveFile[ selectedIdx ].m_gameType && p_newGameMode ) {
                    IO::clearScreen( true, false, false );
                    bool r = !IO::yesNoBox( p_lang ).getResult( STRINGS[ 79 ][ p_lang ] );
                    IO::clearScreen( true, false, false );
                    drawSlotChoice( p_lang, selectedIdx );
                    fillResume( );
                    if( r ) continue;
                } else if( !SAV.m_saveFile[ selectedIdx ].m_gameType && !p_newGameMode )
                    continue;
                return selectedIdx;
            }
            /*
            for( u8 i = 0; i < MAX_SAVE_FILES; ++i )
                if( IN_RANGE_R( 4, 4 + 64 * i, 86, 26 + 64 * i ) ) {
                    selectedIdx = i;
                    drawSlotChoice( p_lang, selectedIdx, i );
                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP ) {
                            SOUND::playSoundEffect( SFX_CHOOSE );
                            return selectedIdx;
                        }
                        if( !IN_RANGE_R( 4, 4 + 64 * i, 86, 26 + 64 * i ) ) break;
                    }
                    drawSlotChoice( p_lang, selectedIdx );
                }
                */
        }
    }

    gameType startScreen::runEpisodeChoice( language p_current ) {
        IO::choiceBox cb
            = IO::choiceBox( MAX_SPECIAL_EPISODES, EPISODE_NAMES[ p_current ], 0, true );
        return ( gameType )( SPECIAL + cb.getResult( STRINGS[ 110 ][ p_current ], true, false ) );
    }

    language startScreen::runLanguageChoice( language p_current ) {
        IO::clearScreen( true, false, false );
        while( IO::yesNoBox( p_current ).getResult( STRINGS[ 85 ][ p_current ] ) ) {
            IO::clearScreen( true, false, false );
            IO::choiceBox cb = IO::choiceBox( LANGUAGES, LANGUAGE_NAMES, 0, true );
            p_current        = (language) cb.getResult( STRINGS[ 109 ][ p_current ], false, false );
            IO::clearScreen( true, false, false );
        }
        return p_current;
    }

    bool startScreen::initNewGame( u8 p_file, gameType p_type, language p_lang ) {
        // Check if this is the first save file
        bool hasSave = false;

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) hasSave |= SAV.m_saveFile[ i ].isGood( );

        if( !hasSave ) { SAV.clear( ); }

        SAV.m_activeFile = p_file;

        SAV.getActiveFile( ).initialize( );
        SAV.getActiveFile( ).m_curBox             = 0;
        SAV.getActiveFile( ).m_options.m_language = p_lang;

        SAV.getActiveFile( ).m_money = 3000;
        SAV.getActiveFile( ).m_id    = rand( ) % 65536;
        SAV.getActiveFile( ).m_sid   = rand( ) % 65536;

        SAV.getActiveFile( ).m_gameType        = p_type;
        SAV.getActiveFile( ).m_options.m_bgIdx = START_BG;

        SAV.getActiveFile( ).m_playtime     = 0;
        SAV.getActiveFile( ).m_HOENN_Badges = 0;
        SAV.getActiveFile( ).m_KANTO_Badges = 0;
        SAV.getActiveFile( ).m_JOHTO_Badges = 0;
        SAV.getActiveFile( ).m_lstBag       = 0;
        SAV.getActiveFile( ).m_lstBagItem   = 0;

        return initSpecialEpisode( SAV.getActiveFile( ).m_gameType - (s8) SPECIAL );
    }

    void startScreen::run( ) {
        // Check if a valid save is present and use its language.
        // If none is present, use the DS's language.
        language cur;
        if( SAV.getActiveFile( ).m_gameType )
            cur = SAV.getActiveFile( ).m_options.m_language;
        else
            cur = translate( TWL_CONFIG ? *(u8*) 0x02000406 : PersonalData->language );

        loop( ) {
            drawSplash( cur );

            choiceType res = runMainChoice( cur );

            if( res == ABORT ) continue;

            IO::clearScreen( true, false, false );
            s8 slot;

            if( res == SPECIAL_EPISODE ) {
                gameType ep = runEpisodeChoice( cur );
                IO::initOAMTable( true );

                if( ep < SPECIAL ) continue;
                IO::clearScreen( true, false, false );
                slot = runSlotChoice( cur, res != CONTINUE );

                if( slot == (s8) -1 ) continue;

                IO::clearScreen( true, false, false );
                if( !initNewGame( slot, ep, runLanguageChoice( cur ) ) ) continue;
                return;
            }

            slot = runSlotChoice( cur, res != CONTINUE );

            if( slot == (s8) -1 ) continue;

            switch( res ) {
            case CONTINUE:
                SAV.m_activeFile = slot;
                return;
            case NEW_GAME:
                if( !initNewGame( slot, NORMAL, runLanguageChoice( cur ) ) ) continue;
                return;
            case TRANSFER_GAME:
                if( !transferGame( slot ) ) continue;
                return;
            default:
                continue;
            }
        }
    }
} // namespace SAVE
