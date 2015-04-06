/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : main.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Main ARM9 entry point

    Copyright (C) 2012 - 2015
    Philip Wellnitz (RedArceus)

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

#include <memory>

#include <nds.h>
#include <fat.h>
#include "nitrofs.h"

#include "libnds_internal.h"

#include <cstdio>
#include <stdlib.h>
#include <dirent.h>

#include <fcntl.h>
#include <unistd.h>

#include "defines.h"

#include "map2d.h"
#include "map2devents.h"
#include "hmMoves.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>

#include "messageBox.h"
#include "yesNoBox.h"
#include "choiceBox.h"
#include "item.h"
#include "berry.h"
#include "uio.h"
#include "battle.h"
#include "statusScreen.h"
#include "statusScreenUI.h"

#include "pokemon.h"
#include "saveGame.h"
#include "keyboard.h"
#include "sprite.h"
#include "fs.h"
#include "dex.h"
#include "dexUI.h"

#include "Gen.h"

#include "BigCirc1.h" 
#include "consoleFont.h"

#include "Back.h"
#include "Save.h"
#include "Option.h"
#include "PokemonSp.h"
#include "Id.h"
#include "SPBag.h"
#include "Nav.h"
#include "PokeDex.h"

#undef _EMULATOR

enum GameMod {
    DEVELOPER,
    ALPHA,
    BETA,
    RELEASE,
    EMULATOR
};
#ifndef _EMULATOR
GameMod gMod = DEVELOPER;
#else
GameMod gMod = EMULATOR;
#endif

void debug( ) {

    //consoleDemoInit( );  //setup the sub screen for printing

    printf( "\n\n\tHello DS dev'rs\n" );
    printf( "\twww.drunkencoders.com\n" );
    printf( "\twww.devkitpro.org" );

    loop( );
}

std::string CodeName = "Executing Exeggcute";
std::string Version = "v0.5";

char acSlot2Game[ 5 ];

/*

#define MAXMAPPOS 75
struct MapRegionPos {
u8 m_lx,
m_ly,
m_rx,
m_ry;
u16 m_ind;
};
enum Region {
NONE = 0,
HOENN = 1,
KANTO = 2,
JOHTO = 3
};

Region acMapRegion = NONE;
const MapRegionPos MapLocations[ 3 ][ MAXMAPPOS ] = {
{
{ 90, 66, 96, 73, 1010 },
{ 115, 139, 121, 146, 308 },
{ 57, 36, 66, 47, 239 },
{ 46, 53, 51, 59, 298 },
{ 37, 73, 47, 89, 245 },
{ 39, 98, 45, 104, 294 },
{ 45, 104, 53, 112, 242 },
{ 47, 136, 54, 142, 291 },
{ 50, 136, 59, 148, 237 },
{ 64, 103, 72, 111, 236 },
{ 64, 117, 72, 125, 235 },
{ 64, 80, 72, 88, 240 },
{ 65, 75, 71, 80, 295 },
{ 69, 59, 78, 67, 238 },
{ 77, 47, 85, 60, 292 },
{ 79, 60, 84, 66, 312 },
{ 89, 80, 105, 96, 244 },
{ 89, 109, 98, 126, 243 },
{ 114, 36, 123, 46, 246 },
{ 140, 52, 149, 62, 202 },
{ 142, 71, 147, 77, 300 },
{ 121, 94, 130, 104, 1000 },
{ 146, 109, 154, 118, 241 },
{ 168, 112, 174, 116, 306 },
{ 161, 112, 167, 116, 320 },
{ 110, 112, 116, 116, 313 },
{ 179, 122, 187, 131, 293 },
{ 210, 95, 219, 111, 250 },
{ 173, 88, 180, 97, 249 },
{ 192, 74, 206, 82, 248 },
{ 200, 60, 204, 66, 302 },
{ 153, 59, 168, 67, 247 },
{ 93, 111, 115, 117, 285 },
{ 114, 111, 130, 117, 284 },
{ 129, 111, 150, 117, 283 },
{ 149, 111, 174, 117, 282 },
{ 173, 111, 191, 117, 281 },
{ 190, 111, 205, 117, 280 },
{ 187, 104, 212, 112, 279 },
{ 187, 80, 205, 105, 278 },
{ 168, 80, 188, 101, 277 },
{ 168, 60, 189, 81, 275 },
{ 190, 60, 205, 76, 276 },
{ 128, 60, 153, 66, 272 },
{ 142, 65, 147, 83, 273 },
{ 115, 82, 147, 88, 274 },
{ 122, 87, 128, 95, 1001 },
{ 103, 82, 115, 88, 269 },
{ 109, 39, 114, 83, 270 },
{ 122, 39, 128, 66, 271 },
{ 90, 39, 96, 83, 262 },
{ 76, 60, 91, 66, 263 },
{ 62, 37, 96, 44, 264 },
{ 39, 54, 45, 75, 266 },
{ 46, 39, 51, 53, 265 },
{ 46, 39, 60, 44, 265 },
{ 45, 75, 66, 81, 267 },
{ 39, 90, 45, 109, 254 },
{ 39, 108, 45, 133, 255 },
{ 39, 132, 57, 139, 256 },
{ 57, 140, 70, 146, 258 },
{ 69, 140, 91, 146, 259 },
{ 90, 122, 96, 146, 260 },
{ 90, 85, 96, 116, 261 },
{ 69, 82, 94, 88, 268 },
{ 50, 103, 66, 110, 252 },
{ 64, 109, 70, 119, 251 },
{ 65, 96, 91, 103, 253 }
}
};
void printMapLocation( const touchPosition& p_t ) {
consoleSetWindow( &Bottom, 5, 0, 20, 1 );
consoleSelect( &Bottom );
consoleClear( );
for( u16 i = 0; i < MAXMAPPOS; ++i ) {
if( p_t.px > MapLocations[ acMapRegion - 1 ][ i ].m_lx && p_t.px < MapLocations[ acMapRegion - 1 ][ i ].m_rx &&
p_t.py > MapLocations[ acMapRegion - 1 ][ i ].m_ly && p_t.py < MapLocations[ acMapRegion - 1 ][ i ].m_ry ) {
printf( FS::getLoc( MapLocations[ acMapRegion - 1 ][ i ].m_ind ) );
return;
}
}
}

void printMapLocation( const MapRegionPos& p_m ) {
consoleSetWindow( &Bottom, 5, 0, 20, 1 );
consoleSelect( &Bottom );
consoleClear( );
printf( FS::getLoc( p_m.m_ind ) );
acMapPoint = std::pair<u8, u8>( ( p_m.m_lx + p_m.m_rx ) / 2, ( p_m.m_ly + p_m.m_ry ) / 2 );
updateTime( );
return;
}*/

std::unique_ptr<MAP::Map> acMap;

u8 DayTimes[ 4 ][ 5 ] = {
    { 7, 10, 15, 17, 23 },
    { 6, 9, 12, 18, 23 },
    { 5, 8, 10, 20, 23 },
    { 7, 9, 13, 19, 23 }
};

u8 getCurrentDaytime( ) {
    time_t unixTime = time( NULL );
    struct tm* timeStruct = gmtime( (const time_t *)&unixTime );

    u8 t = timeStruct->tm_hour, m = timeStruct->tm_mon;

    for( u8 i = 0; i < 5; ++i )
        if( DayTimes[ m / 4 ][ i ] >= t )
            return i;
    return 254;
}


enum ChoiceResult {
    CONTINUE,
    NEW_GAME,
    OPTIONS,
    GEHEIMGESCHEHEN,
    TRANSFER_GAME,
    CANCEL
};

