/*
    Pokémon neo
    ------------------------------

    file        : pokeblock.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2023 - 2023
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
#include "bag/berry.h"
#include "battle/battleDefines.h"
#include "pokemonData.h"

namespace BAG {
    enum pokeblockType : u8 {
        PB_BLACK           = 0,
        PB_GOLD            = 1,
        PB_GOLD_DX         = 2,
        PB_ULTIMATE        = 3,
        PB_LIGHT_RED       = 4,
        PB_LIGHT_RED_DX    = 5,
        PB_RED             = 6,
        PB_RED_DX          = 7,
        PB_LIGHT_BLUE      = 8,
        PB_LIGHT_BLUE_DX   = 9,
        PB_BLUE            = 10,
        PB_BLUE_DX         = 11,
        PB_LIGHT_PINK      = 12,
        PB_LIGHT_PINK_DX   = 13,
        PB_PINK            = 14,
        PB_PINK_DX         = 15,
        PB_LIGHT_YELLOW    = 16,
        PB_LIGHT_YELLOW_DX = 17,
        PB_YELLOW          = 18,
        PB_YELLOW_DX       = 19,
        PB_LIGHT_GREEN     = 20,
        PB_LIGHT_GREEN_DX  = 21,
        PB_GREEN           = 22,
        PB_GREEN_DX        = 23,
    };

    enum berryFlavor : u8 {
        BY_SPICY  = 0,
        BY_DRY    = 1,
        BY_SWEET  = 2,
        BY_BITTER = 3,
        BY_SOUR   = 4,
    };

    constexpr u8 PB_LIGHT_THRESHOLD    = 25;  // level < 25 ~> light block
    constexpr u8 PB_GOLD_THRESHOLD     = 50;  // level > 50 ~> gold block
    constexpr u8 PB_ULTIMATE_THRESHOLD = 100; // level > 100 ~> ultimate block

    constexpr u8 PB_DX_THRESHOLD = 25; // smooth < 25 ~> dx block

    constexpr u8 PB_BLACK_LEVEL  = 1;
    constexpr u8 PB_LIGHT_LEVEL  = 25;
    constexpr u8 PB_NORMAL_LEVEL = 40;
    constexpr u8 PB_GOLD_LEVEL   = 50;

    constexpr u8 PB_DX_SMOOTH       = 10;
    constexpr u8 PB_NORMAL_SMOOTH   = 20;
    constexpr u8 PB_GOLD_SMOOTH     = 100;
    constexpr u8 PB_GOLD_DX_SMOOTH  = 75;
    constexpr u8 PB_ULTIMATE_SMOOTH = 51;

    constexpr u8 DISLIKED_FALVOR = 9;
    constexpr u8 NORMAL_FALVOR   = 10;
    constexpr u8 LIKED_FALVOR    = 11;

    struct pokeblock {
        inline static constexpr pokeblockType fromLevelSmoothness( u8 p_level[ NUM_BERRYSTATS ],
                                                                   u8 p_smoothness ) {
            if( !p_smoothness ) { return PB_BLACK; }

            u8 mx = 0, argmx = NUM_BERRYSTATS, tie = 0;
            for( u8 i = 0; i < NUM_BERRYSTATS; ++i ) {
                if( p_level[ i ] > mx ) {
                    tie   = 0;
                    mx    = p_level[ i ];
                    argmx = i;
                } else if( p_level[ i ] == mx ) {
                    tie++;
                }
            }

            bool isLight    = mx < PB_LIGHT_THRESHOLD;
            bool isGold     = tie > 1 && mx > PB_GOLD_THRESHOLD;
            bool isUltimate = tie > 2 && mx > PB_ULTIMATE_THRESHOLD;
            bool isDX       = p_smoothness < PB_DX_THRESHOLD;

            if( isUltimate ) { return PB_ULTIMATE; }
            if( isGold && isDX ) { return PB_GOLD_DX; }
            if( isGold ) { return PB_GOLD; }

            return static_cast<pokeblockType>( PB_LIGHT_RED + 4 * argmx + isDX + 2 * !isLight );
        }

        inline static constexpr u8 flavorStrength( pokeblockType p_type, u8 p_flavor ) {
            switch( p_type ) {
            default:
            case PB_BLACK: return PB_BLACK_LEVEL;
            case PB_GOLD:
            case PB_GOLD_DX:
            case PB_ULTIMATE: return PB_GOLD_LEVEL;
            case PB_LIGHT_RED:
            case PB_LIGHT_RED_DX: return ( p_flavor == BY_SPICY ) * PB_LIGHT_LEVEL;
            case PB_RED:
            case PB_RED_DX: return ( p_flavor == BY_SPICY ) * PB_NORMAL_LEVEL;
            case PB_LIGHT_BLUE:
            case PB_LIGHT_BLUE_DX: return ( p_flavor == BY_DRY ) * PB_LIGHT_LEVEL;
            case PB_BLUE:
            case PB_BLUE_DX: return ( p_flavor == BY_DRY ) * PB_NORMAL_LEVEL;
            case PB_LIGHT_PINK:
            case PB_LIGHT_PINK_DX: return ( p_flavor == BY_SWEET ) * PB_LIGHT_LEVEL;
            case PB_PINK:
            case PB_PINK_DX: return ( p_flavor == BY_SWEET ) * PB_NORMAL_LEVEL;
            case PB_LIGHT_YELLOW:
            case PB_LIGHT_YELLOW_DX: return ( p_flavor == BY_BITTER ) * PB_LIGHT_LEVEL;
            case PB_YELLOW:
            case PB_YELLOW_DX: return ( p_flavor == BY_BITTER ) * PB_NORMAL_LEVEL;
            case PB_LIGHT_GREEN:
            case PB_LIGHT_GREEN_DX: return ( p_flavor == BY_SOUR ) * PB_LIGHT_LEVEL;
            case PB_GREEN:
            case PB_GREEN_DX: return ( p_flavor == BY_SOUR ) * PB_NORMAL_LEVEL;
            }
        };

        inline static constexpr u8 flavorStrength( pokeblockType p_type ) {
            u8 mx = 0;
            for( u8 i = 0; i < NUM_BERRYSTATS; ++i ) {
                mx = std::max( flavorStrength( p_type, i ), mx );
            }
            return mx;
        }

        inline static constexpr u8 smoothness( pokeblockType p_type ) {
            switch( p_type ) {
            case PB_GOLD: return PB_GOLD_SMOOTH;
            case PB_GOLD_DX: return PB_GOLD_DX_SMOOTH;
            case PB_ULTIMATE: return PB_ULTIMATE_SMOOTH;
            case PB_LIGHT_RED_DX:
            case PB_RED_DX:
            case PB_LIGHT_BLUE_DX:
            case PB_BLUE_DX:
            case PB_LIGHT_PINK_DX:
            case PB_PINK_DX:
            case PB_LIGHT_YELLOW_DX:
            case PB_YELLOW_DX:
            case PB_LIGHT_GREEN_DX:
            case PB_GREEN_DX: return PB_DX_SMOOTH;
            default: return PB_NORMAL_SMOOTH;
            }
        }

        inline static constexpr u8 strengthModifier( pokeblockType p_type, u8 p_nature ) {
            switch( p_type ) {
            case PB_BLACK: return DISLIKED_FALVOR;
            default:
            case PB_GOLD:
            case PB_GOLD_DX: return NORMAL_FALVOR;
            case PB_ULTIMATE: return LIKED_FALVOR;
            case PB_LIGHT_RED:
            case PB_LIGHT_RED_DX:
            case PB_RED:
            case PB_RED_DX: return NatMod[ p_nature ][ BATTLE::ATK - 1 ];
            case PB_LIGHT_BLUE:
            case PB_LIGHT_BLUE_DX:
            case PB_BLUE:
            case PB_BLUE_DX: return NatMod[ p_nature ][ BATTLE::SATK - 1 ];
            case PB_LIGHT_PINK:
            case PB_LIGHT_PINK_DX:
            case PB_PINK:
            case PB_PINK_DX: return NatMod[ p_nature ][ BATTLE::SPEED - 1 ];
            case PB_LIGHT_YELLOW:
            case PB_LIGHT_YELLOW_DX:
            case PB_YELLOW:
            case PB_YELLOW_DX: return NatMod[ p_nature ][ BATTLE::DEF - 1 ];
            case PB_LIGHT_GREEN:
            case PB_LIGHT_GREEN_DX:
            case PB_GREEN:
            case PB_GREEN_DX: return NatMod[ p_nature ][ BATTLE::SDEF - 1 ];
            }
        }
    };
} // namespace BAG
