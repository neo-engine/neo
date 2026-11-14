/*
Pokémon neo
------------------------------

file        : battleUIAnim.cpp
author      : Philip Wellnitz
description : battle animations: throws, hits, status and capture effects, and the end-of-battle
screens

Copyright (C) 2012 - 2026
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

#include "bag/bagViewer.h"
#include "bag/item.h"
#include "battle/ability.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/moveNames.h"
#include "gen/pokemonNames.h"
#include "io/kana.h"
#include "io/keyboard.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/simpleWidget.h"
#include "io/sprite.h"
#include "io/util.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace BATTLE {
    void battleUI::animateBallRelease( bool p_opponent, u8 p_pos, u8 p_ballId ) const {
        u16 x = ( _mode == BM_SINGLE )
                    ? ( p_opponent ? PKMN_OPP_1_X_SINGLE : PKMN_OWN_1_X_SINGLE )
                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_X : PKMN_OPP_1_X )
                                     : ( p_pos ? PKMN_OWN_2_X : PKMN_OWN_1_X ) ) );
        u16 y = ( _mode == BM_SINGLE )
                    ? ( p_opponent ? PKMN_OPP_1_Y_SINGLE : PKMN_OWN_1_Y_SINGLE )
                    : ( ( p_opponent ? ( p_pos ? PKMN_OPP_2_Y : PKMN_OPP_1_Y )
                                     : ( p_pos ? PKMN_OWN_2_Y : PKMN_OWN_1_Y ) ) );

        x += 24;
        y += 48;

        IO::OamTop->matrixBuffer[ 7 ].hdx = 1LLU << 9;
        IO::OamTop->matrixBuffer[ 7 ].vdx = 0;
        IO::OamTop->matrixBuffer[ 7 ].hdy = 0;
        IO::OamTop->matrixBuffer[ 7 ].vdy = 1LLU << 9;

        IO::loadPokeBallIcon( p_ballId, 0, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );

        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 1, x - 1, y - 2, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 2, x - 2, y - 1, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );

        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 3, x - 3, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );

        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 4, x - 3, y + 1, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 5, x - 2, y + 2, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 6, x - 1, y + 4, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 7, x, y + 7, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }

        SOUND::playSoundEffect( SFX_BATTLE_RECALL );

        IO::loadPokeBallIcon( p_ballId, 8, x, y + 10, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 9, x, y + 10, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 10, x, y + 10, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        if( p_opponent ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        }
        IO::updateOAM( false );
        for( u8 i = 0; i < 8; ++i ) { swiWaitForVBlank( ); }

        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = false;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 0;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isHidden      = true;
        IO::updateOAM( false );
        for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::animateBallThrow( u8 p_slot, u8 p_ballId ) const {
        (void) p_slot;

        SOUND::playSoundEffect( SFX_BATTLE_THROW );

        u16 x = 0, y = 0;

        IO::OamTop->matrixBuffer[ 7 ].hdx = 1LLU << 8;
        IO::OamTop->matrixBuffer[ 7 ].vdy = 1LLU << 8;
        IO::OamTop->matrixBuffer[ 7 ].vdx = 0;
        IO::OamTop->matrixBuffer[ 7 ].hdy = 0;

        for( x = 92; x < 184; x++ ) {
            y    = x * x / 100 - 3 * x + 236;
            u8 f = x - 92;

            if( x % 2 == 0 ) {
                IO::OamTop->matrixBuffer[ 7 ].hdx += 5;
                IO::OamTop->matrixBuffer[ 7 ].vdy += 5;
            }

            if( ( f % 6 ) == 0 ) {
                IO::loadPokeBallIcon( p_ballId, ( f / 3 + 6 ) % 9, x, y, SPR_BALL_START_OAM,
                                      SPR_BALL_PAL, SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
                IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
                IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
                IO::updateOAM( false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
            }

            if( x % 4 == 2 ) {
                IO::OamTop->matrixBuffer[ 7 ].hdx++;
                IO::OamTop->matrixBuffer[ 7 ].vdy++;
            }
            if( x % 8 == 4 ) { swiWaitForVBlank( ); }
        }
        IO::OamTop->matrixBuffer[ 7 ].hdx = 1LLU << 9;
        IO::OamTop->matrixBuffer[ 7 ].vdy = 1LLU << 9;

        SOUND::playSoundEffect( SFX_BATTLE_JUMPTOBALL );
        IO::loadPokeBallIcon( p_ballId, 9, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }

        IO::loadPokeBallIcon( p_ballId, 10, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                              SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }
    }

    void battleUI::animateShiny( bool p_opponent, u8 p_slot, u8 p_shinyType ) const {
        SOUND::playSoundEffect( SFX_SHINY );
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        if( p_shinyType == 2 ) { // Forced shinies
            IO::loadUIIcon( IO::ICON::SHINY_BIG_ALT2_START, SPR_SHINY_PARTICLE_START_OAM + 15,
                            SPR_SHINY_ICON_PAL, oam[ SPR_SHINY_PARTICLE_START_OAM + 15 ].gfxIndex,
                            0, 0, 16, 16, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        } else if( p_shinyType == 1 ) { // Circle for shinies due to extra rolls
            IO::loadUIIcon( IO::ICON::SHINY_BIG_ALT_START, SPR_SHINY_PARTICLE_START_OAM + 15,
                            SPR_SHINY_ICON_PAL, oam[ SPR_SHINY_PARTICLE_START_OAM + 15 ].gfxIndex,
                            0, 0, 16, 16, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        } else { // normal star
            IO::loadUIIcon( IO::ICON::SHINY_BIG_START, SPR_SHINY_PARTICLE_START_OAM + 15,
                            SPR_SHINY_ICON_PAL, oam[ SPR_SHINY_PARTICLE_START_OAM + 15 ].gfxIndex,
                            0, 0, 16, 16, false, false, true, OBJPRIORITY_0, false,
                            OBJMODE_BLENDED );
        }

        u16 centerx = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].x + 48;
        u16 centery = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].y + 48;

        u16 radius = 25;
        u16 sm_sh = 4, bg_sh = 8;

        if( !p_opponent ) {
            sm_sh = 8, bg_sh = 16;
            radius *= 2;
            centerx += 48;
            centery += 48;
        }

        constexpr s8 dirs[ 32 ][ 2 ] = {
            { 10, 0 },  { 10, 2 },   { 9, 4 },   { 8, 6 },
            { 7, 7 },   { 6, 8 },    { 4, 9 },   { 2, 10 },

            { 0, 10 },  { -2, 10 },  { -4, 9 },  { -6, 8 },
            { -7, 7 },  { -8, 6 },   { -9, 4 },  { -10, 2 },

            { -10, 0 }, { -10, -2 }, { -9, -4 }, { -8, -6 },
            { -7, -7 }, { -6, -8 },  { -4, -9 }, { -2, -10 },

            { 0, -10 }, { 2, -10 },  { 4, -9 },  { 6, -8 },
            { 7, -7 },  { 8, -6 },   { 9, -4 },  { 10, -2 },
        };

        // init frame
        for( u8 i = 0; i < 8; ++i ) {
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = false;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = false;

            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].x = centerx + dirs[ 4 * i ][ 0 ] - sm_sh;
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].y = centery + dirs[ 4 * i ][ 1 ] - sm_sh;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].x
                = centerx + dirs[ 4 * i + 2 ][ 0 ] - bg_sh;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].y
                = centery + dirs[ 4 * i + 2 ][ 1 ] - bg_sh;
        }
        IO::updateOAM( false );
        swiWaitForVBlank( );

        for( u8 f = 0; f < 14; ++f ) {
            for( u8 i = 0; i < 8; ++i ) {
                u8 pos = ( 4 * i + ( f & 3 ) ) & 31;

                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].x
                    = centerx + ( dirs[ pos ][ 0 ] * ( radius + 5 * f / 4 ) / 10 ) - sm_sh;
                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].y
                    = centery + ( dirs[ pos ][ 1 ] * ( radius + 5 * f / 4 ) / 10 ) - sm_sh;
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].x
                    = centerx + ( dirs[ ( pos + 2 ) & 31 ][ 0 ] * ( radius + 5 * f / 4 ) / 10 )
                      - bg_sh;
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].y
                    = centery + ( dirs[ ( pos + 2 ) & 31 ][ 1 ] * ( radius + 5 * f / 4 ) / 10 )
                      - bg_sh;
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
            if( f & 1 ) { swiWaitForVBlank( ); }
        }

        // hide everything
        for( u8 i = 0; i < 8; ++i ) {
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = true;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = true;
        }

        // some random extra noise
        for( u8 f = 0; f < 8; ++f ) {
            for( u8 i = 0; i < 8; ++i ) {
                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = rand( ) & 2;
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = rand( ) & 2;

                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].x = centerx - 32 - sm_sh + ( rand( ) & 63 );
                oam[ SPR_SHINY_PARTICLE_START_OAM + i ].y = centery - 32 - sm_sh + ( rand( ) & 63 );
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].x
                    = centerx - 32 - bg_sh + ( rand( ) & 63 );
                oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].y
                    = centery - 32 - bg_sh + ( rand( ) & 63 );
            }
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }

        // hide everything
        for( u8 i = 0; i < 8; ++i ) {
            oam[ SPR_SHINY_PARTICLE_START_OAM + i ].isHidden     = true;
            oam[ SPR_SHINY_PARTICLE_START_OAM + 8 + i ].isHidden = true;
        }
        IO::updateOAM( false );

        swiWaitForVBlank( );
    }

    void battleUI::animateStatChange( bool p_opponent, u8 p_slot, bool p_down ) const {
        SpriteEntry* oam = IO::OamTop->oamBuffer;

        s8  diff = 0, shift = 0;
        u16 centerx = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].x + 48;
        u16 centery = oam[ SPR_PKMN_START_OAM( 2 * ( !p_opponent ) + p_slot ) ].y + 48;

        u16 scale = 1;

        if( p_down ) {
            SOUND::playSoundEffect( SFX_BATTLE_DECREASE );
            IO::loadUIIcon( IO::ICON::STAT_DOWN_START, SPR_STAT_CHANGE_PARTICLE_START_OAM,
                            SPR_STAT_CHANGE_PAL, oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM ].gfxIndex,
                            0, 0, 8, 8, false, false, true, OBJPRIORITY_0, false, OBJMODE_BLENDED );
            diff  = 1;
            shift = -20;
        } else {
            SOUND::playSoundEffect( SFX_BATTLE_INCREASE );
            IO::loadUIIcon( IO::ICON::STAT_UP_START, SPR_STAT_CHANGE_PARTICLE_START_OAM,
                            SPR_STAT_CHANGE_PAL, oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM ].gfxIndex,
                            0, 0, 8, 8, false, false, true, OBJPRIORITY_0, false, OBJMODE_BLENDED );
            diff  = -1;
            shift = 20;
        }

        if( !p_opponent ) {
            centerx += 48;
            centery += 48;
            scale = 2;
            shift *= scale;
        }

        constexpr s8 pos[ 15 ][ 2 ] = {
            { -19, 5 }, { -19, -5 }, { -10, 0 }, { -10, 10 }, { -10, -10 }, { 0, 5 },   { 0, -5 },
            { 0, 15 },  { 0, -15 },  { 10, 0 },  { 10, 10 },  { 10, -10 },  { 19, -5 }, { 19, 5 },
        };

        for( u8 i = 0; i < 14; ++i ) {
            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isHidden = false;
            if( !p_opponent ) {
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isSizeDouble  = true;
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isRotateScale = true;
            }

            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].x = centerx + 3 * pos[ i ][ 0 ] / 2;
            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].y
                = centery + shift + 3 * scale * pos[ i ][ 1 ] / 2;
        }
        IO::updateOAM( false );

        for( u8 j = 0; j < 10; ++j ) {
            for( u8 i = 0; i < 14; ++i ) {
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].y += 2 * diff * scale;
            }
            swiWaitForVBlank( );
            IO::updateOAM( false );
        }
        for( u8 i = 0; i < 14; ++i ) {
            oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isHidden = true;
            if( !p_opponent ) {
                oam[ SPR_STAT_CHANGE_PARTICLE_START_OAM + i ].isRotateScale = false;
            }
        }
        IO::updateOAM( false );
    }

    void battleUI::animateHitPkmn( bool p_opponent, u8 p_pos, u8 p_effectiveness ) {
        if( p_effectiveness > 100 ) {
            SOUND::playSoundEffect( SFX_BATTLE_DAMAGE_SUPER );
        } else if( p_effectiveness < 100 ) {
            SOUND::playSoundEffect( SFX_BATTLE_DAMAGE_WEAK );
        } else {
            SOUND::playSoundEffect( SFX_BATTLE_DAMAGE_NORMAL );
        }

        for( u8 f = 0; f < 4; ++f ) {
            hidePkmn( p_opponent, p_pos );
            for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
            showPkmn( p_opponent, p_pos );
            for( u8 i = 0; i < 5; ++i ) { swiWaitForVBlank( ); }
        }
    }

    void battleUI::animateCapturePkmn( u16 p_pokeball, u8 p_ticks ) {
        animateBallThrow( 0, BAG::itemToBall( p_pokeball ) );
        auto pball = BAG::itemToBall( p_pokeball );

        u16 x = IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].x;
        u16 y = IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y;

        hidePkmn( true, 0 );

        x += 3;

        IO::loadPokeBallIcon( pball, 9, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ),
                              OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }

        x += 3;
        IO::loadPokeBallIcon( pball, 0, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL, SPR_PKMN_GFX( 4 ),
                              OBJPRIORITY_0, false );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i ) { swiWaitForVBlank( ); }
        swiWaitForVBlank( );

        SOUND::playSoundEffect( SFX_BATTLE_BALLDROP );
        for( ; y < OPP_PLAT_Y + 14; ++y ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y = y;
            if( y & 1 ) {
                IO::updateOAM( false );
                swiWaitForVBlank( );
            }
        }
        for( ; y >= OPP_PLAT_Y; --y ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y = y;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        SOUND::playSoundEffect( SFX_BATTLE_BALLDROP );
        for( ; y < OPP_PLAT_Y + 14; ++y ) {
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].y = y;
            if( y & 1 ) {
                IO::updateOAM( false );
                swiWaitForVBlank( );
            }
        }

        // "ticks"

        for( int i = 0; i < 30; ++i ) swiWaitForVBlank( );
        if( !p_ticks ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
        for( auto i : { 11, 12, 13, 12, 11, 0 } ) {
            IO::loadPokeBallIcon( pball, i, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                                  SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
            IO::updateOAM( false );
            for( u8 j = 0; j < 4; ++j ) { swiWaitForVBlank( ); }
        }
        for( int i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
        if( p_ticks == 1 ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
        for( auto i : { 14, 15, 16, 15, 14, 0 } ) {
            IO::loadPokeBallIcon( pball, i, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                                  SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
            IO::updateOAM( false );
            for( u8 j = 0; j < 4; ++j ) { swiWaitForVBlank( ); }
        }
        for( int i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
        if( p_ticks == 2 ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_BATTLE_BALLSHAKE );
        for( auto i : { 11, 12, 13, 12, 11, 0 } ) {
            IO::loadPokeBallIcon( pball, i, x, y, SPR_BALL_START_OAM, SPR_BALL_PAL,
                                  SPR_PKMN_GFX( 4 ), OBJPRIORITY_0, false );
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = true;
            IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].rotationIndex = 7;
            IO::updateOAM( false );
            for( u8 j = 0; j < 4; ++j ) { swiWaitForVBlank( ); }
        }
        for( int i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
        if( p_ticks == 3 ) { goto BREAK; }

        SOUND::playSoundEffect( SFX_CAPTURE_SUCCESSFUL );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].palette = SPR_PKMN_SHADOW_PAL;
        IO::updateOAM( false );
        for( int i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
        SOUND::playBGMOneshot( BGM_OS_PKMN_CAPTURE );
        for( int i = 0; i < 150; ++i ) { swiWaitForVBlank( ); }
        SOUND::playBGM( BGM_VICTORY_WILD );
        return;

    BREAK:

        SOUND::playSoundEffect( SFX_BATTLE_RECALL );
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isRotateScale = false;
        IO::OamTop->oamBuffer[ SPR_BALL_START_OAM ].isHidden      = true;
        IO::updateOAM( false );
        showPkmn( true, 0 );
        return;
    }

    void battleUI::animateGetVolatileStatusCondition( pokemon* p_pokemon, bool p_opponent,
                                                      u8 p_slot, volatileStatus p_status ) {
        // TODO
        (void) p_slot;

        auto                  pkmnstr = getPkmnName( p_pokemon, p_opponent );
        std::array<char, 100> buffer{ };

        switch( p_status ) {
        case VS_CONFUSION:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_WAS_CONFUSED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_LASERFOCUS:
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_GAINED_LASER_FOCUS ), pkmnstr.c_str( ) );
            break;
        case VS_MAGNETRISE:
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_ELECTROMAGNETISM_LEVITATED ), pkmnstr.c_str( ) );
            break;
        case VS_AQUARING:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_AQUA_RING_FORMED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_FOCUSENERGY:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_GETTING_PUMPED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_INGRAIN:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_PLANTED_ROOTS ),
                      pkmnstr.c_str( ) );
            break;
        case VS_FORESIGHT:
        case VS_MIRACLEEYE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_WAS_IDENTIFIED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_LEECHSEED:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_SEED_PLANTED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_ENDURE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_BRACED_ITSELF ),
                      pkmnstr.c_str( ) );
            break;
        case VS_PROTECT:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_PROTECTS_ITSELF ),
                      pkmnstr.c_str( ) );
            break;
        case VS_FOLLOWME:
        case VS_RAGEPOWDER:
        case VS_SPOTLIGHT:
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_CENTER_OF_ATTENTION ), pkmnstr.c_str( ) );
            break;
        case VS_MAGICCOAT:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_COATED_MIRROR ),
                      pkmnstr.c_str( ) );
            break;
        case VS_SNATCH:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_SNATCH_WAIT ),
                      pkmnstr.c_str( ) );
            break;
        case VS_TARSHOT:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_TAR_COATED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_POWERTRICK:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_STATS_SWAPPED ),
                      pkmnstr.c_str( ) );
            break;
        case VS_IMPRISON:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_IMPRISON_START ),
                      pkmnstr.c_str( ) );
            break;
        case VS_DESTINYBOND:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_DESTINY_BOND ),
                      pkmnstr.c_str( ) );
            break;
        case VS_BIDE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_STORING_ENERGY ),
                      pkmnstr.c_str( ) );
            break;
        case VS_GRUDGE:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_GRUDGE_START ),
                      pkmnstr.c_str( ) );
            break;
        case VS_POWDERED:
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_COVERED_IN_POWDER ), pkmnstr.c_str( ) );
            break;
        case VS_TELEKINESIS:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_HURLED_INTO_AIR ),
                      pkmnstr.c_str( ) );
            break;
        case VS_SMACKDOWN:
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_SMACK_DOWN_FELL ),
                      pkmnstr.c_str( ) );
            break;
        default: return;
        }
        log( buffer.data( ) );
    }

    void battleUI::animateVolatileStatusCondition( pokemon* p_pokemon, bool p_opponent, u8 p_slot,
                                                   volatileStatus p_status ) {
        // TODO
        (void) p_pokemon;
        (void) p_opponent;
        (void) p_slot;
        (void) p_status;
    }

    void battleUI::animateGetStatusCondition( pokemon* p_pokemon, bool p_opponent, u8 p_slot,
                                              u8 p_status ) {
        (void) p_slot;

        auto                  pkmnstr = getPkmnName( p_pokemon, p_opponent );
        std::array<char, 100> buffer{ };

        if( p_status == POISON ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_GOT_POISONED ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == TOXIC ) {
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_GOT_BADLY_POISONED ), pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == BURN ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_GOT_BURNED ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == SLEEP ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_FELL_ASLEEP ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == FROZEN ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_WAS_FROZEN ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == PARALYSIS ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_WAS_PARALYZED ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
    }

    void battleUI::animateStatusCondition( pokemon* p_pokemon, bool p_opponent, u8 p_slot,
                                           u8 p_status ) {
        (void) p_slot;

        auto                  pkmnstr = getPkmnName( p_pokemon, p_opponent );
        std::array<char, 100> buffer{ };

        if( p_status == POISON ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_IS_POISONED ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == TOXIC ) {
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_IS_BADLY_POISONED ), pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == BURN ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_IS_BURNED ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == SLEEP ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_ASLEEP ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == FROZEN ) {
            snprintf( buffer.data( ), buffer.size( ), GET_STRING( IO::STR_BATTLE_FROZEN_SOLID ),
                      pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
        if( p_status == PARALYSIS ) {
            snprintf( buffer.data( ), buffer.size( ),
                      GET_STRING( IO::STR_BATTLE_PARALYZED_CANT_MOVE ), pkmnstr.c_str( ) );
            log( buffer.data( ) );
        }
    }

    void battleUI::handleCapture( pokemon* p_pokemon ) {
        std::array<char, 100> buffer{ };

        showTopMessagePkmn( p_pokemon );
        printTopMessage( 0, true );

        IO::simpleYesNoBox yn;
        snprintf( buffer.data( ), buffer.size( ), GET_STRING( 139 ),
                  p_pokemon->m_boxdata.getNameS( ).c_str( ) );
        printTopMessage( buffer.data( ), false );

        if( yn.getResult( [ & ]( ) { return printYNMessage( 254 ); },
                          [ & ]( IO::yesNoBox::selection p_selection ) {
                              printYNMessage( p_selection == IO::yesNoBox::NO );
                          } )
            == IO::yesNoBox::YES ) {
            IO::keyboard kbd;
            printTopMessage( GET_STRING( 140 ), false );
            auto nick = kbd.getText( 10 );
            if( strcmp( nick.c_str( ), p_pokemon->m_boxdata.m_name )
                && strcmp( "", nick.c_str( ) ) ) {
                memcpy( p_pokemon->m_boxdata.m_name, nick.c_str( ), PKMN_NAMELENGTH );
                p_pokemon->m_boxdata.setIsNicknamed( true );
                p_pokemon->m_boxdata.setOrigLang( CURRENT_LANGUAGE );
            }
        }
        hideTopMessage( );
        initSub( );
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "battlesub2", 512, 49152,
                             true );
        u16* pal   = IO::BG_PAL( true );
        pal[ 0 ]   = 0;
        pal[ 250 ] = IO::WHITE;
        pal[ 251 ] = IO::GRAY;
        pal[ 252 ] = IO::RGB( 18, 22, 31 );
        pal[ 254 ] = IO::RGB( 31, 18, 18 );
        pal[ 253 ] = IO::RGB( 0, 0, 25 );
        pal[ 255 ] = IO::RGB( 23, 0, 0 );
        resetLog( );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
    }

    void battleUI::handleBattleEnd( bool p_playerWon ) {
        SpriteEntry* oam = IO::OamTop->oamBuffer;
        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < 2; ++j ) {
                hidePkmn( i, j );
                hidePkmnStats( i, j );
            }
        }
        resetLog( );
        IO::loadTrainerSprite( _battleTrainer->m_data.m_trainerBG, WILD_BATTLE_SPRITE_X + 16,
                               OPP_PLAT_Y - 96 + 35, SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ),
                               SPR_PKMN_PAL( 2 * ( !p_playerWon ) ),
                               SPR_PKMN_GFX( 2 * ( !p_playerWon ) ), false );
        IO::OamTop->matrixBuffer[ 5 ].hdx                               = 1 << 7 | 1 << 6 | 1 << 5;
        IO::OamTop->matrixBuffer[ 5 ].vdy                               = 1 << 7 | 1 << 6 | 1 << 5;
        oam[ SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ) ].isRotateScale = true;
        oam[ SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ) ].isSizeDouble  = true;
        oam[ SPR_PKMN_START_OAM( 2 * ( !p_playerWon ) ) ].rotationIndex = 5;

        oam[ SPR_PLATFORM_OAM + 2 ].priority = OBJPRIORITY_3;
        oam[ SPR_PLATFORM_OAM + 3 ].priority = OBJPRIORITY_3;

        IO::loadUIIcon( IO::ICON::MBOX1_START, SPR_MBOX_START_OAM, SPR_BALL_PAL,
                        SPR_PKMN_GFX( 2 * ( !p_playerWon ) + 1 ), 0, 192 - 46, 32, 64, false, false,
                        false, OBJPRIORITY_3, false );
        for( u8 i = 0; i < 13; ++i ) {
            IO::loadSprite( SPR_MBOX_START_OAM + 13 - i, SPR_BALL_PAL,
                            SPR_PKMN_GFX( 2 * ( !p_playerWon ) + 1 ), 32 + 16 * i, 192 - 46, 32, 64,
                            0, 0, 0, false, true, false, OBJPRIORITY_3, false );
        }
        IO::updateOAM( false );

        const bool  kana = ( CURRENT_LANGUAGE == SAVE::JAP );
        std::string msg = kana ? std::string( 1, IO::KANA::ON ) : std::string( );
        for( char* pos = p_playerWon ? _battleTrainer->m_strings.m_message2
                                     : _battleTrainer->m_strings.m_message3;
             *pos; ++pos ) {
            if( *pos == IO::KANA::ON || *pos == IO::KANA::OFF ) { continue; }
            if( *pos == '[' ) {
                std::string accmd = "";
                while( *( ++pos ) != ']' && *pos != '\0' ) {
                    accmd += *pos;
                }
                if( *pos == '\0' ) { break; }
                msg += IO::parseLogCmd( accmd );
                continue;
            } else if( *pos == '\r' ) {
                if( kana ) { msg += IO::KANA::OFF; }
                printTopMessage( msg.c_str( ), false );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::waitForInteract( IO::animateMBS,
                                     [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); } );
                msg = kana ? std::string( 1, IO::KANA::ON ) : std::string( );
                continue;
            }
            msg += *pos;
        }

        printTopMessage( msg.c_str( ), false );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::waitForInteract( IO::animateMBS, [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); } );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );

        IO::printRectangle( 0, 192 - 46, 255, 192, false, 0 );
        for( u8 i = 0; i <= 13; ++i ) { oam[ SPR_MBOX_START_OAM + 13 - i ].isHidden = true; }
        IO::updateOAM( false );
    }

    void battleUI::setNewWeather( weather p_newWeather ) {
        log( GET_STRING( IO::STR_BATTLE_WEATHER_CLEAR + u8( p_newWeather ) ) );
        _currentWeather = p_newWeather;
    }

    void battleUI::continueWeather( ) {
        if( _currentWeather == WE_NONE ) { return; }

        log( GET_STRING( IO::STR_BATTLE_WEATHER_RAIN_ONGOING + u8( _currentWeather ) - 1 ) );
    }

    void battleUI::addPseudoWeather( u8 p_pwIdx ) {
        log( GET_STRING( IO::STR_BATTLE_PW_IONDELUGE_ADDED + u8( p_pwIdx ) ) );
    }

    void battleUI::removePseudoWeather( u8 p_pwIdx ) {
        log( GET_STRING( IO::STR_BATTLE_PW_IONDELUGE_ENDED + u8( p_pwIdx ) ) );
    }

    void battleUI::setNewTerrain( terrain p_newTerrain ) {
        _currentTerrain = p_newTerrain;
        redrawBattleBG( );
        log( GET_STRING( IO::STR_BATTLE_TERRAIN_CLEAR + u8( p_newTerrain ) ) );
    }
} // namespace BATTLE
