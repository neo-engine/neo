/*
Pokémon neo
------------------------------

file        : battleFieldSendOut.cpp
author      : Philip Wellnitz
description : send-out triggers and paradox-form latching

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
    u8 field::_paradoxBestStat( bool p_opponent, u8 p_slot ) {
        static constexpr u8 ORDER[ 5 ] = { ATK, DEF, SATK, SDEF, SPEED };
        u8                  best       = ATK;
        u16                 bestV      = 0;
        for( u8 k = 0; k < 5; ++k ) {
            u16 v = getStat( p_opponent, p_slot, ORDER[ k ], false );
            if( v > bestV ) {
                bestV = v;
                best  = ORDER[ k ];
            }
        }
        return best;
    }

    void field::_paradoxRefresh( battleUI* p_ui ) {
        for( u8 i = 0; i < NUM_SIDES; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pk = getPkmn( i, j );
                if( pk == nullptr || !pk->canBattle( ) ) { continue; }
                const bool proto = pk->getAbility( ) == A_PROTOSYNTHESIS;
                const bool quark = pk->getAbility( ) == A_QUARK_DRIVE;
                if( !proto && !quark ) { continue; }
                if( suppressesAbilities( ) || slotAbilitiesNullified( i, j ) ) { continue; }
                const bool active = proto ? ( _weather == WE_SUN || _weather == WE_HEAVY_SUNSHINE )
                                          : ( _terrain == TR_ELECTRICTERRAIN );
                if( active ) {
                    if( _paradoxStat[ i ][ j ] == 0 ) {
                        p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                        _paradoxStat[ i ][ j ] = _paradoxBestStat( i, j );
                    }
                } else if( _paradoxBooster[ i ][ j ] == 0 ) {
                    _paradoxStat[ i ][ j ] = 0;
                }
            }
        }
    }

    void field::checkOnSendOut( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return; }

        _metronomeNum[ p_opponent ][ p_slot ]   = 0;
        _metronomeLast[ p_opponent ][ p_slot ]  = 0;
        _actionThisTurn[ p_opponent ][ p_slot ] = 0;
        _actionLastTurn[ p_opponent ][ p_slot ] = 0;
        _paradoxStat[ p_opponent ][ p_slot ]    = 0;
        _paradoxBooster[ p_opponent ][ p_slot ] = 0;
        auto cond                               = getSlotCondition( p_opponent, p_slot );
        if( cond & ( SL_HEALINGWISH | SL_LUNARDANCE ) ) {
            bool trigger = !pkmn->canBattle( ) ? false
                                               : ( pkmn->m_stats.m_curHP < pkmn->m_stats.m_maxHP
                                                   || pkmn->m_statusint != 0 );
            if( !trigger && ( cond & SL_LUNARDANCE ) && pkmn->canBattle( ) ) {
                trigger = hasMovePPDeficit( p_opponent, p_slot );
            }
            if( trigger ) {
                bool lunar = ( cond & SL_LUNARDANCE ) != slotCondition( 0 );

                healFully( p_ui, p_opponent, p_slot );
                removeStatusCondition( p_opponent, p_slot );
                if( lunar ) { restoreAllMovePP( p_opponent, p_slot ); }

                auto fmt = std::string( GET_STRING( lunar ? IO::STR_BATTLE_LUNAR_DANCE_HEAL
                                                          : IO::STR_BATTLE_HEALING_WISH_HEAL ) );
                std::array<char, 100> buffer{ };
                snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent )
                              .c_str( ) );
                p_ui->log( buffer.data( ) );

                removeSlotCondition( p_ui, p_opponent, p_slot, cond );
            }
        }

        bool          items = canUseItem( p_opponent, p_slot );
        sideCondition sc_ = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSideCondition( );

        if( sc_ & SC_STEALTHROCK ) {
            if( items && pkmn->getItem( ) == I_HEAVY_DUTY_BOOTS ) {
                // blocked
            } else if( suppressesAbilities( ) || slotAbilitiesNullified( p_opponent, p_slot )
                       || pkmn->getAbility( ) != A_MAGIC_GUARD ) {
                s8 typeMod = 0;
                for( type t : getTypes( p_opponent, p_slot ) ) {
                    switch( getTypeEffectiveness( TYPE_ROCK, t ) ) {
                    case 200: ++typeMod; break; // weak (bug/fire/flying/ice)
                    case 50: --typeMod; break;  // resist (fight/ground/steel)
                    case 25: typeMod -= 2; break;
                    default: break; // 100 neutral; 0 immunity: rock hits all
                    }
                }
                if( typeMod < -6 ) { typeMod = -6; }
                if( typeMod > 6 ) { typeMod = 6; }
                u16 damage = typeMod >= 0
                                 ? (u16) ( ( (u32) pkmn->m_stats.m_maxHP << typeMod ) >> 3 )
                                 : (u16) ( pkmn->m_stats.m_maxHP >> ( 3 - typeMod ) );
                if( damage ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HURT_BY_POINTED_STONES ) );
                    std::array<char, 100> buffer{ };
                    snprintf(
                        buffer.data( ), buffer.size( ), fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent )
                            .c_str( ) );
                    p_ui->log( buffer.data( ) );
                }
                damagePokemon( p_ui, p_opponent, p_slot, damage );
                if( !pkmn->canBattle( ) ) {
                    faintPokemon( p_ui, p_opponent, p_slot );
                    return;
                }
            }
        }

        if( sc_ & SC_SPIKES ) {
            if( isGrounded( p_opponent, p_slot, false )
                && !( items && pkmn->getItem( ) == I_HEAVY_DUTY_BOOTS )
                && ( suppressesAbilities( ) || slotAbilitiesNullified( p_opponent, p_slot )
                     || pkmn->getAbility( ) != A_MAGIC_GUARD ) ) {
                const u16 damageDivisor[ 4 ] = { 0, 8, 6, 4 };
                u8        layers             = sideConditionAmount( p_opponent, SC_SPIKES );
                if( layers > 3 ) { layers = 3; }
                u16 damage = pkmn->m_stats.m_maxHP / damageDivisor[ layers ];
                if( damage ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HURT_BY_SPIKES ) );
                    std::array<char, 100> buffer{ };
                    snprintf(
                        buffer.data( ), buffer.size( ), fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent )
                            .c_str( ) );
                    p_ui->log( buffer.data( ) );
                }
                damagePokemon( p_ui, p_opponent, p_slot, damage );
                if( !pkmn->canBattle( ) ) {
                    faintPokemon( p_ui, p_opponent, p_slot );
                    return;
                }
            }
        }

        if( sc_ & SC_TOXICSPIKES ) {
            if( isGrounded( p_opponent, p_slot ) ) {
                if( hasType( p_opponent, p_slot, TYPE_POISON ) ) {
                    removeSideCondition( p_ui, p_opponent, SC_TOXICSPIKES );
                } else if( !( items && pkmn->getItem( ) == I_HEAVY_DUTY_BOOTS )
                           && !hasType( p_opponent, p_slot, TYPE_STEEL ) ) {
                    u8 status
                        = sideConditionAmount( p_opponent, SC_TOXICSPIKES ) >= 2 ? TOXIC : POISON;
                    if( terrainGuardsStatus( fieldPosition( p_opponent, p_slot ), true, false,
                                             false, false, false, false ) ) {
                    } else if( setStatusCondition( p_opponent, p_slot, status ) ) {
                        p_ui->animateGetStatusCondition( getPkmnOrDisguise( p_opponent, p_slot ),
                                                         p_opponent, p_slot, status );
                        p_ui->updatePkmnStats( p_opponent, p_slot,
                                               getPkmnOrDisguise( p_opponent, p_slot ), true );
                    }
                }
            }
        }

        if( sc_ & SC_STICKYWEB ) {
            if( isGrounded( p_opponent, p_slot, false )
                && !( items && pkmn->getItem( ) == I_HEAVY_DUTY_BOOTS ) ) {
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_CAUGHT_IN_STICKY_WEB ) );
                std::array<char, 100> buffer{ };
                snprintf( buffer.data( ), buffer.size( ), fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent )
                              .c_str( ) );
                p_ui->log( buffer.data( ) );
                boosts bt = boosts( );
                bt.setBoost( SPEED, -1 );
                auto res = addBoosts( p_opponent, p_slot, bt, true, false, BOOST_SRC_HAZARD );
                p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent, p_slot, bt,
                                 res );
            }
        }

        switch( pkmn->getAbility( ) ) {
        // abilities that cannot be suppressed
        case A_COMATOSE:
        case A_NEUTRALIZING_GAS:
            p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
            break;
        case A_ZEN_MODE:
            if( pkmn->m_stats.m_curHP * 2 < pkmn->m_stats.m_maxHP && !( pkmn->getForme( ) & 1 ) ) {
                if( pkmn->getSpecies( ) == PKMN_DARMANITAN ) {
                    p_ui->logAbility( pkmn, p_opponent );
                    pkmn->battleTransform( );
                    p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                }
            }
            break;
        case A_SCHOOLING: {
            if( pkmn->m_level >= 20 && pkmn->m_stats.m_curHP * 4 > pkmn->m_stats.m_maxHP
                && !pkmn->getForme( ) ) {
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
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    break;
                case A_DISGUISE: {
                    if( pkmn->getForme( ) == 0 ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    }
                    break;
                }
                // Stat changing abilities
                case A_DAUNTLESS_SHIELD: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    boosts bt = boosts( );
                    bt.setBoost( DEF, 1 );
                    auto res = addBoosts( p_opponent, p_slot, bt );
                    p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent, p_slot,
                                     bt, res );
                    break;
                }
                case A_INTREPID_SWORD: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    boosts bt = boosts( );
                    bt.setBoost( ATK, 1 );
                    auto res = addBoosts( p_opponent, p_slot, bt );
                    p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent, p_slot,
                                     bt, res );
                    break;
                }
                case A_WIND_RIDER: {
                    if( getSideCondition( p_opponent ) & SC_TAILWIND ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                        boosts bt = boosts( );
                        bt.setBoost( ATK, 1 );
                        auto res = addBoosts( p_opponent, p_slot, bt );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent,
                                         p_slot, bt, res );
                    }
                    break;
                }
                case A_PROTOSYNTHESIS:
                case A_QUARK_DRIVE: {
                    if( !slotAbilitiesNullified( p_opponent, p_slot ) ) {
                        const bool proto = pkmn->getAbility( ) == A_PROTOSYNTHESIS;
                        const bool active
                            = proto ? ( _weather == WE_SUN || _weather == WE_HEAVY_SUNSHINE )
                                    : ( _terrain == TR_ELECTRICTERRAIN );
                        if( active ) {
                            p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                            _paradoxStat[ p_opponent ][ p_slot ]
                                = _paradoxBestStat( p_opponent, p_slot );
                        } else if( items && pkmn->getItem( ) == I_BOOSTER_ENERGY ) {
                            p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                            removeItem( p_ui, p_opponent, p_slot );
                            _paradoxStat[ p_opponent ][ p_slot ]
                                = _paradoxBestStat( p_opponent, p_slot );
                            _paradoxBooster[ p_opponent ][ p_slot ] = 1;
                        }
                    }
                    break;
                }
                case A_INTIMIDATE: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    boosts bt = boosts( );
                    bt.setBoost( ATK, -1 );
                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        if( getPkmn( !p_opponent, i ) == nullptr ) { continue; }
                        if( getSideCondition( !p_opponent ) & SC_MIST ) [[unlikely]] {
                            auto fmtM = std::string( GET_STRING( IO::STR_BATTLE_MIST_PROTECTS ) );
                            char bufM[ 110 ];
                            snprintf( bufM, sizeof( bufM ), fmtM.c_str( ),
                                      p_ui->getPkmnName( getPkmnOrDisguise( !p_opponent, i ),
                                                         !p_opponent )
                                          .c_str( ) );
                            p_ui->log( bufM );
                            continue;
                        }
                        auto res = addBoosts( !p_opponent, i, bt, true, false,
                                              packBoostSrc( { p_opponent, p_slot } ) );
                        p_ui->logBoosts( getPkmnOrDisguise( !p_opponent, i ), !p_opponent, i, bt,
                                         res );
                    }
                    break;
                }
                case A_DOWNLOAD: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    boosts bt = boosts( );

                    u16 def = 0, sdef = 0;
                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        def += getStat( !p_opponent, i, DEF );
                        sdef += getStat( !p_opponent, i, SDEF );
                    }
                    bt.setBoost( def < sdef ? ATK : SATK, 1 );
                    auto res = addBoosts( p_opponent, p_slot, bt );
                    p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent, p_slot,
                                     bt, res );
                    break;
                }
                // Weather abilities
                case A_DRIZZLE:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_RAIN, items && pkmn->getItem( ) == I_DAMP_ROCK );
                    break;
                case A_DROUGHT:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_SUN, items && pkmn->getItem( ) == I_HEAT_ROCK );
                    break;
                case A_SAND_STREAM:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_SANDSTORM, items && pkmn->getItem( ) == I_SMOOTH_ROCK );
                    break;
                case A_SNOW_WARNING:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_HAIL, items && pkmn->getItem( ) == I_ICY_ROCK );
                    break;
                case A_PRIMORDIAL_SEA:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_HEAVY_RAIN );
                    break;
                case A_DESOLATE_LAND:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_HEAVY_SUNSHINE );
                    break;
                case A_DELTA_STREAM:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setWeather( p_ui, WE_HEAVY_WINDS );
                    break;

                // Terrain abilities
                case A_ELECTRIC_SURGE:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setTerrain( p_ui, TR_ELECTRICTERRAIN,
                                items && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                    break;
                case A_PSYCHIC_SURGE:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setTerrain( p_ui, TR_PSYCHICTERRAIN,
                                items && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                    break;
                case A_GRASSY_SURGE:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setTerrain( p_ui, TR_GRASSYTERRAIN,
                                items && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                    break;
                case A_MISTY_SURGE:
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    setTerrain( p_ui, TR_MISTYTERRAIN,
                                items && pkmn->getItem( ) == I_TERRAIN_EXTENDER );
                    break;

                // Form change abilities
                case A_IMPOSTER:
                    if( transformPkmn(
                            p_opponent, p_slot,
                            getSlot( !p_opponent, getOpposingPkmn( p_slot, _mode ) ) ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                        p_ui->updatePkmn( p_opponent, p_slot, getPkmn( p_opponent, p_slot ) );
                        if( getPkmn( p_opponent, p_slot )->getAbility( ) != A_IMPOSTER ) {
                            checkOnSendOut( p_ui, p_opponent, p_slot );
                        }
                    }
                    break;

                case A_TRACE: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    auto tmp = getPkmn( !p_opponent, getOpposingPkmn( p_slot, _mode ) );
                    if( tmp != nullptr && allowsCopy( tmp->getAbility( ) )
                        && changeAbility( p_opponent, p_slot, tmp->getAbility( ) ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );

                        if( getPkmn( p_opponent, p_slot )->getAbility( ) != A_TRACE ) {
                            checkOnSendOut( p_ui, p_opponent, p_slot );
                        }
                    }
                    break;
                }

                case A_ANTICIPATION: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    bool warn = false;
                    for( u8 i = 0; !warn && i < getBattlingPKMNCount( _mode ); ++i ) {
                        auto tmp = getPkmn( !p_opponent, i );
                        if( tmp == nullptr ) { continue; }
                        for( u8 j = 0; j < 4; ++j ) {
                            if( tmp->getMove( j ) ) [[likely]] {
                                auto mdata = FS::getMoveData( tmp->getMove( j ) );
                                if( ( mdata.m_flags & MF_OHKO ) ) [[unlikely]] {
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
                        p_ui->logAnticipation( getPkmnOrDisguise( p_opponent, p_slot ),
                                               p_opponent );
                    }
                    break;
                }

                case A_FOREWARN: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    std::vector<std::pair<u8, u16>> moves = std::vector<std::pair<u8, u16>>( );
                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        auto tmp = getPkmn( !p_opponent, i );
                        if( tmp == nullptr ) { continue; }
                        for( u8 j = 0; j < 4; ++j ) {
                            if( tmp->getMove( j ) ) {
                                moves.push_back( std::pair<u8, u16>(
                                    255 - FS::getMoveData( tmp->getMove( j ) ).m_basePower,
                                    tmp->getMove( j ) ) );
                            }
                        }
                    }
                    std::sort( moves.begin( ), moves.end( ) );
                    p_ui->logForewarn( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent,
                                       moves[ 0 ].second );
                    break;
                }
                case A_FRISK: {
                    std::vector<u16> itms = std::vector<u16>( );

                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        auto tmp = getPkmn( !p_opponent, i );
                        if( tmp == nullptr ) { continue; }
                        if( tmp->getItem( ) ) { itms.push_back( tmp->getItem( ) ); }
                    }
                    if( itms.size( ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                        p_ui->logFrisk( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent, itms );
                    }
                    break;
                }

                case A_SCREEN_CLEANER: {
                    p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        _sides[ i ].removeSideCondition(
                            p_ui, sideCondition( SC_LIGHTSCREEN | SC_REFLECT | SC_AURORAVEIL ) );
                    }
                    break;
                }
                case A_FLOWER_GIFT: {
                    if( pkmn->getSpecies( ) == PKMN_CHERRIM ) {
                        if( !suppressesWeather( ) ) {
                            if( getWeather( ) == WE_SUN || getWeather( ) == WE_HEAVY_SUNSHINE ) {
                                pkmn->setBattleForme( 1 );
                                p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                                break;
                            }
                        }
                        pkmn->setBattleForme( 0 );
                        p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                    }

                    break;
                }
                case A_FORECAST: {
                    if( pkmn->getSpecies( ) == PKMN_CASTFORM ) {
                        if( !suppressesWeather( ) ) {
                            if( getWeather( ) == WE_RAIN || getWeather( ) == WE_HEAVY_RAIN ) {
                                pkmn->setBattleForme( 2 );
                                p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                                break;
                            }
                            if( getWeather( ) == WE_SUN || getWeather( ) == WE_HEAVY_SUNSHINE ) {
                                pkmn->setBattleForme( 1 );
                                p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                                break;
                            }
                            if( getWeather( ) == WE_HAIL ) {
                                pkmn->setBattleForme( 3 );
                                p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                                break;
                            }
                        }
                        pkmn->setBattleForme( 0 );
                        p_ui->updatePkmn( p_opponent, p_slot, pkmn );
                    }
                    break;
                }
                default: break;
                }
                break;
            }
        }

        _mirrorHerbConsume( p_ui );
    }

} // namespace BATTLE
