/*
Pokémon neo
------------------------------

file        : animations.cpp
author      : Philip Wellnitz
description : Various animations.

Copyright (C) 2012 - 2023
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

#include <nds/ndstypes.h>

#include "fs/data.h"
#include "gen/pokemonNames.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace IO::ANIM {
#define PKMN_X 80
#define PKMN_Y 32

    constexpr unsigned short WHITE_PAL[ 16 ]
        = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
            0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };

    /*
     * @brief: Sets the visibility of pkmn sprites (true = hidden)
     */
    void setFrameVis( u8 p_frame, bool p_vis ) {
        for( u8 i = 0; i < 4; ++i ) { OamTop->oamBuffer[ 4 * p_frame + i ].isHidden = p_vis; }
    }

    /*
     * @brief: Moves pkmn sprites.
     */
    void moveFrame( u8 p_frame, s16 p_dx, s16 p_dy ) {
        for( u8 i = 0; i < 4; ++i ) {
            OamTop->oamBuffer[ 4 * p_frame + i ].x += p_dx;
            OamTop->oamBuffer[ 4 * p_frame + i ].y += p_dy;
        }
    }

    /*
     * @brief: Waits until the player presses either A or B.
     */
    void waitForInteract( ) {
        loop( ) {
            scanKeys( );
            swiWaitForVBlank( );
            pressed = keysUp( );

            if( pressed & KEY_A ) { break; }
            if( pressed & KEY_B ) { break; }
            swiWaitForVBlank( );
        }
    }

    /*
     * @brief: Finishes the current frame and checks whether the given buttons were
     * pressed.
     */
    bool interruptableFrame( int p_inter = 0 ) {
        swiWaitForVBlank( );
        scanKeys( );
        pressed = keysUp( );

        if( pressed & p_inter ) { return true; }
        return false;
    }

    bool evolvePkmn( u16 p_startSpecies, u8 p_startForme, u16 p_endSpecies, u8 p_endForme,
                     bool p_shiny, bool p_female, u32 p_pid, bool p_allowAbort ) {

        // TODO: check for moves learnt on level-up

        vramSetup( );
        swiWaitForVBlank( );
        clearScreen( true, true, true );
        resetScale( true, true );
        bgUpdate( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "trade_lower", 480, 49152,
                             true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "trade_upper", 480, 49152,
                             false );

        //        initColors( );
        regularFont->setColor( 0, 0 );
        regularFont->setColor( BLACK_IDX, 1 );
        regularFont->setColor( GRAY_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ]  = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        initOAMTable( false );
        initOAMTable( true );

        // Load both sprites

        u16          tileCnt         = 0;
        constexpr u8 mbox_pal        = 3;
        constexpr u8 mbox_oam_start  = 8;
        constexpr u8 mbox_oam_length = 13;
        tileCnt = IO::loadSprite( "UI/mbox1", mbox_oam_start, mbox_pal, tileCnt, 2, 192 - 46, 32,
                                  64, false, false, false, OBJPRIORITY_3, false );

        for( u8 i = 0; i < mbox_oam_length; ++i ) {
            IO::loadSprite( mbox_oam_start + mbox_oam_length - i, mbox_pal, 0, 30 + 16 * i,
                            192 - 46, 32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_3, false );
        }

        pkmnSpriteInfo pinfoS = { p_startSpecies, p_startForme, p_female, p_shiny, false, p_pid };
        pkmnSpriteInfo pinfoE = { p_endSpecies, p_endForme, p_female, p_shiny, false, p_pid };

        u8  SPR_CIRC_OAM = 60;
        u8  SPR_CIRC_PAL = 15;
        u16 SPR_CIRC_GFX = tileCnt;

        tileCnt = IO::loadSprite( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 14, 32, 32,
                                  false, false, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 1, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 14, 32, 32, 0, 0, 0,
                        false, true, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 2, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 74, 32, 32, 0, 0, 0, true,
                        false, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 3, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 74, 32, 32, 0, 0, 0,
                        true, true, false, OBJPRIORITY_1, false );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble  = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].rotationIndex = i;

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

        tileCnt = loadPKMNSprite( pinfoS, PKMN_X, PKMN_Y, 0, 0, tileCnt, false );
        loadPKMNSprite( pinfoE, PKMN_X, PKMN_Y, 4, 1, tileCnt, false );
        setFrameVis( 1, true );
        updateOAM( false );

        char buffer[ 200 ];
        snprintf( buffer, 200, GET_STRING( STR_ANIM_EVOLUTION_START ),
                  FS::getDisplayName( p_startSpecies ).c_str( ) );
        IO::regularFont->printStringC( buffer, 12, 192 - 40, false );

        SOUND::playBGM( BGM_EVOLVING );
        // Main Animation
        for( u8 i = 0; i < 50; ++i ) { swiWaitForVBlank( ); }

        SOUND::playCry( p_startSpecies );
        for( u8 i = 0; i < 50; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = false;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble  = false;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden      = true;
        }

        copySpritePal( WHITE_PAL, 0, false );
        copySpritePal( WHITE_PAL, 1, false );
        updateOAM( false );

        u8   slowfactor = 7;
        bool abort      = false;
        for( u8 i = 4; i < 7; ++i ) {
            for( u8 k = 0; k < 3; ++k ) {
                setFrameVis( 0, true );
                setFrameVis( 1, false );
                updateOAM( false );
                for( u8 j = 0; j < slowfactor * i; ++j ) {
                    abort |= interruptableFrame( p_allowAbort ? KEY_B : 0 );
                }
                if( abort ) { break; }
                setFrameVis( 0, false );
                setFrameVis( 1, true );
                updateOAM( false );
                for( u8 j = slowfactor * i; j < slowfactor * 8; ++j ) {
                    abort |= interruptableFrame( p_allowAbort ? KEY_B : 0 );
                }
                if( abort ) { break; }
            }
        }

        setFrameVis( 0, true );
        setFrameVis( 1, false );
        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden      = false;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble  = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].rotationIndex = i;
        }
        loadPKMNSprite( pinfoE, PKMN_X, PKMN_Y, 4, 1, tileCnt, false );
        updateOAM( false );

        if( abort ) {
            SOUND::stopBGM( );
            setFrameVis( 0, false );
            setFrameVis( 1, true );
            updateOAM( false );
            SOUND::playCry( p_startSpecies );
            for( u8 i = 0; i < 50; ++i ) { swiWaitForVBlank( ); }
            IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
            IO::regularFont->printStringC( GET_STRING( STR_ANIM_EVOLUTION_ABORTED ), 12, 192 - 40,
                                           false );
            waitForInteract( );
            SOUND::restartBGM( );
            return false;
        } else {
            SOUND::playBGMOneshot( BGM_OS_EVOLVED );
            setFrameVis( 0, true );
            setFrameVis( 1, false );
            updateOAM( false );
            SOUND::playCry( p_endSpecies );
            for( u8 i = 0; i < 50; ++i ) { swiWaitForVBlank( ); }
            IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
            snprintf( buffer, 200, GET_STRING( STR_ANIM_EVOLUTION_COMPLETE ),
                      FS::getDisplayName( p_startSpecies ).c_str( ),
                      FS::getDisplayName( p_endSpecies ).c_str( ) );
            IO::regularFont->printStringC( buffer, 12, 192 - 40, false );
            waitForInteract( );
            SOUND::restartBGM( );
            SAVE::SAV.getActiveFile( ).registerCaughtPkmn( p_endSpecies );
            return true;
        }
    }

    void tradePkmn( const boxPokemon& p_ownToOther, const boxPokemon& p_otherToMe,
                    const char* p_otherName ) {
        char buffer[ 200 ];
        vramSetup( );
        swiWaitForVBlank( );
        clearScreen( true, true, true );
        resetScale( true, true );

        bgUpdate( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "trade_lower", 480, 49152,
                             true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "trade_upper", 480, 49152,
                             false );
        regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ]  = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        initOAMTable( false );
        initOAMTable( true );

        // Show own pkmn on screen
        // Sending <pkmn> to <p_otherName>.
        // "Say good bye to <pkmn>"
        // pkmn enters ball
        // ball swirls to top
        // own and other pkmn cross
        // other pkmn comes down
        // ball opens, shows other pkmn
        // "<p_otherName> sent <other pkmn>"
        // "take good care of <other pkmn>

        u16 tileCnt = 0;

        // message box
        constexpr u8 mbox_pal        = 3;
        constexpr u8 mbox_oam_start  = 8;
        constexpr u8 mbox_oam_length = 13;
        constexpr u8 ownpb_pal       = 4;
        constexpr u8 ownpb_oam_start = 30;
        tileCnt = IO::loadSprite( "UI/mbox1", mbox_oam_start, mbox_pal, 0, 2, 192 - 46, 32, 64,
                                  false, false, false, OBJPRIORITY_3, false );

        for( u8 i = 0; i < mbox_oam_length; ++i ) {
            IO::loadSprite( mbox_oam_start + mbox_oam_length - i, mbox_pal, 0, 30 + 16 * i,
                            192 - 46, 32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_3, false );
        }

        pkmnSpriteInfo pinfoS = { p_ownToOther.getSpecies( ),
                                  p_ownToOther.getForme( ),
                                  p_ownToOther.isFemale( ),
                                  p_ownToOther.isShiny( ),
                                  false,
                                  p_ownToOther.m_pid };
        pkmnSpriteInfo pinfoE = { p_otherToMe.getSpecies( ),
                                  p_otherToMe.getForme( ),
                                  p_otherToMe.isFemale( ),
                                  p_otherToMe.isShiny( ),
                                  false,
                                  p_otherToMe.m_pid };

        for( u8 i = 0; i <= 10; ++i ) {
            snprintf( buffer, 49, "PB/%hhu/%hhu_%hhu", p_ownToOther.m_ball, p_ownToOther.m_ball,
                      i );
            tileCnt
                = IO::loadSprite( buffer, ownpb_oam_start + i, ownpb_pal + i, tileCnt, PKMN_X + 40,
                                  PKMN_Y + 40, 32, 32, false, false, false, OBJPRIORITY_0, false );
            OamTop->oamBuffer[ ownpb_oam_start + i ].isHidden = true;
        }

        u8  SPR_CIRC_OAM = 60;
        u8  SPR_CIRC_PAL = 15;
        u16 SPR_CIRC_GFX = tileCnt;

        tileCnt = IO::loadSprite( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 14, 32, 32,
                                  false, false, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 1, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 14, 32, 32, 0, 0, 0,
                        false, true, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 2, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 74, 32, 32, 0, 0, 0, true,
                        false, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 3, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 74, 32, 32, 0, 0, 0,
                        true, true, false, OBJPRIORITY_1, false );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble  = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].rotationIndex = i;

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

        tileCnt = loadPKMNSprite( pinfoS, PKMN_X, PKMN_Y, 0, 0, tileCnt, false );
        updateOAM( false );

        snprintf( buffer, 199, GET_STRING( STR_ANIM_TRADE_SEND1 ), p_ownToOther.m_name,
                  p_otherName );
        IO::regularFont->printStringC( buffer, 12, 192 - 40, false );

        SOUND::playBGM( BGM_EVOLVING );
        for( u8 i = 0; i < 180; ++i ) { swiWaitForVBlank( ); }

        SOUND::playCry( p_ownToOther.getSpecies( ) );
        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        snprintf( buffer, 199, GET_STRING( STR_ANIM_TRADE_SEND2 ), p_ownToOther.m_name );
        IO::regularFont->printStringC( buffer, 12, 192 - 40, false );
        for( u8 i = 0; i < 50; ++i ) { swiWaitForVBlank( ); }

        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        for( u8 i = 0; i <= mbox_oam_length; ++i ) {
            OamTop->oamBuffer[ mbox_oam_start + i ].isHidden = true;
        }
        setFrameVis( 0, true );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = false;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble  = false;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden      = true;
        }
        OamTop->oamBuffer[ ownpb_oam_start + 10 ].isHidden = false;
        updateOAM( false );
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        OamTop->oamBuffer[ ownpb_oam_start + 10 ].isHidden = true;
        OamTop->oamBuffer[ ownpb_oam_start + 0 ].isHidden  = false;
        updateOAM( false );
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }

        for( u8 j = 0; j < PKMN_Y + 40 + 32; j += 2 ) {
            for( u8 i = 0; i <= 10; ++i ) {
                OamTop->oamBuffer[ ownpb_oam_start + i ].isHidden = true;
                OamTop->oamBuffer[ ownpb_oam_start + i ].y -= 2;

                OamTop->oamBuffer[ ownpb_oam_start + i ].x
                    += ( 1 - 2 * ( ( ( j / 2 ) % 16 ) >= 3 && ( ( j / 2 ) % 16 ) < 11 ) ) * 4;
            }
            OamTop->oamBuffer[ ownpb_oam_start + ( ( j / 2 ) % 8 ) ].isHidden = false;
            updateOAM( false );
            for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }
        }
        for( u8 i = 0; i <= 10; ++i ) { OamTop->oamBuffer[ ownpb_oam_start + i ].isHidden = true; }

        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "trade_center", 480, 49152,
                             false );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "trade_center", 480, 49152,
                             true );

        // pkmn cross

        pinfoS.m_flipX = true;
        tileCnt
            = loadPKMNSprite( pinfoS, 16, 192 - 48, 0, 0, OamTop->oamBuffer[ 0 ].gfxIndex, false );
        tileCnt = loadPKMNSprite( pinfoE, 128 + 16, -48, 4, 1, tileCnt, false );
        updateOAM( false );

        for( u8 j = 0; j < 96; j += 2 ) {
            moveFrame( 0, 0, -2 );
            moveFrame( 1, 0, 2 );
            updateOAM( false );
            for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
        }
        for( u8 i = 0; i < 15; ++i ) { swiWaitForVBlank( ); }
        for( u8 j = 0; j < 96; j += 2 ) {
            moveFrame( 0, 0, -2 );
            moveFrame( 1, 0, 2 );
            updateOAM( false );
            for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
        }
        setFrameVis( 0, true );
        setFrameVis( 1, true );
        updateOAM( false );

        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "trade_lower", 480, 49152,
                             true );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "trade_upper", 480, 49152,
                             false );

        // other pkmn moves down

        tileCnt = loadPKMNSprite( pinfoE, PKMN_X, PKMN_Y, 0, 0, OamTop->oamBuffer[ 0 ].gfxIndex,
                                  false );
        setFrameVis( 0, true );
        updateOAM( false );

        for( u8 i = 0; i <= 10; ++i ) {
            snprintf( buffer, 49, "PB/%hhu/%hhu_%hhu", p_otherToMe.m_ball, p_otherToMe.m_ball, i );
            tileCnt
                = IO::loadSprite( buffer, ownpb_oam_start + i, ownpb_pal + i, tileCnt, PKMN_X + 40,
                                  -32, 32, 32, false, false, false, OBJPRIORITY_0, false );
            OamTop->oamBuffer[ ownpb_oam_start + i ].isHidden = true;
        }

        for( u8 j = 0; j < PKMN_Y + 40 + 32; j += 2 ) {
            for( u8 i = 0; i <= 10; ++i ) {
                OamTop->oamBuffer[ ownpb_oam_start + i ].isHidden = true;
                OamTop->oamBuffer[ ownpb_oam_start + i ].y += 2;

                OamTop->oamBuffer[ ownpb_oam_start + i ].x
                    += ( 1 - 2 * ( ( ( j / 2 ) % 16 ) >= 3 && ( ( j / 2 ) % 16 ) < 11 ) ) * 4;
            }
            OamTop->oamBuffer[ ownpb_oam_start + ( ( j / 2 ) % 8 ) ].isHidden = false;
            updateOAM( false );
            for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }
        }
        for( u8 i = 0; i <= 10; ++i ) { OamTop->oamBuffer[ ownpb_oam_start + i ].isHidden = true; }

        OamTop->oamBuffer[ ownpb_oam_start + 10 ].isHidden = false;
        updateOAM( false );
        for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }

        SOUND::playBGMOneshot( BGM_OS_EVOLVED );
        OamTop->oamBuffer[ ownpb_oam_start + 10 ].isHidden = true;
        setFrameVis( 0, false );
        for( u8 i = 0; i <= mbox_oam_length; ++i ) {
            OamTop->oamBuffer[ mbox_oam_start + i ].isHidden = false;
        }
        tileCnt = IO::loadSprite( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 14, 32, 32,
                                  false, false, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 1, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 14, 32, 32, 0, 0, 0,
                        false, true, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 2, SPR_CIRC_PAL, SPR_CIRC_GFX, 66, 74, 32, 32, 0, 0, 0, true,
                        false, false, OBJPRIORITY_1, false );
        IO::loadSprite( SPR_CIRC_OAM + 3, SPR_CIRC_PAL, SPR_CIRC_GFX, 126, 74, 32, 32, 0, 0, 0,
                        true, true, false, OBJPRIORITY_1, false );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isSizeDouble  = true;
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].rotationIndex = i;

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

        updateOAM( false );

        SOUND::playCry( p_otherToMe.getSpecies( ) );

        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        snprintf( buffer, 199, GET_STRING( STR_ANIM_TRADE_RECV1 ), p_otherName,
                  p_otherToMe.m_name );
        IO::regularFont->printStringC( buffer, 12, 192 - 40, false );
        for( u8 i = 0; i < 150; ++i ) { swiWaitForVBlank( ); }

        IO::printRectangle( 0, 192 - 42, 255, 192, false, 0 );
        snprintf( buffer, 199, GET_STRING( STR_ANIM_TRADE_RECV2 ), p_otherToMe.m_name );
        IO::regularFont->printStringC( buffer, 12, 192 - 40, false );
        for( u8 i = 0; i < 150; ++i ) { swiWaitForVBlank( ); }
        SOUND::restartBGM( );
        SAVE::SAV.getActiveFile( ).registerCaughtPkmn( p_otherToMe.getSpecies( ) );
    }

    void hatchEgg( const pkmnSpriteInfo& p_pkmn ) {
        vramSetup( );
        swiWaitForVBlank( );
        clearScreen( true, true, true );
        resetScale( true, true );

        bgUpdate( );
        regularFont->setColor( 0, 0 );
        regularFont->setColor( WHITE_IDX, 1 );
        regularFont->setColor( GRAY_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ]  = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        initOAMTable( false );
        initOAMTable( true );
        // Load both sprites

        fadeScreen( IO::fadeType::CLEAR_DARK, true, true );

        u16 tileCnt = 0;
        tileCnt     = loadEggSprite( PKMN_X, PKMN_Y, 0, 0, tileCnt, false,
                                     p_pkmn.m_pkmnIdx == PKMN_MANAPHY );

        tileCnt = loadPKMNSprite( p_pkmn, PKMN_X, PKMN_Y, 4, 1, tileCnt, false );
        SOUND::playBGM( BGM_EVOLVING );

        setFrameVis( 1, true );
        updateOAM( false );
        IO::fadeScreen( IO::fadeType::UNFADE, true, true );
        u8 slowfactor = 7;

        for( u8 j = 2; j < 6; ++j ) {
            for( u8 i = 0; i < 10; ++i ) { swiWaitForVBlank( ); }
            moveFrame( 0, 2 * j, 0 );
            updateOAM( false );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            moveFrame( 0, -4 * j, 0 );
            updateOAM( false );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            moveFrame( 0, 2 * j, 0 );
            updateOAM( false );
            for( u8 i = 0; i < slowfactor * j; ++i ) { swiWaitForVBlank( ); }
        }

        fadeScreen( IO::fadeType::CLEAR_DARK, true, true );
        setFrameVis( 0, true );
        setFrameVis( 1, false );
        updateOAM( false );
        IO::fadeScreen( IO::fadeType::UNFADE, true, true );

        SOUND::playBGMOneshot( BGM_OS_EVOLVED );
        char buffer[ 200 ];
        clearScreen( true, true, true );
        snprintf( buffer, 200, GET_STRING( STR_ANIM_EGG_HATCH ),
                  FS::getDisplayName( p_pkmn.m_pkmnIdx ).c_str( ) );
        regularFont->printStringC( buffer, 127, 136, false, font::CENTER );
        setFrameVis( 0, true );
        setFrameVis( 1, false );
        updateOAM( false );
        for( u8 i = 0; i < 50; ++i ) { swiWaitForVBlank( ); }
        waitForInteract( );
        SOUND::restartBGM( );
        SAVE::SAV.getActiveFile( ).registerCaughtPkmn( p_pkmn.m_pkmnIdx );
    }

    void openingAnimation( ) {
        // TODO
    }
} // namespace IO::ANIM
