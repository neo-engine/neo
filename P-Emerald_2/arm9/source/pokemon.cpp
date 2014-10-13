/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : pokemon.cpp
    author      : Philip Wellnitz (RedArceus)
    description : The main Pokémon engine

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


#include "ability.h"
#include "item.h"
#include "berry.h"
#include "move.h"

#include "pokemon.h"
#include "battle.h"

#include "sprite.h"
#include "saveGame.h"
#include "messageBox.h"
#include "screenLoader.h"

#include "print.h"

#include <nds.h>
#include <vector>

extern std::string readString( FILE*, bool p_new = false );
extern std::wstring readWString( FILE*, bool p_new = false );

extern move* AttackList[ 560 ];

extern SpriteInfo spriteInfo[ SPRITE_COUNT ];
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];
extern OAMTable *oam, *oamTop;
extern SavMod savMod;

namespace POKEMON {
    std::string NatureList[ 25 ] = {
        "robust", "einsam", "mutig", "hart",
        "frech", "kühn", "sanft", "locker",
        "pfiffig", "lasch", "scheu", "hastig",
        "ernst", "froh", "naiv", "mäßig",
        "mild", "ruhig", "zaghaft", "hitzig",
        "still", "zart", "forsch", "sacht",
        "kauzig"
    };
    std::string PersonalityList[ 30 ] = {
        "liebt es, zu essen", "nickt oft ein", "schläft gerne", "macht oft Unordnung", "liebt es, zu entspannen",
        "ist stolz auf seine Stärke", "prügelt sich gerne", "besitzt Temperament", "liebt es, zu kämpfen", "ist impulsiv",
        "hat einen robusten Körper", "kann Treffer gut verkraften", "ist äußerst ausdauernd", "hat eine gute Ausdauer", "ist beharrlich",
        "ist sehr neugierig", "ist hinterhältig", "ist äußerst gerissen", "ist oft in Gedanken", "ist sehr pedantisch",
        "besitzt starken Willen", "ist irgendwie eitel", "ist sehr aufsässig", "hasst Niederlagen", "ist dickköpfig",
        "liebt es, zu rennen", "achtet auf Geräusche", "ist ungestüm und einfältig", "ist fast wie eine Clown", "flüchtet schnell"
    };

    const char* getLoc( int p_ind ) {
        if( p_ind < 0 || p_ind > 5000 )
            return "Entfernter Ort";
        char buf[ 50 ];
        sprintf( buf, "nitro:/LOCATIONS/%i.data", p_ind );
        FILE* f = fopen( buf, "r" );
        if( f == 0 ) {
            fclose( f );
            if( savMod == SavMod::_NDS && p_ind > 322 && p_ind < 1000 )
                return getLoc( 3002 );

            return "Entfernter Ort";
        }
        std::string ret = readString( f );
        fclose( f );
        return ret.c_str( );
    }

    std::string Games[ 10 ] = { };

    double NatMod[ 25 ][ 5 ] = {
            { 1.0, 1.0, 1.0, 1.0, 1.0 },
            { 1.2, 0.8, 1.0, 1.0, 1.0 },
            { 1.2, 1.0, 0.8, 1.0, 1.0 },
            { 1.2, 1.0, 1.0, 0.8, 1.0 },
            { 1.2, 1.0, 1.0, 1.0, 0.8 },
            { 0.8, 1.2, 1.0, 1.0, 1.0 },
            { 1.0, 1.0, 1.0, 1.0, 1.0 },
            { 1.0, 1.2, 0.8, 1.0, 1.0 },
            { 1.0, 1.2, 1.0, 0.8, 1.0 },
            { 1.0, 1.2, 1.0, 1.0, 0.8 },
            { 0.8, 1.0, 1.2, 1.0, 1.0 },
            { 1.0, 0.8, 1.2, 1.0, 1.0 },
            { 1.0, 1.0, 1.0, 1.0, 1.0 },
            { 1.0, 1.0, 1.2, 0.8, 1.0 },
            { 1.0, 1.0, 1.2, 1.0, 0.8 },
            { 0.8, 1.0, 1.0, 1.2, 1.0 },
            { 1.0, 0.8, 1.0, 1.2, 1.0 },
            { 1.0, 1.0, 0.8, 1.2, 1.0 },
            { 1.0, 1.0, 1.0, 1.0, 1.0 },
            { 1.0, 1.0, 1.0, 1.2, 0.8 },
            { 0.8, 1.0, 1.0, 1.0, 1.2 },
            { 1.0, 0.8, 1.0, 1.0, 1.2 },
            { 1.0, 1.0, 0.8, 1.0, 1.2 },
            { 1.0, 1.0, 1.0, 0.8, 1.2 },
            { 1.0, 1.0, 1.0, 1.0, 1.0 }
    };

