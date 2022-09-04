/*
Pokémon neo
------------------------------

file        : fs.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.


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

#pragma once

#include <string>

#include "bag/item.h"
#include "battle/battleDefines.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "io/font.h"
#include "io/sprite.h"
#include "map/mapBattleFacilityDefines.h"
#include "map/mapSlice.h"
#include "pokemon.h"
#include "pokemonData.h"
#include "save/saveGame.h"

#ifndef NO_SOUND
#include "sound/sseq.h"
#endif

namespace FS {
    // map data
    static constexpr u16 DIVE_MAP = 1000;

    bool readPal( FILE* p_file, MAP::palette* p_palette, u8 p_count = 6 );
    bool readTiles( FILE* p_file, MAP::tile* p_tiles, u16 p_startIdx = 0, u16 p_size = 512 );
    bool readBlocks( FILE* p_file, MAP::block* p_blocks, u16 p_startIdx = 0, u16 p_size = 512 );

    bool seekTileSet( FILE* p_file, u8 p_tsIdx );

    FILE* openTileSet( );
    FILE* openBank( u16 p_bank, bool p_underwater );

    bool readMapData( FILE* p_file, MAP::mapData* p_result, bool p_close = true );

    bool readMapSlice( FILE* p_mapFile, MAP::mapSlice* p_result, u16 p_x = 0, u16 p_y = 0,
                       bool p_close = true );

    u32 readMapBankInfo( FILE* p_mapFile, MAP::bankInfo* p_info );

    u32 readMapSliceAndData( FILE* p_mapFile, MAP::mapSlice* p_slice, MAP::mapData* p_data, u16 p_x,
                             u16 p_y );

    FILE* openScript( u16 p_scriptId );

    u8* readCry( u16 p_pkmnIdx, u8 p_forme, u32& p_len );
    u8* readSFX( u16 p_sfxID, u16& p_len );

    u16 frameForLocation( u16 p_locationId );
    u16 mugForLocation( u16 p_locationId );
    u16 BGMforLocation( u16 p_locationId );

    /*
     * @brief: Loads location data into MAP_LOCATIONS for the specified bank; returns true
     * on success.
     */
    bool loadLocationData( u8 p_bank );

    // sprite / picture data

    // bool readNavScreenData( u16* p_layer, const char* p_name, u8 p_no );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name,
                          u16 p_paletteSize = 512, u32 p_tileCnt = 192 * 256,
                          bool p_bottom = false );
    bool readPictureData( u16* p_layer, const char* p_Path, const char* p_name, u16 p_paletteSize,
                          u16 p_palStart, u32 p_tileCnt, bool p_bottom );
    bool readSpriteData( IO::SpriteInfo* p_spriteInfo, const char* p_path, const char* p_name,
                         const u32 p_tileCnt, const u16 p_palCnt, bool p_bottom = false );

    // strings

    extern const char* EPISODE_NAMES[ SAVE::MAX_SPECIAL_EPISODES + 1 ][ MAX_LANGUAGES ];

#ifdef DESQUID
    constexpr u16 DESQUID_STRING = ( 1 << 13 );
#endif

    constexpr u16 MAP_STRING = ( 1 << 11 );

    extern const char*       LANGUAGE_NAMES[ MAX_LANGUAGES ];
    extern const char*       HP_ICONS[ MAX_LANGUAGES ];
    extern const char* const MONTHS[ 12 ][ MAX_LANGUAGES ];
#define CURRENT_LANGUAGE SAVE::SAV.getActiveFile( ).m_options.m_language

#define getBadgeName( p_type, p_badge )                                                         \
    ( ( ( p_type ) == 0 )                                                                       \
          ? FS::getBadge( (p_badge) -1 )                                                        \
          : ( ( p_type ) == 1                                                                   \
                  ? FS::getBadge( 8 + ( ( p_badge ) / 10 - 1 ) * 2 + ( ( p_badge ) % 10 ) - 1 ) \
                  : nullptr ) )

