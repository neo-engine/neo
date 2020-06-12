/*
    Pokémon neo
    ------------------------------

    file        : battle.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2020
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

#pragma once
#include <nds.h>

namespace MOVE {
    enum moveFlags : long long unsigned {
        /** Ignores a target's substitute. */
        AUTHENTIC = ( 1 << 0 ),
        /** Power is multiplied by 1.5 when used by a Pokemon with the Strong Jaw Ability. */
        BITE = ( 1 << 1 ),
        /** Has no effect on Pokemon with the Bulletproof Ability. */
        BULLET = ( 1 << 2 ),
        /** The user is unable to make a move between turns. */
        CHARGE = ( 1 << 3 ),
        /** Makes contact. */
        CONTACT = ( 1 << 4 ),
        /** When used by a Pokemon, other Pokemon with the Dancer Ability can attempt to execute the
           same   move. */
        DANCE = ( 1 << 5 ),
        /** Thaws the user if executed successfully while the user is frozen. */
        DEFROST = ( 1 << 6 ),
        /** Can target a Pokemon positioned anywhere in a Triple Battle. */
        DISTANCE = ( 1 << 7 ),
        /** Prevented from being executed or selected during Gravity's effect. */
        GRAVITY = ( 1 << 8 ),
        /** Prevented from being executed or selected during Heal Block's effect. */
        HEAL = ( 1 << 9 ),
        /** Can be copied by Mirror Move. */
        MIRROR = ( 1 << 10 ),
        /** Recoil halves HP */
        MINDBLOWNRECOIL = ( 1 << 11 ),
        /** Prevented from being executed or selected in a Sky Battle. */
        NONSKY = ( 1 << 12 ),
        /** Has no effect on Grass-type Pokemon, Pokemon with the Overcoat Ability, and Pokemon
           holding     Safety Goggles. */
        POWDER = ( 1 << 13 ),
        /** Blocked by Detect, Protect, Spiky Shield, and if not a Status move, King's Shield. */
        PROTECT = ( 1 << 14 ),
        /** Power is multiplied by 1.5 when used by a Pokemon with the Mega Launcher Ability. */
        PULSE = ( 1 << 15 ),
        /** Power is multiplied by 1.2 when used by a Pokemon with the Iron Fist Ability. */
        PUNCH = ( 1 << 16 ),
        /** If this move is successful, the user must recharge on the following turn and cannot make
           a      move. */
        RECHARGE = ( 1 << 17 ),
        /** Bounced back to the original user by Magic Coat or the Magic Bounce Ability. */
        REFLECTABLE = ( 1 << 18 ),
        /** Can be stolen from the original user and instead used by another Pokemon using Snatch.
         */
        SNATCH = ( 1 << 19 ),
        /** Has no effect on Pokemon with the Soundproof Ability. */
        SOUND = ( 1 << 20 ),
        /** Forces the pokemon to use the move for another 1-2 turns. */
        LOCKEDMOVE = ( 1 << 21 ),
        /** Maintains the rage counter */
        RAGE = ( 1 << 22 ),
        /** Roost */
        ROOST = ( 1 << 23 ),
        /** Uproar */
        UPROAR = ( 1 << 24 ),
        /** Self-Switch: User switches after successful use */
        SELFSWITCH = ( 1 << 25 ),
        /** Use source defensive stats as offensive stats */
        DEFASOFF = ( 1 << 26 ),
        /** User is damaged if the attack misses */
        CRASHDAMAGE = ( 1 << 27 ),
        /** OHKO move */
        OHKO = ( 1 << 28 ),
        /** PKMN self-destructs */
        SELFDESTRUCT = ( 1 << 29 ),
        /** PKMN self-destructs */
        SELFDESTRUCTHIT = ( 1 << 30 ),
        /** move cannot be sketched */
        NOSKETCH = ( 1LLU << 31 ),
        /** move pp cannot be increased */
        NOPPBOOST = ( 1LLU << 32 ),
        /** move can be used while asleep */
        SLEEPUSABLE = ( 1LLU << 33 ),
        /** target cannot faint due to move */
        NOFAINT = ( 1LLU << 34 ),
        /** breaks protect */
        BREAKSPROTECT = ( 1LLU << 35 ),
        /** ignores type immunities */
        IGNOREIMMUNITY = ( 1LLU << 36 ),
        /** ignores ground type immunity */
        IGNOREIMMUNITYGROUND = ( 1LLU << 37 ),
        /** ignores abilities */
        IGNOREABILITY = ( 1LLU << 38 ),
        /** ignores defense boosts */
        IGNOREDEFS = ( 1LLU << 39 ),
        /** ignores evasion boosts */
        IGNOREEVASION = ( 1LLU << 40 ),
        /** Defrosts the target */
        DEFROSTTARGET = ( 1LLU << 41 ),
        /** Forces the target to switch out */
        FORCESWITCH = ( 1LLU << 42 ),
        /** Will always land a critical hit */
        WILLCRIT = ( 1LLU << 43 ),
        /** Move uses atk/satk of the target */
        TARGETOFFENSIVES = ( 1LLU << 44 ),
        /** Move hits at a later time */
        FUTUREMOVE = ( 1LLU << 45 ),
        /** Beton pass */
        BATONPASS = ( 1LLU << 46 ),
        /** OHKO move (useless on ice-type pkmn) */
        OHKOICE = ( 1LLU << 47 ),
    };

    enum moveHitTypes : u8 { NOOP = 0, PHYSICAL = 1, SPECIAL = 2, STATUS = 3 };

    enum target : u8 {
        NORMAL                = 0,
        ADJACENT_ALLY         = 2,
        ADJACENT_FOE          = 3,
        ADJACENT_ALLY_OR_SELF = 4,
        ANY                   = 12, // single target
        SELF                  = 5,
        RANDOM                = 6, // single-target, automatic
        ALLIES                = 13,
        ALL_ADJACENT          = 7,
        ALL_ADJACENT_FOES     = 8, // spread
        ALLY_SIDE             = 9,
        FOE_SIDE              = 10,
        ALL                   = 11, // field
        SCRIPTED              = 14,
        ALLY_TEAM             = 15,
    };
} // namespace MOVE

