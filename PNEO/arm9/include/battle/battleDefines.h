/*
   Pokémon neo
   ------------------------------

   file        : battle.h
   author      : Philip Wellnitz
   description : Header file. Consult the corresponding source file for details.

   Copyright (C) 2012 - 2026
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
    constexpr u8 HP              = 0;
    constexpr u8 ATK             = 1;
    constexpr u8 DEF             = 2;
    constexpr u8 SATK            = 3;
    constexpr u8 SDEF            = 4;
    constexpr u8 SPEED           = 5;
    constexpr u8 EVASION         = 6;
    constexpr u8 ACCURACY        = 7;
    constexpr u8 NUM_BOOSTS      = 8;
    constexpr u8 BOOST_MAX_VALUE = 7;

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
            for( u8 i = 0; i < NUM_BOOSTS; ++i ) { setBoost( i, 0 ); }
        }

        constexpr void setBoost( u8 p_stat, s8 p_val ) {
            // shift boost value to be always positive
            p_val += BOOST_MAX_VALUE;

            m_boosts &= ( 0xFFFFFFFF - ( 0xF << ( 4 * p_stat ) ) );
            m_boosts |= ( p_val << ( 4 * p_stat ) );
        }
        constexpr s8 getBoost( u8 p_stat ) const {
            return ( ( m_boosts >> ( 4 * p_stat ) ) & 0xF ) - BOOST_MAX_VALUE;
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
         * @param p_allowOverMax: allow boosts to reach level 7.
         * @returns: the change in boosts.
         */
        inline boosts addBoosts( boosts p_other, bool p_allowOverMax = false ) {
            const s8 maxBoost = p_allowOverMax ? s8( BOOST_MAX_VALUE ) : s8( BOOST_MAX_VALUE - 1 );
            boosts   res      = boosts( );
            for( u8 i = 0; i < NUM_BOOSTS; ++i ) {
                s8 old = getBoost( i );
                setBoost( i, std::min( maxBoost, std::max( s8( -maxBoost ),
                                                           s8( old + p_other.getBoost( i ) ) ) ) );
                res.setBoost( i, getBoost( i ) - old );
            }
            return res;
        }

        /*
         * @brief: Inverts all boosts.
         */
        inline boosts invert( ) {
            for( u8 i = 0; i < NUM_BOOSTS; ++i ) { setBoost( i, -getBoost( i ) ); }
            return *this;
        }

        /*
         * @brief: Returns all negative boosts.
         */
        constexpr boosts negative( ) {
            boosts res = boosts( );
            for( u8 i = 0; i < NUM_BOOSTS; ++i ) {
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
        VS_PARTIALLYTRAPPED = ( 1 << 2 ),
        VS_FLINCH           = ( 1 << 3 ),
        VS_OCTOLOCK         = ( 1 << 4 ), // TODO
        VS_TARSHOT          = ( 1 << 5 ),
        VS_NORETREAT        = ( 1 << 6 ),
        VS_LASERFOCUS       = ( 1 << 7 ),
        VS_SPOTLIGHT        = ( 1 << 8 ),
        VS_BANEFULBUNKER    = ( 1 << 9 ),
        VS_SMACKDOWN        = ( 1 << 10 ),
        VS_POWDERED         = ( 1 << 11 ),
        VS_SPIKYSHIELD      = ( 1 << 12 ),
        VS_KINGSSHIELD      = ( 1 << 13 ),
        VS_ELECTRIFY        = ( 1 << 14 ),
        VS_RAGEPOWDER       = ( 1 << 15 ),
        VS_TELEKINESIS      = ( 1 << 16 ),
        VS_MAGNETRISE       = ( 1 << 17 ),
        VS_AQUARING         = ( 1 << 18 ),
        VS_GASTROACID       = ( 1 << 19 ),
        VS_POWERTRICK       = ( 1 << 20 ),
        VS_HEALBLOCK        = ( 1 << 21 ),
        VS_EMBARGO          = ( 1 << 22 ),
        VS_MIRACLEEYE       = ( 1 << 23 ),
        VS_SUBSTITUTE       = ( 1 << 24 ),
        VS_BIDE             = ( 1 << 25 ),
        VS_FOCUSENERGY      = ( 1 << 26 ),
        VS_DEFENSECURL      = ( 1 << 27 ),
        VS_MINIMIZE         = ( 1 << 28 ),
        VS_LEECHSEED        = ( 1 << 29 ),
        VS_DISABLE          = ( 1 << 30 ),
        VS_FORESIGHT        = ( 1ULL << 31 ),
        VS_SNATCH           = ( 1ULL << 32 ),
        VS_GRUDGE           = ( 1ULL << 33 ),
        VS_IMPRISON         = ( 1ULL << 34 ),
        VS_YAWN             = ( 1ULL << 35 ),
        VS_MAGICCOAT        = ( 1ULL << 36 ),
        VS_INGRAIN          = ( 1ULL << 37 ),
        VS_HELPINGHAND      = ( 1ULL << 38 ),
        VS_TAUNT            = ( 1ULL << 39 ),
        VS_CHARGE           = ( 1ULL << 40 ),
        VS_FOLLOWME         = ( 1ULL << 41 ),
        VS_TORMENT          = ( 1ULL << 42 ),
        VS_ATTRACT          = ( 1ULL << 43 ),
        VS_ENDURE           = ( 1ULL << 44 ),
        VS_PROTECT          = ( 1ULL << 45 ),
        VS_DESTINYBOND      = ( 1ULL << 46 ),
        VS_CURSE            = ( 1ULL << 47 ),
        VS_NIGHTMARE        = ( 1ULL << 48 ),
        VS_STOCKPILE        = ( 1ULL << 49 ), // TODO
        VS_ENCORE           = ( 1ULL << 50 ),
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
        = { 1, 0, 1, 1, 1, 0, 0, 5, 4, 5, 0, 5, 5, 5, 5 };
    const u8 maxSideConditionAmount[ MAX_SIDE_CONDITIONS ]
        = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1 };

    constexpr u8 MAX_SLOT_CONDITIONS = 4;
    enum slotCondition : u8 {
        SL_NONE        = 0,
        SL_WISH        = ( 1 << 0 ),
        SL_HEALINGWISH = ( 1 << 1 ),
        SL_LUNARDANCE  = ( 1 << 2 ),
        SL_FUTURESIGHT = ( 1 << 3 ),
    };

    const u8 defaultSlotConditionDurations[ MAX_SLOT_CONDITIONS ] = { 2, 0, 0, 2 };

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
        return p_slot;
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
        = { BM_SINGLE, true, false, false, false, WE_NONE, 7, 0 };
    constexpr battlePolicy FACILITY_DOUBLE_TRAINER_POLICY
        = { BM_DOUBLE, true, false, false, false, WE_NONE, 7, 0 };

    enum moveFlags : long long unsigned {
        MF_AUTHENTIC            = ( 1 << 0 ), // TODO
        MF_BITE                 = ( 1 << 1 ), // TODO
        MF_BULLET               = ( 1 << 2 ), // TODO
        MF_CHARGE               = ( 1 << 3 ),
        MF_CONTACT              = ( 1 << 4 ),
        MF_DANCE                = ( 1 << 5 ), // TODO
        MF_DEFROST              = ( 1 << 6 ),
        MF_DISTANCE             = ( 1 << 7 ), /* (unused) */
        MF_GRAVITY              = ( 1 << 8 ),
        MF_HEAL                 = ( 1 << 9 ),  // TODO: Add to canselectmove
        MF_MIRROR               = ( 1 << 10 ), // TODO
        MF_MINDBLOWNRECOIL      = ( 1 << 11 ),
        MF_NONSKY               = ( 1 << 12 ), /* (unused) */
        MF_POWDER               = ( 1 << 13 ),
        MF_PROTECT              = ( 1 << 14 ),
        MF_PULSE                = ( 1 << 15 ), // TODO
        MF_PUNCH                = ( 1 << 16 ), // TODO
        MF_RECHARGE             = ( 1 << 17 ),
        MF_REFLECTABLE          = ( 1 << 18 ),
        MF_SNATCH               = ( 1 << 19 ),
        MF_SOUND                = ( 1 << 20 ),
        MF_LOCKEDMOVE           = ( 1 << 21 ),
        MF_RAGE                 = ( 1 << 22 ), // TODO
        MF_ROOST                = ( 1 << 23 ),
        MF_UPROAR               = ( 1 << 24 ), // TODO
        MF_SELFSWITCH           = ( 1 << 25 ),
        MF_DEFASOFF             = ( 1 << 26 ),
        MF_CRASHDAMAGE          = ( 1 << 27 ),
        MF_OHKO                 = ( 1 << 28 ),
        MF_SELFDESTRUCT         = ( 1 << 29 ),
        MF_SELFDESTRUCTHIT      = ( 1 << 30 ),
        MF_NOSKETCH             = ( 1ULL << 31 ), // TODO
        MF_NOPPBOOST            = ( 1ULL << 32 ),
        MF_SLEEPUSABLE          = ( 1ULL << 33 ),
        MF_NOFAINT              = ( 1ULL << 34 ),
        MF_BREAKSPROTECT        = ( 1ULL << 35 ),
        MF_IGNOREIMMUNITY       = ( 1ULL << 36 ),
        MF_IGNOREIMMUNITYGROUND = ( 1ULL << 37 ),
        MF_IGNOREABILITY        = ( 1ULL << 38 ),
        MF_IGNOREDEFS           = ( 1ULL << 39 ), // TODO
        MF_IGNOREEVASION        = ( 1ULL << 40 ),
        MF_DEFROSTTARGET        = ( 1ULL << 41 ),
        MF_FORCESWITCH          = ( 1ULL << 42 ),
        MF_WILLCRIT             = ( 1ULL << 43 ),
        MF_TARGETOFFENSIVES     = ( 1ULL << 44 ), // TODO
        MF_FUTUREMOVE           = ( 1ULL << 45 ), // TODO
        MF_BATONPASS            = ( 1ULL << 46 ),
        MF_OHKOICE              = ( 1ULL << 47 ),
        MF_SLICE                = ( 1ULL << 48 ),
        MF_WIND                 = ( 1ULL << 49 ),
    };

    constexpr u8 NUM_MOVE_HIT_TYPES = 3;
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

        u32 m_reserved = 0;

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
        u16            m_param;  // move id for attack/ m attack; target pkmn
                                 // for swtich; tg item
        fieldPosition  m_target; // If the move has a selectable, single target, it is stored here
        fieldPosition  m_user;
        bool           m_megaEvolve;
        moveData       m_moveData;
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
        u8                         m_actsAfter     = 0;
        u16                        m_powerOverride = 0;
        bool                       m_bounced       = false;
        bool                       m_fromSnatch    = false;
        bool                       m_dancerCopy    = false;

        std::strong_ordering operator<=>( const battleMove& p_other ) {
            if( auto cmp = p_other.m_priority <=> this->m_priority; cmp != 0 ) return cmp;
            if( auto cmp = p_other.m_userSpeed <=> this->m_userSpeed; cmp != 0 ) return cmp;
            return this->m_pertubation <=> p_other.m_pertubation;
        }
    };
} // namespace BATTLE
