/*
    Pokémon neo
    ------------------------------

    file        : sound.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file (item.cpp) for details.

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

#include <nds/ndstypes.h>
#include "gen/bgmNames.h"
#include "gen/locationNames.h"
#include "gen/pokemonNames.h"
#include "gen/sfxNames.h"
#include "gen/trainerClassNames.h"
#include "map/mapDefines.h"

void initSound( );

namespace SOUND {
    constexpr s16 JBOX_DISABLED = s16( -1 );

    extern bool BGMforced;
    extern s16  currentBGM;

    /*
     * @brief: Initializes sound effects for battles (NOP).
     */
    void initBattleSound( );

    /*
     * @brief: Unloads battle SFX and loads general sfx (NOP).
     */
    void deinitBattleSound( );

    /*
     * @brief: Sets the specified bgm.
     * @param p_id: BGM name from bgmNames.h (BGM_<NAME>)
     */
    void playBGM( s16 p_id, bool p_force = false );

    /*
     * @brief: Plays the specified bgm once (non.looped)
     * @param p_id: BGM name from bgmNames.h (BGM_<NAME>)
     */
    void playBGMOneshot( s16 p_id );

    /*
     * @brief: Plays the specified sound effect..
     * @param p_id: SFX name from sfxNames.h (SFX_<NAME>)
     */
    void playSoundEffect( u16 p_id );

    void playCry( u16 p_pokemonId, u8 p_formeId = 0, bool p_female = false );

    void setVolume( u16 p_newValue );
    void dimVolume( );
    void restoreVolume( );

    s16 BGMforWeather( MAP::mapWeather p_weather );
    s16 BGMforMoveMode( MAP::moveMode p_moveMode );

    constexpr s16 BGMforTrainerEncounter( u16 p_trainerClassId ) {
#ifdef NO_SOUND
        (void) p_trainerClassId;
        return 0;
#else
        switch( p_trainerClassId ) {
        case TC_POKEMON_TRAINER: return BGM_ENCOUNTER_BRENDAN;
        case TC__POKEMON_TRAINER: return BGM_ENCOUNTER_MAY;
        case TC_AQUA_ADMIN:
        case TC_AQUA_LEADER:
        case TC_TEAM_AQUA: return BGM_ENCOUNTER_TEAM_M;
        case TC_MAGMA_ADMIN:
        case TC_MAGMA_LEADER:
        case TC_TEAM_MAGMA: return BGM_ENCOUNTER_TEAM_M;
        case TC_TWINS:
        case TC_SCHOOLBOY:
        case TC_SCHOOLGIRL:
        case TC_SCHOOLKID: return BGM_ENCOUNTER_TWINS;
        case TC_TUBER:
        case TC__TUBER:
        case TC_PRESCHOOLER:
        case TC__PRESCHOOLER:
        case TC_FAIRY_TALE_GIRL: return BGM_ENCOUNTER_TUBER;
        case TC_YOUNGSTER:
        case TC_CAMPER:
        case TC_PICNICKER:
        case TC_SIS_BRO:
        case TC_BUG_CATCHER:
        case TC_YOUNG_COUPLE: return BGM_ENCOUNTER_YOUNGSTER;
        case TC_LASS: return BGM_ENCOUNTER_LASS;
        case TC_HIKER:
        case TC_RUIN_MANIAC:
        case TC_TOURIST:
        case TC__TOURIST:
        case TC_BACKPACKER:
        case TC_WORKER: return BGM_ENCOUNTER_HIKER;
        case TC_DELINQUENT:
        case TC_GUITARIST:
        case TC_PUNK_COUPLE:
        case TC_PUNK_GIRL:
        case TC_PUNK_GUY: return BGM_ENCOUNTER_ELECTRIC;
        case TC_SAILOR:
        case TC_RANGERS:
        case TC_MONSIEUR:
        case TC_MADAME:
        case TC_GENTLEMAN:
        case TC_FISHERMAN:
        case TC_BUG_MANIAC:
        case TC_COLLECTOR: return BGM_ENCOUNTER_SAILOR;
        case TC_EXPERT:
        case TC__EXPERT: return BGM_ENCOUNTER_ACE_TRAINER;
        default: return BGM_ENCOUNTER_YOUNGSTER;
        }
#endif
    }

    constexpr s16 BGMforTrainerBattle( u16 p_trainerClassId ) {
#ifdef NO_SOUND
        (void) p_trainerClassId;
        return 0;
#else
        switch( p_trainerClassId ) {
        case TC_POKEMON_TRAINER:
        case TC__POKEMON_TRAINER: return BGM_BATTLE_MAY_BRENDAN;
        case TC_LEADER:
        case TC__LEADER:
        case TC_LEADERS: return BGM_BATTLE_GYM_LEADER;
        case TC_CHAMPION: return BGM_BATTLE_CHAMPION;
        case TC_FRONTIER_BRAIN: return BGM_BATTLE_FRONTIER_BRAIN;
        case TC_MAGMA_LEADER:
        case TC_AQUA_LEADER: return BGM_BATTLE_TEAM_AM_LEADER;
        case TC_AQUA_ADMIN:
        case TC_TEAM_AQUA:
        case TC_MAGMA_ADMIN:
        case TC_TEAM_MAGMA: return BGM_BATTLE_TEAM_AM;
        default: return BGM_BATTLE_TRAINER;
        }
#endif
    }

    constexpr s16 BGMforTrainerWin( u16 p_trainerClassId ) {
#ifdef NO_SOUND
        (void) p_trainerClassId;
        return 0;
#else
        switch( p_trainerClassId ) {
        case TC_LEADER:
        case TC__LEADER:
        case TC_LEADERS: return BGM_VICTORY_GYM_LEADER;
        case TC_MAGMA_LEADER:
        case TC_AQUA_LEADER:
        case TC_AQUA_ADMIN:
        case TC_TEAM_AQUA:
        case TC_MAGMA_ADMIN:
        case TC_TEAM_MAGMA: return BGM_VICTORY_TEAM_AM_GRUNT;
        case TC_FRONTIER_BRAIN: return BGM_VICTORY_FRONTIER_BRAIN;
        default: return BGM_VICTORY_TRAINER;
        }
#endif
    }

    constexpr s16 BGMforWildBattle( u16 p_pokemonId ) {
#ifdef NO_SOUND
        (void) p_pokemonId;
        return 0;
#else
        switch( p_pokemonId ) {
        case PKMN_RAIKOU: return BGM_BATTLE_RAIKOU;
        case PKMN_SUICUNE: return BGM_BATTLE_SUICUNE;
        case PKMN_ENTEI: return BGM_BATTLE_ENTEI;
        case PKMN_LUGIA: return BGM_BATTLE_LUGIA;
        case PKMN_HO_OH: return BGM_BATTLE_HOUOU;
        case PKMN_REGIROCK:
        case PKMN_REGICE:
        case PKMN_REGISTEEL:
        case PKMN_REGIGIGAS:
        case PKMN_REGIELEKI:
        case PKMN_REGIDRAGO: return BGM_BATTLE_REGI;
        case PKMN_GROUDON:
        case PKMN_KYOGRE:
        case PKMN_RAYQUAZA: return BGM_BATTLE_RAYQUAZA;
        case PKMN_AZELF:
        case PKMN_MESPRIT:
        case PKMN_UXIE: return BGM_BATTLE_LAKE;
        case PKMN_HEATRAN:
        case PKMN_CRESSELIA:
        case PKMN_DARKRAI:
        case PKMN_SHAYMIN: return BGM_BATTLE_HEATRAN;
        case PKMN_DIALGA:
        case PKMN_PALKIA: return BGM_BATTLE_DIALGA_PALKIA;
        case PKMN_GIRATINA: return BGM_BATTLE_GIRATINA;
        default:
            if( p_pokemonId > PKMN_DEOXYS && p_pokemonId <= PKMN_ARCEUS ) {
                return BGM_BATTLE_EX01;
            }
            return BGM_BATTLE_WILD;
        }
#endif
    }

    void setJBoxBGM( s16 p_id );
    s16  getJBoxBGM( );

    /*
     * @brief: Starts/Stops the special poketore music
     */
    void setTracerStatus( bool p_active, bool p_silent = false );

    void onWeatherChange( MAP::mapWeather p_newWeather );
    void onLocationChange( u16 p_newLocatonId );
    void onMovementTypeChange( MAP::moveMode p_newMoveMode );
    void restartBGM( );
    void stopBGM( );
} // namespace SOUND
