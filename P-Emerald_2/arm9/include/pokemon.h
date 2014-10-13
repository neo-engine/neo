/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : pokemon.h
    author      : Philip Wellnitz (RedArceus)
    description : Header file. See corresponding source file for details.

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */

#pragma once

#include <nds.h>
#include <nds\ndstypes.h>
#include <string>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "move.h"
#include "ability.h"
#include "item.h"
#include "berry.h"
#include "saveGame.h"
#include "print.h"
#include "fontData.h"
#include <time.h>
#include <map>

#undef RAND_MAX
#define RAND_MAX 4294967295

#define MAXPKMN 649
#define MAXSTOREDPKMN 800

class berry;

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
extern saveGame SAV;

extern PrintConsole Top, Bottom;
extern font::Font cust_font;
extern font::Font cust_font2;

extern item ItemList[ 700 ];
extern ability abilities[ 164 ];

extern move* AttackList[ 560 ];
extern std::string NatureList[ 25 ];
extern std::string TypeList[ 19 ];
extern std::string PersonalityList[ 30 ];
extern std::map<int, std::string> Locations;

enum Type;//defined in "berry.h"
namespace POKEMON {
    enum pkmnLevelUpType {
        MEDIUM_FAST = 0,
        ERRATIC = 1,
        FLUCTUATING = 2,
        MEDIUM_SLOW = 3,
        FAST = 4,
        SLOW = 5
    };
    enum pkmnColorType {
        RED = 0,
        BLUE = 1,
        YELLOW = 2,
        GREEN = 3,
        BLACK = 4,
        BROWN = 5,
        PURPLE = 6,
        GRAY = 7,
        WHITE = 8,
        PINK = 9
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

    namespace PKMNDATA {
#define GREEN RGB(0,24,0)
#define RED RGB(24,0,0)
#define BLUE RGB(0,0,24)
#define WHITE RGB(31,31,31)
#define NORMAL_ RGB(27,27,27)
#define BLACK RGB(0,0,0)
#define YELLOW RGB(24,24,0)
#define PURPLE RGB(24,0,24)
#define TURQOISE RGB(0,24,24)
#define ICE RGB(15,31,31)
#define FAIRY RGB(31,15,31)
#define GROUND RGB(31,31,15)
#define POISON RGB(31,0,15)
#define ORANGE RGB(31,15,0)
#define GHOST RGB(15,0,31)
#define ROCK RGB(28,23,7)    
#define BUG RGB(15,28,7)    
#define STEEL RGB(24,24,24)
#define DRAGON RGB(7,7,24)
#define UNKNOWN RGB(0,42,42)
        struct pokemonData {
            Type            m_types[ 2 ];
            short           m_bases[ 6 ];
            short           m_catchrate;
            unsigned short  m_items[ 4 ];
            pkmnGenderType  m_gender;
            pkmnEggType     m_eggT[ 2 ];
            short           m_eggcyc;
            short           m_baseFriend;
            short           m_EVYield[ 6 ];
            short           m_EXPYield;
            short           m_formecnt;
            short           m_size;
            short           m_weight;
            short           m_expType;
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


        Type            getType( int p_pkmnId, int p_type );
        short           getBase( int p_pkmnId, int p_base );
        short           getCatchRate( int p_pkmnId );
        const char*     getDisplayName( int p_pkmnId );
        const wchar_t*  getWDisplayName( int p_pkmnId );
        void            getWDisplayName( int p_pkmnId, wchar_t* p_name );
        void            getHoldItems( int p_pkmnId, unsigned short* p_items );
        pkmnGenderType  getGenderType( int p_pkmnId );
        const char*     getSpecies( int p_pkmnId );
        u16             getColor( Type p_type );
        const char*     getDexEntry( int p_pkmnId );
        short           getForme( int p_pkmnId, int p_formeId, const char* p_formeName );
        void            getAll( int p_pkmnId, pokemonData& out );

        void            getLearnMoves( int p_pkmnId, int p_fromLevel, int p_toLevel, int p_mode, int p_num, int* p_res );
        bool            canLearn( int p_pkmnId, int p_moveId, int p_mode );
    }

    extern double NatMod[ 25 ][ 5 ];
    extern char Pkmn_SafariCatchRate[ 669 ];

    extern int EXP[ 100 ][ 13 ];

    bool operator==( touchPosition R, touchPosition L );

