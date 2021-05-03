/*
Pokémon neo
------------------------------

file        : choiceBox.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2021
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
#include "defines.h"
#include "uio.h"

namespace IO {
    /*
     * @brief: Makes the player select a numeric value.
     */
    class counter {
      public:
        static void DEFAULT_TICK( ) {
        }

      private:
        s32 _minValue;
        s32 _maxValue;

      public:
        counter( s32 p_minValue, s32 p_maxValue )
            : _minValue( p_minValue ), _maxValue( p_maxValue ) {
        }
        /*
         * @brief: Opens a counter and returns the player's selection.
         * @param p_drawFunction: Callback used to draw the counter. (the special value
         * 0 is used for the BACK button, _minValue - 1 for the EXIT button and _minValue
         * - 2 for the CONFIRM button)
         * @param p_updateValue: Callback used when the selected value changes.
         * @returns: A value between _minValue and _maxValue (both inclusive)
         */
        s32 getResult( std::function<std::vector<std::pair<inputTarget, s32>>( )> p_drawFunction,
                       std::function<void( s32, u8 )>                             p_updateValue,
                       std::function<void( s32 )> p_hoverButton, s32 p_initialValue = 0,
                       std::function<void( )> p_tick = DEFAULT_TICK );

        s32 getResult( const char* p_message, style p_style );
    };
} // namespace IO
