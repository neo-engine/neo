/*
Pokémon neo
------------------------------

file        : main.cpp
author      : Philip Wellnitz
description : Main ARM9 entry point

Copyright (C) 2012 - 2023
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
#include <nds.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "bag/item.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "fs/fs.h"
#include "io/choiceBox.h"
#include "io/keyboard.h"
#include "io/menu.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "map/mapDrawer.h"
#include "map/mapObject.h"
#include "map/mapSlice.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "save/startScreen.h"
#include "sound/sound.h"

#include "consoleFont.h"

#ifdef FLASHCARD
GameMod gMod = GameMod::FCARD;
#elif DESQUID
GameMod gMod = GameMod::DEVELOPER;
#else
GameMod gMod = GameMod::RELEASE;
#endif

fsdataInfo FSDATA;

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
bool          HAD_NEW_GAME      = false;
bool          RESET_GAME        = false;

char** ARGV;

bool nitroFSInit( char** p_basepath = nullptr );

u8 getCurrentDaytime( ) {
    u8 t = SAVE::CURRENT_TIME.m_hours, m = SAVE::CURRENT_DATE.m_month;

    for( u8 i = 0; i < 5; ++i )
        if( DAY_TIMES[ m / 4 ][ i ] >= t ) return i;
    return 2;
}

void initGraphics( ) {
    IO::vramSetup( );

    IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
    IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    for( u8 i = 0; i < 4; ++i ) { bgSetPriority( i, i ); }
    for( u8 i = 0; i < 4; ++i ) { bgSetPriority( 4 + i, i ); }
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

    srand( SAVE::CURRENT_TIME.m_hours ^ ( 100 * SAVE::CURRENT_TIME.m_mins )
           ^ ( 10000 * SAVE::CURRENT_TIME.m_secs )
           ^ ( SAVE::CURRENT_DATE.m_day ^ ( 100 * SAVE::CURRENT_DATE.m_month )
               ^ SAVE::CURRENT_DATE.m_year ) );
}

constexpr u8 getMonthBound( u8 p_month, u8 p_year ) {
    switch( p_month ) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 7:
    case 9:
    case 11: return 31;
    case 3:
    case 5:
    case 8:
    case 10: return 30;
    case 1:
        if( p_year % 4 == 0 && p_year ) {
            return 29;
        } else {
            return 28;
        }
    default: return 0;
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
                        if( ++SAVE::CURRENT_DATE.m_day >= getMonthBound(
                                SAVE::CURRENT_DATE.m_month, SAVE::CURRENT_DATE.m_year ) ) {
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
        if( IN_GAME ) { SAVE::SAV.getActiveFile( ).increaseTime( ); }
    }

    if( !RTC_BAD && tStruct != nullptr ) {
        SAVE::CURRENT_TIME.m_hours = tStruct->tm_hour;
        SAVE::CURRENT_TIME.m_mins  = tStruct->tm_min;
        SAVE::CURRENT_TIME.m_secs  = tStruct->tm_sec;
        SAVE::CURRENT_DATE.m_day   = tStruct->tm_mday - 1;
        SAVE::CURRENT_DATE.m_month = tStruct->tm_mon;
        SAVE::CURRENT_DATE.m_year  = tStruct->tm_year;
    }

    if( !ANIMATE_MAP ) {
        return;
    } else {
        if( IO::LOCATION_TIMER && !--IO::LOCATION_TIMER ) {
            IO::hideLocation( );
        } else if( IO::LOCATION_TIMER > 0 && IO::LOCATION_TIMER < 16 ) {
            IO::hideLocation( IO::LOCATION_TIMER );
        }
    }
    FRAME_COUNT++;
    if( ANIMATE_MAP && MAP::curMap ) MAP::curMap->animateMap( FRAME_COUNT );
}


int main( int, char** p_argv ) {
START:
    // TWL_CONFIG = ( isDSiMode( ) && ( *(u8*) 0x02000400 & 0x0F ) && ( *(u8*) 0x02000401 == 0 )
    //                && ( *(u8*) 0x02000402 == 0 ) && ( *(u8*) 0x02000404 == 0 ) );

    // Init
    powerOn( POWER_ALL_2D );
    nitroFSInit( p_argv );
    ARGV = p_argv;

    // keysSetRepeat( 25, 5 );
    // sysSetBusOwners( true, true );

    // irqEnable( IRQ_VBLANK );
    initGraphics( );
#ifdef DESQUID
    printf( "\n\nBooting NEO. %u %d\n"
            "- Init Graphics     [ OK ]\n"
            "- Init FSROOT       ",
            sizeof( boxPokemon ), sizeof( SAVE::saveGame::playerInfo ) );
#endif
#ifdef DESQUID
    if( FS::readFsInfo( ) ) {
        printf( "[ OK ]\n" );
    } else {
        printf( "[FAIL]\n" );
    }
    printf( "- Init time and RND " );
#else
    FS::readFsInfo( );
#endif
    initTimeAndRnd( );
#ifdef DESQUID
    printf( "[ OK ]\n- Init sound        " );
#endif
    initSound( );
#ifdef DESQUID
    printf( "[ OK ]\n- Init map          " );
#endif

    MAP::curMap = new MAP::mapDrawer( );
#ifdef DESQUID
    printf( "[ OK ]\n- Loading SAV       " );
#endif

    // Read the savegame
    // TODO
    if( true ||  gMod == EMULATOR || ( !FS::CARD::checkCard( ) && !p_argv[ 0 ] )
        || !FS::readSave( p_argv[ 0 ] ) ) {
#ifdef DESQUID
        printf( "[FAIL]\n- Creating new SAV  " );
#endif
        std::memset( &SAVE::SAV, 0, sizeof( SAVE::saveGame ) );
        SAVE::SAV.clear( );
    }

#ifdef DESQUID
    printf( "[ OK ]\nALL GOOD!" );
#endif

    SAVE::startScreen( ).run( );
    IO::clearScreenConsole( false, true );
    IO::clearScreen( false, true );

    FADE_TOP( );
    SOUND::stopBGM( );

    ANIMATE_MAP = false;
    // Reset infinity cave on reload
    SAVE::SAV.getActiveFile( ).infinityCaveCurrentLayer( ) = 0;

    MAP::curMap->registerOnLocationChangedHandler( SOUND::onLocationChange );
    MAP::curMap->registerOnMoveModeChangedHandler( SOUND::onMovementTypeChange );
    MAP::curMap->registerOnWeatherChangedHandler( SOUND::onWeatherChange );

    IO::init( );
    //    MAP::curMap->registerOnBankChangedHandler( IO::showNewMap );
    MAP::curMap->registerOnLocationChangedHandler( IO::showNewLocation );
    MAP::curMap->draw( OBJPRIORITY_2, false, HAD_NEW_GAME );

    // auto curLoc = MAP::curMap->getCurrentLocationId( );
    // SOUND::onLocationChange( curLoc, false );
    // for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }
    // IO::showNewLocation( curLoc, false );

    irqSet( IRQ_VBLANK, vblankIRQ );
    ANIMATE_MAP = true;

    IN_GAME      = true;
    bool stopped = true;
    u8   bmp     = false;
    cooldown     = COOLDOWN_COUNT;
    u8 heldcnt   = 0;
    loop( ) {
        if( RESET_GAME ) { break; }
        scanKeys( );
        touchRead( &touch );
        swiWaitForVBlank( );
        pressed = keysUp( );
        last    = held;
        held    = keysHeld( );
#ifdef DESQUID
        if( held & KEY_L ) {

            //            time_t     unixTime   = time( NULL );
            //            struct tm* timeStruct = gmtime( (const time_t*) &unixTime );
            char buffer[ 100 ];
            snprintf( buffer, 99,
                      "POS %hhu-(%hx,%hx,%hhx). %i:%i, (%02u,%02u)\n"
                      "S-Rou %hhu | %6s (%hu) | %hx %hx | TM %hhu %02hhu :%02hhu ",
                      SAVE::SAV.getActiveFile( ).m_currentMap,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY / 32,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX / 32,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % 32,
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % 32,
                      SAVE::SAV.getActiveFile( ).m_route,
                      FS::getLocation( MAP::curMap->getCurrentLocationId( ) ).c_str( ),
                      MAP::curMap->getCurrentLocationId( ),
                      MAP::curMap
                          ->at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_bottombehave,
                      MAP::curMap
                          ->at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_topbehave,
                      getCurrentDaytime( ), SAVE::CURRENT_TIME.m_hours, SAVE::CURRENT_TIME.m_mins );
            IO::printMessage( buffer );
        }
#endif

        if( GET_AND_WAIT( KEY_A ) ) {
            if( MAP::curMap->currentPosAllowsDirectFieldMove( ) ) {
                for( u8 i = 0; i < 6; ++i ) {
                    if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId ) break;
                    auto a = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];
                    if( a.isEgg( ) ) continue;
                    for( u8 j = 0; j < 4; ++j )
                        for( u8 param = 0; param < 2; ++param ) {
                            if( !BATTLE::isFieldMove( a.m_boxdata.m_moves[ j ] )
                                || !BATTLE::possible( a.m_boxdata.m_moves[ j ], param )
                                || !BATTLE::text( a.m_boxdata.m_moves[ j ], param ) )
                                continue;
                            char buffer[ 100 ];
                            auto mname = FS::getMoveName( a.m_boxdata.m_moves[ j ] );
                            auto fstr  = std::string( GET_STRING( 3 ) );
                            snprintf( buffer, 99, fstr.c_str( ),
                                      GET_STRING( BATTLE::text( a.m_boxdata.m_moves[ j ], param ) ),
                                      mname.c_str( ) );
                            SOUND::playSoundEffect( SFX_CHOOSE );
                            IO::yesNoBox yn;
                            if( yn.getResult( buffer, MSG_NOCLOSE ) == IO::yesNoBox::YES ) {
                                IO::init( );
                                IO::printMessage( 0, MSG_NOCLOSE );
                                swiWaitForVBlank( );
                                snprintf( buffer, 99, GET_STRING( 99 ), a.m_boxdata.m_name,
                                          mname.c_str( ) );
                                IO::printMessage( buffer, MSG_NORMAL );
                                IO::printMessage( 0, MSG_NOCLOSE );
                                IO::init( );
                                if( i || !MAP::curMap->useFollowPkmn( ) ) {
                                    MAP::curMap->usePkmn( a.getSpriteInfo( ) );
                                    swiWaitForVBlank( );
                                    BATTLE::use( a.m_boxdata.m_moves[ j ], param );
                                } else {
                                    swiWaitForVBlank( );

                                    auto d = MAP::curMap->getFollowPkmnDirection( );

                                    if( a.m_boxdata.m_moves[ j ] == M_CUT
                                        || a.m_boxdata.m_moves[ j ] == M_ROCK_SMASH ) {
                                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                                            += MAP::dir[ d ][ 0 ];
                                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                                            += MAP::dir[ d ][ 1 ];
                                    }

                                    BATTLE::use( a.m_boxdata.m_moves[ j ], 2 );

                                    if( a.m_boxdata.m_moves[ j ] == M_CUT
                                        || a.m_boxdata.m_moves[ j ] == M_ROCK_SMASH ) {
                                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                                            -= MAP::dir[ d ][ 0 ];
                                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                                            -= MAP::dir[ d ][ 1 ];
                                    }
                                }
                            } else {
                                IO::printMessage( 0, MSG_NOCLOSE );
                                IO::init( );
                            }
                            goto OUT;
                        }
                }
            }
            MAP::curMap->interact( );
        OUT:
            scanKeys( );
            continue;
        }

        IO::handleInput( p_argv[ 0 ] );

        // Movement
        if( held & ( KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT ) ) {
            MAP::direction curDir = MAP::getDir( held );
            scanKeys( );

            if( heldcnt < 5 && stopped ) {
                if( !heldcnt ) { MAP::curMap->redirectPlayer( curDir, false ); }
                ++heldcnt;
                continue;
            }

            stopped = false;
            if( MAP::curMap->canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, curDir,
                                      SAVE::SAV.getActiveFile( ).m_player.m_movement ) ) {
                MAP::curMap->allowFollowPokemon( );
                MAP::curMap->movePlayer( curDir, ( held & KEY_B ) );
                bmp = false;
            } else if( !bmp ) {
                // Play "Bump" sound
                SOUND::playSoundEffect( SFX_BUMP );
                swiWaitForVBlank( );
                MAP::curMap->stopPlayer( curDir );
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
            heldcnt = 0;
        }

        // tamatama play cry of wild pkmn

        if( FRAME_COUNT == 72 && rand( ) % 100 < 10 ) {
            u8 cr = rand( ) % MAP::MAX_PKMN_PER_SLICE;
            if( MAP::curMap->currentData( ).m_pokemon[ cr ].m_speciesId ) {
                SOUND::playCry( MAP::curMap->currentData( ).m_pokemon[ cr ].m_speciesId );
            }
        }

        // End
    }
    delete MAP::curMap;

    if( RESET_GAME ) {
        SOUND::stopBGM( );
        RESET_GAME = false;
        goto START;
    }

    return 0;
}
