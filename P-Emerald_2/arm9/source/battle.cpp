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

#include <string>

#include "defines.h"

#include "saveGame.h"
#include "battle.h"
#include "battleUI.h"
#include "battleSlot.h"
#include "battleSide.h"
#include "battleField.h"
#include "battleTrainer.h"
#include "uio.h"

#include "pokemon.h"
#include "sound.h"


namespace BATTLE {
    std::string trainerClassNames[ 120 ] = {"Pokémon-Trainer"};

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, u16 p_opponentId,
                    u8 p_platform, u8 p_platform2, u8 p_background, battlePolicy p_policy ) {
        _playerTeam         = p_playerTeam;
        _playerTeamSize     = p_playerTeamSize;

        _opponent = getBattleTrainer( p_opponentId, CURRENT_LANGUAGE );
        for( u8 i = 0; i < _opponent.m_data.m_numPokemon; ++i ) {
            _opponentTeam[ i ] = pokemon( _opponent.m_data.m_pokemon[ i ] );
        }

        _policy = p_policy;
        _isWildBattle = false;

        _field    = field( p_policy.m_weather );
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, _policy.m_mode, false );

        _opponentRuns = false;;
    }
    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon p_opponent,
                    u8 p_platform, u8 p_platform2, u8 p_background, battlePolicy p_policy,
                    bool p_wildPkmnRuns ) {
        _playerTeam         = p_playerTeam;
        _playerTeamSize     = p_playerTeamSize;

        _opponent          = battleTrainer( );
        _opponentTeam[ 0 ] = p_opponent;
        _opponentTeamSize  = 1;

        _policy = p_policy;
        _isWildBattle = true;

        // Initialize the field with the wild pkmn
        _field    = field( p_policy.m_weather, NO_PSEUDO_WEATHER, NO_TERRAIN );
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, _policy.m_mode, true );

        _opponentRuns = p_wildPkmnRuns;
    }

    battle::battleEndReason battle::start( ) {
        if( !_opponentTeamSize || !_playerTeamSize ) { return battle::BATTLE_NONE; }

        char buffer[ 100 ];

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
                for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

                // Compute player's first pokemon's move
                moves[ field::PLAYER_SIDE ][ 0 ] = getMoveSelection( 0,
                        _policy.m_allowMegaEvolution );

                if( moves[ field::PLAYER_SIDE ][ 0 ].m_type == RUN ) {
                    moves[ field::PLAYER_SIDE ][ 0 ] = NO_OP_SELECTION;
                    moves[ field::PLAYER_SIDE ][ 1 ] = NO_OP_SELECTION;
                    playerWillRun                    = true;
                    break;
                }
                if( moves[ field::PLAYER_SIDE ][ 0 ].m_type == CAPTURE ) {
                    playerWillCatch                  = moves[ field::PLAYER_SIDE ][ 0 ].m_param;
                    moves[ field::PLAYER_SIDE ][ 0 ] = NO_OP_SELECTION;
                    moves[ field::PLAYER_SIDE ][ 1 ] = NO_OP_SELECTION;
                    break;
                }

                if( _policy.m_mode == DOUBLE ) {
                    moves[ field::PLAYER_SIDE ][ 1 ] = getMoveSelection(
                        1, _policy.m_allowMegaEvolution
                               && !moves[ field::PLAYER_SIDE ][ 0 ].m_megaEvolve );

                    // Player wishes to start over
                    if( moves[ field::PLAYER_SIDE ][ 1 ].m_type == CANCEL ) { continue; }

                    if( moves[ field::PLAYER_SIDE ][ 1 ].m_type == RUN ) {
                        moves[ field::PLAYER_SIDE ][ 0 ] = NO_OP_SELECTION;
                        moves[ field::PLAYER_SIDE ][ 1 ] = NO_OP_SELECTION;
                        playerWillRun                    = true;
                    }
                    if( moves[ field::PLAYER_SIDE ][ 1 ].m_type == CAPTURE ) {
                        playerWillCatch                  = moves[ field::PLAYER_SIDE ][ 1 ].m_param;
                        moves[ field::PLAYER_SIDE ][ 0 ] = NO_OP_SELECTION;
                        moves[ field::PLAYER_SIDE ][ 1 ] = NO_OP_SELECTION;
                    }
                }
                break;
            }

            // Compute AI's moves

            if( _isWildBattle && _opponentRuns && _field.canSwitchOut( true, 0 ) ) {
                SOUND::playSoundEffect( SFX_BATTLE_ESCAPE );
                snprintf( buffer, 99, GET_STRING( 171 ), _opponentTeam[ 0 ].m_boxdata.m_name );
                _battleUI.log( std::string( buffer ) );
                for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }
                endBattle( battleEnd = BATTLE_RUN );
                return battleEnd;
            }

            _battleUI.resetLog( );


            for( u8 i = 0; i < ( _policy.m_mode == SINGLE ? 1 : 0 ); ++i ) {
                moves[ field::OPPONENT_SIDE ][ i ] = getAIMove( i );
            }

            if( ( playerWillRun && playerRuns( ) )
                    || ( moves[ field::PLAYER_SIDE ][ 0 ].m_type == USE_ITEM
                        && (
                            moves[ field::PLAYER_SIDE ][ 0 ].m_param == I_POKE_DOLL
                            || moves[ field::PLAYER_SIDE ][ 0 ].m_param == I_FLUFFY_TAIL
                            || moves[ field::PLAYER_SIDE ][ 0 ].m_param == I_POKE_TOY ) ) )
                [[likely]] {
                SOUND::playSoundEffect( SFX_BATTLE_ESCAPE );
                _battleUI.log( std::string( GET_STRING( 163 ) ) );
                for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }
                endBattle( battleEnd = BATTLE_RUN );
                return battleEnd;
            } else if ( playerWillRun ) [[unlikely]] {
                _battleUI.log( std::string( GET_STRING( 164 ) ) );
                for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }
            }

            if( playerWillCatch && playerCaptures( playerWillCatch ) ) {
                endBattle( battleEnd = BATTLE_CAPTURE );
                return battleEnd;
            }

            // Sort moves
            std::vector<battleMoveSelection> selection = std::vector<battleMoveSelection>( );
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 0; j <= u8( _policy.m_mode ); ++j ) {
                    // Check for pursuit
                    if( moves[ i ][ j ].m_type == ATTACK
                            && moves[ i ][ j ].m_param == M_PURSUIT ) [[unlikely]] {
                        if( moves[ moves[ i ][ j ].m_target.first ]
                            [ moves[ i ][ j ].m_target.second ].m_type == SWITCH ) [[unlikely]] {
                            moves[ i ][ j ].m_type = SWITCH_PURSUIT;
                        }
                    }
                    selection.push_back( moves[ i ][ j ] );
                }
