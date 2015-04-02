/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : pokemon.cpp
    author      : Philip Wellnitz (RedArceus)
    description : The main Pokémon engine

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


#include "move.h"
#include "pokemon.h"

#include "../defines.h"
#include "../fs/saveGame.h"

#include <nds.h>
#include <vector>

#include <ctime>

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
std::string TasteList[ 6 ] = {
    "scharf",
    "saur",
    "süß",
    "trocken",
    "bitter",
    "all"
};

std::string Games[ 10 ] = { };

double NatMod[ 25 ][ 5 ] = {
    { 1.0, 1.0, 1.0, 1.0, 1.0 },
    { 1.1, 0.9, 1.0, 1.0, 1.0 },
    { 1.1, 1.0, 0.9, 1.0, 1.0 },
    { 1.1, 1.0, 1.0, 0.9, 1.0 },
    { 1.1, 1.0, 1.0, 1.0, 0.9 },
    { 0.9, 1.1, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.1, 0.9, 1.0, 1.0 },
    { 1.0, 1.1, 1.0, 0.9, 1.0 },
    { 1.0, 1.1, 1.0, 1.0, 0.9 },
    { 0.9, 1.0, 1.1, 1.0, 1.0 },
    { 1.0, 0.9, 1.1, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.1, 0.9, 1.0 },
    { 1.0, 1.0, 1.1, 1.0, 0.9 },
    { 0.9, 1.0, 1.0, 1.1, 1.0 },
    { 1.0, 0.9, 1.0, 1.1, 1.0 },
    { 1.0, 1.0, 0.9, 1.1, 1.0 },
    { 1.0, 1.0, 1.0, 1.0, 1.0 },
    { 1.0, 1.0, 1.0, 1.1, 0.9 },
    { 0.9, 1.0, 1.0, 1.0, 1.1 },
    { 1.0, 0.9, 1.0, 1.0, 1.1 },
    { 1.0, 1.0, 0.9, 1.0, 1.1 },
    { 1.0, 1.0, 1.0, 0.9, 1.1 },
    { 1.0, 1.0, 1.0, 1.0, 1.0 }
};

