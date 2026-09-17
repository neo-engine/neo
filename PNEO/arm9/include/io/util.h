/*
Pokémon neo
------------------------------

file        : util.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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
#pragma once
#include <string>
#include <io/font.h>
#include <io/fontUtil.h>
#include <io/inputTarget.h>
#include <nds.h>

#include "io/sprite.h"
#include "save/saveGame.h"

namespace IO {
    const u16 SELECTED_SPR_PAL[ 16 ] = { 0x7FFF, 0x20C3, 0x5208, 0x7FFF, 0x7F2C, 0x318C };

    inline u16* BG_PAL( bool p_bottom ) {
        return p_bottom ? BG_PALETTE_SUB : BG_PALETTE;
    }
    inline u16* BG_BMP( bool p_bottom ) {
        return p_bottom ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 );
    }

    extern ConsoleFont* consoleFont;

    extern OAMTable*  Oam;
    extern SpriteInfo spriteInfo[ SPRITE_COUNT ];

    extern OAMTable*  OamTop;
    extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    extern int          bg2;
    extern int          bg3;
    extern int          bg2sub;
    extern int          bg3sub;
    extern PrintConsole Bottom;
    extern PrintConsole Top;

    void initVideo( bool p_noFade = false );
    void initVideoSub( bool p_noFade = false );
    void vramSetup( bool p_noFade = false );

    void swapScreens( );
    void animateBG( u8 p_frame, int p_bg );

    void setDefaultConsoleTextColors( u16* p_palette, u8 p_start = 1 );

    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, bool p_big = false ); // HP in %
    void displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub = false ); // HP in %
    void displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2,
                    bool p_delay, u8 p_innerR = 14, u8 p_outerR = 15, bool p_sub = false );

    std::string formatDate( u8 p_date[ 3 ] );
    std::string formatDate( u8 p_date[ 3 ], u8 p_language );
    std::string formatDate( SAVE::date p_date );
    std::string formatDate( SAVE::date p_date, u8 p_language );

    constexpr u16 getColor( BATTLE::type p_type ) {
        switch( p_type ) {
        case BATTLE::TYPE_NORMAL: return NORMAL_COLOR; break;
        case BATTLE::TYPE_FIGHTING: return RED; break;
        case BATTLE::TYPE_FLYING: return TURQOISE; break;
        case BATTLE::TYPE_POISON: return POISON_COLOR; break;
        case BATTLE::TYPE_GROUND: return GROUND_COLOR; break;
        case BATTLE::TYPE_ROCK: return ROCK_COLOR; break;
        case BATTLE::TYPE_BUG: return BUG_COLOR; break;
        case BATTLE::TYPE_GHOST: return GHOST_COLOR; break;
        case BATTLE::TYPE_STEEL: return STEEL_COLOR; break;
        case BATTLE::TYPE_UNKNOWN: return UNKNOWN_COLOR; break;
        case BATTLE::TYPE_WATER: return BLUE; break;
        case BATTLE::TYPE_FIRE: return ORANGE; break;
        case BATTLE::TYPE_GRASS: return GREEN; break;
        case BATTLE::TYPE_LIGHTNING: return YELLOW; break;
        case BATTLE::TYPE_PSYCHIC: return PURPLE; break;
        case BATTLE::TYPE_ICE: return ICE_COLOR; break;
        case BATTLE::TYPE_DRAGON: return DRAGON_COLOR; break;
        case BATTLE::TYPE_DARKNESS: return BLACK; break;
        case BATTLE::TYPE_FAIRY: return FAIRY_COLOR; break;
        default: return DRAGON_COLOR; break;
        }
        return WHITE;
    }

    /*
     * @brief A sine approximation via a third-order cosine approx.
     * @param p_x   angle (with 2^15 units/circle)
     * @return     Sine value (Q12)
     */
    constexpr s32 isin( s32 p_x ) {
        const u16 qN = 13, qA = 12, qP = 15, qR = 2 * qN - qP, qS = qN + qP + 1 - qA;
        p_x <<= ( 30 - qN );               // shift to full s32 range (Q13->Q30)
        if( ( p_x ^ ( p_x << 1 ) ) < 0 ) { // test for quadrant 1 or 2
            p_x = ( 1 << 31 ) - p_x;
        }
        p_x >>= ( 30 - qN );
        return p_x * ( ( 3 << qP ) - ( p_x * p_x >> qR ) ) >> qS;
    }

} // namespace IO
