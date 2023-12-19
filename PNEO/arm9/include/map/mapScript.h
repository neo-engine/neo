/*
Pokémon neo
------------------------------

file        : mapScript.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2023
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
#include <nds.h>

namespace MAP {
#define MAX_SCRIPT_SIZE 128
    // opcode : u8, param1 : u8, param2 : u8, param3 : u8
    // opcode : u8, param1x : u11, param2x : u5, param3x : u8
    // opcode : u8, param1s : u5, param2s : u5, param3s : u14
    // opcode : u8, paramA : u12, paramB : u12
#define OPCODE( p_ins )  ( ( p_ins ) >> 24 )
#define PARAM1( p_ins )  ( ( ( p_ins ) >> 16 ) & 0xFF )
#define PARAM2( p_ins )  ( ( ( p_ins ) >> 8 ) & 0xFF )
#define PARAM3( p_ins )  ( ( p_ins ) & ( 0xFF ) )
#define PARAM1X( p_ins ) ( ( ( p_ins ) >> 13 ) & 0x7FF )
#define PARAM2X( p_ins ) ( ( ( p_ins ) >> 8 ) & 0x1F )
#define PARAM3X( p_ins ) ( ( p_ins ) & ( 0xFF ) )
#define PARAM1S( p_ins ) ( ( ( p_ins ) >> 19 ) & 0x1F )
#define PARAM2S( p_ins ) ( ( ( p_ins ) >> 14 ) & 0x1F )
#define PARAM3S( p_ins ) ( ( p_ins ) & ( 0x3FFF ) )
#define PARAMA( p_ins )  ( ( ( p_ins ) >> 12 ) & 0xFFF )
#define PARAMB( p_ins )  ( ( p_ins ) & ( 0xFFF ) )

    // special functions
    constexpr u8 CLL_HEAL_ENTIRE_TEAM     = 1;
    constexpr u8 CLL_RUN_POKE_MART        = 2;
    constexpr u8 CLL_GET_BADGE_COUNT      = 3; // (param1: region [0: Hoenn, 1: battle frontier])
    constexpr u8 CLL_INIT_GAME_ITEM_COUNT = 4;
    constexpr u8 CLL_GET_AND_REMOVE_INIT_GAME_ITEM = 5;
    constexpr u8 CLL_RUN_INITIAL_PKMN_SELECTION    = 6;
    constexpr u8 CLL_NAV_INIT                      = 7; // IO::init( ) from io/message.h
    constexpr u8 CLL_RUN_CATCHING_TUTORIAL         = 8;
    constexpr u8 CLL_AWARD_BADGE                   = 9; // (p1: region, p2: badge)
    constexpr u8 CLL_RUN_CHOICE_BOX                = 10;
    constexpr u8 CLL_GET_CURRENT_DAYTIME           = 11; // getCurrentDaytime( )
    constexpr u8 CLL_DAYCARE_BAA_SAN = 12; // (par: # of day care) (take/hand over pkmn)
    constexpr u8 CLL_DAYCARE_JII_SAN = 13; // (obtain egg)
    constexpr u8 CLL_SAVE_GAME       = 14;
    constexpr u8 CLL_HOURS_MOD       = 15; // current time (hours) % par1 (used for
                                           // shoal cave
    constexpr u8 CLL_PLAYTIME_HOURS = 16;
    constexpr u8 CLL_HALL_OF_FAME   = 17; // register current party in hall of fame, save
                                          // game, warp home
    constexpr u8 CLL_INIT_INFINITY_CAVE      = 18;
    constexpr u8 CLL_CONTINUE_INFINITY_CAVE  = 19;
    constexpr u8 CLL_MAPENTER_INFINITY_CAVE  = 20;
    constexpr u8 CLL_GATECHECK_INFINITY_CAVE = 21;
    constexpr u8 CLL_PKMN_SELF_TRADER        = 22;
    constexpr u8 CLL_PKMN_INGAME_TRADE       = 23;
    constexpr u8 CLL_MYSTERY_EVENT_CLERK     = 24;

    // battle zone facilities
    constexpr u8 BTZ_BATTLE_FACTORY = 0;

    enum opCode : u8 {
        EOP = 0,  // end of program
        SMO = 1,  // set map object
        MMO = 2,  // move map object
        DMO = 3,  // destroy map object
        CFL = 4,  // check flag
        SFL = 5,  // set flag
        CRG = 6,  // check register
        SRG = 7,  // set register
        MRG = 8,  // move register value
        JMP = 9,  // jump
        JMB = 10, // jump backwards

        SMOR = 11, // set map object
        MMOR = 12, // move map object
        DMOR = 13, // destroy map object
        CFLR = 14, // check flag
        SFLR = 15, // set flag

        CRGL = 16, // check register lower
        CRGG = 17, // check register greater
        CRGN = 18, // check register not equal
        MPL  = 19, // move player
        CMO  = 20, // current map object id to reg 0
        GMO  = 21, // Get map object id of object at specified position to reg 0

        CPP = 22, // check player position

        LCKR = 23, // Lock map object
        ULKR = 24, // unlock map object

        BNK = 25, // warp player (bank, z) (needs to be before WRP)
        WRP = 26, // warp player (x, y)

        CVR  = 27, // check variable equal
        CVRN = 28, // check variable not equal
        CVRG = 29, // check variable greater
        CVRL = 30, // check variable lower

        MFO  = 31, // move map object fast
        MFOR = 32, // move map object fast

        GIT = 33, // return how many copies the player has of the specified item

        STF = 34, // set trainer flag
        CTF = 35, // check trainer flag

        ADD = 36, // reg[ par1 ] += par2
        ARG = 37, // reg[ par1 ] += reg[ par2 ]
        DIV = 38, // reg[ par1 ] /= par2
        DRG = 39, // reg[ par1 ] /= reg[ par2 ]

        HPL = 40, // hide player sprite
        SPL = 41, // show player sprite
        WPL = 42, // walk player (also through walls, etc)

        MINR = 43, // reg[ par3 ] = min( reg[ par1 ], reg[ par2 ] )
        MAXR = 44, // reg[ par3 ] = max( reg[ par1 ], reg[ par2 ] )

        GVR  = 45, // get value of variable and write it to register parB
        SVR  = 46, // set value of variable to parB
        SVRR = 47, // set value of variable to reg[ parB ]
        SUB  = 48, // reg[ par1 ] -= par2
        SUBR = 49, // reg[ par1 ] -= reg[ par2 ]

        FMM = 50, // force movement mode (player cannot change move mode themselves)
        UMM = 51, // unlock movement mode
        GMM = 52, // write current movement mode to reg 0
        CMM = 53, // change movement

        PRM  = 54, // check pkmn <param3s> at party slot <p1s>, skip <p2s> if true
        PRMA = 55, // check pkmn <param3s> appears at least <p1s> times in total in any party slot,
                   // skip <p2s> if true

        HPK = 60, // Hide following pkmn

        CMN = 70, // check money >=
        PMN = 71, // pay money

        EXM  = 87, // Exclamation mark
        EXMR = 88, // Exclamation mark (register)
        RDR  = 89, // Redraw objects
        ATT  = 90, // Attach player
        REM  = 91, // Remove player
        FIXR = 92, // Make map object to obtain same pos in map obj arr
        UFXR = 93, // Make map object to obtain same pos in map obj arr

        FNT  = 99,  // faint player
        BTR  = 100, // Battle trainer
        BPK  = 101, // Battle pkmn
        ITM  = 102, // Give item
        TTM  = 103, // Take item
        UTM  = 104, // Use item
        BTRR = 105, // Battle trainer (TODO)
        BPKR = 106, // Battle pkmn (TODO)
        ITMR = 107, // Give item
        TTMR = 108, // Take item
        UTMR = 109, // Use item

        SRT = 110, // set route
        CRT = 111, // check route

        COUR = 112, // Counter message (make player select number between 0 and reg[ parB ]
        MSC  = 113, // play music (temporary)
        RMS  = 114, // Reset music
        CRY  = 115, // Play cry
        SFX  = 116, // Play sound effect
        PMO  = 117, // Play music oneshot
        SMC  = 118, // Set music (TODO)
        SLC  = 119, // Set location (TODO)
        SWT  = 120, // Set weather to parA
        WAT  = 121, // Wait
        MBG  = 122, // Pokemart description begin
        MIT  = 123, // Mart item
        COU  = 124, // Counter message (make player select number between 0 and parB
        YNM  = 125, // yes no message
        CLL  = 126, // Call special function
        MSG  = 127, // message
        CBG  = 128, // choice box begin
        CIT  = 129, // choice item
        BTZ  = 130, // battle zone facility script

        MAP = 140, // redraw current map
        EQ  = 141, // earthquake animation

        DES = 150, // register pkmn as seen in pkdex

        SBC  = 196, // set block
        SBCC = 197, // set block, with player dir correction
        SMM  = 198, // set movement
    };
} // namespace MAP
