/*
Pokémon neo
------------------------------

file        : battleFieldSecondary.cpp
author      : Philip Wellnitz
description : secondary effects, contact, self-status, confusion, destiny bond and grudge

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
    void field::confusionSelfDamage( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return; }

        p_ui->animateHitPkmn( p_opponent, p_slot, 100 );
        // TODO: proper calculation(?)
        damagePokemon( p_ui, p_opponent, p_slot, pkmn->m_stats.m_maxHP / 16 );
    }

    void field::executeSelfStatusEffects( battleUI* p_ui, battleMove p_move,
                                          fieldPosition p_target ) {
        (void) p_target;
        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( user == nullptr ) [[unlikely]] { return; }

        if( p_move.m_param == M_CURSE ) {
            if( !hasType( p_move.m_user.first, p_move.m_user.second, TYPE_GHOST ) ) {
                boosts bs = boosts( );
                bs.setBoost( SPEED, -1 );
                bs.setBoost( ATK, 1 );
                bs.setBoost( DEF, 1 );
                auto res = addBoosts( p_move.m_user.first, p_move.m_user.second, bs );
                p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                 p_move.m_user.first, p_move.m_user.second, bs, res );
                return;
            }
        }

        // Boosts
        if( p_move.m_moveData.m_selfBoosts != boosts( ) ) {
            auto res = addBoosts( p_move.m_user.first, p_move.m_user.second,
                                  p_move.m_moveData.m_selfBoosts );
            p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                             p_move.m_user.first, p_move.m_user.second,
                             p_move.m_moveData.m_selfBoosts, res );
        }
    }

    bool field::isSoundBlockedForStatus( battleMove p_move, bool p_opponent, u8 p_slot ) {
        if( !( p_move.m_moveData.m_flags & MF_SOUND ) ) { return false; }
        bool abilities = !suppressesAbilities( ) && !slotAbilitiesNullified( p_opponent, p_slot )
                         && !( p_move.m_moveData.m_flags & MF_IGNOREABILITY );
        if( !abilities ) { return false; }
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) { return false; }
        return pkmn->getAbility( ) == A_SOUNDPROOF || pkmn->getAbility( ) == A_CACOPHONY;
    }

    bool field::windRiderSteal( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        if( !( p_move.m_moveData.m_flags & MF_WIND ) || p_move.m_user == p_target ) {
            return false;
        }
        if( p_move.m_moveData.m_flags & MF_IGNOREABILITY ) { return false; }
        if( suppressesAbilities( ) || slotAbilitiesNullified( p_target.first, p_target.second ) ) {
            return false;
        }
        auto target = getPkmn( p_target.first, p_target.second );
        if( target == nullptr || target->getAbility( ) != A_WIND_RIDER ) { return false; }
        if( ( getVolatileStatus( p_target.first, p_target.second ) & VS_SUBSTITUTE )
            && !( p_move.m_moveData.m_flags & MF_AUTHENTIC ) ) {
            return false;
        }

        boosts bt = boosts( );
        bt.setBoost( ATK, 1 );
        auto res = addBoosts( p_target.first, p_target.second, bt );
        if( res != boosts( ) ) {
            p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                              p_target.first );
            p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                             p_target.second, bt, res );
        } else {
            constexpr u8 TMP_BUFFER_SIZE = 100;
            char         buffer[ TMP_BUFFER_SIZE + 10 ];
            auto         fmt = std::string( GET_STRING( IO::STR_BATTLE_NO_EFFECT_ON ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, false )
                          .c_str( ) );
            p_ui->log( buffer );
        }
        return true;
    }

    void field::executeSecondaryStatus( battleUI* p_ui, battleMove p_move,
                                        fieldPosition p_target ) {
        if( substituteBlocks( p_move, p_target ) ) { return; }
        auto user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( p_move.m_moveData.m_target == TG_SELF ) {
            target   = user;
            p_target = p_move.m_user;
        }
        if( user == nullptr || target == nullptr ) [[unlikely]] { return; }
        // Check for status conditions
        if( p_move.m_moveData.m_secondaryStatus ) {
            bool corr = false;
            if( !suppressesAbilities( )
                && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                && user->getAbility( ) == A_CORROSION ) {
                corr = ( p_move.m_moveData.m_secondaryStatus == POISON );
            }
            if( gateExternalEffect( p_move, p_target, SC_SAFEGUARD ) ) {
                // empty
            } else if( terrainGuardsStatus( p_target, true,
                                            p_move.m_moveData.m_secondaryStatus == SLEEP, false,
                                            false, true ) ) {
                // empty
            } else if( p_move.m_moveData.m_secondaryStatus == SLEEP
                       && blockSleepByUproar( p_ui, p_target.first, p_target.second ) ) {
                // empty
            } else if( setStatusCondition(
                           p_target.first, p_target.second, p_move.m_moveData.m_secondaryStatus,
                           p_move.m_moveData.m_secondaryStatus == SLEEP ? ( 4 + ( rand( ) & 3 ) )
                                                                        : ( corr ? 254 : 255 ) ) ) {
                p_ui->animateGetStatusCondition(
                    getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                    p_target.second, p_move.m_moveData.m_secondaryStatus );
                p_ui->updatePkmnStats( p_target.first, p_target.second,
                                       getPkmnOrDisguise( p_target.first, p_target.second ), true );
            } else {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
        }

        if( p_move.m_moveData.m_secondaryVolatileStatus ) {
            u8   duration = 255;
            bool fail     = false;
            if( p_move.m_moveData.m_secondaryVolatileStatus & ( VS_PROTECT | VS_ENDURE ) ) {
                duration   = 1;
                auto lstmv = getLastUsedMove( p_target.first, p_target.second );
                if( lstmv.m_moveData.m_volatileStatus & ( VS_PROTECT | VS_ENDURE ) ) {
                    fail = ( rand( ) & 31 ) > 10;
                }
            }
            if( p_move.m_moveData.m_secondaryVolatileStatus & VS_FLINCH ) [[unlikely]] {
                duration = 1;
            }

            bool sgBlocked
                = ( p_move.m_moveData.m_secondaryVolatileStatus & ( VS_CONFUSION | VS_YAWN ) )
                  && gateExternalEffect( p_move, p_target, SC_SAFEGUARD );
            bool tgBlocked
                = !sgBlocked
                  && terrainGuardsStatus(
                      p_target, false, false, false,
                      ( p_move.m_moveData.m_secondaryVolatileStatus & VS_CONFUSION ) != 0, true );
            if( sgBlocked || tgBlocked ) {
            } else if( !fail
                       && addVolatileStatus( p_ui, p_target.first, p_target.second,
                                             p_move.m_moveData.m_secondaryVolatileStatus,
                                             duration ) ) {
                p_ui->animateGetVolatileStatusCondition(
                    getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                    p_target.second, p_move.m_moveData.m_secondaryVolatileStatus );
            } else {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
        }
    }

    void field::executeSecondaryEffects( battleUI* p_ui, battleMove p_move,
                                         fieldPosition p_target ) {
        if( substituteBlocks( p_move, p_target ) ) { return; }
        auto user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( p_move.m_moveData.m_target == TG_SELF ) {
            target   = user;
            p_target = p_move.m_user;
        }
        if( user == nullptr || target == nullptr ) [[unlikely]] { return; }

        // Boosts
        if( p_move.m_moveData.m_secondaryBoosts != boosts( ) ) {
            boosts applied = p_move.m_moveData.m_secondaryBoosts;
            mistStripsNegatives( p_move, p_target, applied );
            if( applied != boosts( ) ) {
                auto res = addBoosts( p_target.first, p_target.second, applied, true, false,
                                      packBoostSrc( p_move.m_user ) );
                p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                 p_target.first, p_target.second, applied, res );
            }
        }

        if( p_move.m_moveData.m_secondarySelfBoosts != boosts( ) ) {
            auto res = addBoosts( p_move.m_user.first, p_move.m_user.second,
                                  p_move.m_moveData.m_secondarySelfBoosts, true, false,
                                  packBoostSrc( p_move.m_user ) );
            p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                             p_move.m_user.first, p_move.m_user.second,
                             p_move.m_moveData.m_secondarySelfBoosts, res );
        }
    }

    void field::executeContact( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        auto  user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto& pkmn   = user;
        auto  target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return; }

        if( !suppressesAbilities( )
            && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
            && user->getAbility( ) == A_LONG_REACH ) {
            return;
        }
        if( canUseItem( p_move.m_user.first, p_move.m_user.second )
            && user->getItem( ) == I_PROTECTIVE_PADS ) {
            return;
        }
        if( canUseItem( p_move.m_user.first, p_move.m_user.second )
            && user->getItem( ) == I_PUNCHING_GLOVE && ( p_move.m_moveData.m_flags & MF_PUNCH ) ) {
            return;
        }

        auto tgvol = getVolatileStatus( p_target.first, p_target.second );

        if( tgvol & VS_SPIKYSHIELD ) {
            damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                           pkmn->m_stats.m_maxHP / 8 );
        }
        if( tgvol & VS_KINGSSHIELD ) {
            boosts bs = boosts( );
            bs.setBoost( ATK, -1 );
            auto res = addBoosts( p_move.m_user.first, p_move.m_user.second, bs, true, false,
                                  packBoostSrc( p_target ) );
            p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                             p_move.m_user.first, p_move.m_user.second, bs, res );
        }
        if( tgvol & VS_OBSTRUCT ) {
            boosts bs = boosts( );
            bs.setBoost( DEF, -2 );
            auto res = addBoosts( p_move.m_user.first, p_move.m_user.second, bs, true, false,
                                  packBoostSrc( p_target ) ); // external: the blocker
            p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                             p_move.m_user.first, p_move.m_user.second, bs, res );
        }
        if( tgvol & VS_BANEFULBUNKER ) {
            if( setStatusCondition( p_move.m_user.first, p_move.m_user.second, POISON ) ) {
                p_ui->animateGetStatusCondition(
                    getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                    p_move.m_user.first, p_move.m_user.second, POISON );
                p_ui->updatePkmnStats(
                    p_move.m_user.first, p_move.m_user.second,
                    getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
            }
        }
        if( tgvol & VS_BEAKBLAST ) {
            if( setStatusCondition( p_move.m_user.first, p_move.m_user.second, BURN ) ) {
                p_ui->animateGetStatusCondition(
                    getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                    p_move.m_user.first, p_move.m_user.second, BURN );
                p_ui->updatePkmnStats(
                    p_move.m_user.first, p_move.m_user.second,
                    getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
            }
        }

        if( !user->canBattle( ) ) { return; }

        // items

        if( canUseItem( p_target.first, p_target.second ) ) {
            switch( target->getItem( ) ) {
            case I_ROCKY_HELMET:
                damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                               pkmn->m_stats.m_maxHP / 6 );
                break;
            case I_STICKY_BARB:
                if( !user->getItem( )
                    && ( target->getAbility( ) != A_STICKY_HOLD
                         || slotAbilitiesNullified( p_target.first, p_target.second ) ) ) {
                    moveItem( p_ui, p_target.first, p_target.second, p_move.m_user.first,
                              p_move.m_user.second );
                }
                break;
            default: break;
            }
        }

        if( !user->canBattle( ) ) { return; }
        // abilities

        if( !suppressesAbilities( )
            && !slotAbilitiesNullified( p_target.first, p_target.second ) ) {
            switch( target->getAbility( ) ) {
            case A_WANDERING_SPIRIT: {
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                p_ui->logAbility( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first );
                target->setBattleTimeAbility( user->getAbility( ) );
                user->setBattleTimeAbility( A_WANDERING_SPIRIT );
                p_ui->logAbility( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first );
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                break;
            }
            case A_MUMMY: {
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                p_ui->logAbility( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first );
                user->setBattleTimeAbility( A_MUMMY );
                p_ui->logAbility( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first );
                break;
            }

            case A_PICKPOCKET: {
                if( user->getAbility( ) != A_STICKY_HOLD && !target->getItem( )
                    && user->getItem( ) ) {
                    p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                      p_target.first );
                    moveItem( p_ui, p_move.m_user.first, p_move.m_user.second, p_target.first,
                              p_target.second );
                }
                break;
            }

            case A_GOOEY:
            case A_TANGLING_HAIR: {
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                boosts bs = boosts( );
                bs.setBoost( SPEED, -1 );
                // the defender's ability is the external source
                auto res = addBoosts( p_move.m_user.first, p_move.m_user.second, bs, true, false,
                                      packBoostSrc( p_target ) );
                p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                 p_move.m_user.first, p_move.m_user.second, bs, res );
                break;
            }
            case A_ROUGH_SKIN:
            case A_IRON_BARBS: {
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                               pkmn->m_stats.m_maxHP / 8 );
                break;
            }
            case A_AFTERMATH: {
                if( !target->canBattle( ) ) {
                    p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                      p_target.first );
                    damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                   pkmn->m_stats.m_maxHP / 4 );
                }
                break;
            }
            case A_CUTE_CHARM: {
                if( !( rand( ) % 3 ) ) {
                    p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                      p_target.first );
                    addVolatileStatus( p_ui, p_move.m_user.first, p_move.m_user.second, VS_ATTRACT,
                                       255 );
                    p_ui->animateGetVolatileStatusCondition(
                        getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                        p_target.second, VS_ATTRACT );
                }
                break;
            }
            case A_FLAME_BODY: {
                if( !( rand( ) % 3 ) ) {
                    if( setStatusCondition( p_move.m_user.first, p_move.m_user.second, BURN ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                          p_target.first );
                        p_ui->animateGetStatusCondition(
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                            p_move.m_user.first, p_move.m_user.second, BURN );
                        p_ui->updatePkmnStats(
                            p_move.m_user.first, p_move.m_user.second,
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                    }
                }
                break;
            }
            case A_POISON_POINT: {
                if( !( rand( ) % 3 ) ) {
                    if( setStatusCondition( p_move.m_user.first, p_move.m_user.second, POISON ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                          p_target.first );
                        p_ui->animateGetStatusCondition(
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                            p_move.m_user.first, p_move.m_user.second, POISON );
                        p_ui->updatePkmnStats(
                            p_move.m_user.first, p_move.m_user.second,
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                    }
                }
                break;
            }
            case A_STATIC: {
                if( !( rand( ) % 3 ) ) {
                    if( setStatusCondition( p_move.m_user.first, p_move.m_user.second,
                                            PARALYSIS ) ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                          p_target.first );
                        p_ui->animateGetStatusCondition(
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                            p_move.m_user.first, p_move.m_user.second, PARALYSIS );
                        p_ui->updatePkmnStats(
                            p_move.m_user.first, p_move.m_user.second,
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                    }
                }
                break;
            }

            case A_EFFECT_SPORE: {
                if( !( rand( ) % 3 ) ) {
                    // TODO message
                    switch( rand( ) % 3 ) {
                    case 0:
                        if( setStatusCondition( p_move.m_user.first, p_move.m_user.second,
                                                BURN ) ) {
                            p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                              p_target.first );
                            p_ui->animateGetStatusCondition(
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                p_move.m_user.first, p_move.m_user.second, BURN );
                            p_ui->updatePkmnStats(
                                p_move.m_user.first, p_move.m_user.second,
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                        }
                        break;
                    case 1:
                        if( setStatusCondition( p_move.m_user.first, p_move.m_user.second,
                                                POISON ) ) {
                            p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                              p_target.first );
                            p_ui->animateGetStatusCondition(
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                p_move.m_user.first, p_move.m_user.second, POISON );
                            p_ui->updatePkmnStats(
                                p_move.m_user.first, p_move.m_user.second,
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                        }
                        break;
                    case 2:
                        if( setStatusCondition( p_move.m_user.first, p_move.m_user.second,
                                                PARALYSIS ) ) {
                            p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                              p_target.first );
                            p_ui->animateGetStatusCondition(
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                p_move.m_user.first, p_move.m_user.second, PARALYSIS );
                            p_ui->updatePkmnStats(
                                p_move.m_user.first, p_move.m_user.second,
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                        }
                        break;
                    default: break;
                    }
                }
                break;
            }

            case A_POISON_TOUCH: {
                if( !( rand( ) % 3 ) ) {
                    if( setStatusCondition( p_target.first, p_target.second, POISON ) ) {
                        p_ui->logAbility(
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                            p_move.m_user.first );
                        p_ui->animateGetStatusCondition(
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                            p_target.first, p_target.second, POISON );
                        p_ui->updatePkmnStats(
                            p_target.first, p_target.second,
                            getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                    }
                }
                break;
            }
            default: break;
            }
        }
    }

    void field::destinyBondTakeDown( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        if( p_move.m_user.first == p_target.first ) { return; }
        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( user == nullptr || !user->canBattle( ) ) { return; }
        faintPokemon( p_ui, p_move.m_user.first, p_move.m_user.second );
    }

    void field::grudgePayOff( battleUI* p_ui, battleMove p_move ) {
        constexpr u8 TMP_BUFFER_SIZE = 130;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( user == nullptr || !user->canBattle( ) ) { return; }
        if( !hasMove( p_move.m_user.first, p_move.m_user.second, p_move.m_param ) ) { return; }
        deducePP( p_move.m_user.first, p_move.m_user.second, p_move.m_param );
        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_GRUDGE_PP_LOST ) );
        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                  p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                     p_move.m_user.first )
                      .c_str( ),
                  FS::getMoveName( p_move.m_param ).c_str( ) );
        p_ui->log( buffer );
    }

} // namespace BATTLE
