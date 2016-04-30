/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : item.h
    author      : Philip Wellnitz
    description : Header file. Consult the corresponding source file for details.

    Copyright (C) 2012 - 2016
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
#include <map>
#include <nds/ndstypes.h>

#include "ability.h"
#include "script.h"
extern const char ITEM_PATH[ ];


class item {
public:
    enum itemEffectType {
        NONE = 0,
        IN_BATTLE = 1, //Medicine/Berries
        HOLD = 2, //Has a hold effect only
        OUT_OF_BATTLE = 4, //Repel, etc
        USE_ON_PKMN = 8   //Evolutionaries, 
    };
    enum itemType {
        GOODS,
        KEY_ITEM,
        TM_HM,
        MAILS,
        MEDICINE,
        BERRIES,
        POKE_BALLS,
        BATTLE_ITEM
    };

    struct itemData {
        itemEffectType  m_itemEffectType;
        u32             m_price;
        u32             m_itemEffect;

        char            m_itemDisplayName[ 15 ];
        char            m_itemDescription[ 200 ];
        char            m_itemShortDescr[ 100 ];
    } m_itemData;
    bool            m_loaded; //Specifies whether the item data has been loaded

    itemType        m_itemType;

    std::string     m_itemName;

    ability::abilityType
        m_inBattleEffect;
    BATTLE::battleScript
        m_inBattleScript;


    //Functions
    std::string     getDisplayName( bool p_new = false );

    std::string     getDescription( );

    std::string     getShortDescription( );

    u32             getEffect( );


    itemEffectType  getEffectType( );

    u32             getPrice( );

    u16             getItemId( );

    virtual bool    load( );

    bool            needsInformation( u8 p_num );

    bool            use( pokemon& p_pokemon );

    bool            use( bool p_dryRun = false );

    bool            useable( );

    //Constructors

    item( const std::string& p_itemName )
        : m_itemName( p_itemName ) {
        m_loaded = false;
    }

    item( )
        : m_itemName( "Null" ) { /*load = false;*/
    }

};

class ball
    : public item {
public:
    ball( const std::string& p_name )
        : item( p_name ) {
        m_itemType = POKE_BALLS;
    }
};

class medicine
    : public item {
public:
    medicine( const std::string& p_name )
        : item( p_name ) {
        m_itemType = MEDICINE;
    }
};

class TM
    : public item {
public:
    u16 m_moveIdx;
    TM( const std::string& p_name, u16 p_moveIdx )
        : item( p_name ) {
        m_itemType = TM_HM;
        m_moveIdx = p_moveIdx;
    }
};

class battleItem
    : public item {
public:
    battleItem( const std::string& p_name )
        : item( p_name ) {
        m_itemType = BATTLE_ITEM;
    }
};

class keyItem
    : public item {
public:
    keyItem( const std::string& p_name )
        : item( p_name ) {
        m_itemType = KEY_ITEM;
    }
};

class mail
    : public item {
public:
    mail( const std::string& p_name )
        : item( p_name ) {
        m_itemType = MAILS;
    }
};

#define MAX_ITEMS 772
extern item* ItemList[ MAX_ITEMS ];


