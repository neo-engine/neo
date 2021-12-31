/*
    Pokémon neo
    ------------------------------

    file        : animations.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

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

#include <nds/ndstypes.h>
#include "pokemon.h"

namespace IO::ANIM {
    /*
     * @brief: Plays the pkmn evolution animation. Destroys everything that was on the
     * screen.
     */
    bool evolvePkmn( u16 p_startSpecies, u8 p_startForme, u16 p_endSpecies, u8 p_endForme,
                     bool p_shiny, bool p_female, bool p_allowAbort = true );

    /*
     * @brief: Plays the egg hatch animation. Destroys everything that was on the
     * screen.
     */
    void hatchEgg( u16 p_endSpecies, u8 p_endForme, bool p_shiny, bool p_female );

    /*
     * @brief: Plays the opening animation.
     */
    void openingAnimation( );
} // namespace IO::ANIM
