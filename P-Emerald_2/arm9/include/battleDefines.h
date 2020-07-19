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
#include <vector>
#include <nds.h>
#include "type.h"

namespace BATTLE {
    constexpr u8 HP       = 0;
    constexpr u8 ATK      = 1;
    constexpr u8 DEF      = 2;
    constexpr u8 SATK     = 3;
    constexpr u8 SDEF     = 4;
    constexpr u8 SPEED    = 5;
    constexpr u8 EVASION  = 6;
    constexpr u8 ACCURACY = 7;

    constexpr u8 BURN      = 1;
    constexpr u8 PARALYSIS = 2;
    constexpr u8 FROZEN    = 3;
    constexpr u8 SLEEP     = 4;
    constexpr u8 POISON    = 5;
    constexpr u8 TOXIC     = 6;

    struct battleTrainer;

    struct boosts {
        u32 m_boosts;

        constexpr boosts( ) {
            m_boosts = 0;
            for( u8 i = 0; i < 8; ++i ) { setBoost( i, 0 ); }
        }

        constexpr void setBoost( u8 p_stat, s8 p_val ) {
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
                setBoost( i, std::min( s8( 6 ),
                                       std::max( s8( -6 ), s8( old + p_other.getBoost( i ) ) ) ) );
                res.setBoost( i, getBoost( i ) - old );
            }
            return res;
        }

        /*
         * @brief: Inverts all boosts.
         */
        inline void invert( ) {
            for( u8 i = 0; i < 8; ++i ) { setBoost( i, -getBoost( i ) ); }
        }

        constexpr auto operator<=>( const boosts& ) const = default;
    };

    typedef std::pair<u8, u8> fieldPosition; // (side, slot)

    constexpr u8 MAX_VOLATILE_STATUS = 64;
    enum volatileStatus : u64 {
        NONE             = 0,
        CONFUSION        = ( 1 << 0 ),
        OBSTRUCT         = ( 1 << 1 ), // TODO
        PARTIALLYTRAPPED = ( 1 << 2 ), // TODO
        FLINCH           = ( 1 << 3 ),
        OCTOLOCK         = ( 1 << 4 ), // TODO
        TARSHOT          = ( 1 << 5 ), // TODO
        NORETREAT        = ( 1 << 6 ),
        LASERFOCUS       = ( 1 << 7 ),
        SPOTLIGHT        = ( 1 << 8 ),  // TODO
        BANEFULBUNKER    = ( 1 << 9 ),  // TODO
        SMACKDOWN        = ( 1 << 10 ), // TODO
        POWDERED         = ( 1 << 11 ), // TODO
        SPIKYSHIELD      = ( 1 << 12 ), // TODO
        KINGSSHIELD      = ( 1 << 13 ), // TODO
        ELECTRIFY        = ( 1 << 14 ),
        RAGEPOWDER       = ( 1 << 15 ), // TODO
        TELEKINESIS      = ( 1 << 16 ),
        MAGNETRISE       = ( 1 << 17 ),
        AQUARING         = ( 1 << 18 ), // TODO
        GASTROACID       = ( 1 << 19 ), // TODO
        POWERTRICK       = ( 1 << 20 ), // TODO
        HEALBLOCK        = ( 1 << 21 ),
        EMBARGO          = ( 1 << 22 ),
        MIRACLEEYE       = ( 1 << 23 ),
        SUBSTITUTE       = ( 1 << 24 ), // TODO
        BIDE             = ( 1 << 25 ), // TODO
        FOCUSENERGY      = ( 1 << 26 ),
        DEFENSECURL      = ( 1 << 27 ), // TODO
        MINIMIZE         = ( 1 << 28 ), // TODO
        LEECHSEED        = ( 1 << 29 ), // TODO
        DISABLE          = ( 1 << 30 ), // TODO
        FORESIGHT        = ( 1LLU << 31 ),
        SNATCH           = ( 1LLU << 32 ), // TODO
        GRUDGE           = ( 1LLU << 33 ), // TODO
        IMPRISON         = ( 1LLU << 34 ), // TODO
        YAWN             = ( 1LLU << 35 ), // TODO
        MAGICCOAT        = ( 1LLU << 36 ), // TODO
        INGRAIN          = ( 1LLU << 37 ), // TODO
        HELPINGHAND      = ( 1LLU << 38 ), // TODO
        TAUNT            = ( 1LLU << 39 ), // TODO
        CHARGE           = ( 1LLU << 40 ), // TODO
        FOLLOWME         = ( 1LLU << 41 ), // TODO
        TORMENT          = ( 1LLU << 42 ), // TODO
        ATTRACT          = ( 1LLU << 43 ),
        ENDURE           = ( 1LLU << 44 ), // TODO
        PROTECT          = ( 1LLU << 45 ),
        DESTINYBOND      = ( 1LLU << 46 ), // TODO
        CURSE            = ( 1LLU << 47 ), // TODO
        NIGHTMARE        = ( 1LLU << 48 ), // TODO
        STOCKPILE        = ( 1LLU << 49 ), // TODO
        ENCORE           = ( 1LLU << 50 ), // TODO
        EXTRATYPE        = ( 1LLU << 51 ),
        MOVECHARGE       = ( 1LLU << 52 ),
        RECHARGE         = ( 1LLU << 53 ),
        ROOST            = ( 1LLU << 54 ),
        BURNUP           = ( 1LLU << 55 ),
        REPLACETYPE      = ( 1LLU << 56 ),
        DIVING           = ( 1LLU << 57 ),
        INAIR            = ( 1LLU << 58 ),
        DIGGING          = ( 1LLU << 59 ),
        INVISIBLE        = ( 1LLU << 60 ),
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

    constexpr u8 MAX_PSEUDO_WEATHER = 8;
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

    constexpr u8 MAX_SIDE_CONDITIONS = 15;
    enum sideCondition : u32 {
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
        = { 1, 0, 1, 1, 1, 0, 0, 5, 5, 5, 0, 5, 5, 5, 5 };
    const u8 maxSideConditionAmount[ MAX_SIDE_CONDITIONS ]
        = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1 };

    constexpr u8 MAX_SLOT_CONDITIONS = 3;
    enum slotCondition : u8 {
        NO_SLOT_CONDITION = 0,
        WISH              = 1,
        HEALINGWISH       = 2,
        LUNARDANCE        = 3,
    };

    const u8 defaultSlotConditionDurations[ MAX_SLOT_CONDITIONS ] = { 3, 1, 1 };

    enum battleMode { SINGLE = 0, DOUBLE = 1 };

    struct battlePolicy {
        battleMode m_mode;
        bool       m_allowMegaEvolution;
        bool       m_distributeEXP;
        bool       m_allowNextPkmnPreview; // Preview of next opp. pkmn
        bool       m_allowCapture;
        weather    m_weather;
        u16        m_aiLevel;
        u16        m_roundLimit;
    };

    constexpr battlePolicy DEFAULT_TRAINER_POLICY
        = { SINGLE, true, true, true, false, NO_WEATHER, 5, 0 };
    constexpr battlePolicy DEFAULT_WILD_POLICY
        = { SINGLE, true, true, true, true, NO_WEATHER, 0, 0 };

} // namespace BATTLE

