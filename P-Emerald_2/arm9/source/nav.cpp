/*
Pokémon neo
------------------------------

file        : nav.cpp
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

#include <map>

#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "keyboard.h"
#include "messageBox.h"
#include "nav.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"
#include "sound.h"

#include "berry.h"
#include "item.h"
#include "pokemon.h"
#include "moveNames.h"
#include "itemNames.h"

#include "bagUI.h"
#include "bagViewer.h"

#include "statusScreen.h"
#include "statusScreenUI.h"

#include "boxUI.h"
#include "boxViewer.h"

#include "dex.h"
#include "dexUI.h"

#include "mapDrawer.h"
#include "mapObject.h"
#include "mapSlice.h"

#include "battle.h"
#include "battleTrainer.h"

#include "Border.h"

#include "bg00.h"

#include "Back.h"
#include "BagSpr.h"

#include "icon_bag_1.h"
#include "icon_bag_2.h"
#include "icon_dex_1.h"
#include "icon_dex_2.h"
#include "icon_id_1.h"
#include "icon_id_2.h"
#include "icon_party_1.h"
#include "icon_party_2.h"
#include "icon_save_1.h"
#include "icon_save_2.h"
#include "icon_settings_1.h"
#include "icon_settings_2.h"

namespace NAV {
    state STATE = HOME;
    bool  ALLOW_INIT = true;
    u8    CURRENT_BANK = 0;
    u16   CURRENT_MAP = 0;

    void drawMapMug( );
    void drawBorder( );

    u8   mainSpritePos[ 12 ][ 2 ]
        = {{0, 0},     {12, 42},   {244, 26}, {244, 56}, {244, 86}, {12, 162},
           {244, 116}, {244, 146}, {0, 0},    {12, 72},  {12, 102}, {12, 132}};
    unsigned int       NAV_DATA[ 12288 ]   = {0};
    unsigned short     NAV_DATA_PAL[ 256 ] = {0};
    backgroundSet BGs[ MAXBG ] = {{"Magnetizing Magnemite", bg00Bitmap, bg00Pal, false, true},
                                       {"Executing Exeggcute", NAV_DATA, NAV_DATA_PAL, true, true},
                                       {"Fighting Torchic", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Reborn Ho-Oh", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Raging Gyarados", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Mystic Guardevoir", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Sleeping Eevee", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Waiting Suicune", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Awakening Xerneas", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Awakening Yveltal", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Fighting Groudon", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Fighting Kyogre", NAV_DATA, NAV_DATA_PAL, true, false},
                                       {"Working Klink", NAV_DATA, NAV_DATA_PAL, true, true}};

#define POS mainSpritePos

    std::map<state, state> backTransition = {{MAP_MUG, HOME},
                                                       {MAP_BIG, MAP},
                                                       {MAP, HOME},

                                                       {HOME, HOME}};

    void home( ) {
        STATE = HOME;
    }

    void drawBorder( ) {
        auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg2 ) : bgGetGfxPtr( IO::bg2sub );
        auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

        dmaCopy( BorderBitmap, ptr, 256 * 192 );
        dmaCopy( BorderPal + 192, pal + 192, 64 );

        if( ANIMATE_MAP ) {
            /*  pal[ WHITE_IDX ] = WHITE;
              pal[ GRAY_IDX ]  = GRAY;
              pal[ BLACK_IDX ] = BLACK;
              regularFont->setColor( WHITE_IDX, 1 );
              regularFont->setColor( GRAY_IDX, 2 );
              regularFont->setColor( 0, 0 ); */
            IO::boldFont->setColor( 0, 1 );
            IO::boldFont->setColor( WHITE_IDX, 2 );

            IO::boldFont->printString( (FS::getLocation( CURRENT_MAP, CURRENT_LANGUAGE ) ).c_str( ), 7,
                    4, !SCREENS_SWAPPED );
        }

        DRAW_TIME = true;
    }
    void updateItems( ) {
        bool b = false;
        for( u8 i = 0; i < 3; ++i ) {
            u16 curitm
                = SAVE::SAV.getActiveFile( )
                      .m_lstUsedItems[ ( SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx + 4 - i )
                                       % 5 ];
            if( !b && SAVE::SAV.getActiveFile( ).m_registeredItem
                && ( !curitm || i == 2
                     || curitm == SAVE::SAV.getActiveFile( ).m_registeredItem ) ) {
                curitm = SAVE::SAV.getActiveFile( ).m_registeredItem;
                b      = true;
            }
            if( curitm )
                IO::loadItemIcon( curitm, POS[ ITM( i ) ][ 0 ] - 16,
                                  POS[ ITM( i ) ][ 1 ] - 16, ITM( i ), ITM( i ),
                                  IO::Oam->oamBuffer[ ITM_BACK + 2 ].gfxIndex + 32 * ( i + 1 ) );
        }
        IO::updateOAM( true );
    }

    void initMainSprites( bool p_showBack = false ) {
        IO::initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28,
                                  32, 32, BackPal, BackTiles, BackTilesLen, false, false,
                                  !p_showBack, OBJPRIORITY_0, true );

        // Main menu sprites
        tileCnt = IO::loadSprite( A_ID, A_ID, tileCnt,
                                  POS[ SAVE_ID ][ 0 ] - 16, POS[ SAVE_ID ][ 1 ] - 16, 32, 32,
                                  icon_save_1Pal, icon_save_1Tiles, icon_save_1TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( FWD_ID, FWD_ID, tileCnt,
                                  POS[ BAG_ID ][ 0 ] - 16, POS[ BAG_ID ][ 1 ] - 16, 32, 32,
                                  icon_bag_1Pal, icon_bag_1Tiles, icon_bag_1TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( BWD_ID, BWD_ID, tileCnt,
                                  POS[ PKMN_ID ][ 0 ] - 16, POS[ PKMN_ID ][ 1 ] - 16, 32, 32,
                                  icon_party_1Pal, icon_party_1Tiles, icon_party_1TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( NAV_ID, NAV_ID, tileCnt,
                                  POS[ NAV_ID ][ 0 ] - 16, POS[ NAV_ID ][ 1 ] - 16, 32, 32,
                                  icon_settings_1Pal, icon_settings_1Tiles, icon_settings_1TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( ID_ID, ID_ID, tileCnt,
                                  POS[ ID_ID ][ 0 ] - 16, POS[ ID_ID ][ 1 ] - 16, 32, 32,
                                  icon_id_1Pal, icon_id_1Tiles, icon_id_1TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( DEX_ID, DEX_ID, tileCnt,
                                  POS[ DEX_ID ][ 0 ] - 16, POS[ DEX_ID ][ 1 ] - 16, 32, 32,
                                  icon_dex_1Pal, icon_dex_1Tiles, icon_dex_1TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        // tileCnt = IO::loadItemIcon( "----", POS[ OPTS_ID ][ 0 ] - 16, POS[ OPTS_ID ][ 1 ] - 16,
        // 67,
        //                            OPTS_ID, tileCnt );

        IO::loadSprite( PKMN_ID, ITM_BACK, tileCnt, POS[ PKMN_ID ][ 0 ] - 16,
                        POS[ PKMN_ID ][ 1 ] - 16, 32, 32, BagSprPal, BagSprTiles, BagSprTilesLen,
                        false, false, false, OBJPRIORITY_2, true );
        IO::loadSprite( OPTS_ID, ITM_BACK, tileCnt, POS[ OPTS_ID ][ 0 ] - 16,
                        POS[ OPTS_ID ][ 1 ] - 16, 32, 32, BagSprPal, BagSprTiles, BagSprTilesLen,
                        false, false, false, OBJPRIORITY_2, true );
        IO::loadSprite( DEX_ID, ITM_BACK, tileCnt, POS[ DEX_ID ][ 0 ] - 16, POS[ DEX_ID ][ 1 ] - 16,
                        32, 32, BagSprPal, BagSprTiles, BagSprTilesLen, false, false, false,
                        OBJPRIORITY_2, true );
        IO::loadSprite( NAV_ID, ITM_BACK, tileCnt, POS[ NAV_ID ][ 0 ] - 16, POS[ NAV_ID ][ 1 ] - 16,
                        32, 32, BagSprPal, BagSprTiles, BagSprTilesLen, false, false, false,
                        OBJPRIORITY_2, true );
        IO::loadSprite( ID_ID, ITM_BACK, tileCnt, POS[ ID_ID ][ 0 ] - 16, POS[ ID_ID ][ 1 ] - 16,
                        32, 32, BagSprPal, BagSprTiles, BagSprTilesLen, false, false, false,
                        OBJPRIORITY_2, true );
        IO::loadSprite( SAVE_ID, ITM_BACK, tileCnt, POS[ SAVE_ID ][ 0 ] - 16,
                        POS[ SAVE_ID ][ 1 ] - 16, 32, 32, BagSprPal, BagSprTiles, BagSprTilesLen,
                        false, false, false, OBJPRIORITY_2, true );
        IO::loadSprite( BAG_ID, ITM_BACK, tileCnt, POS[ BAG_ID ][ 0 ] - 16, POS[ BAG_ID ][ 1 ] - 16,
                        32, 32, BagSprPal, BagSprTiles, BagSprTilesLen, false, false, false,
                        OBJPRIORITY_2, true );

        IO::loadSprite( ITM_BACK, ITM_BACK, tileCnt, POS[ ITM( 0 ) ][ 0 ] - 16,
                        POS[ ITM( 0 ) ][ 1 ] - 16, 32, 32, BagSprPal, BagSprTiles, BagSprTilesLen,
                        false, false, false, OBJPRIORITY_2, true );
        IO::loadSprite( ITM_BACK + 1, ITM_BACK, tileCnt, POS[ ITM( 1 ) ][ 0 ] - 16,
                        POS[ ITM( 1 ) ][ 1 ] - 16, 32, 32, BagSprPal, BagSprTiles, BagSprTilesLen,
                        false, false, false, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( ITM_BACK + 2, ITM_BACK, tileCnt, POS[ ITM( 2 ) ][ 0 ] - 16,
                                  POS[ ITM( 2 ) ][ 1 ] - 16, 32, 32, BagSprPal, BagSprTiles,
                                  BagSprTilesLen, false, false, false, OBJPRIORITY_2, true );
        updateItems( );
        IO::updateOAM( true );
    }

    void drawMapMug( ) {
        auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        char buffer[ 100 ];
        snprintf( buffer, 99, "%03hu/%hu_%hhu", CURRENT_BANK / FS::ITEMS_PER_DIR,
                CURRENT_BANK, getCurrentDaytime( ) % 4 );
        FS::readPictureData( ptr, "nitro:/PICS/MAP_MUG/", buffer, 512, 49152, !SCREENS_SWAPPED );
        drawBorder( );
    }

    void draw( bool p_initMainSrites, u8 p_newIdx ) {
        if( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx == p_newIdx )
            return;
        else if( p_newIdx == u8( 255 ) )
            p_newIdx = SAVE::SAV.getActiveFile( ).m_options.m_bgIdx;

        auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

        if( STATE != MAP_MUG ) {
            if( !BGs[ p_newIdx ].m_loadFromRom ) {
                dmaCopy( BGs[ p_newIdx ].m_mainMenu, ptr, 256 * 192 );
                dmaCopy( BGs[ p_newIdx ].m_mainMenuPal, pal, 192 * 2 );
                SAVE::SAV.getActiveFile( ).m_options.m_bgIdx = p_newIdx;
            } else if( !FS::readNavScreenData( ptr, BGs[ p_newIdx ].m_name.c_str( ), p_newIdx ) ) {
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

    void showNewMap( u8 p_newMap ) {
        if( p_newMap == CURRENT_BANK ) return;
        CURRENT_BANK = p_newMap;
        CURRENT_MAP  = MAP::curMap->getCurrentLocationId( );
        char buffer[ 100 ];
        snprintf( buffer, 99, "%03hhu/%hu_%hhu", CURRENT_BANK / FS::ITEMS_PER_DIR, CURRENT_BANK,
                getCurrentDaytime( ) % 4 );
        if( FS::exists( "nitro:/PICS/MAP_MUG/", buffer ) )
            STATE = MAP_MUG;
        else if( STATE == MAP_MUG )
            STATE = HOME;

        IO::Oam->oamBuffer[ BACK_ID ].isHidden = ( STATE == HOME );
        IO::updateOAM( true );

        draw( false );
    }

    void updateMap( u16 p_newMap ) {
        if( p_newMap != CURRENT_MAP ) {
            CURRENT_MAP = p_newMap;
            draw( false );
        }
    }

    void handleInput( touchPosition p_touch, const char* p_path ) {
        touchPosition& touch = p_touch;

        if( held & KEY_Y ) {
            IO::waitForKeysUp( KEY_Y );
            if( SAVE::SAV.getActiveFile( ).m_registeredItem ) {
                if( ITEM::isUsable( SAVE::SAV.getActiveFile( ).m_registeredItem ) ) {
                    ITEM::use( SAVE::SAV.getActiveFile( ).m_registeredItem );
                    updateItems( );
                } else {
                    IO::messageBox( GET_STRING( 58 ), GET_STRING( 91 ) );
                    draw( true );
                }
            } else {
                IO::messageBox( GET_STRING( 98 ), GET_STRING( 91 ) );
                draw( true );
            }
            swiWaitForVBlank( );
            scanKeys( );
            return;
        }

        bool itmsn = false;
        for( u8 i = 0; i < 3; ++i ) {
            u16 curitm
                = SAVE::SAV.getActiveFile( )
                      .m_lstUsedItems[ ( SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx + 4 - i )
                                       % 5 ];
            if( !itmsn && SAVE::SAV.getActiveFile( ).m_registeredItem
                && ( !curitm || i == 2
                     || curitm == SAVE::SAV.getActiveFile( ).m_registeredItem ) ) {
                curitm = SAVE::SAV.getActiveFile( ).m_registeredItem;
                itmsn  = true;
            }
            /*
            if( GET_AND_WAIT_C( POS[ ITM( i ) ][ 0 ], POS[ ITM( i ) ][ 1 ], 14 ) ) {
                if( curitm ) {
                    if( u16( -1 )
                        == SAVE::SAV.getActiveFile( ).m_bag.count(
                               BAG::toBagType( ItemList[ curitm ]->m_itemType ), curitm ) ) {
                        IO::yesNoBox yn( GET_STRING( 91 ) );
                        char         buffer[ 100 ];
                        snprintf( buffer, 99, GET_STRING( 96 ),
                                  ItemList[ curitm ]->getDisplayName( true ).c_str( ) );
                        if( yn.getResult( buffer ) ) {
                            for( u8 j = i; j < 4; ++j ) {
                                SAVE::SAV.getActiveFile( )
                                    .m_lstUsedItems[ ( SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx
                                                       + 4 - j )
                                                     % 5 ]
                                    = SAVE::SAV.getActiveFile( ).m_lstUsedItems
                                          [ ( SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx + 3
                                              - j )
                                            % 5 ];
                            }
                            SAVE::SAV.getActiveFile( )
                                .m_lstUsedItems[ SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx ]
                                = 0;
                        }
                        draw( true );
                    } else if( ItemList[ curitm ]->useable( ) ) {
                        if( ItemList[ curitm ]->use( true ) ) IO::messageBox( "", 0, false );
                        ItemList[ curitm ]->use( );
                        if( ItemList[ curitm ]->m_itemType != item::KEY_ITEM )
                            SAVE::SAV.getActiveFile( ).m_bag.erase(
                                BAG::toBagType( ItemList[ curitm ]->m_itemType ), curitm, 1 );
                        draw( true );
                    } else {
                        IO::messageBox( GET_STRING( 58 ), GET_STRING( 91 ) );
                        draw( true );
                    }
                } else {
                    IO::messageBox( GET_STRING( 97 ), GET_STRING( 91 ) );
                    draw( true );
                }
                return;
            }
            */
        }

        if( STATE != HOME && GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
            STATE = backTransition[ STATE ];
            draw( false );
            if( STATE == HOME ) {
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::updateOAM( true );
            }
        } else {
            // StartBag
            if( GET_AND_WAIT_C( POS[ BAG_ID ][ 0 ], POS[ BAG_ID ][ 1 ], 16 ) ) {
                BAG::bagViewer bv;
                ANIMATE_MAP = false;
                UPDATE_TIME = false;
                SOUND::dimVolume( );

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                u16 res = bv.run( );

                FADE_TOP_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::clearScreenConsole( true, true );
                STATE      = HOME;
                UPDATE_TIME = true;
                MAP::curMap->draw( );
                ANIMATE_MAP = true;
                SOUND::restoreVolume( );
                draw( true );
                updateItems( );
                if( res ) {
                    ITEM::use( res );
                    updateItems( );
                    draw( true );
                }
            } else if( SAVE::SAV.getActiveFile( )
                           .m_pkmnTeam[ 0 ]
                           .m_boxdata.m_speciesId // StartPkmn
                       && ( GET_AND_WAIT_C( POS[ PKMN_ID ][ 0 ], POS[ PKMN_ID ][ 1 ], 16 ) ) ) {
                ANIMATE_MAP = false;
                SOUND::dimVolume( );
                STATE      = HOME;
                IO::initOAMTable( true );
                videoSetMode( MODE_5_2D );
                STS::statusScreen sts( 0 );

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                auto res = sts.run( );

                FADE_TOP_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::clearScreenConsole( true, true );
                ANIMATE_MAP = true;
                SOUND::restoreVolume( );
                draw( true );
                MAP::curMap->draw( );

                if( res ) MOVE::use( res & 32767, res >> 15 );
            } else if( GET_AND_WAIT_C( POS[ DEX_ID ][ 0 ], POS[ DEX_ID ][ 1 ], 16 ) ) {
                ANIMATE_MAP = false;
                SOUND::dimVolume( );
                STATE      = HOME;

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                DEX::dex( DEX::dex::SHOW_CAUGHT, MAX_PKMN )
                    .run( SAVE::SAV.getActiveFile( ).m_lstDex );

                FADE_TOP_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::clearScreenConsole( true, true );
                ANIMATE_MAP = true;
                SOUND::restoreVolume( );
                draw( true );
                MAP::curMap->draw( );
            } else if( GET_AND_WAIT_C( POS[ OPTS_ID ][ 0 ], POS[ OPTS_ID ][ 1 ], 16 ) ) {

            } else if( GET_AND_WAIT_C( POS[ ID_ID ][ 0 ], POS[ ID_ID ][ 1 ], 16 ) ) {

                STATE = HOME;

                const char* someText[ 12 ]
                    = {"PKMN Spawn",   "Item Spawn", "1 Item Test",  "Dbl Battle",
                       "Sgl Battle",   "Chg NavScrn",  "View Boxes A", "View Boxes B",
                       "Hoenn Badges", "Kanto Badges", "Keyboard", "Plate Spawn" };
                IO::choiceBox test( 12, &someText[ 0 ], 0, false );
                int           res = test.getResult( "Tokens of god-being..." );
                draw( );
                switch( res ) {
                case 0: {
                    memset( SAVE::SAV.getActiveFile( ).m_pkmnTeam, 0,
                            sizeof( SAVE::SAV.getActiveFile( ).m_pkmnTeam ) );
                    std::vector<u16> tmp = { 201, 493, 521, 649, u16( 1 + rand( ) % MAX_PKMN ),
                                             MAX_PKMN };
                    for( int i = 0; i < 6; ++i ) {
                        pokemon&         a   = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];

                        a = pokemon( tmp[ i ], 50, !i ? ( rand( ) % 28 ) : 0, 0, i );

                        a.m_stats.m_acHP *= i / 5.0;
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
                            a.m_boxdata.m_heldItem      = 1 + rand( ) % 400;
                        }

                        for( u16 j = 1; j <= MAX_PKMN; ++j )
                            SAVE::SAV.m_caughtPkmn[ ( j ) / 8 ] |= ( 1 << ( j % 8 ) );
                    }
                    /*
                                        for( u16 j : {493, 649, 648, 647, 487, 492, 641, 642,
                       646,
                       645, 643, 644} ) {
                                            auto a       = pokemon( j, 50, 0, j ).m_boxdata;
                                            a.m_gotPlace = j;
                                            SAVE::SAV.storePkmn( a );
                                            if( a.isShiny( ) ) {
                                              IO::messageBox( "YAAAY" );
                                              s8 idx = SAVE::SAV.getCurrentBox(
                       )->getFirstFreeSpot( );
                                              if( idx == -1 && !( *SAVE::SAV.getCurrentBox( )
                       )[ 17
                       ].isShiny( ) )
                                              IO::messageBox( "Lost :(" );
                                              else if( !( *SAVE::SAV.getCurrentBox( ) )[ idx -
                       1
                       ].isShiny( ) )
                                              IO::messageBox( "Lost :(" );
                                              break;
                                              }
                                        }*/

                    SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 0 ] = M_SURF;
                    SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 1 ]
                        = M_WATERFALL;
                    SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 0 ]
                        = M_ROCK_CLIMB;
                    SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 3 ].m_boxdata.m_moves[ 0 ]
                        = M_SWEET_SCENT;

                    swiWaitForVBlank( );
                    break;
                }
                case 1:
                    for( u16 j = 1; j < MAX_ITEMS; ++j ) {
                        auto c = ITEM::getItemData( j );
                        if( c.m_itemType )
                            SAVE::SAV.getActiveFile( ).m_bag.insert(
                                BAG::toBagType( c.m_itemType ), j, 1 );
                    }
                    break;
                case 2: {
                    // item* curr = ItemList[ rand( ) % 638 ];
                    // while( curr->m_itemName == "Null" ) curr = ItemList[ rand( ) % 638 ];
                    // IO::messageBox( curr, 31 );
                    break;
                }
                case 3: {
                            /*
                    std::vector<pokemon> cpy;

                    for( u8 i = 0; i < 3; ++i ) {
                        pokemon a( 0, i + 456, 0, 30, SAVE::SAV.getActiveFile( ).m_id + 1,
                                   SAVE::SAV.getActiveFile( ).m_sid, "Heiko", false );
                        // a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                    }
                    BATTLE::battleTrainer opp(
                            {"Wally", "Heiko"}, {"Let's battle!", "Auf in den Kampf!"},
                            {"Hmpf\x85 You're not too bad\x85",
                            "Hm\x85 Du bist gar nicht so schlecht\x85"},
                            {"Yay, I won!", "Yay gewonnen!"},
                            {"Well, I lost\x85", "Das war wohl eine Niederlage\x85"},
                            cpy, 0, 0 );
                    auto           bt = SAVE::SAV.getActiveFile( ).getBattleTrainer( );
                    BATTLE::battle test_battle( bt, &opp, 100, BATTLE::weather( rand( ) % 9 ), 10,
                                                0, 5, BATTLE::battle::DOUBLE );
                    ANIMATE_MAP = false;
                    test_battle.start( );
                    SAVE::SAV.getActiveFile( ).updateTeam( bt );
                    delete bt;
                    */
                    break;
                }
                case 4: {
                            /*
                    std::vector<pokemon> cpy;

                    for( u8 i = 0; i < 6; ++i ) {
                        pokemon a( 0, 435 + i, 0, 15, SAVE::SAV.getActiveFile( ).m_id + 1,
                                   SAVE::SAV.getActiveFile( ).m_sid, "Heiko", false );
                        // a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                    }
                    BATTLE::battleTrainer opp(
                            {"Wally", "Heiko"}, {"Let's battle!", "Auf in den Kampf!"},
                            {"Hmpf\x85 You're not too bad\x85",
                            "Hm\x85 Du bist gar nicht so schlecht\x85"},
                            {"Yay, I won!", "Yay gewonnen!"},
                            {"Well, I lost\x85", "Das war wohl eine Niederlage\x85"},
                            cpy, 0, 0 );
                    auto           bt = SAVE::SAV.getActiveFile( ).getBattleTrainer( );
                    BATTLE::battle test_battle( bt, &opp, 100,
                                                BATTLE::HAIL weather( rand( ) % 9 ), 10, 0, 5,
                                                BATTLE::battle::SINGLE );
                    ANIMATE_MAP = false;
                    test_battle.start( );
                    SAVE::SAV.getActiveFile( ).updateTeam( bt );
                    delete bt;
                    */
                    break;
                }
                case 5: {
                    const char* bgNames[ MAXBG ];
                    for( u8 o = 0; o < MAXBG; ++o ) bgNames[ o ] = NAV::BGs[ o ].m_name.c_str( );

                    IO::choiceBox scrnChoice( MAXBG, bgNames, 0, true );
                    draw( true,
                          scrnChoice.getResult( "Welcher Hintergrund\nsoll dargestellt werden?" ) );
                    break;
                }
                case 6:
                case 7: {
                    BOX::boxViewer bxv;
                    ANIMATE_MAP = false;

                    bxv.run( res % 2 );

                    consoleSelect( &IO::Top );
                    consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
                    consoleClear( );
                    consoleSelect( &IO::Bottom );
                    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    break;
                }
                case 8:
                    SAVE::SAV.getActiveFile( ).m_HOENN_Badges <<= 1;
                    SAVE::SAV.getActiveFile( ).m_HOENN_Badges |= 1;
                    break;
                case 9:
                    SAVE::SAV.getActiveFile( ).m_KANTO_Badges <<= 1;
                    SAVE::SAV.getActiveFile( ).m_KANTO_Badges |= 1;
                    break;
                case 10: {
                    IO::keyboard kbd;
                    IO::messageBox( kbd.getText( 10, "Type some text!" ).c_str( ) );
                    break;
                }
                case 11: {
                    for( u16 j = I_FLAME_PLATE; j <= I_FLAME_PLATE + 17; ++j ) {
                        SAVE::SAV.getActiveFile( ).m_bag.insert(
                            BAG::toBagType( ITEM::ITEMTYPE_COLLECTIBLE ), j, 1 );
                    }
                }
                }
                if( res != 10 ) draw( true );
                swiWaitForVBlank( );
                if( res == 3 || res == 4 || res == 6 || res == 7 ) {
                    FADE_TOP_DARK( );
                    ANIMATE_MAP = true;
                    MAP::curMap->draw( );
                }
            } else if( GET_AND_WAIT_C( POS[ NAV_ID ][ 0 ], POS[ NAV_ID ][ 1 ], 16 ) ) { // Save
                IO::yesNoBox Save( GET_STRING( 91 ) );
                if( Save.getResult( GET_STRING( 92 ) ) ) {
                    draw( );
                    if( FS::writeSave( p_path ) )
                        IO::messageBox Succ( GET_STRING( 94 ), GET_STRING( 91 ) );
                    else
                        IO::messageBox Succ( GET_STRING( 95 ), GET_STRING( 91 ) );
                }
                draw( true );
            }
        }
    }
} // namespace NAV
