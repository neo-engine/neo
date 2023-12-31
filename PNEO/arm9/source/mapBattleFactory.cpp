/*
Pokémon neo
------------------------------

file        : mapBattleFactory.cpp
author      : Philip Wellnitz
description : Battle Factory / Slateport battle tent

Copyright (C) 2023 - 2023
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

#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "fs/data.h"
#include "fs/fs.h"
#include "gen/trainerClassNames.h"
#include "io/choiceBox.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/uio.h"
#include "map/mapBattleFacilityDefines.h"
#include "map/mapDrawer.h"
#include "sound/sound.h"
#include "sts/partyScreen.h"

namespace MAP {
    constexpr u8 SPECIAL_BATTLE_1 = 21;
    constexpr u8 SPECIAL_BATTLE_2 = 42;

    u8 nextOpponentStrategy( ) {
        u8 categories[ 7 ] = { 0 };

        for( auto i = 0; i < 6; ++i ) {
            if( !NEXT_OPPONENT_TEAM[ i ].m_speciesId ) { break; }

            for( auto j = 0; j < 4; ++j ) {
                switch( NEXT_OPPONENT_TEAM[ i ].m_moves[ j ] ) {
                case M_ACID_ARMOR:
                case M_AGILITY:
                case M_AMNESIA:
                case M_BARRIER:
                case M_BELLY_DRUM:
                case M_BULK_UP:
                case M_CALM_MIND:
                case M_CHARGE:
                case M_CONVERSION:
                case M_CONVERSION_2:
                case M_COSMIC_POWER:
                case M_DEFENSE_CURL:
                case M_DOUBLE_TEAM:
                case M_DRAGON_DANCE:
                case M_FOCUS_ENERGY:
                case M_GROWTH:
                case M_HARDEN:
                case M_HOWL:
                case M_IRON_DEFENSE:
                case M_MEDITATE:
                case M_MINIMIZE:
                case M_PSYCH_UP:
                case M_SHARPEN:
                case M_SWORDS_DANCE:
                case M_TAIL_GLOW:
                case M_WITHDRAW:
                case M_QUIVER_DANCE: categories[ 0 ]++; break;

                case M_ATTRACT:
                case M_BLOCK:
                case M_CONFUSE_RAY:
                case M_DISABLE:
                case M_ENCORE:
                case M_FLATTER:
                case M_GLARE:
                case M_GRASS_WHISTLE:
                case M_HYPNOSIS:
                case M_IMPRISON:
                case M_LEECH_SEED:
                case M_LOVELY_KISS:
                case M_MEAN_LOOK:
                case M_POISON_GAS:
                case M_POISON_POWDER:
                case M_SING:
                case M_SLEEP_POWDER:
                case M_SNATCH:
                case M_SPIDER_WEB:
                case M_SPIKES:
                case M_SPORE:
                case M_STUN_SPORE:
                case M_SUPERSONIC:
                case M_SWAGGER:
                case M_SWEET_KISS:
                case M_TAUNT:
                case M_TEETER_DANCE:
                case M_THUNDER_WAVE:
                case M_TORMENT:
                case M_TOXIC:
                case M_YAWN:
                case M_WILL_O_WISP: categories[ 1 ]++; break;

                case M_AROMATHERAPY:
                case M_BATON_PASS:
                case M_DETECT:
                case M_ENDURE:
                case M_HAZE:
                case M_HEAL_BELL:
                case M_INGRAIN:
                case M_LIGHT_SCREEN:
                case M_MAGIC_COAT:
                case M_MILK_DRINK:
                case M_MIST:
                case M_MOONLIGHT:
                case M_MORNING_SUN:
                case M_MUD_SPORT:
                case M_PROTECT:
                case M_RECOVER:
                case M_REFLECT:
                case M_REST:
                case M_SAFEGUARD:
                case M_SLACK_OFF:
                case M_SOFT_BOILED:
                case M_SWALLOW:
                case M_SYNTHESIS:
                case M_RECYCLE:
                case M_REFRESH:
                case M_WATER_SPORT:
                case M_WISH: categories[ 2 ]++; break;

                case M_BIDE:
                case M_BLAST_BURN:
                case M_COUNTER:
                case M_DESTINY_BOND:
                case M_DOUBLE_EDGE:
                case M_EXPLOSION:
                case M_FACADE:
                case M_FISSURE:
                case M_FLAIL:
                case M_FOCUS_PUNCH:
                case M_FRENZY_PLANT:
                case M_GRUDGE:
                case M_GUILLOTINE:
                case M_HORN_DRILL:
                case M_HYDRO_CANNON:
                case M_HYPER_BEAM:
                case M_MEMENTO:
                case M_MIRROR_COAT:
                case M_OVERHEAT:
                case M_PAIN_SPLIT:
                case M_PERISH_SONG:
                case M_PSYCHO_BOOST:
                case M_REVERSAL:
                case M_SELF_DESTRUCT:
                case M_SKY_ATTACK:
                case M_VOLT_TACKLE: categories[ 3 ]++; break;

                case M_CHARM:
                case M_COTTON_SPORE:
                case M_FAKE_TEARS:
                case M_FEATHER_DANCE:
                case M_FLASH:
                case M_GROWL:
                case M_KINESIS:
                case M_KNOCK_OFF:
                case M_LEER:
                case M_METAL_SOUND:
                case M_SAND_ATTACK:
                case M_SCARY_FACE:
                case M_SCREECH:
                case M_SMOKESCREEN:
                case M_SPITE:
                case M_STRING_SHOT:
                case M_SWEET_SCENT:
                case M_TAIL_WHIP:
                case M_TICKLE: categories[ 4 ]++; break;

                case M_ASSIST:
                case M_CAMOUFLAGE:
                case M_CURSE:
                case M_FOLLOW_ME:
                case M_METRONOME:
                case M_MIMIC:
                case M_MIRROR_MOVE:
                case M_PRESENT:
                case M_ROLE_PLAY:
                case M_SKETCH:
                case M_SKILL_SWAP:
                case M_SUBSTITUTE:
                case M_TRANSFORM:
                case M_TRICK: categories[ 5 ]++; break;

                case M_HAIL:
                case M_RAIN_DANCE:
                case M_SANDSTORM:
                case M_SUNNY_DAY:
                case M_WEATHER_BALL: categories[ 6 ]++; break;

                default: break;
                }
            }
        }

        auto activeCatCount = 0;
        u8   res            = 0;
        if( categories[ 0 ] > 2 ) {
            activeCatCount++;
            res = 1;
        }
        if( categories[ 1 ] > 2 ) {
            activeCatCount++;
            res = 2;
        }
        if( categories[ 2 ] > 2 ) {
            activeCatCount++;
            res = 3;
        }
        if( categories[ 3 ] > 1 ) {
            activeCatCount++;
            res = 4;
        }
        if( categories[ 4 ] > 1 ) {
            activeCatCount++;
            res = 5;
        }
        if( categories[ 5 ] > 1 ) {
            activeCatCount++;
            res = 6;
        }
        if( categories[ 6 ] > 1 ) {
            activeCatCount++;
            res = 7;
        }

        if( activeCatCount >= 3 ) { res = 8; }
        return res;
    }

    void mapDrawer::runBattleFactory( const ruleSet& p_rules ) {
        if( p_rules.m_id != RSID_SINGLE_LV50 && p_rules.m_id != RSID_SINGLE_LV100
            && p_rules.m_id != RSID_SINGLE_LV30 ) {
            return;
        }

        // obtain basic data
        auto streakVar
            = p_rules.m_id == RSID_SINGLE_LV50
                  ? SAVE::V_BATTLE_FACTORY_50_STREAK
                  : ( p_rules.m_id == RSID_SINGLE_LV100 ? SAVE::V_BATTLE_FACTORY_100_STREAK
                                                        : SAVE::V_SLATEPORT_BATTLE_TENT_STREAK );
        auto streakActiveFlag = p_rules.m_id == RSID_SINGLE_LV50
                                    ? SAVE::F_BATTLE_FACTORY_50_STREAK_ONGOING
                                    : ( p_rules.m_id == RSID_SINGLE_LV100
                                            ? SAVE::F_BATTLE_FACTORY_100_STREAK_ONGOING
                                            : SAVE::F_SLATEPORT_BATTLE_TENT_STREAK_ONGOING );
        u16  currentStreak    = SAVE::SAV.getActiveFile( ).getVar( streakVar );
        bool streakOngoing    = SAVE::SAV.getActiveFile( ).checkFlag( streakActiveFlag );
        auto battlePolicy     = getBattlePolicy( false, p_rules.m_battleMode, false );

        bool isTentMode = p_rules.m_id == RSID_SINGLE_LV30;

        auto streakfortier
            = p_rules.m_id == RSID_SINGLE_LV30 ? 0 : std::min( IV_MAX_STREAK, currentStreak / 7 );

        if( !streakOngoing ) {
            currentStreak = 0;
            SAVE::SAV.getActiveFile( ).setVar( streakVar, currentStreak );
            SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, true );
        }
        auto numPokemon = p_rules.m_battleMode == BATTLE::BM_SINGLE ? 3 : 4;

        // let me hold your pkmn
        printMapMessage( GET_MAP_STRING( 519 ), MSG_NORMAL );

        // initial pkmn selection
        memset( PLAYER_TEMP_TEAM, 0, sizeof( PLAYER_TEMP_TEAM ) );
        for( auto i = 0; i < 6; ) {
            auto      idx = bfPkmnForStreak( streakfortier, isTentMode );
            bfPokemon tmp;
            if( isTentMode ) {
                FS::loadBFPokemonTent( &tmp, idx );
            } else {
                FS::loadBFPokemon( &tmp, idx );
            }

            PLAYER_TEMP_TEAM[ i ] = pokemon( tmp, p_rules.m_level, IV_FOR_STREAK[ streakfortier ] );

            // check that the selection satisfies item and species clause
            bool bad = false;
            for( auto j = 0; j < i; ++j ) {
                if( PLAYER_TEMP_TEAM[ i ].getSpecies( ) == PLAYER_TEMP_TEAM[ j ].getSpecies( )
                    || PLAYER_TEMP_TEAM[ i ].getItem( ) == PLAYER_TEMP_TEAM[ j ].getItem( ) ) {
                    bad = true;
                }
            }
            if( !bad ) { ++i; }
        }
        // heal pkmn to make sure pp are correct
        for( auto i = 0; i < 6; ++i ) { PLAYER_TEMP_TEAM[ i ].heal( ); }

        u8 resultBP = 0;

        bool walkDown = false;
        for( u8 battle = 0; battle < p_rules.m_numBattles; ++battle ) {
            if( battle ) {
                // in-between battle message, heal pkmn team

                movePlayer( LEFT, false );
                movePlayer( LEFT, false );
                movePlayer( UP, false );

                // let me heal your pkmn
                printMapMessage( GET_MAP_STRING( 520 ), MSG_NORMAL );
                for( auto i = 0; i < numPokemon; ++i ) { PLAYER_TEMP_TEAM[ i ].heal( ); }

                // ask if player wants to continue
                if( IO::yesNoBox::NO
                    == IO::yesNoBox( ).getResult(
                        convertMapString( GET_MAP_STRING( 520 + battle ), MSG_NORMAL ).c_str( ),
                        MSG_NORMAL ) ) {
                    IO::init( );

                    movePlayer( DOWN, false );
                    movePlayer( RIGHT, false );
                    movePlayer( RIGHT, false );
                    // reset streak
                    SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, false );
                    break;
                }
                IO::init( );
            }

            bfPokemon pkmncache[ 6 ];
            std::memcpy( pkmncache, NEXT_OPPONENT_TEAM, sizeof( NEXT_OPPONENT_TEAM ) );

            u8 picnum = 0;

            // create next opponent
            if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_1 ) {
                // load special battle 1
                picnum = 19; // frontier brain
                printMapMessage( GET_MAP_STRING( 869 ), MSG_NORMAL );
            } else if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_2 ) {
                // load special battle 2
                picnum = 19; // frontier brain
                printMapMessage( GET_MAP_STRING( 869 ), MSG_NORMAL );
            } else {
                if( !createNextOpponentTrainer( p_rules, streakfortier, true, isTentMode ) ) {
                    SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, false );
                    break;
                }
                picnum = NEXT_OPPONENT.m_picnum;
                printMapMessage( GET_MAP_STRING( 860 + nextOpponentStrategy( ) ), MSG_NORMAL );
            }

            if( battle ) {
                // ask for pkmn swap
                if( IO::yesNoBox::YES
                    == IO::yesNoBox( ).getResult(
                        convertMapString( GET_MAP_STRING( 529 ), MSG_NORMAL ).c_str( ),
                        MSG_NORMAL ) ) {
                    // select one pkmn from opponent
                    IO::init( );

                    std::vector<std::string> pkmnChoices{ };
                    for( auto i = 0; i < numPokemon; ++i ) {
                        pkmnChoices.push_back( FS::getDisplayName( pkmncache[ i ].m_speciesId ) );
                    }
                    IO::choiceBox tos = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
                    auto          res = tos.getResult(
                        convertMapString( GET_MAP_STRING( 875 ), MSG_NOCLOSE ).c_str( ),
                        MSG_NOCLOSE, pkmnChoices, true );
                    IO::init( );

                    if( res < numPokemon ) {
                        // select one pkmn from own pkmn
                        printMapMessage( GET_MAP_STRING( 876 ), MSG_NORMAL );

                        ANIMATE_MAP = false;
                        IO::clearScreen( false );
                        videoSetMode( MODE_5_2D );
                        bgUpdate( );

                        STS::partyScreen sts
                            = STS::partyScreen( PLAYER_TEMP_TEAM, numPokemon, false, false, false,
                                                1, true, true, false );

                        SOUND::dimVolume( );

                        auto res2 = sts.run( );

                        FADE_TOP_DARK( );
                        FADE_SUB_DARK( );
                        IO::clearScreen( false );
                        videoSetMode( MODE_5_2D );
                        IO::resetScale( true, false );
                        bgUpdate( );

                        ANIMATE_MAP = true;
                        SOUND::restoreVolume( );

                        IO::init( );
                        MAP::curMap->draw( );

                        if( res2.getSelectedPkmn( ) < 255 ) {
                            PLAYER_TEMP_TEAM[ res2.getSelectedPkmn( ) ] = pokemon(
                                pkmncache[ res ], p_rules.m_level, IV_FOR_STREAK[ streakfortier ] );
                            printMapMessage( GET_MAP_STRING( 531 ), MSG_NORMAL );
                        }
                    }
                } else {
                    IO::init( );
                }

                movePlayer( DOWN, false );
                movePlayer( RIGHT, false );
                movePlayer( RIGHT, false );
            } else {
                // initial pkmn selection
                printMapMessage( GET_MAP_STRING( 870 ), MSG_NORMAL );

                ANIMATE_MAP = false;
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                STS::partyScreen sts = STS::partyScreen( PLAYER_TEMP_TEAM, 6, false, false, false,
                                                         numPokemon, true, true, false );

                SOUND::dimVolume( );

                auto res = sts.run( );

                FADE_TOP_DARK( );
                FADE_SUB_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                IO::resetScale( true, false );
                bgUpdate( );

                ANIMATE_MAP = true;
                SOUND::restoreVolume( );

                IO::init( );
                MAP::curMap->draw( );

                if( res.getSelectedPkmn( ) == 255 ) {
                    // player aborted
                    SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, false );
                    printMapMessage( GET_MAP_STRING( 532 ), MSG_NORMAL );
                    return;
                }

                // remove unselected pkmn
                // (could do the cheaper in-place sorting, but the added code complexity is not
                // really worth the effort.)
                pokemon cache[ 6 ];
                for( auto i = 0; i < numPokemon; ++i ) {
                    memcpy( &cache[ i ], &PLAYER_TEMP_TEAM[ res.getSelectedPkmn( i + 1 ) ],
                            sizeof( pokemon ) );
                }
                std::memset( &PLAYER_TEMP_TEAM, 0, sizeof( PLAYER_TEMP_TEAM ) );
                for( auto i = 0; i < numPokemon; ++i ) {
                    memcpy( &PLAYER_TEMP_TEAM[ i ], &cache[ i ], sizeof( pokemon ) );
                }

                printMapMessage( GET_MAP_STRING( 874 ), MSG_NORMAL );

                // walk player to battle field
                for( auto i = 0; i < 4; ++i ) { movePlayer( UP, false ); }
                redirectPlayer( RIGHT, false, true );
                walkDown = true;
            }

            // walk in opponent

            // spawn opponent
            u16       curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
            u16       cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
            u16       mapX = curx / SIZE, mapY = cury / SIZE;
            mapObject obj         = mapObject( );
            obj.m_pos             = { u16( mapX * SIZE + 16 ), u16( mapY * SIZE + 12 ), 3 };
            obj.m_picNum          = picnum;
            obj.m_movement        = NO_MOVEMENT;
            obj.m_range           = 0;
            obj.m_direction       = DOWN;
            obj.m_currentMovement = movement{ obj.m_direction, 0 };

            std::pair<u8, mapObject> cur = { 0, obj };
            loadMapObject( cur );

            u8 found = 255;
            for( u8 i = _fixedObjectCount; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                if( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].first == UNUSED_MAPOBJECT ) {
                    found = i;
                    break;
                }
            }
            if( found == 255 ) { found = SAVE::SAV.getActiveFile( ).m_mapObjectCount++; }

            // walk down opponent
            movement m = { DOWN, 0 };
            _mapSprites.setFrameD( cur.first, DOWN );

            for( u8 j = 0; j < 4; ++j ) {
                for( u8 i = 0; i < 16; ++i ) {
                    moveMapObject( cur.second, cur.first, m, false,
                                   SAVE::SAV.getActiveFile( ).m_player.m_direction );
                    m.m_frame = ( m.m_frame + 1 ) & 15;
                    swiWaitForVBlank( );
                }
            }

            cur.second.m_currentMovement = { LEFT, 0 };
            cur.second.m_direction       = LEFT;
            _mapSprites.setFrameD( cur.first, LEFT );
            SAVE::SAV.getActiveFile( ).m_mapObjects[ found ] = cur;

            // print pre-battle message

            printMapMessage( NEXT_OPPONENT.m_beforeBattle.construct( ).c_str( ), MSG_NORMAL );

            // run battle
            auto playerPrio = _mapSprites.getPriority( _playerSprite );
            FADE_TOP_DARK( );
            ANIMATE_MAP = false;
            swiWaitForVBlank( );

            // copy team to prevent losing items
            pokemon copy[ 6 ];
            for( auto i = 0; i < numPokemon; ++i ) {
                PLAYER_TEMP_TEAM[ i ].heal( );
                copy[ i ] = PLAYER_TEMP_TEAM[ i ];
            }
            BATTLE::battle::battleEndReason result;

            if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_1 ) {
                // load special battle 1
                SOUND::playBGM( BGM_BATTLE_FRONTIER_BRAIN );
                // TODO
                printMapMessage( GET_MAP_STRING( 869 ), MSG_NORMAL );

                resultBP += 20;
            } else if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_2 ) {
                // load special battle 2
                SOUND::playBGM( BGM_BATTLE_FRONTIER_BRAIN );
                // TODO
                printMapMessage( GET_MAP_STRING( 869 ), MSG_NORMAL );

                resultBP += 20;
            } else {
                SOUND::playBGM( BGM_BATTLE_FRONTIER_TRAINER );
                BATTLE::battle bt = BATTLE::battle( copy, numPokemon, NEXT_OPPONENT,
                                                    NEXT_OPPONENT_TEAM, numPokemon, p_rules.m_level,
                                                    IV_FOR_STREAK[ streakfortier ], battlePolicy );
                result            = bt.start( false );
            }

            FADE_TOP_DARK( );
            IO::init( );
            draw( playerPrio );
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
            cur = SAVE::SAV.getActiveFile( ).m_mapObjects[ found ];
            _mapSprites.setFrameD( cur.first, LEFT );
            SOUND::restartBGM( );
            ANIMATE_MAP = true;

            // walk out opponent

            m = { UP, 0 };
            _mapSprites.setFrameD( cur.first, UP );

            for( u8 j = 0; j < 4; ++j ) {
                for( u8 i = 0; i < 16; ++i ) {
                    moveMapObject( cur.second, cur.first, m, false,
                                   SAVE::SAV.getActiveFile( ).m_player.m_direction );
                    m.m_frame = ( m.m_frame + 1 ) & 15;
                    swiWaitForVBlank( );
                }
            }

            // remove map object
            _mapSprites.destroySprite( cur.first );
            SAVE::SAV.getActiveFile( ).m_mapObjects[ found ] = { UNUSED_MAPOBJECT, mapObject( ) };

            if( result == BATTLE::battle::BATTLE_PLAYER_WON ) {
                // player won
                if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_1 ) {
                    // some message
                    // award siver symbol
                    awardBadge( 1, 11 );
                }
                if( p_rules.m_hasSpecialBattles && currentStreak + 1 == SPECIAL_BATTLE_2 ) {
                    // some message
                    // award gold symbol
                    awardBadge( 1, 12 );
                }

                currentStreak++;
                SAVE::SAV.getActiveFile( ).setVar( streakVar, currentStreak );
                // move player to clerk

            } else {
                // player lost
                // reset streak
                SAVE::SAV.getActiveFile( ).setFlag( streakActiveFlag, false );
                break;
            }
        }
        if( walkDown ) {
            // return player to initial position
            for( auto i = 0; i < 3; ++i ) { movePlayer( DOWN, false ); }
            redirectPlayer( RIGHT, false, true );
        }

        // return pkmn message
        printMapMessage( GET_MAP_STRING( 532 ), MSG_NORMAL );

        // check if chain is still active
        if( SAVE::SAV.getActiveFile( ).checkFlag( streakActiveFlag ) ) {
            // hand prize(s) to player, depending on the streak
            resultBP += ( currentStreak / 7 ) + 1;

            if( p_rules.m_id == RSID_SINGLE_LV30 ) {
                // hand out full heal
                printMapMessage( GET_MAP_STRING( 871 ), MSG_NORMAL );
                IO::giveItemToPlayer( I_PREMIER_BALL, 1 );
            } else {
                // hand out bp
                printMapMessage( GET_MAP_STRING( 872 ), MSG_NORMAL );
                char buffer[ 100 ];
                snprintf( buffer, 99, GET_MAP_STRING( 873 ), resultBP );
                SOUND::playSoundEffect( SFX_OBTAIN_ITEM );
                printMapMessage( buffer, MSG_INFO );
                SAVE::SAV.getActiveFile( ).m_battlePoints = std::min(
                    u16( -1 ), u16( SAVE::SAV.getActiveFile( ).m_battlePoints + resultBP ) );
            }
        }
    }
} // namespace MAP
