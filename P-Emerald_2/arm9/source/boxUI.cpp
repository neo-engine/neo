/*
Pokémon neo
------------------------------

file        : boxUI.cpp
author      : Philip Wellnitz
description : Storage sys UI

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

#include <nds.h>

#include "ability.h"
#include "boxUI.h"
#include "defines.h"
#include "fs.h"
#include "infopage1.h"
#include "infopage2.h"
#include "infopage3.h"
#include "infopage4.h"
#include "locationNames.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "screenFade.h"
#include "uio.h"

#include "box_arrow.h"
#include "box_arrow2.h"
#include "partybg.h"

#include "NoPkmn.h"
#include "boxsub.h"
#include "boxwp1.h"
#include "boxwp2.h"
#include "boxwp3.h"

#include "arrow.h"
#include "backarrow.h"
#include "noselection_128_32_1.h"
#include "noselection_128_32_2.h"
#include "noselection_64_20.h"
#include "noselection_64_32.h"
#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "status_shiny.h"

#include "x_16_16.h"

namespace BOX {

    // Top screen
#define SPR_PKMN_START_OAM 0
#define SPR_MOVEWINDOW_START_OAM 5
#define SPR_TYPE_OAM( p_type ) ( 10 + ( p_type ) )
#define SPR_BALL_ICON_OAM 12
#define SPR_CHOICE_START_OAM 14
#define SPR_SHINY_ICON_OAM 23
#define SPR_INFOPAGE_START_OAM 24
#define SPR_PKMN_SHADOW_START_OAM 40
#define SPR_NAME_BG_OAM 44

#define SPR_PKMN_PAL 0
#define SPR_TYPE_PAL( p_type ) ( 1 + ( p_type ) )
#define SPR_INFOPAGE_PAL 3
#define SPR_BALL_ICON_PAL 4
#define SPR_BOX_PAL 5
#define SPR_SHINY_ICON_PAL 6
#define SPR_PKMN_SHADOW_PAL 7

    // bottom screen
#define SPR_PKMN_SEL_OAM_SUB 0
#define SPR_PKMN_START_OAM_SUB 1
#define SPR_X_OAM_SUB 39
// #define SPR_ARROW_BACK_OAM_SUB 40
// #define SPR_ARROW_BACK_BG_OAM_SUB 41
#define SPR_NEXT_BG_OAM_SUB 42
#define SPR_NEXT_ARR_OAM_SUB 43
#define SPR_NAME_BG_OAM_SUB 44
#define SPR_PREV_BG_OAM_SUB 46
#define SPR_PREV_ARR_OAM_SUB 47
#define SPR_SEL_ARROW_OAM_SUB 48
#define SPR_PARTY_BG_OAM_SUB 49
#define SPR_PARTY_TEXT_OAM_SUB 52
#define SPR_PKMN_OPTS_OAM_SUB( p_opt ) ( 54 + 3 * ( p_opt ) )

#define MAX_WALLPAPERS 3
#define INFO_X 8
#define INFO_Y 32
#define ANCHOR_X ( 256 - 96 - 8 )
#define ANCHOR_Y 8

    const unsigned int* wallpaperTiles[ MAX_WALLPAPERS ]
        = {boxwp1Bitmap, boxwp2Bitmap, boxwp3Bitmap};
    const unsigned short* wallpaperPals[ MAX_WALLPAPERS ] = {boxwp1Pal, boxwp2Pal, boxwp3Pal};

    void boxUI::initTop( ) {
        IO::initVideo( true );

        IO::clearScreen( false, false, true );
        IO::initOAMTable( false );
        dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
        dmaCopy( partybgPal, BG_PALETTE, 3 * 2 );
        bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
        bgSetScroll( IO::bg3, 0, 0 );
        REG_BLDCNT   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA = 0xff | ( 0x06 << 8 );
    }

    u16  TEXT_BUF[ 64 * 32 * 2 ] = {0};
    void boxUI::initSub( ) {
        REG_BLDCNT_SUB = BLEND_NONE;
        IO::clearScreen( true, false, false );
        IO::resetScale( true, false );
        IO::initOAMTable( true );
        vramSetBankD( VRAM_D_SUB_SPRITE );
        videoSetModeSub( MODE_5_2D | // DISPLAY_BG1_ACTIVE |
                         DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                         | ( ( DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
                               | DISPLAY_SPR_1D_BMP_SIZE_128 | ( 5 << 28 ) | 2 )
                             & 0xffffff0 ) );

        SpriteEntry* oam     = IO::Oam->oamBuffer;
        u16          tileCnt = 0;

        // x
        tileCnt
            = IO::loadSpriteB( SPR_X_OAM_SUB, tileCnt, 236, 172, 16, 16, x_16_16Pal, x_16_16Tiles,
                               x_16_16TilesLen, false, false, false, OBJPRIORITY_2, true );

        // prev / box name / next
        tileCnt
            = IO::loadSpriteB( SPR_PREV_BG_OAM_SUB, tileCnt, 2, -4, 64, 32, noselection_64_32Pal,
                               noselection_64_32Tiles, noselection_64_32TilesLen, true, true, false,
                               OBJPRIORITY_3, true );
        IO::loadSpriteB( SPR_NEXT_BG_OAM_SUB, oam[ SPR_PREV_BG_OAM_SUB ].gfxIndex, 120, -4, 64, 32,
                         0, 0, 0, true, true, false, OBJPRIORITY_3, true );
        tileCnt
            = IO::loadSpriteB( SPR_NAME_BG_OAM_SUB, tileCnt, 36, 0, 64, 32, noselection_128_32_1Pal,
                               noselection_128_32_1Tiles, noselection_128_32_1TilesLen, false,
                               false, false, OBJPRIORITY_3, true );
        tileCnt = IO::loadSpriteB( SPR_NAME_BG_OAM_SUB + 1, tileCnt, 100, 0, 64, 32,
                                   noselection_128_32_2Pal, noselection_128_32_2Tiles,
                                   noselection_128_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                                   true );

        tileCnt
            = IO::loadSpriteB( SPR_PREV_ARR_OAM_SUB, tileCnt, 26, 6, 16, 16, arrowPal, arrowTiles,
                               arrowTilesLen, false, false, false, OBJPRIORITY_1, true );
        IO::loadSpriteB( SPR_NEXT_ARR_OAM_SUB, oam[ SPR_PREV_ARR_OAM_SUB ].gfxIndex, 146, 6, 16, 16,
                         0, 0, arrowTilesLen, false, true, false, OBJPRIORITY_1, true );

        // back arrow
        // tileCnt = IO::loadSpriteB( SPR_ARROW_BACK_OAM_SUB, tileCnt, 102 + 56 + 16, 192 - 19, 16,
        // 16,
        //                           backarrowPal, backarrowTiles, backarrowTilesLen, false, false,
        //                           false, OBJPRIORITY_0, true );
        // IO::loadSpriteB( SPR_ARROW_BACK_BG_OAM_SUB, oam[ SPR_PREV_BG_OAM_SUB ].gfxIndex, 102 +
        // 48,
        //                 192 - 24, 64, 32, 0, 0, 0, false, false, false, OBJPRIORITY_2, true );

        // box arrow
        tileCnt = IO::loadSpriteB( SPR_SEL_ARROW_OAM_SUB, tileCnt, 0, 0, 16, 16, box_arrowPal,
                                   box_arrowTiles, box_arrowTilesLen, false, false, true,
                                   OBJPRIORITY_0, true );

        // Party bg
        IO::loadSpriteB( SPR_PARTY_BG_OAM_SUB + 1, tileCnt, 80, 192 - 32, 32, 32, 0, 0, 0, true,
                         true, false, OBJPRIORITY_2, true );
        IO::loadSpriteB( SPR_PARTY_BG_OAM_SUB + 2, tileCnt, 64, 192 - 32, 32, 32, 0, 0, 0, true,
                         true, false, OBJPRIORITY_2, true );
        tileCnt = IO::loadSpriteB( SPR_PARTY_BG_OAM_SUB, tileCnt, 32, 192 - 20, 32, 32,
                                   noselection_64_20Pal, noselection_64_20Tiles,
                                   noselection_64_20TilesLen, false, false, false, OBJPRIORITY_2,
                                   true );

        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        IO::regularFont->printStringBC( GET_STRING( 380 ), IO::BG_PAL( true ), TEXT_BUF, 64,
                                        IO::font::CENTER );
        tileCnt = IO::loadSpriteB( SPR_PARTY_TEXT_OAM_SUB, tileCnt, 40, 192 - 18, 64, 32, TEXT_BUF,
                                   64 * 32 / 2, false, false, false, OBJPRIORITY_1, true );
        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        IO::regularFont->printStringBC( GET_STRING( 381 ), IO::BG_PAL( true ), TEXT_BUF, 64,
                                        IO::font::CENTER );
        tileCnt = IO::loadSpriteB( SPR_PARTY_TEXT_OAM_SUB + 1, tileCnt, 40, 192 - 18, 64, 32,
                                   TEXT_BUF, 64 * 32 / 2, false, false, true, OBJPRIORITY_1, true );

        // pkmn options
        for( u8 i = 0; i < 6; ++i ) {
            IO::loadSpriteB( SPR_PKMN_OPTS_OAM_SUB( i ) + 1, oam[ SPR_PARTY_BG_OAM_SUB ].gfxIndex,
                             256 - 64, 36 + 21 * i, 32, 32, 0, 0, 0, false, false, true,
                             OBJPRIORITY_2, true );
            IO::loadSpriteB( SPR_PKMN_OPTS_OAM_SUB( i ), oam[ SPR_PARTY_BG_OAM_SUB ].gfxIndex,
                             256 - 32, 24 + 21 * i, 32, 32, 0, 0, 0, true, true, true,
                             OBJPRIORITY_2, true );
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            IO::regularFont->printStringBC( GET_STRING( 382 + i ), IO::BG_PAL( true ), TEXT_BUF, 64,
                                            IO::font::CENTER );
            tileCnt = IO::loadSpriteB( SPR_PKMN_OPTS_OAM_SUB( i ) + 2, tileCnt, 256 - 64,
                                       38 + 21 * i, 64, 32, TEXT_BUF, 64 * 32 / 2, false, false,
                                       true, OBJPRIORITY_1, true );
        }

        tileCnt
            = IO::loadSpriteB( SPR_PKMN_SEL_OAM_SUB, tileCnt, 0, 0, 32, 32, NoPkmnPal, NoPkmnTiles,
                               NoPkmnTilesLen, false, false, true, OBJPRIORITY_1, true );
        // pkmn
        for( u8 i = 0; i < 5; ++i ) {
            for( u8 j = 0; j < 6; ++j ) {
                tileCnt
                    = IO::loadSpriteB( SPR_PKMN_START_OAM_SUB + 6 * i + j, tileCnt, 26 + 26 * j,
                                       32 + 26 * i, 32, 32, NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen,
                                       false, false, false, OBJPRIORITY_3, true );
            }
        }
        // team pkmn
        for( u8 i = 0; i < 3; ++i ) {
            u8 pos = 2 * i;
            tileCnt
                = IO::loadSpriteB( SPR_PKMN_START_OAM_SUB + 30 + pos, tileCnt, 32,
                        32 + 26 * i, 32, 32, NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen,
                        false, false, true, OBJPRIORITY_1, true );
        }
        for( u8 i = 0; i < 3; ++i ) {
            u8 pos = 2 * i + 1;
            tileCnt
                = IO::loadSpriteB( SPR_PKMN_START_OAM_SUB + 30 + pos, tileCnt, 72,
                        32 + 26 * i, 32, 32, NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen,
                        false, false, true, OBJPRIORITY_1, true );
        }

    }

    std::vector<boxUI::interact> boxUI::getInteractions( ) {
        // TODO
        return std::vector<interact>( );
    }

    void boxUI::init( ) {
        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = IO::BG_PAL( i );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = IO::GRAY;
            pal[ IO::COLOR_IDX ] = IO::RGB( 22, 22, 22 );
            pal[ IO::BLACK_IDX ] = IO::BLACK;
            pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

            pal[ 240 ] = IO::RGB( 6, 6, 6 );    // hp bar border color
            pal[ 241 ] = IO::RGB( 12, 30, 12 ); // hp bar green 1
            pal[ 242 ] = IO::RGB( 3, 23, 4 );   // hp bar green 2
            pal[ 243 ] = IO::RGB( 30, 30, 12 ); // hp bar yellow 1
            pal[ 244 ] = IO::RGB( 23, 23, 5 );  // hp bar yellow 2
            pal[ 245 ] = IO::RGB( 30, 15, 12 ); // hp bar red 1
            pal[ 246 ] = IO::RGB( 20, 7, 7 );   // hp bar red 2
        }

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );

        initSub( );
        initTop( );
        bgUpdate( );
        IO::updateOAM( true );
        IO::updateOAM( false );
    }

    void boxUI::setMode( u8 p_newMode ) {
        switch( p_newMode ) {
        default:
        case 0: // Blue arrow for status
            _outlineColor = 0xF4A0;
            IO::loadSpriteB(
                SPR_SEL_ARROW_OAM_SUB, IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].gfxIndex,
                IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x,
                IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y, 16, 16, box_arrowPal, box_arrowTiles,
                box_arrowTilesLen, false, false,
                IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden, OBJPRIORITY_0, true );
            break;
        case 1: // Redish arrow for move
            _outlineColor = 0b1001'0100'0001'1101;
            IO::loadSpriteB(
                SPR_SEL_ARROW_OAM_SUB, IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].gfxIndex,
                IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x,
                IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y, 16, 16, box_arrow2Pal,
                box_arrow2Tiles, box_arrowTilesLen, false, false,
                IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden, OBJPRIORITY_0, true );
            break;
        }
        if( _heldPkmn.getSpecies( ) ) {
            if( !_heldPkmn.isEgg( ) ) {
                IO::loadPKMNIconB(
                        _heldPkmn.getSpecies( ),
                        IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].x,
                        IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].y,
                        SPR_PKMN_SEL_OAM_SUB,
                        IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].gfxIndex, true,
                        _heldPkmn.getForme( ),
                        _heldPkmn.isShiny( ), _heldPkmn.isFemale( ), true, _outlineColor );
                IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].priority = OBJPRIORITY_1;
            } else {
                IO::loadEggIconB( IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].x,
                        IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].y,
                        SPR_PKMN_SEL_OAM_SUB,
                        IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].gfxIndex, true,
                        _heldPkmn.getSpecies( ) == PKMN_MANAPHY, true, _outlineColor );
                IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].priority = OBJPRIORITY_1;
            }
        }
        IO::updateOAM( true );
    }

    void boxUI::draw( box* p_box ) {
        if( !_heldPkmn.getSpecies( ) ) {
            drawPkmnInfoTop( 0 );
        }
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        SpriteEntry* oam = IO::Oam->oamBuffer;

        dmaCopy( wallpaperTiles[ p_box->m_wallpaper % MAX_WALLPAPERS ], bgGetGfxPtr( IO::bg3sub ),
                 256 * 256 );
        dmaCopy( wallpaperPals[ p_box->m_wallpaper % MAX_WALLPAPERS ], BG_PALETTE_SUB, 64 * 2 );
        dmaCopy( boxsubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        dmaCopy( boxsubPal, BG_PALETTE_SUB, 6 * 2 );

        // Load some placeholder
        for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i ) {
            if( p_box->m_pokemon[ i ].getSpecies( ) ) {
                IO::loadSpriteB(
                    SPR_PKMN_START_OAM_SUB + i, oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex,
                    oam[ SPR_PKMN_START_OAM_SUB + i ].x, oam[ SPR_PKMN_START_OAM_SUB + i ].y, 32,
                    32, NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen, false, false, false, OBJPRIORITY_3,
                    true );
            } else {
                oam[ SPR_PKMN_START_OAM_SUB + i ].isHidden = true;
            }
        }
        IO::updateOAM( true );
        IO::regularFont->printStringC( p_box->m_name, 94, 6, true, IO::font::CENTER );

        for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i ) {
            if( p_box->m_pokemon[ i ].getSpecies( ) ) {
                if( !p_box->m_pokemon[ i ].isEgg( ) ) {
                    IO::loadPKMNIconB(
                        ( *p_box )[ i ].getSpecies( ), oam[ SPR_PKMN_START_OAM_SUB + i ].x,
                        oam[ SPR_PKMN_START_OAM_SUB + i ].y, SPR_PKMN_START_OAM_SUB + i,
                        oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex, true,
                        ( *p_box )[ i ].getForme( ), ( *p_box )[ i ].isShiny( ),
                        ( *p_box )[ i ].isFemale( ) );
                } else {
                    IO::loadEggIconB(
                        oam[ SPR_PKMN_START_OAM_SUB + i ].x, oam[ SPR_PKMN_START_OAM_SUB + i ].y,
                        SPR_PKMN_START_OAM_SUB + i, oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex,
                        true, p_box->m_pokemon[ i ].getSpecies( ) == PKMN_MANAPHY );
                }
            } else {
                oam[ SPR_PKMN_START_OAM_SUB + i ].isHidden = true;
            }
        }

        IO::updateOAM( true );
    }

    void boxUI::selectButton( button p_selectedButton, bool p_touched ) {
        switch( p_selectedButton ) {
        case BUTTON_BOX_NAME:
            if( !_heldPkmn.getSpecies( ) ) {
                drawPkmnInfoTop( 0 );
                drawPkmnInfoSub( 0 );
            }
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_NAME_BG_OAM_SUB + 1 ].x;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y
                = IO::Oam->oamBuffer[ SPR_NAME_BG_OAM_SUB ].y - 2;
            break;
        case BUTTON_PARTY:
            if( !_heldPkmn.getSpecies( ) ) {
                drawPkmnInfoTop( 0 );
                drawPkmnInfoSub( 0 );
            }
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_PARTY_BG_OAM_SUB + 2 ].x + 8;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y
                = IO::Oam->oamBuffer[ SPR_PARTY_BG_OAM_SUB ].y - 4;
            break;
        case BUTTON_PKMN_MOVE:
        case BUTTON_PKMN_STATUS:
        case BUTTON_PKMN_RELEASE:
        case BUTTON_PKMN_GIVE_ITEM:
        case BUTTON_PKMN_TAKE_ITEM:
        case BUTTON_PKMN_CANCEL:
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_PKMN_OPTS_OAM_SUB( p_selectedButton - BUTTON_PKMN_MOVE ) ]
                      .x;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y
                = IO::Oam
                      ->oamBuffer[ SPR_PKMN_OPTS_OAM_SUB( p_selectedButton - BUTTON_PKMN_MOVE )
                                   + 1 ]
                      .y
                  - 8;
            break;
        default:
            break;
        }
        if( _heldPkmn.getSpecies( ) ) {
            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x - 20;
            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].y
                = IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y - 4;
        }
        IO::updateOAM( true );
    }

    void boxUI::selectPkmn( boxPokemon* p_pokemon, u8 p_index, bool p_touched ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        if( p_pokemon != nullptr && p_pokemon->getSpecies( ) ) {
            if( !p_pokemon->isEgg( ) ) {
                IO::loadPKMNIconB(
                    p_pokemon->getSpecies( ), oam[ SPR_PKMN_START_OAM_SUB + p_index ].x,
                    oam[ SPR_PKMN_START_OAM_SUB + p_index ].y, SPR_PKMN_SEL_OAM_SUB,
                    oam[ SPR_PKMN_SEL_OAM_SUB ].gfxIndex, true, p_pokemon->getForme( ),
                    p_pokemon->isShiny( ), p_pokemon->isFemale( ), true, _outlineColor );
                oam[ SPR_PKMN_SEL_OAM_SUB ].priority = OBJPRIORITY_1;
            } else {
                IO::loadEggIconB( oam[ SPR_PKMN_START_OAM_SUB + p_index ].x,
                                  oam[ SPR_PKMN_START_OAM_SUB + p_index ].y, SPR_PKMN_SEL_OAM_SUB,
                                  oam[ SPR_PKMN_SEL_OAM_SUB ].gfxIndex, true,
                                  p_pokemon->getSpecies( ) == PKMN_MANAPHY, true, _outlineColor );
                oam[ SPR_PKMN_SEL_OAM_SUB ].priority = OBJPRIORITY_1;
            }
            _currentSelection = p_index;
        } else {
            oam[ SPR_PKMN_SEL_OAM_SUB ].isHidden = true;
            _currentSelection                    = -1;
        }
        IO::updateOAM( true );
        drawPkmnInfoSub( p_pokemon );
    }

    void boxUI::hoverPkmn( boxPokemon* p_pokemon, u8 p_index, bool p_redraw ) {
        if( _heldPkmn.getSpecies( ) ) {
            updateHeldPkmn( p_index );
            return;
        }

        if( p_index != u8( -1 ) ) {
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + p_index ].x + 20;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y
                = IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + p_index ].y + 4;
            auto tmp = ( _heldPkmn.getSpecies( ) || p_pokemon == nullptr ) ? pokemon( _heldPkmn )
                                                                           : pokemon( *p_pokemon );

            if( p_redraw ) {
                drawPkmnInfoTop( &tmp );
                drawPkmnInfoSub( 0 );
            }
        } else {
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden = true;
        }
        IO::updateOAM( true );
    }

    void boxUI::writeLineTop( const char* p_string, u8 p_line, u8 p_color, bool p_bottom ) {
        IO::regularFont->setColor( p_color, 1 );
        IO::regularFont->printStringC( p_string, INFO_X + 12, INFO_Y + 11 + 15 * p_line, p_bottom );
    }

    void boxUI::drawPkmnInfoSub( boxPokemon* p_pokemon ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        if( p_pokemon ) {
            for( u8 i = 0; i < 2; ++i ) {
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) ].isHidden     = false;
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) + 1 ].isHidden = false;
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) + 2 ].isHidden = false;
            }
            for( u8 i = 2; i < 4; ++i ) {
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) ].isHidden     = p_pokemon->isEgg( );
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) + 1 ].isHidden = p_pokemon->isEgg( );
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) + 2 ].isHidden = p_pokemon->isEgg( );
            }
            if( p_pokemon->getItem( ) && !p_pokemon->isEgg( ) ) {
                oam[ SPR_PKMN_OPTS_OAM_SUB( 4 ) ].isHidden     = false;
                oam[ SPR_PKMN_OPTS_OAM_SUB( 4 ) + 1 ].isHidden = false;
                oam[ SPR_PKMN_OPTS_OAM_SUB( 4 ) + 2 ].isHidden = false;
            } else {
                oam[ SPR_PKMN_OPTS_OAM_SUB( 4 ) ].isHidden     = true;
                oam[ SPR_PKMN_OPTS_OAM_SUB( 4 ) + 1 ].isHidden = true;
                oam[ SPR_PKMN_OPTS_OAM_SUB( 4 ) + 2 ].isHidden = true;
            }
            oam[ SPR_PKMN_OPTS_OAM_SUB( 5 ) ].isHidden     = false;
            oam[ SPR_PKMN_OPTS_OAM_SUB( 5 ) + 1 ].isHidden = false;
            oam[ SPR_PKMN_OPTS_OAM_SUB( 5 ) + 2 ].isHidden = false;
        } else {
            for( u8 i = 0; i < 6; ++i ) {
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) ].isHidden     = true;
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) + 1 ].isHidden = true;
                oam[ SPR_PKMN_OPTS_OAM_SUB( i ) + 2 ].isHidden = true;
            }
        }
        IO::updateOAM( true );
    }

    void boxUI::drawPkmnInfoTop( pokemon* p_pokemon ) {
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        SpriteEntry* oam     = IO::OamTop->oamBuffer;
        u16          tileCnt = 0;

        tileCnt = IO::loadSprite( SPR_NAME_BG_OAM + 3, SPR_BOX_PAL, tileCnt, 0, -4, 64, 32,
                                  noselection_128_32_2Pal, noselection_128_32_2Tiles,
                                  noselection_128_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                                  false );
        IO::loadSprite( SPR_NAME_BG_OAM, SPR_BOX_PAL, tileCnt, 66, -4, 64, 32,
                        noselection_128_32_2Pal, noselection_128_32_2Tiles,
                        noselection_128_32_2TilesLen, false, false, false, OBJPRIORITY_3, false );
        IO::loadSprite( SPR_NAME_BG_OAM + 1, SPR_BOX_PAL, tileCnt, 48, -4, 64, 32,
                        noselection_128_32_2Pal, noselection_128_32_2Tiles,
                        noselection_128_32_2TilesLen, false, false, false, OBJPRIORITY_3, false );
        tileCnt = IO::loadSprite( SPR_NAME_BG_OAM + 2, SPR_BOX_PAL, tileCnt, 24, -4, 64, 32,
                                  noselection_128_32_2Pal, noselection_128_32_2Tiles,
                                  noselection_128_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                                  false );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->printStringC( GET_STRING( 379 ), 5, 3, false );

        for( u8 i = 0; i < 128; ++i ) {
            if( i >= SPR_NAME_BG_OAM && i <= SPR_NAME_BG_OAM + 3 ) { continue; }
            oam[ i ].isHidden       = true;
            oam[ i ].attribute[ 0 ] = ATTR0_DISABLED;
            oam[ i ].attribute[ 1 ] = 0;
            oam[ i ].attribute[ 2 ] = 0;
        }

        IO::updateOAM( false );
        if( !p_pokemon || !p_pokemon->getSpecies( ) ) { return; }

        // Name box
        IO::loadSprite( SPR_CHOICE_START_OAM, SPR_BOX_PAL, tileCnt, ANCHOR_X, ANCHOR_Y, 16, 32, 0,
                        0, noselection_96_32_1TilesLen, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM + 5, SPR_BOX_PAL, tileCnt, ANCHOR_X + 80,
                                  ANCHOR_Y, 16, 32, noselection_96_32_1Pal,
                                  noselection_96_32_1Tiles, noselection_96_32_1TilesLen, true, true,
                                  false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 1, SPR_BOX_PAL, tileCnt, ANCHOR_X + 16, ANCHOR_Y, 16,
                        32, 0, 0, noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 2, SPR_BOX_PAL, tileCnt, ANCHOR_X + 32, ANCHOR_Y, 16,
                        32, 0, 0, noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 3, SPR_BOX_PAL, tileCnt, ANCHOR_X + 48, ANCHOR_Y, 16,
                        32, 0, 0, noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_CHOICE_START_OAM + 7, SPR_BOX_PAL, tileCnt, ANCHOR_X + 73, ANCHOR_Y, 16,
                        32, 0, 0, noselection_96_32_2TilesLen, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM + 4, SPR_BOX_PAL, tileCnt, ANCHOR_X + 64,
                                  ANCHOR_Y, 16, 32, noselection_96_32_2Pal,
                                  noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false,
                                  false, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
        // Info BG / Move window
        IO::loadSprite( SPR_MOVEWINDOW_START_OAM + 3, SPR_INFOPAGE_PAL, tileCnt, ANCHOR_X - 6,
                        ANCHOR_Y + 119, 64, 64, 0, 0, infopage1TilesLen, false, false,
                        p_pokemon->isEgg( ), OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_MOVEWINDOW_START_OAM + 1, SPR_INFOPAGE_PAL, tileCnt, ANCHOR_X + 40,
                        ANCHOR_Y + 119 + 64 - 52, 64, 64, 0, 0, infopage1TilesLen, true, true,
                        p_pokemon->isEgg( ), OBJPRIORITY_3, false, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM, SPR_INFOPAGE_PAL, tileCnt, INFO_X, INFO_Y, 64, 64,
                        0, 0, infopage1TilesLen, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 8, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 64,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, infopage1TilesLen, true, true, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 4, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 80,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, infopage1TilesLen, true, true, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 3, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 80,
                        INFO_Y + 64 + 15, 64, 64, 0, 0, infopage1TilesLen, true, true, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM + 7, SPR_INFOPAGE_PAL, tileCnt,
                                  INFO_X + 64, INFO_Y + 64 + 15, 64, 64, infopage1Pal,
                                  infopage1Tiles, infopage1TilesLen, true, true, false,
                                  OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_MOVEWINDOW_START_OAM + 2, SPR_INFOPAGE_PAL, tileCnt, ANCHOR_X + 40,
                        ANCHOR_Y + 119, 64, 64, 0, 0, infopage2TilesLen, false, false,
                        p_pokemon->isEgg( ), OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_MOVEWINDOW_START_OAM, SPR_INFOPAGE_PAL, tileCnt, ANCHOR_X - 6,
                        ANCHOR_Y + 119 + 64 - 52, 64, 64, 0, 0, infopage2TilesLen, true, true,
                        p_pokemon->isEgg( ), OBJPRIORITY_3, false, OBJMODE_BLENDED );

        IO::loadSprite( SPR_INFOPAGE_START_OAM + 2, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 64, INFO_Y,
                        64, 64, 0, 0, infopage2TilesLen, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 1, SPR_INFOPAGE_PAL, tileCnt, INFO_X + 80, INFO_Y,
                        64, 64, 0, 0, infopage2TilesLen, false, false, false, OBJPRIORITY_3, false,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 9, SPR_INFOPAGE_PAL, tileCnt, INFO_X,
                        INFO_Y + 64 - 15, 64, 64, 0, 0, infopage2TilesLen, true, true, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM + 6, SPR_INFOPAGE_PAL, tileCnt, INFO_X,
                                  INFO_Y + 64 + 15, 64, 64, infopage2Pal, infopage2Tiles,
                                  infopage2TilesLen, true, true, false, OBJPRIORITY_3, false,
                                  OBJMODE_BLENDED );

        oam[ SPR_BALL_ICON_OAM ].isHidden = true;

        // Print Basic Pkmn info
        if( !p_pokemon->isEgg( ) ) {
            oam[ SPR_CHOICE_START_OAM + 5 ].isHidden = false;

            pkmnData data = getPkmnData( p_pokemon->getSpecies( ), p_pokemon->getForme( ) );
            // Type icon
            for( u8 i = 0; i < 2; ++i ) {
                tileCnt = IO::loadTypeIcon( data.m_baseForme.m_types[ i ], INFO_X + 128 - 62 + 34,
                                            INFO_Y + 11 + 15 * i, SPR_TYPE_OAM( i ),
                                            SPR_TYPE_PAL( i ), tileCnt, false, CURRENT_LANGUAGE );
                if( data.m_baseForme.m_types[ 0 ] == data.m_baseForme.m_types[ 1 ] ) { break; }
            }

            // Shiny
            tileCnt = IO::loadSprite(
                SPR_SHINY_ICON_OAM, SPR_SHINY_ICON_PAL, tileCnt, ANCHOR_X + 68 - 8, ANCHOR_Y + 20,
                8, 8, status_shinyPal, status_shinyTiles, status_shinyTilesLen, false, false,
                !p_pokemon->isShiny( ), OBJPRIORITY_0, false, OBJMODE_NORMAL );

            // Pokéball Icon
            tileCnt = IO::loadItemIcon( ITEM::ballToItem( p_pokemon->m_boxdata.m_ball ),
                                        ANCHOR_X - 4, ANCHOR_Y, SPR_BALL_ICON_OAM,
                                        SPR_BALL_ICON_PAL, tileCnt, false );

            if( IO::regularFont->stringWidthC( p_pokemon->m_boxdata.m_name ) > 74 ) {
                s8 diff = 66 - IO::regularFont->stringWidthC( p_pokemon->m_boxdata.m_name );
                oam[ SPR_BALL_ICON_OAM ].x = ANCHOR_X + diff - 2;
                IO::regularFont->printStringC( p_pokemon->m_boxdata.m_name, ANCHOR_X + 22 + diff,
                                               ANCHOR_Y + 3, false );
            } else {
                IO::regularFont->printStringC( p_pokemon->m_boxdata.m_name, ANCHOR_X + 22,
                                               ANCHOR_Y + 3, false );
            }

            IO::updateOAM( false );

            // Level
            IO::smallFont->setColor( 0, 0 );
            IO::smallFont->setColor( IO::WHITE_IDX, 1 );
            IO::smallFont->setColor( IO::GRAY_IDX, 2 );
            IO::smallFont->printString( ( "!" + std::to_string( p_pokemon->m_level ) ).c_str( ),
                                        ANCHOR_X + 24, ANCHOR_Y + 11, false );

            // Gender
            if( p_pokemon->getSpecies( ) != PKMN_NIDORAN_F
                && p_pokemon->getSpecies( ) != PKMN_NIDORAN_M ) {
                if( p_pokemon->m_boxdata.m_isFemale ) {
                    IO::regularFont->setColor( IO::RED_IDX, 1 );
                    IO::regularFont->setColor( IO::RED2_IDX, 2 );
                    IO::regularFont->printString( "}", ANCHOR_X + 85, ANCHOR_Y + 15, false );
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                } else if( !p_pokemon->m_boxdata.m_isGenderless ) {
                    IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                    IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
                    IO::regularFont->printString( "{", ANCHOR_X + 85, ANCHOR_Y + 15, false );
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                }
            }

            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            writeLineTop( getDisplayName( p_pokemon->getSpecies( ) ).c_str( ), 0 );

            IO::regularFont->setColor( 0, 2 );
            char buffer[ 50 ];
            snprintf( buffer, 49, "%03hu", p_pokemon->getSpecies( ) );
            IO::regularFont->printStringC( GET_STRING( 337 ), INFO_X + 12, INFO_Y + 27, false );
            if( p_pokemon->isShiny( ) ) {
                IO::regularFont->setColor( IO::RED2_IDX, 1 );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            } else {
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            }
            IO::regularFont->printStringC(
                buffer, IO::regularFont->stringWidthC( GET_STRING( 337 ) ) + INFO_X + 16,
                INFO_Y + 27, false );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( GET_STRING( 378 ), INFO_X + 12, INFO_Y + 27 + 30,
                                           false );
            IO::regularFont->printStringC( GET_STRING( 363 ), INFO_X + 12, INFO_Y + 27 + 45,
                                           false );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->printStringC( GET_STRING( 187 + u8( p_pokemon->getNature( ) ) ),
                                           INFO_X + 144 - 12, INFO_Y + 27 + 30, false,
                                           IO::font::RIGHT );
            IO::regularFont->printStringC( getAbilityName( p_pokemon->getAbility( ) ).c_str( ),
                                           INFO_X + 144 - 12, INFO_Y + 27 + 60, false,
                                           IO::font::RIGHT );

            if( p_pokemon->getItem( ) ) {
                IO::regularFont->setColor( 0, 2 );
                IO::regularFont->printStringC( GET_STRING( 362 ), INFO_X + 12, INFO_Y + 27 + 75,
                                               false );
                IO::regularFont->setColor( IO::COLOR_IDX, 2 );
                IO::regularFont->printStringC( ITEM::getItemName( p_pokemon->getItem( ) ).c_str( ),
                                               INFO_X + 144 - 12, INFO_Y + 27 + 90, false,
                                               IO::font::RIGHT );
            }

            // Moves
            for( u8 i = 0; i < 4; ++i ) {
                if( p_pokemon->m_boxdata.m_moves[ i ] ) {
                    IO::regularFont->printStringC(
                        MOVE::getMoveName( p_pokemon->m_boxdata.m_moves[ i ] ).c_str( ),
                        ANCHOR_X + 2, ANCHOR_Y + 123 + 15 * i, false );
                }
            }
        } else {
            // Egg
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            IO::regularFont->printString( GET_STRING( 34 ), ANCHOR_X + 12, ANCHOR_Y + 8, false );

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
                                           INFO_Y + 11 + 15 * startline, false );
            IO::regularFont->setColor( IO::COLOR_IDX, 2 );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->printStringC(
                ".",
                INFO_X + 9 + shift + IO::regularFont->stringWidth( loc.c_str( ) ) - loc.length( ),
                INFO_Y + 11 + 15 * startline, false );

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
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        }

        // pokemon / shadow
        if( !p_pokemon->isEgg( ) ) {
            tileCnt = IO::loadPKMNSprite( p_pokemon->getSpecies( ), ANCHOR_X, ANCHOR_Y + 26,
                                          SPR_PKMN_START_OAM, SPR_PKMN_PAL, tileCnt, false,
                                          p_pokemon->isShiny( ), p_pokemon->isFemale( ), false,
                                          false, p_pokemon->getForme( ) );
        } else {
            tileCnt = IO::loadEggSprite( ANCHOR_X, ANCHOR_Y + 26, SPR_PKMN_START_OAM, SPR_PKMN_PAL,
                                         tileCnt, false, p_pokemon->getSpecies( ) == PKMN_MANAPHY );
        }
        u16 emptyPal[ 32 ]                = {0};
        IO::OamTop->matrixBuffer[ 0 ].hdx = ( 1 << 8 );
        IO::OamTop->matrixBuffer[ 0 ].vdx = -( 1 << 8 );

        u8 sx = 32, sy = 32;
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 0, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 0 ].gfxIndex, ANCHOR_X - 48 - sx,
                        ANCHOR_Y + 26 - sy, 64, 64, emptyPal, 0, 0, false, false, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 1, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 1 ].gfxIndex, ANCHOR_X + 80 - 48 - sx,
                        ANCHOR_Y + 26 - sy, 32, 64, 0, 0, 0, false, false, false, OBJPRIORITY_3,
                        false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 2, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 2 ].gfxIndex, ANCHOR_X - sx,
                        ANCHOR_Y + 26 + 80 - sy, 64, 32, 0, 0, 0, false, false, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PKMN_SHADOW_START_OAM + 3, SPR_PKMN_SHADOW_PAL,
                        oam[ SPR_PKMN_START_OAM + 3 ].gfxIndex, ANCHOR_X + 80 - sx,
                        ANCHOR_Y + 26 + 80 - sy, 32, 32, 0, 0, 0, false, false, false,
                        OBJPRIORITY_3, false, OBJMODE_BLENDED );

        for( u8 i = 0; i < 4; ++i ) {
            oam[ SPR_PKMN_SHADOW_START_OAM + i ].isRotateScale = true;
            oam[ SPR_PKMN_SHADOW_START_OAM + i ].isSizeDouble  = true;
            oam[ SPR_PKMN_SHADOW_START_OAM + i ].rotationIndex = 0;
        }

        IO::updateOAM( false );
    }

    void boxUI::updatePkmn( boxPokemon* p_pokemon, u8 p_index ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        if( p_pokemon != nullptr && p_pokemon->getSpecies( ) ) {
            if( !p_pokemon->isEgg( ) ) {
                IO::loadPKMNIconB(
                    p_pokemon->getSpecies( ), oam[ SPR_PKMN_START_OAM_SUB + p_index ].x,
                    oam[ SPR_PKMN_START_OAM_SUB + p_index ].y, SPR_PKMN_START_OAM_SUB + p_index,
                    oam[ SPR_PKMN_START_OAM_SUB + p_index ].gfxIndex, true, p_pokemon->getForme( ),
                    p_pokemon->isShiny( ), p_pokemon->isFemale( ) );
            } else {
                IO::loadEggIconB( oam[ SPR_PKMN_START_OAM_SUB + p_index ].x,
                                  oam[ SPR_PKMN_START_OAM_SUB + p_index ].y,
                                  SPR_PKMN_START_OAM_SUB + p_index,
                                  oam[ SPR_PKMN_START_OAM_SUB + p_index ].gfxIndex, true,
                                  p_pokemon->getSpecies( ) == PKMN_MANAPHY );
            }
        } else {
            oam[ SPR_PKMN_START_OAM_SUB + p_index ].isHidden = true;
        }
        if( p_index >= MAX_PKMN_PER_BOX ) {
            oam[ SPR_PKMN_START_OAM_SUB + p_index ].priority = OBJPRIORITY_1;
        }
        if( p_index == _currentSelection ) { selectPkmn( p_pokemon, p_index ); }
        IO::updateOAM( true );
    }

    void boxUI::updateHeldPkmn( u8 p_index ) {
        if( _heldPkmn.getSpecies( ) ) {
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + p_index ].x + 25;
            IO::Oam->oamBuffer[ SPR_SEL_ARROW_OAM_SUB ].y
                = IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + p_index ].y - 1;

            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].isHidden = false;
            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].x
                = IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + p_index ].x + 5;
            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].y
                = IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + p_index ].y - 5;
        } else {
            IO::Oam->oamBuffer[ SPR_PKMN_SEL_OAM_SUB ].isHidden = true;
            hoverPkmn( nullptr, p_index, false );
        }

        IO::updateOAM( true );
    }

    void boxUI::setNewHeldPkmn( boxPokemon* p_pokemon, u8 p_index ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        if( p_pokemon != nullptr ) {
            bool redraw = false;
            if( _heldPkmn.getSpecies( ) ) {
                redraw = true;
            }
            _heldPkmn = *p_pokemon;
            if( !_heldPkmn.isEgg( ) ) {
                IO::loadPKMNIconB(
                        _heldPkmn.getSpecies( ), oam[ SPR_PKMN_START_OAM_SUB + p_index ].x,
                        oam[ SPR_PKMN_START_OAM_SUB + p_index ].y, SPR_PKMN_SEL_OAM_SUB,
                        oam[ SPR_PKMN_SEL_OAM_SUB ].gfxIndex, true, _heldPkmn.getForme( ),
                        _heldPkmn.isShiny( ), _heldPkmn.isFemale( ), true, _outlineColor );
                oam[ SPR_PKMN_SEL_OAM_SUB ].priority = OBJPRIORITY_1;
            } else {
                IO::loadEggIconB( oam[ SPR_PKMN_START_OAM_SUB + p_index ].x,
                        oam[ SPR_PKMN_START_OAM_SUB + p_index ].y, SPR_PKMN_SEL_OAM_SUB,
                        oam[ SPR_PKMN_SEL_OAM_SUB ].gfxIndex, true,
                        _heldPkmn.getSpecies( ) == PKMN_MANAPHY, true, _outlineColor );
                oam[ SPR_PKMN_SEL_OAM_SUB ].priority = OBJPRIORITY_1;
            }
            oam[ SPR_PKMN_SEL_OAM_SUB ].isHidden = false;

            if( redraw ) {
                auto tmp = pokemon( _heldPkmn );
                drawPkmnInfoTop( &tmp );
            }
            drawPkmnInfoSub( 0 );
        } else {
            std::memset( &_heldPkmn, 0, sizeof( boxPokemon ) );
            oam[ SPR_PKMN_SEL_OAM_SUB ].isHidden = true;
        }
        _currentSelection = -1;
        updateHeldPkmn( p_index );
    }

    void boxUI::showParty( pokemon* p_party, u8 p_partyLen ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        // Load some placeholder
        for( u8 i = MAX_PKMN_PER_BOX; i < MAX_PKMN_PER_BOX + p_partyLen; ++i ) {
            if( p_party[ i - MAX_PKMN_PER_BOX ].getSpecies( ) ) {
                IO::loadSpriteB(
                    SPR_PKMN_START_OAM_SUB + i, oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex,
                    oam[ SPR_PKMN_START_OAM_SUB + i ].x, oam[ SPR_PKMN_START_OAM_SUB + i ].y, 32,
                    32, NoPkmnPal, NoPkmnTiles, NoPkmnTilesLen, false, false, false, OBJPRIORITY_1,
                    true );
            } else {
                oam[ SPR_PKMN_START_OAM_SUB + i ].isHidden = true;
            }
        }
        IO::updateOAM( true );

        for( u8 i = MAX_PKMN_PER_BOX; i < MAX_PKMN_PER_BOX + p_partyLen; ++i ) {
            if( p_party[ i - MAX_PKMN_PER_BOX ].getSpecies( ) ) {
                if( !p_party[ i - MAX_PKMN_PER_BOX ].isEgg( ) ) {
                    IO::loadPKMNIconB(
                        p_party[ i - MAX_PKMN_PER_BOX ].getSpecies( ),
                        oam[ SPR_PKMN_START_OAM_SUB + i ].x,
                        oam[ SPR_PKMN_START_OAM_SUB + i ].y, SPR_PKMN_START_OAM_SUB + i,
                        oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex, true,
                        p_party[ i - MAX_PKMN_PER_BOX ].getForme( ),
                        p_party[ i - MAX_PKMN_PER_BOX ].isShiny( ),
                        p_party[ i - MAX_PKMN_PER_BOX ].isFemale( ) );
                } else {
                    IO::loadEggIconB(
                        oam[ SPR_PKMN_START_OAM_SUB + i ].x, oam[ SPR_PKMN_START_OAM_SUB + i ].y,
                        SPR_PKMN_START_OAM_SUB + i, oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex,
                        true, p_party[ i - MAX_PKMN_PER_BOX ].getSpecies( ) == PKMN_MANAPHY );
                }
            } else {
                oam[ SPR_PKMN_START_OAM_SUB + i ].isHidden = true;
            }
            oam[ SPR_PKMN_START_OAM_SUB + i ].priority = OBJPRIORITY_1;
        }
        IO::updateOAM( true );
    }

    void boxUI::hideParty( ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        for( u8 i = MAX_PKMN_PER_BOX; i < MAX_PKMN_PER_BOX + 6; ++i ) {
            oam[ SPR_PKMN_START_OAM_SUB + i ].isHidden = true;
        }
        IO::updateOAM( true );
    }

} // namespace BOX
