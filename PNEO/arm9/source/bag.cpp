/*
Pokémon neo
------------------------------

file        : bag.cpp
author      : Philip Wellnitz
description :

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

#include <algorithm>
#include <nds.h>

#include "bag/bag.h"
#include "bag/item.h"
#include "gen/itemNames.h"
#include "pokemon.h"
#include "save/saveGame.h"

namespace BAG {

#define BETWEEN( p_lb, p_idx, p_ub ) ( ( p_lb ) <= ( p_idx ) && ( p_idx ) <= ( p_ub ) )

    constexpr u16 MAX_ITEMS_ORDER                = 350;
    constexpr u16 ITEMS_ORDER[ MAX_ITEMS_ORDER ] = {
        // usables
        I_REPEL, I_SUPER_REPEL, I_MAX_REPEL, I_ESCAPE_ROPE, I_HONEY,

        // evo items
        I_SUN_STONE, I_MOON_STONE, I_FIRE_STONE, I_THUNDER_STONE, I_WATER_STONE, I_LEAF_STONE,
        I_SHINY_STONE, I_DUSK_STONE, I_DAWN_STONE, I_OVAL_STONE, I_ICE_STONE, I_SWEET_APPLE,
        I_TART_APPLE, I_CRACKED_POT, I_CHIPPED_POT, I_GALARICA_CUFF, I_GALARICA_WREATH,

        // held items
        I_MACHO_BRACE, I_POWER_BRACER, I_POWER_BELT, I_POWER_LENS, I_POWER_BAND, I_POWER_ANKLET,
        I_POWER_WEIGHT,

        I_BRIGHT_POWDER, I_WHITE_HERB, I_QUICK_CLAW, I_SOOTHE_BELL, I_MENTAL_HERB, I_CHOICE_BAND,
        I_KING_S_ROCK, I_SILVER_POWDER, I_AMULET_COIN, I_CLEANSE_TAG, I_SMOKE_BALL, I_EVERSTONE,
        I_FOCUS_BAND, I_LUCKY_EGG, I_SCOPE_LENS, I_METAL_COAT, I_LEFTOVERS, I_DRAGON_SCALE,
        I_LIGHT_BALL, I_SOFT_SAND, I_HARD_STONE, I_MIRACLE_SEED, I_BLACK_GLASSES, I_BLACK_BELT,
        I_MAGNET, I_MYSTIC_WATER, I_SHARP_BEAK, I_POISON_BARB, I_NEVER_MELT_ICE, I_SPELL_TAG,
        I_TWISTED_SPOON, I_CHARCOAL, I_DRAGON_FANG, I_SILK_SCARF, I_SHELL_BELL, I_WIDE_LENS,
        I_MUSCLE_BAND, I_WISE_GLASSES, I_EXPERT_BELT, I_LIGHT_CLAY, I_LIFE_ORB, I_POWER_HERB,
        I_TOXIC_ORB, I_FLAME_ORB, I_QUICK_POWDER, I_FOCUS_SASH, I_ZOOM_LENS, I_METRONOME,
        I_IRON_BALL, I_LAGGING_TAIL, I_DESTINY_KNOT, I_BLACK_SLUDGE, I_ICY_ROCK, I_SMOOTH_ROCK,
        I_HEAT_ROCK, I_DAMP_ROCK, I_GRIP_CLAW, I_CHOICE_SCARF, I_STICKY_BARB, I_SHED_SHELL,
        I_BIG_ROOT, I_CHOICE_SPECS, I_EVIOLITE, I_FLOAT_STONE, I_ROCKY_HELMET, I_AIR_BALLOON,
        I_RED_CARD, I_RING_TARGET, I_BINDING_BAND, I_ABSORB_BULB, I_CELL_BATTERY, I_EJECT_BUTTON,
        I_WEAKNESS_POLICY, I_ASSAULT_VEST, I_LUMINOUS_MOSS, I_SNOWBALL, I_SAFETY_GOGGLES,
        I_ADRENALINE_ORB, I_TERRAIN_EXTENDER, I_PROTECTIVE_PADS, I_THROAT_SPRAY, I_EJECT_PACK,
        I_HEAVY_DUTY_BOOTS, I_BLUNDER_POLICY, I_ROOM_SERVICE, I_UTILITY_UMBRELLA,

        I_ELECTRIC_SEED, I_PSYCHIC_SEED, I_MISTY_SEED, I_GRASSY_SEED,

        I_FLAME_PLATE, I_SPLASH_PLATE, I_ZAP_PLATE, I_MEADOW_PLATE, I_ICICLE_PLATE, I_FIST_PLATE,
        I_TOXIC_PLATE, I_EARTH_PLATE, I_SKY_PLATE, I_MIND_PLATE, I_INSECT_PLATE, I_STONE_PLATE,
        I_SPOOKY_PLATE, I_DRACO_PLATE, I_DREAD_PLATE, I_IRON_PLATE, I_PIXIE_PLATE, I_NULL_PLATE,

        I_FIRE_GEM, I_WATER_GEM, I_ELECTRIC_GEM, I_GRASS_GEM, I_ICE_GEM, I_FIGHTING_GEM,
        I_POISON_GEM, I_GROUND_GEM, I_FLYING_GEM, I_PSYCHIC_GEM, I_BUG_GEM, I_ROCK_GEM, I_GHOST_GEM,
        I_DRAGON_GEM, I_DARK_GEM, I_STEEL_GEM, I_NORMAL_GEM, I_FAIRY_GEM,

        I_FIGHTING_MEMORY, I_FLYING_MEMORY, I_POISON_MEMORY, I_GROUND_MEMORY, I_ROCK_MEMORY,
        I_BUG_MEMORY, I_GHOST_MEMORY, I_STEEL_MEMORY, I_FIRE_MEMORY, I_WATER_MEMORY, I_GRASS_MEMORY,
        I_ELECTRIC_MEMORY, I_PSYCHIC_MEMORY, I_ICE_MEMORY, I_DRAGON_MEMORY, I_DARK_MEMORY,
        I_FAIRY_MEMORY,

        I_DEEP_SEA_TOOTH, I_DEEP_SEA_SCALE, I_LUCKY_PUNCH, I_METAL_POWDER, I_THICK_CLUB, I_LEEK,

        I_SOUL_DEW, I_ADAMANT_ORB, I_LUSTROUS_ORB, I_GRISEOUS_ORB, I_DOUSE_DRIVE, I_SHOCK_DRIVE,
        I_BURN_DRIVE, I_CHILL_DRIVE,

        I_UPGRADE, I_PROTECTOR, I_ELECTIRIZER, I_MAGMARIZER, I_DUBIOUS_DISC, I_REAPER_CLOTH,
        I_RAZOR_CLAW, I_RAZOR_FANG, I_PRISM_SCALE, I_WHIPPED_DREAM, I_SACHET, I_STRAWBERRY_SWEET,
        I_LOVE_SWEET, I_BERRY_SWEET, I_CLOVER_SWEET, I_FLOWER_SWEET, I_STAR_SWEET, I_RIBBON_SWEET,

        I_SEA_INCENSE, I_LAX_INCENSE, I_ODD_INCENSE, I_ROCK_INCENSE, I_FULL_INCENSE, I_WAVE_INCENSE,
        I_ROSE_INCENSE, I_LUCK_INCENSE, I_PURE_INCENSE,

        I_RED_SCARF, I_BLUE_SCARF, I_PINK_SCARF, I_GREEN_SCARF, I_YELLOW_SCARF,

        // mega stones

        I_GENGARITE, I_GARDEVOIRITE, I_AMPHAROSITE, I_VENUSAURITE, I_CHARIZARDITE_X,
        I_BLASTOISINITE, I_MEWTWONITE_X, I_MEWTWONITE_Y, I_BLAZIKENITE, I_MEDICHAMITE,
        I_HOUNDOOMINITE, I_AGGRONITE, I_BANETTITE, I_TYRANITARITE, I_SCIZORITE, I_PINSIRITE,
        I_AERODACTYLITE, I_LUCARIONITE, I_ABOMASITE, I_KANGASKHANITE, I_GYARADOSITE, I_ABSOLITE,
        I_CHARIZARDITE_Y, I_ALAKAZITE, I_HERACRONITE, I_MAWILITE, I_MANECTITE, I_GARCHOMPITE,
        I_LATIASITE, I_LATIOSITE, I_SWAMPERTITE, I_SCEPTILITE, I_SABLENITE, I_ALTARIANITE,
        I_GALLADITE, I_AUDINITE, I_METAGROSSITE, I_SHARPEDONITE, I_SLOWBRONITE, I_STEELIXITE,
        I_PIDGEOTITE, I_GLALITITE, I_DIANCITE, I_CAMERUPTITE, I_LOPUNNITE, I_SALAMENCITE,
        I_BEEDRILLITE,

        // poke balls
        I_POKE_BALL, I_GREAT_BALL, I_ULTRA_BALL, I_MASTER_BALL, I_PREMIER_BALL, I_CHERISH_BALL,
        I_LUXURY_BALL, I_NET_BALL, I_DIVE_BALL, I_NEST_BALL, I_REPEAT_BALL, I_TIMER_BALL,
        I_DUSK_BALL, I_QUICK_BALL, I_HEAL_BALL, I_FAST_BALL, I_LEVEL_BALL, I_LURE_BALL,
        I_HEAVY_BALL, I_LOVE_BALL, I_FRIEND_BALL, I_MOON_BALL, I_BEAST_BALL,

        I_DREAM_BALL, I_SPORT_BALL, I_PARK_BALL, I_SAFARI_BALL,

        // battle items
        I_GUARD_SPEC, I_DIRE_HIT, I_X_ATTACK, I_X_DEFENSE, I_X_SPEED, I_X_ACCURACY, I_X_SP_ATK,
        I_X_SP_DEF, I_POKE_DOLL, I_FLUFFY_TAIL, I_POKE_TOY,

        // rest sorted by idx no
        0 };

    constexpr u16 MEDICINES_ORDER[] = { I_POTION,
                                        I_SUPER_POTION,
                                        I_HYPER_POTION,
                                        I_MAX_POTION,
                                        I_FULL_RESTORE,
                                        I_FRESH_WATER,
                                        I_SODA_POP,
                                        I_LEMONADE,
                                        I_BERRY_JUICE,
                                        I_MOOMOO_MILK,

                                        I_ANTIDOTE,
                                        I_BURN_HEAL,
                                        I_ICE_HEAL,
                                        I_AWAKENING,
                                        I_PARALYZE_HEAL,
                                        I_FULL_HEAL,
                                        I_REVIVE,
                                        I_MAX_REVIVE,

                                        I_ENERGY_POWDER,
                                        I_ENERGY_ROOT,
                                        I_HEAL_POWDER,
                                        I_REVIVAL_HERB,

                                        I_ETHER,
                                        I_MAX_ETHER,
                                        I_ELIXIR,
                                        I_MAX_ELIXIR,

                                        I_LAVA_COOKIE,
                                        I_OLD_GATEAU,
                                        I_SWEET_HEART,
                                        I_CASTELIACONE,
                                        I_LUMIOSE_GALETTE,
                                        I_SHALOUR_SABLE,

                                        I_SACRED_ASH,

                                        I_HP_UP,
                                        I_PROTEIN,
                                        I_IRON,
                                        I_CARBOS,
                                        I_CALCIUM,
                                        I_ZINC,

                                        I_HEALTH_FEATHER,
                                        I_MUSCLE_FEATHER,
                                        I_RESIST_FEATHER,
                                        I_GENIUS_FEATHER,
                                        I_CLEVER_FEATHER,
                                        I_SWIFT_FEATHER,

                                        I_PP_UP,
                                        I_PP_MAX,

                                        I_RARE_CANDY,

                                        I_ABILITY_CAPSULE,
                                        I_ABILITY_PATCH,

                                        0 };

    constexpr u16 KEY_ITEMS_ORDER[]
        = { I_EXP_ALL, I_MACH_BIKE, I_ACRO_BIKE, I_WAILMER_PAIL, I_OLD_ROD, I_GOOD_ROD, I_SUPER_ROD,
            I_POKEBLOCK_KIT, I_POKE_RADAR, I_EON_FLUTE, I_ZYGARDE_CUBE,

            I_COIN_CASE, I_SOOT_SACK,

            I_REVEAL_GLASS, I_PRISON_BOTTLE, I_RED_NECTAR, I_YELLOW_NECTAR, I_PINK_NECTAR,
            I_PURPLE_NECTAR, I_N_SOLARIZER, I_N_LUNARIZER, I_N_SOLARIZER2, I_N_LUNARIZER2,

            I_DEVON_GOODS, I_BASEMENT_KEY2, I_LETTER, I_EON_TICKET, I_SCANNER, I_GO_GOGGLES,
            I_METEORITE, I_KEY_TO_ROOM_1, I_KEY_TO_ROOM_2, I_KEY_TO_ROOM_4, I_KEY_TO_ROOM_6,
            I_STORAGE_KEY2, I_DEVON_SCOPE, I_SS_TICKET2,

            I_DEVON_SCUBA_GEAR, I_CONTEST_COSTUME, I_CONTEST_COSTUME2, I_MAGMA_SUIT, I_AQUA_SUIT,
            I_PAIR_OF_TICKETS, I_MEGA_BRACELET, I_METEORITE2, I_SECRET_KEY2, I_SS_TICKET3,
            I_SILPH_SCOPE,

            I_OVAL_CHARM, I_SHINY_CHARM, I_CATCHING_CHARM, I_WISHING_CHARM,

            I_RED_ORB, I_BLUE_ORB, I_JADE_ORB,

            I_RAINBOW_PASS, I_MYSTIC_TICKET, I_OLD_SEA_MAP, I_AURORA_TICKET,

            // unobtainable key items are sorted by index number

            0 };

    void bag::sort( bagType p_bagType ) {
        if( p_bagType == BERRIES ) {
            std::sort( &_items[ _startIdx[ p_bagType ] ], &_items[ _nextFree[ p_bagType ] ],
                       []( std::pair<u16, u16> p_left, std::pair<u16, u16> p_right ) {
                           if( itemToBerry( p_left.first ) != itemToBerry( p_right.first ) ) {
                               return itemToBerry( p_left.first ) < itemToBerry( p_right.first );
                           }
                           return p_left.first < p_right.first;
                       } );
            return;
        }

        if( p_bagType == TM_HM ) {
            // This looks horrible, but at least it's reasonably fast.
            std::sort( &_items[ _startIdx[ p_bagType ] ], &_items[ _nextFree[ p_bagType ] ],
                       []( std::pair<u16, u16> p_left, std::pair<u16, u16> p_right ) {
                           u16 idx1 = p_left.first, idx2 = p_right.first;

                           if( BETWEEN( I_TM01, idx1, I_TM92 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return idx1 < idx2;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_HM00, idx1, I_HM00 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return true;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return true;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_HM09, idx1, I_HM09 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return true;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_HM01, idx1, I_HM08 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return true;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return true;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return idx1 < idx2;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_TM00, idx1, I_TM00 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return true;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_TM93, idx1, I_TM95 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return idx1 < idx2;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return true;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_TM96, idx1, I_TM99 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return idx1 < idx2;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return true;
                               }
                           }

                           if( BETWEEN( I_TR00, idx1, I_TR99 ) ) {
                               if( BETWEEN( I_TM01, idx2, I_TM92 ) ) {
                                   return idx1 < idx2;
                               } else if( BETWEEN( I_HM00, idx2, I_HM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM09, idx2, I_HM09 ) ) {
                                   return false;
                               } else if( BETWEEN( I_HM01, idx2, I_HM08 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM00, idx2, I_TM00 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM93, idx2, I_TM95 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TM96, idx2, I_TM99 ) ) {
                                   return false;
                               } else if( BETWEEN( I_TR00, idx2, I_TR99 ) ) {
                                   return idx1 < idx2;
                               }
                           }
                           return idx1 < idx2;
                       } );
            return;
        }

        if( p_bagType == KEY_ITEMS ) {
            std::sort( &_items[ _startIdx[ p_bagType ] ], &_items[ _nextFree[ p_bagType ] ],
                       []( std::pair<u16, u16> p_left, std::pair<u16, u16> p_right ) {
                           u16 idx1 = p_left.first, idx2 = p_right.first;
                           for( u16 i = 0; KEY_ITEMS_ORDER[ i ]; ++i ) {
                               if( KEY_ITEMS_ORDER[ i ] == idx2 ) { return false; }
                               if( KEY_ITEMS_ORDER[ i ] == idx1 ) { return true; }
                           }
                           return idx1 < idx2;
                       } );
            return;
        }

        if( p_bagType == ITEMS ) {
            std::sort( &_items[ _startIdx[ p_bagType ] ], &_items[ _nextFree[ p_bagType ] ],
                       []( std::pair<u16, u16> p_left, std::pair<u16, u16> p_right ) {
                           u16 idx1 = p_left.first, idx2 = p_right.first;
                           for( u16 i = 0; i < MAX_ITEMS_ORDER; ++i ) {
                               if( ITEMS_ORDER[ i ] == idx2 ) { return false; }
                               if( ITEMS_ORDER[ i ] == idx1 ) { return true; }
                           }
                           return idx1 < idx2;
                       } );
            return;
        }

        if( p_bagType == MEDICINE ) {
            std::sort( &_items[ _startIdx[ p_bagType ] ], &_items[ _nextFree[ p_bagType ] ],
                       []( std::pair<u16, u16> p_left, std::pair<u16, u16> p_right ) {
                           u16 idx1 = p_left.first, idx2 = p_right.first;
                           for( u16 i = 0; MEDICINES_ORDER[ i ]; ++i ) {
                               if( MEDICINES_ORDER[ i ] == idx2 ) { return false; }
                               if( MEDICINES_ORDER[ i ] == idx1 ) { return true; }
                           }
                           return idx1 < idx2;
                       } );
            return;
        }

        return;
    }

    void bag::insert( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        for( u16 i = _startIdx[ p_bagType ]; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ i ].first == p_itemId ) {
                _items[ i ].second += p_cnt;
                return;
            }
        if( _nextFree[ p_bagType ] >= MAX_ITEMS_IN_BAG
            || _nextFree[ p_bagType ] == _startIdx[ 1 + p_bagType ] ) // Insert failed.
            return;
        _items[ _nextFree[ p_bagType ]++ ] = { p_itemId, p_cnt };

        if( p_bagType == TM_HM || p_bagType == BERRIES ) { sort( p_bagType ); }
    }

    void bag::erase( bagType p_bagType, u16 p_itemId, u16 p_cnt ) {
        for( u16 i = _startIdx[ p_bagType ]; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ i ].first == p_itemId ) {
                if( p_cnt <= _items[ i ].second )
                    _items[ i ].second -= p_cnt;
                else
                    _items[ i ].second = 0;
                if( !_items[ i ].second ) {
                    for( u16 j = i; j < _nextFree[ p_bagType ]; ++j ) _items[ j ] = _items[ j + 1 ];
                    --_nextFree[ p_bagType ];
                }
                return;
            }
    }

    u16 bag::count( bagType p_bagType, u16 p_itemId ) {
        for( u16 i = _startIdx[ p_bagType ]; i < _nextFree[ p_bagType ]; ++i )
            if( _items[ i ].first == p_itemId ) return _items[ i ].second;
        return 0;
    }

    bool bag::empty( bagType p_bagType ) {
        return !( _nextFree[ p_bagType ] - _startIdx[ p_bagType ] );
    }

    std::size_t bag::size( bagType p_bagType ) {
        return _nextFree[ p_bagType ] - _startIdx[ p_bagType ];
    }

    std::pair<u16, u16>* bag::begin( bagType p_bagType ) {
        return _items + _startIdx[ p_bagType ];
    }
    const std::pair<u16, u16>* bag::cbegin( bagType p_bagType ) const {
        return _items + _startIdx[ p_bagType ];
    }
    std::pair<u16, u16>* bag::end( bagType p_bagType ) {
        return _items + _nextFree[ p_bagType ];
    }
    const std::pair<u16, u16>* bag::cend( bagType p_bagType ) const {
        return _items + _nextFree[ p_bagType ];
    }

    void bag::clear( bagType p_bagType ) {
        memset( _items + _startIdx[ p_bagType ], 0,
                _startIdx[ p_bagType + 1 ] - _startIdx[ p_bagType ] + 1 );
        _nextFree[ p_bagType ] = _startIdx[ p_bagType ];
    }

    void bag::swap( bagType p_bagType, u16 p_idx1, u16 p_idx2 ) {
        std::swap( begin( p_bagType )[ p_idx1 ], begin( p_bagType )[ p_idx2 ] );
    }

} // namespace BAG
