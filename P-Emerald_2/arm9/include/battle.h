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

#include "abilityNames.h"
#include "battleDefines.h"
#include "battleField.h"
#include "battleTrainer.h"
#include "battleUI.h"
#include "defines.h"
#include "move.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "type.h"

namespace BATTLE {
    class battle {
      public:
        enum battleEndReason {
            BATTLE_ROUND_LIMIT  = 0,
            BATTLE_OPPONENT_WON = -1,
            BATTLE_PLAYER_WON   = 1,
            BATTLE_NONE         = 2,
            BATTLE_RUN          = 3,
            BATTLE_CAPTURE      = 4,
            BATTLE_OPPONENT_RAN = 5,
        };

        static constexpr u8 BEFORE_BATTLE = 0;
        static constexpr u8 AFTER_BATTLE = 1;

      private:
        u16   _round, _maxRounds, _AILevel;
        field _field;

        battleUI _battleUI;

        battleTrainer _opponent;
        pokemon       _opponentTeam[ 6 ];
        std::set<u8>
            _yieldEXP[ 6 ]; // idx's of the player's pkmn that will receive EXP when the pkmn faints
        u8  _opponentTeamSize;
        u8  _opponentPkmnPerm[ 6 ];

        pokemon* _playerTeam;
        u8       _playerTeamSize;
        u8       _playerPkmnPerm[ 6 ];

        u8 _playerPkmnOrigLevel[ 6 ]; // unaffected by permutation

        battlePolicy _policy;
        bool         _isWildBattle;
        bool         _opponentRuns; // wild pkmn tries to run whenever possible

        u8 _lastMoveChoice = 0;

        /*
         * @brief: Initializes the battle.
         */
        void initBattle( );

        /*
         * @brief: Computes the move target for single battles or lets the player choose a
         * target for double battles.
         */
        battleMoveSelection chooseTarget( const battleMoveSelection& p_move );

        /*
         * @brief: Makes the player select a move for the pokemon in slot p_slot.
         * @returns: a NO_OP move if the corresponding pokemon cannot move.
         */
        battleMoveSelection getMoveSelection( u8 p_slot, bool p_allowMegaEvolution );

        /*
         * @brief: Makes the player select an attack for the pokemon in slot p_slot.
         * A player may not be presented with a choice if the pkmn is forced to use an
         * attack or has no PP left.
         * @returns: A valid battleMoveSelection. May be of type CANCEL if the player
         * didn't choose a move.
         */
        battleMoveSelection chooseAttack( u8 p_slot, bool p_allowMegaEvolution );

        /*
         * @brief: Computes an AI move for the opposing pokemon in slot p_slot.
         */
        battleMoveSelection getAIMove( u8 p_slot );

        /*
         * @brief: Chooses which pokemon the ai sends out when one of its pokemon fainted.
         */
        u8 getNextAIPokemon( ) const;

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
         * @brief: Handles the capture of the wild Pokémon.
         */
        void handleCapture( );

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

        /*
         * @brief: Checks if any of the opponent's pkmn fainted and distributes EXP
         * correspondingly.
         */
        void distributeEXP( );

        /*
         * @brief: Teaches the specified move to the given pkmn. If the pkmn already knows
         * the specified move, nothing happens. If the pkmn knows 4 other moves already,
         * the player is asked which move should be forgotten.
         */
        void learnMove( pokemon* p_pokemon, u16 p_move );

      public:
        /*
         * @brief: Creates a new trainer battle.
         */
        battle( pokemon* p_playerTeam, u8 p_playerTeamSize, const battleTrainer& p_opponent,
                battlePolicy = DEFAULT_TRAINER_POLICY );

        /*
         * @brief: Creates a new trainer battle.
         */
        battle( pokemon* p_playerTeam, u8 p_playerTeamSize, u16 p_opponentId,
                battlePolicy = DEFAULT_TRAINER_POLICY );

        /*
         * @brief: Creates a new wild pkmn battle.
         */
        battle( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon p_opponent, u8 p_platform = 10,
                u8 p_platform2 = 10, u8 p_background = 0, battlePolicy = DEFAULT_WILD_POLICY,
                bool p_wildPkmnRuns = false );

        /*
         * @brief: Starts the battle.
         * @returns: positive values if the player won the battle; negative value if the
         * player lost.
         */
        battleEndReason start( );

        /*
         * @brief: Returns messages of the battle trainer.
         */
        const char* getString( u8 p_stringId );
    };
} // namespace BATTLE