#define I_NONE 0
#define I_MASTER_BALL 1
#define I_ULTRA_BALL 2
#define I_GREAT_BALL 3
#define I_POKE_BALL 4
#define I_SAFARI_BALL 5
#define I_NET_BALL 6
#define I_DIVE_BALL 7
#define I_NEST_BALL 8
#define I_REPEAT_BALL 9
#define I_TIMER_BALL 10
#define I_LUXURY_BALL 11
#define I_PREMIER_BALL 12
#define I_DUSK_BALL 13
#define I_HEAL_BALL 14
#define I_QUICK_BALL 15
#define I_CHERISH_BALL 16
#define I_POTION 17
#define I_ANTIDOTE 18
#define I_BURN_HEAL 19
#define I_ICE_HEAL 20
#define I_AWAKENING 21
#define I_PARALYZE_HEAL 22
#define I_FULL_RESTORE 23
#define I_MAX_POTION 24
#define I_HYPER_POTION 25
#define I_SUPER_POTION 26
#define I_FULL_HEAL 27
#define I_REVIVE 28
#define I_MAX_REVIVE 29
#define I_FRESH_WATER 30
#define I_SODA_POP 31
#define I_LEMONADE 32
#define I_MOOMOO_MILK 33
#define I_ENERGY_POWDER 34
#define I_ENERGY_ROOT 35
#define I_HEAL_POWDER 36
#define I_REVIVAL_HERB 37
#define I_ETHER 38
#define I_MAX_ETHER 39
#define I_ELIXIR 40
#define I_MAX_ELIXIR 41
#define I_LAVA_COOKIE 42
#define I_BERRY_JUICE 43
#define I_SACRED_ASH 44
#define I_HP_UP 45
#define I_PROTEIN 46
#define I_IRON 47
#define I_CARBOS 48
#define I_CALCIUM 49
#define I_RARE_CANDY 50
#define I_PP_UP 51
#define I_ZINC 52
#define I_PP_MAX 53
#define I_OLD_GATEAU 54
#define I_GUARD_SPEC 55
#define I_DIRE_HIT 56
#define I_X_ATTACK 57
#define I_X_DEFENSE 58
#define I_X_SPEED 59
#define I_X_ACCURACY 60
#define I_X_SP_ATK 61
#define I_X_SP_DEF 62
#define I_POKE_DOLL 63
#define I_FLUFFY_TAIL 64
#define I_BLUE_FLUTE 65
#define I_YELLOW_FLUTE 66
#define I_RED_FLUTE 67
#define I_BLACK_FLUTE 68
#define I_WHITE_FLUTE 69
#define I_SHOAL_SALT 70
#define I_SHOAL_SHELL 71
#define I_RED_SHARD 72
#define I_BLUE_SHARD 73
#define I_YELLOW_SHARD 74
#define I_GREEN_SHARD 75
#define I_SUPER_REPEL 76
#define I_MAX_REPEL 77
#define I_ESCAPE_ROPE 78
#define I_REPEL 79
#define I_SUN_STONE 80
#define I_MOON_STONE 81
#define I_FIRE_STONE 82
#define I_THUNDER_STONE 83
#define I_WATER_STONE 84
#define I_LEAF_STONE 85
#define I_TINY_MUSHROOM 86
#define I_BIG_MUSHROOM 87
#define I_PEARL 88
#define I_BIG_PEARL 89
#define I_STARDUST 90
#define I_STAR_PIECE 91
#define I_NUGGET 92
#define I_HEART_SCALE 93
#define I_HONEY 94
#define I_GROWTH_MULCH 95
#define I_DAMP_MULCH 96
#define I_STABLE_MULCH 97
#define I_GOOEY_MULCH 98
#define I_ROOT_FOSSIL 99
#define I_CLAW_FOSSIL 100
#define I_HELIX_FOSSIL 101
#define I_DOME_FOSSIL 102
#define I_OLD_AMBER 103
#define I_ARMOR_FOSSIL 104
#define I_SKULL_FOSSIL 105
#define I_RARE_BONE 106
#define I_SHINY_STONE 107
#define I_DUSK_STONE 108
#define I_DAWN_STONE 109
#define I_OVAL_STONE 110
#define I_ODD_KEYSTONE 111
#define I_GRISEOUS_ORB 112

#define I_DOUSE_DRIVE 116
#define I_SHOCK_DRIVE 117
#define I_BURN_DRIVE 118
#define I_CHILL_DRIVE 119

