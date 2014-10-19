/*
    Pok�mon Emerald 2 Version
    ------------------------------

    file        : screenLoader.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for drawing main screnns and some other stuff

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



#include <fstream>
#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "as_lib9.h"

#include "messageBox.h"
#include "screenLoader.h"
#include "saveGame.h"
#include "pokemon.h"
#include "sprite.h"
#include "bag.h"
#include "item.h"
#include "battle.h"
#include "buffer.h"

//Sprites
#include "Back.h"
#include "Save.h"
#include "Option.h"
#include "PokemonSp.h"
#include "Id.h"
#include "SPBag.h"
#include "Nav.h"
#include "PokeDex.h"
#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Choice_1.h"
#include "Choice_2.h"
#include "Choice_3.h"
#include "Message.h"
#include "Border_1.h"
#include "Border_2.h"
#include "Border_3.h"
#include "Border_4.h"
#include "Border_5.h"

#include "Border.h"

#include "ChSq_a.h"
#include "ChSq_b.h"
#include "BigCirc1.h"

#include "ItemSpr1.h"
#include "ItemSpr2.h"
#include "ItemSpr3.h"

#include "NoItem.h"
#include "BagSpr.h"
#include "BagSpr2.h"

#include "BG1.h"
#include "BG2.h"

PrintConsole Top, Bottom;

unsigned int NAV_DATA[ 12288 ] = { 0 };
unsigned short int NAV_DATA_PAL[ 256 ] = { 0 };
//Centers o t circles.
//pokemon -> ID -> DEX -> Bag -> Opt -> Nav
// X|Y
int mainSpritesPositions[ 12 ] = { 130, 60,
160, 80,
160, 115,
130, 135,
100, 115,
100, 80 };
int mainSpritesPositions2[ 12 ] = { 24, 64,
236, 96,
20, 128,
238, 64,
22, 96,
234, 128 };
backgroundSet BGs[ MAXBG ] = { { "Raging_Gyarados", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Sleeping_Eevee", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Mystic_Guardevoir", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Waiting_Suicune", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Awakening_Xerneas", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Awakening_Yveltal", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Fighting_Groudon", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Fighting_Kyogre", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
{ "Working_Klink", BG1Bitmap, BG1Pal, false, true, mainSpritesPositions },
{ "Reborn_Ho-Oh", BG2Bitmap, BG2Pal, false, true, mainSpritesPositions2 } };
int BG_ind = 9;
extern POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
extern std::vector<int> box_of_st_pkmn[ MAXPKMN ];
extern std::vector<int> free_spaces;

extern SpriteInfo spriteInfo[ SPRITE_COUNT ];
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];
extern OAMTable *Oam, *OamTop;

extern int drawBox( short );
namespace POKEMON {
    extern const char* getLoc( int ind );
}
Region acMapRegion = NONE;
std::pair<int, int> acMapPoint = std::pair<int, int>( 32, 24 );
bool showfirst = true, showmappointer = false;

const MapRegionPos MapLocations[ 3 ][ MAXMAPPOS ] = {
        {
            { 90, 66, 96, 73, 1010 },
            { 115, 139, 121, 146, 308 },
            { 57, 36, 66, 47, 239 },
            { 46, 53, 51, 59, 298 },
            { 37, 73, 47, 89, 245 },
            { 39, 98, 45, 104, 294 },
            { 45, 104, 53, 112, 242 },
            { 47, 136, 54, 142, 291 },
            { 50, 136, 59, 148, 237 },
            { 64, 103, 72, 111, 236 },
            { 64, 117, 72, 125, 235 },
            { 64, 80, 72, 88, 240 },
            { 65, 75, 71, 80, 295 },
            { 69, 59, 78, 67, 238 },
            { 77, 47, 85, 60, 292 },
            { 79, 60, 84, 66, 312 },
            { 89, 80, 105, 96, 244 },
            { 89, 109, 98, 126, 243 },
            { 114, 36, 123, 46, 246 },
            { 140, 52, 149, 62, 202 },
            { 142, 71, 147, 77, 300 },
            { 121, 94, 130, 104, 1000 },
            { 146, 109, 154, 118, 241 },
            { 168, 112, 174, 116, 306 },
            { 161, 112, 167, 116, 320 },
            { 110, 112, 116, 116, 313 },
            { 179, 122, 187, 131, 293 },
            { 210, 95, 219, 111, 250 },
            { 173, 88, 180, 97, 249 },
            { 192, 74, 206, 82, 248 },
            { 200, 60, 204, 66, 302 },
            { 153, 59, 168, 67, 247 },
            { 93, 111, 115, 117, 285 },
            { 114, 111, 130, 117, 284 },
            { 129, 111, 150, 117, 283 },
            { 149, 111, 174, 117, 282 },
            { 173, 111, 191, 117, 281 },
            { 190, 111, 205, 117, 280 },
            { 187, 104, 212, 112, 279 },
            { 187, 80, 205, 105, 278 },
            { 168, 80, 188, 101, 277 },
            { 168, 60, 189, 81, 275 },
            { 190, 60, 205, 76, 276 },
            { 128, 60, 153, 66, 272 },
            { 142, 65, 147, 83, 273 },
            { 115, 82, 147, 88, 274 },
            { 122, 87, 128, 95, 1001 },
            { 103, 82, 115, 88, 269 },
            { 109, 39, 114, 83, 270 },
            { 122, 39, 128, 66, 271 },
            { 90, 39, 96, 83, 262 },
            { 76, 60, 91, 66, 263 },
            { 62, 37, 96, 44, 264 },
            { 39, 54, 45, 75, 266 },
            { 46, 39, 51, 53, 265 },
            { 46, 39, 60, 44, 265 },
            { 45, 75, 66, 81, 267 },
            { 39, 90, 45, 109, 254 },
            { 39, 108, 45, 133, 255 },
            { 39, 132, 57, 139, 256 },
            { 57, 140, 70, 146, 258 },
            { 69, 140, 91, 146, 259 },
            { 90, 122, 96, 146, 260 },
            { 90, 85, 96, 116, 261 },
            { 69, 82, 94, 88, 268 },
            { 50, 103, 66, 110, 252 },
            { 64, 109, 70, 119, 251 },
            { 65, 96, 91, 103, 253 }
        }
};
void printMapLocation( const touchPosition& p_t ) {
    consoleSetWindow( &Bottom, 5, 0, 20, 1 );
    consoleSelect( &Bottom );
    consoleClear( );
    for( int i = 0; i < MAXMAPPOS; ++i )
        if( p_t.px > MapLocations[ acMapRegion - 1 ][ i ].m_lx && p_t.px < MapLocations[ acMapRegion - 1 ][ i ].m_rx &&
            p_t.py > MapLocations[ acMapRegion - 1 ][ i ].m_ly && p_t.py < MapLocations[ acMapRegion - 1 ][ i ].m_ry ) {
        printf( POKEMON::getLoc( MapLocations[ acMapRegion - 1 ][ i ].m_ind ) );
        return;
        }
}

void printMapLocation( const MapRegionPos& p_m ) {
    consoleSetWindow( &Bottom, 5, 0, 20, 1 );
    consoleSelect( &Bottom );
    consoleClear( );
    printf( POKEMON::getLoc( p_m.m_ind ) );
    acMapPoint = std::pair<int, int>( ( p_m.m_lx + p_m.m_rx ) / 2, ( p_m.m_ly + p_m.m_ry ) / 2 );
    updateTime( );
    return;
}
u8 frame = 0;

bool used = false;
bool usin = false;

void updateTime( int p_mapMode ) {
    used = true;
    static bool mm = false;
    if( -1 != p_mapMode )
        mm = p_mapMode;

    if( !usin )
        return;
    //AS_SoundVBL();

    cust_font2.setColor( 0, 0 );
    cust_font2.setColor( 0, 1 );
    cust_font2.setColor( 252, 2 );
    BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );

    frame = ( frame + 1 ) % 256;
    if( mm )
        animateMap( frame );

    time_t unixTime = time( NULL );
    struct tm* timeStruct = gmtime( (const time_t *)&unixTime );

    achours = timeStruct->tm_hour;
    acminutes = timeStruct->tm_min;
    if( acseconds != timeStruct->tm_sec ) {
        acseconds = timeStruct->tm_sec;
        if( showmappointer ) {
            if( showfirst ) {
                showfirst = false;
                Oam->oamBuffer[ SQCH_ID ].isHidden = true;
                Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = false;
            } else {
                showfirst = true;
                Oam->oamBuffer[ SQCH_ID ].isHidden = false;
                Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = true;
            }
            updateOAMSub( Oam );
        }
        char buf[ 15 ];
        sprintf( buf, "%02i:%02i:%02i", achours, acminutes, acseconds );
        dmaCopy( BorderBitmap + 6144 + 3072, bgGetGfxPtr( bg2sub ) + 6 * 3072, 64 * 256 );
        cust_font2.printString( buf, 18 * 8, 192 - 16, true );
    }
    acday = timeStruct->tm_mday;
    acmonth = timeStruct->tm_mon;
    acyear = timeStruct->tm_year + 1900;
}


// regenerate buffers for mp3 stream, must be called each VBlank (only needed if mp3 is used)
void AS_SoundVBL( ) {
    usin = true;
    if( used )
        updateTime( -1 );
    usin = false;
    // refill mp3 file  buffer if needed
    if( IPC_Sound->mp3.needdata ) {
        AS_MP3FillBuffer( IPC_Sound->mp3.mp3buffer + AS_FILEBUFFER_SIZE, AS_FILEBUFFER_SIZE );
        IPC_Sound->mp3.needdata = false;
    }
    scanKeys( );
}

int DayTimes[ 4 ][ 5 ] = {
        { 7, 10, 15, 17, 23 },
        { 6, 9, 12, 18, 23 },
        { 5, 8, 10, 20, 23 },
        { 7, 9, 13, 19, 23 }
};

int getCurrentDaytime( ) {
    time_t unixTime = time( NULL );
    struct tm* timeStruct = gmtime( (const time_t *)&unixTime );

    int t = timeStruct->tm_hour, m = timeStruct->tm_mon;

    for( int i = 0; i < 5; ++i )
        if( DayTimes[ m / 4 ][ i ] >= t )
            return i;
    return -1;
}

unsigned int TEMP[ 12288 ] = { 0 };
unsigned short int TEMP_PAL[ 256 ] = { 0 };

bool loadSprite( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_tileCnt, const int p_palCnt ) {
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
        sizeof( SPRITE_GFX[ 0 ] );

    //char buffer[100];
    sprintf( buffer, "%s%s.raw", p_path, p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }
    fread( TEMP, sizeof( unsigned int ), p_tileCnt, fd );
    fread( TEMP_PAL, sizeof( unsigned short int ), p_palCnt, fd );

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_spriteInfo->entry->gfxIndex * OFFSET_MULTIPLIER ], 4 * p_tileCnt );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ p_spriteInfo->entry->palette * COLORS_PER_PALETTE ], 2 * p_palCnt );
    fclose( fd );
    return true;
}
bool loadSpriteSub( SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_tileCnt, const int p_palCnt ) {
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
        sizeof( SPRITE_GFX_SUB[ 0 ] );

    //char buffer[100];
    sprintf( buffer, "%s%s.raw", p_path, p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }
    fread( TEMP, sizeof( unsigned int ), p_tileCnt, fd );
    fread( TEMP_PAL, sizeof( unsigned short int ), p_palCnt, fd );

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      TEMP,
                      &SPRITE_GFX_SUB[ p_spriteInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                      4 * p_tileCnt );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      TEMP_PAL,
                      &SPRITE_PALETTE_SUB[ p_spriteInfo->entry->palette * COLORS_PER_PALETTE ],
                      2 * p_palCnt );
    fclose( fd );
    return true;
}

bool loadPKMNSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const int& p_pkmnId, const int p_posX,
                     const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_bottom, bool p_shiny, bool p_female, bool p_flipx ) {
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] );


    //char buffer[100];
    if( !p_female )
        sprintf( buffer, "%s%d/%d.raw", p_path, p_pkmnId, p_pkmnId );
    else
        sprintf( buffer, "%s%d/%df.raw", p_path, p_pkmnId, p_pkmnId );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }
    //p_palCnt = 16;
    for( int i = 0; i < 16; ++i )
        TEMP_PAL[ i ] = 0;
    fread( TEMP_PAL, sizeof( unsigned short int ), 16, fd );
    for( int i = 0; i < 96 * 96; ++i )
        TEMP[ i ] = 0;
    fread( TEMP, sizeof( unsigned int ), 96 * 96, fd );
    fclose( fd );
    if( p_shiny ) {
        memset( buffer, 0, sizeof( buffer ) );
        if( !p_female )
            sprintf( buffer, "%s%d/%ds.raw", p_path, p_pkmnId, p_pkmnId );
        else
            sprintf( buffer, "%s%d/%dsf.raw", p_path, p_pkmnId, p_pkmnId );
        fd = fopen( buffer, "rb" );
        for( int i = 0; i < 16; ++i )
            TEMP_PAL[ i ] = 0;
        fread( TEMP_PAL, sizeof( unsigned short int ), 16, fd );
        fclose( fd );
    }
    if( p_bottom ) {
        //swiCopy(TEMP_PAL,&SPRITE_PALETTE_SUB[(++p_palCnt) * COLORS_PER_PALETTE], 32 | COPY_MODE_HWORD);
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
    } else {
        //swiCopy(TEMP_PAL,&SPRITE_PALETTE[(++p_palCnt) * COLORS_PER_PALETTE], 32 | COPY_MODE_HWORD);
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
    }
    SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 64;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = p_flipx ? 32 + p_posX : p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    if( p_bottom ) {
        //swiCopy(TEMP,&SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER], 96*96/2 | COPY_MODE_HWORD);
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    } else {
        //swiCopy(TEMP,&SPRITE_GFX[p_tileCnt * OFFSET_MULTIPLIER], 96*96/2 | COPY_MODE_HWORD);
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    }
    p_tileCnt += 64;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_TALL;
    back->x = p_flipx ? p_posX : 64 + p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 32;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 64;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY + 64;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_WIDE;
    back->x = p_flipx ? 32 + p_posX : p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 32;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY + 64;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = p_flipx ? p_posX : 64 + p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_32;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 16;

    ++p_palCnt;
    if( p_bottom )
        updateOAMSub( p_oam );
    else
        updateOAM( p_oam );
    return true;
}
bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const int& p_pkmnId, const int p_posX,
                        const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_bottom, bool p_shiny, bool p_female, bool p_flipx ) {
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] );

    //char buffer[100];

    if( !p_female )
        sprintf( buffer, "%s%d/%d.raw", p_path, p_pkmnId, p_pkmnId );
    else
        sprintf( buffer, "%s%d/%df.raw", p_path, p_pkmnId, p_pkmnId );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }
    //p_palCnt = 16;
    for( int i = 0; i < 16; ++i )
        TEMP_PAL[ i ] = 0;
    fread( TEMP_PAL, sizeof( unsigned short int ), 16, fd );
    for( int i = 0; i < 96 * 96; ++i )
        TEMP[ i ] = 0;
    fread( TEMP, sizeof( unsigned int ), 96 * 64, fd );
    fclose( fd );
    if( p_shiny ) {
        memset( buffer, 0, sizeof( buffer ) );
        if( !p_female )
            sprintf( buffer, "%s%d/%ds.raw", p_path, p_pkmnId, p_pkmnId );
        else
            sprintf( buffer, "%s%d/%dsf.raw", p_path, p_pkmnId, p_pkmnId );
        fd = fopen( buffer, "rb" );
        for( int i = 0; i < 16; ++i )
            TEMP_PAL[ i ] = 0;
        fread( TEMP_PAL, sizeof( unsigned short int ), 16, fd );
        fclose( fd );
    }
    if( p_bottom )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
    else
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );

    SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 64;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = p_flipx ? 32 + p_posX : p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    if( p_bottom )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    else
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    p_tileCnt += 64;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_TALL;
    back->x = p_flipx ? p_posX : 64 + p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 32;

    ++p_palCnt;
    if( p_bottom )
        updateOAMSub( p_oam );
    else
        updateOAM( p_oam );
    return true;
}

bool loadTrainerSprite( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_posX,
                        const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_bottom, bool p_flipx ) {
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] );

    //char buffer[100];

    sprintf( buffer, "%sSprite_%s.raw", p_path, p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }
    //p_palCnt = 16;
    for( int i = 0; i < 16; ++i )
        TEMP_PAL[ i ] = 0;
    fread( TEMP_PAL, sizeof( unsigned short int ), 16, fd );
    for( int i = 0; i < 96 * 96; ++i )
        TEMP[ i ] = 0;
    fread( TEMP, sizeof( unsigned int ), 96 * 96, fd );
    fclose( fd );

    if( p_bottom )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
    else
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );

    SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 64;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = p_flipx ? 32 + p_posX : p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    if( p_bottom )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    else
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    p_tileCnt += 64;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_TALL;
    back->x = p_flipx ? p_posX : 64 + p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 32;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 64;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY + 64;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_WIDE;
    back->x = p_flipx ? 32 + p_posX : p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 32;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY + 64;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = p_flipx ? p_posX : 64 + p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_32;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 16;

    ++p_palCnt;
    if( p_bottom )
        updateOAMSub( p_oam );
    else
        updateOAM( p_oam );
    return true;
}
bool loadPKMNSpriteTop( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const int p_posX,
                        const int p_posY, int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_bottom, bool p_flipx ) {
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] );

    //char buffer[100];

    sprintf( buffer, "%sSprite_%s.raw", p_path, p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }
    //p_palCnt = 16;
    for( int i = 0; i < 16; ++i )
        TEMP_PAL[ i ] = 0;
    fread( TEMP_PAL, sizeof( unsigned short int ), 16, fd );
    for( int i = 0; i < 96 * 96; ++i )
        TEMP[ i ] = 0;
    fread( TEMP, sizeof( unsigned int ), 96 * 64, fd );
    fclose( fd );

    if( p_bottom )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );
    else
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[ ( ++p_palCnt ) * COLORS_PER_PALETTE ], 32 );

    SpriteInfo * backInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 64;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = p_flipx ? 32 + p_posX : p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    if( p_bottom )
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    else
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], 96 * 96 / 2 );
    p_tileCnt += 64;

    backInfo = &p_spriteInfo[ ++p_oamIndex ];
    back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = p_posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_TALL;
    back->x = p_flipx ? p_posX : 64 + p_posX;
    back->hFlip = p_flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = p_palCnt;
    p_tileCnt += 32;

    ++p_palCnt;
    if( p_bottom )
        updateOAMSub( p_oam );
    else
        updateOAM( p_oam );
    return true;
}

bool loadPicture( u16* p_layer, const char* p_path, const char* p_name, int p_palSize, int p_tileCnt ) {

    //char buffer[100];
    sprintf( buffer, "%s%s.raw", p_path, p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }

    fread( TEMP, sizeof( unsigned int ), 12288, fd );
    fread( TEMP_PAL, sizeof( unsigned short int ), 256, fd );

    dmaCopy( TEMP, p_layer, p_tileCnt );
    dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
    fclose( fd );

    return true;
}
bool loadPictureSub( u16* p_layer, const char* p_path, const char* p_name, int p_palSize, int p_tileCnt ) {

    //char buffer[100];
    sprintf( buffer, "%s%s.raw", p_path, p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }

    fread( TEMP, sizeof( unsigned int ), 12288, fd );
    fread( TEMP_PAL, sizeof( unsigned short int ), 256, fd );

    dmaCopy( TEMP, p_layer, p_tileCnt );
    dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
    fclose( fd );


    return true;
}

bool loadNavScreen( u16* p_layer, const char* p_name, int p_no ) {
    if( p_no == BG_ind && NAV_DATA[ 0 ] ) {
        dmaCopy( NAV_DATA, p_layer, 256 * 192 );
        dmaCopy( NAV_DATA_PAL, BG_PALETTE_SUB, 256 * 2 );
        return true;
    }

    //char buffer[100];
    sprintf( buffer, "nitro:/PICS/NAV/%s.raw", p_name );
    FILE* fd = fopen( buffer, "rb" );

    if( fd == 0 ) {
        fclose( fd );
        return false;
    }

    fread( NAV_DATA, sizeof( unsigned int ), 12288, fd );
    fread( NAV_DATA_PAL, sizeof( unsigned short int ), 256, fd );

    dmaCopy( NAV_DATA, p_layer, 256 * 192 );
    dmaCopy( NAV_DATA_PAL, BG_PALETTE_SUB, 256 * 2 );
    fclose( fd );

    return true;
}

unsigned int BTMs[ 6 ][ 12288 ];
unsigned short int PALs[ 6 ][ 256 ];

unsigned short int cachedPKMN[ 6 ] = { 0 };

int ac = 0;
int positions[ 6 ][ 2 ] = {
        { 14, 2 }, { 16, 3 }, { 14, 9 },
        { 16, 10 }, { 14, 17 }, { 16, 18 }
};
int borders[ 6 ][ 2 ] = {
        { 4, 2 }, { 18, 3 },
        { 4, 9 }, { 18, 10 },
        { 4, 17 }, { 18, 18 }
};

extern SpriteInfo spriteInfo[ SPRITE_COUNT ];
extern OAMTable *Oam;

int initMainSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo ) {
    const int BYTES_PER_16_COLOR_TILE = 32;
    const int COLORS_PER_PALETTE = 16;
    const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                   * (can be set in REG_DISPCNT) */
    const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
        sizeof( SPRITE_GFX_SUB[ 0 ] );

    /* Keep track of the available tiles */
    int nextAvailableTileIdx = 16;

    SpriteInfo * backInfo = &p_spriteInfo[ BACK_ID ];
    SpriteEntry * back = &p_oam->oamBuffer[ BACK_ID ];
    backInfo->oamId = BACK_ID;
    backInfo->width = 32;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = SCREEN_HEIGHT - 28;
    back->isRotateScale = false;
    back->isSizeDouble = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = SCREEN_WIDTH - 28;
    back->rotationIndex = backInfo->oamId;
    back->size = OBJSIZE_32;
    back->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += BackTilesLen / BYTES_PER_16_COLOR_TILE;
    back->priority = OBJPRIORITY_0;
    back->palette = backInfo->oamId;


    SpriteInfo * saveInfo = &p_spriteInfo[ SAVE_ID ];
    SpriteEntry * save = &p_oam->oamBuffer[ SAVE_ID ];
    saveInfo->oamId = SAVE_ID;
    saveInfo->width = 64;
    saveInfo->height = 64;
    saveInfo->angle = 0;
    saveInfo->entry = save;
    save->y = -20;
    save->isRotateScale = false;
    save->isSizeDouble = false;
    save->blendMode = OBJMODE_NORMAL;
    save->isMosaic = false;
    save->colorMode = OBJCOLOR_16;
    save->shape = OBJSHAPE_SQUARE;
    save->x = -20;
    save->size = OBJSIZE_64;
    save->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += SaveTilesLen / BYTES_PER_16_COLOR_TILE;
    save->priority = OBJPRIORITY_0;
    save->palette = saveInfo->oamId;


    //MAIN MENU

    SpriteInfo * optsInfo = &p_spriteInfo[ OPTS_ID ];
    SpriteEntry * opts = &p_oam->oamBuffer[ OPTS_ID ];
    optsInfo->oamId = OPTS_ID;
    optsInfo->width = 32;
    optsInfo->height = 32;
    optsInfo->angle = 0;
    optsInfo->entry = opts;
    opts->y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) + 1 ] - optsInfo->height / 2;
    opts->isRotateScale = false;
    opts->isSizeDouble = false;
    opts->blendMode = OBJMODE_NORMAL;
    opts->isMosaic = false;
    opts->colorMode = OBJCOLOR_16;
    opts->shape = OBJSHAPE_SQUARE;
    opts->x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) ] - optsInfo->width / 2;
    opts->size = OBJSIZE_32;
    opts->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += OptionTilesLen / BYTES_PER_16_COLOR_TILE;
    opts->priority = OBJPRIORITY_0;
    opts->palette = optsInfo->oamId;


    SpriteInfo * pkmnInfo = &p_spriteInfo[ PKMN_ID ];
    SpriteEntry * pkmn = &p_oam->oamBuffer[ PKMN_ID ];
    pkmnInfo->oamId = PKMN_ID;
    pkmnInfo->width = 32;
    pkmnInfo->height = 32;
    pkmnInfo->angle = 0;
    pkmnInfo->entry = pkmn;
    pkmn->y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) + 1 ] - pkmnInfo->height / 2;
    pkmn->isRotateScale = false;
    pkmn->isSizeDouble = false;
    pkmn->blendMode = OBJMODE_NORMAL;
    pkmn->isMosaic = false;
    pkmn->colorMode = OBJCOLOR_16;
    pkmn->shape = OBJSHAPE_SQUARE;
    pkmn->x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) ] - pkmnInfo->width / 2;
    pkmn->size = OBJSIZE_32;
    pkmn->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += PokemonSpTilesLen / BYTES_PER_16_COLOR_TILE;
    pkmn->priority = OBJPRIORITY_0;
    pkmn->palette = pkmnInfo->oamId;


    SpriteInfo * navInfo = &p_spriteInfo[ NAV_ID ];
    SpriteEntry * nav = &p_oam->oamBuffer[ NAV_ID ];
    navInfo->oamId = NAV_ID;
    navInfo->width = 32;
    navInfo->height = 32;
    navInfo->angle = 0;
    navInfo->entry = nav;
    nav->y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) + 1 ] - navInfo->height / 2;
    nav->isRotateScale = false;
    nav->isSizeDouble = false;
    nav->blendMode = OBJMODE_NORMAL;
    nav->isMosaic = false;
    nav->colorMode = OBJCOLOR_16;
    nav->shape = OBJSHAPE_SQUARE;
    nav->x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) ] - navInfo->width / 2;
    nav->size = OBJSIZE_32;
    nav->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += NavTilesLen / BYTES_PER_16_COLOR_TILE;
    nav->priority = OBJPRIORITY_0;
    nav->palette = navInfo->oamId;


    SpriteInfo * idInfo = &p_spriteInfo[ ID_ID ];
    SpriteEntry * id = &p_oam->oamBuffer[ ID_ID ];
    idInfo->oamId = ID_ID;
    idInfo->width = 32;
    idInfo->height = 32;
    idInfo->angle = 0;
    idInfo->entry = id;
    id->y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) + 1 ] - idInfo->height / 2;
    id->isRotateScale = false;
    id->isSizeDouble = false;
    id->blendMode = OBJMODE_NORMAL;
    id->isMosaic = false;
    id->colorMode = OBJCOLOR_16;
    id->shape = OBJSHAPE_SQUARE;
    id->x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) ] - idInfo->width / 2;
    id->size = OBJSIZE_32;
    id->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += IdTilesLen / BYTES_PER_16_COLOR_TILE;
    id->priority = OBJPRIORITY_0;
    id->palette = idInfo->oamId;


    SpriteInfo * dexInfo = &p_spriteInfo[ DEX_ID ];
    SpriteEntry * dex = &p_oam->oamBuffer[ DEX_ID ];
    dexInfo->oamId = DEX_ID;
    dexInfo->width = 32;
    dexInfo->height = 32;
    dexInfo->angle = 0;
    dexInfo->entry = dex;
    dex->y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) + 1 ] - dexInfo->height / 2;
    dex->isRotateScale = false;
    dex->isSizeDouble = false;
    dex->blendMode = OBJMODE_NORMAL;
    dex->isMosaic = false;
    dex->colorMode = OBJCOLOR_16;
    dex->shape = OBJSHAPE_SQUARE;
    dex->x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) ] - dexInfo->width / 2;
    dex->size = OBJSIZE_32;
    dex->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += PokeDexTilesLen / BYTES_PER_16_COLOR_TILE;
    dex->priority = OBJPRIORITY_0;
    dex->palette = dexInfo->oamId;


    SpriteInfo * bagInfo = &p_spriteInfo[ BAG_ID ];
    SpriteEntry * bag = &p_oam->oamBuffer[ BAG_ID ];
    bagInfo->oamId = BAG_ID;
    bagInfo->width = 32;
    bagInfo->height = 32;
    bagInfo->angle = 0;
    bagInfo->entry = bag;
    bag->y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) + 1 ] - bagInfo->height / 2;
    bag->isRotateScale = false;
    bag->isSizeDouble = false;
    bag->blendMode = OBJMODE_NORMAL;
    bag->isMosaic = false;
    bag->colorMode = OBJCOLOR_16;
    bag->shape = OBJSHAPE_SQUARE;
    bag->x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) ] - bagInfo->width / 2;
    bag->size = OBJSIZE_32;
    bag->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += SPBagTilesLen / BYTES_PER_16_COLOR_TILE;
    bag->priority = OBJPRIORITY_0;
    bag->palette = bagInfo->oamId;

    //"A"-Button
    SpriteInfo * AInfo = &p_spriteInfo[ A_ID ];
    SpriteEntry * A = &p_oam->oamBuffer[ A_ID ];
    AInfo->oamId = A_ID;
    AInfo->width = 32;
    AInfo->height = 32;
    AInfo->angle = 0;
    AInfo->entry = A;
    A->y = SCREEN_HEIGHT - 28;
    A->isRotateScale = false;
    A->blendMode = OBJMODE_NORMAL;
    A->isMosaic = false;
    A->colorMode = OBJCOLOR_16;
    A->shape = OBJSHAPE_SQUARE;
    A->isHidden = true;
    A->x = SCREEN_WIDTH - 28;
    A->size = OBJSIZE_32;
    A->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += ATilesLen / BYTES_PER_16_COLOR_TILE;
    A->priority = OBJPRIORITY_0;
    A->palette = AInfo->oamId;


    /* Copy over the sprite palettes */
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BackPal,
                      &SPRITE_PALETTE_SUB[ backInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      SavePal,
                      &SPRITE_PALETTE_SUB[ saveInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      PokemonSpPal,
                      &SPRITE_PALETTE_SUB[ pkmnInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      IdPal,
                      &SPRITE_PALETTE_SUB[ idInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      PokeDexPal,
                      &SPRITE_PALETTE_SUB[ dexInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      SPBagPal,
                      &SPRITE_PALETTE_SUB[ bagInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      OptionPal,
                      &SPRITE_PALETTE_SUB[ optsInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      NavPal,
                      &SPRITE_PALETTE_SUB[ navInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      APal,
                      &SPRITE_PALETTE_SUB[ AInfo->oamId * COLORS_PER_PALETTE ],
                      32 );


    /* Copy the sprite graphics to sprite graphics memory */
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BackTiles,
                      &SPRITE_GFX_SUB[ back->gfxIndex * OFFSET_MULTIPLIER ],
                      BackTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      SaveTiles,
                      &SPRITE_GFX_SUB[ save->gfxIndex * OFFSET_MULTIPLIER ],
                      SaveTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      PokemonSpTiles,
                      &SPRITE_GFX_SUB[ pkmn->gfxIndex * OFFSET_MULTIPLIER ],
                      PokemonSpTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      IdTiles,
                      &SPRITE_GFX_SUB[ id->gfxIndex * OFFSET_MULTIPLIER ],
                      IdTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      PokeDexTiles,
                      &SPRITE_GFX_SUB[ dex->gfxIndex * OFFSET_MULTIPLIER ],
                      PokeDexTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      SPBagTiles,
                      &SPRITE_GFX_SUB[ bag->gfxIndex * OFFSET_MULTIPLIER ],
                      SPBagTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      OptionTiles,
                      &SPRITE_GFX_SUB[ opts->gfxIndex * OFFSET_MULTIPLIER ],
                      OptionTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      NavTiles,
                      &SPRITE_GFX_SUB[ nav->gfxIndex * OFFSET_MULTIPLIER ],
                      NavTilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      ATiles,
                      &SPRITE_GFX_SUB[ A->gfxIndex * OFFSET_MULTIPLIER ],
                      ATilesLen );

    int palcnt = M_ID;

    //Message
    for( int i = M_ID; i < M_ID + 4; ++i ) {
        SpriteInfo * MInfo = &p_spriteInfo[ i ];
        SpriteEntry * M = &p_oam->oamBuffer[ i ];
        MInfo->oamId = i;
        MInfo->width = 64;
        MInfo->height = 64;
        MInfo->angle = 0;
        MInfo->entry = M;
        M->y = 0;
        M->isRotateScale = false;
        M->blendMode = OBJMODE_NORMAL;
        M->isMosaic = false;
        M->colorMode = OBJCOLOR_16;
        M->shape = OBJSHAPE_SQUARE;
        M->isHidden = true;
        M->x = ( i - M_ID ) * 64;
        M->size = OBJSIZE_64;
        M->gfxIndex = nextAvailableTileIdx;
        M->priority = OBJPRIORITY_3;
        M->palette = palcnt;
    }
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      MessagePal,
                      &SPRITE_PALETTE_SUB[ palcnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      MessageTiles,
                      &SPRITE_GFX_SUB[ nextAvailableTileIdx * OFFSET_MULTIPLIER ],
                      MessageTilesLen );
    nextAvailableTileIdx += MessageTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;

    SpriteInfo * FwdInfo = &p_spriteInfo[ FWD_ID ];
    SpriteEntry * Fwd = &p_oam->oamBuffer[ FWD_ID ];
    FwdInfo->oamId = FWD_ID;
    FwdInfo->width = 32;
    FwdInfo->height = 32;
    FwdInfo->angle = 0;
    FwdInfo->entry = Fwd;
    Fwd->y = SCREEN_HEIGHT - 28;
    Fwd->isRotateScale = false;
    Fwd->blendMode = OBJMODE_NORMAL;
    Fwd->isMosaic = false;
    Fwd->colorMode = OBJCOLOR_16;
    Fwd->shape = OBJSHAPE_SQUARE;
    Fwd->isHidden = true;
    Fwd->x = SCREEN_WIDTH - 28;
    Fwd->size = OBJSIZE_32;
    Fwd->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += ForwardTilesLen / BYTES_PER_16_COLOR_TILE;
    Fwd->priority = OBJPRIORITY_1;
    Fwd->palette = palcnt;


    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      ForwardPal,
                      &SPRITE_PALETTE_SUB[ palcnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      ForwardTiles,
                      &SPRITE_GFX_SUB[ Fwd->gfxIndex * OFFSET_MULTIPLIER ],
                      ForwardTilesLen );
    ++palcnt;

    SpriteInfo * BwdInfo = &p_spriteInfo[ BWD_ID ];
    SpriteEntry * Bwd = &p_oam->oamBuffer[ BWD_ID ];
    BwdInfo->oamId = BWD_ID;
    BwdInfo->width = 32;
    BwdInfo->height = 32;
    BwdInfo->angle = 0;
    BwdInfo->entry = Bwd;
    Bwd->y = SCREEN_HEIGHT - 28;
    Bwd->isRotateScale = false;
    Bwd->blendMode = OBJMODE_NORMAL;
    Bwd->isMosaic = false;
    Bwd->colorMode = OBJCOLOR_16;
    Bwd->shape = OBJSHAPE_SQUARE;
    Bwd->isHidden = true;
    Bwd->x = SCREEN_WIDTH - 28;
    Bwd->size = OBJSIZE_32;
    Bwd->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += BackwardTilesLen / BYTES_PER_16_COLOR_TILE;
    Bwd->priority = OBJPRIORITY_1;
    Bwd->palette = palcnt;


    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BackwardPal,
                      &SPRITE_PALETTE_SUB[ palcnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BackwardTiles,
                      &SPRITE_GFX_SUB[ Bwd->gfxIndex * OFFSET_MULTIPLIER ],
                      BackwardTilesLen );
    ++palcnt;

    //choiceBox
    int nextnext = nextAvailableTileIdx + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;

    for( int i = 0; i < 6; ++i ) {
        SpriteInfo * C1Info = &p_spriteInfo[ 2 * i + CHOICE_ID ];
        SpriteEntry * C1 = &p_oam->oamBuffer[ 2 * i + CHOICE_ID ];
        C1Info->oamId = CHOICE_ID;
        C1Info->width = 64;
        C1Info->height = 32;
        C1Info->angle = 0;
        C1Info->entry = C1;
        C1->y = 68 + ( i / 2 ) * 32;
        C1->isRotateScale = false;
        C1->blendMode = OBJMODE_NORMAL;
        C1->isMosaic = false;
        C1->colorMode = OBJCOLOR_16;
        C1->shape = OBJSHAPE_WIDE;
        C1->isHidden = true;
        C1->x = ( ( i % 2 ) ? 32 : 128 );
        C1->size = OBJSIZE_64;
        C1->gfxIndex = nextAvailableTileIdx;

        C1->priority = OBJPRIORITY_2;
        C1->palette = palcnt;

        SpriteInfo * C3Info = &p_spriteInfo[ 2 * i + CHOICE_ID + 1 ];
        SpriteEntry * C3 = &p_oam->oamBuffer[ 2 * i + CHOICE_ID + 1 ];
        C3Info->oamId = CHOICE_ID;
        C3Info->width = 64;
        C3Info->height = 32;
        C3Info->angle = 0;
        C3Info->entry = C3;
        C3->y = 68 + ( i / 2 ) * 32;
        C3->isRotateScale = false;
        C3->blendMode = OBJMODE_NORMAL;
        C3->isMosaic = false;
        C3->colorMode = OBJCOLOR_16;
        C3->shape = OBJSHAPE_WIDE;
        C3->isHidden = true;
        C3->x = ( ( i % 2 ) ? 62 : 160 );
        C3->size = OBJSIZE_64;
        C3->gfxIndex = nextnext;
        C3->priority = OBJPRIORITY_2;
        C3->palette = palcnt;
    }
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      Choice_1Pal,
                      &SPRITE_PALETTE_SUB[ palcnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      Choice_1Tiles,
                      &SPRITE_GFX_SUB[ nextAvailableTileIdx * OFFSET_MULTIPLIER ],
                      Choice_1TilesLen );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      Choice_3Tiles,
                      &SPRITE_GFX_SUB[ nextnext * OFFSET_MULTIPLIER ],
                      Choice_3TilesLen );
    nextAvailableTileIdx = nextnext + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
    for( int i = 0; i < 3; ++i ) {
        SpriteInfo * C2Info = &p_spriteInfo[ i + CHOICE_ID + 12 ];
        SpriteEntry * C2 = &p_oam->oamBuffer[ i + CHOICE_ID + 12 ];
        C2Info->oamId = CHOICE_ID;
        C2Info->width = 64;
        C2Info->height = 32;
        C2Info->angle = 0;
        C2Info->entry = C2;
        C2->y = 68 + (i)* 32;
        C2->isRotateScale = false;
        C2->blendMode = OBJMODE_NORMAL;
        C2->isMosaic = false;
        C2->colorMode = OBJCOLOR_16;
        C2->shape = OBJSHAPE_WIDE;
        C2->isHidden = true;
        C2->x = 96;
        C2->size = OBJSIZE_64;
        C2->gfxIndex = nextAvailableTileIdx;
        C2->priority = OBJPRIORITY_2;
        C2->palette = palcnt;
    }
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_2Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Choice_2TilesLen );
    nextAvailableTileIdx += Choice_2TilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;

    SpriteInfo * SQCHAInfo = &p_spriteInfo[ SQCH_ID ];
    SpriteEntry * SQCHA = &p_oam->oamBuffer[ SQCH_ID ];
    SQCHAInfo->oamId = SQCH_ID;
    SQCHAInfo->width = 16;
    SQCHAInfo->height = 16;
    SQCHAInfo->angle = 0;
    SQCHAInfo->entry = SQCHA;
    SQCHA->y = acMapPoint.second;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_SQUARE;
    SQCHA->isHidden = true;
    SQCHA->x = acMapPoint.first;
    SQCHA->size = OBJSIZE_16;
    SQCHA->gfxIndex = nextAvailableTileIdx;
    SQCHA->priority = OBJPRIORITY_0;
    SQCHA->palette = palcnt;


    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, ChSq_aPal, &SPRITE_PALETTE_SUB[ palcnt * COLORS_PER_PALETTE ], 32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, ChSq_aTiles, &SPRITE_GFX_SUB[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], ChSq_aTilesLen );
    nextAvailableTileIdx += ChSq_aTilesLen / BYTES_PER_16_COLOR_TILE;

    SQCHA = &p_oam->oamBuffer[ SQCH_ID + 1 ];
    SQCHA->y = acMapPoint.second;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_SQUARE;
    SQCHA->isHidden = true;
    SQCHA->x = acMapPoint.first;
    SQCHA->size = OBJSIZE_16;
    SQCHA->gfxIndex = nextAvailableTileIdx;
    SQCHA->priority = OBJPRIORITY_0;
    SQCHA->palette = palcnt;
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      ChSq_bTiles,
                      &SPRITE_GFX_SUB[ nextAvailableTileIdx * OFFSET_MULTIPLIER ],
                      ChSq_bTilesLen );

    nextAvailableTileIdx += ChSq_bTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;

    SpriteInfo * ButtonInfo = &p_spriteInfo[ 90 ];
    SpriteEntry * Button = &p_oam->oamBuffer[ 90 ];
    ButtonInfo->oamId = 90;
    ButtonInfo->width = 32;
    ButtonInfo->height = 32;
    ButtonInfo->angle = 0;
    ButtonInfo->entry = Button;
    Button->y = 0;
    Button->isRotateScale = false;
    Button->blendMode = OBJMODE_NORMAL;
    Button->colorMode = OBJCOLOR_16;
    Button->shape = OBJSHAPE_SQUARE;
    Button->isHidden = true;
    Button->x = 0;
    Button->size = OBJSIZE_32;
    Button->gfxIndex = nextAvailableTileIdx;
    Button->priority = OBJPRIORITY_0;
    Button->palette = palcnt;

    Button = &p_oam->oamBuffer[ 91 ];
    ButtonInfo->oamId = 91;
    ButtonInfo->width = 32;
    ButtonInfo->height = 32;
    ButtonInfo->angle = 0;
    ButtonInfo->entry = Button;
    Button->y = 0;
    Button->isRotateScale = false;
    Button->blendMode = OBJMODE_NORMAL;
    Button->colorMode = OBJCOLOR_16;
    Button->shape = OBJSHAPE_SQUARE;
    Button->isHidden = true;
    Button->x = 0;
    Button->size = OBJSIZE_32;
    Button->gfxIndex = nextAvailableTileIdx;
    Button->priority = OBJPRIORITY_0;
    Button->palette = palcnt;

    Button = &p_oam->oamBuffer[ 92 ];
    ButtonInfo->oamId = 92;
    ButtonInfo->width = 32;
    ButtonInfo->height = 32;
    ButtonInfo->angle = 0;
    ButtonInfo->entry = Button;
    Button->y = 0;
    Button->isRotateScale = false;
    Button->blendMode = OBJMODE_NORMAL;
    Button->colorMode = OBJCOLOR_16;
    Button->shape = OBJSHAPE_SQUARE;
    Button->isHidden = true;
    Button->x = 0;
    Button->size = OBJSIZE_32;
    Button->gfxIndex = nextAvailableTileIdx;
    Button->priority = OBJPRIORITY_0;
    Button->palette = palcnt;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BagSprPal, &SPRITE_PALETTE_SUB[ palcnt * COLORS_PER_PALETTE ], 32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BagSprTiles, &SPRITE_GFX_SUB[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BagSprTilesLen );

    return nextAvailableTileIdx;
}
void setMainSpriteVisibility( bool p_hidden ) {
    for( int i = 2; i <= 7; ++i )
        if( i == 2 && !SAV.m_hasPKMN )
            setSpriteVisibility( &Oam->oamBuffer[ i ], true );
        else
            setSpriteVisibility( &Oam->oamBuffer[ i ], p_hidden );
    updateOAMSub( Oam );
}

void drawSub( ) {
    memset( bgGetGfxPtr( bg2sub ), 0, 256 * 192 );
    memset( bgGetGfxPtr( bg3sub ), 0, 256 * 192 );
    dmaCopy( BorderBitmap, bgGetGfxPtr( bg2sub ), 256 * 192 );
    dmaCopy( BorderPal, BG_PALETTE_SUB, 256 * 2 );
    if( !BGs[ BG_ind ].m_loadFromRom ) {
        dmaCopy( BGs[ BG_ind ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 192 );
        dmaCopy( BGs[ BG_ind ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
    } else if( !loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind ) ) {
        dmaCopy( BGs[ 0 ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 256 );
        dmaCopy( BGs[ 0 ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
        BG_ind = 0;
    }
}

void screenLoader::draw( int p_mode ) {
    _pos = p_mode;
    SpriteEntry * back = &Oam->oamBuffer[ BACK_ID ];
    setSpriteVisibility( back, false );
    if( p_mode == 0 ) {
        drawSub( );
        setMainSpriteVisibility( true );

        for( int i = 0; i < 3; ++i ) {
            Oam->oamBuffer[ 90 + i ].isHidden = false;
            Oam->oamBuffer[ 90 + i ].x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 4 * i ] - 16;
            Oam->oamBuffer[ 90 + i ].y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 4 * i + 1 ] - 16;
            Oam->oamBuffer[ 90 + i ].priority = OBJPRIORITY_1;
        }
        updateOAMSub( Oam );
    } else if( p_mode == 1 ) {
        loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen2" );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Hoenn" );
        acMapRegion = HOENN;
        setMainSpriteVisibility( true );

        for( int i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else if( p_mode == 2 ) {
        loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen3" );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Kanto" );
        acMapRegion = KANTO;
        setMainSpriteVisibility( true );
        for( int i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else if( p_mode == 3 ) {
        loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen2_BG3_KJ" );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Johto" );
        acMapRegion = JOHTO;
        setMainSpriteVisibility( true );
        for( int i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else {

        drawSub( );
        setSpriteVisibility( back, true );
        setMainSpriteVisibility( false );
        for( int i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
        /*initOAMTableSub(Oam);
        initMainSprites(Oam,spriteInfo);*/
    }
    updateOAMSub( Oam );
}
void screenLoader::init( ) {
    //initVideoSub();
    initOAMTableSub( Oam );
    initMainSprites( Oam, spriteInfo );
    this->draw( this->_pos );
}

void drawPKMNIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const int& p_pkmnId, const int p_posX, const int p_posY,
                   int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_subScreen = true ) {

    static const int OFFSET_MULTIPLIER = 32 / sizeof( SPRITE_GFX_SUB[ 0 ] );
    SpriteInfo *ItemInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry *Item = &p_oam->oamBuffer[ p_oamIndex ];
    ItemInfo->oamId = p_oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = p_tileCnt;
    Item->priority = p_subScreen ? OBJPRIORITY_1 : OBJPRIORITY_0;
    Item->palette = p_palCnt;
    Item->x = p_posX;
    Item->y = p_posY;

    //char buffer[100];
    sprintf( buffer, "%d/Icon_%d", p_pkmnId, p_pkmnId );
    if( p_subScreen ) {
        if( !loadSpriteSub( ItemInfo, "nitro:/PICS/SPRITES/PKMN/", buffer, 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemPal,
                              &SPRITE_PALETTE_SUB[ p_palCnt * 16 ],
                              32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemTiles,
                              &SPRITE_GFX_SUB[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                              NoItemTilesLen );
        }
    } else {
        if( !loadSprite( ItemInfo, "nitro:/PICS/SPRITES/PKMN/", buffer, 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemPal,
                              &SPRITE_PALETTE[ p_palCnt * 16 ],
                              32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemTiles,
                              &SPRITE_GFX[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                              NoItemTilesLen );
        }
    }
    p_tileCnt += 512 / 32;
    ++p_palCnt;
}
void drawEggIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const int p_posX, const int p_posY,
                  int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_subScreen = true ) {

    static const int OFFSET_MULTIPLIER = 32 / sizeof( SPRITE_GFX_SUB[ 0 ] );
    SpriteInfo *ItemInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry *Item = &p_oam->oamBuffer[ p_oamIndex ];
    ItemInfo->oamId = p_oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = p_tileCnt;
    Item->priority = p_subScreen ? OBJPRIORITY_1 : OBJPRIORITY_0;
    Item->palette = p_palCnt;
    Item->x = p_posX;
    Item->y = p_posY;

    //char buffer[100];
    sprintf( buffer, "Icon_egg" );
    if( p_subScreen ) {
        if( !loadSpriteSub( ItemInfo, "nitro:/PICS/ICONS/", buffer, 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemPal,
                              &SPRITE_PALETTE_SUB[ p_palCnt * 16 ],
                              32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemTiles,
                              &SPRITE_GFX_SUB[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                              NoItemTilesLen );
        }
    } else {
        if( !loadSprite( ItemInfo, "nitro:/PICS/ICONS/", buffer, 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemPal,
                              &SPRITE_PALETTE[ p_palCnt * 16 ],
                              32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemTiles,
                              &SPRITE_GFX[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                              NoItemTilesLen );
        }
    }
    p_tileCnt += 512 / 32;
    ++p_palCnt;
}
void drawItemIcon( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const int p_posX, const int p_posY,
                   int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_subScreen ) {

    static const int OFFSET_MULTIPLIER = 32 / sizeof( SPRITE_GFX_SUB[ 0 ] );
    SpriteInfo *ItemInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry *Item = &p_oam->oamBuffer[ p_oamIndex ];
    ItemInfo->oamId = p_oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = p_tileCnt;
    Item->priority = p_subScreen ? OBJPRIORITY_1 : OBJPRIORITY_0;
    Item->palette = p_palCnt;
    Item->x = p_posX;
    Item->y = p_posY;
    if( p_subScreen ) {
        if( !loadSpriteSub( ItemInfo, "nitro:/PICS/SPRITES/ITEMS/", p_itemName.c_str( ), 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemPal,
                              &SPRITE_PALETTE_SUB[ p_palCnt * 16 ],
                              32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemTiles,
                              &SPRITE_GFX_SUB[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                              NoItemTilesLen );
        }
    } else {
        if( !loadSprite( ItemInfo, "nitro:/PICS/SPRITES/ITEMS/", p_itemName.c_str( ), 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemPal,
                              &SPRITE_PALETTE[ p_palCnt * 16 ],
                              32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                              NoItemTiles,
                              &SPRITE_GFX[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ],
                              NoItemTilesLen );
        }
    }
    p_tileCnt += 512 / 32;
    ++p_palCnt;
}
void initTop( ) {
    consoleSelect( &Top );
    printf( "\x1b[39m" );	int a = 0, b = 0, c = 0;
    initOAMTable( OamTop );
    cust_font.setColor( 0, 0 );
    cust_font.setColor( 251, 1 );
    cust_font.setColor( 252, 2 );

    BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE[ 252 ] = RGB15( 15, 15, 15 );
    BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );

    for( size_t i = 0; i < SAV.m_PkmnTeam.size( ); i++ ) {
        if( !SAV.m_PkmnTeam[ i ].m_boxdata.m_IV.m_isEgg ) {
            if( i % 2 == 0 ) {
                drawPKMNIcon( OamTop, spriteInfoTop, SAV.m_PkmnTeam[ i ].m_boxdata.m_SPEC, borders[ i ][ 0 ] * 8 - 28, borders[ i ][ 1 ] * 8, a, b, c, false );
                BATTLE::displayHP( 100, 101, borders[ i ][ 0 ] * 8 - 13, borders[ i ][ 1 ] * 8 + 8 - ( i != 2 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
                BATTLE::displayHP( 100, 100 - SAV.m_PkmnTeam[ i ].m_stats.m_acHP * 100 / SAV.m_PkmnTeam[ i ].m_stats.m_maxHP, borders[ i ][ 0 ] * 8 - 13, borders[ i ][ 1 ] * 8 + 8 - ( i != 2 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
            } else {
                drawPKMNIcon( OamTop, spriteInfoTop, SAV.m_PkmnTeam[ i ].m_boxdata.m_SPEC, borders[ i ][ 0 ] * 8 + 76, borders[ i ][ 1 ] * 8, a, b, c, false );
                BATTLE::displayHP( 100, 101, borders[ i ][ 0 ] * 8 + 63, borders[ i ][ 1 ] * 8 + 8 - ( i != 3 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
                BATTLE::displayHP( 100, 100 - SAV.m_PkmnTeam[ i ].m_stats.m_acHP * 100 / SAV.m_PkmnTeam[ i ].m_stats.m_maxHP, borders[ i ][ 0 ] * 8 + 63, borders[ i ][ 1 ] * 8 + 8 - ( i != 3 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
            }
            updateOAM( OamTop );
            char buf[ 100 ];

            int mval = 1 + ( ( i / 2 == 1 ) ? 4 : 8 );

            sprintf( buf, "%ls", SAV.m_PkmnTeam[ i ].m_boxdata.m_Name );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 - mval, false );
            sprintf( buf, "%s", POKEMON::PKMNDATA::getDisplayName( SAV.m_PkmnTeam[ i ].m_boxdata.m_SPEC ) );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 14 - mval, false );

            sprintf( buf, "%hi/%hi KP", SAV.m_PkmnTeam[ i ].m_stats.m_acHP, SAV.m_PkmnTeam[ i ].m_stats.m_maxHP );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 28 - mval, false );

            sprintf( buf, "%s", ItemList[ SAV.m_PkmnTeam[ i ].m_boxdata.getItem( ) ].getDisplayName( ).c_str( ) );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 42 - mval, false );

        } else {
            consoleSetWindow( &Top, borders[ i ][ 0 ], borders[ i ][ 1 ], 12, 6 );

            char buf[ 100 ];
            int mval = 1 + ( ( i / 2 == 1 ) ? 4 : 8 );
            sprintf( buf, "Ei" );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 - mval, false );
            sprintf( buf, "Ei" );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 14 - mval, false );

            sprintf( buf, "%s", ItemList[ SAV.m_PkmnTeam[ i ].m_boxdata.getItem( ) ].getDisplayName( ).c_str( ) );
            cust_font.printString( buf, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 42 - mval, false );

            if( i % 2 == 0 ) {
                drawEggIcon( OamTop, spriteInfoTop, borders[ i ][ 0 ] * 8 - 28, borders[ i ][ 1 ] * 8, a, b, c, false );
            } else {
                drawEggIcon( OamTop, spriteInfoTop, borders[ i ][ 0 ] * 8 + 76, borders[ i ][ 1 ] * 8, a, b, c, false );
            }
            updateOAM( OamTop );
        }
    }
    updateOAM( OamTop );
}
void clearTop( ) {
    consoleSelect( &Top );
    for( size_t i = 0; i < SAV.m_PkmnTeam.size( ); i++ ) {
        consoleSetWindow( &Top, borders[ i ][ 0 ], borders[ i ][ 1 ], 12, 6 );
        consoleClear( );
    }
    initOAMTable( OamTop );
}

int fieldCnt = 0;

void initSub( int pkmIdx ) {
    consoleSelect( &Bottom );
    consoleSetWindow( &Bottom, 16, 0, 16, 16 );
    consoleClear( );
    for( int i = 0; i < 6; ++i ) {
        Oam->oamBuffer[ 15 + 2 * i ].isHidden = true;
        Oam->oamBuffer[ 16 + 2 * i ].isHidden = true;
    }
    updateOAMSub( Oam );
    if( pkmIdx < 0 || pkmIdx >= (int)SAV.m_PkmnTeam.size( ) || SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_IV.m_isEgg ) {
        consoleSelect( &Top );
        return;
    }
    int u = 0;
    for( int i = 0; i < 4; ++i )
        if( SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_Attack[ i ] < MAXATTACK &&
            AttackList[ SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_Attack[ i ] ]->m_isFieldAttack ) {
        Oam->oamBuffer[ 15 + 2 * ( u ) ].isHidden = false;
        Oam->oamBuffer[ 16 + 2 * ( u ) ].isHidden = false;
        Oam->oamBuffer[ 15 + 2 * ( u ) ].y = -7 + 24 * u;
        Oam->oamBuffer[ 16 + 2 * ( u ) ].y = -7 + 24 * u;

        Oam->oamBuffer[ 15 + 2 * ( u ) ].x = 152;
        Oam->oamBuffer[ 16 + 2 * ( u ) ].x = 192 + 24;
        updateOAMSub( Oam );

        consoleSetWindow( &Bottom, 16, 3 * u + 1, 16, 16 );
        printf( "    %s", AttackList[ SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_Attack[ i ] ]->m_moveName.c_str( ) );
        ++u;
        }
    if( SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_Item ) {
        Oam->oamBuffer[ 15 + 2 * ( u ) ].isHidden = false;
        Oam->oamBuffer[ 16 + 2 * ( u ) ].isHidden = false;
        Oam->oamBuffer[ 15 + 2 * ( u ) ].y = -7 + 24 * u;
        Oam->oamBuffer[ 16 + 2 * ( u ) ].y = -7 + 24 * u;

        Oam->oamBuffer[ 15 + 2 * ( u ) ].x = 152;
        Oam->oamBuffer[ 16 + 2 * ( u ) ].x = 192 + 24;
        updateOAMSub( Oam );

        consoleSetWindow( &Bottom, 16, 3 * u + 1, 16, 16 );
        printf( "    Item nehmen" );
    }
    int s = u + SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_Item;
    Oam->oamBuffer[ 15 + 2 * ( s ) ].isHidden = false;
    Oam->oamBuffer[ 16 + 2 * ( s ) ].isHidden = false;
    Oam->oamBuffer[ 15 + 2 * ( s ) ].y = -7 + 24 * u;
    Oam->oamBuffer[ 16 + 2 * ( s ) ].y = -7 + 24 * u;

    Oam->oamBuffer[ 15 + 2 * ( s ) ].x = 152;
    Oam->oamBuffer[ 16 + 2 * ( s ) ].x = 192 + 24;
    updateOAMSub( Oam );

    consoleSetWindow( &Bottom, 16, 3 * s + 1, 16, 16 );
    printf( "    Dexeintrag" );
    consoleSelect( &Top );

    fieldCnt = u;
}
extern void initMapSprites( );
extern void movePlayerOnMap( int, int, int, bool );
void screenLoader::run_pkmn( ) {
    vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition touch;
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
    swiWaitForVBlank( );
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
    drawSub( );
    int acIn = 0, max = SAV.m_PkmnTeam.size( );
    consoleSelect( &Top );
    consoleClear( );
    initTop( );
    initSub( acIn );
    consoleSetWindow( &Top, positions[ acIn ][ 0 ], positions[ acIn ][ 1 ], 2, 2 );
    if( acIn & 1 )
        printf( ">" );
    else
        printf( "<" );

    SpriteEntry * back = &Oam->oamBuffer[ BACK_ID ];
    SpriteEntry * save = &Oam->oamBuffer[ SAVE_ID ];
    setSpriteVisibility( back, false );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    Oam->oamBuffer[ 8 ].isHidden = false;
    Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
    Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
    updateOAMSub( Oam );

    while( 1 ) {
        updateTime( );
        consoleSelect( &Top );
        updateOAMSub( Oam );

        swiWaitForVBlank( );
        scanKeys( );
        int pressed = keysCurrent( );
        touchRead( &touch );

        if( pressed & KEY_X || ( touch.px > 224 && touch.py > 164 ) ) {
            while( 1 ) {
                if( ( keysUp( ) & KEY_X ) )
                    break;

                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
                scanKeys( );
            }
            break;
        } else if( pressed & KEY_A || ( sqrt( sq( touch.px - 128 ) + sq( touch.py - 96 ) ) <= 16 ) ) {
            while( 2 ) {
                if( keysUp( ) & KEY_A )
                    break;

                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
                scanKeys( );
            }
            consoleClear( );
            clearTop( );
            Oam->oamBuffer[ 8 ].isHidden = true;
            updateOAMSub( Oam );
            swiWaitForVBlank( );
            while( 1 ) {
                initSub( -1 );
                int p = SAV.m_PkmnTeam[ acIn ].draw( );
                if( p & KEY_X ) {
                    for( int i = 0; i < max; i++ ) {
                        consoleSetWindow( &Top, positions[ i ][ 0 ], positions[ i ][ 1 ], 2, 2 );
                        consoleClear( );
                    }
                    clearTop( );
                    initSub( -1 );

                    initOAMTableSub( Oam );
                    initMainSprites( Oam, spriteInfo );
                    setSpriteVisibility( back, true );
                    setSpriteVisibility( save, false );
                    setMainSpriteVisibility( false );
                    Oam->oamBuffer[ 8 ].isHidden = true;
                    return;
                } else if( p & KEY_B ) {
                    initOAMTableSub( Oam );
                    initMainSprites( Oam, spriteInfo );
                    setSpriteVisibility( back, false );
                    setSpriteVisibility( save, true );
                    setMainSpriteVisibility( true );
                    Oam->oamBuffer[ 8 ].isHidden = false;
                    Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
                    Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;

                    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
                    drawSub( );

                    acIn %= max;
                    consoleSelect( &Top );
                    consoleClear( );
                    initTop( );
                    consoleSetWindow( &Top, positions[ acIn ][ 0 ], positions[ acIn ][ 1 ], 2, 2 );
                    if( acIn & 1 )
                        printf( ">" );
                    else
                        printf( "<" );

                    initSub( acIn );

                    break;
                } else if( p & KEY_UP ) {
                    if( --acIn <= -1 )
                        acIn = max - 1;
                } else if( p & KEY_DOWN ) {
                    if( ++acIn >= max )
                        acIn = 0;
                }

            }
        } else if( pressed & KEY_UP ) {
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_UP )
                    break;
            }
            if( --acIn <= -1 )
                acIn = max - 1;
            consoleClear( );
            consoleSetWindow( &Top, positions[ acIn ][ 0 ], positions[ acIn ][ 1 ], 2, 2 );
            if( acIn & 1 )
                printf( ">" );
            else
                printf( "<" );
            initSub( acIn );
        } else if( pressed & KEY_DOWN ) {
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_DOWN )
                    break;
            }
            if( ++acIn >= max )
                acIn = 0;
            consoleClear( );
            consoleSetWindow( &Top, positions[ acIn ][ 0 ], positions[ acIn ][ 1 ], 2, 2 );
            if( acIn & 1 )
                printf( ">" );
            else
                printf( "<" );
            initSub( acIn );
        } else if( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item && touch.px >= 152 && touch.py >= ( -7 + 24 * fieldCnt ) && touch.py < ( 17 + 24 * fieldCnt ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            char buf[ 50 ];
            item acI = ItemList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item ];
            SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item = 0;

            initSub( -1 );
            setSpriteVisibility( back, true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH - 28;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT - 28;
            updateOAMSub( Oam );
            sprintf( buf, "%s von %ls\nim Beutel verstaut.", acI.getDisplayName( ).c_str( ), SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Name );
            messageBox( buf, true, true );
            SAV.m_bag.addItem( acI.getItemType( ), acI.getItemId( ), 1 );

            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
            loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
            initTop( );
            setSpriteVisibility( back, false );
            Oam->oamBuffer[ 8 ].isHidden = false;
            updateOAMSub( Oam );
            initSub( acIn );
        } else if( touch.px >= 152 && touch.py >= ( -7 + 24 * ( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item + fieldCnt ) ) && touch.py < ( 17 + 24 * ( fieldCnt + SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item ) ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            item acI = ItemList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item ];
            SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Item = 0;

            initSub( -1 );
            setSpriteVisibility( back, true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH - 28;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT - 28;
            updateOAMSub( Oam );

            this->run_dex( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_SPEC - 1 );

            setSpriteVisibility( back, false );
            setSpriteVisibility( save, true );
            setMainSpriteVisibility( true );
            Oam->oamBuffer[ 8 ].isHidden = false;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
            loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
            initTop( );
            setSpriteVisibility( back, false );
            Oam->oamBuffer[ 8 ].isHidden = false;
            updateOAMSub( Oam );
            initSub( acIn );
            consoleSetWindow( &Top, positions[ acIn ][ 0 ], positions[ acIn ][ 1 ], 2, 2 );
            if( acIn & 1 )
                printf( ">" );
            else
                printf( "<" );
        }
        for( int i = 0; i < fieldCnt; ++i )
            if( touch.px >= 152 && touch.py >= ( -7 + 24 * i ) && touch.py < ( 17 + 24 * i ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            int u = 0, o;
            for( o = 0; o < 4 && u <= i; ++o )
                if( AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Attack[ o ] ]->m_isFieldAttack )
                    u++;
            o--;
            if( AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Attack[ o ] ]->possible( ) ) {
                for( int i2 = 0; i2 < max; i2++ ) {
                    consoleSetWindow( &Top, positions[ i2 ][ 0 ], positions[ i2 ][ 1 ], 2, 2 );
                    consoleClear( );
                }
                clearTop( );
                initSub( -1 );

                initOAMTableSub( Oam );
                initMainSprites( Oam, spriteInfo );
                setSpriteVisibility( back, true );
                setSpriteVisibility( save, false );
                setMainSpriteVisibility( false );
                Oam->oamBuffer[ 8 ].isHidden = true;

                this->draw( -1 );
                initMapSprites( );
                movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );

                char buf[ 50 ];
                sprintf( buf, "%ls setzt %s\nein!", SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Name, AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Attack[ o ] ]->m_moveName.c_str( ) );
                messageBox( buf, true, true );
                shoUseAttack( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_SPEC, SAV.m_PkmnTeam[ acIn ].m_boxdata.m_isFemale, SAV.m_PkmnTeam[ acIn ].m_boxdata.isShiny( ) );
                AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_Attack[ o ] ]->use( );
                return;
            } else {
                initSub( -1 );

                initOAMTableSub( Oam );
                initMainSprites( Oam, spriteInfo );
                setSpriteVisibility( back, true );
                setSpriteVisibility( save, false );
                setMainSpriteVisibility( true );
                Oam->oamBuffer[ 8 ].isHidden = true;
                //loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","PKMNScreen");
                messageBox( "Diese Attacke kann jetzt\nnicht eingesetzt werden.", "Pok�Nav" );

                setSpriteVisibility( back, false );
                setSpriteVisibility( save, true );
                setMainSpriteVisibility( true );
                Oam->oamBuffer[ 8 ].isHidden = false;
                Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
                Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
                updateOAMSub( Oam );
                initSub( acIn );
                //initTop();
            }
            }
    }
    for( int i = 0; i < max; i++ ) {
        consoleSetWindow( &Top, positions[ i ][ 0 ], positions[ i ][ 1 ], 2, 2 );
        consoleClear( );
    }
    clearTop( );
    initSub( -1 );

    initOAMTableSub( Oam );
    initMainSprites( Oam, spriteInfo );
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, false );
    setMainSpriteVisibility( false );
    Oam->oamBuffer[ 8 ].isHidden = true;
}

const int dexsppos[ 2 ][ 9 ] = { { 160, 128, 96, 19, 6, 120, 158, 196, 8 }, { -16, 0, 24, 138, 173, 108, 126, 144, 32 } };
void initDexSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, int& p_oamIndex, int& p_palCnt, int& p_tileCnt ) {
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] );
    /* Keep track of the available tiles */

    memset( SPRITE_GFX_SUB, 0, 32 );

    p_tileCnt = 16;
    p_oamIndex = 0;
    p_palCnt = 0;
    SpriteInfo * backInfo = &p_spriteInfo[ p_oamIndex ];
    SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
    backInfo->oamId = p_oamIndex;
    backInfo->width = 32;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = SCREEN_HEIGHT - 28;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = SCREEN_WIDTH - 28;
    back->rotationIndex = backInfo->oamId;
    back->size = OBJSIZE_32;
    back->gfxIndex = p_tileCnt;
    back->priority = OBJPRIORITY_0;
    back->palette = backInfo->oamId;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BackTiles,
                      &SPRITE_GFX_SUB[ back->gfxIndex * OFFSET_MULTIPLIER ],
                      BackTilesLen );
    p_tileCnt += BackTilesLen / BYTES_PER_16_COLOR_TILE;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BackPal,
                      &SPRITE_PALETTE_SUB[ backInfo->oamId * COLORS_PER_PALETTE ],
                      32 );
    p_palCnt = backInfo->oamId + 1;

    SpriteInfo * Bo4Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * Bo4 = &p_oam->oamBuffer[ p_oamIndex ];
    Bo4Info->oamId = p_oamIndex;
    Bo4Info->width = 64;
    Bo4Info->height = 64;
    Bo4Info->angle = 0;
    Bo4Info->entry = Bo4;
    Bo4->y = 0;
    Bo4->isRotateScale = false;
    Bo4->blendMode = OBJMODE_NORMAL;
    Bo4->isMosaic = false;
    Bo4->colorMode = OBJCOLOR_16;
    Bo4->shape = OBJSHAPE_SQUARE;
    Bo4->isHidden = true;//
    Bo4->x = 0;
    Bo4->size = OBJSIZE_16;//64;
    Bo4->gfxIndex = 0;//p_tileCnt;
    Bo4->priority = OBJPRIORITY_2;
    Bo4->palette = p_palCnt;
    Bo4->vFlip = true;
    Bo4->hFlip = true;

    Bo4 = &p_oam->oamBuffer[ ++p_oamIndex ];
    Bo4->y = 128;
    Bo4->isRotateScale = false;
    Bo4->blendMode = OBJMODE_NORMAL;
    Bo4->isMosaic = false;
    Bo4->colorMode = OBJCOLOR_16;
    Bo4->shape = OBJSHAPE_SQUARE;
    Bo4->isHidden = true;//false;
    Bo4->x = 192;
    Bo4->size = OBJSIZE_16;//64;
    Bo4->gfxIndex = 0;//p_tileCnt;
    Bo4->priority = OBJPRIORITY_2;
    Bo4->palette = p_palCnt;
    Bo4->vFlip = false;
    Bo4->hFlip = false;

    //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    //    Border_4Pal,
    //    &SPRITE_PALETTE_SUB[p_palCnt * COLORS_PER_PALETTE],
    //    32);
    //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    //    Border_4Tiles,
    //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
    //    Border_4TilesLen);

    p_tileCnt += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;

    SpriteInfo * Bo3Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * Bo3 = &p_oam->oamBuffer[ p_oamIndex ];
    Bo3Info->oamId = 3;
    Bo3Info->width = 64;
    Bo3Info->height = 64;
    Bo3Info->angle = 0;
    Bo3Info->entry = Bo3;
    Bo3->y = 0;
    Bo3->isRotateScale = false;
    Bo3->blendMode = OBJMODE_NORMAL;
    Bo3->isMosaic = false;
    Bo3->colorMode = OBJCOLOR_16;
    Bo3->shape = OBJSHAPE_SQUARE;
    Bo3->isHidden = true;//false;
    Bo3->x = 64;
    Bo3->size = OBJSIZE_16;//64;
    Bo3->gfxIndex = 0;//p_tileCnt;
    Bo3->priority = OBJPRIORITY_2;
    Bo3->palette = p_palCnt;
    Bo3->vFlip = true;
    Bo3->hFlip = true;

    Bo3 = &p_oam->oamBuffer[ ++p_oamIndex ];
    Bo3->y = 128;
    Bo3->isRotateScale = false;
    Bo3->blendMode = OBJMODE_NORMAL;
    Bo3->isMosaic = false;
    Bo3->colorMode = OBJCOLOR_16;
    Bo3->shape = OBJSHAPE_SQUARE;
    Bo3->isHidden = true;//false;
    Bo3->x = 128;
    Bo3->size = OBJSIZE_16;//64;
    Bo3->gfxIndex = 0;// p_tileCnt;
    Bo3->priority = OBJPRIORITY_2;
    Bo3->palette = p_palCnt;
    Bo3->vFlip = false;
    Bo3->hFlip = false;

    //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    //    Border_3Tiles,
    //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
    //    Border_3TilesLen);

    p_tileCnt += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;

    SpriteInfo * Bo2Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * Bo2 = &p_oam->oamBuffer[ p_oamIndex ];
    Bo2Info->oamId = 3;
    Bo2Info->width = 64;
    Bo2Info->height = 64;
    Bo2Info->angle = 0;
    Bo2Info->entry = Bo2;
    Bo2->y = 0;
    Bo2->isRotateScale = false;
    Bo2->blendMode = OBJMODE_NORMAL;
    Bo2->isMosaic = false;
    Bo2->colorMode = OBJCOLOR_16;
    Bo2->shape = OBJSHAPE_SQUARE;
    Bo2->isHidden = true;// false;
    Bo2->x = 128;
    Bo2->size = OBJSIZE_16;//64;
    Bo2->gfxIndex = 0;//p_tileCnt;
    Bo2->priority = OBJPRIORITY_2;
    Bo2->palette = p_palCnt;
    Bo2->vFlip = true;
    Bo2->hFlip = true;

    Bo2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    Bo2->y = 128;
    Bo2->isRotateScale = false;
    Bo2->blendMode = OBJMODE_NORMAL;
    Bo2->isMosaic = false;
    Bo2->colorMode = OBJCOLOR_16;
    Bo2->shape = OBJSHAPE_SQUARE;
    Bo2->isHidden = true;// false;
    Bo2->x = 64;
    Bo2->size = OBJSIZE_16;//64;
    Bo2->gfxIndex = 0;//p_tileCnt;
    Bo2->priority = OBJPRIORITY_2;
    Bo2->palette = p_palCnt;
    Bo2->vFlip = false;
    Bo2->hFlip = false;

    //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    //    Border_2Tiles,
    //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
    //    Border_2TilesLen);
    p_tileCnt += Border_2TilesLen / BYTES_PER_16_COLOR_TILE;

    SpriteInfo * Bo1Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * Bo1 = &p_oam->oamBuffer[ p_oamIndex ];
    Bo1Info->oamId = 3;
    Bo1Info->width = 64;
    Bo1Info->height = 64;
    Bo1Info->angle = 0;
    Bo1Info->entry = Bo1;
    Bo1->y = 0;
    Bo1->isRotateScale = false;
    Bo1->blendMode = OBJMODE_NORMAL;
    Bo1->isMosaic = false;
    Bo1->colorMode = OBJCOLOR_16;
    Bo1->shape = OBJSHAPE_SQUARE;
    Bo1->isHidden = true;// false;
    Bo1->x = 192;
    Bo1->size = OBJSIZE_16;//64;
    Bo1->gfxIndex = 0;//p_tileCnt;
    Bo1->priority = OBJPRIORITY_2;
    Bo1->palette = p_palCnt;
    Bo1->vFlip = true;
    Bo1->hFlip = true;

    Bo1 = &p_oam->oamBuffer[ ++p_oamIndex ];
    Bo1->y = 128;
    Bo1->isRotateScale = false;
    Bo1->blendMode = OBJMODE_NORMAL;
    Bo1->isMosaic = false;
    Bo1->colorMode = OBJCOLOR_16;
    Bo1->shape = OBJSHAPE_SQUARE;
    Bo1->isHidden = true;//false;
    Bo1->x = 0;
    Bo1->size = OBJSIZE_16;//64;
    Bo1->gfxIndex = 0;//p_tileCnt;
    Bo1->priority = OBJPRIORITY_2;
    Bo1->palette = p_palCnt;
    Bo1->vFlip = false;
    Bo1->hFlip = false;

    //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    //    Border_1Tiles,
    //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
    //    Border_1TilesLen);
    p_tileCnt += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;

    SpriteInfo * Bo5Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * Bo5 = &p_oam->oamBuffer[ p_oamIndex ];
    Bo5Info->oamId = 3;
    Bo5Info->width = 64;
    Bo5Info->height = 64;
    Bo5Info->angle = 0;
    Bo5Info->entry = Bo5;
    Bo5->y = 64;
    Bo5->isRotateScale = false;
    Bo5->blendMode = OBJMODE_NORMAL;
    Bo5->isMosaic = false;
    Bo5->colorMode = OBJCOLOR_16;
    Bo5->shape = OBJSHAPE_SQUARE;
    Bo5->isHidden = true;//false;
    Bo5->x = 0;
    Bo5->size = OBJSIZE_16;//64;
    Bo5->gfxIndex = 0;//p_tileCnt;
    Bo5->priority = OBJPRIORITY_2;
    Bo5->palette = p_palCnt;
    Bo5->vFlip = true;
    Bo5->hFlip = true;

    Bo5 = &p_oam->oamBuffer[ ++p_oamIndex ];
    Bo5->y = 64;
    Bo5->isRotateScale = false;
    Bo5->blendMode = OBJMODE_NORMAL;
    Bo5->isMosaic = false;
    Bo5->colorMode = OBJCOLOR_16;
    Bo5->shape = OBJSHAPE_SQUARE;
    Bo5->isHidden = true;// false;
    Bo5->x = 192;
    Bo5->size = OBJSIZE_16;//64;
    Bo5->gfxIndex = 0;//p_tileCnt;
    Bo5->priority = OBJPRIORITY_2;
    Bo5->palette = p_palCnt;
    Bo5->vFlip = false;
    Bo5->hFlip = false;

    //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    //    Border_5Tiles,
    //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
    //    Border_5TilesLen);
    p_tileCnt += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;

    ++p_palCnt;
    p_oamIndex = 10;
    for( int i = 0; i < 8; ++i ) {
        SpriteInfo * BagSprInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * BagSpr = &p_oam->oamBuffer[ p_oamIndex ];
        BagSprInfo->oamId = 11;
        BagSprInfo->width = 32;
        BagSprInfo->height = 32;
        BagSprInfo->angle = 0;
        BagSprInfo->entry = BagSpr;
        BagSpr->y = dexsppos[ 1 ][ i ];
        BagSpr->isRotateScale = false;
        BagSpr->blendMode = OBJMODE_NORMAL;
        BagSpr->isMosaic = false;
        BagSpr->colorMode = OBJCOLOR_16;
        BagSpr->shape = OBJSHAPE_SQUARE;
        BagSpr->isHidden = false;
        BagSpr->x = dexsppos[ 0 ][ i ];
        BagSpr->size = OBJSIZE_32;
        BagSpr->gfxIndex = p_tileCnt;
        BagSpr->priority = OBJPRIORITY_2;
        BagSpr->palette = p_palCnt;
    }
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BagSprPal,
                      &SPRITE_PALETTE_SUB[ p_palCnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BagSprTiles,
                      &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ],
                      BagSprTilesLen );

    p_tileCnt += BagSprTilesLen / BYTES_PER_16_COLOR_TILE;
    ++p_palCnt;
    SpriteInfo * B2Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * B2 = &p_oam->oamBuffer[ p_oamIndex ];
    B2Info->oamId = 12;
    B2Info->width = 64;
    B2Info->height = 64;
    B2Info->angle = 0;
    B2Info->entry = B2;
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = dexsppos[ 0 ][ 8 ];
    B2->y = dexsppos[ 1 ][ 8 ];

    B2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = dexsppos[ 0 ][ 8 ] + 64;
    B2->y = dexsppos[ 1 ][ 8 ];
    B2->hFlip = true;

    B2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = dexsppos[ 0 ][ 8 ];
    B2->y = dexsppos[ 1 ][ 8 ] + 64;
    B2->hFlip = false;
    B2->vFlip = true;

    B2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = dexsppos[ 0 ][ 8 ] + 64;
    B2->y = dexsppos[ 1 ][ 8 ] + 64;
    B2->hFlip = true;
    B2->vFlip = true;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BigCirc1Pal,
                      &SPRITE_PALETTE_SUB[ p_palCnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BigCirc1Tiles,
                      &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ],
                      BigCirc1TilesLen );
    p_tileCnt += BigCirc1TilesLen / BYTES_PER_16_COLOR_TILE;
    ++p_palCnt;
    updateOAMSub( p_oam );
}

