/*
Pokémon neo
------------------------------

file        : battleSide.cpp
author      : Philip Wellnitz
description :

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

#include "battle/battle.h"
#include "battle/battleField.h"
#include "battle/battleSide.h"
#include "battle/battleSlot.h"
#include "battle/battleUI.h"
#include "defines.h"

namespace BATTLE {
    void side::age( battleUI* p_ui ) {
        // TODO: only check for first position in single battle
        for( u8 i = 0; i < 2; ++i ) { _slots[ i ].age( p_ui ); }

        for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
            if( _sideConditionCounter[ i ] && _sideConditionCounter[ i ] < 250 ) {
                if( !--_sideConditionCounter[ i ] ) {
                    _sideConditionCounter[ i ] = 1;
                    removeSideCondition( p_ui, sideCondition( 1 << i ) );
                }
            }
        }
    }

    bool side::addSideCondition( battleUI* p_ui, sideCondition p_sideCondition, u8 p_duration,
                                 bool p_allowStacking ) {
        for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
            if( p_sideCondition & ( 1LLU << i ) ) {
                if( !_sideConditionAmount[ i ] ) {
#ifdef DESQUID_MORE
                    // TODO: proper log
                    p_ui->log( "Set side condition " + std::to_string( 1LLU << i ) );
                    WAIT( HALF_SEC );
#else
                    (void) p_ui;
#endif

                    _sideConditionAmount[ i ]++;
                    _sideConditionCounter[ i ]
                        = p_duration ? p_duration : defaultSideConditionDurations[ i ];
                } else if( p_allowStacking
                           && _sideConditionAmount[ i ] < maxSideConditionAmount[ i ] ) {
                    _sideConditionAmount[ i ]++;
                }
            }
        }
        return true;
    }

    bool side::removeSideCondition( battleUI* p_ui, sideCondition p_sideCondition ) {
        for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
            if( p_sideCondition & ( 1LLU << i ) ) {
#ifdef DESQUID_MORE
                // TODO: proper log
                p_ui->log( "Remove side condition " + std::to_string( 1LLU << i ) );
                WAIT( HALF_SEC );
#else
                (void) p_ui;
#endif
                _sideConditionAmount[ i ]  = 0;
                _sideConditionCounter[ i ] = 0;
            }
        }
        return true;
    }

    u16 side::getStat( u8 p_slot, u8 p_stat, bool p_allowAbilities, bool p_ignoreNegative,
                       bool p_ignorePositive, bool p_wonderRoom ) {
        if( getPkmn( p_slot ) == nullptr ) [[unlikely]] { return 0; }
        u16 base = _slots[ p_slot ].getStat( p_stat, p_allowAbilities, p_ignoreNegative,
                                             p_ignorePositive, p_wonderRoom );

        if( !p_ignorePositive && p_stat == SPEED && _sideConditionAmount[ 8 ] )
            [[unlikely]] { // Tailwind
            base *= 2;
        }

        // plus / minus
        if( p_allowAbilities && !p_ignorePositive ) [[likely]] {
            if( p_stat == SATK && !_slots[ p_slot ].abilitiesNullified( )
                && ( getPkmn( p_slot )->getAbility( ) == A_PLUS
                     || getPkmn( p_slot )->getAbility( ) == A_MINUS ) ) [[unlikely]] {
                auto ot = getPkmn( !p_slot );
                if( ot != nullptr && !_slots[ !p_slot ].abilitiesNullified( )
                    && ( ot->getAbility( ) == A_MINUS || ot->getAbility( ) == A_PLUS ) ) {
                    base = 3 * base / 2;
                }
            }
        }

        return std::max( u16( 1 ), base );
    }
} // namespace BATTLE
