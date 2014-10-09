/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : keyboard.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

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

#include <nds.h>
#include <string>
#include <map>

extern PrintConsole Bottom;
#define MAXKEYBOARDS 3
extern void updateTime( int );
class keyboard {
public:
    keyboard( )
        : _ind( 0 ) { }

    std::wstring getText( int p_length, const char* p_msg = 0, bool p_time = true );

private:
    int _ind;
    std::map<std::pair<int, int>, char> _chars;

    void draw( );
    void undraw( );
};
