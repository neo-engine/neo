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

#include <vector>

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

#include "bag_bg_lower.h"
#include "bag_bg_upper.h"

std::string bagnames[ 8 ] = { "Items", "Basis-Items", "TM/VM", "Briefe", "Medizin", "Beeren", "Pokébälle", "Kampfitems" };

void drawBagTop( ) {
    dmaCopy( bag_bg_upperBitmap, bgGetGfxPtr( bg3 ), 256 * 192 );
    dmaCopy( bag_bg_upperPal, BG_PALETTE, 256 * 2 );
}

void drawBagSub( ) {
    dmaCopy( bag_bg_lowerBitmap, bgGetGfxPtr( bg3sub ), 256 * 192 );
    dmaCopy( bag_bg_lowerPal, BG_PALETTE_SUB, 256 * 2 );
}

u16 tileCntSub;
void loadBagSprites( OAMTable * p_oam, SpriteInfo *p_spriteInfo ) {
    tileCntSub = 16;
    tileCntSub = loadSprite( p_oam, p_spriteInfo, BACK_ID, BACK_ID, tileCntSub,
                             SCREEN_WIDTH - 28, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

    tileCntSub = loadSprite( p_oam, p_spriteInfo, 1, 1, tileCntSub,
                             SCREEN_WIDTH - 28, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
    tileCntSub = loadSprite( p_oam, p_spriteInfo, 2, 2, tileCntSub,
                             SCREEN_WIDTH - 28, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
    tileCntSub = loadSprite( p_oam, p_spriteInfo, 3, 3, tileCntSub,
                             SCREEN_WIDTH - 28, SCREEN_HEIGHT - 24, 32, 32, BackPal,
                             BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

    u8 oamIdx = 4;
    u8 palIdx = 4;
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
}

void drawPKMNs( OAMTable * p_oam, SpriteInfo *p_spriteInfo ) {

}

void bagUIInit( ) {

    drawBagTop( );
    drawBagSub( );

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

void bag::draw( u8 p_startBag, u8 p_startItemIdx ) {
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

    u8 currentBag = 0;

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
        }
    }

    bagUIDinit( );
}