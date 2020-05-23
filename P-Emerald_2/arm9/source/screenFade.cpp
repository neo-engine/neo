/*
Pokémon neo
------------------------------

file        : screenFade.cpp
author      : Philip Wellnitz
description :

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

#include <nds/arm9/background.h>
#include <nds/arm9/sprite.h>
#include <nds/bios.h>

#include "defines.h"
#include "fs.h"
#include "screenFade.h"
#include "uio.h"

namespace IO {
    void fadeScreen( fadeType p_type, bool p_bottom, bool p_both ) {
        auto& reg  = ( p_bottom ? REG_BLDY_SUB : REG_BLDY );
        auto& reg2 = ( p_both ? ( !p_bottom ? REG_BLDY_SUB : REG_BLDY ) : reg );

        auto& regcnt  = ( p_bottom ? REG_BLDCNT_SUB : REG_BLDCNT );
        auto& regcnt2 = ( p_both ? ( !p_bottom ? REG_BLDCNT_SUB : REG_BLDCNT ) : regcnt );

        switch( p_type ) {
        case IO::UNFADE: {
            u16 val = 0x1F;
            for( s8 i = 4; i >= 0; --i ) {
                for( u8 j = 0; j < 5; ++j ) swiWaitForVBlank( );
                val &= ~( 1 << u8( i ) );
                reg = reg2 = val;
            }
            swiWaitForVBlank( );
            break;
        }
        case IO::UNFADE_FAST: {
            u16 val = 0x1F;
            for( s8 i = 3; i >= 0; i -= 2 ) {
                swiWaitForVBlank( );
                val &= ~( 3 << u8( i ) );
                reg = reg2 = val;
            }
            swiWaitForVBlank( );
            break;
        }
        case IO::CLEAR_DARK_FAST:
            regcnt = regcnt2 = BLEND_FADE_BLACK | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3
                               | BLEND_SRC_SPRITE;
            reg = reg2 = 1;
            for( u8 i = 1; i < 5; i += 2 ) {
                swiWaitForVBlank( );
                reg = reg2 |= ( 3 << i );
            }
            swiWaitForVBlank( );
            break;
        case IO::CLEAR_DARK:
        case IO::CAVE_ENTRY:
            regcnt = regcnt2 = BLEND_FADE_BLACK | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3
                               | BLEND_SRC_SPRITE;
            reg = reg2 = 1;
            for( u8 i = 1; i < 5; ++i ) {
                for( u8 j = 0; j < 4; ++j ) swiWaitForVBlank( );
                reg = reg2 |= ( 1 << i );
            }
            swiWaitForVBlank( );
            if( p_type == CLEAR_DARK ) break;
            break;
        case IO::CLEAR_WHITE_FAST:
            regcnt = regcnt2 = BLEND_FADE_WHITE | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3
                               | BLEND_SRC_SPRITE;
            reg = reg2 = 1;
            for( u8 i = 1; i < 5; i += 2 ) {
                for( u8 j = 0; j < 4; ++j ) swiWaitForVBlank( );
                reg = reg2 |= ( 3 << i );
            }
            swiWaitForVBlank( );
            break;
        case IO::CLEAR_WHITE:
        case IO::CAVE_EXIT:
            regcnt = regcnt2 = BLEND_FADE_WHITE | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3
                               | BLEND_SRC_SPRITE;
            reg = reg2 = 1;
            for( u8 i = 1; i < 5; ++i ) {
                for( u8 j = 0; j < 4; ++j ) swiWaitForVBlank( );
                reg = reg2 |= ( 1 << i );
            }
            swiWaitForVBlank( );
            if( p_type == CLEAR_WHITE ) break;
            break;
        case IO::BATTLE:
        case IO::BATTLE_STRONG_OPPONENT:
            regcnt = regcnt2 = BLEND_FADE_BLACK | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3;
            for( u8 j = 0; j < p_type - IO::BATTLE + 1; ++j ) {
                u16 val = 0x1F;
                for( s8 i = 4; i >= 0; --i ) {
                    for( u8 k = 0; k < 2 + j; ++k ) swiWaitForVBlank( );
                    val &= ~( 1 << u8( i ) );
                    reg = reg2 = val;
                }
                for( u8 i = 1; i < 3; ++i ) swiWaitForVBlank( );
                reg = reg2 = 1;
                for( u8 i = 1; i < 5; ++i ) {
                    for( u8 k = 0; k < 2 + j; ++k ) swiWaitForVBlank( );
                    reg = reg2 |= ( 1 << i );
                }
                for( u8 i = 1; i < 3; ++i ) swiWaitForVBlank( );
            }
            regcnt = regcnt2 = BLEND_FADE_BLACK | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3
                               | BLEND_SRC_SPRITE;
            for( u8 i = 1; i < 5; ++i ) {
                for( u8 j = 0; j < 4; ++j ) swiWaitForVBlank( );
                reg = reg2 |= ( 1 << i );
            }
            swiWaitForVBlank( );
            break;
        default:
            break;
        }
    }

    void resetScale( bool p_bottom, bool p_both ) {
        if( p_both || !p_bottom ) {
            bgSetScroll( IO::bg3, 0, 0 );
            bgSetScale( IO::bg3, 1 << 8, 1 << 8 );
        }
        if( p_both || p_bottom ) {
            bgSetScroll( IO::bg3sub, 0, 0 );
            bgSetScale( IO::bg3sub, 1 << 8, 1 << 8 );
        }
    }

    void clearScreen( bool p_bottom, bool p_both, bool p_dark ) {
        (void) p_dark;

        if( p_both || p_bottom ) {
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );
            dmaFillHalfWords( 0, BG_PALETTE_SUB, 256 );
        }
        if( p_both || !p_bottom ) {
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
            dmaFillHalfWords( 0, BG_PALETTE, 256 );
        }
        regularFont->setColor( 0, 0 );
    }
    void clearScreenConsole( bool p_bottom, bool p_both ) {
        if( p_both || !p_bottom ) {
            consoleSelect( &Top );
            consoleSetWindow( &Top, 0, 0, 32, 24 );
            consoleClear( );
        }
        if( p_both || p_bottom ) {
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
        }
    }
} // namespace IO
