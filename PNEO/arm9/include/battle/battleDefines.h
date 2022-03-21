/*
    Pokémon neo
    ------------------------------

    file        : battle.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

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

#pragma once
#include <vector>
#include <nds.h>
#include "battle/type.h"

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
        inline boosts invert( ) {
            for( u8 i = 0; i < 8; ++i ) { setBoost( i, -getBoost( i ) ); }
            return *this;
        }

        /*
         * @brief: Returns all negative boosts.
         */
        constexpr boosts negative( ) {
            boosts res = boosts( );
            for( u8 i = 0; i < 8; ++i ) {
                if( getBoost( i ) < 0 ) { res.setBoost( i, -getBoost( i ) ); }
            }
            return res;
        }

        constexpr auto operator<=>( const boosts& ) const = default;
    };

    typedef std::pair<u8, u8> fieldPosition; // (side, slot)

    constexpr u8 MAX_VOLATILE_STATUS = 64;
    enum volatileStatus : u64 {
        VS_NONE             = 0,
        VS_CONFUSION        = ( 1 << 0 ),
        VS_OBSTRUCT         = ( 1 << 1 ),
        VS_PARTIALLYTRAPPED = ( 1 << 2 ), // TODO
        VS_FLINCH           = ( 1 << 3 ),
        VS_OCTOLOCK         = ( 1 << 4 ), // TODO
        VS_TARSHOT          = ( 1 << 5 ), // TODO
        VS_NORETREAT        = ( 1 << 6 ),
        VS_LASERFOCUS       = ( 1 << 7 ),
        VS_SPOTLIGHT        = ( 1 << 8 ), // TODO
        VS_BANEFULBUNKER    = ( 1 << 9 ),
        VS_SMACKDOWN        = ( 1 << 10 ), // TODO
        VS_POWDERED         = ( 1 << 11 ), // TODO
        VS_SPIKYSHIELD      = ( 1 << 12 ),
        VS_KINGSSHIELD      = ( 1 << 13 ),
        VS_ELECTRIFY        = ( 1 << 14 ),
        VS_RAGEPOWDER       = ( 1 << 15 ), // TODO
        VS_TELEKINESIS      = ( 1 << 16 ),
        VS_MAGNETRISE       = ( 1 << 17 ),
        VS_AQUARING         = ( 1 << 18 ),
        VS_GASTROACID       = ( 1 << 19 ), // TODO
        VS_POWERTRICK       = ( 1 << 20 ), // TODO
        VS_HEALBLOCK        = ( 1 << 21 ),
        VS_EMBARGO          = ( 1 << 22 ),
        VS_MIRACLEEYE       = ( 1 << 23 ),
        VS_SUBSTITUTE       = ( 1 << 24 ), // TODO
        VS_BIDE             = ( 1 << 25 ), // TODO
        VS_FOCUSENERGY      = ( 1 << 26 ),
        VS_DEFENSECURL      = ( 1 << 27 ), // TODO
        VS_MINIMIZE         = ( 1 << 28 ), // TODO
        VS_LEECHSEED        = ( 1 << 29 ), // TODO
        VS_DISABLE          = ( 1 << 30 ), // TODO
        VS_FORESIGHT        = ( 1ULL << 31 ),
        VS_SNATCH           = ( 1ULL << 32 ), // TODO
        VS_GRUDGE           = ( 1ULL << 33 ), // TODO
        VS_IMPRISON         = ( 1ULL << 34 ), // TODO
        VS_YAWN             = ( 1ULL << 35 ),
        VS_MAGICCOAT        = ( 1ULL << 36 ), // TODO
        VS_INGRAIN          = ( 1ULL << 37 ),
        VS_HELPINGHAND      = ( 1ULL << 38 ), // TODO
        VS_TAUNT            = ( 1ULL << 39 ), // TODO
        VS_CHARGE           = ( 1ULL << 40 ), // TODO
        VS_FOLLOWME         = ( 1ULL << 41 ), // TODO
        VS_TORMENT          = ( 1ULL << 42 ), // TODO
        VS_ATTRACT          = ( 1ULL << 43 ),
        VS_ENDURE           = ( 1ULL << 44 ), // TODO
        VS_PROTECT          = ( 1ULL << 45 ),
        VS_DESTINYBOND      = ( 1ULL << 46 ), // TODO
        VS_CURSE            = ( 1ULL << 47 ),
        VS_NIGHTMARE        = ( 1ULL << 48 ),
        VS_STOCKPILE        = ( 1ULL << 49 ), // TODO
        VS_ENCORE           = ( 1ULL << 50 ), // TODO
        VS_EXTRATYPE        = ( 1ULL << 51 ),
        VS_MOVECHARGE       = ( 1ULL << 52 ),
        VS_RECHARGE         = ( 1ULL << 53 ),
        VS_ROOST            = ( 1ULL << 54 ),
        VS_BURNUP           = ( 1ULL << 55 ),
        VS_REPLACETYPE      = ( 1ULL << 56 ),
        VS_DIVING           = ( 1ULL << 57 ),
        VS_INAIR            = ( 1ULL << 58 ),
        VS_DIGGING          = ( 1ULL << 59 ),
        VS_INVISIBLE        = ( 1ULL << 60 ),
        VS_BEAKBLAST        = ( 1ULL << 61 ),
        VS_FOCUSPUNCH       = ( 1ULL << 62 ),
        VS_SHELLTRAP        = ( 1ULL << 63 ),
    };

    enum weather : u8 {
        WE_NONE           = 0,
        WE_RAIN           = 1,
        WE_HAIL           = 2,
        WE_FOG            = 3,
        WE_SANDSTORM      = 4,
        WE_SUN            = 5,
        WE_HEAVY_RAIN     = 6,
        WE_HEAVY_SUNSHINE = 7,
        WE_HEAVY_WINDS    = 8
    };

    constexpr u8 MAX_PSEUDO_WEATHER = 8;
    enum pseudoWeather : u8 {
        PW_NONE       = 0,
        PW_IONDELUGE  = 1 << 0,
        PW_MAGICROOM  = 1 << 1,
        PW_WONDERROOM = 1 << 2,
        PW_TRICKROOM  = 1 << 3,
        PW_GRAVITY    = 1 << 4,
        PW_WATERSPORT = 1 << 5,
        PW_MUDSPORT   = 1 << 6,
        PW_FAIRYLOCK  = 1 << 7
    };

    enum terrain : u8 {
        TR_NONE            = 0,
        TR_PSYCHICTERRAIN  = 1,
        TR_ELECTRICTERRAIN = 2,
        TR_MISTYTERRAIN    = 3,
        TR_GRASSYTERRAIN   = 4,
    };

    constexpr u8 MAX_SIDE_CONDITIONS = 15;
    enum sideCondition : u32 {
        SC_NONE         = 0,
        SC_CRAFTYSHIELD = ( 1 << 0 ),
        SC_STICKYWEB    = ( 1 << 1 ),
        SC_MATBLOCK     = ( 1 << 2 ),
        SC_QUICKGUARD   = ( 1 << 3 ),
        SC_WIDEGUARD    = ( 1 << 4 ),
        SC_STEALTHROCK  = ( 1 << 5 ),
        SC_TOXICSPIKES  = ( 1 << 6 ),
        SC_LUCKYCHANT   = ( 1 << 7 ),
        SC_TAILWIND     = ( 1 << 8 ),
        SC_SAFEGUARD    = ( 1 << 9 ),
        SC_SPIKES       = ( 1 << 10 ),
        SC_REFLECT      = ( 1 << 11 ),
        SC_LIGHTSCREEN  = ( 1 << 12 ),
        SC_MIST         = ( 1 << 13 ),
        SC_AURORAVEIL   = ( 1 << 14 ),
    };

    const u8 defaultSideConditionDurations[ MAX_SIDE_CONDITIONS ]
        = { 1, 0, 1, 1, 1, 0, 0, 5, 5, 5, 0, 5, 5, 5, 5 };
    const u8 maxSideConditionAmount[ MAX_SIDE_CONDITIONS ]
        = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1 };

    constexpr u8 MAX_SLOT_CONDITIONS = 3;
    enum slotCondition : u8 {
        SL_NONE        = 0,
        SL_WISH        = 1,
        SL_HEALINGWISH = 2,
        SL_LUNARDANCE  = 3,
    };

    const u8 defaultSlotConditionDurations[ MAX_SLOT_CONDITIONS ] = { 3, 1, 1 };

    enum battleMode {
        BM_SINGLE         = 0,
        BM_DOUBLE         = 1,
        BM_MOCK           = 2,
        BM_MULTI_OPPONENT = 3,
        BM_MULTI_PLAYER   = 4
    };

    constexpr u8 getBattlingPKMNCount( battleMode p_mode ) {
        switch( p_mode ) {
        case BM_DOUBLE:
        case BM_MULTI_OPPONENT:
        case BM_MULTI_PLAYER: return 2;
        default: return 1;
        }
    }

    constexpr u8 getOpposingPkmn( u8 p_slot, battleMode p_mode ) {
        if( getBattlingPKMNCount( p_mode ) == 2 ) { return !p_slot; }
        return p_mode;
    }

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
        = { BM_SINGLE, true, true, true, false, WE_NONE, 5, 0 };
    constexpr battlePolicy DEFAULT_DOUBLE_TRAINER_POLICY
        = { BM_DOUBLE, true, true, true, false, WE_NONE, 5, 0 };
    constexpr battlePolicy DEFAULT_MULTI_TRAINER_POLICY
        = { BM_MULTI_OPPONENT, true, true, true, false, WE_NONE, 5, 0 };
    constexpr battlePolicy DEFAULT_WILD_POLICY
        = { BM_SINGLE, true, true, true, true, WE_NONE, 0, 0 };
    constexpr battlePolicy FACILITY_TRAINER_POLICY
        = { BM_SINGLE, true, false, false, false, WE_NONE, 5, 0 };
    constexpr battlePolicy FACILITY_DOUBLE_TRAINER_POLICY
        = { BM_DOUBLE, true, false, false, false, WE_NONE, 5, 0 };

    enum moveFlags : long long unsigned {
        /** Ignores a target's substitute. */
        MF_AUTHENTIC = ( 1 << 0 ), // TODO
        /** Power is multiplied by 1.5 when used by a Pokemon with the Strong Jaw Ability. */
        MF_BITE = ( 1 << 1 ), // TODO
        /** Has no effect on Pokemon with the Bulletproof Ability. */
        MF_BULLET = ( 1 << 2 ), // TODO
        /** The user is unable to make a move between turns. */
        MF_CHARGE = ( 1 << 3 ),
        /** Makes contact. */
        MF_CONTACT = ( 1 << 4 ),
        /** When used by a Pokemon, other Pokemon with the Dancer Ability can attempt to execute the
           same   move. */
        MF_DANCE = ( 1 << 5 ), // TODO
        /** Thaws the user if executed successfully while the user is frozen. */
        MF_DEFROST = ( 1 << 6 ),
        /** Can target a Pokemon positioned anywhere in a Triple Battle. */
        MF_DISTANCE = ( 1 << 7 ), /* (unused) */
        /** Prevented from being executed or selected during Gravity's effect. */
        MF_GRAVITY = ( 1 << 8 ), // TODO: Add to canSelectMove
        /** Prevented from being executed or selected during Heal Block's effect. */
        MF_HEAL = ( 1 << 9 ), // TODO: Add to canselectmove
        /** Can be copied by Mirror Move. */
        MF_MIRROR = ( 1 << 10 ), // TODO
        /** Recoil halves HP */
        MF_MINDBLOWNRECOIL = ( 1 << 11 ),
        /** Prevented from being executed or selected in a Sky Battle. */
        MF_NONSKY = ( 1 << 12 ), /* (unused) */
        /** Has no effect on Grass-type Pokemon, Pokemon with the Overcoat Ability, and Pokemon
           holding     Safety Goggles. */
        MF_POWDER = ( 1 << 13 ), // TODO
        /** Blocked by Detect, Protect, Spiky Shield, and if not a Status move, King's Shield. */
        MF_PROTECT = ( 1 << 14 ),
        /** Power is multiplied by 1.5 when used by a Pokemon with the Mega Launcher Ability. */
        MF_PULSE = ( 1 << 15 ), // TODO
        /** Power is multiplied by 1.2 when used by a Pokemon with the Iron Fist Ability. */
        MF_PUNCH = ( 1 << 16 ), // TODO
        /** If this move is successful, the user must recharge on the following turn and cannot make
           a      move. */
        MF_RECHARGE = ( 1 << 17 ),
        /** Bounced back to the original user by Magic Coat or the Magic Bounce Ability. */
        MF_REFLECTABLE = ( 1 << 18 ), // TODO
        /** Can be stolen from the original user and instead used by another Pokemon using Snatch.
         */
        MF_SNATCH = ( 1 << 19 ), // TODO
        /** Has no effect on Pokemon with the Soundproof Ability. */
        MF_SOUND = ( 1 << 20 ),
        /** Forces the pokemon to use the move for another 1-2 turns. */
        MF_LOCKEDMOVE = ( 1 << 21 ),
        /** Maintains the rage counter */
        MF_RAGE = ( 1 << 22 ), // TODO
        /** Roost */
        MF_ROOST = ( 1 << 23 ),
        /** Uproar */
        MF_UPROAR = ( 1 << 24 ), // TODO
        /** Self-Switch: User switches after successful use */
        MF_SELFSWITCH = ( 1 << 25 ),
        /** Use source defensive stats as offensive stats */
        MF_DEFASOFF = ( 1 << 26 ),
        /** User is damaged if the attack misses */
        MF_CRASHDAMAGE = ( 1 << 27 ),
        /** OHKO move */
        MF_OHKO = ( 1 << 28 ),
        /** PKMN self-destructs */
        MF_SELFDESTRUCT = ( 1 << 29 ),
        /** PKMN self-destructs */
        MF_SELFDESTRUCTHIT = ( 1 << 30 ),
        /** move cannot be sketched */
        MF_NOSKETCH = ( 1ULL << 31 ), // TODO
        /** move pp cannot be increased */
        MF_NOPPBOOST = ( 1ULL << 32 ),
        /** move can be used while asleep */
        MF_SLEEPUSABLE = ( 1ULL << 33 ),
        /** target cannot faint due to move */
        MF_NOFAINT = ( 1ULL << 34 ),
        /** breaks protect */
        MF_BREAKSPROTECT = ( 1ULL << 35 ),
        /** ignores type immunities */
        MF_IGNOREIMMUNITY = ( 1ULL << 36 ),
        /** ignores ground type immunity */
        MF_IGNOREIMMUNITYGROUND = ( 1ULL << 37 ),
        /** ignores abilities */
        MF_IGNOREABILITY = ( 1ULL << 38 ),
        /** ignores defense boosts */
        MF_IGNOREDEFS = ( 1ULL << 39 ), // TODO
        /** ignores evasion boosts */
        MF_IGNOREEVASION = ( 1ULL << 40 ),
        /** Defrosts the target */
        MF_DEFROSTTARGET = ( 1ULL << 41 ),
        /** Forces the target to switch out */
        MF_FORCESWITCH = ( 1ULL << 42 ),
        /** Will always land a critical hit */
        MF_WILLCRIT = ( 1ULL << 43 ),
        /** Move uses atk/satk of the target */
        MF_TARGETOFFENSIVES = ( 1ULL << 44 ), // TODO
        /** Move hits at a later time */
        MF_FUTUREMOVE = ( 1ULL << 45 ), // TODO
        /** Beton pass */
        MF_BATONPASS = ( 1ULL << 46 ),
        /** OHKO move (useless on ice-type pkmn) */
        MF_OHKOICE = ( 1ULL << 47 ),
    };

    enum moveHitTypes : u8 { MH_NONE = 0, MH_PHYSICAL = 1, MH_SPECIAL = 2, MH_STATUS = 3 };

    enum target : u8 {
        TG_NONE              = 0,
        TG_ANY               = 1,
        TG_ANY_FOE           = 3,
        TG_ALLY              = 2,
        TG_SELF              = 5,
        TG_ALLY_OR_SELF      = 4,
        TG_RANDOM            = 6, // single-target, automatic
        TG_ALL_ALLIES        = 13,
        TG_ALL_FOES          = 8, // spread
        TG_ALL_FOES_AND_ALLY = 7,
        TG_ALLY_SIDE         = 9,
        TG_FOE_SIDE          = 10,
        TG_FIELD             = 11, // field
        TG_SCRIPTED          = 14,
        TG_ALLY_TEAM         = 15,
    };

    struct moveData {
        type        m_type : 8        = TYPE_UNKNOWN;      // ???
        contestType m_contestType : 8 = CONTEST_TYPE_NONE; // Clever, Smart, ...
        u8          m_basePower       = 0;
        u8          m_pp              = 1;

        moveHitTypes m_category : 8          = (moveHitTypes) 0;
        moveHitTypes m_defensiveCategory : 8 = (moveHitTypes) 0; // category used for defending pkmn
        u8           m_accuracy              = 0;                // 255: always hit
        s8           m_priority              = 0;

        sideCondition m_sideCondition : 32 = SC_NONE; // side introduced by the move (reflect, etc)

        weather       m_weather : 8       = WE_NONE; // weather introduced by the move
        pseudoWeather m_pseudoWeather : 8 = PW_NONE; // pseudo weather introduced by the move
        terrain       m_terrain : 8       = TR_NONE; // terrain introduced by the move
        u8            m_status            = 0;

        slotCondition m_slotCondition : 8 = SL_NONE; // stuff introduced on the slot (wish, etc)
        u8            m_fixedDamage       = 0;
        target        m_target : 8        = TG_NONE;
        target m_pressureTarget : 8       = TG_NONE; // restrictions are computed based on different
                                                     // target than resulting effect

        u8 m_heal     = 0; // as m_heal / 240
        u8 m_recoil   = 0; // as dealt damage * m_recoil / 240
        u8 m_drain    = 0; // as dealt damage * m_recoil / 240
        u8 m_multiHit = 0; // as ( min << 4 ) | max

        u8 m_critRatio       = 1;
        u8 m_secondaryChance = 0; // chance that the secondary effect triggers
        u8 m_secondaryStatus = 0;
        u8 m_unused          = 0;

        volatileStatus m_volatileStatus          = (volatileStatus) 0; // confusion, etc
        volatileStatus m_secondaryVolatileStatus = (volatileStatus) 0; // confusion, etc

        boosts m_boosts          = boosts( ); // Status ``boosts'' for the target
        boosts m_selfBoosts      = boosts( ); // Status ``boosts'' for the user (if target != user)
        boosts m_secondaryBoosts = boosts( ); // Stat ``boosts'' for the target
        boosts m_secondarySelfBoosts
            = boosts( ); // Stat ``boosts'' for the user (if target != user)

        moveFlags m_flags = (moveFlags) 0;

        constexpr u8 getMultiHitMin( ) const {
            return m_multiHit >> 4;
        }

        constexpr u8 getMultiHitMax( ) const {
            return m_multiHit & ( ( 1 << 4 ) - 1 );
        }
    };

    enum battleMoveType {
        MT_ATTACK,
        MT_SWITCH,
        MT_SWITCH_PURSUIT, // Pursuit used on a switching target
        MT_USE_ITEM,
        MT_NO_OP,           // No operation (e.g. when trying to run in a double battle)
        MT_NO_OP_NO_CANCEL, // No operation, cannot be undone (e.g. player used an item)
        MT_CAPTURE,         // (try to) capture pokemon.
        MT_RUN,
        MT_CANCEL,       // Cancel / go back to previous move selection
        MT_MESSAGE_ITEM, // Extra message for certain items
        MT_MESSAGE_MOVE, // Extra message for certain moves
    };

    struct battleMoveSelection {
        battleMoveType m_type;
        u16            m_param; // move id for attack/ m attack; target pkmn
                                // for swtich; tg item
        fieldPosition m_target; // If the move has a selectable, single target, it is stored here
        fieldPosition m_user;
        bool          m_megaEvolve;
        moveData      m_moveData;
    };

    constexpr battleMoveSelection NO_OP_SELECTION
        = { MT_NO_OP, 0, { 255, 255 }, { 255, 255 }, false, moveData( ) };

    struct battleMove {
        battleMoveType             m_type;
        u16                        m_param;
        std::vector<fieldPosition> m_target; // List of all pkmn targeted (empty if field or side)
        fieldPosition              m_user;
        s8                         m_priority;
        s16                        m_userSpeed;
        u8                         m_pertubation; // random number to break speed ties
        moveData                   m_moveData;
        bool                       m_megaEvolve;

        std::strong_ordering operator<=>( const battleMove& p_other ) {
            if( auto cmp = p_other.m_priority <=> this->m_priority; cmp != 0 ) return cmp;
            if( auto cmp = p_other.m_userSpeed <=> this->m_userSpeed; cmp != 0 ) return cmp;
            return this->m_pertubation <=> p_other.m_pertubation;
        }
    };
} // namespace BATTLE
