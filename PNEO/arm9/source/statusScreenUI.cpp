/*
Pokémon neo
------------------------------

file        : statusScreenUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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

#include "sts/statusScreenUI.h"
#include "battle/ability.h"
#include "defines.h"
#include "fs/data.h"
#include "fs/fs.h"
#include "gen/locationNames.h"
#include "gen/pokemonNames.h"
#include "io/font.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "sound/sound.h"

#include "NoItem.h"

namespace STS {
    // top screen sprites
#define SPR_WINDOW_START_OAM      0
#define SPR_HP_BAR_OAM            10
#define SPR_PKMN_START_OAM        13
#define SPR_TYPE_OAM( p_type )    ( 22 + ( p_type ) )
#define SPR_BALL_ICON_OAM         24
#define SPR_EXP_BAR_OAM           25
#define SPR_CHOICE_START_OAM      28
#define SPR_STATUS_ICON_OAM       36
#define SPR_SHINY_ICON_OAM        37
#define SPR_INFOPAGE_START_OAM    38
#define SPR_PKMN_SHADOW_START_OAM 48
#define SPR_PAGE_OAM              52
#define SPR_WINDOW_OAM            53
#define SPR_CTYPE_OAM( p_type )   ( 120 + ( p_type ) )

#define SPR_INFOPAGE_PAL        0
#define SPR_PKMN_PAL            1
#define SPR_WINDOW_PAL          2
#define SPR_BALL_ICON_PAL       3
#define SPR_EXP_BAR_PAL         4
#define SPR_BOX_PAL             5
#define SPR_STATUS_ICON_PAL     6
#define SPR_SHINY_ICON_PAL      7
#define SPR_PKMN_SHADOW_PAL     8
#define SPR_PAGE_PAL            9
#define SPR_ARROW_X_PAL         10
#define SPR_TYPE_PAL( p_type )  ( 11 + ( p_type ) )
#define SPR_CTYPE_PAL( p_type ) ( 11 + ( p_type ) )

    // Sub screen sprites
#define SPR_INFOPAGE_START_OAM_SUB        0
#define SPR_ARROW_UP_OAM_SUB              18
#define SPR_ARROW_DOWN_OAM_SUB            19
#define SPR_X_OAM_SUB                     20
#define SPR_PAGE_LEFT_OAM_SUB             21
#define SPR_PAGE_OAM_SUB( p_page )        ( 22 + ( p_page ) )
#define SPR_NAVIGATION_OAM_SUB( p_page )  ( 25 + ( p_page ) )
#define SPR_TYPE_OAM_SUB( p_idx )         ( 30 + ( p_idx ) )
#define SPR_MOVE_OAM_SUB( p_move )        ( 34 + 6 * ( p_move ) )
#define SPR_ABILITY_OAM_SUB               60
#define SPR_ARROW_BACK_OAM_SUB            70
#define SPR_WINDOW_ICON_OAM_SUB( p_page ) ( 71 + ( p_page ) )
#define SPR_WINDOW_PKMN_OAM_SUB           71
#define SPR_WINDOW_MOVE_OAM_SUB           72
#define SPR_WINDOW_CONTEST_OAM_SUB        73
#define SPR_MOVE_DETAILS_OAM_SUB          75
#define SPR_CATEGORY_OAM_SUB( p_idx )     ( 100 + ( p_idx ) )
#define SPR_RIBBON_OAM_SUB( p_idx )       ( 115 + ( p_idx ) )

#define SPR_INFOPAGE_PAL_SUB          0
#define SPR_ARROW_X_PAL_SUB           1
#define SPR_BOX_PAL_SUB               2
#define SPR_WINDOW_PAL_SUB            3
#define SPR_ABILITY_PAL_SUB           4
#define SPR_TYPE_PAL_SUB( p_idx )     ( 5 + ( p_idx ) )
#define SPR_CATEGORY_PAL_SUB( p_idx ) ( 10 + ( p_idx ) )
#define SPR_RIBBON_PAL_SUB( p_idx )   ( 4 + ( p_idx ) )

    constexpr s16 CONTEST_TYPE_POS_X[ 5 ] = { 0, 95, 58, -58, -95 };
    constexpr s16 CONTEST_TYPE_POS_Y[ 5 ] = { -90, -20, 90, 90, -20 };

    // Palettes hardcoded for speed
    const u16 STAT_BG_PAL[ 14 ]
        = { 0x0000, 0x1062, 0x1483, 0x2107, 0x14A5, 0x1CE8, 0x3DEF, 0x5294, 0x1062 };
    const u16 STAT_BG_PAL_SUB[ 14 ] = {
        0x0000, 0x1062, 0x1483, 0x2107, 0x2107, 0x2107, 0x14A5, 0x1CE8, 0x3DEF, 0x1062, 0x5294,
    };

    const u16 ARR_X_SPR_PAL[ 16 ] = {
        0x7FFF, 0x5A6E, 0x6F2D, 0x564A, // arrow_up
        0x001F, 0x0011, 0x18CE          // x_16_16
    };
    const u16 WINDOW_SPR_PAL[ 16 ] = {
        0x7FFF, 0x5A6E, 0x6F2D, 0x5208, // nosel 32 32
        0x7F5F, 0x4214, 0x4A75,         // nosel faint 32 32
        0x2506,                         // nosel blank 32 32
        0x4DC2, 0x5A28, 0x6F2A, 0x7FF2, // sel 32 32
        0x573C, 0x3A32, 0x4254, 0x77BF  // sel faint 32 32
    };

    const u16 MOVEBOX1_SPR_PAL[ 6 ] = { 0x18C6, 0x294A };
    const u16 MOVEBOX3_SPR_PAL[ 6 ] = { 0x5208, 0x294A };

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
                        0, 0, 64 * 64 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 8, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 64,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, 64 * 64 / 2, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/pg1", SPR_INFOPAGE_START_OAM + 7, SPR_INFOPAGE_PAL, tileCnt,
                                  INFO_X + 64, INFO_Y + 64 + 15, 64, 64, true, true, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 1, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 64, INFO_Y,
                        64, 64, 0, 0, 64 * 64 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 9, SPR_INFOPAGE_PAL, tileCnt, INFO_X,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, 64 * 64 / 2, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/pg2", SPR_INFOPAGE_START_OAM + 6, SPR_INFOPAGE_PAL, tileCnt,
                                  INFO_X, INFO_Y + 64 + 15, 64, 64, true, true, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Pkmn Sprite
        if( !p_pokemon->isEgg( ) ) {
            tileCnt = IO::loadPKMNSprite( p_pokemon->getSpriteInfo( ), 0, 54, SPR_PKMN_START_OAM,
                                          SPR_PKMN_PAL, tileCnt, p_bottom );
        } else {
            tileCnt = IO::loadEggSprite( 0, 54, SPR_PKMN_START_OAM, SPR_PKMN_PAL, tileCnt, p_bottom,
                                         p_pokemon->getSpecies( ) == PKMN_MANAPHY );
        }
        u16 emptyPal[ 32 ]                                         = { 0 };
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

        // Pokéball Icon
        tileCnt = IO::loadItemIcon( BAG::ballToItem( p_pokemon->m_boxdata.m_ball ), -4, 28,
                                    SPR_BALL_ICON_OAM, SPR_BALL_ICON_PAL, tileCnt, p_bottom );

        // Name box

        IO::loadSprite( SPR_CHOICE_START_OAM, SPR_BOX_PAL, tileCnt, 0, 28, 16, 32, 0, 0,
                        16 * 32 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 6, SPR_BOX_PAL, tileCnt, 88, 28, 16, 32, 0, 0,
                        16 * 32 / 2, true, true, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "SEL/noselection_96_32_1", SPR_CHOICE_START_OAM + 5, SPR_BOX_PAL,
                                  tileCnt, 74, 28, 16, 32, true, true, false, OBJPRIORITY_3,
                                  p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 1, SPR_BOX_PAL, tileCnt, 16, 28, 16, 32, 0, 0,
                        16 * 32 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 2, SPR_BOX_PAL, tileCnt, 32, 28, 16, 32, 0, 0,
                        16 * 32 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 3, SPR_BOX_PAL, tileCnt, 48, 28, 16, 32, 0, 0,
                        16 * 32 / 2, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 7, SPR_BOX_PAL, tileCnt, 75, 28, 16, 32, 0, 0,
                        16 * 32 / 2, false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "SEL/noselection_96_32_2", SPR_CHOICE_START_OAM + 4, SPR_BOX_PAL,
                                  tileCnt, 64, 28, 16, 32, false, false, false, OBJPRIORITY_3,
                                  p_bottom, OBJMODE_BLENDED );

        // Status
#define STATUS_X 68
#define STATUS_Y 48
        if( !p_pokemon->m_stats.m_curHP ) {
            tileCnt = IO::loadSprite( "STS/status_fnt", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isParalyzed ) {
            tileCnt = IO::loadSprite( "STS/status_par", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isAsleep ) {
            tileCnt = IO::loadSprite( "STS/status_slp", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBadlyPoisoned ) {
            tileCnt = IO::loadSprite( "STS/status_txc", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBurned ) {
            tileCnt = IO::loadSprite( "STS/status_brn", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isFrozen ) {
            tileCnt = IO::loadSprite( "STS/status_frz", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isPoisoned ) {
            tileCnt = IO::loadSprite( "STS/status_psn", SPR_STATUS_ICON_OAM, SPR_STATUS_ICON_PAL,
                                      tileCnt, STATUS_X, STATUS_Y, 8, 8, false, false, false,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        }

        // Shiny
        tileCnt = IO::loadSprite( "STS/status_shiny", SPR_SHINY_ICON_OAM, SPR_SHINY_ICON_PAL,
                                  tileCnt, STATUS_X - 8, STATUS_Y, 8, 8, false, false,
                                  !p_pokemon->isShiny( ) || p_pokemon->isEgg( ), OBJPRIORITY_0,
                                  p_bottom, OBJMODE_NORMAL );

        // Page
        tileCnt
            = IO::loadSprite( "SEL/noselection_64_32", SPR_PAGE_OAM, SPR_PAGE_PAL, tileCnt, 148, -4,
                              64, 32, true, true, false, OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );
        tileCnt = IO::loadSprite( "STS/status_pkmn", SPR_WINDOW_OAM, SPR_ARROW_X_PAL, tileCnt,
                                  148 + 32 - 9, 6, 16, 16, false, false, false, OBJPRIORITY_1,
                                  p_bottom, OBJMODE_NORMAL );

        // Type icon
        for( u8 i = 0; i < 2; ++i ) {
            tileCnt
                = IO::loadTypeIcon( _data.m_baseForme.m_types[ i ], INFO_X - 7 + 128 - 66 + 34 * i,
                                    INFO_Y + 11 + 15 * 2, SPR_TYPE_OAM( i ), SPR_TYPE_PAL( i ),
                                    tileCnt, p_bottom, CURRENT_LANGUAGE );
            oam[ SPR_TYPE_OAM( i ) ].isHidden = true;
        }

        // HP/EXP bar
        IO::loadSprite( SPR_HP_BAR_OAM + 1, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48,
                        INFO_Y + 4 + 15 * 1, 64, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                        p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_HP_BAR_OAM, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48 - 14,
                        INFO_Y + 4 + 15 * 1, 16, 8, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                        p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_EXP_BAR_OAM + 1, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48 - 14,
                        INFO_Y + 15 + 15 * 7, 16, 8, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                        p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_EXP_BAR_OAM, SPR_EXP_BAR_PAL, tileCnt, INFO_X - 7 + 128 - 48 - 27,
                        INFO_Y + 15 + 15 * 7, 16, 8, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                        p_bottom, OBJMODE_NORMAL );
        tileCnt = IO::loadSprite( "BX/hpbar", SPR_EXP_BAR_OAM + 2, SPR_EXP_BAR_PAL, tileCnt,
                                  INFO_X - 7 + 128 - 48, INFO_Y + 15 + 15 * 7, 64, 32, false, false,
                                  true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );

        // Window

        tileCnt = IO::loadSprite( "UI/wn1", SPR_WINDOW_START_OAM, SPR_WINDOW_PAL, tileCnt,
                                  INFO_X - 14, INFO_Y, 64, 32, false, false, false, OBJPRIORITY_3,
                                  p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 1, SPR_WINDOW_PAL, tileCnt, INFO_X - 14, INFO_Y + 32,
                        64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 2, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                        INFO_Y + 32 + 30, 64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/wn2", SPR_WINDOW_START_OAM + 3, SPR_WINDOW_PAL, tileCnt,
                                  INFO_X - 14, INFO_Y + 64 + 28, 64, 32, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 5, SPR_WINDOW_PAL, tileCnt, INFO_X - 14,
                        INFO_Y + 111 - 30, 64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/wn3", SPR_WINDOW_START_OAM + 4, SPR_WINDOW_PAL, tileCnt,
                                  INFO_X - 14, INFO_Y + 111, 64, 32, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        tileCnt = oam[ SPR_TYPE_OAM( 0 ) ].gfxIndex;

#define COND_CENTER_X ( INFO_X + 32 )
#define COND_CENTER_Y ( INFO_Y + 54 )
#define COND_RADIUS   48

        // Contest type icon
        for( u8 i = 0; i < 5; ++i ) {
            u16 x = COND_CENTER_X + COND_RADIUS * CONTEST_TYPE_POS_X[ i ] / 100;
            u16 y = COND_CENTER_Y + COND_RADIUS * CONTEST_TYPE_POS_Y[ i ] / 100;

            tileCnt = IO::loadContestTypeIcon( (BATTLE::contestType) i, x + 16, y + 8,
                                               SPR_CTYPE_OAM( i ), SPR_CTYPE_PAL( i ), tileCnt,
                                               p_bottom, CURRENT_LANGUAGE );
            oam[ SPR_CTYPE_OAM( i ) ].isHidden = true;
        }

        IO::updateOAM( p_bottom );
        return tileCnt;
    }

    u16 statusScreenUI::initBottomScreen( pokemon* p_pokemon, bool p_bottom ) {
        IO::clearScreen( p_bottom, false, true );
        FS::readPictureData( p_bottom ? bgGetGfxPtr( IO::bg2sub ) : bgGetGfxPtr( IO::bg2 ),
                             "nitro:/PICS/", "statussub", 0, 192 * 256, p_bottom );

        IO::initOAMTable( p_bottom );

        SpriteEntry* oam     = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          tileCnt = 0;

        // Arrows
        tileCnt = IO::loadSprite( "UI/arrow_up", SPR_ARROW_DOWN_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                                  tileCnt, 102 + 19, 192 - 21, 16, 16, true, false, !_allowKeyDown,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ARROW_UP_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                        oam[ SPR_ARROW_DOWN_OAM_SUB ].gfxIndex, 102 + 38 + 18, 192 - 16, 16, 16, 0,
                        0, 0, false, false, !_allowKeyUp, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        tileCnt = IO::loadSprite( "UI/backarrow", SPR_ARROW_BACK_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                                  tileCnt, 102 + 56 + 32, 192 - 19, 16, 16, false, false, false,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        // x
        tileCnt = IO::loadSprite( "UI/x_16_16", SPR_X_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 236,
                                  172, 16, 16, false, false, false, OBJPRIORITY_1, p_bottom,
                                  OBJMODE_NORMAL );
        // page windows
        tileCnt = IO::loadSprite( "SEL/noselection_32_64", SPR_PAGE_LEFT_OAM_SUB, SPR_BOX_PAL_SUB,
                                  tileCnt, 0 - 8, 57 - 12, 32, 64, true, true, true, OBJPRIORITY_2,
                                  p_bottom, OBJMODE_NORMAL );
        for( u8 i = 0; i < 3; i++ ) {
            IO::loadSprite( SPR_PAGE_OAM_SUB( i ), SPR_BOX_PAL_SUB,
                            oam[ SPR_PAGE_LEFT_OAM_SUB ].gfxIndex, 256 - 24, 4 + 28 * i, 32, 64, 0,
                            0, 0, false, false, true, OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );
        }

        tileCnt = IO::loadSprite( "STS/status_pkmn", SPR_WINDOW_PKMN_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                                  tileCnt, 256 - 18, 24, 16, 16, false, false, false, OBJPRIORITY_1,
                                  p_bottom, OBJMODE_NORMAL );
        tileCnt = IO::loadSprite( "STS/status_moves", SPR_WINDOW_MOVE_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                                  tileCnt, 256 - 18, 24 + 30, 16, 16, false, false, false,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        tileCnt = IO::loadSprite( "STS/status_contest", SPR_WINDOW_CONTEST_OAM_SUB,
                                  SPR_ARROW_X_PAL_SUB, tileCnt, 256 - 18, 24 + 57, 16, 16, false,
                                  false, false, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        // navigation
        tileCnt = IO::loadSprite( "SEL/noselection_64_32", SPR_NAVIGATION_OAM_SUB( 0 ),
                                  SPR_BOX_PAL_SUB, tileCnt, 0, 0, 64, 32, true, true, true,
                                  OBJPRIORITY_2, p_bottom, OBJMODE_NORMAL );
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
                        INFO_Y_SUB, 64, 64, 0, 0, 64 * 64 / 2, false, false, false, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 7, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 64, INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, 64 * 64 / 2, true,
                        true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 6, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 64, INFO_Y_SUB + 64 + 30, 64, 64, 0, 0, 64 * 64 / 2, true,
                        true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 4, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 96, INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, 64 * 64 / 2, true,
                        true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 3, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 96, INFO_Y_SUB + 64 + 30, 64, 64, 0, 0, 64 * 64 / 2, true,
                        true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 1, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 128, INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, 64 * 64 / 2, true,
                        true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB, SPR_INFOPAGE_PAL_SUB, tileCnt, INFO_X_SUB + 128,
                        INFO_Y_SUB + 64 + 30, 64, 64, 0, 0, 64 * 64 / 2, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/pg1", SPR_INFOPAGE_START_OAM_SUB + 6, SPR_INFOPAGE_PAL_SUB,
                                  tileCnt, INFO_X_SUB + 64, INFO_Y_SUB + 64 + 30, 64, 64, true,
                                  true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 8, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 64, INFO_Y_SUB, 64, 64, 0, 0, 64 * 64 / 2, false, false, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 10, SPR_INFOPAGE_PAL_SUB, tileCnt, INFO_X_SUB,
                        INFO_Y_SUB + 64 - 15, 64, 64, 0, 0, 64 * 64 / 2, true, true, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 5, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 96, INFO_Y_SUB, 64, 64, 0, 0, 64 * 64 / 2, false, false, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM_SUB + 2, SPR_INFOPAGE_PAL_SUB, tileCnt,
                        INFO_X_SUB + 128, INFO_Y_SUB, 64, 64, 0, 0, 64 * 64 / 2, false, false,
                        false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/pg2", SPR_INFOPAGE_START_OAM_SUB + 9, SPR_INFOPAGE_PAL_SUB,
                                  tileCnt, INFO_X_SUB, INFO_Y_SUB + 64 + 30, 64, 64, true, true,
                                  false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Moves
        // move windows
        for( u8 i = 1; i < 3; i++ ) {
            u8 pos = 2 * i - 2;
            if( i == 1 ) {
                tileCnt = IO::loadSprite( "BT/movebox1", SPR_MOVE_OAM_SUB( pos ),
                                          SPR_TYPE_PAL_SUB( pos ), tileCnt, 29, 44 + i * 44, 16, 32,
                                          false, false, !p_pokemon->getMove( pos ), OBJPRIORITY_3,
                                          p_bottom, OBJMODE_NORMAL );
                tileCnt = IO::loadSprite( "BT/movebox2", SPR_MOVE_OAM_SUB( pos ) + 1,
                                          SPR_TYPE_PAL_SUB( pos ), tileCnt, 29 + 16, 44 + i * 44,
                                          16, 32, false, false, !p_pokemon->getMove( pos ),
                                          OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            } else {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 29, 44 + i * 44, 16, 32, 0,
                                0, 0, false, false, !p_pokemon->getMove( pos ), OBJPRIORITY_3,
                                p_bottom, OBJMODE_NORMAL );
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 1, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16, 44 + i * 44, 16,
                                32, 0, 0, 0, false, false, !p_pokemon->getMove( pos ),
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + j, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 16, 44 + i * 44,
                                16, 32, 0, 0, 0, false, false, !p_pokemon->getMove( pos ),
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 5, SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 29 + 5 * 16, 44 + i * 44, 16, 32,
                            0, 0, 0, true, true, !p_pokemon->getMove( pos ), OBJPRIORITY_3,
                            p_bottom, OBJMODE_NORMAL );
        }
        for( u8 i = 1; i < 3; i++ ) {
            u8 pos = 2 * i - 1;
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 131, 44 + i * 44, 16, 32, 0, 0,
                            0, false, false, !p_pokemon->getMove( pos ), OBJPRIORITY_3, p_bottom,
                            OBJMODE_NORMAL );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + j, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 16,
                                44 + i * 44, 16, 32, 0, 0, 0, false, false,
                                !p_pokemon->getMove( pos ), OBJPRIORITY_3, p_bottom,
                                OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 5, SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 131 + 5 * 16, 44 + i * 44, 16,
                            32, 0, 0, 0, true, true, !p_pokemon->getMove( pos ), OBJPRIORITY_3,
                            p_bottom, OBJMODE_NORMAL );
        }

        // type icons
        for( u8 i = 0; i < 4; ++i ) {
            BATTLE::type t;
            if( p_pokemon->getMove( i ) != M_HIDDEN_POWER ) {
                t = _moves[ i ].m_type;
            } else {
                t = p_pokemon->getHPType( );
            }
            tileCnt = IO::loadTypeIcon( t, oam[ SPR_MOVE_OAM_SUB( i ) ].x - 4,
                                        oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_TYPE_OAM_SUB( i ),
                                        SPR_TYPE_PAL_SUB( i ), tileCnt, true, CURRENT_LANGUAGE );

            tileCnt = IO::loadDamageCategoryIcon(
                _moves[ i ].m_category, oam[ SPR_MOVE_OAM_SUB( i ) ].x - 4 + 32,
                oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_CATEGORY_OAM_SUB( i ),
                SPR_CATEGORY_PAL_SUB( i ), tileCnt, true );

            IO::copySpritePal( MOVEBOX1_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, p_bottom );
        }

        // ability window

        tileCnt = IO::loadSprite( "UI/ab3", SPR_ABILITY_OAM_SUB, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 88, INFO_Y_SUB, 32, 64, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_ABILITY_OAM_SUB + 1, SPR_ABILITY_PAL_SUB, tileCnt, INFO_X_SUB - 12,
                        INFO_Y_SUB, 64, 64, 0, 0, 0, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/ab1", SPR_ABILITY_OAM_SUB + 2, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 36, INFO_Y_SUB, 64, 64, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_ABILITY_OAM_SUB + 4, SPR_ABILITY_PAL_SUB, tileCnt, INFO_X_SUB + 64 + 36,
                        INFO_Y_SUB, 64, 64, 0, 0, 0, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "UI/ab2", SPR_ABILITY_OAM_SUB + 3, SPR_ABILITY_PAL_SUB, tileCnt,
                                  INFO_X_SUB + 64 + 68, INFO_Y_SUB, 64, 64, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // move details
        IO::loadSprite( SPR_MOVE_DETAILS_OAM_SUB + 7, SPR_BOX_PAL_SUB, tileCnt, INFO_X_SUB - 12,
                        INFO_Y_SUB - 2, 32, 64, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        tileCnt
            = IO::loadSprite( "SEL/noselection_160_64_1", SPR_MOVE_DETAILS_OAM_SUB + 13,
                              SPR_BOX_PAL_SUB, tileCnt, INFO_X_SUB + 64 + 68 + 32, INFO_Y_SUB - 2,
                              32, 64, true, true, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        for( u8 i = 1; i < 5; ++i ) {
            IO::loadSprite( SPR_MOVE_DETAILS_OAM_SUB + i + 7, SPR_BOX_PAL_SUB, tileCnt,
                            INFO_X_SUB - 12 + 32 * i, INFO_Y_SUB - 2, 32, 64, 0, 0, 0, false, false,
                            true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        }
        tileCnt
            = IO::loadSprite( "SEL/noselection_160_64_2", SPR_MOVE_DETAILS_OAM_SUB + 12,
                              SPR_BOX_PAL_SUB, tileCnt, INFO_X_SUB + 64 + 68, INFO_Y_SUB - 2, 32,
                              64, false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::loadSprite( SPR_MOVE_DETAILS_OAM_SUB, SPR_BOX_PAL_SUB, tileCnt, INFO_X_SUB + 28,
                        INFO_Y_SUB + 56 - 2, 32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        for( u8 i = 0; i < 5; ++i ) {
            IO::loadSprite( SPR_MOVE_DETAILS_OAM_SUB + 6 - i, SPR_BOX_PAL_SUB, tileCnt,
                            INFO_X_SUB + 60 + 24 * i, INFO_Y_SUB + 44 - 2, 32, 32, 0, 0, 0, true,
                            true, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        }
        tileCnt
            = IO::loadSprite( "SEL/noselection_64_20", SPR_MOVE_DETAILS_OAM_SUB + 1,
                              SPR_BOX_PAL_SUB, tileCnt, INFO_X_SUB + 132 + 24, INFO_Y_SUB + 44 - 2,
                              32, 32, true, true, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        IO::copySpritePal( ARR_X_SPR_PAL, SPR_ARROW_X_PAL_SUB, 0, 2 * 7, p_bottom );
        IO::copySpritePal( WINDOW_SPR_PAL, SPR_WINDOW_PAL_SUB, 0, 2 * 16, p_bottom );

        for( u8 r = 0; r < 12; ++r ) {
            tileCnt = IO::loadRibbonIcon( 0, 0, 0, SPR_RIBBON_OAM_SUB( r ), SPR_RIBBON_PAL_SUB( r ),
                                          tileCnt, p_bottom );
            oam[ SPR_RIBBON_OAM_SUB( r ) ].isHidden = true;
        }

        return tileCnt;
    }

    void statusScreenUI::drawBasicInfoTop( pokemon* p_pokemon, bool p_bottom ) {

        FS::readPictureData( p_bottom ? bgGetGfxPtr( IO::bg2sub ) : bgGetGfxPtr( IO::bg2 ),
                             "nitro:/PICS/", "statustop", 0, 192 * 256, p_bottom );

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
                s8 diff = 66 - IO::regularFont->stringWidthC( p_pokemon->m_boxdata.m_name );
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
                IO::regularFont->printString( GET_STRING( IO::STR_UI_STS_ITEM ), 8, 166, p_bottom );
                IO::regularFont->printStringC( FS::getItemName( p_pokemon->getItem( ) ).c_str( ),
                                               16, 178, p_bottom );
            }
        } else {
            oam[ SPR_CHOICE_START_OAM + 6 ].isHidden = false;
            oam[ SPR_CHOICE_START_OAM + 7 ].isHidden = false;

            // Egg
            IO::regularFont->printString( GET_STRING( IO::STR_UI_BAG_PARTY_EGG ), 12, 36,
                                          p_bottom );

            u8 startline = 0;

            IO::regularFont->setColor( IO::BLUE_IDX, 2 );
            writeLineTop( IO::formatDate( p_pokemon->m_boxdata.m_gotDate ).c_str( ), startline++,
                          IO::BLUE2_IDX );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            if( p_pokemon->getSpecies( ) == PKMN_MANAPHY ) {
                writeLineTop( GET_STRING( IO::STR_UI_STS_MANA_EGG_DSCR_LINE1 ), startline++ );
                writeLineTop( GET_STRING( IO::STR_UI_STS_MANA_EGG_DSCR_LINE2 ), startline++ );
                writeLineTop( GET_STRING( IO::STR_UI_STS_MANA_EGG_DSCR_LINE3 ), startline++ );
            } else {
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_DSCR_LINE1 ), startline++ );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_DSCR_LINE2 ), startline++ );
            }
            u8 shift = 2;
            switch( p_pokemon->m_boxdata.m_gotPlace ) {
            case L_POKEMON_DAY_CARE:
            case L_DAY_CARE_COUPLE:
            case L_TRAVELING_MAN:
            case L_POKEWALKER:
                writeLineTop( GET_STRING( IO::STR_UI_STS_FROM ), startline );
                shift += IO::regularFont->stringWidth( GET_STRING( IO::STR_UI_STS_FROM ) );
                break;
            default:
                writeLineTop( GET_STRING( IO::STR_UI_STS_IN_AT ), startline );
                shift += IO::regularFont->stringWidth( GET_STRING( IO::STR_UI_STS_IN_AT ) );
                break;
            case L_RILEY:
            case L_CYNTHIA:
            case L_MR_POKEMON:
            case L_PRIMO:
            case L_POKEMON_RANGER:
                writeLineTop( GET_STRING( IO::STR_UI_STS_FROM_PERSON ), startline );
                shift += IO::regularFont->stringWidthC( GET_STRING( IO::STR_UI_STS_FROM_PERSON ) );
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
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_1_0 ), startline );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_1_1 ), startline + 1 );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_1_2 ), startline + 2 );
            } else if( p_pokemon->m_boxdata.m_steps > 5 ) {
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_2_0 ), startline );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_2_1 ), startline + 1 );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_2_2 ), startline + 2 );
            } else {
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_3_0 ), startline );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_3_1 ), startline + 1 );
                writeLineTop( GET_STRING( IO::STR_UI_STS_EGG_STATUS_3_2 ), startline + 2 );
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
                                               INFO_Y + 11 + 15 * p_line, p_bottom,
                                               IO::font::RIGHT );
            } else {
                IO::regularFont->printString( p_stringRight, INFO_X - 7 + 128,
                                              INFO_Y + 11 + 15 * p_line, p_bottom,
                                              IO::font::RIGHT );
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
        _data = FS::getPkmnData( p_pokemon->getSpecies( ), p_pokemon->getForme( ) );
        for( u8 i = 0; i < 4; ++i ) {
            if( p_pokemon->getMove( i ) ) {
                _moves[ i ] = FS::getMoveData( p_pokemon->getMove( i ) );
            }
        }

        initTopScreen( p_pokemon );
        initBottomScreen( p_pokemon );

        _needsInit = true;
        draw( p_pokemon, p_initialPage );

        bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
        bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
        bgSetScroll( IO::bg3sub, 0, 0 );
        bgSetScroll( IO::bg3, 0, 0 );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );

        dmaCopy( STAT_BG_PAL, BG_PALETTE, 3 * 10 );
        dmaCopy( STAT_BG_PAL_SUB, BG_PALETTE_SUB, 3 * 10 );

        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = IO::BG_PAL( i );
            pal[ IO::COLOR_IDX ] = IO::RGB( 22, 22, 22 );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = IO::RGB( 16, 16, 16 );
            pal[ IO::BLACK_IDX ] = IO::RGB( 8, 8, 8 );
            pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

            pal[ 238 ] = IO::RGB( 13, 25, 25 ); // exp bar blue 1
            pal[ 239 ] = IO::RGB( 6, 18, 18 );  // exp bar blue 2
            pal[ 240 ] = IO::RGB( 6, 6, 6 );    // hp bar border color
            pal[ 241 ] = IO::RGB( 12, 30, 12 ); // hp bar green 1
            pal[ 242 ] = IO::RGB( 3, 23, 4 );   // hp bar green 2
            pal[ 243 ] = IO::RGB( 30, 30, 12 ); // hp bar yellow 1
            pal[ 244 ] = IO::RGB( 23, 23, 5 );  // hp bar yellow 2
            pal[ 245 ] = IO::RGB( 30, 15, 12 ); // hp bar red 1
            pal[ 246 ] = IO::RGB( 20, 7, 7 );   // hp bar red 2
        }

        FS::readData<unsigned int, unsigned short>( "nitro:/PICS/", "partybg2", 256 * 256 / 4, TEMP,
                                                    256, TEMP_PAL );
        dmaCopy( TEMP, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        FS::readData<unsigned int, unsigned short>( "nitro:/PICS/", "partybg", 256 * 256 / 4, TEMP,
                                                    256, TEMP_PAL );
        dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );

        _needsInit       = false;
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
        REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
        bgUpdate( );

        if( !p_pokemon->isEgg( ) ) {
            SOUND::playCry( p_pokemon->getSpecies( ), p_pokemon->getForme( ),
                            p_pokemon->isFemale( ) );
        }
    }

    void statusScreenUI::draw( pokemon* p_pokemon, u8 p_page ) {
        if( !p_pokemon ) { return; }
        if( !_needsInit && ( p_page == _currentPage || p_pokemon->isEgg( ) ) ) { return; }
        _currentPage     = p_pokemon->isEgg( ) ? 0 : p_page;
        REG_BLDALPHA_SUB = 0;
        REG_BLDALPHA     = 0;
        bgUpdate( );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        drawBasicInfoTop( p_pokemon, false );
        IO::regularFont->printString( GET_STRING( IO::STR_UI_STS_PKMN_INFO ), 34, 2, false );
        SpriteEntry* oam                           = IO::OamTop->oamBuffer;
        SpriteEntry* oamSub                        = IO::Oam->oamBuffer;
        oam[ SPR_INFOPAGE_START_OAM + 6 ].isHidden = false;
        oam[ SPR_INFOPAGE_START_OAM + 7 ].isHidden = false;

        for( u8 i = 0; i < getPageCount( ); ++i ) {
            oamSub[ SPR_PAGE_OAM_SUB( i ) ].isHidden = true;
        }
        for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true; }

        for( u8 i = 0; i < 4; ++i ) {
            oamSub[ SPR_TYPE_OAM_SUB( i ) ].isHidden     = true;
            oamSub[ SPR_CATEGORY_OAM_SUB( i ) ].isHidden = true;
            for( u8 j = 0; j < 6; j++ ) { oamSub[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = true; }
        }

        oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden    = true;
        oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden    = true;
        oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = true;
        for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = true; }
        for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true; }
        for( u8 i = 0; i < 12; ++i ) { oamSub[ SPR_RIBBON_OAM_SUB( i ) ].isHidden = true; }
        for( u8 i = 0; i < getPageCount( ); ++i ) {
            oamSub[ SPR_PAGE_OAM_SUB( i ) ].isHidden = ( i == _currentPage ) || p_pokemon->isEgg( );
        }

        if( p_pokemon->isEgg( ) ) {
            IO::loadSprite( "STS/status_pkmn", SPR_WINDOW_OAM, SPR_ARROW_X_PAL,
                            oam[ SPR_WINDOW_OAM ].gfxIndex, 148 + 32 - 9, 6, 16, 16, false, false,
                            false, OBJPRIORITY_1, false, OBJMODE_NORMAL );

            IO::updateOAM( true );
            IO::updateOAM( false );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/", "partybg2", 256 * 256 / 4,
                                                        TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/", "partybg", 256 * 256 / 4,
                                                        TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );

            if( !_needsInit ) {
                IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
                REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
                REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
                REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
                REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
            }
            bgUpdate( );
            return;
        }

        FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "statussub", 0, 256 * 192,
                             true );
        // dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        char buffer[ 50 ];

        if( _currentPage == 2 ) {
            IO::loadSprite( "UI/pg3", SPR_INFOPAGE_START_OAM + 7, SPR_INFOPAGE_PAL,
                            oam[ SPR_INFOPAGE_START_OAM + 7 ].gfxIndex, INFO_X + 64,
                            INFO_Y + 64 + 15, 64, 64, true, true, false, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            IO::loadSprite( "UI/pg4", SPR_INFOPAGE_START_OAM + 6, SPR_INFOPAGE_PAL,
                            oam[ SPR_INFOPAGE_START_OAM + 6 ].gfxIndex, INFO_X, INFO_Y + 64 + 15,
                            64, 64, true, true, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );

        } else {
            IO::loadSprite( "UI/pg1", SPR_INFOPAGE_START_OAM + 7, SPR_INFOPAGE_PAL,
                            oam[ SPR_INFOPAGE_START_OAM + 7 ].gfxIndex, INFO_X + 64,
                            INFO_Y + 64 + 15, 64, 64, true, true, false, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            IO::loadSprite( "UI/pg2", SPR_INFOPAGE_START_OAM + 6, SPR_INFOPAGE_PAL,
                            oam[ SPR_INFOPAGE_START_OAM + 6 ].gfxIndex, INFO_X, INFO_Y + 64 + 15,
                            64, 64, true, true, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );

            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_CTYPE_OAM( i ) ].isHidden = true; }
            IO::loadSprite( "BX/hpbar", SPR_EXP_BAR_OAM + 2, SPR_EXP_BAR_PAL,
                            oam[ SPR_EXP_BAR_OAM + 2 ].gfxIndex, INFO_X - 7 + 128 - 48,
                            INFO_Y + 15 + 15 * 7, 64, 32, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_NORMAL );
        }

        switch( _currentPage ) {
        case 0: {
            // TOP

            // Print Basic Pkmn info
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = false; }
            oam[ SPR_WINDOW_START_OAM + 5 ].isHidden      = true;
            oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden    = true;
            oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden    = false;
            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = false;

            IO::loadSprite( "STS/status_pkmn", SPR_WINDOW_OAM, SPR_ARROW_X_PAL,
                            oam[ SPR_WINDOW_OAM ].gfxIndex, 148 + 32 - 9, 6, 16, 16, false, false,
                            false, OBJPRIORITY_1, false, OBJMODE_NORMAL );

            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_DEX_OBTAINED ) ) {
                // Dex No
                u16 dexno
                    = SAVE::SAV.getActiveFile( ).getPkmnDisplayDexId( p_pokemon->getSpecies( ) );

                if( dexno != u16( -1 ) ) {
                    snprintf( buffer, 49, "%03hu", dexno );
                } else {
                    snprintf( buffer, 49, "???" );
                }
                writeLineTop( GET_STRING( IO::STR_UI_STS_SPECIES_IDX ), buffer, 0, IO::WHITE_IDX,
                              p_pokemon->isShiny( ) ? IO::RED2_IDX : IO::BLACK_IDX );
            }
            // Species Name
            writeLineTop( GET_STRING( IO::STR_UI_STS_SPECIES_NAME ),
                          FS::getDisplayName( p_pokemon->getSpecies( ) ).c_str( ), 1 );
            // Type
            for( u8 i = 0; i < 2; ++i ) {
                IO::loadTypeIcon( _data.m_baseForme.m_types[ i ], INFO_X - 7 + 128 - 66 + 34 * i,
                                  INFO_Y + 11 + 15 * 2, SPR_TYPE_OAM( i ), SPR_TYPE_PAL( i ),
                                  oam[ SPR_TYPE_OAM( i ) ].gfxIndex, false, CURRENT_LANGUAGE );
                oam[ SPR_TYPE_OAM( i ) ].isHidden = true;
            }

            writeLineTop( GET_STRING( IO::STR_UI_BAG_TYPE ), 0, 2, IO::WHITE_IDX, IO::BLACK_IDX );
            oam[ SPR_TYPE_OAM( 1 ) ].isHidden = false;
            if( _data.m_baseForme.m_types[ 0 ] != _data.m_baseForme.m_types[ 1 ] ) {
                oam[ SPR_TYPE_OAM( 0 ) ].isHidden = false;
            }
            // OT
            writeLineTop( GET_STRING( IO::STR_UI_STS_OT ), p_pokemon->m_boxdata.m_oT, 3 );
            // Id
            snprintf( buffer, 49, "%05hu", p_pokemon->m_boxdata.m_oTId );
            writeLineTop( GET_STRING( IO::STR_UI_STS_ID ), buffer, 4 );
            // Exp
            writeLineTop( GET_STRING( IO::STR_UI_STS_EXP ),
                          std::to_string( p_pokemon->m_boxdata.m_experienceGained ).c_str( ), 6 );
            if( p_pokemon->m_level < 100 ) {
                // Exp to next level
                writeLineTop( GET_STRING( IO::STR_UI_STS_NEXT ), 0, 7, IO::WHITE_IDX );
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_LEVEL ), INFO_X,
                                               INFO_Y + 6 + 15 * 8, false );

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
            for( u8 i = 0; i < 12; ++i ) {
                oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = false;
            }

            // Nature
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_NATURE_PREFIX ),
                                           INFO_X_SUB + 12, INFO_LINE_SUB( 0 ), true );
            IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
            IO::regularFont->setColor( IO::BLUE_IDX, 2 );
            auto nstr = std::string(
                GET_STRING( IO::STR_UI_STS_NATURE_START + u8( p_pokemon->getNature( ) ) ) );
            IO::regularFont->printStringC(
                nstr.c_str( ),
                INFO_X_SUB + 12
                    + IO::regularFont->stringWidthC( GET_STRING( IO::STR_UI_STS_NATURE_PREFIX ) ),
                INFO_LINE_SUB( 0 ), true );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            auto fmt = std::string( GET_STRING( IO::STR_UI_STS_NATURE_SUFFIX ) );
            auto wd1 = IO::regularFont->stringWidthC( nstr.c_str( ) );
            IO::regularFont->printStringC(
                fmt.c_str( ),
                INFO_X_SUB + 12 + wd1
                    + +IO::regularFont->stringWidthC( GET_STRING( IO::STR_UI_STS_NATURE_PREFIX ) ),
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
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_EGG_RECEIVED ),
                                               INFO_X_SUB + 12, INFO_LINE_SUB( currentLine++ ),
                                               true );
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
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_EGG_APP_HATCHED ),
                                               INFO_X_SUB + 12, INFO_LINE_SUB( currentLine++ ),
                                               true );

                if( p_pokemon->m_boxdata.m_fateful ) {
                    IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_FATEFUL_ENC ),
                                                   INFO_X_SUB + 12, INFO_LINE_SUB( currentLine++ ),
                                                   true );
                }
            } else if( p_pokemon->wasEgg( ) ) {
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_EGG_HATCHED ),
                                               INFO_X_SUB + 12, INFO_LINE_SUB( currentLine++ ),
                                               true );
                if( p_pokemon->m_boxdata.m_fateful ) {
                    IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_FATEFUL_ENC ),
                                                   INFO_X_SUB + 12, INFO_LINE_SUB( currentLine++ ),
                                                   true );
                }
            } else if( p_pokemon->isForeign( ) ) {
                if( p_pokemon->m_boxdata.m_fateful ) {
                    snprintf( buffer, 49, GET_STRING( IO::STR_UI_STS_APP_FATEFUL_ENC_AT_LEVEL ),
                              p_pokemon->m_boxdata.m_gotLevel );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                } else {
                    snprintf( buffer, 49, GET_STRING( IO::STR_UI_STS_APP_MET_AT_LEVEL ),
                              p_pokemon->m_boxdata.m_gotLevel );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                }
            } else {
                if( p_pokemon->m_boxdata.m_fateful ) {
                    snprintf( buffer, 49, GET_STRING( IO::STR_UI_STS_FATEFUL_ENC_AT_LEVEL ),
                              p_pokemon->m_boxdata.m_gotLevel );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                } else {
                    snprintf( buffer, 49, GET_STRING( IO::STR_UI_STS_MET_AT_LEVEL ),
                              p_pokemon->m_boxdata.m_gotLevel );
                    IO::regularFont->printStringC( buffer, INFO_X_SUB + 12,
                                                   INFO_LINE_SUB( currentLine++ ), true );
                }
            }

            // personality
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_PERSONALITY_START
                                                       + p_pokemon->m_boxdata.getPersonality( ) ),
                                           INFO_X_SUB + 12, INFO_LINE_SUB( currentLine++ ), true );

            // food
            if( currentLine < 9 ) {
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_LIKES ), INFO_X_SUB + 12,
                                               INFO_LINE_SUB( currentLine ), true );

                fmt = std::string( GET_STRING( IO::STR_UI_STS_LIKES_SUFFIX ) );
                snprintf( buffer, 49, fmt.c_str( ),
                          GET_STRING( IO::STR_UI_STS_TASTES_START
                                      + p_pokemon->m_boxdata.getTasteStr( ) ) );
                IO::regularFont->setColor( IO::BLUE2_IDX, 1 );
                IO::regularFont->setColor( IO::BLUE_IDX, 2 );
                IO::regularFont->printStringC(
                    buffer,
                    INFO_X_SUB + 12
                        + IO::regularFont->stringWidthC( GET_STRING( IO::STR_UI_STS_LIKES ) ),
                    INFO_LINE_SUB( currentLine ), true );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
                fmt = std::string( GET_STRING( IO::STR_UI_STS_POKEBLOCK ) );
                IO::regularFont->printStringC(
                    fmt.c_str( ),
                    INFO_X_SUB + 12
                        + IO::regularFont->stringWidthC( GET_STRING( IO::STR_UI_STS_LIKES ) )
                        + IO::regularFont->stringWidthC( buffer ),
                    INFO_LINE_SUB( currentLine ), true );
            }

            break;
        }
        case 1: {
            // TOP

            // Draw Pokemon stats
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = false; }
            oam[ SPR_WINDOW_START_OAM + 5 ].isHidden      = true;
            oam[ SPR_HP_BAR_OAM ].isHidden                = false;
            oam[ SPR_HP_BAR_OAM + 1 ].isHidden            = false;
            oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden    = false;
            oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden    = true;
            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = false;

            IO::loadSprite( "STS/status_moves", SPR_WINDOW_OAM, SPR_ARROW_X_PAL,
                            oam[ SPR_WINDOW_OAM ].gfxIndex, 148 + 32 - 9, 6, 16, 16, false, false,
                            false, OBJPRIORITY_1, false, OBJMODE_NORMAL );

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

            IO::smallFont->printString( HP_ICON, oam[ SPR_HP_BAR_OAM ].x - 14,
                                        oam[ SPR_HP_BAR_OAM ].y - 10,
                                        false ); // HP "icon"
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( IO::GRAY_IDX, 2 );

            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->printString( GET_STRING( IO::STR_UI_PKMN_STAT_LONG_HP ), INFO_X - 7,
                                          INFO_Y + 3, false );

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
                writeLineTop( GET_STRING( IO::STR_UI_PKMN_STAT_LONG_START + 1 + i ), "", i + 1 );

                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                snprintf( buffer, 49, "%hu", p_pokemon->getStat( i + 1 ) );
                IO::regularFont->printString( buffer, INFO_X - 7 + 68, INFO_Y + 11 + 15 * ( i + 1 ),
                                              false, IO::font::CENTER );
            }

            // EV total / Happiness
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->setColor( IO::BLACK_IDX, 4 );
            IO::regularFont->setColor( IO::COLOR_IDX, 3 );

            snprintf( buffer, 49, "\x01 %hu \x02 %hu", p_pokemon->m_boxdata.m_steps, evtotal );

            IO::regularFont->printStringC( buffer, INFO_X - 7 + 128, INFO_Y + 12 + 15 * 7, false,
                                           IO::font::RIGHT );

            // BOTTOM

            for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = false; }
            for( u8 i = 0; i < 12; ++i ) {
                oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true;
            }
            for( u8 i = 0; i < 4; ++i ) {
                IO::copySpritePal( MOVEBOX1_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
            }

            // Ability
            IO::regularFont->setColor( IO::COLOR_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_ABITLITY ),
                                           INFO_X_SUB + 190 - 8, INFO_Y_SUB, true,
                                           IO::font::RIGHT );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            auto aname = FS::getAbilityName( p_pokemon->getAbility( ) );
            IO::regularFont->printStringC( aname.c_str( ), INFO_X_SUB - 5, INFO_Y_SUB + 3, true );

            // Ability description

            IO::loadSprite( "UI/ab3", SPR_ABILITY_OAM_SUB, SPR_ABILITY_PAL_SUB,
                            oamSub[ SPR_ABILITY_OAM_SUB ].gfxIndex, INFO_X_SUB + 88, INFO_Y_SUB, 32,
                            64, false, false, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            IO::regularFont->printBreakingStringC(
                FS::getAbilityDescr( p_pokemon->getAbility( ) ).c_str( ), INFO_X_SUB,
                INFO_Y_SUB + 18, 188, true, IO::font::LEFT, 13 );

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::smallFont->setColor( IO::COLOR_IDX, 1 );
            IO::smallFont->setColor( 0, 2 );

            // Moves
            for( int i = 0; i < 4; i++ ) {
                if( !p_pokemon->getMove( i ) ) { continue; }

                BATTLE::type t;
                if( p_pokemon->getMove( i ) != M_HIDDEN_POWER ) {
                    t = _moves[ i ].m_type;
                } else {
                    t = p_pokemon->getHPType( );
                }
                IO::loadTypeIcon( t, oamSub[ SPR_MOVE_OAM_SUB( i ) ].x - 4,
                                  oamSub[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_TYPE_OAM_SUB( i ),
                                  SPR_TYPE_PAL_SUB( i ), oamSub[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                  true, CURRENT_LANGUAGE );

                IO::loadDamageCategoryIcon(
                    _moves[ i ].m_category, oamSub[ SPR_MOVE_OAM_SUB( i ) ].x - 4 + 32,
                    oamSub[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_CATEGORY_OAM_SUB( i ),
                    SPR_CATEGORY_PAL_SUB( i ), oamSub[ SPR_CATEGORY_OAM_SUB( i ) ].gfxIndex, true );

                IO::copySpritePal( MOVEBOX1_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );

                oamSub[ SPR_TYPE_OAM_SUB( i ) ].isHidden     = false;
                oamSub[ SPR_CATEGORY_OAM_SUB( i ) ].isHidden = false;
                for( u8 j = 0; j < 6; j++ ) {
                    oamSub[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = false;
                }

                auto mname = FS::getMoveName( p_pokemon->getMove( i ) );
                if( mname.length( ) > 18 ) {
                    snprintf( buffer, 20, "%s.", mname.c_str( ) );
                } else {
                    snprintf( buffer, 20, "%s", mname.c_str( ) );
                }

                IO::regularFont->printStringC( buffer, oamSub[ SPR_MOVE_OAM_SUB( i ) ].x + 48,
                                               oamSub[ SPR_MOVE_OAM_SUB( i ) ].y + 7, true,
                                               IO::font::CENTER );

                snprintf(
                    buffer, 49, GET_STRING( IO::STR_UI_STS_PP_FORMAT ),
                    p_pokemon->m_boxdata.m_curPP[ i ],
                    s8( _moves[ i ].m_pp * ( ( 5 + p_pokemon->m_boxdata.PPupget( i ) ) / 5.0 ) ) );
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
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_WINDOW_START_OAM + i ].isHidden = true; }

            IO::loadSprite( "STS/status_contest", SPR_WINDOW_OAM, SPR_ARROW_X_PAL,
                            oam[ SPR_WINDOW_OAM ].gfxIndex, 148 + 32 - 9, 6, 16, 16, false, false,
                            false, OBJPRIORITY_1, false, OBJMODE_NORMAL );
            // condition
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_CONDITION ), INFO_X + 4,
                                           INFO_Y + 2, false, IO::font::LEFT );

            // Contest type icon
            for( u8 i = 0; i < 5; ++i ) {
                IO::loadContestTypeIcon( (BATTLE::contestType) i, oam[ SPR_CTYPE_OAM( i ) ].x,
                                         oam[ SPR_CTYPE_OAM( i ) ].y, SPR_CTYPE_OAM( i ),
                                         SPR_CTYPE_PAL( i ), oam[ SPR_CTYPE_OAM( i ) ].gfxIndex,
                                         false, CURRENT_LANGUAGE );
                oam[ SPR_CTYPE_OAM( i ) ].isHidden = false;

                u16 x = 32 + COND_CENTER_X + COND_RADIUS * CONTEST_TYPE_POS_X[ i ] / 100;
                u16 y = 16 + COND_CENTER_Y + COND_RADIUS * CONTEST_TYPE_POS_Y[ i ] / 100;
                u16 x2
                    = 32 + COND_CENTER_X + COND_RADIUS * CONTEST_TYPE_POS_X[ ( i + 1 ) % 5 ] / 100;
                u16 y2
                    = 16 + COND_CENTER_Y + COND_RADIUS * CONTEST_TYPE_POS_Y[ ( i + 1 ) % 5 ] / 100;

                IO::drawLine( x - 1, y + 1, x2 - 1, y2 + 1, false, IO::GRAY_IDX );
                IO::drawLine( x + 1, y - 1, x2 + 1, y2 - 1, false, IO::GRAY_IDX );
                IO::drawLine( x + 1, y + 1, x2 + 1, y2 + 1, false, IO::GRAY_IDX );
                IO::drawLine( x, y, x2, y2, false, IO::BLACK_IDX );
                IO::drawLine( x, y, COND_CENTER_X + 32, COND_CENTER_Y + 16, false, IO::GRAY_IDX );

                auto st1 = ( p_pokemon->m_boxdata.m_contestStats[ i ] >> 3 ) + 5;
                auto st2 = ( p_pokemon->m_boxdata.m_contestStats[ ( i + 1 ) % 5 ] >> 3 ) + 5;

                for( auto s1 = 1, s2 = 1; s1 < st1 || s2 < st2; ) {
                    x  = 32 + COND_CENTER_X + s1 * CONTEST_TYPE_POS_X[ i ] / 100;
                    y  = 16 + COND_CENTER_Y + s1 * CONTEST_TYPE_POS_Y[ i ] / 100;
                    x2 = 32 + COND_CENTER_X + s2 * CONTEST_TYPE_POS_X[ ( i + 1 ) % 5 ] / 100;
                    y2 = 16 + COND_CENTER_Y + s2 * CONTEST_TYPE_POS_Y[ ( i + 1 ) % 5 ] / 100;
                    IO::drawLine( x, y, x2, y2, false, 241 );

                    if( s1 + 1 <= st1 ) { s1++; }
                    if( s2 + 1 <= st2 ) { s2++; }
                }
                IO::drawLine( x, y, x2, y2, false, 242 );
                IO::drawLine( x + 1, y, x2 + 1, y2, false, 242 );
                IO::drawLine( x, y + 1, x2, y2 + 1, false, 242 );
                IO::drawLine( x - 1, y, x2 - 1, y2, false, 242 );
                IO::drawLine( x, y - 1, x2, y2 - 1, false, 242 );
            }

            snprintf( buffer, 49, GET_STRING( IO::STR_UI_STS_SHEEN ),
                      p_pokemon->m_boxdata.m_contestStats[ 5 ] );
            IO::regularFont->printStringC( buffer, INFO_X - 7 + 128, INFO_Y + 20 + 15 * 7, false,
                                           IO::font::RIGHT );

            // BOTTOM

            oamSub[ SPR_WINDOW_PKMN_OAM_SUB ].isHidden    = false;
            oamSub[ SPR_WINDOW_MOVE_OAM_SUB ].isHidden    = false;
            oamSub[ SPR_WINDOW_CONTEST_OAM_SUB ].isHidden = true;
            for( u8 i = 0; i < 5; ++i ) { oamSub[ SPR_ABILITY_OAM_SUB + i ].isHidden = true; }
            for( u8 i = 0; i < 12; ++i ) {
                oamSub[ SPR_INFOPAGE_START_OAM_SUB + i ].isHidden = true;
            }

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            auto ribs = p_pokemon->m_boxdata.getRibbons( );
            // if no ribbons, write "no ribbons" string

            if( ribs.empty( ) ) {
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_NO_RIBBONS ), 128, 84,
                                               true, IO::font::CENTER );
            } else {
                snprintf( buffer, 49, GET_STRING( IO::STR_UI_STS_RIBBONS ), ribs.size( ) );
                IO::regularFont->printStringC( buffer, 32, 156, true );

                // load up to 12 ribbons
                for( u8 r = 0; r < 12 && r < ribs.size( ); ++r ) {
                    IO::loadRibbonIcon( ribs[ r ], 40 + 48 * ( r % 4 ), 16 + 44 * ( r / 4 ),
                                        SPR_RIBBON_OAM_SUB( r ), SPR_RIBBON_PAL_SUB( r ),
                                        oamSub[ SPR_RIBBON_OAM_SUB( r ) ].gfxIndex, true );
                }
            }
        }
        default: break;
        }
        IO::updateOAM( true );
        IO::updateOAM( false );

        FS::readData<unsigned int, unsigned short>( "nitro:/PICS/", "partybg2", 256 * 256 / 4, TEMP,
                                                    256, TEMP_PAL );
        dmaCopy( TEMP, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        FS::readData<unsigned int, unsigned short>( "nitro:/PICS/", "partybg", 256 * 256 / 4, TEMP,
                                                    256, TEMP_PAL );
        dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );

        if( !_needsInit ) {
            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
        }
        bgUpdate( );
    }

    void statusScreenUI::animate( ) {
        IO::animateBG( _frame, IO::bg3 );
        IO::animateBG( _frame, IO::bg3sub );
        bgUpdate( );
        ++_frame;
    }

    void statusScreenUI::highlightButton( u8 p_button, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        oam[ SPR_NAVIGATION_OAM_SUB( 1 ) ].palette
            = ( p_button == PREV_TARGET ) ? SPR_WINDOW_PAL_SUB : SPR_BOX_PAL_SUB;
        oam[ SPR_NAVIGATION_OAM_SUB( 2 ) ].palette
            = ( p_button == NEXT_TARGET ) ? SPR_WINDOW_PAL_SUB : SPR_BOX_PAL_SUB;
        oam[ SPR_NAVIGATION_OAM_SUB( 0 ) ].palette
            = ( p_button == BACK_TARGET ) ? SPR_WINDOW_PAL_SUB : SPR_BOX_PAL_SUB;

        for( u8 i = 0; i < getPageCount( ); ++i ) {
            oam[ SPR_PAGE_OAM_SUB( i ) ].palette
                = ( p_button == 50 + i ) ? SPR_WINDOW_PAL_SUB : SPR_BOX_PAL_SUB;
        }

        for( u8 i = 0; i < 4; ++i ) {
            if( i == p_button ) {
                IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
            } else {
                IO::copySpritePal( MOVEBOX1_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
            }
        }

        IO::updateOAM( p_bottom );
    }

    std::vector<std::pair<IO::inputTarget, u8>> statusScreenUI::getTouchPositions( bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        auto         res = std::vector<std::pair<IO::inputTarget, u8>>( );

        // navigation buttons
        if( !oam[ SPR_X_OAM_SUB ].isHidden ) {
            res.push_back( std::pair(
                IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                 oam[ SPR_X_OAM_SUB ].x + 24, oam[ SPR_X_OAM_SUB ].y + 24 ),
                EXIT_TARGET ) );
        }

        if( !oam[ SPR_ARROW_UP_OAM_SUB ].isHidden ) {
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_ARROW_UP_OAM_SUB ].x,
                                                       oam[ SPR_ARROW_UP_OAM_SUB ].y,
                                                       oam[ SPR_ARROW_UP_OAM_SUB ].x + 16,
                                                       oam[ SPR_ARROW_UP_OAM_SUB ].y + 16 ),
                                      PREV_TARGET ) );
        }
        if( !oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden ) {
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_ARROW_DOWN_OAM_SUB ].x,
                                                       oam[ SPR_ARROW_DOWN_OAM_SUB ].y,
                                                       oam[ SPR_ARROW_DOWN_OAM_SUB ].x + 16,
                                                       oam[ SPR_ARROW_DOWN_OAM_SUB ].y + 16 ),
                                      NEXT_TARGET ) );
        }
        if( !oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden ) {
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_ARROW_BACK_OAM_SUB ].x,
                                                       oam[ SPR_ARROW_BACK_OAM_SUB ].y,
                                                       oam[ SPR_ARROW_BACK_OAM_SUB ].x + 16,
                                                       oam[ SPR_ARROW_BACK_OAM_SUB ].y + 16 ),
                                      BACK_TARGET ) );
        }

        for( u8 i = 0; i < getPageCount( ); ++i ) {
            if( !oam[ SPR_PAGE_OAM_SUB( i ) ].isHidden ) {
                res.push_back( std::pair( IO::inputTarget( oam[ SPR_PAGE_OAM_SUB( i ) ].x,
                                                           oam[ SPR_PAGE_OAM_SUB( i ) ].y + 4,
                                                           oam[ SPR_PAGE_OAM_SUB( i ) ].x + 32,
                                                           oam[ SPR_PAGE_OAM_SUB( i ) ].y + 36 ),
                                          50 + i ) );
            }
        }

        for( u8 i = 0; i < 4; ++i ) {
            if( !oam[ SPR_MOVE_OAM_SUB( i ) ].isHidden ) {
                res.push_back( std::pair( IO::inputTarget( oam[ SPR_MOVE_OAM_SUB( i ) ].x,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].y - 2,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].x + 96,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].y + 32 ),
                                          i ) );
            }
        }

        return res;
    }

    void statusScreenUI::showDetails( pokemon* p_pokemon, u8 p_page, u8 p_detailsPage ) {
        _currentPage = -1;
        char buffer[ 50 ];
        switch( p_page ) {
        case 1: {
            // highlight move
            for( u8 i = 0; i < 4; ++i ) {
                if( i == p_detailsPage ) {
                    IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
                } else {
                    IO::copySpritePal( MOVEBOX1_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
                }
            }
            // hide ability window
            for( u8 i = 0; i < 5; ++i ) {
                IO::Oam->oamBuffer[ SPR_ABILITY_OAM_SUB + i ].isHidden = p_detailsPage < 4;
            }
            // show move details window
            for( u8 i = 0; i < 14; ++i ) {
                IO::Oam->oamBuffer[ SPR_MOVE_DETAILS_OAM_SUB + i ].isHidden = p_detailsPage >= 4;
            }

            // hide prev/next pkmn icons
            IO::Oam->oamBuffer[ SPR_ARROW_UP_OAM_SUB ].isHidden = !_allowKeyUp || p_detailsPage < 4;
            IO::Oam->oamBuffer[ SPR_ARROW_DOWN_OAM_SUB ].isHidden
                = !_allowKeyDown || p_detailsPage < 4;
            IO::Oam->oamBuffer[ SPR_NAVIGATION_OAM_SUB( 2 ) ].isHidden
                = !_allowKeyUp || p_detailsPage < 4;
            IO::Oam->oamBuffer[ SPR_NAVIGATION_OAM_SUB( 1 ) ].isHidden
                = !_allowKeyDown || p_detailsPage < 4;

            // hide page icons
            for( u8 i = 0; i < getPageCount( ); ++i ) {
                IO::Oam->oamBuffer[ SPR_PAGE_OAM_SUB( i ) ].isHidden
                    = ( i == p_page ) || p_pokemon->isEgg( ) || p_detailsPage < 4;
                IO::Oam->oamBuffer[ SPR_WINDOW_ICON_OAM_SUB( i ) ].isHidden
                    = ( i == p_page ) || p_pokemon->isEgg( ) || p_detailsPage < 4;
            }

            // Clear text window
            FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "statussub", 0,
                                 256 * 78, true );
            // dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 78 );

            // Write move details
            if( p_detailsPage < 4 ) {
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );

                // Move descr
                IO::regularFont->printBreakingStringC(
                    FS::getMoveDescr( p_pokemon->getMove( p_detailsPage ) ).c_str( ),
                    INFO_X_SUB - 8, INFO_Y_SUB + 3 - 2, 200, true, IO::font::LEFT, 13 );

                // power / acc
                char buffer2[ 25 ] = { 0 }, buffer3[ 25 ] = { 0 };

                if( p_pokemon->getMove( p_detailsPage ) == M_HIDDEN_POWER ) {
                    snprintf( buffer2, 24, GET_STRING( IO::STR_UI_BAG_POWER ),
                              p_pokemon->getHPPower( ) );
                } else if( _moves[ p_detailsPage ].m_basePower ) {
                    snprintf( buffer2, 24, GET_STRING( IO::STR_UI_BAG_POWER ),
                              _moves[ p_detailsPage ].m_basePower );
                }
                if( _moves[ p_detailsPage ].m_accuracy > 0
                    && _moves[ p_detailsPage ].m_accuracy <= 100 ) {
                    snprintf( buffer3, 24, GET_STRING( IO::STR_UI_BAG_ACCURACY ),
                              _moves[ p_detailsPage ].m_accuracy );
                } else {
                    snprintf( buffer3, 24, GET_STRING( IO::STR_UI_BAG_ACCURACY_NO_MISS ) );
                }

                snprintf( buffer, 49, "%s %s", buffer2, buffer3 );
                IO::regularFont->printStringC( buffer, INFO_X_SUB + 190 - 8, INFO_Y_SUB + 58 - 2,
                                               true, IO::font::RIGHT );
            } else {
                // restore ability
                IO::regularFont->setColor( IO::COLOR_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_STS_ABITLITY ),
                                               INFO_X_SUB + 190 - 8, INFO_Y_SUB, true,
                                               IO::font::RIGHT );

                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                auto aname = FS::getAbilityName( p_pokemon->getAbility( ) );
                IO::regularFont->printStringC( aname.c_str( ), INFO_X_SUB - 5, INFO_Y_SUB + 3,
                                               true );

                // Ability description
                IO::regularFont->printBreakingStringC(
                    FS::getAbilityDescr( p_pokemon->getAbility( ) ).c_str( ), INFO_X_SUB,
                    INFO_Y_SUB + 18, 188, true, IO::font::LEFT, 13 );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
            }

            IO::updateOAM( true );
            break;
        }
        default: break;
        }
    }
} // namespace STS

//    // Draw extra information about the specified ribbon
//    bool statusScreenUI::drawRibbon( const pokemon& p_pokemon, u8 p_ribbonIdx, bool p_bottom )
//    {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal      = IO::BG_PAL( p_bottom );
//
//        if( p_pokemon->isEgg( ) ) return false;
//
//        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
//        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
//        for( u8 i = 0; i < 12; ++i ) Oam->oamBuffer[ RIBBON_IDX + i ].isHidden = true;
//
//        pal[ IO::WHITE_IDX ] = IO::WHITE;
//        pal[ IO::GRAY_IDX ]  = IO::RGB( 20, 20, 20 );
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
