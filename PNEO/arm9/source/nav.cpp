/*
Pokémon neo
------------------------------

file        : nav.cpp
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

#include <map>

#include "bagViewer.h"
#include "battle.h"
#include "battleTrainer.h"
#include "boxViewer.h"
#include "choiceBox.h"
#include "counter.h"
#include "defines.h"
#include "dex.h"
#include "fs.h"
#include "item.h"
#include "itemNames.h"
#include "keyboard.h"
#include "locationNames.h"
#include "mapDrawer.h"
#include "mapObject.h"
#include "mapSlice.h"
#include "moveNames.h"
#include "nav.h"
#include "partyScreen.h"
#include "pokemon.h"
#include "screenFade.h"
#include "sound.h"
#include "specials.h"
#include "sprite.h"
#include "statusScreen.h"
#include "uio.h"
#include "yesNoBox.h"

namespace NAV {
#define SPR_MSGTEXT_OAM 108
#define SPR_MSGCONT_OAM 112
#define SPR_MSGBOX_OAM  113

#define SPR_MSG_GFX     348
#define SPR_MSG_EXT_GFX 220
#define SPR_MSGBOX_GFX  476
#define SPR_MSGCONT_GFX 508

#define SPR_MENU_OAM_SUB( p_idx )         ( 0 + ( p_idx ) )
#define SPR_ITEM_OAM_SUB( p_i )           ( 0 + ( p_i ) )
#define SPR_MENU_SEL_OAM_SUB              6
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 7 + 8 * ( p_pos ) )
#define SPR_X_OAM_SUB                     56
#define SPR_ARROW_UP_OAM_SUB( p_i )       ( 57 + ( p_i ) )
#define SPR_ARROW_DOWN_OAM_SUB( p_i )     ( 63 + ( p_i ) )
#define SPR_MSGBOX_OAM_SUB                70

#define SPR_MENU_PAL_SUB( p_idx ) ( 0 + ( p_idx ) )
#define SPR_ITEM_PAL_SUB( p_idx ) ( 0 + ( p_idx ) )
#define SPR_MENU_SEL_PAL_SUB      6
#define SPR_BOX_PAL_SUB           7
#define SPR_BOX_SEL_PAL_SUB       8
#define SPR_X_PAL_SUB             9
#define SPR_MSGBOX_PAL_SUB        10

    char TMP_TEXT_BUF[ 512 ]  = { 0 };
    u16  CONT_BUF[ 16 * 16 ]  = { 0 };
    u16  TEXT_BUF[ 64 * 256 ] = { 0 };
    u16  TEXT_PAL[ 16 ]       = { 0, IO::BLACK, IO::GRAY, IO::WHITE, IO::BLUE, IO::BLUE };
    char TEXT_CACHE_1[ 256 ]  = { 0 }; // top line
    char TEXT_CACHE_2[ 256 ]  = { 0 }; // bottom line

    const u16 ARR_X_SPR_PAL[ 16 ] = {
        0x7FFF, 0x5A6E, 0x6F2D, 0x564A, // arrow_up
        0x001F, 0x0011, 0x18CE          // x_16_16
    };

    void hideMessageBox( ) {
        for( u8 i = 0; i < 14; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MSGBOX_OAM + i ].isHidden = true;
        }
        IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + i ].isHidden = true;
        }
        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
        std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
        IO::updateOAM( false );
    }

    void init( bool p_noPic, bool p_bottom ) {
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        if( !p_noPic ) { IO::clearScreen( p_bottom, false, true ); }
        IO::initOAMTable( p_bottom );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        auto ptr  = !p_bottom ? bgGetGfxPtr( IO::bg2 ) : bgGetGfxPtr( IO::bg2sub );
        auto ptr3 = !p_bottom ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        // auto pal  = !p_bottom ? BG_PALETTE : BG_PALETTE_SUB;

        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        bgUpdate( );

        if( !p_noPic ) {
            FS::readPictureData(
                ptr3, "nitro:/PICS/NAV/",
                std::to_string( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx ).c_str( ), 192 * 2,
                192 * 256, p_bottom );
            FS::readPictureData( ptr, "nitro:/PICS/", "Border", 64, 192, 192 * 256, p_bottom );
        }

        u16 tileCnt = 0;

        // Main menu icons
        tileCnt = IO::loadSprite( "MM/party", SPR_MENU_OAM_SUB( 0 ), SPR_MENU_PAL_SUB( 0 ), tileCnt,
                                  256 - 29, 192 - 6 * 29, 32, 32, false, false,
                                  p_noPic || !SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ),
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite(
            "MM/dex", SPR_MENU_OAM_SUB( 1 ), SPR_MENU_PAL_SUB( 1 ), tileCnt, 256 - 29, 192 - 5 * 29,
            32, 32, false, false,
            p_noPic || !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_DEX_OBTAINED ), OBJPRIORITY_2,
            p_bottom );
        tileCnt = IO::loadSprite( "MM/bag", SPR_MENU_OAM_SUB( 2 ), SPR_MENU_PAL_SUB( 2 ), tileCnt,
                                  256 - 29, 192 - 4 * 29, 32, 32, false, false, p_noPic,
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/id", SPR_MENU_OAM_SUB( 3 ), SPR_MENU_PAL_SUB( 3 ), tileCnt,
                                  256 - 29, 192 - 3 * 29, 32, 32, false, false, p_noPic,
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/save", SPR_MENU_OAM_SUB( 4 ), SPR_MENU_PAL_SUB( 4 ), tileCnt,
                                  256 - 29, 192 - 2 * 29, 32, 32, false, false, p_noPic,
                                  OBJPRIORITY_2, p_bottom );
        tileCnt = IO::loadSprite( "MM/settings", SPR_MENU_OAM_SUB( 5 ), SPR_MENU_PAL_SUB( 5 ),
                                  tileCnt, 256 - 29, 192 - 1 * 29, 32, 32, false, false, p_noPic,
                                  OBJPRIORITY_2, p_bottom );

        tileCnt = IO::loadSprite( "MM/select", SPR_MENU_SEL_OAM_SUB, SPR_MENU_SEL_PAL_SUB, tileCnt,
                                  256 - 31, 192 - 1 * 31, 32, 32, false, false, true, OBJPRIORITY_2,
                                  p_bottom );

        // x
        tileCnt = IO::loadSprite( "UI/x_16_16", SPR_X_OAM_SUB, SPR_X_PAL_SUB, tileCnt, 236, 172, 16,
                                  16, false, false, true, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        // arrows
        for( u8 i = 0; i < 6; ++i ) {
            IO::loadSprite( SPR_ARROW_UP_OAM_SUB( i ), SPR_X_PAL_SUB, tileCnt, 0, 0, 16, 16, 0, 0,
                            0, false, false, true, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
            IO::loadSprite( SPR_ARROW_DOWN_OAM_SUB( i ), SPR_X_PAL_SUB, tileCnt, 0, 0, 16, 16, 0, 0,
                            0, true, true, true, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );
        }
        tileCnt = IO::loadSprite( "UI/arrow_up", SPR_ARROW_UP_OAM_SUB( 0 ), SPR_X_PAL_SUB, tileCnt,
                                  0, 0, 16, 16, false, false, true, OBJPRIORITY_1, p_bottom,
                                  OBJMODE_NORMAL );

        // mbox
        for( u8 i = 0; i < 9; ++i ) {
            IO::loadSprite( SPR_MSGBOX_OAM_SUB + 9 - i, SPR_MSGBOX_PAL_SUB, tileCnt, 64 + 16 * i,
                            32, 32, 64, 0, 0, 0, false, true, true, OBJPRIORITY_3, true );
        }
        tileCnt = IO::loadSprite( "UI/mbox1", SPR_MSGBOX_OAM_SUB, SPR_MSGBOX_PAL_SUB, tileCnt, 32,
                                  32, 32, 64, false, false, true, OBJPRIORITY_3, true );

        // Choice boxes

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i;

            if( !i ) {
                tileCnt
                    = IO::loadSprite( "SEL/noselection_96_32_1", SPR_CHOICE_START_OAM_SUB( pos ),
                                      SPR_BOX_PAL_SUB, tileCnt, 29, 42 + i * 36, 16, 32, false,
                                      false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                tileCnt = IO::loadSprite( "SEL/noselection_96_32_2",
                                          SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                          tileCnt, 29 + 11, 42 + i * 36, 16, 32, false, false, true,
                                          OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            } else {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29, 42 + i * 36, 16,
                                32, 0, 0, 0, false, false, true, OBJPRIORITY_3, p_bottom,
                                OBJMODE_BLENDED );
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
            for( u8 j = 2; j < 7; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29 + 5 * 16, 42 + i * 36,
                            16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
        }

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, 42 + i * 36, 16, 32,
                            0, 0, 0, false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            for( u8 j = 1; j < 7; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 7, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131 + 5 * 16,
                            42 + i * 36, 16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_BLENDED );
        }

        IO::copySpritePal( ARR_X_SPR_PAL, SPR_X_PAL_SUB, 0, 2 * 7, p_bottom );
        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );
        IO::updateOAM( p_bottom );
        if( !p_noPic ) { hideMessageBox( ); }
    }

    void _printMessage( const char* p_message ) {
        printMessage( p_message );
    }

    constexpr u8 BGforLocation( u16 p_locId ) {
        switch( p_locId ) {
        case L_HIDDEN_LAKE:
        case L_ROUTE_38: return 7;
        case L_MAUVILLE_CITY:
        case L_PETALBURG_CITY:
        case L_DEWFORD_TOWN:
        case L_MOSSDEEP_CITY: return 3;
        case L_ROUTE_105:
        case L_ROUTE_106:
        case L_ROUTE_107:
        case L_ROUTE_108:
        case L_ROUTE_109: return 2;
        case L_RUSTBORO_CITY:
        case L_SLATEPORT_CITY:
        case L_SOOTOPOLIS_CITY:
        case L_CLIFFELTA_CITY: return 6;
        case L_METEOR_FALLS:
        case L_SEALED_CHAMBER:
        case L_ANCIENT_TOMB:
        case L_ISLAND_CAVE:
        case L_DESERT_RUINS:
        case L_SHOAL_CAVE:
        case L_RUSTURF_TUNNEL:
        case L_GRANITE_CAVE: return 4;
        case L_POKEMON_MART:
        case L_POKEMON_CENTER:
        case L_RUSTBORO_GYM:
        case L_DEWFORD_GYM:
        case L_MAUVILLE_GYM:
        case L_LAVARIDGE_GYM:
        case L_PETALBURG_GYM:
        case L_FORTREE_GYM:
        case L_MOSSDEEP_GYM:
        case L_SOOTOPOLIS_GYM:
        case L_PROF_BIRCH_S_LAB:
        case L_LILYCOVE_MUSEUM: return 1;
        case L_PRETTY_PETAL:
        case L_FORTREE_CITY:
        case L_PETALBURG_WOODS: return 5;
        default: return 0;
        }
    }

    u16  CURRENT_LOCATION = 0;
    u8   LOCATION_TIMER   = 0;
    void showNewLocation( u16 p_newLocation ) {
        if( p_newLocation == CURRENT_LOCATION ) { return; }

        if( p_newLocation == L_POKEMON_CENTER || p_newLocation == L_POKEMON_MART ) { return; }

        CURRENT_LOCATION = p_newLocation;
        LOCATION_TIMER   = 120;

        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        if( BGforLocation( p_newLocation ) == 3 || BGforLocation( p_newLocation ) == 6
            || BGforLocation( p_newLocation ) == 0 ) {
            IO::regularFont->setColor( 1, 1 );
        } else {
            IO::regularFont->setColor( 3, 1 );
        }
        IO::regularFont->setColor( 2, 2 );
        IO::regularFont->printStringBC( ( FS::getLocation( p_newLocation ) + " " ).c_str( ),
                                        TEXT_PAL, TEXT_BUF, 128, IO::font::CENTER );
        u16 tileCnt = SPR_MSG_GFX;
        u16 x = 0, y = 8;
        tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM, tileCnt, x, y, 64, 32, TEXT_BUF, 64 * 32 / 2,
                                   false, false, false, OBJPRIORITY_0, false );
        tileCnt
            = IO::loadSpriteB( SPR_MSGTEXT_OAM + 1, tileCnt, x + 64, y, 64, 32, TEXT_BUF + 64 * 32,
                               64 * 32 / 2, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadLocationBackB( BGforLocation( p_newLocation ), 1, 1, SPR_MSGTEXT_OAM + 2,
                                         tileCnt, false );

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::updateOAM( false );
    }

    void hideLocation( u8 p_remTime ) {
        if( !p_remTime ) {
            LOCATION_TIMER = 0;
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM ].isHidden     = true;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 1 ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 2 ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 3 ].isHidden = true;
        } else {
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM ].y -= 2;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 1 ].y -= 2;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 2 ].y -= 2;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 3 ].y -= 2;
        }
        IO::updateOAM( false );
    }

    void doPrintMessage( const char* p_message, style p_style, u16 p_item = 0,
                         const ITEM::itemData* p_data = 0, bool p_noDelay = false ) {
        IO::regularFont->setColor( 4, 3 );
        IO::regularFont->setColor( 5, 4 );
        u16 x = 12, y = 192 - 40, hg = 32;
        if( p_message ) {
            if( LOCATION_TIMER ) { hideLocation( ); }

            if( p_style == MSG_NORMAL || p_style == MSG_NOCLOSE || p_style == MSG_NORMAL_CONT ) {
                IO::loadSpriteB( "UI/mbox1", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 0, 192 - 46, 32, 64,
                                 false, false, false, OBJPRIORITY_0, false );

                for( u8 i = 0; i < 13; ++i ) {
                    IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 32 + 16 * i, 192 - 46,
                                     32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
                }
                IO::regularFont->setColor( 1, 1 );
                IO::regularFont->setColor( 2, 2 );
            } else if( p_style == MSG_INFO || p_style == MSG_INFO_NOCLOSE || p_style == MSG_ITEM
                       || p_style == MSG_INFO_CONT ) {
                IO::loadSpriteB( "UI/mbox2", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 2, 192 - 46, 32, 64,
                                 false, false, false, OBJPRIORITY_0, false );

                for( u8 i = 0; i < 13; ++i ) {
                    IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 30 + 16 * i, 192 - 46,
                                     32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
                }

                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 2, 2 );
                u16 lns = IO::regularFont->printBreakingStringC(
                    p_message, 0, 0, 192, true, IO::font::LEFT, 12, ' ', 0, false, -1 );
                if( p_style == MSG_ITEM ) {
                    x += 48;
                    if( lns == 1 ) {
                        y += 8;
                    } else {
                        y += 2;
                    }
                }
            } else if( p_style == MSG_MART_ITEM ) {
                p_noDelay = true;
                std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
                std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
                std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
                IO::loadSpriteB( "UI/mboxmart", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 0, 192 - 51, 32, 64,
                                 false, false, false, OBJPRIORITY_0, false );
                for( u8 i = 0; i < 13; ++i ) {
                    IO::loadSpriteB( SPR_MSGBOX_OAM + i + 1, SPR_MSGBOX_GFX, 32 + 16 * i, 192 - 51,
                                     32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
                }

                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 0, 2 );
                x = 54, y = 192 - 50, hg = 64;

                u16 lns = IO::regularFont->printBreakingStringC(
                    p_message, 0, 0, 192, true, IO::font::LEFT, 12, ' ', 0, false, -1 );
                if( lns == 3 ) { y = 192 - 44; }
                if( lns <= 2 ) { y = 192 - 38; }
            } else if( p_style == MSG_SIGN ) {
                // TODO: Load sign graphics

                std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
                std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
                std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
                IO::loadSpriteB( "UI/mbox2", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 2, 192 - 46, 32, 64,
                                 false, false, false, OBJPRIORITY_0, false );

                for( u8 i = 0; i < 13; ++i ) {
                    IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 30 + 16 * i, 192 - 46,
                                     32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
                }

                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 2, 2 );
                if( p_style == MSG_ITEM ) {
                    x += 48;
                    y += 8;
                }
            }
        }

        if( !p_message ) {
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
            std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
        } else {
            u16  cpos    = 0;
            u16  tileCnt = hg == 64 ? SPR_MSG_EXT_GFX : SPR_MSG_GFX;
            bool sp      = false;
            IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;
            std::memset( TMP_TEXT_BUF, 0, sizeof( TMP_TEXT_BUF ) );

            u8   tmppos         = 0;
            char shortbuf[ 20 ] = { 0 };
            while( p_message[ cpos ] ) {
                if( !p_noDelay ) {
                    // Check for special escaped characters ([escape sequence]), that need
                    // to be treated as a single character.
                    if( p_message[ cpos ] == '[' ) {
                        std::memset( shortbuf, 0, sizeof( shortbuf ) );
                        tmppos               = 0;
                        sp                   = true;
                        shortbuf[ tmppos++ ] = p_message[ cpos ];
                        ++cpos;
                        continue;
                    }
                    if( p_message[ cpos ] == ']' ) {
                        sp                   = false;
                        shortbuf[ tmppos++ ] = p_message[ cpos ];
                    } else if( !sp ) {
                        std::memset( shortbuf, 0, sizeof( shortbuf ) );
                        shortbuf[ tmppos = 0 ] = p_message[ cpos ];
                    }
                    if( sp ) {
                        shortbuf[ tmppos++ ] = p_message[ cpos ];
                        ++cpos;
                        continue;
                    }
                }

                tileCnt = hg == 64 ? SPR_MSG_EXT_GFX : SPR_MSG_GFX;
                u8 ln   = 1;
                if( p_noDelay ) {
                    ln = IO::regularFont->printStringBC( p_message, TEXT_PAL, TEXT_BUF,
                                                         256 - ( 64 * !!p_item ), IO::font::LEFT,
                                                         16 - ( 4 * !!p_item ), 64, hg );
                } else {
                    std::strncat( TMP_TEXT_BUF, shortbuf, 20 );
                    std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
                    ln = IO::regularFont->printStringBC( TMP_TEXT_BUF, TEXT_PAL, TEXT_BUF,
                                                         256 - ( 64 * !!p_item ), IO::font::LEFT,
                                                         16 - ( 4 * !!p_item ), 64, hg );
                }

                if( !p_noDelay ) {
                    if( ln == 1 || ( ln == 2 && p_message[ cpos ] == '\n' ) ) {
                        std::strncat( TEXT_CACHE_1, shortbuf, 20 );
                    } else if( ln == 2 || ( ln > 2 && p_message[ cpos ] == '\n' ) ) {
                        std::strncat( TEXT_CACHE_2, shortbuf, 20 );
                    } else {
                        std::strncpy( TEXT_CACHE_1, TEXT_CACHE_2, 256 );
                        std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
                        std::strncat( TEXT_CACHE_2, shortbuf, 20 );
                        std::memset( TMP_TEXT_BUF, 0, sizeof( TMP_TEXT_BUF ) );
                        std::strncat( TMP_TEXT_BUF, TEXT_CACHE_1, 256 );
                        std::strncat( TMP_TEXT_BUF, shortbuf, 20 );
                    }
                }

                tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM, tileCnt, x, y, 64, hg, TEXT_BUF,
                                           64 * hg / 2, false, false, false, OBJPRIORITY_0, false );
                tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM + 1, tileCnt, x + 64, y, 64, hg,
                                           TEXT_BUF + 64 * hg, 64 * hg / 2, false, false, false,
                                           OBJPRIORITY_0, false );
                tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM + 2, tileCnt, x + 128, y, 64, hg,
                                           TEXT_BUF + 2 * 64 * hg, 64 * hg / 2, false, false, false,
                                           OBJPRIORITY_0, false );
                if( !p_item ) {
                    tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM + 3, tileCnt, x + 64 + 128, y, 64,
                                               hg, TEXT_BUF + 3 * 64 * hg, 64 * hg / 2, false,
                                               false, false, OBJPRIORITY_0, false );
                } else if( !cpos ) {
                    if( !p_data || p_data->m_itemType != ITEM::ITEMTYPE_TM ) {
                        tileCnt = IO::loadItemIconB( p_item, 16, 192 - 40, SPR_MSGTEXT_OAM + 3,
                                                     tileCnt, false );
                    } else if( p_data && p_data->m_itemType == ITEM::ITEMTYPE_TM ) {
                        MOVE::moveData move = MOVE::getMoveData( p_data->m_param2 );
                        tileCnt
                            = IO::loadTMIconB( move.m_type, MOVE::isFieldMove( p_data->m_param2 ),
                                               16, 192 - 40, SPR_MSGTEXT_OAM + 3, tileCnt, false );
                    }
                }
                IO::updateOAM( false );
                for( u8 i = 0;
                     i < 80
                             / ( IO::TEXTSPEED
                                 + SAVE::SAV.getActiveFile( ).m_options.m_textSpeedModifier );
                     ++i ) {
                    swiWaitForVBlank( );
                }
                if( p_noDelay ) { break; }
                cpos++;
            }

            if( p_style == MSG_NORMAL || p_style == MSG_INFO || p_style == MSG_NORMAL_CONT
                || p_style == MSG_INFO_CONT || p_style == MSG_ITEM ) {
                // "Continue" char
                IO::regularFont->printCharB( 172, TEXT_PAL, CONT_BUF, 16, 0, 0 );
                tileCnt = IO::loadSpriteB( SPR_MSGCONT_OAM, SPR_MSGCONT_GFX, 254 - 12,
                                           192 - 40 + 24, 16, 16, CONT_BUF, 16 * 16 / 2, false,
                                           false, false, OBJPRIORITY_0, false );
            } else {
                IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;
            }
        }

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->setColor( IO::BLUE_IDX, 3 );
        IO::regularFont->setColor( IO::BLUE2_IDX, 4 );
        IO::updateOAM( false );
    }

    void animateMB( u8 p_frame ) {
        if( ( p_frame & 15 ) == 0 ) {
            SpriteEntry* oam                = IO::OamTop->oamBuffer;
            oam[ SPR_MSGCONT_OAM ].isHidden = !oam[ SPR_MSGCONT_OAM ].isHidden;
            IO::updateOAM( false );
        }
    }

    void waitForInteract( ) {
        scanKeys( );
        cooldown = COOLDOWN_COUNT;
        u8 frame = 0;
        loop( ) {
            animateMB( ++frame );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_A ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    animateMB( ++frame );
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                break;
            }
        }
    }

    void giveItemToPlayer( u16 p_itemId, u16 p_amount ) {
        auto data = ITEM::getItemData( p_itemId );
        SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::toBagType( data.m_itemType ), p_itemId,
                                                 p_amount );
        char buffer[ 100 ];
        auto iname = ITEM::getItemName( p_itemId );

        if( p_amount > 1 ) {
            snprintf( buffer, 99, GET_STRING( 563 ), p_amount, iname.c_str( ) );
        } else {
            snprintf( buffer, 99, GET_STRING( 564 ), iname.c_str( ) );
        }
        switch( data.m_itemType ) {
        case ITEM::ITEMTYPE_KEYITEM: SOUND::playSoundEffect( SFX_OBTAIN_KEY_ITEM ); break;
        case ITEM::ITEMTYPE_TM: SOUND::playSoundEffect( SFX_OBTAIN_TM ); break;
        default: SOUND::playSoundEffect( SFX_OBTAIN_ITEM ); break;
        }
        doPrintMessage( buffer, MSG_ITEM, p_itemId, &data );
        waitForInteract( );
        snprintf( buffer, 99, GET_STRING( 86 ), iname.c_str( ),
                  ITEM::getItemChar( data.m_itemType ),
                  GET_STRING( 11 + BAG::toBagType( data.m_itemType ) ) );
        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
        std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
        doPrintMessage( buffer, MSG_ITEM, p_itemId, &data );
        waitForInteract( );
        hideMessageBox( );
    }

    void printMessage( const char* p_message, style p_style ) {
        doPrintMessage( p_message, p_style );

        if( p_style == MSG_NORMAL_CONT || p_style == MSG_INFO_CONT ) {
            waitForInteract( );
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
            std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
            IO::updateOAM( false );
        }
        if( p_style == MSG_NORMAL || p_style == MSG_INFO ) {
            waitForInteract( );
            hideMessageBox( );
        }
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection ) {
        if( p_selection != 253 ) { doPrintMessage( p_message, p_style ); }

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> res
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; i++ ) {
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

        if( p_message || p_selection >= 253 ) {

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            if( p_selection != 253 ) {
                FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152,
                                     true );
            }
            for( u8 i = 0; i < 7; ++i ) { oam[ SPR_MENU_OAM_SUB( i ) ].isHidden = true; }

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

    std::vector<std::pair<IO::inputTarget, u8>>
    printChoiceMessage( const char* p_message, style p_style, const std::vector<u16>& p_choices,
                        u8 p_selection ) {
        doPrintMessage( p_message, p_style );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selection ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        if( p_message || p_selection >= 254 ) {

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152,
                                 true );
            for( u8 i = 0; i < 7; ++i ) { oam[ SPR_MENU_OAM_SUB( i ) ].isHidden = true; }

            for( u8 i = 0; i < p_choices.size( ); i++ ) {
                for( u8 j = 0; j < 8; j++ ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                }

                IO::regularFont->printString(
                    GET_STRING( p_choices[ i ] ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );

                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               i ) );
            }
        }

        IO::updateOAM( true );

        return res;
    }

    std::vector<std::pair<IO::inputTarget, menuOption>> getTouchPositions( bool p_bottom = true ) {
        auto         res = std::vector<std::pair<IO::inputTarget, menuOption>>( );
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 i = 0; i < 6; ++i ) {
            if( !oam[ SPR_MENU_OAM_SUB( i ) ].isHidden ) {
                res.push_back( std::pair( IO::inputTarget( oam[ SPR_MENU_OAM_SUB( i ) ].x,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].y,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].x + 27,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].y + 27 ),
                                          menuOption( i ) ) );
            }
        }

        return res;
    }

    void handleMenuSelection( menuOption p_selection, const char* p_path ) {
        switch( p_selection ) {
        case VIEW_PARTY: {
            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            u8 teamSize = 0;
            for( ; teamSize < 6; ++teamSize ) {
                if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ teamSize ].m_boxdata.m_speciesId ) {
                    break;
                }
            }
            STS::partyScreen sts
                = STS::partyScreen( SAVE::SAV.getActiveFile( ).m_pkmnTeam, teamSize );

            SOUND::dimVolume( );

            auto res = sts.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            IO::resetScale( true, false );
            bgUpdate( );

            ANIMATE_MAP = true;
            SOUND::restoreVolume( );

            init( );
            MAP::curMap->draw( );
            if( res.m_selectedMove ) {
                for( u8 j = 0; j < 2; ++j ) {
                    if( MOVE::possible( res.m_selectedMove, j ) ) {
                        MOVE::use( res.m_selectedMove, j );
                        break;
                    }
                }
            }
            return;
        }
        case VIEW_DEX: {
            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            DEX::dex dx = DEX::dex( );
            SOUND::dimVolume( );

            dx.run( );
            // DEX::dex::SHOW_CAUGHT, MAX_PKMN ).run( SAVE::SAV.getActiveFile( ).m_lstDex );

            IO::initVideoSub( );
            IO::resetScale( true, false );
            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            MAP::curMap->draw( );
            init( );
            return;
        }
        case VIEW_BAG: {
            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            BAG::bagViewer bv = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam );
            SOUND::dimVolume( );
            u16 res = bv.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );

            if( res ) { ITEM::use( res, _printMessage ); }
            return;
        }
        case VIEW_ID: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            SPX::runIDViewer( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );
            return;
        }
        case SAVE: {
            IO::yesNoBox yn;
            if( yn.getResult( GET_STRING( 92 ), MSG_INFO_NOCLOSE ) == IO::yesNoBox::YES ) {
                init( );
                ANIMATE_MAP = false;
                u16 lst     = -1;
                if( FS::writeSave( p_path, [ & ]( u16 p_perc, u16 p_total ) {
                        u16 stat = p_perc * 18 / p_total;
                        if( stat != lst ) {
                            lst = stat;
                            printMessage( 0, MSG_INFO_NOCLOSE );
                            char        buffer[ 100 ];
                            std::string buf2 = "";
                            for( u8 i = 0; i < stat; ++i ) {
                                buf2 += "\x03";
                                if( i % 3 == 2 ) { buf2 += " "; }
                            }
                            for( u8 i = stat; i < 18; ++i ) {
                                buf2 += "\x04";
                                if( i % 3 == 2 ) { buf2 += " "; }
                            }
                            snprintf( buffer, 99, GET_STRING( 93 ), buf2.c_str( ) );
                            doPrintMessage( buffer, MSG_INFO_NOCLOSE, 0, 0, true );
                        }
                    } ) ) {
                    printMessage( 0, MSG_INFO_NOCLOSE );
                    SOUND::playSoundEffect( SFX_SAVE );
                    printMessage( GET_STRING( 94 ), MSG_INFO );
                } else {
                    printMessage( 0, MSG_INFO_NOCLOSE );
                    printMessage( GET_STRING( 95 ), MSG_INFO );
                }
                ANIMATE_MAP = true;
            } else {
                init( );
            }

            return;
        }
        case SETTINGS: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            SAVE::runSettings( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );

            return;
        }
        default: return;
        }
    }

    std::vector<std::pair<IO::inputTarget, u8>> drawMenu( ) {
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152, true );

        oam[ SPR_X_OAM_SUB ].isHidden = false;
        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
            }

            if( !oam[ SPR_MENU_OAM_SUB( i ) ].isHidden ) {
                if( i != 3 ) {
                    IO::regularFont->printString(
                        GET_STRING( 413 + i ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48 + 13,
                        oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );
                } else {
                    IO::regularFont->printString( SAVE::SAV.getActiveFile( ).m_playername,
                                                  oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48 + 13,
                                                  oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true,
                                                  IO::font::CENTER );
                }

                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               i ) );
                oam[ SPR_MENU_OAM_SUB( i ) ].y = oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y;
                oam[ SPR_MENU_OAM_SUB( i ) ].x = oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x;
            } else {
                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               IO::choiceBox::DISABLED_CHOICE ) );
            }
        }
        oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;

        IO::updateOAM( true );

        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::EXIT_CHOICE ) );
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::BACK_CHOICE ) );

        return res;
    }

    void selectMenuItem( u8 p_selection ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selection ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        IO::updateOAM( true );
    }

    void focusMenu( const char* p_path ) {
        SOUND::playSoundEffect( SFX_MENU );

        IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        auto          res  = menu.getResult( [ & ]( u8 ) { return drawMenu( ); }, selectMenuItem );

        init( );
        if( res != IO::choiceBox::BACK_CHOICE ) {
            handleMenuSelection( NAV::menuOption( res ), p_path );
        }
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    drawItemChoice( const std::vector<std::pair<u16, u32>>& p_offeredItems,
                    const std::vector<std::string>&         p_itemNames,
                    const std::vector<ITEM::itemData>& p_data, u8 p_paymentMethod,
                    u8 p_firstItem ) {
        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        FADE_SUB_DARK( );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152, true );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        char buffer[ 100 ];
        if( p_paymentMethod < 3 ) {
            snprintf( buffer, 99, GET_STRING( 471 + p_paymentMethod ),
                      p_paymentMethod == 0
                          ? SAVE::SAV.getActiveFile( ).m_money
                          : ( p_paymentMethod == 1 ? SAVE::SAV.getActiveFile( ).m_battlePoints
                                                   : SAVE::SAV.getActiveFile( ).m_coins ) );
        } else if( p_paymentMethod == 3 ) {
            snprintf( buffer, 99, GET_STRING( 554 ), SAVE::SAV.getActiveFile( ).m_ashCount );
        }
        IO::regularFont->printStringC( buffer, 2, 2, true, IO::font::LEFT );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        oam[ SPR_X_OAM_SUB ].isHidden = false;

        // next page (TODO)
        // prev page (TODO)

        for( u8 i = 0; i < 6; ++i ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette  = SPR_BOX_PAL_SUB;
            }
            oam[ SPR_ITEM_OAM_SUB( i ) ].isHidden = true;
        }

        for( u8 i = 0; i < std::min( u32( 6 ), u32( p_offeredItems.size( ) - p_firstItem ) );
             i++ ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;

                if( i & 1 ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].x = 130 + 15 * j;
                } else {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].x = 6 + 15 * j;
                }
            }

            if( p_data[ p_firstItem + i ].m_itemType != ITEM::ITEMTYPE_TM ) {
                IO::loadItemIcon(
                    p_offeredItems[ p_firstItem + i ].first, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y, SPR_ITEM_OAM_SUB( i ),
                    SPR_ITEM_PAL_SUB( i ), oam[ SPR_ITEM_OAM_SUB( i ) ].gfxIndex, true );
            } else {
                MOVE::moveData move = MOVE::getMoveData( p_data[ p_firstItem + i ].m_param2 );
                IO::loadTMIcon(
                    move.m_type, MOVE::isFieldMove( p_data[ p_firstItem + i ].m_param2 ),
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                    SPR_ITEM_OAM_SUB( i ), SPR_ITEM_PAL_SUB( i ),
                    oam[ SPR_ITEM_OAM_SUB( i ) ].gfxIndex, true );
            }

            if( IO::regularFont->stringWidthC( p_itemNames[ p_firstItem + i ].c_str( ) ) <= 85 ) {
                IO::regularFont->printStringC( p_itemNames[ p_firstItem + i ].c_str( ),
                                               oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 70,
                                               oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true,
                                               IO::font::CENTER );
            } else {
                IO::regularFont->printStringC( p_itemNames[ p_firstItem + i ].c_str( ),
                                               oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 112,
                                               oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true,
                                               IO::font::RIGHT );
            }
            snprintf( buffer, 90, "$%lu", p_offeredItems[ p_firstItem + i ].second );
            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->printStringC( buffer, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 114,
                                           oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 16, true,
                                           IO::font::RIGHT );

            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 120,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                           i ) );
        }

        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::EXIT_CHOICE ) );
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::BACK_CHOICE ) );

        if( p_firstItem ) { // allow prev page
            res.push_back( std::pair(
                IO::inputTarget( 0, 0, 0 ) /* oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y -
                                 8, oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 )*/
                ,
                IO::choiceBox::PREV_PAGE_CHOICE ) );
        }

        if( size_t( p_firstItem + 6 ) < p_offeredItems.size( ) ) { // allow next page
            res.push_back( std::pair(
                IO::inputTarget( 0, 0, 0 ) /* oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y -
                                 8, oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 )*/
                ,
                IO::choiceBox::NEXT_PAGE_CHOICE ) );
        }

        IO::updateOAM( true );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        IO::fadeScreen( IO::fadeType::UNFADE, true, false );
        bgUpdate( );
        return res;
    }

    void selectItem( std::pair<u16, u32> p_item, const ITEM::itemData& p_itemData,
                     const std::string& p_descr, u8 p_selection ) {

        IO::printRectangle( 128, 0, 255, 40, true, 0 );

        char buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( 474 ),
                  SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( p_itemData.m_itemType ),
                                                          p_item.first ) );
        IO::regularFont->printStringC( buffer, 254, 2, true, IO::font::RIGHT );

        selectMenuItem( p_selection % 6 );
        doPrintMessage( p_descr.c_str( ), MSG_MART_ITEM, p_item.first, &p_itemData );
    }

    std::vector<std::pair<IO::inputTarget, s32>> drawCounter( s32 p_min, s32 p_max ) {
        std::vector<std::pair<IO::inputTarget, s32>> res
            = std::vector<std::pair<IO::inputTarget, s32>>( );

        SpriteEntry* oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < 6; ++i ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette  = SPR_BOX_PAL_SUB;
            }
            oam[ SPR_ITEM_OAM_SUB( i ) ].isHidden = true;
        }

        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       p_min - 1 ) );
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       0 ) );

        s32 mx = p_max, dd = 1;
        if( -p_min > p_max ) { mx = p_max; }

        u8 digs = 0;
        for( auto i = mx; i > 0; i /= 10, ++digs, dd *= 10 ) {}
        dd /= 10;

        // counter box
        for( u8 j = 0; j < 8; j++ ) {
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].isHidden = false;
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].x        = 78 + 12 * j;
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].y        = 88;
        }

        // confirm
        for( u8 j = 0; j < 8; j++ ) {
            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].isHidden = false;
            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].x        = 92 + 8 * j;
            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].y        = 140;
        }

        // up / down

        for( u8 i = 0; i < digs; ++i, dd /= 10 ) {
            oam[ SPR_ARROW_UP_OAM_SUB( i ) ].isHidden = false;
            oam[ SPR_ARROW_UP_OAM_SUB( i ) ].x        = 128 - ( digs * 10 ) + i * 20;
            oam[ SPR_ARROW_UP_OAM_SUB( i ) ].y        = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y - 4;

            res.push_back( std::pair( IO::inputTarget( oam[ SPR_ARROW_UP_OAM_SUB( i ) ].x,
                                                       oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y - 4,
                                                       oam[ SPR_ARROW_UP_OAM_SUB( i ) ].x + 16,
                                                       oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 8 ),
                                      dd ) );

            oam[ SPR_ARROW_DOWN_OAM_SUB( i ) ].isHidden = false;
            oam[ SPR_ARROW_DOWN_OAM_SUB( i ) ].x        = 128 - ( digs * 10 ) + i * 20;
            oam[ SPR_ARROW_DOWN_OAM_SUB( i ) ].y = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 20;

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_ARROW_DOWN_OAM_SUB( i ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 24,
                                            oam[ SPR_ARROW_DOWN_OAM_SUB( i ) ].x + 16,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 36 ),
                           -dd ) );
        }

        IO::regularFont->printString(
            GET_STRING( 323 ), oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 36,
            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 8, true, IO::font::CENTER );

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 64,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 32 ),
                                  p_min - 2 ) );

        IO::updateOAM( true );
        return res;
    }

    void updateCounterValue( s32 p_newValue, u8 p_selectedDigit, u8 p_numDigs ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        IO::printRectangle( oam[ SPR_ARROW_UP_OAM_SUB( 0 ) ].x,
                            oam[ SPR_ARROW_UP_OAM_SUB( 0 ) ].y + 12,
                            oam[ SPR_ARROW_UP_OAM_SUB( p_numDigs - 1 ) ].x + 16,
                            oam[ SPR_ARROW_UP_OAM_SUB( 0 ) ].y + 32, true, 0 );
        BG_PALETTE_SUB[ IO::BLUE_IDX ]  = IO::BLUE;
        BG_PALETTE_SUB[ IO::BLUE2_IDX ] = IO::BLUE2;
        for( u8 dg = p_numDigs; dg > 0; dg--, p_newValue /= 10 ) {
            if( dg - 1 == p_selectedDigit ) {
                IO::regularFont->setColor( 0, 0 );
                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( 0, 0 );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            }

            IO::regularFont->printString( std::to_string( p_newValue % 10 ).c_str( ),
                                          oam[ SPR_ARROW_UP_OAM_SUB( dg - 1 ) ].x + 7,
                                          oam[ SPR_ARROW_UP_OAM_SUB( dg - 1 ) ].y + 13, true,
                                          IO::font::CENTER );
        }
    }

    void hoverCounterButton( s32 p_min, s32 p_max, s32 p_button ) {
        (void) p_max;
        SpriteEntry* oam = IO::Oam->oamBuffer;

        if( p_button == p_min - 2 ) {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].palette = SPR_BOX_SEL_PAL_SUB;
            }
        } else {
            for( u8 j = 0; j < 8; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].palette = SPR_BOX_PAL_SUB;
            }
        }
        IO::updateOAM( true );
    }

    s32 getItemCount( std::pair<u16, u32> p_item, const ITEM::itemData& p_itemData,
                      const std::string& p_name, u8 p_paymentMethod ) {

        SpriteEntry* oam = IO::Oam->oamBuffer;
        IO::printRectangle( 0, 40, 255, 192, true, 0 );

        // Compute max amount of the selected item the player can buy

        s32 mx = 0;
        if( p_paymentMethod == 0 ) {
            mx = SAVE::SAV.getActiveFile( ).m_money / p_item.second;
        } else if( p_paymentMethod == 1 ) {
            mx = SAVE::SAV.getActiveFile( ).m_battlePoints / p_item.second;
        } else if( p_paymentMethod == 2 ) {
            mx = SAVE::SAV.getActiveFile( ).m_coins / p_item.second;
        } else if( p_paymentMethod == 3 ) {
            mx = SAVE::SAV.getActiveFile( ).m_ashCount / p_item.second;
        }

        mx = std::max( s32( 0 ),
                       (s32) std::min( mx, s32( 999
                                                - SAVE::SAV.getActiveFile( ).m_bag.count(
                                                    BAG::toBagType( p_itemData.m_itemType ),
                                                    p_item.first ) ) ) );

        if( p_paymentMethod == 3 ) { mx = 1; }

        char buffer[ 100 ];
        s32  res = 0;
        if( mx == 0 ) {
            IO::printRectangle( 0, 40, 255, 192, true, 0 );
            init( true );
            for( u8 i = 0; i < 128; ++i ) { oam[ i ].isHidden = true; }
            for( u8 i = 0; i < 10; ++i ) {
                oam[ SPR_MSGBOX_OAM_SUB + i ].isHidden = false;
                oam[ SPR_MSGBOX_OAM_SUB + i ].y        = 32;
            }

            IO::updateOAM( true );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            if( p_paymentMethod < 3 ) {
                IO::regularFont->printBreakingStringC( GET_STRING( 479 + p_paymentMethod ), 40, 38,
                                                       256 - 80, true );
            } else if( p_paymentMethod == 3 ) {
                IO::regularFont->printBreakingStringC( GET_STRING( 555 ), 40, 38, 256 - 80, true );
            }

            waitForInteract( );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            return res;
        } else if( mx > 0 ) {
            snprintf( buffer, 100, GET_STRING( 475 ), p_name.c_str( ) );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );

            for( u8 i = 0; i < 10; ++i ) { oam[ SPR_MSGBOX_OAM_SUB + i ].isHidden = false; }

            IO::regularFont->printBreakingStringC( buffer, 40, 38, 256 - 80, true );

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );

            IO::counter c   = IO::counter( 0, mx );
            u8          mdg = 0;
            for( auto tmp = mx; tmp > 0; tmp /= 10, ++mdg ) {}

            res = c.getResult(
                [ & ]( ) { return drawCounter( 0, mx ); },
                [ & ]( u32 p_newValue, u8 p_selDig ) {
                    updateCounterValue( p_newValue, p_selDig, mdg );
                },
                [ & ]( s32 p_hoveredButton ) { hoverCounterButton( 0, mx, p_hoveredButton ); }, 1 );
            if( !res || res == -1 ) { return res; }
        } else {
            res = 1;
        }

        // make the player confirm the choice
        IO::printRectangle( 0, 40, 255, 192, true, 0 );
        init( true );
        for( u8 i = 0; i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < 10; ++i ) {
            oam[ SPR_MSGBOX_OAM_SUB + i ].isHidden = false;
            oam[ SPR_MSGBOX_OAM_SUB + i ].y        = 16;
        }

        u32 cost = res * p_item.second;

        if( p_paymentMethod < 3 ) {
            snprintf( buffer, 99, GET_STRING( 476 + p_paymentMethod ), p_name.c_str( ), res, cost );
        } else if( p_paymentMethod == 3 ) {
            snprintf( buffer, 99, GET_STRING( 556 ), p_name.c_str( ), cost );
        }
        IO::yesNoBox yn;
        auto         conf = yn.getResult(
            [ & ]( ) {
                auto tmpres = printYNMessage( 0, MSG_NORMAL, 253 );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->printBreakingStringC( buffer, 40, 22, 256 - 80, true );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                return tmpres;
            },
            [ & ]( IO::yesNoBox::selection p_selection ) {
                printYNMessage( 0, MSG_NORMAL, p_selection == IO::yesNoBox::NO );
            } );
        if( conf == IO::yesNoBox::NO ) { return 0; }
        SOUND::playSoundEffect( SFX_BUY_SUCCESSFUL );
        return res;
    }

    void buyItem( const std::vector<std::pair<u16, u32>>& p_offeredItems, u8 p_paymentMethod ) {
        std::vector<std::string>    names = std::vector<std::string>( );
        std::vector<std::string>    descr = std::vector<std::string>( );
        std::vector<ITEM::itemData> data  = std::vector<ITEM::itemData>( );

        for( auto i : p_offeredItems ) {
            data.push_back( ITEM::getItemData( i.first ) );
            auto nm = ITEM::getItemName( i.first );
            if( data.back( ).m_itemType == ITEM::ITEMTYPE_TM ) {
                nm += ": " + MOVE::getMoveName( data.back( ).m_param2 );
                descr.push_back( MOVE::getMoveDescr( data.back( ).m_param2 ) );
            } else {
                descr.push_back( ITEM::getItemDescr( i.first ) );
            }
            names.push_back( nm );
        }

        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        hideMessageBox( );

        u8 curPg = 0, oldsel = 0;
        loop( ) {
            // Make player select an item
            auto curItm = cb.getResult(
                [ & ]( u8 p_page ) {
                    curPg = p_page;
                    return drawItemChoice( p_offeredItems, names, data, p_paymentMethod,
                                           6 * p_page );
                },
                [ & ]( u8 p_selection ) {
                    selectItem( p_offeredItems[ 6 * curPg + p_selection ],
                                data[ 6 * curPg + p_selection ], descr[ 6 * curPg + p_selection ],
                                6 * curPg + p_selection );
                },
                oldsel, IO::choiceBox::DEFAULT_TICK, curPg );

            oldsel = curItm;

            if( curItm == IO::choiceBox::BACK_CHOICE || curItm == IO::choiceBox::EXIT_CHOICE ) {
                dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
                init( true );
                hideMessageBox( );
                break;
            }

            curItm += 6 * curPg;

            // Make player choose how many of the chosen item they want to buy

            s32 cnt = getItemCount( p_offeredItems[ curItm ], data[ curItm ], names[ curItm ],
                                    p_paymentMethod );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            init( true );

            if( cnt == -1 ) {
                hideMessageBox( );
                break;
            }

            if( cnt > 0 ) {
                if( p_paymentMethod == 0 ) {
                    SAVE::SAV.getActiveFile( ).m_money -= p_offeredItems[ curItm ].second * cnt;
                } else if( p_paymentMethod == 1 ) {
                    SAVE::SAV.getActiveFile( ).m_battlePoints
                        -= p_offeredItems[ curItm ].second * cnt;
                } else if( p_paymentMethod == 2 ) {
                    SAVE::SAV.getActiveFile( ).m_coins -= p_offeredItems[ curItm ].second * cnt;
                } else if( p_paymentMethod == 3 ) {
                    SAVE::SAV.getActiveFile( ).m_ashCount -= p_offeredItems[ curItm ].second * cnt;
                }
                SAVE::SAV.getActiveFile( ).m_bag.insert(
                    BAG::toBagType( data[ curItm ].m_itemType ), p_offeredItems[ curItm ].first,
                    cnt );
            }
        }
    }

    void handleInput( const char* p_path ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        if( pressed & KEY_Y ) {
            // registered item
            IO::waitForKeysUp( KEY_Y );
            if( SAVE::SAV.getActiveFile( ).m_registeredItem ) {
                if( ITEM::isUsable( SAVE::SAV.getActiveFile( ).m_registeredItem ) ) {
                    ITEM::use( SAVE::SAV.getActiveFile( ).m_registeredItem, []( const char* ) {} );
                    //  updateItems( );
                } else {
                    printMessage( GET_STRING( 58 ) );
                }
            } else {
                printMessage( GET_STRING( 98 ) );
            }
            return;
        }

        if( ( pressed & KEY_X ) || ( pressed & KEY_START ) ) {
            // Open menu
            focusMenu( p_path );
            return;
        }

        for( auto c : getTouchPositions( ) ) {
            if( c.first.inRange( touch ) ) {
                oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = false;
                oam[ SPR_MENU_SEL_OAM_SUB ].x = oam[ SPR_MENU_OAM_SUB( u8( c.second ) ) ].x - 2;
                oam[ SPR_MENU_SEL_OAM_SUB ].y = oam[ SPR_MENU_OAM_SUB( u8( c.second ) ) ].y - 2;

                IO::updateOAM( true );

                bool change = true;
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );

                    if( !c.first.inRange( touch ) ) {
                        change                               = 0;
                        oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;
                        IO::updateOAM( true );
                        break;
                    }
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                if( change ) {
                    handleMenuSelection( c.second, p_path );
                    oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;
                    IO::updateOAM( true );
                }
            }
        }