#define I_SWEET_HEART 134
#define I_ADAMANT_ORB 135
#define I_LUSTROUS_ORB 136
#define I_GREET_MAIL 137
#define I_FAVORED_MAIL 138
#define I_RSVP_MAIL 139
#define I_THANKS_MAIL 140
#define I_INQUIRY_MAIL 141
#define I_LIKE_MAIL 142
#define I_REPLY_MAIL 143
#define I_BRIDGE_MAIL_S 144
#define I_BRIDGE_MAIL_D 145
#define I_BRIDGE_MAIL_T 146
#define I_BRIDGE_MAIL_V 147
#define I_BRIDGE_MAIL_M 148
#define I_CHERI_BERRY 149
#define I_CHESTO_BERRY 150
#define I_PECHA_BERRY 151
#define I_RAWST_BERRY 152
#define I_ASPEAR_BERRY 153
#define I_LEPPA_BERRY 154
#define I_ORAN_BERRY 155
#define I_PERSIM_BERRY 156
#define I_LUM_BERRY 157
#define I_SITRUS_BERRY 158
#define I_FIGY_BERRY 159
#define I_WIKI_BERRY 160
#define I_MAGO_BERRY 161
#define I_AGUAV_BERRY 162
#define I_IAPAPA_BERRY 163
#define I_RAZZ_BERRY 164
#define I_BLUK_BERRY 165
#define I_NANAB_BERRY 166
#define I_WEPEAR_BERRY 167
#define I_PINAP_BERRY 168
#define I_POMEG_BERRY 169
#define I_KELPSY_BERRY 170
#define I_QUALOT_BERRY 171
#define I_HONDEW_BERRY 172
#define I_GREPA_BERRY 173
#define I_TAMATO_BERRY 174
#define I_CORNN_BERRY 175
#define I_MAGOST_BERRY 176
#define I_RABUTA_BERRY 177
#define I_NOMEL_BERRY 178
#define I_SPELON_BERRY 179
#define I_PAMTRE_BERRY 180
#define I_WATMEL_BERRY 181
#define I_DURIN_BERRY 182
#define I_BELUE_BERRY 183
#define I_OCCA_BERRY 184
#define I_PASSHO_BERRY 185
#define I_WACAN_BERRY 186
#define I_RINDO_BERRY 187
#define I_YACHE_BERRY 188
#define I_CHOPLE_BERRY 189
#define I_KEBIA_BERRY 190
#define I_SHUCA_BERRY 191
#define I_COBA_BERRY 192
#define I_PAYAPA_BERRY 193
#define I_TANGA_BERRY 194
#define I_CHARTI_BERRY 195
#define I_KASIB_BERRY 196
#define I_HABAN_BERRY 197
#define I_COLBUR_BERRY 198
#define I_BABIRI_BERRY 199
#define I_CHILAN_BERRY 200
#define I_LIECHI_BERRY 201
#define I_GANLON_BERRY 202
#define I_SALAC_BERRY 203
#define I_PETAYA_BERRY 204
#define I_APICOT_BERRY 205
#define I_LANSAT_BERRY 206
#define I_STARF_BERRY 207
#define I_ENIGMA_BERRY 208
#define I_MICLE_BERRY 209
#define I_CUSTAP_BERRY 210
#define I_JABOCA_BERRY 211
#define I_ROWAP_BERRY 212
#define I_BRIGHT_POWDER 213
#define I_WHITE_HERB 214
#define I_MACHO_BRACE 215
#define I_EXP_SHARE 216
#define I_QUICK_CLAW 217
#define I_SOOTHE_BELL 218
#define I_MENTAL_HERB 219
#define I_CHOICE_BAND 220
#define I_KINGS_ROCK 221
#define I_SILVER_POWDER 222
#define I_AMULET_COIN 223
#define I_CLEANSE_TAG 224
#define I_SOUL_DEW 225
#define I_DEEP_SEA_TOOTH 226
#define I_DEEP_SEA_SCALE 227
#define I_SMOKE_BALL 228
#define I_EVERSTONE 229
#define I_FOCUS_BAND 230
#define I_LUCKY_EGG 231
#define I_SCOPE_LENS 232
#define I_METAL_COAT 233
#define I_LEFTOVERS 234
#define I_DRAGON_SCALE 235
#define I_LIGHT_BALL 236
#define I_SOFT_SAND 237
#define I_HARD_STONE 238
#define I_MIRACLE_SEED 239
#define I_BLACK_GLASSES 240
#define I_BLACK_BELT 241
#define I_MAGNET 242
#define I_MYSTIC_WATER 243
#define I_SHARP_BEAK 244
#define I_POISON_BARB 245
#define I_NEVER_MELT_ICE 246
#define I_SPELL_TAG 247
#define I_TWISTED_SPOON 248
#define I_CHARCOAL 249
#define I_DRAGON_FANG 250
#define I_SILK_SCARF 251
#define I_UP_GRADE 252
#define I_SHELL_BELL 253
#define I_SEA_INCENSE 254
#define I_LAX_INCENSE 255
#define I_LUCKY_PUNCH 256
#define I_METAL_POWDER 257
#define I_THICK_CLUB 258
#define I_STICK 259
#define I_RED_SCARF 260
#define I_BLUE_SCARF 261
#define I_PINK_SCARF 262
#define I_GREEN_SCARF 263
#define I_YELLOW_SCARF 264
#define I_WIDE_LENS 265
#define I_MUSCLE_BAND 266
#define I_WISE_GLASSES 267
#define I_EXPERT_BELT 268
#define I_LIGHT_CLAY 269
#define I_LIFE_ORB 270
#define I_POWER_HERB 271
#define I_TOXIC_ORB 272
#define I_FLAME_ORB 273
#define I_QUICK_POWDER 274
#define I_FOCUS_SASH 275
#define I_ZOOM_LENS 276
#define I_METRONOME 277
#define I_IRON_BALL 278
#define I_LAGGING_TAIL 279
#define I_DESTINY_KNOT 280
#define I_BLACK_SLUDGE 281
#define I_ICY_ROCK 282
#define I_SMOOTH_ROCK 283
#define I_HEAT_ROCK 284
#define I_DAMP_ROCK 285
#define I_GRIP_CLAW 286
#define I_CHOICE_SCARF 287
#define I_STICKY_BARB 288
#define I_POWER_BRACER 289
#define I_POWER_BELT 290
#define I_POWER_LENS 291
#define I_POWER_BAND 292
#define I_POWER_ANKLET 293
#define I_POWER_WEIGHT 294
#define I_SHED_SHELL 295
#define I_BIG_ROOT 296
#define I_CHOICE_SPECS 297
#define I_FLAME_PLATE 298
#define I_SPLASH_PLATE 299
#define I_ZAP_PLATE 300
#define I_MEADOW_PLATE 301
#define I_ICICLE_PLATE 302
#define I_FIST_PLATE 303
#define I_TOXIC_PLATE 304
#define I_EARTH_PLATE 305
#define I_SKY_PLATE 306
#define I_MIND_PLATE 307
#define I_INSECT_PLATE 308
#define I_STONE_PLATE 309
#define I_SPOOKY_PLATE 310
#define I_DRACO_PLATE 311
#define I_DREAD_PLATE 312
#define I_IRON_PLATE 313
#define I_ODD_INCENSE 314
#define I_ROCK_INCENSE 315
#define I_FULL_INCENSE 316
#define I_WAVE_INCENSE 317
#define I_ROSE_INCENSE 318
#define I_LUCK_INCENSE 319
#define I_PURE_INCENSE 320
#define I_PROTECTOR 321
#define I_ELECTIRIZER 322
#define I_MAGMARIZER 323
#define I_DUBIOUS_DISC 324
#define I_REAPER_CLOTH 325
#define I_RAZOR_CLAW 326
#define I_RAZOR_FANG 327
#define I_TM01 328
#define I_TM02 329
#define I_TM03 330
#define I_TM04 331
#define I_TM05 332
#define I_TM06 333
#define I_TM07 334
#define I_TM08 335
#define I_TM09 336
#define I_TM10 337
#define I_TM11 338
#define I_TM12 339
#define I_TM13 340
#define I_TM14 341
#define I_TM15 342
#define I_TM16 343
#define I_TM17 344
#define I_TM18 345
#define I_TM19 346
#define I_TM20 347
#define I_TM21 348
#define I_TM22 349
#define I_TM23 350
#define I_TM24 351
#define I_TM25 352
#define I_TM26 353
#define I_TM27 354
#define I_TM28 355
#define I_TM29 356
#define I_TM30 357
#define I_TM31 358
#define I_TM32 359
#define I_TM33 360
#define I_TM34 361
#define I_TM35 362
#define I_TM36 363
#define I_TM37 364
#define I_TM38 365
#define I_TM39 366
#define I_TM40 367
#define I_TM41 368
#define I_TM42 369
#define I_TM43 370
#define I_TM44 371
#define I_TM45 372
#define I_TM46 373
#define I_TM47 374
#define I_TM48 375
#define I_TM49 376
#define I_TM50 377
#define I_TM51 378
#define I_TM52 379
#define I_TM53 380
#define I_TM54 381
#define I_TM55 382
#define I_TM56 383
#define I_TM57 384
#define I_TM58 385
#define I_TM59 386
#define I_TM60 387
#define I_TM61 388
#define I_TM62 389
#define I_TM63 390
#define I_TM64 391
#define I_TM65 392
#define I_TM66 393
#define I_TM67 394
#define I_TM68 395
#define I_TM69 396
#define I_TM70 397
#define I_TM71 398
#define I_TM72 399
#define I_TM73 400
#define I_TM74 401
#define I_TM75 402
#define I_TM76 403
#define I_TM77 404
#define I_TM78 405
#define I_TM79 406
#define I_TM80 407
#define I_TM81 408
#define I_TM82 409
#define I_TM83 410
#define I_TM84 411
#define I_TM85 412
#define I_TM86 413
#define I_TM87 414
#define I_TM88 415
#define I_TM89 416
#define I_TM90 417
#define I_TM91 418
#define I_TM92 419
#define I_HM01 420
#define I_HM02 421
#define I_HM03 422
#define I_HM04 423
#define I_HM05 424
#define I_HM06 425
#define I_HM07 426
#define I_HM08 427
#define I_EXPLORER_KIT 428
#define I_LOOT_SACK 429
#define I_RULE_BOOK 430
#define I_POKE_RADAR 431
#define I_POINT_CARD 432
#define I_JOURNAL 433
#define I_SEAL_CASE 434
#define I_FASHION_CASE 435
#define I_SEAL_BAG 436
#define I_PAL_PAD 437
#define I_WORKS_KEY 438
#define I_OLD_CHARM 439
#define I_GALACTIC_KEY 440
#define I_RED_CHAIN 441
#define I_TOWN_MAP 442
#define I_VS_SEEKER 443
#define I_COIN_CASE 444
#define I_OLD_ROD 445
#define I_GOOD_ROD 446
#define I_SUPER_ROD 447
#define I_SPRAYDUCK 448
#define I_POFFIN_CASE 449
#define I_BICYCLE 450
#define I_SUITE_KEY 451
#define I_OAKS_LETTER 452
#define I_LUNAR_WING 453
#define I_MEMBER_CARD 454
#define I_AZURE_FLUTE 455
#define I_SS_TICKET 456
#define I_CONTEST_PASS 457
#define I_MAGMA_STONE 458
#define I_PARCEL 459
#define I_COUPON_1 460
#define I_COUPON_2 461
#define I_COUPON_3 462
#define I_SECRET_POTION 464
#define I_VS_RECORDER 465
#define I_GRACIDEA 466
#define I_SECRET_KEY 467
#define I_APRICORN_BOX 468
#define I_UNOWN_REPORT 469
#define I_BERRY_POTS 470
#define I_DOWSING_MACHINE 471
#define I_BLUE_CARD 472
#define I_SLOWPOKE_TAIL 473
#define I_CLEAR_BELL 474
#define I_CARD_KEY 475
#define I_BASEMENT_KEY 476
#define I_SQUIRT_BOTTLE 477
#define I_RED_SCALE 478
#define I_LOST_ITEM 479
#define I_PASS 480
#define I_MACHINE_PART 481
#define I_SILVER_WING 482
#define I_RAINBOW_WING 483
#define I_MYSTERY_EGG 484

