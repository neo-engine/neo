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

#include "move.h"
#include "battleDefines.h"
#include "battleUI.h"
#include "battleField.h"
#include "battleTrainer.h"
#include "defines.h"
#include "pokemon.h"
#include "type.h"
#include "abilityNames.h"
#include "pokemonNames.h"

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
        bool        _opponentRuns; // wild pkmn tries to run whenever possible

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
                battlePolicy = DEFAULT_WILD_POLICY, bool p_wildPkmnRuns = false );

        /*
         * @brief: Starts the battle.
         * @returns: positive values if the player won the battle; negative value if the
         * player lost.
         */
        battleEndReason start( );

    };
} // namespace BATTLE
