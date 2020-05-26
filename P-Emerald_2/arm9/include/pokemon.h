/*
Pokémon neo
------------------------------

file        : pokemon.h
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
#include <algorithm>
#include <type_traits>
#include "defines.h"
#include "pokemonData.h"

struct trainerPokemon {
    u16 m_speciesId;
    u8  m_forme;
    u8  m_level;
    u16 m_ability;
    u16 m_heldItem;
    u16 m_moves[ 4 ];
    u8  m_ev[ 6 ];
    u8  m_iv[ 6 ];
    u8  m_shiny;
};

struct boxPokemon {
    u32 m_pid      = 0;
    u16 m_checksum = 0;

    u16 m_speciesId         = 0;
    u16 m_heldItem          = 0;
    u16 m_oTId              = 0;
    u16 m_oTSid             = 0;
    u32 m_experienceGained  = 0;
    u8  m_steps             = 0; // StepstoHatch/256 // Happiness
    u8  m_ability           = 0;
    u8  m_markings          = 0;
    u8  m_origLang          = 0;
    u8  m_effortValues[ 6 ] = {0}; // HP,Attack,Defense,SAttack,SDefense,Speed
    u8  m_contestStats[ 6 ] = {0}; // Cool, Beauty, Cute, Smart, Tough, Sheen
    u8  m_ribbons1[ 4 ]     = {0};

    u16  m_moves[ 4 ]    = {0};
    u8   m_curPP[ 4 ]    = {0}; //
    u8   m_pPUps         = 0;
    u32  m_iVint         = 0; // hp/5, atk/5, def/5, satk/5, sdef/5, spd/5, nicked/1, isEgg/1
    u8   m_ribbons0[ 4 ] = {0};
    bool m_fateful       = 0; // 1
    bool m_isFemale      = 0; // 1
    bool m_isGenderless  = 0; // 1
    u8   m_altForme      = 0; // 5
    u16  m_hatchPlace    = 0; // PT-like
    u16  m_gotPlace      = 0; // PT-like

    char m_name[ PKMN_NAMELENGTH ] = {0};
    u16  m_hometown                = 0;
    u8   m_ribbons2[ 4 ]           = {0};

    char m_oT[ OTLENGTH ] = {0};
    u8   m_gotDate[ 3 ]   = {0}; //(EGG)
    u8   m_hatchDate[ 3 ] = {0}; // gotDate for nonEgg
    u8   m_pokerus        = 0;   //
    u8   m_ball           = 0;   //
    u8   m_gotLevel       = 0;   // 7
    bool m_oTisFemale     = 0;   // 1
    u8   m_encounter      = 0;
    u8   m_abilitySlot    = 0;

    boxPokemon( ) {
    }
    boxPokemon( u16 p_pkmnId, u16 p_level, u8 p_forme = 0, const char* p_name = 0, u8 p_shiny = 0,
                bool p_hiddenAbility = false, bool p_isEgg = false, u8 p_ball = 0, u8 p_pokerus = 0,
                bool p_fatefulEncounter = false, pkmnData* p_data = nullptr );
    boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
                const char* p_ot, bool p_oTFemale, u8 p_shiny = 0, bool p_hiddenAbility = false,
                bool p_fatefulEncounter = false, bool p_isEgg = false, u16 p_gotPlace = 0,
                u8 p_ball = 0, u8 p_pokerus = 0, u8 p_forme = 0, pkmnData* p_data = nullptr );

    constexpr u16 getSpecies( ) {
        return m_speciesId;
    }
    void setSpecies( u16 p_newSpecies, pkmnData* p_data = 0 );

    constexpr pkmnNatures getNature( )  {
        return ( pkmnNatures )( m_pid % 25 );
    }
    bool setNature( pkmnNatures p_newNature );

    constexpr u16 getAbility( )  {
        return m_ability;
    }
    bool swapAbilities( );
    constexpr bool isShiny( ) {
        return !( ( ( ( m_oTId ^ m_oTSid ) >> 3 )
                    ^ ( ( ( m_pid >> 16 ) ^ ( m_pid % ( 1 << 16 ) ) ) ) >> 3 ) );
    }
    constexpr bool isFemale( )  {
        return m_isFemale;
    }

    /*
     * @brief: Returns the gender. -1 for female, 0 for genderless, and 1 for male
     */
    constexpr s8 gender( ) {
        if( m_isGenderless )
            return 0;
        else if( m_isFemale )
            return -1;
        return 1;
    }

    constexpr u8 EVget( u8 p_i ) {
        return m_effortValues[ p_i ];
    }
    inline void EVset( u8 p_i, u8 p_val ) {
        u16 maxev = 510, curev = 0;
        for( u8 i = 0; i < 6; ++i ) { curev += m_effortValues[ i ]; };
        if( s16( p_val ) - m_effortValues[ p_i ] + curev > maxev ) {
            p_val =  maxev - curev + m_effortValues[ p_i ];
        }
        m_effortValues[ p_i ] = p_val;
    }

    constexpr u8 IVget( u8 p_i ) {
        p_i = 5 - p_i;
        return ( m_iVint >> ( 2 + 5 * p_i ) ) & 31;
    }
    inline void IVset( u8 p_i, u8 p_val ) {
        p_i = 5 - p_i;
        m_iVint &= 0xFFFFFFFF - ( 31 << ( 2 + 5 * p_i ) );
        m_iVint |= ( p_val << ( 2 + 5 * p_i ) );
    }
    constexpr u8 inline PPupget( u8 p_i )  {
        return ( m_pPUps >> ( 2 * p_i ) ) & 3;
    }
    inline void PPupset( u8 p_i, u8 p_val ) {
        m_pPUps &= 0xFF - ( 3 << ( 2 * p_i ) );
        m_pPUps |= ( p_val << ( 2 * p_i ) );
    }
    constexpr u8 getPersonality( ) {
        u8 counter = 1, i = m_pid % 6;

        u8 max = i, maxval = IVget( i );
        for( ; counter < 6; ++counter ) {
            i = ( i + 1 ) % 6;
            if( IVget( i ) > maxval ) {
                maxval = IVget( i );
                max    = i;
            }
        }

        return ( max * 5 ) + ( maxval % 5 );
    }
    constexpr int getTasteStr( ) {
        if( NatMod[ getNature( ) ][ 0 ] == 11 ) return 0;
        if( NatMod[ getNature( ) ][ 1 ] == 11 ) return 1;
        if( NatMod[ getNature( ) ][ 2 ] == 11 ) return 2;
        if( NatMod[ getNature( ) ][ 3 ] == 11 ) return 3;
        if( NatMod[ getNature( ) ][ 4 ] == 11 )
            return 4;
        else
            return 5;
    }
    constexpr u16 getItem( )  {
        return m_heldItem;
    }
    inline void giveItem( u16 p_newItem ) {
        m_heldItem = p_newItem;
        recalculateForme( );
    }
    inline u16 takeItem( ) {
        u16 res    = m_heldItem;
        m_heldItem = 0;
        recalculateForme( );
        return res;
    }

    constexpr type getHPType( ) {
        int a
            = ( ( IVget( 0 ) & 1 ) + 2 * ( IVget( 1 ) & 1 ) + 4 * ( IVget( 2 ) & 1 )
                + 8 * ( IVget( 3 ) & 1 ) + 16 * ( IVget( 4 ) & 1 ) + 32 * ( IVget( 5 ) & 1 ) * 15 )
              / 63;
        return a < 9 ? (type) a : type( a + 1 );
    }
    constexpr u8 getHPPower( )  {
        return 30
               + ( ( ( ( IVget( 0 ) >> 1 ) & 1 ) + 2 * ( ( IVget( 1 ) >> 1 ) & 1 )
                     + 4 * ( ( IVget( 2 ) >> 1 ) & 1 ) + 8 * ( ( IVget( 3 ) >> 1 ) & 1 )
                     + 16 * ( ( IVget( 4 ) >> 1 ) & 1 ) + 32 * ( ( IVget( 5 ) >> 1 ) & 1 ) * 40 )
                   / 63 );
    }
    constexpr bool isEgg( )  {
        return m_iVint & 1;
    }
    void setIsEgg( bool p_val ) {
        if( isEgg( ) == p_val ) return;
        m_iVint ^= 1;
    }
    constexpr bool isNicknamed( )  {
        return m_iVint & 1;
    }
    void setIsNicknamed( bool p_val ) {
        if( isNicknamed( ) == p_val ) return;
        m_iVint ^= 2;
    }

    // Recalculates ability
    void setAbility( u8 p_abilityIdx, pkmnData* p_data = nullptr );
    constexpr u8 getAbilitySlot( ) {
        return m_abilitySlot;
    }

    constexpr u8 getForme( ) {
        return m_altForme;
    }
    inline void setForme( u8 p_newForme ) {
        if( m_altForme != p_newForme ) {
            m_altForme = p_newForme;
            setAbility( getAbilitySlot( ), nullptr );
        }
    }

    bool learnMove( u16 p_move );
    void hatch( );

    // Recalculates form based on held item
    void recalculateForme( );
};
static_assert( std::is_trivially_copyable<boxPokemon>::value, "" );

