/*
    Pokémon neo
    ------------------------------

    file        : sseqData.h
    author      : Philip Wellnitz
    description : Header file.

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

#pragma once
#include "defines.h"

#ifndef MMOD
namespace SOUND::SSEQ {
    constexpr u8 MAX_SAMPLES_PER_SSEQ = 4;
    constexpr u8 NUM_SSEQ             = 100;
    struct sseqData {
        u16 m_bank; // bank used for sseq
        u16 m_sseqId;
        u16 m_sampleCnt;
        u16 m_samplesId[ MAX_SAMPLES_PER_SSEQ ];
    };

    extern const sseqData SSEQ_LIST[ NUM_SSEQ ];
} // namespace SOUND::SSEQ
#endif
