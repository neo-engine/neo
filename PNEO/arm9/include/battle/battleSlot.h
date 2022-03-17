/*
    Pokémon neo
    ------------------------------

    file        : battleSlot.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

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

#pragma once
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <nds.h>

#include "battle/battleDefines.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "fs/data.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "pokemon.h"

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
        u8            _slotConditionCounter[ MAX_SLOT_CONDITIONS ];
        pokemon*      _pokemon;
        pokemon       _transformedPkmn; // pkmn the pkmn is transformed into
        bool          _isTransformed;
        u8            _volatileStatusCounter[ MAX_VOLATILE_STATUS ];
        u8            _volatileStatusAmount[ MAX_VOLATILE_STATUS ]; // Multiple stockpiles
        boosts        _boosts;
        u16           _toxicCount;
        u16 _turnsInPlay; // Number of turns the pkmn in the slot in participating in the battle

        battleMoveSelection _lockedMove; // move that a pkmn is forced to execute (no op if hib)
        u8                  _lockedMoveTurns; // remaining turns the pkmn is locked into _lockedMove
        battleMove          _lastMove;        // Last used moves
        u8                  _consecutiveMoveCount;
        u16                 _disabledMove; // move that was disabled
        pkmnData            _pkmnData;
        std::vector<type>   _altTypes;  // Type(s) the pkmn changed into
        type                _extraType; // Additional type due to forest's curse or trick-or-treat

        u16 _usedItem = 0; // (held) item the pkmn used

      public:
        slot( ) {
            reset( );
            _pokemon = nullptr;
        }

        constexpr u16 getAndIncreaseToxicCount( ) {
            auto res = _toxicCount;
            if( ++_toxicCount >= 15 ) { _toxicCount = 15; }
            return res;
        }

        constexpr u16 getTurnsInPlay( ) {
            return _turnsInPlay;
        }

        /*
         * Ages the slot by one turn, processes all changes
         */
        void age( battleUI* p_ui );

        constexpr pkmnData getPokemonData( ) const {
            return _pkmnData;
        }

        constexpr u8 getConsecutiveMoveCount( ) const {
            return _consecutiveMoveCount;
        }

        constexpr status getSlotStatus( ) const {
            return _status;
        }

        /*
         * @brief: Resets the slot
         */
        inline void reset( ) {
            _usedItem             = 0;
            _turnsInPlay          = 0;
            _toxicCount           = 0;
            _lockedMove           = NO_OP_SELECTION;
            _lockedMoveTurns      = 0;
            _consecutiveMoveCount = 0;
            _status               = status( 0 );
            _slotCondition        = slotCondition( 0 );
            _isTransformed        = false;
            _boosts               = boosts( );
            _lastMove             = battleMove( );
            _disabledMove         = 0;
            _altTypes.clear( );
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            std::memset( _volatileStatusCounter, 0, sizeof( _volatileStatusCounter ) );
            std::memset( _slotConditionCounter, 0, sizeof( _slotConditionCounter ) );
            std::memset( _volatileStatusAmount, 0, sizeof( _volatileStatusAmount ) );
        }

        /*
         * @brief: Sets the type of the specified pkmn.
         */
        inline void setType( battleUI* p_ui, type p_type ) {
            _altTypes = { p_type };
            addVolatileStatus( p_ui, VS_REPLACETYPE, -1 );
        }

        /*
         * @brief: Sets the extra type of the specified pkmn.
         */
        inline void setExtraType( battleUI* p_ui, type p_type ) {
            _extraType = p_type;
            addVolatileStatus( p_ui, VS_EXTRATYPE, -1 );
        }

        /*
         * @brief: Returns a list of all types the pkmn currently has. May be empty
         */
        inline std::vector<type> getTypes( ) const {
            std::vector<type> res = std::vector<type>( );

            for( u8 i = 0; i <= 18; ++i ) {
                if( hasType( type( i ) ) ) { res.push_back( type( i ) ); }
            }

            return res;
        }

        constexpr type getExtraType( ) const {
            return _extraType;
        }

        inline std::vector<type> getAltTypes( ) const {
            return _altTypes;
        }

        /*
         * @brief: Checks whether the pkmn currently has the specified type.
         */
        constexpr bool hasType( type p_type ) const {
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

        /*
         * @brief: Sets the move the specified pkmn used last.
         */
        inline void setLastUsedMove( battleMove p_move ) {
            if( _lastMove.m_param == p_move.m_param ) {
                _consecutiveMoveCount++;
            } else {
                _consecutiveMoveCount = 0;
            }
            _lastMove = p_move;
        }

        inline battleMove getLastUsedMove( ) const {
            return _lastMove;
        }

        /*
         * @brief: Recalls a non-FAINTED pokemon.
         */
        inline void recallPokemon( bool p_keepVolatileStatus = false ) {
            if( !p_keepVolatileStatus ) {
                reset( );
            } else {
                _usedItem             = 0;
                _turnsInPlay          = 0;
                _toxicCount           = 0;
                _lockedMove           = NO_OP_SELECTION;
                _lockedMoveTurns      = 0;
                _slotCondition        = slotCondition( 0 );
                _isTransformed        = false;
                _lastMove             = battleMove( );
                _consecutiveMoveCount = 0;
                _disabledMove         = 0;
                _altTypes.clear( );
                std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            }
            _status = RECALLED;
        }

        /*
         * @brief: Adds the specified amount of damage to the pkmn.
         */
        inline void damagePokemon( u16 p_damage ) {
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

        /*
         * @brief: Heals the specified amount of damage from the pkmn.
         */
        constexpr void healPokemon( u16 p_heal ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

            pkmn->m_stats.m_curHP
                = std::min( pkmn->m_stats.m_maxHP, u16( pkmn->m_stats.m_curHP + p_heal ) );
            if( _isTransformed ) { _pokemon->m_stats.m_curHP = pkmn->m_stats.m_curHP; }
        }

        /*
         * @brief: Faints the pokemon. Deals necessary damage first.
         */
        inline void faintPokemon( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

            pkmn->m_stats.m_curHP = 0;
            pkmn->m_statusint     = 0;
            pkmn->revertBattleTransform( );
            _status = FAINTED;
        }

        constexpr boosts addBoosts( boosts p_boosts, bool p_allowAbilities = true ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr ) { return boosts( ); }

            if( p_allowAbilities && pkmn->getAbility( ) == A_CONTRARY ) { p_boosts.invert( ); }

            return _boosts.addBoosts( p_boosts );
        }
        constexpr bool resetBoosts( ) {
            _boosts = boosts( );
            return true;
        }
        constexpr boosts getBoosts( ) const {
            return _boosts;
        }

        constexpr bool setStatusCondition( u8 p_status, u8 p_duration = 255,
                                           bool p_force = false ) {
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

        /*
         * @brief: Removes any status condition a pkmn may have
         */
        constexpr bool removeStatusCondition( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

            pkmn->m_statusint = 0;
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
            case SLEEP: return pkmn.m_status.m_isAsleep;
            case BURN: return pkmn.m_status.m_isBurned;
            case FROZEN: return pkmn.m_status.m_isFrozen;
            case PARALYSIS: return pkmn.m_status.m_isParalyzed;
            case POISON: return pkmn.m_status.m_isPoisoned;
            case TOXIC: return pkmn.m_status.m_isBadlyPoisoned;
            default: return false;
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
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

            for( u8 i = 0; i < 4; ++i ) {
                if( pkmn->getMove( i ) != p_moveId ) { continue; }
                if( pkmn->m_boxdata.m_curPP[ i ] > p_amount ) {
                    pkmn->m_boxdata.m_curPP[ i ] -= p_amount;
                } else {
                    pkmn->m_boxdata.m_curPP[ i ] = 0;
                }
                break;
            }
        }

        constexpr bool addVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus,
                                          u8 p_duration = 255 ) {
            bool change = false;
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( ( p_volatileStatus & ( 1LLU << i ) )
                    && ( _volatileStatusCounter[ i ] >= 250 || !_volatileStatusAmount[ i ] ) ) {
                    change = true;
                    _volatileStatusAmount[ i ]++;
                    _volatileStatusCounter[ i ] = p_duration;
#ifdef DESQUID_MORE
                    p_ui->log( std::string( "Add volatile status condition " )
                               + std::to_string( 1LLU << i ) + " for "
                               + std::to_string( p_duration ) + " rounds." );
#else
                    (void) p_ui;
#endif
                }
            }
            return change;
        }

        constexpr bool removeVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus ) {
            bool change = false;
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( ( p_volatileStatus & ( 1LLU << i ) ) && _volatileStatusCounter[ i ] ) {
                    change                      = true;
                    _volatileStatusAmount[ i ]  = 0;
                    _volatileStatusCounter[ i ] = 0;
#ifdef DESQUID_MORE
                    p_ui->log( std::string( "Remove volatile status condition " )
                               + std::to_string( 1LLU << i ) );
#else
                    (void) p_ui;
#endif
                }
            }
            return change;
        }

        constexpr volatileStatus getVolatileStatus( ) const {
            volatileStatus res = volatileStatus( 0 );
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( _volatileStatusAmount[ i ] ) { res = volatileStatus( res | ( 1LLU << i ) ); }
            }
            return res;
        }

        constexpr u8 getVolatileStatusAmount( volatileStatus p_volatileStatus ) const {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1LLU << i ) ) { return _volatileStatusAmount[ i ]; }
            }
            return 0;
        }

        constexpr u8 getVolatileStatusCounter( volatileStatus p_volatileStatus ) const {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1LLU << i ) ) { return _volatileStatusCounter[ i ]; }
            }
            return 0;
        }

        inline bool addSlotCondition( battleUI* p_ui, slotCondition p_slotCondition,
                                      u8 p_duration = 0 ) {

            if( ( _slotCondition & p_slotCondition ) == p_slotCondition ) {
                p_ui->log( GET_STRING( 304 ) );
                return false;
            }

#ifdef DESQUID_MORE
            p_ui->log( std::string( "Add slot condition " ) + std::to_string( p_slotCondition ) );
#endif

            for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
                if( ( 1LLU << i ) & p_slotCondition ) {
                    if( !( ( 1LLU << i ) & _slotCondition ) ) {
                        _slotConditionCounter[ i ]
                            = p_duration ? p_duration : defaultSlotConditionDurations[ i ];
                    }
                }
            }
            _slotCondition = slotCondition( _slotCondition | p_slotCondition );
            return true;
        }
        inline bool removeSlotCondition( battleUI* p_ui, slotCondition p_slotCondition ) {
            if( !( _slotCondition & p_slotCondition ) ) { return false; }

            for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
                if( ( 1LLU << i ) & p_slotCondition ) { _slotConditionCounter[ i ] = 0; }
            }
            _slotCondition = slotCondition( _slotCondition & ~p_slotCondition );
