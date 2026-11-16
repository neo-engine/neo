/*
Pokémon neo
------------------------------

file        : battleUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2026
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

#include "bag/bagViewer.h"
#include "bag/item.h"
#include "battle/ability.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/moveNames.h"
#include "gen/pokemonNames.h"
#include "io/kana.h"
#include "io/keyboard.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/simpleWidget.h"
#include "io/sprite.h"
#include "io/util.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace BATTLE {
    void battleUI::initTop( ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( false );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), COMPLETE_SCREEN );

        u16 tileCnt = SPR_PKMN_GFX( 4 ) + 16;

        // status ball
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLEBALL1_START, SPR_STATUS_BALL_OAM( 0 ),
                                  SPR_STATUS_BALL_PAL, tileCnt, OPP_1_HP_X + 8, OPP_1_HP_Y + 8, 16,
                                  16, false, false, true, OBJPRIORITY_0, false );
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLEBALL2_START, SPR_STATUS_BALL_OAM( 1 ),
                                  SPR_STATUS_BALL_PAL, tileCnt, OPP_2_HP_X + 8, OPP_2_HP_Y + 8, 16,
                                  16, false, false, true, OBJPRIORITY_0, false );
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLEBALL3_START, SPR_STATUS_BALL_OAM( 6 ),
                                  SPR_STATUS_BALL_PAL, tileCnt, OWN_1_HP_X + 8, OWN_1_HP_Y + 8, 16,
                                  16, false, false, true, OBJPRIORITY_0, false );
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLEBALL4_START, SPR_STATUS_BALL_OAM( 7 ),
                                  SPR_STATUS_BALL_PAL, tileCnt, OWN_2_HP_X + 8, OWN_2_HP_Y + 8, 16,
                                  16, false, false, true, OBJPRIORITY_0, false );

        for( u8 i = 1 + ( _mode == BM_DOUBLE ); i < 6; ++i ) {
            IO::loadSprite( SPR_STATUS_BALL_OAM( i ), SPR_STATUS_BALL_PAL, SPR_STSBALL_GFX( 0 ),
                            4 + 16 * ( i - 1 - ( _mode == BM_DOUBLE ) ), -2, 16, 16, 0, 0, 0, false,
                            false, true, OBJPRIORITY_0, false );
            IO::loadSprite( SPR_STATUS_BALL_OAM( 6 + i ), SPR_STATUS_BALL_PAL, SPR_STSBALL_GFX( 0 ),
                            156 + 16 * i, 181, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_0,
                            false );
        }
        if( _mode == BM_SINGLE ) {
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
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLE_NAMEBG_START, SPR_STATUSBG_OAM( 0 ),
                                  SPR_STATUSBG_PAL, tileCnt, OPP_1_HP_X - 96, OPP_1_HP_Y, 64, 32,
                                  false, false, true, OBJPRIORITY_3, false, OBJMODE_BLENDED );
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
        tileCnt = IO::loadUIIcon( IO::ICON::HPBAR_BATTLE_OPP_START, SPR_HPBAR_OAM + 1,
                                  SPR_HPBAR_PAL, tileCnt, OPP_2_HP_X, OPP_2_HP_Y, 32, 32, false,
                                  false, true, OBJPRIORITY_3, false, OBJMODE_BLENDED );

        // player
        if( _mode == BM_DOUBLE ) {
            IO::loadSprite( SPR_HPBAR_OAM + 2, SPR_HPBAR_PAL + 1, tileCnt, OWN_1_HP_X, OWN_1_HP_Y,
                            32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, false,
                            OBJMODE_BLENDED );
            tileCnt = IO::loadUIIcon( IO::ICON::HPBAR_BATTLE_OPP_START, SPR_HPBAR_OAM + 3,
                                      SPR_HPBAR_PAL + 1, tileCnt, OWN_2_HP_X, OWN_2_HP_Y, 32, 32,
                                      false, false, true, OBJPRIORITY_3, false, OBJMODE_BLENDED );

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
            tileCnt = IO::loadUIIcon( IO::ICON::HPBAR_BATTLE_OPP_START, SPR_HPBAR_OAM + 2,
                                      SPR_HPBAR_PAL + 1, tileCnt, OWN_2_HP_X, OWN_2_HP_Y, 32, 32,
                                      false, false, true, OBJPRIORITY_3, false, OBJMODE_BLENDED );

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
        tileCnt = IO::loadUIIcon( IO::ICON::STATUS_SHINY_START, SPR_SHINY_ICON_OAM( 0 ),
                                  SPR_SHINY_ICON_PAL, tileCnt, 0, 0, 8, 8, false, false, true,
                                  OBJPRIORITY_0, false, OBJMODE_NORMAL );

        // Shiny / Status icon
        for( u8 i = 0; i < 4; ++i ) {
            u16 anchorx
                = IO::OamTop->oamBuffer[ SPR_HPBAR_OAM + i ].x + ( ( i < 2 ) ? -8 : 34 + 82 );
            u16 anchory = IO::OamTop->oamBuffer[ SPR_HPBAR_OAM + i ].y + 4;
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM( i ), SPR_STATUS_ICON_PAL, tileCnt,
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
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLE_ABILITY_START, SPR_ABILITY_OAM( 0 ), SPR_BOX_PAL,
                                  tileCnt, 128, 64, 64, 32, true, true, true, OBJPRIORITY_3, false,
                                  OBJMODE_NORMAL );

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
        tileCnt = IO::loadUIIcon( IO::ICON::SHINY_BIG_START, SPR_SHINY_PARTICLE_START_OAM + 15,
                                  SPR_SHINY_ICON_PAL, tileCnt, 0, 0, 16, 16, false, false, true,
                                  OBJPRIORITY_0, false, OBJMODE_BLENDED );

        // stat increase / decrease
        for( u8 i = 1; i < 15; ++i ) {
            IO::loadSprite( SPR_STAT_CHANGE_PARTICLE_START_OAM + i, SPR_STAT_CHANGE_PAL, tileCnt, 0,
                            0, 8, 8, 0, 0, 0, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        }
        tileCnt = IO::loadUIIcon( IO::ICON::STAT_UP_START, SPR_STAT_CHANGE_PARTICLE_START_OAM,
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

        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), COMPLETE_SCREEN );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), COMPLETE_SCREEN_SQ );
        bgUpdate( );

        // Load sprites
        // move selection

        IO::loadSprite( SPR_BATTLE_FITE_OAM_SUB, SPR_BATTLE_FITE_PAL_SUB, tileCnt, 128 - 64, 68, 64,
                        64, 0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLE_FITE_START, SPR_BATTLE_FITE_OAM_SUB + 1,
                                  SPR_BATTLE_FITE_PAL_SUB, tileCnt, 128, 64, 64, 64, true, true,
                                  true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        IO::loadSprite( SPR_BATTLE_PKMN_OAM_SUB, SPR_BATTLE_PKMN_PAL_SUB, tileCnt,
                        128 - 32 - 16 - 64 - 6, 68 + 64 + 18, 32, 32, 0, 0, 0, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_BLENDED );
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLE_PKMN_START, SPR_BATTLE_PKMN_OAM_SUB + 1,
                                  SPR_BATTLE_PKMN_PAL_SUB, tileCnt, 128 - 32 - 16 - 32 - 6,
                                  68 + 64 + 18, 32, 32, true, true, true, OBJPRIORITY_3, true,
                                  OBJMODE_BLENDED );

        IO::loadSprite( SPR_BATTLE_RUN_OAM_SUB, SPR_BATTLE_RUN_PAL_SUB, tileCnt, 128 - 32,
                        68 + 64 + 18, 32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadUIIcon( IO::ICON::BATTLE_RUN_START, SPR_BATTLE_RUN_OAM_SUB + 1,
                                  SPR_BATTLE_RUN_PAL_SUB, tileCnt, 128, 68 + 64 + 18, 32, 32, true,
                                  true, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        IO::loadSprite( SPR_BATTLE_BAG_OAM_SUB, SPR_BATTLE_BAG_PAL_SUB, tileCnt, 128 + 32 + 16 + 6,
                        68 + 64 + 18, 32, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
        tileCnt
            = IO::loadUIIcon( IO::ICON::BATTLE_BAG_START, SPR_BATTLE_BAG_OAM_SUB + 1,
                              SPR_BATTLE_BAG_PAL_SUB, tileCnt, 128 + 32 + 16 + 32 + 6, 68 + 64 + 18,
                              32, 32, true, true, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // message boxes

        // small (move / attack selection)
        for( u8 i = 0; i < 7; ++i ) {
            IO::loadSprite( SPR_SMALL_MESSAGE_OAM_SUB + 7 - i, SPR_BOX_PAL_SUB, tileCnt,
                            128 - 32 - 64 + 28 * i, 68 - 18 - 26, 32, 32, 0, 0, 0, false, true,
                            true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        tileCnt = IO::loadUIIcon( IO::ICON::MESSAGE_24_START, SPR_SMALL_MESSAGE_OAM_SUB,
                                  SPR_BOX_PAL_SUB, tileCnt, 128 - 24 - 16 - 64, 68 - 18 - 26, 32,
                                  32, false, false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // large (battle log)

        for( u8 i = 0; i < 4; ++i ) {
            for( u8 j = 0; j < 3; ++j ) {
                IO::loadSprite( SPR_LARGE_MESSAGE_OAM_SUB + 3 + 4 * j - i, SPR_BOX_PAL_SUB, tileCnt,
                                8 + 58 * i, 20 + 49 * j, 64, 64, 0, 0, 0, j == 2, i, true,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        }
        tileCnt = IO::loadUIIcon( IO::ICON::MESSAGE_LARGE_START, SPR_LARGE_MESSAGE_OAM_SUB + 3,
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
                    = IO::loadUIIcon( IO::ICON::MOVEBOX4_START, SPR_MOVE_OAM_SUB( pos ),
                                      SPR_TYPE_PAL_SUB( pos ), tileCnt, 31, 44 + i * 42 + sy, 16,
                                      32, false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
                tileCnt = IO::loadUIIcon( IO::ICON::MOVEBOX2_START, SPR_MOVE_OAM_SUB( pos ) + 1,
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
            tileCnt = IO::loadTypeIcon( TYPE_UNKNOWN, oam[ SPR_MOVE_OAM_SUB( i ) + 1 ].x,
                                        oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_TYPE_OAM_SUB( i ),
                                        SPR_TYPE_PAL_SUB( i ), tileCnt, true, CURRENT_LANGUAGE );
            oam[ SPR_TYPE_OAM_SUB( i ) ].isHidden = true;
        }

        // damage cat icons
        for( u8 i = 0; i < 4; ++i ) {
            tileCnt
                = IO::loadTypeIcon( TYPE_UNKNOWN, oam[ SPR_MOVE_OAM_SUB( i ) + 1 ].x + 32,
                                    oam[ SPR_MOVE_OAM_SUB( i ) ].y - 8, SPR_DMG_CAT_OAM_SUB( i ),
                                    SPR_DMG_CAT_PAL_SUB( i ), tileCnt, true, CURRENT_LANGUAGE );
            oam[ SPR_DMG_CAT_OAM_SUB( i ) ].isHidden = true;
        }

        // choice boxes

        for( u8 i = 0; i < 2; i++ ) {
            u8 pos = 2 * i;
            if( !i ) {
                tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_1_START,
                                          SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB, tileCnt,
                                          29, 42 + i * 36, 16, 32, false, false, true,
                                          OBJPRIORITY_3, true, OBJMODE_BLENDED );
                tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_2_START,
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
        if( _currentTerrain != TR_NONE ) { bg = 1000 + u8( _currentTerrain ); }

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
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), COMPLETE_SCREEN_SQ );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), COMPLETE_SCREEN_SQ );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), COMPLETE_SCREEN_SQ );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), COMPLETE_SCREEN_SQ );
    }

    void battleUI::resetLog( ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        for( u8 i = 0; i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < 12; ++i ) { oam[ SPR_LARGE_MESSAGE_OAM_SUB + i ].isHidden = false; }
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), COMPLETE_SCREEN_SQ );
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
        std::array<char, 50> buffer{ };
        std::string          fmt = "%s";
        if( p_opponent && _isWildBattle ) {
            if( p_sentenceStart ) {
                fmt = std::string( GET_STRING( IO::STR_BATTLE_WILD_NAME_UPPER ) );
            } else {
                fmt = std::string( GET_STRING( IO::STR_BATTLE_WILD_NAME_LOWER ) );
            }
        } else if( p_opponent ) {
            if( p_sentenceStart ) {
                fmt = std::string( GET_STRING( IO::STR_BATTLE_FOE_NAME_UPPER ) );
            } else {
                fmt = std::string( GET_STRING( IO::STR_BATTLE_FOE_NAME_LOWER ) );
            }
        }
        snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ) );
        return std::string( buffer.data( ) );
    }

    void battleUI::logBoosts( pokemon* p_pokemon, bool p_opponent, u8 p_slot, boosts p_intended,
                              boosts p_actual ) {
        std::array<char, 100> buffer{ };
        auto                  pkmnstr = getPkmnName( p_pokemon, p_opponent );
        bool                  up      = false;
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
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_UNCHANGED ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                } else if( bt == 1 ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_UP ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                } else if( bt == 2 ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_UP_SHARPLY ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                } else if( bt >= 3 ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_UP_DRASTICALLY ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                }
                log( std::string( buffer.data( ) ) );
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
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_UNCHANGED ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                } else if( bt == -1 ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_DOWN ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                } else if( bt == -2 ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_DOWN_HARSHLY ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                } else if( bt <= -3 ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_BOOST_DOWN_SEVERELY ) );
                    snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), pkmnstr.c_str( ),
                              GET_STRING( IO::STR_BATTLE_STAT_HP + i ), pkmnstr.c_str( ) );
                }
                log( std::string( buffer.data( ) ) );
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

        std::array<char, 100> buffer{ };
        snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_ABILITY_BANNER ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ),
                  FS::getItemName( p_pokemon->getItem( ) ).c_str( ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ) );

        IO::regularFont->printStringC( buffer.data( ), 128 + ( p_opponent ? 18 : -32 ),
                                       oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 1, false,
                                       p_opponent ? IO::font::LEFT : IO::font::RIGHT, 14, -14 );

        for( u8 i = 0; i < 75; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = true; }
        IO::printRectangle( ( p_opponent ? 128 : 0 ), oam[ SPR_ABILITY_OAM( !p_opponent ) ].y,
                            ( p_opponent ? 255 : 127 ),
                            oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 32, false, 0 );
        IO::updateOAM( false );

        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::logAbility( pokemon* p_pokemon, bool p_opponent ) {
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        SOUND::playSoundEffect( SFX_BATTLE_ABILITY );

        SpriteEntry* oam = IO::OamTop->oamBuffer;

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = false; }
        IO::updateOAM( false );

        std::array<char, 100> buffer{ };
        snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_ABILITY_BANNER ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ),
                  FS::getAbilityName( p_pokemon->getAbility( ) ).c_str( ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ) );

        IO::regularFont->printStringC( buffer.data( ), 128 + ( p_opponent ? 18 : -32 ),
                                       oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 1, false,
                                       p_opponent ? IO::font::LEFT : IO::font::RIGHT, 14, -14 );

        for( u8 i = 0; i < 75; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 3; ++i ) { oam[ SPR_ABILITY_OAM( !p_opponent ) + i ].isHidden = true; }
        IO::printRectangle( ( p_opponent ? 128 : 0 ), oam[ SPR_ABILITY_OAM( !p_opponent ) ].y,
                            ( p_opponent ? 255 : 127 ),
                            oam[ SPR_ABILITY_OAM( !p_opponent ) ].y + 32, false, 0 );
        IO::updateOAM( false );

        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::logForewarn( pokemon* p_pokemon, bool p_opponent, u16 p_move ) {
        std::array<char, 50> buffer{ };
        auto                 fmt = std::string( GET_STRING( IO::STR_BATTLE_DETECTS_MOVE ) );
        snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                  getPkmnName( p_pokemon, p_opponent ).c_str( ),
                  FS::kanaWrap( FS::getMoveName( p_move ).c_str( ), CURRENT_LANGUAGE ) );
        log( std::string( buffer.data( ) ) );
    }

    void battleUI::logAnticipation( pokemon* p_pokemon, bool p_opponent ) {
        std::array<char, 50> buffer{ };
        auto                 fmt = std::string( GET_STRING( IO::STR_BATTLE_SHUDERS ) );
        snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                  getPkmnName( p_pokemon, p_opponent ).c_str( ) );
        log( std::string( buffer.data( ) ) );
    }

    void battleUI::logFrisk( pokemon* p_pokemon, bool p_opponent, std::vector<u16> p_itms ) {
        std::array<char, 100> buffer{ };
        if( p_itms.size( ) == 1 ) {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_IDENTIFIES_ITEM ) );
            snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                      getPkmnName( p_pokemon, p_opponent ).c_str( ),
                      FS::getItemName( p_itms[ 0 ] ).c_str( ) );
        } else if( p_itms.size( ) == 2 ) {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_IDENTIFIES_TWO_ITEMS ) );
            snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                      getPkmnName( p_pokemon, p_opponent ).c_str( ),
                      FS::getItemName( p_itms[ 0 ] ).c_str( ),
                      FS::getItemName( p_itms[ 1 ] ).c_str( ) );
        } else {
            return;
        }
        log( std::string( buffer.data( ) ) );
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
                GET_STRING( 79 ), oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 32 ),
                           IO::yesNoBox::YES ) );

            IO::regularFont->printString(
                GET_STRING( 80 ), oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 48,
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
            IO::loadUIIcon( IO::ICON::MBOX1_START, SPR_MBOX_START_OAM, SPR_BALL_PAL,
                            SPR_PKMN_GFX( 1 ), 0, 192 - 46, 32, 64, false, false, false,
                            OBJPRIORITY_3, false );
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
            IO::regularFont->printBreakingStringC(
                p_message, 12, 192 - 40, 232, false, IO::font::LEFT, 16, ' ', 0, true, 1,
                SAVE::CURRENT_FILE->m_options.m_textSpeedModifier );
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

        IO::loadPKMNSprite( p_pokemon->getSpriteInfo( ), x, y, SPR_PKMN_START_OAM( 0 ),
                            SPR_PKMN_PAL( 0 ), SPR_PKMN_GFX( 0 ), false );
    }

} // namespace BATTLE
