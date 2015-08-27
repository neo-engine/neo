/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSprite.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
#include "mapDefines.h"

namespace MAP {

    class mapSprite {
    private:

        u8 _oamIndex;
        u8 _palette;
        u16 _tileIdx;

        u16 _picNum;
        u8 _curFrame;

    public:
        mapSprite( ) { }
        mapSprite( u16 p_currX, u16 p_currY,
                   u16 p_imageId,
                   u8 p_startFrame,
                   u8 p_oamIdx, u8 p_palIdx, u16 p_tileIdx );

        void setVisibility( bool p_value );
        void drawFrame( u8 p_value );
        void drawFrame( u8 p_value, bool p_hFlip );
        void setFrame( u8 p_value );
        void currentFrame( );
        void nextFrame( );
        void move( direction p_direction, s16 p_amount );
    };
}