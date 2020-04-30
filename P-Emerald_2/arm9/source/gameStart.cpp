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
#include "defines.h"
#include "gen3Parser.h"
#include "saveGame.h"
#include "screenFade.h"
#include "startScreen.h"
#include "uio.h"
#include "yesNoBox.h"
#include "itemNames.h"

namespace SAVE {
    const u16 EP_INTRO_TEXT_START[] = {111};
    const u8  EP_INTRO_TEXT_LEN[]   = {3};

    void printTextAndWait( const char* p_text ) {
        IO::fadeScreen( IO::fadeType::CLEAR_DARK );
        IO::clearScreen( true, true, true );
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ COLOR_IDX ] = GRAY;
        u8 nw                   = 0;
        for( u8 i = 0; i < strlen( p_text ); ++i )
            if( p_text[ i ] == '\n' ) ++nw;
        IO::regularFont->printString( p_text, 128, 89 - 10 * nw, false, IO::font::CENTER );
        swiWaitForVBlank( );
        IO::fadeScreen( IO::fadeType::UNFADE );

        touchPosition touch;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_START )
                || GET_AND_WAIT_R( 1, 1, 256, 192 ) )
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
            SAV->getActiveFile( ).m_player
                = {MAP::mapObject::PLYR, {299, 53, 4}, 0, MAP::moveMode::WALK, 0, 0,
                   MAP::direction::RIGHT};
            SAV->getActiveFile( ).m_isMale = rand( ) % 2;
            SAV->getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_BIKE, 1 );
            SAV->getActiveFile( ).m_currentMap     = 10;
            SAV->getActiveFile( ).m_registeredItem = I_BIKE;
            return true;
        default:
            SAV->getActiveFile( ).m_gameType = UNUSED;
            return false;
        }
    }

    bool startScreen::transferGame( u8 p_slot ) {
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

        SAV->m_activeFile                = p_slot;
        SAV->getActiveFile( ).m_gameType = TRANSFER;

        char savname[ 8 ] = {0};
        for( int i = 0; i < 7; ++i ) savname[ i ] = FS::getNText( save3->unpackeddata[ i ] );
        strcpy( SAV->getActiveFile( ).m_playername, savname );

        SAV->getActiveFile( ).m_isMale = !save3->unpackeddata[ 8 ];

        SAV->getActiveFile( ).m_id = ( save3->unpackeddata[ 11 ] << 8 ) | save3->unpackeddata[ 10 ];
        SAV->getActiveFile( ).m_sid
            = ( save3->unpackeddata[ 13 ] << 8 ) | save3->unpackeddata[ 12 ];

        SAV->getActiveFile( ).m_pt.m_hours
            = ( save3->unpackeddata[ 15 ] << 8 ) | save3->unpackeddata[ 14 ];
        SAV->getActiveFile( ).m_pt.m_mins = save3->unpackeddata[ 16 ];
        SAV->getActiveFile( ).m_pt.m_secs = save3->unpackeddata[ 17 ];

        // SAV->getActiveFile( ).m_gba.m_gameid = ( save3->unpackeddata[ 0xaf ] << 24 ) | (
        // save3->unpackeddata[ 0xae ] << 16 ) | ( save3->unpackeddata[ 0xad ] << 8 ) |
        // save3->unpackeddata[ 0xac ];

        pokemonData p;
        for( u8 i = 0; i < 6; ++i ) {
            if( save3->pokemon[ i ]->personality ) {

                pokemon&             acPkmn  = SAV->getActiveFile( ).m_pkmnTeam[ i ];
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
                acPkmn.m_boxdata.m_markings = acBeltP->markint;

                acPkmn.m_statusint         = acBeltP->status;
                acPkmn.m_level             = acBeltP->level;
                acPkmn.m_boxdata.m_pokerus = acBeltP->pokerus;

                acPkmn.m_stats.m_acHP  = acBeltP->currentHP;
                acPkmn.m_stats.m_maxHP = acBeltP->maxHP;
                acPkmn.m_stats.m_Atk   = acBeltP->move;
                acPkmn.m_stats.m_Def   = acBeltP->defense;
                acPkmn.m_stats.m_SAtk  = acBeltP->spatk;
                acPkmn.m_stats.m_SDef  = acBeltP->spdef;
                acPkmn.m_stats.m_Spd   = acBeltP->speed;

                FS::gen3Pokemon::pokemon_growth_t*& acBG = save3->pokemon_growth[ i ];
                acPkmn.m_boxdata.m_speciesId             = FS::getNPKMNIdx( acBG->species );
                acPkmn.m_boxdata.m_holdItem              = FS::getNItemIdx( acBG->held );
                acPkmn.m_boxdata.m_experienceGained      = acBG->xp;
                acPkmn.m_boxdata.m_steps                 = acBG->happiness;
                acPkmn.m_boxdata.m_pPUps                 = acBG->ppbonuses;

                FS::gen3Pokemon::pokemon_moves_t*& acBA = save3->pokemon_moves[ i ];
                for( u8 j = 0; j < 4; ++j ) {
                    acPkmn.m_boxdata.m_moves[ j ] = acBA->atk[ j ];
                    acPkmn.m_boxdata.m_acPP[ j ]  = acBA->pp[ j ];
                }

                FS::gen3Pokemon::pokemon_effort_t*& acBE = save3->pokemon_effort[ i ];
                for( u8 j = 0; j < 6; ++j ) {
                    acPkmn.m_boxdata.m_effortValues[ j ] = acBE->EV[ j ];
                    acPkmn.m_boxdata.m_contestStats[ j ] = acBE->ConStat[ j ];
                }

                FS::gen3Pokemon::pokemon_misc_t*& acBM = save3->pokemon_misc[ i ];
                acPkmn.m_boxdata.m_iVint               = acBM->IVint;

                getAll( acPkmn.m_boxdata.m_speciesId, p );
                acPkmn.m_boxdata.m_ability
                    = p.m_abilities[ acPkmn.isEgg( ) ];
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

        SAV->getActiveFile( ).m_player     = {MAP::mapObject::PLYR,
                                          {104, 120, 5},
                                          SAV->getActiveFile( ).m_isMale ? (u16) 0 : (u16) 10,
                                          MAP::moveMode::WALK,
                                          0,
                                          0,
                                          MAP::direction::RIGHT};
        SAV->getActiveFile( ).m_isMale     = true;
        SAV->getActiveFile( ).m_currentMap = 10;

        return true;
    }
} // namespace SAVE
