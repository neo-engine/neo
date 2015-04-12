/*
Pokémon Emerald 2 Version
------------------------------

file        : battle.cpp
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <cwchar>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <tuple>
#include <initializer_list>

#include "battle.h"
#include "pokemon.h"
#include "move.h"
#include "item.h"
#include "saveGame.h"
#include "bag.h"
#include "buffer.h"
#include "fs.h"
#include "sprite.h"
#include "uio.h"

namespace BATTLE {
    const char* trainerclassnames[ ] = { "Pokémon-Trainer" };

    const char* ailmentnames[ ] = {
        "none",
        "wurde paralysiert.",
        "schläft ein.",
        "wurde eingefroren.",
        "fängt an zu brennen.",
        "wurde vergiftet.",
        "wurde verwirrt.",
        "Infatuation",
        "ist gefangen.",
        "wurde in Nachtmahr\ngefangen.",
        "wurde Folterknecht\nunterworfen.",
        "wurde blockiert.",
        "gähnt.",
        "kann nicht mehr\ngeheilt werden.",
        "No_type_immunity",
        "wurde bepflanzt",
        "fällt unter ein\nEmbargo.",
        "hört Abgesang.",
        "Ingrain"
    };

    //////////////////////////////////////////////////////////////////////////
    // BEGIN BATTLE
    //////////////////////////////////////////////////////////////////////////
#define C2I(a) ((a) - L'0')
    std::wstring parseLogCmd( battle* p_battle, const std::wstring& p_cmd ) {
        if( p_cmd == L"A" )
            return L"`";
        if( p_cmd == L"CLEAR" )
            return L"";

        if( p_cmd == L"TRAINER" ) {
            std::swprintf( wbuffer, 50, L"%s", p_battle->_opponent.m_battleTrainerName.c_str( ) );
            return std::wstring( wbuffer );
        }
        if( p_cmd == L"TCLASS" ) {
            std::swprintf( wbuffer, 50, L"%s", trainerclassnames[ p_battle->_opponent.m_trainerClass ] );
            return std::wstring( wbuffer );
        }
        if( p_cmd.substr( 0, 4 ) == L"COLR" ) {
            u8 r, g, b;

            r = 10 * C2I( p_cmd[ 5 ] ) + C2I( p_cmd[ 6 ] );
            g = 10 * C2I( p_cmd[ 8 ] ) + C2I( p_cmd[ 9 ] );
            b = 10 * C2I( p_cmd[ 11 ] ) + C2I( p_cmd[ 12 ] );

            p_battle->_battleUI->setLogTextColor( RGB15( r, g, b ) );
            if( r != 15 || g != 15 || b != 15 )
                IO::regularFont->setColor( COLOR_IDX, 1 );
            else
                IO::regularFont->setColor( GRAY_IDX, 1 );
            return L"";
        }

        pokemon target = ACPKMN2( *p_battle, 0, PLAYER );
        bool isPkmn = false;
        bool isOpp = false;

        if( p_cmd.substr( 0, 4 ) == L"OWN1" ) {
            target = ACPKMN2( *p_battle, 0, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OWN2" ) {
            target = ACPKMN2( *p_battle, 1, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP1" ) {
            target = ACPKMN2( *p_battle, 0, OPPONENT );
            isOpp = true;
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP2" ) {
            target = ACPKMN2( *p_battle, 1, OPPONENT );
            isOpp = true;
            isPkmn = true;
        }

        if( isPkmn && p_cmd.length( ) == 4 )
            return target.m_boxdata.m_name + std::wstring( isOpp ? L" (Gegner)" : L"" );

        if( isPkmn ) {
            auto specifier = p_cmd.substr( 5 );
            if( specifier == L"ABILITY" )
                return getWAbilityName( target.m_boxdata.m_ability );
            if( specifier.substr( 0, 4 ) == L"MOVE" ) {
                std::swprintf( wbuffer, 50, L"%s", AttackList[ target.m_boxdata.m_moves[ C2I( specifier[ 4 ] ) - 1 ] ]->m_moveName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"ITEM" ) {
                std::swprintf( wbuffer, 50, L"%s", ItemList[ target.m_boxdata.m_holdItem ]->m_itemName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"LEVEL" )
                return ( L"" + target.m_Level );
        }

        return L"";
    }

    battle::battle( battleTrainer& p_player, battleTrainer& p_opponent, int p_maxRounds, int p_AILevel, battleMode p_battleMode )
        : _player( p_player ), _opponent( p_opponent ) {
        _maxRounds = p_maxRounds;
        _AILevel = p_AILevel;
        m_battleMode = p_battleMode;

        m_distributeEXP = true;
    }


    /**
     *  @brief Write the message p_message to the battle log
     *  @param p_message: The message to be written
     */
    void battle::log( const std::wstring& p_message ) {
        std::wstring msg = L"";
        for( size_t i = 0; i < p_message.length( ); i++ ) {
            if( p_message[ i ] == L'[' ) {
                std::wstring accmd = L"";
                while( p_message[ ++i ] != L']' )
                    accmd += p_message[ i ];
                msg += parseLogCmd( this, accmd );
                if( accmd == L"CLEAR" ) {
                    _battleUI->writeLogText( msg );
                    _battleUI->clearLogScreen( );
                    msg = L"";
                }
            } else
                msg += p_message[ i ];
        }

        if( msg != L"" )
            _battleUI->writeLogText( msg );
        _battleUI->clearLogScreen( );
    }

    /**
     *  @brief      Runs the battle.
     *  @returns    -1 if opponent won, 0 if the battle resulted in a tie, 1 otherwise
     */
    s8 battle::start( ) {
        battleEndReason battleEnd;

        initBattle( );

        _round = 0;
        _maxRounds = 500;
        while( _round++ < _maxRounds ) {
            registerParticipatedPKMN( );

            bool p1CanMove = canMove( PLAYER, 0 );
CHOOSE1:
            firstMoveSwitchTarget = 0;

            _battleMoves[ 0 ][ PLAYER ] = { ( battleMove::type )0, 0, 0 };
            _battleMoves[ 1 ][ PLAYER ] = { ( battleMove::type )0, 0, 0 };
            _battleMoves[ 0 ][ OPPONENT ] = { ( battleMove::type )0, 0, 0 };
            _battleMoves[ 1 ][ OPPONENT ] = { ( battleMove::type )0, 0, 0 };

            if( ACPKMNSTS( 0, PLAYER ) != KO ) {
                if( p1CanMove ) {
                    _battleUI->declareBattleMove( 0, false );
                    if( _endBattle ) {
                        endBattle( battleEnd = RUN );
                        return ( battleEnd );
                    }
                } else
                    log( L"[OWN1] kann nicht angreifen...[A]" );
            }
            //If 1st action is RUN, the player has no choice for a second move
            if( _battleMoves[ 0 ][ PLAYER ].m_type != battleMove::RUN && ACPKMNSTS( 1, PLAYER ) != KO ) {
                if( m_battleMode == DOUBLE && canMove( PLAYER, 1 ) ) {
                    if( !_battleUI->declareBattleMove( 1, p1CanMove && ACPKMNSTS( 0, PLAYER ) != KO ) )
                        goto CHOOSE1;

                    if( _endBattle ) {
                        endBattle( battleEnd = RUN );
                        return ( battleEnd );
                    }
                } else if( m_battleMode == DOUBLE )
                    log( L"[OWN2] kann nicht angreifen...[A]" );
            } else
                _battleMoves[ 1 ][ PLAYER ].m_type = battleMove::RUN;

            getAIMoves( );

            doMoves( );

            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                break;
            }
            doWeather( );
            for( int k = 0; k < 4; ++k ) {
                if( m_battleMode != DOUBLE && ( k % 2 ) )
                    continue;

                handleFaint( k / 2, k % 2 );
                doItem( k / 2, k % 2, ability::BETWEEN_TURNS );
                doAbility( k / 2, k % 2, ability::BETWEEN_TURNS );
                handleSpecialConditions( k / 2, k % 2 );
                handleFaint( k / 2, k % 2 );
            }

            refillBattleSpots( true );
            for( int k = 0; k < 4; ++k ) {
                if( m_battleMode != DOUBLE && ( k % 2 ) )
                    continue;
                handleFaint( k / 2, k % 2, false );
            }
        }

        return battleEnd;
    }

    /**
     *  @brief Initialize the battle.
     */
    void battle::initBattle( ) {
        //Some basic initialization stuff
        _battleUI = new battleUI( this );
        _battleUI->init( );
        pokemonData pdata;
        for( u8 i = 0; i < 6; ++i ) {
            if( _player.m_pkmnTeam.size( ) > i ) {
                getAll( _player.m_pkmnTeam[ i ].m_boxdata.m_speciesId, pdata );
                _pkmns[ i ][ 0 ].m_pokemon = &( _player.m_pkmnTeam[ i ] );
                _pkmns[ i ][ 0 ].m_Types[ 0 ] = pdata.m_types[ 0 ];
                _pkmns[ i ][ 0 ].m_Types[ 1 ] = pdata.m_types[ 1 ];
                _pkmns[ i ][ 0 ].m_Types[ 2 ] = pdata.m_types[ 1 ];
            }
            if( _opponent.m_pkmnTeam.size( ) > i ) {
                getAll( _opponent.m_pkmnTeam[ i ].m_boxdata.m_speciesId, pdata );
                _pkmns[ i ][ 1 ].m_pokemon = &( _opponent.m_pkmnTeam[ i ] );
                _pkmns[ i ][ 1 ].m_Types[ 0 ] = pdata.m_types[ 0 ];
                _pkmns[ i ][ 1 ].m_Types[ 1 ] = pdata.m_types[ 1 ];
                _pkmns[ i ][ 1 ].m_Types[ 2 ] = pdata.m_types[ 1 ];
            }

            ACPOS( i, PLAYER ) = ACPOS( i, OPPONENT ) = i;
            for( u8 o = 0; o < MAX_STATS; ++o )
                ACPKMNSTATCHG( i, PLAYER )[ o ] = ACPKMNSTATCHG( i, OPPONENT )[ o ] = 0;
            if( _player.m_pkmnTeam.size( ) > i ) {
                if( ACPKMN( i, PLAYER ).m_boxdata.m_individualValues.m_isEgg )
                    ACPKMNSTS( i, PLAYER ) = NA;
                else if( ACPKMN( i, PLAYER ).m_stats.m_acHP == 0 )
                    ACPKMNSTS( i, PLAYER ) = KO;
                else if( ACPKMN( i, PLAYER ).m_statusint )
                    ACPKMNSTS( i, PLAYER ) = STS;
                else
                    ACPKMNSTS( i, PLAYER ) = OK;
            } else
                ACPKMNSTS( i, PLAYER ) = NA;
            if( _opponent.m_pkmnTeam.size( ) > i ) {
                if( ACPKMN( i, OPPONENT ).m_boxdata.m_individualValues.m_isEgg )
                    ACPKMNSTS( i, OPPONENT ) = NA;
                else if( ACPKMN( i, OPPONENT ).m_stats.m_acHP == 0 )
                    ACPKMNSTS( i, OPPONENT ) = KO;
                else if( ACPKMN( i, OPPONENT ).m_statusint )
                    ACPKMNSTS( i, OPPONENT ) = STS;
                else ACPKMNSTS( i, OPPONENT ) = OK;
            } else
                ACPKMNSTS( i, OPPONENT ) = NA;
        }

        for( u8 p = 0; p < 2; ++p ) {
            _battleSpotOccupied[ 0 ][ p ] = false;
            _battleSpotOccupied[ 1 ][ p ] = !( m_battleMode == DOUBLE );
        }

        refillBattleSpots( false, false );

        _battleUI->trainerIntro( );

        refillBattleSpots( false );

        if( m_weather != NO_WEATHER ) {
            log( _weatherMessage[ m_weather ] );
        }

        doAbilities( ability::BEFORE_BATTLE );
    }

    /**
     *  @brief send in PKMN for fainted ones, if possible
     *  @param p_choice: Specifies whether the player can choose the PKMN which is/are being sent
     */
    void battle::refillBattleSpots( bool p_choice, bool p_send ) {

        for( u8 i = 0; i < 1 + ( m_battleMode == DOUBLE ); ++i )for( u8 j = 0; j < 2; ++j ) {
            if( !_battleSpotOccupied[ i ][ j ] ) {
                bool refillpossible = false;

                for( u8 k = 1 + ( m_battleMode == DOUBLE );
                     k < ( j ? _opponent.m_pkmnTeam.size( ) : _player.m_pkmnTeam.size( ) ); ++k )
                     if( ACPKMNSTS( k, j ) != KO && ACPKMNSTS( k, j ) != SELECTED
                         && ACPKMNSTS( k, j ) != NA && ACPKMN( k, j ).m_stats.m_acHP ) {
                         refillpossible = true;
                         break;
                     }

                if( !refillpossible ) {
                    _battleSpotOccupied[ i ][ j ] = true;
                    continue;
                }

                u8 nextSpot = i;
                if( !p_choice || j )
                    nextSpot = getNextPKMN( j, i );
                else
                    nextSpot = _battleUI->choosePKMN( m_battleMode == DOUBLE, false );

                if( nextSpot != 7 && nextSpot != i )
                    std::swap( ACPOS( i, j ), ACPOS( nextSpot, j ) );
            }
        }

        //Sort the remaining PKMN according to their status -> No STS, STS, KO, NA
        std::vector<std::pair<u8, u8> > tmp;
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            tmp.push_back( std::pair<u8, u8>( ACPKMNSTS( i, PLAYER ), ACPOS( i, PLAYER ) ) );
        std::sort( tmp.begin( ), tmp.end( ) );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            ACPOS( i, PLAYER ) = tmp[ i - ( m_battleMode == DOUBLE ? 2 : 1 ) ].second;

        //Sort the remaining PKMN according to their status -> No STS, STS, KO, NA (For the opponent)
        tmp.clear( );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            tmp.push_back( std::pair<u8, u8>( ACPKMNSTS( i, OPPONENT ), ACPOS( i, OPPONENT ) ) );
        std::sort( tmp.begin( ), tmp.end( ) );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            ACPOS( i, OPPONENT ) = tmp[ i - ( m_battleMode == DOUBLE ? 2 : 1 ) ].second;

        orderPKMN( false );

        if( p_send ) {
            for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 1 + ( m_battleMode == DOUBLE ); ++j )
                if( !_battleSpotOccupied[ j ][ i ] )
                    _battleUI->updateStats( i, j, false );
            for( u8 i = 0; i < 2; ++i )for( u8 j = 1 + ( m_battleMode == DOUBLE ); j < 6; ++j )
                _battleUI->updateStats( i, j, false );
        }
        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 1 + ( m_battleMode == DOUBLE ); ++i )for( u8 j = 0; j < 2; ++j ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    if( !_battleSpotOccupied[ i ][ j ] ) {
                        if( p_send ) {
                            _battleUI->sendPKMN( j, i );
                            _battleSpotOccupied[ i ][ j ] = true;
                        }
                    }
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
     *  @brief Gets the index of the next PKMN that is not koed.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @returns Minimum i for which ACPKMNSTS(i,p_opponent) != KO and != SELECTED, 7 iff all PKMN fainted
     */
    u8 battle::getNextPKMN( bool p_opponent, u8 p_startIdx ) {
        u8 max = ( p_opponent ? _opponent.m_pkmnTeam.size( ) : _player.m_pkmnTeam.size( ) );

        for( u8 i = p_startIdx; i < max; ++i )
            if( ACPKMNSTS( i, p_opponent ) != KO
                && ACPKMNSTS( i, p_opponent ) != SELECTED )
                return i;
        return 7;
    }

    /**
     *  @brief Orders the PKMN according to their speed.
     *  @param p_includeMovePriority: Determines whether the PKMN's move's priorities should matter.
     */
    void battle::orderPKMN( bool p_includeMovePriority ) {
        _moveOrder[ 0 ][ 0 ] = -1;
        _moveOrder[ 0 ][ 1 ] = -1;
        _moveOrder[ 1 ][ 0 ] = -1;
        _moveOrder[ 1 ][ 1 ] = -1;

        //return;
        bool hasTrickRoom = ( _battleTerrain & TRICK_ROOM );

        std::vector< std::tuple<s16, s16, u16> > inits;
        for( u8 i = 0; i < ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); ++i ) {
            for( u8 j = 0; j < 2; ++j ) {
                s16 acSpd = ACPKMN( i, j ).m_stats.m_Spd;
                s8 movePr = ( ( _battleMoves[ i ][ j ].m_type == battleMove::ATTACK || _battleMoves[ i ][ j ].m_type == battleMove::MEGA_ATTACK ) ?
                              AttackList[ _battleMoves[ i ][ j ].m_value ]->m_movePriority : 0 );
                if( ACPKMN( i, j ).m_status.m_Paralyzed
                    && ACPKMN( i, j ).m_boxdata.m_ability != A_QUICK_FEET )
                    acSpd /= 4;
                else if( ACPKMN( i, j ).m_statusint && ACPKMN( i, j ).m_boxdata.m_ability == A_QUICK_FEET )
                    acSpd += acSpd / 2;

                if( p_includeMovePriority ) {
                    if( hasTrickRoom )
                        acSpd *= -1;
                    if( ACPKMN( i, j ).m_boxdata.m_holdItem == I_QUICK_CLAW
                        && ( ( rand( ) % 100 ) < 20 ) ) {

                        std::swprintf( wbuffer, 100, L"%ls%s agiert dank\neiner Flinkklaue zuerst![A]",
                                       ( ACPKMN( i, j ).m_boxdata.m_name ),
                                       ( j ? " (Gegner)" : "" ) );
                        log( wbuffer );

                        acSpd += 2000;
                    } else if( ACPKMN( i, j ).m_boxdata.m_holdItem == I_LAGGING_TAIL )
                        acSpd -= 2000;
                    else if( ACPKMN( i, j ).m_boxdata.m_ability == A_STALL )
                        acSpd -= 2000;

                    if( ACPKMN( i, j ).m_boxdata.m_holdItem == I_FULL_INCENSE )
                        movePr--;
                }

                if( _battleMoves[ i ][ j ].m_type == battleMove::SWITCH )
                    movePr = 7;
                if( _battleMoves[ i ][ j ].m_type == battleMove::USE_ITEM )
                    movePr = 7;
                inits.push_back( std::tuple<s16, s16, u8>( p_includeMovePriority * movePr, acSpd, 2 * i + j ) );
            }
        }
        std::sort( inits.begin( ), inits.end( ), std::greater < std::tuple<s8, s16, u16> >( ) );

        for( u8 i = 0; i < inits.size( ); ++i ) {
            s16 _0 = 0;
            s16 _1 = 0;
            u16 v = 0;
            std::tie( _0, _1, v ) = inits[ i ];
            u8 isOpp = v % 2,
                isSnd = v / 2;

            _moveOrder[ isSnd ][ isOpp ] = i;
        }
    }

    /**
     *  @brief Applies all possible abilities ordered by their PKMNs speed.
     *  @param p_situation: Current situation, on which an ability may be useable
     */
    void battle::doAbilities( ability::abilityType p_situation ) {
        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    doAbility( j, i, p_situation );
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
    *  @brief Applies all possible abilities ordered by their PKMNs speed.
    *  @param p_opponent: true iff the next opponent's PKMN is requested.
    *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
    *  @param p_situation: Current situation, on which an ability may be useable
    */
    void battle::doAbility( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation ) {
        if( !ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP )
            return;

        auto ab = ability( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability );

        if( ab.m_type & p_situation ) {
            std::swprintf( wbuffer, 50, L"%s von %ls%s wirkt.[A]",
                           ab.m_abilityName.c_str( ),
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? "\n(Gegner)" : "\n" ) );

            log( wbuffer );
            ab.m_effect.execute( *this, &( ACPKMN( p_pokemonPos, p_opponent ) ) );

            //for( u8 k = 0; k < 4; ++k ) {
            //    bool isOpp = k % 2,
            //        isSnd = k / 2;
            //    _battleUI->updateHP( isOpp, isSnd );
            //    _battleUI->updateStats( isOpp, isSnd );
            //}
        }
    }

    /**
     *  @brief Determines whether the specified PKMN can perform a move during the current turn.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    bool battle::canMove( bool p_opponent, u8 p_pokemonPos ) {
        if( m_battleMode != DOUBLE && p_pokemonPos )
            return false;
        return !ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ATTACK_BLOCKED ];
    }

    /**
     *  @brief Computes the AI's moves and stores them into _battleMoves
     */
    void battle::getAIMoves( ) {
        switch( _AILevel ) {
            default:
            case 0: // Trivial AI
            {
                for( u8 i = 0; i < 2; ++i )if( canMove( OPPONENT, i ) )
                    _battleMoves[ i ][ OPPONENT ] = { battleMove::type::ATTACK, ACPKMN( i, OPPONENT ).m_boxdata.m_moves[ 0 ], 0 };
                break;
            }
        }
    }

    /**
     *  @brief Does all of the turn's moves.
     */
    void battle::doMoves( ) {
        //Mega evolve all PKMN first
        for( u8 i = 0; i < 4; ++i ) {
            bool isOpp = i % 2,
                isSnd = i / 2;

            if( _battleMoves[ isSnd ][ isOpp ].m_type == battleMove::MEGA_ATTACK ) {
                megaEvolve( isOpp, isSnd );
                _battleMoves[ isSnd ][ isOpp ].m_type = battleMove::ATTACK;
            }
        }

        orderPKMN( true );

        for( u8 p = 0; p < 4; ++p ) {
            doMove( p );
            registerParticipatedPKMN( );
        }
    }

    /**
     *  @brief Mega-evolves the specified PKMN, if possible.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    void battle::megaEvolve( bool p_opponent, u8 p_pokemonPos ) {
        if( !_allowMegaEvolution )
            return;

        //TODO
    }

    /**
     *  @brief Does the specified PKMN's move.
     */
    void battle::doMove( u8 p_moveNo ) {

        u8 opponent = -1;
        u8 pokemonPos = -1;

        for( opponent = 0; opponent < 2; ++opponent )
            for( pokemonPos = 0; pokemonPos < 2; ++pokemonPos )
                if( _moveOrder[ pokemonPos ][ opponent ] == p_moveNo )
                    goto OUT;
OUT:
        auto& acMove = _battleMoves[ pokemonPos ][ opponent ];

        if( !ACPKMN( pokemonPos, opponent ).m_stats.m_acHP )
            return;

        std::wstring acPkmnStr = L"";
        if( opponent )
            acPkmnStr = L"OPP" + ( pokemonPos + 1 );
        else
            acPkmnStr = L"OWN" + ( pokemonPos + 1 );

        switch( acMove.m_type ) {
            case battleMove::ATTACK:
            {
                auto acAttack = AttackList[ acMove.m_value ];
                if( acMove.m_target == 0 ) {
                    if( !opponent )
                        switch( acAttack->m_moveAffectsWhom ) {
                            case move::moveAffectsTypes::USER:
                                acMove.m_target = 1 + pokemonPos;
                                break;
                            case move::moveAffectsTypes::OWN_FIELD:
                                acMove.m_target = ( 1 << 4 );
                                break;
                            case move::moveAffectsTypes::OPPONENTS_FIELD:
                                acMove.m_target = ( 1 << 5 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES:
                                acMove.m_target = ( 1 << 2 ) | ( 1 << 3 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES_AND_PARTNER:
                                acMove.m_target = ( 1 << ( 1 - pokemonPos ) | ( 1 << 2 ) | ( 1 << 3 ) );
                                break;
                            case move::moveAffectsTypes::SELECTED:
                                acMove.m_target = ( 1 << 2 );
                                break;
                            case move::moveAffectsTypes::RANDOM:
                                acMove.m_target = ( 1 << ( rand( ) % 4 ) );
                                if( acMove.m_target == ( 1 << pokemonPos ) )
                                    acMove.m_target <<= 1;
                                break;
                            default:
                                break;
                    } else
                        switch( acAttack->m_moveAffectsWhom ) {
                            case move::moveAffectsTypes::USER:
                                acMove.m_target = ( ( 1 + pokemonPos ) << 2 );
                                break;
                            case move::moveAffectsTypes::OWN_FIELD:
                                acMove.m_target = ( 1 << 5 );
                                break;
                            case move::moveAffectsTypes::OPPONENTS_FIELD:
                                acMove.m_target = ( 1 << 4 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES:
                                acMove.m_target = ( 1 << 0 ) | ( 1 << 1 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES_AND_PARTNER:
                                acMove.m_target = ( 1 << 0 ) | ( 1 << 1 ) | ( 1 << ( 3 - pokemonPos ) );
                                break;
                            case move::moveAffectsTypes::SELECTED:
                                acMove.m_target = ( 1 << 0 );
                                break;
                            case move::moveAffectsTypes::RANDOM:
                                acMove.m_target = ( 1 << ( rand( ) % 4 ) );
                                if( acMove.m_target == ( 1 << ( 2 + pokemonPos ) ) )
                                    acMove.m_target >>= 1;
                                break;
                            default:
                                break;
                    }
                }
                doAttack( opponent, pokemonPos );
                break;
            }
            case battleMove::SWITCH:
                battle::switchPKMN( opponent, pokemonPos, acMove.m_value );
                break;
            case battleMove::USE_ITEM:
            {
                if( opponent ) {
                    std::swprintf( wbuffer, 100, L"[TRAINER] ([TCLASS]) setzt\n%s ein.[A]", ItemList[ acMove.m_value ]->getDisplayName( true ).c_str( ) );
                    log( wbuffer );
                }
                doItem( opponent, acMove.m_target, ability::abilityType( 0 ) );
                break;
            }
            case battleMove::USE_NAV:
                //TODO
                break;
            default:
                break;
        }
    }

    s16 battle::calcDamage( bool p_userIsOpp, u8 p_userPos, bool p_targetIsOpp, u8 p_targetPos ) {
        auto bm = _battleMoves[ p_userPos ][ p_userIsOpp ];

        //Calculate critical hit chance
        int mod = 16;
        switch( _criticalChance[ p_userPos ][ p_userIsOpp ] ) {
            case 1:
                mod = 8;
                break;
            case 2:
                mod = 4;
                break;
            case 3:
                mod = 3;
                break;
            case 4:
                mod = 2;
                break;
            default:
                break;
        }

        _acDamage[ p_targetPos ][ p_targetIsOpp ] = 0;

        auto move = AttackList[ bm.m_value ];

        if( move->m_moveHitType == move::STAT ) {
            //log( L"Move is a STS move[A]" );
            return ( _acDamage[ p_targetPos ][ p_targetIsOpp ] = 0 );
        }

        //Calculate effectivity
        _effectivity[ p_targetPos ][ p_targetIsOpp ] = getEffectiveness( move->m_moveType, ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 0 ] );
        if( ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 0 ] != ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 1 ] )
            _effectivity[ p_targetPos ][ p_targetIsOpp ] *= getEffectiveness( move->m_moveType, ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 1 ] );
        if( ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 2 ] != ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 1 ] )
            _effectivity[ p_targetPos ][ p_targetIsOpp ] *= getEffectiveness( move->m_moveType, ACPKMNSTR( p_targetPos, p_targetIsOpp ).m_Types[ 2 ] );

        //Calculate critical hit
        _critical[ p_targetPos ][ p_targetIsOpp ] = !( rand( ) % mod );
        //STAB
        float STAB = 1.0f;
        if( getType( ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_speciesId, 0 ) == move->m_moveType
            || getType( ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_speciesId, 1 ) == move->m_moveType )
            STAB = 1.5f;
        if( ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_ADAPTABILITY )
            STAB = 2.0f;

        //Weather
        float weather = 1.0f;

        bool weatherPossible = true;
        for( u8 a = 0; a < 1 + ( m_battleMode == DOUBLE ); ++a ) for( u8 b = 0; b < 2; ++b )
            weatherPossible &= ( ACPKMN( a, b ).m_boxdata.m_ability != A_AIR_LOCK
            && ACPKMN( a, b ).m_boxdata.m_ability != A_CLOUD_NINE );

        if( weatherPossible ) {
            if( m_weather == SUN && move->m_moveType == WASSER )
                weather = 0.5f;
            if( m_weather == HEAVY_SUNSHINE && move->m_moveType == WASSER )
                weather = 0.0f;

            if( m_weather == SUN && move->m_moveType == FEUER )
                weather = 1.5f;
            if( m_weather == HEAVY_SUNSHINE && move->m_moveType == FEUER )
                weather = 1.5f;

            if( m_weather == RAIN && move->m_moveType == FEUER )
                weather = 0.5f;
            if( m_weather == HEAVY_RAIN && move->m_moveType == FEUER )
                weather = 0.0f;

            if( m_weather == RAIN && move->m_moveType == WASSER )
                weather = 1.5f;
            if( m_weather == HEAVY_RAIN && move->m_moveType == WASSER )
                weather = 1.5f;
        }

        //Multi-Target modifier
        float target = 1.0f;
        if( move->m_moveAffectsWhom & ( 8 | 32 ) )
            target = 0.75f;

        //Base damage calculation
        auto moveAtkHitType = move->m_moveHitType;
        auto moveDefHitType = move->m_moveHitType;

        if( bm.m_value == M_PSYSHOCK )
            moveDefHitType = move::PHYS;
        if( bm.m_value == M_PSYSTRIKE )
            moveDefHitType = move::PHYS;
        if( bm.m_value == M_SECRET_SWORD )
            moveDefHitType = move::PHYS;

        float atk = ( ( moveAtkHitType == move::PHYS ) ? ACPKMN( p_userPos, p_userIsOpp ).m_stats.m_Atk : ACPKMN( p_userPos, p_userIsOpp ).m_stats.m_SAtk );
        float def = ( ( moveDefHitType == move::PHYS ) ? ACPKMN( p_targetPos, p_targetIsOpp ).m_stats.m_Def : ACPKMN( p_targetPos, p_targetIsOpp ).m_stats.m_SDef );

        //Burn and other status stuff
        if( ACPKMN( p_userPos, p_userIsOpp ).m_status.m_Burned &&
            ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability != A_GUTS &&
            moveAtkHitType == move::PHYS ) {
            atk /= 2;
        } else if( ACPKMN( p_userPos, p_userIsOpp ).m_statusint &&
                   ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_GUTS &&
                   moveAtkHitType == move::PHYS ) {
            atk *= 1.5;
        } else if( ACPKMN( p_userPos, p_userIsOpp ).m_status.m_Poisoned &&
                   ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_TOXIC_BOOST &&
                   moveAtkHitType == move::PHYS ) {
            atk *= 1.5;
        } else if( ACPKMN( p_userPos, p_userIsOpp ).m_status.m_Poisoned &&
                   ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_FLARE_BOOST &&
                   moveAtkHitType == move::SPEC ) {
            atk *= 1.5;
        }

        if( ACPKMN( p_targetPos, p_targetIsOpp ).m_statusint &&
            ACPKMN( p_targetPos, p_targetIsOpp ).m_boxdata.m_ability == A_MARVEL_SCALE &&
            moveDefHitType == move::PHYS ) {
            def *= 1.5;
        }

        //Stat changes
        if( ACPKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability != A_UNAWARE
            && bm.m_value != M_FOUL_PLAY ) {
            if( moveAtkHitType == move::PHYS ) {
                if( ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ ATK ] > 0 )
                    atk = atk * ( 2 + ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ ATK ] ) / 2.0;
                if( ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ ATK ] < 0 )
                    atk = atk  * 2.0 / ( 2 - ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ ATK ] );
            }
            if( moveAtkHitType == move::SPEC ) {
                if( ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ SATK ] > 0 )
                    atk = atk * ( 2 + ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ SATK ] ) / 2.0;
                if( ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ SATK ] < 0 )
                    atk = atk  * 2.0 / ( 2 - ACPKMNSTATCHG( p_userPos, p_userIsOpp )[ SATK ] );
            }
        }
        if( bm.m_value == M_FOUL_PLAY ) {
            atk = ACPKMN( p_targetPos, p_targetIsOpp ).m_stats.m_Atk;

            if( ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ ATK ] > 0 )
                atk = atk * ( 2 + ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ ATK ] ) / 2.0;
            if( ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ ATK ] < 0 )
                atk = atk  * 2.0 / ( 2 - ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ ATK ] );
        }

        if( ACPKMN( p_targetPos, p_targetIsOpp ).m_boxdata.m_ability != A_UNAWARE
            && bm.m_value != M_CHIP_AWAY ) {
            if( moveDefHitType == move::PHYS ) {
                if( ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ DEF ] > 0 )
                    def = def * ( 2 + ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ DEF ] ) / 2.0;
                if( ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ DEF ] < 0 )
                    def = def  * 2.0 / ( 2 - ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ DEF ] );
            }
            if( moveDefHitType == move::SPEC ) {
                if( ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] > 0 )
                    def = def * ( 2 + ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] ) / 2.0;
                if( ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] < 0 )
                    def = def  * 2.0 / ( 2 - ACPKMNSTATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] );
            }
        }

        _acDamage[ p_targetPos ][ p_targetIsOpp ] = s16( ( ( 2.0f * ACPKMN( p_userPos, p_userIsOpp ).m_Level + 10.0f ) / 250.0f )
                                                         * ( atk / def ) * s16( move->m_moveBasePower ) + 2 );

        float modifier = _effectivity[ p_targetPos ][ p_targetIsOpp ] * ( _critical[ p_targetPos ][ p_targetIsOpp ] ? 1.5f : 1.0f )
            * STAB * weather * target * ( ( 100 - rand( ) % 15 ) / 100.0 );
        _acDamage[ p_targetPos ][ p_targetIsOpp ] = s16( _acDamage[ p_targetPos ][ p_targetIsOpp ] * modifier );

        return _acDamage[ p_targetPos ][ p_targetIsOpp ];
    }

    /**
    *  @brief Applies all possible hold Items ordered by their PKMNs speed.
    *  @param p_situation: Current situation, on which an Item may be useable
    */
    void battle::doItems( ability::abilityType p_situation ) {
        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    doItem( j, i, p_situation );
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
    *  @brief Applies all possible hold items ordered by their PKMNs speed.
    *  @param p_opponent: true iff the next opponent's PKMN is requested.
    *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
    *  @param p_situation: Current situation, on which an item may be useable
    */
    void battle::doItem( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation ) {
        if( !ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP )
            return;

        auto im = *ItemList[ ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability ];

        if( ( im.getEffectType( ) & item::itemEffectType::IN_BATTLE ) && ( im.m_inBattleEffect & p_situation ) ) {
            std::swprintf( wbuffer, 50, L"%s von %ls%s wirkt.[A]",
                           im.getDisplayName( ).c_str( ),
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? "\n(Gegner)" : "\n" ) );

            log( wbuffer );
            im.m_inBattleScript.execute( *this, &( ACPKMN( p_pokemonPos, p_opponent ) ) );
            for( u8 k = 0; k < 4; ++k ) {
                bool isOpp = k % 2,
                    isSnd = k / 2;
                _battleUI->updateHP( isOpp, isSnd );
                _battleUI->updateStats( isOpp, isSnd );
            }
        }
    }

    /**
     *  @brief does the p_moveNo positioned attack of this turn
     *  @param p_moveNo: The number of the attack that shall be done.
     */
    void battle::doAttack( bool p_opponent, u8 p_pokemonPos ) {
        //Check if the user has already fainted
        if( ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP == 0
            || ACPKMNSTS( p_pokemonPos, p_opponent ) == KO )
            return;

        auto& bm = _battleMoves[ p_pokemonPos ][ p_opponent ];
        if( bm.m_type != battleMove::ATTACK )
            return;
        auto acMove = AttackList[ bm.m_value ];

        //Check if the user is frozen/asleep/paralyzed
        if( ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Frozen ) {
            if( ( rand( ) % 100 ) < 20 ) { //PKMN thaws
                std::swprintf( wbuffer, 100, L"%ls%s ist aufgetaut![A]",
                               ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
                ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Frozen = false;
            } else {
                std::swprintf( wbuffer, 100, L"%ls%s ist gefroren.[A]",
                               ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
                _battleUI->showStatus( p_opponent, p_pokemonPos );
                return;
            }
        }
        if( ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Paralyzed ) {
            if( ( rand( ) % 100 ) < 25 ) {
                std::swprintf( wbuffer, 100, L"%ls%s ist paralysiert.[A]",
                               ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
                _battleUI->showStatus( p_opponent, p_pokemonPos );
                return;
            }
        }
        if( ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Asleep ) {
            if( --ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Asleep ) {
                std::swprintf( wbuffer, 100, L"%ls%s bleibt schlafen.[A]",
                               ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
                //Check if the move can be used while the PKMN is asleep
                if( !( acMove->m_moveFlags & move::WHILE_ASLEEP ) ) {
                    _battleUI->showStatus( p_opponent, p_pokemonPos );
                    return;
                }
            } else {
                std::swprintf( wbuffer, 100, L"%ls%s ist aufgewacht![A]",
                               ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
            }
        }
        _battleUI->updateStatus( p_opponent, p_pokemonPos );


        std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]",
                       ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                       ( p_opponent ? " (Gegner)" : "" ),
                       acMove->m_moveName.c_str( ) );
        log( wbuffer );

        _lstMove = bm.m_value;
        if( p_opponent )
            _lstOppMove = _lstMove;
        else
            _lstOwnMove = _lstMove;
        _currentMoveIsOpp = p_opponent;
        _currentMoveIsSnd = p_pokemonPos;

        bool moveHasTarget = false;
        //Try to redirect the move if the original target has already fainted
        if( m_battleMode == DOUBLE ) {
            if( p_opponent ) {
                if( ( bm.m_target & ( 1 << 0 ) )
                    && ACPKMNSTS( 0, PLAYER ) == KO
                    && ACPKMNSTS( 1, PLAYER ) != KO )
                    bm.m_target |= ( 1 << 1 );
                else if( ( bm.m_target & ( 1 << 1 ) )
                         && ACPKMNSTS( 1, PLAYER ) == KO
                         && ACPKMNSTS( 0, PLAYER ) != KO )
                         bm.m_target |= ( 1 << 0 );
            } else {
                if( ( bm.m_target & ( 1 << 2 ) )
                    && ACPKMNSTS( 0, OPPONENT ) == KO
                    && ACPKMNSTS( 1, OPPONENT ) != KO )
                    bm.m_target |= ( 1 << 3 );
                else if( ( bm.m_target & ( 1 << 3 ) )
                         && ACPKMNSTS( 1, OPPONENT ) == KO
                         && ACPKMNSTS( 0, OPPONENT ) != KO )
                         bm.m_target |= ( 1 << 2 );
            }
        }

        u8 moveAccuracy = acMove->m_moveAccuracy;
        if( ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ACCURACY ] > 0 )
            moveAccuracy *= ( 2 + ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ACCURACY ] ) / 2.0;
        else if( ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ACCURACY ] < 0 )
            moveAccuracy *= 2.0 / ( 2 - ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ACCURACY ] );

        //For every target, check if that target protects itself
        for( u8 k = 0; k < 4; ++k ) {
            bool isOpp = k / 2,
                isSnd = k % 2;

            if( !( bm.m_target & ( 1 << k ) ) )
                continue;
            if( !_battleSpotOccupied[ isSnd ][ isOpp ] )
                continue;

            if( m_battleMode != DOUBLE && isSnd )
                continue;


            if( ACPKMNSTATCHG( isSnd, isOpp )[ EVASION ] > 0 )
                moveAccuracy *= 2.0 / ( 2 + ACPKMNSTATCHG( isSnd, isOpp )[ EVASION ] );
            else if( ACPKMNSTATCHG( isSnd, isOpp )[ EVASION ] < 0 )
                moveAccuracy *= ( 2 - ACPKMNSTATCHG( isSnd, isOpp )[ EVASION ] ) / 2.0;

            //Check if the target is protected and if the move is affected by Protect
            if( ACPKMNSTR( isSnd, isOpp ).m_battleStatus == battleStatus::PROTECTED
                && ( acMove->m_moveFlags & move::PROTECT ) ) {
                bm.m_target &= ~( 1 << k );
                std::swprintf( wbuffer, 100, L"%ls%s bleibt unbeeindruckt.[A]",
                               ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
                moveHasTarget = true;
            }
            //Check if the move fails
            else if( moveAccuracy && ( s8( rand( ) % 100 ) < s8( 100 - moveAccuracy ) )
                     && ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_NO_GUARD ) {
                bm.m_target &= ~( 1 << k );
                std::swprintf( wbuffer, 100, L"%ls%s wich aus.[A]",
                               ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
                moveHasTarget = true;
            }
            //If the target is already ko, then it shouldn't be a target at all...
            else if( !ACPKMN( isSnd, isOpp ).m_stats.m_acHP ) {
                bm.m_target &= ~( 1 << k );
            }
            //Activate Items/Abilities before attack
            else {
                doItem( isOpp, isSnd, ability::BEFORE_ATTACK );
                doAbility( isOpp, isSnd, ability::BEFORE_ATTACK );
            }
        }

        //Attack animation
        _battleUI->showAttack( p_opponent, p_pokemonPos );

        //Reduce PP
        for( u8 i = 0; i < 4; ++i )
            if( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_moves[ i ] == bm.m_value ) {
                if( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_acPP[ i ] )
                    ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_acPP[ i ]--;
                break;
            }

        //Damage and stuff
        for( u8 k = 0; k < 4; ++k ) {
            bool isOpp = k / 2,
                isSnd = k % 2;

            if( !( bm.m_target & ( 1 << k ) ) ) {
                continue;
            }
            if( !_battleSpotOccupied[ isSnd ][ isOpp ] )
                continue;

            if( m_battleMode != DOUBLE && isSnd )
                continue;

            if( !ACPKMN( isSnd, isOpp ).m_stats.m_acHP )
                continue;

            moveHasTarget = true;
        }
        for( u8 k = 0; k < 4; ++k ) {
            bool isOpp = k / 2,
                isSnd = k % 2;

            if( !( bm.m_target & ( 1 << k ) ) ) {
                continue;
            }
            if( !_battleSpotOccupied[ isSnd ][ isOpp ] )
                continue;

            if( m_battleMode != DOUBLE && isSnd )
                continue;

            if( !ACPKMN( isSnd, isOpp ).m_stats.m_acHP )
                continue;

            moveHasTarget = true;

            //Calculate damage against the target
            calcDamage( p_opponent, p_pokemonPos, isOpp, isSnd ); //If the damage is negative, then it heals the target

            //Check for abilities/ items on the target
            doItem( isOpp, isSnd, ability::ATTACK );
            doAbility( isOpp, isSnd, ability::ATTACK );

            ACPKMN( isSnd, isOpp ).m_stats.m_acHP = (u16)std::max( s16( 0 ),
                                                                   std::min( s16( ACPKMN( isSnd, isOpp ).m_stats.m_acHP - _acDamage[ isSnd ][ isOpp ] ),
                                                                   (s16)ACPKMN( isSnd, isOpp ).m_stats.m_maxHP ) );

            _battleUI->updateHP( isOpp, isSnd );
            if( acMove->m_moveHitType != move::STAT ) {
                if( _critical[ isSnd ][ isOpp ] )
                    log( L"[COLR:15:15:00]Ein Volltreffer![A][CLEAR][COLR:00:00:00]" );
                if( _effectivity[ isSnd ][ isOpp ] != 1.0f ) {
                    float effectivity = _effectivity[ isSnd ][ isOpp ];
                    if( effectivity > 3.0f )
                        std::swprintf( wbuffer, 100, L"[COLR:00:31:00]Das ist enorm effektiv\ngegen %ls![A][CLEAR][COLR:00:00:00]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                    else if( effectivity > 1.0f )
                        std::swprintf( wbuffer, 100, L"[COLR:00:15:00]Das ist sehr effektiv\ngegen %ls![A][CLEAR][COLR:00:00:00]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                    else if( effectivity == 0.0f )
                        std::swprintf( wbuffer, 100, L"[COLR:31:00:00]Hat die Attacke\n%ls getroffen?[A][CLEAR][COLR:00:00:00]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                    else if( effectivity < 0.3f )
                        std::swprintf( wbuffer, 100, L"[COLR:31:00:00]Das ist nur enorm wenig\neffektiv gegen %ls...[A][CLEAR][COLR:00:00:00]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                    else if( effectivity < 1.0f )
                        std::swprintf( wbuffer, 100, L"[COLR:15:00:00]Das ist nicht sehr effektiv\ngegen %ls.[A][CLEAR][COLR:00:00:00]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                    log( wbuffer );
                }
            }
            //Check if PKMN fainted
            if( !ACPKMN( isSnd, isOpp ).m_stats.m_acHP && _battleSpotOccupied[ isSnd ][ isOpp ] ) {
                handleFaint( isOpp, isSnd );
            } else {
                //Check if an attack effect triggers
                if( ( rand( ) % 100 ) < acMove->m_moveEffectAccuracy ) {
                    acMove->m_moveEffect.execute( *this, &ACPKMN( p_pokemonPos, p_opponent ) );

                    for( u8 s = 0; s < MAX_STATS; s++ )
                        ACPKMNSTATCHG( isSnd, isOpp )[ s ] += _acStatChange[ isSnd ][ isOpp ][ s ];
                    _battleUI->updateStats( isOpp, isSnd );
                }
            }

            //Check for items/abilities
            doItem( isOpp, isSnd, ability::AFTER_ATTACK );
            doAbility( isOpp, isSnd, ability::AFTER_ATTACK );
            //Check if any PKMN on the field fainted

            for( u8 j = 0; j < 4; ++j )
                handleFaint( j / 2, j % 2 );
        }
        if( !moveHasTarget ) {
            log( L"Es schlug fehl...[A]" );
        }
        return;
    }
    /**
    *  @brief Handles special condition damage between turns
    */
    void battle::handleSpecialConditions( bool p_opponent, u8 p_pokemonPos ) {
        if( ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Burned ) {
            std::swprintf( wbuffer, 100, L"Die Verbrennung schadet\n%ls%s.[A]",
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? " (Gegner)" : "" ) );
            log( wbuffer );

            ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP
                = std::max( u16( 0 ), u16( ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP - 1.0 / ( 8 * ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_maxHP ) ) );

            _battleUI->updateHP( p_opponent, p_pokemonPos );
        }
        if( ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Poisoned
            && ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_POISON_HEAL ) {
            std::swprintf( wbuffer, 100, L"Die Vergiftung schadet\n%ls%s.[A]",
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? " (Gegner)" : "" ) );
            log( wbuffer );

            ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP
                = std::max( u16( 0 ), u16( ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP - 1.0 / ( 8 * ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_maxHP ) ) );

            _battleUI->updateHP( p_opponent, p_pokemonPos );
        }
        if( ACPKMN( p_pokemonPos, p_opponent ).m_status.m_Toxic
            && ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_POISON_HEAL ) {
            std::swprintf( wbuffer, 100, L"Die Vergiftung schadet\n%ls%s.[A]",
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? " (Gegner)" : "" ) );
            log( wbuffer );

            ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP
                = std::max( u16( 0 ), u16( ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP
                - ( ++ACPKMNSTR( p_pokemonPos, p_opponent ).m_toxicCount ) / ( 16.0 * ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_maxHP ) ) );

            _battleUI->updateHP( p_opponent, p_pokemonPos );
        }
    }

    void battle::handleFaint( bool p_opponent, u8 p_pokemonPos, bool p_show ) {
        if( m_battleMode != DOUBLE && p_pokemonPos )
            return;
        if( !ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP && _battleSpotOccupied[ p_pokemonPos ][ p_opponent ] ) {
            if( p_show ) {
                std::swprintf( wbuffer, 100, L"%ls%s wurde besiegt.[A]",
                               ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                               ( p_opponent ? " (Gegner)" : "" ) );
                log( wbuffer );
            }
            ACPKMNSTS( p_pokemonPos, p_opponent ) = KO;
            _battleUI->hidePKMN( p_opponent, p_pokemonPos );
            _battleSpotOccupied[ p_pokemonPos ][ p_opponent ] = false;

            if( m_distributeEXP )
                distributeEXP( p_opponent, p_pokemonPos );
        }
    }

    /**
     *  @brief Registers the current participating PKMN
     */
    void battle::registerParticipatedPKMN( ) {
        if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
            && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
            _participatedPKMN[ ACPOS( 0, PLAYER ) ][ PLAYER ] |= ( 1 << ACPOS( 0, OPPONENT ) );
        if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
            && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
            _participatedPKMN[ ACPOS( 0, OPPONENT ) ][ OPPONENT ] |= ( 1 << ACPOS( 0, PLAYER ) );

        if( m_battleMode == DOUBLE ) {
            if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ ACPOS( 0, PLAYER ) ][ PLAYER ] |= ( 1 << ACPOS( 1, OPPONENT ) );
            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
                _participatedPKMN[ ACPOS( 0, OPPONENT ) ][ OPPONENT ] |= ( 1 << ACPOS( 1, PLAYER ) );

            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
                _participatedPKMN[ ACPOS( 1, PLAYER ) ][ PLAYER ] |= ( 1 << ACPOS( 0, OPPONENT ) );
            if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ ACPOS( 1, OPPONENT ) ][ OPPONENT ] |= ( 1 << ACPOS( 0, PLAYER ) );

            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ ACPOS( 1, PLAYER ) ][ PLAYER ] |= ( 1 << ACPOS( 1, OPPONENT ) );
            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ ACPOS( 1, OPPONENT ) ][ OPPONENT ] |= ( 1 << ACPOS( 1, PLAYER ) );
        }
    }

    /**
    *  @brief Distrobutes EXP.
    *  @param p_opponent: true iff the opponent's PKMN fainted.
    *  @param p_pokemonPos: Position of the fainted PKMN (0 or 1)
    */
    void battle::distributeEXP( bool p_opponent, u8 p_pokemonPos ) {
        u8 receivingPKMN = _participatedPKMN[ ACPOS( p_pokemonPos, p_opponent ) ][ p_opponent ];
        _participatedPKMN[ ACPOS( p_pokemonPos, p_opponent ) ][ p_opponent ] = 0;

        float wildModifer = m_isWildBattle ? 1 : 1.5;
        pokemonData p;
        getAll( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, p );
        u16 b = p.m_EXPYield;

        bool printPkmnMsg = false;

        for( u8 i = 0; i < 6; ++i ) {
            u8 sz = ( !p_opponent ) ? _opponent.m_pkmnTeam.size( ) : _player.m_pkmnTeam.size( );

            if( sz <= i )
                break;
            if( !( receivingPKMN & ( 1 << ( ACPOS( i, PLAYER ) ) ) ) )
                continue;

            u8 acidx = 42;
            for( int j = 0; j < sz; ++j )
                if( ACPOS( j, !p_opponent ) == i ) {
                    acidx = j;
                    break;
                }
            if( acidx == 42 )
                continue;

            auto& acPkmn = ACPKMN( acidx, !p_opponent );

            if( acPkmn.m_stats.m_acHP ) {
                if( acPkmn.m_Level == 100 )
                    continue;
                float e = ( acPkmn.m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                u8 L = acPkmn.m_Level;

                float t = ( acPkmn.m_boxdata.m_oTId == FS::SAV->m_id && acPkmn.m_boxdata.m_oTSid == FS::SAV->m_sid ? 1 : 1.5 );

                u32 exp = u32( ( wildModifer * t* b* e* L ) / 7 );

                acPkmn.m_boxdata.m_experienceGained += exp;

                //Distribute EV
                auto acItem = acPkmn.m_boxdata.m_holdItem;
                auto hasPKRS = acPkmn.m_boxdata.m_pokerus;

                u8 multiplier = ( 1 << ( hasPKRS + ( acItem == I_MACHO_BRACE ) ) );

                //Check whether the PKMN can still obtain EV

                u16 evsum = 0;
                for( u8 j = 0; j < 6; ++j )
                    evsum += acPkmn.m_boxdata.m_effortValues[ j ];
                if( evsum >= 510 )
                    continue;

                if( acPkmn.m_boxdata.m_effortValues[ 0 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2 + 4 * ( acItem == I_POWER_WEIGHT ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 1 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4 * ( acItem == I_POWER_BRACER ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 2 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * ( acItem == I_POWER_BELT ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 3 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2 + 4 * ( acItem == I_POWER_ANKLET ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 4 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4 * ( acItem == I_POWER_LENS ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 5 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2 + 4 * ( acItem == I_POWER_BAND ) ) );

                if( acidx < 1 + ( m_battleMode == DOUBLE ) )
                    _battleUI->applyEXPChanges( !p_opponent, acidx, exp ); // Checks also for level-advancement of 1st (and in Doubles 2nd) PKMN
                else { //Advance the level here (this is NOT redundant boilerplate!

                    printPkmnMsg = true;


                    getAll( acPkmn.m_boxdata.m_speciesId, p );

                    bool newLevel = EXP[ L ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                    u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;

                    while( newLevel ) {
                        acPkmn.m_Level++;

                        if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                            acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ]
                            + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPkmn.m_Level / 100 ) + 10;
                        else
                            acPkmn.m_stats.m_maxHP = 1;
                        pkmnNatures nature = acPkmn.m_boxdata.getNature( );

                        acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2 * p.m_bases[ ATK + 1 ]
                            + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 ) * NatMod[ nature ][ ATK ];
                        acPkmn.m_stats.m_Def = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                            + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ DEF ];
                        acPkmn.m_stats.m_Spd = ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                            + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SPD ];
                        acPkmn.m_stats.m_SAtk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                            + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SATK ];
                        acPkmn.m_stats.m_SDef = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                            + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SDEF ];

                        acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

                        std::swprintf( wbuffer, 50, L"%ls%s erreicht\nLevel %d.[A]", acPkmn.m_boxdata.m_name,
                                       ( ( !p_opponent ) ? " (Gegner)" : "" ), acPkmn.m_Level );
                        log( wbuffer );

                        checkForAttackLearn( i );
                        newLevel = acPkmn.m_Level < 100 && EXP[ acPkmn.m_Level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                    }
                }
            }
        }

        if( printPkmnMsg )
            log( L"Weiter PKMN im Team\n erhalten E.-Punkte.[A]" );

        if( FS::SAV->m_EXPShareEnabled && p_opponent ) {
            log( L"Der EP-Teiler wirkt![A]" );
            for( u8 i = ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); i < 6; ++i )if( ACPKMNSTS( i, PLAYER ) != KO &&
                                                                                ACPKMNSTS( i, PLAYER ) != NA ) {

                if( ACPKMN( i, PLAYER ).m_Level == 100 )
                    continue;
                if( receivingPKMN & ( 1 << ( ACPOS( i, PLAYER ) ) ) )
                    continue;

                auto& acPkmn = ACPKMN( i, PLAYER );

                float e = ( acPkmn.m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                u8 L = acPkmn.m_Level;

                float t = ( acPkmn.m_boxdata.m_oTId == FS::SAV->m_id && acPkmn.m_boxdata.m_oTSid == FS::SAV->m_sid ? 1 : 1.5 );

                u32 exp = u32( ( wildModifer * t* b* e* L ) / 7 );

                //Half values through EXP-Share
                acPkmn.m_boxdata.m_experienceGained += exp / 2;

                //Distribute EV
                //Check for EV-enhancing stuff
                auto acItem = acPkmn.m_boxdata.m_holdItem;
                auto hasPKRS = acPkmn.m_boxdata.m_pokerus;

                u8 multiplier = ( 1 << ( hasPKRS + ( acItem == I_MACHO_BRACE ) ) );

                //Check whether the PKMN can still obtain EV

                u16 evsum = 0;
                for( u8 j = 0; j < 6; ++j )
                    evsum += acPkmn.m_boxdata.m_effortValues[ j ];
                if( evsum >= 510 )
                    continue;

                if( acPkmn.m_boxdata.m_effortValues[ 0 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2 + 4 * ( acItem == I_POWER_WEIGHT ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 1 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4 * ( acItem == I_POWER_BRACER ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 2 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * ( acItem == I_POWER_BELT ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 3 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2 + 4 * ( acItem == I_POWER_ANKLET ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 4 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4 * ( acItem == I_POWER_LENS ) ) );
                if( acPkmn.m_boxdata.m_effortValues[ 5 ] <= u8( 252 ) )
                    acPkmn.m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2 + 4 * ( acItem == I_POWER_BAND ) ) );

                //Check for level-advancing

                getAll( acPkmn.m_boxdata.m_speciesId, p );

                bool newLevel = EXP[ L ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;

                while( newLevel ) {
                    acPkmn.m_Level++;

                    if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                        acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPkmn.m_Level / 100 ) + 10;
                    else
                        acPkmn.m_stats.m_maxHP = 1;
                    pkmnNatures nature = acPkmn.m_boxdata.getNature( );

                    acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2 * p.m_bases[ ATK + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 ) * NatMod[ nature ][ ATK ];
                    acPkmn.m_stats.m_Def = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ DEF ];
                    acPkmn.m_stats.m_Spd = ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SPD ];
                    acPkmn.m_stats.m_SAtk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SATK ];
                    acPkmn.m_stats.m_SDef = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SDEF ];

                    acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

                    std::swprintf( wbuffer, 50, L"%ls erreicht Level %d.[A]", acPkmn.m_boxdata.m_name, acPkmn.m_Level );
                    log( wbuffer );

                    checkForAttackLearn( i );

                    newLevel = acPkmn.m_Level < 100 && EXP[ acPkmn.m_Level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                }
            }
        }
    }

    /**
     *  @brief Checks if the own PKMN at position p_pokemonPos
     *  @param p_pokemonPos: Position of the PKMN which coul learn new moves (0 to 5)
     */
    void battle::checkForAttackLearn( u8 p_pokemonPos ) {
        const int MAX_ATTACKS_PER_LEVEL = 10;
        u16 learnable[ MAX_ATTACKS_PER_LEVEL ];
        auto& acPkmn = ACPKMN( p_pokemonPos, PLAYER );

        getLearnMoves( acPkmn.m_boxdata.m_speciesId, acPkmn.m_Level, acPkmn.m_Level, 1, MAX_ATTACKS_PER_LEVEL, learnable );

        for( u8 i = 0; i < MAX_ATTACKS_PER_LEVEL; ++i ) {
            if( !learnable[ i ] )
                break;
            _battleUI->learnMove( p_pokemonPos, learnable[ i ] );
        }

    }

    /**
    *  @brief Tests if the specified PKMN can perform an evolution, and performs it, if possible.
    *  @param p_opponent: true iff the opponent's PKMN shall be tested.
    *  @param p_pokemonPos: Position of the PKMN to be tested (0 or 1)
    */
    void battle::checkForEvolution( bool p_opponent, u8 p_pokemonPos ) {
        if( !FS::SAV->m_evolveInBattle )
            return;

        if( ACPKMN( p_pokemonPos, p_opponent ).canEvolve( ) ) {
            auto& acPkmn = ACPKMN( p_pokemonPos, p_opponent );

            std::swprintf( wbuffer, 50, L"%ls entwickelt sich...[A]", acPkmn.m_boxdata.m_name );
            log( wbuffer );

            acPkmn.evolve( );
            _battleUI->evolvePKMN( p_opponent, p_pokemonPos );

            std::swprintf( wbuffer, 50, L"und wurde zu einem %ls![A]", acPkmn.m_boxdata.m_name );
            log( wbuffer );
        }
    }

    /**
     *  @brief Checks if the battle has to end
     *  @param p_battleEndReason [in/out]: Stores the reason for an end of the battle.
     *  @returns True iff the battle has to end
     */
    bool battle::endConditionHit( battleEndReason& p_battleEndReason ) {
        //Check round limit
        if( _round >= _maxRounds ) {
            p_battleEndReason = battleEndReason::ROUND_LIMIT;
            return true;
        }

        //Check amount of non-koed PKMN
        //PLAYER
        u8 pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( _player.m_pkmnTeam.size( ) > i ) {
                if( ACPKMNSTS( i, PLAYER ) != KO
                    && ACPKMNSTS( i, PLAYER ) != NA
                    && ACPKMN( i, PLAYER ).m_stats.m_acHP )
                    pkmnCnt++;
            } else
                break;
        }
        if( !pkmnCnt ) {
            p_battleEndReason = battleEndReason::OPPONENT_WON;
            return true;
        }

        //OPPONENT
        pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( _opponent.m_pkmnTeam.size( ) > i ) {
                if( ACPKMNSTS( i, OPPONENT ) != KO
                    && ACPKMNSTS( i, OPPONENT ) != NA
                    && ACPKMN( i, OPPONENT ).m_stats.m_acHP )
                    pkmnCnt++;
            } else
                break;
        }
        if( !pkmnCnt ) {
            p_battleEndReason = battleEndReason::PLAYER_WON;
            return true;
        }
        return false;
    }

    /**
     *  @brief Ends the battle.
     *  @param p_battleEndReason: Reason for the end of battle.
     */
    void battle::endBattle( battleEndReason p_battleEndReason ) {
        switch( p_battleEndReason ) {
            case BATTLE::battle::ROUND_LIMIT:
                log( L"Das Rundenlimit dieses\nKampfes wurde erreicht.[A]" );
                _battleUI->showEndScreen( );
                log( L"Der Kampf endet in einem\nUnentschieden![A]" );
                break;
            case BATTLE::battle::OPPONENT_WON:
            {
                std::swprintf( wbuffer, 100, L"[TRAINER] [TCLASS] gewinnt...[A]" );

                _battleUI->showEndScreen( );

                std::swprintf( wbuffer, 100, L"%s[A]",
                               _opponent.getWinMsg( ) );
                log( wbuffer );
                break;
            }
            case BATTLE::battle::PLAYER_WON:
            {
                log( L"Du besiegst [TCLASS] [TRAINER]![A]" );

                _battleUI->showEndScreen( );

                std::swprintf( wbuffer, 100, L"%s[A]",
                               _opponent.getLooseMsg( ) );
                log( wbuffer );
                std::swprintf( wbuffer, 100, L"Du gewinnst %d$.[A]",
                               _opponent.getLooseMoney( ) );
                log( wbuffer );
                break;
            }
            case BATTLE::battle::RUN:
            {
                break;
            }
            default:
                log( L"Der Kampf endet.[A]" );
                break;
        }
    }

    /**
     *  @brief Switches the PKMN
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     *  @param p_newPokemonPos: The new PKMNs current Pos
     */
    void battle::switchPKMN( bool p_opponent, u8 p_pokemonPos, u8 p_newPokemonPos ) {
        if( p_opponent )
            std::swprintf( wbuffer, 100, L"[OPP%d] wurde von [TRAINER]\n([TCLASS]) auf die Bank geschickt.[A]", p_pokemonPos + 1 );
        else
            std::swprintf( wbuffer, 100, L"Auf die Bank [OWN%d]![A]", p_pokemonPos + 1 );
        log( wbuffer );

        _battleUI->hidePKMN( p_opponent, p_pokemonPos );

        std::swap( ACPOS( p_pokemonPos, p_opponent ), ACPOS( p_newPokemonPos, p_opponent ) );

        _battleUI->sendPKMN( p_opponent, p_pokemonPos );
    }

    /**
     *  @brief Applys weather effects.
     */
    void battle::doWeather( ) {
        if( m_weather != NO_WEATHER ) {
            if( --_weatherLength ) {
                log( _weatherMessage[ m_weather ] );
                _weatherEffects[ m_weather ].execute( *this, this );
            } else {
                log( _weatherEndMessage[ m_weather ] );
                m_weather = NO_WEATHER;
            }
        }
    }

    bool battle::run( ) {
        //Check whether run is succesful -- TODO
        _endBattle = true;
        return true;
    }

    battle::~battle( ) {
        delete _battleUI;
    }

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE
    //////////////////////////////////////////////////////////////////////////
}