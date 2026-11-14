/*
   Pokémon neo
   ------------------------------

   file        : battleField.h
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
#include "battle/battleSide.h"
#include "battle/move.h"
#include "fs/data.h"
#include "gen/abilityNames.h"
#include "gen/pokemonNames.h"
#include "io/strings.h"
#include "pokemon.h"

namespace BATTLE {
    class battle;

    /*
     * @brief: The overall field where the battle takes place.
     */
    class field {
      public:
        static constexpr u8 PLAYER_SIDE       = 0;
        static constexpr u8 OWN_SIDE          = PLAYER_SIDE;
        static constexpr u8 OPPONENT_SIDE     = 1;
        static constexpr u8 PKMN_0            = 0;
        static constexpr u8 PKMN_1            = 1;
        static constexpr u8 NUM_SIDES         = 2;
        static constexpr u8 NORMAL_DURATION   = 5;
        static constexpr u8 EXTENDED_DURATION = 8;

      private:
        weather    _weather;
        u8         _weatherTimer; // Number of turns the weather is still active
        u8         _pseudoWeatherTimer[ MAX_PSEUDO_WEATHER ];
        terrain    _terrain;
        u8         _terrainTimer;
        side       _sides[ NUM_SIDES ];
        pokemon*   _teams[ NUM_SIDES ]     = { nullptr, nullptr };
        u8         _teamSizes[ NUM_SIDES ] = { 0, 0 };
        bool       _isWildBattle;
        battleMode _mode;
        battle*    _battle;
        bool       _hitSubstituted  = false;
        bool       _targetActsLater = false;
        bool       _nestedCallsMove = false;
        u8         _beatUpIdx       = 0;
        u16        _beatUpBP[ 6 ]   = { 5, 5, 5, 5, 5, 5 };
        boosts     _mhPending[ 2 ][ 2 ];
        bool       _mhArmed[ 2 ][ 2 ]        = { { false, false }, { false, false } };
        u8         _metronomeNum[ 2 ][ 2 ]   = { { 0, 0 }, { 0, 0 } };
        u16        _metronomeLast[ 2 ][ 2 ]  = { { 0, 0 }, { 0, 0 } };
        u8         _actionThisTurn[ 2 ][ 2 ] = { { 0, 0 }, { 0, 0 } };
        u8         _actionLastTurn[ 2 ][ 2 ] = { { 0, 0 }, { 0, 0 } };
        u8         _paradoxStat[ 2 ][ 2 ]    = { { 0, 0 }, { 0, 0 } };
        u8         _paradoxBooster[ 2 ][ 2 ] = { { 0, 0 }, { 0, 0 } };

        u8   _beatUpAllyBPs( fieldPosition p_user, u16* p_out );
        void _mirrorHerbConsume( battleUI* p_ui );
        u8   _paradoxBestStat( bool p_opponent, u8 p_slot );
        void _paradoxRefresh( battleUI* p_ui );

      public:
        static constexpr u16 BOOST_SRC_UNKNOWN = 0xFFFF;
        static constexpr u16 BOOST_SRC_HAZARD  = 0xFFFE;
        static constexpr u16 BOOST_SRC_MHCOPY  = 0xFFFD;
        static constexpr u8  MOVE_FAIL_NO_PP   = 2;
        static constexpr u8  MOVE_FAIL         = 1;
        static constexpr u8  MOVE_SUCCESS      = 0;

        static constexpr u16 packBoostSrc( fieldPosition p_pos ) {
            return u16( ( u16( p_pos.first ) << 8 ) | p_pos.second );
        }

        field( battle* p_battle, battleMode p_battleMode = BM_SINGLE, bool p_isWildBattle = false,
               weather p_initialWeather = WE_NONE, pseudoWeather p_initialPseudoWeather = PW_NONE,
               terrain p_initialTerrain = TR_NONE );

        /*
         * @brief: Shell field - every member inert, no battle attached.
         */
        field( )
            : _weather{ WE_NONE }, _weatherTimer{ 0 }, _pseudoWeatherTimer{ }, _terrain{ TR_NONE },
              _terrainTimer{ 0 }, _sides{ }, _isWildBattle{ false }, _mode{ BM_SINGLE },
              _battle{ nullptr } {
        }

        /*
         * Initializes the field.
         */
        void init( battleUI* p_ui );
        /*
         * Ages the field by one turn, processes all weather changes
         */
        void age( battleUI* p_ui );

        constexpr u16 getTurnsInPlay( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getTurnsInPlay( p_slot );
        }

        constexpr u16 getAndIncreaseToxicCount( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getAndIncreaseToxicCount(
                p_slot );
        }

        constexpr pkmnData getPkmnData( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPkmnData( p_slot );
        }

        inline slot* getSlot( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlot( p_slot );
        }

        constexpr slot::status getSlotStatus( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotStatus( p_slot );
        }

        /*
         * @brief: Tries to set a new weather; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool setWeather( battleUI* p_ui, weather p_newWeather, bool p_extended = false );
        bool removeWeather( battleUI* p_ui );
        void continueWeather( battleUI* p_ui );
        constexpr weather getWeather( ) const {
            return _weather;
        }

        /*
         * @brief: Tries to set a new pseudo weather; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool setPseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather,
                               bool p_extended = false );
        bool removePseudoWeather( battleUI* p_ui, pseudoWeather p_newPseudoWeather );
        constexpr pseudoWeather getPseudoWeather( ) const {
            pseudoWeather res = pseudoWeather( 0 );
            for( u8 i = 0; i < MAX_PSEUDO_WEATHER; ++i ) {
                if( _pseudoWeatherTimer[ i ] ) { res = pseudoWeather( res | ( 1LLU << i ) ); }
            }
            return res;
        }

        /*
         * @brief: Tries to set a new terrain; returns false iff that fails.
         * @param p_extended: If true, the duration is extended to 8 turns instead of the
         * usual 5.
         */
        bool setTerrain( battleUI* p_ui, terrain p_newTerrain, bool p_extended = false );
        bool removeTerrain( battleUI* p_ui );
        constexpr terrain getTerrain( ) const {
            return _terrain;
        }

        constexpr sideCondition getSideCondition( bool p_opponent ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSideCondition( );
        }

        /*
         * @brief: Adds the given side conditions to the specified side
         */
        inline bool addSideCondition( battleUI* p_ui, bool p_opponent,
                                      sideCondition p_sideCondition, u8 p_duration = 0,
                                      bool p_allowStacking = false ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addSideCondition(
                p_ui, p_sideCondition, p_duration, p_allowStacking );
        }

        /*
         * @brief: Removes the given side conditions from the specified side
         */
        inline bool removeSideCondition( battleUI* p_ui, bool p_opponent,
                                         sideCondition p_sideCondition ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeSideCondition(
                p_ui, p_sideCondition );
        }

        /*
         * @brief: The layer count of the given single side condition on the
         * specified side; 0 when the condition is not set.
         */
        inline u8 sideConditionAmount( bool p_opponent, sideCondition p_sideCondition ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].sideConditionAmount(
                p_sideCondition );
        }

        /*
         * @brief: Adds the given slot conditions to the specified slot.
         */
        inline bool addSlotCondition( battleUI* p_ui, bool p_opponent, u8 p_pos,
                                      slotCondition p_slotCondition, u8 p_duration = 0,
                                      u16                p_amount = 0,
                                      const std::string& p_wisher = std::string( ),
                                      u16                p_src    = 0xFFFF ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addSlotCondition(
                p_ui, p_pos, p_slotCondition, p_duration, p_amount, p_wisher, p_src );
        }

        /*
         * @brief: Removes the given slot conditions from the specified slot
         */
        inline bool removeSlotCondition( battleUI* p_ui, bool p_opponent, u8 p_pos,
                                         slotCondition p_slotCondition ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeSlotCondition(
                p_ui, p_pos, p_slotCondition );
        }

        constexpr slotCondition getSlotCondition( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotCondition( p_pos );
        }

        constexpr u8 getSlotConditionDuration( bool p_opponent, u8 p_pos,
                                               slotCondition p_slotCondition ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotConditionDuration(
                p_pos, p_slotCondition );
        }

        constexpr u16 getSlotConditionAmount( bool p_opponent, u8 p_pos,
                                              slotCondition p_slotCondition ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotConditionAmount(
                p_pos, p_slotCondition );
        }

        constexpr u16 getSlotConditionSrc( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotConditionSrc( p_pos );
        }

        constexpr u16 getSubstituteHp( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSubstituteHp( p_pos );
        }

        inline u16 substituteHpFor( u16 p_maxHp ) {
            return p_maxHp / 4;
        }

        inline void setSubstituteHp( bool p_opponent, u8 p_pos, u16 p_hp ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setSubstituteHp( p_pos, p_hp );
        }

        bool substituteExempt( battleMove const& p_move, fieldPosition const& p_target );

        bool substituteBlocks( battleMove const& p_move, fieldPosition const& p_target ) {
            if( substituteExempt( p_move, p_target ) ) return false;
            return getVolatileStatus( p_target.first, p_target.second ) & VS_SUBSTITUTE;
        }

        bool moveInfiltrates( battleMove const& p_move ) {
            auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
            return user != nullptr && !suppressesAbilities( )
                   && !slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second )
                   && user->getAbility( ) == A_INFILTRATOR;
        }

        bool gateExternalEffect( battleMove const& p_move, fieldPosition const& p_target,
                                 sideCondition p_cond ) {
            if( !( getSideCondition( p_target.first ) & p_cond ) ) return false;
            if( p_target.first == p_move.m_user.first && p_target.second == p_move.m_user.second )
                return false;
            if( moveInfiltrates( p_move ) && p_target.first != p_move.m_user.first ) return false;
            return true;
        }

        bool mistStripsNegatives( battleMove const& p_move, fieldPosition const& p_target,
                                  boosts& p_boosts ) {
            if( !gateExternalEffect( p_move, p_target, SC_MIST ) ) return false;
            bool stripped = false;
            for( u8 i = 0; i < NUM_BOOSTS; ++i ) {
                if( p_boosts.getBoost( i ) < 0 ) {
                    p_boosts.setBoost( i, 0 );
                    stripped = true;
                }
            }
            return stripped;
        }

        u8 terrainGuardsStatus( fieldPosition const& p_target, bool p_statusApp,
                                bool p_sleep = false, bool p_yawnVolatile = false,
                                bool p_confusionVolatile = false, bool p_secondaries = false,
                                bool p_moveEffect = true ) {
            auto terrain = getTerrain( );
            if( terrain != TR_ELECTRICTERRAIN && terrain != TR_MISTYTERRAIN ) { return 0; }
            if( !isGrounded( p_target.first, p_target.second ) ) { return 0; }
            if( isSemiInvulnerable( p_target.first, p_target.second ) ) { return 0; }
            if( terrain == TR_ELECTRICTERRAIN ) {
                if( !p_sleep && !p_yawnVolatile ) { return 0; }
                return ( p_yawnVolatile || ( p_moveEffect && !p_secondaries ) ) ? 2 : 1;
            }
            if( p_confusionVolatile ) { return ( p_moveEffect && !p_secondaries ) ? 2 : 1; }
            if( p_statusApp ) { return ( p_moveEffect && !p_secondaries ) ? 2 : 1; }
            return 0;
        }

        bool psychicTerrainGuards( battleMove const& p_move, fieldPosition const& p_target ) {
            return psychicTerrainGuardsPrio( p_move, p_target, effectiveMovePriority( p_move ) );
        }

        bool psychicTerrainGuardsPrio( battleMove const& p_move, fieldPosition const& p_target,
                                       s8 p_priority ) {
            if( getTerrain( ) != TR_PSYCHICTERRAIN ) { return false; }
            if( p_move.m_moveData.m_target == TG_SELF ) { return false; }
            if( p_priority <= 0 ) { return false; }
            if( p_target.first == p_move.m_user.first ) { return false; }
            if( isSemiInvulnerable( p_target.first, p_target.second ) ) { return false; }
            return isGrounded( p_target.first, p_target.second );
        }

        static bool moveHasSecondaries( battleMove const& p_move ) {
            auto const& d = p_move.m_moveData;
            return d.m_secondaryStatus || d.m_secondaryVolatileStatus
                   || d.m_secondaryBoosts != boosts( ) || d.m_secondarySelfBoosts != boosts( );
        }

        static bool hasSheerForceBoostFlag( u16 p_moveId ) {
            return p_moveId == M_ELECTRO_SHOT || p_moveId == M_ORDER_UP;
        }

        bool sheerForceUser( battleMove const& p_move ) {
            auto user = getPkmn( p_move.m_user.first, p_move.m_user.second );
            if( user == nullptr || suppressesAbilities( )
                || slotAbilitiesNullified( p_move.m_user.first, p_move.m_user.second ) ) {
                return false;
            }
            return user->getAbility( ) == A_SHEER_FORCE;
        }

        bool sheerForceStrips( battleMove const& p_move ) {
            return sheerForceUser( p_move ) && moveHasSecondaries( p_move )
                   && !hasSheerForceBoostFlag( p_move.m_param );
        }

        s8   effectiveMovePriority( battleMove const& p_move );
        bool redirectSingleTarget( battleUI* p_ui, battleMove& p_move, fieldPosition& io_target );

        const std::string& getSlotConditionWisher( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getSlotConditionWisher(
                p_pos );
        }

        void setPartialTrap( bool p_opponent, u8 p_pos, const std::string& p_wisher, u16 p_move,
                             u16 p_species, u8 p_side, u8 p_slot, u8 p_timer ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setPartialTrap(
                p_pos, p_wisher, p_move, p_species, p_side, p_slot, p_timer );
        }
        void clearPartialTrap( bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].clearPartialTrap( p_pos );
        }
        void decreasePartialTrapTimer( bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].decreasePartialTrapTimer( p_pos );
        }
        const std::string& getPartialTrapWisher( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPartialTrapWisher( p_pos );
        }
        u16 getPartialTrapMove( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPartialTrapMove( p_pos );
        }
        u16 getPartialTrapSpecies( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPartialTrapSpecies(
                p_pos );
        }
        u8 getPartialTrapSide( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPartialTrapSide( p_pos );
        }
        u8 getPartialTrapSlot( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPartialTrapSlot( p_pos );
        }
        u8 getPartialTrapTimer( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPartialTrapTimer( p_pos );
        }

        void setDisabledMove( bool p_opponent, u8 p_pos, u16 p_mv ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setDisabledMove( p_pos, p_mv );
        }
        u16 getDisabledMove( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getDisabledMove( p_pos );
        }
        void setEncoreMove( bool p_opponent, u8 p_pos, u16 p_mv ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setEncoreMove( p_pos, p_mv );
        }
        u16 getEncoreMove( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getEncoreMove( p_pos );
        }

        u8 getPerishTimer( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPerishTimer( p_pos );
        }
        void setPerishTimer( bool p_opponent, u8 p_pos, u8 p_timer ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setPerishTimer( p_pos, p_timer );
        }

        u8 getMoveActions( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getMoveActions( p_pos );
        }
        void registerMoveAction( bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].registerMoveAction( p_pos );
        }

        u16 getStallCounter( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStallCounter( p_pos );
        }
        void setStallCounter( bool p_opponent, u8 p_pos, u16 p_counter ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setStallCounter( p_pos, p_counter );
        }
        u8 getStallWindow( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStallWindow( p_pos );
        }
        void setStallWindow( bool p_opponent, u8 p_pos, u8 p_window ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setStallWindow( p_pos, p_window );
        }

        static u8 packCounterPos( fieldPosition p_pos ) {
            return u8( ( p_pos.first ? 1u : 0u ) << 1 | p_pos.second );
        }
        static fieldPosition unpackCounterPos( u8 p_packed ) {
            return fieldPosition( bool( ( p_packed >> 1 ) & 1u ), u8( p_packed & 1u ) );
        }
        bool hasCounter( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasCounter( p_pos );
        }
        bool hasMirrorCoat( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasMirrorCoat( p_pos );
        }
        bool counterRecorded( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].counterRecorded( p_pos );
        }
        bool mirrorRecorded( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].mirrorRecorded( p_pos );
        }
        u16 getCounterDamage( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getCounterDamage( p_pos );
        }
        u16 getMirrorDamage( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getMirrorDamage( p_pos );
        }
        fieldPosition getCounterTarget( bool p_opponent, u8 p_pos ) const {
            return unpackCounterPos(
                _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getCounterFrom( p_pos ) );
        }
        fieldPosition getMirrorTarget( bool p_opponent, u8 p_pos ) const {
            return unpackCounterPos(
                _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getMirrorFrom( p_pos ) );
        }
        void armCounters( bool p_opponent, u8 p_pos, bool p_mirror ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].armCounters( p_pos, p_mirror );
        }
        void recordCounter( bool p_opponent, u8 p_pos, bool p_mirror, u16 p_damage,
                            fieldPosition p_from ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].recordCounter(
                p_pos, p_mirror, p_damage, packCounterPos( p_from ) );
        }
        u16 getBideDamage( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getBideDamage( p_pos );
        }
        fieldPosition getBideTarget( bool p_opponent, u8 p_pos ) const {
            return unpackCounterPos(
                _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getBideFrom( p_pos ) );
        }
        bool bideHit( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].bideHit( p_pos );
        }
        void addBideDamage( bool p_opponent, u8 p_pos, u16 p_damage, fieldPosition p_from ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addBideDamage(
                p_pos, p_damage, packCounterPos( p_from ) );
        }
        void clearBide( bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].clearBide( p_pos );
        }
        void clearCounters( bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].clearCounters( p_pos );
        }
        bool seeded( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].seeded( p_pos );
        }
        void setLeechSource( bool p_opponent, u8 p_pos, fieldPosition p_src ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setLeechSource(
                p_pos, packCounterPos( p_src ) );
        }
        fieldPosition getLeechSource( bool p_opponent, u8 p_pos ) const {
            return unpackCounterPos(
                _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getLeechSource( p_pos ) );
        }
        bool isSoundBlockedForStatus( battleMove p_move, bool p_opponent, u8 p_slot );
        bool windRiderSteal( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        inline void healFully( battleUI* p_ui, bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].healFully( p_pos );
            p_ui->updatePkmnStats( p_opponent, p_pos, getPkmnOrDisguise( p_opponent, p_pos ),
                                   false );
        }

        inline void restoreAllMovePP( bool p_opponent, u8 p_pos ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].restoreAllMovePP( p_pos );
        }

        bool hasMovePPDeficit( bool p_opponent, u8 p_pos ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasMovePPDeficit( p_pos );
        }
        bool hasMove( bool p_opponent, u8 p_pos, u16 p_moveId ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasMove( p_pos, p_moveId );
        }

        void setTeams( pokemon* p_playerTeam, u8 p_playerTeamSize, pokemon* p_opponentTeam,
                       u8 p_opponentTeamSize ) {
            _teams[ PLAYER_SIDE ]       = p_playerTeam;
            _teamSizes[ PLAYER_SIDE ]   = p_playerTeamSize;
            _teams[ OPPONENT_SIDE ]     = p_opponentTeam;
            _teamSizes[ OPPONENT_SIDE ] = p_opponentTeamSize;
        }

        bool hasHealthyBenched( bool p_opponent ) const {
            u8       sideIdx = p_opponent ? OPPONENT_SIDE : PLAYER_SIDE;
            pokemon* team    = _teams[ sideIdx ];
            if( team == nullptr ) { return true; } // unregistered context: never block
            for( u8 k = getBattlingPKMNCount( _mode ); k < _teamSizes[ sideIdx ]; ++k ) {
                if( team[ k ].canBattle( ) ) { return true; }
            }
            return false;
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr volatileStatus getVolatileStatus( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getVolatileStatus( p_slot );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr u8 getVolatileStatusCounter( bool p_opponent, u8 p_slot,
                                               volatileStatus p_volatileStatus ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getVolatileStatusCounter(
                p_slot, p_volatileStatus );
        }

        constexpr u8 getVolatileStatusAmount( bool p_opponent, u8 p_slot,
                                              volatileStatus p_volatileStatus ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getVolatileStatusAmount(
                p_slot, p_volatileStatus );
        }

        constexpr bool removeVolatileStatus( battleUI* p_ui, bool p_opponent, u8 p_slot,
                                             volatileStatus p_volatileStatus ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeVolatileStatus(
                p_ui, p_slot, p_volatileStatus );
        }

        /*
         * @brief: returns the volatile statuses of the pkmn in the given slot.
         */
        constexpr bool addVolatileStatus( battleUI* p_ui, bool p_opponent, u8 p_slot,
                                          volatileStatus p_volatileStatus, u8 p_duration ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addVolatileStatus(
                p_ui, p_slot, p_volatileStatus, p_duration );
        }

        constexpr bool restackHelpingHand( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].restackVolatile( p_slot, 38 );
        }

        constexpr bool setStatusCondition( bool p_opponent, u8 p_slot, u8 p_status,
                                           u8 p_duration = 255, bool p_force = false ) {

            auto pkmn = getPkmn( p_opponent, p_slot );
            if( pkmn == nullptr ) [[unlikely]] { return false; }

            if( !suppressesAbilities( ) && !suppressesWeather( )
                && !slotAbilitiesNullified( p_opponent, p_slot )
                && pkmn->getAbility( ) == A_LEAF_GUARD
                && ( getWeather( ) == WE_SUN || getWeather( ) == WE_HEAVY_SUNSHINE ) ) {
                return false;
            }

            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setStatusCondition(
                p_slot, p_status, p_duration, p_force );
        }

        /*
         * @brief: Removes any status condition a pkmn may have
         */
        constexpr bool removeStatusCondition( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeStatusCondition(
                p_slot );
        }

        /*
         * @brief: Returns the if the pkmn has the specified status condition.
         */
        constexpr u8 hasStatusCondition( bool p_opponent, u8 p_slot, u8 p_status ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasStatusCondition(
                p_slot, p_status );
        }

        /*
         * @brief: Locks the specified pkmn to use the specified move for the next
         * p_duration turns.
         */
        inline void addLockedMove( bool p_opponent, u8 p_slot, battleMoveSelection p_move,
                                   u8 p_duration = 1 ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].addLockedMove( p_slot, p_move,
                                                                              p_duration );
        }

        /*
         * @brief: Frees the specified pkmn from any move locks.
         */
        inline void removeLockedMove( bool p_opponent, u8 p_slot ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].removeLockedMove( p_slot );
        }

        /*
         * @brief: Returns the number of turns the specified pkmn is locked into its
         * current move.
         */
        inline u8 getLockedMoveCount( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getLockedMoveCount( p_slot );
        }

        bool        fatigueConfusionBlocked( bool p_opponent, u8 p_slot );
        static bool sleepTalkExcluded( u16 p_id );
        bool        nestedCallsMoveActive( void ) const {
            return _nestedCallsMove;
        }
        bool uproarActive( void );
        bool blockSleepByUproar( battleUI* p_ui, bool p_opponent, u8 p_slot );
        void uproarWakeAll( battleUI* p_ui );

        /*
         * @brief: Sets the move the specified pkmn used last.
         */
        inline void setLastUsedMove( bool p_opponent, u8 p_slot, battleMove p_move ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setLastUsedMove( p_slot, p_move );
        }

        inline void markMoveActed( bool p_opponent, u8 p_slot ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].markMoveActed( p_slot );
        }

        inline battleMove getLastUsedMove( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getLastUsedMove( p_slot );
        }

        /*
         * @brief: Faints the specified pkmn. Also deals the necessary damage.
         */
        inline void faintPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot ) {
            auto pkmn = getPkmnOrDisguise( p_opponent, p_slot );
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].faintPokemon( p_slot );
            p_ui->updatePkmnStats( p_opponent, p_slot, pkmn, false );
            p_ui->faintPkmn( p_opponent, p_slot, pkmn );
        }

        bool canRecallPokemon( bool p_opponent );

        /*
         * @brief: Recalls the pkmn at the specified position.
         */
        inline void recallPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot,
                                   bool p_keepChanges = false, bool p_forced = false ) {
            p_ui->recallPkmn( p_opponent, p_slot, getPkmnOrDisguise( p_opponent, p_slot ),
                              p_forced );
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].recallPokemon( p_slot,
                                                                              p_keepChanges );
        }

        /*
         * @brief: Deals the specified amount of damage to the specified pkmn. Does not
         * faint pkmn.
         */
        inline void damagePokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_damage ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].damagePokemon( p_slot, p_damage );
            p_ui->updatePkmnStats( p_opponent, p_slot, getPkmnOrDisguise( p_opponent, p_slot ),
                                   false );
        }

        /*
         * @brief: Deals the specified amount of damage to the specified pkmn.
         */
        inline void healPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_damage ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].healPokemon( p_slot, p_damage );
            p_ui->updatePkmnStats( p_opponent, p_slot, getPkmnOrDisguise( p_opponent, p_slot ),
                                   false );
        }

        /*
         * @brief: Sorts the give moves, computes targets. May use up I_CUSTAP_BERRY
         */
        std::vector<battleMove>
        computeSortedBattleMoves( battleUI*                               p_ui,
                                  const std::vector<battleMoveSelection>& p_selectedMoves );

        /*
         * @brief: Deduces the specified number of PP from the first move of the specified
         * pkmn that has the specified p_moveId. If p_amount is 255, the amount defaults
         * to 1 (or 2 if there is a pkmn with A_PRESSURE and no pkmn with
         * A_NEUTRALIZING_GAS on the field.
         */
        constexpr void deducePP( bool p_opponent, u8 p_slot, u16 p_moveId, u8 p_amount = 255 ) {
            if( p_amount == 255 ) {
                p_amount = 1;
                if( !suppressesAbilities( ) && _sides[ !p_opponent ].anyHasAbility( A_PRESSURE ) ) {
                    p_amount = 2;
                }
            }

            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].deducePP( p_slot, p_moveId,
                                                                         p_amount );
        }

        /*
         * @brief: Makes the specified pkmn damage itself due to confusion.
         */
        void confusionSelfDamage( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: checks whether the specified move misses.
         */
        bool moveMisses( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                         bool p_critical );

        /*
         * @brief: Checks whether the specified move will result in a critical hit on the
         * target.
         */
        bool executeCriticalCheck( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes the specified damaging move, dealing damage/heal to the
         * specified target. (Only deals damage/recoil)
         */
        bool executeDamagingMove( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                  bool p_critical, u8 p_damageModifier = 100 );

        /*
         * @brief: Executes all self boosts.
         */
        void executeSelfStatusEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes all status effects of the specified move.
         */
        void executeStatusEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes secondary status effects of the specified move.
         */
        void executeSecondaryStatus( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes all non-status secondary effects.
         */
        void executeSecondaryEffects( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes all effects that happen when the pkmn makes contact using its
         * move.
         */
        void executeContact( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Executes the given battle move and all effects related to it.
         */
        void executeBattleMove( battleUI* p_ui, battleMove p_move,
                                const std::vector<battleMove>& p_targetsMoves,
                                const std::vector<battleMove>& p_tergetedMoves );

        /*
         * @brief: Sets the pkmn of the specified slot. Only used at battle start
         */
        inline void setSlot( bool p_opponent, u8 p_slot, pokemon* p_pokemon ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setSlot( p_slot, p_pokemon );
        }

        inline void setSlotDisguise( bool p_opponent, u8 p_slot, const pokemon* p_pokemon ) {
            _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setSlotDisguise( p_slot, p_pokemon );
        }

        inline u8 willDisguise( pokemon*      p_pokemon,
                                const pokemon p_teamContext[ SAVE::NUM_PARTY_SLOTS ] ) {
            if( p_pokemon->getAbility( ) != A_ILLUSION ) { return 0; }

            for( u8 i = SAVE::NUM_PARTY_SLOTS - 1; i > 0; --i ) {
                if( p_teamContext[ i ].getAbility( ) == A_ILLUSION ) { return 0; }
                if( p_teamContext[ i ].getSpecies( ) && p_teamContext[ i ].canBattle( ) ) {
                    return i;
                }
            }
            return 0;
        }

        /*
         * @brief: Sends out a new pkmn to a slot, overriding anything that was there
         * beforehand
         */
        inline bool sendPokemon( battleUI* p_ui, bool p_opponent, u8 p_slot, pokemon* p_pokemon,
                                 const pokemon p_teamContext[ SAVE::NUM_PARTY_SLOTS ] ) {
            u8 disg = willDisguise( p_pokemon, p_teamContext );
            setSlot( p_opponent, p_slot, p_pokemon );

            if( disg ) { setSlotDisguise( p_opponent, p_slot, &p_teamContext[ disg ] ); }
            p_ui->sendOutPkmn( p_opponent, p_slot, getPkmnOrDisguise( p_opponent, p_slot ) );
            checkOnSendOut( p_ui, p_opponent, p_slot );
            return true;
        }

        /*
         * @brief: Handles anything a pkmn may do when sent out.
         */
        void checkOnSendOut( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: Executes effects that happen when a pkmn eats a berry.
         */
        void checkOnEatBerry( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_berry );

        /*
         * @brief: Checks whether the specified pkmn uses its item after an attack.
         */
        void checkItemAfterAttack( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: Checks for effects that happen when a move damages a pkmn
         */
        void checkOnTakeDamage( battleUI* p_ui, battleMove p_move, fieldPosition p_target,
                                u16 p_damage, u8 p_effectiveness );

        void destinyBondTakeDown( battleUI* p_ui, battleMove p_move, fieldPosition p_target );

        void grudgePayOff( battleUI* p_ui, battleMove p_move );

        /*
         * @brief: Returns a list of all types the pkmn currently has. May be empty
         */
        inline std::vector<type> getTypes( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getTypes( p_slot );
        }

        /*
         * @brief: Sets the type of the specified pkmn.
         */
        inline void setType( battleUI* p_ui, bool p_opponent, u8 p_slot, type p_type ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setType( p_ui, p_slot,
                                                                               p_type );
        }

        /*
         * @brief: Sets the extra type of the specified pkmn.
         */
        inline void setExtraType( battleUI* p_ui, bool p_opponent, u8 p_slot, type p_type ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].setExtraType( p_ui, p_slot,
                                                                                    p_type );
        }

        /*
         * @brief: Checks whether the pkmn currently has the specified type.
         */
        constexpr bool hasType( bool p_opponent, u8 p_slot, type p_type ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].hasType( p_slot, p_type );
        }

        constexpr u8 getConsecutiveMoveCount( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getConsecutiveMoveCount(
                p_slot );
        }

        /*
         * @brief: Adds the specified boost to the specified pkmn, returns true iff
         * successful
         */
        boosts addBoosts( bool p_opponent, u8 p_slot, boosts p_boosts, bool p_allowAbilities = true,
                          bool p_allowOverMax = false, u16 p_src = BOOST_SRC_UNKNOWN );
        bool   resetBoosts( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].resetBoosts( p_slot );
        }
        inline boosts getBoosts( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getBoosts( p_slot );
        }

        inline void revertTransform( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].revertTransform( p_slot );
        }

        /*
         * @brief: Transforms the pkmn at the specified position to the specified pkmn.
         * @returns: true iff the transformation succeeded.
         */
        inline bool transformPkmn( bool p_opponent, u8 p_slot, slot* p_target ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].transformPkmn( p_slot,
                                                                                     p_target );
        }

        /*
         * @brief: Changes the ability of the specified pkmn to p_newAbility.
         * Does nothing and returns false if p_newAbility is 0.
         */
        inline bool changeAbility( bool p_opponent, u8 p_slot, u16 p_newAbility ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].changeAbility( p_slot,
                                                                                     p_newAbility );
        }

        /*
         * @brief: Returns the pkmn in the specified slot or nullptr if the slot is empty.
         */
        constexpr pokemon* getPkmn( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPkmn( p_slot );
        }

        constexpr pokemon* getPkmnOrDisguise( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getPkmnOrDisguise( p_slot );
        }

        /*
         * @brief: Checks whether any pkmn on the field has the specified ability.
         */
        constexpr bool anyHasAbility( u16 p_ability ) const {
            return _sides[ true ].anyHasAbility( p_ability )
                   || _sides[ false ].anyHasAbility( p_ability );
        }

        /*
         * @brief: Checks whether abilities are currently suppressed by something on the
         * field. (Some abilities cannot be suppressed).
         */
        constexpr bool suppressesAbilities( ) const {
            return anyHasAbility( A_NEUTRALIZING_GAS );
        }

        bool slotAbilitiesNullified( bool p_opponent, u8 p_pos ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].abilitiesNullified( p_pos );
        }

        /*
         * @brief: Checks whether the specified pkmn has any of the abilities
         * A_MOLD_BREAKER, A_TERAVOLT, or A_TURBOBLAZE
         */
        inline bool breaksAbilities( bool p_opponent, u8 p_pos ) {
            auto p = getPkmn( p_opponent, p_pos );
            if( p == nullptr ) [[unlikely]] { return false; }

            return p->getAbility( ) == A_MOLD_BREAKER || p->getAbility( ) == A_TERAVOLT
                   || p->getAbility( ) == A_TURBOBLAZE;
        }

        /*
         * @brief: Checks whether weather effects are currently suppressed by something on the
         * field
         */
        constexpr bool suppressesWeather( ) const {
            return anyHasAbility( A_AIR_LOCK ) || anyHasAbility( A_CLOUD_NINE );
        }

        /*
         * @brief: Gets the current value of the specified stat (with all modifiers
         * applied). (HP: 0, ATK 1, etc)
         */
        u16 getStat( bool p_opponent, u8 p_slot, u8 p_stat, bool p_allowAbilities = true,
                     bool p_ignoreNegative = false, bool p_ignorePositive = false );

        /*
         * @brief: Returns how effective the given move is on the specified target.
         * @returns: Damage multiplier multiplied with 100.
         */
        u16 getEffectiveness( battleMove p_move, fieldPosition p_target );

        /*
         * @brief: Checks whether the pkmn can move.
         */
        constexpr bool canSelectMove( bool p_opponent, u8 p_slot ) const {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canSelectMove( p_slot );
        }
        bool sealedByImprison( bool p_opponent, u16 p_move );

        /*
         * @brief: Checks whether the pkmn can use its i-th move.
         */
        bool canSelectMove( bool p_opponent, u8 p_slot, u8 p_moveIdx );

        /*
         * @brief: Returns the move the pkmn in this slot is forced/preparing to use.
         */
        inline battleMoveSelection getStoredMove( bool p_opponent, u8 p_slot ) {
            auto tmp = _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].getStoredMove( p_slot );
            tmp.m_user.first = p_opponent ? OPPONENT_SIDE : PLAYER_SIDE;
            return tmp;
        }

        u8 useMove( battleUI* p_ui, battleMove p_move );

        /*
         * @brief: Checks whether the pkmn can use an item (from the bag or hold).
         */
        constexpr bool canUseItem( bool p_opponent, u8 p_slot, bool p_allowAbilities = true ) {
            if( getPseudoWeather( ) & PW_MAGICROOM ) { return false; }

            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canUseItem(
                p_slot, p_allowAbilities && !suppressesAbilities( ) );
        }

        /*
         * @brief: Checks whether the pkmn can be switched out.
         */
        bool canSwitchOut( bool p_opponent, u8 p_slot );

        /*
         * @brief: Moves the held item.
         */
        void moveItem( battleUI* p_ui, bool p_sourceOpp, u8 p_sorceSlot, bool p_targetOpp,
                       u8 p_targetSlot );

        void giveItem( battleUI* p_ui, bool p_opponent, u8 p_slot, u16 p_item );

        /*
         * @brief: Removes any held item.
         */
        void removeItem( battleUI* p_ui, bool p_opponent, u8 p_slot, bool p_used = true );

        /*
         * @brief: returns the type of the attack of the specified battle move.
         */
        type getMoveType( battleMove p_move );

        /*
         * @brief: returns the base power of the attack of the specified battle move.
         */
        u16 getMovePower( battleMove p_move, fieldPosition p_defender );

        /*
         * @brief: Returns true iff the specified pkmn fulfills all reqs to mega
         * evolve.
         */
        constexpr bool canMegaEvolve( bool p_opponent, u8 p_slot ) {
            return _sides[ p_opponent ? OPPONENT_SIDE : PLAYER_SIDE ].canMegaEvolve( p_slot );
        }

        /*
         * @brief: Mega evolve the specified pkmn.
         */
        void megaEvolve( battleUI* p_ui, bool p_opponent, u8 p_slot );

        /*
         * @brief: returns the weight of the specified pkmn.
         */
        u16 getWeight( bool p_opponent, u8 p_slot, bool p_allowAbilities = true );

        /*
         * @brief: returns whether the specified pkmn currently touches the ground.
         */
        bool isGrounded( bool p_opponent, u8 p_slot, bool p_allowAbilities = true );

        inline bool isSemiInvulnerable( bool p_opponent, u8 p_slot ) {
            return ( getVolatileStatus( p_opponent, p_slot )
                     & ( VS_DIGGING | VS_DIVING | VS_INAIR | VS_INVISIBLE ) )
                   != volatileStatus( 0 );
        }
    };
} // namespace BATTLE
