/*
Pokémon Emerald 2 Version
------------------------------

file        : keyboard.h
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2018
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <map>
#include <string>
#include <nds.h>

namespace IO {
#define MAX_KEYBOARD_PAGES 4
    class keyboard {
      public:
        std::string getText( u8 p_length, const char* p_msg );

        keyboard( ) : _page( 0 ) {
        }

      private:
        u8 _page = 0;

        std::string getText( u8 p_length );

        void drawPage( );
        void drawPage( const char* p_msg );

        void drawChar( u8 p_pos, u16 p_char );
        void clearChar( u8 p_pos );

        u16 getNextChar( );
    };
}