#define GET_MAP_STRING( p_stringId )           FS::getMapString( p_stringId )
#define GET_STRING_L( p_stringId, p_language ) FS::getUIString( p_stringId, p_language )

#ifdef DESQUID
    constexpr u16            MAX_DESQUID_STRINGS = 100;
    extern const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ MAX_LANGUAGES ];
#define GET_STRING( p_stringId )                                                               \
    ( ( ( p_stringId ) >= FS::DESQUID_STRING )                                                 \
          ? FS::DESQUID_STRINGS[ p_stringId - FS::DESQUID_STRING ][ 0 ]                        \
          : ( ( p_stringId ) >= FS::MAP_STRING ? GET_MAP_STRING( p_stringId - FS::MAP_STRING ) \
                                               : GET_STRING_L( p_stringId, CURRENT_LANGUAGE ) ) )
#else
#define GET_STRING( p_stringId )                                                       \
    ( ( p_stringId ) >= FS::MAP_STRING ? GET_MAP_STRING( p_stringId - FS::MAP_STRING ) \
                                       : GET_STRING_L( p_stringId, CURRENT_LANGUAGE ) )
#endif
#define HP_ICON FS::HP_ICONS[ CURRENT_LANGUAGE ]

    bool getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language, char* p_out );
    std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId, u8 p_language );
    std::string getString( const char* p_path, u16 p_maxLen, u16 p_stringId );

    const char* getUIString( u16 p_stringId, u8 p_language );
    const char* getMapString( u16 p_stringId );
    const char* getPkmnPhrase( u16 p_stringId );
    const char* getBadge( u16 p_badgeId );
    const char* getAchievement( u16 p_badgeId, u8 p_language );

    std::string getBGMName( u16 p_locationId );
    std::string getBGMName( u16 p_locationId, u8 p_language );
    bool        getBGMName( u16 p_locationId, u8 p_language, char* p_out );

    std::string getLocation( u16 p_locationId );
    std::string getLocation( u16 p_locationId, u8 p_language );
    bool        getLocation( u16 p_locationId, u8 p_language, char* p_out );

    bool        getAbilityName( u16 p_abilityId, u8 p_language, char* p_out );
    std::string getAbilityName( u16 p_abilityId, u8 p_language );
    std::string getAbilityName( u16 p_abilityId );

    bool        getAbilityDescr( u16 p_abilityId, u8 p_language, char* p_out );
    std::string getAbilityDescr( u16 p_abilityId, u8 p_language );
    std::string getAbilityDescr( u16 p_abilityId );

    std::string getTrainerClassName( u16 p_trainerClass );
    std::string getTrainerClassName( u16 p_trainerClass, u8 p_language );
    bool        getTrainerClassName( u16 p_trainerClass, u8 p_language, char* p_out );

    BATTLE::battleTrainer getBattleTrainer( u16 p_battleTrainerId );
    BATTLE::battleTrainer getBattleTrainer( u16 p_battleTrainerId, u8 p_language );
    bool getBattleTrainer( u16 p_battleTrainerId, u8 p_language, BATTLE::battleTrainer* p_out );

    bool getBattleFacilityTrainer( u16 p_battleTrainerId, u8 p_language,
                                   BATTLE::battleTrainer* p_out );
    bool getBattleFacilityTrainerTeam( u8 p_rulesId, u16 p_battleTrainerId,
                                       BATTLE::battleTrainer* p_out );

    bool        getMoveName( const u16 p_moveId, const u8 p_language, char* p_out );
    std::string getMoveName( const u16 p_moveId, const u8 p_language );
    std::string getMoveName( const u16 p_moveId );

    bool        getMoveDescr( const u16 p_moveId, const u8 p_language, char* p_out );
    std::string getMoveDescr( const u16 p_moveId, const u8 p_language );
    std::string getMoveDescr( const u16 p_moveId );

    bool             getMoveData( const u16 p_moveId, BATTLE::moveData* p_out );
    BATTLE::moveData getMoveData( const u16 p_moveId );

    std::string getSpeciesName( u16 p_pkmnId, u8 p_language, u8 p_forme );
    std::string getSpeciesName( u16 p_pkmnId, u8 p_forme = 0 );
    bool        getSpeciesName( u16 p_pkmnId, char* p_name, u8 p_language, u8 p_forme = 0 );

    std::string getDexEntry( u16 p_pkmnId, u8 p_language, u8 p_forme );
    std::string getDexEntry( u16 p_pkmnId, u8 p_forme = 0 );
    bool        getDexEntry( u16 p_pkmnId, char* p_name, u8 p_language, u8 p_forme = 0 );

    std::string getDisplayName( u16 p_pkmnId, u8 p_language, u8 p_forme );
    std::string getDisplayName( u16 p_pkmnId, u8 p_forme = 0 );
    bool        getDisplayName( u16 p_pkmnId, char* p_name, u8 p_language, u8 p_forme = 0 );

    pkmnData getPkmnData( const u16 p_pkmnId, const u8 p_forme = 0 );
    bool     getPkmnData( const u16 p_pkmnId, pkmnData* p_out );
    bool     getPkmnData( const u16 p_pkmnId, const u8 p_forme, pkmnData* p_out );

    pkmnEvolveData getPkmnEvolveData( const u16 p_pkmnId, const u8 p_forme = 0 );
    bool           getPkmnEvolveData( const u16 p_pkmnId, pkmnEvolveData* p_out );
    bool           getPkmnEvolveData( const u16 p_pkmnId, const u8 p_forme, pkmnEvolveData* p_out );

    const u16 LEARN_TM    = 200;
    const u16 LEARN_TUTOR = 201;
    const u16 LEARN_EGG   = 202;

    void getLearnMoves( u16 p_pkmnId, u8 p_forme, u16 p_fromLevel, u16 p_toLevel, u16 p_num,
                        u16* p_res );
    bool canLearn( u16 p_pkmnId, u8 p_forme, u16 p_moveId, u16 p_maxLevel, u16 p_minLevel = 0 );
    bool canLearn( const u16* p_learnset, u16 p_moveId, u16 p_maxLevel, u16 p_minLevel = 0 );
    const u16* getLearnset( u16 p_pkmnId, u8 p_forme );

    bool        getItemName( const u16 p_itemId, const u8 p_language, char* p_out );
    std::string getItemName( const u16 p_itemId, const u8 p_language );
    std::string getItemName( const u16 p_itemId );

    bool        getItemDescr( const u16 p_itemId, const u8 p_language, char* p_out );
    std::string getItemDescr( const u16 p_itemId, const u8 p_language );
    std::string getItemDescr( const u16 p_itemId );

    BAG::itemData getItemData( const u16 p_itemId );
    bool          getItemData( const u16 p_itemId, BAG::itemData* p_out );

    const char* getRibbonName( const u16 p_ribbonId );

    const char* getRibbonDescr( const u16 p_ribbonId );

    /*
     * @brief: loads the specified variant of the specified pkmn.
     * @param p_streak: Determines IVs given to the pkmn.
     */
    bool loadBattleFacilityPkmn( u16 p_species, u8 p_variant, u8 p_level, u16 p_streak,
                                 trainerPokemon* p_out );

    bool loadBattleFacilityTrainerTeam( const MAP::ruleSet& p_rules, u8 p_trainerClass, u8 p_team,
                                        u16 p_streak, BATTLE::battleTrainer* p_out );

    bool loadBattleFacilityTrainerStrings( u8 p_trainerClass, u8 p_trainer,
                                           BATTLE::battleTrainer* p_out );

#ifndef NO_SOUND
    bool loadSequenceData( SOUND::SSEQ::sequenceData* p_data, FILE* p_f );
    bool loadSoundSequence( SOUND::SSEQ::sequenceData* p_data, u16 p_sseqId );
    bool loadSoundBank( SOUND::SSEQ::sequenceData* p_data, u16 p_sbnkId );
    bool loadSoundSample( SOUND::SSEQ::sequenceData* p_data, u16 p_swarId );
#endif

} // namespace FS
