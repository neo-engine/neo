/*
Pokémon neo
------------------------------

file        : battleUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2021
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

#include <cstdio>

#include "ability.h"
#include "bagViewer.h"
#include "battle.h"
#include "battleTrainer.h"
#include "battleUI.h"
#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "item.h"
#include "keyboard.h"
#include "move.h"
#include "moveNames.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "saveGame.h"
#include "sound.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"

#include "screenFade.h"

namespace BATTLE {

// Top screen
// (opp 1, opp 2, own 1, own 2)
#define SPR_ABILITY_OAM( p_opponent )      ( 3 * ( p_opponent ) )
#define SPR_HPBAR_OAM                      6
#define SPR_STATUSBG_OAM( p_idx )          ( 10 + 3 * ( p_idx ) )
#define SPR_PKMN_START_OAM( p_idx )        ( 22 + 4 * ( p_idx ) )
#define SPR_PKMN_SHADOW_START_OAM( p_idx ) ( 38 + 4 * ( p_idx ) )
#define SPR_STATUS_ICON_OAM( p_idx )       ( 58 + ( p_idx ) )
#define SPR_SHINY_ICON_OAM( p_idx )        ( 62 + ( p_idx ) )
#define SPR_SHINY_PARTICLE_START_OAM       70
#define SPR_STAT_CHANGE_PARTICLE_START_OAM 86
#define SPR_MBOX_START_OAM                 86
#define SPR_BALL_START_OAM                 101
#define SPR_PLATFORM_OAM                   102
#define SPR_STATUS_BALL_OAM( p_idx )       ( 106 + ( p_idx ) )

#define SPR_PKMN_PAL( p_idx ) ( p_idx )
#define SPR_PKMN_SHADOW_PAL   4
#define SPR_PLATFORM_PAL      5
#define SPR_HPBAR_PAL         7
#define SPR_STATUS_ICON_PAL   9
#define SPR_SHINY_ICON_PAL    10
#define SPR_STATUSBG_PAL      11
#define SPR_BOX_PAL           12
#define SPR_STAT_CHANGE_PAL   13
#define SPR_BALL_PAL          14
#define SPR_STATUS_BALL_PAL   15

#define SPR_PKMN_GFX( p_idx )    ( 144 * ( p_idx ) )
#define SPR_STSBALL_GFX( p_idx ) ( SPR_PKMN_GFX( 4 ) + 16 + 4 * ( p_idx ) )

// Sub screen
#define SPR_BATTLE_FITE_OAM_SUB           0
#define SPR_BATTLE_BAG_OAM_SUB            2
#define SPR_BATTLE_PKMN_OAM_SUB           4
#define SPR_BATTLE_RUN_OAM_SUB            6
#define SPR_BATTLE_ICON_OAM_SUB           8
#define SPR_SMALL_MESSAGE_OAM_SUB         9
#define SPR_LARGE_MESSAGE_OAM_SUB         20
#define SPR_TYPE_OAM_SUB( p_idx )         ( 40 + ( p_idx ) )
#define SPR_MOVE_OAM_SUB( p_move )        ( 44 + 6 * ( p_move ) )
#define SPR_DMG_CAT_OAM_SUB( p_idx )      ( 80 + ( p_idx ) )
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 84 + 8 * ( p_pos ) )

#define SPR_BATTLE_FITE_PAL_SUB      0
#define SPR_BATTLE_BAG_PAL_SUB       1
#define SPR_BATTLE_PKMN_PAL_SUB      2
#define SPR_BATTLE_RUN_PAL_SUB       3
#define SPR_BATTLE_ICON_PAL_SUB      4
#define SPR_BOX_PAL_SUB              5
#define SPR_BOX_SEL_PAL_SUB          6
#define SPR_TYPE_PAL_SUB( p_idx )    ( 6 + ( p_idx ) )
#define SPR_DMG_CAT_PAL_SUB( p_idx ) ( 10 + ( p_idx ) )

// other macros
#define WILD_BATTLE_SPRITE_X_START 144
#define WILD_BATTLE_SPRITE_X       160

#define OPP_PLAT_Y 72
#define PLY_PLAT_Y 120
#define OPP_2_HP_X 94
#define OPP_2_HP_Y 44
#define OPP_1_HP_X 100
#define OPP_1_HP_Y 12
#define OWN_2_HP_X 130
#define OWN_2_HP_Y 152
#define OWN_1_HP_X 124
#define OWN_1_HP_Y 120

#define PKMN_OPP_1_X        192
#define PKMN_OPP_1_Y        ( OPP_PLAT_Y + 38 - 96 )
#define PKMN_OPP_1_X_SINGLE 160
#define PKMN_OPP_1_Y_SINGLE ( OPP_PLAT_Y + 35 - 96 )
#define PKMN_OPP_2_X        128
#define PKMN_OPP_2_Y        ( OPP_PLAT_Y + 32 - 96 )

#define PKMN_OWN_1_X        -40
#define PKMN_OWN_1_Y        84
#define PKMN_OWN_1_X_SINGLE 4
#define PKMN_OWN_1_Y_SINGLE 92
#define PKMN_OWN_2_X        40
#define PKMN_OWN_2_Y        100

#define HP_OUTLINE_COL 240
#define OWN_HP_COL     241
#define OPP_HP_COL     245
#define HP_COL( a, b ) ( ( a ) ? ( OPP_HP_COL + (b) *2 ) : ( OWN_HP_COL + (b) *2 ) )

    const u16 MOVEBOX3_SPR_PAL[ 6 ] = { 0x5208, 0x294A };
    const u16 MOVEBOX4_SPR_PAL[ 6 ] = { 0x18C6, 0x294A };

    void battleUI::initTop( ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( false );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );

        u16 tileCnt = SPR_PKMN_GFX( 4 ) + 16;

        // status ball
        tileCnt = IO::loadSprite( "BT/bb1", SPR_STATUS_BALL_OAM( 0 ), SPR_STATUS_BALL_PAL, tileCnt,
                                  OPP_1_HP_X + 8, OPP_1_HP_Y + 8, 16, 16, false, false, true,
                                  OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( "BT/bb2", SPR_STATUS_BALL_OAM( 1 ), SPR_STATUS_BALL_PAL, tileCnt,
                                  OPP_2_HP_X + 8, OPP_2_HP_Y + 8, 16, 16, false, false, true,
                                  OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( "BT/bb3", SPR_STATUS_BALL_OAM( 6 ), SPR_STATUS_BALL_PAL, tileCnt,
                                  OWN_1_HP_X + 8, OWN_1_HP_Y + 8, 16, 16, false, false, true,
                                  OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( "BT/bb4", SPR_STATUS_BALL_OAM( 7 ), SPR_STATUS_BALL_PAL, tileCnt,
                                  OWN_2_HP_X + 8, OWN_2_HP_Y + 8, 16, 16, false, false, true,
                                  OBJPRIORITY_0, false );

        for( u8 i = 1 + ( _mode == DOUBLE ); i < 6; ++i ) {
            IO::loadSprite( SPR_STATUS_BALL_OAM( i ), SPR_STATUS_BALL_PAL, SPR_STSBALL_GFX( 0 ),
                            4 + 16 * ( i - 1 - ( _mode == DOUBLE ) ), -2, 16, 16, 0, 0, 0, false,
                            false, true, OBJPRIORITY_0, false );
            IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + i ), SPR_STATUS_BALL_PAL, SPR_STSBALL_GFX( 0 ),
                            156 + 16 * i, 181, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_0,
                            false );
        }
        if( _mode == SINGLE ) {
            IO::loadSprite( SPR_STATUS_BALL_OAM( 6 ), SPR_STATUS_BALL_PAL, SPR_STSBALL_GFX( 0 ),
                            OWN_2_HP_X + 8, OWN_2_HP_Y + 8, 16, 16, 0, 0, 0, false, false, true,
                            OBJPRIORITY_0, false );
        }

        // platforms
        tileCnt = IO::loadPlatform( _platform2, 144, OPP_PLAT_Y, SPR_PLATFORM_OAM, SPR_PLATFORM_PAL,
                                    tileCnt, false );
        tileCnt = IO::loadPlatform( _platform, -52, PLY_PLAT_Y, SPR_PLATFORM_OAM + 2,
                                    SPR_PLATFORM_PAL + 1, tileCnt, false );

        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].x             = 80 - 16;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 2 ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 2 ].isSizeDouble  = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 2 ].rotationIndex = 0;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].isSizeDouble  = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].rotationIndex = 0;
        IO::OamTop->matrixBuffer[ 0 ].hdx                           = 140;
        IO::OamTop->matrixBuffer[ 0 ].vdy                           = 130;
        IO::OamTop->matrixBuffer[ 1 ].hdx                           = 154;
        IO::OamTop->matrixBuffer[ 1 ].vdy                           = 154;

        // Preload other sprites
        // HP "bars"
        // opponent
        tileCnt = IO::loadSprite( "BT/nmbg", SPR_STATUSBG_OAM( 0 ), SPR_STATUSBG_PAL, tileCnt,
                                  OPP_1_HP_X - 96, OPP_1_HP_Y, 64, 32, false, false, true,
                                  OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_STATUSBG_OAM( 0 ) + 1, SPR_STATUSBG_PAL,
                        IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OPP_1_HP_X - 32,
                        OPP_1_HP_Y, 64, 32, 0, 0, 0, false, true, true, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_STATUSBG_OAM( 1 ), SPR_STATUSBG_PAL,
                        IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OPP_2_HP_X - 96,
                        OPP_2_HP_Y, 64, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_STATUSBG_OAM( 1 ) + 1, SPR_STATUSBG_PAL,
                        IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OPP_2_HP_X - 32,
                        OPP_2_HP_Y, 64, 32, 0, 0, 0, false, true, true, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );

        IO::loadSprite( SPR_HPBAR_OAM, SPR_HPBAR_PAL, tileCnt, OPP_1_HP_X, OPP_1_HP_Y, 32, 32, 0, 0,
                        0, false, false, true, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "BT/hpbar_battle_opp", SPR_HPBAR_OAM + 1, SPR_HPBAR_PAL, tileCnt,
                                  OPP_2_HP_X, OPP_2_HP_Y, 32, 32, false, false, true, OBJPRIORITY_3,
                                  false, OBJMODE_BLENDED );

        // player
        if( _mode == DOUBLE ) {
            IO::loadSprite( SPR_HPBAR_OAM + 2, SPR_HPBAR_PAL + 1, tileCnt, OWN_1_HP_X, OWN_1_HP_Y,
                            32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            tileCnt = IO::loadSprite( "BT/hpbar_battle_opp", SPR_HPBAR_OAM + 3, SPR_HPBAR_PAL + 1,
                                      tileCnt, OWN_2_HP_X, OWN_2_HP_Y, 32, 32, false, false, true,
                                      OBJPRIORITY_3, false, OBJMODE_BLENDED );

            IO::loadSprite( SPR_STATUSBG_OAM( 2 ), SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OWN_1_HP_X,
                            OWN_1_HP_Y, 64, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 2 ) + 2, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_1_HP_X + 64, OWN_1_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 2 ) + 1, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_1_HP_X + 76, OWN_1_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );

            IO::loadSprite( SPR_STATUSBG_OAM( 3 ), SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OWN_2_HP_X,
                            OWN_2_HP_Y, 64, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 3 ) + 2, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_2_HP_X + 64, OWN_2_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 3 ) + 1, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_2_HP_X + 76, OWN_2_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );

        } else {
            IO::loadSprite( SPR_HPBAR_OAM + 3, SPR_HPBAR_PAL + 1, tileCnt, OWN_1_HP_X, OWN_1_HP_Y,
                            32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            tileCnt = IO::loadSprite( "BT/hpbar_battle_opp", SPR_HPBAR_OAM + 2, SPR_HPBAR_PAL + 1,
                                      tileCnt, OWN_2_HP_X, OWN_2_HP_Y, 32, 32, false, false, true,
                                      OBJPRIORITY_3, false, OBJMODE_BLENDED );

            IO::loadSprite( SPR_STATUSBG_OAM( 2 ), SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OWN_2_HP_X,
                            OWN_2_HP_Y, 64, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 2 ) + 2, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_2_HP_X + 64, OWN_2_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 2 ) + 1, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_2_HP_X + 76, OWN_2_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );

            IO::loadSprite( SPR_STATUSBG_OAM( 3 ), SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex, OWN_1_HP_X,
                            OWN_1_HP_Y, 64, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 3 ) + 1, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_1_HP_X + 76, OWN_1_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );
            IO::loadSprite( SPR_STATUSBG_OAM( 3 ) + 2, SPR_STATUSBG_PAL,
                            IO::OamTop->oamBuffer[ SPR_STATUSBG_OAM( 0 ) ].gfxIndex,
                            OWN_1_HP_X + 64, OWN_1_HP_Y, 64, 32, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );
        }

        // Shiny icon
        tileCnt = IO::loadSprite( "STS/status_shiny", SPR_SHINY_ICON_OAM( 0 ), SPR_SHINY_ICON_PAL,
                                  tileCnt, 0, 0, 8, 8, false, false, true, OBJPRIORITY_0, false,
                                  OBJMODE_NORMAL );

        // Shiny / Status icon
        for( u8 i = 0; i < 4; ++i ) {
            u16 anchorx
                = IO::OamTop->oamBuffer[ SPR_HPBAR_OAM + i ].x + ( ( i < 2 ) ? -8 : 34 + 82 );
            u16 anchory = IO::OamTop->oamBuffer[ SPR_HPBAR_OAM + i ].y + 4;
            tileCnt     = IO::loadSprite( SPR_STATUS_ICON_OAM( i ), SPR_STATUS_ICON_PAL, tileCnt,
                                      anchorx, anchory, 8, 8, 0, 0, 8 * 8 / 2, false, false, true,
                                      OBJPRIORITY_0, false, OBJMODE_NORMAL );
            IO::loadSprite( SPR_SHINY_ICON_OAM( i ), SPR_SHINY_ICON_PAL,
                            IO::OamTop->oamBuffer[ SPR_SHINY_ICON_OAM( 0 ) ].gfxIndex, anchorx,
                            anchory + 8, 8, 8, 0, 0, 0, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_NORMAL );
        }

        // Ability windows
        IO::loadSprite( SPR_ABILITY_OAM( 1 ), SPR_BOX_PAL, tileCnt, 64, 96, 64, 32, 0, 0, 0, false,
                        false, true, OBJPRIORITY_3, false, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ABILITY_OAM( 1 ) + 1, SPR_BOX_PAL, tileCnt, 32, 96, 64, 32, 0, 0, 0,
                        false, false, true, OBJPRIORITY_3, false, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ABILITY_OAM( 1 ) + 2, SPR_BOX_PAL, tileCnt, 0, 96, 64, 32, 0, 0, 0,
                        false, false, true, OBJPRIORITY_3, false, OBJMODE_NORMAL );

        IO::loadSprite( SPR_ABILITY_OAM( 0 ) + 1, SPR_BOX_PAL, tileCnt, 128 + 32, 64, 64, 32, 0, 0,
                        0, true, true, true, OBJPRIORITY_3, false, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ABILITY_OAM( 0 ) + 2, SPR_BOX_PAL, tileCnt, 128 + 64, 64, 64, 32, 0, 0,
                        0, true, true, true, OBJPRIORITY_3, false, OBJMODE_NORMAL );
        tileCnt
            = IO::loadSprite( "BT/battle_ability", SPR_ABILITY_OAM( 0 ), SPR_BOX_PAL, tileCnt, 128,
                              64, 64, 32, true, true, true, OBJPRIORITY_3, false, OBJMODE_NORMAL );

        // Shiny animation
        for( u8 i = 0; i < 8; ++i ) {
            IO::loadSprite( SPR_SHINY_PARTICLE_START_OAM + i, SPR_SHINY_ICON_PAL,
                            IO::OamTop->oamBuffer[ SPR_SHINY_ICON_OAM( 0 ) ].gfxIndex, 0, 0, 8, 8,
                            0, 0, 0, false, false, true, OBJPRIORITY_0, false, OBJMODE_BLENDED );
        }
        for( u8 i = 8; i < 15; ++i ) {
            IO::loadSprite( SPR_SHINY_PARTICLE_START_OAM + i, SPR_SHINY_ICON_PAL, tileCnt, 0, 0, 16,
                            16, 0, 0, 0, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        }
        tileCnt = IO::loadSprite( "BT/shiny_big", SPR_SHINY_PARTICLE_START_OAM + 15,
                                  SPR_SHINY_ICON_PAL, tileCnt, 0, 0, 16, 16, false, false, true,
                                  OBJPRIORITY_0, false, OBJMODE_BLENDED );

        // stat increase / decrease
        for( u8 i = 1; i < 15; ++i ) {
            IO::loadSprite( SPR_STAT_CHANGE_PARTICLE_START_OAM + i, SPR_STAT_CHANGE_PAL, tileCnt, 0,
                            0, 8, 8, 0, 0, 0, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        }
        tileCnt = IO::loadSprite( "BT/stat_up", SPR_STAT_CHANGE_PARTICLE_START_OAM,
                                  SPR_STAT_CHANGE_PAL, tileCnt, 0, 0, 8, 8, false, false, true,
                                  OBJPRIORITY_0, false, OBJMODE_BLENDED );

        IO::updateOAM( false );
    }

    void battleUI::initSub( ) {
        IO::initOAMTable( true );
        SpriteEntry* oam = IO::Oam->oamBuffer;

        u16 tileCnt = 0;

        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        bgUpdate( );

        // Load sprites
        // move selection

        IO::loadSprite( SPR_BATTLE_FITE_OAM_SUB, SPR_BATTLE_FITE_PAL_SUB, tileCnt, 128 - 64, 68, 64,
                        64, 0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "BT/ft", SPR_BATTLE_FITE_OAM_SUB + 1, SPR_BATTLE_FITE_PAL_SUB,
                                  tileCnt, 128, 64, 64, 64, true, true, true, OBJPRIORITY_3, true,
                                  OBJMODE_BLENDED );

        IO::loadSprite( SPR_BATTLE_PKMN_OAM_SUB, SPR_BATTLE_PKMN_PAL_SUB, tileCnt,
                        128 - 32 - 16 - 64 - 6, 68 + 64 + 18, 32, 32, 0, 0, 0, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "BT/pk", SPR_BATTLE_PKMN_OAM_SUB + 1, SPR_BATTLE_PKMN_PAL_SUB,
                                  tileCnt, 128 - 32 - 16 - 32 - 6, 68 + 64 + 18, 32, 32, true, true,
                                  true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        IO::loadSprite( SPR_BATTLE_RUN_OAM_SUB, SPR_BATTLE_RUN_PAL_SUB, tileCnt, 128 - 32,
                        68 + 64 + 18, 32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "BT/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                  tileCnt, 128, 68 + 64 + 18, 32, 32, true, true, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        IO::loadSprite( SPR_BATTLE_BAG_OAM_SUB, SPR_BATTLE_BAG_PAL_SUB, tileCnt, 128 + 32 + 16 + 6,
                        68 + 64 + 18, 32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "BT/bg", SPR_BATTLE_BAG_OAM_SUB + 1, SPR_BATTLE_BAG_PAL_SUB,
                                  tileCnt, 128 + 32 + 16 + 32 + 6, 68 + 64 + 18, 32, 32, true, true,
                                  true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // message boxes

        // small (move / attack selection)
        for( u8 i = 0; i < 7; ++i ) {
            IO::loadSprite( SPR_SMALL_MESSAGE_OAM_SUB + 7 - i, SPR_BOX_PAL_SUB, tileCnt,
                            128 - 32 - 64 + 28 * i, 68 - 18 - 26, 32, 32, 0, 0, 0, false, true,
                            true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        tileCnt = IO::loadSprite( "UI/message_24", SPR_SMALL_MESSAGE_OAM_SUB, SPR_BOX_PAL_SUB,
                                  tileCnt, 128 - 24 - 16 - 64, 68 - 18 - 26, 32, 32, false, false,
                                  true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // large (battle log)

        for( u8 i = 0; i < 4; ++i ) {
            for( u8 j = 0; j < 3; ++j ) {
                IO::loadSprite( SPR_LARGE_MESSAGE_OAM_SUB + 3 + 4 * j - i, SPR_BOX_PAL_SUB, tileCnt,
                                8 + 58 * i, 20 + 49 * j, 64, 64, 0, 0, 0, j == 2, i, true,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        }
        tileCnt = IO::loadSprite( "UI/message_large", SPR_LARGE_MESSAGE_OAM_SUB + 3,
                                  SPR_BOX_PAL_SUB, tileCnt, 8, 20, 64, 64, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // Pkmn Icon
        tileCnt = IO::loadSprite( SPR_BATTLE_ICON_OAM_SUB, SPR_BATTLE_ICON_PAL_SUB, tileCnt, 112,
                                  68 + 4, 32, 32, 0, 0, 32 * 32 / 2, false, false, true,
                                  OBJPRIORITY_0, true );

        // move windows
        s16 sy = -22;
        for( u8 i = 1; i < 3; i++ ) {
            u8 pos = 2 * i - 2;
            if( i == 1 ) {
                tileCnt
                    = IO::loadSprite( "BT/movebox4", SPR_MOVE_OAM_SUB( pos ),
                                      SPR_TYPE_PAL_SUB( pos ), tileCnt, 31, 44 + i * 42 + sy, 16,
                                      32, false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
                tileCnt = IO::loadSprite( "BT/movebox2", SPR_MOVE_OAM_SUB( pos ) + 1,
                                          SPR_TYPE_PAL_SUB( pos ), tileCnt, 31 + 16,
                                          44 + i * 42 + sy, 16, 32, false, false, true,
                                          OBJPRIORITY_3, true, OBJMODE_NORMAL );
            } else {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 31, 44 + i * 42 + sy, 16, 32,
                                0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 1, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 31 + 16,
                                44 + i * 42 + sy, 16, 32, 0, 0, 0, false, false, true,
                                OBJPRIORITY_3, true, OBJMODE_NORMAL );
            }
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + j, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 31 + j * 16,
                                44 + i * 42 + sy, 16, 32, 0, 0, 0, false, false, true,
                                OBJPRIORITY_3, true, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 5, SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 31 + 5 * 16, 44 + i * 42 + sy,
                            16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_NORMAL );
        }
        for( u8 i = 1; i < 3; i++ ) {
            u8 pos = 2 * i - 1;
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ), SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 129, 44 + i * 42 + sy, 16, 32, 0,
                            0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + j, SPR_TYPE_PAL_SUB( pos ),
                                oam[ SPR_MOVE_OAM_SUB( 0 ) + 1 ].gfxIndex, 129 + j * 16,
                                44 + i * 42 + sy, 16, 32, 0, 0, 0, false, false, true,
                                OBJPRIORITY_3, true, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_MOVE_OAM_SUB( pos ) + 5, SPR_TYPE_PAL_SUB( pos ),
                            oam[ SPR_MOVE_OAM_SUB( 0 ) ].gfxIndex, 129 + 5 * 16, 44 + i * 42 + sy,
                            16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_NORMAL );
        }

        // type icons
        for( u8 i = 0; i < 4; ++i ) {
            tileCnt = IO::loadTypeIcon( UNKNOWN, oam[ SPR_MOVE_OAM_SUB( i ) + 1 ].x,
                                        oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_TYPE_OAM_SUB( i ),
                                        SPR_TYPE_PAL_SUB( i ), tileCnt, true, CURRENT_LANGUAGE );
            oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden = true;
        }

        // damage cat icons
        for( u8 i = 0; i < 4; ++i ) {
            tileCnt
                = IO::loadTypeIcon( UNKNOWN, oam[ SPR_MOVE_OAM_SUB( i ) + 1 ].x + 32,
                                    oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_DMG_CAT_OAM_SUB( i ),
                                    SPR_DMG_CAT_PAL_SUB( i ), tileCnt, true, CURRENT_LANGUAGE );
            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
        }

        // choice boxes

        for( u8 i = 0; i < 2; i++ ) {
            u8 pos = 2 * i;
            if( !i ) {
                tileCnt
                    = IO::loadSprite( "SEL/noselection_96_32_1", SPR_CHOICE_START_OAM_SUB( pos ),
                                      SPR_BOX_PAL_SUB, tileCnt, 29, 42 + i * 36, 16, 32, false,
                                      false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
                tileCnt = IO::loadSprite( "SEL/noselection_96_32_2",
                                          SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                          tileCnt, 29 + 11, 42 + i * 36, 16, 32, false, false, true,
                                          OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29, 42 + i * 36, 16,
                                32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_BLENDED );
            }
            for( u8 j = 2; j < 7; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29 + 5 * 16, 42 + i * 36,
                            16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
        }

        for( u8 i = 0; i < 2; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, 42 + i * 36, 16, 32,
                            0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            for( u8 j = 1; j < 7; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131 + 5 * 16,
                            42 + i * 36, 16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
        }
        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );

        IO::updateOAM( true );
    }

    void battleUI::animateBallRelease( bool p_opponent, u8 p_pos, u8 p_ballId ) const {
        u16 x = ( _mode == SINGLE ) ? ( p_opponent ? PKMN_OPP_1_X_SINGLE : PKMN_OWN_1_X_SINGLE )
                                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_X : PKMN_OPP_1_X )
                                                     : ( p_pos ? PKMN_OWN_2_X : PKMN_OWN_1_X ) ) );
        u16 y = ( _mode == SINGLE ) ? ( p_opponent ? PKMN_OPP_1_Y_SINGLE : PKMN_OWN_1_Y_SINGLE )
                                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_Y : PKMN_OPP_1_Y )
                                                     : ( p_pos ? PKMN_OWN_2_Y : PKMN_OWN_1_Y ) ) );

        x += 24;
        y += 48;

        IO::OamTop->matrixBuffer[ 7 ].hdx = 1LLU << 9;
        IO::OamTop->matrixBuffer[ 7 ].vdx = 0;
        IO::OamTop->matrixBuffer[ 7 ].hdy = 0;
        IO::OamTop->matrixBuffer[ 7 ].vdy = 1LLU << 9;

        char buffer[ 50 ];
        snprintf( buffer, 49, "PB/%hhu/%hhu_", p_ballId, p_ballId );

        IO::loadSprite( ( std::string( buffer ) + "0" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x, y, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "1" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x - 1, y - 2, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "2" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x - 2, y - 1, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "3" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x - 3, y, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }

        IO::loadSprite( ( std::string( buffer ) + "4" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x - 3, y + 1, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "5" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x - 2, y + 2, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "6" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x - 1, y + 4, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "7" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x, y + 7, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 1; ++i ) { swiWaitForVBlank( ); }
        SOUND::playSoundEffect( SFX_BATTLE_RECALL );

        IO::loadSprite( ( std::string( buffer ) + "8" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x, y + 10, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "9" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x, y + 10, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        IO::loadSprite( ( std::string( buffer ) + "10" ).c_str( ), SPR_BALL_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 4 ), x, y + 10, 32, 32, false, false, false, OBJPRIORITY_0,
                        false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }

        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = false;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 0;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isHidden      = true;
        IO::updateOAM( false );
    }

    void battleUI::animateBallThrow( u8 p_slot, u8 p_ballId ) const {
        (void) p_slot;

        SOUND::playSoundEffect( SFX_BATTLE_THROW );

        u16 x = 0, y = 0;

        char buffer[ 50 ];
        snprintf( buffer, 49, "PB/%hhu/%hhu_", p_ballId, p_ballId );
        IO::OamTop->matrixBuffer[ 7 ].hdx = 1LLU << 8;
        IO::OamTop->matrixBuffer[ 7 ].vdy = 1LLU << 8;
        IO::OamTop->matrixBuffer[ 7 ].vdx = 0;
        IO::OamTop->matrixBuffer[ 7 ].hdy = 0;

        for( x = 92; x < 184; x++ ) {
            y    = x * x / 100 - 3 * x + 236;
            u8 f = x - 92;

            if( x % 2 == 0 ) {
                IO::OamTop->matrixBuffer[ 7 ].hdx += 5;
                IO::OamTop->matrixBuffer[ 7 ].vdy += 5;
            }

            if( ( f % 6 ) == 0 ) {
                IO::loadSprite(
                    ( std::string( buffer ) + std::to_string( ( f / 3 + 6 ) % 9 ) ).c_str( ),
                    SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32, false, false,
                    false, OBJPRIORITY_0, false );
                IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
                IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
                IO::updateOAM( false );
            }

            if( x % 4 == 2 ) {
                IO::OamTop->matrixBuffer[ 7 ].hdx++;
                IO::OamTop->matrixBuffer[ 7 ].vdy++;
            }
            if( x % 8 == 4 ) { swiWaitForVBlank( ); }
        }
        IO::OamTop->matrixBuffer[ 7 ].hdx = 1LLU << 9;
        IO::OamTop->matrixBuffer[ 7 ].vdy = 1LLU << 9;

        SOUND::playSoundEffect( SFX_BATTLE_JUMPTOBALL );
        IO::loadSprite( ( std::string( buffer ) + std::to_string( 9 ) ).c_str( ),
                        SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32, false,
                        false, false, OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }

        IO::loadSprite( ( std::string( buffer ) + std::to_string( 10 ) ).c_str( ),
                        SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32, false,
                        false, false, OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::animateShiny( bool p_opponent, u8 p_slot, u8 p_shinyType ) const {
        SOUND::playSoundEffect( SFX_SHINY );
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        if( p_shinyType == 2 ) { // Forced shinies
            IO::loadSprite( "BT/shiny_big_alt2", SPR_SHINY_PARTICLE_START_OAM + 15,
                            SPR_SHINY_ICON_PAL, oam[ SPR_SHINY_PARTICLE_START_OAM + 15 ].gfxIndex,
                            0, 0, 16, 16, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        } else if( p_shinyType == 1 ) { // Circle for shinies due to extra rolls
            IO::loadSprite( "BT/shiny_big_alt", SPR_SHINY_PARTICLE_START_OAM + 15,
                            SPR_SHINY_ICON_PAL, oam[ SPR_SHINY_PARTICLE_START_OAM + 15 ].gfxIndex,
                            0, 0, 16, 16, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        } else { // normal star
            IO::loadSprite( "BT/shiny_big", SPR_SHINY_PARTICLE_START_OAM + 15, SPR_SHINY_ICON_PAL,
                            oam[ SPR_SHINY_PARTICLE_START_OAM + 15 ].gfxIndex, 0, 0, 16, 16, false,
                            false, true, OBJPRIORITY_0, false, OBJMODE_BLENDED );
        }

        u16 centerx = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].x + 48;
        u16 centery = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].y + 48;

        u16 radius = 25;
        u16 sm_sh = 4, bg_sh = 8;

        if( !p_opponent ) {
            sm_sh = 8, bg_sh = 16;
            radius *= 2;
            centerx += 48;
            centery += 48;
        }

        constexpr s8 dirs[ 32 ][ 2 ] = {
            { 10, 0 },  { 10, 2 },   { 9, 4 },   { 8, 6 },
            { 7, 7 },   { 6, 8 },    { 4, 9 },   { 2, 10 },

            { 0, 10 },  { -2, 10 },  { -4, 9 },  { -6, 8 },
            { -7, 7 },  { -8, 6 },   { -9, 4 },  { -10, 2 },

            { -10, 0 }, { -10, -2 }, { -9, -4 }, { -8, -6 },
            { -7, -7 }, { -6, -8 },  { -4, -9 }, { -2, -10 },

            { 0, -10 }, { 2, -10 },  { 4, -9 },  { 6, -8 },
            { 7, -7 },  { 8, -6 },   { 9, -4 },  { 10, -2 },
        };

        // init frame
        for( u8 i = 0; i < 8; ++i ) {
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = false;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = false;

            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].x = centerx + dirs[ 4 * i ][ 0 ] - sm_sh;
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].y = centery + dirs[ 4 * i ][ 1 ] - sm_sh;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].x
                = centerx + dirs[ 4 * i + 2 ][ 0 ] - bg_sh;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].y
                = centery + dirs[ 4 * i + 2 ][ 1 ] - bg_sh;
        }
        IO::updateOAM( false );
        swiWaitForVBlank( );

        for( u8 f = 0; f < 14; ++f ) {
            for( u8 i = 0; i < 8; ++i ) {
                u8 pos = ( 4 * i + ( f & 3 ) ) & 31;

                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].x
                    = centerx + ( dirs[ pos ][ 0 ] * ( radius + 5 * f / 4 ) / 10 ) - sm_sh;
                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].y
                    = centery + ( dirs[ pos ][ 1 ] * ( radius + 5 * f / 4 ) / 10 ) - sm_sh;
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].x
                    = centerx + ( dirs[ ( pos + 2 ) & 31 ][ 0 ] * ( radius + 5 * f / 4 ) / 10 )
                      - bg_sh;
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].y
                    = centery + ( dirs[ ( pos + 2 ) & 31 ][ 1 ] * ( radius + 5 * f / 4 ) / 10 )
                      - bg_sh;
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
            if( f & 1 ) { swiWaitForVBlank( ); }
        }

        // hide everything
        for( u8 i = 0; i < 8; ++i ) {
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = true;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = true;
        }

        // some random extra noise
        for( u8 f = 0; f < 8; ++f ) {
            for( u8 i = 0; i < 8; ++i ) {
                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = rand( ) & 2;
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = rand( ) & 2;

                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].x = centerx - 32 - sm_sh + ( rand( ) & 63 );
                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].y = centery - 32 - sm_sh + ( rand( ) & 63 );
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].x
                    = centerx - 32 - bg_sh + ( rand( ) & 63 );
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].y
                    = centery - 32 - bg_sh + ( rand( ) & 63 );
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }

        // hide everything
        for( u8 i = 0; i < 8; ++i ) {
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = true;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = true;
        }
        IO::updateOAM( false );

        swiWaitForVBlank( );
    }

    void battleUI::animateStatChange( bool p_opponent, u8 p_slot, bool p_down ) const {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        s8  diff = 0, shift = 0;
        u16 centerx = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].x + 48;
        u16 centery = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].y + 48;

        u16 scale = 1;

        if( p_down ) {
            SOUND::playSoundEffect( SFX_BATTLE_DECREASE );
            IO::loadSprite( "BT/stat_down", SPR_STAT_CHANGE_PARTICLE_START_OAM, SPR_STAT_CHANGE_PAL,
                            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM ].gfxIndex, 0, 0, 8, 8, false,
                            false, true, OBJPRIORITY_0, false, OBJMODE_BLENDED );
            diff  = 1;
            shift = -20;
        } else {
            SOUND::playSoundEffect( SFX_BATTLE_INCREASE );
            IO::loadSprite( "BT/stat_up", SPR_STAT_CHANGE_PARTICLE_START_OAM, SPR_STAT_CHANGE_PAL,
                            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM ].gfxIndex, 0, 0, 8, 8, false,
                            false, true, OBJPRIORITY_0, false, OBJMODE_BLENDED );
            diff  = -1;
            shift = 20;
        }

        if( !p_opponent ) {
            centerx += 48;
            centery += 48;
            scale = 2;
            shift *= scale;
        }

        constexpr s8 pos[ 15 ][ 2 ] = {
            { -19, 5 }, { -19, -5 }, { -10, 0 }, { -10, 10 }, { -10, -10 }, { 0, 5 },   { 0, -5 },
            { 0, 15 },  { 0, -15 },  { 10, 0 },  { 10, 10 },  { 10, -10 },  { 19, -5 }, { 19, 5 },
        };

        for( u8 i = 0; i < 14; ++i ) {
            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isHidden = false;
            if( !p_opponent ) {
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isSizeDouble  = true;
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isRotateScale = true;
            }

            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].x = centerx + 3 * pos[ i ][ 0 ] / 2;
            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].y
                = centery + shift + 3 * scale * pos[ i ][ 1 ] / 2;
        }
        IO::updateOAM( false );

        for( u8 j = 0; j < 10; ++j ) {
            for( u8 i = 0; i < 14; ++i ) {
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].y += 2 * diff * scale;
            }
            swiWaitForVBlank( );
            IO::updateOAM( false );
        }
        for( u8 i = 0; i < 14; ++i ) {
            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isHidden = true;
            if( !p_opponent ) {
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isRotateScale = false;
            }
        }
        IO::updateOAM( false );
    }

    void battleUI::animateHitPkmn( bool p_opponent, u8 p_pos, u8 p_effectiveness ) {
        if( p_effectiveness > 100 ) {
            SOUND::playSoundEffect( SFX_BATTLE_DAMAGE_SUPER );
        } else if( p_effectiveness < 100 ) {
            SOUND::playSoundEffect( SFX_BATTLE_DAMAGE_WEAK );
        } else {
            SOUND::playSoundEffect( SFX_BATTLE_DAMAGE_NORMAL );
        }

        for( u8 f = 0; f < 4; ++f ) {
            hidePkmn( p_opponent, p_pos );
            for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
            showPkmn( p_opponent, p_pos );
            for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        }
    }

    u16 getTODBattleBG( u16 p_base ) {
        u8 curDT = getCurrentDaytime( );

        switch( p_base ) {
        case 1:
            if( curDT == DAYTIME_EVENING || curDT == DAYTIME_NIGHT ) { return 200 + p_base; }
            [[fallthrough]];
        case 2:
        case 3:
            if( curDT == DAYTIME_MORNING || curDT == DAYTIME_DUSK ) { return 100 + p_base; }
            [[fallthrough]];
        default: return p_base;
        }
    }

    void battleUI::redrawBattleBG( ) {
        u16 bg = getTODBattleBG( _background );
        if( _currentTerrain != NO_TERRAIN ) { bg = 1000 + u8( _currentTerrain ); }

        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/BATTLE_BACK/",
                             std::to_string( bg ).c_str( ), 480, 49152 );
        u16* pal              = BG_PALETTE;
        pal[ 0 ]              = 0;
        pal[ HP_OUTLINE_COL ] = IO::BLACK;
        pal[ 250 ]            = IO::WHITE;
        pal[ 251 ]            = IO::GRAY;
        pal[ 252 ]            = IO::RGB( 18, 22, 31 );
        pal[ 254 ]            = IO::RGB( 31, 18, 18 );
        pal[ 253 ]            = IO::RGB( 0, 0, 25 );
        pal[ 255 ]            = IO::RGB( 23, 0, 0 );
    }

    void battleUI::init( weather p_initialWeather, terrain p_initialTerrain ) {
        for( u8 i = 0; i < 2; ++i ) {
            u16* pal = IO::BG_PAL( i );
            pal[ 0 ] = 0;
        }
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::vramSetup( true );

        _currentWeather = p_initialWeather;
        _currentTerrain = p_initialTerrain;

        initTop( );
        initSub( );
        redrawBattleBG( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub2", 512, 49152,
                             true );
        for( u8 i = 0; i < 2; ++i ) {
            u16* pal   = IO::BG_PAL( i );
            pal[ 0 ]   = 0;
            pal[ 250 ] = IO::WHITE;
            pal[ 251 ] = IO::GRAY;
            pal[ 252 ] = IO::RGB( 18, 22, 31 );
            pal[ 254 ] = IO::RGB( 31, 18, 18 );
            pal[ 253 ] = IO::RGB( 0, 0, 25 );
            pal[ 255 ] = IO::RGB( 23, 0, 0 );
        }
    }

    void battleUI::deinit( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, false );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        for( u8 i = 0; i < 2; ++i ) {
            u16* pal = IO::BG_PAL( i );
            std::memset( pal, 0, 256 * sizeof( u16 ) );
        }
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 256 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
    }

    void battleUI::resetLog( ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        for( u8 i = 0; i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = false; }
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        IO::updateOAM( true );
        _currentLogLine = 1;
    }

    void battleUI::log( std::string p_message ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        if( oam[ SPR_LARGE_MESSAGE_OAM_SUB ].isHidden ) { resetLog( ); }

        IO::regularFont->setColor( 250, 1 );
        IO::regularFont->setColor( 251, 2 );

        u8 height = IO::regularFont->printBreakingStringC(
            p_message.c_str( ), 16, 24, 256 - 32, true, IO::font::LEFT, 14, ' ', 0, false, -1 );

        if( _currentLogLine + height > 12 ) { _currentLogLine = 1; }

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ) + ( 10 + 14 * _currentLogLine ) * 128,
                      ( 14 * ( height + 2 ) ) * 256 );
        _currentLogLine += IO::regularFont->printBreakingStringC(
            p_message.c_str( ), 16, 10 + 14 * _currentLogLine, 256 - 32, true, IO::font::LEFT, 14,
            ' ', 0, true );

        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
    }

    std::string battleUI::getPkmnName( pokemon* p_pokemon, bool p_opponent,
                                       bool p_sentenceStart ) const {
        char buffer[ 50 ];
        if( p_opponent && _isWildBattle ) {
            if( p_sentenceStart ) {
                snprintf( buffer, 49, GET_STRING( 311 ), p_pokemon->m_boxdata.m_name );
            } else {
                snprintf( buffer, 49, GET_STRING( 309 ), p_pokemon->m_boxdata.m_name );
            }
        } else if( p_opponent ) {
            if( p_sentenceStart ) {
                snprintf( buffer, 49, GET_STRING( 312 ), p_pokemon->m_boxdata.m_name );
            } else {
                snprintf( buffer, 49, GET_STRING( 310 ), p_pokemon->m_boxdata.m_name );
            }
        } else {
            snprintf( buffer, 49, p_pokemon->m_boxdata.m_name );
        }
        return std::string( buffer );
    }

    void battleUI::logBoosts( pokemon* p_pokemon, bool p_opponent, u8 p_slot, boosts p_intended,
                              boosts p_actual ) {
        char buffer[ 100 ];
        auto pkmnstr = getPkmnName( p_pokemon, p_opponent );
        bool up      = false;
        for( u8 i = 0; i < 8; ++i ) {
            if( p_intended.getBoost( i ) > 0 ) {
                auto bt = p_actual.getBoost( i );
                if( bt == 0 ) {
                } else if( bt == 1 ) {
                    up = true;
                } else if( bt == 2 ) {
                    up = true;
                } else if( bt >= 3 ) {
                    up = true;
                }
            }
        }
        if( up ) { animateStatChange( p_opponent, p_slot, false ); }

        for( u8 i = 0; i < 8; ++i ) {
            if( p_intended.getBoost( i ) > 0 ) {
                auto bt = p_actual.getBoost( i );
                if( bt == 0 ) {
                    snprintf( buffer, 99, GET_STRING( 256 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                } else if( bt == 1 ) {
                    snprintf( buffer, 99, GET_STRING( 257 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                } else if( bt == 2 ) {
                    snprintf( buffer, 99, GET_STRING( 258 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                } else if( bt >= 3 ) {
                    snprintf( buffer, 99, GET_STRING( 259 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                }
                log( std::string( buffer ) );
            }
        }

        up = false;
        for( u8 i = 0; i < 8; ++i ) {
            if( p_intended.getBoost( i ) < 0 ) {
                auto bt = p_actual.getBoost( i );
                if( bt == 0 ) {
                } else if( bt == -1 ) {
                    up = true;
                } else if( bt == -2 ) {
                    up = true;
                } else if( bt <= -3 ) {
                    up = true;
                }
            }
        }
        if( up ) { animateStatChange( p_opponent, p_slot, true ); }

        for( u8 i = 0; i < 8; ++i ) {
            if( p_intended.getBoost( i ) < 0 ) {
                auto bt = p_actual.getBoost( i );
                if( bt == 0 ) {
                    snprintf( buffer, 99, GET_STRING( 256 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                } else if( bt == -1 ) {
                    snprintf( buffer, 99, GET_STRING( 260 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                } else if( bt == -2 ) {
                    snprintf( buffer, 99, GET_STRING( 261 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                } else if( bt <= -3 ) {
                    snprintf( buffer, 99, GET_STRING( 262 ), pkmnstr.c_str( ),
                              GET_STRING( 248 + i ), pkmnstr.c_str( ) );
                }
                log( std::string( buffer ) );
            }
        }
        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::logItem( pokemon* p_pokemon, bool p_opponent ) {
        if( !p_pokemon->getItem( ) ) { return; }

        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        SOUND::playSoundEffect( SFX_BATTLE_ABILITY );

        SpriteEntry* oam = IO::OamTop->oamBuffer;

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = false; }
        IO::updateOAM( false );

        char buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( 393 ), p_pokemon->m_boxdata.m_name,
                  ITEM::getItemName( p_pokemon->getItem( ) ).c_str( ),
                  p_pokemon->m_boxdata.m_name );

        IO::regularFont->printStringC( buffer, 128 + ( p_opponent ? 18 : -32 ),
                                       oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 1, false,
                                       p_opponent ? IO::font::LEFT : IO::font::RIGHT, 14, -14 );

        for( u8 i = 0; i < 75; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = true; }
        dmaFillWords(
            0, bgGetGfxPtr( IO::bg2 ) + ( oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 1 ) * 128,
            30 * 256 );
        IO::updateOAM( false );

        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::logAbility( pokemon* p_pokemon, bool p_opponent ) {
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        SOUND::playSoundEffect( SFX_BATTLE_ABILITY );

        SpriteEntry* oam = IO::OamTop->oamBuffer;

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = false; }
        IO::updateOAM( false );

        char buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( 393 ), p_pokemon->m_boxdata.m_name,
                  getAbilityName( p_pokemon->getAbility( ) ).c_str( ),
                  p_pokemon->m_boxdata.m_name );

        IO::regularFont->printStringC( buffer, 128 + ( p_opponent ? 18 : -32 ),
                                       oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 1, false,
                                       p_opponent ? IO::font::LEFT : IO::font::RIGHT, 14, -14 );

        for( u8 i = 0; i < 75; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = true; }
        dmaFillWords(
            0, bgGetGfxPtr( IO::bg2 ) + ( oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 1 ) * 128,
            30 * 256 );
        IO::updateOAM( false );

        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::logForewarn( pokemon* p_pokemon, bool p_opponent, u16 p_move ) {
        char buffer[ 50 ];
        snprintf( buffer, 49, GET_STRING( 396 ), getPkmnName( p_pokemon, p_opponent ).c_str( ),
                  MOVE::getMoveName( p_move ).c_str( ) );
        log( std::string( buffer ) );
    }

    void battleUI::logAnticipation( pokemon* p_pokemon, bool p_opponent ) {
        char buffer[ 50 ];
        snprintf( buffer, 49, GET_STRING( 397 ), getPkmnName( p_pokemon, p_opponent ).c_str( ) );
        log( std::string( buffer ) );
    }

    void battleUI::logFrisk( pokemon* p_pokemon, bool p_opponent, std::vector<u16> p_itms ) {
        char buffer[ 100 ];
        if( p_itms.size( ) == 1 ) {
            snprintf( buffer, 99, GET_STRING( 398 ), getPkmnName( p_pokemon, p_opponent ).c_str( ),
                      ITEM::getItemName( p_itms[ 0 ] ).c_str( ) );
        } else if( p_itms.size( ) == 2 ) {
            snprintf( buffer, 99, GET_STRING( 399 ), getPkmnName( p_pokemon, p_opponent ).c_str( ),
                      ITEM::getItemName( p_itms[ 0 ] ).c_str( ),
                      ITEM::getItemName( p_itms[ 1 ] ).c_str( ) );
        } else {
            return;
        }
        log( std::string( buffer ) );
    }

    void battleUI::updatePkmnStats( bool p_opponent, u8 p_pos, pokemon* p_pokemon, bool p_redraw ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        u16          anchorx
            = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x + ( p_opponent ? -88 : 34 );
        u16 anchory = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y + 4;
        u16 hpx     = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x;
        u16 hpy     = oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y;

        IO::regularFont->setColor( 250, 1 );
        IO::regularFont->setColor( 251, 2 );
        IO::smallFont->setColor( 250, 1 );
        IO::smallFont->setColor( 251, 2 );
        // show/hide hp bar
        bool disabled = p_pokemon == nullptr || ( p_pos && _mode != DOUBLE );
        oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].isHidden = disabled;
        if( p_redraw ) {
            // clear relevant part of the screen
            dmaFillWords( 0,
                          bgGetGfxPtr( IO::bg2 )
                              + ( oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y ) * 128,
                          32 * 256 );
        }

        if( !p_pos || _mode == DOUBLE ) {
            if( !p_opponent ) {
                if( p_pokemon == nullptr ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 3 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, disabled, OBJPRIORITY_0, false );
                } else if( !p_pokemon->canBattle( ) ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 2 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, false, OBJPRIORITY_0, false );
                } else if( p_pokemon->m_statusint ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 1 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, false, OBJPRIORITY_0, false );
                } else {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 0 ), oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( 6 + p_pos ) ].y, 16, 16, 0, 0, 0,
                                    false, false, false, OBJPRIORITY_0, false );
                }
            } else if( !_isWildBattle ) {
                if( p_pokemon == nullptr ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 3 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, disabled, OBJPRIORITY_0, false );
                } else if( !p_pokemon->canBattle( ) ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 2 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                } else if( p_pokemon->m_statusint ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 1 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                } else {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 0 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                }
            } else {
                if( p_pokemon != nullptr && IN_DEX( p_pokemon->getSpecies( ) ) ) {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 0 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                } else {
                    IO::loadSprite( SPR_STATUS_BALL_OAM( p_pos ), SPR_STATUS_BALL_PAL,
                                    SPR_STSBALL_GFX( 3 ), oam[ SPR_STATUS_BALL_OAM( p_pos ) ].x,
                                    oam[ SPR_STATUS_BALL_OAM( p_pos ) ].y, 16, 16, 0, 0, 0, false,
                                    false, false, OBJPRIORITY_0, false );
                }
            }
        }

        if( p_pokemon != nullptr && ( !p_pos || _mode == DOUBLE ) ) {
            // pkmn name
            u16 namewd = IO::regularFont->stringWidth( p_pokemon->m_boxdata.m_name );
            if( namewd > 60 ) {
                IO::regularFont->printStringC( p_pokemon->m_boxdata.m_name, anchorx, anchory,
                                               false );
                namewd = IO::regularFont->stringWidthC( p_pokemon->m_boxdata.m_name );
            } else {
                IO::regularFont->printString( p_pokemon->m_boxdata.m_name, anchorx, anchory,
                                              false );
            }

            // Level
            IO::smallFont->setColor( 0, 0 );
            IO::smallFont->setColor( 250, 1 );
            IO::smallFont->setColor( 251, 2 );
            IO::smallFont->printString( ( "!" + std::to_string( p_pokemon->m_level ) ).c_str( ),
                                        anchorx - 2, anchory + 9, false );

            // Gender
            if( p_pokemon->getSpecies( ) != PKMN_NIDORAN_F
                && p_pokemon->getSpecies( ) != PKMN_NIDORAN_M ) {
                if( p_pokemon->m_boxdata.m_isFemale ) {
                    IO::regularFont->setColor( 254, 1 );
                    IO::regularFont->setColor( 255, 2 );
                    IO::regularFont->printString( "}", anchorx + 2 + namewd, anchory, false );
                    IO::regularFont->setColor( 250, 1 );
                    IO::regularFont->setColor( 251, 2 );
                } else if( !p_pokemon->m_boxdata.m_isGenderless ) {
                    IO::regularFont->setColor( 252, 1 );
                    IO::regularFont->setColor( 253, 2 );
                    IO::regularFont->printString( "{", anchorx + 2 + namewd, anchory, false );
                    IO::regularFont->setColor( 250, 1 );
                    IO::regularFont->setColor( 251, 2 );
                }
            }

            // Hp bars
            if( p_redraw ) {
                IO::displayHP( 0, _curHP[ !p_opponent ][ p_pos ], hpx, hpy,
                               HP_COL( p_opponent, p_pos ), HP_COL( p_opponent, p_pos ) + 1,
                               false );
            }

            IO::smallFont->setColor( HP_COL( p_opponent, p_pos ), 1 );
            IO::smallFont->setColor( HP_OUTLINE_COL, 2 );
            IO::smallFont->setColor( HP_COL( p_opponent, p_pos ) + 1, 3 );
            IO::smallFont->printString(
                HP_ICON, oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x + 10,
                oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y - 6, false ); // HP "icon"

            IO::displayHP( _curHP[ !p_opponent ][ p_pos ],
                           p_pokemon->m_stats.m_curHP * 100 / p_pokemon->m_stats.m_maxHP, hpx, hpy,
                           HP_COL( p_opponent, p_pos ), HP_COL( p_opponent, p_pos ) + 1,
                           !p_redraw && _curHP[ !p_opponent ][ p_pos ] );
            _curHP[ !p_opponent ][ p_pos ]
                = p_pokemon->m_stats.m_curHP * 100 / p_pokemon->m_stats.m_maxHP;

            IO::smallFont->printString(
                HP_ICON, oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].x + 10,
                oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y - 6, false ); // HP "icon"

            // hp

            if( !p_redraw ) {
                IO::printRectangle( anchorx + 96 - 32 - 28, anchory + 16, anchorx + 96 - 6,
                                    anchory + 9 + 15, false, 0 );
            }
            IO::smallFont->setColor( 250, 1 );
            IO::smallFont->setColor( 251, 2 );
            char buffer[ 10 ];
            snprintf( buffer, 8, "%3d", p_pokemon->m_stats.m_curHP );
            IO::smallFont->printString( buffer, anchorx + 96 - 32 - 28, anchory + 9, false );
            snprintf( buffer, 8, "/%d", p_pokemon->m_stats.m_maxHP );
            IO::smallFont->printString( buffer, anchorx + 96 - 32 - 4, anchory + 9, false );
        }

        // Status / shiny
        if( !p_pokemon->m_stats.m_curHP ) {
            IO::loadSprite(
                "STS/status_fnt", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isParalyzed ) {
            IO::loadSprite(
                "STS/status_par", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isAsleep ) {
            IO::loadSprite(
                "STS/status_slp", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBadlyPoisoned ) {
            IO::loadSprite(
                "STS/status_txc", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isBurned ) {
            IO::loadSprite(
                "STS/status_brn", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isFrozen ) {
            IO::loadSprite(
                "STS/status_frz", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else if( p_pokemon->m_status.m_isPoisoned ) {
            IO::loadSprite(
                "STS/status_psn", SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_STATUS_ICON_PAL,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ]
                    .gfxIndex,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].x,
                IO::OamTop->oamBuffer[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].y, 8, 8,
                false, false, false, OBJPRIORITY_0, false, OBJMODE_NORMAL );
        } else {
            oam[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden = true;
        }

        oam[ SPR_SHINY_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden = !p_pokemon->isShiny( );
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden   = disabled;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].isHidden = disabled;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].isHidden = disabled;
        if( disabled ) {
            oam[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden = true;
        }

        IO::updateOAM( false );
    }

    void battleUI::hidePkmnStats( bool p_opponent, u8 p_pos ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].isHidden    = true;
        oam[ SPR_STATUS_BALL_OAM( 6 * !p_opponent + p_pos ) ].isHidden = true;

        // clear relevant part of the screen
        dmaFillWords( 0,
                      bgGetGfxPtr( IO::bg2 )
                          + ( oam[ SPR_HPBAR_OAM + 2 * ( !p_opponent ) + p_pos ].y ) * 128,
                      32 * 256 );

        oam[ SPR_STATUS_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden  = true;
        oam[ SPR_SHINY_ICON_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden   = true;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) ].isHidden     = true;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].isHidden = true;
        oam[ SPR_STATUSBG_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].isHidden = true;

        IO::updateOAM( false );
    }

    void battleUI::updatePkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        if( p_pokemon == nullptr ) {
            hidePkmn( p_opponent, p_pos );
        } else {
            loadPkmnSprite( p_opponent, p_pos, p_pokemon );
        }
        _curHP[ !p_opponent ][ p_pos ] = 0;
        updatePkmnStats( p_opponent, p_pos, p_pokemon );
    }

    void battleUI::showPkmn( bool p_opponent, u8 p_pos ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 4; ++i ) {
            if( !p_opponent ) {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble  = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].rotationIndex = 1;
            } else {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden = false;
            }
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble = true;
        }

        IO::updateOAM( false );
    }

    void battleUI::hidePkmn( bool p_opponent, u8 p_pos ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale = false;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden      = true;
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = false;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden = true;
        }

        IO::updateOAM( false );
    }

    void battleUI::faintPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        char         buffer[ 100 ];
        SOUND::playSoundEffect( SFX_BATTLE_FAINT );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = false;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isHidden = true;
        }

        for( u8 f = 0; f < 24 * ( 1 + !p_opponent ); ++f ) {
            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].y += 1;
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }

        hidePkmn( p_opponent, p_pos );
        hidePkmnStats( p_opponent, p_pos );

        if( p_pokemon != nullptr ) [[likely]] {
            snprintf( buffer, 99, GET_STRING( 289 ),
                      getPkmnName( p_pokemon, p_opponent ).c_str( ) );
        } else {
#ifdef DESQUID
            snprintf( buffer, 99, GET_STRING( 289 ), "[it's a nullptr]" );
#endif
        }
        log( std::string( buffer ) );
    }

    void battleUI::loadPkmnSprite( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        u16 x = ( _mode == SINGLE ) ? ( p_opponent ? PKMN_OPP_1_X_SINGLE : PKMN_OWN_1_X_SINGLE )
                                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_X : PKMN_OPP_1_X )
                                                     : ( p_pos ? PKMN_OWN_2_X : PKMN_OWN_1_X ) ) );
        u16 y = ( _mode == SINGLE ) ? ( p_opponent ? PKMN_OPP_1_Y_SINGLE : PKMN_OWN_1_Y_SINGLE )
                                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_Y : PKMN_OPP_1_Y )
                                                     : ( p_pos ? PKMN_OWN_2_Y : PKMN_OWN_1_Y ) ) );

        y += IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) );

        if( p_opponent ) {
            IO::loadPKMNSprite(
                p_pokemon->getSpecies( ), x, y, SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_PKMN_PAL( 2 * ( !p_opponent ) + p_pos ),
                SPR_PKMN_GFX( 2 * ( !p_opponent ) + p_pos ), false, p_pokemon->isShiny( ),
                p_pokemon->isFemale( ), false, false, p_pokemon->getForme( ) );
        } else {
            IO::loadPKMNSpriteBack(
                p_pokemon->getSpecies( ), x, y, SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ),
                SPR_PKMN_PAL( 2 * ( !p_opponent ) + p_pos ),
                SPR_PKMN_GFX( 2 * ( !p_opponent ) + p_pos ), false, p_pokemon->isShiny( ),
                p_pokemon->isFemale( ), false, false, p_pokemon->getForme( ) );

            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble  = true;
                oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].rotationIndex = 1;
            }
        }
        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].priority = OBJPRIORITY_3;
        }

        u16 emptyPal[ 32 ]                = { 0, 0, 0 };
        IO::OamTop->matrixBuffer[ 2 ].hdx = ( 1LLU << 8 );
        IO::OamTop->matrixBuffer[ 2 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 2 ].vdx = ( 1LLU << 9 );
        IO::OamTop->matrixBuffer[ 2 ].vdy = ( 1LLU << 10 );

        IO::OamTop->matrixBuffer[ 3 ].hdx = 187;
        IO::OamTop->matrixBuffer[ 3 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 3 ].vdx = 187 << 1;
        IO::OamTop->matrixBuffer[ 3 ].vdy = 187 << 2;

        u16 sy = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0 ].y + 8
                 - ( IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) ) / 2 ),
            sx = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0 ].x + 2
                 - 3 * IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) ) / 4;
        s8 diffx = 80, diffy = 44;
        s8 ske = -23;
        if( !p_opponent ) {
            sy += 4;
            diffx = 97;
            diffy = 48;
            ske   = -36;
        }
        IO::loadSprite(
            SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0, SPR_PKMN_SHADOW_PAL,
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 0 ].gfxIndex, sx, sy, 64, 64,
            emptyPal, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite(
            SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 1, SPR_PKMN_SHADOW_PAL,
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].gfxIndex, diffx + sx, sy,
            32, 64, 0, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 2,
                        SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].gfxIndex,
                        ske + sx, diffy + sy, 64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 3,
                        SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 3 ].gfxIndex,
                        ske + diffx + sx, diffy + sy, 32, 32, 0, 0, 0, false, false, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );

        if( !p_opponent ) {
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) ].x -= 48;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) ].y -= 48;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 1 ].y -= 48;
            oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_pos ) + 2 ].x -= 48;
        }

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isRotateScale
                = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].isSizeDouble = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 2 * ( !p_opponent ) + p_pos ) + i ].rotationIndex
                = 3 - p_opponent;
        }

        IO::updateOAM( false );
    }

    void battleUI::startWildBattle( pokemon* p_pokemon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );
        // Load pkmn sprite
        IO::loadPKMNSprite( p_pokemon->getSpecies( ), WILD_BATTLE_SPRITE_X_START,
                            OPP_PLAT_Y + 35 - 96 + IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) ),
                            SPR_PKMN_START_OAM( 0 ), SPR_PKMN_PAL( 0 ), SPR_PKMN_GFX( 0 ), false,
                            p_pokemon->isShiny( ), p_pokemon->isFemale( ), false, false,
                            p_pokemon->getForme( ) );

        for( u8 i = 0; i < 4; ++i ) { oam[ SPR_PKMN_START_OAM( 0 ) + i ].priority = OBJPRIORITY_3; }
        u16 emptyPal[ 32 ]                = { 0, 0, 0 };
        IO::OamTop->matrixBuffer[ 2 ].hdx = ( 1LLU << 8 );
        IO::OamTop->matrixBuffer[ 2 ].hdy = ( 0 << 8 );
        IO::OamTop->matrixBuffer[ 2 ].vdx = ( 1LLU << 9 );
        IO::OamTop->matrixBuffer[ 2 ].vdy = ( 1LLU << 10 );

        u16 sy = oam[ SPR_PKMN_START_OAM( 0 ) + 0 ].y + 8
                 - ( IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) ) / 2 ),
            sx = oam[ SPR_PKMN_START_OAM( 0 ) + 0 ].x + 2
                 - 3 * IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) ) / 4;
        s8 diffx = 80, diffy = 44;
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 0, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 0 ].gfxIndex, sx, sy, 64, 64, emptyPal, 0, 0,
                        false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 1, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 1 ].gfxIndex, diffx + sx, sy, 32, 64, 0, 0,
                        0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 2, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 2 ].gfxIndex, -23 + sx, diffy + sy, 64, 32,
                        0, 0, 0, false, false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM( 0 ) + 3, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM( 0 ) + 3 ].gfxIndex, -23 + diffx + sx, diffy + sy,
                        32, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + i ].isRotateScale = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + i ].isSizeDouble  = true;
            oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + i ].rotationIndex = 2;
        }

        IO::updateOAM( false );

        for( u16 i = WILD_BATTLE_SPRITE_X_START; i < WILD_BATTLE_SPRITE_X; ++i ) {
            for( u8 j = 0; j < 4; ++j ) {
                oam[ SPR_PKMN_START_OAM( 0 ) + j ].x++;
                oam[ SPR_PKMN_SHADOW_START_OAM( 0 ) + j ].x++;
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }

        SOUND::playCry( p_pokemon->getSpecies( ), p_pokemon->getForme( ), p_pokemon->isFemale( ) );

        for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }

        if( p_pokemon->isShiny( ) ) { animateShiny( true, 0, p_pokemon->m_boxdata.m_shinyType ); }
        _curHP[ 0 ][ 0 ] = 0;
        updatePkmnStats( true, 0, p_pokemon );

        char buffer[ 50 ];
        snprintf( buffer, 49, GET_STRING( 394 ), p_pokemon->m_boxdata.m_name );
        log( std::string( buffer ) );
    }

    void battleUI::startTrainerBattle( battleTrainer* p_trainer ) {
        _battleTrainer = p_trainer;

        SpriteEntry* oam = IO::OamTop->oamBuffer;

        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );
        // Load trainer sprite

        IO::loadTrainerSprite( _battleTrainer->m_data.m_trainerBG, WILD_BATTLE_SPRITE_X + 16,
                               OPP_PLAT_Y - 96 + 35, SPR_PKMN_START_OAM( 0 ), SPR_PKMN_PAL( 0 ),
                               SPR_PKMN_GFX( 0 ), false );

        IO::OamTop->matrixBuffer[ 5 ].hdx            = 1 << 7 | 1 << 6 | 1 << 5;
        IO::OamTop->matrixBuffer[ 5 ].vdy            = 1 << 7 | 1 << 6 | 1 << 5;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isRotateScale = true;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isSizeDouble  = true;
        oam[ SPR_PKMN_START_OAM( 0 ) ].rotationIndex = 5;

        IO::updateOAM( false );
        char buffer[ 50 ];
        snprintf( buffer, 49, GET_STRING( 143 ),
                  getTrainerClassName( _battleTrainer->getClass( ) ).c_str( ),
                  _battleTrainer->m_strings.m_name );
        log( std::string( buffer ) );

        // Slide trainer out

        for( u8 i = 40; i > 0; --i ) {
            oam[ SPR_PKMN_START_OAM( 0 ) ].x += 1;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        oam[ SPR_PKMN_START_OAM( 0 ) ].isRotateScale = false;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isSizeDouble  = false;
        oam[ SPR_PKMN_START_OAM( 0 ) ].isHidden      = true;
        IO::updateOAM( false );
    }

    void battleUI::sendOutPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon ) {
        if( p_pokemon != nullptr ) {
            SAVE::SAV.getActiveFile( ).registerSeenPkmn( p_pokemon->getSpecies( ) );
        }
        IO::fadeScreen( IO::UNFADE, true, true );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
        bgUpdate( );

        char buffer[ 100 ];
        if( p_opponent ) {
            snprintf( buffer, 99, GET_STRING( 263 ),
                      getTrainerClassName( _battleTrainer->getClass( ) ).c_str( ),
                      _battleTrainer->m_strings.m_name, p_pokemon->m_boxdata.m_name );
            log( std::string( buffer ) );
        } else {
            snprintf( buffer, 99, GET_STRING( 395 ), p_pokemon->m_boxdata.m_name );
            log( std::string( buffer ) );
        }

        // play pokeball animation
        animateBallRelease( p_opponent, p_pos, p_pokemon->getBall( ) );
        loadPkmnSprite( p_opponent, p_pos, p_pokemon );
        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

        SOUND::playCry( p_pokemon->getSpecies( ), p_pokemon->getForme( ), p_pokemon->isFemale( ) );

        for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }

        if( p_pokemon->isShiny( ) ) {
            animateShiny( p_opponent, p_pos, p_pokemon->m_boxdata.m_shinyType );
        }

        _curHP[ !p_opponent ][ p_pos ] = 0;
        updatePkmnStats( p_opponent, p_pos, p_pokemon );
    }

    void battleUI::recallPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon, bool p_forced ) {
        char buffer[ 100 ];
        if( p_opponent && _battleTrainer != nullptr ) {
            // TODO
            //            snprintf( buffer, 99, GET_STRING( 274 + p_forced ),
            //                    getTrainerClassName( _battleTrainer->getClass( ) ).c_str( ),
            //                    _battleTrainer->m_strings.m_name, p_pokemon->m_boxdata.m_name );
            //            log( std::string( buffer ) );
        } else {
            snprintf( buffer, 99, GET_STRING( 272 + p_forced ), p_pokemon->m_boxdata.m_name );
            log( std::string( buffer ) );
        }

        // Hide pkmn and status
        SOUND::playSoundEffect( SFX_BATTLE_RECALL );
        for( u8 i = 0; i < 10; ++i ) { swiWaitForVBlank( ); }
        hidePkmn( p_opponent, p_pos );
        hidePkmnStats( p_opponent, p_pos );
        for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::prepareMove( pokemon* p_pokemon, bool p_opponent, u8 p_pos, battleMove p_move ) {
        auto pkmnstr  = getPkmnName( p_pokemon, p_opponent );
        bool hidepkmn = true;
        char buffer[ 100 ];
        switch( p_move.m_param ) {
        case M_DIVE:
            snprintf( buffer, 99, GET_STRING( 540 ), pkmnstr.c_str( ) );
            log( buffer );
            break;
        case M_DIG:
            snprintf( buffer, 99, GET_STRING( 541 ), pkmnstr.c_str( ) );
            log( buffer );
            break;
        case M_FLY:
            snprintf( buffer, 99, GET_STRING( 542 ), pkmnstr.c_str( ) );
            log( buffer );
            break;
        case M_BOUNCE:
            snprintf( buffer, 99, GET_STRING( 543 ), pkmnstr.c_str( ) );
            log( buffer );
            break;
        case M_SKY_DROP:
            snprintf( buffer, 99, GET_STRING( 544 ), pkmnstr.c_str( ) );
            log( buffer );
            break;
        case M_PHANTOM_FORCE:
        case M_SHADOW_FORCE:
            snprintf( buffer, 99, GET_STRING( 545 ), pkmnstr.c_str( ) );
            log( buffer );
            break;
        default: hidepkmn = false; break;
        }
        if( hidepkmn ) { hidePkmn( p_opponent, p_pos ); }
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    battleUI::showMoveSelection( pokemon* p_pokemon, u8 p_slot, u8 p_highlightedButton ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        char         buffer[ 100 ];

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        if( p_highlightedButton == u8( -1 ) ) {
            // initialize stuff
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub", 512, 49152,
                                 true );

            for( u8 i = 0; i < 2; ++i ) {
                u16* pal   = IO::BG_PAL( i );
                pal[ 0 ]   = 0;
                pal[ 250 ] = IO::WHITE;
                pal[ 251 ] = IO::GRAY;
                pal[ 252 ] = IO::RGB( 18, 22, 31 );
                pal[ 254 ] = IO::RGB( 31, 18, 18 );
                pal[ 253 ] = IO::RGB( 0, 0, 25 );
                pal[ 255 ] = IO::RGB( 23, 0, 0 );
            }

            // Clear log window
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x06 << 8 );
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x02 << 8 );
            bgUpdate( );

            for( u8 i = 0; i < 8; ++i ) { oam[ SPR_SMALL_MESSAGE_OAM_SUB + i ].isHidden = false; }

            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_BATTLE_FITE_OAM_SUB + i ].isHidden = false;
                oam[ SPR_BATTLE_PKMN_OAM_SUB + i ].isHidden = false;
                oam[ SPR_BATTLE_RUN_OAM_SUB + i ].isHidden  = false;
                oam[ SPR_BATTLE_BAG_OAM_SUB + i ].isHidden  = false;
            }

            if( !_isWildBattle && !p_slot ) {
                IO::loadSprite( "BT/in", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex, 128, 68 + 64 + 18, 32,
                                32, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadSprite( "BT/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex, 128, 68 + 64 + 18, 32,
                                32, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }

            IO::loadPKMNIcon( p_pokemon->getSpecies( ), oam[ SPR_BATTLE_ICON_OAM_SUB ].x,
                              oam[ SPR_BATTLE_ICON_OAM_SUB ].y, SPR_BATTLE_ICON_OAM_SUB,
                              SPR_BATTLE_ICON_PAL_SUB, oam[ SPR_BATTLE_ICON_OAM_SUB ].gfxIndex,
                              true, p_pokemon->getForme( ), p_pokemon->isShiny( ),
                              p_pokemon->isFemale( ) );

            for( u8 i = 0; i < 4; ++i ) {
                oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = true;
                oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
                for( u8 j = 0; j < 6; j++ ) { oam[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = true; }
            }
            IO::updateOAM( true );

            IO::regularFont->printString( GET_STRING( 264 ), 128,
                                          oam[ SPR_BATTLE_FITE_OAM_SUB ].y + 34, true,
                                          IO::font::CENTER );
            IO::regularFont->printStringC( GET_STRING( 265 ), oam[ SPR_BATTLE_PKMN_OAM_SUB ].x + 32,
                                           oam[ SPR_BATTLE_PKMN_OAM_SUB ].y + 9, true,
                                           IO::font::CENTER );
            if( _isWildBattle || !p_slot ) {
                IO::regularFont->printStringC(
                    GET_STRING( 266 ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                    oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true, IO::font::CENTER );
            } else {
                IO::regularFont->printStringC(
                    GET_STRING( 268 ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                    oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true, IO::font::CENTER );
            }
            IO::regularFont->printStringC( GET_STRING( 267 ), oam[ SPR_BATTLE_BAG_OAM_SUB ].x + 32,
                                           oam[ SPR_BATTLE_BAG_OAM_SUB ].y + 9, true,
                                           IO::font::CENTER );

            snprintf( buffer, 99, GET_STRING( 162 ), p_pokemon->m_boxdata.m_name );
            IO::regularFont->printStringC( buffer, 128, oam[ SPR_SMALL_MESSAGE_OAM_SUB ].y + 4,
                                           true, IO::font::CENTER );
        }
        if( p_highlightedButton == 0 ) {
            IO::loadSprite( "BT/ft2", SPR_BATTLE_FITE_OAM_SUB + 1, SPR_BATTLE_FITE_PAL_SUB,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].y, 64, 64, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadSprite( "BT/ft", SPR_BATTLE_FITE_OAM_SUB + 1, SPR_BATTLE_FITE_PAL_SUB,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_FITE_OAM_SUB + 1 ].y, 64, 64, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_FITE_OAM_SUB ].x,
                                                   oam[ SPR_BATTLE_FITE_OAM_SUB ].y,
                                                   oam[ SPR_BATTLE_FITE_OAM_SUB ].x + 128,
                                                   oam[ SPR_BATTLE_FITE_OAM_SUB ].y + 60 ),
                                  0 ) );

        if( p_highlightedButton == 1 ) {
            IO::loadSprite( "BT/pk2", SPR_BATTLE_PKMN_OAM_SUB + 1, SPR_BATTLE_PKMN_PAL_SUB,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadSprite( "BT/pk", SPR_BATTLE_PKMN_OAM_SUB + 1, SPR_BATTLE_PKMN_PAL_SUB,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_PKMN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_PKMN_OAM_SUB ].x,
                                                   oam[ SPR_BATTLE_PKMN_OAM_SUB ].y,
                                                   oam[ SPR_BATTLE_PKMN_OAM_SUB ].x + 64,
                                                   oam[ SPR_BATTLE_PKMN_OAM_SUB ].y + 32 ),
                                  1 ) );

        if( p_highlightedButton == 2 ) {
            if( !p_slot && !_isWildBattle ) {
                IO::loadSprite( "BT/in2", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadSprite( "BT/rn2", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        } else {
            if( !p_slot && !_isWildBattle ) {
                IO::loadSprite( "BT/in", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            } else {
                IO::loadSprite( "bt/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                                oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        }

        if( _isWildBattle ) {
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      2 ) );
        }

        if( p_highlightedButton == 3 ) {
            IO::loadSprite( "BT/bg2", SPR_BATTLE_BAG_OAM_SUB + 1, SPR_BATTLE_BAG_PAL_SUB,
                            oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadSprite( "BT/bg", SPR_BATTLE_BAG_OAM_SUB + 1, SPR_BATTLE_BAG_PAL_SUB,
                            oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_BAG_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_BAG_OAM_SUB ].x,
                                                   oam[ SPR_BATTLE_BAG_OAM_SUB ].y,
                                                   oam[ SPR_BATTLE_BAG_OAM_SUB ].x + 64,
                                                   oam[ SPR_BATTLE_BAG_OAM_SUB ].y + 32 ),
                                  3 ) );
        return res;
    }

    void battleUI::animateCapturePkmn( u16 p_pokeball, u8 p_ticks ) {
        animateBallThrow( 0, ITEM::itemToBall( p_pokeball ) );
        char buffer[ 50 ];
        snprintf( buffer, 49, "PB/%hhu/%hhu_", ITEM::itemToBall( p_pokeball ),
                  ITEM::itemToBall( p_pokeball ) );

        u16 x = IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].x;
        u16 y = IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y;

        hidePkmn( true, 0 );

        x += 3;
        IO::loadSprite( ( std::string( buffer ) + std::to_string( 9 ) ).c_str( ),
                        SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32, false,
                        false, false, OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }

        x += 3;
        IO::loadSprite( ( std::string( buffer ) + std::to_string( 0 ) ).c_str( ),
                        SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32, false,
                        false, false, OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }
        swiWaitForVBlank( );

        SOUND::playSoundEffect( SFX_BATTLE_BALLDROP );
        for( ; y < OPP_PLAT_Y + 14; ++y ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y = y;
            IO::updateOAM( false );
            if( y & 1 ) { swiWaitForVBlank( ); }
        }
        for( ; y >= OPP_PLAT_Y; --y ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y = y;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        SOUND::playSoundEffect( SFX_BATTLE_BALLDROP );
        for( ; y < OPP_PLAT_Y + 14; ++y ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y = y;
            IO::updateOAM( false );
            if( y & 1 ) { swiWaitForVBlank( ); }
        }

        // "ticks"

        for( int i = 0; i < 30; ++i ) swiWaitForVBlank( );
        if( !p_ticks ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
        for( auto i : { 11, 12, 13, 12, 11, 0 } ) {
            IO::loadSprite( ( std::string( buffer ) + std::to_string( i ) ).c_str( ),
                            SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32,
                            false, false, false, OBJPRIORITY_0, false );
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
            IO::updateOAM( false );
            for( u8 j = 0; j < 4; ++j ) { swiWaitForVBlank( ); }
        }
        for( int i = 0; i < 30; ++i ) swiWaitForVBlank( );
        if( p_ticks == 1 ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
        for( auto i : { 14, 15, 16, 15, 14, 0 } ) {
            IO::loadSprite( ( std::string( buffer ) + std::to_string( i ) ).c_str( ),
                            SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32,
                            false, false, false, OBJPRIORITY_0, false );
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
            IO::updateOAM( false );
            for( u8 j = 0; j < 4; ++j ) { swiWaitForVBlank( ); }
        }
        for( int i = 0; i < 30; ++i ) swiWaitForVBlank( );
        if( p_ticks == 2 ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
        for( auto i : { 11, 12, 13, 12, 11, 0 } ) {
            IO::loadSprite( ( std::string( buffer ) + std::to_string( i ) ).c_str( ),
                            SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), x, y, 32, 32,
                            false, false, false, OBJPRIORITY_0, false );
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
            IO::updateOAM( false );
            for( u8 j = 0; j < 4; ++j ) { swiWaitForVBlank( ); }
        }
        for( int i = 0; i < 30; ++i ) swiWaitForVBlank( );
        if( p_ticks == 3 ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_CAPTURE_SUCCESSFUL );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].palette = SPR_PKMN_SHADOW_PAL;
        IO::updateOAM( false );
        for( int i = 0; i < 30; ++i ) swiWaitForVBlank( );
        SOUND::playBGMOneshot( MOD_OS_PKMN_CAPTURE );
        for( int i = 0; i < 150; ++i ) swiWaitForVBlank( );
        SOUND::playBGM( MOD_VICTORY_WILD );
        return;

    BREAK:

        SOUND::playSoundEffect( SFX_BATTLE_RECALL );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = false;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isHidden      = true;
        IO::updateOAM( false );
        showPkmn( true, 0 );
        return;
    }

    void battleUI::animateGetVolatileStatusCondition( pokemon* p_pokemon, bool p_opponent,
                                                      u8 p_slot, volatileStatus p_status ) {
        // TODO
        (void) p_slot;

        auto pkmnstr = getPkmnName( p_pokemon, p_opponent );
        char buffer[ 100 ];

        switch( p_status ) {
        case CONFUSION: snprintf( buffer, 99, GET_STRING( 661 ), pkmnstr.c_str( ) ); break;
        case LASERFOCUS: snprintf( buffer, 99, GET_STRING( 662 ), pkmnstr.c_str( ) ); break;
        case MAGNETRISE: snprintf( buffer, 99, GET_STRING( 663 ), pkmnstr.c_str( ) ); break;
        case AQUARING: snprintf( buffer, 99, GET_STRING( 664 ), pkmnstr.c_str( ) ); break;
        case FOCUSENERGY: snprintf( buffer, 99, GET_STRING( 665 ), pkmnstr.c_str( ) ); break;
        case INGRAIN: snprintf( buffer, 99, GET_STRING( 666 ), pkmnstr.c_str( ) ); break;
        case FORESIGHT:
        case MIRACLEEYE: snprintf( buffer, 99, GET_STRING( 667 ), pkmnstr.c_str( ) ); break;
        case LEECHSEED: snprintf( buffer, 99, GET_STRING( 668 ), pkmnstr.c_str( ) ); break;
        default: return;
        }
        log( buffer );
    }

    void battleUI::animateVolatileStatusCondition( pokemon* p_pokemon, bool p_opponent, u8 p_slot,
                                                   volatileStatus p_status ) {
        // TODO
        (void) p_pokemon;
        (void) p_opponent;
        (void) p_slot;
        (void) p_status;
    }

    void battleUI::animateGetStatusCondition( pokemon* p_pokemon, bool p_opponent, u8 p_slot,
                                              u8 p_status ) {
        (void) p_slot;

        auto pkmnstr = getPkmnName( p_pokemon, p_opponent );
        char buffer[ 100 ];

        if( p_status == POISON ) {
            snprintf( buffer, 99, GET_STRING( 655 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == TOXIC ) {
            snprintf( buffer, 99, GET_STRING( 656 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == BURN ) {
            snprintf( buffer, 99, GET_STRING( 657 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == SLEEP ) {
            snprintf( buffer, 99, GET_STRING( 659 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == FROZEN ) {
            snprintf( buffer, 99, GET_STRING( 658 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == PARALYSIS ) {
            snprintf( buffer, 99, GET_STRING( 660 ), pkmnstr.c_str( ) );
            log( buffer );
        }
    }

    void battleUI::animateStatusCondition( pokemon* p_pokemon, bool p_opponent, u8 p_slot,
                                           u8 p_status ) {
        (void) p_slot;

        auto pkmnstr = getPkmnName( p_pokemon, p_opponent );
        char buffer[ 100 ];

        if( p_status == POISON ) {
            snprintf( buffer, 99, GET_STRING( 529 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == TOXIC ) {
            snprintf( buffer, 99, GET_STRING( 530 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == BURN ) {
            snprintf( buffer, 99, GET_STRING( 531 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == SLEEP ) {
            snprintf( buffer, 99, GET_STRING( 299 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == FROZEN ) {
            snprintf( buffer, 99, GET_STRING( 297 ), pkmnstr.c_str( ) );
            log( buffer );
        }
        if( p_status == PARALYSIS ) {
            snprintf( buffer, 99, GET_STRING( 301 ), pkmnstr.c_str( ) );
            log( buffer );
        }
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    battleUI::showAttackSelection( pokemon* p_pokemon, bool p_canUseMove[ 4 ],
                                   bool p_showMegaEvolution, u8 p_highlightedButton,
                                   bool p_megaButtonActive ) {
        (void) p_megaButtonActive;

        auto         res = std::vector<std::pair<IO::inputTarget, u8>>( );
        SpriteEntry* oam = IO::Oam->oamBuffer;
        char         buffer[ 100 ];

        // hide yn choice boxes here b/c learn move.
        for( u8 i = 0; i < 4; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
            }
        }

        if( p_highlightedButton == u8( -1 ) ) {
            // initialize stuff
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub3", 512,
                                 49152, true );
            u16* pal   = IO::BG_PAL( true );
            pal[ 0 ]   = 0;
            pal[ 250 ] = IO::WHITE;
            pal[ 251 ] = IO::GRAY;
            pal[ 252 ] = IO::RGB( 18, 22, 31 );
            pal[ 254 ] = IO::RGB( 31, 18, 18 );
            pal[ 253 ] = IO::RGB( 0, 0, 25 );
            pal[ 255 ] = IO::RGB( 23, 0, 0 );

            // Clear log window
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            for( u8 i = 0; i < 8; ++i ) { oam[ SPR_SMALL_MESSAGE_OAM_SUB + i ].isHidden = false; }

            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_BATTLE_FITE_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_PKMN_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_RUN_OAM_SUB + i ].isHidden  = false;
                oam[ SPR_BATTLE_BAG_OAM_SUB + i ].isHidden  = true;
            }

            oam[ SPR_BATTLE_ICON_OAM_SUB ].isHidden = true;

            IO::loadSprite( "BT/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex, 128, 68 + 64 + 18, 32, 32,
                            true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            // Moves
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( 0, 2 );
            auto mdatas = std::vector<MOVE::moveData>( );
            for( int i = 0; i < 4; i++ ) {
                oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = !p_pokemon->getMove( i );
                oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = !p_pokemon->getMove( i );
                for( u8 j = 0; j < 6; j++ ) { oam[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = false; }

                res.push_back( std::pair( IO::inputTarget( oam[ SPR_MOVE_OAM_SUB( i ) ].x,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].y,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].x + 96,
                                                           oam[ SPR_MOVE_OAM_SUB( i ) ].y + 32 ),
                                          p_pokemon->getMove( i ) && p_canUseMove[ i ]
                                              ? i
                                              : IO::choiceBox::DISABLED_CHOICE ) );

                if( !p_pokemon->getMove( i ) ) {
                    IO::loadTypeIcon( type::UNKNOWN, oam[ SPR_TYPE_OAM_SUB( i ) ].x,
                                      oam[ SPR_TYPE_OAM_SUB( i ) ].y, SPR_TYPE_OAM_SUB( i ),
                                      SPR_TYPE_PAL_SUB( i ), oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                      true, CURRENT_LANGUAGE );
                    oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden = true;
                    continue;
                }
                auto mdata = MOVE::getMoveData( p_pokemon->getMove( i ) );
                mdatas.push_back( mdata );

                type t;
                if( p_pokemon->getMove( i ) != M_HIDDEN_POWER ) {
                    t = mdata.m_type;
                } else {
                    t = p_pokemon->getHPType( );
                }
                IO::loadTypeIcon( t, oam[ SPR_TYPE_OAM_SUB( i ) ].x, oam[ SPR_TYPE_OAM_SUB( i ) ].y,
                                  SPR_TYPE_OAM_SUB( i ), SPR_TYPE_PAL_SUB( i ),
                                  oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex, true, CURRENT_LANGUAGE );
                IO::loadDamageCategoryIcon( mdata.m_category, oam[ SPR_DMG_CAT_OAM_SUB( i ) ].x,
                                            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].y,
                                            SPR_DMG_CAT_OAM_SUB( i ), SPR_DMG_CAT_PAL_SUB( i ),
                                            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].gfxIndex, true );
                IO::copySpritePal( MOVEBOX4_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
            }

            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      4 ) );
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      IO::choiceBox::BACK_CHOICE ) );

            // Mega button
            if( p_showMegaEvolution ) {
                res.push_back( std::pair( IO::inputTarget( 0, 0, 0 ), 5 ) );
            }

            IO::updateOAM( true );

            IO::regularFont->printStringC( GET_STRING( 268 ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                                           oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true,
                                           IO::font::CENTER );

            IO::regularFont->printStringC( GET_STRING( 49 ), 128,
                                           oam[ SPR_SMALL_MESSAGE_OAM_SUB ].y + 4, true,
                                           IO::font::CENTER );

            for( int i = 0; i < 4; i++ ) {
                if( !p_pokemon->getMove( i ) ) continue;
                auto mname = MOVE::getMoveName( p_pokemon->getMove( i ) );
                if( mname.length( ) > 18 ) {
                    snprintf( buffer, 20, "%s.", mname.c_str( ) );
                } else {
                    snprintf( buffer, 20, "%s", mname.c_str( ) );
                }

                if( !p_canUseMove[ i ] ) {
                    IO::regularFont->setColor( 255, 1 );
                } else {
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                }
                IO::regularFont->printStringC( buffer, oam[ SPR_MOVE_OAM_SUB( i ) ].x + 48,
                                               oam[ SPR_MOVE_OAM_SUB( i ) ].y + 7, true,
                                               IO::font::CENTER );

                snprintf(
                    buffer, 49, GET_STRING( 377 ), p_pokemon->m_boxdata.m_curPP[ i ],
                    s8( mdatas[ i ].m_pp * ( ( 5 + p_pokemon->m_boxdata.PPupget( i ) ) / 5.0 ) ) );
                IO::smallFont->printStringC( buffer, oam[ SPR_MOVE_OAM_SUB( i ) ].x + 84,
                                             oam[ SPR_MOVE_OAM_SUB( i ) ].y + 13, true,
                                             IO::font::RIGHT );
            }
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        }

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = !p_pokemon->getMove( i );
            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = !p_pokemon->getMove( i );
            IO::copySpritePal( MOVEBOX4_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
        }

        if( p_highlightedButton < 4 ) {
            IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( p_highlightedButton ), 4, 2 * 4,
                               true );
        }

        if( p_highlightedButton == 4 ) {
            IO::loadSprite( "BT/rn2", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadSprite( "BT/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        IO::updateOAM( true );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    battleUI::showTargetSelection( u8 p_choices, bool p_hasChoice,
                                   std::function<pokemon*( bool, u8 )> p_getPkmn,
                                   u8                                  p_highlightedButton ) {

        auto         res = std::vector<std::pair<IO::inputTarget, u8>>( );
        SpriteEntry* oam = IO::Oam->oamBuffer;
        char         buffer[ 100 ];

        // hide yn choice boxes here b/c learn move.
        for( u8 i = 0; i < 4; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
            }
        }

        if( p_highlightedButton == u8( -1 ) ) {
            // initialize stuff
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub3", 512,
                                 49152, true );
            u16* pal   = IO::BG_PAL( true );
            pal[ 0 ]   = 0;
            pal[ 250 ] = IO::WHITE;
            pal[ 251 ] = IO::GRAY;
            pal[ 252 ] = IO::RGB( 18, 22, 31 );
            pal[ 254 ] = IO::RGB( 31, 18, 18 );
            pal[ 253 ] = IO::RGB( 0, 0, 25 );
            pal[ 255 ] = IO::RGB( 23, 0, 0 );

            // Clear log window
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            for( u8 i = 0; i < 8; ++i ) { oam[ SPR_SMALL_MESSAGE_OAM_SUB + i ].isHidden = false; }

            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_BATTLE_FITE_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_PKMN_OAM_SUB + i ].isHidden = true;
                oam[ SPR_BATTLE_RUN_OAM_SUB + i ].isHidden  = false;
                oam[ SPR_BATTLE_BAG_OAM_SUB + i ].isHidden  = true;
            }

            oam[ SPR_BATTLE_ICON_OAM_SUB ].isHidden = true;

            IO::loadSprite( "BT/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex, 128, 68 + 64 + 18, 32, 32,
                            true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( 0, 2 );
            for( int i = 0; i < 4; i++ ) {
                oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = true;
                oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
                for( u8 j = 0; j < 6; j++ ) { oam[ SPR_MOVE_OAM_SUB( i ) + j ].isHidden = false; }

                res.push_back( std::pair(
                    IO::inputTarget( oam[ SPR_MOVE_OAM_SUB( i ) ].x, oam[ SPR_MOVE_OAM_SUB( i ) ].y,
                                     oam[ SPR_MOVE_OAM_SUB( i ) ].x + 96,
                                     oam[ SPR_MOVE_OAM_SUB( i ) ].y + 32 ),
                    !p_hasChoice || ( p_choices & ( 1 << i ) ) ? i
                                                               : IO::choiceBox::DISABLED_CHOICE ) );

                if( !( p_choices & ( 1 << i ) ) ) {
                    IO::loadTypeIcon( type::NORMAL, oam[ SPR_TYPE_OAM_SUB( i ) ].x,
                                      oam[ SPR_TYPE_OAM_SUB( i ) ].y, SPR_TYPE_OAM_SUB( i ),
                                      SPR_TYPE_PAL_SUB( i ), oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                      true, CURRENT_LANGUAGE );
                } else {
                    IO::loadTypeIcon( type::WATER, oam[ SPR_TYPE_OAM_SUB( i ) ].x,
                                      oam[ SPR_TYPE_OAM_SUB( i ) ].y, SPR_TYPE_OAM_SUB( i ),
                                      SPR_TYPE_PAL_SUB( i ), oam[ SPR_TYPE_OAM_SUB( i ) ].gfxIndex,
                                      true, CURRENT_LANGUAGE );
                }
            }

            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      4 ) );
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_BATTLE_RUN_OAM_SUB ].x,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 64,
                                                       oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 32 ),
                                      IO::choiceBox::BACK_CHOICE ) );

            IO::updateOAM( true );

            IO::regularFont->printStringC( GET_STRING( 268 ), oam[ SPR_BATTLE_RUN_OAM_SUB ].x + 32,
                                           oam[ SPR_BATTLE_RUN_OAM_SUB ].y + 9, true,
                                           IO::font::CENTER ); // "BACK"

            IO::regularFont->printStringC( GET_STRING( 166 ), 128,
                                           oam[ SPR_SMALL_MESSAGE_OAM_SUB ].y + 4, true,
                                           IO::font::CENTER ); // "Choose a target"

            for( int i = 0; i < 4; i++ ) {
                if( !( p_choices & ( 1 << i ) ) ) { continue; }

                auto pk = p_getPkmn( i < 2, ( i < 2 ) ^ ( i % 2 ) );
                if( pk == nullptr ) { continue; }

                snprintf( buffer, 20, "%s", pk->m_boxdata.m_name );

                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->printStringC( buffer, oam[ SPR_MOVE_OAM_SUB( i ) ].x + 48,
                                               oam[ SPR_MOVE_OAM_SUB( i ) ].y + 7, true,
                                               IO::font::CENTER );
            }
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        }

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden    = true;
            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
            IO::copySpritePal( MOVEBOX4_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
        }

        if( p_highlightedButton < 4 && p_hasChoice ) {
            IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( p_highlightedButton ), 4, 2 * 4,
                               true );
        }

        // If the selection is forced, the actual selection of the player doesn't matter.
        if( !p_hasChoice && p_highlightedButton < 4 ) {
            for( u8 i = 0; i < 4; ++i ) {
                if( p_choices & ( 1 << i ) ) {
                    IO::copySpritePal( MOVEBOX3_SPR_PAL, SPR_TYPE_PAL_SUB( i ), 4, 2 * 4, true );
                }
            }
        }

        if( p_highlightedButton == 4 ) {
            IO::loadSprite( "BT/rn2", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        } else {
            IO::loadSprite( "BT/rn", SPR_BATTLE_RUN_OAM_SUB + 1, SPR_BATTLE_RUN_PAL_SUB,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].gfxIndex,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].x,
                            oam[ SPR_BATTLE_RUN_OAM_SUB + 1 ].y, 32, 32, true, true, false,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        IO::updateOAM( true );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    battleUI::printYNMessage( u8 p_selection ) {

        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> res
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 4; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( ( i & 1 ) == ( p_selection & 1 ) ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }
        for( u8 i = 2; i < 4; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
            }
        }

        if( p_selection >= 254 ) {
            for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = true; }

            IO::regularFont->printString(
                GET_STRING( 80 ), oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 32 ),
                           IO::yesNoBox::YES ) );

            IO::regularFont->printString(
                GET_STRING( 81 ), oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 32 ),
                           IO::yesNoBox::NO ) );
        }

        IO::updateOAM( true );
        return res;
    }

    void battleUI::printTopMessage( const char* p_message, bool p_init ) {
        if( p_init ) {
            IO::loadSprite( "UI/mbox1", SPR_MBOX_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 1 ), 0,
                            192 - 46, 32, 64, false, false, false, OBJPRIORITY_3, false );
            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSprite( SPR_MBOX_START_OAM + 13 - i, SPR_BALL_PAL, SPR_PKMN_GFX( 1 ),
                                32 + 16 * i, 192 - 46, 32, 64, 0, 0, 0, false, true, false,
                                OBJPRIORITY_3, false );
            }

            IO::updateOAM( false );
            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
        }

        if( p_message ) {
            IO::printRectangle( 0, 192 - 46, 255, 192, false, 0 );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
            BG_PALETTE[ IO::GRAY_IDX ]  = IO::GRAY;
            IO::regularFont->printBreakingStringC( p_message, 12, 192 - 40, 232, false,
                                                   IO::font::LEFT, 16, ' ', 0, true );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        }
    }

    void battleUI::hideTopMessage( ) {
        IO::printRectangle( 0, 192 - 46, 255, 192, false, 0 );
        for( u8 i = 0; i < 14; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MBOX_START_OAM + i ].isHidden = true;
        }
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 0 ].isHidden = true;
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 1 ].isHidden = true;
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 2 ].isHidden = true;
        IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 3 ].isHidden = true;
        IO::updateOAM( false );
    }

    void battleUI::showTopMessagePkmn( pokemon* p_pokemon ) {
        init( _currentWeather, _currentTerrain );
        IO::initOAMTable( false );

        u16 x = 80;
        u8  y = 48;

        IO::loadPKMNSprite( p_pokemon->getSpecies( ), x, y, SPR_PKMN_START_OAM( 0 ),
                            SPR_PKMN_PAL( 0 ), SPR_PKMN_GFX( 0 ), false, p_pokemon->isShiny( ),
                            p_pokemon->isFemale( ), false, false, p_pokemon->getForme( ) );
    }

    void battleUI::handleCapture( pokemon* p_pokemon ) {
        char buffer[ 100 ];

        showTopMessagePkmn( p_pokemon );
        printTopMessage( 0, true );

        IO::yesNoBox yn;
        snprintf( buffer, 99, GET_STRING( 141 ), p_pokemon->m_boxdata.m_name );
        printTopMessage( buffer, false );

        if( yn.getResult( [ & ]( ) { return printYNMessage( 254 ); },
                          [ & ]( IO::yesNoBox::selection p_selection ) {
                              printYNMessage( p_selection == IO::yesNoBox::NO );
                          } )
            == IO::yesNoBox::YES ) {
            IO::keyboard kbd;
            printTopMessage( GET_STRING( 142 ), false );
            auto nick = kbd.getText( 10 );
            if( strcmp( nick.c_str( ), p_pokemon->m_boxdata.m_name )
                && strcmp( "", nick.c_str( ) ) ) {
                strcpy( p_pokemon->m_boxdata.m_name, nick.c_str( ) );
                p_pokemon->m_boxdata.setIsNicknamed( true );
            }
        }
        hideTopMessage( );
        initSub( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub2", 512, 49152,
                             true );
        u16* pal   = IO::BG_PAL( true );
        pal[ 0 ]   = 0;
        pal[ 250 ] = IO::WHITE;
        pal[ 251 ] = IO::GRAY;
        pal[ 252 ] = IO::RGB( 18, 22, 31 );
        pal[ 254 ] = IO::RGB( 31, 18, 18 );
        pal[ 253 ] = IO::RGB( 0, 0, 25 );
        pal[ 255 ] = IO::RGB( 23, 0, 0 );
        resetLog( );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
    }

    void battleUI::handleBattleEnd( bool p_playerWon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < 2; ++j ) {
                hidePkmn( i, j );
                hidePkmnStats( i, j );
            }
        }
        resetLog( );
        IO::loadTrainerSprite( _battleTrainer->m_data.m_trainerBG, WILD_BATTLE_SPRITE_X + 16,
                               OPP_PLAT_Y - 96 + 35, SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ),
                               SPR_PKMN_PAL( 2 * ( !p_playerWon ) ),
                               SPR_PKMN_GFX( 2 * ( !p_playerWon ) ), false );
        IO::OamTop->matrixBuffer[ 5 ].hdx                               = 1 << 7 | 1 << 6 | 1 << 5;
        IO::OamTop->matrixBuffer[ 5 ].vdy                               = 1 << 7 | 1 << 6 | 1 << 5;
        oam[ SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ) ].isRotateScale = true;
        oam[ SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ) ].isSizeDouble  = true;
        oam[ SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ) ].rotationIndex = 5;

        oam[ SPR_PLATFORM_OAM + 2 ].priority = OBJPRIORITY_3;
        oam[ SPR_PLATFORM_OAM + 3 ].priority = OBJPRIORITY_3;

        IO::loadSprite( "UI/mbox1", SPR_MBOX_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 2 * ( !p_playerWon ) + 1 ), 0, 192 - 46, 32, 64, false, false,
                        false, OBJPRIORITY_3, false );
        for( u8 i = 0; i < 13; ++i ) {
            IO::loadSprite( SPR_MBOX_START_OAM + 13 - i, SPR_BALL_PAL,
                            SPR_PKMN_GFX( 2 * ( !p_playerWon ) + 1 ), 32 + 16 * i, 192 - 46, 32, 64,
                            0, 0, 0, false, true, false, OBJPRIORITY_3, false );
        }
        IO::updateOAM( false );

        // Cut the message after the first "page"
        for( char* pos = p_playerWon ? _battleTrainer->m_strings.m_message2
                                     : _battleTrainer->m_strings.m_message3;
             *pos; ++pos ) {
            if( *pos == '\r' ) {
                *pos = 0;
                break;
            }
        }

        printTopMessage( p_playerWon ? _battleTrainer->m_strings.m_message2
                                     : _battleTrainer->m_strings.m_message3,
                         false );

        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        IO::printRectangle( 0, 192 - 46, 255, 192, false, 0 );
        for( u8 i = 0; i <= 13; ++i ) { oam[ SPR_MBOX_START_OAM + 13 - i ].isHidden = true; }
        IO::updateOAM( false );
    }

    void battleUI::setNewWeather( weather p_newWeather ) {
        log( GET_STRING( 491 + u8( p_newWeather ) ) );
        _currentWeather = p_newWeather;
    }

    void battleUI::continueWeather( ) {
        if( _currentWeather == NO_WEATHER ) { return; }

        log( GET_STRING( 500 + u8( _currentWeather ) - 1 ) );
    }

    void battleUI::addPseudoWeather( u8 p_pwIdx ) {
        log( GET_STRING( 513 + u8( p_pwIdx ) ) );
    }

    void battleUI::removePseudoWeather( u8 p_pwIdx ) {
        log( GET_STRING( 521 + u8( p_pwIdx ) ) );
    }

    void battleUI::setNewTerrain( terrain p_newTerrain ) {
        _currentTerrain = p_newTerrain;
        redrawBattleBG( );
        log( GET_STRING( 508 + u8( p_newTerrain ) ) );
    }
} // namespace BATTLE
