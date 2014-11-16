/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : battle.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for Pokémon battles

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
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
#include "screenLoader.h"
#include "saveGame.h"
#include "bag.h"
#include "buffer.h"
#include "fs.h"
#include "sprite.h"

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
    std::wstring parseLogCmd( battle& p_battle, const std::wstring& p_cmd ) {
        if( p_cmd == L"A" )
            return L"`";
        if( p_cmd == L"CLEAR" )
            return L"";

        if( p_cmd == L"TRAINER" ) {
            std::swprintf( wbuffer, 50, L"%s", p_battle._opponent->m_battleTrainerName );
            return std::wstring( wbuffer );
        }
        if( p_cmd == L"TCLASS" ) {
            std::swprintf( wbuffer, 50, L"%s", trainerclassnames[ p_battle._opponent->m_trainerClass ] );
            return std::wstring( wbuffer );
        }
        if( p_cmd.substr( 0, 4 ) == L"COLR" ) {
            u8 r, g, b;

            r = 10 * C2I( p_cmd[ 5 ] ) + C2I( p_cmd[ 6 ] );
            g = 10 * C2I( p_cmd[ 8 ] ) + C2I( p_cmd[ 9 ] );
            b = 10 * C2I( p_cmd[ 11 ] ) + C2I( p_cmd[ 12 ] );

            p_battle._battleUI.setLogTextColor( RGB15( r, g, b ) );
            return L"";
        }

        POKEMON::pokemon target = ACPKMN2( p_battle, 0, PLAYER );
        bool isPkmn = false;
        bool isOpp = false;

        if( p_cmd.substr( 0, 4 ) == L"OWN1" ) {
            target = ACPKMN2( p_battle, 0, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OWN2" ) {
            target = ACPKMN2( p_battle, 1, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP1" ) {
            target = ACPKMN2( p_battle, 0, OPPONENT );
            isOpp = true;
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP2" ) {
            target = ACPKMN2( p_battle, 1, OPPONENT );
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
                std::swprintf( wbuffer, 50, L"%s", ITEMS::ItemList[ target.m_boxdata.m_holdItem ].m_itemName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"LEVEL" )
                return ( L"" + target.m_Level );
        }

        return L"";
    }

    battle::battle( battleTrainer* p_player, battleTrainer* p_opponent, int p_maxRounds, int p_AILevel, battleMode p_battleMode ) {
        _maxRounds = p_maxRounds;
        _AILevel = p_AILevel;
        _player = p_player;
        _opponent = p_opponent;
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
                msg += parseLogCmd( *this, accmd );
                if( accmd == L"CLEAR" ) {
                    _battleUI.writeLogText( msg );
                    _battleUI.clearLogScreen( );
                }
            } else
                msg += p_message[ i ];
        }

        _battleUI.writeLogText( msg );
        _battleUI.clearLogScreen( );
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

            if( p1CanMove ) {
                _battleUI.declareBattleMove( 0, false );
                if( _endBattle ) {
                    endBattle( battleEnd = RUN );
                    return ( battleEnd );
                }
            } else
                log( L"[OWN1] kann nicht angreifen...[A]" );

            //If 1st action is RUN, the player has no choice for a second move
            if( _battleMoves[ 0 ][ PLAYER ].m_type != battleMove::RUN ) {
                if( m_battleMode == DOUBLE && canMove( PLAYER, 1 ) ) {
                    if( !_battleUI.declareBattleMove( 1, p1CanMove ) )
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

            for( u8 i = 0; i < 4; ++i ) {
                swprintf( wbuffer, 200, L"BM%hhu: Type %hhu\nValue: %hhu (%s)\nTarget: %hhu[A]", i,
                          _battleMoves[ i / 2 ][ i % 2 ].m_type,
                          _battleMoves[ i / 2 ][ i % 2 ].m_value,
                          AttackList[ _battleMoves[ i / 2 ][ i % 2 ].m_value ]->m_moveName.c_str( ),
                          _battleMoves[ i / 2 ][ i % 2 ].m_target );
                log( wbuffer );
            }

            doMoves( );

            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                break;
            }
            doWeather( );

            refillBattleSpots( true );
        }

        return battleEnd;
    }

    /**
     *  @brief Initialize the battle.
     */
    void battle::initBattle( ) {
        //Some basic initialization stuff
        _battleUI = battleUI( this );
        _battleUI.init( );

        for( u8 i = 0; i < 6; ++i ) {
            ACPOS( i, PLAYER ) = ACPOS( i, OPPONENT ) = i;
            for( u8 o = 0; o < MAX_STATS; ++o )
                ACPKMNSTATCHG( i, PLAYER )[ o ] = ACPKMNSTATCHG( i, OPPONENT )[ o ] = 0;
            if( _player->m_pkmnTeam->size( ) > i ) {
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
            if( _opponent->m_pkmnTeam->size( ) > i ) {
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

        _battleUI.trainerIntro( );

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

        for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
            if( !_battleSpotOccupied[ i ][ j ] ) {
                u8 nextSpot = i;
                if( !p_choice || j )
                    nextSpot = getNextPKMN( j, i );
                else
                    nextSpot = _battleUI.choosePKMN( i );

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

        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    if( !_battleSpotOccupied[ i ][ j ] ) {
                        if( p_send ) {
                            _battleUI.sendPKMN( j, i );
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
        u8 max = ( p_opponent ? _opponent->m_pkmnTeam->size( ) : _player->m_pkmnTeam->size( ) );

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

        std::vector< std::tuple<s16, u16, u16> > inits;
        for( u8 i = 0; i < ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); ++i ) {
            for( u8 j = 0; j < 2; ++j ) {
                u16 acSpd = ACPKMN( i, j ).m_stats.m_Spd;
                s8 movePr = ( ( _battleMoves[ i ][ j ].m_type == battleMove::ATTACK || _battleMoves[ i ][ j ].m_type == battleMove::MEGA_ATTACK ) ?
                              AttackList[ _battleMoves[ i ][ j ].m_value ]->m_movePriority : 0 );
                if( _battleMoves[ i ][ j ].m_type == battleMove::SWITCH )
                    movePr = 7;
                if( _battleMoves[ i ][ j ].m_type == battleMove::USE_ITEM )
                    movePr = 7;
                inits.push_back( std::tuple<s16, u16, u8>( p_includeMovePriority * movePr, acSpd, 2 * i + j ) );
            }
        }
        std::sort( inits.begin( ), inits.end( ), std::greater < std::tuple<s8, u16, u16> >( ) );

        for( u8 i = 0; i < inits.size( ); ++i ) {
            s16 _0 = 0;
            u16 _1 = 0;
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
        orderPKMN( );

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
        auto ab = ability( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability );

        if( ab.m_type & p_situation ) {
            std::swprintf( wbuffer, 50, L"%s von %ls%s wirkt.[A]",
                           ab.m_abilityName.c_str( ),
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? "\n(Gegner)" : "\n" ) );

            log( wbuffer );
            ab.m_effect.execute( *this, &( ACPKMN( p_pokemonPos, p_opponent ) ) );

            for( u8 k = 0; k < 4; ++k ) {
                bool isOpp = k % 2,
                    isSnd = k / 2;
                _battleUI.updateHP( isOpp, isSnd );
                _battleUI.updateStats( isOpp, isSnd );
            }
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
            for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    doMove( j, i );
                    goto NEXT;
                }
            }
NEXT:
            ;
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
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    void battle::doMove( bool p_opponent, u8 p_pokemonPos ) {
        _acMove = _moveOrder[ p_pokemonPos ][ p_opponent ];

        std::wstring acPkmnStr = L"";
        if( p_opponent )
            acPkmnStr = L"OPP" + ( p_pokemonPos + 1 );
        else
            acPkmnStr = L"OWN" + ( p_pokemonPos + 1 );
        auto& acMove = _battleMoves[ p_pokemonPos ][ p_opponent ];

        switch( acMove.m_type ) {
            case battleMove::ATTACK:
            {
                auto& acAttack = AttackList[ acMove.m_value ];
                if( acMove.m_target == 0 ) {
                    if( !p_opponent )
                        switch( acAttack->m_moveAffectsWhom ) {
                            case move::moveAffectsTypes::USER:
                                acMove.m_target = 1 + p_pokemonPos;
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
                                acMove.m_target = ( 1 << ( p_pokemonPos ) | ( 1 << 2 ) | ( 1 << 3 ) );
                                break;
                            case move::moveAffectsTypes::SELECTED:
                                acMove.m_target = ( 1 << 2 );
                                break;
                            case move::moveAffectsTypes::RANDOM:
                                acMove.m_target = ( 1 << ( rand( ) % 4 ) );
                                if( acMove.m_target == ( 1 << p_pokemonPos ) )
                                    acMove.m_target <<= 1;
                                break;
                            default:
                                break;
                    } else
                        switch( acAttack->m_moveAffectsWhom ) {
                            case move::moveAffectsTypes::USER:
                                acMove.m_target = ( ( 1 + p_pokemonPos ) << 2 );
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
                                acMove.m_target = ( 1 << 0 ) | ( 1 << 1 ) | ( 1 << ( 2 + p_pokemonPos ) );
                                break;
                            case move::moveAffectsTypes::SELECTED:
                                acMove.m_target = ( 1 << 0 );
                                break;
                            case move::moveAffectsTypes::RANDOM:
                                acMove.m_target = ( 1 << ( rand( ) % 4 ) );
                                if( acMove.m_target == ( 1 << ( 2 + p_pokemonPos ) ) )
                                    acMove.m_target >>= 1;
                                break;
                            default:
                                break;
                    }
                }

                calcDamage( p_opponent, p_pokemonPos, rand( ) % 16 );

                doAbilities( ability::BEFORE_ATTACK );
                doItems( ability::BEFORE_ATTACK );

                doAttack( _acMove );

                doAbilities( ability::AFTER_ATTACK );
                doItems( ability::AFTER_ATTACK );

                break;
            }
            case battleMove::SWITCH:
                battle::switchPKMN( p_opponent, p_pokemonPos, acMove.m_value );
                break;
            case battleMove::USE_ITEM:
            {
                if( p_opponent )
                    std::swprintf( wbuffer, 100, L"[TRAINER] ([TCLASS]) setzt\n%s ein.[A]", ITEMS::ItemList[ acMove.m_value ].getDisplayName( true ).c_str( ) );
                doItem( p_opponent, acMove.m_target, ability::abilityType( 0 ) );
                break;
            }
            case battleMove::USE_NAV:
                //TODO
                break;
            default:
                break;
        }
    }

    /**
    *  @brief Calculates the current move's dealt damage.
    *  @param p_randInt: A random integer between 0 and 15 used to adjust the dealt damage
    */
    void battle::calcDamage( bool p_opponent, u8 p_pokemonPos, u8 p_randInt ) {
        //swprintf( wbuffer, 100, L"Calcin for PKMN\n%ls[A]", ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name );
        //log( wbuffer );

        auto bm = _battleMoves[ p_pokemonPos ][ p_opponent ];

        //Calculate critical hit chance
        int mod = 16;
        switch( _criticalChance[ p_pokemonPos ][ p_opponent ] ) {
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

        for( u8 k = 0; k < 4; ++k ) {
            bool isOpp = k % 2,
                isSnd = k / 2;
            _acDamage[ isSnd ][ isOpp ] = 0;

            //swprintf( wbuffer, 100, L"Calcin for target\n%ls[A]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
            //log( wbuffer );

            if( !( bm.m_target & ( 1 << k ) ) ) {
                //log( L"Not a Target[A]" );
                continue;
            }
            auto move = AttackList[ bm.m_value ];

            if( move->m_moveHitType == move::STAT ) {
                //log( L"Move is a STS move[A]" );
                continue;
            }

            POKEMON::PKMNDATA::pokemonData pd;
            POKEMON::PKMNDATA::getAll( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, pd );

            //Calculate effectivity
            _effectivity[ isSnd ][ isOpp ] = getEffectiveness( move->m_moveType, pd.m_types[ 0 ] );
            if( pd.m_types[ 0 ] != pd.m_types[ 1 ] )
                _effectivity[ isSnd ][ isOpp ] *= getEffectiveness( move->m_moveType, pd.m_types[ 1 ] );

            //Calculate critical hit
            if( p_randInt <= 15 )
                _critical[ isSnd ][ isOpp ] = !( rand( ) % mod );
            else {
                _critical[ isSnd ][ isOpp ] = false;

                if( p_randInt == 16 )
                    p_randInt = 0;
                else
                    p_randInt = 15;
            }
            //STAB
            float STAB = 1.0f;
            if( POKEMON::PKMNDATA::getType( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, 0 ) == move->m_moveType
                || POKEMON::PKMNDATA::getType( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, 1 ) == move->m_moveType )
                STAB = 1.5f;
            if( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability == A_ADAPTABILITY )
                STAB = 2.0f;

            //Weather
            float weather = 1.0f;

            bool weatherPossible = true;
            for( u8 a = 0; a < 2; ++a ) for( u8 b = 0; b < 2; ++b )
                weatherPossible &= ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_AIR_LOCK
                && ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_CLOUD_NINE );

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

            //Burn
            float burn = 1.0f;
            if( ACPKMNAIL( p_pokemonPos, p_opponent ) == move::ailment::BURN
                && move->m_moveHitType == move::PHYS
                && ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_GUTS ) {
                burn = 0.5f;
            }

            //Base damage calculation
            auto moveAtkHitType = move->m_moveHitType;
            auto moveDefHitType = move->m_moveHitType;

            if( bm.m_value == M_PSYSHOCK )
                moveDefHitType = move::PHYS;
            if( bm.m_value == M_PSYSTRIKE )
                moveDefHitType = move::PHYS;
            if( bm.m_value == M_SECRET_SWORD )
                moveDefHitType = move::PHYS;

            float atk = ( ( moveAtkHitType == move::PHYS ) ? ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_Atk : ACPKMN( p_pokemonPos, p_opponent ).m_stats.m_SAtk );
            if( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability != A_UNAWARE
                && bm.m_value != M_FOULPLAY ) {
                if( moveAtkHitType == move::PHYS ) {
                    if( ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ATK ] > 0 )
                        atk = atk * ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ATK ] / 2.0;
                    if( ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ATK ] < 0 )
                        atk = atk  * 2.0 / ( -ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ATK ] );
                }
                if( moveAtkHitType == move::SPEC ) {
                    if( ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ SATK ] > 0 )
                        atk = atk * ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ SATK ] / 2.0;
                    if( ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ SATK ] < 0 )
                        atk = atk  * 2.0 / ( -ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ SATK ] );
                }
            }
            if( bm.m_value == M_FOULPLAY ) {
                atk = ACPKMN( isSnd, isOpp ).m_stats.m_Atk;

                if( ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] > 0 )
                    atk = atk * ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] / 2.0;
                if( ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] < 0 )
                    atk = atk  * 2.0 / ( -ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] );
            }

            float def = ( ( moveDefHitType == move::PHYS ) ? ACPKMN( isSnd, isOpp ).m_stats.m_Def : ACPKMN( isSnd, isOpp ).m_stats.m_SDef );
            if( ACPKMN( isSnd, isOpp ).m_boxdata.m_ability != A_UNAWARE
                && bm.m_value != M_CHIP_AWAY ) {
                if( moveDefHitType == move::PHYS ) {
                    if( ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] > 0 )
                        atk = atk * ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] / 2.0;
                    if( ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] < 0 )
                        atk = atk  * 2.0 / ( -ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] );
                }
                if( moveDefHitType == move::SPEC ) {
                    if( ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] > 0 )
                        atk = atk * ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] / 2.0;
                    if( ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] < 0 )
                        atk = atk  * 2.0 / ( -ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] );
                }
            }

            //swprintf( wbuffer, 100, L"Damage init %hd\n%.3f %.3f %hhd[A]", _acDamage[ isSnd ][ isOpp ],
            //          ( ( 2.0f * ACPKMN( p_pokemonPos, p_opponent ).m_Level + 10.0f ) / 250.0f ),
            //          ( atk * 1.0f / def ),
            //          move->m_moveBasePower );
            //log( wbuffer );

            _acDamage[ isSnd ][ isOpp ] = s16( ( ( 2.0f * ACPKMN( p_pokemonPos, p_opponent ).m_Level + 10.0f ) / 250.0f ) * ( atk * 1.0f / def ) * move->m_moveBasePower + 2 );

            //swprintf( wbuffer, 100, L"Damage bef mod %hd\n%.3f %.3f %.3f\n%.3f %.3f %.3f[A]", _acDamage[ isSnd ][ isOpp ],
            //          _effectivity[ isSnd ][ isOpp ],
            //          ( _critical[ isSnd ][ isOpp ] ? 2.0f : 1.0f ),
            //          STAB,
            //          weather,
            //          target,
            //          burn );
            //log( wbuffer );

            float modifier = _effectivity[ isSnd ][ isOpp ] * ( _critical[ isSnd ][ isOpp ] ? 2.0f : 1.0f ) * STAB * weather * target * burn;
            _acDamage[ isSnd ][ isOpp ] = s16( _acDamage[ isSnd ][ isOpp ] * modifier );

            //swprintf( wbuffer, 100, L"Damage aft mod %hd[A]", _acDamage[ isSnd ][ isOpp ] );
            //log( wbuffer );
        }
    }

    /**
    *  @brief Applies all possible hold Items ordered by their PKMNs speed.
    *  @param p_situation: Current situation, on which an Item may be useable
    */
    void battle::doItems( ability::abilityType p_situation ) {
        orderPKMN( );

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
        auto im = ITEMS::ItemList[ ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability ];

        if( ( im.getEffectType( ) & ITEMS::item::itemEffectType::IN_BATTLE ) && ( im.m_inBattleEffect & p_situation ) ) {
            std::swprintf( wbuffer, 50, L"%s von %ls%s wirkt.[A]",
                           im.getDisplayName( ).c_str( ),
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? "\n(Gegner)" : "\n" ) );

            log( wbuffer );
            im.m_inBattleScript.execute( *this, &( ACPKMN( p_pokemonPos, p_opponent ) ) );
            for( u8 k = 0; k < 4; ++k ) {
                bool isOpp = k % 2,
                    isSnd = k / 2;
                _battleUI.updateHP( isOpp, isSnd );
                _battleUI.updateStats( isOpp, isSnd );
            }
        }
    }

    /**
     *  @brief does the p_moveNo positioned attack of this turn
     *  @param p_moveNo: The number of the attack that shall be done.
     */
    void battle::doAttack( u8 p_moveNo ) {
        bool messagePrinted = false;
        for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
            if( _moveOrder[ i ][ j ] == p_moveNo ) {
                auto& bm = _battleMoves[ i ][ j ];
                if( bm.m_type != battleMove::ATTACK )
                    return;
                auto acMove = AttackList[ bm.m_value ];

                //"Preview" Attacks effect
                if( rand( ) % 100 < acMove->m_moveEffectAccuracy )
                    acMove->m_moveEffect.execute( *this, &ACPKMN( i, j ) );

                for( u8 k = 0; k < 4; ++k ) {
                    bool isOpp = k % 2,
                        isSnd = k / 2;

                    if( !( bm.m_target & ( 1 << k ) ) )
                        continue;
                    if( !_battleSpotOccupied[ isSnd ][ isOpp ] )
                        continue;

                    bool doesChange = !!_acDamage[ isSnd ][ isOpp ];

                    for( u8 s = 0; s < MAX_STATS; s++ )
                        doesChange |= !!_acStatChange[ isSnd ][ isOpp ][ s ];

                    if( !messagePrinted ) {
                        std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]",
                                       ( ACPKMN( i, j ).m_boxdata.m_name ),
                                       ( j ? " (Gegner)" : "" ),
                                       acMove->m_moveName.c_str( ) );
                        log( wbuffer );
                        messagePrinted = true;
                    }

                    //Check if the attack fails 
                    if( acMove->m_moveAccuracy && rand( ) * 1.0 / RAND_MAX > acMove->m_moveAccuracy / 100.0 ) {
                        std::swprintf( wbuffer, 100, L"%ls%s wich aus.[A]",
                                       ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                                       ( j ? " (Gegner)" : "" ) );
                        log( wbuffer );
                        goto NEXT;
                    }

                    if( doesChange ) {

                        ACPKMN( isSnd, isOpp ).m_stats.m_acHP = std::max( u16( 0 ),
                                                                          std::min( u16( ACPKMN( isSnd, isOpp ).m_stats.m_acHP - _acDamage[ isSnd ][ isOpp ] ),
                                                                          ACPKMN( isSnd, isOpp ).m_stats.m_maxHP ) );


                        for( u8 s = 0; s < MAX_STATS; s++ )
                            ACPKMNSTATCHG( isSnd, isOpp )[ s ] += _acStatChange[ isSnd ][ isOpp ][ s ];

                        _battleUI.showAttack( p_moveNo );
                        _battleUI.updateHP( isOpp, isSnd );
                        if( _critical[ isSnd ][ isOpp ] )
                            log( L"[COLR:15:15:00]Ein Volltreffer![A][COLR:15:15:15]" );
                        if( _effectivity[ isSnd ][ isOpp ] != 1.0f ) {
                            float effectivity = _effectivity[ isSnd ][ isOpp ];
                            if( effectivity > 3.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:00:31:00]Das ist enorm effektiv\ngegen %ls![A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity > 1.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:00:15:00]Das ist sehr effektiv\ngegen %ls![A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity == 0.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:31:00:00]Hat die Attacke\n%lsgetroffen?[A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity < 0.3f )
                                std::swprintf( wbuffer, 100, L"[COLR:31:00:00]Das ist nur enorm wenig\neffektiv gegen %ls...[A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity < 1.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:15:00:00]Das ist nicht sehr effektiv\ngegen %ls.[A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            log( wbuffer );
                        }

                        //Check if PKMN fainted
                        if( !ACPKMN( isSnd, isOpp ).m_stats.m_acHP ) {
                            std::swprintf( wbuffer, 100, L"%ls%s wurde besiegt.[A]",
                                           ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                                           ( j ? "(Gegner)" : "" ) );
                            log( wbuffer );
                            _battleUI.hidePKMN( isOpp, isSnd );
                            _battleSpotOccupied[ isSnd ][ isOpp ] = false;

                            ACPKMNSTS( isSnd, isOpp ) = KO;

                            if( m_distributeEXP )
                                distributeEXP( isSnd, isOpp );
                        } else
                            _battleUI.updateStats( isOpp, isSnd );
                    } else {
                        std::swprintf( wbuffer, 100, L"%ls%s bleibt von\nder Attacke unbeeindruckt...[A]",
                                       ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                                       ( isOpp ? "(Gegner)" : "" ) );
                        log( wbuffer );
                    }
                }
                for( u8 k = 4; k < 6; ++k ) {
                    if( !( bm.m_target & ( 1 << k ) ) )
                        continue;
                    if( !messagePrinted ) {
                        std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]",
                                       ( ACPKMN( i, j ).m_boxdata.m_name ),
                                       ( j ? "(Gegner)" : "" ),
                                       acMove->m_moveName.c_str( ) );
                        log( wbuffer );
                        messagePrinted = true;
                    }
                    //acMove->m_moveEffect.execute( *this, &( ACPKMN( i, j ) ) );
                }

                if( !messagePrinted ) {
                    std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]\nDie Attacke ging ins leere...[A]",
                                   ( ACPKMN( i, j ).m_boxdata.m_name ),
                                   ( j ? "(Gegner)" : "" ),
                                   acMove->m_moveName.c_str( ) );
                    log( wbuffer );
                    messagePrinted = true;
                }
