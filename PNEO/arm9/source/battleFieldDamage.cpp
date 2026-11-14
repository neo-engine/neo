/*
Pokémon neo
------------------------------

file        : battleFieldDamage.cpp
author      : Philip Wellnitz
description : damaging-move execution: damage, drain, flinch, post-hit bookkeeping

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
    bool field::executeDamagingMove( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                     bool p_critical, u8 p_damageModifier ) {
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        auto user   = getPkmn( p_move.m_user.first, p_move.m_user.second );
        auto target = getPkmn( p_target.first, p_target.second );
        if( user == nullptr || target == nullptr ) [[unlikely]] { return false; }

        _hitSubstituted  = false; // per-hit; see the substitute soak below
        bool supprAbs    = suppressesAbilities( );
        auto userVolStat = getVolatileStatus( p_move.m_user.first, p_move.m_user.second );

        if( p_move.m_param == M_UPROAR ) [[unlikely]] { uproarWakeAll( p_ui ); }
        if( windRiderSteal( p_ui, p_move, p_target ) ) [[unlikely]] { return false; }
        u16 effectiveness = getEffectiveness( p_move, p_target );
        if( effectiveness == 0 ) {
            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_NO_EFFECT_ON ) );
            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                      p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                         p_target.first, false )
                          .c_str( ) );
            p_ui->log( buffer );
            return false;
        }

        // Calculate damage
        u32 damage = 0;

        if( p_move.m_moveData.m_fixedDamage ) {
            damage = p_move.m_moveData.m_fixedDamage;
        } else if( p_move.m_powerOverride ) [[unlikely]] {
            damage = p_move.m_powerOverride;
        } else {
            u16 atk = getStat( p_move.m_user.first, p_move.m_user.second,
                               p_move.m_moveData.m_category == MH_SPECIAL ? SATK : ATK,
                               !supprAbs && p_move.m_param != M_SUNSTEEL_STRIKE, p_critical );
            if( p_move.m_param == M_FOUL_PLAY ) [[unlikely]] {
                u16 tgtBare = getStat( p_target.first, p_target.second, ATK, false, p_critical );
                u16 usrFull = getStat( p_move.m_user.first, p_move.m_user.second, ATK, !supprAbs,
                                       p_critical );
                u16 usrBare
                    = getStat( p_move.m_user.first, p_move.m_user.second, ATK, false, p_critical );
                if( tgtBare && usrBare ) { atk = u16( u32( tgtBare ) * usrFull / usrBare ); }
            }
            u16 def
                = getStat( p_target.first, p_target.second,
                           p_move.m_moveData.m_defensiveCategory == MH_SPECIAL ? SDEF : DEF,
                           !supprAbs && p_move.m_param != M_SUNSTEEL_STRIKE, false, p_critical );
            if( p_move.m_moveData.m_flags & MF_DEFASOFF ) {
                atk = getStat( p_move.m_user.first, p_move.m_user.second,
                               p_move.m_moveData.m_category == MH_SPECIAL ? SDEF : DEF,
                               !supprAbs && p_move.m_param != M_SUNSTEEL_STRIKE, p_critical );
            }

            damage = ( user->m_level * 2 ) / 5 + 2;

            // base power modifying stuff
            u16 movePower = getMovePower( p_move, p_target );

            if( p_move.m_param == M_BEAT_UP ) [[unlikely]] {
                if( _beatUpIdx < 6 ) { movePower = _beatUpBP[ _beatUpIdx ]; }
                _beatUpIdx++;
            }

            for( u8 hh = getVolatileStatusAmount( p_move.m_user.first, p_move.m_user.second,
                                                  VS_HELPINGHAND );
                 hh; --hh ) {
                movePower = ( movePower * 15 ) / 10;
            }

            // speed-based
            if( p_move.m_param == M_GYRO_BALL ) [[unlikely]] {
                auto atkspd = getStat( p_move.m_user.first, p_move.m_user.second, SPEED );
                auto defspd = getStat( p_target.first, p_move.m_user.second, SPEED );

                movePower = atkspd ? 25 * defspd / atkspd : 150;
                if( movePower > 150 ) { movePower = 150; }
                if( movePower == 0 ) { movePower = 1; }
            }

            if( p_move.m_param == M_ELECTRO_BALL ) [[unlikely]] {
                auto atkspd = getStat( p_move.m_user.first, p_move.m_user.second, SPEED );
                auto defspd = getStat( p_target.first, p_move.m_user.second, SPEED );

                if( !defspd ) {
                    movePower = 150;
                } else {
                    switch( atkspd / defspd ) {
                    case 0: movePower = 40; break;
                    case 1: movePower = 60; break;
                    case 2: movePower = 80; break;
                    case 3: movePower = 120; break;
                    default: movePower = 150; break;
                    }
                }
            }

            // weight-based
            auto tgweight   = getWeight( p_target.first, p_target.second, !supprAbs );
            auto userweight = getWeight( p_move.m_user.first, p_move.m_user.second, !supprAbs );

            if( p_move.m_param == M_LOW_KICK || p_move.m_param == M_GRASS_KNOT ) {
                if( tgweight < TMP_BUFFER_SIZE ) {
                    movePower = 20;
                } else if( tgweight < 249 ) {
                    movePower = 40;
                } else if( tgweight < 499 ) {
                    movePower = 60;
                } else if( tgweight < 999 ) {
                    movePower = 80;
                } else if( tgweight < 1999 ) {
                    movePower = 100;
                } else {
                    movePower = 120;
                }
            }

            if( p_move.m_param == M_HEAVY_SLAM || p_move.m_param == M_HEAT_CRASH ) {
                if( tgweight * 2 > userweight ) {
                    movePower = 40;
                } else if( tgweight * 3 > userweight ) {
                    movePower = 60;
                } else if( tgweight * 4 > userweight ) {
                    movePower = 80;
                } else if( tgweight * 5 > userweight ) {
                    movePower = 100;
                } else {
                    movePower = 120;
                }
            }

            // HP-based
            // (clamped divisors: a zero max stat would hang the ARM946 software divider)
            u32 userMaxHP   = user->m_stats.m_maxHP ? user->m_stats.m_maxHP : 1;
            u32 targetMaxHP = target->m_stats.m_maxHP ? target->m_stats.m_maxHP : 1;
            if( p_move.m_param == M_ERUPTION || p_move.m_param == M_WATER_SPOUT
                || p_move.m_param == M_DRAGON_ENERGY ) {
                movePower = 150 * user->m_stats.m_curHP / userMaxHP;
            }
            if( p_move.m_param == M_FLAIL || p_move.m_param == M_REVERSAL ) {
                auto relhp = 48 * user->m_stats.m_curHP / userMaxHP;
                if( relhp > 32 ) {
                    movePower = 20;
                } else if( relhp > 16 ) {
                    movePower = 40;
                } else if( relhp > 9 ) {
                    movePower = 60;
                } else if( relhp > 4 ) {
                    movePower = 100;
                } else if( relhp > 1 ) {
                    movePower = 150;
                } else {
                    movePower = 200;
                }
            }
            if( p_move.m_param == M_CRUSH_GRIP || p_move.m_param == M_WRING_OUT ) {
                movePower = 120 * target->m_stats.m_curHP / targetMaxHP;
            }

            // happiness-based

            if( p_move.m_param == M_RETURN ) { movePower = 1 + user->m_boxdata.m_steps * 10 / 25; }
            if( p_move.m_param == M_FRUSTRATION ) {
                movePower = 1 + ( 255 - user->m_boxdata.m_steps ) * 10 / 25;
            }

            // repetition-based

            if( p_move.m_param == M_FURY_CUTTER ) {
                u8 cnt = getConsecutiveMoveCount( p_move.m_user.first, p_move.m_user.second );

                if( cnt == 0
                    || getLastUsedMove( p_move.m_user.first, p_move.m_user.second ).m_param
                           != M_FURY_CUTTER ) {
                    movePower = 40;
                } else if( cnt == 1 ) {
                    movePower = 80;
                } else {
                    movePower = 160;
                }
            }
            if( p_move.m_param == M_ROLLOUT || p_move.m_param == M_ICE_BALL ) {
                u8 cnt = getConsecutiveMoveCount( p_move.m_user.first, p_move.m_user.second );

                if( cnt == 0
                    || getLastUsedMove( p_move.m_user.first, p_move.m_user.second ).m_param
                           != p_move.m_param ) {
                    movePower = 30;
                } else if( cnt == 1 ) {
                    movePower = 60;
                } else if( cnt == 2 ) {
                    movePower = 120;
                } else if( cnt == 3 ) {
                    movePower = 240;
                } else {
                    movePower = 480;
                }

                if( getVolatileStatus( p_move.m_user.first, p_move.m_user.second )
                    & VS_DEFENSECURL ) {
                    movePower <<= 1;
                }
            }
            if( p_move.m_param == M_SPIT_UP ) {
                u8 stkpile = getVolatileStatusCounter( p_move.m_user.first, p_move.m_user.second,
                                                       VS_STOCKPILE );
                if( stkpile > 3 ) { stkpile = 3; }

                movePower = 100 * stkpile;

                removeVolatileStatus( p_ui, p_move.m_user.first, p_move.m_user.second,
                                      VS_STOCKPILE );
                boosts bt = boosts( );
                bt.setBoost( DEF, -stkpile );
                bt.setBoost( SDEF, -stkpile );
                auto res = addBoosts( p_move.m_user.first, p_move.m_user.second, bt );
                p_ui->logBoosts( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                 p_move.m_user.first, p_move.m_user.second, bt, res );
            }

            damage *= movePower;

            if( !def ) { def = 1; } // zero stat (corrupt data): divisor 0 hangs ARM946
            damage = ( damage * atk ) / def;
            damage = ( damage / 50 ) + 2;

            if( p_move.m_target.size( ) > 1 ) {
                damage = ( damage * 75 ) / 100;
            } else if( p_damageModifier < 100 ) {
                damage = ( damage * p_damageModifier ) / 100;
            }

            if( !suppressesWeather( ) ) [[likely]] {
                if( p_move.m_moveData.m_type == TYPE_WATER
                    && ( _weather == WE_RAIN || _weather == WE_HEAVY_RAIN ) ) [[unlikely]] {
                    damage = ( damage * 3 ) >> 1;
                }
                if( p_move.m_moveData.m_type == TYPE_FIRE
                    && ( _weather == WE_RAIN || _weather == WE_HEAVY_RAIN ) ) [[unlikely]] {
                    damage = ( damage >> 1 );
                }
                if( p_move.m_moveData.m_type == TYPE_FIRE
                    && ( _weather == WE_SUN || _weather == WE_HEAVY_SUNSHINE ) ) [[unlikely]] {
                    damage = ( damage * 3 ) >> 1;
                }
                if( p_move.m_moveData.m_type == TYPE_WATER
                    && ( _weather == WE_SUN || _weather == WE_HEAVY_SUNSHINE ) ) [[unlikely]] {
                    damage = ( damage >> 1 );
                }
            }

            if( p_critical ) { damage = ( damage * 3 ) >> 1; }

            u8 rnd = rand( ) & 15;
            damage = ( damage * ( 85 + rnd ) / 100 );

            // stab
            type moveType = getMoveType( p_move );

            if( !supprAbs && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                && ( user->getAbility( ) == A_PROTEAN || user->getAbility( ) == A_LIBERO ) ) {
                p_ui->logAbility( getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first );
                setType( p_ui, p_move.m_user.first, p_move.m_user.second, moveType );
                // TODO log
            }

            if( hasType( p_move.m_user.first, p_move.m_user.second, moveType ) ) {
                if( !supprAbs
                    && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                    && user->getAbility( ) == A_ADAPTABILITY ) {
                    damage <<= 1;
                } else {
                    damage = ( damage * 3 ) >> 1;
                }
            }

            // effectiveness
            damage = ( damage * effectiveness ) / 100;

            // burn

            if( user->m_status.m_isBurned && p_move.m_moveData.m_category == MH_PHYSICAL
                && ( supprAbs || slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                     || user->getAbility( ) != A_GUTS )
                && p_move.m_param != M_FACADE ) {
                damage >>= 1;
            }

            if( user->getItem( ) == I_METRONOME ) {
                static constexpr u16 DMG_MOD[ 6 ] = { 4096, 4915, 5734, 6553, 7372, 8192 };
                u8 n = _metronomeNum[ p_move.m_user.first ][ p_move.m_user.second ];
                if( n > 5 ) { n = 5; }
                if( n != 0 ) { damage = ( damage * DMG_MOD[ n ] ) / 4096; }
            }

            auto sidec = getSideCondition( !p_move.m_user.first );

            if( supprAbs || !anyHasAbility( A_SCREEN_CLEANER ) ) {
                if( ( sidec & SC_AURORAVEIL )
                    || ( ( sidec & SC_REFLECT )
                         && p_move.m_moveData.m_defensiveCategory == MH_PHYSICAL )
                    || ( ( sidec & SC_LIGHTSCREEN )
                         && p_move.m_moveData.m_defensiveCategory == MH_SPECIAL ) ) {
                    // nerf intentional!
                    damage = ( damage * 66 ) / 100;
                }
            }

            if( minimizeSensitive( p_move.m_param ) ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_MINIMIZE )
                    [[unlikely]] {
                    damage <<= 1;
                }
            }

            if( p_move.m_param == M_SURF || p_move.m_param == M_WHIRLPOOL ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_DIVING ) {
                    damage <<= 1;
                }
            }

            if( p_move.m_param == M_EARTHQUAKE || p_move.m_param == M_MAGNITUDE ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_DIGGING ) {
                    damage <<= 1;
                }
            }

            if( p_move.m_param == M_GUST || p_move.m_param == M_TWISTER ) {
                if( getVolatileStatus( p_target.first, p_target.second ) & VS_INAIR ) {
                    damage <<= 1;
                }
            }

            // abilities

            if( !supprAbs && !slotAbilitiesNullified( p_target.first, p_target.second ) ) {
                switch( target->getAbility( ) ) {
                case A_FLUFFY:
                    if( ( p_move.m_moveData.m_flags & MF_CONTACT )
                        && !( ( p_move.m_moveData.m_flags & MF_PUNCH )
                              && canUseItem( p_move.m_user.first, p_move.m_user.second )
                              && user->getItem( ) == I_PUNCHING_GLOVE ) ) {
                        if( moveType == TYPE_FIRE ) {
                            damage <<= 1;
                        } else {
                            damage >>= 1;
                        }
                    }
                    break;
                case A_FILTER:
                case A_PRISM_ARMOR:
                case A_SOLID_ROCK:
                    if( effectiveness > 100 ) { damage = ( damage * 75 ) / 100; }
                    break;
                case A_SHADOW_SHIELD:
                case A_MULTISCALE:
                    if( target->m_stats.m_curHP == target->m_stats.m_maxHP ) { damage >>= 1; }
                    break;
                case A_ICE_SCALES:
                    if( p_move.m_moveData.m_defensiveCategory == MH_SPECIAL ) { damage >>= 1; }
                    break;
                case A_PUNK_ROCK:
                    if( p_move.m_moveData.m_flags & MF_SOUND ) { damage >>= 1; }
                    break;
                [[likely]] default:
                    break;
                }

                if( getPkmn( p_target.first, !p_target.second ) != nullptr
                    && !slotAbilitiesNullified( p_target.first, u8( !p_target.second ) )
                    && getPkmn( p_target.first, !p_target.second )->getAbility( )
                           == A_FRIEND_GUARD ) [[unlikely]] {
                    damage = ( damage * 75 ) / 100;
                }

                switch( !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                            ? user->getAbility( )
                            : u16( 0 ) ) {
                case A_TORRENT:
                    if( moveType == TYPE_WATER
                        && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                        damage = ( 3 * damage ) >> 1;
                    }
                    break;
                case A_OVERGROW:
                    if( moveType == TYPE_GRASS
                        && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                        damage = ( 3 * damage ) >> 1;
                    }
                    break;
                case A_BLAZE:
                    if( moveType == TYPE_FIRE
                        && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                        damage = ( 3 * damage ) >> 1;
                    }
                    break;
                case A_SWARM:
                    if( moveType == TYPE_BUG
                        && user->m_stats.m_curHP * 3 < user->m_stats.m_maxHP ) {
                        damage = ( 3 * damage ) >> 1;
                    }
                    break;
                case A_STEELWORKER:
                    if( moveType == TYPE_STEEL ) { damage = ( 3 * damage ) >> 1; }
                    break;

                case A_RIVALRY:
                    if( user->gender( ) * target->gender( ) < 0 ) {
                        damage = ( damage * 75 ) / 100;
                    } else if( user->gender( ) * target->gender( ) > 0 ) {
                        damage = ( damage * 125 ) / 100;
                    }
                    break;
                case A_NEUROFORCE:
                    if( effectiveness > 100 ) { damage = ( damage * 125 ) / 100; }
                    break;
                case A_TINTED_LENS:
                    if( effectiveness < 100 ) { damage <<= 1; }
                    break;
                case A_SNIPER:
                    if( p_critical ) { damage = ( 3 * damage ) >> 1; }
                    break;

                [[likely]] default:
                    break;
                }
            }

            if( canUseItem( p_move.m_user.first, p_move.m_user.second, !supprAbs ) ) {
                switch( user->getItem( ) ) {
                case I_EXPERT_BELT:
                    if( effectiveness > 100 ) { damage = ( damage * 4915 ) / 4096; }
                    break;
                case I_LIFE_ORB: damage = ( damage * 5324 ) / 4096; break;
                case I_METRONOME: {
                    auto cons = 100
                                + 20
                                      * std::min( u8( 5 ),
                                                  getConsecutiveMoveCount( p_move.m_user.first,
                                                                           p_move.m_user.second ) );
                    damage    = ( damage * cons / 100 );
                    break;
                }
                default: break;
                }
            }

            if( canUseItem( p_target.first, p_target.second, !supprAbs )
                && ( !supprAbs
                     || !_sides[ p_target.first ? PLAYER_SIDE : OPPONENT_SIDE ].anyHasAbility(
                         A_UNNERVE ) ) ) {
                bool eatitem = false;
                switch( target->getItem( ) ) {
                case I_CHILAN_BERRY:
                    if( moveType == TYPE_NORMAL ) { eatitem = true; }
                    break;
                case I_BABIRI_BERRY:
                    if( moveType == TYPE_STEEL && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_CHARTI_BERRY:
                    if( moveType == TYPE_ROCK && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_CHOPLE_BERRY:
                    if( moveType == TYPE_FIGHTING && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_COBA_BERRY:
                    if( moveType == TYPE_FLYING && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_COLBUR_BERRY:
                    if( moveType == TYPE_DARKNESS && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_HABAN_BERRY:
                    if( moveType == TYPE_DRAGON && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_KASIB_BERRY:
                    if( moveType == TYPE_GHOST && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_KEBIA_BERRY:
                    if( moveType == TYPE_POISON && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_OCCA_BERRY:
                    if( moveType == TYPE_FIRE && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_PASSHO_BERRY:
                    if( moveType == TYPE_WATER && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_PAYAPA_BERRY:
                    if( moveType == TYPE_PSYCHIC && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_RINDO_BERRY:
                    if( moveType == TYPE_GRASS && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_ROSELI_BERRY:
                    if( moveType == TYPE_FAIRY && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_SHUCA_BERRY:
                    if( moveType == TYPE_GROUND && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_TANGA_BERRY:
                    if( moveType == TYPE_BUG && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_WACAN_BERRY:
                    if( moveType == TYPE_LIGHTNING && effectiveness > 100 ) { eatitem = true; }
                    break;
                case I_YACHE_BERRY:
                    if( moveType == TYPE_ICE && effectiveness > 100 ) { eatitem = true; }
                    break;
                default: break;
                }
                if( eatitem ) {
                    p_ui->logItem( target, p_target.first );
                    if( target->getAbility( ) != A_RIPEN || supprAbs
                        || slotAbilitiesNullified( p_target.first, p_target.second ) ) {
                        damage >>= 1;
                    } else {
                        damage >>= 2;
                    }
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_EAT_ITEM ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                    p_move.m_user.second ),
                                                 p_move.m_user.first )
                                  .c_str( ),
                              FS::getItemName( target->getItem( ) ).c_str( ) );
                    p_ui->log( buffer );

                    removeItem( p_ui, p_target.first, p_target.second );
                    checkOnEatBerry( p_ui, p_target.first, p_target.second, target->getItem( ) );
                }
            }

            if( !damage ) { damage = 1; }
            if( damage > 9000 ) { damage = 9000; }
        }

        p_ui->animateHitPkmn( p_target.first, p_target.second, effectiveness );

        if( damage >= target->m_stats.m_curHP ) { damage = target->m_stats.m_curHP; }

        if( p_move.m_moveData.m_flags & MF_NOFAINT ) {
            if( damage >= target->m_stats.m_curHP ) { damage = target->m_stats.m_curHP - 1; }
        }
        if( getVolatileStatus( p_target.first, p_target.second ) & VS_ENDURE ) [[unlikely]] {
            if( damage >= target->m_stats.m_curHP ) {
                damage    = target->m_stats.m_curHP - 1;
                auto fmtE = std::string( GET_STRING( IO::STR_BATTLE_ENDURED_THE_HIT ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmtE.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first )
                              .c_str( ) );
                p_ui->log( buffer );
            }
        }

        if( damage >= target->m_stats.m_curHP ) {
            if( target->m_stats.m_curHP == target->m_stats.m_maxHP ) {
                if( canUseItem( p_target.first, p_target.second, !supprAbs )
                    && target->getItem( ) == I_FOCUS_SASH ) {
                    damage = target->m_stats.m_curHP - 1;
                    p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                   p_target.first );
                    // consume item
                    removeItem( p_ui, p_target.first, p_target.second );
                }
                if( canUseItem( p_target.first, p_target.second, !supprAbs )
                    && target->getItem( ) == I_FOCUS_BAND && rand( ) % 100 < 10 ) {
                    damage = target->m_stats.m_curHP - 1;
                    p_ui->logItem( getPkmnOrDisguise( p_target.first, p_target.second ),
                                   p_target.first );
                }

                if( !supprAbs && !slotAbilitiesNullified( p_target.first, p_target.second )
                    && target->getAbility( ) == A_STURDY ) {
                    damage = target->m_stats.m_curHP - 1;
                    p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                      p_target.first );
                }
            }
        }

        if( !substituteExempt( p_move, p_target ) ) [[unlikely]] {
            u16 subHp = getSubstituteHp( p_target.first, p_target.second );
            if( subHp ) {
                const u32 soak = damage > subHp ? subHp : damage;
                if( damage >= subHp ) {
                    setSubstituteHp( p_target.first, p_target.second, 0 );
                    removeVolatileStatus( p_ui, p_target.first, p_target.second, VS_SUBSTITUTE );
                    p_ui->log( GET_STRING( IO::STR_BATTLE_SUBSTITUTE_FADED ) );
                } else {
                    setSubstituteHp( p_target.first, p_target.second,
                                     u16( subHp - u16( damage ) ) );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_SUBSTITUTE_TAKES_HIT ) );
                    snprintf(
                        buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                        p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                           p_target.first, false )
                            .c_str( ),
                        FS::getMoveName( p_move.m_param ).c_str( ) );
                    p_ui->log( buffer );
                }

                if( damage && p_move.m_moveData.m_recoil
                    && !( p_move.m_param != M_STRUGGLE && !suppressesAbilities( )
                          && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                          && user != nullptr && user->getAbility( ) == A_ROCK_HEAD ) ) {
                    u32 amount = soak * p_move.m_moveData.m_recoil / 240;
                    if( !amount ) { amount = 1; }
                    damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second, u16( amount ) );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HIT_BY_RECOIL ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                    p_move.m_user.second ),
                                                 p_move.m_user.first )
                                  .c_str( ) );
                    p_ui->log( buffer );
                }

                if( damage && p_move.m_moveData.m_drain ) {
                    u32 amount = soak * p_move.m_moveData.m_drain / 240;
                    if( canUseItem( p_move.m_user.first, p_move.m_user.second )
                        && user->getItem( ) == I_BIG_ROOT ) {
                        amount = 13 * amount / 10;
                    }
                    if( !amount ) { amount = 1; }
                    if( !suppressesAbilities( )
                        && !slotAbilitiesNullified( p_target.first, p_target.second )
                        && target->getAbility( ) == A_LIQUID_OOZE ) {
                        p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                          p_target.first );
                        damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                       u16( amount ) );
                    } else if( !( userVolStat & VS_HEALBLOCK ) ) {
                        if( user->m_stats.m_curHP < user->m_stats.m_maxHP ) {
                            healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                         u16( amount ) );
                            auto fmt = std::string( GET_STRING( IO::STR_BATTLE_RESTORES_HP ) );
                            snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                      p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                            p_move.m_user.second ),
                                                         p_move.m_user.first )
                                          .c_str( ) );
                            p_ui->log( buffer );
                        }
                    }
                }

                if( user->canBattle( ) && canUseItem( p_move.m_user.first, p_move.m_user.second )
                    && user->getItem( ) == I_LIFE_ORB ) {
                    damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                                   user->m_stats.m_maxHP / 10 );
                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_LOSES_SOME_HP ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                    p_move.m_user.second ),
                                                 p_move.m_user.first )
                                  .c_str( ) );
                    p_ui->log( buffer );
                }

                if( !user->canBattle( ) ) {
                    faintPokemon( p_ui, p_move.m_user.first, p_move.m_user.second );
                }
                _hitSubstituted = true;
                return true;
            }
        }

        // check for A_DISGUISE
        if( !supprAbs && target->getAbility( ) == A_DISGUISE && !target->getForme( ) ) {
            p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                              p_target.first );
            target->setBattleForme( 1 );
            p_ui->updatePkmn( p_target.first, p_target.second,
                              getPkmnOrDisguise( p_target.first, p_target.second ) );
        } else {
            damagePokemon( p_ui, p_target.first, p_target.second, damage );

            if( getVolatileStatus( p_target.first, p_target.second ) & VS_BIDE ) [[unlikely]] {
                addBideDamage( p_target.first, p_target.second, damage, p_move.m_user );
            }

            if( effectiveness > 100 ) {
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_SUPER_EFFECTIVE_ON ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first, false )
                              .c_str( ) );
                p_ui->log( buffer );
            } else if( effectiveness < 100 ) {
                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_NOT_VERY_EFFECTIVE_ON ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName( getPkmnOrDisguise( p_target.first, p_target.second ),
                                             p_target.first, false )
                              .c_str( ) );
                p_ui->log( buffer );
            }

            if( p_critical ) { p_ui->log( GET_STRING( IO::STR_BATTLE_CRITICAL_HIT ) ); }

            // Calculate recoil / drain

            if( p_move.m_moveData.m_drain ) {
                u32 amount = damage * p_move.m_moveData.m_drain / 240;
                if( canUseItem( p_move.m_user.first, p_move.m_user.second )
                    && user->getItem( ) == I_BIG_ROOT ) {
                    amount = 13 * amount / 10;
                }
                if( !amount ) { amount = 1; }

                if( !suppressesAbilities( )
                    && !slotAbilitiesNullified( p_target.first, p_target.second )
                    && target->getAbility( ) == A_LIQUID_OOZE ) {
                    p_ui->logAbility( getPkmnOrDisguise( p_target.first, p_target.second ),
                                      p_target.first );
                    damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );
                } else if( !( userVolStat & VS_HEALBLOCK ) ) {
                    if( user->m_stats.m_curHP < user->m_stats.m_maxHP ) {
                        healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );

                        auto fmt = std::string( GET_STRING( IO::STR_BATTLE_RESTORES_HP ) );
                        snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                                  p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                        p_move.m_user.second ),
                                                     p_move.m_user.first )
                                      .c_str( ) );
                        p_ui->log( buffer );
                    }
                }
            }

            if( p_move.m_moveData.m_recoil
                && !( p_move.m_param != M_STRUGGLE && !suppressesAbilities( )
                      && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                      && user != nullptr && user->getAbility( ) == A_ROCK_HEAD ) ) {
                u32 amount = damage * p_move.m_moveData.m_recoil / 240;
                if( !amount ) { amount = 1; }

                damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );

                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_HIT_BY_RECOIL ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName(
                                  getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first )
                              .c_str( ) );
                p_ui->log( buffer );
            }

            // Shell bell
            if( user->canBattle( ) && canUseItem( p_move.m_user.first, p_move.m_user.second )
                && user->getItem( ) == I_SHELL_BELL ) {
                if( !( userVolStat & VS_HEALBLOCK ) ) {
                    u32 amount = damage / 8;
                    if( !amount ) { amount = 1; }

                    healPokemon( p_ui, p_move.m_user.first, p_move.m_user.second, amount );

                    auto fmt = std::string( GET_STRING( IO::STR_BATTLE_SHELL_BELL_HEALS ) );
                    snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                              p_ui->getPkmnName( getPkmnOrDisguise( p_move.m_user.first,
                                                                    p_move.m_user.second ),
                                                 p_move.m_user.first )
                                  .c_str( ) );
                    p_ui->log( buffer );
                }
            }

            if( user->canBattle( ) && canUseItem( p_move.m_user.first, p_move.m_user.second )
                && user->getItem( ) == I_LIFE_ORB ) {

                damagePokemon( p_ui, p_move.m_user.first, p_move.m_user.second,
                               user->m_stats.m_maxHP / 10 );

                auto fmt = std::string( GET_STRING( IO::STR_BATTLE_LOSES_SOME_HP ) );
                snprintf( buffer, TMP_BUFFER_SIZE, fmt.c_str( ),
                          p_ui->getPkmnName(
                                  getPkmnOrDisguise( p_move.m_user.first, p_move.m_user.second ),
                                  p_move.m_user.first )
                              .c_str( ) );
                p_ui->log( buffer );
            }

            // check for things that trigger when causing damage
            checkOnTakeDamage( p_ui, p_move, p_target, damage, effectiveness );
        }

        bool dbArmed
            = ( getVolatileStatus( p_target.first, p_target.second ) & VS_DESTINYBOND ) != 0;
        bool grudgeArmed
            = ( getVolatileStatus( p_target.first, p_target.second ) & VS_GRUDGE ) != 0;

        if( !getPkmn( p_target.first, p_target.second )->canBattle( ) ) {
            // pkmn fainted
            faintPokemon( p_ui, p_target.first, p_target.second );
            if( grudgeArmed ) { grudgePayOff( p_ui, p_move ); }
        }

        // Check if user fainted
        if( !user->canBattle( ) ) {
            faintPokemon( p_ui, p_move.m_user.first, p_move.m_user.second );
        }

        if( dbArmed ) { destinyBondTakeDown( p_ui, p_move, p_target ); }

        return true;
    }

} // namespace BATTLE