void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, int& p_oamIndex, int& p_palCnt, int & p_tileCnt, Type p_type, int p_posX, int p_posY, bool p_subScreen ) {
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX_SUB[ 0 ] );

    SpriteInfo * type1Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * type1 = &p_oam->oamBuffer[ p_oamIndex ];
    type1Info->oamId = p_oamIndex;
    type1Info->width = 32;
    type1Info->height = 16;
    type1Info->angle = 0;
    type1Info->entry = type1;
    type1->y = p_posY;
    type1->isRotateScale = false;
    type1->isHidden = false;
    type1->blendMode = OBJMODE_NORMAL;
    type1->isMosaic = false;
    type1->colorMode = OBJCOLOR_16;
    type1->shape = OBJSHAPE_WIDE;
    type1->x = p_posX;
    type1->size = OBJSIZE_32;
    type1->gfxIndex = p_tileCnt;
    type1->priority = OBJPRIORITY_0;
    type1->palette = p_palCnt;

    if( p_subScreen ) {
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          TypePals[ p_type ],
                          &SPRITE_PALETTE_SUB[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        /* Copy the sprite graphics to sprite graphics memory */
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          TypeTiles[ p_type ],
                          &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ],
                          KampfTilesLen );
    } else {
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          TypePals[ p_type ],
                          &SPRITE_PALETTE[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        /* Copy the sprite graphics to sprite graphics memory */
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          TypeTiles[ p_type ],
                          &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                          KampfTilesLen );
    }
    p_tileCnt += KampfTilesLen / BYTES_PER_16_COLOR_TILE;
    ++p_palCnt;
}

