/*
Pokémon neo
------------------------------

file        : boxUI.cpp
author      : Philip Wellnitz
description : Storage sys UI

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

#include <nds.h>

#include "boxUI.h"
#include "defines.h"
#include "fs.h"
#include "pokemon.h"
#include "screenFade.h"
#include "uio.h"

#include "A.h"
#include "Back.h"
#include "Backward.h"
#include "Down.h"
#include "Forward.h"
#include "Up.h"
#include "box_arrow.h"

#include "NoItem.h"
#include "boxsub.h"
#include "boxwp1.h"
#include "boxwp2.h"

#include "backarrow.h"
#include "x_16_16.h"

namespace BOX {

#define SPR_PKMN_START_OAM_SUB 0
#define SPR_X_OAM_SUB 40
#define SPR_ARROW_BACK_OAM_SUB 41

#define MAX_WALLPAPERS 2
    const unsigned int* wallpaperTiles[ MAX_WALLPAPERS ] = { boxwp1Bitmap, boxwp2Bitmap };
    const unsigned short* wallpaperPals[ MAX_WALLPAPERS ] = { boxwp1Pal, boxwp2Pal };


    void boxUI::init( ) {
        REG_BLDCNT_SUB = BLEND_NONE;
        IO::clearScreen( true, false, false );
        IO::initOAMTable( true );
        vramSetBankD( VRAM_D_SUB_SPRITE );
        videoSetModeSub( MODE_5_2D | // DISPLAY_BG1_ACTIVE |
                         DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                         | ( ( DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
                         | DISPLAY_SPR_1D_BMP_SIZE_128 | ( 5 << 28 ) | 2 ) & 0xffffff0 ) );

        u16 tileCnt = 0;

        // back arrow
        tileCnt
            = IO::loadSpriteB( SPR_ARROW_BACK_OAM_SUB, tileCnt, 102 + 56 + 32,
                               192 - 19, 16, 16, backarrowPal, backarrowTiles,
                               backarrowTilesLen, false,
                               false, false, OBJPRIORITY_0, true );
        // x
        tileCnt = IO::loadSpriteB( SPR_X_OAM_SUB, tileCnt, 236, 172, 16, 16,
                                   x_16_16Pal, x_16_16Tiles, x_16_16TilesLen, false, false, false,
                                   OBJPRIORITY_2, true );


        // pkmn
        for( u8 i = 0; i < 5; ++i ) {
            for( u8 j = 0; j < 6; ++j ) {
                tileCnt = IO::loadSpriteB( SPR_PKMN_START_OAM_SUB + 6 * i + j,
                        tileCnt, 29 + 26 * j, 32 + 26 * i,
                        32, 32, NoItemPal, NoItemTiles, NoItemTilesLen,
                        false, false, false, OBJPRIORITY_3, true );
            }
        }

        bgUpdate( );
        IO::updateOAM( true );
    }

    std::vector<IO::inputTarget> boxUI::draw( box* p_box, bool p_showTeam ) {
        std::vector<IO::inputTarget> res;
        SpriteEntry* oam = IO::Oam->oamBuffer;

        dmaCopy( wallpaperTiles[ p_box->m_wallpaper % MAX_WALLPAPERS ],
                 bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        dmaCopy( wallpaperPals[ p_box->m_wallpaper % MAX_WALLPAPERS ], BG_PALETTE_SUB, 64 * 2 );
        dmaCopy( boxsubBitmap, bgGetGfxPtr( IO::bg2sub ), 256 * 256 );
        dmaCopy( boxsubPal, BG_PALETTE_SUB, 6 * 2 );

        for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i ) {
            u8 x = i % 6, y = i / 6;

            res.push_back( IO::inputTarget( oam[ SPR_PKMN_START_OAM_SUB + i ].x,
                                            oam[ SPR_PKMN_START_OAM_SUB + i ].y,
                                            oam[ SPR_PKMN_START_OAM_SUB + i ].x + 26,
                                            oam[ SPR_PKMN_START_OAM_SUB + i ].y + 26 ) );
            // _ranges.push_back( {oam, res.back( )} );
            if( p_box->m_pokemon[ i ].getSpecies( ) ) {
                if( !p_box->m_pokemon[ i ].isEgg( ) ) {
                    IO::loadPKMNIconB( ( *p_box )[ i ].getSpecies( ),
                            oam[ SPR_PKMN_START_OAM_SUB + i ].x,
                            oam[ SPR_PKMN_START_OAM_SUB + i ].y,
                            SPR_PKMN_START_OAM_SUB + i,  oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex,
                            true );
                } else {
                    IO::loadEggIconB(
                            oam[ SPR_PKMN_START_OAM_SUB + i ].x,
                            oam[ SPR_PKMN_START_OAM_SUB + i ].y,
                            SPR_PKMN_START_OAM_SUB + i,  oam[ SPR_PKMN_START_OAM_SUB + i ].gfxIndex,
                            true );
                }
            } else {
                oam[ SPR_PKMN_START_OAM_SUB + i ].isHidden = true;
            }
        }

        IO::updateOAM( true );
        return res;
    }


    constexpr u16 getBoxColor( u8 p_boxIdx ) {
        return RGB15( 4 * ( ( 41 - p_boxIdx ) % 7 + 1 ), ( p_boxIdx * 30 ) / 42 + 1,
                      5 * ( ( 41 - p_boxIdx ) / 7 + 1 ) );
    }

//    boxUI::boxUI( ) {
        /*
        IO::swapScreens( );
        IO::vramSetup( );
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                      | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );

        bgUpdate( );
        swiWaitForVBlank( );
        NAV::draw( );
        drawAllBoxStatus( );

        IO::initOAMTable( false );
        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, 0, tileCnt, SCREEN_WIDTH - 28,
                                  SCREEN_HEIGHT - 28, 32, 32, BackPal, BackTiles, BackTilesLen,
                                  false, false, false, OBJPRIORITY_1, false );
        tileCnt
            = IO::loadSprite( ARROW_ID, 0, 1, tileCnt, 0, 0, 16, 16, box_arrowPal, box_arrowTiles,
                              box_arrowTilesLen, false, false, true, OBJPRIORITY_0, false );
        tileCnt += 64;
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 0 ), 0, PAGE_ICON_START + ( 0 ), tileCnt, 20,
                                  0, 32, 32, time_iconPal, time_iconTiles, time_iconTilesLen, false,
                                  false, true, OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 1 ), 0, PAGE_ICON_START + ( 1 ), tileCnt, 40,
                                  0, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, true,
                                  OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 2 ), 0, PAGE_ICON_START + ( 2 ), tileCnt, 0,
                                  0, 32, 32, PKMNPal, PKMNTiles, PKMNTilesLen, false, false, true,
                                  OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 3 ), 0, PAGE_ICON_START + ( 3 ), tileCnt, 50,
                                  0, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, true,
                                  OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 4 ), 0, PAGE_ICON_START + ( 4 ), tileCnt, 60,
                                  0, 32, 32, ContestPal, ContestTiles, ContestTilesLen, false,
                                  false, true, OBJPRIORITY_0, false );
        for( u8 i = 0; i < 5; ++i ) {
            IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].x = 48 + 32 * i;
            IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].y = 256 - 10;
        }
        IO::updateOAM( false );
        IO::initOAMTable( true );

        BG_PALETTE[ IO::COLOR_IDX ] = IO::GREEN;
        BG_PALETTE[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE[ IO::RED_IDX ]   = IO::RED;

        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GREEN;
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;
        BG_PALETTE_SUB[ IO::RED_IDX ]   = IO::RED;
        */
