/*
Pokémon neo
------------------------------

file        : mysteryGift.cpp
author      : Philip Wellnitz
description : Logic for receiving (and sending) gifts.

Copyright (C) 2023 - 2023
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
#include <sys/select.h>

#include <nds.h>
#include <nds/system.h>

#include <dswifi9.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "defines.h"
#include "fs/fs.h"
#include "gen/movenames.h"
#include "gen/pokemonNames.h"
#include "io/choiceBox.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/mysteryGift.h"
#include "save/startScreen.h"
#include "sound/sound.h"

namespace SAVE {

#define SPR_SMALL_CHOICE_OAM_SUB       0
#define SPR_LARGE_CHOICE_OAM_SUB       1
#define SPR_ARROW_LEFT_OAM_SUB         5
#define SPR_ARROW_RIGHT_OAM_SUB        6
#define SPR_PAGE_LEFT_OAM_SUB          7
#define SPR_PAGE_RIGHT_OAM_SUB         8
#define SPR_CHOICE_OAM_SUB( p_choice ) ( 30 + 10 * ( p_choice ) )
#define SPR_PKMN_OAM_SUB               120

    constexpr u8 SPR_MBOX_PAL_TOP        = 3;
    constexpr u8 SPR_MBOX_OAM_START_TOP  = 8;
    constexpr u8 SPR_MBOX_PAL_TOP2       = 4;
    constexpr u8 SPR_MBOX_OAM_START_TOP2 = 30;
    constexpr u8 SPR_MBOX_OAM_LENGTH     = 13;
    constexpr u8 SPR_CARD_ICON_OAM       = 0;
    constexpr u8 SPR_CARD_ICON_PAL       = 0;

#define SPR_BOX_PAL_SUB      0
#define SPR_SELECTED_PAL_SUB 1
#define SPR_ARROW_X_PAL_SUB  2
#define SPR_PKMN_PAL_SUB     15

    wonderCard TMP_WC;

#ifdef DESQUID
    const u8         RIBBON[ 12 ]  = { 0, 0, 0, 0, 0, 0, 0, 0b10 };  // wishing ribbon
    const u8         RIBBON2[ 12 ] = { 0, 0, 0, 0, 0, 0, 0, 0b100 }; // classic ribbon
    const u16        MOVES[ 4 ]    = { M_TAKE_DOWN };
    const u16        MOVES2[ 4 ]   = { M_ENERGY_BALL, M_ICE_BEAM, M_FLAMETHROWER, M_THUNDERBOLT };
    const u16        ITEMS[ 4 ]    = { I_METAGROSSITE };
    const u16        ITEMS2[ 4 ]   = { I_TOPO_BERRY, I_NION_BERRY };
    const wonderCard TEST_WC1{ 0,
                               0,
                               "A test gift Pok\xe9mon.",
                               MOVES,
                               PKMN_BELDUM,
                               0,
                               5,
                               1002,
                               23,
                               "TEST",
                               2,
                               true,
                               true,
                               false,
                               L_LOVELY_PLACE,
                               0,
                               BAG::itemToBall( I_INFINITY_BALL ),
                               0,
                               0,
                               RIBBON,
                               ITEMS },
        TEST_WC2{
            0, 1, "A set of test gift items.", I_LANSAT_BERRY, 5, I_STARF_BERRY, 5, I_APICOT_BERRY,
            5 },
        TEST_WC3{ 0,
                  2,
                  "A nostalgic Pok\xe9mon.",
                  MOVES2,
                  PKMN_JUMPLUFF,
                  0,
                  50,
                  10905,
                  23,
                  0,
                  1,
                  true,
                  true,
                  false,
                  L_FARAWAY_PLACE,
                  0,
                  BAG::itemToBall( I_INFINITY_BALL ),
                  0,
                  0,
                  RIBBON2,
                  ITEMS2 };
#endif
    void movePkmn( s16 p_dx, s16 p_dy ) {
        for( u8 i = 0; i < 4; ++i ) {
            IO::Oam->oamBuffer[ SPR_PKMN_OAM_SUB + i ].x += p_dx;
            IO::Oam->oamBuffer[ SPR_PKMN_OAM_SUB + i ].y += p_dy;
        }
    }

    void clearText( ) {
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
    }

    void message( const char* p_message, bool p_init = true ) {
        if( p_init ) { clearText( ); }
        IO::regularFont->printStringC( p_message, 12, 192 - 40, false );
    }

    void hideSpritesSub( ) {
        SpriteEntry* oam                        = IO::Oam->oamBuffer;
        oam[ SPR_ARROW_LEFT_OAM_SUB ].isHidden  = true;
        oam[ SPR_ARROW_RIGHT_OAM_SUB ].isHidden = true;
        oam[ SPR_PAGE_LEFT_OAM_SUB ].isHidden   = true;
        oam[ SPR_PAGE_RIGHT_OAM_SUB ].isHidden  = true;
        for( u8 j = 0; j < 6; ++j ) {
            for( u8 i = 0; i < 10; ++i ) { oam[ SPR_CHOICE_OAM_SUB( j ) + i ].isHidden = true; }
        }
        for( u8 i = 0; i < 4; ++i ) { IO::Oam->oamBuffer[ SPR_PKMN_OAM_SUB + i ].isHidden = true; }
        IO::updateOAM( true );
    }

    void initTopSprites( bool p_showMbox = true ) {
        // Sprites
        // top screen

        u16 START = 128;
        u16 tcTop = IO::loadSprite( "UI/mbox2", SPR_MBOX_OAM_START_TOP, SPR_MBOX_PAL_TOP, START, 2,
                                    192 - 46, 32, 64, false, false, !p_showMbox, OBJPRIORITY_3,
                                    false, OBJMODE_BLENDED );

        for( u8 i = 0; i < SPR_MBOX_OAM_LENGTH; ++i ) {
            IO::loadSprite( SPR_MBOX_OAM_START_TOP + SPR_MBOX_OAM_LENGTH - i, SPR_MBOX_PAL_TOP,
                            START, 30 + 16 * i, 192 - 46, 32, 64, 0, 0, 0, false, true, !p_showMbox,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );
        }

        IO::loadSprite( "UI/mbox1", SPR_MBOX_OAM_START_TOP2, SPR_MBOX_PAL_TOP2, tcTop, 2, 6, 32, 64,
                        false, false, true, OBJPRIORITY_3, false );

        for( u8 i = 0; i < SPR_MBOX_OAM_LENGTH; ++i ) {
            IO::loadSprite( SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i, SPR_MBOX_PAL_TOP2,
                            tcTop, 30 + 16 * i, 6, 32, 64, 0, 0, 0, false, true, true,
                            OBJPRIORITY_3, false );
        }

        IO::updateOAM( false );
    }

    void initBottomSprites( ) {
        SpriteEntry* oam     = IO::Oam->oamBuffer;
        u16          tileCnt = 0;
        tileCnt = IO::loadPKMNSprite( pkmnSpriteInfo{ }, 128 - 48, -30, SPR_PKMN_OAM_SUB,
                                      SPR_PKMN_PAL_SUB, tileCnt, true );
        for( u8 i = 0; i < 4; ++i ) { IO::Oam->oamBuffer[ SPR_PKMN_OAM_SUB + i ].isHidden = true; }
        IO::updateOAM( true );

        tileCnt = IO::loadSprite( "SEL/noselection_96_32_1", SPR_LARGE_CHOICE_OAM_SUB,
                                  SPR_BOX_PAL_SUB, tileCnt, 0, 0, 16, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "SEL/noselection_96_32_2", SPR_LARGE_CHOICE_OAM_SUB + 1,
                                  SPR_BOX_PAL_SUB, tileCnt, 0, 0, 16, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );
        tileCnt = IO::loadSprite( "SEL/noselection_64_20", SPR_SMALL_CHOICE_OAM_SUB,
                                  SPR_BOX_PAL_SUB, tileCnt, 0, 0, 32, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // Arrows
        tileCnt
            = IO::loadSprite( "UI/arrow", SPR_ARROW_LEFT_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt, 4,
                              76, 16, 16, false, false, true, OBJPRIORITY_1, true, OBJMODE_NORMAL );
        IO::loadSprite( SPR_ARROW_RIGHT_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                        oam[ SPR_ARROW_LEFT_OAM_SUB ].gfxIndex, 236, 76, 16, 16, 0, 0, 0, false,
                        true, true, OBJPRIORITY_1, true, OBJMODE_NORMAL );

        // page windows
        tileCnt = IO::loadSprite( "SEL/noselection_32_64", SPR_PAGE_LEFT_OAM_SUB, SPR_BOX_PAL_SUB,
                                  tileCnt, 0 - 8, 57 - 12, 32, 64, true, true, true, OBJPRIORITY_2,
                                  true, OBJMODE_BLENDED );
        IO::loadSprite( SPR_PAGE_RIGHT_OAM_SUB, SPR_BOX_PAL_SUB,
                        oam[ SPR_PAGE_LEFT_OAM_SUB ].gfxIndex, 256 - 24, 4 + 28 * 2, 32, 64, 0, 0,
                        0, false, false, true, OBJPRIORITY_2, true, OBJMODE_BLENDED );

        // Pals
        // IO::copySpritePal( arrowPal, SPR_ARROW_X_PAL_SUB, 0, 2 * 4, true );
        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_SELECTED_PAL_SUB, 0, 2 * 8, true );
    }

    std::vector<std::pair<IO::inputTarget, startScreen::choice>>
    drawChoice( u16 p_message, const std::vector<u16>& p_choices, bool p_init = true,
                bool p_left = false, bool p_right = false ) {
        if( p_init ) {
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x04 << 8 );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x04 << 8 );
            bgUpdate( );
            IO::initOAMTable( false );
            IO::initOAMTable( true );

            initTopSprites( );
        }
        if( p_message ) { message( GET_STRING( p_message ), p_init ); }

        // bottom screen
        SpriteEntry* oam = IO::Oam->oamBuffer;

        if( p_init ) { initBottomSprites( ); }

        std::vector<std::pair<IO::inputTarget, startScreen::choice>> res
            = std::vector<std::pair<IO::inputTarget, startScreen::choice>>( );

        hideSpritesSub( );

        if( p_left ) {
            // prev slot button
            res.push_back( std::pair( IO::inputTarget( 1, oam[ SPR_PAGE_LEFT_OAM_SUB ].y + 8, 24,
                                                       oam[ SPR_PAGE_LEFT_OAM_SUB ].y + 48 ),
                                      IO::choiceBox::PREV_PAGE_CHOICE ) );

            oam[ SPR_PAGE_LEFT_OAM_SUB ].isHidden  = false;
            oam[ SPR_ARROW_LEFT_OAM_SUB ].isHidden = false;
        }
        if( p_right ) {
            // next slot button
            res.push_back( std::pair( IO::inputTarget( oam[ SPR_PAGE_RIGHT_OAM_SUB ].x,
                                                       oam[ SPR_PAGE_RIGHT_OAM_SUB ].y + 8,
                                                       oam[ SPR_PAGE_RIGHT_OAM_SUB ].x + 23,
                                                       oam[ SPR_PAGE_RIGHT_OAM_SUB ].y + 48 ),
                                      IO::choiceBox::NEXT_PAGE_CHOICE ) );

            oam[ SPR_PAGE_RIGHT_OAM_SUB ].isHidden  = false;
            oam[ SPR_ARROW_RIGHT_OAM_SUB ].isHidden = false;
        }

        u16 cury = 32;
        u8  ln   = 0;
        for( u8 c = 0; c < p_choices.size( ); ++c ) {
            res.push_back( std::pair( IO::inputTarget( 28, cury, 236, cury + 20 ), c ) );
            IO::regularFont->printStringC( GET_STRING( p_choices[ c ] ), 128, cury + 2, true,
                                           IO::font::CENTER );

            for( u8 i = 0; i <= 8; ++i ) {
                IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + i, SPR_BOX_PAL_SUB,
                                oam[ SPR_SMALL_CHOICE_OAM_SUB ].gfxIndex, 48 + 14 * i, cury, 32, 32,
                                0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                                OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_OAM_SUB( ln ) + 9, SPR_BOX_PAL_SUB,
                            oam[ SPR_SMALL_CHOICE_OAM_SUB ].gfxIndex, 48 + 16 * 8, cury - 12, 32,
                            32, 0, 0, 0, true, true, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

            cury += 22;
            ++ln;
        }
        res.push_back( std::pair( IO::inputTarget( 0, 0, 0 ), IO::choiceBox::BACK_CHOICE ) );

        IO::updateOAM( true );

        return res;
    }

    void selectMainChoice( u8 p_selection ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;

        oam[ SPR_PAGE_LEFT_OAM_SUB ].palette  = SPR_BOX_PAL_SUB;
        oam[ SPR_PAGE_RIGHT_OAM_SUB ].palette = SPR_BOX_PAL_SUB;
        for( u8 j = 0; j < 6; ++j ) {
            for( u8 i = 0; i < 10; ++i ) {
                oam[ SPR_CHOICE_OAM_SUB( j ) + i ].palette = SPR_BOX_PAL_SUB;
            }
        }

        if( p_selection < 6 ) {
            for( u8 i = 0; i < 10; ++i ) {
                oam[ SPR_CHOICE_OAM_SUB( p_selection ) + i ].palette = SPR_SELECTED_PAL_SUB;
            }
        }
        if( p_selection == IO::choiceBox::NEXT_PAGE_CHOICE ) {
            oam[ SPR_PAGE_RIGHT_OAM_SUB ].palette = SPR_SELECTED_PAL_SUB;
        }
        if( p_selection == IO::choiceBox::PREV_PAGE_CHOICE ) {
            oam[ SPR_PAGE_LEFT_OAM_SUB ].palette = SPR_SELECTED_PAL_SUB;
        }
        IO::updateOAM( true );
    }

    bool checkAndDownloadWCLocal( ) {
        // search for wc, download into TMP_WC
        TMP_WC = wonderCard{ };
        message( GET_STRING( IO::STR_UI_SEARCHING_FOR_GIFT ) );
        for( u8 k = 0; k < 120; ++k ) { swiWaitForVBlank( ); }

#ifdef DESQUID
        u8 r = rand( ) % 3;
        if( r == 0 ) {
            memcpy( &TMP_WC, &TEST_WC1, sizeof( wonderCard ) );
        } else if( r == 1 ) {
            memcpy( &TMP_WC, &TEST_WC2, sizeof( wonderCard ) );
        } else if( r == 2 ) {
            memcpy( &TMP_WC, &TEST_WC3, sizeof( wonderCard ) );
        }
        return true;
#else
        return false;
#endif
    }

    bool checkAndDownloadWCFriend( ) {
        // search for wc, download into TMP_WC

#ifdef DESQUID
        // reset all WC data

        memset( SAVE::SAV.getActiveFile( ).m_collectedWonderCards, 0,
                sizeof( SAVE::SAV.getActiveFile( ).m_collectedWonderCards ) );
        memset( SAVE::SAV.getActiveFile( ).m_storedWonderCards, 0,
                sizeof( SAVE::SAV.getActiveFile( ).m_storedWonderCards ) );

        message( "wonder card data deleted" );
        for( u8 k = 0; k < 250; ++k ) { swiWaitForVBlank( ); }
#else
        TMP_WC = wonderCard{ };
        message( GET_STRING( IO::STR_UI_SEARCHING_FOR_GIFT ) );
        for( u8 k = 0; k < 250; ++k ) { swiWaitForVBlank( ); }
#endif
        return false;
    }

    bool checkAndDownloadWCInternet( ) {
#ifdef DESQUID
        static bool WIFI_INITIALIZED = false;

        // search for wc, download into TMP_WC
        TMP_WC = wonderCard{ };
        message( GET_STRING( IO::STR_UI_SEARCHING_FOR_GIFT ) );

        if( !WIFI_INITIALIZED && !Wifi_InitDefault( WFC_CONNECT ) ) { return false; }
        WIFI_INITIALIZED = true;

        const char*   url          = "localhost";
        const char*   request_text = "GET / HTTP/1.1\r\n"
                                     "Host: localhost\r\n"
                                     "User-Agent: Nintendo DS\r\n"
                                     "Accept: */*\r\n\r\n";
        constexpr u32 port         = 8000;

        // Find the IP address of the server, with gethostbyname
        struct hostent* myhost = gethostbyname( url );

        // Create a TCP socket
        int my_socket;
        my_socket = socket( AF_INET, SOCK_STREAM, 0 );

        // Tell the socket to connect to the IP address we found, on port 80 (HTTP)
        struct sockaddr_in sain;
        sain.sin_family      = AF_INET;
        sain.sin_port        = htons( port );
        sain.sin_addr.s_addr = *( (unsigned long*) ( myhost->h_addr_list[ 0 ] ) );
        connect( my_socket, (struct sockaddr*) &sain, sizeof( sain ) );

        // send our request
        send( my_socket, request_text, strlen( request_text ), 0 );

        int  recvd_len;
        char incoming_buffer[ 256 ];

        char result[ 250 ];
        int  respos = 0;

        const char endm[ 5 ] = "\r\n\r\n";
        int        endmpos   = 0;

        // skip header
        while( ( recvd_len = recv( my_socket, incoming_buffer, 1, 0 ) ) != 0 ) {
            if( recvd_len > 0 ) {
                if( incoming_buffer[ 0 ] == endm[ endmpos ] ) {
                    if( ++endmpos == 4 ) { break; }
                } else {
                    endmpos = 0;
                }
            }
        }
        while( ( recvd_len = recv( my_socket, incoming_buffer, 1, 0 ) ) != 0 && respos < 204 ) {
            if( recvd_len > 0 ) {
                for( int k = 0; k < recvd_len; ++k ) { result[ respos++ ] = incoming_buffer[ k ]; }
            }
        }

        shutdown( my_socket, 0 ); // good practice to shutdown the socket.
        closesocket( my_socket ); // remove the socket.

        memcpy( &TMP_WC, &result, sizeof( wonderCard ) );

        for( u8 k = 0; k < 250; ++k ) { swiWaitForVBlank( ); }
        return true;
