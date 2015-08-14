/*
Pokémon Emerald 2 Version
------------------------------

file        : mapSprite.cpp
author      : Philip Wellnitz
description : Map Sprites.

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

#include "mapSprite.h"
#include "sprite.h"
#include "uio.h"

namespace MAP {
    mapSprite::mapSprite( u16 p_imageId, u8 p_frameStart, u8 p_frame, u8 p_frameCount, bool p_isBig )
        : _picNum( p_imageId ), _frameStart( p_frameStart ), _frame( p_frame ), _frameCount( p_frameCount ) {

    }

    void mapSprite::setVisibility( bool p_value ) {
        IO::OamTop->oamBuffer[ _oamIndex ].isHidden = !p_value;
        IO::updateOAM( IO::OamTop );
    }

    void mapSprite::setFrame( u8 p_value ) {
        _frame = p_value;

        IO::updateOAM( IO::OamTop );
    }

    void mapSprite::nextFrame( ) {
        _frame = ( ( _frame + 1 - _frameStart ) % _frameCount ) + _frameStart;
        setFrame( _frame );
    }
}