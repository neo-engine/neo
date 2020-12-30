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

#include "abilityNames.h"
#include "battleTrainer.h"
#include "fs.h"
#include "item.h"
#include "itemNames.h"
#include "nav.h"
#include "saveGame.h"
#include "screenFade.h"
#include "uio.h"

namespace SAVE {
    saveGame   SAV;
    SAVE::date CURRENT_DATE;
    SAVE::time CURRENT_TIME;

    // Time (in h) for a berry to grow to the next stage.
    constexpr u8 BERRY_GROWTH_TIME[ 80 ]
        = { 0,  12, 12, 12, 12, 12, 16, 12,  12,  48,  24,  24,  24, 24, 24, 24, 4,
            4,  4,  4,  4,  12, 12, 12, 12,  12,  24,  24,  24,  24, 24, 72, 72, 72,
            72, 72, 32, 32, 32, 32, 32, 32,  32,  32,  32,  32,  32, 32, 32, 32,

            32, 32, 32, 32, 96, 96, 96, 96,  96,  96,  96,  96,  96, 96, 96, 96, 96,
            96, 56, 56, 56, 56, 56, 56, 108, 108, 108, 108, 108, 108 };

    void saveGame::playerInfo::drawTrainersCard( bool p_bottom, bool p_dummy ) {
        IO::initVideo( true );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        u8           achvs = getAchievementCount( );
        SpriteEntry* oam   = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        IO::initOAMTable( p_bottom );
        u16 tileCnt = 0;

        dmaFillWords( 0, bgGetGfxPtr( p_bottom ? IO::bg2sub : IO::bg2 ), 256 * 192 );
        u16* pal = IO::BG_PAL( p_bottom );

        pal[ 0 ]             = 0;
        pal[ IO::WHITE_IDX ] = IO::WHITE;
        pal[ IO::GRAY_IDX ]  = IO::GRAY;
        pal[ IO::BLACK_IDX ] = IO::BLACK;
        pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
        pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
        pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
        pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( 0, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::BLACK_IDX, 1 );
        IO::boldFont->setColor( IO::WHITE_IDX, 2 );

        if( m_gameType == UNUSED ) {
            IO::boldFont->setColor( IO::GRAY_IDX, 1 );
            IO::boldFont->setColor( IO::WHITE_IDX, 2 );
            IO::boldFont->printString( STRINGS[ 412 ][ m_options.m_language ], 128, 90, p_bottom,
                                       IO::font::CENTER );
            IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
            return;
        }

        FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/id",
                             std::to_string( achvs + 1 ).c_str( ), 440 );

        pal[ 0 ]             = 0;
        pal[ IO::WHITE_IDX ] = IO::WHITE;
        pal[ IO::GRAY_IDX ]  = IO::GRAY;
        pal[ IO::BLACK_IDX ] = IO::BLACK;
        pal[ IO::BLUE_IDX ]  = IO::RGB( 18, 22, 31 );
        pal[ IO::RED_IDX ]   = IO::RGB( 31, 18, 18 );
        pal[ IO::BLUE2_IDX ] = IO::RGB( 0, 0, 25 );
        pal[ IO::RED2_IDX ]  = IO::RGB( 23, 0, 0 );

        // player sprite
        if( !p_dummy ) {
            tileCnt = IO::loadTrainerSprite( m_appearance, 33, 45, 0, 0, tileCnt, p_bottom );
        }

        // stars
        IO::loadSprite( 1, 1, tileCnt, 100, 37, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_2,
                        p_bottom );
        IO::loadSprite( 2, 1, tileCnt, 112, 37, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_2,
                        p_bottom );
        IO::loadSprite( 3, 1, tileCnt, 124, 37, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_2,
                        p_bottom );
        IO::loadSprite( 4, 1, tileCnt, 136, 37, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_2,
                        p_bottom );
        IO::loadSprite( 5, 1, tileCnt, 148, 37, 16, 16, 0, 0, 0, false, false, true, OBJPRIORITY_2,
                        p_bottom );
        tileCnt = IO::loadSprite( "UI/star", 6, 1, tileCnt, 160, 37, 16, 16, false, false, true,
                                  OBJPRIORITY_2, p_bottom );