namespace BATTLE {
#define HP 0
#define ATK 1
#define DEF 2
#define SATK 3
#define SDEF 4
#define SPEED 5
#define EVASION 6
#define ACCURACY 7
    struct boosts {
        u32  m_boosts;

        boosts( ) {
            m_boosts = 0;
            for( u8 i = 0; i < 8; ++i ) { setBoost( i, 0 ); }
        }

        void setBoost( u8 p_stat, s8 p_val ) {
            if( p_val > 7 || p_val < -7 ) {
                fprintf( stderr, "Bad boosts value [%hhu] := %hhd\n", p_stat, p_val );
                return;
            }
            p_val += 7;

            m_boosts &= ( 0xFFFFFFFF - ( 0xF << ( 4 * p_stat ) ) );
            m_boosts |= ( p_val << ( 4 * p_stat ) );
        }
        constexpr s8 getBoost( u8 p_stat ) const {
            return ( ( m_boosts >> ( 4 * p_stat ) ) & 0xF ) - 7;
        }

        /*
         * @returns the boost for the specified stat in the range [ 0, 15 ]; 7 being no
         * boost.
         */
        constexpr u8 getShiftedBoost( u8 p_stat ) const {
            return ( ( m_boosts >> ( 4 * p_stat ) ) & 0xF );
        }

        /*
         * @brief: Adds the given boosts to the boosts.
         * @returns: the change in boosts.
         */
        inline boosts addBoosts( boosts p_other ) {
            boosts res = boosts( );
            for( u8 i = 0; i < 8; ++i ) {
                s8 old = getBoost( i );
                setBoost( i, std::min( s8( 6 ), std::max( s8( -6 ),
                                s8( old + p_other.getBoost( i ) ) ) ) );
                res.setBoost( i, getBoost( i ) - old );
            }
            return res;
        }

        /*
         * @brief: Inverts all boosts.
         */
        inline void invert( ) {
            for( u8 i = 0; i < 8; ++i ) {
                setBoost( i, -getBoost( i ) );
            }
        }
    };

#define MAX_VOLATILE_STATUS 51
    enum volatileStatus : u64 {
        NONE             = 0,
        CONFUSION        = ( 1 << 0 ),
        OBSTRUCT         = ( 1 << 1 ),
        PARTIALLYTRAPPED = ( 1 << 2 ),
        FLINCH           = ( 1 << 3 ),
        OCTOLOCK         = ( 1 << 4 ),
        TARSHOT          = ( 1 << 5 ),
        NORETREAT        = ( 1 << 6 ),
        LASERFOCUS       = ( 1 << 7 ),
        SPOTLIGHT        = ( 1 << 8 ),
        BANEFULBUNKER    = ( 1 << 9 ),
        SMACKDOWN        = ( 1 << 10 ),
        POWDERED         = ( 1 << 11 ),
        SPIKYSHIELD      = ( 1 << 12 ),
        KINGSSHIELD      = ( 1 << 13 ),
        ELECTRIFY        = ( 1 << 14 ),
        RAGEPOWDER       = ( 1 << 15 ),
        TELEKINESIS      = ( 1 << 16 ),
        MAGNETRISE       = ( 1 << 17 ),
        AQUARING         = ( 1 << 18 ),
        GASTROACID       = ( 1 << 19 ),
        POWERTRICK       = ( 1 << 20 ),
        HEALBLOCK        = ( 1 << 21 ),
        EMBARGO          = ( 1 << 22 ),
        MIRACLEEYE       = ( 1 << 23 ),
        SUBSTITUTE       = ( 1 << 24 ),
        BIDE             = ( 1 << 25 ),
        FOCUSENERGY      = ( 1 << 26 ),
        DEFENSECURL      = ( 1 << 27 ),
        MINIMIZE         = ( 1 << 28 ),
        LEECHSEED        = ( 1 << 29 ),
        DISABLE          = ( 1 << 30 ),
        FORESIGHT        = ( 1LLU << 31 ),
        SNATCH           = ( 1LLU << 32 ),
        GRUDGE           = ( 1LLU << 33 ),
        IMPRISON         = ( 1LLU << 34 ),
        YAWN             = ( 1LLU << 35 ),
        MAGICCOAT        = ( 1LLU << 36 ),
        INGRAIN          = ( 1LLU << 37 ),
        HELPINGHAND      = ( 1LLU << 38 ),
        TAUNT            = ( 1LLU << 39 ),
        CHARGE           = ( 1LLU << 40 ),
        FOLLOWME         = ( 1LLU << 41 ),
        TORMENT          = ( 1LLU << 42 ),
        ATTRACT          = ( 1LLU << 43 ),
        ENDURE           = ( 1LLU << 44 ),
        PROTECT          = ( 1LLU << 45 ),
        DESTINYBOND      = ( 1LLU << 46 ),
        CURSE            = ( 1LLU << 47 ),
        NIGHTMARE        = ( 1LLU << 48 ),
        STOCKPILE        = ( 1LLU << 49 ),
        ENCORE           = ( 1LLU << 50 ),
    };