//    }
//    boxUI::~boxUI( ) {
        // IO::swapScreens( );
//    }

    void boxUI::drawAllBoxStatus( bool p_bottom ) {
        /*
        dmaFillWords( 0, bgGetGfxPtr( !p_bottom ? IO::bg2sub : IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( !p_bottom ? IO::bg3sub : IO::bg3 ), 256 * 192 );

        auto pal = BG_PAL( !p_bottom );
        pal[ 0 ] = IO::WHITE;
        char buffer[ 50 ];

        IO::initOAMTable( !p_bottom );
        IO::regularFont->printString( GET_STRING( 61 ), 4, 3, !p_bottom );
        u8  w = 32, h = 24; // width and heigth
        u16 pkmncnt = 0;
        for( u8 i = 0; i < 7; ++i )
            for( u8 j = 0; j < 6; ++j ) {
                pal[ 128 + j * 7 + i ] = getBoxColor( j * 7 + i );
                u8   x                 = 2 + 36 * i;
                u8   y                 = 22 + 28 * j;
                bool prsd              = j * 7 + i == SAVE::SAV.getActiveFile( ).m_curBox;
                IO::printChoiceBox( x, y, x + w, y + h, 6 - 2 * prsd, 128 + j * 7 + i, false,
                                    !p_bottom );
                u8 cnt = SAVE::SAV.getActiveFile( )m_storedPokemon[ j * 7 + i ].count( );
                pkmncnt += cnt;
                if( cnt == MAX_PKMN_PER_BOX )
                    IO::regularFont->setColor( IO::RED_IDX, 1 );
                else
                    IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                snprintf( buffer, 49, "%2d", cnt );
                IO::regularFont->printString( buffer, x + 7, y + 5, !p_bottom );
            }
        snprintf( buffer, 49, "%4d Pokémon", pkmncnt );
        if( pkmncnt == 756 )
            IO::regularFont->setColor( IO::RED_IDX, 1 );
        else
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->printString( buffer, 252, 3, !p_bottom, IO::font::RIGHT );

        for( u8 i = 0; i < 5; ++i ) {
            IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].isHidden = true;
        }
        IO::updateOAM( false );
        */
    }

