/*
Pokémon Emerald 2 Version
------------------------------

file        : keyboard.h
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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


#include <nds.h>
#include <string>
#include <map>

namespace IO {
#define MAXKEYBOARDS 3
    class keyboard {
    public:
        keyboard( )
            : _ind( 0 ) { }

        std::wstring getText( u16 p_length, const char* p_msg = 0, bool p_time = true );

    private:
        int _ind;
        std::map<std::pair<int, int>, char> _chars;

        void draw( );
        void undraw( );
    };
}