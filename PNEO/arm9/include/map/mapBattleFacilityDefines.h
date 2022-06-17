/*
Pokémon neo
------------------------------

file        : mapBattleFacilityDefines.h
author      : Philip Wellnitz
description : General map stuff.

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
#include "battle/battleDefines.h"
#include "defines.h"
#include "gen/pokemonNames.h"
#include "pokemon.h"

namespace MAP {
    struct ruleSet {
        u8 m_id;
        u8 m_level;
        u8 m_numPkmn;
        u8 m_hasSpecialBattles; // if set to 1, use fixed battles for 21st and 49th battle that
                                // award symbols
        u8                 m_numBattles;
        BATTLE::battleMode m_battleMode;
    };

    constexpr u8         NUM_RULESETS = 5;
    extern const ruleSet FACILITY_RULE_SETS[ NUM_RULESETS ];

    constexpr u8 VARIANTS_PER_PKMN  = 3;
    constexpr u8 TEAMS_PER_CLASS    = 15;
    constexpr u8 TRAINERS_PER_CLASS = 5;

    constexpr u8 IV_MAX_STREAK                  = 7;
    constexpr u8 IV_FOR_STREAK[ IV_MAX_STREAK ] = { 3, 6, 9, 12, 15, 21, 31 };

    constexpr u8 TIER_BAD     = 0; // LC, NFE, untiered
    constexpr u8 TIER_AVERAGE = 1; // RU, NU, PU
    constexpr u8 TIER_GOOD    = 2; // non-banned ubers, OU, UU
    constexpr u8 TIER_BANNED  = 3;

    constexpr u8 TIER_MAX_STREAK = 5;
    constexpr u8 TIER_FOR_STREAK[ TIER_MAX_STREAK ]
        = { TIER_BAD, TIER_BAD, TIER_AVERAGE, TIER_AVERAGE, TIER_GOOD };

    /*
     * @brief: returns the general strength tier (as agrred upon by the competitive scene) of the
     * specified pkmn species.
     */
    constexpr u8 getTier( u16 p_species ) {
        switch( p_species ) {
        case PKMN_MEWTWO:
        case PKMN_MEW:
        case PKMN_LUGIA:
        case PKMN_HO_OH:
        case PKMN_CELEBI:
        case PKMN_KYOGRE:
        case PKMN_GROUDON:
        case PKMN_RAYQUAZA:
        case PKMN_JIRACHI:
        case PKMN_DEOXYS:
        case PKMN_DIALGA:
        case PKMN_PALKIA:
        case PKMN_GIRATINA:
        case PKMN_PHIONE:
        case PKMN_MANAPHY:
        case PKMN_DARKRAI:
        case PKMN_SHAYMIN:
        case PKMN_ARCEUS:
        case PKMN_VICTINI:
        case PKMN_RESHIRAM:
        case PKMN_ZEKROM:
        case PKMN_KYUREM:
        case PKMN_KELDEO:
        case PKMN_MELOETTA:
        case PKMN_GENESECT:
        case PKMN_XERNEAS:
        case PKMN_YVELTAL:
        case PKMN_ZYGARDE:
        case PKMN_DIANCIE:
        case PKMN_HOOPA:
        case PKMN_VOLCANION:
        case PKMN_COSMOG:
        case PKMN_COSMOEM:
        case PKMN_SOLGALEO:
        case PKMN_LUNALA:
        case PKMN_NECROZMA:
        case PKMN_MAGEARNA:
        case PKMN_MARSHADOW:
        case PKMN_ZERAORA:
        case PKMN_MELTAN:
        case PKMN_MELMETAL:
        case PKMN_ZACIAN:
        case PKMN_ZAMAZENTA:
        case PKMN_ETERNATUS:
        case PKMN_CALYREX:
        case PKMN_ZARUDE: return TIER_BANNED;
        case PKMN_VENUSAUR:
        case PKMN_NIDOKING:
        case PKMN_CLEFABLE:
        case PKMN_ALAKAZAM:
        case PKMN_TENTACRUEL:
        case PKMN_SLOWBRO:
        case PKMN_GENGAR:
        case PKMN_CHANSEY:
        case PKMN_GYARADOS:
        case PKMN_ZAPDOS:
        case PKMN_MOLTRES:
        case PKMN_DRAGONITE:
        case PKMN_AZUMARILL:
        case PKMN_SLOWKING:
        case PKMN_SCIZOR:
        case PKMN_SKARMORY:
        case PKMN_BLISSEY:
        case PKMN_TYRANITAR:
        case PKMN_BLAZIKEN:
        case PKMN_SWAMPERT:
        case PKMN_PELIPPER:
        case PKMN_CRAWDAUNT:
        case PKMN_SALAMENCE:
        case PKMN_LATIAS:
        case PKMN_LATIOS:
        case PKMN_MAGNEZONE:
        case PKMN_HEATRAN:
        case PKMN_GARCHOMP:
        case PKMN_HIPPOWDON:
        case PKMN_WEAVILE:
        case PKMN_TANGROWTH:
        case PKMN_MAMOSWINE:
        case PKMN_AZELF:
        case PKMN_KROOKODILE:
        case PKMN_FERROTHORN:
        case PKMN_VOLCARONA:
        case PKMN_EXCADRILL:
        case PKMN_CONKELDURR:
        case PKMN_AMOONGUSS:
        case PKMN_MIENSHAO:
        case PKMN_BISHARP:
        case PKMN_MANDIBUZZ:
        case PKMN_HYDREIGON:
        case PKMN_TERRAKION:
        case PKMN_THUNDURUS:
        case PKMN_LANDORUS:
        case PKMN_AEGISLASH:
        case PKMN_HAWLUCHA:
        case PKMN_TAPU_KOKO:
        case PKMN_TAPU_LELE:
        case PKMN_TAPU_BULU:
        case PKMN_TAPU_FINI:
        case PKMN_PRIMARINA:
        case PKMN_TOXAPEX:
        case PKMN_KOMMO_O:
        case PKMN_NIHILEGO:
        case PKMN_BUZZWOLE:
        case PKMN_PHEROMOSA:
        case PKMN_CELESTEELA:
        case PKMN_KARTANA:
        case PKMN_NAGANADEL:
        case PKMN_BLACEPHALON:
        case PKMN_RILLABOOM:
        case PKMN_CINDERACE:
        case PKMN_CORVIKNIGHT:
        case PKMN_BARRASKEWDA:
        case PKMN_HATTERENE:
        case PKMN_DRACOZOLT:
        case PKMN_ARCTOZOLT:
        case PKMN_DRACOVISH:
        case PKMN_DRAGAPULT:
        case PKMN_REGIELEKI:
        case PKMN_SPECTRIER:
        case PKMN_ENAMORUS: return TIER_GOOD;
        case PKMN_CHARIZARD:
        case PKMN_BLASTOISE:
        case PKMN_SANDSLASH:
        case PKMN_NIDOQUEEN:
        case PKMN_VILEPLUME:
        case PKMN_ARCANINE:
        case PKMN_MACHAMP:
        case PKMN_CLOYSTER:
        case PKMN_KINGLER:
        case PKMN_HITMONLEE:
        case PKMN_WEEZING:
        case PKMN_STARMIE:
        case PKMN_SCYTHER:
        case PKMN_TAUROS:
        case PKMN_VAPOREON:
        case PKMN_JOLTEON:
        case PKMN_OMASTAR:
        case PKMN_AERODACTYL:
        case PKMN_SNORLAX:
        case PKMN_HITMONTOP:
        case PKMN_MANTINE:
        case PKMN_RAIKOU:
        case PKMN_ENTEI:
        case PKMN_SUICUNE:
        case PKMN_CROBAT:
        case PKMN_LANTURN:
        case PKMN_XATU:
        case PKMN_QUAGSIRE:
        case PKMN_ESPEON:
        case PKMN_UMBREON:
        case PKMN_STEELIX:
        case PKMN_HERACROSS:
        case PKMN_KINGDRA:
        case PKMN_PORYGON2:
        case PKMN_MILOTIC:
        case PKMN_SCEPTILE:
        case PKMN_GARDEVOIR:
        case PKMN_EXPLOUD:
        case PKMN_REGIROCK:
        case PKMN_REGISTEEL:
        case PKMN_FLYGON:
        case PKMN_LINOONE:
        case PKMN_NINJASK:
        case PKMN_AGGRON:
        case PKMN_SHARPEDO:
        case PKMN_CLAYDOL:
        case PKMN_ABSOL:
        case PKMN_METAGROSS:
        case PKMN_ROSERADE:
        case PKMN_TOGEKISS:
        case PKMN_GALLADE:
        case PKMN_GASTRODON:
        case PKMN_BRONZONG:
        case PKMN_LUCARIO:
        case PKMN_DRAPION:
        case PKMN_TOXICROAK:
        case PKMN_RHYPERIOR:
        case PKMN_PORYGON_Z:
        case PKMN_FROSLASS:
        case PKMN_MESPRIT:
        case PKMN_CRESSELIA:
        case PKMN_GIGALITH:
        case PKMN_SEISMITOAD:
        case PKMN_SCOLIPEDE:
        case PKMN_WHIMSICOTT:
        case PKMN_CHANDELURE:
        case PKMN_AUDINO:
        case PKMN_DARMANITAN:
        case PKMN_SCRAFTY:
        case PKMN_SIGILYPH:
        case PKMN_COFAGRIGUS:
        case PKMN_ARCHEOPS:
        case PKMN_GARBODOR:
        case PKMN_ZOROARK:
        case PKMN_REUNICLUS:
        case PKMN_ESCAVALIER:
        case PKMN_JELLICENT:
        case PKMN_GALVANTULA:
        case PKMN_FERROSEED:
        case PKMN_HAXORUS:
        case PKMN_DRUDDIGON:
        case PKMN_GOLURK:
        case PKMN_BRAVIARY:
        case PKMN_DURANT:
        case PKMN_COBALION:
        case PKMN_VIRIZION:
        case PKMN_TORNADUS:
        case PKMN_NOIVERN:
        case PKMN_DIGGERSBY:
        case PKMN_TALONFLAME:
        case PKMN_PANGORO:
        case PKMN_DOUBLADE:
        case PKMN_AROMATISSE:
        case PKMN_SLURPUFF:
        case PKMN_BARBARACLE:
        case PKMN_DRAGALGE:
        case PKMN_CLAWITZER:
        case PKMN_HELIOLISK:
        case PKMN_TYRANTRUM:
        case PKMN_SYLVEON:
        case PKMN_GOODRA:
        case PKMN_KLEFKI:
        case PKMN_LYCANROC:
        case PKMN_DECIDUEYE:
        case PKMN_INCINEROAR:
        case PKMN_RIBOMBEE:
        case PKMN_MUDSDALE:
        case PKMN_ARAQUANID:
        case PKMN_SALAZZLE:
        case PKMN_BEWEAR:
        case PKMN_TSAREENA:
        case PKMN_COMFEY:
        case PKMN_PASSIMIAN:
        case PKMN_GOLISOPOD:
        case PKMN_PALOSSAND:
        case PKMN_SILVALLY:
        case PKMN_TOGEDEMARU:
        case PKMN_MIMIKYU:
        case PKMN_DRAMPA:
        case PKMN_DHELMISE:
        case PKMN_XURKITREE:
        case PKMN_GUZZLORD:
        case PKMN_STAKATAKA:
        case PKMN_INTELEON:
        case PKMN_SANDACONDA:
        case PKMN_TOXTRICITY:
        case PKMN_POLTEAGEIST:
        case PKMN_GRIMMSNARL:
        case PKMN_OBSTAGOON:
        case PKMN_SIRFETCH_D:
        case PKMN_INDEEDEE:
        case PKMN_COPPERAJAH:
        case PKMN_DURALUDON:
        case PKMN_REGIDRAGO:
        case PKMN_GLASTRIER:
        case PKMN_WYRDEER:
        case PKMN_KLEAVOR:
        case PKMN_URSALUNA:
        case PKMN_BASCULEGION:
        case PKMN_SNEASLER:
        case PKMN_OVERQWIL: return TIER_AVERAGE;
        default: return TIER_BAD;
        }
    }

    constexpr u8 NO_VARIANT = 255;
    constexpr u8 getSpeciesVariantForStreak( u16 p_species, u16 p_streak ) {
        u8 baseTier = getTier( p_species );
        u8 streak   = p_streak >= TIER_MAX_STREAK ? TIER_MAX_STREAK - 1 : p_streak;

        if( baseTier > TIER_FOR_STREAK[ streak ] ) { return NO_VARIANT; }
        if( baseTier + 1 < TIER_FOR_STREAK[ streak ] ) { return NO_VARIANT; }
        if( baseTier < TIER_FOR_STREAK[ streak ] ) { return 2; }
        if( baseTier == TIER_GOOD ) {
            if( p_streak >= TIER_MAX_STREAK - 1 && p_streak < TIER_MAX_STREAK + 2 ) {
                return p_streak - ( TIER_MAX_STREAK - 1 );
            }
            return rand( ) % 3;
        }
        return rand( ) % 2;
    }

} // namespace MAP
