/*
Pokémon Emerald 2 Version
------------------------------

file        : yesNoBox.h
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

#include "messageBox.h"
#include "saveGame.h"

namespace IO {
    class yesNoBox {
    public:
        yesNoBox( bool p_initSprites = true );
        yesNoBox( SAVE::language p_language, bool p_initSprites = true );
        yesNoBox( const char* p_name, bool p_initSprites = true );
        yesNoBox( messageBox p_box, bool p_initSprites = true );

        bool getResult( const char* p_text );

        void draw( u8 p_pressedIdx );

    private:
        bool _isNamed;
        SAVE::language _language;
    };

}
