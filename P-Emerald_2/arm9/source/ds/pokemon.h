/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : pokemon.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2015
    Philip Wellnitz (RedArceus)

    This file is part of Pokémon Emerald 2 Version.

    Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
    */

#pragma once

#include <string>
#include <map>
#include <nds.h>
#include "type.h"

#undef RAND_MAX
#define RAND_MAX 4294967295

#define MAX_PKMN 649

extern std::string NatureList[ 25 ];
extern std::string TypeList[ 19 ];
extern std::string PersonalityList[ 30 ];
extern std::string TasteList[ 6 ];
extern std::map<u16, std::string> Locations;

enum pkmnLevelUpType {
    MEDIUM_FAST = 0,
    ERRATIC = 1,
    FLUCTUATING = 2,
    MEDIUM_SLOW = 3,
    FAST = 4,
    SLOW = 5
};
enum pkmnColorType {
    RED_ = 0,
    BLUE_ = 1,
    YELLOW_ = 2,
    GREEN_ = 3,
    BLACK_ = 4,
    BROWN_ = 5,
    PURPLE_ = 6,
    GRAY_ = 7,
    WHITE_ = 8,
    PINK_ = 9
};
enum pkmnGenderType {
    GENDERLESS = 255,
    FEMALE = 254,
    //   MALE_125=223,
    MALE_250 = 191,
    MALE_500 = 127,
    MALE_750 = 63,
    MALE_875 = 31,
    MALE = 0
};
enum pkmnEggType {
    NONE_ = 0,
    PFLANZE_ = 8,
    KAEFER_ = 4,
    FLUG_ = 5,
    HUMANOTYP_ = 9,
    MINERAL_ = 11,
    AMORPH_ = 12,
    FELD_ = 6,
    WASSER1_ = 2,
    WASSER2_ = 13,
    WASSER3_ = 10,
    MONSTER_ = 1,
    FEE_ = 7,
    DRACHE_ = 15,
    DITTO_ = 14,
    UNBEKANNT_ = 16
};

enum pkmnNatures {
    ROBUST = 0, EINSAM = 1, MUTIG = 2,
    HART = 3, FRECH = 4, KUEHN = 5,
    SANFT = 6, LOCKER = 7, PFIFFIG = 8,
    LASCH = 9, SCHEU = 10, HASTIG = 11,
    ERNST = 12, FROH = 13, NAIV = 14,
    MAESSIG = 15, MILD = 16, RUHIG = 17,
    ZAGHAFT = 18, HITZIG = 19, STILL = 20,
    ZART = 21, FORSCH = 22, SACHT = 23,
    KAUZIG = 24
};

struct pokemonData {
    Type            m_types[ 2 ];
    u16             m_bases[ 6 ];
    u16             m_catchrate;
    u16             m_items[ 4 ];
    pkmnGenderType  m_gender;
    pkmnEggType     m_eggT[ 2 ];
    u16             m_eggcyc;
    u16             m_baseFriend;
    u16             m_EVYield[ 6 ];
    u16             m_EXPYield;
    u16             m_formecnt;
    u16             m_size;
    u16             m_weight;
    u16             m_expType;
    u16             m_abilities[ 4 ];

    union {
        struct evolvestruct {
            s16 m_evolvesInto;
            s16 m_evolutionTrigger;
            s16 m_evolveItem;
            s16 m_evolveLevel;
            s16 m_evolveGender;
            s16 m_evolveLocation;
            s16 m_evolveHeldItem;
            s16 m_evolveDayTime;
            s16 m_evolveKnownMove;
            s16 m_evolveKnownMoveType;
            s16 m_evolveFriendship;
            s16 m_evolveMinimumBeauty;
            s16 m_evolveAtkDefRelation; //1: >; 2: ==; 3 <
            s16 m_evolveAdditionalPartyMember;
            s16 m_evolveTradeSpecies;
        }m_e;
        s16 m_evolveData[ 15 ];
    }               m_evolutions[ 7 ];
};


Type            getType( u16 p_pkmnId, u16 p_type );
u16             getBase( u16 p_pkmnId, u16 p_base );
u16             getCatchRate( u16 p_pkmnId );
const char*     getDisplayName( u16 p_pkmnId );
const wchar_t*  getWDisplayName( u16 p_pkmnId );
void            getWDisplayName( u16 p_pkmnId, wchar_t* p_name );
void            getHoldItems( u16 p_pkmnId, u16* p_items );
pkmnGenderType  getGenderType( u16 p_pkmnId );
const char*     getSpecies( u16 p_pkmnId );
u16             getColor( Type p_type );
const char*     getDexEntry( u16 p_pkmnId );
u16             getForme( u16 p_pkmnId, u16 p_formeId, const char* p_formeName );
void            getAll( u16 p_pkmnId, pokemonData& out );

