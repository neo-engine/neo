/*
Pokémon Emerald 2 Version
------------------------------

file        : choiceBox.h
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

#include <nds.h>
#include "pokemon.h"

namespace IO {
#define MAX_CHOICES_PER_PAGE 3
#define MAX_CHOICES_PER_SMALL_PAGE 6
    class choiceBox {
    public:
        choiceBox( int p_num, const char** p_choices, const char* p_name, bool p_big );
        choiceBox( pokemon p_pokemon, u16 p_moveToLearn );

        void draw( u8 p_pressedIdx );
        int getResult( const char* p_text = 0, bool p_backButton = false, bool p_drawSub = true );
        void kill( );
    private:
        const char** _choices;
        bool _big;
        u8 _num;
        const char* _name;
        const char* _text;
        u8 _acPage;

        bool _drawSub;
    };
}
