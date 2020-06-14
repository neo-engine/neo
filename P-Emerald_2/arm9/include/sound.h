/*
    Pokémon neo
    ------------------------------

    file        : sound.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file (item.cpp) for details.

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

#pragma once

#include <nds/ndstypes.h>
#include "mapDefines.h"
#include "soundbank.h"
#include "locationNames.h"
#include "pokemonNames.h"

void initSound( );

namespace SOUND {
    /*
     * @brief: Initializes sound effects for battles.
     */
    void initBattleSound( );

    /*
     * @brief: Unloads battle SFX and loads general sfx.
     */
    void deinitBattleSound( );

    void playBGM( u16 p_id );
    void playSoundEffect( u16 p_id );

    void playCry( u16 p_pokemonId, u8 p_formeId = 0, bool p_female = false );

    void setVolume( u16 p_newValue );
    void dimVolume( );
    void restoreVolume( );

    u16 BGMforMoveMode( MAP::moveMode p_moveMode );
    constexpr u16 BGMforTrainerEncounter( u16 p_trainerClassId ) {
        switch( p_trainerClassId ) {
            default:
                return MOD_BATTLE_WILD_ALT;
        }
    }
    constexpr u16 BGMforTrainerBattle( u16 p_trainerClassId ) {
        switch( p_trainerClassId ) {
            default:
                return MOD_BATTLE_WILD_ALT;
        }
    }
    constexpr u16 BGMforWildBattle( u16 p_pokemonId ) {
        switch( p_pokemonId ) {
            case PKMN_RAIKOU:
            case PKMN_SUICUNE:
            case PKMN_ENTEI:
                return MOD_BATTLE_RAIKOU_ENTEI_SUICUNE;
            case PKMN_RAYQUAZA:
                return MOD_BATTLE_RAYQUAZA;
            default:
                return MOD_BATTLE_WILD;
        }
    }
    constexpr u16 BGMforLocation( u16 p_locationId ) {
        switch( p_locationId ) {
            case L_ROUTE_38:
                return MOD_ROUTE_38;
            case L_ROUTE_101:
            case L_ROUTE_102:
            case L_ROUTE_103:
                return MOD_ROUTE_101;
            case L_ROUTE_104:
            case L_ROUTE_114:
            case L_ROUTE_115:
            case L_ROUTE_116:
                return MOD_ROUTE_104;
            case L_ROUTE_105:
            case L_ROUTE_106:
            case L_ROUTE_107:
                //            return MOD_ROUTE_104;
            case L_ROUTE_110:
            case L_ROUTE_111:
            case L_ROUTE_112:
                //            return MOD_ROUTE_111;
            case L_ROUTE_113:
                return MOD_ROUTE_113;
            case L_ROUTE_118:
            case L_ROUTE_119:
                //            return MOD_ROUTE_118;
            case L_ROUTE_120:
            case L_ROUTE_121:
                //            return MOD_ROUTE_120;
            case L_ROUTE_123:
                return MOD_ROUTE_123;
            case L_CLIFFELTA_CITY:
                return MOD_CLIFFELTA_CITY;
            case L_FALLARBOR_TOWN:
                return MOD_FALLARBOR_TOWN;
            case L_METEOR_FALLS:
                return MOD_METEOR_FALLS;
            case L_SEALED_CHAMBER:
            case L_ANCIENT_TOMB:
            case L_ISLAND_CAVE:
            case L_DESERT_RUINS:
                return MOD_SEALED_CHAMBER;
            case L_SHOAL_CAVE:
                return MOD_SHOAL_CAVE;
            case L_ROUTE_1112:
                return MOD_DESERT;
            case L_LILYCOVE_MUSEUM:
                return MOD_LILYCOVE_MUSEUM;
            case L_GRANITE_CAVE:
            case L_PETALBURG_WOODS:
            case L_JAGGED_PASS:
                return MOD_CAVE_FORESTS;
            default:
                return MOD_ABANDONED_SHIP;
        }
    }

    void onLocationChange( u16 p_newLocatonId );
    void onMovementTypeChange( MAP::moveMode p_newMoveMode );
    void restartBGM( );
    void stopBGM( );
} // namespace SOUND
