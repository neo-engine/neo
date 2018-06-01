/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : saveGame.cpp
    author      : Philip Wellnitz
    description : Functionality for parsing game save files

    Copyright (C) 2012 - 2018
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

#include <algorithm>
#include <nds.h>

#include "battleTrainer.h"
#include "fs.h"
#include "messageBox.h"
#include "pokemon.h"
#include "saveGame.h"
#include "uio.h"

namespace SAVE {
    std::unique_ptr<saveGame> SAV;

#define PKMN_DATALENGTH 128

    void saveGame::playerInfo::stepIncrease( ) {
        m_stepCount++;
        if( m_repelSteps > 0 ) {
            m_repelSteps--;
            if( !m_repelSteps ) {
                IO::messageBox( GET_STRING( 4 ), true );
                IO::NAV->draw( true );
            }
        }
        if( !m_stepCount ) {
            bool hasHatchSpdUp
                = m_bag.count( BAG::toBagType( item::itemType::KEY_ITEM ), I_OVAL_CHARM );
            for( size_t s = 0; s < 6; ++s ) {
                pokemon& ac = m_pkmnTeam[ s ];
                if( !ac.m_boxdata.m_speciesId ) break;
                hasHatchSpdUp |= ( ac.m_boxdata.m_ability == A_FLAME_BODY
                                   || ac.m_boxdata.m_ability == A_MAGMA_ARMOR );
            }

            for( size_t s = 0; s < 6; ++s ) {
                pokemon& ac = m_pkmnTeam[ s ];
                if( !ac.m_boxdata.m_speciesId ) break;

                if( ac.m_boxdata.m_individualValues.m_isEgg ) {
                    if( ac.m_boxdata.m_steps ) ac.m_boxdata.m_steps--;
                    if( hasHatchSpdUp && ac.m_boxdata.m_steps ) ac.m_boxdata.m_steps--;
                    if( !ac.m_boxdata.m_steps ) {
                        ac.hatch( );
                        break;
                    }
                } else
                    ac.m_boxdata.m_steps
                        = std::min( 255, ac.m_boxdata.m_steps + 1
                                             + ( ac.m_boxdata.m_holdItem == I_CLEAR_BELL ) );
            }
        }
    }

    u8 saveGame::playerInfo::getEncounterLevel( u8 p_tier ) {
        u8 mxlv = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( !m_pkmnTeam[ i ].m_boxdata.m_speciesId ) break;
            mxlv = std::max( mxlv, m_pkmnTeam[ i ].m_level );
        }
        if( !mxlv || m_repelSteps ) return 0;
        mxlv = std::min( 93, mxlv + 6 );
        mxlv = std::min( 5 * getBadgeCount( ) + 8, mxlv + 0 );

        mxlv += m_options.m_levelModifier + ( rand( ) % ( 2 * ( p_tier + 1 ) ) - p_tier - 1 );

        return std::max( (u8) 1, std::min( (u8) 100, mxlv ) );
    }

    bool saveGame::playerInfo::checkFlag( u8 p_idx ) {
        return m_flags[ p_idx >> 3 ] & ( 1 << ( p_idx % 8 ) );
    }
    void saveGame::playerInfo::setFlag( u8 p_idx, bool p_value ) {
        if( p_value != checkFlag( p_idx ) ) m_flags[ p_idx >> 3 ] ^= ( 1 << ( p_idx % 8 ) );
        return;
    }
    u8 saveGame::playerInfo::getBadgeCount( ) {
        u8 cnt = 0;
        for( u8 i = 0; i < 8; ++i ) {
            cnt += !!( m_HOENN_Badges & ( 1 << i ) );
            cnt += !!( m_KANTO_Badges & ( 1 << i ) );
            cnt += !!( m_JOHTO_Badges & ( 1 << i ) );
        }
        return cnt;
    }
    u8 saveGame::playerInfo::getTeamPkmnCount( ) {
        u8 res = 0;
        for( u8 i = 0; i < 6; ++i ) res += !!m_pkmnTeam[ i ].m_boxdata.m_speciesId;
        return res;
    }

    std::vector<pokemon>   tmp;
    BATTLE::battleTrainer* saveGame::playerInfo::getBattleTrainer( ) {
        tmp.clear( );
        for( u8 i = 0; i < 6; ++i )
            if( m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                tmp.push_back( m_pkmnTeam[ i ] );
            else
                break;

        return new BATTLE::battleTrainer( m_playername, "", "", "", "", tmp );
    }
    void saveGame::playerInfo::updateTeam( BATTLE::battleTrainer* p_trainer ) {
        for( u8 i = 0; i < p_trainer->m_pkmnTeam.size( ); ++i )
            m_pkmnTeam[ i ] = p_trainer->m_pkmnTeam[ i ];
    }

    u16 saveGame::getDexCount( ) {
        u16 cnt = 0;
        for( u16 i = 0; i < MAX_PKMN; ++i )
            if( IN_DEX( i ) ) cnt++;
        return cnt;
    }
    // Return the idx of the resulting Box
    s8 saveGame::storePkmn( const pokemon::boxPokemon& p_pokemon ) {
        s8 idx = m_storedPokemon[ getActiveFile( ).m_curBox ].getFirstFreeSpot( );
        u8 i   = 0;
        for( ; idx == -1 && i < MAX_BOXES; )
            idx = m_storedPokemon[ ( ( ++i ) + getActiveFile( ).m_curBox ) % MAX_BOXES ]
                      .getFirstFreeSpot( );
        if( idx == -1 ) // Everything's full :/
            return -1;
        getActiveFile( ).m_curBox = ( getActiveFile( ).m_curBox + i ) % MAX_BOXES;
        m_storedPokemon[ getActiveFile( ).m_curBox ][ idx ] = p_pokemon;
        return getActiveFile( ).m_curBox;
    }
    s8 saveGame::storePkmn( const pokemon& p_pokemon ) {
        return storePkmn( p_pokemon.m_boxdata );
    }
    BOX::box* saveGame::getCurrentBox( ) {
        return m_storedPokemon + getActiveFile( ).m_curBox;
    }

    u16 saveGame::countPkmn( u16 p_pkmnIdx ) {
        u16 res = 0;
        for( u8 i = 0; i < MAX_BOXES; i++ ) res += m_storedPokemon[ i ].count( p_pkmnIdx );
        for( u8 i = 0; i < 6; ++i ) {
            if( !getActiveFile( ).m_pkmnTeam[ i ].isEgg( )
                && getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId == p_pkmnIdx )
                ++res;
            if( !m_clipboard[ i ].isEgg( ) && m_clipboard[ i ].m_speciesId == p_pkmnIdx ) ++res;
        }
        return res;
    }
} // namespace SAVE
