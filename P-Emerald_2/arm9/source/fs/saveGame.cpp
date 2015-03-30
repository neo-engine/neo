/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for parsing game save files

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



#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fat.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <nds.h>
#include <fcntl.h>
#include <unistd.h>

#include "pokemon.h"
#include "saveGame.h"
#include "Gen.h"

std::string sav_nam = "./p_smaragd_2.sav";/*"nitro:/SAV";*/ //as nitro:/SAV doesn't seem to work :( ...
std::string sav_nam_2 = "./p_smaragd_2.gba.sav";/*"nitro:/SAV";*/ //as nitro:/SAV doesn't seem to work :( ...
extern SavMod savMod;
#define PKMN_DATALENGTH 128
extern const u16 POKEMON::OTLENGTH;
extern const u16 POKEMON::PKMN_NAMELENGTH;

extern POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
extern std::vector<int> box_of_st_pkmn[ MAXPKMN ];
extern std::vector<int> free_spaces;

extern "C"{    extern long hexdec( unsigned const char *p_hex ); }

saveGame::saveGame( void p_func( int ) ) {
    u16 booltemp = true;
    FILE* fd = fopen( &sav_nam[ 0 ], "r" );
    if( fd == 0 ) {
        m_good = false;
        m_savTyp = 0;
        return;
    }
    fscanf( fd, "%i ", &m_savTyp );
    if( m_savTyp == 0 ) {
        m_good = true;
        return;
    }
    wchar_t buf[ POKEMON::OTLENGTH ];
    fscanf( fd, "%ls", buf );
    setName( std::wstring( buf ) );
    fscanf( fd, "%i %i", &m_Id, &m_Sid );
    p_func( 10 );
    fscanf( fd, "%i %i", &booltemp, &m_HOENN_Badges );
    m_isMale = bool( booltemp );
    p_func( 20 );
    fscanf( fd, "%i %i %hi %i %i %u", &m_KANTO_Badges, &m_JOHTO_Badges, &m_badges, &m_dex, &m_money, &m_playtime );
    fscanf( fd, "%i", &booltemp );
    m_hasPKMN = bool( booltemp );
    p_func( 30 );

    fscanf( fd, "%i %i %i %i %s %i ", &m_acposx, &m_acposy, &m_acposz, &m_acMapIdx, m_acMapName, &m_acMoveMode );
    fscanf( fd, "%hu", &m_overWorldIdx );

    u16 a;
    for( u8 i = 0; i < 8; ++i ) {
        fscanf( fd, " %i ", &a );
        m_bag.m_bags[ i ].assign( a, std::pair<u16, u16>( 0, 0 ) );
        fread( &m_bag.m_bags[ i ][ 0 ], sizeof( std::pair<u16, u16> ), a, fd );
    }

    fscanf( fd, " %i ", &a );
    m_PkmnTeam = std::vector<POKEMON::pokemon>( a );
    for( u16 i = 0; i < a; ++i )
        fread( &m_PkmnTeam[ i ], sizeof( POKEMON::pokemon ), 1, fd );
    m_hasPKMN = !!a;

    free_spaces.clear( );
    for( u16 i = 0; i < MAXSTOREDPKMN; i++ ) {
        //stored_pkmn[i] = pokemon(fd);
        fread( &stored_pkmn[ i ], sizeof( POKEMON::pokemon::boxPokemon ), 1, fd );
        if( stored_pkmn[ i ].m_speciesId == 0 || stored_pkmn[ i ].m_speciesId > MAXPKMN )
            free_spaces.push_back( i );
        else
            box_of_st_pkmn[ stored_pkmn[ i ].m_speciesId ].push_back( i );
    }
    p_func( 40 );

    u32 inDextmp;
    for( u8 i = 0; i < 23; i++ ) {
        fscanf( fd, " %lu", &inDextmp );
        for( u8 j = 0; j < 32; ++j )
            m_inDex[ 32 * i + j ] = ( inDextmp & ( 1 << j ) );
        p_func( 40 + ( 30 * ( i / 23 ) ) );
    }

    fscanf( fd, " %i", &booltemp );
    m_hasGDex = bool( booltemp );
    fread( m_flags, 1, 1000, fd );
    p_func( 100 );
    fclose( fd );
    m_good = true;

    //todo
    m_EXPShareEnabled = false;
    m_evolveInBattle = true;
}

bool saveGBA( void p_func( int ) ) {
    FILE* fd = fopen( &sav_nam_2[ 0 ], "wb" );
    if( fd == 0 )
        return false;
    fwrite( gen3::SaveParser::Instance( )->unpackeddata, sizeof( u8 ), 14 * sizeof( gen3::block ), fd );
    fclose( fd );
    return true;
}

bool saveGame::save( void p_func( int ) ) {
    if( savMod == _GBA )
        return saveGBA( p_func );

    u16 booltemp;
    FILE* fd = fopen( &sav_nam[ 0 ], "w" );
    if( fd == 0 )
        return false;
    fprintf( fd, "%i ", m_savTyp );
    for( size_t i = 0; i < m_playername.size( ); ++i )
        fprintf( fd, "%lc", m_playername[ i ] );
    for( size_t i = m_playername.size( ); i < POKEMON::OTLENGTH; ++i )
        fprintf( fd, "%lc", 0 );
    fprintf( fd, " " );
    fprintf( fd, "%i %i ", m_Id, m_Sid );
    p_func( 10 );
    booltemp = m_isMale;
    fprintf( fd, "%i %i ", booltemp, m_HOENN_Badges );
    p_func( 20 );
    fprintf( fd, "%i %i %hi %i ", m_KANTO_Badges, m_JOHTO_Badges, m_badges, m_dex );
    booltemp = m_hasPKMN;
    fprintf( fd, "%i %u %i ", m_money, m_playtime, booltemp );
    p_func( 30 );

    fprintf( fd, "%i %i %i %i %s %i ", m_acposx, m_acposy, m_acposz, m_acMapIdx, m_acMapName, m_acMoveMode );
    fprintf( fd, "%hu", m_overWorldIdx );

    for( u8 i = 0; i < 8; ++i ) {
        fprintf( fd, " %i ", m_bag.m_bags[ i ].size( ) );
        fwrite( m_bag.m_bags[ i ].data( ), sizeof( std::pair<u16, u16> ), m_bag.m_bags[ i ].size( ), fd );
    }

    fprintf( fd, " %i ", m_PkmnTeam.size( ) );
    for( size_t i = 0; i < m_PkmnTeam.size( ); ++i )
        //I->save(fd);
        fwrite( &( m_PkmnTeam[ i ] ), 1, sizeof( POKEMON::pokemon ), fd );
    for( u16 i = 0; i < MAXSTOREDPKMN; i++ ) {
        //stored_pkmn[i].save(fd);
        fwrite( &( stored_pkmn[ i ] ), sizeof( POKEMON::pokemon::boxPokemon ), 1, fd );
    }

    p_func( 40 );
    u32 inDextmp;
    for( u8 i = 0; i < 23; i++ ) {
        inDextmp = 0;
        for( u8 j = 0; j < 32; ++j )
            inDextmp |= ( m_inDex[ 32 * i + j ] << j );
        fprintf( fd, " %lu", &inDextmp );
    }
    p_func( 50 );


    booltemp = m_hasGDex;
    fprintf( fd, " %i", booltemp );

    fwrite( m_flags, 1, 1000, fd );
    p_func( 100 );
    fclose( fd );
    return true;
}