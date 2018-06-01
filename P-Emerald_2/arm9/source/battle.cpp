/*
Pokémon Emerald 2 Version
------------------------------

file        : battle.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2018
Philip Wellnitz

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

#include "item.h"
#include "move.h"
#include "pokemon.h"

#include "fs.h"
#include "saveGame.h"
#include "sprite.h"
#include "uio.h"

#include "bag.h"
#include "dex.h"

#include "messageBox.h"

namespace BATTLE {
    std::string trainerClassNames[ 120 ] = {"Pokémon-Trainer",
                                            "Altes Paar",
                                            "Angler",
                                            "Aqua-Vorstand",
                                            "Aromalady",
                                            "Ass-Trainer",
                                            "Backpacker",
                                            "Camper",
                                            "Champ",
                                            "Drachenprofi",
                                            "Experte",
                                            "Forscher",
                                            "Forscherin",
                                            "Gentleman",
                                            "Geschwister",
                                            "Gitarrist",
                                            "Göre",
                                            "Hexe",
                                            "Hitzkopf",
                                            "Interviewer",
                                            "Junges Glück",
                                            "Käfermaniac",
                                            "Käfersammler",
                                            "Kämpferin",
                                            "Künstler",
                                            "Lady",
                                            "Magma-Vorstand",
                                            "Matrose",
                                            "Ninjajunge",
                                            "Picknickerin",
                                            "Planscher",
                                            "Pokéfan",
                                            "Pokémaniac",
                                            "Pokémon Ranger",
                                            "Pokémon-Sammler",
                                            "Pokémon-Trainer",
                                            "Pokémon-Züchter",
                                            "Psycho",
                                            "Ruinenmaniac",
                                            "Schirmdame",
                                            "Schnösel",
                                            "Schönheit",
                                            "Schulkind",
                                            "Schwarzgurt",
                                            "Schwimmer",
                                            "Schwimmerin",
                                            "Senior & Junior",
                                            "Serviererin",
                                            "Team Aqua Boss",
                                            "Team Aqua Rüpel",
                                            "Team Magma Boss",
                                            "Team Magma Rüpel",
                                            "Teenager",
                                            "Top Vier",
                                            "Triathlet",
                                            "Veteran",
                                            "Vogelfänger",
                                            "Vorschüler",
                                            "Vorschülerin",
                                            "Wanderer",
                                            "Zwillinge",
                                            "Ass-Duo",
                                            "Baron",
                                            "Baronin",
                                            "Butler",
                                            "Erbfolgerin",
                                            "Expertin",
                                            "Fahrer",
                                            "Fräulein Reihumkampf",
                                            "Gastronom",
                                            "Garçon",
                                            "Gärtner",
                                            "Geheimbasen-Meister",
                                            "Geheimbasis",
                                            "Graf",
                                            "Gräfin",
                                            "Grips & Muckis",
                                            "Großherzogin",
                                            "Halbstarke",
                                            "Herzog",
                                            "Herzogin",
                                            "Himmelstrainer",
                                            "Hochzeitsreisende",
                                            "Illumina-Gangster",
                                            "Inhaber",
                                            "Kampf-Châtelaine",
                                            "Kimono-Trägerin",
                                            "Koch",
                                            "Künstlerfamilie",
                                            "Künstlerin",
                                            "Marquis",
                                            "Marquise",
                                            "Planscherin",
                                            "Pokéfan-Pärchen",
                                            "Pokémon-Professor",
                                            "Pokémon-Züchterin",
                                            "Punker",
                                            "Punkerin",
                                            "Punker-Pärchen",
                                            "Raubein",
                                            "Rollerskater",
                                            "Rollerskaterin",
                                            "Schräge Schwestern",
                                            "Schüler",
                                            "Schülerin",
                                            "Taucher",
                                            "Taucherin",
                                            "Team Flare Rüpel",
                                            "Team Flare Vorstand",
                                            "Tourist",
                                            "Touristin",
                                            "Trainerhoffnung",
                                            "Träumerin",
                                            "Verdächtige Frau",
                                            "Verdächtiges Kind",
                                            "Verdächtiges Mädchen",
                                            "Vicomte",
                                            "Vicomtesse",
                                            "Wissenshüterin",
                                            "Xeneraner"};

    const char* ailmentnames[] = {"none",
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
                                  "Ingrain"};

    //////////////////////////////////////////////////////////////////////////
    // BEGIN BATTLE
    //////////////////////////////////////////////////////////////////////////

    int battle::getTargetSpecifierValue(
        bool p_targetIsOpp, u8 p_targetPosition,
        const battleScript::command::targetSpecifier& p_targetSpecifier, u8 p_targetVal ) {
        battlePokemon target = CUR_PKMN_STR( p_targetPosition, p_targetIsOpp );
        pokemonData   data;
        getAll( target.m_pokemon->m_boxdata.m_speciesId, data );
        switch( p_targetSpecifier ) {
        case BATTLE::battleScript::command::PKMN_TYPE1:
            return int( target.m_types[ 0 ] );
        case BATTLE::battleScript::command::PKMN_TYPE2:
            return int( target.m_types[ 1 ] );
        case BATTLE::battleScript::command::PKMN_TYPE3:
            return int( target.m_types[ 2 ] );
        case BATTLE::battleScript::command::PKMN_TYPE: {
            for( u8 i = 0; i < 3; ++i )
                if( p_targetVal == target.m_types[ i ] ) return target.m_types[ i ];
            return int( target.m_types[ 0 ] );
        }
        case BATTLE::battleScript::command::PKMN_SIZE:
            return int( data.m_size );
        case BATTLE::battleScript::command::PKMN_WEIGHT:
            return int( data.m_weight );
        case BATTLE::battleScript::command::PKMN_SPECIES:
            return target.m_pokemon->m_boxdata.m_speciesId;
        case BATTLE::battleScript::command::PKMN_ITEM:
            return target.m_pokemon->m_boxdata.m_holdItem;
        case BATTLE::battleScript::command::PKMN_ABILITY:
            return target.m_pokemon->m_boxdata.m_ability;
        case BATTLE::battleScript::command::PKMN_GENDER:
            return ( target.m_pokemon->m_boxdata.m_isFemale
                         ? -1
                         : ( target.m_pokemon->m_boxdata.m_isGenderless ? 0 : 1 ) );
        case BATTLE::battleScript::command::PKMN_STATUS:
            return target.m_pokemon->m_statusint;
        case BATTLE::battleScript::command::PKMN_HP:
            return target.m_pokemon->m_stats.m_acHP;
        case BATTLE::battleScript::command::PKMN_MAX_HP:
            return target.m_pokemon->m_stats.m_maxHP;
        case BATTLE::battleScript::command::PKMN_HP_PERCENT:
            return target.m_pokemon->m_stats.m_acHP * 100 / target.m_pokemon->m_stats.m_maxHP;
        case BATTLE::battleScript::command::PKMN_ATK:
            return target.m_acStatChanges[ ATK ];
        case BATTLE::battleScript::command::PKMN_DEF:
            return target.m_acStatChanges[ DEF ];
        case BATTLE::battleScript::command::PKMN_SPD:
            return target.m_acStatChanges[ SPD ];
        case BATTLE::battleScript::command::PKMN_SATK:
            return target.m_acStatChanges[ SATK ];
        case BATTLE::battleScript::command::PKMN_SDEF:
            return target.m_acStatChanges[ SDEF ];
        case BATTLE::battleScript::command::PKMN_ACCURACY:
            return target.m_acStatChanges[ ACCURACY ];
        case BATTLE::battleScript::command::PKMN_ATTACK_BLOCKED:
            return target.m_acStatChanges[ ATTACK_BLOCKED ];
        case BATTLE::battleScript::command::PKMN_LEVEL:
            return target.m_pokemon->m_level;
        default:
            return -1;
        }
    }
    int battle::getTargetSpecifierValue(
        const battleScript::command::targetSpecifier& p_targetSpecifier ) {
        switch( p_targetSpecifier ) {
        case BATTLE::battleScript::command::BATTLE_ROUND:
            return _round;
        case BATTLE::battleScript::command::BATTLE_MODE:
            return m_battleMode;
        case BATTLE::battleScript::command::BATTLE_WEATHER:
            return m_weather;
        case BATTLE::battleScript::command::BATTLE_LAST_MOVE:
            return _lstMove;
        case BATTLE::battleScript::command::BATTLE_LAST_OWN_MOVE:
            return _lstOwnMove;
        case BATTLE::battleScript::command::BATTLE_LAST_OPP_MOVE:
            return _lstOppMove;
        case BATTLE::battleScript::command::BATTLE_OWN_FST_MOVE:
            if( m_battleMode == battle::SINGLE
                || ( _moveOrder[ 1 ][ PLAYER ] > _moveOrder[ 0 ][ PLAYER ] ) )
                return ( _battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::ATTACK )
                           ? _battleMoves[ 0 ][ PLAYER ].m_value
                           : -1;
            return ( _battleMoves[ 1 ][ PLAYER ].m_type == battle::battleMove::ATTACK )
                       ? _battleMoves[ 1 ][ PLAYER ].m_value
                       : -1;
        case BATTLE::battleScript::command::BATTLE_OWN_SND_MOVE:
            if( m_battleMode == battle::SINGLE ) return -1;
            if( _moveOrder[ 1 ][ PLAYER ] > _moveOrder[ 0 ][ PLAYER ] )
                return ( _battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::ATTACK )
                           ? _battleMoves[ 0 ][ PLAYER ].m_value
                           : -1;
            return ( _battleMoves[ 1 ][ PLAYER ].m_type == battle::battleMove::ATTACK )
                       ? _battleMoves[ 1 ][ PLAYER ].m_value
                       : -1;
        case BATTLE::battleScript::command::BATTLE_OPP_FST_MOVE:
            if( m_battleMode == battle::SINGLE
                || ( _moveOrder[ 1 ][ OPPONENT ] > _moveOrder[ 0 ][ OPPONENT ] ) )
                return ( _battleMoves[ 0 ][ OPPONENT ].m_type == battle::battleMove::ATTACK )
                           ? _battleMoves[ 0 ][ OPPONENT ].m_value
                           : -1;
            return ( _battleMoves[ 1 ][ OPPONENT ].m_type == battle::battleMove::ATTACK )
                       ? _battleMoves[ 1 ][ OPPONENT ].m_value
                       : -1;
        case BATTLE::battleScript::command::BATTLE_OPP_SND_MOVE:
            if( m_battleMode == battle::SINGLE ) return -1;
            if( _moveOrder[ 1 ][ OPPONENT ] < _moveOrder[ 0 ][ OPPONENT ] )
                return ( _battleMoves[ 0 ][ OPPONENT ].m_type == battle::battleMove::ATTACK )
                           ? _battleMoves[ 0 ][ OPPONENT ].m_value
                           : -1;
            return ( _battleMoves[ 1 ][ OPPONENT ].m_type == battle::battleMove::ATTACK )
                       ? _battleMoves[ 1 ][ OPPONENT ].m_value
                       : -1;
        case BATTLE::battleScript::command::BATTLE_OWN_TEAMSIZE:
            return int( _player->m_pkmnTeam.size( ) );
        case BATTLE::battleScript::command::BATTLE_OPP_TEAMSIZE:
            return int( _opponent->m_pkmnTeam.size( ) );
        default:
            return -1;
        }
    }

#define C2I( a ) ( ( a ) - '0' )
    std::string battle::parseLogCmd( const std::string& p_cmd ) {
        if( p_cmd == "A" ) return "`";
        if( p_cmd == "CLEAR" ) return "";

        if( p_cmd == "TRAINER" ) { return _opponent->m_battleTrainerName; }
        if( p_cmd == "TCLASS" ) { return trainerClassNames[ _opponent->m_trainerClass ]; }
        if( p_cmd == "OPPONENT" ) {
            if( m_isWildBattle )
                return "(Wild)";
            else
                return "(Gegner)";
        }
        if( p_cmd.substr( 0, 4 ) == "COLR" ) {
            u8 r, g, b;

            r = 10 * C2I( p_cmd[ 5 ] ) + C2I( p_cmd[ 6 ] );
            g = 10 * C2I( p_cmd[ 8 ] ) + C2I( p_cmd[ 9 ] );
            b = 10 * C2I( p_cmd[ 11 ] ) + C2I( p_cmd[ 12 ] );

            _battleUI.setLogTextColor( RGB15( r, g, b ) );
            if( r != 15 || g != 15 || b != 15 )
                IO::regularFont->setColor( COLOR_IDX, 1 );
            else
                IO::regularFont->setColor( GRAY_IDX, 1 );
            return "";
        }

        pokemon target = CUR_PKMN( 0, PLAYER );
        bool    isPkmn = false;
        bool    isOpp  = false;

        if( p_cmd.substr( 0, 4 ) == "OWN1" ) {
            target = CUR_PKMN( 0, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == "OWN2" ) {
            target = CUR_PKMN( 1, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == "OPP1" ) {
            target = CUR_PKMN( 0, OPPONENT );
            isOpp  = true;
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == "OPP2" ) {
            target = CUR_PKMN( 1, OPPONENT );
            isOpp  = true;
            isPkmn = true;
        }

        if( isPkmn && p_cmd.length( ) == 4 )
            return target.m_boxdata.m_name
                   + std::string( isOpp ? " " + parseLogCmd( "OPPONENT" ) : "" );

        if( isPkmn ) {
            auto specifier = p_cmd.substr( 5 );
            if( specifier == "ABILITY" ) return getAbilityName( target.m_boxdata.m_ability );
            if( specifier.substr( 0, 4 ) == "MOVE" )
                return AttackList[ target.m_boxdata.m_moves[ C2I( specifier[ 4 ] ) - 1 ] ]
                    ->m_moveName;
            if( specifier == "ITEM" ) return ItemList[ target.m_boxdata.m_holdItem ]->m_itemName;
            if( specifier == "LEVEL" ) return ( "" + target.m_level );
        }

        return "";
    }

    battle::battle( battleTrainer* p_player, battleTrainer* p_opponent, int p_maxRounds,
                    weather p_weather, u8 p_platform, u8 p_background, int p_AILevel,
                    battleMode p_battleMode, u8 p_platform2 )
        : _player( p_player ), _opponent( p_opponent ) {
        _maxRounds     = p_maxRounds;
        _AILevel       = p_AILevel;
        m_battleMode   = p_battleMode;
        m_isWildBattle = false;

        m_distributeEXP = true;

        m_weather      = p_weather;
        m_platformId   = p_platform;
        m_platform2Id  = p_platform2 == u8( -1 ) ? p_platform : p_platform2;
        m_backgroundId = p_background;
    }
    battle::battle( battleTrainer* p_player, pokemon* p_opponent, weather p_weather, u8 p_platform,
                    u8 p_platform2, u8 p_background )
        : _player( p_player ), _opponent( 0 ) {
        _maxRounds     = 0;
        _AILevel       = 0;
        m_weather      = p_weather;
        m_platformId   = p_platform;
        m_platform2Id  = p_platform2;
        m_backgroundId = p_background;

        pokemonData pdata;
        getAll( p_opponent->m_boxdata.m_speciesId, pdata );
        _wildPokemon.m_pokemon    = p_opponent;
        _wildPokemon.m_types[ 0 ] = pdata.m_types[ 0 ];
        _wildPokemon.m_types[ 1 ] = pdata.m_types[ 1 ];
        _wildPokemon.m_types[ 2 ] = pdata.m_types[ 1 ];

        m_battleMode    = SINGLE;
        m_isWildBattle  = true;
        m_distributeEXP = true;
    }

    /**
     *  @brief Write the message p_message to the battle log
     *  @param p_message: The message to be written
     */
    void battle::log( const std::string& p_message ) {
        std::string msg = "";
        for( size_t i = 0; i < p_message.length( ); i++ ) {
            if( p_message[ i ] == L'[' ) {
                std::string accmd = "";
                while( p_message[ ++i ] != L']' ) accmd += p_message[ i ];
                if( accmd == "END" ) {
                    _battleUI.writeLogText( msg );
                    return;
                }
                msg += parseLogCmd( accmd );
                if( accmd == "CLEAR" ) {
                    _battleUI.writeLogText( msg );
                    _battleUI.clearLogScreen( );
                    msg = "";
                }
            } else
                msg += p_message[ i ];
        }

        if( msg != "" ) _battleUI.writeLogText( msg );
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
        while( !_maxRounds || ++_round < _maxRounds ) {
            registerParticipatedPKMN( );

            bool p1CanMove = canMove( PLAYER, 0 );
        CHOOSE1:
            firstMoveSwitchTarget = 0;

            _battleMoves[ 0 ][ PLAYER ]   = {(battleMove::type) 0, 0, 0, 0};
            _battleMoves[ 1 ][ PLAYER ]   = {(battleMove::type) 0, 0, 0, 0};
            _battleMoves[ 0 ][ OPPONENT ] = {(battleMove::type) 0, 0, 0, 0};
            _battleMoves[ 1 ][ OPPONENT ] = {(battleMove::type) 0, 0, 0, 0};

            if( CUR_PKMN_STS( 0, PLAYER ) != KO ) {
                if( p1CanMove ) {
                    _battleUI.declareBattleMove( 0, false );
                    if( _endBattle ) {
                        endBattle( battleEnd = RUN );
                        break;
                    }
                } else
                    log( "[OWN1] kann nicht angreifen…[A]" );
            }
            // If 1st action is RUN, the player has no choice for a second move
            if( _battleMoves[ 0 ][ PLAYER ].m_type != battleMove::RUN
                && CUR_PKMN_STS( 1, PLAYER ) != KO ) {
                if( m_battleMode == DOUBLE && canMove( PLAYER, 1 ) ) {
                    if( !_battleUI.declareBattleMove( 1, p1CanMove
                                                             && CUR_PKMN_STS( 0, PLAYER ) != KO ) )
                        goto CHOOSE1;

                    if( _endBattle ) {
                        endBattle( battleEnd = RUN );
                        return ( battleEnd );
                    }
                } else if( m_battleMode == DOUBLE )
                    log( "[OWN2] kann nicht angreifen…[A]" );
            } else
                _battleMoves[ 1 ][ PLAYER ].m_type = battleMove::RUN;

            getAIMoves( );

            doMoves( );
            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                break;
            }
            if( _endBattle ) {
                endBattle( battleEnd = PLAYER_WON );
                break;
            }

            doWeather( );
            for( int k = 0; k < 4; ++k ) {
                if( m_battleMode != DOUBLE && ( k % 2 ) ) continue;

                handleFaint( k / 2, k % 2 );
                doItem( k / 2, k % 2, ability::BETWEEN_TURNS );
                doAbility( k / 2, k % 2, ability::BETWEEN_TURNS );
                handleSpecialConditions( k / 2, k % 2 );
                handleFaint( k / 2, k % 2 );
            }

            refillBattleSpots( true );
            for( int k = 0; k < 4; ++k ) {
                if( m_battleMode != DOUBLE && ( k % 2 ) ) continue;
                handleFaint( k / 2, k % 2, false );
            }
        }

        return battleEnd;
    }

    /**
     *  @brief Initialize the battle.
     */
    void battle::initBattle( ) {
        // Some basic initialization stuff
        _battleUI = battleUI( this );
        _battleUI.init( );

        pokemonData pdata;
        for( u8 i = 0; i < 6; ++i ) {
            if( _player->m_pkmnTeam.size( ) > i ) {
                getAll( _player->m_pkmnTeam[ i ].m_boxdata.m_speciesId, pdata );
                _pkmns[ i ][ 0 ].m_pokemon    = &( _player->m_pkmnTeam[ i ] );
                _pkmns[ i ][ 0 ].m_types[ 0 ] = pdata.m_types[ 0 ];
                _pkmns[ i ][ 0 ].m_types[ 1 ] = pdata.m_types[ 1 ];
                _pkmns[ i ][ 0 ].m_types[ 2 ] = pdata.m_types[ 1 ];
            }
            if( !m_isWildBattle )
                if( _opponent->m_pkmnTeam.size( ) > i ) {
                    getAll( _opponent->m_pkmnTeam[ i ].m_boxdata.m_speciesId, pdata );
                    _pkmns[ i ][ 1 ].m_pokemon    = &( _opponent->m_pkmnTeam[ i ] );
                    _pkmns[ i ][ 1 ].m_types[ 0 ] = pdata.m_types[ 0 ];
                    _pkmns[ i ][ 1 ].m_types[ 1 ] = pdata.m_types[ 1 ];
                    _pkmns[ i ][ 1 ].m_types[ 2 ] = pdata.m_types[ 1 ];
                }

            CUR_POS( i, PLAYER ) = CUR_POS( i, OPPONENT ) = i;
            for( u8 o = 0; o < MAX_STATS; ++o )
                CUR_PKMN_STATCHG( i, PLAYER )[ o ] = CUR_PKMN_STATCHG( i, OPPONENT )[ o ] = 0;
            if( _player->m_pkmnTeam.size( ) > i ) {
                if( CUR_PKMN( i, PLAYER ).m_boxdata.m_individualValues.m_isEgg )
                    CUR_PKMN_STS( i, PLAYER ) = NA;
                else if( CUR_PKMN( i, PLAYER ).m_stats.m_acHP == 0 )
                    CUR_PKMN_STS( i, PLAYER ) = KO;
                else if( CUR_PKMN( i, PLAYER ).m_statusint )
                    CUR_PKMN_STS( i, PLAYER ) = STS;
                else
                    CUR_PKMN_STS( i, PLAYER ) = OK;
            } else
                CUR_PKMN_STS( i, PLAYER ) = NA;
            if( !m_isWildBattle ) {
                if( _opponent->m_pkmnTeam.size( ) > i ) {
                    if( CUR_PKMN( i, OPPONENT ).m_boxdata.m_individualValues.m_isEgg )
                        CUR_PKMN_STS( i, OPPONENT ) = NA;
                    else if( CUR_PKMN( i, OPPONENT ).m_stats.m_acHP == 0 )
                        CUR_PKMN_STS( i, OPPONENT ) = KO;
                    else if( CUR_PKMN( i, OPPONENT ).m_statusint )
                        CUR_PKMN_STS( i, OPPONENT ) = STS;
                    else
                        CUR_PKMN_STS( i, OPPONENT ) = OK;
                } else
                    CUR_PKMN_STS( i, OPPONENT ) = NA;
            }
        }
        // memset( _participatedPKMN, 0, sizeof( _participatedPKMN ) );

        for( u8 p = 0; p < 2; ++p ) {
            _battleSpotOccupied[ 0 ][ p ] = false;
            _battleSpotOccupied[ 1 ][ p ] = ( m_battleMode != DOUBLE );
        }

        refillBattleSpots( false, false );

        if( !m_isWildBattle )
            _battleUI.trainerIntro( );
        else
            _battleUI.pokemonIntro( );

        refillBattleSpots( false );

        if( m_weather != NO_WEATHER ) { log( weatherMessage[ m_weather ] ); }

        doAbilities( ability::BEFORE_BATTLE );
    }

    /**
     *  @brief send in PKMN for fainted ones, if possible
     *  @param p_choice: Specifies whether the player can choose the PKMN which is/are being sent
     */
    void battle::refillBattleSpots( bool p_choice, bool p_send ) {
        for( u8 i = 0; i < 1 + ( m_battleMode == DOUBLE ); ++i )
            for( u8 j = 0; j < 2; ++j ) {
                if( j && m_isWildBattle ) continue;
                if( !_battleSpotOccupied[ i ][ j ] ) {
                    if( CUR_PKMN_STS( i, j ) != KO && CUR_PKMN_STS( i, j ) != NA
                        && CUR_PKMN( i, j ).m_stats.m_acHP )
                        continue;

                    bool refillpossible = false;

                    for( u8 k = 1 + ( m_battleMode == DOUBLE );
                         k < ( j ? _opponent->m_pkmnTeam.size( ) : _player->m_pkmnTeam.size( ) );
                         ++k )
                        if( CUR_PKMN_STS( k, j ) != KO && CUR_PKMN_STS( k, j ) != SELECTED
                            && CUR_PKMN_STS( k, j ) != NA && CUR_PKMN( k, j ).m_stats.m_acHP
                            && !CUR_PKMN( k, j ).isEgg( ) ) {
                            refillpossible = true;
                            break;
                        }

                    if( !refillpossible ) {
                        _battleSpotOccupied[ i ][ j ] = true;
                        continue;
                    }

                    u8 nextSpot = i;
                    if( !p_choice || j )
                        nextSpot = getNextPKMN( j, 1 + p_send * ( m_battleMode == DOUBLE ) );
                    else
                        nextSpot = _battleUI.choosePKMN( m_battleMode == DOUBLE, false );

                    if( nextSpot != 7 && nextSpot != i )
                        std::swap( CUR_POS( i, j ), CUR_POS( nextSpot, j ) );
                    _battleSpotOccupied[ i ][ j ] = ( nextSpot == 7 );
                }
            }

        // Sort the remaining PKMN according to their status -> No STS, STS, KO, NA
        std::vector<std::pair<u8, u8>> tmp;
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            tmp.push_back( std::pair<u8, u8>( CUR_PKMN_STS( i, PLAYER ), CUR_POS( i, PLAYER ) ) );
        std::sort( tmp.begin( ), tmp.end( ) );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            CUR_POS( i, PLAYER ) = tmp[ i - ( m_battleMode == DOUBLE ? 2 : 1 ) ].second;

        if( !m_isWildBattle ) {
            // Sort the remaining PKMN according to their status -> No STS, STS, KO, NA (For the
            // opponent)
            tmp.clear( );
            for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
                tmp.push_back(
                    std::pair<u8, u8>( CUR_PKMN_STS( i, OPPONENT ), CUR_POS( i, OPPONENT ) ) );
            std::sort( tmp.begin( ), tmp.end( ) );
            for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
                CUR_POS( i, OPPONENT ) = tmp[ i - ( m_battleMode == DOUBLE ? 2 : 1 ) ].second;
        }
        orderPKMN( false );

        if( p_send ) {
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 0; j < 1 + ( m_battleMode == DOUBLE ); ++j )
                    if( !_battleSpotOccupied[ j ][ i ] ) _battleUI.updateStats( i, j, false );
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 1 + ( m_battleMode == DOUBLE ); j < 6; ++j )
                    _battleUI.updateStats( i, j, false );
        }
        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 1 + ( m_battleMode == DOUBLE ); ++i )
                for( u8 j = 0; j < 2; ++j ) {
                    if( _moveOrder[ i ][ j ] == p ) {
                        if( !_battleSpotOccupied[ i ][ j ] && p_send ) {
                            _battleUI.sendPKMN( j, i );
                            _battleSpotOccupied[ i ][ j ] = true;
                        }
                        goto NEXT;
                    }
                }
        NEXT:;
        }
    }

    /**
     *  @brief Gets the index of the next PKMN that is not koed.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @returns Minimum i for which CUR_PKMN_STS(i,p_opponent) != KO and != SELECTED, 7 iff all
     * PKMN fainted
     */
    u8 battle::getNextPKMN( bool p_opponent, u8 p_startIdx ) {
        if( p_opponent && m_isWildBattle ) return 7;
        u8 max = ( p_opponent ? _opponent->m_pkmnTeam.size( ) : _player->m_pkmnTeam.size( ) );

        for( u8 i = p_startIdx; i < max; ++i )
            if( CUR_PKMN_STS( i, p_opponent ) != KO && CUR_PKMN_STS( i, p_opponent ) != SELECTED
                && !CUR_PKMN( i, p_opponent ).isEgg( ) && CUR_PKMN( i, p_opponent ).m_stats.m_acHP )
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

        // return;
        bool hasTrickRoom = ( _battleTerrain & TRICK_ROOM );

        std::vector<std::tuple<s16, s16, u16>> inits;
        for( u8 i = 0; i < ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); ++i ) {
            for( u8 j = 0; j < 2; ++j ) {
                s16 acSpd  = CUR_PKMN( i, j ).m_stats.m_Spd;
                s8  movePr = ( ( _battleMoves[ i ][ j ].m_type == battleMove::ATTACK
                                || _battleMoves[ i ][ j ].m_type == battleMove::MEGA_ATTACK )
                                  ? AttackList[ _battleMoves[ i ][ j ].m_value ]->m_movePriority
                                  : 0 );
                if( CUR_PKMN( i, j ).m_status.m_isParalyzed
                    && CUR_PKMN( i, j ).m_boxdata.m_ability != A_QUICK_FEET )
                    acSpd /= 4;
                else if( CUR_PKMN( i, j ).m_statusint
                         && CUR_PKMN( i, j ).m_boxdata.m_ability == A_QUICK_FEET )
                    acSpd += acSpd / 2;

                if( p_includeMovePriority ) {
                    if( hasTrickRoom ) acSpd *= -1;
                    if( CUR_PKMN( i, j ).m_boxdata.m_holdItem == I_QUICK_CLAW
                        && ( ( rand( ) % 100 ) < 20 ) ) {

                        char buffer[ 150 ];
                        snprintf( buffer, 149, GET_STRING( 169 ),
                                  ( CUR_PKMN( i, j ).m_boxdata.m_name ),
                                  ( j ? " [OPPONENT]" : "" ) );
                        log( buffer );

                        acSpd += 2000;
                    } else if( CUR_PKMN( i, j ).m_boxdata.m_holdItem == I_LAGGING_TAIL )
                        acSpd -= 2000;
                    else if( CUR_PKMN( i, j ).m_boxdata.m_ability == A_STALL )
                        acSpd -= 2000;

                    if( CUR_PKMN( i, j ).m_boxdata.m_holdItem == I_FULL_INCENSE ) movePr--;
                }

                if( _battleMoves[ i ][ j ].m_type == battleMove::SWITCH ) movePr = 7;
                if( _battleMoves[ i ][ j ].m_type == battleMove::USE_ITEM ) movePr = 7;
                inits.push_back(
                    std::tuple<s16, s16, u8>( p_includeMovePriority * movePr, acSpd, 2 * i + j ) );
            }
        }
        std::sort( inits.rbegin( ), inits.rend( ) );

        for( u8 i = 0; i < inits.size( ); ++i ) {
            s16 _0                = 0;
            s16 _1                = 0;
            u16 v                 = 0;
            std::tie( _0, _1, v ) = inits[ i ];
            u8 isOpp = v % 2, isSnd = v / 2;

            _moveOrder[ isSnd ][ isOpp ] = i;
        }
    }

    /**
     *  @brief Applies all possible abilities ordered by their PKMNs speed.
     *  @param p_situation: Current situation, on which an ability may be useable
     */
    void battle::doAbilities( ability::abilityType p_situation ) {
        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 0; j < 2; ++j ) {
                    if( _moveOrder[ i ][ j ] == p ) {
                        doAbility( j, i, p_situation );
                        goto NEXT;
                    }
                }
        NEXT:;
        }
    }

    /**
     *  @brief Applies all possible abilities ordered by their PKMNs speed.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     *  @param p_situation: Current situation, on which an ability may be useable
     */
    void battle::doAbility( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation ) {
        if( !CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP ) return;

        auto ab = ability( CUR_PKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability );

        if( ab.m_type & p_situation ) {
            char buffer[ 100 ];
            snprintf( buffer, 99, GET_STRING( 170 ), ab.m_abilityName.c_str( ),
                      ( CUR_PKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                      ( p_opponent ? "\n[OPPONENT]" : "\n" ) );

            log( buffer );
            ab.m_effect.execute( *this, &( CUR_PKMN( p_pokemonPos, p_opponent ) ) );

            // for( u8 k = 0; k < 4; ++k ) {
            //    bool isOpp = k % 2,
            //        isSnd = k / 2;
            //    _battleUI.updateHP( isOpp, isSnd );
            //    _battleUI.updateStats( isOpp, isSnd );
            //}
        }
    }

    /**
     *  @brief Determines whether the specified PKMN can perform a move during the current turn.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    bool battle::canMove( bool p_opponent, u8 p_pokemonPos ) {
        if( m_battleMode != DOUBLE && p_pokemonPos ) return false;
        return !CUR_PKMN_STATCHG( p_pokemonPos, p_opponent )[ ATTACK_BLOCKED ];
    }

    /**
     *  @brief Computes the AI's moves and stores them into _battleMoves
     */
    void battle::getAIMoves( ) {
        switch( _AILevel ) {
        default:
        case 0: // Trivial AI
        {
            for( u8 i = 0; i < 2; ++i )
                if( canMove( OPPONENT, i ) ) {
                    if( !m_isWildBattle )
                        _battleMoves[ i ][ OPPONENT ]
                            = {battleMove::type::ATTACK,
                               CUR_PKMN( i, OPPONENT ).m_boxdata.m_moves[ 0 ], 0, 0};
                    else
                        _battleMoves[ i ][ OPPONENT ]
                            = {battleMove::type::ATTACK,
                               _wildPokemon.m_pokemon->m_boxdata.m_moves[ 0 ], 0, 0};
                }
            break;
        }
        }
    }

    /**
     *  @brief Does all of the turn's moves.
     */
    void battle::doMoves( ) {
        // Use all Items first
        for( u8 i = 0; i < 4; ++i ) {
            bool isOpp = i % 2, isSnd = i / 2;

            if( _battleMoves[ isSnd ][ isOpp ].m_type == battleMove::USE_ITEM )
                doItem( isOpp, isSnd, ability::GRASS );
            if( _endBattle ) return;
        }

        // Mega evolve all PKMN second
        for( u8 i = 0; i < 4; ++i ) {
            bool isOpp = i % 2, isSnd = i / 2;

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
        if( !_allowMegaEvolution ) return;
        (void) p_opponent;
        (void) p_pokemonPos;
        // TODO
    }

    /**
     *  @brief Does the specified PKMN's move.
     */
    void battle::doMove( u8 p_moveNo ) {

        u8 opponent   = -1;
        u8 pokemonPos = -1;

        for( opponent = 0; opponent < 2; ++opponent )
            for( pokemonPos = 0; pokemonPos < 2; ++pokemonPos )
                if( _moveOrder[ pokemonPos ][ opponent ] == p_moveNo ) goto OUT;
    OUT:
        auto& acMove = _battleMoves[ pokemonPos ][ opponent ];

        if( ( !m_isWildBattle || !opponent ) && !CUR_PKMN( pokemonPos, opponent ).m_stats.m_acHP )
            return;

        std::string acPkmnStr = "";
        if( opponent )
            acPkmnStr = "OPP" + ( pokemonPos + 1 );
        else
            acPkmnStr = "OWN" + ( pokemonPos + 1 );

        switch( acMove.m_type ) {
        case battleMove::ATTACK: {
            auto acAttack = AttackList[ acMove.m_value ];
            if( acMove.m_target == 0 ) {
                if( !opponent ) switch( acAttack->m_moveAffectsWhom ) {
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
                        if( acMove.m_target == ( 1 << pokemonPos ) ) acMove.m_target <<= 1;
                        break;
                    default:
                        break;
                    }
                else
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
                        if( acMove.m_target == ( 1 << ( 2 + pokemonPos ) ) ) acMove.m_target >>= 1;
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
        case battleMove::USE_NAV:
        case battleMove::USE_ITEM:
        default:
            break;
        }
    }

    s16 battle::calcDamage( bool p_userIsOpp, u8 p_userPos, bool p_targetIsOpp, u8 p_targetPos ) {
        auto bm = _battleMoves[ p_userPos ][ p_userIsOpp ];

        // Calculate critical hit chance
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
            // log( "Move is a STS move[A]" );
            return ( _acDamage[ p_targetPos ][ p_targetIsOpp ] = 0 );
        }

        // Calculate effectivity
        _effectivity[ p_targetPos ][ p_targetIsOpp ] = getEffectiveness(
            move->m_moveType, CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 0 ] );
        if( CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 0 ]
            != CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 1 ] )
            _effectivity[ p_targetPos ][ p_targetIsOpp ] *= getEffectiveness(
                move->m_moveType, CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 1 ] );
        if( CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 2 ]
            != CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 1 ] )
            _effectivity[ p_targetPos ][ p_targetIsOpp ] *= getEffectiveness(
                move->m_moveType, CUR_PKMN_STR( p_targetPos, p_targetIsOpp ).m_types[ 2 ] );

        // Calculate critical hit
        _critical[ p_targetPos ][ p_targetIsOpp ] = !( rand( ) % mod );
        // STAB
        float       STAB = 1.0f;
        pokemonData pdata;
        getAll( CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_speciesId, pdata );
        if( pdata.m_types[ 0 ] == move->m_moveType || pdata.m_types[ 1 ] == move->m_moveType )
            STAB = 1.5f;
        if( CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_ADAPTABILITY ) STAB = 2.0f;

        // Weather
        float weather = 1.0f;

        bool weatherPossible = true;
        for( u8 a = 0; a < 1 + ( m_battleMode == DOUBLE ); ++a )
            for( u8 b = 0; b < 2; ++b )
                weatherPossible &= ( CUR_PKMN( a, b ).m_boxdata.m_ability != A_AIR_LOCK
                                     && CUR_PKMN( a, b ).m_boxdata.m_ability != A_CLOUD_NINE );

        if( weatherPossible ) {
            if( m_weather == SUN && move->m_moveType == WATER ) weather = 0.5f;
            if( m_weather == HEAVY_SUNSHINE && move->m_moveType == WATER ) weather = 0.0f;

            if( m_weather == SUN && move->m_moveType == FIRE ) weather = 1.5f;
            if( m_weather == HEAVY_SUNSHINE && move->m_moveType == FIRE ) weather = 1.5f;

            if( m_weather == RAIN && move->m_moveType == FIRE ) weather = 0.5f;
            if( m_weather == HEAVY_RAIN && move->m_moveType == FIRE ) weather = 0.0f;

            if( m_weather == RAIN && move->m_moveType == WATER ) weather = 1.5f;
            if( m_weather == HEAVY_RAIN && move->m_moveType == WATER ) weather = 1.5f;
        }

        // Multi-Target modifier
        float target = 1.0f;
        if( move->m_moveAffectsWhom & ( 8 | 32 ) ) target = 0.75f;

        // Base damage calculation
        auto moveAtkHitType = move->m_moveHitType;
        auto moveDefHitType = move->m_moveHitType;

        if( bm.m_value == M_PSYSHOCK ) moveDefHitType = move::PHYS;
        if( bm.m_value == M_PSYSTRIKE ) moveDefHitType = move::PHYS;
        if( bm.m_value == M_SECRET_SWORD ) moveDefHitType = move::PHYS;

        float atk = ( ( moveAtkHitType == move::PHYS )
                          ? CUR_PKMN( p_userPos, p_userIsOpp ).m_stats.m_Atk
                          : CUR_PKMN( p_userPos, p_userIsOpp ).m_stats.m_SAtk );
        float def = ( ( moveDefHitType == move::PHYS )
                          ? CUR_PKMN( p_targetPos, p_targetIsOpp ).m_stats.m_Def
                          : CUR_PKMN( p_targetPos, p_targetIsOpp ).m_stats.m_SDef );

        // Burn and other status stuff
        if( CUR_PKMN( p_userPos, p_userIsOpp ).m_status.m_isBurned
            && CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability != A_GUTS
            && moveAtkHitType == move::PHYS ) {
            atk /= 2;
        } else if( CUR_PKMN( p_userPos, p_userIsOpp ).m_statusint
                   && CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_GUTS
                   && moveAtkHitType == move::PHYS ) {
            atk *= 1.5;
        } else if( CUR_PKMN( p_userPos, p_userIsOpp ).m_status.m_isPoisoned
                   && CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_TOXIC_BOOST
                   && moveAtkHitType == move::PHYS ) {
            atk *= 1.5;
        } else if( CUR_PKMN( p_userPos, p_userIsOpp ).m_status.m_isPoisoned
                   && CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability == A_FLARE_BOOST
                   && moveAtkHitType == move::SPEC ) {
            atk *= 1.5;
        }

        if( CUR_PKMN( p_targetPos, p_targetIsOpp ).m_statusint
            && CUR_PKMN( p_targetPos, p_targetIsOpp ).m_boxdata.m_ability == A_MARVEL_SCALE
            && moveDefHitType == move::PHYS ) {
            def *= 1.5;
        }

        // Stat changes
        if( CUR_PKMN( p_userPos, p_userIsOpp ).m_boxdata.m_ability != A_UNAWARE
            && bm.m_value != M_FOUL_PLAY ) {
            if( moveAtkHitType == move::PHYS ) {
                if( CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ ATK ] > 0 )
                    atk = atk * ( 2 + CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ ATK ] ) / 2.0;
                if( CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ ATK ] < 0 )
                    atk = atk * 2.0 / ( 2 - CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ ATK ] );
            }
            if( moveAtkHitType == move::SPEC ) {
                if( CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ SATK ] > 0 )
                    atk = atk * ( 2 + CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ SATK ] ) / 2.0;
                if( CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ SATK ] < 0 )
                    atk = atk * 2.0 / ( 2 - CUR_PKMN_STATCHG( p_userPos, p_userIsOpp )[ SATK ] );
            }
        }
        if( bm.m_value == M_FOUL_PLAY ) {
            atk = CUR_PKMN( p_targetPos, p_targetIsOpp ).m_stats.m_Atk;

            if( CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ ATK ] > 0 )
                atk = atk * ( 2 + CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ ATK ] ) / 2.0;
            if( CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ ATK ] < 0 )
                atk = atk * 2.0 / ( 2 - CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ ATK ] );
        }

        if( CUR_PKMN( p_targetPos, p_targetIsOpp ).m_boxdata.m_ability != A_UNAWARE
            && bm.m_value != M_CHIP_AWAY ) {
            if( moveDefHitType == move::PHYS ) {
                if( CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ DEF ] > 0 )
                    def = def * ( 2 + CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ DEF ] ) / 2.0;
                if( CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ DEF ] < 0 )
                    def = def * 2.0 / ( 2 - CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ DEF ] );
            }
            if( moveDefHitType == move::SPEC ) {
                if( CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] > 0 )
                    def = def * ( 2 + CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] )
                          / 2.0;
                if( CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] < 0 )
                    def = def * 2.0
                          / ( 2 - CUR_PKMN_STATCHG( p_targetPos, p_targetIsOpp )[ SDEF ] );
            }
        }

        _acDamage[ p_targetPos ][ p_targetIsOpp ]
            = s16( ( ( 2.0f * CUR_PKMN( p_userPos, p_userIsOpp ).m_level + 10.0f ) / 250.0f )
                       * ( atk / def ) * s16( move->m_moveBasePower )
                   + 2 );

        float modifier = _effectivity[ p_targetPos ][ p_targetIsOpp ]
                         * ( _critical[ p_targetPos ][ p_targetIsOpp ] ? 1.5f : 1.0f ) * STAB
                         * weather * target * ( ( 100 - rand( ) % 15 ) / 100.0 );
        _acDamage[ p_targetPos ][ p_targetIsOpp ]
            = s16( _acDamage[ p_targetPos ][ p_targetIsOpp ] * modifier );

        return _acDamage[ p_targetPos ][ p_targetIsOpp ];
    }

    /**
     *  @brief Applies all possible hold Items ordered by their PKMNs speed.
     *  @param p_situation: Current situation, on which an Item may be useable
     */
    void battle::doItems( ability::abilityType p_situation ) {
        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )
                for( u8 j = 0; j < 2; ++j ) {
                    if( _moveOrder[ i ][ j ] == p ) {
                        doItem( j, i, p_situation );
                        goto NEXT;
                    }
                }
        NEXT:;
        }
    }

    /**
     *  @brief Applies all possible hold items ordered by their PKMNs speed.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     *  @param p_situation: Current situation, on which an item may be useable
     */
    void battle::doItem( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation ) {
        char buffer[ 100 ];
        if( p_situation != ability::GRASS ) {
            if( !CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP ) return;

            auto im = *ItemList[ CUR_PKMN( p_pokemonPos, p_opponent ).getItem( ) ];

            if( ( im.getEffectType( ) & item::itemEffectType::IN_BATTLE )
                && ( im.m_inBattleEffect & p_situation ) ) {
                snprintf( buffer, 99, GET_STRING( 170 ), im.getDisplayName( ).c_str( ),
                          ( CUR_PKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                          ( p_opponent ? "\n[OPPONENT]" : "\n" ) );

                log( buffer );
                u16 oldHP[ 4 ] = {0};
                for( u8 k = 0; k < 4; ++k ) {
                    bool isOpp = k % 2, isSnd = k / 2;
                    oldHP[ k ] = CUR_PKMN( isSnd, isOpp ).m_stats.m_acHP;
                }
                im.m_inBattleScript.execute( *this, &( CUR_PKMN( p_pokemonPos, p_opponent ) ) );
                for( u8 k = 0; k < 4; ++k ) {
                    bool isOpp = k % 2, isSnd = k / 2;
                    _battleUI.updateHP( isOpp, isSnd, oldHP[ k ] );
                    _battleUI.updateStats( isOpp, isSnd );
                }
            }
        } else {
            auto im = ItemList[ _battleMoves[ p_pokemonPos ][ p_opponent ].m_value ];
            if( p_opponent ) {
                snprintf( buffer, 99, GET_STRING( 171 ), im->getDisplayName( true ).c_str( ) );
                log( buffer );

                u16 oldHP    = CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP;
                u16 oldHPmax = CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_maxHP;

                if( im->m_itemType != item::MEDICINE || im->needsInformation( 0 )
                    || im->needsInformation( 1 )
                    || !im->use( CUR_PKMN( p_pokemonPos, p_opponent ) ) )
                    log( GET_STRING( 172 ) );
                else {
                    if( CUR_PKMN( p_pokemonPos, PLAYER ).m_boxdata.m_individualValues.m_isEgg )
                        CUR_PKMN_STS( p_pokemonPos, PLAYER ) = NA;
                    else if( CUR_PKMN( p_pokemonPos, PLAYER ).m_stats.m_acHP == 0 )
                        CUR_PKMN_STS( p_pokemonPos, PLAYER ) = KO;
                    else if( CUR_PKMN( p_pokemonPos, PLAYER ).m_statusint )
                        CUR_PKMN_STS( p_pokemonPos, PLAYER ) = STS;
                    else
                        CUR_PKMN_STS( p_pokemonPos, PLAYER ) = OK;
                }

                for( u8 i = 0; i < _opponent->m_itemCount; ++i )
                    if( _opponent->m_items[ i ].first
                        == _battleMoves[ p_pokemonPos ][ p_opponent ].m_value )
                        _opponent->m_items[ i ].second
                            = (u16) std::max( 0, _opponent->m_items[ i ].second - 1 );
                _battleUI.updateHP( p_opponent, p_pokemonPos, oldHP, oldHPmax );
                _battleUI.updateStats( p_opponent, p_pokemonPos );
            } else {
                if( im->m_itemType != item::POKE_BALLS ) {
                    std::sprintf( buffer, GET_STRING( 173 ), im->getDisplayName( true ).c_str( ) );
                    log( buffer );
                }

                if( !_player->m_items && _player->m_itemCount == MAX_ITEMS_IN_BAG ) {
                    SAVE::SAV->getActiveFile( ).m_bag.erase(
                        BAG::toBagType( im->m_itemType ),
                        _battleMoves[ p_pokemonPos ][ p_opponent ].m_value, 1 );
                } else
                    for( u8 i = 0; i < _player->m_itemCount; ++i )
                        if( _player->m_items[ i ].first
                            == _battleMoves[ p_pokemonPos ][ p_opponent ].m_value )
                            _player->m_items[ i ].second
                                = (u16) std::max( 0, _player->m_items[ i ].second - 1 );

                if( im->m_itemType == item::MEDICINE ) {
                    u8 pos = 0;
                    while( _battleMoves[ p_pokemonPos ][ p_opponent ].m_target ) {
                        ++pos;
                        _battleMoves[ p_pokemonPos ][ p_opponent ].m_target >>= 1;
                    }
                    --pos;
                    if( _battleMoves[ p_pokemonPos ][ p_opponent ].m_newItemEffect )
                        im->m_itemData.m_itemEffect
                            = _battleMoves[ p_pokemonPos ][ p_opponent ].m_newItemEffect;

                    u16 oldHP    = CUR_PKMN( pos, p_opponent ).m_stats.m_acHP;
                    u16 oldHPmax = CUR_PKMN( pos, p_opponent ).m_stats.m_maxHP;
                    if( !im->use( CUR_PKMN( pos, p_opponent ) ) )
                        log( GET_STRING( 172 ) );
                    else {
                        if( CUR_PKMN( pos, PLAYER ).m_boxdata.m_individualValues.m_isEgg )
                            CUR_PKMN_STS( pos, PLAYER ) = NA;
                        else if( CUR_PKMN( pos, PLAYER ).m_stats.m_acHP == 0 )
                            CUR_PKMN_STS( pos, PLAYER ) = KO;
                        else if( CUR_PKMN( pos, PLAYER ).m_statusint )
                            CUR_PKMN_STS( pos, PLAYER ) = STS;
                        else
                            CUR_PKMN_STS( pos, PLAYER ) = OK;
                    }

                    if( pos <= 1 ) _battleUI.updateHP( p_opponent, pos, oldHP, oldHPmax );
                    _battleUI.updateStats( p_opponent, pos );
                } else if( im->m_itemType == item::POKE_BALLS ) {
                    if( !m_isWildBattle ) {
                        _battleUI.capture( _battleMoves[ p_pokemonPos ][ p_opponent ].m_value, -1 );
                    } else if( tryCapture( _battleMoves[ p_pokemonPos ][ p_opponent ].m_value ) ) {
                        _wildPokemon.m_pokemon->m_boxdata.m_ball
                            = _battleMoves[ p_pokemonPos ][ p_opponent ].m_value;
                        _endBattle = true;
                        handleCapture( );
                    }
                }
            }
        }
    }

    /**
     * @brief Handles the capture of the wild Pokémon
     */
    void battle::handleCapture( ) {
        u16  spid = _wildPokemon.m_pokemon->m_boxdata.m_speciesId;
        char buffer[ 100 ];
        if( !( SAVE::SAV->m_caughtPkmn[ spid / 8 ] & ( 1 << ( spid % 8 ) ) ) ) {
            SAVE::SAV->m_caughtPkmn[ spid / 8 ] |= ( 1 << ( spid % 8 ) );
            snprintf( buffer, 99, GET_STRING( 174 ), _wildPokemon.m_pokemon->m_boxdata.m_name );
            log( buffer );

            DEX::dex( DEX::dex::SHOW_SINGLE, -1 )
                .run( _wildPokemon.m_pokemon->m_boxdata.m_speciesId );
        }
        _battleUI.handleCapture( );

        // Check whether the pkmn fits in the team
        if( _player->m_pkmnTeam.size( ) < 6 )
            _player->m_pkmnTeam.push_back( *_wildPokemon.m_pokemon );
        else {
            u8 oldbx = SAVE::SAV->getActiveFile( ).m_curBox;
            u8 nb    = SAVE::SAV->storePkmn( *_wildPokemon.m_pokemon );
            if( nb != u8( -1 ) ) {
                sprintf( buffer, GET_STRING( 175 ), _wildPokemon.m_pokemon->m_boxdata.m_name );
                log( buffer );

                if( oldbx != nb ) {
                    sprintf( buffer, GET_STRING( 176 ),
                             SAVE::SAV->m_storedPokemon[ oldbx ].m_name );
                    log( buffer );
                }
                sprintf( buffer, GET_STRING( 177 ), _wildPokemon.m_pokemon->m_boxdata.m_name,
                         SAVE::SAV->m_storedPokemon[ nb ].m_name );
                log( buffer );
            } else {
                log( GET_STRING( 178 ) );
                sprintf( buffer, GET_STRING( 179 ), _wildPokemon.m_pokemon->m_boxdata.m_name );
                log( buffer );
            }
        }
    }

    /**
     *  @brief does the p_moveNo positioned attack of this turn
     *  @param p_moveNo: The number of the attack that shall be done.
     */
    void battle::doAttack( bool p_opponent, u8 p_pokemonPos ) {
        if( p_pokemonPos && m_battleMode != DOUBLE ) return;
        char buffer[ 100 ];

        auto acpkmn   = &CUR_PKMN( p_pokemonPos, p_opponent );
        auto sts      = CUR_PKMN_STS( p_pokemonPos, p_opponent );
        auto statchng = CUR_PKMN_STATCHG( p_pokemonPos, p_opponent );

        // Check if the user has already fainted
        if( !acpkmn->m_stats.m_acHP || sts == KO ) return;

        auto& bm = _battleMoves[ p_pokemonPos ][ p_opponent ];
        if( bm.m_type != battleMove::ATTACK ) return;
        auto acMove = AttackList[ bm.m_value ];

        // Check if the user is frozen/asleep/paralyzed
        if( acpkmn->m_status.m_isFrozen ) {
            if( ( rand( ) % 100 ) < 20 ) { // PKMN thaws
                snprintf( buffer, 99, "%s%s ist aufgetaut![A]", ( acpkmn->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
                acpkmn->m_status.m_isFrozen = false;
            } else {
                snprintf( buffer, 99, "%s%s ist gefroren.[A]", ( acpkmn->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
                _battleUI.showStatus( p_opponent, p_pokemonPos );
                return;
            }
        }
        if( acpkmn->m_status.m_isParalyzed ) {
            if( ( rand( ) % 100 ) < 25 ) {
                snprintf( buffer, 99, "%s%s ist paralysiert.[A]", ( acpkmn->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
                _battleUI.showStatus( p_opponent, p_pokemonPos );
                return;
            }
        }
        if( acpkmn->m_status.m_isAsleep ) {
            if( --acpkmn->m_status.m_isAsleep ) {
                snprintf( buffer, 99, "%s%s bleibt schlafen.[A]", ( acpkmn->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
                // Check if the move can be used while the PKMN is asleep
                if( !( acMove->m_moveFlags & move::WHILE_ASLEEP ) ) {
                    _battleUI.showStatus( p_opponent, p_pokemonPos );
                    return;
                }
            } else {
                snprintf( buffer, 99, "%s%s ist aufgewacht![A]", ( acpkmn->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
            }
        }
        _battleUI.updateStatus( p_opponent, p_pokemonPos );

        snprintf( buffer, 99, GET_STRING( 10 ), ( acpkmn->m_boxdata.m_name ),
                  ( p_opponent ? " [OPPONENT]" : "" ), acMove->m_moveName.c_str( ) );
        log( buffer );

        _lstMove = bm.m_value;
        if( p_opponent )
            _lstOppMove = _lstMove;
        else
            _lstOwnMove = _lstMove;
        _currentMoveIsOpp = p_opponent;
        _currentMoveIsSnd = p_pokemonPos;

        bool moveHasTarget = false;
        // Try to redirect the move if the original target has already fainted
        if( m_battleMode == DOUBLE ) {
            if( p_opponent ) {
                if( ( bm.m_target & ( 1 << 0 ) ) && CUR_PKMN_STS( 0, PLAYER ) == KO
                    && CUR_PKMN_STS( 1, PLAYER ) != KO )
                    bm.m_target |= ( 1 << 1 );
                else if( ( bm.m_target & ( 1 << 1 ) ) && CUR_PKMN_STS( 1, PLAYER ) == KO
                         && CUR_PKMN_STS( 0, PLAYER ) != KO )
                    bm.m_target |= ( 1 << 0 );
            } else {
                if( ( bm.m_target & ( 1 << 2 ) ) && CUR_PKMN_STS( 0, OPPONENT ) == KO
                    && CUR_PKMN_STS( 1, OPPONENT ) != KO )
                    bm.m_target |= ( 1 << 3 );
                else if( ( bm.m_target & ( 1 << 3 ) ) && CUR_PKMN_STS( 1, OPPONENT ) == KO
                         && CUR_PKMN_STS( 0, OPPONENT ) != KO )
                    bm.m_target |= ( 1 << 2 );
            }
        }

        u8 moveAccuracy = acMove->m_moveAccuracy;
        if( statchng[ ACCURACY ] > 0 )
            moveAccuracy *= ( 2 + statchng[ ACCURACY ] ) / 2.0;
        else if( statchng[ ACCURACY ] < 0 )
            moveAccuracy *= 2.0 / ( 2 - statchng[ ACCURACY ] );

        // Reduce PP
        for( u8 i = 0; i < 4; ++i )
            if( acpkmn->m_boxdata.m_moves[ i ] == bm.m_value ) {
                if( acpkmn->m_boxdata.m_acPP[ i ] ) acpkmn->m_boxdata.m_acPP[ i ]--;
                break;
            }
        // For every target, check if that target protects itself
        for( u8 k = 0; k < 4; ++k ) {
            bool isOpp = k / 2, isSnd = k % 2;

            if( !( bm.m_target & ( 1 << k ) ) ) continue;
            if( ( !m_isWildBattle || !isOpp ) && !_battleSpotOccupied[ isSnd ][ isOpp ] ) continue;
            if( m_isWildBattle && isOpp && !_wildPokemon.m_pokemon->m_stats.m_acHP ) continue;

            if( m_battleMode != DOUBLE && isSnd ) continue;

            auto oppstatchng = CUR_PKMN_STATCHG( isSnd, isOpp );
            if( oppstatchng[ EVASION ] > 0 )
                moveAccuracy *= 2.0 / ( 2 + oppstatchng[ EVASION ] );
            else if( oppstatchng[ EVASION ] < 0 )
                moveAccuracy *= ( 2 - oppstatchng[ EVASION ] ) / 2.0;

            // Check if the target is protected and if the move is affected by Protect
            auto str = CUR_PKMN_STR( isSnd, isOpp );
            if( str.m_battleStatus == battleStatus::PROTECTED
                && ( acMove->m_moveFlags & move::PROTECT ) ) {
                bm.m_target &= ~( 1 << k );
                snprintf( buffer, 99, "%s%s bleibt unbeeindruckt.[A]",
                          ( str.m_pokemon->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
                moveHasTarget = true;
            }
            // Check if the move fails
            else if( moveAccuracy && ( s8( rand( ) % 100 ) < s8( 100 - moveAccuracy ) )
                     && acpkmn->m_boxdata.m_ability != A_NO_GUARD ) {
                bm.m_target &= ~( 1 << k );
                snprintf( buffer, 99, "%s%s wich aus.[A]", ( str.m_pokemon->m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
                moveHasTarget = true;
            }
            // If the target is already ko, then it shouldn't be a target at all…
            else if( !str.m_pokemon->m_stats.m_acHP ) {
                bm.m_target &= ~( 1 << k );
            }
            // Activate Items/Abilities before attack
            else {
                doItem( isOpp, isSnd, ability::BEFORE_ATTACK );
                doAbility( isOpp, isSnd, ability::BEFORE_ATTACK );
            }
        }

        // Attack animation
        _battleUI.showAttack( p_opponent, p_pokemonPos );

        // Damage and stuff
        for( u8 k = 0; k < 4; ++k ) {
            bool isOpp = k / 2, isSnd = k % 2;
            auto str = CUR_PKMN_STR( isSnd, isOpp );

            if( !( bm.m_target & ( 1 << k ) ) ) { continue; }
            if( ( !m_isWildBattle || !isOpp ) && !_battleSpotOccupied[ isSnd ][ isOpp ] ) continue;
            if( m_battleMode != DOUBLE && isSnd ) continue;

            if( !str.m_pokemon->m_stats.m_acHP ) continue;

            moveHasTarget = true;

            // Calculate damage against the target
            calcDamage( p_opponent, p_pokemonPos, isOpp,
                        isSnd ); // If the damage is negative, then it heals the target

            // Check for abilities/ items on the target
            doItem( isOpp, isSnd, ability::ATTACK );
            doAbility( isOpp, isSnd, ability::ATTACK );

            u16 oldHP                     = str.m_pokemon->m_stats.m_acHP;
            str.m_pokemon->m_stats.m_acHP = (u16) std::max(
                s16( 0 ),
                std::min( s16( str.m_pokemon->m_stats.m_acHP - _acDamage[ isSnd ][ isOpp ] ),
                          (s16) str.m_pokemon->m_stats.m_maxHP ) );

            _battleUI.updateHP( isOpp, isSnd, oldHP );
            if( acMove->m_moveHitType != move::STAT ) {
                if( _critical[ isSnd ][ isOpp ] )
                    log( "[COLR:15:15:00]Ein Volltreffer![A][CLEAR][COLR:00:00:00]" );
                if( _effectivity[ isSnd ][ isOpp ] != 1.0f ) {
                    float effectivity = _effectivity[ isSnd ][ isOpp ];
                    if( effectivity > 3.0f )
                        snprintf(
                            buffer, 99,
                            "[COLR:00:31:00]Das ist enorm effektiv\ngegen %s![A][CLEAR][COLR:00:00:00]",
                            str.m_pokemon->m_boxdata.m_name );
                    else if( effectivity > 1.0f )
                        snprintf(
                            buffer, 99,
                            "[COLR:00:15:00]Das ist sehr effektiv\ngegen %s![A][CLEAR][COLR:00:00:00]",
                            str.m_pokemon->m_boxdata.m_name );
                    else if( effectivity == 0.0f )
                        snprintf(
                            buffer, 99,
                            "[COLR:31:00:00]Hat die Attacke\n%s getroffen?[A][CLEAR][COLR:00:00:00]",
                            str.m_pokemon->m_boxdata.m_name );
                    else if( effectivity < 0.3f )
                        snprintf(
                            buffer, 99,
                            "[COLR:31:00:00]Das ist nur enorm wenig\neffektiv gegen %s…[A][CLEAR][COLR:00:00:00]",
                            str.m_pokemon->m_boxdata.m_name );
                    else if( effectivity < 1.0f )
                        snprintf(
                            buffer, 99,
                            "[COLR:15:00:00]Das ist nicht sehr effektiv\ngegen %s.[A][CLEAR][COLR:00:00:00]",
                            str.m_pokemon->m_boxdata.m_name );
                    log( buffer );
                }
            }
            // Check if PKMN fainted
            if( !str.m_pokemon->m_stats.m_acHP && _battleSpotOccupied[ isSnd ][ isOpp ] ) {
                handleFaint( isOpp, isSnd );
            } else {
                // Check if an attack effect triggers
                if( ( rand( ) % 100 ) < acMove->m_moveEffectAccuracy ) {
                    acMove->m_moveEffect.execute( *this, acpkmn );

                    for( u8 s = 0; s < MAX_STATS; s++ )
                        str.m_acStatChanges[ s ] += _acStatChange[ isSnd ][ isOpp ][ s ];
                    _battleUI.updateStats( isOpp, isSnd );
                }
            }

            // Check for items/abilities
            doItem( isOpp, isSnd, ability::AFTER_ATTACK );
            doAbility( isOpp, isSnd, ability::AFTER_ATTACK );
            // Check if any PKMN on the field fainted

            for( u8 j = 0; j < 4; ++j ) handleFaint( j / 2, j % 2 );
        }
        if( !moveHasTarget ) { log( "Es schlug fehl…[A]" ); }
        return;
    }

    void battle::storePkmnSts( bool p_opponent, u8 p_pokemonPos ) {
        _storedHP[ p_opponent ][ p_pokemonPos ]
            = CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP;
    }

    void battle::updatePkmnSts( bool p_opponent, u8 p_pokemonPos ) {
        if( _storedHP[ p_opponent ][ p_pokemonPos ]
            != CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP )
            _battleUI.updateHP( p_opponent, p_pokemonPos, _storedHP[ p_opponent ][ p_pokemonPos ] );
    }

    /**
     *  @brief Handles special condition damage between turns
     */
    void battle::handleSpecialConditions( bool p_opponent, u8 p_pokemonPos ) {
        auto acpkmn = &CUR_PKMN( p_pokemonPos, p_opponent );
        char buffer[ 100 ];
        if( acpkmn->m_status.m_isBurned ) {
            snprintf( buffer, 99, "Die Verbrennung schadet\n%s%s.[A]", ( acpkmn->m_boxdata.m_name ),
                      ( p_opponent ? " [OPPONENT]" : "" ) );
            log( buffer );

            u16 oldHP              = acpkmn->m_stats.m_acHP;
            acpkmn->m_stats.m_acHP = std::max(
                u16( 0 ), u16( acpkmn->m_stats.m_acHP - 1.0 / ( 8 * acpkmn->m_stats.m_maxHP ) ) );

            _battleUI.updateHP( p_opponent, p_pokemonPos, oldHP );
        }
        if( acpkmn->m_status.m_isPoisoned && acpkmn->m_boxdata.m_ability != A_POISON_HEAL ) {
            snprintf( buffer, 99, "Die Vergiftung schadet\n%s%s.[A]", ( acpkmn->m_boxdata.m_name ),
                      ( p_opponent ? " [OPPONENT]" : "" ) );
            log( buffer );

            u16 oldHP              = acpkmn->m_stats.m_acHP;
            acpkmn->m_stats.m_acHP = std::max(
                u16( 0 ), u16( acpkmn->m_stats.m_acHP - 1.0 / ( 8 * acpkmn->m_stats.m_maxHP ) ) );

            _battleUI.updateHP( p_opponent, p_pokemonPos, oldHP );
        }
        if( acpkmn->m_status.m_isBadlyPoisoned && acpkmn->m_boxdata.m_ability != A_POISON_HEAL ) {
            snprintf( buffer, 99, "Die Vergiftung schadet\n%s%s.[A]", ( acpkmn->m_boxdata.m_name ),
                      ( p_opponent ? " [OPPONENT]" : "" ) );
            log( buffer );

            auto str = CUR_PKMN_STR( p_pokemonPos, p_opponent );

            u16 oldHP              = acpkmn->m_stats.m_acHP;
            acpkmn->m_stats.m_acHP = std::max(
                u16( 0 ), u16( acpkmn->m_stats.m_acHP
                               - ( ++str.m_toxicCount ) / ( 16.0 * acpkmn->m_stats.m_maxHP ) ) );

            _battleUI.updateHP( p_opponent, p_pokemonPos, oldHP );
        }
    }

    void battle::handleFaint( bool p_opponent, u8 p_pokemonPos, bool p_show ) {
        if( m_battleMode != DOUBLE && p_pokemonPos ) return;
        char buffer[ 100 ];
        if( m_isWildBattle && p_opponent ) {
            if( !_wildPokemon.m_pokemon->m_stats.m_acHP && _wildPokemon.m_acStatus != KO ) {
                if( p_show ) {
                    snprintf( buffer, 99, "%s [OPPONENT] wurde besiegt.[A]",
                              _wildPokemon.m_pokemon->m_boxdata.m_name );
                    log( buffer );
                }
                _wildPokemon.m_acStatus = KO;
                _battleUI.hidePKMN( p_opponent, p_pokemonPos );
                if( m_distributeEXP ) distributeEXP( p_opponent, p_pokemonPos );
            }
            return;
        }
        if( !CUR_PKMN( p_pokemonPos, p_opponent ).m_stats.m_acHP
            && _battleSpotOccupied[ p_pokemonPos ][ p_opponent ]
            && _battleUI.isVisiblePKMN( p_opponent, p_pokemonPos ) ) {
            if( p_show ) {
                snprintf( buffer, 99, "%s%s wurde besiegt.[A]",
                          ( CUR_PKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                          ( p_opponent ? " [OPPONENT]" : "" ) );
                log( buffer );
            }
            CUR_PKMN_STS( p_pokemonPos, p_opponent ) = KO;
            _battleUI.hidePKMN( p_opponent, p_pokemonPos );
            _battleSpotOccupied[ p_pokemonPos ][ p_opponent ] = false;

            if( m_distributeEXP ) distributeEXP( p_opponent, p_pokemonPos );
        }
    }

    /**
     *  @brief Registers the current participating PKMN
     */
    void battle::registerParticipatedPKMN( ) {
        /*   if( CUR_PKMN_STS( 0, PLAYER ) != KO && CUR_PKMN_STS( 0, OPPONENT ) != KO
               && CUR_PKMN_STS( 0, PLAYER ) != NA && CUR_PKMN_STS( 0, OPPONENT ) != NA )
               _participatedPKMN[ CUR_POS( 0, PLAYER ) ][ PLAYER ] |= ( 1 << CUR_POS( 0, OPPONENT )
           ); if( CUR_PKMN_STS( 0, PLAYER ) != KO && CUR_PKMN_STS( 0, OPPONENT ) != KO
               && CUR_PKMN_STS( 0, PLAYER ) != NA && CUR_PKMN_STS( 0, OPPONENT ) != NA )
               _participatedPKMN[ CUR_POS( 0, OPPONENT ) ][ OPPONENT ] |= ( 1 << CUR_POS( 0, PLAYER
           ) );

           if( m_battleMode == DOUBLE ) {
               if( CUR_PKMN_STS( 0, PLAYER ) != KO && CUR_PKMN_STS( 1, OPPONENT ) != KO
                   && CUR_PKMN_STS( 0, PLAYER ) != NA && CUR_PKMN_STS( 1, OPPONENT ) != NA )
                   _participatedPKMN[ CUR_POS( 0, PLAYER ) ][ PLAYER ] |= ( 1 << CUR_POS( 1,
           OPPONENT ) ); if( CUR_PKMN_STS( 1, PLAYER ) != KO && CUR_PKMN_STS( 0, OPPONENT ) != KO
                   && CUR_PKMN_STS( 1, PLAYER ) != NA && CUR_PKMN_STS( 0, OPPONENT ) != NA )
                   _participatedPKMN[ CUR_POS( 0, OPPONENT ) ][ OPPONENT ] |= ( 1 << CUR_POS( 1,
           PLAYER ) );

               if( CUR_PKMN_STS( 1, PLAYER ) != KO && CUR_PKMN_STS( 0, OPPONENT ) != KO
                   && CUR_PKMN_STS( 1, PLAYER ) != NA && CUR_PKMN_STS( 0, OPPONENT ) != NA )
                   _participatedPKMN[ CUR_POS( 1, PLAYER ) ][ PLAYER ] |= ( 1 << CUR_POS( 0,
           OPPONENT ) ); if( CUR_PKMN_STS( 0, PLAYER ) != KO && CUR_PKMN_STS( 1, OPPONENT ) != KO
                   && CUR_PKMN_STS( 0, PLAYER ) != NA && CUR_PKMN_STS( 1, OPPONENT ) != NA )
                   _participatedPKMN[ CUR_POS( 1, OPPONENT ) ][ OPPONENT ] |= ( 1 << CUR_POS( 0,
           PLAYER ) );

               if( CUR_PKMN_STS( 1, PLAYER ) != KO && CUR_PKMN_STS( 1, OPPONENT ) != KO
                   && CUR_PKMN_STS( 1, PLAYER ) != NA && CUR_PKMN_STS( 1, OPPONENT ) != NA )
                   _participatedPKMN[ CUR_POS( 1, PLAYER ) ][ PLAYER ] |= ( 1 << CUR_POS( 1,
           OPPONENT ) ); if( CUR_PKMN_STS( 1, PLAYER ) != KO && CUR_PKMN_STS( 1, OPPONENT ) != KO
                   && CUR_PKMN_STS( 1, PLAYER ) != NA && CUR_PKMN_STS( 1, OPPONENT ) != NA )
                   _participatedPKMN[ CUR_POS( 1, OPPONENT ) ][ OPPONENT ] |= ( 1 << CUR_POS( 1,
           PLAYER ) );
           } */
    }

    /**
     *  @brief Distrobutes EXP.
     *  @param p_opponent: true iff the opponent's PKMN fainted.
     *  @param p_pokemonPos: Position of the fainted PKMN (0 or 1)
     */
    void battle::distributeEXP( bool p_opponent, u8 p_pokemonPos ) {
        (void) p_opponent;
        (void) p_pokemonPos;

        /*   u8 receivingPKMN = _participatedPKMN[ CUR_POS( p_pokemonPos, p_opponent ) ][ p_opponent
           ]; _participatedPKMN[ CUR_POS( p_pokemonPos, p_opponent ) ][ p_opponent ] = 0;

           float wildModifer = m_isWildBattle ? 1 : 1.5;
           pokemonData p;
           getAll( CUR_PKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, p );
           u16 b = p.m_EXPYield;

           bool printPkmnMsg = false;

           for( u8 i = 0; i < 6; ++i ) {
               u8 sz = ( !p_opponent ) ? _opponent->m_pkmnTeam.size( ) : _player->m_pkmnTeam.size(
           );

               if( sz <= i )
                   break;
               if( !( receivingPKMN & ( 1 << ( CUR_POS( i, PLAYER ) ) ) ) )
                   continue;

               u8 acidx = 42;
               for( int j = 0; j < sz; ++j )
                   if( CUR_POS( j, !p_opponent ) == i ) {
                       acidx = j;
                       break;
                   }
               if( acidx == 42 )
                   continue;

               auto& acPkmn = CUR_PKMN( acidx, !p_opponent );

               if( acPkmn.m_stats.m_acHP ) {
                   if( acPkmn.m_level == 100 )
                       continue;
                   float e = ( acPkmn.m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                   u8 L = acPkmn.m_level;

                   float t = ( acPkmn.m_boxdata.m_oTId == SAVE::SAV->getActiveFile( ).m_id
                               && acPkmn.m_boxdata.m_oTSid == SAVE::SAV->getActiveFile( ).m_sid ? 1
           : 1.5 );

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
                       acPkmn.m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2
           + 4 * ( acItem == I_POWER_WEIGHT ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 1 ] <= u8(
           252 ) ) acPkmn.m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4
           * ( acItem == I_POWER_BRACER ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 2 ] <= u8( 252 )
           ) acPkmn.m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * (
           acItem == I_POWER_BELT ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 3 ] <= u8( 252 ) )
                       acPkmn.m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2
           + 4 * ( acItem == I_POWER_ANKLET ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 4 ] <= u8(
           252 ) ) acPkmn.m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4
           * ( acItem == I_POWER_LENS ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 5 ] <= u8( 252 ) )
                       acPkmn.m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2
           + 4 * ( acItem == I_POWER_BAND ) ) );

                   if( acidx < 1 + ( m_battleMode == DOUBLE ) )
                       _battleUI.applyEXPChanges( !p_opponent, acidx, exp ); // Checks also for
           level-advancement of 1st (and in Doubles 2nd) PKMN else { //Advance the level here (this
           is NOT redundant boilerplate!

                       printPkmnMsg = true;


                       getAll( acPkmn.m_boxdata.m_speciesId, p );

                       bool newLevel = EXP[ L ][ p.m_expType ] <=
           acPkmn.m_boxdata.m_experienceGained; u16 HPdif = acPkmn.m_stats.m_maxHP -
           acPkmn.m_stats.m_acHP;

                       while( newLevel ) {
                           acPkmn.m_level++;

                           if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                               acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp
           + 2 * p.m_bases[ 0 ]
                                                            + ( acPkmn.m_boxdata.m_effortValues[ 0 ]
           / 4 ) + 100 )* acPkmn.m_level / 100 ) + 10; else acPkmn.m_stats.m_maxHP = 1; pkmnNatures
           nature = acPkmn.m_boxdata.getNature( );

                           acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack
           + 2 * p.m_bases[ ATK + 1 ]
                                                        + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1
           ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 ) * NatMod[ nature ][ ATK ]; acPkmn.m_stats.m_Def
           = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                                                        + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1
           ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ DEF ]; acPkmn.m_stats.m_Spd =
           ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                                                        + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1
           ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SPD ]; acPkmn.m_stats.m_SAtk
           = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                                                         + ( acPkmn.m_boxdata.m_effortValues[ SATK +
           1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SATK ];
                           acPkmn.m_stats.m_SDef = ( ( (
           acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                                                         + ( acPkmn.m_boxdata.m_effortValues[ SDEF +
           1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SDEF ];

                           acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

                           char buffer[ 50 ];
                           snprintf( buffer, 49, GET_STRING( 168 ), acPkmn.m_boxdata.m_name,
                               ( ( !p_opponent ) ? " [OPPONENT]" : "" ), acPkmn.m_level );
                           log( buffer );

                           checkForAttackLearn( i );
                           newLevel = acPkmn.m_level < 100 && EXP[ acPkmn.m_level ][ p.m_expType ]
           <= acPkmn.m_boxdata.m_experienceGained;
                       }
                   }
               }
           }

           if( printPkmnMsg )
               log( "Weitere Pokémon im Team\n erhalten E.-Punkte.[A]" );

           if( SAVE::SAV->getActiveFile( ).m_options.m_EXPShareEnabled && p_opponent ) {
               log( "Der EP-Teiler wirkt![A]" );
               for( u8 i = ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); i < 6; ++i )if( CUR_PKMN_STS( i,
           PLAYER ) != KO && CUR_PKMN_STS( i, PLAYER ) != NA ) {

                   if( CUR_PKMN( i, PLAYER ).m_level == 100 )
                       continue;
                   if( receivingPKMN & ( 1 << ( CUR_POS( i, PLAYER ) ) ) )
                       continue;

                   auto& acPkmn = CUR_PKMN( i, PLAYER );

                   float e = ( acPkmn.m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                   u8 L = acPkmn.m_level;

                   float t = ( acPkmn.m_boxdata.m_oTId == SAVE::SAV->getActiveFile( ).m_id
                               && acPkmn.m_boxdata.m_oTSid == SAVE::SAV->getActiveFile( ).m_sid ? 1
           : 1.5 );

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
                       acPkmn.m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2
           + 4 * ( acItem == I_POWER_WEIGHT ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 1 ] <= u8(
           252 ) ) acPkmn.m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4
           * ( acItem == I_POWER_BRACER ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 2 ] <= u8( 252 )
           ) acPkmn.m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * (
           acItem == I_POWER_BELT ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 3 ] <= u8( 252 ) )
                       acPkmn.m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2
           + 4 * ( acItem == I_POWER_ANKLET ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 4 ] <= u8(
           252 ) ) acPkmn.m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4
           * ( acItem == I_POWER_LENS ) ) ); if( acPkmn.m_boxdata.m_effortValues[ 5 ] <= u8( 252 ) )
                       acPkmn.m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2
           + 4 * ( acItem == I_POWER_BAND ) ) );

                   //Check for level-advancing

                   getAll( acPkmn.m_boxdata.m_speciesId, p );

                   bool newLevel = EXP[ L ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                   u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;

                   while( newLevel ) {
                       acPkmn.m_level++;

                       if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                           acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2
           * p.m_bases[ 0 ]
                                                        + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4
           ) + 100 )* acPkmn.m_level / 100 ) + 10; else acPkmn.m_stats.m_maxHP = 1; pkmnNatures
           nature = acPkmn.m_boxdata.getNature( );

                       acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2
           * p.m_bases[ ATK + 1 ]
                                                    + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ]
           >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 ) * NatMod[ nature ][ ATK ]; acPkmn.m_stats.m_Def =
           ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                                                    + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ]
           >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ DEF ]; acPkmn.m_stats.m_Spd = (
           ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                                                    + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ]
           >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SPD ]; acPkmn.m_stats.m_SAtk =
           ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                                                     + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ]
           >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SATK ]; acPkmn.m_stats.m_SDef =
           ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                                                     + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ]
           >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SDEF ];

                       acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

                       char buffer[ 50 ];
                       snprintf( buffer, 49, GET_STRING( 168 ), acPkmn.m_boxdata.m_name,
           acPkmn.m_level ); log( buffer );

                       checkForAttackLearn( i );

                       newLevel = acPkmn.m_level < 100 && EXP[ acPkmn.m_level ][ p.m_expType ] <=
           acPkmn.m_boxdata.m_experienceGained;
                   }
               }
           } */
    }

    /**
     *  @brief Checks if the own PKMN at position p_pokemonPos
     *  @param p_pokemonPos: Position of the PKMN which coul learn new moves (0 to 5)
     */
    void battle::checkForAttackLearn( u8 p_pokemonPos ) {
        const int MAX_ATTACKS_PER_LEVEL = 10;
        u16       learnable[ MAX_ATTACKS_PER_LEVEL ];
        auto&     acPkmn = CUR_PKMN( p_pokemonPos, PLAYER );

        getLearnMoves( acPkmn.m_boxdata.m_speciesId, acPkmn.m_level, acPkmn.m_level, 1,
                       MAX_ATTACKS_PER_LEVEL, learnable );

        for( u8 i = 0; i < MAX_ATTACKS_PER_LEVEL; ++i ) {
            if( !learnable[ i ] ) break;
            _battleUI.learnMove( p_pokemonPos, learnable[ i ] );
        }
    }

    /**
     *  @brief Tests if the specified PKMN can perform an evolution, and performs it, if possible.
     *  @param p_opponent: true iff the opponent's PKMN shall be tested.
     *  @param p_pokemonPos: Position of the PKMN to be tested (0 or 1)
     */
    void battle::checkForEvolution( bool p_opponent, u8 p_pokemonPos ) {
        if( !SAVE::SAV->getActiveFile( ).m_options.m_evolveInBattle ) return;

        if( CUR_PKMN( p_pokemonPos, p_opponent ).canEvolve( ) ) {
            auto& acPkmn = CUR_PKMN( p_pokemonPos, p_opponent );

            char buffer[ 50 ];
            snprintf( buffer, 49, GET_STRING( 51 ), acPkmn.m_boxdata.m_name );
            log( buffer );

            acPkmn.evolve( );
            _battleUI.evolvePKMN( p_opponent, p_pokemonPos );

            snprintf( buffer, 49, GET_STRING( 52 ),
                      getDisplayName( acPkmn.m_boxdata.m_speciesId ).c_str( ) );
            log( buffer );
        }
    }

    /**
     *  @brief Checks if the battle has to end
     *  @param p_battleEndReason [in/out]: Stores the reason for an end of the battle.
     *  @returns True iff the battle has to end
     */
    bool battle::endConditionHit( battleEndReason& p_battleEndReason ) {
        // Check round limit
        if( _round >= _maxRounds && _maxRounds ) {
            p_battleEndReason = battleEndReason::ROUND_LIMIT;
            return true;
        }

        // Check amount of non-koed PKMN
        // PLAYER
        u8 pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( _player->m_pkmnTeam.size( ) > i ) {
                if( CUR_PKMN_STS( i, PLAYER ) != KO && CUR_PKMN_STS( i, PLAYER ) != NA
                    && CUR_PKMN( i, PLAYER ).m_stats.m_acHP )
                    pkmnCnt++;
            } else
                break;
        }
        if( !pkmnCnt ) {
            p_battleEndReason = battleEndReason::OPPONENT_WON;
            return true;
        }

        if( m_isWildBattle ) {
            if( _wildPokemon.m_acStatus != KO ) return false;
            p_battleEndReason = battleEndReason::PLAYER_WON;
            return true;
        }

        // OPPONENT
        pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( _opponent->m_pkmnTeam.size( ) > i ) {
                if( CUR_PKMN_STS( i, OPPONENT ) != KO && CUR_PKMN_STS( i, OPPONENT ) != NA
                    && CUR_PKMN( i, OPPONENT ).m_stats.m_acHP )
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
        char buffer[ 50 ];
        switch( p_battleEndReason ) {
        case BATTLE::battle::ROUND_LIMIT:
            log( "Das Rundenlimit dieses\nKampfes wurde erreicht.[A]" );
            _battleUI.showEndScreen( );
            log( "Der Kampf endet in einem\nUnentschieden![A]" );
            break;
        case BATTLE::battle::OPPONENT_WON: {
            if( !m_isWildBattle ) {
                snprintf( buffer, 49, "[TRAINER] [TCLASS] gewinnt…[A]" );

                _battleUI.showEndScreen( );

                snprintf( buffer, 49, "%s[A]", _opponent->getWinMsg( ) );
                log( buffer );
            }
            break;
        }
        case BATTLE::battle::PLAYER_WON: {
            if( !m_isWildBattle ) {
                log( "Du besiegst [TCLASS] [TRAINER]![A]" );

                _battleUI.showEndScreen( );

                snprintf( buffer, 49, "%s[A]", _opponent->getLooseMsg( ) );
                log( buffer );
                snprintf( buffer, 49, "Du gewinnst %d$.[A]", _opponent->getLooseMoney( ) );
                log( buffer );
            }
            break;
        }
        case BATTLE::battle::RUN: {
            break;
        }
        default:
            log( "Der Kampf endet.[A]" );
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
        char buffer[ 100 ];
        if( p_opponent )
            snprintf( buffer, 99,
                      "[OPP%d] wurde von [TRAINER]\n([TCLASS]) auf die Bank geschickt.[A]",
                      p_pokemonPos + 1 );
        else
            snprintf( buffer, 99, "Auf die Bank [OWN%d]![A]", p_pokemonPos + 1 );
        log( buffer );

        _battleUI.hidePKMN( p_opponent, p_pokemonPos );

        std::swap( CUR_POS( p_pokemonPos, p_opponent ), CUR_POS( p_newPokemonPos, p_opponent ) );

        _battleUI.sendPKMN( p_opponent, p_pokemonPos );
    }

    /**
     *  @brief Applys weather effects.
     */
    void battle::doWeather( ) {
        if( m_weather != NO_WEATHER ) {
            if( --_weatherLength ) {
                log( weatherMessage[ m_weather ] );
                weatherEffects[ m_weather ].execute( *this, this );
            } else {
                log( weatherEndMessage[ m_weather ] );
                m_weather = NO_WEATHER;
            }
        }
    }

    /**
     *  @brief Checks whether the player can run from the wild Pokémon.
     */
    bool battle::run( ) {
        // Check whether run is succesful -- TODO
        _endBattle = true;
        return true;
    }

    /**
     * @brief Tries to capture with the specified poké ball
     * @param p_pokeball: item idx of the thrown poke ball
     * @returns true iff the capture was successful
     */
    bool battle::tryCapture( u16 p_pokeBall ) {
        u16         ballCatchRate = 2;
        u16         specId        = _wildPokemon.m_pokemon->m_boxdata.m_speciesId;
        pokemonData p;
        getAll( specId, p );
        switch( p_pokeBall ) {
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
            if( CUR_PKMN( 0, PLAYER ).m_level > _wildPokemon.m_pokemon->m_level ) ballCatchRate = 4;
            if( CUR_PKMN( 0, PLAYER ).m_level / 2 > _wildPokemon.m_pokemon->m_level )
                ballCatchRate = 8;
            if( CUR_PKMN( 0, PLAYER ).m_level / 4 > _wildPokemon.m_pokemon->m_level )
                ballCatchRate = 16;
            break;
        case I_LURE_BALL:
            if( PLAYER_IS_FISHING ) ballCatchRate = 6;
            break;
        case I_MOON_BALL:
            if( _wildPokemon.m_pokemon->canEvolve( I_MOON_STONE ) ) ballCatchRate = 16;
            break;
        case I_LOVE_BALL:
            if( _wildPokemon.m_pokemon->m_boxdata.m_isFemale
                != CUR_PKMN( 0, PLAYER ).m_boxdata.m_isFemale )
                ballCatchRate = 16;
            break;
        case I_HEAVY_BALL:
            ballCatchRate = std::min( 128, p.m_weight >> 2 );
            break;
        case I_FAST_BALL:
            if( p.m_bases[ 5 ] >= 100 ) ballCatchRate = 16;
            break;

        case I_REPEAT_BALL:
            if( SAVE::SAV->m_caughtPkmn[ specId / 8 ] & ( 1 << ( specId % 8 ) ) ) ballCatchRate = 6;
            break;
        case I_TIMER_BALL:
            ballCatchRate = std::min( _round + 10 / 5, 8 );
            break;
        case I_NEST_BALL:
            ballCatchRate = std::max( ( 40 - _wildPokemon.m_pokemon->m_level ) / 5, 2 );
            break;
        case I_NET_BALL:
            if( p.m_types[ 0 ] == type::BUG || p.m_types[ 1 ] == type::BUG
                || p.m_types[ 0 ] == type::WATER || p.m_types[ 1 ] == type::WATER )
                ballCatchRate = 6;
            break;
        case I_DIVE_BALL:
            if( SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::moveMode::DIVE )
                ballCatchRate = 7;
            break;

        case I_QUICK_BALL:
            if( _round < 2 ) ballCatchRate = 10;
            break;
        case I_DUSK_BALL:
            if( AttackList[ M_DIG ]->possible( ) || getCurrentDaytime( ) == 4 ) ballCatchRate = 7;
            break;

        default:
            break;
        }

        u8 status = 2;
        if( _wildPokemon.m_pokemon->m_status.m_isAsleep
            || _wildPokemon.m_pokemon->m_status.m_isFrozen )
            status = 4;
        if( _wildPokemon.m_pokemon->m_status.m_isParalyzed
            || _wildPokemon.m_pokemon->m_status.m_isPoisoned
            || _wildPokemon.m_pokemon->m_status.m_isBadlyPoisoned
            || _wildPokemon.m_pokemon->m_status.m_isBurned )
            status = 3;

        u32 catchRate = ( 3 * _wildPokemon.m_pokemon->m_stats.m_maxHP
                          - 2 * _wildPokemon.m_pokemon->m_stats.m_acHP )
                        * p.m_catchrate * ballCatchRate / 3
                        / _wildPokemon.m_pokemon->m_stats.m_maxHP * status;
        u32 pr   = u32( ( 65535 << 4 ) / ( sqrt( sqrt( ( 255L << 18 ) / catchRate ) ) ) );
        u8  succ = 0;
        for( u8 i = 0; i < 4; ++i ) {
            u16 rn = rand( );
            if( rn > pr ) break;
            succ++;
        }
        _battleUI.capture( p_pokeBall, succ );
        return ( succ == 4 );
    }

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE
    //////////////////////////////////////////////////////////////////////////
} // namespace BATTLE
