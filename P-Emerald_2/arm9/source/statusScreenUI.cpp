/*
Pokémon neo
------------------------------

file        : statusScreenUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "statusScreenUI.h"
#include "ability.h"
#include "defines.h"
#include "font.h"
#include "fs.h"
#include "locationNames.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"
#include "pokemonNames.h"

#include "hpbar.h"
#include "status_brn.h"
#include "status_fnt.h"
#include "status_frz.h"
#include "status_par.h"
#include "status_psn.h"
#include "status_shiny.h"
#include "status_slp.h"
#include "status_txc.h"
#include "NoItem.h"
#include "arrow_up.h"
#include "infopage1.h"
#include "infopage2.h"
#include "infopage3.h"
#include "infopage4.h"
#include "movebox1.h"
#include "movebox2.h"
#include "noselection_32_64.h"
#include "noselection_64_32.h"
#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "window1.h"
#include "window2.h"
#include "window3.h"
#include "x_16_16.h"
#include "ability1.h"
#include "ability2.h"
#include "ability3.h"
#include "status_pkmn.h"
#include "status_contest.h"
#include "status_moves.h"
#include "backarrow.h"

#include "partybg.h"
#include "partybg2.h"
#include "statussub.h"
#include "statustop.h"


namespace STS {
    // top screen sprites
#define SPR_WINDOW_START_OAM 0
#define SPR_HP_BAR_OAM 10
#define SPR_PKMN_START_OAM 13
//#define SPR_ITEM_OAM 21
#define SPR_TYPE_OAM( p_type ) ( 22 + ( p_type ) )
#define SPR_BALL_ICON_OAM 24
#define SPR_EXP_BAR_OAM 25
#define SPR_CHOICE_START_OAM 28
#define SPR_STATUS_ICON_OAM 36
#define SPR_SHINY_ICON_OAM 37
#define SPR_INFOPAGE_START_OAM 38
#define SPR_PKMN_SHADOW_START_OAM 48
#define SPR_PAGE_OAM 52
#define SPR_WINDOW_OAM 53

#define SPR_INFOPAGE_PAL 0
#define SPR_PKMN_PAL 1
//#define SPR_ITEM_PAL 2
#define SPR_WINDOW_PAL 3
#define SPR_TYPE_PAL( p_type ) ( 4 + ( p_type ) )
#define SPR_BALL_ICON_PAL 6
#define SPR_EXP_BAR_PAL 7
#define SPR_BOX_PAL 8
#define SPR_STATUS_ICON_PAL 9
#define SPR_SHINY_ICON_PAL 10
#define SPR_PKMN_SHADOW_PAL 11
#define SPR_PAGE_PAL 12
#define SPR_ARROW_X_PAL 13

    // Sub screen sprites
#define SPR_INFOPAGE_START_OAM_SUB 0
#define SPR_ARROW_UP_OAM_SUB 18
#define SPR_ARROW_DOWN_OAM_SUB 19
#define SPR_X_OAM_SUB 20
#define SPR_PAGE_LEFT_OAM_SUB 21
#define SPR_PAGE_OAM_SUB( p_page ) ( 22 + ( p_page ) )
#define SPR_NAVIGATION_OAM_SUB( p_page ) ( 25 + ( p_page ) )
#define SPR_TYPE_OAM_SUB( p_idx ) ( 30 + ( p_idx ) )
#define SPR_MOVE_OAM_SUB( p_move ) ( 34 + 6 * ( p_move ) )
#define SPR_ABILITY_OAM_SUB 60
#define SPR_ARROW_BACK_OAM_SUB 70
#define SPR_WINDOW_PKMN_OAM_SUB 71
#define SPR_WINDOW_MOVE_OAM_SUB 72
// #define SPR_WINDOW_CONTEST_OAM_SUB 73

#define SPR_INFOPAGE_PAL_SUB 0
#define SPR_ARROW_X_PAL_SUB 1
#define SPR_BOX_PAL_SUB 2
#define SPR_TYPE_PAL_SUB( p_idx ) ( 3 + ( p_idx ) )
#define SPR_ABILITY_PAL_SUB 8

    u16 statusScreenUI::initTopScreen( pokemon* p_pokemon, bool p_bottom ) {
        IO::clearScreen( p_bottom, false, true );
        IO::initOAMTable( p_bottom );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );

        u16 tileCnt = 0;
        // preload sprites to avoid position calculations later
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        // Info BG
#define INFO_X 104
#define INFO_Y 36

        IO::loadSprite( SPR_INFOPAGE_START_OAM, SPR_INFOPAGE_PAL, tileCnt, INFO_X, INFO_Y, 64, 64,
                        0, 0, infopage1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 8, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 64,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, infopage1TilesLen, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM + 7, SPR_INFOPAGE_PAL, tileCnt,
                                  INFO_X + 64, INFO_Y + 64 + 15, 64, 64, infopage1Pal,
                                  infopage1Tiles, infopage1TilesLen, true, true, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 1, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 64, INFO_Y,
                        64, 64, 0, 0, infopage2TilesLen, false, false, false, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 9, SPR_INFOPAGE_PAL, tileCnt, INFO_X,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, infopage2TilesLen, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM + 6, SPR_INFOPAGE_PAL, tileCnt, INFO_X,
                                  INFO_Y + 64 + 15, 64, 64, infopage2Pal, infopage2Tiles,
                                  infopage2TilesLen, true, true, false, OBJPRIORITY_3, p_bottom,
                                  OBJMODE_BLENDED );
        // Window

        tileCnt = IO::loadSprite( SPR_WINDOW_START_OAM, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                                  INFO_Y, 64, 32, window1Pal, window1Tiles, window1TilesLen, false,
                                  false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 1, SPR_WINDOW_PAL, tileCnt, INFO_X - 14, INFO_Y + 32,
                        64, 32, 0, 0, window2TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 2, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                        INFO_Y + 32 + 30, 64, 32, 0, 0, window2TilesLen, false, false, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt
            = IO::loadSprite( SPR_WINDOW_START_OAM + 3, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                              INFO_Y + 64 + 28, 64, 32, window1Pal, window2Tiles, window2TilesLen,
                              false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 5, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                        INFO_Y + 111 - 30, 64, 32, window3Pal, window3Tiles, window3TilesLen, false,
                        false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_WINDOW_START_OAM + 4, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                                  INFO_Y + 111, 64, 32, window3Pal, window3Tiles, window3TilesLen,
                                  false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Pkmn Sprite
        if( !p_pokemon->isEgg( ) ) {
            tileCnt = IO::loadPKMNSprite( p_pokemon->getSpecies( ), 0, 54, SPR_PKMN_START_OAM,
                                          SPR_PKMN_PAL, tileCnt, p_bottom, p_pokemon->isShiny( ),
                                          p_pokemon->isFemale( ), false, false,
                                          p_pokemon->getForme( ) );
        } else {
            tileCnt = IO::loadEggSprite( 0, 54, SPR_PKMN_START_OAM, SPR_PKMN_PAL, tileCnt, p_bottom,
                                         p_pokemon->getSpecies( ) == PKMN_MANAPHY );
        }
        u16 emptyPal[ 32 ]                                         = {0};
        ( p_bottom ? IO::Oam : IO::OamTop )->matrixBuffer[ 0 ].hdx = ( 1 << 8 );
        ( p_bottom ? IO::Oam : IO::OamTop )->matrixBuffer[ 0 ].vdx = -( 1 << 8 );

        u8 sx = 32, sy = 32;
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 0, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 0 ].gfxIndex, 0 - 48 - sx, 54 - sy, 64, 64,
                        emptyPal, 0, 0, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 1, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 1 ].gfxIndex, 80 - 48 - sx, 54 - sy, 32, 64, 0, 0,
                        0, false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 2, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 2 ].gfxIndex, 0 - sx, 54 + 80 - sy, 64, 32, 0, 0,
                        0, false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 3, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 3 ].gfxIndex, 80 - sx, 54 + 80 - sy, 32, 32, 0, 0,
                        0, false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM + i ].isRotateScale = true;
            oam[ SPR_PKMN_SHADOW_START_OAM + i ].isSizeDouble  = true;
            oam[ SPR_PKMN_SHADOW_START_OAM + i ].rotationIndex = 0;
        }

        /*
        // Item icon
        if( !p_pokemon->getItem( ) ) {
                tileCnt = IO::loadSprite( SPR_ITEM_OAM, SPR_ITEM_PAL, tileCnt, 0, 0, 32, 32,
                                  NoItemPal,
                                  NoItemTiles, NoItemTilesLen, false, false, true, OBJPRIORITY_1,
                                  p_bottom, OBJMODE_NORMAL );
        } else {
            tileCnt = IO::loadItemIcon( p_pokemon->getItem( ), 56, 148,
                                    SPR_ITEM_OAM, SPR_ITEM_PAL, tileCnt, p_bottom );
        }
        */

        // Type icon
        for( u8 i = 0; i < 2; ++i ) {
            tileCnt
                = IO::loadTypeIcon( _data.m_baseForme.m_types[ i ], INFO_X - 7 + 128 - 66 + 34 * i,
                                    INFO_Y + 11 + 15 * 2, SPR_TYPE_OAM( i ), SPR_TYPE_PAL( i ),
                                    tileCnt, p_bottom, CURRENT_LANGUAGE );
            oam[ SPR_TYPE_OAM( i ) ].isHidden = true;
        }

        // Pokéball Icon
        tileCnt = IO::loadItemIcon( ITEM::ballToItem( p_pokemon->m_boxdata.m_ball ), -4, 28,
                                    SPR_BALL_ICON_OAM, SPR_BALL_ICON_PAL, tileCnt, p_bottom );

        // Name box

        IO::loadSprite( SPR_CHOICE_START_OAM, SPR_BOX_PAL, tileCnt, 0, 28, 16, 32, 0, 0,
                        noselection_96_32_1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 6, SPR_BOX_PAL, tileCnt, 88, 28, 16, 32, 0, 0,
                        noselection_96_32_1TilesLen, true, true, true, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM + 5, SPR_BOX_PAL, tileCnt, 74, 28, 16, 32,
                                  noselection_96_32_1Pal, noselection_96_32_1Tiles,
                                  noselection_96_32_1TilesLen, true, true, false, OBJPRIORITY_3,
                                  p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 1, SPR_BOX_PAL, tileCnt, 16, 28, 16, 32, 0, 0,
                        noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 2, SPR_BOX_PAL, tileCnt, 32, 28, 16, 32, 0, 0,
                        noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 3, SPR_BOX_PAL, tileCnt, 48, 28, 16, 32, 0, 0,
                        noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 7, SPR_BOX_PAL, tileCnt, 75, 28, 16, 32, 0, 0,
                        noselection_96_32_2TilesLen, false, false, true, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM + 4, SPR_BOX_PAL, tileCnt, 64, 28, 16, 32,
                                  noselection_96_32_2Pal, noselection_96_32_2Tiles,
                                  noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                                  p_bottom, OBJMODE_BLENDED );

        // Status
