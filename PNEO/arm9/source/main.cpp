/*
Pokémon neo
------------------------------

file        : main.cpp
author      : Philip Wellnitz
description : Main ARM9 entry point

Copyright (C) 2012 - 2026
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
#include "io/keyboard.h"
#include "io/menu.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/simpleWidget.h"
#include "io/sprite.h"
#include "io/util.h"
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

time_t unixTime;
bool   DRAW_TIME         = false;
bool   UPDATE_TIME       = true;
bool   ANIMATE_MAP       = false;
u8     FRAME_COUNT       = 0;
u8     TIME_COUNT        = 0;
bool   SCREENS_SWAPPED   = false;
bool   PLAYER_IS_FISHING = false;
bool   INIT_NITROFS      = false;
bool   TWL_CONFIG        = false;
bool   IN_GAME           = false;
bool   RTC_BAD           = false;
bool   HAD_NEW_GAME      = false;
bool   RESET_GAME        = false;
bool   FLUSH_GFX_UPDATE  = false;

static volatile bool RTC_POLL_PENDING = false;

char** ARGV;

bool nitroFSInit( char** p_basepath = nullptr );

u8 getCurrentDaytime( ) {
    u8 t = SAVE::CURRENT_TIME.m_hours, m = SAVE::CURRENT_DATE.m_month;

    for( u8 i = 0; i < 5; ++i ) {
        if( DAY_TIMES[ m / 4 ][ i ] >= t ) { return i; }
    }
    return 2;
}

void initGraphics( ) {
    GFX_GUARD( gfGuard );
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
    if( ( ++TIME_COUNT % 60 ) == 0 ) {
        if( TIME_COUNT == 120 ) {
            TIME_COUNT       = 0;
            RTC_POLL_PENDING = true;
        }
        if( IN_GAME ) { SAVE::CURRENT_FILE->increaseTime( ); }
    }

    if( ANIMATE_MAP ) {
        FRAME_COUNT++;
        if( MAP::curMap ) { MAP::curMap->animateMap( FRAME_COUNT ); }
        if( IO::LOCATION_TIMER && !--IO::LOCATION_TIMER ) {
            IO::hideLocation( );
        } else if( IO::LOCATION_TIMER > 0 && IO::LOCATION_TIMER < 16 ) {
            IO::hideLocation( IO::LOCATION_TIMER );
        }
    }

    if( FLUSH_GFX_UPDATE ) {
        if( IO::LOCATION_TIMER ) {
            IO::commitOAMbgUpdate( false ); // commits bgUpdate in sync with OAM update
            IO::commitOAM( true );
        } else {
            bgUpdate( );
            IO::commitOAM( false );
            IO::commitOAM( true );
        }
    }
}

void pollRTC( ) {
    if( !RTC_POLL_PENDING ) { return; }
    RTC_POLL_PENDING = false;

    auto ct      = std::time( nullptr );
    auto tStruct = std::gmtime( &ct );

    if( tStruct == nullptr ) { return; }

    static u8 prev = 255;
    if( prev != 255 && tStruct->tm_sec == prev ) { RTC_BAD = true; }
    prev = tStruct->tm_sec;

    if( RTC_BAD ) {
        // Advance internal clock manually
        if( ++SAVE::CURRENT_TIME.m_secs >= 60 ) {
            SAVE::CURRENT_TIME.m_secs = 0;
            if( ++SAVE::CURRENT_TIME.m_mins >= 60 ) {
                SAVE::CURRENT_TIME.m_mins = 0;
                if( ++SAVE::CURRENT_TIME.m_hours >= 24 ) {
                    SAVE::CURRENT_TIME.m_hours = 0;
                    if( ++SAVE::CURRENT_DATE.m_day >= getMonthBound( SAVE::CURRENT_DATE.m_month,
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
    } else {
        SAVE::CURRENT_TIME.m_hours = tStruct->tm_hour;
        SAVE::CURRENT_TIME.m_mins  = tStruct->tm_min;
        SAVE::CURRENT_TIME.m_secs  = tStruct->tm_sec;
        SAVE::CURRENT_DATE.m_day   = tStruct->tm_mday - 1;
        SAVE::CURRENT_DATE.m_month = tStruct->tm_mon;
        SAVE::CURRENT_DATE.m_year  = tStruct->tm_year;
    }
}

int main( int, char** p_argv ) {
START:
    // TWL_CONFIG = ( isDSiMode( ) && ( *(u8*) 0x02000400 & 0x0F ) && ( *(u8*) 0x02000401 == 0 )
    //                && ( *(u8*) 0x02000402 == 0 ) && ( *(u8*) 0x02000404 == 0 ) );

    // Init
    powerOn( POWER_ALL_2D );
    nitroFSInit( p_argv );
    ARGV        = p_argv;
    ANIMATE_MAP = true;

    // keysSetRepeat( 25, 5 );
    // sysSetBusOwners( true, true );

    irqSet( IRQ_VBLANK, vblankIRQ );
    irqEnable( IRQ_VBLANK );
    initGraphics( );
    FLUSH_GFX_UPDATE = true;

    {
        AM_GUARD_UNAWARE( amG );

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
        if( gMod == EMULATOR || ( !FS::CARD::checkCard( ) && !p_argv[ 0 ] )
            || !FS::readSave( p_argv[ 0 ] ) ) {
#ifdef DESQUID
            printf( "[FAIL]\n- Creating new SAV  " );
#endif
            std::memset( &SAVE::SAV, 0, sizeof( SAVE::saveGame ) );
            SAVE::SAV.clear( );
        }
        SAVE::CURRENT_FILE = &SAVE::SAV.getActiveFile( );

#ifdef DESQUID
        printf( "[ OK ]\nALL GOOD!" );
#endif
        SAVE::startScreen( ).run( );
        IO::clearScreenConsole( false, true );
        IO::clearScreen( false, true );

        FADE_TOP( );
        SOUND::stopBGM( );

        // Reset infinity cave on reload
        SAVE::CURRENT_FILE->infinityCaveCurrentLayer( ) = 0;

        IO::init( );
        MAP::curMap->registerOnLocationChangedHandler( SOUND::onLocationChange );
        MAP::curMap->registerOnMoveModeChangedHandler( SOUND::onMovementTypeChange );
        MAP::curMap->registerOnWeatherChangedHandler( SOUND::onWeatherChange );
        //    MAP::curMap->registerOnBankChangedHandler( IO::showNewMap );
        MAP::curMap->registerOnLocationChangedHandler( IO::showNewLocation );
        MAP::curMap->draw( OBJPRIORITY_2, false, HAD_NEW_GAME );
    }

    IN_GAME          = true;
    bool stopped     = true;
    u8   bmp         = 0;
    IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
    u8 heldcnt       = 0;
    loop( ) {
        if( RESET_GAME ) { break; }
        pollRTC( );
        scanKeys( );
        touchRead( &IO::TOUCH );
        swiWaitForVBlank( );
        IO::BTN_PRESSED = keysUp( );
        IO::BTN_LAST    = IO::BTN_HELD;
        IO::BTN_HELD    = keysHeld( );
#ifdef DESQUID
        if( IO::BTN_HELD & KEY_L ) {

            //            time_t     unixTime   = time( NULL );
            //            struct tm* timeStruct = gmtime( (const time_t*) &unixTime );
            std::array<char, 100> buffer{ };
            snprintf( buffer.data( ), buffer.size( ),
                      "POS %hhu-(%hx,%hx,%hhx). %i:%i, (%02u,%02u)\n"
                      "S-Rou %hhu | %6s (%hu) | %hx %hx | TM %hhu %02hhu :%02hhu\nANIMATE_MAP %u",
                      SAVE::CURRENT_FILE->m_currentMap, SAVE::CURRENT_FILE->m_player.m_pos.m_posX,
                      SAVE::CURRENT_FILE->m_player.m_pos.m_posY,
                      SAVE::CURRENT_FILE->m_player.m_pos.m_posZ,
                      SAVE::CURRENT_FILE->m_player.m_pos.m_posY / 32,
                      SAVE::CURRENT_FILE->m_player.m_pos.m_posX / 32,
                      SAVE::CURRENT_FILE->m_player.m_pos.m_posX % 32,
                      SAVE::CURRENT_FILE->m_player.m_pos.m_posY % 32, SAVE::CURRENT_FILE->m_route,
                      FS::getLocation( MAP::curMap->getCurrentLocationId( ) ).c_str( ),
                      MAP::curMap->getCurrentLocationId( ),
                      MAP::curMap
                          ->at( SAVE::CURRENT_FILE->m_player.m_pos.m_posX,
                                SAVE::CURRENT_FILE->m_player.m_pos.m_posY )
                          .m_bottombehave,
                      MAP::curMap
                          ->at( SAVE::CURRENT_FILE->m_player.m_pos.m_posX,
                                SAVE::CURRENT_FILE->m_player.m_pos.m_posY )
                          .m_topbehave,
                      getCurrentDaytime( ), SAVE::CURRENT_TIME.m_hours, SAVE::CURRENT_TIME.m_mins,
                      ANIMATE_MAP );
            IO::printMessage( buffer.data( ) );
        }
#endif

        if( GET_AND_WAIT( KEY_A ) ) {
            if( MAP::curMap->currentPosAllowsDirectFieldMove( ) ) {
                for( u8 i = 0; i < 6; ++i ) {
                    if( !SAVE::CURRENT_FILE->m_pkmnTeam[ i ].m_boxdata.m_speciesId ) break;
                    auto a = SAVE::CURRENT_FILE->m_pkmnTeam[ i ];
                    if( a.isEgg( ) ) continue;
                    for( u8 j = 0; j < 4; ++j )
                        for( u8 param = 0; param < 2; ++param ) {
                            if( !BATTLE::isFieldMove( a.m_boxdata.m_moves[ j ] )
                                || !BATTLE::possible( a.m_boxdata.m_moves[ j ], param )
                                || !BATTLE::text( a.m_boxdata.m_moves[ j ], param ) )
                                continue;
                            std::array<char, 100> buffer{ };
                            auto mname = FS::getMoveName( a.m_boxdata.m_moves[ j ] );
                            auto fstr  = std::string( GET_STRING( 3 ) );
                            snprintf( buffer.data( ), buffer.size( ), fstr.c_str( ),
                                      GET_STRING( BATTLE::text( a.m_boxdata.m_moves[ j ], param ) ),
                                      mname.c_str( ) );
                            SOUND::playSoundEffect( SFX_CHOOSE );
                            IO::simpleYesNoBox yn;
                            if( yn.getResult( buffer.data( ), MSG_NOCLOSE ) == IO::yesNoBox::YES ) {
                                IO::init( );
                                IO::printMessage( 0, MSG_NOCLOSE );
                                swiWaitForVBlank( );
                                snprintf( buffer.data( ), buffer.size( ), GET_STRING( 99 ),
                                          a.m_boxdata.m_name, mname.c_str( ) );
                                IO::printMessage( buffer.data( ), MSG_NORMAL );
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
                                        SAVE::CURRENT_FILE->m_player.m_pos.m_posX
                                            += MAP::dir[ d ][ 0 ];
                                        SAVE::CURRENT_FILE->m_player.m_pos.m_posY
                                            += MAP::dir[ d ][ 1 ];
                                    }

                                    BATTLE::use( a.m_boxdata.m_moves[ j ], 2 );

                                    if( a.m_boxdata.m_moves[ j ] == M_CUT
                                        || a.m_boxdata.m_moves[ j ] == M_ROCK_SMASH ) {
                                        SAVE::CURRENT_FILE->m_player.m_pos.m_posX
                                            -= MAP::dir[ d ][ 0 ];
                                        SAVE::CURRENT_FILE->m_player.m_pos.m_posY
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
        if( IO::BTN_HELD & ( KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT ) ) {
            MAP::direction curDir = MAP::getDir( IO::BTN_HELD );
            scanKeys( );

            if( heldcnt < 5 && stopped ) {
                if( !heldcnt ) { MAP::curMap->redirectPlayer( curDir, false ); }
                ++heldcnt;
                continue;
            }

            stopped = false;
            if( MAP::curMap->canMove( SAVE::CURRENT_FILE->m_player.m_pos, curDir,
                                      SAVE::CURRENT_FILE->m_player.m_movement ) ) {
                MAP::curMap->allowFollowPokemon( );
                MAP::curMap->movePlayer( curDir, ( IO::BTN_HELD & KEY_B ) );
                bmp = 0;
            } else if( !bmp ) {
                // Play "Bump" sound
                SOUND::playSoundEffect( SFX_BUMP );
                swiWaitForVBlank( );
                MAP::curMap->stopPlayer( curDir );
                bmp = 1;
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
            bmp     = 0;
            heldcnt = 0;
        }

        // play cry of wild pkmn from time to time
        if( FRAME_COUNT == 72 && rand( ) % 100 < 10 && MAP::MAX_PKMN_PER_SLICE ) {
            u8 cr = rand( ) % MAP::MAX_PKMN_PER_SLICE;
            if( MAP::curMap->currentData( ).m_pokemon[ cr ].m_speciesId ) {
                SOUND::playCry( MAP::curMap->currentData( ).m_pokemon[ cr ].m_speciesId );
            }
        }

        // End
    }

    ANIMATE_MAP = false;
    swiWaitForVBlank( );
    delete MAP::curMap;
    MAP::curMap = nullptr;

    if( RESET_GAME ) {
        SOUND::stopBGM( );
        RESET_GAME       = false;
        RTC_POLL_PENDING = false;
        goto START;
    }

    return 0;
}
