/*
Pokémon Emerald 2 Version
------------------------------

file        : ability.h
author      : Philip Wellnitz 
description :

Copyright (C) 2012 - 2015
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

#pragma once

#include <string>
#include "script.h"

class ability {
public:
    enum abilityType {
        ATTACK = 1,
        BEFORE_BATTLE = 2,
        AFTER_BATTLE = 4,
        GRASS = 8,
        BEFORE_ATTACK = 16,
        AFTER_ATTACK = 32,
        BETWEEN_TURNS = 64
    };
    std::string         m_abilityName;
    std::string         m_flavourText;
    abilityType         m_type;

    BATTLE::battleScript m_effect;

    ///Constructors
    ability( ) { }

    ability( int p_abilityId );
};
std::wstring getWAbilityName( int p_abilityId );

#define A_CACOPHONY 0
#define A_STENCH 1
#define A_DRIZZLE 2
#define A_SPEED_BOOST 3
#define A_BATTLE_ARMOR 4
#define A_STURDY 5
#define A_DAMP 6
#define A_LIMBER 7
#define A_SAND_VEIL 8
#define A_STATIC 9
#define A_VOLT_ABSORB 10
#define A_WATER_ABSORB 11
#define A_OBLIVIOUS 12
#define A_CLOUD_NINE 13
#define A_COMPOUND_EYES 14
#define A_INSOMNIA 15
#define A_COLOR_CHANGE 16
#define A_IMMUNITY 17
#define A_FLASH_FIRE 18
#define A_SHIELD_DUST 19
#define A_OWN_TEMPO 20
#define A_SUCTION_CUPS 21
#define A_INTIMIDATE 22
#define A_SHADOW_TAG 23
#define A_ROUGH_SKIN 24
#define A_WONDER_GUARD 25
#define A_LEVITATE 26
#define A_EFFECT_SPORE 27
#define A_SYNCHRONIZE 28
#define A_CLEAR_BODY 29
#define A_NATURAL_CURE 30
#define A_LIGHTNING_ROD 31
#define A_SERENE_GRACE 32
#define A_SWIFT_SWIM 33
#define A_CHLOROPHYLL 34
#define A_ILLUMINATE 35
#define A_TRACE 36
#define A_HUGE_POWER 37
#define A_POISON_POINT 38
#define A_INNER_FOCUS 39
#define A_MAGMA_ARMOR 40
#define A_WATER_VEIL 41
#define A_MAGNET_PULL 42
#define A_SOUNDPROOF 43
#define A_RAIN_DISH 44
#define A_SAND_STREAM 45
#define A_PRESSURE 46
#define A_THICK_FAT 47
#define A_EARLY_BIRD 48
#define A_FLAME_BODY 49
#define A_RUN_AWAY 50
#define A_KEEN_EYE 51
#define A_HYPER_CUTTER 52
#define A_PICKUP 53
#define A_TRUANT 54
#define A_HUSTLE 55
#define A_CUTE_CHARM 56
#define A_PLUS 57
#define A_MINUS 58
#define A_FORECAST 59
#define A_STICKY 60
#define A_SHED_SKIN 61
#define A_GUTS 62
#define A_MARVEL_SCALE 63
#define A_LIQUID_OOZE 64
#define A_OVERGROW 65
#define A_BLAZE 66
#define A_TORRENT 67
#define A_SWARM 68
#define A_ROCK_HEAD 69
#define A_DROUGHT 70
#define A_ARENA_TRAP 71
#define A_VITAL_SPIRIT 72
#define A_WHITE_SMOKE 73
#define A_PURE_POWER 74
#define A_SHELL_ARMOR 75
#define A_AIR_LOCK 76
#define A_TANGLED_FEET 77
#define A_MOTOR_DRIVE 78
#define A_RIVALRY 79
#define A_STEADFAST 80
#define A_SNOW_CLOAK 81
#define A_GLUTTONY 82
#define A_ANGER_POINT 83
#define A_UBURDEN 84
#define A_HEATPROOF 85
#define A_SIMPLE 86
#define A_DRY_SKIN 87
#define A_DOWNLOAD 88
#define A_IRON_FIST 89
#define A_POISON_HEAL 90
#define A_ADAPTABILITY 91
#define A_SKILL_LINK 92
#define A_HYDRATION 93
#define A_SOLAR_POWER 94
#define A_QUICK_FEET 95
#define A_NORMALIZE 96
#define A_SNIPER_POWERS 97
#define A_MAGIC_GUARD 98
#define A_NO_GUARD 99
#define A_STALL 100
#define A_TECHNICIAN 101
#define A_LEAF_GUARD 102
#define A_KLUTZ 103
#define A_MOLD_BREAKER 104
#define A_SUPER_LUCK 105
#define A_AFTERMATH 106
#define A_ANTICIPATION 107
#define A_FOREWARN 108
#define A_UNAWARE 109
#define A_TINTED_LENS 110
#define A_FILTER 111
#define A_SLOW_START 112
#define A_SCRAPPY 113
#define A_STORM_DRAIN 114
#define A_ICE_BODY 115
#define A_SOLID_ROCK 116
#define A_SNOW_WARNING 117
#define A_HONEY_GATHER 118
#define A_FRISK 119
#define A_RECKLESS 120
#define A_MULTITYPE 121
#define A_FLOWER_GIFT 122
#define A_BAD_DREAMS 123
#define A_PICKPOCKET 124
#define A_SHEER_FORCE 125
#define A_CONTRARY 126
#define A_UNNERVE 127
#define A_DEFIANT 128
#define A_DEFEATIST 129
#define A_CURSED_BODY 130
#define A_HEALER 131
#define A_FRIEND_GUARD 132
#define A_WEAK_ARMOR 133
#define A_HEAVY_METAL 134
#define A_LIGHT_METAL 135
#define A_MULTISCALE 136
#define A_TOXIC_BOOST 137
#define A_FLARE_BOOST 138
#define A_HARVEST 139
#define A_TELEPATHY 140
#define A_MOODY 141
#define A_OVERCOAT 142
#define A_POISON_TOUCH 143
#define A_REGENERATOR 144
#define A_BIG_PECKS 145
#define A_SAND_RUSH 146
#define A_WONDER_SKIN 147
#define A_ANALYTIC 148
#define A_ILLUSION 149
#define A_IMPOSTER 150
#define A_INFILTRATOR 151
#define A_MUMMY 152
#define A_MOXIE 153
#define A_JUSTIFIED 154
#define A_RATTLED 155
#define A_MAGIC_BOUNCE 156
#define A_SAP_SIPPER 157
#define A_PRANKSTER 158
#define A_SAND_FORCE 159
#define A_IRON_BARBS 160
#define A_ZEN_MODE 161
#define A_VICTORY_STAR 162
#define A_TURBOBLAZE 163
#define A_TERAVOLT 164
#define A_AROMA_VEIL 165
#define A_FLOWER_VEIL 166
#define A_CHEEK_POUCH 167
#define A_PROTEAN 168
#define A_FUR_COAT 169
#define A_MAGICIAN 170
#define A_BULLETPROOF 171
#define A_COMPETITIV 172
#define A_STRONG_JAW 173
#define A_REFRIGERATE 174
#define A_SWEET_VEIL 175
#define A_STANCE_CHANGE 176
#define A_GALE_WINGS 177
#define A_MEGA_LAUNCHER 178
#define A_GRASS_PELT 179
#define A_SYMBIOSIS 180
#define A_TOUGH_CLAWS 181
#define A_PIXILATE 182
#define A_GOOEY 183
#define A_AERILATE 184
#define A_PARENTAL_BOND 185
#define A_DARK_AURA 186
#define A_FAIRY_AURA 187
#define A_AURA_BREAK 188

#define A_PRIMORDIAL_SEA 189
#define A_DESOLATE_LAND 190
#define A_DELTA_STREAM 191

#define A_CLONED 192
#define A_TOTAL_GUARD 193
#define A_FISH_EYE 194
#define A_EQUALITY 195
#define A_SHINING_FIN 196
#define A_FOG_WALL 197
#define A_FEAR_GENERATOR 198
#define A_EFFICIENCY 199
#define A_OVERFLOW 200
