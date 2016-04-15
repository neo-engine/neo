/*
Pokémon Emerald 2 Version
------------------------------

file        : statusScreenUI.cpp
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

#include "statusScreenUI.h"
#include "defines.h"
#include "buffer.h"
#include "move.h"
#include "item.h"
#include "uio.h"
#include "sprite.h"
#include "fs.h"
#include "saveGame.h"
#include "ribbon.h"
#include "boxUI.h"

#include <cstdio>
#include <algorithm>

#include "memo.h"
#include "atks.h"
#include "Contest.h"
#include "PKMN.h"
#include "time_icon.h"

#include "anti_pokerus_icon.h"
#include "pokerus_icon.h"

#include "Back.h"
#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Up.h"
#include "Down.h"

namespace STS {
    u8 borders[ 6 ][ 2 ] = {
        { 4, 2 }, { 18, 3 },
        { 4, 9 }, { 18, 10 },
        { 4, 17 }, { 18, 18 }
    };

    regStsScreenUI::regStsScreenUI( u8 p_pageMax ) {
        m_pagemax = p_pageMax;
    }

    //OAMTop indices
#define ICON_IDX( a ) ( a )
#define SHINY_IDX 100
#define PKMN_SPRITE_START 6
#define PAGE_ICON_IDX  10
#define ITEM_ICON_IDX  11
#define TYPE_IDX 12
#define ATK_DMGTYPE_IDX( a ) ( 16 + u8( a ) )
#define RIBBON_IDX 20

#define ICON_PAL( a ) ( a )
#define SHINY_PAL 5
#define PKMN_SPRITE_PAL 6
#define PAGE_ICON_PAL 7
#define ITEM_ICON_PAL 8
#define TYPE_PAL( a ) ( 9 + ( a ) )
#define DMG_TYPE_PAL( a ) ( 13 + u8( a ) )

    //OamSub indices
    //#define BACK_ID  0
#define SUB_PAGE_ICON_IDX( a ) ( 1 + ( a ) )
#define SUB_BALL_IDX( a ) ( 7 + (a) )
    //#define FWD_ID 13
    //#define BWD_ID 14

#define HP_COL 238
    void regStsScreenUI::initSub( ) {
        IO::initOAMTable( true );
        u16 nextAvailableTileIdx = 0;

        nextAvailableTileIdx = IO::loadSprite( BACK_ID, BACK_ID, nextAvailableTileIdx,
                                               SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                               BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite( SUB_PAGE_ICON_IDX( 0 ), SUB_PAGE_ICON_IDX( 0 ), nextAvailableTileIdx,
                                               0, 0, 32, 32, time_iconPal,
                                               time_iconTiles, time_iconTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite( SUB_PAGE_ICON_IDX( 1 ), SUB_PAGE_ICON_IDX( 1 ), nextAvailableTileIdx,
                                               0, 0, 32, 32, PKMNPal,
                                               PKMNTiles, PKMNTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite( SUB_PAGE_ICON_IDX( 2 ), SUB_PAGE_ICON_IDX( 2 ), nextAvailableTileIdx,
                                               0, 0, 32, 32, memoPal,
                                               memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite( SUB_PAGE_ICON_IDX( 3 ), SUB_PAGE_ICON_IDX( 3 ), nextAvailableTileIdx,
                                               0, 0, 32, 32, atksPal,
                                               atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite( SUB_PAGE_ICON_IDX( 4 ), SUB_PAGE_ICON_IDX( 4 ), nextAvailableTileIdx,
                                               0, 0, 32, 32, ContestPal,
                                               ContestTiles, ContestTilesLen, false, false, false, OBJPRIORITY_0, true );
        for( u8 i = 0; i < 5; ++i ) {
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].x = 48 + 32 * i;
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].y = -2 * i;
        }

        for( u8 i = 0; i < 6; ++i ) {
            auto pkmn = FS::SAV->m_pkmnTeam[ i ].m_boxdata;
            if( pkmn.m_individualValues.m_isEgg )
                nextAvailableTileIdx = IO::loadEggIcon( 4 - 2 * i, 28 + 24 * i,
                                                        SUB_BALL_IDX( i ), SUB_BALL_IDX( i ), nextAvailableTileIdx );
            else
                nextAvailableTileIdx = IO::loadItemIcon( !pkmn.m_ball ? "Pokeball" : ItemList[ pkmn.m_ball ]->m_itemName, 4 - 2 * i,
                                                         28 + 24 * i, SUB_BALL_IDX( i ), SUB_BALL_IDX( i ), nextAvailableTileIdx, true );
        }

        nextAvailableTileIdx = IO::loadSprite( FWD_ID, FWD_ID, nextAvailableTileIdx,
                                               SCREEN_WIDTH - 28 - 25, SCREEN_HEIGHT - 25, 32, 32, DownPal,
                                               DownTiles, DownTilesLen, false, false, false, OBJPRIORITY_1, true );
        nextAvailableTileIdx = IO::loadSprite( BWD_ID, BWD_ID, nextAvailableTileIdx,
                                               SCREEN_WIDTH - 25, SCREEN_HEIGHT - 28 - 25, 32, 32, UpPal,
                                               UpTiles, UpTilesLen, false, false, false, OBJPRIORITY_1, true );
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        IO::updateOAM( true );
    }

    void regStsScreenUI::initTop( ) {
        IO::initOAMTable( false );
        u16 tileCnt = 0;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        for( size_t i = 0; i < FS::SAV->getTeamPkmnCount( ); i++ ) {
            char buffer[ 100 ];
            u16 sx, sy;
            u16 x = 133;
            if( i % 2 == 0 ) {
                sx = 20;
                sy = 24 + 60 * ( i / 2 );
            } else {
                sx = 220;
                sy = 36 + 60 * ( i / 2 );
            }
#define ADJUST_X( i, x, buffer ) ( ( ( i ) % 2 ) ? ( x ) : \
                                   123 - IO::regularFont->stringWidth( buffer ) )

            if( !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg ) {
                tileCnt = IO::loadPKMNIcon( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId, sx - 10, sy - 10,
                                            ICON_IDX( i ), ICON_PAL( i ), tileCnt, false );
                IO::displayHP( 100, 101, sx - 8, sy - 8, HP_COL + 2 * i, HP_COL + 1 + 2 * i, false, 23, 26 );
                IO::displayHP( 100, 100 - FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP * 100 / FS::SAV->m_pkmnTeam[ i ].m_stats.m_maxHP,
                               sx - 8, sy - 8, HP_COL + 2 * i, HP_COL + 1 + 2 * i, false, 23, 26 );


                IO::regularFont->printString( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_name,
                                              ADJUST_X( i, x, FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_name ), sy - 14, false );

                IO::regularFont->setColor( HP_COL + 2 * i, 2 );
                if( FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP )
                    sprintf( buffer, "%hi/%hiKP", FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP, FS::SAV->m_pkmnTeam[ i ].m_stats.m_maxHP );
                else
                    sprintf( buffer, "Besiegt" );
                IO::regularFont->printString( buffer, ADJUST_X( i, x, buffer ), sy, false );
                IO::regularFont->setColor( GRAY_IDX, 2 );

                sprintf( buffer, "%s", ItemList[ FS::SAV->m_pkmnTeam[ i ].m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ) );
                IO::regularFont->printString( buffer, ADJUST_X( i, x, buffer ), sy + 14, false );

            } else {
                sprintf( buffer, "Ei" );
                IO::regularFont->printString( buffer, ADJUST_X( i, x, buffer ), sy - 7, false );

                sprintf( buffer, "%s", ItemList[ FS::SAV->m_pkmnTeam[ i ].m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ) );
                IO::regularFont->printString( buffer, ADJUST_X( i, x, buffer ), sy + 7, false );

                tileCnt = IO::loadEggIcon( sx - 10, sy - 10, ICON_IDX( i ), ICON_PAL( i ), tileCnt, false );
            }
        }

        //Preload the page specific sprites
        auto currPkmn = FS::SAV->m_pkmnTeam[ _current ];

        IO::OamTop->oamBuffer[ PKMN_SPRITE_START ].gfxIndex = tileCnt;
        tileCnt += 144;

        tileCnt = IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, tileCnt,
                                  0, -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadItemIcon( ItemList[ currPkmn.m_boxdata.getItem( ) ]->m_itemName, 2, 152,
                                    ITEM_ICON_IDX, ITEM_ICON_PAL, tileCnt, false );

        for( u8 i = 0; i < 4; ++i ) {
            type t = AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_moveType;
            tileCnt = IO::loadTypeIcon( t, 126, 43 + 32 * i, TYPE_IDX + i, TYPE_PAL( i ), tileCnt, false );
        }
        for( u8 i = 0; i < 4; ++i ) {
            tileCnt = IO::loadDamageCategoryIcon( ( move::moveHitTypes )( i % 3 ), 126, 43 + 32 * i,
                                                  ATK_DMGTYPE_IDX( i ), DMG_TYPE_PAL( i % 3 ), tileCnt, false );
        }

        for( u8 i = PKMN_SPRITE_START; i < RIBBON_IDX; ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( false );
    }

    void regStsScreenUI::init( u8 p_current, bool p_initTop ) {
        _current = p_current;

        if( p_initTop ) {
            IO::vramSetup( );
            swiWaitForVBlank( );
            FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PKMNScreen" );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        }
        bgUpdate( );

        IO::NAV->draw( );
        if( p_initTop )
            initTop( );
        initSub( );
    }

    void hideSprites( bool p_bottom ) {
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        for( u8 i = 0; i < 6; ++i )
            Oam->oamBuffer[ ICON_IDX( i ) ].isHidden = true;
        for( u8 i = 0; i < 4; ++i )
            Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
        for( u8 i = 0; i < 4; ++i )
            Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
        for( u8 i = 0; i < 12; ++i )
            Oam->oamBuffer[ RIBBON_IDX + i ].isHidden = true;
    }

    pokemonData data;

    void drawPkmnInformation( const pokemon& p_pokemon, u8& p_page, bool p_newpok, bool p_bottom ) {
        dmaFillWords( 0, bgGetGfxPtr( p_bottom ? IO::bg2sub : IO::bg2 ), 256 * 192 );
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        auto pal = BG_PAL( p_bottom );
        if( p_newpok ) {
            IO::loadItemIcon( !p_pokemon.m_boxdata.m_ball ? "Pokeball" : ItemList[ p_pokemon.m_boxdata.m_ball ]->m_itemName,
                              -6, 22, SHINY_IDX, SHINY_PAL, 1000, p_bottom );
        }

        if( !p_pokemon.m_boxdata.m_individualValues.m_isEgg ) {
            pal[ RED_IDX ] = RED;
            pal[ BLUE_IDX ] = BLUE;
            pal[ RED2_IDX ] = RED2;
            pal[ BLUE2_IDX ] = BLUE2;
            pal[ WHITE_IDX ] = WHITE;
            IO::regularFont->setColor( BLACK_IDX, 1 );

            IO::regularFont->printString( p_pokemon.m_boxdata.m_name, 150, 0, p_bottom );
            s8 G = p_pokemon.m_boxdata.gender( );

            IO::regularFont->printChar( '/', 234, 0, p_bottom );
            if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32 ) {
                if( G == 1 ) {
                    IO::regularFont->setColor( BLUE_IDX, 1 );
                    IO::regularFont->setColor( BLUE2_IDX, 2 );
                    IO::regularFont->printChar( 136, 246, 7, p_bottom );
                } else if( G == -1 ) {
                    IO::regularFont->setColor( RED_IDX, 1 );
                    IO::regularFont->setColor( RED2_IDX, 2 );
                    IO::regularFont->printChar( 137, 246, 7, p_bottom );
                }
            }
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );

            getAll( p_pokemon.m_boxdata.m_speciesId, data );
            IO::regularFont->printString( data.m_displayName, 160, 13, p_bottom );

            if( p_pokemon.m_boxdata.getItem( ) ) {
                IO::regularFont->printString( "Item", 2, 176, p_bottom );
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
                char buffer[ 200 ];
                sprintf( buffer, "%s: %s", ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ),
                         ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getShortDescription( ).c_str( ) );
                IO::regularFont->printString( buffer, 40, 159, p_bottom );
                if( p_newpok ) {
                    IO::loadItemIcon( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->m_itemName, 2, 152,
                                      ITEM_ICON_IDX, ITEM_ICON_PAL, Oam->oamBuffer[ ITEM_ICON_IDX ].gfxIndex, p_bottom );
                }
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
                IO::regularFont->printString( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( ).c_str( ), 56, 168, p_bottom );
                Oam->oamBuffer[ ITEM_ICON_IDX ].isHidden = true;
            }
            if( p_pokemon.m_boxdata.isShiny( ) ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( WHITE_IDX, 2 );
                IO::regularFont->printString( "*", 1, 47, p_bottom );
                IO::regularFont->setColor( WHITE_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            if( p_pokemon.m_boxdata.isCloned( ) ) {
                IO::regularFont->printString( "*", 20, 28, p_bottom );
            }
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );


            if( p_newpok )
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId, 16, 44,
                                         PKMN_SPRITE_START, PKMN_SPRITE_PAL, Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom,
                                         p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale, true ) ) {
                    IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId, 16, 44,
                                        PKMN_SPRITE_START, PKMN_SPRITE_PAL, Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom,
                                        p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale, true );
                }

            u16 exptype = data.m_expType;

            IO::displayHP( 100, 101, 46, 76, HP_COL, HP_COL + 1, false, 50, 56, p_bottom );
            IO::displayHP( 100, 100 - p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP, 46, 76, HP_COL, HP_COL + 1, false, 50, 56, p_bottom );

            IO::displayEP( 100, 101, 46, 76, HP_COL + 2, HP_COL + 3, false, 59, 62, p_bottom );
            IO::displayEP( 0, ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][ exptype ] ) * 100 /
                           ( EXP[ p_pokemon.m_level ][ exptype ] - EXP[ p_pokemon.m_level - 1 ][ exptype ] ), 46, 76, HP_COL + 2, HP_COL + 3, false, 59, 62, p_bottom );

            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->setColor( HP_COL + 2, 1 );
            sprintf( buffer, "EP %lu%%", ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][ exptype ] ) * 100 /
                     ( EXP[ p_pokemon.m_level ][ exptype ] - EXP[ p_pokemon.m_level - 1 ][ exptype ] ) );
            IO::regularFont->printString( buffer, 62, 28, p_bottom, IO::font::CENTER );
            sprintf( buffer, "KP %i%%", p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP );
            IO::regularFont->printString( buffer, 62, 38, p_bottom, IO::font::CENTER );
            IO::regularFont->setColor( GRAY_IDX, 2 );
            IO::regularFont->setColor( BLACK_IDX, 1 );
        } else {
            p_page = -1;
            pal[ WHITE_IDX ] = WHITE;
            IO::regularFont->setColor( BLACK_IDX, 1 );

            IO::regularFont->printString( "Ei", 150, 0, p_bottom );
            IO::regularFont->printChar( '/', 234, 0, p_bottom );
            IO::regularFont->printString( "Ei", 160, 13, p_bottom );

            IO::loadEggSprite( 29, 60, PKMN_SPRITE_START, PKMN_SPRITE_PAL, Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom );
            for( u8 i = 1; i < 4; ++i )
                Oam->oamBuffer[ PKMN_SPRITE_START + i ].isHidden = true;
            Oam->oamBuffer[ ITEM_ICON_IDX ].isHidden = true;
        }
    }

    void drawPkmnStats( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        auto pal = BG_PAL( p_bottom );

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );

        IO::regularFont->printString( "Statuswerte", 32, 0, p_bottom );
        if( !( currPkmn.m_boxdata.m_individualValues.m_isEgg ) ) {

            pal[ RED_IDX ] = RED;
            pal[ BLUE_IDX ] = BLUE;
            pal[ RED2_IDX ] = RED2;
            pal[ BLUE2_IDX ] = BLUE2;

            char buffer[ 50 ];
            sprintf( buffer, "Status auf Lv.%3i:", currPkmn.m_level );
            IO::regularFont->printString( buffer, 110, 30, p_bottom );

            sprintf( buffer, "KP                     %3i", currPkmn.m_stats.m_maxHP );
            IO::regularFont->printString( buffer, 130, 46, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 0 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 ); IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 0 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 ); IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            sprintf( buffer, "ANG                   %3i", currPkmn.m_stats.m_Atk );
            IO::regularFont->printString( buffer, 130, 65, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 1 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 ); IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 1 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 ); IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            sprintf( buffer, "VER                   %3i", currPkmn.m_stats.m_Def );
            IO::regularFont->printString( buffer, 130, 82, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 3 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 ); IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 3 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 ); IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            sprintf( buffer, "SAN                   %3i", currPkmn.m_stats.m_SAtk );
            IO::regularFont->printString( buffer, 130, 99, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 4 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 ); IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 4 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 ); IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            sprintf( buffer, "SVE                   %3i", currPkmn.m_stats.m_SDef );
            IO::regularFont->printString( buffer, 130, 116, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 2 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 ); IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 2 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 ); IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            sprintf( buffer, "INI                   \xC3\xC3""%3i", currPkmn.m_stats.m_Spd );
            IO::regularFont->printString( buffer, 130, 133, p_bottom );

            IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
            IO::printRectangle( (u8) 158, (u8) 48, u8( 158 + 68 ), u8( 48 + 12 ), p_bottom, false, (u8) 251 );

            IO::printRectangle( (u8) 158, (u8) 48,
                                u8( 158 + ( 68.0 * currPkmn.m_boxdata.IVget( 0 ) / 31 ) ),
                                u8( 48 + 6 ), p_bottom, false, 230 );
            IO::printRectangle( (u8) 158, u8( 48 + 6 ),
                                u8( 158 + ( 68.0 * currPkmn.m_boxdata.m_effortValues[ 0 ] / 252 ) ),
                                u8( 48 + 12 ), p_bottom, false, 230 );

            for( int i = 1; i < 6; ++i ) {
                IO::printRectangle( (u8) 158, u8( 50 + ( 17 * i ) ),
                                    u8( 158 + 68 ), u8( 50 + 12 + ( 17 * i ) ), p_bottom, false, (u8) 251 );
                IO::printRectangle( (u8) 158, u8( 50 + ( 17 * i ) ),
                                    u8( 158 + ( 68.0*currPkmn.m_boxdata.IVget( i ) / 31 ) ),
                                    u8( 50 + 6 + ( 17 * i ) ), p_bottom, false, 230 + i );
                IO::printRectangle( (u8) 158, u8( 50 + 6 + ( 17 * i ) ),
                                    u8( 158 + ( 68.0*currPkmn.m_boxdata.m_effortValues[ i ] / 252 ) ),
                                    u8( 50 + 12 + ( 17 * i ) ), p_bottom, false, 230 + i );
            }
        } else {
            if( currPkmn.m_boxdata.m_steps > 10 ) {
                IO::regularFont->printString( "Was da wohl", 16 * 8, 46, p_bottom );
                IO::regularFont->printString( "schlüpfen wird?", 16 * 8, 66, p_bottom );
                IO::regularFont->printString( "Es dauert wohl", 16 * 8, 96, p_bottom );
                IO::regularFont->printString( "noch lange.", 16 * 8, 116, p_bottom );
            } else if( currPkmn.m_boxdata.m_steps > 5 ) {
                IO::regularFont->printString( "Hat es sich", 16 * 8, 46, p_bottom );
                IO::regularFont->printString( "gerade bewegt?", 16 * 8, 66, p_bottom );
                IO::regularFont->printString( "Da tut sich", 16 * 8, 96, p_bottom );
                IO::regularFont->printString( "wohl bald was.", 16 * 8, 116, p_bottom );
            } else {
                IO::regularFont->printString( "Jetzt macht es", 16 * 8, 46, p_bottom );
                IO::regularFont->printString( "schon Geräusche!", 16 * 8, 66, p_bottom );
                IO::regularFont->printString( "Bald ist es", 16 * 8, 96, p_bottom );
                IO::regularFont->printString( "wohl soweit.", 16 * 8, 116, p_bottom );
            }
        }
    }

    void drawPkmnMoves( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        auto pal = BG_PAL( p_bottom );

        IO::regularFont->printString( "Attacken", 32, 0, p_bottom );
        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );

        for( int i = 0; i < 4; i++ ) {
            if( !currPkmn.m_boxdata.m_moves[ i ] )
                continue;
            type t = AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_moveType;
            IO::loadTypeIcon( t, 222, 38 + 30 * i, TYPE_IDX + i, TYPE_PAL( i ),
                              Oam->oamBuffer[ TYPE_IDX + i ].gfxIndex, p_bottom );

            pal[ COLOR_IDX ] = GREEN;
            if( t == data.m_types[ 0 ] || t == data.m_types[ 1 ] ) {
                IO::regularFont->setColor( COLOR_IDX, 1 );
                IO::regularFont->setColor( WHITE_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }

            IO::regularFont->printString( AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ),
                                          128, 30 + 30 * i, p_bottom );

            IO::regularFont->setColor( GRAY_IDX, 1 );
            IO::regularFont->setColor( WHITE_IDX, 2 );
            sprintf( buffer, "AP %2hhu/%2hhu ", currPkmn.m_boxdata.m_acPP[ i ],
                     s8( AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_movePP * ( ( 5 + currPkmn.m_boxdata.PPupget( i ) ) / 5.0 ) ) );
            IO::regularFont->printString( buffer, 135, 45 + 30 * i, p_bottom );
        }
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
    }

    void drawPkmnRibbons( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, ContestPal, ContestTiles, ContestTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );
        IO::regularFont->printString( "Bänder", 32, 0, p_bottom );

        auto rbs = ribbon::getRibbons( currPkmn );
        //Just draw the first 12 ribbons at max
        u16 tileCnt = Oam->oamBuffer[ TYPE_IDX ].gfxIndex;
        for( u8 i = 0; i < std::min( rbs.size( ), 12u ); ++i ) {
            u16 tmp = IO::loadRibbonIcon( rbs[ i ], 128 + 32 * ( i % 4 ), 36 + 40 * ( i / 4 ), RIBBON_IDX + i, TYPE_PAL( i ) % 16, tileCnt, p_bottom );
            if( !tmp ) //Draw an egg when something goes wrong
                tileCnt = IO::loadEggIcon( 128 + 32 * ( i % 4 ), 32 + 40 * ( i / 4 ), RIBBON_IDX + i, TYPE_PAL( i ) % 16, tileCnt, p_bottom );
            else
                tileCnt = tmp;
        }
        if( rbs.empty( ) ) {
            IO::regularFont->printString( "Keine Bänder", 148, 83, p_bottom );
        } else {
            sprintf( buffer, "(%u)", rbs.size( ) );
            IO::regularFont->printString( buffer, 88, 0, p_bottom );
        }
    }

    void drawPkmnGeneralData( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );
        IO::regularFont->printString( "Trainer-Memo", 32, 0, p_bottom );

        if( data.m_types[ 0 ] == data.m_types[ 1 ] ) {
            IO::loadTypeIcon( data.m_types[ 0 ], 250 - 32, 50, TYPE_IDX, TYPE_PAL( 0 ),
                              Oam->oamBuffer[ TYPE_IDX ].gfxIndex, p_bottom );
            Oam->oamBuffer[ TYPE_IDX + 1 ].isHidden = true;
        } else {
            IO::loadTypeIcon( data.m_types[ 0 ], 250 - 64, 50, TYPE_IDX, TYPE_PAL( 0 ),
                              Oam->oamBuffer[ TYPE_IDX ].gfxIndex, p_bottom );
            IO::loadTypeIcon( data.m_types[ 1 ], 250 - 32, 50, TYPE_IDX + 1, TYPE_PAL( 1 ),
                              Oam->oamBuffer[ TYPE_IDX + 1 ].gfxIndex, p_bottom );
        }

        IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );

        IO::regularFont->printString( "OT", 110, 30, p_bottom );
        if( currPkmn.m_boxdata.m_oTisFemale ) {
            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->setColor( RED2_IDX, 2 );
        } else {
            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->setColor( BLUE2_IDX, 2 );
        }
        char buffer[ 50 ];
        sprintf( buffer, "%s/%05d", currPkmn.m_boxdata.m_oT, currPkmn.m_boxdata.m_oTId );
        IO::regularFont->printString( buffer, 250, 30, p_bottom, IO::font::RIGHT );

        if( !currPkmn.m_boxdata.isShiny( ) )
            IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
        sprintf( buffer, "%03d", currPkmn.m_boxdata.m_speciesId );
        IO::regularFont->printString( buffer, 180, 51, p_bottom, IO::font::RIGHT );
        IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->printString( "Nr.", 124, 51, p_bottom );

        bool plrOT = currPkmn.m_boxdata.m_oTId == FS::SAV->m_id && currPkmn.m_boxdata.m_oTSid == FS::SAV->m_sid;
        if( !currPkmn.m_boxdata.m_gotDate[ 0 ] )
            sprintf( buffer, "%s%d",
                     plrOT ? "Gef. auf Lv. " : "Off gef auf Lv.",
                     currPkmn.m_boxdata.m_gotLevel );
        else
            sprintf( buffer, "%s",
                     plrOT ? "Ei erhalten" : "Ei offenbar erh." );
        IO::regularFont->printString( buffer, 250, 76, p_bottom, IO::font::RIGHT, 14 );
        sprintf( buffer, "am %02d.%02d.20%02d,",
                 currPkmn.m_boxdata.m_gotDate[ 0 ] ? currPkmn.m_boxdata.m_gotDate[ 0 ] : currPkmn.m_boxdata.m_hatchDate[ 0 ],
                 currPkmn.m_boxdata.m_gotDate[ 1 ] ? currPkmn.m_boxdata.m_gotDate[ 1 ] : currPkmn.m_boxdata.m_hatchDate[ 1 ],
                 currPkmn.m_boxdata.m_gotDate[ 2 ] ? currPkmn.m_boxdata.m_gotDate[ 2 ] : currPkmn.m_boxdata.m_hatchDate[ 2 ] );
        IO::regularFont->printString( buffer, 250, 90, p_bottom, IO::font::RIGHT, 14 );
        sprintf( buffer, "%s.",
                 FS::getLocation( currPkmn.m_boxdata.m_gotPlace ) );
        IO::regularFont->printMaxString( buffer, std::max( (u32) 124, 250 - IO::regularFont->stringWidth( buffer ) ), 104, p_bottom, 254 );

        if( currPkmn.m_boxdata.m_gotDate[ 0 ] ) {
            sprintf( buffer, "Geschl. %02d.%02d.20%02d,",
                     currPkmn.m_boxdata.m_hatchDate[ 0 ],
                     currPkmn.m_boxdata.m_hatchDate[ 1 ],
                     currPkmn.m_boxdata.m_hatchDate[ 2 ] );
            IO::regularFont->printString( buffer, 250, 125, p_bottom, IO::font::RIGHT, 14 );
            sprintf( buffer, "%s.",
                     FS::getLocation( currPkmn.m_boxdata.m_hatchPlace ) );
            IO::regularFont->printString( buffer, 250, 139, p_bottom, IO::font::RIGHT, 14 );
        } else if( plrOT && currPkmn.m_boxdata.m_fateful )
            IO::regularFont->printString( "Schicksalhafte Begeg.", 102, 139, p_bottom );
        else if( currPkmn.m_boxdata.m_fateful )
            IO::regularFont->printString( "Off. schicksal. Begeg.", 102, 139, p_bottom );
    }

    void drawPkmnNature( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );
        IO::regularFont->printString( "Pokémon-Info", 32, 0, p_bottom );

        IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );

        sprintf( buffer, "Mag %s""e PokéRg.", TasteList[ currPkmn.m_boxdata.getTasteStr( ) ].c_str( ) );
        IO::regularFont->printString( buffer, 250, 30, p_bottom, IO::font::RIGHT );

        sprintf( buffer, "Sehr %s; %s.", NatureList[ currPkmn.m_boxdata.getNature( ) ].c_str( ),
                 PersonalityList[ currPkmn.m_boxdata.getPersonality( ) ].c_str( ) );
        auto str = std::string( buffer );
        auto nStr = FS::breakString( str, IO::regularFont, 122 );
        IO::regularFont->printString( nStr.c_str( ), 245, 48, p_bottom, IO::font::RIGHT, 14 );

        auto acAbility = ability( currPkmn.m_boxdata.m_ability );
        u8 wd = IO::regularFont->stringWidth( acAbility.m_abilityName.c_str( ) );
        if( 250 - wd > 140 )
            IO::regularFont->printString( "Fäh. ", 250 - wd, 94, p_bottom, IO::font::RIGHT );
        u8 nlCnt = 0;
        nStr = FS::breakString( acAbility.m_flavourText, IO::regularFont, 130 );
        for( auto c : nStr )
            if( c == '\n' )
                nlCnt++;
        IO::regularFont->printString( nStr.c_str( ), 250, 108, p_bottom, IO::font::RIGHT, u8( 16 - 2 * nlCnt ) );

        if( currPkmn.m_boxdata.m_oTisFemale ) {
            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->setColor( RED2_IDX, 2 );
        } else {
            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->setColor( BLUE2_IDX, 2 );
        }
        IO::regularFont->printString( acAbility.m_abilityName.c_str( ),
                                      250 - wd,
                                      94, p_bottom );
        IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 );
    }

    // Draw extra information about the specified move
    bool statusScreenUI::drawMove( const pokemon& p_pokemon, u8 p_moveIdx, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        auto pal = BG_PAL( p_bottom );

        if( !currPkmn.m_boxdata.m_moves[ p_moveIdx ] )
            return false;
        if( currPkmn.m_boxdata.m_individualValues.m_isEgg )
            return false;

        for( u8 i = 0; i < 4; ++i )
            Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
        for( u8 i = 0; i < 4; ++i )
            Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;

        pal[ WHITE_IDX ] = WHITE;
        pal[ GRAY_IDX ] = RGB( 20, 20, 20 );
        pal[ BLACK_IDX ] = BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        u8 isNotEgg = 1;
        drawPkmnInformation( currPkmn, isNotEgg, false, p_bottom );
        if( !isNotEgg )
            return false; //This should never occur

        IO::regularFont->printString( "Attackeninfos", 32, 0, p_bottom );
        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );

        move* currMove = AttackList[ currPkmn.m_boxdata.m_moves[ p_moveIdx ] ];

        pal[ COLOR_IDX ] = GREEN;
        if( currMove->m_moveType == data.m_types[ 0 ] || currMove->m_moveType == data.m_types[ 1 ] ) {
            IO::regularFont->setColor( COLOR_IDX, 1 );
            IO::regularFont->setColor( WHITE_IDX, 2 );
        } else {
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        }
        IO::regularFont->printString( currMove->m_moveName.c_str( ), 120, 32, p_bottom );
        IO::regularFont->setColor( GRAY_IDX, 1 );
        IO::regularFont->setColor( WHITE_IDX, 2 );

        IO::loadTypeIcon( currMove->m_moveType, 222, 30, TYPE_IDX + p_moveIdx, TYPE_PAL( p_moveIdx ),
                          Oam->oamBuffer[ TYPE_IDX + p_moveIdx ].gfxIndex, p_bottom );
        IO::loadDamageCategoryIcon( currMove->m_moveHitType, 222, 46, ATK_DMGTYPE_IDX( currMove->m_moveHitType ), DMG_TYPE_PAL( currMove->m_moveHitType ),
                                    Oam->oamBuffer[ ATK_DMGTYPE_IDX( currMove->m_moveHitType ) ].gfxIndex, p_bottom );
        char buffer[ 20 ];

        sprintf( buffer, "AP %2hhu""/""%2hhu ", currPkmn.m_boxdata.m_acPP[ p_moveIdx ],
                 currMove->m_movePP * ( ( 5 + ( ( currPkmn.m_boxdata.m_pPUps >> ( 2 * p_moveIdx ) ) % 4 ) ) / 5 ) );
        IO::regularFont->printString( buffer, 128, 47, p_bottom );

        IO::regularFont->printString( "Stärke", 128, 60, p_bottom );
        if( currMove->m_moveBasePower )
            sprintf( buffer, "%3i", currMove->m_moveBasePower );
        else
            sprintf( buffer, "---" );
        IO::regularFont->printString( buffer, 226, 60, p_bottom );

        IO::regularFont->printString( "Genauigkeit", 128, 72, p_bottom );
        if( currMove->m_moveAccuracy )
            sprintf( buffer, "%3i", currMove->m_moveAccuracy );
        else
            sprintf( buffer, "---" );
        IO::regularFont->printString( buffer, 226, 72, p_bottom );

        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->printString( FS::breakString( currMove->description( ), IO::regularFont, 120 ).c_str( ),
                                      128, 84, p_bottom, IO::font::LEFT, 11 );

        IO::updateOAM( p_bottom );
        return true;
    }

    // Draw extra information about the specified ribbon
    bool statusScreenUI::drawRibbon( const pokemon& p_pokemon, u8 p_ribbonIdx, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        auto pal = BG_PAL( p_bottom );

        if( currPkmn.m_boxdata.m_individualValues.m_isEgg )
            return false;

        for( u8 i = 0; i < 4; ++i )
            Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
        for( u8 i = 0; i < 4; ++i )
            Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
        for( u8 i = 0; i < 12; ++i )
            Oam->oamBuffer[ RIBBON_IDX + i ].isHidden = true;

        pal[ WHITE_IDX ] = WHITE;
        pal[ GRAY_IDX ] = RGB( 20, 20, 20 );
        pal[ BLACK_IDX ] = BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        u8 isNotEgg = 1;
        drawPkmnInformation( currPkmn, isNotEgg, false, p_bottom );
        if( isNotEgg == (u8) -1 )
            return false;

        IO::regularFont->printString( "Bandinfos", 32, 4, p_bottom );
        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex,
                        0, -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_0, p_bottom );

        ribbon currRb = RibbonList[ p_ribbonIdx ];

        u16 tileCnt = Oam->oamBuffer[ TYPE_IDX ].gfxIndex;

        if( !IO::loadRibbonIcon( p_ribbonIdx, 226, 28, RIBBON_IDX, TYPE_PAL( 0 ), tileCnt, p_bottom ) )
            tileCnt = IO::loadEggIcon( 226, 28, RIBBON_IDX, TYPE_PAL( 0 ), tileCnt, p_bottom );

        u8 nlCnt = 0;
        auto nStr = FS::breakString( currRb.m_name == "" ? ( "----" ) : currRb.m_name, IO::regularFont, 110 );
        for( auto c : nStr )
            if( c == '\n' )
                nlCnt++;

        if( currRb.m_name == "" )
            currRb.m_description = "----";

        IO::regularFont->printString( nStr.c_str( ), 226, 39 - 7 * nlCnt, p_bottom, IO::font::RIGHT, 14 );
        IO::regularFont->printString( FS::breakString( ( currRb.m_description == "" ) ?
                                                       ( currPkmn.m_boxdata.m_fateful ? "Ein Band für Pokémon-Freunde." : "Ein Gedenk-Band. An einem mysteriösen Ort erhalten." )
                                                       : currRb.m_description, IO::regularFont, 120 ).c_str( ),
                                      250, 66, p_bottom, IO::font::RIGHT, 14 );
        IO::updateOAM( p_bottom );
        return true;
    }
    void regStsScreenUI::draw( const pokemon& p_pokemon, u8 p_page, bool p_newpok ) {
        hideSprites( false );

        IO::setDefaultConsoleTextColors( BG_PALETTE, 230 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PKMNInfoScreen", 420 );
        IO::updateOAM( false );

        IO::regularFont->setColor( 0, 0 );
        { IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 ); }
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = RGB( 20, 20, 20 );
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        if( p_newpok ) {
            BG_PALETTE_SUB[ GRAY_IDX ] = RGB( 10, 10, 10 );
            BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        }
        auto currPkmn = p_pokemon;

        drawPkmnInformation( currPkmn, p_page, p_newpok, false );
        if( p_page == (u8) -1 )
            p_page = 2;
        switch( p_page ) {
            case 0:
                drawPkmnGeneralData( currPkmn, false );
                break;
            case 1:
                drawPkmnNature( currPkmn, false );
                break;
            case 2:
                drawPkmnStats( currPkmn, false );
                break;
            case 3:
                drawPkmnMoves( currPkmn, false );
                break;
            case 4:
                drawPkmnRibbons( currPkmn, false );
                break;
            default:
                return;
        }
        for( u8 i = 0; i < 5; ++i )
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].isHidden = currPkmn.m_boxdata.m_individualValues.m_isEgg;
        IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( p_page ) ].isHidden = !currPkmn.m_boxdata.m_individualValues.m_isEgg;

        if( p_newpok ) {
            IO::Oam->oamBuffer[ FWD_ID ].isHidden = false;
            IO::Oam->oamBuffer[ BWD_ID ].isHidden = false;
        }
        IO::updateOAM( true );
        IO::updateOAM( false );
    }

    std::vector<IO::inputTarget> regStsScreenUI::draw( u8 p_current, bool p_updatePageIcons ) {
        _current = p_current;
        std::vector<IO::inputTarget> res;
        std::vector<std::string> names;

        auto pkmn = FS::SAV->m_pkmnTeam[ p_current ];
        IO::NAV->draw( );
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;

        if( p_updatePageIcons ) {
            for( u8 i = 0; i < 5; ++i )
                IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].isHidden = pkmn.m_boxdata.m_individualValues.m_isEgg;
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( 2 ) ].isHidden = false;
        }

        for( u8 i = 0; i < 6; ++i ) {
            auto pkmn = FS::SAV->m_pkmnTeam[ i ].m_boxdata;
            if( pkmn.m_individualValues.m_isEgg )
                IO::loadEggIcon( 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ), SUB_BALL_IDX( i ),
                                 IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex );
            else if( i == p_current )
                IO::loadPKMNIcon( pkmn.m_speciesId, 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ),
                                  SUB_BALL_IDX( i ), IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex );
            else if( i < FS::SAV->getTeamPkmnCount( ) )
                IO::loadItemIcon( !pkmn.m_ball ? "Pokeball" : ItemList[ pkmn.m_ball ]->m_itemName, 4 - 2 * i,
                                  28 + 24 * i, SUB_BALL_IDX( i ), SUB_BALL_IDX( i ), IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex, true );
            else
                IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].isHidden = true;
        }

        IO::updateOAM( true );
        if( pkmn.m_boxdata.m_individualValues.m_isEgg )
            return res;

        for( u8 i = 0; i < 4; ++i )
            if( pkmn.m_boxdata.m_moves[ i ] < MAX_ATTACK &&
                AttackList[ pkmn.m_boxdata.m_moves[ i ] ]->m_isFieldAttack )
                names.push_back( AttackList[ pkmn.m_boxdata.m_moves[ i ] ]->m_moveName );


        if( pkmn.m_boxdata.m_holdItem )
            names.push_back( "Item nehmen" );
        else
            names.push_back( "Item geben" );
        names.push_back( "Dexeintrag" );
        for( u8 i = 0; i < names.size( ); ++i ) {
            auto sz = res.size( );
            IO::inputTarget cur( 37 + 100 * ( sz % 2 ), 48 + 41 * ( sz / 2 ),
                                 132 + 100 * ( sz % 2 ), 80 + 41 * ( sz / 2 ) );
            IO::printChoiceBox( cur.m_targetX1, cur.m_targetY1, cur.m_targetX2, cur.m_targetY2,
                                6, COLOR_IDX, false );
            IO::regularFont->printString( names[ i ].c_str( ), ( cur.m_targetX2 + cur.m_targetX1 - 2 ) / 2,
                                          cur.m_targetY1 + 8, true, IO::font::CENTER );
            res.push_back( cur );
        }
        return res;
    }

    void boxStsScreenUI::init( ) {
        //Remember: the storage sys swaps the screens.

        IO::regularFont->setColor( 0, 0 );
        { IO::regularFont->setColor( BLACK_IDX, 1 ); IO::regularFont->setColor( GRAY_IDX, 2 ); }
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = RGB( 20, 20, 20 );
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;

        u16 tileCnt = 0;
        IO::Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex = tileCnt;
        tileCnt += 144;

        tileCnt = IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, tileCnt,
                                  0, 0, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadItemIcon( ItemList[ 0 ]->m_itemName, 2, 152,
                                    ITEM_ICON_IDX, ITEM_ICON_PAL, tileCnt );

        for( u8 i = 0; i < 4; ++i ) {
            type t = AttackList[ 0 ]->m_moveType;
            tileCnt = IO::loadTypeIcon( t, 126, 43 + 32 * i, TYPE_IDX + i, TYPE_PAL( i ), tileCnt, true );
        }
        for( u8 i = 0; i < 4; ++i ) {
            tileCnt = IO::loadDamageCategoryIcon( ( move::moveHitTypes )( i % 3 ), 126, 43 + 32 * i,
                                                  ATK_DMGTYPE_IDX( i ), DMG_TYPE_PAL( i % 3 ), tileCnt, true );
        }

        for( u8 i = PKMN_SPRITE_START; i < RIBBON_IDX; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::setDefaultConsoleTextColors( BG_PALETTE_SUB, 230 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "PKMNInfoScreen", 420, 49162, true );
    }

    void boxStsScreenUI::draw( const pokemon& p_pokemon, u8 p_page, bool p_newpok ) {
        //Remember: the storage sys swaps the screens.
        //Only draw on the sub screen
        hideSprites( true );

        IO::setDefaultConsoleTextColors( BG_PALETTE_SUB, 6 );
        drawPkmnInformation( p_pokemon, p_page, p_newpok, true );
        if( p_page == (u8) -1 )
            p_page = 2;
        switch( p_page ) {
            case 0:
                drawPkmnGeneralData( p_pokemon, true );
                break;
            case 1:
                drawPkmnNature( p_pokemon, true );
                break;
            case 2:
                drawPkmnStats( p_pokemon, true );
                break;
            case 3:
                drawPkmnMoves( p_pokemon, true );
                break;
            case 4:
                drawPkmnRibbons( p_pokemon, true );
                break;
            default:
                break;
        }
        for( u8 i = 0; i < 5; ++i )
            IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].isHidden = p_pokemon.m_boxdata.m_individualValues.m_isEgg;
        IO::OamTop->oamBuffer[ PAGE_ICON_START + p_page ].isHidden = !p_pokemon.m_boxdata.m_individualValues.m_isEgg;
        IO::updateOAM( false );
        IO::updateOAM( true );
    }
}
