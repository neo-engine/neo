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
#include "gen/bgmTranslation.h"
#endif

namespace FS {
    const char PKMNDATA_PATH[]    = "nitro:/PKMNDATA/";
    const char SCRIPT_PATH[]      = "nitro:/DATA/MAP_SCRIPT/";
    const char ICAVE_PATH[]       = "nitro:/DATA/BST_EV/";
    const char MAPLOCATION_PATH[] = "nitro:/DATA/MAP_LOCATION/";

#ifndef NO_SOUND
    const char CRY_PATH[]  = "nitro:/SOUND/CRIES/";
    const char SFX_PATH[]  = "nitro:/SOUND/SFX/";
    const char SSEQ_PATH[] = "nitro:/SOUND/BGM/SSEQ/";
    const char SBNK_PATH[] = "nitro:/SOUND/BGM/SBNK/";
    const char SWAR_PATH[] = "nitro:/SOUND/BGM/SWAR/";
#endif

    const char ITEM_NAME_PATH[]        = "nitro:/DATA/ITEM_NAME/itemname";
    const char ITEM_DSCR_PATH[]        = "nitro:/DATA/ITEM_DSCR/itemdscr";
    const char ABILITY_NAME_PATH[]     = "nitro:/DATA/ABTY_NAME/abtyname";
    const char ABILITY_DSCR_PATH[]     = "nitro:/DATA/ABTY_DSCR/abtydscr";
    const char MOVE_NAME_PATH[]        = "nitro:/DATA/MOVE_NAME/movename";
    const char MOVE_DSCR_PATH[]        = "nitro:/DATA/MOVE_DSCR/movedscr";
    const char POKEMON_NAME_PATH[]     = "nitro:/DATA/PKMN_NAME/pkmnname";
    const char FORME_NAME_PATH[]       = "nitro:/DATA/PKMN_NAME/pkmnfname";
    const char POKEMON_SPECIES_PATH[]  = "nitro:/DATA/PKMN_SPCS/pkmnspcs";
    const char POKEMON_DEXENTRY_PATH[] = "nitro:/DATA/PKMN_DXTR/pkmndxtr";

    const char  BATTLE_STRINGS_PATH[]     = "nitro:/DATA/TRNR_STRS/";
    const char* BATTLE_TRAINER_PATHS[ 3 ] = {
        "nitro:/DATA/TRNR_DATA/0/",
        "nitro:/DATA/TRNR_DATA/1/",
        "nitro:/DATA/TRNR_DATA/2/",
    };
    const char BATTLE_FACILITY_STRINGS_PATH[] = "nitro:/DATA/BFTR_STRS/";
    const char BATTLE_FACILITY_PKMN_PATH[]    = "nitro:/DATA/BFTR_PKMN/";
    const char TCLASS_NAME_PATH[]             = "nitro:/DATA/TRNR_NAME/trnrname";

    const char BGM_NAME_PATH[]      = "nitro:/DATA/BGM_NAME/bgmnames";
    const char LOCATION_NAME_PATH[] = "nitro:/DATA/LOC_NAME/locname";
    const char UISTRING_PATH[]      = "nitro:/STRN/UIS/uis";
    const char MAPSTRING_PATH[]     = "nitro:/STRN/MAP/map";
    const char TRADESTRING_PATH[]   = "nitro:/STRN/TRD/str";
    const char WCSTRING_PATH[]      = "nitro:/STRN/UIS/mys";
    const char PKMNPHRS_PATH[]      = "nitro:/STRN/PHR/phr";
    const char BADGENAME_PATH[]     = "nitro:/STRN/BDG/bdg";
    const char ACHIEVEMENT_PATH[]   = "nitro:/STRN/AVM/avm";
    const char RIBBONNAME_PATH[]    = "nitro:/STRN/RBN/rbn";
    const char RIBBONDSCR_PATH[]    = "nitro:/STRN/RBN/rbd";
    const char TRAINERNAME_PATH[]   = "nitro:/STRN/TRN/name";
    const char TRAINERMSG1_PATH[]   = "nitro:/STRN/TRN/msg1";
    const char TRAINERMSG2_PATH[]   = "nitro:/STRN/TRN/msg2";
    const char TRAINERMSG3_PATH[]   = "nitro:/STRN/TRN/msg3";

