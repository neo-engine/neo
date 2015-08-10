/*
Pokémon Emerald 2 Version
------------------------------

file        : defines.h
author      : Philip Wellnitz 
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
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
#pragma once

#include <nds.h>
#include <nds/ndstypes.h>
#include <cmath>
#include <cstdio>
#include <string>

//#define USE_AS_LIB
#undef _EMULATOR
#define VERSION "0.5"
#define VERSION_NAME "Fighting Torchic"
#define DEBUG

enum GameMod : u8 {
    DEVELOPER,
    ALPHA,
    BETA,
    RELEASE,
    EMULATOR
};
extern GameMod gMod;

std::string toString( u16 p_num ); //I REALLY LIKE WORKAROUNDING g++'S BUGS
u8 getCurrentDaytime( );

extern bool DRAW_TIME;
extern bool ANIMATE_MAP;

extern int achours, acseconds, acminutes, acday, acmonth, acyear;
extern int hours, seconds, minutes, day, month, year;

extern unsigned int TEMP[ 12288 ];
extern unsigned short TEMP_PAL[ 256 ];

#define NO_DATA "Keine Daten."
#define FARAWAY_PLACE "Entfernter Ort"
#define UNKNOWN_SPECIES "Unbekannt"

#define loop() while( 1 )

#define IN_RANGE( p_touch, p_input ) ( (p_touch).px >= (p_input).m_targetX1 && (p_touch).py >= (p_input).m_targetY1 \
                                    && (p_touch).px <= (p_input).m_targetX2 && (p_touch).py <= (p_input).m_targetY2 )
#define IN_RANGE_C( p_touch, p_input ) ( sqrt( sq( (p_touch).px - (p_input).m_targetX1 ) + sq( (p_touch).py - (p_input).m_targetY1 ) ) <= (p_input).m_targetR )

#define GET_AND_WAIT( key ) ( ( pressed & key ) && IO::waitForInput( IO::inputTarget( key ) ) )
#define GET_AND_WAIT_R( p_x1, p_y1, p_x2, p_y2 ) ( IN_RANGE( touch, IO::inputTarget( p_x1, p_y1, p_x2, p_y2 ) )\
                                        && IO::waitForInput( IO::inputTarget( p_x1, p_y1, p_x2, p_y2 ) ) )
#define GET_AND_WAIT_C( p_x, p_y, p_r ) ( IN_RANGE_C( touch, IO::inputTarget( p_x, p_y, p_r ) )\
                                    && IO::waitForInput( IO::inputTarget( p_x, p_y, p_r ) ) )

#define RGB(r, g, b) (RGB15((r), (g), (b)) | BIT(15))
#define COMPL(a) ( RGB( 31 - ( (a) >> 10 ) % 32,31 - ( (a) >> 5 ) % 32,31 - (a) % 32 ) )

#define COLOR_IDX (u8(249))
#define WHITE_IDX (u8(250))
#define GRAY_IDX (u8(251))
#define BLACK_IDX (u8(252))
#define RED_IDX (u8(253))
#define BLUE_IDX (u8(254))

#define CHOICE_COLOR RGB(16,25,19)

#define GREEN RGB(0,20,0)
#define RED RGB(24,0,0)
#define BLUE RGB(0,0,24)
#define WHITE RGB(31,31,31)
#define GRAY RGB(15,15,15)
#define NORMAL_ RGB(27,27,27)
#define BLACK RGB(0,0,0)
#define YELLOW RGB(24,24,0)
#define PURPLE RGB(24,0,24)
#define TURQOISE RGB(0,24,24)
#define ICE RGB(15,31,31)
#define FAIRY RGB(31,15,31)
#define GROUND RGB(31,31,15)
#define POISON RGB(31,0,15)
#define ORANGE RGB(31,15,0)
#define GHOST RGB(15,0,31)
#define ROCK RGB(28,23,7)    
#define BUG RGB(15,28,7)    
#define STEEL RGB(24,24,24)
#define DRAGON RGB(7,7,24)
#define UNKNOWN RGB(0,42,42)

#define BG_PAL( p_sub ) ( ( p_sub ) ? BG_PALETTE_SUB : BG_PALETTE )
#define BG_BMP( p_sub ) ( ( p_sub ) ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 ) )


#define TIMER_SPEED (BUS_CLOCK/1024)
#define sq(a) ((a)*(a))

//Main sprite's OAM indices
#define BACK_ID  0
#define SAVE_ID  1
#define PKMN_ID  2
#define ID_ID  3
#define DEX_ID  4
#define BAG_ID  5
#define OPTS_ID  6
#define NAV_ID  7
#define A_ID  8
//  9 - unused
// 10 - unused
// 11 - unused
// 12 - unused
#define FWD_ID  13
#define BWD_ID  14
#define CHOICE_ID  15
// 14 additional spaces used
#define BORDER_ID 31
// 9 additional spaces used
#define SQCH_ID 80
