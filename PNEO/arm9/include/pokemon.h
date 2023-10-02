/*
Pokémon neo
------------------------------

file        : pokemon.h
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
#include <algorithm>
#include <functional>
#include <type_traits>

#include "battle/ability.h"
#include "defines.h"
#include "gen/itemNames.h"
#include "gen/pokemonNames.h"
#include "pokemonData.h"

/*
 * @brief: holds information that is sufficient to construct a behaviourally consistent
 * pkmn.
 */
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
    u8  m_nature;

    constexpr u16 getSpecies( ) const {
        return m_speciesId;
    }

    constexpr u16 getItem( ) const {
        return m_heldItem;
    }
};

/*
 * @brief: stores non-volatile data of a pkmn, i.e., data preserved even when depositing a
 * pkmn to the pkmn storage system.
 */
struct boxPokemon {
    u32 m_pid      = 0;
    u16 m_checksum = 0;

    u16 m_speciesId         = 0;
    u16 m_heldItem          = 0;
    u16 m_oTId              = 0;
    u16 m_oTSid             = 0;
    u32 m_experienceGained  = 0;
    u8  m_steps             = 0; // StepstoHatch/256 // Happiness
    u16 m_ability           = 0;
    u8  m_origLang          = 0;
    u8  m_effortValues[ 6 ] = { 0 }; // HP,Attack,Defense,SAttack,SDefense,Speed
    u8  m_contestStats[ 6 ] = { 0 }; // Cool, Beauty, Cute, Smart, Tough, Sheen
    u8  m_ribbons1[ 4 ]     = { 0 };

    u16 m_moves[ 4 ]       = { 0 };
    u8  m_curPP[ 4 ]       = { 0 }; //
    u8  m_pPUps            = 0;
    u32 m_iVint            = 0; // hp/5, atk/5, def/5, satk/5, sdef/5, spd/5, nicked/1, isEgg/1
    u8  m_ribbons0[ 4 ]    = { 0 };
    u8  m_fateful : 1      = 0;
    u8  m_isFemale : 1     = 0;
    u8  m_isGenderless : 1 = 0;
    u8  m_altForme : 5     = 0;
    u16 m_hatchPlace       = 0; // PT-like
    u16 m_gotPlace         = 0; // PT-like

    char m_name[ PKMN_NAMELENGTH ] = { 0 };
    u16  m_hometown                = 0;
    u8   m_ribbons2[ 4 ]           = { 0 };

    char m_oT[ OTLENGTH ]  = { 0 };
    u8   m_gotDate[ 3 ]    = { 0 }; //(EGG)
    u8   m_hatchDate[ 3 ]  = { 0 }; // gotDate for nonEgg
    u8   m_pokerus         = 0;     //
    u8   m_ball            = 0;     //
    u8   m_gotLevel : 7    = 0;     //
    u8   m_unused : 1      = 0;     // unused
    u8   m_nature          = 0;
    u8   m_abilitySlot : 2 = 0;
    u8   m_shinyType : 2   = 0;
    u8   m_rand : 4        = 0;

