/*
    Pokémon Emerald 2 Version
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

PrintConsole Top, Bottom;

unsigned int NAV_DATA[ 12288 ] = { 0 };
unsigned short NAV_DATA_PAL[ 256 ] = { 0 };
//Centers o t circles.
//pokemon -> ID -> DEX -> Bag -> Opt -> Nav
// X|Y
u8 mainSpritesPositions[ 12 ] = { 130, 60,
160, 80,
160, 115,
130, 135,
100, 115,
100, 80 };
u8 mainSpritesPositions2[ 12 ] = { 24, 64,
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
{ "Reborn_Ho-Oh", BG2Bitmap, BG2Pal, false, false, mainSpritesPositions2 } };
u8 BG_ind = 9;

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

bool used = false;
bool usin = false;

void updateTime( s8 p_mapMode ) {
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

int ac = 0; //Whats this??
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
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) ] - 16,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) + 1 ] - 16,
                                       32, 32, OptionPal, OptionTiles, OptionTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, PKMN_ID, PKMN_ID, nextAvailableTileIdx,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) ] - 16,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) + 1 ] - 16,
                                       32, 32, PokemonSpPal, PokemonSpTiles, PokemonSpTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, NAV_ID, NAV_ID, nextAvailableTileIdx,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) ] - 16,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) + 1 ] - 16,
                                       32, 32, NavPal, NavTiles, NavTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, ID_ID, ID_ID, nextAvailableTileIdx,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) ] - 16,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) + 1 ] - 16,
                                       32, 32, IdPal, IdTiles, IdTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, DEX_ID, DEX_ID, nextAvailableTileIdx,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) ] - 16,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) + 1 ] - 16,
                                       32, 32, PokeDexPal, PokeDexTiles, PokeDexTilesLen,
                                       false, false, true, OBJPRIORITY_0, true );
    nextAvailableTileIdx = loadSprite( p_oam, p_spriteInfo, BAG_ID, BAG_ID, nextAvailableTileIdx,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) ] - 16,
                                       BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) + 1 ] - 16,
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
        if( i == 2 && !SAV.m_hasPKMN )
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
    if( !BGs[ BG_ind ].m_loadFromRom ) {
        dmaCopy( BGs[ BG_ind ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 192 );
        dmaCopy( BGs[ BG_ind ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
    } else if( !FS::loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind ) ) {
        dmaCopy( BGs[ 0 ].m_mainMenu, bgGetGfxPtr( bg3sub ), 256 * 256 );
        dmaCopy( BGs[ 0 ].m_mainMenuPal, BG_PALETTE_SUB, 256 * 2 );
        BG_ind = 0;
    }
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
            Oam->oamBuffer[ 90 + i ].x = BGs[ BG_ind ].m_mainMenuSpritePoses[ 4 * i ] - 16;
            Oam->oamBuffer[ 90 + i ].y = BGs[ BG_ind ].m_mainMenuSpritePoses[ 4 * i + 1 ] - 16;
            Oam->oamBuffer[ 90 + i ].priority = OBJPRIORITY_1;
        }
        updateOAMSub( Oam );
    } else if( p_mode == 1 ) {
        FS::loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen2" );
        drawBorder( );
        updateTime( true );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Hoenn" );
        acMapRegion = HOENN;
        setMainSpriteVisibility( true );

        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else if( p_mode == 2 ) {
        FS::loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen3" );
        drawBorder( );
        updateTime( true );
        consoleSelect( &Bottom );
        consoleSetWindow( &Bottom, 4, 1, 12, 1 );
        printf( " Kanto" );
        acMapRegion = KANTO;
        setMainSpriteVisibility( true );
        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
    } else if( p_mode == 3 ) {
        FS::loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "BottomScreen2_BG3_KJ" );
        drawBorder( );
        updateTime( true );
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
        updateTime( true );
        setSpriteVisibility( back, true );
        setMainSpriteVisibility( false );
        for( u8 i = 0; i < 3; ++i )
            Oam->oamBuffer[ 90 + i ].isHidden = true;
        updateOAMSub( Oam );
        /*initOAMTableSub(Oam);
        initMainSprites(Oam,spriteInfo);*/
    }
    updateOAMSub( Oam );
    updateTime( true );
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

    for( size_t i = 0; i < SAV.m_PkmnTeam.size( ); i++ ) {
        if( !SAV.m_PkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg ) {
            if( i % 2 == 0 ) {
                FS::drawPKMNIcon( OamTop, spriteInfoTop, SAV.m_PkmnTeam[ i ].m_boxdata.m_speciesId, u16( borders[ i ][ 0 ] * 8 - 28 ), u16( borders[ i ][ 1 ] * 8 ), a, b, c, false );
                BATTLE::battleUI::displayHP( 100, 101, borders[ i ][ 0 ] * 8 - 13, borders[ i ][ 1 ] * 8 + 8 - ( i != 2 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
                BATTLE::battleUI::displayHP( 100, 100 - SAV.m_PkmnTeam[ i ].m_stats.m_acHP * 100 / SAV.m_PkmnTeam[ i ].m_stats.m_maxHP, borders[ i ][ 0 ] * 8 - 13, borders[ i ][ 1 ] * 8 + 8 - ( i != 2 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
            } else {
                FS::drawPKMNIcon( OamTop, spriteInfoTop, SAV.m_PkmnTeam[ i ].m_boxdata.m_speciesId, u16( borders[ i ][ 0 ] * 8 + 76 ), u16( borders[ i ][ 1 ] * 8 ), a, b, c, false );
                BATTLE::battleUI::displayHP( 100, 101, borders[ i ][ 0 ] * 8 + 63, borders[ i ][ 1 ] * 8 + 8 - ( i != 3 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
                BATTLE::battleUI::displayHP( 100, 100 - SAV.m_PkmnTeam[ i ].m_stats.m_acHP * 100 / SAV.m_PkmnTeam[ i ].m_stats.m_maxHP, borders[ i ][ 0 ] * 8 + 63, borders[ i ][ 1 ] * 8 + 8 - ( i != 3 ? 4 : 0 ), 142 + 2 * i, 143 + 2 * i, false, true );
            }
            updateOAM( OamTop );
            //char buffer[ 100 ];

            u8 mval = 1 + ( ( i / 2 == 1 ) ? 4 : 8 );

            sprintf( buffer, "%ls", SAV.m_PkmnTeam[ i ].m_boxdata.m_name );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 - mval, false );
            sprintf( buffer, "%s", POKEMON::PKMNDATA::getDisplayName( SAV.m_PkmnTeam[ i ].m_boxdata.m_speciesId ) );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 14 - mval, false );

            sprintf( buffer, "%hi/%hi KP", SAV.m_PkmnTeam[ i ].m_stats.m_acHP, SAV.m_PkmnTeam[ i ].m_stats.m_maxHP );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 28 - mval, false );

            sprintf( buffer, "%s", ITEMS::ItemList[ SAV.m_PkmnTeam[ i ].m_boxdata.getItem( ) ].getDisplayName( ).c_str( ) );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 42 - mval, false );

        } else {
            consoleSetWindow( &Top, borders[ i ][ 0 ], borders[ i ][ 1 ], 12, 6 );

            char buffer[ 100 ];
            int mval = 1 + ( ( i / 2 == 1 ) ? 4 : 8 );
            sprintf( buffer, "Ei" );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 - mval, false );
            sprintf( buffer, "Ei" );
            cust_font.printString( buffer, borders[ i ][ 0 ] * 8, borders[ i ][ 1 ] * 8 + 14 - mval, false );

            sprintf( buffer, "%s", ITEMS::ItemList[ SAV.m_PkmnTeam[ i ].m_boxdata.getItem( ) ].getDisplayName( ).c_str( ) );
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
    for( size_t i = 0; i < SAV.m_PkmnTeam.size( ); i++ ) {
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
    if( pkmIdx >= SAV.m_PkmnTeam.size( ) || SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_individualValues.m_isEgg ) {
        consoleSelect( &Top );
        return;
    }
    u8 u = 0;
    for( u8 i = 0; i < 4; ++i ) {
        if( SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_moves[ i ] < MAXATTACK &&
            AttackList[ SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_moves[ i ] ]->m_isFieldAttack ) {
            Oam->oamBuffer[ 15 + 2 * ( u ) ].isHidden = false;
            Oam->oamBuffer[ 16 + 2 * ( u ) ].isHidden = false;
            Oam->oamBuffer[ 15 + 2 * ( u ) ].y = -7 + 24 * u;
            Oam->oamBuffer[ 16 + 2 * ( u ) ].y = -7 + 24 * u;

            Oam->oamBuffer[ 15 + 2 * ( u ) ].x = 152;
            Oam->oamBuffer[ 16 + 2 * ( u ) ].x = 192 + 24;
            updateOAMSub( Oam );

            consoleSetWindow( &Bottom, 16, 3 * u + 1, 16, 16 );
            printf( "    %s", AttackList[ SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ) );
            ++u;
        }
    }
    if( SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_holdItem ) {
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
    u16 s = u + SAV.m_PkmnTeam[ pkmIdx ].m_boxdata.m_holdItem;
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
    videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    swiWaitForVBlank( );
    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition touch;
    swiWaitForVBlank( );
    drawSub( );
    u8 acIn = 0, max = SAV.m_PkmnTeam.size( );
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
                s8 p = SAV.m_PkmnTeam[ acIn ].draw( );
                /*                if( p & KEY_X ) {
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
                                    } else*/
                if( p & KEY_B ) {
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
        } else if( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem && touch.px >= 152 && touch.py >= ( -7 + 24 * fieldCnt ) && touch.py < ( 17 + 24 * fieldCnt ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            char buffer[ 50 ];
            ITEMS::item acI = ITEMS::ItemList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem ];
            SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem = 0;

            initSub( -1 );
            setSpriteVisibility( back, true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH - 28;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT - 28;
            updateOAMSub( Oam );
            sprintf( buffer, "%s von %ls\nim Beutel verstaut.", acI.getDisplayName( ).c_str( ), SAV.m_PkmnTeam[ acIn ].m_boxdata.m_name );
            messageBox( buffer, true, true );
            SAV.m_bag.addItem( acI.getItemType( ), acI.getItemId( ), 1 );

            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH / 2 - 16;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT / 2 - 16;
            FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNScreen" );
            initTop( );
            setSpriteVisibility( back, false );
            Oam->oamBuffer[ 8 ].isHidden = false;
            updateOAMSub( Oam );
            initSub( acIn );
        } else if( touch.px >= 152 && touch.py >= ( -7 + 24 * ( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem + fieldCnt ) ) && touch.py < ( 17 + 24 * ( fieldCnt + SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem ) ) ) {
            while( 1 ) {
                scanKeys( );
                auto t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
                swiWaitForVBlank( );
            }
            ITEMS::item acI = ITEMS::ItemList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem ];
            SAV.m_PkmnTeam[ acIn ].m_boxdata.m_holdItem = 0;

            initSub( -1 );
            setSpriteVisibility( back, true );
            Oam->oamBuffer[ 8 ].isHidden = true;
            Oam->oamBuffer[ 8 ].x = SCREEN_WIDTH - 28;
            Oam->oamBuffer[ 8 ].y = SCREEN_HEIGHT - 28;
            updateOAMSub( Oam );

            this->run_dex( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_speciesId - 1 );

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
                if( AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->m_isFieldAttack )
                    u++;
            o--;
            if( AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->possible( ) ) {
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
                movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );

                bgUpdate( );
                swiWaitForVBlank( );

                char buffer[ 50 ];
                sprintf( buffer, "%ls setzt %s\nein!", SAV.m_PkmnTeam[ acIn ].m_boxdata.m_name, AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->m_moveName.c_str( ) );
                messageBox( buffer, true, true );
                shoUseAttack( SAV.m_PkmnTeam[ acIn ].m_boxdata.m_speciesId, SAV.m_PkmnTeam[ acIn ].m_boxdata.m_isFemale, SAV.m_PkmnTeam[ acIn ].m_boxdata.isShiny( ) );
                AttackList[ SAV.m_PkmnTeam[ acIn ].m_boxdata.m_moves[ o ] ]->use( );
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

        if( SAV.m_inDex[ p_pkmnId - 1 ] ) {
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
            if( SAV.m_inDex[ p_pkmnId - 1 ] ) {
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
            if( SAV.m_inDex[ p_pkmnId - 1 ] ) {
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
    printf( "Gefangen: %3i", SAV.m_dex );

    u16 acNum = p_pkmnId, maxn = SAV.m_hasGDex ? 649 : 493, t2 = tilecnt;
    u8 o2 = oamInd, p2 = palcnt, acPage = 0, acMap = acMapRegion;

    consoleSetWindow( &Top, 0, 0, 32, 24 );
    consoleSelect( &Top );
    consoleClear( );
    for( u16 i = ( acNum + maxn - 3 ) % maxn, j = 0; j < 5; i = ( i + 1 ) % maxn, ++j )
        if( i == acNum ) {
        drawTopDexPage( acPage, i + 1, acForme );
        FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true );
        --j;
        continue;
        } else
            FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );

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
                    FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
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
                    FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
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
                    FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
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
                    FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
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
                    FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ 8 ] + 16, dexsppos[ 1 ][ 8 ] + 16, o2, p2, t2, true ); continue;
                    } else
                        FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_inDex[ i ] ? i + 1 : 0, dexsppos[ 0 ][ j ], dexsppos[ 1 ][ j ], o2, p2, t2, true );
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

void initBagSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, u8& p_oamIndex, u8& p_palCnt, u16& p_tileCnt, bool p_subScreen = true ) {
    /* Keep track of the available tiles */
    p_tileCnt = 16;
    p_oamIndex = 0;
    p_palCnt = 0;
    if( p_subScreen ) {
        SpriteInfo * backInfo = &p_spriteInfo[ p_oamIndex ];
        SpriteEntry * back = &p_oam->oamBuffer[ p_oamIndex ];
        backInfo->m_oamId = p_oamIndex;
        backInfo->m_width = 32;
        backInfo->m_height = 32;
        backInfo->m_angle = 0;
        backInfo->m_entry = back;
        back->y = SCREEN_HEIGHT - 28;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = SCREEN_WIDTH - 28;
        back->rotationIndex = backInfo->m_oamId;
        back->size = OBJSIZE_32;
        back->gfxIndex = p_tileCnt;
        back->priority = OBJPRIORITY_0;
        back->palette = backInfo->m_oamId;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BackTiles,
                          &SPRITE_GFX_SUB[ back->gfxIndex * OFFSET_MULTIPLIER ],
                          BackTilesLen );
        p_tileCnt += BackTilesLen / BYTES_PER_16_COLOR_TILE;

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL,
                          BackPal,
                          &SPRITE_PALETTE_SUB[ backInfo->m_oamId * COLORS_PER_PALETTE ],
                          32 );
        p_palCnt = backInfo->m_oamId + 1;


    } else {
        SpriteInfo * BoxInfo = &p_spriteInfo[ ++p_oamIndex ];
        SpriteEntry * Box = &p_oam->oamBuffer[ p_oamIndex ];
        BoxInfo->m_oamId = 1;
        BoxInfo->m_width = 64;
        BoxInfo->m_height = 64;
        BoxInfo->m_angle = 0;
        BoxInfo->m_entry = Box;
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
        BagSprInfo->m_oamId = 11;
        BagSprInfo->m_width = 32;
        BagSprInfo->m_height = 32;
        BagSprInfo->m_angle = 0;
        BagSprInfo->m_entry = BagSpr;
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
        BagSprInfo->m_oamId = 13 + MAXITEMSPERPAGE;
        BagSprInfo->m_width = 32;
        BagSprInfo->m_height = 16;
        BagSprInfo->m_angle = 0;
        BagSprInfo->m_entry = BagSpr;
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

const u8 MAXPERM = 14;
void getRanPerm( u8* p_array, u8* p_out ) {
    u16 used = 0;
    for( u8 i = 0; i < MAXPERM; ++i ) {
        int newInd = rand( ) % MAXPERM;
        while( used & ( 1 << newInd ) )
            newInd = rand( ) % MAXPERM;
        p_out[ newInd ] = p_array[ i ];
        used |= ( 1 << newInd );
    }
}

const std::string choi[ 6 ] = { "\nEinsetzen.", "\nEinem Pok\x82""mon geben.", "\nRegistrieren.", "\nWeitere Daten ansehen." };
u8 getAnswer( ITEMS::item::itemType p_bagtype ) {
    touchPosition t;
    if( p_bagtype != ITEMS::item::BERRIES ) {
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
        if( p_bagtype != ITEMS::item::KEY_ITEM )
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
        for( u8 i = 0; i < 3; ++i ) {
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
                for( u8 i = 0; i < 3; ++i ) {
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
                for( u8 i = 0; i < 3; ++i ) {
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

                for( u8 i = 0; i < 3; ++i ) {
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
void drawBagPage( u8 p_page, u8* p_position, u8 &p_oamIndex, u8& p_palCnt, u16& p_tileCnt, u8 &p_oamIndexTop, u8& p_palCntTop, u16& p_tileCntTop ) {
    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );
    for( u8 j = 11; j < 19; ++j ) {
        Oam->oamBuffer[ j + 2 ].isHidden = true;
        updateOAMSub( Oam );
    }
    u16 ind = 214 + 4 * ( p_page ), maxpage = 1 + ( ( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) ) - 1 ) / 12;
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

    u8 cpy[ MAXPERM ];
    getRanPerm( p_position, cpy );
    for( u8 i = 0; i < MAXPERM; ++i )
        p_position[ i ] = cpy[ i ];

    u8 acpage = 0, oam2 = p_oamIndex, pal2 = p_palCnt;
    u16 tile2 = p_tileCnt;
    touchPosition t;
    for( u8 i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
        updateTime( );
        scanKeys( );
        touchRead( &t );
        if( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 && acpage < maxpage - 1 )
            goto NEXT;
        if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
            goto PREV;
        if( t.px>224 && t.py > 164 )
            goto BACK;
        std::pair<u16, u16> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
        FS::drawItem( Oam, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                      18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == ITEMS::item::KEY_ITEM );
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
            for( u8 i = 0; i < MAXPERM; ++i )
                p_position[ i ] = cpy[ i ];
            oam2 = p_oamIndex;
            pal2 = p_palCnt;
            tile2 = p_tileCnt;
            for( u8 i = 13; i < oam2 + 13; ++i ) Oam->oamBuffer[ i ].isHidden = true;
            updateOAMSub( Oam );
            for( u8 i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                updateTime( );
                scanKeys( );
                touchRead( &t );
                if( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 && acpage < maxpage - 1 )
                    goto NEXT;
                if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
                    goto PREV;
                if( t.px>224 && t.py > 164 )
                    goto BACK;
                std::pair<u16, u16> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                FS::drawItem( Oam, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                              18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == ITEMS::item::KEY_ITEM );
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
            for( u8 i = 0; i < MAXPERM; ++i )
                p_position[ i ] = cpy[ i ];
            oam2 = p_oamIndex;
            pal2 = p_palCnt;
            tile2 = p_tileCnt;
            for( u8 i = 13; i < oam2 + 13; ++i ) Oam->oamBuffer[ i ].isHidden = true;
            updateOAMSub( Oam );
            for( u8 i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                updateTime( );
                scanKeys( );
                touchRead( &t );
                if( ( sqrt( sq( SCREEN_WIDTH - 6 - t.px ) + sq( SCREEN_HEIGHT - 33 - t.py ) ) <= 16 ) && acpage < maxpage - 1 )
                    goto NEXT;
                if( sqrt( sq( SCREEN_WIDTH - 33 - t.px ) + sq( SCREEN_HEIGHT - 6 - t.py ) ) <= 16 && acpage > 0 )
                    goto PREV;
                if( t.px>224 && t.py > 164 )
                    goto BACK;
                std::pair<u16, u16> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                FS::drawItem( Oam, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                              18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == ITEMS::item::KEY_ITEM );
            }
        } else {
            for( u8 io = 0; io < std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++io ) {
                if( sqrt( sq( ( 54 + ( p_position[ io ] % 4 * 48 ) ) - t.px ) + sq( ( 34 + ( p_position[ io ] / 4 ) * 40 ) - t.py ) ) <= 16 ) {
                    while( 1 ) {
                        scanKeys( );
                        swiWaitForVBlank( );
                        updateTime( );
                        auto t = touchReadXY( );
                        if( t.px == 0 && t.py == 0 )
                            break;
                    }
                    oam2 = p_oamIndexTop;
                    pal2 = p_palCntTop;
                    tile2 = p_tileCntTop;
                    consoleSelect( &Top );
                    consoleSetWindow( &Top, 0, 0, 32, 24 );
                    consoleClear( );
                    OamTop->oamBuffer[ 1 ].isHidden = OamTop->oamBuffer[ 2 ].isHidden = OamTop->oamBuffer[ 3 ].isHidden = false;
                    OamTop->oamBuffer[ 1 ].x = 36;
                    OamTop->oamBuffer[ 2 ].x = 100;
                    OamTop->oamBuffer[ 3 ].x = 164;
                    OamTop->oamBuffer[ 1 ].y = OamTop->oamBuffer[ 2 ].y = OamTop->oamBuffer[ 3 ].y = 69;
                    updateOAM( OamTop );

                    std::pair<u16, u16> acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), acpage * 12 + io );
                    FS::drawItem( OamTop, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 22,
                                  50, acElem.second, oam2, pal2, tile2, false, p_page == ITEMS::item::KEY_ITEM );
                    consoleSetWindow( &Top, 7, 9, 13, 1 );
                    consoleSelect( &Top );
                    printf( ITEMS::ItemList[ acElem.first ].getDisplayName( ).c_str( ) );
                    consoleSetWindow( &Top, 5, 11, 24, 10 );
                    printf( ( ITEMS::ItemList[ acElem.first ].getDescription( ) ).c_str( ) );

                    for( u8 i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
                        acElem = SAV.m_bag.elementAt( bag::bagtype( p_page ), i );
                        if( i - acpage * 12 == io )
                            continue;
                        else if( p_position[ i - acpage * 12 ] < 6 )
                            FS::drawItem( OamTop, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 6 + p_position[ i - acpage * 12 ] * 40,
                            10, acElem.second, oam2, pal2, tile2, false, p_page == ITEMS::item::KEY_ITEM );
                        else if( p_position[ i - acpage * 12 ] < 12 )
                            FS::drawItem( OamTop, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 6 + ( p_position[ i - acpage * 12 ] - 6 ) * 40,
                            138, acElem.second, oam2, pal2, tile2, false, p_page == ITEMS::item::KEY_ITEM );
                        else if( p_position[ i - acpage * 12 ] == 12 )
                            FS::drawItem( OamTop, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 238,
                            82, acElem.second, oam2, pal2, tile2, false, p_page == ITEMS::item::KEY_ITEM );
                        else if( p_position[ i - acpage * 12 ] == 13 )
                            FS::drawItem( OamTop, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, -10,
                            82, acElem.second, oam2, pal2, tile2, false, p_page == ITEMS::item::KEY_ITEM );

                        updateTime( );
                    }


                    initOAMTableSub( Oam );
                    initMainSprites( Oam, spriteInfo );
                    setMainSpriteVisibility( true );
                    Oam->oamBuffer[ 8 ].isHidden = true;
                    Oam->oamBuffer[ 0 ].isHidden = false;
                    Oam->oamBuffer[ 1 ].isHidden = true;
                    for( u8 i = 9; i <= 12; ++i )
                        Oam->oamBuffer[ i ].isHidden = true;

                    consoleSelect( &Bottom );
                    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    consoleSetWindow( &Bottom, 1, 1, 30, MAXLINES );
                    swiWaitForVBlank( );
                    updateOAMSub( Oam );

                    u16 acItem = SAV.m_bag.elementAt( ( bag::bagtype )p_page, acpage * 12 + io ).first;
                    u8 ret = getAnswer( ITEMS::ItemList[ acItem ].getItemType( ) );

                    if( ret == 0 ) {
                        //TODO
                    } else if( ret == 1 && ITEMS::ItemList[ SAV.m_bag.elementAt( ( bag::bagtype )p_page, acpage * 12 + io ).first ].getItemType( ) != ITEMS::item::KEY_ITEM ) {
                        u8 a = 2, b = 2;
                        u16 c = 81;
                        u8 num = (int)SAV.m_PkmnTeam.size( );
                        consoleSelect( &Bottom );
                        for( u8 i = 17; i < 26; i += ( ( ( i - 15 ) / 2 ) % 2 ? -2 : +6 ) ) {
                            if( ( ( ( i - 15 ) / 2 ) ^ 1 ) >= num )
                                break;
                            ( Oam->oamBuffer[ i ] ).isHidden = false;
                            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
                            ( Oam->oamBuffer[ i + 1 ] ).y -= 16 * ( 2 - ( ( i - 15 ) / 4 ) );
                            ( Oam->oamBuffer[ i ] ).y -= 16 * ( 2 - ( ( i - 15 ) / 4 ) );
                            updateOAMSub( Oam );
                            consoleSetWindow( &Bottom, ( ( Oam->oamBuffer[ i ] ).x + 6 ) / 8, ( ( Oam->oamBuffer[ i ] ).y + 6 ) / 8, 12, 3 );
                            if( !SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_individualValues.m_isEgg ) {
                                printf( "   %3i/%3i\n ", SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_stats.m_acHP, SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_stats.m_maxHP );
                                wprintf( SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_name ); printf( "\n" );
                                printf( "%11s", ITEMS::ItemList[ SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_holdItem ].getDisplayName( ).c_str( ) );
                                FS::drawPKMNIcon( Oam, spriteInfo, SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_speciesId, ( Oam->oamBuffer[ i ] ).x - 4, ( Oam->oamBuffer[ i ] ).y - 20, a, b, c );
                            } else {
                                printf( " \n " );
                                printf( "Ei" ); printf( "\n" );
                                FS::drawEggIcon( Oam, spriteInfo, ( Oam->oamBuffer[ i ] ).x - 4, ( Oam->oamBuffer[ i ] ).y - 20, a, b, c );
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
                            for( u8 i = 17; i < 26; i += ( ( ( i - 15 ) / 2 ) % 2 ? -2 : +6 ) ) {
                                if( ( ( ( i - 15 ) / 2 ) ^ 1 ) >= num )
                                    break;
                                else if( SAV.m_PkmnTeam[ ( ( i - 15 ) / 2 ) ^ 1 ].m_boxdata.m_individualValues.m_isEgg )
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
                                        if( ac.m_boxdata.m_holdItem != 0 ) {//pokemon hat schon Item 

                                            char buffer[ 100 ];
                                            sprintf( buffer, "%ls hält bereits\ndas Item %s.\nSollen die Items getauscht werden?", ac.m_boxdata.m_name, ITEMS::ItemList[ ac.m_boxdata.m_holdItem ].getDisplayName( ).c_str( ) );
                                            messageBox M = messageBox( "", 0, true, false, false );
                                            if( yesNoBox( M ).getResult( buffer ) ) {
                                                u16 pkmnOldItem = ac.m_boxdata.m_holdItem;
                                                ac.m_boxdata.m_holdItem = acItem;
                                                SAV.m_bag.removeItem( bag::bagtype( p_page ), acItem, 1 );
                                                SAV.m_bag.addItem( ITEMS::ItemList[ pkmnOldItem ].m_itemType, pkmnOldItem, 1 );
                                            } else {
                                                goto OUT;
                                            }
                                        } else {
                                            ac.m_boxdata.m_holdItem = acItem;
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

                    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BagTop" );

                    consoleSelect( &Top );
                    consoleSetWindow( &Top, 0, 0, 32, 24 );
                    consoleClear( );
                    for( u8 i = 0; i < 8; ++i ) {
                        consoleSetWindow( &Top, 0, 3 * i, 12, 3 );
                        u16 acIn = 214 + 4 * i;
                        u16 s = SAV.m_bag.size( ( bag::bagtype )i );
                        char buffer[ 50 ];
                        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
                        if( s == 0 )
                            sprintf( buffer, "  0 Items" );
                        else if( s == 1 )
                            sprintf( buffer, "  1 Item" );
                        else
                            sprintf( buffer, "%3i Items", s );

                        cust_font.printString( buffer, 20, 24 * i + 3, false );
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
                    for( u8 i = 0; i < MAXPERM; ++i )
                        p_position[ i ] = cpy[ i ];

                    for( u8 j = 0; j <= 10; ++j )
                        Oam->oamBuffer[ j ].isHidden = false;
                    swiWaitForVBlank( );
                    updateOAMSub( Oam );
                    for( u8 i = acpage * 12; i < acpage * 12 + std::min( int( SAV.m_bag.size( bag::bagtype( p_page ) ) ) - acpage * 12, 12 ); ++i ) {
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
                        FS::drawItem( Oam, spriteInfo, ITEMS::ItemList[ acElem.first ].m_itemName, 38 + ( p_position[ i - ( acpage * 12 ) ] % 4 * 48 ),
                                      18 + ( p_position[ i - ( acpage * 12 ) ] / 4 ) * 40, acElem.second, oam2, pal2, tile2, true, p_page == ITEMS::item::KEY_ITEM );
                        updateTime( );
                    }
                }
            }
        }
    }
}
void bag::draw( ) {
    vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    memset( bgGetGfxPtr( bg2 ), 0, 256 * 192 );
    memset( bgGetGfxPtr( bg3 ), 0, 256 * 192 );
    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition t;
    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "BagTop" );


    cust_font.setColor( 0, 0 );
    cust_font.setColor( 251, 1 );
    cust_font.setColor( 252, 2 );

    BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE[ 251 ] = RGB15( 20, 20, 20 );
    BG_PALETTE[ 252 ] = RGB15( 3, 3, 3 );

    consoleSelect( &Top );
    for( u8 i = 0; i < 8; ++i ) {
        consoleSetWindow( &Top, 0, 3 * i, 12, 3 );
        u16 acIn = 214 + 4 * i;
        u16 s = this->m_bags[ i ].size( );
        //char buffer[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
        if( s == 0 )
            sprintf( buffer, "  0 Items" );
        else if( s == 1 )
            sprintf( buffer, "  1 Item" );
        else
            sprintf( buffer, "%3i Items", s );

        cust_font.printString( buffer, 20, 24 * i + 3, false );
    }
    drawSub( );
    initOAMTableSub( Oam );
    u8 palcnt = 0, oamInd = 0;
    u16 tilecnt = 0;
    initBagSprites( Oam, spriteInfo, oamInd, palcnt, tilecnt );


    initOAMTable( OamTop );
    u8 palcntT = 0, oamIndT = 0;
    u16 tilecntT = 0;
    initBagSprites( OamTop, spriteInfoTop, oamIndT, palcntT, tilecntT, false );

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );

    u8 positions[ 8 ][ 2 ] = { { 112, 32 }, { 144, 48 }, { 160, 80 }, { 144, 112 }, { 112, 128 }, { 80, 112 }, { 64, 80 }, { 80, 48 } };
    //Goods,Keys,TMs,Mails,Medicine,Berries,PokéBalls,BattleItems
    for( u8 i = 11; i < 19; ++i ) {
        Oam->oamBuffer[ i + 2 ].x = positions[ i - 11 ][ 0 ];
        Oam->oamBuffer[ i + 2 ].y = positions[ i - 11 ][ 1 ];
        Oam->oamBuffer[ i + 2 ].isHidden = false;
        updateOAMSub( Oam );

        u8 nx = positions[ i - 11 ][ 0 ] / 8 + 1, ny = positions[ i - 11 ][ 1 ] / 8 + 1, ind = 214 + 4 * ( i - 11 );
        consoleSetWindow( &Bottom, nx, ny, 2, 2 );
        for( u8 p = 0; p < 4; ++p )
            printf( "%c", ind + p );
    }
    u8 pos[ MAXPERM ], cpy[ MAXPERM ] = { 0 };
    for( u8 i = 0; i < MAXPERM; ++i )
        pos[ i ] = i;
    getRanPerm( pos, cpy );
    for( u8 i = 0; i < MAXPERM; ++i )
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
        for( u8 i = 0; i < 8; ++i )
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
            for( u8 j = 0; j <= 10; ++j )
                Oam->oamBuffer[ j ].isHidden = false;
            for( u8 j = 11; j <= 60; ++j )
                Oam->oamBuffer[ j ].isHidden = true;
            for( u8 j = 11; j < 19; ++j ) {
                Oam->oamBuffer[ j + 2 ].x = positions[ j - 11 ][ 0 ];
                Oam->oamBuffer[ j + 2 ].y = positions[ j - 11 ][ 1 ];
                Oam->oamBuffer[ j + 2 ].isHidden = false;
                updateOAMSub( Oam );

                u8 nx = positions[ j - 11 ][ 0 ] / 8 + 1, ny = positions[ j - 11 ][ 1 ] / 8 + 1, ind = 214 + 4 * ( j - 11 );
                consoleSetWindow( &Bottom, nx, ny, 2, 2 );
                for( u8 p = 0; p < 4; ++p )
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