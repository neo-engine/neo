/*
Pokémon neo
------------------------------

file        : battle.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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

#include <string>

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleField.h"
#include "battle/battleSide.h"
#include "battle/battleSlot.h"
#include "battle/battleTrainer.h"
#include "battle/battleUI.h"
#include "defines.h"
#include "dex/dex.h"
#include "fs/data.h"
#include "gen/bgmNames.h"
#include "io/animations.h"
#include "io/choiceBox.h"
#include "io/strings.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "sts/partyScreen.h"

namespace BATTLE {
    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, const battleTrainer& p_opponentId,
                    battlePolicy p_policy ) {
        _playerTeam     = p_playerTeam;
        _playerTeamSize = p_playerTeamSize;

        _opponent = p_opponentId;

        _policy = p_policy;

        if( _policy.m_mode == BM_MOCK ) {
            _policy.m_mode = BM_SINGLE;
            _isMockBattle  = true;
        } else {
            _isMockBattle = false;
        }
        _isWildBattle = false;
        _AILevel      = _opponent.m_data.m_AILevel;

        _field    = field( _policy.m_mode, false, p_policy.m_weather );
        _battleUI = battleUI( _opponent.m_data.m_battlePlat1, _opponent.m_data.m_battlePlat2,
                              _opponent.m_data.m_battleBG, _policy.m_mode, false );

        _opponentRuns = false;

        // adjust difficulty

        switch( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) ) {
        case 0:
            if( _opponent.m_data.m_numPokemon > 2 ) { _opponent.m_data.m_numPokemon--; }
            if( _AILevel ) { _AILevel--; }
            break;
        case 3:
        default: break;
        case 6:
            // Check if the opponent actually has an extra pkmn
            if( _opponent.m_data.m_numPokemon < SAVE::NUM_PARTY_SLOTS
                && _opponent.m_data.m_pokemon[ _opponent.m_data.m_numPokemon ].m_speciesId ) {
                _opponent.m_data.m_numPokemon++;
            }
            if( _AILevel < 9 ) { _AILevel++; }
            break;
        }

        _opponentTeamSize = _opponent.m_data.m_numPokemon
            = std::min( SAVE::NUM_PARTY_SLOTS, _opponent.m_data.m_numPokemon );
        for( u8 i = 0; i < _opponentTeamSize; ++i ) {
            _opponentTeam[ i ] = pokemon( _opponent.m_data.m_pokemon[ i ] );
            _yieldEXP[ i ]     = std::set<u8>( );
        }
        switch( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) ) {
        case 0:
            for( u8 i = 0; i < _opponentTeamSize; ++i ) {
                if( _opponentTeam[ i ].m_level <= DIFF_EASY_LEVEL_DEC + MIN_OPP_LEVEL ) {
                    _opponentTeam[ i ].setLevel( MIN_OPP_LEVEL );
                } else {
                    _opponentTeam[ i ].setLevel( _opponentTeam[ i ].m_level - DIFF_EASY_LEVEL_DEC );
                }
                _opponentTeam[ i ].m_boxdata.m_moves[ 3 ] = 0;
            }
            break;
        case 3:
        default: break;
        case 6:
            for( u8 i = 0; i < _opponentTeamSize; ++i ) {
                if( _opponentTeam[ i ].m_level > MAX_PKMN_LEVEL - DIFF_HARD_LEVEL_INC ) {
                    _opponentTeam[ i ].setLevel( MAX_PKMN_LEVEL );
                } else {
                    _opponentTeam[ i ].setLevel( _opponentTeam[ i ].m_level + DIFF_HARD_LEVEL_INC );
                }
            }
            break;
        }
    }

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, u16 p_opponentId,
                    battlePolicy p_policy )
        : battle( p_playerTeam, p_playerTeamSize, FS::getBattleTrainer( p_opponentId ), p_policy ) {
    }

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon p_opponent, u8 p_platform,
                    u8 p_platform2, u8 p_background, battlePolicy p_policy, bool p_wildPkmnRuns ) {
        _playerTeam     = p_playerTeam;
        _playerTeamSize = p_playerTeamSize;

        _opponent          = battleTrainer( );
        _opponentTeam[ 0 ] = p_opponent;
        _yieldEXP[ 0 ]     = std::set<u8>( );
        _opponentTeamSize  = 1;

        _policy = p_policy;
        if( _policy.m_mode == BM_MOCK ) {
            _policy.m_mode = BM_SINGLE;
            _isMockBattle  = true;
        } else {
            _isMockBattle = false;
        }
        _AILevel      = _policy.m_aiLevel;
        _isWildBattle = true;

        // Initialize the field with the wild pkmn
        _field    = field( _policy.m_mode, true, p_policy.m_weather, PW_NONE, TR_NONE );
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, _policy.m_mode, true );

        _opponentRuns = p_wildPkmnRuns;
    }

    battle::battleEndReason battle::start( ) {
        if( !_opponentTeamSize || !_playerTeamSize ) { return battle::BATTLE_NONE; }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        swiWaitForVBlank( );
        scanKeys( );

        battleEndReason battleEnd = BATTLE_NONE;
        initBattle( );

        swiWaitForVBlank( );
        scanKeys( );

        _round = 0;
        // Main battle loop
        while( !_maxRounds || _round < _maxRounds ) {
            _round++;

            // register pkmn for exp
            for( u8 i = 0; i < getBattlingPKMNCount( _policy.m_mode ); ++i ) {
                for( u8 j = 0; j < getBattlingPKMNCount( _policy.m_mode ); ++j ) {
                    auto pk = _field.getPkmn( field::PLAYER_SIDE, j );
                    if( pk == nullptr ) { continue; }
                    if( pk->canBattle( ) ) { _yieldEXP[ i ].insert( _playerPkmnPerm[ j ] ); }
                }
            }

            battleMoveSelection moves[ field::NUM_SIDES ][ field::MAX_PKMN_PER_SIDE ]
                = { {} }; // fieldPosition -> selected move

            // Compute player's moves
            bool playerWillRun   = false;
            u16  playerWillCatch = 0;
            loop( ) {
                WAIT( HALF_SEC );

                if( _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 ) == nullptr ) {
                    // There is no first pkmn, so it cannot move.
                    moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type = MT_NO_OP_NO_CANCEL;
                }

                if( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type != MT_NO_OP_NO_CANCEL ) {
                    // Compute player's first pokemon's move
                    moves[ field::PLAYER_SIDE ][ field::PKMN_0 ]
                        = getMoveSelection( field::PKMN_0, _policy.m_allowMegaEvolution );
                }

                if( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type == MT_RUN ) {
                    moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                    moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                    playerWillRun                                = true;
                    break;
                }
                if( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type == MT_CAPTURE ) {
                    playerWillCatch = moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param;
                    moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                    moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                    break;
                }

                if( _policy.m_mode == BM_DOUBLE || _policy.m_mode == BM_MULTI_OPPONENT ) {
                    if( _field.getPkmn( field::PLAYER_SIDE, field::PKMN_1 ) == nullptr ) {
                        // There is no second pkmn, so it cannot move.
                        moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                    } else {
                        moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = getMoveSelection(
                            field::PKMN_1,
                            _policy.m_allowMegaEvolution
                                && !moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_megaEvolve );
                    }

                    // Player wishes to start over
                    if( moves[ field::PLAYER_SIDE ][ field::PKMN_1 ].m_type == MT_CANCEL ) {
                        continue;
                    }

                    if( moves[ field::PLAYER_SIDE ][ field::PKMN_1 ].m_type == MT_RUN ) {
                        // player wants to run, cancel any selected moves for pkmn 0
                        moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                        moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                        playerWillRun                                = true;
                    }
                    if( moves[ field::PLAYER_SIDE ][ field::PKMN_1 ].m_type == MT_CAPTURE ) {
                        // player wants to catch a wild pkmn, cancel any move for pkmn 0
                        playerWillCatch = moves[ field::PLAYER_SIDE ][ field::PKMN_1 ].m_param;
                        moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                        moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                    }
                }
                break;
            }

            // Check if opposing pkmn flees the battle
            if( _isWildBattle && _opponentRuns
                && _field.canSwitchOut( field::OPPONENT_SIDE, field::PKMN_0 ) ) {
                SOUND::playSoundEffect( SFX_BATTLE_ESCAPE );
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_BATTLE_WILD_PKMN_FLED ),
                          _opponentTeam[ field::PKMN_0 ].m_boxdata.m_name );
                _battleUI.log( std::string( buffer ) );
                WAIT( THREE_QUARTER_SEC );
                endBattle( battleEnd = BATTLE_RUN );
                return battleEnd;
            }

            // empty the log window / remove logs from previous rounds
            _battleUI.resetLog( );

            // Compute AI's moves
            for( u8 i = 0; i < getBattlingPKMNCount( _policy.m_mode ); ++i ) {
                moves[ field::OPPONENT_SIDE ][ i ] = getAIMove( i );
            }

            // Check if player can escape the battle
            if( ( playerWillRun && playerRuns( ) )
                || ( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type == MT_USE_ITEM
                     && ( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param == I_POKE_DOLL
                          || moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param == I_FLUFFY_TAIL
                          || moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param
                                 == I_POKE_TOY ) ) ) [[likely]] {
                if( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type == MT_USE_ITEM ) {
                    // player uses special item to escape the battle; remove said item
                    // from the player's bag
                    SAVE::SAV.getActiveFile( ).m_bag.erase(
                        BAG::bag::ITEMS, moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param, 1 );
                }

                SOUND::playSoundEffect( SFX_BATTLE_ESCAPE );
                _battleUI.log( std::string( GET_STRING( IO::STR_UI_BATTLE_GOT_AWAY_SAFELY ) ) );
                WAIT( THREE_QUARTER_SEC );
                endBattle( battleEnd = BATTLE_RUN );
                return battleEnd;
            } else if( playerWillRun ) [[unlikely]] {
                _battleUI.log( std::string( GET_STRING( IO::STR_UI_BATTLE_COULDNT_ESCAPE ) ) );
                WAIT( THREE_QUARTER_SEC );
            }

            // Check if player successfully catches
            if( playerWillCatch && playerCaptures( playerWillCatch ) ) {
                endBattle( battleEnd = BATTLE_CAPTURE );
                return battleEnd;
            }

            // Sort moves
            std::vector<battleMoveSelection> selection = std::vector<battleMoveSelection>( );
            for( u8 side = 0; side < field::NUM_SIDES; ++side )
                for( u8 i = 0; i < getBattlingPKMNCount( _policy.m_mode ); ++i ) {
                    // Check for pursuit
                    if( moves[ side ][ i ].m_type == MT_ATTACK
                        && moves[ side ][ i ].m_param == M_PURSUIT ) [[unlikely]] {
                        if( moves[ moves[ side ][ i ].m_target.first ]
                                 [ moves[ side ][ i ].m_target.second ]
                                     .m_type
                            == MT_SWITCH ) [[unlikely]] {
                            moves[ side ][ i ].m_type = MT_SWITCH_PURSUIT;
                        }
                    }
                    if( moves[ side ][ i ].m_type == MT_NO_OP_NO_CANCEL ) {
                        moves[ side ][ i ].m_type = MT_NO_OP;
                    }
                    selection.push_back( moves[ side ][ i ] );
                }

            auto sortedMoves = _field.computeSortedBattleMoves( &_battleUI, selection );

            // Mega evolve battlers
            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                if( sortedMoves[ i ].m_megaEvolve && sortedMoves[ i ].m_type == MT_ATTACK ) {
                    megaEvolve( sortedMoves[ i ].m_user );
                } else {
                    sortedMoves[ i ].m_megaEvolve = false;
                }
            }

            // Execute moves
            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                WAIT( HALF_SEC );
                // show messages for special items
                if( sortedMoves[ i ].m_type == MT_MESSAGE_ITEM ) [[unlikely]] {
                    auto itmnm = FS::getItemName( sortedMoves[ i ].m_param );
                    auto fmt = std::string( GET_STRING( IO::STR_UI_BATTLE_USE_ITEM_TO_ACT_FIRST ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ), itmnm.c_str( ),
                              _battleUI
                                  .getPkmnName( _field.getPkmn( sortedMoves[ i ].m_user.first,
                                                                sortedMoves[ i ].m_user.second ),
                                                sortedMoves[ i ].m_user.first, false )
                                  .c_str( ),
                              itmnm.c_str( ) );
                    _battleUI.log( std::string( buffer ) );
                }

                // show special messages for special moves
                if( sortedMoves[ i ].m_type == MT_MESSAGE_MOVE ) [[unlikely]] {
                    auto pnm = std::string(
                        _battleUI.getPkmnName( _field.getPkmn( sortedMoves[ i ].m_user.first,
                                                               sortedMoves[ i ].m_user.second ),
                                               sortedMoves[ i ].m_user.first, false ) );
                    switch( sortedMoves[ i ].m_param ) {
                    case M_SHELL_TRAP:
                        snprintf( buffer, TMP_BUFFER_SIZE,
                                  GET_STRING( IO::STR_UI_BATTLE_PREPARE_SHELL_TRAP ),
                                  pnm.c_str( ) );
                        _battleUI.log( buffer );
                        _field.addVolatileStatus( &_battleUI, sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second, VS_SHELLTRAP, 1 );
                        break;
                    case M_FOCUS_PUNCH:
                        snprintf( buffer, TMP_BUFFER_SIZE,
                                  GET_STRING( IO::STR_UI_BATTLE_PREPARE_FOCUS_PUNCH ),
                                  pnm.c_str( ) );
                        _battleUI.log( buffer );
                        _field.addVolatileStatus( &_battleUI, sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second, VS_FOCUSPUNCH,
                                                  1 );
                        break;
                    case M_BEAK_BLAST:
                        snprintf( buffer, TMP_BUFFER_SIZE,
                                  GET_STRING( IO::STR_UI_BATTLE_PREPARE_BEAK_BLAST ),
                                  pnm.c_str( ) );
                        _field.addVolatileStatus( &_battleUI, sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second, VS_BEAKBLAST, 1 );
                        _battleUI.log( buffer );
                        break;
                    default: break;
                    }
                }

                // pkmn attacks
                if( sortedMoves[ i ].m_type == MT_ATTACK ) [[likely]] {
                    std::vector<battleMove> targets    = std::vector<battleMove>( );
                    std::vector<battleMove> targetedBy = std::vector<battleMove>( );

                    for( auto m : sortedMoves ) {
                        for( auto t : sortedMoves[ i ].m_target )
                            if( t == m.m_user ) { targets.push_back( m ); }
                        for( auto t : m.m_target )
                            if( t == sortedMoves[ i ].m_user ) { targetedBy.push_back( m ); }
                    }

                    _field.executeBattleMove( &_battleUI, sortedMoves[ i ], targets, targetedBy );

                    distributeEXP( );
                    checkAndRefillBattleSpots( slot::status::RECALLED );
                }

                // execute switch-outs
                if( sortedMoves[ i ].m_type == MT_SWITCH ) {
                    switchPokemon( sortedMoves[ i ].m_user, sortedMoves[ i ].m_param );
                }

                // actually use items
                if( sortedMoves[ i ].m_type == MT_USE_ITEM ) {
                    useItem( sortedMoves[ i ].m_user, sortedMoves[ i ].m_param );
                }

                // check if the battle ended (e.g. if all pkmn got ko'ed)
                if( endConditionHit( battleEnd ) ) {
                    endBattle( battleEnd );
                    return battleEnd;
                }
            }

            // execute end-of-turn effects
            _field.age( &_battleUI );

            distributeEXP( );
            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                return battleEnd;
            }

            checkAndRefillBattleSpots( slot::status::FAINTED );
        }
        endBattle( battleEnd );
        return battleEnd;
    }

    void battle::initBattle( ) {
        SOUND::initBattleSound( );

        _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

        // prepare pkmn permutation; used to restore pkmn order after battle
        for( u8 i = 0; i < SAVE::NUM_PARTY_SLOTS; ++i ) {
            _opponentPkmnPerm[ i ]    = i;
            _playerPkmnPerm[ i ]      = i;
            _playerPkmnOrigLevel[ i ] = ( i < _playerTeamSize ) * _playerTeam[ i ].m_level;
        }

        sortPkmn( true );
        sortPkmn( false );

        if( _isWildBattle ) {
            _battleUI.startWildBattle( &_opponentTeam[ 0 ] );
            _field.setSlot( field::OPPONENT_SIDE, field::PKMN_0, &_opponentTeam[ 0 ] );
        } else {
            _battleUI.startTrainerBattle( &_opponent );
        }

        // send out pkmn (for opponent only if not a wild pkmn)
        for( u8 side = u8( _isWildBattle ); side < field::NUM_SIDES; ++side ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _policy.m_mode ); ++j ) {
                if( ( side && _playerTeamSize <= j ) || ( !side && _opponentTeamSize <= j ) ) {
                    _field.setSlot( !side, j, nullptr );
                    continue;
                }
                _battleUI.sendOutPkmn( !side, j,
                                       ( side ? &_playerTeam[ j ] : &_opponentTeam[ j ] ) );
                _field.setSlot( !side, j, ( side ? &_playerTeam[ j ] : &_opponentTeam[ j ] ) );
            }
        }

        _field.init( &_battleUI );

        for( u8 side = 0; side < field::NUM_SIDES; ++side ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _policy.m_mode ); ++j ) {
                if( ( side && _playerTeamSize <= j ) || ( !side && _opponentTeamSize <= j ) ) {
                    continue;
                }
                _field.checkOnSendOut( &_battleUI, !side, j );
            }
        }
    }

    battleMoveSelection battle::chooseTarget( const battleMoveSelection& p_move ) {
        auto res       = p_move;
        res.m_moveData = FS::getMoveData( res.m_param );

        if( _policy.m_mode == BM_SINGLE ) {
            switch( res.m_moveData.m_target ) {
            case TG_ANY:
            case TG_ANY_FOE:
            case TG_ALL_FOES_AND_ALLY:
            case TG_ALL_FOES:
            case TG_RANDOM: res.m_target = { field::OPPONENT_SIDE, field::PKMN_0 }; return res;
            case TG_SELF:
            case TG_ALLY_OR_SELF:
            case TG_ALL_ALLIES: res.m_target = { field::PLAYER_SIDE, field::PKMN_0 }; return res;
            default: res.m_target = { 255, 255 }; return res;
            }
        }

        if( getBattlingPKMNCount( _policy.m_mode ) == 2 ) {
            // Double battle: User needs to choose / confirm target
            u8   possibleTargets = 0;
            u8   initialSel      = 0;
            bool hasChoice       = true;

            const u8 opp1
                = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_1 ) != nullptr ? ( 1 << 0 ) : 0,
                opp2
                = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_0 ) != nullptr ? ( 1 << 1 ) : 0,
                self = _field.getPkmn( field::PLAYER_SIDE, p_move.m_user.second ) != nullptr
                           ? ( 1 << ( 2 + p_move.m_user.second ) )
                           : 0,
                ally = _field.getPkmn( field::PLAYER_SIDE, !p_move.m_user.second ) != nullptr
                           ? ( 1 << ( 2 + !p_move.m_user.second ) )
                           : 0;
            if( !opp2 ) { initialSel++; }
            switch( res.m_moveData.m_target ) {
            case TG_ANY: possibleTargets |= opp1 | opp2 | ally; break;
            case TG_ANY_FOE: possibleTargets |= opp1 | opp2; break;
            case TG_ALLY_OR_SELF: possibleTargets |= self | ally; break;
            case TG_ALLY:
                initialSel   = ( 2 + !p_move.m_user.second );
                res.m_target = { field::PLAYER_SIDE, !p_move.m_user.second };
                possibleTargets |= ally;
                hasChoice = false;
                break;
            case TG_RANDOM: res.m_target = { field::OPPONENT_SIDE, rand( ) % 2 }; [[fallthrough]];
            case TG_SCRIPTED:
            case TG_NONE:
            case TG_SELF:
                initialSel = ( 2 + p_move.m_user.second );
                possibleTargets |= self;
                hasChoice = false;
                break;
            case TG_ALL_FOES:
                res.m_target = { field::OPPONENT_SIDE, field::PKMN_0 };
                [[fallthrough]];
            case TG_FOE_SIDE:
                possibleTargets |= opp1 | opp2;
                hasChoice = false;
                break;
            case TG_FIELD:
                possibleTargets |= opp1 | opp2 | ally | self;
                hasChoice = false;
                break;
            case TG_ALL_FOES_AND_ALLY:
                res.m_target = { field::OPPONENT_SIDE, field::PKMN_0 };
                possibleTargets |= opp1 | opp2 | ally;
                hasChoice = false;
                break;
            case TG_ALL_ALLIES:
                res.m_target = { field::PLAYER_SIDE, field::PKMN_0 };
                [[fallthrough]];
            case TG_ALLY_SIDE:
            case TG_ALLY_TEAM:
                possibleTargets |= self | ally;
                hasChoice = false;
                break;
            }

            IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT_CANCEL );

            auto getPkmn
                = [ & ]( bool p_opp, u8 p_slot ) { return _field.getPkmn( p_opp, p_slot ); };

            u8 rs = cb.getResult(
                [ & ]( u8 ) {
                    return _battleUI.showTargetSelection( possibleTargets, hasChoice, getPkmn );
                },
                [ & ]( u8 p_selection ) {
                    _battleUI.showTargetSelection( possibleTargets, hasChoice, getPkmn,
                                                   p_selection );
                },
                initialSel );
            if( rs < 4 ) {
                // player selects a target
                if( hasChoice ) { res.m_target = { rs < 2, ( rs < 2 ) ^ ( rs % 2 ) }; }
                return res;
            }
            // cancel selection
            res.m_type = MT_CANCEL;
            return res;
        }
        res.m_type   = MT_CANCEL;
        res.m_target = { 255, 255 };
        return res;
    }

    battleMoveSelection battle::chooseAttack( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };
        res.m_type              = MT_ATTACK;

        bool mega = _field.canMegaEvolve( field::PLAYER_SIDE, p_slot ) && p_allowMegaEvolution;

        // check if pkmn has move left, otherwise make it use "Struggle"
        bool canUse[ 4 ], strgl = false;
        for( u8 i = 0; i < 4; ++i ) {
            canUse[ i ] = _field.canSelectMove( field::PLAYER_SIDE, p_slot, i );
            strgl       = strgl || canUse[ i ];
        }

        if( _isMockBattle ) {
            // Always choose Tackle
            _battleUI.showAttackSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), canUse,
                                           mega );
            auto curSel = 0;
            _battleUI.showAttackSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), canUse,
                                           mega, curSel, res.m_megaEvolve );
            WAIT( HALF_SEC );
            _battleUI.showAttackSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), canUse,
                                           mega, curSel = 2, res.m_megaEvolve );
            WAIT( FULL_SEC );
            res.m_param     = _field.getPkmn( field::PLAYER_SIDE, p_slot )->getMove( curSel );
            _lastMoveChoice = curSel;
            return chooseTarget( res );
        }

        if( !strgl ) {
            // pkmn will struggle
            res.m_param = M_STRUGGLE;
            return chooseTarget( res );
        }

        IO::choiceBox cb     = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT_CANCEL );
        u8            curSel = _lastMoveChoice;

        loop( ) {
            u8 rs = cb.getResult(
                [ & ]( u8 ) {
                    return _battleUI.showAttackSelection(
                        _field.getPkmn( field::PLAYER_SIDE, p_slot ), canUse, mega );
                },
                [ & ]( u8 p_selection ) {
                    curSel = p_selection;
                    _battleUI.showAttackSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ),
                                                   canUse, mega, curSel, res.m_megaEvolve );
                },
                curSel );

            if( rs == IO::choiceBox::BACK_CHOICE || rs == 4 ) {
                res.m_type = MT_CANCEL;
                return res;
            }
            if( rs < 4 ) {
                // player selects an attack
                if( canUse[ rs ] ) {
                    res.m_param = _field.getPkmn( field::PLAYER_SIDE, p_slot )->getMove( curSel );
                    auto tmp    = chooseTarget( res );

                    if( tmp.m_type != MT_CANCEL ) {
                        _lastMoveChoice = curSel;
                        return tmp;
                    } else {
                        res.m_param = 0;
                    }
                }
            }
            if( mega && rs == 5 ) { // Toggle Mega Evolution
                res.m_megaEvolve = !res.m_megaEvolve;
            }
        }
    }

    battleMoveSelection battle::handleMoveSelectionSelection( u8 p_slot, bool p_allowMegaEvolution,
                                                              u8 p_button ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };
        switch( p_button ) {
        case 0: // Choose attack
            SOUND::playSoundEffect( SFX_CHOOSE );
            res = chooseAttack( p_slot, p_allowMegaEvolution );
            if( res.m_type != MT_CANCEL ) { return res; }
            break;
        case 1: { // Choose pkmn
            SOUND::playSoundEffect( SFX_CHOOSE );

            STS::partyScreen pt = STS::partyScreen(
                _playerTeam, _playerTeamSize, false, false, false, 1, false, false, false, true,
                getBattlingPKMNCount( _policy.m_mode ), p_slot );

            auto r = pt.run( p_slot );

            _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

            for( u8 side = 0; side < field::NUM_SIDES; ++side )
                for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                    auto st = _field.getSlotStatus( side, j2 );
                    if( st == slot::status::NORMAL ) {
                        _battleUI.updatePkmn( side, j2, _field.getPkmn( side, j2 ) );
                    }
                }

            if( r.getSelectedPkmn( ) < 255 ) {
                res.m_type  = MT_SWITCH;
                res.m_param = r.getSelectedPkmn( );
                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot );
                return res;
            }
            break;
        }
        case 2: // Run / Cancel
            if( _isWildBattle ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                res.m_type = MT_RUN;
            } else if( p_slot ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                res.m_type = MT_CANCEL;
            } else {
                cooldown = COOLDOWN_COUNT;
                break;
            }
            return res;
        case 3: { // Choose item
            SOUND::playSoundEffect( SFX_CHOOSE );

            BAG::bagViewer bv = BAG::bagViewer(
                _playerTeam, _isWildBattle ? BAG::bagViewer::WILD_BATTLE : BAG::bagViewer::BATTLE );
            u16 itm = bv.getItem( );

            if( itm ) {
                auto idata = FS::getItemData( itm );

                if( idata.m_itemType == BAG::ITEMTYPE_POKEBALL ) {
                    // Player throws a ball
                    res.m_type  = MT_CAPTURE;
                    res.m_param = itm;
                } else if( ( idata.m_itemType & 15 ) == 2 ) {
                    // Already used
                    res.m_type  = MT_NO_OP_NO_CANCEL;
                    res.m_param = 0;
                } else {
                    res.m_type  = MT_USE_ITEM;
                    res.m_param = itm;
                }
            }

            _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

            for( u8 side = 0; side < field::NUM_SIDES; ++side )
                for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                    auto st = _field.getSlotStatus( side, j2 );
                    if( st == slot::status::NORMAL ) {
                        _battleUI.updatePkmn( side, j2, _field.getPkmn( side, j2 ) );
                    }
                }

            if( itm ) {
                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot );
                return res;
            }
            break;
        }
        default: break;
        }
        res.m_type = MT_CANCEL;
        return res;
    }

    battleMoveSelection battle::getMoveSelection( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };

        if( !_field.canSelectMove( field::PLAYER_SIDE, p_slot ) ) {
            // pkmn is hibernating / charging
            return _field.getStoredMove( field::PLAYER_SIDE, p_slot );
        }

        auto choices
            = _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot );
        u8 curSel = 0;
        _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot, curSel );

        if( _isMockBattle ) {
            if( _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 )->m_stats.m_curHP * 2
                    > _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 )->m_stats.m_maxHP
                && _round <= 2 ) { // Choose tackle
                WAIT( FULL_SEC );

                SOUND::playSoundEffect( SFX_CHOOSE );
                return chooseAttack( p_slot, p_allowMegaEvolution );
            } else { // throw a poke ball
                WAIT( FULL_SEC );

                SOUND::playSoundEffect( SFX_CHOOSE );
                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot,
                                             curSel = 3 );
                WAIT( FULL_SEC );

                BAG::bagViewer bv = BAG::bagViewer( _playerTeam, BAG::bagViewer::MOCK_BATTLE );
                bv.getItem( );
                res.m_type  = MT_CAPTURE;
                res.m_param = I_POKE_BALL;

                _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

                for( u8 side = 0; side < field::NUM_SIDES; ++side )
                    for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                        auto st = _field.getSlotStatus( side, j2 );
                        if( st == slot::status::NORMAL ) {
                            _battleUI.updatePkmn( side, j2, _field.getPkmn( side, j2 ) );
                        }
                    }

                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot );
                return res;
            }
        }

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            for( auto i : choices ) {
                if( i.first.inRange( touch ) ) {
                    _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ),
                                                 p_slot, curSel = i.second );

                    bool bad = false;
                    while( touch.px || touch.py ) {
                        swiWaitForVBlank( );
                        if( !i.first.inRange( touch ) ) {
                            bad = true;
                            break;
                        }
                        scanKeys( );
                        touchRead( &touch );
                        swiWaitForVBlank( );
                    }
                    if( !bad ) {
                        res = handleMoveSelectionSelection( p_slot, p_allowMegaEvolution, curSel );
                        if( res.m_type != MT_CANCEL ) { return res; }
                        _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ),
                                                     p_slot );
                        _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ),
                                                     p_slot, curSel );
                    }
                }
            }

            if( p_slot && ( pressed & KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                res.m_type = MT_CANCEL;
                return res;
            }
            if( pressed & KEY_A ) {
                res = handleMoveSelectionSelection( p_slot, p_allowMegaEvolution, curSel );
                if( res.m_type != MT_CANCEL ) { return res; }

                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot );
                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot,
                                             curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 3;
                } else if( curSel < 3 ) {
                    curSel++;
                } else {
                    curSel = 0;
                }

                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot,
                                             curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 1;
                } else {
                    curSel--;
                }

                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot,
                                             curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 2;
                } else {
                    curSel = 0;
                }

                _battleUI.showMoveSelection( _field.getPkmn( field::PLAYER_SIDE, p_slot ), p_slot,
                                             curSel );

                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }

        return res;
    }

    bool battle::endConditionHit( battle::battleEndReason& p_out ) {
        if( _isWildBattle
            && _field.getSlotStatus( field::OPPONENT_SIDE, field::PKMN_0 ) == slot::FAINTED ) {
            distributeEXP( );
            p_out = battleEndReason::BATTLE_PLAYER_WON;
            return true;
        }
        if( _isWildBattle
            && _field.getSlotStatus( field::OPPONENT_SIDE, field::PKMN_0 ) == slot::RECALLED ) {
            // wild pkmn fled
            p_out = battleEndReason::BATTLE_OPPONENT_RAN;
            return true;
        }

        if( _round >= _maxRounds && _maxRounds ) {
            p_out = battleEndReason::BATTLE_ROUND_LIMIT;
            return true;
        }

        // Check amount of non-koed PKMN
        // PLAYER
        u8 pkmnCnt = 0;
        for( u8 i = 0; i < _playerTeamSize; ++i ) {
            if( _playerTeam[ i ].canBattle( ) ) { pkmnCnt++; }
        }
        if( !pkmnCnt ) {
            p_out = battleEndReason::BATTLE_OPPONENT_WON;
            return true;
        }

        // OPPONENT
        pkmnCnt = 0;

        for( u8 i = 0; i < _opponentTeamSize; ++i ) {
            if( _opponentTeam[ i ].canBattle( ) ) { pkmnCnt++; }
        }
        if( !pkmnCnt ) {
            p_out = battleEndReason::BATTLE_PLAYER_WON;
            return true;
        }

        return false;
    }

    u16  MOVE_BUFFER[ 20 ];
    void battle::endBattle( battle::battleEndReason p_battleEndReason ) {
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        if( p_battleEndReason == BATTLE_OPPONENT_WON ) { SOUND::setVolume( 0 ); }
        if( _isWildBattle && p_battleEndReason != BATTLE_RUN ) {
            if( p_battleEndReason != BATTLE_OPPONENT_WON ) { SOUND::playBGM( BGM_VICTORY_WILD ); }
            if( p_battleEndReason == BATTLE_CAPTURE ) {
                handleCapture( );
            } else {
                WAIT( THREEHALF_SEC );
            }
        } else if( !_isWildBattle ) {
            if( p_battleEndReason == BATTLE_PLAYER_WON ) {
                SOUND::playBGM( SOUND::BGMforTrainerWin( _opponent.m_data.m_trainerClass ) );
                _battleUI.handleBattleEnd( true );

                if( _opponent.m_data.m_moneyEarned ) {
                    snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_BATTLE_WIN_MONEY ),
                              _opponent.m_data.m_moneyEarned );
                    _battleUI.log( buffer );
                    WAIT( FULL_SEC );
                    SAVE::SAV.getActiveFile( ).m_money += _opponent.m_data.m_moneyEarned;
                    if( SAVE::SAV.getActiveFile( ).m_money > 999'999'999 ) {
                        SAVE::SAV.getActiveFile( ).m_money = 999'999'999;
                    }
                }
            }
            if( p_battleEndReason == BATTLE_OPPONENT_WON ) {
                _battleUI.handleBattleEnd( false );

                if( _opponent.m_data.m_moneyEarned ) {
                    snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_BATTLE_LOSE_MONEY ),
                              _opponent.m_data.m_moneyEarned );
                    _battleUI.log( buffer );
                    WAIT( FULL_SEC );

                    if( SAVE::SAV.getActiveFile( ).m_money < _opponent.m_data.m_moneyEarned ) {
                        SAVE::SAV.getActiveFile( ).m_money = 0;
                    } else {
                        SAVE::SAV.getActiveFile( ).m_money -= _opponent.m_data.m_moneyEarned;
                    }
                }
            }
        }

        restoreInitialOrder( false );
        resetBattleTransformations( false );

        // Check for evolutions / attack learn

        for( u8 i = 0; i < _playerTeamSize; ++i ) {
            if( _playerTeam[ i ].m_level != _playerPkmnOrigLevel[ i ] ) {
                // pkmn was elevated to a new level, check for new moves

                FS::getLearnMoves( _playerTeam[ i ].getSpecies( ), _playerTeam[ i ].getForme( ),
                                   _playerPkmnOrigLevel[ i ] + 1, _playerTeam[ i ].m_level, 20,
                                   MOVE_BUFFER );

                for( u8 j = 0; j < 20; ++j ) {
                    if( !MOVE_BUFFER[ j ] ) { break; }
                    bool gd = true;
                    for( u8 mv = 0; mv < 4; ++mv ) {
                        if( _playerTeam[ i ].getMove( i ) == MOVE_BUFFER[ j ] ) { gd = false; }
                    }
                    if( gd ) {
                        _battleUI.showTopMessagePkmn( &_playerTeam[ i ] );
                        _battleUI.printTopMessage( 0, true );
                        _playerTeam[ i ].learnMove(
                            MOVE_BUFFER[ j ],
                            [ & ]( const char* p_message ) {
                                _battleUI.printTopMessage( p_message, true );
                                WAIT( THREEHALF_SEC );
                            },
                            [ & ]( boxPokemon* p_pkmn, u16 ) -> u8 {
                                IO::choiceBox cb = IO::choiceBox(
                                    IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT_CANCEL );
                                u8 curSel = 0;

                                bool canUse[ 4 ] = { 1, 1, 1, 1 };
                                u8   rs          = cb.getResult(
                                               [ & ]( u8 ) {
                                        return _battleUI.showAttackSelection( p_pkmn, canUse,
                                                                                         false );
                                               },
                                               [ & ]( u8 p_selection ) {
                                        curSel = p_selection;
                                        _battleUI.showAttackSelection( p_pkmn, canUse, false,
                                                                                  curSel, false );
                                               },
                                               curSel );

                                if( rs < 4 ) {
                                    return rs;
                                } else {
                                    return 4;
                                }
                            },
                            [ & ]( const char* p_message ) {
                                IO::yesNoBox yn;
                                _battleUI.printTopMessage( p_message, true );
                                return yn.getResult(
                                           [ & ]( ) { return _battleUI.printYNMessage( 254 ); },
                                           [ & ]( IO::yesNoBox::selection p_selection ) {
                                               _battleUI.printYNMessage( p_selection
                                                                         == IO::yesNoBox::NO );
                                           } )
                                       == IO::yesNoBox::YES;
                            } );
                        _battleUI.hideTopMessage( );
                    }
                }
            }
        }
        _battleUI.deinit( );
        SOUND::deinitBattleSound( );

        for( u8 i = 0; i < _playerTeamSize; ++i ) {
            if( _playerTeam[ i ].m_level != _playerPkmnOrigLevel[ i ] ) {
                // Check for evolution

                auto edata = FS::getPkmnEvolveData( _playerTeam[ i ].getSpecies( ),
                                                    _playerTeam[ i ].getForme( ) );

                u8 ev = 0;
                if( _playerTeam[ i ].getItem( ) != I_EVERSTONE
                    && ( ev = _playerTeam[ i ].canEvolve( 0, EVOMETHOD_LEVEL_UP, &edata ) ) ) {

                    u16 oldsp = _playerTeam[ i ].getSpecies( );
                    u8  oldfm = _playerTeam[ i ].getForme( );
                    u16 newsp = edata.m_evolutions[ ev - 1 ].m_target;
                    u8  newfm = edata.m_evolutions[ ev - 1 ].m_targetForme;

                    if( IO::ANIM::evolvePkmn(
                            oldsp, oldfm, newsp, newfm, _playerTeam[ i ].isShiny( ),
                            _playerTeam[ i ].isFemale( ), _playerTeam[ i ].getPid( ), true ) ) {
                        _playerTeam[ i ].evolve( );
                    }
                }
            }
        }
    }

    bool battle::playerRuns( ) {
        auto p1 = _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 );
        if( p1 == nullptr ) { return true; }

        if( p1->getItem( ) == I_SMOKE_BALL ) { return true; }

        if( !_field.suppressesAbilities( ) ) {
            if( p1->getAbility( ) == A_RUN_AWAY ) { return true; }
        }

        u16 ownSpeed = _field.getStat( field::PLAYER_SIDE, field::PKMN_0, SPEED );
        u16 oppSpeed = _field.getStat( field::OPPONENT_SIDE, field::PKMN_0, SPEED );

        if( ownSpeed > oppSpeed || oppSpeed == 0 ) { return true; }
        if( !_field.canSwitchOut( field::PLAYER_SIDE, field::PKMN_0 ) ) { return false; }

        return ( ownSpeed + ( rand( ) % oppSpeed ) >= oppSpeed );
    }

    bool battle::playerCaptures( u16 p_pokeball ) {
        if( !_isMockBattle ) {
            SAVE::SAV.getActiveFile( ).m_bag.erase( BAG::bag::ITEMS, p_pokeball, 1 );
        }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        u16          ballCatchRate = 2;

        auto plpk = _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 );
        if( plpk == nullptr ) { return false; }
        auto wild = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_0 );
        if( wild == nullptr ) { return false; }
        u16 specId = wild->getSpecies( );

        auto p = _field.getPkmnData( field::OPPONENT_SIDE, field::PKMN_0 );

        switch( p_pokeball ) {
        case I_SAFARI_BALL:
        case I_SPORT_BALL:
        case I_GREAT_BALL: ballCatchRate = 3; break;
        case I_ULTRA_BALL: ballCatchRate = 4; break;
        case I_MASTER_BALL: ballCatchRate = 512; break;

        case I_LEVEL_BALL:
            if( plpk->m_level > wild->m_level ) ballCatchRate = 4;
            if( plpk->m_level / 2 > wild->m_level ) ballCatchRate = 8;
            if( plpk->m_level / 4 > wild->m_level ) ballCatchRate = 16;
            break;
        case I_LURE_BALL:
            if( PLAYER_IS_FISHING ) ballCatchRate = 6;
            break;
        case I_MOON_BALL:
            if( wild->canEvolve( I_MOON_STONE ) ) ballCatchRate = 16;
            break;
        case I_LOVE_BALL:
            if( wild->isFemale( ) * plpk->isFemale( ) < 0 ) ballCatchRate = 16;
            break;
        case I_HEAVY_BALL:
            ballCatchRate
                = std::min( 128, _field.getWeight( field::OPPONENT_SIDE, field::PKMN_0 ) >> 2 );
            break;
        case I_FAST_BALL:
            if( p.m_baseForme.m_bases[ 5 ] >= 100 ) ballCatchRate = 16;
            break;

        case I_REPEAT_BALL:
            if( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ specId / 8 ] & ( 1LLU << ( specId % 8 ) ) )
                ballCatchRate = 6;
            break;
        case I_TIMER_BALL: ballCatchRate = std::min( _round + 10 / 5, 8 ); break;
        case I_NEST_BALL: ballCatchRate = std::max( ( 40 - wild->m_level ) / 5, 2 ); break;
        case I_NET_BALL:
            if( _field.hasType( field::OPPONENT_SIDE, field::PKMN_0, TYPE_BUG )
                || _field.hasType( field::OPPONENT_SIDE, field::PKMN_0, TYPE_WATER ) ) {
                ballCatchRate = 6;
            }
            break;
        case I_DIVE_BALL:
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::moveMode::DIVE )
                ballCatchRate = 7;
            break;

        case I_QUICK_BALL:
            if( _round < 2 ) ballCatchRate = 10;
            break;
        case I_DUSK_BALL:
            if( possible( M_DIG, 0 ) || getCurrentDaytime( ) == DAYTIME_EVENING
                || getCurrentDaytime( ) == DAYTIME_NIGHT )
                ballCatchRate = 7;
            break;

        case I_PREMIER_BALL:
            if( wild->isShiny( ) ) { ballCatchRate = 512; }
            ballCatchRate = 2;
            break;

        case I_BEAST_BALL: ballCatchRate = 0; break;

        default: break;
        }

        u8 status = 2;
        if( wild->m_status.m_isAsleep || wild->m_status.m_isFrozen ) status = 4;
        if( wild->m_status.m_isParalyzed || wild->m_status.m_isPoisoned
            || wild->m_status.m_isBadlyPoisoned || wild->m_status.m_isBurned )
            status = 3;

        u32 catchRate = ( 3 * wild->m_stats.m_maxHP - 2 * wild->m_stats.m_curHP ) * p.m_catchrate
                        * ballCatchRate / 3 / wild->m_stats.m_maxHP * status;
        if( !catchRate ) { catchRate = 1; }
        u32 pr   = u32( ( 65535 << 4 ) / ( sqrt( sqrt( ( 255L << 18 ) / catchRate ) ) ) );
        u8  succ = 0;
        for( u8 i = 0; i < 4; ++i ) {
            u16 rn = rand( );
            if( rn > pr ) break;
            succ++;
        }

        if( _isMockBattle ) {
            succ = 4; // Let's not make Wally sad
        }

        _battleUI.animateCapturePkmn( p_pokeball, succ );

        switch( succ ) {
        default:
        case 0:
            _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_0 ) );
            break;
        case 1:
            _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_1 ) );
            break;
        case 2:
            _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_2 ) );
            break;
        case 3:
            _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_3 ) );
            break;
        case 4:
            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_BATTLE_PKMN_CAPTURE_SUCC ),
                      wild->m_boxdata.m_name );
            _battleUI.log( buffer );
            break;
        }
        WAIT( FULL_SEC );

        if( succ == 4 ) {
            wild->m_boxdata.m_ball = BAG::itemToBall( p_pokeball );
            if( p_pokeball == I_FRIEND_BALL ) { wild->m_boxdata.m_steps = 200; }
            if( p_pokeball == I_HEAL_BALL ) { wild->heal( ); }
            return true;
        }
        return false;
    }

    void battle::handleCapture( ) {
        _field.revertTransform( field::OPPONENT_SIDE, field::PKMN_0 );
        if( _isMockBattle ) [[unlikely]] {
            // Nothing to be done here.
            return;
        }

        auto pkmn = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_0 );
        if( pkmn == nullptr ) [[unlikely]] { return; }

        u16          spid            = pkmn->getSpecies( );
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        if( !( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ spid / 8 ] & ( 1LLU << ( spid % 8 ) ) ) ) {
            SAVE::SAV.getActiveFile( ).registerCaughtPkmn( spid );
            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_PKMN_DEX_REGISTERED ),
                      FS::getDisplayName( spid ).c_str( ) );
            _battleUI.log( buffer );

            WAIT( FULL_SEC );

            DEX::dex( ).run( spid, pkmn->getForme( ), pkmn->isShiny( ), pkmn->isFemale( ) );
        }
        _battleUI.handleCapture( _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_0 ) );

        // Check whether the pkmn fits in the team
        if( _playerTeamSize < 6 ) {
            std::memcpy( &_playerTeam[ _playerTeamSize ], pkmn, sizeof( pokemon ) );
            _playerPkmnOrigLevel[ _playerTeamSize++ ] = pkmn->m_level;
        } else {
            u8 oldbx = SAVE::SAV.getActiveFile( ).m_curBox;
            u8 nb    = SAVE::SAV.getActiveFile( ).storePkmn( *pkmn );
            if( nb != u8( -1 ) ) {
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_PKMN_SENT_TO_STORAGE ),
                          pkmn->m_boxdata.m_name );
                _battleUI.log( buffer );

                if( oldbx != nb ) {
                    snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_STORAGE_BOX_FULL ),
                              SAVE::SAV.getActiveFile( ).m_storedPokemon[ oldbx ].m_name );
                    _battleUI.log( buffer );
                }
                sprintf( buffer, GET_STRING( IO::STR_UI_STORAGE_BOX_PICKED ),
                         pkmn->m_boxdata.m_name,
                         SAVE::SAV.getActiveFile( ).m_storedPokemon[ nb ].m_name );
                _battleUI.log( buffer );
            } else {
                _battleUI.log( GET_STRING( IO::STR_UI_STORAGE_ALL_BOXES_FULL ) );
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_PKMN_RELEASED ),
                          pkmn->m_boxdata.m_name );
                _battleUI.log( buffer );
            }
            WAIT( DOUBLE_SEC );
        }
    }

    void battle::megaEvolve( fieldPosition p_position ) {
        _field.megaEvolve( &_battleUI, p_position.first, p_position.second );
        if( !p_position.first ) { _policy.m_allowMegaEvolution = false; }
    }

    void battle::checkAndRefillBattleSpots( slot::status p_checkType ) {
        for( u8 i = 0; i < 2; ++i )
            for( u8 j = 0; j < getBattlingPKMNCount( _policy.m_mode ); ++j ) {
                if( _field.getSlotStatus( i, j ) == p_checkType ) {
                    if( i && !_isWildBattle ) {
                        // AI chooses a next pkmn
                        auto nxt = getNextAIPokemon( );
                        if( nxt != 255 ) { switchPokemon( { i, j }, nxt ); }
                    } else if( !i ) {
                        // Check if the player has something to send out
                        bool good = false;
                        for( u8 k = j + 1; k < _playerTeamSize; ++k ) {
                            if( _playerTeam[ k ].canBattle( ) ) { good = true; }
                        }
                        if( !good ) { continue; }

                        // Make the player choose a pokemon

                        STS::partyScreen pt = STS::partyScreen(
                            _playerTeam, _playerTeamSize, false, false, false, 1, false, false,
                            false, false, getBattlingPKMNCount( _policy.m_mode ), j );

                        auto res = pt.run( j );

                        if( res.getSelectedPkmn( ) == 255 ) [[unlikely]] {
#ifdef DESQUID
                            _battleUI.log( "Oh well, time to desquid..." );
#endif
                        }

                        _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

                        for( u8 i2 = 0; i2 < 2; ++i2 )
                            for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                                if( i == i2 && j == j2 ) { continue; }
                                _battleUI.updatePkmn( i2, j2, _field.getPkmn( i2, j2 ) );
                            }

                        switchPokemon( { i, j }, res.getSelectedPkmn( ) );
                    }
                }
            }
    }

    void battle::switchPokemon( fieldPosition p_toSwitch, u16 p_newIndex ) {
        auto oldst = _field.getSlotStatus( p_toSwitch.first, p_toSwitch.second );
        if( oldst != slot::status::FAINTED && oldst != slot::status::RECALLED ) {
            _field.recallPokemon( &_battleUI, p_toSwitch.first, p_toSwitch.second, false, true );
        }

        if( p_toSwitch.first ) {
            // opponent
            std::swap( _opponentTeam[ p_toSwitch.second ], _opponentTeam[ p_newIndex ] );
            std::swap( _yieldEXP[ p_toSwitch.second ], _yieldEXP[ p_newIndex ] );
            std::swap( _opponentPkmnPerm[ p_toSwitch.second ], _opponentPkmnPerm[ p_newIndex ] );
            _field.sendPokemon( &_battleUI, p_toSwitch.first, p_toSwitch.second,
                                &_opponentTeam[ p_toSwitch.second ] );
        } else {
            // player
            std::swap( _playerTeam[ p_toSwitch.second ], _playerTeam[ p_newIndex ] );
            std::swap( _playerPkmnPerm[ p_toSwitch.second ], _playerPkmnPerm[ p_newIndex ] );
            _field.sendPokemon( &_battleUI, p_toSwitch.first, p_toSwitch.second,
                                &_playerTeam[ p_toSwitch.second ] );
        }
    }

    void battle::useItem( fieldPosition p_target, u16 p_item ) {
        auto         idata           = FS::getItemData( p_item );
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        auto         pkmn = _field.getPkmn( p_target.first, p_target.second );

        if( p_target.first ) {
            // opponent item
            bool itemfound = false;
            for( u8 i = 0; i < 5; ++i ) {
                if( _opponent.m_data.m_items[ i ] == p_item ) {
                    _opponent.m_data.m_items[ i ] = 0;
                    itemfound                     = true;
                    break;
                }
            }
            if( !itemfound ) [[unlikely]] {
                // cannot use non-existing item . . .
                return;
            }

            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_BATTLE_TRAINER_USED_ITEM ),
                      FS::getTrainerClassName( _opponent.getClass( ) ).c_str( ),
                      _opponent.m_strings.m_name, FS::getItemName( p_item ).c_str( ) );
            _battleUI.log( buffer );
        } else {
            // player item
            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_BATTLE_PLAYER_USED_ITEM ),
                      FS::getItemName( p_item ) );
            _battleUI.log( buffer );
        }

        auto   volst   = _field.getVolatileStatus( p_target.first, p_target.second );
        bool   remitem = true;
        bool   boost   = false;
        boosts bs      = boosts( );
        switch( p_item ) {
        // Due to animations, slightly different effects; need to handle medicine here
        // again
        case I_POTION: _field.healPokemon( &_battleUI, p_target.first, p_target.second, 30 ); break;
        case I_FRESH_WATER:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 40 );
            break;
        case I_ENERGY_POWDER:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 50 );
            break;
        case I_SUPER_POTION:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 60 );
            break;
        case I_SODA_POP:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 70 );
            break;
        case I_LEMONADE:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 90 );
            break;
        case I_MOOMOO_MILK:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 100 );
            break;
        case I_HYPER_POTION:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 120 );
            break;
        case I_ENERGY_ROOT:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second, 200 );
            break;
        case I_MAX_POTION:
            _field.healPokemon( &_battleUI, p_target.first, p_target.second,
                                pkmn->m_stats.m_maxHP );
            break;
        case I_FULL_RESTORE:
            _field.removeStatusCondition( p_target.first, p_target.second );
            _field.healPokemon( &_battleUI, p_target.first, p_target.second,
                                pkmn->m_stats.m_maxHP );
            break;
        case I_HEAL_POWDER:
        case I_FULL_HEAL:
            _field.removeStatusCondition( p_target.first, p_target.second );
            _battleUI.updatePkmnStats( p_target.first, p_target.second, pkmn );
            break;

        case I_X_ATTACK:
            bs.setBoost( ATK, 2 );
            boost = true;
            break;
        case I_X_DEFENSE:
            bs.setBoost( DEF, 2 );
            boost = true;

            break;
        case I_X_SPEED:
            bs.setBoost( SPEED, 2 );
            boost = true;
            break;
        case I_X_ACCURACY:
            bs.setBoost( ACCURACY, 2 );
            boost = true;

            break;
        case I_X_SP_ATK:
            bs.setBoost( SATK, 2 );
            boost = true;
            break;
        case I_X_SP_DEF:
            bs.setBoost( SDEF, 2 );
            boost = true;
            break;
            [[unlikely]] case I_NION_BERRY : {
                bs = _field.getBoosts( p_target.first, p_target.second );
                if( bs.negative( ) != boosts( ) ) {
                    bs    = bs.negative( ).invert( );
                    boost = true;
                } else {
                    _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
                }
                break;
            }

        case I_BLUE_FLUTE:
            remitem = false;
            if( _field.hasStatusCondition( p_target.first, p_target.second, SLEEP ) ) {
                _field.removeStatusCondition( p_target.first, p_target.second );
                _battleUI.updatePkmnStats( p_target.first, p_target.second, pkmn );
            } else {
                _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
            }
            break;

        case I_GUARD_SPEC:
            if( !_field.addSideCondition( &_battleUI, p_target.first, SC_MIST, 5 ) ) {
                _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
            }
            break;
        case I_DIRE_HIT:
            if( !_field.addVolatileStatus( &_battleUI, p_target.first, p_target.second,
                                           VS_FOCUSENERGY, 255 ) ) {
                _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
            }
            break;
        case I_YELLOW_FLUTE:
            remitem = false;
            [[fallthrough]];
            [[unlikely]] case I_RIE_BERRY : case I_PERSIM_BERRY : if( volst & VS_CONFUSION ) {
                _field.removeVolatileStatus( &_battleUI, p_target.first, p_target.second,
                                             VS_CONFUSION );
                auto fmt = std::string( GET_STRING( IO::STR_UI_BATTLE_CONFUSION_HEALED ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          _battleUI.getPkmnName( pkmn, p_target.first ).c_str( ) );
                _battleUI.log( buffer );
            }
            else {
                _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
            }
            break;

        case I_RED_FLUTE:
            remitem = false;
            [[fallthrough]];
            [[unlikely]] case I_GARC_BERRY : if( volst & VS_ATTRACT ) {
                _field.removeVolatileStatus( &_battleUI, p_target.first, p_target.second,
                                             VS_ATTRACT );
            }
            else {
                _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
            }
            break;

        default: break;
        }

        if( boost ) {
            auto res = _field.addBoosts( p_target.first, p_target.second, bs );
            if( res != boosts( ) ) {
                _battleUI.logBoosts( pkmn, p_target.first, p_target.second, bs, res );
            } else {
                _battleUI.log( GET_STRING( IO::STR_UI_BATTLE_NOTHING_HAPPENED ) );
            }
        }
        if( remitem && !p_target.first ) {
            SAVE::SAV.getActiveFile( ).m_bag.erase( BAG::toBagType( idata.m_itemType ), p_item, 1 );
        }
    }

    void battle::sortPkmn( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8*      perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8       len  = p_opponent ? _opponentTeamSize : _playerTeamSize;

        if( len > 6 ) [[unlikely]] { len = 6; }

        for( u8 i = 0; i < len - 1; ++i ) {
            if( !pkmn[ 0 ].canBattle( ) ) {
                // pkmn cannot battle, move it to the end of the list.
                for( u8 j = 1; j < len - i; ++j ) {
                    std::swap( pkmn[ j - 1 ], pkmn[ j ] );
                    if( p_opponent ) { std::swap( _yieldEXP[ j - 1 ], _yieldEXP[ j ] ); }
                    std::swap( perm[ j - 1 ], perm[ j ] );
                }
            }
        }
    }

    void battle::restoreInitialOrder( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8*      perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8       len  = p_opponent ? _opponentTeamSize : _playerTeamSize;

        for( u8 i = 0; i < len - 1; ++i ) {
            // In each step, move the i-th pkmn to the correct place.
            while( perm[ i ] != i ) {
                std::swap( pkmn[ i ], pkmn[ perm[ i ] ] );
                if( p_opponent ) { std::swap( _yieldEXP[ i ], _yieldEXP[ perm[ i ] ] ); }
                std::swap( perm[ i ], perm[ perm[ i ] ] );
            }
        }
    }

    void battle::resetBattleTransformations( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8       len  = p_opponent ? _opponentTeamSize : _playerTeamSize;
        for( u8 i = 0; i < len; ++i ) {
            pkmn[ i ].revertBattleTransform( );
            pkmn[ i ].setBattleTimeAbility( 0 );
        }
    }

    void battle::distributeEXP( ) {
        if( !_policy.m_distributeEXP ) { return; }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        for( u8 j = 0; j < getBattlingPKMNCount( _policy.m_mode ); ++j ) {
            if( _field.getSlotStatus( field::OPPONENT_SIDE, j ) == slot::status::FAINTED ) {
                if( _yieldEXP[ j ].empty( ) ) { continue; } // already distributed

                // distribute EXP
#ifdef DESQUID_MORE
                std::string lmsg = "Distributing EXP to ";
#endif
                std::vector<u8> reg = std::vector<u8>( ), share = std::vector<u8>( );
                for( u8 q2 = 0; q2 < _playerTeamSize; ++q2 ) {
                    if( _yieldEXP[ j ].count( _playerPkmnPerm[ q2 ] )
                        && _playerTeam[ q2 ].canBattle( ) ) {
#ifdef DESQUID_MORE
                        lmsg += std::string( _playerTeam[ q2 ].m_boxdata.m_name ) + " ";
#endif
                        // pkmn that get regular exp
                        reg.push_back( q2 );
                    } else if( _playerTeam[ q2 ].canBattle( )
                               && SAVE::SAV.getActiveFile( ).m_options.m_EXPShareEnabled ) {
                        // pkmn that get exp via exp share
                        share.push_back( q2 );
                    }
                }
#ifdef DESQUID_MORE
                _battleUI.log( lmsg );
#endif
                _yieldEXP[ j ].clear( );
                // base exp (before distribution to pkmn)
                u32 baseexp = _field.getPkmnData( field::OPPONENT_SIDE, j ).m_baseForme.m_expYield
                                  * _opponentTeam[ j ].m_level
                              >> 3;
                if( !_isWildBattle ) { baseexp = baseexp * 3 / 2; }
                if( reg.size( ) && share.size( ) ) { baseexp /= 2; }

                for( const std::vector<u8>& lst : { reg, share } ) {
                    if( lst.size( ) ) {
                        for( auto i : lst ) {
                            // distribute ev
                            for( u8 ev = 0; ev < 6; ++ev ) {
                                u8 m1 = ( _playerTeam[ i ].getItem( ) == I_MACHO_BRACE );
                                if( _playerTeam[ i ].m_boxdata.m_pokerus ) { m1++; }

                                u8 m2 = 0;
                                if( ev + I_POWER_BRACER - 1 == _playerTeam[ i ].getItem( ) ) {
                                    m2 = 8;
                                }
                                if( !ev && _playerTeam[ i ].getItem( ) == I_POWER_WEIGHT ) {
                                    m2 = 8;
                                }

                                _playerTeam[ i ].EVset(
                                    ev, _playerTeam[ i ].EVget( ev )
                                            + ( ( _field.getPkmnData( field::OPPONENT_SIDE, j )
                                                      .m_baseForme.m_evYield[ ev ]
                                                  + m2 )
                                                << m1 ) );
                            }

                            u32 curexp = baseexp / reg.size( );
                            if( _playerTeam[ i ].getItem( ) == I_LUCKY_EGG ) { curexp <<= 1; }
                            if( _playerTeam[ i ].isForeign( ) ) { curexp <<= 1; }

                            u8 oldlv = _playerTeam[ i ].m_level;
                            if( _playerTeam[ i ].m_level < 100 ) {
                                _playerTeam[ i ].gainExperience( curexp );
                                snprintf( buffer, TMP_BUFFER_SIZE,
                                          GET_STRING( IO::STR_UI_BATTLE_EXP_GAINED ),
                                          _playerTeam[ i ].m_boxdata.m_name, curexp );
                                _battleUI.log( buffer );
                                if( _playerTeam[ i ].m_level != oldlv ) {
                                    snprintf( buffer, TMP_BUFFER_SIZE,
                                              GET_STRING( IO::STR_UI_BATTLE_ADVANCE_LEVEL ),
                                              _playerTeam[ i ].m_boxdata.m_name,
                                              _playerTeam[ i ].m_level );
                                    _battleUI.log( buffer );
                                }
                            }
                            if( i < getBattlingPKMNCount( _policy.m_mode ) ) {
                                // update battleUI
                                _battleUI.updatePkmnStats( field::PLAYER_SIDE, i,
                                                           _field.getPkmn( field::PLAYER_SIDE, i ),
                                                           true );
                            }
                        }
                    }
                }
            }
        }
    }

    const char* battle::getMessage( u8 p_stringId ) {
        if( _isWildBattle ) { return 0; }

        switch( p_stringId ) {
        case battle::BEFORE_BATTLE: return _opponent.m_strings.m_message1;
        case battle::AFTER_BATTLE: return _opponent.m_strings.m_message2;
        default: break;
        }
        return 0;
    }
} // namespace BATTLE
