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

#include "startScreen.h"
#include "defines.h"
#include "uio.h"
#include "screenFade.h"
#include "fs.h"
#include "mapDrawer.h"
#include "yesNoBox.h"
#include "messageBox.h"
#include "Gen.h"

void fillResume( ) {
    IO::regularFont->setColor( 0, 0 );
    IO::regularFont->setColor( 251, 1 );
    IO::regularFont->setColor( 252, 2 );

    BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
    if( FS::SAV->m_isMale )
        BG_PALETTE_SUB[ 252 ] = RGB15( 0, 0, 31 );
    else
        BG_PALETTE_SUB[ 252 ] = RGB15( 31, 0, 0 );

    sprintf( buffer, "%ls", FS::SAV->m_playername );
    IO::regularFont->printString( buffer, 128, 5, true );

    sprintf( buffer, "%s", FS::getLoc( MAP::curMap->getCurrentLocationId( ) ) );
    IO::regularFont->printString( "Ort:", 16, 23, true );
    IO::regularFont->printString( buffer, 128, 23, true );


    sprintf( buffer, "%d:%02d", FS::SAV->m_pt.m_hours, FS::SAV->m_pt.m_mins );
    IO::regularFont->printString( "Spielzeit:", 16, 37, true );
    IO::regularFont->printString( buffer, 128, 37, true );

    sprintf( buffer, "??" );
    IO::regularFont->printString( "Orden:", 16, 51, true );
    IO::regularFont->printString( buffer, 128, 51, true );

    sprintf( buffer, "??" );
    IO::regularFont->printString( "PokéDex:", 16, 65, true );
    IO::regularFont->printString( buffer, 128, 65, true );
}

void drawSplash( ) {
    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Title" );
    if( IO::BGs[ FS::SAV->m_bgIdx ].m_allowsOverlay )
        IO::drawSub( );
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
    touchPosition tp;
    loop( ) {
        scanKeys( );
        tp = touchReadXY( );
        swiWaitForVBlank( );

        int pressed = keysCurrent( );
        if( ( pressed & KEY_A ) || ( pressed & KEY_START ) || ( tp.px || tp.py ) )
            break;
        ++frame;
        if( !( frame % 120 ) ) {
            printf( "     BER\x9A""HRE, UM ZU STARTEN" );
            frame = 0;
        } else if( ( frame % 120 ) == 60 )
            consoleClear( );
    }
    while( tp.px || tp.py ) {
        scanKeys( );
        tp = touchReadXY( );
        swiWaitForVBlank( );
    }

    IO::clearScreenConsole( true, true );
    consoleSelect( &IO::Top );
}

void initNewGame( ) {
    FS::SAV = new FS::saveGame( );
    wcscpy( FS::SAV->m_playername, L"Test" );
    FS::SAV->m_activatedPNav = false;
    FS::SAV->m_money = 3000;
    FS::SAV->m_id = rand( ) % 65536;
    FS::SAV->m_sid = rand( ) % 65536;

    FS::SAV->m_savTyp = 1;
    FS::SAV->m_playtime = 0;
    FS::SAV->m_HOENN_Badges = 0;
    FS::SAV->m_KANTO_Badges = 0;
    FS::SAV->m_JOHTO_Badges = 0;
    FS::SAV->m_bgIdx = START_BG;
    FS::SAV->m_lstBag = 0;
    FS::SAV->m_lstBagItem = 0;

    memset( FS::SAV->m_pkmnTeam, 0, sizeof( FS::SAV->m_pkmnTeam ) );
    memset( FS::SAV->m_storedPokemon, 0, sizeof( FS::SAV->m_storedPokemon ) );
    for( u8 i = 0; i < MAX_BOXES; ++i )
        sprintf( ( FS::SAV->m_storedPokemon + i )->m_name, "Box %d", i + 1 );
    FS::SAV->m_curBox = 0;

    FS::SAV->m_player = { MAP::mapObject::PLYR, { 104, 120, 5 }, 0, MAP::moveMode::WALK, 0, 0, MAP::direction::RIGHT };
    FS::SAV->m_isMale = true;
    FS::SAV->m_currentMap = 10;
}

