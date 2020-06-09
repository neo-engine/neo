/*
Pokémon neo
------------------------------

file        : battleUI.h
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

#include "battleDefines.h"

namespace BATTLE {
    class battleUI {
        u8   _platform;
        u8   _platform2;
        u8   _background;
        battleMode _mode;

        u8   _curHP[ 2 ][ 2 ] = { { 0 } };

        void initTop( );
        void initSub( );

        /*
         * @brief: Updates the HP bar for the given pkmn to the specified value.
         * @param p_init: Initialize the hp bar first.
         */
        void updateHPbar( bool p_opponent, u8 p_pos, u8 p_newValue, bool p_init = false );

      public:
        battleUI() {}

        battleUI( u8 p_platform, u8 p_platform2, u8 p_background, battleMode p_mode )
            : _platform( p_platform ), _platform2( p_platform2 ), _background( p_background ),
              _mode( p_mode ) {
        }

        /*
         * @brief: Initializes the battleUI. Destroys anything that was previously on the
         * screen.
         */
        void init( );

        /*
         * @brief: Update the stats of the pkmn at the given position.
         */
        void updatePkmnStats( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: Recalls the given pkmn.
         */
        void recallPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: Sends out the given pkmn at the specified position.
         */
        void sendOutPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: shows the wild pkmn appear
         */
        void startWildBattle( pokemon* p_pokemon );
    };
} // namespace BATTLE
