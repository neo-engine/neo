/*
Pokémon Emerald 2 Version
------------------------------

file        : fs.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

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

#include <string>

#include <nds.h>

#include "../io/sprite.h"
#include "../io/font.h"
#include "saveGame.h"

namespace FS {
    extern unsigned int NAV_DATA[ 12288 ];
    extern unsigned short NAV_DATA_PAL[ 256 ];

#define MAXITEMSPERPAGE 12

    std::string readString( FILE*, bool p_new = false );
    std::wstring readWString( FILE*, bool p_new = false );

    template<typename T>
    bool readData( const char* p_path, const char* p_name, const T p_dataCnt, T* p_data );
    template<typename T1, typename T2>
    bool readData( const char* p_path, const char* p_name, const T1 p_dataCnt1, T1* p_data1, const T2 p_dataCnt2, T2* p_data2 );

    std::string breakString( const std::string& p_string, u8 p_lineLength );
    std::string breakString( const std::string& p_string, IO::font* p_font, u8 p_lineLength );

    bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256 );
    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt );

    const char* getLoc( u16 p_ind );
}