/*
    Pok�mon Emerald 2 Version
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
#include <string>

#define sq(a) ((a)*(a))
#define TIMER_SPEED (BUS_CLOCK/1024)

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

class move;
extern move* AttackList[ 560 ];
extern void shoUseAttack( u16 p_pkmIdx, bool p_female, bool p_shiny );

namespace POKEMON {
    bool drawInfoSub( u16* p_layer, int p_PKMN );
}

enum Region {
    NONE = 0,
    HOENN = 1,
    KANTO = 2,
    JOHTO = 3
};

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
#define SQCH_ID 41
// 42 used

#define MAXBG 10
struct backgroundSet {
    std::string             m_name;
    const u32               *m_mainMenu;
    const u16               *m_mainMenuPal;
    bool                    m_loadFromRom;
    bool                    m_allowsOverlay;
    u8                      *m_mainMenuSpritePoses;
};
extern backgroundSet BGs[ MAXBG ];
extern int BG_ind;

void vramSetup( );

void updateTime( int p_mapMode = 0 );
void animateMap( u8 p_frame );

void initVideoSub( );
void drawSub( );

void animateBack( );
void setMainSpriteVisibility( bool p_hidden );

void drawItem( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const u16 p_posX, const u16 p_posY, const u16 p_cnt,
               u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen, bool p_showcnt );
void drawItemIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const u16 p_posX, const u16 p_posY,
                   u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen = true );
void drawPKMNIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const u16& p_pkmnNo, const u16 p_posX, const u16 p_posY,
                   u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen );
void drawEggIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const u16 p_posX, const u16 p_posY,
                  u8& p_oamIndex, u8& p_palcnt, u16& p_nextAvailableTileIdx, bool p_subScreen );
void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16 & p_nextAvailableTileIdx, Type p_type, u16 p_posX, u16 p_posY, bool p_bottom );

int getCurrentDaytime( );

extern std::string bagnames[ 8 ];
class screenLoader {
private:
    u8 _pos;

public:
    screenLoader( u8 p_pos ) : _pos( p_pos ) { }

    void draw( u8 p_mode );
    void init( );

    void run_bag( );
    void run_pkmn( );
    void run_dex( u16 p_num = 0 );
};