void fillWeiter( ) {
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

    sprintf( buffer, "%s", FS::getLoc( FS::SAV->m_acMapIdx ) );
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
void killWeiter( ) {
    consoleSetWindow( &IO::Bottom, 1, 1, 30, 22 );
    consoleSelect( &IO::Bottom );
    consoleClear( );
}

ChoiceResult opScreen( ) {
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
        std::pair<u8, u8>( 0, 84 ),
        std::pair<u8, u8>( 87, 108 ),
        std::pair<u8, u8>( 113, 134 ),
        std::pair<u8, u8>( 139, 160 ),
        std::pair<u8, u8>( 165, 186 )
    };

    /* if(gMod == DEVELOPER){
    consoleSelect(&IO::Bottom);
    consoleSetWindow(&IO::Bottom,0,23,30,2);
    printf("Slot 2: %s",acSlot2Game);
    }*/

    switch( FS::SAV->m_savTyp ) {
        case 3:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu0", 512, 49152, true );
            MaxVal = 5;
            fillWeiter( );
            break;
        }
        case 2:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu1", 512, 49152, true );
            MaxVal = 4;
            fillWeiter( );
            break;
        }
        case 1:
        {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "MainMenu2", 512, 49152, true );
            MaxVal = 3;
            results[ 2 ] = OPTIONS;

            fillWeiter( );
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
            killWeiter( );
            return CANCEL;
        }
    }

    //return OPTIONS;

    touchPosition touch;
    consoleSelect( &IO::Bottom );
    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
    loop( ) {
        swiWaitForVBlank( );

        scanKeys( );
        touch = touchReadXY( );
        //printf("(%d|%d|%d|%d)\n",touch.px,touch.py,touch.rawx,touch.rawy);
        //fflush(stdout);

        scanKeys( );
        u32 p = keysCurrent( );
        u32 k = keysHeld( ) | keysDown( );
        if( ( FS::SAV->m_savTyp == 1 ) && ( k & KEY_SELECT ) && ( k & KEY_RIGHT ) && ( k & KEY_L ) && ( k & KEY_R ) ) {
            killWeiter( );
            consoleClear( );
            ++FS::SAV->m_savTyp;
            return opScreen( );
        } else if( ( gMod == DEVELOPER ) && ( FS::SAV->m_savTyp == 2 ) && ( k & KEY_START ) && ( k & KEY_LEFT ) && ( k & KEY_L ) && ( k & KEY_R ) ) {
            killWeiter( );
            consoleClear( );
            ++FS::SAV->m_savTyp;
            return opScreen( );
        } else if( p & KEY_B ) {
            killWeiter( );
            consoleClear( );
            FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "ClearD", 512, 49152, true );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "ClearD", 512, 49152, true );
            for( u16 i = 1; i < 256; ++i )
                BG_PALETTE_SUB[ i ] = RGB15( 31, 31, 31 );
            return CANCEL;
        }
        for( u16 i = 0; i < MaxVal; i++ )
            if( ( touch.py > ranges[ i ].first && touch.py < ranges[ i ].second ) ) {
                loop( ) {
                    scanKeys( );
                    touch = touchReadXY( );
                    if( touch.px == 0 && touch.py == 0 )
                        break;
                }
                killWeiter( );
                FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "ClearD", 512, 49152, true );
                FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "ClearD", 512, 49152, true );
                for( u16 j = 1; j < 256; ++j )
                    BG_PALETTE_SUB[ j ] = RGB15( 31, 31, 31 );

                return results[ i ];
            }
    }
}

u8 lastdir;
s8 dir[ 5 ][ 2 ] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };

int MOV = 20;

bool cut::possible( ) {
    return false;
}
bool rockSmash::possible( ) {
    return false;
}
bool fly::possible( ) {
    return false;
}
bool flash::possible( ) {
    return true;
}
bool whirlpool::possible( ) {
    return false;
}
bool surf::possible( ) {
    return ( FS::SAV->m_acMoveMode != MAP::MoveMode::SURF )
        && acMap->m_blocks[ FS::SAV->m_acposy / 20 + 10 + dir[ lastdir ][ 0 ] ][ FS::SAV->m_acposx / 20 + 10 + dir[ lastdir ][ 1 ] ].m_movedata == 4;
}

bool heroIsBig = false;


void startScreen( ) {

    //irqInit( );
    irqEnable( IRQ_VBLANK );
    irqSet( IRQ_VBLANK, [ ]( ) { scanKeys( ); } );

    IO::vramSetup( );

    videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

    // set up our top bitmap background
    IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    //bgSet( IO::bg3, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( IO::bg3, 3 );
    bgUpdate( );

    // set up our bottom bitmap background
    IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
    // bgSet( IO::bg3sub, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( IO::bg3sub, 3 );
    bgUpdate( );
    //// set up our bottom bitmap background
    IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    //// bgSet( IO::bg2sub, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( IO::bg2sub, 2 );
    //bgUpdate();

    IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );

    IO::consoleFont->gfx = (u16*)consoleFontTiles;
    IO::consoleFont->pal = (u16*)consoleFontPal;
    IO::consoleFont->numChars = 218;
    IO::consoleFont->numColors = 16;
    IO::consoleFont->bpp = 8;
    IO::consoleFont->asciiOffset = 32;
    IO::consoleFont->convertSingleColor = false;

    consoleSetFont( &IO::Top, IO::consoleFont );
    consoleSetFont( &IO::Bottom, IO::consoleFont );

    //ReadSavegame
    /*if( gMod != EMULATOR ) {
        FS::SAV = FS::readSave( );
        } else {
        FS::SAV->m_savTyp = 0;
        }*/
