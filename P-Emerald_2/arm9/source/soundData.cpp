/*
Pokémon neo
------------------------------

file        : sound.cpp
author      : Philip Wellnitz
description :

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

#include "sound.h"
#include "locationNames.h"
#include "pokemonNames.h"

namespace SOUND {
    u16 currentLocation = 0;
    MAP::moveMode currentMoveMode = MAP::WALK;

    void onLocationChange( u16 p_newLocation ) {
        if( currentLocation == p_newLocation ) { return; }

        currentLocation = p_newLocation;
        if( currentMoveMode == MAP::WALK ) {
            playBGM( BGMforLocation( currentLocation ) );
        }
    }

    void onMovementTypeChange( MAP::moveMode p_newMoveMode ) {
        if( currentMoveMode == p_newMoveMode ) { return; }

        currentMoveMode = p_newMoveMode;
        playBGM( BGMforMoveMode( currentMoveMode ) );
    }

    void restartBGM( ) {
        playBGM( BGMforMoveMode( currentMoveMode ) );
    }

    u16 BGMforMoveMode( MAP::moveMode p_moveMode ) {
        switch( p_moveMode ) {
            case MAP::DIVE:
                return MOD_DIVING;
            case MAP::SURF:
                return MOD_SURFING;
            case MAP::BIKE:
            case MAP::ACRO_BIKE:
            case MAP::BIKE_JUMP:
                return MOD_CYCLING;
            case MAP::WALK:
            case MAP::ROCK_CLIMB:
            case MAP::SIT:
            default:
                return BGMforLocation( currentLocation );
        }
    }


    u16 BGMforTrainerEncounter( u16 p_trainerClassId ) {
        switch( p_trainerClassId ) {
            default:
                return MOD_BATTLE_WILD_ALT;
        }
    }

    u16 BGMforTrainerBattle( u16 p_trainerClassId ) {
        switch( p_trainerClassId ) {
            default:
                return MOD_BATTLE_WILD_ALT;
        }
    }

    u16 BGMforWildBattle( u16 p_pokemonId ) {
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

    u16 BGMforLocation( u16 p_locationId ) {
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
}
