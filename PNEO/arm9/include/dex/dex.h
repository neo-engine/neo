/*
Pokémon neo
------------------------------

file        : dex.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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

#include <nds.h>
#include "dex/dexUI.h"
#include "gen/pokemonNames.h"

namespace DEX {
    u16 nextEntry( u16 p_current );
    u16 previousEntry( u16 p_current );

    constexpr u16 LOCAL_DEX_SIZE = 277;
    constexpr u8  MAX_PAGES      = 3;

    constexpr u16 MAX_LOCAL_DEX_PAGES = 137;
    constexpr u8  MAX_LOCAL_DEX_SLOTS = 3;
    constexpr u16 EMPTY               = 0;

    constexpr u16 LOCAL_DEX[ LOCAL_DEX_SIZE ] = {
        // Starter
        PKMN_TREECKO, PKMN_GROVYLE, PKMN_SCEPTILE, PKMN_TORCHIC, PKMN_COMBUSKEN, PKMN_BLAZIKEN,
        PKMN_MUDKIP, PKMN_MARSHTOMP, PKMN_SWAMPERT,

        // fields
        PKMN_POOCHYENA, PKMN_MIGHTYENA, PKMN_ZIGZAGOON, PKMN_LINOONE, PKMN_OBSTAGOON, PKMN_SKITTY,
        PKMN_DELCATTY, PKMN_GLAMEOW, PKMN_PURUGLY, PKMN_ZORUA, PKMN_ZOROARK, PKMN_WHISMUR,
        PKMN_LOUDRED, PKMN_EXPLOUD, PKMN_ZANGOOSE, PKMN_SEVIPER,

        // Bugs
        PKMN_WURMPLE, PKMN_SILCOON, PKMN_BEAUTIFLY, PKMN_CASCOON, PKMN_DUSTOX, PKMN_SURSKIT,
        PKMN_MASQUERAIN, PKMN_NINCADA, PKMN_NINJASK, PKMN_SHEDINJA, PKMN_VOLBEAT, PKMN_ILLUMISE,
        PKMN_PINSIR, PKMN_HERACROSS, PKMN_LARVESTA, PKMN_VOLCARONA,

        // Birds (n bats)
        PKMN_TAILLOW, PKMN_SWELLOW, PKMN_WINGULL, PKMN_PELIPPER, PKMN_ZUBAT, PKMN_GOLBAT,
        PKMN_CROBAT, PKMN_SKARMORY, PKMN_SWABLU, PKMN_ALTARIA, PKMN_DODUO, PKMN_DODRIO,

        // Espers
        PKMN_RALTS, PKMN_KIRLIA, PKMN_GARDEVOIR, PKMN_GALLADE, PKMN_ABRA, PKMN_KADABRA,
        PKMN_ALAKAZAM, PKMN_SPOINK, PKMN_GRUMPIG, PKMN_GOTHITA, PKMN_GOTHORITA, PKMN_GOTHITELLE,
        PKMN_SOLOSIS, PKMN_DUOSION, PKMN_REUNICLUS, PKMN_WYNAUT, PKMN_WOBBUFFET, PKMN_GIRAFARIG,
        PKMN_MUNNA, PKMN_MUSHARNA,

        // Plants and stuff from the forest
        PKMN_LOTAD, PKMN_LOMBRE, PKMN_LUDICOLO, PKMN_SEEDOT, PKMN_NUZLEAF, PKMN_SHIFTRY,
        PKMN_SLAKOTH, PKMN_VIGOROTH, PKMN_SLAKING, PKMN_SHROOMISH, PKMN_BRELOOM, PKMN_FOONGUS,
        PKMN_AMOONGUSS, PKMN_IGGLYBUFF, PKMN_JIGGLYPUFF, PKMN_WIGGLYTUFF, PKMN_BUDEW, PKMN_ROSELIA,
        PKMN_ROSERADE, PKMN_ODDISH, PKMN_GLOOM, PKMN_VILEPLUME, PKMN_BELLOSSOM, PKMN_CACNEA,
        PKMN_CACTURNE, PKMN_MARACTUS, PKMN_TROPIUS, PKMN_HOPPIP, PKMN_SKIPLOOM, PKMN_JUMPLUFF,

        // rivers
        PKMN_GOLDEEN, PKMN_SEAKING, PKMN_MAGIKARP, PKMN_GYARADOS, PKMN_FEEBAS, PKMN_MILOTIC,
        PKMN_BARBOACH, PKMN_WHISCASH, PKMN_AZURILL, PKMN_MARILL, PKMN_AZUMARILL, PKMN_PSYDUCK,
        PKMN_GOLDUCK,

        // Rocks n caves
        PKMN_GEODUDE, PKMN_GRAVELER, PKMN_GOLEM, PKMN_NOSEPASS, PKMN_PROBOPASS, PKMN_ARON,
        PKMN_LAIRON, PKMN_AGGRON, PKMN_SABLEYE, PKMN_MAWILE, PKMN_LUNATONE, PKMN_SOLROCK,
        PKMN_RHYHORN, PKMN_RHYDON, PKMN_RHYPERIOR,

        // Things destroying rocks
        PKMN_MAKUHITA, PKMN_HARIYAMA, PKMN_MACHOP, PKMN_MACHOKE, PKMN_MACHAMP, PKMN_MEDITITE,
        PKMN_MEDICHAM, PKMN_CRABRAWLER, PKMN_CRABOMINABLE,

        // ocean
        PKMN_TENTACOOL, PKMN_TENTACRUEL, PKMN_CARVANHA, PKMN_SHARPEDO, PKMN_HORSEA, PKMN_SEADRA,
        PKMN_KINGDRA,

        // bzzzt
        PKMN_ELECTRIKE, PKMN_MANECTRIC, PKMN_PLUSLE, PKMN_MINUN, PKMN_MAGNEMITE, PKMN_MAGNETON,
        PKMN_MAGNEZONE, PKMN_VOLTORB, PKMN_ELECTRODE, PKMN_ROTOM, PKMN_PICHU, PKMN_PIKACHU,
        PKMN_RAICHU, PKMN_SPINDA,

        // burning stuff
        PKMN_NUMEL, PKMN_CAMERUPT, PKMN_SLUGMA, PKMN_MAGCARGO, PKMN_TORKOAL, PKMN_VULPIX,
        PKMN_NINETALES, PKMN_TURTONATOR,

        // dirty stuff
        PKMN_GULPIN, PKMN_SWALOT, PKMN_GRIMER, PKMN_MUK, PKMN_KOFFING, PKMN_WEEZING, PKMN_TRUBBISH,
        PKMN_GARBODOR, PKMN_KECLEON,

        // beach
        PKMN_CORPHISH, PKMN_CRAWDAUNT, PKMN_SANDYGAST, PKMN_PALOSSAND, PKMN_STARYU, PKMN_STARMIE,
        PKMN_CLOBBOPUS, PKMN_GRAPPLOCT,

        // hail and sand
        PKMN_SPHEAL, PKMN_SEALEO, PKMN_WALREIN, PKMN_SNORUNT, PKMN_GLALIE, PKMN_FROSLASS, PKMN_SNOM,
        PKMN_FROSMOTH, PKMN_SNEASEL, PKMN_WEAVILE, PKMN_GLIGAR, PKMN_GLISCOR, PKMN_SANDSHREW,
        PKMN_SANDSLASH, PKMN_BALTOY, PKMN_CLAYDOL, PKMN_TRAPINCH, PKMN_VIBRAVA, PKMN_FLYGON,

        // spooky
        PKMN_SHUPPET, PKMN_BANETTE, PKMN_MIMIKYU, PKMN_DUSKULL, PKMN_DUSCLOPS, PKMN_DUSKNOIR,
        PKMN_LITWICK, PKMN_LAMPENT, PKMN_CHANDELURE, PKMN_MISDREAVUS, PKMN_MISMAGIUS, PKMN_MURKROW,
        PKMN_HONCHKROW, PKMN_CHINGLING, PKMN_CHIMECHO, PKMN_ABSOL, PKMN_NATU, PKMN_XATU,
        PKMN_CASTFORM,

        // deep sea
        PKMN_CHINCHOU, PKMN_LANTURN, PKMN_CLAMPERL, PKMN_HUNTAIL, PKMN_GOREBYSS, PKMN_WAILMER,
        PKMN_WAILORD, PKMN_CORSOLA, PKMN_CURSOLA, PKMN_LUVDISC, PKMN_MANTYKE, PKMN_MANTINE,
        PKMN_FINNEON, PKMN_LUMINEON, PKMN_MAREANIE, PKMN_TOXAPEX,

        // fossils n almost fossils
        PKMN_RELICANTH, PKMN_LILEEP, PKMN_CRADILY, PKMN_ANORITH, PKMN_ARMALDO,

        // pseudos
        PKMN_BELDUM, PKMN_METANG, PKMN_METAGROSS, PKMN_BAGON, PKMN_SHELGON, PKMN_SALAMENCE,
        PKMN_GIBLE, PKMN_GABITE, PKMN_GARCHOMP, PKMN_DEINO, PKMN_ZWEILOUS, PKMN_HYDREIGON,
        PKMN_GOOMY, PKMN_SLIGGOO, PKMN_GOODRA, PKMN_DREEPY, PKMN_DRAKLOAK, PKMN_DRAGAPULT,

        // legendaries
        PKMN_REGIROCK, PKMN_REGICE, PKMN_REGISTEEL, PKMN_REGIELEKI, PKMN_REGIDRAGO, PKMN_REGIGIGAS,
        PKMN_LATIAS, PKMN_LATIOS, PKMN_KYOGRE, PKMN_GROUDON, PKMN_RAYQUAZA,

        // mythical
        PKMN_JIRACHI, PKMN_DEOXYS,

        // secret
        PKMN_CHATOT };

    constexpr u16 LOCAL_DEX_PAGES[ MAX_LOCAL_DEX_PAGES ][ MAX_LOCAL_DEX_SLOTS ] = {
        { EMPTY, EMPTY, EMPTY },

        // Starter
        { PKMN_TREECKO, PKMN_GROVYLE, PKMN_SCEPTILE },
        { PKMN_TORCHIC, PKMN_COMBUSKEN, PKMN_BLAZIKEN },
        { PKMN_MUDKIP, PKMN_MARSHTOMP, PKMN_SWAMPERT },

        // fields
        { PKMN_POOCHYENA, PKMN_MIGHTYENA },
        { PKMN_ZIGZAGOON, PKMN_LINOONE },
        { EMPTY, EMPTY, PKMN_OBSTAGOON },
        { PKMN_SKITTY, PKMN_DELCATTY },
        { PKMN_GLAMEOW, PKMN_PURUGLY },
        { PKMN_ZORUA, PKMN_ZOROARK },
        { PKMN_WHISMUR, PKMN_LOUDRED, PKMN_EXPLOUD },
        { PKMN_ZANGOOSE, EMPTY, PKMN_SEVIPER },

        // Bugs
        { PKMN_WURMPLE, PKMN_SILCOON, PKMN_BEAUTIFLY },
        { EMPTY, PKMN_CASCOON, PKMN_DUSTOX },
        { PKMN_SURSKIT, PKMN_MASQUERAIN },
        { PKMN_NINCADA, PKMN_NINJASK },
        { EMPTY, PKMN_SHEDINJA },
        { PKMN_VOLBEAT, EMPTY, PKMN_ILLUMISE },
        { PKMN_PINSIR, EMPTY, PKMN_HERACROSS },
        { PKMN_LARVESTA, PKMN_VOLCARONA, EMPTY },

        // Birds (n bats)
        { PKMN_TAILLOW, PKMN_SWELLOW },
        { PKMN_WINGULL, PKMN_PELIPPER },
        { PKMN_ZUBAT, PKMN_GOLBAT, PKMN_CROBAT },
        { EMPTY, PKMN_SKARMORY },
        { PKMN_SWABLU, PKMN_ALTARIA },
        { PKMN_DODUO, PKMN_DODRIO },

        // Espers
        { PKMN_RALTS, PKMN_KIRLIA, PKMN_GARDEVOIR },
        { EMPTY, EMPTY, PKMN_GALLADE },
        { PKMN_ABRA, PKMN_KADABRA, PKMN_ALAKAZAM },
        { PKMN_SPOINK, PKMN_GRUMPIG },
        { PKMN_GOTHITA, PKMN_GOTHORITA, PKMN_GOTHITELLE },
        { PKMN_SOLOSIS, PKMN_DUOSION, PKMN_REUNICLUS },
        { PKMN_WYNAUT, PKMN_WOBBUFFET },
        { EMPTY, PKMN_GIRAFARIG },
        { PKMN_MUNNA, PKMN_MUSHARNA },

        // Plants and stuff from the forest
        { PKMN_LOTAD, PKMN_LOMBRE, PKMN_LUDICOLO },
        { PKMN_SEEDOT, PKMN_NUZLEAF, PKMN_SHIFTRY },
        { PKMN_SLAKOTH, PKMN_VIGOROTH, PKMN_SLAKING },
        { PKMN_SHROOMISH, PKMN_BRELOOM },
        { PKMN_FOONGUS, PKMN_AMOONGUSS },
        { PKMN_IGGLYBUFF, PKMN_JIGGLYPUFF, PKMN_WIGGLYTUFF },
        { PKMN_BUDEW, PKMN_ROSELIA, PKMN_ROSERADE },
        { PKMN_ODDISH, PKMN_GLOOM, PKMN_VILEPLUME },
        { EMPTY, EMPTY, PKMN_BELLOSSOM },
        { PKMN_CACNEA, PKMN_CACTURNE },
        { EMPTY, PKMN_MARACTUS },
        { EMPTY, PKMN_TROPIUS },
        { PKMN_HOPPIP, PKMN_SKIPLOOM, PKMN_JUMPLUFF },

        // rivers
        { PKMN_GOLDEEN, PKMN_SEAKING },
        { PKMN_MAGIKARP, PKMN_GYARADOS },
        { PKMN_FEEBAS, PKMN_MILOTIC },
        { PKMN_BARBOACH, PKMN_WHISCASH },
        { PKMN_AZURILL, PKMN_MARILL, PKMN_AZUMARILL },
        { PKMN_PSYDUCK, PKMN_GOLDUCK },

        // Rocks n caves
        { PKMN_GEODUDE, PKMN_GRAVELER, PKMN_GOLEM },
        { PKMN_NOSEPASS, PKMN_PROBOPASS },
        { PKMN_ARON, PKMN_LAIRON, PKMN_AGGRON },
        { PKMN_SABLEYE, EMPTY, PKMN_MAWILE },
        { PKMN_LUNATONE, EMPTY, PKMN_SOLROCK },
        { PKMN_RHYHORN, PKMN_RHYDON, PKMN_RHYPERIOR },

        // Things destroying rocks
        { PKMN_MAKUHITA, PKMN_HARIYAMA },
        { PKMN_MACHOP, PKMN_MACHOKE, PKMN_MACHAMP },
        { PKMN_MEDITITE, PKMN_MEDICHAM },
        { PKMN_CRABRAWLER, PKMN_CRABOMINABLE },

        // ocean
        { PKMN_TENTACOOL, PKMN_TENTACRUEL },
        { PKMN_CARVANHA, PKMN_SHARPEDO },
        { PKMN_HORSEA, PKMN_SEADRA, PKMN_KINGDRA },

        // bzzzt
        { PKMN_ELECTRIKE, PKMN_MANECTRIC },
        { PKMN_PLUSLE, EMPTY, PKMN_MINUN },
        { PKMN_MAGNEMITE, PKMN_MAGNETON, PKMN_MAGNEZONE },
        { PKMN_VOLTORB, PKMN_ELECTRODE },
        { EMPTY, PKMN_ROTOM },
        { PKMN_PICHU, PKMN_PIKACHU, PKMN_RAICHU },
        { EMPTY, PKMN_SPINDA },

        // burning stuff
        { PKMN_NUMEL, PKMN_CAMERUPT },
        { PKMN_SLUGMA, PKMN_MAGCARGO },
        { EMPTY, PKMN_TORKOAL },
        { PKMN_VULPIX, PKMN_NINETALES },
        { EMPTY, PKMN_TURTONATOR, EMPTY },

        // dirty stuff
        { PKMN_GULPIN, PKMN_SWALOT },
        { PKMN_GRIMER, PKMN_MUK },
        { PKMN_KOFFING, PKMN_WEEZING },
        { PKMN_TRUBBISH, PKMN_GARBODOR },
        { EMPTY, PKMN_KECLEON },

        // beach
        { PKMN_CORPHISH, PKMN_CRAWDAUNT },
        { PKMN_SANDYGAST, PKMN_PALOSSAND },
        { PKMN_STARYU, PKMN_STARMIE },
        { PKMN_CLOBBOPUS, PKMN_GRAPPLOCT },

        // hail and sand
        { PKMN_SPHEAL, PKMN_SEALEO, PKMN_WALREIN },
        { PKMN_SNORUNT, PKMN_GLALIE },
        { EMPTY, PKMN_FROSLASS },
        { PKMN_SNOM, PKMN_FROSMOTH },
        { PKMN_SNEASEL, PKMN_WEAVILE },
        { PKMN_GLIGAR, PKMN_GLISCOR },
        { PKMN_SANDSHREW, PKMN_SANDSLASH },
        { PKMN_BALTOY, PKMN_CLAYDOL },
        { PKMN_TRAPINCH, PKMN_VIBRAVA, PKMN_FLYGON },

        // spooky
        { PKMN_SHUPPET, PKMN_BANETTE },
        { EMPTY, PKMN_MIMIKYU },
        { PKMN_DUSKULL, PKMN_DUSCLOPS, PKMN_DUSKNOIR },
        { PKMN_LITWICK, PKMN_LAMPENT, PKMN_CHANDELURE },
        { PKMN_MISDREAVUS, PKMN_MISMAGIUS },
        { PKMN_MURKROW, PKMN_HONCHKROW },
        { PKMN_CHINGLING, PKMN_CHIMECHO },
        { EMPTY, PKMN_ABSOL },
        { PKMN_NATU, PKMN_XATU },
        { EMPTY, PKMN_CASTFORM },

        // deep sea
        { PKMN_CHINCHOU, PKMN_LANTURN },
        { PKMN_CLAMPERL, PKMN_HUNTAIL },
        { EMPTY, PKMN_GOREBYSS },
        { PKMN_WAILMER, PKMN_WAILORD },
        { EMPTY, PKMN_CORSOLA },
        { EMPTY, EMPTY, PKMN_CURSOLA },
        { EMPTY, PKMN_LUVDISC },
        { PKMN_MANTYKE, PKMN_MANTINE },
        { PKMN_FINNEON, PKMN_LUMINEON },
        { PKMN_MAREANIE, PKMN_TOXAPEX },

        // fossils n almost fossils
        { EMPTY, PKMN_RELICANTH },
        { PKMN_LILEEP, PKMN_CRADILY },
        { PKMN_ANORITH, PKMN_ARMALDO },

        // pseudos
        { PKMN_BELDUM, PKMN_METANG, PKMN_METAGROSS },
        { PKMN_BAGON, PKMN_SHELGON, PKMN_SALAMENCE },
        { PKMN_GIBLE, PKMN_GABITE, PKMN_GARCHOMP },
        { PKMN_DEINO, PKMN_ZWEILOUS, PKMN_HYDREIGON },
        { PKMN_GOOMY, PKMN_SLIGGOO, PKMN_GOODRA },
        { PKMN_DREEPY, PKMN_DRAKLOAK, PKMN_DRAGAPULT },

        // legendaries
        { PKMN_REGIROCK, PKMN_REGICE, PKMN_REGISTEEL },
        { PKMN_REGIELEKI, EMPTY, PKMN_REGIDRAGO },
        { EMPTY, PKMN_REGIGIGAS },
        { PKMN_LATIAS, EMPTY, PKMN_LATIOS },
        { PKMN_KYOGRE, PKMN_GROUDON, PKMN_RAYQUAZA },

        // mythical
        { EMPTY, PKMN_JIRACHI },
        { EMPTY, PKMN_DEOXYS },

        // secret
        { EMPTY, PKMN_CHATOT },

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

    constexpr bool requiredForCompletion( u16 p_pkmnIdx, bool p_localDex = false ) {
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
        case PKMN_CHATOT: return !p_localDex;
        }
        return true;
    }

    /*
     * @brief: returns the smallest index no of a pkmn in the nat dex that the player has
     * seen or caught.
     */
    u16 getFirstPkmnInNatDex( );

    /*
     * @brief: returns the highest index no of a pkmn in the nat dex that the player has
     * seen or caught.
     */
    u16 getLastPkmnInNatDex( );

    /*
     * @brief: returns the smallest index no of a pkmn in the local dex that the player has
     * seen or caught.
     */
    u16 getFirstPkmnInLocDex( );

    /*
     * @brief: returns the highest index no of a pkmn in the local dex that the player has
     * seen or caught.
     */
    u16 getLastPkmnInLocDex( );

    /*
     * @brief: Returns the pair (page, slot) containing the pkmn specified by p_natIdx or
     * (0, 255) if no such pair exists.
     */
    constexpr std::pair<u16, u8> getLocSlotForNat( u16 p_natIdx ) {
        for( u16 i = 0; i < MAX_LOCAL_DEX_PAGES - 1; ++i ) {
            for( u8 j = 0; j < MAX_LOCAL_DEX_SLOTS; ++j ) {
                if( LOCAL_DEX_PAGES[ i ][ j ] == p_natIdx ) { return { i, j }; }
            }
        }
        return { 0, 255 };
    }

    class dex {
      public:
        enum mode { LOCAL_DEX = 0, NATIONAL_DEX = 1, SHOW_SINGLE = 2 };

      private:
        dexUI* _dexUI;
        mode   _mode         = LOCAL_DEX;
        u8     _currentPage  = 0;
        u8     _currentForme = 0; // only available for the national dex mode

        u16                _natDexUB;
        u16                _natDexLB;
        u16                _locDexUB;
        u16                _locDexLB;
        std::pair<u16, u8> _locFirstPage;
        std::pair<u16, u8> _locLastPage;

        /*
         * @brief: Swaps between the local/national dex modes.
         * @param p_startIdx: pkmn to highlight in the new mode.
         */
        void changeMode( mode p_newMode, u16 p_startIdx );

        /*
         * @brief: Select an entry while in the national dex mode.
         */
        void selectNational( u16 p_pkmnIdx, bool p_forceDraw = false, u8 p_forme = 0,
                             bool p_shiny = false, bool p_female = false );

        /*
         * @brief: Select an entry while in the local dex mode.
         */
        void selectLocal( u16 p_page, u8 p_slot, s8 p_dir, bool p_forceDraw = false, u8 p_forme = 0,
                          bool p_shiny = false, bool p_female = false );

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
