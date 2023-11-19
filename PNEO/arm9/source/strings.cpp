/*
Pok\xe9mon neo
------------------------------

file        : strings.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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
#include "fs/data.h"
#include "save/saveGame.h"

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

namespace FS {
    const char* EPISODE_NAMES[ SAVE::MAX_SPECIAL_EPISODES + 1 ][ MAX_LANGUAGES ]
        = { { "Dev's Heaven", "Dev's Heaven" } };

    const char* LANGUAGE_NAMES[ MAX_LANGUAGES ] = { "English", "Deutsch" };

    const char* HP_ICONS[ MAX_LANGUAGES ] = { "\xd", "\xe" };

#ifdef DESQUID
    const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ MAX_LANGUAGES ] = {
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
        { "Squids are evil. Eat them!" },
        { "PKMN" },
        { "Item Spawn" },
        { "Flags" },

        // 50

        { "Warp" },
        { "Time" },
        { "Btl Trainer" },
        { "Edit Badges" },
        { "All Badges" },

        { "Edit Route" },
        { "Set Dawn" },
        { "Set Day" },
        { "Set Dusk" },
        { "Set Night" },

        // 60

        { "Playtime +10" },
        { "Default Team" },
        { "Repel 9999" },
        { "Repel Off" },
        { "Fill Boxes" },

        { "No Route" },
        { "Route 1 (Std)" },
        { "Route 2 (Aqu)" },
        { "Route 3 (Mgm)" },
        { "Route 4 (Non)" },
    };

#endif

    const char* const MONTHS[ 12 ][ MAX_LANGUAGES ]
        = { { "Jan.", "Jan." }, { "Feb.", "Feb." }, { "Mar.", "M\xe4r." }, { "Apr.", "Apr." },
            { "May", "Mai" },   { "June", "Juni" }, { "July", "Juli" },    { "Aug.", "Aug." },
            { "Sep.", "Sep." }, { "Oct.", "Okt." }, { "Nov.", "Nov." },    { "Dec.", "Dez." } };
} // namespace FS
