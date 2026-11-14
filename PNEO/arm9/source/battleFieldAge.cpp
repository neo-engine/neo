/*
Pokémon neo
------------------------------

file        : battleFieldAge.cpp
author      : Philip Wellnitz
description : turn upkeep: timers, volatiles, hazards, end-of-turn effects

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

#include "battle/ability.h"
#include "battle/battle.h"
#include "battle/battleField.h"
#include "battle/battleSide.h"
#include "battle/battleSlot.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "battle/type.h"
#include "defines.h"
#include "gen/abilityNames.h"
#include "gen/itemNames.h"
#include "gen/pokemonNames.h"
#include "save/saveGame.h"

namespace BATTLE {
    void field::age( battleUI* p_ui ) {
        for( u8 i = 0; i < NUM_SIDES; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                _actionLastTurn[ i ][ j ] = _actionThisTurn[ i ][ j ];
                _actionThisTurn[ i ][ j ] = 0;
            }
        }
        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                if( !( getSlotCondition( i, j ) & SL_FUTURESIGHT ) ) { continue; }
                if( getSlotConditionDuration( i, j, SL_FUTURESIGHT ) != 1 ) { continue; }

                const u16 mv  = getSlotConditionAmount( i, j, SL_FUTURESIGHT );
                const u16 src = getSlotConditionSrc( i, j );
                removeSlotCondition( p_ui, i, j, SL_FUTURESIGHT );

                auto victim = getPkmn( i, j );
                if( victim == nullptr || !victim->canBattle( ) ) { continue; }

                battleMove fs    = { };
                fs.m_type        = MT_ATTACK;
                fs.m_param       = mv;
                fs.m_moveData    = FS::getMoveData( mv );
                fs.m_user.first  = u8( ( src >> 8 ) & 1 );
                fs.m_user.second = u8( src & 0xFF );
                fs.m_target.push_back( { i, j } );
                if( getPkmn( fs.m_user.first, fs.m_user.second ) == nullptr ) { continue; }
                bool critical = executeCriticalCheck( p_ui, fs, { i, j } );
                executeDamagingMove( p_ui, fs, { i, j }, critical, 100 );
            }
        }
        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                if( !( getSlotCondition( i, j ) & SL_WISH ) ) { continue; }
                if( getSlotConditionDuration( i, j, SL_WISH ) != 1 ) { continue; }

                const std::string wisher = getSlotConditionWisher( i, j );
                const u16         amount = getSlotConditionAmount( i, j, SL_WISH );

                removeSlotCondition( p_ui, i, j, SL_WISH );

                auto pkmn = getPkmn( i, j );
                if( pkmn == nullptr || !pkmn->canBattle( ) ) { continue; }

                u16 deficit = pkmn->m_stats.m_maxHP - pkmn->m_stats.m_curHP;
                if( !deficit ) { continue; }

                u16 healed = amount > deficit ? deficit : amount;
                if( !healed ) { continue; }
                healPokemon( p_ui, i, j, healed );

                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_WISH_CAME_TRUE ) );
                std::array<char, 200> buffer{ };
                snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ), wisher.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                p_ui->log( buffer.data( ) );
            }
        }

        for( u8 i = 0; i < 2; ++i ) { _sides[ i ].age( p_ui ); }
        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto sl = getSlot( i, j );
                if( sl == nullptr ) { continue; }
                bool acted = sl->takeMoveActed( );
                auto cnt   = sl->getLockedMoveCount( );
                if( !cnt ) { continue; }
                auto lm = sl->getLockedMove( );
                if( lm.m_param == M_UPROAR ) {
                    if( getSlotStatus( i, j ) == slot::status::NORMAL && cnt > 1 ) {
                        sl->setLockedMoveCount( u8( cnt - 1 ) );
                    } else {
                        sl->removeLockedMove( );
                    }
                    continue;
                }
                if( !( FS::getMoveData( lm.m_param ).m_flags & MF_LOCKEDMOVE ) ) { continue; }
                auto pk = getPkmn( i, j );
                if( pk == nullptr ) { continue; }
                if( pk->m_status.m_isAsleep ) {
                    sl->removeLockedMove( );
                    continue;
                }
                if( acted ) { continue; }
                if( cnt > 1 ) {
                    sl->setLockedMoveCount( u8( cnt - 1 ) );
                } else {
                    sl->removeLockedMove( );
                    if( getSlotStatus( i, j ) == slot::status::NORMAL
                        && !fatigueConfusionBlocked( i, j ) ) {
                        auto fmtL = std::string( GET_STRING( IO::STR_BATTLE_BECAME_CONFUSED ) );
                        char bufL[ 110 ];
                        snprintf( bufL, sizeof( bufL ), fmtL.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                        p_ui->log( bufL );
                        if( !( getVolatileStatus( i, j ) & VS_CONFUSION ) ) {
                            addVolatileStatus( p_ui, i, j, VS_CONFUSION,
                                               250 + 2 + ( rand( ) % 4 ) );
                        }
                    }
                }
            }
        }
        if( _weatherTimer > 0 ) {
            if( _weatherTimer < 240 && !--_weatherTimer ) {
                // Weather ended
                removeWeather( p_ui );
            } else {
                continueWeather( p_ui );
            }
        }
        if( _terrainTimer > 0 && _terrainTimer < 240 ) {
            if( !--_terrainTimer ) {
                // terrain ended
                removeTerrain( p_ui );
            }
        }

        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( _pseudoWeatherTimer[ i ] > 0 && _pseudoWeatherTimer[ i ] < 240 ) {
                if( !--_pseudoWeatherTimer[ i ] ) {
                    // pseudo weather ended
                    p_ui->removePseudoWeather( i );
                }
            }
        }

        // abilities
        bool supprA = suppressesAbilities( );
        if( !supprA ) {
            for( u8 i = 0; i < 2; ++i ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto pkmn = getPkmn( i, j );
                    if( pkmn == nullptr || supprA || slotAbilitiesNullified( i, j ) ) { continue; }

                    auto adj = getPkmn( i, !j );

                    if( adj != nullptr && !slotAbilitiesNullified( i, u8( !j ) )
                        && adj->getAbility( ) == A_HEALER ) {
                        if( pkmn->m_statusint && !( rand( ) % 3 ) ) {
                            if( removeStatusCondition( i, j ) ) {
                                p_ui->logAbility( getPkmnOrDisguise( i, !j ), i );
                                p_ui->updatePkmnStats( i, j, getPkmnOrDisguise( i, j ), true );
                            }
                        }
                    }

                    switch( pkmn->getAbility( ) ) {
                    case A_SHED_SKIN:
                        if( pkmn->m_statusint && !( rand( ) % 3 ) ) {
                            if( removeStatusCondition( i, j ) ) {
                                p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                                p_ui->updatePkmnStats( i, j, getPkmnOrDisguise( i, j ), true );
                            }
                        }
                        break;
                    case A_SPEED_BOOST: {
                        boosts bs  = getBoosts( i, j );
                        boosts res = boosts( );
                        if( bs.getBoost( SPEED ) < 6 ) {
                            res.setBoost( SPEED, 1 );
                            p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                            auto res2 = addBoosts( i, j, res );
                            p_ui->logBoosts( getPkmnOrDisguise( i, j ), i, j, res, res2 );
                        }
                        break;
                    }
                    case A_MOODY: {
                        boosts bs  = getBoosts( i, j );
                        boosts res = boosts( );
                        u8     rs = 1 + rand( ) % 5, lw = 1 + rand( ) % 5;

                        bool gd = false;
                        for( u8 i2 = 1; i2 < 6; ++i2 ) {
                            if( bs.getBoost( i2 ) < 6 ) { gd = true; }
                        }
                        while( gd && bs.getBoost( rs ) >= 6 ) { rs = 1 + rand( ) % 5; }
                        res.setBoost( rs, 2 );
                        gd = false;
                        for( u8 i2 = 1; i2 < 6; ++i2 ) {
                            if( bs.getBoost( i2 ) > -6 ) { gd = true; }
                        }
                        while( gd && ( bs.getBoost( lw ) <= -6 || rs == lw ) ) {
                            lw = 1 + rand( ) % 5;
                        }
                        res.setBoost( lw, -1 );

                        if( res != boosts( ) ) {
                            p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                            auto res2 = addBoosts( i, j, res );
                            p_ui->logBoosts( getPkmnOrDisguise( i, j ), i, j, res, res2 );
                        }
                        break;
                    }
                    case A_ZEN_MODE:
                        if( pkmn->m_stats.m_curHP * 2 < pkmn->m_stats.m_maxHP
                            && !( pkmn->getForme( ) & 1 ) ) {
                            if( pkmn->getSpecies( ) == PKMN_DARMANITAN ) {
                                p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                                pkmn->battleTransform( );
                                p_ui->updatePkmn( i, j, getPkmnOrDisguise( i, j ) );
                            }
                        }
                        break;
                    case A_POWER_CONSTRUCT:
                        if( pkmn->m_stats.m_curHP * 2 < pkmn->m_stats.m_maxHP
                            && pkmn->getForme( ) < 2 ) {
                            if( pkmn->getSpecies( ) == PKMN_ZYGARDE ) {
                                p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                                pkmn->battleTransform( );
                                p_ui->updatePkmn( i, j, getPkmnOrDisguise( i, j ) );
                            }
                        }
                        break;
                    case A_SHIELDS_DOWN:
                        if( pkmn->m_stats.m_curHP * 2 < pkmn->m_stats.m_maxHP
                            && pkmn->getForme( ) < 7 ) {
                            if( pkmn->getSpecies( ) == PKMN_MINIOR ) {
                                p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                                // TODO: implement minior battle transform
                                pkmn->battleTransform( );
                                p_ui->updatePkmn( i, j, getPkmnOrDisguise( i, j ) );
                            }
                        }
                        break;

                    case A_SCHOOLING: {
                        if( pkmn->m_level >= 20 && pkmn->m_stats.m_curHP * 4 > pkmn->m_stats.m_maxHP
                            && !pkmn->getForme( ) ) {
                            p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                            pkmn->battleTransform( );
                            p_ui->updatePkmn( i, j, getPkmnOrDisguise( i, j ) );
                        }
                        break;
                    }

                    default: break;
                    }
                }
            }
        }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        // volatile status
        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pkmn = getPkmn( i, j );
                if( pkmn == nullptr || supprA ) { continue; }

                auto volst = getVolatileStatus( i, j );
                if( volst & VS_NIGHTMARE ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HARMED_BY_NIGHTMARE ) );
                    if( hasStatusCondition( i, j, SLEEP ) ) {
                        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                        p_ui->log( buffer );
                        u16 amount = pkmn->m_stats.m_maxHP / 4;
                        if( !amount ) { amount = 1; }
                        damagePokemon( p_ui, i, j, amount );
                    } else {
                        _sides[ i ].removeVolatileStatus( p_ui, j, VS_NIGHTMARE );
                    }
                }

                if( volst & VS_YAWN ) {
                    if( getVolatileStatusCounter( i, j, VS_YAWN ) == 1 ) {
                        if( blockSleepByUproar( p_ui, i, j ) ) {
                        } else if( u8 tg
                                   = terrainGuardsStatus( fieldPosition( i, j ), true, true ) ) {
                            if( tg == 2 ) {
                                auto fmtT
                                    = std::string( GET_STRING( IO::STR_BATTLE_TERRAIN_PROTECTS ) );
                                char bufT[ 110 ];
                                snprintf(
                                    bufT, sizeof( bufT ), fmtT.c_str( ),
                                    p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                                p_ui->log( bufT );
                            }
                        } else if( setStatusCondition( i, j, SLEEP, 4 + ( rand( ) & 3 ) ) ) {
                            p_ui->animateGetStatusCondition( getPkmnOrDisguise( i, j ), i, j,
                                                             SLEEP );
                            p_ui->updatePkmnStats( i, j, getPkmnOrDisguise( i, j ), true );
                        }

                        _sides[ i ].removeVolatileStatus( p_ui, j, VS_YAWN );
                    }
                }
                if( volst & VS_AQUARING ) {
                    if( !( volst & VS_HEALBLOCK ) ) {
                        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HEALED_BY_AQUA_RING ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                        p_ui->log( buffer );
                        u16 amount = pkmn->m_stats.m_maxHP / 16;
                        if( !amount ) { amount = 1; }
                        if( canUseItem( i, j ) && pkmn->getItem( ) == I_BIG_ROOT ) {
                            amount = ( amount * 13 ) / 10;
                        }
                        healPokemon( p_ui, i, j, amount );
                    }
                }
                if( volst & VS_INGRAIN ) {
                    if( !( volst & VS_HEALBLOCK ) ) {
                        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HEALED_BY_INGRAIN ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                        p_ui->log( buffer );
                        u16 amount = pkmn->m_stats.m_maxHP / 16;
                        if( !amount ) { amount = 1; }
                        if( canUseItem( i, j ) && pkmn->getItem( ) == I_BIG_ROOT ) {
                            amount = ( amount * 13 ) / 10;
                        }
                        healPokemon( p_ui, i, j, amount );
                    }
                }
                if( volst & VS_CURSE ) {
#ifdef DESQUID_MORE
                    p_ui->log( std::to_string( volst ) );
#endif
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HARMED_BY_CURSE ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                    p_ui->log( buffer );
                    u16 amount = pkmn->m_stats.m_maxHP / 4;
                    if( !amount ) { amount = 1; }
                    damagePokemon( p_ui, i, j, amount );
                }

                if( volst & VS_PARTIALLYTRAPPED ) [[unlikely]] {
                    auto tside = getPartialTrapSide( i, j );
                    auto tslot = getPartialTrapSlot( i, j );
                    auto binder
                        = ( tside < 2 && tslot < 2 ) ? getPkmn( bool( tside ), tslot ) : nullptr;
                    bool gone = binder == nullptr || !binder->canBattle( )
                                || binder->getSpecies( ) != getPartialTrapSpecies( i, j );
                    if( gone || getPartialTrapTimer( i, j ) <= 1 ) {
                        auto fmtT = std::string( GET_STRING( IO::STR_BATTLE_RELEASED_FROM ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmtT.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ),
                                  getPartialTrapWisher( i, j ).c_str( ),
                                  FS::getMoveName( getPartialTrapMove( i, j ) ).c_str( ) );
                        p_ui->log( buffer );
                        clearPartialTrap( i, j );
                        _sides[ i ].removeVolatileStatus( p_ui, j, VS_PARTIALLYTRAPPED );
                    } else {
                        decreasePartialTrapTimer( i, j );
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    }
                }

                if( volst & VS_DISABLE ) [[unlikely]] {
                    if( getVolatileStatusCounter( i, j, VS_DISABLE ) == 1 ) {
                        auto fmtD = std::string( GET_STRING( IO::STR_BATTLE_DISABLE_END ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmtD.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ),
                                  FS::getMoveName( getDisabledMove( i, j ) ).c_str( ) );
                        p_ui->log( buffer );
                        setDisabledMove( i, j, 0 );
                        _sides[ i ].removeVolatileStatus( p_ui, j, VS_DISABLE );
                    }
                }

                if( volst & VS_ENCORE ) [[unlikely]] {
                    bool ppOut = false;
                    auto enc   = getEncoreMove( i, j );
                    if( enc ) {
                        for( u8 s = 0; s < 4; ++s ) {
                            if( pkmn->getMove( s ) == enc ) {
                                ppOut = !pkmn->m_boxdata.m_curPP[ s ];
                                break;
                            }
                        }
                    }
                    if( ppOut || getVolatileStatusCounter( i, j, VS_ENCORE ) == 1 ) {
                        auto fmtE = std::string( GET_STRING( IO::STR_BATTLE_ENCORE_END ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmtE.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                        p_ui->log( buffer );
                        setEncoreMove( i, j, 0 );
                        _sides[ i ].removeVolatileStatus( p_ui, j, VS_ENCORE );
                    }
                }

                if( volst & VS_TELEKINESIS ) [[unlikely]] {
                    if( getVolatileStatusCounter( i, j, VS_TELEKINESIS ) == 1 ) {
                        auto fmtT = std::string( GET_STRING( IO::STR_BATTLE_TELEKINESIS_END ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmtT.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                        p_ui->log( buffer );
                        _sides[ i ].removeVolatileStatus( p_ui, j, VS_TELEKINESIS );
                    }
                }

                if( hasCounter( i, j ) || hasMirrorCoat( i, j ) ) { clearCounters( i, j ); }

                if( getStallWindow( i, j ) ) {
                    setStallWindow( i, j, u8( getStallWindow( i, j ) - 1 ) );
                }

                if( getPerishTimer( i, j ) ) [[unlikely]] {
                    u8 timer = getPerishTimer( i, j );
                    --timer;
                    if( timer >= 1 ) { setPerishTimer( i, j, timer ); }
                    auto fmtP = std::string( GET_STRING( IO::STR_BATTLE_PERISH_COUNT ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmtP.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ),
                              int( timer ) >= 1
                                  ? FS::kanaWrap( u32( int( timer ) - 1 ), CURRENT_LANGUAGE )
                                  : "-1" );
                    p_ui->log( buffer );
                    if( timer == 1 ) {
                        setPerishTimer( i, j, 0 );
                        pkmn->m_stats.m_curHP = 0;
                    }
                }

                if( !getPkmn( i, j )->canBattle( ) ) {
                    // pkmn fainted
                    faintPokemon( p_ui, i, j );
                }
            }
        }

        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pkmn = getPkmn( i, j );
                if( pkmn == nullptr || !pkmn->canBattle( ) ) { continue; }
                if( !( getVolatileStatus( i, j ) & VS_LEECHSEED ) || !seeded( i, j ) ) { continue; }
                auto srcPos = getLeechSource( i, j );
                auto src    = getPkmn( srcPos.first, srcPos.second );
                if( src == nullptr || !src->canBattle( ) ) { continue; }
                u16 drain = pkmn->m_stats.m_maxHP / 8;
                damagePokemon( p_ui, i, j, drain );
                if( !getPkmn( i, j )->canBattle( ) ) { faintPokemon( p_ui, i, j ); }
                healPokemon( p_ui, srcPos.first, srcPos.second, drain );
            }
        }

        if( getTerrain( ) == TR_GRASSYTERRAIN ) {
            for( u8 i = 0; i < 2; ++i ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto pkmn = getPkmn( i, j );
                    if( pkmn == nullptr || !pkmn->canBattle( ) ) { continue; }
                    if( getVolatileStatus( i, j ) & VS_HEALBLOCK ) { continue; }
                    if( !isGrounded( i, j ) ) { continue; }
                    if( isSemiInvulnerable( i, j ) ) { continue; }
                    healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                }
            }
        }

        // status conditions

        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pkmn = getPkmn( i, j );
                if( pkmn == nullptr ) { continue; }

                auto volst = getVolatileStatus( i, j );

                if( hasStatusCondition( i, j, SLEEP ) && !supprA ) {
                    for( u8 i2 = 0; i2 < 2; ++i2 ) {
                        for( u8 j2 = 0; j2 < getBattlingPKMNCount( _mode ); ++j2 ) {
                            if( i2 == i && j2 == j ) { continue; }
                            auto p2 = getPkmn( i2, j2 );
                            if( p2 == nullptr ) { continue; }

                            if( p2->getAbility( ) == A_BAD_DREAMS
                                && !slotAbilitiesNullified( i2, j2 ) ) {
                                p_ui->logAbility( getPkmnOrDisguise( i2, j2 ), i2 );
                                damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                            }
                        }
                    }
                }

                if( hasStatusCondition( i, j, POISON ) ) {
                    p_ui->animateStatusCondition( getPkmnOrDisguise( i, j ), i, j, POISON );
                    if( supprA || ( volst & VS_GASTROACID )
                        || pkmn->getAbility( ) != A_POISON_HEAL ) {
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    } else if( !( volst & VS_HEALBLOCK ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                        healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    }
                }
                if( hasStatusCondition( i, j, TOXIC ) ) {
                    p_ui->animateStatusCondition( getPkmnOrDisguise( i, j ), i, j, TOXIC );
                    u8 turns = getAndIncreaseToxicCount( i, j );
                    if( turns > 15 ) { turns = 15; }
                    if( supprA || ( volst & VS_GASTROACID )
                        || pkmn->getAbility( ) != A_POISON_HEAL ) {
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP * turns / 16 );
                    } else if( !( volst & VS_HEALBLOCK ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                        healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    }
                }
                if( hasStatusCondition( i, j, BURN ) ) {
                    p_ui->animateStatusCondition( getPkmnOrDisguise( i, j ), i, j, BURN );
                    damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                }

                if( !getPkmn( i, j )->canBattle( ) ) {
                    // pkmn fainted
                    faintPokemon( p_ui, i, j );
                }
            }
        }

        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pkmn = getPkmn( i, j );
                if( pkmn == nullptr ) { continue; }
                if( !canUseItem( i, j ) ) { continue; }
                auto volst = getVolatileStatus( i, j );

                if( pkmn->getItem( ) == I_LEFTOVERS && !( volst & VS_HEALBLOCK ) ) {
                    healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                }
                if( pkmn->getItem( ) == I_BLACK_SLUDGE ) {
                    if( hasType( i, j, TYPE_POISON ) ) {
                        if( !( volst & VS_HEALBLOCK ) ) {
                            healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                        }
                    } else {
                        if( suppressesAbilities( ) || ( volst & VS_GASTROACID )
                            || pkmn->getAbility( ) != A_MAGIC_GUARD ) {
                            damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                        }
                    }
                }
                if( pkmn->getItem( ) == I_STICKY_BARB ) {
                    if( suppressesAbilities( ) || ( volst & VS_GASTROACID )
                        || pkmn->getAbility( ) != A_MAGIC_GUARD ) {
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    }
                }
                if( pkmn->getItem( ) == I_FLAME_ORB ) {
                    if( terrainGuardsStatus( fieldPosition( i, j ), true, false, false, false,
                                             false, false ) ) {
                    } else if( setStatusCondition( i, j, BURN ) ) {
                        p_ui->animateGetStatusCondition( getPkmnOrDisguise( i, j ), i, j, BURN );
                        p_ui->updatePkmnStats( i, j, getPkmnOrDisguise( i, j ), true );
                    }
                }
                if( pkmn->getItem( ) == I_TOXIC_ORB ) {
                    if( terrainGuardsStatus( fieldPosition( i, j ), true, false, false, false,
                                             false, false ) ) {
                    } else if( setStatusCondition( i, j, TOXIC ) ) {
                        p_ui->animateGetStatusCondition( getPkmnOrDisguise( i, j ), i, j, TOXIC );
                        p_ui->updatePkmnStats( i, j, getPkmnOrDisguise( i, j ), true );
                    }
                }
                if( !getPkmn( i, j )->canBattle( ) ) {
                    // pkmn fainted
                    faintPokemon( p_ui, i, j );
                }
            }
        }
        _mirrorHerbConsume( p_ui );
    }

} // namespace BATTLE