    const char LOCDATA_PATH[]        = "nitro:/DATA/location.datab";
    const char MOVE_DATA_PATH[]      = "nitro:/DATA/move.datab";
    const char ITEM_DATA_PATH[]      = "nitro:/DATA/item.datab";
    const char BERRY_DATA_PATH[]     = "nitro:/DATA/berry.datab";
    const char POKEMON_DATA_PATH[]   = "nitro:/DATA/pkmn.datab";
    const char PKMN_LEARNSET_PATH[]  = "nitro:/DATA/pkmn.learnset.datab";
    const char POKEMON_EVOS_PATH[]   = "nitro:/DATA/pkmn.evolve.datab";
    const char FORME_DATA_PATH[]     = "nitro:/DATA/pkmnf.datab";
    const char FORME_LEARNSET_PATH[] = "nitro:/DATA/pkmnf.learnset.datab";
    const char FORME_EVOS_PATH[]     = "nitro:/DATA/pkmnf.evolve.datab";

    constexpr u16 LEARNSET_SIZE = 2 * 400;

    bool getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language, char* p_out ) {
        FILE* f = openSplit( p_path, p_stringId, ".str" );
        if( !f ) return false;

        std::fseek( f, p_language * p_maxLen, SEEK_SET );
        fread( p_out, 1, p_maxLen, f );
        fclose( f );
        DC_FlushRange( p_out, p_maxLen );

        return true;
    }

    bool getString( FILE* p_bankFile, u16 p_maxLen, u16 p_stringId, char* p_out ) {
        if( !p_bankFile ) { return false; }
        if( std::fseek( p_bankFile, p_stringId * p_maxLen, SEEK_SET ) ) { return false; }
        fread( p_out, 1, p_maxLen, p_bankFile );
        DC_FlushRange( p_out, p_maxLen );
        return true;
    }

    char TMP_BUFFER_SHORT[ 50 ];
    u8   CRY_DATA[ 22050 * 2 ];

    FILE* LOCATION_DATA_FILE = nullptr;

    struct locationData {
        u16 m_bgmNameIdx = 0;
        u8  m_frameType  = 0;
        u8  m_mugType    = 0;
    };

    u16 BGMforLocation( u16 p_locationId ) {
        if( !checkOrOpen( LOCATION_DATA_FILE, LOCDATA_PATH ) ) { return BGM_NONE; }

        if( std::fseek( LOCATION_DATA_FILE, p_locationId * 4, SEEK_SET ) ) { return BGM_NONE; }

        auto l = locationData( );
        fread( &l, sizeof( locationData ), 1, LOCATION_DATA_FILE );

        DC_FlushRange( &l, sizeof( locationData ) );

        return l.m_bgmNameIdx;
    }

    u16 frameForLocation( u16 p_locationId ) {
        if( !checkOrOpen( LOCATION_DATA_FILE, LOCDATA_PATH ) ) { return 0; }
        if( std::fseek( LOCATION_DATA_FILE, p_locationId * sizeof( locationData ), SEEK_SET ) ) {
            return 0;
        }

        auto l = locationData( );
        fread( &l, sizeof( locationData ), 1, LOCATION_DATA_FILE );

        DC_FlushRange( &l, sizeof( locationData ) );

        return l.m_frameType;
    }

    u16 mugForLocation( u16 p_locationId ) {
        if( !checkOrOpen( LOCATION_DATA_FILE, LOCDATA_PATH ) ) { return 0; }
        if( std::fseek( LOCATION_DATA_FILE, p_locationId * sizeof( locationData ), SEEK_SET ) ) {
            return 0;
        }

        auto l = locationData( );
        fread( &l, sizeof( locationData ), 1, LOCATION_DATA_FILE );

        DC_FlushRange( &l, sizeof( locationData ) );
        return l.m_mugType;
    }

    FILE* openScript( u16 p_scriptId ) {
        return openSplit( SCRIPT_PATH, p_scriptId, ".mapscr", 10 * 30 );
    }

    FILE* openInfinityCave( u8 p_stat ) {
        return open( ICAVE_PATH, p_stat, ".data" );
    }

    FILE* openHabitatData( u8 p_bank ) {
        return open( MAPLOCATION_PATH, p_bank, ".wpoke.data" );
    }

    bool loadLocationData( u8 p_bank ) {
        FILE* f = open( MAPLOCATION_PATH, p_bank, ".loc.data" );
        if( !f ) {
            MAP::MAP_LOCATIONS.m_good = false;
            MAP::MAP_LOCATIONS.m_bank = 0;
            return false;
        }

        fread( &MAP::MAP_LOCATIONS.m_owMapSizeX, sizeof( u8 ), 1, f );
        fread( &MAP::MAP_LOCATIONS.m_owMapSizeY, sizeof( u8 ), 1, f );
        fread( &MAP::MAP_LOCATIONS.m_mapImageRes, sizeof( u8 ), 1, f );
        fread( &MAP::MAP_LOCATIONS.m_mapImageShiftX, sizeof( u8 ), 1, f );
        fread( &MAP::MAP_LOCATIONS.m_mapImageShiftY, sizeof( u8 ), 1, f );
        fread( &MAP::MAP_LOCATIONS.m_defaultLocation, sizeof( u16 ), 1, f );

        MAP::MAP_LOCATIONS.m_locationData.resize( MAP::MAP_LOCATIONS.m_owMapSizeX
                                                  * MAP::MAP_LOCATIONS.m_owMapSizeY );

        fread( &MAP::MAP_LOCATIONS.m_locationData[ 0 ], sizeof( u16 ),
               MAP::MAP_LOCATIONS.m_locationData.size( ), f );

        MAP::MAP_LOCATIONS.m_good = true;
        MAP::MAP_LOCATIONS.m_bank = p_bank;
        fclose( f );
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

    u8* readCry( u16 p_pkmnIdx, u8 p_forme, u32& p_len ) {
#ifndef NO_SOUND
        FILE* f;

        if( p_forme ) {
            snprintf( TMP_BUFFER_SHORT, 49, "_%hhu.raw", p_forme );
            f = openSplit( CRY_PATH, p_pkmnIdx, ".raw", MAX_PKMN );
        }
        if( !p_forme || !f ) { f = openSplit( CRY_PATH, p_pkmnIdx, ".raw", MAX_PKMN ); }
        if( !f ) { return nullptr; }

        std::memset( CRY_DATA, 0, sizeof( CRY_DATA ) );
        if( !( p_len = read( f, CRY_DATA, sizeof( u8 ), sizeof( CRY_DATA ) ) ) ) { return nullptr; }
        p_len >>= 2;
        return CRY_DATA;
#else
        (void) p_pkmnIdx;
        (void) p_forme;
        (void) p_len;
        return nullptr;
#endif
    }

    u8* readSFX( u16 p_sfxIdx, u16& p_len ) {
#ifndef NO_SOUND
        FILE* f = openSplit( SFX_PATH, p_sfxIdx, ".raw", 400 );
        if( !f ) { return nullptr; }

        std::memset( CRY_DATA, 0, sizeof( CRY_DATA ) );
        if( !( p_len = read( f, CRY_DATA, 1, sizeof( CRY_DATA ) ) ) ) { return nullptr; }
        p_len >>= 2;
        return CRY_DATA;
#else
        (void) p_sfxIdx;
        (void) p_len;
        return nullptr;
#endif
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

    bool seekTileSet( FILE* p_file, u8 p_tsIdx ) {
        if( !p_file ) { return false; }

        MAP::blockSetBankHeader info;
        if( fseek( p_file, 0, SEEK_SET ) ) { return 2; }
        fread( &info, sizeof( MAP::blockSetBankHeader ), 1, p_file );

        if( fseek( p_file,
                   sizeof( MAP::blockSetBankHeader )
                       + ( p_tsIdx
                           * ( ( sizeof( MAP::tile ) * 512 )                        // tiles
                               + ( 4 + 512 * ( 8 * sizeof( MAP::blockAtom ) + 2 ) ) // blocks
                               + ( sizeof( u16 ) * 16 * 8 * info.m_dayTimeCount )   // pals
                               ) ),
                   SEEK_SET ) ) {
            return false;
        }
        return true;
    }

    FILE* openTileSet( ) {
        FILE* f = open( MAP::MAP_PATH, "tileset", ".tsb" );
        if( !f ) { return nullptr; }
        return f;
    }

    FILE* openBank( u16 p_bank, bool p_underwater ) {
        snprintf( TMP_BUFFER_SHORT, 45, "%i", p_bank + ( p_underwater ? DIVE_MAP : 0 ) );
        FILE* f = open( MAP::MAP_PATH, TMP_BUFFER_SHORT, ".bank" );
        if( !f ) { return nullptr; }
        return f;
    }

    bool readMapData( FILE* p_file, MAP::mapData* p_result, bool p_close ) {
        if( !p_file ) {
            std::memset( p_result, 0, sizeof( MAP::mapData ) );
            return false;
        }
        fread( p_result, sizeof( MAP::mapData ), 1, p_file );
        if( p_close ) { fclose( p_file ); }
        return true;
    }

    bool readMapSlice( FILE* p_mapFile, MAP::mapSlice* p_result, u16 p_x, u16 p_y, bool p_close ) {
        if( p_mapFile == 0 ) return false;
        p_result->m_x = p_x;
        p_result->m_y = p_y;
        read( p_mapFile, &p_result->m_data, sizeof( MAP::mapSliceData ), 1 );
        if( p_close ) { fclose( p_mapFile ); }
        return true;
    }

    u32 readMapBankInfo( FILE* p_mapFile, MAP::bankInfo* p_info ) {
        if( p_mapFile == 0 ) { return 1; }

        if( fseek( p_mapFile, 0, SEEK_SET ) ) { return 2; }
        fread( p_info, sizeof( MAP::bankInfo ), 1, p_mapFile );

        return 0;
    }

    u32 readMapSliceAndData( FILE* p_mapFile, MAP::mapSlice* p_slice, MAP::mapData* p_data, u16 p_x,
                             u16 p_y ) {
        MAP::bankInfo info;
        if( readMapBankInfo( p_mapFile, &info ) ) { return 1; }

        if( fseek( p_mapFile,
                   sizeof( MAP::bankInfo )
                       + ( ( info.m_sizeX + 1 ) * p_y + p_x )
                             * ( sizeof( MAP::mapSliceData ) + sizeof( MAP::mapData ) ),
                   SEEK_SET ) ) {
            return 3;
        }

        if( p_slice == nullptr ) {
            if( fseek( p_mapFile, sizeof( MAP::mapSliceData ), SEEK_CUR ) ) { return 6; }
        } else if( !readMapSlice( p_mapFile, p_slice, p_x, p_y, false ) ) {
            return 4;
        }
        if( p_data == nullptr ) {
            if( fseek( p_mapFile, sizeof( MAP::mapData ), SEEK_CUR ) ) { return 7; }
        } else if( !readMapData( p_mapFile, p_data, false ) ) {
            return 5;
        }
        return 0;
    }

    bool getBGMName( const u16 p_BGMId, const u8 p_language, char* p_out ) {
#ifndef NO_SOUND
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;

        auto bgmid = SOUND::SSEQ::BGMIndexForName( p_BGMId );
        if( bgmid < 0 ) { return false; }
        if( !checkOrOpen( bankfile, BGM_NAME_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, BGM_NAMELENGTH, bgmid, p_out ) ) { return true; }
#else
        (void) p_BGMId;
        (void) p_language;
        (void) p_out;
#endif
        return false;
    }
    std::string getBGMName( const u16 p_BGMId, const u8 p_language ) {
        char tmpbuf[ BGM_NAMELENGTH ];
        if( !getBGMName( p_BGMId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }

    std::string getBGMName( const u16 p_BGMId ) {
        return getBGMName( p_BGMId, CURRENT_LANGUAGE );
    }

    bool getLocation( const u16 p_locationId, const u8 p_language, char* p_out ) {
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, LOCATION_NAME_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, LOCATION_NAMELENGTH, p_locationId, p_out ) ) { return true; }
        return false;
    }
    std::string getLocation( const u16 p_locationId, const u8 p_language ) {
        char tmpbuf[ LOCATION_NAMELENGTH ];
        if( !getLocation( p_locationId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }

    std::string getLocation( const u16 p_locationId ) {
        return getLocation( p_locationId, CURRENT_LANGUAGE );
    }

    bool getItemName( const u16 p_itemId, const u8 p_language, char* p_out ) {
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, ITEM_NAME_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, ITEM_NAMELENGTH, p_itemId, p_out ) ) { return true; }
        return false;
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
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, ITEM_DSCR_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, ITEM_DSCRLENGTH, p_itemId, p_out ) ) { return true; }
        return false;
    }
    std::string getItemDescr( const u16 p_itemId, const u8 p_language ) {
        char tmpbuf[ ITEM_DSCRLENGTH ];
        if( !getItemDescr( p_itemId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }
    std::string getItemDescr( const u16 p_itemId ) {
        return getItemDescr( p_itemId, CURRENT_LANGUAGE );
    }

    BAG::itemData getItemData( const u16 p_itemId ) {
        BAG::itemData res;
        if( getItemData( p_itemId, &res ) ) { return res; }
        getItemData( 0, &res );
        return res;
    }
    bool getItemData( const u16 p_itemId, BAG::itemData* p_out ) {
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, ITEM_DATA_PATH ) ) { return false; }
        if( std::fseek( bankfile, p_itemId * sizeof( BAG::itemData ), SEEK_SET ) ) { return false; }
        fread( p_out, sizeof( BAG::itemData ), 1, bankfile );
        return true;
    }

    BAG::berry getBerryData( const u8 p_berryId ) {
        BAG::berry res;
        if( getBerryData( p_berryId, &res ) ) { return res; }
        getBerryData( 0, &res );
        return res;
    }
    bool getBerryData( const u8 p_berryId, BAG::berry* p_out ) {
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, BERRY_DATA_PATH ) ) { return false; }
        if( std::fseek( bankfile, p_berryId * sizeof( BAG::berry ), SEEK_SET ) ) { return false; }
        fread( p_out, sizeof( BAG::berry ), 1, bankfile );
        return true;
    }

    bool getMoveName( const u16 p_moveId, const u8 p_language, char* p_out ) {
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, MOVE_NAME_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, MOVE_NAMELENGTH, p_moveId, p_out ) ) { return true; }
        return false;
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
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, MOVE_DSCR_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, MOVE_DSCRLENGTH, p_moveId, p_out ) ) { return true; }
        return false;
    }
    std::string getMoveDescr( const u16 p_moveId, const u8 p_language ) {
        char tmpbuf[ MOVE_DSCRLENGTH ];
        if( !getMoveDescr( p_moveId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }
    std::string getMoveDescr( const u16 p_moveId ) {
        return getMoveDescr( p_moveId, CURRENT_LANGUAGE );
    }

    BATTLE::moveData getMoveData( const u16 p_moveId ) {
        BATTLE::moveData res;
        if( getMoveData( p_moveId, &res ) ) { return res; }
        getMoveData( 0, &res );
        return res;
    }
    bool getMoveData( const u16 p_moveId, BATTLE::moveData* p_out ) {
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, MOVE_DATA_PATH ) ) { return false; }
        if( std::fseek( bankfile, p_moveId * sizeof( BATTLE::moveData ), SEEK_SET ) ) {
            return false;
        }
        fread( p_out, sizeof( BATTLE::moveData ), 1, bankfile );
        return true;
    }

    bool getTrainerClassName( u16 p_trainerClass, u8 p_language, char* p_out ) {
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, TCLASS_NAME_PATH, lastLang, p_language );
        if( getString( bankfile, TCLASS_NAMELENGTH, p_trainerClass, p_out ) ) { return true; }
        return false;
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

    BATTLE::battleTrainer getBattleTrainer( u16 p_battleTrainerId ) {
        return getBattleTrainer( p_battleTrainerId, CURRENT_LANGUAGE );
    }

    BATTLE::battleTrainer getBattleTrainer( u16 p_battleTrainerId, u8 p_language ) {
        BATTLE::battleTrainer res = BATTLE::battleTrainer( );
        if( getBattleTrainer( p_battleTrainerId, p_language, &res ) ) { return res; }
        getBattleTrainer( 0, p_language, &res );
        return res;
    }

    bool getBattleTrainer( u16 p_battleTrainerId, u8 p_language, BATTLE::battleTrainer* p_out ) {
        static u8    lastLang = -1;
        static FILE* namefile = nullptr;
        static FILE* msg1file = nullptr;
        static FILE* msg2file = nullptr;
        static FILE* msg3file = nullptr;
        checkOrOpen( namefile, TRAINERNAME_PATH, lastLang, p_language );
        if( !getString( namefile, TRAINERNAME_LEN, p_battleTrainerId, p_out->m_strings.m_name ) ) {
            return false;
        }
        checkOrOpen( msg1file, TRAINERMSG1_PATH, lastLang, p_language );
        if( !getString( msg1file, TRAINERMSG_LEN, p_battleTrainerId,
                        p_out->m_strings.m_message1 ) ) {
            return false;
        }
        checkOrOpen( msg2file, TRAINERMSG2_PATH, lastLang, p_language );
        if( !getString( msg2file, TRAINERMSG_LEN, p_battleTrainerId,
                        p_out->m_strings.m_message2 ) ) {
            return false;
        }
        checkOrOpen( msg3file, TRAINERMSG3_PATH, lastLang, p_language );
        if( !getString( msg3file, TRAINERMSG_LEN, p_battleTrainerId,
                        p_out->m_strings.m_message3 ) ) {
            return false;
        }

        FILE* f = openSplit(
            BATTLE_TRAINER_PATHS[ SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) / 3 ],
            p_battleTrainerId, ".trnr.data" );
        if( !f && SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) != 3 ) {
            f = openSplit( BATTLE_TRAINER_PATHS[ 1 ], p_battleTrainerId, ".trnr.data" );
        }
        if( !f ) { return false; }
        fread( &p_out->m_data, sizeof( BATTLE::trainerData ), 1, f );
        fclose( f );
        return true;
    }

    bool loadBattleFacilityTrainerStrings( u8 p_trainerClass, u8 p_trainer,
                                           BATTLE::battleTrainer* p_out ) {
        FILE* f = openSplit(
            BATTLE_FACILITY_STRINGS_PATH, MAP::TRAINERS_PER_CLASS * p_trainerClass + p_trainer,
            ( "_" + std::to_string( 1 + CURRENT_LANGUAGE ) + ".trnr.str" ).c_str( ) );
        if( !f ) return false;
        fread( &p_out->m_strings, sizeof( BATTLE::trainerStrings ), 1, f );
        fclose( f );
        return true;
    }

    bool loadBattleFacilityPkmn( u16 p_species, u8 p_variant, u8 p_level, u16 p_streak,
                                 trainerPokemon* p_out ) {
        (void) p_variant;

        u16 streak = p_streak >= MAP::IV_MAX_STREAK ? MAP::IV_MAX_STREAK - 1 : p_streak;
        u8  iv     = MAP::IV_FOR_STREAK[ streak ];

        // TODO: actually load these pkmn from ROM
        p_out->m_speciesId  = p_species;
        p_out->m_forme      = 0;
        p_out->m_ability    = 0;
        p_out->m_heldItem   = 0;
        p_out->m_moves[ 0 ] = 1;
        p_out->m_moves[ 1 ] = 0;
        p_out->m_moves[ 2 ] = 0;
        p_out->m_moves[ 3 ] = 0;
        p_out->m_ev[ 0 ]    = 0;
        p_out->m_ev[ 1 ]    = 0;
        p_out->m_ev[ 2 ]    = 0;
        p_out->m_ev[ 3 ]    = 0;
        p_out->m_ev[ 4 ]    = 0;
        p_out->m_ev[ 5 ]    = 0;
        p_out->m_shiny      = 0;
        p_out->m_nature     = 0;

        // adjust pkmn to given parameters
        p_out->m_level   = p_level;
        p_out->m_iv[ 0 ] = p_out->m_iv[ 1 ] = p_out->m_iv[ 2 ] = iv;
        p_out->m_iv[ 3 ] = p_out->m_iv[ 4 ] = p_out->m_iv[ 5 ] = iv;

        return true;
    }

    bool loadBattleFacilityTrainerTeam( const MAP::ruleSet& p_rules, u8 p_trainerClass, u8 p_team,
                                        u16 p_streak, BATTLE::battleTrainer* p_out ) {
        (void) p_rules;
        (void) p_trainerClass;
        (void) p_team;
        (void) p_streak;
        (void) p_out;
        // TODO
        return false;
    }

