/*
    Pokémon neo
    ------------------------------

    file        : specials.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2021 - 2023
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

namespace SPX {
    /*
     * @brief: Makes the plader choose an initial pkmn.
     */
    void runInitialPkmnSelection( );

    /*
     * @brief: Shows how Wally catches a Ralts (which may be shiny to troll the player; in
     * that case flag 27 is set).
     */
    void runCatchingTutorial( );

    /*
     * @brief: Runs a slot machine mini game (TODO)
     */
    void runSlotMachine( );

    /*
     * @brief: Runs the pokeblock blender mini game.
     */
    void runPokeblockBlender( u8 p_numNPC, bool p_rotom = false, bool p_blendMaster = false );

    /*
     * @brief: Shows the players Id and badges.
     */
    void runIDViewer( );

    void runHallOfFame( );
} // namespace SPX