bool transferGame( ) {
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
    if( acgame == -1 )
        return false;

    IO::drawSub( );
    IO::yesNoBox yn = IO::yesNoBox( );
    if( yn.getResult( "Möchtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?" ) ) {
        IO::messageBox( "Solltest du im Folgenden\nspeichern, so werden Daten\nauf das GBA-Modul geschrieben." );
        IO::messageBox( "Bitte entferne daher das\nGBA-Modul nicht, es könnte\nden Spielstand beschädigen." );
        IO::messageBox( "Auch das Speichern an sich\nkann den Spielstand\nbeschädigen." );
        yn = IO::yesNoBox( );
        if( yn.getResult( "Möchtest du fortfahren?" ) ) {
            IO::messageBox( "Lade Spielstand..." );
            //int loadgame = acgame > 2 ? 1 : 0;

            // gen3::SaveParser* save3 = gen3::SaveParser::Instance( );

            //   if( save3->load( loadgame ) == -1 ) {
            IO::messageBox( "Ein Fehler ist aufgetreten.\nKehre zum Hauptmenü zurück." );
            return false;
            //   }

            FS::SAV = new FS::saveGame( );
            wchar_t savname[ 8 ] = { 0 };
            // for( int i = 0; i < 7; ++i )
            //     savname[ i ] = gen3::getNText( save3->unpackeddata[ i ] );
            wcscpy( FS::SAV->m_playername, savname );

            //  FS::SAV->m_isMale = !save3->unpackeddata[ 8 ];

            //FS::SAV->m_id = ( save3->unpackeddata[ 11 ] << 8 ) | save3->unpackeddata[ 10 ];
            //FS::SAV->m_sid = ( save3->unpackeddata[ 13 ] << 8 ) | save3->unpackeddata[ 12 ];

            //FS::SAV->m_pt.m_hours = ( save3->unpackeddata[ 15 ] << 8 ) | save3->unpackeddata[ 14 ];
            //FS::SAV->m_pt.m_mins = save3->unpackeddata[ 16 ];
            //FS::SAV->m_pt.m_secs = save3->unpackeddata[ 17 ];

            // FS::SAV->m_gba.m_gameid = ( save3->unpackeddata[ 0xaf ] << 24 ) | ( save3->unpackeddata[ 0xae ] << 16 ) | ( save3->unpackeddata[ 0xad ] << 8 ) | save3->unpackeddata[ 0xac ];

            /*    pokemonData p;
                for( u8 i = 0; i < 6; ++i ) {
                if( save3->pokemon[ i ]->personality ) {

                pokemon &acPkmn = FS::SAV->m_pkmnTeam[ i ];
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

                //  FS::SAV->m_overWorldIdx = 20 * ( ( acgame + 1 ) / 2 ) + ( FS::SAV->m_isMale ? 0 : 10 );

            IO::Oam->oamBuffer[ SAVE_ID ].isHidden = false;

            IO::messageBox( "Abgeschlossen." );
        }
    }
    return true;
}

startScreen::ChoiceResult startScreen::runChoice( ) {
    consoleSelect( &IO::Top );
    consoleClear( );

    ChoiceResult results[ 5 ] = {
        CONTINUE,
        NEW_GAME,
        GEHEIMGESCHEHEN,
        OPTIONS,
        TRANSFER_GAME,
    };
    u16 MaxVal;
    std::pair<u8, u8> ranges[ 5 ] = {
        std::pair<u8, u8>( 1, 84 ),
        std::pair<u8, u8>( 87, 108 ),
        std::pair<u8, u8>( 113, 134 ),
        std::pair<u8, u8>( 139, 160 ),
        std::pair<u8, u8>( 165, 186 )
    };

    switch( FS::SAV->m_savTyp ) {
        case 3:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu0", 512, 49152, true );
            MaxVal = 5;
            fillResume( );
            break;
        }
        case 2:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu1", 512, 49152, true );
            MaxVal = 4;
            fillResume( );
            break;
        }
        case 1:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu2", 512, 49152, true );
            MaxVal = 3;
            results[ 2 ] = OPTIONS;

            fillResume( );
            break;
        }
        case 0:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu3", 512, 49152, true );

            MaxVal = 2;
            ranges[ 0 ] = std::pair<int, int>( 0, 20 );
            ranges[ 1 ] = std::pair<int, int>( 25, 40 );
            results[ 0 ] = NEW_GAME;
            results[ 1 ] = OPTIONS;

            break;
        }
        default:
        {
            IO::clearScreenConsole( true, true );
            return CANCEL;
        }
    }


    touchPosition touch;
    consoleSelect( &IO::Bottom );
    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
    loop( ) {
        swiWaitForVBlank( );
        scanKeys( );
        touch = touchReadXY( );
        u32 p = keysUp( );
        u32 k = keysHeld( ) | keysDown( );
        if( ( FS::SAV->m_savTyp == 1 ) && ( k & KEY_SELECT ) && ( k & KEY_RIGHT ) && ( k & KEY_L ) && ( k & KEY_R ) ) {
            IO::clearScreenConsole( true, true );
            ++FS::SAV->m_savTyp;
            return REDRAW;
        } else if( ( gMod == DEVELOPER ) && ( FS::SAV->m_savTyp == 2 ) && ( k & KEY_START ) && ( k & KEY_LEFT ) && ( k & KEY_L ) && ( k & KEY_R ) ) {
            IO::clearScreenConsole( true, true );
            ++FS::SAV->m_savTyp;
            return REDRAW;
        } else if( p & KEY_B ) {
            IO::clearScreenConsole( true, true );
            IO::clearScreen( true );
            for( u16 i = 1; i < 256; ++i )
                BG_PALETTE_SUB[ i ] = RGB15( 31, 31, 31 );
            return CANCEL;
        }
        for( u16 i = 0; i < MaxVal; i++ )
            if( GET_AND_WAIT_R( u8( 1 ), ranges[ i ].first, u8( 255 ), ranges[ i ].second ) ) {
                IO::clearScreenConsole( true, true );
                IO::clearScreen( true );
                for( u16 j = 1; j < 256; ++j )
                    BG_PALETTE_SUB[ j ] = RGB15( 31, 31, 31 );

                return results[ i ];
            }
    }
}

void startScreen::run( ) {
    loop( ) {
        drawSplash( );
        ChoiceResult res;
        while( ( res = runChoice( ) ) == REDRAW ) {
        }
        switch( res ) {
            return;
            case startScreen::NEW_GAME:
                initNewGame( );
            case startScreen::CONTINUE:
                IO::clearScreenConsole( true, true );
                IO::clearScreen( false, false, false );
                return;
            case startScreen::GEHEIMGESCHEHEN:
                break;
            case startScreen::TRANSFER_GAME:
                if( !transferGame( ) )
                    break;
                IO::clearScreenConsole( true, true );
                IO::clearScreen( false, false, false );
                return;
            case startScreen::CANCEL:
            default:
                break;
        }
    }
}