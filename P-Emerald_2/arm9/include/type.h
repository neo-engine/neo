/*
    Pokémon neo
    ------------------------------

    file        : type.h
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

#include "Boden.h"
#include "Dra.h"
#include "Eis.h"
#include "Elek.h"
#include "Fee.h"
#include "Feuer.h"
#include "Flug.h"
#include "Geist.h"
#include "Gest.h"
#include "Gift.h"
#include "Kaefer.h"
#include "Kampf.h"
#include "Normal.h"
#include "Pfl.h"
#include "Psycho.h"
#include "Stahl.h"
#include "Unbek.h"
#include "Unl.h"
#include "Wasser.h"

enum type {
    NORMAL    = 0,
    FIGHTING  = 1,
    FLYING    = 2,
    POISON    = 3,
    GROUND    = 4,
    ROCK      = 5,
    BUG       = 6,
    GHOST     = 7,
    STEEL     = 8,
    UNKNOWN   = 9,
    WATER     = 10,
    FIRE      = 11,
    GRASS     = 12,
    LIGHTNING = 13,
    PSYCHIC   = 14,
    ICE       = 15,
    DRAGON    = 16,
    DARKNESS  = 17,
    FAIRY     = 18
};

float getEffectiveness( const type& p_t1, const type& p_t2 );

enum ailment {
    NONE = 0,
    PARALYSIS,
    SLEEP,
    FREEZE,
    BURN,
    POISONED,
    CONFUSION,
    INFATUATION,
    TRAP,
    NIGHTMARE,
    TORMENT,
    DISABLE,
    YAWN,
    HEAL_BLOCK,
    NO_TYPE_IMMUNITY,
    LEECH_SEED,
    EMBARGO,
    PERISH_SONG,
    INGRAIN
};