START:
    //Intro

    //StartScreen

    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Title" );

    if( IO::BGs[ FS::SAV->m_bgIdx ].m_allowsOverlay )
        IO::drawSub( );
    FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "Clear", 512, 49152, true );

    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
    consoleSelect( &IO::Bottom );

    BG_PALETTE[ 3 ] = BG_PALETTE_SUB[ 3 ] = RGB15( 0, 0, 0 );

    printf( "@ Philip \"RedArceus\" Wellnitz\n                     2012 - 2015\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );

    if( gMod == DEVELOPER )
        printf( "                   %10sdev\n", Version.c_str( ) );
    else if( gMod == BETA )
        printf( "                  %10sbeta\n", Version.c_str( ) );
    else if( gMod == ALPHA )
        printf( "                 %10salpha\n", Version.c_str( ) );
    else if( gMod == EMULATOR )
        printf( "                   %10semu\n", Version.c_str( ) );
    if( gMod != RELEASE ) {
        std::string s = "\"" + CodeName + "\"";
        s.insert( s.begin( ), 32 - s.length( ), ' ' );
        printf( s.c_str( ) );
    }

    consoleSetWindow( &IO::Top, 0, 23, 32, 1 );
    consoleSelect( &IO::Top );
    int D0000 = 0;
    touchPosition tp;
    loop( ) {
        scanKeys( );
        tp = touchReadXY( );
        swiWaitForVBlank( );

        int pressed = keysCurrent( );
        if( ( pressed & KEY_A ) || ( pressed & KEY_START ) || ( tp.px || tp.py ) )
            break;
        ++D0000;
        if( !( D0000 % 120 ) ) {
            printf( "     BER\x9A""HRE, UM ZU STARTEN" );
            D0000 = 0;
        } else if( ( D0000 % 120 ) == 60 )
            consoleClear( );
    }

    while( tp.px || tp.py ) {
        scanKeys( );
        tp = touchReadXY( );
        swiWaitForVBlank( );
    }

    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
    consoleSelect( &IO::Bottom );
    consoleClear( );
    consoleSelect( &IO::Top );

    time_t uTime = time( NULL );
    tm* tStruct = gmtime( (const time_t *)&uTime );

    IO::hours = tStruct->tm_hour;
    IO::month = tStruct->tm_min;
    IO::seconds = tStruct->tm_sec;
    IO::day = tStruct->tm_mday;
    IO::month = tStruct->tm_mon + 1;
    IO::year = tStruct->tm_year + 1900;

    IO::ticks = 0;
    timerStart( 0, ClockDivider_1024, 0, NULL );
    IO::ticks += timerElapsed( 0 );

    srand( IO::hours ^ ( 100 * IO::minutes ) ^ ( 10000 * IO::seconds ) ^ ( IO::day ^ ( 100 * IO::month ) ^ IO::year ) );
    LastPID = rand( );

    //StartMenu

    switch( opScreen( ) ) {
        //        case TRANSFER_GAME:
        //        {
        //            char cmpgm[ 5 ][ 4 ] = { "BPE", "AXP", "AXV", "BPR", "BPG" };
        //            int acgame = -1;
        //
        //            for( u8 i = 0; i < 5; ++i ) {
        //                for( u8 j = 0; j < 3; ++j )
        //                    if( cmpgm[ i ][ j ] != acSlot2Game[ j ] )
        //                        goto CONT;
        //                acgame = i;
        //CONT:
        //                ;
        //            }
        //            if( acgame == -1 )
        //                goto START;
        //
        //            scrn.init( );
        //            IO::yesNoBox yn = IO::yesNoBox( IO::messageBox( "", 0, false, false, false ) );
        //            if( yn.getResult( "Möchtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?", false ) ) {
        //                IO::messageBox( "Solltest du im Folgenden\nspeichern, so werden Daten\nauf das GBA-Modul geschrieben.", false, false );
        //                IO::messageBox( "Bitte entferne daher das\nGBA-Modul nicht, es könnte\nden Spielstand beschädigen.", false, false );
        //                IO::messageBox( "Auch das Speichern an sich\nkann den Spielstand\nbeschädigen.", false, false );
        //                yn = IO::yesNoBox( );
        //                if( yn.getResult( "Möchtest du fortfahren?", false ) ) {
        //                    IO::messageBox( "Lade Spielstand...", 0, false, false, false );
        //                    int loadgame = acgame > 2 ? 1 : 0;
        //
        //                    gen3::SaveParser* save3 = gen3::SaveParser::Instance( );
        //
        //                    if( save3->load( loadgame ) == -1 ) {
        //                        IO::messageBox( "Ein Fehler ist aufgetreten.\nKehre zum Hauptmenü zurück." );
        //                        goto START;
        //                    }
        //                    FS::SAV = new saveGame( );
        //                    wchar_t savname[ 8 ] = { 0 };
        //                    for( int i = 0; i < 7; ++i )
        //                        savname[ i ] = gen3::getNText( save3->unpackeddata[ i ] );
        //                    FS::SAV->setName( savname );
        //
        //                    FS::SAV->m_isMale = !save3->unpackeddata[ 8 ];
        //
        //                    FS::SAV->m_Id = ( save3->unpackeddata[ 11 ] << 8 ) | save3->unpackeddata[ 10 ];
        //                    FS::SAV->m_Sid = ( save3->unpackeddata[ 13 ] << 8 ) | save3->unpackeddata[ 12 ];
        //
        //                    FS::SAV->m_pt.m_hours = ( save3->unpackeddata[ 15 ] << 8 ) | save3->unpackeddata[ 14 ];
        //                    FS::SAV->m_pt.m_mins = save3->unpackeddata[ 16 ];
        //                    FS::SAV->m_pt.m_secs = save3->unpackeddata[ 17 ];
        //
        //                    FS::SAV->m_gba.m_gameid = ( save3->unpackeddata[ 0xaf ] << 24 ) | ( save3->unpackeddata[ 0xae ] << 16 ) | ( save3->unpackeddata[ 0xad ] << 8 ) | save3->unpackeddata[ 0xac ];
        //
        //                    pokemonData p;
        //                    for( u8 i = 0; i < 6; ++i ) {
        //                        if( save3->pokemon[ i ]->personality ) {
        //                            FS::SAV->m_PkmnTeam.push_back( pokemon( ) );
        //
        //                            pokemon &acPkmn = FS::SAV->m_PkmnTeam[ i ];
        //                            gen3::belt_pokemon_t* &acBeltP = save3->pokemon[ i ];
        //
        //
        //                            acPkmn.m_boxdata.m_pid = acBeltP->personality;
        //                            acPkmn.m_boxdata.m_oTSid = acBeltP->otid >> 16;
        //                            acPkmn.m_boxdata.m_oTId = acBeltP->otid % ( 1 << 16 );
        //                            for( int i = 0; i < 10; ++i )
        //                                acPkmn.m_boxdata.m_name[ i ] = gen3::getNText( acBeltP->name[ i ] );
        //                            acPkmn.m_boxdata.m_name[ 10 ] = 0;
        //                            acPkmn.m_boxdata.m_hometown = acBeltP->language;
        //                            for( int i = 0; i < 7; ++i )
        //                                acPkmn.m_boxdata.m_oT[ i ] = gen3::getNText( acBeltP->otname[ i ] );
        //                            acPkmn.m_boxdata.m_oT[ 7 ] = 0;
        //                            acPkmn.m_boxdata.m_markings = acBeltP->markint;
        //
        //                            acPkmn.m_statusint = acBeltP->status;
        //                            acPkmn.m_Level = acBeltP->level;
        //                            acPkmn.m_boxdata.m_pokerus = acBeltP->pokerus;
        //
        //                            acPkmn.m_stats.m_acHP = acBeltP->currentHP;
        //                            acPkmn.m_stats.m_maxHP = acBeltP->maxHP;
        //                            acPkmn.m_stats.m_Atk = acBeltP->move;
        //                            acPkmn.m_stats.m_Def = acBeltP->defense;
        //                            acPkmn.m_stats.m_SAtk = acBeltP->spatk;
        //                            acPkmn.m_stats.m_SDef = acBeltP->spdef;
        //                            acPkmn.m_stats.m_Spd = acBeltP->speed;
        //
        //                            gen3::pokemon::pokemon_growth_t* &acBG = save3->pokemon_growth[ i ];
        //                            acPkmn.m_boxdata.m_speciesId = gen3::getNPKMNIdx( acBG->species );
        //                            acPkmn.m_boxdata.m_holdItem = gen3::getNItemIdx( acBG->held );
        //                            acPkmn.m_boxdata.m_experienceGained = acBG->xp;
        //                            acPkmn.m_boxdata.m_steps = acBG->happiness;
        //                            acPkmn.m_boxdata.m_pPUps = acBG->ppbonuses;
        //
        //                            gen3::pokemon::pokemon_moves_t* &acBA = save3->pokemon_moves[ i ];
        //                            for( int i = 0; i < 4; ++i ) {
        //                                acPkmn.m_boxdata.m_moves[ i ] = acBA->atk[ i ];
        //                                acPkmn.m_boxdata.m_acPP[ i ] = acBA->pp[ i ];
        //                            }
        //
        //                            gen3::pokemon::pokemon_effort_t* &acBE = save3->pokemon_effort[ i ];
        //                            for( int i = 0; i < 6; ++i ) {
        //                                acPkmn.m_boxdata.m_effortValues[ i ] = acBE->EV[ i ];
        //                                acPkmn.m_boxdata.m_contestStats[ i ] = acBE->ConStat[ i ];
        //                            }
        //
        //                            gen3::pokemon::pokemon_misc_t* &acBM = save3->pokemon_misc[ i ];
        //                            acPkmn.m_boxdata.m_iVint = acBM->IVint;
        //
        //                            getAll( acPkmn.m_boxdata.m_speciesId, p );
        //                            acPkmn.m_boxdata.m_ability = p.m_abilities[ acPkmn.m_boxdata.m_individualValues.m_isEgg ];
        //                            acPkmn.m_boxdata.m_individualValues.m_isEgg = acPkmn.m_boxdata.m_individualValues.m_isNicked;
        //                            acPkmn.m_boxdata.m_gotPlace = gen3::getNLocation( acBM->locationcaught );
        //
        //                            acPkmn.m_boxdata.m_gotLevel = acBM->levelcaught;
        //
        //                            if( acPkmn.m_boxdata.m_individualValues.m_isEgg || acPkmn.m_boxdata.m_gotLevel ) {
        //                                acPkmn.m_boxdata.m_hatchPlace = 999;
        //                                acPkmn.m_boxdata.m_gotLevel = 5;
        //                                acPkmn.m_boxdata.m_hatchDate[ 0 ] =
        //                                    acPkmn.m_boxdata.m_hatchDate[ 1 ] =
        //                                    acPkmn.m_boxdata.m_hatchDate[ 2 ] = 0;
        //                                acPkmn.m_boxdata.m_gotDate[ 0 ] =
        //                                    acPkmn.m_boxdata.m_gotDate[ 1 ] =
        //                                    acPkmn.m_boxdata.m_gotDate[ 2 ] = 1;
        //                            }
        //                            acPkmn.m_boxdata.m_oTisFemale = acBM->tgender;
        //                            acPkmn.m_boxdata.m_ball = acBM->pokeball;
        //                            acPkmn.m_boxdata.m_gotDate[ 0 ] =
        //                                acPkmn.m_boxdata.m_gotDate[ 1 ] =
        //                                acPkmn.m_boxdata.m_gotDate[ 2 ] = 0;
        //
        //                            FS::SAV->m_hasPKMN = true;
        //                        }
        //                    }
        //                    savMod = _GBA;
        //                    //load player to default pos
        //                    strcpy( FS::SAV->m_acMapName, "0/98" );
        //                    FS::SAV->m_acMapIdx = 1000;
        //                    FS::SAV->m_acposx = 2 * 20;
        //                    FS::SAV->m_acposy = 25 * 20;
        //                    FS::SAV->m_acposz = 3;
        //
        //                    FS::SAV->m_overWorldIdx = 20 * ( ( acgame + 1 ) / 2 ) + ( FS::SAV->m_isMale ? 0 : 10 );
        //
        //                    IO::Oam->oamBuffer[ SAVE_ID ].isHidden = false;
        //
        //                    IO::messageBox( "Abgeschlossen." );
        //                    break;
        //                } else goto START;
        //            } else goto START;
        //        }
        case GEHEIMGESCHEHEN:
        case CANCEL:
            //printf("%i",FS::SAV->SavTyp);
            //while(1);
            if( gMod != EMULATOR )
                goto START;
        case OPTIONS:
            FS::SAV = new FS::saveGame( );
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

            FS::SAV->m_overWorldIdx = 0;
            strcpy( FS::SAV->m_acMapName, "0/98" );
            FS::SAV->m_acMapIdx = 1000;
            FS::SAV->m_acposx = 2 * 20, FS::SAV->m_acposy = 25 * 20, FS::SAV->m_acposz = 3;
            break;
        case CONTINUE:
            break;
        case NEW_GAME:
            break;
    }
    swiWaitForVBlank( );
    swiWaitForIRQ( );
}

