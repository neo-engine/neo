/*
Pokémon Emerald 2 Version
------------------------------

file        : screenLoader.cpp
author      : Philip Wellnitz (RedArceus)
description :

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



#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "defines.h"

#ifdef USE_AS_LIB
#include "as_lib9.h"
#endif

#include "messageBox.h"
#include "screenLoader.h"
#include "saveGame.h"
#include "pokemon.h"
#include "sprite.h"
#include "bag.h"
#include "item.h"
#include "battle.h"
#include "fs.h"

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
#include "Up.h"
#include "Down.h"
#include "Choice_1.h"
#include "Choice_2.h"
#include "Choice_3.h"
#include "Message.h"

#include "Border.h"

#include "ChSq_a.h"
#include "ChSq_b.h"
#include "BigCirc1.h"

#include "ItemSpr1.h"
#include "ItemSpr2.h"
#include "ItemSpr3.h"

#include "memo.h"
#include "atks.h"
#include "Contest.h"

#include "NoItem.h"
#include "BagSpr.h"
#include "BagSpr2.h"

#include "BG1.h"
#include "BG2.h"
#include "BG3.h"

PrintConsole Top, Bottom;

unsigned int NAV_DATA[ 12288 ] = { 0 };
unsigned short NAV_DATA_PAL[ 256 ] = { 0 };
//Centers o t circles.
//pokemon -> ID -> DEX -> Bag -> Opt -> Nav
// X|Y
u8 mainSpritesPositions[ 12 ] = { 24, 64,
236, 96,
20, 128,
238, 64,
22, 96,
234, 128 };
backgroundSet BGs[ MAXBG ] = {/* { "Raging Gyarados", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Sleeping Eevee", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Mystic Guardevoir", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Waiting Suicune", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Awakening Xerneas", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Awakening Yveltal", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Fighting Groudon", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
    { "Fighting Kyogre", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },*/
    { "Fighting Torchic", BG3Bitmap, BG3Pal, false, false, mainSpritesPositions },
    { "Reborn Ho-Oh", BG2Bitmap, BG2Pal, false, false, mainSpritesPositions },
    { "Working Klink", BG1Bitmap, BG1Pal, false, true, mainSpritesPositions } };


Region acMapRegion = NONE;
std::pair<u8, u8> acMapPoint = std::pair<u8, u8>( 32, 24 );
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
    for( u16 i = 0; i < MAXMAPPOS; ++i ) {
        if( p_t.px > MapLocations[ acMapRegion - 1 ][ i ].m_lx && p_t.px < MapLocations[ acMapRegion - 1 ][ i ].m_rx &&
            p_t.py > MapLocations[ acMapRegion - 1 ][ i ].m_ly && p_t.py < MapLocations[ acMapRegion - 1 ][ i ].m_ry ) {
            printf( FS::getLoc( MapLocations[ acMapRegion - 1 ][ i ].m_ind ) );
            return;
        }
    }
}

void printMapLocation( const MapRegionPos& p_m ) {
    consoleSetWindow( &Bottom, 5, 0, 20, 1 );
    consoleSelect( &Bottom );
    consoleClear( );
    printf( FS::getLoc( p_m.m_ind ) );
    acMapPoint = std::pair<u8, u8>( ( p_m.m_lx + p_m.m_rx ) / 2, ( p_m.m_ly + p_m.m_ry ) / 2 );
    updateTime( );
    return;
}
u8 frame = 0;

#ifdef USE_AS_LIB
bool used = false;
bool usin = false;
#endif

void updateTime( s8 p_mapMode ) {
#ifdef USE_AS_LIB
    used = true;
#endif
    bool mm = false;
    if( -1 != p_mapMode )
        mm = p_mapMode;

#ifdef USE_AS_LIB
    if( !usin )
        return;
    //AS_SoundVBL();
#endif

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

    if( acseconds != timeStruct->tm_sec ) {
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
        } else {
            Oam->oamBuffer[ SQCH_ID ].isHidden = true;
            Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = true;
            updateOAMSub( Oam );
        }

        BG_PALETTE_SUB[ 249 ] = RGB15( 31, 31, 31 );
        cust_font2.setColor( 249, 1 );
        cust_font2.setColor( 249, 2 );

        char buffer[ 50 ];
        sprintf( buffer, "%02i:%02i:%02i", achours, acminutes, acseconds );
        cust_font2.printString( buffer, 18 * 8, 192 - 16, true );

        achours = timeStruct->tm_hour;
        acminutes = timeStruct->tm_min;
        acseconds = timeStruct->tm_sec;

        cust_font2.setColor( 0, 1 );
        cust_font2.setColor( 252, 2 );
        sprintf( buffer, "%02i:%02i:%02i", achours, acminutes, acseconds );
        cust_font2.printString( buffer, 18 * 8, 192 - 16, true );
    }
    achours = timeStruct->tm_hour;
    acminutes = timeStruct->tm_min;
    acday = timeStruct->tm_mday;
    acmonth = timeStruct->tm_mon;
    acyear = timeStruct->tm_year + 1900;
}

#ifdef USE_AS_LIB
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
#endif

