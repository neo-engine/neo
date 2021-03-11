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

const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ] = {
    { "No data.", "Keine Daten." },
    { "Faraway Place", "Entfernter Ort" },
    { "Unknown", "Unbekannt" },
    { "%sUse %s?", "%sM\xf6"
                   "chtest du %s nutzen?" },
    { "The repel wore off.", "Der Schutz ist aufgebraucht." },

    { "Just an old Pok\xe9 Ball|", "Doch nur ein alter Pok\xe9"
                                   "ball|" },
    { "You fished a Pok\xe9mon!", "Du hast ein Pok\xe9mon geangelt!" },
    { "Map border.\nReturn, or you\nwill get lost!",
      "Ende der Kartendaten.\nKehr um, sonst\nverirrst du dich!" },
    { "\nSomething's on the hook!", "\nDa ist etwas am Haken!" },
    { "It disappeared|", "Es ist entkommen|" },

    // 10

    { "%s used %s!", "%s setzt %s ein!" },
    { "Items", "Items" },
    { "Medicine", "Heilitems" },
    { "TM/HM", "TM/VM" },
    { "Berries", "Beeren" },

    { "Key Items", "Basis-Items" },
    { "Firmness: %s", "G\xfcte: %s" },
    { "Hard", "Hart" },
    { "Soft", "Weich" },
    { "Super hard", "Steinhart" },

    // 20

    { "Super soft", "Sehr weich" },
    { "Very hard", "Sehr hart" },
    { "Normal", "Normal" },
    { "Size:%4.1fcm", "Gr\xf6\xdf"
                      "e:%4.1fcm" },
    { "Spicy", "Scharf" },

    { "Dry", "Trocken" },
    { "Sweet", "S\xfc\xdf" },
    { "Bitter", "Bitter" },
    { "Sour", "Sauer" },
    { "Type", "Typ" },

    // 30

    { "Categ.", "Kateg." },
    { "PP", "AP" },
    { "Strength", "St\xe4rke" },
    { "Accuracy", "Genauigkeit" },
    { "Egg", "Ei" },

    { "Already learned", "Bereits erlernt" },
    { "Possible", "Erlernbar" },
    { "Not possible", "Nicht erlernbar" },
    { "Level %3d\n%3d/%3d HP", "Level %3d\n%3d/%3d KP" },
    { "Level %3d\nFainted", "Level %3d\nBesiegt" },

    // 40

    { "Possible", "M\xf6glich" },
    { "Not possible", "Nicht m\xf6glich" },
    { "No item", "Kein Item" },
    { "No items", "Keine Items" },
    { "Give", "Geben" },

    { "Apply", "Anwenden" },
    { "Register", "Registrieren" },
    { "Use", "Einsetzen" },
    { "Toss All", "Alle Wegwerfen" },
    { "Select a move!", "Welche Attacke?" },

    // 50

    { "Used %s.", "%s eingesetzt." },
    { "What?\n%s is evolving!", "Nanu?\n%s entwickelt sich!" },
    { "Congratulations! Your %s\nevolved into %s!", "Gl\xfc"
                                                    "ckwunsch! Dein %s\nwurde zu einem %s!" },
    { "It would be wasted|", "Es w\xfcrde keine Wirkung haben|" },
    { "%s already has an item.\nSwap the items?",
      "%s tr\xe4gt bereits ein Item.\nSollen die Items getauscht werden?" },

    { "You can't choose this item.", "Dieses Item kann nicht ausgew\xe4hlt werden." },
    { "Choose %s?", "%s ausw\xe4hlen?" },
    { "Do what with %s?", "Was tun mit %s?" },
    { "You can't use that right now.", "Das kann jetzt nicht eingesetzt werden." },
    { "Pok\xe9mon team", "Pok\xe9mon-Team" },

    // 60

    { "Cache", "Zwischenablage" },
    { "All boxes", "Alle Boxen" },
    { "New name for box %s", "Name f\xfcr Box %s" },
    { "Repel activated.", "Schutz eingesetzt." },
    { "Super Repel activated.", "Superschutz eingesetzt." },

    { "Max Repel activated.", "Top-Schutz eingesetzt." },
    { "Exp. All deactivated.", "EP-Teiler ausgeschaltet." },
    { "Exp. All activated.", "EP-Teiler eingeschaltet." },
    { "Coins: %lu.", "M\xfcnzen: %lu." },
    { "BP: %lu.", "Kampfpunkte: %lu." },

    // 70

    { "TOUCH TO START", "BER\xdc"
                        "HRE, UM ZU STARTEN" },
    { "Continue", "Weiter" },
    { "New Game", "Neues Spiel" },
    { "Special Episodes", "Spezialepisoden" },
    { "Import Save", "Spiel-Import" },

    { "Continue your adventure", "Setze dein Abenteuer fort" },
    { "Start a new adventure", "Starte ein neues Abenteuer" },
    { "Learn your companion's stories", "Erfahre mehr \xfc"
                                        "ber deine Begleiter" },
    { "Revive myths of old", "Setze alte Abenteuer fort" },
    { "This slot contains a saved game.\nOverwrite it?",
      "Es existiert bereits ein Spielstand.\nSoll dieser \xfc"
      "berschrieben werden?" },

    // 80

    { "Yes", "Ja" },
    { "No", "Nein" },
    { "(empty)", "(leer)" },
    { "Transfer", "Transfer" },
    { "Special", "Spezial" },

    { "The language is set to English.\nChoose a different language?",
      "Die aktuelle Sprache ist Deutsch.\nSprache wechseln?" },
    { "You put the %s\ninto the [%hu] %s bag.", "%s bei den\n[%hu] %s verstaut." },
    { "Stored %s %s", "%s %s in der" },
    { "Stored %s", "%s in der" },
    { "Stored %3d %s", "%3d %s in der" },

    // 90

    { "The scent vanished|", "Der Duft verstrich wirkungslos|" },
    { "Pok\xe9Nav", "Pok\xe9Nav" },
    { " Would you like to save your progress?", " M\xf6"
                                                "chtest du deinen Fortschritt sichern?" },
    { "Saving a lot of data| Don't cut the power.\n%s",
      "Speichere eine Menge Daten| Nicht abschalten.\n%s" },
    { "Saving complete!", "Speichern erfolgreich!" },

    { "An error occured.\nProgress not saved.", "Es trat ein Fehler auf\nSpiel nicht gesichert." },
    { "No %s available.\nRemove the icon?",
      "Kein Exemplar des Items\n%s vorhanden.\nIcon entfernen?" },
    { "Recently used items\nwill appear here.", "Hier erscheinen zuletzt\neingesetzte Items." },
    { "You can register an item to the Y button.", "Du kannst ein Item auf Y registrieren." },
    { "%s used %s!", "%s setzt %s ein!" },

    // 100

    { "You can't use this\nmove right now.", "Diese Attacke kann jetzt\nnicht eingesetzt werden." },
    { "Took %s\nfrom %s.", "%s von\n%s verstaut." },
    { "%s already\nknows %s!", "%s beherrscht\n%s bereits!" },
    { "%s learned\n%s!", "%s erlernt\n%s!" },
    { "%s already knows 4 moves. Forget a move?",
      "%s kennt bereits 4 Attacken. Soll eine verlernt werden?" },

    { "Select a move!", "Welche Attacke?" },
    { "%s can't\nforget %s!", "%s kann\n%s nicht vergessen!" },
    { "%s can't\nlearn %s!", "%s kann\n%s nicht erlernen!" },
    { "%hhu badges", "%hhu Orden" },
    { "Pick a language", "W\xe4hle eine Sprache" },

    // 110

    { "Choose an episode", "W\xe4hle eine Speizalepisode" },
    { "Welcome to Special Episode 0:\nDeveloper's Heaven.",
      "Willkommen zur Spezialepisode 0:\nDeveloper's Heaven" },
    { "In this episode, you will gain\nthe mighty powers of the\ndev himself.",
      "In dieser Episode erh\xe4ltst du\ndie F\xe4higkeiten des Entwicklers." },
    { "Do whatever you want.", "Mach damit, was immer du m\xf6"
                               "chtest." },
    { "Do you want to import your\nsave from the GBA game?",
      "M\xf6"
      "chtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?" },

    { "Importing a save file will\ncopy all Pok\xe9mon and items\nfrom the GBA game.",
      "Beim Importieren werden alle\nPok\xe9mon und Items vom\nGBA-Modul kopiert." },
    { "Further, you will start at\nthe same position as you\nleft off in the GBA game.",
      "Du wirst das Spiel an der\nselben Stelle wie auf dem\nGBA-Modul fortsetzen." },
    { "No data will be written to\nthe GBA Game Pak.",
      "Es werden keine Daten auf\ndas GBA-Modul geschrieben." },
    { "However, proceed at your\nown risk.",
      "Trotzdem geschieht das\nImportieren auf eigene\nGefahr." },
    { "No compatible cartrige found.\nAborting.",
      "Kein kompatibles GBA-Modul\ngefunden.\nBreche ab." },

    // 120

    { "Proceed?", "Fortfahren?" },
    { "Importing data|", "Lade Spielstand|" },
    { "An error occured.\nAborting.", "Ein Fehler ist aufgetreten.\nKehre zum Hauptmen\xfc zur\xfc"
                                      "ck." },
    { "Completed.", "Abgeschlossen." },
    { "Entries", "Eintr\xe4ge" },

    { "All", "Alle" },
    { "HP", "KP" },
    { "Attack", "Angriff" },
    { "Defense", "Vert." },
    { "Sp. Atk", "Sp. Ang." },

    // 130

    { "Sp. Def", "Sp. Ver." },
    { "Speed", "Init." },
    { "f. %s", "aus %s" },
    { "female", "weiblich" },
    { "male", "m\xe4nnlich" },

    { "(shining)", "(schillernd)" },
    { "Fainted", "Besiegt" },
    { "Statistics", "Statuswerte" },
    { "Lv.%3i", "Lv.%3i" },
    { "%s tries to learn %s.", "%s versucht %s zu lernen." },

    // 140

    { "The terrain didn't change.", "Das Terrain blieb unver\xe4ndert." },
    { "Do you want to give a nick name to %s?", "M\xf6"
                                                "chtest du dem %s einen Spitznamen geben?" },
    { "Choose a nick name!", "W\xe4hle einen Spitznamen!" },
    { "You are challenged by %s %s!", "Eine Herausforderung von %s %s!" },
    { "Frzn", "Eingfr" },

    { "Prlz", "Prlyse" },
    { "Burn", "Verbrng" },
    { "Slp", "Schlaf" },
    { "Psn", "Gift" },
    { "Switch", "Aussenden" },

    // 150

    { "In Battle", "Im Kampf" },
    { "Send In", "In den Kampf" },
    { "To be sent", "Schon ausgew\xe4hlt" },
    { "Eggs can't hold items!", "Ein Ei kann kein Item tragen." },
    { "battle!", "nicht k\xe4mpfen!" },

    { "Status", "Bericht" },
    { "Moves", "Attacken" },
    { "ATK", "ANG" },
    { "DEF", "VER" },
    { "SAT", "SAN" },

    // 160

    { "SDF", "SVE" },
    { "SPD", "INI" },
    { "What will %s do?", "Was soll %s tun?" },
    { "You got away safely.", "Du bist entkommen." },
    { "You couldn't escape|", "Flucht gescheitert|" },

    { "Choose a target!", "Welches Pok\xe9mon angreifen?" },
    { "Choose a Pok\xe9mon.", "Welches Pok\xe9mon?" },
    { "%s gained %lu EXP.", "%s gewinnt %lu E.-Punkte." },
    { "%s advanced to level %d.", "%s erreicht Level %d." },
    { "The %s enabled\n%s to go first!", "%.0s%s agiert dank\neiner %s zuerst!" },

    // 170

    { "The wild %s fled!", "%s (wild) floh." },
    { "But nothing happened|", "Nichts geschah|" },
    { "It got wasted|", "Es hatte keine Wirkung|" },
    { "Used %s.", "%s eingesetzt." },
    { "%s's data was\nregistred to the Pok\xe9"
      "Dex.",
      "Die Daten von %s\nwurden im Pok\xe9"
      "Dex gespeichert." },

    { "%s was sent to\nthe Pok\xe9mon Storage System.",
      "%s wurde an das\nPok\xe9mon-Lagerungssystem\ngeschickt." },
    { "Box [129]%s\" is full.", "Box [131]%s[129] ist voll." },
    { "%s was deposited to Box [129]%s\".", "%s wurde in Box [131]%s[129] abgelegt." },
    { "There is no space left for additional Pok\xe9mon.",
      "Du hast keinen Platz f\xfcr weitere Pok\xe9mon." },
    { "%s was released.", "%s wurde wieder\nfreigelassen." },

    // 180

    { "FIGHT", "Kampf" },
    { "BAG", "Beutel" },
    { "POK\xe9MON", "Pok\xe9mon" },
    { "RUN", "Flucht" },
    { "Pwr", "Stk" },

    { "Meteor Falls", "Meteorf\xe4lle" },
    { "\xd", "\xe" }, // HP icon
    { "Hardy", "Robust" },
    { "Lonely", "Einsam" },
    { "Brave", "Mutig" },

    // 190

    { "Adamant", "Hart" },
    { "Naughy", "Frech" },
    { "Bold", "K\xfc"
              "hn" },
    { "Docile", "Sanft" },
    { "Relaxed", "Locker" },

    { "Impish", "Pfiffig" },
    { "Lax", "Lasch" },
    { "Timid", "Scheu" },
    { "Hasty", "Hastig" },
    { "Serious", "Ernst" },

    // 200

    { "Jolly", "Froh" },
    { "Naive", "Naiv" },
    { "Modest", "M\xe4"
                "\xdf"
                "ig" },
    { "Mild", "Mild" },
    { "Quiet", "Ruhig" },

    { "Bashful", "Zaghaft" },
    { "Rash", "Hitzig" },
    { "Calm", "Still" },
    { "Gentle", "Zart" },
    { "Sassy", "Forsch" },

    // 210

    { "Careful", "Sacht" },
    { "Quirky", "Kauzig" },
    { "Loves to eat.", "Liebt es, zu essen." },
    { "Often dozes off.", "Nickt oft ein." },
    { "Nods off a lot.", "Schl\xe4"
                         "ft gerne." },

    { "Scatters things often.", "Macht oft Unordnung." },
    { "Likes to relax.", "Liebt es zu entspannen." },
    { "Proud of its power.", "Ist stolz auf seine St\xe4"
                             "rke." },
    { "Likes to thrash about.", "Pr\xfc"
                                "gelt sich gerne." },
    { "A little quick tempered.", "Besitzt Temperament." },

    // 220

    { "Likes to fight.", "Liebt es zu k\xe4"
                         "mpfen." },
    { "Quick tempered.", "Ist impulsiv." },
    { "Sturdy body.", "Hat einen robusten K\xf6"
                      "rper." },
    { "Capable of taking hits.", "Kann Treffer gut verkraften." },
    { "Highly persistent.", "Ist \xe4"
                            "u\xdf"
                            "erst ausdauernd." },

    { "Good endurance.", "Hat eine gute Ausdauer." },
    { "Good perseverance.", "Ist beharrlich." },
    { "Highly curious.", "Ist sehr neugierig." },
    { "Mischievous.", "Ist hinterh\xe4"
                      "ltig." },
    { "Thoroughly cunning.", "Ist \xe4"
                             "u\xdf"
                             "erst gerissen." },

    // 230

    { "Often lost in thought.", "Ist oft in Gedanken." },
    { "Very finicky.", "Ist sehr pedantisch." },
    { "Strong willed.", "Besitzt starken Willen." },
    { "Somewhat vain.", "Ist etwas eitel." },
    { "Strongly defiant.", "Ist sehr aufs\xe4"
                           "ssig." },

    { "Hates to lose.", "Hasst Niederlagen." },
    { "Somewhat stubborn.", "Ist dickk\xf6"
                            "pfig." },
    { "Likes to run.", "Liebt es, zu rennen." },
    { "Alert to sounds.", "Achtet auf Ger\xe4"
                          "usche." },
    { "Impetuous and silly.", "Ist ungest\xfc"
                              "m und einf\xe4"
                              "ltig." },

    // 240

    { "Somewhat of a clown.", "Ist fast wie eine Clown." },
    { "Quick to flee.", "Fl\xfc"
                        "chtet schnell." },
    { "spicy", "scharf" },
    { "sour", "saur" },
    { "sweet", "s\xfc\xdf" },

    { "dry", "trocken" },
    { "bitter", "bitter" },
    { "all", "all" },
    { "HP", "HP" },
    { "Attack", "Angriff" },

    // 250

    { "Defense", "Verteidigung" },
    { "Sp. Attack", "Spez Angr." },
    { "Sp. Def.", "Sepz. Vert." },
    { "Speed", "Initiative" },
    { "Evasion", "Fluchtwert" },

    { "Accuracy", "Genauigkeit" },
    { "%s's %s did not change.", "%.0s%s von %s bleibt unver\xe4ndert." },
    { "%s's %s rose.", "%.0s%s von %s steigt." },
    { "%s's %s rose sharply.", "%.0s%s von %s steigt stark." },
    { "%s's %s rose drastically.", "%.0s%s von %s steigt drastisch." },

    // 260

    { "%s's %s fell.", "%.0s%s von %s sinkt." },
    { "%s's %s harshly fell.", "%.0s%s von %s sinkt stark." },
    { "%s's %s severely fell.", "%.0s%s von %s sinkt drastisch." },
    { "%s %s sends out %s.", "%s %s schickt %s in den Kampf." },
    { "F I G H T", "K A M P F" },

    { "POK\xc9MON", "POK\xc9MON" },
    { "RUN", "FLUCHT" },
    { "BAG", "BEUTEL" },
    { "BACK", "ZUR\xdc"
              "CK" },
    { "%s sets up a trap.", "%s legt eine Falle." },

    // 270

    { "%s is tightening its focus.", "%s versch\xe4rft seinen Fokus." },
    { "%s's beak glows.", "Der Schnabel von %s gl\xfcht." },
    { "%s returned to you.", "%s kam zu dir zur\xfc"
                             "ck." },
    { "That is enough %s!", "Das reicht, %s!" },
    { "%s returned to %s.", "%s kommt zu %s zur\xfc"
                            "ck." },

    { "%s withdrew %s.", "%s ruft %s zur\xfc"
                         "ck." },
    { "%s recharges.", "%s l\xe4"
                       "dt sich wieder auf." },
    { "%s became confused.", "%s ist vor Ersch\xf6pfung verwirrt." },
    { "%s ate its %s. Its confusion vanished.", "%s isst die %s. Es ist nicht mehr verwirrt." },
    { "%s ate its %s.", "%s isst die %s." },

    // 280

    { "%s evaded the attack.", "%s weicht aus." },
    { "%s passes its %s. The %s found its purpose.",
      "%s gibt seine %s weiter. Die %s findet ihre Bestimmung." },
    { "The %s protects %s.", "Die %s sch\xfctzt %s." },
    { "%s obtained the %s.", "%s erh\xe4lt das Item %s." },
    { "It has no effect on %s|", "Es hat keine Wirkung auf %s|" },

    { "It is super effective on %s.", "Es ist sehr effektiv gegen %s." },
    { "It is not very effective on %s.", "Es ist nicht sehr effektiv gegen %s." },
    { "%s is hit by the recoil.", "Der R\xfc"
                                  "cksto\xdf schadet %s." },
    { "%s restores some HP.", "%s heilt ein paar KP." },
    { "%s fainted.", "%s wurde besiegt." },

    // 290

    { "It missed %s.", "%s weicht aus." },
    { "A critical hit!", "Ein Volltreffer!" },
    { "%s prepares a move.", "%s bereitet einen Angriff vor." },
    { "%s is confused.", "%s ist verwirrt." },
    { "%s snapped out of its confusion.", "%s ist nicht mehr verwirrt." },

    { "It hurt itself in its confusion.", "Es hat sich in seiner Verwirrung selbst verletzt." },
    { "%s flinches.", "%s schreckt zur\xfc"
                      "ck." },
    { "%s is frozen solid.", "%s ist gefroren." },
    { "%s was thawed.", "%s ist aufgetaut." },
    { "%s is asleep.", "%s schl\xe4"
                       "ft." },

    // 300

    { "%s woke up.", "%s ist aufgewacht." },
    { "%s is paralyzed. It can't move.", "%s ist paralysiert. Es kann nicht angreifen." },
    { "%s is immobilized by its love.", "%s ist starr vor Liebe." },
    { "The weather didn't change.", "Das Wetter bleibt unver\xe4ndert." },
    { "It failed.", "Es schlug fehl." },

    { "%s became fully charged due to its Power Herb!", "Dank Energiekraut ist %s sofort bereit!" },
    { "%s looses some HP.", "%s verliert ein paar KP." },
    { "%s made a wish upon its %s.", "%s w\xfcnscht sich St\xe4rke von seinem %s." },
    { "%s's wish for power was granted!", "Der Wunsch von %s wurde erh\xf6hrt!" },
    { "the wild %s", "%s (wild)" },

    // 310

    { "the foe's %s", "%s (Gegner)" },
    { "The wild %s", "%s (wild)" },
    { "The foe's %s", "%s (Gegner)" },
    { "This tree looks like it can be cut down. ", "Ein kleiner Baum. " },
    { "This rock appears to be breakable. ", "Ein kleiner Felsen. " },

    { "A huge whirlpool. ", "Ein riesiger Strudel. " },
    { "The water is a deep blue| ", "Das Wasser ist tiefblau| " },
    { "The sea is deep here. ", "Das Wasser ist sehr tief. " },
    { "It's a big boulder. ", "Ein gro\xdf"
                              "er Felsen. " },
    { "The wall is very rocky|\n", "Eine steile Felswand|\n" },

    // 320

    { "It's a large Waterfall. ", "Ein gro\xdf"
                                  "er Wasserfall. " },
    { "Something moved! ", "Da hat sich etwas bewegt! " },
    { "Light is shining from the surface. ", "Licht scheint von der Oberfl\xe4"
                                             "che. " },
    { "Select", "Ausw\xe4hlen" },
    { "Status", "Status" },

    { "Give Item", "Item geben" },
    { "Take Item", "Item nehmen" },
    { "Use Item", "Item nutzen" },
    { "Swap", "Austauschen" },
    { "Pok\xe9"
      "Dex",
      "Pok\xe9"
      "Dex" },

    // 330

    { "Cancel", "Zur\xfc"
                "ck" },
    { "Deselect", "Abw\xe4hlen" },
    { "Select %hhu Pok\xe9mon!", "W\xe4hle %hhu Pok\xe9mon!" },
    { "Select a Pok\xe9mon!", "W\xe4hle ein Pok\xe9mon!" },
    { "Gave %s\nto %s.", "%s an\n%s gegeben." },

    { "Choose these Pok\xe9mon?", "Diese Pok\xe9mon w\xe4hlen?" },
    { "Choose this Pok\xe9mon?", "Dieses Pok\xe9mon w\xe4hlen?" },
    { "Dex No.", "Dex Nr." },
    { "Name", "Name" },
    { "OT", "OT" },

    // 340

    { "ID No.", "Id Nr." },
    { "Exp.", "E.-Pkt." },
    { "Next", "N\xe4"
              "chst." },
    { "Level", "Level" },
    { "Pok\xe9mon Info", "Pok\xe9mon-Info" },

    { "It looks like this Egg", "Was da wohl schl\xfcpfen" },
    { "will take a long time", "wird? Es dauert wohl" },
    { "to hatch.", "noch lange." },
    { "It appears to move", "Hat es sich gerade" },
    { "occasionally. It may", "bewegt? Da tut sich" },

    // 350

    { "be close to hatching.", "wohl bald was." },
    { "Sounds can be heard", "Jetzt macht es schon" },
    { "coming from inside!", "Ger\xe4usche! Bald ist" },
    { "It will hatch soon!", "es wohl soweit!" },
    { "A mysterious Pok\xe9mon", "Ein r\xe4tselhaftes" },

    { "Egg received", "Pok\xe9mon-Ei erhalten" },
    { "from", "vom" },
    { "in/at", "in/bei" },
    { "from", "von" },
    { "A highly mysterious", "Ein \xe4u\xdf"
                             "erst seltsames" },

    // 360

    { "Pok\xe9mon Egg that", "Pok\xe9mon-Ei, das" },
    { "somehow reached you", "irgendwie zu dir fand" },
    { "Item", "Item" },
    { "Ability", "F\xe4higkeit" },
    { "", "Wesen: " },

    { " nature.", "" },
    { "Egg received.", "Ei erhalten." },
    { "Had a fateful encunter.", "Schicksalhafte Begegnung." },
    { "Egg hatched.", "Ei geschl\xfcpft." },
    { "Egg apparently hatched.", "Ei offenbar geschl\xfcpft." },

    // 370

    { "Met at Lv. %hu.", "Erhalten mit Lv. %hu." },
    { "Apparently met at Lv. %hu.", "Offenbar erhalten mit Lv. %hu." },
    { "Had a fateful enc. at Lv. %hu.", "Schicksalhafte Begeg. mit Lv. %hu." },
    { "App. had a fatef. enc. at Lv. %hu.", "Off. schicks. Begeg. mit Lv. %hu." },
    { "Likes ", "Mag " },

    { "%s ", "%se " },
    { "Pok\xe9"
      "blocks.",
      "Pok\xe9riegel." },
    { "%2hhu/%2hhu", "%2hhu/%2hhu" }, // PP
    { "Nature", "Wesen" },
    { "Move Pok\xe9mon", "Pok\xe9mon bewegen" },

    // 380

    { "Show Party", "Team-PKMN" },
    { "Hide Party", "Box-PKMN" },
    { "Move", "Bewegen" },
    { "Summary", "Details" },
    { "Release", "Freilassen" },

    { "Give Item", "Item geben" },
    { "Take Item", "Item nehm." },
    { "Cancel", "Abbrechen" },
    { "Huh? The evolution\nwas interrupted|", "Huh? Die Entwicklung\nwurde abgebrochen|" },
    { "%s hatched from the Egg!", "%s schl\xfcpfte aus dem Ei!" },

    // 390
    { "Power: %hhu", "St\xe4rke: %hhu" },
    { "Accuracy: %hhu", "Genauigkeit: %hhu" },
    { "(never misses)", "(trifft immer)" },
    { "%s's\n%s%.0s", "%.0s%s\nvon %s" },
    { "You encountered a wild %s!", "Ein wildes %s erscheint!" },

    { "Go %s!", "Los %s!" },
    { "%s detects %s.", "%s erkennt %s." },                 // Forewarn
    { "%s shudders.", "%s erschaudert." },                  // Anticipation
    { "%s identifies %s.", "%s erkennt %s." },              // Frisk
    { "%s identifies %s and %s.", "%s erkennt %s und%s." }, // Frisk

    // 400

    { "Hit %hhu times.", "%hhu Mal getroffen." },
    { "Details", "Details" },
    { "Deregister", "Registrieren" },
    { "%s did not learn %s.", "%s hat %s nicht erlernt." },
    { "Mystery Gift", "Geheimgeschehen" },

    { "T R A I N E R'S   C A R D", "T R A I N E R P A S S" },
    { "Name", "Name" },
    { "Play time", "Spielzeit" },
    { "Money", "Geld" },
    { "Pok\xe9"
      "Dex",
      "Pok\xe9"
      "Dex" },

    // 410

    { "Adventure started on %s", "Abenteuer gest. am %s" },
    { "Last save created in/at %s\non %s, %02hhu:%02hhu",
      "Zuletzt gesp. in/bei %s\nam %s um %02hhu:%02hhu." },
    { "No save data in this slot.", "Kein Spielstand in diesem Slot." },
    { "Pok\xe9mon", "Pok\xe9mon" },
    { "Pok\xe9"
      "Dex",
      "Pok\xe9"
      "Dex" },

    { "Bag", "Beutel" },
    { "Trainer ID", "Trainerpass" },
    { "Save", "Sichern" },
    { "Options", "Optionen" },
    { "Text Speed", "Textgeschw." },

    // 420

    { "Language", "Sprache" },
    { "BGM", "Musik" },
    { "SFX", "Soundeffekte" },
    { "Difficulty", "Schwierigkeit" },
    { "Wallpaper", "Hintergrund" },

    { "Slow", "Langs." },
    { "Medium", "Mittel" },
    { "Fast", "Schnell" },
    { "On", "An" },
    { "Off", "Aus" },

    // 430
    { "Wynaut", "Isso" },
    { "Skitty", "Eneco" },
    { "Absol", "Absol" },
    { "Sound disabled", "Musik deaktiviert" },
    { "Hoenn Pok\xe9mon League Badges", "Hoenn Pok\xe9mon-Liga Orden" },

    { "Battle Frontier Symbols", "Kampfzonensymbole" },
    { "%s obtained\nthe %s!", "%s erhielt\n%s!" },
    { "Choose the %s Pok\xe9mon %s?", "Das %s-Pok\xe9mon %s w\xe4hlen?" },
    { "Prof. Birch is in trouble!\nRelease a Pok\xe9mon and rescue him!",
      "Prof. Birk ist in Schwierigkeiten!\nSchnapp' dir ein Pok\xe9mon und hilf ihm!" },
    { "BACKSPACE", "ENTFERNEN" },

    // 440

    { "ENTER", "ENTER" },
    { "Hello..?\nCan you hear me?", "Hallo..?\nKannst du mich h\xf6ren?" },
    { "Nice! Your Pok\xe9Nav seems to be working just fine.",
      "Gut! Dein Pok\xe9Nav funktioniert schonmal." },
    { "My name is Birch, but everyone calls me Pok\xe9mon Professor.",
      "Mein Name ist Birk, aber jeder nennt mich Pok\xe9mon Professor." },
    { "But first of all, welcome to the world of Pok\xe9mon!",
      "Aber zun\xe4"
      "chst, willkommen in der Welt der Pok\xe9mon!" },

    { "This is what we call a [129]Pok\xe9mon.\"",
      "Dies ist ein sogenanntes [131]Pok\xe9mon[129]." },
    { "This world is widely inhabited by creatures known as Pok\xe9mon.",
      "Auf dieser Welt leben Wesen, die als Pok\xe9mon bekannt sind." },
    { "We humans live alongside Pok\xe9mon, at times as friendly playmates,",
      "Wir Menschen leben Seite an Seite mit ihnen, manchmal als Freunde," },
    { "at times as cooperative workmates.", "manchmal auch als Arbeitskollegen." },
    { "And sometimes, we band together and battle others like us.",
      "Und manchmal schlie\xdf"
      "en wir uns zusammen und tragen K\xe4mpfe aus." },

    // 450

    { "But despite our closeness, there is much to learn about Pok\xe9mon.",
      "Und dennoch gibt es noch viel \xfc"
      "ber die Pok\xe9mon zu lernen." },
    { "In fact, there are many, many secrets surrounding Pok\xe9mon.",
      "Es gibt noch viele, viele Geheimnisse um die Pok\xe9mon." },
    { "To unravel Pok\xe9mon mysteries, I've been undertaking research.",
      "Um ihre Mysterien aufzudecken, widme ich mich der Forschung." },
    { "That's what I do.", "Ja, genau das tue ich." },
    { "Ah sorry, I got carried away|", "Ah, ich schweife ab|" },

    { "I am calling because I need some information from you.",
      "Ich rufe an, weil ich noch ein paar Informationen von dir ben\xfctige." },
    { "For now, just complete your profile in your Pok\xe9Nav.",
      "Kannst du bitte dein Pok\xe9Nav-Profil ausf\xfcllen?" },
    { "Choose a picture!", "W\xe4hle ein Bild!" },
    { "Your name?", "Dein Name?" },
    { "Are you fine with your profile?", "Ist dieses Profil so in Ordnung?" },

    // 460

    { "Brendan", "Brix" },
    { "May", "Maike" },
    { "Alright ", "Also gut " },
    { ", are you ready?", ", bist du bereit?" },
    { "Your very own adventure is about to unfold.",
      "Dein ganz eigenes Abenteuer wird nun beginnen." },

    { "Take courage, and leap into the world of Pok\xe9mon,",
      "Hab nur Mut, und st\xfcrze dich in die Welt der Pok\xe9mon," },
    { "where dreams, adventure, and friendships await!",
      "wo Abenteuer, Tr\xe4ume und Freundschaften warten." },
    { "Well, I'll be expecting you later.\nCome see me in my Pok\xe9mon Lab.",
      "Ah, komm sp\xe4ter in mein Pok\xe9mon-\nLabor, ich erwarte dich dort." },
    { "Buy", "Kaufen" },
    { "Sell", "Verkaufen" },

    // 470

    { "Welcome!\nHow may I help you today?", "Willkommen!\nWie kann ich helfen?" },
    { "Money: $%lu", "Geld: $%lu" },
    { "BP: %lu", "GP: %lu" },
    { "Coins: %lu", "M\xfcnzen: %lu" },
    { "In Bag: %lu", "Beutel: %lu" },

    { "%s, sure!\nHow many do you need?", "%s, gerne!\nWie viele Exemplare ben\xf6tigst du?" },
    { "%s, and you want %ld? This will be $%lu then.", "%s und du m\xf6"
                                                       "chtest %ld? Das macht dann $%lu." },
    { "%s, and you want %ld? This will be %lu BP then.", "%s und du m\xf6"
                                                         "chtest %ld? Das macht dann %lu GP." },
    { "%s, and you want %ld? This will be %lu coins then.",
      "%s und du m\xf6"
      "chtest %ld? Das macht dann %lu M\xfcnzen." },
    { "You don't have enough money.", "Du hast nicht genug Geld." },

    // 480

    { "You don't have enough BP.", "Du hast nicht genug GP." },
    { "You don't have enough coins.", "Du hast nicht genug M\xfcnzen." },
    { "I cannot buy this item.", "Ich kann dieses Item nicht kaufen." },
    { "Sell All", "Alle Verkaufen" },
    { "I can pay you $%lu. Is this fine?", "Ich kann $%lu zahlen. Ist das ok?" },

    { "Do you really want to toss this item?", "Willst du dieses Item wirklich wegwerfen?" },
    { "Gotcha! %s was caught!", "Toll! %s wurde gefangen!" },
    { "Oh, no! It broke free!", "Mist! Es hat sich befreit!" },
    { "Aww! It appeared to be caught!", "Oh. Fast h\xe4tte es geklappt!" },
    { "Aargh! Almost had it!", "Mist! Das war knapp!" },

    // 490

    { "Shoot! It was so close, too!", "Verflixt! Es war doch fast gefangen." },
    { "The weather became normal again.", "Das Wetter wird wieder normal." },
    { "It started to rain.", "Es beginnt zu regnen." },
    { "It started to hail.", "Es beginnt zu hageln." },
    { "The air became foggy.", "Dichter Nebel zieht auf." },

    { "A sandstorm formed.", "Ein Sandsturm zieht auf." },
    { "The sunlight turned harsh.", "Das Sonnenlicht wird st\xe4rker." },
    { "A heavy rain began to fall.", "Es beginnt enorm zu regnen." },
    { "The sunlight turned extremely harsh.", "Das Sonnenlicht wird enorm kr\xe4rker." },
    { "Mysterious strong winds are protecting Flying-type Pok\xe9mon.",
      "Starke Winde sch\xfctzen Flug-Pok\xe9mon." },

    // 500
    { "Rain continues to fall.", "Es regnet weiter." },
    { "Hail continues to fall.", "Es hagelt." },
    { "The air is foggy.", "Dichter Nebel." },
    { "The sandstorm rages.", "Der Sandsturm w\xfctet." },
    { "The sunlight is harsh.", "Das Sonnenlicht ist stark." },

    { "Heavy rain continues to fall.", "Enormer Regen." },
    { "The extremely harsh sunlight continues.", "Extremes Sonnenlicht." },
    { "Strong winds continue to blow.", "Starke Winde." },
    { "The battlefield became normal.", "Das Feld wird normal." },
    { "The battlefield became weird.", "Das Feld wird komisch." },

    // 510

    { "An electric current runs across the battlefield.",
      "Eine elektrische Str\xf6mung flie\xdft durch das Feld." },
    { "Mist swrils around the battlefield.", "Das Feld wird in Nebel geh\xfcllt." },
    { "Grass grew to cover the battlefield.", "Gras w\xe4"
                                              "chst \xfc"
                                              "ber das Feld." },
    { "A deluge of ions showers the battlefield.",
      "Ein elektrisch geladener Niederschlag regnet auf das Feld herab." },
    { "It created a strange room in which items no longer work.",
      "Es entsteht ein Raum, in dem keine Items mehr funktionieren." },

    { "It created a bizarre area in which the Defense and Sp. Defense stats are swapped.",
      "Es entsteht ein Raum, in dem Vert. und Sp.Vert. vertauscht sind." },
    { "It twisted the dimensions.", "Es hat die Dimensionen verdreht." },
    { "The gravity intensifies.", "Die Schwerkraft wird st\xe4rker." },
    { "Fire's power was weakened.", "Die St\xe4rke des Feuers wird geschw\xe4"
                                    "cht." },
    { "Electricity's power was weakened.", "Die St\xe4rke der Elektrizit\xe4t wird geschw\xe4"
                                           "cht." },

    // 520

    { "All Pok\xe9mon become locked to the field for the next turn.",
      "Alle Pok\xe9mon sind f\xfcr eine Runde an das Feld gebunden." },
    { "The ion shower disappeared.", "Der elektrisch geladene Niederschlag verschwindet." },
    { "The Magic Room disappeared.", "Der Magieraum verschwindet." },

    { "The Wonder Room disappeared.", "Der Wunderraum verschwindet." },
    { "The twisted dimensions returned to normal.", "Die Dimensionen sind wieder normal." },

    { "The gravity became normal again.", "Die Schwerkraft ist wieder normal." },
    { "Fire's power returned to normal", "Die St\xe4rke des Feuers ist wieder normal" },
    { "Electricity's power returned to normal.",
      "Die St\xe4rke der Elektrizit\xe4t ist wieder normal." },
    { "The Fairy Lock stopped.", "Feenschloss l\xe4sst nach." },
    { "%s is poisoned.", "%s ist vergiftet." },

    // 530

    { "%s is badly poisoned.", "%s ist schwer vergiftet." },
    { "%s is burned.", "%s ist verbrannt." },
    { "%s restores some HP by listening to the soothing sound of its Shell Bell.",
      "%s lauscht dem beruhigenden Klang seines Seegesangs und heilt ein paar KP." },
    { "%s is harmed by its curse.", "Der Fluch schadet %s." },
    { "%s restores some HP using its roots.", "Die Wurzeln von %s heilen ein paar KP." },

    { "%s's Aqua Ring restores some HP.", "Der Wasserring von %s heilt ein paar KP." },
    { "%s is harmed by its nightmare.", "Der Alptraum schadet %s." },
    { "%s activated its %s.", "%s nutzt sein Item %s." },
    { "%s's Air Balloon popped.", "Der Luftballon von %s platzte." },
    { "%s drank its %s.", "%s trank sein %s." },

    // 540

    { "%s dives.", "%s taucht ab." },
    { "%s dug a hole.", "%s gr\xe4"
                        "bt sich ein." },
    { "%s flies up high.", "%s fliegt hoch." },
    { "%s jumps up high.", "%s springt hoch." },
    { "%s <SKYDROP>.", "%s <SKYDROP>" },

    { "%s disappeared.", "%s verschwindet." },
    { "%s continues and crashes.", "%s macht weiter und bricht zusammen." },
    { "It's a one-hit KO!", "Ein K.O.-Treffer!" },
    { "%s lost its focus and cannot attack.",
      "%s hat seinen Fokus verloren und kann nicht angreifen." },
    { "%s's trap failed.", "Die Falle von %s bleibt wirkungslos." },

    // 550

    { "Pok\xe9mon Trainer", "Pok\xe9mon-Trainer" },
    { "%s %s used %s.", "%s %s setzt %s ein." },
    { "You earned $%lu.", "Du gewinnst $%lu." },
    { "You payed $%lu to the winner.", "Du zahlst dem Gewinner $%lu." },
    { "Ash: $%lu grams", "Asche: $%lu Gramm" },

    { "You don't have enough ash.", "Du hast nicht genug Asche." },
    { "%s? I can make one for %lu grams ash.", "%s? Alles klar, das macht dann %lu Gramm Asche." },
    { "Collected ash: %lu grams.", "Gesammelte Asche: %lu Gramm." },
    { "A big boulder that became moveable thanks to your Pok\xe9mon's Strength.",
      "Ein gro\xdf"
      "er Felsen, der durch die St\xe4rke deiner Pok\xe9mon bewegbar wurde." },
    { "You booted up the PC.", "Du hast den PC angeschaltet." },

    // 560

    { "It's a map of the Hoenn region.", "Eine Karte der Hoenn-Region." },
    { "You scurried to a Pok\xe9mon Center,\nprotecting your exhausted\n"
      "and fainted Pok\xe9mon\nfrom further harm|",
      "Du rennst zu einem Pok\xe9mon-Center\num deine ersch\xf6pten und\n"
      "besiegten Pok\xe9mon vor\nweiterem Schaden zu sch\xfctzen|" },
    { "You scurried back home,\nprotecting your exhausted\n"
      "and fainted Pok\xe9mon\nfrom further harm|",
      "Du rennst zur\xfc"
      "ck nach Hause\num deine ersch\xf6pten\n"
      "und besiegten Pok\xe9mon vor\nweiterem Schaden zu sch\xfctzen|" },
    { "Obtained %hu times the\nitem %s.", "%hu-mal das Item\n%s erhalten." },
    { "Obtained %s.", "%s erhalten." },

    { "One %s was planted here.", "Eine %s wurde hier gepflanzt." },
    { "The %s that was planted here has sprouted.",
      "Die %s, die hier geflanzt wurde ist ausgetrieben." },
    { "The %s plant is growing bigger.", "Die %s w\xe4"
                                         "chst." },
    { "The %s plant is in full bloom.", "Die %s bl\xfcht." },
    { "The %s plant carries %hhu berries.", "Die %s tr\xe4gt %hhu Beeren." },

    // 570

    { "Would you like to harvest the berries?", "M\xf6"
                                                "chtest du die Beeren ernten?" },
    { "The soil is very soft.\nWould you like to plant a berry?",
      "Der Boden ist sehr weich\nM\xf6"
      "chtest du eine Beere einpflanzen?" },
    { "Planted a %s.", "%s eingepflanzt." },
    { "You watered the berry plant.", "Du hast die Beerenpflanze gegossen." },
    { "Would you like to water the berries?", "M\xf6"
                                              "chtest du die Beeren gie\xdf"
                                              "en?" },

    { "No. ", "Nr. " },
    { "P O K \xc9 D E X", "P O K \xc9 D E X" },
    { "Hoenn Pok\xe9"
      "Dex",
      "Hoenn-Pok\xe9"
      "Dex" },
    { "National Pok\xe9"
      "Dex",
      "Nationaler Pok\xe9"
      "Dex" },
    { "Seen", "Ges." },

    // 580

    { "Own", "Gef." },
    { "???", "???" },
    { "%s Pok\xe9mon  HT %.1f m  WT %.1f kg", "%s-Pok\xe9mon  GR %.1f m  GW %.1f kg" },
    { "Seen: %hu", "Gesehen: %hu" },
    { "Own: %hu", "Gefangen: %hu" },

    { "Ludicobox", "Kappalobox" },
    { "Little Root", "Kleine Wurzel" },
    { "Abandon Ship", "Verlasse Schiff" },
    { "Battle-Sinnoh", "Kampf-Sinnoh" },
    { "Battles Ahead", "K\xe4mpfe Vorraus" },

    // 590

    { "Battle-Gym Leader", "Kampf-Arenaleiter" },
    { "Battle-Rival", "Kampf-Rivale" },
    { "A Palace for Battles", "Ein Palast der K\xe4mpfe" },
    { "Battle-Roaming Beast", "Kampf-Streifendes Biest" },
    { "Battle-Sky High", "Kampf-Himmelhoch" },

    { "Battle-Team AM", "Kampf-Team AM" },
    { "Battle-Trainer", "Kampf-Trainer" },
    { "Battles to the Top", "Bis nach Ganz Oben" },
    { "Battle-Wild Pok\xe9mon", "Kampf-Wildes Pok\xe9mon" },
    { "Battle-Rare Pok\xe9mon", "Kampf-Spz. Pok\xe9mon" },

    // 600

    { "Darkness Everywhere", "Finsternis \xdc"
                             "berall" },
    { "Lonely Cliff", "Einsame Klippe" },
    { "Mastering the Bike", "Zu Rad" },
    { "Riding the Waves", "Zu See" },
    { "Exploring the Deep Sea", "Abgetaucht" },

    { "Storm of Sand", "Sturm aus Sand" },
    { "Beach Time", "Strand-Zeit" },
    { "Orange Leaves", "Herbstfarben" },
    { "Onwards", "Vorran" },
    { "Golden Radiance", "Goldener Glanz" },

    // 610

    { "Flowers to the World", "Blumen in die Welt" },
    { "Small Gardens", "Kleine G\xe4rten" },
    { "Help!", "Hilfe!" },
    { "A Lake's Secret", "Des Sees Mysterium" },
    { "Where the Sea Begins", "Wo die See Beginnt" },

    { "Round and Round", "Gro\xdf und Rund" },
    { "Celebrating Art", "F\xfcr die Kunst" },
    { "Celebrating the Sea", "F\xfcr die See" },
    { "Falling Meteors", "Fallende Meteore" },
    { "Atop a Volcano", "Auf einem Vulkan" },

    // 620

    { "Mysterious Gifts", "Wundersame Geschenke" },
    { "First Rest", "Erste Pause" },
    { "Whiff of Salt", "Salzige Brise" },
    { "Pok\xe9mon Center", "Pok\xe9mon-Center" },
    { "Pok\xe9mon Gym", "Pok\xe9mon-Arena" },

    { "Friendly Shop", "Pok\xe9-Markt" },
    { "Lab in the Woods", "Labor im Wald" },
    { "To the Mountains", "In den Bergen" },
    { "Travelling Hoenn 8", "Durch Hoenn 8" },
    { "Travelling Hoenn 1", "Durch Hoenn 1" },

    // 630

    { "Travelling Hoenn 2", "Durch Hoenn 2" },
    { "Travelling Hoenn 3", "Durch Hoenn 3" },
    { "Travelling Hoenn 4", "Durch Hoenn 4" },
    { "Travelling Hoenn 5", "Durch Hoenn 5" },
    { "Travelling Hoenn 6", "Durch Hoenn 6" },

    { "Travelling Hoenn 7", "Durch Hoenn 7" },
    { "Science and Nature", "Natur und Wissenschaft" },
    { "Into the Wild", "In die Wildnis" },
    { "Sealed for Eons", "Versiegelt f\xfcr \xc4onen" },
    { "High and Low", "Ebbe und Flut" },

    // 640

    { "Ships and Seaweed", "Schiffe und Seegras" },
    { "White Mountain", "Wei\xdf"
                        "er Berg" },
    { "Hiding", "Versteck" },
    { "Trainer 101", "Wie man Trainer wird" },
    { "Whats Going On?", "Was Passiert?" },

    { "Another Desert", "Eine Andere W\xfcste" },
    { "Stop playback", "Wiedergabe stoppen" },
    { "Choose a Song", "W\xe4hle ein Lied" },
    { "Islands 2", "Eilande 2" },
    { "Battle-Regis", "Kampf-Regis" },

    // 650

    { "Battle-FB", "Kampf-Koryph\xe4"
                   "e" },
    { "Battle-Time Space", "Kampf-Raum Zeit" },
    { "Islands 3", "Eilande 3" },
    { "Moonlight", "Mondschein" },
    { "Hidden Land", "Verborgenes Land" },

    { "%s got poisoned.", "%s wurde vergiftet." },
    { "%s got badly poisoned.", "%s wurde schwer vergiftet." },
    { "%s got burned.", "%s wurde verbrannt." },
    { "%s was frozen solid.", "%s erstarrt zu Eis." },
    { "%s fell asleep.", "%s schl\xe4"
                         "ft ein." },

    // 660
    { "%s was paralyzed.", "%s wurde paralysiert." },
    { "%s was confused.", "%s wurde verwirrt." },
    { "%s gained laser focus.", "%s sch\xe4rft seine Sinne." },
    { "%s levitated on electromagnetism.", "%s schwebt dank Elektromagnetismus." },
    { "%s formed an Aqua Ring.", "%s bildet einen Wasserring." },

    { "%s is getting pumped.", "%s pumpt sich auf." },
    { "%s planted its roots.", "%s pflanzt seine Wurzeln." },
    { "%s was identified.", "%s wurde erkannt." },
    { "%s was planted.", "%s wurde bepflanzt." },
    { "Scent of Grass", "Duft der Wiese" },

    // 670

    { "Atop the Trees", "\xdc"
                        "ber den B\xe4umen" },
    { "In Silence", "In Stille" },
    { "%s disappeared|", "%s verschwand|" },
    { "Exchange", "Eintauschen" },
    { "%s protects itself.", "%s sch\xfctzt sich selbst." },

    { "%s's protection was broken.", "Der Schutz von %s wurde durchbrochen." },
    { "Handed over %hu times the\nitem %s.", "%hu-mal das Item\n%s \xfc"
                                             "bergeben." },
    { "Handed over the %s.", "%s \xfc"
                             "bergeben." },
    { "Distortion World", "Zerrwelt" },
    { "Sailing", "Zu Schiff" },

    // 680

    { "Riding the Waves (alt)", "Zu See (var)" },
    { "An Arena for Battles", "Ein Ring f\xfcr K\xe4mpfe" },
    { "Used %hu times the\nitem %s.", "%hu-mal das Item\n%s eingesetzt." },
    { "Used the %s.", "%s eingesetzt." },
};
