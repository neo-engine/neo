/*
    Pokémon neo
    ------------------------------

    file        : animations.cpp
    author      : Philip Wellnitz
    description : Various animations.

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

#include <nds/ndstypes.h>
#include "fs.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"
#include "sound.h"

namespace IO::ANIM {
#define PKMN_X 80
#define PKMN_Y 24

    /*
     * @brief: Sets the visibility of pkmn sprites (true = hidden)
     */
    void setFrameVis( u8 p_frame, bool p_vis ) {
        for( u8 i = 0; i < 4; ++i ) { OamTop->oamBuffer[ 4 * p_frame + i ].isHidden = p_vis; }
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
                     bool p_shiny, bool p_female, bool p_allowAbort ) {
        vramSetup( );
        swiWaitForVBlank( );
        clearScreen( true, true, true );
        resetScale( true, true );

        bgUpdate( );
        //        initColors( );
        regularFont->setColor( 0, 0 );
        regularFont->setColor( WHITE_IDX, 1 );
        regularFont->setColor( GRAY_IDX, 2 );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ]  = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        char buffer[ 200 ];
        snprintf( buffer, 200, GET_STRING( 51 ), getDisplayName( p_startSpecies ).c_str( ) );
        regularFont->printStringC( buffer, 127, 136, false, font::CENTER );

        initOAMTable( false );
        initOAMTable( true );

        // Load both sprites

        u16 tileCnt = 0;
        tileCnt     = loadPKMNSprite( p_startSpecies, PKMN_X, PKMN_Y, 0, 0, tileCnt, false, p_shiny,
                                  p_female, false, false, p_startForme );
        tileCnt     = loadPKMNSprite( p_endSpecies, PKMN_X, PKMN_Y, 4, 1, tileCnt, false, p_shiny,
                                  p_female, false, false, p_endForme );
        setFrameVis( 1, true );
        updateOAM( false );

        // Main Animation
        waitForInteract( );
        SOUND::playBGM( MOD_EVOLVING );

        u8   slowfactor = 7;
        bool abort      = false;
        for( u8 i = 4; i < 7; ++i ) {
            for( u8 k = 0; k < 2; ++k ) {
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
        if( abort ) {
            clearScreen( true, true, true );
            regularFont->printStringC( GET_STRING( 388 ), 127, 136, false, font::CENTER );
            setFrameVis( 0, false );
            setFrameVis( 1, true );
            updateOAM( false );
            waitForInteract( );
            return false;
        } else {
            clearScreen( true, true, true );
            snprintf( buffer, 200, GET_STRING( 52 ), getDisplayName( p_startSpecies ).c_str( ),
                      getDisplayName( p_endSpecies ).c_str( ) );
            regularFont->printStringC( buffer, 127, 136, false, font::CENTER );
            setFrameVis( 0, true );
            setFrameVis( 1, false );
            updateOAM( false );
            waitForInteract( );
            return true;
        }
    }

    void hatchEgg( u16 p_endSpecies, u8 p_endForme, bool p_shiny, bool p_female ) {
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

        u16 tileCnt = 0;
        tileCnt
            = loadEggSprite( PKMN_X, PKMN_Y, 0, 0, tileCnt, false, p_endSpecies == PKMN_MANAPHY );
        tileCnt = loadPKMNSprite( p_endSpecies, PKMN_X, PKMN_Y, 4, 1, tileCnt, false, p_shiny,
                                  p_female, false, false, p_endForme );
        setFrameVis( 1, true );
        updateOAM( false );

        u8 slowfactor = 7;
        for( u8 i = 4; i < 7; ++i ) {
            for( u8 k = 0; k < 2; ++k ) {
                setFrameVis( 0, true );
                setFrameVis( 1, false );
                updateOAM( false );
                for( u8 j = 0; j < slowfactor * i; ++j ) { swiWaitForVBlank( ); }
                setFrameVis( 0, false );
                setFrameVis( 1, true );
                updateOAM( false );
                for( u8 j = slowfactor * i; j < slowfactor * 8; ++j ) { swiWaitForVBlank( ); }
            }
        }
        char buffer[ 200 ];
        clearScreen( true, true, true );
        snprintf( buffer, 200, GET_STRING( 389 ), getDisplayName( p_endSpecies ).c_str( ) );
        regularFont->printStringC( buffer, 127, 136, false, font::CENTER );
        setFrameVis( 0, true );
        setFrameVis( 1, false );
        updateOAM( false );
        waitForInteract( );
    }

    void openingAnimation( ) {
        // TODO
    }
} // namespace IO::ANIM
