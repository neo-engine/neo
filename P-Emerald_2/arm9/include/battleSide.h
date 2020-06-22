/*
    Pokémon neo
    ------------------------------

    file        : battleSide.h
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
#include "battleSlot.h"
#include "battleDefines.h"
#include "defines.h"
#include "pokemon.h"
#include "type.h"
#include "abilityNames.h"
#include "pokemonNames.h"

namespace BATTLE {
    /*
     * @brief: A side of the field.
     */
    class side {
      private:
        u8 _sideConditionCounter[ MAX_SIDE_CONDITIONS ]; // Counts turns that side con is active
        u8 _sideConditionAmount[ MAX_SIDE_CONDITIONS ];

        slot _slots[ 2 ];

      public:
        side( ) {
            std::memset( _sideConditionAmount, 0, sizeof( _sideConditionAmount ) );
            std::memset( _sideConditionCounter, 0, sizeof( _sideConditionCounter ) );
        }

        constexpr pkmnData getPkmnData( u8 p_slot ) const {
            return _slots[ p_slot ].getPokemonData( );
        }

        inline slot* getSlot( u8 p_slot ) {
            return &_slots[ p_slot ];
        }

        constexpr slot::status getSlotStatus( u8 p_slot ) const {
            return _slots[ p_slot ].getSlotStatus( );
        }

        /*
         * Ages the side by one turn, processes all changes
         */
        void age( battleUI* p_ui );

        /*
         * @brief: Sets the type of the specified pkmn.
         */
        inline void setType( u8 p_slot, type p_type ) {
            return _slots[ p_slot ].setType( p_type );
        }

        /*
         * @brief: Sets the extra type of the specified pkmn.
         */
        inline void setExtraType( u8 p_slot, type p_type ) {
            return _slots[ p_slot ].setExtraType( p_type );
        }

        /*
         * @brief: Returns a list of all types the pkmn currently has. May be empty
         */
        inline std::vector<type> getTypes( u8 p_slot ) const {
            return _slots[ p_slot ].getTypes( );
        }

        /*
         * @brief: Checks whether the pkmn currently has the specified type.
         */
        constexpr bool hasType( u8 p_slot, type p_type ) const {
            return _slots[ p_slot ].hasType( p_type );
        }


        /*
         * @brief: Tries to add the specified side condition(s).
         * @param p_duration: The duration of the condition in turns. (0 for the
         * defauls amount)
         */
        constexpr bool addSideCondition( sideCondition p_sideCondition, u8 p_duration = 0 ) {
            for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
                if( p_sideCondition & ( 1 << i ) ) {
                    _sideConditionAmount[ i ]++;
                    _sideConditionCounter[ i ] = p_duration;
                }
            }
            return true;
        }

        constexpr bool removeSideCondition( sideCondition p_sideCondition ) {
            for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
                if( p_sideCondition & ( 1 << i ) ) {
                    _sideConditionAmount[ i ] = 0;
                    _sideConditionCounter[ i ] = 0;
                }
            }
            return true;
        }


        constexpr sideCondition getSideCondition( ) const {
            sideCondition res = sideCondition( 0 );
            for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
                if( _sideConditionAmount[ i ] ) {
                    res = sideCondition( res | ( 1 << i ) );
                }
            }
            return res;
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr volatileStatus getVolatileStatus( u8 p_slot ) const {
            return _slots[ p_slot ].getVolatileStatus( );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr u8 getVolatileStatusCounter( u8 p_slot,
                                               volatileStatus p_volatileStatus ) const {
            return _slots[ p_slot ].getVolatileStatusCounter( p_volatileStatus );
        }

        constexpr bool removeVolatileStatus( u8 p_slot,
                                             volatileStatus p_volatileStatus ) {
            return _slots[ p_slot ].removeVolatileStatus( p_volatileStatus );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr bool addVolatileStatus( u8 p_slot, volatileStatus p_volatileStatus,
                                          u8 p_duration ) {
            return  _slots[ p_slot ].addVolatileStatus( p_volatileStatus, p_duration );
        }

        constexpr bool setStatusCondition( u8 p_slot, u8 p_status, u8 p_duration = 255 ) {
            return _slots[ p_slot ].setStatusCondition( p_status, p_duration );
        }

        /*
         * @brief: Removes any status condition a pkmn may have
         */
        constexpr bool removeStatusCondition( u8 p_slot ) {
            return _slots[ p_slot ].removeStatusCondition( );
        }

        /*
         * @brief: Returns the if the pkmn has the specified status condition.
        */
        constexpr u8 hasStatusCondition( u8 p_slot, u8 p_status ) const {
            return _slots[ p_slot ].hasStatusCondition( p_status );
        }

        /*
         * @brief: Sets the pokemon of the specified slot. Only used at battle start
         */
        inline void setSlot( u8 p_slot, pokemon* p_pokemon ) {
            _slots[ p_slot ].setPkmn( p_pokemon );
        }

        /*
         * @brief: Returns the pkmn in the specified slot or nullptr if the slot is empty.
         */
        constexpr pokemon* getPkmn( u8 p_slot ) {
            return _slots[ p_slot ].getPkmn( );
        }

        constexpr u8 getConsecutiveMoveCount( u8 p_slot ) const {
            return _slots[ p_slot ].getConsecutiveMoveCount( );
        }

        /*
         * @brief: Adds the specified boost to the specified pkmn, returns true iff
         * successful
         */
        boosts addBoosts( u8 p_slot, boosts p_boosts, bool p_allowAbilities = true ) {
            return _slots[ p_slot ].addBoosts( p_boosts, p_allowAbilities );
        }
        bool   resetBoosts( u8 p_slot ) {
            return _slots[ p_slot ].resetBoosts( );
        }
        inline boosts getBoosts( u8 p_slot ) const {
            return _slots[ p_slot ].getBoosts( );
        }

        inline void revertTransform( u8 p_slot ) {
            return _slots[ p_slot ].revertTransform( );
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( u8 p_slot, slot* p_target ) {
            return _slots[ p_slot ].transformPkmn( p_target );
        }

        /*
         * @brief: Changes the ability of the specified pkmn to p_newAbility.
         * Does nothing and returns false if p_newAbility is 0.
         */
        inline bool changeAbility( u8 p_slot, u16 p_newAbility ) {
            return _slots[ p_slot ].changeAbility( p_newAbility );
        }

        /*
         * @brief: Checks whether any pkmn on the field has the specified ability.
         */
        constexpr bool anyHasAbility( u16 p_ability ) const {
            return _slots[ 0 ].hasAbility( p_ability )
                || _slots[ 1 ].hasAbility( p_ability );
        }

        /*
         * @brief: Deduces the specified number of PP from the first move of the specified
         * pkmn that has the specified p_moveId.
         */
        constexpr void deducePP( u8 p_slot, u16 p_moveId, u8 p_amount ) {
            _slots[ p_slot ].deducePP( p_moveId, p_amount );
        }

        /*
         * @brief: Locks the specified pkmn to use the specified move for the next
         * p_duration turns.
         */
        inline void addLockedMove( u8 p_slot, battleMoveSelection p_move, u8 p_duration = 1 ) {
            _slots[ p_slot ].addLockedMove( p_move, p_duration );
        }

        /*
         * @brief: Frees the specified pkmn from any move locks.
         */
        inline void removeLockedMove( u8 p_slot ) {
            _slots[ p_slot ].removeLockedMove( );
        }

        /*
         * @brief: Returns the number of turns the specified pkmn is locked into its
         * current move.
         */
        inline u8 getLockedMoveCount( u8 p_slot ) const {
            return _slots[ p_slot ].getLockedMoveCount( );
        }

        /*
         * @brief: Sets the move the specified pkmn used last.
         */
        inline void setLastUsedMove( u8 p_slot, battleMove p_move ) {
            _slots[ p_slot ].setLastUsedMove( p_move );
        }

        inline battleMove getLastUsedMove( u8 p_slot ) const {
            return _slots[ p_slot ].getLastUsedMove( );
        }

        /*
         * @brief: Faints the specified pokemon. Also deals the necessary damage.
         */
        inline void faintPokemon( u8 p_slot ) {
            _slots[ p_slot ].faintPokemon( );
        }

        /*
         * @brief: Recalls the pokemon at the specified position.
         */
        inline void recallPokemon( u8 p_slot, bool p_keepChanges = false ) {
            _slots[ p_slot ].recallPokemon( p_keepChanges );
        }

        /*
         * @brief: Deals the specified amount of damage to the specified pokemon.
         */
        inline void damagePokemon( u8 p_slot, u16 p_damage ) {
            _slots[ p_slot ].damagePokemon( p_damage );
        }

        /*
         * @brief: Heals the specified amount of damage from the specified pokemon.
         */
        inline void healPokemon( u8 p_slot, u16 p_damage ) {
            _slots[ p_slot ].healPokemon( p_damage );
        }


        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (HP: 0, ATK 1, etc)
         */
        constexpr u16 getStat( u8 p_slot, u8 p_stat,
                               bool p_allowAbilities = true,
                               bool p_ignoreNegative = false, bool p_ignorePositive = false ) {
            if( getPkmn( p_slot ) == nullptr ) [[unlikely]] { return 0; }
            u16 base = _slots[ p_slot ].getStat( p_stat, p_allowAbilities,
                                                 p_ignoreNegative, p_ignorePositive );

            if( !p_ignorePositive && p_stat == SPEED
                    && _sideConditionAmount[ 8 ] ) [[unlikely]] { // Tailwind
                base *= 2;
            }

            // plus / minus
            if( p_allowAbilities && !p_ignorePositive ) [[likely]] {
                if( p_stat == SATK && ( getPkmn( p_slot )->getAbility( ) == A_PLUS
                        || getPkmn( p_slot )->getAbility( ) == A_MINUS ) ) [[unlikely]] {
                    auto ot = getPkmn( !p_slot );
                    if( ot != nullptr &&
                            ( ot->getAbility( ) == A_MINUS || ot->getAbility( ) == A_PLUS ) ) {
                        base = 3 * base / 2;
                    }
                }
            }

            return std::max( u16( 1 ), base );
        }

        /*
         * @brief: Checks whether the pokemon can move.
         */
        constexpr bool canSelectMove( u8 p_slot ) const {
            return _slots[ p_slot ].canSelectMove( );
        }
        /*
         * @brief: Checks whether the pokemon can use its i-th move.
         */
        constexpr bool canSelectMove( u8 p_slot, u8 p_moveIdx ) {

            if( getPkmn( !p_slot ) == nullptr
                    && MOVE::getMoveData( getPkmn( p_slot )->getMove( p_moveIdx ) ).m_target
                    == MOVE::ALLY ) [[unlikely]] {
                return false;
            }

            return _slots[ p_slot ].canSelectMove( p_moveIdx );
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( u8 p_slot ) {
            auto tmp = _slots[ p_slot ].getStoredMove( );
            tmp.m_user.second = p_slot;
            return tmp;
        }

        /*
         * @brief: pokemon uses move with the given moveid. Returns false if the move
         * failed (e.g. due to confusion)
         */
        inline bool useMove( battleUI* p_ui, u8 p_slot, u16 p_moveId ) {
            return _slots[ p_slot ].useMove( p_ui, p_moveId );
        }

        /*
         * @brief: Checks whether the pokemon can use an item (from the bag).
         */
        constexpr bool canUseItem( u8 p_slot, bool p_allowAbilities = true ) {
            return _slots[ p_slot ].canUseItem( p_allowAbilities );
        }

        /*
         * @brief: Gives the specified item to the specified pkmn.
         */
        inline void giveItem( u8 p_slot, u16 p_item ) {
            _slots[ p_slot ].giveItem( p_item );
        }

        /*
         * @brief: Removes any held item.
         */
        inline void removeItem( u8 p_slot, bool p_used = true ) {
            _slots[ p_slot ].removeItem( p_used );
        }

        /*
         * @brief: Returns true iff the specified pokemon fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( u8 p_slot ) {
            return _slots[ p_slot ].canMegaEvolve( );
        }

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        inline bool isGrounded( u8 p_slot, bool p_allowAbilities = true ) {
            return _slots[ p_slot ].isGrounded( p_allowAbilities );
        }

        /*
         * @brief: Checks whether the pokemon can be switched out.
         */
        inline bool canSwitchOut( u8 p_slot ) {
            return _slots[ p_slot ].canSwitchOut( );
        }
    };
}
