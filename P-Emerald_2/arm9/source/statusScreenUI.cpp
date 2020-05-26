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
#include "boxUI.h"
#include "defines.h"
#include "font.h"
#include "fs.h"
#include "item.h"
#include "move.h"
#include "ribbon.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

#include "anti_pokerus_icon.h"
#include "pokerus_icon.h"

#include "hpbar.h"
#include "itemicon.h"
#include "status_brn.h"
#include "status_fnt.h"
#include "status_frz.h"
#include "status_par.h"
#include "status_psn.h"
#include "status_shiny.h"
#include "status_slp.h"
#include "status_txc.h"

#include "NoItem.h"

#include "partybg.h"
#include "partybg2.h"
#include "statussub.h"
#include "statustop.h"

#include "infopage1.h"
#include "infopage2.h"
#include "infopage3.h"
#include "infopage4.h"
#include "window1.h"
#include "window2.h"
#include "window3.h"

namespace STS {
    // top screen sprites
#define SPR_WINDOW_START_OAM 0
#define SPR_INFOPAGE_START_OAM 5
#define SPR_PKMN_START_OAM 13
#define SPR_ITEM_OAM 14

#define SPR_INOFPAGE_PAL 0
#define SPR_PKMN_PAL 1
#define SPR_ITEM_PAL 2
#define SPR_WINDOW_PAL 3

