/*
Pokémon neo
------------------------------

file        : battleFieldMove.cpp
author      : Philip Wellnitz
description : the battle-move executor: immunity, charge, protection and the full turn lane

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
    void field::executeBattleMove( battleUI* p_ui, battleMove p_move,
                                   const std::vector<battleMove>& p_targetsMoves,
                                   const std::vector<battleMove>& p_tergetedMoves ) {

        (void) p_tergetedMoves;

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        bool         opponent   = p_move.m_user.first;
        u8           slot       = p_move.m_user.second;
        bool         anyHit     = false;
        bool         sfStrip    = sheerForceStrips( p_move );
        bool         danceActed = false;
        auto         slotc      = getVolatileStatus( opponent, slot );

        if( getSlotStatus( opponent, slot ) != slot::status::NORMAL ) { return; }

        if( u8 err = useMove( p_ui, p_move ); err != MOVE_SUCCESS ) {
            if( err != MOVE_FAIL_NO_PP && !_nestedCallsMove
                && !( p_move.m_param == M_BIDE && ( slotc & VS_BIDE ) ) ) {
                deducePP( opponent, slot, p_move.m_param );
            }

            if( p_move.m_param == M_BIDE && ( slotc & VS_BIDE ) ) [[unlikely]] {
                removeVolatileStatus( p_ui, opponent, slot, VS_BIDE );
                removeLockedMove( opponent, slot );
                clearBide( opponent, slot );
            }

            if( slotc & VS_MOVECHARGE ) { // Move is already charged
                removeVolatileStatus( p_ui, opponent, slot,
                                      volatileStatus( VS_MOVECHARGE | VS_DIGGING | VS_DIVING
                                                      | VS_INAIR | VS_INVISIBLE ) );
                removeLockedMove( opponent, slot );
                p_ui->showPkmn( opponent, slot );
            }

            WAIT( HALF_SEC );
            return;
        }

        if( p_move.m_moveData.m_flags & MF_CHARGE ) {
            if( slotc & VS_MOVECHARGE ) { // Move is already charged
                removeVolatileStatus( p_ui, opponent, slot,
                                      volatileStatus( VS_MOVECHARGE | VS_DIGGING | VS_DIVING
                                                      | VS_INAIR | VS_INVISIBLE ) );
                removeLockedMove( opponent, slot );
                p_ui->showPkmn( opponent, slot );
            } else {
                auto tg = getPkmn( opponent, slot );
                if( tg == nullptr ) [[unlikely]] { return; }

                p_ui->prepareMove( getPkmnOrDisguise( opponent, slot ), opponent, slot, p_move );
                WAIT( HALF_SEC );

                if( ( p_move.m_param == M_SOLAR_BLADE || p_move.m_param == M_SOLAR_BEAM )
                    && !suppressesWeather( )
                    && ( _weather == WE_SUN || _weather == WE_HEAVY_SUNSHINE ) ) [[unlikely]] {
                    // empty!
                } else if( canUseItem( opponent, slot ) && tg->getItem( ) == I_POWER_HERB )
                    [[unlikely]] {
                    p_ui->logItem( getPkmnOrDisguise( opponent, slot ), opponent );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_POWER_HERB_CHARGED ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent )
                                  .c_str( ) );
                    p_ui->log( buffer );

                    removeItem( p_ui, opponent, slot );
                } else {
                    addVolatileStatus( p_ui, opponent, slot, VS_MOVECHARGE, 255 );
                    switch( p_move.m_param ) {
                    case M_DIVE: addVolatileStatus( p_ui, opponent, slot, VS_DIVING, 255 ); break;
                    case M_DIG: addVolatileStatus( p_ui, opponent, slot, VS_DIGGING, 255 ); break;
                    case M_FLY:
                    case M_BOUNCE:
                    case M_SKY_DROP:
                        addVolatileStatus( p_ui, opponent, slot, VS_INAIR, 255 );
                        break;
                    case M_PHANTOM_FORCE:
                    case M_SHADOW_FORCE:
                        addVolatileStatus( p_ui, opponent, slot, VS_INVISIBLE, 255 );
                        break;

                    default: break;
                    }

                    battleMoveSelection bms = NO_OP_SELECTION;
                    bms.m_type              = MT_ATTACK;
                    bms.m_param             = p_move.m_param;
                    bms.m_moveData          = p_move.m_moveData;
                    if( p_move.m_target.size( ) != 1 ) [[unlikely]] {
#ifdef DESQUID
                        p_ui->log( "[error] Multi-turn attack has multiple targets." );
#endif
                    } else {
                        bms.m_target = p_move.m_target[ 0 ];
                    }
                    bms.m_user = p_move.m_user;

                    addLockedMove( opponent, slot, bms );
                    if( !p_move.m_dancerCopy ) {
                        setLastUsedMove( opponent, slot, p_move );
                        markMoveActed( opponent, slot );
                    }
                    WAIT( HALF_SEC );
                    return;
                }
            }
        }

        if( !_nestedCallsMove && !getLockedMoveCount( opponent, slot ) ) {
            deducePP( opponent, slot, p_move.m_param );
        }

        if( p_move.m_param == M_PERISH_SONG ) [[unlikely]] {
            bool anyResult = false;
            bool anyNew    = false;
            for( u8 i = 0; i < 2; ++i ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto pkmn = getPkmn( bool( i ), j );
                    if( pkmn == nullptr || !pkmn->canBattle( ) ) { continue; }
                    if( isSoundBlockedForStatus( p_move, bool( i ), j ) ) {
                        anyResult = true;
                        continue;
                    }
                    if( getPerishTimer( bool( i ), j ) ) { continue; }
                    setPerishTimer( bool( i ), j, 5 );
                    anyResult = anyNew = true;
                }
            }
            if( anyNew ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_PERISH_SONG ) );
            } else if( !anyResult ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            }
        }

        if( p_move.m_param == M_COUNTER || p_move.m_param == M_MIRROR_COAT ) [[unlikely]] {
            bool          mirror = ( p_move.m_param == M_MIRROR_COAT );
            fieldPosition tgt
                = mirror ? getMirrorTarget( opponent, slot ) : getCounterTarget( opponent, slot );
            p_move.m_powerOverride
                = mirror ? getMirrorDamage( opponent, slot ) : getCounterDamage( opponent, slot );
            clearCounters( opponent, slot );
            if( getPkmn( tgt.first, tgt.second ) == nullptr
                || getSlotStatus( tgt.first, tgt.second ) == slot::status::FAINTED ) [[unlikely]] {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return;
            }
            p_move.m_target.clear( );
            p_move.m_target.push_back( tgt );
        }

        bool bidePayback = false;
        if( p_move.m_param == M_BIDE && ( slotc & VS_BIDE ) ) [[unlikely]] {
            if( getVolatileStatusCounter( opponent, slot, VS_BIDE ) > 1 ) {
                p_ui->log( GET_STRING( IO::STR_BATTLE_STORING_ENERGY ) );
                return;
            }
            auto fmtB = std::string( GET_STRING( IO::STR_BATTLE_ENERGY_RELEASED ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmtB.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );
            u16  recorded = getBideDamage( opponent, slot );
            bool hit      = bideHit( opponent, slot );
            removeVolatileStatus( p_ui, opponent, slot, VS_BIDE );
            removeLockedMove( opponent, slot );
            if( !hit || !recorded ) [[unlikely]] {
                clearBide( opponent, slot );
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return;
            }
            fieldPosition tgt  = getBideTarget( opponent, slot );
            auto          tgtP = getPkmn( tgt.first, tgt.second );
            if( tgtP == nullptr || !tgtP->canBattle( ) ) [[unlikely]] {
                u8 live[ 2 ] = { 0, 0 };
                u8 n         = 0;
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto cand = getPkmn( !opponent, j );
                    if( cand != nullptr && cand->canBattle( ) ) { live[ n++ ] = j; }
                }
                if( !n ) [[unlikely]] {
                    clearBide( opponent, slot );
                    return;
                }
                tgt = { !opponent, live[ rand( ) % n ] };
            }
            clearBide( opponent, slot );
            p_move.m_moveData.m_flags = moveFlags( p_move.m_moveData.m_flags & ~MF_IGNOREIMMUNITY );
            p_move.m_powerOverride    = ( recorded > 0x7FFFu ) ? 0xFFFFu : u16( recorded * 2 );
            p_move.m_target.clear( );
            p_move.m_target.push_back( tgt );
            bidePayback = true;
        }

        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_USED_MOVE ) );
        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                  p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ),
                  FS::getMoveName( p_move.m_param ).c_str( ) );
        if( !bidePayback ) { p_ui->log( buffer ); }

        if( !p_move.m_fromSnatch && ( p_move.m_moveData.m_flags & MF_SNATCH )
            && p_move.m_moveData.m_category == MH_STATUS ) [[unlikely]] {
            fieldPosition holder = { 255, 255 };
            for( u8 pass = 0; pass < 2 && holder.first == 255; ++pass ) {
                bool s = ( pass == 0 ) ? !opponent : opponent;
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    auto cand = getPkmn( s, j );
                    if( cand != nullptr && cand->canBattle( )
                        && ( getVolatileStatus( s, j ) & VS_SNATCH ) ) {
                        holder = { s, j };
                        break;
                    }
                }
            }
            if( holder.first != 255 ) [[unlikely]] {
                removeVolatileStatus( p_ui, holder.first, holder.second, VS_SNATCH );
                auto fmtS = std::string( GET_STRING( IO::STR_BATTLE_SNATCH_STOLEN ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmtS.c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( holder.first, holder.second ),
                                       holder.first )
                        .c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                battleMove stolen( p_move );
                stolen.m_fromSnatch = true;
                stolen.m_user       = holder;
                stolen.m_target.clear( );
                stolen.m_target.push_back( holder );
                auto fmt2 = std::string( GET_STRING( IO::STR_BATTLE_USED_MOVE ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt2.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( holder.first, holder.second ),
                                             holder.first )
                              .c_str( ),
                          FS::getMoveName( stolen.m_param ).c_str( ) );
                p_ui->log( buffer );
                executeStatusEffects( p_ui, stolen, holder );
                return;
            }
        }

        bool moveHadTarget = _mode == BM_SINGLE;
        for( u8 i = 0; i < p_move.m_target.size( ); ++i ) {
            auto curTg = getPkmn( p_move.m_target[ i ].first, p_move.m_target[ i ].second );
            if( curTg == nullptr ) {
                if( p_move.m_target.size( ) == 1 && _mode == BM_DOUBLE
                    && p_move.m_target[ i ].first != opponent ) {
                    p_move.m_target[ i ].second ^= 1;
                    curTg = getPkmn( p_move.m_target[ i ].first, p_move.m_target[ i ].second );
                    if( curTg == nullptr ) { break; }
                } else {
                    continue;
                }
            }
            moveHadTarget = true;

            redirectSingleTarget( p_ui, p_move, p_move.m_target[ i ] );

            if( p_move.m_moveData.m_flags & MF_FUTUREMOVE ) [[unlikely]] {
                bool tOpp = p_move.m_target[ i ].first;
                u8   tPos = p_move.m_target[ i ].second;
                u16  src  = u16( ( u16( p_move.m_user.first ? 1 : 0 ) << 8 )
                                 | u16( p_move.m_user.second ) );
                if( !addSlotCondition( p_ui, tOpp, tPos, SL_FUTURESIGHT, 3, p_move.m_param,
                                       std::string( ), src ) ) {
                    p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                } else {
                    auto fmtF = std::string( GET_STRING( IO::STR_BATTLE_FUTURE_SIGHT_SEES ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmtF.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                    p_move.m_user.second ),
                                                 p_move.m_user.first )
                                  .c_str( ) );
                    p_ui->log( buffer );
                }
                continue;
            }

            // Check for multi-hit moves
            u8 numHits = 1, strengthMod = 100;
            if( p_move.m_param == M_BEAT_UP ) [[unlikely]] {
                numHits    = _beatUpAllyBPs( p_move.m_user, _beatUpBP );
                _beatUpIdx = 0;
            } else if( p_move.m_moveData.getMultiHitMax( ) > 1 ) [[unlikely]] {
                numHits = p_move.m_moveData.getMultiHitMin( );
                numHits += rand( )
                           % ( p_move.m_moveData.getMultiHitMax( )
                               - p_move.m_moveData.getMultiHitMin( ) + 1 );
                auto diceUser = getPkmn( p_move.m_user.first, p_move.m_user.second );
                if( diceUser != nullptr && canUseItem( p_move.m_user.first, p_move.m_user.second )
                    && diceUser->getItem( ) == I_LOADED_DICE ) {
                    if( p_move.m_moveData.getMultiHitMin( ) == 2
                        && p_move.m_moveData.getMultiHitMax( ) == 5 && numHits < 4 ) {
                        numHits = u8( 5 - ( rand( ) % 2 ) );
                    } else if( numHits == 10 ) {
                        numHits = u8( numHits - ( rand( ) % 7 ) );
                    }
                }

                if( !suppressesAbilities( )
                    && getPkmn( opponent, slot )->getAbility( ) == A_SKILL_LINK
                    && !slotAbilitiesNullified( opponent, slot ) ) [[unlikely]] {
                    numHits = p_move.m_moveData.getMultiHitMax( );
                }
            } else if( !suppressesAbilities( )
                       && getPkmn( opponent, slot )->getAbility( ) == A_PARENTAL_BOND
                       && !slotAbilitiesNullified( opponent, slot )
                       && p_move.m_moveData.m_category != MH_STATUS && p_move.m_target.size( ) <= 1
                       && !( p_move.m_moveData.m_flags & MF_CHARGE )
                       && !( p_move.m_moveData.m_flags & MF_FUTUREMOVE )
                       && p_move.m_param != M_DRAGON_DARTS && p_move.m_param != M_ENDEAVOR
                       && p_move.m_param != M_EXPLOSION && p_move.m_param != M_ICE_BALL )
                [[unlikely]] {
                numHits     = 2;
                strengthMod = 25;
            }
            bool multihit = numHits > 1 || p_move.m_param == M_BEAT_UP;

            u8 hits = 0;
            for( u8 j = 0; j < numHits; ++j ) {
                _hitSubstituted = false;
                auto tgsc
                    = getVolatileStatus( p_move.m_target[ i ].first, p_move.m_target[ i ].second );
                bool protect = false;
                bool tgBlock = false;
                if( psychicTerrainGuards( p_move, p_move.m_target[ i ] ) ) [[unlikely]] {
                    protect = true;
                    tgBlock = true;
                    fmt     = std::string( GET_STRING( IO::STR_BATTLE_TERRAIN_PROTECTS ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                    p_move.m_target[ i ].second ),
                                                 p_move.m_target[ i ].first )
                                  .c_str( ) );
                    p_ui->log( buffer );
                }
                u16 guardStr = 0;
                if( !protect ) {
                    auto const& md          = p_move.m_moveData;
                    auto        sc          = getSideCondition( p_move.m_target[ i ].first );
                    bool        protectable = ( md.m_flags & MF_PROTECT ) != 0;
                    if( ( sc & SC_QUICKGUARD ) && effectiveMovePriority( p_move ) > 0
                        && protectable ) {
                        guardStr = IO::STR_BATTLE_QUICKGUARD_PROTECTS;
                    } else if( ( sc & SC_WIDEGUARD ) && protectable
                               && ( md.m_target == TG_ALL_FOES_AND_ALLY
                                    || md.m_target == TG_ALL_FOES ) ) {
                        guardStr = IO::STR_BATTLE_WIDEGUARD_PROTECTS;
                    } else if( ( sc & SC_CRAFTYSHIELD ) && md.m_category == MH_STATUS
                               && md.m_target != TG_SELF && md.m_target != TG_FIELD ) {
                        guardStr = IO::STR_BATTLE_CRAFTYSHIELD_PROTECTS;
                    } else if( ( sc & SC_MATBLOCK ) && protectable && md.m_category != MH_STATUS
                               && md.m_target != TG_SELF ) {
                        guardStr = IO::STR_BATTLE_MATBLOCK_BLOCKS;
                    }
                    if( guardStr ) [[unlikely]] {
                        protect = true;
                        fmt     = std::string( GET_STRING( guardStr ) );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                  p_move.m_target[ i ].second ),
                                               p_move.m_target[ i ].first )
                                .c_str( ) );
                        p_ui->log( buffer );
                    }
                }
                if( !guardStr && !tgBlock
                    && ( ( tgsc & VS_PROTECT ) || ( tgsc & VS_OBSTRUCT )
                         || ( tgsc & VS_SPIKYSHIELD )
                         || ( p_move.m_moveData.m_category != MH_STATUS
                              && ( tgsc & VS_KINGSSHIELD ) ) ) ) [[unlikely]] {
                    protect = true;
                    if( ( p_move.m_moveData.m_flags & MF_PROTECT ) ) {
                        fmt = std::string( GET_STRING( IO::STR_BATTLE_PROTECTS_ITSELF ) );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                  p_move.m_target[ i ].second ),
                                               p_move.m_target[ i ].first )
                                .c_str( ) );
                        p_ui->log( buffer );
                    }
                }

                if( protect && !guardStr && !tgBlock
                    && ( p_move.m_moveData.m_flags & MF_BREAKSPROTECT ) ) {
                    // remove protect
                    removeVolatileStatus( p_ui, p_move.m_target[ i ].first,
                                          p_move.m_target[ i ].second, VS_PROTECT );
                    tgsc = getVolatileStatus( p_move.m_target[ i ].first,
                                              p_move.m_target[ i ].second );
                    fmt  = std::string( GET_STRING( IO::STR_BATTLE_PROTECTION_BROKEN ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                    p_move.m_target[ i ].second ),
                                                 p_move.m_target[ i ].first )
                                  .c_str( ) );
                    p_ui->log( buffer );
                    protect = false;
                }

                if( !protect && !guardStr && !p_move.m_bounced
                    && ( p_move.m_moveData.m_flags & MF_REFLECTABLE )
                    && p_move.m_moveData.m_category == MH_STATUS
                    && p_move.m_target[ i ] != p_move.m_user && ( tgsc & VS_MAGICCOAT ) )
                    [[unlikely]] {
                    battleMove bounced( p_move );
                    bounced.m_bounced = true;
                    bounced.m_user    = p_move.m_target[ i ];
                    bounced.m_target.clear( );
                    bounced.m_target.push_back( p_move.m_user );
                    auto fmtB = std::string( GET_STRING( IO::STR_BATTLE_USED_MOVE ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmtB.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( bounced.m_user.first,
                                                                    bounced.m_user.second ),
                                                 bounced.m_user.first )
                                  .c_str( ),
                              FS::getMoveName( bounced.m_param ).c_str( ) );
                    p_ui->log( buffer );
                    if( psychicTerrainGuardsPrio( bounced, p_move.m_user,
                                                  effectiveMovePriority( p_move ) ) ) [[unlikely]] {
                        fmt = std::string( GET_STRING( IO::STR_BATTLE_TERRAIN_PROTECTS ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                        p_move.m_user.second ),
                                                     p_move.m_user.first )
                                      .c_str( ) );
                        p_ui->log( buffer );
                    } else {
                        executeStatusEffects( p_ui, bounced, p_move.m_user );
                    }
                    protect = true;
                }

                if( ( p_move.m_moveData.m_flags & MF_PROTECT ) && !protect ) [[likely]] {
                    bool critical = executeCriticalCheck( p_ui, p_move, p_move.m_target[ i ] );
                    bool missed   = j == 0 || p_move.m_param == M_TRIPLE_KICK
                                    || p_move.m_param == M_TRIPLE_AXEL;
                    if( missed ) {
                        missed = moveMisses( p_ui, p_move, p_move.m_target[ i ], critical );
                    }
                    if( missed ) {
                        fmt = std::string( GET_STRING( IO::STR_BATTLE_EVADED_ATTACK ) );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                  p_move.m_target[ i ].second ),
                                               p_move.m_target[ i ].first )
                                .c_str( ) );
                        p_ui->log( buffer );
                        // check for crash damage

                        if( p_move.m_moveData.m_flags & MF_CRASHDAMAGE ) {
                            u16 maxHP = getPkmn( opponent, slot )->m_stats.m_maxHP;
                            damagePokemon( p_ui, opponent, slot, maxHP / 2 );
                            fmt = std::string( GET_STRING( IO::STR_BATTLE_CONTINUES_CRASHES ) );
                            snprintf(
                                buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent )
                                    .c_str( ) );
                            p_ui->log( buffer );

                            if( !getPkmn( opponent, slot )->canBattle( ) ) {
                                faintPokemon( p_ui, opponent, slot );
                            }
                        }
                        break;
                    } else if( p_move.m_moveData.m_flags & MF_OHKO ) [[unlikely]] {
                        anyHit = true;
                        if( !substituteExempt( p_move, p_move.m_target[ i ] )
                            && getSubstituteHp( p_move.m_target[ i ].first,
                                                p_move.m_target[ i ].second ) ) {
                            setSubstituteHp( p_move.m_target[ i ].first,
                                             p_move.m_target[ i ].second, 0 );
                            removeVolatileStatus( p_ui, p_move.m_target[ i ].first,
                                                  p_move.m_target[ i ].second, VS_SUBSTITUTE );
                            p_ui->log( GET_STRING( IO::STR_BATTLE_ONE_HIT_KO ) );
                            p_ui->log( GET_STRING( IO::STR_BATTLE_SUBSTITUTE_FADED ) );
                            _hitSubstituted = true; // target-side effects off
                        } else {
                            bool dbArmed     = ( getVolatileStatus( p_move.m_target[ i ].first,
                                                                    p_move.m_target[ i ].second )
                                                 & VS_DESTINYBOND )
                                               != 0;
                            bool grudgeArmed = ( getVolatileStatus( p_move.m_target[ i ].first,
                                                                    p_move.m_target[ i ].second )
                                                 & VS_GRUDGE )
                                               != 0;
                            faintPokemon( p_ui, p_move.m_target[ i ].first,
                                          p_move.m_target[ i ].second );
                            p_ui->log( GET_STRING( IO::STR_BATTLE_ONE_HIT_KO ) );
                            if( grudgeArmed ) { grudgePayOff( p_ui, p_move ); }
                            if( dbArmed ) {
                                destinyBondTakeDown( p_ui, p_move, p_move.m_target[ i ] );
                            }
                        }
                    } else if( p_move.m_moveData.m_category != MH_STATUS ) {
                        if( !executeDamagingMove( p_ui, p_move, p_move.m_target[ i ], critical,
                                                  j == 1 && strengthMod < 100 ? strengthMod
                                                                              : 100 ) ) {
                            break;
                        }
                        anyHit = true;
                    }
                    hits++;
                }

                if( guardStr || protect ) [[unlikely]] { break; }

                // Pkmn fainted
                if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { break; }

                if( !sfStrip ) { executeSelfStatusEffects( p_ui, p_move, p_move.m_target[ i ] ); }

                // Target fainted
                if( getSlotStatus( p_move.m_target[ i ].first, p_move.m_target[ i ].second )
                    == slot::status::FAINTED ) {
                    break;
                }

                if( !protect && !_hitSubstituted ) [[likely]] {
                    _targetActsLater = false;
                    if( p_move.m_moveData.m_volatileStatus & ( VS_ENCORE | VS_DISABLE ) )
                        [[unlikely]] {
                        for( const auto& m : p_targetsMoves ) {
                            if( m.m_user == p_move.m_target[ i ] ) {
                                _targetActsLater = true;
                                break;
                            }
                        }
                    }
                    executeStatusEffects( p_ui, p_move, p_move.m_target[ i ] );
                    danceActed = true;
                    auto tgt   = getPkmn( p_move.m_target[ i ].first, p_move.m_target[ i ].second );
                    bool cloak
                        = tgt != nullptr
                          && canUseItem( p_move.m_target[ i ].first, p_move.m_target[ i ].second )
                          && tgt->getItem( ) == I_COVERT_CLOAK;

                    if( !cloak && !sfStrip
                        && ( p_move.m_moveData.m_secondaryStatus
                             || p_move.m_moveData.m_secondaryVolatileStatus )
                        && ( rand( ) % 100 < p_move.m_moveData.m_secondaryChance ) ) {
                        executeSecondaryStatus( p_ui, p_move, p_move.m_target[ i ] );
                    }
                    // Other secondary effects
                    if( !cloak && !sfStrip
                        && rand( ) % 100 < p_move.m_moveData.m_secondaryChance ) {
                        executeSecondaryEffects( p_ui, p_move, p_move.m_target[ i ] );
                    }
                    if( !cloak && p_move.m_moveData.m_category != MH_STATUS
                        && ( sfStrip
                             || !( p_move.m_moveData.m_secondaryVolatileStatus & VS_FLINCH ) ) ) {
                        auto tk = getSlotStatus( p_move.m_target[ i ].first,
                                                 p_move.m_target[ i ].second );
                        if( tk != slot::status::FAINTED
                            && canUseItem( p_move.m_user.first, p_move.m_user.second ) ) {
                            auto uItm = getPkmn( p_move.m_user.first, p_move.m_user.second );
                            if( uItm != nullptr
                                && ( uItm->getItem( ) == I_KING_S_ROCK
                                     || uItm->getItem( ) == I_RAZOR_FANG )
                                && ( rand( ) % 100 < 10 ) ) {
                                addVolatileStatus( p_ui, p_move.m_target[ i ].first,
                                                   p_move.m_target[ i ].second, VS_FLINCH, 1 );
                            }
                        }
                    }
                }

                // Pkmn fainted
                if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { break; }
                // Target fainted
                if( getSlotStatus( p_move.m_target[ i ].first, p_move.m_target[ i ].second )
                    == slot::status::FAINTED ) {
                    break;
                }

                if( ( p_move.m_moveData.m_flags & MF_CONTACT ) && !_hitSubstituted ) {
                    executeContact( p_ui, p_move, p_move.m_target[ i ] );
                }

                // Pkmn fainted
                if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { break; }
                // Target fainted
                if( getSlotStatus( p_move.m_target[ i ].first, p_move.m_target[ i ].second )
                    == slot::status::FAINTED ) {
                    break;
                }

                if( ( p_move.m_moveData.m_flags & MF_FORCESWITCH ) && !_hitSubstituted ) {
                    if( windRiderSteal( p_ui, p_move, p_move.m_target[ i ] ) ) { break; }
                    if( _isWildBattle && p_move.m_target[ i ].first != opponent ) {
                        recallPokemon( p_ui, OPPONENT_SIDE, 0 );
                        break;
                    }
                    if( canRecallPokemon( p_move.m_target[ i ].first ) ) {
                        recallPokemon( p_ui, p_move.m_target[ i ].first,
                                       p_move.m_target[ i ].second );
                    }
                    break;
                }

                if( ( p_move.m_moveData.m_flags & MF_DEFROSTTARGET ) && !_hitSubstituted ) {
                    if( hasStatusCondition( p_move.m_target[ i ].first, p_move.m_target[ i ].second,
                                            FROZEN ) ) {
                        fmt = std::string( GET_STRING( IO::STR_BATTLE_WAS_THAWED ) );
                        snprintf(
                            buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                            p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                  p_move.m_target[ i ].second ),
                                               p_move.m_target[ i ].first )
                                .c_str( ) );
                        p_ui->log( buffer );
                        removeStatusCondition( p_move.m_target[ i ].first,
                                               p_move.m_target[ i ].second );
                        p_ui->updatePkmnStats( p_move.m_target[ i ].first,
                                               p_move.m_target[ i ].second,
                                               getPkmnOrDisguise( p_move.m_target[ i ].first,
                                                                  p_move.m_target[ i ].second ) );
                        WAIT( HALF_SEC );
                    }
                }

                if( !_hitSubstituted ) {
                    checkItemAfterAttack( p_ui, p_move.m_target[ i ].first,
                                          p_move.m_target[ i ].second );
                }
                checkItemAfterAttack( p_ui, opponent, slot );
            }

            if( multihit && hits >= 1 ) {
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_HIT_TIMES ),
                          FS::kanaWrap( hits, CURRENT_LANGUAGE ) );
                p_ui->log( buffer );
            }

            if( getSlotStatus( opponent, slot ) == slot::status::FAINTED ) { continue; }
        }

        if( !moveHadTarget ) {
            p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) ); // "It failed."
            return;
        }

        // Check for flags and stuff
        if( p_move.m_moveData.m_flags & MF_MINDBLOWNRECOIL ) {
            // pkmn loses half of its max hp
            u16 maxHP = getPkmn( opponent, slot )->m_stats.m_maxHP;
            damagePokemon( p_ui, opponent, slot, maxHP / 2 );

            if( !getPkmn( opponent, slot )->canBattle( ) ) {
                faintPokemon( p_ui, opponent, slot );
                return;
            }
        }

        if( p_move.m_moveData.m_flags & MF_RECHARGE ) {
            battleMoveSelection bms = NO_OP_SELECTION;
            bms.m_type              = MT_ATTACK;
            addLockedMove( opponent, slot, bms, 0 );
            addVolatileStatus( p_ui, opponent, slot, VS_RECHARGE, 255 );
        }

        if( p_move.m_moveData.m_flags & MF_LOCKEDMOVE && anyHit && !p_move.m_dancerCopy ) {
            auto                lmcnt = getLockedMoveCount( opponent, slot );
            battleMoveSelection bms   = NO_OP_SELECTION;
            bms.m_type                = MT_ATTACK;
            bms.m_param               = p_move.m_param;
            bms.m_moveData            = p_move.m_moveData;
            bms.m_target              = { 255, 255 };
            bms.m_user                = p_move.m_user;
            if( !lmcnt ) {
                addLockedMove( opponent, slot, bms, 1 + rand( ) % 2 );
            } else if( --lmcnt ) {
                addLockedMove( opponent, slot, bms, lmcnt );
            } else {
                removeLockedMove( opponent, slot );
                if( !fatigueConfusionBlocked( opponent, slot ) ) {
                    fmt = std::string( GET_STRING( IO::STR_BATTLE_BECAME_CONFUSED ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent )
                                  .c_str( ) );
                    p_ui->log( buffer );

                    if( getVolatileStatus( opponent, slot ) & VS_CONFUSION ) {
                        // empty!
                    } else {
                        addVolatileStatus( p_ui, opponent, slot, VS_CONFUSION,
                                           250 + 2 + ( rand( ) % 4 ) );
                    }
                }
            }
        }

        if( p_move.m_param == M_UPROAR && anyHit ) [[unlikely]] {
            if( !getLockedMoveCount( opponent, slot ) ) {
                battleMoveSelection bms = NO_OP_SELECTION;
                bms.m_type              = MT_ATTACK;
                bms.m_param             = p_move.m_param;
                bms.m_moveData          = p_move.m_moveData;
                bms.m_user              = p_move.m_user;
                addLockedMove( opponent, slot, bms, 3 );
                fmt = std::string( GET_STRING( IO::STR_BATTLE_UPROAR_START ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
            }
        }

        if( p_move.m_moveData.m_flags & MF_ROOST ) {
            addVolatileStatus( p_ui, opponent, slot, VS_ROOST, 2 );
        }

        if( p_move.m_param == M_BURN_UP ) {
            addVolatileStatus( p_ui, opponent, slot, VS_BURNUP, 255 );
        }

        if( canRecallPokemon( opponent ) ) {
            if( p_move.m_moveData.m_flags & MF_SELFSWITCH ) {
                recallPokemon( p_ui, opponent, slot );
            }

            if( p_move.m_moveData.m_flags & MF_BATONPASS ) {
                recallPokemon( p_ui, opponent, slot, true );
            }
        }

        if( p_move.m_moveData.m_flags & MF_SELFDESTRUCT ) { faintPokemon( p_ui, opponent, slot ); }

        // check for held item
        checkItemAfterAttack( p_ui, opponent, slot );
        _mirrorHerbConsume( p_ui );
        if( !p_move.m_dancerCopy ) {
            setLastUsedMove( opponent, slot, p_move );
            markMoveActed( opponent, slot );
        }

        _actionThisTurn[ opponent ][ slot ] = ( anyHit || danceActed ) ? 1 : 0;

        if( ( p_move.m_moveData.m_flags & MF_DANCE ) && !p_move.m_dancerCopy
            && ( anyHit || danceActed ) && !suppressesAbilities( ) ) {
            fieldPosition dancers[ NUM_SIDES * 2 ];
            u8            nDancers = 0;
            for( u8 i = 0; i < NUM_SIDES; ++i ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                    fieldPosition pos = { i, j };
                    auto          pk  = getPkmn( i, j );
                    if( pk == nullptr || !pk->canBattle( ) || pos == p_move.m_user ) { continue; }
                    if( pk->getAbility( ) != A_DANCER || slotAbilitiesNullified( i, j ) ) {
                        continue;
                    }
                    if( getVolatileStatus( i, j ) & ( VS_DIGGING | VS_DIVING | VS_INAIR ) ) {
                        continue;
                    }
                    dancers[ nDancers++ ] = pos;
                }
            }
            // insertion sort, ascending storedStats-analog speed
            for( u8 a = 1; a < nDancers; ++a ) {
                for( u8 b = a; b > 0; --b ) {
                    if( getStat( dancers[ b ].first, dancers[ b ].second, SPEED, false ) >= getStat(
                            dancers[ b - 1 ].first, dancers[ b - 1 ].second, SPEED, false ) ) {
                        break;
                    }
                    const fieldPosition t = dancers[ b ];
                    dancers[ b ]          = dancers[ b - 1 ];
                    dancers[ b - 1 ]      = t;
                }
            }
            auto firstUser = getPkmn( p_move.m_user.first, p_move.m_user.second );
            for( u8 d = 0; d < nDancers; ++d ) {
                if( firstUser == nullptr || !firstUser->canBattle( ) ) { break; }
                auto pk = getPkmn( dancers[ d ].first, dancers[ d ].second );
                if( pk == nullptr || !pk->canBattle( ) ) { continue; }
                p_ui->logAbility( pk, bool( dancers[ d ].first ) );
                battleMove nested   = { };
                nested.m_type       = MT_ATTACK;
                nested.m_param      = p_move.m_param;
                nested.m_user       = dancers[ d ];
                nested.m_priority   = p_move.m_moveData.m_priority;
                nested.m_moveData   = p_move.m_moveData;
                nested.m_dancerCopy = true;
                switch( p_move.m_moveData.m_target ) {
                case TG_SELF:
                case TG_ALLY_OR_SELF:
                case TG_ALL_ALLIES: nested.m_target.push_back( dancers[ d ] ); break;
                case TG_ANY:
                case TG_ANY_FOE:
                case TG_ALL_FOES:
                case TG_ALL_FOES_AND_ALLY:
                case TG_RANDOM:
                    nested.m_target.push_back( { u8( !dancers[ d ].first ), 0 } );
                    break;
                default: break;
                }
                bool outer       = _nestedCallsMove;
                _nestedCallsMove = true;
                executeBattleMove( p_ui, nested, std::vector<battleMove>( ),
                                   std::vector<battleMove>( ) );
                _nestedCallsMove = outer;
            }
        }
    }

} // namespace BATTLE
