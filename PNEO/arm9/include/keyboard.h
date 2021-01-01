/*
Pokémon neo
------------------------------

file        : keyboard.h
author      : Philip Wellnitz
description :

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

#include <functional>
#include <string>
#include <vector>
#include <nds.h>
#include "uio.h"

namespace IO {
#define MAX_KEYBOARD_PAGES 5
    class keyboard {
      private:
        u8 _page   = 0;
        u8 _frame  = 0;
        u8 _curSel = 0;

        u8 maxPages( ) const;

        std::vector<std::pair<inputTarget, u8>> getTouchPositions( ) const;

        void drawPage( ) const;

        void init( ) const;

        void drawChar( u8 p_pos, u16 p_char );
        void clearChar( u8 p_pos );

        void select( u8 p_idx, u8 p_frame );

        u16 getNextChar( );

      public:
        keyboard( ) {
        }

        std::string getText( u8 p_length );
    };
} // namespace IO
