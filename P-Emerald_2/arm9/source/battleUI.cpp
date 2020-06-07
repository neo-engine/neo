/*
Pokémon neo
------------------------------

file        : battleUI.cpp
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

#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <functional>
#include <initializer_list>
#include <tuple>

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
#include "moveChoiceBox.h"
#include "moveNames.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "saveGame.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"

#include "screenFade.h"

// Sprites
#include "A.h"
#include "Back.h"

#include "BattleBall1.h" //Normal
#include "BattleBall2.h" //Statused
#include "BattleBall3.h" //Fainted
#include "BattleBall4.h" //NA

#include "Down.h"
#include "Up.h"
#include "atks.h"
#include "memo.h"

#include "PokeBall1.h"
#include "PokeBall10.h"
#include "PokeBall11.h"
#include "PokeBall12.h"
#include "PokeBall13.h"
#include "PokeBall14.h"
#include "PokeBall15.h"
#include "PokeBall16.h"
#include "PokeBall17.h"
#include "PokeBall2.h"
#include "PokeBall3.h"
#include "PokeBall4.h"
#include "PokeBall5.h"
#include "PokeBall6.h"
#include "PokeBall7.h"
#include "PokeBall8.h"
#include "PokeBall9.h"

#include "Shiny1.h"
#include "Shiny2.h"

namespace BATTLE {

// Top screen
#define SPR_PKMN_START_OAM( p_idx ) ( 4 * ( p_idx ) )
#define SPR_PLATFORM_OAM 16

#define SPR_PKMN_PAL( p_idx ) ( ( p_idx ) )
#define SPR_PLATFORM_PAL 4

#define SPR_PKMN_GFX( p_idx ) ( 144 * ( p_idx ) )

#define WILD_BATTLE_HP_X 98
#define WILD_BATTLE_HP_Y 40
#define WILD_BATTLE_SPRITE_X_START 128
#define WILD_BATTLE_SPRITE_X 144

#define OPP_PLAT_Y 56
#define PLY_PLAT_Y 120

    void battleUI::initTop( ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( IO::OamTop );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );

        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/BATTLE_BACK/",
                std::to_string( _background ).c_str( ), 512,
                             49152 );

        u16 tileCnt = SPR_PKMN_GFX( 4 );

        // platforms
        tileCnt = IO::loadSprite( SPR_PLATFORM_OAM, SPR_PLATFORM_PAL, tileCnt, 128, OPP_PLAT_Y, 64, 64,
                IO::PlatformPals[ _platform2 ], IO::PlatformTiles[ 2 * _platform2 ], 2048, false,
                false, false, OBJPRIORITY_3, false );
        tileCnt = IO::loadSprite( SPR_PLATFORM_OAM + 1, SPR_PLATFORM_PAL, tileCnt, 192, OPP_PLAT_Y, 64, 64,
                IO::PlatformPals[ _platform2 ], IO::PlatformTiles[ 2 * _platform2 + 1 ], 2048,
                false, false, false, OBJPRIORITY_3, false );
        tileCnt = IO::loadSprite( SPR_PLATFORM_OAM + 2, SPR_PLATFORM_PAL + 1, tileCnt,
                -52, PLY_PLAT_Y, 64, 64, IO::PlatformPals[ _platform ],
                IO::PlatformTiles[ 2 * _platform ], 2048, false,
                false, false, OBJPRIORITY_3, false );
        tileCnt = IO::loadSprite( SPR_PLATFORM_OAM + 3, SPR_PLATFORM_PAL + 1, tileCnt, 80
                - 16, PLY_PLAT_Y, 64, 64, IO::PlatformPals[ _platform ],
                IO::PlatformTiles[ 2 * _platform + 1 ], 2048, false,
                false, false, OBJPRIORITY_3, false );
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 2 ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 2 ].isSizeDouble  = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 2 ].rotationIndex = 0;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].isSizeDouble  = true;
        IO::OamTop->oamBuffer[ SPR_PLATFORM_OAM + 3 ].rotationIndex = 0;
        IO::OamTop->matrixBuffer[ 0 ].hdx                         = 140;
        IO::OamTop->matrixBuffer[ 0 ].vdy                         = 130;
        IO::OamTop->matrixBuffer[ 1 ].hdx                         = 154;
        IO::OamTop->matrixBuffer[ 1 ].vdy                         = 154;


        IO::updateOAM( false );
    }

    void battleUI::initSub( ) {
        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

    }

    void battleUI::init( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::vramSetup( true );
        initTop( );
        initSub( );
    }

    void battleUI::startWildBattle( pokemon* p_pokemon ) {
        // Load pkmn sprite
        IO::loadPKMNSprite( p_pokemon->getSpecies( ), WILD_BATTLE_SPRITE_X_START,
                    OPP_PLAT_Y + 35 - 96 + IO::pkmnSpriteHeight( p_pokemon->getSpecies( ) ),
                    SPR_PKMN_START_OAM( 0 ), SPR_PKMN_PAL( 0 ),
                    SPR_PKMN_GFX( 0 ), false, p_pokemon->isShiny( ),
                    p_pokemon->isFemale( ), false, false, p_pokemon->getForme( ) );
        IO::fadeScreen( IO::UNFADE, true, true );

        for( u16 i = WILD_BATTLE_SPRITE_X_START; i < WILD_BATTLE_SPRITE_X; ++i ) {
            IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) ].x++;
            IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 1 ].x++;
            IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 2 ].x++;
            IO::OamTop->oamBuffer[ SPR_PKMN_START_OAM( 0 ) + 3 ].x++;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        if( p_pokemon->isShiny( ) ) {
           // TODO
           // animateShiny( WILD_BATTLE_SPRITE_X + 16, WILD_BATTLE_SPRITE_X_START + 16,
           //         SHINY_ANIM, 15, tileCnt );
        }

    }
} // namespace BATTLE
