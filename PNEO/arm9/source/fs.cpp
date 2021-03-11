/*
Pokémon neo
------------------------------

file        : fs.cpp
author      : Philip Wellnitz
description :

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

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>

#include <fat.h>
#include <sys/stat.h>

#include "ability.h"
#include "battleTrainer.h"
#include "defines.h"
#include "fs.h"
#include "item.h"
#include "mapDrawer.h"
#include "move.h"
#include "nav.h"
#include "pokemon.h"
#include "uio.h"

const char PKMNDATA_PATH[] = "nitro:/PKMNDATA/";
const char SCRIPT_PATH[]   = "nitro:/DATA/MAP_SCRIPT/";

const char CRY_PATH[]              = "nitro:/SOUND/CRIES/";
const char SFX_PATH[]              = "nitro:/SOUND/SFX/";
const char LOCATION_NAME_PATH[]    = "nitro:/DATA/LOC_NAME/";
const char ITEM_NAME_PATH[]        = "nitro:/DATA/ITEM_NAME/";
const char ITEM_DSCR_PATH[]        = "nitro:/DATA/ITEM_DSCR/";
const char ITEM_DATA_PATH[]        = "nitro:/DATA/ITEM_DATA/";
const char ABILITY_NAME_PATH[]     = "nitro:/DATA/ABTY_NAME/";
const char ABILITY_DSCR_PATH[]     = "nitro:/DATA/ABTY_DSCR/";
const char MOVE_NAME_PATH[]        = "nitro:/DATA/MOVE_NAME/";
const char MOVE_DSCR_PATH[]        = "nitro:/DATA/MOVE_DSCR/";
const char MOVE_DATA_PATH[]        = "nitro:/DATA/MOVE_DATA/";
const char POKEMON_NAME_PATH[]     = "nitro:/DATA/PKMN_NAME/";
const char POKEMON_SPECIES_PATH[]  = "nitro:/DATA/PKMN_SPCS/";
const char POKEMON_DATA_PATH[]     = "nitro:/DATA/PKMN_DATA/";
const char POKEMON_EVOS_PATH[]     = "nitro:/DATA/PKMN_EVOS/";
const char POKEMON_DEXENTRY_PATH[] = "nitro:/DATA/PKMN_DXTR/";
const char PKMN_LEARNSET_PATH[]    = "nitro:/DATA/PKMN_LEARN/";

const char BATTLE_STRINGS_PATH[] = "nitro:/DATA/TRNR_STRS/";
const char BATTLE_TRAINER_PATH[] = "nitro:/DATA/TRNR_DATA/";
const char TCLASS_NAME_PATH[]    = "nitro:/DATA/TRNR_NAME/";

const char MAP_DATA_PATH[] = "nitro:/DATA/MAP_DATA/";

const char STRING_PATH[]      = "nitro:/STRN/STR/";
const char MAPSTRING_PATH[]   = "nitro:/STRN/MAP/";
const char BADGENAME_PATH[]   = "nitro:/STRN/BDG/";
const char ACHIEVEMENT_PATH[] = "nitro:/STRN/AVM/";

namespace FS {
    char TMP_BUFFER[ 100 ];
    char TMP_BUFFER_SHORT[ 50 ];
    u8   CRY_DATA[ 22050 * 2 ];

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

    bool exists( const char* p_path ) {
        FILE* fd  = fopen( p_path, "r" );
        bool  res = !!fd;
        fclose( fd );
        return res;
    }
    bool exists( const char* p_path, const char* p_name ) {
        FILE* fd  = open( p_path, p_name );
        bool  res = !!fd;
        fclose( fd );
        return res;
    }
    bool exists( const char* p_path, u16 p_pkmnIdx, const char* p_name ) {
        snprintf( TMP_BUFFER, 99, "%s%d/%d%s.raw", p_path, p_pkmnIdx, p_pkmnIdx, p_name );
        FILE* fd  = fopen( TMP_BUFFER, "rb" );
        bool  res = !!fd;
        fclose( fd );
        return res;
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

    void close( FILE* p_file ) {
        fclose( p_file );
    }
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fread( p_buffer, p_size, p_count, p_stream );
    }
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream ) return 0;
        return fwrite( p_buffer, p_size, p_count, p_stream );
    }

    FILE* openScript( u16 p_scriptId ) {
        return openSplit( SCRIPT_PATH, p_scriptId, ".mapscr", 10 * 30 );
    }

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt,
                   unsigned short* p_data ) {
        FILE* fd = open( p_path, p_name );
        if( !fd ) return false;
        read( fd, p_data, sizeof( unsigned short ), p_dataCnt );
        close( fd );
        return true;
    }

    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name,
                         const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, p_tileCnt, TEMP, p_palCnt,
                                                     TEMP_PAL ) )
            return false;

        IO::copySpriteData( TEMP, p_spriteInfo->m_entry->gfxIndex, 4 * p_tileCnt, p_bottom );
        IO::copySpritePal( TEMP_PAL, p_spriteInfo->m_entry->palette, 2 * p_palCnt, p_bottom );
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize,
                          u32 p_tileCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, 12288, TEMP, 256, TEMP_PAL ) )
            return false;

        if( p_tileCnt ) { dmaCopy( TEMP, p_layer, p_tileCnt ); }
        if( p_palSize ) {
            if( p_bottom )
                dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
            else
                dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
        }
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize,
                          u16 p_palStart, u32 p_tileCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, 12288, TEMP, 256, TEMP_PAL ) )
            return false;

        if( p_tileCnt ) { dmaCopy( TEMP, p_layer, p_tileCnt ); }
        if( p_palSize ) {
            if( p_bottom )
                dmaCopy( TEMP_PAL + p_palStart, BG_PALETTE_SUB + p_palStart, p_palSize );
            else
                dmaCopy( TEMP_PAL + p_palStart, BG_PALETTE + p_palStart, p_palSize );
        }
        return true;
    }

    u8* readCry( u16 p_pkmnIdx, u8 p_forme, u16& p_len ) {
        FILE* f;

        if( p_forme ) {
            snprintf( TMP_BUFFER_SHORT, 49, "_%hhu.raw", p_forme );
            f = FS::openSplit( CRY_PATH, p_pkmnIdx, ".raw", MAX_PKMN );
        }
        if( !p_forme || !f ) { f = FS::openSplit( CRY_PATH, p_pkmnIdx, ".raw", MAX_PKMN ); }
        if( !f ) { return nullptr; }

        std::memset( CRY_DATA, 0, sizeof( CRY_DATA ) );
        if( !( p_len = read( f, CRY_DATA, sizeof( u8 ), sizeof( CRY_DATA ) ) ) ) { return nullptr; }
        return CRY_DATA;
    }

    u8* readSFX( u16 p_sfxIdx, u16& p_len ) {
        FILE* f = FS::openSplit( SFX_PATH, p_sfxIdx, ".raw", 400 );
        if( !f ) { return nullptr; }

        std::memset( CRY_DATA, 0, sizeof( CRY_DATA ) );
        if( !( p_len = read( f, CRY_DATA, 1, sizeof( CRY_DATA ) ) ) ) { return nullptr; }
        return CRY_DATA;
    }

    /*
    bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no ) {
        if( p_no == SAVE::SAV.getActiveFile( ).m_options.m_bgIdx && NAV::NAV_DATA[ 0 ] ) {
            dmaCopy( NAV::NAV_DATA, p_layer, 256 * 192 );
            dmaCopy( NAV::NAV_DATA_PAL, IO::BG_PAL( !SCREENS_SWAPPED ), 192 * 2 );
            return true;
        }

        if( !readData( "nitro:/PICS/NAV/", p_name, (unsigned int) 12288, NAV::NAV_DATA,
                       (unsigned short) 192, NAV::NAV_DATA_PAL ) )
            return false;

        dmaCopy( NAV::NAV_DATA, p_layer, 256 * 192 );
        dmaCopy( NAV::NAV_DATA_PAL, IO::BG_PAL( !SCREENS_SWAPPED ), 192 * 2 );

        return true;
    }
    */

    bool readNop( FILE* p_file, u32 p_cnt ) {
        u8 tmp;
        for( u32 i = 0; i < p_cnt; ++i )
            if( !read( p_file, &tmp, sizeof( u8 ), 1 ) ) return false;
        return true;
    }

    bool readPal( FILE* p_file, MAP::palette* p_palette, u8 p_count ) {
        if( p_file == 0 ) return false;
        read( p_file, p_palette, sizeof( u16 ) * 16, p_count );
        return true;
    }

    bool readTiles( FILE* p_file, MAP::tile* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        read( p_file, p_tileSet + p_startIdx, sizeof( MAP::tile ) * p_size, 1 );
        return true;
    }

    bool readBlocks( FILE* p_file, MAP::block* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 ) return false;
        readNop( p_file, 4 );
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottom, 4 * sizeof( MAP::blockAtom ),
                  1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_top, 4 * sizeof( MAP::blockAtom ), 1 );
        }
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottombehave, sizeof( u8 ), 1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_topbehave, sizeof( u8 ), 1 );
        }
        return true;
    }

    bool readBankData( u8 /* p_bank */, MAP::bankInfo& /*p_result*/ ) {
        return false;

        /*
        snprintf( TMP_BUFFER_SHORT, 45, "%hhu/%hhu", p_bank, p_bank );
        FILE* f = open( MAP::MAP_PATH, TMP_BUFFER_SHORT, ".bnk" );
        if( !f ) return false;
        read( f, &p_result.m_locationId, sizeof( u16 ), 1 );
        close( f );
        return true;
        */
    }

    bool readMapData( u8 p_bank, u8 p_mapX, u8 p_mapY, MAP::mapData& p_result ) {
        snprintf( TMP_BUFFER_SHORT, 45, "%hhu/%hu/%hu_%hu", p_bank, p_mapY, p_mapY, p_mapX );
        FILE* f = open( MAP_DATA_PATH, TMP_BUFFER_SHORT, ".map.data" );
        if( !f ) {
            std::memset( &p_result, 0, sizeof( MAP::mapData ) );
            return false;
        }
        fread( &p_result, sizeof( MAP::mapData ), 1, f );
        close( f );
        return true;
    }

    bool getLocation( const u16 p_locationId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( LOCATION_NAME_PATH, p_locationId, ".str", 5000 );
        if( !f ) return false;

        std::fseek( f, p_language * LOCATION_NAMELENGTH, SEEK_SET );
        fread( p_out, 1, LOCATION_NAMELENGTH, f );
        fclose( f );
        return true;
    }
    std::string getLocation( const u16 p_locationId, const u8 p_language ) {
        char tmpbuf[ LOCATION_NAMELENGTH ];
        if( !getLocation( p_locationId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }
    std::string getLocation( const u16 p_locationId ) {
        return getLocation( p_locationId, CURRENT_LANGUAGE );
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

namespace ITEM {
    bool getItemName( const u16 p_itemId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( ITEM_NAME_PATH, p_itemId, ".str" );
        if( !f ) return false;

        std::fseek( f, p_language * ITEM_NAMELENGTH, SEEK_SET );
        fread( p_out, 1, ITEM_NAMELENGTH, f );
        fclose( f );
        return true;
    }
    std::string getItemName( const u16 p_itemId, const u8 p_language ) {
        char tmpbuf[ ITEM_NAMELENGTH ];
        if( !getItemName( p_itemId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }
    std::string getItemName( const u16 p_itemId ) {
        return getItemName( p_itemId, CURRENT_LANGUAGE );
    }

    bool getItemDescr( const u16 p_itemId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( ITEM_DSCR_PATH, p_itemId, ".str" );
        if( !f ) return false;

        std::fseek( f, p_language * ITEM_DSCRLENGTH, SEEK_SET );
        fread( p_out, 1, ITEM_DSCRLENGTH, f );
        fclose( f );
        return true;
    }

    std::string getItemDescr( const u16 p_itemId, const u8 p_language ) {
        char tmpbuf[ ITEM_DSCRLENGTH ];
        if( !getItemDescr( p_itemId, p_language, tmpbuf ) ) {
            return "---"
#ifdef DESQUID
                   + std::string( " moveid " ) + std::to_string( p_itemId ) + " lang "
                   + std::to_string( p_language )
#endif
                ;
        }
        return std::string( tmpbuf );
    }

    std::string getItemDescr( const u16 p_itemId ) {
        return getItemDescr( p_itemId, CURRENT_LANGUAGE );
    }

    itemData getItemData( const u16 p_itemId ) {
        itemData res;
        if( getItemData( p_itemId, &res ) ) { return res; }
        getItemData( 0, &res );
        return res;
    }
    bool getItemData( const u16 p_itemId, itemData* p_out ) {
        FILE* f = FS::openSplit( ITEM_DATA_PATH, p_itemId, ".data" );
        if( !f ) return false;
        fread( p_out, sizeof( itemData ), 1, f );
        fclose( f );
        return true;
    }
} // namespace ITEM

namespace MOVE {
    bool getMoveName( const u16 p_moveId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( MOVE_NAME_PATH, p_moveId, ".str" );
        if( !f ) return false;

        std::fseek( f, p_language * MOVE_NAMELENGTH, SEEK_SET );
        fread( p_out, 1, MOVE_NAMELENGTH, f );
        fclose( f );
        return true;
    }

    std::string getMoveName( const u16 p_moveId, const u8 p_language ) {
        char tmpbuf[ MOVE_NAMELENGTH ];
        if( !getMoveName( p_moveId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }

    std::string getMoveName( const u16 p_moveId ) {
        return getMoveName( p_moveId, CURRENT_LANGUAGE );
    }

    bool getMoveDescr( const u16 p_moveId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( MOVE_DSCR_PATH, p_moveId, ".str" );
        if( !f ) return false;

        std::fseek( f, p_language * MOVE_DSCRLENGTH, SEEK_SET );
        fread( p_out, 1, MOVE_DSCRLENGTH, f );
        fclose( f );
        return true;
    }

    std::string getMoveDescr( const u16 p_moveId, const u8 p_language ) {
        char tmpbuf[ MOVE_DSCRLENGTH ];
        if( !getMoveDescr( p_moveId, p_language, tmpbuf ) ) {
            return "---"
#ifdef DESQUID
                   + std::string( " moveid " ) + std::to_string( p_moveId ) + " lang "
                   + std::to_string( p_language )
#endif
                ;
        }
        return std::string( tmpbuf );
    }

    std::string getMoveDescr( const u16 p_moveId ) {
        return getMoveDescr( p_moveId, CURRENT_LANGUAGE );
    }

    moveData getMoveData( const u16 p_moveId ) {
        moveData res;
        if( getMoveData( p_moveId, &res ) ) { return res; }
        getMoveData( 0, &res );
        return res;
    }
    bool getMoveData( const u16 p_moveId, moveData* p_out ) {
        FILE* f = FS::openSplit( MOVE_DATA_PATH, p_moveId, ".data" );
        if( !f ) return false;
        fread( p_out, sizeof( moveData ), 1, f );
        fclose( f );
        return true;
    }
} // namespace MOVE

namespace BATTLE {
    bool getTrainerClassName( u16 p_trainerClass, u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( TCLASS_NAME_PATH, p_trainerClass, ".str" );
        if( !f ) return false;

        std::fseek( f, p_language * TCLASS_NAMELENGTH, SEEK_SET );
        fread( p_out, 1, TCLASS_NAMELENGTH, f );
        fclose( f );
        return true;
    }

    std::string getTrainerClassName( u16 p_trainerClass, u8 p_language ) {
        char tmpbuf[ TCLASS_NAMELENGTH ];
        if( !getTrainerClassName( p_trainerClass, p_language, tmpbuf ) ) {
            return std::string( GET_STRING( 550 ) );
        }
        return std::string( tmpbuf );
    }

    std::string getTrainerClassName( u16 p_trainerClass ) {
        return getTrainerClassName( p_trainerClass, CURRENT_LANGUAGE );
    }

    battleTrainer getBattleTrainer( u16 p_battleTrainerId ) {
        return getBattleTrainer( p_battleTrainerId, CURRENT_LANGUAGE );
    }

    battleTrainer getBattleTrainer( u16 p_battleTrainerId, u8 p_language ) {
        battleTrainer res = battleTrainer( );
        if( getBattleTrainer( p_battleTrainerId, p_language, &res ) ) { return res; }
        getBattleTrainer( 0, p_language, &res );
        return res;
    }

    bool getBattleTrainer( u16 p_battleTrainerId, u8 p_language, battleTrainer* p_out ) {
        FILE* f
            = FS::openSplit( BATTLE_STRINGS_PATH, p_battleTrainerId,
                             ( "_" + std::to_string( 1 + p_language ) + ".trnr.str" ).c_str( ) );
        if( !f ) return false;
        fread( &p_out->m_strings, sizeof( trainerStrings ), 1, f );
        fclose( f );
        f = FS::openSplit( BATTLE_TRAINER_PATH, p_battleTrainerId, ".trnr.data" );
        if( !f ) return false;
        fread( &p_out->m_data, sizeof( trainerData ), 1, f );
        fclose( f );
        return true;
    }
} // namespace BATTLE

bool getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language, char* p_out ) {
    FILE* f = FS::openSplit( p_path, p_stringId, ".str" );
    if( !f ) return false;

    std::fseek( f, p_language * p_maxLen, SEEK_SET );
    fread( p_out, 1, p_maxLen, f );
    fclose( f );

    return true;
}

const char* getMapString( u16 p_stringId ) {
    static char st_buffer[ MAPSTRING_LEN + 10 ];

    // std::memset( st_buffer, 0, sizeof( st_buffer ) );
    if( getString( MAPSTRING_PATH, MAPSTRING_LEN, p_stringId, CURRENT_LANGUAGE, st_buffer ) ) {
        return st_buffer;
    }
    return "";
}

const char* getBadge( u16 p_stringId ) {
    static char st_buffer[ BADGENAME_LEN + 10 ];

    // std::memset( st_buffer, 0, sizeof( st_buffer ) );
    if( getString( BADGENAME_PATH, BADGENAME_LEN, p_stringId, CURRENT_LANGUAGE, st_buffer ) ) {
        return st_buffer;
    }
    return "";
}

const char* getAchievement( u16 p_stringId, u8 p_language ) {
    static char st_buffer[ ACHIEVEMENT_LEN + 10 ];

    // std::memset( st_buffer, 0, sizeof( st_buffer ) );
    if( getString( ACHIEVEMENT_PATH, ACHIEVEMENT_LEN, p_stringId, p_language, st_buffer ) ) {
        return st_buffer;
    }
    return "";
}

std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language ) {
    char tmpbuf[ p_maxLen + 5 ];
    if( !getString( p_path, p_maxLen, p_stringId, p_language, tmpbuf ) ) { return "---"; }
    return std::string( tmpbuf );
}

std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId ) {
    return getString( p_path, p_maxLen, p_stringId, CURRENT_LANGUAGE );
}

bool getAbilityName( u16 p_abilityId, u8 p_language, char* p_out ) {
    return getString( ABILITY_NAME_PATH, ABILITY_NAMELENGTH, p_abilityId, p_language, p_out );
}
std::string getAbilityName( u16 p_abilityId, u8 p_language ) {
    return getString( ABILITY_NAME_PATH, ABILITY_NAMELENGTH, p_abilityId, p_language );
}
std::string getAbilityName( u16 p_abilityId ) {
    return getAbilityName( p_abilityId, CURRENT_LANGUAGE );
}

bool getAbilityDescr( u16 p_abilityId, u8 p_language, char* p_out ) {
    return getString( ABILITY_DSCR_PATH, ABILITY_DSCRLENGTH, p_abilityId, p_language, p_out );
}
std::string getAbilityDescr( u16 p_abilityId, u8 p_language ) {
    return getString( ABILITY_DSCR_PATH, ABILITY_DSCRLENGTH, p_abilityId, p_language );
}
std::string getAbilityDescr( u16 p_abilityId ) {
    return getAbilityDescr( p_abilityId, CURRENT_LANGUAGE );
}

std::string getSpeciesName( u16 p_pkmnId, u8 p_language, u8 p_forme ) {
    char tmpbuf[ SPECIES_NAMELENGTH + 2 ];
    if( !getSpeciesName( p_pkmnId, tmpbuf, p_language, p_forme ) ) { return "???"; }
    return std::string( tmpbuf );
}

std::string getSpeciesName( u16 p_pkmnId, u8 p_forme ) {
    return getSpeciesName( p_pkmnId, CURRENT_LANGUAGE, p_forme );
}

bool getSpeciesName( u16 p_pkmnId, char* p_out, u8 p_language, u8 p_forme ) {
    FILE* f;
    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.str", p_forme );
        f = FS::openSplit( POKEMON_SPECIES_PATH, p_pkmnId, tmpbuf );
    }
    if( !p_forme || !f ) { f = FS::openSplit( POKEMON_SPECIES_PATH, p_pkmnId, ".str" ); }
    if( !f ) return false;

    std::fseek( f, p_language * SPECIES_NAMELENGTH, SEEK_SET );
    assert( SPECIES_NAMELENGTH == fread( p_out, 1, SPECIES_NAMELENGTH, f ) );
    fclose( f );
    return true;
}

std::string getDexEntry( u16 p_pkmnId, u8 p_language, u8 p_forme ) {
    char tmpbuf[ DEXENTRY_NAMELENGTH + 2 ];
    if( !getDexEntry( p_pkmnId, tmpbuf, p_language, p_forme ) ) { return "???"; }
    return std::string( tmpbuf );
}

std::string getDexEntry( u16 p_pkmnId, u8 p_forme ) {
    return getDexEntry( p_pkmnId, CURRENT_LANGUAGE, p_forme );
}

bool getDexEntry( u16 p_pkmnId, char* p_out, u8 p_language, u8 p_forme ) {
    FILE* f;
    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.str", p_forme );
        f = FS::openSplit( POKEMON_DEXENTRY_PATH, p_pkmnId, tmpbuf );
    }
    if( !p_forme || !f ) { f = FS::openSplit( POKEMON_DEXENTRY_PATH, p_pkmnId, ".str" ); }
    if( !f ) return false;

    std::fseek( f, p_language * DEXENTRY_NAMELENGTH, SEEK_SET );
    assert( DEXENTRY_NAMELENGTH == fread( p_out, 1, DEXENTRY_NAMELENGTH, f ) );
    fclose( f );
    return true;
}

std::string getDisplayName( u16 p_pkmnId, u8 p_language, u8 p_forme ) {
    char tmpbuf[ 50 ];
    if( !getDisplayName( p_pkmnId, tmpbuf, p_language, p_forme ) ) { return "???"; }
    return std::string( tmpbuf );
}

std::string getDisplayName( u16 p_pkmnId, u8 p_forme ) {
    return getDisplayName( p_pkmnId, CURRENT_LANGUAGE, p_forme );
}

bool getDisplayName( u16 p_pkmnId, char* p_out, u8 p_language, u8 p_forme ) {
    FILE* f;
    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.str", p_forme );
        f = FS::openSplit( POKEMON_NAME_PATH, p_pkmnId, tmpbuf );
    }
    if( !p_forme || !f ) { f = FS::openSplit( POKEMON_NAME_PATH, p_pkmnId, ".str" ); }
    if( !f ) return false;

    u8 len = PKMN_NAMELENGTH;
    if( p_forme ) { len = 30; }

    std::fseek( f, p_language * len, SEEK_SET );
    assert( len == fread( p_out, 1, len, f ) );
    fclose( f );
    return true;
}

pkmnData getPkmnData( const u16 p_pkmnId, const u8 p_forme ) {
    pkmnData res;
    if( getPkmnData( p_pkmnId, p_forme, &res ) ) { return res; }
    getPkmnData( 0, &res );
    return res;
}
bool getPkmnData( const u16 p_pkmnId, pkmnData* p_out ) {
    return getPkmnData( p_pkmnId, 0, p_out );
}
bool getPkmnData( const u16 p_pkmnId, const u8 p_forme, pkmnData* p_out ) {
    FILE* f = FS::openSplit( POKEMON_DATA_PATH, p_pkmnId, ".data" );
    if( !f ) return false;
    fread( p_out, sizeof( pkmnData ), 1, f );
    fclose( f );

    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.data", p_forme );
        f = FS::openSplit( POKEMON_DATA_PATH, p_pkmnId, tmpbuf );
        if( !f ) return true;
        fread( &p_out->m_baseForme, sizeof( pkmnFormeData ), 1, f );
        fclose( f );
    }
    return true;
}

