/*
Pokémon neo
------------------------------

file        : battleFieldItems.cpp
author      : Philip Wellnitz
description : held-item triggers: berry eating, mirror herb, post-attack and on-take-damage item
effects

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
    void field::checkOnEatBerry( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_berry ) {
        (void) p_berry;

        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return; }
        if( !suppressesAbilities( ) ) {
            switch( pkmn->getAbility( ) ) {
            [[unlikely]] case A_CHEEK_POUCH:
                p_ui->logAbility( getPkmnOrDisguise( p_opponent, p_slot ), p_opponent );
                healPokemon( p_ui, p_opponent, p_slot, pkmn->m_stats.m_maxHP / 3 );
                break;
            [[likely]] default:
                break;
            }
        }
    }

    void field::_mirrorHerbConsume( battleUI* p_ui ) {
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        for( int side = 0; side < 2; ++side ) {
            for( u8 sl = 0; sl < 2; ++sl ) {
                if( !_mhArmed[ side ][ sl ] ) { continue; }
                auto   herb = getPkmn( bool( side ), sl );
                bool   ok = herb != nullptr && herb->canBattle( ) && canUseItem( bool( side ), sl )
                            && herb->getItem( ) == I_MIRROR_HERB;
                boosts copy              = _mhPending[ side ][ sl ];
                _mhArmed[ side ][ sl ]   = false;
                _mhPending[ side ][ sl ] = boosts( );
                if( !ok || copy == boosts( ) ) { continue; }

                p_ui->logItem( getPkmnOrDisguise( bool( side ), sl ), bool( side ) );
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_ACTIVATE_ITEM ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( bool( side ), sl ), bool( side ) )
                              .c_str( ),
                          FS::getItemName( herb->getItem( ) ).c_str( ) );
                p_ui->log( buffer );
                auto res = addBoosts( bool( side ), sl, copy, true, false, BOOST_SRC_MHCOPY );
                p_ui->logBoosts( getPkmnOrDisguise( bool( side ), sl ), bool( side ), sl, copy,
                                 res );
                removeItem( p_ui, bool( side ), sl );
            }
        }
    }

    void field::checkItemAfterAttack( battleUI* p_ui, bool p_opponent, u8 p_pos ) {
        auto pkmn = getPkmn( p_opponent, p_pos );
        if( pkmn == nullptr ) { return; }
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        bool items = canUseItem( p_opponent, p_pos );
        if( !items || !pkmn->getItem( ) ) { return; }

        bool supprAbs    = suppressesAbilities( );
        auto userVolStat = getVolatileStatus( p_opponent, p_pos );

        bool lowhptrigger = pkmn->m_stats.m_curHP * 4 <= pkmn->m_stats.m_maxHP;

        switch( pkmn->getItem( ) ) {
        case I_BERRY_JUICE:
            if( lowhptrigger ) {
                p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_DRINK_ITEM ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                              .c_str( ),
                          FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                p_ui->log( buffer );
                healPokemon( p_ui, p_opponent, p_pos, pkmn->m_stats.m_maxHP / 4 );
                removeItem( p_ui, p_opponent, p_pos );
                checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
            }
            break;
        case I_WHITE_HERB: {
            boosts bs = getBoosts( p_opponent, p_pos );
            if( bs.negative( ) != boosts( ) ) {
                bs       = bs.negative( ).invert( );
                auto res = addBoosts( p_opponent, p_pos, bs );
                if( res != boosts( ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_ACTIVATE_ITEM ) );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos, bs,
                                     res );
                    removeItem( p_ui, p_opponent, p_pos );
                }
            }
            break;
        }
        case I_MENTAL_HERB:
            if( userVolStat & VS_ATTRACT ) {
                p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_ACTIVATE_ITEM ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                              .c_str( ),
                          FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                p_ui->log( buffer );

                removeVolatileStatus( p_ui, p_opponent, p_pos, VS_ATTRACT );
                removeItem( p_ui, p_opponent, p_pos );
            }
            break;

        default: break;
        }

        if( supprAbs
            || !_sides[ p_opponent ? PLAYER_SIDE : OPPONENT_SIDE ].anyHasAbility( A_UNNERVE ) ) {

            if( !supprAbs && pkmn->getAbility( ) == A_GLUTTONY ) {
                lowhptrigger = pkmn->m_stats.m_curHP * 2 <= pkmn->m_stats.m_maxHP;
            }

            bool ripen = !supprAbs && pkmn->getAbility( ) == A_RIPEN;
            auto fmt   = std::string( GET_STRING( IO::STR_BATTLE_EAT_ITEM ) );

            switch( pkmn->getItem( ) ) {
            case I_NION_BERRY: {
                boosts bs = getBoosts( p_opponent, p_pos );
                if( bs.negative( ) != boosts( ) ) {
                    boosts tmp = bs.negative( ).invert( );
                    bs         = tmp;
                    if( ripen ) { bs.addBoosts( tmp ); };
                    auto res = addBoosts( p_opponent, p_pos, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         bs, res );
                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            case I_CHERI_BERRY:
                if( hasStatusCondition( p_opponent, p_pos, PARALYSIS ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeStatusCondition( p_opponent, p_pos );
                    p_ui->updatePkmnStats( p_opponent, p_pos,
                                           getPkmnOrDisguise( p_opponent, p_pos ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_CHESTO_BERRY:
                if( hasStatusCondition( p_opponent, p_pos, SLEEP ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeStatusCondition( p_opponent, p_pos );
                    p_ui->updatePkmnStats( p_opponent, p_pos,
                                           getPkmnOrDisguise( p_opponent, p_pos ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_DRASH_BERRY:
            case I_PECHA_BERRY:
                if( hasStatusCondition( p_opponent, p_pos, POISON ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeStatusCondition( p_opponent, p_pos );
                    p_ui->updatePkmnStats( p_opponent, p_pos,
                                           getPkmnOrDisguise( p_opponent, p_pos ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_CHRO_BERRY:
            case I_RAWST_BERRY:
                if( hasStatusCondition( p_opponent, p_pos, BURN ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeStatusCondition( p_opponent, p_pos );
                    p_ui->updatePkmnStats( p_opponent, p_pos,
                                           getPkmnOrDisguise( p_opponent, p_pos ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_PUMKIN_BERRY:
            case I_ASPEAR_BERRY:
                if( hasStatusCondition( p_opponent, p_pos, FROZEN ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeStatusCondition( p_opponent, p_pos );
                    p_ui->updatePkmnStats( p_opponent, p_pos,
                                           getPkmnOrDisguise( p_opponent, p_pos ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_ORAN_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos, 10 * ( 1 + ripen ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_RIE_BERRY:
            case I_PERSIM_BERRY:
                if( userVolStat & VS_CONFUSION ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    fmt = std::string( GET_STRING( IO::STR_BATTLE_EAT_ITEM_HEAL_CONFUSION ) );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );

                    removeVolatileStatus( p_ui, p_opponent, p_pos, VS_CONFUSION );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_GARC_BERRY:
                if( userVolStat & VS_ATTRACT ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    fmt = std::string( GET_STRING( IO::STR_BATTLE_EAT_ITEM ) );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );

                    removeVolatileStatus( p_ui, p_opponent, p_pos, VS_ATTRACT );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_LUM_BERRY:
                if( pkmn->m_statusint || ( userVolStat & VS_CONFUSION ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeStatusCondition( p_opponent, p_pos );
                    removeVolatileStatus( p_ui, p_opponent, p_pos, VS_CONFUSION );
                    p_ui->updatePkmnStats( p_opponent, p_pos,
                                           getPkmnOrDisguise( p_opponent, p_pos ) );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_SITRUS_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos,
                                 pkmn->m_stats.m_maxHP * ( 1 + ripen ) / 4 );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_FIGY_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos,
                                 pkmn->m_stats.m_maxHP * ( 1 + ripen ) / 4 );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_WIKI_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos,
                                 pkmn->m_stats.m_maxHP * ( 1 + ripen ) / 4 );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_MAGO_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos,
                                 pkmn->m_stats.m_maxHP * ( 1 + ripen ) / 4 );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_AGUAV_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos,
                                 pkmn->m_stats.m_maxHP * ( 1 + ripen ) / 4 );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_IAPAPA_BERRY:
                if( lowhptrigger ) {
                    p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                            .c_str( ),
                        FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    healPokemon( p_ui, p_opponent, p_pos,
                                 pkmn->m_stats.m_maxHP * ( 1 + ripen ) / 4 );
                    removeItem( p_ui, p_opponent, p_pos );
                    checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                }
                break;
            case I_LIECHI_BERRY: {
                if( lowhptrigger ) {
                    boosts bs = boosts( );
                    bs.setBoost( ATK, 3 * ( 1 + ripen ) );

                    auto res = addBoosts( p_opponent, p_pos, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         bs, res );
                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            case I_GANLON_BERRY: {
                if( lowhptrigger ) {
                    boosts bs = boosts( );
                    bs.setBoost( DEF, 3 * ( 1 + ripen ) );

                    auto res = addBoosts( p_opponent, p_pos, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         bs, res );
                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            case I_SALAC_BERRY: {
                if( lowhptrigger ) {
                    boosts bs = boosts( );
                    bs.setBoost( SPEED, 3 * ( 1 + ripen ) );

                    auto res = addBoosts( p_opponent, p_pos, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         bs, res );
                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            case I_PETAYA_BERRY: {
                if( lowhptrigger ) {
                    boosts bs = boosts( );
                    bs.setBoost( SATK, 3 * ( 1 + ripen ) );

                    auto res = addBoosts( p_opponent, p_pos, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         bs, res );
                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            case I_APICOT_BERRY: {
                if( lowhptrigger ) {
                    boosts bs = boosts( );
                    bs.setBoost( SDEF, 3 * ( 1 + ripen ) );

                    auto res = addBoosts( p_opponent, p_pos, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );
                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         bs, res );
                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            case I_STARF_BERRY: {
                if( lowhptrigger ) {
                    boosts bs  = getBoosts( p_opponent, p_pos );
                    boosts res = boosts( );
                    u8     rs  = 1 + rand( ) % 5;

                    bool gd = false;
                    for( u8 i2 = 1; i2 < 6; ++i2 ) {
                        if( bs.getBoost( i2 ) < 6 ) { gd = true; }
                    }
                    while( gd && bs.getBoost( rs ) >= 6 ) { rs = 1 + rand( ) % 5; }
                    res.setBoost( rs, 3 * ( 1 + ripen ) );

                    auto res2 = addBoosts( p_opponent, p_pos, res );
                    if( res2 != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent )
                                .c_str( ),
                            FS::getItemName( pkmn->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        p_ui->logBoosts( getPkmnOrDisguise( p_opponent, p_pos ), p_opponent, p_pos,
                                         res, res2 );

                        removeItem( p_ui, p_opponent, p_pos );
                        checkOnEatBerry( p_ui, p_opponent, p_pos, pkmn->getItem( ) );
                    }
                }
                break;
            }
            [[likely]] default:
                break;
            }
        }
        return;
    }

    void field::checkOnTakeDamage( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                   u16 p_damage, u8 p_effectiveness ) {
        if( !p_damage ) { return; }

        auto user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return; }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        bool         supprAbs = suppressesAbilities( );

        // Check for disguise of A_ILLUSION that breaks
        if( p_damage ) {
            if( getSlot( p_target.first, p_target.second )->isDisguised( ) ) {
                setSlotDisguise( p_target.first, p_target.second, nullptr );
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                p_ui->updatePkmn( p_target.first, p_target.second,
                                  getPkmnOrDisguise( p_target.first, p_target.second ) );
            }
        }

        // Check for M_FOCUS_PUNCH and M_SHELL_TRAP
        auto tgvol = getVolatileStatus( p_target.first, p_target.second );
        if( p_damage && ( tgvol & VS_FOCUSPUNCH ) ) {
            removeVolatileStatus( p_ui, p_target.first, p_target.second, VS_FOCUSPUNCH );
        }
        if( p_move.m_moveData.m_category == MH_PHYSICAL && p_damage && ( tgvol & VS_SHELLTRAP ) ) {
            removeVolatileStatus( p_ui, p_target.first, p_target.second, VS_SHELLTRAP );
        }

        if( p_move.m_user.first != p_target.first ) {
            auto cat = p_move.m_moveData.m_category;
            if( cat == MH_PHYSICAL && hasCounter( p_target.first, p_target.second ) ) {
                recordCounter( p_target.first, p_target.second, false, u16( p_damage * 2 ),
                               p_move.m_user );
            } else if( cat == MH_SPECIAL && hasMirrorCoat( p_target.first, p_target.second ) ) {
                recordCounter( p_target.first, p_target.second, true, u16( p_damage * 2 ),
                               p_move.m_user );
            }
        }

        if( canUseItem( p_target.first, p_target.second ) ) {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_ACTIVATE_ITEM ) );
            switch( target->getItem( ) ) {
            case I_WEAKNESS_POLICY:
                if( p_effectiveness > 100 && target->canBattle( )
                    && target->m_stats.m_curHP < target->m_stats.m_maxHP ) {
                    boosts bs = boosts( );
                    bs.setBoost( ATK, 2 );
                    bs.setBoost( SATK, 2 );

                    auto res = addBoosts( p_target.first, p_target.second, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, p_target.second, bs, res );

                        removeItem( p_ui, p_target.first, p_target.second );
                    }
                }
                break;
            case I_SNOWBALL:
                if( p_move.m_moveData.m_type == TYPE_ICE && target->canBattle( )
                    && target->m_stats.m_curHP < target->m_stats.m_maxHP ) {
                    boosts bs = boosts( );
                    bs.setBoost( ATK, 1 );

                    auto res = addBoosts( p_target.first, p_target.second, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, p_target.second, bs, res );

                        removeItem( p_ui, p_target.first, p_target.second );
                    }
                }
                break;
            case I_ABSORB_BULB:
                if( p_move.m_moveData.m_type == TYPE_WATER && target->canBattle( )
                    && target->m_stats.m_curHP < target->m_stats.m_maxHP ) {
                    boosts bs = boosts( );
                    bs.setBoost( SATK, 1 );

                    auto res = addBoosts( p_target.first, p_target.second, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, p_target.second, bs, res );

                        removeItem( p_ui, p_target.first, p_target.second );
                    }
                }
                break;

            case I_LUMINOUS_MOSS:
                if( p_move.m_moveData.m_type == TYPE_WATER && target->canBattle( )
                    && target->m_stats.m_curHP < target->m_stats.m_maxHP ) {
                    boosts bs = boosts( );
                    bs.setBoost( SDEF, 1 );

                    auto res = addBoosts( p_target.first, p_target.second, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, p_target.second, bs, res );

                        removeItem( p_ui, p_target.first, p_target.second );
                    }
                }
                break;
            case I_CELL_BATTERY:
                if( p_move.m_moveData.m_type == TYPE_LIGHTNING && target->canBattle( )
                    && target->m_stats.m_curHP < target->m_stats.m_maxHP ) {
                    boosts bs = boosts( );
                    bs.setBoost( ATK, 1 );

                    auto res = addBoosts( p_target.first, p_target.second, bs );
                    if( res != boosts( ) ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, p_target.second, bs, res );

                        removeItem( p_ui, p_target.first, p_target.second );
                    }
                }
                break;

            case I_AIR_BALLOON: {
                fmt = std::string( GET_STRING( IO::STR_BATTLE_ACTIVATE_AIR_BALLON ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( buffer );
                removeItem( p_ui, p_target.first, p_target.second );
                break;
            }
            case I_EJECT_BUTTON: {
                if( canRecallPokemon( p_target.first ) ) {
                    p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                   p_target.first );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ),
                        FS::getItemName( target->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );
                    removeItem( p_ui, p_target.first, p_target.second );
                    recallPokemon( p_ui, p_target.first, p_target.second );
                }
                break;
            }
            default: break;
            }
        }

        // berries
        if( supprAbs
            || !_sides[ !p_move.m_user.first ? PLAYER_SIDE : OPPONENT_SIDE ].anyHasAbility(
                A_UNNERVE ) ) {
            if( canUseItem( p_target.first, p_target.second ) ) {
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_EAT_ITEM ) );
                switch( target->getItem( ) ) {
                case I_ENIGMA_BERRY:
                    if( p_effectiveness > 100 && target->canBattle( )
                        && target->m_stats.m_curHP < target->m_stats.m_maxHP ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        if( supprAbs || target->getAbility( ) != A_RIPEN ) {
                            healPokemon( p_ui, p_target.first, p_target.second,
                                         target->m_stats.m_maxHP / 4 );
                        } else {
                            healPokemon( p_ui, p_target.first, p_target.second,
                                         target->m_stats.m_maxHP / 2 );
                        }

                        removeItem( p_ui, p_target.first, p_target.second );
                        checkOnEatBerry( p_ui, p_target.first, p_target.second,
                                         target->getItem( ) );
                    }
                    break;
                case I_JABOCA_BERRY:
                    if( user->canBattle( ) && p_move.m_moveData.m_category == MH_PHYSICAL ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        if( supprAbs || target->getAbility( ) != A_RIPEN ) {
                            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                           target->m_stats.m_maxHP / 8 );
                        } else {
                            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                           target->m_stats.m_maxHP / 4 );
                        }

                        removeItem( p_ui, p_target.first, p_target.second );
                        checkOnEatBerry( p_ui, p_target.first, p_target.second,
                                         target->getItem( ) );
                    }
                    break;
                case I_KEE_BERRY:
                    if( user->canBattle( ) && p_move.m_moveData.m_category == MH_PHYSICAL ) {
                        boosts bs = boosts( );
                        if( supprAbs || target->getAbility( ) != A_RIPEN ) {
                            bs.setBoost( DEF, 1 );
                        } else {
                            bs.setBoost( DEF, 2 );
                        }

                        auto res = addBoosts( p_target.first, p_target.second, bs );
                        if( res != boosts( ) ) {
                            p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first );
                            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                      p_ui->getPkmnName(
                                              getPkmnOrDisguise( p_target.first, p_target.second ),
                                              p_target.first )
                                          .c_str( ),
                                      FS::getItemName( target->getItem( ) ).c_str( ) );
                            p_ui->log( buffer );

                            p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first, p_target.second, bs, res );

                            removeItem( p_ui, p_target.first, p_target.second );
                            checkOnEatBerry( p_ui, p_target.first, p_target.second,
                                             target->getItem( ) );
                        }
                    }
                    break;
                case I_MARANGA_BERRY:
                    if( user->canBattle( ) && p_move.m_moveData.m_category == MH_SPECIAL ) {
                        boosts bs = boosts( );
                        if( supprAbs || target->getAbility( ) != A_RIPEN ) {
                            bs.setBoost( SDEF, 1 );
                        } else {
                            bs.setBoost( SDEF, 2 );
                        }

                        auto res = addBoosts( p_target.first, p_target.second, bs );
                        if( res != boosts( ) ) {
                            p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first );
                            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                      p_ui->getPkmnName(
                                              getPkmnOrDisguise( p_target.first, p_target.second ),
                                              p_target.first )
                                          .c_str( ),
                                      FS::getItemName( target->getItem( ) ).c_str( ) );
                            p_ui->log( buffer );

                            p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first, p_target.second, bs, res );

                            removeItem( p_ui, p_target.first, p_target.second );
                            checkOnEatBerry( p_ui, p_target.first, p_target.second,
                                             target->getItem( ) );
                        }
                    }
                    break;

                case I_ROWAP_BERRY:
                    if( user->canBattle( ) && p_move.m_moveData.m_category == MH_SPECIAL ) {
                        p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                       p_target.first );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                               p_target.first )
                                .c_str( ),
                            FS::getItemName( target->getItem( ) ).c_str( ) );
                        p_ui->log( buffer );

                        if( supprAbs || target->getAbility( ) != A_RIPEN ) {
                            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                           target->m_stats.m_maxHP / 8 );
                        } else {
                            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                           target->m_stats.m_maxHP / 4 );
                        }

                        removeItem( p_ui, p_target.first, p_target.second );
                        checkOnEatBerry( p_ui, p_target.first, p_target.second,
                                         target->getItem( ) );
                    }
                    break;

                default: break;
                }
            }
        }
    }

} // namespace BATTLE
