/*
Pokémon Emerald 2 Version
------------------------------

file        : strings.cpp
author      : Philip Wellnitz
description :

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

#include "defines.h"

const char * const STRINGS[ MAX_STRINGS ][ LANGUAGES ] = {
    { "No data.", "Keine Daten." },
    { "Faraway Place", "Entfernter Ort" },
    { "Unknown", "Unbekannt" },
    { "%s\nUse %s?", "%s\nMöchtest du %s nutzen?" },
    { "The repel wore off.","Der Schutz ist aufgebraucht." },

    { "Just an old Poké Ball…", "Doch nur ein alter Pokéball…" },
    { "You fished a Pokémon!", "Du hast ein Pokémon geangelt!" },
    { "Map border.\nReturn, or you\nwill get lost!", "Ende der Kartendaten.\nKehr um, sonst\nverirrst du dich!" },
    { "\nSomething's on the hook!", "\nDa ist etwas am Haken!" },
    { "It disappeared…", "Es ist entkommen…" },

    // 10

    { "%s used %s!", "%s setzt %s ein!" },
    { "Items", "Items" },
    { "Medicine", "Medizin" },
    { "TM/HM", "TM/VM" },
    { "Berries", "Beeren" },

    { "Key Items", "Basis-Items" },
    { "Firmness: %s", "Güte: %s" },
    { "Hard", "Hart" },
    { "Soft", "Weich" },
    { "Super hard", "Steinhart" },

    // 20

    { "Super soft", "Sehr weich" },
    { "Very hard", "Sehr hart" },
    { "Normal", "Normal" },
    { "Size:%4.1fcm", "Größe:%4.1fcm" },
    { "Spicy", "Scharf" },

    { "Dry", "Trocken"},
    { "Sweet", "Süß" },
    { "Bitter", "Bitter" },
    { "Sour", "Sauer" },
    { "Type", "Typ" },

    // 30

    { "Categ.", "Kateg." },
    { "PP", "AP" },
    { "Strength", "Stärke" },
    { "Accuracy", "Genauigkeit" },
    { "Egg", "Ei" },

    { "Already\nlearned", "Bereits\nerlernt" },
    { "Possible", "Erlernbar" },
    { "Not\npossible", "Nicht\nerlernbar" },
    { "Level %3d\n%3d/%3d HP", "Level %3d\n%3d/%3d KP" },
    { "Level %3d\n Fainted", "Level %3d\n Besiegt" },

    // 40

    { "Possible", "Möglich" },
    { "Not\npossible", "Nicht\nmöglich" },
    { "No item", "Kein Item" },
    { "No items", "Keine Items" },
    { "Give", "Geben" },

    { "Apply", "Anwenden" },
    { "Register", "Registrieren" },
    { "Use", "Einsetzen" },
    { "Trash", "Wegwerfen" },
    { "Which move?", "Welche Attacke?" },

    // 50

    { "Used %s on %s.", "%s auf %s angewendet." },
    { "%s is evolving…", "%s entwickelt sich…" },
    { "…and became a(n)\n%s!", "…und wurde zu einem\n%s!" },
    { "It would be\nwasted…", "Es würde keine\nWirkung haben…" },
    { "%s holds\n%s already.\nSwap the items?", "%s trägt bereits\ndas Item %s.\nSollen die Items getauscht werden?" },

    { "You can't choose\nthis item.", "Dieses Item kann nicht\nausgewählt werden." },
    { "Choose %s?", "%s auswählen?" },
    { "What to do with %s?", "Was tun mit %s?" },
    { "You can't use\nthat right now.", "Das kann jetzt nicht\neingesetzt werden." },
    { "Pokémon team", "Pokémon-Team" },

    // 60

    { "Cache", "Zwischenablage" },
    { "All boxes", "Alle Boxen" },
    { "New name for box “%s”", "Name für Box „%s“" },
    { "Repel activated.", "Schutz eingesetzt." },
    { "Super Repel activated.", "Superschutz eingesetzt." },

    { "Max Repel activated.", "Top-Schutz eingesetzt." },
    { "Exp. All deactivated.", "EP-Teiler ausgeschaltet." },
    { "Exp. All activated.", "EP-Teiler eingeschaltet." },
    { "Coins: %lu.", "Münzen: %lu." },
    { "BP: %lu.", "Kampfpunkte: %lu." },

    // 70

    { "         TOUCH TO START", "     BER\x9A""HRE, UM ZU STARTEN" }
};