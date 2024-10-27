/*
Pokémon neo
------------------------------

file        : saveOptions.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2021 - 2022
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

#include "defines.h"
#include "fs/data.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "save/saveGame.h"
#include "save/saveOptions.h"
#include "sound/sound.h"

namespace SAVE {
#define SPR_CHOICE_START_OAM_SUB( p_pos ) ( 0 + 10 * ( p_pos ) )
#define SPR_X_OAM_SUB                     60

#define SPR_BOX_PAL_SUB     7
#define SPR_BOX_SEL_PAL_SUB 8
#define SPR_X_PAL_SUB       9

    constexpr u8 MAX_SETTINGS = 6;

    void drawSub( ) {
        FS::readPictureData(
            bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/NAV/",
            std::to_string( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx ).c_str( ), 192 * 2,
            192 * 256, false );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        SpriteEntry* oam = IO::Oam->oamBuffer;
        // Print options
        //
        // Textspeed
        // Language
        // Enable BGN [if compiled w/ sound]
        // Enable SFX [if compiled w/ sound]
        // level modifier / encounter rate modifier
        // Nav Background

        // Options string
        IO::regularFont->printString( GET_STRING( 418 ), 4, 4, true );

        for( u8 i = 0; i < MAX_SETTINGS; ++i ) {
            IO::regularFont->printStringC( GET_STRING( 419 + i ),
                                           oam[ SPR_CHOICE_START_OAM_SUB( i ) ].x + 6,
                                           oam[ SPR_CHOICE_START_OAM_SUB( i ) ].y + 2, true );
        }

        // Text speed
        if( SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) == 0 ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 425 ),
                                    oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 6 + 86,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y, true, IO::font::LEFT );
        if( SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) == 1 ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 426 ), oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 158,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y, true,
                                    IO::font::CENTER );
        if( SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) == 2 ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 427 ), 256 - 12 - 10,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].y, true, IO::font::RIGHT );

        // language
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::boldFont->printStringC( "<", oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].x + 6 + 86,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y, true, IO::font::LEFT );
        IO::boldFont->printStringC( ">", 256 - 12 - 10, oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y,
                                    true, IO::font::RIGHT );
        IO::boldFont->setColor( IO::BLUE_IDX, 2 );
        IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        IO::boldFont->printStringC(
            FS::LANGUAGE_NAMES[ CURRENT_LANGUAGE ], oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 158,
            oam[ SPR_CHOICE_START_OAM_SUB( 1 ) ].y, true, IO::font::CENTER );

        // bgm
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 428 ),
                                    oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].x + 6 + 86,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y, true, IO::font::LEFT );
        if( !SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 429 ), 256 - 12 - 10,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 2 ) ].y, true, IO::font::RIGHT );
#else
        IO::boldFont->setColor( IO::BLUE_IDX, 2 );
        IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        IO::boldFont->printStringC( GET_STRING( 431 ), oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 158,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y, true,
                                    IO::font::CENTER );

#endif

        // sfx
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 428 ),
                                    oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].x + 6 + 86,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y, true, IO::font::LEFT );
        if( !SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 429 ), 256 - 12 - 10,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y, true, IO::font::RIGHT );
#else
        IO::boldFont->setColor( IO::BLUE_IDX, 2 );
        IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        IO::boldFont->printStringC( GET_STRING( 431 ), oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].x + 158,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 3 ) ].y, true,
                                    IO::font::CENTER );

#endif

        // Diff
        if( SAVE::SAV.getActiveFile( ).m_options.m_difficulty < 3 ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 430 ),
                                    oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].x + 6 + 86,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].y, true, IO::font::LEFT );
        if( SAVE::SAV.getActiveFile( ).m_options.m_difficulty == 3 ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 431 ), oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].x + 158,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].y, true,
                                    IO::font::CENTER );
        if( SAVE::SAV.getActiveFile( ).m_options.m_difficulty > 3 ) {
            IO::boldFont->setColor( IO::BLUE_IDX, 2 );
            IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        } else {
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        }
        IO::boldFont->printStringC( GET_STRING( 432 ), 256 - 12 - 10,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 4 ) ].y, true, IO::font::RIGHT );

        // BG
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::boldFont->printStringC( "<", oam[ SPR_CHOICE_START_OAM_SUB( 5 ) ].x + 6 + 86,
                                    oam[ SPR_CHOICE_START_OAM_SUB( 5 ) ].y, true, IO::font::LEFT );
        IO::boldFont->printStringC( ">", 256 - 12 - 10, oam[ SPR_CHOICE_START_OAM_SUB( 5 ) ].y,
                                    true, IO::font::RIGHT );

        IO::boldFont->setColor( IO::BLUE_IDX, 2 );
        IO::boldFont->setColor( IO::BLUE2_IDX, 1 );
        IO::boldFont->printStringC(
            std::to_string( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx ).c_str( ),
            oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ].x + 158, oam[ SPR_CHOICE_START_OAM_SUB( 5 ) ].y,
            true, IO::font::CENTER );
    }

    void init( ) {
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

        FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/", "Border", 64, 192, 192 * 256,
                             false );

        for( u8 i = 0; i < 2; ++i ) {
            u16* pal             = IO::BG_PAL( i );
            pal[ IO::WHITE_IDX ] = IO::WHITE;
            pal[ IO::GRAY_IDX ]  = IO::GRAY;
            pal[ IO::BLACK_IDX ] = IO::BLACK;
            pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
            pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
            pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
            pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

            pal[ 240 ] = IO::RGB( 6, 6, 6 );    // hp bar border color
            pal[ 241 ] = IO::RGB( 12, 30, 12 ); // hp bar green 1
            pal[ 242 ] = IO::RGB( 3, 23, 4 );   // hp bar green 2
            pal[ 243 ] = IO::RGB( 30, 30, 12 ); // hp bar yellow 1
            pal[ 244 ] = IO::RGB( 23, 23, 5 );  // hp bar yellow 2
            pal[ 245 ] = IO::RGB( 30, 15, 12 ); // hp bar red 1
            pal[ 246 ] = IO::RGB( 20, 7, 7 );   // hp bar red 2
        }

        u16 tileCnt = 0;

        // x
        tileCnt = IO::loadUIIcon( IO::ICON::X_16_16_START, SPR_X_OAM_SUB, SPR_X_PAL_SUB, tileCnt,
                                  236, 172, 16, 16, false, false, false, OBJPRIORITY_1, true,
                                  OBJMODE_NORMAL );

        // Choice boxes
        for( u8 i = 0; i < MAX_SETTINGS; ++i ) {
            IO::loadSprite( SPR_CHOICE_START_OAM_SUB( i ), SPR_BOX_PAL_SUB, tileCnt, 12,
                            24 + 23 * i, 32, 32, 0, 0, 0, false, false, false, OBJPRIORITY_3, true,
                            OBJMODE_BLENDED );
            for( u8 j = 0; j < 8; ++j ) {
                IO::loadSprite( SPR_CHOICE_START_OAM_SUB( i ) + 8 - j, SPR_BOX_PAL_SUB, tileCnt,
                                12 + 32 + 24 * j, 24 + 23 * i - 12, 32, 32, 0, 0, 0, true, true,
                                false, OBJPRIORITY_3, true, OBJMODE_BLENDED );
            }
        }
        tileCnt = IO::loadUIIcon( IO::ICON::NOSELECTION_64_20_START, SPR_CHOICE_START_OAM_SUB( 0 ),
                                  SPR_BOX_PAL_SUB, tileCnt, 12, 24, 32, 32, false, false, false,
                                  OBJPRIORITY_3, true, OBJMODE_BLENDED );

        IO::copySpritePal( IO::SELECTED_SPR_PAL, SPR_BOX_SEL_PAL_SUB, 0, 2 * 8, true );
        IO::updateOAM( true );

        drawSub( );
        REG_BLDCNT       = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA     = TRANSPARENCY_COEFF;
        REG_BLDCNT_SUB   = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BG3;
        REG_BLDALPHA_SUB = TRANSPARENCY_COEFF;
        bgUpdate( );
    }

    void select( u8 p_option ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        for( u8 i = 0; i < MAX_SETTINGS; ++i ) {
            for( u8 j = 0; j <= 8; ++j ) {
                oam[ SPR_CHOICE_START_OAM_SUB( i ) + 8 - j ].palette = SPR_BOX_PAL_SUB;
            }
        }
        for( u8 j = 0; j <= 8; ++j ) {
            oam[ SPR_CHOICE_START_OAM_SUB( p_option ) + 8 - j ].palette = SPR_BOX_SEL_PAL_SUB;
        }
        IO::updateOAM( true );
    }

    void increaseSetting( u8 p_option ) {
        switch( p_option ) {
        case 0: // Text Speed
            if( SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) == 2 ) { return; }
            SAVE::SAV.getActiveFile( ).m_options.setTextSpeed(
                SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) + 1 );
            break;
        case 1: // Language
            SAVE::SAV.getActiveFile( ).m_options.m_language
                = language( ( SAVE::SAV.getActiveFile( ).m_options.m_language + 1 ) % LANGUAGES );
            break;
#ifndef NO_SOUND
        case 2: // BGM
            if( !SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
                return;
            } else {
                SAVE::SAV.getActiveFile( ).m_options.m_enableBGM = false;
                SOUND::restartBGM( );
            }
            break;
        case 3: // SFX
            if( !SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
                return;
            } else {
                SAVE::SAV.getActiveFile( ).m_options.m_enableSFX = false;
            }
            break;
#endif
        case 4: // Difficulty
            if( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) == 6 ) { return; }
            SAVE::SAV.getActiveFile( ).m_options.setDifficulty(
                SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) + 3 );
            break;
        case 5: // BG
            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx
                = ( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx + 1 ) % NUM_BGS;
        default: break;
        }
        drawSub( );
    }

    void decreaseSetting( u8 p_option ) {
        switch( p_option ) {
        case 0: // Text Speed
            if( SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) == 0 ) { return; }
            SAVE::SAV.getActiveFile( ).m_options.setTextSpeed(
                SAVE::SAV.getActiveFile( ).m_options.getTextSpeed( ) - 1 );
            break;
        case 1: // Language
            SAVE::SAV.getActiveFile( ).m_options.m_language = language(
                ( SAVE::SAV.getActiveFile( ).m_options.m_language + LANGUAGES - 1 ) % LANGUAGES );
            break;
#ifndef NO_SOUND
        case 2: // BGM
            if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
                return;
            } else {
                SAVE::SAV.getActiveFile( ).m_options.m_enableBGM = true;
                SOUND::restartBGM( );
                SOUND::dimVolume( );
            }
            break;
        case 3: // SFX
            if( SAVE::SAV.getActiveFile( ).m_options.m_enableSFX ) {
                return;
            } else {
                SAVE::SAV.getActiveFile( ).m_options.m_enableSFX = true;
            }
            break;
#endif
        case 4: // Difficulty
            if( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) == 0 ) { return; }
            SAVE::SAV.getActiveFile( ).m_options.setDifficulty(
                SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) - 3 );
            break;
        case 5: // BG
            SAVE::SAV.getActiveFile( ).m_options.m_bgIdx
                = ( SAVE::SAV.getActiveFile( ).m_options.m_bgIdx + NUM_BGS - 1 ) % NUM_BGS;
        default: break;
        }
        drawSub( );
    }

    void runSettings( ) {
        init( );
        u8 currentSelection = 0;
        select( currentSelection );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                return;
            }

            if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( currentSelection = ( currentSelection + 1 ) % MAX_SETTINGS );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( currentSelection = ( currentSelection + MAX_SETTINGS - 1 ) % MAX_SETTINGS );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                decreaseSetting( currentSelection );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                increaseSetting( currentSelection );
                cooldown = COOLDOWN_COUNT;
            }

            swiWaitForVBlank( );
        }
    }
} // namespace SAVE
