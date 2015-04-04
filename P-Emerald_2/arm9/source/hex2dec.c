/*
Pokémon Emerald 2 Version
------------------------------

file        : hex2dec.c
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



static const long hextable[] = {
    [0 ... 255] = -1, // bit aligned access into this table is considerably
    ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // faster for most modern processors,
    ['A'] = 10, 11, 12, 13, 14, 15,       // for the space conscious, reduce to
    ['a'] = 10, 11, 12, 13, 14, 15        // signed char.
};
long hexdec(unsigned const char *p_hex) {
    long ret = 0; 
    while (*p_hex && ret >= 0) 
        ret = (ret << 4) | hextable[*p_hex++];
    return ret; 
}