u8 DayTimes[ 4 ][ 5 ] = {
    { 7, 10, 15, 17, 23 },
    { 6, 9, 12, 18, 23 },
    { 5, 8, 10, 20, 23 },
    { 7, 9, 13, 19, 23 }
};

u8 getCurrentDaytime( ) {
    time_t unixTime = time( NULL );
    struct tm* timeStruct = gmtime( (const time_t *)&unixTime );

    u8 t = timeStruct->tm_hour, m = timeStruct->tm_mon;

    for( u8 i = 0; i < 5; ++i )
        if( DayTimes[ m / 4 ][ i ] >= t )
            return i;
    return 254;
}

u32 BTMs[ 6 ][ 12288 ];
u16 PALs[ 6 ][ 256 ];

u16 cachedPKMN[ 6 ] = { 0 };

u8 positions[ 6 ][ 2 ] = {
    { 14, 2 }, { 16, 3 }, { 14, 9 },
    { 16, 10 }, { 14, 17 }, { 16, 18 }
};
u8 borders[ 6 ][ 2 ] = {
    { 4, 2 }, { 18, 3 },
    { 4, 9 }, { 18, 10 },
    { 4, 17 }, { 18, 18 }
};

u16 initMainSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo ) {
    /* Keep track of the available tiles */
    u16 nextAvailableTileIdx = 16;

    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, BACK_ID, BACK_ID, nextAvailableTileIdx,
                                       SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                       BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, SAVE_ID, SAVE_ID, nextAvailableTileIdx,
                                       -20, -20, 64, 64, SavePal,
                                       SaveTiles, SaveTilesLen, false, false, false, OBJPRIORITY_0, true );

    //Main menu sprites

    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, OPTS_ID, OPTS_ID, nextAvailableTileIdx,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) ] - 16,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) + 1 ] - 16,
                                       32, 32, OptionPal, OptionTiles, OptionTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, PKMN_ID, PKMN_ID, nextAvailableTileIdx,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) ] - 16,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) + 1 ] - 16,
                                       32, 32, PokemonSpPal, PokemonSpTiles, PokemonSpTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, NAV_ID, NAV_ID, nextAvailableTileIdx,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) ] - 16,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) + 1 ] - 16,
                                       32, 32, NavPal, NavTiles, NavTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, ID_ID, ID_ID, nextAvailableTileIdx,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) ] - 16,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) + 1 ] - 16,
                                       32, 32, IdPal, IdTiles, IdTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, DEX_ID, DEX_ID, nextAvailableTileIdx,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) ] - 16,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) + 1 ] - 16,
                                       32, 32, PokeDexPal, PokeDexTiles, PokeDexTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, BAG_ID, BAG_ID, nextAvailableTileIdx,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) ] - 16,
                                       BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) + 1 ] - 16,
                                       32, 32, SPBagPal, SPBagTiles, SPBagTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );

    //"A"-Button

    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, A_ID, A_ID, nextAvailableTileIdx,
                                       SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                                       ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );

    int palcnt = A_ID + 1;

    // Choice Box Sprites

    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, FWD_ID, palcnt++, nextAvailableTileIdx,
                                       SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, ForwardPal,
                                       ForwardTiles, ForwardTilesLen, false, false, true, OBJPRIORITY_1, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, BWD_ID, palcnt++, nextAvailableTileIdx,
                                       SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackwardPal,
                                       BackwardTiles, BackwardTilesLen, false, false, true, OBJPRIORITY_1, true );

    u16 nextnext, nn2;

    for( u8 i = 0; i < 6; ++i ) {
        nextnext = loadSprite( p_oam, p_spriteInfo, 2 * i + CHOICE_ID, palcnt, nextAvailableTileIdx,
                               ( ( i % 2 ) ? 32 : 128 ), 68 + ( i / 2 ) * 32, 64, 32, Choice_1Pal,
                               Choice_1Tiles, Choice_1TilesLen, false, false, true, OBJPRIORITY_2, true );

        nn2 = loadSprite( p_oam, p_spriteInfo, 2 * i + CHOICE_ID + 1, palcnt, nextnext,
                          ( ( i % 2 ) ? 62 : 160 ), 68 + ( i / 2 ) * 32, 64, 32, Choice_1Pal,
                          Choice_3Tiles, Choice_3TilesLen, false, false, true, OBJPRIORITY_2, true );
    }
    for( u8 i = 0; i < 3; ++i ) {
        nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, i + CHOICE_ID + 12, palcnt, nn2,
                                           96, 68 + (i)* 32, 64, 32, Choice_1Pal,
                                           Choice_2Tiles, Choice_2TilesLen, false, false, true, OBJPRIORITY_2, true );
    }

    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, SQCH_ID, ++palcnt, nextAvailableTileIdx,
                                       acMapPoint.first, acMapPoint.second, 16, 16, ChSq_aPal,
                                       ChSq_aTiles, ChSq_aTilesLen, false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, SQCH_ID + 1, palcnt, nextAvailableTileIdx,
                                       acMapPoint.first, acMapPoint.second, 16, 16, ChSq_bPal,
                                       ChSq_bTiles, ChSq_bTilesLen, false, false, true, OBJPRIORITY_0, true );


    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, 90, ++palcnt, nextAvailableTileIdx,
                                       acMapPoint.first, acMapPoint.second, 32, 32, memoPal,
                                       memoTiles, memoTilesLen, false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, 91, ++palcnt, nextAvailableTileIdx,
                                       acMapPoint.first, acMapPoint.second, 32, 32, atksPal,
                                       atksTiles, atksTilesLen, false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, 92, ++palcnt, nextAvailableTileIdx,
                                       acMapPoint.first, acMapPoint.second, 32, 32, ContestPal,
                                       ContestTiles, ContestTilesLen, false, false, true, OBJPRIORITY_0, true );

    return nextAvailableTileIdx;
}
void setMainSpriteVisibility( bool p_hidden, bool p_save ) {
    for( u8 i = 2; i <= 7; ++i )
        if( i == 2 && !SAV->m_hasPKMN )
            setSpriteVisibility( &Oam->oamBuffer[ i ], true );
        else
            setSpriteVisibility( &Oam->oamBuffer[ i ], p_hidden );

    if( p_save )
        setSpriteVisibility( &Oam->oamBuffer[ SAVE_ID ], p_hidden );
    updateOAMSub( Oam );
}

