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

#ifdef DESQUID
#define DESQUID_LOG( p_msg ) NAV::printMessage( p_msg )
#else
#define DESQUID_LOG( p_msg ) \
    {}
#endif

#ifdef DESQUID
constexpr u16 DESQUID_STRING = ( 1 << 13 );
#endif

constexpr u16 MAP_STRING = ( 1 << 11 );

// Assumes that the Backup is a 512k flash memory
constexpr u32 BACKUP_SIZE = ( 512 * 1024 );

constexpr u16 MAX_ITEMS           = 1300;
constexpr u16 MAX_ITEMS_IN_BAG    = 1280;
constexpr u16 MAX_ATTACK          = 830;
constexpr u16 MAX_MOVE            = MAX_ATTACK;
constexpr u16 MAX_PKMN            = 905;
constexpr u16 OTLENGTH            = 8;
constexpr u16 SPECIES_NAMELENGTH  = 30;
constexpr u16 PKMN_NAMELENGTH     = 15;
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
constexpr u16 BADGENAME_LEN   = 50;
constexpr u16 ACHIEVEMENT_LEN = 100;
constexpr u16 PKMNPHRS_LEN    = 150;

#undef RAND_MAX
#define RAND_MAX 4294967295

enum GameMod : u8 { DEVELOPER, ALPHA, BETA, RELEASE, EMULATOR, FCARD };
extern GameMod gMod;

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

extern char** ARGV;

extern int           pressed, held, last;
extern touchPosition touch;
extern u8            cooldown;

extern bool TWL_CONFIG;

extern unsigned int   TEMP[ 256 * 256 / 4 ];
extern unsigned short TEMP_PAL[ 256 ];

constexpr u8 LANGUAGES      = 2;
constexpr u8 NUM_BGS        = 12;
constexpr u8 MAX_NAV_BG     = 13;
constexpr u8 DEFAULT_NAV_BG = 5;
constexpr u8 INITIAL_NAVBG  = DEFAULT_NAV_BG;

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
    MSG_SHOW_MONEY_FLAG = 128, // show the current money
};

struct pkmnSpriteInfo {
    u16 m_pkmnIdx;
    u8  m_forme;
    u8  m_female;
    u8  m_shiny;
    u8  m_flipX;
};

extern const char*       LANGUAGE_NAMES[ LANGUAGES ];
extern const char*       HP_ICONS[ LANGUAGES ];
extern const char* const MONTHS[ 12 ][ LANGUAGES ];
#define CURRENT_LANGUAGE SAVE::SAV.getActiveFile( ).m_options.m_language

bool getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language, char* p_out );
std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language );
std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId );

const char* getUIString( u16 p_stringId, u8 p_language );
const char* getMapString( u16 p_stringId );
const char* getPkmnPhrase( u16 p_stringId );
const char* getBadge( u16 p_badgeId );
const char* getAchievement( u16 p_badgeId, u8 p_language );

#define getBadgeName( p_type, p_badge )                                                     \
    ( ( ( p_type ) == 0 )                                                                   \
          ? getBadge( (p_badge) -1 )                                                        \
          : ( ( p_type ) == 1                                                               \
                  ? getBadge( 8 + ( ( p_badge ) / 10 - 1 ) * 2 + ( ( p_badge ) % 10 ) - 1 ) \
                  : nullptr ) )

#define GET_MAP_STRING( p_stringId )           getMapString( p_stringId )
#define GET_STRING_L( p_stringId, p_language ) getUIString( p_stringId, p_language )

#ifdef DESQUID
constexpr u16            MAX_DESQUID_STRINGS = 100;
extern const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ LANGUAGES ];
#define GET_STRING( p_stringId )                                                       \
    ( ( ( p_stringId ) >= DESQUID_STRING )                                             \
          ? DESQUID_STRINGS[ p_stringId - DESQUID_STRING ][ 0 ]                        \
          : ( ( p_stringId ) >= MAP_STRING ? GET_MAP_STRING( p_stringId - MAP_STRING ) \
                                           : GET_STRING_L( p_stringId, CURRENT_LANGUAGE ) ) )
#else
#define GET_STRING( p_stringId )                                               \
    ( ( p_stringId ) >= MAP_STRING ? GET_MAP_STRING( p_stringId - MAP_STRING ) \
                                   : GET_STRING_L( p_stringId, CURRENT_LANGUAGE ) )
#endif
#define HP_ICON HP_ICONS[ CURRENT_LANGUAGE ]

#define NO_DATA         GET_STRING( 0 )
#define FARAWAY_PLACE   GET_STRING( 1 )
#define UNKNOWN_SPECIES GET_STRING( 2 )
#define POKE_NAV        GET_STRING( 8 )

#define loop( )     while( 1 )
#define TIMER_SPEED ( BUS_CLOCK / 1024 )
#define sq( a )     ( ( a ) * ( a ) )

#define IN_DEX( pidx ) \
    ( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ ( pidx ) / 8 ] & ( 1 << ( ( pidx ) % 8 ) ) )
