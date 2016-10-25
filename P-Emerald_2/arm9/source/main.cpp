/*
Pokémon Emerald 2 Version
------------------------------

file        : main.cpp
author      : Philip Wellnitz
description : Main ARM9 entry point

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


#include <nds.h>
#include <fat.h>
#include <filesystem.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <memory>

#include "defines.h"
#include "saveGame.h"
#include "fs.h"
#include "startScreen.h"

#include "hmMoves.h"
#include "item.h"
#include "berry.h"
#include "pokemon.h"

#include "uio.h"
#include "messageBox.h"
#include "yesNoBox.h"
#include "choiceBox.h"
#include "keyboard.h"
#include "sprite.h"
#include "screenFade.h"

#include "mapDrawer.h"
#include "mapSlice.h"
#include "mapObject.h"

#include "battle.h"
#include "battleTrainer.h"
#include "nav.h"


#include "BigCirc1.h"
#include "consoleFont.h"

#ifndef _EMULATOR
GameMod gMod = GameMod::DEVELOPER;
#else
GameMod gMod = GameMod::EMULATOR;
#endif

u8 DayTimes[ 4 ][ 5 ] = {
    { 7, 10, 15, 17, 23 },
    { 6, 9, 12, 18, 23 },
    { 5, 8, 10, 20, 23 },
    { 7, 9, 13, 19, 23 }
};

int hours = 0, seconds = 0, minutes = 0, day = 0, month = 0, year = 0;
int achours = 0, acseconds = 0, acminutes = 0, acday = 0, acmonth = 0, acyear = 0;
int pressed, held, last;
bool DRAW_TIME = false;
bool UPDATE_TIME = true;
bool ANIMATE_MAP = false;
u8 FRAME_COUNT = 0;
bool SCREENS_SWAPPED = false;
bool PLAYER_IS_FISHING = false;
bool INIT_NITROFS = false;

char** ARGV;

u8 getCurrentDaytime( ) {
    u8 t = achours, m = acmonth;

    for( u8 i = 0; i < 5; ++i )
        if( DayTimes[ m / 4 ][ i ] >= t )
            return i;
    return 254;
}

void initGraphics( ) {
    IO::vramSetup( );

    IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
    IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    for( u8 i = 0; i < 4; ++i )
        bgSetPriority( i, i );
    for( u8 i = 0; i < 4; ++i )
        bgSetPriority( 4 + i, i );
    bgUpdate( );

    IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );

    IO::consoleFont->gfx = ( u16* ) const_cast<unsigned int*>( consoleFontTiles );
    IO::consoleFont->pal = ( u16* ) const_cast<unsigned short*>( consoleFontPal );
    IO::consoleFont->numChars = 218;
    IO::consoleFont->numColors = 16;
    IO::consoleFont->bpp = 8;
    IO::consoleFont->asciiOffset = 32;
    IO::consoleFont->convertSingleColor = false;

    consoleSetFont( &IO::Top, IO::consoleFont );
    consoleSetFont( &IO::Bottom, IO::consoleFont );
}
void initTimeAndRnd( ) {
    time_t uTime = time( NULL );
    tm* tStruct = gmtime( (const time_t *) &uTime );

    hours = tStruct->tm_hour;
    month = tStruct->tm_min;
    seconds = tStruct->tm_sec;
    day = tStruct->tm_mday;
    month = tStruct->tm_mon + 1;
    year = tStruct->tm_year + 1900;

    srand( hours ^ ( 100 * minutes ) ^ ( 10000 * seconds ) ^ ( day ^ ( 100 * month ) ^ year ) );
}

int main( int, char** p_argv ) {
    //Init
    powerOn( POWER_ALL_2D );
    nitroFSInit( p_argv );
    ARGV = p_argv;

    irqEnable( IRQ_VBLANK );
    initGraphics( );
    initTimeAndRnd( );

    //Read the savegame
    if( gMod != EMULATOR && p_argv[ 0 ] )
        SAVE::SAV = FS::readSave( p_argv[ 0 ] );
    else
        SAVE::SAV = 0;
    if( !SAVE::SAV )
        SAVE::SAV = std::unique_ptr<SAVE::saveGame>( new SAVE::saveGame( ) );
    SAVE::startScreen( ).run( );
    IO::clearScreenConsole( false, true );
    IO::clearScreen( false, true );

    irqSet( IRQ_VBLANK, [ ] ( ) {
        scanKeys( );
        FRAME_COUNT++;

        if( ANIMATE_MAP && MAP::curMap )
            MAP::curMap->animateMap( FRAME_COUNT );

        if( INIT_NITROFS ) {
            nitroFSInit( ARGV );
            INIT_NITROFS = false;
        }

        if( !UPDATE_TIME )
            return;

        auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

        IO::boldFont->setColor( 0, 0 );
        u8 oldC1 = IO::boldFont->getColor( 1 );
        u8 oldC2 = IO::boldFont->getColor( 2 );
        IO::boldFont->setColor( 0, 1 );
        IO::boldFont->setColor( BLACK_IDX, 2 );
        pal[ BLACK_IDX ] = BLACK;
        time_t unixTime = time( NULL );
        struct tm* timeStruct = gmtime( (const time_t *) &unixTime );

        if( acseconds != timeStruct->tm_sec || DRAW_TIME ) {
            DRAW_TIME = false;
            pal[ WHITE_IDX ] = WHITE;
            IO::boldFont->setColor( WHITE_IDX, 1 );
            IO::boldFont->setColor( WHITE_IDX, 2 );

            char buffer[ 50 ];
            sprintf( buffer, "%02i:%02i:%02i", achours, acminutes, acseconds );
            IO::boldFont->printString( buffer, 18 * 8, 192 - 16, !SCREENS_SWAPPED );

            SAVE::SAV->getActiveFile( ).m_pt.m_secs++; // I know, this is rather inaccurate 

            SAVE::SAV->getActiveFile( ).m_pt.m_mins += ( SAVE::SAV->getActiveFile( ).m_pt.m_secs / 60 );
            SAVE::SAV->getActiveFile( ).m_pt.m_hours += ( SAVE::SAV->getActiveFile( ).m_pt.m_mins / 60 );

            SAVE::SAV->getActiveFile( ).m_pt.m_secs %= 60;
            SAVE::SAV->getActiveFile( ).m_pt.m_mins %= 60;

            achours = timeStruct->tm_hour;
            acminutes = timeStruct->tm_min;
            acseconds = timeStruct->tm_sec;

            IO::boldFont->setColor( 0, 1 );
            IO::boldFont->setColor( BLACK_IDX, 2 );
            sprintf( buffer, "%02i:%02i:%02i", achours, acminutes, acseconds );
            IO::boldFont->printString( buffer, 18 * 8, 192 - 16, !SCREENS_SWAPPED );
        }
        achours = timeStruct->tm_hour;
        acminutes = timeStruct->tm_min;
        acday = timeStruct->tm_mday;
        acmonth = timeStruct->tm_mon + 1;
        acyear = timeStruct->tm_year + 1900;

        IO::boldFont->setColor( oldC1, 1 );
        IO::boldFont->setColor( oldC2, 2 );
    } );
    IO::NAV = new IO::nav( );

    FADE_TOP( );
    MAP::curMap = new MAP::mapDrawer( );
    MAP::curMap->draw( );


    ANIMATE_MAP = true;

    touchPosition touch;
    bool stopped = true;
    u8 bmp = false;
    loop( ) {
        swiWaitForVBlank( );
        touchRead( &touch );
        pressed = keysUp( );
        last = held;
        held = keysHeld( );

#ifdef DEBUG
        if( held & KEY_L && gMod == DEVELOPER ) {
            time_t unixTime = time( NULL );
            struct tm* timeStruct = gmtime( (const time_t *) &unixTime );
            char buffer[ 100 ];
            snprintf( buffer, 99, "Currently at %hu-(%hu,%hu,%hu).\nMap: %hu:%hu, (%02hX,%02hX)\nFRAME: %hhu; %2d:%2d:%2d (%2d)",
                      SAVE::SAV->getActiveFile( ).m_currentMap,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY / 32,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX / 32,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX % 32,
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY % 32,
                      FRAME_COUNT,
                      achours, acminutes, acseconds, timeStruct->tm_sec );
            IO::messageBox m( buffer );
            IO::NAV->draw( true );
        }
#endif

        if( held & KEY_A ) {
            for( u8 i = 0; i < 6; ++i ) {
                if( !SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                    break;
                auto a = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ];
                if( a.m_boxdata.m_individualValues.m_isEgg )
                    continue;
                for( u8 j = 0; j < 4; ++j ) {
                    if( !AttackList[ a.m_boxdata.m_moves[ j ] ]->m_isFieldAttack
                        || !AttackList[ a.m_boxdata.m_moves[ j ] ]->possible( ) )
                        continue;
                    char buffer[ 50 ];
                    snprintf( buffer, 49, GET_STRING( 3 ), AttackList[ a.m_boxdata.m_moves[ j ] ]->text( ), AttackList[ a.m_boxdata.m_moves[ j ] ]->m_moveName.c_str( ) );
                    IO::yesNoBox yn;
                    if( yn.getResult( buffer ) ) {
                        IO::NAV->draw( );
                        swiWaitForVBlank( );
                        snprintf( buffer, 49, GET_STRING( 99 ), a.m_boxdata.m_name, AttackList[ a.m_boxdata.m_moves[ j ] ]->m_moveName.c_str( ) );
                        IO::messageBox( buffer, 0, false );
                        MAP::curMap->usePkmn( a.m_boxdata.m_speciesId, a.m_boxdata.m_isFemale, a.m_boxdata.isShiny( ) );
                        IO::NAV->draw( true );
                        swiWaitForVBlank( );

                        AttackList[ a.m_boxdata.m_moves[ j ] ]->use( );
                    }
                    IO::NAV->draw( true );
                    goto OUT;
                }
            }
OUT:
            scanKeys( );
            continue;
        }
        //Movement
        if( held & ( KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT ) ) {
            MAP::direction curDir = GET_DIR( held );
            scanKeys( );

            stopped = false;
            if( MAP::curMap->canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, curDir, SAVE::SAV->getActiveFile( ).m_player.m_movement ) ) {
                MAP::curMap->movePlayer( curDir, ( held & KEY_B ) );
                bmp = false;
            } else if( !bmp ) {
                //Play "Bump" sound
                MAP::curMap->stopPlayer( curDir );
                swiWaitForVBlank( );
                bmp = true;
            } else if( bmp < 2 ) {
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                MAP::curMap->stopPlayer( );
                bmp = 2;
            }
            continue;
        }
        if( !stopped ) {
            MAP::curMap->stopPlayer( );
            stopped = true;
            bmp = false;
        }

        IO::NAV->handleInput( touch, p_argv[ 0 ] );
        //End
        scanKeys( );
    }
    delete MAP::curMap;
    delete IO::NAV;
    return 0;
}
