/*
    Pokémon neo
    ------------------------------

    file        : specials.cpp
    author      : Philip Wellnitz
    description : Various mini games

    Copyright (C) 2021 - 2023
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

#include "defines.h"
#include "fs/fs.h"
#include "gen/pokemonNames.h"
#include "gen/sprites.raw.h"
#include "io/choiceBox.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "map/mapDrawer.h"
#include "pokemon.h"
#include "pokemonData.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace SPX {
    void runCatchingTutorial( ) {
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

        bool shiny = rand( ) & 1;
        SAVE::SAV.getActiveFile( ).setFlag( 27, shiny );

        pokemon ralts = pokemon( PKMN_RALTS, 5, 0, 0, shiny * 2 );
        ralts.IVset( 0, 31 );
        ralts.IVset( 2, 31 );
        ralts.EVset( 0, 252 );
        ralts.EVset( 2, 252 );
        ralts.m_boxdata.m_isFemale  = false;
        ralts.m_boxdata.m_shinyType = 0;

        u8 platform = 1, plat2 = 1;
        u8 battleBack = 1;

        pokemon zigzagoon = pokemon( PKMN_ZIGZAGOON, 5, 0, 0, 0 );
        zigzagoon.IVset( 1, 0 );
        zigzagoon.EVset( 1, 0 );

        BATTLE::battlePolicy policy = BATTLE::battlePolicy( BATTLE::DEFAULT_WILD_POLICY );
        policy.m_mode               = BATTLE::BM_MOCK;

        BATTLE::battle( &zigzagoon, 1, ralts, platform, plat2, battleBack, policy ).start( );
        SOUND::restartBGM( );

        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        bgUpdate( );
        IO::initVideoSub( );
        ANIMATE_MAP = true;
        SOUND::restoreVolume( );
        IO::init( );
        MAP::curMap->draw( );
    }

    void runInitialPkmnSelection( ) {
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 5 + 6 * ( p_pos ) )
#define SPR_BOX_PAL_SUB                   5
#define SPR_BOX_SEL_PAL_SUB               6

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

        tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL2_START, 0, 0, tileCnt, 70, 86, 32, 32,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL2_START, 1, 1, tileCnt, 112, 100, 32, 32,
                                  false, false, false, OBJPRIORITY_0, true );
        tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL2_START, 2, 2, tileCnt, 154, 86, 32, 32,
                                  false, false, false, OBJPRIORITY_0, true );

        for( u8 i = 0; i < 3; ++i ) {
            tpos.push_back( std::pair( IO::inputTarget( IO::Oam->oamBuffer[ i ].x + 16,
                                                        IO::Oam->oamBuffer[ i ].y + 16, 8 ),
                                       i ) );
        }

        tileCnt = IO::loadUIIcon( IO::ICON::STARTERHAND_START, 3, 4, tileCnt, 154, 86, 32, 32,
                                  false, false, true, OBJPRIORITY_0, true );

        // Choice boxes

        constexpr u16 y = 192 - 40;

        for( u8 i = 0; i < 1; i++ ) {
            u8 pos = 2 * i;

            tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_1_START,
                                      SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB, tileCnt, 29,
                                      y, 16, 32, false, false, true, OBJPRIORITY_0, true,
                                      OBJMODE_BLENDED );
            tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_2_START,
                                      SPR_CHOICE_START_OAM_SUB( pos ) + 1, SPR_BOX_PAL_SUB, tileCnt,
                                      29 + 16, y, 16, 32, false, false, true, OBJPRIORITY_3, true,
                                      OBJMODE_BLENDED );
            for( u8 j = 2; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                                29 + j * 16, y, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            29 + 5 * 16, y, 16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
        }
        for( u8 i = 0; i < 1; i++ ) {
            u8 pos = 2 * i + 1;
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ), SPR_BOX_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex, 131, y,
                            16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
            for( u8 j = 1; j < 5; j++ ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + j, SPR_BOX_PAL_SUB,
                                IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) + 1 ].gfxIndex,
                                131 + j * 16, y, 16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3,
                                true, OBJMODE_BLENDED );
            }
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( pos ) + 5, SPR_BOX_PAL_SUB,
                            IO::Oam->oamBuffer[ SPR_CHOICE_START_OAM_SUB( 0 ) ].gfxIndex,
                            131 + 5 * 16, y, 16, 32, 0, 0, 0, true, true, true, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
        }
        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );

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
                tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL1_START, curSel, curSel,
                                           IO::Oam->oamBuffer[ curSel ].gfxIndex,
                                           IO::Oam->oamBuffer[ curSel ].x,
                                           IO::Oam->oamBuffer[ curSel ].y, 32, 32, false, false,
                                           false, OBJPRIORITY_0, true );
            }
            if( frame % 4 == 0 ) {
                tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL2_START, curSel, curSel,
                                           IO::Oam->oamBuffer[ curSel ].gfxIndex,
                                           IO::Oam->oamBuffer[ curSel ].x,
                                           IO::Oam->oamBuffer[ curSel ].y, 32, 32, false, false,
                                           false, OBJPRIORITY_0, true );
            }
            if( frame % 8 == 6 ) {
                tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL3_START, curSel, curSel,
                                           IO::Oam->oamBuffer[ curSel ].gfxIndex,
                                           IO::Oam->oamBuffer[ curSel ].x,
                                           IO::Oam->oamBuffer[ curSel ].y, 32, 32, false, false,
                                           false, OBJPRIORITY_0, true );
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
            IO::loadUIIcon( IO::ICON::MBOX1_START, 8, 3, tc, 2, 192 - 46, 32, 64, false, false,
                            false, OBJPRIORITY_3, false, OBJMODE_BLENDED );

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
            for( u8 i = 0; i < 8; ++i ) {
                IO::OamTop->oamBuffer[ i ].isHidden      = true;
                IO::OamTop->oamBuffer[ i ].isRotateScale = false;
            }
            IO::updateOAM( false );
            u8 res = cb.getResult(
                [ & ]( u8 ) {
                    frame = 0;
                    return tpos;
                },
                [ & ]( u8 p_selection ) {
                    tileCnt = IO::loadUIIcon( IO::ICON::STARTERBALL2_START, curSel, curSel,
                                              IO::Oam->oamBuffer[ curSel ].gfxIndex,
                                              IO::Oam->oamBuffer[ curSel ].x,
                                              IO::Oam->oamBuffer[ curSel ].y, 32, 32, false, false,
                                              false, OBJPRIORITY_0, true );

                    curSel = p_selection;

                    IO::Oam->oamBuffer[ 3 ].isHidden = false;
                    IO::Oam->oamBuffer[ 3 ].x        = IO::Oam->oamBuffer[ p_selection ].x;
                    IO::Oam->oamBuffer[ 3 ].y        = IO::Oam->oamBuffer[ p_selection ].y - 32;
                    IO::updateOAM( true );
                },
                curSel, tick );

            // Make the player confirm the choice

            tc = 0;

            u8 SPR_CIRC_OAM = 4;
            u8 SPR_CIRC_PAL = 1;
            u8 SPR_CIRC_GFX = tc;

            tc = IO::loadUIIcon( IO::ICON::BIGCIRC1_START, SPR_CIRC_OAM, SPR_CIRC_PAL, SPR_CIRC_GFX,
                                 66, 14, 32, 32, false, false, false, OBJPRIORITY_1, false );
            IO::loadSprite( SPR_CIRC_OAM + 1, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 14, 32, 32, 0, 0, 0,
                            false, true, false, OBJPRIORITY_1, false );
            IO::loadSprite( SPR_CIRC_OAM + 2, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 74, 32, 32, 0, 0, 0,
                            true, false, false, OBJPRIORITY_1, false );
            IO::loadSprite( SPR_CIRC_OAM + 3, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 74, 32, 32, 0, 0, 0,
                            true, true, false, OBJPRIORITY_1, false );

            for( u8 i = 0; i < 4; ++i ) {
                IO ::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = true;
                IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble   = true;
                IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].rotationIndex  = i;

                if( i & 1 ) {
                    IO::OamTop->matrixBuffer[ i ].hdx = -( 1LL << 7 );
                } else {
                    IO::OamTop->matrixBuffer[ i ].hdx = ( 1LL << 7 );
                }
                IO::OamTop->matrixBuffer[ i ].hdy = ( 0LL << 8 );
                IO::OamTop->matrixBuffer[ i ].vdx = ( 0LL << 8 );
                if( i > 1 ) {
                    IO::OamTop->matrixBuffer[ i ].vdy = -( 1LL << 7 );
                } else {
                    IO::OamTop->matrixBuffer[ i ].vdy = ( 1LL << 7 );
                }
            }

            tc = IO::loadPKMNSprite( pkmn[ res ].getSpriteInfo( ), 80, 16 + 12, 0, 0, tc, false );

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );
            // mbox
            IO::loadUIIcon( IO::ICON::MBOX2_START, 8, 3, tc, 2, 192 - 46, 32, 64, false, false,
                            false, OBJPRIORITY_3, false, OBJMODE_BLENDED );

            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSprite( 8 + 13 - i, 3, tc, 30 + 16 * i, 192 - 46, 32, 64, 0, 0, 0, false,
                                true, false, OBJPRIORITY_3, false, OBJMODE_BLENDED );
            }

            SOUND::playCry( pkmn[ res ].getSpecies( ), pkmn[ res ].getForme( ),
                            pkmn[ res ].isFemale( ) );
            IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );

            char buffer[ 100 ];
            snprintf(
                buffer, 99, GET_STRING( 437 ),
                FS::getSpeciesName( pkmn[ res ].getSpecies( ), pkmn[ res ].getForme( ) ).c_str( ),
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
        IO::init( );
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
        tileCnt = IO::loadUIIcon( IO::ICON::X_16_16_START, 9, 15, tileCnt, 236, 172, 16, 16, false,
                                  false, false, OBJPRIORITY_1, true, OBJMODE_NORMAL );

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
                    tileCnt = IO::loadUIIcon( IO::BADGE_ICON_START[ i ], i, i, tileCnt,
                                              spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64, false, false,
                                              false, OBJPRIORITY_0, true );
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
                    tileCnt = IO::loadUIIcon( IO::GOLD_SYMBOL_ICON_START[ i ], i, i, tileCnt,
                                              spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64, false, false,
                                              false, OBJPRIORITY_0, true );

                } else if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << i ) ) {
                    tileCnt = IO::loadUIIcon( IO::SILVER_SYMBOL_ICON_START[ i ], i, i, tileCnt,
                                              spos[ i ][ 0 ], spos[ i ][ 1 ], 64, 64, false, false,
                                              false, OBJPRIORITY_0, true );
                }
            }

            break;
        }
        default: return;
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

    constexpr u8 RIBBON_HOENN_CHAMP = 20;
    void         runHallOfFame( ) {
        char buffer[ 200 ] = { 0 };
        // fade screen
        // set current player position to position home
        ANIMATE_MAP = false;

        SAVE::SAV.getActiveFile( ).m_currentMap         = 20;
        SAVE::SAV.getActiveFile( ).m_player.m_direction = MAP::DOWN;

        if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
            // TODO: move to FSINFO
            SAVE::SAV.getActiveFile( ).m_player = MAP::mapPlayer(
                { 0x2b, 0x89, 3 }, u16( 10 * SAVE::SAV.getActiveFile( ).m_appearance ),
                MAP::moveMode::WALK );
        } else {
            // TODO: move to FSINFO
            SAVE::SAV.getActiveFile( ).m_player = MAP::mapPlayer(
                { 0x31, 0xa9, 3 }, u16( 10 * SAVE::SAV.getActiveFile( ).m_appearance ),
                MAP::moveMode::WALK );
        }

        // heal party pkmn
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
            auto tmp = SAVE::SAV.getActiveFile( ).getTeamPkmn( i );
            if( tmp ) {
                tmp->heal( );
                // award champion ribbon
                tmp->m_boxdata.awardRibbon( RIBBON_HOENN_CHAMP );
            }
        }

        // add star to trainers card (if it doesn't exist already)
        SAVE::SAV.getActiveFile( ).registerAchievement(
            SAVE::saveGame::playerInfo::ACHIEVEMENT_HALL_OF_FAME );

        // add achievement
        SAVE::SAV.getActiveFile( ).m_lastAchievementDate  = SAVE::CURRENT_DATE;
        SAVE::SAV.getActiveFile( ).m_lastAchievementEvent = 9; // hall of fame
                                                               // message
        SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_GAME_CLEAR, 1 );

        // save game
        u16 lst = -1;
        if( FS::writeSave( ARGV[ 0 ], [ & ]( u16 p_perc, u16 p_total ) {
                u16 stat = p_perc * 18 / p_total;
                if( stat != lst ) {
                    lst = stat;
                    IO::printMessage( 0, MSG_INFO_NOCLOSE );
                    std::string buf2 = "";
                    for( u8 i = 0; i < stat; ++i ) {
                        buf2 += "\x03";
                        if( i % 3 == 2 ) { buf2 += " "; }
                    }
                    for( u8 i = stat; i < 18; ++i ) {
                        buf2 += "\x04";
                        if( i % 3 == 2 ) { buf2 += " "; }
                    }
                    snprintf( buffer, 99, GET_STRING( 93 ), buf2.c_str( ) );
                    IO::printMessage( buffer, MSG_INFO_NOCLOSE, true );
                }
            } ) ) {
            IO::printMessage( 0, MSG_INFO_NOCLOSE );
            SOUND::playSoundEffect( SFX_SAVE );
            IO::printMessage( GET_STRING( 94 ), MSG_INFO );
        } else {
            IO::printMessage( 0, MSG_INFO_NOCLOSE );
            IO::printMessage( GET_STRING( 95 ), MSG_INFO );
        }

        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        videoSetMode( MODE_5_2D );
        IO::clearScreen( true, true, true );
        IO::initVideo( true );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        bgUpdate( );

        // show hall of fame screen
        // show credits
        // reset game
        RESET_GAME = true;
    }
} // namespace SPX

namespace MAP {
    void mapDrawer::earthquake( ) {
        SOUND::playSoundEffect( SFX_HM_STRENGTH );
        for( u8 k = 0; k < 2; ++k ) {
            swiWaitForVBlank( );
            moveCamera( RIGHT, false, false );
            moveCamera( RIGHT, false, false );
            swiWaitForVBlank( );
            moveCamera( RIGHT, false, false );
            moveCamera( RIGHT, false, false );
            swiWaitForVBlank( );
            moveCamera( MAP::LEFT, false, false );
            moveCamera( MAP::LEFT, false, false );
        }
        for( u8 k = 0; k < 4; ++k ) {
            swiWaitForVBlank( );
            moveCamera( RIGHT, false, false );
            moveCamera( RIGHT, false, false );
            swiWaitForVBlank( );
            moveCamera( MAP::LEFT, false, false );
            moveCamera( MAP::LEFT, false, false );
            swiWaitForVBlank( );
            moveCamera( MAP::LEFT, false, false );
            moveCamera( MAP::LEFT, false, false );
        }
        for( u8 k = 0; k < 2; ++k ) {
            swiWaitForVBlank( );
            moveCamera( RIGHT, false, false );
            moveCamera( RIGHT, false, false );
            swiWaitForVBlank( );
            moveCamera( RIGHT, false, false );
            moveCamera( RIGHT, false, false );
            swiWaitForVBlank( );
            moveCamera( MAP::LEFT, false, false );
            moveCamera( MAP::LEFT, false, false );
        }
        swiWaitForVBlank( );
        swiWaitForVBlank( );
    }

    void mapDrawer::runDayCareLady( u8 p_daycare ) {
        char        buffer[ 200 ] = { 0 };
        boxPokemon* dc1           = &SAVE::SAV.getActiveFile( ).m_dayCarePkmn[ p_daycare * 2 ];
        boxPokemon* dc2           = &SAVE::SAV.getActiveFile( ).m_dayCarePkmn[ p_daycare * 2 + 1 ];
        boxPokemon* dce           = &SAVE::SAV.getActiveFile( ).m_dayCareEgg[ p_daycare ];

        u8* dcl1 = &SAVE::SAV.getActiveFile( ).m_dayCareDepositLevel[ p_daycare * 2 ];
        u8* dcl2 = &SAVE::SAV.getActiveFile( ).m_dayCareDepositLevel[ p_daycare * 2 + 1 ];

        if( dce->getSpecies( ) ) {
            // an egg spawned, redirect to jii san
            printMapMessage( GET_MAP_STRING( 476 ), MSG_NORMAL );
            return;
        }

        if( !dc1->getSpecies( ) && dc2->getSpecies( ) ) {
            std::swap( *dc1, *dc2 );
            std::swap( *dcl1, *dcl2 );
        }

        u8 depositpkmn = false;

        if( !dc1->getSpecies( ) ) {
            // no pkmn deposited, ask if player wants to deposit a pkmn
            if( IO::yesNoBox::YES
                == IO::yesNoBox( ).getResult(
                    convertMapString( GET_MAP_STRING( 477 ), MSG_NORMAL ).c_str( ), MSG_NORMAL ) ) {
                IO::init( );
                depositpkmn = true;
            } else {
                IO::init( );
                printMapMessage( GET_MAP_STRING( 478 ), MSG_NORMAL );
                return;
            }
        } else {
            snprintf( buffer, 199, GET_MAP_STRING( 479 ), dc1->m_name );
            printMapMessage( buffer, MSG_NORMAL );

            if( !dc2->getSpecies( ) ) {
                // ask if player wants to deposit a second pkmn
                if( IO::yesNoBox::YES
                    == IO::yesNoBox( ).getResult(
                        convertMapString( GET_MAP_STRING( 480 ), MSG_NORMAL ).c_str( ),
                        MSG_NORMAL ) ) {
                    IO::init( );
                    depositpkmn = 2;
                } else {
                    IO::init( );
                }
            }

            if( !depositpkmn ) {
                // ask if he player wants to take a pkmn back
                printMapMessage( GET_MAP_STRING( 483 ), MSG_NOCLOSE );
                loop( ) {
                    u8 takeback = IO::chooseDaycarePkmn( p_daycare );
                    IO::init( );

                    if( takeback > 1 ) {
                        // player doesn't want to get pkmn back
                        printMapMessage( GET_MAP_STRING( 478 ), MSG_NORMAL );
                        break;
                    }

                    // check if there is space in the player's team
                    if( SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) >= 6 ) {
                        printMapMessage( GET_MAP_STRING( 487 ), MSG_NORMAL );
                        break;
                    }

                    pokemon pk   = pokemon( dc1[ takeback ] );
                    u32     cost = ( pk.m_level - dcl1[ takeback ] + 1 ) * 100;
                    snprintf( buffer, 199, GET_MAP_STRING( 488 ), dc1[ takeback ].m_name, cost );

                    if( IO::yesNoBox::YES
                        == IO::yesNoBox( ).getResult(
                            convertMapString( buffer, MSG_NORMAL ).c_str( ), MSG_NORMAL ) ) {
                        IO::init( );
                        // check if the player has enough money
                        if( SAVE::SAV.getActiveFile( ).m_money >= cost ) {
                            SOUND::playSoundEffect( SFX_BUY_SUCCESSFUL );
                            SAVE::SAV.getActiveFile( ).m_money -= cost;
                            snprintf( buffer, 199, GET_MAP_STRING( 490 ), dc1[ takeback ].m_name );
                            printMapMessage( buffer, MSG_NORMAL );

                            snprintf( buffer, 199, GET_MAP_STRING( 491 ), dc1[ takeback ].m_name );
                            printMapMessage( buffer, MSG_INFO );

                            SAVE::SAV.getActiveFile( ).setTeamPkmn(
                                SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), &pk );

                            dc1[ takeback ]  = boxPokemon( );
                            dcl1[ takeback ] = 0;

                            if( !takeback ) {
                                std::swap( *dc1, *dc2 );
                                std::swap( *dcl1, *dcl2 );
                            }

                            // check if the player wants to take back the other
                            // pkmn as well
                            if( dc1->getSpecies( ) ) {
                                printMapMessage( GET_MAP_STRING( 492 ), MSG_NOCLOSE );
                                continue;
                            }
                            printMapMessage( GET_MAP_STRING( 482 ), MSG_NORMAL );
                            break;
                        } else {
                            printMapMessage( GET_MAP_STRING( 489 ), MSG_NORMAL );
                            break;
                        }
                    } else {
                        IO::init( );
                        printMapMessage( GET_MAP_STRING( 482 ), MSG_NORMAL );
                        break;
                    }
                }
                return;
            }
        }

        while( depositpkmn && depositpkmn <= 2 ) {
            // check if the player has at least 2 pkmn

            u8 plyerpkmncnt = 0;
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
                if( !SAVE::SAV.getActiveFile( ).getTeamPkmn( i )->isEgg( ) ) { plyerpkmncnt++; }
            }

            if( plyerpkmncnt < 2 ) {
                // player has only 1 pkmn
                printMapMessage( GET_MAP_STRING( 484 ), MSG_NORMAL );
                break;
            }

            // make player select a pkmn
            printMapMessage( GET_MAP_STRING( 481 ), MSG_NORMAL );

            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            STS::partyScreen sts = STS::partyScreen( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                     SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ),
                                                     false, false, false, 1, true, true, false );

            SOUND::dimVolume( );

            auto res = sts.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            IO::resetScale( true, false );
            bgUpdate( );

            ANIMATE_MAP = true;
            SOUND::restoreVolume( );

            IO::init( );
            MAP::curMap->draw( );

            // check if the player has another pkmn that can battle

            u8 selpkmn = res.getSelectedPkmn( );

            if( selpkmn >= SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) ) {
                // player aborted
                printMapMessage( GET_MAP_STRING( 482 ), MSG_NORMAL );
                break;
            }

            plyerpkmncnt = 0;
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
                if( i == selpkmn ) { continue; }
                if( SAVE::SAV.getActiveFile( ).getTeamPkmn( i )->canBattle( ) ) { plyerpkmncnt++; }
            }

            if( !plyerpkmncnt ) {
                // no remaining pkmn
                printMapMessage( GET_MAP_STRING( 485 ), MSG_NORMAL );
                break;
            }

            // actually deposit the pkmn

            if( SAVE::SAV.getActiveFile( ).getTeamPkmn( selpkmn ) != nullptr ) [[likely]] {
                dc1[ depositpkmn - 1 ]
                    = SAVE::SAV.getActiveFile( ).getTeamPkmn( selpkmn )->m_boxdata;
                dcl1[ depositpkmn - 1 ]
                    = SAVE::SAV.getActiveFile( ).getTeamPkmn( selpkmn )->m_level;
                SAVE::SAV.getActiveFile( ).setTeamPkmn( selpkmn, (boxPokemon*) nullptr );
                SAVE::SAV.getActiveFile( ).consolidatePkmn( );

                if( selpkmn == 0 ) { MAP::curMap->removeFollowPkmn( ); }

                snprintf( buffer, 199, GET_MAP_STRING( 486 ), dc1[ depositpkmn - 1 ].m_name );
                printMapMessage( buffer, MSG_NORMAL );
            } else {
                break;
            }

            if( depositpkmn < 2 ) {
                // ask if player wants to deposit a second pkmn
                if( IO::yesNoBox::YES
                    == IO::yesNoBox( ).getResult(
                        convertMapString( GET_MAP_STRING( 480 ), MSG_NORMAL ).c_str( ),
                        MSG_NORMAL ) ) {
                    IO::init( );
                    depositpkmn = 2;
                } else {
                    IO::init( );
                    printMapMessage( GET_MAP_STRING( 482 ), MSG_NORMAL );
                    break;
                }
            } else {
                break;
            }
        }
    }

    void mapDrawer::runDayCareGuy( u8 p_daycare ) {
        char        buffer[ 200 ] = { 0 };
        boxPokemon* dc1           = &SAVE::SAV.getActiveFile( ).m_dayCarePkmn[ p_daycare * 2 ];
        boxPokemon* dc2           = &SAVE::SAV.getActiveFile( ).m_dayCarePkmn[ p_daycare * 2 + 1 ];
        boxPokemon* dce           = &SAVE::SAV.getActiveFile( ).m_dayCareEgg[ p_daycare ];

        u8* dcl1 = &SAVE::SAV.getActiveFile( ).m_dayCareDepositLevel[ p_daycare * 2 ];
        u8* dcl2 = &SAVE::SAV.getActiveFile( ).m_dayCareDepositLevel[ p_daycare * 2 + 1 ];

        if( dce->getSpecies( ) ) {
            // an egg spawned
            // ask player if they want to obtain the egg
            if( IO::yesNoBox::NO
                == IO::yesNoBox( ).getResult(
                    convertMapString( GET_MAP_STRING( 464 ), MSG_NORMAL ).c_str( ), MSG_NORMAL ) ) {
                IO::init( );
                // ask if they really don't want the egg
                if( IO::yesNoBox::YES
                    == IO::yesNoBox( ).getResult(
                        convertMapString( GET_MAP_STRING( 465 ), MSG_NORMAL ).c_str( ),
                        MSG_NORMAL ) ) {
                    IO::init( );
                    // throw away the egg
                    *dce = boxPokemon( );
                    SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_HOENN_DAYCARE_EGG + p_daycare,
                                                        false );
                    printMapMessage( GET_MAP_STRING( 466 ), MSG_NORMAL );
                    return;
                }
            }
            IO::init( );
            // hand egg to player

            // check if they have space for an egg
            auto teampkmncnt = SAVE::SAV.getActiveFile( ).getTeamPkmnCount( );
            if( teampkmncnt >= 6 ) {
                // player has no space left
                printMapMessage( GET_MAP_STRING( 473 ), MSG_NORMAL );
                return;
            }

            SOUND::playSoundEffect( SFX_OBTAIN_EGG );
            printMapMessage( GET_MAP_STRING( 474 ), MSG_INFO );
            printMapMessage( GET_MAP_STRING( 475 ), MSG_NORMAL );

            dce->m_gotPlace = L_DAY_CARE_COUPLE;

            SAVE::SAV.getActiveFile( ).setTeamPkmn( teampkmncnt, dce );
            SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_HOENN_DAYCARE_EGG + p_daycare, false );
            *dce = boxPokemon( );
        } else {
            // no egg
            if( !dc1->getSpecies( ) && dc2->getSpecies( ) ) {
                std::swap( *dc1, *dc2 );
                std::swap( *dcl1, *dcl2 );
            }

            if( !dc1->getSpecies( ) ) {
                printMapMessage( GET_MAP_STRING( 463 ), MSG_NORMAL );
                return;
            } else {
                if( !dc2->getSpecies( ) ) {
                    snprintf( buffer, 199, GET_MAP_STRING( 467 ), dc1->m_name );
                    printMapMessage( buffer, MSG_NORMAL );
                } else {
                    snprintf( buffer, 199, GET_MAP_STRING( 468 ), dc1->m_name, dc2->m_name );
                    printMapMessage( buffer, MSG_NORMAL );

                    u8 comp = dc1->getCompatibility( *dc2 );
                    printMapMessage( GET_MAP_STRING( 469 + comp ), MSG_NORMAL );
                }
                return;
            }
        }
    }
} // namespace MAP
