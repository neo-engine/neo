/*
Pokémon Emerald 2 Version
------------------------------

file        : bagUI.cpp
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


#include "screenLoader.h"
#include "bag.h"
#include "defines.h"
#include "item.h"
#include "fs.h"
#include "messageBox.h"
#include "saveGame.h"

#include <vector>

//Sprites
#include "Back.h"
#include "BagBall1.h"
#include "BagBall2.h"
#include "BagHm1.h"
#include "BagHm2.h"
#include "BagKey1.h"
#include "BagKey2.h"
#include "BagMediine1.h"
#include "BagMediine2.h"
#include "BagBerry1.h"
#include "BagBerry2.h"

#include "bag_bg_lower.h"
#include "bag_bg_upper.h"

std::string bagnames[ 8 ] = { "Items", "Basis-Items", "TM/VM", "Items", "Medizin", "Beeren", "Items", "Items" };

void drawBagTop( ) {
    dmaCopy( bag_bg_upperBitmap, bgGetGfxPtr( bg3 ), 256 * 192 );
    dmaCopy( bag_bg_upperPal, BG_PALETTE, 256 * 2 );
}

void drawBagSub( ) {
    dmaCopy( bag_bg_lowerBitmap, bgGetGfxPtr( bg3sub ), 256 * 192 );
    dmaCopy( bag_bg_lowerPal, BG_PALETTE_SUB, 256 * 2 );
}

const unsigned short * bagPals[ 10 ] = { BagBall1Pal, BagBall2Pal, BagMediine1Pal, BagMediine2Pal, BagHm1Pal, BagHm2Pal, BagBerry1Pal, BagBerry2Pal, BagKey1Pal, BagKey2Pal };
const unsigned int * bagTiles[ 10 ] = { BagBall1Tiles, BagBall2Tiles, BagMediine1Tiles, BagMediine2Tiles, BagHm1Tiles, BagHm2Tiles, BagBerry1Tiles, BagBerry2Tiles, BagKey1Tiles, BagKey2Tiles };

u16 tileCntSub;
void loadBagSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo ) {
    tileCntSub = 16;
    tileCntSub = loadSprite( p_oam, p_spriteInfo, BACK_ID, BACK_ID, tileCntSub,
                             SCREEN_WIDTH - 32, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

    tileCntSub = loadSprite( p_oam, p_spriteInfo, 1, 1, tileCntSub,
                             SCREEN_WIDTH - 64, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );
    tileCntSub = loadSprite( p_oam, p_spriteInfo, 2, 2, tileCntSub,
                             SCREEN_WIDTH - 96, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );
    tileCntSub = loadSprite( p_oam, p_spriteInfo, 3, 3, tileCntSub,
                             SCREEN_WIDTH - 128, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );

    u8 oamIdx = 3;
    u8 palIdx = 3;
    for( u8 i = 0; i < SAV.m_PkmnTeam.size( ); ++i ) {
        auto acPkmn = SAV.m_PkmnTeam[ i ];

        FS::drawPKMNIcon( Oam,
                          spriteInfo,
                          acPkmn.m_boxdata.m_speciesId,
                          8,
                          26 + i * 26,
                          oamIdx,
                          palIdx,
                          tileCntSub,
                          true );
    }
    for( u8 i = 0; i < 5; ++i ) {
        tileCntSub = loadSprite( p_oam, p_spriteInfo, 10 + i, 10 + i, tileCntSub,
                                 26 * SAV.m_bagPoses[ i ], 3, 32, 32, bagPals[ 2 * i ],
                                 bagTiles[ 2 * i ], BackTilesLen, false, false, false, OBJPRIORITY_0, true );
    }
}

#define GENERAL_ITEMS 0
#define MEDICINE 1
#define TMHM 2
#define BERRIES 3
#define KEY_ITEMS 4

void showActiveBag( u8 p_bagNo, bool p_active = true ) {
    u16 tileIdx = Oam->oamBuffer[ 10 + p_bagNo ].gfxIndex;
    loadSprite( Oam, spriteInfo, 10 + p_bagNo, 10 + p_bagNo, tileIdx,
                26 * SAV.m_bagPoses[ p_bagNo ], 3, 32, 32, bagPals[ 2 * p_bagNo + p_active ],
                bagTiles[ 2 * p_bagNo + p_active ], BackTilesLen, false, false, false, OBJPRIORITY_0, true );
}

void initColors( ) {

    cust_font.setColor( 0, 0 );
    cust_font.setColor( BLACK_IDX, 1 );
    cust_font.setColor( GRAY_IDX, 2 );
    cust_font2.setColor( 0, 0 );
    cust_font2.setColor( GRAY_IDX, 1 );
    cust_font2.setColor( WHITE_IDX, 2 );

    BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
    BG_PALETTE_SUB[ GRAY_IDX ] = STEEL;
    BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
    BG_PALETTE_SUB[ RED_IDX ] = RED;
    BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
    BG_PALETTE[ WHITE_IDX ] = WHITE;
    BG_PALETTE[ GRAY_IDX ] = STEEL;
    BG_PALETTE[ BLACK_IDX ] = BLACK;
    BG_PALETTE[ RED_IDX ] = RED;
    BG_PALETTE[ BLUE_IDX ] = BLUE;

}

void drawActiveBagTop( u8 p_bagNo ) {
    drawBagTop( );
    initColors( );

    u8 idx = 0;
    switch( p_bagNo ) {
        case GENERAL_ITEMS:{
            cust_font.printString( bagnames[ 0 ].c_str( ), 106, 4, false );
            idx = 0;
            break;
        }
        case MEDICINE:{
            cust_font.printString( bagnames[ 4 ].c_str( ), 104, 4, false );
            idx = 4;
            break;
        }
        case TMHM:{
            cust_font.printString( bagnames[ 2 ].c_str( ), 106, 4, false );
            idx = 2;
            break;
        }
        case BERRIES:{
            cust_font.printString( bagnames[ 5 ].c_str( ), 104, 4, false );
            idx = 5;
            break;
        }
        case KEY_ITEMS:{
            cust_font.printString( bagnames[ 1 ].c_str( ), 90, 4, false );
            idx = 1;
            break;
        }
        default:
            return;
    }
    if( idx ) {
        consoleSelect( &Top );
        consoleSetWindow( &Top, 0, 22, 32, 3 );
        consoleClear( );

        consoleSelect( &Top );
        consoleSetWindow( &Top, 0, 22, 12, 3 );
        u16 acIn = 214 + 4 * idx;
        u16 s = SAV.m_bag.m_bags[ idx ].size( );
        //char buffer[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );

        if( s == 1 )
            sprintf( buffer, "  1 Item" );
        else
            sprintf( buffer, "%3i Items", s );

        cust_font.printString( buffer, 16, 177, false );
    } else {
        consoleSelect( &Top );
        consoleSetWindow( &Top, 0, 22, 12, 3 );
        consoleClear( );

        consoleSetWindow( &Top, 0, 22, 12, 3 );
        u16 acIn = 214 + 4 * idx;
        u16 s = SAV.m_bag.m_bags[ idx ].size( );
        //char buffer[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
        sprintf( buffer, "%3i,", s );

        cust_font.printString( buffer, 16, 177, false );

        idx = 3;
        consoleSetWindow( &Top, 6, 22, 12, 3 );
        acIn = 214 + 4 * idx;
        s = SAV.m_bag.m_bags[ idx ].size( );
        //char buffer[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
        sprintf( buffer, "%2i,", s );

        cust_font.printString( buffer, 64, 177, false );


        idx = 6;
        consoleSetWindow( &Top, 11, 22, 12, 3 );
        acIn = 214 + 4 * idx;
        s = SAV.m_bag.m_bags[ idx ].size( );
        //char buffer[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
        sprintf( buffer, "%2i,", s );

        cust_font.printString( buffer, 104, 177, false );


        idx = 7;
        consoleSetWindow( &Top, 16, 22, 12, 3 );
        acIn = 214 + 4 * idx;
        s = SAV.m_bag.m_bags[ idx ].size( );
        //char buffer[ 50 ];
        printf( "%c%c\n%c%c ", ( acIn ), ( acIn + 1 ), ( acIn + 2 ), ( acIn + 3 ) );
        sprintf( buffer, "%2i Items", s );

        cust_font.printString( buffer, 144, 177, false );
    }
}

void drawItemTop( ITEMS::item* p_item, u16 p_count ) {
    u8 oamIdxTop = 0,
        palCntTop = 0;
    u16 tileCntTop = 0;

    std::string display;

    if( p_item->m_itemType != ITEMS::item::itemType::TM_HM ) {
        FS::drawItemIcon( OamTop, spriteInfoTop, p_item->m_itemName, 112, 46, oamIdxTop, palCntTop, tileCntTop, false );
        updateOAM( OamTop );
        display = p_item->getDisplayName( true );
    } else {
        auto mv = *( static_cast<ITEMS::TM*>( p_item ) );
        display = p_item->getDisplayName( true ) + ": " + AttackList[ mv.m_moveIdx ]->m_moveName;
    }

    cust_font.printString( display.c_str( ),
                           128 - cust_font.stringWidth( display.c_str( ) ) / 2, 26, false );
}

void drawPKMNs( OAMTable * p_oam, SpriteInfo *p_spriteInfo ) {

}

void bagUIInit( ) {

    drawBagTop( );
    drawBagSub( );

    initColors( );

    initOAMTable( OamTop );
    initOAMTableSub( Oam );

    loadBagSprites( Oam, spriteInfo );
    drawPKMNs( Oam, spriteInfo );
}

void bagUIDinit( ) {
    initOAMTable( OamTop );
    initOAMTableSub( Oam );
    initMainSprites( Oam, spriteInfo );
    setMainSpriteVisibility( false );
    Oam->oamBuffer[ 8 ].isHidden = true;
    Oam->oamBuffer[ 0 ].isHidden = true;
    Oam->oamBuffer[ 1 ].isHidden = false;
    videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    drawSub( );
}

void bag::draw( u8& p_startBag, u8& p_startItemIdx ) {
    vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    videoSetModeSub( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    memset( bgGetGfxPtr( bg2 ), 0, 256 * 192 );
    memset( bgGetGfxPtr( bg3 ), 0, 256 * 192 );
    Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    consoleSetFont( &Top, &cfont );

    Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    consoleSetFont( &Bottom, &cfont );

    touchPosition t;

    bagUIInit( );

    showActiveBag( p_startBag );
    drawActiveBagTop( p_startBag );

    if( !SAV.m_bag.m_bags[ p_startBag ].empty( ) ) {
        p_startItemIdx = std::min( p_startItemIdx, u8( SAV.m_bag.m_bags[ p_startBag ].size( ) - 1 ) );

        drawItemTop( ITEMS::ItemList[ SAV.m_bag.m_bags[ p_startBag ][ p_startItemIdx ].first ],
                     SAV.m_bag.m_bags[ p_startBag ][ p_startItemIdx ].second );
    }

    while( 42 ) {
        swiWaitForVBlank( );
        updateOAMSub( Oam );
        touchRead( &t );
        scanKeys( );
        u32 pressed = keysDown( ), up = keysUp( ), held = keysHeld( );
        if( ( t.px > 228 && t.py > 168 ) ) { //Back
            Oam->oamBuffer[ BACK_ID ].y -= 4;
            updateOAMSub( Oam );
            if( !waitForTouchUp( false, false, 228, 164 ) ) {
                Oam->oamBuffer[ BACK_ID ].y += 4;
                continue;
            }
            break;
        } else if( ( held & KEY_LEFT ) ) {
            while( 1 ) {
                if( keysUp( ) & KEY_LEFT )
                    break;
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
            }
            showActiveBag( p_startBag, false );

            u8 currBgPos = SAV.m_bagPoses[ p_startBag ];
            currBgPos = ( currBgPos + 4 ) % 5;
            for( u8 i = 0; i < 5; ++i )
                if( SAV.m_bagPoses[ i ] == currBgPos ) {
                    p_startBag = i;
                    break;
                }

            showActiveBag( p_startBag );
            drawActiveBagTop( p_startBag );
        } else if( ( held & KEY_RIGHT ) ) {
            while( 1 ) {
                if( keysUp( ) & KEY_RIGHT )
                    break;
                scanKeys( );
                swiWaitForVBlank( );
                updateTime( );
            }
            showActiveBag( p_startBag, false );

            u8 currBgPos = SAV.m_bagPoses[ p_startBag ];
            currBgPos = ( currBgPos + 1 ) % 5;
            for( u8 i = 0; i < 5; ++i )
                if( SAV.m_bagPoses[ i ] == currBgPos ) {
                    p_startBag = i;
                    break;
                }

            showActiveBag( p_startBag );
            drawActiveBagTop( p_startBag );
        }
        for( u8 i = 0; i < 5; ++i ) {

        }
    }

    bagUIDinit( );
}