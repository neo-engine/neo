/*
   Pokémon neo
   ------------------------------

   file        : battleTrainer.h
   author      : Philip Wellnitz
   description : Header file. Consult the corresponding source file for details.

   Copyright (C) 2012 - 2026
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
#include <functional>
#include <string>
#include <nds/ndstypes.h>
#include "defines.h"
#include "gen/trainerClassNames.h"
#include "io/easyChat.h"
#include "pokemon.h"

namespace BATTLE {
    constexpr u8 OWNER_A        = 0;
    constexpr u8 OWNER_B        = 1;
    constexpr u8 NO_PKMN        = 255;
    constexpr u8 MULTI_MAX_SIZE = 12;

    struct trainerStrings {
        static constexpr u8  NAME_LENGTH = TRAINERNAME_LEN;
        static constexpr u16 MSG_LENGTH  = TRAINERMSG_LEN;

        char m_name[ NAME_LENGTH ];
        char m_message1[ MSG_LENGTH ];
        char m_message2[ MSG_LENGTH ];
    };

    struct trainerData {
        static constexpr u8 NUM_ITEMS = 5;
        static constexpr u8 NUM_PKMN  = 6;

        u16 m_trainerClass : 8;
        u16 m_trainerBG : 8;

        u16 m_battlePlat1 : 8;
        u16 m_battlePlat2 : 8;

        u16 m_battleBG : 8;
        u16 m_AILevel : 4;
        u16 m_numPokemon : 3;
        u16 m_forceDoubleBattle : 1;

        u16 m_moneyMultiplier;

        u16            m_items[ NUM_ITEMS ];
        trainerPokemon m_pokemon[ NUM_PKMN ];
    };

    struct battleTrainer {
        trainerStrings m_strings;
        trainerData    m_data;

        constexpr u8 getClass( ) const {
            return m_data.m_trainerClass;
        }
    };

    struct bfTrainer {
        IO::easyChatMessage m_beforeBattle;
        IO::easyChatMessage m_onWinAgainstPlayer;
        IO::easyChatMessage m_onLoseAgainstPlayer;
        u16                 m_trainerClass : 7;
        u16                 m_trainerNameIdx : 9;
        u16                 m_trainerBG : 8; // in-battle pic
        u16                 m_picnum : 8;    // ow sprite num
    };

    constexpr bool isDoubleBattleTrainerClass( u16 p_trainerClass ) {
        switch( p_trainerClass ) {
        case TC_ACE_DUO:
        case TC_BRAINS_BRAWN:
        case TC_HONEYMOONERS:
        case TC_INTERVIEWERS:
        case TC_LEADERS:
        case TC_MYSTERIOUS_SISTERS:
        case TC_POKE_FAN_FAMILY:
        case TC_PUNK_COUPLE:
        case TC_SIS_BRO:
        case TC_SWIMMERS:
        case TC_TEAMMATES:
        case TC_TWINS:
        case TC_YOUNG_COUPLE: return true;
        default: break;
        }
        return false;
    }

    constexpr u8 getTeamIndex( u8 p_owner, u8 p_position, bool p_multi ) {
        return p_multi ? u8( 2 * p_position + p_owner ) : p_position;
    }

    constexpr u8 teamIndexToOwner( u8 p_teamIndex, bool p_multi ) {
        return p_multi ? u8( p_teamIndex & 1 ) : OWNER_A;
    }

    constexpr u8 nextBenchIndex( const std::function<bool( u8 )>& p_canBattle, u8 p_teamSize,
                                 u8 p_activeCount, u8 p_owner, bool p_multi ) {
        for( u8 i = p_activeCount; i < p_teamSize; ++i ) {
            if( p_multi && u8( i & 1 ) != p_owner ) { continue; }
            if( p_canBattle( i ) ) { return i; }
        }
        return NO_PKMN;
    }

    constexpr void sortGroup( u8 p_offset, u8 p_step, u8 p_groupLen,
                              const std::function<bool( u8 )>&     p_canBattle,
                              const std::function<void( u8, u8 )>& p_swap ) {
        for( u8 i = 0; i + 1 < p_groupLen; ++i ) {
            if( !p_canBattle( p_offset ) ) {
                for( u8 j = 1; j + i < p_groupLen; ++j ) {
                    p_swap( u8( p_offset + ( j - 1 ) * p_step ), u8( p_offset + j * p_step ) );
                }
            }
        }
    }

    constexpr void restoreGroup( u8 p_offset, u8 p_step, u8 p_groupLen, u8* p_perm,
                                 const std::function<void( u8, u8 )>& p_swap ) {
        for( u8 k = 0; k + 1 < p_groupLen; ++k ) {
            const u8 i = u8( p_offset + k * p_step );
            while( p_perm[ i ] != i ) {
                p_swap( i, p_perm[ i ] );
                std::swap( p_perm[ i ], p_perm[ p_perm[ i ] ] );
            }
        }
    }

    constexpr u8 groupLen( u8 p_size, u8 p_owner, bool p_multi ) {
        if( !p_multi ) { return p_size; }
        return u8( ( p_size - p_owner + 1 ) / 2 );
    }

    constexpr bool holdsMember( u8 p_teamIndex, u8 p_count, u8 p_count2, bool p_multi ) {
        if( !p_multi ) { return true; }
        return ( p_teamIndex >> 1 ) < ( ( p_teamIndex & 1 ) ? p_count2 : p_count );
    }
} // namespace BATTLE
