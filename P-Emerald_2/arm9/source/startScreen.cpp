/*
Pokémon Emerald 2 Version
------------------------------

file        : startScreen.cpp
author      : Philip Wellnitz
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2016
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

#include "startScreen.h"
#include "defines.h"
#include "uio.h"
#include "nav.h"
#include "screenFade.h"
#include "fs.h"
#include "mapDrawer.h"
#include "yesNoBox.h"
#include "messageBox.h"
#include "Gen.h"

namespace SAVE {
    void initColors( ) {
        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        BG_PALETTE_SUB[ GRAY_IDX ] = NORMAL_COLOR;
        BG_PALETTE_SUB[ RED_IDX ] = GREEN;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
    }

    void fillResume( ) {
        initColors( );
        if( SAVE::SAV->getActiveFile( ).m_isMale )
            BG_PALETTE_SUB[ BLACK_IDX ] = BLUE;
        else
            BG_PALETTE_SUB[ BLACK_IDX ] = RED;

        sprintf( buffer, "%s", SAVE::SAV->getActiveFile( ).m_playername );
        IO::regularFont->printString( buffer, 128, 5, true );

        sprintf( buffer, "%s", FS::getLocation( MAP::curMap->getCurrentLocationId( ) ) );
        IO::regularFont->printString( "Ort:", 16, 23, true );
        IO::regularFont->printString( buffer, 128, 23, true );


        sprintf( buffer, "%d:%02d", SAVE::SAV->getActiveFile( ).m_pt.m_hours, SAVE::SAV->getActiveFile( ).m_pt.m_mins );
        IO::regularFont->printString( "Spielzeit:", 16, 37, true );
        IO::regularFont->printString( buffer, 128, 37, true );

        sprintf( buffer, "??" );
        IO::regularFont->printString( "Orden:", 16, 51, true );
        IO::regularFont->printString( buffer, 128, 51, true );

        sprintf( buffer, "??" );
        IO::regularFont->printString( "PokéDex:", 16, 65, true );
        IO::regularFont->printString( buffer, 128, 65, true );
    }

    language translate( u8 p_dSRegion ) {
        switch( p_dSRegion ) {
            case 1:
                return SAVE::EN;
            case 3:
                return SAVE::GER;

            default:
                return SAVE::EN;
        }
    }

    void drawSplash( language p_lang ) {
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Title" );
        IO::clearScreen( true, false, false );

        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );

        BG_PALETTE[ 3 ] = BG_PALETTE_SUB[ 3 ] = RGB15( 0, 0, 0 );

        printf( "@ Philip Wellnitz 2012 - 2016\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );

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
        u8 frame = 0;
        touchPosition touch;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START ) || GET_AND_WAIT_R( 1, 1, 256, 192 ) )
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

    void startScreen::drawMainChoice( language p_lang, std::vector<u8> p_toDraw, s8 p_selected, s8 p_pressed ) {
        initColors( );
        for( u8 i = 0; i < p_toDraw.size( ); ++i ) {
            IO::printChoiceBox( 4, 4 + 48 * i, 136, 26 + 48 * i, 6, ( i == p_selected ) ? RED_IDX : COLOR_IDX, i == p_pressed );
            IO::regularFont->printString( STRINGS[ p_toDraw[ i ] ][ p_lang ], 70 + 2 * ( i == p_pressed ),
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
            IO::boldFont->printString( STRINGS[ p_toDraw[ i ] + 4 ][ p_lang ], 248, 28 + 48 * i, true, IO::font::RIGHT );
        }
    }

    void startScreen::drawSlotChoice( s8 p_selected, s8 p_pressed ) {
        initColors( );
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            IO::printChoiceBox( 4, 4 + 64 * i, 136, 26 + 64 * i, 6, ( i == p_selected ) ? RED_IDX : COLOR_IDX, i == p_pressed );
            switch( SAV->m_saveFile[ i ].m_gameType ) {
                case UNUSED:
                    IO::regularFont->printString( STRINGS[ 82 ][ SAV->m_saveFile[ i ].m_options.m_language ], 70 + 2 * ( i == p_pressed ),
                                                  8 + 64 * i + ( i == p_pressed ), true, IO::font::CENTER );
                    break;
                case NORMAL:
                    IO::regularFont->printString( CHAPTER_NAMES[ 2 * SAV->m_saveFile[ i ].m_chapter ][ SAV->m_saveFile[ i ].m_options.m_language ], 70 + 2 * ( i == p_pressed ),
                                                  8 + 64 * i + ( i == p_pressed ), true, IO::font::CENTER );
                    IO::regularFont->printString( CHAPTER_NAMES[ 2 * SAV->m_saveFile[ i ].m_chapter + 1 ][ SAV->m_saveFile[ i ].m_options.m_language ], 248,
                                                  8 + 68 * i, true, IO::font::RIGHT );
                    break;
                case TRANSFER:
                    IO::regularFont->printString( STRINGS[ 83 ][ SAV->m_saveFile[ i ].m_options.m_language ], 70 + 2 * ( i == p_pressed ),
                                                  8 + 64 * i + ( i == p_pressed ), true, IO::font::CENTER );
                    break;
                default:
                    IO::regularFont->printString( STRINGS[ 84 ][ SAV->m_saveFile[ i ].m_options.m_language ],
                                                  70 + 2 * ( i == p_pressed ), 8 + 64 * i + ( i == p_pressed ), true, IO::font::CENTER );
                    IO::regularFont->printString( EPISODE_NAMES[ SAV->m_saveFile[ i ].m_gameType - SPECIAL ][ SAV->m_saveFile[ i ].m_options.m_language ],
                                                  248, 8 + 68 * i, true, IO::font::RIGHT );
            }
        }

    }

    startScreen::choiceType startScreen::runMainChoice( language p_lang ) {
        std::vector<u8> vis;
        std::vector<choiceType> res;
        bool hasSave = false;

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i )
            hasSave |= !!( SAV->m_saveFile[ i ].m_gameType );

        if( hasSave ) {
            vis.push_back( 71 );
            res.push_back( CONTINUE );
        }
        vis.push_back( 72 );
        res.push_back( NEW_GAME );
        vis.push_back( 73 );
        res.push_back( SPECIAL_EPISODE );
        if( SAV->m_transfersRemaining ) {
            vis.push_back( 74 );
            res.push_back( TRANSFER_GAME );
        }

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
                for( u16 i = 1; i < 256; ++i )
                    BG_PALETTE_SUB[ i ] = RGB15( 31, 31, 31 );
                return ABORT;
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                selectedIdx = ( selectedIdx + 1 ) % vis.size( );
                drawMainChoice( p_lang, vis, selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                selectedIdx = ( selectedIdx + vis.size( ) - 1 ) % vis.size( );
                drawMainChoice( p_lang, vis, selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) )
                return res[ selectedIdx ];
        }
    }

    s8 startScreen::runSlotChoice( language p_lang, bool p_newGameMode ) {
        u8 selectedIdx = 0;
        drawSlotChoice( 0 );

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
                for( u16 i = 1; i < 256; ++i )
                    BG_PALETTE_SUB[ i ] = RGB15( 31, 31, 31 );
                return -1;
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                selectedIdx = ( selectedIdx + 1 ) % MAX_SAVE_FILES;
                drawSlotChoice( selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                selectedIdx = ( selectedIdx + MAX_SAVE_FILES - 1 ) % MAX_SAVE_FILES;
                drawSlotChoice( selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) ) {
                if( SAV->m_saveFile[ selectedIdx ].m_gameType && p_newGameMode ) {
                    bool r = !IO::yesNoBox( p_lang ).getResult( GET_STRING( 79 ) );
                    drawSlotChoice( selectedIdx );
                    if( r ) continue;
                } else if( !SAV->m_saveFile[ selectedIdx ].m_gameType && !p_newGameMode )
                    continue;
                return selectedIdx;
            }
        }
    }

    gameType startScreen::runEpisodeChoice( ) {
        return (gameType) 0;
    }

    language startScreen::runLanguageChoice( language p_current ) {
        IO::clearScreen( true, false, false );
        while( IO::yesNoBox( p_current ).getResult( STRINGS[ 85 ][ p_current ] ) ) {
            IO::clearScreen( true, false, false );

        }
        return p_current;
    }

    bool startScreen::initNewGame( u8 p_file, gameType p_type, language p_lang ) {
        // Check if this is the first save file
        bool hasSave = false;

        for( u8 i = 0; i < MAX_SAVE_FILES; ++i )
            hasSave |= !!( SAV->m_saveFile[ i ].m_gameType );

        if( !hasSave ) {
            memset( SAV->m_storedPokemon, 0, sizeof( SAV->m_storedPokemon ) );
            for( u8 i = 0; i < MAX_BOXES; ++i )
                sprintf( ( SAV->m_storedPokemon + i )->m_name, "Box %d", i + 1 );
        }

        SAV->m_activeFile = p_file;

        SAV->getActiveFile( ).m_curBox = 0;
        SAV->getActiveFile( ).m_options.m_language = p_lang;

        SAV->getActiveFile( ).m_activatedPNav = false;
        SAV->getActiveFile( ).m_money = 3000;
        SAV->getActiveFile( ).m_id = rand( ) % 65536;
        SAV->getActiveFile( ).m_sid = rand( ) % 65536;

        SAV->getActiveFile( ).m_gameType = p_type;
        SAV->getActiveFile( ).m_options.m_bgIdx = START_BG;

        SAV->getActiveFile( ).m_playtime = 0;
        SAV->getActiveFile( ).m_HOENN_Badges = 0;
        SAV->getActiveFile( ).m_KANTO_Badges = 0;
        SAV->getActiveFile( ).m_JOHTO_Badges = 0;
        SAV->getActiveFile( ).m_lstBag = 0;
        SAV->getActiveFile( ).m_lstBagItem = 0;


        memset( SAVE::SAV->getActiveFile( ).m_pkmnTeam, 0, sizeof( SAVE::SAV->getActiveFile( ).m_pkmnTeam ) );


        strcpy( SAV->getActiveFile( ).m_playername, "Test" );
        SAVE::SAV->getActiveFile( ).m_player = { MAP::mapObject::PLYR, { 104, 120, 5 }, 0, MAP::moveMode::WALK, 0, 0, MAP::direction::RIGHT };
        SAVE::SAV->getActiveFile( ).m_isMale = true;
        SAVE::SAV->getActiveFile( ).m_currentMap = 10;

        return true;
    }

    bool startScreen::transferGame( u8 p_slot ) {
        char acSlot2Game[ 5 ];

        sysSetBusOwners( true, true );
        memcpy( acSlot2Game, (char*) 0x080000AC, 4 );

        char cmpgm[ 5 ][ 4 ] = { "BPE", "AXP", "AXV", "BPR", "BPG" };
        s8 acgame = -1;

        for( u8 i = 0; i < 5; ++i )
            if( !strcmp( cmpgm[ i ], acSlot2Game ) ) {
                acgame = i;
                break;
            }
        if( acgame == -1 ) {
            return false;
        }

        IO::NAV->draw( );
        IO::yesNoBox yn = IO::yesNoBox( );
        if( yn.getResult( "Möchtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?" ) ) {
            // TODO: Add some messages here informing the player about what's going on
            yn = IO::yesNoBox( );
            if( yn.getResult( "Möchtest du fortfahren?" ) ) {
                IO::messageBox( "Lade Spielstand…" );
                //int loadgame = acgame > 2 ? 1 : 0;

                // gen3::SaveParser* save3 = gen3::SaveParser::Instance( );

                //   if( save3->load( loadgame ) == -1 ) {
                IO::messageBox( "Ein Fehler ist aufgetreten.\nKehre zum Hauptmenü zurück." );
                return false;
                //   }

                SAV->m_activeFile = p_slot;
                SAV->getActiveFile( ).m_gameType = TRANSFER;

                char savname[ 8 ] = { 0 };
                // for( int i = 0; i < 7; ++i )
                //     savname[ i ] = gen3::getNText( save3->unpackeddata[ i ] );
                strcpy( SAVE::SAV->getActiveFile( ).m_playername, savname );

                //  SAVE::SAV->getActiveFile( ).m_isMale = !save3->unpackeddata[ 8 ];

                //SAVE::SAV->getActiveFile( ).m_id = ( save3->unpackeddata[ 11 ] << 8 ) | save3->unpackeddata[ 10 ];
                //SAVE::SAV->getActiveFile( ).m_sid = ( save3->unpackeddata[ 13 ] << 8 ) | save3->unpackeddata[ 12 ];

                //SAVE::SAV->getActiveFile( ).m_pt.m_hours = ( save3->unpackeddata[ 15 ] << 8 ) | save3->unpackeddata[ 14 ];
                //SAVE::SAV->getActiveFile( ).m_pt.m_mins = save3->unpackeddata[ 16 ];
                //SAVE::SAV->getActiveFile( ).m_pt.m_secs = save3->unpackeddata[ 17 ];

                // SAVE::SAV->getActiveFile( ).m_gba.m_gameid = ( save3->unpackeddata[ 0xaf ] << 24 ) | ( save3->unpackeddata[ 0xae ] << 16 ) | ( save3->unpackeddata[ 0xad ] << 8 ) | save3->unpackeddata[ 0xac ];

                /*    pokemonData p;
                    for( u8 i = 0; i < 6; ++i ) {
                    if( save3->pokemon[ i ]->personality ) {

                    pokemon &acPkmn = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ];
                    gen3::belt_pokemon_t* &acBeltP = save3->pokemon[ i ];


                    acPkmn.m_boxdata.m_pid = acBeltP->personality;
                    acPkmn.m_boxdata.m_oTSid = acBeltP->otid >> 16;
                    acPkmn.m_boxdata.m_oTId = acBeltP->otid % ( 1 << 16 );
                    for( int i = 0; i < 10; ++i )
                    acPkmn.m_boxdata.m_name[ i ] = gen3::getNText( acBeltP->name[ i ] );
                    acPkmn.m_boxdata.m_name[ 10 ] = 0;
                    acPkmn.m_boxdata.m_hometown = acBeltP->language;
                    for( int i = 0; i < 7; ++i )
                    acPkmn.m_boxdata.m_oT[ i ] = gen3::getNText( acBeltP->otname[ i ] );
                    acPkmn.m_boxdata.m_oT[ 7 ] = 0;
                    acPkmn.m_boxdata.m_markings = acBeltP->markint;

                    acPkmn.m_statusint = acBeltP->status;
                    acPkmn.m_level = acBeltP->level;
                    acPkmn.m_boxdata.m_pokerus = acBeltP->pokerus;

                    acPkmn.m_stats.m_acHP = acBeltP->currentHP;
                    acPkmn.m_stats.m_maxHP = acBeltP->maxHP;
                    acPkmn.m_stats.m_Atk = acBeltP->move;
                    acPkmn.m_stats.m_Def = acBeltP->defense;
                    acPkmn.m_stats.m_SAtk = acBeltP->spatk;
                    acPkmn.m_stats.m_SDef = acBeltP->spdef;
                    acPkmn.m_stats.m_Spd = acBeltP->speed;

                    gen3::pokemon::pokemon_growth_t* &acBG = save3->pokemon_growth[ i ];
                    acPkmn.m_boxdata.m_speciesId = gen3::getNPKMNIdx( acBG->species );
                    acPkmn.m_boxdata.m_holdItem = gen3::getNItemIdx( acBG->held );
                    acPkmn.m_boxdata.m_experienceGained = acBG->xp;
                    acPkmn.m_boxdata.m_steps = acBG->happiness;
                    acPkmn.m_boxdata.m_pPUps = acBG->ppbonuses;

                    gen3::pokemon::pokemon_moves_t* &acBA = save3->pokemon_moves[ i ];
                    for( int i = 0; i < 4; ++i ) {
                    acPkmn.m_boxdata.m_moves[ i ] = acBA->atk[ i ];
                    acPkmn.m_boxdata.m_acPP[ i ] = acBA->pp[ i ];
                    }

                    gen3::pokemon::pokemon_effort_t* &acBE = save3->pokemon_effort[ i ];
                    for( int i = 0; i < 6; ++i ) {
                    acPkmn.m_boxdata.m_effortValues[ i ] = acBE->EV[ i ];
                    acPkmn.m_boxdata.m_contestStats[ i ] = acBE->ConStat[ i ];
                    }

                    gen3::pokemon::pokemon_misc_t* &acBM = save3->pokemon_misc[ i ];
                    acPkmn.m_boxdata.m_iVint = acBM->IVint;

                    getAll( acPkmn.m_boxdata.m_speciesId, p );
                    acPkmn.m_boxdata.m_ability = p.m_abilities[ acPkmn.m_boxdata.m_individualValues.m_isEgg ];
                    acPkmn.m_boxdata.m_individualValues.m_isEgg = acPkmn.m_boxdata.m_individualValues.m_isNicked;
                    acPkmn.m_boxdata.m_gotPlace = gen3::getNLocation( acBM->locationcaught );

                    acPkmn.m_boxdata.m_gotLevel = acBM->levelcaught;

                    if( acPkmn.m_boxdata.m_individualValues.m_isEgg || acPkmn.m_boxdata.m_gotLevel ) {
                    acPkmn.m_boxdata.m_hatchPlace = 999;
                    acPkmn.m_boxdata.m_gotLevel = 5;
                    acPkmn.m_boxdata.m_hatchDate[ 0 ] =
                    acPkmn.m_boxdata.m_hatchDate[ 1 ] =
                    acPkmn.m_boxdata.m_hatchDate[ 2 ] = 0;
                    acPkmn.m_boxdata.m_gotDate[ 0 ] =
                    acPkmn.m_boxdata.m_gotDate[ 1 ] =
                    acPkmn.m_boxdata.m_gotDate[ 2 ] = 1;
                    }
                    acPkmn.m_boxdata.m_oTisFemale = acBM->tgender;
                    acPkmn.m_boxdata.m_ball = acBM->pokeball;
                    acPkmn.m_boxdata.m_gotDate[ 0 ] =
                    acPkmn.m_boxdata.m_gotDate[ 1 ] =
                    acPkmn.m_boxdata.m_gotDate[ 2 ] = 0;

                    }
                    }*/
                    //savMod = _GBA;

                    //  SAVE::SAV->getActiveFile( ).m_overWorldIdx = 20 * ( ( acgame + 1 ) / 2 ) + ( SAVE::SAV->getActiveFile( ).m_isMale ? 0 : 10 );

                IO::Oam->oamBuffer[ SAVE_ID ].isHidden = false;

                IO::messageBox( "Abgeschlossen." );
            }
        }
        return true;
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

            if( res == ABORT )
                continue;

            IO::clearScreen( true, false, false );
            s8 slot = runSlotChoice( cur, res == NEW_GAME );

            if( slot == (s8) -1 )
                continue;

            switch( res ) {
                case CONTINUE:
                    SAV->m_activeFile = slot;
                    return;
                case NEW_GAME:
                    if( !initNewGame( slot, NORMAL, runLanguageChoice( cur ) ) )
                        continue;
                    return;
                case TRANSFER_GAME:
                    if( !transferGame( slot ) )
                        continue;
                    return;
                case SPECIAL_EPISODE:
                {
                    gameType ep = runEpisodeChoice( );
                    if( ep < SPECIAL )
                        continue;
                    if( !initNewGame( slot, ep, runLanguageChoice( cur ) ) )
                        continue;
                    return;
                }
                default:
                    continue;
            }
        }
    }
}