    enum weather : u8 {
        NO_WEATHER     = 0,
        RAIN           = 1,
        HAIL           = 2,
        FOG            = 3,
        SANDSTORM      = 4,
        SUN            = 5,
        HEAVY_RAIN     = 6,
        HEAVY_SUNSHINE = 7,
        HEAVY_WINDS    = 8
    };

    enum pseudoWeather : u8 {
        NO_PSEUDO_WEATHER = 0,
        IONDELUGE         = 1 << 0,
        MAGICROOM         = 1 << 1,
        WONDERROOM        = 1 << 2,
        TRICKROOM         = 1 << 3,
        GRAVITY           = 1 << 4,
        WATERSPORT        = 1 << 5,
        MUDSPORT          = 1 << 6,
        FAIRYLOCK         = 1 << 7
    };

    enum terrain : u8 {
        NO_TERRAIN      = 0,
        PSYCHICTERRAIN  = 1,
        ELECTRICTERRAIN = 2,
        MISTYTERRAIN    = 3,
        GRASSYTERRAIN   = 4,
    };

#define MAX_SIDE_CONDITIONS 15
    enum sideCondition : u16 {
        NO_SIDE_CONDITION = 0,
        CRAFTYSHIELD      = ( 1 << 0 ),
        STICKYWEB         = ( 1 << 1 ),
        MATBLOCK          = ( 1 << 2 ),
        QUICKGUARD        = ( 1 << 3 ),
        WIDEGUARD         = ( 1 << 4 ),
        STEALTHROCK       = ( 1 << 5 ),
        TOXICSPIKES       = ( 1 << 6 ),
        LUCKYCHANT        = ( 1 << 7 ),
        TAILWIND          = ( 1 << 8 ),
        SAFEGUARD         = ( 1 << 9 ),
        SPIKES            = ( 1 << 10 ),
        REFLECT           = ( 1 << 11 ),
        LIGHTSCREEN       = ( 1 << 12 ),
        MIST              = ( 1 << 13 ),
        AURORAVEIL        = ( 1 << 14 ),
    };

    const u8 defaultSideConditionDurations[ MAX_SIDE_CONDITIONS ]
        = {1, 0, 1, 1, 1, 0, 0, 5, 5, 5, 0, 5, 5, 5, 5};
    const u8 maxSideConditionAmount[ MAX_SIDE_CONDITIONS ]
        = {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1};

    enum slotCondition : u8 {
        NO_SLOT_CONDITION = 0,
        WISH              = 1,
        HEALINGWISH       = 2,
        LUNARDANCE        = 3,
    };

    enum battleMode { SINGLE = 0, DOUBLE = 1 };

    struct battlePolicy {
        battleMode  m_mode;
        bool        m_allowMegaEvolution;
        bool        m_distributeEXP;
        bool        m_allowNextPkmnPreview; // Preview of next opp. pkmn
        bool        m_allowCapture;
        weather     m_weather;
        u16         m_aiLevel;
        u16         m_roundLimit;
    };

    constexpr battlePolicy DEFAULT_TRAINER_POLICY = {
        SINGLE, true, true, true, false, NO_WEATHER, 5, 0
    };
    constexpr battlePolicy DEFAULT_WILD_POLICY = {
        SINGLE, true, true, true, true, NO_WEATHER, 0, 0
    };


} // namespace BATTLE
