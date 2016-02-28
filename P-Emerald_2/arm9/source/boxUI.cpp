/*
Pokémon Emerald 2 Version
------------------------------

file        : boxUI.cpp
author      : Philip Wellnitz
description : Storage sys UI

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

#include <nds.h>

#include "boxUI.h"
#include "uio.h"
#include "defines.h"
#include "fs.h"
#include "pokemon.h"

#include "Back.h"
#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Up.h"
#include "Down.h"
#include "box_arrow.h"

#include "memo.h"
#include "atks.h"
#include "Contest.h"

namespace BOX {
#define ARROW_ID 1
#define HELD_PKMN 2
#define FWD_ICON 3
#define BWD_ICON 4
#define PAGE_ICON_START 5
#define PKMN_START 10
#define PKMN_PALETTE_START 15 
#define PKMN_TILES_START 96 + 182

    constexpr u16 getBoxColor( u8 p_boxIdx ) {
        return RGB15( 4 * ( ( 41 - p_boxIdx ) % 7 + 1 ),
                      ( p_boxIdx * 30 ) / 42 + 1,
                      5 * ( ( 41 - p_boxIdx ) / 7 + 1 ) );
    }

    boxUI::boxUI( ) {
        IO::swapScreens( );
        IO::vramSetup( );
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );

        bgUpdate( );
        swiWaitForVBlank( );
        IO::NAV->draw( );
        drawAllBoxStatus( );

        IO::initOAMTable( false );
        u16 tileCnt = 0;

        tileCnt = IO::loadSprite( BACK_ID, BACK_ID, 0, tileCnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_1, false );
        tileCnt = IO::loadSprite( ARROW_ID, 0, 1, tileCnt,
                                  0, 0, 16, 16, box_arrowPal, box_arrowTiles, box_arrowTilesLen,
                                  false, false, true, OBJPRIORITY_0, false );
        tileCnt += 64;
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 0 ), 0, PAGE_ICON_START + ( 0 ), tileCnt,
                                  0, 0, 32, 32, memoPal,
                                  memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 1 ), 0, PAGE_ICON_START + ( 1 ), tileCnt,
                                  20, 0, 32, 32, memoPal,
                                  memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 2 ), 0, PAGE_ICON_START + ( 2 ), tileCnt,
                                  40, 0, 32, 32, memoPal,
                                  memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 3 ), 0, PAGE_ICON_START + ( 3 ), tileCnt,
                                  50, 0, 32, 32, atksPal,
                                  atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadSprite( PAGE_ICON_START + ( 4 ), 0, PAGE_ICON_START + ( 4 ), tileCnt,
                                  60, 0, 32, 32, ContestPal,
                                  ContestTiles, ContestTilesLen, false, false, false, OBJPRIORITY_0, false );

        IO::updateOAM( false );
        IO::initOAMTable( true );

        BG_PALETTE[ COLOR_IDX ] = GREEN;
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ RED_IDX ] = RED;

        BG_PALETTE_SUB[ COLOR_IDX ] = GREEN;
        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
    }
    boxUI::~boxUI( ) {
        IO::swapScreens( );
    }

    void boxUI::drawAllBoxStatus( bool p_bottom ) {
        dmaFillWords( 0, bgGetGfxPtr( !p_bottom ? IO::bg2sub : IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( !p_bottom ? IO::bg3sub : IO::bg3 ), 256 * 192 );
        IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );
        consoleClear( );

        auto pal = BG_PAL( !p_bottom );
        pal[ 0 ] = WHITE;

        IO::initOAMTable( !p_bottom );
        IO::printString( IO::regularFont, "Alle Boxen", 4, 3, !p_bottom );
        u8 w = 32, h = 24; //width and heigth
        u16 pkmncnt = 0;
        for( u8 i = 0; i < 7; ++i )
            for( u8 j = 0; j < 6; ++j ) {
                pal[ 128 + j * 7 + i ] = getBoxColor( j * 7 + i );
                u8 x = 2 + 36 * i;
                u8 y = 22 + 28 * j;
                bool prsd = j * 7 + i == FS::SAV->m_curBox;
                IO::printChoiceBox( x, y, x + w, y + h, 6 - 2 * prsd, 128 + j * 7 + i,
                                    false, !p_bottom );
                u8 cnt = FS::SAV->m_storedPokemon[ j * 7 + i ].count( );
                pkmncnt += cnt;
                if( cnt == MAX_PKMN_PER_BOX )
                    IO::regularFont->setColor( RED_IDX, 1 );
                else
                    IO::regularFont->setColor( BLACK_IDX, 1 );
                sprintf( buffer, "%2d", cnt );
                IO::printString( IO::regularFont, buffer,
                                 x + 7, y + 5, !p_bottom );
            }
        sprintf( buffer, "%4d Pokémon", pkmncnt );
        if( pkmncnt == 756 )
            IO::regularFont->setColor( RED_IDX, 1 );
        else
            IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::printString( IO::regularFont, buffer, 252 - IO::regularFont->stringWidth( buffer ), 3, !p_bottom );
    }

#define POS_X( i ) ( 30 + 33 * ( (i) % 6 ) )
#define POS_Y( i ) ( 45 + 25 * ( (i) / 6 ) )
#define TEAM_POS_X( i ) ( 18 + 16 + 32 * (i) )
#define TEAM_POS_Y( i ) ( 8 + 57 + 28 * 3 )

    void BOX::boxUI::buttonChange( button p_button, bool p_pressed ) {
        u8 dx = p_pressed * 2;
        u8 dy = p_pressed;
        switch( p_button ) {
            case BOX::boxUI::BUTTON_LEFT:
                IO::printChoiceBox( 24, 23, 48, 48, 6, GRAY_IDX, p_pressed, false ); // <
                IO::printString( IO::regularFont, "<", dx + 31, dy + 28, false );
                break;
            case BOX::boxUI::BUTTON_RIGHT:
                IO::printChoiceBox( 208, 23, 232, 48, 6, GRAY_IDX, p_pressed, false ); // >
                IO::printString( IO::regularFont, ">", dx + 216, dy + 28, false );
                break;
            case BOX::boxUI::BUTTON_BOX_NAME:
            {
                box* box = FS::SAV->getCurrentBox( );
                IO::printChoiceBox( 50, 23, 206, 48, 6, COLOR_IDX, p_pressed, false ); //Box name
                IO::printString( IO::regularFont, box->m_name, dx + 127 - IO::regularFont->stringWidth( box->m_name ) / 2, dy + 28, false );
                break;
            }
            default:
                break;
        }
    }

    std::vector<IO::inputTarget> boxUI::draw( bool p_showTeam ) {
        BG_PALETTE[ COLOR_IDX ] = getBoxColor( FS::SAV->m_curBox );

        std::vector<IO::inputTarget> res;
        _ranges.clear( );
        _showTeam = p_showTeam;
        box* box = FS::SAV->getCurrentBox( );

        //SubScreen stuff
        IO::printChoiceBox( 50, 23, 206, 48, 6, COLOR_IDX, false, false ); //Box name
        IO::printChoiceBox( 24, 23, 48, 48, 6, GRAY_IDX, false, false ); // <
        IO::printChoiceBox( 208, 23, 232, 48, 6, GRAY_IDX, false, false ); // >

        IO::printString( IO::regularFont, box->m_name, 127 - IO::regularFont->stringWidth( box->m_name ) / 2, 28, false );
        IO::printString( IO::regularFont, "<", 31, 28, false );
        IO::printString( IO::regularFont, ">", 216, 28, false );

        u8 oam = PKMN_START;
        u8 pal = PKMN_PALETTE_START;
        u16 tileCnt = PKMN_TILES_START;

        IO::printChoiceBox( 24, 51, 232, 136, 6, COLOR_IDX, false, false );

        for( u8 i = 0; i < MAX_PKMN_PER_BOX; ++i ) {
            res.push_back( IO::inputTarget( POS_X( i ), POS_Y( i ), POS_X( i ) + 28, POS_Y( i ) + 21 ) );
            _ranges.push_back( { oam, res.back( ) } );
            if( box->m_pokemon[ i ].m_speciesId ) {
                if( !box->m_pokemon[ i ].m_individualValues.m_isEgg ) {
                    tileCnt = IO::loadPKMNIcon( box->m_pokemon[ i ].m_speciesId,
                                                POS_X( i ), POS_Y( i ), oam++, pal / 16, pal % 16, tileCnt, false );
                } else
                    tileCnt = IO::loadEggIcon( POS_X( i ), POS_Y( i ), oam++, pal / 16, pal % 16, tileCnt, false );
                ++pal;
            } else {
                IO::OamTop->oamBuffer[ oam++ ].isHidden = true;
                tileCnt += 32;
            }
        }

        IO::printRectangle( 0, 140, 255, 192, false, false, WHITE_IDX );
        IO::printString( IO::regularFont, p_showTeam ? "Pokémon-Team" : "Zwischenablage", 2, 176, false );
        for( u8 i = 0; i < 6; ++i ) {
            res.push_back( IO::inputTarget( TEAM_POS_X( i ), TEAM_POS_Y( i ),
                                            TEAM_POS_X( i ) + 28, TEAM_POS_Y( i ) + 21 ) );
            _ranges.push_back( { oam, res.back( ) } );

            IO::printChoiceBox( TEAM_POS_X( i ), TEAM_POS_Y( i ),
                                TEAM_POS_X( i ) + 28, TEAM_POS_Y( i ) + 21, 3,
                                GRAY_IDX, false, false );

            u16 species = p_showTeam ? FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId
                : FS::SAV->m_clipboard[ i ].m_speciesId;
            bool isEgg = p_showTeam ? FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg
                : FS::SAV->m_clipboard[ i ].m_individualValues.m_isEgg;

            if( species ) {
                if( !isEgg )
                    tileCnt = IO::loadPKMNIcon( species, TEAM_POS_X( i ) - 3, TEAM_POS_Y( i ) - 10,
                                                oam++, pal / 16, pal % 16, tileCnt, false );
                else
                    tileCnt = IO::loadEggIcon( TEAM_POS_X( i ) - 3, TEAM_POS_Y( i ) - 10,
                                               oam++, pal / 16, pal % 16, tileCnt, false );
                ++pal;
            } else {
                IO::OamTop->oamBuffer[ oam++ ].isHidden = true;
                tileCnt += 32;
            }
        }

        drawAllBoxStatus( );
        IO::updateOAM( false );
        return res;
    }

    void boxUI::updateAtHand( touchPosition p_touch, u8 p_oamIdx ) {
        IO::OamTop->oamBuffer[ p_oamIdx ].x = p_touch.px - 16;
        IO::OamTop->oamBuffer[ p_oamIdx ].y = p_touch.py - 16;
        IO::updateOAM( false );
    }
    /*
        u8 boxUI::getSprite( u8 p_oldIdx, u8 p_rangeIdx ) {
            IO::OamTop->oamBuffer[ _ranges[ p_rangeIdx ].first ].priority = OBJPRIORITY_0;
            return _ranges[ p_rangeIdx ].first;
        }

        u8 boxUI::acceptTouch( u8 p_oldIdx, u8 p_rangeIdx, bool p_allowTakePkmn ) {
            return 0;
        }

        u32 boxUI::acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ) {
            if( p_startIdx < MAX_PKMN_PER_BOX ) {
                IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].x = POS_X( p_startIdx ) - 3;
                IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].y = POS_Y( p_startIdx ) - 10;
            } else {
                IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].x = TEAM_POS_X( p_startIdx ) - 3;
                IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].y = TEAM_POS_Y( p_startIdx ) - 10;
            }
            IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].priority = OBJPRIORITY_0;
            IO::updateOAM( false );

            if( u8( -1 ) == p_dropIdx )
                return 0;

            if( p_startIdx >= MAX_PKMN_PER_BOX || p_dropIdx >= MAX_PKMN_PER_BOX ) {
                if( p_startIdx >= MAX_PKMN_PER_BOX ) {
                    IO::OamTop->oamBuffer[ _ranges[ p_dropIdx ].first ].x = TEAM_POS_X( p_startIdx ) - 3;
                    IO::OamTop->oamBuffer[ _ranges[ p_dropIdx ].first ].y = TEAM_POS_Y( p_startIdx ) - 10;
                } else {
                    IO::OamTop->oamBuffer[ _ranges[ p_dropIdx ].first ].x = POS_X( p_startIdx ) - 3;
                    IO::OamTop->oamBuffer[ _ranges[ p_dropIdx ].first ].y = POS_Y( p_startIdx ) - 10;
                }
                if( p_dropIdx >= MAX_PKMN_PER_BOX ) {
                    IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].x = TEAM_POS_X( p_dropIdx ) - 3;
                    IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].y = TEAM_POS_Y( p_dropIdx ) - 10;
                } else {
                    IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].x = POS_X( p_dropIdx ) - 3;
                    IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ].y = POS_Y( p_dropIdx ) - 10;
                }

                std::swap( IO::OamTop->oamBuffer[ _ranges[ p_startIdx ].first ], IO::OamTop->oamBuffer[ _ranges[ p_dropIdx ].first ] );
                IO::updateOAM( false );
            }
            return 0;
        }
    */
    void boxUI::select( u8 p_index ) {
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
        bool holding = !IO::OamTop->oamBuffer[ HELD_PKMN ].isHidden;
        IO::OamTop->oamBuffer[ ARROW_ID ].x = x + 20 - 5 * holding;
        IO::OamTop->oamBuffer[ ARROW_ID ].y = y + 4 - 5 * holding;
        IO::OamTop->oamBuffer[ ARROW_ID ].isHidden = false;

        if( p_index < MAX_PKMN_PER_BOX + 6 ) {
            IO::OamTop->oamBuffer[ p_index + PKMN_START ].x = x;
            IO::OamTop->oamBuffer[ p_index + PKMN_START ].y = y;
        }
        //Held pkmn
        IO::OamTop->oamBuffer[ HELD_PKMN ].x = x - 5;
        IO::OamTop->oamBuffer[ HELD_PKMN ].y = y - 5;
        IO::updateOAM( false );
    }

    void boxUI::takePkmn( u8 p_index, u16 p_heldPkmnIdx, bool p_isEgg ) {
        if( p_index != (u8) -1 ) {
            box* box = FS::SAV->getCurrentBox( );
            auto spr = IO::OamTop->oamBuffer[ PKMN_START + p_index ];

            pokemon::boxPokemon bpm;
            if( p_index < MAX_PKMN_PER_BOX )
                bpm = box->m_pokemon[ p_index ];
            else if( _showTeam )
                bpm = FS::SAV->m_pkmnTeam[ p_index - MAX_PKMN_PER_BOX ].m_boxdata;
            else
                bpm = FS::SAV->m_clipboard[ p_index - MAX_PKMN_PER_BOX ];

            if( bpm.m_speciesId ) {
                u8 pal = p_index + PKMN_PALETTE_START;
                if( !bpm.m_individualValues.m_isEgg ) {
                    IO::loadPKMNIcon( bpm.m_speciesId,
                                      POS_X( p_index ), POS_Y( p_index ),
                                      PKMN_START + p_index, pal / 16, pal % 16, PKMN_TILES_START + 32 * p_index, false );
                } else
                    IO::loadEggIcon( POS_X( p_index ), POS_Y( p_index ), PKMN_START + p_index,
                                     pal / 16, pal % 16, PKMN_TILES_START + 32 * p_index, false );
            } else {
                IO::OamTop->oamBuffer[ PKMN_START + p_index ].isHidden = true;
            }

            if( p_heldPkmnIdx ) {
                if( !p_isEgg )
                    IO::loadPKMNIcon( p_heldPkmnIdx, POS_X( p_index ), POS_Y( p_index ),
                                      HELD_PKMN, 6, 0, 64, false );
                else
                    IO::loadEggIcon( POS_X( p_index ), POS_Y( p_index ), HELD_PKMN,
                                     6, 0, 64, false );
            } else
                IO::OamTop->oamBuffer[ HELD_PKMN ].isHidden = true;
        } else {
            IO::OamTop->oamBuffer[ HELD_PKMN ].isHidden = true;
        }
        select( p_index );
    }

    void boxUI::updateTeam( ) {
        for( u8 i = 0; i < 6; ++i ) {
            u16 species = FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId;
            bool isEgg = FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg;
            u8 pal = MAX_PKMN_PER_BOX + PKMN_PALETTE_START + i;

            if( species ) {
                if( !isEgg )
                    IO::loadPKMNIcon( species, TEAM_POS_X( i ) - 3, TEAM_POS_Y( i ) - 10,
                                      PKMN_START + 18 + i, pal / 16, pal % 16, PKMN_TILES_START + 32 * MAX_PKMN_PER_BOX + 32 * i, false );
                else
                    IO::loadEggIcon( TEAM_POS_X( i ) - 3, TEAM_POS_Y( i ) - 10,
                                     PKMN_START + 18 + i, pal / 16, pal % 16, PKMN_TILES_START + 32 * MAX_PKMN_PER_BOX + 32 * i, false );
            } else
                IO::OamTop->oamBuffer[ PKMN_START + 18 + i ].isHidden = true;
        }
        IO::updateOAM( false );
    }
}