void showNewMap( u16 p_mapIdx ) {
    //for( u8 i = 0; i < 3; ++i ) {
    //    for( u8 j = 0; j < 75; ++j ) {
    //        MapRegionPos m = MapLocations[ i ][ j ];
    //        if( m.m_ind != p_mapIdx )
    //            continue;
    //        acMapRegion = Region( i + 1 );
    //        scrn.draw( mode = 1 + i );
    //        printMapLocation( m );
    //        IO::Oam->oamBuffer[ SQCH_ID ].x = IO::Oam->oamBuffer[ SQCH_ID + 1 ].x = ( m.m_lx + m.m_rx ) / 2 - 8;
    //        IO::Oam->oamBuffer[ SQCH_ID ].y = IO::Oam->oamBuffer[ SQCH_ID + 1 ].y = ( m.m_ly + m.m_ry ) / 2 - 8;
    //        IO::Oam->oamBuffer[ SQCH_ID ].isHidden = IO::Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = false;
    //        updateOAM( true );

    //        FS::SAV->m_acMapIdx = p_mapIdx;

    //        swiWaitForIRQ( );
    //        swiWaitForVBlank( );
    //        return;
    //    }
    //}
}

bool left = false;
void loadframe( IO::SpriteInfo* p_si, int p_idx, int p_frame, bool p_big = false ) {
    char buffer[ 50 ];
    sprintf( buffer, "%i/%i", p_idx, p_frame );
    if( !p_big )
        FS::readSpriteData( p_si, "nitro:/PICS/SPRITES/OW/", buffer, 64, 16 );
    else
        FS::readSpriteData( p_si, "nitro:/PICS/SPRITES/OW/", buffer, 128, 16 );
}

void animateHero( int p_dir, int p_frame, bool p_runDisable = false ) {
    heroIsBig = false;

    left = !left;
    bool bike = ( MAP::MoveMode )FS::SAV->m_acMoveMode == MAP::MoveMode::BIKE, run = ( keysHeld( ) & KEY_B ) && !p_runDisable;
    if( p_frame == 0 ) {
        switch( p_dir ) {
            case 0:
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
                IO::updateOAM( false );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForIRQ( );
                return;
            case 1:
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                IO::OamTop->oamBuffer[ 0 ].hFlip = true;
                if( !run ) {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 7, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 8, heroIsBig );
                } else {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 16, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 17, heroIsBig );
                }

                IO::updateOAM( false );
                swiWaitForVBlank( );
                swiWaitForIRQ( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            case 2:
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run ) {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 3, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 4, heroIsBig );
                } else {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 12, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 13, heroIsBig );
                }
                IO::updateOAM( false );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            case 3:
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run ) {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 7, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 8, heroIsBig );
                } else {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 16, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 17, heroIsBig );
                }
                IO::updateOAM( false );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            case 4:
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run ) {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 5, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 6, heroIsBig );
                } else {
                    if( left )
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 14, heroIsBig );
                    else
                        loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 15, heroIsBig );
                }
                IO::updateOAM( false );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            default:
                break;
        }
    }
    if( p_frame == 1 ) {
        switch( p_dir ) {
            case 0:
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
                IO::updateOAM( false );
                return;
            case 1:
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                IO::OamTop->oamBuffer[ 0 ].hFlip = true;
                IO::updateOAM( false );
                if( !run )
                    swiWaitForVBlank( );
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
                else
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 11, heroIsBig );
                IO::updateOAM( false );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 2, 0 );
                bgUpdate( );
                return;
            case 2:
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                IO::updateOAM( false );
                if( !run )
                    swiWaitForVBlank( );
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
                else
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 9, heroIsBig );
                IO::updateOAM( false );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, 2 );
                bgUpdate( );
                return;
            case 3:
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                IO::updateOAM( false );
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
                else
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 11, heroIsBig );
                IO::updateOAM( false );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], -2, 0 );
                bgUpdate( );
                return;
            case 4:
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 1, heroIsBig );
                else
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 10, heroIsBig );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                IO::updateOAM( false );
                bgUpdate( );
                swiWaitForVBlank( );
                for( u8 i = 1; i < 4; ++i )
                    bgScroll( MAP::bgs[ i ], 0, -2 );
                bgUpdate( );
                return;
            default:
                break;
        }
    }
    if( p_frame == 2 ) {
        switch( p_dir ) {
            case 0:
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0 );
                IO::updateOAM( false );
                return;
            case 1:
                IO::OamTop->oamBuffer[ 0 ].hFlip = true;
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
                //else
                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,11,heroIsBig);
                IO::updateOAM( false );
                return;
            case 2:
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 0, heroIsBig );
                //else
                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,9,heroIsBig);
                IO::updateOAM( false );
                return;
            case 3:
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 2, heroIsBig );
                //else
                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,11,heroIsBig);
                IO::updateOAM( false );
                return;
            case 4:
                IO::OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &IO::spriteInfoTop[ 0 ], FS::SAV->m_overWorldIdx, 1, heroIsBig );
                //else
                //    loadframe(&IO::spriteInfoTop[0],FS::SAV->owIdx,10,heroIsBig);
                IO::updateOAM( false );
                return;
            default:
                break;
        }
    }
}