void drawBorder( ) {
    dmaCopy( BorderBitmap, bgGetGfxPtr( bg2sub ), 256 * 192 );
    dmaCopy( BorderPal, BG_PALETTE_SUB, 64 );
}

void drawSub( ) {
    //memset( bgGetGfxPtr( bg2sub ), 0, 256 * 192 );
    //memset( bgGetGfxPtr( bg3sub ), 0, 256 * 192 );
    if( !BGs[ SAV->m_bgIdx ].m_loadFromRom ) {
        dmaCopy( BGs[ SAV->m_bgIdx ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 192 );
        dmaCopy( BGs[ SAV->m_bgIdx ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
    } else if( !FS::loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ SAV->m_bgIdx ].m_name.c_str( ), SAV->m_bgIdx ) ) {
        dmaCopy( BGs[ 0 ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 256 );
        dmaCopy( BGs[ 0 ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
        SAV->m_bgIdx = 0;
    }
    drawBorder( );
}

void drawSub( u8 p_newIdx ) {
    if( SAV->m_bgIdx == p_newIdx )
        return;

    if( !BGs[ p_newIdx ].m_loadFromRom ) {
        dmaCopy( BGs[ p_newIdx ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 192 );
        dmaCopy( BGs[ p_newIdx ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
        SAV->m_bgIdx = p_newIdx;
    } else if( !FS::loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ p_newIdx ].m_name.c_str( ), p_newIdx ) ) {
        dmaCopy( BGs[ 0 ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 256 );
        dmaCopy( BGs[ 0 ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
        SAV->m_bgIdx = 0;
    } else
        SAV->m_bgIdx = p_newIdx;
    drawBorder( );
}

void screenLoader::draw( s8 p_mode ) {
    _pos = p_mode;
    SpriteEntry * back = &Oam->oamBuffer[ BACK_ID ];
    setSpriteVisibility( back, false );
    if( p_mode == 0 ) {
        drawSub( );
        setMainSpriteVisibility( true );

        for( u8 i = 0; i < 3; ++i ) {
            Oam->oamBuffer[ 90 + i ].isHidden = false;
            Oam->oamBuffer[ 90 + i ].x = BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 4 * i ] - 16;
            Oam->oamBuffer[ 90 + i ].y = BGs[ SAV->m_bgIdx ].m_mainMenuSpritePoses[ 4 * i + 1 ] - 16;
            Oam->oamBuffer[ 90 + i ].priority = OBJPRIORITY_1;
        }
        updateOAMSub( Oam );
    } else if( p_mode == 1 ) {
        showmappointer = true;
        FS::loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen2" );
        drawBorder( );
        updateTime( s8( 1 ) );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Hoenn" );
        acMapRegion = HOENN;
        setMainSpriteVisibility( true );

        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else if( p_mode == 2 ) {
        showmappointer = true;
        FS::loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen3" );
        drawBorder( );
        updateTime( s8( 1 ) );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Kanto" );
        acMapRegion = KANTO;
        setMainSpriteVisibility( true );
        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else if( p_mode == 3 ) {
        showmappointer = true;
        FS::loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen2_BG3_KJ" );
        drawBorder( );
        updateTime( s8( 1 ) );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Johto" );
        acMapRegion = JOHTO;
        setMainSpriteVisibility( true );
        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else {
        drawSub( );
        //updateTime( s8( 1 ) );
        setSpriteVisibility( back, true );
        setMainSpriteVisibility( false );
        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    }
    updateOAMSub( Oam );
    //updateTime( s8( 1 ) );
}
void screenLoader::init( ) {
    //initVideoSub();
    initOAMTableSub( Oam );
    initMainSprites( Oam, spriteInfo );
    this->draw( this->_pos );
}

void initTop( ) {
    consoleSelect( &Top );
    printf( "\x1b[39m" );
    u8 a = 0, b = 0;
    u16 c = 0;
    initOAMTable( OamTop );
    cust_font.setColor( 0, 0 );
    cust_font.setColor( 251, 1 );
    cust_font.setColor( 252, 2 );

    BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE[ 252 ] = RGB15( 15, 15, 15 );
    BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );

    for( size_t i = 0; i < SAV->m_PkmnTeam.size( ); i++ ) {
        if( !SAV->m_PkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg ) {
            if( i % 2 == 0 ) {
                FS::drawPKMNIcon( OamTop, spriteInfoTop, SAV->m_PkmnTeam[ i ].m_boxdata.m_speciesId, u16( borders[ i ][ 0 ] * 8 - 28 ), u16( borders[ i ][ 1 ] * 8 ), a, b, c, false );
                BATTLE::battleUI::displayHP( 100, 101, borders[ i ][ 0 ] * 8 - 13, borders[ i ][ 1 ] * 8 + 8 - ( i != 2 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
                BATTLE::battleUI::displayHP( 100, 100 - SAV->m_PkmnTeam[ i ].m_stats.m_acHP * 100 / SAV->m_PkmnTeam[ i ].m_stats.m_maxHP, borders[ i ][ 0 ] * 8 - 13, borders[ i ][ 1 ] * 8 + 8 - ( i != 2 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
            } else {
                FS::drawPKMNIcon( OamTop, spriteInfoTop, SAV->m_PkmnTeam[ i ].m_boxdata.m_speciesId, u16( borders[ i ][ 0 ] * 8 + 76 ), u16( borders[ i ][ 1 ] * 8 ), a, b, c, false );
                BATTLE::battleUI::displayHP( 100, 101, borders[ i ][ 0 ] * 8 + 63, borders[ i ][ 1 ] * 8 + 8 - ( i != 3 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
                BATTLE::battleUI::displayHP( 100, 100 - SAV->m_PkmnTeam[ i ].m_stats.m_acHP * 100 / SAV->m_PkmnTeam[ i ].m_stats.m_maxHP, borders[ i ][ 0 ] * 8 + 63, borders[ i ][ 1 ] * 8 + 8 - ( i != 3 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
            }
            updateOAM( OamTop );
            //char buffer[ 100 ];

            u8 mval = 1 + ( ( i / 2 == 1 ) ? 4 : 8 );

            sprintf( buffer, "%ls", SAV->m_PkmnTeam[ i ].m_boxdata.m_name );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 - mval, false );
            sprintf( buffer, "%s", POKEMON::PKMNDATA::getDisplayName( SAV->m_PkmnTeam[ i ].m_boxdata.m_speciesId ) );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 14 - mval, false );

            sprintf( buffer, "%hi/%hi KP", SAV->m_PkmnTeam[ i ].m_stats.m_acHP, SAV->m_PkmnTeam[ i ].m_stats.m_maxHP );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 28 - mval, false );

            sprintf( buffer, "%s", ITEMS::ItemList[ SAV->m_PkmnTeam[ i ].m_boxdata.getItem( ) ]->getDisplayName( ).c_str( ) );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 42 - mval, false );

        } else {
            consoleSetWindow( &Top, borders[ i ][ 0 ], borders[ i ][ 1 ], 12, 6 );

            char buffer[ 100 ];
            int mval = 1 + ( ( i / 2 == 1 ) ? 4 : 8 );
            sprintf( buffer, "Ei" );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 - mval, false );
            sprintf( buffer, "Ei" );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 14 - mval, false );

            sprintf( buffer, "%s", ITEMS::ItemList[ SAV->m_PkmnTeam[ i ].m_boxdata.getItem( ) ]->getDisplayName( ).c_str( ) );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 42 - mval, false );

            if( i % 2 == 0 ) {
                FS::drawEggIcon( OamTop, spriteInfoTop, u16( borders[ i ][ 0 ] * 8 - 28 ), u16( borders[ i ][ 1 ] * 8 ), a, b, c, false );
            } else {
                FS::drawEggIcon( OamTop, spriteInfoTop, u16( borders[ i ][ 0 ] * 8 + 76 ), u16( borders[ i ][ 1 ] * 8 ), a, b, c, false );
            }
            updateOAM( OamTop );
        }
    }
    updateOAM( OamTop );
}
void clearTop( ) {
    consoleSelect( &Top );
    for( size_t i = 0; i < SAV->m_PkmnTeam.size( ); i++ ) {
        consoleSetWindow( &Top, borders[ i ][ 0 ], borders[ i ][ 1 ], 12, 6 );
        consoleClear( );
    }
    initOAMTable( OamTop );
}

u8 fieldCnt = 0;

void initSub( u16 pkmIdx ) {
    consoleSelect( &Bottom );
    consoleSetWindow( &Bottom, 16, 0, 16, 16 );
    consoleClear( );
    for( u8 i = 0; i < 6; ++i ) {
        Oam->oamBuffer[ 15 + 2 * i ].isHidden = true;
        Oam->oamBuffer[ 16 + 2 * i ].isHidden = true;
    }
    updateOAMSub( Oam );
    if( pkmIdx >= SAV->m_PkmnTeam.size( ) || SAV->m_PkmnTeam[ pkmIdx ].m_boxdata.m_individualValues.m_isEgg ) {
        consoleSelect( &Top );
        return;
    }
    u8 u = 0;
    for( u8 i = 0; i < 4; ++i ) {
        if( SAV->m_PkmnTeam[ pkmIdx ].m_boxdata.m_moves[ i ] < MAXATTACK &&
            AttackList[ SAV->m_PkmnTeam[ pkmIdx ].m_boxdata.m_moves[ i ] ]->m_isFieldAttack ) {
            Oam->oamBuffer[ 15 + 2 * ( u ) ].isHidden = false;
            Oam->oamBuffer[ 16 + 2 * ( u ) ].isHidden = false;
            Oam->oamBuffer[ 15 + 2 * ( u ) ].y = -7 + 24 * u;
            Oam->oamBuffer[ 16 + 2 * ( u ) ].y = -7 + 24 * u;

            Oam->oamBuffer[ 15 + 2 * ( u ) ].x = 152;
            Oam->oamBuffer[ 16 + 2 * ( u ) ].x = 192 + 24;
            updateOAMSub( Oam );

            consoleSetWindow( &Bottom, 16, 3 * u + 1, 16, 16 );
            printf( "    %s", AttackList[ SAV->m_PkmnTeam[ pkmIdx ].m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ) );
            ++u;
        }
    }
    if( SAV->m_PkmnTeam[ pkmIdx ].m_boxdata.m_holdItem ) {
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
    u16 s = u + SAV->m_PkmnTeam[ pkmIdx ].m_boxdata.m_holdItem;
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

void loadPKMNSprites( ) {
    int palcnt = A_ID + 1;
    u16 nextAvailableTileIdx = 208;

    // Choice Box Sprites

    nextAvailableTileIdx = loadSprite( Oam, spriteInfo, FWD_ID, palcnt++, nextAvailableTileIdx,
                                       SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, DownPal,
                                       DownTiles, DownTilesLen, false, false, true, OBJPRIORITY_1, true );
    nextAvailableTileIdx = loadSprite( Oam, spriteInfo, BWD_ID, palcnt++, nextAvailableTileIdx,
                                       SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, UpPal,
                                       UpTiles, UpTilesLen, false, false, true, OBJPRIORITY_1, true );
}

void screenLoader::run_pkmn( ) {
    vramSetup( );
    videoSetMode( MODE_5_2D/* | DISPLAY_BG2_ACTIVE*/ | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    swiWaitForVBlank( );
    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition touch;
    swiWaitForVBlank( );
    drawSub( );
    u8 acIn = 0, max = SAV->m_PkmnTeam.size( );
    consoleSelect( &Top );
    consoleClear( );

    clearTop( );

    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
    bgUpdate( );
    initTop( );
    initSub( acIn );
    loadPKMNSprites( );
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
        u32 pressed = keysCurrent( );
        u32 held = keysHeld( );
        touchRead( &touch );

        if( held & KEY_X || ( touch.px > 224 && touch.py > 164 ) ) {
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
        } else if( ( pressed & KEY_A ) || ( sqrt( sq( touch.px - 128 ) + sq( touch.py - 96 ) ) <= 16 ) ) {
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
                u32 p = SAV->m_PkmnTeam[ acIn ].draw( );
                if( p & KEY_X ) {
                    for( u8 i = 0; i < max; i++ ) {
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
                    loadPKMNSprites( );
                    Oam->oamBuffer[ 8 ].isHidden = false;
                    Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
                    Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;

                    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
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
                    if( acIn == 0 )
                        acIn = max - 1;
                    else
                        --acIn;
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
            if( acIn == 0 )
                acIn = max - 1;
            else
                --acIn;
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
        } else if( SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem && touch.px >= 152 && touch.py >= ( -7 + 24 * fieldCnt ) && touch.py < ( 17 + 24 * fieldCnt ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            char buffer[ 50 ];
            ITEMS::item acI = *ITEMS::ItemList[ SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem ];
            SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem = 0;

            initSub( -1 );
            setSpriteVisibility( back, true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH - 28;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT - 28;
            updateOAMSub( Oam );
            sprintf( buffer, "%s von %ls\nim Beutel verstaut.", acI.getDisplayName( ).c_str( ), SAV->m_PkmnTeam[ acIn ].m_boxdata.m_name );
            messageBox( buffer, true, true );
            SAV->m_bag.addItem( acI.getItemType( ), acI.getItemId( ), 1 );

            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
            FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
            initTop( );
            setSpriteVisibility( back, false );
            Oam->oamBuffer[ 8 ].isHidden = false;
            updateOAMSub( Oam );
            initSub( acIn );
        } else if( touch.px >= 152 && touch.py >= ( -7 + 24 * ( SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem + fieldCnt ) ) && touch.py < ( 17 + 24 * ( fieldCnt + SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem ) ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            ITEMS::item acI = *ITEMS::ItemList[ SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem ];
            SAV->m_PkmnTeam[ acIn ].m_boxdata.m_holdItem = 0;

            initSub( -1 );
            setSpriteVisibility( back, true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH - 28;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT - 28;
            updateOAMSub( Oam );

            this->run_dex( SAV->m_PkmnTeam[ acIn ].m_boxdata.m_speciesId - 1 );

            setSpriteVisibility( back, false );
            setSpriteVisibility( save, true );
            setMainSpriteVisibility( true );
            Oam->oamBuffer[ 8 ].isHidden = false;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
            FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
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
        for( u8 i = 0; i < fieldCnt; ++i )
            if( touch.px >= 152 && touch.py >= ( -7 + 24 * i ) && touch.py < ( 17 + 24 * i ) ) {
                while( 1 ) {
                    scanKeys( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                u8 u = 0, o;
                for( o = 0; o < 4 && u <= i; ++o )
                    if( AttackList[ SAV->m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->m_isFieldAttack )
                        u++;
                o--;
                if( AttackList[ SAV->m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->possible( ) ) {
                    for( u8 i2 = 0; i2 < max; i2++ ) {
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
                    movePlayerOnMap( SAV->m_acposx / 20, SAV->m_acposy / 20, SAV->m_acposz, true );

                    bgUpdate( );
                    swiWaitForVBlank( );

                    char buffer[ 50 ];
                    sprintf( buffer, "%ls setzt %s\nein!", SAV->m_PkmnTeam[ acIn ].m_boxdata.m_name, AttackList[ SAV->m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->m_moveName.c_str( ) );
                    messageBox( buffer, true, true );
                    shoUseAttack( SAV->m_PkmnTeam[ acIn ].m_boxdata.m_speciesId, SAV->m_PkmnTeam[ acIn ].m_boxdata.m_isFemale, SAV->m_PkmnTeam[ acIn ].m_boxdata.isShiny( ) );
                    AttackList[ SAV->m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->use( );
                    return;
                } else {
                    initSub( -1 );

                    initOAMTableSub( Oam );
                    initMainSprites( Oam, spriteInfo );
                    setSpriteVisibility( back, true );
                    setSpriteVisibility( save, false );
                    setMainSpriteVisibility( true );
                    loadPKMNSprites( );
                    Oam->oamBuffer[ 8 ].isHidden = true;
                    //loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","PKMNScreen");
                    messageBox( "Diese Attacke kann jetzt\nnicht eingesetzt werden.", "PokéNav" );

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
    for( u8 i = 0; i < max; i++ ) {
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

const u8 dexsppos[ 2 ][ 9 ] = { { 160, 128, 96, 19, 6, 120, 158, 196, 8 }, { -16, 0, 24, 138, 173, 108, 126, 144, 32 } };
void initDexSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16& p_tileCnt ) {

    memset( SPRITE_GFX_SUB, 0, 32 );

    p_tileCnt = 16;
    p_oamIndex = 0;
    p_palCnt = 0;

    p_tileCnt = loadSprite( p_oam, p_spriteInfo, p_oamIndex++, p_palCnt++, p_tileCnt,
                            SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                            BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, false );
    ++p_palCnt;
    p_oamIndex = 10;
    for( u8 i = 0; i < 8; ++i ) {
        SpriteInfo * BagSprInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * BagSpr = &p_oam->oamBuffer[ p_oamIndex ];
        BagSprInfo->m_oamId = 11;
        BagSprInfo->m_width = 32;
        BagSprInfo->m_height = 32;
        BagSprInfo->m_angle = 0;
        BagSprInfo->m_entry = BagSpr;
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
    B2Info->m_oamId = 12;
    B2Info->m_width = 64;
    B2Info->m_height = 64;
    B2Info->m_angle = 0;
    B2Info->m_entry = B2;
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

void drawTypeIcon( OAMTable *p_oam, SpriteInfo * p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16 & p_tileCnt, Type p_type, u16 p_posX, u16 p_posY, bool p_subScreen ) {
    SpriteInfo * type1Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * type1 = &p_oam->oamBuffer[ p_oamIndex ];
    type1Info->m_oamId = p_oamIndex;
    type1Info->m_width = 32;
    type1Info->m_height = 16;
    type1Info->m_angle = 0;
    type1Info->m_entry = type1;
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

void formes( OAMTable *p_oam, SpriteInfo * p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16 & p_tileCnt, u16 p_pkmnId, bool p_female, POKEMON::pkmnGenderType p_pkmnGenderType ) {
    SpriteInfo * B2Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * B2 = &p_oam->oamBuffer[ p_oamIndex ];
    B2Info->m_oamId = 12;
    B2Info->m_width = 64;
    B2Info->m_height = 64;
    B2Info->m_angle = 0;
    B2Info->m_entry = B2;
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
        FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80, 64, p_oamIndex, p_palCnt, p_tileCnt, false, false, p_female );
        return;
    } else {
        for( u8 i = p_oamIndex; i > p_oamIndex - 8; --i )
            if( i % 2 )
                p_oam->oamBuffer[ i ].x -= 50;
            else
                p_oam->oamBuffer[ i ].x += 50;
        updateOAM( p_oam );
        if( !FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 - 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, false, p_female, true ) )
            FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 - 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, false, !p_female, true );
        --p_palCnt;
        if( !FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 + 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, true, p_female ) )
            FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 80 + 50, 64, p_oamIndex, p_palCnt, p_tileCnt, false, true, !p_female );
    }
    SpriteInfo * C1Info = &p_spriteInfo[ ++p_oamIndex ];
    SpriteEntry * C1 = &p_oam->oamBuffer[ p_oamIndex ];
    C1Info->m_oamId = p_oamIndex;
    C1Info->m_width = 64;
    C1Info->m_height = 32;
    C1Info->m_angle = 0;
    C1Info->m_entry = C1;
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
    C3Info->m_oamId = p_oamIndex;
    C3Info->m_width = 64;
    C3Info->m_height = 32;
    C3Info->m_angle = 0;
    C3Info->m_entry = C3;
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
        for( u8 i = p_oamIndex; i > p_oamIndex - 4; --i )
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

void drawTopDexPage( u8 p_page, u16 p_pkmnId, u8 p_pkmnFormeId = 0 ) {
    initOAMTable( OamTop );
    consoleSetWindow( &Top, 0, 0, 32, 24 );
    consoleSelect( &Top );
    printf( "\x1b[37m" );
    consoleClear( );
    u8 a = 0, b = 0;
    u16 c = 0;
    POKEMON::PKMNDATA::pokemonData acpkmndata;
    POKEMON::pkmnGenderType acG = POKEMON::GENDERLESS;
    u16 newformepkmn = p_pkmnId;
    if( p_page < 4 ) {
        POKEMON::PKMNDATA::getAll( p_pkmnId, acpkmndata );
        FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "DexTop" );
        if( p_page == 2 )
            memset( bgGetGfxPtr( bg3 ) + 7168, 1, 256 * 192 - 14336 );

        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ 252 ] = RGB15( 31, 31, 31 );

        if( SAV->m_inDex[ p_pkmnId - 1 ] ) {
            BG_PALETTE[ 0 ] = POKEMON::PKMNDATA::getColor( acpkmndata.m_types[ 0 ] );

            bool isFixed = ( acpkmndata.m_gender == POKEMON::GENDERLESS ) || ( acpkmndata.m_gender == POKEMON::MALE ) || ( acpkmndata.m_gender == POKEMON::FEMALE );
            p_pkmnFormeId %= acpkmndata.m_formecnt ? ( ( isFixed ? 1 : 2 )*acpkmndata.m_formecnt ) : 2;
            newformepkmn = acpkmndata.m_formecnt ? POKEMON::PKMNDATA::getForme( p_pkmnId, p_pkmnFormeId / ( isFixed ? 1 : 2 ), 0 ) : p_pkmnId;
            acG = acpkmndata.m_gender;
            if( acpkmndata.m_formecnt )
                POKEMON::PKMNDATA::getAll( newformepkmn, acpkmndata );

            BG_PALETTE[ 1 ] = POKEMON::PKMNDATA::getColor( acpkmndata.m_types[ 0 ] );
            FS::drawPKMNIcon( OamTop, spriteInfoTop, ( p_pkmnId == 493 || p_pkmnId == 649 ) ? p_pkmnId : newformepkmn, 0, 8, a, b, c, false );
            drawTypeIcon( OamTop, spriteInfoTop, a, b, c, acpkmndata.m_types[ 0 ], 33, 35, false );
            if( acpkmndata.m_types[ 0 ] != acpkmndata.m_types[ 1 ] )
                drawTypeIcon( OamTop, spriteInfoTop, a, b, c, acpkmndata.m_types[ 1 ], 65, 35, false );
            updateOAM( OamTop );
            printf( "\n    Du hast %i dieser Pok\x82""mon.\n\n", box_of_st_pkmn[ p_pkmnId - 1 ].size( ) );
            char buffer[ 50 ];
            sprintf( buffer, "%s", POKEMON::PKMNDATA::getDisplayName( p_pkmnId ) );
            sprintf( buffer, "%s - %s", buffer, POKEMON::PKMNDATA::getSpecies( p_pkmnId ) );
            cust_font.printString( buffer, 36, 20, false );
            printf( "\n\n %03i", p_pkmnId );
        } else {
            printf( "\n    Keine Daten vorhanden.\n\n" );
            char buffer[ 50 ];
            sprintf( buffer, "???????????? - %s", POKEMON::PKMNDATA::getSpecies( 0 ) );
            cust_font.printString( buffer, 36, 20, false );
            printf( "\n\n %03i", p_pkmnId );
        }
    }
    switch( p_page ) {
        case 0:{
            printf( "\x1b[37m" );
            if( SAV->m_inDex[ p_pkmnId - 1 ] ) {
                BG_PALETTE[ 42 ] = POKEMON::PKMNDATA::getColor( acpkmndata.m_types[ 1 ] );
                for( u8 i = 0; i < 6; ++i ) {
                    FONT::putrec( u8( 19 + 40 * i ), u8( std::max( 56, 102 - acpkmndata.m_bases[ i ] / 3 ) ), u8( 37 + 40 * i ), (u8)102, false, true );
                    //font::putrec(17 + 40 * i,std::min(103, 56 + acpkmndata.Bases[i] / 3),(6* (acpkmndata.Bases[i] % 3)) + 16 + 40 * i,std::min(103, 58 + acpkmndata.Bases[i] / 3),statColor[i],false);
                }
                printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                sprintf( buffer, "GW.  %5.1fkg", acpkmndata.m_weight / 10.0 );
                cust_font.printString( buffer, 10, 109, false );
                sprintf( buffer, "GR.  %6.1fm", acpkmndata.m_size / 10.0 );
                cust_font.printString( buffer, 100, 109, false );
                consoleSetWindow( &Top, 1, 16, 30, 24 );
                printf( POKEMON::PKMNDATA::getDexEntry( p_pkmnId ) );
            } else {
                printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                sprintf( buffer, "GW.  ???.?kg" );
                cust_font.printString( buffer, 10, 109, false );
                sprintf( buffer, "GR.  ???.?m" );
                cust_font.printString( buffer, 100, 109, false );
                consoleSetWindow( &Top, 1, 16, 30, 24 );
                printf( POKEMON::PKMNDATA::getDexEntry( 0 ) );
            }
            break;
        }
        case 2:{
            if( SAV->m_inDex[ p_pkmnId - 1 ] ) {
                formes( OamTop, spriteInfoTop, a, b, c, newformepkmn, p_pkmnFormeId % 2, acG );
            } else
                formes( OamTop, spriteInfoTop, a, b, c, 0, 0, POKEMON::GENDERLESS );
            break;
        }
        case 4:{
            FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BottomScreen2" );
            break;
        }
        case 5:{
            FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BottomScreen3" );
            break;
        }
        case 6:{
            FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BottomScreen2_BG3_KJ" );
            break;
        }
    }
    swiWaitForVBlank( );
}

void screenLoader::run_dex( u16 p_pkmnId ) {
    vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition t;
    u8 acForme = 0;
    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "DexTop", 32 );

    drawSub( );
    initOAMTableSub( Oam );
    initOAMTable( OamTop );
    u8 palcnt = 0;
    u16 tilecnt = 0;
    u8 oamInd = 0;
    initDexSprites( Oam, spriteInfo, oamInd, palcnt, tilecnt );

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );
    printf( "Gefangen: %3i", SAV->m_dex );

    u16 acNum = p_pkmnId, maxn = SAV->m_hasGDex ? 649 : 493, t2 = tilecnt;
    u8 o2 = oamInd, p2 = palcnt, acPage = 0, acMap = acMapRegion;

    consoleSetWindow( &Top, 0, 0, 32, 24 );
    consoleSelect( &Top );
    consoleClear( );
    for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
        if( i == acNum ) {
            drawTopDexPage( acPage, i + 1, acForme );
            FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true );
            --j;
            continue;
        } else
            FS::drawPKMNIcon( Oam, spriteInfo, SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );

        spriteInfo[ 16 ].m_entry->isHidden = true;
        updateOAMSub( Oam );
        while( 42 ) {
            updateTime( );
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            touchRead( &t );
            scanKeys( );
            u32 pressed = keysDown( ), up = keysUp( ), held = keysHeld( );
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
                for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                        drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                        FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_UP ) || ( ( held & ~up ) & KEY_UP ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_UP )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + maxn - 1 ) % maxn;
                for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                        drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                        FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_R ) || ( ( held & ~up ) & KEY_R ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_R )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + 15 ) % maxn;
                for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                        drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                        FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( pressed & KEY_L ) || ( ( held & ~up ) & KEY_L ) ) {
                scanKeys( );
                if( keysUp( ) & KEY_L )
                    continue;
                swiWaitForVBlank( );
                o2 = oamInd; p2 = palcnt; t2 = tilecnt;
                acNum = ( acNum + maxn - 15 ) % maxn;
                for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                    if( i == acNum ) {
                        drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                        FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                    continue;
            } else if( ( held & KEY_LEFT ) ) {
                while( 1 ) {
                    if( keysUp( ) & KEY_LEFT )
                        break;
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                }
                if( acPage == 1 )
                    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
                spriteInfo[ 16 + acPage ].m_entry->isHidden = false;
                acPage = ( acPage + 2 ) % 3;
                spriteInfo[ 16 + acPage ].m_entry->isHidden = true;
                updateOAMSub( Oam );
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
            } else if( ( held & KEY_RIGHT ) ) {
                while( 1 ) {
                    if( keysUp( ) & KEY_RIGHT )
                        break;
                    scanKeys( );
                    swiWaitForVBlank( );
                    updateTime( );
                }
                if( acPage == 1 )
                    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
                spriteInfo[ 16 + acPage ].m_entry->isHidden = false;
                acPage = ( acPage + 1 ) % 3;
                spriteInfo[ 16 + acPage ].m_entry->isHidden = true;
                updateOAMSub( Oam );
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
            } else if( ( held & KEY_SELECT ) ) {
                if( acPage > 1 ) {
                    while( 1 ) {
                        if( !( keysHeld( ) & KEY_SELECT ) )
                            break;
                        scanKeys( );
                        swiWaitForVBlank( );
                        updateTime( );
                    }
                }
                if( acPage == 1 )
                    acMap = ( acMap + 1 ) % 3;
                else
                    acForme = ( acForme + 1 ) % 60;
                drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, acNum + 1, acForme );
            }
            for( u8 q = 0; q < 5; ++q )
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
                    for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
                        if( i == acNum ) {
                            drawTopDexPage( acPage == 1 ? 4 + acMap : acPage, i + 1, acForme ); --j;
                            FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                        } else
                            FS::drawPKMNIcon( Oam, spriteInfo, SAV->m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
                }
            for( u8 q = 5; q < 8; ++q )
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
                        FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
                    spriteInfo[ 16 + acPage ].m_entry->isHidden = false;
                    acPage = ( q + 1 ) % 3;
                    spriteInfo[ 16 + acPage ].m_entry->isHidden = true;
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