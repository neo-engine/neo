/*
Pokémon neo
------------------------------

file        : bagUI.cpp
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

#include "bagUI.h"
#include "bagViewer.h"
#include "berry.h"
#include "defines.h"
#include "fs.h"
#include "item.h"
#include "messageBox.h"
#include "saveGame.h"
#include "screenFade.h"

#include <algorithm>
#include <cstdio>
#include <vector>

// Sprites
#include "BagBall1.h"
#include "BagBall2.h"
#include "BagBerry1.h"
#include "BagBerry2.h"
#include "BagHm1.h"
#include "BagHm2.h"
#include "BagKey1.h"
#include "BagKey2.h"
#include "BagMediine1.h"
#include "BagMediine2.h"
#include "backarrow.h"
#include "NoItem.h"
#include "Up.h"
#include "Down.h"
#include "Back.h"

#include "noselection_64_32.h"
#include "noselection_64_20.h"
#include "noselection_96_32_4.h"

namespace BAG {
    const unsigned short* bagPals[ 10 ]
        = {BagBall1Pal, BagBall2Pal,  BagMediine1Pal, BagMediine2Pal, BagHm1Pal,
           BagHm2Pal,   BagBerry1Pal, BagBerry2Pal,   BagKey1Pal,     BagKey2Pal};
    const unsigned int* bagTiles[ 10 ]
        = {BagBall1Tiles, BagBall2Tiles,  BagMediine1Tiles, BagMediine2Tiles, BagHm1Tiles,
           BagHm2Tiles,   BagBerry1Tiles, BagBerry2Tiles,   BagKey1Tiles,     BagKey2Tiles};


#define SPR_TRANSFER_OAM_SUB 0
#define SPR_MESSAGE_WINDOW_OAM_SUB 1
#define SPR_ARROW_UP_OAM_SUB 18
#define SPR_ARROW_DOWN_OAM_SUB 19
#define SPR_X_OAM_SUB 20
#define SPR_ARROW_BACK_OAM_SUB 21
#define SPR_ITEM_WINDOW_OAM_SUB( p_window ) ( 22 + 6 * ( p_window ) )
#define SPR_PKMN_START_OAM_SUB 80
#define SPR_BAG_ICON_OAM_SUB 87

#define SPR_BOX_PAL_SUB 0
#define SPR_SELECTED_PAL_SUB 1
#define SPR_ARROW_X_PAL_SUB 2
#define SPR_PKMN_PAL_SUB 3
#define SPR_BAG_ICON_PAL_SUB 9
#define SPR_BAG_ICON_SEL_PAL_SUB 10
#define SPR_ITEM_ICON_PAL_SUB 11
#define SPR_BACK_PAL_SUB 12
#define SPR_DOWN_PAL_SUB 13
#define SPR_TRANSFER_PAL_SUB 15

    void showActiveBag( u8 p_bagNo ) {
        u16 tileIdx = IO::Oam->oamBuffer[ SPR_BAG_ICON_OAM_SUB + p_bagNo ].gfxIndex;

        for( u8 i = 0; i < 5; ++i ) {
            IO::loadSprite( SPR_BAG_ICON_OAM_SUB + i, SPR_BAG_ICON_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_BAG_ICON_OAM_SUB + i ].gfxIndex,
                        27 * i, 0, 32, 32,
                        bagPals[ 2 * i ], bagTiles[ 2 * i ],
                        NoItemTilesLen, false, false, false, OBJPRIORITY_3, true,
                        OBJMODE_BLENDED );
        }

        IO::loadSprite( SPR_BAG_ICON_OAM_SUB + p_bagNo, SPR_BAG_ICON_SEL_PAL_SUB, tileIdx,
                27 * p_bagNo, 0, 32, 32,
                bagPals[ 2 * p_bagNo + 1 ], bagTiles[ 2 * p_bagNo + 1 ],
                NoItemTilesLen, false, false, false, OBJPRIORITY_3, true, OBJMODE_BLENDED );

    }

    void initColors( ) {
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE_SUB[ IO::RED_IDX ]   = IO::RED;
        BG_PALETTE_SUB[ IO::BLUE_IDX ]  = IO::BLUE;
        BG_PALETTE[ IO::WHITE_IDX ]     = IO::WHITE;
        BG_PALETTE[ IO::GRAY_IDX ]      = IO::GRAY;
        BG_PALETTE[ IO::BLACK_IDX ]     = IO::BLACK;
        BG_PALETTE[ IO::RED_IDX ]       = IO::RED;
        BG_PALETTE[ IO::BLUE_IDX ]      = IO::BLUE;
    }

    void bagUI::init( ) {
        IO::vramSetup( );
        swiWaitForVBlank( );
        IO::clearScreen( true, true );
        IO::resetScale( true, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BagUpper" );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "BagLower", 512, 49152,
                             true );

        bgUpdate( );
        initColors( );

        for( u8 i = 0; i < 6; ++i ) {
            if( !_playerTeam[ i ].getSpecies( ) ) { break; }

            _teamItemCache[ i ] = std::pair( _playerTeam[ i ].getItem( ),
                ITEM::getItemName( _playerTeam[ i ].getItem( ) ) );
        }

        IO::initOAMTable( false );
        // Don't init anything else for the top screen here

        IO::initOAMTable( true );
        SpriteEntry* oam = IO::Oam->oamBuffer;

        u16 tileCnt = drawPkmnIcons( );

        tileCnt = IO::loadSprite( SPR_ARROW_BACK_OAM_SUB, SPR_BACK_PAL_SUB,
                                  tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 26,
                                  32, 32, BackPal, BackTiles, BackTilesLen, false, false, false,
                                  OBJPRIORITY_3, true );

        tileCnt = IO::loadSprite( SPR_ARROW_DOWN_OAM_SUB, SPR_DOWN_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 60, SCREEN_HEIGHT - 26,
                                  32, 32, DownPal, DownTiles, DownTilesLen, false, false, false,
                                  OBJPRIORITY_3, true );
        tileCnt = IO::loadSprite( SPR_ARROW_UP_OAM_SUB, SPR_ARROW_X_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26,
                                  32, 32, UpPal, UpTiles, UpTilesLen, false, false, false,
                                  OBJPRIORITY_3, true );


        for( u8 i = 0; i < 5; ++i ) {
            tileCnt = IO::loadSprite( SPR_BAG_ICON_OAM_SUB + i, SPR_BAG_ICON_PAL_SUB, tileCnt,
                                      26 * i, 0, 32, 32, bagPals[ 2 * i ], bagTiles[ 2 * i ],
                                      NoItemTilesLen, false,
                                      false, false, OBJPRIORITY_3, true );
        }

        tileCnt = IO::loadSprite( SPR_TRANSFER_OAM_SUB, SPR_TRANSFER_PAL_SUB,
                                  tileCnt, SCREEN_WIDTH - 92,
                                  SCREEN_HEIGHT - 26, 32, 32, 0, 0, NoItemTilesLen, false,
                                  false, true, OBJPRIORITY_3, true );

        // item windows

        tileCnt =
            IO::loadSprite( SPR_ITEM_WINDOW_OAM_SUB( 0 ), SPR_BOX_PAL_SUB,
                                  tileCnt, 0, 0, 32, 32, noselection_64_20Pal,
                                  noselection_64_20Tiles, NoItemTilesLen, false,
                                  false, true, OBJPRIORITY_3, true,
                                  OBJMODE_BLENDED );

        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            for( u8 j = 0; j < 4; ++j ) {
                IO::loadSprite( SPR_ITEM_WINDOW_OAM_SUB( i ) + j, SPR_BOX_PAL_SUB,
                        oam[ SPR_ITEM_WINDOW_OAM_SUB( 0 ) ].gfxIndex,
                        136 + 24 * j, 6 + 20 * i, 32, 32, 0, 0, NoItemTilesLen, false,
                        false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_ITEM_WINDOW_OAM_SUB( i ) + 4, SPR_BOX_PAL_SUB,
                        oam[ SPR_ITEM_WINDOW_OAM_SUB( 0 ) ].gfxIndex,
                        254 - 32, 6 + 20 * i - 12, 32, 32, 0, 0, NoItemTilesLen, true,
                        true, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }

        IO::copySpritePal( noselection_96_32_4Pal, SPR_SELECTED_PAL_SUB, 0, 2 * 8, true );
        IO::updateOAM( true );
    }

    u16 bagUI::drawPkmnIcons( ) {
        u16 tileCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            auto acPkmn = _playerTeam[ i ];
            if( !acPkmn.getSpecies( ) ) break;
            if( acPkmn.isEgg( ) )
                tileCnt = IO::loadEggIcon( 4, 30 + i * 26, SPR_PKMN_START_OAM_SUB + i,
                        SPR_PKMN_PAL_SUB + i, tileCnt );
            else
                tileCnt = IO::loadPKMNIcon(
                    acPkmn.getSpecies( ), 4, 30 + i * 26, SPR_PKMN_START_OAM_SUB + i,
                    SPR_PKMN_PAL_SUB + i,
                    tileCnt, true, acPkmn.getForme( ), acPkmn.isShiny( ), acPkmn.isFemale( ) );
            IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + i ].priority = OBJPRIORITY_3;
        }
        return tileCnt;
    }

    void drawItemTop( u16 p_itemId, const ITEM::itemData* p_data, u16 p_count ) {
        std::string display;
        std::string descr;
        IO::OamTop->oamBuffer[ 0 ].isHidden = true;
        IO::OamTop->oamBuffer[ 1 ].isHidden = true;
        IO::OamTop->oamBuffer[ 2 ].isHidden = true;
        IO::updateOAM( false );

        if( !p_itemId || p_data == nullptr ) { return; }

        if( p_data->m_itemType != ITEM::ITEMTYPE_TM ) {
            IO::loadItemIcon( p_itemId, 112, 44, 0, 0, 0, false );

            display = ITEM::getItemName( p_itemId );
            descr   = ITEM::getItemDescr( p_itemId );
            char buffer[ 100 ];

            if( p_data->m_itemType != ITEM::ITEMTYPE_KEYITEM
                && p_data->m_itemType != ITEM::ITEMTYPE_FORMECHANGE ) {
                snprintf( buffer, 99, "x %d", p_count );
                IO::regularFont->printStringC( buffer, 146, 52, false );
            }

            if( p_data->m_itemType & ITEM::ITEMTYPE_BERRY ) {
                /*
                IO::regularFont->setColor( IO::RED_IDX, 1 );
                snprintf( buffer, 99, GET_STRING( 16 ),
                          ( curr->m_berryData.m_berryGuete == berry::berryGueteType::HARD )
                              ? GET_STRING( 17 )
                              : ( ( curr->m_berryData.m_berryGuete == berry::berryGueteType::SOFT )
                                      ? GET_STRING( 18 )
                                      : ( ( curr->m_berryData.m_berryGuete
                                            == berry::berryGueteType::SUPER_HARD )
                                              ? GET_STRING( 19 )
                                              : ( ( curr->m_berryData.m_berryGuete
                                                    == berry::berryGueteType::SUPER_SOFT )
                                                      ? GET_STRING( 20 )
                                                      : ( ( curr->m_berryData.m_berryGuete
                                                            == berry::berryGueteType::VERY_HARD )
                                                              ? GET_STRING( 21 )
                                                              : ( GET_STRING( 22 ) ) ) ) ) ) );
                IO::regularFont->printStringC( buffer, 24, 145, false );
                IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                snprintf( buffer, 99, GET_STRING( 23 ), curr->m_berryData.m_berrySize / 10.0 );
                IO::regularFont->printStringC( buffer, 140, 145, false );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );

                u8 poses[ 5 ] = {18, 66, 124, 150, 194};
                u8 mx         = 0;
                for( u8 i = 0; i < 5; ++i )
                    mx = std::max( mx, curr->m_berryData.m_berryTaste[ i ] );
                for( u8 i = 0; i < 5; ++i ) {
                    if( curr->m_berryData.m_berryTaste[ i ] != mx ) {
                        IO::regularFont->setColor( IO::GRAY_IDX, 1 );
                        IO::regularFont->setColor( IO::WHITE_IDX, 2 );
                    } else {
                        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                    }
                    IO::regularFont->printStringC( GET_STRING( 24 + i ), poses[ i ], 160, false );
                }
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                */
            }
        } else {
            descr   = MOVE::getMoveDescr( p_data->m_param2 );
            MOVE::moveData move = MOVE::getMoveData( p_data->m_param2 );
            u16 tileCnt = IO::loadTMIcon( move.m_type, MOVE::isFieldMove( p_data->m_param2 ), 112,
                                          44, 0, 0, 0, false );

            display = ITEM::getItemName( p_itemId ) + ": "
                      + MOVE::getMoveName( p_data->m_param2 );

            IO::regularFont->printStringC( GET_STRING( 29 ), 33, 145, false );
            tileCnt
                = IO::loadTypeIcon( move.m_type, 62, 144, 1, 1, tileCnt, false, CURRENT_LANGUAGE );

            IO::regularFont->printStringC( GET_STRING( 30 ), 100, 145, false );
            IO::loadDamageCategoryIcon( move.m_category, 152, 144, 2, 2, tileCnt, false );

            IO::regularFont->printStringC( GET_STRING( 31 ), 190, 145, false );
            char buffer[ 100 ];
            snprintf( buffer, 99, "%2d", move.m_pp );
            IO::regularFont->printStringC( buffer, 229, 145, false, IO::font::RIGHT );

            IO::regularFont->setColor( IO::RED_IDX, 1 );
            IO::regularFont->printStringC( GET_STRING( 32 ), 33, 160, false );
            if( move.m_category != MOVE::STATUS && move.m_basePower > 1 )
                snprintf( buffer, 99, "%3d", move.m_basePower );
            else
                snprintf( buffer, 99, "---" );
            IO::regularFont->printStringC( buffer, 108, 160, false, IO::font::RIGHT );

            IO::regularFont->setColor( IO::BLUE_IDX, 1 );
            IO::regularFont->printStringC( GET_STRING( 33 ), 124, 160, false );
            if( move.m_accuracy )
                snprintf( buffer, 99, "%3d", move.m_accuracy );
            else
                std::sprintf( buffer, "---" );
            IO::regularFont->printStringC( buffer, 229, 160, false, IO::font::RIGHT );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        }

        IO::regularFont->printStringC( display.c_str( ), 128, 26, false, IO::font::CENTER );

        IO::regularFont->printBreakingStringC( descr.c_str( ), 33, 83, 196, false,
                IO::font::LEFT, 11 );
        IO::updateOAM( false );
    }

    std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>>
    bagUI::drawPkmn( u16 p_itemId, const ITEM::itemData* p_data ) {
        std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> res;

        for( u8 i = 0; i < 6; ++i ) {
            const u8 FIRST_LINE = 33 + 26 * i;
            const u8 SECOND_LINE = 44 + 26 * i;
            const u8 SINGLE_LINE = 38 + 26 * i;

            if( !_playerTeam[ i ].m_boxdata.m_speciesId ) break;

            if( _playerTeam[ i ].getItem( )
                    != _teamItemCache[ i ].first ) {
                // Cache miss, check whether the item can be found somewhere else
                bool gd = false;
                for( u8 j = 0; j < 6; ++j ) {
                    if( _playerTeam[ i ].getItem( ) == _teamItemCache[ j ].first ) {
                        // Found the item on a pkmn, probably the items got swapped
                        std::swap(  _teamItemCache[ i ], _teamItemCache[ j ] );
                        gd = true;
                        break;
                    }
                }
                for( u8 j = 0; j < MAX_ITEMS_PER_PAGE; ++j ) {
                    if( _playerTeam[ i ].getItem( ) == _itemCache[ j ].first ) {
                        // Found the item in the bag cache, probably the item was given to
                        // the pkmn
                        _teamItemCache[ i ] = _itemCache[ j ];
                        gd = true;
                        break;
                    }
                }

                if( !gd ) {
                    // Complete cache miss, need to reload
                    _teamItemCache[ i ].first = _playerTeam[ i ].getItem( );
                    _teamItemCache[ i ].second = ITEM::getItemName( _playerTeam[ i ].getItem( ) );
                }
            }

            if( p_data != nullptr && ( _lastPkmnItemType != p_data->m_itemType
                    || p_data->m_itemType == ITEM::ITEMTYPE_TM ) ) {
                IO::printRectangle( 0, 33 + 26 * i, 128, 33 + 26 * i + 26, true, 0 );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            }

            if( _playerTeam[ i ].isEgg( ) ) {
                if( p_data != nullptr && _lastPkmnItemType != p_data->m_itemType ) {
                    IO::regularFont->printStringC( GET_STRING( 34 ), 45, SINGLE_LINE, true );
                }
                // res.push_back(
                //    {IO::inputTarget( 0, 33 + 26 * i, 128, 33 + 26 * i + 26 ), {0, true}} );
            } else {
                res.push_back( {IO::inputTarget( 0, 33 + 26 * i, 128, 33 + 26 * i + 26 ),
                                {_playerTeam[ i ].getItem( ), true}} );
                if( p_data == nullptr ||
                        ( _lastPkmnItemType == p_data->m_itemType
                          && p_data->m_itemType != ITEM::ITEMTYPE_TM ) ) { continue; }

                IO::regularFont->printStringC( _playerTeam[ i ].m_boxdata.m_name, 40,
                            FIRST_LINE, true );
                if( p_itemId && p_data->m_itemType == ITEM::ITEMTYPE_TM ) {
                    u16 currMv = p_data->m_param2;
                    if( currMv == _playerTeam[ i ].getMove( 0 )
                        || currMv
                               == _playerTeam[ i ].getMove( 1 )
                        || currMv
                               == _playerTeam[ i ].getMove( 2 )
                        || currMv
                               == _playerTeam[ i ].getMove( 3 ) ) {
                        IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( 35 ), 40, SECOND_LINE, true,
                                                      IO::font::LEFT, 11 );
                    } else if( canLearn( _playerTeam[ i ].getSpecies( ), currMv, LEARN_TM ) ) {
                        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GREEN;
                        IO::regularFont->setColor( IO::COLOR_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( 36 ), 40, SECOND_LINE, true );
                    } else {
                        IO::regularFont->setColor( IO::RED_IDX, 1 );
                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->printStringC( GET_STRING( 37 ), 40, SECOND_LINE, true,
                                                      IO::font::LEFT, 11 );
                    }
                } else if( p_itemId && ( p_data->m_itemType & 15 ) == ITEM::ITEMTYPE_MEDICINE ) {
                    IO::smallFont->setColor( 0, 0 );
                    IO::smallFont->setColor( IO::GRAY_IDX, 1 );
                    IO::smallFont->setColor( 0, 2 );
                    IO::smallFont->printString( "!", 40, SECOND_LINE - 2, true );
                    char buffer[ 10 ];
                    snprintf( buffer, 8, "%d", _playerTeam[ i ].m_level );
                    IO::smallFont->printStringC( buffer, 48, SECOND_LINE - 1, true );

                    snprintf( buffer, 8, "%3d", _playerTeam[ i ].m_stats.m_curHP );
                    IO::smallFont->printStringC( buffer, 40 + 78 - 44, SECOND_LINE - 1,
                            true );
                    snprintf( buffer, 8, "/%d", _playerTeam[ i ].m_stats.m_maxHP );
                    IO::smallFont->printStringC( buffer, 40 + 76 - 20, SECOND_LINE - 1, true );
                } else {
                    if( p_itemId && p_data->m_itemType == ITEM::ITEMTYPE_EVOLUTION ) {
                        if( _playerTeam[ i ].canEvolve( p_itemId, EVOMETHOD_ITEM ) ) {
                            BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GREEN;
                            IO::regularFont->setColor( IO::COLOR_IDX, 1 );
                            IO::regularFont->setColor( 0, 2 );
                            IO::regularFont->printStringC( GET_STRING( 40 ), 40, SECOND_LINE,
                                    true );
                        } else {
                            IO::regularFont->setColor( IO::RED_IDX, 1 );
                            IO::regularFont->setColor( 0, 2 );
                            IO::regularFont->printStringC( GET_STRING( 41 ), 40, SECOND_LINE, true,
                                    IO::font::LEFT, 11 );
                        }
                    } else {

                        IO::regularFont->setColor( 0, 2 );
                        IO::regularFont->setColor( IO::GRAY_IDX, 1 );

                        if( _playerTeam[ i ].getItem( ) ) {
                            IO::regularFont->printStringC( _teamItemCache[ i ].second.c_str( ),
                                40, SECOND_LINE, true );
                        } else
                            IO::regularFont->printStringC( GET_STRING( 42 ), 40, SECOND_LINE, true );
                    }
                }
            }
        }
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        if( p_data != nullptr ) {
            _lastPkmnItemType = p_data->m_itemType;
        }
        return res;
    }

    void bagUI::drawItemSub( u16 p_itemId, const ITEM::itemData* p_data, u16 p_idx, bool p_selected,
                      bool p_pressed, bool p_clearOnly ) {
        (void) p_pressed;

        if( p_itemId != _itemCache[ p_idx ].first && p_data != nullptr ) {

            // Cache miss, check whether the item can be found somewhere else
            bool gd = false;
            for( u8 j = 0; j < MAX_ITEMS_PER_PAGE; ++j ) {
                if( p_itemId == _itemCache[ j ].first ) {
                    // Found the item at a different place in the bag, probably the items got swapped
                    std::swap(  _itemCache[ p_idx ], _itemCache[ j ] );
                    gd = true;
                    break;
                }
            }
            for( u8 j = 0; j < 6; ++j ) {
                if( p_itemId == _teamItemCache[ j ].first ) {
                    // Found the item in the pkmn cache, probably the item was taken from
                    // the pkmn
                    _itemCache[ p_idx ] = _teamItemCache[ j ];
                    gd = true;
                    break;
                }
            }

            if( !gd ) {
                // Full cache miss
                _itemCache[ p_idx ].first = p_itemId;
                if( p_data->m_itemType != ITEM::ITEMTYPE_TM ) {
                    _itemCache[ p_idx ].second = ITEM::getItemName( p_itemId );
                } else {
                    _itemCache[ p_idx ].second = MOVE::getMoveName( p_data->m_param2 );
                }
            }
        }

        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            for( u8 j = 0; j < 5; ++j ) {
                IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( i ) + j ].palette
                    = SPR_BOX_PAL_SUB;
            }
        }

        for( u8 j = 0; j < 5; ++j ) {
            IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) + j ].isHidden = p_clearOnly
                || !p_itemId;
            if( p_selected ) {
                IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) + j ].palette
                    = SPR_SELECTED_PAL_SUB;
            }
        }

        IO::updateOAM( true );

        u16 x = IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) ].x;
        u16 y = IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( p_idx ) ].y;

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::printRectangle( x, y, 255, y + 20, true, 0 );
        if( p_clearOnly || !p_itemId || p_data == nullptr ) return;

        if( ITEM::getItemChar( p_data->m_itemType )
                != ITEM::getItemChar( ITEM::ITEMTYPE_COLLECTIBLE )
            && toBagType( p_data->m_itemType ) == bag::bagType::ITEMS ) {
            IO::boldFont->printChar( ITEM::getItemChar( p_data->m_itemType ), 236, y + 2, true );
        } else if( p_itemId == SAVE::SAV.getActiveFile( ).m_registeredItem ) {
            IO::boldFont->printChar( 'Y', 238, y + 2, true );
        } else if( p_data->m_itemType == ITEM::ITEMTYPE_TM
                   && MOVE::isFieldMove( p_data->m_param2 ) ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->printChar( 468 + 2, 236, y + 2, true );
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        } else if( p_data->m_itemType == ITEM::ITEMTYPE_TM ) {
            IO::boldFont->printChar( 468 + 2, 236, y + 2, true );
        }

        IO::regularFont->printStringC( _itemCache[ p_idx ].second.c_str( ), x + 6, y + 2, true );

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    void drawTop( u8 p_page ) {
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->printStringC( GET_STRING( 11 + p_page ), 128, 4, false, IO::font::CENTER );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    void bagUI::selectItem( u8 p_idx, std::pair<u16, u16> p_item, const ITEM::itemData* p_data,
                            bool p_pressed ) {
        if( p_idx != _selectedIdx ) {
            // Unselecd old item
            drawTop( _currentPage );
        }
        _selectedIdx = p_idx;

        drawPkmn( p_item.first, p_data );
        drawItemTop( p_item.first, p_data, p_item.second );
        if( p_idx < MAX_ITEMS_PER_PAGE )
            drawItemSub( p_item.first, p_data, p_idx, true, p_pressed, false );
    }

    std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>>
    bagUI::drawBagPage( bag::bagType p_page,
            const std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>>& p_items,
            u8 p_selection ) {
        std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> res;
        _currentPage = p_page;
        _selectedIdx = 255;

        showActiveBag( p_page );
        IO::updateOAM( true );

        drawTop( p_page );
        initColors( );

        auto           pkmnTg = drawPkmn( 0, nullptr );
        if( p_items.size( ) ) {
            for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
                if( i < p_items.size( ) ) {
                    u16 curitem = p_items[ i ].first.first;
                    auto& data = p_items[ i ].second;

                    drawItemSub( curitem, &data, i, i == p_selection, false, false );

                    u16 x = IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].x;
                    u16 y = IO::Oam->oamBuffer[ SPR_ITEM_WINDOW_OAM_SUB( i ) ].y;
                    res.push_back(
                            {IO::inputTarget( x, y, 256, y + 20 ), {curitem, 0}} );
                } else {
                    drawItemSub( 0, nullptr, i, false, false, true );
                }
            }
        } else {
            for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
                drawItemSub( 0, nullptr, i, false, false, true );
            }
            IO::regularFont->printStringC( GET_STRING( 43 ), 182, 89, true, IO::font::CENTER );
            IO::updateOAM( false );
        }
        res.insert( res.end( ), pkmnTg.begin( ), pkmnTg.end( ) );

        return res;
    }

    void bagUI::updateSprite( touchPosition p_touch ) {
        IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].x = p_touch.px - 16;
        IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].y = p_touch.py - 16;
        IO::updateOAM( true );
    }

    bool bagUI::getSprite( u8 p_idx, std::pair<u16, u16> p_item, const ITEM::itemData* p_data ) {
        if( !p_item.first ) return false;

        if( p_idx >= MAX_ITEMS_PER_PAGE ) { // It's a PKMN
            if( !_playerTeam[ p_idx - MAX_ITEMS_PER_PAGE ].getItem( ) ) [[unlikely]] {
                // Something went wrong
                return false;
            }
        }

        if( p_data->m_itemType != ITEM::ITEMTYPE_TM ) {
            IO::loadItemIcon( p_item.first, 0, 0, SPR_TRANSFER_OAM_SUB, SPR_TRANSFER_PAL_SUB,
                              IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].gfxIndex );
        } else {
            MOVE::moveData move = MOVE::getMoveData( p_data->m_param2 );

            IO::loadTMIcon( move.m_type, MOVE::isFieldMove( p_data->m_param2 ), 0, 0,
                    SPR_TRANSFER_OAM_SUB, SPR_TRANSFER_PAL_SUB,
                    IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].gfxIndex );
        }

        selectItem( p_idx, p_item, p_data, true );
        return true;
    }

    void bagUI::dropSprite( u8 p_idx, std::pair<u16, u16> p_item, const ITEM::itemData* p_data ) {
        IO::Oam->oamBuffer[ SPR_TRANSFER_OAM_SUB ].isHidden = true;
        selectItem( p_idx, p_item, p_data );
    }
} // namespace BAG