#ifdef DESQUID
/*
            for( u8 i = 0; i < selection.size( ); ++i ) {
                _battleUI.log( "Move sel " + std::to_string( i )
                        + " tp: " + std::to_string( u16( selection[ i ].m_type ) )
                        + " param: " + std::to_string( selection[ i ].m_param )
                        + " " + MOVE::getMoveName( selection[ i ].m_param )
                        + " user: " + std::to_string( selection[ i ].m_user.first )
                        + ", " + std::to_string( selection[ i ].m_user.second ) );
//                        + " tg: " + std::to_string( selection[ i ].m_target.first )
//                        + ", " + std::to_string( selection[ i ].m_target.second ) );
            }
            */
#endif

            auto sortedMoves = _field.computeSortedBattleMoves( &_battleUI, selection );

#ifdef DESQUID
            /*
            _battleUI.log( "Sorting done" );
            for( u8 i = 0; i < sortedMoves.size( ); ++i ) {
                _battleUI.log( "Move sel " + std::to_string( i )
                        + " tp: " + std::to_string( u16( sortedMoves[ i ].m_type ) )
                        + " param: " + std::to_string( sortedMoves[ i ].m_param )
                        + " user: " + std::to_string( sortedMoves[ i ].m_user.first )
                        + ", " + std::to_string( sortedMoves[ i ].m_user.second )
                        + " spd: " + std::to_string( sortedMoves[ i ].m_userSpeed )
                        + " prio: " + std::to_string( sortedMoves[ i ].m_priority )
//                        + " tg: " + std::to_string( sortedMoves[ i ].m_target.first )
//                        + ", " + std::to_string( sortedMoves[ i ].m_target.second )
                        );
            }
            for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }
            */
