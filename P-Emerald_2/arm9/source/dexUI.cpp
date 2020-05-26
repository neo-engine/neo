/*
Pokémon neo
------------------------------

file        : dexUI.cpp
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

#include "dexUI.h"
#include "defines.h"
#include "dex.h"
#include "fs.h"
#include "nav.h"
#include "pokemon.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

#include "Back.h"
#include "BagSpr.h"
#include "BigCirc1.h"
#include "PKMN.h"
#include "memo.h"
#include "star.h"
#include "time_icon.h"

#include "DexTop.h"
#include "DexTop0.h"
#include "DexTop02.h"
#include "DexTop1.h"
#include "DexTop12.h"
#include "DexTop2.h"
#include "DexTop22.h"
#include "DexTop3.h"
#include "DexTop32.h"

#include "DexSub.h"
#include "DexSub2.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace DEX {
#define PKMN_SPRITE_START( a ) ( 4 * ( a ) )
#define STAR_START PKMN_SPRITE_START( 8 )

    void setAllVis( bool p_vis ) {
        for( u8 i = PKMN_ICON_START; i <= BG_SPR_START + 16; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = p_vis;
    }
    void setCghtVis( bool p_vis ) {
        for( u8 i = 0; i < 5; ++i ) {
            IO::Oam->oamBuffer[ FRAME_START_2 + i ].isHidden     = p_vis;
            IO::Oam->oamBuffer[ PKMN_ICON_START_2 + i ].isHidden = p_vis;
        }
    }
    void loadPkmnIconToSlot( u16 p_pkmnIdx, u8 p_slot, bool p_showAll ) {
        if( !p_showAll ) {
            IO::loadPKMNIcon( p_pkmnIdx, 64, 18 + 32 * p_slot, PKMN_ICON_START_2 + p_slot, 1,
                              p_slot, IO::Oam->oamBuffer[ PKMN_ICON_START_2 + p_slot ].gfxIndex );
        } else {
            u16 shf = 2 + 4 * !!( IO::Oam->oamBuffer[ BG_SPR_START + 2 * 3 ].x == 44 );
            if( p_slot >= shf && p_slot < 30 ) {
                IO::loadPKMNIcon( p_pkmnIdx, IO::Oam->oamBuffer[ FRAME_START + p_slot ].x,
                                  IO::Oam->oamBuffer[ FRAME_START + p_slot ].y - 6,
                                  PKMN_ICON_START + p_slot, 1 + p_slot / 16, p_slot % 16,
                                  IO::Oam->oamBuffer[ PKMN_ICON_START_2 ].gfxIndex
                                      + 31 * ( p_slot - shf ),
                                  true );
                IO::Oam->oamBuffer[ PKMN_ICON_START + p_slot ].priority = OBJPRIORITY_3;
            }
        }
    }

    void moveAll( s8 p_x ) {
        for( u8 i = PKMN_ICON_START; i <= BG_SPR_START + 16; ++i ) IO::Oam->oamBuffer[ i ].x += p_x;
    }

    dexUI::dexUI( bool p_useInDex, u16 p_maxPkmn )
        : _useInDex( p_useInDex ), _maxPkmn( p_maxPkmn ) {
        IO::vramSetup( );
        videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                         | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );

        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );

        IO::Bottom
            = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );

        consoleSelect( &IO::Top );
        printf( "\x1b[39m" );

        // Initialize the top screen
        IO::initOAMTable( false );

        u16 tileCnt
            = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", SAVE::SAV.getActiveFile( ).m_lstDex,
                                  80, 64, PKMN_SPRITE_START( 0 ), 0, 0, false );
        tileCnt
            = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", SAVE::SAV.getActiveFile( ).m_lstDex,
                                  80, 64, PKMN_SPRITE_START( 1 ), 1, tileCnt, false );

        tileCnt = IO::loadPKMNIcon( SAVE::SAV.getActiveFile( ).m_lstDex, 0, 8,
                                    PKMN_SPRITE_START( 2 ), 2, 288, false );
        tileCnt = IO::loadTypeIcon( type( 0 ), 33, 35, PKMN_SPRITE_START( 2 ) + 1, 3, tileCnt,
                                    false, SAVE::SAV.getActiveFile( ).m_options.m_language );
        tileCnt = IO::loadTypeIcon( type( 0 ), 33, 35, PKMN_SPRITE_START( 2 ) + 2, 4, tileCnt,
                                    false, SAVE::SAV.getActiveFile( ).m_options.m_language );
        tileCnt = IO::loadPKMNIcon( SAVE::SAV.getActiveFile( ).m_lstDex, 0, 8,
                                    PKMN_SPRITE_START( 3 ), 5, tileCnt, false );
        for( u8 i = 0; i < 30; ++i ) {
            u8 x = 67 - 13 * ( i % 10 );
            if( i % 10 >= 5 ) x = 176 + 13 * ( i % 5 );
            IO::loadSprite( STAR_START + i, 5, tileCnt, x, 64 + 24 * ( i / 10 ), 16, 16, starPal,
                            starTiles, starTilesLen, false, false, false, OBJPRIORITY_2, false );
        }
        for( u8 i = PKMN_SPRITE_START( 0 ); i <= STAR_START + 30; ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;

        IO::updateOAM( false );

        // Init sub
        NAV::draw( );
        IO::initOAMTable( true );

        tileCnt = 0;
        tileCnt = IO::loadSprite( BACK_ID, 0, BACK_ID, tileCnt, SCREEN_WIDTH - 28,
                                  SCREEN_HEIGHT - 28, 32, 32, BackPal, BackTiles, BackTilesLen,
                                  false, false, false, OBJPRIORITY_1, true );

        tileCnt
            = IO::loadSprite( PAGE_START + 0, 0, PAGE_START + 0, tileCnt, 80, -10, 32, 32, memoPal,
                              memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt
            = IO::loadSprite( PAGE_START + 1, 0, PAGE_START + 1, tileCnt, 112, -10, 32, 32, PKMNPal,
                              PKMNTiles, PKMNTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( PAGE_START + 2, 0, PAGE_START + 2, tileCnt, 144, -10, 32, 32,
                                  time_iconPal, time_iconTiles, time_iconTilesLen, false, false,
                                  false, OBJPRIORITY_0, true );

        u16 tc2, tc3 = 0;
        u8  k = 0;
        for( u8 i : {4, 0, 5, 1, 6, 2, 7, 3} ) {
            u16 x = 88 * ( i % 4 ) - ( i > 3 ) * 44, y = 28 + ( i > 3 ) * 72;
            tc2 = IO::loadSprite( BG_SPR_START + 2 * k, 0, PAGE_START + 3, tileCnt, x, y, 64, 64,
                                  DexSub2Pal, DexSub2Tiles, DexSub2TilesLen, false, false, true,
                                  OBJPRIORITY_3, true );
            IO::loadSprite( BG_SPR_START + 2 * k + 1, 0, PAGE_START + 3, tileCnt, 20 + x, y, 64, 64,
                            DexSub2Pal, DexSub2Tiles, DexSub2TilesLen, false, true, true,
                            OBJPRIORITY_3, true );
            u8   l  = 0;
            auto jj = ( i > 3 ) ? std::vector<u8>( {0, 2, 3, 1} ) : std::vector<u8>( {2, 0, 1, 3} );
            for( u8 j : jj ) {
                tc3 = IO::loadSprite( FRAME_START + 4 * k + l, 0, PAGE_START + 4, tc2,
                                      x + 6 + 40 * ( j % 2 ), y + 2 + 32 * ( j / 2 ), 32, 32,
                                      DexSubPal, DexSubTiles, DexSubTilesLen, false, false, true,
                                      OBJPRIORITY_3, true );
                ++l;
            }
            ++k;
        }
        for( u8 i = 0; i < 5; ++i ) {
            IO::loadSprite( FRAME_START_2 + i, 0, PAGE_START + 4, tc2, 64, 24 + 32 * i, 32, 32,
                            DexSubPal, DexSubTiles, DexSubTilesLen, false, false, true,
                            OBJPRIORITY_2, true );
            tc3 = IO::loadSprite( PKMN_ICON_START_2 + i, 1, i, tc3, 64, 24 + 32 * i, 32, 32,
                                  DexSubPal, DexSubTiles, DexSubTilesLen, false, false, true,
                                  OBJPRIORITY_1, true );
        }
        IO::updateOAM( true );
    }

    void dexUI::drawFormes( u16 p_pkmnId, u8 p_forme, u8 p_formeCnt, bool p_isFemale,
                            bool p_isGenderless ) {
        u16 tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 10, 64,
                                          PKMN_SPRITE_START( 0 ), 0, 0, false, false,
                                          p_isFemale && !p_isGenderless, true, false, p_forme );
        if( p_formeCnt )
            IO::boldFont->printString( getDisplayName( p_pkmnId, p_forme ).c_str( ), 58, 157, false,
                                       IO::font::CENTER );
        else if( p_isFemale && !p_isGenderless )
            IO::boldFont->printString( GET_STRING( 133 ), 58, 157, false, IO::font::CENTER );
        else if( !p_isFemale && !p_isGenderless )
            IO::boldFont->printString( GET_STRING( 134 ), 58, 157, false, IO::font::CENTER );
        else
            IO::boldFont->printString( getDisplayName( p_pkmnId ).c_str( ), 58, 157, false,
                                       IO::font::CENTER );

        tileCnt = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmnId, 110, 64,
                                      PKMN_SPRITE_START( 1 ), 1, tileCnt, false, true,
                                      p_isFemale && !p_isGenderless, false, false, p_forme );
        if( p_formeCnt )
            IO::boldFont->printString( getDisplayName( p_pkmnId, p_forme ).c_str( ), 158, 150,
                                       false, IO::font::CENTER );
        else if( p_isFemale && !p_isGenderless )
            IO::boldFont->printString( GET_STRING( 133 ), 158, 150, false, IO::font::CENTER );
        else if( !p_isFemale && !p_isGenderless )
            IO::boldFont->printString( GET_STRING( 134 ), 158, 150, false, IO::font::CENTER );
        else
            IO::boldFont->printString( getDisplayName( p_pkmnId ).c_str( ), 158, 150, false,
                                       IO::font::CENTER );
        IO::boldFont->printString( GET_STRING( 135 ), 158, 166, false, IO::font::CENTER );

        // Load Icons of the other formes ( max 4 )
        if( !p_formeCnt ) {
            for( u8 i = PKMN_SPRITE_START( 3 ); i <= PKMN_SPRITE_START( 4 ); ++i )
                IO::OamTop->oamBuffer[ i ].isHidden = true;
            IO::updateOAM( false );
            return;
        }
        u8 currpos = p_forme;
        tileCnt    = IO::OamTop->oamBuffer[ STAR_START ].gfxIndex + 64;
        for( u8 i = 0; i < (u8) std::min( 4, p_formeCnt - 1 ); ++i ) {
            currpos = ( currpos + 1 ) % ( p_formeCnt );
            tileCnt = IO::loadPKMNIcon( p_pkmnId, 210, 150 - 35 * i, PKMN_SPRITE_START( 3 ) + i,
                                        6 + i, tileCnt, false, currpos );
        }
        for( u8 i = (u8) std::min( 4, p_formeCnt - 1 ); i < 4u; ++i )
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 3 ) + i ].isHidden = true;
        IO::updateOAM( false );
    }

    void dexUI::drawPage( u16 p_pkmnIdx, u8 p_page, u8 p_forme ) {
        IO::fadeScreen( IO::CLEAR_WHITE_FAST );
        if( !p_pkmnIdx ) p_pkmnIdx = _maxPkmn;

        for( u8 i = 0; i < 3; ++i ) IO::Oam->oamBuffer[ PAGE_START + i ].isHidden = true;

        consoleSelect( &IO::Top );
        consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
        consoleClear( );
        printf( "\x1b[39m" );

        // Init some colors
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::BLACK_IDX, 2 );
        IO::boldFont->setColor( IO::WHITE_IDX, 1 );

        if( _useInDex && !IN_DEX( p_pkmnIdx ) ) {
            for( u8 i = 0; i < 128; ++i ) IO::OamTop->oamBuffer[ i ].isHidden = true;
            dmaCopy( DexTopBitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
            dmaCopy( DexTopPal, BG_PALETTE, 512 );
            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", 0, 80, 35, PKMN_SPRITE_START( 0 ), 0,
                                0, false );

            BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
            BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
            IO::boldFont->printString( NO_DATA, 128, 150, false, IO::font::CENTER );
            IO::updateOAM( true );

            IO::fadeScreen( IO::UNFADE_FAST );
            return;
        }
        for( u8 i = 0; i < 3; ++i ) IO::Oam->oamBuffer[ PAGE_START + i ].isHidden = i == p_page;
        IO::updateOAM( true );

        pokemonData data;
        getAll( p_pkmnIdx, data );

        bool isFixed = !FS::exists( "nitro:/PICS/SPRITES/PKMN/", p_pkmnIdx, "f" );

        if( p_page <= 1 )
            p_forme %= data.m_formecnt ? ( ( 2 - isFixed ) * data.m_formecnt ) : ( 2 - isFixed );

        u8 currFormeIdx = data.m_formecnt ? p_forme / ( 2 - isFixed ) : p_forme;
        u8 formeCnt     = data.m_formecnt;

        if( currFormeIdx ) getAll( p_pkmnIdx, data, currFormeIdx );

        for( u8 i = 0; i < PKMN_SPRITE_START( 5 ); ++i ) IO::OamTop->oamBuffer[ i ].isHidden = true;

        // Draw top screen's background

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
                printf( GET_STRING( 132 ), getDisplayName( data.m_preEvolution ).c_str( ) );
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
                printf( GET_STRING( 132 ), getDisplayName( data.m_preEvolution ).c_str( ) );
                break;
            case 3:
            default:
                if( p_page == 0 ) {
                    dmaCopy( DexTop3Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                    dmaCopy( DexTop3Pal + 64, BG_PALETTE + 64, 256 );
                } else {
                    dmaCopy( DexTop32Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
                    dmaCopy( DexTop32Pal + 64, BG_PALETTE + 64, 256 );
                }
                IO::loadItemIcon( data.m_preEvolution, 8, 20, PKMN_SPRITE_START( 2 ), 2,
                                  IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );
                consoleSetWindow( &IO::Top, 6, 4, 15, 1 );
                printf( GET_STRING( 132 ),
                        ITEM::getItemName( data.m_preEvolution, CURRENT_LANGUAGE ).c_str( ) );
                break;
            }
            switch( data.m_types[ 0 ] ) {
            case NORMAL:
            case FLYING:
            case UNKNOWN:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Normal",
                                     128 );
                break;
            case FIGHTING:
            case GROUND:
            case ROCK:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Fighting",
                                     128 );
                break;
            case POISON:
            case GHOST:
            case PSYCHIC:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Psychic",
                                     128 );
                break;
            case BUG:
            case GRASS:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Grass",
                                     128 );
                break;
            case STEEL:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Metal",
                                     128 );
                break;
            case WATER:
            case ICE:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Water",
                                     128 );
                break;
            case FIRE:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Fire",
                                     128 );
                break;
            case LIGHTNING:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Lightning",
                                     128 );
                break;
            case DRAGON:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Dragon",
                                     128 );
                break;
            case DARKNESS:
                FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/TYPE_BACK/", "Dark",
                                     128 );
                IO::regularFont->setColor( IO::BLACK_IDX, 2 );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                if( data.m_stage == 0 || data.m_stage == 3 ) {
                    IO::boldFont->setColor( IO::BLACK_IDX, 1 );
                    IO::boldFont->setColor( IO::WHITE_IDX, 2 );
                }
                break;
            default:
                break;
            }

            BG_PALETTE[ 251 ]           = RGB15( 3, 3, 3 );
            BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
            BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;

            IO::boldFont->printString( data.m_displayName, 58, 11, false );

            if( data.m_types[ 0 ] != data.m_types[ 1 ] ) {
                IO::loadTypeIcon( data.m_types[ 0 ], 183, 28, PKMN_SPRITE_START( 2 ) + 1, 3,
                                  IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].gfxIndex,
                                  false, SAVE::SAV.getActiveFile( ).m_options.m_language );
                IO::loadTypeIcon( data.m_types[ 1 ], 215, 28, PKMN_SPRITE_START( 2 ) + 2, 4,
                                  IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].gfxIndex,
                                  false, SAVE::SAV.getActiveFile( ).m_options.m_language );
            } else {
                IO::loadTypeIcon( data.m_types[ 0 ], 199, 28, PKMN_SPRITE_START( 2 ) + 1, 3,
                                  IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].gfxIndex,
                                  false, SAVE::SAV.getActiveFile( ).m_options.m_language );
                IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;
            }

            consoleSetWindow( &IO::Top, 28, 1, 32, 2 );
            printf( "%03d", p_pkmnIdx );
            consoleSetWindow( &IO::Top, 28, 2, 32, 3 );
            printf( "%3d", SAVE::SAV.countPkmn( p_pkmnIdx ) );
        }

        switch( p_page ) {
        case 0: {
            bool isF = currFormeIdx % 2 && !isFixed;
            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmnIdx, 80, 35,
                                PKMN_SPRITE_START( 0 ), 0, 0, false, false, isF, false, false,
                                currFormeIdx / ( 2 - isFixed ) );

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
            IO::regularFont->printString( GET_STRING( 126 ), 80, 49, false, IO::font::RIGHT );
            IO::regularFont->printString( GET_STRING( 127 ), 80, 74, false, IO::font::RIGHT );
            IO::regularFont->printString( GET_STRING( 130 ), 80, 99, false, IO::font::RIGHT );

            IO::regularFont->printString( GET_STRING( 129 ), 176, 49, false );
            IO::regularFont->printString( GET_STRING( 128 ), 176, 74, false );
            IO::regularFont->printString( GET_STRING( 131 ), 176, 99, false );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::WHITE_IDX, 2 );
            IO::regularFont->printString(
                NO_DATA /* FS::breakString( data.m_dexEntry, IO::regularFont, 224 ).c_str( ) */, 16,
                134, false, IO::font::LEFT, 12 );

            consoleSetWindow( &IO::Top, 3, 22, 32, 23 );
            if( strlen( data.m_species ) >= 13 ) consoleSetWindow( &IO::Top, 2, 22, 32, 23 );
            if( strlen( data.m_species ) == 8 || strlen( data.m_species ) == 9 ) printf( " " );
            printf( "%8s %4.1fm %5.1fkg", data.m_species, data.m_size / 10.0,
                    data.m_weight / 10.0 );
            break;
        }
        case 1: {
            for( u8 i = 0; i < 30; ++i ) IO::OamTop->oamBuffer[ i + STAR_START ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;

            drawFormes( p_pkmnIdx, currFormeIdx / ( 2 - isFixed ), formeCnt, currFormeIdx % 2,
                        isFixed );
            break;
        }
        case 2: {
            if( p_forme % 3 == 0 )
                FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "HoennMap" );
            else if( p_forme % 3 == 1 )
                FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "KantoMap" );
            else
                FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "JohtoMap" );

            BG_PALETTE[ 251 ]           = RGB15( 3, 3, 3 );
            BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
            BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;

            IO::boldFont->printString( data.m_displayName, 32, 4, false );
            IO::loadPKMNIcon( p_pkmnIdx, 0, -4, PKMN_SPRITE_START( 2 ), 2,
                              IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) ].gfxIndex, false );

            for( u8 i = 0; i < 30; ++i ) IO::OamTop->oamBuffer[ i + STAR_START ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 1 ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 2 ) + 2 ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) ].isHidden     = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) + 1 ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) + 2 ].isHidden = true;
            IO::OamTop->oamBuffer[ PKMN_SPRITE_START( 0 ) + 3 ].isHidden = true;
            break;
        }
        default:
            break;
        }

        IO::updateOAM( false );
        IO::fadeScreen( IO::UNFADE_FAST );
        return;
    }

    void dexUI::drawSub( u8 p_mode, u16 p_pkmnIdcs[ 32 ], u16 p_idxStart, u8 p_selectedIdx,
                         u8 p_oldIdx ) {
        setAllVis( true );
        setCghtVis( true );
        switch( (dex::mode) p_mode ) {
        case dex::SHOW_SINGLE:
        case dex::SHOW_CAUGHT:
            NAV::draw( );
            IO::regularFont->setColor( 0, 0 );
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( 0, 2 );
            setCghtVis( false );
            IO::regularFont->printString( GET_STRING( 124 ), 2, 0, true );
            for( u8 i = 0; i < 5; ++i ) {
                u16 id = p_pkmnIdcs[ ( p_idxStart + i ) % 5 ] % ( MAX_PKMN + 1 );

                if( !id ) {
                    IO::Oam->oamBuffer[ FRAME_START_2 + i ].isHidden     = true;
                    IO::Oam->oamBuffer[ PKMN_ICON_START_2 + i ].isHidden = true;
                    if( !i ) IO::boldFont->printString( NO_DATA, 128, 89, true, IO::font::CENTER );
                    continue;
                }

                bool inDex = IN_DEX( id );
                loadPkmnIconToSlot( inDex * id, i, false );
                char buffer[ 10 ];
                snprintf( buffer, 9, "%03d", id );
                IO::boldFont->printString( buffer, 32, 28 + 32 * i, true );
                IO::boldFont->printString( getDisplayName( id ).c_str( ), 100, 28 + 32 * i, true );

                if( i == p_selectedIdx ) {
                    pokemonData p;
                    getAll( id, p );
                    BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::getColor( p.m_types[ 0 ] );
                }
                IO::printRectangle( 64 + 3, 24 + 32 * i + 2, 64 + 28, 24 + 32 * i + 26, true,
                                    i == p_selectedIdx ? IO::COLOR_IDX : IO::WHITE_IDX );
            }
            break;
        case dex::SHOW_ALL:
            setAllVis( false );

            IO::regularFont->printString( GET_STRING( 125 ), 2, 0, true );
            if( p_oldIdx )
                IO::printRectangle( IO::Oam->oamBuffer[ FRAME_START + p_oldIdx ].x + 3,
                                    IO::Oam->oamBuffer[ FRAME_START + p_oldIdx ].y + 2,
                                    IO::Oam->oamBuffer[ FRAME_START + p_oldIdx ].x + 29,
                                    IO::Oam->oamBuffer[ FRAME_START + p_oldIdx ].y + 25, true, 0 );

            if( 12 <= p_selectedIdx && p_selectedIdx < 16 ) {
                if( IO::Oam->oamBuffer[ BG_SPR_START + 2 * 3 ].x == 88 - 44 ) moveAll( +44 );
            } else if( 16 <= p_selectedIdx && p_selectedIdx < 20 ) {
                if( IO::Oam->oamBuffer[ BG_SPR_START + 2 * 3 ].x == 88 ) moveAll( -44 );
            }

            for( u8 i = 0; i < 32; ++i ) {
                u16 id = p_pkmnIdcs[ ( p_idxStart + i ) % 32 ];
                if( id > MAX_PKMN ) id = 0;
                if( !id ) {
                    IO::Oam->oamBuffer[ FRAME_START + i ].isHidden     = true;
                    IO::Oam->oamBuffer[ PKMN_ICON_START + i ].isHidden = true;
                    if( !( i % 4 ) ) {
                        IO::Oam->oamBuffer[ BG_SPR_START + 2 * ( i / 4 ) ].isHidden     = true;
                        IO::Oam->oamBuffer[ BG_SPR_START + 2 * ( i / 4 ) + 1 ].isHidden = true;
                    }
                    continue;
                }
                bool inDex = IN_DEX( id );
                loadPkmnIconToSlot( inDex * id, i, true );
                if( i == p_selectedIdx ) {
                    pokemonData p;
                    getAll( id * inDex, p );
                    IO::Oam->oamBuffer[ PKMN_ICON_START + i ].priority = OBJPRIORITY_1;
                    BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::getColor( p.m_types[ 0 ] );
                    IO::printRectangle( IO::Oam->oamBuffer[ FRAME_START + i ].x + 3,
                                        IO::Oam->oamBuffer[ FRAME_START + i ].y + 2,
                                        IO::Oam->oamBuffer[ FRAME_START + i ].x + 29,
                                        IO::Oam->oamBuffer[ FRAME_START + i ].y + 25, true,
                                        IO::COLOR_IDX );
                }
            }
            break;
        }
        IO::updateOAM( true );
    }
} // namespace DEX
