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

#include "DexTop.h"
#include "DexTop0.h"
#include "DexTop1.h"
#include "DexTop2.h"
#include "DexTop3.h"
#include "DexTop02.h"
#include "DexTop12.h"
#include "DexTop22.h"
#include "DexTop32.h"

#include "DexSub.h"
#include "DexSub2.h"

#include <vector>
#include <algorithm>
#include <cstdio>

namespace DEX {
#define PKMN_SPRITE_START( a ) ( 4 * ( a ) )
#define STAR_START PKMN_SPRITE_START( 8 )

#define PAGE_ICON_START 1 // + 3
#define BG_SPR_START 4 // + 2 * 8
#define FRAME_START 20 // + 32

#define PKMN_ICON_SUB( a ) ( 13 + ( a ) )
#define PKMN_ICON_SUB_PAL( a ) ( 3 + ( a ) )


    dexUI::dexUI( bool p_useInDex, u16 p_maxPkmn )
        : _useInDex( p_useInDex ), _maxPkmn( p_maxPkmn ) {
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
            IO::loadSprite( STAR_START + i, 5, tileCnt, x, 64 + 24 * ( i / 10 ), 16, 16, starPal, starTiles, starTilesLen, false, false, false, OBJPRIORITY_2, false );
        }
        for( u8 i = PKMN_SPRITE_START( 0 ); i <= STAR_START + 30; ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;

        IO::updateOAM( false );

        //Init sub
        IO::NAV->draw( );

        tileCnt = 0;
        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, 0, tileCnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_1, false );

    }

    void dexUI::drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName ) {
        pokemonData data; getAll( FS::SAV->m_lstDex, data );

        u16 tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_formeIdx, 10, 64,
                                          PKMN_SPRITE_START( 0 ), 0, 0, false, false, p_hasGenderDifference && ( _currForme % 2 ), true );
        if( data.m_formecnt )
            IO::boldFont->printString( p_formeName.c_str( ), 58, 157, false, IO::font::CENTER );
        else if( p_hasGenderDifference && ( _currForme % 2 ) )
            IO::boldFont->printString( "weiblich", 58, 157, false, IO::font::CENTER );
        else if( p_hasGenderDifference )
            IO::boldFont->printString( "männlich", 58, 157, false, IO::font::CENTER );
        else
            IO::boldFont->printString( getDisplayName( FS::SAV->m_lstDex ), 58, 157, false, IO::font::CENTER );

        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_formeIdx, 110, 64,
                                      PKMN_SPRITE_START( 1 ), 1, tileCnt, false, true, p_hasGenderDifference && ( _currForme % 2 ) );
        if( data.m_formecnt )
            IO::boldFont->printString( p_formeName.c_str( ), 158, 150, false, IO::font::CENTER );
        else if( p_hasGenderDifference && ( _currForme % 2 ) )
            IO::boldFont->printString( "weiblich", 158, 150, false, IO::font::CENTER );
        else if( p_hasGenderDifference )
            IO::boldFont->printString( "männlich", 158, 150, false, IO::font::CENTER );
        else
            IO::boldFont->printString( getDisplayName( FS::SAV->m_lstDex ), 158, 150, false, IO::font::CENTER );
        IO::boldFont->printString( "(schillernd)", 158, 166, false, IO::font::CENTER );

        //Load Icons of the other formes ( max 4 )
        if( !data.m_formecnt ) {
            for( u8 i = PKMN_SPRITE_START( 3 ); i <= PKMN_SPRITE_START( 4 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            IO::updateOAM( false );
            return;
        }
        u8 currpos = ( _currForme / ( 1 + p_hasGenderDifference ) ) % data.m_formecnt;
        tileCnt = IO::OamTop->oamBuffer[ STAR_START ].gfxIndex + 64;
        for( u8 i = 0; i < u16( std::min( 4, data.m_formecnt - 1 ) ); ++i ) {
            currpos = ( currpos + 1 ) % data.m_formecnt;
            tileCnt = IO::loadPKMNIcon( data.m_formeIdx[ currpos ], 210, 150 - 35 * i, PKMN_SPRITE_START( 3 ) + i, 6 + i, tileCnt, false );
        }
        for( u8 i = std::min( u16( 4 ), data.m_formecnt ); i < 4u; ++i )
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 3 ) + i ].isHidden = true;
        IO::updateOAM( false );
    }

    void dexUI::drawPage( u16 p_pkmnIdx, u8 p_page ) {
        if( !p_pkmnIdx )
            p_pkmnIdx = _maxPkmn;

        if( _useInDex && !IN_DEX( p_pkmnIdx ) ) {
            IO::NAV->draw( );
            dmaCopy( DexTopBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
            dmaCopy( DexTopPal, BG_PALETTE, 480 );
            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", 0, 80, 35, PKMN_SPRITE_START( 0 ), 0, 0, false );

            IO::boldFont->setColor( BLACK_IDX, 2 );
            IO::boldFont->setColor( WHITE_IDX, 1 );
            BG_PALETTE[ WHITE_IDX ] = WHITE;
            BG_PALETTE[ BLACK_IDX ] = BLACK;
            IO::boldFont->printString( "Keine Daten.", 128, 150, false, IO::font::CENTER );
            return;
        }

        pokemonData data; getAll( p_pkmnIdx, data );

        bool isFixed = !FS::exists( "nitro:/PICS/SPRITES/PKMN/", p_pkmnIdx, "f" );

        if( p_page <= 1 )
            _currForme %= data.m_formecnt ? ( ( 2 - isFixed ) * data.m_formecnt ) : ( 2 - isFixed );

        u16 currFormeIdx = data.m_formecnt ? data.m_formeIdx[ _currForme / ( 2 - isFixed ) ] : p_pkmnIdx;
        sprintf( buffer, "%s", data.m_formecnt ? data.m_formeName[ _currForme / ( 2 - isFixed ) ] : data.m_displayName );
        std::string formeName = std::string( buffer );

        if( currFormeIdx != p_pkmnIdx )
            getAll( currFormeIdx, data );


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

        for( u8 i = 0; i < PKMN_SPRITE_START( 5 ); ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;

        //Draw top screen's background

        if( p_page <= 1 ) {
            switch( data.m_stage ) {
                case 0:
                    if( p_page == 0 ) {
                        dmaCopy( DexTop0Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop0Pal + 64, BG_PALETTE + 64, 256 );
                    } else {
                        dmaCopy( DexTop02Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop02Pal + 64, BG_PALETTE + 64, 256 );
                    }
                    IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].isHidden = true;
                    break;
                case 1:
                    if( p_page == 0 ) {
                        dmaCopy( DexTop1Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop1Pal + 64, BG_PALETTE + 64, 256 );
                    } else {
                        dmaCopy( DexTop12Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop12Pal + 64, BG_PALETTE + 64, 256 );
                    }
                    IO::loadPKMNIcon( data.m_preEvolution, 8, 20, PKMN_SPRITE_START( 2 ), 2,
                                      IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );
                    consoleSetWindow( &IO::Top, 6, 4, 15, 1 );
                    printf( "aus %s", getDisplayName( data.m_preEvolution ) );
                    break;
                case 2:
                    if( p_page == 0 ) {
                        dmaCopy( DexTop2Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop2Pal + 64, BG_PALETTE + 64, 256 );
                    } else {
                        dmaCopy( DexTop22Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop22Pal + 64, BG_PALETTE + 64, 256 );
                    }
                    IO::loadPKMNIcon( data.m_preEvolution, 8, 20, PKMN_SPRITE_START( 2 ), 2,
                                      IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );
                    consoleSetWindow( &IO::Top, 6, 4, 15, 1 );
                    printf( "aus %s", getDisplayName( data.m_preEvolution ) );
                    break;
                case 3:
                    if( p_page == 0 ) {
                        dmaCopy( DexTop3Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop3Pal + 64, BG_PALETTE + 64, 256 );
                    } else {
                        dmaCopy( DexTop32Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                        dmaCopy( DexTop32Pal + 64, BG_PALETTE + 64, 256 );
                    }
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

            BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );
            BG_PALETTE[ WHITE_IDX ] = WHITE;
            BG_PALETTE[ BLACK_IDX ] = BLACK;

            IO::boldFont->printString( data.m_displayName, 58, 11, false );

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

            consoleSetWindow( &IO::Top, 28, 1, 32, 2 );
            printf( "%03d", p_pkmnIdx );
            consoleSetWindow( &IO::Top, 28, 2, 32, 3 );
            printf( "%3d", FS::SAV->countPkmn( p_pkmnIdx ) );
        }

        switch( p_page ) {
            case 0:
            {
                IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", currFormeIdx, 80, 35, PKMN_SPRITE_START( 0 ), 0, 0, false );

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
                IO::regularFont->printString( "KP", 80, 49, false, IO::font::RIGHT );
                IO::regularFont->printString( "Angriff", 80, 74, false, IO::font::RIGHT );
                IO::regularFont->printString( "Sp.-Angr", 80, 99, false, IO::font::RIGHT );

                IO::regularFont->printString( "Initiative", 176, 49, false );
                IO::regularFont->printString( "Vert", 176, 74, false );
                IO::regularFont->printString( "Sp.-Vert", 176, 99, false );

                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( WHITE_IDX, 2 );
                IO::regularFont->printString( FS::breakString( data.m_dexEntry, IO::regularFont, 224 ).c_str( ), 16, 134, false, IO::font::LEFT, 12 );

                consoleSetWindow( &IO::Top, 3, 22, 32, 23 );
                if( strlen( data.m_species ) >= 13 )
                    consoleSetWindow( &IO::Top, 2, 22, 32, 23 );
                if( strlen( data.m_species ) == 8 || strlen( data.m_species ) == 9 ) printf( " " );
                printf( "%8s %4.1fm %5.1fkg", FS::convertToOld( data.m_species ).c_str( ), data.m_size / 10.0, data.m_weight / 10.0 );
                break;
            }
            case 1:
            {
                for( u8 i = 0; i < 30; ++i )
                    IO::OamTop->oamBuffer[ i + STAR_START ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;

                isFixed = !isFixed;
                drawFormes( currFormeIdx, isFixed, formeName );
                break;
            }
            case 2:
            {
                if( _currForme % 3 == 0 )
                    FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "HoennMap" );
                else if( _currForme % 3 == 1 )
                    FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "KantoMap" );
                else
                    FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "JohtoMap" );

                BG_PALETTE[ 251 ] = RGB15( 3, 3, 3 );
                BG_PALETTE[ WHITE_IDX ] = WHITE;
                BG_PALETTE[ BLACK_IDX ] = BLACK;

                IO::boldFont->printString( data.m_displayName, 32, 4, false );
                IO::loadPKMNIcon( p_pkmnIdx, 0, -4, PKMN_SPRITE_START( 2 ), 2,
                                  IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );

                for( u8 i = 0; i < 30; ++i )
                    IO::OamTop->oamBuffer[ i + STAR_START ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) + 1 ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) + 2 ].isHidden = true;
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) + 3 ].isHidden = true;
                break;
            }
            default:
                break;
        }

        IO::updateOAM( false );
        return;
    }

    s8 dexUI::drawSub( u8 p_mode, u16 p_pkmnIdcs[ 32 ], u16 p_idxStart, u8 p_selectedIdx ) {

    }

    void dexUI::changeMode( u8 p_newMode ) {

    }

    void dexUI::select( u8 p_idx ) {

    }
}