void formes( OAMTable *p_oam, SpriteInfo * p_spriteInfo, int& p_oamIndex, int& p_palCnt, int & p_tileCnt, int p_pkmnId, bool p_female, POKEMON::pkmnGenderType p_pkmnGenderType ) {
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32;
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof( SPRITE_GFX[ 0 ] );

    SpriteInfo * B2Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * B2 = &p_oam->oamBuffer[ p_oamIndex ];
    B2Info->oamId = 12;
    B2Info->width = 64;
    B2Info->height = 64;
    B2Info->angle = 0;
    B2Info->entry = B2;
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = 64;
    B2->y = 48;
    p_spriteInfo[ ++p_oamIndex ] = *B2Info;
    p_oam->oamBuffer[ p_oamIndex ] = *B2;

    B2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = 64 + 64;
    B2->y = 48;
    B2->hFlip = true;
    p_spriteInfo[ ++p_oamIndex ] = *B2Info;
    p_oam->oamBuffer[ p_oamIndex ] = *B2;

    B2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = 64;
    B2->y = 48 + 64;
    B2->hFlip = false;
    B2->vFlip = true;
    p_spriteInfo[ ++p_oamIndex ] = *B2Info;
    p_oam->oamBuffer[ p_oamIndex ] = *B2;

    B2 = &p_oam->oamBuffer[ ++p_oamIndex ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = p_tileCnt;
    B2->priority = OBJPRIORITY_1;
    B2->palette = p_palCnt;
    B2->x = 64 + 64;
    B2->y = 48 + 64;
    B2->hFlip = true;
    B2->vFlip = true;
    p_spriteInfo[ ++p_oamIndex ] = *B2Info;
    p_oam->oamBuffer[ p_oamIndex ] = *B2;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BigCirc1Pal,
                      &SPRITE_PALETTE[ p_palCnt * COLORS_PER_PALETTE ],
                      32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                      BigCirc1Tiles,
                      &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                      BigCirc1TilesLen );
    p_tileCnt += BigCirc1TilesLen / BYTES_PER_16_COLOR_TILE;
    if( p_pkmnId == 0 ) {
        updateOAM( p_oam );
        loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80, 64, p_oamIndex, p_palCnt, p_tileCnt, false, false, p_female );
        return;
    } else {
        for( int i = p_oamIndex; i > p_oamIndex - 8; --i )
            if( i % 2 )
                p_oam->oamBuffer[ i ].x -= 50;
            else
                p_oam->oamBuffer[ i ].x += 50;
        updateOAM( p_oam );
        if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 - 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, false, p_female, true ) )
            loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 - 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, false, !p_female, true );
        --p_palCnt;
        if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 + 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, true, p_female ) )
            loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 + 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, true, !p_female );
    }
    SpriteInfo * C1Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * C1 = &p_oam->oamBuffer[ p_oamIndex ];
    C1Info->oamId = p_oamIndex;
    C1Info->width = 64;
    C1Info->height = 32;
    C1Info->angle = 0;
    C1Info->entry = C1;
    C1->y = 168;
    C1->isRotateScale = false;
    C1->blendMode = OBJMODE_NORMAL;
    C1->isMosaic = false;
    C1->colorMode = OBJCOLOR_16;
    C1->shape = OBJSHAPE_WIDE;
    C1->isHidden = false;
    C1->x = -8;
    C1->size = OBJSIZE_64;
    C1->gfxIndex = p_tileCnt;

    C1->priority = OBJPRIORITY_2;
    C1->palette = p_palCnt;
    p_spriteInfo[ ++p_oamIndex ] = *C1Info;
    p_oam->oamBuffer[ p_oamIndex ] = *C1;
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_1Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Choice_1TilesLen );
    p_tileCnt += Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;

    SpriteInfo * C3Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * C3 = &p_oam->oamBuffer[ p_oamIndex ];
    C3Info->oamId = p_oamIndex;
    C3Info->width = 64;
    C3Info->height = 32;
    C3Info->angle = 0;
    C3Info->entry = C3;
    C3->y = 168;
    C3->isRotateScale = false;
    C3->blendMode = OBJMODE_NORMAL;
    C3->isMosaic = false;
    C3->colorMode = OBJCOLOR_16;
    C3->shape = OBJSHAPE_WIDE;
    C3->isHidden = false;
    C3->x = 64 - 8;
    C3->size = OBJSIZE_64;
    C3->gfxIndex = p_tileCnt;
    C3->priority = OBJPRIORITY_2;
    C3->palette = p_palCnt;
    p_spriteInfo[ ++p_oamIndex ] = *C3Info;
    p_oam->oamBuffer[ p_oamIndex ] = *C3;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_1Pal, &SPRITE_PALETTE[ p_palCnt * COLORS_PER_PALETTE ], 32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_3Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Choice_3TilesLen );
    p_tileCnt += Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
    if( p_pkmnId == 0 || p_pkmnGenderType == POKEMON::GENDERLESS )
        for( int i = p_oamIndex; i > p_oamIndex - 4; --i )
            p_oam->oamBuffer[ i ].isHidden = true;
    else {
        p_oam->oamBuffer[ p_oamIndex ].x += 272 - 128;
        p_oam->oamBuffer[ p_oamIndex - 2 ].x += 272 - 128;
        consoleSetWindow( &Top, 2, 22, 31, 10 );
        if( ( p_female || p_pkmnGenderType == POKEMON::FEMALE ) && p_pkmnGenderType != POKEMON::MALE )
            printf( " weiblich          weiblich\n                 (schillernd)" );
        else
            printf( " m\x84""nnlich          m\x84""nnlich\n                 (schillernd)" );
    }
    updateOAM( p_oam );
}
void drawTopDexPage( int p_page, int p_pkmnId, int p_pkmnFormeId = 0 ) {
    initOAMTable( OamTop );
    consoleSetWindow( &Top, 0, 0, 32, 24 );
    consoleSelect( &Top );
    printf( "\x1b[37m" );
    consoleClear( );
    int a = 0, b = 0, c = 0;
    POKEMON::PKMNDATA::pokemonData acpkmndata;
    POKEMON::pkmnGenderType acG = POKEMON::GENDERLESS;
    int newformepkmn = p_pkmnId;
    if( p_page < 4 ) {
        POKEMON::PKMNDATA::getAll( p_pkmnId, acpkmndata );
        loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "DexTop" );
        if( p_page == 2 )
            memset( bgGetGfxPtr( bg3 ) + 7168, 1, 256 * 192 - 14336 );

        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ 252 ] = RGB15( 31, 31, 31 );

        if( SAV.m_inDex[ p_pkmnId - 1 ] ) {
            BG_PALETTE[ 0 ] = POKEMON::PKMNDATA::getColor( acpkmndata.m_types[ 0 ] );

            bool isFixed = ( acpkmndata.m_gender == POKEMON::GENDERLESS ) || ( acpkmndata.m_gender == POKEMON::MALE ) || ( acpkmndata.m_gender == POKEMON::FEMALE );
            p_pkmnFormeId %= acpkmndata.m_formecnt ? ( ( isFixed ? 1 : 2 )*acpkmndata.m_formecnt ) : 2;
            newformepkmn = acpkmndata.m_formecnt ? POKEMON::PKMNDATA::getForme( p_pkmnId, p_pkmnFormeId / ( isFixed ? 1 : 2 ), 0 ) : p_pkmnId;
            acG = acpkmndata.m_gender;
            if( acpkmndata.m_formecnt )
                POKEMON::PKMNDATA::getAll( newformepkmn, acpkmndata );

            BG_PALETTE[ 1 ] = POKEMON::PKMNDATA::getColor( acpkmndata.m_types[ 0 ] );
            drawPKMNIcon( OamTop, spriteInfoTop, ( p_pkmnId == 493 || p_pkmnId == 649 ) ? p_pkmnId : newformepkmn, 0, 8, a, b, c, false );
            drawTypeIcon( OamTop, spriteInfoTop, a, b, c, acpkmndata.m_types[ 0 ], 33, 35, false );
            if( acpkmndata.m_types[ 0 ] != acpkmndata.m_types[ 1 ] )
                drawTypeIcon( OamTop, spriteInfoTop, a, b, c, acpkmndata.m_types[ 1 ], 65, 35, false );
            updateOAM( OamTop );
            printf( "\n    Du hast %i dieser pokemon.\n\n", box_of_st_pkmn[ p_pkmnId - 1 ].size( ) );
            char buf[ 50 ];
            sprintf( buf, "%s", POKEMON::PKMNDATA::getDisplayName( p_pkmnId ) );
            sprintf( buf, "%s - %s", buf, POKEMON::PKMNDATA::getSpecies( p_pkmnId ) );
            cust_font.printString( buf, 36, 20, false );
            printf( "\n\n %03i", p_pkmnId );
        } else {
            printf( "\n    Keine Daten vorhanden.\n\n" );
            char buf[ 50 ];
            sprintf( buf, "???????????? - %s", POKEMON::PKMNDATA::getSpecies( 0 ) );
            cust_font.printString( buf, 36, 20, false );
            printf( "\n\n %03i", p_pkmnId );
        }
    }
    switch( p_page ) {
        case 0:{
            printf( "\x1b[37m" );
            if( SAV.m_inDex[ p_pkmnId - 1 ] ) {
                BG_PALETTE[ 42 ] = POKEMON::PKMNDATA::getColor( acpkmndata.m_types[ 1 ] );
                for( int i = 0; i < 6; ++i ) {
                    font::putrec( 19 + 40 * i, std::max( 56, 102 - acpkmndata.m_bases[ i ] / 3 ), 37 + 40 * i, 102, false, true );
                    //font::putrec(17 + 40 * i,std::min(103, 56 + acpkmndata.Bases[i] / 3),(6* (acpkmndata.Bases[i] % 3)) + 16 + 40 * i,std::min(103, 58 + acpkmndata.Bases[i] / 3),statColor[i],false);
                }
                printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                char buf[ 50 ];
                sprintf( buf, "GW.  %5.1fkg", acpkmndata.m_weight / 10.0 );
                cust_font.printString( buf, 10, 109, false );
                sprintf( buf, "GR.  %6.1fm", acpkmndata.m_size / 10.0 );
                cust_font.printString( buf, 100, 109, false );
                consoleSetWindow( &Top, 1, 16, 30, 24 );
                printf( POKEMON::PKMNDATA::getDexEntry( p_pkmnId ) );
            } else {
                printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                char buf[ 50 ];
                sprintf( buf, "GW.  ???.?kg" );
                cust_font.printString( buf, 10, 109, false );
                sprintf( buf, "GR.  ???.?m" );
                cust_font.printString( buf, 100, 109, false );
                consoleSetWindow( &Top, 1, 16, 30, 24 );
                printf( POKEMON::PKMNDATA::getDexEntry( 0 ) );
            }
            break;
        }
        case 2:{
            if( SAV.m_inDex[ p_pkmnId - 1 ] ) {
                formes( OamTop, spriteInfoTop, a, b, c, newformepkmn, p_pkmnFormeId % 2, acG );
            } else
                formes( OamTop, spriteInfoTop, a, b, c, 0, 0, POKEMON::GENDERLESS );
            break;
        }
        case 4:{
            loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BottomScreen2" );
            break;
        }
        case 5:{
            loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BottomScreen3" );
            break;
        }
        case 6:{
            loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BottomScreen2_BG3_KJ" );
            break;
        }
    }
    swiWaitForVBlank( );
}
void screenLoader::run_dex( int p_pkmnId ) {
    vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition t;	int acForme = 0;
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "DexTop", 32 );

    drawSub( );
    initOAMTableSub( Oam );
    initOAMTable( OamTop );
    int palcnt = 0, tilecnt = 0, oamInd = 0;
    initDexSprites( Oam, spriteInfo, oamInd, palcnt, tilecnt );

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );
    printf( "Gefangen: %3i", SAV.m_dex );

    int acNum = p_pkmnId, maxn = SAV.m_hasGDex ? 649 : 493, o2 = oamInd, p2 = palcnt, t2 = tilecnt, acPage = 0, acMap = acMapRegion;

    consoleSetWindow( &Top, 0, 0, 32, 24 );
    consoleSelect( &Top );
    consoleClear( );
    for( int i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
        if( i == acNum ) {
        drawTopDexPage( acPage, i + 1, acForme );
        loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true );
        --j;
        continue;
        } else
            drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );

        spriteInfo[ 16 ].entry->isHidden = true;
        updateOAMSub( Oam );
        while( 42 ) {
            updateTime( );
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            touchRead( &t );
            scanKeys( );
            int pressed = keysDown( ), up = keysUp( ), held = keysHeld( );
            if( ( t.px > 224 && t.py > 164 ) ) {
                while( ( t.px > 224 && t.py > 164 ) ) {
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                }
                consoleSelect( &Bottom );
                consoleSetWindow( &Bottom, 4, 0, 20, 3 );
                consoleClear( );
                break;
            } else if( ( pressed & KEY_DOWN ) || ( ( held & ~up ) & KEY_DOWN ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_DOWN )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + 1 ) % maxn;
                for( int i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                    drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                    loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_UP ) || ( ( held & ~up ) & KEY_UP ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_UP )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + maxn - 1 ) % maxn;
                for( int i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                    drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                    loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_R ) || ( ( held & ~up ) & KEY_R ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_R )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + 15 ) % maxn;
                for( int i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                    drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                    loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_L ) || ( ( held & ~up ) & KEY_L ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_L )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + maxn - 15 ) % maxn;
                for( int i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                    drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                    loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_LEFT ) ) {
                while( 1 ) {
                    if( keysUp( ) & KEY_LEFT )
                        break;
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                }
                if( acPage == 1 )
                    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
                spriteInfo[ 16 + acPage ].entry->isHidden = false;
                acPage = ( acPage + 2 ) % 3;
                spriteInfo[ 16 + acPage ].entry->isHidden = true;
                updateOAMSub( Oam );
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
            } else if( ( pressed & KEY_RIGHT ) ) {
                while( 1 ) {
                    if( keysUp( ) & KEY_RIGHT )
                        break;
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                }
                if( acPage == 1 )
                    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
                spriteInfo[ 16 + acPage ].entry->isHidden = false;
                acPage = ( acPage + 1 ) % 3;
                spriteInfo[ 16 + acPage ].entry->isHidden = true;
                updateOAMSub( Oam );
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
            } else if( ( pressed & KEY_SELECT ) ) {
                if( acPage > 1 )
                    while( 1 ) {
                    if( keysUp( ) & KEY_SELECT )
                        break;
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                    }
                if( acPage == 1 )
                    acMap = ( acMap + 1 ) % 3;
                else
                    acForme = ( acForme + 1 ) % 60;
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
            }
            for( int q = 0; q < 5; ++q )
                if( sqrt( sq( dexsppos[ 0 ][ q ] - t.px + 16 ) + sq( dexsppos[ 1 ][ q ] - t.py + 16 ) ) <= 16 ) {
                while( 1 ) {
                    scanKeys( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                    updateTime( );
                }
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + maxn - 3 + q + ( q > 2 ? 1 : 0 ) ) % maxn;
                for( int i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                    drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                    loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                }
            for( int q = 5; q < 8; ++q )
                if( sqrt( sq( dexsppos[ 0 ][ q ] - t.px + 16 ) + sq( dexsppos[ 1 ][ q ] - t.py + 16 ) ) <= 16 ) {
                while( 1 ) {
                    scanKeys( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                    updateTime( );
                }
                if( acPage == 1 )
                    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
                spriteInfo[ 16 + acPage ].entry->isHidden = false;
                acPage = ( q + 1 ) % 3;
                spriteInfo[ 16 + acPage ].entry->isHidden = true;
                updateOAMSub( Oam );
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
                }
        }
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleSelect( &Bottom );
        consoleClear( );
        consoleSetWindow( &Top, 0, 0, 32, 24 );
        consoleSelect( &Top );
        consoleClear( );
        initOAMTable( OamTop );
        initOAMTableSub( Oam );
        initMainSprites( Oam, spriteInfo );
        setMainSpriteVisibility( false );
        Oam->oamBuffer[ 8 ].isHidden = true;
        Oam->oamBuffer[ 0 ].isHidden = true;
        Oam->oamBuffer[ 1 ].isHidden = false;
}

