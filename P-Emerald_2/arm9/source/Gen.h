/*
Pokémon Emerald 2 Version
------------------------------

file        : Gen.h
author      : Philip Wellnitz 
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2016
Philip Wellnitz 

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

/*

#pragma once
#include "nds.h"


namespace gen3 //using code from "pokehack" originally by "Grazfather"
{
    bool isValidItem( int p_gen5Idx );
    int getItemIdx( int p_gen5Idx );
    int getNItemIdx( int p_gen3Idx );
    int getPKMNIdx( int p_gen5Idx );
    int getNPKMNIdx( int p_gen3Idx );
    int getNLocation( int p_gen3Idx );
    int getLocation( int p_gen5Idx );
    int getNText( int p_in );

#define SAVEFILE_LEN (1 << 17)
#define NUM_BLOCKS_SLOT 14
#define NUM_BLOCKS_EXTRA 4
#define NUM_BLOCKS_TOTAL 32
#define BLOCK_DATA_LEN 3968
#define BLOCK_PADDING_LEN 116
#define BLOCK_FOOTER_LEN 12
#define BLOCK_TOTAL_LEN (BLOCK_DATA_LEN + BLOCK_PADDING_LEN + BLOCK_FOOTER_LEN)
#define SAVESLOT_LEN (14 * 3968)
#define BELT_OFFSET_RSE 0x11B8
#define BELT_OFFSET_FRLG 0xFB8
#define BOX_OFFSET 0x4D84

#define NUM_BELT_POKEMON 6
#define NUM_BOXES 14
#define NUM_POKEMON_PER_BOX 30
#define NUM_BOX_POKEMON (NUM_BOXES * NUM_POKEMON_PER_BOX)
#define POKEMON_DATA_LENGTH 48

#define NUM_POKEMON_SPECIES 440
#define NUM_ITEMS 377
#define NUM_ATTACKS 355


    typedef struct {
        u8 blocknum;
        u8 padding;
        u16 checksum;
        u32 validation; // 0x08012025
        int savenumber;
    } block_footer;

    typedef struct {
        u8 data[ BLOCK_DATA_LEN ];
        u8 padding[ BLOCK_PADDING_LEN ];
        block_footer footer;
    } block;

    u16 get_checksum( block* );

    typedef struct {
        u32 personality;
        u32 otid;
        u8 name[ 10 ];
        u16 language;
        u8 otname[ 7 ];
        union {
            struct {
                u8 circle : 1;
                u8 triangle : 1;
                u8 square : 1;
                u8 heart : 1;
                u8 xbit : 4; // unused
            } mark;
            u8 markint;
        };
        u16 checksum;
        u16 x1;				// unused
        u8 data[ POKEMON_DATA_LENGTH ];
        u32 status;
        u8 level;
        u8 pokerus;
        u16 currentHP;
        u16 maxHP;
        u16 move;
        u16 defense;
        u16 speed;
        u16 spatk;
        u16 spdef;
    } belt_pokemon_t;

    typedef struct {
        u32 personality;
        u32 otid;
        u8 name[ 10 ];
        u16 language;
        u8 otname[ 7 ];
        u8 mark;
        u16 checksum;
        u16 x1;				// unused
        u8 data[ POKEMON_DATA_LENGTH ];
    } box_pokemon_t;

    class pokemon {
    public:
        typedef struct {
            u16 species;
            u16 held;
            u32 xp;
            u8 ppbonuses;
            u8 happiness;
            u8 p_x;				// unused
        } pokemon_growth_t;

        typedef struct {
            union {
                struct {
                    u16 atk1;
                    u16 atk2;
                    u16 atk3;
                    u16 atk4;
                }_atk;
                u16 atk[ 4 ];
            };
            union {
                struct {
                    u8 pp1;
                    u8 pp2;
                    u8 pp3;
                    u8 pp4;
                }_pp;
                u8 pp[ 4 ];
            };
        } pokemon_moves_t;

        typedef struct {
            union {
                struct {
                    u8 hp;
                    u8 move;
                    u8 defense;
                    u8 speed;
                    u8 spatk;
                    u8 spdef;
                }_ev;
                u8 EV[ 6 ];
            };
            union {
                struct {
                    u8 coolness;
                    u8 beauty;
                    u8 cuteness;
                    u8 smartness;
                    u8 toughness;
                    u8 feel;
                }_con;
                u8 ConStat[ 6 ];
            };
        } pokemon_effort_t;

        typedef struct {
            u8 pokerus;
            u8 locationcaught;
            s8 levelcaught : 7;
            u8 game : 4;
            u8 pokeball : 4;
            u8 tgender : 1;
            union {
                struct {
                    u32 hp : 5;
                    u32 atk : 5;
                    u32 def : 5;
                    u32 spd : 5;
                    u32 spatk : 5;
                    u32 spdef : 5;
                    u32 egg : 1;
                    u32 ability : 1;
                } IVs;
                u32 IVint;
            };
            union {
                struct {
                    u8 coolnormal : 1;
                    u8 coolsuper : 1;
                    u8 coolhyper : 1;
                    u8 coolmaster : 1;
                    u8 beautynormal : 1;
                    u8 beautysuper : 1;
                    u8 beautyhyper : 1;
                    u8 beautymaster : 1;
                    u8 cutenormal : 1;
                    u8 cutesuper : 1;
                    u8 cutehyper : 1;
                    u8 cutemaster : 1;
                    u8 smartnormal : 1;
                    u8 smartsuper : 1;
                    u8 smarthyper : 1;
                    u8 smartmaster : 1;
                    u8 toughnormal : 1;
                    u8 toughsuper : 1;
                    u8 toughhyper : 1;
                    u8 toughmaster : 1;
                    u8 champion : 1;
                    u8 winning : 1;
                    u8 victory : 1;
                    u8 artist : 1;
                    u8 effort : 1;
                    u8 marine : 1;
                    u8 land : 1;
                    u8 sky : 1;
                    u8 country : 1;
                    u8 national : 1;
                    u8 earth : 1;
                    u8 world : 1;
                } ribbons;
                u32 ribbonint;
            };
        } pokemon_misc_t;
    };

    // Order:
    //00. GAEM	 06. AGEM	 12. EGAM	 18. MGAE
    //01. GAME	 07. AGME	 13. EGMA	 19. MGEA
    //02. GEAM	 08. AEGM	 14. EAGM	 20. MAGE
    //03. GEMA	 09. AEMG	 15. EAMG	 21. MAEG
    //04. GMAE	 10. AMGE	 16. EMGA	 22. MEGA
    //05. GMEA	 11. AMEG	 17. EMAG	 23. MEAG
    
    // Where in data each struct is, based on AEGM order
    static const int DataOrderTable[ 24 ][ 4 ] = { 
    //    A  E  G  M  
         {1, 2, 0, 3}, 
         {1, 3, 0, 2}, 
         {2, 1, 0, 3}, 
         {3, 1, 0, 2}, 
         {2, 3, 0, 1}, 
         {3, 2, 0, 1}, 
         {0, 2, 1, 3}, 
         {0, 3, 1, 2}, 
         {0, 1, 2, 3}, 
         {0, 1, 3, 2}, 
         {0, 3, 2, 1}, 
         {0, 2, 3, 1}, 
         {2, 0, 1, 3}, 
         {3, 0, 1, 2}, 
         {1, 0, 2, 3}, 
         {1, 0, 3, 2}, 
         {3, 0, 2, 1}, 
         {2, 0, 3, 1}, 
         {2, 3, 1, 0}, 
         {3, 2, 1, 0}, 
         {1, 3, 2, 0}, 
         {1, 2, 3, 0}, 
         {3, 1, 2, 0}, 
         {2, 1, 3, 0} };

    static const int belt_offsets[ ] = { BELT_OFFSET_RSE, BELT_OFFSET_FRLG };

    class SaveParser {
    public:
        static SaveParser* Instance( );
        int get_newest_save( block *[ NUM_BLOCKS_TOTAL ] );
        char* parse_save( block*[ NUM_BLOCKS_TOTAL ] );
        u16 get_block_checksum( block* b );
        int pack_save( char *, block *[ NUM_BLOCKS_TOTAL ], char[ SAVEFILE_LEN ] );
        char* get_text( u8* p_raw, bool p_isNickname );
        //void print_pokemon(box_pokemon_t*);
        int parse_pokemon( char*, int, void**, pokemon::pokemon_moves_t**, pokemon::pokemon_effort_t**, pokemon::pokemon_growth_t**, pokemon::pokemon_misc_t**, int, int );
        u16 encrypt( u8*, u32, u32 );
        int load( int );
        int save( );
        belt_pokemon_t *pokemon[ NUM_BELT_POKEMON ];
        pokemon::pokemon_moves_t *pokemon_moves[ NUM_BELT_POKEMON ];
        pokemon::pokemon_effort_t *pokemon_effort[ NUM_BELT_POKEMON ];
        pokemon::pokemon_growth_t *pokemon_growth[ NUM_BELT_POKEMON ];
        pokemon::pokemon_misc_t *pokemon_misc[ NUM_BELT_POKEMON ];

        char *unpackeddata;
    private:
        SaveParser( ) { };	// Private constructor because this is a singleton
        block *m_blocks[ NUM_BLOCKS_TOTAL ];
        static SaveParser* spInstance;
    };

    u16 encrypt( u8 *p_data, u32 p_pv, u32 p_otid );
    char* get_text( u8* p_raw, int p_maxLen );
    int parse_pokemon( char* p_buf, int p_offset, void** p_pokemon, pokemon::pokemon_moves_t** p_pa, pokemon::pokemon_effort_t** p_pe,
                       pokemon::pokemon_growth_t** p_pg, pokemon::pokemon_misc_t** p_pm, int p_num, int p_size );
    void print_pokemon( box_pokemon_t* p_pokemon );

    pokemon getGen5Pkmn( const pokemon& p_gen3Pkmn );
    pokemon getGen3Pkmn( const pokemon &p_gen5Pkmn );
}
namespace gen4 {
    bool isValidItem( int p_gen5Idx );
    int getItemIdx( int p_gen5Idx );
    int getItemIdx( int p_gen4Idx );

    class pokemon {

    };

    pokemon getGen5Pkmn( pokemon p_gen4Pkmn );
    pokemon getGen4Pkmn( pokemon p_gen5Pkmn );

    enum GAME {
        DIAMANT = 0,
        PERL = 1,
        PLATIN = 2,
        HEARTGOLD = 3,
        SOULSILVER = 4
    };
}
*/