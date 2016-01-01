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

namespace BOX {

    void boxUI::init( ) {
        IO::vramSetup( );
        videoSetMode( MODE_5_2D/* | DISPLAY_BG2_ACTIVE*/ | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );
        videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );

        swiWaitForVBlank( );
        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PKMNInfoScreen" );

        IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );

        consoleSelect( &IO::Top );
        consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
        consoleClear( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );
        consoleClear( );

        bgUpdate( );

        IO::drawSub( );

        IO::initOAMTable( true );
        u16 nextAvailableTileIdx = 0;

        nextAvailableTileIdx = IO::loadSprite( BACK_ID, BACK_ID, 0, nextAvailableTileIdx,
                                               SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                               BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

        IO::updateOAM( true );
        IO::initOAMTable( false );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        BG_PALETTE_SUB[ COLOR_IDX ] = GREEN;
        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
    }

#define POS_X( i ) ( 16 + 32 * ( (i) % 7 ) )
#define POS_Y( i ) ( 57 + 28 * ( (i) / 7 ) )

    std::vector<IO::inputTarget> boxUI::draw( std::pair<u16, u16> p_pokemon[ 30 ], u8 p_pos, box* p_box, u8 p_oldpos, bool p_newPok, bool p_showTeam ) {
        std::vector<IO::inputTarget> res;
        _ranges.clear( );

        u16 pidx = p_pokemon[ p_pos + 1 ].first;
        if( p_showTeam && p_pos >= 21 ) {
            pidx = FS::SAV->m_pkmnTeam[ p_pos - 21 ].m_boxdata.m_speciesId;
            if( FS::SAV->m_pkmnTeam[ p_pos - 21 ].m_boxdata.m_individualValues.m_isEgg )
                pidx = MAX_PKMN + 1;
        }

        pokemonData p;
        if( pidx != MAX_PKMN + 1 )
            getAll( pidx, p );

        //SubScreen stuff
        IO::printChoiceBox( 48, 23, 204, 48, 6, COLOR_IDX, false );
        if( pidx ) {
            char buffer[ 50 ];
            if( pidx != MAX_PKMN + 1 )
                sprintf( buffer, "%s", p.m_displayName );
            else
                sprintf( buffer, "Ei" );
            IO::printString( IO::regularFont, buffer, 126 - IO::regularFont->stringWidth( buffer ) / 2, 28, true );
        } else {
            IO::printString( IO::regularFont, "Leer",
                             126 - IO::regularFont->stringWidth( "Leer" ) / 2, 28, true );
        }

        if( p_oldpos == 42 ) {
            for( u8 cnt = 0; cnt < 28 - 7 * p_showTeam; ++cnt ) {
                IO::printChoiceBox( POS_X( cnt ), POS_Y( cnt ),
                                    POS_X( cnt ) + 28, POS_Y( cnt ) + 21, 3, ( cnt == p_pos ) ? RED_IDX : GRAY_IDX, false );
            }
        } else {
            u8 c1 = ( p_showTeam && ( p_oldpos >= 21 ) ) ? 16 : 0;
            u8 c2 = ( p_showTeam && ( p_pos >= 21 ) ) ? 16 : 0;

            IO::printChoiceBox( c1 + POS_X( p_oldpos ), c1 / 2 + POS_Y( p_oldpos ),
                                c1 + POS_X( p_oldpos ) + 28, c1 / 2 + POS_Y( p_oldpos ) + 21, 3, GRAY_IDX, false );
            IO::printChoiceBox( c2 + POS_X( p_pos ), c2 / 2 + POS_Y( p_pos ),
                                c2 + POS_X( p_pos ) + 28, c2 / 2 + POS_Y( p_pos ) + 21, 3, RED_IDX, false );
        }

        u8 oam = 3;
        u8 pal = 4;
        u16 tileCnt = 96;

        u16 lst = p_pokemon[ 1 ].first;

        if( p_newPok ) {
            for( u8 i = 1; i < 29 - 7 * p_showTeam; ++i ) {
                res.push_back( IO::inputTarget( POS_X( i - 1 ), POS_Y( i - 1 ), POS_X( i - 1 ) + 28, POS_Y( i - 1 ) + 21 ) );
                _ranges.push_back( { oam, res.back( ) } );

                if( !p_pokemon[ i ].first ) {
                    IO::Oam->oamBuffer[ oam++ ].isHidden = true;
                    continue;
                }

                if( i > 1 && lst == p_pokemon[ i ].first ) {
                    memcpy( &IO::Oam->oamBuffer[ oam ], &IO::Oam->oamBuffer[ oam - 1 ], sizeof( SpriteEntry ) );
                    IO::Oam->oamBuffer[ oam ].x = POS_X( i - 1 ) - 3;
                    IO::Oam->oamBuffer[ oam++ ].y = POS_Y( i - 1 ) - 10;
                } else {
                    lst = p_pokemon[ i ].first;
                    if( lst != MAX_PKMN + 1 )
                        tileCnt = IO::loadPKMNIcon( lst, POS_X( i - 1 ) - 3, POS_Y( i - 1 ) - 10, oam++, pal / 16, pal % 16, tileCnt );
                    else
                        tileCnt = IO::loadEggIcon( POS_X( i - 1 ) - 3, POS_Y( i - 1 ) - 10, oam++, pal / 16, pal % 16, tileCnt );
                    IO::updateOAM( true );
                    ++pal;
                }
            }

            if( p_showTeam ) {
                IO::printRectangle( 0, 140, 255, 192, true, false, WHITE_IDX );
                IO::printString( IO::regularFont, "Pokémon-Team", 2, 176, true );
                for( u8 i = 0; i < 6; ++i ) {
                    res.push_back( IO::inputTarget( 16 + POS_X( 21 + i ), 8 + POS_Y( 21 + i ),
                                                    16 + POS_X( 21 + i ) + 28, 8 + POS_Y( 21 + i ) + 21 ) );
                    _ranges.push_back( { oam, res.back( ) } );

                    IO::printChoiceBox( 16 + POS_X( 21 + i ), 8 + POS_Y( 21 + i ),
                                        16 + POS_X( 21 + i ) + 28, 8 + POS_Y( 21 + i ) + 21, 3,
                                        ( 21 + i == p_pos ) ? RED_IDX : GRAY_IDX, false );
                    if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId ) {
                        if( !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg ) {
                            tileCnt = IO::loadPKMNIcon( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId,
                                                        16 + POS_X( 21 + i ) - 3, 8 + POS_Y( 21 + i ) - 10, oam++, pal / 16, pal % 16, tileCnt );
                        } else
                            tileCnt = IO::loadEggIcon( 16 + POS_X( 21 + i ) - 3, 8 + POS_Y( 21 + i ) - 10, oam++, pal / 16, pal % 16, tileCnt );
                        ++pal;
                    } else {
                        IO::Oam->oamBuffer[ oam++ ].isHidden = true;
                    }
                }
            }
        } else {
            for( u8 i = 1; i < 29 - 7 * p_showTeam; ++i ) {
                res.push_back( IO::inputTarget( POS_X( i - 1 ), POS_Y( i - 1 ), POS_X( i - 1 ) + 28, POS_Y( i - 1 ) + 21 ) );
                _ranges.push_back( { oam++, res.back( ) } );
            }

            if( p_showTeam ) for( u8 i = 0; i < 6; ++i ) {
                res.push_back( IO::inputTarget( 16 + POS_X( 21 + i ), 8 + POS_Y( 21 + i ),
                                                16 + POS_X( 21 + i ) + 28, 8 + POS_Y( 21 + i ) + 21 ) );
                _ranges.push_back( { oam++, res.back( ) } );
            }
        }
        IO::updateOAM( true );
        return res;
    }

    void boxUI::updateAtHand( touchPosition p_touch, u8 p_oamIdx ) {
        IO::Oam->oamBuffer[ p_oamIdx ].x = p_touch.px - 16;
        IO::Oam->oamBuffer[ p_oamIdx ].y = p_touch.py - 16;
        IO::updateOAM( true );
    }

    u8 boxUI::getSprite( u8 p_oldIdx, u8 p_rangeIdx ) {
        IO::Oam->oamBuffer[ _ranges[ p_rangeIdx ].first ].priority = OBJPRIORITY_0;

        if( p_oldIdx < 21 ) {
            IO::printChoiceBox( POS_X( p_oldIdx ), POS_Y( p_oldIdx ),
                                POS_X( p_oldIdx ) + 28, POS_Y( p_oldIdx ) + 21,
                                3, GRAY_IDX, false );
        } else {
            IO::printChoiceBox( 16 + POS_X( p_oldIdx ), 8 + POS_Y( p_oldIdx ),
                                16 + POS_X( p_oldIdx ) + 28, 8 + POS_Y( p_oldIdx ) + 21,
                                3, GRAY_IDX, false );
        }
        if( p_rangeIdx < 21 ) {
            IO::printChoiceBox( POS_X( p_rangeIdx ), POS_Y( p_rangeIdx ),
                                POS_X( p_rangeIdx ) + 28, POS_Y( p_rangeIdx ) + 21,
                                3, RED_IDX, true );
        } else {
            IO::printChoiceBox( 16 + POS_X( p_rangeIdx ), 8 + POS_Y( p_rangeIdx ),
                                16 + POS_X( p_rangeIdx ) + 28, 8 + POS_Y( p_rangeIdx ) + 21,
                                3, RED_IDX, true );

            IO::printRectangle( 16 + POS_X( p_rangeIdx ), 8 + POS_Y( p_rangeIdx ),
                                16 + POS_X( p_rangeIdx ) + 28, 9 + POS_Y( p_rangeIdx ),
                                true, false, WHITE_IDX );
            IO::printRectangle( 14 + POS_X( p_rangeIdx ), 8 + POS_Y( p_rangeIdx ),
                                16 + POS_X( p_rangeIdx ), 8 + POS_Y( p_rangeIdx ) + 21,
                                true, false, WHITE_IDX );
        }

        return _ranges[ p_rangeIdx ].first;
    }

    u8 boxUI::acceptTouch( u8 p_oldIdx, u8 p_rangeIdx, bool p_allowTakePkmn ) {
        if( p_oldIdx < 21 || !p_allowTakePkmn ) {
            IO::printChoiceBox( POS_X( p_oldIdx ), POS_Y( p_oldIdx ),
                                POS_X( p_oldIdx ) + 28, POS_Y( p_oldIdx ) + 21,
                                3, GRAY_IDX, false );
        } else {
            IO::printChoiceBox( 16 + POS_X( p_oldIdx ), 8 + POS_Y( p_oldIdx ),
                                16 + POS_X( p_oldIdx ) + 28, 8 + POS_Y( p_oldIdx ) + 21,
                                3, GRAY_IDX, false );
        }

        if( p_rangeIdx < 21 || !p_allowTakePkmn ) {
            IO::printChoiceBox( POS_X( p_rangeIdx ), POS_Y( p_rangeIdx ),
                                POS_X( p_rangeIdx ) + 28, POS_Y( p_rangeIdx ) + 21,
                                3, RED_IDX, false );
        } else {
            IO::printChoiceBox( 16 + POS_X( p_rangeIdx ), 8 + POS_Y( p_rangeIdx ),
                                16 + POS_X( p_rangeIdx ) + 28, 8 + POS_Y( p_rangeIdx ) + 21,
                                3, RED_IDX, false );
        }
        return 0;
    }

    u32 boxUI::acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ) {

        IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].x = POS_X( p_startIdx ) - 3;
        IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].y = POS_Y( p_startIdx ) - 10;
        IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].priority = OBJPRIORITY_0;
        IO::updateOAM( true );

        if( p_startIdx < 21 ) {
            IO::printChoiceBox( POS_X( p_startIdx ), POS_Y( p_startIdx ),
                                POS_X( p_startIdx ) + 28, POS_Y( p_startIdx ) + 21,
                                3, RED_IDX, false );
        } else {
            IO::printChoiceBox( 16 + POS_X( p_startIdx ), 8 + POS_Y( p_startIdx ),
                                16 + POS_X( p_startIdx ) + 28, 8 + POS_Y( p_startIdx ) + 21,
                                3, RED_IDX, false );
        }

        if( u8( -1 ) == p_dropIdx )
            return 0;

        if( p_startIdx >= 21 || p_dropIdx >= 21 ) {
            if( p_startIdx >= 21 ) {
                IO::Oam->oamBuffer[ _ranges[ p_dropIdx ].first ].x = 16 + POS_X( p_startIdx ) - 3;
                IO::Oam->oamBuffer[ _ranges[ p_dropIdx ].first ].y = 8 + POS_Y( p_startIdx ) - 10;
            } else {
                IO::Oam->oamBuffer[ _ranges[ p_dropIdx ].first ].x = POS_X( p_startIdx ) - 3;
                IO::Oam->oamBuffer[ _ranges[ p_dropIdx ].first ].y = POS_Y( p_startIdx ) - 10;
            }
            if( p_dropIdx >= 21 ) {
                IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].x = 16 + POS_X( p_dropIdx ) - 3;
                IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].y = 8 + POS_Y( p_dropIdx ) - 10;
            } else {
                IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].x = POS_X( p_dropIdx ) - 3;
                IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ].y = POS_Y( p_dropIdx ) - 10;
            }

            std::swap( IO::Oam->oamBuffer[ _ranges[ p_startIdx ].first ], IO::Oam->oamBuffer[ _ranges[ p_dropIdx ].first ] );
            IO::updateOAM( true );
        }
        return 0;
    }

}