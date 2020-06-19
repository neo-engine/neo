/*
    Pokémon neo
    ------------------------------

    file        : battleSlot.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

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

#pragma once
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <nds.h>

#include "move.h"
#include "battleDefines.h"
#include "defines.h"
#include "pokemon.h"
#include "type.h"
#include "abilityNames.h"
#include "pokemonNames.h"

namespace BATTLE {
    /*
     * @brief: A slot on the field.
     */
    class slot {
      public:
        enum status {
            EMPTY = 0,
            NORMAL,
            FAINTED,
            RECALLED, // player/ AI recalled their pokemon
        };

      private:
        status        _status;
        slotCondition _slotCondition;
        pokemon*      _pokemon;
        pokemon       _transformedPkmn; // pkmn the pkmn is transformed into
        bool          _isTransformed;
        u8            _volatileStatusCounter[ MAX_VOLATILE_STATUS ];
        u8            _volatileStatusAmount[ MAX_VOLATILE_STATUS ]; // Multiple stockpiles
        boosts        _boosts;
        u16           _turnsInPlay; // Number of turns the pkmn in the slot in participating in the battle

        battleMoveSelection _lockedMove; // move that a pkmn is forced to execute (no op if hib)
        u8         _lockedMoveTurns; // remaining turns the pkmn is locked into _lockedMove
        battleMove _lastMove; // Last used moves
        u16        _disabledMove; // move that was disabled
        pkmnData   _pkmnData;
        std::vector<type> _altTypes; // Type(s) the pkmn changed into
        type       _extraType; // Additional type due to forest's curse or trick-or-treat

      public:
        slot( ) {
            reset( );
            _pokemon = nullptr;
        }

        /*
         * Ages the slot by one turn, processes all changes
         */
        void age( battleUI* p_ui );

        /*
         * @brief: Resets the slot
         */
        inline void reset( ) {
            _turnsInPlay = 0;
            _lockedMove = NO_OP_SELECTION;
            _lockedMoveTurns = 0;
            _status = status( 0 );
            _slotCondition = slotCondition( 0 );
            _isTransformed = false;
            _boosts = boosts( );
            _lastMove = battleMove( );
            _disabledMove = 0;
            _altTypes.clear( );
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            std::memset( _volatileStatusCounter, 0, sizeof( _volatileStatusCounter ) );
            std::memset( _volatileStatusAmount, 0, sizeof( _volatileStatusAmount ) );
        }

        /*
         * @brief: Returns a list of all types the pkmn currently has. May be empty
         */
        inline std::vector<type> getTypes( ) const {
            std::vector<type> res = std::vector<type>( );

            for( u8 i = 0; i < 18; ++i ) {
                if( hasType( type( i ) ) ) {
                    res.push_back( type( i ) );
                }
            }

            return res;
        }

        /*
         * @brief: Checks whether the pkmn currently has the specified type.
         */
        constexpr bool hasType( type p_type ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            if( _volatileStatusCounter[ 54 ] && p_type == type::FLYING ) [[unlikely]] { // Roost
                return false;
            }

            if( _volatileStatusCounter[ 54 ] && p_type == type::NORMAL ) [[unlikely]] { // Roost
                if( _pkmnData.m_baseForme.m_types[ 0 ] == type::FLYING
                        && _pkmnData.m_baseForme.m_types[ 1 ] == type::FLYING ) {
                    return true;
                }
            }

            if( _volatileStatusCounter[ 55 ] && p_type == type::FIRE ) [[unlikely]] { // Burn Up
                return false;
            }

            if( _volatileStatusCounter[ 51 ] ) [[unlikely]] { // Extra type
                if( p_type == _extraType ) {
                    return true;
                }
            }

            if( _volatileStatusCounter[ 56 ] ) [[unlikely]] { // replace type
                for( auto t : _altTypes ) {
                    if( p_type == t ) {
                        return true;
                    }
                }
                return false;
            }

            if( _pkmnData.m_baseForme.m_types[ 0 ] == p_type
                    || _pkmnData.m_baseForme.m_types[ 1 ] == p_type ) {
                return true;
            }
            return false;
        }

        /*
         * @brief: Sets the move the specified pkmn used last.
         */
        inline void setLastUsedMove( battleMove p_move ) {
            _lastMove = p_move;
        }

        /*
         * @brief: Recalls a non-FAINTED pokemon.
         */
        inline void recallPokemon( bool p_keepVolatileStatus = false ) {
            if( !p_keepVolatileStatus ) {
                reset( );
            } else {
                _turnsInPlay = 0;
                _lockedMove = NO_OP_SELECTION;
                _lockedMoveTurns = 0;
                _slotCondition = slotCondition( 0 );
                _isTransformed = false;
                _lastMove = battleMove( );
                _disabledMove = 0;
                _altTypes.clear( );
                std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            }
            _status = RECALLED;
        }

        /*
         * @brief: Sends out a new pokemon to an EMPTY slot.
         */
        bool sendPokemon( pokemon* p_pokemon );

        /*
         * @brief: Adds the specified amount of damage to the pkmn.
         */
        inline void damagePokemon( u16 p_damage ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            if( p_damage >= getPkmn( )->m_stats.m_curHP ) {
                faintPokemon( );
            } else {
                getPkmn( )->m_stats.m_curHP -= p_damage;
                if( _isTransformed ) {
                    _pokemon->m_stats.m_curHP = getPkmn( )->m_stats.m_curHP;
                }
            }
        }

        /*
         * @brief: Heals the specified amount of damage from the pkmn.
         */
        constexpr void healPokemon( u16 p_heal ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            getPkmn( )->m_stats.m_curHP = std::min( getPkmn( )->m_stats.m_maxHP,
                    u16( getPkmn( )->m_stats.m_curHP + p_heal ) );
            if( _isTransformed ) {
                _pokemon->m_stats.m_curHP = getPkmn( )->m_stats.m_curHP;
            }
        }

        /*
         * @brief: Faints the pokemon. Deals necessary damage first.
         */
        inline void faintPokemon( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            reset( );

            getPkmn( )->m_stats.m_curHP = 0;
            getPkmn( )->revertBattleTransform( );
            _status = FAINTED;
        }

        boosts addBoosts( boosts p_boosts, bool p_allowAbilities = true );
        bool   resetBoosts( battleUI* p_ui );
        inline boosts getBoosts( ) const {
            return _boosts;
        }

        constexpr bool setStatusCondition( u8 p_status, u8 p_duration = 255 ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            if( getPkmn( )->m_statusint ) { return false; }

            switch( p_status ) {
                case SLEEP:
                    getPkmn( )->m_status.m_isAsleep = p_duration;
                    if( _isTransformed ) { _pokemon->m_status.m_isAsleep = p_duration; }
                    break;
                case BURN:
                    getPkmn( )->m_status.m_isBurned = true;
                    if( _isTransformed ) { _pokemon->m_status.m_isBurned = true; }
                    break;
                case FROZEN:
                    getPkmn( )->m_status.m_isFrozen = true;
                    if( _isTransformed ) { _pokemon->m_status.m_isFrozen = true; }
                    break;
                case PARALYSIS:
                    getPkmn( )->m_status.m_isParalyzed = true;
                    if( _isTransformed ) { _pokemon->m_status.m_isParalyzed = true; }
                    break;
                case POISON:
                    getPkmn( )->m_status.m_isPoisoned = true;
                    if( _isTransformed ) { _pokemon->m_status.m_isPoisoned = true; }
                    break;
                case TOXIC:
                    getPkmn( )->m_status.m_isBadlyPoisoned = true;
                    if( _isTransformed ) { _pokemon->m_status.m_isBadlyPoisoned = true; }
                    break;
                default:
                    return false;
            }
            return true;
        }

        /*
         * @brief: Removes any status condition a pkmn may have
         */
        constexpr bool removeStatusCondition( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            getPkmn( )->m_statusint = 0;
            if( _isTransformed ) { _pokemon->m_statusint = 0; }

            return true;
        }

        /*
         * @brief: Returns the if the pkmn has the specified status condition.
         */
        constexpr u8 hasStatusCondition( u8 p_status ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            auto& pkmn = _isTransformed ? _transformedPkmn : *_pokemon;

            if( !pkmn.m_statusint ) [[likely]] { return false; }

            switch( p_status ) {
                case SLEEP:
                    return pkmn.m_status.m_isAsleep;
                case BURN:
                    return pkmn.m_status.m_isBurned;
                case FROZEN:
                    return pkmn.m_status.m_isFrozen;
                case PARALYSIS:
                    return pkmn.m_status.m_isParalyzed;
                case POISON:
                    return pkmn.m_status.m_isPoisoned;
                case TOXIC:
                    return pkmn.m_status.m_isBadlyPoisoned;
                default:
                    return false;
            }
        }

        /*
         * @brief: Checks whether the pkmn has the specified ability.
         */
        constexpr bool hasAbility( u16 p_abilityId ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }
            auto& pkmn = _isTransformed ? _transformedPkmn : *_pokemon;
            return pkmn.getAbility( ) == p_abilityId;
        }

        /*
         * @brief: Deduces the specified number of PP from the first move of the specified
         * pkmn that has the specified p_moveId.
         */
        constexpr void deducePP( u16 p_moveId, u8 p_amount ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            for( u8 i = 0; i < 4; ++i ) {
                if( getPkmn( )->getMove( i ) != p_moveId ) { continue; }
                if( getPkmn( )->m_boxdata.m_curPP[ i ] > p_amount ) {
                    getPkmn( )->m_boxdata.m_curPP[ i ] -= p_amount;
                } else {
                    getPkmn( )->m_boxdata.m_curPP[ i ] = 0;
                }
                break;
            }
        }

        constexpr bool addVolatileStatus( volatileStatus p_volatileStatus, u8 p_duration = 255 ) {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1 << i ) ) {
                    _volatileStatusAmount[ i ]++;
                    _volatileStatusCounter[ i ] = p_duration;
                }
            }
            return true;
        }

        constexpr bool removeVolatileStatus( volatileStatus p_volatileStatus ) {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1 << i ) ) {
                    _volatileStatusAmount[ i ] = 0;
                    _volatileStatusCounter[ i ] = 0;
                }
            }
            return true;
        }

        constexpr volatileStatus getVolatileStatus( ) const {
            volatileStatus res = volatileStatus( 0 );
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( _volatileStatusAmount[ i ] ) {
                    res = volatileStatus( res | ( 1 << i ) );
                }
            }
            return res;
        }

        constexpr u8 getVolatileStatusCounter( volatileStatus p_volatileStatus ) const {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1 << i ) ) {
                    return _volatileStatusCounter[ i ];
                }
            }
            return 0;
        }

        bool          addSlotCondition( battleUI* p_ui, slotCondition p_slotCondition,
                                        u8 p_duration );
        constexpr slotCondition getSlotCondition( ) const {
            return _slotCondition;
        }

        /*
         * @brief: Removes any held item.
         */
        constexpr void removeItem( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _pokemon->takeItem( );
            if( _isTransformed ) { _transformedPkmn.takeItem( ); }
        }

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        inline bool isGrounded( bool p_allowAbilities = true ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            bool grn = hasType( FLYING ) ||
                ( p_allowAbilities && getPkmn( )->getAbility( ) == A_LEVITATE )
                || getPkmn( )->getItem( ) == I_AIR_BALLOON
                || _volatileStatusCounter[ 16 ] /* telekinesis */
                || _volatileStatusCounter[ 17 ] /* magnet rise */;

            grn = grn && !( getPkmn( )->getItem( ) == I_IRON_BALL )
                && !_volatileStatusCounter[ 37 ] /* ingrain */
                && !_volatileStatusCounter[ 10 ] /* smack down / Thousand arrows */;

            return grn;
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (current HP: 0, ATK 1, etc)
         * @returns: The numerical value of the stat or a value in [0, 15] for ACCURACY
         * and EVASION, where 7 represents no change.
         */
        constexpr u16 getStat( u8 p_stat, bool p_allowAbilities = true ) {
            if( _pokemon == nullptr ) [[unlikely]] { return 0; }

            if( p_stat == EVASION || p_stat == ACCURACY ) {
                return _boosts.getShiftedBoost( p_stat );
            }

            // base value
            u16 base = getPkmn( )->getStat( p_stat );
            if( p_stat == HP ) { base = getPkmn( )->m_stats.m_curHP; }

            // apply boosts
            s8 bst = _boosts.getBoost( p_stat );
            if( bst > 0 ) {
                base *= bst;
                base >>= 1;
            } else if( bst < 0 ) {
                base *= 2;
                base /= -bst;
            }

            // status (par)
            if( p_stat == SPEED && ( !p_allowAbilities
                        || getPkmn( )->getAbility( ) != A_QUICK_FEET )
                    && getPkmn( )->m_status.m_isParalyzed ) {
                base >>= 1;
            }

            // Abilities
            if( p_allowAbilities ) [[likely]] {
                switch( getPkmn( )->getAbility( ) ) {
                    case A_SLOW_START: [[unlikely]] {
                        if( ( p_stat == ATK || p_stat == SPEED )
                                && _turnsInPlay < 5 ) {
                            base >>= 1;
                        }
                        break;
                    }
                    case A_DEFEATIST:
                        if( p_stat == ATK || p_stat == SATK ) {
                            if( getPkmn( )->m_stats.m_curHP * 2 < getPkmn( )->m_stats.m_maxHP ) {
                                base >>= 1;
                            }
                        }
                        break;
                    case A_PURE_POWER:
                    case A_HUGE_POWER:
                        if( p_stat == ATK ) { base <<= 1; }
                        break;
                    case A_FUR_COAT:
                        if( p_stat == DEF ) { base <<= 1; }
                        break;
                    case A_GORILLA_TACTICS:
                    case A_HUSTLE:
                        if( p_stat == ATK ) { base = 3 * base / 2; }
                        break;
                    case A_GUTS: {
                        if( getPkmn( )->m_statusint ) {
                            if( p_stat == ATK ) { base = 3 * base / 2; }
                        }
                        break;
                    }
                    case A_MARVEL_SCALE: {
                        if( getPkmn( )->m_statusint ) {
                            if( p_stat == DEF ) { base = 3 * base / 2; }
                        }
                        break;
                    }
                    case A_QUICK_FEET: {
                        if( getPkmn( )->m_statusint ) {
                            if( p_stat == SPEED ) { base = 3 * base / 2; }
                        }
                        break;
                    }

                    [[likely]] default:
                        break;
                }
            }

            // Special boosts
            switch( getPkmn( )->getItem( ) ) {
                case I_CHOICE_SCARF:
                    if( p_stat == SPEED ) { base = 3 * base / 2; }
                    break;
                case I_CHOICE_BAND:
                    if( p_stat == ATK ) { base = 3 * base / 2; }
                    break;
                case I_CHOICE_SPECS:
                    if( p_stat == SATK ) { base = 3 * base / 2; }
                    break;
                case I_ASSAULT_VEST:
                    if( p_stat == SDEF ) { base = 3 * base / 2; }
                    break;
                case I_DEEP_SEA_SCALE:
                    if( getPkmn( )->getSpecies( ) == PKMN_CLAMPERL && p_stat == SDEF ) { base <<= 1; }
                    break;
                case I_DEEP_SEA_TOOTH:
                    if( getPkmn( )->getSpecies( ) == PKMN_CLAMPERL && p_stat == SATK ) { base <<= 1; }
                    break;
                case I_LIGHT_BALL:
                    if( getPkmn( )->getSpecies( ) == PKMN_PIKACHU &&
                            ( p_stat == SATK || p_stat == ATK ) ) { base <<= 1; }
                    break;
                case I_QUICK_POWDER:
                    if( getPkmn( )->getSpecies( ) == PKMN_DITTO && !_isTransformed
                            && p_stat == SPEED ) { base <<= 1; }
                    break;
                case I_METAL_POWDER:
                    if( getPkmn( )->getSpecies( ) == PKMN_DITTO && !_isTransformed
                            && p_stat == DEF ) { base <<= 1; }
                    break;
                [[unlikely]] case I_EVIOLITE:
                    if( ( p_stat == SDEF || p_stat == DEF )
                            && !_pokemon->isFullyEvolved( ) ) { base = 3 * base / 2; }
                    break;

                [[likely]] default:
                    break;
            }

            return std::max( base, u16( 1 ) );
        }

        /*
         * @brief: Checks whether the pokemon can move.
         */
        constexpr bool canSelectMove( ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }
            if( ( getVolatileStatus( ) & RECHARGE )
                    || ( getVolatileStatus( ) & CHARGE )
                    || _lockedMoveTurns ) {
                return false;
            }
            return true;
        }

        /*
         * @brief: Checks whether the pokemon can use its i-th move.
         */
        constexpr bool canSelectMove( u8 p_moveIdx ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            auto mv = getPkmn( )->getMove( p_moveIdx );
            if( !mv ) { return false; }


            // pkmn may only use the move it is locked into
            if( _lockedMove.m_type == ATTACK && _lockedMove.m_param != mv ) { return false; }

            if( getPkmn( )->getItem( ) == I_CHOICE_SCARF
                    || getPkmn( )->getItem( ) == I_CHOICE_SPECS
                    || getPkmn( )->getItem( ) == I_CHOICE_BAND ) [[unlikely]] {
                if( _lastMove.m_param && _lastMove.m_param != mv ) {
                    return false;
                }
            }

            if( !getPkmn( )->m_boxdata.m_curPP[ p_moveIdx ] ) {
                return false;
            }

            auto volstat = getVolatileStatus( );

            if( ( volstat & TAUNT ) || getPkmn( )->getItem( ) == I_ASSAULT_VEST ) [[unlikely]] {
                if( MOVE::getMoveData( mv ).m_category == MOVE::STATUS ) {
                    return false;
                }
            }

            if( volstat & DISABLE ) [[unlikely]] { // Disable
                if( _disabledMove == mv ) { return false; }
            }

            if( volstat & TORMENT ) [[unlikely]] { // Torment
                if( _lastMove.m_param == mv ) { return false; }
            }

            return true;
        }

        /*
         * @brief: Locks the specified pkmn to use the specified move for the next
         * p_duration turns.
         */
        inline void addLockedMove( battleMoveSelection p_move, u8 p_duration = 1 ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _lockedMove = p_move;
            _lockedMoveTurns = p_duration;
        }

        /*
         * @brief: Frees the specified pkmn from any move locks.
         */
        inline void removeLockedMove( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _lockedMove = NO_OP_SELECTION;
            _lockedMoveTurns = 0;
        }

        /*
         * @brief: Returns the number of turns the specified pkmn is locked into its
         * current move.
         */
        inline u8 getLockedMoveCount( ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return 0; }

            return _lockedMoveTurns;
        }


        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return NO_OP_SELECTION; }
            return _lockedMove;
        }

        /*
         * @brief: pokemon uses move with the given moveid. Returns false if the move
         * failed (e.g. due to confusion)
         */
        bool useMove( battleUI* p_ui, u16 p_moveId, MOVE::moveData* p_mdata = nullptr );

        /*
         * @brief: Checks whether the pokemon can use an item (from the bag).
         */
        constexpr bool canUseItem( ) const {
            // TODO
            return true;
        }

        /*
         * @brief: Checks whether the pokemon can be switched out.
         */
        inline bool canSwitchOut( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            if( getPkmn( )->getItem( ) == I_SHED_SHELL ) { return true; }

            auto volstat = getVolatileStatus( );
            if( volstat & NORETREAT ) [[unlikely]] { // no retreat
                return false;
            }
            if( volstat & INGRAIN ) [[unlikely]] { // ingrain
                return false;
            }
            if( volstat & OCTOLOCK ) [[unlikely]] { // octo lock
                return false;
            }

            return true;
        }

        /*
         * @brief: Computes a battle move from the given user's and its targets'
         * selections.
         */
        battleMove computeBattleMove( battleMoveSelection               p_usersSelection,
                                      const std::vector<battleMoveSelection>&
                                      p_targetsSelecotions );

        /*
         * @brief: Computes the base damage dealt to each target
         * @param p_targetsMoves: Moves of the targets of the user's move.
         * @param p_targetedMoves: Moves that target the user.
         */
        std::vector<s16> computeDamageDealt( battleMove               p_usersMove,
                                             const std::vector<battleMove>& p_targetsMoves,
                                             const std::vector<battleMove>& p_targetedMoves );

        /*
         * @brief: Computes the actual damage taken by the specified move.
         */
        s16 computeDamageTaken( battleMove p_move, u16 p_baseDamage );

        /*
         * @brief: Computes the recoil the pokemon in the slot would do if hit by the
         * given amount of damage.
         */
        u16 computeRecoil( u16 p_damage );

        /*
         * @brief: Checks whether the pokemon in this slot absorbs the given move
         * (e.g. due to rage powder / storm drain / etc.)
         */
        bool absorbesMove( battleMove p_move, u16 p_baseDamage );

        /*
         * @brief: Sets the pkmn to the specified value.
         */
        inline void setPkmn( pokemon* p_pokemon ) {
            _pokemon = p_pokemon;
            _pkmnData = getPkmnData( p_pokemon->getSpecies( ), p_pokemon->getForme( ) );
            reset( );
        }

        /*
         * @brief: Returns the pkmn currently in the slot (or nullptr if the slot is
         * empty)
         */
        constexpr pokemon* getPkmn( ) {
            if( !_isTransformed ) {
                return _pokemon;
            } else {
                return &_transformedPkmn;
            }
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( pokemon* p_target ) {
            if( _pokemon == nullptr ) [[unlikely]] {
                return false;
            }
            if( p_target != nullptr ) {
                _isTransformed = true;
                _transformedPkmn = *p_target;
                _pkmnData = getPkmnData( _transformedPkmn.getSpecies( ),
                        _transformedPkmn.getForme( ) );

                _pokemon->setBattleTimeAbility( 0 );
                _transformedPkmn.m_stats.m_curHP = _pokemon->m_stats.m_curHP;
                _transformedPkmn.m_stats.m_maxHP = _pokemon->m_stats.m_maxHP;
                _transformedPkmn.m_boxdata.m_heldItem
                    = _pokemon->m_boxdata.m_heldItem;
                std::strncpy( _transformedPkmn.m_boxdata.m_name,
                        _pokemon->m_boxdata.m_name, PKMN_NAMELENGTH );
                for( u8 i = 0; i < 4; ++i ) {
                    _transformedPkmn.m_boxdata.m_curPP[ i ] = 5;
                }
                _transformedPkmn.m_level = _pokemon->m_level;

            } else {
                _isTransformed = false;
                std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            }
            return true;
        }

        /*
         * @brief: Returns true iff the specified pokemon fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( ) {
            return getPkmn( )->canBattleTransform( );
        }

        /*
         * @brief: Changes the ability of the specified pkmn to p_newAbility.
         * Does nothing and returns false if p_newAbility is 0.
         */
        inline bool changeAbility( u16 p_newAbility ) {
            if( !_pokemon ) [[unlikely]] { return false; }
            return _pokemon->setBattleTimeAbility( p_newAbility );
        }

        /*
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }
            return getPkmn( )->getAbility( ) == A_NEUTRALIZING_GAS;
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }
            switch( getPkmn( )->getAbility( ) ) {
                case A_AIR_LOCK:
                case A_CLOUD_NINE:
                    return true;
                [[likely]] default:
                    return false;
            }
        }

    };
}