#ifdef DESQUID
        if( pressed & KEY_SELECT ) {
            std::vector<u16> choices
                = { DESQUID_STRING + 46, DESQUID_STRING + 47, DESQUID_STRING + 48,
                    DESQUID_STRING + 50, DESQUID_STRING + 51, DESQUID_STRING + 52 };

            IO::choiceBox test = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
            switch( test.getResult( GET_STRING( DESQUID_STRING + 49 ), MSG_NOCLOSE, choices ) ) {
            case 0: {
                memset( SAVE::SAV.getActiveFile( ).m_pkmnTeam, 0,
                        sizeof( SAVE::SAV.getActiveFile( ).m_pkmnTeam ) );
                std::vector<u16> tmp
                    = { 201, 493, PKMN_EEVEE, 649, u16( 1 + rand( ) % MAX_PKMN ), MAX_PKMN };
                for( int i = 0; i < 6; ++i ) {
                    pokemon& a = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];

                    a = pokemon( tmp[ i ], 50, !i ? ( rand( ) % 28 ) : 0, 0, i );

                    a.m_stats.m_curHP *= i / 5.0;
                    a.m_boxdata.m_experienceGained += 750;

                    // Hand out some ribbons
                    for( u8 j = 0; j < 4; ++j ) {
                        a.m_boxdata.m_ribbons0[ j ] = rand( ) % 255;
                        a.m_boxdata.m_ribbons1[ j ] = rand( ) % 255;
                        a.m_boxdata.m_ribbons2[ j ] = rand( ) % 255;
                    }
                    a.m_boxdata.m_ribbons1[ 2 ] = rand( ) % 63;
                    a.m_boxdata.m_ribbons1[ 3 ] = 0;
                    if( a.m_boxdata.m_speciesId == 493 ) {
                        u8 plate = rand( ) % 17;
                        if( plate < 16 )
                            a.giveItem( I_FLAME_PLATE + plate );
                        else
                            a.giveItem( I_PIXIE_PLATE );
                    } else {
                        a.m_boxdata.m_heldItem = 1 + rand( ) % 400;
                    }

                    for( u16 j = 1; j <= MAX_PKMN; ++j ) {
                        SAVE::SAV.getActiveFile( ).registerCaughtPkmn( j );
                    }
                }
                for( u16 j : { 493, 649, 648, 647, 487, 492, 641, 642, 646, 645, 643, 644 } ) {
                    auto a       = pokemon( j, 50, 0, 0, j ).m_boxdata;
                    a.m_gotPlace = j;
                    SAVE::SAV.getActiveFile( ).storePkmn( a );
                }

                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 0 ] = M_SURF;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 1 ] = M_WATERFALL;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 0 ] = M_ROCK_CLIMB;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 3 ].m_boxdata.m_moves[ 0 ] = M_SWEET_SCENT;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 3 ].m_boxdata.m_moves[ 1 ] = M_DIG;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 3 ].m_boxdata.m_moves[ 2 ] = M_TELEPORT;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 5 ].m_boxdata.m_moves[ 0 ] = M_ROCK_SMASH;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 5 ].m_boxdata.m_moves[ 1 ] = M_CUT;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 5 ].m_boxdata.m_moves[ 2 ] = M_STRENGTH;

                init( );
                swiWaitForVBlank( );
                break;
            }
            case 1:
                for( u16 j = 1; j < MAX_ITEMS; ++j ) {
                    auto c = ITEM::getItemData( j );
                    if( c.m_itemType )
                        SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::toBagType( c.m_itemType ), j,
                                                                 1 );
                }
                init( );
                break;
            case 2: {
                init( );
                SAVE::CURRENT_TIME.m_hours = ( SAVE::CURRENT_TIME.m_hours + 5 ) % 24;
                SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_NAT_DEX_OBTAINED, true );
                /*
                init( );
                MAP::curMap->faintPlayer( );
                BOX::boxViewer bxv;
                ANIMATE_MAP = false;
                videoSetMode( MODE_5_2D );
                bgUpdate( );
                SOUND::dimVolume( );

                bxv.run( );

                FADE_TOP_DARK( );
                FADE_SUB_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::initVideoSub( );
                IO::resetScale( true, false );
                ANIMATE_MAP = true;
                SOUND::restoreVolume( );
                init( );
                MAP::curMap->draw( );
                */
                break;
            }
            case 3: {
                init( );
                MAP::curMap->awardBadge( 0, 1 + rand( ) % 8 );
                break;
            }
            case 4: {
                // SPX::runInitialPkmnSelection( );
                SPX::runCatchingTutorial( );
                break;
            }
            case 5: {
                init( );
                std::vector<std::pair<u16, u32>> items
                    = { { I_POKE_BALL, 200 },        { I_MASTER_BALL, 10 },  { I_REVIVE, 1500 },
                        { I_FULL_RESTORE, 100 },     { I_EXP_CANDY_XL, 10 }, { I_TM00, 5000 },
                        { I_TOPO_BERRY, 100000000 }, { I_ORAN_BERRY, 20 } };
                MAP::curMap->runPokeMart( items );
                break;
            }
            }
        }
