/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : screenLoader.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */

#pragma once

#include <nds.h>

#include "sprite.h"
#include "pokemon.h"
#include "defines.h"
#include <string>

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
class saveGame;
extern saveGame SAV;

extern PrintConsole Top, Bottom;
extern ConsoleFont cfont;
extern int achours, acseconds, acminutes, acday, acmonth, acyear;
extern int hours, seconds, minutes, day, month, year;
extern u32 ticks;

extern POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
extern std::vector<int> box_of_st_pkmn[ MAXPKMN ];
extern std::vector<int> free_spaces;

extern SpriteInfo spriteInfo[ SPRITE_COUNT ];
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];
extern OAMTable *Oam, *OamTop;

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

#define MAXBG 10
struct backgroundSet {
    std::string             m_name;
    const unsigned int      *m_mainMenu;
    const unsigned short    *m_mainMenuPal;
    bool                    m_loadFromRom;
    bool                    m_allowsOverlay;
    u8                      *m_mainMenuSpritePoses;
};
extern backgroundSet BGs[ MAXBG ];
extern u8 BG_ind;

void vramSetup( );

void updateTime( s8 p_mapMode = 0 );
void animateMap( u8 p_frame );

void initVideoSub( );
void drawSub( );

void animateBack( );
void setMainSpriteVisibility( bool p_hidden, bool p_save = false );

void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16 & p_nextAvailableTileIdx, Type p_type, u16 p_posX, u16 p_posY, bool p_bottom );

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
