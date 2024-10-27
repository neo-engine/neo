/*
Pokémon neo
------------------------------

file        : bagUI.cpp
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

#include <algorithm>
#include <cstdio>
#include <vector>

#include "bag/bagUI.h"
#include "bag/bagViewer.h"
#include "bag/item.h"
#include "defines.h"
#include "fs/data.h"
#include "io/choiceBox.h"
#include "io/screenFade.h"
#include "io/strings.h"
#include "io/yesNoBox.h"
#include "save/saveGame.h"

// Sprites
#include "NoItem.h"

namespace BAG {
    constexpr u8 MAX_PARTY_PKMN          = SAVE::NUM_PARTY_SLOTS;
    constexpr u8 SPRITES_PER_CB_WINDOW   = 6;
    constexpr u8 NUM_CB_CHOICES          = 6;
    constexpr u8 SPRITES_PER_ITEM_WINDOW = 5;

    constexpr u8 SPR_TRANSFER_OAM_SUB = 0;
    constexpr u8 SPR_MSG_BOX_OAM_SUB  = 1;
    constexpr u8 SPR_CHOICE_START_OAM_SUB( u8 p_pos ) {
        return 14 + SPRITES_PER_CB_WINDOW * p_pos;
    }
    constexpr u8 SPR_ARROW_DOWN_OAM_SUB = 52;
    constexpr u8 SPR_ARROW_BACK_OAM_SUB = 53;
    constexpr u8 SPR_ARROW_UP_OAM_SUB   = 54;
    constexpr u8 SPR_ITEM_WINDOW_OAM_SUB( u8 p_window ) {
        return 55 + SPRITES_PER_ITEM_WINDOW * p_window;
    }
    constexpr u8 SPR_PKMN_START_OAM_SUB    = 100;
    constexpr u8 SPR_MSG_PKMN_SEL_OAM_SUB  = 106;
    constexpr u8 SPR_MSG_BOX_SMALL_OAM_SUB = 115;
    constexpr u8 SPR_BAG_ICON_SEL_OAM_SUB  = 120;
    constexpr u8 SPR_BAG_ICON_OAM_SUB      = 121;

    constexpr u8 SPR_BOX_PAL_SUB          = 0;
    constexpr u8 SPR_SELECTED_PAL_SUB     = 1;
    constexpr u8 SPR_PKMN_PAL_SUB         = 2;
    constexpr u8 SPR_BAG_ICON_PAL_SUB     = 8;
    constexpr u8 SPR_BAG_ICON_SEL_PAL_SUB = 9;
    // constexpr u8 SPR_ITEM_ICON_PAL_SUB    = 10;
    constexpr u8 SPR_ARROW_X_PAL_SUB  = 11;
    constexpr u8 SPR_BACK_PAL_SUB     = 12;
    constexpr u8 SPR_DOWN_PAL_SUB     = 13;
    constexpr u8 SPR_TRANSFER_PAL_SUB = 15;
    // constexpr u8 SPR_TYPE_PAL_SUB( u8 p_type ) {
    //     return 10 + ( p_type )
    // }

    constexpr u32 BAG_ICON_START[]
        = { IO::ICON::BAGBALL1_START,    IO::ICON::BAGBALL2_START,  IO::ICON::BAGMEDIINE1_START,
            IO::ICON::BAGMEDIINE2_START, IO::ICON::BAGHM1_START,    IO::ICON::BAGHM2_START,
            IO::ICON::BAGBERRY1_START,   IO::ICON::BAGBERRY2_START, IO::ICON::BAGKEY1_START,
            IO::ICON::BAGKEY2_START };

    void showActiveBag( u8 p_bagNo ) {
        u16 tileIdx = IO::Oam->oamBuffer[ SPR_BAG_ICON_SEL_OAM_SUB ].gfxIndex;

        IO::loadUIIcon( BAG_ICON_START[ 2 * p_bagNo + 1 ], SPR_BAG_ICON_SEL_OAM_SUB,
                        SPR_BAG_ICON_SEL_PAL_SUB, tileIdx, 27 * p_bagNo, 0, 32, 32, false, false,
                        false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
    }

    void bagUI::init( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::initOAMTable( false );
        // Don't init anything else for the top screen here
        IO::initOAMTable( true );
        IO::vramSetup( );
        swiWaitForVBlank( );
        IO::clearScreen( true, true );
        IO::resetScale( true, true );

        for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
            if( !_playerTeam[ i ].getSpecies( ) ) { break; }

            _teamItemCache[ i ] = std::pair( _playerTeam[ i ].getItem( ),
                                             FS::getItemName( _playerTeam[ i ].getItem( ) ) );
        }

        _lastPkmnItemType = 255;

        SpriteEntry* oam = IO::Oam->oamBuffer;

        u16 tileCnt = drawPkmnIcons( );

        tileCnt = IO::loadUIIcon( IO::ICON::BACK_START, SPR_ARROW_BACK_OAM_SUB, SPR_BACK_PAL_SUB,
                                  tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 26, 32, 32, false,
                                  false, false, OBJPRIORITY_3, true );

        tileCnt = IO::loadUIIcon( IO::ICON::DOWN_START, SPR_ARROW_DOWN_OAM_SUB, SPR_DOWN_PAL_SUB,
                                  tileCnt, SCREEN_WIDTH - 60, SCREEN_HEIGHT - 26, 32, 32, false,
                                  false, false, OBJPRIORITY_3, true );
        tileCnt = IO::loadUIIcon( IO::ICON::UP_START, SPR_ARROW_UP_OAM_SUB, SPR_ARROW_X_PAL_SUB,
                                  tileCnt, SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26, 32, 32, false,
                                  false, false, OBJPRIORITY_3, true );

        for( u8 i = 0; i < SPRITES_PER_ITEM_WINDOW; ++i ) {
            tileCnt = IO::loadUIIcon( BAG_ICON_START[ 2 * i ], SPR_BAG_ICON_OAM_SUB + i,
                                      SPR_BAG_ICON_PAL_SUB, tileCnt, 27 * i, 0, 32, 32, false,
                                      false, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        tileCnt = IO::loadUIIcon( BAG_ICON_START[ 1 ], SPR_BAG_ICON_SEL_OAM_SUB,
                                  SPR_BAG_ICON_SEL_PAL_SUB, tileCnt, 27, 0, 32, 32, false, false,
                                  false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

        tileCnt = IO::loadSprite( SPR_TRANSFER_OAM_SUB, SPR_TRANSFER_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26, 32, 32, 0, 0,
                                  NoItemTilesLen, false, false, true, OBJPRIORITY_3, true );

        // item windows

        tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_64_20_START, SPR_ITEM_WINDOW_OAM_SUB( 0 ),
                                  SPR_BOX_PAL_SUB, tileCnt, 0, 0, 32, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            for( u8 j = 0; j < 4; ++j ) {
                IO::loadSprite( SPR_ITEM_WINDOW_OAM_SUB( i ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_ITEM_WINDOW_OAM_SUB( 0 ) ].gfxIndex, 136 + 24 * j,
                                6 + 20 * i, 32, 32, 0, 0, NoItemTilesLen, false, false, true,
                                OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_ITEM_WINDOW_OAM_SUB( i ) + 4, SPR_BOX_PAL_SUB,
                            oam[ SPR_ITEM_WINDOW_OAM_SUB( 0 ) ].gfxIndex, 254 - 32, 6 + 20 * i - 12,
                            32, 32, 0, 0, NoItemTilesLen, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
        }

        // choice windows

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i;

            if( !i ) {
                tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_1_START,
                                          SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB, tileCnt,
                                          29, 80 + i * 36, 16, 32, false, false, true,
                                          OBJPRIORITY_3, true, OBJMODE_NORMAL );
                tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_2_START,
                                          SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                          tileCnt, 29 + 16, 80 + i * 36, 16, 32, false, false, true,
                                          OBJPRIORITY_3, true, OBJMODE_NORMAL );
            } else {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29, 80 + i * 36, 16,
                                32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                                OBJMODE_NORMAL );
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + 16,
                                80 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_NORMAL );
            }
            for( u8 j = 2; j < SPRITES_PER_CB_WINDOW - 1; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 16,
                                80 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + SPRITES_PER_CB_WINDOW - 1,
                            SPR_BOX_PAL_SUB, oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            29 + ( SPRITES_PER_CB_WINDOW - 1 ) * 16, 80 + i * 36, 16, 32, 0, 0, 0,
                            true, true, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
        }

        for( u8 i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, 80 + i * 36, 16, 32,
                            0, 0, 0, false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
            for( u8 j = 1; j < SPRITES_PER_CB_WINDOW - 1; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 131 + j * 16,
                                80 + i * 36, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_NORMAL );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + SPRITES_PER_CB_WINDOW - 1,
                            SPR_BOX_PAL_SUB, oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            131 + ( SPRITES_PER_CB_WINDOW - 1 ) * 16, 80 + i * 36, 16, 32, 0, 0, 0,
                            true, true, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
        }

        // message box
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB, SPR_BOX_PAL_SUB,
                        oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 29, 80 - 36, 16, 32, 0, 0, 0,
                        false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
        for( u8 j = 1; j < 12; j++ ) {
            IO::loadSprite( SPR_MSG_BOX_OAM_SUB + j, SPR_BOX_PAL_SUB,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex, 29 + j * 16, 80 - 36,
                            16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                            OBJMODE_NORMAL );
        }
        IO::loadSprite( SPR_MSG_BOX_OAM_SUB + 12, SPR_BOX_PAL_SUB,
                        oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 20 + 12 * 16, 80 - 36, 16,
                        32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );

        // Pkmn selection

        for( u8 i = 1; i < 5; ++i ) {
            IO::loadSprite( SPR_MSG_PKMN_SEL_OAM_SUB + i, SPR_SELECTED_PAL_SUB, tileCnt,
                            4 + 24 * ( 5 - i ), 33, 32, 32, 0, 0, 0, false, false, true,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_BLANK_32_24_START, SPR_MSG_PKMN_SEL_OAM_SUB,
                                  SPR_SELECTED_PAL_SUB, tileCnt, 8, 33, 32, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        // Small message box
        for( u8 i = 1; i < 5; ++i ) {
            IO::loadSprite( SPR_MSG_BOX_SMALL_OAM_SUB + i, SPR_BOX_PAL_SUB, tileCnt,
                            3 + 24 * ( 5 - i ), -9, 32, 32, 0, 0, 0, true, true, true,
                            OBJPRIORITY_3, true, OBJMODE_NORMAL );
        }
        tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_64_20_START, SPR_MSG_BOX_SMALL_OAM_SUB,
                                  SPR_BOX_PAL_SUB, tileCnt, 3, 3, 32, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_NORMAL );

        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_SELECTED_PAL_SUB, 0, 2 * 8, true );
        IO::updateOAM( true );

        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BagUpper" );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "BagLower", 512, 49152,
                             true );

        IO::initColors( );
        bgUpdate( );
    }

    u16 bagUI::drawPkmnIcons( ) {
        u16 tileCnt = 0;
        for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
            auto acPkmn = _playerTeam[ i ];
            if( !acPkmn.getSpecies( ) ) break;
            if( acPkmn.isEgg( ) ) {
                tileCnt = IO::loadEggIcon( 10, 23 + i * 26, SPR_PKMN_START_OAM_SUB + i,
                                           SPR_PKMN_PAL_SUB + i, tileCnt );
            } else {
                tileCnt = IO::loadPKMNIcon( acPkmn.getSpriteInfo( ), 9, 25 + i * 26,
                                            SPR_PKMN_START_OAM_SUB + i, SPR_PKMN_PAL_SUB + i,
                                            tileCnt, true );
            }
            IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + i ].priority = OBJPRIORITY_3;
        }
        return tileCnt;
    }

    void drawItemTop( u16 p_itemId, const itemData* p_data, u16 p_count ) {
        IO::OamTop->oamBuffer[ 0 ].isHidden = true;
        IO::OamTop->oamBuffer[ 1 ].isHidden = true;
        IO::OamTop->oamBuffer[ 2 ].isHidden = true;
        IO::updateOAM( false );

        if( !p_itemId || p_data == nullptr ) { return; }

        std::string descr;
        char        buffer[ 100 ];

        if( p_data->m_itemType != ITEMTYPE_TM ) {
            IO::loadItemIcon( p_itemId, 112, 44, 0, 0, 0, false );

            if( p_data->m_itemType & ITEMTYPE_BERRY ) {
                snprintf( buffer, 90, "%s%hu: %s", GET_STRING( IO::STR_UI_BAG_NUMBER ),
                          itemToBerry( p_itemId ), FS::getItemName( p_itemId ).c_str( ) );
            } else {
                FS::getItemName( p_itemId, CURRENT_LANGUAGE, buffer );
            }

            IO::regularFont->printStringC( buffer, 128, 26, false, IO::font::CENTER );

            descr = FS::getItemDescr( p_itemId );
            if( p_data->m_itemType != ITEMTYPE_KEYITEM
                && p_data->m_itemType != ITEMTYPE_FORMECHANGE ) {
                snprintf( buffer, 99, "x %d", p_count );
                IO::regularFont->printStringC( buffer, 146, 52, false );
            }

            if( p_data->m_itemType & ITEMTYPE_BERRY ) {
                berry data = FS::getBerryData( itemToBerry( p_itemId ) );

                IO::regularFont->setColor( IO::RED_IDX, 1 );
                if( data.m_firmness ) {
                    std::string firmness = GET_STRING( 16 + data.m_firmness );
                    snprintf( buffer, 99, GET_STRING( 16 ), firmness.c_str( ) );
                    IO::regularFont->printStringC( buffer, 24, 145, false );
                }
                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                snprintf( buffer, 99, GET_STRING( 23 ), data.m_size / 10.0 );
                IO::regularFont->printStringC( buffer, 140, 145, false );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );

                u8 poses[ 2 ][ 5 ] = { { 18, 66, 104, 150, 194 }, { 18, 66, 124, 150, 194 } };
                u8 mx              = 0;
                for( u8 i = 0; i < 5; ++i ) mx = std::max( mx, data.m_flavor[ i ] );
                for( u8 i = 0; i < 5; ++i ) {
                    if( data.m_flavor[ i ] != mx ) {
                        IO::regularFont->setColor( IO::GRAY_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                    } else {
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                    }
                    IO::regularFont->printStringC( GET_STRING( 24 + i ),
                                                   poses[ CURRENT_LANGUAGE ][ i ], 160, false );
                }
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            }
        } else {
            if( p_data->m_effect == 2 ) { // TR
                snprintf( buffer, 99, "x %d", p_count );
                IO::regularFont->printStringC( buffer, 146, 52, false );
            }

            descr                 = FS::getMoveDescr( p_data->m_param2 );
            BATTLE::moveData move = FS::getMoveData( p_data->m_param2 );

            u8 tmtype = p_data->m_effect;
            if( tmtype == 1 && BATTLE::isFieldMove( p_data->m_param2 ) ) { tmtype = 0; }
            u16 tileCnt = IO::loadTMIcon( move.m_type, tmtype, 112, 44, 0, 0, 0, false );

            snprintf( buffer, 99, "%s: %s", FS::getItemName( p_itemId ).c_str( ),
                      FS::getMoveName( p_data->m_param2 ).c_str( ) );
            IO::regularFont->printStringC( buffer, 128, 26, false, IO::font::CENTER );

            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_TYPE ), 56, 147, false,
                                           IO::font::RIGHT );
            tileCnt
                = IO::loadTypeIcon( move.m_type, 62, 146, 1, 1, tileCnt, false, CURRENT_LANGUAGE );

            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_CATEGORY ), 146, 147, false,
                                           IO::font::RIGHT );
            IO::loadDamageCategoryIcon( move.m_category, 152, 146, 2, 2, tileCnt, false );

            snprintf( buffer, 99, "%s  %2d", GET_STRING( IO::STR_UI_BAG_PP ), move.m_pp );
            IO::regularFont->printStringC( buffer, 225, 147, false, IO::font::RIGHT );

            IO::regularFont->setColor( IO::RED_IDX, 1 );

            // power / acc
            if( move.m_basePower ) {
                snprintf( buffer, 24, GET_STRING( IO::STR_UI_BAG_POWER ), move.m_basePower );
                IO::regularFont->printStringC( buffer, 80, 166, false, IO::font::CENTER );
            }

            if( move.m_accuracy > 0 && move.m_accuracy <= 100 ) {
                snprintf( buffer, 24, GET_STRING( IO::STR_UI_BAG_ACCURACY ), move.m_accuracy );
            } else {
                snprintf( buffer, 24, GET_STRING( IO::STR_UI_BAG_ACCURACY_NO_MISS ) );
            }
            IO::regularFont->setColor( IO::BLUE_IDX, 1 );
            if( move.m_basePower ) {
                IO::regularFont->printStringC( buffer, 176, 166, false, IO::font::CENTER );
            } else {
                IO::regularFont->printStringC( buffer, 128, 166, false, IO::font::CENTER );
            }
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        }

        IO::regularFont->printBreakingStringC( descr.c_str( ), 33, 83, 196, false, IO::font::LEFT,
                                               11 );
        IO::updateOAM( false );
        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
    }

    void bagUI::drawPkmn( u16 p_itemId, const itemData* p_data ) {
        for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
            const u8 FIRST_LINE  = 27 + 26 * i;
            const u8 SECOND_LINE = 39 + 26 * i;
            const u8 SINGLE_LINE = 32 + 26 * i;

            if( !_playerTeam[ i ].m_boxdata.m_speciesId ) {
                IO::printRectangle( 0, 28 + 26 * i, 133, 28 + 26 * i + 26, true, 0 );
                continue;
            }

            if( _playerTeam[ i ].getItem( ) != _teamItemCache[ i ].first ) {
                // Cache miss, check whether the item can be found somewhere else
                bool gd = false;
                for( u8 j = 0; j < MAX_PARTY_PKMN; ++j ) {
                    if( _playerTeam[ i ].getItem( ) == _teamItemCache[ j ].first ) {
                        // Found the item on a pkmn, probably the items got swapped
                        std::swap( _teamItemCache[ i ], _teamItemCache[ j ] );
                        gd = true;
                        break;
                    }
                }
                for( u8 j = 0; j < MAX_ITEMS_PER_PAGE; ++j ) {
                    if( _playerTeam[ i ].getItem( ) == _itemCache[ j ].first ) {
                        // Found the item in the bag cache, probably the item was given to
                        // the pkmn
                        _teamItemCache[ i ] = _itemCache[ j ];
                        gd                  = true;
                        break;
                    }
                }

                if( !gd ) {
                    // Complete cache miss, need to reload
                    _teamItemCache[ i ].first  = _playerTeam[ i ].getItem( );
                    _teamItemCache[ i ].second = FS::getItemName( _playerTeam[ i ].getItem( ) );
                }
            }

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            if( p_data == nullptr
                || ( _lastPkmnItemType != p_data->m_itemType || p_data->m_itemType == ITEMTYPE_TM
                     || p_data->m_itemType == ITEMTYPE_EVOLUTION ) ) {
                IO::printRectangle( 0, 28 + 26 * i, 133, 28 + 26 * i + 26, true, 0 );
            }

            if( _playerTeam[ i ].isEgg( ) ) {
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_PARTY_EGG ), 45,
                                               SINGLE_LINE, true );
            } else {
                if( p_data == nullptr ) {
                    IO::regularFont->printStringC( _playerTeam[ i ].m_boxdata.m_name, 45,
                                                   FIRST_LINE, true );
                    IO::regularFont->setColor( 0, 2 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 1 );
                    if( _playerTeam[ i ].getItem( ) ) {
                        IO::regularFont->printStringC( _teamItemCache[ i ].second.c_str( ), 45,
                                                       SECOND_LINE, true );
                    } else
                        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_PARTY_NO_ITEM ),
                                                       45, SECOND_LINE, true );

                    continue;
                }
                if( _lastPkmnItemType == p_data->m_itemType && p_data->m_itemType != ITEMTYPE_TM
                    && p_data->m_itemType != ITEMTYPE_EVOLUTION ) {
                    continue;
                }

                IO::regularFont->printStringC( _playerTeam[ i ].m_boxdata.m_name, 45, FIRST_LINE,
                                               true );
                if( p_itemId && p_data->m_itemType == ITEMTYPE_TM ) {
                    u16 currMv = p_data->m_param2;
                    if( currMv == _playerTeam[ i ].getMove( 0 )
                        || currMv == _playerTeam[ i ].getMove( 1 )
                        || currMv == _playerTeam[ i ].getMove( 2 )
                        || currMv == _playerTeam[ i ].getMove( 3 ) ) {
                        IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_ALREADY_LEARNED ),
                                                       45, SECOND_LINE, true, IO::font::LEFT, 11 );
                    } else if( FS::canLearn( _playerTeam[ i ].getSpecies( ),
                                             _playerTeam[ i ].getForme( ), currMv,
                                             FS::LEARN_TM ) ) {
                        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GREEN;
                        IO::regularFont->setColor( IO::COLOR_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_LEARN_POSSIBLE ),
                                                       45, SECOND_LINE, true );
                    } else {
                        IO::regularFont->setColor( IO::RED_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC(
                            GET_STRING( IO::STR_UI_BAG_LEARN_NOT_POSSIBLE ), 45, SECOND_LINE, true,
                            IO::font::LEFT, 11 );
                    }
                } else if( p_itemId && ( p_data->m_itemType & 15 ) == ITEMTYPE_MEDICINE ) {
                    IO::smallFont->setColor( 0, 0 );
                    IO::smallFont->setColor( IO::GRAY_IDX, 1 );
                    IO::smallFont->setColor( 0, 2 );
                    IO::smallFont->printString( "!", 45, SECOND_LINE - 2, true );
                    char buffer[ 10 ];
                    snprintf( buffer, 8, "%d", _playerTeam[ i ].m_level );
                    IO::smallFont->printStringC( buffer, 53, SECOND_LINE - 1, true );

                    snprintf( buffer, 8, "%3d", _playerTeam[ i ].m_stats.m_curHP );
                    IO::smallFont->printStringC( buffer, 45 + 80 - 44, SECOND_LINE - 1, true );
                    snprintf( buffer, 8, "/%d", _playerTeam[ i ].m_stats.m_maxHP );
                    IO::smallFont->printStringC( buffer, 45 + 78 - 20, SECOND_LINE - 1, true );
                } else if( p_itemId && p_data->m_itemType == ITEMTYPE_EVOLUTION ) {
                    if( _playerTeam[ i ].canEvolve( p_itemId, EVOMETHOD_ITEM ) ) {
                        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GREEN;
                        IO::regularFont->setColor( IO::COLOR_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_POSSIBLE ), 45,
                                                       SECOND_LINE, true );
                    } else {
                        IO::regularFont->setColor( IO::RED_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_NOT_POSSIBLE ),
                                                       45, SECOND_LINE, true, IO::font::LEFT, 11 );
                    }
                } else {
                    IO::regularFont->setColor( 0, 2 );
                    IO::regularFont->setColor( IO::GRAY_IDX, 1 );
                    if( _playerTeam[ i ].getItem( ) ) {
                        IO::regularFont->printStringC( _teamItemCache[ i ].second.c_str( ), 45,
                                                       SECOND_LINE, true );
                    } else
                        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_PARTY_NO_ITEM ),
                                                       45, SECOND_LINE, true );
                }
            }
        }
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        if( p_data != nullptr ) { _lastPkmnItemType = p_data->m_itemType; }
    }

    void bagUI::drawItemSub( u16 p_itemId, const itemData* p_data, u16 p_idx ) {
        if( p_itemId != _itemCache[ p_idx ].first && p_data != nullptr ) {

            // Cache miss, check whether the item can be found somewhere else
            bool gd = false;
            for( u8 j = 0; j < MAX_ITEMS_PER_PAGE; ++j ) {
                if( p_itemId == _itemCache[ j ].first ) {
                    // Found the item at a different place in the bag, probably the items got
                    // swapped
                    std::swap( _itemCache[ p_idx ], _itemCache[ j ] );
                    gd = true;
                    break;
                }
            }
            for( u8 j = 0; j < MAX_PARTY_PKMN; ++j ) {
                if( p_itemId == _teamItemCache[ j ].first ) {
                    // Found the item in the pkmn cache, probably the item was taken from
                    // the pkmn
                    _itemCache[ p_idx ] = _teamItemCache[ j ];
                    gd                  = true;
                    break;
                }
            }

            if( !gd ) {
                // Full cache miss
                _itemCache[ p_idx ].first = p_itemId;
                if( p_data->m_itemType != ITEMTYPE_TM ) {
                    _itemCache[ p_idx ].second = FS::getItemName( p_itemId );
                } else {
                    _itemCache[ p_idx ].second = FS::getMoveName( p_data->m_param2 );
                }
            }
        }

        u16 x = IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) ].x;
        u16 y = IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) ].y;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::printRectangle( x - 3, y, 255, y + 20, true, 0 );
        if( !p_itemId || p_data == nullptr ) {
            for( u8 j = 0; j < SPRITES_PER_ITEM_WINDOW; ++j ) {
                IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) + j ].isHidden = true;
            }
            return;
        }
        for( u8 j = 0; j < SPRITES_PER_ITEM_WINDOW; ++j ) {
            IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) + j ].isHidden = false;
        }

        if( getItemChar( p_data->m_itemType ) != getItemChar( ITEMTYPE_COLLECTIBLE )
            && toBagType( p_data->m_itemType ) == bag::bagType::ITEMS ) {
            IO::boldFont->printChar( getItemChar( p_data->m_itemType ), 236, y + 2, true );
        } else if( p_itemId == SAVE::SAV.getActiveFile( ).m_registeredItem ) {
            IO::boldFont->printChar( 'Y', 238, y + 2, true );
        } else if( p_data->m_itemType == ITEMTYPE_TM && BATTLE::isFieldMove( p_data->m_param2 ) ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->printChar( getItemChar( ITEMTYPE_TM ), 236, y + 2, true );
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        } else if( p_data->m_itemType == ITEMTYPE_TM ) {
            IO::boldFont->printChar( getItemChar( ITEMTYPE_TM ), 236, y + 2, true );
        }

        IO::regularFont->printStringC( _itemCache[ p_idx ].second.c_str( ), x + 6, y + 2, true );

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    void drawTop( u8 p_page ) {
        IO::OamTop->oamBuffer[ 0 ].isHidden = true;
        IO::OamTop->oamBuffer[ 1 ].isHidden = true;
        IO::OamTop->oamBuffer[ 2 ].isHidden = true;
        IO::updateOAM( false );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_PAGE_NAME_START + p_page ), 128,
                                       4, false, IO::font::CENTER );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    void bagUI::drawPkmnChoice( ) {
        SpriteEntry* oam                       = IO::Oam->oamBuffer;
        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = true;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = true;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = false;
        for( u8 i = 0; i < 5; ++i ) { oam[ SPR_MSG_BOX_SMALL_OAM_SUB + i ].isHidden = false; }

        IO::updateOAM( true );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_CHOOSE_PKMN ), 67, 5, true,
                                       IO::font::CENTER );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    void bagUI::undrawPkmnChoice( ) {
        SpriteEntry* oam                       = IO::Oam->oamBuffer;
        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = false;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = false;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = false;
        for( u8 i = 0; i < 5; ++i ) { oam[ SPR_MSG_BOX_SMALL_OAM_SUB + i ].isHidden = true; }

        IO::updateOAM( true );
        IO::printRectangle( 0, 0, 5 * 24 + 4, 28, true, 0 );
    }

    void bagUI::printMessage( const char* p_message ) {
        SpriteEntry* oam  = IO::Oam->oamBuffer;
        _lastPkmnItemType = 255;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        if( p_message ) {
            for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }

            IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                                256 - oam[ SPR_MSG_BOX_OAM_SUB ].x,
                                oam[ SPR_MSG_BOX_OAM_SUB ].y + 31, true, 0 );
            auto lineCnt = IO::regularFont->printBreakingStringC(
                               p_message, 128, oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, 196, true,
                               IO::font::CENTER, 14, ' ', 0, false, -1 )
                           - 1;
            IO::regularFont->printBreakingStringC( p_message, 128,
                                                   oam[ SPR_MSG_BOX_OAM_SUB ].y + 8 - lineCnt * 7,
                                                   196, true, IO::font::CENTER, 14 );
        }

        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = true;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = true;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = true;

        IO::updateOAM( true );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>
    bagUI::printYNMessage( const char* p_message, u8 p_selection, bool p_bottom ) {
        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> res
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );
        SpriteEntry* oam  = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        _lastPkmnItemType = 255;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        if( p_message ) {
            for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }

            IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                                256 - oam[ SPR_MSG_BOX_OAM_SUB ].x,
                                oam[ SPR_MSG_BOX_OAM_SUB ].y + 31, true, 0 );

            auto lineCnt = IO::regularFont->printBreakingStringC(
                               p_message, 128, oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, 196, true,
                               IO::font::CENTER, 14, ' ', 0, false, -1 )
                           - 1;
            IO::regularFont->printBreakingStringC( p_message, 128,
                                                   oam[ SPR_MSG_BOX_OAM_SUB ].y + 8 - lineCnt * 7,
                                                   196, true, IO::font::CENTER, 14 );
        }

        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = true;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = true;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = true;
        for( u8 i = 0; i < NUM_CB_CHOICES; i++ ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = ( ( i & 1 ) == ( p_selection & 1 ) ) ? SPR_SELECTED_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }

        if( p_message ) {
            for( u8 i = 0; i < 2; i++ ) {
                for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                    oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                }
            }

            IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 95,
                                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 31, true, 0 );
            IO::regularFont->printString(
                GET_STRING( IO::STR_UI_YES ), oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 8, p_bottom, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 32 ),
                           IO::yesNoBox::YES ) );

            IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 95,
                                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 31, true, 0 );
            IO::regularFont->printString(
                GET_STRING( IO::STR_UI_NO ), oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 8, p_bottom, IO::font::CENTER );

            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 32 ),
                           IO::yesNoBox::NO ) );
        }
        IO::updateOAM( p_bottom );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, s32>> bagUI::drawCounter( u16 p_message, s32 p_min,
                                                                     s32 p_max ) {
        auto  res         = std::vector<std::pair<IO::inputTarget, s32>>( );
        auto& oam         = IO::Oam->oamBuffer;
        _lastPkmnItemType = 255;

        // compute num digits required
        s32 mx = p_max, dd = 1;
        if( -p_min > p_max ) { mx = -p_min; }

        u8 digs = 0;
        for( auto i = mx; i > 0; i /= 10, ++digs, dd *= 10 ) {}
        dd /= 10;

        // Back
        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = true;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = true;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = true;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        // message window
        for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }

        IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                            256 - oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y + 31,
                            true, 0 );
        IO::regularFont->printStringC( GET_STRING( p_message ), 128,
                                       oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, true, IO::font::CENTER );

        // counter window
        for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].isHidden = false;
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].palette  = SPR_BOX_PAL_SUB;
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].x += 51;
        }
        IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 95,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 31, true, 0 );

        // up / down

        for( u8 i = 0; i < digs; ++i, dd /= 10 ) {
            auto upx = 128 - ( digs * 10 ) + i * 20;
            auto upy = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y - 4;
            res.push_back( std::pair( IO::inputTarget( upx, upy - 4, upx + 16, upy + 12 ), dd ) );
            IO::printRectangle( upx, upy - 4, upx + 16, upy + 12, true, 0 );
            IO::boldFont->printStringC( "+", upx + 8, upy, true, IO::font::CENTER );

            auto downx = 128 - ( digs * 10 ) + i * 20;
            auto downy = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 20;
            res.push_back(
                std::pair( IO::inputTarget( downx, downy + 4, downx + 16, downy + 16 ), -dd ) );
            IO::printRectangle( downx, downy + 4, downx + 16, downy + 16, true, 0 );
            IO::boldFont->printStringC( "-", downx + 8, downy, true, IO::font::CENTER );
        }

        // windows for confirm (p_min - 2) and cancel (0)
        for( u8 i = 2; i <= 3; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette  = SPR_BOX_PAL_SUB;
            }
        }

        // exit / cancel
        res.push_back( std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 96,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 32 ),
                                  p_min - 3 ) );
        res.push_back( std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 96,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 32 ),
                                  0 ) );
        IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x,
                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y,
                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 95,
                            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 31, true, 0 );
        IO::regularFont->printString(
            GET_STRING( IO::STR_UI_CANCEL ), oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 48,
            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y + 8, true, IO::font::CENTER );

        // confirm
        IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x,
                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y,
                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 95,
                            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 31, true, 0 );
        // TODO: Use more specific confirm message?
        IO::regularFont->printString(
            GET_STRING( IO::STR_UI_SELECT ), oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 48,
            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 8, true, IO::font::CENTER );

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 96,
                                                   oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y + 32 ),
                                  p_min - 2 ) );
        IO::updateOAM( true );
        return res;
    }

    void bagUI::updateCounter( s32 p_newValue, u8 p_selectedDigit, u8 p_numDigs ) {
        auto& oam = IO::Oam->oamBuffer;
        IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 9,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 95,
                            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 23, true, 0 );

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
            auto upx = 128 - ( p_numDigs * 10 ) + ( dg - 1 ) * 20;
            auto upy = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y - 4;

            IO::regularFont->printString( std::to_string( p_newValue % 10 ).c_str( ), upx + 8,
                                          upy + 14, true, IO::font::CENTER );
        }
    }

    void bagUI::hoverCounterButton( s32 p_min, s32 p_max, s32 p_button ) {
        (void) p_max;
        auto& oam = IO::Oam->oamBuffer;
        for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
            oam[ SPR_CHOICE_START_OAM_SUB( 2 ) + j ].palette = SPR_BOX_PAL_SUB;
            oam[ SPR_CHOICE_START_OAM_SUB( 3 ) + j ].palette = SPR_BOX_PAL_SUB;
        }

        if( p_button == p_min - 2 ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( 3 ) + j ].palette = SPR_SELECTED_PAL_SUB;
            }
        } else if( p_button == p_min - 3 || p_button == 0 ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( 2 ) + j ].palette = SPR_SELECTED_PAL_SUB;
            }
        }

        IO::updateOAM( true );
    }

    void bagUI::destroyCounter( ) {
        auto& oam = IO::Oam->oamBuffer;
        for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + j ].x -= 51;
        }
    }

    std::vector<std::pair<IO::inputTarget, u8>>
    bagUI::drawChoice( u16 p_item, const itemData* p_data, const std::vector<u16>& p_texts ) {
        auto  res         = std::vector<std::pair<IO::inputTarget, u8>>( );
        auto& oam         = IO::Oam->oamBuffer;
        _lastPkmnItemType = 255;

        // Back
        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = true;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = true;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = true;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        res.push_back( std::pair( IO::inputTarget( 1, 1, 0, 0 ), IO::choiceBox::BACK_CHOICE ) );

        for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }

        for( u8 i = 0; i < p_texts.size( ); ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = !i ? SPR_SELECTED_PAL_SUB : SPR_BOX_PAL_SUB;
            }
            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                           i ) );
        }
        IO::updateOAM( true );

        char buffer[ 100 ];
        if( p_data->m_itemType != ITEMTYPE_TM ) {
            bool found = false;
            for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
                if( _itemCache[ i ].first == p_item ) {
                    snprintf( buffer, 99, GET_STRING( IO::STR_UI_BAG_ASK_ITEM_ACTION ),
                              _itemCache[ i ].second.c_str( ) );
                    found = true;
                    break;
                }
            }
            if( !found ) {
                for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
                    if( _teamItemCache[ i ].first == p_item ) {
                        snprintf( buffer, 99, GET_STRING( IO::STR_UI_BAG_ASK_ITEM_ACTION ),
                                  _teamItemCache[ i ].second.c_str( ) );
                        found = true;
                        break;
                    }
                }
            }
            if( !found ) [[unlikely]] {
                snprintf( buffer, 99, GET_STRING( IO::STR_UI_BAG_ASK_ITEM_ACTION ),
                          FS::getItemName( p_item ).c_str( ) );
            }
        } else {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_BAG_ASK_ITEM_ACTION ),
                      FS::getItemName( p_item ).c_str( ) );
        }

        IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                            256 - oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y + 31,
                            true, 0 );
        IO::regularFont->printStringC( buffer, 128, oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, true,
                                       IO::font::CENTER );

        for( u8 i = 0; i < p_texts.size( ); ++i ) {
            IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 95,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 31, true, 0 );
            IO::regularFont->printString(
                GET_STRING( p_texts[ i ] ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );
        }

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        return res;
    }

    std::vector<std::pair<IO::inputTarget, u8>> bagUI::drawMoveChoice( const boxPokemon* p_pokemon,
                                                                       u16 p_extraMove ) {
        auto  res         = std::vector<std::pair<IO::inputTarget, u8>>( );
        auto& oam         = IO::Oam->oamBuffer;
        _lastPkmnItemType = 255;

        // Back
        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = true;
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = true;
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = false;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_ARROW_UP_OAM_SUB ].x + 16,
                                                   oam[ SPR_ARROW_UP_OAM_SUB ].y + 16, 16 ),
                                  IO::choiceBox::BACK_CHOICE ) );

        for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = false; }

        for( u8 i = 0; i < 4 + !!p_extraMove; ++i ) {
            if( i < 4 && !p_pokemon->getMove( i ) ) { break; }

            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = false;
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = !i ? SPR_SELECTED_PAL_SUB : SPR_BOX_PAL_SUB;
            }
            res.push_back(
                std::pair( IO::inputTarget( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 96,
                                            oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 32 ),
                           i ) );
        }
        IO::updateOAM( true );

        IO::printRectangle( oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y,
                            256 - oam[ SPR_MSG_BOX_OAM_SUB ].x, oam[ SPR_MSG_BOX_OAM_SUB ].y + 31,
                            true, 0 );
        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_CHOOSE_MOVE ), 128,
                                       oam[ SPR_MSG_BOX_OAM_SUB ].y + 8, true, IO::font::CENTER );

        for( u8 i = 0; i < 4 + !!p_extraMove; ++i ) {
            if( i < 4 && !p_pokemon->getMove( i ) ) { break; }
            auto mv = ( i < 4 ) ? p_pokemon->getMove( i ) : p_extraMove;

            IO::printRectangle( oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 95,
                                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 31, true, 0 );
            IO::regularFont->printString(
                FS::getMoveName( mv ).c_str( ), oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 48,
                oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 8, true, IO::font::CENTER );
        }

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        return res;
    }

    void bagUI::selectChoice( u8 p_selection ) {
        if( p_selection >= NUM_CB_CHOICES ) { return; }
        auto& oam = IO::Oam->oamBuffer;

        for( u8 i = 0; i < NUM_CB_CHOICES; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                    = i == p_selection ? SPR_SELECTED_PAL_SUB : SPR_BOX_PAL_SUB;
            }
        }
        IO::updateOAM( true );
    }

    void bagUI::selectMoveChoice( u8 p_selection ) {
        selectChoice( p_selection );
    }

    void bagUI::selectItem( u8 p_idx, std::pair<u16, u16> p_item, const itemData* p_data ) {
        if( p_idx != _selectedIdx ) {
            // Unselect old item
            drawTop( _currentPage );
        }
        _selectedIdx = p_idx;

        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_ITEM_WINDOW; ++j ) {
                IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( i ) + j ].palette = SPR_BOX_PAL_SUB;
            }
        }

        for( u8 j = 0; j < SPRITES_PER_ITEM_WINDOW; ++j ) {
            IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) + j ].isHidden = p_data == nullptr;
            IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) + j ].palette
                = SPR_SELECTED_PAL_SUB;
        }
        IO::updateOAM( true );
        drawPkmn( p_item.first, p_data );
        drawItemTop( p_item.first, p_data, p_item.second );
    }

    void bagUI::drawBagPage( bag::bagType                                                 p_page,
                             const std::vector<std::pair<std::pair<u16, u16>, itemData>>& p_items,
                             u8 p_selection ) {
        _currentPage = p_page;
        _selectedIdx = p_selection;
        auto& oam    = IO::Oam->oamBuffer;

        oam[ SPR_ARROW_UP_OAM_SUB ].isHidden   = !p_items.size( );
        oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden = !p_items.size( );
        oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden = false;
        for( u8 i = 0; i < 13; ++i ) { oam[ SPR_MSG_BOX_OAM_SUB + i ].isHidden = true; }
        for( u8 i = 0; i < NUM_CB_CHOICES; ++i ) {
            for( u8 j = 0; j < SPRITES_PER_CB_WINDOW; j++ ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden = true;
            }
        }
        IO::updateOAM( true );

        showActiveBag( p_page );

        drawTop( p_page );
        IO::initColors( );

        if( p_selection < p_items.size( ) ) {
            auto& idat = p_items[ p_selection ].second;
            selectItem( p_selection, p_items[ p_selection ].first, &idat );
        } else {
            _lastPkmnItemType = 255;
            drawPkmn( 0, nullptr );
        }

        if( p_items.size( ) ) {
            for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
                if( i < p_items.size( ) ) {
                    u16   curitem = p_items[ i ].first.first;
                    auto& data    = p_items[ i ].second;
                    drawItemSub( curitem, &data, i );
                } else {
                    drawItemSub( 0, nullptr, i );
                }
            }
        } else {
            for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) { drawItemSub( 0, nullptr, i ); }
            IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_EMPTY ), 182, 89, true,
                                           IO::font::CENTER );
            IO::updateOAM( false );
        }
        IO::updateOAM( true );
    }

    std::vector<std::pair<IO::inputTarget, u8>> bagUI::getPkmnInputTarget( ) const {
        auto res = std::vector<std::pair<IO::inputTarget, u8>>( );
        for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
            if( !_playerTeam[ i ].getSpecies( ) ) { break; }
            res.push_back(
                std::pair( IO::inputTarget( 0, 28 + 26 * i, 128, 28 + 26 * i + 26 ), i ) );
        }
        return res;
    }

    std::pair<IO::inputTarget, u8> bagUI::getButtonInputTarget( u8 p_button ) const {
        auto& oam = IO::Oam->oamBuffer;
        switch( p_button ) {
        case IO::choiceBox::BACK_CHOICE:
            return std::pair( IO::inputTarget( oam[ SPR_ARROW_BACK_OAM_SUB ].x + 16,
                                               oam[ SPR_ARROW_BACK_OAM_SUB ].y + 16, 16 ),
                              p_button );
        case IO::choiceBox::NEXT_PAGE_CHOICE:
            return std::pair( IO::inputTarget( oam[ SPR_ARROW_DOWN_OAM_SUB ].x + 16,
                                               oam[ SPR_ARROW_DOWN_OAM_SUB ].y + 16, 16 ),
                              p_button );
        case IO::choiceBox::PREV_PAGE_CHOICE:
            return std::pair( IO::inputTarget( oam[ SPR_ARROW_UP_OAM_SUB ].x + 16,
                                               oam[ SPR_ARROW_UP_OAM_SUB ].y + 16, 16 ),
                              p_button );
        [[unlikely]] default:
            break;
        }

        return std::pair( IO::inputTarget( 0, 0, 0 ), 0 );
    }

    void bagUI::selectPkmn( u8 p_selection ) {
        auto& oam = IO::Oam->oamBuffer;
        if( p_selection < MAX_PARTY_PKMN ) {
            for( u8 i = 0; i < 5; ++i ) {
                oam[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].isHidden = false;
                oam[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].y        = 29 + 26 * p_selection;
            }
        } else {
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].isHidden = true; }
        }
        IO::updateOAM( true );
    }

    std::vector<std::pair<IO::inputTarget, u8>> bagUI::getTouchPositions( ) {
        auto         res = std::vector<std::pair<IO::inputTarget, u8>>( );
        SpriteEntry* oam = IO::Oam->oamBuffer;

        // back
        if( !oam[ SPR_ARROW_BACK_OAM_SUB ].isHidden ) {
            res.push_back( std::pair<IO::inputTarget, u8>{
                IO::inputTarget( oam[ SPR_ARROW_BACK_OAM_SUB ].x, oam[ SPR_ARROW_BACK_OAM_SUB ].y,
                                 oam[ SPR_ARROW_BACK_OAM_SUB ].x + 24,
                                 oam[ SPR_ARROW_BACK_OAM_SUB ].y + 24 ),
                0 } );
        }

        // forwards
        if( !oam[ SPR_ARROW_DOWN_OAM_SUB ].isHidden ) {
            res.push_back( std::pair<IO::inputTarget, u8>{
                IO::inputTarget( oam[ SPR_ARROW_DOWN_OAM_SUB ].x, oam[ SPR_ARROW_DOWN_OAM_SUB ].y,
                                 oam[ SPR_ARROW_DOWN_OAM_SUB ].x + 24,
                                 oam[ SPR_ARROW_DOWN_OAM_SUB ].y + 24 ),
                90 } );
        }

        // backwards
        if( !oam[ SPR_ARROW_UP_OAM_SUB ].isHidden ) {
            res.push_back( std::pair<IO::inputTarget, u8>{
                IO::inputTarget( oam[ SPR_ARROW_UP_OAM_SUB ].x, oam[ SPR_ARROW_UP_OAM_SUB ].y,
                                 oam[ SPR_ARROW_UP_OAM_SUB ].x + 24,
                                 oam[ SPR_ARROW_UP_OAM_SUB ].y + 24 ),
                91 } );
        }

        // page icons

        for( u8 i = 0; i < 5; ++i ) {
            res.push_back( std::pair<IO::inputTarget, u8>{
                IO::inputTarget( oam[ SPR_BAG_ICON_OAM_SUB + i ].x,
                                 oam[ SPR_BAG_ICON_OAM_SUB + i ].y,
                                 oam[ SPR_BAG_ICON_OAM_SUB + i ].x + 27,
                                 oam[ SPR_BAG_ICON_OAM_SUB + i ].y + 27 ),
                1 + i } );
        }

        // items
        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            if( !oam[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].isHidden ) {
                res.push_back( std::pair<IO::inputTarget, u8>{
                    IO::inputTarget( oam[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].x,
                                     oam[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].y,
                                     oam[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].x + 118,
                                     oam[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].y + 19 ),
                    100 + i } );
            }
        }
        return res;
    }

    void bagUI::updateSprite( touchPosition p_touch ) {
        IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].x = p_touch.px - 16;
        IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].y = p_touch.py - 16;
        IO::updateOAM( true );
    }

    bool bagUI::getSprite( u8 p_idx, std::pair<u16, u16> p_item, const itemData* p_data ) {
        if( !p_item.first ) return false;

        if( p_idx >= MAX_ITEMS_PER_PAGE ) { // It's a PKMN
            if( !_playerTeam[ p_idx - MAX_ITEMS_PER_PAGE ].getItem( ) ) [[unlikely]] {
                // Something went wrong
                return false;
            }
        }

        if( p_data->m_itemType != ITEMTYPE_TM ) {
            IO::loadItemIcon( p_item.first, 0, 0, SPR_TRANSFER_OAM_SUB, SPR_TRANSFER_PAL_SUB,
                              IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].gfxIndex );
        } else {
            BATTLE::moveData move = FS::getMoveData( p_data->m_param2 );

            u8 tmtype = p_data->m_effect;
            if( tmtype == 1 && BATTLE::isFieldMove( p_data->m_param2 ) ) { tmtype = 0; }
            IO::loadTMIcon( move.m_type, tmtype, 0, 0, SPR_TRANSFER_OAM_SUB, SPR_TRANSFER_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].gfxIndex );
        }

        selectItem( p_idx, p_item, p_data );
        return true;
    }

    void bagUI::dropSprite( u8 p_idx, std::pair<u16, u16> p_item, const itemData* p_data ) {
        IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].isHidden = true;
        selectItem( p_idx, p_item, p_data );
    }
} // namespace BAG