#endif

            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                if( sortedMoves[ i ].m_megaEvolve ) {
                    megaEvolve( sortedMoves[ i ].m_user );
                }
            }

            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                for( u8 j = 0; j < 30; ++j ) { swiWaitForVBlank( ); }
                if( sortedMoves[ i ].m_type == battleMoveType::MESSAGE_ITEM ) [[unlikely]] {
                    auto itmnm = ITEM::getItemName( sortedMoves[ i ].m_param );
                    snprintf( buffer, 99, GET_STRING( 169 ), itmnm.c_str( ),
                              _battleUI.getPkmnName(
                                  _field.getPkmn( sortedMoves[ i ].m_user.first,
                                  sortedMoves[ i ].m_user.second ),
                                  sortedMoves[ i ].m_user.first, false ).c_str( ),
                              itmnm.c_str( ) );
                    _battleUI.log( std::string( buffer ) );
                }

                if( sortedMoves[ i ].m_type == battleMoveType::MESSAGE_MOVE ) [[unlikely]] {
                    switch( sortedMoves[ i ].m_param ) {
                        case M_SHELL_TRAP:
                            snprintf( buffer, 99, GET_STRING( 269 ),
                              _battleUI.getPkmnName(
                                  _field.getPkmn( sortedMoves[ i ].m_user.first,
                                  sortedMoves[ i ].m_user.second ),
                                  sortedMoves[ i ].m_user.first, false ).c_str( ) );
                            _battleUI.log( buffer );
                            break;
                        case M_FOCUS_PUNCH:
                            snprintf( buffer, 99, GET_STRING( 270 ),
                              _battleUI.getPkmnName(
                                  _field.getPkmn( sortedMoves[ i ].m_user.first,
                                  sortedMoves[ i ].m_user.second ),
                                  sortedMoves[ i ].m_user.first, false ).c_str( ) );
                            _battleUI.log( buffer );
                            break;
                        case M_BEAK_BLAST:
                            snprintf( buffer, 99, GET_STRING( 271 ),
                              _battleUI.getPkmnName(
                                  _field.getPkmn( sortedMoves[ i ].m_user.first,
                                  sortedMoves[ i ].m_user.second ),
                                  sortedMoves[ i ].m_user.first, false ).c_str( ) );
                            _battleUI.log( buffer );
                            break;
                        default:
                            break;
                    }
                }

                if( sortedMoves[ i ].m_type == battleMoveType::ATTACK ) [[likely]] {
                    std::vector<battleMove> targets    = std::vector<battleMove>( );
                    std::vector<battleMove> targetedBy = std::vector<battleMove>( );

                    for( auto m : sortedMoves ) {
                        for( auto t : sortedMoves[ i ].m_target )
                            if( t == m.m_user ) { targets.push_back( m ); }
                        for( auto t : m.m_target )
                            if( t == sortedMoves[ i ].m_user ) { targetedBy.push_back( m ); }
                    }

                    _field.executeBattleMove( &_battleUI, sortedMoves[ i ], targets, targetedBy );

                    checkAndRefillBattleSpots( slot::status::RECALLED );
                }

                if( sortedMoves[ i ].m_type == battleMoveType::SWITCH ) {
                    switchPokemon( sortedMoves[ i ].m_user, sortedMoves[ i ].m_param );
                }

                if( sortedMoves[ i ].m_type == battleMoveType::USE_ITEM ) {
                    useItem( sortedMoves[ i ].m_user, sortedMoves[ i ].m_param );
                }

                if( endConditionHit( battleEnd ) ) {
                    endBattle( battleEnd );
                    return battleEnd;
                }
            }
            _field.age( &_battleUI );

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

        _battleUI.init( );

        for( u8 i = 0; i < 6; ++i ) {
            _opponentPkmnPerm[ i ] = i;
            _playerPkmnPerm[ i ] = i;
        }

        sortPkmn( true );
        sortPkmn( false );

        if( _isWildBattle ) {
            _battleUI.startWildBattle( &_opponentTeam[ 0 ] );
            _field.setSlot( true, 0, &_opponentTeam[ 0 ] );
        } else {
            _battleUI.startTrainerBattle( &_opponent );
        }

        for( u8 i = u8( _isWildBattle ); i < 2; ++i )
            for( u8 j = 0; j <= u8( _policy.m_mode ); ++j ) {
                _battleUI.sendOutPkmn( !i, j, ( i ? &_playerTeam[ j ] : &_opponentTeam[ j ] ) );
                _field.setSlot( !i, j, ( i ? &_playerTeam[ j ] : &_opponentTeam[ j ] ) );
            }

        _field.init( &_battleUI );

        for( u8 i = 0; i < 2; ++i )
            for( u8 j = 0; j <= u8( _policy.m_mode ); ++j ) {
                _field.checkOnSendOut( &_battleUI, !i, j );
            }
    }

    battleMoveSelection battle::chooseTarget( const battleMoveSelection& p_move ) {
        auto res = p_move;
        res.m_moveData = MOVE::getMoveData( res.m_param );

        if( _policy.m_mode == SINGLE ) {
            switch( res.m_moveData.m_target ) {
                case MOVE::ANY:
                case MOVE::ANY_FOE:
                case MOVE::ALL_FOES_AND_ALLY:
                case MOVE::ALL_FOES:
                case MOVE::RANDOM:
                    res.m_target = { true, 0 };
                    return res;
                case MOVE::SELF:
                case MOVE::ALLY_OR_SELF:
                case MOVE::ALL_ALLIES:
                    res.m_target = { false, 0 };
                    return res;
                default:
                    res.m_target = { 255, 255 };
                    return res;
            }
        }

        // User needs to choose / confirm target (TODO)

        res.m_type = CANCEL;
        return res;
    }

    battleMoveSelection battle::chooseAttack( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user = {field::PLAYER_SIDE, p_slot};
        res.m_type = ATTACK;

        bool mega = _field.canMegaEvolve( false, p_slot ) && p_allowMegaEvolution;

        bool canUse[ 4 ], strgl = false;
        for( u8 i = 0; i < 4; ++i ) {
            canUse[ i ] = _field.canSelectMove( false, p_slot, i );
            strgl = strgl || canUse[ i ];
        }

        if( !strgl ) {
            // pkmn will struggle
            res.m_param = M_STRUGGLE;
            return chooseTarget( res );
        }

        _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse, mega );
        u8 curSel = 0;
        _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse, mega,
                                       curSel, res.m_megaEvolve );

        cooldown     = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                res.m_type = CANCEL;
                return res;
            }
            if( pressed & KEY_A ) {
                if( curSel < 4 ) { // Choos attack curSel
                    if( canUse[ curSel ] ) {
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        res.m_param = _field.getPkmn( false, p_slot )->getMove( curSel );
                        auto tmp = chooseTarget( res );

                        if( tmp.m_type != CANCEL ) { return tmp; }
                        else {
                            res.m_param = 0;
                        }
                    }
                } else if( curSel == 4 ) { // Cancel
                    SOUND::playSoundEffect( SFX_CANCEL );
                    res.m_type = CANCEL;
                    return res;
                } else if ( mega && curSel == 5 ) { // Toggle Mega Evolution
                    res.m_megaEvolve = !res.m_megaEvolve;
                    _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse,
                                                   mega, curSel, res.m_megaEvolve );
                }

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) || GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel < 4 && _field.getPkmn( false, p_slot )->getMove( curSel ^ 1 ) ) {
                    curSel ^= 1;
                } else if( curSel == 5 ) { curSel = 4; }
                else if( curSel == 4 && mega) {
                    curSel = 5;
                } else {
                    cooldown = COOLDOWN_COUNT;
                    continue;
                }

                _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse,
                                               mega, curSel, res.m_megaEvolve );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel + 2 < 4 && _field.getPkmn( false, p_slot )->getMove( curSel + 2 ) ) {
                    curSel += 2;
                } else if( curSel < 4 ) { curSel = 4; }
                else if( curSel == 4 ) { curSel = 0; }
                else {
                    cooldown = COOLDOWN_COUNT;
                    continue;
                }

                _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse,
                                               mega, curSel, res.m_megaEvolve );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel >= 2 && curSel < 4 ) { curSel -= 2; }
                else if( curSel == 4 ) {
                    if( _field.getPkmn( false, p_slot )->getMove( 2 ) ) {
                        curSel = 2;
                    } else {
                        curSel = 0;
                    }
                } else if( curSel < 2 ) { curSel = 4; }
                else {
                    cooldown = COOLDOWN_COUNT;
                    continue;
                }

                _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse,
                                               mega, curSel, res.m_megaEvolve );

                cooldown = COOLDOWN_COUNT;

            }
            swiWaitForVBlank( );
        }

        return res;
    }

    battleMoveSelection battle::getMoveSelection( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user = {field::PLAYER_SIDE, p_slot};

        if( !_field.canSelectMove( false, p_slot ) ) {
            // pkmn is hibernating / charging
            return _field.getStoredMove( false, p_slot );
        }

        _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot );
        u8 curSel = 0;
        _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

        cooldown     = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( p_slot && ( pressed & KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                res.m_type = CANCEL;
                return res;
            }
            if( pressed & KEY_A ) {
                switch( curSel ) {
                    case 0: // Choose attack
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        res = chooseAttack( p_slot, p_allowMegaEvolution );
                        if( res.m_type != CANCEL ) {
                            return res;
                        }
                        break;
                    case 1: // Choose pkmn
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        // TODO
                        break;
                    case 2: // Run / Cancel
                        if( _isWildBattle ) {
                            SOUND::playSoundEffect( SFX_CHOOSE );
                            res.m_type = RUN;
                        } else if( p_slot ) {
                            SOUND::playSoundEffect( SFX_CANCEL );
                            res.m_type = CANCEL;
                        }
                        return res;
                    case 3: // Choose item
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        // TODO
                        break;
                    default:
                        break;
                }
                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot );
                curSel = 0;
                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ),
                        p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) { curSel = 3; }
                else if( curSel < 3 ) { curSel++; }
                else { curSel = 0; }

                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) { curSel = 1; }
                else { curSel--; }

                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 && ( p_slot || _isWildBattle ) ) { curSel = 2; }
                else { curSel = 0; }

                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }

        return res;
    }

    battleMoveSelection battle::getAIMove( u8 p_slot ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user = { true, p_slot };
        switch( _policy.m_aiLevel ) {
            default:
            [[likely]] case 0: { // Wild pkmn
                // Pick a random move
                res.m_type = ATTACK;
                bool canUse[ 4 ], str = false;
                for( u8 i = 0; i < 4; ++i ) {
                    canUse[ i ] = _field.canSelectMove( true, p_slot, i );
                    str = str || canUse[ i ];
                }

                if( !str ) { // pkmn struggles
                    res.m_param = M_STRUGGLE;
                } else {
                    u8 mv = rand( ) % 4;
                    while( !canUse[ mv ] ) { mv = rand( ) % 4; }
                    res.m_param = _field.getPkmn( true, p_slot )->getMove( mv );
                }

                // Pick a random target
                auto mdata = MOVE::getMoveData( res.m_param );
                res.m_moveData = mdata;
                auto tg = mdata.m_pressureTarget != MOVE::NO_TARGET
                    ? mdata.m_pressureTarget : mdata.m_target;

                bool canTarget[ 4 ];
                for( u8 i = 0; i < 4; ++i ) {
                    canTarget[ i ] =  _field.getPkmn( i < 2, i & 1 ) != nullptr;
                }
                u8 ctg = rand( ) % 2;

                switch( tg ) {
                    case MOVE::RANDOM:
                    case MOVE::ANY_FOE:
                    [[likely]] case MOVE::ANY:
                        while( !canTarget[ 2 + ctg ] ) { ctg = rand( ) & 1; }
                        res.m_target = fieldPosition( false, ctg );
                        break;
                    case MOVE::ALLY_OR_SELF:
                        while( !canTarget[ ctg ] ) { ctg = rand( ) & 1; }
                        res.m_target = fieldPosition( true, ctg );
                        break;
                    [[unlikely]] default:
                        break;
                }
                break;
            }
        }
        return res;
    }

    bool battle::endConditionHit( battle::battleEndReason& p_out ) {
        if( _isWildBattle && _field.getSlotStatus( true, 0 ) == slot::FAINTED ) {
            // distribute exp
            // TODO
            p_out = battleEndReason::BATTLE_PLAYER_WON;
            return true;
        }
        if( _isWildBattle && _field.getSlotStatus( true, 0 ) == slot::RECALLED ) {
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
            if( _playerTeam[ _playerPkmnPerm[ i ] ].canBattle( ) ) {
                pkmnCnt++;
            }
        }
        if( !pkmnCnt ) {
            p_out = battleEndReason::BATTLE_OPPONENT_WON;
            return true;
        }

        // OPPONENT
        pkmnCnt = 0;
        for( u8 i = 0; i < _opponentTeamSize; ++i ) {
            if( _opponentTeam[ _opponentPkmnPerm[ i ] ].canBattle( ) ) {
                pkmnCnt++;
            }
        }
        if( !pkmnCnt ) {
            p_out = battleEndReason::BATTLE_PLAYER_WON;
            return true;
        }

        return false;
    }

    void battle::endBattle( battle::battleEndReason p_battleEndReason ) {
        // TODO
        (void) p_battleEndReason;

        restoreInitialOrder( false );
        resetBattleTransformations( false );

        _battleUI.deinit( );
        SOUND::deinitBattleSound( );
    }

    bool battle::playerRuns( ) {
        auto p1 = _field.getPkmn( false, 0 );
        if( p1 == nullptr ) { return true; }

        if( p1->getItem( ) == I_SMOKE_BALL ) {
            return true;
        }

        if( !_field.suppressesAbilities( ) ) {
            if( p1->getAbility( ) == A_RUN_AWAY ) {
                return true;
            }
        }

        u16 ownSpeed = _field.getStat( false, 0, SPEED );
        u16 oppSpeed = _field.getStat( true, 0, SPEED );

        if( ownSpeed > oppSpeed || oppSpeed == 0 ) { return true; }
        if( !_field.canSwitchOut( false, 0 ) ) { return false; }

        return ( ownSpeed + ( rand( ) % oppSpeed ) >= oppSpeed );
    }

    bool battle::playerCaptures( u16 p_pokeball ) {
        // TODO
        (void) p_pokeball;

        return false;
    }

    void battle::megaEvolve( fieldPosition p_position ) {
        _field.megaEvolve( &_battleUI, p_position.first, p_position.second );
        _policy.m_allowMegaEvolution = false;
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

    void battle::sortPkmn( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8* perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8 len = p_opponent ? _opponentTeamSize : _playerTeamSize;

        for( u8 i = 0; i < len - 1; ++i ) {
            if( !pkmn[ i ].canBattle( ) ) {
                // pkmn cannot battle, move it to the end of the list.
                for( u8 j = i + 1; j < len; ++j ) {
                    std::swap( pkmn[ j - 1 ], pkmn[ j ] );
                    std::swap( perm[ j - 1 ], perm[ j ] );
                }
            }
        }
    }

    void battle::restoreInitialOrder( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8* perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8 len = p_opponent ? _opponentTeamSize : _playerTeamSize;

        for( u8 i = 0; i < len - 1; ++i ) {
            // In each step, move the i-th pkmn to the correct place.
            while( perm[ i ] != i ) {
                std::swap( pkmn[ i ], pkmn[ perm[ i ] ] );
                std::swap( perm[ i ], perm[ perm[ i ] ] );
            }
        }
    }

    void battle::resetBattleTransformations( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8 len = p_opponent ? _opponentTeamSize : _playerTeamSize;
        for( u8 i = 0; i < len; ++i ) {
            pkmn[ i ].revertBattleTransform( );
            pkmn[ i ].setBattleTimeAbility( 0 );
        }
    }
} // namespace BATTLE
