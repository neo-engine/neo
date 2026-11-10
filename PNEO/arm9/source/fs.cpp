/*
Pokémon neo
------------------------------

file        : fs.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2026
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fat.h>
#include <fs/card.h>
#include <sys/stat.h>

#include "bag/item.h"
#include "battle/ability.h"
#include "battle/battleTrainer.h"
#include "battle/move.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/bgmNames.h"
#include "gen/pokemonFormes.h"
#include "io/util.h"
#include "map/mapDrawer.h"
#include "pokemon.h"

#ifndef NO_SOUND
#include "sound/sseq.h"
#endif

namespace FS {
    const char* OW_NPC_BANK_PATH    = "nitro:/PICS/SPRITES/npc.rsdb";
    const char* BERRY_BANK_PATH     = "nitro:/PICS/SPRITES/berry.rsdb";
    const char* NPCP_BANK_PATH[ 4 ] = {
        "nitro:/PICS/SPRITES/NPCP.rsdb",
        "nitro:/PICS/SPRITES/NPCPs.rsdb",
        "nitro:/PICS/SPRITES/NPCPf.rsdb",
        "nitro:/PICS/SPRITES/NPCPfs.rsdb",
    };

    char TMP_BUFFER[ 100 ];

    compressedSpriteBank NPC_BANK, BERRY_BANK, NPCP_BANK[ 4 ];

    u8 SPR_SCRATCH[ 0x5A00 ] __attribute__( ( aligned( 4 ) ) ) = { 0 };

    bool isMemoryStream( FILE* p_f ) {
        return p_f == NPC_BANK.m_memoryFile || p_f == BERRY_BANK.m_memoryFile
               || p_f == NPCP_BANK[ 0 ].m_memoryFile || p_f == NPCP_BANK[ 1 ].m_memoryFile
               || p_f == NPCP_BANK[ 2 ].m_memoryFile || p_f == NPCP_BANK[ 3 ].m_memoryFile;
    }

    long fileSize( FILE* p_f ) {
        if( fseek( p_f, 0, SEEK_END ) ) { return -1; }
        long sz = ftell( p_f );
        fseek( p_f, 0, SEEK_SET );
        return sz;
    }

    FILE* openSpriteBank( compressedSpriteBank& p_bank, const char* p_path, u32 p_id ) {
        if( !p_bank.m_bankFile ) {
            p_bank.m_bankFile = fopen( p_path, "rb" );
            if( !p_bank.m_bankFile ) { return nullptr; }
            if( fread( &p_bank.m_recordSize, 4, 1, p_bank.m_bankFile ) != 1
                || p_bank.m_recordSize < 8 || p_bank.m_recordSize > 0x5000 ) {
                fclose( p_bank.m_bankFile );
                p_bank.m_bankFile = nullptr;
                return nullptr;
            }
        }

        if( ( COMPRESSION & COMPRESS_SPRITE ) == 0 ) {
            if( fseek( p_bank.m_bankFile, 4 + p_id * p_bank.m_recordSize, SEEK_SET ) ) {
                return nullptr;
            }
            return p_bank.m_bankFile;
        }

        u32 entry[ 2 ] = { 0, 0 };
        if( fseek( p_bank.m_bankFile, 4 + p_id * 8, SEEK_SET )
            || fread( entry, 8, 1, p_bank.m_bankFile ) != 1 ) {
            return nullptr;
        }

        long sz = fileSize( p_bank.m_bankFile );
        if( sz < 0 || entry[ 1 ] < 4 || entry[ 1 ] > sizeof( SPR_SCRATCH )
            || (long) entry[ 0 ] + (long) entry[ 1 ] > sz ) {
            return nullptr;
        }
        if( fseek( p_bank.m_bankFile, entry[ 0 ], SEEK_SET )
            || fread( SPR_SCRATCH, 1, entry[ 1 ], p_bank.m_bankFile ) != entry[ 1 ] ) {
            return nullptr;
        }
        if( SPR_SCRATCH[ 0 ] != 0x10 ) { return nullptr; }
        u32 dsize = (u32) SPR_SCRATCH[ 1 ] | ( (u32) SPR_SCRATCH[ 2 ] << 8 )
                    | ( (u32) SPR_SCRATCH[ 3 ] << 16 );
        if( dsize != p_bank.m_recordSize ) { return nullptr; }
        if( !p_bank.m_recordBuffer ) {
            p_bank.m_recordBuffer = (u8*) malloc( p_bank.m_recordSize );
            if( p_bank.m_recordBuffer ) {
                p_bank.m_memoryFile = fmemopen( p_bank.m_recordBuffer, p_bank.m_recordSize, "rb" );
            }
            if( !p_bank.m_memoryFile ) {
                free( p_bank.m_recordBuffer );
                p_bank.m_recordBuffer = nullptr;
                return nullptr;
            }
        }
        swiDecompressLZSSWram( SPR_SCRATCH, p_bank.m_recordBuffer );
        if( fseek( p_bank.m_memoryFile, 0, SEEK_SET ) ) { return nullptr; }
        return p_bank.m_memoryFile;
    }

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

    FILE* openNPCBank( u16 p_imageId ) {
        return openSpriteBank( NPC_BANK, OW_NPC_BANK_PATH, p_imageId );
    }

    FILE* openBerryBank( u16 p_imageId ) {
        return openSpriteBank( BERRY_BANK, BERRY_BANK_PATH, p_imageId );
    }

    FILE* openNPCPBank( u16 p_imageId, bool p_shiny, bool p_female ) {
        auto idx = 2 * p_female + p_shiny;
        return openSpriteBank( NPCP_BANK[ idx ], NPCP_BANK_PATH[ idx ], p_imageId );
    }

    void close( FILE* p_file ) {
        fclose( p_file );
    }
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) { return 0; }
        auto res = fread( p_buffer, p_size, p_count, p_stream );
        if( isMemoryStream( p_stream ) ) {
            DC_FlushRange( p_buffer, p_size * p_count );
        } else {
            DC_InvalidateRange( p_buffer, p_size * p_count );
        }
        return res;
    }
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) { return 0; }
        DC_FlushRange( p_buffer, p_size * p_count );
        return fwrite( p_buffer, p_size, p_count, p_stream );
    }

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt,
                   unsigned short* p_data ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) { return false; }
        read( fd, p_data, sizeof( unsigned short ), p_dataCnt );
        close( fd );
        return true;
    }

    u8 RL_SCRATCH[ 0x10200 ] __attribute__( ( aligned( 4 ) ) ) = { 0 };
    u8 RL_DECOMP[ 0x10200 ] __attribute__( ( aligned( 4 ) ) )  = { 0 };

    inline u32 rlDecompressedSize( const u8* p_hdr ) {
        return (u32) p_hdr[ 1 ] | ( (u32) p_hdr[ 2 ] << 8 ) | ( (u32) p_hdr[ 3 ] << 16 );
    }

    bool readThrough( FILE* p_f, u8 p_category, u32 p_rasterBytes, void* p_p1, u32 p_palBytes,
                      void* p_p2 ) {
        if( !p_f ) { return false; }

        if( ( COMPRESSION & p_category ) == 0 ) {
            read( p_f, p_p1, p_rasterBytes, 1 );
            read( p_f, p_p2, p_palBytes, 1 );
            return true;
        }

        long csize = fileSize( p_f );
        if( csize < 4 || csize > (long) sizeof( RL_SCRATCH ) ) { return false; }

        if( read( p_f, RL_SCRATCH, 1, (size_t) csize ) != (size_t) csize ) { return false; }
        if( RL_SCRATCH[ 0 ] != 0x30 ) { return false; }

        u32 dsize = rlDecompressedSize( RL_SCRATCH );
        if( dsize != p_rasterBytes + p_palBytes ) { return false; }
        if( dsize > (u32) sizeof( RL_DECOMP ) ) { return false; }

        swiDecompressRLEWram( RL_SCRATCH, RL_DECOMP );

        memcpy( p_p1, RL_DECOMP, p_rasterBytes );
        memcpy( p_p2, RL_DECOMP + p_rasterBytes, p_palBytes );
        DC_FlushRange( p_p1, p_rasterBytes );
        DC_FlushRange( p_p2, p_palBytes );
        return true;
    }

    bool readNop( FILE* p_file, u32 p_cnt ) {
        if( !p_file ) { return false; }
        constexpr u32 CHUNK = 256;
        u8            buf[ CHUNK ];
        while( p_cnt > 0 ) {
            u32 n = p_cnt < CHUNK ? p_cnt : CHUNK;
            if( read( p_file, buf, 1, n ) != n ) { return false; }
            p_cnt -= n;
        }
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

        auto oldl = SAVE::CURRENT_FILE->m_lastSaveLocation;
        auto oldd = SAVE::CURRENT_FILE->m_lastSaveDate;
        auto oldt = SAVE::CURRENT_FILE->m_lastSaveTime;

        SAVE::CURRENT_FILE->m_lastSaveLocation = MAP::curMap->getCurrentLocationId( );
        SAVE::CURRENT_FILE->m_lastSaveDate     = SAVE::CURRENT_DATE;
        SAVE::CURRENT_FILE->m_lastSaveTime     = SAVE::CURRENT_TIME;

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
        SAVE::CURRENT_FILE->m_lastSaveLocation = oldl;
        SAVE::CURRENT_FILE->m_lastSaveDate     = oldd;
        SAVE::CURRENT_FILE->m_lastSaveTime     = oldt;
        return false;
    }
} // namespace FS