void            getLearnMoves( u16 p_pkmnId, u16 p_fromLevel, u16 p_toLevel, u16 p_mode, u16 p_num, u16* p_res );
bool            canLearn( u16 p_pkmnId, u16 p_moveId, u16 p_mode );


extern double NatMod[ 25 ][ 5 ];

extern u32 EXP[ 100 ][ 13 ];

extern u32 LastPID;

const u16 OTLENGTH = 8;
const u16 PKMN_NAMELENGTH = 11;
struct pokemon {
public:
    struct boxPokemon {
        u32            m_pid : 32;
        u16      m_b1 : 16;
        u16      m_checksum : 16;
        //BLOCKA{
        u16      m_speciesId : 16;
        u16      m_holdItem : 16;
        u16      m_oTId : 16;
        u16      m_oTSid : 16;
        u32            m_experienceGained : 32;
        u8           m_steps : 8; //StepstoHatch/256 // Happiness 
        u8           m_ability : 8;
        union {
            struct {
                u8 m_circle : 1;
                u8 m_triangle : 1;
                u8 m_square : 1;
                u8 m_heart : 1;
                u8 m_star : 1;
                u8 m_diamond : 1;
                u8 m_xbit : 2; // unused
            } mark;
            u8 m_markings;
        };
        u8                      m_origLang;
        u8           m_effortValues[ 6 ];	//HP,Attack,Defense,SAttack,SDefense,Speed
        u8           m_contestStats[ 6 ]; //Cool, Beauty, Cute, Smart, Tough, Sheen
        u8           m_ribbons1[ 4 ];
        //}

        //BLOCKB{
        u16          m_moves[ 4 ];
        u8           m_acPP[ 4 ]; //
        union {
            struct {
                u8 m_Up1 : 2;
                u8 m_Up2 : 2;
                u8 m_Up3 : 2;
                u8 m_Up4 : 2;
            }m_ppup;
            u8 m_pPUps;
        };

        union {
            struct {
                u8 m_hp : 5;
                u8 m_attack : 5;
                u8 m_defense : 5;
                u8 m_speed : 5;
                u8 m_sAttack : 5;
                u8 m_sDefense : 5;
                bool m_isNicked : 1;
                bool m_isEgg : 1;
            } m_individualValues; //HP,Attack,Defense,Speed,SAttack,SDefense
            u32 m_iVint;
        };
        u8                      m_ribbons0[ 4 ];
        bool                    m_fateful : 1;
        bool                    m_isFemale : 1;
        bool                    m_isGenderless : 1;
        u8                      m_altForme : 5;
        bool                    m_cloned : 1;
u16: 15;
        u16                     m_hatchPlace : 16; //PT-like
        u16                     m_gotPlace : 16; //PT-like
        //}

        //BLOCKC{
        wchar_t                 m_name[ PKMN_NAMELENGTH ];
        u16                     m_hometown : 16;
        u8                      m_ribbons2[ 4 ];
u32: 32;
        //}

        //BLOCKD{
        wchar_t                 m_oT[ OTLENGTH ];
        u8                      m_gotDate[ 3 ];//(EGG)
        u8                      m_hatchDate[ 3 ]; //gotDate for nonEgg
u32: 32;
        u8                      m_pokerus : 8;//
        u8                      m_ball : 8;//
        u16      m_gotLevel : 7;
        bool                    m_oTisFemale : 1;
        enum encounter : char {
            EGG = 0x0,
            GRASS = 0x2,
            DIA_PAL = 0x4,
            CAVE = 0x5,
            SURF = 0x7,
            BUILDING = 0x9,
            SAFZONE = 0xA,
            STARTER = 0xC
        } m_encounter: 8;
        u8           m_HGSSBall;
u8: 8;
        //}

        pkmnNatures             getNature( ) {
            return (pkmnNatures)( m_pid % 25 );
        }
        u16                     getAbility( ) {
            return m_ability;
        }
        bool                    isShiny( );
        bool                    isCloned( );
        s8                      gender( );

