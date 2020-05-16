/*
Pokémon neo
------------------------------

file        new : item.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, new either version 3 of the License, new or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, new see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <vector>

#include "berry.h"
#include "item.h"
#include "itemNames.h"
#include "move.h"
#include "moveNames.h"
#include "pokemon.h"
#include "script.h"
#include "pokemonNames.h"

#include "mapDefines.h"
#include "mapDrawer.h"
#include "messageBox.h"
#include "saveGame.h"
#include "uio.h"

namespace ITEM {
    u16 getItemChar( const u8 p_itemType ) {
        if( p_itemType & ITEMTYPE_BERRY ) {
            return 473;
        }

        auto itemType = ( p_itemType & 15 );

        switch( itemType ) {
            case ITEMTYPE_POKEBALL:
                return 474;
            case ITEMTYPE_BATTLEITEM:
                return 475;
            case ITEMTYPE_COLLECTIBLE:
            case ITEMTYPE_USABLE:
            case ITEMTYPE_EVOLUTION:
                return 468;
            case ITEMTYPE_MEDICINE:
                return 472;
            case ITEMTYPE_FORMECHANGE:
            case ITEMTYPE_KEYITEM:
                return 469;
            case ITEMTYPE_TM:
                return 470;
            default:
                return ' ';
        }
    }

    bool use( const u16 p_itemId, const itemData& p_data, pokemon& p_pokemon,
              std::function<u8( u8 )> p_callback, bool p_inbattle ) {
        if( p_pokemon.isEgg( ) || !p_pokemon.m_boxdata.m_speciesId ) { return false; }

        itemData item = p_data;

        if( ( item.m_itemType & 15 ) != ITEMTYPE_MEDICINE
                && item.m_itemType != ITEMTYPE_FORMECHANGE ) {
            return false;
        }

        if( item.m_itemType == ITEMTYPE_FORMECHANGE ) {
            if( p_inbattle ) return false;

            if( p_itemId == I_REVEAL_GLASS ) {
                if( p_pokemon.m_boxdata.m_speciesId == PKMN_TORNADUS
                    || p_pokemon.m_boxdata.m_speciesId == PKMN_THUNDURUS
                    || p_pokemon.m_boxdata.m_speciesId == PKMN_LANDORUS ) {
                    p_pokemon.setForme( 1 - p_pokemon.getForme( ) );
                    return true;
                }
                return false;
            }
            if( p_itemId == I_PRISON_BOTTLE ) {
                if( p_pokemon.m_boxdata.m_speciesId == PKMN_HOOPA
                    && !p_pokemon.getForme( ) ) {
                    p_pokemon.setBattleForme( 1 );
                    return true;
                }
                return false;
            }
            if( p_itemId == I_GRACIDEA ) {
                if( p_pokemon.m_boxdata.m_speciesId == PKMN_SHAYMIN
                    && !p_pokemon.getForme( ) ) {
                    if( p_pokemon.m_status.m_isFrozen )
                        return false;
                    p_pokemon.setBattleForme( 1 );
                    return true;
                }
                return false;
            }

            if( p_pokemon.m_boxdata.m_speciesId != item.m_param1 ||
                p_pokemon.getForme( ) == item.m_param2 ) {
                return false;
            }
            p_pokemon.setForme( item.m_param2 );
            return true;
        }

        if( p_inbattle && item.m_effect >= 7 ) { return false; }

        bool change = false;

        if( item.m_effect == 3 ) {
            if( p_pokemon.getTasteStr( ) == 4 ) {
                change |= p_pokemon.m_boxdata.m_steps < 255;
                p_pokemon.m_boxdata.m_steps += change;
            } else {
                change |= !!p_pokemon.m_boxdata.m_steps;
                p_pokemon.m_boxdata.m_steps -= std::min( p_pokemon.m_boxdata.m_steps, (u8) 10 );
            }
            item.m_effect = 1;
        }
        if ( item.m_effect == 5 ) {
            if( p_pokemon.getTasteStr( ) == 4 ) {
                change |= p_pokemon.m_boxdata.m_steps < 255;
                p_pokemon.m_boxdata.m_steps += change;
            } else {
                change |= !!p_pokemon.m_boxdata.m_steps;
                p_pokemon.m_boxdata.m_steps -= std::min( p_pokemon.m_boxdata.m_steps, (u8) 5 );
            }
            item.m_effect = 4;
        }

        switch( item.m_effect ) {
            case 1: { // heal hp
                if( ( !p_pokemon.m_stats.m_acHP ) == item.m_param3 &&
                        p_pokemon.m_stats.m_acHP < p_pokemon.m_stats.m_maxHP ) {
                    p_pokemon.m_stats.m_acHP = std::min( p_pokemon.m_stats.m_maxHP,
                            (u16)( p_pokemon.m_stats.m_acHP + item.m_param1 ) );
                    p_pokemon.m_stats.m_acHP = (u16) std::min( (u32) p_pokemon.m_stats.m_maxHP,
                            p_pokemon.m_stats.m_acHP +
                            ( item.m_param2 * (u32) p_pokemon.m_stats.m_maxHP / 100 ) );
                    change = true;
                }
                return change;
            }
            case 2: { // heal hp + status
                if( ( !p_pokemon.m_stats.m_acHP ) == item.m_param3 &&
                        p_pokemon.m_stats.m_acHP < p_pokemon.m_stats.m_maxHP ) {
                    p_pokemon.m_stats.m_acHP = std::min( p_pokemon.m_stats.m_maxHP,
                            (u16)( p_pokemon.m_stats.m_acHP + item.m_param1 ) );
                    p_pokemon.m_stats.m_acHP = std::min( p_pokemon.m_stats.m_maxHP,
                            (u16)( p_pokemon.m_stats.m_acHP +
                            ( item.m_param2 * p_pokemon.m_stats.m_maxHP / 100 ) ) );
                    change = true;
                }
                if( p_pokemon.m_statusint ) {
                    p_pokemon.m_statusint = 0;
                    change = true;
                }
                return change;
            }
            case 4: { // heal status
                switch( item.m_param1 ) {
                    case 0: { // all status
                        change |= !!p_pokemon.m_statusint;
                        p_pokemon.m_statusint = 0;
                        break;
                    }
                    case 1: { // poison
                        change |= p_pokemon.m_status.m_isPoisoned
                            || p_pokemon.m_status.m_isBadlyPoisoned;
                        p_pokemon.m_status.m_isPoisoned = p_pokemon.m_status.m_isBadlyPoisoned
                            = false;
                        break;
                    }
                    case 2: { // burn
                        change |= p_pokemon.m_status.m_isBurned;
                        p_pokemon.m_status.m_isBurned = false;
                        break;
                    }
                    case 3: { // ice
                        change |= p_pokemon.m_status.m_isFrozen;
                        p_pokemon.m_status.m_isFrozen = false;
                        break;
                    }
                    case 4: { // sleep
                        change |= !!p_pokemon.m_status.m_isAsleep;
                        p_pokemon.m_status.m_isAsleep = 0;
                        break;
                    }
                    case 5: { // paralyze
                        change |= p_pokemon.m_status.m_isParalyzed;
                        p_pokemon.m_status.m_isParalyzed = false;
                        break;
                    }
                    default: {
                        break;
                    }
                }
                return change;
            }
            case 6: { // heal pp
                u8 mv = 0;
                if( item.m_param3 == 4 ) {
                    mv = 15;
                } else {
                    mv = p_callback( item.m_param3 );
                }
                for( u8 i = 0; i < 4; ++i ) {
                    MOVE::moveData mdata = MOVE::getMoveData( p_pokemon.m_boxdata.m_moves[ i ] );

                    if( ( mv & ( 1 << i ) ) && p_pokemon.m_boxdata.m_moves[ i ] ) {
                        u8& tmp = p_pokemon.m_boxdata.m_acPP[ i ];
                        u8 mx  = s8( mdata.m_pp * ( 5 + p_pokemon.PPupget( i ) ) / 5 );
                        change |= tmp < mx;
                        tmp = std::min( mx, (u8)( tmp + item.m_param1 + item.m_param2
                                    * mx / 100 ) );
                    }
                }
                return change;
            }
            case 7: { // sacred ash
                return p_callback( 255 );
            }
            case 8: { // ev up
                if( p_pokemon.m_boxdata.m_effortValues[ item.m_param1 ] >= item.m_param3 ) {
                    return false;
                }
                p_pokemon.m_boxdata.m_effortValues[ item.m_param1 ] = std::min( (u8) item.m_param3,
                        (u8)( p_pokemon.m_boxdata.m_effortValues[ item.m_param1 ]
                            + item.m_param2 ) );
                p_pokemon.recalculateStats( );
                return true;
            }
            case 9: { // level up
                if( p_pokemon.m_level >= item.m_param2 ) {
                    return false;
                }
                return p_pokemon.setLevel( p_pokemon.m_level + item.m_param1 );
            }
            case 10: { // pp up
                u8 mv = 0;
                if( item.m_param3 == 4 ) {
                    mv = 15;
                } else {
                    mv = p_callback( item.m_param3 );
                }
                for( u8 i = 0; i < 4; ++i ) {
                    if( ( mv & ( 1 << i ) ) && p_pokemon.m_boxdata.m_moves[ i ] ) {
                        MOVE::moveData mdata
                            = MOVE::getMoveData( p_pokemon.m_boxdata.m_moves[ i ] );
                        u8 ppup = p_pokemon.PPupget( i );
                        if( ppup >= item.m_param2 || ( mdata.m_flags & MOVE::NOPPBOOST ) ) {
                            continue;
                        }
                        u8& tmp = p_pokemon.m_boxdata.m_acPP[ i ];
                        u8 oldmx = s8( mdata.m_pp * ( 5 + p_pokemon.PPupget( i ) ) / 5 );

                        p_pokemon.PPupset( i, std::min( (u8) item.m_param2, (u8)( ppup
                                        + item.m_param1 ) ) );
                        u8 mx = s8( mdata.m_pp * ( 5 + p_pokemon.PPupget( i ) ) / 5 );

                        tmp = std::min( mx, (u8)( mx - oldmx + tmp ) );
                        change = true;
                    }
                }
                return change;
            }
            case 11: { // ev down berry
                if( !p_pokemon.m_boxdata.m_effortValues[ item.m_param1 ]
                        && p_pokemon.m_boxdata.m_steps == 0xff ) {
                    return false;
                }
                p_pokemon.m_boxdata.m_effortValues[ item.m_param1 ] = std::max( 0, std::min( 100,
                        p_pokemon.m_boxdata.m_effortValues[ item.m_param1 ] - 10 ) );
                p_pokemon.recalculateStats( );
                if( p_pokemon.m_boxdata.m_steps >= 245 ) {
                    p_pokemon.m_boxdata.m_steps = 255;
                } else {
                    p_pokemon.m_boxdata.m_steps += 10;
                }
                return true;
            }
            case 12: { // ability capsule
                return p_pokemon.swapAbilities( );
            }
            case 13: { // iv set
                if( ( item.m_param2 && p_pokemon.IVget( item.m_param1 ) == 31 )
                    || ( !item.m_param2 && p_pokemon.IVget( item.m_param1 ) == item.m_param3 ) ) {
                    return false;
                }
                if( item.m_param2 ) {
                    p_pokemon.IVset( item.m_param1, std::min( 31,
                                p_pokemon.IVget( item.m_param1 ) + item.m_param2 ) );
                } else {
                    p_pokemon.IVset( item.m_param1, item.m_param3 );
                }
                return true;
            }
            case 14: { // exp up
                return p_pokemon.gainExperience( item.m_param1 );
            }
            case 15: { // nature mint
                return p_pokemon.setNature( (pkmnNatures) item.m_param1 );
            }
            default:
                break;
        }
        return change;
    }

    // Returns false if the original UI has not to be redrawn/will be exited
    bool use( const u16 p_itemId, bool p_dryRun ) {
        char buffer[ 50 ];
        if( !p_dryRun ) {
            bool ex = false;
            for( u8 i = 0; i < 5; ++i )
                if( SAVE::SAV->getActiveFile( ).m_lstUsedItems[ i ] == p_itemId ) {
                    ex = true;
                    break;
                }
            if( !ex ) {
                SAVE::SAV->getActiveFile( )
                    .m_lstUsedItems[ SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx ]
                    = p_itemId;
                SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx
                    = ( SAVE::SAV->getActiveFile( ).m_lstUsedItemsIdx + 1 ) % 5;
            }
        }
        switch( p_itemId ) {
            case I_REPEL:
                if( !p_dryRun ) {
                    SAVE::SAV->getActiveFile( ).m_repelSteps
                        = std::max( SAVE::SAV->getActiveFile( ).m_repelSteps, (s16) 50 );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                    IO::messageBox( GET_STRING( 63 ), false );
                }
                return true;
            case I_SUPER_REPEL:
                if( !p_dryRun ) {
                    SAVE::SAV->getActiveFile( ).m_repelSteps
                        = std::max( SAVE::SAV->getActiveFile( ).m_repelSteps, (s16) 100 );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                    IO::messageBox( GET_STRING( 64 ), false );
                }
                return true;
            case I_MAX_REPEL:
                if( !p_dryRun ) {
                    SAVE::SAV->getActiveFile( ).m_repelSteps
                        = std::max( SAVE::SAV->getActiveFile( ).m_repelSteps, (s16) 250 );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                    IO::messageBox( GET_STRING( 65 ), false );
                }
                return true;
            case I_EXP_SHARE:
                if( !p_dryRun ) {
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                    if( SAVE::SAV->getActiveFile( ).m_options.m_EXPShareEnabled )
                        IO::messageBox( GET_STRING( 66 ), false );
                    else
                        IO::messageBox( GET_STRING( 67 ), false );
                    SAVE::SAV->getActiveFile( ).m_options.m_EXPShareEnabled
                        = !SAVE::SAV->getActiveFile( ).m_options.m_EXPShareEnabled;
                }
                return true;
            case I_COIN_CASE:
                if( !p_dryRun ) {
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                    snprintf( buffer, 50, GET_STRING( 68 ), SAVE::SAV->getActiveFile( ).m_coins );
                    IO::messageBox( buffer, false );
                }
                return true;
            case I_POINT_CARD:
                if( !p_dryRun ) {
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                    snprintf( buffer, 50, GET_STRING( 69 ), SAVE::SAV->getActiveFile( ).m_battlePoints );
                    IO::messageBox( buffer, false );
                }
                return true;
            case I_ESCAPE_ROPE:
                if( !p_dryRun ) MOVE::use( M_DIG, 0 );
                return false;
            case I_HONEY:
                if( !p_dryRun ) MOVE::use( M_SWEET_SCENT, 0 );
                return false;
            case I_BIKE2:
            case I_BIKE:
            case I_MACH_BIKE:
            case I_ACRO_BIKE:
                if( SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::WALK ) {
                    if( !p_dryRun ) MAP::curMap->changeMoveMode( MAP::BIKE );
                    return false;
                } else if( SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::BIKE ) {
                    if( !p_dryRun ) MAP::curMap->changeMoveMode( MAP::WALK );
                    return false;
                } else
                    return true;
            case I_OLD_ROD:
                if( MAP::curMap->canFish( SAVE::SAV->getActiveFile( ).m_player.m_pos,
                            SAVE::SAV->getActiveFile( ).m_player.m_direction ) ) {
                    if( !p_dryRun )
                        MAP::curMap->fishPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction, 0 );
                    return false;
                } else
                    return true;
            case I_GOOD_ROD:
                if( MAP::curMap->canFish( SAVE::SAV->getActiveFile( ).m_player.m_pos,
                            SAVE::SAV->getActiveFile( ).m_player.m_direction ) ) {
                    if( !p_dryRun )
                        MAP::curMap->fishPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction, 1 );
                    return false;
                } else
                    return true;
            case I_SUPER_ROD:
                if( MAP::curMap->canFish( SAVE::SAV->getActiveFile( ).m_player.m_pos,
                            SAVE::SAV->getActiveFile( ).m_player.m_direction ) ) {
                    if( !p_dryRun )
                        MAP::curMap->fishPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction, 2 );
                    return false;
                } else
                    return true;
            default:
                break;
        }
        return false;
    }

    bool isUsable( const u16 p_itemId ) {
        switch( p_itemId ) {
            case I_REPEL:
            case I_SUPER_REPEL:
            case I_MAX_REPEL:
            case I_EXP_SHARE:
            case I_COIN_CASE:
            case I_POINT_CARD:
                return true;
            case I_ESCAPE_ROPE:
                return MOVE::possible( M_DIG, 0 );
            case I_HONEY:
                return MOVE::possible( M_SWEET_SCENT, 0 );
            case I_BIKE2:
            case I_BIKE:
            case I_MACH_BIKE:
            case I_ACRO_BIKE:
                return SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::WALK
                    || SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::BIKE;
            case I_OLD_ROD:
            case I_GOOD_ROD:
            case I_SUPER_ROD:
                return MAP::curMap->canFish( SAVE::SAV->getActiveFile( ).m_player.m_pos,
                        SAVE::SAV->getActiveFile( ).m_player.m_direction );
            case I_POKE_RADAR:
                // TODO
            case I_VS_SEEKER:
                // TODO
            case I_VS_RECORDER:
                // TODO
            case I_APRICORN_BOX:
                // TODO
            case I_DOWSING_MACHINE:
                // TODO
            case I_GB_SOUNDS:
                // TODO
            case I_DNA_SPLICERS:
            case I_DNA_SPLICERS2:
                // TODO
            case I_SOOT_SACK:
                // TODO
            case I_ZYGARDE_CUBE:
                // TODO
            case I_N_SOLARIZER:
                // TODO
            case I_N_LUNARIZER:
                // TODO
            case I_N_SOLARIZER2:
                // TODO
            case I_N_LUNARIZER2:
                // TODO
            case I_SPRAYDUCK: case I_WAILMER_PAIL:
                // TODO
            default:
                break;
        }
        return false;
    }
}
