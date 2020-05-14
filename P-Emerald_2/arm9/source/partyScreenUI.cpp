/*
Pokémon neo
------------------------------

file        : partyScreenUI.cpp
author      : Philip Wellnitz
description : Draw UI for the pkmn party screen

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

#include "partyScreenUI.h";
#include "defines.h"
#include "saveGame.h"
#include "screenFade.h"
#include "uio.h"

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
#include "party_mark1.h"
#include "party_mark2.h"
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
    partyScreenUI::partyScreenUI( pokemon p_team[ 6 ], u8 p_teamLength ) {
        _team       = p_team;
        _teamLength = p_teamLength;
    }

    u16 partyScreenUI::initTopScreen( bool p_bottom ) {
        if( p_bottom ) {
            IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
            bgSetPriority( IO::bg3sub, 3 );
            bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        } else {
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
            bgSetPriority( IO::bg3, 3 );
            bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
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

#define SPR_HP_BAR_OAM( p_pos ) ( p_pos )
#define SPR_PKMN_BG_OAM( p_pos ) ( 6 + 2 * ( p_pos ) )
#define SPR_ITEM_ICON_OAM( p_pos ) ( 18 + ( p_pos ) )
#define SPR_PKMN_ICON_OAM( p_pos ) ( 24 + ( p_pos ) )
#define SPR_STATUS_ICON_OAM( p_pos ) ( 30 + ( p_pos ) )
#define SPR_SHINY_ICON_OAM( p_pos ) ( 36 + ( p_pos ) )
#define SPR_PKMN_BG_PAL 0
#define SPR_PKMN_ICON_PAL( p_pos ) ( 4 + ( p_pos ) )
#define SPR_ITEM_ICON_PAL 10
#define SPR_STATUS_ICON_PAL 11
#define SPR_HP_BAR_PAL 12
#define SPR_SHINY_ICON_PAL 13

            // background "box"
            tileCnt
                = IO::loadSprite( SPR_PKMN_BG_OAM( pos ), SPR_PKMN_BG_PAL, tileCnt, 4, 4 + 61 * i,
                                  64, 64, party_blank1Pal, party_blank1Tiles, party_blank1TilesLen,
                                  false, false, false, OBJPRIORITY_3, p_bottom, OBJMODE_BLENDED );
            tileCnt = IO::loadSprite( SPR_PKMN_BG_OAM( pos ) + 1, SPR_PKMN_BG_PAL, tileCnt, 68,
                                      4 + 61 * i, 64, 64, party_blank1Pal, party_blank2Tiles,
                                      party_blank2TilesLen, false, false, false, OBJPRIORITY_3,
                                      p_bottom, OBJMODE_BLENDED );

            // PKMN icon
            tileCnt = IO::loadEggIcon( 3, 2 + 61 * i, SPR_PKMN_ICON_OAM( pos ),
                                       SPR_PKMN_ICON_PAL( pos ), tileCnt, p_bottom );
            IO::OamTop->oamBuffer[ SPR_PKMN_ICON_OAM( pos ) ].isHidden = true;

            if( i ) {
                // Item icon
                IO::loadSprite( SPR_ITEM_ICON_OAM( pos ), SPR_ITEM_ICON_PAL,
                                oam[ SPR_ITEM_ICON_OAM( 0 ) ].gfxIndex, 3 + 32 - 9,
                                2 + 32 - 8 + 61 * i, 8, 8, itemiconPal, itemiconTiles,
                                itemiconTilesLen, false, false, true, OBJPRIORITY_0, p_bottom,
                                OBJMODE_NORMAL );

                // HP bar
                IO::loadSprite( SPR_HP_BAR_OAM( pos ), SPR_HP_BAR_PAL,
                                oam[ SPR_HP_BAR_OAM( 0 ) ].gfxIndex, 120 - 48, 34 + 61 * i, 64, 32,
                                hpbarPal, hpbarTiles, hpbarTilesLen, false, false, true,
                                OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            } else {
                // Item icon
                tileCnt = IO::loadSprite( SPR_ITEM_ICON_OAM( pos ), SPR_ITEM_ICON_PAL, tileCnt,
                                          3 + 32 - 9, 2 + 32 - 8 + 61 * i, 8, 8, itemiconPal,
                                          itemiconTiles, itemiconTilesLen, false, false, true,
                                          OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

                // HP bar
                tileCnt
                    = IO::loadSprite( SPR_HP_BAR_OAM( pos ), SPR_HP_BAR_PAL, tileCnt, 120 - 48,
                                      34 + 61 * i, 64, 32, hpbarPal, hpbarTiles, hpbarTilesLen,
                                      false, false, true, OBJPRIORITY_3, p_bottom, OBJMODE_NORMAL );
            }

            // Status icon
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM( pos ), SPR_STATUS_ICON_PAL, tileCnt,
                                      130 - 62 - 22, 33 + 61 * i, 8, 8, status_parPal,
                                      status_parTiles, status_parTilesLen / 2, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            // Shiny icon
            tileCnt = IO::loadSprite( SPR_SHINY_ICON_OAM( pos ), SPR_SHINY_ICON_PAL, tileCnt,
                                      130 - 62 - 32, 33 + 61 * i, 8, 8, status_shinyPal,
                                      status_shinyTiles, status_shinyTilesLen, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
        }

        // right half
        for( size_t i = 0; i < 3; i++ ) {
            u8 pos = 2 * i + 1;

            // background "box"
            tileCnt = IO::loadSprite( SPR_PKMN_BG_OAM( pos ), SPR_PKMN_BG_PAL, tileCnt, 131,
                                      12 + 61 * i, 64, 64, party_blank1Pal, party_blank1Tiles,
                                      party_blank1TilesLen, false, false, false, OBJPRIORITY_3,
                                      p_bottom, OBJMODE_BLENDED );
            tileCnt = IO::loadSprite( SPR_PKMN_BG_OAM( pos ) + 1, SPR_PKMN_BG_PAL, tileCnt, 195,
                                      12 + 61 * i, 64, 64, party_blank1Pal, party_blank2Tiles,
                                      party_blank2TilesLen, false, false, false, OBJPRIORITY_3,
                                      p_bottom, OBJMODE_BLENDED );

            // PKMN icon
            tileCnt = IO::loadEggIcon( 130, 10 + 61 * i, SPR_PKMN_ICON_OAM( pos ),
                                       SPR_PKMN_ICON_PAL( pos ), tileCnt, p_bottom );
            IO::OamTop->oamBuffer[ SPR_PKMN_ICON_OAM( pos ) ].isHidden = true;

            // Item icon
            IO::loadSprite(
                SPR_ITEM_ICON_OAM( pos ), SPR_ITEM_ICON_PAL, oam[ SPR_ITEM_ICON_OAM( 0 ) ].gfxIndex,
                130 + 32 - 9, 10 + 32 - 8 + 61 * i, 8, 8, itemiconPal, itemiconTiles,
                itemiconTilesLen, false, false, true, OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

            // Status icon
            tileCnt = IO::loadSprite( SPR_STATUS_ICON_OAM( pos ), SPR_STATUS_ICON_PAL, tileCnt,
                                      257 - 62 - 22, 41 + 61 * i, 8, 8, status_parPal,
                                      status_parTiles, status_parTilesLen / 2, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );
            // Shiny icon
            tileCnt = IO::loadSprite( SPR_SHINY_ICON_OAM( pos ), SPR_SHINY_ICON_PAL, tileCnt,
                                      257 - 62 - 32, 41 + 61 * i, 8, 8, status_shinyPal,
                                      status_shinyTiles, status_shinyTilesLen, false, false, true,
                                      OBJPRIORITY_0, p_bottom, OBJMODE_NORMAL );

            // HP bar
            IO::loadSprite( SPR_HP_BAR_OAM( pos ), SPR_HP_BAR_PAL,
                            oam[ SPR_HP_BAR_OAM( 0 ) ].gfxIndex, 247 - 48, 42 + 61 * i, 64, 32,
                            hpbarPal, hpbarTiles, hpbarTilesLen, false, false, true, OBJPRIORITY_3,
                            p_bottom, OBJMODE_NORMAL );
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

    u16 partyScreenUI::initBottomScreen( bool p_bottom ) {
        if( p_bottom ) {
            IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
            bgSetPriority( IO::bg3sub, 3 );
            bgSetScale( IO::bg3sub, 1 << 7, 1 << 7 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3sub ), 256 * 256 );
        } else {
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
            bgSetPriority( IO::bg3, 3 );
            bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
            dmaCopy( partybgBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
        }
        u16* pal = BG_PAL( p_bottom );

        dmaCopy( partybgPal, pal, 3 * 2 );
        IO::initOAMTable( p_bottom );

        // TODO
        return 0;
    }

    void partyScreenUI::drawPartyPkmn( u8 p_pos, bool p_selected, bool p_redraw, bool p_bottom ) {
        SpriteEntry* oam      = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        u16          anchor_x = oam[ 6 + 2 * p_pos ].x;
        u16          anchor_y = oam[ 6 + 2 * p_pos ].y;

        if( p_redraw ) {
            IO::printRectangle( anchor_x, anchor_y, anchor_x + 124, anchor_y + 61, p_bottom, 0 );
            oam[ p_pos ].isHidden      = true; // hp bar
            oam[ 18 + p_pos ].isHidden = true; // item
            oam[ 24 + p_pos ].isHidden = true; // status
            oam[ 30 + p_pos ].isHidden = true; // pkmn
            oam[ 36 + p_pos ].isHidden = true; // shiny
        }

        if( p_pos >= _teamLength ) {
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
        if( _team[ p_pos ].m_stats.m_acHP ) {
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

        if( _team[ p_pos ].isEgg( ) ) {
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
                IO::regularFont->printString( _team[ p_pos ].m_boxdata.m_name, anchor_x + 30,
                                              anchor_y + 12, false );

                if( _team[ p_pos ].m_boxdata.m_isFemale ) {
                    IO::regularFont->setColor( RED_IDX, 1 );
                    IO::regularFont->setColor( RED2_IDX, 2 );
                    IO::regularFont->printString( "}", anchor_x + 109, anchor_y + 12, false );
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                } else if( !_team[ p_pos ].m_boxdata.m_isGenderless ) {
                    IO::regularFont->setColor( BLUE_IDX, 1 );
                    IO::regularFont->setColor( BLUE2_IDX, 2 );
                    IO::regularFont->printString( "{", anchor_x + 109, anchor_y + 12, false );
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }

                // HP
                u8 barWidth = 45 * _team[ p_pos ].m_stats.m_acHP / _team[ p_pos ].m_stats.m_maxHP;
                if( _team[ p_pos ].m_stats.m_acHP * 2 >= _team[ p_pos ].m_stats.m_maxHP ) {
                    IO::smallFont->setColor( 240, 2 );
                    IO::smallFont->setColor( 241, 1 );
                    IO::smallFont->setColor( 242, 3 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 31, anchor_x + 69 + barWidth,
                                        anchor_y + 32, false, 241 );
                    IO::printRectangle( anchor_x + 69, anchor_y + 33, anchor_x + 69 + barWidth,
                                        anchor_y + 33, false, 242 );
                } else if( _team[ p_pos ].m_stats.m_acHP * 4 >= _team[ p_pos ].m_stats.m_maxHP ) {
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
                    if( _team[ p_pos ].m_stats.m_acHP ) {
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
                snprintf( buffer, 8, "%d", _team[ p_pos ].m_level );

                IO::smallFont->printString( buffer, anchor_x + 24, anchor_y + 32, false );

                snprintf( buffer, 8, "%3d", _team[ p_pos ].m_stats.m_acHP );
                IO::smallFont->printString( buffer, anchor_x + 116 - 32 - 24, anchor_y + 32,
                                            false );
                snprintf( buffer, 8, "/%d", _team[ p_pos ].m_stats.m_maxHP );
                IO::smallFont->printString( buffer, anchor_x + 116 - 32, anchor_y + 32, false );

                // update sprites
                oam[ p_pos ].isHidden      = false;
                oam[ 18 + p_pos ].isHidden = !_team[ p_pos ].m_boxdata.getItem( ); // item

                oam[ 36 + p_pos ].isHidden = !_team[ p_pos ].isShiny( ); // shiny status

                // other status conditions
                if( !_team[ p_pos ].m_stats.m_acHP ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_fntPal, status_fntTiles,
                                    status_fntTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isParalyzed ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_parPal, status_parTiles,
                                    status_parTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isAsleep ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_slpPal, status_slpTiles,
                                    status_slpTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isBadlyPoisoned ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_txcPal, status_txcTiles,
                                    status_txcTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isBurned ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_brnPal, status_brnTiles,
                                    status_brnTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isFrozen ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_frzPal, status_frzTiles,
                                    status_frzTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                } else if( _team[ p_pos ].m_status.m_isPoisoned ) {
                    IO::loadSprite( 24 + p_pos, 11, oam[ 24 + p_pos ].gfxIndex, oam[ 24 + p_pos ].x,
                                    oam[ 24 + p_pos ].y, 8, 8, status_psnPal, status_psnTiles,
                                    status_psnTilesLen / 2, false, false, false, OBJPRIORITY_0,
                                    p_bottom, OBJMODE_NORMAL );
                }
                IO::updateOAM( p_bottom ); // Shipout fast stuff first
                bgUpdate( );
                IO::loadPKMNIcon( _team[ p_pos ].m_boxdata.m_speciesId, oam[ 30 + p_pos ].x,
                                  oam[ 30 + p_pos ].y, 30 + p_pos, 4 + p_pos,
                                  oam[ 30 + p_pos ].gfxIndex, p_bottom );
            }
        }

        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::animatePartyPkmn( u8 p_frame, bool p_bottom ) {
        if( ( p_frame & 3 ) != 3 ) { return; } // Only do something every fourth frame

        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        if( ( p_frame & 7 ) != 7 ) {
            oam[ 30 + _selectedIdx ].y
                = partyTopScreenPkmnIconPosY( _selectedIdx ) + ( ( p_frame >> 3 ) & 1 ) * 4 - 2;
        } else {
            for( u8 i = 0; i < 6; i++ ) {
                oam[ 30 + i ].y
                    = partyTopScreenPkmnIconPosY( i ) + ( ( p_frame >> 3 ) & 1 ) * 4 - 2;
            }
        }
        IO::updateOAM( p_bottom );
    }

    void partyScreenUI::init( u8 p_initialSelection ) {
        _selectedIdx = p_initialSelection;
        IO::vramSetup( );
        initTopScreen( );
        initBottomScreen( );

        for( u8 i = 0; i < 6; i++ ) { drawPartyPkmn( i, i == _selectedIdx ); }
        bgUpdate( );
    }

    void partyScreenUI::animate( u8 p_frame ) {
        animatePartyPkmn( p_frame );
        IO::animateBG( p_frame, IO::bg3 );
        IO::animateBG( p_frame, IO::bg3sub );
        bgUpdate( );
    }

    void partyScreenUI::select( u8 p_selectedIdx ) {
        drawPartyPkmn( _selectedIdx, false, false );
        _selectedIdx = p_selectedIdx;
        drawPartyPkmn( _selectedIdx, true, false );
    }

    void partyScreenUI::mark( u8 p_markIdx, u8 p_color ) {
        // TODO
    }

    void partyScreenUI::unmark( u8 p_markIdx ) {
        // TODO
    }

    void partyScreenUI::swap( u8 p_idx1, u8 p_idx2 ) {
        IO::fadeScreen( IO::CLEAR_DARK_FAST );
        drawPartyPkmn( p_idx1, p_idx1 == _selectedIdx );
        drawPartyPkmn( p_idx2, p_idx2 == _selectedIdx );
        IO::fadeScreen( IO::UNFADE_FAST );
    }
} // namespace STS