/*
Pokémon neo
------------------------------

file        : battle.cpp
author      : Philip Wellnitz
description :

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
#include "io/message.h"
#include "io/simpleWidget.h"
#include "io/strings.h"
#include "io/util.h"
#include "map/mapDrawer.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "sts/partyScreen.h"

namespace BATTLE {
    constexpr u8 FACILITY_BG       = 32;
    constexpr u8 FACILITY_PLATFORM = 10;

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, const battleTrainer& p_opponentId,
                    battlePolicy p_policy )
        : _field{ this, p_policy.m_mode, false, p_policy.m_weather } {
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

        _battleUI = battleUI( _opponent.m_data.m_battlePlat1, _opponent.m_data.m_battlePlat2,
                              _opponent.m_data.m_battleBG, _policy.m_mode, false,
                              _policy.m_distributeEXP );

        _opponentRuns = false;

        _opponentTeamSize = _opponent.m_data.m_numPokemon;
        for( u8 i = 0; i < _opponentTeamSize; ++i ) {
            _opponentTeam[ i ] = pokemon( _opponent.m_data.m_pokemon[ i ] );
            _yieldEXP[ i ]     = std::set<u8>( );
        }
        adjustAIDifficulty( );
    }

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, const battleTrainer& p_opponent,
                    const battleTrainer& p_opponent2, battlePolicy p_policy )
        : _field{ this, p_policy.m_mode, false, p_policy.m_weather } {
        _playerTeam     = p_playerTeam;
        _playerTeamSize = p_playerTeamSize;

        _opponent      = p_opponent;
        _opponent2     = p_opponent2;
        _isMultiBattle = true;

        _policy       = p_policy;
        _isMockBattle = false;
        _isWildBattle = false;

        _AILevel  = std::max( _opponent.m_data.m_AILevel, _opponent2.m_data.m_AILevel );
        _battleUI = battleUI( _opponent.m_data.m_battlePlat1, _opponent.m_data.m_battlePlat2,
                              _opponent.m_data.m_battleBG, _policy.m_mode, false,
                              _policy.m_distributeEXP );

        _opponentRuns = false;
        u8 nA         = std::min<u8>( u8( _opponent.m_data.m_numPokemon ), SAVE::NUM_PARTY_SLOTS );
        u8 nB         = std::min<u8>( u8( _opponent2.m_data.m_numPokemon ), SAVE::NUM_PARTY_SLOTS );
        _opponentTeamSize = u8( 2 * std::max( nA, nB ) );
        for( u8 i = 0; i < MAX_OPPONENT_TEAM; ++i ) {
            memset( &_opponentTeam[ i ], 0, sizeof( pokemon ) );
            _yieldEXP[ i ] = std::set<u8>( );
        }
        for( u8 i = 0; i < nA; ++i ) {
            _opponentTeam[ getTeamIndex( OWNER_A, i, true ) ]
                = pokemon( _opponent.m_data.m_pokemon[ i ] );
        }
        for( u8 i = 0; i < nB; ++i ) {
            _opponentTeam[ getTeamIndex( OWNER_B, i, true ) ]
                = pokemon( _opponent2.m_data.m_pokemon[ i ] );
        }
        adjustAIDifficulty( );
    }

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, u16 p_opponentId,
                    battlePolicy p_policy )
        : battle( p_playerTeam, p_playerTeamSize, FS::getBattleTrainer( p_opponentId ), p_policy ) {
    }
    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, u16 p_opponentId, u16 p_opponentId2,
                    battlePolicy p_policy )
        : battle( p_playerTeam, p_playerTeamSize, FS::getBattleTrainer( p_opponentId ),
                  FS::getBattleTrainer( p_opponentId2 ), p_policy ) {
    }

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, const bfTrainer& p_opponent,
                    const bfPokemon p_opponentTeam[ 6 ], u8 p_opponentTeamSize, u8 p_level, u8 p_iv,
                    battlePolicy p_policy )
        : _field{ this, p_policy.m_mode, false, p_policy.m_weather } {
        _playerTeam     = p_playerTeam;
        _playerTeamSize = p_playerTeamSize;

        _opponent = battleTrainer{ };

        snprintf( _opponent.m_strings.m_name, trainerStrings::NAME_LENGTH - 1, "%s",
                  FS::getBFTrainerName( p_opponent.m_trainerNameIdx ) );
        snprintf( _opponent.m_strings.m_message1, trainerStrings::MSG_LENGTH - 1, "%s",
                  p_opponent.m_beforeBattle.construct( ).c_str( ) );
        snprintf( _opponent.m_strings.m_message2, trainerStrings::MSG_LENGTH - 1, "%s",
                  p_opponent.m_onLoseAgainstPlayer.construct( ).c_str( ) );

        _opponent.m_data.m_trainerClass = p_opponent.m_trainerClass;
        _opponent.m_data.m_trainerBG    = p_opponent.m_trainerBG;

        _policy = p_policy;

        if( _policy.m_mode == BM_MOCK ) {
            _policy.m_mode = BM_SINGLE;
            _isMockBattle  = true;
        } else {
            _isMockBattle = false;
        }
        _isWildBattle = false;
        _AILevel = _opponent.m_data.m_AILevel = p_policy.m_aiLevel;

        _battleUI = battleUI( FACILITY_PLATFORM, FACILITY_PLATFORM, FACILITY_BG, _policy.m_mode,
                              false, _policy.m_distributeEXP );

        _opponentRuns = false;

        _opponentTeamSize = _opponent.m_data.m_numPokemon = p_opponentTeamSize;

        for( u8 i = 0; i < _opponentTeamSize; ++i ) {
            _opponentTeam[ i ] = pokemon( p_opponentTeam[ i ], p_level, p_iv );
            _yieldEXP[ i ]     = std::set<u8>( );
        }
    }

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon p_opponent, u8 p_platform,
                    u8 p_platform2, u8 p_background, battlePolicy p_policy, bool p_wildPkmnRuns )
        : _field{ this, p_policy.m_mode, false, p_policy.m_weather, PW_NONE, TR_NONE } {
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
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, _policy.m_mode, true, _policy.m_distributeEXP );

        _opponentRuns = p_wildPkmnRuns;
    }

    void battle::adjustAIDifficulty( ) {
        auto playermaxlv = 0;
        for( auto i = 0; i < _playerTeamSize; ++i ) {
            if( _playerTeam[ i ].getLevel( ) > playermaxlv ) {
                playermaxlv = _playerTeam[ i ].getLevel( );
            }
        }
        auto oppmaxlv = 0;
        for( u8 i = 0; i < _opponentTeamSize; ++i ) {
            if( _opponentTeam[ i ].getLevel( ) > oppmaxlv ) {
                oppmaxlv = _opponentTeam[ i ].getLevel( );
            }
        }

        switch( SAVE::CURRENT_FILE->m_options.getDifficulty( ) ) {
        case 0: {
            // easy
            auto targetlv = std::max( int( MIN_OPP_LEVEL ),
                                      playermaxlv - DIFF_EASY_LEVEL_DEC - DIFF_HARD_LEVEL_INC );
            if( targetlv > MAX_PKMN_LEVEL ) { targetlv = MAX_PKMN_LEVEL; }
            auto leveladjust = 0;
            if( oppmaxlv < targetlv ) { leveladjust = targetlv - oppmaxlv; }

            for( u8 i = 0; i < _opponentTeamSize; ++i ) {
                _opponentTeam[ i ].setLevel( _opponentTeam[ i ].getLevel( ) + leveladjust );
                _opponentTeam[ i ].m_boxdata.m_moves[ 3 ] = 0;
            }
            break;
        }
        case 3:
        default: {
            // normal
            auto targetlv = std::max( int( MIN_OPP_LEVEL ), playermaxlv - DIFF_HARD_LEVEL_INC );
            if( targetlv > MAX_PKMN_LEVEL ) { targetlv = MAX_PKMN_LEVEL; }
            auto leveladjust = 0;
            if( oppmaxlv < targetlv ) { leveladjust = targetlv - oppmaxlv; }

            for( u8 i = 0; i < _opponentTeamSize; ++i ) {
                _opponentTeam[ i ].setLevel( _opponentTeam[ i ].getLevel( ) + leveladjust );
            }
            break;
        }
        case 6: {
            // hard
            auto targetlv    = playermaxlv;
            auto leveladjust = 0;
            if( oppmaxlv < targetlv ) { leveladjust = targetlv - oppmaxlv; }

            for( u8 i = 0; i < _opponentTeamSize; ++i ) {
                _opponentTeam[ i ].setLevel( _opponentTeam[ i ].getLevel( ) + leveladjust );
            }
            break;
        }
        }
    }

    battle::battleEndReason battle::start( bool p_firstPkmnIsFollowing ) {
        if( !_opponentTeamSize || !_playerTeamSize ) { return battle::BATTLE_NONE; }

        _field.setTeams( _playerTeam, _playerTeamSize, _opponentTeam, _opponentTeamSize );

        ANIMATE_MAP = false;

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        swiWaitForVBlank( );
        scanKeys( );

        battleEndReason battleEnd = BATTLE_NONE;
        initBattle( p_firstPkmnIsFollowing );

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

            battleMoveSelection moves[ field::NUM_SIDES ][ side::MAX_PKMN_PER_SIDE ]
                = { { } }; // fieldPosition -> selected move

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
                    // player wants to run, cancel any selected moves
                    moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                    moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                    playerWillRun                                = true;
                    break;
                }
                if( moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_type == MT_CAPTURE ) {
                    // player wants to catch a wild pkmn, cancel any moves
                    // m_param contains the type of poke ball the player wishes to throw
                    playerWillCatch = moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param;
                    moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                    moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                    break;
                }

                if( _policy.m_mode == BM_DOUBLE ) {
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
                        // player wants to run, cancel any selected moves
                        moves[ field::PLAYER_SIDE ][ field::PKMN_0 ] = NO_OP_SELECTION;
                        moves[ field::PLAYER_SIDE ][ field::PKMN_1 ] = NO_OP_SELECTION;
                        playerWillRun                                = true;
                    }
                    if( moves[ field::PLAYER_SIDE ][ field::PKMN_1 ].m_type == MT_CAPTURE ) {
                        // player wants to catch a wild pkmn, cancel any moves
                        // m_param contains the type of poke ball the player wishes to throw
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
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_WILD_PKMN_FLED ),
                          _opponentTeam[ field::PKMN_0 ].m_boxdata.getNameS( ).c_str( ) );
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
                    SAVE::CURRENT_FILE->m_bag.erase(
                        BAG::bag::ITEMS, moves[ field::PLAYER_SIDE ][ field::PKMN_0 ].m_param, 1 );
                }

                SOUND::playSoundEffect( SFX_BATTLE_ESCAPE );
                _battleUI.log( std::string( GET_STRING( IO::STR_BATTLE_GOT_AWAY_SAFELY ) ) );
                WAIT( THREE_QUARTER_SEC );
                endBattle( battleEnd = BATTLE_RUN );
                return battleEnd;
            } else if( playerWillRun ) [[unlikely]] {
                // escape failed
                _battleUI.log( std::string( GET_STRING( IO::STR_BATTLE_COULDNT_ESCAPE ) ) );
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
                    auto fmt   = std::string( GET_STRING( IO::STR_BATTLE_USE_ITEM_TO_ACT_FIRST ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ), itmnm.c_str( ),
                              _battleUI
                                  .getPkmnName(
                                      _field.getPkmnOrDisguise( sortedMoves[ i ].m_user.first,
                                                                sortedMoves[ i ].m_user.second ),
                                      sortedMoves[ i ].m_user.first, false )
                                  .c_str( ),
                              itmnm.c_str( ) );
                    _battleUI.log( std::string( buffer ) );
                }

                // show special messages for special moves
                if( sortedMoves[ i ].m_type == MT_MESSAGE_MOVE ) [[unlikely]] {
                    auto pnm = std::string( _battleUI.getPkmnName(
                        _field.getPkmnOrDisguise( sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second ),
                        sortedMoves[ i ].m_user.first, false ) );
                    switch( sortedMoves[ i ].m_param ) {
                    case M_SHELL_TRAP:
                        snprintf( buffer, TMP_BUFFER_SIZE,
                                  GET_STRING( IO::STR_BATTLE_PREPARE_SHELL_TRAP ), pnm.c_str( ) );
                        _battleUI.log( buffer );
                        _field.addVolatileStatus( &_battleUI, sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second, VS_SHELLTRAP, 1 );
                        break;
                    case M_FOCUS_PUNCH:
                        snprintf( buffer, TMP_BUFFER_SIZE,
                                  GET_STRING( IO::STR_BATTLE_PREPARE_FOCUS_PUNCH ), pnm.c_str( ) );
                        _battleUI.log( buffer );
                        _field.addVolatileStatus( &_battleUI, sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second, VS_FOCUSPUNCH,
                                                  1 );
                        break;
                    case M_BEAK_BLAST:
                        snprintf( buffer, TMP_BUFFER_SIZE,
                                  GET_STRING( IO::STR_BATTLE_PREPARE_BEAK_BLAST ), pnm.c_str( ) );
                        _field.addVolatileStatus( &_battleUI, sortedMoves[ i ].m_user.first,
                                                  sortedMoves[ i ].m_user.second, VS_BEAKBLAST, 1 );
                        _battleUI.log( buffer );
                        break;
                    case M_COUNTER:
                    case M_MIRROR_COAT:
                        _field.armCounters( sortedMoves[ i ].m_user.first,
                                            sortedMoves[ i ].m_user.second,
                                            sortedMoves[ i ].m_param == M_MIRROR_COAT );
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

                sortedMoves[ i ]
                    = battleMove{ MT_NO_OP, 0, { }, { 255, 255 }, 0, 0, 0, moveData( ), false };

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

    void battle::initBattle( bool p_followPkmn ) {
        SOUND::initBattleSound( );

        _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

        // prepare pkmn permutation; used to restore pkmn order after battle
        for( u8 i = 0; i < MAX_OPPONENT_TEAM; ++i ) { _opponentPkmnPerm[ i ] = i; }
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
            _battleUI.startTrainerBattle( &_opponent, isMultiBattle( ) ? &_opponent2 : nullptr );
        }

        // send out pkmn (for opponent only if not a wild pkmn)
        for( u8 side = u8( _isWildBattle ); side < field::NUM_SIDES; ++side ) {
            for( u8 j = 0; j < getBattlingPKMNCount( _policy.m_mode ); ++j ) {
                if( ( side && _playerTeamSize <= j ) || ( !side && _opponentTeamSize <= j ) ) {
                    _field.setSlot( !side, j, nullptr );
                    continue;
                }

                auto pkmn = ( side ? &_playerTeam[ j ] : &_opponentTeam[ j ] );
                u8   disg = _field.willDisguise( pkmn, ( side ? _playerTeam : _opponentTeam ) );
                _field.setSlot( !side, j, pkmn );

                if( disg ) {
                    _field.setSlotDisguise(
                        !side, j, ( side ? &_playerTeam[ disg ] : &_opponentTeam[ disg ] ) );
                }
                if( side && j == 0 && p_followPkmn ) {
                    _battleUI.sendOutFollowPkmn( j, _field.getPkmnOrDisguise( !side, j ) );
                } else {
                    _battleUI.sendOutPkmn( !side, j, _field.getPkmnOrDisguise( !side, j ) );
                }
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

    bool battle::canRecallPokemon( bool p_opponent ) {
        auto start = getBattlingPKMNCount( _policy.m_mode );
        auto bound = p_opponent ? _opponentTeamSize : _playerTeamSize;
        auto team  = p_opponent ? _opponentTeam : _playerTeam;

        for( auto i = start; i < bound; ++i ) {
            if( team[ i ].canBattle( ) ) { return true; }
        }
        return false;
    }
} // namespace BATTLE
