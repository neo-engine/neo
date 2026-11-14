/*
Pokémon neo
------------------------------

file        : battleField.cpp
author      : Philip Wellnitz
description :

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
    field::field( battle* p_battle, battleMode p_battleMode, bool p_isWildBattle,
                  weather p_initialWeather, pseudoWeather p_initialPseudoWeather,
                  terrain p_initialTerrain ) {
        _battle       = p_battle;
        _mode         = p_battleMode;
        _weather      = p_initialWeather;
        _weatherTimer = u8( -1 ); // Initial weather stays forever
        _isWildBattle = p_isWildBattle;

        std::memset( _pseudoWeatherTimer, 0, sizeof( _pseudoWeatherTimer ) );
        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( p_initialPseudoWeather & ( 1LLU << i ) ) [[unlikely]] {
                _pseudoWeatherTimer[ i ] = u8( -1 ); // Initial pseudo weather stays forever
            }
        }

        _terrain      = p_initialTerrain;
        _terrainTimer = u8( -1 ); // Initial terrain stays forever

        for( auto i = 0; i < NUM_SIDES; ++i ) { _sides[ i ] = side( ); }
    }

    void field::init( battleUI* p_ui ) {
        (void) p_ui;
    }

    bool field::setWeather( battleUI* p_ui, weather p_newWeather, bool p_extended ) {
        if( p_newWeather == _weather ) {
            p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            return false;
        }

        if( _weather == WE_HEAVY_RAIN || _weather == WE_HEAVY_SUNSHINE
            || _weather == WE_HEAVY_WINDS ) [[unlikely]] {
            // weather can be replaced only with a similar weather
            if( p_newWeather != WE_HEAVY_RAIN && p_newWeather != WE_HEAVY_SUNSHINE
                && p_newWeather != WE_HEAVY_WINDS ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return false;
            }
        }

        _weather      = p_newWeather;
        _weatherTimer = p_extended ? EXTENDED_DURATION : NORMAL_DURATION;

        p_ui->setNewWeather( p_newWeather );

        // Check for castform

        if( !suppressesAbilities( ) ) {
            for( u8 i = 0; i < 2; ++i ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto pkmn = getPkmn( i, j );
                    if( pkmn == nullptr ) { continue; }

                    if( pkmn->getSpecies( ) == PKMN_CASTFORM && !slotAbilitiesNullified( i, j )
                        && pkmn->getAbility( ) == A_FORECAST ) {
                        if( !suppressesWeather( ) ) {
                            if( getWeather( ) == WE_RAIN || getWeather( ) == WE_HEAVY_RAIN ) {
                                pkmn->setBattleForme( 2 );
                                p_ui->updatePkmn( i, j, pkmn );
                                continue;
                            }
                            if( getWeather( ) == WE_SUN || getWeather( ) == WE_HEAVY_SUNSHINE ) {
                                pkmn->setBattleForme( 1 );
                                p_ui->updatePkmn( i, j, pkmn );
                                continue;
                            }
                            if( getWeather( ) == WE_HAIL ) {
                                pkmn->setBattleForme( 3 );
                                p_ui->updatePkmn( i, j, pkmn );
                                continue;
                            }
                        }
                        pkmn->setBattleForme( 0 );
                        p_ui->updatePkmn( i, j, pkmn );
                    }

                    if( pkmn->getSpecies( ) == PKMN_CHERRIM && !slotAbilitiesNullified( i, j )
                        && pkmn->getAbility( ) == A_FLOWER_GIFT ) {
                        if( !suppressesWeather( ) ) {
                            if( getWeather( ) == WE_SUN || getWeather( ) == WE_HEAVY_SUNSHINE ) {
                                pkmn->setBattleForme( 1 );
                                p_ui->updatePkmn( i, j, pkmn );
                                continue;
                            }
                        }
                        pkmn->setBattleForme( 0 );
                        p_ui->updatePkmn( i, j, pkmn );
                    }
                }
            }
        }

        _paradoxRefresh( p_ui );
        return true;
    }

    void field::continueWeather( battleUI* p_ui ) {
        p_ui->continueWeather( );

        auto weather = getWeather( );
        if( weather == WE_NONE || suppressesWeather( ) ) {
            // nothing to do
            return;
        }

        bool supprA = suppressesAbilities( );

        for( u8 i = 0; i < 2; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pkmn = getPkmn( i, j );
                if( pkmn == nullptr || ( getVolatileStatus( i, j ) & VS_GASTROACID ) ) { continue; }
                auto volst = getVolatileStatus( i, j );

                if( weather == WE_RAIN || weather == WE_HEAVY_RAIN ) {
                    if( !supprA && pkmn->getAbility( ) == A_HYDRATION ) {
                        if( pkmn->m_statusint ) {
                            if( removeStatusCondition( i, j ) ) {
                                p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                                p_ui->updatePkmnStats( i, j, getPkmnOrDisguise( i, j ), true );
                            }
                        }
                    }
                    if( !supprA && pkmn->getAbility( ) == A_DRY_SKIN
                        && !( volst & VS_HEALBLOCK ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                        healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    }
                    if( !supprA && pkmn->getAbility( ) == A_RAIN_DISH
                        && !( volst & VS_HEALBLOCK ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                        healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                    }
                }
                if( weather == WE_SUN || weather == WE_HEAVY_SUNSHINE ) {
                    if( !supprA
                        && ( pkmn->getAbility( ) == A_DRY_SKIN
                             || pkmn->getAbility( ) == A_SOLAR_POWER ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 8 );
                    }
                }
                if( weather == WE_HAIL ) {
                    if( !supprA && pkmn->getAbility( ) == A_ICE_BODY ) {
                        if( !( volst & VS_HEALBLOCK ) ) {
                            p_ui->logAbility( getPkmnOrDisguise( i, j ), i );
                            healPokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                        }
                    } else if( !supprA
                               && ( pkmn->getAbility( ) == A_SNOW_CLOAK
                                    || pkmn->getAbility( ) == A_MAGIC_GUARD
                                    || pkmn->getAbility( ) == A_OVERCOAT ) ) {
                    } else if( canUseItem( i, j ) && pkmn->getItem( ) == I_SAFETY_GOGGLES ) {
                    } else if( hasType( i, j, TYPE_ICE ) ) {
                    } else {
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                    }
                }

                if( weather == WE_SANDSTORM ) {
                    if( hasType( i, j, TYPE_GROUND ) || hasType( i, j, TYPE_ROCK )
                        || hasType( i, j, TYPE_STEEL ) ) {
                    } else if( !supprA
                               && ( pkmn->getAbility( ) == A_SAND_FORCE
                                    || pkmn->getAbility( ) == A_SAND_RUSH
                                    || pkmn->getAbility( ) == A_SAND_VEIL
                                    || pkmn->getAbility( ) == A_MAGIC_GUARD
                                    || pkmn->getAbility( ) == A_OVERCOAT ) ) {
                    } else if( canUseItem( i, j ) && pkmn->getItem( ) == I_SAFETY_GOGGLES ) {
                    } else {
                        damagePokemon( p_ui, i, j, pkmn->m_stats.m_maxHP / 16 );
                    }
                }

                if( !getPkmn( i, j )->canBattle( ) ) {
                    // pkmn fainted
                    faintPokemon( p_ui, i, j );
                }
            }
        }
    }

    bool field::removeWeather( battleUI* p_ui ) {
        if( _weather != WE_NONE ) {
            p_ui->setNewWeather( WE_NONE );
        } else {
            return false;
        }
        _weather      = WE_NONE;
        _weatherTimer = 0;
        _paradoxRefresh( p_ui );
        return true;
    }

    bool field::setPseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather,
                                  bool p_extended ) {
        bool change = false;
        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( ( p_newPseudoWeather & ( 1LLU << i ) ) && _pseudoWeatherTimer[ i ]
                && ( ( 1LLU << i ) == PW_MAGICROOM || ( 1LLU << i ) == PW_WONDERROOM
                     || ( 1LLU << i ) == PW_TRICKROOM ) ) {
                removePseudoWeather( p_ui, pseudoWeather( 1LLU << i ) );
                change = true;
                continue;
            }
            if( ( p_newPseudoWeather & ( 1LLU << i ) ) && !_pseudoWeatherTimer[ i ] ) {
                change                   = true;
                _pseudoWeatherTimer[ i ] = p_extended ? EXTENDED_DURATION : NORMAL_DURATION;
                if( ( 1LLU << i ) == PW_FAIRYLOCK ) { _pseudoWeatherTimer[ i ] = 2; }
                p_ui->addPseudoWeather( i );
            }
        }
        if( !change ) { p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) ); }
        return change;
    }

    bool field::removePseudoWeather( battleUI* p_ui, pseudoWeather p_pseudoWeather ) {
        bool change = false;
        for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
            if( ( p_pseudoWeather & ( 1LLU << i ) ) && _pseudoWeatherTimer[ i ] ) {
                change                   = true;
                _pseudoWeatherTimer[ i ] = 0;
                p_ui->removePseudoWeather( i );
            }
        }
        return change;
    }

    bool field::setTerrain( battleUI* p_ui, terrain p_newTerrain, bool p_extended ) {
        if( _terrain == p_newTerrain ) {
            p_ui->log( GET_STRING( IO::STR_BATTLE_TERRAIN_NO_CHANGE ) );
            return false;
        }
        p_ui->setNewTerrain( p_newTerrain );
        _terrain      = p_newTerrain;
        _terrainTimer = p_extended ? EXTENDED_DURATION : NORMAL_DURATION;
        _paradoxRefresh( p_ui );
        return true;
    }

    bool field::removeTerrain( battleUI* p_ui ) {
        if( _terrain == TR_NONE ) { return false; }
        p_ui->setNewTerrain( _terrain = TR_NONE );
        _terrainTimer = 0;
        _paradoxRefresh( p_ui );
        return true;
    }

    void field::megaEvolve( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) [[unlikely]] { return; }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        auto         fmt = std::string( GET_STRING( IO::STR_BATTLE_MEGA_EVOLVE_WISH ) );
        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                  p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent ).c_str( ),
                  FS::getItemName( pkmn->getItem( ) ).c_str( ) );
        p_ui->log( std::string( buffer ) );

        pkmn->battleTransform( );
        pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) [[unlikely]] { return; }
        p_ui->updatePkmn( p_opponent, p_slot, getPkmnOrDisguise( p_opponent, p_slot ) );

        fmt = std::string( GET_STRING( IO::STR_BATTLE_MEGA_EVOLVE_WISH_GRANTED ) );
        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                  p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent ).c_str( ),
                  pkmn->getItem( ) );
        p_ui->log( std::string( buffer ) );

        checkOnSendOut( p_ui, p_opponent, p_slot );
    }

    bool field::canRecallPokemon( bool p_opponent ) {
        return _battle->canRecallPokemon( p_opponent );
    }

    std::vector<battleMove>
    field::computeSortedBattleMoves( battleUI*                               p_ui,
                                     const std::vector<battleMoveSelection>& p_selectedMoves ) {
        std::vector<battleMove> res = std::vector<battleMove>( );

        // randomly pertube scores slightly to break ties.
        u8 pertub[ 4 ] = { 0, 1, 2, 3 };
        for( u8 i = 0; i < 4; ++i ) { std::swap( pertub[ i ], pertub[ rand( ) % 4 ] ); }

        for( u8 j = 0; j < p_selectedMoves.size( ); ++j ) {
            battleMove bm;
            auto       m = p_selectedMoves[ j ];

            bm.m_param       = m.m_param;
            bm.m_user        = m.m_user;
            bm.m_target      = std::vector<fieldPosition>( );
            bm.m_pertubation = pertub[ j ];
            bm.m_moveData    = m.m_moveData;
            bm.m_megaEvolve  = m.m_megaEvolve;

            // compute targets, priority
            switch( m.m_type ) {
            case MT_ATTACK:
            case MT_SWITCH_PURSUIT: {
                if( m.m_type == MT_SWITCH_PURSUIT ) {
                    // Double the power of pursuit, ensure it happens before the switch
                    bm.m_priority = 123;
                    bm.m_moveData.m_basePower *= 2;
                } else {
                    bm.m_priority = 3 * m.m_moveData.m_priority;
                    bool haspd    = false;

                    // Check for priority changing abilities
                    if( !suppressesAbilities( )
                        && !slotAbilitiesNullified( m.m_user.first, m.m_user.second ) ) [[likely]] {
                        // Gale wings
                        if( m.m_moveData.m_type == TYPE_FLYING
                            && getPkmn( m.m_user.first, m.m_user.second )->getStat( HP )
                                   == getPkmn( m.m_user.first, m.m_user.second )->m_stats.m_maxHP
                            && getPkmn( m.m_user.first, m.m_user.second )->getAbility( )
                                   == A_GALE_WINGS ) [[unlikely]] {
                            bm.m_priority += 3;
                        }

                        // Prankster
                        if( m.m_moveData.m_category == MH_STATUS
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
                        if( getPkmn( m.m_user.first, m.m_user.second )->getAbility( ) == A_STALL ) {
                            bm.m_priority--;
                            haspd = true;
                        }
                    }

                    // Items

                    if( canUseItem( m.m_user.first, m.m_user.second ) ) {
                        if( !haspd
                            && getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                   == I_LAGGING_TAIL ) [[unlikely]] {
                            bm.m_priority--;
                        }

                        if( !haspd
                            && getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                   == I_FULL_INCENSE ) [[unlikely]] {
                            bm.m_priority--;
                        }

                        if( getPkmn( m.m_user.first, m.m_user.second )->getItem( ) == I_QUICK_CLAW )
                            [[unlikely]] {
                            if( rand( ) % 100 < 20 ) {
                                bm.m_priority++;
                                res.push_back( { MT_MESSAGE_ITEM,
                                                 I_QUICK_CLAW,
                                                 { },
                                                 { 255, 255 },
                                                 127,
                                                 0,
                                                 u8( 5 + j ),
                                                 moveData( ),
                                                 false } );
                            }
                        }

                        if( suppressesAbilities( )
                            || !_sides[ m.m_user.first ? PLAYER_SIDE : OPPONENT_SIDE ]
                                    .anyHasAbility( A_UNNERVE ) ) {
                            if( getPkmn( m.m_user.first, m.m_user.second )->getItem( )
                                    == I_CUSTAP_BERRY
                                && getPkmn( m.m_user.first, m.m_user.second )->getStat( HP ) * 4
                                       < getPkmn( m.m_user.first, m.m_user.second )
                                             ->m_stats.m_maxHP ) [[unlikely]] {
                                p_ui->logItem( getPkmnOrDisguise( m.m_user.first, m.m_user.second ),
                                               m.m_user.first );
                                bm.m_priority++;
                                res.push_back( { MT_MESSAGE_ITEM,
                                                 I_CUSTAP_BERRY,
                                                 { },
                                                 { m.m_user.first, m.m_user.second },
                                                 127,
                                                 0,
                                                 u8( 5 + j ),
                                                 moveData( ),
                                                 false } );
                                removeItem( p_ui, m.m_user.first, m.m_user.second );
                                checkOnEatBerry( p_ui, m.m_user.first, m.m_user.second,
                                                 I_CUSTAP_BERRY );
                            }
                        }
                    }

                    // moves w/ changing message

                    if( bm.m_param == M_FOCUS_PUNCH || bm.m_param == M_BEAK_BLAST
                        || bm.m_param == M_SHELL_TRAP || bm.m_param == M_COUNTER
                        || bm.m_param == M_MIRROR_COAT ) {
                        res.push_back( { MT_MESSAGE_MOVE,
                                         bm.m_param,
                                         { },
                                         { m.m_user.first, m.m_user.second },
                                         125,
                                         0,
                                         u8( 5 + j ),
                                         moveData( ),
                                         false } );
                    }
                }
                bm.m_type = MT_ATTACK;

                auto tg = ( m.m_moveData.m_pressureTarget != TG_NONE )
                              ? m.m_moveData.m_pressureTarget
                              : m.m_moveData.m_target;
                switch( tg ) {
                case TG_ANY:
                case TG_RANDOM:
                case TG_ALLY_OR_SELF:
                case TG_ANY_FOE:
                    // target is already computed
                    bm.m_target.push_back( m.m_target );
                    break;
                case TG_ALL_FOES_AND_ALLY:
                    if( getPkmn( m.m_user.first, !m.m_user.second ) ) {
                        bm.m_target.push_back( fieldPosition( m.m_user.first, !m.m_user.second ) );
                    }
                    [[fallthrough]];
                case TG_ALL_FOES:
                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        if( getPkmn( !m.m_user.first, i ) != nullptr ) {
                            bm.m_target.push_back( fieldPosition( !m.m_user.first, i ) );
                        }
                    }
                    break;
                case TG_ALL_ALLIES:
                    for( u8 i = 0; i < getBattlingPKMNCount( _mode ); ++i ) {
                        if( getPkmn( m.m_user.first, i ) != nullptr ) {
                            bm.m_target.push_back( fieldPosition( m.m_user.first, i ) );
                        }
                    }
                    break;
                case TG_SELF:
                    bm.m_target.push_back( fieldPosition( m.m_user.first, m.m_user.second ) );
                    break;
                case TG_ALLY:
                    bm.m_target.push_back( fieldPosition( m.m_user.first, !m.m_user.second ) );
                    break;
                case TG_FIELD:
                case TG_SCRIPTED:
                case TG_ALLY_SIDE:
                case TG_ALLY_TEAM:
                    bm.m_target.push_back( fieldPosition( m.m_user.first, m.m_user.second ) );
                    break;
                case TG_FOE_SIDE:
                    bm.m_target.push_back( fieldPosition( m.m_user.first, m.m_user.second ) );
                    break;
                default: break;
                }
                break;
            }
            case MT_SWITCH:
            case MT_USE_ITEM:
                bm.m_type = m.m_type;
                bm.m_target.push_back( m.m_user );
                bm.m_priority = 120;
                break;
            default:
                bm.m_type = MT_NO_OP;
                bm.m_target.push_back( m.m_user );
                bm.m_priority = -120;
                break;
            }

            if( bm.m_priority > -100 ) [[likely]] {
                if( getPseudoWeather( ) & PW_TRICKROOM ) {
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

        for( size_t i = 0; i < res.size( ); ++i ) {
            if( res[ i ].m_type != MT_ATTACK || res[ i ].m_target.empty( ) ) { continue; }
            for( size_t j = 0; j < res[ i ].m_target.size( ) && j < 8; ++j ) {
                bool targetActsAfter = false;
                for( size_t k = i + 1; k < res.size( ); ++k ) {
                    if( ( res[ k ].m_type == MT_ATTACK || res[ k ].m_type == MT_SWITCH
                          || res[ k ].m_type == MT_USE_ITEM )
                        && res[ k ].m_user == res[ i ].m_target[ j ] ) {
                        targetActsAfter = true;
                        break;
                    }
                }
                if( !targetActsAfter ) { res[ i ].m_actsAfter |= ( 1U << j ); }
            }
        }

        return res;
    }

    boosts field::addBoosts( bool p_opponent, u8 p_slot, boosts p_boosts, bool p_allowAbilities,
                             bool p_allowOverMax, u16 p_src ) {
        bool external = ( p_src == BOOST_SRC_HAZARD )
                        || ( p_src != BOOST_SRC_UNKNOWN && p_src != BOOST_SRC_MHCOPY
                             && p_src != packBoostSrc( { p_opponent, p_slot } ) );
        if( external ) [[unlikely]] {
            auto holder = getPkmn( p_opponent, p_slot );
            if( holder != nullptr && canUseItem( p_opponent, p_slot )
                && holder->getItem( ) == I_CLEAR_AMULET ) {
                for( u8 st = 0; st < NUM_BOOSTS; ++st ) {
                    if( p_boosts.getBoost( st ) < 0 ) { p_boosts.setBoost( st, 0 ); }
                }
            }
        }

        auto res = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addBoosts(
            p_slot, p_boosts, p_allowAbilities && !suppressesAbilities( ), p_allowOverMax );

        for( int herbSide = 0; herbSide < 2; ++herbSide ) {
            if( bool( herbSide ) == p_opponent ) { continue; } // same side as the boost
            for( u8 hs = 0; hs < 2; ++hs ) {
                auto herb = getPkmn( bool( herbSide ), hs );
                if( herb == nullptr || !herb->canBattle( ) ) { continue; }
                if( p_src == BOOST_SRC_MHCOPY || !canUseItem( bool( herbSide ), hs )
                    || herb->getItem( ) != I_MIRROR_HERB ) {
                    continue;
                }
                bool armed = _mhArmed[ herbSide ][ hs ];
                for( u8 st = 0; st < NUM_BOOSTS; ++st ) {
                    s8 delta = res.getBoost( st );
                    if( delta <= 0 ) { continue; }
                    s8 cur = _mhPending[ herbSide ][ hs ].getBoost( st );
                    _mhPending[ herbSide ][ hs ].setBoost(
                        st, s8( ( cur + delta > 6 ) ? 6 : cur + delta ) );
                    armed = true;
                }
                _mhArmed[ herbSide ][ hs ] = armed;
            }
        }
        return res;
    }

    bool field::sealedByImprison( bool p_opponent, u16 p_move ) {
        if( p_move == 0 || p_move == M_STRUGGLE ) { return false; }
        for( u8 s = 0; s < getBattlingPKMNCount( _mode ); ++s ) {
            if( !( getVolatileStatus( !p_opponent, s ) & VS_IMPRISON ) ) { continue; }
            auto holder = getPkmn( !p_opponent, s );
            if( holder == nullptr ) { continue; }
            for( u8 k = 0; k < 4; ++k ) {
                if( holder->getMove( k ) == p_move ) { return true; }
            }
        }
        return false;
    }

    bool field::canSelectMove( bool p_opponent, u8 p_slot, u8 p_moveIdx ) {
        if( getPseudoWeather( ) & PW_GRAVITY ) [[unlikely]] {
            auto pkmn = getPkmn( p_opponent, p_slot );
            if( pkmn != nullptr ) {
                auto mv = pkmn->getMove( p_moveIdx );
                if( mv && ( FS::getMoveData( mv ).m_flags & MF_GRAVITY ) ) [[unlikely]] {
                    return false;
                }
            }
        }
        {
            auto pkmn = getPkmn( p_opponent, p_slot );
            if( pkmn != nullptr && sealedByImprison( p_opponent, pkmn->getMove( p_moveIdx ) ) )
                [[unlikely]] {
                return false;
            }
        }

        return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSelectMove( p_slot,
                                                                                 p_moveIdx );
    }

    bool field::canSwitchOut( bool p_opponent, u8 p_slot ) {
        if( getPseudoWeather( ) & PW_FAIRYLOCK ) [[unlikely]] { return false; }

        if( !_sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSwitchOut( p_slot ) )
            [[unlikely]] {
            return false;
        }

        if( !suppressesAbilities( ) ) [[likely]] {
            auto p1 = getPkmn( !p_opponent, 0 );
            auto p2 = getPkmn( !p_opponent, 1 );
            if( getPkmn( p_opponent, p_slot )->getAbility( ) != A_SHADOW_TAG
                || slotAbilitiesNullified( p_opponent, p_slot ) ) [[likely]] {
                if( ( p1 != nullptr && !slotAbilitiesNullified( !p_opponent, 0 )
                      && p1->getAbility( ) == A_SHADOW_TAG )
                    || ( p2 != nullptr && !slotAbilitiesNullified( !p_opponent, 1 )
                         && p2->getAbility( ) == A_SHADOW_TAG ) ) [[unlikely]] {
                    return false;
                }
            }

            // Arena trap
            if( ( p1 != nullptr && p1->getAbility( ) == A_ARENA_TRAP )
                || ( p2 != nullptr && p2->getAbility( ) == A_ARENA_TRAP ) ) [[unlikely]] {
                if( isGrounded( p_opponent, p_slot ) ) [[likely]] { return false; }
            }

            // Magnet pull
            if( ( p1 != nullptr && p1->getAbility( ) == A_MAGNET_PULL )
                || ( p2 != nullptr && p2->getAbility( ) == A_MAGNET_PULL ) ) [[unlikely]] {
                if( hasType( p_opponent, p_slot, TYPE_STEEL ) ) [[unlikely]] { return false; }
            }
        }

        return true;
    }

    void field::moveItem( battleUI* p_ui, bool p_sourceOpp, u8 p_sorceSlot, bool p_targetOpp,
                          u8 p_targetSlot ) {
        auto tg = getPkmn( p_targetOpp, p_targetSlot );
        if( tg == nullptr ) [[unlikely]] { return; }
        auto sc = getPkmn( p_sourceOpp, p_sorceSlot );
        if( sc == nullptr ) [[unlikely]] { return; }

        u16 item = sc->getItem( );
        removeItem( p_ui, p_sourceOpp, p_sorceSlot, false );
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        // Check for move item effects
        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_PASS_ITEM_SPECIAL ) );

        switch( item ) {
        case I_CHILAN_BERRY2: {
            auto itemname = FS::getItemName( item );
            snprintf(
                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ),
                itemname.c_str( ), itemname.c_str( ) );
            p_ui->log( buffer );
            WAIT( HALF_SEC );
            boosts bt = boosts( );
            bt.setBoost( DEF, 7 );
            auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt, /*p_allowAbilities=*/true,
                                  /*p_allowOverMax=*/true );
            p_ui->logBoosts( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp,
                             p_sorceSlot, bt, res );
            return;
        }
        case I_EGGANT_BERRY: {
            auto itemname = FS::getItemName( item );
            snprintf(
                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ),
                itemname.c_str( ), itemname.c_str( ) );
            p_ui->log( buffer );
            WAIT( HALF_SEC );
            boosts bt = boosts( );
            bt.setBoost( SDEF, 7 );
            auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt, /*p_allowAbilities=*/true,
                                  /*p_allowOverMax=*/true );
            p_ui->logBoosts( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp,
                             p_sorceSlot, bt, res );
            return;
        }
        case I_NUTPEA_BERRY: {
            auto itemname = FS::getItemName( item );
            snprintf(
                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ),
                itemname.c_str( ), itemname.c_str( ) );
            p_ui->log( buffer );
            WAIT( HALF_SEC );
            boosts bt = boosts( );
            bt.setBoost( ATK, 7 );
            auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt, /*p_allowAbilities=*/true,
                                  /*p_allowOverMax=*/true );
            p_ui->logBoosts( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp,
                             p_sorceSlot, bt, res );
            return;
        }
        case I_STRIB_BERRY: {
            auto itemname = FS::getItemName( item );
            snprintf(
                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ),
                itemname.c_str( ), itemname.c_str( ) );
            p_ui->log( buffer );
            WAIT( HALF_SEC );
            boosts bt = boosts( );
            bt.setBoost( SATK, 7 );
            auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt, /*p_allowAbilities=*/true,
                                  /*p_allowOverMax=*/true );
            p_ui->logBoosts( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp,
                             p_sorceSlot, bt, res );
            return;
        }
        case I_YAGO_BERRY: {
            auto itemname = FS::getItemName( item );
            snprintf(
                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ),
                itemname.c_str( ), itemname.c_str( ) );
            p_ui->log( buffer );
            WAIT( HALF_SEC );
            boosts bt = boosts( );
            bt.setBoost( SPEED, 7 );
            auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt, /*p_allowAbilities=*/true,
                                  /*p_allowOverMax=*/true );
            p_ui->logBoosts( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp,
                             p_sorceSlot, bt, res );
            return;
        }
        case I_TOPO_BERRY: {
            auto itemname = FS::getItemName( item );
            snprintf(
                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ),
                itemname.c_str( ), itemname.c_str( ) );
            p_ui->log( buffer );
            WAIT( HALF_SEC );
            addVolatileStatus( p_ui, p_sourceOpp, p_sorceSlot, VS_PROTECT, 1 );
            snprintf(
                buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_PASS_ITEM_PROTECT_PKMN ),
                itemname.c_str( ),
                p_ui->getPkmnName( getPkmnOrDisguise( p_sourceOpp, p_sorceSlot ), p_sourceOpp )
                    .c_str( ) );
            WAIT( HALF_SEC );
            return;
        }
        [[likely]] default:
            break;
        }

        giveItem( p_ui, p_targetOpp, p_targetSlot, item );
    }

    void field::giveItem( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_item ) {
        _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].giveItem( p_slot, p_item );

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        auto         fmt = std::string( GET_STRING( IO::STR_BATTLE_NO_EFFECT_ON ) );
        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                  p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent ).c_str( ),
                  FS::getItemName( p_item ).c_str( ) );
        p_ui->log( buffer );
        WAIT( HALF_SEC );
    }

    void field::removeItem( battleUI* p_ui, bool p_opponent, u8 p_slot, bool p_used ) {
        _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeItem( p_slot, p_used );

        // Check for symbiosis
        auto p2 = getPkmn( p_opponent, !p_slot );
        if( p2 == nullptr ) [[likely]] { return; }

        if( !suppressesAbilities( ) && p_used && p2->getAbility( ) == A_SYMBIOSIS && p2->getItem( )
            && !slotAbilitiesNullified( p_opponent, u8( !p_slot ) ) ) {
            p_ui->logAbility( getPkmnOrDisguise( p_opponent, !p_slot ), p_opponent );
            moveItem( p_ui, p_opponent, !p_slot, p_opponent, p_slot );
        }
    }

    u16 field::getWeight( bool p_opponent, u8 p_slot, bool p_allowAbilities ) {
        bool ab = p_allowAbilities;
        u16  wg = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getWeight( p_slot, ab );

        if( canUseItem( p_opponent, p_slot, ab )
            && getPkmn( p_opponent, p_slot )->getItem( ) == I_FLOAT_STONE ) {
            wg >>= 1;
        }

        return wg;
    }

    bool field::isGrounded( bool p_opponent, u8 p_slot, bool p_allowAbilities ) {
        bool ab  = p_allowAbilities;
        bool grn = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].isGrounded( p_slot, ab );

        if( _pseudoWeatherTimer[ 4 ] ) [[unlikely]] { // gravity
            grn = true;
        }

        return grn;
    }
} // namespace BATTLE