inline void movePlayer( u16 p_direction ) {
    acMap->movePlayer( p_direction );
}

bool movePlayerOnMap( s16 p_x, s16 p_y, s16 p_z, bool p_init /*= true*/ ) {
    bool WTW = ( gMod == DEVELOPER ) && ( keysHeld( ) & KEY_R );

    MAP::MoveMode playermoveMode = ( MAP::MoveMode )FS::SAV->m_acMoveMode;

    p_x += 10;
    p_y += 10;

    if( p_x < 0 )
        return false;
    if( p_y < 0 )
        return false;
    if( p_x >= (int)acMap->m_sizey + 20 )
        return false;
    if( p_y >= (int)acMap->m_sizex + 20 )
        return false;

    int lastmovedata = acMap->m_blocks[ FS::SAV->m_acposy / 20 + 10 ][ FS::SAV->m_acposx / 20 + 10 ].m_movedata;
    int acmovedata = acMap->m_blocks[ p_y ][ p_x ].m_movedata;
    MAP::Block acBlock = acMap->m_blockSets.m_blocks[ acMap->m_blocks[ p_y ][ p_x ].m_blockidx ],
        lastBlock = acMap->m_blockSets.m_blocks[ acMap->m_blocks[ FS::SAV->m_acposy / 20 + 10 ][ FS::SAV->m_acposx / 20 + 10 ].m_blockidx ];

    int verhalten = acBlock.m_bottombehave, hintergrund = acBlock.m_topbehave;
    int lstverhalten = lastBlock.m_bottombehave, lsthintergrund = lastBlock.m_topbehave;
    if( verhalten == 0xa0 && playermoveMode != MAP::MoveMode::WALK ) //nur normales laufen möglich
        return false;

    if( verhalten == 0xc1 && p_y != FS::SAV->m_acposy / 20 + 10 ) //Rechts-Links-Blockung
        return false;
    if( verhalten == 0xc0 && p_x != FS::SAV->m_acposx / 20 + 10 ) //Oben-Unten-Blockung
        return false;
    if( verhalten >= 0xd3 && verhalten <= 0xd7 ) //fester block
        return false;

    if( !WTW ) {
        if( acmovedata == 1 )
            return false;
        if( ( acmovedata == 4 && playermoveMode != MAP::MoveMode::SURF ) )
            return false;
    }
    if( acmovedata == 0xc && playermoveMode == MAP::MoveMode::SURF ) {
        FS::SAV->m_acMoveMode = MAP::MoveMode::WALK;

    }

    int movedir = 0;
    int oldx = FS::SAV->m_acposy / 20 + 10, oldy = FS::SAV->m_acposx / 20 + 10;
    if( oldy < p_x )
        movedir = 1;
    else if( oldy > p_x )
        movedir = 3;
    else if( oldx < p_y )
        movedir = 2;
    else if( oldx > p_y )
        movedir = 4;

    if( lastmovedata == 0 && acmovedata % 4 == 0 )
        FS::SAV->m_acposz = p_z = acmovedata / 4;

    IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_2;
    if( ( verhalten == 0x70 || lstverhalten == 0x70 ) && p_z >= 4 )
        IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_1;
    if( acmovedata == 60 ) {
        if( p_z <= 3 )
            IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_3;
        else
            IO::OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_1;
    }
    if( WTW || ( acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) )
        animateHero( movedir, 0 );
    else {
        animateHero( movedir, 2 );
        swiWaitForVBlank( );
        bgUpdate( );
        return false;
    }

    if( p_x < 10 ) {
        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'W' && p_y >= a.m_move + 10 && p_y < a.m_move + a.m_mapsx + 10 ) {
                    showNewMap( a.m_mapidx );
                    strcpy( FS::SAV->m_acMapName, a.m_name );
                    FS::SAV->m_acMapIdx = a.m_mapidx;
                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
                    p_y -= a.m_move;
                    p_x = a.m_mapsy + 10;
                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 17, p_y - 18, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_y < 10 ) {
        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'N' && p_x >= a.m_move + 10 && p_x < a.m_move + a.m_mapsy + 10 ) {
                    showNewMap( a.m_mapidx );
                    strcpy( FS::SAV->m_acMapName, a.m_name );
                    FS::SAV->m_acMapIdx = a.m_mapidx;
                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
                    p_x -= a.m_move;
                    p_y = a.m_mapsx + 10;
                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 16, p_y - 19, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_x >= s32( acMap->m_sizey + 10 ) ) {
        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'E' && p_y >= a.m_move + 10 && p_y < a.m_move + a.m_mapsx + 10 ) {
                    showNewMap( a.m_mapidx );
                    strcpy( FS::SAV->m_acMapName, a.m_name );
                    FS::SAV->m_acMapIdx = a.m_mapidx;
                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
                    p_y -= a.m_move;
                    p_x = 9;
                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 15, p_y - 18, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_y >= s32( acMap->m_sizex + 10 ) ) {

        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'S'  && p_x >= a.m_move + 10 && p_x < a.m_move + a.m_mapsy + 10 ) {
                    showNewMap( a.m_mapidx );
                    strcpy( FS::SAV->m_acMapName, a.m_name );
                    FS::SAV->m_acMapIdx = a.m_mapidx;
                    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", a.m_name ) );
                    p_x -= a.m_move;
                    p_y = 9;
                    FS::SAV->m_acposx = 20 * ( p_x - 10 );
                    FS::SAV->m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 16, p_y - 17, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_init )
        acMap->draw( p_x - 16, p_y - 18, p_init );
    else
        movePlayer( movedir );

    animateHero( movedir, 1 );
    swiWaitForVBlank( );
    animateHero( movedir, 2 );
    swiWaitForVBlank( );
    if( p_init )
        animateHero( lastdir, 2 );

    IO::updateOAM( false );

    return true;
}

void animateMap( u8 p_frame ) {
    u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
    for( size_t i = 0; i < acMap->m_animations.size( ); ++i ) {
        MAP::Animation& a = acMap->m_animations[ i ];
        if( ( p_frame ) % ( a.m_speed ) == 0 || a.m_speed == 1 ) {
            a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
            swiCopy( &a.m_animationTiles[ a.m_acFrame ], tileMemory + a.m_tileIdx * 32, 16 );
        }
    }
}

