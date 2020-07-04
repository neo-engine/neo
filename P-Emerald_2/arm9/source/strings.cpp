/*
Pok\xe9mon neo
------------------------------

file        : strings.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
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

const char* SAVE::EPISODE_NAMES[ MAX_SPECIAL_EPISODES + 1 ][ LANGUAGES ]
    = { { "Dev's Heaven", "Dev's Heaven" } };

const char* LANGUAGE_NAMES[ LANGUAGES ] = { "English", "Deutsch" };

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
    { "Tokens of god being|" },
};

#endif

const char* const MONTHS[ 12 ][ LANGUAGES ]
    = { { "Jan.", "Jan." }, { "Feb.", "Feb." }, { "Mar.", "M\xe4r." }, { "Apr.", "Apr." },
        { "May", "Mai" },   { "June", "Juni" }, { "July", "Juli" },    { "Aug.", "Aug." },
        { "Sep.", "Sep." }, { "Oct.", "Okt." }, { "Nov.", "Nov." },    { "Dec.", "Dez." } };

const char* const ACHIEVEMENTS[ MAX_ACHIEVEMENTS ][ LANGUAGES ] = {
    { "", "" },
    { "Won the Stone Badge on %s", "Steinorden erhalten am %s" },
    { "Won the Knuckle Badge on %s", "Kn\f6"
                                     "chelorden erhalten am %s" },
    { "Won the Dynamo Badge on %s", "Dynamo-Orden erhalten am %s" },
    { "Won the Heat Badge on %s", "Hitzeorden erhalten am %s" },
    { "Won the Balance Badge on %s", "Balanceorden erhalten am %s" },
    { "Won the Feather Badge on %s", "Federorden erhalten am %s" },
    { "Won the Mind Badge on %s", "Mentalrden erhalten am %s" },
    { "Won the Rain Badge on %s", "Schauerorden erhalten am %s" },
    { "Entered the Hall of Fame on %s", "Einzug in die Ruhmeshalle am %s" },
    { "Won the S. Knowledge Sym. on %s", "S-Kenntnissym. erhalten am %s" },
    { "Won the G. Knowledge Sym. on %s", "G-Kenntnissym. erhalten am %s" },
    { "Won the S. Guts Symbol on %s", "S-Grobsymbol erhalten am %s" },
    { "Won the G. Guts Symbol on %s", "G-Grobsymbol erhalten am %s" },
    { "Won the S. Tactics Symbol on %s", "S-Taktiksymbol erhalten am %s" },
    { "Won the G. Tactics Symbol on %s", "G-Taktiksymbol erhalten am %s" },
    { "Won the S. Luck Symbol on %s", "S-Lossymbol erhalten am %s" },
    { "Won the G. Luck Symbol on %s", "G-Lossymbol erhalten am %s" },
    { "Won the S. Spirits Symbol on %s", "S-Seelensymbol erhalten am %s" },
    { "Won the G. Spirits Symbol on %s", "G-Seelensymbol erhalten am %s" },
    { "Won the S. Brave Symbol on %s", "S-Bravursymbol erhalten am %s" },
    { "Won the G. Brave Symbol on %s", "G-Bravursymbol erhalten am %s" },
    { "Won the S. Ability Symbol on %s", "S-Anlagesymbol erhalten am %s" },
    { "Won the G. Ability Symbol on %s", "G-Anlagesymbol erhalten am %s" },
    { "Battle Frontier cleared on %s", "Kampfzone bezwungen am %s" },
};

const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ] = {
    { "No data.", "Keine Daten." },
    { "Faraway Place", "Entfernter Ort" },
    { "Unknown", "Unbekannt" },
    { "%sUse %s?", "%sM\xf6"
                   "chtest du %s nutzen?" },
    { "The repel wore off.", "Der Schutz ist aufgebraucht." },

    { "Just an old Pok\xe9 Ball", "Doch nur ein alter Pok\xe9"
                                    "ball" },
    { "You fished a Pok\xe9mon!", "Du hast ein Pok\xe9mon geangelt!" },
    { "Map border.\nReturn, or you\nwill get lost!",
      "Ende der Kartendaten.\nKehr um, sonst\nverirrst du dich!" },
    { "\nSomething's on the hook!", "\nDa ist etwas am Haken!" },
    { "It disappeared", "Es ist entkommen" },

    // 10

    { "%s used %s!", "%s setzt %s ein!" },
    { "Items", "Items" },
    { "Medicine", "Medizin" },
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
    { "Level %3d\n Fainted", "Level %3d\n Besiegt" },

    // 40

    { "Possible", "M\xf6glich" },
    { "Not possible", "Nicht m\xf6glich" },
    { "No item", "Kein Item" },
    { "No items", "Keine Items" },
    { "Give", "Geben" },

    { "Apply", "Anwenden" },
    { "Register", "Registrieren" },
    { "Use", "Einsetzen" },
    { "Toss", "Wegwerfen" },
    { "Select a move!", "Welche Attacke?" },

    // 50

    { "Used %s.", "%s eingesetzt." },
    { "What?\n%s is evolving!", "Nanu?\n%s entwickelt sich!" },
    { "Congratulations! Your %s\nevolved into %s!", "Gl\xfc"
                                                    "ckwunsch! Dein %s\nwurde zu einem %s!" },
    { "It would be wasted|", "Es w\xfcrde keine Wirkung haben|" },
    { "%s already has an item.\nSwap the items?",
      "%s tr\xe4gt bereits ein Item.\nSollen die Items getauscht werden?" },

    { "You can't choose\nthis item.", "Dieses Item kann nicht\nausgew\xe4hlt werden." },
    { "Choose %s?", "%s ausw\xe4hlen?" },
    { "Do what with %s?", "Was tun mit %s?" },
    { "You can't use\nthat right now.", "Das kann jetzt nicht\neingesetzt werden." },
    { "Pok\xe9mon team", "Pok\xe9mon-Team" },

    // 60

    { "Cache", "Zwischenablage" },
    { "All boxes", "Alle Boxen" },
    { "New name for box %s", "Name f\xfcr Box %s" },
    { "Repel activated.", "Schutz eingesetzt." },
    { "Super Repel activated.", "Superschutz eingesetzt." },

    { "Max Repel activated.", "Top-Schutz eingesetzt." },
    { "Exp. All deactivated.", "EP-Teiler ausgeschaltet." },
    { "Exp. All activated.", "EP-Teiler eingeschaltet." },
    { "Coins: %lu.", "M\xfcnzen: %lu." },
    { "BP: %lu.", "Kampfpunkte: %lu." },

    // 70

    { "TOUCH TO START", "BER\x9A"
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
    { "This slot contains a saved game.\nOverride it?",
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
    { "into the %s bag.`", "%s-Tasche verstaut.`" },
    { "Stored %s %s", "%s %s in der" },
    { "Stored %s", "%s in der" },
    { "Stored %3d %s", "%3d %s in der" },

    // 90

    { "The scent vanished", "Der Duft verstrich wirkungslos" },
    { "Pok\xe9Nav", "Pok\xe9Nav" },
    { " Would you like to save your progress?", " M\xf6"
                                                    "chtest du deinen Fortschritt sichern?" },
    { "Saving a lot of data. Don't turn off the power.\n%s", "Speichere eine Menge Daten.\n%s" },
    { "Saving complete!", "Speichern erfolgreich!" },

    { "An error occured.\nProgress not saved.", "Es trat ein Fehler auf\nSpiel nicht gesichert." },
    { "No %s available.\nRemove the icon?",
      "Kein Exemplar des Items\n%s vorhanden.\nIcon entfernen?" },
    { "Recently used items\nwill appear here.", "Hier erscheinen zuletzt\neingesetzte Items." },
    { "You may register an\nitem to the button Y.", "Du kannst ein Item\nauf Y registrieren." },
    { "%s used %s!", "%s setzt %s ein!" },

    // 100

    { "You can't use this\nmove right now.", "Diese Attacke kann jetzt\nnicht eingesetzt werden." },
    { "Took %s\nfrom %s.", "%s von\n%s verstaut." },
    { "%s already\nknows %s!", "%s beherrscht\n%s bereits!" },
    { "%s learned\n%s!", "%s erlernt\n%s!" },
    { "%s already knows 4 moves. Forget a move?",
      "%s beherrscht bereits 4 Attacken. Soll eine verlernt werden?" },

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
    { "Importing data", "Lade Spielstand" },
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
    { "[TRAINER] ([TCLASS]) sent\nout %s![A]",
      "[TRAINER] ([TCLASS]) schickt\n%s in den Kampf![A]" },

    // 140

    { "Go [OWN%d]![A]", "Los [OWN%d]![A]" },
    { "Do you want to give\na nick name to %s?", "M\xf6"
                                                 "chtest du dem %s\neinen Spitznamen geben?" },
    { "Choose a nick name!", "W\xe4hle einen Spitznamen!" },
    { "You are challenged by\n%s %s![END]", "Eine Herausforderung von\n%s %s![END]" },
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
    { "%s gained %lu EXP.[A]", "%s gewinnt %lu E.-Punkte.[A]" },
    { "%s advanced\nto level %d.[A]", "%s erreicht Level %d.[A]" },
    { "The %s enabled\n%s to go first!", "%.0s%s agiert dank\neiner %s zuerst!" },

    // 170

    { "The wild %s fled!", "%s (wild) floh." },
    { "[TRAINER] ([TCLASS])\nused %s.[A]", "[TRAINER] ([TCLASS]) setzt\n%s ein.[A]" },
    { "It got wasted|", "Es hat keine Wirkung|" },
    { "Used %s.", "%s eingesetzt." },
    { "%s's data was\nregistred to the Pok\xe9"
      "dex.",
      "Die Daten von %s\nwurden im Pok\xe9"
      "dex gespeichert." },

    { "%s was sent to\nthe Pok\xe9mon Storage System.",
      "%s wurde an das\nPok\xe9mon-Lagerungssystem\ngeschickt." },
    { "Box %s is full.", "Box %s ist voll." },
    { "%s was deposited\nto Box %s.", "%s wurde in\nBox %s abgelegt." },
    { "There is no space left\nfor additional Pok\xe9mon.",
      "Du hast keinen Platz\nf\xfcr weitere Pok\xe9mon." },
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
    { "%s's %s did not change.", "%.0s%s von %s blieb unver\xe4ndert." },
    { "%s's %s rose.", "%.0s%s von %s stieg." },
    { "%s's %s rose sharply.", "%.0s%s von %s stieg stark." },
    { "%s's %s rose drastically.", "%.0s%s von %s stieg drastisch." },

    // 260

    { "%s's %s fell.", "%.0s%s von %s sank." },
    { "%s's %s harshly fell.", "%.0s%s von %s sank stark." },
    { "%s's %s severely fell.", "%.0s%s von %s sank drastisch." },
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
                             "ck" },
    { "That is enough %s.", "Das reicht, %s." },
    { "%s returned to %s.", "%s kam zu %s zur\xfc"
                            "ck." },

    { "%s withdrew %s.", "%s rief %s zur\xfc"
                         "ck." },
    { "%s recharges.", "%s l\xe4"
                       "dt sich wieder auf." },
    { "%s became confused.", "%s ist vor Ersch\xf6pfung verwirrt." },
    { "%s ate its %s. Its confusion vanished.", "%s a\xdf die %s. Es ist nicht mehr verwirrt." },
    { "%s ate its %s.", "%s a\xdf die %s." },

    // 280

    { "%s remains unaffected.", "%s blieb unber\xfchrt." },
    { "%s passes its %s. The %s found its purpose.",
      "%s gibt seine %s weiter. Die %s fand ihre Bestimmung." },
    { "The %s protects %s.", "Die %s sch\xfctzt %s." },
    { "%s obtained the %s.", "%s erhielt das Item %s." },
    { "It has no effect on %s|", "Es hat keine Wirkung auf %s|" },

    { "It is super effective on %s.", "Es ist sehr effektiv gegen %s." },
    { "It is not very effective on %s.", "Es ist nicht sehr effektiv gegen %s." },
    { "%s is hit by the recoil.", "Der R\xfc"
                                  "cksto\xdf schadet %s." },
    { "%s restores some HP.", "%s heilt ein paar KP." },
    { "%s fainted.", "%s wurde besiegt." },

    // 290

    { "It missed %s.", "%s wich aus." },
    { "A critical hit!", "Ein Volltreffer!" },
    { "%s prepares a move.", "%s bereitet einen Angriff vor." },
    { "%s is confused.", "%s ist verwirrst." },
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
    { "This tree looks like it can be\ncut down. ", "Ein kleiner Baum.\n" },
    { "This rock appears to be breakable.\n", "Ein kleiner Felsen.\n" },

    { "A huge whirlpool.\n", "Ein riesiger Strudel.\n" },
    { "The water is a deep blue...\n", "Das Wasser ist tiefblau...\n" },
    { "The sea is deep here.\n", "Das Wasser ist sehr tief.\n" },
    { "It's a big boulder.\n", "Ein großer Felsen.\n" },
    { "The wall is very rocky...\n", "Eine steile Felswand.\n" },

    // 320

    { "It's a large Waterfall.\n", "Ein großer Wasserfall.\n" },
    { "Something moved!\n", "Da hat sich etwas bewegt!\n" },
    { "Light is shining from the\nsurface. ", "Licht scheint von der\nOberfl\xe4"
                                              "che. " },
    { "Select", "Ausw\xe4hlen" },
    { "Status", "Status" },

    { "Give Item", "Item geben" },
    { "Take Item", "Item nehmen" },
    { "Use Item", "Item nutzen" },
    { "Swap", "Austauschen" },
    { "Pok\xe9"
      "dex",
      "Pok\xe9"
      "dex" },

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
      "dex",
      "Pok\xe9"
      "dex" },

    // 410

    { "Adventure started on %s", "Abenteuer gest. am %s" },
    { "Last save created in/at %s\non %s, %02hhu:%02hhu",
      "Zuletzt gesp. in/bei %s\nam %s um %02hhu:%02hhu." },
    { "No save data in this slot.", "Kein Spielstand in diesem Slot."},
    { "Pok\xe9mon", "Pok\xe9mon" },
    { "Pok\xe9""dex", "Pok\xe9""dex" },

    { "Bag", "Beutel" },
    { "Trainer ID", "Trainerpass" },
    { "Save", "Speichern" },
    { "Options", "Optionen" },
    { "Text Speed", "Textgeschw."},

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
    { "Sound disabled", "Musik deaktiviert"},
};
