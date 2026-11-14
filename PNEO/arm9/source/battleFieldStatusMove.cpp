/*
Pokémon neo
------------------------------

file        : battleFieldStatusMove.cpp
author      : Philip Wellnitz
description : status-move execution: the full MOVE_EFFECT switch

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
    void field::executeStatusEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        auto user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( p_move.m_moveData.m_target == TG_SELF || p_move.m_moveData.m_target == TG_FIELD
            || p_move.m_moveData.m_target == TG_FOE_SIDE
            || p_move.m_moveData.m_target == TG_ALLY_SIDE ) {
            target   = user;
            p_target = p_move.m_user;
        }
        if( user == nullptr || target == nullptr ) [[unlikely]] { return; }

        if( p_move.m_param == M_SLEEP_TALK ) [[unlikely]] {
            bool opponent = p_move.m_user.first;
            u8   slot     = p_move.m_user.second;
            bool asleep   = hasStatusCondition( opponent, slot, SLEEP );
            bool comatose = !asleep && !suppressesAbilities( )
                            && !slotAbilitiesNullified( opponent, slot )
                            && user->getAbility( ) == A_COMATOSE;
            if( !asleep && !comatose ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return;
            }

            u16 pool[ 4 ] = { 0, 0, 0, 0 };
            u8  n         = 0;
            for( u8 i = 0; i < 4; ++i ) {
                u16 id = user->m_boxdata.m_moves[ i ];
                if( id == 0 || sleepTalkExcluded( id ) ) { continue; }
                if( FS::getMoveData( id ).m_flags & MF_CHARGE ) { continue; }
                pool[ n++ ] = id;
            }
            if( n == 0 ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return;
            }

            u16        chosen = pool[ rand( ) % n ];
            moveData   md     = FS::getMoveData( chosen );
            battleMove nested = { };
            nested.m_type     = MT_ATTACK;
            nested.m_param    = chosen;
            nested.m_user     = p_move.m_user;
            nested.m_priority = md.m_priority;
            nested.m_moveData = md;
            switch( md.m_target ) {
            case TG_SELF:
            case TG_ALLY_OR_SELF:
            case TG_ALL_ALLIES: nested.m_target.push_back( p_move.m_user ); break;
            case TG_ANY:
            case TG_ANY_FOE:
            case TG_ALL_FOES:
            case TG_ALL_FOES_AND_ALLY:
            case TG_RANDOM: nested.m_target.push_back( { u8( !opponent ), 0 } ); break;
            default: break;
            }
            bool outer       = _nestedCallsMove;
            _nestedCallsMove = true;
            executeBattleMove( p_ui, nested, std::vector<battleMove>( ),
                               std::vector<battleMove>( ) );
            _nestedCallsMove = outer;
            return;
        }

        if( p_move.m_param == M_MIRROR_MOVE ) [[unlikely]] {
            battleMove last = getLastUsedMove( p_target.first, p_target.second );
            moveData   md   = FS::getMoveData( last.m_param );
            if( !last.m_param || !( md.m_flags & MF_MIRROR ) ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return;
            }
            battleMove nested = { };
            nested.m_type     = MT_ATTACK;
            nested.m_param    = last.m_param;
            nested.m_user     = p_move.m_user;
            nested.m_priority = md.m_priority;
            nested.m_moveData = md;
            switch( md.m_target ) {
            case TG_SELF:
            case TG_ALLY_OR_SELF:
            case TG_ALL_ALLIES: nested.m_target.push_back( p_move.m_user ); break;
            case TG_ANY:
            case TG_ANY_FOE:
            case TG_ALL_FOES:
            case TG_ALL_FOES_AND_ALLY:
            case TG_RANDOM: nested.m_target.push_back( p_target ); break;
            default: break;
            }
            bool outer       = _nestedCallsMove;
            _nestedCallsMove = true;
            executeBattleMove( p_ui, nested, std::vector<battleMove>( ),
                               std::vector<battleMove>( ) );
            _nestedCallsMove = outer;
            return;
        }

        if( substituteBlocks( p_move, p_target ) ) [[unlikely]] {
            p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            return;
        }

        if( p_move.m_param == M_SUBSTITUTE ) {
            const u16 maxHp = target->m_stats.m_maxHP;
            const u16 hp    = target->m_stats.m_curHP;
            if( getVolatileStatus( p_target.first, p_target.second ) & VS_SUBSTITUTE ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else if( maxHp == 1 || u32( hp ) * 4 <= u32( maxHp ) ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else {
                const u16 cost = substituteHpFor( maxHp );
                addVolatileStatus( p_ui, p_target.first, p_target.second, VS_SUBSTITUTE, 255 );
                setSubstituteHp( p_target.first, p_target.second, cost );
                p_ui->animateGetVolatileStatusCondition(
                    getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                    p_target.second, VS_SUBSTITUTE );
                removeVolatileStatus( p_ui, p_target.first, p_target.second,
                                      volatileStatus( VS_PARTIALLYTRAPPED | VS_OCTOLOCK ) );
                damagePokemon( p_ui, p_target.first, p_target.second, cost );
                p_ui->updatePkmnStats( p_target.first, p_target.second,
                                       getPkmnOrDisguise( p_target.first, p_target.second ) );
            }
            return;
        }

        if( p_move.m_param == M_CURSE ) {
            if( hasType( p_move.m_user.first, p_move.m_user.second, TYPE_GHOST ) ) {
                damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                               user->m_stats.m_maxHP / 2 );
            } else {
                return;
            }
        }

        if( p_move.m_param == M_TRANSFORM ) {
            if( transformPkmn( p_move.m_user.first, p_move.m_user.second,
                               getSlot( p_target.first, p_target.second ) ) ) {
                p_ui->updatePkmn( p_move.m_user.first, p_move.m_user.second,
                                  getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ) );
                if( getPkmn( p_move.m_user.first, p_move.m_user.second )->getAbility( )
                    != A_IMPOSTER ) {
                    checkOnSendOut( p_ui, p_move.m_user.first, p_move.m_user.second );
                }
            } else {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
            return;
        }

        if( p_move.m_param == M_REST ) {
            if( getVolatileStatus( p_target.first, p_target.second ) & VS_HEALBLOCK ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else {
                if( !blockSleepByUproar( p_ui, p_move.m_user.first, p_move.m_user.second ) ) {
                    user->m_status.m_isAsleep = 3;
                    p_ui->animateGetStatusCondition(
                        getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                        p_target.second, p_move.m_moveData.m_status );
                }
                p_ui->updatePkmnStats( p_target.first, p_target.second,
                                       getPkmnOrDisguise( p_target.first, p_target.second ) );
                healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                             user->m_stats.m_maxHP );
            }
            return;
        }

        if( p_move.m_param == M_AUTOTOMIZE ) {
            auto slA = getSlot( p_move.m_user.first, p_move.m_user.second );
            bool contrary
                = ( slA != nullptr && slA->hasAbility( A_CONTRARY ) ); // folds Gastro Acid
            s8 spe = slA ? slA->getBoosts( ).getBoost( SPEED ) : 0;
            if( ( !contrary && spe == 6 ) || ( contrary && spe == -6 ) ) [[unlikely]] {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return;
            }
        }

        if( p_move.m_moveData.m_boosts != boosts( ) ) {
            boosts applied = p_move.m_moveData.m_boosts;
            if( mistStripsNegatives( p_move, p_target, applied ) ) [[unlikely]] {
                auto fmtM = std::string( GET_STRING( IO::STR_BATTLE_MIST_PROTECTS ) );
                char bufM[ 110 ];
                snprintf( bufM, sizeof( bufM ), fmtM.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( bufM );
            }
            if( applied != boosts( ) ) {
                auto res = addBoosts( p_target.first, p_target.second, applied, true, false,
                                      packBoostSrc( p_move.m_user ) );
                p_ui->logBoosts( getPkmnOrDisguise( p_target.first, p_target.second ),
                                 p_target.first, p_target.second, applied, res );
            }
        }

        if( p_move.m_param == M_AUTOTOMIZE ) {
            auto slA = getSlot( p_move.m_user.first, p_move.m_user.second );
            if( slA != nullptr && getWeight( p_move.m_user.first, p_move.m_user.second ) > 1 )
                [[likely]] {
                slA->addAutotomizeCut( );
                auto fmtA = std::string( GET_STRING( IO::STR_BATTLE_BECAME_NIMBLE ) );
                char bufA[ 110 ];
                snprintf( bufA, sizeof( bufA ), fmtA.c_str( ),
                          p_ui->getPkmnName(
                                  getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first )
                              .c_str( ) );
                p_ui->log( bufA );
            }
        }

        // Heal
        if( p_move.m_moveData.m_heal ) {
            if( getVolatileStatus( p_target.first, p_target.second ) & VS_HEALBLOCK ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else {
                u16 amount = target->m_stats.m_maxHP * p_move.m_moveData.m_heal / 240;
                if( !amount ) { amount = 1; }
                healPokemon( p_ui, p_target.first, p_target.second, amount );
            }
        }
        if( p_move.m_param == M_SYNTHESIS || p_move.m_param == M_MOONLIGHT
            || p_move.m_param == M_MORNING_SUN ) {
            if( getVolatileStatus( p_target.first, p_target.second ) & VS_HEALBLOCK ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else {
                u16 amount = 1;
                switch( getWeather( ) ) {
                case WE_SUN:
                case WE_HEAVY_SUNSHINE: amount = target->m_stats.m_maxHP * 2 / 3; break;
                case WE_NONE:
                case WE_HEAVY_WINDS: amount = target->m_stats.m_maxHP / 2; break;
                default: amount = target->m_stats.m_maxHP / 4; break;
                }
                if( !amount ) { amount = 1; }
                healPokemon( p_ui, p_target.first, p_target.second, amount );
            }
        }
        if( p_move.m_param == M_SHORE_UP ) {
            if( getVolatileStatus( p_target.first, p_target.second ) & VS_HEALBLOCK ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else {
                u16 amount = 1;
                switch( getWeather( ) ) {
                case WE_SANDSTORM: amount = target->m_stats.m_maxHP * 2 / 3; break;
                default: amount = target->m_stats.m_maxHP / 2; break;
                }
                if( !amount ) { amount = 1; }
                healPokemon( p_ui, p_target.first, p_target.second, amount );
            }
        }
        if( p_move.m_param == M_PURIFY ) {
            if( removeVolatileStatus( p_ui, p_target.first, p_target.second,
                                      volatileStatus( u64( -1 ) ) ) ) {
                if( !( getVolatileStatus( p_move.m_user.first, p_move.m_user.second )
                       & VS_HEALBLOCK ) ) {
                    u16 amount = user->m_stats.m_maxHP / 2;
                    if( !amount ) { amount = 1; }
                    healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );
                }
            } else {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
        }
        if( p_move.m_param == M_STRENGTH_SAP ) {
            u16 amount = getStat( p_target.first, p_target.second, ATK );
            if( canUseItem( p_move.m_user.first, p_move.m_user.second )
                && user->getItem( ) == I_BIG_ROOT ) {
                amount = amount * 13 / 10;
            }
            if( !amount ) { amount = 1; }
            if( !suppressesAbilities( )
                && !slotAbilitiesNullified( p_target.first, p_target.second )
                && target->getAbility( ) == A_LIQUID_OOZE ) {
                p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                  p_target.first );
                damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );
            } else if( !( getVolatileStatus( p_move.m_user.first, p_move.m_user.second )
                          & VS_HEALBLOCK ) ) {
                healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );
            }
        }

        // Check for status conditions
        if( p_move.m_moveData.m_status ) {
            bool corr = false;
            if( !suppressesAbilities( )
                && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                && user->getAbility( ) == A_CORROSION ) {
                corr = ( p_move.m_moveData.m_secondaryStatus == POISON );
            }

            if( gateExternalEffect( p_move, p_target, SC_SAFEGUARD ) ) [[unlikely]] {
                auto fmtS = std::string( GET_STRING( IO::STR_BATTLE_SAFEGUARD_PROTECTS ) );
                char bufS[ 110 ];
                snprintf( bufS, sizeof( bufS ), fmtS.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( bufS );
            } else if( u8 tg = terrainGuardsStatus(
                           p_target, true, p_move.m_moveData.m_status == SLEEP ) ) [[unlikely]] {
                if( tg == 2 ) {
                    auto fmtT = std::string( GET_STRING( IO::STR_BATTLE_TERRAIN_PROTECTS ) );
                    char bufT[ 110 ];
                    snprintf(
                        bufT, sizeof( bufT ), fmtT.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ) );
                    p_ui->log( bufT );
                }
            } else if( p_move.m_moveData.m_status == SLEEP
                       && blockSleepByUproar( p_ui, p_target.first, p_target.second ) ) {
            } else if( setStatusCondition(
                           p_target.first, p_target.second, p_move.m_moveData.m_status,
                           p_move.m_moveData.m_status == SLEEP ? ( 4 + ( rand( ) & 3 ) )
                                                               : ( corr ? 254 : 255 ) ) ) {
                p_ui->animateGetStatusCondition(
                    getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                    p_target.second, p_move.m_moveData.m_status );
                p_ui->updatePkmnStats( p_target.first, p_target.second,
                                       getPkmnOrDisguise( p_target.first, p_target.second ), true );
            } else {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
        }

        if( p_move.m_moveData.m_sideCondition ) {
            sideCondition cond     = p_move.m_moveData.m_sideCondition;
            bool          stacking = cond & ( SC_SPIKES | SC_TOXICSPIKES );
            bool          tg     = p_move.m_moveData.m_target == TG_FOE_SIDE ? !p_move.m_user.first
                                                                             : p_move.m_user.first;
            u8            layers = sideConditionAmount( tg, cond );
            bool tailwindNew = ( cond & SC_TAILWIND ) && !( getSideCondition( tg ) & SC_TAILWIND );
            addSideCondition( p_ui, tg, cond, 0, stacking );
            if( sideConditionAmount( tg, cond ) > layers ) {
                auto sideStart = [ & ]( u16 p_str ) {
                    auto fmtS = std::string( GET_STRING( p_str ) );
                    char bufS[ 110 ];
                    snprintf( bufS, sizeof( bufS ), fmtS.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                    p_move.m_user.second ),
                                                 p_move.m_user.first )
                                  .c_str( ) );
                    p_ui->log( bufS );
                };
                switch( cond ) {
                case SC_STEALTHROCK:
                    p_ui->log( GET_STRING( IO::STR_BATTLE_POINTED_STONES_SET ) );
                    break;
                case SC_SPIKES: p_ui->log( GET_STRING( IO::STR_BATTLE_SPIKES_SET ) ); break;
                case SC_TOXICSPIKES:
                    p_ui->log( GET_STRING( IO::STR_BATTLE_TOXIC_SPIKES_SET ) );
                    break;
                case SC_STICKYWEB: p_ui->log( GET_STRING( IO::STR_BATTLE_STICKY_WEB_SET ) ); break;
                case SC_QUICKGUARD: sideStart( IO::STR_BATTLE_QUICKGUARD_PROTECTS ); break;
                case SC_WIDEGUARD: sideStart( IO::STR_BATTLE_WIDEGUARD_PROTECTS ); break;
                case SC_CRAFTYSHIELD: sideStart( IO::STR_BATTLE_CRAFTYSHIELD_PROTECTS ); break;
                case SC_MATBLOCK: sideStart( IO::STR_BATTLE_MATBLOCK_BLOCKS ); break;
                case SC_SAFEGUARD: sideStart( IO::STR_BATTLE_SAFEGUARD_SET ); break;
                case SC_MIST: sideStart( IO::STR_BATTLE_MIST_SET ); break;
                default: break; // tailwind / lucky chant / screens stay silent
                }
            }

            if( tailwindNew && !suppressesAbilities( ) ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto pk = getPkmn( tg, j );
                    if( pk == nullptr || pk->getAbility( ) != A_WIND_RIDER
                        || slotAbilitiesNullified( tg, j ) ) {
                        continue;
                    }
                    boosts bt = boosts( );
                    bt.setBoost( ATK, 1 );
                    auto res = addBoosts( tg, j, bt );
                    p_ui->logAbility( getPkmnOrDisguise( tg, j ), tg );
                    p_ui->logBoosts( getPkmnOrDisguise( tg, j ), tg, j, bt, res );
                }
            }
        }

        if( p_move.m_moveData.m_weather ) {
            bool extended = false;
            if( canUseItem( p_move.m_user.first, p_move.m_user.second ) ) {
                if( p_move.m_moveData.m_weather == WE_RAIN && user->getItem( ) == I_DAMP_ROCK ) {
                    extended = true;
                }
                if( p_move.m_moveData.m_weather == WE_SUN && user->getItem( ) == I_HEAT_ROCK ) {
                    extended = true;
                }
                if( p_move.m_moveData.m_weather == WE_SANDSTORM
                    && user->getItem( ) == I_SMOOTH_ROCK ) {
                    extended = true;
                }
                if( p_move.m_moveData.m_weather == WE_HAIL && user->getItem( ) == I_ICY_ROCK ) {
                    extended = true;
                }
            }
            setWeather( p_ui, p_move.m_moveData.m_weather, extended );
        }
        if( p_move.m_moveData.m_pseudoWeather ) {
            bool extended = false;
            if( p_move.m_moveData.m_pseudoWeather == PW_MAGICROOM
                || p_move.m_moveData.m_pseudoWeather == PW_WONDERROOM
                || p_move.m_moveData.m_pseudoWeather == PW_TRICKROOM ) {
                extended = canUseItem( p_move.m_user.first, p_move.m_user.second )
                           && user->getItem( ) == I_ROOM_SERVICE;
            }

            setPseudoWeather( p_ui, p_move.m_moveData.m_pseudoWeather, extended );
        }
        if( p_move.m_moveData.m_terrain ) {
            bool extended = canUseItem( p_move.m_user.first, p_move.m_user.second )
                            && user->getItem( ) == I_TERRAIN_EXTENDER;
            setTerrain( p_ui, p_move.m_moveData.m_terrain, extended );
        }

        if( p_move.m_moveData.m_slotCondition ) {
            auto cond = slotCondition( 1LLU << ( u8( p_move.m_moveData.m_slotCondition ) - 1 ) );

            if( ( cond & ( SL_HEALINGWISH | SL_LUNARDANCE ) )
                && !hasHealthyBenched( p_target.first ) ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            } else {
                u16         amount = 0;
                std::string wisher;
                if( cond & SL_WISH ) {
                    auto wishUser = getPkmn( p_move.m_user.first, p_move.m_user.second );
                    if( wishUser != nullptr ) { amount = wishUser->m_stats.m_maxHP / 2; }
                    wisher = p_ui->getPkmnName(
                        getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                        p_move.m_user.first );
                }
                addSlotCondition( p_ui, p_target.first, p_target.second, cond, 0, amount, wisher );
            }
        }

        if( p_move.m_moveData.m_flags & MF_SELFDESTRUCTHIT ) {
            faintPokemon( p_ui, p_move.m_user.first, p_move.m_user.second );
        }

        if( p_move.m_moveData.m_volatileStatus ) {
            u8   duration   = 255;
            bool fail       = false;
            bool leechFresh = false;
            bool smackDone  = false;
            bool minDone    = false;
            if( p_move.m_moveData.m_volatileStatus & VS_BIDE ) { duration = 3; }
            if( p_move.m_moveData.m_volatileStatus & VS_POWDERED ) {
                duration = 1;
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_POWDERED )
                    [[unlikely]] {
                    removeVolatileStatus( p_ui, p_target.first, p_target.second, VS_POWDERED );
                }
            }
            if( p_move.m_moveData.m_volatileStatus & ( VS_PROTECT | VS_ENDURE ) ) {
                duration = 1;

                if( getStallWindow( p_target.first, p_target.second ) ) {
                    u16 counter = getStallCounter( p_target.first, p_target.second );
                    if( !counter ) { counter = 3; }
                    if( rand( ) % counter ) {
                        fail = true;
                        setStallCounter( p_target.first, p_target.second, 0 );
                        setStallWindow( p_target.first, p_target.second, 0 );
                    } else {
                        setStallCounter( p_target.first, p_target.second,
                                         counter < 729 ? u16( counter * 3 ) : counter );
                        setStallWindow( p_target.first, p_target.second, 2 );
                    }
                } else {
                    setStallCounter( p_target.first, p_target.second, 3 );
                    setStallWindow( p_target.first, p_target.second, 2 );
                }
            }
            if( p_move.m_moveData.m_volatileStatus
                & ( VS_FOLLOWME | VS_RAGEPOWDER | VS_SPOTLIGHT ) ) {
                duration = 1;
                fail     = ( _mode != BM_DOUBLE );
            }
            if( p_move.m_moveData.m_volatileStatus & ( VS_MAGICCOAT | VS_SNATCH ) ) {
                duration = 1;
            }
            if( p_move.m_moveData.m_volatileStatus & VS_LEECHSEED ) {
                if( hasType( p_target.first, p_target.second, TYPE_GRASS ) ) [[unlikely]] {
                    auto fmtG = std::string( GET_STRING( IO::STR_BATTLE_NO_EFFECT_ON ) );
                    char bufG[ 110 ];
                    snprintf(
                        bufG, sizeof( bufG ), fmtG.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ) );
                    p_ui->log( bufG );
                    return;
                }
                leechFresh
                    = !( getVolatileStatus( p_target.first, p_target.second ) & VS_LEECHSEED );
            }
            if( p_move.m_moveData.m_volatileStatus & VS_GASTROACID ) {
                auto tg = getPkmn( p_target.first, p_target.second );
                if( tg != nullptr
                    && ( abilityCantSuppress( tg->getAbility( ) )
                         || ( canUseItem( p_target.first, p_target.second )
                              && tg->getItem( ) == I_ABILITY_SHIELD )
                         || ( getVolatileStatus( p_target.first, p_target.second )
                              & VS_GASTROACID ) ) ) {
                    fail = true;
                }
            }

            if( p_move.m_moveData.m_volatileStatus & VS_IMPRISON ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_IMPRISON )
                    [[unlikely]] {
                    fail = true;
                }
            }
            if( p_move.m_moveData.m_volatileStatus & VS_HELPINGHAND ) {
                if( p_move.m_actsAfter & 1U ) { fail = true; }
                duration = 1;
            }
            if( p_move.m_moveData.m_volatileStatus & VS_DESTINYBOND ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_DESTINYBOND )
                    [[unlikely]] {
                    removeVolatileStatus( p_ui, p_target.first, p_target.second, VS_DESTINYBOND );
                    fail = true;
                }
            }
            if( p_move.m_moveData.m_volatileStatus & VS_TELEKINESIS ) {
                duration = 4;
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_TELEKINESIS )
                    [[unlikely]] {
                    fail = true;
                }
                auto spec = getPkmn( p_target.first, p_target.second )
                                ? getPkmn( p_target.first, p_target.second )->getSpecies( )
                                : 0;
                if( spec == PKMN_DIGLETT || spec == PKMN_DUGTRIO || spec == PKMN_SANDYGAST
                    || spec == PKMN_PALOSSAND ) [[unlikely]] {
                    auto fmtG = std::string( GET_STRING( IO::STR_BATTLE_NO_EFFECT_ON ) );
                    char bufG[ 110 ];
                    snprintf(
                        bufG, sizeof( bufG ), fmtG.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ) );
                    p_ui->log( bufG );
                    return;
                }
                if( getVolatileStatus( p_target.first, p_target.second )
                    & ( VS_SMACKDOWN | VS_INGRAIN ) ) [[unlikely]] {
                    fail = true;
                }
            }

            if( p_move.m_moveData.m_volatileStatus & VS_SMACKDOWN ) {
                auto slS     = getSlot( p_target.first, p_target.second );
                auto tgS     = slS ? slS->getPkmn( ) : nullptr;
                auto tgVl    = getVolatileStatus( p_target.first, p_target.second );
                bool printed = false;
                if( tgVl & VS_SMACKDOWN ) [[unlikely]] {
                    if( tgVl & VS_INAIR ) {
                        removeVolatileStatus( p_ui, p_target.first, p_target.second,
                                              volatileStatus( VS_MOVECHARGE | VS_INAIR ) );
                        removeLockedMove( p_target.first, p_target.second );
                        printed = true;
                    }
                    smackDone = true;
                } else {
                    bool levit   = ( slS != nullptr && slS->hasAbility( A_LEVITATE ) );
                    bool applies = ( slS != nullptr && slS->hasType( TYPE_FLYING ) ) || levit;
                    if( tgS != nullptr
                        && ( ( canUseItem( p_target.first, p_target.second )
                               && tgS->getItem( ) == I_IRON_BALL )
                             || ( tgVl & VS_INGRAIN ) || _pseudoWeatherTimer[ 4 ] /* gravity */ ) )
                        [[unlikely]] {
                        applies = false;
                    }
                    if( tgVl & VS_INAIR ) [[unlikely]] {
                        removeVolatileStatus( p_ui, p_target.first, p_target.second,
                                              volatileStatus( VS_MOVECHARGE | VS_INAIR ) );
                        removeLockedMove( p_target.first, p_target.second );
                        applies = true;
                    }
                    if( tgVl & VS_MAGNETRISE ) [[unlikely]] {
                        removeVolatileStatus( p_ui, p_target.first, p_target.second,
                                              VS_MAGNETRISE );
                        applies = true;
                    }
                    if( tgVl & VS_TELEKINESIS ) [[unlikely]] {
                        removeVolatileStatus( p_ui, p_target.first, p_target.second,
                                              VS_TELEKINESIS );
                        applies = true;
                    }
                    if( applies ) {
                        addVolatileStatus( p_ui, p_target.first, p_target.second, VS_SMACKDOWN,
                                           255 );
                        printed = true;
                    }
                    smackDone = true;
                }
                if( printed ) {
                    auto fmtK = std::string( GET_STRING( IO::STR_BATTLE_SMACK_DOWN_FELL ) );
                    char bufK[ 110 ];
                    snprintf(
                        bufK, sizeof( bufK ), fmtK.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ) );
                    p_ui->log( bufK );
                }
            }

            if( p_move.m_moveData.m_volatileStatus & VS_MINIMIZE ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_MINIMIZE )
                    [[unlikely]] {
                    minDone = true;
                }
            }

            u16 lockMove = 0;
            if( p_move.m_moveData.m_volatileStatus & ( VS_ENCORE | VS_DISABLE ) ) [[unlikely]] {
                bool encore = ( p_move.m_moveData.m_volatileStatus & VS_ENCORE ) != 0;
                lockMove    = getLastUsedMove( p_target.first, p_target.second ).m_param;
                bool inSet  = false;
                u8   pp     = 0;
                for( u8 s = 0; s < 4; ++s ) {
                    if( target->getMove( s ) == lockMove && lockMove ) {
                        inSet = true;
                        pp    = target->m_boxdata.m_curPP[ s ];
                        break;
                    }
                }
                if( !lockMove || lockMove == M_STRUGGLE || ( encore && !inSet )
                    || ( inSet && !pp ) ) {
                    fail = true;
                }
                duration = encore ? u8( _targetActsLater ? 4 : 5 ) : u8( _targetActsLater ? 5 : 6 );
            }

            bool sgBlocked = ( p_move.m_moveData.m_volatileStatus & ( VS_CONFUSION | VS_YAWN ) )
                             && gateExternalEffect( p_move, p_target, SC_SAFEGUARD );
            u8   tvT       = 0;
            if( !sgBlocked && !fail
                && ( p_move.m_moveData.m_volatileStatus & ( VS_CONFUSION | VS_YAWN ) ) ) {
                tvT = terrainGuardsStatus(
                    p_target, false, false, ( p_move.m_moveData.m_volatileStatus & VS_YAWN ) != 0,
                    ( p_move.m_moveData.m_volatileStatus & VS_CONFUSION ) != 0 );
            }
            if( !fail && ( p_move.m_moveData.m_volatileStatus & VS_POWERTRICK )
                && ( getVolatileStatus( p_target.first, p_target.second ) & VS_POWERTRICK ) )
                [[unlikely]] {
                removeVolatileStatus( p_ui, p_target.first, p_target.second, VS_POWERTRICK );
                auto fmtR = std::string( GET_STRING( IO::STR_BATTLE_STATS_SWAPPED ) );
                char bufR[ 160 ];
                snprintf( bufR, sizeof( bufR ), fmtR.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( bufR );
            }
            bool hhDone = false;
            if( !fail && ( p_move.m_moveData.m_volatileStatus & VS_HELPINGHAND )
                && ( getVolatileStatus( p_target.first, p_target.second ) & VS_HELPINGHAND ) )
                [[unlikely]] {
                restackHelpingHand( p_target.first, p_target.second );
                hhDone    = true;
                auto fmtH = std::string( GET_STRING( IO::STR_BATTLE_HELPING_HAND ) );
                char bufH[ 200 ];
                snprintf( bufH, sizeof( bufH ), fmtH.c_str( ),
                          p_ui->getPkmnName(
                                  getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first )
                              .c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( bufH );
            }
            if( sgBlocked ) [[unlikely]] {
                auto fmtS = std::string( GET_STRING( IO::STR_BATTLE_SAFEGUARD_PROTECTS ) );
                char bufS[ 110 ];
                snprintf( bufS, sizeof( bufS ), fmtS.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( bufS );
            } else if( tvT ) [[unlikely]] {
                if( tvT == 2 ) {
                    auto fmtT = std::string( GET_STRING( IO::STR_BATTLE_TERRAIN_PROTECTS ) );
                    char bufT[ 110 ];
                    snprintf(
                        bufT, sizeof( bufT ), fmtT.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ) );
                    p_ui->log( bufT );
                }
            } else if( hhDone ) {
                // empty
            } else if( smackDone ) {
                // empty
            } else if( minDone ) {
                // empty
            } else if( !fail
                       && addVolatileStatus( p_ui, p_target.first, p_target.second,
                                             p_move.m_moveData.m_volatileStatus, duration ) ) {
                if( leechFresh ) {
                    setLeechSource( p_target.first, p_target.second, p_move.m_user );
                }
                if( p_move.m_moveData.m_volatileStatus & VS_BIDE ) [[unlikely]] {
                    battleMoveSelection bms = NO_OP_SELECTION;
                    bms.m_type              = MT_ATTACK;
                    bms.m_param             = p_move.m_param;
                    bms.m_moveData          = p_move.m_moveData;
                    bms.m_user              = p_move.m_user;
                    addLockedMove( p_target.first, p_target.second, bms );
                }
                if( p_move.m_moveData.m_volatileStatus & VS_PARTIALLYTRAPPED ) [[unlikely]] {
                    auto binder = getPkmn( p_move.m_user.first, p_move.m_user.second );
                    auto bname  = p_ui->getPkmnName(
                        getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                        p_move.m_user.first );
                    setPartialTrap( p_target.first, p_target.second, bname, p_move.m_param,
                                    binder == nullptr ? 0 : binder->getSpecies( ),
                                    p_move.m_user.first, p_move.m_user.second,
                                    5 + ( rand( ) & 1 ) );
                    auto fmtT = std::string( GET_STRING( IO::STR_BATTLE_BECAME_TRAPPED ) );
                    char bufT[ 160 ];
                    snprintf(
                        bufT, sizeof( bufT ), fmtT.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ),
                        bname.c_str( ), FS::getMoveName( p_move.m_param ).c_str( ) );
                    p_ui->log( bufT );
                } else if( lockMove
                           && ( p_move.m_moveData.m_volatileStatus & ( VS_ENCORE | VS_DISABLE ) ) )
                    [[unlikely]] {
                    bool encore = ( p_move.m_moveData.m_volatileStatus & VS_ENCORE ) != 0;
                    auto tname  = p_ui->getPkmnName(
                        getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first );
                    if( encore ) {
                        setEncoreMove( p_target.first, p_target.second, lockMove );
                        auto fmtE = std::string( GET_STRING( IO::STR_BATTLE_ENCORE_START ) );
                        char bufE[ 160 ];
                        snprintf( bufE, sizeof( bufE ), fmtE.c_str( ), tname.c_str( ) );
                        p_ui->log( bufE );
                    } else {
                        setDisabledMove( p_target.first, p_target.second, lockMove );
                        auto fmtD = std::string( GET_STRING( IO::STR_BATTLE_DISABLE_START ) );
                        char bufD[ 160 ];
                        snprintf( bufD, sizeof( bufD ), fmtD.c_str( ), tname.c_str( ),
                                  FS::getMoveName( lockMove ).c_str( ) );
                        p_ui->log( bufD );
                    }
                } else if( p_move.m_moveData.m_volatileStatus & VS_GASTROACID ) [[unlikely]] {
                    auto aname = p_ui->getPkmnName(
                        getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first );
                    auto fmtA = std::string( GET_STRING( IO::STR_BATTLE_ABILITY_SUPPRESSED ) );
                    char bufA[ 160 ];
                    snprintf( bufA, sizeof( bufA ), fmtA.c_str( ), aname.c_str( ) );
                    p_ui->log( bufA );
                } else if( p_move.m_moveData.m_volatileStatus & VS_HELPINGHAND ) [[unlikely]] {
                    auto fmtH = std::string( GET_STRING( IO::STR_BATTLE_HELPING_HAND ) );
                    char bufH[ 200 ];
                    snprintf(
                        bufH, sizeof( bufH ), fmtH.c_str( ),
                        p_ui->getPkmnName(
                                getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                p_move.m_user.first )
                            .c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first )
                            .c_str( ) );
                    p_ui->log( bufH );
                } else {
                    p_ui->animateGetVolatileStatusCondition(
                        getPkmnOrDisguise( p_target.first, p_target.second ), p_target.first,
                        p_target.second, p_move.m_moveData.m_volatileStatus );
                }
            } else {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
        }
    }

} // namespace BATTLE
