/*
Pokémon Emerald 2 Version
------------------------------

file        : fs.cpp
author      : Philip Wellnitz
description :

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

#include <string>
#include <vector>
#include <initializer_list>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include "fs.h"
#include "ability.h"
#include "move.h"
#include "pokemon.h"
#include "item.h"
#include "berry.h"
#include "uio.h"
#include "defines.h"
#include "mapSlice.h"

#include "messageBox.h"

const char ITEM_PATH[ ] = "nitro:/ITEMS/";
const char PKMNDATA_PATH[ ] = "nitro:/PKMNDATA/";
const char ABILITYDATA_PATH[ ] = "nitro:/PKMNDATA/ABILITIES/";

ability::ability( int p_abilityId ) {
    FILE* f = FS::open( ABILITYDATA_PATH, p_abilityId, ".data" );

    if( !f )
        return;

    m_abilityName = FS::readString( f, true );
    m_flavourText = FS::readString( f, true );
    u32 tmp;
    fscanf( f, "%lu", &tmp );
    m_type = static_cast<ability::abilityType>( tmp );
    FS::close( f );
}

std::string getAbilityName( int p_abilityId ) {
    FILE* f = FS::open( ABILITYDATA_PATH, p_abilityId, ".data" );

    if( !f )
        return "---";
    auto ret = FS::readString( f, false );
    FS::close( f );
    return ret;
}

namespace FS {
    bool exists( const char* p_path, const char* p_name ) {
        FILE* fd = open( p_path, p_name );
        if( !fd )
            return false;
        fclose( fd );
        return true;
    }
    bool exists( const char* p_path, u16 p_name, bool p_unused ) {
        (void) p_unused;

        FILE* fd = open( p_path, p_name );
        if( !fd )
            return false;
        fclose( fd );
        return true;
    }
    bool exists( const char* p_path, u16 p_pkmnIdx, const char* p_name ) {
        char* buffer = new char[ 100 ];
        snprintf( buffer, 99, "%s%d/%d%s.raw", p_path, p_pkmnIdx, p_pkmnIdx, p_name );
        FILE* fd = fopen( buffer, "rb" );
        delete[ ] buffer;
        if( fd == 0 ) {
            fclose( fd );
            return false;
        }
        fclose( fd );
        return true;
    }

    FILE* open( const char* p_path, const char* p_name, const char* p_ext, const char* p_mode ) {
        char* buffer = new char[ 100 ];
        snprintf( buffer, 99, "%s%s%s", p_path, p_name, p_ext );
        auto res = fopen( buffer, p_mode );
        delete[ ] buffer;
        return res;
    }
    FILE* open( const char* p_path, u16 p_value, const char* p_ext, const char* p_mode ) {
        char* buffer = new char[ 100 ];
        snprintf( buffer, 99, "%s%d%s", p_path, p_value, p_ext );
        auto res = fopen( buffer, p_mode );
        delete[ ] buffer;
        return res;
    }
    void close( FILE* p_file ) {
        fclose( p_file );
    }
    size_t read( FILE* p_stream, void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream )
            return 0;
        return fread( p_buffer, p_size, p_count, p_stream );
    }
    size_t write( FILE* p_stream, const void* p_buffer, size_t p_size, size_t p_count ) {
        if( !p_stream )
            return 0;
        return fwrite( p_buffer, p_size, p_count, p_stream );
    }

    bool readData( const char* p_path, const char* p_name, unsigned short p_dataCnt, unsigned short* p_data ) {
        FILE* fd = open( p_path, p_name );
        if( !fd )
            return false;
        read( fd, p_data, sizeof( unsigned short ), p_dataCnt );
        close( fd );
        return true;
    }

    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name, const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, p_tileCnt, TEMP, p_palCnt, TEMP_PAL ) )
            return false;

        IO::copySpriteData( TEMP, p_spriteInfo->m_entry->gfxIndex, 4 * p_tileCnt, p_bottom );
        IO::copySpritePal( TEMP_PAL, p_spriteInfo->m_entry->palette, 2 * p_palCnt, p_bottom );
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize, u32 p_tileCnt, bool p_bottom ) {
        if( !readData<unsigned int, unsigned short>( p_path, p_name, 12288, TEMP, 256, TEMP_PAL ) )
            return false;

        dmaCopy( TEMP, p_layer, p_tileCnt );
        if( p_bottom )
            dmaCopy( TEMP_PAL, BG_PALETTE_SUB, p_palSize );
        else
            dmaCopy( TEMP_PAL, BG_PALETTE, p_palSize );
        return true;
    }

    bool readPictureData( u16* p_layer, const char* p_path, const char* p_name, u16 p_palSize, u16 p_palStart, u32 p_tileCnt, bool p_bottom ) {
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
        if( p_no == SAVE::SAV->getActiveFile( ).m_options.m_bgIdx && IO::NAV_DATA[ 0 ] ) {
            dmaCopy( IO::NAV_DATA, p_layer, 256 * 192 );
            dmaCopy( IO::NAV_DATA_PAL, BG_PAL( !SCREENS_SWAPPED ), 192 * 2 );
            return true;
        }

        if( !readData( "nitro:/PICS/NAV/", p_name, (unsigned int) 12288, IO::NAV_DATA, (unsigned short) 192, IO::NAV_DATA_PAL ) )
            return false;

        dmaCopy( IO::NAV_DATA, p_layer, 256 * 192 );
        dmaCopy( IO::NAV_DATA_PAL, BG_PAL( !SCREENS_SWAPPED ), 192 * 2 );

        return true;
    }

    bool readNop( FILE* p_file, u32 p_cnt ) {
        if( p_file == 0 )
            return false;
        read( p_file, 0, 1, p_cnt );
        return true;
    }

    bool readPal( FILE* p_file, MAP::palette* p_palette ) {
        if( p_file == 0 )
            return false;
        read( p_file, p_palette, sizeof( u16 ) * 16, 6 );
        return true;
    }

    bool readTiles( FILE* p_file, MAP::tile* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 )
            return false;
        read( p_file, p_tileSet + p_startIdx, sizeof( MAP::tile )*p_size, 1 );
        return true;
    }

    bool readBlocks( FILE* p_file, MAP::block* p_tileSet, u16 p_startIdx, u16 p_size ) {
        if( p_file == 0 )
            return false;
        readNop( p_file, 4 );
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottom, 4 * sizeof( MAP::blockAtom ), 1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_top, 4 * sizeof( MAP::blockAtom ), 1 );
        }
        for( u16 i = 0; i < p_size; ++i ) {
            read( p_file, &( p_tileSet + p_startIdx + i )->m_bottombehave, sizeof( u8 ), 1 );
            read( p_file, &( p_tileSet + p_startIdx + i )->m_topbehave, sizeof( u8 ), 1 );
        }
        return true;
    }

    u8 readAnimations( FILE* p_file, MAP::tileSet::animation* p_animations ) {
        if( !p_file )
            return 0;
        u8 N;
        fread( &N, sizeof( u8 ), 1, p_file );

        if( !p_animations )
            return 0;

        N = std::min( N, MAP::MAX_ANIM_PER_TILE_SET );

        for( int i = 0; i < N; ++i ) {
            auto& a = p_animations[ i ];
            fread( &a.m_tileIdx, sizeof( u16 ), 1, p_file );
            fread( &a.m_speed, sizeof( u8 ), 1, p_file );
            fread( &a.m_maxFrame, sizeof( u8 ), 1, p_file );
            a.m_acFrame = 0;
            fread( a.m_tiles, sizeof( MAP::tile ), a.m_maxFrame, p_file );
        }
        fclose( p_file );
        return N;
    }

    std::string readString( FILE* p_file, bool p_new ) {
        std::string ret = "";
        int ac;

        while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );

        if( ac == '*' || ac == EOF ) {
            return ret;
        } else ret += ac;

        while( ( ac = fgetc( p_file ) ) != '*' && ac != EOF ) {
            if( ac == '|' )
                ret += (char) 136;
            else if( ac == '#' )
                ret += (char) 137;
            else
                ret += ac;
        }
        if( !p_new )
            return convertToOld( ret );
        else
            return ret;
    }

    std::string breakString( const std::string& p_string, u8 p_lineLength ) {
        std::string result = "";

        u8 acLineLength = 0;
        std::string tmp = "";
        for( auto c : p_string ) {
            if( c == ' ' ) {
                if( acLineLength + tmp.length( ) > p_lineLength ) {
                    if( acLineLength ) {
                        result += "\n" + tmp + " ";
                        acLineLength = tmp.length( );
                        tmp = "";
                    } else {
                        result += tmp + "\n";
                        acLineLength = 0;
                        tmp = "";
                    }
                } else {
                    result += tmp + ' ';
                    tmp = "";
                    acLineLength += tmp.length( ) + 1;
                }
            } else
                tmp += c;
        }

        if( acLineLength + tmp.length( ) > p_lineLength  && acLineLength )
            result += "\n" + tmp + " ";
        else
            result += tmp;
        return result;
    }

    std::string breakString( const std::string& p_string, IO::font* p_font, u8 p_lineLength ) {
        std::string result = "";

        u8 acLineLength = 0;
        std::string tmp = "";
        for( auto c : p_string ) {
            if( c == ' ' ) {
                u8 tmpLen = p_font->stringWidth( tmp.c_str( ) );
                if( acLineLength + tmpLen > p_lineLength ) {
                    if( acLineLength ) {
                        result += "\n" + tmp + " ";
                        acLineLength = tmpLen;
                        tmp = "";
                    } else {
                        result += tmp + "\n";
                        acLineLength = 0;
                        tmp = "";
                    }
                } else {
                    result += tmp + ' ';
                    tmp = "";
                    acLineLength += tmpLen;
                }
            } else
                tmp += c;
        }

        if( acLineLength + p_font->stringWidth( tmp.c_str( ) ) > p_lineLength  && acLineLength )
            result += "\n" + tmp + " ";
        else
            result += tmp;
        return result;
    }

    std::string convertToOld( const std::string& p_string ) {
        std::string ret = "";
        for( auto ac = p_string.begin( ); ac != p_string.end( ); ++ac ) {
            if( *ac == 'ä' )
                ret += '\x84';
            else if( *ac == 'Ä' )
                ret += '\x8E';
            else if( *ac == 'ü' )
                ret += '\x81';
            else if( *ac == 'Ü' )
                ret += '\x9A';
            else if( *ac == 'ö' )
                ret += '\x94';
            else if( *ac == 'Ö' )
                ret += '\x99';
            else if( *ac == 'ß' )
                ret += '\x9D';
            else if( *ac == 'é' )
                ret += '\x82';
            else if( *ac == '%' )
                ret += ' ';
            else if( *ac == '|' )
                ret += (char) 136;
            else if( *ac == '#' )
                ret += (char) 137;
            else if( *ac == '\r' )
                ret += "";
            else
                ret += *ac;
        }
        return ret;
    }

    std::string getLocation( u16 p_ind ) {
        if( p_ind > 5000 )
            return FARAWAY_PLACE;
        FILE* f = FS::open( "nitro:/LOCATIONS/", p_ind, ".data" );

        if( !f ) {
            if( p_ind > 322 && p_ind < 1000 )
                return getLocation( 3002 );

            return FARAWAY_PLACE;
        }
        auto res = readString( f, true );
        fclose( f );
        return res;
    }

    std::unique_ptr<SAVE::saveGame> readSave( const char* p_path ) {
        FILE* f = FS::open( "", p_path, ".sav" );
        if( !f )
            return 0;

        std::unique_ptr<SAVE::saveGame> result = std::unique_ptr<SAVE::saveGame>( new SAVE::saveGame( ) );
        FS::read( f, result.get( ), sizeof( SAVE::saveGame ), 1 );
        FS::close( f );
        return result;
    }

    bool writeSave( std::unique_ptr<SAVE::saveGame>& p_saveGame, const char* p_path ) {
        FILE* f = FS::open( "", p_path, ".sav", "w" );
        if( !f )
            return 0;
        FS::write( f, p_saveGame.get( ), sizeof( SAVE::saveGame ), 1 );
        FS::close( f );
        return true;
    }
}

std::string toString( u16 p_num ) {
    char buffer[ 32 ];
    sprintf( buffer, "%hu", p_num );
    return std::string( buffer );
}

std::string getDisplayName( u16 p_pkmnId ) {
    pokemonData tmp;
    if( !getAll( p_pkmnId, tmp ) ) {
        return "???";
    }
    return tmp.m_displayName;
}
void getDisplayName( u16 p_pkmnId, char* p_name ) {
    pokemonData tmp;
    if( !getAll( p_pkmnId, tmp ) ) {
        strcpy( p_name, "???" );
        return;
    }
    strcpy( p_name, tmp.m_displayName );
}

bool getAll( u16 p_pkmnId, pokemonData& p_out ) {
    FILE* f = FS::open( PKMNDATA_PATH, p_pkmnId, ".data" );
    if( f == 0 )
        return false;

    FS::read( f, &p_out, sizeof( pokemonData ), 1 );
    FS::close( f );
    return true;
}

void getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_mode, u16 p_amount, u16* p_result ) {
    FILE* f = FS::open( ( std::string( PKMNDATA_PATH ) + "/LEARNSETS/" ).c_str( ), p_pkmnId, ".learnset.data" );
    if( !f )
        return;

    u16* buffer = new u16[ 700 ];
    FS::read( f, buffer, sizeof( u16 ), 699 );
    FS::close( f );
    u16 ptr = 0;

    u16 rescnt = 0;
    for( u8 i = 0; i < p_amount; ++i )
        p_result[ i ] = 0;

    if( p_fromLevel > p_toLevel ) {
        std::vector<u16> reses;
        for( u16 i = 0; i <= p_fromLevel; ++i ) {
            u16 z = buffer[ ptr++ ];
            for( int j = 0; j < z; ++j ) {
                u16 g = buffer[ ptr++ ], h = buffer[ ptr++ ];
                if( i >= p_toLevel && h == (u16) p_mode && g < MAX_ATTACK )
                    reses.push_back( g );
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
N:
            ;
        }
        delete[ ] buffer;
        return;
    } else {
        for( u16 i = 0; i <= p_toLevel; ++i ) {
            u16 z = buffer[ ptr++ ];
            for( u16 j = 0; j < z; ++j ) {
                u16 g = buffer[ ptr++ ], h = buffer[ ptr++ ];
                if( i >= p_fromLevel && h == p_mode && g < MAX_ATTACK ) {
                    for( u16 k = 0; k < rescnt; ++k )
                        if( g == p_result[ k ] )
                            goto NEXT;
                    p_result[ rescnt ] = g;
                    if( ++rescnt == p_amount ) {
                        delete[ ] buffer;
                        return;
                    }
NEXT:
                    ;
                }
            }
        }
    }
    delete[ ] buffer;
}
bool canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_mode ) {
    FILE* f = FS::open( ( std::string( PKMNDATA_PATH ) + "/LEARNSETS/" ).c_str( ), p_pkmnId, ".learnset.data" );
    if( !f )
        return false;

    u16* buffer = new u16[ 700 ];
    FS::read( f, buffer, sizeof( u16 ), 699 );
    FS::close( f );
    u16 ptr = 0;

    for( int i = 0; i <= 100; ++i ) {
        int z = buffer[ ptr++ ];
        for( int j = 0; j < z; ++j ) {
            u16 g = buffer[ ptr++ ], h = buffer[ ptr++ ];
            if( g == p_moveId && h == p_mode ) {
                delete[ ] buffer;
                return true;
            }
        }
    }
    delete[ ] buffer;
    return false;
}

u16 item::getItemId( ) {
    for( u16 i = 0; i < MAX_ITEMS; ++i )
        if( ItemList[ i ]->m_itemName == m_itemName )
            return i;
    return 0;
}

bool item::load( ) {
    if( m_loaded )
        return true;
    FILE* f = FS::open( ITEM_PATH, m_itemName.c_str( ), ".data" );
    if( !f )
        return false;

    memset( &m_itemData, 0, sizeof( itemData ) );
    u8 tmp;
    fscanf( f, "%hhu %lu %lu\n", &tmp,
            &m_itemData.m_price, &m_itemData.m_itemEffect );
    m_itemData.m_itemEffectType = static_cast<item::itemEffectType>( tmp );
    strcpy( m_itemData.m_itemDisplayName, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemDescription, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemShortDescr, FS::readString( f, true ).c_str( ) );
    FS::close( f );
    return m_loaded = true;
}

bool berry::load( ) {
    if( m_loaded )
        return true;
    FILE* f = FS::open( ITEM_PATH, m_itemName.c_str( ), ".data" );
    if( !f )
        return false;

    memset( &m_itemData, 0, sizeof( itemData ) );
    u8 tmp;
    fscanf( f, "%hhu %lu %lu\n", &tmp,
            &m_itemData.m_price, &m_itemData.m_itemEffect );
    m_itemData.m_itemEffectType = static_cast<item::itemEffectType>( tmp );
    strcpy( m_itemData.m_itemDisplayName, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemDescription, FS::readString( f, true ).c_str( ) );
    strcpy( m_itemData.m_itemShortDescr, FS::readString( f, true ).c_str( ) );

    memset( &m_berryData, 0, sizeof( berryData ) );
    fscanf( f, "%hu %hhu", &m_berryData.m_berrySize, &tmp );
    m_berryData.m_berryGuete = static_cast<berry::berryGueteType>( tmp );
    fscanf( f, "%hhu %hhu", &tmp, &m_berryData.m_naturalGiftStrength );
    m_berryData.m_naturalGiftType = static_cast<type>( tmp );

    for( u8 i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &m_berryData.m_berryTaste[ i ] );
    fscanf( f, "%hhu %hhu %hhu", &m_berryData.m_hoursPerGrowthStage,
            &m_berryData.m_minBerries, &m_berryData.m_maxBerries );
    FS::close( f );
    return m_loaded = true;
}

std::string item::getDisplayName( bool p_new ) {
    if( !m_loaded && !load( ) )
        return m_itemName;
    if( p_new )
        return std::string( m_itemData.m_itemDisplayName );
    else
        return FS::convertToOld( std::string( m_itemData.m_itemDisplayName ) );
}

std::string item::getDescription( ) {
    if( !m_loaded && !load( ) )
        return NO_DATA;
    return std::string( m_itemData.m_itemDescription );
}

std::string item::getShortDescription( ) {
    if( !m_loaded && !load( ) )
        return NO_DATA;
    return std::string( m_itemData.m_itemShortDescr );
}


u32 item::getEffect( ) {
    if( !m_loaded && !load( ) )
        return 0;
    return m_itemData.m_itemEffect;
}

item::itemEffectType item::getEffectType( ) {
    if( !m_loaded && !load( ) )
        return itemEffectType::NONE;
    return m_itemData.m_itemEffectType;
}

u32 item::getPrice( ) {
    if( !m_loaded && !load( ) )
        return 0;
    return m_itemData.m_price;
}
