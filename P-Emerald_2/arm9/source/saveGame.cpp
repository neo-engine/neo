/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.cpp
    author      : Philip Wellnitz
    description : Functionality for parsing game save files

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

#include <nds.h>

#include "saveGame.h"
#include "fs.h"
#include "uio.h"
#include "messageBox.h"
//#include "Gen.h"

namespace FS {
    const char* sav_nam = "p_smaragd_2";
    const char* sav_nam_2 = "p_smaragd_2.gba";
    SavMod savMod = _NDS;
    std::vector<pokemon> tmp;
    saveGame* SAV;

#define PKMN_DATALENGTH 128    
    saveGame* readSave( ) {
        FILE* f = open( "./", sav_nam, ".sav" );
        if( !f )
            return 0;

        saveGame* result = new saveGame( );
        read( f, result, sizeof( saveGame ), 1 );
        close( f );
        return result;
    }
    
    bool writeSave( saveGame* p_saveGame ) {
        FILE* f = open( "./", sav_nam, ".sav", "w" );
        if( !f )
            return 0;
        write( f, p_saveGame, sizeof( saveGame ), 1 );
        close( f );
        return true;
    }

    void saveGame::stepIncrease( ) {
        static u8 stepCnt = 0;
        stepCnt++;
        if( m_repelSteps > 0 ) {
            m_repelSteps--;
            if( !m_repelSteps ) {
                IO::messageBox( "Der Schutz ist aufgebraucht." );
                IO::drawSub( true );
            }
        }
        if( !stepCnt ) {
            bool hasHatchSpdUp = m_bag.count( BAG::toBagType( item::itemType::KEY_ITEM ), I_OVAL_CHARM );
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

    u8 saveGame::getEncounterLevel( u8 p_tier ) {
        u8 mxlv = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( !m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                break;
            mxlv = std::max( mxlv, m_pkmnTeam[ i ].m_Level );
        }
        if( !mxlv || m_repelSteps )
            return 0;
        mxlv = std::min( 93, mxlv + 6 );
        mxlv = std::min( 5 * getBadgeCount( ) + 8, mxlv + 0 );

        return mxlv + ( rand( ) % ( 2 * ( p_tier + 1 ) ) - p_tier - 1 );
    }
}
