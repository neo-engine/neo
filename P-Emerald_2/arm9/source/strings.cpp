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
#include "saveGame.h"

const char* const SAVE::CHAPTER_NAMES[ 2 * MAX_CHAPTERS ][ LANGUAGES ] = {
    { "Prologue", "Prolog" },
    { "I Choose You!", "Ich Wähle Dich!" },
    { "Chapter 1", "Kapitel 1" },
    { "(tba)", "(tba)" },
    { "Chapter 2", "Kapitel 2" },
    { "(tba)", "(tba)" },
    { "Chapter 3", "Kapitel 3" },
    { "(tba)", "(tba)" },
    { "Chapter 4", "Kapitel 4" },
    { "(tba)", "(tba)" },

    { "Chapter 5", "Kapitel 5" },
    { "(tba)", "(tba)" },
    { "Chapter 6", "Kapitel 6" },
    { "(tba)", "(tba)" },
    { "Chapter 7", "Kapitel 7" },
    { "(tba)", "(tba)" },
    { "Chapter 8", "Kapitel 8" },
    { "(tba)", "(tba)" },
    { "Chapter 9", "Kapitel 9" },
    { "(tba)", "(tba)" },

    { "Final", "Letztes" },
    { "Pokémon", "Pokémon" },
    { "Epilogue", "Epilog" },
    { "Life Goes On", "Das Leben Geht Weiter" },
};

const char* SAVE::EPISODE_NAMES[ LANGUAGES ][ MAX_SPECIAL_EPISODES ] = {
    { 
        "Dev's Heaven"
    }, {
        "Dev's Heaven"
    }
};

const char * LANGUAGE_NAMES[ LANGUAGES ] = {
    "English",
    "Deutsch"
};

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
    { "%s holds\n%s already.\nSwap the items?", "%s trägt bereits\ndas Item %s.\nSollen die Items getauscht werden?" }, //TODO

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

    { "         TOUCH TO START", "     BER\x9A""HRE, UM ZU STARTEN" },
    { "Continue", "Weiter" },
    { "New Game", "Neues Spiel" },
    { "Special Episodes", "Spezialepisoden" },
    { "Import Save", "Spielstand importieren" },

    { "Continue your adventure", "Setze dein Abenteuer fort" },
    { "Start a new adventure", "Starte ein neues Abenteuer" },
    { "Learn your companion's stories", "Erfahre mehr über deine Begleiter" },
    { "Revive myths of old", "Setze alte Abenteuer fort" },
    { "This slot contains a saved game.\nOverride it?", "Es existiert bereits ein Spielstand.\nSoll dieser überschrieben werden?" },

    // 80

    { "Yes", "Ja" },
    { "No", "Nein" },
    { "(empty)", "(leer)" },
    { "Transfer", "Transfer" },
    { "Special", "Spezial" },

    { "The language is currently\nset to “English”.\nChange the language setting?", "Die eingestellte Sprache\nist „Deutsch“.\nSprache ändern?" },
    { "into the %s bag.`","%s-Tasche verstaut.`" },
    { "Stored %s %s",  "%s %s in der" },
    { "Stored %s", "%s in der" },
    { "Stored %3d %s", "%3d %s in der" },

    // 90

    { "The scent vanished…", "Der Duft verstrich wirkungslos…" },
    { "PokéNav", "PokéNav" },
    { " Would you like to\n save your progress?\n", " Möchtest du deinen\n Fortschritt sichern?\n" },
    { "Save?\nFrom an emulator?!", "Speichern?\nIn einem Emulator?!" },
    { "Saved successfully!", "Speichern erfolgreich!" },

    { "An error occured.\nProgress not saved.", "Es trat ein Fehler auf\nSpiel nicht gesichert." },
    { "No %s available.\nRemove the icon?", "Kein Exemplar des Items\n%s vorhanden.\nIcon entfernen?" },
    { "Recently used items\nwill appear here.", "Hier erscheinen zuletzt\neingesetzte Items." },
    { "You may register an\nitem to the button Y.", "Du kannst ein Item\nauf Y registrieren." },
    { "%s used %s!", "%s setzt %s\nein!" },


    // 100


    { "You can't use this\nmove right now.","Diese Attacke kann jetzt\nnicht eingesetzt werden." },
    { "Stored %s\nfrom %s.", "%s von %s\nim Beutel verstaut." },
    { "%s already\nknows %s!", "%s beherrscht\n%s bereits!" },
    { "%s learned\n%s!", "%s erlernt\n%s!" },
    { "%s already\nknows 4 moves.\nForget a move?", "%s beherrscht\nbereits 4 Attacken.\nSoll eine verlernt werden?" },

    { "Which move?", "Welche Attacke?" },
    { "%s can't\nforget %s!", "%s kann\n%s nicht vergessen!" },
    { "%s can't\nlearn %s!", "%s kann\n%s nicht erlernen!" },
    { "%hhu badges", "%hhu Orden" },
    { "Pick a language", "Wähle eine Sprache" },

    // 110

    { "Choose an episode", "Wähle eine Speizalepisode" },
    { "Welcome to Special Episode 0:\nDeveloper's Heaven.", "Willkommen zur Spezialepisode 0:\nDeveloper's Heaven" },
    { "In this episode, you will gain\nthe mighty powers of the\ndev himself.", "In dieser Episode erhältst du\ndie Fähigkeiten des Entwicklers." },
    { "Do whatever you want.", "Mach damit, was immer du möchtest." }
};