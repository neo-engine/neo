/*
Pokémon neo
------------------------------

file        : battleTeam.cpp
author      : Philip Wellnitz
description : team plumbing: mega evolution, refilling slots, switching, items, team order and EXP

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
                        auto nxt = getNextAIPokemon( getOwner( j ) );
                        if( nxt != NO_PKMN ) { switchPokemon( { i, j }, nxt ); }
                    } else if( !i ) {
                        // Check if the player has something to send out
                        bool good = false;
                        for( u8 k = getBattlingPKMNCount( _policy.m_mode ); k < _playerTeamSize;
                             ++k ) {
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
                                _battleUI.updatePkmn( i2, j2, _field.getPkmnOrDisguise( i2, j2 ) );
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
                                &_opponentTeam[ p_toSwitch.second ], _opponentTeam );
        } else {
            // player
            std::swap( _playerTeam[ p_toSwitch.second ], _playerTeam[ p_newIndex ] );
            std::swap( _playerPkmnPerm[ p_toSwitch.second ], _playerPkmnPerm[ p_newIndex ] );
            _field.sendPokemon( &_battleUI, p_toSwitch.first, p_toSwitch.second,
                                &_playerTeam[ p_toSwitch.second ], _playerTeam );
        }
    }

    void battle::useItem( fieldPosition p_target, u16 p_item ) {
        auto         idata           = FS::getItemData( p_item );
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];
        auto         pkmn = _field.getPkmn( p_target.first, p_target.second );

        if( p_target.first ) {
            // opponent item
            auto& tr        = getOpponent( getOwner( p_target.second ) );
            bool  itemfound = false;
            for( u8 i = 0; i < 5; ++i ) {
                if( tr.m_data.m_items[ i ] == p_item ) {
                    tr.m_data.m_items[ i ] = 0;
                    itemfound              = true;
                    break;
                }
            }
            if( !itemfound ) [[unlikely]] {
                // cannot use non-existing item . . .
                return;
            }

            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_TRAINER_USED_ITEM ),
                      FS::getTrainerClassName( tr.getClass( ) ).c_str( ), tr.m_strings.m_name,
                      FS::getItemName( p_item ).c_str( ) );
            _battleUI.log( buffer );
        } else {
            // player item
            snprintf( buffer, TMP_BUFFER_SIZE, GET_STRING( IO::STR_BATTLE_PLAYER_USED_ITEM ),
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
            _battleUI.updatePkmnStats(
                p_target.first, p_target.second,
                _field.getPkmnOrDisguise( p_target.first, p_target.second ) );
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
        [[unlikely]] case I_NION_BERRY: {
            bs = _field.getBoosts( p_target.first, p_target.second );
            if( bs.negative( ) != boosts( ) ) {
                bs    = bs.negative( ).invert( );
                boost = true;
            } else {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
            break;
        }

        case I_BLUE_FLUTE:
            remitem = false;
            if( _field.hasStatusCondition( p_target.first, p_target.second, SLEEP ) ) {
                _field.removeStatusCondition( p_target.first, p_target.second );
                _battleUI.updatePkmnStats(
                    p_target.first, p_target.second,
                    _field.getPkmnOrDisguise( p_target.first, p_target.second ) );
            } else {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
            break;

        case I_GUARD_SPEC:
            if( !_field.addSideCondition( &_battleUI, p_target.first, SC_MIST, 5 ) ) {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
            break;
        case I_DIRE_HIT:
            if( !_field.addVolatileStatus( &_battleUI, p_target.first, p_target.second,
                                           VS_FOCUSENERGY, 255 ) ) {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
            break;
        case I_YELLOW_FLUTE:
            remitem = false;
            [[fallthrough]];
        [[unlikely]] case I_RIE_BERRY:
        case I_PERSIM_BERRY:
            if( volst & VS_CONFUSION ) {
                _field.removeVolatileStatus( &_battleUI, p_target.first, p_target.second,
                                             VS_CONFUSION );
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_CONFUSION_HEALED ) );
                snprintf(
                    buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                    _battleUI
                        .getPkmnName( _field.getPkmnOrDisguise( p_target.first, p_target.second ),
                                      p_target.first )
                        .c_str( ) );
                _battleUI.log( buffer );
            } else {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
            break;

        case I_RED_FLUTE:
            remitem = false;
            [[fallthrough]];
        [[unlikely]] case I_GARC_BERRY:
            if( volst & VS_ATTRACT ) {
                _field.removeVolatileStatus( &_battleUI, p_target.first, p_target.second,
                                             VS_ATTRACT );
            } else {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
            break;

        default: break;
        }

        if( boost ) {
            auto res = _field.addBoosts( p_target.first, p_target.second, bs );
            if( res != boosts( ) ) {
                _battleUI.logBoosts( _field.getPkmnOrDisguise( p_target.first, p_target.second ),
                                     p_target.first, p_target.second, bs, res );
            } else {
                _battleUI.log( GET_STRING( IO::STR_BATTLE_NOTHING_HAPPENED ) );
            }
        }
        if( remitem && !p_target.first ) {
            SAVE::CURRENT_FILE->m_bag.erase( BAG::toBagType( idata.m_itemType ), p_item, 1 );
        }
    }

    void battle::sortPkmn( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8*      perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8       len  = p_opponent ? _opponentTeamSize : _playerTeamSize;

        if( len > ( p_opponent && isMultiBattle( ) ? MAX_OPPONENT_TEAM : SAVE::NUM_PARTY_SLOTS ) )
            [[unlikely]] {
            len = p_opponent && isMultiBattle( ) ? MAX_OPPONENT_TEAM : SAVE::NUM_PARTY_SLOTS;
        }

        const auto swapSortAt = [ pkmn, perm, p_opponent, this ]( u8 p_a, u8 p_b ) {
            std::swap( pkmn[ p_a ], pkmn[ p_b ] );
            if( p_opponent ) { std::swap( _yieldEXP[ p_a ], _yieldEXP[ p_b ] ); }
            std::swap( perm[ p_a ], perm[ p_b ] );
        };
        const auto canBattleAt = [ pkmn ]( u8 p_idx ) { return pkmn[ p_idx ].canBattle( ); };

        if( p_opponent && isMultiBattle( ) ) {
            for( u8 owner : { OWNER_A, OWNER_B } ) {
                sortGroup( owner, 2, groupLen( len, owner, true ), canBattleAt, swapSortAt );
            }
            return;
        }
        sortGroup( OWNER_A, 1, len, canBattleAt, swapSortAt );
    }

    void battle::restoreInitialOrder( bool p_opponent ) {
        pokemon* pkmn = p_opponent ? _opponentTeam : _playerTeam;
        u8*      perm = p_opponent ? _opponentPkmnPerm : _playerPkmnPerm;
        u8       len  = p_opponent ? _opponentTeamSize : _playerTeamSize;

        const auto swapRestoreAt = [ pkmn, p_opponent, this ]( u8 p_a, u8 p_b ) {
            std::swap( pkmn[ p_a ], pkmn[ p_b ] );
            if( p_opponent ) { std::swap( _yieldEXP[ p_a ], _yieldEXP[ p_b ] ); }
        };

        if( p_opponent && isMultiBattle( ) ) {
            for( u8 owner : { OWNER_A, OWNER_B } ) {
                restoreGroup( owner, 2, groupLen( len, owner, true ), perm, swapRestoreAt );
            }
            return;
        }
        restoreGroup( OWNER_A, 1, len, perm, swapRestoreAt );
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
                        lmsg += _playerTeam[ q2 ].m_boxdata.getNameS( ) + " ";
#endif
                        // pkmn that get regular exp
                        reg.push_back( q2 );
                    } else if( _playerTeam[ q2 ].canBattle( )
                               && SAVE::CURRENT_FILE->m_options.m_EXPShareEnabled ) {
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

                            u32 curexp = baseexp / ( reg.size( ) ? reg.size( ) : 1 );
                            if( _playerTeam[ i ].getItem( ) == I_LUCKY_EGG ) { curexp <<= 1; }
                            if( _playerTeam[ i ].isForeign( ) ) { curexp <<= 1; }

                            u8 oldlv = _playerTeam[ i ].m_level;
                            if( _playerTeam[ i ].m_level < 100 ) {
                                _playerTeam[ i ].gainExperience( curexp );
                                snprintf( buffer, TMP_BUFFER_SIZE,
                                          GET_STRING( IO::STR_BATTLE_EXP_GAINED ),
                                          _playerTeam[ i ].m_boxdata.getNameS( ).c_str( ),
                                          FS::kanaWrap( curexp, CURRENT_LANGUAGE ) );
                                _battleUI.log( buffer );
                                if( _playerTeam[ i ].m_level != oldlv ) {
                                    snprintf( buffer, TMP_BUFFER_SIZE,
                                              GET_STRING( IO::STR_BATTLE_ADVANCE_LEVEL ),
                                              _playerTeam[ i ].m_boxdata.getNameS( ).c_str( ),
                                              FS::kanaWrap( _playerTeam[ i ].m_level,
                                                            CURRENT_LANGUAGE ) );
                                    _battleUI.log( buffer );
                                }
                            }
                            if( i < getBattlingPKMNCount( _policy.m_mode ) ) {
                                // update battleUI
                                _battleUI.updatePkmnStats(
                                    field::PLAYER_SIDE, i,
                                    _field.getPkmnOrDisguise( field::PLAYER_SIDE, i ), true );
                            }
                        }
                    }
                }
            }
        }
    }

} // namespace BATTLE
