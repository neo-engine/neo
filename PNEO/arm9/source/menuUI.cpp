/*
Pokémon neo
------------------------------

file        : menuUI.cpp
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

#include <map>
#include <vector>

#include "bag/bagViewer.h"
#include "bag/item.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "box/boxViewer.h"
#include "defines.h"
#include "dex/dex.h"
#include "fs/fs.h"
#include "gen/itemNames.h"
#include "gen/locationNames.h"
#include "gen/moveNames.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/keyboard.h"
#include "io/menu.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/navApp.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "map/mapDrawer.h"
#include "map/mapObject.h"
#include "map/mapSlice.h"
#include "pokemon.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"
#include "sts/statusScreen.h"

namespace IO {
    constexpr u8 SPRITES_PER_CB_WINDOW = 8;

    const u16 ARR_X_SPR_PAL[ 16 ] = {
        0x7FFF, 0x5A6E, 0x6F2D, 0x564A, // arrow_up
        0x001F, 0x0011, 0x18CE          // x_16_16
    };

    void init( bool p_noPic, bool p_bottom ) {
        recomputeNavApps( );
        NAV_NEEDS_REDRAW = false;

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
        auto& oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        auto ptr  = !p_bottom ? bgGetGfxPtr( IO::bg2 ) : bgGetGfxPtr( IO::bg2sub );
        auto ptr3 = !p_bottom ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        // auto pal  = !p_bottom ? BG_PALETTE : BG_PALETTE_SUB;

        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        bgUpdate( );

        u16 tileCnt = 32; // some space for nav apps

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
            for( u8 j = 2; j < SPRITES_PER_CB_WINDOW - 1; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + SPRITES_PER_CB_WINDOW - 1,
                            SPR_BOX_PAL_SUB, oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            29 + 5 * 16, 42 + i * 36, 16, 32, 0, 0, 0, true, true, true,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        }

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, 42 + i * 36, 16, 32,
                            0, 0, 0, false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            for( u8 j = 1; j < SPRITES_PER_CB_WINDOW - 1; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 11,
                                42 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + SPRITES_PER_CB_WINDOW - 1,
                            SPR_BOX_PAL_SUB, oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            131 + 5 * 16, 42 + i * 36, 16, 32, 0, 0, 0, true, true, true,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
        }

        oam[ SPR_PAGE_BG_OAM_SUB ] = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ];
        oam[ SPR_PAGE_BG_OAM_SUB + 1 ]
            = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + SPRITES_PER_CB_WINDOW - 1 ];
        oam[ SPR_PAGE_BG_OAM_SUB + 2 ] = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ];
        oam[ SPR_PAGE_BG_OAM_SUB + 3 ]
            = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + SPRITES_PER_CB_WINDOW - 1 ];

        oam[ SPR_PAGE_BG_OAM_SUB ].isHidden           = oam[ SPR_PAGE_BG_OAM_SUB + 1 ].isHidden
            = oam[ SPR_PAGE_BG_OAM_SUB + 2 ].isHidden = oam[ SPR_PAGE_BG_OAM_SUB + 3 ].isHidden
            = true;
        oam[ SPR_PAGE_BG_OAM_SUB ].y           = oam[ SPR_PAGE_BG_OAM_SUB + 1 ].y
            = oam[ SPR_PAGE_BG_OAM_SUB + 2 ].y = oam[ SPR_PAGE_BG_OAM_SUB + 3 ].y = 192 - 40;

        oam[ SPR_PAGE_BG_OAM_SUB ].x     = 64 - 16;
        oam[ SPR_PAGE_BG_OAM_SUB + 1 ].x = oam[ SPR_PAGE_BG_OAM_SUB ].x + 16;
        oam[ SPR_PAGE_BG_OAM_SUB + 2 ].x = 128 + 64 - 16;
        oam[ SPR_PAGE_BG_OAM_SUB + 3 ].x = oam[ SPR_PAGE_BG_OAM_SUB + 2 ].x + 16;

        tileCnt
            = IO::loadSprite( "UI/arrow", SPR_PAGE_BG_OAM_SUB + 4, SPR_X_PAL_SUB, tileCnt,
                              oam[ SPR_PAGE_BG_OAM_SUB ].x + 8, oam[ SPR_PAGE_BG_OAM_SUB ].y + 8,
                              16, 16, false, false, true, OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        oam[ SPR_PAGE_BG_OAM_SUB + 5 ]   = oam[ SPR_PAGE_BG_OAM_SUB + 4 ];
        oam[ SPR_PAGE_BG_OAM_SUB + 5 ].x = oam[ SPR_PAGE_BG_OAM_SUB + 2 ].x + 8;
        oam[ SPR_PAGE_BG_OAM_SUB + 5 ].y = oam[ SPR_PAGE_BG_OAM_SUB + 2 ].y + 8;

        oam[ SPR_PAGE_BG_OAM_SUB + 5 ].hFlip = true;

        // app icons
        for( u8 i = 0; i < MAX_NAV_APPS; ++i ) {
            tileCnt = IO::loadSprite( SPR_NAV_APP_ICON_SUB( i ), SPR_NAV_APP_ICON_PAL_SUB( i ),
                                      tileCnt, 10, 10 + 49 * i, 64, 64, 0, 0, 64 * 64 / 2, false,
                                      false, true, OBJPRIORITY_1, p_bottom );
            if( CUR_NAV_APP == nullptr && i < NAV_APPS.size( ) && NAV_APPS[ i ] != nullptr ) {
                NAV_APPS[ i ]->drawIcon( SPR_NAV_APP_ICON_SUB( i ), p_bottom );
                oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden = p_noPic;
            } else {
                oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden = true;
            }
        }

        IO::copySpritePal( ARR_X_SPR_PAL, SPR_X_PAL_SUB, 0, 2 * 7, p_bottom );
        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );
        IO::updateOAM( p_bottom );

        if( !p_noPic ) {
            FS::readPictureData( ptr, "nitro:/PICS/", "Border", 64, 192, 192 * 256, p_bottom );
            if( CUR_NAV_APP != nullptr ) {
                CUR_NAV_APP->load( p_bottom );
            } else {
                FS::readPictureData(
                    ptr3, "nitro:/PICS/NAV/",
                    std::to_string( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx ).c_str( ),
                    192 * 2, 192 * 256, p_bottom );
            }
            hideMessageBox( );
        }
    }

    void redraw( bool p_bottom ) {
        auto& oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        auto ptr  = !p_bottom ? bgGetGfxPtr( IO::bg2 ) : bgGetGfxPtr( IO::bg2sub );
        auto ptr3 = !p_bottom ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        // auto pal  = !p_bottom ? BG_PALETTE : BG_PALETTE_SUB;

        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        bgUpdate( );

        // app icons
        for( u8 i = 0; i < MAX_NAV_APPS; ++i ) {
            if( CUR_NAV_APP == nullptr && i < NAV_APPS.size( ) && NAV_APPS[ i ] != nullptr ) {
                NAV_APPS[ i ]->drawIcon( SPR_NAV_APP_ICON_SUB( i ), p_bottom );
                oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden = false;
            } else {
                oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden = true;
            }
        }

        for( u8 i = SPR_NAV_APP_RSV_SUB; i < 128; ++i ) { oam[ i ].isHidden = true; }

        FS::readPictureData( ptr, "nitro:/PICS/", "Border", 64, 192, 192 * 256, p_bottom );
        if( CUR_NAV_APP != nullptr ) {
            CUR_NAV_APP->load( p_bottom );
        } else {
            FS::readPictureData(
                ptr3, "nitro:/PICS/NAV/",
                std::to_string( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx ).c_str( ), 192 * 2,
                192 * 256, p_bottom );
        }

        IO::updateOAM( p_bottom );
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    printYNMessage( const char* p_message, style p_style, u8 p_selection, bool p_showMoney ) {
        if( p_selection != 253 ) { doPrintMessage( p_message, p_style ); }

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> res
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );

        auto& oam = IO::Oam->oamBuffer;

        for( u8 i = SPR_NAV_APP_ICON_SUB( 0 ); i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < NUM_CB_CHOICES; i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( ( i & 1 ) == ( p_selection & 1 ) ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }
        for( u8 i = 2; i < 4; i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
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
                GET_STRING( IO::STR_UI_YES ), oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 32 ),
                           IO::yesNoBox::YES ) );

            IO::regularFont->printString(
                GET_STRING( IO::STR_UI_NO ), oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 8, true, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 32 ),
                           IO::yesNoBox::NO ) );

            if( p_showMoney ) {
                char buffer[ 100 ];
                snprintf( buffer, 99, GET_STRING( IO::STR_UI_MONEY ),
                          SAVE::SAV.getActiveFile( ).m_money );
                IO::regularFont->printStringC( buffer, 2, 2, true, IO::font::LEFT );
            }
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

        auto& oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < NUM_CB_CHOICES; i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selection ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        if( p_message || p_selection >= 254 ) {

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152,
                                 true );
            for( u8 i = 0; i < 7; ++i ) { oam[ SPR_MENU_OAM_SUB( i ) ].isHidden = true; }
            for( u8 i = SPR_NAV_APP_ICON_SUB( 0 ); i < 128; ++i ) { oam[ i ].isHidden = true; }

            for( u8 i = 0; i < p_choices.size( ); i++ ) {
                for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
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

            if( p_selection == 1 ) {
                oam[ SPR_X_OAM_SUB ].isHidden = false;
                res.push_back( std::pair(
                    IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                     oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                    IO::choiceBox::EXIT_CHOICE ) );
                res.push_back( std::pair(
                    IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                     oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                    IO::choiceBox::BACK_CHOICE ) );
            }
        }

        IO::updateOAM( true );

        return res;
    }

    std::vector<std::pair<IO::inputTarget, menuOption>> getTouchPositions( bool p_bottom ) {
        auto  res = std::vector<std::pair<IO::inputTarget, menuOption>>( );
        auto& oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 i = 0; i < 6; ++i ) {
            if( !oam[ SPR_MENU_OAM_SUB( i ) ].isHidden ) {
                res.push_back( std::pair( IO::inputTarget( oam[ SPR_MENU_OAM_SUB( i ) ].x,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].y,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].x + 27,
                                                           oam[ SPR_MENU_OAM_SUB( i ) ].y + 27 ),
                                          menuOption( i ) ) );
            }
        }

        if( CUR_NAV_APP == nullptr ) {
            for( u8 i = 0; i < MAX_NAV_APPS; ++i ) {
                if( !oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden ) {
                    res.push_back(
                        std::pair( IO::inputTarget( oam[ SPR_NAV_APP_ICON_SUB( i ) ].x,
                                                    oam[ SPR_NAV_APP_ICON_SUB( i ) ].y,
                                                    oam[ SPR_NAV_APP_ICON_SUB( i ) ].x + 48,
                                                    oam[ SPR_NAV_APP_ICON_SUB( i ) ].y + 48 ),
                                   menuOption( NAV_APP_START + i ) ) );
                }
            }
        }

        return res;
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

        auto& oam = IO::Oam->oamBuffer;
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152, true );

        for( u8 i = SPR_NAV_APP_ICON_SUB( 0 ); i < 128; ++i ) { oam[ i ].isHidden = true; }
        oam[ SPR_X_OAM_SUB ].isHidden = false;
        for( u8 i = 0; i < NUM_CB_CHOICES; i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
            }

            if( !oam[ SPR_MENU_OAM_SUB( i ) ].isHidden ) {
                if( i == IO::STR_UI_MENU_ITEM_TRAINER_ID - IO::STR_UI_MENU_ITEM_NAME_START ) {
                    IO::regularFont->printString( SAVE::SAV.getActiveFile( ).m_playername,
                                                  oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48 + 13,
                                                  oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true,
                                                  IO::font::CENTER );
                } else {
                    IO::regularFont->printString( GET_STRING( IO::STR_UI_MENU_ITEM_NAME_START + i ),
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
        auto& oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < NUM_CB_CHOICES; i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( i == p_selection ) ? SPR_BOX_SEL_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        oam[ SPR_PAGE_BG_OAM_SUB ].palette = oam[ SPR_PAGE_BG_OAM_SUB + 1 ].palette
            = ( p_selection == IO::choiceBox::PREV_PAGE_CHOICE ) ? SPR_BOX_SEL_PAL_SUB
                                                                 : SPR_BOX_PAL_SUB;
        oam[ SPR_PAGE_BG_OAM_SUB + 2 ].palette = oam[ SPR_PAGE_BG_OAM_SUB + 3 ].palette
            = ( p_selection == IO::choiceBox::NEXT_PAGE_CHOICE ) ? SPR_BOX_SEL_PAL_SUB
                                                                 : SPR_BOX_PAL_SUB;

        IO::updateOAM( true );
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    drawItemChoice( const std::vector<std::pair<u16, u32>>& p_offeredItems,
                    const std::vector<std::string>&         p_itemNames,
                    const std::vector<BAG::itemData>& p_data, u8 p_paymentMethod, u8 p_firstItem ) {

        auto num_items
            = std::min( p_data.size( ), std::min( p_itemNames.size( ), p_offeredItems.size( ) ) );

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
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_MONEYTYPE_START + p_paymentMethod ),
                      p_paymentMethod == 0
                          ? SAVE::SAV.getActiveFile( ).m_money
                          : ( p_paymentMethod == 1 ? SAVE::SAV.getActiveFile( ).m_battlePoints
                                                   : SAVE::SAV.getActiveFile( ).m_coins ) );
        } else if( p_paymentMethod == 3 ) {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_MONEYTYPE_ASH ),
                      SAVE::SAV.getActiveFile( ).m_ashCount );
        }
        IO::regularFont->printStringC( buffer, 2, 2, true, IO::font::LEFT );

        auto& oam = IO::Oam->oamBuffer;

        oam[ SPR_X_OAM_SUB ].isHidden = false;

        for( u8 i = SPR_NAV_APP_ICON_SUB( 0 ); i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < NUM_CB_CHOICES; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette  = SPR_BOX_PAL_SUB;
            }
            oam[ SPR_ITEM_OAM_SUB( i ) ].isHidden = true;
        }

        for( u8 i = 0; i < std::min( u32( NUM_CB_CHOICES ), u32( num_items - p_firstItem ) );
             i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;

                if( i & 1 ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].x = 130 + 15 * j;
                } else {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].x = 6 + 15 * j;
                }
            }

            if( p_data[ p_firstItem + i ].m_itemType != BAG::ITEMTYPE_TM ) {
                IO::loadItemIcon(
                    p_offeredItems[ p_firstItem + i ].first, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y, SPR_ITEM_OAM_SUB( i ),
                    SPR_ITEM_PAL_SUB( i ), oam[ SPR_ITEM_OAM_SUB( i ) ].gfxIndex, true );
            } else {
                BATTLE::moveData move = FS::getMoveData( p_data[ p_firstItem + i ].m_param2 );

                u8 tmtype = p_data[ p_firstItem + i ].m_effect;
                if( tmtype == 1 && BATTLE::isFieldMove( p_data[ p_firstItem + i ].m_param2 ) ) {
                    tmtype = 0;
                }
                IO::loadTMIcon( move.m_type, tmtype, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y, SPR_ITEM_OAM_SUB( i ),
                                SPR_ITEM_PAL_SUB( i ), oam[ SPR_ITEM_OAM_SUB( i ) ].gfxIndex,
                                true );
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

        // prev page
        oam[ SPR_PAGE_BG_OAM_SUB ].isHidden = oam[ SPR_PAGE_BG_OAM_SUB + 1 ].isHidden = false;

        bool prevpg                             = !!p_firstItem;
        oam[ SPR_PAGE_BG_OAM_SUB + 4 ].isHidden = !prevpg;
        if( prevpg ) {
            res.push_back(
                { IO::inputTarget( oam[ SPR_PAGE_BG_OAM_SUB ].x, oam[ SPR_PAGE_BG_OAM_SUB ].y,
                                   oam[ SPR_PAGE_BG_OAM_SUB ].x + 32,
                                   oam[ SPR_PAGE_BG_OAM_SUB ].y + 32 ),
                  IO::choiceBox::PREV_PAGE_CHOICE } );
        }

        // next page
        oam[ SPR_PAGE_BG_OAM_SUB + 2 ].isHidden = oam[ SPR_PAGE_BG_OAM_SUB + 3 ].isHidden = false;
        bool nextpg = size_t( p_firstItem + NUM_CB_CHOICES ) < num_items;
        oam[ SPR_PAGE_BG_OAM_SUB + 5 ].isHidden = !nextpg;
        if( nextpg ) {
            res.push_back( { IO::inputTarget( oam[ SPR_PAGE_BG_OAM_SUB + 2 ].x,
                                              oam[ SPR_PAGE_BG_OAM_SUB + 2 ].y,
                                              oam[ SPR_PAGE_BG_OAM_SUB + 2 ].x + 32,
                                              oam[ SPR_PAGE_BG_OAM_SUB + 2 ].y + 32 ),
                             IO::choiceBox::NEXT_PAGE_CHOICE } );
        }

        // page no
        snprintf( buffer, 90, "%i / %i", p_firstItem / NUM_CB_CHOICES + 1,
                  ( num_items - 1 ) / NUM_CB_CHOICES + 1 );
        IO::regularFont->printStringC( buffer, 128, oam[ SPR_PAGE_BG_OAM_SUB ].y + 8, true,
                                       IO::font::CENTER );

        IO::updateOAM( true );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        IO::fadeScreen( IO::fadeType::UNFADE, true, false );
        bgUpdate( );
        return res;
    }

    void selectItem( std::pair<u16, u32> p_item, const BAG::itemData& p_itemData,
                     const std::string& p_descr, u8 p_selection ) {
        if( p_selection == IO::choiceBox::PREV_PAGE_CHOICE ) {
            selectMenuItem( p_selection );
        } else if( p_selection == IO::choiceBox::NEXT_PAGE_CHOICE ) {
            selectMenuItem( p_selection );
        } else if( p_selection == IO::choiceBox::EXIT_CHOICE
                   || p_selection == IO::choiceBox::BACK_CHOICE ) {
            // empty!
        } else {
            IO::printRectangle( 128, 0, 255, 40, true, 0 );

            char buffer[ 100 ];
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_ITEMCOUNT_IN_BAG ),
                      SAVE::SAV.getActiveFile( ).m_bag.count(
                          BAG::toBagType( p_itemData.m_itemType ), p_item.first ) );
            IO::regularFont->printStringC( buffer, 254, 2, true, IO::font::RIGHT );

            selectMenuItem( p_selection % NUM_CB_CHOICES );
            doPrintMessage( p_descr.c_str( ), MSG_MART_ITEM, p_item.first, &p_itemData );
        }
    }

    std::vector<std::pair<IO::inputTarget, s32>> drawCounter( s32 p_min, s32 p_max ) {
        std::vector<std::pair<IO::inputTarget, s32>> res
            = std::vector<std::pair<IO::inputTarget, s32>>( );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152, true );

        auto& oam = IO::Oam->oamBuffer;
        for( u8 i = SPR_NAV_APP_ICON_SUB( 0 ); i < 128; ++i ) { oam[ i ].isHidden = true; }

        for( u8 i = 0; i < NUM_CB_CHOICES; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette  = SPR_BOX_PAL_SUB;
            }
            oam[ SPR_ITEM_OAM_SUB( i ) ].isHidden = true;
        }

        // Exit
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       p_min - 3 ) );
        // cancel
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       0 ) );

        s32 mx = p_max, dd = 1;
        if( -p_min > p_max ) { mx = -p_min; }

        u8 digs = 0;
        for( auto i = mx; i > 0; i /= 10, ++digs, dd *= 10 ) {}
        dd /= 10;

        // counter box
        for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].isHidden = false;
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].x        = 78 + 12 * j;
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].y        = 88;
        }

        // confirm
        for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
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
            GET_STRING( IO::STR_UI_SELECT ), oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 36,
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
        auto& oam = IO::Oam->oamBuffer;

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
        auto& oam = IO::Oam->oamBuffer;

        if( p_button == p_min - 2 ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].palette = SPR_BOX_SEL_PAL_SUB;
            }
        } else {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) + j ].palette = SPR_BOX_PAL_SUB;
            }
        }
        IO::updateOAM( true );
    }

    std::vector<std::pair<IO::inputTarget, u8>> drawDaycareChoice( u8 p_daycare ) {
        std::vector<std::pair<IO::inputTarget, u8>> res
            = std::vector<std::pair<IO::inputTarget, u8>>( );

        boxPokemon* dcstart  = &SAVE::SAV.getActiveFile( ).m_dayCarePkmn[ p_daycare * 2 ];
        u8*         dclstart = &SAVE::SAV.getActiveFile( ).m_dayCareDepositLevel[ p_daycare * 2 ];

        FADE_SUB_DARK( );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "subbg", 12, 49152, true );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE_SUB[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
        BG_PALETTE_SUB[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
        BG_PALETTE_SUB[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
        BG_PALETTE_SUB[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        char buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( IO::STR_UI_MONEYTYPE_MONEY ),
                  SAVE::SAV.getActiveFile( ).m_money );
        IO::regularFont->printStringC( buffer, 2, 2, true, IO::font::LEFT );

        auto& oam = IO::Oam->oamBuffer;

        oam[ SPR_X_OAM_SUB ].isHidden = false;

        for( u8 i = SPR_NAV_APP_ICON_SUB( 0 ); i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < NUM_CB_CHOICES; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette  = SPR_BOX_PAL_SUB;
            }
            oam[ SPR_ITEM_OAM_SUB( i ) ].isHidden = true;
        }

        // compatibility hearts
        u8 comp = 0;
        if( dcstart[ 1 ].getSpecies( ) ) { comp = dcstart[ 0 ].getCompatibility( dcstart[ 1 ] ); }

        IO::regularFont->setColor( 0, 2 );
        for( u8 i = 0; i < 3; ++i ) {
            if( i < comp ) {
                IO::regularFont->setColor( IO::RED2_IDX, 1 );
            } else {
                IO::regularFont->setColor( IO::GRAY_IDX, 1 );
            }
            IO::regularFont->printStringC( "\x01", 127 - 16 + 16 * i, 16, true, IO::font::CENTER );
        }
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;

                if( i & 1 ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].x = 130 + 15 * j;
                } else {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].x = 6 + 15 * j;
                }
            }

            if( dcstart[ i ].getSpecies( ) ) {
                // load pkmn icon
                IO::loadPKMNIcon(
                    dcstart[ i ].getSpriteInfo( ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y, SPR_ITEM_OAM_SUB( i ),
                    SPR_ITEM_PAL_SUB( i ), oam[ SPR_ITEM_OAM_SUB( i ) ].gfxIndex, true );

                // pkmn nick name
                auto strwd = IO::regularFont->stringWidthC( dcstart[ i ].m_name );
                u16  posx  = 0;
                if( strwd <= 85 ) {
                    IO::regularFont->printStringC(
                        dcstart[ i ].m_name, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 70,
                        oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true, IO::font::CENTER );
                    posx = oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 70 + strwd / 2 + 2;
                } else {
                    IO::regularFont->printStringC(
                        dcstart[ i ].m_name, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 102,
                        oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true, IO::font::RIGHT );
                    posx = oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 104;
                }

                if( dcstart[ i ].getSpecies( ) != PKMN_NIDORAN_F
                    && dcstart[ i ].getSpecies( ) != PKMN_NIDORAN_M ) {
                    if( dcstart[ i ].isFemale( ) ) {
                        IO::regularFont->setColor( IO::RED_IDX, 1 );
                        IO::regularFont->setColor( IO::RED2_IDX, 2 );
                        IO::regularFont->printString(
                            "}", posx, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true );
                        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                    } else if( !dcstart[ i ].m_isGenderless ) {
                        IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                        IO::regularFont->setColor( IO::BLUE2_IDX, 2 );
                        IO::regularFont->printString(
                            "{", posx, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true );
                        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                    }
                }

                IO::regularFont->setColor( 0, 2 );

                // level
                pkmnData data
                    = FS::getPkmnData( dcstart[ i ].getSpecies( ), dcstart[ i ].getForme( ) );
                u8 lv = calcLevel( dcstart[ i ], &data ), oldlv = dclstart[ i ];

                if( lv - oldlv > 0 ) {
                    snprintf( buffer, 95, "Lv.%hhu (+%hu)", lv, lv - oldlv );
                } else {
                    snprintf( buffer, 95, "Lv.%hhu", lv );
                }
                IO::regularFont->printStringC( buffer, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 70,
                                               oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 16, true,
                                               IO::font::CENTER );

                IO::regularFont->setColor( IO::GRAY_IDX, 2 );

                // iv
                for( u8 j = 0; j < 6; ++j ) {
                    snprintf( buffer, 49, "%hu", dcstart[ i ].IVget( j ) );

                    if( j && NatMod[ u8( dcstart[ i ].getNature( ) ) ][ j - 1 ] == 9 ) {
                        IO::regularFont->setColor( IO::BLUE_IDX, 2 );
                    } else if( j && NatMod[ u8( dcstart[ i ].getNature( ) ) ][ j - 1 ] == 11 ) {
                        IO::regularFont->setColor( IO::RED_IDX, 2 );
                    } else {
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                    }
                    IO::regularFont->printStringC(
                        buffer, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 36 + 40 * ( j % 3 ),
                        142 + ( 14 * ( j / 3 ) ), true, IO::font::RIGHT );
                    IO::regularFont->printStringC( GET_STRING( IO::STR_UI_PKMN_STAT_START + j ),
                                                   oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 4
                                                       + 40 * ( j % 3 ),
                                                   142 + ( 14 * ( j / 3 ) ), true, IO::font::LEFT );
                }

                // moves

                for( u8 j = 0; j < 4; ++j ) {
                    if( dcstart[ i ].getMove( j ) ) {
                        auto mname = FS::getMoveName( dcstart[ i ].getMove( j ) );
                        if( mname.length( ) > 18 ) {
                            snprintf( buffer, 20, "%s.", mname.c_str( ) );
                        } else {
                            snprintf( buffer, 20, "%s", mname.c_str( ) );
                        }
                    } else {
                        snprintf( buffer, 20, GET_STRING( IO::STR_UI_NONE ) );
                    }
                    IO::regularFont->printStringC(
                        buffer, 64 + 128 * i, oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 36 + 14 * j,
                        true, IO::font::CENTER );
                }

                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 120,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               i ) );
            } else {
                res.push_back(
                    std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 120,
                                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                               IO::choiceBox::DISABLED_CHOICE ) );
            }
        }

        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::EXIT_CHOICE ) );
        res.push_back(
            std::pair( IO::inputTarget( oam[ SPR_X_OAM_SUB ].x - 8, oam[ SPR_X_OAM_SUB ].y - 8,
                                        oam[ SPR_X_OAM_SUB ].x + 32, oam[ SPR_X_OAM_SUB ].y + 32 ),
                       IO::choiceBox::BACK_CHOICE ) );

        IO::updateOAM( true );
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        IO::fadeScreen( IO::fadeType::UNFADE, true, false );
        bgUpdate( );
        return res;
    }
} // namespace IO
