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
#include "bag.h"
#include "bagViewer.h"
#include "berry.h"
#include "defines.h"
#include "fs.h"
#include "item.h"
#include "messageBox.h"
#include "saveGame.h"
#include "uio.h"

#include <algorithm>
#include <cstdio>
#include <vector>

// Sprites
#include "Back.h"
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
#include "Down.h"
#include "Up.h"

namespace BAG {
    const unsigned short* bagPals[ 10 ]
        = {BagBall1Pal, BagBall2Pal,  BagMediine1Pal, BagMediine2Pal, BagHm1Pal,
           BagHm2Pal,   BagBerry1Pal, BagBerry2Pal,   BagKey1Pal,     BagKey2Pal};
    const unsigned int* bagTiles[ 10 ]
        = {BagBall1Tiles, BagBall2Tiles,  BagMediine1Tiles, BagMediine2Tiles, BagHm1Tiles,
           BagHm2Tiles,   BagBerry1Tiles, BagBerry2Tiles,   BagKey1Tiles,     BagKey2Tiles};

    //#define BACK_ID 0
#define PKMN_SUB 1
#define BAG_SUB 7
#define SORT_SUB 12
    //#define UP_SUB 13
    //#define DOWN_SUB 14
#define TRANSFER_SUB 15

    void showActiveBag( u8 p_bagNo, bool p_active = true ) {
        u16 tileIdx = IO::Oam->oamBuffer[ BAG_SUB + p_bagNo ].gfxIndex;
        IO::loadSprite( BAG_SUB + p_bagNo, BAG_SUB + p_bagNo, tileIdx,
                        26 * p_bagNo /*SAV->getActiveFile( ).m_bagPoses[ p_bagNo ]*/, 3, 32, 32,
                        bagPals[ 2 * p_bagNo + p_active ], bagTiles[ 2 * p_bagNo + p_active ],
                        BackTilesLen, false, false, false, OBJPRIORITY_3, true );
    }