struct pokemon {
  public:
    boxPokemon m_boxdata;

    union {
        struct {
            u8 m_isAsleep : 3;
            u8 m_isPoisoned : 1;
            u8 m_isBurned : 1;
            u8 m_isFrozen : 1;
            u8 m_isParalyzed : 1;
            u8 m_isBadlyPoisoned : 1;
        } m_status;
        u8 m_statusint;
    };
    u8 m_level : 8;
    u8 m_battleForme : 8;
    struct stats {
        u16 m_curHP : 16; // current HP
        u16 m_maxHP : 16;
        u16 m_Atk : 16;
        u16 m_Def : 16;
        u16 m_SAtk : 16;
        u16 m_SDef : 16;
        u16 m_Spd : 16;

        u16 getStat( u8 p_i ) const {
            switch( p_i ) {
            case 0:
                return m_maxHP;
            case 1:
                return m_Atk;
            case 2:
                return m_Def;
            case 3:
                return m_SAtk;
            case 4:
                return m_SDef;
            case 5:
                return m_Spd;
            }
            return 0;
        }
        void setStat( u8 p_i, u16 p_val ) {
            switch( p_i ) {
            case 0:
                m_maxHP = p_val;
                return;
            case 1:
                m_Atk = p_val;
                return;
            case 2:
                m_Def = p_val;
                return;
            case 3:
                m_SAtk = p_val;
                return;
            case 4:
                m_SDef = p_val;
                return;
            case 5:
                m_Spd = p_val;
                return;
            }
        }
    } m_stats;

