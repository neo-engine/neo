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
#include "battleUI.h"
#include "battleSlot.h"
#include "battleSide.h"
#include "battleField.h"
#include "defines.h"
#include "ability.h"
#include "abilityNames.h"
#include "itemNames.h"
#include "pokemonNames.h"
#include "saveGame.h"

#include "type.h"

namespace BATTLE {
    field::field( weather p_initialWeather, pseudoWeather p_initialPseudoWeather,
                  terrain p_initialTerrain ) {
        _weather = p_initialWeather;
        _weatherTimer = u8( -1 ); // Initial weather stays forever

        std::memset( _pseudoWeatherTimer, 0, sizeof( _pseudoWeatherTimer ) );
        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( p_initialPseudoWeather & ( 1 << i ) ) [[unlikely]] {
                _pseudoWeatherTimer[ i ] = u8( -1 ); // Initial pseudo weather stays forever
            }
        }

        _terrain = p_initialTerrain;
        _terrainTimer = u8( -1 ); // Initial terrain stays forever
    }

    void field::checkOnEatBerry( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_berry ) {
        (void) p_berry;

        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return; }
        if( !suppressesAbilities( ) ) {
            switch( pkmn->getAbility( ) ) {
                [[unlikely]] case A_CHEEK_POUCH:
                    p_ui->logAbility( pkmn, p_opponent );
                    healPokemon( p_ui, p_opponent, p_slot, pkmn->m_stats.m_maxHP / 3 );
                    break;
                [[likely]] default:
                    break;
            }
        }
    }

    void field::checkOnSendOut( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return; }

        bool items = canUseItem( p_opponent, p_slot );

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
                    case A_UNNERVE:
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
                        setWeather( p_ui, RAIN, items && pkmn->getItem( ) == I_DAMP_ROCK );
                        break;
                    case A_DROUGHT:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, SUN, items && pkmn->getItem( ) == I_HEAT_ROCK );
                        break;
                    case A_SAND_STREAM:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, SANDSTORM, items && pkmn->getItem( ) == I_SMOOTH_ROCK );
                        break;
                    case A_SNOW_WARNING:
                        p_ui->logAbility( pkmn, p_opponent );
                        setWeather( p_ui, HAIL, items && pkmn->getItem( ) == I_ICY_ROCK );
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
                        setTerrain( p_ui, ELECTRICTERRAIN, items
                                && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;
                    case A_PSYCHIC_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, PSYCHICTERRAIN, items
                                && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;
                    case A_GRASSY_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, GRASSYTERRAIN, items
                                && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;
                    case A_MISTY_SURGE:
                        p_ui->logAbility( pkmn, p_opponent );
                        setTerrain( p_ui, MISTYTERRAIN, items
                                && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                        break;

                    // Form change abilities
                    case A_IMPOSTER:
                        if( transformPkmn( p_opponent, p_slot, getSlot( !p_opponent, p_slot ) ) ) {
                            p_ui->logAbility( pkmn, p_opponent );
                            p_ui->updatePkmn( p_opponent, p_slot, getPkmn( p_opponent, p_slot ) );
                            if( getPkmn( p_opponent, p_slot )->getAbility ( ) != A_IMPOSTER ) {
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

                            if( getPkmn( p_opponent, p_slot )->getAbility ( ) != A_TRACE ) {
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
                                        eff *= getTypeEffectiveness( mdata.m_type, tp );
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
                            _sides[ i ].removeSideCondition(
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
        (void) p_ui;
        // TODO
    }

    void field::age( battleUI* p_ui ) {
        (void) p_ui;
        // TODO
    }

    bool field::setWeather( battleUI* p_ui, weather p_newWeather, bool p_extended ) {
        // TODO: add proper log

        p_ui->log( "[setWeather]: " + std::to_string( u8( p_newWeather ) ) );

        if( p_newWeather == _weather ) {
            p_ui->log( GET_STRING( 304 ) );
            return false;
        }

        if( _weather == HEAVY_RAIN || _weather == HEAVY_SUNSHINE
                || _weather == HEAVY_WINDS ) [[unlikely]] {
            // weather can be replaced only with a similar weather
            if( p_newWeather != HEAVY_RAIN && p_newWeather != HEAVY_SUNSHINE
                    && p_newWeather != HEAVY_WINDS ) {
                p_ui->log( GET_STRING( 304 ) );
                return false;
            }
        }

        _weather = p_newWeather;
        _weatherTimer = p_extended ? EXTENDED_DURATION : NORMAL_DURATION;

        return true;
    }

    bool field::removeWeather( battleUI* p_ui ) {
        // TODO: proper log

        p_ui->log( "Weather removed" );

        _weather = NO_WEATHER;
        _weatherTimer = 0;
        return true;
    }

    bool field::setPseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather,
                                  bool p_extended ) {

        // TODO proper log
        // TODO can this return false?

        p_ui->log( "Set pseudoWeather " + std::to_string( u8( p_newPseudoWeather ) ) );

        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( p_newPseudoWeather & ( 1 << i ) ) {
                _pseudoWeatherTimer[ i ] = p_extended ? EXTENDED_DURATION : NORMAL_DURATION;
            }
        }
        return true;
    }

    bool field::removePseudoWeather( battleUI* p_ui, pseudoWeather p_pseudoWeather ) {
        // TODO proper log
        p_ui->log( "Pseudo Weather removed" );
        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( p_pseudoWeather & ( 1 << i ) ) {
                _pseudoWeatherTimer[ i ] = 0;
            }
        }
        return false;
    }

    bool field::setTerrain( battleUI* p_ui, terrain p_newTerrain, bool p_extended ) {
        // TODO: proper log
        p_ui->log( "Set terrain " + std::to_string( u8( p_newTerrain ) ) );

        _terrain = p_newTerrain;
        _terrainTimer = p_extended ? EXTENDED_DURATION : NORMAL_DURATION;
        return true;
    }

    bool field::removeTerrain( battleUI* p_ui ) {
        // TODO proper log
        p_ui->log( "terrain removed" );
        _terrain = NO_TERRAIN;
        _terrainTimer = 0;
        return true;
    }

    void field::megaEvolve( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        char buffer[ 100 ];

        snprintf( buffer, 99, GET_STRING( 307 ), p_ui->getPkmnName(
                    getPkmn( p_opponent, p_slot ), p_opponent ).c_str( ),
                    ITEM::getItemName( getPkmn( p_opponent, p_slot )->getItem( ) ).c_str( ) );
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
    field::computeSortedBattleMoves( battleUI* p_ui,
            const std::vector<battleMoveSelection>& p_selectedMoves ) {
        std::vector<battleMove> res = std::vector<battleMove>( );

        u8 pertub[ 4 ] = { 0, 1, 2, 3 };
        for( u8 i = 0; i < 4; ++i ) {
            std::swap( pertub[ i ], pertub[ rand( ) % 4 ] );
        }

        // TODO: add redirecting from follow me / abilities / etc

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

                        if( canUseItem( m.m_user.first, m.m_user.second ) ) {
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
                                removeItem( p_ui, m.m_user.first, m.m_user.second );
                                checkOnEatBerry( p_ui, m.m_user.first, m.m_user.second,
                                                 I_CUSTAP_BERRY );
                            }
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
                if( getPseudoWeather( ) & TRICKROOM ) {
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

    void field::confusionSelfDamage( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        // TODO
        (void) p_ui;
        (void) p_opponent;
        (void) p_slot;
    }

    void field::executeStatusEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        // TODO

        (void) p_ui;
        (void) p_move;
        (void) p_target;
    }

    void field::executeSecondaryStatus( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        // TODO

        (void) p_ui;
        (void) p_move;
        (void) p_target;

    }

    void field::executeSecondaryEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        // TODO

        (void) p_ui;
        (void) p_move;
        (void) p_target;

    }

    void field::executeContact( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        // TODO

        (void) p_ui;
        (void) p_move;
        (void) p_target;

    }


    bool field::useMove( battleUI* p_ui, battleMove p_move ) {
        char buffer[ 100 ];
        bool opponent = p_move.m_user.first;
        u8 slot = p_move.m_user.second;

        auto volst = getVolatileStatus( opponent, slot );

        if( volst & RECHARGE ) [[unlikely]] {
            snprintf( buffer, 99, GET_STRING( 276 ), p_ui->getPkmnName(
                        getPkmn( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );

            removeVolatileStatus( opponent, slot, RECHARGE );
            removeLockedMove( opponent, slot );
            return false;
        }

        if( volst & FLINCH ) [[unlikely]] {
            snprintf( buffer, 99, GET_STRING( 296 ), p_ui->getPkmnName(
                        getPkmn( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );
            return false;
        }

        if( hasStatusCondition( opponent, slot, FROZEN ) ) [[unlikely]] {
            if( p_move.m_moveData.m_type == FIRE
                    || ( p_move.m_moveData.m_flags & MOVE::DEFROST )
                    || ( rand( ) % 100 < 20 ) ) {
                // user thaws
                snprintf( buffer, 99, GET_STRING( 298 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                removeStatusCondition( opponent, slot );
                p_ui->updatePkmnStats( opponent, slot, getPkmn( opponent, slot ) );
            } else {
                p_ui->animateStatusCondition( opponent, slot, FROZEN );
                snprintf( buffer, 99, GET_STRING( 297 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                return false;
            }
        }

        if( u8 slp = hasStatusCondition( opponent, slot, SLEEP ); slp ) {
            if( --slp ) {
                // pkmn continues to sleep
                p_ui->animateStatusCondition( opponent, slot, SLEEP );
                snprintf( buffer, 99, GET_STRING( 299 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                setStatusCondition( opponent, slot, SLEEP, slp );

                if( !( p_move.m_moveData.m_flags & MOVE::SLEEPUSABLE ) ) {
                    return false;
                }
            } else {
                removeStatusCondition( opponent, slot );
                snprintf( buffer, 99, GET_STRING( 300 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                p_ui->updatePkmnStats( opponent, slot, getPkmn( opponent, slot ) );
            }
        } else if( p_move.m_moveData.m_flags & MOVE::SLEEPUSABLE ) {
            if( getPkmn( opponent, slot )->getAbility( ) != A_COMATOSE ) {
                snprintf( buffer, 99, GET_STRING( 10 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ),
                        MOVE::getMoveName( p_move.m_param ).c_str( ) );
                p_ui->log( buffer );

                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }

                p_ui->log( GET_STRING( 304 ) );
                return false;
            }
        }

        if( hasStatusCondition( opponent, slot, PARALYSIS ) ) {
            if( rand( ) % 100 < 25 ) {
                p_ui->animateStatusCondition( opponent, slot, PARALYSIS );
                snprintf( buffer, 99, GET_STRING( 301 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                return false;
            }
        }

        if( volst & CONFUSION ) [[unlikely]] {
            u8 curVal = getVolatileStatusCounter( opponent, slot, CONFUSION );
            if( curVal > 0 ) {
                --curVal;
                p_ui->animateVolatileStatusCondition( opponent, slot, CONFUSION );
                snprintf( buffer, 99, GET_STRING( 293 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                addVolatileStatus( opponent, slot, CONFUSION, curVal );

                if( rand( ) % 300 < 100 ) {
                    confusionSelfDamage( p_ui, opponent, slot );
                    p_ui->log( GET_STRING( 295 ) );
                    return false;
                }
            } else {
                snprintf( buffer, 99, GET_STRING( 294 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                removeVolatileStatus( opponent, slot, CONFUSION );
            }
        }

        if( ( volst & HEALBLOCK ) && ( p_move.m_moveData.m_flags & MOVE::HEAL ) ) [[unlikely]] {
            return false;
        }

        if( _pseudoWeatherTimer[ 4 ] && ( p_move.m_moveData.m_flags & MOVE::GRAVITY ) )
            [[unlikely]] {
            return false;
        }

        if( volst & ATTRACT ) [[unlikely]] {
            if( rand( ) % 100 < 50 ) {
                snprintf( buffer, 99, GET_STRING( 302 ), p_ui->getPkmnName(
                            getPkmn( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                return false;
            }
        }

        return true;
    }

    bool field::moveMisses( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                            bool p_critical ) {
        // TODO



        (void) p_ui;
        (void) p_move;
        (void) p_target;
        (void) p_critical;

        return false;
    }

    bool field::executeCriticalCheck( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        char buffer[ 100 ];
        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return false; }

        bool opponent = p_move.m_user.first;
        u8 slot = p_move.m_user.second;

        bool supprAbs = suppressesAbilities( );
        auto userVolStat = getVolatileStatus( p_move.m_user.first, p_move.m_user.second );

        // Check for crit suppr.

        if( !supprAbs && (
                    target->getAbility( ) == A_BATTLE_ARMOR ||
                    target->getAbility( ) == A_SHELL_ARMOR ) ) {
            return false;
        }

        if( getSideCondition( !p_target.first ) & LUCKYCHANT ) {
            return false;
        }

        if( userVolStat & LASERFOCUS ) {
            return true;
        }

        // Calculate critical hit

        bool critical = false;

        u8 critLevel = p_move.m_moveData.m_critRatio;

        bool item = canUseItem( p_move.m_user.first, p_move.m_user.second );

        if( item ) {
            if( user->getItem( ) == I_RAZOR_CLAW
                    || user->getItem( ) == I_SCOPE_LENS ) [[unlikely]] {
                critLevel++;
            } else if( user->getSpecies( ) == PKMN_FARFETCH_D ||
                    user->getSpecies( ) == PKMN_SIRFETCH_D ) [[unlikely]] {
                if( user->getItem( ) == I_LEEK ) [[unlikely]] {
                    critLevel += 2;
                }
            } else if( user->getSpecies( ) == PKMN_CHANSEY ||
                    user->getSpecies( ) == PKMN_BLISSEY ||
                    user->getSpecies( ) == PKMN_HAPPINY ) [[unlikely]] {
                if( user->getItem( ) == I_LUCKY_PUNCH ) [[unlikely]] {
                    critLevel += 2;
                }
            }
        }

        if( !supprAbs && user->getAbility( ) == A_SUPER_LUCK ) {
            critLevel++;
        }

        if( item && user->getItem( ) == I_LANSAT_BERRY ) [[unlikely]] {
            u8 factor = 4;
            if( !supprAbs && user->getAbility( ) == A_GLUTTONY ) {
                factor = 2;
            }

            if( user->m_stats.m_curHP * factor < user->m_stats.m_maxHP ) {
                if( supprAbs ||
                        !_sides[ opponent ? PLAYER_SIDE : OPPONENT_SIDE ].anyHasAbility(
                            A_UNNERVE ) ) [[likely]] {

                    snprintf( buffer, 99, GET_STRING( 279 ), p_ui->getPkmnName(
                                getPkmn( opponent, slot ), opponent ).c_str( ),
                            ITEM::getItemName( I_LANSAT_BERRY ).c_str( ) );
                    p_ui->log( buffer );

                    critLevel += 3;

                    removeItem( p_ui, opponent, slot );
                    checkOnEatBerry( p_ui, opponent, slot, I_LANSAT_BERRY );
                }
            }
        } else if( userVolStat & FOCUSENERGY ) {
            critLevel += 2;
        }

        if( critLevel >= 3 ) [[unlikely]] { critical = true; }
        else if( critLevel == 2 ) [[unlikely]] { critical = !( rand( ) % 2 ); }
        else if( critLevel == 1 ) { critical = !( rand( ) % 8 ); }
        else [[likely]] { critical = !( rand( ) % 24 ); }

        return critical;
    }

    u16 field::getMovePower( battleMove p_move ) {
        u16 basepower = p_move.m_moveData.m_basePower;
        type moveType = p_move.m_moveData.m_type;

        if( p_move.m_param == M_HIDDEN_POWER ) {
            basepower = getPkmn( p_move.m_user.first, p_move.m_user.second )->getHPPower( );
            moveType = getPkmn( p_move.m_user.first, p_move.m_user.second )->getHPType( );
        }

        if( !suppressesAbilities( ) ) {
            switch( getPkmn( p_move.m_user.first, p_move.m_user.second )->getAbility( ) ) {
                case A_PIXILATE:
                case A_AERILATE:
                case A_REFRIGERATE:
                case A_GALVANIZE:
                    if( moveType == NORMAL ) { basepower = ( basepower * 120 ) / 100; }
                    break;
                [[likely]] default:
                    break;
            }
        }

        if( p_move.m_param == M_WEATHER_BALL ) {
            switch( getWeather( ) ) {
                case NO_WEATHER:
                    basepower = 50; break;
                default:
                    basepower = 100; break;
            }
        }

        if( p_move.m_param == M_TERRAIN_PULSE ) {
            switch( getTerrain( ) ) {
                case NO_TERRAIN:
                    basepower = 50; break;
                default:
                    basepower = 100; break;
            }
        }

        if( p_move.m_param == M_FACADE ) {
            if( getPkmn( p_move.m_user.first, p_move.m_user.second )->m_statusint ) {
                basepower *= 2;
            }
        }

        return basepower;
    }

    type field::getMoveType( battleMove p_move ) {
        type moveType = p_move.m_moveData.m_type;

        if( p_move.m_param == M_STRUGGLE ) { return NORMAL; }

        if( p_move.m_param == M_HIDDEN_POWER ) [[unlikely]] {
            moveType = getPkmn( p_move.m_user.first, p_move.m_user.second )->getHPType( );
        }

        if( p_move.m_param == M_AURA_WHEEL ) [[unlikely]] {
            if( getPkmn( p_move.m_user.first, p_move.m_user.second )->getForme( ) == 1 ) {
                moveType = DARK;
            } else {
                moveType = ELECTRIC;
            }
        }

        if( !suppressesAbilities( ) ) {
            switch( getPkmn( p_move.m_user.first, p_move.m_user.second )->getAbility( ) ) {
                case A_NORMALIZE:
                    moveType = NORMAL;
                    break;
                case A_PIXILATE:
                    if( moveType == NORMAL ) { return FAIRY; }
                    break;
                case A_AERILATE:
                    if( moveType == NORMAL ) { return FLYING; }
                    break;
                case A_GALVANIZE:
                    if( moveType == NORMAL ) { return ELECTRIC; }
                    break;
                case A_REFRIGERATE:
                    if( moveType == NORMAL ) { return ICE; }
                    break;
                case A_LIQUID_VOICE:
                    if( p_move.m_moveData.m_flags & MOVE::SOUND ) {
                        return WATER;
                    }
                    break;
                [[likely]] default:
                    break;
            }
        }

        if( p_move.m_param == M_JUDGMENT && canUseItem( p_move.m_user.first,
                    p_move.m_user.second ) ) [[unlikely]] {
            switch( getPkmn( p_move.m_user.first, p_move.m_user.second )->getItem( ) ) {
                case I_FLAME_PLATE:
                    moveType = FIRE;
                    break;
                case I_SPLASH_PLATE:
                    moveType = WATER;
                    break;
                case I_ZAP_PLATE:
                    moveType = ELECTRIC;
                    break;
                case I_MEADOW_PLATE:
                    moveType = GRASS;
                    break;
                case I_ICICLE_PLATE:
                    moveType = ICE;
                    break;
                case I_FIST_PLATE:
                    moveType = FIGHT;
                    break;
                case I_TOXIC_PLATE:
                    moveType = type::POISON;
                    break;
                case I_EARTH_PLATE:
                    moveType = GROUND;
                    break;
                case I_SKY_PLATE:
                    moveType = FLYING;
                    break;
                case I_MIND_PLATE:
                    moveType = PSYCHIC;
                    break;
                case I_INSECT_PLATE:
                    moveType = BUG;
                    break;
                case I_STONE_PLATE:
                    moveType = ROCK;
                    break;
                case I_SPOOKY_PLATE:
                    moveType = GHOST;
                    break;
                case I_DRACO_PLATE:
                    moveType = DRAGON;
                    break;
                case I_DREAD_PLATE:
                    moveType = DARK;
                    break;
                case I_IRON_PLATE:
                    moveType = STEEL;
                    break;
                case I_PIXIE_PLATE:
                    moveType = FAIRY;
                    break;
                case I_NULL_PLATE:
                    moveType = UNKNOWN;
                    break;
                default:
                    moveType = NORMAL;
                    break;
            }
        }

        if( p_move.m_param == M_MULTI_ATTACK && canUseItem( p_move.m_user.first,
                    p_move.m_user.second ) ) [[unlikely]] {
            switch( getPkmn( p_move.m_user.first, p_move.m_user.second )->getItem( ) ) {
                case I_FIGHTING_MEMORY:
                    moveType = FIGHT;
                    break;
                case I_FLYING_MEMORY:
                    moveType = FLYING;
                    break;
                case I_POISON_MEMORY:
                    moveType = type::POISON;
                    break;
                case I_GROUND_MEMORY:
                    moveType = GROUND;
                    break;
                case I_ROCK_MEMORY:
                    moveType = ROCK;
                    break;
                case I_BUG_MEMORY:
                    moveType = BUG;
                    break;
                case I_GHOST_MEMORY:
                    moveType = GHOST;
                    break;
                case I_STEEL_MEMORY:
                    moveType = STEEL;
                    break;
                case I_FIRE_MEMORY:
                    moveType = FIRE;
                    break;
                case I_WATER_MEMORY:
                    moveType = WATER;
                    break;
                case I_GRASS_MEMORY:
                    moveType = GRASS;
                    break;
                case I_ELECTRIC_MEMORY:
                    moveType = ELECTRIC;
                    break;
                case I_PSYCHIC_MEMORY:
                    moveType = PSYCHIC;
                    break;
                case I_ICE_MEMORY:
                    moveType = ICE;
                    break;
                case I_DRAGON_MEMORY:
                    moveType = DRAGON;
                    break;
                case I_DARK_MEMORY:
                    moveType = DARK;
                    break;
                case I_FAIRY_MEMORY:
                    moveType = FAIRY;
                    break;
                default:
                    moveType = NORMAL;
                    break;
            }
        }

        if( p_move.m_param == M_TECHNO_BLAST && canUseItem( p_move.m_user.first,
                    p_move.m_user.second ) ) [[unlikely]] {
            switch( getPkmn( p_move.m_user.first, p_move.m_user.second )->getItem( ) ) {
                case I_BURN_DRIVE:
                    moveType = FIRE; break;
                case I_DOUSE_DRIVE:
                    moveType = WATER; break;
                case I_CHILL_DRIVE:
                    moveType = ICE; break;
                case I_SHOCK_DRIVE:
                    moveType = ELECTRIC; break;
                default:
                    moveType = NORMAL;
                    break;
            }
        }

        if( p_move.m_param == M_REVELATION_DANCE ) {
            auto types = getTypes( p_move.m_user.first, p_move.m_user.second );
            if( types.size( ) >= 1 ) {
                moveType = types[ 0 ];
            } else {
                moveType = NORMAL;
            }
        }

        if( p_move.m_param == M_WEATHER_BALL ) {
            switch( getWeather( ) ) {
                case RAIN:
                case HEAVY_RAIN:
                    moveType = WATER;
                    break;
                case SUN:
                case HEAVY_SUNSHINE:
                    moveType = FIRE;
                    break;
                case HAIL:
                    moveType = ICE;
                    break;
                case SANDSTORM:
                    moveType = ROCK;
                    break;
                case FOG:
                    moveType = FLYING;
                    break;
                default:
                    moveType = NORMAL;
                    break;
            }
        }

        if( p_move.m_param == M_TERRAIN_PULSE ) {
            switch( getTerrain( ) ) {
                case PSYCHICTERRAIN:
                    moveType = PSYCHIC;
                    break;
                case ELECTRICTERRAIN:
                    moveType = ELECTRIC;
                    break;
                case MISTYTERRAIN:
                    moveType = FAIRY;
                    break;
                case GRASSYTERRAIN:
                    moveType = GRASS;
                    break;
                default:
                    moveType = NORMAL;
                    break;
            }
        }

        if( getVolatileStatus( p_move.m_user.first, p_move.m_user.second )
                & ELECTRIFY ) {
            moveType = ELECTRIC;
        }

        if( ( getPseudoWeather( ) & IONDELUGE ) && moveType == NORMAL ) {
            moveType = ELECTRIC;
        }

        return moveType;
    }

    u16 field::getEffectiveness( battleMove p_move, fieldPosition p_target ) {
        u16 res = 100;

        auto target = getPkmn( p_target.first, p_target.second );
        if( target == nullptr ) [[unlikely]] { return 0; }

        bool items = canUseItem( p_target.first, p_target.second );

        type moveType = getMoveType( p_move );

        // Heavy weather

        if( getWeather( ) == HEAVY_SUNSHINE && moveType == WATER ) { return 0; }
        if( getWeather( ) == HEAVY_RAIN && moveType == FIRE ) { return 0; }

        if( ( p_move.m_moveData.m_flags & MOVE::SOUND )
                && ( getPkmn( p_target.first, p_target.second )->getAbility( ) == A_SOUNDPROOF
                || getPkmn( p_target.first, p_target.second )->getAbility( ) == A_CACOPHONY )
                && !suppressesAbilities( ) ) [[unlikely]] {
            return 0;
        }

        // Iron ball
        if( moveType == GROUND && items && target->getItem( ) == I_IRON_BALL ) [[unlikely]] {
            return res;
        }

        // Thousand arrows
        if( p_move.m_param == M_THOUSAND_ARROWS ) [[unlikely]] {
            if( !isGrounded( p_target.first, p_target.second )
                    && hasType( p_target.first, p_target.second, FLYING ) ) {
                return res;
            }
        }

        for( type t : getTypes( p_target.first, p_target.second ) ) {
            u16 curval = getTypeEffectiveness( moveType, t );

            if( !isGrounded( p_target.first, p_target.second, p_move.m_param != M_SUNSTEEL_STRIKE )
                    && moveType == GROUND ) [[unlikely]] {
                curval = 0;
            }

            if( p_move.m_param == M_FLYING_PRESS ) {
                curval = ( curval * getTypeEffectiveness( FLYING, t ) ) / 100;
            }

            if( t == GHOST && ( moveType == NORMAL || moveType == FIGHT ) ) [[unlikely]] {
                if( !suppressesAbilities( )
                        && getPkmn( p_move.m_user.first, p_move.m_user.second )->getAbility( )
                        == A_SCRAPPY ) {
                    continue;
                }
                if( getVolatileStatus( p_target.first, p_target.second ) & FORESIGHT ) [[unlikely]] {
                    continue;
                }
            }

            if( t == DARK && moveType == PSYCHIC
                    && ( getVolatileStatus( p_target.first, p_target.second ) & MIRACLEEYE ) ) [[unlikely]] {
                continue;
            }

            if( t == WATER && p_move.m_param == M_FREEZE_DRY ) {
                curval = 200;
            }

            if( t == FLYING ) {
                if( !suppressesWeather( ) && _weather == HEAVY_WINDS ) [[unlikely]] {
                    continue;
                }
            }

            if( !curval && items && target->getItem( ) == I_RING_TARGET ) [[unlikely]] {
                continue;
            }

            res = ( res * curval / 100 );
        }

        return res;
    }

    bool field::executeDamagingMove( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                     bool p_critical, u8 p_damageModifier ) {
        char buffer[ 100 ];

        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return false; }

        bool supprAbs = suppressesAbilities( );
        // auto userVolStat = getVolatileStatus( p_move.m_user.first, p_move.m_user.second );

        u16 effectiveness = getEffectiveness( p_move, p_target );

        if( effectiveness == 0 ) {
            snprintf( buffer, 99, GET_STRING( 284 ),
                      p_ui->getPkmnName( target, p_target.first ).c_str( ) );
            p_ui->log( buffer );
            return false;
        }

        // Calculate damage
        u32 damage = 0;

        if( p_move.m_moveData.m_fixedDamage ) {
            damage = p_move.m_moveData.m_fixedDamage;
        } else {
            u16 atk =
                getStat( p_move.m_user.first, p_move.m_user.second,
                        p_move.m_moveData.m_category == MOVE::SPECIAL ? SATK : ATK, !supprAbs
                        && p_move.m_param != M_SUNSTEEL_STRIKE, p_critical );
            u16 def =
                getStat( p_target.first, p_target.second,
                        p_move.m_moveData.m_defensiveCategory == MOVE::SPECIAL ? SDEF : DEF,
                        !supprAbs && p_move.m_param != M_SUNSTEEL_STRIKE, false, p_critical );
            if( p_move.m_moveData.m_flags & MOVE::DEFASOFF ) {
                atk =
                    getStat( p_move.m_user.first, p_move.m_user.second,
                            p_move.m_moveData.m_category == MOVE::SPECIAL ? SDEF : DEF, !supprAbs
                            && p_move.m_param != M_SUNSTEEL_STRIKE, p_critical );
            }

            damage = ( user->m_level * 2 ) / 5 + 2;

            // base power modifying stuff
            u16 movePower = getMovePower( p_move );

            // speed-based
            if( p_move.m_param == M_GYRO_BALL ) [[unlikely]] {
                auto atkspd = getStat( p_move.m_user.first, p_move.m_user.second, SPEED );
                auto defspd = getStat( p_target.first, p_move.m_user.second, SPEED );

                movePower = atkspd ? 25 * defspd / atkspd : 150;
                if( movePower > 150 ) { movePower = 150; }
                if( movePower == 0 ) { movePower = 1; }
            }

            if( p_move.m_param == M_ELECTRO_BALL ) [[unlikely]] {
                auto atkspd = getStat( p_move.m_user.first, p_move.m_user.second, SPEED );
                auto defspd = getStat( p_target.first, p_move.m_user.second, SPEED );

                if( !defspd ) { movePower = 150; }
                else {
                    switch( atkspd / defspd ) {
                        case 0:
                            movePower = 40;
                            break;
                        case 1:
                            movePower = 60;
                            break;
                        case 2:
                            movePower = 80;
                            break;
                        case 3:
                            movePower = 120;
                            break;
                        default:
                            movePower = 150;
                            break;
                    }
                }
            }

            // TODO: weight-based stuff

            // HP-based
            if( p_move.m_param == M_ERUPTION || p_move.m_param == M_WATER_SPOUT ) {
                movePower = 150 * user->m_stats.m_curHP / user->m_stats.m_maxHP;
            }
            if( p_move.m_param == M_FLAIL || p_move.m_param == M_REVERSAL ) {
                auto relhp = 48 * user->m_stats.m_curHP / user->m_stats.m_maxHP;
                if( relhp > 32 ) { movePower = 20; }
                else if( relhp > 16 ) { movePower = 40; }
                else if( relhp > 9 ) { movePower = 60; }
                else if( relhp > 4 ) { movePower = 100; }
                else if( relhp > 1 ) { movePower = 150; }
                else { movePower = 200; }
            }
            if( p_move.m_param == M_CRUSH_GRIP || p_move.m_param == M_WRING_OUT ) {
                movePower = 120 * target->m_stats.m_curHP / target->m_stats.m_maxHP;
            }

            // happiness-based

            if( p_move.m_param == M_RETURN ) {
                movePower = 1 + user->m_boxdata.m_steps * 10 / 25;
            }
            if( p_move.m_param == M_FRUSTRATION ) {
                movePower = 1 + ( 255 - user->m_boxdata.m_steps ) * 10 / 25;
            }

            // repetition-based

            if( p_move.m_param == M_FURY_CUTTER ) {
                u8 cnt = getConsecutiveMoveCount( p_move.m_user.first, p_move.m_user.second );

                if( cnt == 0 || getLastUsedMove( p_move.m_user.first, p_move.m_user.second ).
                        m_param != M_FURY_CUTTER ) { movePower = 40; }
                else if( cnt == 1 ) { movePower = 80; }
                else { movePower = 160; }
            }
            if( p_move.m_param == M_ROLLOUT || p_move.m_param == M_ICE_BALL ) {
                u8 cnt = getConsecutiveMoveCount( p_move.m_user.first, p_move.m_user.second );

                if( cnt == 0 || getLastUsedMove( p_move.m_user.first, p_move.m_user.second ).
                        m_param != p_move.m_param ) { movePower = 30; }
                else if( cnt == 1 ) { movePower = 60; }
                else if( cnt == 2 ) { movePower = 120; }
                else if( cnt == 3 ) { movePower = 240; }
                else { movePower = 480; }

                if( getVolatileStatus( p_move.m_user.first, p_move.m_user.second )
                        & DEFENSECURL ) {
                    movePower <<= 1;
                }
            }
            if( p_move.m_param == M_SPIT_UP ) {
                u8 stkpile = getVolatileStatusCounter( p_move.m_user.first, p_move.m_user.second,
                        STOCKPILE );
                if( stkpile > 3 ) { stkpile = 3; }

                movePower = 100 * stkpile;

                removeVolatileStatus( p_move.m_user.first, p_move.m_user.second, STOCKPILE );
                boosts bt = boosts( );
                bt.setBoost( DEF, -stkpile );
                bt.setBoost( SDEF, -stkpile );
                auto res = addBoosts( p_move.m_user.first, p_move.m_user.second, bt );
                p_ui->logBoosts( user,  p_move.m_user.first, p_move.m_user.second, bt, res );
            }

            // TODO
            // Stored power, power trip: 20 + 20 * atk stat boost
            // punishment: 60 + 20 * def stat boost

            // acrobatics
            // assurance
            // avalanche / revenge
            // grass / fire / water pledge
            // hex
            // payback
            // round
            // smelling salts
            // stomping tantrum
            // wake-up slap
            //
            // fling, natural gift
            //
            // beat up
            // echoed voice
            // magnitude
            // present
            // triple kick
            // trump card

            damage *= movePower;

            damage = ( damage * atk ) / def;
            damage = ( damage / 50 ) + 2;

            if( p_move.m_target.size( ) > 1 ) {
                damage = ( damage * 75 ) / 100;
            }

            if( !suppressesWeather( ) ) [[likely]] {
                if( p_move.m_moveData.m_type == WATER &&
                        ( _weather == RAIN || _weather == HEAVY_RAIN ) ) [[unlikely]] {
                    damage = ( damage * 3 ) >> 1;
                }
                if( p_move.m_moveData.m_type == FIRE &&
                        ( _weather == RAIN || _weather == HEAVY_RAIN ) ) [[unlikely]] {
                    damage = ( damage >> 1 );
                }
                if( p_move.m_moveData.m_type == FIRE &&
                        ( _weather == SUN || _weather == HEAVY_SUNSHINE ) ) [[unlikely]] {
                    damage = ( damage * 3 ) >> 1;
                }
                if( p_move.m_moveData.m_type == WATER &&
                        ( _weather == SUN || _weather == HEAVY_SUNSHINE ) ) [[unlikely]] {
                    damage = ( damage >> 1 );
                }
            }

            if( p_critical ) {
                damage = ( damage * 3 ) >> 1;
            }

            u8 rnd = rand( ) & 15;
            damage = ( damage * ( 85 + rnd ) / 100 );

            // stab
            type moveType = getMoveType( p_move );

            if( !supprAbs && ( user->getAbility( ) == A_PROTEAN
                        || user->getAbility( ) == A_LIBERO ) ) {
                p_ui->logAbility( user, p_move.m_user.first );
                setType( p_move.m_user.first, p_move.m_user.second, moveType );
                // TODO log
            }

            if( hasType( p_move.m_user.first, p_move.m_user.second, moveType ) ) {
                if( !supprAbs && user->getAbility( ) == A_ADAPTABILITY ) {
                    damage <<= 1;
                } else {
                    damage = ( damage * 3 ) >> 1;
                }
            }

            // effectiveness
            damage = ( damage * effectiveness ) / 100;

            // burn

            if( user->m_status.m_isBurned && p_move.m_moveData.m_category == MOVE::PHYSICAL
                    && ( supprAbs || user->getAbility( ) != A_GUTS )
                    && p_move.m_param != M_FACADE ) {
                damage >>= 1;
            }

            damage = damage * p_damageModifier / 100;

            auto sidec = getSideCondition( !p_move.m_user.first );

            if( supprAbs || !anyHasAbility( A_SCREEN_CLEANER ) ) {
                if( ( sidec & AURORAVEIL )
                        || ( ( sidec & REFLECT ) &&
                            p_move.m_moveData.m_defensiveCategory == MOVE::PHYSICAL )
                        || ( ( sidec & LIGHTSCREEN ) &&
                            p_move.m_moveData.m_defensiveCategory == MOVE::SPECIAL ) ) {
                    // nerf intentional!
                    damage = ( damage * 66 ) / 100;
                }
            }

            if( p_move.m_param == M_BODY_SLAM
                    || p_move.m_param == M_DRAGON_RUSH
                    || p_move.m_param == M_FLYING_PRESS
                    || p_move.m_param == M_HEAT_CRASH
                    || p_move.m_param == M_HEAVY_SLAM
                    || p_move.m_param == M_PHANTOM_FORCE
                    || p_move.m_param == M_SHADOW_FORCE
                    || p_move.m_param == M_STOMP ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & MINIMIZE ) [[unlikely]] {
                    damage <<= 1;
                }
            }

            if( p_move.m_param == M_SURF || p_move.m_param == M_WHIRLPOOL ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & DIVING ) {
                    damage <<= 1;
                }
            }

            if( p_move.m_param == M_EARTHQUAKE || p_move.m_param == M_MAGNITUDE ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & DIGGING ) {
                    damage <<= 1;
                }
            }

            if( p_move.m_param == M_GUST || p_move.m_param == M_TWISTER ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & INAIR ) {
                    damage <<= 1;
                }
            }

            // abilities

            // TODO
            // aura break
            // iron fist, reckless
            // battery
            // sheer force, sand force, analytic, tough claws
            // fairy aura, dark aura
            // technician
            // flare boost
            // toxic boost
            // strong jaw
            // mega launcher
            // heatproof
            // dry skin
            //
            // Muscle band
            // wise glasses
            //
            // plates, gems, incenses
            // adamant orb, lustrous orb, griseous orb, soul dew
            // normal gem
            // solar beam/blade for non-sunny
            // me first
            // knock off
            // helping hand
            // charge
            // brine
            // venoshock
            // retaliate
            // fusion bolt/flare
            // terrains
            // mud sport / water sport

            // foul play
            //
            // water bubble/stakeout
            // thick fat/water bubble (def)
            //
            // chip away/ sacred sword
            // wonder room
            //
            //  . . .

            if( !supprAbs ) {
                switch( target->getAbility( ) ) {
                    case A_FLUFFY:
                        if( p_move.m_moveData.m_flags & MOVE::CONTACT ) {
                            if( moveType == FIRE ) {
                                damage <<= 1;
                            } else {
                                damage >>= 1;
                            }
                        }
                        break;
                    case A_FILTER:
                    case A_PRISM_ARMOR:
                    case A_SOLID_ROCK:
                        if( effectiveness > 100 ) {
                            damage = ( damage * 75 ) / 100;
                        }
                        break;
                    case A_SHADOW_SHIELD:
                    case A_MULTISCALE:
                        if( target->m_stats.m_curHP == target->m_stats.m_maxHP ) {
                            damage >>= 1;
                        }
                        break;
                    case A_ICE_SCALES:
                        if( p_move.m_moveData.m_defensiveCategory == MOVE::SPECIAL ) {
                            damage >>= 1;
                        }
                        break;
                    case A_PUNK_ROCK:
                        if( p_move.m_moveData.m_flags & MOVE::SOUND ) {
                            damage >>= 1;
                        }
                        break;
                    [[likely]] default:
                        break;
                }

                if( getPkmn( p_target.first, !p_target.second ) != nullptr
                        && getPkmn( p_target.first, !p_target.second )->getAbility( ) ==
                        A_FRIEND_GUARD ) [[unlikely]] {
                    damage = ( damage * 75 ) / 100;
                }

                switch( user->getAbility( ) ) {
                    case A_TORRENT:
                        if( moveType == WATER
                                && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                            damage = ( 3 * damage ) >> 1;
                        }
                        break;
                    case A_OVERGROW:
                        if( moveType == GRASS
                                && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                            damage = ( 3 * damage ) >> 1;
                        }
                        break;
                    case A_BLAZE:
                        if( moveType == FIRE
                                && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                            damage = ( 3 * damage ) >> 1;
                        }
                        break;
                    case A_SWARM:
                        if( moveType == BUG
                                && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                            damage = ( 3 * damage ) >> 1;
                        }
                        break;
                    case A_STEELWORKER:
                        if( moveType == STEEL ) {
                            damage = ( 3 * damage ) >> 1;
                        }
                        break;

                    case A_RIVALRY:
                        if( user->gender( ) * target->gender( ) < 0 ) {
                            damage = ( damage * 75 ) / 100;
                        } else if( user->gender( ) * target->gender( ) > 0 ) {
                            damage = ( damage * 125 ) / 100;
                        }
                        break;
                    case A_NEUROFORCE:
                        if( effectiveness > 100 ) {
                            damage = ( damage * 125 ) / 100;
                        }
                        break;
                    case A_TINTED_LENS:
                        if( effectiveness < 100 ) {
                            damage <<= 1;
                        }
                        break;
                    case A_SNIPER:
                        if( p_critical ) {
                            damage = ( 3 * damage ) >> 1;
                        }
                        break;


                    [[likely]] default:
                        break;
                }
            }

            if( canUseItem( p_move.m_user.first, p_move.m_user.second, !supprAbs ) ) {
                switch( user->getItem( ) ) {
                    case I_EXPERT_BELT:
                        if( effectiveness > 100 ) {
                            damage = ( damage * 120 ) / 100;
                        }
                        break;
                    case I_LIFE_ORB:
                        if( effectiveness > 100 ) {
                            damage = ( damage * 130 ) / 100;
                        }
                        break;
                    case I_METRONOME: {
                        auto cons = 100 + 20 * std::min( u8( 5 ),
                                getConsecutiveMoveCount( p_move.m_user.first,
                                    p_move.m_user.second ) );
                        damage = ( damage * cons / 100 );
                        break;
                    }
                    default:
                            break;
                }
            }

            if( canUseItem( p_target.first, p_target.second, !supprAbs )
                    && ( !supprAbs || !_sides[ p_target.first ? PLAYER_SIDE
                        : OPPONENT_SIDE ].anyHasAbility( A_UNNERVE ) ) ) {
                bool eatitem = false;
                switch( target->getItem( ) ) {
                    case I_CHILAN_BERRY:
                        if( moveType == type::NORMAL ) { eatitem = true; }
                        break;
                    case I_BABIRI_BERRY:
                        if( moveType == type::STEEL && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_CHARTI_BERRY:
                        if( moveType == type::ROCK && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_CHOPLE_BERRY:
                        if( moveType == type::FIGHTING && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_COBA_BERRY:
                        if( moveType == type::FLYING && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_COLBUR_BERRY:
                        if( moveType == type::DARK && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_HABAN_BERRY:
                        if( moveType == type::DRAGON && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_KASIB_BERRY:
                        if( moveType == type::GHOST && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_KEBIA_BERRY:
                        if( moveType == type::POISON && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_OCCA_BERRY:
                        if( moveType == type::FIRE && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_PASSHO_BERRY:
                        if( moveType == type::WATER && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_PAYAPA_BERRY:
                        if( moveType == type::PSYCHIC && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_RINDO_BERRY:
                        if( moveType == type::GRASS && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_ROSELI_BERRY:
                        if( moveType == type::FAIRY && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_SHUCA_BERRY:
                        if( moveType == type::GROUND && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_TANGA_BERRY:
                        if( moveType == type::BUG && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_WACAN_BERRY:
                        if( moveType == type::ELECTRIC && effectiveness > 100 ) { eatitem = true; }
                        break;
                    case I_YACHE_BERRY:
                        if( moveType == type::ICE && effectiveness > 100 ) { eatitem = true; }
                        break;
                    default:
                        break;
                }
                if( eatitem ) {
                    damage >>= 1;
                    snprintf( buffer, 99, GET_STRING( 279 ),
                            p_ui->getPkmnName( user, p_move.m_user.first ).c_str( ),
                            ITEM::getItemName( I_LANSAT_BERRY ).c_str( ) );
                    p_ui->log( buffer );

                    removeItem( p_ui, p_target.first, p_target.second );
                    checkOnEatBerry( p_ui, p_target.first, p_target.second,
                            target->getItem( ) );
                }
            }

            if( !damage ) { damage = 1; }
            if( damage > 9000 ) { damage = 9000; }
        }

        p_ui->animateHitPkmn( p_target.first, p_target.second, effectiveness );

        damagePokemon( p_ui, p_target.first, p_target.second, damage );

        if( effectiveness > 100 ) {
            snprintf( buffer, 99, GET_STRING( 285 ),
                      p_ui->getPkmnName( target, p_target.first, false ).c_str( ) );
            p_ui->log( buffer );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        } else if( effectiveness < 100 ) {
            snprintf( buffer, 99, GET_STRING( 286 ),
                      p_ui->getPkmnName( target, p_target.first, false ).c_str( ) );
            p_ui->log( buffer );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        }

        if( p_critical ) {
            p_ui->log( GET_STRING( 291 ) );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        }

        if( !getPkmn( p_target.first, p_target.second )->canBattle( ) ) {
            // pkmn fainted
            faintPokemon( p_ui, p_target.first, p_target.second );
        }

        // Calculate recoil / drain

        if( p_move.m_moveData.m_drain ) {
            healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                    damage * p_move.m_moveData.m_drain / 240 );

            snprintf( buffer, 99, GET_STRING( 288 ),
                      p_ui->getPkmnName( user, p_move.m_user.first ).c_str( ) );
            p_ui->log( buffer );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        }

        if( p_move.m_moveData.m_recoil ) {
            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                    damage * p_move.m_moveData.m_recoil / 240 );

            snprintf( buffer, 99, GET_STRING( 287 ),
                      p_ui->getPkmnName( user, p_move.m_user.first ).c_str( ) );
            p_ui->log( buffer );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        }

        // Life orb recoil
        if( user->canBattle( ) && canUseItem( p_move.m_user.first, p_move.m_user.second )
                && user->getItem( ) == I_LIFE_ORB ) {

            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                    user->m_stats.m_maxHP / 16 );

            snprintf( buffer, 99, GET_STRING( 306 ),
                      p_ui->getPkmnName( user, p_move.m_user.first ).c_str( ) );
            p_ui->log( buffer );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        }

        // Check if user fainted
        if( !user->canBattle( ) ) {
            faintPokemon( p_ui, p_move.m_user.first, p_move.m_user.second );
        }

        return true;
    }

    void field::executeBattleMove( battleUI* p_ui, battleMove p_move,
                                   const std::vector<battleMove>& p_targetsMoves,
                                   const std::vector<battleMove>& p_tergetedMoves ) {

        (void) p_targetsMoves;
        (void) p_tergetedMoves;

        char buffer[ 100 ];
        bool opponent = p_move.m_user.first;
        u8 slot = p_move.m_user.second;

        if( getSlotStatus( opponent, slot ) != slot::status::NORMAL ) { return; }

        // Check if the move fails
        if( !useMove( p_ui, p_move ) ) {
            // Move failed (e.g. due to confusion, sleep etc.
            deducePP( opponent, slot, p_move.m_param );

            setLastUsedMove( opponent, slot, battleMove( ) );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
            return;
        }

        auto slotc = getVolatileStatus( opponent, slot );

        if( p_move.m_moveData.m_flags & MOVE::CHARGE ) {
            if( slotc & MOVECHARGE ) { // Move is already charged
                removeVolatileStatus( opponent, slot, volatileStatus(
                            MOVECHARGE | DIGGING | DIVING | INAIR | INVISIBLE ) );
            } else {
                p_ui->prepareMove( opponent, slot, p_move );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }

                if( ( p_move.m_param == M_SOLAR_BLADE || p_move.m_param == M_SOLAR_BEAM )
                        && !suppressesWeather( ) &&
                        ( _weather == SUN || _weather == HEAVY_SUNSHINE ) ) [[unlikely]] {
                    // empty!
                } else if( canUseItem( opponent, slot ) &&
                        getPkmn( opponent, slot )->getItem( ) == I_POWER_HERB ) [[unlikely]] {
                    snprintf( buffer, 99, GET_STRING( 305 ), p_ui->getPkmnName(
                                getPkmn( opponent, slot ), opponent ).c_str( ) );
                    p_ui->log( buffer );
                    for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }

                    removeItem( p_ui, opponent, slot );
                } else {
                    addVolatileStatus( opponent, slot, MOVECHARGE, 1 );

                    switch( p_move.m_param ) {
                        case M_DIVE:
                            addVolatileStatus( opponent, slot, DIVING, 1 );
                            break;
                        case M_DIG:
                            addVolatileStatus( opponent, slot, DIGGING, 1 );
                            break;
                        case M_FLY:
                        case M_BOUNCE:
                        case M_SKY_DROP:
                            addVolatileStatus( opponent, slot, INAIR, 1 );
                            break;
                        case M_PHANTOM_FORCE:
                        case M_SHADOW_FORCE:
                            addVolatileStatus( opponent, slot, INVISIBLE, 1 );
                            break;

                        default: break;
                    }

                    battleMoveSelection bms = NO_OP_SELECTION;
                    bms.m_type = ATTACK;
                    bms.m_param = p_move.m_param;
                    bms.m_moveData = p_move.m_moveData;
                    if( p_move.m_target.size( ) != 1 ) [[unlikely]] {
#ifdef DESQUID
                        p_ui->log( "[error] Multi-turn attack has multiple targets." );
#endif
                    } else {
                        bms.m_target = p_move.m_target[ 0 ];
                    }
                    bms.m_user = p_move.m_user;

                    addLockedMove( opponent, slot, bms );
                    for( u8 i = 0; i < 10; ++i ) { swiWaitForVBlank( ); }
                    return;
                }
            }
        }

        if( !getLockedMoveCount( opponent, slot ) ) {
            deducePP( opponent, slot, p_move.m_param );
        }
        snprintf( buffer, 99, GET_STRING( 10 ), p_ui->getPkmnName(
                    getPkmn( opponent, slot ), opponent ).c_str( ),
                MOVE::getMoveName( p_move.m_param ).c_str( ) );
        p_ui->log( buffer );

        for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

        for( u8 i = 0; i < p_move.m_target.size( ); ++i ) {
            // Check for multi-hit moves

            u8 numHits = 1, strengthMod = 100;
            if( p_move.m_moveData.getMultiHitMax( ) > 1 ) [[unlikely]] {
                numHits = p_move.m_moveData.getMultiHitMin( );
                numHits += rand( ) % ( p_move.m_moveData.getMultiHitMax( )
                        - p_move.m_moveData.getMultiHitMin( ) );

                if( !suppressesAbilities( ) &&
                        getPkmn( opponent, slot )->getAbility( ) == A_SKILL_LINK ) [[unlikely]] {
                    numHits = p_move.m_moveData.getMultiHitMax( );
                }
            } else if( !suppressesAbilities( ) &&
                    getPkmn( opponent, slot )->getAbility( ) == A_PARENTAL_BOND
                    && p_move.m_moveData.m_category != MOVE::STATUS ) [[unlikely]] {
                numHits = 2;
                strengthMod = 25;
            }

            u8 hits = 0;
            for( u8 j = 0; j < numHits; ++j ) {
                auto tgsc = getVolatileStatus( p_move.m_target[ i ].first,
                        p_move.m_target[ i ].second );

                // Check if the move is protected against
                bool protect = false;
                if( p_move.m_moveData.m_flags & MOVE::PROTECT ) [[likely]] {
                    if( ( tgsc & PROTECT ) || ( tgsc & OBSTRUCT ) || ( tgsc & SPIKYSHIELD )
                            || ( p_move.m_moveData.m_category != MOVE::STATUS &&
                                ( tgsc & KINGSSHIELD ) ) ) [[unlikely]] {
                        protect = true;
                    }
                }

                if( !protect && p_move.m_moveData.m_category != MOVE::STATUS ) [[likely]] {
                    bool critical = executeCriticalCheck( p_ui, p_move, p_move.m_target[ i ] );

                    // Check if the move misses
                    if( moveMisses( p_ui, p_move, p_move.m_target[ i ], critical ) ) {
                        snprintf( buffer, 99, GET_STRING( 280 ), p_ui->getPkmnName(
                                    getPkmn( p_move.m_target[ i ].first,
                                        p_move.m_target[ i ].second ),
                                    p_move.m_target[ i ].first ).c_str( ) );
                        p_ui->log( buffer );
                        for( u8 k = 0; k < 30; ++k ) { swiWaitForVBlank( ); }
                        continue;
                    }

                    if( !executeDamagingMove( p_ui, p_move, p_move.m_target[ i ], critical,
                                j == 1 && strengthMod < 100 ? strengthMod : 100 ) ) {
                        break;
                    }
                    hits++;
                }

                // Pkmn fainted
                if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { break; }
                // Target fainted
                if( getSlotStatus( p_move.m_target[ i ].first,
                            p_move.m_target[ i ].second ) == slot::status::FAINTED ) { break; }

                if( !protect ) [[likely]] {
                    executeStatusEffects( p_ui, p_move, p_move.m_target[ i ] );

                    // Check for secondary status effects
                    if( p_move.m_moveData.m_secondaryStatus
                            && ( rand( ) % 100 < p_move.m_moveData.m_secondaryChance ) ) {
                        executeSecondaryStatus( p_ui, p_move, p_move.m_target[ i ] );
                    }
                    // Other secondary effects
                    if( rand( ) % 100 < p_move.m_moveData.m_secondaryChance ) {
                        executeSecondaryEffects( p_ui, p_move, p_move.m_target[ i ] );
                    }
                }

                // Pkmn fainted
                if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { break; }
                // Target fainted
                if( getSlotStatus( p_move.m_target[ i ].first,
                            p_move.m_target[ i ].second ) == slot::status::FAINTED ) { break; }


                if( p_move.m_moveData.m_flags & MOVE::CONTACT ) {
                    // move makes contact
                    executeContact( p_ui, p_move, p_move.m_target[ i ] );
                }

                // Pkmn fainted
                if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { break; }
                // Target fainted
                if( getSlotStatus( p_move.m_target[ i ].first,
                            p_move.m_target[ i ].second ) == slot::status::FAINTED ) { break;; }

                if( p_move.m_moveData.m_flags & MOVE::FORCESWITCH ) {
                    // move forces switch out
                    recallPokemon( p_ui, p_move.m_target[ i ].first, p_move.m_target[ i ].second );
                    break;
                }

                if( p_move.m_moveData.m_flags & MOVE::DEFROSTTARGET ) {
                    if( hasStatusCondition( p_move.m_target[ i ].first, p_move.m_target[ i
                    ].second, FROZEN ) ) {
                        snprintf( buffer, 99, GET_STRING( 298 ), p_ui->getPkmnName(
                                    getPkmn( p_move.m_target[ i ].first,
                                        p_move.m_target[ i ].second ),
                                    p_move.m_target[ i ].first ).c_str( ) );
                        p_ui->log( buffer );
                        removeStatusCondition( p_move.m_target[ i ].first, p_move.m_target[ i
                        ].second );
                        p_ui->updatePkmnStats( p_move.m_target[ i ].first,
                                p_move.m_target[ i ].second,
                                getPkmn( p_move.m_target[ i ].first,
                                    p_move.m_target[ i ].second ) );
                        for( u8 k = 0; k < 30; ++k ) { swiWaitForVBlank( ); }
                    }
                }
            }

            if( hits > 1 ) {
                snprintf( buffer, 99, GET_STRING( 400 ), hits );
                p_ui->log( buffer );
                for( u8 k = 0; k < 30; ++k ) { swiWaitForVBlank( ); }
            }

            if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { return; }
        }


        // Check for flags and stuff

        if( p_move.m_moveData.m_flags & MOVE::DANCE ) {
            // Check if any pkmn has the "dancer" ability
            // TODO
        }

        if( p_move.m_moveData.m_flags & MOVE::MINDBLOWNRECOIL ) {
            // pkmn loses half of its max hp
            u16 maxHP = getPkmn( opponent, slot )->m_stats.m_maxHP;
            damagePokemon( p_ui, opponent, slot, maxHP / 2 );

            if( !getPkmn( opponent, slot)->canBattle( ) ) {
                faintPokemon( p_ui, opponent, slot );
                return;
            }
        }

        if( p_move.m_moveData.m_flags & MOVE::RECHARGE ) {
            battleMoveSelection bms = NO_OP_SELECTION;
            bms.m_type = ATTACK;
            addLockedMove( opponent, slot, bms, 0 );
            addVolatileStatus( opponent, slot, RECHARGE, 1 );
        }

        if( p_move.m_moveData.m_flags & MOVE::LOCKEDMOVE ) {
            auto lmcnt = getLockedMoveCount( opponent, slot );
            battleMoveSelection bms = NO_OP_SELECTION;
            bms.m_type = ATTACK;
            bms.m_param = p_move.m_param;
            bms.m_moveData = p_move.m_moveData;
            bms.m_target = { 255, 255 };
            bms.m_user = p_move.m_user;
            if( !lmcnt ) {
                addLockedMove( opponent, slot, bms, 1 + rand( ) % 2 );
            } else if( --lmcnt ) {
                addLockedMove( opponent, slot, bms, lmcnt );
            } else {
                removeLockedMove( opponent, slot );
                if( suppressesAbilities( )
                        || getPkmn( opponent, slot )->getAbility( ) != A_OWN_TEMPO
                        || ( getTerrain( ) == MISTYTERRAIN && isGrounded( opponent, slot ) )
                        || ( getVolatileStatus( opponent, slot ) & SUBSTITUTE ) ) {
                    snprintf( buffer, 99, GET_STRING( 277 ), p_ui->getPkmnName(
                                getPkmn( opponent, slot ), opponent ).c_str( ) );
                    p_ui->log( buffer );

                    if( getVolatileStatus( opponent, slot ) & CONFUSION ) {
                        // empty!
                    } else {
                        if( suppressesAbilities( ) ||
                                !_sides[ opponent ? PLAYER_SIDE : OPPONENT_SIDE ].anyHasAbility(
                                    A_UNNERVE ) ) [[likely]] {
                            if( canUseItem( opponent, slot ) &&
                                    ( getPkmn( opponent, slot )->getItem( ) == I_LUM_BERRY
                                      || getPkmn( opponent, slot )->getItem( ) == I_PERSIM_BERRY
                                      || getPkmn( opponent, slot )->getItem( ) == I_RIE_BERRY ) ) {
                                auto berry = getPkmn( opponent, slot )->getItem( );

                                snprintf( buffer, 99, GET_STRING( 278 ), p_ui->getPkmnName(
                                            getPkmn( opponent, slot ), opponent ).c_str( ),
                                        ITEM::getItemName( berry ).c_str( ) );
                                p_ui->log( buffer );

                                checkOnEatBerry( p_ui, opponent, slot, berry );
                            } else {
                                addVolatileStatus( opponent, slot, CONFUSION, 2 + ( rand( ) % 4 ) );
                            }
                        } else {
                            addVolatileStatus( opponent, slot, CONFUSION, 2 + ( rand( ) % 4 ) );
                        }
                    }
                }
            }
        }

        if( p_move.m_moveData.m_flags & MOVE::ROOST ) {
            addVolatileStatus( opponent, slot, ROOST, 1 );
        }

        if( p_move.m_param == M_BURN_UP ) {
            addVolatileStatus( opponent, slot, BURNUP, -1 );
        }

        if( p_move.m_moveData.m_flags & MOVE::SELFSWITCH ) {
            recallPokemon( p_ui, opponent, slot );
        }

        if( p_move.m_moveData.m_flags & MOVE::BATONPASS ) {
            recallPokemon( p_ui, opponent, slot, true );
        }

        if( p_move.m_moveData.m_flags & MOVE::SELFDESTRUCT ) {
            faintPokemon( p_ui, opponent, slot );
        }

        setLastUsedMove( opponent, slot, p_move );
    }

} // namespace BATTLE
