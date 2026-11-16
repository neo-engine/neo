/*
Pokémon neo
------------------------------

file        : defines.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

Copyright (C) 2012 - 2026
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

namespace IO {
    // UI strings
    constexpr u16 STR_UI_TOUCH_TO_START              = 69;
    constexpr u16 STR_UI_CHOOSE_A_DIFFERENT_LANGUAGE = 84;
    constexpr u16 STR_UI_PICK_A_LANGUAGE             = 108;
    constexpr u16 STR_UI_OVERWRITE_EXISTING_SAVEDATA = 78;

    constexpr u16 STR_ANIM_EVOLUTION_START    = 50;
    constexpr u16 STR_ANIM_EVOLUTION_COMPLETE = 51;
    constexpr u16 STR_ANIM_EVOLUTION_ABORTED  = 318;
    constexpr u16 STR_ANIM_EGG_HATCH          = 319;

    constexpr u16 STR_ANIM_TRADE_SEND1 = 552;
    constexpr u16 STR_ANIM_TRADE_SEND2 = 553;
    constexpr u16 STR_ANIM_TRADE_RECV1 = 554;
    constexpr u16 STR_ANIM_TRADE_RECV2 = 555;

    constexpr u16 STR_UI_CONTINUE        = 70;
    constexpr u16 STR_UI_NEW_GAME        = 71;
    constexpr u16 STR_UI_SPECIAL_EPISODE = 72;
    constexpr u16 STR_UI_TRANSFER_GAME   = 73;
    constexpr u16 STR_UI_MYSTERY_GIFT    = 326;

    constexpr u16 STR_UI_YES       = 79;
    constexpr u16 STR_UI_NO        = 80;
    constexpr u16 STR_UI_SELECT    = 253;
    constexpr u16 STR_UI_STATUS    = 254;
    constexpr u16 STR_UI_GIVE_ITEM = 255;
    constexpr u16 STR_UI_TAKE_ITEM = 256;
    constexpr u16 STR_UI_USE_ITEM  = 257;
    constexpr u16 STR_UI_SWAP      = 258;
    constexpr u16 STR_UI_DEX_ENTRY = 259;
    constexpr u16 STR_UI_DESELECT  = 261;
    constexpr u16 STR_UI_SEND_IN   = 148;
    constexpr u16 STR_UI_CANCEL    = 260;
    constexpr u16 STR_UI_MONEY     = 393;
    constexpr u16 STR_UI_NONE      = 530;

    constexpr u16 STR_UI_RECEIVE_GIFT  = 556;
    constexpr u16 STR_UI_CHECK_WC      = 557;
    constexpr u16 STR_UI_WELCOME_TO_MG = 558;

    constexpr u16 STR_UI_OBTAIN_DESQUID  = 559;
    constexpr u16 STR_UI_OBTAIN_WIRELESS = 560;
    constexpr u16 STR_UI_OBTAIN_INTERNET = 561;
    constexpr u16 STR_UI_OBTAIN_SEL_MSG  = 562;
    constexpr u16 STR_UI_NO_SPACE        = 563;

    constexpr u16 STR_UI_SEARCHING_FOR_GIFT           = 564;
    constexpr u16 STR_UI_NO_GIFT_FOUND                = 565;
    constexpr u16 STR_UI_ACCEPT_GIFT                  = 566;
    constexpr u16 STR_UI_ACCEPT                       = 567;
    constexpr u16 STR_UI_DECLINE                      = 568;
    constexpr u16 STR_UI_GIFT_DOWNLOADING             = 569;
    constexpr u16 STR_UI_GIFT_ALREADY_COLLECTED       = 570;
    constexpr u16 STR_UI_GIFT_RECEIVED                = 571;
    constexpr u16 STR_UI_THANK_YOU_FOR_PLAYING        = 578;
    constexpr u16 STR_UI_WFC_SETUP_FAILED             = 660;
    constexpr u16 STR_UI_WFC_SERVER_ERROR             = 661;
    constexpr u16 STR_UI_DISTRIBUTING_GIFT            = 662;
    constexpr u16 STR_UI_DISTRIBUTING_CONVERSION_LOSS = 663;

    constexpr u16 STR_UI_PLEASE_COLLECT_GIFT = 572;
    constexpr u16 STR_UI_WONDERCARD          = 573;
    constexpr u16 STR_UI_DATE_RECEIVED       = 574;

    constexpr u16 STR_UI_WC_FLIP = 575;
    constexpr u16 STR_UI_WC_SEND = 576;
    constexpr u16 STR_UI_WC_TOSS = 577;

    constexpr u16 STR_UI_EP0_TEXT_START = 110;
    constexpr u16 STR_UI_EP0_TEXT_END   = 112;

    constexpr u16 STR_UI_INIT_GAME_TEXT0             = 363;
    constexpr u16 STR_UI_INIT_GAME_TEXT1             = 364;
    constexpr u16 STR_UI_INIT_GAME_TEXT2             = 368;
    constexpr u16 STR_UI_INIT_GAME_CHOOSE_APPEARANCE = 379;
    constexpr u16 STR_UI_INIT_GAME_CHOOSE_NAME       = 380;
    constexpr u16 STR_UI_INIT_GAME_CHARACTER_OK      = 381;
    constexpr u16 STR_UI_INIT_GAME_DEFAULT_NAME0     = 382;
    constexpr u16 STR_UI_INIT_GAME_DEFAULT_NAME1     = 383;
    constexpr u16 STR_UI_INIT_GAME_TEXT3             = 384;
    constexpr u16 STR_UI_INIT_GAME_TEXT4             = 385;
    constexpr u16 STR_UI_INIT_GAME_TEXT5             = 386;

    constexpr u16 STR_UI_PKMN_STAT_START = 524;
    constexpr u16 STR_UI_PKMN_STAT_HP    = STR_UI_PKMN_STAT_START + 0;
    constexpr u16 STR_UI_PKMN_STAT_ATK   = STR_UI_PKMN_STAT_START + 1;
    constexpr u16 STR_UI_PKMN_STAT_DEF   = STR_UI_PKMN_STAT_START + 2;
    constexpr u16 STR_UI_PKMN_STAT_SAT   = STR_UI_PKMN_STAT_START + 3;
    constexpr u16 STR_UI_PKMN_STAT_SDF   = STR_UI_PKMN_STAT_START + 4;
    constexpr u16 STR_UI_PKMN_STAT_SPD   = STR_UI_PKMN_STAT_START + 5;

    constexpr u16 STR_UI_PKMN_STAT_LONG_START = 125;
    constexpr u16 STR_UI_PKMN_STAT_LONG_HP    = STR_UI_PKMN_STAT_LONG_START + 0;
    constexpr u16 STR_UI_PKMN_STAT_LONG_ATK   = STR_UI_PKMN_STAT_LONG_START + 1;
    constexpr u16 STR_UI_PKMN_STAT_LONG_DEF   = STR_UI_PKMN_STAT_LONG_START + 2;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SAT   = STR_UI_PKMN_STAT_LONG_START + 3;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SDF   = STR_UI_PKMN_STAT_LONG_START + 4;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SPD   = STR_UI_PKMN_STAT_LONG_START + 5;

    constexpr u16 STR_UI_PKMN_ALREADY_KNOWS_MOVE    = 101;
    constexpr u16 STR_UI_PKMN_LEARNED_MOVE          = 102;
    constexpr u16 STR_UI_PKMN_TRIES_TO_LEARN_MOVE   = 138;
    constexpr u16 STR_UI_PKMN_ALREADY_KNOWS_4_MOVES = 103;
    constexpr u16 STR_UI_PKMN_CANT_FORGET_MOVE      = 105;
    constexpr u16 STR_UI_PKMN_CANT_LEARN_MOVE       = 106;
    constexpr u16 STR_UI_PKMN_DIDNT_LEARN_MOVE      = 325;

    constexpr u16 STR_UI_JBOX_LUDICOBOX     = 441;
    constexpr u16 STR_UI_JBOX_CHOOSE_SONG   = 503;
    constexpr u16 STR_UI_JBOX_STOP_PLAYBACK = 502;
    constexpr u16 STR_UI_JBOX_CHOOSE_RECORD = 540;

    constexpr u16 STR_UI_WOULD_YOU_LIKE_TO_SAVE = 91;
    constexpr u16 STR_UI_SAVING_A_LOT_OF_DATA   = 92;
    constexpr u16 STR_UI_SAVING_COMPLETE        = 93;
    constexpr u16 STR_UI_SAVING_FAILED          = 94;

    constexpr u16 STR_UI_MONEYTYPE_START_PAY = 649;
    constexpr u16 STR_UI_MONEYTYPE_START     = 393;
    constexpr u16 STR_UI_MONEYTYPE_MONEY     = STR_UI_MONEYTYPE_START + 0;
    constexpr u16 STR_UI_MONEYTYPE_BP        = STR_UI_MONEYTYPE_START + 1;
    constexpr u16 STR_UI_MONEYTYPE_COINS     = STR_UI_MONEYTYPE_START + 2;
    constexpr u16 STR_UI_MONEYTYPE_ASH       = 410;

    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_START = 401;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_MONEY = STR_UI_NOT_ENOUGH_MONEYTYPE_START + 0;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_BP    = STR_UI_NOT_ENOUGH_MONEYTYPE_START + 1;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_COINS = STR_UI_NOT_ENOUGH_MONEYTYPE_START + 2;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_ASH   = 411;

    constexpr u16 STR_UI_MART_CLERK_HOW_MANY              = 397;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START = 398;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_MONEY
        = STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + 0;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_BP
        = STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + 1;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_COINS
        = STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + 2;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_ASH = 412;

    constexpr u16 STR_UI_ITEMCOUNT_IN_BAG = 396;

    constexpr u16 STR_UI_ASH_COUNT  = 413;
    constexpr u16 STR_UI_COIN_COUNT = 67;
    constexpr u16 STR_UI_BP_COUNT   = 68;

    constexpr u16 STR_UI_MENU_ITEM_NAME_START = 335;
    constexpr u16 STR_UI_MENU_ITEM_PKMN       = STR_UI_MENU_ITEM_NAME_START + 0;
    constexpr u16 STR_UI_MENU_ITEM_POKEDEX    = STR_UI_MENU_ITEM_NAME_START + 1;
    constexpr u16 STR_UI_MENU_ITEM_BAG        = STR_UI_MENU_ITEM_NAME_START + 2;
    constexpr u16 STR_UI_MENU_ITEM_TRAINER_ID = STR_UI_MENU_ITEM_NAME_START + 3;
    constexpr u16 STR_UI_MENU_ITEM_SAVE       = STR_UI_MENU_ITEM_NAME_START + 4;
    constexpr u16 STR_UI_MENU_ITEM_OPTIONS    = STR_UI_MENU_ITEM_NAME_START + 5;

    constexpr u16 STR_UI_KEY_BACKSPACE = 361;
    constexpr u16 STR_UI_KEY_ENTER     = 362;

    constexpr u16 STR_UI_USED_ITEM_X_TIMES        = 522;
    constexpr u16 STR_UI_USED_ITEM                = 523;
    constexpr u16 STR_UI_HANDED_OVER_ITEM_X_TIMES = 516;
    constexpr u16 STR_UI_HANDED_OVER_ITEM         = 517;
    constexpr u16 STR_UI_OBTAINED_ITEM_X_TIMES    = 419;
    constexpr u16 STR_UI_OBTAINED_ITEM            = 420;
    constexpr u16 STR_UI_PUT_ITEM_INTO_BAG        = 85;

    constexpr u16 STR_ITEM_TRACER_NO_CHARGE      = 535;
    constexpr u16 STR_ITEM_REPEL_ACTIVATED       = 62;
    constexpr u16 STR_ITEM_SUPER_REPEL_ACTIVATED = 63;
    constexpr u16 STR_ITEM_MAX_REPEL_ACTIVATED   = 64;
    constexpr u16 STR_ITEM_EXP_ALL_ACTIVATED     = 65;
    constexpr u16 STR_ITEM_EXP_ALL_DEACTIVATED   = 66;

    constexpr u16 STR_UI_CANNOT_USE_FIELD_ITEM  = 57;
    constexpr u16 STR_UI_CAN_REGISTER_ITEM_TO_Y = 97;

    constexpr u16 STR_UI_STS_TOOK_ITEM_FROM_PKMN      = 100;
    constexpr u16 STR_UI_STS_GAVE_ITEM_TO_PKMN        = 264;
    constexpr u16 STR_UI_STS_CHOOSE_THESE_PKMN        = 265;
    constexpr u16 STR_UI_STS_CHOOSE_THIS_PKMN         = 266;
    constexpr u16 STR_UI_STS_NATURE_START             = 176;
    constexpr u16 STR_UI_STS_PERSONALITY_START        = 201;
    constexpr u16 STR_UI_STS_FLY_WHERE                = 541;
    constexpr u16 STR_UI_STS_SHEEN                    = 542;
    constexpr u16 STR_UI_STS_CONDITION                = 543;
    constexpr u16 STR_UI_STS_NO_RIBBONS               = 544;
    constexpr u16 STR_UI_STS_RIBBONS                  = 545;
    constexpr u16 STR_UI_STS_SPECIES_IDX              = 267;
    constexpr u16 STR_UI_STS_SPECIES_NAME             = 268;
    constexpr u16 STR_UI_STS_OT                       = 269;
    constexpr u16 STR_UI_STS_ID                       = 270;
    constexpr u16 STR_UI_STS_EXP                      = 271;
    constexpr u16 STR_UI_STS_NEXT                     = 272;
    constexpr u16 STR_UI_STS_LEVEL                    = 273;
    constexpr u16 STR_UI_STS_PKMN_INFO                = 274;
    constexpr u16 STR_UI_STS_FROM                     = 286;
    constexpr u16 STR_UI_STS_IN_AT                    = 287;
    constexpr u16 STR_UI_STS_FROM_PERSON              = 288;
    constexpr u16 STR_UI_STS_NATURE_PREFIX            = 294;
    constexpr u16 STR_UI_STS_NATURE_SUFFIX            = 295;
    constexpr u16 STR_UI_STS_FATEFUL_ENC              = 297;
    constexpr u16 STR_UI_STS_FATEFUL_ENC_AT_LEVEL     = 302;
    constexpr u16 STR_UI_STS_APP_FATEFUL_ENC_AT_LEVEL = 303;
    constexpr u16 STR_UI_STS_MET_AT_LEVEL             = 300;
    constexpr u16 STR_UI_STS_APP_MET_AT_LEVEL         = 301;
    constexpr u16 STR_UI_STS_EGG_RECEIVED             = 296;
    constexpr u16 STR_UI_STS_EGG_HATCHED              = 298;
    constexpr u16 STR_UI_STS_EGG_APP_HATCHED          = 299;
    constexpr u16 STR_UI_STS_MANA_EGG_DSCR_LINE1      = 289;
    constexpr u16 STR_UI_STS_MANA_EGG_DSCR_LINE2      = 290;
    constexpr u16 STR_UI_STS_MANA_EGG_DSCR_LINE3      = 291;
    constexpr u16 STR_UI_STS_EGG_DSCR_LINE1           = 284;
    constexpr u16 STR_UI_STS_EGG_DSCR_LINE2           = 285;
    constexpr u16 STR_UI_STS_EGG_STATUS_1_0           = 275;
    constexpr u16 STR_UI_STS_EGG_STATUS_1_1           = 276;
    constexpr u16 STR_UI_STS_EGG_STATUS_1_2           = 277;
    constexpr u16 STR_UI_STS_EGG_STATUS_2_0           = 278;
    constexpr u16 STR_UI_STS_EGG_STATUS_2_1           = 279;
    constexpr u16 STR_UI_STS_EGG_STATUS_2_2           = 280;
    constexpr u16 STR_UI_STS_EGG_STATUS_3_0           = 281;
    constexpr u16 STR_UI_STS_EGG_STATUS_3_1           = 282;
    constexpr u16 STR_UI_STS_EGG_STATUS_3_2           = 283;
    constexpr u16 STR_UI_STS_ITEM                     = 292;
    constexpr u16 STR_UI_STS_ABITLITY                 = 293;
    constexpr u16 STR_UI_STS_LIKES                    = 304;
    constexpr u16 STR_UI_STS_LIKES_SUFFIX             = 305;
    constexpr u16 STR_UI_STS_POKEBLOCK                = 306;
    constexpr u16 STR_UI_STS_TASTES_START             = 231;
    constexpr u16 STR_UI_STS_PP_FORMAT                = 307;

    constexpr u16 STR_UI_BAG_PAGE_NAME_START    = 10;
    constexpr u16 STR_UI_BAG_PAGE_NAME_ITEMS    = STR_UI_BAG_PAGE_NAME_START + 0;
    constexpr u16 STR_UI_BAG_PAGE_NAME_MEDICINE = STR_UI_BAG_PAGE_NAME_START + 1;
    constexpr u16 STR_UI_BAG_PAGE_NAME_TM_HM    = STR_UI_BAG_PAGE_NAME_START + 2;
    constexpr u16 STR_UI_BAG_PAGE_NAME_BERRIES  = STR_UI_BAG_PAGE_NAME_START + 3;
    constexpr u16 STR_UI_BAG_PAGE_NAME_KEY_ITEM = STR_UI_BAG_PAGE_NAME_START + 4;
    constexpr u16 STR_UI_BAG_NUMBER             = 431;
    constexpr u16 STR_UI_BAG_TYPE               = 28;
    constexpr u16 STR_UI_BAG_CATEGORY           = 29;
    constexpr u16 STR_UI_BAG_PP                 = 30;
    constexpr u16 STR_UI_BAG_POWER              = 320;
    constexpr u16 STR_UI_BAG_ACCURACY           = 321;
    constexpr u16 STR_UI_BAG_ACCURACY_NO_MISS   = 322;
    constexpr u16 STR_UI_BAG_PARTY_EGG          = 33;
    constexpr u16 STR_UI_BAG_PARTY_NO_ITEM      = 41;
    constexpr u16 STR_UI_BAG_ALREADY_LEARNED    = 34;
    constexpr u16 STR_UI_BAG_LEARN_POSSIBLE     = 35;
    constexpr u16 STR_UI_BAG_LEARN_NOT_POSSIBLE = 36;
    constexpr u16 STR_UI_BAG_POSSIBLE           = 39;
    constexpr u16 STR_UI_BAG_NOT_POSSIBLE       = 40;
    constexpr u16 STR_UI_BAG_CHOOSE_PKMN        = 537;
    constexpr u16 STR_UI_BAG_ASK_ITEM_ACTION    = 56;
    constexpr u16 STR_UI_BAG_CHOOSE_MOVE        = 48;
    constexpr u16 STR_UI_BAG_EMPTY              = 42;
    constexpr u16 STR_UI_BAG_ITEM_USED          = 49;
    constexpr u16 STR_UI_BAG_ITEM_WASTED        = 52;
    constexpr u16 STR_UI_BAG_PKMN_CARRIES_ITEM  = 53;
    constexpr u16 STR_UI_BAG_CHOOSE_ITEM        = 55;
    constexpr u16 STR_UI_BAG_ITEM_GIVE          = 43;
    constexpr u16 STR_UI_BAG_ITEM_APPLY         = 44;
    constexpr u16 STR_UI_BAG_ITEM_REGISTER      = 45;
    constexpr u16 STR_UI_BAG_ITEM_USE           = 46;
    constexpr u16 STR_UI_BAG_ITEM_TOSS          = 47;
    constexpr u16 STR_UI_BAG_ITEM_SELL          = 405;
    constexpr u16 STR_UI_BAG_ITEM_DETAILS       = 323;
    constexpr u16 STR_UI_BAG_ITEM_DEREGISTER    = 324;
    constexpr u16 STR_UI_BAG_CANNOT_BUY_ITEM    = 404;
    constexpr u16 STR_UI_BAG_WILL_BUY_ITEM_FOR  = 406;
    constexpr u16 STR_UI_BAG_ITEM_TOSS_CONFIRM  = 407;
    constexpr u16 STR_UI_BAG_ITEM_TOSS_ASK_QTY  = 538;
    constexpr u16 STR_UI_BAG_ITEM_SELL_ASK_QTY  = 539;

    constexpr u16 STR_BATTLE_BANK = ( 1 << 12 );

    constexpr u16 STR_BATTLE_USED_MOVE                      = STR_BATTLE_BANK + 0;
    constexpr u16 STR_BATTLE_SELECT_A_MOVE                  = STR_BATTLE_BANK + 1;
    constexpr u16 STR_BATTLE_PLAYER_USED_ITEM               = STR_BATTLE_BANK + 2;
    constexpr u16 STR_BATTLE_TERRAIN_NO_CHANGE              = STR_BATTLE_BANK + 3;
    constexpr u16 STR_BATTLE_CHALLENGED                     = STR_BATTLE_BANK + 4;
    constexpr u16 STR_BATTLE_WHAT_TO_DO                     = STR_BATTLE_BANK + 5;
    constexpr u16 STR_BATTLE_GOT_AWAY_SAFELY                = STR_BATTLE_BANK + 6;
    constexpr u16 STR_BATTLE_COULDNT_ESCAPE                 = STR_BATTLE_BANK + 7;
    constexpr u16 STR_BATTLE_EXP_GAINED                     = STR_BATTLE_BANK + 8;
    constexpr u16 STR_BATTLE_ADVANCE_LEVEL                  = STR_BATTLE_BANK + 9;
    constexpr u16 STR_BATTLE_USE_ITEM_TO_ACT_FIRST          = STR_BATTLE_BANK + 10;
    constexpr u16 STR_BATTLE_WILD_PKMN_FLED                 = STR_BATTLE_BANK + 11;
    constexpr u16 STR_BATTLE_NOTHING_HAPPENED               = STR_BATTLE_BANK + 12;
    constexpr u16 STR_BATTLE_STAT_HP                        = STR_BATTLE_BANK + 13;
    constexpr u16 STR_BATTLE_STAT_ATK                       = STR_BATTLE_BANK + 14;
    constexpr u16 STR_BATTLE_STAT_DEF                       = STR_BATTLE_BANK + 15;
    constexpr u16 STR_BATTLE_STAT_SP_ATK                    = STR_BATTLE_BANK + 16;
    constexpr u16 STR_BATTLE_STAT_SP_DEF                    = STR_BATTLE_BANK + 17;
    constexpr u16 STR_BATTLE_STAT_SPEED                     = STR_BATTLE_BANK + 18;
    constexpr u16 STR_BATTLE_STAT_EVASION                   = STR_BATTLE_BANK + 19;
    constexpr u16 STR_BATTLE_STAT_ACCURACY                  = STR_BATTLE_BANK + 20;
    constexpr u16 STR_BATTLE_BOOST_UNCHANGED                = STR_BATTLE_BANK + 21;
    constexpr u16 STR_BATTLE_BOOST_UP                       = STR_BATTLE_BANK + 22;
    constexpr u16 STR_BATTLE_BOOST_UP_SHARPLY               = STR_BATTLE_BANK + 23;
    constexpr u16 STR_BATTLE_BOOST_UP_DRASTICALLY           = STR_BATTLE_BANK + 24;
    constexpr u16 STR_BATTLE_BOOST_DOWN                     = STR_BATTLE_BANK + 25;
    constexpr u16 STR_BATTLE_BOOST_DOWN_HARSHLY             = STR_BATTLE_BANK + 26;
    constexpr u16 STR_BATTLE_BOOST_DOWN_SEVERELY            = STR_BATTLE_BANK + 27;
    constexpr u16 STR_BATTLE_SENDS_OUT                      = STR_BATTLE_BANK + 28;
    constexpr u16 STR_BATTLE_MENU_FIGHT                     = STR_BATTLE_BANK + 29;
    constexpr u16 STR_BATTLE_MENU_POKEMON                   = STR_BATTLE_BANK + 30;
    constexpr u16 STR_BATTLE_MENU_RUN                       = STR_BATTLE_BANK + 31;
    constexpr u16 STR_BATTLE_MENU_BAG                       = STR_BATTLE_BANK + 32;
    constexpr u16 STR_BATTLE_MENU_BACK                      = STR_BATTLE_BANK + 33;
    constexpr u16 STR_BATTLE_PREPARE_SHELL_TRAP             = STR_BATTLE_BANK + 34;
    constexpr u16 STR_BATTLE_PREPARE_FOCUS_PUNCH            = STR_BATTLE_BANK + 35;
    constexpr u16 STR_BATTLE_PREPARE_BEAK_BLAST             = STR_BATTLE_BANK + 36;
    constexpr u16 STR_BATTLE_PKMN_RETURNED                  = STR_BATTLE_BANK + 37;
    constexpr u16 STR_BATTLE_THAT_IS_ENOUGH                 = STR_BATTLE_BANK + 38;
    constexpr u16 STR_BATTLE_RECHARGES                      = STR_BATTLE_BANK + 39;
    constexpr u16 STR_BATTLE_BECAME_CONFUSED                = STR_BATTLE_BANK + 40;
    constexpr u16 STR_BATTLE_EAT_ITEM_HEAL_CONFUSION        = STR_BATTLE_BANK + 41;
    constexpr u16 STR_BATTLE_EAT_ITEM                       = STR_BATTLE_BANK + 42;
    constexpr u16 STR_BATTLE_EVADED_ATTACK                  = STR_BATTLE_BANK + 43;
    constexpr u16 STR_BATTLE_PASS_ITEM_SPECIAL              = STR_BATTLE_BANK + 44;
    constexpr u16 STR_BATTLE_PASS_ITEM_PROTECT_PKMN         = STR_BATTLE_BANK + 45;
    constexpr u16 STR_BATTLE_NO_EFFECT_ON                   = STR_BATTLE_BANK + 46;
    constexpr u16 STR_BATTLE_SUPER_EFFECTIVE_ON             = STR_BATTLE_BANK + 47;
    constexpr u16 STR_BATTLE_NOT_VERY_EFFECTIVE_ON          = STR_BATTLE_BANK + 48;
    constexpr u16 STR_BATTLE_HIT_BY_RECOIL                  = STR_BATTLE_BANK + 49;
    constexpr u16 STR_BATTLE_RESTORES_HP                    = STR_BATTLE_BANK + 50;
    constexpr u16 STR_BATTLE_PKMN_FAINTED                   = STR_BATTLE_BANK + 51;
    constexpr u16 STR_BATTLE_CRITICAL_HIT                   = STR_BATTLE_BANK + 52;
    constexpr u16 STR_BATTLE_IS_CONFUSED                    = STR_BATTLE_BANK + 53;
    constexpr u16 STR_BATTLE_CONFUSION_HEALED               = STR_BATTLE_BANK + 54;
    constexpr u16 STR_BATTLE_HURT_ITSELF_IN_CONFUSION       = STR_BATTLE_BANK + 55;
    constexpr u16 STR_BATTLE_FLINCHES                       = STR_BATTLE_BANK + 56;
    constexpr u16 STR_BATTLE_FROZEN_SOLID                   = STR_BATTLE_BANK + 57;
    constexpr u16 STR_BATTLE_WAS_THAWED                     = STR_BATTLE_BANK + 58;
    constexpr u16 STR_BATTLE_ASLEEP                         = STR_BATTLE_BANK + 59;
    constexpr u16 STR_BATTLE_WOKE_UP                        = STR_BATTLE_BANK + 60;
    constexpr u16 STR_BATTLE_PARALYZED_CANT_MOVE            = STR_BATTLE_BANK + 61;
    constexpr u16 STR_BATTLE_IMMOBILIZED_BY_LOVE            = STR_BATTLE_BANK + 62;
    constexpr u16 STR_BATTLE_IT_FAILED                      = STR_BATTLE_BANK + 63;
    constexpr u16 STR_BATTLE_POWER_HERB_CHARGED             = STR_BATTLE_BANK + 64;
    constexpr u16 STR_BATTLE_LOSES_SOME_HP                  = STR_BATTLE_BANK + 65;
    constexpr u16 STR_BATTLE_MEGA_EVOLVE_WISH               = STR_BATTLE_BANK + 66;
    constexpr u16 STR_BATTLE_MEGA_EVOLVE_WISH_GRANTED       = STR_BATTLE_BANK + 67;
    constexpr u16 STR_BATTLE_WILD_NAME_LOWER                = STR_BATTLE_BANK + 68;
    constexpr u16 STR_BATTLE_FOE_NAME_LOWER                 = STR_BATTLE_BANK + 69;
    constexpr u16 STR_BATTLE_WILD_NAME_UPPER                = STR_BATTLE_BANK + 70;
    constexpr u16 STR_BATTLE_FOE_NAME_UPPER                 = STR_BATTLE_BANK + 71;
    constexpr u16 STR_BATTLE_HP_FORMAT                      = STR_BATTLE_BANK + 72;
    constexpr u16 STR_BATTLE_ABILITY_BANNER                 = STR_BATTLE_BANK + 73;
    constexpr u16 STR_BATTLE_ENCOUNTERED_WILD               = STR_BATTLE_BANK + 74;
    constexpr u16 STR_BATTLE_SEND_OUT_GO                    = STR_BATTLE_BANK + 75;
    constexpr u16 STR_BATTLE_DETECTS_MOVE                   = STR_BATTLE_BANK + 76;
    constexpr u16 STR_BATTLE_SHUDERS                        = STR_BATTLE_BANK + 77;
    constexpr u16 STR_BATTLE_IDENTIFIES_ITEM                = STR_BATTLE_BANK + 78;
    constexpr u16 STR_BATTLE_IDENTIFIES_TWO_ITEMS           = STR_BATTLE_BANK + 79;
    constexpr u16 STR_BATTLE_HIT_TIMES                      = STR_BATTLE_BANK + 80;
    constexpr u16 STR_BATTLE_PKMN_CAPTURE_SUCC              = STR_BATTLE_BANK + 81;
    constexpr u16 STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_0 = STR_BATTLE_BANK + 82;
    constexpr u16 STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_1 = STR_BATTLE_BANK + 83;
    constexpr u16 STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_2 = STR_BATTLE_BANK + 84;
    constexpr u16 STR_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_3 = STR_BATTLE_BANK + 85;
    constexpr u16 STR_BATTLE_WEATHER_CLEAR                  = STR_BATTLE_BANK + 86;
    constexpr u16 STR_BATTLE_WEATHER_RAIN_STARTED           = STR_BATTLE_BANK + 87;
    constexpr u16 STR_BATTLE_WEATHER_HAIL_STARTED           = STR_BATTLE_BANK + 88;
    constexpr u16 STR_BATTLE_WEATHER_FOG                    = STR_BATTLE_BANK + 89;
    constexpr u16 STR_BATTLE_WEATHER_SANDSTORM              = STR_BATTLE_BANK + 90;
    constexpr u16 STR_BATTLE_WEATHER_SUN                    = STR_BATTLE_BANK + 91;
    constexpr u16 STR_BATTLE_WEATHER_HEAVY_RAIN             = STR_BATTLE_BANK + 92;
    constexpr u16 STR_BATTLE_WEATHER_HEAVY_SUN              = STR_BATTLE_BANK + 93;
    constexpr u16 STR_BATTLE_WEATHER_HEAVY_WINDS            = STR_BATTLE_BANK + 94;
    constexpr u16 STR_BATTLE_WEATHER_RAIN_ONGOING           = STR_BATTLE_BANK + 95;
    constexpr u16 STR_BATTLE_WEATHER_HAIL_ONGOING           = STR_BATTLE_BANK + 96;
    constexpr u16 STR_BATTLE_WEATHER_FOG_ONGOING            = STR_BATTLE_BANK + 97;
    constexpr u16 STR_BATTLE_WEATHER_SANDSTORM_ONGOING      = STR_BATTLE_BANK + 98;
    constexpr u16 STR_BATTLE_WEATHER_SUN_ONGOING            = STR_BATTLE_BANK + 99;
    constexpr u16 STR_BATTLE_WEATHER_HEAVY_RAIN_ONGOING     = STR_BATTLE_BANK + 100;
    constexpr u16 STR_BATTLE_WEATHER_HEAVY_SUN_ONGOING      = STR_BATTLE_BANK + 101;
    constexpr u16 STR_BATTLE_WEATHER_HEAVY_WINDS_ONGOING    = STR_BATTLE_BANK + 102;
    constexpr u16 STR_BATTLE_TERRAIN_CLEAR                  = STR_BATTLE_BANK + 103;
    constexpr u16 STR_BATTLE_TERRAIN_PSYCHIC                = STR_BATTLE_BANK + 104;
    constexpr u16 STR_BATTLE_TERRAIN_ELECTRIC               = STR_BATTLE_BANK + 105;
    constexpr u16 STR_BATTLE_TERRAIN_MISTY                  = STR_BATTLE_BANK + 106;
    constexpr u16 STR_BATTLE_TERRAIN_GRASS                  = STR_BATTLE_BANK + 107;
    constexpr u16 STR_BATTLE_PW_IONDELUGE_ADDED             = STR_BATTLE_BANK + 108;
    constexpr u16 STR_BATTLE_PW_MAGICROOM_ADDED             = STR_BATTLE_BANK + 109;
    constexpr u16 STR_BATTLE_PW_WONDERROOM_ADDED            = STR_BATTLE_BANK + 110;
    constexpr u16 STR_BATTLE_PW_TRICKROOM_ADDED             = STR_BATTLE_BANK + 111;
    constexpr u16 STR_BATTLE_PW_GRAVITY_ADDED               = STR_BATTLE_BANK + 112;
    constexpr u16 STR_BATTLE_PW_WATERSPORT_ADDED            = STR_BATTLE_BANK + 113;
    constexpr u16 STR_BATTLE_PW_MUDSPORT_ADDED              = STR_BATTLE_BANK + 114;
    constexpr u16 STR_BATTLE_PW_FAIRYLOCK_ADDED             = STR_BATTLE_BANK + 115;
    constexpr u16 STR_BATTLE_PW_IONDELUGE_ENDED             = STR_BATTLE_BANK + 116;
    constexpr u16 STR_BATTLE_PW_MAGICROOM_ENDED             = STR_BATTLE_BANK + 117;
    constexpr u16 STR_BATTLE_PW_WONDERROOM_ENDED            = STR_BATTLE_BANK + 118;
    constexpr u16 STR_BATTLE_PW_TRICKROOM_ENDED             = STR_BATTLE_BANK + 119;
    constexpr u16 STR_BATTLE_PW_GRAVITY_ENDED               = STR_BATTLE_BANK + 120;
    constexpr u16 STR_BATTLE_PW_WATERSPORT_ENDED            = STR_BATTLE_BANK + 121;
    constexpr u16 STR_BATTLE_PW_MUDSPORT_ENDED              = STR_BATTLE_BANK + 122;
    constexpr u16 STR_BATTLE_PW_FAIRYLOCK_ENDED             = STR_BATTLE_BANK + 123;
    constexpr u16 STR_BATTLE_IS_POISONED                    = STR_BATTLE_BANK + 124;
    constexpr u16 STR_BATTLE_IS_BADLY_POISONED              = STR_BATTLE_BANK + 125;
    constexpr u16 STR_BATTLE_IS_BURNED                      = STR_BATTLE_BANK + 126;
    constexpr u16 STR_BATTLE_SHELL_BELL_HEALS               = STR_BATTLE_BANK + 127;
    constexpr u16 STR_BATTLE_HARMED_BY_CURSE                = STR_BATTLE_BANK + 128;
    constexpr u16 STR_BATTLE_HEALED_BY_INGRAIN              = STR_BATTLE_BANK + 129;
    constexpr u16 STR_BATTLE_HEALED_BY_AQUA_RING            = STR_BATTLE_BANK + 130;
    constexpr u16 STR_BATTLE_HARMED_BY_NIGHTMARE            = STR_BATTLE_BANK + 131;
    constexpr u16 STR_BATTLE_ACTIVATE_ITEM                  = STR_BATTLE_BANK + 132;
    constexpr u16 STR_BATTLE_ACTIVATE_AIR_BALLON            = STR_BATTLE_BANK + 133;
    constexpr u16 STR_BATTLE_DRINK_ITEM                     = STR_BATTLE_BANK + 134;
    constexpr u16 STR_BATTLE_DIVES                          = STR_BATTLE_BANK + 135;
    constexpr u16 STR_BATTLE_DUG_HOLE                       = STR_BATTLE_BANK + 136;
    constexpr u16 STR_BATTLE_FLIES_UP                       = STR_BATTLE_BANK + 137;
    constexpr u16 STR_BATTLE_JUMPS_UP                       = STR_BATTLE_BANK + 138;
    constexpr u16 STR_BATTLE_SKYDROP                        = STR_BATTLE_BANK + 139;
    constexpr u16 STR_BATTLE_DISAPPEARED                    = STR_BATTLE_BANK + 140;
    constexpr u16 STR_BATTLE_CONTINUES_CRASHES              = STR_BATTLE_BANK + 141;
    constexpr u16 STR_BATTLE_ONE_HIT_KO                     = STR_BATTLE_BANK + 142;
    constexpr u16 STR_BATTLE_LOST_FOCUS                     = STR_BATTLE_BANK + 143;
    constexpr u16 STR_BATTLE_TRAINER_USED_ITEM              = STR_BATTLE_BANK + 144;
    constexpr u16 STR_BATTLE_WIN_MONEY                      = STR_BATTLE_BANK + 145;
    constexpr u16 STR_BATTLE_LOSE_MONEY                     = STR_BATTLE_BANK + 146;
    constexpr u16 STR_BATTLE_GOT_POISONED                   = STR_BATTLE_BANK + 147;
    constexpr u16 STR_BATTLE_GOT_BADLY_POISONED             = STR_BATTLE_BANK + 148;
    constexpr u16 STR_BATTLE_GOT_BURNED                     = STR_BATTLE_BANK + 149;
    constexpr u16 STR_BATTLE_WAS_FROZEN                     = STR_BATTLE_BANK + 150;
    constexpr u16 STR_BATTLE_FELL_ASLEEP                    = STR_BATTLE_BANK + 151;
    constexpr u16 STR_BATTLE_WAS_PARALYZED                  = STR_BATTLE_BANK + 152;
    constexpr u16 STR_BATTLE_WAS_CONFUSED                   = STR_BATTLE_BANK + 153;
    constexpr u16 STR_BATTLE_GAINED_LASER_FOCUS             = STR_BATTLE_BANK + 154;
    constexpr u16 STR_BATTLE_ELECTROMAGNETISM_LEVITATED     = STR_BATTLE_BANK + 155;
    constexpr u16 STR_BATTLE_AQUA_RING_FORMED               = STR_BATTLE_BANK + 156;
    constexpr u16 STR_BATTLE_GETTING_PUMPED                 = STR_BATTLE_BANK + 157;
    constexpr u16 STR_BATTLE_PLANTED_ROOTS                  = STR_BATTLE_BANK + 158;
    constexpr u16 STR_BATTLE_WAS_IDENTIFIED                 = STR_BATTLE_BANK + 159;
    constexpr u16 STR_BATTLE_SEED_PLANTED                   = STR_BATTLE_BANK + 160;
    constexpr u16 STR_BATTLE_PROTECTS_ITSELF                = STR_BATTLE_BANK + 161;
    constexpr u16 STR_BATTLE_PROTECTION_BROKEN              = STR_BATTLE_BANK + 162;
    constexpr u16 STR_BATTLE_PKMN_CAPTURE_FORCED_FAIL       = STR_BATTLE_BANK + 163;
    constexpr u16 STR_BATTLE_HURT_BY_POINTED_STONES         = STR_BATTLE_BANK + 164;
    constexpr u16 STR_BATTLE_HURT_BY_SPIKES                 = STR_BATTLE_BANK + 165;
    constexpr u16 STR_BATTLE_POINTED_STONES_SET             = STR_BATTLE_BANK + 166;
    constexpr u16 STR_BATTLE_SPIKES_SET                     = STR_BATTLE_BANK + 167;
    constexpr u16 STR_BATTLE_TOXIC_SPIKES_SET               = STR_BATTLE_BANK + 168;
    constexpr u16 STR_BATTLE_STICKY_WEB_SET                 = STR_BATTLE_BANK + 169;
    constexpr u16 STR_BATTLE_CAUGHT_IN_STICKY_WEB           = STR_BATTLE_BANK + 170;
    constexpr u16 STR_BATTLE_WISH_CAME_TRUE                 = STR_BATTLE_BANK + 171;
    constexpr u16 STR_BATTLE_HEALING_WISH_HEAL              = STR_BATTLE_BANK + 172;
    constexpr u16 STR_BATTLE_LUNAR_DANCE_HEAL               = STR_BATTLE_BANK + 173;
    constexpr u16 STR_BATTLE_SUBSTITUTE_TAKES_HIT           = STR_BATTLE_BANK + 174;
    constexpr u16 STR_BATTLE_SUBSTITUTE_FADED               = STR_BATTLE_BANK + 175;
    constexpr u16 STR_BATTLE_QUICKGUARD_PROTECTS            = STR_BATTLE_BANK + 176;
    constexpr u16 STR_BATTLE_WIDEGUARD_PROTECTS             = STR_BATTLE_BANK + 177;
    constexpr u16 STR_BATTLE_CRAFTYSHIELD_PROTECTS          = STR_BATTLE_BANK + 178;
    constexpr u16 STR_BATTLE_MATBLOCK_BLOCKS                = STR_BATTLE_BANK + 179;
    constexpr u16 STR_BATTLE_SAFEGUARD_SET                  = STR_BATTLE_BANK + 180;
    constexpr u16 STR_BATTLE_SAFEGUARD_PROTECTS             = STR_BATTLE_BANK + 181;
    constexpr u16 STR_BATTLE_MIST_SET                       = STR_BATTLE_BANK + 182;
    constexpr u16 STR_BATTLE_MIST_PROTECTS                  = STR_BATTLE_BANK + 183;
    constexpr u16 STR_BATTLE_CENTER_OF_ATTENTION            = STR_BATTLE_BANK + 184;
    constexpr u16 STR_BATTLE_BECAME_TRAPPED                 = STR_BATTLE_BANK + 185;
    constexpr u16 STR_BATTLE_RELEASED_FROM                  = STR_BATTLE_BANK + 186;
    constexpr u16 STR_BATTLE_ENCORE_START                   = STR_BATTLE_BANK + 187;
    constexpr u16 STR_BATTLE_ENCORE_END                     = STR_BATTLE_BANK + 188;
    constexpr u16 STR_BATTLE_DISABLE_START                  = STR_BATTLE_BANK + 189;
    constexpr u16 STR_BATTLE_DISABLE_END                    = STR_BATTLE_BANK + 190;
    constexpr u16 STR_BATTLE_PERISH_SONG                    = STR_BATTLE_BANK + 191;
    constexpr u16 STR_BATTLE_PERISH_COUNT                   = STR_BATTLE_BANK + 192;
    constexpr u16 STR_BATTLE_BRACED_ITSELF                  = STR_BATTLE_BANK + 193;
    constexpr u16 STR_BATTLE_ENDURED_THE_HIT                = STR_BATTLE_BANK + 194;
    constexpr u16 STR_BATTLE_COATED_MIRROR                  = STR_BATTLE_BANK + 195;
    constexpr u16 STR_BATTLE_SNATCH_WAIT                    = STR_BATTLE_BANK + 196;
    constexpr u16 STR_BATTLE_SNATCH_STOLEN                  = STR_BATTLE_BANK + 197;
    constexpr u16 STR_BATTLE_TAR_COATED                     = STR_BATTLE_BANK + 198;
    constexpr u16 STR_BATTLE_ABILITY_SUPPRESSED             = STR_BATTLE_BANK + 199;
    constexpr u16 STR_BATTLE_STATS_SWAPPED                  = STR_BATTLE_BANK + 200;
    constexpr u16 STR_BATTLE_IMPRISON_START                 = STR_BATTLE_BANK + 201;
    constexpr u16 STR_BATTLE_SEALED_MOVE                    = STR_BATTLE_BANK + 202;
    constexpr u16 STR_BATTLE_HELPING_HAND                   = STR_BATTLE_BANK + 203;
    constexpr u16 STR_BATTLE_DESTINY_BOND                   = STR_BATTLE_BANK + 204;
    constexpr u16 STR_BATTLE_STORING_ENERGY                 = STR_BATTLE_BANK + 205;
    constexpr u16 STR_BATTLE_ENERGY_RELEASED                = STR_BATTLE_BANK + 206;
    constexpr u16 STR_BATTLE_GRUDGE_START                   = STR_BATTLE_BANK + 207;
    constexpr u16 STR_BATTLE_GRUDGE_PP_LOST                 = STR_BATTLE_BANK + 208;
    constexpr u16 STR_BATTLE_COVERED_IN_POWDER              = STR_BATTLE_BANK + 209;
    constexpr u16 STR_BATTLE_POWDER_EXPLODES                = STR_BATTLE_BANK + 210;
    constexpr u16 STR_BATTLE_HURLED_INTO_AIR                = STR_BATTLE_BANK + 211;
    constexpr u16 STR_BATTLE_TELEKINESIS_END                = STR_BATTLE_BANK + 212;
    constexpr u16 STR_BATTLE_SMACK_DOWN_FELL                = STR_BATTLE_BANK + 213;
    constexpr u16 STR_BATTLE_BECAME_NIMBLE                  = STR_BATTLE_BANK + 214;
    constexpr u16 STR_BATTLE_UPROAR_START                   = STR_BATTLE_BANK + 215;
    constexpr u16 STR_BATTLE_FUTURE_SIGHT_SEES              = STR_BATTLE_BANK + 216;
    constexpr u16 STR_BATTLE_TERRAIN_PROTECTS               = STR_BATTLE_BANK + 217;

    constexpr u16 STR_UI_PKMN_DEX_REGISTERED    = 163;
    constexpr u16 STR_UI_PKMN_SENT_TO_STORAGE   = 164;
    constexpr u16 STR_UI_STORAGE_BOX_FULL       = 165;
    constexpr u16 STR_UI_STORAGE_BOX_PICKED     = 166;
    constexpr u16 STR_UI_STORAGE_ALL_BOXES_FULL = 167;
    constexpr u16 STR_UI_PKMN_RELEASED          = 168;

    constexpr u16 STR_MAP_MY_NEXT_VISIT = 701;
    constexpr u16 STR_MAP_MY_GIFT       = 702;
    constexpr u16 STR_MAP_MY_MAKE_SPACE = 703;
    constexpr u16 STR_MAP_MY_RECEIVE    = 704;

    constexpr u16 STR_MAP_ST_INTRO = 705;
    constexpr u16 STR_MAP_ST_MSG1  = 706;
    constexpr u16 STR_MAP_ST_MSG2  = 707;
    constexpr u16 STR_MAP_ST_MSG3  = 708;
    constexpr u16 STR_MAP_ST_MSG4  = 709;

    constexpr u16 STR_MAP_ICAVE_REPORT_REQS           = 682;
    constexpr u16 STR_MAP_ICAVE_REPORT_REQS_NOT_MET   = 683;
    constexpr u16 STR_MAP_ICAVE_REPORT_REQS_MET_Q     = 684;
    constexpr u16 STR_MAP_ICAVE_REPORT_REQS_MET_AFTER = 685;

    constexpr u16 STR_MAP_FISH_SUCCESS       = 8;
    constexpr u16 STR_MAP_FISH_FAIL          = 9;
    constexpr u16 STR_MAP_FISH_FAIL_OLD_BALL = 5;
    constexpr u16 STR_MAP_FISH_SUCCESSS_PKMN = 6;

    constexpr u16 STR_MAP_FAINT_TO_POKE_CENTER = 417;
    constexpr u16 STR_MAP_FAINT_TO_HOME        = 418;
    constexpr u16 STR_MAP_TRACER_FAIL          = 536;

    constexpr u16 STR_MAP_HM_MESSAGE_DIVE_UP            = 252;
    constexpr u16 STR_MAP_HM_MESSAGE_DIVE_DOWN          = 247;
    constexpr u16 STR_MAP_HM_MESSAGE_CUT                = 243;
    constexpr u16 STR_MAP_HM_MESSAGE_ROCK_SMASH         = 244;
    constexpr u16 STR_MAP_HM_MESSAGE_WHIRLPOOL          = 245;
    constexpr u16 STR_MAP_HM_MESSAGE_SURF               = 246;
    constexpr u16 STR_MAP_HM_MESSAGE_STRENGTH           = 248;
    constexpr u16 STR_MAP_HM_MESSAGE_STRENGTH_ENABLED   = 414;
    constexpr u16 STR_MAP_HM_MESSAGE_ROCK_CLIMB         = 249;
    constexpr u16 STR_MAP_HM_MESSAGE_WATERFALL          = 250;
    constexpr u16 STR_MAP_HM_MESSAGE_HEADBUTT           = 251;
    constexpr u16 STR_MAP_HM_MESSAGE_SWEET_SCENT_FAILED = 89;

    constexpr u16 STR_VF_0_COINS        = 664;
    constexpr u16 STR_VF_1_COIN         = 665;
    constexpr u16 STR_VF_MULT_COINS     = 666;
    constexpr u16 STR_VF_GAME_CLEAR     = 667;
    constexpr u16 STR_VF_OBT_1_COIN     = 668;
    constexpr u16 STR_VF_OBT_MULT_COINS = 669;

    constexpr u16 STR_VF_QUIT       = 670;
    constexpr u16 STR_VF_OPEN_MEMO  = 671;
    constexpr u16 STR_VF_CLOSE_MEMO = 672;

    constexpr u16 STR_VF_VOLTORB_FLIP               = 673;
    constexpr u16 STR_VF_FLIP_FOR_COINS             = 674;
    constexpr u16 STR_VF_MULT                       = 675;
    constexpr u16 STR_VF_GAME_OVER                  = 676;
    constexpr u16 STR_VF_COLLECTED_COINS            = 677;
    constexpr u16 STR_VF_COLLECTED_COINS_THIS_ROUND = 678;
    constexpr u16 STR_VF_QUIT_NOW_1                 = 679;
    constexpr u16 STR_VF_QUIT_NOW_2                 = 680;
    constexpr u16 STR_VF_QUIT_NOW_3                 = 681;
} // namespace IO