#define I_AURORA_TICKET 486
#define I_OLD_SEAMAP 487
#define I_HM09 488
#define I_HM10 489
#define I_HM11 490
#define I_HM12 491

#define I_FAST_BALL 492
#define I_LEVEL_BALL 493
#define I_LURE_BALL 494
#define I_HEAVY_BALL 495
#define I_LOVE_BALL 496
#define I_FRIEND_BALL 497
#define I_MOON_BALL 498
#define I_SPORT_BALL 499
#define I_PARK_BALL 500
#define I_PHOTO_ALBUM 501
#define I_GB_SOUNDS 502
#define I_TIDAL_BELL 503
#define I_RAGE_CANDY_BAR 504

#define I_PUMKIN_BERRY 505
#define I_DRASH_BERRY 506
#define I_EGGANT_BERRY 507
#define I_STRIB_BERRY 508
#define I_CHILIAN_BERRY 509
#define I_NUTPEA_BERRY 510
#define I_GINEMA_BERRY 511
#define I_KUO_BERRY 512
#define I_YAGO_BERRY 513
#define I_TOUGA_BERRY 514
#define I_NINIKU_BERRY 515
#define I_TOPO_BERRY 516

#define I_SPHERE_SEGMENT 531

#define I_JADE_ORB 532
#define I_LOCK_CAPSULE 533
#define I_RED_ORB 534
#define I_BLUE_ORB 535
#define I_ENIGMA_STONE 536
#define I_PRISM_SCALE 537
#define I_EVIOLITE 538
#define I_FLOAT_STONE 539
#define I_ROCKY_HELMET 540
#define I_AIR_BALLOON 541
#define I_RED_CARD 542
#define I_RING_TARGET 543
#define I_BINDING_BAND 544
#define I_ABSORB_BULB 545
#define I_CELL_BATTERY 546
#define I_EJECT_BUTTON 547
#define I_FIRE_GEM 548
#define I_WATER_GEM 549
#define I_ELECTRIC_GEM 550
#define I_GRASS_GEM 551
#define I_ICE_GEM 552
#define I_FIGHTING_GEM 553
#define I_POISON_GEM 554
#define I_GROUND_GEM 555
#define I_FLYING_GEM 556
#define I_PSYCHIC_GEM 557
#define I_BUG_GEM 558
#define I_ROCK_GEM 559
#define I_GHOST_GEM 560
#define I_DRAGON_GEM 561
#define I_DARK_GEM 562
#define I_STEEL_GEM 563
#define I_NORMAL_GEM 564
#define I_HEALTH_WING 565
#define I_MUSCLE_WING 566
#define I_RESIST_WING 567
#define I_GENIUS_WING 568
#define I_CLEVER_WING 569
#define I_SWIFT_WING 570
#define I_PRETTY_WING 571
#define I_COVER_FOSSIL 572
#define I_PLUME_FOSSIL 573
#define I_LIBERTY_PASS 574
#define I_PASS_ORB 575
#define I_DREAM_BALL 576
#define I_POKE_TOY 577
#define I_PROP_CASE 578
#define I_DRAGON_SKULL 579
#define I_BALM_MUSHROOM 580
#define I_BIG_NUGGET 581
#define I_PEARL_STRING 582
#define I_COMET_SHARD 583
#define I_RELIC_COPPER 584
#define I_RELIC_SILVER 585
#define I_RELIC_GOLD 586
#define I_RELIC_VASE 587
#define I_RELIC_BAND 588
#define I_RELIC_STATUE 589
#define I_RELIC_CROWN 590
#define I_CASTELIACONE 591

