/*
Pokémon neo
------------------------------

file        : main.cpp
author      : Philip Wellnitz
description : Main ARM9 entry point

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

#include <fat.h>
#include <filesystem.h>
#include <nds.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "defines.h"
#include "fs.h"
#include "saveGame.h"
#include "sound.h"
#include "startScreen.h"

#include "berry.h"
#include "item.h"
#include "pokemon.h"

#include "choiceBox.h"
#include "keyboard.h"
#include "messageBox.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"

#include "mapDrawer.h"
#include "mapObject.h"
#include "mapSlice.h"

#include "battle.h"
#include "battleTrainer.h"
#include "nav.h"

#include "BigCirc1.h"
#include "consoleFont.h"

#ifdef DESQUID
GameMod gMod = GameMod::DEVELOPER;
#else
GameMod gMod = GameMod::ALPHA;
#endif

constexpr u8 DAY_TIMES[ 4 ][ 5 ]
    = { { 7, 10, 15, 17, 23 }, { 6, 9, 12, 18, 23 }, { 5, 8, 10, 20, 23 }, { 7, 9, 13, 19, 23 } };

time_t        unixTime;
int           pressed, held, last;
touchPosition touch;
u8            cooldown          = COOLDOWN_COUNT;
bool          DRAW_TIME         = false;
bool          UPDATE_TIME       = true;
bool          ANIMATE_MAP       = false;
u8            FRAME_COUNT       = 0;
u8            TIME_COUNT        = 0;
bool          SCREENS_SWAPPED   = false;
bool          PLAYER_IS_FISHING = false;
bool          INIT_NITROFS      = false;
bool          TWL_CONFIG        = false;
bool          IN_GAME           = false;
bool          RTC_BAD           = false;

char** ARGV;

u8 getCurrentDaytime( ) {
    u8 t = SAVE::CURRENT_TIME.m_hours, m = SAVE::CURRENT_DATE.m_month;

    for( u8 i = 0; i < 5; ++i )
        if( DAY_TIMES[ m / 4 ][ i ] <= t ) return i;
    return 254;
}

void initGraphics( ) {
    IO::vramSetup( );

    IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
    IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    for( u8 i = 0; i < 4; ++i ) bgSetPriority( i, i );
    for( u8 i = 0; i < 4; ++i ) bgSetPriority( 4 + i, i );
    bgUpdate( );

    IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    IO::Bottom
        = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );

    IO::consoleFont->gfx                = (u16*) const_cast<unsigned int*>( consoleFontTiles );
    IO::consoleFont->pal                = (u16*) const_cast<unsigned short*>( consoleFontPal );
    IO::consoleFont->numChars           = 218;
    IO::consoleFont->numColors          = 16;
    IO::consoleFont->bpp                = 8;
    IO::consoleFont->asciiOffset        = 32;
    IO::consoleFont->convertSingleColor = false;

    consoleSetFont( &IO::Top, IO::consoleFont );
    consoleSetFont( &IO::Bottom, IO::consoleFont );
}
void initTimeAndRnd( ) {
    auto ct      = std::time( nullptr );
    auto tStruct = std::gmtime( &ct );

    if( tStruct != nullptr ) {
        SAVE::CURRENT_TIME.m_hours = tStruct->tm_hour;
        SAVE::CURRENT_TIME.m_mins  = tStruct->tm_min;
        SAVE::CURRENT_TIME.m_secs  = tStruct->tm_sec;
        SAVE::CURRENT_DATE.m_day   = tStruct->tm_mday - 1;
        SAVE::CURRENT_DATE.m_month = tStruct->tm_mon;
        SAVE::CURRENT_DATE.m_year  = tStruct->tm_year;
    }

    srand( SAVE::CURRENT_TIME.m_hours ^ ( 100 * SAVE::CURRENT_TIME.m_mins ) ^
            ( 10000 * SAVE::CURRENT_TIME.m_secs ) ^ ( SAVE::CURRENT_DATE.m_day ^
                ( 100 * SAVE::CURRENT_DATE.m_month ) ^ SAVE::CURRENT_DATE.m_year ) );
}

constexpr u8 getMonthBound( u8 p_month, u8 p_year ) {
    switch( p_month ) {
        case 0: case 2: case 4: case 6: case 7: case 9: case 11:
            return 31;
        case 3: case 5: case 8: case 10:
            return 30;
        case 1:
            if( p_year % 4 == 0 && p_year ) {
                return 29;
            } else {
                return 28;
            }
        default:
            return 0;
    }
}

void vblankIRQ( ) {
    auto ct      = std::time( nullptr );
    auto tStruct = std::gmtime( &ct );
    if( ( ++TIME_COUNT % 60 ) == 0 ) {
        if( TIME_COUNT == 120 ) {
            TIME_COUNT = 0;

            if( RTC_BAD || SAVE::CURRENT_TIME.m_secs == tStruct->tm_sec ) {
                // RTC is not usable, do it ourselves
                RTC_BAD = true;
            }
        }
        if( RTC_BAD ) {
            if( ++SAVE::CURRENT_TIME.m_secs >= 60 ) {
                SAVE::CURRENT_TIME.m_secs = 0;
                if( ++SAVE::CURRENT_TIME.m_mins >= 60 ) {
                    SAVE::CURRENT_TIME.m_mins = 0;
                    if( ++SAVE::CURRENT_TIME.m_hours >= 24 ) {
                        SAVE::CURRENT_TIME.m_hours = 0;
                        if( ++SAVE::CURRENT_DATE.m_day
                                >= getMonthBound( SAVE::CURRENT_DATE.m_month,
                                    SAVE::CURRENT_DATE.m_year ) ) {
                            SAVE::CURRENT_DATE.m_day = 0;
                            if( ++SAVE::CURRENT_DATE.m_month >= 12 ) {
                                SAVE::CURRENT_DATE.m_month = 0;
                                ++SAVE::CURRENT_DATE.m_year;
                            }
                        }
                    }
                }
            }
        }
        if( IN_GAME ) {
            SAVE::SAV.getActiveFile( ).increaseTime( );
        }
    }

    if( !RTC_BAD && tStruct != nullptr ) {
        SAVE::CURRENT_TIME.m_hours = tStruct->tm_hour;
        SAVE::CURRENT_TIME.m_mins  = tStruct->tm_min;
        SAVE::CURRENT_TIME.m_secs  = tStruct->tm_sec;
        SAVE::CURRENT_DATE.m_day   = tStruct->tm_mday - 1;
        SAVE::CURRENT_DATE.m_month = tStruct->tm_mon;
        SAVE::CURRENT_DATE.m_year  = tStruct->tm_year;
    }

    if( !ANIMATE_MAP ) return;
    scanKeys( );
    FRAME_COUNT++;
    if( ANIMATE_MAP && MAP::curMap ) MAP::curMap->animateMap( FRAME_COUNT );
}

int main( int, char** p_argv ) {
    TWL_CONFIG = ( isDSiMode( ) && ( *(u8*) 0x02000400 & 0x0F ) && ( *(u8*) 0x02000401 == 0 )
                   && ( *(u8*) 0x02000402 == 0 ) && ( *(u8*) 0x02000404 == 0 ) );

    // Init
    powerOn( POWER_ALL_2D );
    nitroFSInit( p_argv );
    ARGV = p_argv;

    irqEnable( IRQ_VBLANK );
    initGraphics( );
    initTimeAndRnd( );
    initSound( );

    keysSetRepeat( 25, 5 );
    sysSetBusOwners( true, true );

    // Read the savegame
    if( gMod == EMULATOR || ( !FS::CARD::checkCard( ) && !p_argv[ 0 ] )
        || !FS::readSave( p_argv[ 0 ] ) ) {
        SAVE::SAV.clear( );
    }

    SAVE::startScreen( ).run( );
    IO::clearScreenConsole( false, true );
    IO::clearScreen( false, true );

    FADE_TOP( );

    // TODO remove
    SAVE::SAV.getActiveFile( ).m_options.m_enableBGM = true;
    SAVE::SAV.getActiveFile( ).m_options.m_enableSFX = true;
    SAVE::SAV.m_version                              = VERSION;

    MAP::curMap = new MAP::mapDrawer( );
    MAP::curMap->registerOnLocationChangedHandler( SOUND::onLocationChange );
    MAP::curMap->registerOnMoveModeChangedHandler( SOUND::onMovementTypeChange );

    MAP::curMap->draw( );
    MAP::loadNewBank( SAVE::SAV.getActiveFile( ).m_currentMap );

    ANIMATE_MAP = true;
    NAV::draw( true );
    MAP::curMap->registerOnBankChangedHandler( NAV::showNewMap );
    MAP::curMap->registerOnLocationChangedHandler( NAV::updateMap );
    NAV::showNewMap( SAVE::SAV.getActiveFile( ).m_currentMap );

    irqSet( IRQ_VBLANK, vblankIRQ );

    IN_GAME = true;
    bool stopped = true;
    u8   bmp     = false;
    loop( ) {
        swiWaitForVBlank( );
        touchRead( &touch );
        pressed = keysUp( );
        last    = held;
        held    = keysHeld( );

#ifdef DESQUID
        if( held & KEY_L ) {
            //            time_t     unixTime   = time( NULL );
            //            struct tm* timeStruct = gmtime( (const time_t*) &unixTime );
            char buffer[ 100 ];
            snprintf( buffer, 99,
                      "Currently at %hhu-(%hx,%hx,%hhx).\nMap: %i:%i,"
                      "(%02u,%02u)\n %hhu %s (%hu) %lx %hx %hx | %hhu %hhu %hhu",
                      SAVE::SAV.getActiveFile( ).m_currentMap,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY / 32,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX / 32,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % 32,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % 32,
                      MAP::CURRENT_BANK.m_bank,
                      FS::getLocation( MAP::curMap->getCurrentLocationId( ) ).c_str( ),
                      MAP::curMap->getCurrentLocationId( ),
                      ( reinterpret_cast<u32*>( ( (u8*) &MAP::CURRENT_BANK ) + 1 ) )[ 0 ],
                      MAP::curMap
                          ->at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_bottombehave,
                      MAP::curMap
                          ->at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_topbehave,
                      SAVE::CURRENT_TIME.m_hours, SAVE::CURRENT_TIME.m_mins,
                      SAVE::CURRENT_TIME.m_secs );
            IO::messageBox m( buffer );
            NAV::draw( true );
        }
#endif

        if( held & KEY_A ) {
            for( u8 i = 0; i < 6; ++i ) {
                if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId ) break;
                auto a = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];
                if( a.isEgg( ) ) continue;
                for( u8 j = 0; j < 4; ++j )
                    for( u8 param = 0; param < 2; ++param ) {
                        if( !MOVE::isFieldMove( a.m_boxdata.m_moves[ j ] )
                            || !MOVE::possible( a.m_boxdata.m_moves[ j ], param )
                            || !MOVE::text( a.m_boxdata.m_moves[ j ], param ) )
                            continue;
                        char buffer[ 50 ];
                        auto mname = MOVE::getMoveName( a.m_boxdata.m_moves[ j ] );
                        snprintf( buffer, 49, GET_STRING( 3 ),
                                  GET_STRING( MOVE::text( a.m_boxdata.m_moves[ j ], param ) ),
                                  mname.c_str( ) );
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        IO::yesNoBox yn;
                        if( yn.getResult( buffer ) ) {
                            NAV::draw( );
                            swiWaitForVBlank( );
                            snprintf( buffer, 49, GET_STRING( 99 ), a.m_boxdata.m_name,
                                      mname.c_str( ) );
                            IO::messageBox( buffer, 0, false );
                            MAP::curMap->usePkmn( a.m_boxdata.m_speciesId, a.m_boxdata.m_isFemale,
                                                  a.m_boxdata.isShiny( ) );
                            NAV::draw( true );
                            swiWaitForVBlank( );

                            MOVE::use( a.m_boxdata.m_moves[ j ], param );
                        }
                        NAV::draw( true );
                        goto OUT;
                    }
            }
            MAP::curMap->interact( );
        OUT:
            scanKeys( );
            continue;
        }
        // Movement
        if( held & ( KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT ) ) {
            MAP::direction curDir = MAP::getDir( held );
            scanKeys( );

            stopped = false;
            if( MAP::curMap->canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, curDir,
                                      SAVE::SAV.getActiveFile( ).m_player.m_movement ) ) {
                MAP::curMap->movePlayer( curDir, ( held & KEY_B ) );
                bmp = false;
            } else if( !bmp ) {
                // Play "Bump" sound
                MAP::curMap->stopPlayer( curDir );
                SOUND::playSoundEffect( SFX_BUMP );
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
            bmp     = false;
        }

        NAV::handleInput( touch, p_argv[ 0 ] );
        // End
        scanKeys( );
    }
    delete MAP::curMap;
    return 0;
}
