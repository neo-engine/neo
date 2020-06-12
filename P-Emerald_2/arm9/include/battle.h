/*
    Pokémon neo
    ------------------------------

    file        : battle.h
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

#include "battleDefines.h"
#include "battleTrainer.h"
#include "battleUI.h"
#include "defines.h"
#include "pokemon.h"
#include "type.h"
#include "abilityNames.h"
#include "pokemonNames.h"

namespace BATTLE {
    typedef std::pair<u8, u8> fieldPosition; // (side, slot)

    enum battleMoveType {
        ATTACK,
        MEGA_ATTACK, // Mega evolve and attack
        SWITCH,
        SWITCH_PURSUIT, // Pursuit used on a switching target
        USE_ITEM,
        NO_OP,   // No operation (e.g. when trying to run in a double battle)
        CAPTURE, // (try to) capture pokemon.
        RUN,
        CANCEL // Cancel / go back to previous move selection
    };

    struct battleMoveSelection {
        battleMoveType m_type;
        u16            m_param; // move id for attack/ m attack; target pkmn
                                // for swtich; tg item
        std::vector<fieldPosition> m_target;
        fieldPosition              m_user;
    };

    const inline battleMoveSelection NO_OP_SELECTION = {NO_OP, 0, {}, {255, 255}};


    struct battleMove {
        battleMoveType             m_type;
        u16                        m_param;
        std::vector<fieldPosition> m_target;
        fieldPosition              m_user;
        s8                         m_priority;
        s16                        m_userSpeed;
        u8                         m_pertubation; // random number to break speed ties

        bool operator<( const battleMove& p_other ) {
            if( this->m_priority < p_other.m_priority ) return true;
            if( this->m_userSpeed > p_other.m_userSpeed ) return true;
            if( this->m_pertubation < p_other.m_pertubation ) return true;
            return false;
        }
    };

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

        bool          _hibernating; // pkmn is hibernating after e.g. hyper beam
        bool          _charging; // pkmn is preparing _lockedMove
        battleMoveSelection _lockedMove; // move that a pkmn is forced to execute (no op if hib)
        u8            _lockedMoveTurns; // remaining turns the pkmn is locked into _lockedMove

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
            _hibernating = false;
            _charging = false;
            _lockedMove = NO_OP_SELECTION;
            _lockedMoveTurns = 0;
            _status = status( 0 );
            _slotCondition = slotCondition( 0 );
            _isTransformed = false;
            _boosts = boosts( );
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            std::memset( _volatileStatusCounter, 0, sizeof( _volatileStatusCounter ) );
            std::memset( _volatileStatusAmount, 0, sizeof( _volatileStatusAmount ) );
        }

        /*
         * @brief: Recalls a non-FAINTED pokemon.
         */
        pokemon* recallPokemon( battleUI* p_ui, bool p_keepVolatileStatus = false );
        /*
         * @brief: Sends out a new pokemon to an EMPTY slot.
         */
        bool sendPokemon( battleUI* p_ui, pokemon* p_pokemon );

        bool damagePokemon( battleUI* p_ui, u16 p_damage );
        bool healPokemon( battleUI* p_ui, u16 p_heal );
        /*
         * @brief: Faints the pokemon. Deals necessary damage first.
         */
        bool faintPokemon( battleUI* p_ui );

        boosts addBoosts( boosts p_boosts, bool p_allowAbilities = true );
        bool   resetBoosts( battleUI* p_ui );
        inline boosts getBoosts( ) const {
            return _boosts;
        }

        bool           addVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus );
        constexpr volatileStatus getVolatileStatus( ) const {
            volatileStatus res = volatileStatus( 0 );
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( _volatileStatusAmount[ i ] ) {
                    res = volatileStatus( res | ( 1 << i ) );
                }
            }
            return res;
        }

        bool          addSlotCondition( battleUI* p_ui, slotCondition p_slotCondition );
        constexpr slotCondition getSlotCondition( ) const {
            return _slotCondition;
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (current HP: 0, ATK 1, etc)
         * @returns: The numerical value of the stat or a value in [0, 15] for ACCURACY
         * and EVASION, where 7 represents no change.
         */
        constexpr u16 getStat( u8 p_stat, bool p_allowAbilities = true  ) {
            if( _pokemon == nullptr ) { return 0; }

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
            if( p_allowAbilities ) {
                switch( getPkmn( )->getAbility( ) ) {
                    case A_SLOW_START: {
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

                    default:
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
                case I_EVIOLITE:
                    if( ( p_stat == SDEF || p_stat == DEF )
                            && !_pokemon->isFullyEvolved( ) ) { base = 3 * base / 2; }
                    break;

                default:
                    break;
            }

            return std::max( base, u16( 1 ) );
        }

        /*
         * @brief: Checks whether the pokemon can move.
         */
        constexpr bool canSelectMove( ) const {
            if( _pokemon == nullptr ) { return false; }
            return !_hibernating && !_charging;
        }

        /*
         * @brief: Checks whether the pokemon can use its i-th move.
         */
        constexpr bool canSelectMove( u8 p_moveIdx ) const {
            // TODO
            return true;
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( ) const {
            if( _pokemon == nullptr ) { return NO_OP_SELECTION; }
            return _lockedMove;
        }

        /*
         * @brief: pokemon uses move with the given moveid. Returns false if the move
         * failed (e.g. due to confusion)
         */
        bool useMove( battleUI* p_ui, u16 p_moveId );

        /*
         * @brief: pokemon is hit by move with the given id
         */
        void hitByMove( battleUI* p_ui, u16 p_moveId );

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
        constexpr bool canSwitchOut( ) const {
            // TODO
            return true;
        }

        /*
         * @brief: Computes a battle move from the given user's and its targets'
         * selections.
         */
        battleMove computeBattleMove( battleMoveSelection               p_usersSelection,
                                      std::vector<battleMoveSelection>& p_targetsSelecotions );

        /*
         * @brief: Computes the base damage dealt to each target
         * @param p_targetsMoves: Moves of the targets of the user's move.
         * @param p_targetedMoves: Moves that target the user.
         */
        std::vector<u16> computeDamageDealt( battleMove               p_usersMove,
                                             std::vector<battleMove>& p_targetsMoves,
                                             std::vector<battleMove>& p_targetedMoves );

        /*
         * @brief: Computes the actual damage taken by the specified move.
         */
        u16 computeDamageTaken( battleMove p_move, u16 p_baseDamage );

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
            if( _pokemon == nullptr ) {
                return false;
            }
            if( p_target != nullptr ) {
                _isTransformed = true;
                _transformedPkmn = *p_target;

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

            } else {
                _isTransformed = false;
                std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            }
            return true;
        }

        /*
         * @brief: Changes the ability of the specified pkmn to p_newAbility.
         * Does nothing and returns false if p_newAbility is 0.
         */
        inline bool changeAbility( u16 p_newAbility ) {
            if( !_pokemon ) { return false; }
            return _pokemon->setBattleTimeAbility( p_newAbility );
        }

        /*
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) {
            if( _pokemon == nullptr ) { return false; }
            return getPkmn( )->getAbility( ) == A_NEUTRALIZING_GAS;
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) {
            if( _pokemon == nullptr ) { return false; }
            switch( getPkmn( )->getAbility( ) ) {
                case A_AIR_LOCK:
                case A_CLOUD_NINE:
                    return true;
                default:
                    return false;
            }
        }

    };

    /*
     * @brief: A side of the field.
     */
    class side {
      private:
        u8 _sideConditionCounter[ MAX_SIDE_CONDITIONS ]; // Counts turns that side con is active
        u8 _sideConditionAmount[ MAX_SIDE_CONDITIONS ];

        slot _slots[ 2 ];

      public:
        /*
         * Ages the side by one turn, processes all changes
         */
        void age( battleUI* p_ui );

        /*
         * @brief: Tries to add the specified side condition(s).
         * @param p_duration: The duration of the condition in turns. (0 for the
         * defauls amount)
         */
        bool addSideCondition( battleUI* p_ui, sideCondition p_sideCondition, u8 p_duration = 0 );

        bool removeSideCondition( battleUI* p_ui, sideCondition p_sideCondition );

        battleMove computeBattleMove( u8 p_slot, battleMoveSelection& p_usersSelection,
                                      std::vector<battleMoveSelection>& p_targetsSelecotions );

        /*
         * @brief: Computes the base damage dealt to each target
         * @param p_targetsMoves: Moves of the targets of the user's move.
         * @param p_targetedMoves: Moves that target the user.
         */
        std::vector<u16> computeDamageDealt( u8 p_slot, battleMove p_usersMove,
                                             std::vector<battleMove>& p_targetsMoves,
                                             std::vector<battleMove>& p_targetedMoves );

        /*
         * @brief: Computes the actual damage taken by the specified move.
         */
        u16 computeDamageTaken( u8 p_slot, battleMove p_move, u16 p_baseDamage );

        /*
         * @brief: Computes the recoil the pokemon in the slot would do if hit by the
         * given amount of damage.
         */
        u16 computeRecoil( u8 p_slot, u16 p_damage );

        /*
         * @brief: Checks whether the pokemon in this slot absorbs the given move
         * (e.g. due to rage powder / storm drain / etc.)
         */
        bool absorbesMove( u8 p_slot, battleMove p_move, u16 p_baseDamage );

        /*
         * @brief: Sends out a new pokemon to an EMPTY slot.
         */
        bool sendPokemon( battleUI* p_ui, u8 p_slot, pokemon* p_pokemon );

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

        /*
         * @brief: Adds the specified boost to the specified pkmn, returns true iff
         * successful
         */
        boosts addBoosts( u8 p_slot, boosts p_boosts, bool p_allowAbilities = true ) {
            return _slots[ p_slot ].addBoosts( p_boosts, p_allowAbilities );
        }
        bool   resetBoosts( battleUI* p_ui, u8 p_slot ) {
            return _slots[ p_slot ].resetBoosts( p_ui );
        }
        inline boosts getBoosts( u8 p_slot ) const { return _slots[ p_slot ].getBoosts( ); }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( u8 p_slot, pokemon* p_target ) {
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
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) {
            return _slots[ 0 ].suppressesAbilities( ) || _slots[ 1 ].suppressesAbilities( );
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) {
            return _slots[ 0 ].suppressesWeather( ) || _slots[ 1 ].suppressesWeather( );
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (HP: 0, ATK 1, etc)
         */
        constexpr u16 getStat( u8 p_slot, u8 p_stat, bool p_allowAbilities = true ) {
            if( getPkmn( p_slot ) == nullptr ) { return 0; }
            u16 base = _slots[ p_slot ].getStat( p_stat, p_allowAbilities );

            if( p_stat == SPEED && _sideConditionAmount[ 8 ] ) { // Tailwind
                base *= 2;
            }

            // plus / minus
            if( p_allowAbilities ) {
                if( p_stat == SATK && ( getPkmn( p_slot )->getAbility( ) == A_PLUS
                        || getPkmn( p_slot )->getAbility( ) == A_MINUS ) ) {
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
        constexpr bool canSelectMove( u8 p_slot, u8 p_moveIdx ) const {
            return _slots[ p_slot ].canSelectMove( p_moveIdx );
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( u8 p_slot ) const {
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
         * @brief: pokemon is hit by move with the given id
         */
        void hitByMove( battleUI* p_ui, u8 p_slot, u16 p_moveId ) {
            _slots[ p_slot ].hitByMove( p_ui, p_moveId );
        }

        /*
         * @brief: Checks whether the pokemon can use an item (from the bag).
         */
        constexpr bool canUseItem( u8 p_slot ) const {
            return _slots[ p_slot ].canUseItem( );
        }

        /*
         * @brief: Checks whether the pokemon can be switched out.
         */
        constexpr bool canSwitchOut( u8 p_slot ) const {
            return _slots[ p_slot ].canSwitchOut( );
        }
    };

    /*
     * @brief: The overall field where the battle takes place.
     */
    class field {
      public:
        static constexpr u8 PLAYER_SIDE   = 0;
        static constexpr u8 OPPONENT_SIDE = 1;

      private:
        weather _weather;
        u8      _weatherTimer; // Number of turns the weather is still active

        pseudoWeather _pseudoWeather;
        u8            _pseudoWeatherTimer;

        terrain _terrain;
        u8      _terrainTimer;

        side _sides[ 2 ];

      public:
        field( weather       p_initialWeather       = NO_WEATHER,
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

        /*
         * @brief: Tries to set a new weather; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool    setWeather( battleUI* p_ui, weather p_newWeather, bool p_extended = false );
        bool    removeWeather( battleUI* p_ui );
        weather getWeather( );

        /*
         * @brief: Tries to set a new pseudo weather; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool          setPseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather,
                                        bool p_extended = false );
        bool          removePseudoWeather( battleUI* p_ui );
        pseudoWeather getPseudoWeather( );

        /*
         * @brief: Tries to set a new terrain; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool    setTerrain( battleUI* p_ui, terrain p_newTerrain, bool p_extended = false );
        bool    removeTerrain( battleUI* p_ui );
        terrain getTerrain( );

        bool addSideCondition( battleUI* p_ui, u8 p_side, sideCondition p_sideCondition,
                               u8 p_duration = 0 );
        bool removeSideCondition( battleUI* p_ui, u8 p_side, sideCondition p_sideCondition );

        std::vector<battleMove>
        computeSortedBattleMoves( const std::vector<battleMoveSelection>& p_selectedMoves );

        void executeBattleMove( battleUI* p_ui, battleMove p_move,
                                std::vector<battleMove> p_targetsMoves,
                                std::vector<battleMove> p_tergetedMoves );

        /*
         * @brief: Sets the pokemon of the specified slot. Only used at battle start
         */
        inline void setSlot( bool p_opponent, u8 p_slot, pokemon* p_pokemon ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setSlot( p_slot, p_pokemon );
        }

        /*
         * @brief: Sends out a new pokemon to an EMPTY slot.
         */
        bool sendPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, pokemon* p_pokemon );

        /*
         * @brief: Handles anything a pkmn may do when sent out.
         */
        void checkOnSendOut( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: Adds the specified boost to the specified pkmn, returns true iff
         * successful
         */
        boosts   addBoosts( bool p_opponent, u8 p_slot, boosts p_boosts,
                          bool p_allowAbilities = true ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addBoosts(
                    p_slot, p_boosts, p_allowAbilities && suppressesAbilities( ) );
        }
        bool   resetBoosts( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].resetBoosts( p_ui, p_slot );
        }
        inline boosts getBoosts( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getBoosts( p_slot );
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( bool p_opponent, u8 p_slot, pokemon* p_target ) {
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
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) {
            return _sides[ true ].suppressesAbilities( ) || _sides[ false ].suppressesAbilities( );
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) {
            return _sides[ true ].suppressesWeather( ) || _sides[ false ].suppressesWeather( );
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (HP: 0, ATK 1, etc)
         */
        constexpr u16 getStat( bool p_opponent, u8 p_slot, u8 p_stat,
                               bool p_allowAbilities = true ) {
            if( getPkmn( p_opponent, p_slot ) == nullptr ) { return 0; }

            bool allowAbilities = p_allowAbilities && !suppressesAbilities( );
            bool allowWeather = suppressesWeather( );

            u16 base = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStat(
                    p_slot, p_stat, allowAbilities );

            if( allowWeather && allowAbilities ) {
                if( _weather == SUN || _weather == HEAVY_SUNSHINE ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    auto p2 = getPkmn( p_opponent, !p_slot );

                    // flower gift
                    if( ( p1 != nullptr && p1->getAbility( ) == A_FLOWER_GIFT )
                            || ( p2 != nullptr && p2->getAbility( ) == A_FLOWER_GIFT ) ) {
                        if( p_stat == ATK || p_stat == SDEF ) {
                            base = ( 3 * base ) >> 1;
                        }
                    }

                    // solar power
                    if( p1 != nullptr && p1->getAbility( ) == A_SOLAR_POWER &&
                            p_stat == SATK ) {
                        base = ( 3 * base ) >> 1;
                    }

                    // chlorophyll
                    if( p1 != nullptr && p1->getAbility( ) == A_CHLOROPHYLL &&
                            p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
                if( _weather == SANDSTORM ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    // sand rush
                    if( p1 != nullptr && p1->getAbility( ) == A_SAND_RUSH &&
                            p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
                if( _weather == RAIN || _weather == HEAVY_RAIN ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    // swift swin
                    if( p1 != nullptr && p1->getAbility( ) == A_SWIFT_SWIM &&
                            p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
                if( _weather == HAIL ) {
                    auto p1 = getPkmn( p_opponent, p_slot );
                    // slush rush
                    if( p1 != nullptr && p1->getAbility( ) == A_SLUSH_RUSH &&
                            p_stat == SPEED ) {
                        base <<= 1;
                    }
                }
            }

            if( allowAbilities ) {
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
         * @brief: Checks whether the pokemon can move.
         */
        constexpr bool canSelectMove( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSelectMove( p_slot );
        }
        /*
         * @brief: Checks whether the pokemon can use its i-th move.
         */
        constexpr bool canSelectMove( bool p_opponent, u8 p_slot, u8 p_moveIdx ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].
                canSelectMove( p_slot, p_moveIdx );
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( bool p_opponent, u8 p_slot ) const {
            auto tmp = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStoredMove( p_slot );
            tmp.m_user.first = p_opponent ? OPPONENT_SIDE : PLAYER_SIDE;
            return tmp;
        }

        /*
         * @brief: pokemon uses move with the given moveid. Returns false if the move
         * failed (e.g. due to confusion)
         */
        inline bool useMove( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_moveId ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].
                useMove( p_ui, p_slot, p_moveId );
        }

        /*
         * @brief: pokemon is hit by move with the given id
         */
        void hitByMove( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_moveId ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hitByMove( p_ui, p_slot, p_moveId );
        }

        /*
         * @brief: Checks whether the pokemon can use an item (from the bag).
         */
        constexpr bool canUseItem( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canUseItem( p_slot );
        }

        /*
         * @brief: Checks whether the pokemon can be switched out.
         */
        constexpr bool canSwitchOut( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSwitchOut( p_slot );
        }
    };

    class battle {
        public:
        enum battleEndReason {
            BATTLE_ROUND_LIMIT  = 0,
            BATTLE_OPPONENT_WON = -1,
            BATTLE_PLAYER_WON   = 1,
            BATTLE_NONE         = 2,
            BATTLE_RUN          = 3,
            BATTLE_CAPTURE      = 4
        };

      private:
        u16   _round, _maxRounds, _AILevel;
        field _field;

        battleUI _battleUI;

        battleTrainer _opponent;
        pokemon       _opponentTeam[ 6 ];
        u8            _opponentTeamSize;
        u8            _opponentPkmnPerm[ 6 ];

        pokemon* _playerTeam;
        u8       _playerTeamSize;
        u8       _playerPkmnPerm[ 6 ];

        u8 _curPkmnPosition[ 6 ][ 2 ]; // me; opp; maps the Pkmn's positions in the
                                       //  teams to their real in-battle positions

        battlePolicy _policy;
        bool        _isWildBattle;

        /*
         * @brief: Initializes the battle.
         */
        void initBattle( );

        /*
         * @brief: Makes the player select a move for the pokemon in slot p_slot.
         * Returns a NO_OP move if the corresponding pokemon cannot move.
         */
        battleMoveSelection getMoveSelection( u8 p_slot, bool p_allowMegaEvolution );

        /*
         * @brief: Computes an AI move for the opposing pokemon in slot p_slot.
         */
        battleMoveSelection getAIMove( u8 p_slot );

        /*
         * @brief: Checks whether the battle hit an end
         */
        bool endConditionHit( battleEndReason& p_out );

        /*
         * @brief: Ends the battle due to the given reason.
         */
        void endBattle( battleEndReason p_battleEndReason );

        /*
         * @brief: Player tries to run.
         */
        bool playerRuns( );

        /*
         * @brief: Player tries to capture to wild pokemon with the specified pokemon.
         * @param p_pokeball: Item id of the pokeball used.
         */
        bool playerCaptures( u16 p_pokeball );

        /*
         * @brief: Mega evolves the pokemon at the specified position.
         */
        void megaEvolve( fieldPosition p_position );

        /*
         * @brief: Checks for slots with the given type and refills them if necessary.
         */
        void checkAndRefillBattleSpots( slot::status p_checkType );

        /*
         * @brief: Switches the pokemon at the field position with the p_newIndex-th pokemon
         * of the corresponding trainer.
         */
        void switchPokemon( fieldPosition p_toSwitch, u16 p_newIndex );

        /*
         * @brief: Uses the specified item on the pokemon at the field position.
         */
        void useItem( fieldPosition p_target, u16 p_item );


        /*
         * @brief: Moves pkmn that cannot battle to the end of the list.
         */
        void sortPkmn( bool p_opponent );

        /*
         * @brief: Restores the order the pkmn were at the start of the battle
         */
        void restoreInitialOrder( bool p_opponent );

        /*
         * @brief: Resets any battle-time only transformations of pkmn.
         */
        void resetBattleTransformations( bool p_opponent );
      public:
        /*
         * @brief: Creates a new trainer battle.
         */
        battle( pokemon* p_playerTeam, u8 p_playerTeamSize,
                u16 p_opponentId, u8 p_platform = 10, u8 p_platform2 = 10, u8 p_background = 0,
                battlePolicy = DEFAULT_TRAINER_POLICY );

        /*
         * @brief: Creates a new wild pkmn battle.
         */
        battle( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon p_opponent,
                u8 p_platform = 10, u8 p_platform2 = 10, u8 p_background = 0,
                battlePolicy = DEFAULT_WILD_POLICY );

        /*
         * @brief: Starts the battle.
         * @returns: positive values if the player won the battle; negative value if the
         * player lost.
         */
        battleEndReason start( );

    };
} // namespace BATTLE
