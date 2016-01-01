/*
Pokémon Emerald 2 Version
------------------------------

file        : statusScreenUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2015
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
#include <vector>

namespace STS {
    class statusScreenUI {
    protected:
        friend class statusScreen;
        friend class regStsScreen;

        std::vector<pokemon>* _pokemon;

        bool _showTakeItem;
        u8 _showMoveCnt;

        virtual void init( u8 p_current, bool p_initTop = true ) = 0;
        virtual bool draw( u8 p_current, u8 p_moveIdx );
        virtual void drawRibbon( u8 p_current, u8 p_ribbonIdx );
        virtual void draw( u8 p_current, u8 p_page, bool p_newpok ) = 0;
        virtual void draw( u8 p_current ) = 0;
    public:
        u8 m_pagemax;
    };

    class regStsScreenUI : public statusScreenUI {
        u8 _current;
        void initTop( );
        void initSub( u16 p_pkmIdx );

    public:

        void init( u8 p_current, bool p_initTop = true ) override;
        bool draw( u8 p_current, u8 p_moveIdx ) override;
        void drawRibbon( u8 p_current, u8 p_ribbonIdx ) override;
        void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
        void draw( u8 p_current ) override;

        regStsScreenUI( std::vector<pokemon>* p_pokemon, u8 p_pageMax = 3 );
    };

    class boxStsScreenUI : public regStsScreenUI {
    public:
        static void drawPkmnInformation( const pokemon& p_pokemon );
        void init( u8 p_current, bool p_initTop = true ) override;
        void draw( u8 p_current ) override;

    };

    //class battleStsScreenUI : public statusScreenUI {
    //public:
    //    void init( u8 p_current ) override;
    //    void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
    //    void draw( u8 p_current ) override;
    //};
}