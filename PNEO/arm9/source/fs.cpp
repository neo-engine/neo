/*
Pokémon neo
------------------------------

file        : fs.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

#include <fat.h>
#include <sys/stat.h>

#include "bag/item.h"
#include "battle/ability.h"
#include "battle/battleTrainer.h"
#include "battle/move.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/bgmNames.h"
#include "gen/pokemonFormes.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "pokemon.h"

#ifndef NO_SOUND
#include "sound/sseq.h"
#endif

namespace FS {
    char TMP_BUFFER[ 100 ];

    bool checkOrOpen( FILE*& p_f, const char* p_path ) {
        if( p_f == nullptr ) { p_f = fopen( p_path, "rb" ); }
        if( p_f == nullptr ) { return false; }
        return true;
    }
    bool checkOrOpen( FILE*& p_f, const char* p_path, u8& p_lastLang, u8 p_language ) {
        if( p_f == nullptr || p_language != p_lastLang ) {
            // open the bank file
            if( p_f != nullptr ) { fclose( p_f ); }
            p_f        = FS::openBank( p_path, p_language, ".strb" );
            p_lastLang = p_language;
        }
        if( p_f == nullptr ) { return false; }
        return true;
    }

    bool SD_ACCESSED = false, SD_READ = false;
    bool SDFound( ) {
        if( !SD_ACCESSED ) {
            SD_READ     = !access( "sd:/", F_OK );
            SD_ACCESSED = true;
        }
        return SD_READ;
    }

    bool FC_ACCESSED = false, FC_READ = false;
    bool FCFound( ) {
        if( !FC_ACCESSED ) {
            FC_READ     = !access( "fat:/", F_OK );
            FC_ACCESSED = true;
        }
        return FC_READ;
    }

    bool readFsInfo( ) {
        FILE* f = fopen( "nitro:/fsinfo", "rb" );
        if( !f ) { return false; }
        fread( &FSDATA, sizeof( fsdataInfo ), 1, f );
        fclose( f );
        return true;
    }

    FILE* open( const char* p_path, const char* p_name, const char* p_ext, const char* p_mode ) {
        snprintf( TMP_BUFFER, 99, "%s%s%s", p_path, p_name, p_ext );
        return fopen( TMP_BUFFER, p_mode );
    }
    FILE* open( const char* p_path, u16 p_value, const char* p_ext, const char* p_mode ) {
        snprintf( TMP_BUFFER, 99, "%s%d%s", p_path, p_value, p_ext );
        return fopen( TMP_BUFFER, p_mode );
    }
    FILE* openSplit( const char* p_path, u16 p_value, const char* p_ext, u16 p_maxValue,
                     const char* p_mode ) {
        if( p_maxValue < 10 * ITEMS_PER_DIR ) {
            snprintf( TMP_BUFFER, 99, "%s%d/%d%s", p_path, p_value / ITEMS_PER_DIR, p_value,
                      p_ext );
        } else if( p_maxValue < 100 * ITEMS_PER_DIR ) {
            snprintf( TMP_BUFFER, 99, "%s%02d/%d%s", p_path, p_value / ITEMS_PER_DIR, p_value,
                      p_ext );
        } else {
            snprintf( TMP_BUFFER, 99, "%s%03d/%d%s", p_path, p_value / ITEMS_PER_DIR, p_value,
                      p_ext );
        }

        return fopen( TMP_BUFFER, p_mode );
    }

    FILE* openBank( const char* p_path, u8 p_lang, const char* p_ext, const char* p_mode ) {
        snprintf( TMP_BUFFER, 99, "%s.%hhu%s", p_path, p_lang, p_ext );
        return fopen( TMP_BUFFER, p_mode );
    }

    void close( FILE* p_file ) {
        fclose( p_file );
    }
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        auto res = fread( p_buffer, p_size, p_count, p_stream );
        DC_FlushRange( p_buffer, p_size * p_count );
        return res;
    }
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fwrite( p_buffer, p_size, p_count, p_stream );
    }

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt,
                   unsigned short* p_data ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        read( fd, p_data, sizeof( unsigned short ), p_dataCnt );
        close( fd );
        return true;
    }

    bool readNop( FILE* p_file, u32 p_cnt ) {
        u8 tmp;
        for( u32 i = 0; i < p_cnt; ++i )
            if( !read( p_file, &tmp, sizeof( u8 ), 1 ) ) return false;
        return true;
    }

    bool readSave( const char* p_path ) {
#ifndef FLASHCARD
        CARD::readData( 0, reinterpret_cast<u8*>( &SAVE::SAV ), sizeof( SAVE::saveGame ) );
        if( SAVE::SAV.isGood( ) ) { return true; }
#endif

        FILE* f = FS::open( p_path, "PNEO", ".sav", "r" );
        if( f != nullptr ) {
            fread( &SAVE::SAV, sizeof( SAVE::saveGame ), 1, f );
            fclose( f );
            return SAVE::SAV.isGood( );
        }
        return false;
    }

    bool writeSave( const char* p_path ) {
        return writeSave( p_path, []( u16, u16 ) {} );
    }

    bool writeSave( const char* p_path, std::function<void( u16, u16 )> p_progress ) {

        auto oldl = SAVE::SAV.getActiveFile( ).m_lastSaveLocation;
        auto oldd = SAVE::SAV.getActiveFile( ).m_lastSaveDate;
        auto oldt = SAVE::SAV.getActiveFile( ).m_lastSaveTime;

        SAVE::SAV.getActiveFile( ).m_lastSaveLocation = MAP::curMap->getCurrentLocationId( );
        SAVE::SAV.getActiveFile( ).m_lastSaveDate     = SAVE::CURRENT_DATE;
        SAVE::SAV.getActiveFile( ).m_lastSaveTime     = SAVE::CURRENT_TIME;

#ifndef FLASHCARD
        if( CARD::writeData( reinterpret_cast<u8*>( &SAVE::SAV ), sizeof( SAVE::saveGame ),
                             p_progress ) ) {
            return true;
        }
#else
        (void) p_progress;
#endif

        FILE* f = FS::open( p_path, "PNEO", ".sav", "w" );
        if( f != nullptr ) {
            FS::write( f, &SAVE::SAV, sizeof( SAVE::saveGame ), 1 );
            FS::close( f );
            return true;
        }
        FS::close( f );
        SAVE::SAV.getActiveFile( ).m_lastSaveLocation = oldl;
        SAVE::SAV.getActiveFile( ).m_lastSaveDate     = oldd;
        SAVE::SAV.getActiveFile( ).m_lastSaveTime     = oldt;
        return false;
    }
} // namespace FS