#define STATUS_X 68
#define STATUS_Y 48
        if( !p_pokemon->m_stats.m_curHP ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_fntPal, status_fntTiles,
                                      status_fntTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isParalyzed ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_parPal, status_parTiles,
                                      status_parTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isAsleep ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_slpPal, status_slpTiles,
                                      status_slpTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBadlyPoisoned ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_txcPal, status_txcTiles,
                                      status_txcTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBurned ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_brnPal, status_brnTiles,
                                      status_brnTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isFrozen ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_frzPal, status_frzTiles,
                                      status_frzTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isPoisoned ) {
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL, tileCnt, STATUS_X,
                                      STATUS_Y, 8, 8, status_psnPal, status_psnTiles,
                                      status_psnTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                      p_bottom, OBJMODE_NORMAL );
        }

        // Shiny
        tileCnt = IO::loadSprite( SPR_SHINY_ICON_OAM, SPR_SHINY_ICON_PAL, tileCnt, STATUS_X - 8,
                                  STATUS_Y, 8, 8, status_shinyPal, status_shinyTiles,
                                  status_shinyTilesLen, false, false,
                                  !p_pokemon->isShiny( ) || p_pokemon->isEgg( ), OBJPRIORITY_0,
                                  p_bottom, OBJMODE_NORMAL );

        // Page
        tileCnt = IO::loadSprite( SPR_PAGE_OAM, SPR_PAGE_PAL, tileCnt, 148, -4, 64, 32,
                                  noselection_64_32Pal, noselection_64_32Tiles,
                                  noselection_64_32TilesLen, true, true, false, OBJPRIORITY_2,
                                  p_bottom, OBJMODE_NORMAL );
        tileCnt
            = IO::loadSprite( SPR_WINDOW_OAM, SPR_ARROW_X_PAL, tileCnt,
                              148 + 32 - 9, 6, 16, 16, status_pkmnPal, status_pkmnTiles,
                              status_pkmnTilesLen, false,
                              false, false, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        // HP/EXP bar
        IO::loadSprite( SPR_HP_BAR_OAM + 1, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48,
                        INFO_Y + 4 + 15 * 1, 64, 32, hpbarPal, hpbarTiles, hpbarTilesLen, false,
                        false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_HP_BAR_OAM, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48 - 14,
                        INFO_Y + 4 + 15 * 1, 16, 8, hpbarPal, hpbarTiles, hpbarTilesLen / 16, false,
                        false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_EXP_BAR_OAM + 1, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48 - 14,
                        INFO_Y + 15 + 15 * 7, 16, 8, hpbarPal, hpbarTiles, hpbarTilesLen / 16,
                        false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_EXP_BAR_OAM, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48 - 27,
                        INFO_Y + 15 + 15 * 7, 16, 8, hpbarPal, hpbarTiles, hpbarTilesLen / 16,
                        false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        tileCnt
            = IO::loadSprite( SPR_EXP_BAR_OAM + 2, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48,
                              INFO_Y + 15 + 15 * 7, 64, 32, hpbarPal, hpbarTiles, hpbarTilesLen,
                              false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );

        IO::updateOAM( p_bottom );
        return tileCnt;
    }

    u16 statusScreenUI::initBottomScreen( pokemon* p_pokemon, bool p_bottom ) {
        IO::clearScreen( p_bottom, false, true );
        if( p_bottom ) {
            dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        } else {
            dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        }

        IO::initOAMTable( p_bottom );

        SpriteEntry* oam     = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          tileCnt = 0;

        // Arrows
        tileCnt
            = IO::loadSprite( SPR_ARROW_DOWN_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 102 + 19,
                              192 - 21, 16, 16, arrow_upPal, arrow_upTiles, arrow_upTilesLen, true,
                              false, !_allowKeyDown, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ARROW_UP_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                        oam[ SPR_ARROW_DOWN_OAM_SUB ].gfxIndex, 102 + 38 + 18, 192 - 16, 16, 16,
                        arrow_upPal, arrow_upTiles, arrow_upTilesLen, false, false, !_allowKeyUp,
                        OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        tileCnt
            = IO::loadSprite( SPR_ARROW_BACK_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 102 + 56 + 32,
                              192 - 19, 16, 16, backarrowPal, backarrowTiles,
                              backarrowTilesLen, false,
                              false, false, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        // x
        tileCnt = IO::loadSprite( SPR_X_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 236, 172, 16, 16,
                                  x_16_16Pal, x_16_16Tiles, x_16_16TilesLen, false, false, false,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        // page windows
        tileCnt = IO::loadSprite( SPR_PAGE_LEFT_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, 0 - 8, 57 - 12,
                                  32, 64, noselection_32_64Pal, noselection_32_64Tiles,
                                  noselection_32_64TilesLen, true, true, true, OBJPRIORITY_2,
                                  p_bottom, OBJMODE_NORMAL );
        for( u8 i = 0; i < 3; i++ ) {
            IO::loadSprite( SPR_PAGE_OAM_SUB( i ), SPR_BOX_PAL_SUB,
                            oam[ SPR_PAGE_LEFT_OAM_SUB ].gfxIndex, 256 - 24, 4 + 28 * i, 32, 64,
                            noselection_32_64Pal, noselection_32_64Tiles, noselection_32_64TilesLen,
                            false, false, true, OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );
        }


        tileCnt
            = IO::loadSprite( SPR_WINDOW_PKMN_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt,
                              256 - 18, 24, 16, 16, 0, status_pkmnTiles,
                              status_pkmnTilesLen, false,
                              false, false, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        tileCnt
            = IO::loadSprite( SPR_WINDOW_MOVE_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt,
                              256 - 18, 24 + 30, 16, 16, 0, status_movesTiles,
                              status_movesTilesLen, false,
                              false, false, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
//        tileCnt
//            = IO::loadSprite( SPR_WINDOW_CONTEST_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt,
//                              256 - 18, 24 + 57, 16, 16, 0, status_contestTiles,
//                              status_contestTilesLen, false,
//                              false, false, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        // navigation
        tileCnt = IO::loadSprite( SPR_NAVIGATION_OAM_SUB( 0 ), SPR_BOX_PAL_SUB, tileCnt, 0, 0, 64,
                                  32, noselection_64_32Pal, noselection_64_32Tiles,
                                  noselection_64_32TilesLen, true, true, true, OBJPRIORITY_2,
                                  p_bottom, OBJMODE_NORMAL );
        for( u8 i = 0; i < 3; i++ ) {
            IO::loadSprite( SPR_NAVIGATION_OAM_SUB( i ), SPR_BOX_PAL_SUB,
                            oam[ SPR_NAVIGATION_OAM_SUB( 0 ) ].gfxIndex, 102 + 64 - 32 * i,
                            192 - 24, 64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_2, p_bottom,
                            OBJMODE_NORMAL );
        }
        oam[ SPR_NAVIGATION_OAM_SUB( 2 ) ].isHidden = !_allowKeyUp;
        oam[ SPR_NAVIGATION_OAM_SUB( 1 ) ].isHidden = !_allowKeyDown;

        // Info BG
#define INFO_X_SUB 32
#define INFO_Y_SUB 4

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 11, SPR_INFOPAGE_PAL_SUB, tileCnt, INFO_X_SUB,
                        INFO_Y_SUB, 64, 64, 0, 0, infopage1TilesLen, false, false, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 7, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 64, INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, infopage1TilesLen,
                        true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 6, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 64, INFO_Y_SUB + 64 + 30, 64, 64, 0, 0, infopage1TilesLen,
                        true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 4, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 96, INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, infopage1TilesLen,
                        true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 3, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 96, INFO_Y_SUB + 64 + 30, 64, 64, 0, 0, infopage1TilesLen,
                        true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 1, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 128, INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, infopage1TilesLen,
                        true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB, SPR_INFOPAGE_PAL_SUB, tileCnt, INFO_X_SUB + 128,
                        INFO_Y_SUB + 64 + 30, 64, 64, 0, 0, infopage1TilesLen, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 6, SPR_INFOPAGE_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 64, INFO_Y_SUB + 64 + 30, 64, 64, infopage1Pal,
                                  infopage1Tiles, infopage1TilesLen, true, true, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 8, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 64, INFO_Y_SUB, 64, 64, 0, 0, infopage2TilesLen, false, false,
                        false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 10, SPR_INFOPAGE_PAL_SUB, tileCnt, INFO_X_SUB,
                        INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, infopage2TilesLen, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 5, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 96, INFO_Y_SUB, 64, 64, 0, 0, infopage2TilesLen, false, false,
                        false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 2, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 128, INFO_Y_SUB, 64, 64, 0, 0, infopage2TilesLen, false, false,
                        false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 9, SPR_INFOPAGE_PAL_SUB, tileCnt,
                                  INFO_X_SUB, INFO_Y_SUB + 64 + 30, 64, 64, infopage2Pal,
                                  infopage2Tiles, infopage2TilesLen, true, true, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Moves
        // move windows
        for( u8 i = 1; i < 3; i++ ) {
            u8 pos = 2 * i - 2;
            if( i == 1 ) {
                tileCnt = IO::loadSprite(
                    SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ), tileCnt, 29, 44 + i * 44, 16,
                    32, 0, movebox1Tiles, movebox1TilesLen, false, false,
                    !p_pokemon->m_boxdata.m_moves[ pos ], OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
                tileCnt = IO::loadSprite(
                    SPR_MOVE_OAM_SUB( pos ) + 1, SPR_TYPE_PAL_SUB( pos ), tileCnt, 29 + 16,
                    44 + i * 44, 16, 32, 0, movebox2Tiles, movebox2TilesLen, false, false,
                    !p_pokemon->m_boxdata.m_moves[ pos ], OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            } else {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 29, 44 + i * 44, 16, 32, 0,
                                0, 0, false, false, !p_pokemon->m_boxdata.m_moves[ pos ],
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 1, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16, 44 + i * 44, 16,
                                32, 0, 0, 0, false, false, !p_pokemon->m_boxdata.m_moves[ pos ],
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + j, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 16, 44 + i * 44,
                                16, 32, 0, 0, 0, false, false, !p_pokemon->m_boxdata.m_moves[ pos ],
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 5, SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 29 + 5 * 16, 44 + i * 44, 16, 32,
                            0, 0, 0, true, true, !p_pokemon->m_boxdata.m_moves[ pos ],
                            OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        }
        for( u8 i = 1; i < 3; i++ ) {
            u8 pos = 2 * i - 1;
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 131, 44 + i * 44, 16, 32, 0, 0,
                            0, false, false, !p_pokemon->m_boxdata.m_moves[ pos ], OBJPRIORITY_3,
                            p_bottom, OBJMODE_NORMAL );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + j, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 16,
                                44 + i * 44, 16, 32, 0, 0, 0, false, false,
                                !p_pokemon->m_boxdata.m_moves[ pos ], OBJPRIORITY_3, p_bottom,
                                OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 5, SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 131 + 5 * 16, 44 + i * 44, 16,
                            32, 0, 0, 0, true, true, !p_pokemon->m_boxdata.m_moves[ pos ],
                            OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
        }

        // type icons
        for( u8 i = 0; i < 4; ++i ) {
            type t;
            if( p_pokemon->m_boxdata.m_moves[ i ] != M_HIDDEN_POWER ) {
                auto mdata = MOVE::getMoveData( p_pokemon->m_boxdata.m_moves[ i ] );
                t          = mdata.m_type;
            } else {
                t = p_pokemon->getHPType( );
            }
            tileCnt = IO::loadTypeIcon( t, oam[ SPR_MOVE_OAM_SUB( i ) ].x - 4,
                                        oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_TYPE_OAM_SUB( i ),
                                        SPR_TYPE_PAL_SUB( i ), tileCnt, true, CURRENT_LANGUAGE );

            IO::copySpritePal( movebox1Pal + 4, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, p_bottom );
        }

        // ability window

        tileCnt = IO::loadSprite( SPR_ABILITY_OAM_SUB, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 88, INFO_Y_SUB, 32, 64, 0,
                                  ability3Tiles, ability3TilesLen, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_ABILITY_OAM_SUB + 1, SPR_ABILITY_PAL_SUB, tileCnt,
                        INFO_X_SUB - 12, INFO_Y_SUB, 64, 64, 0, 0, 0, false, false, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_ABILITY_OAM_SUB + 2, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 36, INFO_Y_SUB, 64, 64, ability1Pal,
                                  ability1Tiles, ability1TilesLen, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_ABILITY_OAM_SUB + 4, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 64 + 36, INFO_Y_SUB, 64, 64, 0, 0, 0,
                                  false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_ABILITY_OAM_SUB + 3, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 64 + 68, INFO_Y_SUB, 64, 64, 0,
                                  ability2Tiles, ability2TilesLen, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Build the shared pals
        IO::copySpritePal( arrow_upPal, SPR_ARROW_X_PAL_SUB, 0, 2 * 4, p_bottom );
        IO::copySpritePal( x_16_16Pal + 4, SPR_ARROW_X_PAL_SUB, 4, 2 * 3, p_bottom );

        return tileCnt;
    }

    void statusScreenUI::drawBasicInfoTop( pokemon* p_pokemon, bool p_bottom ) {
        if( p_bottom ) {
            dmaCopy( statustopBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        } else {
            dmaCopy( statustopBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        }
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        for( u8 i = 0; i < 6; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = true; }
        oam[ SPR_TYPE_OAM( 1 ) ].isHidden = true;
        if( _data.m_baseForme.m_types[ 0 ] != _data.m_baseForme.m_types[ 1 ] ) {
            oam[ SPR_TYPE_OAM( 0 ) ].isHidden = true;
        }
        oam[ SPR_BALL_ICON_OAM ].isHidden        = true;
        oam[ SPR_HP_BAR_OAM ].isHidden           = true;
        oam[ SPR_HP_BAR_OAM + 1 ].isHidden       = true;
        oam[ SPR_EXP_BAR_OAM ].isHidden          = true;
        oam[ SPR_EXP_BAR_OAM + 1 ].isHidden      = true;
        oam[ SPR_EXP_BAR_OAM + 2 ].isHidden      = true;
        oam[ SPR_CHOICE_START_OAM + 5 ].isHidden = true;
        oam[ SPR_CHOICE_START_OAM + 6 ].isHidden = true;
        oam[ SPR_CHOICE_START_OAM + 7 ].isHidden = true;

        // Print Basic Pkmn info
        if( !p_pokemon->isEgg( ) ) {
            oam[ SPR_BALL_ICON_OAM ].isHidden        = false;
            oam[ SPR_CHOICE_START_OAM + 5 ].isHidden = false;
            //
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = false; }
            if( IO::regularFont->stringWidthC( p_pokemon->m_boxdata.m_name ) > 66 ) {
                s8 diff = 66
                    - IO::regularFont->stringWidthC( p_pokemon->m_boxdata.m_name );
                oam[ SPR_BALL_ICON_OAM ].x = diff - 2;
                IO::regularFont->printStringC( p_pokemon->m_boxdata.m_name, 22 + diff, 31,
                        p_bottom );
            } else {
                IO::regularFont->printStringC( p_pokemon->m_boxdata.m_name, 22, 31, p_bottom );
            }

            // Level
            IO::smallFont->setColor( 0, 0 );
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( IO::GRAY_IDX, 2 );
            IO::smallFont->printString( ( "!" + std::to_string( p_pokemon->m_level ) ).c_str( ), 24,
                                        39, p_bottom );

            // Gender
            if( p_pokemon->getSpecies( ) != PKMN_NIDORAN_F
                && p_pokemon->getSpecies( ) != PKMN_NIDORAN_M ) {
                if( p_pokemon->m_boxdata.m_isFemale ) {
                    IO::regularFont->setColor( IO::RED_IDX, 1 );
                    IO::regularFont->setColor( IO::RED2_IDX, 2 );
                    IO::regularFont->printString( "}", 79, 43, p_bottom );
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                } else if( !p_pokemon->m_boxdata.m_isGenderless ) {
                    IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                    IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
                    IO::regularFont->printString( "{", 79, 43, p_bottom );
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                }
            }
            // Item
            if( p_pokemon->getItem( ) ) {
                IO::regularFont->printString( GET_STRING( 362 ), 8, 166, p_bottom );
                IO::regularFont->printStringC( ITEM::getItemName( p_pokemon->getItem( ) ).c_str( ),
                                               16, 178, p_bottom );
            }
        } else {
            oam[ SPR_CHOICE_START_OAM + 6 ].isHidden = false;
            oam[ SPR_CHOICE_START_OAM + 7 ].isHidden = false;

            // Egg
            IO::regularFont->printString( GET_STRING( 34 ), 12, 36, p_bottom );

            u8 startline = 0;

            IO::regularFont->setColor( IO::BLUE_IDX, 2 );
            writeLineTop( IO::formatDate( p_pokemon->m_boxdata.m_gotDate ).c_str( ), startline++,
                          IO::BLUE2_IDX );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            if( p_pokemon->getSpecies( ) == PKMN_MANAPHY ) {
                writeLineTop( GET_STRING( 359 ), startline++ );
                writeLineTop( GET_STRING( 360 ), startline++ );
                writeLineTop( GET_STRING( 361 ), startline++ );
            } else {
                writeLineTop( GET_STRING( 354 ), startline++ );
                writeLineTop( GET_STRING( 355 ), startline++ );
            }
            u8 shift = 2;
            switch( p_pokemon->m_boxdata.m_gotPlace ) {
            case L_POKEMON_DAY_CARE:
            case L_DAY_CARE_COUPLE:
            case L_TRAVELING_MAN:
            case L_POKEWALKER:
                writeLineTop( GET_STRING( 356 ), startline );
                shift += IO::regularFont->stringWidth( GET_STRING( 356 ) );
                break;
            default:
                writeLineTop( GET_STRING( 357 ), startline );
                shift += IO::regularFont->stringWidth( GET_STRING( 357 ) );
                break;
            case L_RILEY:
            case L_CYNTHIA:
            case L_MR_POKEMON:
            case L_PRIMO:
            case L_POKEMON_RANGER:
                writeLineTop( GET_STRING( 358 ), startline );
                shift += IO::regularFont->stringWidthC( GET_STRING( 358 ) );
                break;
            }
            auto loc = FS::getLocation( p_pokemon->m_boxdata.m_gotPlace );
            IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
            IO::regularFont->setColor( IO::BLUE_IDX, 2 );
            IO::regularFont->printStringC( loc.c_str( ), INFO_X + 9 + shift,
                                           INFO_Y + 11 + 15 * startline, p_bottom );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->printStringC(
                ".",
                INFO_X + 9 + shift + IO::regularFont->stringWidth( loc.c_str( ) ) - loc.length( ),
                INFO_Y + 11 + 15 * startline, p_bottom );

            startline = 5;

            if( p_pokemon->m_boxdata.m_steps > 10 ) {
                for( u8 i = 0; i < 3; ++i ) {
                    writeLineTop( GET_STRING( 345 + i ), startline + i );
                }
            } else if( p_pokemon->m_boxdata.m_steps > 5 ) {
                for( u8 i = 0; i < 3; ++i ) {
                    writeLineTop( GET_STRING( 348 + i ), startline + i );
                }
            } else {
                for( u8 i = 0; i < 3; ++i ) {
                    writeLineTop( GET_STRING( 351 + i ), startline + i );
                }
            }
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        }
    }

    void statusScreenUI::writeLineTop( const char* p_stringLeft, const char* p_stringRight,
                                       u8 p_line, u8 p_colorLeft, u8 p_colorRight, bool p_bottom ) {
        if( p_stringLeft ) {
            IO::regularFont->setColor( p_colorLeft, 1 );
            IO::regularFont->printStringC( p_stringLeft, INFO_X - 7, INFO_Y + 11 + 15 * p_line,
                                           p_bottom );
        }
        if( p_stringRight ) {
            IO::regularFont->setColor( p_colorRight, 1 );
            if( IO::regularFont->stringWidth( p_stringRight ) > 78 ) {
                IO::regularFont->printStringC( p_stringRight, INFO_X - 7 + 128,
                                          INFO_Y + 11 + 15 * p_line, p_bottom, IO::font::RIGHT );
            } else {
                IO::regularFont->printString( p_stringRight, INFO_X - 7 + 128,
                                          INFO_Y + 11 + 15 * p_line, p_bottom, IO::font::RIGHT );
            }
        }
    }

    void statusScreenUI::writeLineTop( const char* p_string, u8 p_line, u8 p_color,
                                       bool p_bottom ) {
        IO::regularFont->setColor( p_color, 1 );
        IO::regularFont->printStringC( p_string, INFO_X + 9, INFO_Y + 11 + 15 * p_line, p_bottom );
    }

#define INFO_LINE_SUB( p_line ) ( INFO_Y_SUB + 11 + 15 * ( p_line ) )

    void statusScreenUI::init( pokemon* p_pokemon, u8 p_initialPage, bool p_allowKeyUp,
                               bool p_allowKeyDown ) {
        _allowKeyUp   = p_allowKeyUp;
        _allowKeyDown = p_allowKeyDown;

        REG_BLDALPHA_SUB = 0;
        REG_BLDALPHA     = 0;
        bgUpdate( );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::clearScreen( true, true, true );
        IO::vramSetup( true );
        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );
        _data = getPkmnData( p_pokemon->getSpecies( ), p_pokemon->getForme( ) );

        initTopScreen( p_pokemon );
        initBottomScreen( p_pokemon );

        _needsInit = true;
        draw( p_pokemon, p_initialPage );

        dmaCopy( partybgPal, BG_PALETTE, 3 * 2 );
        dmaCopy( statustopPal + 3, BG_PALETTE + 3, 8 * 2 );
        dmaCopy( partybgPal, BG_PALETTE_SUB, 3 * 2 );
        dmaCopy( statussubPal + 3, BG_PALETTE_SUB + 3, 8 * 2 );

        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = BG_PAL( i );
            pal[ IO::COLOR_IDX ] = RGB( 22, 22, 22 );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = RGB( 16, 16, 16 );
            pal[ IO::BLACK_IDX ] = RGB( 8, 8, 8 );
            pal[ IO::BLUE_IDX ]  = RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = RGB( 23, 0, 0 );

            pal[ 238 ] = RGB( 13, 25, 25 ); // exp bar blue 1
            pal[ 239 ] = RGB( 6, 18, 18 );  // exp bar blue 2
            pal[ 240 ] = RGB( 6, 6, 6 );    // hp bar border color
            pal[ 241 ] = RGB( 12, 30, 12 ); // hp bar green 1
            pal[ 242 ] = RGB( 3, 23, 4 );   // hp bar green 2
            pal[ 243 ] = RGB( 30, 30, 12 ); // hp bar yellow 1
            pal[ 244 ] = RGB( 23, 23, 5 );  // hp bar yellow 2
            pal[ 245 ] = RGB( 30, 15, 12 ); // hp bar red 1
            pal[ 246 ] = RGB( 20, 7, 7 );   // hp bar red 2
        }
        bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
        bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
        bgSetScroll( IO::bg3sub, 0, 0 );
        bgSetScroll( IO::bg3, 0, 0 );
        dmaCopy( partybg2Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
        _needsInit = false;
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
        REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
        bgUpdate( );
    }

    void statusScreenUI::draw( pokemon* p_pokemon, u8 p_page ) {
        if( !p_pokemon ) { return; }
        if( !_needsInit && ( p_page == _currentPage || p_pokemon->isEgg( ) ) ) { return; }
        _currentPage =  p_pokemon->isEgg( ) ? 0 : p_page;
        REG_BLDALPHA_SUB = 0;
        REG_BLDALPHA     = 0;
        bgUpdate( );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );


        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        drawBasicInfoTop( p_pokemon, false );
        IO::regularFont->printString( GET_STRING( 344 ), 34, 2, false );
        SpriteEntry* oam                           = IO::OamTop->oamBuffer;
        SpriteEntry* oamSub                        = IO::Oam->oamBuffer;
        oam[ SPR_INFOPAGE_START_OAM + 6 ].isHidden = false;
        oam[ SPR_INFOPAGE_START_OAM + 7 ].isHidden = false;

        for( u8 i = 0; i < getPageCount( ); ++i ) {
            oamSub[ SPR_PAGE_OAM_SUB( i ) ].isHidden = true;
        }
        for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true; }

        for( u8 i = 0; i < 4; ++i ) {
            oamSub[ SPR_TYPE_OAM_SUB( i ) ].isHidden = true;
            for( u8 j = 0; j < 6; j++ ) { oamSub[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = true; }
        }

        oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden = true;
        oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden = true;
        //            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = true;
        for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = true; }
        for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true; }
        for( u8 i = 0; i < getPageCount( ); ++i ) {
            oamSub[ SPR_PAGE_OAM_SUB( i ) ].isHidden = ( i == _currentPage ) || p_pokemon->isEgg( );
        }

        if( p_pokemon->isEgg( ) ) {
            IO::loadSprite( SPR_WINDOW_OAM, SPR_ARROW_X_PAL, oam[ SPR_WINDOW_OAM ].gfxIndex,
                            148 + 32 - 9, 6, 16, 16, status_pkmnPal, status_pkmnTiles,
                            status_pkmnTilesLen, false,
                            false, false, OBJPRIORITY_1, false, OBJMODE_NORMAL );

            IO::updateOAM( true );
            IO::updateOAM( false );

            if( !_needsInit ) {
                IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
                REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
                REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
                REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
                REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
            }
            dmaCopy( partybg2Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            bgUpdate( );
            return;
        }

        dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        char buffer[ 50 ];

        switch( _currentPage ) {
        case 0: {
            // TOP

            // Print Basic Pkmn info
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = false; }
            oam[ SPR_WINDOW_START_OAM + 5 ].isHidden = true;
            oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden = true;
            oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden = false;
//            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = false;

            IO::loadSprite( SPR_WINDOW_OAM, SPR_ARROW_X_PAL, oam[ SPR_WINDOW_OAM ].gfxIndex,
                            148 + 32 - 9, 6, 16, 16, status_pkmnPal, status_pkmnTiles,
                            status_pkmnTilesLen, false,
                            false, false, OBJPRIORITY_1, false, OBJMODE_NORMAL );

            // Dex Nr
            snprintf( buffer, 49, "%03hu", p_pokemon->getSpecies( ) );
            writeLineTop( GET_STRING( 337 ), buffer, 0, IO::WHITE_IDX,
                          p_pokemon->isShiny( ) ? IO::RED2_IDX : IO::BLACK_IDX );
            // Species Name
            writeLineTop( GET_STRING( 338 ), getDisplayName( p_pokemon->getSpecies( ) ).c_str( ),
                          1 );
            // Type
            writeLineTop( GET_STRING( 29 ), 0, 2, IO::WHITE_IDX, IO::BLACK_IDX );
            oam[ SPR_TYPE_OAM( 1 ) ].isHidden = false;
            if( _data.m_baseForme.m_types[ 0 ] != _data.m_baseForme.m_types[ 1 ] ) {
                oam[ SPR_TYPE_OAM( 0 ) ].isHidden = false;
            }
            // OT
            writeLineTop( GET_STRING( 339 ), p_pokemon->m_boxdata.m_oT, 3 );
            // Id
            snprintf( buffer, 49, "%05hu", p_pokemon->m_boxdata.m_oTId );
            writeLineTop( GET_STRING( 340 ), buffer, 4 );
            // Exp
            writeLineTop( GET_STRING( 341 ),
                          std::to_string( p_pokemon->m_boxdata.m_experienceGained ).c_str( ), 6 );
            if( p_pokemon->m_level < 100 ) {
                // Exp to next level
                writeLineTop( GET_STRING( 342 ), 0, 7, IO::WHITE_IDX );
                IO::regularFont->printStringC( GET_STRING( 343 ), INFO_X, INFO_Y + 6 + 15 * 8,
                                               false );
                oam[ SPR_EXP_BAR_OAM ].isHidden     = false;
                oam[ SPR_EXP_BAR_OAM + 1 ].isHidden = false;
                oam[ SPR_EXP_BAR_OAM + 2 ].isHidden = false;
                u16 exptype                         = _data.getExpType( );
                u8  barWidth                        = ( 45 + 27 )
                              * ( p_pokemon->m_boxdata.m_experienceGained
                                  - EXP[ p_pokemon->m_level - 1 ][ exptype ] )
                              / ( EXP[ p_pokemon->m_level ][ exptype ]
                                  - EXP[ p_pokemon->m_level - 1 ][ exptype ] );
                u16 togo = EXP[ p_pokemon->m_level ][ exptype ]
                           - p_pokemon->m_boxdata.m_experienceGained;

                IO::printRectangle( oam[ SPR_EXP_BAR_OAM ].x + 1, oam[ SPR_EXP_BAR_OAM ].y + 1,
                                    oam[ SPR_EXP_BAR_OAM ].x + 1 + barWidth,
                                    oam[ SPR_EXP_BAR_OAM ].y + 2, false, 238 );
                IO::printRectangle( oam[ SPR_EXP_BAR_OAM ].x + 1, oam[ SPR_EXP_BAR_OAM ].y + 3,
                                    oam[ SPR_EXP_BAR_OAM ].x + 1 + barWidth,
                                    oam[ SPR_EXP_BAR_OAM ].y + 3, false, 239 );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->printString( std::to_string( togo ).c_str( ), INFO_X + 128 - 7,
                                              INFO_Y + 4 + 15 * 8, false, IO::font::RIGHT );
            }

            // BOTTOM
            for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = true; }
            for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = false; }

            // Nature
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->printStringC( GET_STRING( 364 ), INFO_X_SUB + 12, INFO_LINE_SUB( 0 ),
                                           true );
            IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
            IO::regularFont->setColor( IO::BLUE_IDX, 2 );
            IO::regularFont->printStringC( GET_STRING( 187 + u8( p_pokemon->getNature( ) ) ),
                                           INFO_X_SUB + 12
                                               + IO::regularFont->stringWidthC( GET_STRING( 364 ) ),
                                           INFO_LINE_SUB( 0 ), true );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->printStringC( GET_STRING( 365 ),
                                           INFO_X_SUB + 12
                                               + IO::regularFont->stringWidthC( GET_STRING(
                                                     187 + u8( p_pokemon->getNature( ) ) ) )
                                               + IO::regularFont->stringWidthC( GET_STRING( 364 ) ),
                                           INFO_LINE_SUB( 0 ), true );

            u8 currentLine = 1;
            if( p_pokemon->wasEgg( ) ) {
                // Got date
                IO::regularFont->printStringC(
                    IO::formatDate( p_pokemon->m_boxdata.m_gotDate ).c_str( ), INFO_X_SUB + 12,
                    INFO_LINE_SUB( currentLine++ ), true );

                // Got location
                auto loc = FS::getLocation( p_pokemon->m_boxdata.m_gotPlace );
                IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
                IO::regularFont->setColor( IO::BLUE_IDX, 2 );
                IO::regularFont->printStringC( loc.c_str( ), INFO_X_SUB + 12,
                                               INFO_LINE_SUB( currentLine++ ), true );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
                IO::regularFont->printStringC( GET_STRING( 366 ), INFO_X_SUB + 12,
                                               INFO_LINE_SUB( currentLine++ ), true );
            }
            // Caught date
            IO::regularFont->printStringC(
                IO::formatDate( p_pokemon->m_boxdata.m_hatchDate ).c_str( ), INFO_X_SUB + 12,
                INFO_LINE_SUB( currentLine++ ), true );
            IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
            IO::regularFont->setColor( IO::BLUE_IDX, 2 );
            auto loc = FS::getLocation( p_pokemon->m_boxdata.m_hatchPlace );
            IO::regularFont->printStringC( loc.c_str( ), INFO_X_SUB + 12,
                                           INFO_LINE_SUB( currentLine++ ), true );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );

            // Met at lv / hatched
            if( p_pokemon->wasEgg( ) && p_pokemon->isForeign( ) ) {
                IO::regularFont->printStringC( GET_STRING( 369 ), INFO_X_SUB + 12,
                                               INFO_LINE_SUB( currentLine++ ), true );

                if( p_pokemon->m_boxdata.m_fateful ) {
                    IO::regularFont->printStringC( GET_STRING( 367 ), INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                }
            } else if( p_pokemon->wasEgg( ) ) {
                IO::regularFont->printStringC( GET_STRING( 368 ), INFO_X_SUB + 12,
                                               INFO_LINE_SUB( currentLine++ ), true );
                if( p_pokemon->m_boxdata.m_fateful ) {
                    IO::regularFont->printStringC( GET_STRING( 367 ), INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                }
            } else if( p_pokemon->isForeign( ) ) {
                if( p_pokemon->m_boxdata.m_fateful ) {
                    snprintf( buffer, 49, GET_STRING( 373 ), p_pokemon->m_level );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                } else {
                    snprintf( buffer, 49, GET_STRING( 371 ), p_pokemon->m_level );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                }
            } else {
                if( p_pokemon->m_boxdata.m_fateful ) {
                    snprintf( buffer, 49, GET_STRING( 372 ), p_pokemon->m_level );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                } else {
                    snprintf( buffer, 49, GET_STRING( 370 ), p_pokemon->m_level );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                }
            }

            // personality
            IO::regularFont->printStringC(
                GET_STRING( 212 + p_pokemon->m_boxdata.getPersonality( ) ), INFO_X_SUB + 12,
                INFO_LINE_SUB( currentLine++ ), true );

            // food
            if( currentLine < 9 ) {
                IO::regularFont->printStringC( GET_STRING( 374 ), INFO_X_SUB + 12,
                                               INFO_LINE_SUB( currentLine ), true );

                snprintf( buffer, 49, GET_STRING( 375 ),
                          GET_STRING( 242 + p_pokemon->m_boxdata.getTasteStr( ) ) );
                IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
                IO::regularFont->setColor( IO::BLUE_IDX, 2 );
                IO::regularFont->printStringC(
                    buffer, INFO_X_SUB + 12 + IO::regularFont->stringWidthC( GET_STRING( 374 ) ),
                    INFO_LINE_SUB( currentLine ), true );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
                IO::regularFont->printStringC(
                    GET_STRING( 376 ),
                    INFO_X_SUB + 12 + IO::regularFont->stringWidthC( GET_STRING( 374 ) )
                        + IO::regularFont->stringWidthC( buffer ),
                    INFO_LINE_SUB( currentLine ), true );
            }

            break;
        }
        case 1: {
            // TOP

            // Draw Pokemon stats
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = false; }
            oam[ SPR_WINDOW_START_OAM + 5 ].isHidden   = true;
            oam[ SPR_HP_BAR_OAM ].isHidden             = false;
            oam[ SPR_HP_BAR_OAM + 1 ].isHidden         = false;
            oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden = false;
            oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden = true;
//            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = false;

            IO::loadSprite( SPR_WINDOW_OAM, SPR_ARROW_X_PAL, oam[ SPR_WINDOW_OAM ].gfxIndex,
                            148 + 32 - 9, 6, 16, 16, 0, status_movesTiles,
                            status_movesTilesLen, false,
                            false, false, OBJPRIORITY_1, false, OBJMODE_NORMAL );

            // HP
            u8 barWidth = ( 45 + 14 ) * p_pokemon->m_stats.m_curHP / p_pokemon->m_stats.m_maxHP;
            if( p_pokemon->m_stats.m_curHP * 2 >= p_pokemon->m_stats.m_maxHP ) {
                IO::smallFont->setColor( 240, 2 );
                IO::smallFont->setColor( 241, 1 );
                IO::smallFont->setColor( 242, 3 );
                IO::printRectangle( oam[ SPR_HP_BAR_OAM ].x + 1, oam[ SPR_HP_BAR_OAM ].y + 1,
                                    oam[ SPR_HP_BAR_OAM ].x + 1 + barWidth,
                                    oam[ SPR_HP_BAR_OAM ].y + 2, false, 241 );
                IO::printRectangle( oam[ SPR_HP_BAR_OAM ].x + 1, oam[ SPR_HP_BAR_OAM ].y + 3,
                                    oam[ SPR_HP_BAR_OAM ].x + 1 + barWidth,
                                    oam[ SPR_HP_BAR_OAM ].y + 3, false, 242 );
            } else if( p_pokemon->m_stats.m_curHP * 4 >= p_pokemon->m_stats.m_maxHP ) {
                IO::smallFont->setColor( 240, 2 );
                IO::smallFont->setColor( 243, 1 );
                IO::smallFont->setColor( 244, 3 );
                IO::printRectangle( oam[ SPR_HP_BAR_OAM ].x + 1, oam[ SPR_HP_BAR_OAM ].y + 1,
                                    oam[ SPR_HP_BAR_OAM ].x + 1 + barWidth,
                                    oam[ SPR_HP_BAR_OAM ].y + 2, false, 243 );
                IO::printRectangle( oam[ SPR_HP_BAR_OAM ].x + 1, oam[ SPR_HP_BAR_OAM ].y + 3,
                                    oam[ SPR_HP_BAR_OAM ].x + 1 + barWidth,
                                    oam[ SPR_HP_BAR_OAM ].y + 3, false, 244 );
            } else {
                IO::smallFont->setColor( 240, 2 );
                IO::smallFont->setColor( 245, 1 );
                IO::smallFont->setColor( 246, 3 );
                if( p_pokemon->m_stats.m_curHP ) {
                    IO::printRectangle( oam[ SPR_HP_BAR_OAM ].x + 1, oam[ SPR_HP_BAR_OAM ].y + 1,
                                        oam[ SPR_HP_BAR_OAM ].x + 1 + barWidth,
                                        oam[ SPR_HP_BAR_OAM ].y + 2, false, 245 );
                    IO::printRectangle( oam[ SPR_HP_BAR_OAM ].x + 1, oam[ SPR_HP_BAR_OAM ].y + 3,
                                        oam[ SPR_HP_BAR_OAM ].x + 1 + barWidth,
                                        oam[ SPR_HP_BAR_OAM ].y + 3, false, 246 );
                }
            }

            IO::smallFont->printString( GET_STRING( 186 ), oam[ SPR_HP_BAR_OAM ].x - 14,
                                        oam[ SPR_HP_BAR_OAM ].y - 10,
                                        false ); // HP "icon"
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( IO::GRAY_IDX, 2 );

            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->printString( GET_STRING( 126 ), INFO_X - 7, INFO_Y + 3, false );

            snprintf( buffer, 49, "%hu/%hu", p_pokemon->IVget( 0 ), p_pokemon->EVget( 0 ) );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( buffer, INFO_X - 8 + 128, INFO_Y + 4, false,
                                           IO::font::RIGHT );

            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            snprintf( buffer, 49, "%hu/%hu", p_pokemon->m_stats.m_curHP,
                      p_pokemon->m_stats.m_maxHP );
            IO::regularFont->printStringC( buffer, INFO_X - 7 + 72, INFO_Y + 3, false,
                                           IO::font::CENTER );

            u16 evtotal = p_pokemon->EVget( 0 );
            for( u8 i = 0; i < 5; ++i ) {
                snprintf( buffer, 49, "%hu/%hu", p_pokemon->IVget( i + 1 ),
                          p_pokemon->EVget( i + 1 ) );
                evtotal += p_pokemon->EVget( i + 1 );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
                IO::regularFont->printStringC( buffer, INFO_X - 7 + 128,
                                               INFO_Y + 12 + 15 * ( i + 1 ), false,
                                               IO::font::RIGHT );

                if( NatMod[ u8( p_pokemon->getNature( ) ) ][ i ] == 9 ) {
                    IO::regularFont->setColor( IO::BLUE_IDX, 2 );
                } else if( NatMod[ u8( p_pokemon->getNature( ) ) ][ i ] == 11 ) {
                    IO::regularFont->setColor( IO::RED_IDX, 2 );
                } else {
                    IO::regularFont->setColor( IO::COLOR_IDX, 2 );
                }
                writeLineTop( GET_STRING( 127 + i ), "", i + 1 );

                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                snprintf( buffer, 49, "%hu", p_pokemon->getStat( i + 1 ) );
                IO::regularFont->printString( buffer, INFO_X - 7 + 68, INFO_Y + 11 + 15 * ( i + 1 ),
                                              false, IO::font::CENTER );
            }

            // EV total / Happiness
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->setColor( IO::BLACK_IDX, 4 );
            IO::regularFont->setColor( IO::COLOR_IDX, 3 );

            snprintf( buffer, 49, "\x01 %hu \x02 %hu", p_pokemon->m_boxdata.m_steps,
                    evtotal );

            IO::regularFont->printStringC( buffer, INFO_X - 7 + 128,
                    INFO_Y + 12 + 15 * 7, false,
                    IO::font::RIGHT );


            // BOTTOM

            for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = false; }
            for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true; }

            // Ability
            IO::regularFont->setColor( IO::COLOR_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( GET_STRING( 363 ), INFO_X_SUB + 190 - 8,
                    INFO_Y_SUB, true, IO::font::RIGHT );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            auto aname = getAbilityName( p_pokemon->getAbility( ) );
            IO::regularFont->printStringC( aname.c_str( ), INFO_X_SUB - 5, INFO_Y_SUB + 3, true );

            // Ability description

            IO::regularFont->printBreakingStringC(
                    getAbilityDescr( p_pokemon->getAbility( ) ).c_str( ),
                    INFO_X_SUB, INFO_Y_SUB + 18, 188, true, IO::font::LEFT, 13 );


            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::smallFont->setColor( IO::COLOR_IDX, 1 );
            IO::smallFont->setColor( 0, 2 );
            // Moves
            for( int i = 0; i < 4; i++ ) {
                if( !p_pokemon->m_boxdata.m_moves[ i ] ) continue;

                auto mdata = MOVE::getMoveData( p_pokemon->m_boxdata.m_moves[ i ] );
                oamSub[ SPR_TYPE_OAM_SUB( i ) ].isHidden = false;
                for( u8 j = 0; j < 6; j++ ) {
                    oamSub[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = false;
                }

                auto mname = MOVE::getMoveName( p_pokemon->m_boxdata.m_moves[ i ] );
                if( mname.length( ) > 18 ) {
                    snprintf( buffer, 20, "%s.", mname.c_str( ) );
                } else {
                    snprintf( buffer, 20, "%s", mname.c_str( ) );
                }

                IO::regularFont->printStringC( buffer, oamSub[ SPR_MOVE_OAM_SUB( i ) ].x + 48,
                                               oamSub[ SPR_MOVE_OAM_SUB( i ) ].y + 7, true,
                                               IO::font::CENTER );

                snprintf( buffer, 49, GET_STRING( 377 ), p_pokemon->m_boxdata.m_curPP[ i ],
                          s8( mdata.m_pp * ( ( 5 + p_pokemon->m_boxdata.PPupget( i ) ) / 5.0 ) ) );
                IO::smallFont->printStringC( buffer, oamSub[ SPR_MOVE_OAM_SUB( i ) ].x + 91,
                                             oamSub[ SPR_MOVE_OAM_SUB( i ) ].y + 13, true,
                                             IO::font::RIGHT );
            }
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );

            break;
        }
        case 2: {
            // TOP

            IO::loadSprite( SPR_WINDOW_OAM, SPR_ARROW_X_PAL, oam[ SPR_WINDOW_OAM ].gfxIndex,
                            148 + 32 - 9, 6, 16, 16, 0, status_contestTiles,
                            status_contestTilesLen, false,
                            false, false, OBJPRIORITY_1, false, OBJMODE_NORMAL );


            // BOTTOM

            oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden = false;
            oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden = false;
//            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = true;
            for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = true; }
            for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true; }
        }
        default:
            break;
        }
        IO::updateOAM( true );
        IO::updateOAM( false );
        dmaCopy( partybg2Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
        if( !_needsInit ) {
            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
        }
        bgUpdate( );
    }

    void statusScreenUI::animate( u8 p_frame ) {
        IO::animateBG( p_frame, IO::bg3 );
        IO::animateBG( p_frame, IO::bg3sub );
        bgUpdate( );
    }
} // namespace STS

