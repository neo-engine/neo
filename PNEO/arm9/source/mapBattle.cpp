/*
Pokémon neo
------------------------------

file        : mapBattle.cpp
author      : Philip Wellnitz
description : Map drawing engine: functions related to wild and trainer battles

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

#include <algorithm>

#include "bagViewer.h"
#include "battle.h"
#include "battleDefines.h"
#include "battleTrainer.h"
#include "defines.h"
#include "fs.h"
#include "gameStart.h"
#include "mapDrawer.h"
#include "nav.h"
#include "pokemonNames.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sound.h"
#include "sprite.h"
#include "uio.h"

namespace MAP {
    void mapDrawer::disablePkmn( s16 p_steps ) {
        SAVE::SAV.getActiveFile( ).m_repelSteps = p_steps;
    }

    void mapDrawer::enablePkmn( ) {
        SAVE::SAV.getActiveFile( ).m_repelSteps = 0;
    }

    void mapDrawer::handleWildPkmn( u16 p_globX, u16 p_globY ) {
        u8 moveData = atom( p_globX, p_globY ).m_movedata;
        u8 behave   = at( p_globX, p_globY ).m_bottombehave;

        if( SAVE::SAV.getActiveFile( ).m_repelSteps ) return;
        // handle Pkmn stuff
        if( moveData == MVD_SURF && behave != BEH_WATERFALL )
            handleWildPkmn( WATER );
        else if( behave == BEH_GRASS || behave == BEH_GRASS_ASH || behave == BEH_GRASS_UNDERWATER
                 || behave == BEH_SAND_WITH_ENCOUNTER_AND_FISH
                 || behave == BEH_CAVE_WITH_ENCOUNTER )
            handleWildPkmn( GRASS );
        else if( behave == BEH_LONG_GRASS )
            handleWildPkmn( HIGH_GRASS );
        //        else if( currentData( ).m_mapType & CAVE )
        //            handleWildPkmn( GRASS );
    }

    pokemon wildPkmn;
    bool    mapDrawer::handleWildPkmn( wildPkmnType p_type, bool p_forceEncounter ) {
        u16 rn
            = rand( ) % ( 512 + 3 * SAVE::SAV.getActiveFile( ).m_options.m_encounterRateModifier );
        if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) rn /= 8;
        if( p_forceEncounter ) rn %= 40;

        u8 tier;
        if( rn < 2 )
            tier = 4;
        else if( rn < 6 )
            tier = 3;
        else if( rn < 14 )
            tier = 2;
        else if( rn < 26 )
            tier = 1;
        else
            tier = 0;
        u8 level = SAVE::SAV.getActiveFile( ).getEncounterLevel( tier );

        if( rn > 40 || !level ) {
            if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) {
                _playerIsFast = false;
                NAV::printMessage( GET_STRING( 5 ) );
            }
            return false;
        }
        if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) {
            _playerIsFast = false;
            NAV::printMessage( GET_STRING( 6 ) );
        } else if( SAVE::SAV.getActiveFile( ).m_repelSteps && !p_forceEncounter ) {
            return false;
        }

        s8 availmod = ( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) - 3 ) / 3;

        u8 total = 0;
        for( u8 i = 0; i < currentData( ).m_pokemonDescrCount; ++i ) {
            if( currentData( ).m_pokemon[ i ].m_encounterType == p_type ) {
                s8 ownedbadge = SAVE::SAV.getActiveFile( ).getBadgeCount( ) + availmod;
                if( ownedbadge < 0 ) { ownedbadge = 0; }

                if( ownedbadge >= currentData( ).m_pokemon[ i ].m_slot ) {

                    if( currentData( ).m_pokemon[ i ].m_daytime
                        & ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) {
                        total += currentData( ).m_pokemon[ i ].m_encounterRate;
                    } else {
#ifdef DESQUID_MORE
                        NAV::printMessage(
                            ( std::string( "Ignoring pkmn due to wrong time: " )
                              + std::to_string( i ) + " "
                              + std::to_string( currentData( ).m_pokemon[ i ].m_daytime ) + " vs "
                              + std::to_string( ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) )
                                .c_str( ),
                            MSG_INFO );
#endif
                    }
                } else {
#ifdef DESQUID_MORE
                    NAV::printMessage( ( std::string( "Ignoring pkmn due to insufficient badges: " )
                                         + std::to_string( i ) )
                                           .c_str( ),
                                       MSG_INFO );
#endif
                }
            }
        }
        if( !total ) {
#ifdef DESQUID_MORE
            NAV::printMessage( "No pkmn", MSG_INFO );
#endif
            return false;
        }

#ifdef DESQUID_MORE
        NAV::printMessage( ( std::to_string( total ) ).c_str( ), MSG_INFO );
#endif
        u16 pkmnId      = 0;
        u16 backup      = 0;
        u8  pkmnForme   = 0;
        u8  backupForme = 0;

        u8 res = rand( ) % total;
        total  = 0;
        for( u8 i = 0; i < currentData( ).m_pokemonDescrCount; ++i ) {
            if( currentData( ).m_pokemon[ i ].m_encounterType == p_type ) {
                s8 ownedbadge = SAVE::SAV.getActiveFile( ).getBadgeCount( ) + availmod;
                if( ownedbadge < 0 ) { ownedbadge = 0; }

                if( ownedbadge >= currentData( ).m_pokemon[ i ].m_slot
                    && ( currentData( ).m_pokemon[ i ].m_daytime
                         & ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) ) {
                    total += currentData( ).m_pokemon[ i ].m_encounterRate;

                    // if the player hasn't obtained the nat dex yet, they should only see
                    // pkmn that are in the local dex
                    if( SAVE::SAV.getActiveFile( ).getPkmnDisplayDexId(
                            currentData( ).m_pokemon[ i ].m_speciesId )
                        != u16( -1 ) ) {
                        backup      = currentData( ).m_pokemon[ i ].m_speciesId;
                        backupForme = currentData( ).m_pokemon[ i ].m_forme;
                    } else {
                        continue;
                    }

                    if( total > res ) {
                        pkmnId    = currentData( ).m_pokemon[ i ].m_speciesId;
                        pkmnForme = currentData( ).m_pokemon[ i ].m_forme;
#ifdef DESQUID_MORE
                        NAV::printMessage(
                            ( std::to_string( pkmnId ) + " " + std::to_string( pkmnForme ) )
                                .c_str( ),
                            MSG_INFO );
#endif
                        break;
                    }
                }
            }
        }

        if( !pkmnId ) {
            pkmnId    = backup;
            pkmnForme = backupForme;
        }
        if( !pkmnId ) { return false; }

        if( pkmnId == PKMN_PIKACHU && !( rand( ) & PIKACHU_IS_MIMIKYU_MOD ) ) {
            pkmnId = PKMN_MIMIKYU;
        }

        ANIMATE_MAP = false;
        DRAW_TIME   = false;

        bool luckyenc = SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_WISHING_CHARM )
                            ? !( rand( ) & 127 )
                            : !( rand( ) & 2047 );
        bool charm    = SAVE::SAV.getActiveFile( ).m_bag.count(
               BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SHINY_CHARM );

        if( luckyenc ) {
            SOUND::playBGM( MOD_BATTLE_WILD_ALT );
        } else {
            SOUND::playBGM( SOUND::BGMforWildBattle( pkmnId ) );
        }
        _playerIsFast = false;
        _fastBike     = false;
        _mapSprites.setFrameD( _playerSprite, SAVE::SAV.getActiveFile( ).m_player.m_direction );

        IO::fadeScreen( IO::BATTLE );
        IO::BG_PAL( true )[ 0 ] = 0;
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );

        wildPkmn = pokemon( pkmnId, level, pkmnForme, 0, luckyenc ? 255 : ( charm ? 3 : 0 ),
                            luckyenc, false, 0, 0, luckyenc );

        u8 platform = 0, plat2 = 0;
        u8 battleBack = p_type == WATER ? currentData( ).m_surfBattleBG : currentData( ).m_battleBG;
        switch( p_type ) {
        case WATER:
            platform = currentData( ).m_surfBattlePlat1;
            plat2    = currentData( ).m_surfBattlePlat2;
            break;
        case OLD_ROD:
        case GOOD_ROD:
        case SUPER_ROD:
            platform = currentData( ).m_battlePlat1;
            plat2    = currentData( ).m_surfBattlePlat2;
            break;

        default:
            platform = currentData( ).m_battlePlat1;
            plat2    = currentData( ).m_battlePlat2;
            break;
        }

        auto playerPrio = _mapSprites.getPriority( _playerSprite );
        swiWaitForVBlank( );
        if( BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                            SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), wildPkmn, platform,
                            plat2, battleBack, getBattlePolicy( true ) )
                .start( )
            == BATTLE::battle::BATTLE_OPPONENT_WON ) {
            faintPlayer( );
            return true;
        }
        SOUND::restartBGM( );
        FADE_TOP_DARK( );
        draw( playerPrio );
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
        NAV::init( );

        ANIMATE_MAP = true;
        DRAW_TIME   = true;
        return true;
    }

    BATTLE::battlePolicy mapDrawer::getBattlePolicy( bool p_isWildBattle, BATTLE::battleMode p_mode,
                                                     bool p_distributeEXP ) {
        BATTLE::battlePolicy res = p_isWildBattle
                                       ? BATTLE::battlePolicy( BATTLE::DEFAULT_WILD_POLICY )
                                       : BATTLE::battlePolicy( BATTLE::DEFAULT_TRAINER_POLICY );

        res.m_mode               = p_mode;
        res.m_distributeEXP      = p_distributeEXP;
        res.m_allowMegaEvolution = SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_MEGA_EVOLUTION );

        res.m_weather = BATTLE::weather::NO_WEATHER;
        switch( getWeather( ) ) {
        case SUNNY: res.m_weather = BATTLE::weather::SUN; break;
        case RAINY:
        case THUNDERSTORM: res.m_weather = BATTLE::weather::RAIN; break;
        case SNOW:
        case BLIZZARD: res.m_weather = BATTLE::weather::HAIL; break;
        case SANDSTORM: res.m_weather = BATTLE::weather::SANDSTORM; break;
        case FOG: res.m_weather = BATTLE::weather::FOG; break;
        case HEAVY_SUNLIGHT: res.m_weather = BATTLE::weather::HEAVY_SUNSHINE; break;
        case HEAVY_RAIN: res.m_weather = BATTLE::weather::HEAVY_RAIN; break;
        default: break;
        }

        return res;
    }

    bool mapDrawer::requestWildPkmn( bool p_forceHighGrass ) {
        u8 moveData = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        u8 behave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                        .m_bottombehave;

        if( moveData == MVD_SURF && behave != BEH_WATERFALL ) {
            return handleWildPkmn( WATER, true );
        } else if( ( behave == BEH_GRASS_ASH || behave == BEH_SAND_WITH_ENCOUNTER_AND_FISH
                     || behave == BEH_GRASS )
                   && !p_forceHighGrass ) {
            return handleWildPkmn( GRASS, true );
        } else if( behave == BEH_LONG_GRASS || p_forceHighGrass ) {
            return handleWildPkmn( HIGH_GRASS, true );
        } else if( currentData( ).m_mapType & CAVE ) {
            return handleWildPkmn( GRASS, true );
        }
        return false;
    }

    void mapDrawer::resetTracerChain( ) {
        // TODO
    }

    void mapDrawer::startTracerChain( ) {
        // TODO
    }

    u8 mapDrawer::getTracerPkmn( u16 p_globX, u16 p_globY ) {
        return NO_TRACER_PKMN;
    }

    bool mapDrawer::updateTracerChain( direction p_dir ) {
        return false;
    }

    bool mapDrawer::continueTracerChain( ) {
        return false;
    }

    void mapDrawer::useTracer( position p_position ) {
        // TODO
    }

} // namespace MAP