#define I_LIGHT_STONE 616
#define I_DARK_STONE 617
#define I_TM93 618
#define I_TM94 619
#define I_TM95 620
#define I_XTRANSCEIVER 621
#define I_GOD_STONE 622
#define I_GRAM_1 623
#define I_GRAM_2 624
#define I_GRAM_3 625
#define I_XTRANSCEIVER2 626
#define I_MEDAL_BOX 627
#define I_DNA_SPLICERS 628
#define I_DNA_SPLICERS2 629
#define I_PERMIT 630
#define I_OVAL_CHARM 631
#define I_SHINY_CHARM 632
#define I_PLASMA_CARD 633
#define I_GRUBBY_HANKY 634
#define I_COLRESS_MACHINE 635
#define I_DROPPED_ITEM 636
#define I_DROPPED_ITEM2 637
#define I_REVEAL_GLASS 638

#define I_WEAKNESS_POLICY 639
#define I_ASSAULT_VEST 640
#define I_HOLO_CASTER 641
#define I_PROFS_LETTER 642
#define I_ROLLER_SKATES 643
#define I_PIXIE_PLATE 644
#define I_ABILITY_CAPSULE 645
#define I_WHIPPED_DREAM 646
#define I_SACHET 647
#define I_LUMINOUS_MOSS 648
#define I_SNOWBALL 649
#define I_SAFETY_GOGGLES 650
#define I_POKE_FLUTE 651
#define I_RICH_MULCH 652
#define I_SURPRISE_MULCH 653
#define I_BOOST_MULCH 654
#define I_AMAZE_MULCH 655
#define I_GENGARITE 656
#define I_GARDEVOIRITE 657
#define I_AMPHAROSITE 658
#define I_VENUSAURITE 659
#define I_CHARIZARDITE_X 660
#define I_BLASTOISINITE 661
#define I_MEWTWONITE_X 662
#define I_MEWTWONITE_Y 663
#define I_BLAZIKENITE 664
#define I_MEDICHAMITE 665
#define I_HOUNDOOMINITE 666
#define I_AGGRONITE 667
#define I_BANETTITE 668
#define I_TYRANITARITE 669
#define I_SCIZORITE 670
#define I_PINSIRITE 671
#define I_AERODACTYLITE 672
#define I_LUCARIONITE 673
#define I_ABOMASITE 674
#define I_KANGASKHANITE 675
#define I_GYARADOSITE 676
#define I_ABSOLITE 677
#define I_CHARIZARDITE_Y 678
#define I_ALAKAZITE 679
#define I_HERACRONITE 680
#define I_MAWILITE 681
#define I_MANECTITE 682
#define I_GARCHOMPITE 683
#define I_LATIASITE 684
#define I_LATIOSITE 685
#define I_ROSELI_BERRY 686
#define I_KEE_BERRY 687
#define I_MARANGA_BERRY 688
#define I_SPRINKLOTAD 689
#define I_TM96 690
#define I_TM97 691
#define I_TM98 692
#define I_TM99 693
#define I_TM100 694

