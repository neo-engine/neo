/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : hexdec.c
    author      : Philip Wellnitz (RedArceus)
    description : Hex to decimal converter.

    Copyright (C) 2012 - 2014
        Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        is required.

    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
        distribution.
*/



static const long hextable[] = {
    [0 ... 255] = -1, // bit aligned access into this table is considerably
    ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // faster for most modern processors,
    ['A'] = 10, 11, 12, 13, 14, 15,       // for the space conscious, reduce to
    ['a'] = 10, 11, 12, 13, 14, 15        // signed char.
};
long hexdec(unsigned const char *hex) {
    long ret = 0; 
    while (*hex && ret >= 0) 
        ret = (ret << 4) | hextable[*hex++];
    return ret; 
}