        unsigned inline char    IVget( u8 p_i ) {
            switch( p_i ) {
                case 0: return m_individualValues.m_hp;
                case 1: return m_individualValues.m_attack;
                case 2: return m_individualValues.m_defense;
                case 5: return m_individualValues.m_speed;
                case 3: return m_individualValues.m_sAttack;
                case 4: return m_individualValues.m_sDefense;
                default: return 0;
            }
        }
        u8 inline               PPupget( u8 p_i ) {
            switch( p_i ) {
                case 0: return m_ppup.m_Up1;
                case 1: return m_ppup.m_Up2;
                case 2: return m_ppup.m_Up3;
                case 3: return m_ppup.m_Up4;
                default: return 0;
            }
        }
        u8                      getPersonality( ) {
            u8 counter = 1, i = m_pid % 6;

            u8 max = i, maxval = IVget( i );
            for( ; counter < 6; ++counter ) {
                i = ( i + 1 ) % 6;
                if( IVget( i ) > maxval ) {
                    maxval = IVget( i );
                    max = i;
                }
            }

            return ( max * 5 ) + ( maxval % 5 );
        }
        int             getTasteStr( ) {
            if( NatMod[ getNature( ) ][ 0 ] == 1.1 )
                return 0;
            if( NatMod[ getNature( ) ][ 1 ] == 1.1 )
                return 1;
            if( NatMod[ getNature( ) ][ 2 ] == 1.1 )
                return 2;
            if( NatMod[ getNature( ) ][ 3 ] == 1.1 )
                return 3;
            if( NatMod[ getNature( ) ][ 4 ] == 1.1 )
                return 4;
            else
                return 5;
        }
        u16                     getItem( ) {
            return m_holdItem;
        }
        Type                    getHPType( ) {
            int a = ( ( IVget( 0 ) & 1 ) + 2 * ( IVget( 1 ) & 1 ) + 4 * ( IVget( 2 ) & 1 ) + 8 * ( IVget( 3 ) & 1 ) + 16 * ( IVget( 4 ) & 1 ) + 32 * ( IVget( 5 ) & 1 ) * 15 ) / 63;
            return a < 9 ? (Type)a : Type( a + 1 );
        }
        u8                      getHPPower( ) {
            return 30 + ( ( ( ( IVget( 0 ) >> 1 ) & 1 ) + 2 * ( ( IVget( 1 ) >> 1 ) & 1 ) + 4 * ( ( IVget( 2 ) >> 1 ) & 1 ) + 8 * ( ( IVget( 3 ) >> 1 ) & 1 ) + 16 * ( ( IVget( 4 ) >> 1 ) & 1 ) + 32 * ( ( IVget( 5 ) >> 1 ) & 1 ) * 40 ) / 63 );
        }

        boxPokemon( ) { }
        boxPokemon( u16*            p_moves,
                    u16             p_pkmnId,
                    const wchar_t*  p_name,
                    u16             p_level,
                    u16             p_id,
                    u16             p_sid,
                    const wchar_t*  p_ot,
                    bool            p_oTFemale,
                    bool            p_cloned,
                    bool            p_shiny,
                    bool            p_hiddenAbility = false,
                    bool            p_fatefulEncounter = false,
                    bool            p_isEgg = false,
                    u16             p_gotPlace = 0,
                    u8              p_ball = 0,
                    u8              p_pokerus = 0 );

        ~boxPokemon( ) { }
        
    } m_boxdata;

    union {
        struct {
            u8   m_Asleep : 3;
            bool            m_Poisoned : 1;
            bool            m_Burned : 1;
            bool            m_Frozen : 1;
            bool            m_Paralyzed : 1;
            bool            m_Toxic : 1;
        } m_status;
        u32                 m_statusint;
    };
u8: 8;
u8: 8;
u8: 8;
    u8 m_Level : 8;
u8: 8;
    struct stats {
        u16 m_acHP : 16;	//current HP
        u16 m_maxHP : 16;
        u16 m_Atk : 16;
        u16 m_Def : 16;
        u16 m_Spd : 16;
        u16 m_SAtk : 16;
        u16 m_SDef : 16;
    }m_stats;

    pokemon( ) { }
    pokemon( u16*           p_moves,
             u16            p_species,
             const wchar_t* p_name,
             u16            p_level,
             u16            p_id,
             u16            p_sid,
             const wchar_t* p_ot,
             bool           p_oTFemale,
             bool           p_cloned,
             bool           p_shiny,
             bool           p_hiddenAbility = false,
             bool           p_fatefulEncounter = false,
             bool           p_isEgg = false,
             u16            p_gotPlace = 0,
             u8             p_ball = 0,
             u8             p_pokerus = 0 );

    void            evolve( u16 p_suppliedItem = 0, u16 p_Trigger = 1 );
    bool            canEvolve( u16 p_suppliedItem = 0, u16 p_Trigger = 1 );
    
    bool      operator==( const pokemon& p_other ) const;
};
