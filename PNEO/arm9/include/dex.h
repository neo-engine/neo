/*
Pokémon neo
------------------------------

file        : dex.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2021
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
#include "dexUI.h"
#include "pokemonNames.h"

namespace DEX {
    u16 nextEntry( u16 p_current );
    u16 previousEntry( u16 p_current );

    constexpr u16 LOCAL_DEX_SIZE = 250;
    constexpr u8  MAX_PAGES      = 3;

    constexpr u16 MAX_LOCALDEX_PAGES = 250;
    constexpr u16 EMPTY              = 0;
    constexpr u16 ALOLAN_FORME       = 10000;
    constexpr u16 GALARIAN_FORME     = 20000;

    constexpr u16 LOCAL_DEX[ LOCAL_DEX_SIZE ] = {
        PKMN_TREECKO,
        PKMN_GROVYLE,
        PKMN_SCEPTILE,
        PKMN_TORCHIC,

        PKMN_COMBUSKEN,
        PKMN_BLAZIKEN,
        PKMN_MUDKIP,
        PKMN_MARSHTOMP,
        PKMN_SWAMPERT,

        // 10

        PKMN_POOCHYENA,
        PKMN_MIGHTYENA,
        PKMN_ZIGZAGOON,
        PKMN_LINOONE,
        PKMN_OBSTAGOON,

        PKMN_WURMPLE,
        PKMN_SILCOON,
        PKMN_BEAUTIFLY,
        PKMN_CASCOON,
        PKMN_DUSTOX,

        // 20

        PKMN_LOTAD,
        PKMN_LOMBRE,
        PKMN_LUDICOLO,
        PKMN_SEEDOT,
        PKMN_NUZLEAF,

        PKMN_SHIFTRY,
        PKMN_TAILLOW,
        PKMN_SWELLOW,
        PKMN_WINGULL,
        PKMN_PELIPPER,

        // 30

        PKMN_RALTS,
        PKMN_KIRLIA,
        PKMN_GARDEVOIR,
        PKMN_GALLADE,
        PKMN_GOTHITA,

        PKMN_GOTHORITA,
        PKMN_GOTHITELLE,
        PKMN_SOLOSIS,
        PKMN_DUOSION,
        PKMN_REUNICLUS,

        // 40

        PKMN_SURSKIT,
        PKMN_MASQUERAIN,
        PKMN_SHROOMISH,
        PKMN_BRELOOM,
        PKMN_SLAKOTH,

        PKMN_VIGOROTH,
        PKMN_SLAKING,
        PKMN_ABRA,
        PKMN_KADABRA,
        PKMN_ALAKAZAM,

        // 50

        PKMN_NINCADA,
        PKMN_NINJASK,
        PKMN_SHEDINJA,
        PKMN_WHISMUR,
        PKMN_LOUDRED,

        PKMN_EXPLOUD,
        PKMN_MAKUHITA,
        PKMN_HARIYAMA,
        PKMN_GOLDEEN,
        PKMN_SEAKING,

        // 60

        PKMN_MAGIKARP,
        PKMN_GYARADOS,
        PKMN_AZURILL,
        PKMN_MARILL,
        PKMN_AZUMARILL,

        PKMN_GEODUDE,
        PKMN_GRAVELER,
        PKMN_GOLEM,
        PKMN_NOSEPASS,
        PKMN_PROBOPASS,

        // 70

        PKMN_SKITTY,
        PKMN_DELCATTY,
        PKMN_FOONGUS,
        PKMN_AMOONGUSS,
        PKMN_ZUBAT,

        PKMN_GOLBAT,
        PKMN_CROBAT,
        PKMN_TENTACOOL,
        PKMN_TENTACRUEL,
        PKMN_SABLEYE,

        // 80

        PKMN_MAWILE,
        PKMN_ARON,
        PKMN_LAIRON,
        PKMN_AGGRON,
        PKMN_MACHOP,

        PKMN_MACHOKE,
        PKMN_MACHAMP,
        PKMN_MEDITITE,
        PKMN_MEDICHAM,
        PKMN_ELECTRIKE,

        // 90

        PKMN_MANECTRIC,
        PKMN_PLUSLE,
        PKMN_MINUN,
        PKMN_MAGNEMITE,
        PKMN_MAGNETON,

        PKMN_MAGNEZONE,
        PKMN_VOLTORB,
        PKMN_ELECTRODE,
        PKMN_VOLBEAT,
        PKMN_ILLUMISE,

        // 100

        PKMN_ODDISH,
        PKMN_GLOOM,
        PKMN_VILEPLUME,
        PKMN_BELLOSSOM,
        PKMN_DODUO,

        PKMN_DODRIO,
        PKMN_GLAMEOW,
        PKMN_PURUGLY,
        PKMN_BUDEW,
        PKMN_ROSELIA,

        // 110

        PKMN_ROSERADE,
        PKMN_GULPIN,
        PKMN_SWALOT,
        PKMN_CARVANHA,
        PKMN_SHARPEDO,

        PKMN_FINNEON,
        PKMN_LUMINEON,
        PKMN_WAILMER,
        PKMN_WAILORD,
        PKMN_SHELLOS,

        // 120

        PKMN_GASTRODON,
        PKMN_NUMEL,
        PKMN_CAMERUPT,
        PKMN_SLUGMA,
        PKMN_MAGCARGO,

        PKMN_TORKOAL,
        PKMN_GRIMER,
        PKMN_MUK,
        PKMN_KOFFING,
        PKMN_WEEZING,

        // 130

        PKMN_SPOINK,
        PKMN_GRUMPIG,
        PKMN_SANDSHREW,
        PKMN_SANDSLASH,
        PKMN_SPINDA,

        PKMN_SKARMORY,
        PKMN_TRAPINCH,
        PKMN_VIBRAVA,
        PKMN_FLYGON,
        PKMN_CACNEA,

        // 140

        PKMN_CACTURNE,
        PKMN_SWABLU,
        PKMN_ALTARIA,
        PKMN_ZANGOOSE,
        PKMN_SEVIPER,

        PKMN_LUNATONE,
        PKMN_SOLROCK,
        PKMN_BARBOACH,
        PKMN_WHISCASH,
        PKMN_CORPHISH,

        // 150

        PKMN_CRAWDAUNT,
        PKMN_BALTOY,
        PKMN_CLAYDOL,
        PKMN_LILEEP,
        PKMN_CRADILY,

        PKMN_ANORITH,
        PKMN_ARMALDO,
        PKMN_IGGLYBUFF,
        PKMN_JIGGLYPUFF,
        PKMN_WIGGLYTUFF,

        // 160

        PKMN_FEEBAS,
        PKMN_MILOTIC,
        PKMN_CASTFORM,
        PKMN_STARYU,
        PKMN_STARMIE,

        PKMN_KECLEON,
        PKMN_SHUPPET,
        PKMN_BANETTE,
        PKMN_DUSKULL,
        PKMN_DUSCLOPS,

        // 170

        PKMN_DUSKNOIR,
        PKMN_LITWICK,
        PKMN_LAMPENT,
        PKMN_CHANDELURE,
        PKMN_GASTLY,

        PKMN_HAUNTER,
        PKMN_GENGAR,
        PKMN_MISDREAVUS,
        PKMN_MISMAGIUS,
        PKMN_MURKROW,

        // 180

        PKMN_HONCHKROW,
        PKMN_TROPIUS,
        PKMN_CHINGLING,
        PKMN_CHIMECHO,
        PKMN_ABSOL,

        PKMN_VULPIX,
        PKMN_NINETALES,
        PKMN_PICHU,
        PKMN_PIKACHU,
        PKMN_RAICHU,

        // 190

        PKMN_PSYDUCK,
        PKMN_GOLDUCK,
        PKMN_WYNAUT,
        PKMN_WOBBUFFET,
        PKMN_HOPPIP,

        PKMN_SKIPLOOM,
        PKMN_JUMPLUFF,
        PKMN_NATU,
        PKMN_XATU,
        PKMN_GIRAFARIG,

        // 200

        PKMN_PHANPY,
        PKMN_DONPHAN,
        PKMN_PINSIR,
        PKMN_HERACROSS,
        PKMN_RHYHORN,

        PKMN_RHYDON,
        PKMN_RHYPERIOR,
        PKMN_MUNNA,
        PKMN_MUSHARNA,
        PKMN_SNORUNT,

        // 210

        PKMN_GLALIE,
        PKMN_FROSLASS,
        PKMN_SPHEAL,
        PKMN_SEALEO,
        PKMN_WALREIN,

        PKMN_CLAMPERL,
        PKMN_HUNTAIL,
        PKMN_GOREBYSS,
        PKMN_RELICANTH,
        PKMN_CORSOLA,

        // 220

        PKMN_CURSOLA,
        PKMN_MANTYKE,
        PKMN_MANTINE,
        PKMN_CHINCHOU,
        PKMN_LANTURN,

        PKMN_LUVDISC,
        PKMN_HORSEA,
        PKMN_SEADRA,
        PKMN_KINGDRA,
        PKMN_ZORUA,

        // 230

        PKMN_ZOROARK,
        PKMN_BAGON,
        PKMN_SHELGON,
        PKMN_SALAMENCE,
        PKMN_BELDUM,

        PKMN_METANG,
        PKMN_METAGROSS,
        PKMN_ROTOM,
        PKMN_REGIROCK,
        PKMN_REGICE,

        // 240

        PKMN_REGISTEEL,
        PKMN_REGIELEKI,
        PKMN_REGIDRAGO,
        PKMN_REGIGIGAS,
        PKMN_LATIAS,

        PKMN_LATIOS,
        PKMN_KYOGRE,
        PKMN_GROUDON,
        PKMN_RAYQUAZA,
        PKMN_JIRACHI,

        // 250

        PKMN_DEOXYS,

    };

    constexpr u16 LOCAL_DEX_PAGES[ MAX_LOCALDEX_PAGES ][ 3 ] = {
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_TREECKO, PKMN_GROVYLE, PKMN_SCEPTILE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_TORCHIC, PKMN_COMBUSKEN, PKMN_BLAZIKEN },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MUDKIP, PKMN_MARSHTOMP, PKMN_SWAMPERT },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_POOCHYENA, PKMN_MIGHTYENA },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ZIGZAGOON, PKMN_LINOONE },
        { GALARIAN_FORME + PKMN_ZIGZAGOON, GALARIAN_FORME + PKMN_LINOONE, PKMN_OBSTAGOON },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_WURMPLE, PKMN_SILCOON, PKMN_BEAUTIFLY },
        { EMPTY, PKMN_CASCOON, PKMN_DUSTOX },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_LOTAD, PKMN_LOMBRE, PKMN_LUDICOLO },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SEEDOT, PKMN_NUZLEAF, PKMN_SHIFTRY },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_TAILLOW, PKMN_SWELLOW },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_WINGULL, PKMN_PELIPPER },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_RALTS, PKMN_KIRLIA, PKMN_GARDEVOIR },
        { EMPTY, EMPTY, PKMN_GALLADE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GOTHITA, PKMN_GOTHORITA, PKMN_GOTHITELLE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SOLOSIS, PKMN_DUOSION, PKMN_REUNICLUS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SURSKIT, PKMN_MASQUERAIN },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SHROOMISH, PKMN_BRELOOM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SLAKOTH, PKMN_VIGOROTH, PKMN_SLAKING },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ABRA, PKMN_KADABRA, PKMN_ALAKAZAM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_NINCADA, PKMN_NINJASK },
        { EMPTY, PKMN_SHEDINJA },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_WHISMUR, PKMN_LOUDRED, PKMN_EXPLOUD },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MAKUHITA, PKMN_HARIYAMA },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GOLDEEN, PKMN_SEAKING },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MAGIKARP, PKMN_GYARADOS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_AZURILL, PKMN_MARILL, PKMN_AZUMARILL },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GEODUDE, PKMN_GRAVELER, PKMN_GOLEM },
        { ALOLAN_FORME + PKMN_GEODUDE, ALOLAN_FORME + PKMN_GRAVELER, ALOLAN_FORME + PKMN_GOLEM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_NOSEPASS, PKMN_PROBOPASS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SKITTY, PKMN_DELCATTY },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_FOONGUS, PKMN_AMOONGUSS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ZUBAT, PKMN_GOLBAT, PKMN_CROBAT },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_TENTACOOL, PKMN_TENTACRUEL },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_SABLEYE },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_MAWILE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ARON, PKMN_LAIRON, PKMN_AGGRON },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MACHOP, PKMN_MACHOKE, PKMN_MACHAMP },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MEDITITE, PKMN_MEDICHAM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ELECTRIKE, PKMN_MANECTRIC },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_PLUSLE },
        { EMPTY, PKMN_MINUN },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MAGNEMITE, PKMN_MAGNETON, PKMN_MAGNEZONE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_VOLTORB, PKMN_ELECTRODE },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_VOLBEAT },
        { EMPTY, PKMN_ILLUMISE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ODDISH, PKMN_GLOOM, PKMN_VILEPLUME },
        { EMPTY, EMPTY, PKMN_BELLOSSOM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_DODUO, PKMN_DODRIO },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GLAMEOW, PKMN_PURUGLY },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_BUDEW, PKMN_ROSELIA, PKMN_ROSERADE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GULPIN, PKMN_SWALOT },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CARVANHA, PKMN_SHARPEDO },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_FINNEON, PKMN_LUMINEON },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_WAILMER, PKMN_WAILORD },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SHELLOS, PKMN_GASTRODON },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_NUMEL, PKMN_CAMERUPT },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SLUGMA, PKMN_MAGCARGO },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_TORKOAL },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GRIMER, PKMN_MUK },
        { ALOLAN_FORME + PKMN_GRIMER, ALOLAN_FORME + PKMN_MUK },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_KOFFING, PKMN_WEEZING },
        { EMPTY, GALARIAN_FORME + PKMN_WEEZING },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SPOINK, PKMN_GRUMPIG },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SANDSHREW, PKMN_SANDSLASH },
        { ALOLAN_FORME + PKMN_SANDSHREW, ALOLAN_FORME + PKMN_SANDSLASH },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_SPINDA },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_SKARMORY },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_TRAPINCH, PKMN_VIBRAVA, PKMN_FLYGON },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CACNEA, PKMN_CACTURNE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SWABLU, PKMN_ALTARIA },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_ZANGOOSE },
        { EMPTY, PKMN_SEVIPER },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_LUNATONE },
        { EMPTY, PKMN_SOLROCK },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_BARBOACH, PKMN_WHISCASH },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CORPHISH, PKMN_CRAWDAUNT },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_BALTOY, PKMN_CLAYDOL },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_LILEEP, PKMN_CRADILY },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ANORITH, PKMN_ARMALDO },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_IGGLYBUFF, PKMN_JIGGLYPUFF, PKMN_WIGGLYTUFF },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_FEEBAS, PKMN_MILOTIC },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_CASTFORM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_STARYU, PKMN_STARMIE },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_KECLEON },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SHUPPET, PKMN_BANETTE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_DUSKULL, PKMN_DUSCLOPS, PKMN_DUSKNOIR },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_LITWICK, PKMN_LAMPENT, PKMN_CHANDELURE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_GASTLY, PKMN_HAUNTER, PKMN_GENGAR },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MISDREAVUS, PKMN_MISMAGIUS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MURKROW, PKMN_HONCHKROW },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_TROPIUS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CHINGLING, PKMN_CHIMECHO },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_ABSOL },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_VULPIX, PKMN_NINETALES },
        { ALOLAN_FORME + PKMN_VULPIX, ALOLAN_FORME + PKMN_NINETALES },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_PICHU, PKMN_PIKACHU, PKMN_RAICHU },
        { EMPTY, EMPTY, ALOLAN_FORME + PKMN_RAICHU },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_PSYDUCK, PKMN_GOLDUCK },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_WYNAUT, PKMN_WOBBUFFET },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_HOPPIP, PKMN_SKIPLOOM, PKMN_JUMPLUFF },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_NATU, PKMN_XATU },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_GIRAFARIG },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_PHANPY, PKMN_DONPHAN },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_PINSIR },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_HERACROSS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_RHYHORN, PKMN_RHYDON, PKMN_RHYPERIOR },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MUNNA, PKMN_MUSHARNA },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SNORUNT, PKMN_GLALIE },
        { EMPTY, PKMN_FROSLASS },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_SPHEAL, PKMN_SEALEO, PKMN_WALREIN },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CLAMPERL, PKMN_HUNTAIL },
        { EMPTY, PKMN_GOREBYSS },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_RELICANTH },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CORSOLA },
        { GALARIAN_FORME + PKMN_CORSOLA, PKMN_CURSOLA },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_MANTYKE, PKMN_MANTINE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_CHINCHOU, PKMN_LANTURN },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_LUVDISC },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_HORSEA, PKMN_SEADRA, PKMN_KINGDRA },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_ZORUA, PKMN_ZOROARK },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_BAGON, PKMN_SHELGON, PKMN_SALAMENCE },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_BELDUM, PKMN_METANG, PKMN_METAGROSS },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_ROTOM },
        { EMPTY, EMPTY, EMPTY },
        { PKMN_REGIROCK, PKMN_REGICE, PKMN_REGISTEEL },
        { PKMN_REGIELEKI, PKMN_REGIDRAGO },
        { EMPTY, PKMN_REGIGIGAS },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_LATIAS },
        { EMPTY, PKMN_LATIOS },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_KYOGRE },
        { EMPTY, PKMN_GROUDON },
        { EMPTY, PKMN_RAYQUAZA },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_JIRACHI },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, PKMN_DEOXYS },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, EMPTY, EMPTY },
        { EMPTY, EMPTY, EMPTY },
    };

    /*
     * @brief: Returns the number of the specified pkmn in the local pokédex. Returns
     * u16(-1) if the specified pkmn is not found in the local pokédex
     */
    constexpr u16 getDexNo( u16 p_pkmnIdx ) {
        for( u16 i = 0; i < LOCAL_DEX_SIZE; ++i ) {
            if( LOCAL_DEX[ i ] == p_pkmnIdx ) { return i + 1; }
        }
        return u16( -1 );
    }

    constexpr bool requiredForCompletion( u16 p_pkmnIdx ) {
        switch( p_pkmnIdx ) {
        case PKMN_MEW:
        case PKMN_CELEBI:
        case PKMN_JIRACHI:
        case PKMN_DEOXYS:
        case PKMN_PHIONE:
        case PKMN_MANAPHY:
        case PKMN_SHAYMIN:
        case PKMN_DARKRAI:
        case PKMN_ARCEUS:
        case PKMN_VICTINI:
        case PKMN_KELDEO:
        case PKMN_MELOETTA:
        case PKMN_GENESECT:
        case PKMN_HOOPA:
        case PKMN_DIANCIE:
        case PKMN_VOLCANION:
        case PKMN_MAGEARNA:
        case PKMN_MARSHADOW:
        case PKMN_MELTAN:
        case PKMN_MELMETAL:
        case PKMN_ZERAORA:
        case PKMN_ZARUDE: return false;
        }
        return true;
    }

    class dex {
      public:
        enum mode { LOCAL_DEX = 0, NATIONAL_DEX = 1, SHOW_SINGLE = 2 };

      private:
        dexUI* _dexUI;
        mode   _mode                     = LOCAL_DEX;
        u16    _currentNationalSelection = 0;
        u16    _currentLocalPage         = 0;
        u16    _currentLocalSlot         = 0;
        u8     _currentPage              = 0;
        u8     _currentForme             = 0; // only available for the national dex mode

        /*
         * @brief: Swaps between the local/national dex modes.
         * @param p_startIdx: pkmn to highlight in the new mode.
         */
        void changeMode( mode p_newMode, u16 p_startIdx );

        /*
         * @brief: Shows the dex entry of the specified pkmn.
         */
        void showEntry( u16 p_pkmnIdx, u8 p_forme = 0, bool p_shiny = false,
                        bool p_female = false );
        /*
         * @brief: Select an entry while in the national dex mode.
         */
        void selectNational( u16 p_pkmnIdx );

        /*
         * @brief: Select an entry while in the local dex mode.
         */
        void selectLocal( u16 p_page, u8 p_slot );

        /*
         * @brief: Run dex welcome screen which shows the progress and an initial dex mode
         * selection (local/national), if the national dex is already unlocked.
         * @returns: true if the player wants to exit the dex.
         */
        bool runModeChoice( );

        /*
         * @brief: Runs the main logic of the dex, handles mode switching and selecting
         * different pkmn/pages.
         */
        void runDex( );

      public:
        dex( );
        ~dex( ) {
            if( _dexUI ) { delete _dexUI; }
        }

        /*
         * @brief: Runs the dex.
         */
        void run( );

        /*
         * @brief: Shows a single dex entry of the specified pkmn, no navigation, formes
         * or habitats. Displays the national dex number iff the player has unlocked the
         * national dex (local dex no otherwise).
         */
        void run( u16 p_pkmnIdx, u8 p_forme = 0, bool p_shiny = false, bool p_female = false );
    };
} // namespace DEX