    extern unsigned int LastPID;
    extern int page;

    const short OTLENGTH = 8;
    const short PKMN_NAMELENGTH = 11;
    struct pokemon {
    public:
        struct boxPokemon {
            unsigned int            m_PID : 32;
            unsigned short int      m_b1 : 16;
            unsigned short int      m_checksum : 16;
            //BLOCKA{
            unsigned short int      m_SPEC : 16;
            unsigned short int      m_Item : 16;
            unsigned short int      m_ID : 16;
            unsigned short int      m_SID : 16;
            unsigned int            m_exp : 32;
            unsigned char           m_steps : 8; //StepstoHatch/256 // Happiness 
            unsigned char           m_ability : 8;
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
            unsigned char           m_EV[ 6 ];	//HP,Attack,Defense,SAttack,SDefense,Speed
            unsigned char           m_ConStats[ 6 ]; //Cool, Beauty, Cute, Smart, Tough, Sheen
            unsigned char           m_ribbons1[ 4 ];
            //}

            //BLOCKB{
            unsigned short          m_Attack[ 4 ];
            unsigned char           m_AcPP[ 4 ]; //
            union {
                struct {
                    u8 m_Up1 : 2;
                    u8 m_Up2 : 2;
                    u8 m_Up3 : 2;
                    u8 m_Up4 : 2;
                }m_ppup;
                u8 m_PPUps;
            };

            union {
                struct {
                    unsigned char m_HP : 5;
                    unsigned char m_Attack : 5;
                    unsigned char m_Defense : 5;
                    unsigned char m_Speed : 5;
                    unsigned char m_SAttack : 5;
                    unsigned char m_SDefense : 5;
                    bool m_isNicked : 1;
                    bool m_isEgg : 1;
                } m_IV; //HP,Attack,Defense,Speed,SAttack,SDefense
                u32 m_IVint;
            };
            unsigned char           m_ribbons0[ 4 ];
            bool                    m_fateful : 1;
            bool                    m_isFemale : 1;
            bool                    m_isGenderless : 1;
            unsigned char           m_altForme : 5;
            bool                    m_cloned : 1;
            unsigned short int : 15;
            unsigned short          m_hatchPlace : 16; //PT-like
            unsigned short          m_gotPlace : 16; //PT-like
            //}

            //BLOCKC{
            wchar_t                 m_Name[ PKMN_NAMELENGTH ];
            unsigned short          m_hometown : 16;
            unsigned char           m_ribbons2[ 4 ];
            unsigned int : 32;
            //}

            //BLOCKD{
            wchar_t                 m_OT[ OTLENGTH ];
            unsigned char           m_gotDate[ 3 ];//(EGG)
            unsigned char           m_hatchDate[ 3 ]; //gotDate for nonEgg
            unsigned int : 32;
            unsigned char           m_PKRUS : 8;//
            unsigned char           m_Ball : 8;//
            unsigned short int      m_gotLevel : 7;
            bool                    m_OTisFemale : 1;
            enum encounter : char {
                EGG = 0x0,
                GRASS = 0x2,
                DIA_PAL = 0x4,
                CAVE = 0x5,
                SURF = 0x7,
                BUILDING = 0x9,
                SAFZONE = 0xA,
                STARTER = 0xC
            }m_encounter: 8;
            unsigned char m_HGSSBall;
            unsigned char : 8;
            //}

            pkmnNatures getNature( ) {
                return (pkmnNatures)( m_PID % 25 );
            }
            int getAbility( ) {
                return m_ability;
            }
            bool isShiny( );
            bool isCloned( );
            int gender( );

