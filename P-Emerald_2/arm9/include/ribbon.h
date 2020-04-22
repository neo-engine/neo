/*
Pokémon neo
------------------------------

file        : ribbon.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <string>
#include <vector>

#include <nds.h>
#include "pokemon.h"

struct ribbon {
    std::string m_name;
    std::string m_description;

    static std::vector<u8> getRibbons( const pokemon& p_pokemon );
};

#define MAX_RIBBONS ( 12 * 8 )
extern ribbon RibbonList[ MAX_RIBBONS ];