    boxPokemon( ) {
    }
    boxPokemon( u16 p_pkmnId, u16 p_level, u8 p_forme = 0, const char* p_name = 0, u8 p_shiny = 0,
                bool p_hiddenAbility = false, bool p_isEgg = false, u8 p_ball = 0, u8 p_pokerus = 0,
                bool p_fatefulEncounter = false, pkmnData* p_data = nullptr );
    boxPokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
                const char* p_ot, u8 p_shiny = 0, bool p_hiddenAbility = false,
                bool p_fatefulEncounter = false, bool p_isEgg = false, u16 p_gotPlace = 0,
                u8 p_ball = 0, u8 p_pokerus = 0, u8 p_forme = 0, pkmnData* p_data = nullptr );

    inline pkmnSpriteInfo getSpriteInfo( bool p_flipX = false ) {
        return { getSpecies( ), getForme( ), isFemale( ), isShiny( ), p_flipX, getPid( ) };
    }

    constexpr u32 getPid( ) const {
        return m_pid;
    }

    std::vector<u8> getRibbons( ) const;

    void awardRibbon( u8 p_ribbon );

    /*
     * @brief: Adds the specified amount of exp. Uses a fast, but very basic overflow
     * protection.
     */
    bool gainExperience( u32 p_amount );

    /*
     * @brief: Returns whether the given pkmn can still evolve.
     */
    bool isFullyEvolved( ) const;

    /*
     * @brief: Returns how likely it is that this pkmn produces an egg with p_other.
     * @returns: 0 (no compatibility) up to 3 (highest compatibility)
     */
    u8 getCompatibility( const boxPokemon& p_other ) const;

    /*
     * @brief: Tries to breed with the specified other pkmn; writes the resulting egg to
     * p_result.
     * @returns: true on success, false on failure.
     */
    bool breed( const boxPokemon& p_other, boxPokemon& p_result ) const;

    /*
     * @brief: Returns the poke ball the pkmn was caught in.
     */
    constexpr u8 getBall( ) const {
        return m_ball;
    }

    constexpr u16 getMove( u8 p_idx ) const {
        return m_moves[ p_idx ];
    }

    constexpr bool wasEgg( ) const {
        return !!m_gotPlace;
    }
    bool isForeign( ) const;

    constexpr u16 getSpecies( ) const {
        return m_speciesId;
    }
    void setSpecies( u16 p_newSpecies, pkmnData* p_data = 0 );

    /*
     * @brief: Returns the species of an offspring of this pkmn. (ignoring incenses and
     * other held items)
     */
    u16 getBaseSpecies( ) const;

    constexpr pkmnNatures getNature( ) const {
        return (pkmnNatures) ( m_nature );
    }
    bool setNature( pkmnNatures p_newNature );

    constexpr u16 getAbility( ) const {
        return m_ability;
    }
    bool           swapAbilities( bool p_toHidden = false );
    constexpr bool isShiny( ) const {
        return !( ( ( ( m_oTId ^ m_oTSid ) >> 3 )
                    ^ ( ( ( m_pid >> 16 ) ^ ( m_pid % ( 1 << 16 ) ) ) ) >> 3 ) );
    }
    constexpr bool isFemale( ) const {
        return m_isFemale;
    }

    /*
     * @brief: Returns the gender. -1 for female, 0 for genderless, and 1 for male
     */
    constexpr s8 gender( ) const {
        if( m_isGenderless )
            return 0;
        else if( m_isFemale )
            return -1;
        return 1;
    }

    constexpr u8 EVget( u8 p_i ) const {
        return m_effortValues[ p_i ];
    }
    inline void EVset( u8 p_i, u8 p_val ) {
        u16 maxev = 510, curev = 0;
        for( u8 i = 0; i < 6; ++i ) { curev += m_effortValues[ i ]; };
        if( s16( p_val ) - m_effortValues[ p_i ] + curev > maxev ) {
            p_val = maxev - curev + m_effortValues[ p_i ];
        }
        m_effortValues[ p_i ] = p_val;
    }

    constexpr u8 IVget( u8 p_i ) const {
        p_i = 5 - p_i;
        return ( m_iVint >> ( 2 + 5 * p_i ) ) & 31;
    }
    inline void IVset( u8 p_i, u8 p_val ) {
        p_i = 5 - p_i;
        m_iVint &= 0xFFFFFFFF - ( 31 << ( 2 + 5 * p_i ) );
        m_iVint |= ( p_val << ( 2 + 5 * p_i ) );
    }
    constexpr u8 inline PPupget( u8 p_i ) const {
        return ( m_pPUps >> ( 2 * p_i ) ) & 3;
    }
    inline void PPupset( u8 p_i, u8 p_val ) {
        m_pPUps &= 0xFF - ( 3 << ( 2 * p_i ) );
        m_pPUps |= ( p_val << ( 2 * p_i ) );
    }
    constexpr u8 getPersonality( ) const {
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
    constexpr int getTasteStr( ) const {
        if( NatMod[ getNature( ) ][ 0 ] == 11 ) return 0;
        if( NatMod[ getNature( ) ][ 1 ] == 11 ) return 1;
        if( NatMod[ getNature( ) ][ 2 ] == 11 ) return 2;
        if( NatMod[ getNature( ) ][ 3 ] == 11 ) return 3;
        if( NatMod[ getNature( ) ][ 4 ] == 11 )
            return 4;
        else
            return 5;
    }
    constexpr u16 getItem( ) const {
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

    constexpr BATTLE::type getHPType( ) const {
        int a
            = ( ( IVget( 0 ) & 1 ) + 2 * ( IVget( 1 ) & 1 ) + 4 * ( IVget( 2 ) & 1 )
                + 8 * ( IVget( 3 ) & 1 ) + 16 * ( IVget( 4 ) & 1 ) + 32 * ( IVget( 5 ) & 1 ) * 15 )
              / 63;
        return a < 8 ? ( BATTLE::type )( a + 1 ) : BATTLE::type( a + 2 );
    }
    constexpr u8 getHPPower( ) const {
        return 30
               + ( ( ( ( IVget( 0 ) >> 1 ) & 1 ) + 2 * ( ( IVget( 1 ) >> 1 ) & 1 )
                     + 4 * ( ( IVget( 2 ) >> 1 ) & 1 ) + 8 * ( ( IVget( 3 ) >> 1 ) & 1 )
                     + 16 * ( ( IVget( 4 ) >> 1 ) & 1 ) + 32 * ( ( IVget( 5 ) >> 1 ) & 1 ) * 40 )
                   / 63 );
    }
    constexpr bool isEgg( ) const {
        return m_iVint & 1;
    }
    void setIsEgg( bool p_val ) {
        if( isEgg( ) == p_val ) return;
        m_iVint ^= 1;
    }
    constexpr bool isNicknamed( ) const {
        return ( m_iVint >> 1 ) & 1;
    }
    void setIsNicknamed( bool p_val ) {
        if( isNicknamed( ) == p_val ) return;
        m_iVint ^= 2;
    }

    // Recalculates ability
    void         setAbility( u8 p_abilityIdx, pkmnData* p_data = nullptr );
    constexpr u8 getAbilitySlot( ) const {
        return m_abilitySlot;
    }

    constexpr u8 getForme( ) const {
        return m_altForme;
    }
    inline void setForme( u8 p_newForme ) {
        if( m_altForme != p_newForme ) {
            m_altForme = p_newForme;
            setAbility( getAbilitySlot( ), nullptr );
        }
    }

    bool learnMove( u16 p_move, std::function<void( const char* )> p_message,
                    std::function<u8( boxPokemon*, u16 )> p_getMove,
                    std::function<bool( const char* )>    p_yesNoMessage );
    void hatch( );

    // Recalculates form based on held item
    void recalculateForme( );
};
static_assert( std::is_trivially_copyable<boxPokemon>::value, "" );

/*
 * @brief: stores boxdata and additionally volatile pkmn data, i.e., data that is lost when
 * depositing the pkmn to the storage system.
 */
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
    u8  m_level : 8;
    u8  m_battleForme : 8;
    u16 m_battleTimeAbility : 16;
    struct stats {
        u16 m_curHP : 16; // current HP
        u16 m_maxHP : 16;
        u16 m_Atk : 16;
        u16 m_Def : 16;
        u16 m_SAtk : 16;
        u16 m_SDef : 16;
        u16 m_Spd : 16;

        constexpr u16 getStat( u8 p_i ) const {
            switch( p_i ) {
            case 0: return m_maxHP;
            case 1: return m_Atk;
            case 2: return m_Def;
            case 3: return m_SAtk;
            case 4: return m_SDef;
            case 5: return m_Spd;
            }
            return 0;
        }
        inline void setStat( u8 p_i, u16 p_val ) {
            switch( p_i ) {
            case 0: m_maxHP = p_val; return;
            case 1: m_Atk = p_val; return;
            case 2: m_Def = p_val; return;
            case 3: m_SAtk = p_val; return;
            case 4: m_SDef = p_val; return;
            case 5: m_Spd = p_val; return;
            }
        }
    } m_stats;

    pokemon( ) {
    }
    pokemon( boxPokemon& p_boxPokemon );
    pokemon( trainerPokemon& p_trainerPokemon );
    pokemon( u16 p_pkmnId, u16 p_level, u8 p_forme = 0, const char* p_name = 0, u8 p_shiny = 0,
             bool p_hiddenAbility = false, bool p_isEgg = false, u8 p_ball = 3, u8 p_pokerus = 0,
             bool p_fatefulEncounter = false );
    pokemon( u16* p_moves, u16 p_pkmnId, const char* p_name, u16 p_level, u16 p_id, u16 p_sid,
             const char* p_ot, u8 p_shiny = 0, bool p_hiddenAbility = false,
             bool p_fatefulEncounter = false, bool p_isEgg = false, u16 p_gotPlace = 0,
             u8 p_ball = 3, u8 p_pokerus = 0, u8 p_forme = 0 );

    inline pkmnSpriteInfo getSpriteInfo( bool p_flipX = false ) {
        return m_boxdata.getSpriteInfo( p_flipX );
    }

    /*
     * @brief: Returns whether the given pkmn can still evolve.
     */
    inline bool isFullyEvolved( ) const {
        return m_boxdata.isFullyEvolved( );
    }

    /*
     * @brief: Returns the poke ball the pkmn was caught in.
     */
    constexpr u8 getBall( ) const {
        return m_boxdata.getBall( );
    }

    /*
     * @brief: Returns whether the pkmn can participate in a battle i.e. is not an egg and
     * is not fainted.
     */
    constexpr bool canBattle( ) const {
        return !isEgg( ) && m_stats.m_curHP;
    }

    constexpr u16 getStat( u8 p_i ) const {
        return m_stats.getStat( p_i );
    }
    inline void setStat( u8 p_i, u16 p_val ) {
        m_stats.setStat( p_i, p_val );
    }

    constexpr u16 getMove( u8 p_idx ) const {
        return m_boxdata.getMove( p_idx );
    }

    /*
     * @brief: Sets the status of the pkmn.
     * @param p_status: 0: sleep, 1: poison, 2: burn, 3: frozen, 4: par, 5: badly poisoned
     * @param p_value: optional value for status (for sleep #rem rounds.
     */
    void setStatus( u8 p_status, u8 p_value = 1 );

    bool                  heal( );
    constexpr pkmnNatures getNature( ) const {
        return m_boxdata.getNature( );
    }
    bool setNature( pkmnNatures p_newNature );

    constexpr bool wasEgg( ) const {
        return m_boxdata.wasEgg( );
    }
    inline bool isForeign( ) const {
        return m_boxdata.isForeign( );
    }

    constexpr u16 getSpecies( ) const {
        return m_boxdata.getSpecies( );
    }

    constexpr u32 getPid( ) const {
        return m_boxdata.getPid( );
    }

    inline void setSpecies( u16 p_newSpecies ) {
        m_boxdata.setSpecies( p_newSpecies );
        recalculateStats( );
    }

    constexpr u16 getAbility( ) const {
        if( m_battleTimeAbility ) {
            return m_battleTimeAbility;
        } else {
            return m_boxdata.getAbility( );
        }
    }
    inline bool setBattleTimeAbility( u16 p_ability ) {
        if( !BATTLE::allowsReplace( getAbility( ) ) ) { return false; }
        m_battleTimeAbility = p_ability;
        return true;
    }
    constexpr bool isShiny( ) const {
        return m_boxdata.isShiny( );
    }
    constexpr bool isFemale( ) const {
        return m_boxdata.isFemale( );
    }
    constexpr s8 gender( ) const {
        return m_boxdata.gender( );
    }
    constexpr unsigned char EVget( u8 p_i ) const {
        return m_boxdata.EVget( p_i );
    }
    void inline EVset( u8 p_i, u8 p_val ) {
        m_boxdata.EVset( p_i, p_val );
        recalculateStats( );
    }

    constexpr unsigned char IVget( u8 p_i ) const {
        return m_boxdata.IVget( p_i );
    }
    void inline IVset( u8 p_i, u8 p_val ) {
        m_boxdata.IVset( p_i, p_val );
        recalculateStats( );
    }
    constexpr u8 PPupget( u8 p_i ) const {
        return m_boxdata.PPupget( p_i );
    }
    inline void PPupset( u8 p_i, u8 p_val ) {
        m_boxdata.PPupset( p_i, p_val );
    }
    constexpr u8 getPersonality( ) const {
        return m_boxdata.getPersonality( );
    }
    constexpr int getTasteStr( ) const {
        return m_boxdata.getTasteStr( );
    }
    constexpr u16 getItem( ) const {
        return m_boxdata.getItem( );
    }
    constexpr BATTLE::type getHPType( ) const {
        return m_boxdata.getHPType( );
    }
    constexpr u8 getHPPower( ) const {
        return m_boxdata.getHPPower( );
    }
    constexpr u8 getForme( ) const {
        return std::max( m_battleForme, m_boxdata.getForme( ) );
    }
    void setForme( u8 p_newForme );
    void setBattleForme( u8 p_newForme );
    void giveItem( u16 p_newItem );
    u16  takeItem( );

    constexpr bool canBattleTransform( ) const {
        if( getForme( ) ) { return false; }
        switch( getSpecies( ) ) {
        case PKMN_VENUSAUR: return getItem( ) == I_VENUSAURITE;
        case PKMN_CHARIZARD:
            return getItem( ) == I_CHARIZARDITE_X || getItem( ) == I_CHARIZARDITE_Y;
        case PKMN_BLASTOISE: return getItem( ) == I_BLASTOISINITE;
        case PKMN_ALAKAZAM: return getItem( ) == I_ALAKAZITE;
        case PKMN_GENGAR: return getItem( ) == I_GENGARITE;
        case PKMN_KANGASKHAN: return getItem( ) == I_KANGASKHANITE;
        case PKMN_PINSIR: return getItem( ) == I_PINSIRITE;
        case PKMN_GYARADOS: return getItem( ) == I_GYARADOSITE;
        case PKMN_AERODACTYL: return getItem( ) == I_AERODACTYLITE;
        case PKMN_MEWTWO: return getItem( ) == I_MEWTWONITE_X || getItem( ) == I_MEWTWONITE_Y;
        case PKMN_AMPHAROS: return getItem( ) == I_AMPHAROSITE;
        case PKMN_SCIZOR: return getItem( ) == I_SCIZORITE;
        case PKMN_HERACROSS: return getItem( ) == I_HERACRONITE;
        case PKMN_HOUNDOOM: return getItem( ) == I_HOUNDOOMINITE;
        case PKMN_TYRANITAR: return getItem( ) == I_TYRANITARITE;
        case PKMN_BLAZIKEN: return getItem( ) == I_BLAZIKENITE;
        case PKMN_GARDEVOIR: return getItem( ) == I_GARDEVOIRITE;
        case PKMN_MAWILE: return getItem( ) == I_MAWILITE;
        case PKMN_AGGRON: return getItem( ) == I_AGGRONITE;
        case PKMN_MEDICHAM: return getItem( ) == I_MEDICHAMITE;
        case PKMN_MANECTRIC: return getItem( ) == I_MANECTITE;
        case PKMN_BANETTE: return getItem( ) == I_BANETTITE;
        case PKMN_ABSOL: return getItem( ) == I_ABSOLITE;
        case PKMN_GARCHOMP: return getItem( ) == I_GARCHOMPITE;
        case PKMN_LUCARIO: return getItem( ) == I_LUCARIONITE;
        case PKMN_ABOMASNOW: return getItem( ) == I_ABOMASITE;
        case PKMN_BEEDRILL: return getItem( ) == I_BEEDRILLITE;
        case PKMN_PIDGEOT: return getItem( ) == I_PIDGEOTITE;
        case PKMN_SLOWBRO: return getItem( ) == I_SLOWBRONITE;
        case PKMN_STEELIX: return getItem( ) == I_STEELIXITE;
        case PKMN_SCEPTILE: return getItem( ) == I_SCEPTILITE;
        case PKMN_SWAMPERT: return getItem( ) == I_SWAMPERTITE;
        case PKMN_SABLEYE: return getItem( ) == I_SABLENITE;
        case PKMN_SHARPEDO: return getItem( ) == I_SHARPEDONITE;
        case PKMN_CAMERUPT: return getItem( ) == I_CAMERUPTITE;
        case PKMN_ALTARIA: return getItem( ) == I_ALTARIANITE;
        case PKMN_GLALIE: return getItem( ) == I_GLALITITE;
        case PKMN_SALAMENCE: return getItem( ) == I_SALAMENCITE;
        case PKMN_METAGROSS: return getItem( ) == I_METAGROSSITE;
        case PKMN_LATIAS: return getItem( ) == I_LATIASITE;
        case PKMN_LATIOS: return getItem( ) == I_LATIOSITE;
        case PKMN_RAYQUAZA: return getItem( ) == I_JADE_ORB;
        case PKMN_LOPUNNY: return getItem( ) == I_LOPUNNITE;
        case PKMN_GALLADE: return getItem( ) == I_GALLADITE;
        case PKMN_AUDINO: return getItem( ) == I_AUDINITE;
        case PKMN_DIANCIE: return getItem( ) == I_DIANCITE;
        case PKMN_KYOGRE: return getItem( ) == I_BLUE_ORB;
        case PKMN_GROUDON:
            return getItem( ) == I_RED_ORB;
        [[likely]] default:
            return false;
        }
    }

    void battleTransform( );
    void revertBattleTransform( );

    bool        setLevel( u8 p_newLevel );
    bool        setExperience( u32 p_amount );
    bool        gainExperience( u32 p_amount );
    inline bool swapAbilities( bool p_toHidden = false ) {
        return m_boxdata.swapAbilities( p_toHidden );
    }

    void recalculateStats( );
    void recalculateStats( pkmnData& p_data );

    constexpr bool isEgg( ) const {
        return m_boxdata.isEgg( );
    }

    inline bool learnMove( u16 p_move, std::function<void( const char* )> p_message,
                           std::function<u8( boxPokemon*, u16 )> p_getMove,
                           std::function<bool( const char* )>    p_yesNoMessage ) {
        return m_boxdata.learnMove( p_move, p_message, p_getMove, p_yesNoMessage );
    }
    void evolve( u16 p_suppliedItem = 0, evolutionMethod p_method = EVOMETHOD_LEVEL_UP );
    u8   canEvolve( u16 p_suppliedItem = 0, evolutionMethod p_method = EVOMETHOD_LEVEL_UP,
                    pkmnEvolveData* p_edata = nullptr );

    inline void hatch( ) {
        m_boxdata.hatch( );
    }
};

static_assert( std::is_trivially_copyable<pokemon>::value, "" );

pokemon::stats calcStats( boxPokemon& p_boxdata, const pkmnData* p_data );
pokemon::stats calcStats( boxPokemon& p_boxdata, u8 p_level, const pkmnData* p_data );
u16            calcLevel( boxPokemon& p_boxdata, const pkmnData* p_data );
