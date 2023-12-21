/*
    Pokémon neo
    ------------------------------

    file        : berry.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

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

#pragma once
#include <nds.h>

namespace BAG {
    constexpr u8 NUM_BERRYSTATS = 5;

    struct berry {
        u16 m_size;     // fluff
        u8  m_firmness; // fluff
        u8  m_growthTime;
        u8  m_yield;
        u8  m_naturalGiftPower;
        u8  m_naturalGiftType;
        u8  m_smoothness;
        u8  m_flavor[ NUM_BERRYSTATS ];
    };
} // namespace BAG
