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

#include <nds/system.h>

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

    wonderCard TMP_WC;

#ifdef DESQUID
    const u8         RIBBON[ 12 ] = { 0, 0, 0, 0, 0, 0, 1 };
    const u16        MOVES[ 4 ]   = { M_TAKE_DOWN };
    const u16        ITEMS[ 4 ]   = { I_METAGROSSITE };
    const wonderCard TEST_WC1{ 0,
                               0,
                               "A test gift Pokémon.",
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
                               BAG::itemToBall( I_CHERISH_BALL ),
                               0,
                               0,
                               RIBBON,
                               ITEMS },
        TEST_WC2{
            0, 1, "A set of test gift items.", I_LANSAT_BERRY, 5, I_STARF_BERRY, 5, I_APICOT_BERRY,
            5 };
#endif

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
        IO::updateOAM( true );
    }

    std::vector<std::pair<IO::inputTarget, startScreen::choice>>
    drawChoice( u16 p_message, const std::vector<u16>& p_choices, bool p_init = true ) {
        if( p_init ) {
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x04 << 8 );
            REG_BLDCNT       = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA     = 0xff | ( 0x04 << 8 );
            bgUpdate( );
            IO::initOAMTable( false );
            IO::initOAMTable( true );

            // Sprites
            // top screen

            u16 START = 128;
            u16 tcTop = IO::loadSprite( "UI/mbox2", SPR_MBOX_OAM_START_TOP, SPR_MBOX_PAL_TOP, START,
                                        2, 192 - 46, 32, 64, false, false, false, OBJPRIORITY_3,
                                        false, OBJMODE_BLENDED );

            for( u8 i = 0; i < SPR_MBOX_OAM_LENGTH; ++i ) {
                IO::loadSprite( SPR_MBOX_OAM_START_TOP + SPR_MBOX_OAM_LENGTH - i, SPR_MBOX_PAL_TOP,
                                START, 30 + 16 * i, 192 - 46, 32, 64, 0, 0, 0, false, true, false,
                                OBJPRIORITY_3, false, OBJMODE_BLENDED );
            }

            IO::loadSprite( "UI/mbox1", SPR_MBOX_OAM_START_TOP2, SPR_MBOX_PAL_TOP2, tcTop, 2, 6, 32,
                            64, false, false, true, OBJPRIORITY_3, false );

            for( u8 i = 0; i < SPR_MBOX_OAM_LENGTH; ++i ) {
                IO::loadSprite( SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i,
                                SPR_MBOX_PAL_TOP2, tcTop, 30 + 16 * i, 6, 32, 64, 0, 0, 0, false,
                                true, true, OBJPRIORITY_3, false );
            }

            IO::updateOAM( false );
        }
        message( GET_STRING( p_message ), p_init );

        // bottom screen
        SpriteEntry* oam = IO::Oam->oamBuffer;

        u16 tileCnt = 0;

        if( p_init ) {
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
            tileCnt = IO::loadSprite( "UI/arrow", SPR_ARROW_LEFT_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                                      tileCnt, 4, 76, 16, 16, false, false, true, OBJPRIORITY_1,
                                      true, OBJMODE_NORMAL );
            IO::loadSprite( SPR_ARROW_RIGHT_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                            oam[ SPR_ARROW_LEFT_OAM_SUB ].gfxIndex, 236, 76, 16, 16, 0, 0, 0, false,
                            true, true, OBJPRIORITY_1, true, OBJMODE_NORMAL );

            // page windows
            tileCnt = IO::loadSprite( "SEL/noselection_32_64", SPR_PAGE_LEFT_OAM_SUB,
                                      SPR_BOX_PAL_SUB, tileCnt, 0 - 8, 57 - 12, 32, 64, true, true,
                                      true, OBJPRIORITY_2, true, OBJMODE_BLENDED );
            IO::loadSprite( SPR_PAGE_RIGHT_OAM_SUB, SPR_BOX_PAL_SUB,
                            oam[ SPR_PAGE_LEFT_OAM_SUB ].gfxIndex, 256 - 24, 4 + 28 * 2, 32, 64, 0,
                            0, 0, false, false, true, OBJPRIORITY_2, true, OBJMODE_BLENDED );

            // Pals
            // IO::copySpritePal( arrowPal, SPR_ARROW_X_PAL_SUB, 0, 2 * 4, true );
            IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_SELECTED_PAL_SUB, 0, 2 * 8, true );
        }

        std::vector<std::pair<IO::inputTarget, startScreen::choice>> res
            = std::vector<std::pair<IO::inputTarget, startScreen::choice>>( );

        hideSpritesSub( );

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
        if( rand( ) & 1 ) {
            memcpy( &TMP_WC, &TEST_WC1, sizeof( wonderCard ) );
        } else {
            memcpy( &TMP_WC, &TEST_WC2, sizeof( wonderCard ) );
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
        // search for wc, download into TMP_WC
        TMP_WC = wonderCard{ };
        message( GET_STRING( IO::STR_UI_SEARCHING_FOR_GIFT ) );
        for( u8 k = 0; k < 250; ++k ) { swiWaitForVBlank( ); }
        return false;
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

            } else {
                pkmnSpriteInfo sInfo{ TMP_WC.m_data.m_pkmn.m_species,
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

        if( freespace == SAVE::MAX_STORED_WC ) {
            // player has no space. abort
            message( GET_STRING( IO::STR_UI_NO_SPACE ) );
            IO::waitForInteractS( );

            hideSpritesSub( );
            for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
                oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
            }
            IO::updateOAM( false );
            return false;
        }
        if( hasWC || SAVE::SAV.getActiveFile( ).collectedWC( TMP_WC.m_id ) ) {
            // player owns/owned the gift, decline
            message( GET_STRING( IO::STR_UI_GIFT_ALREADY_COLLECTED ), false );
            IO::waitForInteractS( );

            hideSpritesSub( );
            for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
                oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
            }
            IO::updateOAM( false );
            return false;
        }

        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
        if( !cb.getResult(
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
            hideSpritesSub( );
            for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
                oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
            }
            IO::updateOAM( false );

            return false;
        }

        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        message( GET_STRING( IO::STR_UI_GIFT_DOWNLOADING ), false );
        hideSpritesSub( );
        // play obtain animation
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "wc3", 300, 256 * 192 / 2,
                             true );

        memcpy( &SAVE::SAV.getActiveFile( ).m_storedWonderCards[ freespace ], &TMP_WC,
                sizeof( wonderCard ) );
        // save game
        FS::writeSave( ARGV[ 0 ], []( u16, u16 ) {} );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "wcbg", 0, 256 * 256 / 2,
                             true );
        for( u8 i = 0; i <= SPR_MBOX_OAM_LENGTH; ++i ) {
            oam[ SPR_MBOX_OAM_START_TOP2 + SPR_MBOX_OAM_LENGTH - i ].isHidden = true;
        }
        IO::updateOAM( false );
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
