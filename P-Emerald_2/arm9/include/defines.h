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

#define VERSION ( "0.8-" __DATE__ " " __TIME__ )
#define VERSION_NAME "Magnetizing Magnemite neo"
#define GAME_TITLE "POKEMON NEO"

#ifdef DESQUID
#define DESQUID_STRING ( 1 << 13 )
#endif

// Assumes that the Backup is a 512k flash memory
#define BACKUP_SIZE ( 512 * 1024 )

#define MAX_ITEMS 1280
#define MAX_ITEMS_IN_BAG MAX_ITEMS
#define MAX_ATTACK 800
#define MAX_MOVE MAX_ATTACK
#define MAX_PKMN 890
#define OTLENGTH 8
#define PKMN_NAMELENGTH 15
#define ITEM_NAMELENGTH 20
#define MOVE_NAMELENGTH 20
#define ABILITY_NAMELENGTH 20
#define ABILITY_DSCRLENGTH 200
#define LOCATION_NAMELENGTH 25

#undef RAND_MAX
#define RAND_MAX 4294967295

enum GameMod : u8 { DEVELOPER, ALPHA, BETA, RELEASE, EMULATOR };
extern GameMod gMod;

u8 getCurrentDaytime( );

// num frames between button input
#define COOLDOWN_COUNT 8

extern bool DRAW_TIME;
extern bool UPDATE_TIME;
extern bool ANIMATE_MAP;
extern u8   FRAME_COUNT;
extern bool SCREENS_SWAPPED;
extern bool PLAYER_IS_FISHING;
extern bool INIT_NITROFS;

extern char** ARGV;

extern int achours, acseconds, acminutes, acday, acmonth, acyear;
extern int hours, seconds, minutes, day, month, year;

extern int           pressed, held, last;
extern touchPosition touch;
extern u8            cooldown;

extern bool TWL_CONFIG;

extern unsigned int   TEMP[ 12288 ];
extern unsigned short TEMP_PAL[ 256 ];

#define LANGUAGES 2
#define MAX_STRINGS 500

extern const char*       LANGUAGE_NAMES[ LANGUAGES ];
extern const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ];
extern const char* const MONTHS[ 12 ][ LANGUAGES ];
#define CURRENT_LANGUAGE SAVE::SAV.getActiveFile( ).m_options.m_language

#ifdef DESQUID
#define MAX_DESQUID_STRINGS 50
extern const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ LANGUAGES ];
#define GET_STRING( p_stringId )                                                                 \
    ( ( ( p_stringId ) >= DESQUID_STRING ) ? DESQUID_STRINGS[ p_stringId - DESQUID_STRING ][ 0 ] \
                                           : STRINGS[ p_stringId ][ CURRENT_LANGUAGE ] )
#else
#define GET_STRING( p_stringId ) STRINGS[ p_stringId ][ CURRENT_LANGUAGE ]
#endif

#define NO_DATA GET_STRING( 0 )
#define FARAWAY_PLACE GET_STRING( 1 )
#define UNKNOWN_SPECIES GET_STRING( 2 )
#define POKE_NAV GET_STRING( 8 )

#define loop( ) while( 1 )
#define TIMER_SPEED ( BUS_CLOCK / 1024 )
#define sq( a ) ( ( a ) * ( a ) )

#define IN_DEX( pidx ) ( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ ( pidx ) / 8 ] & ( 1 << ( ( pidx ) % 8 ) ) )

// Main sprite's OAM indices
#define BACK_ID 0
#define SAVE_ID 1
#define PKMN_ID 2
#define ID_ID 3
#define DEX_ID 4
#define BAG_ID 5
#define OPTS_ID 6
#define NAV_ID 7
#define A_ID 8
#define ITM( a ) ( 9 + ( a ) )
#define FWD_ID 13
#define BWD_ID 14
#define ITM_BACK 15

#define SQCH_ID 80
