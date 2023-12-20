/*
Pokémon neo
------------------------------

file        : defines.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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

namespace IO {

    // UI strings
    constexpr u16 STR_UI_TOUCH_TO_START = 70;

    constexpr u16 STR_UI_CHOOSE_A_DIFFERENT_LANGUAGE = 85; // States current language and
                                                           // asks if the player wishes to
                                                           // change the language

    constexpr u16 STR_UI_PICK_A_LANGUAGE             = 109; // "Pick a language."
    constexpr u16 STR_UI_OVERWRITE_EXISTING_SAVEDATA = 79;

    constexpr u16 STR_ANIM_EVOLUTION_START    = 51;
    constexpr u16 STR_ANIM_EVOLUTION_COMPLETE = 52;
    constexpr u16 STR_ANIM_EVOLUTION_ABORTED  = 388;
    constexpr u16 STR_ANIM_EGG_HATCH          = 389;

    constexpr u16 STR_ANIM_TRADE_SEND1 = 712;
    constexpr u16 STR_ANIM_TRADE_SEND2 = 713;
    constexpr u16 STR_ANIM_TRADE_RECV1 = 714;
    constexpr u16 STR_ANIM_TRADE_RECV2 = 715;

    constexpr u16 STR_UI_CONTINUE        = 71;
    constexpr u16 STR_UI_NEW_GAME        = 72;
    constexpr u16 STR_UI_SPECIAL_EPISODE = 73;
    constexpr u16 STR_UI_TRANSFER_GAME   = 74;
    constexpr u16 STR_UI_MYSTERY_GIFT    = 404;

    constexpr u16 STR_UI_YES       = 80;
    constexpr u16 STR_UI_NO        = 81;
    constexpr u16 STR_UI_SELECT    = 323;
    constexpr u16 STR_UI_STATUS    = 324;
    constexpr u16 STR_UI_GIVE_ITEM = 325;
    constexpr u16 STR_UI_TAKE_ITEM = 326;
    constexpr u16 STR_UI_USE_ITEM  = 327;
    constexpr u16 STR_UI_SWAP      = 328;
    constexpr u16 STR_UI_DEX_ENTRY = 329;
    constexpr u16 STR_UI_DESELECT  = 331;
    constexpr u16 STR_UI_SEND_IN   = 151;
    constexpr u16 STR_UI_CANCEL    = 330;
    constexpr u16 STR_UI_MONEY     = 471;
    constexpr u16 STR_UI_NONE      = 690;

    constexpr u16 STR_UI_RECEIVE_GIFT  = 716;
    constexpr u16 STR_UI_CHECK_WC      = 717;
    constexpr u16 STR_UI_WELCOME_TO_MG = 718;

    constexpr u16 STR_UI_OBTAIN_FRIEND   = 719;
    constexpr u16 STR_UI_OBTAIN_WIRELESS = 720;
    constexpr u16 STR_UI_OBTAIN_INTERNET = 721;
    constexpr u16 STR_UI_OBTAIN_SEL_MSG  = 722;
    constexpr u16 STR_UI_NO_SPACE        = 723;

    constexpr u16 STR_UI_SEARCHING_FOR_GIFT     = 724;
    constexpr u16 STR_UI_NO_GIFT_FOUND          = 725;
    constexpr u16 STR_UI_ACCEPT_GIFT            = 726;
    constexpr u16 STR_UI_ACCEPT                 = 727;
    constexpr u16 STR_UI_DECLINE                = 728;
    constexpr u16 STR_UI_GIFT_DOWNLOADING       = 729;
    constexpr u16 STR_UI_GIFT_ALREADY_COLLECTED = 730;
    constexpr u16 STR_UI_GIFT_RECEIVED          = 731;
    constexpr u16 STR_UI_THANK_YOU_FOR_PLAYING  = 738;

    constexpr u16 STR_UI_PLEASE_COLLECT_GIFT = 732;
    constexpr u16 STR_UI_WONDERCARD          = 733;
    constexpr u16 STR_UI_DATE_RECEIVED       = 734;

    constexpr u16 STR_UI_WC_FLIP = 735;
    constexpr u16 STR_UI_WC_SEND = 736;
    constexpr u16 STR_UI_WC_TOSS = 737;

    constexpr u16 STR_UI_EP0_TEXT_START = 111;
    constexpr u16 STR_UI_EP0_TEXT_END   = 113;

    constexpr u16 STR_UI_INIT_GAME_TEXT0             = 441;
    constexpr u16 STR_UI_INIT_GAME_TEXT1             = 442;
    constexpr u16 STR_UI_INIT_GAME_TEXT2             = 446;
    constexpr u16 STR_UI_INIT_GAME_CHOOSE_APPEARANCE = 457;
    constexpr u16 STR_UI_INIT_GAME_CHOOSE_NAME       = 458;
    constexpr u16 STR_UI_INIT_GAME_CHARACTER_OK      = 459;
    constexpr u16 STR_UI_INIT_GAME_DEFAULT_NAME0     = 460;
    constexpr u16 STR_UI_INIT_GAME_DEFAULT_NAME1     = 461;
    constexpr u16 STR_UI_INIT_GAME_TEXT3             = 462;
    constexpr u16 STR_UI_INIT_GAME_TEXT4             = 463;
    constexpr u16 STR_UI_INIT_GAME_TEXT5             = 464;

    constexpr u16 STR_UI_PKMN_STAT_START = 684;
    constexpr u16 STR_UI_PKMN_STAT_HP    = STR_UI_PKMN_STAT_START + 0;
    constexpr u16 STR_UI_PKMN_STAT_ATK   = STR_UI_PKMN_STAT_START + 1;
    constexpr u16 STR_UI_PKMN_STAT_DEF   = STR_UI_PKMN_STAT_START + 2;
    constexpr u16 STR_UI_PKMN_STAT_SAT   = STR_UI_PKMN_STAT_START + 3;
    constexpr u16 STR_UI_PKMN_STAT_SDF   = STR_UI_PKMN_STAT_START + 4;
    constexpr u16 STR_UI_PKMN_STAT_SPD   = STR_UI_PKMN_STAT_START + 5;

    constexpr u16 STR_UI_PKMN_STAT_LONG_START = 126;
    constexpr u16 STR_UI_PKMN_STAT_LONG_HP    = STR_UI_PKMN_STAT_LONG_START + 0;
    constexpr u16 STR_UI_PKMN_STAT_LONG_ATK   = STR_UI_PKMN_STAT_LONG_START + 1;
    constexpr u16 STR_UI_PKMN_STAT_LONG_DEF   = STR_UI_PKMN_STAT_LONG_START + 2;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SAT   = STR_UI_PKMN_STAT_LONG_START + 3;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SDF   = STR_UI_PKMN_STAT_LONG_START + 4;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SPD   = STR_UI_PKMN_STAT_LONG_START + 5;

    constexpr u16 STR_UI_PKMN_ALREADY_KNOWS_MOVE    = 102;
    constexpr u16 STR_UI_PKMN_LEARNED_MOVE          = 103;
    constexpr u16 STR_UI_PKMN_TRIES_TO_LEARN_MOVE   = 139;
    constexpr u16 STR_UI_PKMN_ALREADY_KNOWS_4_MOVES = 104;
    constexpr u16 STR_UI_PKMN_CANT_FORGET_MOVE      = 106;
    constexpr u16 STR_UI_PKMN_CANT_LEARN_MOVE       = 107;
    constexpr u16 STR_UI_PKMN_DIDNT_LEARN_MOVE      = 403;

    constexpr u16 STR_UI_JBOX_LUDICOBOX     = 585;
    constexpr u16 STR_UI_JBOX_CHOOSE_SONG   = 647;
    constexpr u16 STR_UI_JBOX_STOP_PLAYBACK = 646;
    constexpr u16 STR_UI_JBOX_CHOOSE_RECORD = 700;

    constexpr u16 STR_UI_WOULD_YOU_LIKE_TO_SAVE = 92;
    constexpr u16 STR_UI_SAVING_A_LOT_OF_DATA   = 93;
    constexpr u16 STR_UI_SAVING_COMPLETE        = 94;
    constexpr u16 STR_UI_SAVING_FAILED          = 95;

    constexpr u16 STR_UI_MONEYTYPE_START = 471;
    constexpr u16 STR_UI_MONEYTYPE_MONEY = STR_UI_MONEYTYPE_START + 0;
    constexpr u16 STR_UI_MONEYTYPE_BP    = STR_UI_MONEYTYPE_START + 1;
    constexpr u16 STR_UI_MONEYTYPE_COINS = STR_UI_MONEYTYPE_START + 2;
    constexpr u16 STR_UI_MONEYTYPE_ASH   = 554;

    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_START = 479;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_MONEY = STR_UI_NOT_ENOUGH_MONEYTYPE_START + 0;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_BP    = STR_UI_NOT_ENOUGH_MONEYTYPE_START + 1;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_COINS = STR_UI_NOT_ENOUGH_MONEYTYPE_START + 2;
    constexpr u16 STR_UI_NOT_ENOUGH_MONEYTYPE_ASH   = 555;

    constexpr u16 STR_UI_MART_CLERK_HOW_MANY              = 475;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START = 476;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_MONEY
        = STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + 0;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_BP
        = STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + 1;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_COINS
        = STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + 2;
    constexpr u16 STR_UI_MART_CLERK_TOTAL_MONEYTYPE_ASH = 556;

    constexpr u16 STR_UI_ITEMCOUNT_IN_BAG = 474; // "In Bag: %lu"

    constexpr u16 STR_UI_ASH_COUNT  = 557;
    constexpr u16 STR_UI_COIN_COUNT = 68;
    constexpr u16 STR_UI_BP_COUNT   = 69;

    constexpr u16 STR_UI_MENU_ITEM_NAME_START = 413;
    constexpr u16 STR_UI_MENU_ITEM_PKMN       = STR_UI_MENU_ITEM_NAME_START + 0;
    constexpr u16 STR_UI_MENU_ITEM_POKEDEX    = STR_UI_MENU_ITEM_NAME_START + 1;
    constexpr u16 STR_UI_MENU_ITEM_BAG        = STR_UI_MENU_ITEM_NAME_START + 2;
    constexpr u16 STR_UI_MENU_ITEM_TRAINER_ID = STR_UI_MENU_ITEM_NAME_START + 3;
    constexpr u16 STR_UI_MENU_ITEM_SAVE       = STR_UI_MENU_ITEM_NAME_START + 4;
    constexpr u16 STR_UI_MENU_ITEM_OPTIONS    = STR_UI_MENU_ITEM_NAME_START + 5;

    constexpr u16 STR_UI_KEY_BACKSPACE = 439;
    constexpr u16 STR_UI_KEY_ENTER     = 440;

    constexpr u16 STR_UI_USED_ITEM_X_TIMES        = 682;
    constexpr u16 STR_UI_USED_ITEM                = 683;
    constexpr u16 STR_UI_HANDED_OVER_ITEM_X_TIMES = 676;
    constexpr u16 STR_UI_HANDED_OVER_ITEM         = 677;
    constexpr u16 STR_UI_OBTAINED_ITEM_X_TIMES    = 563;
    constexpr u16 STR_UI_OBTAINED_ITEM            = 564;
    constexpr u16 STR_UI_PUT_ITEM_INTO_BAG        = 86;

    constexpr u16 STR_ITEM_TRACER_NO_CHARGE      = 695;
    constexpr u16 STR_ITEM_REPEL_ACTIVATED       = 63;
    constexpr u16 STR_ITEM_SUPER_REPEL_ACTIVATED = 64;
    constexpr u16 STR_ITEM_MAX_REPEL_ACTIVATED   = 65;
    constexpr u16 STR_ITEM_EXP_ALL_ACTIVATED     = 66;
    constexpr u16 STR_ITEM_EXP_ALL_DEACTIVATED   = 67;

    constexpr u16 STR_UI_CANNOT_USE_FIELD_ITEM  = 58;
    constexpr u16 STR_UI_CAN_REGISTER_ITEM_TO_Y = 98;

    constexpr u16 STR_UI_STS_TOOK_ITEM_FROM_PKMN      = 101;
    constexpr u16 STR_UI_STS_GAVE_ITEM_TO_PKMN        = 334;
    constexpr u16 STR_UI_STS_CHOOSE_THESE_PKMN        = 335;
    constexpr u16 STR_UI_STS_CHOOSE_THIS_PKMN         = 336;
    constexpr u16 STR_UI_STS_NATURE_START             = 187;
    constexpr u16 STR_UI_STS_PERSONALITY_START        = 212;
    constexpr u16 STR_UI_STS_FLY_WHERE                = 701;
    constexpr u16 STR_UI_STS_SHEEN                    = 702;
    constexpr u16 STR_UI_STS_CONDITION                = 703;
    constexpr u16 STR_UI_STS_NO_RIBBONS               = 704;
    constexpr u16 STR_UI_STS_RIBBONS                  = 705;
    constexpr u16 STR_UI_STS_SPECIES_IDX              = 337;
    constexpr u16 STR_UI_STS_SPECIES_NAME             = 338;
    constexpr u16 STR_UI_STS_OT                       = 339;
    constexpr u16 STR_UI_STS_ID                       = 340;
    constexpr u16 STR_UI_STS_EXP                      = 341;
    constexpr u16 STR_UI_STS_NEXT                     = 342;
    constexpr u16 STR_UI_STS_LEVEL                    = 343;
    constexpr u16 STR_UI_STS_PKMN_INFO                = 344;
    constexpr u16 STR_UI_STS_FROM                     = 356;
    constexpr u16 STR_UI_STS_IN_AT                    = 357;
    constexpr u16 STR_UI_STS_FROM_PERSON              = 358;
    constexpr u16 STR_UI_STS_NATURE_PREFIX            = 364;
    constexpr u16 STR_UI_STS_NATURE_SUFFIX            = 365;
    constexpr u16 STR_UI_STS_FATEFUL_ENC              = 367;
    constexpr u16 STR_UI_STS_FATEFUL_ENC_AT_LEVEL     = 372;
    constexpr u16 STR_UI_STS_APP_FATEFUL_ENC_AT_LEVEL = 373;
    constexpr u16 STR_UI_STS_MET_AT_LEVEL             = 370;
    constexpr u16 STR_UI_STS_APP_MET_AT_LEVEL         = 371;
    constexpr u16 STR_UI_STS_EGG_RECEIVED             = 366;
    constexpr u16 STR_UI_STS_EGG_HATCHED              = 368;
    constexpr u16 STR_UI_STS_EGG_APP_HATCHED          = 369;
    constexpr u16 STR_UI_STS_MANA_EGG_DSCR_LINE1      = 359;
    constexpr u16 STR_UI_STS_MANA_EGG_DSCR_LINE2      = 360;
    constexpr u16 STR_UI_STS_MANA_EGG_DSCR_LINE3      = 361;
    constexpr u16 STR_UI_STS_EGG_DSCR_LINE1           = 354;
    constexpr u16 STR_UI_STS_EGG_DSCR_LINE2           = 355;
    constexpr u16 STR_UI_STS_EGG_STATUS_1_0           = 345;
    constexpr u16 STR_UI_STS_EGG_STATUS_1_1           = 346;
    constexpr u16 STR_UI_STS_EGG_STATUS_1_2           = 347;
    constexpr u16 STR_UI_STS_EGG_STATUS_2_0           = 348;
    constexpr u16 STR_UI_STS_EGG_STATUS_2_1           = 349;
    constexpr u16 STR_UI_STS_EGG_STATUS_2_2           = 350;
    constexpr u16 STR_UI_STS_EGG_STATUS_3_0           = 351;
    constexpr u16 STR_UI_STS_EGG_STATUS_3_1           = 352;
    constexpr u16 STR_UI_STS_EGG_STATUS_3_2           = 353;
    constexpr u16 STR_UI_STS_ITEM                     = 362;
    constexpr u16 STR_UI_STS_ABITLITY                 = 363;
    constexpr u16 STR_UI_STS_LIKES                    = 374;
    constexpr u16 STR_UI_STS_LIKES_SUFFIX             = 375;
    constexpr u16 STR_UI_STS_POKEBLOCK                = 376;
    constexpr u16 STR_UI_STS_TASTES_START             = 242;
    constexpr u16 STR_UI_STS_PP_FORMAT                = 377;

    constexpr u16 STR_UI_BAG_PAGE_NAME_START    = 11;
    constexpr u16 STR_UI_BAG_PAGE_NAME_ITEMS    = STR_UI_BAG_PAGE_NAME_START + 0;
    constexpr u16 STR_UI_BAG_PAGE_NAME_MEDICINE = STR_UI_BAG_PAGE_NAME_START + 1;
    constexpr u16 STR_UI_BAG_PAGE_NAME_TM_HM    = STR_UI_BAG_PAGE_NAME_START + 2;
    constexpr u16 STR_UI_BAG_PAGE_NAME_BERRIES  = STR_UI_BAG_PAGE_NAME_START + 3;
    constexpr u16 STR_UI_BAG_PAGE_NAME_KEY_ITEM = STR_UI_BAG_PAGE_NAME_START + 4;
    constexpr u16 STR_UI_BAG_NUMBER             = 575; // "No."
    constexpr u16 STR_UI_BAG_TYPE               = 29;  // "Type" (for TM/HM move type)
    constexpr u16 STR_UI_BAG_CATEGORY           = 30;  // "Categ." (for TM/HM move category)
    constexpr u16 STR_UI_BAG_PP                 = 31;  // "PP" (for TM/HM move PP)
    constexpr u16 STR_UI_BAG_POWER              = 390;
    constexpr u16 STR_UI_BAG_ACCURACY           = 391;
    constexpr u16 STR_UI_BAG_ACCURACY_NO_MISS   = 392;
    constexpr u16 STR_UI_BAG_PARTY_EGG          = 34;
    constexpr u16 STR_UI_BAG_PARTY_NO_ITEM      = 42;
    constexpr u16 STR_UI_BAG_ALREADY_LEARNED    = 35;
    constexpr u16 STR_UI_BAG_LEARN_POSSIBLE     = 36;
    constexpr u16 STR_UI_BAG_LEARN_NOT_POSSIBLE = 37;
    constexpr u16 STR_UI_BAG_POSSIBLE           = 40;
    constexpr u16 STR_UI_BAG_NOT_POSSIBLE       = 41;
    constexpr u16 STR_UI_BAG_CHOOSE_PKMN        = 697;
    constexpr u16 STR_UI_BAG_ASK_ITEM_ACTION    = 57;
    constexpr u16 STR_UI_BAG_CHOOSE_MOVE        = 49;
    constexpr u16 STR_UI_BAG_EMPTY              = 43;
    constexpr u16 STR_UI_BAG_ITEM_USED          = 50;
    constexpr u16 STR_UI_BAG_ITEM_WASTED        = 53;
    constexpr u16 STR_UI_BAG_PKMN_CARRIES_ITEM  = 54;
    constexpr u16 STR_UI_BAG_CHOOSE_ITEM        = 56;
    constexpr u16 STR_UI_BAG_ITEM_GIVE          = 44;
    constexpr u16 STR_UI_BAG_ITEM_APPLY         = 45;
    constexpr u16 STR_UI_BAG_ITEM_REGISTER      = 46;
    constexpr u16 STR_UI_BAG_ITEM_USE           = 47;
    constexpr u16 STR_UI_BAG_ITEM_TOSS          = 48;
    constexpr u16 STR_UI_BAG_ITEM_SELL          = 483;
    constexpr u16 STR_UI_BAG_ITEM_DETAILS       = 401;
    constexpr u16 STR_UI_BAG_ITEM_DEREGISTER    = 402;
    constexpr u16 STR_UI_BAG_CANNOT_BUY_ITEM    = 482;
    constexpr u16 STR_UI_BAG_WILL_BUY_ITEM_FOR  = 484;
    constexpr u16 STR_UI_BAG_ITEM_TOSS_CONFIRM  = 485;
    constexpr u16 STR_UI_BAG_ITEM_TOSS_ASK_QTY  = 698;
    constexpr u16 STR_UI_BAG_ITEM_SELL_ASK_QTY  = 699;

    constexpr u16 STR_UI_BATTLE_WILD_PKMN_FLED         = 170;
    constexpr u16 STR_UI_BATTLE_GOT_AWAY_SAFELY        = 163;
    constexpr u16 STR_UI_BATTLE_COULDNT_ESCAPE         = 164;
    constexpr u16 STR_UI_BATTLE_USE_ITEM_TO_ACT_FIRST  = 169;
    constexpr u16 STR_UI_BATTLE_PASS_ITEM_SPECIAL      = 281;
    constexpr u16 STR_UI_BATTLE_PASS_ITEM_PROTECT_PKMN = 282;
    constexpr u16 STR_UI_BATTLE_NO_EFFECT_ON           = 284;
    constexpr u16 STR_UI_BATTLE_IT_FAILED              = 304;

    constexpr u16 STR_UI_BATTLE_PREPARE_SHELL_TRAP  = 269;
    constexpr u16 STR_UI_BATTLE_PREPARE_FOCUS_PUNCH = 270;
    constexpr u16 STR_UI_BATTLE_PREPARE_BEAK_BLAST  = 271;

    constexpr u16 STR_UI_BATTLE_DRINK_ITEM              = 539;
    constexpr u16 STR_UI_BATTLE_ACTIVATE_ITEM           = 537;
    constexpr u16 STR_UI_BATTLE_EAT_ITEM                = 279;
    constexpr u16 STR_UI_BATTLE_EAT_ITEM_HEAL_CONFUSION = 278;
    constexpr u16 STR_UI_BATTLE_ACTIVATE_AIR_BALLON     = 538;

    constexpr u16 STR_UI_BATTLE_HARMED_BY_NIGHTMARE = 536;
    constexpr u16 STR_UI_BATTLE_HEALED_BY_AQUA_RING = 535;
    constexpr u16 STR_UI_BATTLE_HEALED_BY_INGRAIN   = 534;
    constexpr u16 STR_UI_BATTLE_HARMED_BY_CURSE     = 533;

    constexpr u16 STR_UI_BATTLE_MEGA_EVOLVE_WISH         = 307;
    constexpr u16 STR_UI_BATTLE_MEGA_EVOLVE_WISH_GRANTED = 308;

    constexpr u16 STR_UI_BATTLE_TERRAIN_NO_CHANGE = 140;

    constexpr u16 STR_UI_BATTLE_CONFUSION_HEALED = 294;

    constexpr u16 STR_UI_BATTLE_WIN_MONEY  = 552;
    constexpr u16 STR_UI_BATTLE_LOSE_MONEY = 553;

    constexpr u16 STR_UI_BATTLE_EXP_GAINED    = 167;
    constexpr u16 STR_UI_BATTLE_ADVANCE_LEVEL = 168;

    constexpr u16 STR_UI_BATTLE_TRAINER_USED_ITEM = 551;
    constexpr u16 STR_UI_BATTLE_PLAYER_USED_ITEM  = 50;

    constexpr u16 STR_UI_BATTLE_NOTHING_HAPPENED = 171;

    constexpr u16 STR_UI_BATTLE_PKMN_CAPTURE_SUCC              = 486;
    constexpr u16 STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_0 = 487;
    constexpr u16 STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_1 = 488;
    constexpr u16 STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_2 = 489;
    constexpr u16 STR_UI_BATTLE_PKMN_CAPTURE_FAIL_AFTER_TICK_3 = 490;

    constexpr u16 STR_UI_PKMN_DEX_REGISTERED    = 174;
    constexpr u16 STR_UI_PKMN_SENT_TO_STORAGE   = 175;
    constexpr u16 STR_UI_STORAGE_BOX_FULL       = 176;
    constexpr u16 STR_UI_STORAGE_BOX_PICKED     = 177;
    constexpr u16 STR_UI_STORAGE_ALL_BOXES_FULL = 178;
    constexpr u16 STR_UI_PKMN_RELEASED          = 179;

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

    constexpr u16 STR_MAP_FAINT_TO_POKE_CENTER = 561;
    constexpr u16 STR_MAP_FAINT_TO_HOME        = 562;
    constexpr u16 STR_MAP_TRACER_FAIL          = 696;

    constexpr u16 STR_MAP_HM_MESSAGE_DIVE_UP            = 322;
    constexpr u16 STR_MAP_HM_MESSAGE_DIVE_DOWN          = 317;
    constexpr u16 STR_MAP_HM_MESSAGE_CUT                = 313;
    constexpr u16 STR_MAP_HM_MESSAGE_ROCK_SMASH         = 314;
    constexpr u16 STR_MAP_HM_MESSAGE_WHIRLPOOL          = 315;
    constexpr u16 STR_MAP_HM_MESSAGE_SURF               = 316;
    constexpr u16 STR_MAP_HM_MESSAGE_STRENGTH           = 318;
    constexpr u16 STR_MAP_HM_MESSAGE_STRENGTH_ENABLED   = 558;
    constexpr u16 STR_MAP_HM_MESSAGE_ROCK_CLIMB         = 319;
    constexpr u16 STR_MAP_HM_MESSAGE_WATERFALL          = 320;
    constexpr u16 STR_MAP_HM_MESSAGE_HEADBUTT           = 321;
    constexpr u16 STR_MAP_HM_MESSAGE_SWEET_SCENT_FAILED = 90;
} // namespace IO