void initMapSprites( ) {
    IO::initOAMTable( false );

    IO::SpriteInfo * SQCHAInfo = &IO::spriteInfoTop[ 0 ];
    SpriteEntry * SQCHA = &IO::OamTop->oamBuffer[ 0 ];
    SQCHAInfo->m_oamId = 0;
    SQCHAInfo->m_width = 16;
    SQCHAInfo->m_height = 32;
    SQCHAInfo->m_angle = 0;
    SQCHAInfo->m_entry = SQCHA;
    SQCHA->y = 72;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_TALL;
    SQCHA->isHidden = false;
    SQCHA->x = 120;
    SQCHA->size = OBJSIZE_32;
    SQCHA->gfxIndex = 0;
    SQCHA->priority = OBJPRIORITY_2;
    SQCHA->palette = 0;

    loadframe( SQCHAInfo, FS::SAV->m_overWorldIdx, 0 );

    SQCHAInfo = &IO::spriteInfoTop[ 1 ];
    SQCHA = &IO::OamTop->oamBuffer[ 1 ];
    SQCHAInfo->m_oamId = 1;
    SQCHAInfo->m_width = 32;
    SQCHAInfo->m_height = 32;
    SQCHAInfo->m_angle = 0;
    SQCHAInfo->m_entry = SQCHA;
    SQCHA->y = 72;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_SQUARE;
    SQCHA->isHidden = true;
    SQCHA->x = 112;
    SQCHA->size = OBJSIZE_32;
    SQCHA->gfxIndex = 16;
    SQCHA->priority = OBJPRIORITY_2;
    SQCHA->palette = 0;

    IO::SpriteInfo * B2Info = &IO::spriteInfoTop[ 2 ];
    SpriteEntry * B2 = &IO::OamTop->oamBuffer[ 2 ];
    B2Info->m_oamId = 2;
    B2Info->m_width = 64;
    B2Info->m_height = 64;
    B2Info->m_angle = 0;
    B2Info->m_entry = B2;
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 64;
    B2->y = 32;

    B2 = &IO::OamTop->oamBuffer[ 3 ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 128;
    B2->y = 32;
    B2->hFlip = true;

    B2 = &IO::OamTop->oamBuffer[ 4 ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 64;
    B2->y = 96;
    B2->hFlip = false;
    B2->vFlip = true;

    B2 = &IO::OamTop->oamBuffer[ 5 ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 128;
    B2->y = 96;
    B2->hFlip = true;
    B2->vFlip = true;

    dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, BigCirc1Pal, &SPRITE_PALETTE[ 16 ], 32 );
    dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, BigCirc1Tiles, &SPRITE_GFX[ 32 * 32 / sizeof( SPRITE_GFX[ 0 ] ) ], BigCirc1TilesLen );
    IO::updateOAM( false );
}

int stepcnt = 0;
void stepincrease( ) {
    //stepcnt = ( stepcnt + 1 ) % 256;
    //if( stepcnt == 0 ) {
    //    for( size_t s = 0; s < FS::SAV->m_PkmnTeam.size( ); ++s ) {
    //        pokemon& ac = FS::SAV->m_PkmnTeam[ s ];

    //        if( ac.m_boxdata.m_individualValues.m_isEgg ) {
    //            ac.m_boxdata.m_steps--;
    //            if( ac.m_boxdata.m_steps == 0 ) {
    //                ac.m_boxdata.m_individualValues.m_isEgg = false;
    //                ac.m_boxdata.m_hatchPlace = FS::SAV->m_acMapIdx;
    //                ac.m_boxdata.m_hatchDate[ 0 ] = acday;
    //                ac.m_boxdata.m_hatchDate[ 1 ] = acmonth + 1;
    //                ac.m_boxdata.m_hatchDate[ 2 ] = ( acyear + 1900 ) % 100;
    //                char buffer[ 50 ];
    //                sprintf( buffer, "%ls schüpfte\naus dem Ei!", ac.m_boxdata.m_name );
    //                IO::messageBox M( buffer );
    //            }
    //        } else
    //            ac.m_boxdata.m_steps = std::min( 255, ac.m_boxdata.m_steps + 1 );
    //    }
    //}
}

void cut::use( ) { }
void rockSmash::use( ) { }
void fly::use( ) { }
void flash::use( ) { }
void whirlpool::use( ) { }
void surf::use( ) {
    //heroIsBig = true;
    FS::SAV->m_acMoveMode = MAP::MoveMode::SURF;
    movePlayerOnMap( FS::SAV->m_acposx / 20 + dir[ lastdir ][ 1 ], FS::SAV->m_acposy / 20 + dir[ lastdir ][ 0 ], FS::SAV->m_acposz, false );
    FS::SAV->m_acposx += 20 * dir[ lastdir ][ 1 ];
    FS::SAV->m_acposy += 20 * dir[ lastdir ][ 0 ];
}

void shoUseAttack( u16 p_pkmIdx, bool p_female, bool p_shiny ) {
    IO::OamTop->oamBuffer[ 0 ].isHidden = true;
    IO::OamTop->oamBuffer[ 1 ].isHidden = false;
    for( u8 i = 0; i < 5; ++i ) {
        loadframe( &IO::spriteInfoTop[ 1 ], FS::SAV->m_overWorldIdx + 4, i, true );
        IO::updateOAM( false );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
    }
    for( u8 i = 0; i < 4; ++i )
        IO::OamTop->oamBuffer[ 2 + i ].isHidden = false;
    if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 6, 2, 96, false, p_shiny, p_female ) ) {
        IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 6, 2, 96, false, p_shiny, !p_female );
    }
    IO::updateOAM( false );

    for( u8 i = 0; i < 40; ++i )
        swiWaitForVBlank( );

    //animateHero(lastdir,2);
    IO::OamTop->oamBuffer[ 0 ].isHidden = false;
    IO::OamTop->oamBuffer[ 1 ].isHidden = true;
    for( u8 i = 0; i < 8; ++i )
        IO::OamTop->oamBuffer[ 2 + i ].isHidden = true;
    IO::updateOAM( false );
}


u8 positions[ 6 ][ 2 ] = {
    { 14, 2 }, { 16, 3 }, { 14, 9 },
    { 16, 10 }, { 14, 17 }, { 16, 18 }
};
void initMainSprites( ) {
    IO::initOAMTable( true );
    u16 tileCnt = 0;

    tileCnt = IO::loadSprite( BACK_ID, BACK_ID, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );

    tileCnt = IO::loadSprite( SAVE_ID, SAVE_ID, tileCnt,
                              -20, -20, 64, 64, SavePal,
                              SaveTiles, SaveTilesLen, false, false, false, OBJPRIORITY_0, true );

    //Main menu sprites

    tileCnt = IO::loadSprite( OPTS_ID, OPTS_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) + 1 ] - 16,
                              32, 32, OptionPal, OptionTiles, OptionTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( PKMN_ID, PKMN_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) + 1 ] - 16,
                              32, 32, PokemonSpPal, PokemonSpTiles, PokemonSpTilesLen,
                              false, false, !FS::SAV->m_pkmnTeam[ 0 ].m_boxdata.m_speciesId, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( NAV_ID, NAV_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) + 1 ] - 16,
                              32, 32, NavPal, NavTiles, NavTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( ID_ID, ID_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) + 1 ] - 16,
                              32, 32, IdPal, IdTiles, IdTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( DEX_ID, DEX_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) + 1 ] - 16,
                              32, 32, PokeDexPal, PokeDexTiles, PokeDexTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( BAG_ID, BAG_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) + 1 ] - 16,
                              32, 32, SPBagPal, SPBagTiles, SPBagTilesLen,
                              false, false, false, OBJPRIORITY_0, true );

    IO::updateOAM( true );
}



