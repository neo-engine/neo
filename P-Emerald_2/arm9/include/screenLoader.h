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
extern unsigned int ticks;

#define MAXMAPPOS 75
struct MapRegionPos {
    int m_lx,
        m_ly,
        m_rx,
        m_ry,
        m_ind;
};
extern MapRegionPos MapLocations[ 3 ][ MAXMAPPOS ];
void printMapLocation( const MapRegionPos& p_m );

class move;
extern move* AttackList[ 560 ];
extern void shoUseAttack( int p_pkmIdx, bool p_female, bool p_shiny );

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
#define M_ID  9
// 10 - used
// 11 - used
// 12 - used
#define FWD_ID  13
#define BWD_ID  14
#define CHOICE_ID  15
// 14 additional spaces used
#define BORDER_ID 31
// 9 additional spaces used
#define SQCH_ID 41
// 42 used

#define MAXBG 9
struct backgroundSet {
    std::string             m_name;
    const unsigned int      *m_mainMenu;
    const unsigned short    *m_mainMenuPal;
    bool                    m_loadFromRom;
    bool                    m_allowsOverlay;
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

void drawItem( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const int p_posX, const int p_posY, const int p_cnt,
               int& p_oamIndex, int& p_palcnt, int& p_nextAvailableTileIdx, bool p_subScreen, bool p_showcnt );
void drawItemIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const int p_posX, const int p_posY,
                   int& p_oamIndex, int& p_palcnt, int& p_nextAvailableTileIdx, bool p_subScreen = true );
void drawPKMNIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const int& p_pkmnNo, const int p_posX, const int p_posY,
                   int& p_oamIndex, int& p_palcnt, int& p_nextAvailableTileIdx, bool p_subScreen );
void drawEggIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const int p_posX, const int p_posY,
                  int& p_oamIndex, int& p_palcnt, int& p_nextAvailableTileIdx, bool p_subScreen );

bool loadNavScreen( u16* p_layer, const char* p_name, int p_no );
bool loadPicture( u16* p_layer, const char* p_Path, const char* p_name, int p_paletteSize = 512, int p_tileCnt = 192 * 256 );
bool loadPictureSub( u16* p_layer, const char* p_path, const char* p_name, int p_paletteSize = 512, int p_tileCnt = 192 * 256 );
bool loadSprite( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_tileCnt, const int p_palCnt );
bool loadSpriteSub( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_tileCnt, const int p_palCnt );
bool loadPKMNSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const int& p_pkmnNo, const int p_posX,
                     const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_nextAvailableTileIdx, bool p_bottom, bool p_shiny = false, bool p_female = false, bool p_flipX = false );
bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const int& p_pkmnNo, const int p_posX,
                        const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_nextAvailableTileIdx, bool p_bottom, bool p_shiny = false, bool p_female = false, bool p_flipX = false );

bool loadTrainerSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_posX,
                        const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_nextAvailableTileIdx, bool p_bottom, bool p_flipX = false );
bool loadTrainerSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_posX,
                           const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_nextAvailableTileIdx, bool p_bottom, bool p_flipX = false );

void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, int& p_oamIndex, int& p_palCnt, int & p_nextAvailableTileIdx, Type p_type, int p_posX, int p_posY, bool p_bottom );

int getCurrentDaytime( );

extern std::string bagnames[ 8 ];
class screenLoader {
private:
    int _pos;

public:
    screenLoader( int p_pos ) : _pos( p_pos ) { }

    void draw( int p_mode );
    void init( );

    void run_bag( );
    void run_pkmn( );
    void run_dex( int p_num = 0 );
};