u32 EXP[ 100 ][ 13 ] = {
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

pokemonData data;
pokemon::boxPokemon::boxPokemon( u16*           p_moves,
                                 u16            p_pkmnId,
                                 const wchar_t* p_name,
                                 u16            p_level,
                                 u16            p_id,
                                 u16            p_sid,
                                 const wchar_t* p_oT,
                                 bool           p_oTFemale,
                                 bool           p_cloned,
                                 bool           p_shiny,
                                 bool           p_hiddenAbility,
                                 bool           p_fatefulEncounter,
                                 bool           p_isEgg,
                                 u16            p_gotPlace,
                                 u8             p_ball,
                                 u8             p_pokerus ) {
    getAll( p_pkmnId, data );

    srand( LastPID );
    LastPID = m_pid = rand( );
    if( p_shiny )
        while( !isShiny( ) )
            LastPID = m_pid = rand( );
    else
        while( isShiny( ) )
            LastPID = m_pid = rand( );
    m_checksum = 0;
    m_speciesId = p_pkmnId;

    if( data.m_items[ 3 ] )
        m_holdItem = data.m_items[ 3 ];
    else {
        m_b1 = rand( ) % 100;
        if( m_b1 < 5 && data.m_items[ 0 ] )
            m_holdItem = data.m_items[ 0 ];
        else if( m_b1 < 20 && data.m_items[ 1 ] )
            m_holdItem = data.m_items[ 1 ];
        else if( m_b1 < 80 && data.m_items[ 2 ] )
            m_holdItem = data.m_items[ 2 ];
        else
            m_holdItem = 0;
    }

    m_oTId = p_id;
    m_oTSid = p_sid;
    if( !p_isEgg )
        m_experienceGained = EXP[ p_level - 1 ][ data.m_expType ];
    else
        m_experienceGained = 0;

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
    m_ability = p_hiddenAbility ? ( ( m_pid & 1 || ( data.m_abilities[ 3 ] == 0 ) ) ? data.m_abilities[ 2 ] : data.m_abilities[ 3 ] ) :
        ( ( m_pid & 1 || ( data.m_abilities[ 1 ] == 0 ) ) ? data.m_abilities[ 0 ] : data.m_abilities[ 1 ] );
    m_markings = 0;
    m_origLang = 5;
    for( int i = 0; i < 6; ++i ) m_effortValues[ i ] = 0;
    for( int i = 0; i < 6; ++i ) m_contestStats[ i ] = 0;
    for( int i = 0; i < 4; ++i ) m_ribbons1[ i ] = 0;

    if( p_moves )
        for( int i = 0; i < 4; ++i ) m_moves[ i ] = p_moves[ i ];
    else
        getLearnMoves( p_pkmnId, p_level, 0, 1, 4, m_moves );
    for( int i = 0; i < 4; ++i ) m_acPP[ i ] = (u8)( AttackList[ m_moves[ i ] ]->m_movePP );

    m_ppup.m_Up1 = 0;
    m_ppup.m_Up2 = 0;
    m_ppup.m_Up3 = 0;
    m_ppup.m_Up4 = 0;
    m_individualValues.m_attack = rand( ) % 32;
    m_individualValues.m_defense = rand( ) % 32;
    m_individualValues.m_hp = rand( ) % 32;
    m_individualValues.m_sAttack = rand( ) % 32;
    m_individualValues.m_sDefense = rand( ) % 32;
    m_individualValues.m_speed = rand( ) % 32;
    m_individualValues.m_isNicked = false;
    m_individualValues.m_isEgg = p_isEgg;
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
    else if( ( m_pid % 256 ) >= A )
        m_isFemale = m_isGenderless = false;
    else m_isFemale = true,
        m_isGenderless = false;

    m_altForme = 0;
    m_cloned = p_cloned;
    if( p_name ) {
        wcscpy( m_name, p_name );
        m_individualValues.m_isNicked = true;
    } else {
        getWDisplayName( p_pkmnId, m_name );
        m_individualValues.m_isNicked = false;
    }
    m_hometown = 4;
    for( int i = 0; i < 4; ++i ) m_ribbons2[ i ] = 0;
    wcscpy( m_oT, p_oT );
    m_pokerus = p_pokerus;
    m_ball = p_ball;
    m_gotLevel = p_level;
    m_oTisFemale = p_oTFemale;
    m_encounter = (encounter)0;
    m_HGSSBall = 0;

}
pokemon::pokemon( u16*              p_moves,
                  u16               p_pkmnId,
                  const wchar_t*    p_name,
                  u16               p_level,
                  u16               p_id,
                  u16               p_sid,
                  const wchar_t*    p_oT,
                  bool              p_oTFemale,
                  bool              p_cloned,
                  bool              p_shiny,
                  bool              p_hiddenAbility,
                  bool              p_fatefulEncounter,
                  bool              p_isEgg,
                  u16               p_gotPlace,
                  u8                p_ball,
                  u8                p_pokerus )
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
    getAll( p_pkmnId, data );
    if( p_pkmnId != 292 )
        m_stats.m_acHP = m_stats.m_maxHP = ( ( m_boxdata.m_individualValues.m_hp + 2 * data.m_bases[ 0 ] + ( m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )*p_level / 100 ) + 10;
    else
        m_stats.m_acHP = m_stats.m_maxHP = 1;

    pkmnNatures nature = m_boxdata.getNature( );
    m_stats.m_Atk = ( ( ( m_boxdata.m_individualValues.m_attack + 2 * data.m_bases[ 1 ] + ( m_boxdata.m_effortValues[ 1 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 0 ];
    m_stats.m_Def = ( ( ( m_boxdata.m_individualValues.m_defense + 2 * data.m_bases[ 2 ] + ( m_boxdata.m_effortValues[ 2 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 1 ];
    m_stats.m_Spd = ( ( ( m_boxdata.m_individualValues.m_speed + 2 * data.m_bases[ 5 ] + ( m_boxdata.m_effortValues[ 5 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 2 ];
    m_stats.m_SAtk = ( ( ( m_boxdata.m_individualValues.m_sAttack + 2 * data.m_bases[ 3 ] + ( m_boxdata.m_effortValues[ 3 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 3 ];
    m_stats.m_SDef = ( ( ( m_boxdata.m_individualValues.m_sDefense + 2 * data.m_bases[ 4 ] + ( m_boxdata.m_effortValues[ 4 ] >> 2 ) )*p_level / 100.0 ) + 5 )*NatMod[ nature ][ 4 ];

    m_status.m_Asleep = m_status.m_Burned = m_status.m_Frozen = m_status.m_Paralyzed = m_status.m_Poisoned = m_status.m_Toxic = false;
}

bool pokemon::boxPokemon::isShiny( ) {
    return ( ( ( m_oTId ^ m_oTSid ) >> 3 ) ^ ( ( ( m_pid >> 16 ) ^ ( m_pid % ( 1 << 16 ) ) ) ) >> 3 ) == 0;
}
bool pokemon::boxPokemon::isCloned( ) {
    return ( ( m_pid >> 16 )&( m_pid % ( 1 << 16 ) ) ) < ( ( m_pid >> 16 ) ^ ( m_pid % ( 1 << 16 ) ) );
}
s8 pokemon::boxPokemon::gender( ) {
    if( m_isGenderless )
        return 0;
    else if( m_isFemale )
        return -1;
    return 1;
}

u32 LastPID = 42;

bool pokemon::canEvolve( u16 p_item, u16 p_method ) {
    if( m_boxdata.m_individualValues.m_isEgg )
        return false;

    getAll( m_boxdata.m_speciesId, data );

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
        if( data.m_evolutions[ i ].m_e.m_evolveLocation && FS::SAV->m_acMapIdx != data.m_evolutions[ i ].m_e.m_evolveLocation )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveHeldItem && m_boxdata.m_holdItem != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_moves[ j ] );
            if( !b )
                continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType == AttackList[ m_boxdata.m_moves[ j ] ]->m_moveType );
            if( !b )
                continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_contestStats[ 1 ] )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
            bool b = false;
            for( int j = 0; j < 6; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember == FS::SAV->m_PkmnTeam[ i ].m_boxdata.m_speciesId );
            if( !b )
                continue;
        }
        if( p_method != data.m_evolutions[ i ].m_e.m_evolutionTrigger )
            continue;
        return true;
    }
    return false;
}

void pokemon::evolve( u16 p_item, u16 p_method ) {
    if( m_boxdata.m_individualValues.m_isEgg )
        return;

    getAll( m_boxdata.m_speciesId, data );

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
        if( data.m_evolutions[ i ].m_e.m_evolveLocation && FS::SAV->m_acMapIdx != data.m_evolutions[ i ].m_e.m_evolveLocation )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveHeldItem && m_boxdata.m_holdItem != data.m_evolutions[ i ].m_e.m_evolveHeldItem )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMove ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMove == m_boxdata.m_moves[ j ] );
            if( !b )
                continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType ) {
            bool b = false;
            for( int j = 0; j < 4; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveKnownMoveType == AttackList[ m_boxdata.m_moves[ j ] ]->m_moveType );
            if( !b )
                continue;
        }
        if( data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty && data.m_evolutions[ i ].m_e.m_evolveMinimumBeauty < m_boxdata.m_contestStats[ 1 ] )
            continue;
        if( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember ) {
            bool b = false;
            for( int j = 0; j < 6; ++j )
                b |= ( data.m_evolutions[ i ].m_e.m_evolveAdditionalPartyMember == FS::SAV->m_PkmnTeam[ i ].m_boxdata.m_speciesId );
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

    int HPdif = m_stats.m_maxHP - m_stats.m_acHP;
    m_boxdata.m_speciesId = into;
    getAll( m_boxdata.m_speciesId, data );
    if( m_boxdata.m_speciesId != 292 )
        m_stats.m_maxHP = ( ( m_boxdata.m_individualValues.m_hp + 2 * data.m_bases[ 0 ] + ( m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )*m_Level / 100 ) + 10;
    else
        m_stats.m_maxHP = 1;

    if( !m_boxdata.m_individualValues.m_isNicked )
        wcscpy( m_boxdata.m_name, getWDisplayName( m_boxdata.m_speciesId ) );

    pkmnNatures nature = m_boxdata.getNature( );
    m_stats.m_Atk = ( ( ( m_boxdata.m_individualValues.m_attack + 2 * data.m_bases[ 1 ] + ( m_boxdata.m_effortValues[ 1 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 0 ];
    m_stats.m_Def = ( ( ( m_boxdata.m_individualValues.m_defense + 2 * data.m_bases[ 2 ] + ( m_boxdata.m_effortValues[ 2 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 1 ];
    m_stats.m_Spd = ( ( ( m_boxdata.m_individualValues.m_speed + 2 * data.m_bases[ 3 ] + ( m_boxdata.m_effortValues[ 3 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 2 ];
    m_stats.m_SAtk = ( ( ( m_boxdata.m_individualValues.m_sAttack + 2 * data.m_bases[ 4 ] + ( m_boxdata.m_effortValues[ 4 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 3 ];
    m_stats.m_SDef = ( ( ( m_boxdata.m_individualValues.m_sDefense + 2 * data.m_bases[ 5 ] + ( m_boxdata.m_effortValues[ 5 ] >> 2 ) )*m_Level / 100.0 ) + 5 )*NatMod[ nature ][ 4 ];

    m_stats.m_acHP = m_stats.m_maxHP - HPdif;
}

//TODO: enhance equality test
bool pokemon::operator==( const pokemon& p_other ) const {
    if( m_boxdata.m_pid != p_other.m_boxdata.m_pid
        || m_boxdata.m_b1 != p_other.m_boxdata.m_b1
        || m_boxdata.m_checksum != p_other.m_boxdata.m_checksum
        || m_boxdata.m_speciesId != p_other.m_boxdata.m_speciesId
        || m_boxdata.m_holdItem != p_other.m_boxdata.m_holdItem
        || m_boxdata.m_oTId != p_other.m_boxdata.m_oTId
        || m_boxdata.m_oTSid != p_other.m_boxdata.m_oTSid
        || m_boxdata.m_experienceGained != p_other.m_boxdata.m_experienceGained
        || m_boxdata.m_steps != p_other.m_boxdata.m_steps
        || m_boxdata.m_ability != p_other.m_boxdata.m_ability
        || m_boxdata.m_markings != p_other.m_boxdata.m_markings
        || m_boxdata.m_origLang != p_other.m_boxdata.m_origLang )
        return false;
    return true;
}
