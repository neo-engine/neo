/*
    Pokémon neo
    ------------------------------

    file        : specials.cpp
    author      : Philip Wellnitz
    description : Various mini games

    Copyright (C) 2020 - 2020
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

#include <vector>
#include <nds.h>

#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "mapDrawer.h"
#include "nav.h"
#include "pokemon.h"
#include "pokemonData.h"
#include "pokemonNames.h"
#include "screenFade.h"
#include "sound.h"
#include "specials.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"

#include "noselection_96_32_1.h"
#include "noselection_96_32_2.h"
#include "noselection_96_32_4.h"
#include "x_16_16.h"

namespace SPX {
    void runInitialPkmnSelection( ) {
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 5 + 6 * ( p_pos ) )
#define SPR_BOX_PAL_SUB 5
#define SPR_BOX_SEL_PAL_SUB 6

        ANIMATE_MAP = false;
        videoSetMode( MODE_5_2D );
        bgUpdate( );
        SOUND::dimVolume( );
        IO::initVideo( );
        IO::initVideoSub( );
        FADE_TOP_DARK( );
        FADE_SUB_DARK( );

        IO::clearScreen( true, true, true );

        IO::initOAMTable( true );
        IO::initOAMTable( false );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "startersel", 480, 49152,
                             true );
        dmaFillWords( 0x1010101, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        BG_PALETTE[ 1 ] = BG_PALETTE_SUB[ 1 ];

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        BG_PALETTE[ IO::WHITE_IDX ]     = IO::WHITE;
        BG_PALETTE[ IO::BLACK_IDX ]     = IO::BLACK;
        BG_PALETTE[ IO::GRAY_IDX ]      = IO::GRAY;
        BG_PALETTE_SUB[ IO::WHITE_IDX ] = IO::WHITE;
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = IO::GRAY;
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::BLACK;

        // Sprites sub
        // poke balls
        u16                                         tileCnt = 0;
        std::vector<std::pair<IO::inputTarget, u8>> tpos
            = std::vector<std::pair<IO::inputTarget, u8>>( );
        std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>> ynpos
            = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>( );

        tileCnt = IO::loadSprite( "UI/sb2", 0, 0, tileCnt, 70, 86, 32, 32, false, false, false,
                                  OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( "UI/sb2", 1, 1, tileCnt, 112, 100, 32, 32, false, false, false,
                                  OBJPRIORITY_0, true );
        tileCnt = IO::loadSprite( "UI/sb2", 2, 2, tileCnt, 154, 86, 32, 32, false, false, false,
                                  OBJPRIORITY_0, true );

        for( u8 i = 0; i < 3; ++i ) {
            tpos.push_back( std::pair( IO::inputTarget( IO::Oam->oamBuffer[ i ].x + 16,
                                                        IO::Oam->oamBuffer[ i ].y + 16, 8 ),
                                       i ) );
        }

        tileCnt = IO::loadSprite( "UI/sh", 3, 4, tileCnt, 154, 86, 32, 32, false, false, true,
                                  OBJPRIORITY_0, true );

        // Choice boxes

        constexpr u16 y = 192 - 40;

        for( u8 i = 0; i < 1; i++ ) {
            u8 pos = 2 * i;

            tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB, tileCnt, 29,
                                      y, 16, 32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                                      noselection_96_32_1TilesLen, false, false, true,
                                      OBJPRIORITY_0, true, OBJMODE_BLENDED );
            tileCnt = IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB, tileCnt,
                                      29 + 16, y, 16, 32, noselection_96_32_2Pal,
                                      noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false,
                                      false, true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                                29 + j * 16, y, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            29 + 5 * 16, y, 16, 32, noselection_96_32_1Pal,
                            noselection_96_32_1Tiles, noselection_96_32_1TilesLen, true, true, true,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        for( u8 i = 0; i < 1; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, y,
                            16, 32, noselection_96_32_1Pal, noselection_96_32_1Tiles,
                            noselection_96_32_1TilesLen, false, false, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                                131 + j * 16, y, 16, 32, noselection_96_32_2Pal,
                                noselection_96_32_2Tiles, noselection_96_32_2TilesLen, false, false,
                                true, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            131 + 5 * 16, y, 16, 32, noselection_96_32_1Pal,
                            noselection_96_32_1Tiles, noselection_96_32_1TilesLen, true, true, true,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        IO::copySpritePal( noselection_96_32_4Pal, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );

        ynpos.push_back( std::pair(
            IO::inputTarget( IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x,
                             IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y,
                             IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 96,
                             IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 32 ),
            IO::yesNoBox::YES ) );
        ynpos.push_back( std::pair(
            IO::inputTarget( IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x,
                             IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                             IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 96,
                             IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 32 ),
            IO::yesNoBox::NO ) );

        IO::updateOAM( true );
        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );

        auto cb = IO::choiceBox( IO::choiceBox::MODE_LEFT_RIGHT );

        std::vector<pokemon> pkmn = {
            pokemon( PKMN_TREECKO, 5, 0, 0, 255 * ( ( rand( ) & 3 ) == 3 ), ( rand( ) & 7 ) == 7,
                     false, 3, 0, true ),
            pokemon( PKMN_TORCHIC, 5, 0, 0, 255 * ( ( rand( ) & 3 ) == 3 ), ( rand( ) & 7 ) == 7,
                     false, 3, 0, true ),
            pokemon( PKMN_MUDKIP, 5, 0, 0, 255 * ( ( rand( ) & 3 ) == 3 ), ( rand( ) & 7 ) == 7,
                     false, 3, 0, true ),
        };

        u8 curSel = 1;

        u8   frame = 0;
        auto tick  = [ & ]( ) {
            ++frame;

            if( frame % 8 == 2 ) {
                tileCnt = IO::loadSprite(
                    "UI/sb1", curSel, curSel, IO::Oam->oamBuffer[ curSel ].gfxIndex,
                    IO::Oam->oamBuffer[ curSel ].x, IO::Oam->oamBuffer[ curSel ].y, 32, 32, false,
                    false, false, OBJPRIORITY_0, true );
            }
            if( frame % 4 == 0 ) {
                tileCnt = IO::loadSprite(
                    "UI/sb2", curSel, curSel, IO::Oam->oamBuffer[ curSel ].gfxIndex,
                    IO::Oam->oamBuffer[ curSel ].x, IO::Oam->oamBuffer[ curSel ].y, 32, 32, false,
                    false, false, OBJPRIORITY_0, true );
            }
            if( frame % 8 == 6 ) {
                tileCnt = IO::loadSprite(
                    "UI/sb3", curSel, curSel, IO::Oam->oamBuffer[ curSel ].gfxIndex,
                    IO::Oam->oamBuffer[ curSel ].x, IO::Oam->oamBuffer[ curSel ].y, 32, 32, false,
                    false, false, OBJPRIORITY_0, true );
            }

            if( frame % 16 == 0 ) {
                IO::Oam->oamBuffer[ 3 ].y += 4;
                IO::updateOAM( true );
            }
            if( frame % 16 == 8 ) {
                IO::Oam->oamBuffer[ 3 ].y -= 4;
                IO::updateOAM( true );
            }
        };

        loop( ) {
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            u16 tc = 144 + 256;
            IO::loadSprite( "UI/mbox1", 8, 3, tc, 2, 192 - 46, 32, 64, false, false, false,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );

            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSprite( 8 + 13 - i, 3, tc, 30 + 16 * i, 192 - 46, 32, 64, 0, 0, 0, false,
                                true, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
            }

            IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
            IO::printRectangle( 0, 192 - 42, 255, 192, true, 0 );
            IO::regularFont->printStringC( GET_STRING( 438 ), 12, 192 - 40, false );

            for( u8 i = 0; i < 12; ++i ) {
                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + i ].isHidden = true;
            }
            for( u8 i = 0; i < 8; ++i ) { IO::OamTop->oamBuffer[ i ].isHidden = true; }
            IO::updateOAM( false );
            u8 res = cb.getResult(
                [ & ]( u8 ) {
                    frame = 0;
                    return tpos;
                },
                [ & ]( u8 p_selection ) {
                    tileCnt = IO::loadSprite(
                        "UI/sb2", curSel, curSel, IO::Oam->oamBuffer[ curSel ].gfxIndex,
                        IO::Oam->oamBuffer[ curSel ].x, IO::Oam->oamBuffer[ curSel ].y, 32, 32,
                        false, false, false, OBJPRIORITY_0, true );

                    curSel = p_selection;

                    IO::Oam->oamBuffer[ 3 ].isHidden = false;
                    IO::Oam->oamBuffer[ 3 ].x        = IO::Oam->oamBuffer[ p_selection ].x;
                    IO::Oam->oamBuffer[ 3 ].y        = IO::Oam->oamBuffer[ p_selection ].y - 32;
                    IO::updateOAM( true );
                },
                curSel, tick );

            // Make the player confirm the choice

            tc = 0;
            tc = IO::loadSprite( "UI/cc", 4, 1, tc, 64, 12, 64, 64, false, false, false,
                                 OBJPRIORITY_1, false );
            IO::loadSprite( 5, 1, 0, 128, 12, 64, 64, 0, 0, 0, false, true, false, OBJPRIORITY_1,
                            false );
            IO::loadSprite( 6, 1, 0, 64, 64 + 12, 64, 64, 0, 0, 0, true, false, false,
                            OBJPRIORITY_1, false );
            IO::loadSprite( 7, 1, 0, 128, 64 + 12, 64, 64, 0, 0, 0, true, true, false,
                            OBJPRIORITY_1, false );

            tc = IO::loadPKMNSprite( pkmn[ res ].getSpecies( ), 80, 16 + 12, 0, 0, tc, false,
                                     pkmn[ res ].isShiny( ), pkmn[ res ].isFemale( ), false, false,
                                     0 );

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            // mbox
            IO::loadSprite( "UI/mbox2", 8, 3, tc, 2, 192 - 46, 32, 64, false, false, false,
                            OBJPRIORITY_3, false, OBJMODE_BLENDED );

            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSprite( 8 + 13 - i, 3, tc, 30 + 16 * i, 192 - 46, 32, 64, 0, 0, 0, false,
                                true, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
            }

            SOUND::playCry( pkmn[ res ].getSpecies( ), pkmn[ res ].getForme( ),
                            pkmn[ res ].isFemale( ) );
            IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );

            char buffer[ 100 ];
            snprintf( buffer, 99, GET_STRING( 437 ),
                      getSpeciesName( pkmn[ res ].getSpecies( ), pkmn[ res ].getForme( ) ).c_str( ),
                      pkmn[ res ].m_boxdata.m_name );

            IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
            IO::regularFont->printStringC( buffer, 12, 192 - 40, false );

            IO::updateOAM( false );

            IO::yesNoBox yn;

            if( yn.getResult(
                    [ & ]( ) {
                        IO::regularFont->printString(
                            GET_STRING( 80 ),
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 48,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y + 8, true,
                            IO::font::CENTER );
                        IO::regularFont->printString(
                            GET_STRING( 81 ),
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 48,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y + 8, true,
                            IO::font::CENTER );

                        return ynpos;
                    },
                    [ & ]( IO::yesNoBox::selection p_selection ) {
                        for( u8 i = 0; i < 2; i++ ) {
                            for( u8 j = 0; j < 6; j++ ) {
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( i ) + j ].isHidden
                                    = false;
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( i ) + j ].palette
                                    = ( ( i & 1 ) == ( p_selection == IO::yesNoBox::NO ) )
                                          ? SPR_BOX_SEL_PAL_SUB
                                          : SPR_BOX_PAL_SUB;
                            }
                        }
                        IO::updateOAM( true );
                    },
                    IO::yesNoBox::NO, tick )
                == IO::yesNoBox::YES ) {

                SAVE::SAV.getActiveFile( ).registerCaughtPkmn( pkmn[ res ].getSpecies( ) );
                SAVE::SAV.getActiveFile( ).setTeamPkmn( 0, &pkmn[ res ] );
                SAVE::SAV.getActiveFile( ).setVar( SAVE::V_INITIAL_PKMN_CHOICE, res + 1 );
                break;
            }
        }

        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        bgUpdate( );
        IO::initVideoSub( );
        ANIMATE_MAP = true;
        SOUND::restoreVolume( );
        NAV::init( );
        MAP::curMap->draw( );

#undef SPR_CHOICE_START_OAM_SUB
#undef SPR_BOX_PAL_SUB
#undef SPR_BOX_SEL_PAL_SUB
    }

    void runSlotMachine( ) {
        // TODO
    }

    void runPokeBlockMaker( ) {
        // TODO
    }

    void drawBadges( u8 p_page ) {
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 20 );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::GRAY_IDX, 1 );
        IO::regularFont->setColor( 0, 2 );
        u16 tileCnt = 0;
        // x
        tileCnt = IO::loadSprite( 9, 15, tileCnt, 236, 172, 16, 16, x_16_16Pal, x_16_16Tiles,
                                  x_16_16TilesLen, false, false, false, OBJPRIORITY_1, true,
                                  OBJMODE_NORMAL );

        for( u8 i = 0; i < 8; ++i ) { IO::Oam->oamBuffer[ i ].isHidden = true; }

        switch( p_page ) {
        case 0: { // Hoenn badges
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "bc1", 480, 49152,
                                 true );
            BG_PALETTE_SUB[ IO::GRAY_IDX ] = RGB15( 24, 24, 24 );
            IO::regularFont->printStringC( GET_STRING( 434 ), 2, 0, true );

            constexpr u16 spos[ 9 ][ 2 ] = { { 8, 19 }, { 67, 19 }, { 128, 19 }, { 182, 19 },
                                             { 9, 89 }, { 68, 90 }, { 128, 90 }, { 182, 90 } };

            for( u8 i = 0; i < 8; ++i ) {
                if( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << i ) ) {
                    tileCnt = IO::loadSprite( ( "ba/b" + std::to_string( i + 1 ) ).c_str( ), i, i,
                                              tileCnt, spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64,
                                              false, false, false, OBJPRIORITY_0, true );
                }
            }

            break;
        }
        case 1: { // battle frontier
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "bc2", 480, 49152,
                                 true );
            BG_PALETTE_SUB[ IO::GRAY_IDX ] = RGB15( 24, 24, 24 );
            IO::regularFont->printStringC( GET_STRING( 435 ), 2, 0, true );

            constexpr u16 spos[ 7 ][ 2 ] = { { 6, 18 },  { 66, 18 }, { 126, 18 }, { 186, 18 },
                                             { 36, 88 }, { 96, 88 }, { 156, 88 } };

            for( u8 i = 0; i < 7; ++i ) {
                if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << ( 7 + i ) ) ) {
                    tileCnt = IO::loadSprite( ( "ba/s" + std::to_string( i + 1 ) + "2" ).c_str( ),
                                              i, i, tileCnt, spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64,
                                              false, false, false, OBJPRIORITY_0, true );

                } else if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << i ) ) {
                    tileCnt = IO::loadSprite( ( "ba/s" + std::to_string( i + 1 ) + "1" ).c_str( ),
                                              i, i, tileCnt, spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64,
                                              false, false, false, OBJPRIORITY_0, true );
                }
            }

            break;
        }
        default:
            return;
        }

        IO::updateOAM( true );
    }

    void runIDViewer( ) {
        IO::vramSetup( );
        IO::clearScreen( true, true, true );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( IO::GRAY_IDX, 2 );

        SAVE::SAV.getActiveFile( ).drawTrainersCard( false );

        u8 currentPage = 0;

        drawBadges( currentPage );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown = COOLDOWN_COUNT;
                return;
            }

            if( GET_KEY_COOLDOWN( KEY_RIGHT ) || GET_KEY_COOLDOWN( KEY_R ) ) { // next badge case
                if( SAVE::SAV.getActiveFile( ).hasBadgeCase( currentPage + 1 ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    drawBadges( ++currentPage );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( currentPage
                       && ( GET_KEY_COOLDOWN( KEY_LEFT )
                            || GET_KEY_COOLDOWN( KEY_L ) ) ) { // next badge case
                if( SAVE::SAV.getActiveFile( ).hasBadgeCase( currentPage - 1 ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    drawBadges( --currentPage );
                }
                cooldown = COOLDOWN_COUNT;
            }
        }
    }

} // namespace SPX
