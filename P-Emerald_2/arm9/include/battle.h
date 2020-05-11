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
#include <map>
#include <set>
#include <string>
#include <vector>

#include <nds.h>

#include "defines.h"
#include "battleDefines.h"
#include "battleTrainer.h"
#include "battleUI.h"
#include "pokemon.h"
#include "type.h"

namespace BATTLE {
    typedef std::pair<u8, u8> fieldPosition; // (side, slot)

    enum battleMoveType {
        ATTACK,
        MEGA_ATTACK, // Mega evolve and attack
        SWITCH,
        SWITCH_PURSUIT, // Pursuit used on a switching target
        USE_ITEM,
        NO_OP, // No operation (e.g. when trying to run in a double battle)
        CAPTURE, // (try to) capture pokemon.
        RUN,
        CANCEL // Cancel / go back to previous move selection
    };

    struct battleMoveSelection {
        battleMoveType             m_type;
        u16                        m_param; // move id for attack/ m attack; target pkmn
                                            // for swtich; tg item
        std::vector<fieldPosition> m_target;
        fieldPosition              m_user;
    };

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
            status         _status;
            slotCondition  _slotCondition;
            pokemon*       _pokemon;
            u8             _volatileStatusCounter[ MAX_VOLATILE_STATUS ];
            u8             _volatileStatusAmount[ MAX_VOLATILE_STATUS ]; // Multiple stockpiles
            boosts         _boosts;
        public:
            /*
             * @brief: Constructs and initializes a new slot, optionally with a default
             * (wild) pokemon.
             */
            slot( pokemon* p_initialPokemon = nullptr );

            /*
             * Ages the slot by one turn, processes all changes
             */
            void age( battleUI p_ui );

            /*
             * @brief: Recalls a non-FAINTED pokemon.
             */
            pokemon* recallPokemon( battleUI p_ui, bool p_keepVolatileStatus = false );
            /*
             * @brief: Sends out a new pokemon to an EMPTY slot.
             */
            bool sendPokemon( battleUI p_ui, pokemon* p_pokemon );

            bool damagePokemon( battleUI p_ui, u16 p_damage );
            bool healPokemon( battleUI p_ui, u16 p_heal );
            /*
             * @brief: Faints the pokemon. Deals necessary damage first.
             */
            bool faintPokemon( battleUI p_ui );

            bool addBoosts( battleUI p_ui, boosts p_boosts );
            bool resetBoosts( battleUI p_ui );
            boosts getBoosts( );

            bool addVolatileStatus( battleUI p_ui, volatileStatus p_volatileStatus );
            volatileStatus getVolatileStatus( );

            bool addSlotCondition( battleUI p_ui, slotCondition p_slotCondition );
            slotCondition getSlotCondition( );

            /*
             * @brief: Gets the current value of the specified stat (with all modifiers
             * applied). (HP: 0, ATK 1, etc)
             */
            u16 getStat( u8 p_stat );

            /*
             * @brief: Checks whether the pokemon can move.
             */
            bool canSelectMove( );
            /*
             * @brief: Checks whether the pokemon can use its i-th move.
             */
            bool canSelectMove( u8 p_moveIdx );

            /*
             * @brief: pokemon uses move with the given moveid. Returns false if the move
             * failed (e.g. due to confusion)
             */
            bool useMove( battleUI p_ui, u16 p_moveId );

            /*
             * @brief: pokemon is hit by move with the given id
             */
            void hitByMove( battleUI p_ui, u16 p_moveId );

            /*
             * @brief: Checks whether the pokemon can use an item (from the bag).
             */
            bool canUseItem( );

            /*
             * @brief: Checks whether the pokemon can be switched out.
             */
            bool canSwitchOut( );

            /*
             * @brief: Computes a battle move from the given user's and its targets'
             * selections.
             */
            battleMove computeBattleMove( battleMoveSelection p_usersSelection,
                                          std::vector<battleMoveSelection>& p_targetsSelecotions );

            /*
             * @brief: Computes the base damage dealt to each target
             * @param p_targetsMoves: Moves of the targets of the user's move.
             * @param p_targetedMoves: Moves that target the user.
             */
            std::vector<u16> computeDamageDealt( battleMove p_usersMove,
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
    };

    /*
     * @brief: A side of the field.
     */
    class side {
        private:
            u8   _sideConditionCounter[ MAX_SIDE_CONDITIONS ]; // Counts turns that side con is active
            u8   _sideConditionAmount[ MAX_SIDE_CONDITIONS ];

