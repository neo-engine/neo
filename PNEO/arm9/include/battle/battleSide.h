/*
   Pokémon neo
   ------------------------------

   file        : battleSide.h
   author      : Philip Wellnitz
   description : Header file. Consult the corresponding source file for details.

   Copyright (C) 2012 - 2026
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
#include "battle/battleSlot.h"
#include "battle/move.h"
#include "defines.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "pokemon.h"

namespace BATTLE {
    /*
     * @brief: A side of the field.
     */
    class side {
      public:
        static constexpr u8 MAX_PKMN_PER_SIDE = 2;

      private:
        u8 _sideConditionCounter[ MAX_SIDE_CONDITIONS ]; // Counts turns that side con is active
        u8 _sideConditionAmount[ MAX_SIDE_CONDITIONS ];

        slot _slots[ MAX_PKMN_PER_SIDE ];

      public:
        side( ) {
            std::memset( _sideConditionAmount, 0, sizeof( _sideConditionAmount ) );
            std::memset( _sideConditionCounter, 0, sizeof( _sideConditionCounter ) );

            for( auto i = 0; i < MAX_PKMN_PER_SIDE; ++i ) { _slots[ i ] = slot( ); }
        }

        constexpr u16 getAndIncreaseToxicCount( u8 p_slot ) {
            return _slots[ p_slot ].getAndIncreaseToxicCount( );
        }

        constexpr u16 getTurnsInPlay( u8 p_slot ) {
            return _slots[ p_slot ].getTurnsInPlay( );
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
        inline void setType( battleUI* p_ui, u8 p_slot, type p_type ) {
            return _slots[ p_slot ].setType( p_ui, p_type );
        }

        /*
         * @brief: Sets the extra type of the specified pkmn.
         */
        inline void setExtraType( battleUI* p_ui, u8 p_slot, type p_type ) {
            return _slots[ p_slot ].setExtraType( p_ui, p_type );
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
        bool addSideCondition( battleUI* p_ui, sideCondition p_sideCondition, u8 p_duration = 0,
                               bool p_allowStacking = false );

        /*
         * @brief: The current amount (layer count) of the given single side
         * condition; 0 when it is not set. Spikes/toxic spikes use this as
         * their layer number (maxSideConditionAmount caps them at 3/2).
         */
        constexpr u8 sideConditionAmount( sideCondition p_sideCondition ) const {
            for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
                if( p_sideCondition & ( 1LLU << i ) ) { return _sideConditionAmount[ i ]; }
            }
            return 0;
        }

        bool removeSideCondition( battleUI* p_ui, sideCondition p_sideCondition );

        /*
         * @brief: Tries to add the specified slot condition(s).
         * @param p_duration: The duration of the condition in turns. (0 for the
         * defauls amount)
         */
        inline bool addSlotCondition( battleUI* p_ui, u8 p_pos, slotCondition p_slotCondition,
                                      u8 p_duration = 0, u16 p_amount = 0,
                                      const std::string& p_wisher = std::string( ),
                                      u16                p_src    = 0xFFFF ) {
            return _slots[ p_pos ].addSlotCondition( p_ui, p_slotCondition, p_duration, p_amount,
                                                     p_wisher, p_src );
        }

        inline bool removeSlotCondition( battleUI* p_ui, u8 p_pos, slotCondition p_slotCondition ) {
            return _slots[ p_pos ].removeSlotCondition( p_ui, p_slotCondition );
        }

        constexpr slotCondition getSlotCondition( u8 p_pos ) const {
            return _slots[ p_pos ].getSlotCondition( );
        }

        constexpr u16 getSubstituteHp( u8 p_pos ) const {
            return _slots[ p_pos ].getSubstituteHp( );
        }

        constexpr void setSubstituteHp( u8 p_pos, u16 p_hp ) {
            _slots[ p_pos ].setSubstituteHp( p_hp );
        }

        constexpr u8 getSlotConditionDuration( u8 p_pos, slotCondition p_slotCondition ) const {
            return _slots[ p_pos ].getSlotConditionDuration( p_slotCondition );
        }

        constexpr u16 getSlotConditionAmount( u8 p_pos, slotCondition p_slotCondition ) const {
            return _slots[ p_pos ].getSlotConditionAmount( p_slotCondition );
        }

        const std::string& getSlotConditionWisher( u8 p_pos ) const {
            return _slots[ p_pos ].getSlotConditionWisher( );
        }

        constexpr u16 getSlotConditionSrc( u8 p_pos ) const {
            return _slots[ p_pos ].getSlotConditionSrc( );
        }

        void setPartialTrap( u8 p_pos, const std::string& p_wisher, u16 p_move, u16 p_species,
                             u8 p_side, u8 p_slot, u8 p_timer ) {
            _slots[ p_pos ].setPartialTrap( p_wisher, p_move, p_species, p_side, p_slot, p_timer );
        }
        void clearPartialTrap( u8 p_pos ) {
            _slots[ p_pos ].clearPartialTrap( );
        }
        void decreasePartialTrapTimer( u8 p_pos ) {
            _slots[ p_pos ].decreasePartialTrapTimer( );
        }
        const std::string& getPartialTrapWisher( u8 p_pos ) const {
            return _slots[ p_pos ].getPartialTrapWisher( );
        }
        u16 getPartialTrapMove( u8 p_pos ) const {
            return _slots[ p_pos ].getPartialTrapMove( );
        }
        u16 getPartialTrapSpecies( u8 p_pos ) const {
            return _slots[ p_pos ].getPartialTrapSpecies( );
        }
        u8 getPartialTrapSide( u8 p_pos ) const {
            return _slots[ p_pos ].getPartialTrapSide( );
        }
        u8 getPartialTrapSlot( u8 p_pos ) const {
            return _slots[ p_pos ].getPartialTrapSlot( );
        }
        u8 getPartialTrapTimer( u8 p_pos ) const {
            return _slots[ p_pos ].getPartialTrapTimer( );
        }

        void setDisabledMove( u8 p_pos, u16 p_mv ) {
            _slots[ p_pos ].setDisabledMove( p_mv );
        }
        u16 getDisabledMove( u8 p_pos ) const {
            return _slots[ p_pos ].getDisabledMove( );
        }
        void setEncoreMove( u8 p_pos, u16 p_mv ) {
            _slots[ p_pos ].setEncoreMove( p_mv );
        }
        u16 getEncoreMove( u8 p_pos ) const {
            return _slots[ p_pos ].getEncoreMove( );
        }

        u8 getPerishTimer( u8 p_pos ) const {
            return _slots[ p_pos ].getPerishTimer( );
        }
        void setPerishTimer( u8 p_pos, u8 p_timer ) {
            _slots[ p_pos ].setPerishTimer( p_timer );
        }

        u8 getMoveActions( u8 p_pos ) const {
            return _slots[ p_pos ].getMoveActions( );
        }
        void registerMoveAction( u8 p_pos ) {
            _slots[ p_pos ].registerMoveAction( );
        }

        u16 getStallCounter( u8 p_pos ) const {
            return _slots[ p_pos ].getStallCounter( );
        }
        void setStallCounter( u8 p_pos, u16 p_counter ) {
            _slots[ p_pos ].setStallCounter( p_counter );
        }
        u8 getStallWindow( u8 p_pos ) const {
            return _slots[ p_pos ].getStallWindow( );
        }
        void setStallWindow( u8 p_pos, u8 p_window ) {
            _slots[ p_pos ].setStallWindow( p_window );
        }

        void setLeechSource( u8 p_pos, u8 p_src ) {
            _slots[ p_pos ].setLeechSource( p_src );
        }
        u8 getLeechSource( u8 p_pos ) const {
            return _slots[ p_pos ].getLeechSource( );
        }
        bool seeded( u8 p_pos ) const {
            return _slots[ p_pos ].seeded( );
        }

        bool hasCounter( u8 p_pos ) const {
            return _slots[ p_pos ].hasCounter( );
        }
        bool hasMirrorCoat( u8 p_pos ) const {
            return _slots[ p_pos ].hasMirrorCoat( );
        }
        bool counterRecorded( u8 p_pos ) const {
            return _slots[ p_pos ].counterRecorded( );
        }
        bool mirrorRecorded( u8 p_pos ) const {
            return _slots[ p_pos ].mirrorRecorded( );
        }
        u16 getCounterDamage( u8 p_pos ) const {
            return _slots[ p_pos ].getCounterDamage( );
        }
        u8 getCounterFrom( u8 p_pos ) const {
            return _slots[ p_pos ].getCounterFrom( );
        }
        u16 getMirrorDamage( u8 p_pos ) const {
            return _slots[ p_pos ].getMirrorDamage( );
        }
        u8 getMirrorFrom( u8 p_pos ) const {
            return _slots[ p_pos ].getMirrorFrom( );
        }
        void armCounters( u8 p_pos, bool p_mirror ) {
            _slots[ p_pos ].armCounters( p_mirror );
        }
        void recordCounter( u8 p_pos, bool p_mirror, u16 p_damage, u8 p_from ) {
            _slots[ p_pos ].recordCounter( p_mirror, p_damage, p_from );
        }
        void clearCounters( u8 p_pos ) {
            _slots[ p_pos ].clearCounters( );
        }
        u16 getBideDamage( u8 p_pos ) const {
            return _slots[ p_pos ].getBideDamage( );
        }
        u8 getBideFrom( u8 p_pos ) const {
            return _slots[ p_pos ].getBideFrom( );
        }
        bool bideHit( u8 p_pos ) const {
            return _slots[ p_pos ].bideHit( );
        }
        void addBideDamage( u8 p_pos, u16 p_damage, u8 p_from ) {
            _slots[ p_pos ].addBideDamage( p_damage, p_from );
        }
        void clearBide( u8 p_pos ) {
            _slots[ p_pos ].clearBide( );
        }

        inline void healFully( u8 p_pos ) {
            _slots[ p_pos ].healFully( );
        }

        inline void restoreAllMovePP( u8 p_pos ) {
            _slots[ p_pos ].restoreAllMovePP( );
        }

        bool hasMovePPDeficit( u8 p_pos ) {
            return _slots[ p_pos ].hasMovePPDeficit( );
        }
        bool hasMove( u8 p_pos, u16 p_moveId ) {
            return _slots[ p_pos ].hasMove( p_moveId );
        }

        constexpr sideCondition getSideCondition( ) const {
            sideCondition res = sideCondition( 0 );
            for( u8 i = 0; i < MAX_SIDE_CONDITIONS; ++i ) {
                if( _sideConditionAmount[ i ] ) { res = sideCondition( res | ( 1LLU << i ) ); }
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
        constexpr u8 getVolatileStatusCounter( u8 p_slot, volatileStatus p_volatileStatus ) const {
            return _slots[ p_slot ].getVolatileStatusCounter( p_volatileStatus );
        }

        constexpr u8 getVolatileStatusAmount( u8 p_slot, volatileStatus p_volatileStatus ) const {
            return _slots[ p_slot ].getVolatileStatusAmount( p_volatileStatus );
        }

        constexpr bool removeVolatileStatus( battleUI* p_ui, u8 p_slot,
                                             volatileStatus p_volatileStatus ) {
            return _slots[ p_slot ].removeVolatileStatus( p_ui, p_volatileStatus );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr bool addVolatileStatus( battleUI* p_ui, u8 p_slot,
                                          volatileStatus p_volatileStatus, u8 p_duration ) {
            return _slots[ p_slot ].addVolatileStatus( p_ui, p_volatileStatus, p_duration );
        }

        constexpr bool restackVolatile( u8 p_slot, u8 p_index ) {
            return _slots[ p_slot ].restackVolatile( p_index );
        }

        constexpr bool setStatusCondition( u8 p_slot, u8 p_status, u8 p_duration = 255,
                                           bool p_force = false ) {
            return _slots[ p_slot ].setStatusCondition( p_status, p_duration, p_force );
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
         * @brief: Sets the pkmn of the specified slot. Only used at battle start
         */
        inline void setSlot( u8 p_slot, pokemon* p_pokemon ) {
            _slots[ p_slot ].setPkmn( p_pokemon );
        }

        inline void setSlotDisguise( u8 p_slot, const pokemon* p_pokemon ) {
            _slots[ p_slot ].setDisguise( p_pokemon );
        }

        /*
         * @brief: Returns the pkmn in the specified slot or nullptr if the slot is empty.
         */
        constexpr pokemon* getPkmn( u8 p_slot ) {
            return _slots[ p_slot ].getPkmn( );
        }

        constexpr pokemon* getPkmnOrDisguise( u8 p_slot ) const {
            return _slots[ p_slot ].getPkmnOrDisguise( );
        }

        constexpr u8 getConsecutiveMoveCount( u8 p_slot ) const {
            return _slots[ p_slot ].getConsecutiveMoveCount( );
        }

        /*
         * @brief: Adds the specified boost to the specified pkmn, returns true iff
         * successful
         */
        boosts addBoosts( u8 p_slot, boosts p_boosts, bool p_allowAbilities = true,
                          bool p_allowOverMax = false ) {
            return _slots[ p_slot ].addBoosts( p_boosts, p_allowAbilities, p_allowOverMax );
        }
        bool resetBoosts( u8 p_slot ) {
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
            return _slots[ 0 ].hasAbility( p_ability ) || _slots[ 1 ].hasAbility( p_ability );
        }

        bool abilitiesNullified( u8 p_pos ) const {
            return _slots[ p_pos ].abilitiesNullified( );
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

        inline void markMoveActed( u8 p_slot ) {
            _slots[ p_slot ].markMoveActed( );
        }

        inline bool takeMoveActed( u8 p_slot ) {
            return _slots[ p_slot ].takeMoveActed( );
        }

        inline battleMove getLastUsedMove( u8 p_slot ) const {
            return _slots[ p_slot ].getLastUsedMove( );
        }

        /*
         * @brief: Faints the specified pkmn. Also deals the necessary damage.
         */
        inline void faintPokemon( u8 p_slot ) {
            _slots[ p_slot ].faintPokemon( );
        }

        /*
         * @brief: Recalls the pkmn at the specified position.
         */
        inline void recallPokemon( u8 p_slot, bool p_keepChanges = false ) {
            _slots[ p_slot ].recallPokemon( p_keepChanges );
        }

        /*
         * @brief: Deals the specified amount of damage to the specified pkmn.
         */
        inline void damagePokemon( u8 p_slot, u16 p_damage ) {
            _slots[ p_slot ].damagePokemon( p_damage );
        }

        /*
         * @brief: Heals the specified amount of damage from the specified pkmn.
         */
        inline void healPokemon( u8 p_slot, u16 p_damage ) {
            _slots[ p_slot ].healPokemon( p_damage );
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (HP: 0, ATK 1, etc)
         */
        u16 getStat( u8 p_slot, u8 p_stat, bool p_allowAbilities = true,
                     bool p_ignoreNegative = false, bool p_ignorePositive = false,
                     bool p_wonderRoom = false );

        /*
         * @brief: Checks whether the pkmn can move.
         */
        constexpr bool canSelectMove( u8 p_slot ) const {
            return _slots[ p_slot ].canSelectMove( );
        }
        /*
         * @brief: Checks whether the pkmn can use its i-th move.
         */
        constexpr bool canSelectMove( u8 p_slot, u8 p_moveIdx ) {

            if( getPkmn( !p_slot ) == nullptr
                && FS::getMoveData( getPkmn( p_slot )->getMove( p_moveIdx ) ).m_target == TG_ALLY )
                [[unlikely]] {
                return false;
            }

            return _slots[ p_slot ].canSelectMove( p_moveIdx );
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( u8 p_slot ) {
            auto tmp          = _slots[ p_slot ].getStoredMove( );
            tmp.m_user.second = p_slot;
            return tmp;
        }

        /*
         * @brief: Checks whether the pkmn can use an item (from the bag).
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
         * @brief: Returns true iff the specified pkmn fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( u8 p_slot ) {
            return _slots[ p_slot ].canMegaEvolve( );
        }

        /*
         * @brief: returns the weight of the specified pkmn.
         */
        inline u16 getWeight( u8 p_slot, bool p_allowAbilities = true ) {
            return _slots[ p_slot ].getWeight( p_allowAbilities );
        }

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        inline bool isGrounded( u8 p_slot, bool p_allowAbilities = true ) {
            return _slots[ p_slot ].isGrounded( p_allowAbilities );
        }

        /*
         * @brief: Checks whether the pkmn can be switched out.
         */
        inline bool canSwitchOut( u8 p_slot ) {
            return _slots[ p_slot ].canSwitchOut( );
        }
    };
} // namespace BATTLE
