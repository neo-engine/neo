/*
Pokémon neo
------------------------------

file        : keyboard.h
author      : Philip Wellnitz
description :

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

#include <functional>
#include <vector>
#include <string>
#include <nds.h>

namespace IO {
#define MAX_KEYBOARD_PAGES 4
    class keyboard {
      private:
        u8               _page = 0;
        std::vector<u16> _curRes;

        u8 maxPages( ) const;

        void drawPage( ) const;

        void init( ) const;

        void drawChar( u8 p_pos, u16 p_char );
        void clearChar( u8 p_pos );

        u16 getNextChar( );
      public:
        keyboard( ) { }

        std::string getText( u8 p_length );
    };
} // namespace IO
