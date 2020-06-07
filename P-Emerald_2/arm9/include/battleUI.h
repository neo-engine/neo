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

#include "battle.h"

namespace BATTLE {
    class battleUI {
        u8   _platform;
        u8   _platform2;
        u8   _background;
        bool _isWildBattle;

        void initTop( );
        void initSub( );

      public:
        battleUI() {}

        battleUI( u8 p_platform, u8 p_platform2, u8 p_background, bool p_isWildBattle )
            : _platform( p_platform ), _platform2( p_platform2 ), _background( p_background ),
              _isWildBattle( p_isWildBattle ) {
        }

        /*
         * @brief: Initializes the battleUI. Destroys anything that was previously on the
         * screen.
         */
        void init( );

        /*
         * @brief: shows the wild pkmn appear
         */
        void startWildBattle( pokemon* p_pokemon );
    };
} // namespace BATTLE
