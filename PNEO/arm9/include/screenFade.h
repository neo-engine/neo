/*
Pokémon neo
------------------------------

file        : screenFade.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
#pragma once

namespace IO {
    enum fadeType {
        UNFADE,
        UNFADE_FAST,
        UNFADE_IMMEDIATE_HALF,
        UNFADE_IMMEDIATE,

        CLEAR_DARK,
        CLEAR_DARK_FAST,
        CLEAR_DARK_IMMEDIATE_HALF,
        CLEAR_DARK_IMMEDIATE,
        CLEAR_WHITE,
        CLEAR_WHITE_FAST,
        CLEAR_WHITE_IMMEDIATE_HALF,
        CLEAR_WHITE_IMMEDIATE,
        CAVE_ENTRY,
        CAVE_EXIT,

        BATTLE,
        BATTLE_STRONG_OPPONENT
    };

#define FADE_TOP( )                                                                   \
    do {                                                                              \
        REG_BLDCNT = BLEND_FADE_WHITE | BLEND_SRC_BG0 | BLEND_SRC_BG1 | BLEND_SRC_BG2 \
                     | BLEND_SRC_BG3 | BLEND_SRC_SPRITE;                              \
        REG_BLDY = 0x1F;                                                              \
    } while( 0 )
#define FADE_TOP_DARK( )                                                              \
    do {                                                                              \
        REG_BLDCNT = BLEND_FADE_BLACK | BLEND_SRC_BG0 | BLEND_SRC_BG1 | BLEND_SRC_BG2 \
                     | BLEND_SRC_BG3 | BLEND_SRC_SPRITE;                              \
        REG_BLDY = 0x1F;                                                              \
    } while( 0 )
#define FADE_SUB_DARK( )                                                                  \
    do {                                                                                  \
        REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG0 | BLEND_SRC_BG1 | BLEND_SRC_BG2 \
                         | BLEND_SRC_BG3 | BLEND_SRC_SPRITE;                              \
        REG_BLDY_SUB = 0x1F;                                                              \
    } while( 0 )

    void fadeScreen( fadeType p_type, bool p_bottom = false, bool p_both = false );
    void resetScale( bool p_bottom, bool p_both = false );
    void clearScreen( bool p_bottom, bool p_both = false, bool p_dark = true );
    void clearScreenConsole( bool p_bottom, bool p_both = false );
} // namespace IO
