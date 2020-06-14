/*
Pokémon neo
------------------------------

file        : battleField.cpp
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

#include <vector>
#include <algorithm>

#include "move.h"
#include "battle.h"
#include "defines.h"
#include "ability.h"
#include "abilityNames.h"
#include "itemNames.h"
#include "pokemonNames.h"
#include "saveGame.h"

namespace BATTLE {
    field::field( weather p_initialWeather, pseudoWeather p_initialPseudoWeather,
                  terrain p_initialTerrain ) {
        _weather = p_initialWeather;
        _weatherTimer = u8( -1 ); // Initial weather stays forever

        _pseudoWeather = p_initialPseudoWeather;
        std::memset( _pseudoWeatherTimer, 0, sizeof( _pseudoWeatherTimer ) );
        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( p_initialPseudoWeather & ( 1 << i ) ) [[unlikely]] {
                _pseudoWeatherTimer[ i ] = u8( -1 ); // Initial pseudo weather stays forever
            }
        }

        _terrain = p_initialTerrain;
        _terrainTimer = u8( -1 ); // Initial terrain stays forever
    }

    void field::checkOnSendOut( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return; }

        switch( pkmn->getAbility( ) ) {
            // abilities that cannot be suppressed
            case A_COMATOSE:
            case A_NEUTRALIZING_GAS:
                p_ui->logAbility( pkmn, p_opponent );
                break;
            case A_ZEN_MODE:
                if( pkmn->m_stats.m_curHP * 2 < pkmn->m_stats.m_maxHP ) {
                    if( pkmn->getSpecies( ) == PKMN_DARMANITAN ) {
                        p_ui->logAbility( pkmn, p_opponent );
                        pkmn->battleTransform( );
                        p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                    }
                }
                break;
            case A_SCHOOLING: {
                if( pkmn->m_level >= 20 && pkmn->m_stats.m_curHP * 4 > pkmn->m_stats.m_maxHP ) {
                    p_ui->logAbility( pkmn, p_opponent );
                    pkmn->battleTransform( );
                    p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                }
                break;
            }

            default:
            if( !suppressesAbilities( ) ) {
                switch( pkmn->getAbility( ) ) {
                    case A_ARENA_TRAP:
                    case A_SHADOW_TAG:
                    case A_MAGNET_PULL:
                    case A_CLOUD_NINE:
                    case A_AIR_LOCK:
                    case A_MOLD_BREAKER:
                    case A_TERAVOLT:
                    case A_TURBOBLAZE:
                    case A_PRESSURE:
                        p_ui->logAbility( pkmn, p_opponent );
                        break;

                    // Stat changing abilities
                    case A_DAUNTLESS_SHIELD: {
                        p_ui->logAbility( pkmn, p_opponent );
                        boosts bt = boosts( ); bt.setBoost( DEF, 1 );
                        auto res = addBoosts( p_opponent, p_slot, bt );
                        p_ui->logBoosts( pkmn, p_opponent, p_slot, bt, res );
                        break;
                    }
                    case A_INTREPID_SWORD: {
                        p_ui->logAbility( pkmn, p_opponent );
                        boosts bt = boosts( ); bt.setBoost( ATK, 1 );
                        auto res = addBoosts( p_opponent, p_slot, bt );
                        p_ui->logBoosts( pkmn, p_opponent, p_slot, bt, res );
                        break;
                    }
                    case A_INTIMIDATE: {
                        p_ui->logAbility( pkmn, p_opponent );
                        boosts bt = boosts( ); bt.setBoost( ATK, -1 );
                        for( u8 i = 0; i < 2; ++i ) {
                            if( getPkmn( !p_opponent, i ) == nullptr ) { continue; }
                            auto res = addBoosts( !p_opponent, i, bt );
                            p_ui->logBoosts( getPkmn( !p_opponent, i ), !p_opponent, i, bt, res );
                        }
                        break;
                    }
                    case A_DOWNLOAD: {
                        p_ui->logAbility( pkmn, p_opponent );
                        boosts bt = boosts( );

                        u16 def = 0, sdef = 0;
                        for( u8 i = 0; i < 2; ++i ) {
                            def += getStat( !p_opponent, i, DEF );
                            sdef += getStat( !p_opponent, i, SDEF );
                        }
                        bt.setBoost( def < sdef ? ATK : SATK, 1 );
                        auto res = addBoosts( p_opponent, p_slot, bt );
                        p_ui->logBoosts( pkmn, p_opponent, p_slot, bt, res );
                        break;
                    }

                    // Weather abilities
                    case A_DRIZZLE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, RAIN, pkmn->getItem( ) == I_DAMP_ROCK );
                        break;
                    case A_DROUGHT:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, SUN, pkmn->getItem( ) == I_HEAT_ROCK );
                        break;
                    case A_SAND_STREAM:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, SANDSTORM, pkmn->getItem( ) == I_SMOOTH_ROCK );
                        break;
                    case A_SNOW_WARNING:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, HAIL, pkmn->getItem( ) == I_ICY_ROCK );
                        break;
                    case A_PRIMORDIAL_SEA:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, HEAVY_RAIN );
                        break;
                    case A_DESOLATE_LAND:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, HEAVY_SUNSHINE );
                        break;
                    case A_DELTA_STREAM:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, HEAVY_WINDS );
                        break;

                    // Terrain abilities
                    case A_ELECTRIC_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, ELECTRICTERRAIN, pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;
                    case A_PSYCHIC_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, PSYCHICTERRAIN, pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;
                    case A_GRASSY_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, GRASSYTERRAIN, pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;
                    case A_MISTY_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, MISTYTERRAIN, pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;

                    // Form change abilities
                    case A_IMPOSTER:
                        if( transformPkmn( p_opponent, p_slot, getPkmn( !p_opponent, p_slot ) ) ) {
                            p_ui->logAbility( pkmn, p_opponent );
                            p_ui->updatePkmn( p_opponent, p_slot, getPkmn( p_opponent, p_slot ) );
                            if( pkmn->getAbility ( ) != A_IMPOSTER ) {
                                checkOnSendOut( p_ui, p_opponent, p_slot );
                            }
                        }
                        break;

                    case A_TRACE: {
                        p_ui->logAbility( pkmn, p_opponent );
                        auto tmp = getPkmn( !p_opponent, p_slot );
                        if( tmp != nullptr && allowsCopy( tmp->getAbility( ) ) &&
                                changeAbility( p_opponent, p_slot, tmp->getAbility( ) ) ) {
                            p_ui->logAbility( pkmn, p_opponent );

                            if( pkmn->getAbility ( ) != A_TRACE ) {
                                checkOnSendOut( p_ui, p_opponent, p_slot );
                            }
                        }
                        break;
                    }

                    case A_ANTICIPATION: {
                        p_ui->logAbility( pkmn, p_opponent );
                        bool warn = false;
                        for( u8 i = 0; !warn && i < 2; ++i ) {
                            auto tmp = getPkmn( !p_opponent, i );
                            if( tmp == nullptr ) { continue; }
                            for( u8 j = 0; j < 4; ++j ) {
                                if( tmp->getMove( j ) ) [[likely]] {
                                    auto mdata = MOVE::getMoveData( tmp->getMove( j ) );
                                    if( ( mdata.m_flags & MOVE::OHKO ) ) [[unlikely]] {
                                        warn = true;
                                        break;
                                    }
                                    u16 eff = 1;
                                    for( auto tp : getTypes( p_opponent, p_slot ) ) {
                                        eff *= getEffectiveness( mdata.m_type, tp );
                                    }
                                    if( eff > 1 ) {
                                        warn = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if( warn ) {
                            p_ui->logAnticipation( pkmn, p_opponent );
                        }
                        break;
                    }

                    case A_FOREWARN: {
                        p_ui->logAbility( pkmn, p_opponent );
                        std::vector<std::pair<u8, u16>> moves
                            = std::vector<std::pair<u8, u16>>( );
                        for( u8 i = 0; i < 2; ++i ) {
                            auto tmp = getPkmn( !p_opponent, i );
                            if( tmp == nullptr ) { continue; }
                            for( u8 j = 0; j < 4; ++j ) {
                                if( tmp->getMove( j ) ) {
                                    moves.push_back( std::pair<u8, u16>(
                                                255 - MOVE::getMoveData( tmp->getMove( j )
                                                    ).m_basePower,
                                                tmp->getMove( j )
                                                ) );
                                }
                            }
                        }
                        std::sort( moves.begin( ), moves.end( ) );
                        p_ui->logForewarn( pkmn, p_opponent, moves[ 0 ].second );
                        break;
                    }
                    case A_FRISK: {
                        std::vector<u16> itms = std::vector<u16>( );

                        for( u8 i = 0; i < 2; ++i ) {
                            auto tmp = getPkmn( !p_opponent, i );
                            if( tmp == nullptr ) { continue; }
                            if( tmp->getItem( ) ) {
                                itms.push_back( tmp->getItem( ) );
                            }
                        }
                        if( itms.size( ) ) {
                            p_ui->logAbility( pkmn, p_opponent );
                            p_ui->logFrisk( pkmn, p_opponent, itms );
                        }
                        break;
                    }

                    case A_SCREEN_CLEANER: {
                        p_ui->logAbility( pkmn, p_opponent );
                        for( u8 i = 0; i < 2; ++i ) {
                            _sides[ i ].removeSideCondition( p_ui,
                                    sideCondition( LIGHTSCREEN | REFLECT | AURORAVEIL ) );
                        }
                        break;
                    }

                    default:
                        break;
                }
                break;
            }
        }
    }

    void field::init( battleUI* p_ui ) {
        // TODO
    }

    void field::age( battleUI* p_ui ) {
        // TODO
    }

    bool field::setWeather( battleUI* p_ui, weather p_newWeather, bool p_extended ) {
        // TODO
        return false;
    }

    bool field::removeWeather( battleUI* p_ui ) {
        // TODO
        return false;
    }

    weather field::getWeather( ) {
        // TODO
        return weather( 0 );
    }

    bool field::setPseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather,
                                  bool p_extended ) {
        // TODO
        return false;
    }

    bool field::removePseudoWeather( battleUI* p_ui ) {
        // TODO
        return false;
    }

    pseudoWeather field::getPseudoWeather( ) {
        // TODO
        return pseudoWeather( 0 );
    }

    bool field::setTerrain( battleUI* p_ui, terrain p_newTerrain, bool p_extended ) {
        // TODO
        return false;
    }

    bool field::removeTerrain( battleUI* p_ui ) {
        // TODO
        return false;
    }

    terrain field::getTerrain( ) {
        // TODO
        return terrain( 0 );
    }

    bool field::addSideCondition( battleUI* p_ui, u8 p_side, sideCondition p_sideCondition,
                                  u8 p_duration ) {
        // TODO
        return false;
    }

    bool field::removeSideCondition( battleUI* p_ui, u8 p_side, sideCondition p_sideCondition ) {
        // TODO
        return false;
    }

    void field::megaEvolve( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        char buffer[ 100 ];

        snprintf( buffer, 99, GET_STRING( 307 ), p_ui->getPkmnName(
                    getPkmn( p_opponent, p_slot ), p_opponent ).c_str( ),
                    getPkmn( p_opponent, p_slot )->getItem( ) );
        p_ui->log( std::string( buffer ) );

        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        getPkmn( p_opponent, p_slot )->battleTransform( );
        p_ui->updatePkmn( p_opponent, p_slot, getPkmn( p_opponent, p_slot ) );

        snprintf( buffer, 99, GET_STRING( 308 ), p_ui->getPkmnName(
                    getPkmn( p_opponent, p_slot ), p_opponent ).c_str( ),
                    getPkmn( p_opponent, p_slot )->getItem( ) );
        p_ui->log( std::string( buffer ) );

        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

        checkOnSendOut( p_ui, p_opponent, p_slot );
    }

    std::vector<battleMove>
    field::computeSortedBattleMoves( const std::vector<battleMoveSelection>& p_selectedMoves ) {
        std::vector<battleMove> res = std::vector<battleMove>( );

        u8 pertub[ 4 ] = { 0, 1, 2, 3 };
        for( u8 i = 0; i < 4; ++i ) {
            std::swap( pertub[ i ], pertub[ rand( ) % 4 ] );
        }

        for( u8 j = 0; j < p_selectedMoves.size( ); ++j ) {
            battleMove bm;
            auto m = p_selectedMoves[ j ];

            bm.m_param = m.m_param;
            bm.m_user = m.m_user;
            bm.m_target = std::vector<fieldPosition>( );
            bm.m_pertubation = pertub[ j ];
            bm.m_moveData = m.m_moveData;
            bm.m_megaEvolve = m.m_megaEvolve;

            // compute targets, priority
            switch( m.m_type ) {
                case ATTACK:
                case SWITCH_PURSUIT: {
                    if( m.m_type == battleMoveType::SWITCH_PURSUIT ) {
                        // Double the power of pursuit
                        bm.m_priority = 123;
                        bm.m_moveData.m_basePower *= 2;
                    } else {
                        bm.m_priority = 3 * m.m_moveData.m_priority;
                        bool haspd = false;

                        // Check for priority changing abilities
                        if( !suppressesAbilities( ) ) [[likely]] {
                            // Gale wings
                            if( m.m_moveData.m_type == FLYING &&
                                    getPkmn( m.m_user.first, m.m_user.second )->getStat( HP )
                                        == getPkmn( m.m_user.first, m.m_user.second )->
                                        m_stats.m_maxHP &&
                                    getPkmn( m.m_user.first, m.m_user.second )->getAbility( )
                                    == A_GALE_WINGS ) [[unlikely]] {
                                bm.m_priority += 3;
                            }

                            // Prankster
                            if( m.m_moveData.m_category == MOVE::STATUS
                                    && getPkmn( m.m_user.first, m.m_user.second )->getAbility( )
                                    == A_PRANKSTER ) [[unlikely]] {
                                bm.m_priority += 3;
                            }

                            // Triage
                            if( m.m_moveData.m_heal
                                    && getPkmn( m.m_user.first, m.m_user.second )->getAbility( )
                                    == A_TRIAGE ) [[unlikely]] {
                                bm.m_priority += 9;
                            }

                            // Stall
                            if( getPkmn( m.m_user.first, m.m_user.second )->getAbility( )
                                    == A_STALL ) {
                                bm.m_priority--;
                                haspd = true;
                            }
                        }

                        // Items

                        if( !haspd && getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                == I_LAGGING_TAIL ) [[unlikely]] {
                            bm.m_priority--;
                        }

                        if( !haspd && getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                == I_FULL_INCENSE ) [[unlikely]] {
                            bm.m_priority--;
                        }

                        if( getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                == I_QUICK_CLAW ) [[unlikely]] {
                            if( rand( ) % 100 < 20 ) {
                                bm.m_priority++;
                                res.push_back( { MESSAGE_ITEM, I_QUICK_CLAW, { },
                                    { 255, 255 }, 127, 0, u8( 5 + j ), MOVE::moveData( ), false } );
                            }
                        }
                        if( getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                == I_CUSTAP_BERRY &&
                                getPkmn( m.m_user.first, m.m_user.second )->getStat( HP )
                                    * 4 < getPkmn( m.m_user.first, m.m_user.second )->
                                        m_stats.m_maxHP ) [[unlikely]] {
                            bm.m_priority++;
                            res.push_back( { MESSAGE_ITEM, I_CUSTAP_BERRY, { },
                                    { 255, 255 }, 127, 0, u8( 5 + j ), MOVE::moveData( ), false } );
                            removeItem( m.m_user.first, m.m_user.second );
                        }

                        // moves w/ changing message

                        if( bm.m_param == M_FOCUS_PUNCH
                                || bm.m_param == M_BEAK_BLAST
                                || bm.m_param == M_SHELL_TRAP ) {
                            res.push_back( { MESSAGE_MOVE, bm.m_param, { },
                                    { 255, 255 }, 125, 0, u8( 5 + j ), MOVE::moveData( ), false } );
                        }
                    }
                    bm.m_type = ATTACK;

                    auto tg = ( m.m_moveData.m_pressureTarget != MOVE::NO_TARGET )
                        ? m.m_moveData.m_pressureTarget : m.m_moveData.m_target;
                    switch( tg ) {
                        case MOVE::ANY:
                        case MOVE::RANDOM:
                        case MOVE::ALLY_OR_SELF:
                        case MOVE::ANY_FOE:
                            // target is already computed
                            bm.m_target.push_back( m.m_target );
                            break;
                        case MOVE::ALL_FOES_AND_ALLY:
                            if( getPkmn( m.m_user.first, !m.m_user.second ) ) {
                                bm.m_target.push_back( fieldPosition( m.m_user.first,
                                            !m.m_user.second ) );
                            }
                            [[fallthrough]];
                        case MOVE::ALL_FOES:
                            for( u8 i = 0; i < 2; ++i ) {
                                if( getPkmn( !m.m_user.first, i ) != nullptr ) {
                                    bm.m_target.push_back( fieldPosition( !m.m_user.first, i ) );
                                }
                            }
                            break;
                        case MOVE::ALL_ALLIES:
                            for( u8 i = 0; i < 2; ++i ) {
                                if( getPkmn( m.m_user.first, i ) != nullptr ) {
                                    bm.m_target.push_back( fieldPosition( m.m_user.first, i ) );
                                }
                            }
                            break;
                        case MOVE::SELF:
                            bm.m_target.push_back( fieldPosition( m.m_user.first,
                                                   m.m_user.second ) );
                            break;
                        case MOVE::ALLY:
                            bm.m_target.push_back( fieldPosition( m.m_user.first,
                                                   !m.m_user.second ) );
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case SWITCH:
                case USE_ITEM:
                    bm.m_type = m.m_type;
                    bm.m_target.push_back( m.m_user );
                    bm.m_priority = 120;
                    break;
                default:
                    bm.m_type = NO_OP;
                    bm.m_target.push_back( m.m_user );
                    bm.m_priority = -120;
                    break;
            }

            if( bm.m_priority > -100 ) [[likely]] {
                if( _pseudoWeather & TRICKROOM ) {
                    bm.m_userSpeed = -getStat( m.m_user.first, m.m_user.second, SPEED );
                } else {
                    bm.m_userSpeed = getStat( m.m_user.first, m.m_user.second, SPEED );
                }
            } else {
                bm.m_userSpeed = 0;
            }
            res.push_back( bm );
        }

        std::sort( res.begin( ), res.end( ) );

        return res;
    }

    void field::executeBattleMove( battleUI* p_ui, battleMove p_move,
                                   std::vector<battleMove> p_targetsMoves,
                                   std::vector<battleMove> p_tergetedMoves ) {
        p_ui->log(
                std::string( getPkmn( p_move.m_user.first, p_move.m_user.second )->m_boxdata.m_name ) + " used " +
                MOVE::getMoveName( p_move.m_param ) );

        for( u8 i = 0; i < 60; ++i ) {
            swiWaitForVBlank( );
        }

        // TODO
    }

} // namespace BATTLE
