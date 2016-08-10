/*
Pokémon Emerald 2 Version
------------------------------

file        : statusScreenUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include "pokemon.h"
#include "uio.h"
#include <vector>

namespace STS {
    class statusScreenUI {
    public:
        u8 m_pagemax;
        virtual bool drawMove( const pokemon& p_pokemon, u8 p_moveIdx, bool p_bottom = false );
        virtual bool drawRibbon( const pokemon& p_pokemon, u8 p_ribbonIdx, bool p_bottom = false );
        virtual void draw( const pokemon& p_pokemon, u8 p_page, bool p_newpok ) = 0;

        virtual ~statusScreenUI( ) { }
    };

    class regStsScreenUI : public statusScreenUI {
        u8 _current;
        void initTop( );
        void initSub( );

    public:
        void init( u8 p_current, bool p_initTop = true );
        void draw( const pokemon& p_pokemon, u8 p_page, bool p_newpok ) override;
        std::vector<IO::inputTarget> draw( u8 p_current, bool p_updatePageIcons );

        regStsScreenUI( u8 p_pageMax = 5 );
    };

    class boxStsScreenUI : public regStsScreenUI {
    public:
        void init( );
        void draw( const pokemon& p_pokemon, u8 p_page, bool p_newpok ) override;
    };

    //class battleStsScreenUI : public statusScreenUI {
    //public:
    //    void init( u8 p_current ) override;
    //    void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
    //    void draw( u8 p_current ) override;
    //};
}