namespace MOVE {
    enum moveFlags : long long unsigned {
        /** Ignores a target's substitute. */
        AUTHENTIC = ( 1 << 0 ), // TODO
        /** Power is multiplied by 1.5 when used by a Pokemon with the Strong Jaw Ability. */
        BITE = ( 1 << 1 ), // TODO
        /** Has no effect on Pokemon with the Bulletproof Ability. */
        BULLET = ( 1 << 2 ), // TODO
        /** The user is unable to make a move between turns. */
        CHARGE = ( 1 << 3 ),
        /** Makes contact. */
        CONTACT = ( 1 << 4 ),
        /** When used by a Pokemon, other Pokemon with the Dancer Ability can attempt to execute the
           same   move. */
        DANCE = ( 1 << 5 ), // TODO
        /** Thaws the user if executed successfully while the user is frozen. */
        DEFROST = ( 1 << 6 ),
        /** Can target a Pokemon positioned anywhere in a Triple Battle. */
        DISTANCE = ( 1 << 7 ), /* (unused) */
        /** Prevented from being executed or selected during Gravity's effect. */
        GRAVITY = ( 1 << 8 ), // TODO: Add to canSelectMove
        /** Prevented from being executed or selected during Heal Block's effect. */
        HEAL = ( 1 << 9 ), // TODO: Add to canselectmove
        /** Can be copied by Mirror Move. */
        MIRROR = ( 1 << 10 ), // TODO
        /** Recoil halves HP */
        MINDBLOWNRECOIL = ( 1 << 11 ),
        /** Prevented from being executed or selected in a Sky Battle. */
        NONSKY = ( 1 << 12 ), /* (unused) */
        /** Has no effect on Grass-type Pokemon, Pokemon with the Overcoat Ability, and Pokemon
           holding     Safety Goggles. */
        POWDER = ( 1 << 13 ), // TODO
        /** Blocked by Detect, Protect, Spiky Shield, and if not a Status move, King's Shield. */
        PROTECT = ( 1 << 14 ),
        /** Power is multiplied by 1.5 when used by a Pokemon with the Mega Launcher Ability. */
        PULSE = ( 1 << 15 ), // TODO
        /** Power is multiplied by 1.2 when used by a Pokemon with the Iron Fist Ability. */
        PUNCH = ( 1 << 16 ), // TODO
        /** If this move is successful, the user must recharge on the following turn and cannot make
           a      move. */
        RECHARGE = ( 1 << 17 ),
        /** Bounced back to the original user by Magic Coat or the Magic Bounce Ability. */
        REFLECTABLE = ( 1 << 18 ), // TODO
        /** Can be stolen from the original user and instead used by another Pokemon using Snatch.
         */
        SNATCH = ( 1 << 19 ), // TODO
        /** Has no effect on Pokemon with the Soundproof Ability. */
        SOUND = ( 1 << 20 ), // TODO
        /** Forces the pokemon to use the move for another 1-2 turns. */
        LOCKEDMOVE = ( 1 << 21 ),
        /** Maintains the rage counter */
        RAGE = ( 1 << 22 ), // TODO
        /** Roost */
        ROOST = ( 1 << 23 ),
        /** Uproar */
        UPROAR = ( 1 << 24 ), // TODO
        /** Self-Switch: User switches after successful use */
        SELFSWITCH = ( 1 << 25 ),
        /** Use source defensive stats as offensive stats */
        DEFASOFF = ( 1 << 26 ),
        /** User is damaged if the attack misses */
        CRASHDAMAGE = ( 1 << 27 ), // TODO
        /** OHKO move */
        OHKO = ( 1 << 28 ), // TODO
        /** PKMN self-destructs */
        SELFDESTRUCT = ( 1 << 29 ),
        /** PKMN self-destructs */
        SELFDESTRUCTHIT = ( 1 << 30 ),
        /** move cannot be sketched */
        NOSKETCH = ( 1LLU << 31 ), // TODO
        /** move pp cannot be increased */
        NOPPBOOST = ( 1LLU << 32 ),
        /** move can be used while asleep */
        SLEEPUSABLE = ( 1LLU << 33 ),
        /** target cannot faint due to move */
        NOFAINT = ( 1LLU << 34 ), // TODO
        /** breaks protect */
        BREAKSPROTECT = ( 1LLU << 35 ), // TODO
        /** ignores type immunities */
        IGNOREIMMUNITY = ( 1LLU << 36 ), // TODO
        /** ignores ground type immunity */
        IGNOREIMMUNITYGROUND = ( 1LLU << 37 ), // TODO
        /** ignores abilities */
        IGNOREABILITY = ( 1LLU << 38 ), // TODO
        /** ignores defense boosts */
        IGNOREDEFS = ( 1LLU << 39 ), // TODO
        /** ignores evasion boosts */
        IGNOREEVASION = ( 1LLU << 40 ),
        /** Defrosts the target */
        DEFROSTTARGET = ( 1LLU << 41 ),
        /** Forces the target to switch out */
        FORCESWITCH = ( 1LLU << 42 ),
        /** Will always land a critical hit */
        WILLCRIT = ( 1LLU << 43 ),
        /** Move uses atk/satk of the target */
        TARGETOFFENSIVES = ( 1LLU << 44 ), // TODO
        /** Move hits at a later time */
        FUTUREMOVE = ( 1LLU << 45 ), // TODO
        /** Beton pass */
        BATONPASS = ( 1LLU << 46 ),
        /** OHKO move (useless on ice-type pkmn) */
        OHKOICE = ( 1LLU << 47 ), // TODO
    };

