/*
    Pokémon neo
    ------------------------------

    file        : sseqData.cpp
    author      : Philip Wellnitz
    description : Definitions of sseq music files

    Copyright (C) 2012 - 2022
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

#ifndef MMOD

#include "sound/sseqData.h"

namespace SOUND::SSEQ {
    constexpr u16 BANK_PL_BGM_FIELD       = 0;
    constexpr u16 BANK_PL_BGM_DUNGEON     = 1;
    constexpr u16 BANK_PL_BGM_BASIC       = 2;
    constexpr u16 BANK_PL_BGM_BATTLE      = 3;
    constexpr u16 BANK_BW_GATE            = 4;
    constexpr u16 BANK_BW_R10             = 5;
    constexpr u16 BANK_PMDB_ESC           = 6;
    constexpr u16 BANK_BW_EMO             = 7;
    constexpr u16 BANK_BW_BT_REGI         = 8;
    constexpr u16 BANK_BW_CAVE_REGI       = 9;
    constexpr u16 BANK_BW_BT_LEADER       = 10;
    constexpr u16 BANK_BW_BT_CHAMP        = 11;
    constexpr u16 BANK_BW_UNDERSEA_TUNNEL = 12;
    constexpr u16 BANK_BW_FS              = 13;
    constexpr u16 BANK_GS_BGM_BASIC       = 14;

    constexpr u16 SSEQ_PL_FLOAROMA_TOWN   = 0;
    constexpr u16 SSEQ_PL_LAKE_CAVE       = 1;
    constexpr u16 SSEQ_PL_FOREST          = 2;
    constexpr u16 SSEQ_PL_CAVE            = 3;
    constexpr u16 SSEQ_PL_GYM             = 4;
    constexpr u16 SSEQ_PL_PC_N            = 5;
    constexpr u16 SSEQ_PL_EYE_SHIRONA     = 6;
    constexpr u16 SSEQ_PL_EYE_MOUNTAIN    = 7;
    constexpr u16 SSEQ_PL_BT_PKMN         = 8;
    constexpr u16 SSEQ_PL_POKETORE        = 9;
    constexpr u16 SSEQ_PL_OS_BADGE        = 10;
    constexpr u16 SSEQ_PL_OS_TM           = 11;
    constexpr u16 SSEQ_PL_OS_HEAL         = 12;
    constexpr u16 SSEQ_BW_UNDERSEA_TUNNEL = 13;
    constexpr u16 SSEQ_PL_WIFIGIFT        = 14;
    constexpr u16 SSEQ_PL_TORN_WORLD      = 15;
    constexpr u16 SSEQ_PL_BT_TIMESPACE    = 16;
    constexpr u16 SSEQ_BW_GATE            = 17;
    constexpr u16 SSEQ_BW_R10             = 18;
    constexpr u16 SSEQ_PMDB_ESC           = 19;
    constexpr u16 SSEQ_BW_EMO             = 20;
    constexpr u16 SSEQ_BW_BT_REGI         = 21;
    constexpr u16 SSEQ_BW_CAVE_REGI       = 22;
    constexpr u16 SSEQ_BW_BT_LEADER       = 23;
    constexpr u16 SSEQ_BW_BT_CHAMP        = 24;
    constexpr u16 SSEQ_BW_FS              = 25;
    constexpr u16 SSEQ_GS_SHINKA          = 26;

    constexpr u16 SWAR_PL_BASIC           = 0;
    constexpr u16 SWAR_PL_BGM_FIELD       = 1;
    constexpr u16 SWAR_PL_BGM_DUNGEON     = 2;
    constexpr u16 SWAR_PL_BGM_BATTLE      = 3;
    constexpr u16 SWAR_BW_GATE            = 4;
    constexpr u16 SWAR_BW_R10             = 5;
    constexpr u16 SWAR_PMDB               = 6;
    constexpr u16 SWAR_BW_EMO             = 7;
    constexpr u16 SWAR_BW_BT_REGI         = 8;
    constexpr u16 SWAR_BW_CAVE_REGI       = 9;
    constexpr u16 SWAR_BW_BT_LEADER       = 10;
    constexpr u16 SWAR_BW_BT_CHAMP        = 11;
    constexpr u16 SWAR_BW_UNDERSEA_TUNNEL = 12;
    constexpr u16 SWAR_BW_FS              = 13;
    constexpr u16 SWAR_GS_BASIC           = 14;

    const sseqData SSEQ_LIST[ NUM_SSEQ ] = {
        { BANK_PL_BGM_FIELD, SSEQ_PL_FLOAROMA_TOWN, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_FIELD } },
        { BANK_PL_BGM_FIELD, SSEQ_PL_GYM, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_FIELD } },
        { BANK_PL_BGM_FIELD, SSEQ_PL_PC_N, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_FIELD } },
        { BANK_BW_GATE, SSEQ_BW_GATE, 1, { SWAR_BW_GATE } },
        { BANK_BW_R10, SSEQ_BW_R10, 1, { SWAR_BW_R10 } },

        { BANK_PMDB_ESC, SSEQ_PMDB_ESC, 1, { SWAR_PMDB } }, // defunct (sbnk tto large)
        { BANK_PL_BGM_DUNGEON, SSEQ_PL_LAKE_CAVE, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_DUNGEON } },
        { BANK_PL_BGM_DUNGEON, SSEQ_PL_FOREST, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_DUNGEON } },
        { BANK_PL_BGM_DUNGEON, SSEQ_PL_CAVE, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_DUNGEON } },
        { BANK_PL_BGM_DUNGEON, SSEQ_PL_TORN_WORLD, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_DUNGEON } },

        // 10

        { BANK_BW_CAVE_REGI, SSEQ_BW_CAVE_REGI, 1, { SWAR_BW_CAVE_REGI } },
        { BANK_PL_BGM_BASIC, SSEQ_PL_EYE_SHIRONA, 1, { SWAR_PL_BASIC } },
        { BANK_PL_BGM_BASIC, SSEQ_PL_EYE_MOUNTAIN, 1, { SWAR_PL_BASIC } },
        { BANK_BW_BT_LEADER, SSEQ_BW_BT_LEADER, 1, { SWAR_BW_BT_LEADER } },
        { BANK_BW_BT_CHAMP, SSEQ_BW_BT_CHAMP, 1, { SWAR_BW_BT_CHAMP } },

        { BANK_PL_BGM_BATTLE, SSEQ_PL_BT_PKMN, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_BATTLE } },
        { BANK_PL_BGM_BATTLE, SSEQ_PL_BT_TIMESPACE, 2, { SWAR_PL_BASIC, SWAR_PL_BGM_BATTLE } },
        { BANK_BW_BT_REGI, SSEQ_BW_BT_REGI, 1, { SWAR_BW_BT_REGI } },
        { BANK_BW_EMO, SSEQ_BW_EMO, 1, { SWAR_BW_EMO } },
        { BANK_PL_BGM_BASIC, SSEQ_PL_POKETORE, 1, { SWAR_PL_BASIC } },

        // 20

        { BANK_PL_BGM_BASIC, SSEQ_PL_WIFIGIFT, 1, { SWAR_PL_BASIC } },
        { BANK_PL_BGM_BASIC, SSEQ_PL_OS_BADGE, 1, { SWAR_PL_BASIC } },
        { BANK_PL_BGM_BASIC, SSEQ_PL_OS_TM, 1, { SWAR_PL_BASIC } },
        { BANK_PL_BGM_BASIC, SSEQ_PL_OS_HEAL, 1, { SWAR_PL_BASIC } },
        { BANK_BW_UNDERSEA_TUNNEL, SSEQ_BW_UNDERSEA_TUNNEL, 1, { SWAR_BW_UNDERSEA_TUNNEL } },

        { BANK_BW_FS, SSEQ_BW_FS, 1, { SWAR_BW_FS } },
        { BANK_GS_BGM_BASIC, SSEQ_GS_SHINKA, 1, { SWAR_GS_BASIC } },
    };

} // namespace SOUND::SSEQ
#endif
