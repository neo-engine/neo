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

    constexpr u16 STR_ANIM_EVOLUTION_START    = 51;
    constexpr u16 STR_ANIM_EVOLUTION_COMPLETE = 52;
    constexpr u16 STR_ANIM_EVOLUTION_ABORTED  = 388;
    constexpr u16 STR_ANIM_EGG_HATCH          = 389;

    constexpr u16 STR_UI_YES    = 80;
    constexpr u16 STR_UI_NO     = 81;
    constexpr u16 STR_UI_SELECT = 323;
    constexpr u16 STR_UI_CANCEL = 330;
    constexpr u16 STR_UI_MONEY  = 471;
    constexpr u16 STR_UI_NONE   = 690;

    constexpr u16 STR_UI_PKMN_STAT_START = 684;
    constexpr u16 STR_UI_PKMN_STAT_HP    = STR_UI_PKMN_STAT_START + 0;
    constexpr u16 STR_UI_PKMN_STAT_ATK   = STR_UI_PKMN_STAT_START + 1;
    constexpr u16 STR_UI_PKMN_STAT_DEF   = STR_UI_PKMN_STAT_START + 2;
    constexpr u16 STR_UI_PKMN_STAT_SAT   = STR_UI_PKMN_STAT_START + 3;
    constexpr u16 STR_UI_PKMN_STAT_SDF   = STR_UI_PKMN_STAT_START + 4;
    constexpr u16 STR_UI_PKMN_STAT_SPD   = STR_UI_PKMN_STAT_START + 5;

    constexpr u16 STR_UI_PKMN_STAT_LONG_START = 127;
    constexpr u16 STR_UI_PKMN_STAT_LONG_HP    = STR_UI_PKMN_STAT_LONG_START + 0;
    constexpr u16 STR_UI_PKMN_STAT_LONG_ATK   = STR_UI_PKMN_STAT_LONG_START + 1;
    constexpr u16 STR_UI_PKMN_STAT_LONG_DEF   = STR_UI_PKMN_STAT_LONG_START + 2;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SAT   = STR_UI_PKMN_STAT_LONG_START + 3;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SDF   = STR_UI_PKMN_STAT_LONG_START + 4;
    constexpr u16 STR_UI_PKMN_STAT_LONG_SPD   = STR_UI_PKMN_STAT_LONG_START + 5;

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

    constexpr u16 STR_UI_MENU_ITEM_NAME_START = 413;
    constexpr u16 STR_UI_MENU_ITEM_PKMN       = STR_UI_MENU_ITEM_NAME_START + 0;
    constexpr u16 STR_UI_MENU_ITEM_POKEDEX    = STR_UI_MENU_ITEM_NAME_START + 1;
    constexpr u16 STR_UI_MENU_ITEM_BAG        = STR_UI_MENU_ITEM_NAME_START + 2;
    constexpr u16 STR_UI_MENU_ITEM_TRAINER_ID = STR_UI_MENU_ITEM_NAME_START + 3;
    constexpr u16 STR_UI_MENU_ITEM_SAVE       = STR_UI_MENU_ITEM_NAME_START + 4;
    constexpr u16 STR_UI_MENU_ITEM_OPTIONS    = STR_UI_MENU_ITEM_NAME_START + 5;

    constexpr u16 STR_UI_USED_ITEM_X_TIMES        = 682;
    constexpr u16 STR_UI_USED_ITEM                = 683;
    constexpr u16 STR_UI_HANDED_OVER_ITEM_X_TIMES = 676;
    constexpr u16 STR_UI_HANDED_OVER_ITEM         = 677;
    constexpr u16 STR_UI_OBTAINED_ITEM_X_TIMES    = 563;
    constexpr u16 STR_UI_OBTAINED_ITEM            = 564;
    constexpr u16 STR_UI_PUT_ITEM_INTO_BAG        = 86;

    constexpr u16 STR_UI_CANNOT_USE_FIELD_ITEM  = 58;
    constexpr u16 STR_UI_CAN_REGISTER_ITEM_TO_Y = 98;

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
    constexpr u16 STR_UI_BAG_ITEM_USED          = 51;
    constexpr u16 STR_UI_BAG_ITEM_WASTED        = 53;
    constexpr u16 STR_UI_BAG_PKMN_CARRIES_ITEM  = 54;
    constexpr u16 STR_UI_BAG_CHOOSE_ITEM        = 56;
    constexpr u16 STR_UI_BAG_ITEM_GIVE          = 44;
    constexpr u16 STR_UI_BAG_ITEM_APPLY         = 45;
    constexpr u16 STR_UI_BAG_ITEM_REGISTER      = 46;
    constexpr u16 STR_UI_BAG_ITEM_USE           = 47;
    constexpr u16 STR_UI_BAG_ITEM_TOSS          = 48;
    constexpr u16 STR_UI_BAG_ITEM_SELL          = 483;
    constexpr u16 STR_UI_BAG_CANNOT_BUY_ITEM    = 482;
    constexpr u16 STR_UI_BAG_WILL_BUY_ITEM_FOR  = 484;
    constexpr u16 STR_UI_BAG_ITEM_TOSS_CONFIRM  = 485;
    constexpr u16 STR_UI_BAG_ITEM_TOSS_ASK_QTY  = 698;
    constexpr u16 STR_UI_BAG_ITEM_SELL_ASK_QTY  = 699;

} // namespace IO