    pokemon( ) {
    }
    pokemon( boxPokemon& p_boxPokemon );
    pokemon( trainerPokemon& p_trainerPokemon );
    pokemon( u16 p_pkmnId, u16 p_level, u8 p_forme = 0, const char* p_name = 0, u8 p_shiny = 0,
             bool p_hiddenAbility = false, bool p_isEgg = false, u8 p_ball = 0, u8 p_pokerus = 0,
             bool p_fatefulEncounter = false );
    pokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
             const char* p_ot, bool p_oTFemale, u8 p_shiny = 0, bool p_hiddenAbility = false,
             bool p_fatefulEncounter = false, bool p_isEgg = false, u16 p_gotPlace = 0,
             u8 p_ball = 0, u8 p_pokerus = 0, u8 p_forme = 0 );

    u16 getStat( u8 p_i ) const {
        return m_stats.getStat( p_i );
    }
    void setStat( u8 p_i, u16 p_val ) {
        m_stats.setStat( p_i, p_val );
    }

    /*
     * @brief: Sets the status of the pkmn.
     * @param p_status: 0: sleep, 1: poison, 2: burn, 3: frozen, 4: par, 5: badly poisoned
     * @param p_value: optional value for status (for sleep #rem rounds.
     */
    void setStatus( u8 p_status, u8 p_value = 1 );

    bool        heal( );
    constexpr pkmnNatures getNature( ) {
        return m_boxdata.getNature( );
    }
    bool setNature( pkmnNatures p_newNature );

    constexpr u16 getSpecies( ) {
        return m_boxdata.getSpecies( );
    }
    inline void setSpecies( u16 p_newSpecies ) {
        m_boxdata.setSpecies( p_newSpecies );
        recalculateStats( );
    }

    constexpr u16 getAbility( ) {
        return m_boxdata.getAbility( );
    }
    constexpr bool isShiny( ) {
        return m_boxdata.isShiny( );
    }
    constexpr bool isFemale( ) {
        return m_boxdata.isFemale( );
    }
    constexpr s8 gender( ) {
        return m_boxdata.gender( );
    }
    constexpr unsigned char EVget( u8 p_i ) {
        return m_boxdata.EVget( p_i );
    }
    void inline EVset( u8 p_i, u8 p_val ) {
        m_boxdata.EVset( p_i, p_val );
        recalculateStats( );
    }

    constexpr unsigned char IVget( u8 p_i ) {
        return m_boxdata.IVget( p_i );
    }
    void inline IVset( u8 p_i, u8 p_val ) {
        m_boxdata.IVset( p_i, p_val );
        recalculateStats( );
    }
    constexpr u8 PPupget( u8 p_i ) {
        return m_boxdata.PPupget( p_i );
    }
    inline void PPupset( u8 p_i, u8 p_val ) {
        m_boxdata.PPupset( p_i, p_val );
    }
    constexpr u8 getPersonality( ) {
        return m_boxdata.getPersonality( );
    }
    constexpr int getTasteStr( ) {
        return m_boxdata.getTasteStr( );
    }
    constexpr u16 getItem( ) {
        return m_boxdata.getItem( );
    }
    constexpr type getHPType( )  {
        return m_boxdata.getHPType( );
    }
    constexpr u8 getHPPower( )  {
        return m_boxdata.getHPPower( );
    }
    constexpr u8 getForme( ) {
        return std::max( m_battleForme, m_boxdata.getForme( ) );
    }
    void setForme( u8 p_newForme );
    void setBattleForme( u8 p_newForme );
    void giveItem( u16 p_newItem );
    u16  takeItem( );

    bool canBattleTransform( ) const;
    void battleTransform( );
    void revertBattleTransform( );

    bool setLevel( u8 p_newLevel );
    bool setExperience( u32 p_amount );
    bool gainExperience( u32 p_amount );
    bool swapAbilities( ) {
        return m_boxdata.swapAbilities( );
    }

    void recalculateStats( );
    void recalculateStats( pkmnData& p_data );

    constexpr bool isEgg( )  {
        return m_boxdata.isEgg( );
    }

    bool learnMove( u16 p_move ) {
        return m_boxdata.learnMove( p_move );
    }
    void evolve( u16 p_suppliedItem = 0, u16 p_Trigger = 1 );
    bool canEvolve( u16 p_suppliedItem = 0, u16 p_Trigger = 1 );

    void hatch( ) {
        m_boxdata.hatch( );
    }
};

static_assert( std::is_trivially_copyable<pokemon>::value, "" );

pokemon::stats calcStats( boxPokemon& p_boxdata, const pkmnData* p_data );
pokemon::stats calcStats( boxPokemon& p_boxdata, u8 p_level, const pkmnData* p_data );
u16            calcLevel( boxPokemon& p_boxdata, const pkmnData* p_data );
