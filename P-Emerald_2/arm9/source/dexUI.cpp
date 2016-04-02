/*
Pokémon Emerald 2 Version
------------------------------

file        : dexUI.cpp
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

#include "dexUI.h"
#include "dex.h"
#include "uio.h"
#include "sprite.h"
#include "nav.h"
#include "fs.h"
#include "saveGame.h"
#include "pokemon.h"
#include "defines.h"

#include "Back.h"
#include "BagSpr.h"
#include "BigCirc1.h"
#include "memo.h"
#include "PKMN.h"
#include "time_icon.h"
#include "star.h"

#include "DexTop0.h"
#include "DexTop1.h"
#include "DexTop2.h"
#include "DexTop3.h"

#include <vector>
#include <algorithm>
#include <cstdio>

namespace DEX {
#define PKMN_SPRITE_START(a) (4*(a))
#define STAR_START PKMN_SPRITE_START( 5 )

#define BAG_SPR_SUB(a) (1 + (a))
#define BIG_CIRC_START 9
#define PKMN_ICON_SUB(a) (13 + (a))
#define PAGE_ICON_START 22

#define PKMN_ICON_SUB_PAL(a) (3 + (a))
#define PKMN_SPRITE_SUB_PAL 8
#define PAGE_ICON_PAL_START 9


    void dexUI::init( ) {
        IO::vramSetup( );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );

        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );

        IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );

        consoleSelect( &IO::Top );
        printf( "\x1b[39m" );

        //Initialize the top screen
        IO::initOAMTable( false );

        u16 tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", FS::SAV->m_lstDex, 80, 64, PKMN_SPRITE_START( 0 ), 0, 0, false );
        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", FS::SAV->m_lstDex, 80, 64, PKMN_SPRITE_START( 1 ), 1, tileCnt, false );

        tileCnt = IO::loadPKMNIcon( FS::SAV->m_lstDex, 0, 8, PKMN_SPRITE_START( 2 ), 2, 288, false );
        tileCnt = IO::loadTypeIcon( type( 0 ), 33, 35, PKMN_SPRITE_START( 2 ) + 1, 3, tileCnt, false );
        tileCnt = IO::loadTypeIcon( type( 0 ), 33, 35, PKMN_SPRITE_START( 2 ) + 2, 4, tileCnt, false );
        tileCnt = IO::loadPKMNIcon( FS::SAV->m_lstDex, 0, 8, PKMN_SPRITE_START( 3 ), 5, tileCnt, false );
        for( u8 i = 0; i < 30; ++i ) {
            u8 x = 67 - 13 * ( i % 10 );
            if( i % 10 >= 5 )
                x = 176 + 13 * ( i % 5 );
            IO::loadSprite( STAR_START + i, 6, tileCnt, x, 64 + 24 * ( i / 10 ), 16, 16, starPal, starTiles, starTilesLen, false, false, false, OBJPRIORITY_2, false );
        }
        for( u8 i = PKMN_SPRITE_START( 0 ); i <= STAR_START + 30; ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;

        IO::updateOAM( false );

        //Initialize the subScreen
        IO::initOAMTable( true );

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, 0, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

        for( u8 i = 0; i < 5; ++i ) {
            tileCnt = IO::loadSprite( BAG_SPR_SUB( i ), 1, tileCnt, dexsppos[ 0 ][ i ], dexsppos[ 1 ][ i ], 32, 32, BagSprPal,
                                      BagSprTiles, BagSprTilesLen, false, false, !( _useInDex ), OBJPRIORITY_2, true );
            tileCnt = IO::loadPKMNIcon( 0, dexsppos[ 0 ][ i ], dexsppos[ 1 ][ i ], PKMN_ICON_SUB( i ), PKMN_ICON_SUB_PAL( i ), tileCnt, true );
            IO::Oam->oamBuffer[ PKMN_ICON_SUB( i ) ].isHidden = !( _useInDex );
        }

        tileCnt = IO::loadSprite( BIG_CIRC_START, 2, tileCnt, 8, 32, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, false, false, false, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( BIG_CIRC_START + 1, 2, tileCnt, 72, 32, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, false, true, false, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( BIG_CIRC_START + 2, 2, tileCnt, 8, 96, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, true, false, false, OBJPRIORITY_2, true );
        tileCnt = IO::loadSprite( BIG_CIRC_START + 3, 2, tileCnt, 72, 96, 64, 64, BigCirc1Pal,
                                  BigCirc1Tiles, BigCirc1TilesLen, true, true, false, OBJPRIORITY_2, true );

        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", FS::SAV->m_lstDex, dexsppos[ 0 ][ 8 ] + 16,
                                      dexsppos[ 1 ][ 8 ] + 16, PKMN_ICON_SUB( 5 ), PKMN_SPRITE_SUB_PAL, tileCnt, true );

        tileCnt = IO::loadSprite( PAGE_ICON_START, PAGE_ICON_PAL_START, tileCnt, dexsppos[ 0 ][ 5 ], dexsppos[ 1 ][ 5 ], 32, 32, memoPal,
                                  memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( PAGE_ICON_START + 1, PAGE_ICON_PAL_START + 1, tileCnt, dexsppos[ 0 ][ 6 ], dexsppos[ 1 ][ 6 ], 32, 32, time_iconPal,
                                  time_iconTiles, time_iconTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( PAGE_ICON_START + 2, PAGE_ICON_PAL_START + 2, tileCnt, dexsppos[ 0 ][ 7 ], dexsppos[ 1 ][ 7 ], 32, 32, PKMNPal,
                                  PKMNTiles, PKMNTilesLen, false, false, false, OBJPRIORITY_0, true );

        IO::updateOAM( true );
    }

    void dexUI::drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName ) {
        IO::boldFont->setColor( WHITE_IDX, 2 );
        IO::boldFont->setColor( 0, 1 );

        if( p_formeIdx == u16( -1 ) ) {
            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", 0, 80, 64, PKMN_SPRITE_START( 0 ), 0, 0, false );
            IO::boldFont->printString( "Keine Daten.", 90, 167, false );
            for( u8 i = PKMN_SPRITE_START( 1 ); i < PKMN_SPRITE_START( 2 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            for( u8 i = PKMN_SPRITE_START( 3 ); i <= PKMN_SPRITE_START( 4 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            IO::updateOAM( false );
            return;
        }

        pokemonData data; getAll( FS::SAV->m_lstDex, data );
        if( data.m_types[ 0 ] == NORMAL
            || data.m_types[ 0 ] == GROUND
            || data.m_types[ 0 ] == ICE )
            IO::boldFont->setColor( BLACK_IDX, 2 );

        //IO::boldFont->setColor( WHITE_IDX, 2 );
        //IO::boldFont->setColor( COLOR_IDX, 2 );

        u16 tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_formeIdx, 10, 64,
                                          PKMN_SPRITE_START( 0 ), 0, 0, false, false, p_hasGenderDifference && ( _currForme % 2 ), true );
        if( data.m_formecnt )
            IO::boldFont->printString( p_formeName.c_str( ), 5, 167, false );
        else if( p_hasGenderDifference && ( _currForme % 2 ) )
            IO::boldFont->printString( "weiblich", 20, 167, false );
        else if( p_hasGenderDifference )
            IO::boldFont->printString( "männlich", 20, 167, false );
        else
            IO::boldFont->printString( getDisplayName( FS::SAV->m_lstDex ), 10, 167, false );

        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_formeIdx, 110, 64,
                                      PKMN_SPRITE_START( 1 ), 1, tileCnt, false, true, p_hasGenderDifference && ( _currForme % 2 ) );
        if( data.m_formecnt )
            IO::boldFont->printString( p_formeName.c_str( ), 110, 160, false );
        else if( p_hasGenderDifference && ( _currForme % 2 ) )
            IO::boldFont->printString( "weiblich", 130, 160, false );
        else if( p_hasGenderDifference )
            IO::boldFont->printString( "männlich", 130, 160, false );
        else
            IO::boldFont->printString( getDisplayName( FS::SAV->m_lstDex ), 110, 160, false );
        IO::boldFont->printString( "(schillernd)", 110, 176, false );

        //Load Icons of the other formes ( max 4 )
        if( !data.m_formecnt ) {
            for( u8 i = PKMN_SPRITE_START( 3 ); i <= PKMN_SPRITE_START( 4 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            IO::updateOAM( false );
            return;
        }
        u8 currpos = ( _currForme / ( 1 + p_hasGenderDifference ) ) % data.m_formecnt;
        tileCnt = IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 3 ) ].gfxIndex;
        for( u8 i = 0; i < u16( std::min( 4, data.m_formecnt - 1 ) ); ++i ) {
            currpos = ( currpos + 1 ) % data.m_formecnt;
            tileCnt = IO::loadPKMNIcon( data.m_formeIdx[ currpos ], 210, 150 - 35 * i, PKMN_SPRITE_START( 3 ) + i, 5 + i, tileCnt, false );
        }
        for( u8 i = std::min( u16( 4 ), data.m_formecnt ); i < 4u; ++i )
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 3 ) + i ].isHidden = true;
        IO::updateOAM( false );
    }

    u16 oldPkmn;
    u8 oldForme = -1;
    void dexUI::undrawFormes( u16 p_formeIdx ) {
        //Print over the text the same text, but with the color of the background
        pokemonData data2; getAll( FS::SAV->m_lstDex, data2 );
        if( !( _useInDex ) || IN_DEX( FS::SAV->m_lstDex ) ) {
            BG_PALETTE[ 0 ] = IO::getColor( data2.m_types[ 0 ] );
            BG_PALETTE[ COLOR_IDX ] = IO::getColor( data2.m_types[ 0 ] );
        } else {
            BG_PALETTE[ 0 ] = BLACK;
            BG_PALETTE[ COLOR_IDX ] = BLACK;
        }
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        IO::regularFont->setColor( WHITE_IDX, 0 );
        IO::regularFont->setColor( WHITE_IDX, 1 );
        IO::regularFont->setColor( WHITE_IDX, 2 );
        pokemonData data3; getAll( oldPkmn, data3 );
        sprintf( buffer, "%s", data3.m_displayName );
        sprintf( buffer, "%s - %s", buffer, data3.m_species );
        IO::regularFont->printString( std::string( 52, ' ' ).c_str( ), 36, 20, false );

        IO::regularFont->setColor( COLOR_IDX, 0 );
        IO::regularFont->setColor( COLOR_IDX, 1 );
        IO::regularFont->setColor( COLOR_IDX, 2 );
        if( p_formeIdx == u16( -1 ) ) {
            IO::regularFont->printString( std::string( 40, ' ' ).c_str( ), 90, 167, false );
            return;
        }
        IO::regularFont->printString( std::string( 40, ' ' ).c_str( ), 5, 167, false );
        IO::regularFont->printString( std::string( 40, ' ' ).c_str( ), 110, 160, false );
        IO::regularFont->printString( std::string( 40, ' ' ).c_str( ), 110, 176, false );

    }

    u16 newformepkmn = -1;
    std::string formeName;
    bool isFixed;
    void dexUI::drawPage( bool p_newPok, bool p_newPage ) {
        if( !FS::SAV->m_lstDex )
            FS::SAV->m_lstDex = _maxPkmn;
        pokemonData data; getAll( FS::SAV->m_lstDex, data );
        isFixed = !FS::exists( "nitro:/PICS/SPRITES/PKMN/", FS::SAV->m_lstDex, "f" );
        if( _currPage != 1 )
            _currForme %= data.m_formecnt ? ( ( isFixed ? 1 : 2 ) * data.m_formecnt ) : ( isFixed ? 1 : 2 );
        if( !p_newPok && !p_newPage  && !data.m_formecnt && isFixed && _currPage != 1 )
            return; //Nothing changed, so just do nothing.

        //Redraw the subscreen iff p_newPok
        if( p_newPok ) {
            IO::NAV->draw( );

            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", ( !( _useInDex ) || IN_DEX( FS::SAV->m_lstDex ) ) ? FS::SAV->m_lstDex : 0, dexsppos[ 0 ][ 8 ] + 16,
                                dexsppos[ 1 ][ 8 ] + 16, PKMN_ICON_SUB( 5 ), PKMN_SPRITE_SUB_PAL,
                                IO::Oam->oamBuffer[ PKMN_ICON_SUB( 5 ) ].gfxIndex, true );
            if( _useInDex ) {
                u16 pidx = ( FS::SAV->m_lstDex + _maxPkmn - 5 ) % _maxPkmn;
                for( u8 i = 0; i < 5; ++i ) {
                    if( ( ( ++pidx ) %= _maxPkmn ) == FS::SAV->m_lstDex - 1 )
                        pidx = ( pidx + 1 ) % _maxPkmn;
                    IO::loadPKMNIcon( IN_DEX( pidx + 1 ) ? ( pidx + 1 ) : 0, dexsppos[ 0 ][ i ],
                                      dexsppos[ 1 ][ i ], PKMN_ICON_SUB( i ), PKMN_ICON_SUB_PAL( i ),
                                      IO::Oam->oamBuffer[ PKMN_ICON_SUB( i ) ].gfxIndex, true );
                }
            }
        }
        for( u8 i = 0; i < 3; ++i )
            IO::Oam->oamBuffer[ i + PAGE_ICON_START ].isHidden = ( i == _currPage );

        IO::updateOAM( true );



        consoleSelect( &IO::Top );
        consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
        consoleClear( );
        printf( "\x1b[39m" );
        //Init some colors
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( WHITE_IDX, 2 );
        IO::boldFont->setColor( BLACK_IDX, 2 );
        IO::boldFont->setColor( WHITE_IDX, 1 );

        //Draw top screen's background
        if( _currPage == 0 ) {
            switch( data.m_stage ) {
                case 0:
                    dmaCopy( DexTop0Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                    dmaCopy( DexTop0Pal + 64, BG_PALETTE + 64, 256 );
                    break;
                case 1:
                    dmaCopy( DexTop1Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                    dmaCopy( DexTop1Pal + 64, BG_PALETTE + 64, 256 );
                    IO::loadPKMNIcon( data.m_preEvolution, 8, 20, PKMN_SPRITE_START( 2 ), 2,
                                      IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );
                    consoleSetWindow( &IO::Top, 6, 4, 15, 1 );
                    printf( "aus %s", getDisplayName( data.m_preEvolution ) );
                    break;
                case 2:
                    dmaCopy( DexTop2Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                    dmaCopy( DexTop2Pal + 64, BG_PALETTE + 64, 256 );
                    IO::loadPKMNIcon( data.m_preEvolution, 8, 20, PKMN_SPRITE_START( 2 ), 2,
                                      IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );
                    consoleSetWindow( &IO::Top, 6, 4, 15, 1 );
                    printf( "aus %s", getDisplayName( data.m_preEvolution ) );
                    break;
                case 3:
                    dmaCopy( DexTop3Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                    dmaCopy( DexTop3Pal + 64, BG_PALETTE + 64, 256 );
                    IO::loadItemIcon( ItemList[ data.m_preEvolution ]->m_itemName, 8, 20, PKMN_SPRITE_START( 2 ), 2,
                                      IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );
                    consoleSetWindow( &IO::Top, 6, 4, 15, 1 );
                    printf( "aus %s", ItemList[ data.m_preEvolution ]->m_itemName.c_str( ) );
                    break;
                default:
                    break;
            }
            switch( data.m_types[ 0 ] ) {
                case NORMAL:
                case FLYING:
                case UNKNOWN:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Normal", 128 );
                    break;
                case FIGHTING:
                case GROUND:
                case ROCK:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Fighting", 128 );
                    break;
                case POISON:
                case GHOST:
                case PSYCHIC:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Psychic", 128 );
                    break;
                case BUG:
                case GRASS:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Grass", 128 );
                    break;
                case STEEL:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Metal", 128 );
                    break;
                case WATER:
                case ICE:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Water", 128 );
                    break;
                case FIRE:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Fire", 128 );
                    break;
                case LIGHTNING:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Lightning", 128 );
                    break;
                case DRAGON:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Dragon", 128 );
                    break;
                case DARKNESS:
                    FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Dark", 128 );
                    IO::regularFont->setColor( BLACK_IDX, 2 );
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    if( data.m_stage == 0 || data.m_stage == 3 ) {
                        IO::boldFont->setColor( BLACK_IDX, 1 );
                        IO::boldFont->setColor( WHITE_IDX, 2 );
                    }
                    break;
                default:
                    break;
            }
        } else if( _currPage == 1 ) {
            if( _currForme % 3 == 0 )
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "HoennMap" );
            else if( _currForme % 3 == 1 )
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "KantoMap" );
            else
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "JohtoMap" );
        }

        BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        IO::boldFont->printString( data.m_displayName, 58, 11, false );

        consoleSetWindow( &IO::Top, 28, 1, 32, 2 );
        printf( "%03d", FS::SAV->m_lstDex );
        consoleSetWindow( &IO::Top, 28, 2, 32, 3 );
        printf( "%3d", FS::SAV->countPkmn( FS::SAV->m_lstDex ) );

        if( data.m_types[ 0 ] != data.m_types[ 1 ] ) {
            IO::loadTypeIcon( data.m_types[ 0 ], 183, 28, PKMN_SPRITE_START( 2 ) + 1, 3,
                              IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].gfxIndex, false );
            IO::loadTypeIcon( data.m_types[ 1 ], 215, 28, PKMN_SPRITE_START( 2 ) + 2, 4,
                              IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].gfxIndex, false );
        } else {
            IO::loadTypeIcon( data.m_types[ 0 ], 199, 28, PKMN_SPRITE_START( 2 ) + 1, 3,
                              IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].gfxIndex, false );
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;
        }

        switch( _currPage ) {
            case 0:
            {
                IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", FS::SAV->m_lstDex, 80, 35, PKMN_SPRITE_START( 0 ), 0, 0, false );

                for( u8 i = 0; i < 30; ++i ) {
                    u8 bs;
                    if( i / 5 == 0 )
                        bs = data.m_bases[ i / 5 ];
                    else if( i / 5 == 1 )
                        bs = data.m_bases[ 5 ];
                    else
                        bs = data.m_bases[ i / 5 - 1 ];

                    IO::OamTop->oamBuffer[ i + STAR_START ].isHidden = ( bs < 60 + 20 * ( i % 5 ) );
                }
                IO::regularFont->printString( "KP", 80 - IO::regularFont->stringWidth( "KP" ), 49, false );
                IO::regularFont->printString( "Angriff", 80 - IO::regularFont->stringWidth( "Angriff" ), 74, false );
                IO::regularFont->printString( "Sp.-Angr", 80 - IO::regularFont->stringWidth( "Sp.-Angr" ), 99, false );

                IO::regularFont->printString( "Initiative", 176, 49, false );
                IO::regularFont->printString( "Vert", 176, 74, false );
                IO::regularFont->printString( "Sp.-Vert", 176, 99, false );

                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( WHITE_IDX, 2 );
                IO::regularFont->printString( FS::breakString( data.m_dexEntry, IO::regularFont, 224 ).c_str( ), 16, 134, false, 12 );

                consoleSetWindow( &IO::Top, 3, 22, 32, 23 );
                if( strlen( data.m_species ) >= 13 )
                    consoleSetWindow( &IO::Top, 2, 22, 32, 23 );
                if( strlen( data.m_species ) == 8 || strlen( data.m_species ) == 9 ) printf( " " );
                printf( "%8s %4.1fm %5.1fkg", FS::convertToOld( data.m_species ).c_str( ), data.m_size / 10.0, data.m_weight / 10.0 );
                break;
            }
            default:
                break;
        }

        IO::updateOAM( false );
        return;
        newformepkmn = FS::SAV->m_lstDex;
        for( u8 i = 0; i < PKMN_SPRITE_START( 5 ); ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;
        char buffer[ 50 ];
        if( _currPage != 1 ) {
            if( !( _useInDex ) || IN_DEX( FS::SAV->m_lstDex ) ) {
                BG_PALETTE[ 0 ] = IO::getColor( data.m_types[ 0 ] );
                sprintf( buffer, "%s - %s", buffer, data.m_species );
                IO::regularFont->printString( buffer, 36, 20, false );
                if( !_currPage ) {
                    consoleSetWindow( &IO::Top, 1, 16, 30, 24 );
                    printf( "%s", data.m_dexEntry );
                    consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
                }

                _currForme %= data.m_formecnt ? ( ( isFixed ? 1 : 2 ) * data.m_formecnt ) : ( isFixed ? 1 : 2 );

                newformepkmn = data.m_formecnt ? data.m_formeIdx[ _currForme / ( isFixed ? 1 : 2 ) ] : FS::SAV->m_lstDex;
                sprintf( buffer, "%s", data.m_formecnt ? data.m_formeName[ _currForme / ( isFixed ? 1 : 2 ) ] : data.m_displayName );
                formeName = std::string( buffer );

                if( data.m_formecnt )
                    getAll( newformepkmn, data );

                //BG_PALETTE[ 1 ] = IO::getColor( data.m_types[ 0 ] );
                IO::loadPKMNIcon( ( FS::SAV->m_lstDex == 493 || FS::SAV->m_lstDex == 649 ) ? FS::SAV->m_lstDex : newformepkmn, 0, 8,
                                  PKMN_SPRITE_START( 2 ), 2, IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );

                IO::loadTypeIcon( data.m_types[ 0 ], 33, 35, PKMN_SPRITE_START( 2 ) + 1, 3,
                                  IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].gfxIndex, false );
                if( data.m_types[ 0 ] != data.m_types[ 1 ] ) {
                    IO::loadTypeIcon( data.m_types[ 1 ], 65, 35, PKMN_SPRITE_START( 2 ) + 2, 4,
                                      IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].gfxIndex, false );
                } else
                    IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;
                printf( "\n    Du hast %2hu dieser Pok\x82""mon.\n\n", FS::SAV->countPkmn( FS::SAV->m_lstDex ) );
                printf( "\n\n %03i", FS::SAV->m_lstDex );
            } else {
                getAll( 0, data );
                printf( "\n    Keine Daten vorhanden.\n\n" );
                sprintf( buffer, "???????????? - %s", data.m_species );
                IO::regularFont->printString( buffer, 36, 20, false );
                printf( "\n\n %03i", FS::SAV->m_lstDex );
            }
        }

        switch( _currPage ) {
            case 0:
            {
                printf( "\x1b[37m" );
                if( !( _useInDex ) || IN_DEX( FS::SAV->m_lstDex ) ) {
                    BG_PALETTE[ COLOR_IDX ] = IO::getColor( data.m_types[ 1 ] );
                    for( u8 i = 0; i < 6; ++i ) {
                        IO::printRectangle( u8( 19 + 40 * i ), u8( std::max( 56, 102 - data.m_bases[ i ] / 3 ) ),
                                            u8( 37 + 40 * i ), (u8) 102, false, true );
                    }
                    printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                    sprintf( buffer, "GW.  %5.1fkg", data.m_weight / 10.0 );
                    IO::regularFont->printString( buffer, 10, 109, false );
                    sprintf( buffer, "GR.  %6.1fm", data.m_size / 10.0 );
                    IO::regularFont->printString( buffer, 100, 109, false );
                } else {
                    printf( "\n\n  KP   ANG  DEF  SAN  SDF  INT\n\n\n\n\n\n\n\n\n" );
                    sprintf( buffer, "GW.  ???.?kg" );
                    IO::regularFont->printString( buffer, 10, 109, false );
                    sprintf( buffer, "GR.  ???.?m" );
                    IO::regularFont->printString( buffer, 100, 109, false );
                    consoleSetWindow( &IO::Top, 1, 16, 30, 24 );
                    printf( "%s", data.m_dexEntry );
                }
                break;
            }
            case 2:
            {
                if( !( _useInDex ) || IN_DEX( FS::SAV->m_lstDex ) ) {
                    isFixed = !isFixed;
                    oldForme = _currForme;
                    oldPkmn = FS::SAV->m_lstDex;
                    drawFormes( newformepkmn, isFixed, formeName );
                } else {
                    oldForme = _currForme;
                    oldPkmn = FS::SAV->m_lstDex;
                    drawFormes( newformepkmn = -1, isFixed = 0, formeName );
                }
            }
        }

        IO::updateOAM( false );
    }
}
