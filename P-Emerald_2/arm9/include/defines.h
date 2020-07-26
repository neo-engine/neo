/*
Pokémon neo
------------------------------

file        : defines.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
#pragma once

#include <cmath>
#include <cstdio>
#include <string>
#include <nds.h>
#include <nds/ndstypes.h>

#ifdef DESQUID
#define DESQUID_STRING ( 1 << 13 )
#endif

// Assumes that the Backup is a 512k flash memory
#define BACKUP_SIZE ( 512 * 1024 )

#define MAX_ITEMS           1280
#define MAX_ITEMS_IN_BAG    MAX_ITEMS
#define MAX_ATTACK          820
#define MAX_MOVE            MAX_ATTACK
#define MAX_PKMN            893
#define OTLENGTH            8
#define SPECIES_NAMELENGTH  30
#define PKMN_NAMELENGTH     15
#define ITEM_NAMELENGTH     20
#define ITEM_DSCRLENGTH     200
#define MOVE_NAMELENGTH     20
#define MOVE_DSCRLENGTH     200
#define ABILITY_NAMELENGTH  20
#define TCLASS_NAMELENGTH   30
#define ABILITY_DSCRLENGTH  200
#define LOCATION_NAMELENGTH 25

#undef RAND_MAX
#define RAND_MAX 4294967295

enum GameMod : u8 { DEVELOPER, ALPHA, BETA, RELEASE, EMULATOR };
extern GameMod gMod;

u8 getCurrentDaytime( );

// num frames between button input
#define COOLDOWN_COUNT 6

extern bool DRAW_TIME;
extern bool UPDATE_TIME;
extern bool ANIMATE_MAP;
extern u8   FRAME_COUNT;
extern bool SCREENS_SWAPPED;
extern bool PLAYER_IS_FISHING;
extern bool INIT_NITROFS;

extern char** ARGV;

extern int           pressed, held, last;
extern touchPosition touch;
extern u8            cooldown;

extern bool TWL_CONFIG;

extern unsigned int   TEMP[ 12288 ];
extern unsigned short TEMP_PAL[ 256 ];

constexpr u8  LANGUAGES        = 2;
constexpr u16 MAX_STRINGS      = 800;
constexpr u8  MAX_ACHIEVEMENTS = 50;
constexpr u8  MAX_BADGENAMES   = 25;
constexpr u8  NUM_BGS          = 12;
constexpr u8  INITIAL_NAVBG    = NUM_BGS - 1;

constexpr u8 DAYTIME_NIGHT   = 0;
constexpr u8 DAYTIME_MORNING = 1;
constexpr u8 DAYTIME_DAY     = 2;
constexpr u8 DAYTIME_DUSK    = 3;
constexpr u8 DAYTIME_EVENING = 4;

constexpr u32 TRANSPARENCY_COEFF = 0x0671;
enum style {
    MSG_NORMAL       = 0,
    MSG_INFO         = 1,
    MSG_NOCLOSE      = 2, // Msgbox stays open, future calls to print append text
    MSG_INFO_NOCLOSE = 3,
    MSG_MART_ITEM    = 4, // Message used for items / pokemart
    MSG_ITEM         = 5, // Message used when player obtains an item
    MSG_SIGN         = 6,
};

extern const char*       LANGUAGE_NAMES[ LANGUAGES ];
extern const char* const ACHIEVEMENTS[ MAX_ACHIEVEMENTS ][ LANGUAGES ];
extern const char* const BADGENAME[ MAX_BADGENAMES ][ LANGUAGES ];
extern const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ];
extern const char* const MONTHS[ 12 ][ LANGUAGES ];
#define CURRENT_LANGUAGE SAVE::SAV.getActiveFile( ).m_options.m_language

#define getBadgeName( p_type, p_badge )                                                            \
    ( ( ( p_type ) == 0 )                                                                          \
          ? BADGENAME[ (p_badge) -1 ][ CURRENT_LANGUAGE ]                                          \
          : ( ( p_type ) == 1 ? BADGENAME[ 8 + ( ( p_badge ) / 10 - 1 ) * 2 + ( ( p_badge ) % 10 ) \
                                           - 1 ][ CURRENT_LANGUAGE ]                               \
                              : 0 ) )

#ifdef DESQUID
constexpr u16            MAX_DESQUID_STRINGS = 100;
extern const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ LANGUAGES ];
#define GET_STRING( p_stringId )                                                                 \
    ( ( ( p_stringId ) >= DESQUID_STRING ) ? DESQUID_STRINGS[ p_stringId - DESQUID_STRING ][ 0 ] \
                                           : STRINGS[ p_stringId ][ CURRENT_LANGUAGE ] )
#else
#define GET_STRING( p_stringId ) STRINGS[ p_stringId ][ CURRENT_LANGUAGE ]
#endif

#define NO_DATA         GET_STRING( 0 )
#define FARAWAY_PLACE   GET_STRING( 1 )
#define UNKNOWN_SPECIES GET_STRING( 2 )
#define POKE_NAV        GET_STRING( 8 )

#define loop( )     while( 1 )
#define TIMER_SPEED ( BUS_CLOCK / 1024 )
#define sq( a )     ( ( a ) * ( a ) )

#define IN_DEX( pidx ) \
    ( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ ( pidx ) / 8 ] & ( 1 << ( ( pidx ) % 8 ) ) )