#define I_POWER_PLANT_PASS 695
#define I_MEGA_RING 696
#define I_INTRIGUING_STONE 697
#define I_COMMON_STONE 698
#define I_DISCOUNT_COUPON 699
#define I_ELEVATOR_KEY 700
#define I_TMV_PASS 701
#define I_HONOR_OF_KALOS 702
#define I_ADVENTURE_RULES 703
#define I_STRANGE_SOUVENIR 704
#define I_LENS_CASE 705
#define I_TRAVEL_TRUNK 706
#define I_TRAVEL_TRUNK2 707
#define I_LUMIOSE_GALETTE 708

#define I_SHALOUR_SABLE 709
#define I_JAW_FOSSIL 710
#define I_SAIL_FOSSIL 711
#define I_LOOKER_TICKET 712
#define I_BIKE2 713
#define I_HOLO_CASTER2 714
#define I_FAIRY_GEM 715
#define I_MEGA_CHARM 716
#define I_MEGA_GLOVE 717

#define I_MACH_BIKE 718
#define I_ACRO_BIKE 719
#define I_WAILMER_PAIL 720
#define I_DEVON_PARTS 721
#define I_SOOT_SACK 722
#define I_BASEMENT_KEY2 723
#define I_POKEBLOCK_KIT 724
#define I_LETTER 725
#define I_EON_TICKET 726
#define I_SCANNER 727
#define I_GO_GOGGLES 728
#define I_METEORITE 729
#define I_KEY_TO_ROOM_1 730
#define I_KEY_TO_ROOM_2 731
#define I_KEY_TO_ROOM_4 732
#define I_KEY_TO_ROOM_6 733
#define I_STORAGE_KEY 734
#define I_DEVON_SCOPE 735

