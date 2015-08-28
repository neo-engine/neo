/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.cpp
    author      : Philip Wellnitz
    description : Functionality for parsing game save files

    Copyright (C) 2012 - 2015
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
#include <cstdio>
#include <nds.h>

#include "saveGame.h"
//#include "Gen.h"

namespace FS {
    std::string sav_nam = "./p_smaragd_2.sav";
    std::string sav_nam_2 = "./p_smaragd_2.gba.sav";
    SavMod savMod = _NDS;
    saveGame* SAV;

#define PKMN_DATALENGTH 128
    BAG::bag* readBag( FILE* p_file ) {
        static BAG::bag* result = new BAG::bag( );

        for( u8 i = 0; i < 5; ++i ) {
            size_t sz;
            fread( &sz, sizeof( size_t ), 1, p_file );
            for( size_t j = 0; j < sz; ++j ) {
                std::pair<u16, u16> ac;
                fread( &ac.first, sizeof( u16 ), 1, p_file );
                fread( &ac.second, sizeof( u16 ), 1, p_file );
                result->insert( BAG::bag::bagType( i ), ac.first, ac.second );
            }
        }

        return result;
    }

    BOX::box* readStoredPkmn( FILE* p_file ) {
        static BOX::box* result = new BOX::box( );
        pokemon::boxPokemon tmp;
        for( u16 i = 0; i < MAX_PKMN + 1; ++i ) {
            u16 cnt; fread( &cnt, sizeof( u16 ), 1, p_file );
            for( u16 j = 0; j < cnt; ++j ) {
                fread( &tmp, sizeof( pokemon::boxPokemon ), 1, p_file );
                result->insert( tmp );
            }
        }
        return result;
    }

    saveGame* readSave( ) {
        FILE* f = fopen( sav_nam.c_str( ), "rb" );

        if( !f )
            return 0;

        saveGame* result = new saveGame( );
        fread( result, sizeof( saveGame ), 1, f );

        result->m_bag = readBag( f );
        if( !result->m_bag )
            result->m_bag = new BAG::bag( );

        result->m_storedPokemon = readStoredPkmn( f );
        if( !result->m_storedPokemon )
            result->m_storedPokemon = new BOX::box( );

        fclose( f );
        return result;
    }

    bool writeBag( BAG::bag* p_bag, FILE* p_file ) {
        if( !p_bag )
            return false;

        for( u8 i = 0; i < 5; ++i ) {
            auto bg = p_bag->element( BAG::bag::bagType( i ) );
            auto sz = bg.size( );
            fwrite( &sz, sizeof( size_t ), 1, p_file );
            for( auto j : bg ) {
                fwrite( &j.first, sizeof( u16 ), 1, p_file );
                fwrite( &j.second, sizeof( u16 ), 1, p_file );
            }
        }
        return true;
    }

    bool writeStoredPkmn( BOX::box* p_stPkmn, FILE* p_file ) {
        if( !p_stPkmn )
            return false;

        for( u16 i = 0; i < MAX_PKMN + 1; ++i ) {
            u16 cnt = p_stPkmn->count( i );
            fwrite( &cnt, sizeof( u16 ), 1, p_file );
            for( auto tmp : (*p_stPkmn)[ i ] )
                fwrite( &tmp, sizeof( pokemon::boxPokemon ), 1, p_file );
        }
    }

    bool writeSave( saveGame* p_saveGame ) {
        FILE* f = fopen( sav_nam.c_str( ), "wb" );
        if( !f )
            return 0;
        fwrite( p_saveGame, sizeof( saveGame ), 1, f );
        writeBag( p_saveGame->m_bag, f );
        writeStoredPkmn( p_saveGame->m_storedPokemon, f );

        fclose( f );
        return true;
    }


    void saveGame::stepIncrease( ) {
        static u8 stepCnt = 0;
        stepCnt++;
        if( !stepCnt ) {
            bool hasHatchSpdUp = m_bag->count( BAG::toBagType( item::itemType::KEY_ITEM ), I_OVAL_CHARM );
            for( size_t s = 0; s < 6; ++s ) {
                pokemon& ac = m_pkmnTeam[ s ];
                if( !ac.m_boxdata.m_speciesId )
                    break;
                hasHatchSpdUp |= ( ac.m_boxdata.m_ability == A_FLAME_BODY
                                   || ac.m_boxdata.m_ability == A_MAGMA_ARMOR );
            }

            for( size_t s = 0; s < 6; ++s ) {
                pokemon& ac = m_pkmnTeam[ s ];
                if( !ac.m_boxdata.m_speciesId )
                    break;

                if( ac.m_boxdata.m_individualValues.m_isEgg ) {
                    if( ac.m_boxdata.m_steps )
                        ac.m_boxdata.m_steps--;
                    if( hasHatchSpdUp && ac.m_boxdata.m_steps )
                        ac.m_boxdata.m_steps--;
                    if( !ac.m_boxdata.m_steps ) {
                        ac.hatch( );
                        break;
                    }
                } else
                    ac.m_boxdata.m_steps = std::min( 255, ac.m_boxdata.m_steps + 1 + ( ac.m_boxdata.m_holdItem == I_CLEAR_BELL ) );
            }
        }
    }
}