    u16 statusScreenUI::initTopScreen( bool p_bottom ) {
        IO::clearScreen( p_bottom, false, true );
        if( p_bottom ) {
            dmaCopy( statustopBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        } else {
            dmaCopy( statustopBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 256 );
        }
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

        IO::loadSprite( SPR_INFOPAGE_START_OAM, SPR_INOFPAGE_PAL, tileCnt, INFO_X, INFO_Y, 64, 64,
                        0, 0, infopage1TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM + 7, SPR_INOFPAGE_PAL, tileCnt,
                                  INFO_X + 64, INFO_Y + 64 + 15, 64, 64, infopage1Pal,
                                  infopage1Tiles, infopage1TilesLen, true, true, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 1, SPR_INOFPAGE_PAL, tileCnt, INFO_X + 64, INFO_Y,
                        64, 64, 0, 0, infopage2TilesLen, false, false, false, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_INFOPAGE_START_OAM + 6, SPR_INOFPAGE_PAL, tileCnt, INFO_X,
                                  INFO_Y + 64 + 15, 64, 64, infopage2Pal, infopage2Tiles,
                                  infopage2TilesLen, true, true, false, OBJPRIORITY_3, p_bottom,
                                  OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 2, SPR_INOFPAGE_PAL, tileCnt, INFO_X, INFO_Y + 64,
                        32, 16, 0, 0, infopage3TilesLen, false, false, false, OBJPRIORITY_3,
                        p_bottom, OBJMODE_BLENDED );
        tileCnt
            = IO::loadSprite( SPR_INFOPAGE_START_OAM + 5, SPR_INOFPAGE_PAL, tileCnt, INFO_X + 96,
                              INFO_Y + 63, 32, 16, infopage3Pal, infopage3Tiles, infopage3TilesLen,
                              true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_INFOPAGE_START_OAM + 3, SPR_INOFPAGE_PAL, tileCnt, INFO_X + 32,
                        INFO_Y + 64, 32, 16, 0, 0, infopage4TilesLen, false, false, false,
                        OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt
            = IO::loadSprite( SPR_INFOPAGE_START_OAM + 4, SPR_INOFPAGE_PAL, tileCnt, INFO_X + 64,
                              INFO_Y + 63, 32, 16, infopage4Pal, infopage4Tiles, infopage4TilesLen,
                              true, true, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Window

        tileCnt = IO::loadSprite( SPR_WINDOW_START_OAM, SPR_WINDOW_PAL, tileCnt, INFO_X, INFO_Y, 64,
                                  32, window1Pal, window1Tiles, window1TilesLen, false, false,
                                  false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 1, SPR_WINDOW_PAL, tileCnt, INFO_X, INFO_Y + 32, 64,
                        32, 0, 0, window2TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        IO::loadSprite( SPR_WINDOW_START_OAM + 2, SPR_WINDOW_PAL, tileCnt, INFO_X, INFO_Y + 32 + 30,
                        64, 32, 0, 0, window2TilesLen, false, false, false, OBJPRIORITY_3, p_bottom,
                        OBJMODE_BLENDED );
        tileCnt
            = IO::loadSprite( SPR_WINDOW_START_OAM + 3, SPR_WINDOW_PAL, tileCnt, INFO_X,
                              INFO_Y + 64 + 28, 64, 32, window1Pal, window2Tiles, window2TilesLen,
                              false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( SPR_WINDOW_START_OAM + 4, SPR_WINDOW_PAL, tileCnt, INFO_X,
                                  INFO_Y + 111, 64, 32, window3Pal, window3Tiles, window3TilesLen,
                                  false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

        // Pkmn Sprite
        tileCnt = IO::loadSprite( SPR_PKMN_START_OAM, SPR_PKMN_PAL, tileCnt, 8, 54, 64, 64, 0, 0,
                                  96 * 96 / 2, false, false, false, OBJPRIORITY_2, p_bottom,
                                  OBJMODE_NORMAL );

        // Item icon
        tileCnt = IO::loadSprite( SPR_ITEM_OAM, SPR_ITEM_PAL, tileCnt, 72, 140, 32, 32, NoItemPal,
                                  NoItemTiles, NoItemTilesLen, false, false, true, OBJPRIORITY_1,
                                  p_bottom, OBJMODE_NORMAL );

        IO::updateOAM( p_bottom );
        return tileCnt;
    }

    u16 statusScreenUI::initBottomScreen( bool p_bottom ) {
        IO::clearScreen( p_bottom, false, true );
        if( p_bottom ) {
            dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        } else {
            dmaCopy( statussubBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 256 );
        }

        IO::initOAMTable( p_bottom );

        SpriteEntry* oam     = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          tileCnt = 0;

        return tileCnt;
    }

    void statusScreenUI::drawBasicInfoTop( pokemon* p_pokemon, bool p_bottom ) {
        if( p_bottom ) {
            dmaCopy( partybg2Bitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        } else {
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
        }
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        // Print Basic Pkmn info
        if( !p_pokemon->isEgg( ) ) {
            // Load correct sprite
            IO::loadPKMNSprite( p_pokemon->getSpecies( ), oam[ SPR_PKMN_START_OAM ].x,
                                oam[ SPR_PKMN_START_OAM ].y, SPR_PKMN_START_OAM, SPR_PKMN_PAL,
                                oam[ SPR_PKMN_START_OAM ].gfxIndex, p_bottom, p_pokemon->isShiny( ),
                                p_pokemon->isFemale( ), true, false, p_pokemon->getForme( ) );

            IO::regularFont->printString( p_pokemon->m_boxdata.m_name, 12, 34, p_bottom );
        } else {
            IO::loadEggSprite( oam[ SPR_PKMN_START_OAM ].x, oam[ SPR_PKMN_START_OAM ].y,
                               SPR_PKMN_START_OAM, SPR_PKMN_PAL, oam[ SPR_PKMN_START_OAM ].gfxIndex,
                               p_bottom );

            IO::regularFont->printString( GET_STRING( 34 ), 12, 34, p_bottom );
        }
        IO::updateOAM( p_bottom );
    }

    void statusScreenUI::writeLineTop( const char* p_string, u8 p_line, u8 p_color,
                                       bool p_bottom ) {
        IO::regularFont->setColor( p_color, 1 );
        IO::regularFont->printString( p_string, INFO_X + 12, INFO_Y + 11 + 15 * p_line, p_bottom );
    }

    void statusScreenUI::init( pokemon* p_pokemon, u8 p_initialPage, bool p_allowKeyUp,
                               bool p_allowKeyDown ) {
        _allowKeyUp   = p_allowKeyUp;
        _allowKeyDown = p_allowKeyDown;

        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::vramSetup( true );
        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

        initTopScreen( );
        initBottomScreen( );

        _data = getPkmnData( p_pokemon->getSpecies( ), p_pokemon->getForme( ) );

        draw( p_pokemon, p_initialPage );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );

        dmaCopy( partybgPal, BG_PALETTE, 3 * 2 );
        dmaCopy( statustopPal + 3, BG_PALETTE + 3, 8 * 2 );
        dmaCopy( partybgPal, BG_PALETTE_SUB, 3 * 2 );
        dmaCopy( statussubPal + 3, BG_PALETTE_SUB + 3, 8 * 2 );

        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = BG_PAL( i );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = IO::GRAY;
            pal[ IO::BLACK_IDX ] = IO::BLACK;
            pal[ IO::BLUE_IDX ]  = RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = RGB( 23, 0, 0 );

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
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = 0xff | ( 0x05 << 8 );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
        REG_BLDALPHA     = 0xff | ( 0x05 << 8 );
        bgUpdate( );
    }

    void statusScreenUI::draw( pokemon* p_pokemon, u8 p_page ) {
        if( _currentPage == p_page || !p_pokemon ) { return; }
        _currentPage = p_pokemon->isEgg( ) ? 0 : p_page;
        drawBasicInfoTop( p_pokemon, false );

        switch( _currentPage ) {
        case 0:
            break;
        default:
            break;
        }
    }

    void STS::statusScreenUI::animate( u8 p_frame ) {
        IO::animateBG( p_frame, IO::bg3 );
        IO::animateBG( p_frame, IO::bg3sub );
        bgUpdate( );
    }
} // namespace STS

//    void drawPkmnInformation( pokemon& p_pokemon, u8& p_page, bool p_newpok, bool p_bottom ) {
//        dmaFillWords( 0, bgGetGfxPtr( p_bottom ? IO::bg2sub : IO::bg2 ), 256 * 192 );
//        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal = BG_PAL( p_bottom );
//        if( p_newpok ) {
//            IO::loadItemIcon( 0 /* !p_pokemon.m_boxdata.m_ball
//                                  ? "Pokeball"
//                                  : ItemList[ p_pokemon.m_boxdata.m_ball ]->m_itemName */,
//                              -6, 22, SHINY_IDX, SHINY_PAL, 1000, p_bottom );
//        }
//
//        if( !p_pokemon.isEgg( ) ) {
//            pal[ IO::RED_IDX ]   = IO::RED;
//            pal[ IO::BLUE_IDX ]  = IO::BLUE;
//            pal[ IO::RED2_IDX ]  = IO::RED2;
//            pal[ IO::BLUE2_IDX ] = IO::BLUE2;
//            pal[ IO::WHITE_IDX ] = IO::WHITE;
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//
//            IO::regularFont->printString( p_pokemon.m_boxdata.m_name, 150, 0, p_bottom );
//            s8 G = p_pokemon.m_boxdata.gender( );
//
//            IO::regularFont->printChar( '/', 234, 0, p_bottom );
//            if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32
//            ) {
//                if( G == 1 ) {
//                    IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//                    IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//                    IO::regularFont->printChar( 136, 246, 7, p_bottom );
//                } else if( G == -1 ) {
//                    IO::regularFont->setColor( IO::RED_IDX, 1 );
//                    IO::regularFont->setColor( IO::RED2_IDX, 2 );
//                    IO::regularFont->printChar( 137, 246, 7, p_bottom );
//                }
//            }
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//
//
//            IO::regularFont->printString( getDisplayName( p_pokemon.m_boxdata.m_speciesId,
//                        CURRENT_LANGUAGE ).c_str( ), 160, 13, p_bottom );
//
//            if( p_pokemon.m_boxdata.getItem( ) ) {
//                IO::regularFont->printString( "Item", 2, 176, p_bottom );
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( 0, 2 );
//                char buffer[ 200 ];
//                snprintf(
//                    buffer, 199, "%s",
//                    ITEM::getItemName( p_pokemon.getItem( ) ).c_str( ) );
//                IO::regularFont->printString( buffer, 40, 159, p_bottom );
//                if( p_newpok ) {
//                    IO::loadItemIcon( p_pokemon.getItem( ), 2, 152, ITEM_ICON_IDX,
//                    ITEM_ICON_PAL,
//                                      Oam->oamBuffer[ ITEM_ICON_IDX ].gfxIndex, p_bottom );
//                }
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( 0, 2 );
//                IO::regularFont->printString(
//                    ITEM::getItemName( p_pokemon.m_boxdata.getItem( ) ).c_str( ),
//                    56, 168, p_bottom );
//                Oam->oamBuffer[ ITEM_ICON_IDX ].isHidden = true;
//            }
//            if( p_pokemon.m_boxdata.isShiny( ) ) {
//                IO::regularFont->setColor( IO::RED_IDX, 1 );
//                IO::regularFont->setColor( IO::WHITE_IDX, 2 );
//                IO::regularFont->printString( "*", 1, 47, p_bottom );
//                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//
//            if( p_newpok )
//                if( !IO::loadPKMNSprite(
//                        p_pokemon.m_boxdata.m_speciesId, 16, 44,
//                        PKMN_SPRITE_START, PKMN_SPRITE_PAL,
//                        Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom,
//                        p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale, true,
//                        false, p_pokemon.getForme( ) ) ) {
//                    IO::loadPKMNSprite(
//                        p_pokemon.m_boxdata.m_speciesId, 16, 44,
//                        PKMN_SPRITE_START, PKMN_SPRITE_PAL,
//                        Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom,
//                        p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale, true,
//                        false, p_pokemon.getForme( ) );
//                }
//
//            u16 exptype = data.m_expTypeFormeCnt >> 5;
//
//            IO::displayHP( 100, 101, 46, 76, HP_COL, HP_COL + 1, false, 50, 56, p_bottom );
//            IO::displayHP( 100, 100 - p_pokemon.m_stats.m_curHP * 100 /
//            p_pokemon.m_stats.m_maxHP,
//                           46, 76, HP_COL, HP_COL + 1, false, 50, 56, p_bottom );
//
//            IO::displayEP( 100, 101, 46, 76, HP_COL + 2, HP_COL + 3, false, 59, 62, p_bottom
//            ); IO::displayEP(
//                0,
//                ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][
//                exptype ] )
//                    * 100
//                    / ( EXP[ p_pokemon.m_level ][ exptype ]
//                        - EXP[ p_pokemon.m_level - 1 ][ exptype ] ),
//                46, 76, HP_COL + 2, HP_COL + 3, false, 59, 62, p_bottom );
//
//            IO::regularFont->setColor( 0, 2 );
//            IO::regularFont->setColor( HP_COL + 2, 1 );
//            char buffer[ 50 ];
//            snprintf(
//                buffer, 49, "EP %lu%%",
//                ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][
//                exptype ] )
//                    * 100
//                    / ( EXP[ p_pokemon.m_level ][ exptype ]
//                        - EXP[ p_pokemon.m_level - 1 ][ exptype ] ) );
//            IO::regularFont->printString( buffer, 62, 28, p_bottom, IO::font::CENTER );
//            snprintf( buffer, 49, "%s %i%%", GET_STRING( 126 ),
//                      p_pokemon.m_stats.m_curHP * 100 / p_pokemon.m_stats.m_maxHP );
//            IO::regularFont->printString( buffer, 62, 38, p_bottom, IO::font::CENTER );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        } else {
//            p_page           = -1;
//            pal[ IO::WHITE_IDX ] = IO::WHITE;
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//
//            IO::regularFont->printString( GET_STRING( 34 ), 150, 0, p_bottom );
//            IO::regularFont->printChar( '/', 234, 0, p_bottom );
//            IO::regularFont->printString( GET_STRING( 34 ), 160, 13, p_bottom );
//
//            IO::loadEggSprite( 29, 60, PKMN_SPRITE_START, PKMN_SPRITE_PAL,
//                               Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom );
//            for( u8 i = 1; i < 4; ++i ) Oam->oamBuffer[ PKMN_SPRITE_START + i ].isHidden =
//            true; Oam->oamBuffer[ ITEM_ICON_IDX ].isHidden = true;
//        }
//    }
//
//    void drawPkmnStats( const pokemon& p_pokemon, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal      = BG_PAL( p_bottom );
//
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false,
//                        OBJPRIORITY_0, p_bottom );
//
//        IO::regularFont->printString( GET_STRING( 137 ), 32, 0, p_bottom );
//        if( !( currPkmn.isEgg( ) ) ) {
//
//            pal[ IO::RED_IDX ]   = IO::RED;
//            pal[ IO::BLUE_IDX ]  = IO::BLUE;
//            pal[ IO::RED2_IDX ]  = IO::RED2;
//            pal[ IO::BLUE2_IDX ] = IO::BLUE2;
//
//            char buffer[ 50 ];
//            snprintf( buffer, 49, GET_STRING( 138 ), currPkmn.m_level );
//            IO::regularFont->printString( buffer, 110, 30, p_bottom );
//
//            snprintf( buffer, 49, "KP                     %3i", currPkmn.m_stats.m_maxHP );
//            IO::regularFont->printString( buffer, 130, 46, p_bottom );
//
//            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 0 ] == 1.1 ) {
//                IO::regularFont->setColor( IO::RED_IDX, 1 );
//                IO::regularFont->setColor( IO::RED2_IDX, 2 );
//            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 0 ] == 0.9 ) {
//                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//            snprintf( buffer, 49, "ANG                   %3i", currPkmn.m_stats.m_Atk );
//            IO::regularFont->printString( buffer, 130, 65, p_bottom );
//
//            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 1 ] == 1.1 ) {
//                IO::regularFont->setColor( IO::RED_IDX, 1 );
//                IO::regularFont->setColor( IO::RED2_IDX, 2 );
//            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 1 ] == 0.9 ) {
//                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//            snprintf( buffer, 49, "VER                   %3i", currPkmn.m_stats.m_Def );
//            IO::regularFont->printString( buffer, 130, 82, p_bottom );
//
//            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 3 ] == 1.1 ) {
//                IO::regularFont->setColor( IO::RED_IDX, 1 );
//                IO::regularFont->setColor( IO::RED2_IDX, 2 );
//            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 3 ] == 0.9 ) {
//                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//            snprintf( buffer, 49, "SAN                   %3i", currPkmn.m_stats.m_SAtk );
//            IO::regularFont->printString( buffer, 130, 99, p_bottom );
//
//            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 4 ] == 1.1 ) {
//                IO::regularFont->setColor( IO::RED_IDX, 1 );
//                IO::regularFont->setColor( IO::RED2_IDX, 2 );
//            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 4 ] == 0.9 ) {
//                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//            snprintf( buffer, 49, "SVE                   %3i", currPkmn.m_stats.m_SDef );
//            IO::regularFont->printString( buffer, 130, 116, p_bottom );
//
//            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 2 ] == 1.1 ) {
//                IO::regularFont->setColor( IO::RED_IDX, 1 );
//                IO::regularFont->setColor( IO::RED2_IDX, 2 );
//            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 2 ] == 0.9 ) {
//                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//            snprintf( buffer, 49,
//                      "INI                   \xC3\xC3"
//                      "%3i",
//                      currPkmn.m_stats.m_Spd );
//            IO::regularFont->printString( buffer, 130, 133, p_bottom );
//
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            IO::printRectangle( (u8) 158, (u8) 48, u8( 158 + 68 ), u8( 48 + 12 ), p_bottom,
//                                (u8) 251 );
//
//            IO::printRectangle( (u8) 158, (u8) 48,
//                                u8( 158 + ( 68.0 * currPkmn.m_boxdata.IVget( 0 ) / 31 ) ),
//                                u8( 48 + 6 ), p_bottom, 230 );
//            IO::printRectangle( (u8) 158, u8( 48 + 6 ),
//                                u8( 158 + ( 68.0 * currPkmn.m_boxdata.m_effortValues[ 0 ] /
//                                252 ) ), u8( 48 + 12 ), p_bottom, 230 );
//
//            for( int i = 1; i < 6; ++i ) {
//                IO::printRectangle( (u8) 158, u8( 50 + ( 17 * i ) ), u8( 158 + 68 ),
//                                    u8( 50 + 12 + ( 17 * i ) ), p_bottom, (u8) 251 );
//                IO::printRectangle( (u8) 158, u8( 50 + ( 17 * i ) ),
//                                    u8( 158 + ( 68.0 * currPkmn.m_boxdata.IVget( i ) / 31 ) ),
//                                    u8( 50 + 6 + ( 17 * i ) ), p_bottom, 230 + i );
//                IO::printRectangle(
//                    (u8) 158, u8( 50 + 6 + ( 17 * i ) ),
//                    u8( 158 + ( 68.0 * currPkmn.m_boxdata.m_effortValues[ i ] / 252 ) ),
//                    u8( 50 + 12 + ( 17 * i ) ), p_bottom, 230 + i );
//            }
//        } else {
//            if( currPkmn.m_boxdata.m_steps > 10 ) {
//                IO::regularFont->printString( "Was da wohl", 16 * 8, 46, p_bottom );
//                IO::regularFont->printString( "schlüpfen wird?", 16 * 8, 66, p_bottom );
//                IO::regularFont->printString( "Es dauert wohl", 16 * 8, 96, p_bottom );
//                IO::regularFont->printString( "noch lange.", 16 * 8, 116, p_bottom );
//            } else if( currPkmn.m_boxdata.m_steps > 5 ) {
//                IO::regularFont->printString( "Hat es sich", 16 * 8, 46, p_bottom );
//                IO::regularFont->printString( "gerade bewegt?", 16 * 8, 66, p_bottom );
//                IO::regularFont->printString( "Da tut sich", 16 * 8, 96, p_bottom );
//                IO::regularFont->printString( "wohl bald was.", 16 * 8, 116, p_bottom );
//            } else {
//                IO::regularFont->printString( "Jetzt macht es", 16 * 8, 46, p_bottom );
//                IO::regularFont->printString( "schon Geräusche!", 16 * 8, 66, p_bottom );
//                IO::regularFont->printString( "Bald ist es", 16 * 8, 96, p_bottom );
//                IO::regularFont->printString( "wohl soweit.", 16 * 8, 116, p_bottom );
//            }
//        }
//    }
//
//    void drawPkmnMoves( const pokemon& p_pokemon, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal      = BG_PAL( p_bottom );
//
//        IO::regularFont->printString( "Attacken", 32, 0, p_bottom );
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false,
//                        OBJPRIORITY_0, p_bottom );
//
//        for( int i = 0; i < 4; i++ ) {
//            if( !currPkmn.m_boxdata.m_moves[ i ] ) continue;
//
//            auto mdata = MOVE::getMoveData( currPkmn.m_boxdata.m_moves[ i ] );
//
//            type t = mdata.m_type;
//            IO::loadTypeIcon( t, 222, 38 + 30 * i, TYPE_IDX + i, TYPE_PAL( i ),
//                              Oam->oamBuffer[ TYPE_IDX + i ].gfxIndex, p_bottom,
//                              SAVE::SAV.getActiveFile( ).m_options.m_language );
//
//            pal[ IO::COLOR_IDX ] = IO::GREEN;
//            if( t == data.m_baseForme.m_types[ 0 ] || t == data.m_baseForme.m_types[ 1 ] ) {
//                IO::regularFont->setColor( IO::COLOR_IDX, 1 );
//                IO::regularFont->setColor( IO::WHITE_IDX, 2 );
//            } else {
//                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//            }
//
//            IO::regularFont->printString( MOVE::getMoveName( currPkmn.m_boxdata.m_moves[ i ],
//                        CURRENT_LANGUAGE ).c_str( ), 128, 30 + 30 * i, p_bottom );
//
//            IO::regularFont->setColor( IO::GRAY_IDX, 1 );
//            IO::regularFont->setColor( IO::WHITE_IDX, 2 );
//            char buffer[ 50 ];
//            snprintf( buffer, 49, "AP %2hhu/%2hhu ", currPkmn.m_boxdata.m_curPP[ i ],
//                      s8( mdata.m_pp * ( ( 5 + currPkmn.m_boxdata.PPupget( i ) ) / 5.0 ) ) );
//            IO::regularFont->printString( buffer, 135, 45 + 30 * i, p_bottom );
//        }
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//    }
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
//    void drawPkmnGeneralData( const pokemon& p_pokemon, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false,
//                        OBJPRIORITY_0, p_bottom );
//        IO::regularFont->printString( "Trainer-Memo", 32, 0, p_bottom );
//
//        if( data.m_baseForme.m_types[ 0 ] == data.m_baseForme.m_types[ 1 ] ) {
//            IO::loadTypeIcon( data.m_baseForme.m_types[ 0 ], 250 - 32, 50, TYPE_IDX, TYPE_PAL(
//            0 ),
//                              Oam->oamBuffer[ TYPE_IDX ].gfxIndex, p_bottom,
//                              SAVE::SAV.getActiveFile( ).m_options.m_language );
//            Oam->oamBuffer[ TYPE_IDX + 1 ].isHidden = true;
//        } else {
//            IO::loadTypeIcon( data.m_baseForme.m_types[ 0 ], 250 - 64, 50, TYPE_IDX, TYPE_PAL(
//            0 ),
//                              Oam->oamBuffer[ TYPE_IDX ].gfxIndex, p_bottom,
//                              SAVE::SAV.getActiveFile( ).m_options.m_language );
//            IO::loadTypeIcon( data.m_baseForme.m_types[ 1 ], 250 - 32, 50, TYPE_IDX + 1,
//            TYPE_PAL( 1 ),
//                              Oam->oamBuffer[ TYPE_IDX + 1 ].gfxIndex, p_bottom,
//                              SAVE::SAV.getActiveFile( ).m_options.m_language );
//        }
//
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//
//        IO::regularFont->printString( "OT", 110, 30, p_bottom );
//        if( currPkmn.m_boxdata.m_oTisFemale ) {
//            IO::regularFont->setColor( IO::RED_IDX, 1 );
//            IO::regularFont->setColor( IO::RED2_IDX, 2 );
//        } else {
//            IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//            IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//        }
//        char buffer[ 50 ];
//        snprintf( buffer, 49, "%s/%05d", currPkmn.m_boxdata.m_oT, currPkmn.m_boxdata.m_oTId );
//        IO::regularFont->printString( buffer, 250, 30, p_bottom, IO::font::RIGHT );
//
//        if( !currPkmn.m_boxdata.isShiny( ) ) {
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        }
//        snprintf( buffer, 49, "%03d", currPkmn.m_boxdata.m_speciesId );
//        IO::regularFont->printString( buffer, 180, 51, p_bottom, IO::font::RIGHT );
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        IO::regularFont->printString( "Nr.", 124, 51, p_bottom );
//
//        bool plrOT = currPkmn.m_boxdata.m_oTId == SAVE::SAV.getActiveFile( ).m_id
//                     && currPkmn.m_boxdata.m_oTSid == SAVE::SAV.getActiveFile( ).m_sid;
//        if( !currPkmn.m_boxdata.m_gotDate[ 0 ] )
//            snprintf( buffer, 49, "%s%d", plrOT ? "Gef. auf Lv. " : "Off gef auf Lv.",
//                      currPkmn.m_boxdata.m_gotLevel );
//        else
//            snprintf( buffer, 49, "%s", plrOT ? "Ei erhalten" : "Ei offenbar erh." );
//        IO::regularFont->printString( buffer, 250, 76, p_bottom, IO::font::RIGHT, 14 );
//        snprintf( buffer, 49, "am %02d.%02d.20%02d,",
//                  currPkmn.m_boxdata.m_gotDate[ 0 ] ? currPkmn.m_boxdata.m_gotDate[ 0 ]
//                                                    : currPkmn.m_boxdata.m_hatchDate[ 0 ],
//                  currPkmn.m_boxdata.m_gotDate[ 1 ] ? currPkmn.m_boxdata.m_gotDate[ 1 ]
//                                                    : currPkmn.m_boxdata.m_hatchDate[ 1 ],
//                  currPkmn.m_boxdata.m_gotDate[ 2 ] ? currPkmn.m_boxdata.m_gotDate[ 2 ]
//                                                    : currPkmn.m_boxdata.m_hatchDate[ 2 ] );
//        IO::regularFont->printString( buffer, 250, 90, p_bottom, IO::font::RIGHT, 14 );
//        snprintf( buffer, 49, "%s.", FS::getLocation( currPkmn.m_boxdata.m_gotPlace,
//                    CURRENT_LANGUAGE ).c_str( ) );
//        IO::regularFont->printMaxString(
//            buffer, std::max( (u32) 124, 250 - IO::regularFont->stringWidth( buffer ) ), 104,
//            p_bottom, 254 );
//
//        if( currPkmn.m_boxdata.m_gotDate[ 0 ] ) {
//            snprintf( buffer, 49, "Geschl. %02d.%02d.20%02d,", currPkmn.m_boxdata.m_hatchDate[
//            0 ],
//                      currPkmn.m_boxdata.m_hatchDate[ 1 ], currPkmn.m_boxdata.m_hatchDate[ 2 ]
//                      );
//            IO::regularFont->printString( buffer, 250, 125, p_bottom, IO::font::RIGHT, 14 );
//            snprintf( buffer, 49, "%s.",
//                      FS::getLocation( currPkmn.m_boxdata.m_hatchPlace,
//                          CURRENT_LANGUAGE ).c_str( ) );
//            IO::regularFont->printString( buffer, 250, 139, p_bottom, IO::font::RIGHT, 14 );
//        } else if( plrOT && currPkmn.m_boxdata.m_fateful )
//            IO::regularFont->printString( "Schicksalhafte Begeg.", 102, 139, p_bottom );
//        else if( currPkmn.m_boxdata.m_fateful )
//            IO::regularFont->printString( "Off. schicksal. Begeg.", 102, 139, p_bottom );
//    }
//
//    void drawPkmnNature( const pokemon& p_pokemon, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//
//        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX
//        ].gfxIndex, 0,
//                        -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false,
//                        OBJPRIORITY_0, p_bottom );
//        IO::regularFont->printString( "Pokémon-Info", 32, 0, p_bottom );
//
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//
//        char buffer[ 50 ];
//        snprintf( buffer, 49,
//                  "Mag %s"
//                  "e PokéRg.",
//                  GET_STRING( 242 + currPkmn.m_boxdata.getTasteStr( ) ) );
//        IO::regularFont->printString( buffer, 250, 30, p_bottom, IO::font::RIGHT );
//
//        snprintf( buffer, 49, "Sehr %s; %s.",
//                  GET_STRING( 187 + currPkmn.m_boxdata.getNature( ) ),
//                  GET_STRING( 212 + currPkmn.m_boxdata.getPersonality( ) ) );
//        auto str  = std::string( buffer );
//        auto nStr = FS::breakString( str, IO::regularFont, 122 );
//        IO::regularFont->printString( nStr.c_str( ), 245, 48, p_bottom, IO::font::RIGHT, 14 );
//
//        auto curAbName = getAbilityName( currPkmn.m_boxdata.m_ability );
//        u8   wd        = IO::regularFont->stringWidth( curAbName.c_str( ) );
//        if( 250 - wd > 140 )
//            IO::regularFont->printString( "Fäh. ", 250 - wd, 94, p_bottom, IO::font::RIGHT );
//        u8 nlCnt = 0;
//        /*
//        nStr     = FS::breakString( acAbility.m_flavourText, IO::regularFont, 130 );
//        for( auto c : nStr )
//            if( c == '\n' ) nlCnt++;
//        IO::regularFont->printString( nStr.c_str( ), 250, 108, p_bottom, IO::font::RIGHT,
//                                      u8( 16 - 2 * nlCnt ) );
//        */
//
//        if( currPkmn.m_boxdata.m_oTisFemale ) {
//            IO::regularFont->setColor( IO::RED_IDX, 1 );
//            IO::regularFont->setColor( IO::RED2_IDX, 2 );
//        } else {
//            IO::regularFont->setColor( IO::BLUE_IDX, 1 );
//            IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
//        }
//        IO::regularFont->printString( curAbName.c_str( ), 250 - wd, 94, p_bottom );
//        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//    }
//
//    // Draw extra information about the specified move
//    bool statusScreenUI::drawMove( const pokemon& p_pokemon, u8 p_moveIdx, bool p_bottom ) {
//        auto currPkmn = p_pokemon;
//        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
//        auto pal      = BG_PAL( p_bottom );
//
//        if( !currPkmn.m_boxdata.m_moves[ p_moveIdx ] ) return false;
//        if( currPkmn.isEgg( ) ) return false;
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
//        auto mdata = MOVE::getMoveData( currPkmn.m_boxdata.m_moves[ p_moveIdx ] );
//
//        pal[ IO::COLOR_IDX ] = IO::GREEN;
//        if( mdata.m_type == data.m_baseForme.m_types[ 0 ]
//            || mdata.m_type == data.m_baseForme.m_types[ 1 ] ) {
//            IO::regularFont->setColor( IO::COLOR_IDX, 1 );
//            IO::regularFont->setColor( IO::WHITE_IDX, 2 );
//        } else {
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        }
//        IO::regularFont->printString( MOVE::getMoveName( currPkmn.m_boxdata.m_moves[ p_moveIdx
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
//            currPkmn.m_boxdata.m_curPP[ p_moveIdx ], mdata.m_pp
//            * ( ( 5 + ( ( currPkmn.m_boxdata.m_pPUps >> ( 2 * p_moveIdx ) ) % 4 ) ) / 5 ) );
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
//        if( currPkmn.isEgg( ) ) return false;
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
//                                 ? ( currPkmn.m_boxdata.m_fateful
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
//    void regStsScreenUI::draw( pokemon& p_pokemon, u8 p_page, bool p_newpok ) {
//        hideSprites( false );
//
//        IO::setDefaultConsoleTextColors( BG_PALETTE, 230 );
//        // FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PKMNInfoScreen", 420
//        ); IO::updateOAM( false );
//
//        IO::regularFont->setColor( 0, 0 );
//        {
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        }
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        IO::boldFont->setColor( 0, 0 );
//        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
//        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
//
//        BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
//        BG_PALETTE[ IO::GRAY_IDX ]  = RGB( 20, 20, 20 );
//        BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
//        if( p_newpok ) {
//            BG_PALETTE_SUB[ IO::GRAY_IDX ]  = RGB( 10, 10, 10 );
//            BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
//        }
//        auto currPkmn = p_pokemon;
//
//        drawPkmnInformation( currPkmn, p_page, p_newpok, false );
//        if( p_page == (u8) -1 ) p_page = 2;
//        switch( p_page ) {
//        case 0:
//            drawPkmnGeneralData( currPkmn, false );
//            break;
//        case 1:
//            drawPkmnNature( currPkmn, false );
//            break;
//        case 2:
//            drawPkmnStats( currPkmn, false );
//            break;
//        case 3:
//            drawPkmnMoves( currPkmn, false );
//            break;
//        case 4:
//            drawPkmnRibbons( currPkmn, false );
//            break;
//        default:
//            return;
//        }
//        for( u8 i = 0; i < 5; ++i )
//            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].isHidden
//                = currPkmn.isEgg( );
//        IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( p_page ) ].isHidden
//            = !currPkmn.isEgg( );
//
//        if( p_newpok ) {
//            IO::Oam->oamBuffer[ FWD_ID ].isHidden = false;
//            IO::Oam->oamBuffer[ BWD_ID ].isHidden = false;
//        }
//        IO::updateOAM( true );
//        IO::updateOAM( false );
//    }
//
//    std::vector<IO::inputTarget> regStsScreenUI::draw( u8 p_current, bool p_updatePageIcons )
//    {
//        drawPartyPkmn( &SAVE::SAV.getActiveFile( ).m_pkmnTeam[ _current ], _current, false,
//                       false );
//        _current  = p_current;
//        auto pkmn = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ p_current ];
//        drawPartyPkmn( &pkmn, p_current, true, false );
//
//        std::vector<IO::inputTarget> res;
//        std::vector<std::string>     names;
//
//        NAV::draw( );
//        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::CHOICE_COLOR;
//        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
//        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
//
//        if( p_updatePageIcons ) {
//            for( u8 i = 0; i < 5; ++i )
//                IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].isHidden
//                    = pkmn.isEgg( );
//            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( 2 ) ].isHidden = false;
//        }
//
//        for( u8 i = 0; i < 6; ++i ) {
//            auto p = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata;
//            if( p.isEgg( ) )
//                IO::loadEggIcon( 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ), SUB_BALL_IDX( i ),
//                                 IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex );
//            else if( i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) )
//                IO::loadPKMNIcon( p.m_speciesId, 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ),
//                                  SUB_BALL_IDX( i ),
//                                  IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex, true,
//                                  p.getForme( ), p.isShiny( ), p.isFemale( ) );
//            else
//                IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].isHidden = true;
//        }
//
//        IO::updateOAM( true );
//        if( pkmn.isEgg( ) ) return res;
//
//        /*
//        for( u8 i = 0; i < 4; ++i )
//            if( pkmn.m_boxdata.m_moves[ i ] < MAX_ATTACK
//                && AttackList[ pkmn.m_boxdata.m_moves[ i ] ]->m_isFieldAttack )
//                names.push_back( AttackList[ pkmn.m_boxdata.m_moves[ i ] ]->m_moveName );
//
//        if( pkmn.m_boxdata.m_holdItem )
//            names.push_back( "Item nehmen" );
//        else
//            names.push_back( "Item geben" );
//        names.push_back( "Dexeintrag" );
//        for( u8 i = 0; i < names.size( ); ++i ) {
//            auto            sz = res.size( );
//            IO::inputTarget cur( 37 + 100 * ( sz % 2 ), 48 + 41 * ( sz / 2 ),
//                                 132 + 100 * ( sz % 2 ), 80 + 41 * ( sz / 2 ) );
//            IO::printChoiceBox( cur.m_targetX1, cur.m_targetY1, cur.m_targetX2,
//            cur.m_targetY2, 6,
//                                COLOR_IDX, false );
//            IO::regularFont->printString( names[ i ].c_str( ),
//                                          ( cur.m_targetX2 + cur.m_targetX1 - 2 ) / 2,
//                                          cur.m_targetY1 + 8, true, IO::font::CENTER );
//            res.push_back( cur );
//        }
//        */
//        return res;
//    }
//
//    void boxStsScreenUI::init( ) {
//        // Remember: the storage sys swaps the screens.
//
//        IO::regularFont->setColor( 0, 0 );
//        {
//            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
//            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        }
//        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
//        IO::boldFont->setColor( 0, 0 );
//        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
//        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
//
//        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
//        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = RGB( 20, 20, 20 );
//        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
//
//        u16 tileCnt                                      = 0;
//        IO::Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex = tileCnt;
//        tileCnt += 144;
//
//        tileCnt
//            = IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, tileCnt, 0, 0, 32, 32, memoPal,
//                              memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0,
//                              true );
//        tileCnt = IO::loadItemIcon( 0, 2, 152, ITEM_ICON_IDX, ITEM_ICON_PAL,
//                                    tileCnt );
//
//        for( u8 i = 0; i < 4; ++i ) {
//            type t  = UNKNOWN;
//            tileCnt = IO::loadTypeIcon( t, 126, 43 + 32 * i, TYPE_IDX + i, TYPE_PAL( i ),
//            tileCnt,
//                                        true, SAVE::SAV.getActiveFile( ).m_options.m_language
//                                        );
//        }
//        for( u8 i = 0; i < 4; ++i ) {
//            tileCnt = IO::loadDamageCategoryIcon( ( MOVE::moveHitTypes )( i % 3 ), 126, 43 +
//            32 * i,
//                                                  ATK_DMGTYPE_IDX( i ), DMG_TYPE_PAL( i % 3 ),
//                                                  tileCnt, true );
//        }
//
//        for( u8 i = PKMN_SPRITE_START; i < RIBBON_IDX; ++i )
//            IO::Oam->oamBuffer[ i ].isHidden = true;
//        IO::setDefaultConsoleTextColors( BG_PALETTE_SUB, 230 );
//        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "PKMNInfoScreen", 420,
//                             49162, true );
//    }
//
//    void boxStsScreenUI::draw( pokemon& p_pokemon, u8 p_page, bool p_newpok ) {
//        // Remember: the storage sys swaps the screens.
//        // Only draw on the sub screen
//        hideSprites( true );
//
//        IO::setDefaultConsoleTextColors( BG_PALETTE_SUB, 6 );
//        drawPkmnInformation( p_pokemon, p_page, p_newpok, true );
//        if( p_page == (u8) -1 ) p_page = 2;
//        switch( p_page ) {
//        case 0:
//            drawPkmnGeneralData( p_pokemon, true );
//            break;
//        case 1:
//            drawPkmnNature( p_pokemon, true );
//            break;
//        case 2:
//            drawPkmnStats( p_pokemon, true );
//            break;
//        case 3:
//            drawPkmnMoves( p_pokemon, true );
//            break;
//        case 4:
//            drawPkmnRibbons( p_pokemon, true );
//            break;
//        default:
//            break;
//        }
//        for( u8 i = 0; i < 5; ++i )
//            IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].isHidden
//                = p_pokemon.isEgg( );
//        IO::OamTop->oamBuffer[ PAGE_ICON_START + p_page ].isHidden
//            = !p_pokemon.isEgg( );
//        IO::updateOAM( false );
//        IO::updateOAM( true );
//    }
//} // namespace STS