NEXT:
                _lstMove = ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ];
                if( j )
                    _lstOppMove = _lstMove;
                else
                    _lstOwnMove = _lstMove;
                return;
            }
        }
    }

    /**
     *  @brief Registers the current participating PKMN
     */
    void battle::registerParticipatedPKMN( ) {
        if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
            && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
            _participatedPKMN[ &ACPKMN( 0, PLAYER ) ].insert( &ACPKMN( 0, OPPONENT ) );
        if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
            && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
            _participatedPKMN[ &ACPKMN( 0, OPPONENT ) ].insert( &ACPKMN( 0, PLAYER ) );

        if( m_battleMode == DOUBLE ) {
            if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 0, PLAYER ) ].insert( &ACPKMN( 1, OPPONENT ) );
            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 0, OPPONENT ) ].insert( &ACPKMN( 1, PLAYER ) );

            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, PLAYER ) ].insert( &ACPKMN( 0, OPPONENT ) );
            if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, OPPONENT ) ].insert( &ACPKMN( 0, PLAYER ) );

            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, PLAYER ) ].insert( &ACPKMN( 1, OPPONENT ) );
            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, OPPONENT ) ].insert( &ACPKMN( 1, PLAYER ) );
        }
    }

    /**
    *  @brief Distrobutes EXP.
    *  @param p_opponent: true iff the opponent's PKMN fainted.
    *  @param p_pokemonPos: Position of the fainted PKMN (0 or 1)
    */
    void battle::distributeEXP( bool p_opponent, u8 p_pokemonPos ) {
        auto& receivingPKMN = _participatedPKMN[ &ACPKMN( p_pokemonPos, p_opponent ) ];

        float wildModifer = m_isWildBattle ? 1 : 1.5;
        POKEMON::PKMNDATA::pokemonData p;
        POKEMON::PKMNDATA::getAll( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, p );
        u16 b = p.m_EXPYield;

        for( auto i : receivingPKMN ) {
            if( i->m_stats.m_acHP ) {
                if( i->m_Level == 100 )
                    continue;
                float e = ( i->m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                u8 L = i->m_Level;

                float t = ( i->m_boxdata.m_oTId == SAV.m_Id && i->m_boxdata.m_oTSid == SAV.m_Sid ? 1 : 1.5 );

                u32 exp = u32( ( wildModifer * t* b* e* L ) / 7 );

                i->m_boxdata.m_experienceGained += exp;

                //Distribute EV
                auto acItem = i->m_boxdata.m_holdItem;
                auto hasPKRS = i->m_boxdata.m_pokerus;

                u8 multiplier = ( 1 << ( hasPKRS + ( acItem == I_MACHO_BRACE ) ) );

                //Check whether the PKMN can still obtain EV

                u16 evsum = 0;
                for( u8 j = 0; j < 6; ++j )
                    evsum += i->m_boxdata.m_effortValues[ j ];
                if( evsum >= 510 )
                    continue;

                if( i->m_boxdata.m_effortValues[ 0 ] <= u8( 252 ) )
                    i->m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2 + 4 * ( acItem == I_POWER_WEIGHT ) ) );
                if( i->m_boxdata.m_effortValues[ 1 ] <= u8( 252 ) )
                    i->m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4 * ( acItem == I_POWER_BRACER ) ) );
                if( i->m_boxdata.m_effortValues[ 2 ] <= u8( 252 ) )
                    i->m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * ( acItem == I_POWER_BELT ) ) );
                if( i->m_boxdata.m_effortValues[ 3 ] <= u8( 252 ) )
                    i->m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2 + 4 * ( acItem == I_POWER_ANKLET ) ) );
                if( i->m_boxdata.m_effortValues[ 4 ] <= u8( 252 ) )
                    i->m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4 * ( acItem == I_POWER_LENS ) ) );
                if( i->m_boxdata.m_effortValues[ 5 ] <= u8( 252 ) )
                    i->m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2 + 4 * ( acItem == I_POWER_BAND ) ) );
            }
        }

        _battleUI.applyEXPChanges( ); // Checks also for level-advancement of 1st (and in Doubles 2nd) PKMN

        if( SAV.m_EXPShareEnabled && !p_opponent ) {
            log( L"Der EP-Teiler wirkt![A]" );
            for( u8 i = ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); i < 6; ++i )if( ACPKMNSTS( i, PLAYER ) != KO &&
                                                                                ACPKMNSTS( i, PLAYER ) != NA ) {

                if( ACPKMN( i, PLAYER ).m_Level == 100 )
                    continue;

                auto& acPkmn = ACPKMN( i, PLAYER );

                float e = ( acPkmn.m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                u8 L = acPkmn.m_Level;

                float t = ( acPkmn.m_boxdata.m_oTId == SAV.m_Id && acPkmn.m_boxdata.m_oTSid == SAV.m_Sid ? 1 : 1.5 );

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

                POKEMON::PKMNDATA::getAll( acPkmn.m_boxdata.m_speciesId, p );

                bool newLevel = POKEMON::EXP[ L ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;

                while( newLevel ) {
                    acPkmn.m_Level++;

                    if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                        acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPkmn.m_Level / 100 ) + 10;
                    else
                        acPkmn.m_stats.m_maxHP = 1;
                    POKEMON::pkmnNatures nature = acPkmn.m_boxdata.getNature( );

                    acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2 * p.m_bases[ ATK + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 ) * POKEMON::NatMod[ nature ][ ATK ];
                    acPkmn.m_stats.m_Def = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ DEF ];
                    acPkmn.m_stats.m_Spd = ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ SPD ];
                    acPkmn.m_stats.m_SAtk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ SATK ];
                    acPkmn.m_stats.m_SDef = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ SDEF ];

                    acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

                    std::swprintf( wbuffer, 50, L"%ls erreicht Level %d.[A]", acPkmn.m_boxdata.m_name, acPkmn.m_Level );
                    log( wbuffer );

                    u8 oldSpec = acPkmn.m_boxdata.m_speciesId;
                    checkForAttackLearn( i );
                    checkForEvolution( PLAYER, i );
                    if( oldSpec != acPkmn.m_boxdata.m_speciesId )
                        checkForAttackLearn( i );

                    newLevel = acPkmn.m_Level < 100 && POKEMON::EXP[ acPkmn.m_Level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
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

        POKEMON::PKMNDATA::getLearnMoves( acPkmn.m_boxdata.m_speciesId, acPkmn.m_Level, acPkmn.m_Level, 1, MAX_ATTACKS_PER_LEVEL, learnable );

        for( u8 i = 0; i < MAX_ATTACKS_PER_LEVEL; ++i ) {
            if( !learnable[ i ] )
                break;
            _battleUI.learnMove( p_pokemonPos, learnable[ i ] );
        }

    }

    /**
    *  @brief Tests if the specified PKMN can perform an evolution, and performs it, if possible.
    *  @param p_opponent: true iff the opponent's PKMN shall be tested.
    *  @param p_pokemonPos: Position of the PKMN to be tested (0 or 1)
    */
    void battle::checkForEvolution( bool p_opponent, u8 p_pokemonPos ) {
        if( !SAV.m_evolveInBattle )
            return;

        if( ACPKMN( p_pokemonPos, p_opponent ).canEvolve( ) ) {
            auto& acPkmn = ACPKMN( p_pokemonPos, p_opponent );

            std::swprintf( wbuffer, 50, L"%ls entwickelt sich[A]", acPkmn.m_boxdata.m_name );
            log( wbuffer );

            acPkmn.evolve( );
            _battleUI.evolvePKMN( p_opponent, p_pokemonPos );

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
            if( _player->m_pkmnTeam->size( ) > i ) {
                if( ACPKMNSTS( i, PLAYER ) != KO )
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
            if( _opponent->m_pkmnTeam->size( ) > i ) {
                if( ACPKMNSTS( i, OPPONENT ) != KO )
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
                log( L"Das Rundenlimit dieses\nKampfes wurde erreicht.[A][CLEAR]Der Kampf endet in einem\nUnentschieden![A]" );
                break;
            case BATTLE::battle::OPPONENT_WON:
            {
                std::swprintf( wbuffer, 100, L"[TRAINER] [TCLASS] gewinnt...[A][CLEAR]%s[A]",
                               _opponent->getWinMsg( ) );
                log( wbuffer );
                break;
            }
            case BATTLE::battle::PLAYER_WON:
            {
                std::swprintf( wbuffer, 100, L"Du besiegst [TRAINER]\n[TCLASS]![A][CLEAR]%s[A]",
                               _opponent->getLooseMsg( ) );
                log( wbuffer );
                std::swprintf( wbuffer, 100, L"Du gewinnst %d$.[A]",
                               _opponent->getLooseMoney( ) );
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

        _battleUI.dinit( );
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

        _battleUI.hidePKMN( p_opponent, p_pokemonPos );

        std::swap( ACPOS( p_pokemonPos, p_opponent ), ACPOS( p_newPokemonPos, p_opponent ) );

        _battleUI.sendPKMN( p_opponent, p_pokemonPos );
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

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE
    //////////////////////////////////////////////////////////////////////////
}