pkmnEvolveData getPkmnEvolveData( const u16 p_pkmnId, const u8 p_forme ) {
    pkmnEvolveData res = pkmnEvolveData( );
    if( getPkmnEvolveData( p_pkmnId, p_forme, &res ) ) { return res; }
    return res;
}
bool getPkmnEvolveData( const u16 p_pkmnId, pkmnEvolveData* p_out ) {
    return getPkmnEvolveData( p_pkmnId, 0, p_out );
}
bool getPkmnEvolveData( const u16 p_pkmnId, const u8 p_forme, pkmnEvolveData* p_out ) {
    FILE* f;
    if( p_forme ) {
        char tmpbuf[ 40 ];
        snprintf( tmpbuf, 35, "_%hhu.evo.data", p_forme );
        f = FS::openSplit( POKEMON_EVOS_PATH, p_pkmnId, tmpbuf );
        if( !f ) return false;
        fread( p_out, sizeof( pkmnEvolveData ), 1, f );
        fclose( f );
    } else {
        f = FS::openSplit( POKEMON_EVOS_PATH, p_pkmnId, ".evo.data" );
        if( !f ) return false;
        fread( p_out, sizeof( pkmnEvolveData ), 1, f );
        fclose( f );
    }
    return true;
}

u16  LEARNSET_BUFFER[ 700 ];
void getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_amount, u16* p_result ) {
    FILE* f = FS::openSplit( PKMN_LEARNSET_PATH, p_pkmnId, ".learnset.data" );
    if( !f ) f = FS::openSplit( PKMN_LEARNSET_PATH, 201, ".learnset.data" );
    if( !f ) return;

    FS::read( f, LEARNSET_BUFFER, sizeof( u16 ), 680 );
    FS::close( f );
    u16 ptr = 0;

    for( u8 i = 0; i < p_amount; ++i ) p_result[ i ] = 0;
    if( p_fromLevel > p_toLevel ) std::swap( p_fromLevel, p_toLevel );

    std::vector<u16> reses;
    for( u16 i = 0; i <= p_toLevel; ++i ) {
        while( i == LEARNSET_BUFFER[ ptr ] ) {
            if( i >= p_fromLevel ) {
                reses.push_back( LEARNSET_BUFFER[ ++ptr ] );
            } else {
                ++ptr;
            }
            ptr++;
        }
    }
    auto I = reses.rbegin( );
    for( u16 i = 0; i < p_amount && I != reses.rend( ); ++i, ++I ) {
        for( u16 z = 0; z < i; ++z )
            if( *I == p_result[ z ] ) {
                --i;
                goto N;
            }
        p_result[ i ] = *I;
    N:;
    }
    return;
}

bool canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_maxLevel ) {
    FILE* f = FS::openSplit( PKMN_LEARNSET_PATH, p_pkmnId, ".learnset.data" );
    if( !f ) return false;

    FS::read( f, LEARNSET_BUFFER, sizeof( u16 ), 680 );
    FS::close( f );
    u16 ptr = 0;

    for( u16 i = 0; i <= p_maxLevel; ++i ) {
        while( i == LEARNSET_BUFFER[ ptr ] ) {
            if( p_moveId == LEARNSET_BUFFER[ ++ptr ] ) { return true; }
            ptr++;
        }
    }
    return false;
}
