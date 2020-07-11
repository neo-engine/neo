/*
Pokémon neo
------------------------------

file        : fs.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.


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

    bool   exists( const char* p_path );
    bool   exists( const char* p_path, const char* p_name );
    bool   exists( const char* p_path, u16 p_pkmnIdx, const char* p_name = "" );
    FILE*  open( const char* p_path, const char* p_name, const char* p_ext = ".raw",
                 const char* p_mode = "rb" );
    FILE*  open( const char* p_path, u16 p_value, const char* p_ext = ".raw",
                 const char* p_mode = "rb" );
    FILE*  openSplit( const char* p_path, u16 p_value, const char* p_ext = ".raw",
                      u16 p_maxValue = 99 * ITEMS_PER_DIR, const char* p_mode = "rb" );
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
    bool readPal( FILE* p_file, MAP::palette* p_palette );
    bool readTiles( FILE* p_file, MAP::tile* p_tiles, u16 p_startIdx = 0, u16 p_size = 512 );
    bool readBlocks( FILE* p_file, MAP::block* p_blocks, u16 p_startIdx = 0, u16 p_size = 512 );
    u8   readAnimations( FILE* p_file, MAP::tileSet::animation* p_animations );
    bool readBankData( u8 p_bank, MAP::bankInfo& p_result );

    FILE* openScript( MAP::warpPos p_pos, u8 p_id );
    FILE* openScript( u8 p_map, u16 p_globX, u16 p_globY, u8 p_id );
    FILE* openScript( u8 p_bank, u8 p_mapX, u8 p_mapY, u8 p_relX, u8 p_relY, u8 p_id );

    u8*  readCry( u16 p_pkmnIdx, u8 p_forme, u16& p_len );
    u8* readSFX( u16 p_sfxID, u16& p_len );

    // bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name,
                          u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256,
                          bool p_bottom = false );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize,
                          u16 p_palStart, u32 p_tileCnt, bool p_bottom );
    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name,
                         const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom = false );

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
