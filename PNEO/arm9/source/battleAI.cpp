/*
Pokémon neo
------------------------------

file        : battleAI.cpp
author      : Philip Wellnitz
description : Implementations for wild poke / trainer AI

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
#include "battle/battleTrainer.h"
#include "battle/battleUI.h"
#include "defines.h"
#include "fs/data.h"
#include "gen/bgmNames.h"
#include "io/animations.h"
#include "io/uio.h"
#include "pokemon.h"
#include "sound/sound.h"

namespace BATTLE {
    u8 battle::getNextAIPokemon( ) const {
        for( u8 i = getBattlingPKMNCount( _policy.m_mode ); i < _opponentTeamSize; ++i ) {
            if( _opponentTeam[ i ].canBattle( ) ) { return i; }
        }
        return 255;
    }

    battleMoveSelection battle::getAIMove( u8 p_slot ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::OPPONENT_SIDE, p_slot };
        auto pkmn               = _field.getPkmn( field::OPPONENT_SIDE, p_slot );
        if( pkmn == nullptr ) { return res; }

        // Use item if remotely sensible
        for( u8 i = 0; i < trainerData::NUM_ITEMS; ++i ) {
            switch( _opponent.m_data.m_items[ i ] ) {
            case I_POTION:
            case I_FRESH_WATER:
            case I_ENERGY_POWDER:
            case I_SUPER_POTION:
            case I_SODA_POP:
            case I_LEMONADE:
            case I_MOOMOO_MILK:
            case I_HYPER_POTION:
            case I_ENERGY_ROOT:
            case I_MAX_POTION:
            case I_FULL_RESTORE:
                if( pkmn->m_stats.m_curHP * 5 < pkmn->m_stats.m_maxHP ) {
                    res.m_type  = MT_USE_ITEM;
                    res.m_param = _opponent.m_data.m_items[ i ];
                    return res;
                }
                break;
            case I_HEAL_POWDER:
            case I_FULL_HEAL:
                if( pkmn->m_statusint ) {
                    res.m_type  = MT_USE_ITEM;
                    res.m_param = _opponent.m_data.m_items[ i ];
                    return res;
                }
                break;
            case I_X_ATTACK:
            case I_X_DEFENSE:
            case I_X_SPEED:
            case I_X_ACCURACY:
            case I_X_SP_ATK:
            case I_X_SP_DEF:
                res.m_type  = MT_USE_ITEM;
                res.m_param = _opponent.m_data.m_items[ i ];
                return res;
            default: break;
            }
        }

        // Mega evolve starting with ai level 6
        if( _AILevel >= 6 ) {
            if( pkmn->canBattleTransform( ) ) { res.m_megaEvolve = true; }
        }
        res.m_type = MT_ATTACK;
        // check if pkmn has move left, ow struggle
        bool canUse[ 4 ], str = false;
        for( u8 i = 0; i < 4; ++i ) {
            canUse[ i ] = _field.canSelectMove( field::OPPONENT_SIDE, p_slot, i );
            str         = str || canUse[ i ];
        }

        if( !str ) { // pkmn struggles
            res.m_param = M_STRUGGLE;
        }

        switch( _AILevel ) {
        default:
            [[likely]] case 0 : { // Wild pkmn
                if( str ) {
                    // Pick a random move
                    u8 mv = rand( ) % 4;
                    while( !canUse[ mv ] ) { mv = rand( ) % 4; }
                    res.m_param = _field.getPkmn( field::OPPONENT_SIDE, p_slot )->getMove( mv );
                }
                // Choose a target
                // Pick a random target
                auto mdata     = FS::getMoveData( res.m_param );
                res.m_moveData = mdata;
                auto tg
                    = mdata.m_pressureTarget != TG_NONE ? mdata.m_pressureTarget : mdata.m_target;

                bool canTarget[ 4 ];
                for( u8 i = 0; i < 4; ++i ) {
                    canTarget[ i ] = _field.getPkmn( i < 2, i & 1 ) != nullptr;
                }
                u8 ctg = rand( ) % 2;

                switch( tg ) {
                case TG_RANDOM:
                case TG_ANY_FOE:
                    [[likely]] case TG_ANY : while( !canTarget[ 2 + ctg ] ) {
                        ctg = rand( ) & 1;
                    }
                    res.m_target = fieldPosition( field::PLAYER_SIDE, ctg );
                    break;
                case TG_ALLY_OR_SELF:
                    while( !canTarget[ ctg ] ) { ctg = rand( ) & 1; }
                    res.m_target = fieldPosition( field::OPPONENT_SIDE, ctg );
                    break;
                    [[unlikely]] default : break;
                }

                return res;
            }
        case 1:
        case 2: // Simple trainer
        case 3: // Ace trainer
        case 4: // Rival
        case 5: // Gym Leader / simple battle frontier
        case 6: // Elite Four / 7-streak battle frontier
        case 7: // Champ
        case 8: // Frontier Brain
        case 9: {
            u8         score[ 4 ] = { 100, 100, 100, 100 };
            battleMove bmove[ 4 ];

            for( u8 i = 0; i < 4; ++i ) {
                if( !canUse[ i ] ) {
                    score[ i ] = 0;
                    continue;
                }
                bmove[ i ].m_param    = pkmn->getMove( i );
                bmove[ i ].m_moveData = FS::getMoveData( pkmn->getMove( i ) );
                // TODO: do this properly for double battles
                auto tg = bmove[ i ].m_moveData.m_pressureTarget != TG_NONE
                              ? bmove[ i ].m_moveData.m_pressureTarget
                              : bmove[ i ].m_moveData.m_target;

                bool canTarget[ 4 ];
                for( u8 j = 0; j < 4; ++j ) {
                    if( _field.getPkmn( j < 2, j & 1 ) == nullptr ) {
                        canTarget[ j ] = false;
                        continue;
                    }
                    if( _AILevel > 3 ) {
                        canTarget[ j ] = !!_field.getEffectiveness( bmove[ i ], { j / 2, j % 2 } );
                    } else {
                        canTarget[ j ] = true;
                    }
                }
                u8 ctg = rand( ) % 2;

                switch( tg ) {
                case TG_RANDOM:
                case TG_ANY_FOE:
                    [[likely]] case TG_ANY : if( !canTarget[ 2 ] && !canTarget[ 3 ] ) {
                        ctg = 0;
                    }
                    else {
                        while( !canTarget[ 2 + ctg ] ) { ctg = rand( ) & 1; }
                    }
                    bmove[ i ].m_target = { fieldPosition( field::PLAYER_SIDE, ctg ) };
                    break;
                case TG_ALLY_OR_SELF:
                    while( !canTarget[ ctg ] ) { ctg = rand( ) & 1; }
                    bmove[ i ].m_target = { fieldPosition( field::OPPONENT_SIDE, ctg ) };
                    break;
                case TG_SELF:
                    bmove[ i ].m_target = { fieldPosition( field::OPPONENT_SIDE, p_slot ) };
                    break;
                    [[unlikely]] default : bmove[ i ].m_target
                        = { fieldPosition( field::PLAYER_SIDE, field::PKMN_0 ) };
                    break;
                }

                auto target = _field.getPkmn( bmove[ i ].m_target[ 0 ].first,
                                              bmove[ i ].m_target[ 0 ].second );

                if( _AILevel < 4 && bmove[ i ].m_moveData.m_category == MH_STATUS ) {
                    // Bad trainers don't want to use status moves
                    score[ i ] -= 5;
                } else if( bmove[ i ].m_moveData.m_category == MH_STATUS ) {
                    if( _AILevel > 2 && bmove[ i ].m_moveData.m_weather != _field.getWeather( ) ) {
                        if( _AILevel > 4 && _field.suppressesWeather( ) ) {
                            score[ i ] = 1;
                            continue;
                        } else {
                            score[ i ] = 200;
                        }
                    } else if( _AILevel > 3 && bmove[ i ].m_moveData.m_status && target != nullptr
                               && !target->m_statusint ) {
                        score[ i ] += 20;
                    } else if( pkmn->m_boxdata.m_curPP[ i ] + _AILevel / 2
                               < bmove[ i ].m_moveData.m_pp ) {
                        score[ i ] = 1;
                        continue;
                    }
                    score[ i ] = score[ i ] + 5 - ( rand( ) % ( 13 - _AILevel ) );
                }

                // Check for vol stat changes
                if( _AILevel > 4 && bmove[ i ].m_moveData.m_volatileStatus ) {
                    if( target != nullptr
                        && ( _field.getVolatileStatus( field::OPPONENT_SIDE, p_slot )
                             & bmove[ i ].m_moveData.m_volatileStatus ) ) {
                        score[ i ] = 1;
                        continue;
                    } else if( _AILevel > 5 ) {
                        score[ i ] += 5;
                    }
                }

                if( _AILevel > 4
                    && _field.hasType( field::OPPONENT_SIDE, p_slot,
                                       bmove[ i ].m_moveData.m_type ) ) {
                    score[ i ] += _AILevel / 2;
                }
                if( bmove[ i ].m_moveData.m_category == MH_PHYSICAL ) {
                    if( _AILevel > 3
                        && _field.getStat( field::OPPONENT_SIDE, p_slot, ATK )
                               > _field.getStat( field::OPPONENT_SIDE, p_slot, SATK ) ) {
                        score[ i ] += _AILevel / 2;
                    }
                }
                if( bmove[ i ].m_moveData.m_category == MH_SPECIAL ) {
                    if( _AILevel > 3
                        && _field.getStat( field::OPPONENT_SIDE, p_slot, SATK )
                               > _field.getStat( field::OPPONENT_SIDE, p_slot, ATK ) ) {
                        score[ i ] += _AILevel / 2;
                    }
                }

                if( _AILevel > 5 ) {
                    u16 eff = _field.getEffectiveness( bmove[ i ], bmove[ i ].m_target[ 0 ] );
                    score[ i ] += ( eff - 100 ) / 3;
                }
                if( _AILevel > 2 ) {
                    if( bmove[ i ].m_moveData.m_basePower > _AILevel * 9 ) {
                        score[ i ] += ( bmove[ i ].m_moveData.m_basePower - 50 ) / 10;
                    }
                }
                if( _AILevel > 6 ) {
                    score[ i ] -= ( 100 - bmove[ i ].m_moveData.m_accuracy ) / 10;
                }
                score[ i ] += ( rand( ) % 5 );
            }

            // pick the move with the highest score
            u8 mxscr = 0, idx = -1;
            for( u8 i = 0; i < 4; ++i ) {
                if( score[ i ] > mxscr ) {
                    mxscr = score[ i ];
                    idx   = i;
                }
            }

            // TODO: switch pkmn if mx score is too low

            res.m_type     = MT_ATTACK;
            res.m_target   = bmove[ idx ].m_target[ 0 ];
            res.m_param    = bmove[ idx ].m_param;
            res.m_moveData = bmove[ idx ].m_moveData;

            break;
        }
        }
        return res;
    }
} // namespace BATTLE