    enum moveHitTypes : u8 { NOOP = 0, PHYSICAL = 1, SPECIAL = 2, STATUS = 3 };

    enum target : u8 {
        NO_TARGET         = 0,
        ANY               = 1,
        ANY_FOE           = 3,
        ALLY              = 2,
        SELF              = 5,
        ALLY_OR_SELF      = 4,
        RANDOM            = 6, // single-target, automatic
        ALL_ALLIES        = 13,
        ALL_FOES          = 8, // spread
        ALL_FOES_AND_ALLY = 7,
        ALLY_SIDE         = 9,
        FOE_SIDE          = 10,
        FIELD             = 11, // field
        SCRIPTED          = 14,
        ALLY_TEAM         = 15,
    };

    struct moveData {
        type        m_type : 8        = UNKNOWN;         // ???
        contestType m_contestType : 8 = NO_CONTEST_TYPE; // Clever, Smart, ...
        u8          m_basePower       = 0;
        u8          m_pp              = 1;

        moveHitTypes m_category : 8          = (moveHitTypes) 0;
        moveHitTypes m_defensiveCategory : 8 = (moveHitTypes) 0; // category used for defending pkmn
        u8           m_accuracy              = 0;                // 255: always hit
        s8           m_priority              = 0;

        BATTLE::sideCondition m_sideCondition : 32
            = BATTLE::NO_SIDE_CONDITION; // side introduced by the move (reflect, etc)

