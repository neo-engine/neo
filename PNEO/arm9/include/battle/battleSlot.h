/*
   Pokémon neo
   ------------------------------

   file        : battleSlot.h
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
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <nds.h>

#include "battle/battleDefines.h"
#include "battle/battleUI.h"
#include "battle/move.h"
#include "fs/data.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "io/strings.h"
#include "pokemon.h"

namespace BATTLE {
    /*
     * @brief: A slot on the field.
     */
    class slot {
      public:
        enum status {
            EMPTY = 0,
            NORMAL,
            FAINTED,
            RECALLED, // player/ AI recalled their pokemon
        };

      private:
        status              _status;
        slotCondition       _slotCondition = static_cast<slotCondition>( 0 );
        u8                  _slotConditionCounter[ MAX_SLOT_CONDITIONS ] = { 0 };
        u16                 _slotConditionAmount[ MAX_SLOT_CONDITIONS ]  = { 0 };
        std::string         _slotConditionWisher;
        u16                 _slotConditionSrc = 0xFFFF;
        u16                 _substituteHp     = 0;
        std::string         _trapWisher;
        u16                 _trapMove       = 0;
        u16                 _trapSpecies    = 0;
        u8                  _trapSide       = 0xFF;
        u8                  _trapSlot       = 0xFF;
        u8                  _trapTimer      = 0;
        u8                  _perishTimer    = 0;
        u8                  _moveActions    = 0;
        u16                 _stallCounter   = 0;
        u8                  _stallWindow    = 0;
        u16                 _counterDamage  = 0;
        u8                  _counterFrom    = 0xFF;
        u16                 _mirrorDamage   = 0;
        u8                  _mirrorFrom     = 0xFF;
        u8                  _leechSource    = 0xFF;
        u16                 _bideDamage     = 0;
        u8                  _bideFrom       = 0xFF;
        u8                  _autotomizeCuts = 0;
        pokemon*            _pokemon;
        pokemon             _transformedPkmn;
        bool                _isTransformed;
        const pokemon*      _disguise;
        u8                  _volatileStatusCounter[ MAX_VOLATILE_STATUS ];
        u8                  _volatileStatusAmount[ MAX_VOLATILE_STATUS ];
        boosts              _boosts;
        u16                 _toxicCount;
        u16                 _turnsInPlay;
        battleMoveSelection _lockedMove; // move that a pkmn is forced to execute (no op if hib)
        u8                  _lockedMoveTurns; // remaining turns the pkmn is locked into _lockedMove
        battleMove          _lastMove;        // Last used moves
        bool                _moveActedThisTurn = false;
        u8                  _consecutiveMoveCount;
        u16                 _disabledMove; // move that was disabled
        u16                 _encoreMove = 0;
        pkmnData            _pkmnData;
        std::vector<type>   _altTypes;
        type                _extraType{ };
        u16                 _usedItem = 0; // (held) item the pkmn used

      public:
        slot( ) {
            reset( );
            _pokemon  = nullptr;
            _pkmnData = pkmnData( );
        }

        constexpr u16 getAndIncreaseToxicCount( ) {
            auto res = _toxicCount;
            if( ++_toxicCount >= 15 ) { _toxicCount = 15; }
            return res;
        }

        constexpr u16 getTurnsInPlay( ) {
            return _turnsInPlay;
        }

        /*
         * Ages the slot by one turn, processes all changes
         */
        void age( battleUI* p_ui );

        constexpr pkmnData getPokemonData( ) const {
            return _pkmnData;
        }

        constexpr u8 getConsecutiveMoveCount( ) const {
            return _consecutiveMoveCount;
        }

        constexpr status getSlotStatus( ) const {
            return _status;
        }

        /*
         * @brief: Resets the slot
         */
        inline void reset( ) {
            _usedItem             = 0;
            _turnsInPlay          = 0;
            _toxicCount           = 0;
            _lockedMove           = NO_OP_SELECTION;
            _lockedMoveTurns      = 0;
            _consecutiveMoveCount = 0;
            _status               = status( 0 );
            _isTransformed        = false;
            _boosts               = boosts( );
            _lastMove             = battleMove( );
            _moveActedThisTurn    = false;
            _disabledMove         = 0;
            _encoreMove           = 0;
            _perishTimer          = 0;
            _moveActions          = 0;
            _stallCounter         = 0;
            _stallWindow          = 0;
            _counterDamage        = 0;
            _counterFrom          = 0xFF;

            _mirrorDamage   = 0;
            _mirrorFrom     = 0xFF;
            _leechSource    = 0xFF;
            _bideDamage     = 0;
            _bideFrom       = 0xFF;
            _autotomizeCuts = 0;
            _disguise       = nullptr;
            _altTypes.clear( );
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );
            std::memset( _volatileStatusCounter, 0, sizeof( _volatileStatusCounter ) );
            std::memset( _volatileStatusAmount, 0, sizeof( _volatileStatusAmount ) );
            clearPartialTrap( );
            _substituteHp = 0;
        }

        /*
         * @brief: Sets the type of the specified pkmn.
         */
        inline void setType( battleUI* p_ui, type p_type ) {
            _altTypes = { p_type };
            addVolatileStatus( p_ui, VS_REPLACETYPE, -1 );
        }

        /*
         * @brief: Sets the extra type of the specified pkmn.
         */
        inline void setExtraType( battleUI* p_ui, type p_type ) {
            _extraType = p_type;
            addVolatileStatus( p_ui, VS_EXTRATYPE, -1 );
        }

        /*
         * @brief: Returns a list of all types the pkmn currently has. May be empty
         */
        inline std::vector<type> getTypes( ) const {
            std::vector<type> res = std::vector<type>( );

            for( u8 i = 0; i < NUM_TYPES; ++i ) {
                if( hasType( type( i ) ) ) { res.push_back( type( i ) ); }
            }

            return res;
        }

        constexpr type getExtraType( ) const {
            return _extraType;
        }

        inline std::vector<type> getAltTypes( ) const {
            return _altTypes;
        }

        /*
         * @brief: Checks whether the pkmn currently has the specified type.
         */
        bool hasType( type p_type ) const;

        /*
         * @brief: Sets the move the specified pkmn used last.
         */
        inline void setLastUsedMove( battleMove p_move ) {
            if( _lastMove.m_param == p_move.m_param ) {
                _consecutiveMoveCount++;
            } else {
                _consecutiveMoveCount = 0;
            }
            _lastMove = p_move;
        }

        inline battleMove getLastUsedMove( ) const {
            return _lastMove;
        }

        inline void markMoveActed( ) {
            _moveActedThisTurn = true;
        }

        inline bool takeMoveActed( ) {
            bool acted         = _moveActedThisTurn;
            _moveActedThisTurn = false;
            return acted;
        }

        /*
         * @brief: Recalls a non-FAINTED pokemon.
         */
        void recallPokemon( bool p_keepVolatileStatus = false );

        /*
         * @brief: Adds the specified amount of damage to the pkmn.
         */
        void damagePokemon( u16 p_damage );

        /*
         * @brief: Heals the specified amount of damage from the pkmn.
         */
        constexpr void healPokemon( u16 p_heal ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

            pkmn->m_stats.m_curHP
                = std::min( pkmn->m_stats.m_maxHP, u16( pkmn->m_stats.m_curHP + p_heal ) );
            if( _isTransformed ) { _pokemon->m_stats.m_curHP = pkmn->m_stats.m_curHP; }
        }

        constexpr void healFully( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

            pkmn->m_stats.m_curHP = pkmn->m_stats.m_maxHP;
            if( _isTransformed ) { _pokemon->m_stats.m_curHP = pkmn->m_stats.m_maxHP; }
        }

        void restoreAllMovePP( );

        bool hasMovePPDeficit( );

        /*
         * @brief: true iff the move sits in one of the pkmn's four slots.
         */
        bool hasMove( u16 p_moveId ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

            for( u8 i = 0; i < 4; ++i ) {
                if( pkmn->m_boxdata.m_moves[ i ] == p_moveId ) { return true; }
            }
            return false;
        }

        /*
         * @brief: Faints the pokemon. Deals necessary damage first.
         */
        void faintPokemon( );

        constexpr boosts addBoosts( boosts p_boosts, bool p_allowAbilities = true,
                                    bool p_allowOverMax = false ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr ) { return boosts( ); }

            if( p_allowAbilities && !abilitiesNullified( ) && pkmn->getAbility( ) == A_CONTRARY ) {
                p_boosts.invert( );
            }

            return _boosts.addBoosts( p_boosts, p_allowOverMax );
        }
        constexpr bool resetBoosts( ) {
            _boosts = boosts( );
            return true;
        }
        constexpr boosts getBoosts( ) const {
            return _boosts;
        }

        bool setStatusCondition( u8 p_status, u8 p_duration = 255, bool p_force = false );

        /*
         * @brief: Removes any status condition a pkmn may have
         */
        constexpr bool removeStatusCondition( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

            pkmn->m_statusint = 0;
            if( _isTransformed ) { _pokemon->m_statusint = 0; }

            return true;
        }

        /*
         * @brief: Returns the if the pkmn has the specified status condition.
         */
        constexpr u8 hasStatusCondition( u8 p_status ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }

            auto& pkmn = _isTransformed ? _transformedPkmn : *_pokemon;

            if( !pkmn.m_statusint ) [[likely]] { return false; }

            switch( p_status ) {
            case SLEEP: return pkmn.m_status.m_isAsleep;
            case BURN: return pkmn.m_status.m_isBurned;
            case FROZEN: return pkmn.m_status.m_isFrozen;
            case PARALYSIS: return pkmn.m_status.m_isParalyzed;
            case POISON: return pkmn.m_status.m_isPoisoned;
            case TOXIC: return pkmn.m_status.m_isBadlyPoisoned;
            default: return false;
            }
        }

        /*
         * @brief: Checks whether the pkmn has the specified ability.
         */
        constexpr bool hasAbility( u16 p_abilityId ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return false; }
            if( abilitiesNullified( ) ) [[unlikely]] { return false; }
            auto& pkmn = _isTransformed ? _transformedPkmn : *_pokemon;
            return pkmn.getAbility( ) == p_abilityId;
        }

        bool abilitiesNullified( ) const {
            return _volatileStatusCounter[ 19 ] != 0; // VS_GASTROACID
        }

        /*
         * @brief: Deduces the specified number of PP from the first move of the specified
         * pkmn that has the specified p_moveId.
         */
        constexpr void deducePP( u16 p_moveId, u8 p_amount ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return; }

            for( u8 i = 0; i < 4; ++i ) {
                if( pkmn->getMove( i ) != p_moveId ) { continue; }
                if( pkmn->m_boxdata.m_curPP[ i ] > p_amount ) {
                    pkmn->m_boxdata.m_curPP[ i ] -= p_amount;
                } else {
                    pkmn->m_boxdata.m_curPP[ i ] = 0;
                }
                break;
            }
        }

        bool addVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus,
                                u8 p_duration = 255 );

        constexpr bool restackVolatile( u8 p_index ) {
            if( !_volatileStatusCounter[ p_index ] ) { return false; }
            if( _volatileStatusAmount[ p_index ] < 255 ) { _volatileStatusAmount[ p_index ]++; }
            _volatileStatusCounter[ p_index ] = 1;
            return true;
        }

        constexpr bool removeVolatileStatus( battleUI* p_ui, volatileStatus p_volatileStatus ) {
            bool change = false;
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( ( p_volatileStatus & ( 1LLU << i ) ) && _volatileStatusCounter[ i ] ) {
                    change                      = true;
                    _volatileStatusAmount[ i ]  = 0;
                    _volatileStatusCounter[ i ] = 0;
#ifdef DESQUID_MORE
                    p_ui->log( std::string( "Remove volatile status condition " )
                               + std::to_string( 1LLU << i ) );
#else
                    (void) p_ui;
#endif
                }
            }
            return change;
        }

        constexpr volatileStatus getVolatileStatus( ) const {
            volatileStatus res = volatileStatus( 0 );
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( _volatileStatusAmount[ i ] ) { res = volatileStatus( res | ( 1LLU << i ) ); }
            }
            return res;
        }

        constexpr u16 getSubstituteHp( ) const {
            return _substituteHp;
        }
        constexpr void setSubstituteHp( u16 p_hp ) {
            _substituteHp = p_hp;
        }

        constexpr u8 getVolatileStatusAmount( volatileStatus p_volatileStatus ) const {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1LLU << i ) ) { return _volatileStatusAmount[ i ]; }
            }
            return 0;
        }

        constexpr u8 getVolatileStatusCounter( volatileStatus p_volatileStatus ) const {
            for( u8 i = 0; i < MAX_VOLATILE_STATUS; ++i ) {
                if( p_volatileStatus & ( 1LLU << i ) ) { return _volatileStatusCounter[ i ]; }
            }
            return 0;
        }

        inline bool addSlotCondition( battleUI* p_ui, slotCondition p_slotCondition,
                                      u8 p_duration = 0, u16 p_amount = 0,
                                      const std::string& p_wisher = std::string( ),
                                      u16                p_src    = 0xFFFF ) {

            if( ( _slotCondition & p_slotCondition ) == p_slotCondition ) { return false; }

#ifdef DESQUID_MORE
            p_ui->log( std::string( "Add slot condition " ) + std::to_string( p_slotCondition ) );
#else
            (void) p_ui;
#endif

            for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
                if( ( 1LLU << i ) & p_slotCondition ) {
                    if( !( ( 1LLU << i ) & _slotCondition ) ) {
                        _slotConditionCounter[ i ]
                            = p_duration ? p_duration : defaultSlotConditionDurations[ i ];
                        _slotConditionAmount[ i ] = p_amount;
                    }
                }
            }
            if( p_slotCondition & SL_WISH ) { _slotConditionWisher = p_wisher; }
            if( p_slotCondition & SL_FUTURESIGHT ) { _slotConditionSrc = p_src; }
            _slotCondition = slotCondition( _slotCondition | p_slotCondition );
            return true;
        }
        bool removeSlotCondition( battleUI* p_ui, slotCondition p_slotCondition );
        constexpr slotCondition getSlotCondition( ) const {
            return _slotCondition;
        }

        /*
         * @brief: Remaining duration of a slot condition (0 = absent or
         * permanent). A condition whose counter is 1 ages out during the
         * next slot::age pass.
         */
        constexpr u8 getSlotConditionDuration( slotCondition p_slotCondition ) const {
            for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
                if( p_slotCondition == slotCondition( 1LLU << i ) ) {
                    return _slotConditionCounter[ i ];
                }
            }
            return 0;
        }

        constexpr u16 getSlotConditionAmount( slotCondition p_slotCondition ) const {
            for( u8 i = 0; i < MAX_SLOT_CONDITIONS; ++i ) {
                if( p_slotCondition == slotCondition( 1LLU << i ) ) {
                    return _slotConditionAmount[ i ];
                }
            }
            return 0;
        }

        const std::string& getSlotConditionWisher( ) const {
            return _slotConditionWisher;
        }

        constexpr u16 getSlotConditionSrc( ) const {
            return _slotConditionSrc;
        }

        void setPartialTrap( const std::string& p_wisher, u16 p_move, u16 p_species, u8 p_side,
                             u8 p_slot, u8 p_timer ) {
            _trapWisher  = p_wisher;
            _trapMove    = p_move;
            _trapSpecies = p_species;
            _trapSide    = p_side;
            _trapSlot    = p_slot;
            _trapTimer   = p_timer;
        }
        void clearPartialTrap( ) {
            _trapWisher.clear( );
            _trapMove    = 0;
            _trapSpecies = 0;
            _trapSide    = 0xFF;
            _trapSlot    = 0xFF;
            _trapTimer   = 0;
        }
        void decreasePartialTrapTimer( ) {
            if( _trapTimer ) { --_trapTimer; }
        }
        const std::string& getPartialTrapWisher( ) const {
            return _trapWisher;
        }
        u16 getPartialTrapMove( ) const {
            return _trapMove;
        }
        u16 getPartialTrapSpecies( ) const {
            return _trapSpecies;
        }
        u8 getPartialTrapSide( ) const {
            return _trapSide;
        }
        u8 getPartialTrapSlot( ) const {
            return _trapSlot;
        }
        u8 getPartialTrapTimer( ) const {
            return _trapTimer;
        }

        u8 getPerishTimer( ) const {
            return _perishTimer;
        }
        void setPerishTimer( u8 p_timer ) {
            _perishTimer = p_timer;
        }

        u8 getMoveActions( ) const {
            return _moveActions;
        }
        void registerMoveAction( ) {
            ++_moveActions;
        }

        u16 getStallCounter( ) const {
            return _stallCounter;
        }
        void setStallCounter( u16 p_counter ) {
            _stallCounter = p_counter;
        }
        u8 getStallWindow( ) const {
            return _stallWindow;
        }
        void setStallWindow( u8 p_window ) {
            _stallWindow = p_window;
        }

        bool hasCounter( ) const {
            return _counterFrom != 0xFF;
        }
        bool hasMirrorCoat( ) const {
            return _mirrorFrom != 0xFF;
        }
        bool counterRecorded( ) const {
            return _counterFrom < 0xFE;
        }
        bool mirrorRecorded( ) const {
            return _mirrorFrom < 0xFE;
        }
        u16 getCounterDamage( ) const {
            return _counterDamage;
        }
        u8 getCounterFrom( ) const {
            return _counterFrom;
        }
        u16 getMirrorDamage( ) const {
            return _mirrorDamage;
        }
        u8 getMirrorFrom( ) const {
            return _mirrorFrom;
        }
        void armCounters( bool p_mirror ) {
            _counterDamage = 0;
            _counterFrom   = ( p_mirror ? 0xFF : 0xFE );
            _mirrorDamage  = 0;
            _mirrorFrom    = ( p_mirror ? 0xFE : 0xFF );
        }
        void recordCounter( bool p_mirror, u16 p_damage, u8 p_from ) {
            if( p_mirror ) {
                _mirrorDamage = p_damage;
                _mirrorFrom   = p_from;
            } else {
                _counterDamage = p_damage;
                _counterFrom   = p_from;
            }
        }
        void clearCounters( ) {
            _counterDamage = 0;
            _counterFrom   = 0xFF;
            _mirrorDamage  = 0;
            _mirrorFrom    = 0xFF;
        }

        u16 getBideDamage( ) const {
            return _bideDamage;
        }
        u8 getBideFrom( ) const {
            return _bideFrom;
        }
        bool bideHit( ) const {
            return _bideFrom != 0xFF;
        }
        void addBideDamage( u16 p_damage, u8 p_from ) {
            const u32 tot = u32( _bideDamage ) + p_damage;
            _bideDamage   = ( tot > 0xFFFFu ) ? 0xFFFFu : u16( tot );
            _bideFrom     = p_from;
        }
        void clearBide( ) {
            _bideDamage = 0;
            _bideFrom   = 0xFF;
        }

        void setLeechSource( u8 p_src ) {
            _leechSource = p_src;
        }
        u8 getLeechSource( ) const {
            return _leechSource;
        }
        void clearLeechSource( ) {
            _leechSource = 0xFF;
        }
        bool seeded( ) const {
            return _leechSource != 0xFF;
        }

        inline void setDisabledMove( u16 p_mv ) {
            _disabledMove = p_mv;
        }
        u16 getDisabledMove( ) const {
            return _disabledMove;
        }
        inline void setEncoreMove( u16 p_mv ) {
            _encoreMove = p_mv;
        }
        u16 getEncoreMove( ) const {
            return _encoreMove;
        }
        constexpr void giveItem( u16 p_item ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _pokemon->giveItem( p_item );
            if( _isTransformed ) { _transformedPkmn.giveItem( p_item ); }
        }

        /*
         * @brief: Removes any held item. Stores it as a used item if p_used is true.
         */
        constexpr void removeItem( bool p_used = true ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            if( p_used ) {
                _usedItem = _pokemon->takeItem( );
            } else {
                _pokemon->takeItem( );
            }
            if( _isTransformed ) { _transformedPkmn.takeItem( ); }
        }

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        bool isGrounded( bool p_allowAbilities = true );

        /*
         * @brief: returns the weight of the specified pkmn.
         */
        u16 getWeight( bool p_allowAbilities = true );

        inline void addAutotomizeCut( ) {
            if( _autotomizeCuts < 255 ) { _autotomizeCuts++; }
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (current HP: 0, ATK 1, etc)
         * @returns: The numerical value of the stat or a value in [0, 15] for ACCURACY
         * and EVASION, where 7 represents no change.
         */
        u16 getStat( u8 p_stat, bool p_allowAbilities = true, bool p_ignoreNegative = false,
                     bool p_ignorePositive = false, bool p_wonderRoom = false );

        /*
         * @brief: Checks whether the pkmn can move.
         */
        bool canSelectMove( ) const;

        /*
         * @brief: Checks whether the pkmn can use its i-th move.
         */
        bool canSelectMove( u8 p_moveIdx );

        /*
         * @brief: Locks the specified pkmn to use the specified move for the next
         * p_duration turns.
         */
        inline void addLockedMove( battleMoveSelection p_move, u8 p_duration = 1 ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _lockedMove      = p_move;
            _lockedMoveTurns = p_duration;
        }

        inline battleMoveSelection getLockedMove( ) const {
            return _lockedMove;
        }

        inline void setLockedMoveCount( u8 p_turns ) {
            _lockedMoveTurns = p_turns;
        }

        /*
         * @brief: Frees the specified pkmn from any move locks.
         */
        inline void removeLockedMove( ) {
            if( _pokemon == nullptr ) [[unlikely]] { return; }

            _lockedMove      = NO_OP_SELECTION;
            _lockedMoveTurns = 0;
        }

        /*
         * @brief: Returns the number of turns the specified pkmn is locked into its
         * current move.
         */
        inline u8 getLockedMoveCount( ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return 0; }

            return _lockedMoveTurns;
        }

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( ) const {
            if( _pokemon == nullptr ) [[unlikely]] { return NO_OP_SELECTION; }
            return _lockedMove;
        }

        /*
         * @brief: Checks whether the pkmn can use a held item.
         */
        constexpr bool canUseItem( bool p_allowAbilities = true ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }

            if( p_allowAbilities && pkmn->getAbility( ) == A_KLUTZ ) [[unlikely]] { return false; }

            if( getVolatileStatus( ) & VS_EMBARGO ) [[unlikely]] { return false; }
            return true;
        }

        /*
         * @brief: Checks whether the pkmn can be switched out.
         */
        bool canSwitchOut( );

        /*
         * @brief: Sets the pkmn to the specified value.
         */
        void setPkmn( pokemon* p_pokemon );

        inline void setDisguise( const pokemon* p_pokemon ) {
            if( !p_pokemon ) {
                _disguise = nullptr;
                return;
            }
            _disguise = p_pokemon;
        }

        /*
         * @brief: Returns the pkmn currently in the slot (or nullptr if the slot is
         * empty)
         */
        constexpr pokemon* getPkmn( ) {
            if( _status != NORMAL ) { return nullptr; }

            if( !_isTransformed ) {
                return _pokemon;
            } else {
                return &_transformedPkmn;
            }
        }

        pokemon* getPkmnOrDisguise( ) const;

        inline bool isDisguised( ) const {
            return _disguise != nullptr;
        }

        inline void revertTransform( ) {
            if( !_isTransformed ) { return; }

            _isTransformed = false;
            std::memset( &_transformedPkmn, 0, sizeof( pokemon ) );

            _pkmnData = FS::getPkmnData( _pokemon->getSpecies( ), _pokemon->getForme( ) );
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        bool transformPkmn( slot* p_target );

        /*
         * @brief: Returns true iff the specified pkmn fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr ) [[unlikely]] { return false; }

            return !_isTransformed && pkmn->canBattleTransform( );
        }

        /*
         * @brief: Changes the ability of the specified pkmn to p_newAbility.
         * Does nothing and returns false if p_newAbility is 0.
         */
        inline bool changeAbility( u16 p_newAbility ) {
            if( !_pokemon ) [[unlikely]] { return false; }
            return _pokemon->setBattleTimeAbility( p_newAbility );
        }

        /*
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }
            return pkmn->getAbility( ) == A_NEUTRALIZING_GAS;
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) {
            auto pkmn = getPkmn( );
            if( pkmn == nullptr || _pokemon == nullptr ) [[unlikely]] { return false; }
            switch( pkmn->getAbility( ) ) {
            case A_AIR_LOCK:
            case A_CLOUD_NINE:
                return true;
            [[likely]] default:
                return false;
            }
        }
    };
} // namespace BATTLE