    void initColors( ) {
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ]  = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ]   = RED;
        BG_PALETTE_SUB[ BLUE_IDX ]  = BLUE;
        BG_PALETTE[ WHITE_IDX ]     = WHITE;
        BG_PALETTE[ GRAY_IDX ]      = GRAY;
        BG_PALETTE[ BLACK_IDX ]     = BLACK;
        BG_PALETTE[ RED_IDX ]       = RED;
        BG_PALETTE[ BLUE_IDX ]      = BLUE;
    }

    void bagUI::init( ) {
        IO::vramSetup( );
        swiWaitForVBlank( );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BagUpper" );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "BagLower", 512, 49152,
                             true );

        bgUpdate( );
        initColors( );

        IO::initOAMTable( false );
        // Don't init anything else for the top screen here

        IO::initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 26,
                                  32, 32, BackPal, BackTiles, BackTilesLen, false, false, false,
                                  OBJPRIORITY_3, true );
        tileCnt = drawPkmnIcons( );
        for( u8 i = 0; i < 5; ++i ) {
            tileCnt = IO::loadSprite( BAG_SUB + i, BAG_SUB + i, tileCnt,
                                      26 * i /*SAVE::SAV->getActiveFile( ).m_bagPoses[ i ]*/, 3, 32,
                                      32, bagPals[ 2 * i ], bagTiles[ 2 * i ], BackTilesLen, false,
                                      false, false, OBJPRIORITY_3, true );
        }

        tileCnt = IO::loadSprite( FWD_ID, FWD_ID, tileCnt, SCREEN_WIDTH - 60, SCREEN_HEIGHT - 26,
                                  32, 32, DownPal, DownTiles, DownTilesLen, false, false, false,
                                  OBJPRIORITY_3, true );
        tileCnt = IO::loadSprite( BWD_ID, BWD_ID, tileCnt, SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26,
                                  32, 32, UpPal, UpTiles, UpTilesLen, false, false, false,
                                  OBJPRIORITY_3, true );
        tileCnt = IO::loadSprite( TRANSFER_SUB, TRANSFER_SUB, tileCnt, SCREEN_WIDTH - 92,
                                  SCREEN_HEIGHT - 26, 32, 32, UpPal, UpTiles, UpTilesLen, false,
                                  false, true, OBJPRIORITY_3, true );

        IO::updateOAM( true );
    }

    u16 bagUI::drawPkmnIcons( ) {
        u16 tileCnt = 32;
        for( u8 i = 0; i < 6; ++i ) {
            auto acPkmn = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ];
            if( !acPkmn.m_boxdata.m_speciesId ) break;
            if( acPkmn.isEgg( ) )
                tileCnt = IO::loadEggIcon( 8, 26 + i * 26, PKMN_SUB + i, PKMN_SUB + i, tileCnt );
            else
                tileCnt = IO::loadPKMNIcon( acPkmn.m_boxdata.m_speciesId, 8, 26 + i * 26,
                                            PKMN_SUB + i, PKMN_SUB + i, tileCnt, true,
                                            acPkmn.getForme( ) );
            IO::Oam->oamBuffer[ PKMN_SUB + i ].priority = OBJPRIORITY_3;
        }
        return tileCnt;
    }

    void drawItemTop( item* p_item, u16 p_count ) {
        std::string display;
        std::string descr;
        IO::OamTop->oamBuffer[ 0 ].isHidden = true;
        IO::OamTop->oamBuffer[ 1 ].isHidden = true;
        IO::OamTop->oamBuffer[ 2 ].isHidden = true;

        if( p_item->m_itemType != item::itemType::TM_HM ) {
            IO::loadItemIcon( p_item->m_itemName, 112, 46, 0, 0, 0, false );
            IO::OamTop->oamBuffer[ 1 ].isHidden = true;
            IO::OamTop->oamBuffer[ 2 ].isHidden = true;

            display = p_item->getDisplayName( true );
            descr   = p_item->getDescription( );
            char buffer[ 100 ];

            if( p_item->m_itemType != item::itemType::KEY_ITEM ) {
                snprintf( buffer, 99, "x %d", p_count );
                IO::regularFont->printString( buffer, 144, 53, false );
            }

            if( p_item->m_itemType == item::itemType::BERRIES ) {
                berry* curr = static_cast<berry*>( p_item );
                curr->load( );

                IO::regularFont->setColor( RED_IDX, 1 );
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
                IO::regularFont->printString( buffer, 24, 145, false );
                IO::regularFont->setColor( BLUE_IDX, 1 );
                snprintf( buffer, 99, GET_STRING( 23 ), curr->m_berryData.m_berrySize / 10.0 );
                IO::regularFont->printString( buffer, 140, 145, false );
                IO::regularFont->setColor( BLACK_IDX, 1 );

                u8 poses[ 5 ] = {18, 66, 124, 150, 194};
                u8 mx         = 0;
                for( u8 i = 0; i < 5; ++i )
                    mx = std::max( mx, curr->m_berryData.m_berryTaste[ i ] );
                for( u8 i = 0; i < 5; ++i ) {
                    if( curr->m_berryData.m_berryTaste[ i ] != mx ) {
                        IO::regularFont->setColor( GRAY_IDX, 1 );
                        IO::regularFont->setColor( WHITE_IDX, 2 );
                    } else {
                        IO::regularFont->setColor( GRAY_IDX, 2 );
                        IO::regularFont->setColor( BLACK_IDX, 1 );
                    }
                    IO::regularFont->printString( GET_STRING( 24 + i ), poses[ i ], 160, false );
                }
                IO::regularFont->setColor( GRAY_IDX, 2 );
                IO::regularFont->setColor( BLACK_IDX, 1 );
            }
        } else {
            auto mv = *( static_cast<TM*>( p_item ) );

            u16 tileCnt = IO::loadTMIcon( AttackList[ mv.m_moveIdx ]->m_moveType,
                                          AttackList[ mv.m_moveIdx ]->m_isFieldAttack, 112, 46, 0,
                                          0, 0, false );

            display
                = p_item->getDisplayName( true ) + ": " + AttackList[ mv.m_moveIdx ]->m_moveName;
            descr = FS::breakString( AttackList[ mv.m_moveIdx ]->description( ), IO::regularFont,
                                     196 );

            IO::regularFont->printString( GET_STRING( 29 ), 33, 145, false );
            tileCnt
                = IO::loadTypeIcon( AttackList[ mv.m_moveIdx ]->m_moveType, 62, 144, 1, 1, tileCnt,
                                    false, SAVE::SAV->getActiveFile( ).m_options.m_language );

            IO::regularFont->printString( GET_STRING( 30 ), 100, 145, false );
            IO::loadDamageCategoryIcon( AttackList[ mv.m_moveIdx ]->m_moveHitType, 152, 144, 2, 2,
                                        tileCnt, false );

            IO::regularFont->printString( GET_STRING( 31 ), 190, 145, false );
            char buffer[ 100 ];
            snprintf( buffer, 99, "%2d", AttackList[ mv.m_moveIdx ]->m_movePP );
            IO::regularFont->printString( buffer, 229, 145, false, IO::font::RIGHT );

            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->printString( GET_STRING( 32 ), 33, 160, false );
            if( AttackList[ mv.m_moveIdx ]->m_moveHitType != move::moveHitTypes::STAT
                && AttackList[ mv.m_moveIdx ]->m_moveBasePower > 1 )
                snprintf( buffer, 99, "%3d", AttackList[ mv.m_moveIdx ]->m_moveBasePower );
            else
                snprintf( buffer, 99, "---" );
            IO::regularFont->printString( buffer, 108, 160, false, IO::font::RIGHT );

            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->printString( GET_STRING( 33 ), 124, 160, false );
            if( AttackList[ mv.m_moveIdx ]->m_moveAccuracy )
                snprintf( buffer, 99, "%3d", AttackList[ mv.m_moveIdx ]->m_moveAccuracy );
            else
                std::sprintf( buffer, "---" );
            IO::regularFont->printString( buffer, 229, 160, false, IO::font::RIGHT );
            IO::regularFont->setColor( BLACK_IDX, 1 );
        }

        IO::regularFont->printString( display.c_str( ), 128, 26, false, IO::font::CENTER );

        IO::regularFont->printString( descr.c_str( ), 33, 83, false, IO::font::LEFT, 11 );
        IO::updateOAM( false );
    }

    std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> bagUI::drawPkmn( item* p_item ) {
        std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> res;
        for( u8 i = 0; i < 6; ++i ) {
            if( !SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId ) break;
            IO::printRectangle( 0, 33 + 26 * i, 128, 33 + 26 * i + 26, true, 0 );
            IO::regularFont->setColor( WHITE_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );

            if( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].isEgg( ) ) {
                IO::regularFont->printString( GET_STRING( 34 ), 45, 38 + 26 * i, true );
                res.push_back(
                    {IO::inputTarget( 0, 33 + 26 * i, 128, 33 + 26 * i + 26 ), {0, true}} );
            } else {
                res.push_back(
                    {IO::inputTarget( 0, 33 + 26 * i, 128, 33 + 26 * i + 26 ),
                     {SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].getItem( ), true}} );
                if( p_item && p_item->m_itemType == item::itemType::TM_HM ) {
                    u16 currMv = static_cast<TM*>( p_item )->m_moveIdx;
                    if( currMv == SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_moves[ 0 ]
                        || currMv
                               == SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_moves[ 1 ]
                        || currMv
                               == SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_moves[ 2 ]
                        || currMv
                               == SAVE::SAV->getActiveFile( )
                                      .m_pkmnTeam[ i ]
                                      .m_boxdata.m_moves[ 3 ] ) {
                        IO::regularFont->setColor( BLUE_IDX, 1 );
                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->printString( GET_STRING( 35 ), 40, 33 + 26 * i, true,
                                                      IO::font::LEFT, 11 );
                    } else if( canLearn( SAVE::SAV->getActiveFile( )
                                             .m_pkmnTeam[ i ]
                                             .m_boxdata.m_speciesId,
                                         currMv, 4 ) ) {
                        BG_PALETTE_SUB[ COLOR_IDX ] = GREEN;
                        IO::regularFont->setColor( COLOR_IDX, 1 );
                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->printString( GET_STRING( 36 ), 40, 38 + 26 * i, true );
                    } else {
                        IO::regularFont->setColor( RED_IDX, 1 );
                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->printString( GET_STRING( 37 ), 40, 33 + 26 * i, true,
                                                      IO::font::LEFT, 11 );
                    }
                } else if( p_item && p_item->m_itemType == item::itemType::MEDICINE ) {
                    char buffer[ 100 ];
                    if( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_stats.m_acHP ) {
                        snprintf( buffer, 99, GET_STRING( 38 ),
                                  SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_level,
                                  SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_stats.m_acHP,
                                  SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_stats.m_maxHP );
                    } else {
                        snprintf( buffer, 99, GET_STRING( 39 ),
                                  SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_level );
                    }
                    IO::regularFont->printString( buffer, 40, 33 + 26 * i, true, IO::font::LEFT,
                                                  11 );
                } else {
                    if( p_item && p_item->getEffectType( ) == item::itemEffectType::USE_ON_PKMN ) {
                        if( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].canEvolve(
                                p_item->getItemId( ), 3 ) ) {
                            BG_PALETTE_SUB[ COLOR_IDX ] = GREEN;
                            IO::regularFont->setColor( COLOR_IDX, 1 );
                            IO::regularFont->setColor( BLACK_IDX, 2 );
                            IO::regularFont->printString( GET_STRING( 40 ), 40, 38 + 26 * i, true );
                        } else {
                            IO::regularFont->setColor( RED_IDX, 1 );
                            IO::regularFont->setColor( BLACK_IDX, 2 );
                            IO::regularFont->printString( GET_STRING( 41 ), 40, 33 + 26 * i, true,
                                                          IO::font::LEFT, 11 );
                        }
                    } else {
                        IO::regularFont->printString(
                            SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_name, 45,
                            33 + 26 * i, true );

                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->setColor( GRAY_IDX, 1 );

                        if( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].getItem( ) ) {
                            IO::regularFont->printString( ItemList[ SAVE::SAV->getActiveFile( )
                                                                    .m_pkmnTeam[ i ].getItem( ) ]
                                                              ->getDisplayName( true )
                                                              .c_str( ),
                                                          40, 44 + 26 * i, true );
                        } else
                            IO::regularFont->printString( GET_STRING( 42 ), 40, 44 + 26 * i, true );
                    }
                }
            }
        }
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        return res;
    }

    void drawItemSub( item* p_item, u16 p_x, u16 p_y, bool p_selected, bool p_pressed,
                      bool p_clearOnly = false ) {
        IO::printRectangle( p_x, p_y, 255, p_y + 18, true, 0 );
        if( p_clearOnly || !p_item ) return;
        if( p_item->m_itemType != item::itemType::GOODS
            && toBagType( p_item->m_itemType ) == bag::bagType::ITEMS ) {
            IO::printChoiceBox( p_x, p_y, p_x + 106 + 13, p_y + 16, 3, 16,
                                p_selected ? RED_IDX : GRAY_IDX, p_pressed );
            IO::boldFont->printChar( 490 - 22 + u16( p_item->m_itemType ),
                                     p_x + 102 + 2 * p_pressed, p_y - 2 + p_pressed, true );
        } else if( p_item->getItemId( ) == SAVE::SAV->getActiveFile( ).m_registeredItem ) {
            IO::printChoiceBox( p_x, p_y, p_x + 106 + 13, p_y + 16, 3, 16,
                                p_selected ? RED_IDX : GRAY_IDX, p_pressed );
            IO::boldFont->printChar( 'Y', p_x + 106 + 2 * p_pressed, p_y - 2 + p_pressed, true );
        } else if( p_item->m_itemType == item::itemType::TM_HM
                   && AttackList[ static_cast<TM*>( p_item )->m_moveIdx ]->m_isFieldAttack ) {
            IO::printChoiceBox( p_x, p_y, p_x + 106 + 13, p_y + 16, 3, 16,
                                p_selected ? RED_IDX : GRAY_IDX, p_pressed );
            IO::boldFont->setColor( BLUE_IDX, 2 );
            IO::boldFont->printChar( 490 - 22 + u16( p_item->m_itemType ),
                                     p_x + 102 + 2 * p_pressed, p_y - 2 + p_pressed, true );
            IO::boldFont->setColor( WHITE_IDX, 2 );
        } else if( p_item->m_itemType == item::itemType::TM_HM ) {
            IO::printChoiceBox( p_x, p_y, p_x + 106 + 13, p_y + 16, 3, 16,
                                p_selected ? RED_IDX : GRAY_IDX, p_pressed );
            IO::boldFont->printChar( 490 - 22 + u16( p_item->m_itemType ),
                                     p_x + 102 + 2 * p_pressed, p_y - 2 + p_pressed, true );
        } else
            IO::printChoiceBox( p_x, p_y, p_x + 106, p_y + 16, 3, p_selected ? RED_IDX : GRAY_IDX,
                                p_pressed );
        if( p_item->m_itemType != item::itemType::TM_HM )
            IO::regularFont->printString( p_item->getDisplayName( true ).c_str( ),
                                          p_x + 3 + 2 * p_pressed, p_y + 1 + p_pressed, true );
        else
            IO::regularFont->printString(
                AttackList[ static_cast<TM*>( p_item )->m_moveIdx ]->m_moveName.c_str( ),
                p_x + 3 + 2 * p_pressed, p_y + 1 + p_pressed, true );
    }

    void drawTop( u8 p_page ) {
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "BagUpper" );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        initColors( );
        IO::regularFont->printString( GET_STRING( 11 + p_page ), 128, 4, false, IO::font::CENTER );
    }

    void bagUI::selectItem( u8 p_idx, std::pair<u16, u16> p_item, bool p_pressed ) {
        drawPkmn( ItemList[ p_item.first ] );
        drawItemTop( ItemList[ p_item.first ], p_item.second );
        if( p_idx < MAX_ITEMS_PER_PAGE )
            drawItemSub( ItemList[ p_item.first ], 132, 4 + p_idx * 18, true, p_pressed, false );
    }
    void bagUI::unselectItem( bag::bagType p_page, u8 p_idx, u16 p_item ) {
        drawPkmn( 0 );
        drawTop( p_page );
        if( p_idx < MAX_ITEMS_PER_PAGE )
            drawItemSub( ItemList[ p_item ], 132, 4 + p_idx * 18, false, false, false );
    }

    std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>>
    bagUI::drawBagPage( bag::bagType p_page, u16 p_firstDisplayedItem ) {
        std::vector<std::pair<IO::inputTarget, bagUI::targetInfo>> res;

        for( u8 i = 0; i < 5; ++i ) {
            showActiveBag( i, i == p_page );
            // if( i != p_page )
            //    res.push_back( IO::inputTarget( 26 * i + 13, 3 + 13, 13 ) );
        }
        IO::updateOAM( true );

        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "BagLower", 512, 49152,
                             true );
        drawTop( p_page );
        initColors( );

        auto pkmnTg = drawPkmn( 0 );
        if( !SAVE::SAV->getActiveFile( ).m_bag.empty( p_page ) ) {
            u16 sz = SAVE::SAV->getActiveFile( ).m_bag.size( p_page );
            for( u8 i = 0; i < 9; ++i ) {
                drawItemSub( ItemList[ SAVE::SAV->getActiveFile( )
                                           .m_bag( p_page, ( p_firstDisplayedItem + i ) % sz )
                                           .first ],
                             132, 4 + i * 18, false, false, i >= sz );

                if( i < sz )
                    res.push_back( {IO::inputTarget( 132, 4 + i * 18, 256, 20 + i * 18 ),
                                    {SAVE::SAV->getActiveFile( )
                                         .m_bag( p_page, ( p_firstDisplayedItem + i ) % sz )
                                         .first,
                                     false}} );
            }
        } else {
            for( u8 i = 0; i < 5; ++i ) {
                drawItemSub( 0, 132, 76 + 18 * i, false, false, true );
                drawItemSub( 0, 132, 76 - 18 * i, false, false, true );
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            }
            IO::regularFont->printString( GET_STRING( 43 ), 182, 89, true, IO::font::CENTER );
            IO::updateOAM( false );
        }
        res.insert( res.end( ), pkmnTg.begin( ), pkmnTg.end( ) );

        return res;
    }

    void bagUI::updateSprite( touchPosition p_touch ) {
        IO::Oam->oamBuffer[ TRANSFER_SUB ].x = p_touch.px - 16;
        IO::Oam->oamBuffer[ TRANSFER_SUB ].y = p_touch.py - 16;
        IO::updateOAM( true );
    }

    bool bagUI::getSprite( u8 p_idx, std::pair<u16, u16> p_item ) {
        if( !p_item.first ) return false;

        if( p_idx >= MAX_ITEMS_PER_PAGE ) { // It's a PKMN
            if( !SAVE::SAV->getActiveFile( )
                     .m_pkmnTeam[ p_idx - MAX_ITEMS_PER_PAGE ].getItem( ) ) // Something went wrong
                return false;
        }

        if( ItemList[ p_item.first ]->m_itemType != item::itemType::TM_HM ) {
            IO::loadItemIcon( ItemList[ p_item.first ]->m_itemName, 0, 0, TRANSFER_SUB,
                              TRANSFER_SUB, IO::Oam->oamBuffer[ TRANSFER_SUB ].gfxIndex );
        } else {
            IO::loadTMIcon(
                AttackList[ static_cast<TM*>( ItemList[ p_item.first ] )->m_moveIdx ]->m_moveType,
                AttackList[ static_cast<TM*>( ItemList[ p_item.first ] )->m_moveIdx ]
                    ->m_isFieldAttack,
                0, 0, TRANSFER_SUB, TRANSFER_SUB, IO::Oam->oamBuffer[ TRANSFER_SUB ].gfxIndex );
        }

        selectItem( p_idx, p_item, true );
        return true;
    }

    void bagUI::dropSprite( bag::bagType p_page, u8 p_idx, std::pair<u16, u16> p_item ) {
        IO::Oam->oamBuffer[ TRANSFER_SUB ].isHidden = true;
        IO::updateOAM( true );

        unselectItem( p_page, p_idx, p_item.first );
        selectItem( p_idx, p_item );
    }
} // namespace BAG