#endif
    }
} // namespace NAV

/*
                void drawMapMug( ) {
                    auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr(
IO::bg3sub ); char buffer[ 100 ]; snprintf( buffer, 99, "%03hu/%hu_%hhu", CURRENT_BANK /
FS::ITEMS_PER_DIR, CURRENT_BANK, getCurrentDaytime( ) % 4 ); FS::readPictureData( ptr,
"nitro:/PICS/MAP_MUG/", buffer, 512, 49152, !SCREENS_SWAPPED ); drawBorder( );
                }

                void draw( bool p_initMainSrites, u8 p_newIdx ) {
                    if( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx == p_newIdx )
                        return;
                    else if( p_newIdx == u8( 255 ) )
                        p_newIdx = SAVE::SAV.getActiveFile( ).m_options.m_bgIdx;

                    auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr(
IO::bg3sub ); auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

                    if( STATE != MAP_MUG ) {
                        if( !BGs[ p_newIdx ].m_loadFromRom ) {
                            dmaCopy( BGs[ p_newIdx ].m_mainMenu, ptr, 256 * 192 );
                            dmaCopy( BGs[ p_newIdx ].m_mainMenuPal, pal, 192 * 2 );
                            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = p_newIdx;
                        } else if( !FS::readNavScreenData( ptr, BGs[ p_newIdx ].m_name.c_str( ),
                                                           p_newIdx ) ) {
                            dmaCopy( BGs[ 0 ].m_mainMenu, ptr, 256 * 192 );
                            dmaCopy( BGs[ 0 ].m_mainMenuPal, pal, 192 * 2 );
                            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = 0;
                        } else
                            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = p_newIdx;
                        drawBorder( );
                    } else if( STATE == MAP_MUG ) {
                        drawMapMug( );
                    }
                    if( p_initMainSrites && ALLOW_INIT ) initMainSprites( STATE != HOME );
                }

                void updateMap( u16 p_newMap ) {
                    if( p_newMap != CURRENT_MAP ) {
                        CURRENT_MAP = p_newMap;
                        draw( false );
                    }
                }

            } // namespace NAV
*/
