/*
Pokémon neo
------------------------------

file        : fs.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.


Copyright (C) 2012 - 2021
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

#pragma once

#include <functional>
#include <string>

#include <nds.h>

#include "font.h"
#include "mapSlice.h"
#include "saveGame.h"
#include "sprite.h"

namespace FS {
#define MAXITEMSPERPAGE 12
    const u16 ITEMS_PER_DIR = 30;

    bool SDFound( );
    bool FCFound( );

    FILE*  open( const char* p_path, const char* p_name, const char* p_ext = ".raw",
                 const char* p_mode = "rb" );
    FILE*  open( const char* p_path, u16 p_value, const char* p_ext = ".raw",
                 const char* p_mode = "rb" );
    FILE*  openSplit( const char* p_path, u16 p_value, const char* p_ext = ".raw",
                      u16 p_maxValue = 99 * ITEMS_PER_DIR, const char* p_mode = "rb" );
    FILE*  openBank( const char* p_path, u8 p_lang, const char* p_ext = ".strb",
                     const char* p_mode = "rb" );
    void   close( FILE* p_file );
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count );
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count );

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt,
                   unsigned short* p_data );
    template <typename T1, typename T2>
    bool readData( const char* p_path, const char* p_name, T1 p_dataCnt1, T1* p_data1,
                   T2 p_dataCnt2, T2* p_data2 ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        read( fd, p_data1, sizeof( T1 ), p_dataCnt1 );
        read( fd, p_data2, sizeof( T2 ), p_dataCnt2 );
        close( fd );
        return true;
    }

    bool readNop( FILE* p_file, u32 p_cnt );
    bool readPal( FILE* p_file, MAP::palette* p_palette, u8 p_count = 6 );
    bool readTiles( FILE* p_file, MAP::tile* p_tiles, u16 p_startIdx = 0, u16 p_size = 512 );
    bool readBlocks( FILE* p_file, MAP::block* p_blocks, u16 p_startIdx = 0, u16 p_size = 512 );

    bool seekTileSet( FILE* p_file, u8 p_tsIdx );

    FILE* openTileSet( );
    FILE* openBank( u8 p_bank );

    bool readMapData( FILE* p_file, MAP::mapData* p_result, bool p_close = true );

    bool readMapSlice( FILE* p_mapFile, MAP::mapSlice* p_result, u16 p_x = 0, u16 p_y = 0,
                       bool p_close = true );

    u32 readMapSliceAndData( FILE* p_mapFile, MAP::mapSlice* p_slice, MAP::mapData* p_data, u16 p_x,
                             u16 p_y );

    FILE* openScript( u16 p_scriptId );

    u8* readCry( u16 p_pkmnIdx, u8 p_forme, u16& p_len );
    u8* readSFX( u16 p_sfxID, u16& p_len );

    u16 frameForLocation( u16 p_locationId );
    u16 mugForLocation( u16 p_locationId );
    u16 BGMforLocation( u16 p_locationId );

    // bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name,
                          u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256,
                          bool p_bottom = false );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize,
                          u16 p_palStart, u32 p_tileCnt, bool p_bottom );
    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name,
                         const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom = false );

    std::string getBGMName( u16 p_locationId );
    std::string getBGMName( u16 p_locationId, u8 p_language );
    bool        getBGMName( u16 p_locationId, u8 p_language, char* p_out );

    std::string getLocation( u16 p_locationId );
    std::string getLocation( u16 p_locationId, u8 p_language );
    bool        getLocation( u16 p_locationId, u8 p_language, char* p_out );

    bool readSave( const char* p_path );
    bool writeSave( const char* p_path );
    bool writeSave( const char* p_path, std::function<void( u16, u16 )> p_progress );

    namespace CARD {
        bool checkCard( );

        void waitBusy( );
        void waitWriteInProgress( );
        u8   transfer( u8 p_data );
        void write( u8 p_data );

        void readData( u32 p_address, u8* p_out, u32 p_cnt );
        bool writeData( u8* p_data, u32 p_cnt, std::function<void( u16, u16 )> p_progress );
        bool writeData( u32 p_addressr, u8* p_data, u32 p_cnt );
    } // namespace CARD
} // namespace FS
