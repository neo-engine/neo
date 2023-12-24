/*
Pokémon neo
------------------------------

file        : pokeblockUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2023 - 2023
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

#include <cstdio>

#include "bag/pokeblockUI.h"
#include "bag/pokeblockViewer.h"
#include "defines.h"
#include "fs/data.h"
#include "io/screenFade.h"
#include "io/strings.h"

// Sprites
#include "NoItem.h"

namespace BAG {
    constexpr u8 MAX_PARTY_PKMN = SAVE::NUM_PARTY_SLOTS;

    constexpr u8 SPR_POKEBLOCK_OAM_TOP = 0;

    constexpr u8 SPR_ARROW_BACK_OAM_SUB    = 53;
    constexpr u8 SPR_POKEBLOCK_OAM_SUB     = 55;
    constexpr u8 SPR_POKEBLOCK_BG_OAM_SUB  = 79;
    constexpr u8 SPR_PKMN_START_OAM_SUB    = 105;
    constexpr u8 SPR_MSG_PKMN_SEL_OAM_SUB  = 111;
    constexpr u8 SPR_POKEBLOCK_SEL_OAM_SUB = 127;

    constexpr u8 SPR_POKEBLOCK_PAL_TOP = 0;

    constexpr u8 SPR_SELECTED_PAL_SUB        = 0;
    constexpr u8 SPR_PKMN_PAL_SUB            = 1;
    constexpr u8 SPR_ARROW_X_PAL_SUB         = 7;
    constexpr u8 SPR_BACK_PAL_SUB            = 8;
    constexpr u8 SPR_POKEBLOCK_BLACK_PAL_SUB = 9;
    constexpr u8 SPR_POKEBLOCK_RED_PAL_SUB   = 10;
    constexpr u8 SPR_POKEBLOCK_PAL_SUB       = 11;

    constexpr u8 POKEBLOCK_X      = 136;
    constexpr u8 POKEBLOCK_Y      = 4;
    constexpr u8 POKEBLOCK_SIZE_X = 28;
    constexpr u8 POKEBLOCK_SIZE_Y = 26;

    constexpr u16 EMPTY_PAL[ 16 ] = { 0 };
    constexpr u16 RED_PAL[ 16 ]
        = { 0,      0x151f, 0x151f, 0x151f, 0x151f, 0x151f, 0x151f, 0x151f,
            0x151f, 0x151f, 0x151f, 0x151f, 0x151f, 0x151f, 0x151f, 0x151f };

    std::vector<std::pair<IO::inputTarget, u8>> pokeblockUI::getTouchPositions( ) {
        return { };
    }

    void pokeblockUI::init( ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::initOAMTable( false );
        IO::initOAMTable( true );
        IO::vramSetup( );
        swiWaitForVBlank( );
        IO::clearScreen( true, true );
        IO::resetScale( true, true );

        // top

        u16 tileCntT = 0;

        tileCntT = IO::loadPokeblockIcon( 0, 16, 134, SPR_POKEBLOCK_OAM_TOP, SPR_POKEBLOCK_PAL_TOP,
                                          tileCntT, false );

        // bottom
        u16 tileCnt = 0;

        for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
            auto acPkmn = _playerTeam[ i ];
            if( !acPkmn.getSpecies( ) ) break;
            if( acPkmn.isEgg( ) ) {
                tileCnt = IO::loadEggIcon( 10, 23 + i * 26, SPR_PKMN_START_OAM_SUB + i,
                                           SPR_PKMN_PAL_SUB + i, tileCnt );
            } else {
                tileCnt = IO::loadPKMNIcon( acPkmn.getSpriteInfo( ), 9, 25 + i * 26,
                                            SPR_PKMN_START_OAM_SUB + i, SPR_PKMN_PAL_SUB + i,
                                            tileCnt, true );
            }
            IO::Oam->oamBuffer[ SPR_PKMN_START_OAM_SUB + i ].priority = OBJPRIORITY_3;
        }

        tileCnt = IO::loadSprite( "BG/Back", SPR_ARROW_BACK_OAM_SUB, SPR_BACK_PAL_SUB, tileCnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 26, 32, 32, false, false,
                                  false, OBJPRIORITY_3, true );

        // Pokeblocks, load blocks
        u8 t = 0;
        for( u8 y = 0; y < BLOCKS_PER_PAGE / BLOCKS_PER_ROW; ++y ) {
            for( u8 x = 0; x < BLOCKS_PER_ROW; ++x, ++t ) {
                tileCnt = IO::loadPokeblockIcon(
                    t, POKEBLOCK_X + POKEBLOCK_SIZE_X * x, POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y,
                    SPR_POKEBLOCK_OAM_SUB + t, SPR_POKEBLOCK_PAL_SUB + t / 6, tileCnt );
                tileCnt = IO::loadPokeblockIcon(
                    t, POKEBLOCK_X + POKEBLOCK_SIZE_X * x, POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y,
                    SPR_POKEBLOCK_BG_OAM_SUB + t, SPR_POKEBLOCK_PAL_SUB + t / 6, tileCnt );
            }
        }

        // Pkmn selection
        for( u8 i = 1; i < 5; ++i ) {
            IO::loadSprite( SPR_MSG_PKMN_SEL_OAM_SUB + i, SPR_SELECTED_PAL_SUB, tileCnt,
                            4 + 24 * ( 5 - i ), 33, 32, 32, 0, 0, 0, false, false, true,
                            OBJPRIORITY_3, true, OBJMODE_BLENDED );
        }
        tileCnt = IO::loadSprite( "SEL/noselection_blank_32_24", SPR_MSG_PKMN_SEL_OAM_SUB,
                                  SPR_SELECTED_PAL_SUB, tileCnt, 8, 33, 32, 32, false, false, true,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        IO::updateOAM( true );

        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PokeblockUpper" );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "PokeblockLower", 512,
                             49152, true );

        IO::initColors( );

        // "PokeBlocks"
        IO::regularFont->printString( GET_STRING( 739 ), 128, 5, false, IO::font::CENTER );

        bgUpdate( );
    }

    void pokeblockUI::initBlockView( u16* p_pokeblockCount ) {
        char buffer[ 100 ];
#ifdef DESQUID
        p_pokeblockCount[ 6 ] = 0;
        // for( u8 i = 0; i < 24; ++i ) { p_pokeblockCount[ i ] = rand( ) % 999; }
#endif

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        // "Select"
        IO::regularFont->printString( GET_STRING( 740 ), 64, 5, true, IO::font::CENTER );

        // Pokeblocks, load blocks
        IO::loadPokeblockIcon( 0, POKEBLOCK_X + POKEBLOCK_SIZE_X * 0,
                               POKEBLOCK_Y + POKEBLOCK_SIZE_Y * 0, SPR_POKEBLOCK_SEL_OAM_SUB,
                               SPR_POKEBLOCK_RED_PAL_SUB,
                               IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB ].gfxIndex );

        IO::Oam->oamBuffer[ SPR_POKEBLOCK_SEL_OAM_SUB ].isHidden = true;

        u8 t = 0;
        for( u8 y = 0; y < BLOCKS_PER_PAGE / BLOCKS_PER_ROW; ++y ) {
            for( u8 x = 0; x < BLOCKS_PER_ROW; ++x, ++t ) {
                if( !t || !p_pokeblockCount[ t ] ) {
                    IO::boldFont->setColor( 0, 1 );
                    IO::boldFont->setColor( IO::WHITE_IDX, 2 );
                } else {
                    IO::boldFont->setColor( IO::WHITE_IDX, 1 );
                    IO::boldFont->setColor( IO::BLACK_IDX, 2 );
                }
                if( p_pokeblockCount[ t ] ) {
                    IO::loadPokeblockIcon(
                        t, POKEBLOCK_X + POKEBLOCK_SIZE_X * x, POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y,
                        SPR_POKEBLOCK_OAM_SUB + t, SPR_POKEBLOCK_PAL_SUB + t / 6,
                        IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + t ].gfxIndex );
                } else {
                    IO::loadPokeblockIcon(
                        t, POKEBLOCK_X + POKEBLOCK_SIZE_X * x, POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y,
                        SPR_POKEBLOCK_OAM_SUB + t, SPR_POKEBLOCK_BLACK_PAL_SUB,
                        IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + t ].gfxIndex );
                }
                IO::loadPokeblockIcon( t, POKEBLOCK_X + POKEBLOCK_SIZE_X * x,
                                       POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y,
                                       SPR_POKEBLOCK_BG_OAM_SUB + t, SPR_POKEBLOCK_BLACK_PAL_SUB,
                                       IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + t ].gfxIndex );
                IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + t ].isRotateScale    = true;
                IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + t ].rotationIndex    = 1;
                IO::Oam->oamBuffer[ SPR_POKEBLOCK_BG_OAM_SUB + t ].isRotateScale = true;
                IO::Oam->oamBuffer[ SPR_POKEBLOCK_BG_OAM_SUB + t ].rotationIndex = 0;

                if( p_pokeblockCount[ t ] < 99 ) {
                    snprintf( buffer, 99, "%u", p_pokeblockCount[ t ] );
                    IO::boldFont->printStringC( buffer, POKEBLOCK_X + POKEBLOCK_SIZE_X * x + 15,
                                                POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y + 7, true,
                                                IO::font::CENTER );
                } else {
                    IO::boldFont->printStringC( "99", POKEBLOCK_X + POKEBLOCK_SIZE_X * x + 15,
                                                POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y + 7, true,
                                                IO::font::CENTER );
                }
            }
        }

        IO::Oam->matrixBuffer[ 0 ].vdx = ( 0LL << 8 );
        IO::Oam->matrixBuffer[ 0 ].vdy = ( 3LL << 7 );
        IO::Oam->matrixBuffer[ 0 ].hdx = ( 3LL << 7 );
        IO::Oam->matrixBuffer[ 0 ].hdy = ( 0LL << 8 );

        IO::Oam->matrixBuffer[ 1 ].vdx = ( 0LL << 8 );
        IO::Oam->matrixBuffer[ 1 ].vdy = ( 3LL << 7 ) + 31;
        IO::Oam->matrixBuffer[ 1 ].hdx = ( 3LL << 7 ) + 31;
        IO::Oam->matrixBuffer[ 1 ].hdy = ( 0LL << 8 );

        IO::copySpritePal( EMPTY_PAL, SPR_POKEBLOCK_BLACK_PAL_SUB, 0, 2 * 16, true );
        IO::copySpritePal( RED_PAL, SPR_POKEBLOCK_RED_PAL_SUB, 0, 2 * 16, true );

        // hide pkmn selection sprite
        for( u8 i = 0; i < 5; ++i ) {
            IO::Oam->oamBuffer[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].isHidden = true;
        }
        IO::updateOAM( true );
        IO::updateOAM( false );
    }

    void pokeblockUI::initPkmnView( ) {
        IO::printRectangle( 141, 0, 255, 192, true, 0 );
        IO::printRectangle( 0, 0, 140, 27, true, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        // "Feed"
        IO::regularFont->printString( GET_STRING( 741 ), 64, 5, true, IO::font::CENTER );

        // hide pokeblocks on subscreen
        for( u8 i = 0; i < BLOCKS_PER_PAGE; ++i ) {
            IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + i ].isRotateScale    = false;
            IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB + i ].isHidden         = true;
            IO::Oam->oamBuffer[ SPR_POKEBLOCK_BG_OAM_SUB + i ].isRotateScale = false;
            IO::Oam->oamBuffer[ SPR_POKEBLOCK_BG_OAM_SUB + i ].isHidden      = true;
        }
        IO::Oam->oamBuffer[ SPR_POKEBLOCK_SEL_OAM_SUB ].isHidden = true;

        IO::updateOAM( true );
        IO::updateOAM( false );
    }

    void pokeblockUI::animateFeedBlockToPkmn( u8 p_pkmnIdx, u8 p_blockType ) {
        // load pkmn sprite
        // load block sprite
        // load dispenser sprite
    }

    void pokeblockUI::selectBlock( u8 p_blockType, u16 p_blockOwned ) {
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        IO::printRectangle( 0, 28, 140, 192, true, 0 );
        IO::printRectangle( 0, IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y, 255, 192, false,
                            0 );
        if( p_blockOwned ) {
            char buffer[ 100 ];

            // draw info on top
            IO::loadPokeblockIcon( p_blockType, IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].x,
                                   IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y,
                                   SPR_POKEBLOCK_OAM_TOP, SPR_POKEBLOCK_PAL_TOP,
                                   IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].gfxIndex, false );

            // block amount
            IO::regularFont->printString( std::to_string( p_blockOwned ).c_str( ),
                                          IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].x + 16,
                                          IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y + 32,
                                          false, IO::font::CENTER );

            // block flavor
            IO::regularFont->printStringC(
                GET_STRING( 766 + p_blockType ),
                IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].x * 2 + 24,
                IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y, false, IO::font::LEFT );

            // Flavor
            IO::regularFont->setColor( IO::BLUE_IDX, 1 );
            snprintf( buffer, 99, GET_STRING( 791 ),
                      pokeblock::flavorStrength( pokeblockType{ p_blockType } ) );
            IO::regularFont->printStringC( buffer, 64 + 48,
                                           IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y + 32,
                                           false, IO::font::CENTER );

            // Strength
            IO::regularFont->setColor( IO::RED_IDX, 1 );
            snprintf( buffer, 99, GET_STRING( 792 ),
                      pokeblock::smoothness( pokeblockType{ p_blockType } ) );
            IO::regularFont->printStringC( buffer, 128 + 64,
                                           IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y + 32,
                                           false, IO::font::CENTER );
        } else {
            // "Try to create this block"
            IO::regularFont->printStringC( GET_STRING( 790 ), 128,
                                           IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].y + 16,
                                           false, IO::font::CENTER );
            IO::OamTop->oamBuffer[ SPR_POKEBLOCK_OAM_TOP ].isHidden = true;
        }

        if( !IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB ].isHidden ) {
            // highlight block on bottom
            u8 x = p_blockType % BLOCKS_PER_ROW;
            u8 y = p_blockType / BLOCKS_PER_ROW;
            IO::loadPokeblockIcon( 0, POKEBLOCK_X + POKEBLOCK_SIZE_X * x,
                                   POKEBLOCK_Y + POKEBLOCK_SIZE_Y * y, SPR_POKEBLOCK_SEL_OAM_SUB,
                                   SPR_POKEBLOCK_RED_PAL_SUB,
                                   IO::Oam->oamBuffer[ SPR_POKEBLOCK_OAM_SUB ].gfxIndex );
            IO::copySpritePal( RED_PAL, SPR_POKEBLOCK_RED_PAL_SUB, 0, 2 * 16, true );
            IO::updateOAM( true );
        }
        IO::updateOAM( false );

        // update pkmn info

        BG_PALETTE_SUB[ IO::COLOR_IDX ] = IO::GREEN;
        for( u8 i = 0; i < MAX_PARTY_PKMN; ++i ) {
            const u8 FIRST_LINE  = 27 + 26 * i;
            const u8 SECOND_LINE = 39 + 26 * i;
            const u8 SINGLE_LINE = 32 + 26 * i;

            if( !_playerTeam[ i ].m_boxdata.m_speciesId ) {
                IO::printRectangle( 0, 28 + 26 * i, 133, 28 + 26 * i + 26, true, 0 );
                continue;
            }

            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->setColor( IO::GRAY_IDX, 2 );

            if( _playerTeam[ i ].isEgg( ) ) {
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                IO::regularFont->setColor( IO::GRAY_IDX, 2 );
                IO::regularFont->printStringC( GET_STRING( IO::STR_UI_BAG_PARTY_EGG ), 45,
                                               SINGLE_LINE, true );
            } else {
                if( p_blockOwned ) {
                    IO::regularFont->printStringC( _playerTeam[ i ].m_boxdata.m_name, 45,
                                                   FIRST_LINE, true );
                    IO::regularFont->setColor( 0, 2 );
                    if( _playerTeam[ i ].m_boxdata.m_contestStats[ 5 ] == 255 ) {
                        // pkmn is full
                        IO::regularFont->setColor( IO::BLUE_IDX, 1 );
                        IO::regularFont->printStringC( GET_STRING( 796 ), 45, SECOND_LINE, true );
                    } else {
                        auto str = pokeblock::strengthModifier( pokeblockType{ p_blockType },
                                                                _playerTeam[ i ].getNature( ) );
                        if( str == LIKED_FALVOR ) {
                            IO::regularFont->setColor( IO::COLOR_IDX, 1 );
                            IO::regularFont->printStringC( GET_STRING( 793 ), 45, SECOND_LINE,
                                                           true );
                        } else if( str == NORMAL_FALVOR ) {
                            IO::regularFont->printStringC( GET_STRING( 794 ), 45, SECOND_LINE,
                                                           true );
                        } else if( str == DISLIKED_FALVOR ) {
                            IO::regularFont->setColor( IO::RED_IDX, 1 );
                            IO::regularFont->printStringC( GET_STRING( 795 ), 45, SECOND_LINE,
                                                           true );
                        }
                    }
                } else {
                    IO::regularFont->printStringC( _playerTeam[ i ].m_boxdata.m_name, 45,
                                                   SINGLE_LINE, true );
                }
            }
        }
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
    }

    void pokeblockUI::selectPkmn( u8 p_pkmnIdx, u8 p_blockType ) {
        // highlight selected pkmn

        auto& oam = IO::Oam->oamBuffer;
        if( p_pkmnIdx < MAX_PARTY_PKMN ) {
            for( u8 i = 0; i < 5; ++i ) {
                oam[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].isHidden = false;
                oam[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].y        = 29 + 26 * p_pkmnIdx;
            }
        } else {
            for( u8 i = 0; i < 5; ++i ) { oam[ SPR_MSG_PKMN_SEL_OAM_SUB + i ].isHidden = true; }
        }
        IO::updateOAM( true );

        // update condition preview
    }
} // namespace BAG
