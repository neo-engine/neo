/*
Pokémon neo
------------------------------

file        : battleScripts.cpp
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

#include <map>
#include <set>
#include <string>
#include <vector>

#include <nds.h>

#include "ability.h"
#include "abilityNames.h"
#include "bag.h"
#include "battle.h"
#include "defines.h"
#include "item.h"
#include "itemNames.h"
#include "move.h"
#include "moveNames.h"
#include "pokemon.h"
#include "script.h"

namespace BATTLE {
#define NO_WEATHER( a ) NEQ( a, PKMN_ABILITY, A_AIR_LOCK ), NEQ( a, PKMN_ABILITY, A_CLOUD_NINE )
#define IS_DOUBLE EQ( BATTLE_, BATTLE_MODE, battle::DOUBLE )
#define IS_ALIVE( a ) NEQ( a, PKMN_HP, 0 )
    battleScript weatherEffects[ 9 ] = {
        battleScript( ),
        // Rain
        battleScript( {
            cmd( {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), // Hydration
                  EQ( OWN1, PKMN_ABILITY, A_HYDRATION ), NEQ( OWN1, PKMN_STATUS, 0 )},
                 cmd::OWN1, cmd::PKMN_STATUS, cmd::SET, 0, ( 247 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), EQ( OWN2, PKMN_ABILITY, A_HYDRATION ),
                  NEQ( OWN2, PKMN_STATUS, 0 )},
                 cmd::OWN1, cmd::PKMN_STATUS, cmd::SET, 0, ( 248 ) ),
            cmd( {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN1 ), NO_WEATHER( OWN2 ),
                  NO_WEATHER( OPPONENT2 ), EQ( OPPONENT1, PKMN_ABILITY, A_HYDRATION ),
                  NEQ( OPPONENT1, PKMN_STATUS, 0 )},
                 cmd::OPPONENT1, cmd::PKMN_STATUS, cmd::SET, 0, ( 249 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OWN2 ),
                  NO_WEATHER( OPPONENT1 ), EQ( OPPONENT2, PKMN_ABILITY, A_HYDRATION ),
                  NEQ( OPPONENT2, PKMN_STATUS, 0 )},
                 cmd::OPPONENT2, cmd::PKMN_STATUS, cmd::SET, 0, ( 250 ) ),

            cmd(
                {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), // Dry skin
                 EQ( OWN1, PKMN_ABILITY, A_DRY_SKIN ), NEQ( OWN1, PKMN_HP_PERCENT, 100 )},
                cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 251 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OWN2, PKMN_ABILITY, A_DRY_SKIN ),
                 NEQ( OWN2, PKMN_HP_PERCENT, 100 )},
                cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 252 ) ),
            cmd(
                {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OPPONENT1, PKMN_ABILITY, A_DRY_SKIN ),
                 NEQ( OPPONENT1, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 253 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OWN2 ), EQ( OPPONENT2, PKMN_ABILITY, A_DRY_SKIN ),
                 NEQ( OPPONENT2, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 254 ) ),

            cmd(
                {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), // Rain dish
                 EQ( OWN1, PKMN_ABILITY, A_RAIN_DISH ), NEQ( OWN1, PKMN_HP_PERCENT, 100 )},
                cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 251 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OWN2, PKMN_ABILITY, A_RAIN_DISH ),
                 NEQ( OWN2, PKMN_HP_PERCENT, 100 )},
                cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 252 ) ),
            cmd(
                {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OPPONENT1, PKMN_ABILITY, A_RAIN_DISH ),
                 NEQ( OPPONENT1, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 253 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OWN2 ), EQ( OPPONENT2, PKMN_ABILITY, A_RAIN_DISH ),
                 NEQ( OPPONENT2, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 254 ) ),
        } ),
        battleScript( std::vector<cmd>{
            // Hail
            cmd( {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), NEQ( OWN1, PKMN_TYPE, ICE ),
                  NEQ( OWN1, PKMN_ABILITY, A_ICE_BODY ), NEQ( OWN1, PKMN_ABILITY, A_SNOW_CLOAK ),
                  NEQ( OWN1, PKMN_ABILITY, A_MAGIC_GUARD ), NEQ( OWN1, PKMN_ABILITY, A_OVERCOAT ),
                  NEQ( OWN1, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 255 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), NEQ( OWN2, PKMN_TYPE, ICE ),
                  NEQ( OWN2, PKMN_ABILITY, A_ICE_BODY ), NEQ( OWN2, PKMN_ABILITY, A_SNOW_CLOAK ),
                  NEQ( OWN2, PKMN_ABILITY, A_MAGIC_GUARD ), NEQ( OWN2, PKMN_ABILITY, A_OVERCOAT ),
                  NEQ( OWN2, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 256 ) ),
            cmd( {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                  NO_WEATHER( OPPONENT2 ), NEQ( OPPONENT1, PKMN_TYPE, ICE ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_ICE_BODY ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_SNOW_CLOAK ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_MAGIC_GUARD ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_OVERCOAT ),
                  NEQ( OPPONENT1, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                 val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 257 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                  NO_WEATHER( OPPONENT1 ), NEQ( OPPONENT2, PKMN_TYPE, ICE ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_ICE_BODY ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_SNOW_CLOAK ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_MAGIC_GUARD ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_OVERCOAT ),
                  NEQ( OPPONENT2, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                 val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 258 ) ),

            cmd(
                {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), // Ice Body
                 EQ( OWN1, PKMN_ABILITY, A_ICE_BODY ), NEQ( OWN1, PKMN_HP_PERCENT, 100 )},
                cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 251 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OWN2, PKMN_ABILITY, A_ICE_BODY ),
                 NEQ( OWN2, PKMN_HP_PERCENT, 100 )},
                cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 252 ) ),
            cmd(
                {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OPPONENT1, PKMN_ABILITY, A_ICE_BODY ),
                 NEQ( OPPONENT1, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 253 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OWN2 ), EQ( OPPONENT2, PKMN_ABILITY, A_ICE_BODY ),
                 NEQ( OPPONENT2, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, 1.0f / 16 ),
                ( 254 ) ),
        } ),
        battleScript( std::vector<cmd>{
            // Fog
        } ),
        battleScript( std::vector<cmd>{
            // Sand
            cmd( {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), NEQ( OWN1, PKMN_TYPE, GROUND ),
                  NEQ( OWN1, PKMN_TYPE, ROCK ), NEQ( OWN1, PKMN_TYPE, STEEL ),
                  NEQ( OWN1, PKMN_ABILITY, A_SAND_FORCE ), NEQ( OWN1, PKMN_ABILITY, A_SAND_RUSH ),
                  NEQ( OWN1, PKMN_ABILITY, A_SAND_VEIL ), NEQ( OWN1, PKMN_ABILITY, A_MAGIC_GUARD ),
                  NEQ( OWN1, PKMN_ABILITY, A_OVERCOAT ), NEQ( OWN1, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 259 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), NEQ( OWN2, PKMN_TYPE, GROUND ),
                  NEQ( OWN2, PKMN_TYPE, ROCK ), NEQ( OWN2, PKMN_TYPE, STEEL ),
                  NEQ( OWN2, PKMN_ABILITY, A_SAND_FORCE ), NEQ( OWN2, PKMN_ABILITY, A_SAND_RUSH ),
                  NEQ( OWN2, PKMN_ABILITY, A_SAND_VEIL ), NEQ( OWN2, PKMN_ABILITY, A_MAGIC_GUARD ),
                  NEQ( OWN2, PKMN_ABILITY, A_OVERCOAT ), NEQ( OWN2, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 260 ) ),
            cmd( {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                  NO_WEATHER( OPPONENT2 ), NEQ( OPPONENT1, PKMN_TYPE, GROUND ),
                  NEQ( OPPONENT1, PKMN_TYPE, ROCK ), NEQ( OPPONENT1, PKMN_TYPE, STEEL ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_SAND_FORCE ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_SAND_RUSH ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_SAND_VEIL ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_MAGIC_GUARD ),
                  NEQ( OPPONENT1, PKMN_ABILITY, A_OVERCOAT ),
                  NEQ( OPPONENT1, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                 val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 261 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                  NO_WEATHER( OPPONENT1 ), NEQ( OPPONENT2, PKMN_TYPE, GROUND ),
                  NEQ( OPPONENT2, PKMN_TYPE, ROCK ), NEQ( OPPONENT2, PKMN_TYPE, STEEL ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_SAND_FORCE ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_SAND_RUSH ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_SAND_VEIL ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_MAGIC_GUARD ),
                  NEQ( OPPONENT2, PKMN_ABILITY, A_OVERCOAT ),
                  NEQ( OPPONENT2, PKMN_ITEM, I_SAFETY_GOGGLES )},
                 cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                 val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, -1.0f / 16 ),
                ( 262 ) ),
        } ),
        battleScript( std::vector<cmd>{
            // Sun
            cmd( {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), // Dry skin
                  EQ( OWN1, PKMN_ABILITY, A_DRY_SKIN ), NEQ( OWN1, PKMN_HP_PERCENT, 100 )},
                 cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, -1.0f / 8 ),
                ( 263 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OPPONENT2 ), EQ( OWN2, PKMN_ABILITY, A_DRY_SKIN ),
                  NEQ( OWN2, PKMN_HP_PERCENT, 100 )},
                 cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, -1.0f / 8 ),
                ( 264 ) ),
            cmd( {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                  NO_WEATHER( OPPONENT2 ), EQ( OPPONENT1, PKMN_ABILITY, A_DRY_SKIN ),
                  NEQ( OPPONENT1, PKMN_HP_PERCENT, 100 )},
                 cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                 val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, -1.0f / 8 ),
                ( 265 ) ),
            cmd( {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                  NO_WEATHER( OWN2 ), EQ( OPPONENT2, PKMN_ABILITY, A_DRY_SKIN ),
                  NEQ( OPPONENT2, PKMN_HP_PERCENT, 100 )},
                 cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                 val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, -1.0f / 8 ),
                ( 266 ) ),

            cmd(
                {IS_ALIVE( OWN1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), // Solar power
                 EQ( OWN1, PKMN_ABILITY, A_SOLAR_POWER ), NEQ( OWN1, PKMN_HP_PERCENT, 100 )},
                cmd::OWN1, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN1, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 251 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OWN2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OWN2, PKMN_ABILITY, A_SOLAR_POWER ),
                 NEQ( OWN2, PKMN_HP_PERCENT, 100 )},
                cmd::OWN2, cmd::PKMN_HP, cmd::ADD, val( cmd::OWN2, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 252 ) ),
            cmd(
                {IS_ALIVE( OPPONENT1 ), NO_WEATHER( OWN2 ), NO_WEATHER( OWN1 ),
                 NO_WEATHER( OPPONENT2 ), EQ( OPPONENT1, PKMN_ABILITY, A_SOLAR_POWER ),
                 NEQ( OPPONENT1, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT1, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT1, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 253 ) ),
            cmd(
                {IS_DOUBLE, IS_ALIVE( OPPONENT2 ), NO_WEATHER( OWN1 ), NO_WEATHER( OPPONENT1 ),
                 NO_WEATHER( OWN2 ), EQ( OPPONENT2, PKMN_ABILITY, A_SOLAR_POWER ),
                 NEQ( OPPONENT2, PKMN_HP_PERCENT, 100 )},
                cmd::OPPONENT2, cmd::PKMN_HP, cmd::ADD,
                val( cmd::OPPONENT2, cmd::PKMN_MAX_HP, 1.0f / 8 ),
                ( 254 ) ),
        } ),

        battleScript( std::vector<cmd>{} ), battleScript( std::vector<cmd>{} ),
        battleScript( std::vector<cmd>{} )};
    u16 weatherMessage[ 9 ]    = { 0, 267, 268, 269, 270, 271, 272, 273, 274 };
    u16 weatherEndMessage[ 9 ] = { 0, 275, 276, 277, 278, 279, 280, 281, 282 };
}
