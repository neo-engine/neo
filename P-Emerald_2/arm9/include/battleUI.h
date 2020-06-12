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
         * @brief: Returns
         */
        inline std::string getOpponentString( bool p_opponent ) const {
            if( !p_opponent ) { return ""; }
            else if( _isWildBattle ) { return std::string( GET_STRING( 311 ) ); }
            else { return std::string( GET_STRING( 312 ) ); }
        }
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

        /*
         * @brief: prints the given message to the battle log.
         */
        void log( std::string p_message );

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
         * @brief: Updates the pkmn at the specified position (e.g. after a form change)
         */
        void updatePkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

        /*
         * @brief: Recalls the given pkmn.
         */
        void recallPkmn( bool p_opponent, u8 p_pos, pokemon* p_pokemon );

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
        void showMoveSelection( pokemon* p_pokemon, u8 p_highlightedButton = -1 );
    };
} // namespace BATTLE