            slot _slots[ 2 ];
        public:
            side( ) { }

            /*
             * Ages the side by one turn, processes all changes
             */
            void age( battleUI p_ui );

            /*
             * @brief: Tries to add the specified side condition(s).
             * @param p_duration: The duration of the condition in turns. (0 for the
             * defauls amount)
             */
            bool addSideCondition( battleUI p_ui, sideCondition p_sideCondition,
                                   u8 p_duration = 0 );

            bool removeSideCondition( battleUI p_ui, sideCondition p_sideCondition );

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
    };


    /*
     * @brief: The overall field where the battle takes place.
     */
    class field {
        public:
            static const u8 PLAYER_SIDE = 0;
            static const u8 OPPONENT_SIDE = 1;
        private:
            weather         _weather;
            u8              _weatherTimer; // Number of turns the weather is still active

            pseudoWeather   _pseudoWeather;
            u8              _pseudoWeatherTimer;

            terrain         _terrain;
            u8              _terrainTimer;

            side            _sides[ 2 ];
        public:
            field( weather p_initialWeather = NO_WEATHER,
                   pseudoWeather p_initialPseudoWeather = NO_PSEUDO_WEATHER,
                   terrain p_initialTerrain = NO_TERRAIN );

            /*
             * Initializes the field.
             */
            void init( battleUI p_ui );
            /*
             * Ages the field by one turn, processes all weather changes
             */
            void age( battleUI p_ui );

            /*
             * @brief: Tries to set a new weather; returns false iff that fails.
             */
            bool    setWeather( battleUI p_ui, weather p_newWeather );
            bool    removeWeather( battleUI p_ui );
            weather getWeather( );

            /*
             * @brief: Tries to set a new pseudo weather; returns false iff that fails.
             */
            bool          setPseudoWeather( battleUI p_ui, pseudoWeather p_newPseudoWeather );
            bool          removePseudoWeather( battleUI p_ui );
            pseudoWeather getPseudoWeather( );

            /*
             * @brief: Tries to set a new terrain; returns false iff that fails.
             */
            bool    setTerrain( battleUI p_ui, terrain p_newTerrainr );
            bool    removeTerrain( battleUI p_ui );
            terrain getTerrain( );

            bool addSideCondition( battleUI p_ui, u8 p_side, sideCondition p_sideCondition,
                                   u8 p_duration = 0 );
            bool removeSideCondition( battleUI p_ui, u8 p_side, sideCondition p_sideCondition );

            std::vector<battleMove>
            computeSortedBattleMoves( const std::vector<battleMoveSelection>& p_selectedMoves );

            void executeBattleMove( battleUI p_ui, battleMove p_move,
                                    std::vector<battleMove> p_targetsMoves,
                                    std::vector<battleMove> p_tergetedMoves );
    };


    class battle {
        private:
            u16     _round, _maxRounds, _AILevel;
            field   _field;

            battleUI _battleUI;

            battleTrainer   _opponent;
            pokemon         _opponentTeam[ 6 ];
            u8              _opponentTeamSize;

            pokemon* _playerTeam;
            u8       _playerTeamSize;

            u8 _curPkmnPosition[ 6 ][ 2 ]; // me; opp; maps the Pkmn's positions in the
                                          //  teams to their real in-battle positions

            bool     _allowMegaEvolution;
        public:
            enum battleMode { SINGLE = 0, DOUBLE = 1 };

            enum battleEndReason {
                BATTLE_ROUND_LIMIT  = 0,
                BATTLE_OPPONENT_WON = -1,
                BATTLE_PLAYER_WON   = 1,
                BATTLE_NONE         = 2,
                BATTLE_RUN          = 3,
                BATTLE_CAPTURE      = 4
            };

            bool m_distributeEXP;
            bool m_isWildBattle;

            battleMode m_battleMode;

            battle( pokemon* p_playerTeam, u16 p_opponentId, u16 p_maxRounds,
                    weather p_weather, u8 p_platform, u8 p_background, u16 p_AILevel = 5,
                    battleMode p_battlemode = SINGLE, u8 p_platform2 = -1,
                    bool p_allowMegaEvolution = true );
            battle( pokemon* p_playerTeam, pokemon p_opponent,
                    weather p_weather, u8 p_platform, u8 p_platform2, u8 p_background,
                    bool p_allowMegaEvolution = true );


            s8 start( ); // Runs battle; returns -1 if opponent wins, 0 if tie, 1 otherwise

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
    };
} // namespace BATTLE
