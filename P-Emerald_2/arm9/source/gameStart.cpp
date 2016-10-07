/*
Pokémon Emerald 2 Version
------------------------------

file        : gameStart.cpp
author      : Philip Wellnitz
description :

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

#include "gameStart.h"
#include "saveGame.h"
#include "defines.h"
#include "startScreen.h"
#include "uio.h"
#include "yesNoBox.h"
#include "screenFade.h"

namespace SAVE {
    const u16 EP_INTRO_TEXT_START[ ] = { 111 };
    const u8 EP_INTRO_TEXT_LEN[ ] = { 3 };

    void printTextAndWait( const char* p_text ) {
        IO::fadeScreen( IO::fadeType::CLEAR_DARK );
        IO::clearScreen( true, true, true );
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ COLOR_IDX ] = GRAY;
        u8 nw = 0;
        for( u8 i = 0; i < strlen( p_text ); ++i )
            if( p_text[ i ] == '\n' )
                ++nw;
        IO::regularFont->printString( p_text, 128, 89 - 10 * nw, false, IO::font::CENTER );
        swiWaitForVBlank( );
        IO::fadeScreen( IO::fadeType::UNFADE );

        touchPosition touch;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START ) || GET_AND_WAIT_R( 1, 1, 256, 192 ) )
                break;
        }
    }

    void printEpisodeInfo( s8 p_episode ) {
        IO::regularFont->setColor( COLOR_IDX, 2 );
        IO::regularFont->setColor( WHITE_IDX, 1 );

        for( u8 i = 0; i < EP_INTRO_TEXT_LEN[ p_episode ]; ++i )
            printTextAndWait( GET_STRING( EP_INTRO_TEXT_START[ p_episode ] + i ) );
    }

    bool initSpecialEpisode( s8 p_episode ) {
        if( p_episode < 0 ) { // New game
            SAV->getActiveFile( ).m_gameType = UNUSED;
            return false;
        }

        printEpisodeInfo( p_episode );

        switch( p_episode ) {
            case 0:
                strcpy( SAV->getActiveFile( ).m_playername, "Test" );
                SAVE::SAV->getActiveFile( ).m_player = { MAP::mapObject::PLYR,{ 104, 120, 5 }, 0, MAP::moveMode::WALK, 0, 0, MAP::direction::RIGHT };
                SAVE::SAV->getActiveFile( ).m_isMale = true;
                SAVE::SAV->getActiveFile( ).m_currentMap = 10;
                return true;
            default:
                SAV->getActiveFile( ).m_gameType = UNUSED;
                return false;
        }
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
}