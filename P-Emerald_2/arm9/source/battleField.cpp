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
        _pseudoWeatherTimer = u8( -1 ); // Initial pseudo weather stays forever

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
                        pkmnData own = getPkmnData( pkmn->getSpecies( ), pkmn->getForme( ) );
                        for( u8 i = 0; !warn && i < 2; ++i ) {
                            auto tmp = getPkmn( !p_opponent, i );
                            if( tmp == nullptr ) { continue; }
                            for( u8 j = 0; j < 4; ++j ) {
                                if( tmp->getMove( j ) ) {
                                    auto mdata = MOVE::getMoveData( tmp->getMove( j ) );
                                    if( ( mdata.m_flags & MOVE::OHKO ) ) {
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

    std::vector<battleMove>
    field::computeSortedBattleMoves( const std::vector<battleMoveSelection>& p_selectedMoves ) {
        // TODO
        return {};
    }

    void field::executeBattleMove( battleUI* p_ui, battleMove p_move,
                                   std::vector<battleMove> p_targetsMoves,
                                   std::vector<battleMove> p_tergetedMoves ) {
        // TODO
    }

} // namespace BATTLE