#define POS_X( i ) ( 30 + 33 * ( ( i ) % 6 ) )
#define POS_Y( i ) ( 45 + 25 * ( ( i ) / 6 ) )
#define TEAM_POS_X( i ) ( 18 + 16 + 32 * ( i ) )
#define TEAM_POS_Y( i ) ( 8 + 57 + 28 * 3 )

    void BOX::boxUI::buttonChange( button p_button, bool p_pressed ) {

        /*
        u8 dx = p_pressed * 2;
        u8 dy = p_pressed;
        switch( p_button ) {
        case BOX::boxUI::BUTTON_LEFT:
            IO::printChoiceBox( 24, 23, 48, 48, 6, IO::GRAY_IDX, p_pressed, false ); // <
            IO::regularFont->printString( "<", dx + 31, dy + 28, false );
            break;
        case BOX::boxUI::BUTTON_RIGHT:
            IO::printChoiceBox( 208, 23, 232, 48, 6, IO::GRAY_IDX, p_pressed, false ); // >
            IO::regularFont->printString( ">", dx + 216, dy + 28, false );
            break;
        case BOX::boxUI::BUTTON_BOX_NAME: {
            box* box = SAVE::SAV.getActiveFile( )getCurrentBox( );
            IO::printChoiceBox( 50, 23, 206, 48, 6, IO::COLOR_IDX, p_pressed, false ); // Box name
            IO::regularFont->printString( box->m_name, dx + 127, dy + 28, false, IO::font::CENTER );
            break;
        }
        default:
            break;
        }
        */
    }

    //std::vector<IO::inputTarget> boxUI::draw( bool p_showTeam ) {
        /*
        BG_PALETTE[ IO::COLOR_IDX ] = getBoxColor( SAVE::SAV.getActiveFile( ).m_curBox );

        std::vector<IO::inputTarget> res;
        _ranges.clear( );
        _showTeam = p_showTeam;
        box* box  = SAVE::SAV.getActiveFile( )getCurrentBox( );

        // SubScreen stuff
        IO::printChoiceBox( 50, 23, 206, 48, 6, IO::COLOR_IDX, false, false ); // Box name
        IO::printChoiceBox( 24, 23, 48, 48, 6, IO::GRAY_IDX, false, false );   // <
        IO::printChoiceBox( 208, 23, 232, 48, 6, IO::GRAY_IDX, false, false ); // >

        IO::regularFont->printString( box->m_name, 127, 28, false, IO::font::CENTER );
        IO::regularFont->printString( "<", 31, 28, false );
        IO::regularFont->printString( ">", 216, 28, false );

        u8  oam     = PKMN_START;
        u8  pal     = PKMN_PALETTE_START;
        u16 tileCnt = PKMN_TILES_START;

        IO::printChoiceBox( 24, 51, 232, 136, 6, IO::COLOR_IDX, false, false );

        IO::printRectangle( 0, 140, 255, 192, false,  IO::WHITE_IDX );
        IO::regularFont->printString( p_showTeam ? GET_STRING( 59 ) : GET_STRING( 60 ), 2, 176,
                                      false );
        for( u8 i = 0; i < 6; ++i ) {
            res.push_back( IO::inputTarget( TEAM_POS_X( i ), TEAM_POS_Y( i ), TEAM_POS_X( i ) + 28,
                                            TEAM_POS_Y( i ) + 21 ) );
            _ranges.push_back( {oam, res.back( )} );

            IO::printChoiceBox( TEAM_POS_X( i ), TEAM_POS_Y( i ), TEAM_POS_X( i ) + 28,
                                TEAM_POS_Y( i ) + 21, 3, IO::GRAY_IDX, false, false );

        }

        drawAllBoxStatus( );
        IO::updateOAM( false );
        return res;
        */
  //  }

    void boxUI::select( u8 p_index ) {
        /*
        if( p_index == (u8) -1 ) {
            IO::OamTop->oamBuffer[ ARROW_ID ].isHidden = true;
            IO::updateOAM( false );
            drawAllBoxStatus( );
            return;
        }
        u8 x = 0, y = 0;
        if( p_index < MAX_PKMN_PER_BOX ) {
            x = POS_X( p_index );
            y = POS_Y( p_index );
        } else if( p_index < MAX_PKMN_PER_BOX + 6 ) {
            x = TEAM_POS_X( p_index - MAX_PKMN_PER_BOX ) - 3;
            y = TEAM_POS_Y( p_index - MAX_PKMN_PER_BOX ) - 10;
        } else if( p_index == MAX_PKMN_PER_BOX + 6 ) {
            x = 24;
            y = 23;
        } else if( p_index == MAX_PKMN_PER_BOX + 7 ) {
            x = 208;
            y = 23;
        }
        bool holding                               = !IO::OamTop->oamBuffer[ HELD_PKMN ].isHidden;
        IO::OamTop->oamBuffer[ ARROW_ID ].x        = x + 20 - 5 * holding;
        IO::OamTop->oamBuffer[ ARROW_ID ].y        = y + 4 - 5 * holding;
        IO::OamTop->oamBuffer[ ARROW_ID ].isHidden = false;

        if( p_index < MAX_PKMN_PER_BOX + 6 ) {
            IO::OamTop->oamBuffer[ p_index + PKMN_START ].x = x;
            IO::OamTop->oamBuffer[ p_index + PKMN_START ].y = y;
        }
        // Held pkmn
        IO::OamTop->oamBuffer[ HELD_PKMN ].x = x - 5;
        IO::OamTop->oamBuffer[ HELD_PKMN ].y = y - 5;
        IO::updateOAM( false );
        */
    }

    void boxUI::takePkmn( u8 p_index, u16 p_heldPkmnIdx, bool p_isEgg ) {
        /*
        if( p_index != (u8) -1 ) {
            box* box = SAVE::SAV.getActiveFile( )getCurrentBox( );

            boxPokemon bpm;
            if( p_index < MAX_PKMN_PER_BOX )
                bpm = box->m_pokemon[ p_index ];
            else if( _showTeam )
                bpm = SAVE::SAV.getActiveFile( )
                          .m_pkmnTeam[ p_index - MAX_PKMN_PER_BOX ]
                          .m_boxdata;

            if( bpm.m_speciesId ) {
                u8 pal = p_index + PKMN_PALETTE_START;
                if( !bpm.isEgg( ) ) {
                    IO::loadPKMNIcon( bpm.m_speciesId, POS_X( p_index ), POS_Y( p_index ),
                                      PKMN_START + p_index, pal / 16, pal % 16,
                                      PKMN_TILES_START + 32 * p_index, false );
                } else
                    IO::loadEggIcon( POS_X( p_index ), POS_Y( p_index ), PKMN_START + p_index,
                                     pal / 16, pal % 16, PKMN_TILES_START + 32 * p_index, false );
            } else {
                IO::OamTop->oamBuffer[ PKMN_START + p_index ].isHidden = true;
            }

            if( p_heldPkmnIdx ) {
                if( !p_isEgg )
                    IO::loadPKMNIcon( p_heldPkmnIdx, POS_X( p_index ), POS_Y( p_index ), HELD_PKMN,
                                      6, 0, 64, false );
                else
                    IO::loadEggIcon( POS_X( p_index ), POS_Y( p_index ), HELD_PKMN, 6, 0, 64,
                                     false );
            } else
                IO::OamTop->oamBuffer[ HELD_PKMN ].isHidden = true;
        } else {
            IO::OamTop->oamBuffer[ HELD_PKMN ].isHidden = true;
        }
        select( p_index );
        */
    }

    void boxUI::updateTeam( ) {
        /*
        for( u8 i = 0; i < 6; ++i ) {
            u16  species = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId;
            bool isEgg
                = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].isEgg( );
            u8 pal = MAX_PKMN_PER_BOX + PKMN_PALETTE_START + i;

            if( species ) {
                if( !isEgg )
                    IO::loadPKMNIcon( species, TEAM_POS_X( i ) - 3, TEAM_POS_Y( i ) - 10,
                                      PKMN_START + 18 + i, pal / 16, pal % 16,
                                      PKMN_TILES_START + 32 * MAX_PKMN_PER_BOX + 32 * i, false );
                else
                    IO::loadEggIcon( TEAM_POS_X( i ) - 3, TEAM_POS_Y( i ) - 10, PKMN_START + 18 + i,
                                     pal / 16, pal % 16,
                                     PKMN_TILES_START + 32 * MAX_PKMN_PER_BOX + 32 * i, false );
            } else
                IO::OamTop->oamBuffer[ PKMN_START + 18 + i ].isHidden = true;
        }
        IO::updateOAM( false );
        */
    }
} // namespace BOX