        for( u8 i = 0; i < achvs; ++i ) { oam[ 1 + i ].isHidden = false; }

        // Text

        char buffer[ 100 ];

        // "TRAINER'S CARD"
        IO::boldFont->printString( STRINGS[ 405 ][ m_options.m_language ], 112, 13, p_bottom,
                                   IO::font::CENTER );

        if( achvs < 5 ) {
            snprintf( buffer, 99, "%s %05hu", STRINGS[ 340 ][ m_options.m_language ], m_id );
        } else {
            snprintf( buffer, 99, "%05hu", m_id );
        }
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->printStringC( buffer, 218, 35, p_bottom, IO::font::RIGHT );

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );

        // player name
        IO::regularFont->printString( STRINGS[ 406 ][ m_options.m_language ], 108, 52, p_bottom );
        IO::regularFont->printString( m_playername, 234, 52, p_bottom, IO::font::RIGHT );

        // play time
        if( !p_dummy ) {
            IO::regularFont->printString( STRINGS[ 407 ][ m_options.m_language ], 108, 68,
                                          p_bottom );
            snprintf( buffer, 99, "%hu:%02hhu", m_playTime.m_hours, m_playTime.m_mins );
            IO::regularFont->printString( buffer, 234, 68, p_bottom, IO::font::RIGHT );

            // Money
            IO::regularFont->printString( STRINGS[ 408 ][ m_options.m_language ], 108, 84,
                                          p_bottom );
            snprintf( buffer, 99, "$%lu", m_money );
            IO::regularFont->printString( buffer, 234, 84, p_bottom, IO::font::RIGHT );

            if( checkFlag( F_DEX_OBTAINED ) ) {
                // PokeDex
                IO::regularFont->printString( STRINGS[ 409 ][ m_options.m_language ], 108, 100,
                                              p_bottom );
                snprintf( buffer, 99, "%hhu/%hhu", getSeenCount( ), getCaughtCount( ) );
                IO::regularFont->printString( buffer, 234, 100, p_bottom, IO::font::RIGHT );
            }

            // Last Badge / Hall of Fame.
            if( m_lastAchievementEvent ) {
                snprintf( buffer, 99,
                          ACHIEVEMENTS[ m_lastAchievementEvent ][ m_options.m_language ],
                          IO::formatDate( m_lastAchievementDate, m_options.m_language ).c_str( ) );
                IO::regularFont->printStringC( buffer, 242, 116, p_bottom, IO::font::RIGHT );
            }

            // Last save
            if( m_lastSaveLocation ) {
                snprintf( buffer, 99, STRINGS[ 411 ][ m_options.m_language ],
                          FS::getLocation( m_lastSaveLocation, m_options.m_language ).c_str( ),
                          IO::formatDate( m_lastSaveDate, m_options.m_language ).c_str( ),
                          m_lastSaveTime.m_hours, m_lastSaveTime.m_mins );
                IO::regularFont->printBreakingStringC( buffer, 242, 132, 230, p_bottom,
                                                       IO::font::RIGHT );
            }

            // Adventure started
            snprintf( buffer, 99, STRINGS[ 410 ][ m_options.m_language ],
                      IO::formatDate( m_startDate, m_options.m_language ).c_str( ) );
            IO::regularFont->printStringC( buffer, 242, 164, p_bottom, IO::font::RIGHT );
        }
        IO::updateOAM( p_bottom );
        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );
    }

    void saveGame::playerInfo::clear( ) {
        m_good1 = 0;
        std::memset( m_playername, 0, sizeof( m_playername ) );
        m_id = m_appearance = m_sid = 0;
        m_startDate = m_lastSaveDate = m_lastAchievementDate = { 0, 0, 0 };
        m_lastSaveTime = m_playTime = { 0, 0, 0 };
        m_lastSaveLocation          = 0;
        m_HOENN_Badges = m_FRONTIER_Badges = m_KANTO_Badges = m_JOHTO_Badges = m_RESERVED_Badges
            = m_lastAchievementEvent                                         = 0;

        m_ashCount = m_money = m_coins = m_battlePoints = m_currentMap = m_stepCount = m_curBox
            = m_lstDex                                                               = 0;
        std::memset( m_lstViewedItem, 0, sizeof( m_lstViewedItem ) );

        m_mapObjectCount = 0;
        m_playerPriority = OBJPRIORITY_1;

        m_options.clear( );

        m_registeredItem = m_lstBag = m_lstUsedItem = m_repelSteps = 0;
        std::memset( m_pkmnTeam, 0, sizeof( m_pkmnTeam ) );
        std::memset( m_vars, 0, sizeof( m_vars ) );
        std::memset( m_flags, 0, sizeof( m_flags ) );

        std::memset( m_berryTrees, 0, sizeof( m_berryTrees ) );
        std::memset( m_berryHealth, 0, sizeof( m_berryHealth ) );
        std::memset( m_berryPlantedDate, 0, sizeof( m_berryPlantedDate ) );
        std::memset( m_berryPlantedTime, 0, sizeof( m_berryPlantedTime ) );

        m_initGameItemCount = 0;
        std::memset( m_initGameItems, 0, sizeof( m_initGameItems ) );
        std::memset( m_reserved, 0, sizeof( m_reserved ) );

        std::memset( &m_bag, 0, sizeof( BAG::bag ) );

        std::memset( m_storedPokemon, 0, sizeof( m_storedPokemon ) );

        std::memset( m_caughtPkmn, 0, sizeof( m_caughtPkmn ) );
        std::memset( m_seenPkmn, 0, sizeof( m_seenPkmn ) );
    }
    void saveGame::playerInfo::initialize( ) {
        clear( );

        m_good1 = GOOD_MAGIC1;
        m_good2 = GOOD_MAGIC2;

        m_startDate       = CURRENT_DATE;
        m_options.m_bgIdx = INITIAL_NAVBG;

        m_bag = BAG::bag( );
        for( u8 i = 0; i < MAX_BOXES; ++i ) {
            sprintf( ( m_storedPokemon + i )->m_name, "Box %d", i + 1 );
            ( m_storedPokemon + i )->m_wallpaper = i;
        }

        m_curBox = 0;
        m_money  = 3000;
        m_id     = rand( ) % 65536;
        m_sid    = rand( ) % 65536;

        // Default Berry trees

        // Route 102
        m_berryTrees[ 0 ]  = ITEM::itemToBerry( I_ORAN_BERRY );
        m_berryHealth[ 0 ] = 255;
        m_berryTrees[ 1 ]  = ITEM::itemToBerry( I_PECHA_BERRY );
        m_berryHealth[ 1 ] = 255;

        // Route 104 (south)
        m_berryTrees[ 2 ]  = ITEM::itemToBerry( I_ORAN_BERRY );
        m_berryHealth[ 2 ] = 255;
        m_berryTrees[ 3 ]  = 0;
        m_berryHealth[ 3 ] = 0;
        m_berryTrees[ 4 ]  = ITEM::itemToBerry( I_PECHA_BERRY );
        m_berryHealth[ 4 ] = 255;

        // Route 104 (north)
        m_berryTrees[ 5 ]  = ITEM::itemToBerry( I_ORAN_BERRY );
        m_berryHealth[ 5 ] = 255;
        m_berryTrees[ 6 ]  = 0;
        m_berryHealth[ 6 ] = 0;
        m_berryTrees[ 7 ]  = 0;
        m_berryHealth[ 7 ] = 0;
        m_berryTrees[ 8 ]  = ITEM::itemToBerry( I_CHERI_BERRY );
        m_berryHealth[ 8 ] = 255;

        m_berryTrees[ 9 ]   = ITEM::itemToBerry( I_CHERI_BERRY );
        m_berryHealth[ 9 ]  = 255;
        m_berryTrees[ 10 ]  = 0;
        m_berryHealth[ 10 ] = 0;
        m_berryTrees[ 11 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 11 ] = 255;

        // Route 116
        m_berryTrees[ 12 ]  = ITEM::itemToBerry( I_CHESTO_BERRY );
        m_berryHealth[ 12 ] = 255;
        m_berryTrees[ 16 ]  = 0;
        m_berryHealth[ 16 ] = 0;
        m_berryTrees[ 17 ]  = 0;
        m_berryHealth[ 17 ] = 0;
        m_berryTrees[ 13 ]  = ITEM::itemToBerry( I_PINAP_BERRY );
        m_berryHealth[ 13 ] = 255;

        // Route 115
        m_berryTrees[ 14 ]  = ITEM::itemToBerry( I_CHILAN_BERRY );
        m_berryHealth[ 14 ] = 255;
        m_berryTrees[ 15 ]  = ITEM::itemToBerry( I_ROSELI_BERRY );
        m_berryHealth[ 15 ] = 255;
    }

    bool saveGame::playerInfo::berryIsAlive( u8 p_berrySlot ) const {
        if( m_berryHealth[ p_berrySlot ] == 255 ) { return true; }

        // For each hour, decrease the health by 1.
        if( CURRENT_DATE < m_berryPlantedDate[ p_berrySlot ]
            || CURRENT_TIME < m_berryPlantedTime[ p_berrySlot ] ) {
            return false;
        }

        if( !m_berryHealth[ p_berrySlot ] || !m_berryTrees[ p_berrySlot ] ) { return false; }
        return true;
    }

    u8 saveGame::playerInfo::getBerryYield( u8 p_berrySlot ) const {
        if( m_berryHealth[ p_berrySlot ] == 255 ) { return 2; }

        // TODO: Double yield if leading party pkmn has harvest
        // TODO
        return 0;
    }

    u8 saveGame::playerInfo::getBerryStage( u8 p_berrySlot ) const {
        if( m_berryHealth[ p_berrySlot ] == 255 ) { return 4; }

        // TODO

        return 0;
    }

    void saveGame::playerInfo::plantBerry( u8 p_berrySlot, u16 p_berry ) {
        m_berryHealth[ p_berrySlot ]      = 5 * BERRY_GROWTH_TIME[ ITEM::itemToBerry( p_berry ) ];
        m_berryTrees[ p_berrySlot ]       = ITEM::itemToBerry( p_berry );
        m_berryPlantedDate[ p_berrySlot ] = CURRENT_DATE;
        m_berryPlantedTime[ p_berrySlot ] = CURRENT_TIME;
    }

    void saveGame::playerInfo::waterBerry( u8 p_berrySlot ) {
        if( m_berryHealth[ p_berrySlot ] < 200 ) { m_berryHealth[ p_berrySlot ]++; }
    }

    void saveGame::playerInfo::stepIncrease( ) {
        m_stepCount++;
        if( m_repelSteps > 0 ) {
            m_repelSteps--;
            if( !m_repelSteps ) { NAV::printMessage( GET_STRING( 4 ) ); }
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
        s16 mxlv = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( !m_pkmnTeam[ i ].m_boxdata.m_speciesId ) break;
            mxlv = std::max( mxlv, s16( m_pkmnTeam[ i ].m_level ) );
        }
        if( !mxlv || m_repelSteps ) return 0;
        mxlv = std::min( s16( 93 ), s16( mxlv + 6 ) );
        mxlv = std::min( s16( 5 * getBadgeCount( ) + 8 ), s16( mxlv + 0 ) );

        if( !checkFlag( F_DEX_OBTAINED ) ) { mxlv = 4; }

        mxlv += m_options.m_levelModifier / 2 + ( rand( ) % ( 2 * ( p_tier + 1 ) ) - p_tier - 1 );

        return u8( std::max( (s16) 2, std::min( (s16) 100, mxlv ) ) );
    }

    bool saveGame::playerInfo::checkFlag( u16 p_idx ) {
        return m_flags[ p_idx >> 4 ] & ( 1 << ( p_idx & 15 ) );
    }
    void saveGame::playerInfo::setFlag( u16 p_idx, bool p_value ) {
        if( p_value != checkFlag( p_idx ) ) m_flags[ p_idx >> 4 ] ^= ( 1 << ( p_idx & 15 ) );
        return;
    }

    u16 saveGame::playerInfo::getVar( u8 p_idx ) {
        return m_vars[ p_idx ];
    }
    void saveGame::playerInfo::setVar( u8 p_idx, u16 p_value ) {
        m_vars[ p_idx ] = p_value;
    }

    void saveGame::playerInfo::increaseVar( u8 p_idx ) {
        if( m_vars[ p_idx ] < u16( -1 ) ) { m_vars[ p_idx ]++; }
    }

    u8 saveGame::playerInfo::getBadgeCount( u8 p_region ) {
        u8 cnt = 0;
        if( p_region == 0 || p_region == 255 ) { cnt += std::popcount( m_HOENN_Badges ); }
        if( p_region == 1 || p_region == 255 ) { cnt += std::popcount( m_FRONTIER_Badges ); }
        if( p_region == 2 || p_region == 255 ) { cnt += std::popcount( m_KANTO_Badges ); }
        if( p_region == 3 || p_region == 255 ) { cnt += std::popcount( m_JOHTO_Badges ); }
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
            idx = m_storedPokemon[ ( ( ++i ) + m_curBox ) % MAX_BOXES ].getFirstFreeSpot( );
        if( idx == -1 ) // Everything's full :/
            return -1;
        m_curBox                           = ( m_curBox + i ) % MAX_BOXES;
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
            if( !m_pkmnTeam[ i ].isEgg( ) && m_pkmnTeam[ i ].m_boxdata.m_speciesId == p_pkmnIdx )
                ++res;
        }
        return res;
    }

    bool saveGame::playerInfo::setTeamPkmn( u8 p_position, pokemon* p_pokemon ) {
        if( p_pokemon != nullptr ) {
            m_pkmnTeam[ p_position ] = *p_pokemon;
        } else {
            if( countAlivePkmn( ) - m_pkmnTeam[ p_position ].canBattle( ) == 0 ) { return false; }
            memset( &m_pkmnTeam[ p_position ], 0, sizeof( pokemon ) );
        }
        return true;
    }

    bool saveGame::playerInfo::setTeamPkmn( u8 p_position, boxPokemon* p_pokemon ) {
        if( p_pokemon != nullptr ) {
            m_pkmnTeam[ p_position ] = pokemon( *p_pokemon );
        } else {
            if( countAlivePkmn( ) - m_pkmnTeam[ p_position ].canBattle( ) == 0 ) { return false; }
            memset( &m_pkmnTeam[ p_position ], 0, sizeof( pokemon ) );
        }
        return true;
    }

    u8 saveGame::playerInfo::consolidatePkmn( ) {
        u8 currentGap = 0;
        u8 res        = u8( -1 );
        for( u8 i = 0; i < 6; ++i ) {
            if( !m_pkmnTeam[ i ].getSpecies( ) ) {
                currentGap++;
            } else if( currentGap ) {
                if( res == u8( -1 ) ) { res = i - currentGap; }
                m_pkmnTeam[ i - currentGap ] = m_pkmnTeam[ i ];
                memset( &m_pkmnTeam[ i ], 0, sizeof( pokemon ) );
            }
        }
        return res;
    }

    bool saveGame::isGood( ) {
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) {
            if( SAV.m_saveFile[ i ].isGood( ) ) { return true; }
        }
        // return VERSION == m_version;
        return false;
    }
    void saveGame::clear( ) {
        for( u8 i = 0; i < MAX_SAVE_FILES; ++i ) { SAV.m_saveFile[ i ].clear( ); }
        m_version    = VERSION;
        m_activeFile = 0;
    }
} // namespace SAVE