#else
        TMP_WC = wonderCard{ };
        message( GET_STRING( IO::STR_UI_SEARCHING_FOR_GIFT ) );
        for( u8 k = 0; k < 250; ++k ) { swiWaitForVBlank( ); }
        return false;
#endif
    }

    void displayWonderCard( u8 p_cardIdx, bool p_reverse = false ) {
        bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
        bgSetScroll( IO::bg3, 0, 0 );
        bgUpdate( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "wcbg", 0, 256 * 256 / 2,
                             false );
        if( p_reverse ) {
            FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "wc2", 200, 3, 256 * 192,
                                 false );
        } else {
            FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "wc1", 200, 3, 256 * 192,
                                 false );
        }

        BG_PALETTE[ 1 ]     = 0xfbba;
        BG_PALETTE[ 2 ]     = 0xf775;
        BG_PALETTE_SUB[ 1 ] = 0xfbba;
        BG_PALETTE_SUB[ 2 ] = 0xf775;

        initTopSprites( false );

        const auto& wc = SAVE::SAV.getActiveFile( ).m_storedWonderCards[ p_cardIdx ];

        BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK2;
        BG_PALETTE[ IO::GRAY_IDX ]  = IO::STEEL_COLOR;
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        for( u8 i = 0; i < 3; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CARD_ICON_OAM + i ].isHidden = true;
        }
        if( !p_reverse ) {
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_WONDERCARD ), 16, 28, false );
            if( SAVE::SAV.getActiveFile( ).collectedWC( wc.m_id ) ) {
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_THANK_YOU_FOR_PLAYING ), 16,
                                               85, false );
            } else {
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_PLEASE_COLLECT_GIFT ), 16, 85,
                                               false );
            }
            IO::regularFont->printStringC(
                IO::formatDate( SAVE::date{ wc.m_year, wc.m_month, wc.m_day } ).c_str( ), 144, 150,
                false );