        BATTLE::weather       m_weather : 8 = BATTLE::NO_WEATHER; // weather introduced by the move
        BATTLE::pseudoWeather m_pseudoWeather : 8
            = BATTLE::NO_PSEUDO_WEATHER;                    // pseudo weather introduced by the move
        BATTLE::terrain m_terrain : 8 = BATTLE::NO_TERRAIN; // terrain introduced by the move
        u8              m_status      = 0;

        BATTLE::slotCondition m_slotCondition : 8
            = (BATTLE::slotCondition) 0; // stuff introduced on the slot (wish, etc)
        u8     m_fixedDamage        = 0;
        target m_target : 8         = (target) 0;
        target m_pressureTarget : 8 = (target) 0; // restrictions are computed based on different
                                                  // target than resulting effect

        u8 m_heal     = 0; // as m_heal / 240
        u8 m_recoil   = 0; // as dealt damage * m_recoil / 240
        u8 m_drain    = 0; // as dealt damage * m_recoil / 240
        u8 m_multiHit = 0; // as ( min << 4 ) | max

        u8 m_critRatio       = 1;
        u8 m_secondaryChance = 0; // chance that the secondary effect triggers
        u8 m_secondaryStatus = 0;
        u8 m_unused          = 0;

        BATTLE::volatileStatus m_volatileStatus = (BATTLE::volatileStatus) 0; // confusion, etc
        BATTLE::volatileStatus m_secondaryVolatileStatus
            = (BATTLE::volatileStatus) 0; // confusion, etc

        BATTLE::boosts m_boosts = BATTLE::boosts( ); // Status ``boosts'' for the target
        BATTLE::boosts m_selfBoosts
            = BATTLE::boosts( ); // Status ``boosts'' for the user (if target != user)
        BATTLE::boosts m_secondaryBoosts = BATTLE::boosts( ); // Stat ``boosts'' for the target
        BATTLE::boosts m_secondarySelfBoosts
            = BATTLE::boosts( ); // Stat ``boosts'' for the user (if target != user)

        moveFlags m_flags = (moveFlags) 0;

        constexpr u8 getMultiHitMin( ) const {
            return m_multiHit >> 4;
        }

        constexpr u8 getMultiHitMax( ) const {
            return m_multiHit & ( ( 1 << 4 ) - 1 );
        }
    };
} // namespace MOVE

namespace BATTLE {
    enum battleMoveType {
        ATTACK,
        SWITCH,
        SWITCH_PURSUIT, // Pursuit used on a switching target
        USE_ITEM,
        NO_OP,           // No operation (e.g. when trying to run in a double battle)
        NO_OP_NO_CANCEL, // No operation, cannot be undone (e.g. player used an item)
        CAPTURE,         // (try to) capture pokemon.
        RUN,
        CANCEL,       // Cancel / go back to previous move selection
        MESSAGE_ITEM, // Extra message for certain items
        MESSAGE_MOVE, // Extra message for certain moves
    };

    struct battleMoveSelection {
        battleMoveType m_type;
        u16            m_param;  // move id for attack/ m attack; target pkmn
                                 // for swtich; tg item
        fieldPosition  m_target; // If the move has a selectable, single target, it is stored here
        fieldPosition  m_user;
        bool           m_megaEvolve;
        MOVE::moveData m_moveData;
    };

    constexpr battleMoveSelection NO_OP_SELECTION
        = { NO_OP, 0, { 255, 255 }, { 255, 255 }, false, MOVE::moveData( ) };

    struct battleMove {
        battleMoveType             m_type;
        u16                        m_param;
        std::vector<fieldPosition> m_target; // List of all pkmn targeted (empty if field or side)
        fieldPosition              m_user;
        s8                         m_priority;
        s16                        m_userSpeed;
        u8                         m_pertubation; // random number to break speed ties
        MOVE::moveData             m_moveData;
        bool                       m_megaEvolve;

        std::strong_ordering operator<=>( const battleMove& p_other ) {
            if( auto cmp = p_other.m_priority <=> this->m_priority; cmp != 0 ) return cmp;
            if( auto cmp = p_other.m_userSpeed <=> this->m_userSpeed; cmp != 0 ) return cmp;
            return this->m_pertubation <=> p_other.m_pertubation;
        }
    };
} // namespace BATTLE
