/*
    Pokémon neo
    ------------------------------

    file        : specialsPokeblockBlender.cpp
    author      : Philip Wellnitz
    description : Mini game to create Pokeblocks

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

#include <algorithm>
#include <tuple>
#include <vector>
#include <nds.h>

#include "bag/bagViewer.h"
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
    struct pokeblockNPC {
        u16 m_name;        // mapstring idx, if 0 then rotom
        u8  m_percPerfect; // chance in /255 to hit perfect
        u8  m_percHit;     // chance in /255 to hit
        u8  m_percMiss;    // chance in /255 to miss
        u8  m_berryTier;   // 0 - selected by player,
    };

    struct berryPathInfo {
        s16 m_startX;
        s16 m_startY;
        s8  m_speedX;
        s8  m_speedY;
    };

    char NAME_CACHE[ 3 ][ 20 ];

    constexpr berryPathInfo BERRY_START_DATA[ 4 ] = {
        { 242, 20, -2, 1 },
        { -18, 140, 2, -1 },
        { 242, 140, -2, -1 },
        { -18, 20, 2, 1 },
    };

    constexpr u8 BERRY_TIER_LENGTH = 5;
    constexpr u8 HITBONUS_MODIFIER = 1;
    constexpr u8 MASTER_TIER       = 0;
    constexpr u8 ROTOM_TIER        = 4;

    constexpr u16 TEXTFIELD_COLOR     = 0x8c68;
    constexpr u8  TEXTFIELD_COLOR_IDX = 20;

    constexpr u16 TEXT_COLOR      = 0x8856;
    constexpr u8  TEXT_COLOR_IDX  = 21;
    constexpr u16 TEXT_COLOR2     = 0x9ddd;
    constexpr u8  TEXT_COLOR_IDX2 = 22;

    constexpr u8 SPR_PERFECT_OAM = 0;
    constexpr u8 SPR_MISS_OAM    = 1;
    constexpr u8 SPR_GOOD_OAM    = 2;
    constexpr u8 SPR_BERRY_OAM   = 3;
    constexpr u8 BERRY_Y         = 64;
    constexpr u8 BERRY_MIN_Y     = 24;
    constexpr u8 BERRY_MAX_Y     = 84;

    constexpr u8 SPR_PERFECT_PAL = 0;
    constexpr u8 SPR_MISS_PAL    = 1;
    constexpr u8 SPR_GOOD_PAL    = 2;
    constexpr u8 SPR_BERRY_PAL   = 3;

    constexpr u8 SPR_ACTIVE_SUB_OAM_START = 0;
    constexpr u8 SPR_ACTIVE_SUB_OAM_END   = 3;
    constexpr u8 SPR_BERRY_SUB_OAM        = 4;
    constexpr u8 SPR_LID_SUB_OAM          = 4;
    constexpr u8 SPR_CNT_SUB_OAM          = 5;
    constexpr u8 SPR_PERFECT_SUB_OAM      = 10;
    constexpr u8 SPR_MISS_SUB_OAM         = 20;
    constexpr u8 SPR_GOOD_SUB_OAM         = 30;

    constexpr u8 SPR_ACTIVE_SUB_PAL  = 0;
    constexpr u8 SPR_BERRY_SUB_PAL   = 1;
    constexpr u8 SPR_LID_SUB_PAL     = 1;
    constexpr u8 SPR_CNT_SUB_PAL     = 2;
    constexpr u8 SPR_PERFECT_SUB_PAL = 3;
    constexpr u8 SPR_MISS_SUB_PAL    = 4;
    constexpr u8 SPR_GOOD_SUB_PAL    = 5;

    u16 BERRY_TIER_1[ BERRY_TIER_LENGTH ]
        = { I_CHERI_BERRY, I_PECHA_BERRY, I_RAWST_BERRY, I_ASPEAR_BERRY, I_CHESTO_BERRY };
    u16 BERRY_TIER_2[ BERRY_TIER_LENGTH ]
        = { I_SPELON_BERRY, I_PAMTRE_BERRY, I_WATMEL_BERRY, I_DURIN_BERRY, I_BELUE_BERRY };

    // used by master if player chose tier 2 berry
    u16 BERRY_TIER_2b[ BERRY_TIER_LENGTH ]
        = { I_TAMATO_BERRY, I_CORNN_BERRY, I_MAGOST_BERRY, I_RABUTA_BERRY, I_NOMEL_BERRY };

    pokeblockNPC BLENDER_NPC[ 5 ][ 3 ] = {
        { { 717, 255, 0, 0, 2 } }, // berry master
        { { 711, 60, 140, 80, 1 } },
        { { 712, 80, 120, 40, 1 }, { 713, 120, 140, 80, 1 } },
        { { 714, 120, 180, 60, 1 }, { 715, 40, 160, 10, 1 }, { 716, 60, 120, 80, 1 } },
        { { 0, 255, 0, 0, 0 }, { 0, 255, 0, 0, 0 }, { 0, 255, 0, 0, 0 } }, // rotom
    };

    u8 computeBaseSmoothness( u8 p_chosenBerries[ 4 ], u16 p_rpm ) {
        // take average of berry smoothness, subtract number of berries
        u8 smooth = 0;
        u8 cnt    = 0;
        for( ; cnt < 4; ++cnt ) {
            if( !p_chosenBerries[ cnt ] ) {
                --cnt;
                break;
            }
            smooth += FS::getBerryData( p_chosenBerries[ cnt ] ).m_smoothness;
        }
        if( cnt == 0 ) { return 0; }
        smooth /= cnt;
        return static_cast<u8>( std::min( 255, int( smooth - cnt ) * 10000 / p_rpm ) );
    }

    u8 computeBaseLevel( u8 p_chosenBerries[ 4 ] ) {
        // for each berry, compute smoothness values

        s16 flavor[ BAG::NUM_BERRYSTATS ] = { 0 };

        u8 cnt = 0;
        for( ; cnt < 4; ++cnt ) {
            if( !p_chosenBerries[ cnt ] ) {
                --cnt;
                break;
            }
            auto data = FS::getBerryData( p_chosenBerries[ cnt ] );
            for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) { flavor[ j ] += data.m_flavor[ j ]; }
        }

        s16 flavor_reduced[ BAG::NUM_BERRYSTATS ];

        flavor_reduced[ BAG::BY_SPICY ]  = flavor[ BAG::BY_SPICY ] - flavor[ BAG::BY_DRY ];
        flavor_reduced[ BAG::BY_DRY ]    = flavor[ BAG::BY_DRY ] - flavor[ BAG::BY_SWEET ];
        flavor_reduced[ BAG::BY_SWEET ]  = flavor[ BAG::BY_SWEET ] - flavor[ BAG::BY_BITTER ];
        flavor_reduced[ BAG::BY_BITTER ] = flavor[ BAG::BY_BITTER ] - flavor[ BAG::BY_SOUR ];
        flavor_reduced[ BAG::BY_SOUR ]   = flavor[ BAG::BY_SOUR ] - flavor[ BAG::BY_SPICY ];

        u8 mx = 0;
        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) {
            if( flavor_reduced[ j ] > mx ) { mx = static_cast<u8>( flavor_reduced[ j ] ); }
        }
        return mx;
    }

    BAG::pokeblockType computeBlockType( u8 p_chosenBerries[ 4 ], u8 p_hitbonus, u16 p_rpm ) {
        if( !p_rpm ) { return BAG::PB_BLACK; }

        // for each berry, compute smoothness values
        u8 flavor[ BAG::NUM_BERRYSTATS ] = { 0 };

        u8 cnt = 0;
        for( ; cnt < 4; ++cnt ) {
            if( !p_chosenBerries[ cnt ] ) {
                --cnt;
                break;
            }
            auto data = FS::getBerryData( p_chosenBerries[ cnt ] );
            for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) { flavor[ j ] += data.m_flavor[ j ]; }
        }

        for( u8 i = 0; i < cnt - 1; ++i ) {
            for( u8 j = i + 1; j < cnt; ++j ) {
                if( p_chosenBerries[ i ] == p_chosenBerries[ j ] ) { return BAG::PB_BLACK; }
            }
        }

        s16 flavor_reduced[ BAG::NUM_BERRYSTATS ];

        flavor_reduced[ BAG::BY_SPICY ] = s16{ flavor[ BAG::BY_SPICY ] } - flavor[ BAG::BY_DRY ];
        flavor_reduced[ BAG::BY_DRY ]   = s16{ flavor[ BAG::BY_DRY ] } - flavor[ BAG::BY_SWEET ];
        flavor_reduced[ BAG::BY_SWEET ] = s16{ flavor[ BAG::BY_SWEET ] } - flavor[ BAG::BY_BITTER ];
        flavor_reduced[ BAG::BY_BITTER ] = s16{ flavor[ BAG::BY_BITTER ] } - flavor[ BAG::BY_SOUR ];
        flavor_reduced[ BAG::BY_SOUR ]   = s16{ flavor[ BAG::BY_SOUR ] } - flavor[ BAG::BY_SPICY ];

        u8 numneg = 0;
        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) { numneg += flavor_reduced[ j ] < 0; }

        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) {
            flavor_reduced[ j ] = std::max( 0, flavor_reduced[ j ] - numneg );
        }

        for( u8 j = 0; j < BAG::NUM_BERRYSTATS; ++j ) {
            flavor[ j ] = static_cast<u8>( std::min(
                s32( 255 ), s32( flavor_reduced[ j ] ) * ( 1 + p_hitbonus / HITBONUS_MODIFIER ) ) );
        }
        return BAG::pokeblock::fromLevelSmoothness(
            flavor, computeBaseSmoothness( p_chosenBerries, p_rpm ) );
    }

    u8 computeNPCBerry( u8 p_chosenBerries[ 4 ], u8 p_berryTier ) {
        // pick berry that is best for the pokeblock from available tier

        if( p_berryTier == 1 ) {
            u8 mxlv = 0, argmx = BAG::itemToBerry( BERRY_TIER_1[ 0 ] );
            for( u8 i = 0; i < BERRY_TIER_LENGTH; ++i ) {
                bool ignore = false;
                u8   fr     = 4;
                for( u8 j = 0; j < 4; ++j ) {
                    if( p_chosenBerries[ j ] == BAG::itemToBerry( BERRY_TIER_1[ i ] ) ) {
                        ignore = true;
                        break;
                    }
                    if( !p_chosenBerries[ j ] ) {
                        fr = j;
                        break;
                    }
                }
                if( ignore ) { continue; }

                p_chosenBerries[ fr ] = BAG::itemToBerry( BERRY_TIER_1[ i ] );
                u8 nlv                = computeBaseLevel( p_chosenBerries );
                p_chosenBerries[ fr ] = 0;

                if( nlv > mxlv ) {
                    mxlv  = nlv;
                    argmx = BAG::itemToBerry( BERRY_TIER_1[ i ] );
                }
            }
            return argmx;
        }

        if( p_berryTier == 2 ) {
            // first check if a replacement berry should be picked
            bool repl = false;
            for( u8 i = 0; i < BERRY_TIER_LENGTH; ++i ) {
                for( u8 j = 0; j < 4 && !repl; ++j ) {
                    if( p_chosenBerries[ j ] == BAG::itemToBerry( BERRY_TIER_2[ i ] ) ) {
                        repl = true;
                        break;
                    }
                }
            }

            u8 fr = 4;
            for( u8 j = 0; j < 4; ++j ) {
                if( !p_chosenBerries[ j ] ) {
                    fr = j;
                    break;
                }
            }

            u8 mxlv = 0, argmx = BAG::itemToBerry( repl ? BERRY_TIER_2b[ 0 ] : BERRY_TIER_2[ 0 ] );
            for( u8 i = 0; i < BERRY_TIER_LENGTH; ++i ) {
                u8 cbr = BAG::itemToBerry( repl ? BERRY_TIER_2b[ i ] : BERRY_TIER_2[ i ] );

                p_chosenBerries[ fr ] = cbr;
                u8 nlv                = computeBaseLevel( p_chosenBerries );
                p_chosenBerries[ fr ] = 0;

                if( nlv > mxlv ) {
                    mxlv  = nlv;
                    argmx = cbr;
                }
            }
            return argmx;
        }

        return 0;
    }

#define PROGRESSBAR_START 94
#define PROGRESSBAR_END   162
    void drawProgressBar( u8 p_progress ) {
        IO::printRectangle( PROGRESSBAR_START, 2, PROGRESSBAR_START + 3 * p_progress - 1, 16, true,
                            0 );
    }

#define PLAYERNAME_TOPY   54
#define PLAYERNAME_BOTY   120
#define PLAYERNAME_LEFTX  37
#define PLAYERNAME_RIGHTX 216

    void initBlender( u8 p_npctier ) {
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        IO::initOAMTable( false );
        IO::initOAMTable( true );
        IO::vramSetup( true );
        swiWaitForVBlank( );
        IO::clearScreen( true, true );
        IO::resetScale( true, true );

        // top: score board and result prediction
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PokeblockUpper", 512, 49152,
                             false );
        IO::initColors( false, true, true );
        // "Berry blender"
        IO::regularFont->printString( GET_STRING( 802 ), 128, 5, false, IO::font::CENTER );

        u16 tileCntT = 0;
        tileCntT
            = IO::loadUIIcon( IO::ICON::BB_PERFECT_START, SPR_PERFECT_OAM, SPR_PERFECT_PAL,
                              tileCntT, 0, 0, 16, 16, false, false, true, OBJPRIORITY_1, false );
        tileCntT = IO::loadUIIcon( IO::ICON::BB_MISS_START, SPR_MISS_OAM, SPR_MISS_PAL, tileCntT, 0,
                                   0, 16, 16, false, false, true, OBJPRIORITY_1, false );
        tileCntT = IO::loadUIIcon( IO::ICON::BB_GOOD_START, SPR_GOOD_OAM, SPR_GOOD_PAL, tileCntT, 0,
                                   0, 16, 16, false, false, true, OBJPRIORITY_1, false );

        for( u8 i = 0; i < 4; ++i ) {
            tileCntT = IO::loadItemIcon( 0, 64 + 32 * ( ( i + 1 ) % 4 ), BERRY_Y, SPR_BERRY_OAM + i,
                                         SPR_BERRY_PAL + i, tileCntT, false );
            IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].isHidden = true;
        }
        IO::updateOAM( false );
        // load all types of pokeblocks and display the predicted one9

        // bottom: blander and actual mini game
        // load sprites

        u16 tileCnt = 0;
        IO::loadUIIcon( IO::ICON::BB_PLAYER_ACTIVE_START, SPR_ACTIVE_SUB_OAM_START,
                        SPR_ACTIVE_SUB_PAL, tileCnt, 173, 46 - 32, 32, 32, false, false, false,
                        OBJPRIORITY_3, true );
        IO::loadUIIcon( IO::ICON::BB_PLAYER_ACTIVE_START, SPR_ACTIVE_SUB_OAM_START + 1,
                        SPR_ACTIVE_SUB_PAL, tileCnt, 83 - 32, 146, 32, 32, true, true, false,
                        OBJPRIORITY_3, true );
        IO::loadUIIcon( IO::ICON::BB_PLAYER_ACTIVE_START, SPR_ACTIVE_SUB_OAM_START + 2,
                        SPR_ACTIVE_SUB_PAL, tileCnt, 173, 146, 32, 32, true, false, false,
                        OBJPRIORITY_3, true );
        tileCnt = IO::loadUIIcon( IO::ICON::BB_PLAYER_ACTIVE_START, SPR_ACTIVE_SUB_OAM_START + 3,
                                  SPR_ACTIVE_SUB_PAL, tileCnt, 83 - 32, 46 - 32, 32, 32, false,
                                  true, false, OBJPRIORITY_3, true );

        tileCnt
            = IO::loadUIIcon( IO::ICON::BB_LID_START, SPR_LID_SUB_OAM, SPR_LID_SUB_PAL, tileCnt,
                              128 - 64, 96 - 64, 64, 64, false, true, true, OBJPRIORITY_3, true );

        tileCnt
            = IO::loadUIIcon( IO::ICON::BB_START_START, SPR_CNT_SUB_OAM, SPR_CNT_SUB_PAL, tileCnt,
                              83 - 32, 46 - 32, 64, 32, false, true, true, OBJPRIORITY_1, true );

        tileCnt
            = IO::loadUIIcon( IO::ICON::BB_PERFECT_START, SPR_PERFECT_SUB_OAM, SPR_PERFECT_SUB_PAL,
                              tileCnt, 0, 0, 16, 16, false, false, true, OBJPRIORITY_1, true );
        tileCnt = IO::loadUIIcon( IO::ICON::BB_MISS_START, SPR_MISS_SUB_OAM, SPR_MISS_SUB_PAL,
                                  tileCnt, 0, 0, 16, 16, false, false, true, OBJPRIORITY_1, true );
        tileCnt = IO::loadUIIcon( IO::ICON::BB_GOOD_START, SPR_GOOD_SUB_OAM, SPR_GOOD_SUB_PAL,
                                  tileCnt, 0, 0, 16, 16, false, false, true, OBJPRIORITY_1, true );

        for( u8 i = 1; i < 10; ++i ) {
            IO::loadUIIcon( IO::ICON::BB_PERFECT_START, SPR_PERFECT_SUB_OAM + i,
                            SPR_PERFECT_SUB_PAL, IO::Oam->oamBuffer[ SPR_PERFECT_SUB_OAM ].gfxIndex,
                            0, 0, 16, 16, false, false, true, OBJPRIORITY_1, true );
            IO::loadUIIcon( IO::ICON::BB_MISS_START, SPR_MISS_SUB_OAM + i, SPR_MISS_SUB_PAL,
                            IO::Oam->oamBuffer[ SPR_MISS_SUB_OAM ].gfxIndex, 0, 0, 16, 16, false,
                            false, true, OBJPRIORITY_1, true );
            IO::loadUIIcon( IO::ICON::BB_GOOD_START, SPR_GOOD_SUB_OAM + i, SPR_GOOD_SUB_PAL,
                            IO::Oam->oamBuffer[ SPR_GOOD_SUB_OAM ].gfxIndex, 0, 0, 16, 16, false,
                            false, true, OBJPRIORITY_1, true );
        }

        tileCnt = IO::loadItemIcon( 0, 0, 0, SPR_BERRY_SUB_OAM, SPR_BERRY_SUB_PAL, tileCnt );
        IO::Oam->oamBuffer[ SPR_BERRY_SUB_OAM ].isHidden = true;

        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( 0, 1 );
        IO::boldFont->setColor( TEXT_COLOR_IDX, 2 );

        // load bg graphic
        if( p_npctier == ROTOM_TIER ) {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "blendersub_rotom", 512,
                                 49152, true );
            BG_PALETTE_SUB[ TEXTFIELD_COLOR_IDX ] = TEXTFIELD_COLOR;
            BG_PALETTE_SUB[ TEXT_COLOR_IDX ]      = TEXT_COLOR;
            BG_PALETTE_SUB[ TEXT_COLOR_IDX2 ]     = TEXT_COLOR2;

            // initialize progress bar
            IO::printRectangle( PROGRESSBAR_START, 2, PROGRESSBAR_END, 16, true,
                                TEXTFIELD_COLOR_IDX );

            // print player names and load active sprite
            IO::boldFont->setColor( TEXT_COLOR_IDX2, 2 );
            IO::boldFont->printStringC( SAVE::SAV.getActiveFile( ).m_playername, PLAYERNAME_RIGHTX,
                                        PLAYERNAME_TOPY, true, IO::font::CENTER );

            IO::boldFont->setColor( TEXT_COLOR_IDX, 2 );
            IO::boldFont->printStringC(
                SAVE::SAV.getActiveFile( ).getTeamPkmn( 0 )->m_boxdata.m_name, PLAYERNAME_LEFTX,
                PLAYERNAME_BOTY, true, IO::font::CENTER );
            IO::boldFont->printStringC(
                SAVE::SAV.getActiveFile( ).getTeamPkmn( 0 )->m_boxdata.m_name, PLAYERNAME_RIGHTX,
                PLAYERNAME_BOTY, true, IO::font::CENTER );
            IO::boldFont->printStringC(
                SAVE::SAV.getActiveFile( ).getTeamPkmn( 0 )->m_boxdata.m_name, PLAYERNAME_LEFTX,
                PLAYERNAME_TOPY, true, IO::font::CENTER );
        } else {
            FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "blendersub", 512,
                                 49152, true );
            BG_PALETTE_SUB[ TEXTFIELD_COLOR_IDX ] = TEXTFIELD_COLOR;
            BG_PALETTE_SUB[ TEXT_COLOR_IDX ]      = TEXT_COLOR;
            BG_PALETTE_SUB[ TEXT_COLOR_IDX2 ]     = TEXT_COLOR2;

            // initialize progress bar
            IO::printRectangle( PROGRESSBAR_START, 2, PROGRESSBAR_END, 16, true,
                                TEXTFIELD_COLOR_IDX );

            // print player names and load active sprite
            IO::boldFont->setColor( TEXT_COLOR_IDX2, 2 );
            IO::boldFont->printStringC( SAVE::SAV.getActiveFile( ).m_playername, PLAYERNAME_RIGHTX,
                                        PLAYERNAME_TOPY, true, IO::font::CENTER );
            IO::boldFont->setColor( TEXT_COLOR_IDX, 2 );

            for( u8 i = 0; i < 3; ++i ) {
                if( BLENDER_NPC[ p_npctier ][ i ].m_name ) {
                    snprintf( NAME_CACHE[ i ], 19,
                              GET_MAP_STRING( BLENDER_NPC[ p_npctier ][ i ].m_name ) );
                }
            }
            if( BLENDER_NPC[ p_npctier ][ 0 ].m_name ) {
                IO::boldFont->printStringC( GET_MAP_STRING( BLENDER_NPC[ p_npctier ][ 0 ].m_name ),
                                            PLAYERNAME_LEFTX, PLAYERNAME_BOTY, true,
                                            IO::font::CENTER );
            } else {
                IO::Oam->oamBuffer[ SPR_ACTIVE_SUB_OAM_START + 1 ].isHidden = true;
            }
            if( BLENDER_NPC[ p_npctier ][ 1 ].m_name ) {
                IO::boldFont->printStringC( GET_MAP_STRING( BLENDER_NPC[ p_npctier ][ 1 ].m_name ),
                                            PLAYERNAME_RIGHTX, PLAYERNAME_BOTY, true,
                                            IO::font::CENTER );
            } else {
                IO::Oam->oamBuffer[ SPR_ACTIVE_SUB_OAM_START + 2 ].isHidden = true;
            }
            if( BLENDER_NPC[ p_npctier ][ 2 ].m_name ) {
                IO::boldFont->printStringC( GET_MAP_STRING( BLENDER_NPC[ p_npctier ][ 2 ].m_name ),
                                            PLAYERNAME_LEFTX, PLAYERNAME_TOPY, true,
                                            IO::font::CENTER );
            } else {
                IO::Oam->oamBuffer[ SPR_ACTIVE_SUB_OAM_START + 3 ].isHidden = true;
            }
        }

        IO::updateOAM( true );

        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
    }

    void updateLid( u16 p_position ) {
        // These are faster but produce noticeable wobble due to precision issues.
        // s16 s = IO::isin( p_position / 2 ) >> 4;
        // s16 c = IO::isin( ( 1 << 13 ) + p_position / 2 ) >> 4;

        s16 s = sinLerp( p_position / 2 ) >> 4;
        s16 c = cosLerp( p_position / 2 ) >> 4;

        IO::Oam->matrixBuffer[ 0 ].hdx = c / 2;
        IO::Oam->matrixBuffer[ 0 ].hdy = s / 2;
        IO::Oam->matrixBuffer[ 0 ].vdx = ( -s ) / 2;
        IO::Oam->matrixBuffer[ 0 ].vdy = c / 2;

        IO::updateOAM( true );
    }

    void animateThrowInBerry( u8 p_berries[ 4 ] ) {
        SpriteEntry* oam  = IO::Oam->oamBuffer;
        SpriteEntry* oamT = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 4; ++i ) {
            if( !p_berries[ i ] ) { break; }
            // load sprite
            IO::loadItemIcon( BAG::berryToItem( p_berries[ i ] ), BERRY_START_DATA[ i ].m_startX,
                              BERRY_START_DATA[ i ].m_startY, SPR_BERRY_SUB_OAM, SPR_BERRY_SUB_PAL,
                              oam[ SPR_BERRY_SUB_OAM ].gfxIndex );

            oam[ SPR_BERRY_SUB_OAM ].isSizeDouble  = true;
            oam[ SPR_BERRY_SUB_OAM ].isRotateScale = true;
            oam[ SPR_BERRY_SUB_OAM ].rotationIndex = 0;

            IO::Oam->matrixBuffer[ 0 ].vdx = ( 0LL << 8 );
            IO::Oam->matrixBuffer[ 0 ].vdy = ( 1LL << 7 );
            IO::Oam->matrixBuffer[ 0 ].hdx = ( 1LL << 7 );
            IO::Oam->matrixBuffer[ 0 ].hdy = ( 0LL << 8 );

            // move berry to center

            s16  sy      = oam[ SPR_BERRY_SUB_OAM ].y; // need s16 due to overflow
            auto targety = oam[ SPR_BERRY_SUB_OAM ].y;
            u16  angle   = 0;
            // perform 3 bounces upward, while maintaining x and y speed); results in
            // berry being at center screen
            for( u8 j = 0, bounce = 10; j < 4; ++j, --bounce ) {
                s16 upspeed = bounce;
                do {
                    targety += BERRY_START_DATA[ i ].m_speedY;
                    oam[ SPR_BERRY_SUB_OAM ].x += BERRY_START_DATA[ i ].m_speedX;
                    sy += BERRY_START_DATA[ i ].m_speedY - upspeed;
                    oam[ SPR_BERRY_SUB_OAM ].y = sy;
                    upspeed--;
                    IO::updateOAM( true );
                    swiWaitForVBlank( );
                    swiWaitForVBlank( );
                    swiWaitForVBlank( );
                    angle += -( BERRY_START_DATA[ i ].m_speedX ) * 1280;
                    updateLid( angle );
                    IO::updateOAM( true );
                } while( targety >= sy );
                SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
            }
            oam[ SPR_BERRY_SUB_OAM ].isSizeDouble  = false;
            oam[ SPR_BERRY_SUB_OAM ].isRotateScale = false;
            oam[ SPR_BERRY_SUB_OAM ].isHidden      = true;
            IO::updateOAM( true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            IO::loadItemIcon( BAG::berryToItem( p_berries[ i ] ), oamT[ SPR_BERRY_OAM + i ].x,
                              oamT[ SPR_BERRY_OAM + i ].y, SPR_BERRY_OAM + i, SPR_BERRY_PAL + i,
                              oamT[ SPR_BERRY_OAM + i ].gfxIndex, false );
            IO::updateOAM( false );
        }
    }

    void animateCloseLid( u8 p_npctier ) {
        // for now, load correct lid
        if( p_npctier == ROTOM_TIER ) {
            IO::loadUIIcon( IO::ICON::BB_LID_ROTOM_START, SPR_LID_SUB_OAM, SPR_LID_SUB_PAL,
                            IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].gfxIndex, 128 - 64, 96 - 64, 64,
                            64, false, true, true, OBJPRIORITY_3, true );
        } else {
            IO::loadUIIcon( IO::ICON::BB_LID_START, SPR_LID_SUB_OAM, SPR_LID_SUB_PAL,
                            IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].gfxIndex, 128 - 64, 96 - 64, 64,
                            64, false, true, true, OBJPRIORITY_3, true );
        }

        IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].isSizeDouble  = true;
        IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].isRotateScale = true;
        IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].rotationIndex = 0;

        IO::Oam->matrixBuffer[ 0 ].vdx = ( 0LL << 8 );
        IO::Oam->matrixBuffer[ 0 ].vdy = ( 1LL << 7 );
        IO::Oam->matrixBuffer[ 0 ].hdx = ( 1LL << 7 );
        IO::Oam->matrixBuffer[ 0 ].hdy = ( 0LL << 8 );
        IO::updateOAM( true );
    }

    u16 speedToRPM100( u16 p_speed ) {
        u32 res = p_speed;
        res *= 360 * 1000;
        return ( res >> 16 );
    }

    char* toString( int p_num ) {
        static char buffer[ 50 ];
        snprintf( buffer, 19, "%i", p_num );
        return buffer;
    }

    void scoreBoard( u8 p_miss_count[ 4 ], u8 p_hit_count[ 4 ], u8 p_perfect_count[ 4 ],
                     u16 p_mxSpeed, u8 p_npctier, u8 p_berries[ 4 ] ) {
        char buffer[ 100 ];
        // update statistics on top and rpm counter on bottom

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );

        // print names and stats

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::boldFont->setColor( IO::WHITE_IDX, 1 );
        IO::boldFont->setColor( IO::BLACK_IDX, 2 );

        constexpr u8 TOP_Y = 26;

        IO::boldFont->printStringC( GET_STRING( 804 ), 32, TOP_Y, false );

        IO::OamTop->oamBuffer[ SPR_PERFECT_OAM ].y        = TOP_Y;
        IO::OamTop->oamBuffer[ SPR_PERFECT_OAM ].x        = 128 - 6;
        IO::OamTop->oamBuffer[ SPR_PERFECT_OAM ].isHidden = false;

        IO::OamTop->oamBuffer[ SPR_GOOD_OAM ].y        = TOP_Y;
        IO::OamTop->oamBuffer[ SPR_GOOD_OAM ].x        = 160 - 6;
        IO::OamTop->oamBuffer[ SPR_GOOD_OAM ].isHidden = false;

        IO::OamTop->oamBuffer[ SPR_MISS_OAM ].y        = TOP_Y;
        IO::OamTop->oamBuffer[ SPR_MISS_OAM ].x        = 192 - 6;
        IO::OamTop->oamBuffer[ SPR_MISS_OAM ].isHidden = false;

        u8 bonus = 0;

        constexpr u8 LINE_Y   = 42;
        constexpr u8 LINE_SEP = 18;

        u8 rank[ 4 ] = { 0 };

        std::vector<std::tuple<u8, u8, s8, u8>> scores{ };
        for( u8 i = 0; i < 4; ++i ) {
            if( !p_berries[ i ] ) { break; }
            scores.push_back( { p_perfect_count[ i ], p_hit_count[ i ], -p_miss_count[ i ], i } );
        }
        std::sort( scores.rbegin( ), scores.rend( ) );
        for( u8 i = 0; i < 4; ++i ) {
            if( !p_berries[ i ] ) { break; }
            auto [ p, h, m, r ] = scores[ i ];
            rank[ r ]           = i;
        }

        if( p_npctier != ROTOM_TIER ) {
            for( u8 i = 0; i < 4; ++i ) {
                if( !i ) {
                    IO::boldFont->setColor( IO::RED_IDX, 2 );
                } else {
                    IO::boldFont->setColor( IO::BLACK_IDX, 2 );
                }

                u8 y = rank[ i ];

                IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y = LINE_Y + LINE_SEP * y - 8;
                IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].x = 0;
                if( !p_berries[ i ] ) { break; }
                if( !i ) {
                    IO::boldFont->printStringC( SAVE::SAV.getActiveFile( ).m_playername, 32,
                                                LINE_Y + LINE_SEP * y, false );
                } else {
                    IO::boldFont->printStringC( NAME_CACHE[ i - 1 ], 32, LINE_Y + LINE_SEP * y,
                                                false );
                }

                IO::boldFont->printStringC( toString( p_perfect_count[ i ] ), 128,
                                            LINE_Y + LINE_SEP * y, false, IO::font::CENTER );
                IO::boldFont->printStringC( toString( p_hit_count[ i ] ), 160,
                                            LINE_Y + LINE_SEP * y, false, IO::font::CENTER );
                IO::boldFont->printStringC( toString( p_miss_count[ i ] ), 192,
                                            LINE_Y + LINE_SEP * y, false, IO::font::CENTER );

                if( p_mxSpeed ) {
                    if( p_perfect_count[ i ] > p_miss_count[ i ] ) {
                        bonus += p_perfect_count[ i ] - p_miss_count[ i ];
                        IO::boldFont->printStringC(
                            toString( p_perfect_count[ i ] - p_miss_count[ i ] ), 224,
                            LINE_Y + LINE_SEP * y, false, IO::font::CENTER );
                    }
                }
            }
        } else {
            for( u8 i = 0; i < 4; ++i ) {
                IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y = LINE_Y + LINE_SEP * 5 / 2;
                IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].x = 64 + i * 32;
            }

            IO::boldFont->printStringC( SAVE::SAV.getActiveFile( ).m_playername, 32,
                                        LINE_Y + LINE_SEP * 0, false );
            IO::boldFont->printStringC( toString( p_perfect_count[ 0 ] ), 128,
                                        LINE_Y + LINE_SEP * 0, false, IO::font::CENTER );
            IO::boldFont->printStringC( toString( p_hit_count[ 0 ] ), 160, LINE_Y + LINE_SEP * 0,
                                        false, IO::font::CENTER );
            IO::boldFont->printStringC( toString( p_miss_count[ 0 ] ), 192, LINE_Y + LINE_SEP * 0,
                                        false, IO::font::CENTER );

            if( p_mxSpeed ) {
                if( p_perfect_count[ 0 ] > p_miss_count[ 0 ] ) {
                    bonus += p_perfect_count[ 0 ] - p_miss_count[ 0 ];
                    IO::boldFont->printStringC(
                        toString( p_perfect_count[ 0 ] - p_miss_count[ 0 ] ), 224,
                        LINE_Y + LINE_SEP * 0, false, IO::font::CENTER );
                }
            }

            IO::boldFont->printStringC(
                SAVE::SAV.getActiveFile( ).getTeamPkmn( 0 )->m_boxdata.m_name, 32,
                LINE_Y + LINE_SEP * 1, false );
            IO::boldFont->printStringC(
                toString( p_perfect_count[ 1 ] + p_perfect_count[ 2 ] + p_perfect_count[ 3 ] ), 128,
                LINE_Y + LINE_SEP * 1, false, IO::font::CENTER );
            IO::boldFont->printStringC(
                toString( p_hit_count[ 1 ] + p_hit_count[ 2 ] + p_hit_count[ 3 ] ), 160,
                LINE_Y + LINE_SEP * 1, false, IO::font::CENTER );
            IO::boldFont->printStringC(
                toString( p_miss_count[ 1 ] + p_miss_count[ 2 ] + p_miss_count[ 3 ] ), 192,
                LINE_Y + LINE_SEP * 1, false, IO::font::CENTER );

            u8 bo  = p_perfect_count[ 1 ] + p_perfect_count[ 2 ] + p_perfect_count[ 3 ];
            u8 bo2 = p_miss_count[ 1 ] + p_miss_count[ 2 ] + p_miss_count[ 3 ];

            if( p_mxSpeed ) {
                if( bo > bo2 ) {
                    bonus += bo - bo2;
                    IO::boldFont->printStringC( toString( bo - bo2 ), 224, LINE_Y + LINE_SEP * 1,
                                                false, IO::font::CENTER );
                }
            }
        }

        if( p_mxSpeed && bonus ) {
            IO::boldFont->printStringC( GET_STRING( 805 ), 224, TOP_Y, false, IO::font::CENTER );

            snprintf( buffer, 199, GET_STRING( 806 ), bonus );
            IO::boldFont->printStringC( buffer, 250, 112, false, IO::font::RIGHT );
        }
        // max RPM
        snprintf( buffer, 199, GET_STRING( 807 ), speedToRPM100( p_mxSpeed ) / 100.0 );
        IO::boldFont->printStringC( buffer, 6, 112, false );
        IO::updateOAM( false );
    }

    void updateStats( u8 p_miss_count[ 4 ], u8 p_hit_count[ 4 ], u8 p_perfect_count[ 4 ],
                      u16 p_mxSpeed, u8 p_npctier, u8 p_berries[ 4 ] ) {
        char buffer[ 100 ];
        // update statistics on top and rpm counter on bottom

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );

        // print names and stats

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );

        u8 player = 4;
        u8 bonus  = 0;
        if( p_npctier != ROTOM_TIER ) {
            for( u8 i = 0; i < 4; ++i ) {
                if( !p_berries[ i ] ) {
                    player = i;
                    break;
                }
                if( p_perfect_count[ i ] > p_miss_count[ i ] ) {
                    bonus += p_perfect_count[ i ] - p_miss_count[ i ];
                }
            }
        } else {
            if( p_perfect_count[ 0 ] > p_miss_count[ 0 ] ) {
                bonus += p_perfect_count[ 0 ] - p_miss_count[ 0 ];
            }
            u8 bo  = p_perfect_count[ 1 ] + p_perfect_count[ 2 ] + p_perfect_count[ 3 ];
            u8 bo2 = p_miss_count[ 1 ] + p_miss_count[ 2 ] + p_miss_count[ 3 ];
            if( bo > bo2 ) { bonus += bo - bo2; }
        }

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y
                = BERRY_Y - 3 * p_perfect_count[ i ] - p_hit_count[ i ] + 3 * p_miss_count[ i ];
            if( IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y < BERRY_MIN_Y ) {
                IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y = BERRY_MIN_Y;
            }
            if( IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y > BERRY_MAX_Y ) {
                IO::OamTop->oamBuffer[ SPR_BERRY_OAM + i ].y = BERRY_MAX_Y;
            }
        }
        IO::updateOAM( false );

#ifdef DESQUID_MORE
        snprintf( buffer, 199, "Berry Strength: %hhu", computeBaseLevel( p_berries ) );
        IO::regularFont->printStringC( buffer, 6, 26, false );

        if( bonus ) {
            // bonus
            snprintf( buffer, 199, "Bonus: %hhu", bonus );
            IO::regularFont->printStringC( buffer, 128, 26, false );
        }

        // max RPM
        snprintf( buffer, 199, "Max RPM: %06.2f", speedToRPM100( p_mxSpeed ) / 100.0 );
        IO::regularFont->printStringC( buffer, 6, 6, false );

        snprintf( buffer, 199, "Sheen: %hhu",
                  computeBaseSmoothness( p_berries, speedToRPM100( p_mxSpeed ) ) );
        IO::regularFont->printStringC( buffer, 128, 6, false );
#endif

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        // compute expected output
        auto result = computeBlockType( p_berries, bonus, speedToRPM100( p_mxSpeed ) );
        char sbuffer[ 60 ];
        snprintf( sbuffer, 59, GET_STRING( u8( result ) + 742 ) );
        snprintf( buffer, 199, GET_STRING( 808 ), player, sbuffer );
        IO::regularFont->printStringC( buffer, 16, 133, false );
    }

    void updateRPM( u16 p_speed ) {
        char buffer[ 100 ];
        // rpm counter on bottom:
        static u16 last_speed = 0;
        if( p_speed != last_speed ) {
            IO::printRectangle( 128 - 25, 155, 128 + 25, 190, true, 0 );
            last_speed = p_speed;
            IO::boldFont->setColor( 0, 1 );
            IO::boldFont->setColor( TEXT_COLOR_IDX, 2 );
            snprintf( buffer, 99, "%06.2f", speedToRPM100( p_speed ) / 100.0 );
            IO::boldFont->printStringC( buffer, 148, 161, true, IO::font::RIGHT );
        }
    }

    const u16 ARROW_POS[ 4 ] = {
        0xE000, // player
        0x6000, // npc1
        0xA000, // npc2
        0x2000  // npc3
    };

    constexpr bool inNPCDist( u16 p_speed, u16 p_pos1, u16 p_pos2 ) {
        if( p_pos2 < p_pos1 ) { std::swap( p_pos1, p_pos2 ); }
        return p_pos2 - p_pos1 < 6 * p_speed;
    }

    constexpr bool inGoodDist( u16 p_speed, u16 p_pos1, u16 p_pos2 ) {
        if( p_pos2 < p_pos1 ) { std::swap( p_pos1, p_pos2 ); }
        return p_pos2 - p_pos1 < 4 * p_speed;
    }

    constexpr bool inPerfectDist( u16 p_speed, u16 p_pos1, u16 p_pos2 ) {
        if( p_pos2 < p_pos1 ) { std::swap( p_pos1, p_pos2 ); }
        return p_pos2 - p_pos1 < 2 * p_speed;
    }

    u8   FREE_PERFECT = 0;
    void animatePerfectHit( u16 p_position ) {
        SOUND::playSoundEffect( SFX_SHINY );
        p_position -= ( 1 << 14 );
        IO::Oam->oamBuffer[ SPR_PERFECT_SUB_OAM + FREE_PERFECT ].isHidden = false;
        IO::Oam->oamBuffer[ SPR_PERFECT_SUB_OAM + FREE_PERFECT ].x
            = rand( ) % 4 + 126
              + ( ( ( rand( ) % 8 + 64 ) * ( IO::isin( p_position / 2 - ( 1 << 13 ) ) ) ) >> 12 );
        IO::Oam->oamBuffer[ SPR_PERFECT_SUB_OAM + FREE_PERFECT ].y
            = rand( ) % 4 + 94
              + ( ( ( rand( ) % 8 + 64 ) * ( IO::isin( p_position / 2 ) ) ) >> 12 );
        IO::updateOAM( true );

        FREE_PERFECT = ( FREE_PERFECT + 1 ) % 10;
    }

    u8   FREE_GOOD = 0;
    void animateGoodHit( u16 p_position ) {
        SOUND::playSoundEffect( SFX_SELECT );
        p_position -= ( 1 << 14 );
        IO::Oam->oamBuffer[ SPR_GOOD_SUB_OAM + FREE_GOOD ].isHidden = false;
        IO::Oam->oamBuffer[ SPR_GOOD_SUB_OAM + FREE_GOOD ].x
            = rand( ) % 4 + 126
              + ( ( ( rand( ) % 8 + 64 ) * ( IO::isin( p_position / 2 - ( 1 << 13 ) ) ) ) >> 12 );
        IO::Oam->oamBuffer[ SPR_GOOD_SUB_OAM + FREE_GOOD ].y
            = rand( ) % 4 + 94
              + ( ( ( rand( ) % 8 + 64 ) * ( IO::isin( p_position / 2 ) ) ) >> 12 );
        IO::updateOAM( true );

        FREE_GOOD = ( FREE_GOOD + 1 ) % 10;
    }

    u8   FREE_MISS = 0;
    void animateMiss( u16 p_position ) {
        SOUND::playSoundEffect( SFX_CANCEL );
        p_position -= ( 1 << 14 );
        IO::Oam->oamBuffer[ SPR_MISS_SUB_OAM + FREE_MISS ].isHidden = false;
        IO::Oam->oamBuffer[ SPR_MISS_SUB_OAM + FREE_MISS ].x
            = rand( ) % 4 + 126
              + ( ( ( rand( ) % 8 + 64 ) * ( IO::isin( p_position / 2 - ( 1 << 13 ) ) ) ) >> 12 );
        IO::Oam->oamBuffer[ SPR_MISS_SUB_OAM + FREE_MISS ].y
            = rand( ) % 4 + 94
              + ( ( ( rand( ) % 8 + 64 ) * ( IO::isin( p_position / 2 ) ) ) >> 12 );
        IO::updateOAM( true );

        FREE_MISS = ( FREE_MISS + 1 ) % 10;
    }

    void animateCountDown( ) {
        // "3"
        IO::loadUIIcon( IO::ICON::BB_THREE_START, SPR_CNT_SUB_OAM, SPR_CNT_SUB_PAL,
                        IO::Oam->oamBuffer[ SPR_CNT_SUB_OAM ].gfxIndex, 128 - 16, 96 - 16, 32, 32,
                        false, false, false, OBJPRIORITY_1, true );
        IO::updateOAM( true );
        SOUND::playSoundEffect( SFX_CHOOSE );
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        // "2"
        IO::loadUIIcon( IO::ICON::BB_TWO_START, SPR_CNT_SUB_OAM, SPR_CNT_SUB_PAL,
                        IO::Oam->oamBuffer[ SPR_CNT_SUB_OAM ].gfxIndex, 128 - 16, 96 - 16, 32, 32,
                        false, false, false, OBJPRIORITY_1, true );
        IO::updateOAM( true );
        SOUND::playSoundEffect( SFX_CHOOSE );
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        // "1"
        IO::loadUIIcon( IO::ICON::BB_ONE_START, SPR_CNT_SUB_OAM, SPR_CNT_SUB_PAL,
                        IO::Oam->oamBuffer[ SPR_CNT_SUB_OAM ].gfxIndex, 128 - 16, 96 - 16, 32, 32,
                        false, false, false, OBJPRIORITY_1, true );
        IO::updateOAM( true );
        SOUND::playSoundEffect( SFX_CHOOSE );
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        // "START"
        IO::loadUIIcon( IO::ICON::BB_START_START, SPR_CNT_SUB_OAM, SPR_CNT_SUB_PAL,
                        IO::Oam->oamBuffer[ SPR_CNT_SUB_OAM ].gfxIndex, 128 - 32, 96 - 16, 64, 32,
                        false, false, false, OBJPRIORITY_1, true );
        IO::updateOAM( true );
        SOUND::playSoundEffect( SFX_EXMARK );
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        SOUND::playBGM( BGM_CYCLING );
        IO::Oam->oamBuffer[ SPR_CNT_SUB_OAM ].isHidden = true;
        IO::updateOAM( true );
    }

    constexpr u16 FASTSPEED_THRESHOLD = 1500;
    constexpr u16 PERFECT_GAIN        = 3 * 128;
    constexpr u16 GOOD_GAIN           = 2 * 128;
    constexpr u16 PERFECT_GAIN_FAST   = 128;
    constexpr u16 GOOD_GAIN_FAST      = 0;
    constexpr u16 MISS_LOSS           = 2 * 128;
    constexpr u16 MIN_SPEED           = 128;
    constexpr u16 OVERHEAT_SPEED      = 4642;

    std::pair<u8, u16> blenderMinigame( u8 p_berries[ 4 ], u8 p_npctier ) {
        // runs the blender mini game, returns (hitbonus, max rpm * 100),
        // returns rpm = 0 if the machine overheated (when reaching > 300 rpm)
        // hitbonus is # of perfect hits - # of misses

        // center piece rotates 12 times, on exact hit: oo (once per rev)
        // ~> max hitbonus is 48

        initBlender( p_npctier );
        animateThrowInBerry( p_berries );
        animateCloseLid( p_npctier );

        animateCountDown( );

        u8 miss_count[ 4 ]    = { 0 };
        u8 hit_count[ 4 ]     = { 0 };
        u8 perfect_count[ 4 ] = { 0 };

        u16 currentPosition = 0;
        u16 currentSpeed    = 128;
        u32 progress        = 0;
        u8  lstcnt          = 0;

        u16 mxSpeed = 0;
        updateStats( miss_count, hit_count, perfect_count, mxSpeed, p_npctier, p_berries );

        bool overheat = false;

        scanKeys( );
        cooldown = COOLDOWN_COUNT;

        u8 numPlayer = 0;
        for( ; numPlayer < 4; ++numPlayer ) {
            if( !p_berries[ numPlayer ] ) { break; }
        }
        u8 adj = p_npctier == ROTOM_TIER ? 1 : numPlayer;

        u32 cooldownNPC[ 4 ] = { 0 };

        loop( ) {
            scanKeys( );
            touchRead( &touch );
            pressed = keysUp( );

            if( p_npctier == ROTOM_TIER ) {
                currentPosition += currentSpeed;
            } else {
                currentPosition -= currentSpeed;
            }
            progress += currentSpeed;

            // stop game after 12 full turns, update progress bar every half turn
            if( ( progress >> 15 ) > lstcnt ) {
                lstcnt = ( progress >> 15 );
                if( lstcnt < 24 ) {
                    drawProgressBar( lstcnt );
                    updateStats( miss_count, hit_count, perfect_count, mxSpeed, p_npctier,
                                 p_berries );
                    if( currentSpeed - 32 >= MIN_SPEED ) { currentSpeed -= 32; }
                } else {
                    break;
                }
            }

            if( inNPCDist( currentSpeed, currentPosition, currentPosition / 0x1000 * 0x1000 ) ) {
                // clean up old sprites
                for( u8 i = 0; i < 5; ++i ) {
                    IO::Oam->oamBuffer[ SPR_GOOD_SUB_OAM + ( FREE_GOOD + i + 2 ) % 10 ].isHidden
                        = true;
                    IO::Oam->oamBuffer[ SPR_PERFECT_SUB_OAM + ( FREE_PERFECT + i + 2 ) % 10 ]
                        .isHidden
                        = true;
                    IO::Oam->oamBuffer[ SPR_MISS_SUB_OAM + ( FREE_MISS + i + 2 ) % 10 ].isHidden
                        = true;
                }
                IO::updateOAM( true );
                FREE_GOOD    = ( FREE_GOOD + 1 ) % 10;
                FREE_MISS    = ( FREE_MISS + 1 ) % 10;
                FREE_PERFECT = ( FREE_PERFECT + 1 ) % 10;
            }

            updateLid( currentPosition );

            // check for button presses/ touch input
            // if close to own position
            // oo increases speed by 3 * 128 if speed is < 1500, by 128 ow
            // o increases speed by 2 * 128 is speed is < 1500, by 0 ow
            // x decreases speed by 2 * 128
            // numbers devided by number of players (if playing with a non-rotom)
            // if speed is larger than 4642, the machine overheats

            if( ( pressed & KEY_A ) || touch.px || touch.py ) {
                if( inPerfectDist( currentSpeed, currentPosition, ARROW_POS[ 0 ] ) ) {
                    // perfect hit
                    animatePerfectHit( ARROW_POS[ 0 ] );
                    perfect_count[ 0 ]++;
                    if( currentSpeed < FASTSPEED_THRESHOLD ) {
                        currentSpeed += PERFECT_GAIN / adj;
                    } else {
                        currentSpeed += PERFECT_GAIN_FAST / adj;
                    }
                } else if( inGoodDist( currentSpeed, currentPosition, ARROW_POS[ 0 ] ) ) {
                    animateGoodHit( ARROW_POS[ 0 ] );
                    hit_count[ 0 ]++;
                    if( currentSpeed < FASTSPEED_THRESHOLD ) {
                        currentSpeed += GOOD_GAIN / adj;
                    } else {
                        currentSpeed += GOOD_GAIN_FAST / adj;
                    }

                } else {
                    animateMiss( ARROW_POS[ 0 ] );
                    miss_count[ 0 ]++;
                    if( currentSpeed - MISS_LOSS / adj > MIN_SPEED ) {
                        currentSpeed -= MISS_LOSS / adj;
                    } else {
                        currentSpeed = MIN_SPEED;
                    }
                }

                scanKeys( );
                cooldown = 2;
            }

            // check if arrow is close to arrow of an NPC
            for( u8 i = 1; i < numPlayer; ++i ) {
                auto npc = BLENDER_NPC[ p_npctier ][ i - 1 ];
                u8   rnd = rand( );

                if( inNPCDist( currentSpeed, currentPosition, cooldownNPC[ i ] ) ) {
                    continue;
                } else {
                    cooldownNPC[ i ] = -1;
                }

                if( inPerfectDist( currentSpeed, currentPosition, ARROW_POS[ i ] ) ) {
                    if( npc.m_percPerfect == 255 || rnd < npc.m_percPerfect ) {
                        // perfect hit
                        animatePerfectHit( ARROW_POS[ i ] );
                        cooldownNPC[ i ] = currentPosition;
                        perfect_count[ i ]++;
                        if( currentSpeed < FASTSPEED_THRESHOLD ) {
                            currentSpeed += PERFECT_GAIN / adj;
                        } else {
                            currentSpeed += PERFECT_GAIN_FAST / adj;
                        }
                    }
                } else if( inGoodDist( currentSpeed, currentPosition, ARROW_POS[ i ] ) ) {
                    if( rnd < npc.m_percHit ) {
                        animateGoodHit( ARROW_POS[ i ] );
                        cooldownNPC[ i ] = currentPosition;
                        hit_count[ i ]++;
                        if( currentSpeed < FASTSPEED_THRESHOLD ) {
                            currentSpeed += GOOD_GAIN / adj;
                        } else {
                            currentSpeed += GOOD_GAIN_FAST / adj;
                        }
                    }
                } else if( inNPCDist( currentSpeed, currentPosition, ARROW_POS[ i ] ) ) {
                    if( rnd < npc.m_percMiss ) {
                        animateMiss( ARROW_POS[ i ] );
                        cooldownNPC[ i ] = currentPosition;
                        miss_count[ i ]++;
                        if( currentSpeed - MISS_LOSS / adj > MIN_SPEED ) {
                            currentSpeed -= MISS_LOSS / adj;
                        } else {
                            currentSpeed = MIN_SPEED;
                        }
                    }
                }
            }

            if( currentSpeed > OVERHEAT_SPEED ) {
                mxSpeed  = 0;
                overheat = true;
                break;
            }

            if( currentSpeed > mxSpeed ) { mxSpeed = currentSpeed; }
            updateRPM( currentSpeed );

            swiWaitForVBlank( );
        }

        for( u8 i = 0; i < 10; ++i ) {
            IO::Oam->oamBuffer[ SPR_GOOD_SUB_OAM + ( FREE_GOOD + i + 2 ) % 10 ].isHidden = true;
            IO::Oam->oamBuffer[ SPR_PERFECT_SUB_OAM + ( FREE_PERFECT + i + 2 ) % 10 ].isHidden
                = true;
            IO::Oam->oamBuffer[ SPR_MISS_SUB_OAM + ( FREE_MISS + i + 2 ) % 10 ].isHidden = true;
        }
        IO::updateOAM( true );

        // stop blender
        while( currentSpeed ) {
            if( p_npctier == ROTOM_TIER ) {
                currentPosition += currentSpeed;
            } else {
                currentPosition -= currentSpeed;
            }
            if( currentSpeed > 10 ) {
                currentSpeed -= 10;
            } else {
                currentSpeed = 0;
            }
            swiWaitForVBlank( );
            updateLid( currentPosition );
            updateRPM( currentSpeed );
        }
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        swiWaitForVBlank( );
        scoreBoard( miss_count, hit_count, perfect_count, mxSpeed, p_npctier, p_berries );
        swiWaitForVBlank( );
        u8 hitbonus = 0;
        if( !overheat ) {
            for( u8 i = 0; i < 4; ++i ) {
                if( perfect_count[ i ] > miss_count[ i ] ) {
                    hitbonus += perfect_count[ i ] - miss_count[ i ];
                }
            }
        }
        return { hitbonus, speedToRPM100( mxSpeed ) };
    }

    void displayResult( BAG::pokeblockType p_type, u8 p_amount, bool p_overheat ) {
        // add p_numNPC pokeblocks of type result to bag
        char buffer[ 200 ];
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );

        if( p_overheat ) {
            IO::regularFont->printStringC( GET_STRING( 800 ), 16, 133, false );
            IO::waitForInteractS( );
            IO::printRectangle( 0, 132, 255, 192, false, 0 );
        }

        IO::printRectangle( 0, 0, 255, 24, false, 0 );
        IO::regularFont->printString( GET_STRING( 803 ), 128, 5, false, IO::font::CENTER );
        char sbuffer[ 60 ];
        snprintf( sbuffer, 59, GET_STRING( u8( p_type ) + 742 ) );
        snprintf( buffer, 199, GET_STRING( 801 ), p_amount, sbuffer );
        IO::regularFont->printStringC( buffer, 16, 133, false );

        // remove lid, show resulting block
        IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].isSizeDouble  = false;
        IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].isRotateScale = false;
        IO::Oam->oamBuffer[ SPR_LID_SUB_OAM ].isHidden      = true;

        // reset the blending machine
        IO::printRectangle( PROGRESSBAR_START, 2, PROGRESSBAR_END, 16, true, TEXTFIELD_COLOR_IDX );
        IO::printRectangle( 128 - 25, 155, 128 + 25, 190, true, 0 );

        IO::updateOAM( true );

        IO::loadPokeblockIcon( p_type, 128 - 16, 96 - 16, SPR_BERRY_SUB_OAM, SPR_BERRY_SUB_PAL,
                               IO::Oam->oamBuffer[ SPR_BERRY_SUB_OAM ].gfxIndex );
        IO::updateOAM( true );
        SOUND::playSoundEffect( SFX_OBTAIN_ITEM );
        SOUND::restartBGM( );
        IO::waitForInteractS( );
    }

    void runPokeblockBlender( u8 p_numNPC, bool p_rotom, bool p_blendMaster ) {
        // when playing with npc, need to pick 1 berry, when playing with rotom, need to
        // pick 4 berries

        SOUND::dimVolume( );

        u8 npctier = p_rotom ? ROTOM_TIER : p_numNPC;
        if( p_blendMaster ) { npctier = MASTER_TIER; }
        u8 berriesFormNPC = p_numNPC;
        if( !berriesFormNPC ) { berriesFormNPC = 1; }
        if( p_rotom ) { berriesFormNPC = 0; }

        u8 requiredBerries = p_rotom ? 4 : 1;
        u8 berries[ 4 ]    = { 0 };
        u8 berriesPicked   = 0;
        // make player select required berries
        for( ; berriesPicked < requiredBerries; ++berriesPicked ) {
            BAG::bagViewer bv  = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                 BAG::bagViewer::CHOOSE_BERRY );
            u16            itm = bv.getItem( true );

            if( !itm ) {
                // player didn't pick a berry, abort
                // return berries
                for( u8 i = 0; i < requiredBerries; ++i ) {
                    SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::BERRIES,
                                                             BAG::berryToItem( berries[ i ] ), 1 );
                }

                SOUND::restoreVolume( );
                return;
            }
            berries[ berriesPicked ] = BAG::itemToBerry( itm );
        }

        for( u8 i = 0; i < berriesFormNPC && berriesPicked < 4; ++berriesPicked, ++i ) {
            // compute remaining berries, each NPC picks one berry
            u8 berry = computeNPCBerry( berries, BLENDER_NPC[ npctier ][ i ].m_berryTier );
            if( berry ) {
                berries[ berriesPicked ] = berry;
            } else {
                // default to berry no 1.
                berries[ berriesPicked ] = 1;
            }
        }

        // start rotating, allow pressing A
        SOUND::restoreVolume( );
        auto [ hitbonus, rpm ] = blenderMinigame( berries, npctier );

        // compute resulting pokeblocks

        auto result = computeBlockType( berries, hitbonus, rpm );

        if( p_rotom && result == BAG::PB_ULTIMATE ) { result = BAG::PB_GOLD_DX; }
        if( p_blendMaster && result == BAG::PB_GOLD_DX ) { result = BAG::PB_ULTIMATE; }

        displayResult( result, p_numNPC + 1, rpm == 0 );

        // add pokeblocks to bag
        SAVE::SAV.getActiveFile( ).m_pokeblockCount[ u8( result ) ] += p_numNPC + 1;
    }
} // namespace SPX
