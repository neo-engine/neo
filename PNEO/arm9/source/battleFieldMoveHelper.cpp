/*
Pokémon neo
------------------------------

file        : battleFieldMoveHelper.cpp
author      : Philip Wellnitz
description :

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
    u8 field::useMove( battleUI* p_ui, battleMove p_move ) {
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        bool         opponent = p_move.m_user.first;
        u8           slot     = p_move.m_user.second;

        auto volst = getVolatileStatus( opponent, slot );
        registerMoveAction( opponent, slot );
        if( ( p_move.m_param == M_FAKE_OUT || p_move.m_param == M_FIRST_IMPRESSION
              || p_move.m_param == M_MAT_BLOCK )
            && getMoveActions( opponent, slot ) > 1 ) [[unlikely]] {
            p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
            return MOVE_FAIL_NO_PP;
        }

        if( p_move.m_param == M_COUNTER || p_move.m_param == M_MIRROR_COAT ) [[unlikely]] {
            bool ok = ( p_move.m_param == M_MIRROR_COAT )
                          ? ( hasMirrorCoat( opponent, slot ) && mirrorRecorded( opponent, slot ) )
                          : ( hasCounter( opponent, slot ) && counterRecorded( opponent, slot ) );
            if( !ok ) [[unlikely]] {
                p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                return MOVE_FAIL;
            }
        }

        if( p_move.m_param == M_FOCUS_PUNCH && !( volst & VS_FOCUSPUNCH ) ) [[unlikely]] {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_LOST_FOCUS ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );
            return MOVE_FAIL;
        }
        if( p_move.m_param == M_SHELL_TRAP && ( volst & VS_SHELLTRAP ) ) [[unlikely]] {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HARMED_BY_NIGHTMARE ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );
            return MOVE_FAIL;
        }

        if( ( volst & VS_POWDERED ) && p_move.m_moveData.m_type == TYPE_FIRE ) [[unlikely]] {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_POWDER_EXPLODES ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      FS::getMoveName( p_move.m_param ).c_str( ) );
            p_ui->log( buffer );
            u16 maxHP = getPkmn( opponent, slot )->m_stats.m_maxHP;
            u16 blast = ( maxHP + 2 ) / 4;
            if( !blast ) { blast = 1; }
            damagePokemon( p_ui, opponent, slot, blast );
            if( !getPkmn( opponent, slot )->canBattle( ) ) { faintPokemon( p_ui, opponent, slot ); }
            return MOVE_FAIL;
        }

        if( volst & VS_GRUDGE ) [[unlikely]] {
            removeVolatileStatus( p_ui, opponent, slot, VS_GRUDGE );
        }

        if( volst & VS_RECHARGE ) [[unlikely]] {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_RECHARGES ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );

            removeVolatileStatus( p_ui, opponent, slot, VS_RECHARGE );
            removeLockedMove( opponent, slot );
            return MOVE_FAIL_NO_PP;
        }

        if( volst & VS_FLINCH ) [[unlikely]] {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_FLINCHES ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
            p_ui->log( buffer );
            return MOVE_FAIL_NO_PP;
        }

        if( hasStatusCondition( opponent, slot, FROZEN ) ) [[unlikely]] {
            if( p_move.m_moveData.m_type == TYPE_FIRE || ( p_move.m_moveData.m_flags & MF_DEFROST )
                || ( rand( ) % 100 < 20 ) ) {
                // user thaws
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_WAS_THAWED ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                removeStatusCondition( opponent, slot );
                p_ui->updatePkmnStats( opponent, slot, getPkmn( opponent, slot ) );
            } else {
                p_ui->animateStatusCondition( getPkmn( opponent, slot ), opponent, slot, FROZEN );
                return MOVE_FAIL_NO_PP;
            }
        }

        if( !_nestedCallsMove || p_move.m_dancerCopy ) {
            if( u8 slp = hasStatusCondition( opponent, slot, SLEEP ); slp ) {
                if( slp > 1 ) {
                    // pkmn continues to sleep
                    p_ui->animateStatusCondition( getPkmn( opponent, slot ), opponent, slot,
                                                  SLEEP );
                    // force the sleep counter to decrease
                    setStatusCondition( opponent, slot, SLEEP, slp - 1, true );
                    if( !( p_move.m_moveData.m_flags & MF_SLEEPUSABLE ) ) {
                        return MOVE_FAIL_NO_PP;
                    }
                } else {
                    removeStatusCondition( opponent, slot );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_WOKE_UP ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent )
                                  .c_str( ) );
                    p_ui->log( buffer );
                    p_ui->updatePkmnStats( opponent, slot, getPkmn( opponent, slot ) );
                }
            } else if( p_move.m_moveData.m_flags & MF_SLEEPUSABLE ) {
                if( getPkmn( opponent, slot )->getAbility( ) != A_COMATOSE ) {
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_USED_MOVE ) );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ),
                        FS::getMoveName( p_move.m_param ).c_str( ) );
                    p_ui->log( buffer );
                    p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
                    return MOVE_FAIL;
                }
            }
        }

        if( hasStatusCondition( opponent, slot, PARALYSIS ) ) {
            if( rand( ) % 100 < 25 ) {
                p_ui->animateStatusCondition( getPkmn( opponent, slot ), opponent, slot,
                                              PARALYSIS );
                return MOVE_FAIL_NO_PP;
            }
        }

        if( volst & VS_CONFUSION ) [[unlikely]] {
            u8 curVal = getVolatileStatusCounter( opponent, slot, VS_CONFUSION );
            if( curVal > 250 ) {
                --curVal;
                p_ui->animateVolatileStatusCondition( getPkmn( opponent, slot ), opponent, slot,
                                                      VS_CONFUSION );
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_IS_CONFUSED ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                addVolatileStatus( p_ui, opponent, slot, VS_CONFUSION, curVal );

                if( rand( ) % 300 < 100 ) {
                    confusionSelfDamage( p_ui, opponent, slot );
                    p_ui->log( GET_STRING( IO::STR_BATTLE_HURT_ITSELF_IN_CONFUSION ) );
                    return MOVE_FAIL_NO_PP;
                }
            } else {
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_CONFUSION_HEALED ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                removeVolatileStatus( p_ui, opponent, slot, VS_CONFUSION );
            }
        }

        if( ( volst & VS_HEALBLOCK ) && ( p_move.m_moveData.m_flags & MF_HEAL ) ) [[unlikely]] {
            // TODO: this may be wrong
            return MOVE_FAIL;
        }

        if( _pseudoWeatherTimer[ 4 ]
            && ( ( p_move.m_moveData.m_flags & MF_GRAVITY ) || p_move.m_param == M_TELEKINESIS ) )
            [[unlikely]] {
            return MOVE_FAIL;
        }

        if( sealedByImprison( opponent, p_move.m_param ) ) [[unlikely]] {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_SEALED_MOVE ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ),
                      FS::getMoveName( p_move.m_param ).c_str( ) );
            p_ui->log( buffer );
            return MOVE_FAIL;
        }

        if( volst & VS_ATTRACT ) [[unlikely]] {
            if( rand( ) % 100 < 50 ) {
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_IMMOBILIZED_BY_LOVE ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                    p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ) );
                p_ui->log( buffer );
                return MOVE_FAIL_NO_PP;
            }
        }

        if( ( volst & VS_DESTINYBOND ) && p_move.m_param != M_DESTINY_BOND ) [[unlikely]] {
            removeVolatileStatus( p_ui, opponent, slot, VS_DESTINYBOND );
        }

        if( getPkmn( opponent, slot )->getItem( ) == I_METRONOME ) {
            const bool twoTurnVol
                = ( volst & ( VS_MOVECHARGE | VS_DIGGING | VS_DIVING | VS_INAIR ) )
                  != volatileStatus( 0 );
            if( p_move.m_param != M_ASSIST && p_move.m_param != M_METRONOME
                && !( ( p_move.m_moveData.m_flags & MF_CHARGE ) && !twoTurnVol ) ) {
                u8&  num  = _metronomeNum[ opponent ][ slot ];
                u16& last = _metronomeLast[ opponent ][ slot ];
                if( last == p_move.m_param && _actionLastTurn[ opponent ][ slot ] ) {
                    ++num;
                } else if( twoTurnVol ) {
                    num = ( last != p_move.m_param ) ? u8( 1 ) : u8( num + 1 );
                } else {
                    num = 0;
                }
                last = p_move.m_param;
            }
        }

        return MOVE_SUCCESS;
    }
    bool field::moveMisses( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                            bool p_critical ) {
        auto user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return false; }

        auto tgvol    = getVolatileStatus( p_target.first, p_target.second );
        auto usvol    = getVolatileStatus( p_move.m_user.first, p_move.m_user.second );
        bool supprAbs = suppressesAbilities( );

        if( !supprAbs && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
            && user->getAbility( ) == A_NO_GUARD ) {
            return false;
        }

        if( ( tgvol & VS_TELEKINESIS ) && !( p_move.m_moveData.m_flags & MF_OHKO ) ) {
            return false;
        }

        if( tgvol & VS_MINIMIZE ) [[unlikely]] {
            if( minimizeSensitive( p_move.m_param ) ) { return false; }
        }

        if( p_move.m_moveData.m_target == TG_SELF || p_move.m_moveData.m_target == TG_FIELD
            || p_move.m_moveData.m_target == TG_FOE_SIDE
            || p_move.m_moveData.m_target == TG_ALLY_SIDE ) {
            return false;
        }

        if( tgvol & VS_DIGGING ) {
            return !( p_move.m_param == M_EARTHQUAKE || p_move.m_param == M_MAGNITUDE
                      || p_move.m_param == M_FISSURE );
        }
        if( tgvol & VS_DIVING ) {
            return !( p_move.m_param == M_SURF || p_move.m_param == M_WHIRLPOOL );
        }
        if( tgvol & VS_INAIR ) {
            return !( p_move.m_param == M_GUST || p_move.m_param == M_TWISTER
                      || p_move.m_param == M_THUNDER || p_move.m_param == M_THOUSAND_ARROWS
                      || p_move.m_param == M_HURRICANE || p_move.m_param == M_SMACK_DOWN
                      || p_move.m_param == M_SKY_UPPERCUT );
        }
        if( tgvol & VS_INVISIBLE ) { return true; }

        if( p_move.m_moveData.m_flags & MF_OHKOICE ) {
            if( hasType( p_target.first, p_target.second, TYPE_ICE ) ) { return true; }
        }
        if( p_move.m_moveData.m_flags & MF_OHKO ) {
            if( target->m_level > user->m_level ) { return true; }
            return ( rand( ) % 100 ) > ( 20 + user->m_level - target->m_level );
        }

        if( usvol & VS_FORESIGHT ) { return false; }
        if( usvol & VS_MIRACLEEYE ) { return false; }

        u16 acc = p_move.m_moveData.m_accuracy;
        if( !acc || acc > 100 ) { return false; }

        if( !suppressesWeather( ) && getWeather( ) == WE_FOG ) { acc = ( 3 * acc ) / 5; }

        if( getPseudoWeather( ) & PW_GRAVITY ) [[unlikely]] {
            acc = u16( ( u32( acc ) * 6840u ) / 4096u );
        }

        bool ignoreEvasion = !!( p_move.m_moveData.m_flags & MF_IGNOREEVASION ) || p_critical;

        s8 ev = s8( getStat( p_move.m_user.first, p_move.m_user.second, ACCURACY ) - 7 );
        if( !ignoreEvasion ) {
            ev -= s8( getStat( p_target.first, p_target.second, EVASION ) - 7 );
        }

        if( ev < -6 ) { ev = -6; }
        if( ev > 6 ) { ev = 6; }

        if( ev < 0 ) {
            acc = ( acc * 3 ) / ( -ev + 3 );
        } else if( ev > 0 ) {
            acc = ( acc * ( ev + 3 ) ) / 3;
        }

        if( !supprAbs && !breaksAbilities( p_move.m_user.first, p_move.m_user.second ) ) {
            // Victory Star
            if( _sides[ p_move.m_user.first ].anyHasAbility( A_VICTORY_STAR ) ) {
                acc = ( acc * 11 ) / 10;
            }

            // Compound Eyes
            if( !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                && user->getAbility( ) == A_COMPOUND_EYES ) {
                acc = ( acc * 13 ) / 10;
            }

            // Hustle
            if( !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                && user->getAbility( ) == A_HUSTLE
                && p_move.m_moveData.m_category == MH_PHYSICAL ) {
                acc = ( acc * 8 ) / 10;
            }

            // Wonder Skin
            if( !slotAbilitiesNullified( p_target.first, p_target.second )
                && target->getAbility( ) == A_WONDER_SKIN
                && p_move.m_moveData.m_category == MH_STATUS ) {
                acc = 50;
            }
            // Sand Veil
            if( !slotAbilitiesNullified( p_target.first, p_target.second )
                && target->getAbility( ) == A_SAND_VEIL && getWeather( ) == WE_SANDSTORM
                && !suppressesWeather( ) ) {
                acc = ( 4 * acc ) / 5;
            }
            // Snow Cloak
            if( !slotAbilitiesNullified( p_target.first, p_target.second )
                && target->getAbility( ) == A_SNOW_CLOAK && getWeather( ) == WE_HAIL
                && !suppressesWeather( ) ) {
                acc = ( 4 * acc ) / 5;
            }
            // Tangled Feet
            if( !slotAbilitiesNullified( p_target.first, p_target.second )
                && target->getAbility( ) == A_TANGLED_FEET && ( tgvol & VS_CONFUSION ) ) {
                acc >>= 1;
            }
        }

        if( canUseItem( p_target.first, p_target.second ) ) {
            if( target->getItem( ) == I_BRIGHT_POWDER || target->getItem( ) == I_LAX_INCENSE ) {
                acc = ( 9 * acc ) / 10;
            }
        }
        if( canUseItem( p_move.m_user.first, p_move.m_user.second ) ) {
            if( user->getItem( ) == I_WIDE_LENS ) { acc = ( 11 * acc ) / 10; }
            if( user->getItem( ) == I_ZOOM_LENS ) {
                // Zoom Lens
                for( size_t j = 0; j < p_move.m_target.size( ) && j < 8; ++j ) {
                    if( p_move.m_target[ j ] == p_target && ( p_move.m_actsAfter & ( 1U << j ) ) ) {
                        acc = ( 12 * acc ) / 10;
                        break;
                    }
                }
            }
        }

#ifdef DESQUID_MORE
        p_ui->log( std::string( "Accuracy computed " ) + std::to_string( acc ) );
#else
        (void) p_ui;
#endif

        return rand( ) % 100 >= acc;
    }

    bool field::executeCriticalCheck( battleUI* p_ui, battleMove p_move, fieldPosition p_target ) {
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        auto         user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto         target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return false; }

        bool opponent = p_move.m_user.first;
        u8   slot     = p_move.m_user.second;

        bool supprAbs    = suppressesAbilities( );
        auto userVolStat = getVolatileStatus( p_move.m_user.first, p_move.m_user.second );

        // Check for crit suppr.

        if( !supprAbs && !slotAbilitiesNullified( p_target.first, p_target.second )
            && ( target->getAbility( ) == A_BATTLE_ARMOR
                 || target->getAbility( ) == A_SHELL_ARMOR ) ) {
            return false;
        }

        if( getSideCondition( !p_target.first ) & SC_LUCKYCHANT ) { return false; }

        if( userVolStat & VS_LASERFOCUS ) { return true; }
        if( p_move.m_moveData.m_flags & MF_WILLCRIT ) { return true; }

        // Calculate critical hit

        bool critical = false;

        u8 critLevel = p_move.m_moveData.m_critRatio;

        bool item = canUseItem( p_move.m_user.first, p_move.m_user.second );

        if( item ) {
            if( user->getItem( ) == I_RAZOR_CLAW || user->getItem( ) == I_SCOPE_LENS )
                [[unlikely]] {
                critLevel++;
            } else if( user->getSpecies( ) == PKMN_FARFETCH_D
                       || user->getSpecies( ) == PKMN_SIRFETCH_D ) [[unlikely]] {
                if( user->getItem( ) == I_LEEK ) [[unlikely]] { critLevel += 2; }
            } else if( user->getSpecies( ) == PKMN_CHANSEY || user->getSpecies( ) == PKMN_BLISSEY
                       || user->getSpecies( ) == PKMN_HAPPINY ) [[unlikely]] {
                if( user->getItem( ) == I_LUCKY_PUNCH ) [[unlikely]] { critLevel += 2; }
            }
        }

        if( !supprAbs && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
            && user->getAbility( ) == A_SUPER_LUCK ) {
            critLevel++;
        }

        if( item && user->getItem( ) == I_LANSAT_BERRY ) [[unlikely]] {
            u8 factor = 4;
            if( !supprAbs && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                && user->getAbility( ) == A_GLUTTONY ) {
                factor = 2;
            }

            if( user->m_stats.m_curHP * factor < user->m_stats.m_maxHP ) {
                if( supprAbs
                    || !_sides[ opponent ? PLAYER_SIDE : OPPONENT_SIDE ].anyHasAbility(
                        A_UNNERVE ) ) [[likely]] {
                    p_ui->logItem( getPkmnOrDisguise( opponent, slot ), opponent );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_EAT_ITEM ) );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( opponent, slot ), opponent ).c_str( ),
                        FS::getItemName( I_LANSAT_BERRY ).c_str( ) );
                    p_ui->log( buffer );

                    critLevel += 3;

                    removeItem( p_ui, opponent, slot );
                    checkOnEatBerry( p_ui, opponent, slot, I_LANSAT_BERRY );
                }
            }
        } else if( userVolStat & VS_FOCUSENERGY ) {
            critLevel += 2;
        }

        if( critLevel >= 3 ) [[unlikely]] {
            critical = true;
        } else if( critLevel == 2 ) [[unlikely]] {
            critical = !( rand( ) % 2 );
        } else if( critLevel == 1 ) {
            critical = !( rand( ) % 8 );
        } else [[likely]] {
            critical = !( rand( ) % 24 );
        }

        return critical;
    }
    bool field::fatigueConfusionBlocked( bool p_opponent, u8 p_slot ) {
        auto pkmn = getPkmn( p_opponent, p_slot );
        if( pkmn == nullptr ) [[unlikely]] { return true; }
        if( !suppressesAbilities( ) && !slotAbilitiesNullified( p_opponent, p_slot )
            && pkmn->getAbility( ) == A_OWN_TEMPO ) {
            return true;
        }
        if( getTerrain( ) == TR_MISTYTERRAIN && isGrounded( p_opponent, p_slot )
            && !isSemiInvulnerable( p_opponent, p_slot ) ) {
            return true;
        }
        return false;
    }

    bool field::sleepTalkExcluded( u16 p_id ) {
        static constexpr u16 NOSLEEPTALK[] = {
            M_RAZOR_WIND,     M_FLY,           M_SOLAR_BEAM,     M_DIG,           M_MIMIC,
            M_BIDE,           M_METRONOME,     M_MIRROR_MOVE,    M_SKULL_BASH,    M_SKY_ATTACK,
            M_STRUGGLE,       M_SKETCH,        M_SLEEP_TALK,     M_UPROAR,        M_FOCUS_PUNCH,
            M_NATURE_POWER,   M_ASSIST,        M_DIVE,           M_BOUNCE,        M_ME_FIRST,
            M_COPYCAT,        M_CHATTER,       M_SHADOW_FORCE,   M_SKY_DROP,      M_FREEZE_SHOCK,
            M_ICE_BURN,       M_BELCH,         M_PHANTOM_FORCE,  M_GEOMANCY,      M_CELEBRATE,
            M_HOLD_HANDS,     M_SOLAR_BLADE,   M_BEAK_BLAST,     M_SHELL_TRAP,    M_DYNAMAX_CANNON,
            M_BLAZING_TORQUE, M_WICKED_TORQUE, M_NOXIOUS_TORQUE, M_COMBAT_TORQUE, M_MAGICAL_TORQUE,
        };
        for( u16 v : NOSLEEPTALK ) {
            if( v == p_id ) { return true; }
        }
        return false;
    }

    bool field::uproarActive( void ) {
        for( u8 i = 0; i < NUM_SIDES; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto sl = getSlot( i, j );
                if( sl == nullptr ) { continue; }
                if( sl->getLockedMoveCount( ) && sl->getLockedMove( ).m_param == M_UPROAR ) {
                    return true;
                }
            }
        }
        return false;
    }

    bool field::blockSleepByUproar( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
        if( !uproarActive( ) ) { return false; }
        (void) p_opponent;
        (void) p_slot;
        p_ui->log( GET_STRING( IO::STR_BATTLE_IT_FAILED ) );
        return true;
    }

    u8 field::_beatUpAllyBPs( fieldPosition p_user, u16* p_out ) {
        const u8 side  = p_user.first ? OPPONENT_SIDE : PLAYER_SIDE;
        pokemon* team  = _teams[ side ];
        u8       count = 0;
        if( team != nullptr ) {
            for( u8 k = 0; k < _teamSizes[ side ] && count < 6; ++k ) {
                pokemon* a = &team[ k ];
                if( k != p_user.second && ( !a->canBattle( ) || a->m_statusint ) ) { continue; }
                const pkmnData d = FS::getPkmnData( a->getSpecies( ), a->getForme( ) );
                p_out[ count++ ] = u16( 5u + d.m_baseForme.m_bases[ ATK ] / 10u );
            }
        }
        if( !count ) { p_out[ count++ ] = 5; } // user-only fallback (teams
        return count;                          // never empty in practice)
    }

    void field::uproarWakeAll( battleUI* p_ui ) {
        char buffer[ 110 ];
        for( u8 i = 0; i < NUM_SIDES; ++i ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _mode ); ++j ) {
                auto pk = getPkmn( i, j );
                if( pk == nullptr || !pk->canBattle( ) ) { continue; }
                if( !hasStatusCondition( i, j, SLEEP ) ) { continue; }
                removeStatusCondition( i, j );
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_WOKE_UP ) );
                snprintf( buffer, sizeof( buffer ), fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( i, j ), i ).c_str( ) );
                p_ui->log( buffer );
                p_ui->updatePkmnStats( i, j, getPkmn( i, j ) );
            }
        }
    }

    bool field::substituteExempt( battleMove const& p_move, fieldPosition const& p_target ) {
        if( p_move.m_param == M_SUBSTITUTE ) return true;
        if( p_target.first == p_move.m_user.first && p_target.second == p_move.m_user.second )
            return true;
        if( p_move.m_moveData.m_flags & MF_AUTHENTIC ) return true;
        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( user != nullptr && !suppressesAbilities( )
            && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
            && user->getAbility( ) == A_INFILTRATOR )
            return true;
        return false;
    }

    s8 field::effectiveMovePriority( battleMove const& p_move ) {
        auto const& d    = p_move.m_moveData;
        s8          pri  = d.m_priority;
        auto        user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( user != nullptr && !suppressesAbilities( )
            && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second ) ) {
            switch( user->getAbility( ) ) {
            case A_PRANKSTER:
                if( d.m_category == MH_STATUS ) { pri += 1; }
                break;
            case A_GALE_WINGS:
                if( d.m_type == TYPE_FLYING && user->getStat( HP ) == user->m_stats.m_maxHP ) {
                    pri += 1;
                }
                break;
            case A_TRIAGE:
                if( d.m_heal ) { pri += 3; }
                break;
            default: break;
            }
        }
        return pri;
    }

    bool field::redirectSingleTarget( battleUI* p_ui, battleMove& p_move,
                                      fieldPosition& io_target ) {
        if( _mode != BM_DOUBLE || p_move.m_target.size( ) != 1 ) return false;
        if( io_target.first == p_move.m_user.first ) return false;
        auto const& md = p_move.m_moveData;
        if( md.m_target != TG_ANY && md.m_target != TG_ANY_FOE ) return false;

        auto usable = [ & ]( u8 p_slot ) {
            return p_slot != io_target.second && getPkmn( io_target.first, p_slot ) != nullptr
                   && getSlotStatus( io_target.first, p_slot ) == slot::status::NORMAL;
        };
        const u8 count = getBattlingPKMNCount( _mode );

        for( u8 s = 0; s < count; ++s ) { // priority 2
            if( !usable( s ) ) continue;
            if( getVolatileStatus( io_target.first, s ) & VS_SPOTLIGHT ) [[unlikely]] {
                io_target = { io_target.first, s };
                return true;
            }
        }
        for( u8 s = 0; s < count; ++s ) { // priority 1
            if( !usable( s ) ) continue;
            if( getVolatileStatus( io_target.first, s ) & ( VS_FOLLOWME | VS_RAGEPOWDER ) )
                [[unlikely]] {
                io_target = { io_target.first, s };
                return true;
            }
        }
        // priority 0: the type-drawing abilities (TYPE_LIGHTNING is this
        // engine's Electric)
        bool water = ( md.m_type == TYPE_WATER );
        bool elec  = ( md.m_type == TYPE_LIGHTNING );
        if( ( water || elec ) && !suppressesAbilities( ) ) {
            for( u8 s = 0; s < count; ++s ) {
                if( !usable( s ) ) continue;
                auto pk = getPkmn( io_target.first, s );
                if( !slotAbilitiesNullified( io_target.first, s )
                    && ( ( water && pk->getAbility( ) == A_STORM_DRAIN )
                         || ( elec && pk->getAbility( ) == A_LIGHTNING_ROD ) ) ) [[unlikely]] {
                    p_ui->logAbility( pk, io_target.first );
                    io_target = { io_target.first, s };
                    return true;
                }
            }
        }
        return false;
    }
} // namespace BATTLE
