/*
Pokémon neo
------------------------------

file        : battleSlot.cpp
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

#include "battle/battle.h"
#include "battle/battleField.h"
#include "battle/battleSide.h"
#include "battle/battleSlot.h"
#include "battle/battleUI.h"
#include "defines.h"

namespace BATTLE {
    void slot::age( battleUI* p_ui ) {
        if( _pokemon == nullptr ) [[unlikely]] { return; }

        for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
            if( _slotConditionCounter[ i ] && _slotConditionCounter[ i ] < 250 ) {
                if( !--_slotConditionCounter[ i ] ) {
                    _slotConditionCounter[ i ] = 1;
                    removeSlotCondition( p_ui, slotCondition( 1LLU << i ) );
                }
            }
        }
        for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
            if( _volatileStatusCounter[ i ] && _volatileStatusCounter[ i ] < 250 ) {
                if( !--_volatileStatusCounter[ i ] ) {
                    _volatileStatusCounter[ i ] = 1;
                    removeVolatileStatus( p_ui, volatileStatus( 1LLU << i ) );
                }
            }
        }
        _turnsInPlay++;
    }

    bool slot::hasType( type p_type ) const {
        if( _pokemon == nullptr ) [[unlikely]] { return false; }

        if( _volatileStatusCounter[ 54 ] && p_type == TYPE_FLYING ) [[unlikely]] { // Roost
            return false;
        }

        if( _volatileStatusCounter[ 54 ] && p_type == TYPE_NORMAL ) [[unlikely]] { // Roost
            if( _pkmnData.m_baseForme.m_types[ 0 ] == TYPE_FLYING
                && _pkmnData.m_baseForme.m_types[ 1 ] == TYPE_FLYING ) {
                return true;
            }
        }

        if( _volatileStatusCounter[ 55 ] && p_type == TYPE_FIRE ) [[unlikely]] { // Burn Up
            return false;
        }

        if( _volatileStatusCounter[ 51 ] ) [[unlikely]] { // Extra type
            if( p_type == _extraType ) { return true; }
        }

        if( _volatileStatusCounter[ 56 ] ) [[unlikely]] { // replace type
            for( auto t : _altTypes ) {
                if( p_type == t ) { return true; }
            }
            return false;
        }

        if( _pkmnData.m_baseForme.m_types[ 0 ] == p_type
            || _pkmnData.m_baseForme.m_types[ 1 ] == p_type ) {
            return true;
        }
        return false;
    }

    void slot::recallPokemon( bool p_keepVolatileStatus ) {
        if( !p_keepVolatileStatus ) {
            reset( );
        } else {
            _usedItem                    = 0;
            _turnsInPlay                 = 0;
            _toxicCount                  = 0;
            _lockedMove                  = NO_OP_SELECTION;
            _lockedMoveTurns             = 0;
            _isTransformed               = false;
            _lastMove                    = battleMove( );
            _moveActedThisTurn           = false;
            _consecutiveMoveCount        = 0;
            _disabledMove                = 0;
            _encoreMove                  = 0;
            _perishTimer                 = 0;
            _moveActions                 = 0;
            _stallCounter                = 0;
            _stallWindow                 = 0;
            _counterDamage               = 0;
            _counterFrom                 = 0xFF;
            _mirrorDamage                = 0;
            _mirrorFrom                  = 0xFF;
            _leechSource                 = 0xFF;
            _autotomizeCuts              = 0;
            _volatileStatusCounter[ 46 ] = 0;
            _volatileStatusAmount[ 46 ]  = 0;
            _volatileStatusCounter[ 34 ] = 0;
            _volatileStatusAmount[ 34 ]  = 0;
            _volatileStatusCounter[ 10 ] = 0;
            _volatileStatusAmount[ 10 ]  = 0;
            _volatileStatusCounter[ 28 ] = 0;
            _volatileStatusAmount[ 28 ]  = 0;
            _altTypes.clear( );
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
        }
        _status = RECALLED;
    }

    void slot::damagePokemon( u16 p_damage ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

        if( p_damage >= pkmn->m_stats.m_curHP ) {
            pkmn->m_stats.m_curHP = 0;
            pkmn->m_statusint     = 0;
            if( _isTransformed ) {
                _pokemon->m_stats.m_curHP = 0;
                _pokemon->m_statusint     = 0;
            }
        } else {
            pkmn->m_stats.m_curHP -= p_damage;
            if( _isTransformed ) { _pokemon->m_stats.m_curHP = pkmn->m_stats.m_curHP; }
        }
    }

    void slot::restoreAllMovePP( ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

        for( u8 i = 0; i < 4; ++i ) {
            if( pkmn->m_boxdata.m_moves[ i ] == 0 ) { continue; }
            auto mdata = FS::getMoveData( pkmn->m_boxdata.m_moves[ i ] );
            pkmn->m_boxdata.m_curPP[ i ]
                = u8( mdata.m_pp * ( 5 + pkmn->m_boxdata.PPupget( i ) ) / 5 );
        }
    }

    bool slot::hasMovePPDeficit( ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

        for( u8 i = 0; i < 4; ++i ) {
            if( pkmn->m_boxdata.m_moves[ i ] == 0 ) { continue; }
            auto mdata = FS::getMoveData( pkmn->m_boxdata.m_moves[ i ] );
            if( pkmn->m_boxdata.m_curPP[ i ]
                < u8( mdata.m_pp * ( 5 + pkmn->m_boxdata.PPupget( i ) ) / 5 ) ) {
                return true;
            }
        }
        return false;
    }

    void slot::faintPokemon( ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

        pkmn->m_stats.m_curHP = 0;
        pkmn->m_statusint     = 0;
        pkmn->revertBattleTransform( );
        _status       = FAINTED;
        _substituteHp = 0;
    }

    bool slot::setStatusCondition( u8 p_status, u8 p_duration, bool p_force ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

        if( !p_force && pkmn->m_statusint ) { return false; }

        switch( p_status ) {
        case SLEEP:
            if( pkmn->getAbility( ) == A_COMATOSE ) { return false; }
            pkmn->m_status.m_isAsleep = p_duration;
            if( _isTransformed ) { _pokemon->m_status.m_isAsleep = p_duration; }
            break;
        case BURN:
            if( hasType( TYPE_FIRE ) ) { return false; }
            pkmn->m_status.m_isBurned = true;
            if( _isTransformed ) { _pokemon->m_status.m_isBurned = true; }
            break;
        case FROZEN:
            if( hasType( TYPE_ICE ) ) { return false; }
            pkmn->m_status.m_isFrozen = true;
            if( _isTransformed ) { _pokemon->m_status.m_isFrozen = true; }
            break;
        case PARALYSIS:
            if( hasType( TYPE_LIGHTNING ) ) { return false; }
            pkmn->m_status.m_isParalyzed = true;
            if( _isTransformed ) { _pokemon->m_status.m_isParalyzed = true; }
            break;
        case POISON:
            // p_duration == 254 iff pkmn causing poison has corrosion ability
            if( hasType( TYPE_POISON ) && p_duration != 254 ) { return false; }
            if( hasType( TYPE_STEEL ) && p_duration != 254 ) { return false; }
            pkmn->m_status.m_isPoisoned = true;
            if( _isTransformed ) { _pokemon->m_status.m_isPoisoned = true; }
            break;
        case TOXIC:
            // p_duration == 254 iff pkmn causing poison has corrosion ability
            if( hasType( TYPE_POISON ) && p_duration != 254 ) { return false; }
            if( hasType( TYPE_STEEL ) && p_duration != 254 ) { return false; }
            _toxicCount                      = 1;
            pkmn->m_status.m_isBadlyPoisoned = true;
            if( _isTransformed ) { _pokemon->m_status.m_isBadlyPoisoned = true; }
            break;
        default: return false;
        }
        return true;
    }

    bool slot::addVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus, u8 p_duration ) {
        bool change = false;
        for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
            if( ( p_volatileStatus & ( 1LLU << i ) )
                && ( _volatileStatusCounter[ i ] >= 250 || !_volatileStatusAmount[ i ] ) ) {
                change = true;
                _volatileStatusAmount[ i ]++;
                _volatileStatusCounter[ i ] = p_duration;
#ifdef DESQUID_MORE
                p_ui->log( std::string( "Add volatile status condition " )
                           + std::to_string( 1LLU << i ) + " for " + std::to_string( p_duration )
                           + " rounds." );
#else
                (void) p_ui;
#endif
            }
        }
        return change;
    }

    bool slot::removeSlotCondition( battleUI* p_ui, slotCondition p_slotCondition ) {
        if( !( _slotCondition & p_slotCondition ) ) { return false; }

        for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
            if( ( 1LLU << i ) & p_slotCondition ) {
                _slotConditionCounter[ i ] = 0;
                _slotConditionAmount[ i ]  = 0;
            }
        }
        if( p_slotCondition & SL_WISH ) { _slotConditionWisher.clear( ); }
        if( p_slotCondition & SL_FUTURESIGHT ) { _slotConditionSrc = 0xFFFF; }
        _slotCondition = slotCondition( _slotCondition & ~p_slotCondition );
#ifdef DESQUID_MORE
        p_ui->log( std::string( "Remove slot condition " ) + std::to_string( p_slotCondition ) );
#else
        (void) p_ui;
#endif
        return true;
    }

    bool slot::isGrounded( bool p_allowAbilities ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return true; }

        bool grn = !(
            hasType( TYPE_FLYING )
            || ( p_allowAbilities && !abilitiesNullified( ) && pkmn->getAbility( ) == A_LEVITATE )
            || ( canUseItem( p_allowAbilities ) && pkmn->getItem( ) == I_AIR_BALLOON )
            || _volatileStatusCounter[ 16 ] /* telekinesis */
            || _volatileStatusCounter[ 17 ] ) /* magnet rise */;

        grn = grn || ( canUseItem( p_allowAbilities ) && pkmn->getItem( ) == I_IRON_BALL )
              || _volatileStatusCounter[ 37 ] /* ingrain */
              || _volatileStatusCounter[ 10 ] /* smack down / Thousand arrows */;

        return grn;
    }

    u16 slot::getWeight( bool p_allowAbilities ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr ) [[unlikely]] { return 0; }

        u16 wg = getPokemonData( ).m_baseForme.m_weight;

        if( p_allowAbilities && pkmn->getAbility( ) == A_HEAVY_METAL ) { wg <<= 1; }
        if( p_allowAbilities && pkmn->getAbility( ) == A_LIGHT_METAL ) { wg >>= 1; }

        for( u8 i = 0; i < _autotomizeCuts; ++i ) {
            if( wg > 1000 ) {
                wg = u16( wg - 1000 );
            } else {
                wg = 1;
            }
        }

        return wg;
    }

    u16 slot::getStat( u8 p_stat, bool p_allowAbilities, bool p_ignoreNegative,
                       bool p_ignorePositive, bool p_wonderRoom ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return 0; }

        if( p_stat == EVASION || p_stat == ACCURACY ) {
            auto res = _boosts.getShiftedBoost( p_stat );
            if( res < 7 && p_ignoreNegative ) { return 7; }
            if( res > 7 && p_ignorePositive ) { return 7; }

            return res;
        }

        u8 src = p_stat;
        if( p_wonderRoom ) [[unlikely]] {
            if( src == DEF ) {
                src = SDEF;
            } else if( src == SDEF ) {
                src = DEF;
            }
        }
        if( _volatileStatusCounter[ 20 ] && ( src == ATK || src == DEF ) ) [[unlikely]] {
            src = u8( src == ATK ? DEF : ATK );
        }
        u16 base = pkmn->getStat( src );
        if( p_stat == HP ) { base = pkmn->m_stats.m_curHP; }

        // apply boosts
        s8 bst = _boosts.getBoost( p_stat );
        if( bst > 0 && !p_ignorePositive ) {
            base *= ( bst + 2 );
            base >>= 1;
        } else if( bst < 0 && !p_ignoreNegative ) {
            base *= 2;
            base /= ( -bst + 2 );
        }

        // status (par)
        if( !p_ignoreNegative && p_stat == SPEED
            && ( !p_allowAbilities || pkmn->getAbility( ) != A_QUICK_FEET )
            && pkmn->m_status.m_isParalyzed ) {
            base >>= 1;
        }

        // Abilities
        if( p_allowAbilities ) [[likely]] {
            switch( pkmn->getAbility( ) ) {
            case A_UNBURDEN: {
                if( !p_ignorePositive && p_stat == SPEED && _usedItem && !pkmn->getItem( ) )
                    [[unlikely]] {
                    base <<= 1;
                }
                break;
            }
            case A_SLOW_START:
                [[unlikely]] {
                    if( !p_ignoreNegative && ( p_stat == ATK || p_stat == SPEED )
                        && _turnsInPlay < 5 ) {
                        base >>= 1;
                    }
                    break;
                }
            case A_DEFEATIST:
                if( !p_ignoreNegative && ( p_stat == ATK || p_stat == SATK ) ) {
                    if( pkmn->m_stats.m_curHP * 2 < pkmn->m_stats.m_maxHP ) { base >>= 1; }
                }
                break;
            case A_PURE_POWER:
            case A_HUGE_POWER:
                if( !p_ignorePositive && p_stat == ATK ) { base <<= 1; }
                break;
            case A_FUR_COAT:
                if( !p_ignorePositive && p_stat == DEF ) { base <<= 1; }
                break;
            case A_GORILLA_TACTICS:
            case A_HUSTLE:
                if( !p_ignorePositive && p_stat == ATK ) { base = 3 * base / 2; }
                break;
            case A_GUTS: {
                if( !p_ignorePositive && pkmn->m_statusint ) {
                    if( p_stat == ATK ) { base = 3 * base / 2; }
                }
                break;
            }
            case A_MARVEL_SCALE: {
                if( !p_ignorePositive && pkmn->m_statusint ) {
                    if( p_stat == DEF ) { base = 3 * base / 2; }
                }
                break;
            }
            case A_QUICK_FEET: {
                if( !p_ignorePositive && pkmn->m_statusint ) {
                    if( p_stat == SPEED ) { base = 3 * base / 2; }
                }
                break;
            }

            [[likely]] default:
                break;
            }
        }

        // Special boosts
        if( canUseItem( p_allowAbilities ) ) [[likely]] {
            switch( pkmn->getItem( ) ) {
            case I_CHOICE_SCARF:
                if( !p_ignorePositive && p_stat == SPEED ) { base = 3 * base / 2; }
                break;
            case I_CHOICE_BAND:
                if( !p_ignorePositive && p_stat == ATK ) { base = 3 * base / 2; }
                break;
            case I_CHOICE_SPECS:
                if( !p_ignorePositive && p_stat == SATK ) { base = 3 * base / 2; }
                break;
            case I_ASSAULT_VEST:
                if( !p_ignorePositive && p_stat == SDEF ) { base = 3 * base / 2; }
                break;
            case I_DEEP_SEA_SCALE:
                if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_CLAMPERL && p_stat == SDEF ) {
                    base <<= 1;
                }
                break;
            case I_DEEP_SEA_TOOTH:
                if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_CLAMPERL && p_stat == SATK ) {
                    base <<= 1;
                }
                break;
            case I_LIGHT_BALL:
                if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_PIKACHU
                    && ( p_stat == SATK || p_stat == ATK ) ) {
                    base <<= 1;
                }
                break;
            case I_QUICK_POWDER:
                if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_DITTO && !_isTransformed
                    && p_stat == SPEED ) {
                    base <<= 1;
                }
                break;
            case I_METAL_POWDER:
                if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_DITTO && !_isTransformed
                    && p_stat == DEF ) {
                    base <<= 1;
                }
                break;
            [[unlikely]] case I_EVIOLITE:
                if( !p_ignorePositive && ( p_stat == SDEF || p_stat == DEF )
                    && !_pokemon->isFullyEvolved( ) ) {
                    base = 3 * base / 2;
                }
                break;

            [[likely]] default:
                break;
            }
        }

        return std::max( base, u16( 1 ) );
    }

    bool slot::canSelectMove( ) const {
        if( _pokemon == nullptr ) [[unlikely]] { return false; }
        if( ( getVolatileStatus( ) & VS_RECHARGE ) || _lockedMoveTurns ) { return false; }
        return true;
    }

    bool slot::canSelectMove( u8 p_moveIdx ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

        auto mv = pkmn->getMove( p_moveIdx );
        if( !mv ) { return false; }

        // pkmn may only use the move it is locked into
        if( _lockedMove.m_type == MT_ATTACK && _lockedMove.m_param != mv ) { return false; }

        if( pkmn->getItem( ) == I_CHOICE_SCARF || pkmn->getItem( ) == I_CHOICE_SPECS
            || pkmn->getItem( ) == I_CHOICE_BAND ) [[unlikely]] {
            if( _lastMove.m_param && _lastMove.m_param != mv ) { return false; }
        }

        if( !pkmn->m_boxdata.m_curPP[ p_moveIdx ] ) { return false; }

        auto volstat = getVolatileStatus( );

        if( ( volstat & VS_TAUNT ) || pkmn->getItem( ) == I_ASSAULT_VEST ) [[unlikely]] {
            if( FS::getMoveData( mv ).m_category == MH_STATUS ) { return false; }
        }

        if( volstat & VS_DISABLE ) [[unlikely]] {
            if( _disabledMove == mv ) { return false; }
        }

        if( volstat & VS_ENCORE ) [[unlikely]] {
            if( _encoreMove && mv != _encoreMove ) {
                bool inSet = false;
                for( u8 s = 0; s < 4; ++s ) {
                    if( pkmn->getMove( s ) == _encoreMove ) {
                        inSet = true;
                        break;
                    }
                }
                if( inSet ) { return false; }
            }
        }

        if( volstat & VS_TORMENT ) [[unlikely]] {
            if( _lastMove.m_param == mv ) { return false; }
        }

        return true;
    }

    bool slot::canSwitchOut( ) {
        auto pkmn = getPkmn( );
        if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

        if( canUseItem( ) && pkmn->getItem( ) == I_SHED_SHELL ) [[unlikely]] { return true; }

        auto volstat = getVolatileStatus( );
        if( volstat & VS_NORETREAT ) [[unlikely]] { return false; }
        if( volstat & VS_INGRAIN ) [[unlikely]] { return false; }
        if( volstat & VS_OCTOLOCK ) [[unlikely]] { return false; }
        if( volstat & VS_PARTIALLYTRAPPED ) [[unlikely]] { return false; }

        return true;
    }

    void slot::setPkmn( pokemon* p_pokemon ) {
        if( !p_pokemon ) {
            _status = EMPTY;
            reset( );
            return;
        }
        _pokemon  = p_pokemon;
        _pkmnData = FS::getPkmnData( p_pokemon->getSpecies( ), p_pokemon->getForme( ) );
        reset( );
        _status = NORMAL;
    }

    pokemon* slot::getPkmnOrDisguise( ) const {
        if( _status != NORMAL || _pokemon == nullptr ) { return nullptr; }

        static pokemon result;

        if( !_isTransformed ) {
            result = *_pokemon;
        } else {
            result = _transformedPkmn;
        }

        if( isDisguised( ) ) {
            result.m_boxdata.m_speciesId = _disguise->getSpecies( );
            result.m_boxdata.m_altForme  = _disguise->getForme( );
            result.m_boxdata.m_shinyType = _disguise->m_boxdata.m_shinyType;
            result.m_boxdata.m_pid       = _disguise->m_boxdata.m_pid;
            result.m_boxdata.m_oTId      = _disguise->m_boxdata.m_oTId;
            result.m_boxdata.m_oTSid     = _disguise->m_boxdata.m_oTSid;
            std::strncpy( result.m_boxdata.m_name, _disguise->m_boxdata.m_name, PKMN_NAMELENGTH );
        }
        return &result;
    }

    bool slot::transformPkmn( slot* p_target ) {
        if( _pokemon == nullptr ) [[unlikely]] { return false; }

        if( p_target != nullptr ) {
            auto target = p_target->getPkmn( );
            if( target == nullptr ) [[unlikely]] { return false; }

            _isTransformed   = true;
            _transformedPkmn = *target;
            _pkmnData
                = FS::getPkmnData( _transformedPkmn.getSpecies( ), _transformedPkmn.getForme( ) );

            _pokemon->setBattleTimeAbility( 0 );
            _transformedPkmn.m_stats.m_curHP      = _pokemon->m_stats.m_curHP;
            _transformedPkmn.m_stats.m_maxHP      = _pokemon->m_stats.m_maxHP;
            _transformedPkmn.m_boxdata.m_heldItem = _pokemon->m_boxdata.m_heldItem;
            std::strncpy( _transformedPkmn.m_boxdata.m_name, _pokemon->m_boxdata.m_name,
                          PKMN_NAMELENGTH );
            for( u8 i = 0; i < 4; ++i ) { _transformedPkmn.m_boxdata.m_curPP[ i ] = 5; }
            _transformedPkmn.m_level = _pokemon->m_level;

            auto volstat = p_target->getVolatileStatus( );
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( volstat & ( 1LLU << i ) ) {
                    _volatileStatusAmount[ i ]
                        = p_target->getVolatileStatusAmount( volatileStatus( 1LLU << i ) );
                    _volatileStatusCounter[ i ]
                        = p_target->getVolatileStatusCounter( volatileStatus( 1LLU << i ) );
                }
            }

            _extraType = p_target->getExtraType( );
            _altTypes  = p_target->getAltTypes( );
            _boosts    = p_target->getBoosts( );
        } else {
            _isTransformed = false;
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
        }
        return true;
    }
} // namespace BATTLE
