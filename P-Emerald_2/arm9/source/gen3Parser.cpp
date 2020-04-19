/*
Pokémon neo
------------------------------

file        : gen3parser.cpp
author      : Philip Wellnitz
description :

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

#include <cstdio>
#include <cstdlib>
#include <cstring>
//#include <iostream>
#include <sstream>

#include "gen3Parser.h"
#include "pokemon.h"
#include "saveGame.h"

namespace FS {
#define MAX_TRAINER_NAME_LEN 7
#define MAX_NICKNAME_LEN 10

    int gbatods[ 386 ][ 2 ]
        = {{1, 1},     {2, 2},     {3, 3},     {4, 4},     {5, 5},     {6, 6},     {7, 7},
           {8, 8},     {9, 9},     {10, 10},   {11, 11},   {12, 12},   {13, 13},   {14, 14},
           {15, 15},   {16, 16},   {17, 17},   {18, 18},   {19, 19},   {20, 20},   {21, 21},
           {22, 22},   {23, 23},   {24, 24},   {25, 25},   {26, 26},   {27, 27},   {28, 28},
           {29, 29},   {30, 30},   {31, 31},   {32, 32},   {33, 33},   {34, 34},   {35, 35},
           {36, 36},   {37, 37},   {38, 38},   {39, 39},   {40, 40},   {41, 41},   {42, 42},
           {43, 43},   {44, 44},   {45, 45},   {46, 46},   {47, 47},   {48, 48},   {49, 49},
           {50, 50},   {51, 51},   {52, 52},   {53, 53},   {54, 54},   {55, 55},   {56, 56},
           {57, 57},   {58, 58},   {59, 59},   {60, 60},   {61, 61},   {62, 62},   {63, 63},
           {64, 64},   {65, 65},   {66, 66},   {67, 67},   {68, 68},   {69, 69},   {70, 70},
           {71, 71},   {72, 72},   {73, 73},   {74, 74},   {75, 75},   {76, 76},   {77, 77},
           {78, 78},   {79, 79},   {80, 80},   {81, 81},   {82, 82},   {83, 83},   {84, 84},
           {85, 85},   {86, 86},   {87, 87},   {88, 88},   {89, 89},   {90, 90},   {91, 91},
           {92, 92},   {93, 93},   {94, 94},   {95, 95},   {96, 96},   {97, 97},   {98, 98},
           {99, 99},   {100, 100}, {101, 101}, {102, 102}, {103, 103}, {104, 104}, {105, 105},
           {106, 106}, {107, 107}, {108, 108}, {109, 109}, {110, 110}, {111, 111}, {112, 112},
           {113, 113}, {114, 114}, {115, 115}, {116, 116}, {117, 117}, {118, 118}, {119, 119},
           {120, 120}, {121, 121}, {122, 122}, {123, 123}, {124, 124}, {125, 125}, {126, 126},
           {127, 127}, {128, 128}, {129, 129}, {130, 130}, {131, 131}, {132, 132}, {133, 133},
           {134, 134}, {135, 135}, {136, 136}, {137, 137}, {138, 138}, {139, 139}, {140, 140},
           {141, 141}, {142, 142}, {143, 143}, {144, 144}, {145, 145}, {146, 146}, {147, 147},
           {148, 148}, {149, 149}, {150, 150}, {151, 151}, {152, 152}, {153, 153}, {154, 154},
           {155, 155}, {156, 156}, {157, 157}, {158, 158}, {159, 159}, {160, 160}, {161, 161},
           {162, 162}, {163, 163}, {164, 164}, {165, 165}, {166, 166}, {167, 167}, {168, 168},
           {169, 169}, {170, 170}, {171, 171}, {172, 172}, {173, 173}, {174, 174}, {175, 175},
           {176, 176}, {177, 177}, {178, 178}, {179, 179}, {180, 180}, {181, 181}, {182, 182},
           {183, 183}, {184, 184}, {185, 185}, {186, 186}, {187, 187}, {188, 188}, {189, 189},
           {190, 190}, {191, 191}, {192, 192}, {193, 193}, {194, 194}, {195, 195}, {196, 196},
           {197, 197}, {198, 198}, {199, 199}, {200, 200}, {201, 201}, {202, 202}, {203, 203},
           {204, 204}, {205, 205}, {206, 206}, {207, 207}, {208, 208}, {209, 209}, {210, 210},
           {211, 211}, {212, 212}, {213, 213}, {214, 214}, {215, 215}, {216, 216}, {217, 217},
           {218, 218}, {219, 219}, {220, 220}, {221, 221}, {222, 222}, {223, 223}, {224, 224},
           {225, 225}, {226, 226}, {227, 227}, {228, 228}, {229, 229}, {230, 230}, {231, 231},
           {232, 232}, {233, 233}, {234, 234}, {235, 235}, {236, 236}, {237, 237}, {238, 238},
           {239, 239}, {240, 240}, {241, 241}, {242, 242}, {243, 243}, {244, 244}, {245, 245},
           {246, 246}, {247, 247}, {248, 248}, {249, 249}, {250, 250}, {251, 251}, {277, 252},
           {278, 253}, {279, 254}, {280, 255}, {281, 256}, {282, 257}, {283, 258}, {284, 259},
           {285, 260}, {286, 261}, {287, 262}, {288, 263}, {289, 264}, {290, 265}, {291, 266},
           {292, 267}, {293, 268}, {294, 269}, {295, 270}, {296, 271}, {297, 272}, {298, 273},
           {299, 274}, {300, 275}, {304, 276}, {305, 277}, {309, 278}, {310, 279}, {392, 280},
           {393, 281}, {394, 282}, {311, 283}, {312, 284}, {306, 285}, {307, 286}, {364, 287},
           {365, 288}, {366, 289}, {301, 290}, {302, 291}, {303, 292}, {370, 293}, {371, 294},
           {372, 295}, {335, 296}, {336, 297}, {350, 298}, {320, 299}, {315, 300}, {316, 301},
           {322, 302}, {355, 303}, {382, 304}, {383, 305}, {384, 306}, {356, 307}, {357, 308},
           {337, 309}, {338, 310}, {353, 311}, {354, 312}, {386, 313}, {387, 314}, {363, 315},
           {367, 316}, {368, 317}, {330, 318}, {331, 319}, {313, 320}, {314, 321}, {339, 322},
           {340, 323}, {321, 324}, {351, 325}, {352, 326}, {308, 327}, {332, 328}, {333, 329},
           {334, 330}, {344, 331}, {345, 332}, {358, 333}, {359, 334}, {380, 335}, {379, 336},
           {348, 337}, {349, 338}, {323, 339}, {324, 340}, {326, 341}, {327, 342}, {318, 343},
           {319, 344}, {388, 345}, {389, 346}, {390, 347}, {391, 348}, {328, 349}, {329, 350},
           {385, 351}, {317, 352}, {377, 353}, {378, 354}, {361, 355}, {362, 356}, {369, 357},
           {411, 358}, {376, 359}, {360, 360}, {346, 361}, {347, 362}, {341, 363}, {342, 364},
           {343, 365}, {373, 366}, {374, 367}, {375, 368}, {381, 369}, {325, 370}, {395, 371},
           {396, 372}, {397, 373}, {398, 374}, {399, 375}, {400, 376}, {401, 377}, {402, 378},
           {403, 379}, {407, 380}, {408, 381}, {404, 382}, {405, 383}, {406, 384}, {409, 385},
           {410, 386}};

    int getNPKMNIdx( int gen3idx ) {
        for( int i = 0; i < 386; i++ )
            if( int( gen3idx ) == gbatods[ i ][ 0 ] ) return gbatods[ i ][ 1 ];
        return 0;
    }
    int getPKMNIdx( int gen5idx ) {
        for( int i = 0; i < 386; i++ )
            if( int( gen5idx ) == gbatods[ i ][ 1 ] ) return gbatods[ i ][ 0 ];
        return 0;
    }

    int gbatodsitems[ 310 ][ 2 ]
        = {{0, 0},     {1, 1},     {2, 2},     {3, 3},     {4, 4},     {5, 5},     {6, 6},
           {7, 7},     {8, 8},     {9, 9},     {10, 10},   {11, 11},   {12, 12},   {13, 17},
           {14, 18},   {15, 19},   {16, 20},   {17, 21},   {18, 22},   {19, 23},   {20, 24},
           {21, 25},   {22, 26},   {23, 27},   {24, 28},   {25, 29},   {26, 30},   {27, 31},
           {28, 32},   {29, 33},   {30, 34},   {31, 35},   {32, 36},   {33, 37},   {34, 38},
           {35, 39},   {36, 40},   {37, 41},   {38, 42},   {39, 65},   {40, 66},   {41, 67},
           {42, 68},   {43, 69},   {44, 43},   {45, 44},   {46, 70},   {47, 71},   {48, 72},
           {49, 73},   {50, 74},   {51, 75},   {63, 45},   {64, 46},   {65, 47},   {66, 48},
           {67, 49},   {68, 50},   {69, 51},   {70, 52},   {71, 53},   {73, 55},   {74, 56},
           {75, 57},   {76, 58},   {77, 59},   {78, 60},   {79, 61},   {80, 63},   {81, 64},
           {83, 76},   {84, 77},   {85, 78},   {86, 79},   {93, 80},   {94, 81},   {95, 82},
           {96, 83},   {97, 84},   {98, 85},   {103, 86},  {104, 87},  {106, 88},  {107, 89},
           {108, 90},  {109, 91},  {110, 92},  {111, 93},  {121, 0},   {122, 0},   {123, 0},
           {124, 0},   {125, 0},   {126, 0},   {127, 0},   {128, 0},   {129, 0},   {130, 0},
           {131, 0},   {132, 0},   {133, 149}, {134, 150}, {135, 151}, {136, 152}, {137, 153},
           {138, 154}, {139, 155}, {140, 156}, {141, 157}, {142, 158}, {143, 159}, {144, 160},
           {145, 161}, {146, 162}, {147, 163}, {148, 164}, {149, 165}, {150, 166}, {151, 167},
           {152, 168}, {153, 169}, {154, 170}, {155, 171}, {156, 172}, {157, 173}, {158, 174},
           {159, 175}, {160, 176}, {161, 177}, {162, 178}, {163, 179}, {164, 180}, {165, 181},
           {166, 182}, {167, 183}, {168, 201}, {169, 202}, {170, 203}, {171, 204}, {172, 205},
           {173, 206}, {174, 207}, {175, 208}, {179, 213}, {180, 214}, {181, 215}, {182, 216},
           {183, 217}, {184, 218}, {185, 219}, {186, 220}, {187, 221}, {188, 222}, {189, 223},
           {190, 224}, {191, 225}, {192, 226}, {193, 227}, {194, 228}, {195, 229}, {196, 230},
           {197, 231}, {198, 232}, {199, 233}, {200, 234}, {201, 235}, {202, 236}, {203, 237},
           {204, 238}, {205, 239}, {206, 240}, {207, 241}, {208, 242}, {209, 243}, {210, 244},
           {211, 245}, {212, 246}, {213, 247}, {214, 248}, {215, 249}, {216, 250}, {217, 251},
           {218, 252}, {219, 253}, {220, 254}, {221, 255}, {222, 256}, {223, 257}, {224, 258},
           {225, 259}, {254, 260}, {255, 261}, {256, 262}, {257, 263}, {258, 264}, {259, 0},
           {260, 444}, {261, 471}, {262, 445}, {263, 446}, {264, 447}, {265, 456}, {266, 457},
           {268, 0},   {269, 0},   {270, 0},   {271, 476}, {272, 0},   {273, 0},   {274, 0},
           {275, 0},   {276, 534}, {277, 535}, {278, 0},   {279, 0},   {280, 0},   {281, 0},
           {282, 0},   {283, 0},   {284, 0},   {285, 463}, {286, 99},  {287, 100}, {288, 0},
           {289, 328}, {290, 329}, {291, 330}, {292, 331}, {293, 332}, {294, 333}, {295, 334},
           {296, 335}, {297, 336}, {298, 337}, {299, 338}, {300, 339}, {301, 340}, {302, 341},
           {303, 342}, {304, 343}, {305, 344}, {306, 345}, {307, 346}, {308, 347}, {309, 348},
           {310, 349}, {311, 350}, {312, 351}, {313, 352}, {314, 353}, {315, 354}, {316, 355},
           {317, 356}, {318, 357}, {319, 358}, {320, 359}, {321, 360}, {322, 361}, {323, 362},
           {324, 363}, {325, 364}, {326, 365}, {327, 366}, {328, 367}, {329, 368}, {330, 369},
           {331, 370}, {332, 371}, {333, 372}, {334, 373}, {335, 374}, {336, 375}, {337, 376},
           {338, 377}, {339, 420}, {340, 421}, {341, 422}, {342, 423}, {343, 424}, {344, 0},
           {345, 0},   {346, 0},   {349, 0},   {350, 0},   {351, 467}, {352, 0},   {353, 0},
           {354, 103}, {355, 475}, {356, 0},   {357, 101}, {358, 102}, {359, 0},   {360, 450},
           {361, 442}, {362, 443}, {363, 0},   {364, 0},   {365, 0},   {366, 0},   {367, 0},
           {368, 0},   {369, 0},   {370, 0},   {371, 0},   {372, 0},   {373, 0},   {374, 0},
           {375, 0},   {376, 0}};

    int getNItemIdx( int p_gen3Idx ) {
        for( int i = 0; i < 310; i++ )
            if( int( p_gen3Idx ) == gbatodsitems[ i ][ 0 ] ) return gbatodsitems[ i ][ 1 ];
        return 0;
    }
    int getItemIdx( int p_gen5Idx ) {
        for( int i = 0; i < 310; i++ )
            if( int( p_gen5Idx ) == gbatodsitems[ i ][ 1 ] ) return gbatodsitems[ i ][ 0 ];
        return 0;
    }

#if( defined __linux__ ) || ( defined __APPLE__ )
    int gbatounicode[ 240 ][ 2 ]{
        {0x00, L'\0'}, {0x01, L'あ'}, {0x02, L'い'}, {0x03, L'う'}, {0x04, L'え'}, {0x05, L'お'},
        {0x06, L'か'}, {0x07, L'き'}, {0x08, L'く'}, {0x09, L'け'}, {0x0A, L'こ'}, {0x0B, L'さ'},
        {0x0C, L'し'}, {0x0D, L'す'}, {0x0E, L'せ'}, {0x0F, L'そ'}, {0x10, L'た'}, {0x11, L'ち'},
        {0x12, L'つ'}, {0x13, L'て'}, {0x14, L'と'}, {0x15, L'な'}, {0x16, L'に'}, {0x17, L'ぬ'},
        {0x18, L'ね'}, {0x19, L'の'}, {0x1A, L'は'}, {0x1B, L'ひ'}, {0x1C, L'ふ'}, {0x1D, L'へ'},
        {0x1E, L'ほ'}, {0x1F, L'ま'}, {0x20, L'み'}, {0x21, L'む'}, {0x22, L'め'}, {0x23, L'も'},
        {0x24, L'や'}, {0x25, L'ゆ'}, {0x26, L'よ'}, {0x27, L'ら'}, {0x28, L'り'}, {0x29, L'る'},
        {0x2A, L'れ'}, {0x2B, L'ろ'}, {0x2C, L'わ'}, {0x2D, L'を'}, {0x2E, L'ん'}, {0x2F, L'ぁ'},
        {0x30, L'ぃ'}, {0x31, L'ぅ'}, {0x32, L'ぇ'}, {0x33, L'ぉ'}, {0x34, L'ゃ'}, {0x35, L'ゅ'},
        {0x36, L'ょ'}, {0x37, L'が'}, {0x38, L'ぎ'}, {0x39, L'ぐ'}, {0x3A, L'げ'}, {0x3B, L'ご'},
        {0x3C, L'ざ'}, {0x3D, L'じ'}, {0x3E, L'ず'}, {0x3F, L'ぜ'}, {0x40, L'ぞ'}, {0x41, L'だ'},
        {0x42, L'ぢ'}, {0x43, L'づ'}, {0x44, L'で'}, {0x45, L'ど'}, {0x46, L'ば'}, {0x47, L'び'},
        {0x48, L'ぶ'}, {0x49, L'べ'}, {0x4A, L'ぼ'}, {0x4B, L'ぱ'}, {0x4C, L'ぴ'}, {0x4D, L'ぷ'},
        {0x4E, L'ぺ'}, {0x4F, L'ぽ'}, {0x50, L'\0'}, {0x51, L'ア'}, {0x52, L'イ'}, {0x53, L'ウ'},
        {0x54, L'エ'}, {0x55, L'オ'}, {0x56, L'カ'}, {0x57, L'キ'}, {0x58, L'ク'}, {0x59, L'ケ'},
        {0x5A, L'コ'}, {0x5B, L'サ'}, {0x5C, L'シ'}, {0x5D, L'ス'}, {0x5E, L'セ'}, {0x5F, L'ソ'},
        {0x60, L'タ'}, {0x61, L'チ'}, {0x62, L'ツ'}, {0x63, L'テ'}, {0x64, L'ト'}, {0x65, L'ナ'},
        {0x66, L'ニ'}, {0x67, L'ヌ'}, {0x68, L'ネ'}, {0x69, L'ノ'}, {0x6A, L'ハ'}, {0x6B, L'ヒ'},
        {0x6C, L'フ'}, {0x6D, L'ヘ'}, {0x6E, L'ホ'}, {0x6F, L'マ'}, {0x70, L'ミ'}, {0x71, L'ム'},
        {0x72, L'メ'}, {0x73, L'モ'}, {0x74, L'ヤ'}, {0x75, L'ユ'}, {0x76, L'ヨ'}, {0x77, L'ラ'},
        {0x78, L'リ'}, {0x79, L'ル'}, {0x7A, L'レ'}, {0x7B, L'ロ'}, {0x7C, L'ワ'}, {0x7D, L'ヲ'},
        {0x7E, L'ン'}, {0x7F, L'ァ'}, {0x80, L'ィ'}, {0x81, L'ゥ'}, {0x82, L'ェ'}, {0x83, L'ォ'},
        {0x84, L'ャ'}, {0x85, L'ュ'}, {0x86, L'ョ'}, {0x87, L'ガ'}, {0x88, L'ギ'}, {0x89, L'グ'},
        {0x8A, L'ゲ'}, {0x8B, L'ゴ'}, {0x8C, L'ザ'}, {0x8D, L'ジ'}, {0x8E, L'ズ'}, {0x8F, L'ゼ'},
        {0x90, L'ゾ'}, {0x91, L'ダ'}, {0x92, L'ヂ'}, {0x93, L'ヅ'}, {0x94, L'デ'}, {0x95, L'ド'},
        {0x96, L'バ'}, {0x97, L'ビ'}, {0x98, L'ブ'}, {0x99, L'ベ'}, {0x9A, L'ボ'}, {0x9B, L'パ'},
        {0x9C, L'ピ'}, {0x9D, L'プ'}, {0x9E, L'ペ'}, {0x9F, L'ポ'}, {0xA0, L'\0'}, {0xA1, L'0'},
        {0xA2, L'1'},  {0xA3, L'2'},  {0xA4, L'3'},  {0xA5, L'4'},  {0xA6, L'5'},  {0xA7, L'6'},
        {0xA8, L'7'},  {0xA9, L'8'},  {0xAA, L'9'},  {0xAB, L'!'},  {0xAC, L'?'},  {0xAD, L'.'},
        {0xAE, L'-'},  {0xAF, L'\0'}, {0xB0, L'…'},  {0xB1, L'“'},  {0xB2, L'”'},  {0xB3, L'‘'},
        {0xB4, L'’'},  {0xB5, L'♂'},  {0xB6, L'♀'},  {0xB7, L'\0'}, {0xB8, L',L'}, {0xB9, L'\0'},
        {0xBA, L'/'},  {0xBB, L'A'},  {0xBC, L'B'},  {0xBD, L'C'},  {0xBE, L'D'},  {0xBF, L'E'},
        {0xC0, L'F'},  {0xC1, L'G'},  {0xC2, L'H'},  {0xC3, L'I'},  {0xC4, L'J'},  {0xC5, L'K'},
        {0xC6, L'L'},  {0xC7, L'M'},  {0xC8, L'N'},  {0xC9, L'O'},  {0xCA, L'P'},  {0xCB, L'Q'},
        {0xCC, L'R'},  {0xCD, L'S'},  {0xCE, L'T'},  {0xCF, L'U'},  {0xD0, L'V'},  {0xD1, L'W'},
        {0xD2, L'X'},  {0xD3, L'Y'},  {0xD4, L'Z'},  {0xD5, L'a'},  {0xD6, L'b'},  {0xD7, L'c'},
        {0xD8, L'd'},  {0xD9, L'e'},  {0xDA, L'f'},  {0xDB, L'g'},  {0xDC, L'h'},  {0xDD, L'i'},
        {0xDE, L'j'},  {0xDF, L'k'},  {0xE0, L'l'},  {0xE1, L'm'},  {0xE2, L'n'},  {0xE3, L'o'},
        {0xE4, L'p'},  {0xE5, L'q'},  {0xE6, L'r'},  {0xE7, L's'},  {0xE8, L't'},  {0xE9, L'u'},
        {0xEA, L'v'},  {0xEB, L'w'},  {0xEC, L'x'},  {0xED, L'y'},  {0xEE, L'z'},  {0xEF, L'\0'},
    };
#else
    int gbatounicode[ 240 ][ 2 ] = {
        {0x00, '\0'}, {0x01, 'あ'}, {0x02, 'い'}, {0x03, 'う'}, {0x04, 'え'}, {0x05, 'お'},
        {0x06, 'か'}, {0x07, 'き'}, {0x08, 'く'}, {0x09, 'け'}, {0x0A, 'こ'}, {0x0B, 'さ'},
        {0x0C, 'し'}, {0x0D, 'す'}, {0x0E, 'せ'}, {0x0F, 'そ'}, {0x10, 'た'}, {0x11, 'ち'},
        {0x12, 'つ'}, {0x13, 'て'}, {0x14, 'と'}, {0x15, 'な'}, {0x16, 'に'}, {0x17, 'ぬ'},
        {0x18, 'ね'}, {0x19, 'の'}, {0x1A, 'は'}, {0x1B, 'ひ'}, {0x1C, 'ふ'}, {0x1D, 'へ'},
        {0x1E, 'ほ'}, {0x1F, 'ま'}, {0x20, 'み'}, {0x21, 'む'}, {0x22, 'め'}, {0x23, 'も'},
        {0x24, 'や'}, {0x25, 'ゆ'}, {0x26, 'よ'}, {0x27, 'ら'}, {0x28, 'り'}, {0x29, 'る'},
        {0x2A, 'れ'}, {0x2B, 'ろ'}, {0x2C, 'わ'}, {0x2D, 'を'}, {0x2E, 'ん'}, {0x2F, 'ぁ'},
        {0x30, 'ぃ'}, {0x31, 'ぅ'}, {0x32, 'ぇ'}, {0x33, 'ぉ'}, {0x34, 'ゃ'}, {0x35, 'ゅ'},
        {0x36, 'ょ'}, {0x37, 'が'}, {0x38, 'ぎ'}, {0x39, 'ぐ'}, {0x3A, 'げ'}, {0x3B, 'ご'},
        {0x3C, 'ざ'}, {0x3D, 'じ'}, {0x3E, 'ず'}, {0x3F, 'ぜ'}, {0x40, 'ぞ'}, {0x41, 'だ'},
        {0x42, 'ぢ'}, {0x43, 'づ'}, {0x44, 'で'}, {0x45, 'ど'}, {0x46, 'ば'}, {0x47, 'び'},
        {0x48, 'ぶ'}, {0x49, 'べ'}, {0x4A, 'ぼ'}, {0x4B, 'ぱ'}, {0x4C, 'ぴ'}, {0x4D, 'ぷ'},
        {0x4E, 'ぺ'}, {0x4F, 'ぽ'}, {0x50, '\0'}, {0x51, 'ア'}, {0x52, 'イ'}, {0x53, 'ウ'},
        {0x54, 'エ'}, {0x55, 'オ'}, {0x56, 'カ'}, {0x57, 'キ'}, {0x58, 'ク'}, {0x59, 'ケ'},
        {0x5A, 'コ'}, {0x5B, 'サ'}, {0x5C, 'シ'}, {0x5D, 'ス'}, {0x5E, 'セ'}, {0x5F, 'ソ'},
        {0x60, 'タ'}, {0x61, 'チ'}, {0x62, 'ツ'}, {0x63, 'テ'}, {0x64, 'ト'}, {0x65, 'ナ'},
        {0x66, 'ニ'}, {0x67, 'ヌ'}, {0x68, 'ネ'}, {0x69, 'ノ'}, {0x6A, 'ハ'}, {0x6B, 'ヒ'},
        {0x6C, 'フ'}, {0x6D, 'ヘ'}, {0x6E, 'ホ'}, {0x6F, 'マ'}, {0x70, 'ミ'}, {0x71, 'ム'},
        {0x72, 'メ'}, {0x73, 'モ'}, {0x74, 'ヤ'}, {0x75, 'ユ'}, {0x76, 'ヨ'}, {0x77, 'ラ'},
        {0x78, 'リ'}, {0x79, 'ル'}, {0x7A, 'レ'}, {0x7B, 'ロ'}, {0x7C, 'ワ'}, {0x7D, 'ヲ'},
        {0x7E, 'ン'}, {0x7F, 'ァ'}, {0x80, 'ィ'}, {0x81, 'ゥ'}, {0x82, 'ェ'}, {0x83, 'ォ'},
        {0x84, 'ャ'}, {0x85, 'ュ'}, {0x86, 'ョ'}, {0x87, 'ガ'}, {0x88, 'ギ'}, {0x89, 'グ'},
        {0x8A, 'ゲ'}, {0x8B, 'ゴ'}, {0x8C, 'ザ'}, {0x8D, 'ジ'}, {0x8E, 'ズ'}, {0x8F, 'ゼ'},
        {0x90, 'ゾ'}, {0x91, 'ダ'}, {0x92, 'ヂ'}, {0x93, 'ヅ'}, {0x94, 'デ'}, {0x95, 'ド'},
        {0x96, 'バ'}, {0x97, 'ビ'}, {0x98, 'ブ'}, {0x99, 'ベ'}, {0x9A, 'ボ'}, {0x9B, 'パ'},
        {0x9C, 'ピ'}, {0x9D, 'プ'}, {0x9E, 'ペ'}, {0x9F, 'ポ'}, {0xA0, '\0'}, {0xA1, '0'},
        {0xA2, '1'},  {0xA3, '2'},  {0xA4, '3'},  {0xA5, '4'},  {0xA6, '5'},  {0xA7, '6'},
        {0xA8, '7'},  {0xA9, '8'},  {0xAA, '9'},  {0xAB, '!'},  {0xAC, '?'},  {0xAD, '.'},
        {0xAE, '-'},  {0xAF, '\0'}, {0xB0, '…'},  {0xB1, '“'},  {0xB2, '”'},  {0xB3, '‘'},
        {0xB4, '’'},  {0xB5, '♂'},  {0xB6, '♀'},  {0xB7, '\0'}, {0xB8, ','},  {0xB9, '\0'},
        {0xBA, '/'},  {0xBB, 'A'},  {0xBC, 'B'},  {0xBD, 'C'},  {0xBE, 'D'},  {0xBF, 'E'},
        {0xC0, 'F'},  {0xC1, 'G'},  {0xC2, 'H'},  {0xC3, 'I'},  {0xC4, 'J'},  {0xC5, 'K'},
        {0xC6, 'L'},  {0xC7, 'M'},  {0xC8, 'N'},  {0xC9, 'O'},  {0xCA, 'P'},  {0xCB, 'Q'},
        {0xCC, 'R'},  {0xCD, 'S'},  {0xCE, 'T'},  {0xCF, 'U'},  {0xD0, 'V'},  {0xD1, 'W'},
        {0xD2, 'X'},  {0xD3, 'Y'},  {0xD4, 'Z'},  {0xD5, 'a'},  {0xD6, 'b'},  {0xD7, 'c'},
        {0xD8, 'd'},  {0xD9, 'e'},  {0xDA, 'f'},  {0xDB, 'g'},  {0xDC, 'h'},  {0xDD, 'i'},
        {0xDE, 'j'},  {0xDF, 'k'},  {0xE0, 'l'},  {0xE1, 'm'},  {0xE2, 'n'},  {0xE3, 'o'},
        {0xE4, 'p'},  {0xE5, 'q'},  {0xE6, 'r'},  {0xE7, 's'},  {0xE8, 't'},  {0xE9, 'u'},
        {0xEA, 'v'},  {0xEB, 'w'},  {0xEC, 'x'},  {0xED, 'y'},  {0xEE, 'z'},  {0xEF, '\0'},
    };
#endif
    int getNText( int p_in ) {
        for( int i = 0; i < 240; i++ )
            if( p_in == int( gbatounicode[ i ][ 0 ] ) ) return int( gbatounicode[ i ][ 1 ] );
        return '\0';
    }

    int getNLocation( int p_gen3Idx ) {
        if( p_gen3Idx < 88 ) return p_gen3Idx + 235;
        if( p_gen3Idx < 99 ) return p_gen3Idx - 88 + 138;
        if( p_gen3Idx == 99 ) return 152;
        if( p_gen3Idx == 100 ) return 158;
        if( p_gen3Idx < 126 ) return p_gen3Idx - 101 + 149;

        return 322 + p_gen3Idx;
    }
    static void smemcpy( u8* p_dest, const u8* p_src, int p_size ) {
        while( p_size-- ) *p_dest++ = *p_src++;
    }
    gen3Parser* gen3Parser::spInstance = NULL;

    int gen3Parser::get_newest_save( block* p_blocks[ NUM_BLOCKS_TOTAL ] ) {
        int i, newestSave = 0;

        for( i = 0; i < NUM_BLOCKS_TOTAL; i++ )
            if( p_blocks[ i ]->footer.savenumber > newestSave )
                newestSave = p_blocks[ i ]->footer.savenumber;
        return newestSave;
    }

    u16 gen3Parser::get_block_checksum( block* p_b ) {
        int checksum = 0;
        int i;
        for( i = 0; i < BLOCK_DATA_LEN; i += 4 ) checksum += *( (int*) p_b + i / 4 );
        checksum += checksum >> 16;
        checksum &= 0xFFFF;

        return (u16) checksum;
    }

    char* gen3Parser::parse_save( block* p_blocks[ NUM_BLOCKS_TOTAL ] ) {
        char* data;
        int   i, newestSave;

        for( i = 0; i < NUM_BLOCKS_TOTAL; i++ ) {
            p_blocks[ i ] = new block( );
            smemcpy( (u8*) ( p_blocks[ i ] ), SRAM + i * ( sizeof( block ) ), sizeof( block ) );
        }

        newestSave = get_newest_save( p_blocks );

        data = (char*) malloc( SAVESLOT_LEN );
        for( i = 0; i < NUM_BLOCKS_TOTAL; i++ )
            if( p_blocks[ i ]->footer.savenumber == newestSave )
                memcpy( data + BLOCK_DATA_LEN * p_blocks[ i ]->footer.blocknum, p_blocks[ i ]->data,
                        BLOCK_DATA_LEN );
        return data;
    }

    int gen3Parser::pack_save( char* p_unpackeddata, block* p_blocks[ NUM_BLOCKS_TOTAL ],
                               char p_savefile[ SAVEFILE_LEN ] ) {
        FILE* f;
        int   i, newestSave;
        int   tempCount = 0;

        newestSave = get_newest_save( p_blocks );

        // Re-split into blocks and place over buffer
        for( i = 0; i < NUM_BLOCKS_TOTAL; i++ )
            if( p_blocks[ i ]->footer.savenumber == newestSave ) {
                memcpy( p_blocks[ i ]->data,
                        p_unpackeddata + BLOCK_DATA_LEN * p_blocks[ i ]->footer.blocknum,
                        BLOCK_DATA_LEN );
                // Re-calculate and set this block's checksum
                p_blocks[ i ]->footer.checksum = get_block_checksum( p_blocks[ i ] );
                tempCount++;
            }
        free( p_unpackeddata );

        // Save back to file
        if( ( f = fopen( (char*) SRAM, "wb" ) ) == NULL ) return -1;

        if( fwrite( p_savefile, SAVEFILE_LEN, 1, f ) != 1 ) {
            fclose( f );

            return -1;
        }
        fclose( f );

        return 0;
    }

    char* gen3Parser::get_text( u8* p_raw, bool p_isNickname ) {
        char* actual_text;
        int   len;

        if( p_isNickname ) {
            actual_text = new char[ MAX_NICKNAME_LEN ];
            len         = MAX_NICKNAME_LEN;
        } else {
            actual_text = new char[ MAX_TRAINER_NAME_LEN ];
            len         = MAX_TRAINER_NAME_LEN;
        }

        for( int i = 0; i < len; i++ ) {
            if( int( p_raw[ i ] ) != 255 )
                actual_text[ i ] = getNText( p_raw[ i ] );
            else
                actual_text[ i ] = '\0';
        }

        return actual_text;
    }

    int gen3Parser::parse_pokemon( char* p_buf, int p_offset, void** p_pokemon,
                                   gen3Pokemon::pokemon_moves_t**  p_pa,
                                   gen3Pokemon::pokemon_effort_t** p_pe,
                                   gen3Pokemon::pokemon_growth_t** p_pg,
                                   gen3Pokemon::pokemon_misc_t** p_pm, int p_num, int p_size ) {
        int i;
        if( p_size != sizeof( belt_pokemon_t ) && p_size != sizeof( box_pokemon_t ) ) return -1;

        // Parse gen3Pokemon
        for( i = 0; i < p_num; i++ ) {
            int o;

            // Read data on gen3Pokemon
            p_pokemon[ i ] = ( p_buf + p_offset + ( i * p_size ) );

            // Unencrypt gen3Pokemon's data
            // box and belt gen3Pokemon have these struct members all at
            // the same offset so we can cast to either type
            gen3Parser::Instance( )->encrypt( ( (box_pokemon_t*) p_pokemon[ i ] )->data,
                                              ( (box_pokemon_t*) p_pokemon[ i ] )->personality,
                                              ( (box_pokemon_t*) p_pokemon[ i ] )->otid );

            // Figure out the order
            o = ( (box_pokemon_t*) p_pokemon[ i ] )->personality % 24;
            p_pa[ i ]
                = (gen3Pokemon::pokemon_moves_t*) ( ( (box_pokemon_t*) p_pokemon[ i ] )->data
                                                    + DataOrderTable[ o ][ 0 ]
                                                          * sizeof(
                                                                gen3Pokemon::pokemon_moves_t ) );
            p_pe[ i ]
                = (gen3Pokemon::pokemon_effort_t*) ( ( (box_pokemon_t*) p_pokemon[ i ] )->data
                                                     + DataOrderTable[ o ][ 1 ]
                                                           * sizeof(
                                                                 gen3Pokemon::pokemon_effort_t ) );
            p_pg[ i ]
                = (gen3Pokemon::pokemon_growth_t*) ( ( (box_pokemon_t*) p_pokemon[ i ] )->data
                                                     + DataOrderTable[ o ][ 2 ]
                                                           * sizeof(
                                                                 gen3Pokemon::pokemon_growth_t ) );
            p_pm[ i ]
                = (gen3Pokemon::pokemon_misc_t*) ( ( (box_pokemon_t*) p_pokemon[ i ] )->data
                                                   + DataOrderTable[ o ][ 3 ]
                                                         * sizeof( gen3Pokemon::pokemon_misc_t ) );
        }

        return 0;
    }

    gen3Parser* gen3Parser::Instance( ) {
        if( !spInstance ) // If an instance hasn't already been created, so do
            spInstance = new gen3Parser( );

        return spInstance;
    }

    int gen3Parser::load( int p_game ) {
        // Parse save
        unpackeddata = parse_save( m_blocks );
        if( unpackeddata == NULL ) return -1;
        // Decode belt part
        parse_pokemon( unpackeddata, belt_offsets[ p_game ], (void**) pokemon, pokemon_moves,
                       pokemon_effort, pokemon_growth, pokemon_misc, NUM_BELT_POKEMON,
                       sizeof( belt_pokemon_t ) );

        return 0;
    }

    u16 gen3Parser::encrypt( u8* p_data, u32 p_pv, u32 p_otid ) {
        u32 xorkey   = p_pv ^ p_otid;
        u16 checksum = 0;
        u32 i;

        for( i = 0; i < POKEMON_DATA_LENGTH; i += 4 ) {
            checksum += p_data[ i + 1 ] << 8 | p_data[ i ];
            checksum += p_data[ i + 3 ] << 8 | p_data[ i + 2 ];

            p_data[ i ] ^= ( xorkey >> 0 ) & 0xFF;
            p_data[ i + 1 ] ^= ( xorkey >> 8 ) & 0xFF;
            p_data[ i + 2 ] ^= ( xorkey >> 16 ) & 0xFF;
            p_data[ i + 3 ] ^= ( xorkey >> 24 ) & 0xFF;
        }

        return checksum;
    }
}
