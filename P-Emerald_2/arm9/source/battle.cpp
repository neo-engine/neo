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

#include "animations.h"
#include "battle.h"
#include "battleField.h"
#include "battleSide.h"
#include "battleSlot.h"
#include "battleTrainer.h"
#include "battleUI.h"
#include "choiceBox.h"
#include "dex.h"
#include "partyScreen.h"
#include "saveGame.h"
#include "uio.h"
#include "yesNoBox.h"

#include "bagViewer.h"
#include "pokemon.h"
#include "sound.h"

namespace BATTLE {
    std::string trainerClassNames[ 120 ] = { "Pokémon-Trainer" };

    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, u16 p_opponentId, u8 p_platform,
                    u8 p_platform2, u8 p_background, battlePolicy p_policy ) {
        _playerTeam     = p_playerTeam;
        _playerTeamSize = p_playerTeamSize;

        _opponent = getBattleTrainer( p_opponentId, CURRENT_LANGUAGE );
        for( u8 i = 0; i < _opponent.m_data.m_numPokemon; ++i ) {
            _opponentTeam[ i ] = pokemon( _opponent.m_data.m_pokemon[ i ] );
            _yieldEXP[ i ]     = std::set<u8>( );
        }

        _policy       = p_policy;
        _isWildBattle = false;

        _field    = field( p_policy.m_weather );
        _battleUI = battleUI( p_platform, p_platform2 == u8( -1 ) ? p_platform : p_platform2,
                              p_background, _policy.m_mode, false );

        _opponentRuns = false;
        ;
    }
    battle::battle( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon p_opponent, u8 p_platform,
                    u8 p_platform2, u8 p_background, battlePolicy p_policy, bool p_wildPkmnRuns ) {
        _playerTeam     = p_playerTeam;
        _playerTeamSize = p_playerTeamSize;

        _opponent          = battleTrainer( );
        _opponentTeam[ 0 ] = p_opponent;
        _yieldEXP[ 0 ]     = std::set<u8>( );
        _opponentTeamSize  = 1;

        _policy       = p_policy;
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

        swiWaitForVBlank( );
        scanKeys( );

        battleEndReason battleEnd = BATTLE_NONE;
        initBattle( );

        swiWaitForVBlank( );
        scanKeys( );

        _round = 0;
        // Main battle loop
        while( !_maxRounds || ++_round < _maxRounds ) {
            // register pkmn for exp

            for( u8 i = 0; i < ( _policy.m_mode == SINGLE ? 1 : 2 ); ++i ) {
                for( u8 j = 0; j < ( _policy.m_mode == SINGLE ? 1 : 2 ); ++j ) {
                    if( _field.getPkmn( false, j )->canBattle( ) ) {
                        _yieldEXP[ i ].insert( _playerPkmnPerm[ j ] );
                    }
                }
            }

            battleMoveSelection moves[ 2 ][ 2 ] = { {} }; // fieldPosition -> selected move

            // Compute player's moves
            bool playerWillRun   = false;
            u16  playerWillCatch = 0;
            loop( ) {
                for( u8 i = 0; i < 30; ++i ) { swiWaitForVBlank( ); }

                if( moves[ field::PLAYER_SIDE ][ 0 ].m_type != NO_OP_NO_CANCEL ) {
                    // Compute player's first pokemon's move
                    moves[ field::PLAYER_SIDE ][ 0 ]
                        = getMoveSelection( 0, _policy.m_allowMegaEvolution );
                }

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

            for( u8 i = 0; i < ( _policy.m_mode == SINGLE ? 1 : 2 ); ++i ) {
                moves[ field::OPPONENT_SIDE ][ i ] = getAIMove( i );
            }

            if( ( playerWillRun && playerRuns( ) )
                || ( moves[ field::PLAYER_SIDE ][ 0 ].m_type == USE_ITEM
                     && ( moves[ field::PLAYER_SIDE ][ 0 ].m_param == I_POKE_DOLL
                          || moves[ field::PLAYER_SIDE ][ 0 ].m_param == I_FLUFFY_TAIL
                          || moves[ field::PLAYER_SIDE ][ 0 ].m_param == I_POKE_TOY ) ) )
                [[likely]] {
                    if( moves[ field::PLAYER_SIDE ][ 0 ].m_type == USE_ITEM ) {
                        SAVE::SAV.getActiveFile( ).m_bag.erase(
                            BAG::bag::ITEMS, moves[ field::PLAYER_SIDE ][ 0 ].m_param, 1 );
                    }

                    SOUND::playSoundEffect( SFX_BATTLE_ESCAPE );
                    _battleUI.log( std::string( GET_STRING( 163 ) ) );
                    for( u8 i = 0; i < 45; ++i ) { swiWaitForVBlank( ); }
                    endBattle( battleEnd = BATTLE_RUN );
                    return battleEnd;
                }
            else if( playerWillRun )
                [[unlikely]] {
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
                    if( moves[ i ][ j ].m_type == ATTACK && moves[ i ][ j ].m_param == M_PURSUIT )
                        [[unlikely]] {
                            if( moves[ moves[ i ][ j ].m_target.first ]
                                     [ moves[ i ][ j ].m_target.second ]
                                         .m_type
                                == SWITCH )
                                [[unlikely]] {
                                    moves[ i ][ j ].m_type = SWITCH_PURSUIT;
                                }
                        }
                    if( moves[ i ][ j ].m_type == NO_OP_NO_CANCEL ) {
                        moves[ i ][ j ].m_type = NO_OP;
                    }
                    selection.push_back( moves[ i ][ j ] );
                }
#ifdef DESQUID_MORE

            for( u8 i = 0; i < selection.size( ); ++i ) {
                _battleUI.log( "Move sel " + std::to_string( i )
                               + " tp: " + std::to_string( u16( selection[ i ].m_type ) )
                               + " param: " + std::to_string( selection[ i ].m_param ) + " "
                               + MOVE::getMoveName( selection[ i ].m_param )
                               + " user: " + std::to_string( selection[ i ].m_user.first ) + ", "
                               + std::to_string( selection[ i ].m_user.second )
                               + " tg: " + std::to_string( selection[ i ].m_target.first ) + ", "
                               + std::to_string( selection[ i ].m_target.second ) );
            }
#endif

            auto sortedMoves = _field.computeSortedBattleMoves( &_battleUI, selection );

#ifdef DESQUID_MORE
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
#endif

            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                if( sortedMoves[ i ].m_megaEvolve && sortedMoves[ i ].m_type == ATTACK ) {
                    megaEvolve( sortedMoves[ i ].m_user );
                } else {
                    sortedMoves[ i ].m_megaEvolve = false;
                }
            }

            for( size_t i = 0; i < sortedMoves.size( ); ++i ) {
                for( u8 j = 0; j < 30; ++j ) { swiWaitForVBlank( ); }
                if( sortedMoves[ i ].m_type == battleMoveType::MESSAGE_ITEM ) [[unlikely]] {
                        auto itmnm = ITEM::getItemName( sortedMoves[ i ].m_param );
                        snprintf(
                            buffer, 99, GET_STRING( 169 ), itmnm.c_str( ),
                            _battleUI
                                .getPkmnName( _field.getPkmn( sortedMoves[ i ].m_user.first,
                                                              sortedMoves[ i ].m_user.second ),
                                              sortedMoves[ i ].m_user.first, false )
                                .c_str( ),
                            itmnm.c_str( ) );
                        _battleUI.log( std::string( buffer ) );
                    }

                if( sortedMoves[ i ].m_type == battleMoveType::MESSAGE_MOVE ) [[unlikely]] {
                        switch( sortedMoves[ i ].m_param ) {
                        case M_SHELL_TRAP:
                            snprintf(
                                buffer, 99, GET_STRING( 269 ),
                                _battleUI
                                    .getPkmnName( _field.getPkmn( sortedMoves[ i ].m_user.first,
                                                                  sortedMoves[ i ].m_user.second ),
                                                  sortedMoves[ i ].m_user.first, false )
                                    .c_str( ) );
                            _battleUI.log( buffer );
                            break;
                        case M_FOCUS_PUNCH:
                            snprintf(
                                buffer, 99, GET_STRING( 270 ),
                                _battleUI
                                    .getPkmnName( _field.getPkmn( sortedMoves[ i ].m_user.first,
                                                                  sortedMoves[ i ].m_user.second ),
                                                  sortedMoves[ i ].m_user.first, false )
                                    .c_str( ) );
                            _battleUI.log( buffer );
                            break;
                        case M_BEAK_BLAST:
                            snprintf(
                                buffer, 99, GET_STRING( 271 ),
                                _battleUI
                                    .getPkmnName( _field.getPkmn( sortedMoves[ i ].m_user.first,
                                                                  sortedMoves[ i ].m_user.second ),
                                                  sortedMoves[ i ].m_user.first, false )
                                    .c_str( ) );
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

                        _field.executeBattleMove( &_battleUI, sortedMoves[ i ], targets,
                                                  targetedBy );

                        distributeEXP( );
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

        _battleUI.init( );

        for( u8 i = 0; i < 6; ++i ) {
            _opponentPkmnPerm[ i ]    = i;
            _playerPkmnPerm[ i ]      = i;
            _playerPkmnOrigLevel[ i ] = ( i < _playerTeamSize ) * _playerTeam[ i ].m_level;
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
        auto res       = p_move;
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
        res.m_user              = { field::PLAYER_SIDE, p_slot };
        res.m_type              = ATTACK;

        bool mega = _field.canMegaEvolve( false, p_slot ) && p_allowMegaEvolution;

        bool canUse[ 4 ], strgl = false;
        for( u8 i = 0; i < 4; ++i ) {
            canUse[ i ] = _field.canSelectMove( false, p_slot, i );
            strgl       = strgl || canUse[ i ];
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
                    return _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse,
                                                          mega );
                },
                [ & ]( u8 p_selection ) {
                    curSel = p_selection;
                    _battleUI.showAttackSelection( _field.getPkmn( false, p_slot ), canUse, mega,
                                                   curSel, res.m_megaEvolve );
                },
                curSel );

            if( rs == IO::choiceBox::BACK_CHOICE || rs == 4 ) {
                res.m_type = CANCEL;
                return res;
            }
            if( rs < 4 ) {
                // player selects an attack
                if( canUse[ rs ] ) {
                    res.m_param = _field.getPkmn( false, p_slot )->getMove( curSel );
                    auto tmp    = chooseTarget( res );

                    if( tmp.m_type != CANCEL ) {
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

    battleMoveSelection battle::getMoveSelection( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };

        if( !_field.canSelectMove( false, p_slot ) ) {
            // pkmn is hibernating / charging
            return _field.getStoredMove( false, p_slot );
        }

        _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot );
        u8 curSel = 0;
        _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

        cooldown = COOLDOWN_COUNT;
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
                    if( res.m_type != CANCEL ) { return res; }
                    break;
                case 1: { // Choose pkmn
                    SOUND::playSoundEffect( SFX_CHOOSE );

                    STS::partyScreen pt = STS::partyScreen(
                        _playerTeam, _playerTeamSize, false, false, false, 1, false, false, false,
                        true, 1 + u8( _policy.m_mode ), p_slot );

                    auto r = pt.run( p_slot );

                    _battleUI.init( );

                    for( u8 i2 = 0; i2 < 2; ++i2 )
                        for( u8 j2 = 0; j2 <= u8( _policy.m_mode ); ++j2 ) {
                            auto st = _field.getSlotStatus( i2, j2 );
                            if( st == slot::status::NORMAL ) {
                                _battleUI.updatePkmn( i2, j2, _field.getPkmn( i2, j2 ) );
                            }
                        }

                    if( r.getSelectedPkmn( ) < 255 ) {
                        res.m_type  = SWITCH;
                        res.m_param = r.getSelectedPkmn( );
                        _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot );
                        return res;
                    }
                    break;
                }
                case 2: // Run / Cancel
                    if( _isWildBattle ) {
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        res.m_type = RUN;
                    } else if( p_slot ) {
                        SOUND::playSoundEffect( SFX_CANCEL );
                        res.m_type = CANCEL;
                    }
                    return res;
                case 3: { // Choose item
                    SOUND::playSoundEffect( SFX_CHOOSE );

                    BAG::bagViewer bv
                        = BAG::bagViewer( _playerTeam, _isWildBattle ? BAG::bagViewer::WILD_BATTLE
                                                                     : BAG::bagViewer::BATTLE );
                    u16 itm = bv.getItem( );

                    if( itm ) {
                        auto idata = ITEM::getItemData( itm );

                        if( idata.m_itemType == ITEM::ITEMTYPE_POKEBALL ) {
                            // Player throws a ball
                            res.m_type  = CAPTURE;
                            res.m_param = itm;
                        } else if( ( idata.m_itemType & 15 ) == 2 ) {
                            // Already used
                            res.m_type  = NO_OP_NO_CANCEL;
                            res.m_param = 0;
                        } else {
                            res.m_type  = USE_ITEM;
                            res.m_param = itm;
                        }
                    }

                    _battleUI.init( );

                    for( u8 i2 = 0; i2 < 2; ++i2 )
                        for( u8 j2 = 0; j2 <= u8( _policy.m_mode ); ++j2 ) {
                            auto st = _field.getSlotStatus( i2, j2 );
                            if( st == slot::status::NORMAL ) {
                                _battleUI.updatePkmn( i2, j2, _field.getPkmn( i2, j2 ) );
                            }
                        }

                    if( itm ) {
                        _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot );
                        return res;
                    }
                    break;
                }
                default:
                    break;
                }
                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot );
                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

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

                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 1;
                } else {
                    curSel--;
                }

                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 && ( p_slot || _isWildBattle ) ) {
                    curSel = 2;
                } else {
                    curSel = 0;
                }

                _battleUI.showMoveSelection( _field.getPkmn( false, p_slot ), p_slot, curSel );

                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }

        return res;
    }

    u8 battle::getNextAIPokemon( ) const {
        for( u8 i = u8( _policy.m_mode ) + 1; i < _opponentTeamSize; ++i ) {
            if( _opponentTeam[ i ].canBattle( ) ) { return i; }
        }
        return 255;
    }

    battleMoveSelection battle::getAIMove( u8 p_slot ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { true, p_slot };
        switch( _policy.m_aiLevel ) {
        default:
            [[likely]] case 0 : { // Wild pkmn
                // Pick a random move
                res.m_type = ATTACK;
                bool canUse[ 4 ], str = false;
                for( u8 i = 0; i < 4; ++i ) {
                    canUse[ i ] = _field.canSelectMove( true, p_slot, i );
                    str         = str || canUse[ i ];
                }

                if( !str ) { // pkmn struggles
                    res.m_param = M_STRUGGLE;
                } else {
                    u8 mv = rand( ) % 4;
                    while( !canUse[ mv ] ) { mv = rand( ) % 4; }
                    res.m_param = _field.getPkmn( true, p_slot )->getMove( mv );
                }

                // Pick a random target
                auto mdata     = MOVE::getMoveData( res.m_param );
                res.m_moveData = mdata;
                auto tg        = mdata.m_pressureTarget != MOVE::NO_TARGET ? mdata.m_pressureTarget
                                                                    : mdata.m_target;

                bool canTarget[ 4 ];
                for( u8 i = 0; i < 4; ++i ) {
                    canTarget[ i ] = _field.getPkmn( i < 2, i & 1 ) != nullptr;
                }
                u8 ctg = rand( ) % 2;

                switch( tg ) {
                case MOVE::RANDOM:
                case MOVE::ANY_FOE:
                    [[likely]] case MOVE::ANY : while( !canTarget[ 2 + ctg ] ) {
                        ctg = rand( ) & 1;
                    }
                    res.m_target = fieldPosition( false, ctg );
                    break;
                case MOVE::ALLY_OR_SELF:
                    while( !canTarget[ ctg ] ) { ctg = rand( ) & 1; }
                    res.m_target = fieldPosition( true, ctg );
                    break;
                    [[unlikely]] default : break;
                }
                break;
            }
        }
        return res;
    }

    bool battle::endConditionHit( battle::battleEndReason& p_out ) {
        if( _isWildBattle && _field.getSlotStatus( true, 0 ) == slot::FAINTED ) {
            distributeEXP( );
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
        if( _isWildBattle && p_battleEndReason != BATTLE_RUN ) {
            SOUND::playBGM( MOD_VICTORY_WILD );
            for( u8 i = 0; i < 90; ++i ) { swiWaitForVBlank( ); }
        }

        switch( p_battleEndReason ) {
        case BATTLE_CAPTURE:
            handleCapture( );
            break;
        default:
            break;
        }

        restoreInitialOrder( false );
        resetBattleTransformations( false );

        // Check for evolutions / attack learn

        for( u8 i = 0; i < _playerTeamSize; ++i ) {
            if( _playerTeam[ i ].m_level != _playerPkmnOrigLevel[ i ] ) {
                // pkmn was elevated to a new level, check for new moves

                getLearnMoves( _playerTeam[ i ].getSpecies( ), _playerPkmnOrigLevel[ i ] + 1,
                               _playerTeam[ i ].m_level, 20, MOVE_BUFFER );

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
                                for( u8 g = 0; g < 100; ++g ) swiWaitForVBlank( );
                            },
                            [ & ]( boxPokemon*, u16 ) -> u8 {
                                IO::choiceBox cb = IO::choiceBox(
                                    IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT_CANCEL );
                                u8 curSel = 0;

                                bool canUse[ 4 ] = { 1, 1, 1, 1 };
                                u8   rs          = cb.getResult(
                                    [ & ]( u8 ) {
                                        return _battleUI.showAttackSelection( &_playerTeam[ i ],
                                                                              canUse, false );
                                    },
                                    [ & ]( u8 p_selection ) {
                                        curSel = p_selection;
                                        _battleUI.showAttackSelection( &_playerTeam[ i ], canUse,
                                                                       false, curSel, false );
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

                auto edata = getPkmnEvolveData( _playerTeam[ i ].getSpecies( ),
                                                _playerTeam[ i ].getForme( ) );

                u8 ev = 0;
                if( ( ev = _playerTeam[ i ].getItem( ) != I_EVERSTONE
                           && _playerTeam[ i ].canEvolve( 0, EVOMETHOD_LEVEL_UP, &edata ) ) ) {

                    u16 oldsp = _playerTeam[ i ].getSpecies( );
                    u8  oldfm = _playerTeam[ i ].getForme( );
                    u16 newsp = edata.m_evolutions[ ev - 1 ].m_target;
                    u8  newfm = edata.m_evolutions[ ev - 1 ].m_targetForme;

                    if( IO::ANIM::evolvePkmn( oldsp, oldfm, newsp, newfm,
                                              _playerTeam[ i ].isShiny( ),
                                              _playerTeam[ i ].isFemale( ), true ) ) {
                        _playerTeam[ i ].evolve( );
                    }
                }
            }
        }
    }

    bool battle::playerRuns( ) {
        auto p1 = _field.getPkmn( false, 0 );
        if( p1 == nullptr ) { return true; }

        if( p1->getItem( ) == I_SMOKE_BALL ) { return true; }

        if( !_field.suppressesAbilities( ) ) {
            if( p1->getAbility( ) == A_RUN_AWAY ) { return true; }
        }

        u16 ownSpeed = _field.getStat( false, 0, SPEED );
        u16 oppSpeed = _field.getStat( true, 0, SPEED );

        if( ownSpeed > oppSpeed || oppSpeed == 0 ) { return true; }
        if( !_field.canSwitchOut( false, 0 ) ) { return false; }

        return ( ownSpeed + ( rand( ) % oppSpeed ) >= oppSpeed );
    }

    bool battle::playerCaptures( u16 p_pokeball ) {
        SAVE::SAV.getActiveFile( ).m_bag.erase( BAG::bag::ITEMS, p_pokeball, 1 );

        char buffer[ 100 ];
        u16  ballCatchRate = 2;

        auto plpk   = _field.getPkmn( false, 0 );
        auto wild   = _field.getPkmn( true, 0 );
        u16  specId = wild->getSpecies( );

        auto p = _field.getPkmnData( true, 0 );

        switch( p_pokeball ) {
        case I_SAFARI_BALL:
        case I_SPORT_BALL:
        case I_GREAT_BALL:
            ballCatchRate = 3;
            break;
        case I_ULTRA_BALL:
            ballCatchRate = 4;
            break;
        case I_MASTER_BALL:
            ballCatchRate = 512;
            break;

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
            ballCatchRate = std::min( 128, p.m_baseForme.m_weight >> 2 );
            break;
        case I_FAST_BALL:
            if( p.m_baseForme.m_bases[ 5 ] >= 100 ) ballCatchRate = 16;
            break;

        case I_REPEAT_BALL:
            if( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ specId / 8 ] & ( 1 << ( specId % 8 ) ) )
                ballCatchRate = 6;
            break;
        case I_TIMER_BALL:
            ballCatchRate = std::min( _round + 10 / 5, 8 );
            break;
        case I_NEST_BALL:
            ballCatchRate = std::max( ( 40 - wild->m_level ) / 5, 2 );
            break;
        case I_NET_BALL:
            if( p.m_baseForme.m_types[ 0 ] == type::BUG || p.m_baseForme.m_types[ 1 ] == type::BUG
                || p.m_baseForme.m_types[ 0 ] == type::WATER
                || p.m_baseForme.m_types[ 1 ] == type::WATER )
                ballCatchRate = 6;
            break;
        case I_DIVE_BALL:
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::moveMode::DIVE )
                ballCatchRate = 7;
            break;

        case I_QUICK_BALL:
            if( _round < 2 ) ballCatchRate = 10;
            break;
        case I_DUSK_BALL:
            if( MOVE::possible( M_DIG, 0 ) || getCurrentDaytime( ) == 4 ) ballCatchRate = 7;
            break;

        default:
            break;
        }

        u8 status = 2;
        if( wild->m_status.m_isAsleep || wild->m_status.m_isFrozen ) status = 4;
        if( wild->m_status.m_isParalyzed || wild->m_status.m_isPoisoned
            || wild->m_status.m_isBadlyPoisoned || wild->m_status.m_isBurned )
            status = 3;

        u32 catchRate = ( 3 * wild->m_stats.m_maxHP - 2 * wild->m_stats.m_curHP ) * p.m_catchrate
                        * ballCatchRate / 3 / wild->m_stats.m_maxHP * status;
        u32 pr   = u32( ( 65535 << 4 ) / ( sqrt( sqrt( ( 255L << 18 ) / catchRate ) ) ) );
        u8  succ = 0;
        for( u8 i = 0; i < 4; ++i ) {
            u16 rn = rand( );
            if( rn > pr ) break;
            succ++;
        }
        _battleUI.animateCapturePkmn( p_pokeball, succ );

        switch( succ ) {
        default:
        case 0:
        case 1:
        case 2:
        case 3:
            _battleUI.log( GET_STRING( 487 + succ ) );
            break;
        case 4:
            snprintf( buffer, 99, GET_STRING( 486 ), wild->m_boxdata.m_name );
            _battleUI.log( buffer );
            break;
        }
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

        if( succ == 4 ) {
            wild->m_boxdata.m_ball = ITEM::itemToBall( p_pokeball );
            if( p_pokeball == I_FRIEND_BALL ) { wild->m_boxdata.m_steps = 200; }
            if( p_pokeball == I_HEAL_BALL ) { wild->heal( ); }
            return true;
        }
        return false;
    }

    void battle::handleCapture( ) {
        _field.revertTransform( true, 0 );
        u16 spid = _field.getPkmn( true, 0 )->getSpecies( );

        char buffer[ 100 ];
        if( !( SAVE::SAV.getActiveFile( ).m_caughtPkmn[ spid / 8 ] & ( 1 << ( spid % 8 ) ) ) ) {
            SAVE::SAV.getActiveFile( ).registerCaughtPkmn( spid );
            snprintf( buffer, 99, GET_STRING( 174 ), getDisplayName( spid ).c_str( ) );
            _battleUI.log( buffer );

            for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }

            // TODO
            // DEX::dex( DEX::dex::SHOW_SINGLE, -1 ).run( spid );
        }
        _battleUI.handleCapture( _field.getPkmn( true, 0 ) );

        // Check whether the pkmn fits in the team
        auto pkmn = _field.getPkmn( true, 0 );
        if( pkmn == nullptr ) [[unlikely]] {
                return;
            }
        else if( _playerTeamSize < 6 ) {
            std::memcpy( &_playerTeam[ _playerTeamSize ], pkmn, sizeof( pokemon ) );
            _playerTeamSize++;
        } else {
            u8 oldbx = SAVE::SAV.getActiveFile( ).m_curBox;
            u8 nb    = SAVE::SAV.getActiveFile( ).storePkmn( *pkmn );
            if( nb != u8( -1 ) ) {
                snprintf( buffer, 99, GET_STRING( 175 ), pkmn->m_boxdata.m_name );
                _battleUI.log( buffer );

                if( oldbx != nb ) {
                    snprintf( buffer, 99, GET_STRING( 176 ),
                              SAVE::SAV.getActiveFile( ).m_storedPokemon[ oldbx ].m_name );
                    _battleUI.log( buffer );
                }
                sprintf( buffer, GET_STRING( 177 ), pkmn->m_boxdata.m_name,
                         SAVE::SAV.getActiveFile( ).m_storedPokemon[ nb ].m_name );
                _battleUI.log( buffer );
            } else {
                _battleUI.log( GET_STRING( 178 ) );
                snprintf( buffer, 99, GET_STRING( 179 ), pkmn->m_boxdata.m_name );
                _battleUI.log( buffer );
            }
            for( u8 i = 0; i < 120; ++i ) { swiWaitForVBlank( ); }
        }
    }

    void battle::megaEvolve( fieldPosition p_position ) {
        _field.megaEvolve( &_battleUI, p_position.first, p_position.second );
        if( !p_position.first ) { _policy.m_allowMegaEvolution = false; }
    }

    void battle::checkAndRefillBattleSpots( slot::status p_checkType ) {
        for( u8 i = 0; i < 2; ++i )
            for( u8 j = 0; j <= u8( _policy.m_mode ); ++j ) {
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
                            false, false, 1 + u8( _policy.m_mode ), j );

                        auto res = pt.run( j );

                        if( res.getSelectedPkmn( ) == 255 ) [[unlikely]] {
#ifdef DESQUID
                                _battleUI.log( "Oh well, time to desquid..." );
#endif
                            }

                        _battleUI.init( );

                        for( u8 i2 = 0; i2 < 2; ++i2 )
                            for( u8 j2 = 0; j2 <= u8( _policy.m_mode ); ++j2 ) {
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
        // TODO
    }

    void battle::sortPkmn( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8*      perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8       len  = p_opponent ? _opponentTeamSize : _playerTeamSize;

        for( u8 i = 0; i < len - 1; ++i ) {
            if( !pkmn[ i ].canBattle( ) ) {
                // pkmn cannot battle, move it to the end of the list.
                for( u8 j = i + 1; j < len; ++j ) {
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

        char buffer[ 100 ];
        for( u8 j = 0; j <= u8( _policy.m_mode ); ++j ) {
            if( _field.getSlotStatus( true, j ) == slot::status::FAINTED ) {
                // distribute EXP
#ifdef DESQUID_MORE
                std::string lmsg = "Distributing EXP to ";
#endif
                std::vector<u8> reg, share;
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
                u32 baseexp = _field.getPkmnData( true, j ).m_baseForme.m_expYield
                                  * _opponentTeam[ j ].m_level
                              >> 3;
                if( !_isWildBattle ) { baseexp = baseexp * 3 / 2; }
                if( reg.size( ) && share.size( ) ) { baseexp /= 2; }

                for( auto lst : { reg, share } ) {
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

                                _playerTeam[ i ].EVset( ev,
                                                        _playerTeam[ i ].EVget( ev )
                                                            + ( ( _field.getPkmnData( true, j )
                                                                      .m_baseForme.m_evYield[ ev ]
                                                                  + m2 )
                                                                << m1 ) );
                            }

                            u32 curexp = baseexp / reg.size( );
                            if( _playerTeam[ i ].getItem( ) == I_LUCKY_EGG ) { curexp <<= 1; }
                            if( _playerTeam[ i ].isForeign( ) ) { curexp <<= 1; }

                            u8 oldlv = _playerTeam[ i ].m_level;

                            _playerTeam[ i ].gainExperience( curexp );
                            snprintf( buffer, 99, GET_STRING( 167 ),
                                      _playerTeam[ i ].m_boxdata.m_name, curexp );
                            _battleUI.log( buffer );
                            for( u8 g = 0; g < 30; ++g ) { swiWaitForVBlank( ); }
                            if( _playerTeam[ i ].m_level != oldlv ) {
                                snprintf( buffer, 99, GET_STRING( 168 ),
                                          _playerTeam[ i ].m_boxdata.m_name,
                                          _playerTeam[ i ].m_level );
                                _battleUI.log( buffer );
                                for( u8 g = 0; g < 30; ++g ) { swiWaitForVBlank( ); }
                            }
                            if( i <= u8( _policy.m_mode ) ) {
                                // update battleUI
                                _battleUI.updatePkmnStats( false, i, _field.getPkmn( false, i ),
                                                           true );
                            }
                        }
                    }
                }
            }
        }
    }
} // namespace BATTLE
