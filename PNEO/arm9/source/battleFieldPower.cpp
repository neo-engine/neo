/*
Pokémon neo
------------------------------

file        : battleFieldPower.cpp
author      : Philip Wellnitz
description : move power, type and effectiveness computation

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

#include "battle/ability.h"
#include "battle/battle.h"
#include "battle/battleField.h"
#include "battle/battleSide.h"
#include "battle/battleSlot.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "battle/type.h"
#include "defines.h"
#include "gen/abilityNames.h"
#include "gen/itemNames.h"
#include "gen/pokemonNames.h"
#include "save/saveGame.h"

namespace BATTLE {
    static type heldItemPowerBoost( u16 p_item ) {
        switch( p_item ) {
        case I_SILK_SCARF: return TYPE_NORMAL;
        case I_BLACK_BELT: return TYPE_FIGHTING;
        case I_SHARP_BEAK: return TYPE_FLYING;
        case I_POISON_BARB: return TYPE_POISON;
        case I_SOFT_SAND: return TYPE_GROUND;
        case I_HARD_STONE: return TYPE_ROCK;
        case I_SILVER_POWDER: return TYPE_BUG;
        case I_SPELL_TAG: return TYPE_GHOST;
        case I_DRAGON_FANG: return TYPE_DRAGON;
        case I_BLACK_GLASSES: return TYPE_DARKNESS;
        case I_FAIRY_FEATHER: return TYPE_FAIRY;
        case I_CHARCOAL: return TYPE_FIRE;
        case I_MYSTIC_WATER: return TYPE_WATER;
        case I_MAGNET: return TYPE_LIGHTNING;
        case I_MIRACLE_SEED: return TYPE_GRASS;
        case I_NEVER_MELT_ICE: return TYPE_ICE;
        case I_TWISTED_SPOON: return TYPE_PSYCHIC;
        case I_METAL_COAT: return TYPE_STEEL;
        case I_FLAME_PLATE: return TYPE_FIRE;
        case I_SPLASH_PLATE: return TYPE_WATER;
        case I_ZAP_PLATE: return TYPE_LIGHTNING;
        case I_MEADOW_PLATE: return TYPE_GRASS;
        case I_ICICLE_PLATE: return TYPE_ICE;
        case I_FIST_PLATE: return TYPE_FIGHTING;
        case I_TOXIC_PLATE: return TYPE_POISON;
        case I_EARTH_PLATE: return TYPE_GROUND;
        case I_SKY_PLATE: return TYPE_FLYING;
        case I_MIND_PLATE: return TYPE_PSYCHIC;
        case I_INSECT_PLATE: return TYPE_BUG;
        case I_STONE_PLATE: return TYPE_ROCK;
        case I_SPOOKY_PLATE: return TYPE_GHOST;
        case I_DRACO_PLATE: return TYPE_DRAGON;
        case I_DREAD_PLATE: return TYPE_DARKNESS;
        case I_IRON_PLATE: return TYPE_STEEL;
        case I_PIXIE_PLATE: return TYPE_FAIRY;
        case I_SEA_INCENSE: return TYPE_WATER;
        case I_WAVE_INCENSE: return TYPE_WATER;
        case I_ROSE_INCENSE: return TYPE_GRASS;
        case I_ROCK_INCENSE: return TYPE_ROCK;
        case I_ODD_INCENSE: return TYPE_PSYCHIC;
        default: return TYPE_UNKNOWN;
        }
    }

    u16 field::getMovePower( battleMove p_move, fieldPosition p_defender ) {
        u16  basepower = p_move.m_moveData.m_basePower;
        type moveType  = p_move.m_moveData.m_type;

        if( p_move.m_param == M_HIDDEN_POWER ) {
            basepower = getPkmn( p_move.m_user.first, p_move.m_user.second )->getHPPower( );
            moveType  = getPkmn( p_move.m_user.first, p_move.m_user.second )->getHPType( );
        }

        {
            auto pw = getPseudoWeather( );
            if( ( pw & PW_WATERSPORT && moveType == TYPE_FIRE )
                || ( pw & PW_MUDSPORT && moveType == TYPE_LIGHTNING ) ) [[unlikely]] {
                basepower = u16( ( u32( basepower ) * 1352u ) / 4096u );
            }
        }

        if( !suppressesAbilities( )
            && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second ) ) {
            switch( getPkmn( p_move.m_user.first, p_move.m_user.second )->getAbility( ) ) {
            case A_PIXILATE:
            case A_AERILATE:
            case A_REFRIGERATE:
            case A_GALVANIZE:
                if( moveType == TYPE_NORMAL ) { basepower = ( basepower * 120 ) / 100; }
                break;
            [[likely]] default:
                break;
            }
        }

        if( p_move.m_param == M_WEATHER_BALL && !suppressesWeather( ) ) {
            switch( getWeather( ) ) {
            case WE_NONE: basepower = 50; break;
            default: basepower = 100; break;
            }
        }

        if( p_move.m_param == M_TERRAIN_PULSE ) {
            switch( getTerrain( ) ) {
            case TR_NONE: basepower = 50; break;
            default: basepower = 100; break;
            }
        }

        if( p_move.m_param == M_FACADE ) {
            if( getPkmn( p_move.m_user.first, p_move.m_user.second )->m_statusint ) {
                basepower *= 2;
            }
        }

        auto terrainAttacker = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( terrainAttacker != nullptr ) {
            bool atkGrounded = isGrounded( p_move.m_user.first, p_move.m_user.second );
            bool defGrounded = getPkmn( p_defender.first, p_defender.second ) != nullptr
                               && isGrounded( p_defender.first, p_defender.second );
            bool atkAir      = isSemiInvulnerable( p_move.m_user.first, p_move.m_user.second );
            bool defAir      = getPkmn( p_defender.first, p_defender.second ) != nullptr
                               && isSemiInvulnerable( p_defender.first, p_defender.second );
            switch( getTerrain( ) ) {
            case TR_ELECTRICTERRAIN:
                if( moveType == TYPE_LIGHTNING && atkGrounded && !atkAir ) {
                    basepower = u16( ( u32( basepower ) * 5325u ) / 4096u );
                }
                break;
            case TR_GRASSYTERRAIN:
                if( ( p_move.m_param == M_EARTHQUAKE || p_move.m_param == M_BULLDOZE
                      || p_move.m_param == M_MAGNITUDE )
                    && defGrounded && !defAir ) {
                    basepower = u16( basepower / 2 );
                } else if( moveType == TYPE_GRASS && atkGrounded ) {
                    basepower = u16( ( u32( basepower ) * 5325u ) / 4096u );
                }
                break;
            case TR_MISTYTERRAIN:
                if( moveType == TYPE_DRAGON && defGrounded && !defAir ) {
                    basepower = u16( basepower / 2 );
                }
                break;
            case TR_PSYCHICTERRAIN:
                if( moveType == TYPE_PSYCHIC && atkGrounded && !atkAir ) {
                    basepower = u16( ( u32( basepower ) * 5325u ) / 4096u );
                }
                break;
            [[likely]] default:
                break;
            }
        }

        auto booster = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( booster != nullptr && canUseItem( p_move.m_user.first, p_move.m_user.second ) ) {
            type itemBoost = heldItemPowerBoost( booster->getItem( ) );
            if( itemBoost != TYPE_UNKNOWN && moveType == itemBoost ) {
                basepower = u16( ( u32( basepower ) * 4915u ) / 4096u );
            }
        }

        if( booster != nullptr && !suppressesAbilities( )
            && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second ) ) {
            auto uflags = p_move.m_moveData.m_flags;
            switch( booster->getAbility( ) ) {
            case A_STRONG_JAW:
                if( uflags & MF_BITE ) { basepower = u16( ( u32( basepower ) * 3u ) / 2u ); }
                break;
            case A_MEGA_LAUNCHER:
                if( uflags & MF_PULSE ) { basepower = u16( ( u32( basepower ) * 3u ) / 2u ); }
                break;
            case A_SHARPNESS:
                if( uflags & MF_SLICE ) { basepower = u16( ( u32( basepower ) * 3u ) / 2u ); }
                break;
            case A_SHEER_FORCE:
                if( moveHasSecondaries( p_move ) || hasSheerForceBoostFlag( p_move.m_param ) ) {
                    basepower = u16( ( u32( basepower ) * 5325u ) / 4096u );
                }
                break;
            case A_TOUGH_CLAWS:
                if( uflags & MF_CONTACT ) {
                    basepower = u16( ( u32( basepower ) * 5325u ) / 4096u );
                }
                break;
            case A_IRON_FIST:
                if( uflags & MF_PUNCH ) { basepower = u16( ( u32( basepower ) * 4915u ) / 4096u ); }
                break;
            [[likely]] default:
                break;
            }
        }
        if( booster != nullptr && canUseItem( p_move.m_user.first, p_move.m_user.second )
            && booster->getItem( ) == I_PUNCHING_GLOVE
            && ( p_move.m_moveData.m_flags & MF_PUNCH ) ) {
            basepower = u16( ( u32( basepower ) * 4506u ) / 4096u );
        }

        return basepower;
    }

    type field::getMoveType( battleMove p_move ) {
        type moveType = p_move.m_moveData.m_type;
        auto pkmn     = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( pkmn == nullptr ) [[unlikely]] { return TYPE_UNKNOWN; }

        if( p_move.m_param == M_STRUGGLE ) { return TYPE_NORMAL; }

        if( p_move.m_param == M_HIDDEN_POWER ) [[unlikely]] { moveType = pkmn->getHPType( ); }

        if( p_move.m_param == M_AURA_WHEEL ) [[unlikely]] {
            if( pkmn->getForme( ) == 1 ) {
                moveType = TYPE_DARKNESS;
            } else {
                moveType = TYPE_LIGHTNING;
            }
        }

        if( !suppressesAbilities( )
            && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second ) ) {
            switch( pkmn->getAbility( ) ) {
            case A_NORMALIZE: moveType = TYPE_NORMAL; break;
            case A_PIXILATE:
                if( moveType == TYPE_NORMAL ) { return TYPE_FAIRY; }
                break;
            case A_AERILATE:
                if( moveType == TYPE_NORMAL ) { return TYPE_FLYING; }
                break;
            case A_GALVANIZE:
                if( moveType == TYPE_NORMAL ) { return TYPE_LIGHTNING; }
                break;
            case A_REFRIGERATE:
                if( moveType == TYPE_NORMAL ) { return TYPE_ICE; }
                break;
            case A_LIQUID_VOICE:
                if( p_move.m_moveData.m_flags & MF_SOUND ) { return TYPE_WATER; }
                break;
            [[likely]] default:
                break;
            }
        }

        if( p_move.m_param == M_JUDGMENT
            && canUseItem( p_move.m_user.first, p_move.m_user.second ) ) [[unlikely]] {
            switch( pkmn->getItem( ) ) {
            case I_FLAME_PLATE: moveType = TYPE_FIRE; break;
            case I_SPLASH_PLATE: moveType = TYPE_WATER; break;
            case I_ZAP_PLATE: moveType = TYPE_LIGHTNING; break;
            case I_MEADOW_PLATE: moveType = TYPE_GRASS; break;
            case I_ICICLE_PLATE: moveType = TYPE_ICE; break;
            case I_FIST_PLATE: moveType = TYPE_FIGHTING; break;
            case I_TOXIC_PLATE: moveType = TYPE_POISON; break;
            case I_EARTH_PLATE: moveType = TYPE_GROUND; break;
            case I_SKY_PLATE: moveType = TYPE_FLYING; break;
            case I_MIND_PLATE: moveType = TYPE_PSYCHIC; break;
            case I_INSECT_PLATE: moveType = TYPE_BUG; break;
            case I_STONE_PLATE: moveType = TYPE_ROCK; break;
            case I_SPOOKY_PLATE: moveType = TYPE_GHOST; break;
            case I_DRACO_PLATE: moveType = TYPE_DRAGON; break;
            case I_DREAD_PLATE: moveType = TYPE_DARKNESS; break;
            case I_IRON_PLATE: moveType = TYPE_STEEL; break;
            case I_PIXIE_PLATE: moveType = TYPE_FAIRY; break;
            case I_NULL_PLATE: moveType = TYPE_UNKNOWN; break;
            default: moveType = TYPE_NORMAL; break;
            }
        }

        if( p_move.m_param == M_MULTI_ATTACK
            && canUseItem( p_move.m_user.first, p_move.m_user.second ) ) [[unlikely]] {
            switch( pkmn->getItem( ) ) {
            case I_FIGHTING_MEMORY: moveType = TYPE_FIGHTING; break;
            case I_FLYING_MEMORY: moveType = TYPE_FLYING; break;
            case I_POISON_MEMORY: moveType = TYPE_POISON; break;
            case I_GROUND_MEMORY: moveType = TYPE_GROUND; break;
            case I_ROCK_MEMORY: moveType = TYPE_ROCK; break;
            case I_BUG_MEMORY: moveType = TYPE_BUG; break;
            case I_GHOST_MEMORY: moveType = TYPE_GHOST; break;
            case I_STEEL_MEMORY: moveType = TYPE_STEEL; break;
            case I_FIRE_MEMORY: moveType = TYPE_FIRE; break;
            case I_WATER_MEMORY: moveType = TYPE_WATER; break;
            case I_GRASS_MEMORY: moveType = TYPE_GRASS; break;
            case I_ELECTRIC_MEMORY: moveType = TYPE_LIGHTNING; break;
            case I_PSYCHIC_MEMORY: moveType = TYPE_PSYCHIC; break;
            case I_ICE_MEMORY: moveType = TYPE_ICE; break;
            case I_DRAGON_MEMORY: moveType = TYPE_DRAGON; break;
            case I_DARK_MEMORY: moveType = TYPE_DARKNESS; break;
            case I_FAIRY_MEMORY: moveType = TYPE_FAIRY; break;
            default: moveType = TYPE_NORMAL; break;
            }
        }

        if( p_move.m_param == M_TECHNO_BLAST
            && canUseItem( p_move.m_user.first, p_move.m_user.second ) ) [[unlikely]] {
            switch( pkmn->getItem( ) ) {
            case I_BURN_DRIVE: moveType = TYPE_FIRE; break;
            case I_DOUSE_DRIVE: moveType = TYPE_WATER; break;
            case I_CHILL_DRIVE: moveType = TYPE_ICE; break;
            case I_SHOCK_DRIVE: moveType = TYPE_LIGHTNING; break;
            default: moveType = TYPE_NORMAL; break;
            }
        }

        if( p_move.m_param == M_REVELATION_DANCE ) {
            auto types = getTypes( p_move.m_user.first, p_move.m_user.second );
            if( types.size( ) >= 1 ) {
                moveType = types[ 0 ];
            } else {
                moveType = TYPE_NORMAL;
            }
        }

        if( p_move.m_param == M_WEATHER_BALL && !suppressesWeather( ) ) {
            switch( getWeather( ) ) {
            case WE_RAIN:
            case WE_HEAVY_RAIN: moveType = TYPE_WATER; break;
            case WE_SUN:
            case WE_HEAVY_SUNSHINE: moveType = TYPE_FIRE; break;
            case WE_HAIL: moveType = TYPE_ICE; break;
            case WE_SANDSTORM: moveType = TYPE_ROCK; break;
            case WE_FOG: moveType = TYPE_FLYING; break;
            default: moveType = TYPE_NORMAL; break;
            }
        }

        if( p_move.m_param == M_TERRAIN_PULSE ) {
            switch( getTerrain( ) ) {
            case TR_PSYCHICTERRAIN: moveType = TYPE_PSYCHIC; break;
            case TR_ELECTRICTERRAIN: moveType = TYPE_LIGHTNING; break;
            case TR_MISTYTERRAIN: moveType = TYPE_FAIRY; break;
            case TR_GRASSYTERRAIN: moveType = TYPE_GRASS; break;
            default: moveType = TYPE_NORMAL; break;
            }
        }

        if( getVolatileStatus( p_move.m_user.first, p_move.m_user.second ) & VS_ELECTRIFY ) {
            moveType = TYPE_LIGHTNING;
        }

        if( ( getPseudoWeather( ) & PW_IONDELUGE ) && moveType == TYPE_NORMAL ) {
            moveType = TYPE_LIGHTNING;
        }

        return moveType;
    }

    u16 field::getEffectiveness( battleMove p_move, fieldPosition p_target ) {
        u16 res = 100;

        auto target = getPkmn( p_target.first, p_target.second );
        if( target == nullptr ) [[unlikely]] { return 0; }

        bool items = canUseItem( p_target.first, p_target.second );

        bool abilities = !suppressesAbilities( )
                         && !slotAbilitiesNullified( p_target.first, p_target.second )
                         && !( p_move.m_moveData.m_flags & MF_IGNOREABILITY );

        bool wonderguard = abilities && target->getAbility( ) == A_WONDER_GUARD;

        type moveType = getMoveType( p_move );

        // Heavy weather

        if( getWeather( ) == WE_HEAVY_SUNSHINE && moveType == TYPE_WATER
            && !suppressesWeather( ) ) {
            return 0;
        }
        if( getWeather( ) == WE_HEAVY_RAIN && moveType == TYPE_FIRE && !suppressesWeather( ) ) {
            return 0;
        }

        if( ( p_move.m_moveData.m_flags & MF_SOUND )
            && ( target->getAbility( ) == A_SOUNDPROOF || target->getAbility( ) == A_CACOPHONY )
            && abilities ) [[unlikely]] {
            return 0;
        }

        if( abilities && ( p_move.m_moveData.m_flags & MF_BULLET )
            && target->getAbility( ) == A_BULLETPROOF ) [[unlikely]] {
            return 0;
        }

        // Thousand arrows
        if( p_move.m_param == M_THOUSAND_ARROWS
            || ( p_move.m_moveData.m_flags & MF_IGNOREIMMUNITYGROUND ) ) [[unlikely]] {
            if( !isGrounded( p_target.first, p_target.second )
                && hasType( p_target.first, p_target.second, TYPE_FLYING ) ) {
                return res;
            }
        }

        bool targetIsGrounded
            = isGrounded( p_target.first, p_target.second, p_move.m_param != M_SUNSTEEL_STRIKE );
        auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
        if( user == nullptr ) [[unlikely]] { return 0; }

        auto       targetTypes = getTypes( p_target.first, p_target.second );
        const bool tarshot
            = ( getVolatileStatus( p_target.first, p_target.second ) & VS_TARSHOT ) != 0;
        const type firstType = targetTypes.empty( ) ? TYPE_NORMAL : targetTypes[ 0 ];

        for( type t : targetTypes ) {
            u16 curval = getTypeEffectiveness( moveType, t );

            if( !targetIsGrounded && moveType == TYPE_GROUND ) [[unlikely]] {
                curval = 0;
            } else if( targetIsGrounded && moveType == TYPE_GROUND && t == TYPE_FLYING ) {
                curval = 100;
            }

            if( p_move.m_param == M_FLYING_PRESS ) {
                curval = ( curval * getTypeEffectiveness( TYPE_FLYING, t ) ) / 100;
            }

            if( !curval && ( p_move.m_moveData.m_flags & MF_IGNOREIMMUNITY ) ) { curval = 100; }

            if( t == TYPE_GHOST && ( moveType == TYPE_NORMAL || moveType == TYPE_FIGHTING ) )
                [[unlikely]] {
                if( abilities && user->getAbility( ) == A_SCRAPPY ) { continue; }
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_FORESIGHT )
                    [[unlikely]] {
                    continue;
                }
            }

            if( t == TYPE_DARKNESS && moveType == TYPE_PSYCHIC
                && ( getVolatileStatus( p_target.first, p_target.second ) & VS_MIRACLEEYE ) )
                [[unlikely]] {
                continue;
            }

            if( t == TYPE_WATER && p_move.m_param == M_FREEZE_DRY ) { curval = 200; }

            if( t == TYPE_FLYING ) {
                if( !suppressesWeather( ) && _weather == WE_HEAVY_WINDS ) [[unlikely]] { continue; }
            }

            if( !curval && items && target->getItem( ) == I_RING_TARGET ) [[unlikely]] { continue; }

            if( tarshot && moveType == TYPE_FIRE && t == firstType && curval ) [[unlikely]] {
                curval = u16( curval * 2 );
            }

            res = ( res * curval / 100 );
        }

        if( wonderguard && res <= 100 ) { return 0; }

        return res;
    }

    u16 field::getStat( bool p_opponent, u8 p_slot, u8 p_stat, bool p_allowAbilities,
                        bool p_ignoreNegative, bool p_ignorePositive ) {
        if( getPkmn( p_opponent, p_slot ) == nullptr ) [[unlikely]] { return 0; }

        bool allowAbilities = p_allowAbilities && !suppressesAbilities( );
        bool allowWeather   = !suppressesWeather( );

        u16 base = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStat(
            p_slot, p_stat, allowAbilities, p_ignoreNegative, p_ignorePositive,
            ( getPseudoWeather( ) & PW_WONDERROOM ) != 0 );

        if( allowWeather && allowAbilities && !p_ignorePositive ) {
            if( _weather == WE_SUN || _weather == WE_HEAVY_SUNSHINE ) {
                auto p1 = getPkmn( p_opponent, p_slot );
                auto p2 = getPkmn( p_opponent, !p_slot );

                // flower gift
                if( ( p1 != nullptr && p1->getAbility( ) == A_FLOWER_GIFT )
                    || ( p2 != nullptr && p2->getAbility( ) == A_FLOWER_GIFT ) ) {
                    if( p_stat == ATK || p_stat == SDEF ) { base = ( 3 * base ) >> 1; }
                }

                // solar power
                if( p1 != nullptr && p1->getAbility( ) == A_SOLAR_POWER && p_stat == SATK ) {
                    base = ( 3 * base ) >> 1;
                }

                // chlorophyll
                if( p1 != nullptr && p1->getAbility( ) == A_CHLOROPHYLL && p_stat == SPEED ) {
                    base <<= 1;
                }
            }
            if( _weather == WE_SANDSTORM ) {
                auto p1 = getPkmn( p_opponent, p_slot );
                // sand rush
                if( p1 != nullptr && p1->getAbility( ) == A_SAND_RUSH && p_stat == SPEED ) {
                    base <<= 1;
                }
            }
            if( _weather == WE_RAIN || _weather == WE_HEAVY_RAIN ) {
                auto p1 = getPkmn( p_opponent, p_slot );
                // swift swin
                if( p1 != nullptr && p1->getAbility( ) == A_SWIFT_SWIM && p_stat == SPEED ) {
                    base <<= 1;
                }
            }
            if( _weather == WE_HAIL ) {
                auto p1 = getPkmn( p_opponent, p_slot );
                // slush rush
                if( p1 != nullptr && p1->getAbility( ) == A_SLUSH_RUSH && p_stat == SPEED ) {
                    base <<= 1;
                }
            }
        }

        if( allowAbilities && !p_ignorePositive ) {
            if( _terrain == TR_GRASSYTERRAIN ) {
                if( getPkmn( p_opponent, p_slot )->getAbility( ) == A_GRASS_PELT ) {
                    if( p_stat == DEF ) { base = ( base * 3 ) / 2; }
                }
            }
            if( _terrain == TR_ELECTRICTERRAIN ) {
                if( getPkmn( p_opponent, p_slot )->getAbility( ) == A_SURGE_SURFER ) {
                    if( p_stat == SPEED ) { base <<= 1; }
                }
            }
            auto pk = getPkmn( p_opponent, p_slot );
            if( _paradoxStat[ p_opponent ][ p_slot ] == p_stat && pk != nullptr
                && ( pk->getAbility( ) == A_PROTOSYNTHESIS || pk->getAbility( ) == A_QUARK_DRIVE )
                && !slotAbilitiesNullified( p_opponent, p_slot ) ) {
                if( p_stat == SPEED ) {
                    base = ( base * 3 ) / 2;
                } else {
                    base = ( base * 5325 ) / 4096;
                }
            }
        }
        if( !p_ignorePositive && p_stat == SPEED
            && ( getSideCondition( p_opponent ) & SC_TAILWIND ) ) {
            base <<= 1;
        }

        return std::max( u16( 1 ), base );
    }
} // namespace BATTLE