#ifdef DESQUID_MORE
            p_ui->log( std::string( "Remove slot condition " )
                       + std::to_string( p_slotCondition ) );
#else
            (void) p_ui;
#endif
            return true;
        }
        constexpr slotCondition getSlotCondition( ) const {
            return _slotCondition;
        }

        constexpr void giveItem( u16 p_item ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _pokemon->giveItem( p_item );
            if( _isTransformed ) { _transformedPkmn.giveItem( p_item ); }
        }

        /*
         * @brief: Removes any held item. Stores it as a used item if p_used is true.
         */
        constexpr void removeItem( bool p_used = true ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            if( p_used ) {
                _usedItem = _pokemon->takeItem( );
            } else {
                _pokemon->takeItem( );
            }
            if( _isTransformed ) { _transformedPkmn.takeItem( ); }
        }

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        inline bool isGrounded( bool p_allowAbilities = true ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return true; }

            bool grn = !( hasType( TYPE_FLYING )
                          || ( p_allowAbilities && pkmn->getAbility( ) == A_LEVITATE )
                          || ( canUseItem( p_allowAbilities ) && pkmn->getItem( ) == I_AIR_BALLOON )
                          || _volatileStatusCounter[ 16 ] /* telekinesis */
                          || _volatileStatusCounter[ 17 ] ) /* magnet rise */;

            grn = grn || ( canUseItem( p_allowAbilities ) && pkmn->getItem( ) == I_IRON_BALL )
                  || _volatileStatusCounter[ 37 ] /* ingrain */
                  || _volatileStatusCounter[ 10 ] /* smack down / Thousand arrows */;

            return grn;
        }

        /*
         * @brief: returns the weight of the specified pkmn.
         */
        inline u16 getWeight( bool p_allowAbilities = true ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr ) [[unlikely]] { return 0; }

            u16 wg = getPokemonData( ).m_baseForme.m_weight;

            if( p_allowAbilities && pkmn->getAbility( ) == A_HEAVY_METAL ) { wg <<= 1; }
            if( p_allowAbilities && pkmn->getAbility( ) == A_LIGHT_METAL ) { wg >>= 1; }

            return wg;
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (current HP: 0, ATK 1, etc)
         * @returns: The numerical value of the stat or a value in [0, 15] for ACCURACY
         * and EVASION, where 7 represents no change.
         */
        constexpr u16 getStat( u8 p_stat, bool p_allowAbilities = true,
                               bool p_ignoreNegative = false, bool p_ignorePositive = false ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return 0; }

            if( p_stat == EVASION || p_stat == ACCURACY ) {
                auto res = _boosts.getShiftedBoost( p_stat );
                if( res < 7 && p_ignoreNegative ) { return 7; }
                if( res > 7 && p_ignorePositive ) { return 7; }

                return res;
            }

            // base value
            u16 base = pkmn->getStat( p_stat );
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

                    [[likely]] default : break;
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
                    if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_CLAMPERL
                        && p_stat == SDEF ) {
                        base <<= 1;
                    }
                    break;
                case I_DEEP_SEA_TOOTH:
                    if( !p_ignorePositive && pkmn->getSpecies( ) == PKMN_CLAMPERL
                        && p_stat == SATK ) {
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
                    [[unlikely]] case I_EVIOLITE
                        : if( !p_ignorePositive && ( p_stat == SDEF || p_stat == DEF )
                              && !_pokemon->isFullyEvolved( ) ) {
                        base = 3 * base / 2;
                    }
                    break;

                    [[likely]] default : break;
                }
            }

            return std::max( base, u16( 1 ) );
        }

        /*
         * @brief: Checks whether the pokemon can move.
         */
        constexpr bool canSelectMove( ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }
            if( ( getVolatileStatus( ) & VS_RECHARGE ) || ( getVolatileStatus( ) & VS_CHARGE )
                || _lockedMoveTurns ) {
                return false;
            }
            return true;
        }

        /*
         * @brief: Checks whether the pokemon can use its i-th move.
         */
        constexpr bool canSelectMove( u8 p_moveIdx ) {
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

            if( volstat & VS_DISABLE ) [[unlikely]] { // Disable
                if( _disabledMove == mv ) { return false; }
            }

            if( volstat & VS_TORMENT ) [[unlikely]] { // Torment
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

            _lockedMove      = p_move;
            _lockedMoveTurns = p_duration;
        }

        /*
         * @brief: Frees the specified pkmn from any move locks.
         */
        inline void removeLockedMove( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _lockedMove      = NO_OP_SELECTION;
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
         * @brief: Checks whether the pokemon can use a held item.
         */
        constexpr bool canUseItem( bool p_allowAbilities = true ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

            if( p_allowAbilities && pkmn->getAbility( ) == A_KLUTZ ) [[unlikely]] { return false; }

            if( getVolatileStatus( ) & VS_EMBARGO ) [[unlikely]] { return false; }
            return true;
        }

        /*
         * @brief: Checks whether the pokemon can be switched out.
         */
        inline bool canSwitchOut( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

            if( canUseItem( ) && pkmn->getItem( ) == I_SHED_SHELL ) [[unlikely]] { return true; }

            auto volstat = getVolatileStatus( );
            if( volstat & VS_NORETREAT ) [[unlikely]] { // no retreat
                return false;
            }
            if( volstat & VS_INGRAIN ) [[unlikely]] { // ingrain
                return false;
            }
            if( volstat & VS_OCTOLOCK ) [[unlikely]] { // octo lock
                return false;
            }

            return true;
        }

        /*
         * @brief: Sets the pkmn to the specified value.
         */
        inline void setPkmn( pokemon* p_pokemon ) {
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

        /*
         * @brief: Returns the pkmn currently in the slot (or nullptr if the slot is
         * empty)
         */
        constexpr pokemon* getPkmn( ) {
            if( _status != NORMAL ) { return nullptr; }

            if( !_isTransformed ) {
                return _pokemon;
            } else {
                return &_transformedPkmn;
            }
        }

        inline void revertTransform( ) {
            if( !_isTransformed ) { return; }

            _isTransformed = false;
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );

            _pkmnData = FS::getPkmnData( _pokemon->getSpecies( ), _pokemon->getForme( ) );
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( slot* p_target ) {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            if( p_target != nullptr ) {
                auto target = p_target->getPkmn( );
                if( target == nullptr ) [[unlikely]] { return false; }

                _isTransformed   = true;
                _transformedPkmn = *target;
                _pkmnData        = FS::getPkmnData( _transformedPkmn.getSpecies( ),
                                                    _transformedPkmn.getForme( ) );

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

        /*
         * @brief: Returns true iff the specified pokemon fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr ) [[unlikely]] { return false; }

            return !_isTransformed && pkmn->canBattleTransform( );
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
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }
            return pkmn->getAbility( ) == A_NEUTRALIZING_GAS;
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }
            switch( pkmn->getAbility( ) ) {
            case A_AIR_LOCK:
            case A_CLOUD_NINE: return true; [[likely]] default : return false;
            }
        }
    };
} // namespace BATTLE
