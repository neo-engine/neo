/*
Pokémon Emerald 2 Version
------------------------------

file        : startScreen.cpp
author      : Philip Wellnitz
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2018
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
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
#include "startScreen.h"
#include "uio.h"
#include "yesNoBox.h"

namespace SAVE {
    void initColors( ) {
        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        BG_PALETTE_SUB[ GRAY_IDX ]  = NORMAL_COLOR;
        BG_PALETTE_SUB[ RED_IDX ]   = GREEN;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
    }

    language translate( u8 p_dSRegion ) {
        switch( p_dSRegion ) {
        case 1:
            // return SAVE::EN;
        case 3:
        default:
            return SAVE::GER;
            // default:
            // return SAVE::EN;
        }
    }

    void drawSplash( language p_lang ) {
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Title" );
        IO::clearScreen( true, false, false );

        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );

        BG_PALETTE[ 3 ] = BG_PALETTE_SUB[ 3 ] = RGB15( 0, 0, 0 );

        printf( "@ Philip Wellnitz 2012 - 2018\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );

        if( gMod == DEVELOPER )
            printf( "                   %10sdev\n", VERSION );
        else if( gMod == BETA )
            printf( "                  %10sbeta\n", VERSION );
        else if( gMod == ALPHA )
            printf( "                 %10salpha\n", VERSION );
        else if( gMod == EMULATOR )
            printf( "                   %10semu\n", VERSION );
        printf( "%32s", VERSION_NAME );

        consoleSetWindow( &IO::Top, 0, 23, 32, 1 );
        consoleSelect( &IO::Top );
        u8            frame = 0;
        touchPosition touch;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START )
                || GET_AND_WAIT_R( 1, 1, 256, 192 ) )
                break;
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
                                ( i == p_selected ) ? RED_IDX : COLOR_IDX, i == p_pressed );
            IO::regularFont->printString( STRINGS[ p_toDraw[ i ] ][ p_lang ],
                                          70 + 2 * ( i == p_pressed ),
                                          8 + 48 * i + ( i == p_pressed ), true, IO::font::CENTER );
        }
        swiWaitForVBlank( );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( WHITE_IDX, 1 );
        for( u8 i = 0; i < p_toDraw.size( ); ++i ) {
            if( i == p_selected || i == p_pressed )
                IO::boldFont->setColor( BLACK_IDX, 2 );
            else
                IO::boldFont->setColor( GRAY_IDX, 2 );
            IO::boldFont->printString( STRINGS[ p_toDraw[ i ] + 4 ][ p_lang ], 248, 28 + 48 * i,
                                       true, IO::font::RIGHT );
        }
    }

    void fillResume( ) {
        IO::regularFont->setColor( BLUE_IDX, 1 );
        IO::regularFont->setColor( WHITE_IDX, 2 );
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            if( !SAV->m_saveFile[ i ].m_gameType ) continue;

            if( SAV->m_saveFile[ i ].m_isMale )
                BG_PALETTE_SUB[ BLUE_IDX ] = BLUE2;
            else
                BG_PALETTE_SUB[ BLUE_IDX ] = RED2;

            IO::regularFont->printString( SAV->m_saveFile[ i ].m_playername, 8, 30 + 64 * i, true );
            IO::regularFont->printString(
                FS::getLocation( MAP::curMap->getCurrentLocationId( ) ).c_str( ), 248, 30 + 64 * i,
                true, IO::font::RIGHT );

            char buffer[ 50 ];
            snprintf( buffer, 49, "%03d:%02d", SAV->m_saveFile[ i ].m_pt.m_hours,
                      SAV->getActiveFile( ).m_pt.m_mins );
            IO::regularFont->printString( buffer, 8, 46 + 64 * i, true );

            snprintf( buffer, 49, STRINGS[ 108 ][ SAV->m_saveFile[ i ].m_options.m_language ],
                      SAV->m_saveFile[ i ].getBadgeCount( ) );
            IO::regularFont->printString( buffer, 110, 46 + 64 * i, true, IO::font::CENTER );

            snprintf( buffer, 49, "PokéDex %03d", SAV->getDexCount( ) );
            IO::regularFont->printString( buffer, 248, 46 + 64 * i, true, IO::font::RIGHT );
        }
    }

    void startScreen::drawSlotChoice( language p_current, s8 p_selected, s8 p_pressed ) {
        initColors( );
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            IO::printChoiceBox( 4, 4 + 64 * i, 86, 26 + 64 * i, 6,
                                ( i == p_selected ) ? RED_IDX : COLOR_IDX, i == p_pressed );
            switch( SAV->m_saveFile[ i ].m_gameType ) {
            case UNUSED:
                IO::regularFont->printString(
                    STRINGS[ 82 ][ p_current ], 45 + 2 * ( i == p_pressed ),
                    8 + 64 * i + ( i == p_pressed ), true, IO::font::CENTER );
                break;
            case NORMAL:
                IO::regularFont->printString(
                    CHAPTER_NAMES[ 2 * SAV->m_saveFile[ i ].m_chapter ]
                                 [ SAV->m_saveFile[ i ].m_options.m_language ],
                    45 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true,
                    IO::font::CENTER );
                IO::regularFont->printString(
                    CHAPTER_NAMES[ 2 * SAV->m_saveFile[ i ].m_chapter + 1 ]
                                 [ SAV->m_saveFile[ i ].m_options.m_language ],
                    248, 8 + 64 * i, true, IO::font::RIGHT );
                break;
            case TRANSFER:
                IO::regularFont->printString(
                    STRINGS[ 83 ][ SAV->m_saveFile[ i ].m_options.m_language ],
                    45 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true,
                    IO::font::CENTER );
                break;
            default:
                IO::regularFont->printString(
                    STRINGS[ 84 ][ SAV->m_saveFile[ i ].m_options.m_language ],
                    45 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true,
                    IO::font::CENTER );
                IO::regularFont->printString(
                    EPISODE_NAMES[ SAV->m_saveFile[ i ].m_options.m_language ]
                                 [ SAV->m_saveFile[ i ].m_gameType - (u8) SPECIAL ],
                    248, 8 + 64 * i, true, IO::font::RIGHT );
            }
        }
        swiWaitForVBlank( );
    }

    startScreen::choiceType startScreen::runMainChoice( language p_lang ) {
        std::vector<u8>         vis;
        std::vector<choiceType> res;
        bool                    hasSave = false;

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) hasSave |= !!( SAV->m_saveFile[ i ].m_gameType );

        if( hasSave ) {
            vis.push_back( 71 );
            res.push_back( CONTINUE );
        }
        //  vis.push_back( 72 );
        //  res.push_back( NEW_GAME );
        vis.push_back( 73 );
        res.push_back( SPECIAL_EPISODE );
        //  if( gMod == DEVELOPER || SAV->m_transfersRemaining ) {
        //      vis.push_back( 74 );
        //      res.push_back( TRANSFER_GAME );
        //  }

        u8 selectedIdx = 0;
        drawMainChoice( p_lang, vis, 0 );

        touchPosition touch;
        consoleSelect( &IO::Bottom );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            u32 pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_B ) ) {
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true );
                for( u16 i = 1; i < 256; ++i ) BG_PALETTE_SUB[ i ] = WHITE;
                return ABORT;
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                selectedIdx = ( selectedIdx + 1 ) % vis.size( );
                drawMainChoice( p_lang, vis, selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                selectedIdx = ( selectedIdx + vis.size( ) - 1 ) % vis.size( );
                drawMainChoice( p_lang, vis, selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) )
                return res[ selectedIdx ];

            for( u8 i = 0; i < vis.size( ); ++i )
                if( IN_RANGE_R( 4, 4 + 48 * i, 136, 26 + 48 * i ) ) {
                    selectedIdx = i;
                    drawMainChoice( p_lang, vis, selectedIdx, i );

                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );

                        if( TOUCH_UP ) return res[ selectedIdx ];
                        if( !IN_RANGE_R( 4, 4 + 48 * i, 136, 26 + 48 * i ) ) break;
                    }
                    drawMainChoice( p_lang, vis, selectedIdx );
                }
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
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true );
                for( u16 i = 1; i < 256; ++i ) BG_PALETTE_SUB[ i ] = WHITE;
                return -1;
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                selectedIdx = ( selectedIdx + 1 ) % MAX_SAVE_FILES;
                drawSlotChoice( p_lang, selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                selectedIdx = ( selectedIdx + MAX_SAVE_FILES - 1 ) % MAX_SAVE_FILES;
                drawSlotChoice( p_lang, selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) ) {
                if( SAV->m_saveFile[ selectedIdx ].m_gameType && p_newGameMode ) {
                    IO::clearScreen( true, false, false );
                    bool r = !IO::yesNoBox( p_lang ).getResult( STRINGS[ 79 ][ p_lang ] );
                    IO::clearScreen( true, false, false );
                    drawSlotChoice( p_lang, selectedIdx );
                    fillResume( );
                    if( r ) continue;
                } else if( !SAV->m_saveFile[ selectedIdx ].m_gameType && !p_newGameMode )
                    continue;
                return selectedIdx;
            }
            for( u8 i = 0; i < MAX_SAVE_FILES; ++i )
                if( IN_RANGE_R( 4, 4 + 64 * i, 86, 26 + 64 * i ) ) {
                    selectedIdx = i;
                    drawSlotChoice( p_lang, selectedIdx, i );
                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP ) { return selectedIdx; }
                        if( !IN_RANGE_R( 4, 4 + 64 * i, 86, 26 + 64 * i ) ) break;
                    }
                    drawSlotChoice( p_lang, selectedIdx );
                }
        }
    }

    gameType startScreen::runEpisodeChoice( language p_current ) {
        IO::choiceBox cb
            = IO::choiceBox( MAX_SPECIAL_EPISODES, EPISODE_NAMES[ p_current ], 0, true );
        return ( gameType )( SPECIAL + cb.getResult( STRINGS[ 110 ][ p_current ], true, false ) );
    }

    language startScreen::runLanguageChoice( language p_current ) {
        return p_current;

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

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) hasSave |= !!( SAV->m_saveFile[ i ].m_gameType );

        if( !hasSave ) {
            memset( SAV->m_storedPokemon, 0, sizeof( SAV->m_storedPokemon ) );
            for( u8 i = 0; i < MAX_BOXES; ++i )
                sprintf( ( SAV->m_storedPokemon + i )->m_name, "Box %d", i + 1 );
        }

        SAV->m_activeFile = p_file;

        SAV->getActiveFile( ).m_curBox             = 0;
        SAV->getActiveFile( ).m_options.m_language = p_lang;

        SAV->getActiveFile( ).m_money = 3000;
        SAV->getActiveFile( ).m_id    = rand( ) % 65536;
        SAV->getActiveFile( ).m_sid   = rand( ) % 65536;

        SAV->getActiveFile( ).m_gameType        = p_type;
        SAV->getActiveFile( ).m_options.m_bgIdx = START_BG;

        SAV->getActiveFile( ).m_playtime     = 0;
        SAV->getActiveFile( ).m_HOENN_Badges = 0;
        SAV->getActiveFile( ).m_KANTO_Badges = 0;
        SAV->getActiveFile( ).m_JOHTO_Badges = 0;
        SAV->getActiveFile( ).m_lstBag       = 0;
        SAV->getActiveFile( ).m_lstBagItem   = 0;

        memset( SAVE::SAV->getActiveFile( ).m_pkmnTeam, 0,
                sizeof( SAVE::SAV->getActiveFile( ).m_pkmnTeam ) );

        return initSpecialEpisode( SAV->getActiveFile( ).m_gameType - (s8) SPECIAL );
    }

    void startScreen::run( ) {
        // Check if a valid save is present and use its language.
        // If none is present, use the DS's language.
        language cur;
        if( SAV->getActiveFile( ).m_gameType )
            cur = SAV->getActiveFile( ).m_options.m_language;
        else
            cur = translate( PersonalData->language );

        loop( ) {
            drawSplash( cur );
            // TODO: Add fade

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
                SAV->m_activeFile = slot;
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
