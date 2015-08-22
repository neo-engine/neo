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
#include "fs.h"

#include "messageBox.h"

namespace MAP {
    const char* OW_PATH = "nitro:/PICS/SPRITES/OW/";
    //void loadframe( IO::SpriteInfo* p_si, int p_idx, int p_frame, bool p_big = false ) {
    //    char buffer[ 50 ];
    //    sprintf( buffer, "%i/%i", p_idx, p_frame );
    //    if( !p_big )
    //        FS::readSpriteData( p_si, "nitro:/PICS/SPRITES/OW/", buffer, 64, 16 );
    //    else
    //        FS::readSpriteData( p_si, "nitro:/PICS/SPRITES/OW/", buffer, 128, 16 );
    //}

    mapSprite::mapSprite( u16 p_currX, u16 p_currY,
                          u16 p_imageId,
                          u8 p_startFrame,
                          bool p_isBig,
                          u8 p_oamIdx, u8 p_palIdx, u16 p_tileIdx )
                          :_picNum( p_imageId ),
                          _curFrame( p_startFrame ),
                          _oamIndex( p_oamIdx ), _palette( p_palIdx ), _tileIdx( p_tileIdx ) {
        if( !IO::loadOWSprite( OW_PATH, _picNum, _curFrame, p_currX, p_currY, p_oamIdx, p_palIdx, p_tileIdx ) ) {
            IO::messageBox m( "Sprite failed" );
            IO::drawSub( true );
        }
        IO::updateOAM( false );
    }

    void mapSprite::setVisibility( bool p_value ) {
        IO::OamTop->oamBuffer[ _oamIndex ].isHidden = !p_value;
        IO::updateOAM( false );
    }

    void mapSprite::setFrame( u8 p_value ) {
        _curFrame = p_value;
        IO::loadOWSprite( OW_PATH, _picNum, _curFrame, IO::OamTop->oamBuffer[ _oamIndex ].x,
                          IO::OamTop->oamBuffer[ _oamIndex ].y, _oamIndex, _palette, _tileIdx );
        IO::updateOAM( false );
    }

    void mapSprite::nextFrame( ) {
        _curFrame++;
        if( _curFrame % 3 == 0 )
            _curFrame -= 2;
        setFrame( _curFrame );
    }

    void mapSprite::move( mapSlice::direction p_direction, s16 p_amount ) {
        IO::OamTop->oamBuffer[ _oamIndex ].x += p_amount * dir[ p_direction ][ 0 ];
        IO::OamTop->oamBuffer[ _oamIndex ].y += p_amount * dir[ p_direction ][ 1 ];
        IO::updateOAM( false );
    }
}