            unsigned inline char IVget( int p_i ) {
                switch( p_i ) {
                    case 0: return m_IV.m_HP;
                    case 1: return m_IV.m_Attack;
                    case 2: return m_IV.m_Defense;
                    case 3: return m_IV.m_Speed;
                    case 4: return m_IV.m_SAttack;
                    case 5: return m_IV.m_SDefense;
                    default: return 0;
                }
            }
            u8 inline PPupget( int p_i ) {
                switch( p_i ) {
                    case 0: return m_ppup.m_Up1;
                    case 1: return m_ppup.m_Up2;
                    case 2: return m_ppup.m_Up3;
                    case 3: return m_ppup.m_Up4;
                    default: return 0;
                }
            }
            int getPersonality( ) {
                int counter = 1, i = m_PID % 6;

                short max = i, maxval = IVget( i );
                for( ; counter < 6; ++counter ) {
                    i = ( i + 1 ) % 6;
                    if( IVget( i ) > maxval ) {
                        maxval = IVget( i );
                        max = i;
                    }
                }

                return ( max * 5 ) + ( maxval % 5 );
            }
            std::string getTasteStr( ) {
                if( NatMod[ getNature( ) ][ 0 ] == 1.2 )
                    return "scharf";
                if( NatMod[ getNature( ) ][ 1 ] == 1.2 )
                    return "saur";
                if( NatMod[ getNature( ) ][ 2 ] == 1.2 )
                    return "s\xFE\xFF";
                if( NatMod[ getNature( ) ][ 3 ] == 1.2 )
                    return "trocken";
                if( NatMod[ getNature( ) ][ 4 ] == 1.2 )
                    return "bitter";
                else
                    return "all";
            }
            int getItem( ) {
                return m_Item;
            }
            Type getHPType( ) {
                int a = ( ( IVget( 0 ) & 1 ) + 2 * ( IVget( 1 ) & 1 ) + 4 * ( IVget( 2 ) & 1 ) + 8 * ( IVget( 3 ) & 1 ) + 16 * ( IVget( 4 ) & 1 ) + 32 * ( IVget( 5 ) & 1 ) * 15 ) / 63;
                return a < 9 ? (Type)a : Type( a + 1 );
            }
            int getHPPower( ) {
                return 30 + ( ( ( ( IVget( 0 ) >> 1 ) & 1 ) + 2 * ( ( IVget( 1 ) >> 1 ) & 1 ) + 4 * ( ( IVget( 2 ) >> 1 ) & 1 ) + 8 * ( ( IVget( 3 ) >> 1 ) & 1 ) + 16 * ( ( IVget( 4 ) >> 1 ) & 1 ) + 32 * ( ( IVget( 5 ) >> 1 ) & 1 ) * 40 ) / 63 );
            }

            boxPokemon( ) { }
            boxPokemon( u16* p_attacks,
                      int p_species,
                      const wchar_t* p_name,
                      short p_level,
                      unsigned short p_id,
                      unsigned short p_sid,
                      const wchar_t* p_ot,
                      bool p_oTFemale,
                      bool p_cloned,
                      bool p_shiny,
                      bool p_hiddenAbility = false,
                      bool p_fatefulEncounter = false,
                      bool p_isEgg = false,
                      short p_gotPlace = 0,
                      char p_ball = 0,
                      char p_pokerus = 0 );

            ~boxPokemon( ) { }

            int draw( ) {
                return 0;
            }

        }m_boxdata;

        union {
            struct {
                unsigned char m_Asleep : 3;
                bool m_Poisoned : 1;
                bool m_Burned : 1;
                bool m_Frozen : 1;
                bool m_Paralyzed : 1;
                bool m_Toxic : 1;
            } m_status;
            u32 m_statusint;
        };
        unsigned char : 8;
        unsigned char : 8;
        unsigned char : 8;
        unsigned char m_Level : 8;
        unsigned char : 8;
        struct stats {
            unsigned short m_acHP : 16;	//Aktuelle HP
            unsigned short m_maxHP : 16;
            unsigned short m_Atk : 16;
            unsigned short m_Def : 16;
            unsigned short m_Spd : 16;
            unsigned short m_SAtk : 16;
            unsigned short m_SDef : 16;
        }m_stats;

        pokemon( ) { }
        pokemon( u16* p_moves,
              int p_species,
              const wchar_t* p_name,
              short p_level,
              unsigned short p_id,
              unsigned short p_sid,
              const wchar_t* p_ot,
              bool p_oTFemale,
              bool p_cloned,
              bool p_shiny,
              bool p_hiddenAbility = false,
              bool p_fatefulEncounter = false,
              bool p_isEgg = false,
              short p_gotPlace = 0,
              char p_ball = 0,
              char p_pokerus = 0 );
        ~pokemon( ) { }

        void evolve( int p_suppliedItem = 0, int p_Trigger = 1 );
        bool canEvolve( int p_suppliedItem = 0, int p_Trigger = 1 );

        int draw( );
        void drawPage( int p_Page, PrintConsole* p_Top, PrintConsole* p_Bottom, bool p_newpok );
    };
}