#define MAXITEMSPERPAGE 12
void initBagSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_subScreen = true ) {
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                          * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
        ( p_subScreen ? sizeof( SPRITE_GFX_SUB[ 0 ] ) : sizeof( SPRITE_GFX[ 0 ] ) );
    /* Keep track of the available tiles */
    p_tileCnt = 16;
    p_oamIndex = 0;
    p_palCnt = 0;
    if( p_subScreen ) {
        SpriteInfo * backInfo = &p_spriteInfo[ p_oamIndex ];
        SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->oamId = p_oamIndex;
        backInfo->width = 32;
        backInfo->height = 32;
        backInfo->angle = 0;
        backInfo->entry = back;
        back->y = SCREEN_HEIGHT - 28;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = SCREEN_WIDTH - 28;
        back->rotationIndex = backInfo->oamId;
        back->size = OBJSIZE_32;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = backInfo->oamId;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BackTiles,
                          &SPRITE_GFX_SUB[ back->gfxIndex * OFFSET_MULTIPLIER ],
                          BackTilesLen );
        p_tileCnt += BackTilesLen / BYTES_PER_16_COLOR_TILE;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BackPal,
                          &SPRITE_PALETTE_SUB[ backInfo->oamId * COLORS_PER_PALETTE ],
                          32 );
        p_palCnt = backInfo->oamId + 1;

        SpriteInfo * Bo4Info = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Bo4 = &p_oam->oamBuffer[ p_oamIndex ];
        Bo4Info->oamId = p_oamIndex;
        Bo4Info->width = 64;
        Bo4Info->height = 64;
        Bo4Info->angle = 0;
        Bo4Info->entry = Bo4;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->size = OBJSIZE_16;//
        Bo4->gfxIndex = 0;//p_tileCnt;

        Bo4 = &p_oam->oamBuffer[ ++p_oamIndex ];
        Bo4->size = OBJSIZE_16;//64;
        Bo4->gfxIndex = 0;//p_tileCnt;

        p_tileCnt += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo3Info = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Bo3 = &p_oam->oamBuffer[ p_oamIndex ];
        Bo3Info->oamId = 3;
        Bo3Info->width = 64;
        Bo3Info->height = 64;
        Bo3Info->angle = 0;
        Bo3Info->entry = Bo3;
        Bo3->y = 0;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = false;
        Bo3->x = 64;
        Bo3->size = OBJSIZE_16;//64;
        Bo3->gfxIndex = 0;//p_tileCnt;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = p_palCnt;
        Bo3->vFlip = true;
        Bo3->hFlip = true;

        Bo3 = &p_oam->oamBuffer[ ++p_oamIndex ];
        Bo3->y = 128;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = false;
        Bo3->x = 128;
        Bo3->size = OBJSIZE_16;//64;
        Bo3->gfxIndex = 0;//p_tileCnt;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = p_palCnt;
        Bo3->vFlip = false;
        Bo3->hFlip = false;

        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        //    Border_3Tiles,
        //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
        //    Border_3TilesLen);

        p_tileCnt += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo2Info = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Bo2 = &p_oam->oamBuffer[ p_oamIndex ];
        Bo2Info->oamId = 3;
        Bo2Info->width = 64;
        Bo2Info->height = 64;
        Bo2Info->angle = 0;
        Bo2Info->entry = Bo2;
        Bo2->y = 0;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = false;
        Bo2->x = 128;
        Bo2->size = OBJSIZE_16;//64;
        Bo2->gfxIndex = 0;//p_tileCnt;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = p_palCnt;
        Bo2->vFlip = true;
        Bo2->hFlip = true;

        Bo2 = &p_oam->oamBuffer[ ++p_oamIndex ];
        Bo2->y = 128;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = false;
        Bo2->x = 64;
        Bo2->size = OBJSIZE_16;//64;
        Bo2->gfxIndex = 0;//p_tileCnt;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = p_palCnt;
        Bo2->vFlip = false;
        Bo2->hFlip = false;

        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        //    Border_2Tiles,
        //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
        //    Border_2TilesLen);
        p_tileCnt += Border_2TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo1Info = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Bo1 = &p_oam->oamBuffer[ p_oamIndex ];
        Bo1Info->oamId = 3;
        Bo1Info->width = 64;
        Bo1Info->height = 64;
        Bo1Info->angle = 0;
        Bo1Info->entry = Bo1;
        Bo1->y = 0;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = false;
        Bo1->x = 192;
        Bo1->size = OBJSIZE_16;//64;
        Bo1->gfxIndex = 0;//p_tileCnt;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = p_palCnt;
        Bo1->vFlip = true;
        Bo1->hFlip = true;

        Bo1 = &p_oam->oamBuffer[ ++p_oamIndex ];
        Bo1->y = 128;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = false;
        Bo1->x = 0;
        Bo1->size = OBJSIZE_16;//64;
        Bo1->gfxIndex = 0;//p_tileCnt;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = p_palCnt;
        Bo1->vFlip = false;
        Bo1->hFlip = false;

        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        //    Border_1Tiles,
        //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
        //    Border_1TilesLen);
        p_tileCnt += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;

        SpriteInfo * Bo5Info = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Bo5 = &p_oam->oamBuffer[ p_oamIndex ];
        Bo5Info->oamId = 3;
        Bo5Info->width = 64;
        Bo5Info->height = 64;
        Bo5Info->angle = 0;
        Bo5Info->entry = Bo5;
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 0;
        Bo5->size = OBJSIZE_16;//64;
        Bo5->gfxIndex = 0;//p_tileCnt;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = p_palCnt;
        Bo5->vFlip = true;
        Bo5->hFlip = true;

        Bo5 = &p_oam->oamBuffer[ ++p_oamIndex ];
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 192;
        Bo5->size = OBJSIZE_16;//64;
        Bo5->gfxIndex = 0;//p_tileCnt;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = p_palCnt;
        Bo5->vFlip = false;
        Bo5->hFlip = false;

        //dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        //    Border_5Tiles,
        //    &SPRITE_GFX_SUB[p_tileCnt * OFFSET_MULTIPLIER],
        //    Border_5TilesLen);
        p_tileCnt += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;
    } else {
        SpriteInfo * BoxInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Box = &p_oam->oamBuffer[ p_oamIndex ];
        BoxInfo->oamId = 1;
        BoxInfo->width = 64;
        BoxInfo->height = 64;
        BoxInfo->angle = 0;
        BoxInfo->entry = Box;
        Box->y = 0;
        Box->isRotateScale = false;
        Box->blendMode = OBJMODE_NORMAL;
        Box->isMosaic = false;
        Box->colorMode = OBJCOLOR_16;
        Box->shape = OBJSHAPE_SQUARE;
        Box->isHidden = true;
        Box->x = 0;
        Box->size = OBJSIZE_64;
        Box->gfxIndex = p_tileCnt;
        Box->priority = OBJPRIORITY_3;
        Box->palette = p_palCnt;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          ItemSpr1Tiles,
                          &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                          ItemSpr1TilesLen );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          ItemSpr1Pal,
                          &SPRITE_PALETTE[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        p_tileCnt += ItemSpr1TilesLen / BYTES_PER_16_COLOR_TILE;

        Box = &p_oam->oamBuffer[ ++p_oamIndex ];
        Box->y = 0;
        Box->isRotateScale = false;
        Box->blendMode = OBJMODE_NORMAL;
        Box->isMosaic = false;
        Box->colorMode = OBJCOLOR_16;
        Box->shape = OBJSHAPE_SQUARE;
        Box->isHidden = true;
        Box->x = 0;
        Box->size = OBJSIZE_64;
        Box->gfxIndex = p_tileCnt;
        Box->priority = OBJPRIORITY_3;
        Box->palette = p_palCnt;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          ItemSpr2Tiles,
                          &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                          ItemSpr2TilesLen );
        p_tileCnt += ItemSpr2TilesLen / BYTES_PER_16_COLOR_TILE;

        Box = &p_oam->oamBuffer[ ++p_oamIndex ];
        Box->y = 0;
        Box->isRotateScale = false;
        Box->blendMode = OBJMODE_NORMAL;
        Box->isMosaic = false;
        Box->colorMode = OBJCOLOR_16;
        Box->shape = OBJSHAPE_SQUARE;
        Box->isHidden = true;
        Box->x = 0;
        Box->size = OBJSIZE_64;
        Box->gfxIndex = p_tileCnt;
        Box->priority = OBJPRIORITY_3;
        Box->palette = p_palCnt;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          ItemSpr3Tiles,
                          &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                          ItemSpr3TilesLen );
        p_tileCnt += ItemSpr3TilesLen / BYTES_PER_16_COLOR_TILE;
    }
    ++p_palCnt;
    p_oamIndex = 10;
    for( int i = 0; i < MAXITEMSPERPAGE + 2; ++i ) {
        SpriteInfo * BagSprInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * BagSpr = &p_oam->oamBuffer[ p_oamIndex ];
        BagSprInfo->oamId = 11;
        BagSprInfo->width = 32;
        BagSprInfo->height = 32;
        BagSprInfo->angle = 0;
        BagSprInfo->entry = BagSpr;
        BagSpr->y = 0;
        BagSpr->isRotateScale = false;
        BagSpr->blendMode = OBJMODE_NORMAL;
        BagSpr->isMosaic = false;
        BagSpr->colorMode = OBJCOLOR_16;
        BagSpr->shape = OBJSHAPE_SQUARE;
        BagSpr->isHidden = true;
        BagSpr->x = 0;
        BagSpr->size = OBJSIZE_32;
        BagSpr->gfxIndex = p_tileCnt;
        BagSpr->priority = BGs[ BG_ind ].m_allowsOverlay ? OBJPRIORITY_3 : OBJPRIORITY_2;
        if( !p_subScreen )
            BagSpr->priority = OBJPRIORITY_2;
        BagSpr->palette = p_palCnt;
    }
    if( p_subScreen ) {
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSprPal,
                          &SPRITE_PALETTE_SUB[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSprTiles,
                          &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ],
                          BagSprTilesLen );
    } else {
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSprPal,
                          &SPRITE_PALETTE[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSprTiles,
                          &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                          BagSprTilesLen );
    }
    p_tileCnt += BagSprTilesLen / BYTES_PER_16_COLOR_TILE;
    ++p_palCnt;
    for( int i = 0; i < MAXITEMSPERPAGE; ++i ) {
        SpriteInfo * BagSprInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * BagSpr = &p_oam->oamBuffer[ p_oamIndex ];
        BagSprInfo->oamId = 13 + MAXITEMSPERPAGE;
        BagSprInfo->width = 32;
        BagSprInfo->height = 16;
        BagSprInfo->angle = 0;
        BagSprInfo->entry = BagSpr;
        BagSpr->y = 0;
        BagSpr->isRotateScale = false;
        BagSpr->blendMode = OBJMODE_NORMAL;
        BagSpr->isMosaic = false;
        BagSpr->colorMode = OBJCOLOR_16;
        BagSpr->shape = OBJSHAPE_WIDE;
        BagSpr->isHidden = true;
        BagSpr->x = 0;
        BagSpr->size = OBJSIZE_32;
        BagSpr->gfxIndex = p_tileCnt;
        BagSpr->priority = OBJPRIORITY_1;
        BagSpr->palette = p_palCnt;
    }
    if( p_subScreen ) {
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSpr2Pal,
                          &SPRITE_PALETTE_SUB[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSpr2Tiles,
                          &SPRITE_GFX_SUB[ p_tileCnt * OFFSET_MULTIPLIER ],
                          BagSpr2TilesLen );
    } else {
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSpr2Pal,
                          &SPRITE_PALETTE[ p_palCnt * COLORS_PER_PALETTE ],
                          32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BagSpr2Tiles,
                          &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ],
                          BagSpr2TilesLen );
    }
    p_tileCnt += BagSpr2TilesLen / BYTES_PER_16_COLOR_TILE;
    ++p_palCnt;
}