#ifndef NO_SOUND
    bool loadSequenceData( SOUND::SSEQ::sequenceData* p_data, FILE* p_f ) {
        if( !p_f ) { return false; }
        fseek( p_f, 0, SEEK_END );
        p_data->m_size = ftell( p_f );

        rewind( p_f );
        p_data->m_data = malloc( p_data->m_size );

        if( !p_data->m_data ) { return false; }
        fread( p_data->m_data, 1, p_data->m_size, p_f );
        DC_FlushRange( p_data->m_data, p_data->m_size );
        return true;
    }

    bool loadSoundSequence( SOUND::SSEQ::sequenceData* p_data, u16 p_sseqId ) {
        FILE* f   = openSplit( SSEQ_PATH, p_sseqId, ".sseq", 9 * ITEMS_PER_DIR, "rb" );
        auto  res = loadSequenceData( p_data, f );
        fclose( f );
        return res;
    }

    bool loadSoundBank( SOUND::SSEQ::sequenceData* p_data, u16 p_sbnkId ) {
        FILE* f   = openSplit( SBNK_PATH, p_sbnkId, ".sbnk", 9 * ITEMS_PER_DIR, "rb" );
        auto  res = loadSequenceData( p_data, f );
        fclose( f );
        return res;
    }

    bool loadSoundSample( SOUND::SSEQ::sequenceData* p_data, u16 p_swarId ) {
        FILE* f   = openSplit( SWAR_PATH, p_swarId, ".swar", 9 * ITEMS_PER_DIR, "rb" );
        auto  res = loadSequenceData( p_data, f );
        fclose( f );
        return res;
    }
