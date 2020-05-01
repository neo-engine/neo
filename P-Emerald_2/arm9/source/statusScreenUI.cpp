/*
Pokémon neo
------------------------------

file        : statusScreenUI.cpp
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

#include <algorithm>
#include <cstdio>

#include "defines.h"
#include "boxUI.h"
#include "fs.h"
#include "item.h"
#include "move.h"
#include "ribbon.h"
#include "saveGame.h"
#include "sprite.h"
#include "statusScreenUI.h"
#include "uio.h"

#include "Contest.h"
#include "PKMN.h"
#include "atks.h"
#include "memo.h"
#include "time_icon.h"

#include "anti_pokerus_icon.h"
#include "pokerus_icon.h"

#include "A.h"
#include "Back.h"
#include "Backward.h"
#include "Down.h"
#include "Forward.h"
#include "Up.h"

#include "hpbar.h"
#include "itemicon.h"
#include "party_blank1.h"
#include "party_blank2.h"
#include "party_box1.h"
#include "party_box2.h"
#include "party_box_sel1.h"
#include "party_box_sel2.h"
#include "party_fnt1.h"
#include "party_fnt2.h"
#include "party_fnt_sel1.h"
#include "party_fnt_sel2.h"
#include "partybg.h"
#include "status_brn.h"
#include "status_fnt.h"
#include "status_frz.h"
#include "status_par.h"
#include "status_psn.h"
#include "status_shiny.h"
#include "status_slp.h"
#include "status_txc.h"

namespace STS {
    u8 borders[ 6 ][ 2 ] = {{4, 2}, {18, 3}, {4, 9}, {18, 10}, {4, 17}, {18, 18}};

    regStsScreenUI::regStsScreenUI( u8 p_pageMax ) {
        m_pagemax = p_pageMax;
    }

    constexpr u16 partyTopScreenPkmnIconPosY( u8 p_pos ) {
        return ( p_pos & 1 ) * 8 + 2 + 61 * ( p_pos >> 1 );
    }

    u16 initPartyTopScreen( bool p_bottom = false ) {
        IO::vramSetup( );
        if( p_bottom ) {
            IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_512x512, 5, 0 );
            bgSetPriority( IO::bg3sub, 3 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3sub ), 512 * 512 );
        } else {
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_512x512, 5, 0 );
            bgSetPriority( IO::bg3, 3 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 512 * 512 );
        }
        u16* pal = BG_PAL( p_bottom );

        dmaCopy( partybgPal, pal, 3 * 2 );
        IO::initOAMTable( p_bottom );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( WHITE_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( WHITE_IDX, 1 );
        IO::smallFont->setColor( GRAY_IDX, 2 );

        pal[ WHITE_IDX ] = WHITE;
        pal[ GRAY_IDX ]  = GRAY;
        pal[ BLACK_IDX ] = BLACK;
        pal[ BLUE_IDX ]  = RGB( 18, 22, 31 );
        pal[ RED_IDX ]   = RGB( 31, 18, 18 );
        pal[ BLUE2_IDX ] = RGB( 0, 0, 25 );
        pal[ RED2_IDX ]  = RGB( 23, 0, 0 );

        pal[ 240 ] = RGB( 6, 6, 6 );    // hp bar border color
        pal[ 241 ] = RGB( 12, 30, 12 ); // hp bar green 1
        pal[ 242 ] = RGB( 3, 23, 4 );   // hp bar green 2
        pal[ 243 ] = RGB( 30, 30, 12 ); // hp bar yellow 1
        pal[ 244 ] = RGB( 23, 23, 5 );  // hp bar yellow 2
        pal[ 245 ] = RGB( 30, 15, 12 ); // hp bar red 1
        pal[ 246 ] = RGB( 20, 7, 7 );   // hp bar red 2

        u16 tileCnt = 0;
        // preload sprites to avoid position calculations later
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        // left half
        for( size_t i = 0; i < 3; i++ ) {
            u8 pos = 2 * i;

            // background "box"
            tileCnt
                = IO::loadSprite( 6 + 2 * pos, 0, tileCnt, 4, 4 + 61 * i, 64, 64, party_blank1Pal,
                                  party_blank1Tiles, party_blank1TilesLen, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            tileCnt
                = IO::loadSprite( 7 + 2 * pos, 0, tileCnt, 68, 4 + 61 * i, 64, 64, party_blank1Pal,
                                  party_blank2Tiles, party_blank2TilesLen, false, false, false,
                                  OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            // PKMN icon
            tileCnt = IO::loadEggIcon( 3, 2 + 61 * i, 30 + pos, 4 + pos, tileCnt, p_bottom );
            IO::OamTop->oamBuffer[ 30 + pos ].isHidden = true;

            if( i ) {
                // Item icon
                IO::loadSprite( 18 + pos, 10, oam[ 18 ].gfxIndex, 3 + 32 - 9, 2 + 32 - 8 + 61 * i,
                                8, 8, itemiconPal, itemiconTiles, itemiconTilesLen, false, false,
                                true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

                // HP bar
                IO::loadSprite( pos, 12, oam[ 0 ].gfxIndex, 120 - 48, 34 + 61 * i, 64, 32, hpbarPal,
                                hpbarTiles, hpbarTilesLen, false, false, true, OBJPRIORITY_3,
                                p_bottom, OBJMODE_NORMAL );
            } else {
                // Item icon
                tileCnt = IO::loadSprite( 18 + pos, 10, tileCnt, 3 + 32 - 9, 2 + 32 - 8 + 61 * i, 8,
                                          8, itemiconPal, itemiconTiles, itemiconTilesLen, false,
                                          false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

                // HP bar
                tileCnt = IO::loadSprite( pos, 12, tileCnt, 120 - 48, 34 + 61 * i, 64, 32, hpbarPal,
                                          hpbarTiles, hpbarTilesLen, false, false, true,
                                          OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }

            // Status icon
            tileCnt = IO::loadSprite( 24 + pos, 11, tileCnt, 130 - 62 - 22, 33 + 61 * i, 8, 8,
                                      status_parPal, status_parTiles, status_parTilesLen / 2, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            // Shiny icon
            tileCnt = IO::loadSprite( 36 + pos, 13, tileCnt, 130 - 62 - 32, 33 + 61 * i, 8, 8,
                                      status_shinyPal, status_shinyTiles, status_shinyTilesLen,
                                      false, false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        }

        // right half
        for( size_t i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;

            // background "box"
            tileCnt
                = IO::loadSprite( 6 + 2 * pos, 0, tileCnt, 131, 12 + 61 * i, 64, 64,
                                  party_blank1Pal, party_blank1Tiles, party_blank1TilesLen, false,
                                  false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            tileCnt
                = IO::loadSprite( 7 + 2 * pos, 0, tileCnt, 195, 12 + 61 * i, 64, 64,
                                  party_blank1Pal, party_blank2Tiles, party_blank2TilesLen, false,
                                  false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            // PKMN icon
            tileCnt = IO::loadEggIcon( 130, 10 + 61 * i, 30 + pos, 4 + pos, tileCnt, p_bottom );
            IO::OamTop->oamBuffer[ 30 + pos ].isHidden = true;

            // Item icon
            IO::loadSprite( 18 + pos, 10, oam[ 18 ].gfxIndex, 130 + 32 - 9, 10 + 32 - 8 + 61 * i, 8,
                            8, itemiconPal, itemiconTiles, itemiconTilesLen, false, false, true,
                            OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

            // Status icon
            tileCnt = IO::loadSprite( 24 + pos, 11, tileCnt, 257 - 62 - 22, 41 + 61 * i, 8, 8,
                                      status_parPal, status_parTiles, status_parTilesLen / 2, false,
                                      false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            // Shiny icon
            tileCnt = IO::loadSprite( 36 + pos, 13, tileCnt, 257 - 62 - 32, 41 + 61 * i, 8, 8,
                                      status_shinyPal, status_shinyTiles, status_shinyTilesLen,
                                      false, false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

            // HP bar
            IO::loadSprite( pos, 12, oam[ 0 ].gfxIndex, 247 - 48, 42 + 61 * i, 64, 32, hpbarPal,
                            hpbarTiles, hpbarTilesLen, false, false, true, OBJPRIORITY_3, p_bottom,
                            OBJMODE_NORMAL );
        }

        if( p_bottom ) {
            REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA_SUB = 0xff | ( 0x06 << 8 );
        } else {
            REG_BLDCNT   = BLEND_ALPHA | BLEND_DST_BG3;
            REG_BLDALPHA = 0xff | ( 0x06 << 8 );
        }
        bgUpdate( );
        IO::updateOAM( p_bottom );
        return tileCnt;
    }

    void drawPartyPkmn( pokemon* p_pokemon, u8 p_pos, bool p_selected,
                        bool p_redraw = true, bool p_bottom = false ) {
        SpriteEntry* oam      = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          anchor_x = oam[ 6 + 2 * p_pos ].x;
        u16          anchor_y = oam[ 6 + 2 * p_pos ].y;

        if( p_redraw ) {
            IO::printRectangle( anchor_x, anchor_y, anchor_x + 124, anchor_y + 61, p_bottom, 0 );
            oam[ p_pos ].isHidden      = true; // hp bar
            oam[ 18 + p_pos ].isHidden = true; // item22
            oam[ 24 + p_pos ].isHidden = true; // status
            oam[ 30 + p_pos ].isHidden = true; // pkmn
            oam[ 36 + p_pos ].isHidden = true; // shiny
        }

        if( p_pokemon == nullptr ) {
            // No Pkmn -> draw empty box
            SpriteEntry old = oam[ 6 + 2 * p_pos ];
            IO::loadSprite( 6 + 2 * p_pos, 0, old.gfxIndex, old.x, old.y, 64, 64, party_blank1Pal,
                            party_blank1Tiles, party_blank1TilesLen, false, false, false,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            old = oam[ 7 + 2 * p_pos ];
            IO::loadSprite( 7 + 2 * p_pos, 0, old.gfxIndex, old.x, old.y, 64, 64, party_blank1Pal,
                            party_blank2Tiles, party_blank2TilesLen, false, false, false,
                            OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );

            IO::updateOAM( p_bottom );
            return;
        }
        if( p_pokemon->m_stats.m_acHP ) {
            // Pkmn is not fainted
            if( p_selected ) {
                SpriteEntry old = oam[ 6 + 2 * p_pos ];
                IO::loadSprite( 6 + 2 * p_pos, 3, old.gfxIndex, old.x, old.y, 64, 64,
                                party_box_sel1Pal, party_box_sel1Tiles, party_box_sel1TilesLen,
                                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                old = oam[ 7 + 2 * p_pos ];
                IO::loadSprite( 7 + 2 * p_pos, 3, old.gfxIndex, old.x, old.y, 64, 64,
                                party_box_sel1Pal, party_box_sel2Tiles, party_box_sel2TilesLen,
                                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            } else {
                SpriteEntry old = oam[ 6 + 2 * p_pos ];
                IO::loadSprite( 6 + 2 * p_pos, 1, old.gfxIndex, old.x, old.y, 64, 64, party_box1Pal,
                                party_box1Tiles, party_box1TilesLen, false, false, false,
                                OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                old = oam[ 7 + 2 * p_pos ];
                IO::loadSprite( 7 + 2 * p_pos, 1, old.gfxIndex, old.x, old.y, 64, 64, party_box1Pal,
                                party_box2Tiles, party_box2TilesLen, false, false, false,
                                OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
        } else {
            // Pkmn is fainted
            if( p_selected ) {
                SpriteEntry old = oam[ 6 + 2 * p_pos ];
                IO::loadSprite( 6 + 2 * p_pos, 3, old.gfxIndex, old.x, old.y, 64, 64,
                                party_fnt_sel1Pal, party_fnt_sel1Tiles, party_fnt_sel1TilesLen,
                                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                old = oam[ 7 + 2 * p_pos ];
                IO::loadSprite( 7 + 2 * p_pos, 3, old.gfxIndex, old.x, old.y, 64, 64,
                                party_fnt_sel1Pal, party_fnt_sel2Tiles, party_fnt_sel2TilesLen,
                                false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            } else {
                SpriteEntry old = oam[ 6 + 2 * p_pos ];
                IO::loadSprite( 6 + 2 * p_pos, 2, old.gfxIndex, old.x, old.y, 64, 64, party_fnt1Pal,
                                party_fnt1Tiles, party_fnt1TilesLen, false, false, false,
                                OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
                old = oam[ 7 + 2 * p_pos ];
                IO::loadSprite( 7 + 2 * p_pos, 2, old.gfxIndex, old.x, old.y, 64, 64, party_fnt1Pal,
                                party_fnt2Tiles, party_fnt2TilesLen, false, false, false,
                                OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            }
        }

        if( p_pokemon->isEgg( ) ) {
            if( p_redraw ) {
                // general data
                IO::regularFont->printString( GET_STRING( 34 ), anchor_x + 30, anchor_y + 12,
                                              false );
                IO::loadEggIcon( oam[ 30 + p_pos ].x, oam[ 30 + p_pos ].y, 30 + p_pos, 4 + p_pos,
                                 oam[ 30 + p_pos ].gfxIndex, p_bottom );
                bgUpdate( );
            }
        } else {
            if( p_redraw ) {
                // general data
                IO::regularFont->printString( p_pokemon->m_boxdata.m_name, anchor_x + 30,
                                              anchor_y + 12, false );

                if( p_pokemon->m_boxdata.m_isFemale ) {
                    IO::regularFont->setColor( RED_IDX, 1 );
                    IO::regularFont->setColor( RED2_IDX, 2 );
                    IO::regularFont->printString( "}", anchor_x + 109, anchor_y + 12, false );
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                } else if( !p_pokemon->m_boxdata.m_isGenderless ) {
                    IO::regularFont->setColor( BLUE_IDX, 1 );
                    IO::regularFont->setColor( BLUE2_IDX, 2 );
                    IO::regularFont->printString( "{", anchor_x + 109, anchor_y + 12, false );
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }

                // HP
                u8 barWidth = 45 * p_pokemon->m_stats.m_acHP / p_pokemon->m_stats.m_maxHP;
                if( p_pokemon->m_stats.m_acHP * 2 >= p_pokemon->m_stats.m_maxHP ) {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 241, 1 );
                    IO::smallFont->setColor( 242, 3 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                        anchor_y + 32, false, 241 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                        anchor_y + 33, false, 242 );
                } else if( p_pokemon->m_stats.m_acHP * 4 >= p_pokemon->m_stats.m_maxHP ) {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 243, 1 );
                    IO::smallFont->setColor( 244, 3 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                        anchor_y + 32, false, 243 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                        anchor_y + 33, false, 244 );
                } else {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 245, 1 );
                    IO::smallFont->setColor( 246, 3 );
                    if( p_pokemon->m_stats.m_acHP ) {
                        IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                            anchor_y + 32, false, 245 );
                        IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                            anchor_y + 33, false, 246 );
                    }
                }

                IO::smallFont->printString( GET_STRING( 186 ), anchor_x + 116 - 62, anchor_y + 20,
                                            false ); // HP "icon"
                IO::smallFont->setColor( WHITE_IDX, 1 );
                IO::smallFont->setColor( GRAY_IDX, 2 );

                IO::smallFont->printString( "!", anchor_x + 15, anchor_y + 33, false );

                char buffer[ 10 ];
                snprintf( buffer, 8, "%d", p_pokemon->m_level );

                IO::smallFont->printString( buffer, anchor_x + 24, anchor_y + 32, false );

                snprintf( buffer, 8, "%3d", p_pokemon->m_stats.m_acHP );
                IO::smallFont->printString( buffer, anchor_x + 116 - 32 - 24, anchor_y + 32,
                                            false );
                snprintf( buffer, 8, "/%d", p_pokemon->m_stats.m_maxHP );
                IO::smallFont->printString( buffer, anchor_x + 116 - 32, anchor_y + 32, false );

                // update sprites
                oam[ p_pos ].isHidden      = false;
                oam[ 18 + p_pos ].isHidden = !p_pokemon->m_boxdata.getItem( ); // item

                oam[ 36 + p_pos ].isHidden = !p_pokemon->isShiny( ); // shiny status

                // other status conditions
                if( !p_pokemon->m_stats.m_acHP ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_fntPal, status_fntTiles,
                                    status_fntTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( p_pokemon->m_status.m_isParalyzed ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_parPal, status_parTiles,
                                    status_parTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( p_pokemon->m_status.m_isAsleep ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_slpPal, status_slpTiles,
                                    status_slpTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( p_pokemon->m_status.m_isBadlyPoisoned ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_txcPal, status_txcTiles,
                                    status_txcTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( p_pokemon->m_status.m_isBurned ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_brnPal, status_brnTiles,
                                    status_brnTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( p_pokemon->m_status.m_isFrozen ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_frzPal, status_frzTiles,
                                    status_frzTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( p_pokemon->m_status.m_isPoisoned ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_psnPal, status_psnTiles,
                                    status_psnTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                }
                IO::updateOAM( p_bottom ); // Shipout fast stuff first
                bgUpdate( );
                IO::loadPKMNIcon( p_pokemon->m_boxdata.m_speciesId, oam[ 30 + p_pos ].x,
                                  oam[ 30 + p_pos ].y, 30 + p_pos, 4 + p_pos,
                                  oam[ 30 + p_pos ].gfxIndex, p_bottom, p_pokemon->getForme( ),
                                  p_pokemon->isShiny( ), p_pokemon->isFemale( ) );
            }
        }

        IO::updateOAM( p_bottom );
    }

    void animateBG( u8 p_frame, bool p_bottom ) {
        if( ( p_frame & 255 ) == 255 ) {
            bgScrollf( p_bottom ? IO::bg3sub : IO::bg3, -( 255 << 6 ), -( 255 << 6 ) );
        }
        bgScrollf( p_bottom ? IO::bg3sub : IO::bg3, 1 << 6, 1 << 6 );
    }

    void animatePartyPkmn( u8 p_frame, u8 p_selection, bool p_bottom = false ) {
        if( ( p_frame & 3 ) != 3 ) { return; } // Only do something every fourth frame

        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        if( ( p_frame & 7 ) != 7 ) {
            oam[ 30 + p_selection ].y =
                partyTopScreenPkmnIconPosY( p_selection ) + ( ( p_frame >> 3 ) & 1 ) * 4 - 2;
        } else {
            for( u8 i = 0; i < 6; i++ ) {
                oam[ 30 + i ].y =
                    partyTopScreenPkmnIconPosY( i ) + ( ( p_frame >> 3 ) & 1 ) * 4 - 2;
            }
        }
        IO::updateOAM( p_bottom );
    }

    // OAMTop indices
#define ICON_IDX( a ) ( a )
#define SHINY_IDX 100
#define PKMN_SPRITE_START 6
#define PAGE_ICON_IDX 10
#define ITEM_ICON_IDX 11
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

    // OamSub indices
    //#define BACK_ID  0
#define SUB_PAGE_ICON_IDX( a ) ( 1 + ( a ) )
#define SUB_BALL_IDX( a ) ( 7 + ( a ) )
    //#define FWD_ID 13
    //#define BWD_ID 14

#define HP_COL 238
    pkmnData data;
    void regStsScreenUI::initSub( ) {
        IO::initOAMTable( true );
        u16 nextAvailableTileIdx = 0;

        nextAvailableTileIdx = IO::loadSprite(
            BACK_ID, BACK_ID, nextAvailableTileIdx, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32,
            BackPal, BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx
            = IO::loadSprite( SUB_PAGE_ICON_IDX( 0 ), SUB_PAGE_ICON_IDX( 0 ), nextAvailableTileIdx,
                              0, 0, 32, 32, time_iconPal, time_iconTiles, time_iconTilesLen, false,
                              false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite(
            SUB_PAGE_ICON_IDX( 1 ), SUB_PAGE_ICON_IDX( 1 ), nextAvailableTileIdx, 0, 0, 32, 32,
            PKMNPal, PKMNTiles, PKMNTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite(
            SUB_PAGE_ICON_IDX( 2 ), SUB_PAGE_ICON_IDX( 2 ), nextAvailableTileIdx, 0, 0, 32, 32,
            memoPal, memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite(
            SUB_PAGE_ICON_IDX( 3 ), SUB_PAGE_ICON_IDX( 3 ), nextAvailableTileIdx, 0, 0, 32, 32,
            atksPal, atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = IO::loadSprite(
            SUB_PAGE_ICON_IDX( 4 ), SUB_PAGE_ICON_IDX( 4 ), nextAvailableTileIdx, 0, 0, 32, 32,
            ContestPal, ContestTiles, ContestTilesLen, false, false, false, OBJPRIORITY_0, true );
        for( u8 i = 0; i < 5; ++i ) {
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].x = 48 + 32 * i;
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].y = -2 * i;
        }

        for( u8 i = 0; i < 6; ++i ) {
            auto pkmn = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata;
            if( pkmn.isEgg( ) )
                nextAvailableTileIdx = IO::loadEggIcon( 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ),
                                                        SUB_BALL_IDX( i ), nextAvailableTileIdx );
            else
                nextAvailableTileIdx = IO::loadPKMNIcon( 0, 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ),
                                                        SUB_BALL_IDX( i ), nextAvailableTileIdx );
            IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].isHidden = true;
        }

        nextAvailableTileIdx = IO::loadSprite(
            FWD_ID, FWD_ID, nextAvailableTileIdx, SCREEN_WIDTH - 28 - 25, SCREEN_HEIGHT - 25, 32,
            32, DownPal, DownTiles, DownTilesLen, false, false, false, OBJPRIORITY_1, true );
        nextAvailableTileIdx = IO::loadSprite(
            BWD_ID, BWD_ID, nextAvailableTileIdx, SCREEN_WIDTH - 25, SCREEN_HEIGHT - 28 - 25, 32,
            32, UpPal, UpTiles, UpTilesLen, false, false, false, OBJPRIORITY_1, true );
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        IO::updateOAM( true );
    }

    void regStsScreenUI::initTop( ) {
        u16 tileCnt = initPartyTopScreen( false );

        for( u8 i = 0; i < 6; i++ ) {
            if( i < SAVE::SAV->getActiveFile( ).getTeamPkmnCount( ) ) {
                drawPartyPkmn( &SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ], i, i == _current );
            } else {
                drawPartyPkmn( nullptr, i, !i );
            }
        }
    }

    void regStsScreenUI::init( u8 p_current, bool p_initTop ) {
        _current = p_current;
        IO::NAV->draw( );
        if( p_initTop ) initTop( );
        initSub( );
    }

    void regStsScreenUI::animate( u8 p_frame, u8 p_page ) {
		if (p_page == 0) {// party overview
            animatePartyPkmn( p_frame, this->_current );
		}
        animateBG( p_frame, false );
        bgUpdate( );
    }

    void hideSprites( bool p_bottom ) {
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        for( u8 i = 0; i < 6; ++i ) Oam->oamBuffer[ ICON_IDX( i ) ].isHidden = true;
        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
        for( u8 i = 0; i < 12; ++i ) Oam->oamBuffer[ RIBBON_IDX + i ].isHidden = true;
    }

    void drawPkmnInformation( pokemon& p_pokemon, u8& p_page, bool p_newpok, bool p_bottom ) {
        dmaFillWords( 0, bgGetGfxPtr( p_bottom ? IO::bg2sub : IO::bg2 ), 256 * 192 );
        auto Oam = p_bottom ? IO::Oam : IO::OamTop;
        auto pal = BG_PAL( p_bottom );
        if( p_newpok ) {
            IO::loadItemIcon( !p_pokemon.m_boxdata.m_ball
                                  ? "Pokeball"
                                  : ItemList[ p_pokemon.m_boxdata.m_ball ]->m_itemName,
                              -6, 22, SHINY_IDX, SHINY_PAL, 1000, p_bottom );
        }

        data = getPkmnData( p_pokemon.m_boxdata.m_speciesId, p_pokemon.getForme( ) );

        if( !p_pokemon.isEgg( ) ) {
            pal[ RED_IDX ]   = RED;
            pal[ BLUE_IDX ]  = BLUE;
            pal[ RED2_IDX ]  = RED2;
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


            IO::regularFont->printString( getDisplayName( p_pokemon.m_boxdata.m_speciesId,
                        CURRENT_LANGUAGE ).c_str( ), 160, 13, p_bottom );

            if( p_pokemon.m_boxdata.getItem( ) ) {
                IO::regularFont->printString( "Item", 2, 176, p_bottom );
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
                char buffer[ 200 ];
                snprintf(
                    buffer, 199, "%s: %s",
                    ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ),
                    ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getShortDescription( ).c_str( ) );
                IO::regularFont->printString( buffer, 40, 159, p_bottom );
                if( p_newpok ) {
                    IO::loadItemIcon( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->m_itemName, 2,
                                      152, ITEM_ICON_IDX, ITEM_ICON_PAL,
                                      Oam->oamBuffer[ ITEM_ICON_IDX ].gfxIndex, p_bottom );
                }
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( 0, 2 );
                IO::regularFont->printString(
                    ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( ).c_str( ), 56, 168,
                    p_bottom );
                Oam->oamBuffer[ ITEM_ICON_IDX ].isHidden = true;
            }
            if( p_pokemon.m_boxdata.isShiny( ) ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( WHITE_IDX, 2 );
                IO::regularFont->printString( "*", 1, 47, p_bottom );
                IO::regularFont->setColor( WHITE_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );

            if( p_newpok )
                if( !IO::loadPKMNSprite(
                        p_pokemon.m_boxdata.m_speciesId, 16, 44,
                        PKMN_SPRITE_START, PKMN_SPRITE_PAL,
                        Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom,
                        p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale, true,
                        false, p_pokemon.getForme( ) ) ) {
                    IO::loadPKMNSprite(
                        p_pokemon.m_boxdata.m_speciesId, 16, 44,
                        PKMN_SPRITE_START, PKMN_SPRITE_PAL,
                        Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom,
                        p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale, true,
                        false, p_pokemon.getForme( ) );
                }

            u16 exptype = data.m_expTypeFormeCnt >> 5;

            IO::displayHP( 100, 101, 46, 76, HP_COL, HP_COL + 1, false, 50, 56, p_bottom );
            IO::displayHP( 100, 100 - p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP,
                           46, 76, HP_COL, HP_COL + 1, false, 50, 56, p_bottom );

            IO::displayEP( 100, 101, 46, 76, HP_COL + 2, HP_COL + 3, false, 59, 62, p_bottom );
            IO::displayEP(
                0,
                ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][ exptype ] )
                    * 100
                    / ( EXP[ p_pokemon.m_level ][ exptype ]
                        - EXP[ p_pokemon.m_level - 1 ][ exptype ] ),
                46, 76, HP_COL + 2, HP_COL + 3, false, 59, 62, p_bottom );

            IO::regularFont->setColor( 0, 2 );
            IO::regularFont->setColor( HP_COL + 2, 1 );
            char buffer[ 50 ];
            snprintf(
                buffer, 49, "EP %lu%%",
                ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][ exptype ] )
                    * 100
                    / ( EXP[ p_pokemon.m_level ][ exptype ]
                        - EXP[ p_pokemon.m_level - 1 ][ exptype ] ) );
            IO::regularFont->printString( buffer, 62, 28, p_bottom, IO::font::CENTER );
            snprintf( buffer, 49, "%s %i%%", GET_STRING( 126 ),
                      p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP );
            IO::regularFont->printString( buffer, 62, 38, p_bottom, IO::font::CENTER );
            IO::regularFont->setColor( GRAY_IDX, 2 );
            IO::regularFont->setColor( BLACK_IDX, 1 );
        } else {
            p_page           = -1;
            pal[ WHITE_IDX ] = WHITE;
            IO::regularFont->setColor( BLACK_IDX, 1 );

            IO::regularFont->printString( GET_STRING( 34 ), 150, 0, p_bottom );
            IO::regularFont->printChar( '/', 234, 0, p_bottom );
            IO::regularFont->printString( GET_STRING( 34 ), 160, 13, p_bottom );

            IO::loadEggSprite( 29, 60, PKMN_SPRITE_START, PKMN_SPRITE_PAL,
                               Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex, p_bottom );
            for( u8 i = 1; i < 4; ++i ) Oam->oamBuffer[ PKMN_SPRITE_START + i ].isHidden = true;
            Oam->oamBuffer[ ITEM_ICON_IDX ].isHidden = true;
        }
    }

    void drawPkmnStats( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
        auto pal      = BG_PAL( p_bottom );

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );

        IO::regularFont->printString( GET_STRING( 137 ), 32, 0, p_bottom );
        if( !( currPkmn.isEgg( ) ) ) {

            pal[ RED_IDX ]   = RED;
            pal[ BLUE_IDX ]  = BLUE;
            pal[ RED2_IDX ]  = RED2;
            pal[ BLUE2_IDX ] = BLUE2;

            char buffer[ 50 ];
            snprintf( buffer, 49, GET_STRING( 138 ), currPkmn.m_level );
            IO::regularFont->printString( buffer, 110, 30, p_bottom );

            snprintf( buffer, 49, "KP                     %3i", currPkmn.m_stats.m_maxHP );
            IO::regularFont->printString( buffer, 130, 46, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 0 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 0 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 );
                IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            snprintf( buffer, 49, "ANG                   %3i", currPkmn.m_stats.m_Atk );
            IO::regularFont->printString( buffer, 130, 65, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 1 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 1 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 );
                IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            snprintf( buffer, 49, "VER                   %3i", currPkmn.m_stats.m_Def );
            IO::regularFont->printString( buffer, 130, 82, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 3 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 3 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 );
                IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            snprintf( buffer, 49, "SAN                   %3i", currPkmn.m_stats.m_SAtk );
            IO::regularFont->printString( buffer, 130, 99, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 4 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 4 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 );
                IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            snprintf( buffer, 49, "SVE                   %3i", currPkmn.m_stats.m_SDef );
            IO::regularFont->printString( buffer, 130, 116, p_bottom );

            if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 2 ] == 1.1 ) {
                IO::regularFont->setColor( RED_IDX, 1 );
                IO::regularFont->setColor( RED2_IDX, 2 );
            } else if( NatMod[ currPkmn.m_boxdata.getNature( ) ][ 2 ] == 0.9 ) {
                IO::regularFont->setColor( BLUE_IDX, 1 );
                IO::regularFont->setColor( BLUE2_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }
            snprintf( buffer, 49,
                      "INI                   \xC3\xC3"
                      "%3i",
                      currPkmn.m_stats.m_Spd );
            IO::regularFont->printString( buffer, 130, 133, p_bottom );

            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
            IO::printRectangle( (u8) 158, (u8) 48, u8( 158 + 68 ), u8( 48 + 12 ), p_bottom,
                                (u8) 251 );

            IO::printRectangle( (u8) 158, (u8) 48,
                                u8( 158 + ( 68.0 * currPkmn.m_boxdata.IVget( 0 ) / 31 ) ),
                                u8( 48 + 6 ), p_bottom, 230 );
            IO::printRectangle( (u8) 158, u8( 48 + 6 ),
                                u8( 158 + ( 68.0 * currPkmn.m_boxdata.m_effortValues[ 0 ] / 252 ) ),
                                u8( 48 + 12 ), p_bottom, 230 );

            for( int i = 1; i < 6; ++i ) {
                IO::printRectangle( (u8) 158, u8( 50 + ( 17 * i ) ), u8( 158 + 68 ),
                                    u8( 50 + 12 + ( 17 * i ) ), p_bottom, (u8) 251 );
                IO::printRectangle( (u8) 158, u8( 50 + ( 17 * i ) ),
                                    u8( 158 + ( 68.0 * currPkmn.m_boxdata.IVget( i ) / 31 ) ),
                                    u8( 50 + 6 + ( 17 * i ) ), p_bottom, 230 + i );
                IO::printRectangle(
                    (u8) 158, u8( 50 + 6 + ( 17 * i ) ),
                    u8( 158 + ( 68.0 * currPkmn.m_boxdata.m_effortValues[ i ] / 252 ) ),
                    u8( 50 + 12 + ( 17 * i ) ), p_bottom, 230 + i );
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
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
        auto pal      = BG_PAL( p_bottom );

        IO::regularFont->printString( "Attacken", 32, 0, p_bottom );
        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );

        for( int i = 0; i < 4; i++ ) {
            if( !currPkmn.m_boxdata.m_moves[ i ] ) continue;
            type t = AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_moveType;
            IO::loadTypeIcon( t, 222, 38 + 30 * i, TYPE_IDX + i, TYPE_PAL( i ),
                              Oam->oamBuffer[ TYPE_IDX + i ].gfxIndex, p_bottom,
                              SAVE::SAV->getActiveFile( ).m_options.m_language );

            pal[ COLOR_IDX ] = GREEN;
            if( t == data.m_baseForme.m_types[ 0 ] || t == data.m_baseForme.m_types[ 1 ] ) {
                IO::regularFont->setColor( COLOR_IDX, 1 );
                IO::regularFont->setColor( WHITE_IDX, 2 );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
            }

            IO::regularFont->printString(
                AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ), 128,
                30 + 30 * i, p_bottom );

            IO::regularFont->setColor( GRAY_IDX, 1 );
            IO::regularFont->setColor( WHITE_IDX, 2 );
            char buffer[ 50 ];
            snprintf( buffer, 49, "AP %2hhu/%2hhu ", currPkmn.m_boxdata.m_acPP[ i ],
                      s8( AttackList[ currPkmn.m_boxdata.m_moves[ i ] ]->m_movePP
                          * ( ( 5 + currPkmn.m_boxdata.PPupget( i ) ) / 5.0 ) ) );
            IO::regularFont->printString( buffer, 135, 45 + 30 * i, p_bottom );
        }
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
    }

    void drawPkmnRibbons( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, ContestPal, ContestTiles, ContestTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );
        IO::regularFont->printString( "Bänder", 32, 0, p_bottom );

        auto rbs = ribbon::getRibbons( currPkmn );
        // Just draw the first 12 ribbons at max
        u16 tileCnt = Oam->oamBuffer[ TYPE_IDX ].gfxIndex;
        for( u8 i = 0; i < std::min( rbs.size( ), 12u ); ++i ) {
            u16 tmp = IO::loadRibbonIcon( rbs[ i ], 128 + 32 * ( i % 4 ), 36 + 40 * ( i / 4 ),
                                          RIBBON_IDX + i, TYPE_PAL( i ) % 16, tileCnt, p_bottom );
            if( !tmp ) // Draw an egg when something goes wrong
                tileCnt = IO::loadEggIcon( 128 + 32 * ( i % 4 ), 32 + 40 * ( i / 4 ),
                                           RIBBON_IDX + i, TYPE_PAL( i ) % 16, tileCnt, p_bottom );
            else
                tileCnt = tmp;
        }
        if( rbs.empty( ) ) {
            IO::regularFont->printString( "Keine Bänder", 148, 83, p_bottom );
        } else {
            char buffer[ 49 ];
            snprintf( buffer, 49, "(%u)", rbs.size( ) );
            IO::regularFont->printString( buffer, 88, 0, p_bottom );
        }
    }

    void drawPkmnGeneralData( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );
        IO::regularFont->printString( "Trainer-Memo", 32, 0, p_bottom );

        if( data.m_baseForme.m_types[ 0 ] == data.m_baseForme.m_types[ 1 ] ) {
            IO::loadTypeIcon( data.m_baseForme.m_types[ 0 ], 250 - 32, 50, TYPE_IDX, TYPE_PAL( 0 ),
                              Oam->oamBuffer[ TYPE_IDX ].gfxIndex, p_bottom,
                              SAVE::SAV->getActiveFile( ).m_options.m_language );
            Oam->oamBuffer[ TYPE_IDX + 1 ].isHidden = true;
        } else {
            IO::loadTypeIcon( data.m_baseForme.m_types[ 0 ], 250 - 64, 50, TYPE_IDX, TYPE_PAL( 0 ),
                              Oam->oamBuffer[ TYPE_IDX ].gfxIndex, p_bottom,
                              SAVE::SAV->getActiveFile( ).m_options.m_language );
            IO::loadTypeIcon( data.m_baseForme.m_types[ 1 ], 250 - 32, 50, TYPE_IDX + 1, TYPE_PAL( 1 ),
                              Oam->oamBuffer[ TYPE_IDX + 1 ].gfxIndex, p_bottom,
                              SAVE::SAV->getActiveFile( ).m_options.m_language );
        }

        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );

        IO::regularFont->printString( "OT", 110, 30, p_bottom );
        if( currPkmn.m_boxdata.m_oTisFemale ) {
            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->setColor( RED2_IDX, 2 );
        } else {
            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->setColor( BLUE2_IDX, 2 );
        }
        char buffer[ 50 ];
        snprintf( buffer, 49, "%s/%05d", currPkmn.m_boxdata.m_oT, currPkmn.m_boxdata.m_oTId );
        IO::regularFont->printString( buffer, 250, 30, p_bottom, IO::font::RIGHT );

        if( !currPkmn.m_boxdata.isShiny( ) ) {
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        }
        snprintf( buffer, 49, "%03d", currPkmn.m_boxdata.m_speciesId );
        IO::regularFont->printString( buffer, 180, 51, p_bottom, IO::font::RIGHT );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->printString( "Nr.", 124, 51, p_bottom );

        bool plrOT = currPkmn.m_boxdata.m_oTId == SAVE::SAV->getActiveFile( ).m_id
                     && currPkmn.m_boxdata.m_oTSid == SAVE::SAV->getActiveFile( ).m_sid;
        if( !currPkmn.m_boxdata.m_gotDate[ 0 ] )
            snprintf( buffer, 49, "%s%d", plrOT ? "Gef. auf Lv. " : "Off gef auf Lv.",
                      currPkmn.m_boxdata.m_gotLevel );
        else
            snprintf( buffer, 49, "%s", plrOT ? "Ei erhalten" : "Ei offenbar erh." );
        IO::regularFont->printString( buffer, 250, 76, p_bottom, IO::font::RIGHT, 14 );
        snprintf( buffer, 49, "am %02d.%02d.20%02d,",
                  currPkmn.m_boxdata.m_gotDate[ 0 ] ? currPkmn.m_boxdata.m_gotDate[ 0 ]
                                                    : currPkmn.m_boxdata.m_hatchDate[ 0 ],
                  currPkmn.m_boxdata.m_gotDate[ 1 ] ? currPkmn.m_boxdata.m_gotDate[ 1 ]
                                                    : currPkmn.m_boxdata.m_hatchDate[ 1 ],
                  currPkmn.m_boxdata.m_gotDate[ 2 ] ? currPkmn.m_boxdata.m_gotDate[ 2 ]
                                                    : currPkmn.m_boxdata.m_hatchDate[ 2 ] );
        IO::regularFont->printString( buffer, 250, 90, p_bottom, IO::font::RIGHT, 14 );
        snprintf( buffer, 49, "%s.", FS::getLocation( currPkmn.m_boxdata.m_gotPlace ).c_str( ) );
        IO::regularFont->printMaxString(
            buffer, std::max( (u32) 124, 250 - IO::regularFont->stringWidth( buffer ) ), 104,
            p_bottom, 254 );

        if( currPkmn.m_boxdata.m_gotDate[ 0 ] ) {
            snprintf( buffer, 49, "Geschl. %02d.%02d.20%02d,", currPkmn.m_boxdata.m_hatchDate[ 0 ],
                      currPkmn.m_boxdata.m_hatchDate[ 1 ], currPkmn.m_boxdata.m_hatchDate[ 2 ] );
            IO::regularFont->printString( buffer, 250, 125, p_bottom, IO::font::RIGHT, 14 );
            snprintf( buffer, 49, "%s.",
                      FS::getLocation( currPkmn.m_boxdata.m_hatchPlace ).c_str( ) );
            IO::regularFont->printString( buffer, 250, 139, p_bottom, IO::font::RIGHT, 14 );
        } else if( plrOT && currPkmn.m_boxdata.m_fateful )
            IO::regularFont->printString( "Schicksalhafte Begeg.", 102, 139, p_bottom );
        else if( currPkmn.m_boxdata.m_fateful )
            IO::regularFont->printString( "Off. schicksal. Begeg.", 102, 139, p_bottom );
    }

    void drawPkmnNature( const pokemon& p_pokemon, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;

        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, memoPal, memoTiles, memoTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );
        IO::regularFont->printString( "Pokémon-Info", 32, 0, p_bottom );

        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );

        char buffer[ 50 ];
        snprintf( buffer, 49,
                  "Mag %s"
                  "e PokéRg.",
                  GET_STRING( 242 + currPkmn.m_boxdata.getTasteStr( ) ) );
        IO::regularFont->printString( buffer, 250, 30, p_bottom, IO::font::RIGHT );

        snprintf( buffer, 49, "Sehr %s; %s.",
                  GET_STRING( 187 + currPkmn.m_boxdata.getNature( ) ),
                  GET_STRING( 212 + currPkmn.m_boxdata.getPersonality( ) ) );
        auto str  = std::string( buffer );
        auto nStr = FS::breakString( str, IO::regularFont, 122 );
        IO::regularFont->printString( nStr.c_str( ), 245, 48, p_bottom, IO::font::RIGHT, 14 );

        auto curAbName = getAbilityName( currPkmn.m_boxdata.m_ability, CURRENT_LANGUAGE );
        u8   wd        = IO::regularFont->stringWidth( curAbName.c_str( ) );
        if( 250 - wd > 140 )
            IO::regularFont->printString( "Fäh. ", 250 - wd, 94, p_bottom, IO::font::RIGHT );
        u8 nlCnt = 0;
        /*
        nStr     = FS::breakString( acAbility.m_flavourText, IO::regularFont, 130 );
        for( auto c : nStr )
            if( c == '\n' ) nlCnt++;
        IO::regularFont->printString( nStr.c_str( ), 250, 108, p_bottom, IO::font::RIGHT,
                                      u8( 16 - 2 * nlCnt ) );
        */

        if( currPkmn.m_boxdata.m_oTisFemale ) {
            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->setColor( RED2_IDX, 2 );
        } else {
            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->setColor( BLUE2_IDX, 2 );
        }
        IO::regularFont->printString( curAbName.c_str( ), 250 - wd, 94, p_bottom );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
    }

    // Draw extra information about the specified move
    bool statusScreenUI::drawMove( const pokemon& p_pokemon, u8 p_moveIdx, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
        auto pal      = BG_PAL( p_bottom );

        if( !currPkmn.m_boxdata.m_moves[ p_moveIdx ] ) return false;
        if( currPkmn.isEgg( ) ) return false;

        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;

        pal[ WHITE_IDX ] = WHITE;
        pal[ GRAY_IDX ]  = RGB( 20, 20, 20 );
        pal[ BLACK_IDX ] = BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        u8 isNotEgg = 1;
        drawPkmnInformation( currPkmn, isNotEgg, false, p_bottom );
        if( !isNotEgg ) return false; // This should never occur

        IO::regularFont->printString( "Attackeninfos", 32, 0, p_bottom );
        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );

        move* currMove = AttackList[ currPkmn.m_boxdata.m_moves[ p_moveIdx ] ];

        pal[ COLOR_IDX ] = GREEN;
        if( currMove->m_moveType == data.m_baseForme.m_types[ 0 ]
            || currMove->m_moveType == data.m_baseForme.m_types[ 1 ] ) {
            IO::regularFont->setColor( COLOR_IDX, 1 );
            IO::regularFont->setColor( WHITE_IDX, 2 );
        } else {
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        }
        IO::regularFont->printString( currMove->m_moveName.c_str( ), 120, 32, p_bottom );
        IO::regularFont->setColor( GRAY_IDX, 1 );
        IO::regularFont->setColor( WHITE_IDX, 2 );

        IO::loadTypeIcon( currMove->m_moveType, 222, 30, TYPE_IDX + p_moveIdx,
                          TYPE_PAL( p_moveIdx ), Oam->oamBuffer[ TYPE_IDX + p_moveIdx ].gfxIndex,
                          p_bottom, SAVE::SAV->getActiveFile( ).m_options.m_language );
        IO::loadDamageCategoryIcon(
            currMove->m_moveHitType, 222, 46, ATK_DMGTYPE_IDX( currMove->m_moveHitType ),
            DMG_TYPE_PAL( currMove->m_moveHitType ),
            Oam->oamBuffer[ ATK_DMGTYPE_IDX( currMove->m_moveHitType ) ].gfxIndex, p_bottom );
        char buffer[ 20 ];

        snprintf(
            buffer, 19,
            "AP %2hhu"
            "/"
            "%2hhu ",
            currPkmn.m_boxdata.m_acPP[ p_moveIdx ],
            currMove->m_movePP
                * ( ( 5 + ( ( currPkmn.m_boxdata.m_pPUps >> ( 2 * p_moveIdx ) ) % 4 ) ) / 5 ) );
        IO::regularFont->printString( buffer, 128, 47, p_bottom );

        IO::regularFont->printString( "Stärke", 128, 60, p_bottom );
        if( currMove->m_moveBasePower )
            snprintf( buffer, 19, "%3i", currMove->m_moveBasePower );
        else
            snprintf( buffer, 19, "---" );
        IO::regularFont->printString( buffer, 226, 60, p_bottom );

        IO::regularFont->printString( "Genauigkeit", 128, 72, p_bottom );
        if( currMove->m_moveAccuracy )
            snprintf( buffer, 19, "%3i", currMove->m_moveAccuracy );
        else
            snprintf( buffer, 19, "---" );
        IO::regularFont->printString( buffer, 226, 72, p_bottom );

        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->printString(
            FS::breakString( currMove->description( ), IO::regularFont, 120 ).c_str( ), 128, 84,
            p_bottom, IO::font::LEFT, 11 );

        IO::updateOAM( p_bottom );
        return true;
    }

    // Draw extra information about the specified ribbon
    bool statusScreenUI::drawRibbon( const pokemon& p_pokemon, u8 p_ribbonIdx, bool p_bottom ) {
        auto currPkmn = p_pokemon;
        auto Oam      = p_bottom ? IO::Oam : IO::OamTop;
        auto pal      = BG_PAL( p_bottom );

        if( currPkmn.isEgg( ) ) return false;

        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ TYPE_IDX + i ].isHidden = true;
        for( u8 i = 0; i < 4; ++i ) Oam->oamBuffer[ ATK_DMGTYPE_IDX( i ) ].isHidden = true;
        for( u8 i = 0; i < 12; ++i ) Oam->oamBuffer[ RIBBON_IDX + i ].isHidden = true;

        pal[ WHITE_IDX ] = WHITE;
        pal[ GRAY_IDX ]  = RGB( 20, 20, 20 );
        pal[ BLACK_IDX ] = BLACK;
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        u8 isNotEgg = 1;
        drawPkmnInformation( currPkmn, isNotEgg, false, p_bottom );
        if( isNotEgg == (u8) -1 ) return false;

        IO::regularFont->printString( "Bandinfos", 32, 4, p_bottom );
        IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, Oam->oamBuffer[ PAGE_ICON_IDX ].gfxIndex, 0,
                        -5, 32, 32, atksPal, atksTiles, atksTilesLen, false, false, false,
                        OBJPRIORITY_0, p_bottom );

        ribbon currRb = RibbonList[ p_ribbonIdx ];

        u16 tileCnt = Oam->oamBuffer[ TYPE_IDX ].gfxIndex;

        if( !IO::loadRibbonIcon( p_ribbonIdx, 226, 28, RIBBON_IDX, TYPE_PAL( 0 ), tileCnt,
                                 p_bottom ) )
            tileCnt = IO::loadEggIcon( 226, 28, RIBBON_IDX, TYPE_PAL( 0 ), tileCnt, p_bottom );

        u8   nlCnt = 0;
        auto nStr  = FS::breakString( currRb.m_name == "" ? ( "----" ) : currRb.m_name,
                                     IO::regularFont, 110 );
        for( auto c : nStr )
            if( c == '\n' ) nlCnt++;

        if( currRb.m_name == "" ) currRb.m_description = "----";

        IO::regularFont->printString( nStr.c_str( ), 226, 39 - 7 * nlCnt, p_bottom, IO::font::RIGHT,
                                      14 );
        IO::regularFont->printString(
            FS::breakString( ( currRb.m_description == "" )
                                 ? ( currPkmn.m_boxdata.m_fateful
                                         ? "Ein Band für Pokémon-Freunde."
                                         : "Ein Gedenk-Band. An einem mysteriösen Ort erhalten." )
                                 : currRb.m_description,
                             IO::regularFont, 120 )
                .c_str( ),
            250, 66, p_bottom, IO::font::RIGHT, 14 );
        IO::updateOAM( p_bottom );
        return true;
    }
    void regStsScreenUI::draw( pokemon& p_pokemon, u8 p_page, bool p_newpok ) {
        hideSprites( false );

        IO::setDefaultConsoleTextColors( BG_PALETTE, 230 );
        // FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PKMNInfoScreen", 420 );
        IO::updateOAM( false );

        IO::regularFont->setColor( 0, 0 );
        {
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        }
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ]  = RGB( 20, 20, 20 );
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        if( p_newpok ) {
            BG_PALETTE_SUB[ GRAY_IDX ]  = RGB( 10, 10, 10 );
            BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        }
        auto currPkmn = p_pokemon;

        drawPkmnInformation( currPkmn, p_page, p_newpok, false );
        if( p_page == (u8) -1 ) p_page = 2;
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
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].isHidden
                = currPkmn.isEgg( );
        IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( p_page ) ].isHidden
            = !currPkmn.isEgg( );

        if( p_newpok ) {
            IO::Oam->oamBuffer[ FWD_ID ].isHidden = false;
            IO::Oam->oamBuffer[ BWD_ID ].isHidden = false;
        }
        IO::updateOAM( true );
        IO::updateOAM( false );
    }

    std::vector<IO::inputTarget> regStsScreenUI::draw( u8 p_current, bool p_updatePageIcons ) {
        drawPartyPkmn( &SAVE::SAV->getActiveFile( ).m_pkmnTeam[ _current ], _current, false,
                       false );
        _current  = p_current;
        auto pkmn = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ p_current ];
        drawPartyPkmn( &pkmn, p_current, true, false );

        std::vector<IO::inputTarget> res;
        std::vector<std::string>     names;

        IO::NAV->draw( );
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        BG_PALETTE_SUB[ GRAY_IDX ]  = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;

        if( p_updatePageIcons ) {
            for( u8 i = 0; i < 5; ++i )
                IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( i ) ].isHidden
                    = pkmn.isEgg( );
            IO::Oam->oamBuffer[ SUB_PAGE_ICON_IDX( 2 ) ].isHidden = false;
        }

        for( u8 i = 0; i < 6; ++i ) {
            auto p = SAVE::SAV->getActiveFile( ).m_pkmnTeam[ i ].m_boxdata;
            if( p.isEgg( ) )
                IO::loadEggIcon( 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ), SUB_BALL_IDX( i ),
                                 IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex );
            else if( i < SAVE::SAV->getActiveFile( ).getTeamPkmnCount( ) )
                IO::loadPKMNIcon( p.m_speciesId, 4 - 2 * i, 28 + 24 * i, SUB_BALL_IDX( i ),
                                  SUB_BALL_IDX( i ),
                                  IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].gfxIndex, true,
                                  p.getForme( ), p.isShiny( ), p.isFemale( ) );
            else
                IO::Oam->oamBuffer[ SUB_BALL_IDX( i ) ].isHidden = true;
        }

        IO::updateOAM( true );
        if( pkmn.isEgg( ) ) return res;

        /*
        for( u8 i = 0; i < 4; ++i )
            if( pkmn.m_boxdata.m_moves[ i ] < MAX_ATTACK
                && AttackList[ pkmn.m_boxdata.m_moves[ i ] ]->m_isFieldAttack )
                names.push_back( AttackList[ pkmn.m_boxdata.m_moves[ i ] ]->m_moveName );

        if( pkmn.m_boxdata.m_holdItem )
            names.push_back( "Item nehmen" );
        else
            names.push_back( "Item geben" );
        names.push_back( "Dexeintrag" );
        for( u8 i = 0; i < names.size( ); ++i ) {
            auto            sz = res.size( );
            IO::inputTarget cur( 37 + 100 * ( sz % 2 ), 48 + 41 * ( sz / 2 ),
                                 132 + 100 * ( sz % 2 ), 80 + 41 * ( sz / 2 ) );
            IO::printChoiceBox( cur.m_targetX1, cur.m_targetY1, cur.m_targetX2, cur.m_targetY2, 6,
                                COLOR_IDX, false );
            IO::regularFont->printString( names[ i ].c_str( ),
                                          ( cur.m_targetX2 + cur.m_targetX1 - 2 ) / 2,
                                          cur.m_targetY1 + 8, true, IO::font::CENTER );
            res.push_back( cur );
        }
        */
        return res;
    }

    void boxStsScreenUI::init( ) {
        // Remember: the storage sys swaps the screens.

        IO::regularFont->setColor( 0, 0 );
        {
            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        }
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ]  = RGB( 20, 20, 20 );
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;

        u16 tileCnt                                      = 0;
        IO::Oam->oamBuffer[ PKMN_SPRITE_START ].gfxIndex = tileCnt;
        tileCnt += 144;

        tileCnt
            = IO::loadSprite( PAGE_ICON_IDX, PAGE_ICON_PAL, tileCnt, 0, 0, 32, 32, memoPal,
                              memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadItemIcon( ItemList[ 0 ]->m_itemName, 2, 152, ITEM_ICON_IDX, ITEM_ICON_PAL,
                                    tileCnt );

        for( u8 i = 0; i < 4; ++i ) {
            type t  = AttackList[ 0 ]->m_moveType;
            tileCnt = IO::loadTypeIcon( t, 126, 43 + 32 * i, TYPE_IDX + i, TYPE_PAL( i ), tileCnt,
                                        true, SAVE::SAV->getActiveFile( ).m_options.m_language );
        }
        for( u8 i = 0; i < 4; ++i ) {
            tileCnt = IO::loadDamageCategoryIcon( ( move::moveHitTypes )( i % 3 ), 126, 43 + 32 * i,
                                                  ATK_DMGTYPE_IDX( i ), DMG_TYPE_PAL( i % 3 ),
                                                  tileCnt, true );
        }

        for( u8 i = PKMN_SPRITE_START; i < RIBBON_IDX; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::setDefaultConsoleTextColors( BG_PALETTE_SUB, 230 );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "PKMNInfoScreen", 420,
                             49162, true );
    }

    void boxStsScreenUI::draw( pokemon& p_pokemon, u8 p_page, bool p_newpok ) {
        // Remember: the storage sys swaps the screens.
        // Only draw on the sub screen
        hideSprites( true );

        IO::setDefaultConsoleTextColors( BG_PALETTE_SUB, 6 );
        drawPkmnInformation( p_pokemon, p_page, p_newpok, true );
        if( p_page == (u8) -1 ) p_page = 2;
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
            IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].isHidden
                = p_pokemon.isEgg( );
        IO::OamTop->oamBuffer[ PAGE_ICON_START + p_page ].isHidden
            = !p_pokemon.isEgg( );
        IO::updateOAM( false );
        IO::updateOAM( true );
    }
} // namespace STS