void drawItem( OAMTable* p_oam, SpriteInfo* p_spriteInfo, const std::string& p_itemName, const int p_posX, const int p_posY, const int p_itemCount,
               int& p_oamIndex, int& p_palCnt, int& p_tileCnt, bool p_subScreen = true, bool p_showItemCount = false ) {

    static const int OFFSET_MULTIPLIER = 32 / sizeof( SPRITE_GFX_SUB[ 0 ] );
    SpriteInfo *ItemInfo = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry *Item = &p_oam->oamBuffer[ p_oamIndex ];
    ItemInfo->oamId = p_oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    p_oam->oamBuffer[ p_oamIndex - 2 * MAXITEMSPERPAGE ].x = Item->x = p_posX;
    p_oam->oamBuffer[ p_oamIndex - MAXITEMSPERPAGE ].x = p_posX + 8;
    p_oam->oamBuffer[ p_oamIndex - 2 * MAXITEMSPERPAGE ].y = Item->y = p_posY;
    p_oam->oamBuffer[ p_oamIndex - MAXITEMSPERPAGE ].y = p_posY + 28;
    p_oam->oamBuffer[ p_oamIndex - 2 * MAXITEMSPERPAGE ].isHidden = false;
    p_oam->oamBuffer[ p_oamIndex - MAXITEMSPERPAGE ].isHidden = p_showItemCount;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = p_tileCnt;
    Item->priority = p_subScreen ? OBJPRIORITY_1 : OBJPRIORITY_0;
    Item->palette = p_palCnt;
    if( p_subScreen ) {
        if( !loadSpriteSub( ItemInfo, "nitro:/PICS/SPRITES/ITEMS/", p_itemName.c_str( ), 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, NoItemPal, &SPRITE_PALETTE_SUB[ p_palCnt * 16 ], 32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, NoItemTiles, &SPRITE_GFX_SUB[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ], NoItemTilesLen );
        }
    } else {
        if( !loadSprite( ItemInfo, "nitro:/PICS/SPRITES/ITEMS/", p_itemName.c_str( ), 128, 16 ) ) {
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, NoItemPal, &SPRITE_PALETTE[ p_palCnt * 16 ], 32 );
            dmaCopyHalfWords( SPRITE_DMA_CHANNEL, NoItemTiles, &SPRITE_GFX[ ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER ], NoItemTilesLen );
        }
    }
    p_tileCnt += p_spriteInfo->height * p_spriteInfo->width / 32;
    ++p_palCnt;
    if( p_subScreen ) {
        updateOAMSub( p_oam );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, ( p_posX + 10 ) / 8, ( p_posY + 32 ) / 8, 3, 1 );
    } else {
        updateOAM( p_oam );
        consoleSelect( &Top );
        consoleSetWindow( &Top, ( p_posX + 10 ) / 8, ( p_posY + 32 ) / 8, 3, 1 );
    }
    if( !p_showItemCount )
        printf( "%3i", p_itemCount );
}
const int MAXPERM = 14;
void getRanPerm( int* p_array, int* p_out ) {
    int used = 0;
    for( int i = 0; i < MAXPERM; ++i ) {
        int newInd = rand( ) % MAXPERM;
        while( used & ( 1 << newInd ) )
            newInd = rand( ) % MAXPERM;
        p_out[ newInd ] = p_array[ i ];
        used |= ( 1 << newInd );
    }
}