#ifdef DESQUID
            IO::regularFont->printStringC( std::to_string( wc.m_id ).c_str( ), 16, 149, false );
#endif

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            IO::regularFont->printStringC( wc.m_title, 16, 62, false );
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_DATE_RECEIVED ), 48, 149, false );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );

            // depending on wc type, load icon
            switch( wc.m_type ) {
            case SAVE::WCTYPE_ITEM: {
                u8  idx   = 0;
                u16 tccnt = 0;
                for( u8 i = 0; i < 3; ++i ) {
                    if( !wc.m_data.m_item.m_itemId[ 2 - i ] ) { continue; }
                    tccnt = IO::loadItemIcon(
                        wc.m_data.m_item.m_itemId[ 2 - i ], 256 - 24 * ( idx + 1 ) - 20, 18,
                        SPR_CARD_ICON_OAM + idx, SPR_CARD_ICON_PAL + idx, tccnt, false );
                    ++idx;
                }
                IO::updateOAM( false );
                break;
            }
            case SAVE::WCTYPE_PKMN: {
                if( wc.m_data.m_pkmn.m_isEgg ) {
                    IO::loadEggIcon( 256 - 32 - 24 - 14, 14, SPR_CARD_ICON_OAM, SPR_CARD_ICON_PAL,
                                     0, false, wc.m_data.m_pkmn.m_species == PKMN_MANAPHY );
                } else {
                    auto sInfo = pkmnSpriteInfo{ wc.m_data.m_pkmn.m_species,
                                                 wc.m_data.m_pkmn.m_forme,
                                                 wc.m_data.m_pkmn.m_female,
                                                 wc.m_data.m_pkmn.m_shiny == 2,
                                                 false,
                                                 DEFAULT_SPRITE_PID };
                    IO::loadPKMNIcon( sInfo, 256 - 32 - 24 - 14, 14, SPR_CARD_ICON_OAM,
                                      SPR_CARD_ICON_PAL, 0, false );
                }
                IO::updateOAM( false );
                break;
            }
            default: break;
            }
        } else {
            IO::updateOAM( false );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            IO::regularFont->printStringC(
                GET_WC_STRING( SAVE::SAV.getActiveFile( ).collectedWC( wc.m_id )
                               + 2 * wc.m_descriptionId ),
                16, 36, false );
        }
    }

    void wcAlbum( ) {
        u8   currentCard = 0;
        bool reverse     = false;

        loop( ) {
            // TODO: "Details" (flips card)
            // "Delete card" (deletes card)
            // "Back"
            // "left" / "right"

            IO::choiceBox    cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
            std::vector<u16> wcopts;
            auto             res = cb.getResult(
                [ & ]( u8 p_slot ) {
                    currentCard = p_slot;
                    auto& wc = SAVE::SAV.getActiveFile( ).m_storedWonderCards[ currentCard ];

                    dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
                    wcopts.clear( );
                    wcopts.push_back( IO::STR_UI_WC_FLIP );
                    if( SAVE::SAV.getActiveFile( ).collectedWC( wc.m_id ) ) {
                        wcopts.push_back( IO::STR_UI_WC_TOSS );
                    }
                    wcopts.push_back( IO::STR_UI_CANCEL );

                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                    auto rs = drawChoice( 0, wcopts, false, currentCard > 0,
                                                      currentCard + 1 < SAVE::MAX_STORED_WC
                                                          && SAVE::SAV.getActiveFile( )
                                                                     .m_storedWonderCards[ currentCard + 1 ]
                                                                     .m_type
                                                                 != SAVE::WCTYPE_NONE );
                    displayWonderCard( currentCard, reverse );
                    return rs;
                },
                [ & ]( u8 p_choice ) { selectMainChoice( p_choice ); }, 0,
                [ & ]( ) {
                    // ++frame;
                    // IO::animateBG( frame, IO::bg3 );
                    // IO::animateBG( frame, IO::bg3sub );
                    // bgUpdate( );
                },
                currentCard );

            if( res == wcopts.size( ) - 1 || res == IO::choiceBox::BACK_CHOICE ) {
                clearText( );
                hideSpritesSub( );
                return;
            } else if( res == 0 ) {
                reverse = !reverse;
            } else if( res == 1 && res == wcopts.size( ) - 2 ) {
                for( u8 i = currentCard; i + 1 < SAVE::MAX_STORED_WC; ++i ) {
                    memcpy( &SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i ],
                            &SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i + 1 ],
                            sizeof( wonderCard ) );
                }
                memset( &SAVE::SAV.getActiveFile( ).m_storedWonderCards[ SAVE::MAX_STORED_WC - 1 ],
                        0, sizeof( wonderCard ) );

                if( SAVE::SAV.getActiveFile( ).m_storedWonderCards[ 0 ].m_type
                    == SAVE::WCTYPE_NONE ) {
                    clearText( );
                    hideSpritesSub( );
                    return;
                }
            }
        }
    }

    bool acceptWC( ) {
        if( TMP_WC.m_type == SAVE::WCTYPE_NONE ) { return false; }

        // print information on the event, ask yn if the player wants to accept.
        clearText( );

        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
            oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = false;
        }
        IO::updateOAM( false );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->printStringC( TMP_WC.m_title, 12, 12, false );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        pkmnSpriteInfo sInfo{ };

        // depending on wc type, load icon
        switch( TMP_WC.m_type ) {
        case SAVE::WCTYPE_ITEM: {
            u8  idx   = 0;
            u16 tccnt = 0;
            for( u8 i = 0; i < 3; ++i ) {
                if( !TMP_WC.m_data.m_item.m_itemId[ 2 - i ] ) { continue; }
                tccnt = IO::loadItemIcon( TMP_WC.m_data.m_item.m_itemId[ 2 - i ],
                                          256 - 32 * ( idx + 1 ) - 6, 24, SPR_CARD_ICON_OAM + idx,
                                          SPR_CARD_ICON_PAL + idx, tccnt, false );
                ++idx;
            }
            IO::updateOAM( false );
            break;
        }
        case SAVE::WCTYPE_PKMN: {
            if( TMP_WC.m_data.m_pkmn.m_isEgg ) {
                IO::loadEggIcon( 256 - 32 - 6, 24, SPR_CARD_ICON_OAM, SPR_CARD_ICON_PAL, 0, false,
                                 TMP_WC.m_data.m_pkmn.m_species == PKMN_MANAPHY );
                sInfo = pkmnSpriteInfo{ 0,     1 + TMP_WC.m_data.m_pkmn.m_species == PKMN_MANAPHY,
                                        false, false,
                                        false, DEFAULT_SPRITE_PID };
            } else {
                sInfo = pkmnSpriteInfo{ TMP_WC.m_data.m_pkmn.m_species,
                                        TMP_WC.m_data.m_pkmn.m_forme,
                                        TMP_WC.m_data.m_pkmn.m_female,
                                        TMP_WC.m_data.m_pkmn.m_shiny == 2,
                                        false,
                                        DEFAULT_SPRITE_PID };
                IO::loadPKMNIcon( sInfo, 256 - 32 - 6, 24, SPR_CARD_ICON_OAM, SPR_CARD_ICON_PAL, 0,
                                  false );
            }
            IO::updateOAM( false );
            break;
        }
        default: break;
        }

        // Check if the player already has the event, if so, decline to download it again
        u8   freespace = MAX_STORED_WC;
        bool hasWC     = false;

        for( u8 i = 0; i < MAX_STORED_WC; ++i ) {
            if( SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i ].m_type == SAVE::WCTYPE_NONE ) {
                freespace = i;
                break;
            }
            if( SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i ].m_type != SAVE::WCTYPE_NONE
                && SAVE::SAV.getActiveFile( ).m_storedWonderCards[ i ].m_id == TMP_WC.m_id ) {
                hasWC = true;
                break;
            }
        }

        if( hasWC || SAVE::SAV.getActiveFile( ).collectedWC( TMP_WC.m_id ) ) {
            // player owns/owned the gift, decline
            message( GET_STRING( IO::STR_UI_GIFT_ALREADY_COLLECTED ), false );
            IO::waitForInteractS( );

            clearText( );
            hideSpritesSub( );
            for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
                oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
            }
            IO::updateOAM( false );
            return false;
        }

        if( freespace == SAVE::MAX_STORED_WC ) {
            // player has no space. abort
            message( GET_STRING( IO::STR_UI_NO_SPACE ), false );
            IO::waitForInteractS( );

            clearText( );
            hideSpritesSub( );
            for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
                oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
            }
            IO::updateOAM( false );
            return false;
        }

        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
        if( cb.getResult(
                [ & ]( u8 ) {
                    return drawChoice( IO::STR_UI_ACCEPT_GIFT,
                                       { IO::STR_UI_ACCEPT, IO::STR_UI_DECLINE }, false );
                },
                [ & ]( u8 p_choice ) { selectMainChoice( p_choice ); }, 0,
                [ & ]( ) {
                    // ++frame;
                    // IO::animateBG( frame, IO::bg3 );
                    // IO::animateBG( frame, IO::bg3sub );
                    // bgUpdate( );
                } ) ) {
            // player declined the gift
            clearText( );
            hideSpritesSub( );
            for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
                oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
            }
            IO::updateOAM( false );

            return false;
        }

        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        message( GET_STRING( IO::STR_UI_GIFT_DOWNLOADING ), false );
        hideSpritesSub( );
        // play obtain animation
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "wc3", 300, 256 * 192,
                             true );
        bgSetScale( IO::bg3sub, 1 << 8, 1 << 8 );
        bgUpdate( );
        memcpy( &SAVE::SAV.getActiveFile( ).m_storedWonderCards[ freespace ], &TMP_WC,
                sizeof( wonderCard ) );

        SAVE::SAV.getActiveFile( ).m_storedWonderCards[ freespace ].m_year
            = SAVE::CURRENT_DATE.m_year;
        SAVE::SAV.getActiveFile( ).m_storedWonderCards[ freespace ].m_month
            = SAVE::CURRENT_DATE.m_month;
        SAVE::SAV.getActiveFile( ).m_storedWonderCards[ freespace ].m_day
            = SAVE::CURRENT_DATE.m_day;
        SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_UNCOLLECTED_MYSTERY_EVENT, 1 );

        IO::loadPKMNSprite( sInfo, 128 - 48, -30, SPR_PKMN_OAM_SUB, SPR_PKMN_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_PKMN_OAM_SUB ].gfxIndex, true );

        for( u8 y = 0; y <= 110; ++y ) {
            movePkmn( 0, 1 );
            IO::updateOAM( true );
            swiWaitForVBlank( );
        }

        // save game
        FS::writeSave( ARGV[ 0 ], []( u16, u16 ) {} );
        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        message( GET_STRING( IO::STR_UI_GIFT_RECEIVED ), false );
        IO::waitForInteractS( );

        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        message( GET_STRING( IO::STR_UI_PLEASE_COLLECT_GIFT ), false );
        IO::waitForInteractS( );

        // display wonder card

        IO::clearScreen( true, true, true );

        bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
        bgSetScroll( IO::bg3sub, 0, 0 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "wcbg", 0, 256 * 256 / 2,
                             true );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        hideSpritesSub( );

        displayWonderCard( freespace );
        IO::waitForInteractS( );
        return true;
    }

    void runMysteryGift( ) {
        SOUND::playBGM( BGM_MYSTERY_GIFT );
        IO::clearScreen( true, true, true );

        bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
        bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
        bgSetScroll( IO::bg3sub, 0, 0 );
        bgSetScroll( IO::bg3, 0, 0 );
        bgUpdate( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "wcbg", 0, 256 * 256 / 2,
                             true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "wcbg", 0, 256 * 256 / 2,
                             false );

        BG_PALETTE[ 1 ]     = 0xfbba;
        BG_PALETTE[ 2 ]     = 0xf775;
        BG_PALETTE_SUB[ 1 ] = 0xfbba;
        BG_PALETTE_SUB[ 2 ] = 0xf775;

        std::vector<u16> recvChoices{ IO::STR_UI_OBTAIN_FRIEND, IO::STR_UI_OBTAIN_WIRELESS,
                                      IO::STR_UI_OBTAIN_INTERNET, IO::STR_UI_CANCEL };

        // u8 frame = 0;

        loop( ) {
            std::vector<u16> mainChoices{ IO::STR_UI_RECEIVE_GIFT };
            if( SAVE::SAV.getActiveFile( ).m_storedWonderCards[ 0 ].m_type != SAVE::WCTYPE_NONE ) {
                mainChoices.push_back( IO::STR_UI_CHECK_WC );
            }
            mainChoices.push_back( IO::STR_UI_CANCEL );

            IO::initOAMTable( true );
            IO::initOAMTable( false );
            // ++frame;
            // IO::animateBG( frame, IO::bg3 );
            // IO::animateBG( frame, IO::bg3sub );
            bgUpdate( );

            // general choice
            // - receive gift
            // - view wonder cards
            // - exit

            IO::choiceBox cb  = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
            auto          res = cb.getResult(
                [ & ]( u8 ) { return drawChoice( IO::STR_UI_WELCOME_TO_MG, mainChoices ); },
                [ & ]( u8 p_choice ) { selectMainChoice( p_choice ); }, 0,
                [ & ]( ) {
                    // ++frame;
                    // IO::animateBG( frame, IO::bg3 );
                    // IO::animateBG( frame, IO::bg3sub );
                    // bgUpdate( );
                } );
            hideSpritesSub( );

            if( res == IO::choiceBox::BACK_CHOICE || res == mainChoices.size( ) - 1 ) {
                break;
            } else if( res == 0 ) {
                // receive event

                // check if empty space is available
                if( SAVE::SAV.getActiveFile( ).m_storedWonderCards[ SAVE::MAX_STORED_WC - 1 ].m_type
                    != SAVE::WCTYPE_NONE ) {
                    message( GET_STRING( IO::STR_UI_NO_SPACE ) );
                    IO::waitForInteractS( );
                    continue;
                }

                // ask for method to use for finding events
                IO::choiceBox cb2  = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
                auto          res2 = cb2.getResult(
                    [ & ]( u8 ) {
                        clearText( );
                        return drawChoice( IO::STR_UI_OBTAIN_SEL_MSG, recvChoices, false );
                    },
                    [ & ]( u8 p_choice ) { selectMainChoice( p_choice ); }, 0,
                    [ & ]( ) {
                        // ++frame;
                        // IO::animateBG( frame, IO::bg3 );
                        // IO::animateBG( frame, IO::bg3sub );
                        // bgUpdate( );
                    } );
                hideSpritesSub( );

                if( res2 == IO::choiceBox::BACK_CHOICE || res2 == recvChoices.size( ) - 1 ) {
                    continue;
                } else if( res2 == 0 ) {
                    // receive from friend
                    if( checkAndDownloadWCFriend( ) && TMP_WC.m_type != SAVE::WCTYPE_NONE ) {
                        // event found, ask if player wants to accept
                        acceptWC( );
                    } else {
                        message( GET_STRING( IO::STR_UI_NO_GIFT_FOUND ) );
                        IO::waitForInteractS( );
                    }
                } else if( res2 == 1 ) {
                    // receive from wireless
                    if( checkAndDownloadWCLocal( ) && TMP_WC.m_type != SAVE::WCTYPE_NONE ) {
                        // event found, ask if player wants to accept
                        acceptWC( );
                    } else {
                        message( GET_STRING( IO::STR_UI_NO_GIFT_FOUND ) );
                        IO::waitForInteractS( );
                    }
                } else if( res2 == 2 ) {
                    // receive from internet
                    if( checkAndDownloadWCInternet( ) && TMP_WC.m_type != SAVE::WCTYPE_NONE ) {
                        // event found, ask if player wants to accept
                        acceptWC( );
                    } else {
                        message( GET_STRING( IO::STR_UI_NO_GIFT_FOUND ) );
                        IO::waitForInteractS( );
                    }
                }
            } else if( res == 1 && res == mainChoices.size( ) - 2 ) {
                // check wonder cards
                clearText( );
                hideSpritesSub( );
                initTopSprites( false );
                wcAlbum( );
            }
        }
        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( true, true, true );
        IO::resetScale( true, true );
        bgUpdate( );

        SOUND::stopBGM( );
    }
} // namespace SAVE
