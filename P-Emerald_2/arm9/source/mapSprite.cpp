/*
Pokémon neo
------------------------------

file        : mapSprite.cpp
author      : Philip Wellnitz
description : Map Sprites.

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mapSprite.h"
#include "fs.h"
#include "nav.h"
#include "uio.h"

namespace MAP {
    mapSprite::mapSprite( u16 p_currX, u16 p_currY, u16 p_imageId, u8 p_startFrame, u8 p_oamIdx,
                          u16 p_tileIdx )
        : _oamIndex( p_oamIdx ), _picNum( p_imageId ), _curFrame( p_startFrame ) {
        if( !IO::loadOWSpriteB( _picNum, p_currX, p_currY, _oamIndex, p_tileIdx, _palData,
                                _frameData ) ) {
#ifdef DESQUID
            NAV::printMessage( "Sprite failed" );
#endif
        }
        drawFrame( p_startFrame );
    }

    ObjPriority mapSprite::getPriority( ) {
        return IO::OamTop->oamBuffer[ _oamIndex ].priority;
    }

    void mapSprite::setVisibility( bool p_value ) {
        IO::OamTop->oamBuffer[ _oamIndex ].isHidden = !p_value;
        IO::updateOAM( false );
    }
    void mapSprite::setPriority( ObjPriority p_value ) {
        IO::OamTop->oamBuffer[ _oamIndex ].priority = p_value;
        IO::updateOAM( false );
    }
    void mapSprite::drawFrame( u8 p_value ) {
        u8 frame = p_value;
        if( frame % PLAYER_FAST >= 9 ) frame -= 3;
        if( p_value % PLAYER_FAST == 15 ) frame--;

        drawFrame( frame / PLAYER_FAST * 9 + frame % PLAYER_FAST,
                   ( frame != p_value )
                       && ( p_value % PLAYER_FAST < 12 || p_value % PLAYER_FAST == 15 ) );
    }
    void mapSprite::drawFrame( u8 p_value, bool p_hFlip ) {
        IO::setOWSpriteFrame( p_value, p_hFlip, _oamIndex, _palData, _frameData );
        IO::updateOAM( false );
    }

    void mapSprite::setFrame( u8 p_value ) {
        _curFrame = p_value;
        drawFrame( _curFrame );
    }

    void mapSprite::currentFrame( ) {
        drawFrame( _curFrame );
    }
    void mapSprite::nextFrame( ) {
        if( ( ( ++_curFrame ) % PLAYER_FAST ) % 3 == 0 ) _curFrame -= 2;
        drawFrame( _curFrame );
    }

    void mapSprite::move( direction p_direction, s16 p_amount ) {
        IO::OamTop->oamBuffer[ _oamIndex ].x += p_amount * dir[ p_direction ][ 0 ];
        IO::OamTop->oamBuffer[ _oamIndex ].y += p_amount * dir[ p_direction ][ 1 ];
        IO::updateOAM( false );
    }
} // namespace MAP
