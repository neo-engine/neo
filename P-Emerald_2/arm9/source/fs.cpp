/*
Pokémon neo
------------------------------

file        : fs.cpp
author      : Philip Wellnitz
description :

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
#include "move.h"
#include "pokemon.h"
#include "uio.h"

#include "messageBox.h"

const char PKMNDATA_PATH[] = "nitro:/PKMNDATA/";
const char SCRIPT_PATH[]   = "nitro:/MAPS/SCRIPTS/";

const char LOCATION_NAME_PATH[] = "nitro:/DATA/LOC_NAME/";
const char ITEM_NAME_PATH[]     = "nitro:/DATA/ITEM_NAME/";
const char ITEM_DATA_PATH[]     = "nitro:/DATA/ITEM_DATA/";
const char ABILITY_NAME_PATH[]  = "nitro:/DATA/ABTY_NAME/";
const char ABILITY_DSCR_PATH[]  = "nitro:/DATA/ABTY_DSCR/";
const char MOVE_NAME_PATH[]     = "nitro:/DATA/MOVE_NAME/";
const char MOVE_DSCR_PATH[]     = "nitro:/DATA/MOVE_DSCR/";
const char MOVE_DATA_PATH[]     = "nitro:/DATA/MOVE_DATA/";
const char POKEMON_NAME_PATH[]  = "nitro:/DATA/PKMN_NAME/";
const char POKEMON_DATA_PATH[]  = "nitro:/DATA/PKMN_DATA/";
const char PKMN_LEARNSET_PATH[] = "nitro:/DATA/PKMN_LEARN/";

const char BATTLE_TRAINER_PATH[] = "n/a";

namespace FS {
    char TMP_BUFFER[ 100 ];
    char TMP_BUFFER_SHORT[ 50 ];

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

    FILE* openScript( u8 p_bank, u8 p_mapX, u8 p_mapY, u8 p_relX, u8 p_relY, u8 p_id ) {
        snprintf( TMP_BUFFER_SHORT, 50, "%hhu/%hhu_%hhu/%hhu_%hhu_%hhu", p_bank, p_mapY, p_mapX,
                  p_relX, p_relY, p_id );
        return open( SCRIPT_PATH, TMP_BUFFER_SHORT, ".bin", "r" );
    }
    FILE* openScript( MAP::warpPos p_pos, u8 p_id ) {
        return openScript( p_pos.first, p_pos.second.m_posX, p_pos.second.m_posY, p_id );
    }
    FILE* openScript( u8 p_map, u16 p_globX, u16 p_globY, u8 p_id ) {
        return openScript( p_map, u8( p_globX / MAP::SIZE ), u8( p_globY / MAP::SIZE ),
                           u8( p_globX % MAP::SIZE ), u8( p_globY % MAP::SIZE ), p_id );
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

        dmaCopy( TEMP, p_layer, p_tileCnt );
        if( p_bottom )
            dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
        else
            dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize,
                          u16 p_palStart, u32 p_tileCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, 12288, TEMP, 256, TEMP_PAL ) )
            return false;

        dmaCopy( TEMP, p_layer, p_tileCnt );
        if( p_bottom )
            dmaCopy( TEMP_PAL + p_palStart, BG_PALETTE_SUB + p_palStart, p_palSize );
        else
            dmaCopy( TEMP_PAL + p_palStart, BG_PALETTE + p_palStart, p_palSize );
        return true;
    }

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

    bool readNop( FILE* p_file, u32 p_cnt ) {
        u8 tmp;
        for( u32 i = 0; i < p_cnt; ++i )
            if( !read( p_file, &tmp, sizeof( u8 ), 1 ) ) return false;
        return true;
    }

    bool readPal( FILE* p_file, MAP::palette* p_palette ) {
        if( p_file == 0 ) return false;
        read( p_file, p_palette, sizeof( u16 ) * 16, 6 );
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

    u8 readAnimations( FILE* p_file, MAP::tileSet::animation* p_animations ) {
        if( !p_file ) return 0;
        u8 N;
        fread( &N, sizeof( u8 ), 1, p_file );

        if( !p_animations ) return 0;

        N = std::min( N, MAP::MAX_ANIM_PER_TILE_SET );

        for( int i = 0; i < N; ++i ) {
            auto& a = p_animations[ i ];
            fread( &a.m_tileIdx, sizeof( u16 ), 1, p_file );
            fread( &a.m_speed, sizeof( u8 ), 1, p_file );
            fread( &a.m_maxFrame, sizeof( u8 ), 1, p_file );
            a.m_acFrame = 0;
            fread( ( (u8*) a.m_tiles ), sizeof( u8 ), a.m_maxFrame * 32, p_file );
        }
        fclose( p_file );
        return N;
    }

    bool readBankData( u8 p_bank, MAP::bankInfo& p_result ) {
        snprintf( TMP_BUFFER_SHORT, 45, "%hhu/%hhu", p_bank, p_bank );
        FILE* f = open( MAP::MAP_PATH, TMP_BUFFER_SHORT, ".bnk" );
        if( !f ) return false;
        read( f, &p_result.m_locationId, sizeof( u16 ), 1 );
        read( f, &p_result.m_battleBg, sizeof( u8 ), 1 );
        read( f, &p_result.m_mapType, sizeof( u8 ), 1 );
        for( u8 i = 0; i < MAX_MAP_LOCATIONS; ++i ) {
            read( f, &p_result.m_data[ i ].m_upperLeftX, sizeof( u16 ), 5 );
            read( f, &p_result.m_data[ i ].m_battleBg, sizeof( u8 ), 1 );
            read( f, &p_result.m_data[ i ].m_mapType, sizeof( u8 ), 1 );
        }
        close( f );
        return true;
    }

    bool getLocation( const u16 p_locationId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( LOCATION_NAME_PATH, p_locationId, ".str", 5000 );
        if( !f ) return false;

        for( int i = 0; i <= p_language; ++i ) { fread( p_out, 1, LOCATION_NAMELENGTH, f ); }
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
        if( CARD::checkCard( ) ) {
            CARD::readData( 0, reinterpret_cast<u8*>( &SAVE::SAV ), sizeof( SAVE::saveGame ) );
            if( SAVE::SAV.isGood( ) ) { return true; }
        }

        FILE* f = FS::open( "", p_path, ".sav" );
        if( !f ) return 0;

        FS::read( f, &SAVE::SAV, sizeof( SAVE::saveGame ), 1 );
        FS::close( f );
        return SAVE::SAV.isGood( );
    }

    bool writeSave( const char* p_path ) {
        return writeSave( p_path, []( u16, u16 ) {} );
    }

    bool writeSave( const char* p_path, std::function<void( u16, u16 )> p_progress ) {
        if( CARD::checkCard( ) ) {
            if( CARD::writeData( reinterpret_cast<u8*>( &SAVE::SAV ), sizeof( SAVE::saveGame ),
                                 p_progress ) ) {
                return true;
            }
        }

        FILE* f = FS::open( "", p_path, ".sav", "w" );
        if( !f ) return false;
        FS::write( f, &SAVE::SAV, sizeof( SAVE::saveGame ), 1 );
        FS::close( f );
        return true;
    }
} // namespace FS

namespace ITEM {
    bool getItemName( const u16 p_itemId, const u8 p_language, char* p_out ) {
        FILE* f = FS::openSplit( ITEM_NAME_PATH, p_itemId, ".str" );
        if( !f ) return false;

        for( int i = 0; i <= p_language; ++i ) { fread( p_out, 1, ITEM_NAMELENGTH, f ); }
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

        for( int i = 0; i <= p_language; ++i ) { fread( p_out, 1, MOVE_NAMELENGTH, f ); }
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

        for( int i = 0; i <= p_language; ++i ) { fread( p_out, 1, MOVE_DSCRLENGTH, f ); }
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
    battleTrainer getBattleTrainer( u16 p_battleTrainerId, u8 p_language ) {
        battleTrainer res;
        if( getBattleTrainer( p_battleTrainerId, p_language, &res ) ) { return res; }
        getBattleTrainer( 0, p_language, &res );
        return res;
    }
    bool getBattleTrainer( u16 p_battleTrainerId, u8 p_language, battleTrainer* p_out ) {
        FILE* f = FS::openSplit( BATTLE_TRAINER_PATH, p_battleTrainerId,
                                 ( "-" + std::to_string( p_language ) + ".data" ).c_str( ) );
        if( !f ) return false;
        fread( p_out, sizeof( battleTrainer ), 1, f );
        fclose( f );
        return true;
    }
} // namespace BATTLE

bool getAbilityName( u16 p_abilityId, u8 p_language, char* p_out ) {
    FILE* f = FS::openSplit( ABILITY_NAME_PATH, p_abilityId, ".str" );
    if( !f ) return false;

    for( u8 i = 0; i <= p_language; ++i ) { fread( p_out, 1, ABILITY_NAMELENGTH, f ); }
    fclose( f );
    return true;
}

std::string getAbilityName( u16 p_abilityId, u8 p_language ) {
    char tmpbuf[ ABILITY_NAMELENGTH ];
    if( !getAbilityName( p_abilityId, p_language, tmpbuf ) ) { return "---"; }
    return std::string( tmpbuf );
}

std::string getAbilityName( u16 p_abilityId ) {
    return getAbilityName( p_abilityId, CURRENT_LANGUAGE );
}

bool getAbilityDescr( u16 p_abilityId, u8 p_language, char* p_out ) {
    FILE* f = FS::openSplit( ABILITY_DSCR_PATH, p_abilityId, ".str" );
    if( !f ) return false;

    for( u8 i = 0; i <= p_language; ++i ) { fread( p_out, 1, ABILITY_DSCRLENGTH, f ); }
    fclose( f );
    return true;
}

std::string getAbilityDescr( u16 p_abilityId, u8 p_language ) {
    char tmpbuf[ ABILITY_DSCRLENGTH ];
    if( !getAbilityDescr( p_abilityId, p_language, tmpbuf ) ) { return "---"; }
    return std::string( tmpbuf );
}

std::string getAbilityDescr( u16 p_abilityId ) {
    return getAbilityDescr( p_abilityId, CURRENT_LANGUAGE );
}

std::string getDisplayName( u16 p_pkmnId, u8 p_language, u8 p_forme ) {
    char tmpbuf[ 20 ];
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

    for( int i = 0; i <= p_language; ++i ) {
        assert( PKMN_NAMELENGTH + ( 5 * !!p_forme )
                == fread( p_out, 1, PKMN_NAMELENGTH + ( 5 * !!p_forme ), f ) );
    }
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

bool getAll( u16 p_pkmnId, pokemonData& p_out, u8 p_forme ) {
    FILE* f;
    if( p_forme ) {
        snprintf( FS::TMP_BUFFER_SHORT, 13, "%02hu/%hu-%hhu", p_pkmnId / FS::ITEMS_PER_DIR,
                  p_pkmnId, p_forme );
        f = FS::open( PKMNDATA_PATH, FS::TMP_BUFFER_SHORT, ".data" );
    }

    if( !p_forme || !f ) f = FS::openSplit( PKMNDATA_PATH, p_pkmnId, ".data" );
    if( !f ) return false;

    FS::read( f, &p_out, sizeof( pokemonData ), 1 );
    FS::close( f );
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
