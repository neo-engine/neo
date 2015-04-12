/*
Pokémon Emerald 2 Version
------------------------------

file        : bagUI.cpp
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "bag.h"
#include "bagUI.h"
#include "bagViewer.h"
#include "defines.h"
#include "item.h"
#include "berry.h"
#include "fs.h"
#include "messageBox.h"
#include "saveGame.h"
#include "uio.h"

#include <vector>

//Sprites
#include "Back.h"
#include "BagBall1.h"
#include "BagBall2.h"
#include "BagHm1.h"
#include "BagHm2.h"
#include "BagKey1.h"
#include "BagKey2.h"
#include "BagMediine1.h"
#include "BagMediine2.h"
#include "BagBerry1.h"
#include "BagBerry2.h"
#include "Up.h"
#include "Down.h"

#include "bag_bg_lower.h"
#include "bag_bg_upper.h"

namespace BAG {
    const unsigned short * bagPals[ 10 ] = { BagBall1Pal, BagBall2Pal, BagMediine1Pal, BagMediine2Pal, BagHm1Pal, BagHm2Pal, BagBerry1Pal, BagBerry2Pal, BagKey1Pal, BagKey2Pal };
    const unsigned int * bagTiles[ 10 ] = { BagBall1Tiles, BagBall2Tiles, BagMediine1Tiles, BagMediine2Tiles, BagHm1Tiles, BagHm2Tiles, BagBerry1Tiles, BagBerry2Tiles, BagKey1Tiles, BagKey2Tiles };

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
                        26 * p_bagNo /*SAV->m_bagPoses[ p_bagNo ]*/, 3, 32, 32, bagPals[ 2 * p_bagNo + p_active ],
                        bagTiles[ 2 * p_bagNo + p_active ], BackTilesLen, false, false, false, OBJPRIORITY_3, true );
    }

    void initColors( ) {
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
        BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ RED_IDX ] = RED;
        BG_PALETTE[ BLUE_IDX ] = BLUE;
    }

    void bagUI::init( ) {
        IO::vramSetup( );
        videoSetMode( MODE_5_2D/* | DISPLAY_BG2_ACTIVE*/ | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        swiWaitForVBlank( );
        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );

        dmaCopy( bag_bg_upperBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaCopy( bag_bg_upperPal, BG_PALETTE, 256 * 2 );


        IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );

        FS::readPictureData( bgGetGfxPtr( IO::bg2sub ), "nitro:/PICS/", "Clear", 512, 49152, true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "Clear", 512, 49152, true );
        dmaCopy( bag_bg_lowerBitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );
        dmaCopy( bag_bg_lowerPal, BG_PALETTE_SUB, 256 * 2 );

        consoleSelect( &IO::Top );
        consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
        consoleClear( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );
        consoleClear( );

        bgUpdate( );
        initColors( );

        IO::initOAMTable( false );
        //Don't init anything else for the top screen here

        IO::initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, tileCnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 26, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        for( u8 i = 0; i < 6; ++i ) {
            auto acPkmn = FS::SAV->m_pkmnTeam[ i ];
            if( !acPkmn.m_boxdata.m_speciesId )
                break;
            if( acPkmn.m_boxdata.m_individualValues.m_isEgg )
                tileCnt = IO::loadEggIcon( 8, 26 + i * 26, PKMN_SUB + i, PKMN_SUB + i, tileCnt );
            else
                tileCnt = IO::loadPKMNIcon( acPkmn.m_boxdata.m_speciesId, 8, 26 + i * 26, PKMN_SUB + i, PKMN_SUB + i, tileCnt );
        }
        for( u8 i = 0; i < 5; ++i ) {
            tileCnt = IO::loadSprite( BAG_SUB + i, BAG_SUB + i, tileCnt,
                                      26 * i/*FS::SAV->m_bagPoses[ i ]*/, 3, 32, 32, bagPals[ 2 * i ],
                                      bagTiles[ 2 * i ], BackTilesLen, false, false, false, OBJPRIORITY_3, true );
        }

        tileCnt = IO::loadSprite( FWD_ID, FWD_ID, tileCnt,
                                  SCREEN_WIDTH - 60, SCREEN_HEIGHT - 26, 32, 32, DownPal,
                                  DownTiles, DownTilesLen, false, false, false, OBJPRIORITY_3, true );
        tileCnt = IO::loadSprite( BWD_ID, BWD_ID, tileCnt,
                                  SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26, 32, 32, UpPal,
                                  UpTiles, UpTilesLen, false, false, false, OBJPRIORITY_3, true );
        tileCnt = IO::loadSprite( TRANSFER_SUB, TRANSFER_SUB, tileCnt,
                                  SCREEN_WIDTH - 92, SCREEN_HEIGHT - 26, 32, 32, UpPal,
                                  UpTiles, UpTilesLen, false, false, true, OBJPRIORITY_3, true );

        IO::updateOAM( true );
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
            descr = p_item->getDescription( true );

            if( p_item->m_itemType != item::itemType::KEY_ITEM ) {
                std::sprintf( buffer, "x %d", p_count );
                IO::regularFont->printString( buffer, 144, 53, false );
            }

            if( p_item->m_itemType == item::itemType::BERRIES ) {
                berry* curr = static_cast<berry*>( p_item );
                curr->load( );

                IO::regularFont->setColor( RED_IDX, 1 );
                sprintf( buffer, "Güte: %s", ( curr->m_berryGuete == berry::berryGueteType::HARD ) ? "Hart" :
                         ( ( curr->m_berryGuete == berry::berryGueteType::SOFT ) ? "Weich" :
                         ( ( curr->m_berryGuete == berry::berryGueteType::SUPER_HARD ) ? "Steinhart" :
                         ( ( curr->m_berryGuete == berry::berryGueteType::SUPER_SOFT ) ? "Normal" :
                         ( ( curr->m_berryGuete == berry::berryGueteType::VERY_HARD ) ? "Sehr hart" :
                         ( "Sehr weich" ) ) ) ) ) );
                IO::regularFont->printString( buffer, 24, 145, false );
                IO::regularFont->setColor( BLUE_IDX, 1 );
                sprintf( buffer, "Größe:%4.1fcm", curr->m_berrySize / 10.0 );
                IO::regularFont->printString( buffer, 140, 145, false );
                IO::regularFont->setColor( BLACK_IDX, 1 );

                std::string tastes[ 5 ] = { "Scharf", "Trocken", "Süß", "Bitter", "Sauer" };
                u8 poses[ 5 ] = { 18, 66, 124, 150, 194 };
                u8 mx = 0;
                for( u8 i = 0; i < 5; ++i )
                    mx = std::max( mx, curr->m_berryTaste[ i ] );
                for( u8 i = 0; i < 5; ++i ) {
                    if( curr->m_berryTaste[ i ] != mx ) {
                        IO::regularFont->setColor( GRAY_IDX, 1 );
                        IO::regularFont->setColor( WHITE_IDX, 2 );
                    } else {
                        IO::regularFont->setColor( GRAY_IDX, 2 );
                        IO::regularFont->setColor( BLACK_IDX, 1 );
                    }
                    IO::regularFont->printString( tastes[ i ].c_str( ), poses[ i ], 160, false );
                }
                IO::regularFont->setColor( GRAY_IDX, 2 );
                IO::regularFont->setColor( BLACK_IDX, 1 );
            }
        } else {
            auto mv = *( static_cast<TM*>( p_item ) );

            u16 tileCnt = IO::loadTMIcon( AttackList[ mv.m_moveIdx ]->m_moveType,
                                          AttackList[ mv.m_moveIdx ]->m_isFieldAttack, 112, 46, 0, 0, 0, false );

            display = p_item->getDisplayName( true ) + ": " + AttackList[ mv.m_moveIdx ]->m_moveName;
            descr = FS::breakString( AttackList[ mv.m_moveIdx ]->description( ), IO::regularFont, 196 );


            IO::regularFont->printString( "Typ", 33, 145, false );
            tileCnt = IO::loadTypeIcon( AttackList[ mv.m_moveIdx ]->m_moveType, 62, 144, 1, 1, tileCnt, false );

            IO::regularFont->printString( "Kateg.", 100, 145, false );
            IO::loadDamageCategoryIcon( AttackList[ mv.m_moveIdx ]->m_moveHitType, 152, 144, 2, 2, tileCnt, false );

            IO::regularFont->printString( "AP", 190, 145, false );
            std::sprintf( buffer, "%2d", AttackList[ mv.m_moveIdx ]->m_movePP );
            IO::regularFont->printString( buffer, 229 - IO::regularFont->stringWidth( buffer ), 145, false );

            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->printString( "Stärke", 33, 160, false );
            if( AttackList[ mv.m_moveIdx ]->m_moveHitType != move::moveHitTypes::STAT
                &&  AttackList[ mv.m_moveIdx ]->m_moveBasePower > 1 )
                std::sprintf( buffer, "%3d", AttackList[ mv.m_moveIdx ]->m_moveBasePower );
            else
                std::sprintf( buffer, "---" );
            IO::regularFont->printString( buffer, 108 - IO::regularFont->stringWidth( buffer ), 160, false );

            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->printString( "Genauigkeit", 124, 160, false );
            if( AttackList[ mv.m_moveIdx ]->m_moveAccuracy )
                std::sprintf( buffer, "%3d", AttackList[ mv.m_moveIdx ]->m_moveAccuracy );
            else
                std::sprintf( buffer, "---" );
            IO::regularFont->printString( buffer, 229 - IO::regularFont->stringWidth( buffer ), 160, false );
            IO::regularFont->setColor( BLACK_IDX, 1 );
        }

        IO::regularFont->printString( display.c_str( ),
                                      128 - IO::regularFont->stringWidth( display.c_str( ) ) / 2, 26, false );


        IO::regularFont->printString( descr.c_str( ), 33, 83, false, 11 );
        IO::updateOAM( false );
    }

    u8 bagUI::drawPkmn( item* p_item ) {
        u8 pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                break;
            IO::printRectangle( 0, 33 + 26 * i, 128, 33 + 26 * i + 26, true, false, 0 );
            IO::regularFont->setColor( WHITE_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );

            if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg )
                IO::regularFont->printString( "Ei", 45, 38 + 26 * i, true );
            else {
                pkmnCnt++;
                _ranges.push_back( { IO::inputTarget( 0, 33 + 26 * i, 128, 33 + 26 * i + 26 ),
                                   ( 1 << 15 ) | ( i << 12 ) | FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_holdItem } );

                if( p_item->m_itemType == item::itemType::TM_HM ) {
                    u16 currMv = static_cast<TM*>( p_item )->m_moveIdx;
                    if( currMv == FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_moves[ 0 ]
                        || currMv == FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_moves[ 1 ]
                        || currMv == FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_moves[ 2 ]
                        || currMv == FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_moves[ 3 ] ) {
                        IO::regularFont->setColor( BLUE_IDX, 1 );
                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->printString( "Bereits\nerlernt", 45, 33 + 26 * i, true, 11 );
                    } else if( canLearn( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId, currMv, 4 ) ) {
                        BG_PALETTE_SUB[ COLOR_IDX ] = GREEN;
                        IO::regularFont->setColor( COLOR_IDX, 1 );
                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->printString( "Erlernbar", 45, 38 + 26 * i, true );
                    } else {
                        IO::regularFont->setColor( RED_IDX, 1 );
                        IO::regularFont->setColor( BLACK_IDX, 2 );
                        IO::regularFont->printString( "Nicht\nerlernbar", 45, 33 + 26 * i, true, 11 );
                    }
                } else if( p_item->m_itemType == item::itemType::MEDICINE ) {
                    sprintf( buffer, "Level %3d\n%3d/%3d KP", FS::SAV->m_pkmnTeam[ i ].m_Level, FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP, FS::SAV->m_pkmnTeam[ i ].m_stats.m_maxHP );
                    IO::regularFont->printString( buffer, 45, 33 + 26 * i, true, 11 );
                } else {
                    IO::regularFont->printString( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_name, 45, 33 + 26 * i, true );

                    IO::regularFont->setColor( BLACK_IDX, 2 );
                    IO::regularFont->setColor( GRAY_IDX, 1 );

                    if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_holdItem ) {
                        IO::regularFont->printString( ItemList[ FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_holdItem ]->getDisplayName( true ).c_str( ),
                                                      45, 44 + 26 * i, true );
                    } else
                        IO::regularFont->printString( "Kein Item", 45, 44 + 26 * i, true );
                }
            }
        }
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        return pkmnCnt;
    }

    void drawItemSub( item* p_item, u16 p_x, u16 p_y, bool p_selected, bool p_pressed, bool p_clearOnly = false ) {
        IO::printRectangle( p_x, p_y, 255, p_y + 18, true, false, 0 );
        if( p_clearOnly )
            return;
        if( p_item->getItemType( ) != item::itemType::GOODS
            && toBagType( p_item->getItemType( ) ) == bag::bagType::ITEMS ) {
            IO::printChoiceBox( p_x, p_y, p_x + 106 + 13, p_y + 16, 3, 16, p_selected ? RED_IDX : GRAY_IDX, p_pressed );
            IO::printChar( IO::boldFont, 490 - 22 + u16( p_item->getItemType( ) ), p_x + 102 + 2 * p_pressed, p_y - 2 + p_pressed, true );
        } else
            IO::printChoiceBox( p_x, p_y, p_x + 106, p_y + 16, 3, p_selected ? RED_IDX : GRAY_IDX, p_pressed );
        if( p_item->getItemType( ) != item::itemType::TM_HM )
            IO::printString( IO::regularFont, p_item->getDisplayName( true ).c_str( ), p_x + 3 + 2 * p_pressed, p_y + 1 + p_pressed, true );
        else
            IO::printString( IO::regularFont, AttackList[ static_cast<TM*>( p_item )->m_moveIdx ]->m_moveName.c_str( ), p_x + 3 + 2 * p_pressed, p_y + 1 + p_pressed, true );
    }

    std::vector<IO::inputTarget> bagUI::drawBagPage( u8 p_page, u16 p_itemIdx ) {
        std::vector<IO::inputTarget> res;
        _ranges.clear( );

        for( u8 i = 0; i < 5; ++i ) {
            showActiveBag( i, i == p_page );
            //if( i != p_page )
            //    res.push_back( IO::inputTarget( 26 * i + 13, 3 + 13, 13 ) );
        }
        IO::updateOAM( true );

        dmaCopy( bag_bg_upperBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        dmaCopy( bag_bg_upperPal, BG_PALETTE, 256 * 2 );
        dmaCopy( bag_bg_lowerBitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );
        dmaCopy( bag_bg_lowerPal, BG_PALETTE_SUB, 256 * 2 );
        initColors( );

        switch( p_page ) {
            case 0: case 2:{
                IO::regularFont->printString( bagnames[ p_page ].c_str( ), 106, 4, false );
                break;
            }
            case 1: case 3:{
                IO::regularFont->printString( bagnames[ p_page ].c_str( ), 104, 4, false );
                break;
            }
            case 4:{
                IO::regularFont->printString( bagnames[ p_page ].c_str( ), 90, 4, false );
                break;
            }
            default:
                break;
        }

        u8 pkmnCnt = drawPkmn( ItemList[ _bag[ p_page ][ p_itemIdx ].first ] );
        for( u8 i = 0; i < pkmnCnt; ++i )
            res.push_back( _ranges[ _ranges.size( ) - pkmnCnt + i ].first );

        if( !_bag[ p_page ].empty( ) ) {
            drawItemTop( ItemList[ _bag[ p_page ][ p_itemIdx ].first ], _bag[ p_page ][ p_itemIdx ].second );
            drawItemSub( ItemList[ _bag[ p_page ][ p_itemIdx ].first ], 132, 76, true, false );
            res.push_back( IO::inputTarget( 132, 76, 256, 92 ) );
            _ranges.push_back( { res.back( ), _bag[ p_page ][ p_itemIdx ].first } );

            u16 sz = _bag[ p_page ].size( );
            u8 tp = ( sz - 1 ) / 2;

            for( u8 i = 1; i < 5; ++i ) {
                drawItemSub( ItemList[ _bag[ p_page ][ ( p_itemIdx + i ) % sz ].first ], 132, 76 + 18 * i, false, false, i > tp );
                if( i <= tp ) {
                    res.push_back( IO::inputTarget( 132, 76 + 18 * i, 256, 92 + 18 * i ) );
                    _ranges.push_back( { res.back( ), _bag[ p_page ][ ( p_itemIdx + i ) % sz ].first } );
                }

                drawItemSub( ItemList[ _bag[ p_page ][ ( p_itemIdx + sz - i ) % sz ].first ], 132, 76 - 18 * i, false, false, i > ( tp + !( sz % 2 ) ) );
                if( i <= ( tp + !( sz % 2 ) ) ) {
                    res.push_back( IO::inputTarget( 132, 76 - 18 * i, 256, 92 - 18 * i ) );
                    _ranges.push_back( { res.back( ), _bag[ p_page ][ ( p_itemIdx + sz - i ) % sz ].first } );
                }
            }

        } else {
            for( u8 i = 0; i < 5; ++i ) {
                drawItemSub( 0, 132, 76 + 18 * i, false, false, true );
                drawItemSub( 0, 132, 76 - 18 * i, false, false, true );
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            }
            IO::regularFont->printString( "Keine Items", 140, 89, true );
            IO::updateOAM( false );
        }

        return res;
    }

    void bagUI::updateAtHand( touchPosition p_touch, u8 p_oamIdx ) {
        IO::Oam->oamBuffer[ p_oamIdx ].x = p_touch.px - 16;
        IO::Oam->oamBuffer[ p_oamIdx ].y = p_touch.py - 16;
        IO::updateOAM( true );
    }

    u8 bagUI::getSprite( u8 p_rangeIdx, touchPosition p_currPos ) {
        if( _ranges[ p_rangeIdx ].second & ( 1 << 15 ) ) {//It's a PKMN
            if( !( _ranges[ p_rangeIdx ].second % ( 1 << 12 ) ) )
                return 0;
            if( ItemList[ _ranges[ p_rangeIdx ].second % ( 1 << 12 ) ]->m_itemType != item::itemType::TM_HM ) {
                IO::loadItemIcon( ItemList[ _ranges[ p_rangeIdx ].second % ( 1 << 12 ) ]->m_itemName, p_currPos.px, p_currPos.py,
                                  TRANSFER_SUB, TRANSFER_SUB, IO::Oam->oamBuffer[ TRANSFER_SUB ].gfxIndex );
            } else {
                IO::loadTMIcon( AttackList[ static_cast<TM*>( ItemList[ _ranges[ p_rangeIdx ].second % ( 1 << 12 ) ] )->m_moveIdx ]->m_moveType,
                                AttackList[ static_cast<TM*>( ItemList[ _ranges[ p_rangeIdx ].second % ( 1 << 12 ) ] )->m_moveIdx ]->m_isFieldAttack,
                                p_currPos.px, p_currPos.py, TRANSFER_SUB, TRANSFER_SUB, IO::Oam->oamBuffer[ TRANSFER_SUB ].gfxIndex );
            }
        } else { //It's an ordinary item
            if( !_ranges[ p_rangeIdx ].second )
                return 0;
            //So show it
            if( ItemList[ _ranges[ p_rangeIdx ].second ]->m_itemType != item::itemType::TM_HM ) {
                IO::loadItemIcon( ItemList[ _ranges[ p_rangeIdx ].second ]->m_itemName, p_currPos.px, p_currPos.py,
                                  TRANSFER_SUB, TRANSFER_SUB, IO::Oam->oamBuffer[ TRANSFER_SUB ].gfxIndex );
            } else {
                IO::loadTMIcon( AttackList[ static_cast<TM*>( ItemList[ _ranges[ p_rangeIdx ].second ] )->m_moveIdx ]->m_moveType,
                                AttackList[ static_cast<TM*>( ItemList[ _ranges[ p_rangeIdx ].second ] )->m_moveIdx ]->m_isFieldAttack,
                                p_currPos.px, p_currPos.py, TRANSFER_SUB, TRANSFER_SUB, IO::Oam->oamBuffer[ TRANSFER_SUB ].gfxIndex );
            }
            drawItemSub( ItemList[ _ranges[ p_rangeIdx ].second ], _ranges[ p_rangeIdx ].first.m_targetX1,
                         _ranges[ p_rangeIdx ].first.m_targetY1, _ranges[ p_rangeIdx ].first.m_targetY1 == 76, true );
        }
        return TRANSFER_SUB;
    }
    u32 bagUI::acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ) {
        IO::Oam->oamBuffer[ p_oamIdx ].isHidden = true;
        IO::updateOAM( true );

        u32 res = 0;

        //Redraw the startIdx area
        if( _ranges[ p_startIdx ].second & ( 1 << 15 ) ) {//It's a PKMN -> Nothing to do
            res |= ( 2 << 24 );
            if( !( _ranges[ p_dropIdx ].second & ( 1 << 15 ) ) ) {
                res |= ( _ranges[ p_startIdx ].second % ( 1 << 12 ) );
            }
            res |= ( ( ( _ranges[ p_startIdx ].second >> 12 ) % 8 ) << 16 );
        } else { //It's an ordinary item
            drawItemSub( ItemList[ _ranges[ p_startIdx ].second ], _ranges[ p_startIdx ].first.m_targetX1,
                         _ranges[ p_startIdx ].first.m_targetY1, _ranges[ p_startIdx ].first.m_targetY1 == 76, false );

            res |= _ranges[ p_startIdx ].second;
        }

        if( _ranges[ p_dropIdx ].second & ( 1 << 15 ) ) {
            res |= ( 1 << 24 );
            if( _ranges[ p_startIdx ].second & ( 1 << 15 ) ) {
                res |= ( ( _ranges[ p_dropIdx ].second >> 12 ) % 8 );
            } else {
                res |= ( ( ( _ranges[ p_dropIdx ].second >> 12 ) % 8 ) << 16 );
            }
        }

        return res;
    }
}