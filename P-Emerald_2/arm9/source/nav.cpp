/*
Pokémon Emerald 2 Version
------------------------------

file        : nav.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
Philip Wellnitz

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

#include <map>

#include "nav.h"
#include "uio.h"
#include "messageBox.h"
#include "yesNoBox.h"
#include "choiceBox.h"
#include "keyboard.h"
#include "sprite.h"
#include "screenFade.h"
#include "fs.h"
#include "defines.h"

#include "hmMoves.h"
#include "item.h"
#include "berry.h"
#include "pokemon.h"

#include "bagUI.h"
#include "bagViewer.h"

#include "statusScreen.h"
#include "statusScreenUI.h"

#include "boxUI.h"
#include "boxViewer.h"

#include "dex.h"
#include "dexUI.h"

#include "mapDrawer.h"
#include "mapSlice.h"
#include "mapObject.h"

#include "battle.h"
#include "battleTrainer.h"

#include "Border.h"

#include "Back.h"
#include "poweroff.h"
#include "poweron.h"
#include "Option.h"
#include "PokemonSp.h"
#include "Id.h"
#include "SPBag.h"
#include "Save.h"
#include "PokeDex.h"
#include "BagSpr.h"

namespace IO {
    nav* NAV = 0;
    u8 mainSpritesPositions[ 12 ] = { 24, 64,
        236, 96,
        20, 128,
        238, 64,
        22, 96,
        234, 128 };
    u8 mainSpritesPositions2[ 12 ] = { 8, 64,
        252, 96,
        4, 128,
        254, 64,
        6, 96,
        250, 128 };
    unsigned int NAV_DATA[ 12288 ] = { 0 };
    unsigned short NAV_DATA_PAL[ 256 ] = { 0 };
    nav::backgroundSet BGs[ MAXBG ] = { { "Executing Exeggcute", NAV_DATA, NAV_DATA_PAL, true, true, mainSpritesPositions },
                                  { "Raging Gyarados", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Sleeping Eevee", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Mystic Guardevoir", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Waiting Suicune", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Awakening Xerneas", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Awakening Yveltal", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Fighting Groudon", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Fighting Kyogre", NAV_DATA,NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Fighting Torchic", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Reborn Ho-Oh", NAV_DATA, NAV_DATA_PAL, true, false, mainSpritesPositions },
                                  { "Working Klink", NAV_DATA, NAV_DATA_PAL, true, true, mainSpritesPositions }
    };

#define POS( p_isHome ) ( (p_isHome) ? IO::BGs[ SAVE::SAV->getActiveFile( ).m_options.m_bgIdx ].m_mainMenuSpritePoses : mainSpritesPositions2 )

    std::map<nav::state, nav::state> backTransition = {
        { nav::MAP_MUG, nav::MAP },
        { nav::MAP_BIG, nav::MAP },
        { nav::MAP, nav::HOME },

        { nav::HOME, nav::HOME }
    };

    void drawBorder( ) {
        auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( bg2 ) : bgGetGfxPtr( bg2sub );
        auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

        dmaCopy( BorderBitmap, ptr, 256 * 192 );
        dmaCopy( BorderPal + 192, pal + 192, 64 );

        DRAW_TIME = true;
    }

    void updateItems( ) {
        bool b = false;
        for( u8 i = 0; i < 3; ++i ) {
            u16 curitm = SAVE::SAV->getActiveFile( ).m_lstUsedItems[ ( SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx + 4 - i ) % 5 ];
            if( !b && SAVE::SAV->getActiveFile( ).m_registeredItem && ( !curitm || i == 2 || curitm == SAVE::SAV->getActiveFile( ).m_registeredItem ) ) {
                curitm = SAVE::SAV->getActiveFile( ).m_registeredItem;
                b = true;
            }
            if( curitm )
                IO::loadItemIcon( ItemList[ curitm ]->m_itemName,
                                  80 + 32 * i, -4, ITM( i ), ITM( i ),
                                  IO::Oam->oamBuffer[ ITM_BACK + 2 ].gfxIndex + 32 * ( i + 1 ) );
        }
        IO::updateOAM( true );
    }

    void initMainSprites( bool p_power = false, bool p_showBack = false ) {
        IO::initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, tileCnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, !p_showBack, OBJPRIORITY_0, true );

        if( !p_power ) {
            tileCnt = IO::loadSprite( SAVE_ID, SAVE_ID, tileCnt,
                                      -20, -20, 64, 64, poweroffPal,
                                      poweroffTiles, poweroffTilesLen, false, false, false, OBJPRIORITY_0, true );
        } else {
            tileCnt = IO::loadSprite( SAVE_ID, SAVE_ID, tileCnt,
                                      -20, -20, 64, 64, poweronPal,
                                      poweronTiles, poweroffTilesLen, false, false, false, OBJPRIORITY_0, true );
        }
        //Main menu sprites
        tileCnt = IO::loadSprite( OPTS_ID, OPTS_ID, tileCnt,
                                  POS( !p_power || !p_showBack )[ 2 * ( OPTS_ID - 2 ) ] - 16,
                                  POS( !p_power || !p_showBack )[ 2 * ( OPTS_ID - 2 ) + 1 ] - 16,
                                  32, 32, OptionPal, OptionTiles, OptionTilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( PKMN_ID, PKMN_ID, tileCnt,
                                  POS( !p_power || !p_showBack )[ 2 * ( PKMN_ID - 2 ) ] - 16,
                                  POS( !p_power || !p_showBack )[ 2 * ( PKMN_ID - 2 ) + 1 ] - 16,
                                  32, 32, PokemonSpPal, PokemonSpTiles, PokemonSpTilesLen,
                                  false, false, !SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_pid, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( NAV_ID, NAV_ID, tileCnt,
                                  POS( !p_power || !p_showBack )[ 2 * ( NAV_ID - 2 ) ] - 16,
                                  POS( !p_power || !p_showBack )[ 2 * ( NAV_ID - 2 ) + 1 ] - 16,
                                  32, 32, SavePal, SaveTiles, SaveTilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( ID_ID, ID_ID, tileCnt,
                                  POS( !p_power || !p_showBack )[ 2 * ( ID_ID - 2 ) ] - 16,
                                  POS( !p_power || !p_showBack )[ 2 * ( ID_ID - 2 ) + 1 ] - 16,
                                  32, 32, IdPal, IdTiles, IdTilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( DEX_ID, DEX_ID, tileCnt,
                                  POS( !p_power || !p_showBack )[ 2 * ( DEX_ID - 2 ) ] - 16,
                                  POS( !p_power || !p_showBack )[ 2 * ( DEX_ID - 2 ) + 1 ] - 16,
                                  32, 32, PokeDexPal, PokeDexTiles, PokeDexTilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( BAG_ID, BAG_ID, tileCnt,
                                  POS( !p_power || !p_showBack )[ 2 * ( BAG_ID - 2 ) ] - 16,
                                  POS( !p_power || !p_showBack )[ 2 * ( BAG_ID - 2 ) + 1 ] - 16,
                                  32, 32, SPBagPal, SPBagTiles, SPBagTilesLen,
                                  false, false, false, OBJPRIORITY_0, true );

        IO::loadSprite( ITM_BACK, ITM_BACK, tileCnt,
                        80, -4, 32, 32, BagSprPal,
                        BagSprTiles, BagSprTilesLen, false, false, false, OBJPRIORITY_2, true );
        IO::loadSprite( ITM_BACK + 1, ITM_BACK, tileCnt,
                        112, -4, 32, 32, BagSprPal,
                        BagSprTiles, BagSprTilesLen, false, false, false, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( ITM_BACK + 2, ITM_BACK, tileCnt,
                                  144, -4, 32, 32, BagSprPal,
                                  BagSprTiles, BagSprTilesLen, false, false, false, OBJPRIORITY_2, true );
        updateItems( );
        IO::updateOAM( true );
    }

    nav::nav( ) {
        _allowInit = true;
        _power = false;
        _state = HOME;
        draw( true );
    }

    void nav::drawMapMug( ) {
        auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( bg3 ) : bgGetGfxPtr( bg3sub );
        char buffer[ 100 ];
        snprintf( buffer, 99, "%hu_%hhu", _curMap, getCurrentDaytime( ) % 4 );
        FS::readPictureData( ptr, "nitro:/PICS/MAP_MUG/", buffer, 512, 49152, !SCREENS_SWAPPED );
        drawBorder( );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        regularFont->setColor( WHITE_IDX, 1 );
        regularFont->setColor( GRAY_IDX, 2 );
        regularFont->setColor( 0, 0 );
        regularFont->printString( FS::getLocation( MAP::mapInfo[ _curMap / 100 * 100 + 10 ].first ).c_str( ), 246, 8, !SCREENS_SWAPPED, IO::font::RIGHT );
        regularFont->printString( FS::getLocation( MAP::mapInfo[ _curMap ].first ).c_str( ), 10, 166, !SCREENS_SWAPPED );
        regularFont->setColor( WHITE_IDX, 2 );
        regularFont->setColor( BLACK_IDX, 1 );
    }

    void nav::draw( bool p_initMainSrites, u8 p_newIdx ) {
        if( SAVE::SAV->getActiveFile( ).m_options.m_bgIdx == p_newIdx )
            return;
        else if( p_newIdx == u8( 255 ) )
            p_newIdx = SAVE::SAV->getActiveFile( ).m_options.m_bgIdx;

        auto ptr = SCREENS_SWAPPED ? bgGetGfxPtr( bg3 ) : bgGetGfxPtr( bg3sub );
        auto pal = SCREENS_SWAPPED ? BG_PALETTE : BG_PALETTE_SUB;

        if( !_power || _state != MAP_MUG ) {
            if( !BGs[ p_newIdx ].m_loadFromRom ) {
                dmaCopy( BGs[ p_newIdx ].m_mainMenu, ptr, 256 * 192 );
                dmaCopy( BGs[ p_newIdx ].m_mainMenuPal, pal, 192 * 2 );
                SAVE::SAV->getActiveFile( ).m_options.m_bgIdx = p_newIdx;
            } else if( !FS::readNavScreenData( ptr, BGs[ p_newIdx ].m_name.c_str( ), p_newIdx ) ) {
                dmaCopy( BGs[ 0 ].m_mainMenu, ptr, 256 * 192 );
                dmaCopy( BGs[ 0 ].m_mainMenuPal, pal, 192 * 2 );
                SAVE::SAV->getActiveFile( ).m_options.m_bgIdx = 0;
            } else
                SAVE::SAV->getActiveFile( ).m_options.m_bgIdx = p_newIdx;
            drawBorder( );
        } else if( _state == MAP_MUG ) {
            drawBorder( );
            drawMapMug( );
        }
        if( p_initMainSrites && _allowInit )
            initMainSprites( _power, _power && _state != HOME );

    }

    void nav::showNewMap( u8 p_newMap ) {
        if( p_newMap == _curMap )
            return;
        if( p_newMap % 100 == 10 ) {
            _state = MAP;
            _curMap = p_newMap;
            draw( true );
        } else
            if( true || FS::exists( "nitro:/PICS/MAP_MUG/", _curMap, false ) ) {
                _power = true;
                _state = MAP_MUG;
                _curMap = p_newMap;
                draw( true );
            }
    }

    void nav::handleInput( touchPosition p_touch, const char* p_path ) {
        touchPosition& touch = p_touch;

        if( held & KEY_Y ) {
            IO::waitForKeysUp( KEY_Y );
            if( SAVE::SAV->getActiveFile( ).m_registeredItem ) {
                if( ItemList[ SAVE::SAV->getActiveFile( ).m_registeredItem ]->useable( ) ) {
                    ItemList[ SAVE::SAV->getActiveFile( ).m_registeredItem ]->use( );
                    updateItems( );
                } else {
                    IO::messageBox( GET_STRING( 58 ), GET_STRING( 91 ) );
                    IO::NAV->draw( true );
                }
            } else {
                IO::messageBox( GET_STRING( 98 ), GET_STRING( 91 ) );
                IO::NAV->draw( true );
            }
            swiWaitForVBlank( );
            scanKeys( );
            return;
        }

        bool itmsn = false;
        for( u8 i = 0; i < 3; ++i ) {
            u16 curitm = SAVE::SAV->getActiveFile( ).m_lstUsedItems[ ( SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx + 4 - i ) % 5 ];
            if( !itmsn && SAVE::SAV->getActiveFile( ).m_registeredItem
                && ( !curitm || i == 2 || curitm == SAVE::SAV->getActiveFile( ).m_registeredItem ) ) {
                curitm = SAVE::SAV->getActiveFile( ).m_registeredItem;
                itmsn = true;
            }
            if( GET_AND_WAIT_C( 96 + 32 * i, 12, 14 ) ) {
                if( curitm ) {
                    if( u16( -1 ) == SAVE::SAV->getActiveFile( ).m_bag.count( BAG::toBagType( ItemList[ curitm ]->m_itemType ), curitm ) ) {
                        IO::yesNoBox yn( GET_STRING( 91 ) );
                        char buffer[ 100 ];
                        snprintf( buffer, 99, GET_STRING( 96 ), ItemList[ curitm ]->getDisplayName( true ).c_str( ) );
                        if( yn.getResult( buffer ) ) {
                            for( u8 j = i; j < 4; ++j ) {
                                SAVE::SAV->getActiveFile( ).m_lstUsedItems[ ( SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx + 4 - j ) % 5 ] =
                                    SAVE::SAV->getActiveFile( ).m_lstUsedItems[ ( SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx + 3 - j ) % 5 ];
                            }
                            SAVE::SAV->getActiveFile( ).m_lstUsedItems[ SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx ] = 0;
                        }
                        IO::NAV->draw( true );
                    } else if( ItemList[ curitm ]->useable( ) ) {
                        if( ItemList[ curitm ]->use( true ) )
                            IO::messageBox( "", 0, false );
                        ItemList[ curitm ]->use( );
                        if( ItemList[ curitm ]->m_itemType != item::KEY_ITEM )
                            SAVE::SAV->getActiveFile( ).m_bag.erase( BAG::toBagType( ItemList[ curitm ]->m_itemType ), curitm, 1 );
                        IO::NAV->draw( true );
                    } else {
                        IO::messageBox( GET_STRING( 58 ), GET_STRING( 91 ) );
                        IO::NAV->draw( true );
                    }
                } else {
                    IO::messageBox( GET_STRING( 97 ), GET_STRING( 91 ) );
                    IO::NAV->draw( true );
                }
                return;
            }
        }

        if( _state != HOME && _power && GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
            _state = backTransition[ _state ];
            draw( true );
        } else {
            //StartBag
            if( GET_AND_WAIT_C( POS( _state == HOME || !_power )[ 6 ],
                                POS( _state == HOME || !_power )[ 7 ], 16 ) ) {
                BAG::bagViewer bv;
                ANIMATE_MAP = false;
                UPDATE_TIME = false;

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                u16 res = bv.run( );

                FADE_TOP_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::clearScreenConsole( true, true );
                _state = HOME;
                draw( true );
                UPDATE_TIME = true;
                MAP::curMap->draw( );
                ANIMATE_MAP = true;
                updateItems( );
                if( res ) {
                    ItemList[ res ]->use( false );
                    updateItems( );
                    draw( true );
                }
            } else if( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_speciesId     //StartPkmn
                       && ( GET_AND_WAIT_C( POS( _state == HOME || !_power )[ 0 ],
                                            POS( _state == HOME || !_power )[ 1 ], 16 ) ) ) {
                ANIMATE_MAP = false;
                _state = HOME;
                draw( true );
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
                draw( true );
                MAP::curMap->draw( );
                ANIMATE_MAP = true;

                if( res )
                    res->use( );
            } else if( GET_AND_WAIT_C( POS( _state == HOME || !_power )[ 4 ],         //StartDex
                                       POS( _state == HOME || !_power )[ 5 ], 16 ) ) {
                ANIMATE_MAP = false;
                _state = HOME;
                draw( true );

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                DEX::dex( DEX::dex::SHOW_CAUGHT, MAX_PKMN ).run( SAVE::SAV->getActiveFile( ).m_lstDex );

                FADE_TOP_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                IO::clearScreenConsole( true, true );
                draw( true );
                MAP::curMap->draw( );
                ANIMATE_MAP = true;
            } else if( GET_AND_WAIT_C( POS( _state == HOME || !_power )[ 8 ],         //StartOptions
                                       POS( _state == HOME || !_power )[ 9 ], 16 ) ) {


            } else if( GET_AND_WAIT_C( POS( _state == HOME || !_power )[ 2 ],         //StartID
                                       POS( _state == HOME || !_power )[ 3 ], 16 ) ) {

                _state = HOME;
                draw( true );

                const char *someText[ 11 ] = { "PKMN Spawn", "Item Spawn", "1 Item Test",
                                                "Dbl Battle", "Sgl Battle", "Chg NavScrn",
                                                "View Boxes A", "View Boxes B", "Hoenn Badges",
                                                "Kanto Badges", "Keyboard" };
                IO::choiceBox test( 11, &someText[ 0 ], 0, false );
                int res = test.getResult( "Tokens of god-being..." );
                draw( );
                switch( res ) {
                    case 0:
                    {
                        memset( SAVE::SAV->getActiveFile( ).m_pkmnTeam, 0, sizeof( SAVE::SAV->getActiveFile( ).m_pkmnTeam ) );
                        for( int i = 0; i < 5; ++i ) {
                            pokemon& a = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ];
                            a = pokemon( 0, rand( ) % MAX_PKMN + 1, 0,
                                         50, SAVE::SAV->getActiveFile( ).m_id, SAVE::SAV->getActiveFile( ).m_sid, SAVE::SAV->getActiveFile( ).m_playername,
                                         !SAVE::SAV->getActiveFile( ).m_isMale, i, false, i % 2, i == 3, i + rand( ) % 500, i, i );
                            a.m_stats.m_acHP *= i / 5.0;
                            a.m_boxdata.m_experienceGained += 750;

                            //Hand out some ribbons
                            for( u8 j = 0; j < 4; ++j ) {
                                a.m_boxdata.m_ribbons0[ j ] = rand( ) % 255;
                                a.m_boxdata.m_ribbons1[ j ] = rand( ) % 255;
                                a.m_boxdata.m_ribbons2[ j ] = rand( ) % 255;
                            }
                            a.m_boxdata.m_ribbons1[ 2 ] = rand( ) % 63;
                            a.m_boxdata.m_ribbons1[ 3 ] = 0;
                            a.m_boxdata.m_holdItem = 1 + rand( ) % 400;

                            SAVE::SAV->m_caughtPkmn[ ( a.m_boxdata.m_speciesId ) / 8 ] |= ( 1 << ( ( a.m_boxdata.m_speciesId ) % 8 ) );
                        }

                        for( u16 j : { 493, 649, 648, 647, 487, 492, 641, 642, 646, 645, 643, 644 } ) {
                            auto a = pokemon( j, 50, 0, j ).m_boxdata;
                            a.m_gotPlace = j;
                            SAVE::SAV->storePkmn( a );
                            /*if( a.isShiny( ) ) {
                              IO::messageBox( "YAAAY" );
                              s8 idx = SAVE::SAV->getCurrentBox( )->getFirstFreeSpot( );
                              if( idx == -1 && !( *SAVE::SAV->getCurrentBox( ) )[ 17 ].isShiny( ) )
                              IO::messageBox( "Lost :(" );
                              else if( !( *SAVE::SAV->getCurrentBox( ) )[ idx - 1 ].isShiny( ) )
                              IO::messageBox( "Lost :(" );
                              break;
                              }*/
                        }

                        SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 0 ] = M_SURF;
                        SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 1 ] = M_WATERFALL;
                        SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 0 ] = M_ROCK_CLIMB;
                        SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 3 ].m_boxdata.m_moves[ 0 ] = M_SWEET_SCENT;

                        swiWaitForVBlank( );
                        break;
                    }
                    case 1:
                        for( u16 j = 1; j < 772; ++j ) {
                            if( ItemList[ j ]->m_itemName != "Null" )
                                SAVE::SAV->getActiveFile( ).m_bag.insert( BAG::toBagType( ItemList[ j ]->m_itemType ), j, 1 );
                        }
                        break;
                    case 2:
                    {
                        item* curr = ItemList[ rand( ) % 638 ];
                        while( curr->m_itemName == "Null" )
                            curr = ItemList[ rand( ) % 638 ];
                        IO::messageBox( curr, 31 );
                        break;
                    }
                    case 3:
                    {
                        std::vector<pokemon> cpy;

                        for( u8 i = 0; i < 3; ++i ) {
                            pokemon a( 0, i + 456, 0,
                                       30, SAVE::SAV->getActiveFile( ).m_id + 1, SAVE::SAV->getActiveFile( ).m_sid, "Heiko", false );
                            //a.stats.acHP = i*a.stats.maxHP/5;
                            cpy.push_back( a );
                        }
                        BATTLE::battleTrainer opp( "Heiko", "Auf in den Kampf!", "Hm… Du bist gar nicht so schlecht…",
                                                   "Yay gewonnen!", "Das war wohl eine Niederlage…", cpy, 0, 0 );
                        auto bt = SAVE::SAV->getActiveFile( ).getBattleTrainer( );
                        BATTLE::battle test_battle( bt, &opp, 100,
                                                    BATTLE::weather( rand( ) % 9 ), 10, 0, 5, BATTLE::battle::DOUBLE );
                        ANIMATE_MAP = false;
                        test_battle.start( );
                        SAVE::SAV->getActiveFile( ).updateTeam( bt );
                        delete bt;
                        break;
                    }
                    case 4:
                    {
                        std::vector<pokemon> cpy;

                        for( u8 i = 0; i < 6; ++i ) {
                            pokemon a( 0, 435 + i, 0,
                                       15, SAVE::SAV->getActiveFile( ).m_id + 1, SAVE::SAV->getActiveFile( ).m_sid, "Heiko", false );
                            //a.stats.acHP = i*a.stats.maxHP/5;
                            cpy.push_back( a );
                        }
                        BATTLE::battleTrainer opp( "Heiko", "Auf in den Kampf!", "Hm… Du bist gar nicht so schlecht…",
                                                   "Yay gewonnen!", "Das war wohl eine Niederlage…", cpy, 0, 0 );

                        auto bt = SAVE::SAV->getActiveFile( ).getBattleTrainer( );
                        BATTLE::battle test_battle( bt, &opp, 100,
                                                    BATTLE::HAIL/*weather( rand( ) % 9 )*/, 10, 0, 5, BATTLE::battle::SINGLE );
                        ANIMATE_MAP = false;
                        test_battle.start( );
                        SAVE::SAV->getActiveFile( ).updateTeam( bt );
                        delete bt;
                        break;
                    }
                    case 5:
                    {
                        const char *bgNames[ MAXBG ];
                        for( u8 o = 0; o < MAXBG; ++o )
                            bgNames[ o ] = IO::BGs[ o ].m_name.c_str( );

                        IO::choiceBox scrnChoice( MAXBG, bgNames, 0, true );
                        draw( true, scrnChoice.getResult( "Welcher Hintergrund\nsoll dargestellt werden?" ) );
                        break;
                    }
                    case 6: case 7:
                    {
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
                        SAVE::SAV->getActiveFile( ).m_HOENN_Badges <<= 1;
                        SAVE::SAV->getActiveFile( ).m_HOENN_Badges |= 1;
                        break;
                    case 9:
                        SAVE::SAV->getActiveFile( ).m_KANTO_Badges <<= 1;
                        SAVE::SAV->getActiveFile( ).m_KANTO_Badges |= 1;
                        break;
                    case 10:
                    {
                        IO::keyboard kbd;
                        IO::messageBox( kbd.getText( 10, "Type some text!" ).c_str( ), true );
                        break;
                    }
                }
                draw( true );
                swiWaitForVBlank( );
                if( res == 3 || res == 4 || res == 6 || res == 7 ) {
                    FADE_TOP_DARK( );
                    ANIMATE_MAP = true;
                    MAP::curMap->draw( );
                }
            } else if( GET_AND_WAIT_C( POS( _state == HOME || !_power )[ 10 ],        //Save
                                       POS( _state == HOME || !_power )[ 11 ], 16 ) ) {
                IO::yesNoBox Save( GET_STRING( 91 ) );
                if( Save.getResult( GET_STRING( 92 ) ) ) {
                    draw( );
                    if( !p_path || gMod == EMULATOR )
                        IO::messageBox Succ( GET_STRING( 93 ), GET_STRING( 91 ) );
                    else if( p_path && FS::writeSave( SAVE::SAV, p_path ) )
                        IO::messageBox Succ( GET_STRING( 94 ), GET_STRING( 91 ) );
                    else
                        IO::messageBox Succ( GET_STRING( 95 ), GET_STRING( 91 ) );
                }
                draw( true );
            } else if( touch.px && touch.py && GET_AND_WAIT_C( 8, 12, 17 ) ) {               //Start Pokénav
                _power = !_power;
                draw( true );
            }
        }
    }
}
