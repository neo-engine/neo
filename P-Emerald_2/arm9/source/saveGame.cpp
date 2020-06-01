/*
    Pokémon neo
    ------------------------------

    file        : saveGame.cpp
    author      : Philip Wellnitz
    description : Functionality for parsing game save files

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
#include <nds.h>

#include "battleTrainer.h"
#include "fs.h"
#include "messageBox.h"
#include "saveGame.h"
#include "uio.h"
#include "itemNames.h"
#include "abilityNames.h"

namespace SAVE {
    saveGame SAV;

    void saveGame::playerInfo::clear( ) {
        std::memset( this, 0, sizeof( saveGame::playerInfo ) );
    }
    void saveGame::playerInfo::initialize( ) {
        clear( );
        m_good1 = GOOD_MAGIC1;
        m_good2 = GOOD_MAGIC2;

        m_bag = BAG::bag( );
        for( u8 i = 0; i < MAX_BOXES; ++i ) {
            sprintf( ( m_storedPokemon + i )->m_name, "Box %d", i + 1 );
            ( m_storedPokemon + i )->m_wallpaper = i;
        }
    }

    void saveGame::playerInfo::stepIncrease( ) {
        m_stepCount++;
        if( m_repelSteps > 0 ) {
            m_repelSteps--;
            if( !m_repelSteps ) {
                IO::messageBox( GET_STRING( 4 ), true );
                NAV::draw( true );
            }
        }
        if( !m_stepCount ) {
            bool hasHatchSpdUp
                = m_bag.count( BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_OVAL_CHARM );
            for( size_t s = 0; s < 6; ++s ) {
                pokemon& ac = m_pkmnTeam[ s ];
                if( !ac.m_boxdata.m_speciesId ) break;
                hasHatchSpdUp |= ( ac.m_boxdata.m_ability == A_FLAME_BODY
                                   || ac.m_boxdata.m_ability == A_MAGMA_ARMOR );
            }

            for( size_t s = 0; s < 6; ++s ) {
                pokemon& ac = m_pkmnTeam[ s ];
                if( !ac.m_boxdata.m_speciesId ) break;

                if( ac.isEgg( ) ) {
                    if( ac.m_boxdata.m_steps ) ac.m_boxdata.m_steps--;
                    if( hasHatchSpdUp && ac.m_boxdata.m_steps ) ac.m_boxdata.m_steps--;
                    if( !ac.m_boxdata.m_steps ) {
                        ac.hatch( );
                        break;
                    }
                } else
                    ac.m_boxdata.m_steps
                        = std::min( 255, ac.m_boxdata.m_steps + 1
                                             + ( ac.m_boxdata.m_heldItem == I_CLEAR_BELL ) );
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

    u16 saveGame::playerInfo::getDexCount( ) {
        u16 cnt = 0;
        for( u16 i = 0; i < MAX_PKMN; ++i )
            if( IN_DEX( i ) ) cnt++;
        return cnt;
    }
    // Return the idx of the resulting Box
    s8 saveGame::playerInfo::storePkmn( const boxPokemon& p_pokemon ) {
        s8 idx = m_storedPokemon[ m_curBox ].getFirstFreeSpot( );
        u8 i   = 0;
        for( ; idx == -1 && i < MAX_BOXES; )
            idx = m_storedPokemon[ ( ( ++i ) + m_curBox ) % MAX_BOXES ]
                      .getFirstFreeSpot( );
        if( idx == -1 ) // Everything's full :/
            return -1;
        m_curBox = ( m_curBox + i ) % MAX_BOXES;
        m_storedPokemon[ m_curBox ][ idx ] = p_pokemon;
        return m_curBox;
    }
    s8 saveGame::playerInfo::storePkmn( const pokemon& p_pokemon ) {
        return storePkmn( p_pokemon.m_boxdata );
    }
    BOX::box* saveGame::playerInfo::getCurrentBox( ) {
        return m_storedPokemon + m_curBox;
    }

    u16 saveGame::playerInfo::countPkmn( u16 p_pkmnIdx ) {
        u16 res = 0;
        for( u8 i = 0; i < MAX_BOXES; i++ ) res += m_storedPokemon[ i ].count( p_pkmnIdx );
        for( u8 i = 0; i < 6; ++i ) {
            if( !m_pkmnTeam[ i ].isEgg( )
                && m_pkmnTeam[ i ].m_boxdata.m_speciesId == p_pkmnIdx )
                ++res;
        }
        return res;
    }

    bool saveGame::isGood( ) {
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            if( SAV.m_saveFile[ i ].isGood( ) ) {
                return true;
            }
        }
        return false;
    }
    void saveGame::clear( ) {
        std::memset( this, 0, sizeof( saveGame ) );
    }
} // namespace SAVE
