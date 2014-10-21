/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : ability.h
    author      : Philip Wellnitz (RedArceus)
    description : Abilitíes

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
#pragma once

#include <string>

class ability {
public:
    enum abilityType {
        ATTACK = 1,
        BEFORE_BATTLE = 2,
        AFTER_BATTLE = 4,
        GRASS = 8,
        BEFORE_ATTACK = 16,
        AFTER_ATTACK = 32
    };
    std::string         m_abilityName;
    std::string         m_flavourText;
    abilityType         m_type;

    ///Constructors
    ability( ) { }

    ability( int p_abilityId );

    void run( ... );
};
std::wstring getWAbilityName( int p_abilityId );

#define SOUNDPROOF 0
#define STENCH 1
#define DRIZZLE 2
#define SPEED_BOOST 3
#define BATTLE_ARMOR 4
#define STURDY 5
#define DAMP 6
#define LIMBER 7
#define SAND_VEIL 8
#define STATIC 9
#define VOLT_ABSORB 10
#define WATER_ABSORB 11
#define OBLIVIOUS 12
#define CLOUD_NINE 13
#define COMPOUND_EYES 14
#define INSOMNIA 15
#define COLOR_CHANGE 16
#define IMMUNITY 17
#define FLASH_FIRE 18
#define SHIELD_DUST 19
#define OWN_TEMPO 20
#define SUCTION_CUPS 21
#define INTIMIDATE 22
#define SHADOW_TAG 23
#define ROUGH_SKIN 24
#define WONDER_GUARD 25
#define LEVITATE 26
#define EFFECT_SPORE 27
#define SYNCHRONIZE 28
#define CLEAR_BODY 29
#define NATURAL_CURE 30
#define LIGHTNING_ROD 31
#define SERENE_GRACE 32
#define SWIFT_SWIM 33
#define CHLOROPHYLL 34
#define ILLUMINATE 35
#define TRACE 36
#define HUGE_POWER 37
#define POISON_POINT 38
#define INNER_FOCUS 39
#define MAGMA_ARMOR 40
#define WATER_VEIL 41
#define MAGNET_PULL 42
#define SOUNDPROOF 43
#define RAIN_DISH 44
#define SAND_STREAM 45
#define PRESSURE 46
#define THICK_FAT 47
#define EARLY_BIRD 48
#define FLAME_BODY 49
#define RUN_AWAY 50
#define KEEN_EYE 51
#define HYPER_CUTTER 52
#define PICKUP 53
#define TRUANT 54
#define HUSTLE 55
#define CUTE_CHARM 56
#define PLUS 57
#define MINUS 58
#define FORECAST 59
#define STICKY 60
#define SHED_SKIN 61
#define GUTS 62
#define MARVEL_SCALE 63
#define LIQUID_OOZE 64
#define OVERGROW 65
#define BLAZE 66
#define TORRENT 67
#define SWARM 68
#define ROCK_HEAD 69
#define DROUGHT 70
#define ARENA_TRAP 71
#define VITAL_SPIRIT 72
#define WHITE_SMOKE 73
#define PURE_POWER 74
#define SHELL_ARMOR 75
#define AIR_LOCK 76
#define TANGLED_FEET 77
#define MOTOR_DRIVE 78
#define RIVALRY 79
#define STEADFAST 80
#define SNOW_CLOAK 81
#define GLUTTONY 82
#define ANGER_POINT 83
#define UBURDEN 84
#define HEATPROOF 85
#define SIMPLE 86
#define DRY_SKIN 87
#define DOWNLOAD 88
#define IRON_FIST 89
#define POISON 90
#define ADAPTABILITY 91
#define SKILL_LINK 92
#define HYDRATION 93
#define SOLAR_POWER 94
#define QUICK_FEET 95
#define NORMALIZE 96
#define SNIPER_POWERS 97
#define MAGIC_GUARD 98
#define NO_GUARD 99
#define STALL 100
#define TECHNICIAN 101
#define LEAF_GUARD 102
#define KLUTZ 103
#define MOLD_BREAKER 104
#define SUPER_LUCK 105
#define AFTERMATH 106
#define ANTICIPATION 107
#define FOREWARN 108
#define UNAWARE 109
#define TINTED_LENS 110
#define FILTER 111
#define SLOW_START 112
#define SCRAPPY 113
#define STORM_DRAIN 114
#define ICE_BODY 115
#define SOLID_ROCK 116
#define SNOW_WARNING 117
#define HONEY_GATHER 118
#define FRISK 119
#define RECKLESS 120
#define MULTITYPE 121
#define FLOWER_GIFT 122
#define BAD_DREAMS 123
#define PICKPOCKET 124
#define SHEER_FORCE 125
#define CONTRARY 126
#define UNNERVE 127
#define DEFIANT 128
#define DEFEATIST 129
#define CURSED_BODY 130
#define HEALER 131
#define FRIEND_GUARD 132
#define WEAK_ARMOR 133
#define HEAVY_METAL 134
#define LIGHT_METAL 135
#define MULTISCALE 136
#define TOXIC_BOOST 137
#define FLARE_BOOST 138
#define HARVEST 139
#define TELEPATHY 140
#define MOODY 141
#define OVERCOAT 142
#define POISON_TOUCH 143
#define REGENERATOR 144
#define BIG_PECKS 145
#define SAND_RUSH 146
#define WONDER_SKIN 147
#define ANALYTIC 148
#define ILLUSION 149
#define IMPOSTER 150
#define INFILTRATOR 151
#define MUMMY 152
#define MOXIE 153
#define JUSTIFIED 154
#define RATTLED 155
#define MAGIC_BOUNCE 156
#define SAP_SIPPER 157
#define PRANKSTER 158
#define SAND_FORCE 159
#define IRON_BARBS 160
#define ZEN_MODE 161
#define VICTORY_STAR 162
#define TURBOBLAZE 163
#define TERAVOLT 164
#define AROMA_VEIL 165
#define FLOWER_VEIL 166
#define CHEEK_POUCH 167
#define PROTEAN 168
#define FUR_COAT 169
#define MAGICIAN 170
#define BULLETPROOF 171
#define COMPETITIV 172
#define STRONG_JAW 173
#define REFRIGERATE 174
#define SWEET_VEIL 175
#define STANCE_CHANGE 176
#define GALE_WINGS 177
#define MEGA_LAUNCHER 178
#define GRASS_PELT 179
#define SYMBIOSIS 180
#define TOUGH_CLAWS 181
#define PIXILATE 182
#define GOOEY 183
#define AERILATE 184
#define PARENTAL_BOND 185
#define DARK_AURA 186
#define FAIRY_AURA 187
#define AURA_BREAK 188
#define CLONED 189
#define TOTAL_GUARD 190
#define FISH_EYE 191
#define EQUALITY 192
#define SHINING_FIN 193
#define FOG_WALL 194
#define FEAR_GENERATOR 195
#define EFFICIENCY 196
#define OVERFLOW 197
