/*
   Pokémon neo
   ------------------------------

file        : battleField.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2021
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

#include "abilityNames.h"
#include "battleDefines.h"
#include "battleSide.h"
#include "defines.h"
#include "move.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "type.h"

namespace BATTLE {
    /*
     * @brief: The overall field where the battle takes place.
     */
    class field {
      public:
        static constexpr u8 PLAYER_SIDE       = 0;
        static constexpr u8 OPPONENT_SIDE     = 1;
        static constexpr u8 NORMAL_DURATION   = 5;
        static constexpr u8 EXTENDED_DURATION = 8;

      private:
        weather _weather;
        u8      _weatherTimer; // Number of turns the weather is still active

        u8 _pseudoWeatherTimer[ MAX_PSEUDO_WEATHER ];

        terrain _terrain;
        u8      _terrainTimer;

        side _sides[ 2 ];

        battleMode _mode;

      public:
        field( battleMode p_battleMode = SINGLE, weather p_initialWeather = NO_WEATHER,
               pseudoWeather p_initialPseudoWeather = NO_PSEUDO_WEATHER,
               terrain       p_initialTerrain       = NO_TERRAIN );

        /*
         * Initializes the field.
         */
        void init( battleUI* p_ui );
        /*
         * Ages the field by one turn, processes all weather changes
         */
        void age( battleUI* p_ui );

        constexpr u16 getTurnsInPlay( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getTurnsInPlay( p_slot );
        }

        constexpr u16 getAndIncreaseToxicCount( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getAndIncreaseToxicCount(
                p_slot );
        }

        constexpr pkmnData getPkmnData( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPkmnData( p_slot );
        }

        inline slot* getSlot( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlot( p_slot );
        }

        constexpr slot::status getSlotStatus( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotStatus( p_slot );
        }

        /*
         * @brief: Tries to set a new weather; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool setWeather( battleUI* p_ui, weather p_newWeather, bool p_extended = false );
        bool removeWeather( battleUI* p_ui );
        void continueWeather( battleUI* p_ui );
        constexpr weather getWeather( ) const {
            return _weather;
        }

        /*
         * @brief: Tries to set a new pseudo weather; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool setPseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather,
                               bool p_extended = false );
        bool removePseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather );
        constexpr pseudoWeather getPseudoWeather( ) const {
            pseudoWeather res = pseudoWeather( 0 );
            for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
                if( _pseudoWeatherTimer[ i ] ) { res = pseudoWeather( res | ( 1LLU << i ) ); }
            }
            return res;
        }

        /*
         * @brief: Tries to set a new terrain; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool setTerrain( battleUI* p_ui, terrain p_newTerrain, bool p_extended = false );
        bool removeTerrain( battleUI* p_ui );
        constexpr terrain getTerrain( ) const {
            return _terrain;
        }

        constexpr sideCondition getSideCondition( bool p_opponent ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSideCondition( );
        }

        /*
         * @brief: Adds the given side conditions to the specified side
         */
        inline bool addSideCondition( battleUI* p_ui, bool p_opponent,
                                      sideCondition p_sideCondition, u8 p_duration = 0 ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addSideCondition(
                p_ui, p_sideCondition, p_duration );
        }

        /*
         * @brief: Removes the given side conditions from the specified side
         */
        inline bool removeSideCondition( battleUI* p_ui, bool p_opponent,
                                         sideCondition p_sideCondition ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeSideCondition(
                p_ui, p_sideCondition );
        }

        /*
         * @brief: Adds the given slot conditions to the specified slot
         */
        inline bool addSlotCondition( battleUI* p_ui, bool p_opponent, u8 p_pos,
                                      slotCondition p_slotCondition, u8 p_duration = 0 ) {
            // TODO: proper log
            p_ui->log( "Set slot condition " + std::to_string( u8( p_slotCondition ) ) );

            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addSlotCondition(
                p_ui, p_pos, p_slotCondition, p_duration );
        }

        /*
         * @brief: Removes the given slot conditions from the specified slot
         */
        inline bool removeSideCondition( battleUI* p_ui, bool p_opponent, u8 p_pos,
                                         slotCondition p_slotCondition ) {
            // TODO: proper log
            p_ui->log( "Remove slot condition " + std::to_string( u8( p_slotCondition ) ) );

            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeSlotCondition(
                p_ui, p_pos, p_slotCondition );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr volatileStatus getVolatileStatus( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getVolatileStatus( p_slot );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr u8 getVolatileStatusCounter( bool p_opponent, u8 p_slot,
                                               volatileStatus p_volatileStatus ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getVolatileStatusCounter(
                p_slot, p_volatileStatus );
        }

        constexpr bool removeVolatileStatus( battleUI* p_ui, bool p_opponent, u8 p_slot,
                                             volatileStatus p_volatileStatus ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeVolatileStatus(
                p_ui, p_slot, p_volatileStatus );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr bool addVolatileStatus( battleUI* p_ui, bool p_opponent, u8 p_slot,
                                          volatileStatus p_volatileStatus, u8 p_duration ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addVolatileStatus(
                p_ui, p_slot, p_volatileStatus, p_duration );
        }

        constexpr bool setStatusCondition( bool p_opponent, u8 p_slot, u8 p_status,
                                           u8 p_duration = 255 ) {

            auto pkmn = getPkmn( p_opponent, p_slot );
            if( pkmn == nullptr ) [[unlikely]] { return false; }

            if( !suppressesAbilities( ) && !suppressesWeather( )
                && pkmn->getAbility( ) == A_LEAF_GUARD
                && ( getWeather( ) == SUN || getWeather( ) == HEAVY_SUNSHINE ) ) {
                return false;
            }

            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setStatusCondition(
                p_slot, p_status, p_duration );
        }

        /*
         * @brief: Removes any status condition a pkmn may have
         */
        constexpr bool removeStatusCondition( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeStatusCondition(
                p_slot );
        }

        /*
         * @brief: Returns the if the pkmn has the specified status condition.
         */
        constexpr u8 hasStatusCondition( bool p_opponent, u8 p_slot, u8 p_status ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasStatusCondition(
                p_slot, p_status );
        }

        /*
         * @brief: Locks the specified pkmn to use the specified move for the next
         * p_duration turns.
         */
        inline void addLockedMove( bool p_opponent, u8 p_slot, battleMoveSelection p_move,
                                   u8 p_duration = 1 ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addLockedMove( p_slot, p_move,
                                                                              p_duration );
        }

        /*
         * @brief: Frees the specified pkmn from any move locks.
         */
        inline void removeLockedMove( bool p_opponent, u8 p_slot ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeLockedMove( p_slot );
        }

        /*
         * @brief: Returns the number of turns the specified pkmn is locked into its
         * current move.
         */
        inline u8 getLockedMoveCount( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getLockedMoveCount( p_slot );
        }

        /*
         * @brief: Sets the move the specified pkmn used last.
         */
        inline void setLastUsedMove( bool p_opponent, u8 p_slot, battleMove p_move ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setLastUsedMove( p_slot, p_move );
        }

        inline battleMove getLastUsedMove( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getLastUsedMove( p_slot );
        }

        /*
         * @brief: Faints the specified pokemon. Also deals the necessary damage.
         */
        inline void faintPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
            auto pkmn = getPkmn( p_opponent, p_slot );
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].faintPokemon( p_slot );
            p_ui->updatePkmnStats( p_opponent, p_slot, pkmn, false );
            p_ui->faintPkmn( p_opponent, p_slot, pkmn );
        }

        /*
         * @brief: Recalls the pokemon at the specified position.
         */
        inline void recallPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot,
                                   bool p_keepChanges = false, bool p_forced = false ) {
            p_ui->recallPkmn( p_opponent, p_slot, getPkmn( p_opponent, p_slot ), p_forced );
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].recallPokemon( p_slot,
                                                                              p_keepChanges );
        }

        /*
         * @brief: Deals the specified amount of damage to the specified pokemon. Does not
         * faint pokemon.
         */
        inline void damagePokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_damage ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].damagePokemon( p_slot, p_damage );
            p_ui->updatePkmnStats( p_opponent, p_slot, getPkmn( p_opponent, p_slot ), false );
        }

        /*
         * @brief: Deals the specified amount of damage to the specified pokemon.
         */
        inline void healPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_damage ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].healPokemon( p_slot, p_damage );
            p_ui->updatePkmnStats( p_opponent, p_slot, getPkmn( p_opponent, p_slot ), false );
        }

        /*
         * @brief: Sorts the give moves, computes targets. May use up I_CUSTAP_BERRY
         */
        std::vector<battleMove>
        computeSortedBattleMoves( battleUI*                               p_ui,
                                  const std::vector<battleMoveSelection>& p_selectedMoves );

        /*
         * @brief: Deduces the specified number of PP from the first move of the specified
         * pkmn that has the specified p_moveId. If p_amount is 255, the amount defaults
         * to 1 (or 2 if there is a pkmn with A_PRESSURE and no pkmn with
         * A_NEUTRALIZING_GAS on the field.
         */
        constexpr void deducePP( bool p_opponent, u8 p_slot, u16 p_moveId, u8 p_amount = 255 ) {
            if( p_amount == 255 ) {
                p_amount = 1;
                if( !suppressesAbilities( ) && _sides[ !p_opponent ].anyHasAbility( A_PRESSURE ) ) {
                    p_amount = 2;
                }
            }

            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].deducePP( p_slot, p_moveId,
                                                                         p_amount );
        }

        /*
         * @brief: Makes the specified pkmn damage itself due to confusion.
         */
        void confusionSelfDamage( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: checks whether the specified move misses.
         */
        bool moveMisses( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                         bool p_critical );

        /*
         * @brief: Checks whether the specified move will result in a critical hit on the
         * target.
         */
        bool executeCriticalCheck( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes the specified damaging move, dealing damage/heal to the
         * specified target. (Only deals damage/recoil)
         */
        bool executeDamagingMove( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                  bool p_critical, u8 p_damageModifier = 100 );

        /*
         * @brief: Executes all self boosts.
         */
        void executeSelfStatusEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes all status effects of the specified move.
         */
        void executeStatusEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes secondary status effects of the specified move.
         */
        void executeSecondaryStatus( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes all non-status secondary effects.
         */
        void executeSecondaryEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes all effects that happen when the pkmn makes contact using its
         * move.
         */
        void executeContact( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes the given battle move and all effects related to it.
         */
        void executeBattleMove( battleUI* p_ui, battleMove p_move,
                                const std::vector<battleMove>& p_targetsMoves,
                                const std::vector<battleMove>& p_tergetedMoves );

        /*
         * @brief: Sets the pokemon of the specified slot. Only used at battle start
         */
        inline void setSlot( bool p_opponent, u8 p_slot, pokemon* p_pokemon ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setSlot( p_slot, p_pokemon );
        }

        /*
         * @brief: Sends out a new pokemon to a slot, overriding anything that was there
         * beforehand
         */
        inline bool sendPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, pokemon* p_pokemon ) {
            setSlot( p_opponent, p_slot, p_pokemon );
            p_ui->sendOutPkmn( p_opponent, p_slot, p_pokemon );
            checkOnSendOut( p_ui, p_opponent, p_slot );
            return true;
        }

        /*
         * @brief: Handles anything a pkmn may do when sent out.
         */
        void checkOnSendOut( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: Executes effects that happen when a pkmn eats a berry.
         */
        void checkOnEatBerry( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_berry );

        /*
         * @brief: Checks whether the specified pkmn uses its item after an attack.
         */
        void checkItemAfterAttack( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: Checks for effects tha happen when a move damages a pkmn
         */
        void checkOnTakeDamage( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                u16 p_damage, u8 p_effectiveness );

        /*
         * @brief: Returns a list of all types the pkmn currently has. May be empty
         */
        inline std::vector<type> getTypes( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getTypes( p_slot );
        }

        /*
         * @brief: Sets the type of the specified pkmn.
         */
        inline void setType( battleUI* p_ui, bool p_opponent, u8 p_slot, type p_type ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setType( p_ui, p_slot,
                                                                               p_type );
        }

        /*
         * @brief: Sets the extra type of the specified pkmn.
         */
        inline void setExtraType( battleUI* p_ui, bool p_opponent, u8 p_slot, type p_type ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setExtraType( p_ui, p_slot,
                                                                                    p_type );
        }

        /*
         * @brief: Checks whether the pkmn currently has the specified type.
         */
        constexpr bool hasType( bool p_opponent, u8 p_slot, type p_type ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasType( p_slot, p_type );
        }

        constexpr u8 getConsecutiveMoveCount( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getConsecutiveMoveCount(
                p_slot );
        }

        /*
         * @brief: Adds the specified boost to the specified pkmn, returns true iff
         * successful
         */
        boosts addBoosts( bool p_opponent, u8 p_slot, boosts p_boosts,
                          bool p_allowAbilities = true ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addBoosts(
                p_slot, p_boosts, p_allowAbilities && !suppressesAbilities( ) );
        }
        bool resetBoosts( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].resetBoosts( p_slot );
        }
        inline boosts getBoosts( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getBoosts( p_slot );
        }

        inline void revertTransform( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].revertTransform( p_slot );
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( bool p_opponent, u8 p_slot, slot* p_target ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].transformPkmn( p_slot,
                                                                                     p_target );
        }

        /*
         * @brief: Changes the ability of the specified pkmn to p_newAbility.
         * Does nothing and returns false if p_newAbility is 0.
         */
        inline bool changeAbility( bool p_opponent, u8 p_slot, u16 p_newAbility ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].changeAbility( p_slot,
                                                                                     p_newAbility );
        }

        /*
         * @brief: Returns the pkmn in the specified slot or nullptr if the slot is empty.
         */
        constexpr pokemon* getPkmn( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPkmn( p_slot );
        }

        /*
         * @brief: Checks whether any pkmn on the field has the specified ability.
         */
        constexpr bool anyHasAbility( u16 p_ability ) const {
            return _sides[ true ].anyHasAbility( p_ability )
                   || _sides[ false ].anyHasAbility( p_ability );
        }

        /*
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) const {
            return anyHasAbility( A_NEUTRALIZING_GAS );
        }

        /*
         * @brief: Checks whether the specified pkmn has any of the abilities
         * A_MOLD_BREAKER, A_TERAVOLT, or A_TURBOBLAZE
         */
        inline bool breaksAbilities( bool p_opponent, u8 p_pos ) {
            auto p = getPkmn( p_opponent, p_pos );
            if( p == nullptr ) [[unlikely]] { return false; }

            return p->getAbility( ) == A_MOLD_BREAKER || p->getAbility( ) == A_TERAVOLT
                   || p->getAbility( ) == A_TURBOBLAZE;
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) const {
            return anyHasAbility( A_AIR_LOCK ) || anyHasAbility( A_CLOUD_NINE );
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (HP: 0, ATK 1, etc)
         */
        constexpr u16 getStat( bool p_opponent, u8 p_slot, u8 p_stat, bool p_allowAbilities = true,
                               bool p_ignoreNegative = false, bool p_ignorePositive = false ) {
            if( getPkmn( p_opponent, p_slot ) == nullptr ) [[unlikely]] { return 0; }

            bool allowAbilities = p_allowAbilities && !suppressesAbilities( );
            bool allowWeather   = suppressesWeather( );

            u16 base = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStat(
                p_slot, p_stat, allowAbilities, p_ignoreNegative, p_ignorePositive );

            if( allowWeather && allowAbilities && !p_ignorePositive ) {
                if( _weather == SUN || _weather == HEAVY_SUNSHINE ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    auto p2 = getPkmn( p_opponent, !p_slot );

                    // flower gift
                    if( ( p1 != nullptr && p1->getAbility( ) == A_FLOWER_GIFT )
                        || ( p2 != nullptr && p2->getAbility( ) == A_FLOWER_GIFT ) ) {
                        if( p_stat == ATK || p_stat == SDEF ) { base = ( 3 * base ) >> 1; }
                    }

                    // solar power
                    if( p1 != nullptr && p1->getAbility( ) == A_SOLAR_POWER && p_stat == SATK ) {
                        base = ( 3 * base ) >> 1;
                    }

                    // chlorophyll
                    if( p1 != nullptr && p1->getAbility( ) == A_CHLOROPHYLL && p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
                if( _weather == SANDSTORM ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    // sand rush
                    if( p1 != nullptr && p1->getAbility( ) == A_SAND_RUSH && p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
                if( _weather == RAIN || _weather == HEAVY_RAIN ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    // swift swin
                    if( p1 != nullptr && p1->getAbility( ) == A_SWIFT_SWIM && p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
                if( _weather == HAIL ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    // slush rush
                    if( p1 != nullptr && p1->getAbility( ) == A_SLUSH_RUSH && p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
            }

            if( allowAbilities && !p_ignorePositive ) {
                if( _terrain == GRASSYTERRAIN ) {
                    if( getPkmn( p_opponent, p_slot )->getAbility( ) == A_GRASS_PELT ) {
                        if( p_stat == DEF ) { base = ( base * 3 ) / 2; }
                    }
                }
                if( _terrain == ELECTRICTERRAIN ) {
                    if( getPkmn( p_opponent, p_slot )->getAbility( ) == A_SURGE_SURFER ) {
                        if( p_stat == SPEED ) { base <<= 1; }
                    }
                }
            }

            return std::max( u16( 1 ), base );
        }

        /*
         * @brief: Returns how effective the given move is on the specified target.
         * @returns: Damage multiplier multiplied with 100.
         */
        u16 getEffectiveness( battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Checks whether the pokemon can move.
         */
        constexpr bool canSelectMove( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSelectMove( p_slot );
        }
        /*
         * @brief: Checks whether the pokemon can use its i-th move.
         */
        constexpr bool canSelectMove( bool p_opponent, u8 p_slot, u8 p_moveIdx ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSelectMove( p_slot,
                                                                                     p_moveIdx );
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( bool p_opponent, u8 p_slot ) {
            auto tmp = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStoredMove( p_slot );
            tmp.m_user.first = p_opponent ? OPPONENT_SIDE : PLAYER_SIDE;
            return tmp;
        }

        /*
         * @brief: pokemon uses move with the given moveid. Returns false if the move
         * failed (e.g. due to confusion)
         */
        bool useMove( battleUI* p_ui, battleMove p_move );

        /*
         * @brief: Checks whether the pokemon can use an item (from the bag or hold).
         */
        constexpr bool canUseItem( bool p_opponent, u8 p_slot, bool p_allowAbilities = true ) {
            if( getPseudoWeather( ) & MAGICROOM ) { return false; }

            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canUseItem(
                p_slot, p_allowAbilities && !suppressesAbilities( ) );
        }

        /*
         * @brief: Checks whether the pokemon can be switched out.
         */
        inline bool canSwitchOut( bool p_opponent, u8 p_slot ) {
            if( !_sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSwitchOut( p_slot ) )
                [[unlikely]] {
                return false;
            }

            if( !suppressesAbilities( ) ) [[likely]] {
                auto p1 = getPkmn( !p_opponent, 0 );
                auto p2 = getPkmn( !p_opponent, 1 );
                // Shadow tag
                if( getPkmn( p_opponent, p_slot )->getAbility( ) != A_SHADOW_TAG ) [[likely]] {
                    if( ( p1 != nullptr && p1->getAbility( ) == A_SHADOW_TAG )
                        || ( p2 != nullptr && p2->getAbility( ) == A_SHADOW_TAG ) ) [[unlikely]] {
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
                    if( hasType( p_opponent, p_slot, STEEL ) ) [[unlikely]] { return false; }
                }
            }

            return true;
        }

        /*
         * @brief: Moves the held item.
         */
        inline void moveItem( battleUI* p_ui, bool p_sourceOpp, u8 p_sorceSlot, bool p_targetOpp,
                              u8 p_targetSlot ) {
            auto tg = getPkmn( p_targetOpp, p_targetSlot );
            if( tg == nullptr ) [[unlikely]] { return; }
            auto sc = getPkmn( p_sourceOpp, p_sorceSlot );
            if( sc == nullptr ) [[unlikely]] { return; }

            u16 item = sc->getItem( );
            removeItem( p_ui, p_sourceOpp, p_sorceSlot, false );
            char buffer[ 100 ];

            // Check for move item effects

            switch( item ) {
            case I_CHILAN_BERRY2: {
                auto itemname = ITEM::getItemName( item );
                snprintf( buffer, 99, GET_STRING( 281 ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ), itemname.c_str( ),
                          itemname.c_str( ) );
                p_ui->log( buffer );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                boosts bt = boosts( );
                bt.setBoost( DEF, 7 );
                auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt );
                p_ui->logBoosts( sc, p_sourceOpp, p_sorceSlot, bt, res );
                return;
            }
            case I_EGGANT_BERRY: {
                auto itemname = ITEM::getItemName( item );
                snprintf( buffer, 99, GET_STRING( 281 ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ), itemname.c_str( ),
                          itemname.c_str( ) );
                p_ui->log( buffer );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                boosts bt = boosts( );
                bt.setBoost( SDEF, 7 );
                auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt );
                p_ui->logBoosts( sc, p_sourceOpp, p_sorceSlot, bt, res );
                return;
            }
            case I_NUTPEA_BERRY: {
                auto itemname = ITEM::getItemName( item );
                snprintf( buffer, 99, GET_STRING( 281 ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ), itemname.c_str( ),
                          itemname.c_str( ) );
                p_ui->log( buffer );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                boosts bt = boosts( );
                bt.setBoost( ATK, 7 );
                auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt );
                p_ui->logBoosts( sc, p_sourceOpp, p_sorceSlot, bt, res );
                return;
            }
            case I_STRIB_BERRY: {
                auto itemname = ITEM::getItemName( item );
                snprintf( buffer, 99, GET_STRING( 281 ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ), itemname.c_str( ),
                          itemname.c_str( ) );
                p_ui->log( buffer );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                boosts bt = boosts( );
                bt.setBoost( SATK, 7 );
                auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt );
                p_ui->logBoosts( sc, p_sourceOpp, p_sorceSlot, bt, res );
                return;
            }
            case I_YAGO_BERRY: {
                auto itemname = ITEM::getItemName( item );
                snprintf( buffer, 99, GET_STRING( 281 ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ), itemname.c_str( ),
                          itemname.c_str( ) );
                p_ui->log( buffer );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                boosts bt = boosts( );
                bt.setBoost( SPEED, 7 );
                auto res = addBoosts( p_sourceOpp, p_sorceSlot, bt );
                p_ui->logBoosts( sc, p_sourceOpp, p_sorceSlot, bt, res );
                return;
            }
            case I_TOPO_BERRY: {
                auto itemname = ITEM::getItemName( item );
                snprintf( buffer, 99, GET_STRING( 281 ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ), itemname.c_str( ),
                          itemname.c_str( ) );
                p_ui->log( buffer );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                addVolatileStatus( p_ui, p_sourceOpp, p_sorceSlot, PROTECT, 1 );
                snprintf( buffer, 99, GET_STRING( 282 ), itemname.c_str( ),
                          p_ui->getPkmnName( sc, p_sourceOpp ).c_str( ) );
                for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
                return;
            }
                [[likely]] default : break;
            }

            giveItem( p_ui, p_targetOpp, p_targetSlot, item );
        }

        inline void giveItem( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_item ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].giveItem( p_slot, p_item );

            char buffer[ 100 ];
            snprintf( buffer, 99, GET_STRING( 284 ),
                      p_ui->getPkmnName( getPkmn( p_opponent, p_slot ), p_opponent ).c_str( ),
                      ITEM::getItemName( p_item ).c_str( ) );
            p_ui->log( buffer );
            for( u8 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        }

        /*
         * @brief: Removes any held item.
         */
        inline void removeItem( battleUI* p_ui, bool p_opponent, u8 p_slot, bool p_used = true ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeItem( p_slot, p_used );

            // Check for symbiosis
            auto p2 = getPkmn( p_opponent, !p_slot );
            if( p2 == nullptr ) [[likely]] { return; }

            if( !suppressesAbilities( ) && p_used && p2->getAbility( ) == A_SYMBIOSIS
                && p2->getItem( ) ) {
                p_ui->logAbility( p2, p_opponent );
                moveItem( p_ui, p_opponent, !p_slot, p_opponent, p_slot );
            }
        }

        /*
         * @brief: returns the type of the attack of the specified battle move.
         */
        type getMoveType( battleMove p_move );

        /*
         * @brief: returns the base power of the attack of the specified battle move.
         */
        u16 getMovePower( battleMove p_move );

        /*
         * @brief: Returns true iff the specified pokemon fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canMegaEvolve( p_slot );
        }

        /*
         * @brief: Mega evolve the specified pkmn.
         */
        void megaEvolve( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: returns the weight of the specified pkmn.
         */
        inline u16 getWeight( bool p_opponent, u8 p_slot, bool p_allowAbilities = true ) {
            bool ab = p_allowAbilities;
            u16  wg = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getWeight( p_slot, ab );

            if( canUseItem( p_opponent, p_slot, ab )
                && getPkmn( p_opponent, p_slot )->getItem( ) == I_FLOAT_STONE ) {
                wg >>= 1;
            }

            return wg;
        }

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        inline bool isGrounded( bool p_opponent, u8 p_slot, bool p_allowAbilities = true ) {
            bool ab  = p_allowAbilities;
            bool grn = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].isGrounded( p_slot, ab );

            if( _pseudoWeatherTimer[ 4 ] ) [[unlikely]] { // gravity
                grn = true;
            }

            return grn;
        }
    };
} // namespace BATTLE
