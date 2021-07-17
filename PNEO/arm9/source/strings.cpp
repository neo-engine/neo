/*
Pok\xe9mon neo
------------------------------

file        : strings.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2021
Philip Wellnitz

tHis file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pok\xe9mon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "defines.h"
#include "saveGame.h"

// ae \xe4
// e´ \xe9
// oe \xf6
// ue \xfc

// AE \xc4
// E´ \xc9
// OE \xd6
// UE \xdc
// ss \xdf
// { (male)
// } (female)
//

// ,, [131]
// '' [129]
// ´´ \"

const char* SAVE::EPISODE_NAMES[ MAX_SPECIAL_EPISODES + 1 ][ LANGUAGES ]
    = { { "Dev's Heaven", "Dev's Heaven" } };

const char* LANGUAGE_NAMES[ LANGUAGES ] = { "English", "Deutsch" };

const char* HP_ICONS[ LANGUAGES ] = { "\xd", "\xe" };

#ifdef DESQUID
const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ LANGUAGES ] = {
    { "Desquid" },
    { "Basic Data" },
    { "Status" },
    { "Egg Status" },
    { "Nature" },

    { "HP/PP" },
    { "Moves/Item" },
    { "IV" },
    { "EV" },
    { "Duplicate" },

    // 10

    { "Delete" },
    { "Heal" },
    { "Species: %s" },
    { "Forme: %s" },
    { "Ability: %s" },

    { "Sleep" },
    { "Poison" },
    { "Burn" },
    { "Freeze" },
    { "Paralyzed" },

    // 20

    { "Toxic" },
    { "Level" },
    { "EXP" },
    { "Shininess" },
    { "Is Egg?" },

    { "Steps/Happiness" },
    { "Obt. at %s" },
    { "Hatd at %s" },
    { "OT Id" },
    { "OT SId" },

    // 30

    { "Nature: %s" },
    { "%2s Attack" },
    { "%2s Defense" },
    { "%2s Special Atk" },
    { "%2s Special Def" },

    { "%2s Speed" },
    { "HP" },
    { "Attack" },
    { "Defense" },
    { "Special Attack" },

    // 40

    { "Special Defense" },
    { "Speed" },
    { "%s" },
    { "Held: %s" },
    { "PP(%s)" },

    { "Fateful Enc." },
    { "PKMN Spawn" },
    { "Item Spawn" },
    { "View Boxes" },
    { "Tokens of god-being|" },

    // 50

    { "RND Badge" },
    { "Init PKMN" },
    { "Pok\xe9Mart" },
};

#endif

const char* const MONTHS[ 12 ][ LANGUAGES ]
    = { { "Jan.", "Jan." }, { "Feb.", "Feb." }, { "Mar.", "M\xe4r." }, { "Apr.", "Apr." },
        { "May", "Mai" },   { "June", "Juni" }, { "July", "Juli" },    { "Aug.", "Aug." },
        { "Sep.", "Sep." }, { "Oct.", "Okt." }, { "Nov.", "Nov." },    { "Dec.", "Dez." } };
