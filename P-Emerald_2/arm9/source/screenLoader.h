/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : screenLoader.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

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

#pragma once

#include <nds.h>

#include "sprite.h"
#include "pokemon.h"
#include "defines.h"
#include <string>


extern int drawBox( u16 );

extern void initMapSprites( );
extern bool movePlayerOnMap( s16, s16, s16, bool );

#define MAXMAPPOS 75
struct MapRegionPos {
    u8 m_lx,
        m_ly,
        m_rx,
        m_ry;
    u16 m_ind;
};
extern const MapRegionPos MapLocations[ 3 ][ MAXMAPPOS ];
void printMapLocation( const MapRegionPos& p_m );
extern void printMapLocation( const touchPosition& t );

class move;
extern move* AttackList[ 560 ];
extern void shoUseAttack( u16 p_pkmIdx, bool p_female, bool p_shiny );

enum Region {
    NONE = 0,
    HOENN = 1,
    KANTO = 2,
    JOHTO = 3
};
extern std::map<u16, std::string> Locations;
extern Region acMapRegion;
extern bool showmappointer;


void animateMap( u8 p_frame );

void animateBack( );
void setMainSpriteVisibility( bool p_hidden, bool p_save = false );

u8 getCurrentDaytime( );

extern std::string bagnames[ 8 ];
class screenLoader {
private:
    s8 _pos;

public:
    screenLoader( s8 p_pos ) : _pos( p_pos ) { }

    void draw( s8 p_mode );
    void init( );

    void run_bag( );
    void run_pkmn( );
    void run_dex( u16 p_num = 0 );
};

//Some specific things
u16 initMainSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo );