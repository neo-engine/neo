/*
Pokémon Emerald 2 Version
------------------------------

file        : fs.h
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

#include <string>

#include <nds.h>

#include "sprite.h"
#include "font.h"
#include "saveGame.h"
#include "mapSlice.h"

namespace FS {
#define MAXITEMSPERPAGE 12

    std::string readString( FILE*, bool p_new = false );

    bool exists( const char* p_path, const char* p_name );
    bool exists( const char* p_path, u16 p_name, bool p_unused );
    bool exists( const char* p_path, u16 p_pkmnIdx, const char* p_name = "" );
    FILE* open( const char* p_path, const char* p_name, const char* p_ext = ".raw", const char* p_mode = "r" );
    FILE* open( const char* p_path, u16 p_value, const char* p_ext = ".raw", const char* p_mode = "r" );
    void close( FILE* p_file );
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count );
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count );

    // No, I'm absolutely not aware of templates.
    bool readData( const char* p_path, const char* p_name, const unsigned short p_dataCnt, unsigned short* p_data );
    bool readData( const char* p_path, const char* p_name, const unsigned int p_dataCnt1, unsigned int* p_data1, const unsigned short p_dataCnt2, unsigned short* p_data2 );
    bool readData( const char* p_path, const char* p_name, const unsigned short p_dataCnt1, unsigned short* p_data1, const unsigned int p_dataCnt2, unsigned int* p_data2 );

    bool readNop( FILE* p_file, u32 p_cnt );
    bool readPal( FILE* p_file, MAP::palette* p_palette );
    bool readTiles( FILE* p_file, MAP::tile* p_tiles, u16 p_startIdx = 0, u16 p_size = 512 );
    bool readBlocks( FILE* p_file, MAP::block* p_blocks, u16 p_startIdx = 0, u16 p_size = 512 );
    u8   readAnimations( FILE* p_file, MAP::tileSet::animation* p_animations );

    std::string breakString( const std::string& p_string, u8 p_lineLength );
    std::string breakString( const std::string& p_string, IO::font* p_font, u8 p_lineLength );

    std::string convertToOld( const std::string& p_string );

    bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256, bool p_bottom = false );
    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom = false );

    const char* getLoc( u16 p_ind );
}