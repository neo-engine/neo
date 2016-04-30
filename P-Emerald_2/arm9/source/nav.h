/*
Pokémon Emerald 2 Version
------------------------------

file        : nav.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2016
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
#pragma once

#include <nds.h>
#include <nds/ndstypes.h>
#include <string>

namespace IO {
#define MAXBG 12
#define START_BG 0
    extern unsigned int NAV_DATA[ 12288 ];
    extern unsigned short NAV_DATA_PAL[ 256 ];

    class nav {
    public:
        struct backgroundSet {
            std::string             m_name;
            const unsigned int      *m_mainMenu;
            const unsigned short    *m_mainMenuPal;
            bool                    m_loadFromRom;
            bool                    m_allowsOverlay;
            u8                      *m_mainMenuSpritePoses;
        };
        enum state {
            HOME,
            MAP,
            MAP_BIG,
            MAP_MUG
        };
    private:
        bool _power;
        state _state;
        bool _allowInit;
        u8 _curMap;

        void drawMapMug( );
    public:
        nav( );
        void draw( bool p_initMainSprites = false, u8 p_newIdx = (u8)255 );
        void showNewMap( u8 p_map );
        void handleInput( touchPosition p_touch );
        void home( ) {
            _state = HOME;
        }
        void togglePower( ) {
            _power = !_power;
        }
    };
    extern nav::backgroundSet BGs[ MAXBG ];
    extern nav* NAV;
}