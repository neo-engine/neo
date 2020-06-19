/*
Pokémon neo
------------------------------

file        : battleUI.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <string>
#include <vector>

#include "battleDefines.h"
#include "saveGame.h"

namespace BATTLE {
    class battleUI {
        u8   _platform;
        u8   _platform2;
        u8   _background;
        battleMode _mode;
        bool _isWildBattle;
        battleTrainer* _battleTrainer = nullptr;

        u8   _currentLogLine = 0;

        u8   _curHP[ 2 ][ 2 ] = { { 0 } };

        void initTop( );
        void initSub( );

        /*
         * @brief: Updates the HP bar for the given pkmn to the specified value.
         * @param p_init: Initialize the hp bar first.
         */
        void updateHPbar( bool p_opponent, u8 p_pos, u8 p_newValue, bool p_init = false );

        /*
         * @brief: Loads a pkmn sprite for the given pkmn at the specified position.
         */
        void loadPkmnSprite( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: Animates the given ball type opening
         */
        void animateBallRelease( bool p_opponent, u8 p_slot, u8 p_ballId ) const;

        /*
         * @brief: Animates the given ball being thrown at the opponents pkmn in the given
         * slot.
         */
        void animateBallThrow( u8 p_slot, u8 p_ballId ) const;

        /*
         * @brief: Plays the shiny animation for the given pkmn.
         */
        void animateShiny( bool p_opponent, u8 p_slot, u8 p_shinyType = 0 ) const;

        /*
         * @brief: Plays the boosts animation for the given stat change.
         */
        void animateStatChange( bool p_opponent, u8 p_slot, bool p_down ) const;
     public:
        battleUI( ) { }

        battleUI( u8 p_platform, u8 p_platform2, u8 p_background, battleMode p_mode,
                  bool p_isWildBattle )
            : _platform( p_platform ), _platform2( p_platform2 ), _background( p_background ),
              _mode( p_mode ), _isWildBattle( p_isWildBattle ) {
        }

        /*
         * @brief: Initializes the battleUI. Destroys anything that was previously on the
         * screen.
         */
        void init( );

        void deinit( );

        /*
         * @brief: Returns a string correctly describing the given pkmn.
         */
        std::string getPkmnName( pokemon* p_pokemon, bool p_opponent,
                                 bool p_sentenceStart = true ) const;

        /*
         * @brief: Plays an animation that the pkmn at the specified slot currently has
         * the specified status condition.
         */
        void animateVolatileStatusCondition( bool p_opponent, u8 p_slot, volatileStatus p_status );

        /*
         * @brief: Plays an animation that the pkmn at the specified slot currently has
         * the specified status condition.
         */
        void animateStatusCondition( bool p_opponent, u8 p_slot, u8 p_status );

        /*
         * @brief: Runs the animation when the given move is prepared and prints
         * corresponding log commands.
         */
        void prepareMove( bool p_opponent, u8 p_slot, battleMove p_move );

        /*
         * @brief: Initializes and shows the log screen.
         */
        void resetLog( );

        /*
         * @brief: prints the given message to the battle log.
         */
        void log( std::string p_message );

        /*
         * @brief: Logs the boosts the given pkmn obtains.
         */
        void logBoosts( pokemon* p_pokemon, bool p_opponent, u8 p_slot, boosts p_intended,
                        boosts p_actual );

        /*
         * @brief: Logs that the given pkmn's ability is effective.
         */
        void logAbility( pokemon* p_pokemon, bool p_opponent );

        /*
         * @brief: Prints that the given pkmn's forewarn ability has identified the
         * specified move.
         */
        void logForewarn( pokemon* p_pokemon, bool p_opponent, u16 p_move );

        /*
         * @brief: Prints that the given pkmn's anticipation ability has identified
         * something.
         */
        void logAnticipation( pokemon* p_pokemon, bool p_opponent );

        /*
         * @brief: Prints that the given pkmn's frisk ability has identified the
         * specified items.
         */
        void logFrisk( pokemon* p_pokemon, bool p_opponent, std::vector<u16> p_itms );

        /*
         * @brief: Update the stats of the pkmn at the given position.
         */
        void updatePkmnStats( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: Hides the specified pokemon's stats.
         */
        void hidePkmnStats( bool p_opponent, u8 p_pos );

        /*
         * @brief: Hides the specified pokemon.
         */
        void hidePkmn( bool p_opponent, u8 p_pos );

        /*
         * @brief: Updates the pkmn at the specified position (e.g. after a form change)
         */
        void updatePkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: Recalls the given pkmn.
         * @param p_forced: True if the recall was forced by the trainer
         */
        void recallPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon, bool p_forced = false );

        /*
         * @brief: Sends out the given pkmn at the specified position.
         */
        void sendOutPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: shows the wild pkmn appear
         */
        void startWildBattle( pokemon* p_pokemon );

        /*
         * @brief: plays the intro for the given trainer
         */
        void startTrainerBattle( battleTrainer* p_trainer );

        /*
         * @brief: Shows the move selection for the given pkmn ("attack", "item", "pkmn",
         * "run", etc)
         */
        void showMoveSelection( pokemon* p_pokemon, u8 p_slot, u8 p_highlightedButton = -1 );

        /*
         * @brief: Shows the attack selection for the given pkmn.
         */
        void showAttackSelection( pokemon* p_pokemon, bool p_canUseMove[ 4 ],
                                  bool p_showMegaEvolution, u8 p_highlightedButton = -1,
                                  bool p_megaButtonActive = false );
    };
} // namespace BATTLE