#endif

    const char* getUIString( u16 p_stringId, u8 p_language ) {
        static char  st_buffer[ UISTRING_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, UISTRING_PATH, lastLang, p_language );
        if( getString( bankfile, UISTRING_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "(uistring failed)";
    }

    const char* getPkmnPhrase( u16 p_stringId ) {
        static char  st_buffer[ PKMNPHRS_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, PKMNPHRS_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, PKMNPHRS_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "";
    }

    const char* getMapString( u16 p_stringId ) {
        static char  st_buffer[ MAPSTRING_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, MAPSTRING_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, MAPSTRING_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "(string failed)";
    }

    const char* getTradeString( u16 p_stringId ) {
        static char  st_buffer[ TRADESTRING_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, TRADESTRING_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, TRADESTRING_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "(string failed)";
    }

    const char* getWCString( u16 p_stringId ) {
        static char  st_buffer[ WCSTRING_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, WCSTRING_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, WCSTRING_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "(string failed)";
    }

    const char* getBadge( u16 p_stringId ) {
        static char  st_buffer[ BADGENAME_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, BADGENAME_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, BADGENAME_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "";
    }

    const char* getAchievement( u16 p_stringId, u8 p_language ) {
        static char  st_buffer[ ACHIEVEMENT_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, ACHIEVEMENT_PATH, lastLang, p_language );
        if( getString( bankfile, ACHIEVEMENT_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "";
    }

    const char* getRibbonName( u16 p_stringId ) {
        static char  st_buffer[ RIBBONNAME_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, RIBBONNAME_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, RIBBONNAME_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "";
    }

    const char* getRibbonDescr( u16 p_stringId ) {
        static char  st_buffer[ RIBBONDSCR_LEN + 10 ];
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        checkOrOpen( bankfile, RIBBONDSCR_PATH, lastLang, CURRENT_LANGUAGE );
        if( getString( bankfile, RIBBONDSCR_LEN, p_stringId, st_buffer ) ) { return st_buffer; }
        return "";
    }

    std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language ) {
        char tmpbuf[ p_maxLen + 5 ] = { 0 };
        if( !getString( p_path, p_maxLen, p_stringId, p_language, tmpbuf ) ) { return "---"; }
        return std::string( tmpbuf );
    }

    std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId ) {
        return getString( p_path, p_maxLen, p_stringId, CURRENT_LANGUAGE );
    }

    bool getAbilityName( u16 p_abilityId, u8 p_language, char* p_out ) {
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, ABILITY_NAME_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, ABILITY_NAMELENGTH, p_abilityId, p_out ) ) { return true; }
        return false;
    }
    std::string getAbilityName( u16 p_abilityId, u8 p_language ) {
        char st_buffer[ ABILITY_NAMELENGTH + 10 ] = { 0 };
        getAbilityName( p_abilityId, p_language, st_buffer );
        return std::string( st_buffer );
    }
    std::string getAbilityName( u16 p_abilityId ) {
        return getAbilityName( p_abilityId, CURRENT_LANGUAGE );
    }

    bool getAbilityDescr( u16 p_abilityId, u8 p_language, char* p_out ) {
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        if( !checkOrOpen( bankfile, ABILITY_DSCR_PATH, lastLang, p_language ) ) { return false; }
        if( getString( bankfile, ABILITY_DSCRLENGTH, p_abilityId, p_out ) ) { return true; }
        return false;
    }
    std::string getAbilityDescr( u16 p_abilityId, u8 p_language ) {
        char st_buffer[ ABILITY_DSCRLENGTH + 10 ] = { 0 };
        getAbilityDescr( p_abilityId, p_language, st_buffer );
        return std::string( st_buffer );
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
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        //    static FILE* fbankfile = nullptr;
        //    auto         id        = -1;
        (void) p_forme;
        /*    if( p_forme && ( id = formeIdx( p_pkmnId, p_forme ) ) != -1 ) {
                if( !checkOrOpen( fbankfile, FORME_SPECIES_PATH, lastLang, p_language ) ) {
                    return false;
                }
                if( getString( fbankfile, SPECIES_NAMELENGTH, id, p_out ) ) { return true; }
            } else */
        {
            if( !checkOrOpen( bankfile, POKEMON_SPECIES_PATH, lastLang, p_language ) ) {
                return false;
            }
            if( getString( bankfile, SPECIES_NAMELENGTH, p_pkmnId, p_out ) ) { return true; }
        }
        return false;
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
        static u8    lastLang = -1;
        static FILE* bankfile = nullptr;
        (void) p_forme;
        //    static FILE* fbankfile = nullptr;
        //    auto         id        = -1;
        /*    if( p_forme && ( id = formeIdx( p_pkmnId, p_forme ) ) != -1 ) {
                if( !checkOrOpen( fbankfile, FORME_DEXENTRY_PATH, lastLang, p_language ) ) {
                    return false;
                }
                if( getString( fbankfile, DEXENTRY_NAMELENGTH, id, p_out ) ) { return true; }
            } else */
        {
            if( !checkOrOpen( bankfile, POKEMON_DEXENTRY_PATH, lastLang, p_language ) ) {
                return false;
            }
            if( getString( bankfile, DEXENTRY_NAMELENGTH, p_pkmnId, p_out ) ) { return true; }
        }
        return false;
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
        static u8    lastLang  = -1;
        static FILE* bankfile  = nullptr;
        static FILE* fbankfile = nullptr;
        auto         id        = -1;
        if( p_forme && ( id = formeIdx( p_pkmnId, p_forme ) ) != -1 ) {
            if( !checkOrOpen( fbankfile, FORME_NAME_PATH, lastLang, p_language ) ) { return false; }
            if( getString( fbankfile, FORME_NAMELENGTH, id, p_out ) ) { return true; }
        } else {
            if( !checkOrOpen( bankfile, POKEMON_NAME_PATH, lastLang, p_language ) ) {
                return false;
            }
            if( getString( bankfile, PKMN_NAMELENGTH, p_pkmnId, p_out ) ) { return true; }
        }
        return false;
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
        static FILE* bankfile  = nullptr;
        static FILE* bankfilef = nullptr;
        auto         id        = -1;
        if( p_forme && ( id = formeIdx( p_pkmnId, p_forme ) ) != -1 ) {
            if( !checkOrOpen( bankfilef, FORME_DATA_PATH ) ) { return false; }
            if( std::fseek( bankfilef, id * sizeof( pkmnData ), SEEK_SET ) ) { return false; }
            fread( p_out, sizeof( pkmnData ), 1, bankfilef );
        } else {
            if( !checkOrOpen( bankfile, POKEMON_DATA_PATH ) ) { return false; }
            if( std::fseek( bankfile, p_pkmnId * sizeof( pkmnData ), SEEK_SET ) ) { return false; }
            fread( p_out, sizeof( pkmnData ), 1, bankfile );
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
        static FILE* bankfile  = nullptr;
        static FILE* bankfilef = nullptr;
        auto         id        = -1;
        if( p_forme && ( id = formeIdx( p_pkmnId, p_forme ) ) != -1 ) {
            if( !checkOrOpen( bankfilef, FORME_EVOS_PATH ) ) { return false; }
            if( std::fseek( bankfilef, id * sizeof( pkmnEvolveData ), SEEK_SET ) ) { return false; }
            fread( p_out, sizeof( pkmnEvolveData ), 1, bankfilef );
        } else {
            if( !checkOrOpen( bankfile, POKEMON_EVOS_PATH ) ) { return false; }
            if( std::fseek( bankfile, p_pkmnId * sizeof( pkmnEvolveData ), SEEK_SET ) ) {
                return false;
            }
            fread( p_out, sizeof( pkmnEvolveData ), 1, bankfile );
        }
        return true;
    }

    u16  LEARNSET_BUFFER[ LEARNSET_SIZE + 10 ];
    void getLearnMoves( u16 p_pkmnId, u8 p_forme, u16 p_fromLevel, u16 p_toLevel, u16 p_amount,
                        u16* p_result ) {
        auto learnset = getLearnset( p_pkmnId, p_forme );
        if( !learnset ) { return; }
        u16 ptr = 0;

        for( u8 i = 0; i < p_amount; ++i ) p_result[ i ] = 0;
        if( p_fromLevel > p_toLevel ) std::swap( p_fromLevel, p_toLevel );

        std::vector<u16> reses;
        for( u16 i = 0; i <= p_toLevel; ++i ) {
            while( i == learnset[ ptr ] ) {
                if( i >= p_fromLevel ) {
                    reses.push_back( learnset[ ++ptr ] );
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

    bool canLearn( const u16* p_learnset, u16 p_moveId, u16 p_maxLevel, u16 p_minLevel ) {
        if( !p_learnset ) { return false; }

        u16 ptr = 0;
        for( u16 i = 0; i <= p_maxLevel; ++i ) {
            while( i == p_learnset[ ptr ] ) {
                if( p_moveId == p_learnset[ ++ptr ] && i >= p_minLevel ) { return true; }
                ptr++;
            }
        }
        return false;
    }
    bool canLearn( u16 p_pkmnId, u8 p_forme, u16 p_moveId, u16 p_maxLevel, u16 p_minLevel ) {
        return canLearn( getLearnset( p_pkmnId, p_forme ), p_moveId, p_maxLevel, p_minLevel );
    }

    bool getLearnset( u16 p_pkmnId, u8 p_forme, u16* p_out ) {
        static FILE* bankfile  = nullptr;
        static FILE* bankfilef = nullptr;
        auto         id        = -1;
        if( p_forme && ( id = formeIdx( p_pkmnId, p_forme ) ) != -1 ) {
            if( !checkOrOpen( bankfilef, FORME_LEARNSET_PATH ) ) { return false; }
            if( std::fseek( bankfilef, id * LEARNSET_SIZE * sizeof( u16 ), SEEK_SET ) ) {
                return false;
            }
            fread( p_out, sizeof( u16 ), LEARNSET_SIZE, bankfilef );
        } else {
            if( !checkOrOpen( bankfile, PKMN_LEARNSET_PATH ) ) { return false; }
            if( std::fseek( bankfile, p_pkmnId * LEARNSET_SIZE * sizeof( u16 ), SEEK_SET ) ) {
                return false;
            }
            fread( p_out, sizeof( u16 ), LEARNSET_SIZE, bankfile );
        }
        return true;
    }
    const u16* getLearnset( u16 p_pkmnId, u8 p_forme ) {
        if( getLearnset( p_pkmnId, p_forme, LEARNSET_BUFFER ) ) { return LEARNSET_BUFFER; }
        return nullptr;
    }

} // namespace FS