//
//    void drawPkmnRibbons( const pokemon& p_pokemon, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, ContestPal, ContestTiles, ContestTilesLen, false, false,
//                        false, OBJPRIORITY_0, p_bottom );
//        IO::regularFont->printString( "Bänder", 32, 0, p_bottom );
//
//        auto rbs = ribbon::getRibbons( currPkmn );
//        // Just draw the first 12 ribbons at max
//        u16 tileCnt = Oam->oamBuffer[ TYPE_IDX ].gfxIndex;
//        for( u8 i = 0; i < std::min( rbs.size( ), 12u ); ++i ) {
//            u16 tmp = IO::loadRibbonIcon( rbs[ i ], 128 + 32 * ( i % 4 ), 36 + 40 * ( i / 4 ),
//                                          RIBBON_IDX + i, TYPE_PAL( i ) % 16, tileCnt,
//                                          p_bottom );
//            if( !tmp ) // Draw an egg when something goes wrong
//                tileCnt = IO::loadEggIcon( 128 + 32 * ( i % 4 ), 32 + 40 * ( i / 4 ),
//                                           RIBBON_IDX + i, TYPE_PAL( i ) % 16, tileCnt,
//                                           p_bottom );
//            else
//                tileCnt = tmp;
//        }
//        if( rbs.empty( ) ) {
//            IO::regularFont->printString( "Keine Bänder", 148, 83, p_bottom );
//        } else {
//            char buffer[ 49 ];
//            snprintf( buffer, 49, "(%u)", rbs.size( ) );
//            IO::regularFont->printString( buffer, 88, 0, p_bottom );
//        }
//    }
//
//    // Draw extra information about the specified move
//    bool statusScreenUI::drawMove( const pokemon& p_pokemon, u8 p_moveIdx, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal      = BG_PAL( p_bottom );
//
//        if( !p_pokemon->m_boxdata.m_moves[ p_moveIdx ] ) return false;
//        if( p_pokemon->isEgg( ) ) return false;
//
//        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
//        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
//
//        pal[ IO::WHITE_IDX ] = IO::WHITE;
//        pal[ IO::GRAY_IDX ]  = RGB( 20, 20, 20 );
//        pal[ IO::BLACK_IDX ] = IO::BLACK;
//        IO::regularFont->setColor( 0, 0 );
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        IO::boldFont->setColor( 0, 0 );
//        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
//        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
//
//        u8 isNotEgg = 1;
//        drawPkmnInformation( currPkmn, isNotEgg, false, p_bottom );
//        if( !isNotEgg ) return false; // This should never occur
//
//        IO::regularFont->printString( "Attackeninfos", 32, 0, p_bottom );
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false,
//                        OBJPRIORITY_0, p_bottom );
//
//        auto mdata = MOVE::getMoveData( p_pokemon->m_boxdata.m_moves[ p_moveIdx ] );
//
//        pal[ IO::COLOR_IDX ] = IO::GREEN;
//        if( mdata.m_type == _data.m_baseForme.m_types[ 0 ]
//            || mdata.m_type == _data.m_baseForme.m_types[ 1 ] ) {
//            IO::regularFont->setColor( IO::COLOR_IDX, 1 );
//            IO::regularFont->setColor( IO::WHITE_IDX, 2 );
//        } else {
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        }
//        IO::regularFont->printString( MOVE::getMoveName( p_pokemon->m_boxdata.m_moves[ p_moveIdx
//        ],
//                    CURRENT_LANGUAGE ).c_str( ), 120, 32, p_bottom );
//        IO::regularFont->setColor( IO::GRAY_IDX, 1 );
//        IO::regularFont->setColor( IO::WHITE_IDX, 2 );
//
//        IO::loadTypeIcon( mdata.m_type, 222, 30, TYPE_IDX + p_moveIdx,
//                          TYPE_PAL( p_moveIdx ), Oam->oamBuffer[ TYPE_IDX + p_moveIdx
//                          ].gfxIndex, p_bottom, SAVE::SAV.getActiveFile(
//                          ).m_options.m_language );
//        IO::loadDamageCategoryIcon(
//            mdata.m_category, 222, 46, ATK_DMGTYPE_IDX( mdata.m_category ),
//            DMG_TYPE_PAL( mdata.m_category ),
//            Oam->oamBuffer[ ATK_DMGTYPE_IDX( mdata.m_category ) ].gfxIndex, p_bottom );
//        char buffer[ 20 ];
//
//        snprintf(
//            buffer, 19,
//            "AP %2hhu"
//            "/"
//            "%2hhu ",
//            p_pokemon->m_boxdata.m_curPP[ p_moveIdx ], mdata.m_pp
//            * ( ( 5 + ( ( p_pokemon->m_boxdata.m_pPUps >> ( 2 * p_moveIdx ) ) % 4 ) ) / 5 ) );
//        IO::regularFont->printString( buffer, 128, 47, p_bottom );
//
//        IO::regularFont->printString( "Stärke", 128, 60, p_bottom );
//        if( mdata.m_basePower )
//            snprintf( buffer, 19, "%3i", mdata.m_basePower );
//        else
//            snprintf( buffer, 19, "---" );
//        IO::regularFont->printString( buffer, 226, 60, p_bottom );
//
//        IO::regularFont->printString( "Genauigkeit", 128, 72, p_bottom );
//        if( mdata.m_accuracy )
//            snprintf( buffer, 19, "%3i", mdata.m_accuracy );
//        else
//            snprintf( buffer, 19, "---" );
//        IO::regularFont->printString( buffer, 226, 72, p_bottom );
//
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
////        IO::regularFont->printString(
////            FS::breakString( currMove->description( ), IO::regularFont, 120 ).c_str( ), 128,
/// 84, /            p_bottom, IO::font::LEFT, 11 );
//
//        IO::updateOAM( p_bottom );
//        return true;
//    }
//
//    // Draw extra information about the specified ribbon
//    bool statusScreenUI::drawRibbon( const pokemon& p_pokemon, u8 p_ribbonIdx, bool p_bottom )
//    {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal      = BG_PAL( p_bottom );
//
//        if( p_pokemon->isEgg( ) ) return false;
//
//        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
//        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
//        for( u8 i = 0; i < 12; ++i ) Oam->oamBuffer[ RIBBON_IDX + i ].isHidden = true;
//
//        pal[ IO::WHITE_IDX ] = IO::WHITE;
//        pal[ IO::GRAY_IDX ]  = RGB( 20, 20, 20 );
//        pal[ IO::BLACK_IDX ] = IO::BLACK;
//        IO::regularFont->setColor( 0, 0 );
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        IO::boldFont->setColor( 0, 0 );
//        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
//        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
//
//        u8 isNotEgg = 1;
//        drawPkmnInformation( currPkmn, isNotEgg, false, p_bottom );
//        if( isNotEgg == (u8) -1 ) return false;
//
//        IO::regularFont->printString( "Bandinfos", 32, 4, p_bottom );
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false,
//                        OBJPRIORITY_0, p_bottom );
//
//        ribbon currRb = RibbonList[ p_ribbonIdx ];
//
//        u16 tileCnt = Oam->oamBuffer[ TYPE_IDX ].gfxIndex;
//
//        if( !IO::loadRibbonIcon( p_ribbonIdx, 226, 28, RIBBON_IDX, TYPE_PAL( 0 ), tileCnt,
//                                 p_bottom ) )
//            tileCnt = IO::loadEggIcon( 226, 28, RIBBON_IDX, TYPE_PAL( 0 ), tileCnt, p_bottom
//            );
//
//        u8   nlCnt = 0;
//        auto nStr  = FS::breakString( currRb.m_name == "" ? ( "----" ) : currRb.m_name,
//                                     IO::regularFont, 110 );
//        for( auto c : nStr )
//            if( c == '\n' ) nlCnt++;
//
//        if( currRb.m_name == "" ) currRb.m_description = "----";
//
//        IO::regularFont->printString( nStr.c_str( ), 226, 39 - 7 * nlCnt, p_bottom,
//        IO::font::RIGHT,
//                                      14 );
//        IO::regularFont->printString(
//            FS::breakString( ( currRb.m_description == "" )
//                                 ? ( p_pokemon->m_boxdata.m_fateful
//                                         ? "Ein Band für Pokémon-Freunde."
//                                         : "Ein Gedenk-Band. An einem mysteriösen Ort
//                                         erhalten." )
//                                 : currRb.m_description,
//                             IO::regularFont, 120 )
//                .c_str( ),
//            250, 66, p_bottom, IO::font::RIGHT, 14 );
//        IO::updateOAM( p_bottom );
//        return true;
//    }