    namespace PKMNDATA {
        const char PKMNDATA_PATH[ ] = "nitro:/PKMNDATA/";
        Type getType( int p_pkmnId, int p_type ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return UNBEKANNT;
            }
            char buf[ 12 ];
            fscanf( f, "%s", buf );
            fclose( f );
            return (Type)( buf[ p_type ] - 42 );
        }
        short getBase( int p_pkmnId, int p_base ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return 0;
            }
            char buf[ 12 ];
            fscanf( f, "%s", buf );
            fclose( f );
            return (short)buf[ 2 + p_base ];
        }
        short getCatchRate( int p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return 0;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fclose( f );
            return buf;
        }
        const char* getDisplayName( int p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return "???";
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            std::string ret = readString( f, true );
            fclose( f );
            return ret.c_str( );
        }
        const wchar_t* getWDisplayName( int p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return L"???";
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            std::wstring ret = readWString( f );
            fclose( f );
            return ret.c_str( );
        }
        void getWDisplayName( int p_pkmnId, wchar_t* p_name ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                wcscpy( p_name, L"???" );
                return;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            std::wstring ret = readWString( f );
            fclose( f );
            wcscpy( p_name, ret.c_str( ) );
        }
        void getHoldItems( int p_pkmnId, unsigned short* p_items ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                p_items[ 0 ] = p_items[ 1 ] = p_items[ 2 ] = p_items[ 3 ] = 0;
                return;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            readString( f );
            p_items[ 0 ] = p_items[ 1 ] = p_items[ 2 ] = p_items[ 3 ] = 0;
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &p_items[ i ] );
            fclose( f );
            return;
        }
        pkmnGenderType getGenderType( int p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return (pkmnGenderType)0;
            }
            for( int i = 0; i < 8; ++i )
                fgetc( f );
            short buf; fscanf( f, "%hi", &buf );
            fgetc( f );
            readString( f );
            for( int i = 0; i < 5; ++i )
                fscanf( f, "%hi", &buf );
            fclose( f );
            return (pkmnGenderType)buf;
        }
        const char* getDexEntry( int p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return "Keine Daten vorhanden.";
            }
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            for( int i = 2; i < 8; ++i )
                fgetc( f );
            short s;
            fscanf( f, "%hi", &s );
            fgetc( f );
            readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &s );
            fscanf( f, "%hi", &s );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            fgetc( f );
            fgetc( f );
            for( int i = 0; i < 6; ++i )
                fgetc( f );
            fscanf( f, "%hi", &s );
            fgetc( f );
            std::string ret = readString( f );
            fclose( f );
            return ret.c_str( );
        }
        short getForme( int p_pkmnId, int p_formeId, const char* p_retFormeName ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return p_pkmnId;
            }
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            for( int i = 2; i < 8; ++i )
                fgetc( f );
            short s;
            fscanf( f, "%hi", &s );
            fgetc( f );
            readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &s );
            fscanf( f, "%hi", &s );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            fgetc( f );
            fgetc( f );
            for( int i = 0; i < 6; ++i )
                fgetc( f );
            fscanf( f, "%hi", &s );
            fgetc( f );
            readString( f );
            fscanf( f, "%hi", &s );
            short d; std::string ret;
            for( int i = 0; i <= std::min( (int)s, p_formeId ); ++i ) {
                fscanf( f, "%hi", &d ); ret = readString( f );
            }
            //fscanf(f,"%hi",d); 
            //ret = readString(f);
            fclose( f );
            p_retFormeName = ret.c_str( );
            return d;
        }
        const char* getSpecies( int p_pkmnId ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return "UNBEKANNT";
            }
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            for( int i = 2; i < 8; ++i )
                fgetc( f );
            short s;
            fscanf( f, "%hi", &s );
            fgetc( f );
            readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &s );
            fscanf( f, "%hi", &s );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                fgetc( f );
            fgetc( f );
            fgetc( f );
            for( int i = 0; i < 6; ++i )
                fgetc( f );
            fscanf( f, "%hi", &s );
            fgetc( f );
            readString( f );
            fscanf( f, "%hi", &s );
            short d; std::string ret2;
            for( int i = 0; i < s; ++i ) {
                fscanf( f, "%hi", &d ); readString( f );
            }
            fscanf( f, " " );
            ret2 = readString( f, true );
            fclose( f );
            return ret2.c_str( );
        }

        void getAll( int p_pkmnId, pokemonData& p_out ) {
            char pt[ 100 ];
            sprintf( pt, "%s%d.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );

            if( f == 0 ) {
                fclose( f );
                return;
            }
            for( int i = 0; i < 2; ++i )
                p_out.m_types[ i ] = (Type)( ( fgetc( f ) ) - 42 );
            for( int i = 2; i < 8; ++i )
                p_out.m_bases[ i - 2 ] = (short)fgetc( f );
            fscanf( f, "%hi", &p_out.m_catchrate );
            fgetc( f );
            readString( f );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hi", &p_out.m_items[ i ] );
            fscanf( f, "%hi", &p_out.m_gender );
            fgetc( f );
            for( int i = 0; i < 2; ++i )
                p_out.m_eggT[ i ] = (pkmnEggType)( ( fgetc( f ) ) - 42 );
            p_out.m_eggcyc = ( ( fgetc( f ) ) - 42 );
            p_out.m_baseFriend = ( ( fgetc( f ) ) - 42 );
            for( int i = 0; i < 6; ++i )
                p_out.m_EVYield[ i ] = ( ( fgetc( f ) ) - 42 );
            fscanf( f, "%hi", &p_out.m_EXPYield );
            fgetc( f );
            readString( f );
            fscanf( f, "%hi", &p_out.m_formecnt );
            short d;
            for( int i = 0; i < p_out.m_formecnt; ++i ) {
                fscanf( f, "%hi", &d ); readString( f );
            }
            fscanf( f, " " );
            readString( f, true );
            fscanf( f, "%hi", &p_out.m_size );
            fscanf( f, "%hi", &p_out.m_weight );
            fscanf( f, "%hi", &p_out.m_expType );
            for( int i = 0; i < 4; ++i )
                fscanf( f, "%hu ", &p_out.m_abilities[ i ] );
            for( int i = 0; i < 7; ++i )
                for( int j = 0; j < 15; ++j )
                    fscanf( f, "%hi ", &( p_out.m_evolutions[ i ].m_evolveData[ j ] ) );
            fclose( f );
            return;
        }

        void getLearnMoves( int p_pkmnId, int p_fromLevel, int p_toLevel, int p_mode, int p_amount, u16* p_result ) {

            char pt[ 150 ];
            sprintf( pt, "%s/LEARNSETS/%d.learnset.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );
            if( !f )
                return;

            int rescnt = 0;

            if( p_fromLevel > p_toLevel ) {
                std::vector<u16> reses;
                for( int i = 0; i <= p_fromLevel; ++i ) {
                    int z; fscanf( f, "%d", &z );
                    for( int j = 0; j < z; ++j ) {
                        u16 g, h;
                        fscanf( f, "%hd %hd", &g, &h );
                        if( i >= p_toLevel && h == (u16)p_mode && g < MAXATTACK )
                            reses.push_back( g );
                    }
                }
                auto I = reses.rbegin( );
                for( int i = 0; i < p_amount && I != reses.rend( ); ++i, ++I ) {
                    for( int z = 0; z < i; ++z )
                        if( *I == p_result[ z ] ) {
                        --i;
                        goto N;
                        }
                    p_result[ i ] = *I;
N:
                    ;
                }
                fclose( f );
                return;
            } else {
                for( int i = 0; i <= p_toLevel; ++i ) {
                    int z; fscanf( f, "%d", &z );
                    for( int j = 0; j < z; ++j ) {
                        u16 g, h;
                        fscanf( f, "%hd %hd", &g, &h );
                        if( i >= p_fromLevel && h == p_mode && g < MAXATTACK ) {
                            for( int z = 0; z < rescnt; ++z )
                                if( g == p_result[ z ] )
                                    goto NEXT;
                            p_result[ rescnt ] = g;
                            if( ++rescnt == p_amount )
                                return;
NEXT:
                            ;
                        }
                    }
                }
            }
            fclose( f );
        }
        bool canLearn( int p_pkmnId, int p_moveId, int p_mode ) {
            char pt[ 150 ];
            sprintf( pt, "%s/LEARNSETS/%d.learnset.data", PKMNDATA_PATH, p_pkmnId );
            FILE* f = fopen( pt, "r" );
            if( !f )
                return false;

            for( int i = 0; i <= 100; ++i ) {
                int z; fscanf( f, "%d", &z );
                for( int j = 0; j < z; ++j ) {
                    u16 g, h;
                    fscanf( f, "%hd %hd", &g, &h );
                    if( g == p_moveId && h == p_mode )
                        return true;
                }
            }
            return false;
        }

        u16 getColor( Type p_type ) {
            switch( p_type ) {
                case NORMAL:
                    return NORMAL_;
                    break;
                case KAMPF:
                    return RED;
                    break;
                case FLUG:
                    return TURQOISE;
                    break;
                case GIFT:
                    return POISON;
                    break;
                case BODEN:
                    return GROUND;
                    break;
                case GESTEIN:
                    return ROCK;
                    break;
                case KAEFER:
                    return BUG;
                    break;
                case GEIST:
                    return GHOST;
                    break;
                case STAHL:
                    return STEEL;
                    break;
                case UNBEKANNT:
                    return UNKNOWN;
                    break;
                case WASSER:
                    return BLUE;
                    break;
                case FEUER:
                    return ORANGE;
                    break;
                case PFLANZE:
                    return GREEN;
                    break;
                case ELEKTRO:
                    return YELLOW;
                    break;
                case PSYCHO:
                    return PURPLE;
                    break;
                case EIS:
                    return ICE;
                    break;
                case DRACHE:
                    return DRAGON;
                    break;
                case UNLICHT:
                    return BLACK;
                    break;
                case FEE:
                    return FAIRY;
                    break;
                default:
                    return DRAGON;
                    break;
            }
            return WHITE;
        }
    }

    char Pkmn_SafariCatchRate[ 669 ];

    int EXP[ 100 ][ 13 ] = {
            { 0, 0, 0, 0, 0, 0, 1, 15, 6, 8, 9, 10, 4 },
            { 15, 6, 8, 9, 10, 4, 2, 37, 15, 19, 48, 23, 9 },
            { 52, 21, 27, 57, 33, 13, 3, 70, 30, 37, 39, 47, 19 },
            { 122, 51, 64, 96, 80, 32, 4, 115, 49, 61, 39, 76, 33 },
            { 237, 100, 125, 135, 156, 65, 5, 169, 72, 91, 44, 114, 47 },
            { 406, 172, 216, 179, 270, 112, 6, 231, 102, 127, 57, 158, 66 },
            { 637, 274, 343, 236, 428, 178, 7, 305, 135, 169, 78, 212, 98 },
            { 942, 409, 512, 314, 640, 276, 8, 384, 174, 217, 105, 271, 117 },
            { 1326, 583, 729, 419, 911, 393, 9, 474, 217, 271, 141, 339, 147 },
            { 1800, 800, 1000, 560, 1250, 540, 10, 569, 264, 331, 182, 413, 205 },
            { 2369, 1064, 1331, 742, 1663, 745, 11, 672, 318, 397, 231, 497, 222 },
            { 3041, 1382, 1728, 973, 2160, 967, 12, 781, 375, 469, 288, 586, 263 },
            { 3822, 1757, 2197, 1261, 2746, 1230, 13, 897, 438, 547, 351, 684, 361 },
            { 4719, 2195, 2744, 1612, 3430, 1591, 14, 1018, 505, 631, 423, 788, 366 },
            { 5737, 2700, 3375, 2035, 4218, 1957, 15, 1144, 576, 721, 500, 902, 500 },
            { 6881, 3276, 4096, 2535, 5120, 2457, 16, 1274, 654, 817, 585, 1021, 589 },
            { 8155, 3930, 4913, 3120, 6141, 3046, 17, 1409, 735, 919, 678, 1149, 686 },
            { 9564, 4665, 5832, 3798, 7290, 3732, 18, 1547, 822, 1027, 777, 1283, 794 },
            { 11111, 5487, 6859, 4575, 8573, 4526, 19, 1689, 913, 1141, 885, 1427, 914 },
            { 12800, 6400, 8000, 5460, 10000, 5440, 20, 1832, 1008, 1261, 998, 1576, 1042 },
            { 14632, 7408, 9261, 6458, 11576, 6482, 21, 1978, 1110, 1387, 1119, 1734, 1184 },
            { 16610, 8518, 10648, 7577, 13310, 7666, 22, 2127, 1215, 1519, 1248, 1898, 1337 },
            { 18737, 9733, 12167, 8825, 15208, 9003, 23, 2275, 1326, 1657, 1383, 2072, 1503 },
            { 21012, 11059, 13824, 10208, 17280, 10506, 24, 2425, 1441, 1801, 1527, 2251, 1681 },
            { 23437, 12500, 15625, 11735, 19531, 12187, 25, 2575, 1560, 1951, 1676, 2439, 1873 },
            { 26012, 14060, 17576, 13411, 21970, 14060, 26, 2725, 1686, 2107, 1833, 2633, 2080 },
            { 28737, 15746, 19683, 15244, 24603, 16140, 27, 2873, 1815, 2269, 1998, 2837, 2299 },
            { 31610, 17561, 21952, 17242, 27440, 18439, 28, 3022, 1950, 2437, 2169, 3046, 2535 },
            { 34632, 19511, 24389, 19411, 30486, 20974, 29, 3168, 2089, 2611, 2349, 3264, 2786 },
            { 37800, 21600, 27000, 21760, 33750, 23760, 30, 3311, 2232, 2791, 2534, 3488, 3051 },
            { 41111, 23832, 29791, 24294, 37238, 26811, 31, 3453, 2382, 2977, 2727, 3722, 3335 },
            { 44564, 26214, 32768, 27021, 40960, 30146, 32, 3591, 2535, 3169, 2928, 3961, 3634 },
            { 48155, 28749, 35937, 29949, 44921, 33780, 33, 3726, 2694, 3367, 3135, 4209, 3951 },
            { 51881, 31443, 39304, 33084, 49130, 37731, 34, 3856, 2857, 3571, 3351, 4463, 4286 },
            { 55737, 34300, 42875, 36435, 53593, 42017, 35, 3982, 3024, 3781, 3572, 4727, 4639 },
            { 59719, 37324, 46656, 40007, 58320, 46656, 36, 4103, 3198, 3997, 3801, 4996, 3997 },
            { 63822, 40522, 50653, 43808, 63316, 50653, 37, 4219, 3375, 4219, 4038, 5274, 5316 },
            { 68041, 43897, 54872, 47846, 68590, 55969, 38, 4328, 3558, 4447, 4281, 5558, 4536 },
            { 72369, 47455, 59319, 52127, 74148, 60505, 39, 4431, 3745, 4681, 4533, 5852, 6055 },
            { 76800, 51200, 64000, 56660, 80000, 66560, 40, 4526, 3936, 4921, 4790, 6151, 5117 },
            { 81326, 55136, 68921, 61450, 86151, 71677, 41, 4616, 4134, 5167, 5055, 6459, 6856 },
            { 85942, 59270, 74088, 66505, 92610, 78533, 42, 4695, 4335, 5419, 5328, 6773, 5744 },
            { 90637, 63605, 79507, 71833, 99383, 84277, 43, 4769, 4542, 5677, 5607, 7097, 7721 },
            { 95406, 68147, 85184, 77440, 106480, 91998, 44, 4831, 4753, 5941, 5895, 7426, 6417 },
            { 100237, 72900, 91125, 83335, 113906, 98415, 45, 4885, 4968, 6211, 6188, 7764, 8654 },
            { 105122, 77868, 97336, 89523, 121670, 107069, 46, 4930, 5190, 6487, 6489, 8108, 7136 },
            { 110052, 83058, 103823, 96012, 129778, 114205, 47, 4963, 5415, 6769, 6798, 8462, 9658 },
            { 115015, 88473, 110592, 102810, 138240, 123863, 48, 4986, 5646, 7057, 7113, 8821, 7903 },
            { 120001, 94119, 117649, 109923, 147061, 131766, 49, 4999, 5881, 7351, 7437, 9189, 10734 },
            { 125000, 100000, 125000, 117360, 156250, 142500, 50, 6324, 6120, 7651, 7766, 9563, 8722 },
            { 131324, 106120, 132651, 125126, 165813, 151222, 51, 6471, 6366, 7957, 8103, 9947, 11883 },
            { 137795, 112486, 140608, 133229, 175760, 163105, 52, 6615, 6615, 8269, 8448, 10336, 9592 },
            { 144410, 119101, 148877, 141677, 186096, 172697, 53, 6755, 6870, 8587, 8799, 10734, 13110 },
            { 151165, 125971, 157464, 150476, 196830, 185807, 54, 6891, 7129, 8911, 9159, 11138, 10515 },
            { 158056, 133100, 166375, 159635, 207968, 196322, 55, 7023, 7392, 9241, 9524, 11552, 14417 },
            { 165079, 140492, 175616, 169159, 219520, 210739, 56, 7150, 7662, 9577, 9897, 11971, 11492 },
            { 172229, 148154, 185193, 179056, 231491, 222231, 57, 7274, 7935, 9919, 10278, 12399, 15805 },
            { 179503, 156089, 195112, 189334, 243890, 238036, 58, 7391, 8214, 10267, 10665, 12833, 12526 },
            { 186894, 164303, 205379, 199999, 256723, 250562, 59, 7506, 8497, 10621, 11061, 13277, 17278 },
            { 194400, 172800, 216000, 211060, 270000, 267840, 60, 7613, 8784, 10981, 11462, 13726, 13616 },
            { 202013, 181584, 226981, 222522, 283726, 281456, 61, 7715, 9078, 11347, 11871, 14184, 18837 },
            { 209728, 190662, 238328, 234393, 297910, 300293, 62, 7812, 9375, 11719, 12288, 14648, 14766 },
            { 217540, 200037, 250047, 246681, 312558, 315059, 63, 7903, 9678, 12097, 12711, 15122, 20485 },
            { 225443, 209715, 262144, 259392, 327680, 335544, 64, 7988, 9985, 12481, 13143, 15601, 15976 },
            { 233431, 219700, 274625, 272535, 343281, 351520, 65, 8065, 10296, 12871, 13580, 16089, 22224 },
            { 241496, 229996, 287496, 286115, 359370, 373744, 66, 8137, 10614, 13267, 14025, 16583, 17247 },
            { 249633, 240610, 300763, 300140, 375953, 390991, 67, 8201, 10935, 13669, 14478, 17087, 24059 },
            { 257834, 251545, 314432, 314618, 393040, 415050, 68, 9572, 11262, 14077, 14937, 17596, 18581 },
            { 267406, 262807, 328509, 329555, 410636, 433631, 69, 9052, 11593, 14491, 15405, 18114, 25989 },
            { 276458, 274400, 343000, 344960, 428750, 459620, 70, 9870, 11928, 14911, 15878, 18638, 19980 },
            { 286328, 286328, 357911, 360838, 447388, 479600, 71, 10030, 12270, 15337, 16359, 19172, 28017 },
            { 296358, 298598, 373248, 377197, 466560, 507617, 72, 9409, 12615, 15769, 16848, 19711, 21446 },
            { 305767, 311213, 389017, 394045, 486271, 529063, 73, 10307, 12966, 16207, 17343, 20259, 30146 },
            { 316074, 324179, 405224, 411388, 506530, 559209, 74, 10457, 13321, 16651, 17847, 20813, 22978 },
            { 326531, 337500, 421875, 429235, 527343, 582187, 75, 9724, 13680, 17101, 18356, 21377, 32379 },
            { 336255, 351180, 438976, 447591, 548720, 614566, 76, 10710, 14046, 17557, 18873, 21946, 24580 },
            { 346965, 365226, 456533, 466464, 570666, 639146, 77, 10847, 14415, 18019, 19398, 22524, 34717 },
            { 357812, 379641, 474552, 485862, 593190, 673863, 78, 9995, 14790, 18487, 19929, 23108, 26252 },
            { 367807, 394431, 493039, 505791, 616298, 700115, 79, 11073, 15169, 18961, 20469, 23702, 37165 },
            { 378880, 409600, 512000, 526260, 640000, 737280, 80, 11197, 15552, 19441, 21014, 24301, 27995 },
            { 390077, 425152, 531441, 547274, 664301, 765275, 81, 10216, 15942, 19927, 21567, 24909, 39722 },
            { 400293, 441094, 551368, 568841, 689210, 804997, 82, 11393, 16335, 20419, 22128, 25523, 29812 },
            { 411686, 457429, 571787, 590969, 714733, 834809, 83, 11504, 16734, 20917, 22695, 26147, 42392 },
            { 423190, 474163, 592704, 613664, 740880, 877201, 84, 10382, 17137, 21421, 23271, 26776, 31704 },
            { 433572, 491300, 614125, 636935, 767656, 908905, 85, 11667, 17544, 21931, 23852, 27414, 45179 },
            { 445239, 508844, 636056, 660787, 795070, 954084, 86, 11762, 17958, 22447, 24441, 28058, 33670 },
            { 457001, 526802, 658503, 685228, 823128, 987754, 87, 10488, 18375, 22969, 25038, 28712, 48083 },
            { 467489, 545177, 681472, 710266, 851840, 1035837, 88, 11889, 18798, 23497, 25641, 29371, 35715 },
            { 479378, 563975, 704969, 735907, 881211, 1071552, 89, 11968, 19225, 24031, 26253, 30039, 51108 },
            { 491346, 583200, 729000, 762160, 911250, 1122660, 90, 10532, 19656, 24571, 26870, 30713, 37839 },
            { 501878, 602856, 753571, 789030, 941963, 1160499, 91, 12056, 20094, 25117, 27495, 31397, 54254 },
            { 513934, 622950, 778688, 816525, 973360, 1214753, 92, 12115, 20535, 25669, 28128, 32086, 40043 },
            { 526049, 643485, 804357, 844653, 1005446, 1254796, 93, 10508, 20982, 26227, 28767, 32784, 57526 },
            { 536557, 664467, 830584, 873420, 1038230, 1312322, 94, 12163, 21433, 26791, 29415, 33488, 42330 },
            { 548720, 685900, 857375, 902835, 1071718, 1354652, 95, 12202, 21888, 27361, 30068, 34202, 60925 },
            { 560922, 707788, 884736, 932903, 1105920, 1415577, 96, 10411, 22350, 27937, 30729, 34921, 44699 },
            { 571333, 730138, 912673, 963632, 1140841, 1460276, 97, 12206, 22815, 28519, 31398, 35649, 64455 },
            { 583539, 752953, 941192, 995030, 1176490, 1524731, 98, 8343, 23286, 29107, 32073, 36383, 47153 },
            { 591882, 776239, 970299, 1027103, 1212873, 1571884, 99, 8118, 23761, 29701, 32757, 37127, 68116 },
            { 600000, 800000, 1000000, 1059860, 1250000, 1640000, 100, 0, 0, 0, 0, 0, 0 } };


    PKMNDATA::pokemonData data;

    pokemon::boxPokemon::boxPokemon( u16* p_moves,
                                     int p_pkmnId,
                                     const wchar_t* p_name,
                                     short p_level,
                                     unsigned short p_id,
                                     unsigned short p_sid,
                                     const wchar_t* p_oT,
                                     bool p_oTFemale,
                                     bool p_cloned,
                                     bool p_shiny,
                                     bool p_hiddenAbility,
                                     bool p_fatefulEncounter,
                                     bool p_isEgg,
                                     short p_gotPlace,
                                     char p_ball,
                                     char p_pokerus ) {
        PKMNDATA::getAll( p_pkmnId, data );

        srand( LastPID );
        LastPID = m_PID = rand( );
        if( p_shiny )
            while( !isShiny( ) )
                LastPID = m_PID = rand( );
        else
            while( isShiny( ) )
                LastPID = m_PID = rand( );
        m_checksum = 0;
        m_SPEC = p_pkmnId;

        if( data.m_items[ 3 ] )
            m_Item = data.m_items[ 3 ];
        else {
            m_b1 = rand( ) % 100;
            if( m_b1 < 5 && data.m_items[ 0 ] )
                m_Item = data.m_items[ 0 ];
            else if( m_b1 < 20 && data.m_items[ 1 ] )
                m_Item = data.m_items[ 1 ];
            else if( m_b1 < 80 && data.m_items[ 2 ] )
                m_Item = data.m_items[ 2 ];
            else
                m_Item = 0;
        }

        m_ID = p_id;
        m_SID = p_sid;
        if( !p_isEgg )
            m_exp = EXP[ p_level - 1 ][ data.m_expType ];
        else
            m_exp = 0;

        time_t unixTime = time( NULL );
        tm* timeStruct = gmtime( (const time_t *)&unixTime );

        if( p_isEgg ) {
            m_steps = data.m_eggcyc;
            m_gotDate[ 0 ] = timeStruct->tm_mday;
            m_gotDate[ 1 ] = timeStruct->tm_mon + 1;
            m_gotDate[ 2 ] = ( timeStruct->tm_year + 1900 ) % 100;
            m_gotPlace = p_gotPlace;
            m_hatchDate[ 0 ] = m_hatchDate[ 1 ] = m_hatchDate[ 2 ] = m_hatchPlace = 0;
        } else {
            m_steps = data.m_baseFriend;
            m_gotDate[ 0 ] = m_gotDate[ 1 ] = m_gotDate[ 2 ] = m_hatchPlace = 0;
            m_hatchDate[ 0 ] = timeStruct->tm_mday;
            m_hatchDate[ 1 ] = timeStruct->tm_mon + 1;
            m_hatchDate[ 2 ] = ( timeStruct->tm_year + 1900 ) % 100;
            m_gotPlace = p_gotPlace;
        }
        m_ability = p_hiddenAbility ? ( ( m_PID & 1 || ( data.m_abilities[ 3 ] == 0 ) ) ? data.m_abilities[ 2 ] : data.m_abilities[ 3 ] ) :
            ( ( m_PID & 1 || ( data.m_abilities[ 1 ] == 0 ) ) ? data.m_abilities[ 0 ] : data.m_abilities[ 1 ] );
        m_markings = NONE;
        m_origLang = 5;
        for( int i = 0; i < 6; ++i ) m_EV[ i ] = 0;
        for( int i = 0; i < 6; ++i ) m_ConStats[ i ] = 0;
        for( int i = 0; i < 4; ++i ) m_ribbons1[ i ] = 0;

        if( p_moves )
            for( int i = 0; i < 4; ++i ) m_Attack[ i ] = p_moves[ i ];
        else
            PKMNDATA::getLearnMoves( p_pkmnId, p_level, 0, 1, 4, m_Attack );
        for( int i = 0; i < 4; ++i ) m_AcPP[ i ] = (unsigned char)( AttackList[ m_Attack[ i ] ]->m_movePP );

        m_ppup.m_Up1 = 0;
        m_ppup.m_Up2 = 0;
        m_ppup.m_Up3 = 0;
        m_ppup.m_Up4 = 0;
        m_IV.m_Attack = rand( ) % 32;
        m_IV.m_Defense = rand( ) % 32;
        m_IV.m_HP = rand( ) % 32;
        m_IV.m_SAttack = rand( ) % 32;
        m_IV.m_SDefense = rand( ) % 32;
        m_IV.m_Speed = rand( ) % 32;
        m_IV.m_isNicked = false;
        m_IV.m_isEgg = p_isEgg;
        for( int i = 0; i < 4; ++i ) m_ribbons0[ i ] = 0;
        m_fateful = p_fatefulEncounter;


        pkmnGenderType A = data.m_gender;
        if( A == MALE )
            m_isFemale = m_isGenderless = false;
        else if( A == FEMALE )
            m_isFemale = true,
            m_isGenderless = false;
        else if( A == GENDERLESS )
            m_isFemale = false,
            m_isGenderless = true;
        else if( ( m_PID % 256 ) >= A )
            m_isFemale = m_isGenderless = false;
        else m_isFemale = true,
            m_isGenderless = false;

        m_altForme = 0;
        m_cloned = p_cloned;
        if( p_name ) {
            wcscpy( m_Name, p_name );
            m_IV.m_isNicked = true;
        } else {
            PKMNDATA::getWDisplayName( p_pkmnId, m_Name );
            m_IV.m_isNicked = false;
        }
        m_hometown = 4;
        for( int i = 0; i < 4; ++i ) m_ribbons2[ i ] = 0;
        wcscpy( m_OT, p_oT );
        m_PKRUS = p_pokerus;
        m_Ball = p_ball;
        m_gotLevel = p_level;
        m_OTisFemale = p_oTFemale;
        m_encounter = (encounter)0;
        m_HGSSBall = 0;

    }
    pokemon::pokemon( u16* p_moves,
                      int p_pkmnId,
                      const wchar_t* p_name,
                      short p_level,
                      unsigned short p_id,
                      unsigned short p_sid,
                      const wchar_t* p_oT,
                      bool p_oTFemale,
                      bool p_cloned,
                      bool p_shiny,
                      bool p_hiddenAbility,
                      bool p_fatefulEncounter,
                      bool p_isEgg,
                      short p_gotPlace,
                      char p_ball,
                      char p_pokerus )
                      : m_boxdata( p_moves,
                      p_pkmnId,
                      p_name,
                      p_level,
                      p_id,
                      p_sid,
                      p_oT,
                      p_oTFemale,
                      p_cloned,
                      p_shiny,
                      p_hiddenAbility,
                      p_fatefulEncounter,
                      p_isEgg, p_gotPlace,
                      p_ball, p_pokerus ),
                      m_Level( p_level ) {
        PKMNDATA::getAll( p_pkmnId, data );
        if( p_pkmnId != 292 )
            m_stats.m_acHP = m_stats.m_maxHP = ( ( m_boxdata.m_IV.m_HP + 2 * data.m_bases[ 0 ] + ( m_boxdata.m_EV[ 0 ] / 4 ) + 100 )*p_level / 100 ) + 10;
        else
            m_stats.m_acHP = m_stats.m_maxHP = 1;

        pkmnNatures nature = m_boxdata.getNature( );
        m_stats.m_Atk = ( ( ( m_boxdata.m_IV.m_Attack + 2 * data.m_bases[ 1 ] + ( m_boxdata.m_EV[ 1 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 0 ];
        m_stats.m_Def = ( ( ( m_boxdata.m_IV.m_Defense + 2 * data.m_bases[ 2 ] + ( m_boxdata.m_EV[ 2 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 1 ];
        m_stats.m_Spd = ( ( ( m_boxdata.m_IV.m_Speed + 2 * data.m_bases[ 3 ] + ( m_boxdata.m_EV[ 3 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 2 ];
        m_stats.m_SAtk = ( ( ( m_boxdata.m_IV.m_SAttack + 2 * data.m_bases[ 4 ] + ( m_boxdata.m_EV[ 4 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 3 ];
        m_stats.m_SDef = ( ( ( m_boxdata.m_IV.m_SDefense + 2 * data.m_bases[ 5 ] + ( m_boxdata.m_EV[ 5 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 4 ];

        m_status.m_Asleep = m_status.m_Burned = m_status.m_Frozen = m_status.m_Paralyzed = m_status.m_Poisoned = m_status.m_Toxic = false;
    }

    void setDefaultConsoleTextColors( u16* p_palette, int p_start = 1 ) {
        p_palette[ p_start * 16 - 1 ] = RGB15( 0, 0, 0 ); //30 normal black
        p_palette[ ( p_start + 1 ) * 16 - 1 ] = RGB15( 15, 0, 0 ); //31 normal red
        p_palette[ ( p_start + 2 ) * 16 - 1 ] = RGB15( 0, 15, 0 ); //32 normal green
        p_palette[ ( p_start + 3 ) * 16 - 1 ] = RGB15( 15, 15, 0 ); //33 normal yellow

        p_palette[ ( p_start + 4 ) * 16 - 1 ] = RGB15( 0, 0, 15 ); //34 normal blue
        p_palette[ ( p_start + 5 ) * 16 - 1 ] = RGB15( 15, 0, 15 ); //35 normal magenta
        p_palette[ ( p_start + 6 ) * 16 - 1 ] = RGB15( 0, 15, 15 ); //36 normal cyan
        p_palette[ ( p_start + 7 ) * 16 - 1 ] = RGB15( 24, 24, 24 ); //37 normal white
    }


    void pokemon::drawPage( int p_page, PrintConsole* p_top, PrintConsole* p_bottom, bool p_newpok ) {
        setDefaultConsoleTextColors( BG_PALETTE, 6 );
        loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNInfoScreen", 32 );

        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        cust_font2.setColor( 0, 0 );
        cust_font2.setColor( 253, 1 );
        cust_font2.setColor( 254, 2 );

        BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 251 ] = RGB15( 20, 20, 20 );
        BG_PALETTE[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE_SUB[ 253 ] = RGB15( 10, 10, 10 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );


        consoleSelect( p_top );
        consoleSetWindow( p_top, 0, 0, 32, 24 );
        consoleClear( );
        consoleSelect( p_bottom );
        consoleClear( );

        consoleSelect( p_top );

        initOAMTable( oamTop );
        updateOAMSub( oam );

        int a2 = 0, b2 = 0, c2 = 0;
        if( !( m_boxdata.m_IV.m_isEgg ) ) {

            BG_PALETTE[ 254 ] = RGB15( 31, 0, 0 );
            BG_PALETTE[ 255 ] = RGB15( 0, 0, 31 );
            BG_PALETTE[ 253 ] = RGB15( 31, 31, 31 );
            cust_font.setColor( 253, 1 );

            consoleSetWindow( p_top, 20, 1, 13, 2 );

            cust_font.print_string( &( m_boxdata.m_Name[ 0 ] ), 150, 2, false );
            int G = m_boxdata.gender( );

            cust_font.print_char( '/', 234, 2, false );
            if( m_boxdata.m_SPEC != 29 && m_boxdata.m_SPEC != 32 ) {
                if( G == 1 ) {
                    cust_font.setColor( 255, 1 );
                    cust_font.print_char( 136, 246, 8, false );
                } else {
                    cust_font.setColor( 254, 1 );
                    cust_font.print_char( 137, 246, 8, false );
                }
            }
            cust_font.setColor( 253, 1 );

            PKMNDATA::getAll( m_boxdata.m_SPEC, data );
            cust_font.print_string( PKMNDATA::getDisplayName( m_boxdata.m_SPEC ), 160, 16, false );

            if( m_boxdata.getItem( ) ) {
                cust_font.print_string( "Item", 2, 176, false );
                cust_font.setColor( 252, 1 );
                cust_font.setColor( 0, 2 );
                char buf[ 200 ];
                sprintf( buf, "%s: %s", ItemList[ m_boxdata.getItem( ) ].getDisplayName( true ).c_str( ),
                         ItemList[ m_boxdata.getItem( ) ].getShortDescription( true ).c_str( ) );
                cust_font.print_string( buf, 50, 159, false );
                drawItemIcon( oamTop, spriteInfoTop, ItemList[ m_boxdata.getItem( ) ].m_itemName, 2, 152, a2, b2, c2, false );
                updateOAM( oamTop );
            } else {
                cust_font.setColor( 252, 1 );
                cust_font.setColor( 0, 2 );
                cust_font.print_string( ItemList[ m_boxdata.getItem( ) ].getDisplayName( ).c_str( ), 56, 168, false );
            }
            cust_font.setColor( 251, 1 );
            cust_font.setColor( 252, 2 );
        } else {
            p_page = 0;
            BG_PALETTE[ 253 ] = RGB15( 31, 31, 31 );
            cust_font.setColor( 253, 1 );

            cust_font.print_string( "Ei", 150, 2, false );
            cust_font.print_char( '/', 234, 2, false );
            cust_font.print_string( "Ei", 160, 18, false );
            cust_font.setColor( 251, 1 );
        }
        switch( p_page ) {
            case 0:
                cust_font.print_string( "Pokémon-Info", 36, 4, false );
                if( !( m_boxdata.m_IV.m_isEgg ) ) {
                    consoleSetWindow( p_top, 16, 4, 32, 24 );
                    printf( "     Lv.%3i", m_Level );

                    BG_PALETTE[ 254 ] = RGB15( 31, 0, 0 );
                    BG_PALETTE[ 255 ] = RGB15( 0, 0, 31 );

                    char buf[ 50 ];
                    sprintf( buf, "KP                     %3i", m_stats.m_maxHP );
                    cust_font.print_string( buf, 130, 44, false );

                    if( NatMod[ m_boxdata.getNature( ) ][ 0 ] == 1.2 )
                        cust_font.setColor( 254, 1 );
                    else if( NatMod[ m_boxdata.getNature( ) ][ 0 ] == 0.8 )
                        cust_font.setColor( 255, 1 );
                    else
                        cust_font.setColor( 251, 1 );
                    sprintf( buf, "ANG                   %3i", m_stats.m_Atk );
                    cust_font.print_string( buf, 130, 69, false );

                    if( NatMod[ m_boxdata.getNature( ) ][ 1 ] == 1.2 )
                        cust_font.setColor( 254, 1 );
                    else if( NatMod[ m_boxdata.getNature( ) ][ 1 ] == 0.8 )
                        cust_font.setColor( 255, 1 );
                    else
                        cust_font.setColor( 251, 1 );
                    sprintf( buf, "VER                   %3i", m_stats.m_Def );
                    cust_font.print_string( buf, 130, 86, false );

                    if( NatMod[ m_boxdata.getNature( ) ][ 2 ] == 1.2 )
                        cust_font.setColor( 254, 1 );
                    else if( NatMod[ m_boxdata.getNature( ) ][ 2 ] == 0.8 )
                        cust_font.setColor( 255, 1 );
                    else
                        cust_font.setColor( 251, 1 );
                    sprintf( buf, "INI                   \xC3\xC3""%3i", m_stats.m_Spd );
                    cust_font.print_string( buf, 130, 103, false );

                    if( NatMod[ m_boxdata.getNature( ) ][ 3 ] == 1.2 )
                        cust_font.setColor( 254, 1 );
                    else if( NatMod[ m_boxdata.getNature( ) ][ 3 ] == 0.8 )
                        cust_font.setColor( 255, 1 );
                    else
                        cust_font.setColor( 251, 1 );
                    sprintf( buf, "SAN                   %3i", m_stats.m_SAtk );
                    cust_font.print_string( buf, 130, 120, false );

                    if( NatMod[ m_boxdata.getNature( ) ][ 4 ] == 1.2 )
                        cust_font.setColor( 254, 1 );
                    else if( NatMod[ m_boxdata.getNature( ) ][ 4 ] == 0.8 )
                        cust_font.setColor( 255, 1 );
                    else
                        cust_font.setColor( 251, 1 );
                    sprintf( buf, "SVE                   %3i", m_stats.m_SDef );
                    cust_font.print_string( buf, 130, 137, false );

                    font::putrec( 158, 46, 158 + 68, 46 + 12, false, false, 251 );

                    font::putrec( 158, 46, 158 + int( 68.0*m_boxdata.IVget( 0 ) / 31 ), 46 + 6, false, false, 7 * 16 - 1 );
                    font::putrec( 158, 46 + 6, 158 + int( 68.0*m_boxdata.m_EV[ 0 ] / 255 ), 46 + 12, false, false, 7 * 16 - 1 );

                    for( int i = 1; i < 6; ++i ) {
                        font::putrec( 158, 54 + ( 17 * i ), 158 + 68, 54 + 12 + ( 17 * i ), false, false, 251 );
                        font::putrec( 158, 54 + ( 17 * i ), 158 + int( 68.0*m_boxdata.IVget( i ) / 31 ), 54 + 6 + ( 17 * i ), false, false, ( 7 + i ) * 16 - 1 );
                        font::putrec( 158, 54 + 6 + ( 17 * i ), 158 + int( 68.0*m_boxdata.m_EV[ i ] / 255 ), 54 + 12 + ( 17 * i ), false, false, ( 7 + i ) * 16 - 1 );
                    }
                } else {
                    consoleSetWindow( p_top, 16, 4, 32, 24 );
                    if( m_boxdata.m_steps > 10 ) {
                        cust_font.print_string( "Was da wohl", 16 * 8, 50, false );
                        cust_font.print_string( "schlüpfen wird?", 16 * 8, 70, false );
                        cust_font.print_string( "Es dauert wohl", 16 * 8, 100, false );
                        cust_font.print_string( "noch lange.", 16 * 8, 120, false );
                    } else if( m_boxdata.m_steps > 5 ) {
                        cust_font.print_string( "Hat es sich", 16 * 8, 50, false );
                        cust_font.print_string( "gerade bewegt?", 16 * 8, 70, false );
                        cust_font.print_string( "Da tut sich", 16 * 8, 100, false );
                        cust_font.print_string( "wohl bald was.", 16 * 8, 120, false );
                    } else {
                        cust_font.print_string( "Jetzt macht es", 16 * 8, 50, false );
                        cust_font.print_string( "schon Geräusche!", 16 * 8, 70, false );
                        cust_font.print_string( "Bald ist es", 16 * 8, 100, false );
                        cust_font.print_string( "wohl soweit.", 16 * 8, 120, false );
                    }
                }
                break;
            case 1:
            {
                cust_font.print_string( "Attacken", 36, 4, false );

                consoleSetWindow( p_top, 16, 5, 32, 24 );
                for( int i = 0; i < 4; i++ ) {
                    if( m_boxdata.m_Attack[ i ] == 0 )
                        continue;
                    Type t = AttackList[ m_boxdata.m_Attack[ i ] ]->m_moveType;
                    drawTypeIcon( oamTop, spriteInfoTop, a2, b2, c2, t, 126, 43 + 32 * i, false );

                    /* if(t == data.Types[0] || t == data.Types[1])
                    printf("\x1b[32m");*/

                    if( i == 0 )
                        printf( "    %s\n    AP %2hhu""/""%2hhu ",
                        ( AttackList[ m_boxdata.m_Attack[ 0 ] ]->m_moveName.c_str( ) ), m_boxdata.m_AcPP[ 0 ],
                        AttackList[ m_boxdata.m_Attack[ 0 ] ]->m_movePP* ( ( 5 + m_boxdata.m_ppup.m_Up1 ) / 5 ) );
                    if( i == 1 )
                        printf( "    %s\n    AP %2hhu""/""%2hhu ",
                        ( AttackList[ m_boxdata.m_Attack[ 1 ] ]->m_moveName.c_str( ) ), m_boxdata.m_AcPP[ 1 ],
                        AttackList[ m_boxdata.m_Attack[ 1 ] ]->m_movePP* ( ( 5 + m_boxdata.m_ppup.m_Up2 ) / 5 ) );
                    if( i == 2 )
                        printf( "    %s\n    AP %2hhu""/""%2hhu ",
                        ( AttackList[ m_boxdata.m_Attack[ 2 ] ]->m_moveName.c_str( ) ), m_boxdata.m_AcPP[ 2 ],
                        AttackList[ m_boxdata.m_Attack[ 2 ] ]->m_movePP* ( ( 5 + m_boxdata.m_ppup.m_Up3 ) / 5 ) );
                    if( i == 3 )
                        printf( "    %s\n    AP %2hhu""/""%2hhu ",
                        ( AttackList[ m_boxdata.m_Attack[ 3 ] ]->m_moveName.c_str( ) ), m_boxdata.m_AcPP[ 3 ],
                        AttackList[ m_boxdata.m_Attack[ 3 ] ]->m_movePP* ( ( 5 + m_boxdata.m_ppup.m_Up4 ) / 5 ) );
                    switch( AttackList[ m_boxdata.m_Attack[ i ] ]->m_moveHitType ) {
                        case move::PHYS:
                            printf( "PHS" );
                            break;
                        case move::SPEC:
                            printf( "SPC" );
                            break;
                        case move::STAT:
                            printf( "STS" );
                            break;
                    }
                    printf( "\n    S " );
                    if( AttackList[ m_boxdata.m_Attack[ i ] ]->m_moveBasePower )
                        printf( "%3i", AttackList[ m_boxdata.m_Attack[ i ] ]->m_moveBasePower );
                    else
                        printf( "---" );
                    printf( " G " );
                    if( AttackList[ m_boxdata.m_Attack[ i ] ]->m_moveAccuracy )
                        printf( "%3i", AttackList[ m_boxdata.m_Attack[ i ] ]->m_moveAccuracy );
                    else
                        printf( "---" );

                    printf( "\n\n" );
                    updateOAM( oamTop );
                    printf( "\x1b[39m" );
                }
                break;
            }
            case 2:
                cust_font.print_string( "Bänder", 36, 4, false );

                break;
            default:
                return;
        }

        consoleSelect( p_bottom );
        printf( "\x1b[39m" );
        int o2s = 50, p2s = 2, t2s = 780;

        oam->oamBuffer[ 0 ].x = 256 - 24;
        oam->oamBuffer[ 0 ].y = 196 - 28;
        oam->oamBuffer[ 13 ].isHidden = false;
        oam->oamBuffer[ 14 ].isHidden = false;
        oam->oamBuffer[ 14 ].x = 256 - 24;
        oam->oamBuffer[ 14 ].y = 196 - 28 - 22;
        oam->oamBuffer[ 13 ].x = 256 - 28 - 18;
        oam->oamBuffer[ 13 ].y = 196 - 28;

        if( !( m_boxdata.m_IV.m_isEgg ) ) {
            if( data.m_types[ 0 ] == data.m_types[ 1 ] ) {
                drawTypeIcon( oam, spriteInfo, o2s, p2s, t2s, data.m_types[ 0 ], 256 - 50, 24, true );
                oam->oamBuffer[ ++o2s ].isHidden = true;
            } else {
                drawTypeIcon( oam, spriteInfo, o2s, p2s, t2s, data.m_types[ 0 ], 256 - 68, 24, true );
                drawTypeIcon( oam, spriteInfo, o2s, p2s, t2s, data.m_types[ 1 ], 256 - 32, 24, true );
            }
            drawItemIcon( oam, spriteInfo, m_boxdata.m_Ball == 0 ? "Pokeball" : ItemList[ m_boxdata.m_Ball ].m_itemName, 256 - 100, 0, o2s, p2s, t2s, true );

            oam->oamBuffer[ 15 ].isHidden = false;
            oam->oamBuffer[ 15 ].y = 0;
            oam->oamBuffer[ 15 ].x = 256 - 96;
            oam->oamBuffer[ 16 ].isHidden = false;
            oam->oamBuffer[ 16 ].y = 0;
            oam->oamBuffer[ 16 ].x = 256 - 32;
            updateOAMSub( oam );

            consoleSetWindow( p_bottom, 23, 1, 20, 5 );

            if( m_boxdata.isShiny( ) ) {
                printf( "\x1b[34m""*%03i ", m_boxdata.m_SPEC );
            } else
                printf( "\x1b[33m""#%03i ", m_boxdata.m_SPEC );

            if( m_boxdata.m_PKRUS ) {
                printf( "\x1b[39m""PKRS" );
            }

            oam->oamBuffer[ 90 ].isHidden = false;
            oam->oamBuffer[ 90 ].y = -4;
            oam->oamBuffer[ 90 ].x = -8;
            oam->oamBuffer[ 90 ].priority = OBJPRIORITY_1;
            oam->oamBuffer[ 91 ].isHidden = false;
            oam->oamBuffer[ 91 ].y = 18;
            oam->oamBuffer[ 91 ].x = -8;
            oam->oamBuffer[ 91 ].priority = OBJPRIORITY_1;
            oam->oamBuffer[ 92 ].isHidden = false;
            oam->oamBuffer[ 92 ].y = -8;
            oam->oamBuffer[ 92 ].x = 14;
            oam->oamBuffer[ 92 ].priority = OBJPRIORITY_1;

            oam->oamBuffer[ 90 + page ].isHidden = true;

        } else {
            oam->oamBuffer[ 90 ].isHidden = true;
            oam->oamBuffer[ 91 ].isHidden = true;
            oam->oamBuffer[ 92 ].isHidden = true;

            oam->oamBuffer[ 15 ].isHidden = false;
            oam->oamBuffer[ 15 ].y = 0;
            oam->oamBuffer[ 15 ].x = 256 - 32;
            oam->oamBuffer[ 16 ].isHidden = true;

            oam->oamBuffer[ o2s++ ].isHidden = true;
            oam->oamBuffer[ o2s++ ].isHidden = true;
            oam->oamBuffer[ o2s ].isHidden = true;
            drawItemIcon( oam, spriteInfo, m_boxdata.m_Ball == 0 ? "Pokeball" : ItemList[ m_boxdata.m_Ball ].m_itemName, 256 - 32, 0, o2s, p2s, t2s, true );
            updateOAMSub( oam );
        }
        printf( "\x1b[33m" );
        for( int i = 0; i < 4; ++i ) {
            oam->oamBuffer[ 9 + i ].isHidden = false;
            oam->oamBuffer[ 9 + i ].hFlip = true;
            oam->oamBuffer[ 9 + i ].priority = OBJPRIORITY_3;
            oam->oamBuffer[ 9 + i ].y = 192 - 54;
        }
        oam->oamBuffer[ 18 ].isHidden = false;
        oam->oamBuffer[ 18 ].y = 192 - 12;
        oam->oamBuffer[ 18 ].x = 76;
        oam->oamBuffer[ 18 ].priority = OBJPRIORITY_1;
        oam->oamBuffer[ 28 ].isHidden = false;
        oam->oamBuffer[ 28 ].y = 192 - 12;
        oam->oamBuffer[ 28 ].x = 0;
        oam->oamBuffer[ 28 ].priority = OBJPRIORITY_1;
        oam->oamBuffer[ 27 ].isHidden = false;
        oam->oamBuffer[ 27 ].y = 192 - 12;
        oam->oamBuffer[ 27 ].x = 20;
        oam->oamBuffer[ 27 ].priority = OBJPRIORITY_1;
        updateOAMSub( oam );
        if( !( m_boxdata.m_IV.m_isEgg ) ) {
            consoleSetWindow( p_bottom, 0, 23, 20, 5 );
            printf( "%s", abilities[ m_boxdata.m_ability ].m_abilityName.c_str( ) );
            consoleSetWindow( p_bottom, 1, 18, 28, 5 );
            printf( "%s", abilities[ m_boxdata.m_ability ].m_flavourText.c_str( ) );
        }

        consoleSetWindow( p_bottom, 3, 3, 27, 15 );

        //wprintf(L"  OT %ls\n  (%05i/%05i)\n\n",&(m_boxdata.OT[0]),m_boxdata.ID,m_boxdata.SID);

        drawSub( );


        cust_font.setColor( 0, 0 );
        cust_font.setColor( 254, 1 );
        cust_font.setColor( 255, 2 );
        BG_PALETTE_SUB[ 253 ] = m_boxdata.m_OTisFemale ? RGB15( 31, 15, 0 ) : RGB15( 0, 15, 31 );
        BG_PALETTE_SUB[ 255 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );

        cust_font.print_string( "OT:", 28, 22, true );
        cust_font.setColor( 253, 2 );
        cust_font.print_string( m_boxdata.m_OT, 56, 16, true );
        cust_font.setColor( 255, 2 );

        char buf[ 50 ];
        sprintf( buf, "(%05i/%05i)", m_boxdata.m_ID, m_boxdata.m_SID );
        cust_font.print_string( buf, 50, 30, true );

        if( m_boxdata.m_ID == SAV.m_Id && m_boxdata.m_SID == SAV.m_Sid ) //Trainer is OT
        {
            if( m_boxdata.m_fateful )
                cust_font.print_string( "Schicksalhafte Begegnung.", 28, 120, true );
            if( !( m_boxdata.m_gotDate[ 0 ] ) ) {
                if( savMod == SavMod::_NDS )
                    sprintf( buf, "Gefangen am %02i.%02i.%02i mit Lv. %i", m_boxdata.m_hatchDate[ 0 ], m_boxdata.m_hatchDate[ 1 ], m_boxdata.m_hatchDate[ 2 ], m_boxdata.m_gotLevel );
                else
                    sprintf( buf, "Gefangen mit Lv. %i", m_boxdata.m_gotLevel );
                cust_font.print_string( buf, 28, 44, true );
                sprintf( buf, "in/bei %s.", getLoc( m_boxdata.m_gotPlace ) );
                cust_font.print_string( buf, 35, 58, true );
                sprintf( buf, "Besitzt ein %s""es Wesen,", &( NatureList[ m_boxdata.getNature( ) ][ 0 ] ) );
                cust_font.print_string( buf, 28, 76, true );
                sprintf( buf, "%s"".", &( PersonalityList[ m_boxdata.getPersonality( ) ][ 0 ] ) );
                cust_font.print_string( buf, 35, 90, true );

                sprintf( buf, "Mag %s""e Pokériegel.", &( m_boxdata.getTasteStr( )[ 0 ] ) );
                cust_font.print_string( buf, 28, 104, true );
            } else {
                if( savMod == SavMod::_NDS )
                    sprintf( buf, "Als Ei erhalten am %02i.%02i.%02i", m_boxdata.m_gotDate[ 0 ], m_boxdata.m_gotDate[ 1 ], m_boxdata.m_gotDate[ 2 ] );
                else
                    sprintf( buf, "Als Ei erhalten." );
                cust_font.print_string( buf, 28, 44, true );
                sprintf( buf, "in/bei %s.", getLoc( m_boxdata.m_gotPlace ) );
                cust_font.print_string( buf, 35, 58, true );
                if( !( m_boxdata.m_IV.m_isEgg ) ) {

                    if( savMod == SavMod::_NDS ) {
                        sprintf( buf, "Geschlüpft am %02i.%02i.%02i", m_boxdata.m_hatchDate[ 0 ], m_boxdata.m_hatchDate[ 1 ], m_boxdata.m_hatchDate[ 2 ] );
                        cust_font.print_string( buf, 28, 72, true );
                        sprintf( buf, "in/bei %s.", getLoc( m_boxdata.m_hatchPlace ) );
                        cust_font.print_string( buf, 35, 86, true );
                    } else {
                        sprintf( buf, "Geschlüpft in/bei %s.", getLoc( m_boxdata.m_hatchPlace ) );
                        cust_font.print_string( buf, 28, 72, true );
                    }
                    if( !m_boxdata.m_fateful ) {
                        sprintf( buf, "Besitzt ein %s""es Wesen,", &( NatureList[ m_boxdata.getNature( ) ][ 0 ] ) );
                        cust_font.print_string( buf, 28, 100, true );
                        sprintf( buf, "%s"".", &( PersonalityList[ m_boxdata.getPersonality( ) ][ 0 ] ) );
                        cust_font.print_string( buf, 35, 114, true );

                        sprintf( buf, "Mag %s""e Pokériegel.", &( m_boxdata.getTasteStr( )[ 0 ] ) );
                        cust_font.print_string( buf, 28, 128, true );
                    } else {
                        sprintf( buf, "Besitzt ein %s""es Wesen.", &( NatureList[ m_boxdata.getNature( ) ][ 0 ] ) );
                        cust_font.print_string( buf, 28, 100, true );
                    }
                }
            }
        } else {
            if( !( m_boxdata.m_gotDate[ 0 ] ) ) {
                if( savMod == SavMod::_NDS )
                    sprintf( buf, "Off. gef. am %02i.%02i.%02i mit Lv. %i.", m_boxdata.m_hatchDate[ 0 ], m_boxdata.m_hatchDate[ 1 ], m_boxdata.m_hatchDate[ 2 ], m_boxdata.m_gotLevel );
                else
                    sprintf( buf, "Offenbar gefangen mit Lv. %i.", m_boxdata.m_gotLevel );
                cust_font.print_string( buf, 28, 44, true );
                sprintf( buf, "in/bei %s.", getLoc( m_boxdata.m_gotPlace ) );
                cust_font.print_string( buf, 35, 58, true );
                sprintf( buf, "Besitzt ein %s""es Wesen,", &( NatureList[ m_boxdata.getNature( ) ][ 0 ] ) );
                cust_font.print_string( buf, 28, 76, true );
                sprintf( buf, "%s"".", &( PersonalityList[ m_boxdata.getPersonality( ) ][ 0 ] ) );
                cust_font.print_string( buf, 35, 90, true );

                sprintf( buf, "Mag %s""e Pokériegel.", &( m_boxdata.getTasteStr( )[ 0 ] ) );
                cust_font.print_string( buf, 28, 104, true );
            } else {
                if( savMod == SavMod::_NDS )
                    sprintf( buf, "Off. Als Ei erh. am %02i.%02i.%02i", m_boxdata.m_gotDate[ 0 ], m_boxdata.m_gotDate[ 1 ], m_boxdata.m_gotDate[ 2 ] );
                else
                    sprintf( buf, "Offenbar als Ei erhalten." );
                cust_font.print_string( buf, 28, 44, true );
                sprintf( buf, "in/bei %s.", getLoc( m_boxdata.m_gotPlace ) );
                cust_font.print_string( buf, 35, 58, true );
                if( !( m_boxdata.m_IV.m_isEgg ) ) {
                    if( savMod == SavMod::_NDS ) {
                        sprintf( buf, "Geschlüpft am %02i.%02i.%02i", m_boxdata.m_hatchDate[ 0 ], m_boxdata.m_hatchDate[ 1 ], m_boxdata.m_hatchDate[ 2 ] );
                        cust_font.print_string( buf, 28, 72, true );
                        sprintf( buf, "in/bei %s.", getLoc( m_boxdata.m_hatchPlace ) );
                        cust_font.print_string( buf, 35, 86, true );
                    } else {
                        sprintf( buf, "Geschlüpft in/bei %s.", getLoc( m_boxdata.m_hatchPlace ) );
                        cust_font.print_string( buf, 28, 72, true );
                    }
                    if( !m_boxdata.m_fateful ) {
                        sprintf( buf, "Besitzt ein %s""es Wesen,", &( NatureList[ m_boxdata.getNature( ) ][ 0 ] ) );
                        cust_font.print_string( buf, 28, 100, true );
                        sprintf( buf, "%s"".", &( PersonalityList[ m_boxdata.getPersonality( ) ][ 0 ] ) );
                        cust_font.print_string( buf, 35, 114, true );

                        sprintf( buf, "Mag %s""e Pokériegel.", &( m_boxdata.getTasteStr( )[ 0 ] ) );
                        cust_font.print_string( buf, 28, 128, true );
                    } else {
                        sprintf( buf, "Besitzt ein %s""es Wesen.", &( NatureList[ m_boxdata.getNature( ) ][ 0 ] ) );
                        cust_font.print_string( buf, 28, 100, true );
                    }
                }
            }
            if( m_boxdata.m_fateful )
                cust_font.print_string( "Off. schicks. Begegnung.", 28, 120, true );
        }

        if( !( m_boxdata.m_IV.m_isEgg ) ) {
            printf( "\x1b[33m" );

            consoleSelect( p_top );
            consoleSetWindow( p_top, 4, 5, 12, 2 );

            if( !loadPKMNSprite( oamTop, spriteInfoTop, "nitro:/PICS/SPRITES/pokemon/", m_boxdata.m_SPEC, 16, 48, a2, b2, c2, false, m_boxdata.isShiny( ), m_boxdata.m_isFemale, true ) )
                loadPKMNSprite( oamTop, spriteInfoTop, "nitro:/PICS/SPRITES/pokemon/", m_boxdata.m_SPEC, 16, 48, a2, b2, c2, false, m_boxdata.isShiny( ), !m_boxdata.m_isFemale, true );

            int exptype = data.m_expType;

            printf( "EP(%3i%%)\nKP(%3i%%)", ( m_boxdata.m_exp - POKEMON::EXP[ m_Level - 1 ][ exptype ] ) * 100 / ( POKEMON::EXP[ m_Level ][ exptype ] - POKEMON::EXP[ m_Level - 1 ][ exptype ] ),
                    m_stats.m_acHP * 100 / m_stats.m_maxHP );
            BATTLE::displayHP( 100, 101, 46, 80, 97, 98, false, 50, 56 );
            BATTLE::displayHP( 100, 100 - m_stats.m_acHP * 100 / m_stats.m_maxHP, 46, 80, 97, 98, false, 50, 56 );

            BATTLE::displayEP( 100, 101, 46, 80, 99, 100, false, 59, 62 );
            BATTLE::displayEP( 0, ( m_boxdata.m_exp - POKEMON::EXP[ m_Level - 1 ][ exptype ] ) * 100 / ( POKEMON::EXP[ m_Level ][ exptype ] - POKEMON::EXP[ m_Level - 1 ][ exptype ] ), 46, 80, 99, 100, false, 59, 62 );

        }
    }
    extern bool drawInfoSub( u16* p_layer, int p_pkmnId );
    int pokemon::draw( ) {

        // Load bitmap to top background
        loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNInfoScreen" );

        consoleSelect( &Top );
        consoleClear( );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleSelect( &Bottom );
        consoleClear( );

        bgUpdate( );

        int pagemax = 3;

        consoleSelect( &Bottom );
        wprintf( &( m_boxdata.m_Name[ 0 ] ) );

        drawPage( page, &Top, &Bottom, true );
        touchPosition touch;
        while( 1 ) {
            touchRead( &touch );
            swiWaitForVBlank( );
            updateOAMSub( oam );
            updateTime( );
            scanKeys( );
            int pressed = keysCurrent( );
            if( ( pressed & KEY_B ) || ( pressed & KEY_X ) || ( sqrt( sq( 248 - touch.px ) + sq( 184 - touch.py ) ) <= 16 ) ) {
                while( 1 ) {
                    scanKeys( );
                    if( !( keysCurrent( ) & KEY_B ) )
                        break;
                    if( !( keysCurrent( ) & KEY_X ) )
                        break;
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                consoleSelect( &Bottom );
                consoleClear( );
                consoleSelect( &Top );
                consoleSetWindow( &Top, 0, 0, 32, 30 );
                consoleClear( );
                swiWaitForVBlank( );

                initOAMTableSub( oam );

                return pressed | KEY_B;
            } else if( ( pressed & KEY_DOWN ) || ( sqrt( sq( 220 - touch.px ) + sq( 184 - touch.py ) ) <= 16 ) ) {
                while( 1 ) {
                    scanKeys( );
                    if( keysUp( ) & KEY_DOWN )
                        break;
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                consoleSelect( &Bottom );
                consoleClear( );
                consoleSelect( &Top );
                consoleSetWindow( &Top, 0, 0, 32, 30 );
                consoleClear( );
                swiWaitForVBlank( );

                //initOAMTableSub(oam);

                return pressed | KEY_DOWN;
            } else if( ( pressed & KEY_UP ) || ( sqrt( sq( 248 - touch.px ) + sq( 162 - touch.py ) ) <= 16 ) ) {
                while( 1 ) {
                    scanKeys( );
                    if( keysUp( ) & KEY_UP )
                        break;
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                consoleSelect( &Bottom );
                consoleClear( );
                consoleSelect( &Top );
                consoleSetWindow( &Top, 0, 0, 32, 30 );
                consoleClear( );
                swiWaitForVBlank( );

                //initOAMTableSub(oam);

                return pressed | KEY_UP;
            } else if( pressed & KEY_RIGHT ) {
                if( ++page == pagemax )
                    page = 0;
                while( 1 ) {
                    scanKeys( );
                    if( keysUp( ) & KEY_RIGHT )
                        break;
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                drawPage( page, &Top, &Bottom, false );
            } else if( pressed & KEY_LEFT ) {
                if( --page == -1 )
                    page = pagemax - 1;
                while( 1 ) {
                    scanKeys( );
                    if( keysUp( ) & KEY_LEFT )
                        break;
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                drawPage( page, &Top, &Bottom, false );
            }

            else if( page != 0 && ( sqrt( sq( 16 - 4 - touch.px ) + sq( 16 - 4 - touch.py ) ) <= 16 ) ) {
                page = 0;
                while( 1 ) {
                    scanKeys( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                drawPage( page, &Top, &Bottom, false );
            } else if( page != 1 && ( sqrt( sq( 16 - 8 - touch.px ) + sq( 16 + 18 - touch.py ) ) <= 16 ) ) {
                page = 1;
                while( 1 ) {
                    scanKeys( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                drawPage( page, &Top, &Bottom, false );
            } else if( page != 2 && ( sqrt( sq( 16 + 18 - touch.px ) + sq( 16 - 6 - touch.py ) ) <= 16 ) ) {
                page = 2;
                while( 1 ) {
                    scanKeys( );
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                    swiWaitForVBlank( );
                }
                drawPage( page, &Top, &Bottom, false );
            }
        }
    }

    bool pokemon::boxPokemon::isShiny( ) {
        return ( ( ( m_ID^m_SID ) >> 3 ) ^ ( ( ( m_PID >> 16 ) ^ ( m_PID % ( 1 << 16 ) ) ) ) >> 3 ) == 0;
    }
    bool pokemon::boxPokemon::isCloned( ) {
        return ( ( m_PID >> 16 )&( m_PID % ( 1 << 16 ) ) ) < ( ( m_PID >> 16 ) ^ ( m_PID % ( 1 << 16 ) ) );
    }
    int pokemon::boxPokemon::gender( ) {
        if( m_isGenderless )
            return 0;
        else if( m_isFemale )
            return -1;
        return 1;
    }

    unsigned int LastPID = 42;
    int page = 0;


    bool pokemon::canEvolve( int p_item, int p_method ) {
        if( m_boxdata.m_IV.m_isEgg )
            return false;

        PKMNDATA::getAll( m_boxdata.m_SPEC, data );

        for( int i = 0; i < 7; ++i ) {
            if( m_Level < data.m_evolutions[ i ].m_e.m_evolveLevel )
                continue;
            if( m_boxdata.m_steps < data.m_evolutions[ i ].m_e.m_evolveFriendship )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveItem && p_item != data.m_evolutions[ i ].m_e.m_evolveItem )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveDayTime != -1 && getCurrentDaytime( ) != data.m_evolutions[ i ].m_e.m_evolveDayTime )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolvesInto == 0 )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveGender && m_boxdata.gender( ) != data.m_evolutions[ i ].m_e.m_evolveGender )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveLocation && SAV.m_acMapIdx != data.m_evolutions[ i ].m_e.m_evolveLocation )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveHeldItem && m_boxdata.m_Item != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
                bool b = false;
                for( int j = 0; j < 4; ++j )
                    b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_Attack[ j ] );
                if( !b )
                    continue;
            }
            if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
                bool b = false;
                for( int j = 0; j < 4; ++j )
                    b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType == AttackList[ m_boxdata.m_Attack[ j ] ]->m_moveType );
                if( !b )
                    continue;
            }
            if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_ConStats[ 1 ] )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
                bool b = false;
                for( int j = 0; j < 6; ++j )
                    b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember == SAV.m_PkmnTeam[ i ].m_boxdata.m_SPEC );
                if( !b )
                    continue;
            }
            if( p_method != data.m_evolutions[ i ].m_e.m_evolutionTrigger )
                continue;
            return true;
        }
        return false;
    }

    void pokemon::evolve( int p_item, int p_method ) {
        if( m_boxdata.m_IV.m_isEgg )
            return;

        PKMNDATA::getAll( m_boxdata.m_SPEC, data );

        int into = 0;

        for( int i = 0; i < 7; ++i ) {
            if( m_Level < data.m_evolutions[ i ].m_e.m_evolveLevel )
                continue;
            if( m_boxdata.m_steps < data.m_evolutions[ i ].m_e.m_evolveFriendship )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveItem && p_item != data.m_evolutions[ i ].m_e.m_evolveItem )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveDayTime != -1 && getCurrentDaytime( ) != data.m_evolutions[ i ].m_e.m_evolveDayTime )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolvesInto == 0 )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveGender && m_boxdata.gender( ) != data.m_evolutions[ i ].m_e.m_evolveGender )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveLocation && SAV.m_acMapIdx != data.m_evolutions[ i ].m_e.m_evolveLocation )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveHeldItem && m_boxdata.m_Item != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
                bool b = false;
                for( int j = 0; j < 4; ++j )
                    b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_Attack[ j ] );
                if( !b )
                    continue;
            }
            if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
                bool b = false;
                for( int j = 0; j < 4; ++j )
                    b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType == AttackList[ m_boxdata.m_Attack[ j ] ]->m_moveType );
                if( !b )
                    continue;
            }
            if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_ConStats[ 1 ] )
                continue;
            if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
                bool b = false;
                for( int j = 0; j < 6; ++j )
                    b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember == SAV.m_PkmnTeam[ i ].m_boxdata.m_SPEC );
                if( !b )
                    continue;
            }
            if( p_method != data.m_evolutions[ i ].m_e.m_evolutionTrigger )
                continue;
            into = data.m_evolutions[ i ].m_e.m_evolvesInto;
            break;
        }
        if( into == 0 )
            return;

        m_boxdata.m_SPEC = into;
        PKMNDATA::getAll( m_boxdata.m_SPEC, data );
        if( m_boxdata.m_SPEC != 292 )
            m_stats.m_maxHP = ( ( m_boxdata.m_IV.m_HP + 2 * data.m_bases[ 0 ] + ( m_boxdata.m_EV[ 0 ] / 4 ) + 100 )*m_Level / 100 ) + 10;
        else
            m_stats.m_maxHP = 1;

        if( !m_boxdata.m_IV.m_isNicked )
            wcscpy( m_boxdata.m_Name, PKMNDATA::getWDisplayName( m_boxdata.m_SPEC ) );

        int HPdif = m_stats.m_maxHP - m_stats.m_acHP;
        pkmnNatures nature = m_boxdata.getNature( );
        m_stats.m_Atk = ( ( ( m_boxdata.m_IV.m_Attack + 2 * data.m_bases[ 1 ] + ( m_boxdata.m_EV[ 1 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 0 ];
        m_stats.m_Def = ( ( ( m_boxdata.m_IV.m_Defense + 2 * data.m_bases[ 2 ] + ( m_boxdata.m_EV[ 2 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 1 ];
        m_stats.m_Spd = ( ( ( m_boxdata.m_IV.m_Speed + 2 * data.m_bases[ 3 ] + ( m_boxdata.m_EV[ 3 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 2 ];
        m_stats.m_SAtk = ( ( ( m_boxdata.m_IV.m_SAttack + 2 * data.m_bases[ 4 ] + ( m_boxdata.m_EV[ 4 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 3 ];
        m_stats.m_SDef = ( ( ( m_boxdata.m_IV.m_SDefense + 2 * data.m_bases[ 5 ] + ( m_boxdata.m_EV[ 5 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 4 ];

        m_stats.m_acHP = m_stats.m_maxHP - HPdif;
    }
}