/*
Pokémon neo
------------------------------

file        : pokemon.h
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

#pragma once

#include <cstring>
#include <string>
#include <nds/ndstypes.h>
#include "defines.h"
#include "script.h"
#include "type.h"

class move {
  public:
    enum moveAffectsTypes : u8 {
        SELECTED              = 0,
        DEPENDS_ON_ATTACK     = 1,
        OWN_FIELD             = 2,
        RANDOM                = 4,
        BOTH_FOES             = 8,
        USER                  = 16,
        BOTH_FOES_AND_PARTNER = 32,
        OPPONENTS_FIELD       = 64
    };

    enum moveFlags : u8 {
        MAKES_CONTACT = 1,
        PROTECT       = 2,
        MAGIC_COAT    = 4,
        SNATCH        = 8,
        MIRROR_MOVE   = 16,
        KINGS_ROCK    = 32,
        SOUNDPROOF    = 64,
        WHILE_ASLEEP  = 128
    };

    enum moveHitTypes : u8 { PHYS = 0, SPEC = 1, STAT = 2 };

    bool m_isFieldAttack;

    std::string          m_moveName;
    BATTLE::battleScript m_moveEffect;
    char                 m_moveBasePower;
    type                 m_moveType;
    char                 m_moveAccuracy;
    u8                   m_movePP;
    char                 m_moveEffectAccuracy;
    moveAffectsTypes     m_moveAffectsWhom;
    char                 m_movePriority;
    moveFlags            m_moveFlags;
    moveHitTypes         m_moveHitType;

    char m_description[ 300 ] = {0};

    // Constructrs

    move( ) {
    }

    move( const std::string p_moveName, BATTLE::battleScript p_moveEffect, char p_moveBasePower,
          type p_moveType, char p_moveAccuracy, char p_movePP, char p_moveEffectAccuracy,
          moveAffectsTypes p_moveAffectsWhom, char p_movePriority, moveFlags p_moveFlags,
          moveHitTypes p_moveHitType,
          const char*  p_description = "Keine genaueren Informationen verfügbar." )
        : m_isFieldAttack( false ), m_moveName( p_moveName ), m_moveEffect( p_moveEffect ),
          m_moveBasePower( p_moveBasePower ), m_moveType( p_moveType ),
          m_moveAccuracy( p_moveAccuracy ), m_movePP( p_movePP ),
          m_moveEffectAccuracy( p_moveEffectAccuracy ), m_moveAffectsWhom( p_moveAffectsWhom ),
          m_movePriority( p_movePriority ), m_moveFlags( p_moveFlags ),
          m_moveHitType( p_moveHitType ) {
        strcpy( m_description, p_description );
    }

    ~move( ) {
    }

    virtual bool possible( ) {
        return false;
    }

    virtual void use( ) {
    }

    virtual const char* text( ) {
        return "N/A";
    }
    virtual const char* description( ) {
        return m_description;
    }
};

extern move* AttackList[ MAX_ATTACK ];

#define M_POUND 1
#define M_KARATE_CHOP 2
#define M_DOUBLE_SLAP 3
#define M_COMET_PUNCH 4
#define M_MEGA_PUNCH 5
#define M_PAY_DAY 6
#define M_FIRE_PUNCH 7
#define M_ICE_PUNCH 8
#define M_THUNDER_PUNCH 9
#define M_SCRATCH 10
#define M_VICE_GRIP 11
#define M_GUILLOTINE 12
#define M_RAZOR_WIND 13
#define M_SWORDS_DANCE 14
#define M_CUT 15
#define M_GUST 16
#define M_WING_ATTACK 17
#define M_WHIRLWIND 18
#define M_FLY 19
#define M_BIND 20
#define M_SLAM 21
#define M_VINE_WHIP 22
#define M_STOMP 23
#define M_DOUBLE_KICK 24
#define M_MEGA_KICK 25
#define M_JUMP_KICK 26
#define M_ROLLING_KICK 27
#define M_SAND_ATTACK 28
#define M_HEADBUTT 29
#define M_HORN_ATTACK 30
#define M_FURY_ATTACK 31
#define M_HORN_DRILL 32
#define M_TACKLE 33
#define M_BODY_SLAM 34
#define M_WRAP 35
#define M_TAKE_DOWN 36
#define M_THRASH 37
#define M_DOUBLE_EDGE 38
#define M_TAIL_WHIP 39
#define M_POISON_STING 40
#define M_TWINEEDLE 41
#define M_PIN_MISSILE 42
#define M_LEER 43
#define M_BITE 44
#define M_GROWL 45
#define M_ROAR 46
#define M_SING 47
#define M_SUPERSONIC 48
#define M_SONIC_BOOM 49
#define M_DISABLE 50
#define M_ACID 51
#define M_EMBER 52
#define M_FLAMETHROWER 53
#define M_MIST 54
#define M_WATER_GUN 55
#define M_HYDRO_PUMP 56
#define M_SURF 57
#define M_ICE_BEAM 58
#define M_BLIZZARD 59
#define M_PSYBEAM 60
#define M_BUBBLE_BEAM 61
#define M_AURORA_BEAM 62
#define M_HYPER_BEAM 63
#define M_PECK 64
#define M_DRILL_PECK 65
#define M_SUBMISSION 66
#define M_LOW_KICK 67
#define M_COUNTER 68
#define M_SEISMIC_TOSS 69
#define M_STRENGTH 70
#define M_ABSORB 71
#define M_MEGA_DRAIN 72
#define M_LEECH_SEED 73
#define M_GROWTH 74
#define M_RAZOR_LEAF 75
#define M_SOLAR_BEAM 76
#define M_POISON_POWDER 77
#define M_STUN_SPORE 78
#define M_SLEEP_POWDER 79
#define M_PETAL_DANCE 80
#define M_STRING_SHOT 81
#define M_DRAGON_RAGE 82
#define M_FIRE_SPIN 83
#define M_THUNDER_SHOCK 84
#define M_THUNDERBOLT 85
#define M_THUNDER_WAVE 86
#define M_THUNDER 87
#define M_ROCK_THROW 88
#define M_EARTHQUAKE 89
#define M_FISSURE 90
#define M_DIG 91
#define M_TOXIC 92
#define M_CONFUSION 93
#define M_PSYCHIC 94
#define M_HYPNOSIS 95
#define M_MEDITATE 96
#define M_AGILITY 97
#define M_QUICK_ATTACK 98
#define M_RAGE 99
#define M_TELEPORT 100
#define M_NIGHT_SHADE 101
#define M_MIMIC 102
#define M_SCREECH 103
#define M_DOUBLE_TEAM 104
#define M_RECOVER 105
#define M_HARDEN 106
#define M_MINIMIZE 107
#define M_SMOKESCREEN 108
#define M_CONFUSE_RAY 109
#define M_WITHDRAW 110
#define M_DEFENSE_CURL 111
#define M_BARRIER 112
#define M_LIGHT_SCREEN 113
#define M_HAZE 114
#define M_REFLECT 115
#define M_FOCUS_ENERGY 116
#define M_BIDE 117
#define M_METRONOME 118
#define M_MIRROR_MOVE 119
#define M_SELF_DESTRUCT 120
#define M_EGG_BOMB 121
#define M_LICK 122
#define M_SMOG 123
#define M_SLUDGE 124
#define M_BONE_CLUB 125
#define M_FIRE_BLAST 126
#define M_WATERFALL 127
#define M_CLAMP 128
#define M_SWIFT 129
#define M_SKULL_BASH 130
#define M_SPIKE_CANNON 131
#define M_CONSTRICT 132
#define M_AMNESIA 133
#define M_KINESIS 134
#define M_SOFT_BOILED 135
#define M_HIGH_JUMP_KICK 136
#define M_GLARE 137
#define M_DREAM_EATER 138
#define M_POISON_GAS 139
#define M_BARRAGE 140
#define M_LEECH_LIFE 141
#define M_LOVELY_KISS 142
#define M_SKY_ATTACK 143
#define M_TRANSFORM 144
#define M_BUBBLE 145
#define M_DIZZY_PUNCH 146
#define M_SPORE 147
#define M_FLASH 148
#define M_PSYWAVE 149
#define M_SPLASH 150
#define M_ACID_ARMOR 151
#define M_CRABHAMMER 152
#define M_EXPLOSION 153
#define M_FURY_SWIPES 154
#define M_BONEMERANG 155
#define M_REST 156
#define M_ROCK_SLIDE 157
#define M_HYPER_FANG 158
#define M_SHARPEN 159
#define M_CONVERSION 160
#define M_TRI_ATTACK 161
#define M_SUPER_FANG 162
#define M_SLASH 163
#define M_SUBSTITUTE 164
#define M_STRUGGLE 165
#define M_SKETCH 166
#define M_TRIPLE_KICK 167
#define M_THIEF 168
#define M_SPIDER_WEB 169
#define M_MIND_READER 170
#define M_NIGHTMARE 171
#define M_FLAME_WHEEL 172
#define M_SNORE 173
#define M_CURSE 174
#define M_FLAIL 175
#define M_CONVERSION_2 176
#define M_AEROBLAST 177
#define M_COTTON_SPORE 178
#define M_REVERSAL 179
#define M_SPITE 180
#define M_POWDER_SNOW 181
#define M_PROTECT 182
#define M_MACH_PUNCH 183
#define M_SCARY_FACE 184
#define M_FEINT_ATTACK 185
#define M_SWEET_KISS 186
#define M_BELLY_DRUM 187
#define M_SLUDGE_BOMB 188
#define M_MUD_SLAP 189
#define M_OCTAZOOKA 190
#define M_SPIKES 191
#define M_ZAP_CANNON 192
#define M_FORESIGHT 193
#define M_DESTINY_BOND 194
#define M_PERISH_SONG 195
#define M_ICY_WIND 196
#define M_DETECT 197
#define M_BONE_RUSH 198
#define M_LOCK_ON 199
#define M_OUTRAGE 200
#define M_SANDSTORM 201
#define M_GIGA_DRAIN 202
#define M_ENDURE 203
#define M_CHARM 204
#define M_ROLLOUT 205
#define M_FALSE_SWIPE 206
#define M_SWAGGER 207
#define M_MILK_DRINK 208
#define M_SPARK 209
#define M_FURY_CUTTER 210
#define M_STEEL_WING 211
#define M_MEAN_LOOK 212
#define M_ATTRACT 213
#define M_SLEEP_TALK 214
#define M_HEAL_BELL 215
#define M_RETURN 216
#define M_PRESENT 217
#define M_FRUSTRATION 218
#define M_SAFEGUARD 219
#define M_PAIN_SPLIT 220
#define M_SACRED_FIRE 221
#define M_MAGNITUDE 222
#define M_DYNAMIC_PUNCH 223
#define M_MEGAHORN 224
#define M_DRAGON_BREATH 225
#define M_BATON_PASS 226
#define M_ENCORE 227
#define M_PURSUIT 228
#define M_RAPID_SPIN 229
#define M_SWEET_SCENT 230
#define M_IRON_TAIL 231
#define M_METAL_CLAW 232
#define M_VITAL_THROW 233
#define M_MORNING_SUN 234
#define M_SYNTHESIS 235
#define M_MOONLIGHT 236
#define M_HIDDEN_POWER 237
#define M_CROSS_CHOP 238
#define M_TWISTER 239
#define M_RAIN_DANCE 240
#define M_SUNNY_DAY 241
#define M_CRUNCH 242
#define M_MIRROR_COAT 243
#define M_PSYCH_UP 244
#define M_EXTREME_SPEED 245
#define M_ANCIENT_POWER 246
#define M_SHADOW_BALL 247
#define M_FUTURE_SIGHT 248
#define M_ROCK_SMASH 249
#define M_WHIRLPOOL 250
#define M_BEAT_UP 251
#define M_FAKE_OUT 252
#define M_UPROAR 253
#define M_STOCKPILE 254
#define M_SPIT_UP 255
#define M_SWALLOW 256
#define M_HEAT_WAVE 257
#define M_HAIL 258
#define M_TORMENT 259
#define M_FLATTER 260
#define M_WILL_O_WISP 261
#define M_MEMENTO 262
#define M_FACADE 263
#define M_FOCUS_PUNCH 264
#define M_SMELLING_SALTS 265
#define M_FOLLOW_ME 266
#define M_NATURE_POWER 267
#define M_CHARGE 268
#define M_TAUNT 269
#define M_HELPING_HAND 270
#define M_TRICK 271
#define M_ROLE_PLAY 272
#define M_WISH 273
#define M_ASSIST 274
#define M_INGRAIN 275
#define M_SUPERPOWER 276
#define M_MAGIC_COAT 277
#define M_RECYCLE 278
#define M_REVENGE 279
#define M_BRICK_BREAK 280
#define M_YAWN 281
#define M_KNOCK_OFF 282
#define M_ENDEAVOR 283
#define M_ERUPTION 284
#define M_SKILL_SWAP 285
#define M_IMPRISON 286
#define M_REFRESH 287
#define M_GRUDGE 288
#define M_SNATCH 289
#define M_SECRET_POWER 290
#define M_DIVE 291
#define M_ARM_THRUST 292
#define M_CAMOUFLAGE 293
#define M_TAIL_GLOW 294
#define M_LUSTER_PURGE 295
#define M_MIST_BALL 296
#define M_FEATHER_DANCE 297
#define M_TEETER_DANCE 298
#define M_BLAZE_KICK 299
#define M_MUD_SPORT 300
#define M_ICE_BALL 301
#define M_NEEDLE_ARM 302
#define M_SLACK_OFF 303
#define M_HYPER_VOICE 304
#define M_POISON_FANG 305
#define M_CRUSH_CLAW 306
#define M_BLAST_BURN 307
#define M_HYDRO_CANNON 308
#define M_METEOR_MASH 309
#define M_ASTONISH 310
#define M_WEATHER_BALL 311
#define M_AROMATHERAPY 312
#define M_FAKE_TEARS 313
#define M_AIR_CUTTER 314
#define M_OVERHEAT 315
#define M_ODOR_SLEUTH 316
#define M_ROCK_TOMB 317
#define M_SILVER_WIND 318
#define M_METAL_SOUND 319
#define M_GRASS_WHISTLE 320
#define M_TICKLE 321
#define M_COSMIC_POWER 322
#define M_WATER_SPOUT 323
#define M_SIGNAL_BEAM 324
#define M_SHADOW_PUNCH 325
#define M_EXTRASENSORY 326
#define M_SKY_UPPERCUT 327
#define M_SAND_TOMB 328
#define M_SHEER_COLD 329
#define M_MUDDY_WATER 330
#define M_BULLET_SEED 331
#define M_AERIAL_ACE 332
#define M_ICICLE_SPEAR 333
#define M_IRON_DEFENSE 334
#define M_BLOCK 335
#define M_HOWL 336
#define M_DRAGON_CLAW 337
#define M_FRENZY_PLANT 338
#define M_BULK_UP 339
#define M_BOUNCE 340
#define M_MUD_SHOT 341
#define M_POISON_TAIL 342
#define M_COVET 343
#define M_VOLT_TACKLE 344
#define M_MAGICAL_LEAF 345
#define M_WATER_SPORT 346
#define M_CALM_MIND 347
#define M_LEAF_BLADE 348
#define M_DRAGON_DANCE 349
#define M_ROCK_BLAST 350
#define M_SHOCK_WAVE 351
#define M_WATER_PULSE 352
#define M_DOOM_DESIRE 353
#define M_PSYCHO_BOOST 354
#define M_ROOST 355
#define M_GRAVITY 356
#define M_MIRACLE_EYE 357
#define M_WAKE_UP_SLAP 358
#define M_HAMMER_ARM 359
#define M_GYRO_BALL 360
#define M_HEALING_WISH 361
#define M_BRINE 362
#define M_NATURAL_GIFT 363
#define M_FEINT 364
#define M_PLUCK 365
#define M_TAILWIND 366
#define M_ACUPRESSURE 367
#define M_METAL_BURST 368
#define M_U_TURN 369
#define M_CLOSE_COMBAT 370
#define M_PAYBACK 371
#define M_ASSURANCE 372
#define M_EMBARGO 373
#define M_FLING 374
#define M_PSYCHO_SHIFT 375
#define M_TRUMP_CARD 376
#define M_HEAL_BLOCK 377
#define M_WRING_OUT 378
#define M_POWER_TRICK 379
#define M_GASTRO_ACID 380
#define M_LUCKY_CHANT 381
#define M_ME_FIRST 382
#define M_COPYCAT 383
#define M_POWER_SWAP 384
#define M_GUARD_SWAP 385
#define M_PUNISHMENT 386
#define M_LAST_RESORT 387
#define M_WORRY_SEED 388
#define M_SUCKER_PUNCH 389
#define M_TOXIC_SPIKES 390
#define M_HEART_SWAP 391
#define M_AQUA_RING 392
#define M_MAGNET_RISE 393
#define M_FLARE_BLITZ 394
#define M_FORCE_PALM 395
#define M_AURA_SPHERE 396
#define M_ROCK_POLISH 397
#define M_POISON_JAB 398
#define M_DARK_PULSE 399
#define M_NIGHT_SLASH 400
#define M_AQUA_TAIL 401
#define M_SEED_BOMB 402
#define M_AIR_SLASH 403
#define M_X_SCISSOR 404
#define M_BUG_BUZZ 405
#define M_DRAGON_PULSE 406
#define M_DRAGON_RUSH 407
#define M_POWER_GEM 408
#define M_DRAIN_PUNCH 409
#define M_VACUUM_WAVE 410
#define M_FOCUS_BLAST 411
#define M_ENERGY_BALL 412
#define M_BRAVE_BIRD 413
#define M_EARTH_POWER 414
#define M_SWITCHEROO 415
#define M_GIGA_IMPACT 416
#define M_NASTY_PLOT 417
#define M_BULLET_PUNCH 418
#define M_AVALANCHE 419
#define M_ICE_SHARD 420
#define M_SHADOW_CLAW 421
#define M_THUNDER_FANG 422
#define M_ICE_FANG 423
#define M_FIRE_FANG 424
#define M_SHADOW_SNEAK 425
#define M_MUD_BOMB 426
#define M_PSYCHO_CUT 427
#define M_ZEN_HEADBUTT 428
#define M_MIRROR_SHOT 429
#define M_FLASH_CANNON 430
#define M_ROCK_CLIMB 431
#define M_DEFOG 432
#define M_TRICK_ROOM 433
#define M_DRACO_METEOR 434
#define M_DISCHARGE 435
#define M_LAVA_PLUME 436
#define M_LEAF_STORM 437
#define M_POWER_WHIP 438
#define M_ROCK_WRECKER 439
#define M_CROSS_POISON 440
#define M_GUNK_SHOT 441
#define M_IRON_HEAD 442
#define M_MAGNET_BOMB 443
#define M_STONE_EDGE 444
#define M_CAPTIVATE 445
#define M_STEALTH_ROCK 446
#define M_GRASS_KNOT 447
#define M_CHATTER 448
#define M_JUDGMENT 449
#define M_BUG_BITE 450
#define M_CHARGE_BEAM 451
#define M_WOOD_HAMMER 452
#define M_AQUA_JET 453
#define M_ATTACK_ORDER 454
#define M_DEFEND_ORDER 455
#define M_HEAL_ORDER 456
#define M_HEAD_SMASH 457
#define M_DOUBLE_HIT 458
#define M_ROAR_OF_TIME 459
#define M_SPACIAL_REND 460
#define M_LUNAR_DANCE 461
#define M_CRUSH_GRIP 462
#define M_MAGMA_STORM 463
#define M_DARK_VOID 464
#define M_SEED_FLARE 465
#define M_OMINOUS_WIND 466
#define M_SHADOW_FORCE 467
#define M_HONE_CLAWS 468
#define M_WIDE_GUARD 469
#define M_GUARD_SPLIT 470
#define M_POWER_SPLIT 471
#define M_WONDER_ROOM 472
#define M_PSYSHOCK 473
#define M_VENOSHOCK 474
#define M_AUTOTOMIZE 475
#define M_RAGE_POWDER 476
#define M_TELEKINESIS 477
#define M_MAGIC_ROOM 478
#define M_SMACK_DOWN 479
#define M_STORM_THROW 480
#define M_FLAME_BURST 481
#define M_SLUDGE_WAVE 482
#define M_QUIVER_DANCE 483
#define M_HEAVY_SLAM 484
#define M_SYNCHRONOISE 485
#define M_ELECTRO_BALL 486
#define M_SOAK 487
#define M_FLAME_CHARGE 488
#define M_COIL 489
#define M_LOW_SWEEP 490
#define M_ACID_SPRAY 491
#define M_FOUL_PLAY 492
#define M_SIMPLE_BEAM 493
#define M_ENTRAINMENT 494
#define M_AFTER_YOU 495
#define M_ROUND 496
#define M_ECHOED_VOICE 497
#define M_CHIP_AWAY 498
#define M_CLEAR_SMOG 499
#define M_STORED_POWER 500
#define M_QUICK_GUARD 501
#define M_ALLY_SWITCH 502
#define M_SCALD 503
#define M_SHELL_SMASH 504
#define M_HEAL_PULSE 505
#define M_HEX 506
#define M_SKY_DROP 507
#define M_SHIFT_GEAR 508
#define M_CIRCLE_THROW 509
#define M_INCINERATE 510
#define M_QUASH 511
#define M_ACROBATICS 512
#define M_REFLECT_TYPE 513
#define M_RETALIATE 514
#define M_FINAL_GAMBIT 515
#define M_BESTOW 516
#define M_INFERNO 517
#define M_WATER_PLEDGE 518
#define M_FIRE_PLEDGE 519
#define M_GRASS_PLEDGE 520
#define M_VOLT_SWITCH 521
#define M_STRUGGLE_BUG 522
#define M_BULLDOZE 523
#define M_FROST_BREATH 524
#define M_DRAGON_TAIL 525
#define M_WORK_UP 526
#define M_ELECTROWEB 527
#define M_WILD_CHARGE 528
#define M_DRILL_RUN 529
#define M_DUAL_CHOP 530
#define M_HEART_STAMP 531
#define M_HORN_LEECH 532
#define M_SACRED_SWORD 533
#define M_RAZOR_SHELL 534
#define M_HEAT_CRASH 535
#define M_LEAF_TORNADO 536
#define M_STEAMROLLER 537
#define M_COTTON_GUARD 538
#define M_NIGHT_DAZE 539
#define M_PSYSTRIKE 540
#define M_TAIL_SLAP 541
#define M_HURRICANE 542
#define M_HEAD_CHARGE 543
#define M_GEAR_GRIND 544
#define M_SEARING_SHOT 545
#define M_TECHNO_BLAST 546
#define M_RELIC_SONG 547
#define M_SECRET_SWORD 548
#define M_GLACIATE 549
#define M_BOLT_STRIKE 550
#define M_BLUE_FLARE 551
#define M_FIERY_DANCE 552
#define M_FREEZE_SHOCK 553
#define M_ICE_BURN 554
#define M_SNARL 555
#define M_ICICLE_CRASH 556
#define M_V_CREATE 557
#define M_FUSION_FLARE 558
#define M_FUSION_BOLT 559
#define M_FLYING_PRESS 560
#define M_MAT_BLOCK 561
#define M_BELCH 562
#define M_ROTOTILLER 563
#define M_STICKY_WEB 564
#define M_FELL_STINGER 565
#define M_PHANTOM_FORCE 566
#define M_TRICK_OR_TREAT 567
#define M_NOBLE_ROAR 568
#define M_ION_DELUGE 569
#define M_PARABOLIC_CHARGE 570
#define M_FORESTS_CURSE 571
#define M_PETAL_BLIZZARD 572
#define M_FREEZE_DRY 573
#define M_DISARMING_VOICE 574
#define M_PARTING_SHOT 575
#define M_TOPSY_TURVY 576
#define M_DRAINING_KISS 577
#define M_CRAFTY_SHIELD 578
#define M_FLOWER_SHIELD 579
#define M_GRASSY_TERRAIN 580
#define M_MISTY_TERRAIN 581
#define M_ELECTRIFY 582
#define M_PLAY_ROUGH 583
#define M_FAIRY_WIND 584
#define M_MOONBLAST 585
#define M_BOOMBURST 586
#define M_FAIRY_LOCK 587
#define M_KINGS_SHIELD 588
#define M_PLAY_NICE 589
#define M_CONFIDE 590
#define M_DIAMOND_STORM 591
#define M_STEAM_ERUPTION 592
#define M_HYPERSPACE_HOLE 593
#define M_WATER_SHURIKEN 594
#define M_MYSTICAL_FIRE 595
#define M_SPIKY_SHIELD 596
#define M_AROMATIC_MIST 597
#define M_EERIE_IMPULSE 598
#define M_VENOM_DRENCH 599
#define M_POWDER 600
#define M_GEOMANCY 601
#define M_MAGNETIC_FLUX 602
#define M_HAPPY_HOUR 603
#define M_ELECTRIC_TERRAIN 604
#define M_DAZZLING_GLEAM 605
#define M_CELEBRATE 606
#define M_HOLD_HANDS 607
#define M_BABY_DOLL_EYES 608
#define M_NUZZLE 609
#define M_HOLD_BACK 610
#define M_INFESTATION 611
#define M_POWER_UP_PUNCH 612
#define M_OBLIVION_WING 613
#define M_THOUSAND_ARROWS 614
#define M_THOUSAND_WAVES 615
#define M_LANDS_WRATH 616
#define M_LIGHT_OF_RUIN 617
#define M_ORIGIN_PULSE 618
#define M_PRECIPICE_BLADES 619
#define M_DRAGON_ASCENT 620
#define M_HYPERSPACE_FURY 621