#define I_SS_TICKET2 736
#define I_HM13 737
#define I_DEVON_SCUBA_GEAR 738
#define I_CONTEST_COSTUME 739
#define I_CONTEST_COSTUME2 740
#define I_MAGMA_SUIT 741
#define I_AQUA_SUIT 742
#define I_PAIR_OF_TICKETS 743

#define I_MEGA_BRACELET 744
#define I_MEGA_PENDANT 745
#define I_MEGA_GLASSES 746
#define I_MEGA_ANCHOR 747
#define I_MEGA_STICKPIN 748
#define I_MEGA_TIARA 749
#define I_MEGA_ANKLET 750

#define I_METEORITE4 751

#define I_SWAMPERTITE 752
#define I_SCEPTILITE 753
#define I_SABLENITE 754
#define I_ALTARIANITE 755
#define I_GALLADITE 756
#define I_AUDINITE 757
#define I_METAGROSSITE 758
#define I_SHARPEDONITE 759
#define I_SLOWBRONITE 760
#define I_STEELIXITE 761
#define I_PIDGEOTITE 762
#define I_GLALITITE 763
#define I_DIANCITE 764
#define I_PRISON_BOTTLE 765
#define I_MEGA_CUFF 766
#define I_CAMERUPTITE 767
#define I_LOPUNNITE 768
#define I_SALAMENCITE 769
#define I_BEEDRILLITE 770
#define I_METEORITE2 771
#define I_METEORITE3 772
#define I_KEY_STONE 773
#define I_METEORITE_SHARD 774
#define I_EON_FLUTE 775