DEX::dexUI dui( true, 1, FS::SAV->m_hasGDex ? 649 : 493 );
DEX::dex dx( FS::SAV->m_hasGDex ? 649 : 493, &dui );

int main( int p_argc, char** p_argv ) {


    //Init
    powerOn( POWER_ALL_2D );

    fatInitDefault( );
    nitroFSInit( p_argv[ 0 ] );

    //PRE-Intro
    touchPosition touch;

    sysSetBusOwners( true, true );
    memcpy( acSlot2Game, (char*)0x080000AC, 4 );

    startScreen( );


    heroIsBig = FS::SAV->m_acMoveMode != MAP::MoveMode::WALK;

    FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "Clear", 512, 49152, true );
    FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "Clear", 512, 49152, true );

    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "Clear" );
    acMap = std::unique_ptr<MAP::Map>( new MAP::Map( "nitro:/MAPS/", FS::SAV->m_acMapName ) );

    movePlayerOnMap( FS::SAV->m_acposx / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, true );
    lastdir = 0;

    IO::regularFont->setColor( RGB( 0, 31, 31 ), 0 );

    int HILFSCOUNTER = 252;

    FS::SAV->m_hasGDex = true;
    FS::SAV->m_evolveInBattle = true;

    initMapSprites( );
    IO::updateOAM( false );

    IO::drawSub( );
    //Eliminate the time stamp when the rtc does not work
    IO::updateTime( s8( 1 ) );
    IO::drawSub( );

    char buffer[ 120 ] = { 0 };
    swiWaitForIRQ( );
    swiWaitForVBlank( );

    initMainSprites( );
    IO::updateOAM( true );

    consoleSelect( &IO::Bottom );
    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
    consoleClear( );

    loop( ) {
        IO::updateTime( s8( 1 ) );
        swiWaitForVBlank( );
        touchRead( &touch );
        int pressed = keysUp( ), held = keysHeld( );

        if( held & KEY_L && gMod == DEVELOPER ) {
            u32 KEYS_CUR = ( ( ( ( ~REG_KEYINPUT ) & 0x3ff ) | ( ( ( ~__transferRegion( )->buttons ) & 3 ) << 10 ) | ( ( ( ~__transferRegion( )->buttons ) << 6 ) & ( KEY_TOUCH | KEY_LID ) ) ) ^ KEY_LID );

            std::sprintf( buffer, "Keys: %lu, H: %lu, D: %lu, U: %lu, C: %lu\nMap: %3i %3i, x: %3i y: %3i z: %3i\n%s %i",
                          KEYS_CUR,
                          keysHeld( ),
                          keysDown( ),
                          keysUp( ),
                          keysCurrent( ),
                          acMap->m_sizex,
                          acMap->m_sizey,
                          FS::SAV->m_acposx / 20,
                          ( FS::SAV->m_acposy ) / 20,
                          FS::SAV->m_acposz,
                          FS::SAV->m_acMapName,
                          FS::SAV->m_acMapIdx );
            IO::messageBox m( buffer );

            initMainSprites( );
            IO::drawSub( );
        }

        if( pressed & KEY_A ) {
            for( u8 i = 0; i < 6; ++i ) {
                if( !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                    break;
                auto a = FS::SAV->m_pkmnTeam[ i ];
                if( !a.m_boxdata.m_individualValues.m_isEgg ) {
                    for( u8 i = 0; i < 4; ++i ) {
                        if( AttackList[ a.m_boxdata.m_moves[ i ] ]->m_isFieldAttack
                            && AttackList[ a.m_boxdata.m_moves[ i ] ]->possible( ) ) {

                            char buffer[ 50 ];
                            sprintf( buffer, "%s\nMöchtest du %s nutzen?", AttackList[ a.m_boxdata.m_moves[ i ] ]->text( ), AttackList[ a.m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ) );
                            IO::yesNoBox yn;
                            if( yn.getResult( buffer ) ) {
                                IO::drawSub( );
                                sprintf( buffer, "%ls setzt %s\nein!", a.m_boxdata.m_name, AttackList[ a.m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ) );
                                IO::messageBox( buffer, true, true );
                                shoUseAttack( a.m_boxdata.m_speciesId, a.m_boxdata.m_isFemale, a.m_boxdata.isShiny( ) );
                                AttackList[ a.m_boxdata.m_moves[ i ] ]->use( );
                            }
                            IO::drawSub( );
                            initMainSprites( );
                            goto OUT;
                        }
                    }
                }
            }
OUT:
            ;
        }
        //Moving
        if( pressed & KEY_DOWN ) {
            animateHero( 2, 2, true );
            lastdir = 2;
            continue;
        }
        if( pressed & KEY_RIGHT ) {
            animateHero( 1, 2, true );
            lastdir = 1;
            continue;
        }
        if( pressed & KEY_UP ) {
            animateHero( 4, 2, true );
            lastdir = 4;
            continue;
        }
        if( pressed & KEY_LEFT ) {
            animateHero( 3, 2, true );
            lastdir = 3;
            continue;
        }

        if( held & KEY_DOWN ) {
            scanKeys( );
            if( movePlayerOnMap( FS::SAV->m_acposx / 20, ( FS::SAV->m_acposy + MOV ) / 20, FS::SAV->m_acposz, false ) ) {
                FS::SAV->m_acposy += MOV;
                stepincrease( );
                lastdir = 2;
            }
            if( FS::SAV->m_acMoveMode != MAP::MoveMode::BIKE )
                continue;
        }
        if( held & KEY_LEFT ) {
            scanKeys( );
            if( movePlayerOnMap( ( FS::SAV->m_acposx - MOV ) / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, false ) ) {
                FS::SAV->m_acposx -= MOV;
                stepincrease( );
                lastdir = 3;
            }
            if( FS::SAV->m_acMoveMode != MAP::MoveMode::BIKE )
                continue;
        }
        if( held & KEY_RIGHT ) {
            scanKeys( );
            if( movePlayerOnMap( ( FS::SAV->m_acposx + MOV ) / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, false ) ) {
                FS::SAV->m_acposx += MOV;
                stepincrease( );
                lastdir = 1;
            }
            if( FS::SAV->m_acMoveMode != MAP::MoveMode::BIKE )
                continue;
        }
        if( held & KEY_UP ) {
            scanKeys( );
            if( movePlayerOnMap( FS::SAV->m_acposx / 20, ( FS::SAV->m_acposy - MOV ) / 20, FS::SAV->m_acposz, false ) ) {
                FS::SAV->m_acposy -= MOV;
                stepincrease( );
                lastdir = 4;
            }
            if( FS::SAV->m_acMoveMode != MAP::MoveMode::BIKE )
                continue;
        }
        //StartBag
#define p_time true
#define p_timeParameter (s8(1))
        if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 6 ],
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 7 ], 16 ) ) {


            //FS::SAV->m_bag.draw( FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            //initMapSprites( );
            //movePlayerOnMap( FS::SAV->m_acposx / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, true );
        } else if( FS::SAV->m_pkmnTeam[ 0 ].m_boxdata.m_speciesId     //StartPkmn
                   && ( GET_AND_WAIT( KEY_START )
                   || GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 0 ],
                   IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 1 ], 16 ) ) ) {

            std::vector<pokemon> tmp;
            for( u8 i = 0; i < 6; ++i )
                if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                    tmp.push_back( FS::SAV->m_pkmnTeam[ i ] );
                else
                    break;
            STS::regStsScreenUI rsUI( &tmp );
            STS::regStsScreen sts( 0, &rsUI );
            sts.run( true, s8( 1 ) );

            for( u8 i = 0; i < tmp.size( ); ++i )
                FS::SAV->m_pkmnTeam[ i ] = tmp[ i ];

            consoleSelect( &IO::Top );
            consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
            consoleClear( );
            consoleSelect( &IO::Bottom );
            consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
            consoleClear( );
            initMapSprites( );
            initMainSprites( );
            movePlayerOnMap( FS::SAV->m_acposx / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, true );
        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 4 ],        //StartDex
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 5 ], 16 ) ) {

            dx.run( dui.currPkmn( ), true, 0 );

            consoleSelect( &IO::Top );
            consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
            consoleClear( );
            consoleSelect( &IO::Bottom );
            consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
            consoleClear( );

            initMapSprites( );
            initMainSprites( );
            movePlayerOnMap( FS::SAV->m_acposx / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, true );
        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 8 ],        //StartOptions
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 9 ], 16 ) ) {


        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 ],        //StartID
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 3 ], 16 ) ) {

            const char *someText[ 7 ] = { "PKMN-Spawn", "Item-Spawn", "1-Item-Test", "Dbl Battle", "Sgl Battle", "Chg NavScrn", " ... " };
            IO::choiceBox test( 6, &someText[ 0 ], 0, false );
            int res = test.getResult( "Tokens of god-being...", true );
            IO::drawSub( );
            switch( res ) {
                case 0:
                {
                    memset( FS::SAV->m_pkmnTeam, 0, sizeof( FS::SAV->m_pkmnTeam ) );
                    for( int i = 0; i < 3; ++i ) {
                        pokemon& a = FS::SAV->m_pkmnTeam[ i ];
                        a = pokemon( 0, HILFSCOUNTER, 0,
                                     50, FS::SAV->m_id, FS::SAV->m_sid, FS::SAV->m_playername,
                                     !FS::SAV->m_isMale, false, rand( ) % 2, rand( ) % 2, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        if( canLearn( HILFSCOUNTER, 57, 4 ) )
                            a.m_boxdata.m_moves[ 2 ] = 57;
                        if( canLearn( HILFSCOUNTER, 19, 4 ) )
                            a.m_boxdata.m_moves[ 1 ] = 19;
                        a.m_boxdata.m_experienceGained += 750;

                        FS::SAV->m_inDex[ ( a.m_boxdata.m_speciesId - 1 ) / 8 ] |= ( 1 << ( ( a.m_boxdata.m_speciesId - 1 ) % 8 ) );

                        HILFSCOUNTER = 3 + ( ( HILFSCOUNTER ) % 649 );
                    }
                    for( u16 i = 0; i < 649 / 8; ++i )
                        FS::SAV->m_inDex[ i ] = 255;

                    swiWaitForVBlank( );
                    break;
                }
                case 1:
                    if( !FS::SAV->m_bag )
                        FS::SAV->m_bag = new BAG::bag( );
                    for( u16 j = 1; j < 800; ++j )
                        if( ItemList[ j ]->m_itemName != "Null" )
                            FS::SAV->m_bag->insert( BAG::toBagType( ItemList[ j ]->m_itemType ), j, 1 );
                    break;
                case 2:
                    IO::messageBox( berry( "Ginemabeere" ), 31 );
                    break;
                case 3:{
                    std::vector<pokemon> tmp, cpy;
                    for( u8 i = 0; i < 6; ++i )
                        if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                            tmp.push_back( FS::SAV->m_pkmnTeam[ i ] );
                        else
                            break;
                    BATTLE::battleTrainer me( "TEST", 0, 0, 0, 0, &tmp, 0 );

                    for( u8 i = 0; i < 3; ++i ) {
                        pokemon a( 0, HILFSCOUNTER, 0,
                                   30, FS::SAV->m_id + 1, FS::SAV->m_sid, L"Heiko"/*FS::SAV->getName()*/, i % 2, true, rand( ) % 2, true, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }

                    BATTLE::battleTrainer opp( "Heiko", "Auf in den Kampf!", "Hm... Du bist gar nicht so schlecht...", "Yay gewonnen!", "Das war wohl eine Niederlage...", &( cpy ), 0 );

                    BATTLE::battle test_battle( &me, &opp, 100, 5, BATTLE::battle::DOUBLE );
                    test_battle.start( );
                    for( u8 i = 0; i < tmp.size( ); ++i )
                        FS::SAV->m_pkmnTeam[ i ] = tmp[ i ];
                    break;
                }
                case 4:{
                    std::vector<pokemon> tmp, cpy;
                    for( u8 i = 0; i < 6; ++i )
                        if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                            tmp.push_back( FS::SAV->m_pkmnTeam[ i ] );
                        else
                            break;
                    BATTLE::battleTrainer me( "TEST", 0, 0, 0, 0, &tmp, 0 );

                    for( u8 i = 0; i < 6; ++i ) {
                        pokemon a( 0, HILFSCOUNTER, 0,
                                   15, FS::SAV->m_id + 1, FS::SAV->m_sid, L"Heiko"/*FS::SAV->getName()*/, i % 2, true, rand( ) % 2, true, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }

                    BATTLE::battleTrainer opp( "Heiko", "Auf in den Kampf!", "Hm... Du bist gar nicht so schlecht...", "Yay gewonnen!", "Das war wohl eine Niederlage...", &( cpy ), 0 );

                    BATTLE::battle test_battle( &me, &opp, 100, 5, BATTLE::battle::SINGLE );
                    test_battle.start( );
                    for( u8 i = 0; i < tmp.size( ); ++i )
                        FS::SAV->m_pkmnTeam[ i ] = tmp[ i ];
                    break;
                }
                case 5:{
                    const char *bgNames[ MAXBG ];
                    for( u8 o = 0; o < MAXBG; ++o )
                        bgNames[ o ] = IO::BGs[ o ].m_name.c_str( );

                    IO::choiceBox scrnChoice( MAXBG, bgNames, 0, true );
                    IO::drawSub( scrnChoice.getResult( "Welcher Hintergrund\nsoll dargestellt werden?" ) );
                }
            }

            IO::drawSub( );
            initMainSprites( );
            if( res == 3 || res == 4 ) {
                initMapSprites( );
                movePlayerOnMap( FS::SAV->m_acposx / 20, FS::SAV->m_acposy / 20, FS::SAV->m_acposz, true );
            }
        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 10 ],  //Start Pokénav
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 11 ], 16 ) ) {

        } else if( touch.px != 0 && touch.py != 0 && GET_AND_WAIT_C( 8, 12, 17 ) ) {
            IO::yesNoBox Save( "PokéNav " );
            if( Save.getResult( "Möchtest du deinen\nFortschritt sichern?\n" ) ) {
                IO::drawSub( );
                if( gMod == EMULATOR )
                    IO::messageBox Succ( "Speichern?\nIn einem Emulator?!", "PokéNav" );
                else if( FS::writeSave( FS::SAV ) )
                    IO::messageBox Succ( "Fortschritt\nerfolgreich gesichert!", "PokéNav" );
                else
                    IO::messageBox Succ( "Es trat ein Fehler auf\nSpiel nicht gesichert.", "PokéNav" );
            }
            initMainSprites( );
            IO::drawSub( );
        }
        //End 

        scanKeys( );
    }
    return 0;
}