const std::string choi[ 6 ] = { "\nEinsetzen.", "\nEinem pokemon geben.", "\nRegistrieren.", "\nWeitere Daten ansehen." };
int getAnswer( item::itemType p_bagtype ) {
    touchPosition t;
    if( p_bagtype != item::BERRIES ) {
        ( Oam->oamBuffer[ 20 ] ).isHidden = false;
        ( Oam->oamBuffer[ 21 ] ).isHidden = false;
        ( Oam->oamBuffer[ 29 ] ).isHidden = false;
        ( Oam->oamBuffer[ 24 ] ).isHidden = false;
        ( Oam->oamBuffer[ 25 ] ).isHidden = false;
        ( Oam->oamBuffer[ 28 ] ).isHidden = false;
        ( Oam->oamBuffer[ 20 ] ).y -= 32;
        ( Oam->oamBuffer[ 21 ] ).y -= 32;
        ( Oam->oamBuffer[ 28 ] ).y -= 32;
        ( Oam->oamBuffer[ 24 ] ).y -= 32;
        ( Oam->oamBuffer[ 25 ] ).y -= 32;
        ( Oam->oamBuffer[ 29 ] ).y -= 32;
        updateOAMSub( Oam );

        consoleSetWindow( &Bottom, 5, 9, 22, 3 );
        printf( choi[ 0 ].c_str( ) );

        consoleSetWindow( &Bottom, 5, 13, 22, 3 );
        if( p_bagtype != item::KEY_ITEM )
            printf( choi[ 1 ].c_str( ) );
        else
            printf( choi[ 2 ].c_str( ) );

        while( 42 ) {
            updateTime( );
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            scanKeys( );
            touchRead( &t );
            if( t.px > 224 && t.py > 164 ) {
                while( 1 ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                consoleSelect( &Bottom );
                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                consoleClear( );
                return 4;
            }

            if( t.px > 31 && t.py > 67 && t.px < 225 && t.py < 101 ) {
                ( Oam->oamBuffer[ 20 ] ).isHidden = true;
                ( Oam->oamBuffer[ 21 ] ).isHidden = true;
                ( Oam->oamBuffer[ 28 ] ).isHidden = true;
                updateOAMSub( Oam );
                while( 1 ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                ( Oam->oamBuffer[ 24 ] ).isHidden = true;
                ( Oam->oamBuffer[ 25 ] ).isHidden = true;
                ( Oam->oamBuffer[ 29 ] ).isHidden = true;
                ( Oam->oamBuffer[ 20 ] ).y += 32;
                ( Oam->oamBuffer[ 21 ] ).y += 32;
                ( Oam->oamBuffer[ 28 ] ).y += 32;
                ( Oam->oamBuffer[ 24 ] ).y += 32;
                ( Oam->oamBuffer[ 25 ] ).y += 32;
                ( Oam->oamBuffer[ 29 ] ).y += 32;
                updateOAMSub( Oam );
                consoleSetWindow( &Bottom, 1, 1, 30, 24 );
                consoleClear( );

                return 0;
            } else if( t.px > 31 && t.py > 99 && t.px < 225 && t.py < 143 ) {
                ( Oam->oamBuffer[ 24 ] ).isHidden = true;
                ( Oam->oamBuffer[ 25 ] ).isHidden = true;
                ( Oam->oamBuffer[ 29 ] ).isHidden = true;
                updateOAMSub( Oam );
                while( 1 ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                ( Oam->oamBuffer[ 20 ] ).isHidden = true;
                ( Oam->oamBuffer[ 21 ] ).isHidden = true;
                ( Oam->oamBuffer[ 28 ] ).isHidden = true;
                ( Oam->oamBuffer[ 20 ] ).y += 32;
                ( Oam->oamBuffer[ 21 ] ).y += 32;
                ( Oam->oamBuffer[ 28 ] ).y += 32;
                ( Oam->oamBuffer[ 24 ] ).y += 32;
                ( Oam->oamBuffer[ 25 ] ).y += 32;
                ( Oam->oamBuffer[ 29 ] ).y += 32;
                updateOAMSub( Oam );
                consoleSetWindow( &Bottom, 1, 1, 30, 24 );
                consoleClear( );

                return 1;
            }
        }
    } else {
        for( int i = 0; i < 3; ++i ) {
            ( Oam->oamBuffer[ 17 + 4 * i ] ).isHidden = false;
            ( Oam->oamBuffer[ 17 + 4 * i ] ).y -= 16;
            ( Oam->oamBuffer[ 16 + 4 * i ] ).isHidden = false;
            ( Oam->oamBuffer[ 16 + 4 * i ] ).y -= 16;
            ( Oam->oamBuffer[ 27 + i ] ).isHidden = false;
            ( Oam->oamBuffer[ 27 + i ] ).y -= 16;
            updateOAMSub( Oam );
            consoleSetWindow( &Bottom, 5, 7 + 4 * i, 22, 3 );
            if( i == 2 )
                ++i;
            printf( choi[ i ].c_str( ) );
        }
        while( 42 ) {
            updateTime( );
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            scanKeys( );
            touchRead( &t );
            if( t.px>224 && t.py > 164 ) {
                while( 1 ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                consoleSelect( &Bottom );
                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                consoleClear( );
                return 4;
            }
            if( t.px > 31 && t.py > 51 && t.px < 225 && t.py < 85 ) {
                Oam->oamBuffer[ 17 ].isHidden = true;
                Oam->oamBuffer[ 16 ].isHidden = true;
                Oam->oamBuffer[ 27 ].isHidden = true;
                updateOAMSub( Oam );
                while( 1 ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                consoleSetWindow( &Bottom, 1, 1, 30, 24 );
                consoleClear( );
                for( int i = 0; i < 3; ++i ) {
                    ( Oam->oamBuffer[ 17 + 4 * i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 17 + 4 * i ] ).y += 16;
                    ( Oam->oamBuffer[ 16 + 4 * i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 16 + 4 * i ] ).y += 16;
                    ( Oam->oamBuffer[ 27 + i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 27 + i ] ).y += 16;
                    updateOAMSub( Oam );
                }
                return 0;
            } else if( t.px > 31 && t.py > 83 && t.px < 225 && t.py < 116 ) {
                Oam->oamBuffer[ 20 ].isHidden = true;
                Oam->oamBuffer[ 21 ].isHidden = true;
                Oam->oamBuffer[ 28 ].isHidden = true;
                updateOAMSub( Oam );
                while( 1 ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                consoleSetWindow( &Bottom, 1, 1, 30, 24 );
                consoleClear( );
                for( int i = 0; i < 3; ++i ) {
                    ( Oam->oamBuffer[ 17 + 4 * i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 17 + 4 * i ] ).y += 16;
                    ( Oam->oamBuffer[ 16 + 4 * i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 16 + 4 * i ] ).y += 16;
                    ( Oam->oamBuffer[ 27 + i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 27 + i ] ).y += 16;
                    updateOAMSub( Oam );
                }
                return 1;
            } else if( t.px > 31 && t.py > 115 && t.px < 225 && t.py < 148 ) {
                Oam->oamBuffer[ 24 ].isHidden = true;
                Oam->oamBuffer[ 25 ].isHidden = true;
                Oam->oamBuffer[ 29 ].isHidden = true;
                updateOAMSub( Oam );
                while( 1 ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    updateTime( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                consoleSetWindow( &Bottom, 1, 1, 30, 24 );
                consoleClear( );

                for( int i = 0; i < 3; ++i ) {
                    ( Oam->oamBuffer[ 17 + 4 * i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 17 + 4 * i ] ).y += 16;
                    ( Oam->oamBuffer[ 16 + 4 * i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 16 + 4 * i ] ).y += 16;
                    ( Oam->oamBuffer[ 27 + i ] ).isHidden = true;
                    ( Oam->oamBuffer[ 27 + i ] ).y += 16;
                    updateOAMSub( Oam );
                }
                return 2;
            }
        }
    }
}
std::string bagnames[ 8 ] = { "Items", "Schl\x81""sselitems", "TM/VM", "Briefe", "Medizin", "Beeren", "Pok\x82""b\x84""lle", "Kampfitems" };
void drawBagPage( int p_page, int* p_position, int &p_oamIndex, int& p_palCnt, int& p_tileCnt, int &p_oamIndexTop, int& p_palCntTop, int& p_tileCntTop ) {
    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );
    for( int j = 11; j < 19; ++j ) {
        Oam->oamBuffer[ j + 2 ].isHidden = true;
        updateOAMSub( Oam );
    }
    int ind = 214 + 4 * ( p_page ), maxpage = 1 + ( ( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) ) - 1 ) / 12;
    printf( "%c%c %s\n%c%c  1/%i", ind, ind + 1, bagnames[ p_page ].c_str( ), ind + 2, ind + 3, maxpage );

    Oam->oamBuffer[ 11 ].x = SCREEN_WIDTH - 22;
    Oam->oamBuffer[ 11 ].y = SCREEN_HEIGHT - 49;
    Oam->oamBuffer[ 11 ].priority = OBJPRIORITY_1;
    Oam->oamBuffer[ 12 ].x = SCREEN_WIDTH - 49;
    Oam->oamBuffer[ 12 ].y = SCREEN_HEIGHT - 22;
    Oam->oamBuffer[ 12 ].priority = OBJPRIORITY_1;

    if( maxpage > 1 ) {
        Oam->oamBuffer[ 11 ].isHidden = false;
        updateOAMSub( Oam );
        consoleSetWindow( &Bottom, 31, 19, 2, 2 );
        printf( "%c\n%c", 246, 247 );
    }

    int cpy[ MAXPERM ];
    getRanPerm( p_position, cpy );
    for( int i = 0; i < MAXPERM; ++i )
        p_position[ i ] = cpy[ i ];

    int acpage = 0, oam2 = p_oamIndex, pal2 = p_palCnt, tile2 = p_tileCnt;
    touchPosition t;
    for( int i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
        updateTime( );
        scanKeys( );
        touchRead( &t );
        if( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 && acpage < maxpage - 1 )
            goto NEXT;
        if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
            goto PREV;
        if( t.px>224 && t.py > 164 )
            goto BACK;
        std::pair<int, int> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
        drawItem( Oam, spriteInfo, ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                  18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == item::KEY_ITEM );
    }
    while( 42 ) {
        updateTime( );
        swiWaitForVBlank( );
        updateOAMSub( Oam );
        scanKeys( );
        touchRead( &t );
        //int pressed = keysUp(),held = keysHeld();
        if( t.px > 224 && t.py > 164 ) {
BACK:
            while( 1 ) {
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            return;
        } else if( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 && acpage < maxpage - 1 ) {
NEXT:
            while( 1 ) {
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            acpage++;
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            printf( "%c%c %s\n%c%c %2i/%i", ind, ind + 1, bagnames[ p_page ].c_str( ), ind + 2, ind + 3, acpage + 1, maxpage );

            Oam->oamBuffer[ 12 ].isHidden = false;
            consoleSetWindow( &Bottom, 27, 22, 2, 2 );
            printf( "%c\n%c", 248, 249 );
            consoleSetWindow( &Bottom, 31, 19, 2, 2 );
            printf( "%c\n%c", 246, 247 );
            if( maxpage == acpage + 1 ) {
                Oam->oamBuffer[ 11 ].isHidden = true;
                consoleSetWindow( &Bottom, 31, 19, 2, 2 );
                consoleClear( );
            }
            updateOAMSub( Oam );
            getRanPerm( p_position, cpy );
            for( int i = 0; i < MAXPERM; ++i )
                p_position[ i ] = cpy[ i ];
            oam2 = p_oamIndex;
            pal2 = p_palCnt;
            tile2 = p_tileCnt;
            for( int i = 13; i < oam2 + 13; ++i ) Oam->oamBuffer[ i ].isHidden = true;
            updateOAMSub( Oam );
            for( int i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                updateTime( );
                scanKeys( );
                touchRead( &t );
                if( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 && acpage < maxpage - 1 )
                    goto NEXT;
                if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
                    goto PREV;
                if( t.px>224 && t.py > 164 )
                    goto BACK;
                std::pair<int, int> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                drawItem( Oam, spriteInfo, ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                          18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == item::KEY_ITEM );
            }
        } else if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 ) {
PREV:
            while( 1 ) {
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            acpage--;
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            printf( "%c%c %s\n%c%c %2i/%i", ind, ind + 1, bagnames[ p_page ].c_str( ), ind + 2, ind + 3, acpage + 1, maxpage );

            Oam->oamBuffer[ 11 ].isHidden = false;
            consoleSetWindow( &Bottom, 31, 19, 2, 2 );
            printf( "%c\n%c", 246, 247 );
            consoleSetWindow( &Bottom, 27, 22, 2, 2 );
            printf( "%c\n%c", 248, 249 );
            if( 0 == acpage ) {
                Oam->oamBuffer[ 12 ].isHidden = true;
                consoleSetWindow( &Bottom, 27, 22, 2, 2 );
                consoleClear( );
            }
            updateOAMSub( Oam );
            getRanPerm( p_position, cpy );
            for( int i = 0; i < MAXPERM; ++i )
                p_position[ i ] = cpy[ i ];
            oam2 = p_oamIndex;
            pal2 = p_palCnt;
            tile2 = p_tileCnt;
            for( int i = 13; i < oam2 + 13; ++i ) Oam->oamBuffer[ i ].isHidden = true;
            updateOAMSub( Oam );
            for( int i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                updateTime( );
                scanKeys( );
                touchRead( &t );
                if( ( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 ) && acpage < maxpage - 1 )
                    goto NEXT;
                if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
                    goto PREV;
                if( t.px>224 && t.py > 164 )
                    goto BACK;
                std::pair<int, int> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                drawItem( Oam, spriteInfo, ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                          18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == item::KEY_ITEM );
            }
        } else
            for( int io = 0; io < std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++io )
                if( sqrt( sq( ( 54 + ( p_position[ io ] % 4 * 48 ) ) - t.px ) + sq( ( 34 + ( p_position[ io ] / 4 ) * 40 ) - t.py ) ) <= 16 ) {
            while( 1 ) {
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            oam2 = p_oamIndex;
            pal2 = p_palCnt;
            tile2 = p_tileCnt;
            consoleSelect( &Top );
            consoleSetWindow( &Top, 0, 0, 32, 24 );
            consoleClear( );
            OamTop->oamBuffer[ 1 ].isHidden = OamTop->oamBuffer[ 2 ].isHidden = OamTop->oamBuffer[ 3 ].isHidden = false;
            OamTop->oamBuffer[ 1 ].x = 36;
            OamTop->oamBuffer[ 2 ].x = 100;
            OamTop->oamBuffer[ 3 ].x = 164;
            OamTop->oamBuffer[ 1 ].y = OamTop->oamBuffer[ 2 ].y = OamTop->oamBuffer[ 3 ].y = 69;
            updateOAM( OamTop );

            std::pair<int, int> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), acpage * 12 + io );
            drawItem( OamTop, spriteInfo, ItemList[ acElem.first ].m_itemName, 22,
                      50, acElem.second, oam2, pal2, tile2, false, p_page == item::KEY_ITEM );
            consoleSetWindow( &Top, 7, 9, 13, 1 );
            consoleSelect( &Top );
            printf( ItemList[ acElem.first ].getDisplayName( ).c_str( ) );
            consoleSetWindow( &Top, 5, 11, 24, 10 );
            printf( ( ItemList[ acElem.first ].getDescription( ) ).c_str( ) );

            for( int i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                if( i - acpage * 12 == io )
                    continue;
                else if( p_position[ i - acpage * 12 ] < 6 )
                    drawItem( OamTop, spriteInfo, ItemList[ acElem.first ].m_itemName, 6 + p_position[ i - acpage * 12 ] * 40,
                    10, acElem.second, oam2, pal2, tile2, false, p_page == item::KEY_ITEM );
                else if( p_position[ i - acpage * 12 ] < 12 )
                    drawItem( OamTop, spriteInfo, ItemList[ acElem.first ].m_itemName, 6 + ( p_position[ i - acpage * 12 ] - 6 ) * 40,
                    138, acElem.second, oam2, pal2, tile2, false, p_page == item::KEY_ITEM );
                else if( p_position[ i - acpage * 12 ] == 12 )
                    drawItem( OamTop, spriteInfo, ItemList[ acElem.first ].m_itemName, 238,
                    82, acElem.second, oam2, pal2, tile2, false, p_page == item::KEY_ITEM );
                else if( p_position[ i - acpage * 12 ] == 13 )
                    drawItem( OamTop, spriteInfo, ItemList[ acElem.first ].m_itemName, -10,
                    82, acElem.second, oam2, pal2, tile2, false, p_page == item::KEY_ITEM );

                updateTime( );
            }


            initOAMTableSub( Oam );
            initMainSprites( Oam, spriteInfo );
            setMainSpriteVisibility( true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 0 ].isHidden = false;
            Oam->oamBuffer[ 1 ].isHidden = true;
            for( int i = 9; i <= 12; ++i )
                Oam->oamBuffer[ i ].isHidden = true;

            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            consoleSetWindow( &Bottom, 1, 1, 30, MAXLINES );
            swiWaitForVBlank( );
            updateOAMSub( Oam );

            int acItem = SAV.m_bag.elementAt( ( bag::bagtype )p_page, acpage * 12 + io ).first;
            int ret = getAnswer( ItemList[ acItem ].getItemType( ) );

            if( ret == 0 ) {

            } else if( ret == 1 &&
                       ItemList[ SAV.m_bag.elementAt( ( bag::bagtype )p_page, acpage * 12 + io ).first ].getItemType( ) != item::KEY_ITEM ) {
                int a = 2, b = 2, c = 81;
                int num = (int)SAV.m_PkmnTeam.size( );
                consoleSelect( &Bottom );
                for( int i = 17; i < 26; i += ( ( ( i - 15 ) / 2 ) % 2 ? -2 : +6 ) ) {
                    if( ( ( ( i - 15 ) / 2 ) ^ 1 ) >= num )
                        break;
                    ( Oam->oamBuffer[ i ] ).isHidden = false;
                    ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
                    ( Oam->oamBuffer[ i + 1 ] ).y -= 16 * ( 2 - ( ( i - 15 ) / 4 ) );
                    ( Oam->oamBuffer[ i ] ).y -= 16 * ( 2 - ( ( i - 15 ) / 4 ) );
                    updateOAMSub( Oam );
                    consoleSetWindow( &Bottom, ( ( Oam->oamBuffer[ i ] ).x + 6 ) / 8, ( ( Oam->oamBuffer[ i ] ).y + 6 ) / 8, 12, 3 );
                    if( !SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_IV.m_isEgg ) {
                        printf( "   %3i/%3i\n ", SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_stats.m_acHP, SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_stats.m_maxHP );
                        wprintf( SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_Name ); printf( "\n" );
                        printf( "%11s", ItemList[ SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_Item ].getDisplayName( ).c_str( ) );
                        drawPKMNIcon( Oam, spriteInfo, SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_SPEC, ( Oam->oamBuffer[ i ] ).x - 4, ( Oam->oamBuffer[ i ] ).y - 20, a, b, c );
                    } else {
                        printf( " \n " );
                        printf( "Ei" ); printf( "\n" );
                        drawEggIcon( Oam, spriteInfo, ( Oam->oamBuffer[ i ] ).x - 4, ( Oam->oamBuffer[ i ] ).y - 20, a, b, c );
                    }
                    updateOAMSub( Oam );
                }
                updateOAMSub( Oam );
                while( 42 ) {
                    swiWaitForVBlank( );
                    updateOAMSub( Oam );
                    updateTime( );
                    touchRead( &t );

                    if( t.px > 224 && t.py > 164 ) {
                        while( 1 ) {
                            scanKeys( );
                            swiWaitForVBlank( );
                            updateTime( );
                            auto t = touchReadXY( );
                            if( t.px == 0 && t.py == 0 )
                                break;
                        }
                        consoleSelect( &Bottom );
                        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                        consoleClear( );
                        break;
                    }
                    for( int i = 17; i < 26; i += ( ( ( i - 15 ) / 2 ) % 2 ? -2 : +6 ) ) {
                        if( ( ( ( i - 15 ) / 2 ) ^ 1 ) >= num )
                            break;
                        else if( SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_IV.m_isEgg )
                            continue;
                        else if( t.px > Oam->oamBuffer[ i ].x && t.py > Oam->oamBuffer[ i ].y && t.px - 64 < Oam->oamBuffer[ i + 1 ].x && t.py - 32 < Oam->oamBuffer[ i ].y ) {
                            ( Oam->oamBuffer[ i ] ).isHidden = true;
                            ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
                            ( Oam->oamBuffer[ 3 + ( ( ( i - 15 ) / 2 ) ^ 1 ) ] ).isHidden = true;
                            updateOAMSub( Oam );
                            while( 1 ) {
                                swiWaitForVBlank( );
                                scanKeys( );
                                updateTime( );
                                auto t = touchReadXY( );
                                if( t.px == 0 && t.py == 0 )
                                    break;
                            }
                            consoleSetWindow( &Bottom, 1, 1, 30, 24 );
                            consoleClear( );
                            {
                                POKEMON::pokemon& ac = SAV.m_PkmnTeam[ ( ( ( i - 15 ) / 2 ) ^ 1 ) ];
                                initOAMTableSub( Oam );
                                initMainSprites( Oam, spriteInfo );
                                setMainSpriteVisibility( true );
                                Oam->oamBuffer[ 8 ].isHidden = true;
                                Oam->oamBuffer[ 0 ].isHidden = false;
                                Oam->oamBuffer[ 1 ].isHidden = true;
                                if( ac.m_boxdata.m_Item != 0 ) {//pokemon hat schon Item 

                                    char buf[ 100 ];
                                    sprintf( buf, "%ls h�lt bereits\ndas Item %s.\nSollen die Items getauscht werden?", ac.m_boxdata.m_Name, ItemList[ ac.m_boxdata.m_Item ].getDisplayName( ).c_str( ) );
                                    messageBox M = messageBox( "", 0, true, false, false );
                                    if( yesNoBox( M ).getResult( buf ) ) {
                                        int pkmnOldItem = ac.m_boxdata.m_Item;
                                        ac.m_boxdata.m_Item = acItem;
                                        SAV.m_bag.removeItem( bag::bagtype( p_page ), acItem, 1 );
                                        SAV.m_bag.addItem( ItemList[ pkmnOldItem ].m_itemType, pkmnOldItem, 1 );
                                    } else {
                                        goto OUT;
                                    }
                                } else {
                                    ac.m_boxdata.m_Item = acItem;
                                    SAV.m_bag.removeItem( bag::bagtype( p_page ), acItem, 1 );
                                }

                                goto OUT;
                            }
                        }
                    }
                }
OUT:
                ;
            } else if( ret == 2 ) {
            } else if( ret == 3 ) {
            }

            //todo: some code's still missing

            initOAMTable( OamTop );
            initOAMTableSub( Oam );

            loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BagTop" );

            consoleSelect( &Top );
            consoleSetWindow( &Top, 0, 0, 32, 24 );
            consoleClear( );
            for( int i = 0; i < 8; ++i ) {
                consoleSetWindow( &Top, 0, 3 * i, 12, 3 );
                int acIn = 214 + 4 * i;
                int s = SAV.m_bag.size( ( bag::bagtype )i );
                char buf[ 50 ];
                printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
                if( s == 0 )
                    sprintf( buf, "  0 Items" );
                else if( s == 1 )
                    sprintf( buf, "  1 Item" );
                else
                    sprintf( buf, "%3i Items", s );

                cust_font.printString( buf, 20, 24 * i + 3, false );
            }
            oam2 = p_oamIndex;
            pal2 = p_palCnt;
            tile2 = p_tileCnt;

            initBagSprites( OamTop, spriteInfoTop, p_oamIndexTop, p_palCntTop, p_tileCntTop, false );
            updateOAM( OamTop );
            initBagSprites( Oam, spriteInfo, p_oamIndex, p_palCnt, p_tileCnt );
            swiWaitForVBlank( );
            updateOAMSub( Oam );

            Oam->oamBuffer[ 11 ].x = SCREEN_WIDTH - 22;
            Oam->oamBuffer[ 11 ].y = SCREEN_HEIGHT - 49;
            Oam->oamBuffer[ 11 ].priority = OBJPRIORITY_1;
            Oam->oamBuffer[ 12 ].x = SCREEN_WIDTH - 49;
            Oam->oamBuffer[ 12 ].y = SCREEN_HEIGHT - 22;
            Oam->oamBuffer[ 12 ].priority = OBJPRIORITY_1;
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            printf( "%c%c %s        \n%c%c %2i/%i", ind, ind + 1, bagnames[ p_page ].c_str( ), ind + 2, ind + 3, acpage + 1, maxpage );

            Oam->oamBuffer[ 11 ].isHidden = false;
            Oam->oamBuffer[ 12 ].isHidden = false;
            swiWaitForVBlank( );
            updateOAMSub( Oam );

            consoleSetWindow( &Bottom, 31, 19, 2, 2 );
            printf( "%c\n%c", 246, 247 );
            consoleSetWindow( &Bottom, 27, 22, 2, 2 );
            printf( "%c\n%c", 248, 249 );
            if( 0 == acpage ) {
                Oam->oamBuffer[ 12 ].isHidden = true;
                updateOAMSub( Oam );
                consoleSetWindow( &Bottom, 27, 22, 2, 2 );
                consoleClear( );
            }
            if( maxpage == acpage + 1 ) {
                Oam->oamBuffer[ 11 ].isHidden = true;
                updateOAMSub( Oam );
                consoleSetWindow( &Bottom, 31, 19, 2, 2 );
                consoleClear( );
            }
            getRanPerm( p_position, cpy );
            for( int i = 0; i < MAXPERM; ++i )
                p_position[ i ] = cpy[ i ];

            for( int j = 0; j <= 10; ++j )
                Oam->oamBuffer[ j ].isHidden = false;
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            for( int i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                updateTime( );
                scanKeys( );
                touchRead( &t );
                if( ( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 ) && acpage < maxpage - 1 )
                    goto NEXT;
                if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
                    goto PREV;
                if( t.px>224 && t.py > 164 )
                    goto BACK;
                auto acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                drawItem( Oam, spriteInfo, ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                          18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == item::KEY_ITEM );
                updateTime( );
            }
                }
    }
}
void bag::draw( ) {
    vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition t;
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BagTop" );

    consoleSelect( &Top );
    for( int i = 0; i < 8; ++i ) {
        consoleSetWindow( &Top, 0, 3 * i, 12, 3 );
        int acIn = 214 + 4 * i;
        int s = ( int )this->m_bags[ i ].size( );
        char buf[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
        if( s == 0 )
            sprintf( buf, "  0 Items" );
        else if( s == 1 )
            sprintf( buf, "  1 Item" );
        else
            sprintf( buf, "%3i Items", s );

        cust_font.printString( buf, 20, 24 * i + 3, false );
    }
    drawSub( );
    initOAMTableSub( Oam );
    int palcnt = 0, tilecnt = 0, oamInd = 0;
    initBagSprites( Oam, spriteInfo, oamInd, palcnt, tilecnt );


    initOAMTable( OamTop );
    int palcntT = 0, tilecntT = 0, oamIndT = 0;
    initBagSprites( OamTop, spriteInfoTop, oamIndT, palcntT, tilecntT, false );

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );

    int positions[ 8 ][ 2 ] = { { 112, 32 }, { 144, 48 }, { 160, 80 }, { 144, 112 }, { 112, 128 }, { 80, 112 }, { 64, 80 }, { 80, 48 } };
    //Goods,Keys,TMs,Mails,Medicine,Berries,Pok�Balls,BattleItems
    for( int i = 11; i < 19; ++i ) {
        Oam->oamBuffer[ i + 2 ].x = positions[ i - 11 ][ 0 ];
        Oam->oamBuffer[ i + 2 ].y = positions[ i - 11 ][ 1 ];
        Oam->oamBuffer[ i + 2 ].isHidden = false;
        updateOAMSub( Oam );

        int nx = positions[ i - 11 ][ 0 ] / 8 + 1, ny = positions[ i - 11 ][ 1 ] / 8 + 1, ind = 214 + 4 * ( i - 11 );
        consoleSetWindow( &Bottom, nx, ny, 2, 2 );
        for( int p = 0; p < 4; ++p )
            printf( "%c", ind + p );
    }
    int pos[ MAXPERM ], cpy[ MAXPERM ] = { 0 };
    for( int i = 0; i < MAXPERM; ++i )
        pos[ i ] = i;
    getRanPerm( pos, cpy );
    for( int i = 0; i < MAXPERM; ++i )
        pos[ i ] = cpy[ i ];

    while( 42 ) {
        updateTime( );
        swiWaitForVBlank( );
        updateOAMSub( Oam );
        scanKeys( );
        touchRead( &t );
        //int pressed = keysUp(),held = keysHeld();
        if( t.px>224 && t.py > 164 ) {
            while( 1 ) {
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 4, 0, 20, 3 );
            consoleClear( );
            break;
        }
        for( int i = 0; i < 8; ++i )
            if( sqrt( sq( 16 + positions[ i ][ 0 ] - t.px ) + sq( 16 + positions[ i ][ 1 ] - t.py ) ) <= 16 ) {
            while( 1 ) {
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            drawBagPage( i, pos, oamInd, palcnt, tilecnt, oamIndT, palcntT, tilecntT );

            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleSelect( &Bottom );
            consoleClear( );
            for( int j = 0; j <= 10; ++j )
                Oam->oamBuffer[ j ].isHidden = false;
            for( int j = 11; j <= 60; ++j )
                Oam->oamBuffer[ j ].isHidden = true;
            for( int j = 11; j < 19; ++j ) {
                Oam->oamBuffer[ j + 2 ].x = positions[ j - 11 ][ 0 ];
                Oam->oamBuffer[ j + 2 ].y = positions[ j - 11 ][ 1 ];
                Oam->oamBuffer[ j + 2 ].isHidden = false;
                updateOAMSub( Oam );

                int nx = positions[ j - 11 ][ 0 ] / 8 + 1, ny = positions[ j - 11 ][ 1 ] / 8 + 1, ind = 214 + 4 * ( j - 11 );
                consoleSetWindow( &Bottom, nx, ny, 2, 2 );
                for( int p = 0; p < 4; ++p )
                    printf( "%c", ind + p );
            }
            updateOAMSub( Oam );
            }
    }

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );
    consoleSetWindow( &Top, 0, 0, 32, 24 );
    consoleSelect( &Top );
    consoleClear( );
    initOAMTable( OamTop );
    initOAMTableSub( Oam );
    initMainSprites( Oam, spriteInfo );
    setMainSpriteVisibility( false );
    Oam->oamBuffer[ 8 ].isHidden = true;
    Oam->oamBuffer[ 0 ].isHidden = true;
    Oam->oamBuffer[ 1 ].isHidden = false;
}