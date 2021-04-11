/*
    Pokémon neo
    ------------------------------

    file        : saveGame.cpp
    author      : Philip Wellnitz
    description : Functionality for parsing game save files

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
#include <nds.h>

#include "abilityNames.h"
#include "battleTrainer.h"
#include "dex.h"
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
            IO::boldFont->printString( GET_STRING_L( 412, m_options.m_language ), 128, 90, p_bottom,
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
        IO::boldFont->printString( GET_STRING_L( 405, m_options.m_language ), 112, 13, p_bottom,
                                   IO::font::CENTER );

        if( achvs < 5 ) {
            snprintf( buffer, 99, "%s %05hu", GET_STRING_L( 340, m_options.m_language ), m_id );
        } else {
            snprintf( buffer, 99, "%05hu", m_id );
        }
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->printStringC( buffer, 218, 35, p_bottom, IO::font::RIGHT );

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );

        // player name
        IO::regularFont->printString( GET_STRING_L( 406, m_options.m_language ), 108, 52,
                                      p_bottom );
        IO::regularFont->printString( m_playername, 234, 52, p_bottom, IO::font::RIGHT );

        // play time
        if( !p_dummy ) {
            IO::regularFont->printString( GET_STRING_L( 407, m_options.m_language ), 108, 68,
                                          p_bottom );
            snprintf( buffer, 99, "%hu:%02hhu", m_playTime.m_hours, m_playTime.m_mins );
            IO::regularFont->printString( buffer, 234, 68, p_bottom, IO::font::RIGHT );

            // Money
            IO::regularFont->printString( GET_STRING_L( 408, m_options.m_language ), 108, 84,
                                          p_bottom );
            snprintf( buffer, 99, "$%lu", m_money );
            IO::regularFont->printString( buffer, 234, 84, p_bottom, IO::font::RIGHT );

            if( checkFlag( F_DEX_OBTAINED ) ) {
                // PokeDex
                IO::regularFont->printString( GET_STRING_L( 409, m_options.m_language ), 108, 100,
                                              p_bottom );
                snprintf( buffer, 99, "%hu/%hu", getSeenCount( ), getCaughtCount( ) );
                IO::regularFont->printString( buffer, 234, 100, p_bottom, IO::font::RIGHT );
            }

            // Last Badge / Hall of Fame.
            if( m_lastAchievementEvent ) {
                snprintf( buffer, 99,
                          getAchievement( m_lastAchievementEvent, m_options.m_language ),
                          IO::formatDate( m_lastAchievementDate, m_options.m_language ).c_str( ) );
                IO::regularFont->printStringC( buffer, 242, 116, p_bottom, IO::font::RIGHT );
            }

            // Last save
            if( m_lastSaveLocation ) {
                snprintf( buffer, 99, GET_STRING_L( 411, m_options.m_language ),
                          FS::getLocation( m_lastSaveLocation, m_options.m_language ).c_str( ),
                          IO::formatDate( m_lastSaveDate, m_options.m_language ).c_str( ),
                          m_lastSaveTime.m_hours, m_lastSaveTime.m_mins );
                IO::regularFont->printBreakingStringC( buffer, 242, 132, 230, p_bottom,
                                                       IO::font::RIGHT );
            }

            // Adventure started
            snprintf( buffer, 99, GET_STRING_L( 410, m_options.m_language ),
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

        m_achievements = 0;

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

        // Route 115 (north)
        m_berryTrees[ 27 ]  = ITEM::itemToBerry( I_MAGO_BERRY );
        m_berryHealth[ 27 ] = 255;
        m_berryTrees[ 28 ]  = ITEM::itemToBerry( I_WIKI_BERRY );
        m_berryHealth[ 28 ] = 255;
        m_berryTrees[ 29 ]  = ITEM::itemToBerry( I_IAPAPA_BERRY );
        m_berryHealth[ 29 ] = 255;

        // R 115 (east)
        m_berryTrees[ 30 ]  = ITEM::itemToBerry( I_AGUAV_BERRY );
        m_berryHealth[ 30 ] = 255;
        m_berryTrees[ 31 ]  = ITEM::itemToBerry( I_FIGY_BERRY );
        m_berryHealth[ 31 ] = 255;

        // R 114
        m_berryTrees[ 32 ]  = ITEM::itemToBerry( I_RINDO_BERRY );
        m_berryHealth[ 32 ] = 255;
        m_berryTrees[ 33 ]  = ITEM::itemToBerry( I_CHOPLE_BERRY );
        m_berryHealth[ 33 ] = 255;
        m_berryTrees[ 34 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 34 ] = 255;

        // Route 117
        m_berryTrees[ 18 ]  = ITEM::itemToBerry( I_WEPEAR_BERRY );
        m_berryHealth[ 18 ] = 255;
        m_berryTrees[ 19 ]  = ITEM::itemToBerry( I_PINAP_BERRY );
        m_berryHealth[ 19 ] = 255;
        m_berryTrees[ 20 ]  = ITEM::itemToBerry( I_NANAB_BERRY );
        m_berryHealth[ 20 ] = 255;

        // Route 110 (north)
        m_berryTrees[ 21 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 21 ] = 255;
        m_berryTrees[ 22 ]  = ITEM::itemToBerry( I_NANAB_BERRY );
        m_berryHealth[ 22 ] = 255;
        m_berryTrees[ 23 ]  = ITEM::itemToBerry( I_WACAN_BERRY );
        m_berryHealth[ 23 ] = 255;

        // Route 103 (east)
        m_berryTrees[ 24 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 24 ] = 255;
        m_berryTrees[ 25 ]  = 0;
        m_berryHealth[ 25 ] = 0;
        m_berryTrees[ 26 ]  = ITEM::itemToBerry( I_CHERI_BERRY );
        m_berryHealth[ 26 ] = 255;

        // R 111
        m_berryTrees[ 35 ]  = ITEM::itemToBerry( I_RINDO_BERRY );
        m_berryHealth[ 35 ] = 255;
        m_berryTrees[ 36 ]  = ITEM::itemToBerry( I_CHOPLE_BERRY );
        m_berryHealth[ 36 ] = 255;
        m_berryTrees[ 37 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 37 ] = 255;
        m_berryTrees[ 38 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 38 ] = 255;

        // R 112
        m_berryTrees[ 39 ]  = ITEM::itemToBerry( I_COBA_BERRY );
        m_berryHealth[ 39 ] = 255;
        m_berryTrees[ 40 ]  = ITEM::itemToBerry( I_SHUCA_BERRY );
        m_berryHealth[ 40 ] = 255;
        m_berryTrees[ 41 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 41 ] = 255;
        m_berryTrees[ 42 ]  = ITEM::itemToBerry( I_HABAN_BERRY );
        m_berryHealth[ 42 ] = 255;

        // R 118
        m_berryTrees[ 43 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 43 ] = 255;
        m_berryTrees[ 44 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 44 ] = 255;
        m_berryTrees[ 45 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 45 ] = 255;

        // R 123
        m_berryTrees[ 46 ]  = ITEM::itemToBerry( I_TAMATO_BERRY );
        m_berryHealth[ 46 ] = 255;
        m_berryTrees[ 47 ]  = ITEM::itemToBerry( I_BELUE_BERRY );
        m_berryHealth[ 47 ] = 255;

        m_berryTrees[ 48 ]  = ITEM::itemToBerry( I_GREPA_BERRY );
        m_berryHealth[ 48 ] = 255;
        m_berryTrees[ 49 ]  = ITEM::itemToBerry( I_HONDEW_BERRY );
        m_berryHealth[ 49 ] = 255;

        m_berryTrees[ 50 ]  = ITEM::itemToBerry( I_CORNN_BERRY );
        m_berryHealth[ 50 ] = 255;
        m_berryTrees[ 51 ]  = ITEM::itemToBerry( I_MAGOST_BERRY );
        m_berryHealth[ 51 ] = 255;

        m_berryTrees[ 52 ]  = ITEM::itemToBerry( I_RABUTA_BERRY );
        m_berryHealth[ 52 ] = 255;
        m_berryTrees[ 53 ]  = ITEM::itemToBerry( I_NOMEL_BERRY );
        m_berryHealth[ 53 ] = 255;

        m_berryTrees[ 54 ]  = ITEM::itemToBerry( I_SPELON_BERRY );
        m_berryHealth[ 54 ] = 255;
        m_berryTrees[ 55 ]  = ITEM::itemToBerry( I_PAMTRE_BERRY );
        m_berryHealth[ 55 ] = 255;

        m_berryTrees[ 56 ]  = ITEM::itemToBerry( I_WATMEL_BERRY );
        m_berryHealth[ 56 ] = 255;
        m_berryTrees[ 57 ]  = ITEM::itemToBerry( I_DURIN_BERRY );
        m_berryHealth[ 57 ] = 255;

        // r 123 (north east)

        m_berryTrees[ 58 ]  = ITEM::itemToBerry( I_KEBIA_BERRY );
        m_berryHealth[ 58 ] = 255;
        m_berryTrees[ 59 ]  = ITEM::itemToBerry( I_SHUCA_BERRY );
        m_berryHealth[ 59 ] = 255;
        m_berryTrees[ 60 ]  = ITEM::itemToBerry( I_TANGA_BERRY );
        m_berryHealth[ 60 ] = 255;

        // r 123 (south east)

        m_berryTrees[ 61 ]  = ITEM::itemToBerry( I_HABAN_BERRY );
        m_berryHealth[ 61 ] = 255;
        m_berryTrees[ 62 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 62 ] = 255;
        m_berryTrees[ 63 ]  = ITEM::itemToBerry( I_BABIRI_BERRY );
        m_berryHealth[ 63 ] = 255;

        // r 119 (south)

        m_berryTrees[ 64 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 64 ] = 255;
        m_berryTrees[ 65 ]  = ITEM::itemToBerry( I_COLBUR_BERRY );
        m_berryHealth[ 65 ] = 255;

        // r 119 (north west)

        m_berryTrees[ 66 ]  = ITEM::itemToBerry( I_CHARTI_BERRY );
        m_berryHealth[ 66 ] = 255;
        m_berryTrees[ 67 ]  = ITEM::itemToBerry( I_HONDEW_BERRY );
        m_berryHealth[ 67 ] = 255;

        // r 119 (north east)

        m_berryTrees[ 67 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 67 ] = 255;
        m_berryTrees[ 68 ]  = ITEM::itemToBerry( I_COBA_BERRY );
        m_berryHealth[ 68 ] = 255;
        m_berryTrees[ 69 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 69 ] = 255;

        // r 120 (north east)

        m_berryTrees[ 70 ]  = ITEM::itemToBerry( I_KELPSY_BERRY );
        m_berryHealth[ 70 ] = 255;
        m_berryTrees[ 71 ]  = ITEM::itemToBerry( I_QUALOT_BERRY );
        m_berryHealth[ 71 ] = 255;
        m_berryTrees[ 72 ]  = ITEM::itemToBerry( I_BLUK_BERRY );
        m_berryHealth[ 72 ] = 255;

        // r 120 s

        m_berryTrees[ 73 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 73 ] = 255;
        m_berryTrees[ 74 ]  = ITEM::itemToBerry( I_LEPPA_BERRY );
        m_berryHealth[ 74 ] = 255;
        m_berryTrees[ 75 ]  = ITEM::itemToBerry( I_LUM_BERRY );
        m_berryHealth[ 75 ] = 255;
        m_berryTrees[ 76 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 76 ] = 255;

        m_berryTrees[ 77 ]  = ITEM::itemToBerry( I_OCCA_BERRY );
        m_berryHealth[ 77 ] = 255;
        m_berryTrees[ 78 ]  = ITEM::itemToBerry( I_RINDO_BERRY );
        m_berryHealth[ 78 ] = 255;
        m_berryTrees[ 79 ]  = ITEM::itemToBerry( I_WACAN_BERRY );
        m_berryHealth[ 79 ] = 255;

        // r 121

        m_berryTrees[ 80 ]  = ITEM::itemToBerry( I_ORAN_BERRY );
        m_berryHealth[ 80 ] = 255;
        m_berryTrees[ 81 ]  = ITEM::itemToBerry( I_LUM_BERRY );
        m_berryHealth[ 81 ] = 255;
        m_berryTrees[ 82 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 82 ] = 255;
        m_berryTrees[ 83 ]  = ITEM::itemToBerry( I_LUM_BERRY );
        m_berryHealth[ 83 ] = 255;

        m_berryTrees[ 84 ]  = ITEM::itemToBerry( I_ORAN_BERRY );
        m_berryHealth[ 84 ] = 255;
        m_berryTrees[ 85 ]  = ITEM::itemToBerry( I_LUM_BERRY );
        m_berryHealth[ 85 ] = 255;
        m_berryTrees[ 86 ]  = ITEM::itemToBerry( I_SITRUS_BERRY );
        m_berryHealth[ 86 ] = 255;
        m_berryTrees[ 87 ]  = ITEM::itemToBerry( I_LUM_BERRY );
        m_berryHealth[ 87 ] = 255;
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

        // add exp to day care pkmn
        for( u8 i = 0; i < 6; ++i ) {
            if( m_dayCarePkmn[ i ].getSpecies( ) ) { m_dayCarePkmn[ i ].gainExperience( 1 ); }
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

            // check for eggs
            for( u8 i = 0; i < 3; ++i ) {
                if( m_dayCareEgg[ i ].getSpecies( ) ) {
                    // egg already exists
                    continue;
                }
                if( !m_dayCarePkmn[ 2 * i ].getSpecies( )
                    || !m_dayCarePkmn[ 2 * i + 1 ].getSpecies( ) ) { // not enough pkmn to breed
                    continue;
                }
                // check compatibility
                u32 comp = m_dayCarePkmn[ 2 * i ].getCompatibility( m_dayCarePkmn[ 2 * i + 1 ] );
                comp *= 20;

                // check for egg charm
                if( m_bag.count( BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_OVAL_CHARM ) ) {
                    comp = ( comp / 2 ) * 3;
                }

                if( u32( rand( ) % 100 ) < comp ) {
                    // create egg
                    m_dayCarePkmn[ 2 * i ].breed( m_dayCarePkmn[ 2 * i + 1 ], m_dayCareEgg[ i ] );
                    setFlag( F_HOENN_DAYCARE_EGG + i, 1 );
                }
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

    bool saveGame::playerInfo::checkFlag( u16 p_idx ) const {
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

    u16 saveGame::playerInfo::getLocalSeenCount( ) const {
        u16 res = 0;

        for( u16 i = 0; i < DEX::LOCAL_DEX_SIZE; ++i ) {
            if( seen( DEX::LOCAL_DEX[ i ] ) ) { ++res; }
        }

        return std::max( res, getLocalCaughtCount( ) );
    }

    u16 saveGame::playerInfo::getLocalCaughtCount( ) const {
        u16 res = 0;

        for( u16 i = 0; i < DEX::LOCAL_DEX_SIZE; ++i ) {
            if( caught( DEX::LOCAL_DEX[ i ] ) ) { ++res; }
        }

        return res;
    }

    bool saveGame::playerInfo::dexCompleted( ) const {
        for( u16 i = 1; i <= MAX_PKMN; ++i ) {
            if( DEX::requiredForCompletion( i ) && !caught( i ) ) { return false; }
        }
        return true;
    }

    bool saveGame::playerInfo::dexSeenCompleted( ) const {
        for( u16 i = 1; i <= MAX_PKMN; ++i ) {
            if( DEX::requiredForCompletion( i ) && !seen( i ) ) { return false; }
        }
        return true;
    }

    bool saveGame::playerInfo::localDexCompleted( ) const {
        for( u16 i = 0; i < DEX::LOCAL_DEX_SIZE; ++i ) {
            if( DEX::requiredForCompletion( DEX::LOCAL_DEX[ i ], true )
                && !caught( DEX::LOCAL_DEX[ i ] ) ) {
                return false;
            }
        }
        return true;
    }

    bool saveGame::playerInfo::localDexSeenCompleted( ) const {
        for( u16 i = 0; i < DEX::LOCAL_DEX_SIZE; ++i ) {
            if( DEX::requiredForCompletion( DEX::LOCAL_DEX[ i ], true )
                && !seen( DEX::LOCAL_DEX[ i ] ) ) {
                return false;
            }
        }
        return true;
    }

    u16 saveGame::playerInfo::getPkmnDisplayDexId( u16 p_pokemon ) const {
        if( checkFlag( F_NAT_DEX_OBTAINED ) ) { return p_pokemon; }
        return DEX::getDexNo( p_pokemon );
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
