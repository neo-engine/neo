/*
Pokémon neo
------------------------------

file        : battle.cpp
author      : Philip Wellnitz
description :

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

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <functional>
#include <initializer_list>
#include <tuple>

#include "defines.h"

#include "battle.h"
#include "battleTrainer.h"

#include "ability.h"
#include "abilityNames.h"
#include "item.h"
#include "itemNames.h"
#include "move.h"
#include "moveNames.h"
#include "pokemon.h"
#include "pokemonData.h"

#include "fs.h"
#include "saveGame.h"

#include "bag.h"
#include "dex.h"

namespace BATTLE {
    std::string trainerClassNames[ 120 ] = {"Pokémon-Trainer"};

    battle::battle( pokemon* p_playerTeam, u16 p_opponentId, u16 p_maxRounds, weather p_weather,
                    u8 p_platform, u8 p_background, u16 p_AILevel, battleMode p_battleMode,
                    u8 p_platform2, bool p_allowMegaEvolution ) {
        _playerTeam         = p_playerTeam;
        _allowMegaEvolution = p_allowMegaEvolution;
        for( _playerTeamSize = 0; _playerTeamSize < 6; ++_playerTeamSize ) {
            if( !p_playerTeam[ _playerTeamSize ].m_boxdata.m_speciesId ) { break; }
        }

        _opponent = getBattleTrainer( p_opponentId, CURRENT_LANGUAGE );
        for( u8 i = 0; i < _opponent.m_data.m_numPokemon; ++i ) {
            _opponentTeam[ i ] = pokemon( _opponent.m_data.m_pokemon[ i ] );
        }

        _maxRounds      = p_maxRounds;
        _AILevel        = p_AILevel;
        m_battleMode    = p_battleMode;
        m_isWildBattle  = false;
        m_distributeEXP = true;

        _field    = field( p_weather );
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, m_isWildBattle );
    }
    battle::battle( pokemon* p_playerTeam, pokemon p_opponent, weather p_weather, u8 p_platform,
                    u8 p_platform2, u8 p_background, bool p_allowMegaEvolution ) {
        _maxRounds          = 0;
        _AILevel            = 0;
        _playerTeam         = p_playerTeam;
        _allowMegaEvolution = p_allowMegaEvolution;

        for( _playerTeamSize = 0; _playerTeamSize < 6; ++_playerTeamSize ) {
            if( !p_playerTeam[ _playerTeamSize ].m_boxdata.m_speciesId ) { break; }
        }

        _opponent          = battleTrainer( );
        _opponentTeam[ 0 ] = p_opponent;
        _opponentTeamSize  = 1;

        m_battleMode = SINGLE;

        _field    = field( p_weather );
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, m_isWildBattle );

        m_isWildBattle  = true;
        m_distributeEXP = true;
    }

    /**
     *  @brief:     Runs the battle.
     *  @returns:   -1 if opponent won, 0 if the battle resulted in a tie, 1 if player
     *  won; 2 on error.
     */
    s8 battle::start( ) {
        if( !_opponentTeamSize || !_playerTeamSize ) { return 2; }

        battleEndReason battleEnd = BATTLE_NONE;
        initBattle( );

        _round = 0;
        // Main battle loop
        while( !_maxRounds || ++_round < _maxRounds ) {
            battleMoveSelection moves[ 2 ][ 2 ] = {{}}; // fieldPosition -> selected move

            // Compute player's moves
            bool playerWillRun   = false;
            u16  playerWillCatch = 0;
            loop( ) {
                // Compute player's first pokemon's move
                moves[ field::PLAYER_SIDE ][ 0 ] = getMoveSelection( 0, _allowMegaEvolution );

                if( m_battleMode == DOUBLE ) {
                    // Compute player's first pokemon's move
                    if( moves[ field::PLAYER_SIDE ][ 0 ].m_type == RUN ) {
                        moves[ field::PLAYER_SIDE ][ 0 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 0}};
                        moves[ field::PLAYER_SIDE ][ 1 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 1}};
                        playerWillRun                    = true;
                        break;
                    }
                    if( moves[ field::PLAYER_SIDE ][ 0 ].m_type == CAPTURE ) {
                        playerWillCatch                  = moves[ field::PLAYER_SIDE ][ 0 ].m_param;
                        moves[ field::PLAYER_SIDE ][ 0 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 0}};
                        moves[ field::PLAYER_SIDE ][ 1 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 1}};
                        break;
                    }

                    moves[ field::PLAYER_SIDE ][ 1 ] = getMoveSelection(
                        1, _allowMegaEvolution
                               && moves[ field::PLAYER_SIDE ][ 0 ].m_type != MEGA_ATTACK );

                    // Player wishes to start over
                    if( moves[ field::PLAYER_SIDE ][ 1 ].m_type == CANCEL ) { continue; }

                    if( moves[ field::PLAYER_SIDE ][ 1 ].m_type == RUN ) {
                        moves[ field::PLAYER_SIDE ][ 0 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 0}};
                        moves[ field::PLAYER_SIDE ][ 1 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 1}};
                        playerWillRun                    = true;
                    }
                    if( moves[ field::PLAYER_SIDE ][ 1 ].m_type == CAPTURE ) {
                        playerWillCatch                  = moves[ field::PLAYER_SIDE ][ 1 ].m_param;
                        moves[ field::PLAYER_SIDE ][ 0 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 0}};
                        moves[ field::PLAYER_SIDE ][ 1 ] = {NO_OP, 0, {}, {field::PLAYER_SIDE, 1}};
                    }
                }
                break;
            }

            // Compute AI's moves
            for( u8 i = 0; i < u8( m_battleMode ); ++i ) {
                moves[ field::OPPONENT_SIDE ][ i ] = getAIMove( i );
            }

            if( playerWillRun && playerRuns( ) ) {
                endBattle( battleEnd = BATTLE_RUN );
                return battleEnd;
            }
            if( playerWillCatch && playerCaptures( playerWillCatch ) ) {
                endBattle( battleEnd = BATTLE_CAPTURE );
                return battleEnd;
            }

            // Sort moves
            std::vector<battleMoveSelection> selection = std::vector<battleMoveSelection>( );
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 0; j < u8( m_battleMode ); ++j ) {
                    selection.push_back( moves[ i ][ j ] );
                }

            auto sortedMoves = _field.computeSortedBattleMoves( selection );

            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                if( sortedMoves[ i ].m_type == battleMoveType::MEGA_ATTACK ) {
                    megaEvolve( sortedMoves[ i ].m_user );
                }

                if( sortedMoves[ i ].m_type == battleMoveType::MEGA_ATTACK
                    && sortedMoves[ i ].m_type == battleMoveType::ATTACK
                    && sortedMoves[ i ].m_type == battleMoveType::SWITCH_PURSUIT ) {
                    std::vector<battleMove> targets    = std::vector<battleMove>( );
                    std::vector<battleMove> targetedBy = std::vector<battleMove>( );

                    for( auto m : sortedMoves ) {
                        for( auto t : sortedMoves[ i ].m_target )
                            if( t == m.m_user ) { targets.push_back( m ); }
                        for( auto t : m.m_target )
                            if( t == sortedMoves[ i ].m_user ) { targetedBy.push_back( m ); }
                    }

                    _field.executeBattleMove( _battleUI, sortedMoves[ i ], targets, targetedBy );

                    checkAndRefillBattleSpots( slot::status::RECALLED );
                }

                if( sortedMoves[ i ].m_type == battleMoveType::SWITCH ) {
                    switchPokemon( sortedMoves[ i ].m_user, sortedMoves[ i ].m_param );
                }

                if( sortedMoves[ i ].m_type == battleMoveType::USE_ITEM ) {
                    useItem( sortedMoves[ i ].m_user, sortedMoves[ i ].m_param );
                }
            }
            _field.age( _battleUI );

            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                return battleEnd;
            }

            checkAndRefillBattleSpots( slot::status::FAINTED );
        }
        return battleEnd;
    }

    void battle::initBattle( ) {
        // TODO
    }

    battleMoveSelection battle::getMoveSelection( u8 p_slot, bool p_allowMegaEvolution ) {
        // TODO
        (void) p_slot;
        (void) p_allowMegaEvolution;

        return {NO_OP, 0, {}, {field::PLAYER_SIDE, 0}};
    }

    battleMoveSelection battle::getAIMove( u8 p_slot ) {
        // TODO
        (void) p_slot;

        return {NO_OP, 0, {}, {field::PLAYER_SIDE, 0}};
    }

    bool battle::endConditionHit( battle::battleEndReason& p_out ) {
        // TODO
        (void) p_out;

        return false;
    }

    void battle::endBattle( battle::battleEndReason p_battleEndReason ) {
        // TODO
    }

    bool battle::playerRuns( ) {
        // TODO

        return true;
    }

    bool battle::playerCaptures( u16 p_pokeball ) {
        // TODO
        (void) p_pokeball;

        return false;
    }

    void battle::megaEvolve( fieldPosition p_position ) {
        // TODO
        (void) p_position;
    }

    void battle::checkAndRefillBattleSpots( slot::status p_checkType ) {
        // TODO
        (void) p_checkType;
    }

    void battle::switchPokemon( fieldPosition p_toSwitch, u16 p_newIndex ) {
        // TODO
        (void) p_toSwitch;
        (void) p_newIndex;
    }

    void battle::useItem( fieldPosition p_target, u16 p_item ) {
        // TODO
        (void) p_target;
        (void) p_item;
    }
} // namespace BATTLE
