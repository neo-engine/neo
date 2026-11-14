/*
Pokémon neo
------------------------------

file        : battleAftermath.cpp
author      : Philip Wellnitz
description : the end of the fight: endBattle bookkeeping, running away and capturing

Copyright (C) 2026
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

                if( _opponent.m_data.m_moneyMultiplier ) {
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_WIN_MONEY ),
                        FS::kanaWrap( _opponent.m_data.m_moneyMultiplier, CURRENT_LANGUAGE ) );
                    _battleUI.log( buffer );
                    WAIT( FULL_SEC );
                    SAVE::CURRENT_FILE->m_money += _opponent.m_data.m_moneyMultiplier;
                    if( SAVE::CURRENT_FILE->m_money > 999'999'999 ) {
                        SAVE::CURRENT_FILE->m_money = 999'999'999;
                    }
                }
            }
            if( p_battleEndReason == BATTLE_OPPONENT_WON ) {
                _battleUI.handleBattleEnd( false );

                if( _opponent.m_data.m_moneyMultiplier ) {
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_LOSE_MONEY ),
                        FS::kanaWrap( _opponent.m_data.m_moneyMultiplier, CURRENT_LANGUAGE ) );
                    _battleUI.log( buffer );
                    WAIT( FULL_SEC );

                    if( SAVE::CURRENT_FILE->m_money < _opponent.m_data.m_moneyMultiplier ) {
                        SAVE::CURRENT_FILE->m_money = 0;
                    } else {
                        SAVE::CURRENT_FILE->m_money -= _opponent.m_data.m_moneyMultiplier;
                    }
                }
            }
        }

        restoreInitialOrder( false );
        resetBattleTransformations( false );

        if( p_battleEndReason == BATTLE_PLAYER_WON ) {
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
                            if( _playerTeam[ i ].getMove( mv ) == MOVE_BUFFER[ j ] ) { gd = false; }
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
                                    IO::simpleChoiceBox cb = IO::simpleChoiceBox(
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
                                        IO::DEFAULT_TICK, curSel );

                                    if( rs < 4 ) {
                                        return rs;
                                    } else {
                                        return 4;
                                    }
                                },
                                [ & ]( const char* p_message ) {
                                    IO::simpleYesNoBox yn;
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
        }
        _battleUI.deinit( );
        SOUND::deinitBattleSound( );

        if( p_battleEndReason == BATTLE_PLAYER_WON ) {
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
        if( !_isMockBattle ) { SAVE::CURRENT_FILE->m_bag.erase( BAG::bag::ITEMS, p_pokeball, 1 ); }

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        u16          ballCatchRate = 2;

        auto plpk = _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 );
        if( plpk == nullptr ) { return false; }
        auto wild = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_0 );
        if( wild == nullptr ) { return false; }
        u16 specId = wild->getSpecies( );

        auto p = _field.getPkmnData( field::OPPONENT_SIDE, field::PKMN_0 );

        bool forceFail = false;

        if( MAP::curMap != nullptr && MAP::curMap->getCurrentLocationId( ) == L_INFINITY_CAVE ) {
            forceFail = p_pokeball != I_INFINITY_BALL;
        } else {
            forceFail = p_pokeball == I_INFINITY_BALL;
        }

        if( forceFail ) {
            _battleUI.animateCapturePkmn( p_pokeball, 0 );
            _battleUI.log( GET_STRING( IO::STR_BATTLE_PKMN_CAPTURE_FORCED_FAIL ) );

            return false;
        }

        switch( p_pokeball ) {
        case I_SAFARI_BALL:
        case I_SPORT_BALL:
        case I_GREAT_BALL: ballCatchRate = 3; break;
        case I_ULTRA_BALL: ballCatchRate = 4; break;
        case I_INFINITY_BALL:
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
            if( SAVE::CURRENT_FILE->caught( specId ) ) ballCatchRate = 6;
            break;
        case I_TIMER_BALL: ballCatchRate = std::min( ( _round + 10 ) / 5, 8 ); break;
        case I_NEST_BALL: ballCatchRate = std::max( ( 40 - wild->m_level ) / 5, 2 ); break;
        case I_NET_BALL:
            if( _field.hasType( field::OPPONENT_SIDE, field::PKMN_0, TYPE_BUG )
                || _field.hasType( field::OPPONENT_SIDE, field::PKMN_0, TYPE_WATER ) ) {
                ballCatchRate = 6;
            }
            break;
        case I_DIVE_BALL:
            if( SAVE::CURRENT_FILE->m_player.m_movement == MAP::moveMode::DIVE ) ballCatchRate = 7;
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
            ballCatchRate = 2;
            if( wild->isShiny( ) ) { ballCatchRate = 512; }
            break;

        case I_BEAST_BALL: ballCatchRate = 0; break;

        default: break;
        }

        u8 status = 2;
        if( wild->m_status.m_isAsleep || wild->m_status.m_isFrozen ) status = 4;
        if( wild->m_status.m_isParalyzed || wild->m_status.m_isPoisoned
            || wild->m_status.m_isBadlyPoisoned || wild->m_status.m_isBurned )
            status = 3;

        u32 maxHP     = wild->m_stats.m_maxHP ? wild->m_stats.m_maxHP : 1; // avoid div-by-0
        u32 catchRate = ( 3 * maxHP - 2 * wild->m_stats.m_curHP ) * p.m_catchrate * ballCatchRate
                        / 3 / maxHP * status;
        if( !catchRate ) { catchRate = 1; }
        u32 inner = u32( ( 255L << 18 ) / catchRate );
        if( !inner ) { inner = 1; }
        u32 pr   = u32( ( 65535 << 4 ) / ( sqrt( sqrt( inner ) ) ) );
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
        case 0: _battleUI.log( GET_STRING( IO::STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_0 ) ); break;
        case 1: _battleUI.log( GET_STRING( IO::STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_1 ) ); break;
        case 2: _battleUI.log( GET_STRING( IO::STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_2 ) ); break;
        case 3: _battleUI.log( GET_STRING( IO::STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_3 ) ); break;
        case 4:
            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_PKMN_CAPTURE_SUCC ),
                      wild->m_boxdata.getNameS( ).c_str( ) );
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
        if( !SAVE::CURRENT_FILE->caught( spid ) ) {
            SAVE::CURRENT_FILE->registerCaughtPkmn( spid );
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
            u8 oldbx = SAVE::CURRENT_FILE->m_curBox;
            u8 nb    = SAVE::CURRENT_FILE->storePkmn( *pkmn );
            if( nb != u8( -1 ) ) {
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_PKMN_SENT_TO_STORAGE ),
                          pkmn->m_boxdata.getNameS( ).c_str( ) );
                _battleUI.log( buffer );

                if( oldbx != nb ) {
                    snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_STORAGE_BOX_FULL ),
                              SAVE::CURRENT_FILE->m_storedPokemon[ oldbx ].getName(
                                  CURRENT_LANGUAGE == SAVE::JAP ) );
                    _battleUI.log( buffer );
                }
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_STORAGE_BOX_PICKED ),
                          pkmn->m_boxdata.getNameS( ).c_str( ),
                          SAVE::CURRENT_FILE->m_storedPokemon[ nb ].getName( CURRENT_LANGUAGE
                                                                             == SAVE::JAP ) );
                _battleUI.log( buffer );
            } else {
                _battleUI.log( GET_STRING( IO::STR_UI_STORAGE_ALL_BOXES_FULL ) );
                snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_UI_PKMN_RELEASED ),
                          pkmn->m_boxdata.getNameS( ).c_str( ) );
                _battleUI.log( buffer );
            }
            WAIT( DOUBLE_SEC );
        }
    }

} // namespace BATTLE
