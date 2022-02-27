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

// UI strings

constexpr u16 STR_ANIM_EVOLUTION_START    = 51;
constexpr u16 STR_ANIM_EVOLUTION_COMPLETE = 52;
constexpr u16 STR_ANIM_EVOLUTION_ABORTED  = 388;
constexpr u16 STR_ANIM_EGG_HATCH          = 389;

constexpr u16 STR_UI_YES = 80;
constexpr u16 STR_UI_NO  = 81;

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
