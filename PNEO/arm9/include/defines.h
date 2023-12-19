/*
Pokémon neo
------------------------------

file        : defines.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2022
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
#pragma once

#include <cmath>
#include <cstdio>
#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#define WAIT( p_time )                                                               \
    do {                                                                             \
        for( size_t __wait = 0; __wait < p_time; ++__wait ) { swiWaitForVBlank( ); } \
    } while( false )
#define HALF_SEC          30
#define THREE_QUARTER_SEC 45
#define FULL_SEC          60
#define THREEHALF_SEC     90
#define DOUBLE_SEC        120

#ifdef DESQUID
#define DESQUID_LOG( p_msg ) IO::printMessage( p_msg )
#else
#define DESQUID_LOG( p_msg ) \
    {}
#endif

struct fsdataInfo {
    static constexpr u8 MAX_OW_MAPS = 8;

    u16 m_maxPkmn = 0;
    u16 m_maxItem = 0;

    u16 m_maxMove   = 0;  // valid move idx 0..m_maxMove
    u8  m_languages = 2;  // num languages
    u8  m_fileSplit = 30; // max num files per dir

    u8 m_maxNavBG     = 0; // valid subscrn wallpaper 0..m_maxNavBg
    u8 m_defaultNavBG = 0;
    u8 m_owMapCount   = 1;
    u8 m_defaultOWMap = 0;

    u8 m_owMaps[ MAX_OW_MAPS ] = { 10 };

    inline bool isOWMap( u8 p_map ) {
        for( u8 i = 0; i < m_owMapCount; ++i ) {
            if( m_owMaps[ i ] == p_map ) { return true; }
        }
        return false;
    }
};

extern fsdataInfo FSDATA;

// Assumes that the Backup is a 512k flash memory
constexpr u32 BACKUP_SIZE = ( 512 * 1024 );

#define MAX_ITEMS      FSDATA.m_maxItem
#define MAX_ATTACK     FSDATA.m_maxMove
#define MAX_MOVE       FSDATA.m_maxMove
#define MAX_PKMN       FSDATA.m_maxPkmn
#define ITEMS_PER_DIR  FSDATA.m_fileSplit
#define MAX_NAV_BG     FSDATA.m_maxNavBG
#define NUM_BGS        ( MAX_NAV_BG - 1 )
#define DEFAULT_NAV_BG FSDATA.m_defaultNavBG
#define INITIAL_NAVBG  DEFAULT_NAV_BG
#define LANGUAGES      FSDATA.m_languages

constexpr u8 MAX_PKMN_LEVEL      = 100;
constexpr u8 MIN_OPP_LEVEL       = 5;
constexpr u8 DIFF_HARD_LEVEL_INC = 8;
constexpr u8 DIFF_EASY_LEVEL_DEC = 3;

constexpr u16 MAX_ITEMS_IN_BAG = 1280;
constexpr u16 MAX_LANGUAGES    = 6; // max number of different languages supported

constexpr u16 OTLENGTH            = 8;
constexpr u16 SPECIES_NAMELENGTH  = 30;
constexpr u16 PKMN_NAMELENGTH     = 14;
constexpr u16 FORME_NAMELENGTH    = 30;
constexpr u16 ITEM_NAMELENGTH     = 20;
constexpr u16 ITEM_DSCRLENGTH     = 200;
constexpr u16 MOVE_NAMELENGTH     = 20;
constexpr u16 MOVE_DSCRLENGTH     = 200;
constexpr u16 ABILITY_NAMELENGTH  = 20;
constexpr u16 TCLASS_NAMELENGTH   = 30;
constexpr u16 ABILITY_DSCRLENGTH  = 200;
constexpr u16 LOCATION_NAMELENGTH = 25;
constexpr u16 BGM_NAMELENGTH      = 25;
constexpr u16 DEXENTRY_NAMELENGTH = 200;

constexpr u16 UISTRING_LEN    = 250;
constexpr u16 MAPSTRING_LEN   = 800;
constexpr u16 TRADESTRING_LEN = 12;
constexpr u16 RIBBONNAME_LEN  = 50;
constexpr u16 RIBBONDSCR_LEN  = 250;
constexpr u16 BADGENAME_LEN   = 50;
constexpr u16 ACHIEVEMENT_LEN = 100;
constexpr u16 PKMNPHRS_LEN    = 150;
constexpr u16 TRAINERNAME_LEN = 16;
constexpr u16 TRAINERMSG_LEN  = 200;

#undef RAND_MAX
#define RAND_MAX 4294967295

enum GameMod : u8 { DEVELOPER, ALPHA, BETA, RELEASE, EMULATOR, FCARD };
extern GameMod gMod;

constexpr u8 DAY_TIMES[ 4 ][ 5 ]
    = { { 7, 10, 15, 17, 23 }, { 6, 9, 12, 18, 23 }, { 5, 8, 10, 20, 23 }, { 7, 9, 13, 19, 23 } };

u8 getCurrentDaytime( );

// num frames between button input
constexpr u8 COOLDOWN_COUNT = 6;

extern bool DRAW_TIME;
extern bool UPDATE_TIME;
extern bool ANIMATE_MAP;
extern u8   FRAME_COUNT;
extern bool SCREENS_SWAPPED;
extern bool PLAYER_IS_FISHING;
extern bool INIT_NITROFS;
extern bool HAD_NEW_GAME;
extern bool RESET_GAME;

extern char** ARGV;

extern int           pressed, held, last;
extern touchPosition touch;
extern u8            cooldown;

extern bool TWL_CONFIG;

extern unsigned int   TEMP[ 256 * 256 / 4 ];
extern unsigned short TEMP_PAL[ 256 ];

constexpr u8 DAYTIMES        = 5;
constexpr u8 DAYTIME_NIGHT   = 0;
constexpr u8 DAYTIME_MORNING = 1;
constexpr u8 DAYTIME_DAY     = 2;
constexpr u8 DAYTIME_DUSK    = 3;
constexpr u8 DAYTIME_EVENING = 4;

constexpr u32 TRANSPARENCY_COEFF = 0x0671;
enum style : u8 {
    MSG_NORMAL          = 0,
    MSG_INFO            = 1,
    MSG_NOCLOSE         = 2, // Msgbox stays open, future calls to print append text
    MSG_INFO_NOCLOSE    = 3,
    MSG_MART_ITEM       = 4, // Message used for items / pokemart
    MSG_ITEM            = 5, // Message used when player obtains an item
    MSG_SIGN            = 6,
    MSG_NORMAL_CONT     = 7,
    MSG_INFO_CONT       = 8,
    MSG_BRAILLE         = 9,
    MSG_SHOW_MONEY_FLAG = 128, // show the current money
};

struct pkmnSpriteInfo {
    u16 m_pkmnIdx;
    u8  m_forme;
    u8  m_female;
    u8  m_shiny;
    u8  m_flipX;
    u32 m_pid; // used for spinda and pkmn recolor
};

constexpr u32 DEFAULT_SPRITE_PID = 0x88888888;

#define loop( )     while( 1 )
#define TIMER_SPEED ( BUS_CLOCK / 1024 )
#define sq( a )     ( ( a ) * ( a ) )

#define IN_DEX( pidx ) \
    ( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ ( pidx ) / 8 ] & ( 1 << ( ( pidx ) % 8 ) ) )
