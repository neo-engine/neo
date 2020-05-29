/*
Pok\xe9mon neo
------------------------------

file        : strings.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
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

const char* const SAVE::CHAPTER_NAMES[ 2 * MAX_CHAPTERS ][ LANGUAGES ] = {
    {"Prologue", "Prolog"},     {"I Choose You!", "Ich W\xe4hle Dich!"},
    {"Chapter 1", "Kapitel 1"}, {"(tba)", "(tba)"},
    {"Chapter 2", "Kapitel 2"}, {"(tba)", "(tba)"},
    {"Chapter 3", "Kapitel 3"}, {"(tba)", "(tba)"},
    {"Chapter 4", "Kapitel 4"}, {"(tba)", "(tba)"},

    {"Chapter 5", "Kapitel 5"}, {"(tba)", "(tba)"},
    {"Chapter 6", "Kapitel 6"}, {"(tba)", "(tba)"},
    {"Chapter 7", "Kapitel 7"}, {"(tba)", "(tba)"},
    {"Chapter 8", "Kapitel 8"}, {"(tba)", "(tba)"},
    {"Chapter 9", "Kapitel 9"}, {"(tba)", "(tba)"},

    {"Final", "Letztes"},       {"Pok\xe9mon", "Pok\xe9mon"},
    {"Epilogue", "Epilog"},     {"Life Goes On", "Das Leben Geht Weiter"},
};

const char* SAVE::EPISODE_NAMES[ LANGUAGES ][ MAX_SPECIAL_EPISODES ]
    = {{"Dev's Heaven"}, {"Dev's Heaven"}};

const char* LANGUAGE_NAMES[ LANGUAGES ] = {"English", "Deutsch"};

#ifdef DESQUID
const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ LANGUAGES ] = {
    {"Desquid"},
    {"Basic Data"},
    {"Status"},
    {"Egg Status"},
    {"Nature"},

    {"HP/PP"},
    {"Moves/Item"},
    {"IV"},
    {"EV"},
    {"Duplicate"},

    // 10

    {"Delete"},
    {"Heal"},
    {"Species: %s"},
    {"Forme: %s"},
    {"Ability: %s"},

    {"Sleep"},
    {"Poison"},
    {"Burn"},
    {"Freeze"},
    {"Paralyzed"},

    // 20

    {"Toxic"},
    {"Level"},
    {"EXP"},
    {"Shininess"},
    {"Is Egg?"},

    {"Steps/Happiness"},
    {"Obt. at %s"},
    {"Hatd at %s"},
    {"OT Id"},
    {"OT SId"},

    // 30

    {"Nature: %s"},
    {"%2s Attack"},
    {"%2s Defense"},
    {"%2s Speed"},
    {"%2s Special Atk"},

    {"%2s Special Def"},
    {"HP"},
    {"Attack"},
    {"Defense"},
    {"Special Attack"},

    // 40

    {"Special Defense"},
    {"Speed"},
    {"%s"},
    {"Held: %s"},
    {"PP(%s)"},

    {"Fateful Enc."},
};

#endif

const char* const MONTHS[ 12 ][ LANGUAGES ] = {
    {"Jan.", "Jan."},
    {"Feb.", "Feb."},
    {"Mar.", "M\xe4r."},
    {"Apr.", "Apr."},
    {"May", "Mai"},
    {"June", "Juni"},
    {"July", "Juli"},
    {"Aug.", "Aug."},
    {"Sep.", "Sep."},
    {"Oct.", "Okt."},
    {"Nov.", "Nov."},
    {"Dec.", "Dez."}
};

const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ] = {
    {"No data.", "Keine Daten."},
    {"Faraway Place", "Entfernter Ort"},
    {"Unknown", "Unbekannt"},
    {"%sUse %s?", "%sM\xf6"
                  "chtest du %s nutzen?"},
    {"The repel wore off.", "Der Schutz ist aufgebraucht."},

    {"Just an old Pok\xe9 Ball", "Doch nur ein alter Pok\xe9"
                                   "ball"},
    {"You fished a Pok\xe9mon!", "Du hast ein Pok\xe9mon geangelt!"},
    {"Map border.\nReturn, or you\nwill get lost!",
     "Ende der Kartendaten.\nKehr um, sonst\nverirrst du dich!"},
    {"\nSomething's on the hook!", "\nDa ist etwas am Haken!"},
    {"It disappeared", "Es ist entkommen"},

    // 10

    {"%s%s used %s![A]", "%s%s setzt\n%s ein![A]"},
    {"Items", "Items"},
    {"Medicine", "Medizin"},
    {"TM/HM", "TM/VM"},
    {"Berries", "Beeren"},

    {"Key Items", "Basis-Items"},
    {"Firmness: %s", "G\xfcte: %s"},
    {"Hard", "Hart"},
    {"Soft", "Weich"},
    {"Super hard", "Steinhart"},

    // 20

    {"Super soft", "Sehr weich"},
    {"Very hard", "Sehr hart"},
    {"Normal", "Normal"},
    {"Size:%4.1fcm", "Gr\xf6\xdf"
                     "e:%4.1fcm"},
    {"Spicy", "Scharf"},

    {"Dry", "Trocken"},
    {"Sweet", "S\xfc\xdf"},
    {"Bitter", "Bitter"},
    {"Sour", "Sauer"},
    {"Type", "Typ"},

    // 30

    {"Categ.", "Kateg."},
    {"PP", "AP"},
    {"Strength", "St\xe4rke"},
    {"Accuracy", "Genauigkeit"},
    {"Egg", "Ei"},

    {"Already\nlearned", "Bereits\nerlernt"},
    {"Possible", "Erlernbar"},
    {"Not\npossible", "Nicht\nerlernbar"},
    {"Level %3d\n%3d/%3d HP", "Level %3d\n%3d/%3d KP"},
    {"Level %3d\n Fainted", "Level %3d\n Besiegt"},

    // 40

    {"Possible", "M\xf6glich"},
    {"Not\npossible", "Nicht\nm\xf6glich"},
    {"No item", "Kein Item"},
    {"No items", "Keine Items"},
    {"Give", "Geben"},

    {"Apply", "Anwenden"},
    {"Register", "Registrieren"},
    {"Use", "Einsetzen"},
    {"Trash", "Wegwerfen"},
    {"Select a move!", "Welche Attacke?"},

    // 50

    {"Used %s on %s.", "%s auf %s angewendet."},
    {"%s is evolving", "%s entwickelt sich"},
    {"and became a(n)\n%s!", "und wurde zu einem\n%s!"},
    {"It would be\nwasted", "Es w\xfcrde keine\nWirkung haben"},
    {"%s holds\n%s already.\nSwap the items?",
     "%s tr\xe4gt bereits\ndas Item %s.\nSollen die Items getauscht werden?"}, // TODO

    {"You can't choose\nthis item.", "Dieses Item kann nicht\nausgew\xe4hlt werden."},
    {"Choose %s?", "%s ausw\xe4hlen?"},
    {"Do what with %s?", "Was tun mit %s?"},
    {"You can't use\nthat right now.", "Das kann jetzt nicht\neingesetzt werden."},
    {"Pok\xe9mon team", "Pok\xe9mon-Team"},

    // 60

    {"Cache", "Zwischenablage"},
    {"All boxes", "Alle Boxen"},
    {"New name for box %s", "Name f\xfcr Box %s"},
    {"Repel activated.", "Schutz eingesetzt."},
    {"Super Repel activated.", "Superschutz eingesetzt."},

    {"Max Repel activated.", "Top-Schutz eingesetzt."},
    {"Exp. All deactivated.", "EP-Teiler ausgeschaltet."},
    {"Exp. All activated.", "EP-Teiler eingeschaltet."},
    {"Coins: %lu.", "M\xfcnzen: %lu."},
    {"BP: %lu.", "Kampfpunkte: %lu."},

    // 70

    {"         TOUCH TO START", "     BER\x9A"
                                "HRE, UM ZU STARTEN"},
    {"Continue", "Weiter"},
    {"New Game", "Neues Spiel"},
    {"Special Episodes", "Spezialepisoden"},
    {"Import Save", "Spiel-Import"},

    {"Continue your adventure", "Setze dein Abenteuer fort"},
    {"Start a new adventure", "Starte ein neues Abenteuer"},
    {"Learn your companion's stories", "Erfahre mehr \xfc"
                                       "ber deine Begleiter"},
    {"Revive myths of old", "Setze alte Abenteuer fort"},
    {"This slot contains a saved game.\nOverride it?",
     "Es existiert bereits ein Spielstand.\nSoll dieser \xfc"
     "berschrieben werden?"},

    // 80

    {"Yes", "Ja"},
    {"No", "Nein"},
    {"(empty)", "(leer)"},
    {"Transfer", "Transfer"},
    {"Special", "Spezial"},

    {"The language is currently\nset to English.\nChange the language setting?",
     "Die eingestellte Sprache\nist Deutsch.\nSprache \xe4ndern?"},
    {"into the %s bag.`", "%s-Tasche verstaut.`"},
    {"Stored %s %s", "%s %s in der"},
    {"Stored %s", "%s in der"},
    {"Stored %3d %s", "%3d %s in der"},

    // 90

    {"The scent vanished", "Der Duft verstrich wirkungslos"},
    {"Pok\xe9Nav", "Pok\xe9Nav"},
    {" Would you like to\n save your progress?\n", " M\xf6"
                                                   "chtest du deinen\n Fortschritt sichern?\n"},
    {"Save?\nFrom an emulator?!", "Speichern?\nIn einem Emulator?!"},
    {"Saved successfully!", "Speichern erfolgreich!"},

    {"An error occured.\nProgress not saved.", "Es trat ein Fehler auf\nSpiel nicht gesichert."},
    {"No %s available.\nRemove the icon?",
     "Kein Exemplar des Items\n%s vorhanden.\nIcon entfernen?"},
    {"Recently used items\nwill appear here.", "Hier erscheinen zuletzt\neingesetzte Items."},
    {"You may register an\nitem to the button Y.", "Du kannst ein Item\nauf Y registrieren."},
    {"%s used %s!", "%s setzt %s\nein!"},

    // 100

    {"You can't use this\nmove right now.", "Diese Attacke kann jetzt\nnicht eingesetzt werden."},
    {"Took %s\nfrom %s.", "%s von\n%s verstaut."},
    {"%s already\nknows %s!", "%s beherrscht\n%s bereits!"},
    {"%s learned\n%s!", "%s erlernt\n%s!"},
    {"%s already\nknows 4 moves.\nForget a move?",
     "%s beherrscht\nbereits 4 Attacken.\nSoll eine verlernt werden?"},

    {"Select a move!", "Welche Attacke?"},
    {"%s can't\nforget %s!", "%s kann\n%s nicht vergessen!"},
    {"%s can't\nlearn %s!", "%s kann\n%s nicht erlernen!"},
    {"%hhu badges", "%hhu Orden"},
    {"Pick a language", "W\xe4hle eine Sprache"},

    // 110

    {"Choose an episode", "W\xe4hle eine Speizalepisode"},
    {"Welcome to Special Episode 0:\nDeveloper's Heaven.",
     "Willkommen zur Spezialepisode 0:\nDeveloper's Heaven"},
    {"In this episode, you will gain\nthe mighty powers of the\ndev himself.",
     "In dieser Episode erh\xe4ltst du\ndie F\xe4higkeiten des Entwicklers."},
    {"Do whatever you want.", "Mach damit, was immer du m\xf6"
                              "chtest."},
    {"Do you want to import your\nsave from the GBA game?",
     "M\xf6"
     "chtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?"},

    {"Importing a save file will\ncopy all Pok\xe9mon and items\nfrom the GBA game.",
     "Beim Importieren werden alle\nPok\xe9mon und Items vom\nGBA-Modul kopiert."},
    {"Further, you will start at\nthe same position as you\nleft off in the GBA game.",
     "Du wirst das Spiel an der\nselben Stelle wie auf dem\nGBA-Modul fortsetzen."},
    {"No data will be written to\nthe GBA Game Pak.",
     "Es werden keine Daten auf\ndas GBA-Modul geschrieben."},
    {"However, proceed at your\nown risk.",
     "Trotzdem geschieht das\nImportieren auf eigene\nGefahr."},
    {"No compatible cartrige found.\nAborting.",
     "Kein kompatibles GBA-Modul\ngefunden.\nBreche ab."},

    // 120

    {"Proceed?", "Fortfahren?"},
    {"Importing data", "Lade Spielstand"},
    {"An error occured.\nAborting.", "Ein Fehler ist aufgetreten.\nKehre zum Hauptmen\xfc zur\xfc"
                                     "ck."},
    {"Completed.", "Abgeschlossen."},
    {"Entries", "Eintr\xe4ge"},

    {"All", "Alle"},
    {"HP", "KP"},
    {"Attack", "Angriff"},
    {"Defense", "Vert."},
    {"Sp. Atk", "Sp. Ang."},

    // 130

    {"Sp. Def", "Sp. Ver."},
    {"Speed", "Init."},
    {"f. %s", "aus %s"},
    {"female", "weiblich"},
    {"male", "m\xe4nnlich"},

    {"(shining)", "(schillernd)"},
    {"Fainted", "Besiegt"},
    {"Statistics", "Statuswerte"},
    {"Lv.%3i", "Lv.%3i"},
    {"[TRAINER] ([TCLASS]) sent\nout %s![A]", "[TRAINER] ([TCLASS]) schickt\n%s in den Kampf![A]"},

    // 140

    {"Go [OWN%d]![A]", "Los [OWN%d]![A]"},
    {"Do you want to give\na nick name to %s?", "M\xf6"
                                                "chtest du dem %s\neinen Spitznamen geben?"},
    {"Choose a nick name!", "W\xe4hle einen Spitznamen!"},
    {"You are challenged by\n%s %s![END]", "Eine Herausforderung von\n%s %s![END]"},
    {"Frzn", "Eingfr"},

    {"Prlz", "Prlyse"},
    {"Burn", "Verbrng"},
    {"Slp", "Schlaf"},
    {"Psn", "Gift"},
    {"Switch", "Aussenden"},

    // 150

    {"In Battle", "Bereits im Kampf"},
    {"Cannot Battle", "Schon besiegt"},
    {"To be sent", "Schon ausgew\xe4hlt"},
    {"Eggs can't", "Ein Ei kann"},
    {"battle!", "nicht k\xe4mpfen!"},

    {"Status", "Bericht"},
    {"Moves", "Attacken"},
    {"ATK", "ANG"},
    {"DEF", "VER"},
    {"SAT", "SAN"},

    // 160

    {"SDF", "SVE"},
    {"SPD", "INI"},
    {"What will %s do?", "Was soll %s tun?"},
    {"You got away safely.[A]", "Du bist entkommen.[A]"},
    {"You couldn't escape[A]", "Flucht gescheitert[A]"},

    {"Choose a target!", "Welches PKMN angreifen?"},
    {"Choose a Pok\xe9mon.", "Welches Pok\xe9mon?"},
    {"%s gained %lu EXP.[A]", "%s gewinnt %lu E.-Punkte.[A]"},
    {"%s advanced\nto level %d.[A]", "%s erreicht Level %d.[A]"},
    {"The quick claw enabled\n%s%s to go first![A]",
     "%s%s agiert dank\neiner Flinkklaue zuerst![A]"},

    // 170

    {"%s of %s%s applies.[A]", "%s von %s%s wirkt.[A]"},
    {"[TRAINER] ([TCLASS])\nused %s.[A]", "[TRAINER] ([TCLASS]) setzt\n%s ein.[A]"},
    {"It got wasted[A]", "Es hat keine Wirkung[A]"},
    {"Used %s.[A]", "%s eingesetzt.[A]"},
    {"%s's data was\nregistred to the Pok\xe9 Dex.[A]", "Die Daten von %s\nwurden im Pok\xe9"
                                                        "Dex gespeichert.[A]"},

    {"%s was sent to\nthe Pok\xe9mon Storage System.[A]",
     "%s wurde an das\nPok\xe9mon-Lagerungssystem\ngeschickt.[A]"},
    {"Box %s is full.[A]", "Box %s ist voll.[A]"},
    {"%s was deposited\nto Box %s.[A]", "%s wurde in\nBox %s abgelegt.[A]"},
    {"There is no space left\nfor additional Pok\xe9mon.[A]",
     "Du hast keinen Platz\nf\xfcr weitere Pok\xe9mon.[A]"},
    {"%s was released.[A]", "%s wurde wieder\nfreigelassen.[A]"},

    // 180

    {"FIGHT", "Kampf"},
    {"BAG", "Beutel"},
    {"POK\xe9MON", "Pok\xe9mon"},
    {"RUN", "Flucht"},
    {"Pwr", "Stk"},

    {"Meteor Falls", "Meteorf\xe4lle"},
    {"\xd", "\xe"}, // HP icon
    {"Hardy", "Robust"},
    {"Lonely", "Einsam"},
    {"Brave", "Mutig"},

    // 190

    {"Adamant", "Hart"},
    {"Naughy", "Frech"},
    {"Bold", "K\xfc"
             "hn"},
    {"Docile", "Sanft"},
    {"Relaxed", "Locker"},

    {"Impish", "Pfiffig"},
    {"Lax", "Lasch"},
    {"Timid", "Scheu"},
    {"Hasty", "Hastig"},
    {"Serious", "Ernst"},

    // 200

    {"Jolly", "Froh"},
    {"Naive", "Naiv"},
    {"Modest", "M\xe4"
               "\xdf"
               "ig"},
    {"Mild", "Mild"},
    {"Quiet", "Ruhig"},

    {"Bashful", "Zaghaft"},
    {"rash", "Hitzig"},
    {"Calm", "Still"},
    {"Gentle", "Zart"},
    {"Sassy", "Forsch"},

    // 210

    {"Careful", "Sacht"},
    {"Quirky", "Kauzig"},
    {"Loves to eat.", "Liebt es, zu essen."},
    {"Often dozes off.", "Nickt oft ein."},
    {"Nods off a lot.", "Schl\xe4"
                       "ft gerne."},

    {"Scatters things often.", "Macht oft Unordnung."},
    {"Likes to relax.", "Liebt es zu entspannen."},
    {"Proud of its power.", "Ist stolz auf seine St\xe4"
                           "rke."},
    {"Likes to thrash about.", "Pr\xfc"
                              "gelt sich gerne."},
    {"A little quick tempered.", "Besitzt Temperament."},

    // 220

    {"Likes to fight.", "Liebt es zu k\xe4"
                       "mpfen."},
    {"Quick tempered.", "Ist impulsiv."},
    {"Sturdy body.", "Hat einen robusten K\xf6"
                    "rper."},
    {"Capable of taking hits.", "Kann Treffer gut verkraften."},
    {"Highly persistent.", "Ist \xe4"
                          "u\xdf"
                          "erst ausdauernd."},

    {"Good endurance.", "Hat eine gute Ausdauer."},
    {"Good perseverance.", "Ist beharrlich."},
    {"Highly curious.", "Ist sehr neugierig."},
    {"Mischievous.", "Ist hinterh\xe4"
                    "ltig."},
    {"Thoroughly cunning.", "Ist \xe4"
                           "u\xdf"
                           "erst gerissen."},

    // 230

    {"Often lost in thought.", "Ist oft in Gedanken."},
    {"Very finicky.", "Ist sehr pedantisch."},
    {"Strong willed.", "Besitzt starken Willen."},
    {"Somewhat vain.", "Ist etwas eitel."},
    {"Strongly defiant.", "Ist sehr aufs\xe4"
                         "ssig."},

    {"Hates to lose.", "Hasst Niederlagen."},
    {"Somewhat stubborn.", "Ist dickk\xf6"
                          "pfig."},
    {"Likes to run.", "Liebt es, zu rennen."},
    {"Alert to sounds.", "Achtet auf Ger\xe4"
                        "usche."},
    {"Impetuous and silly.", "Ist ungest\xfc"
                            "m und einf\xe4"
                            "ltig."},

    // 240

    {"Somewhat of a clown.", "Ist fast wie eine Clown."},
    {"Quick to flee.", "Fl\xfc"
                      "chtet schnell."},
    {"spicy", "scharf"},
    {"sour", "saur"},
    {"sweet", "s\xfc\xdf"},

    {"dry", "trocken"},
    {"bitter", "bitter"},
    {"all", "all"},
    {"", "[OWN1:ABILITY] von\n[OWN1] wirkt![A][CLEAR][OWN1] heilt sich.[A]"},
    {"", "[OWN2:ABILITY] von\n[OWN2] wirkt![A][CLEAR][OWN2] heilt sich.[A]"},

    // 250

    {"", "[OPP1:ABILITY] von\n[OPP1] wirkt![A][CLEAR][OPP1] heilt sich.[A]"},
    {"", "[OPP2:ABILITY] von\n[OPP2] wirkt![A][CLEAR][OPP2] heilt sich.[A]"},
    {"", "[OWN1:ABILITY] von\n[OWN1] wirkt![A][CLEAR]KP von [OWN1]\nregenerieren sich.[A]"},
    {"", "[OWN2:ABILITY] von\n[OWN2] wirkt![A][CLEAR]KP von [OWN2]\nregenerieren sich.[A]"},
    {"", "[OPP1:ABILITY] von\n[OPP1] wirkt![A][CLEAR]KP von [OPP1]\nregenerieren sich.[A]"},

    {"", "[OPP2:ABILITY] von\n[OPP2] wirkt![A][CLEAR]KP von [OPP2]\nregenerieren sich.[A]"},
    {"", "Der Hagel schadet [OWN1].[A]"},
    {"", "Der Hagel schadet [OWN2].[A]"},
    {"", "Der Hagel schadet\n[OPP1].[A]"},
    {"", "Der Hagel schadet\n[OPP2].[A]"},

    // 260

    {"", "Der Sandsturm schadet [OWN1].[A]"},
    {"", "Der Sandsturm schadet [OWN2].[A]"},
    {"", "Der Sandsturm schadet\n[OPP1].[A]"},
    {"", "Der Sandsturm schadet\n[OPP2].[A]"},
    {"", "[OWN1:ABILITY] von\n[OWN1] wirkt![A][CLEAR]Das Sonnenlicht\nschadet [OWN1].[A]"},

    {"", "[OWN2:ABILITY] von\n[OWN2] wirkt![A][CLEAR]Das Sonnenlicht\nschadet [OWN2].[A]"},
    {"", "[OPP1:ABILITY] von\n[OPP1] wirkt![A][CLEAR]Das Sonnenlicht\nschadet [OPP1].[A]"},
    {"", "[OPP2:ABILITY] von\n[OPP2] wirkt![A][CLEAR]Das Sonnenlicht\nschadet [OPP2].[A]"},
    {"", "Es regnet.[A]"},
    {"", "Es hagelt.[A]"},

    // 270

    {"", "Es herrscht dichter Nebel\x85[A]"},
    {"", "Der Sandsturm wütet.[A]"},
    {"", "Gleißendes Sonnenlicht.[A]"},
    {"", "Enormer Regen.[A]"},
    {"", "Extremes Sonnenlicht.[A]"},

    {"", "Starke Winde wehen.[A]"},
    {"", "Es hat aufgehört zu regnen.[A]"},
    {"", "Es hat aufgehört zu hageln.[A]"},
    {"", "Der Nebel verzog sich.[A]"},
    {"", "Der Sandsturm legt sich.[A]"},

    // 280

    {"", "Das Sonnenlicht wurde wieder normal.[A]"},
    {"", "Es hat aufgehört zu regnen.[A]"},
    {"", "Das Sonnenlicht wurde wieder normal.[A]"},
    {"", "Windstille\x85[A]"},
    {"", "%s%s ist aufgetaut![A]"},

    {"", "%s%s ist gefroren.[A]"},
    {"", "%s%s ist paralysiert.[A]"},
    {"", "%s%s bleibt schlafen.[A]"},
    {"", "%s%s ist aufgewacht![A]"},
    {"", "%s%s bleibt unbeeindruckt.[A]"},

    // 290

    {"", "%s%s wich aus.[A]"},
    {"", "[COLR:15:15:00]Ein Volltreffer![A][CLEAR][COLR:00:00:00]"},
    {"", "[COLR:00:31:00]Das ist enorm effektiv\ngegen %s![A][CLEAR][COLR:00:00:00]"},
    {"", "[COLR:00:15:00]Das ist sehr effektiv\ngegen %s![A][CLEAR][COLR:00:00:00]"},
    {"", "[COLR:31:00:00]Hat die Attacke\n%s getroffen?[A][CLEAR][COLR:00:00:00]"},

    {"", "[COLR:31:00:00]Das ist nur enorm wenig\neffektiv gegen%s\x85[A][CLEAR][COLR:00:00:00]"},
    {"", "[COLR:15:00:00]Das ist nicht sehr effektiv\ngegen %s.[A][CLEAR][COLR:00:00:00]"},
    {"", "Es schlug fehl\x85[A]"},
    {"", "Die Verbrennung schadet\n%s%s.[A]"},
    {"", "Die Vergiftung schadet\n%s%s.[A]"},

    // 300

    {"", "%s%s wurde besiegt.[A]"},
    {"", "Das Rundenlimit dieses\nKampfes wurde erreicht.[A]"},
    {"", "Der Kampf endet in einem\nUnentschieden![A]"},
    {"", "[TRAINER] [TCLASS] gewinnt<85>[A]"},
    {"", "Du besiegst [TCLASS] [TRAINER]![A]"},

    {"", "Du gewinnst %d$.[A]"},
    {"", "Der Kampf endet.[A]"},
    {"", "[OPP%d] wurde von [TRAINER]\n([TCLASS]) auf die Bank geschickt.[A]"},
    {"", "Auf die Bank [OWN%d]![A]"},
    {"", "[OWN1] kann nicht angreifen<85>[A]"},

    // 310

    {"", "[OWN2] kann nicht angreifen<85>[A]"},
    {"", "(Wild)"},
    {"", "(Gegner)"},
    {"This tree looks like it can be\ncut down. ", "Ein kleiner Baum.\n"},
    {"This rock appears to be breakable.\n", "Ein kleiner Felsen.\n"},

    {"A huge whirlpool.\n", "Ein riesiger Strudel.\n"},
    {"The water is a deep blue...\n", "Das Wasser ist tiefblau...\n"},
    {"The sea is deep here.\n", "Das Wasser ist sehr tief.\n"},
    {"It's a big boulder.\n", "Ein großer Felsen.\n"},
    {"The wall is very rocky...\n", "Eine steile Felswand.\n"},

    // 320

    {"It's a large Waterfall.\n", "Ein großer Wasserfall.\n"},
    {"Something moved!\n", "Da hat sich etwas bewegt!\n"},
    {"Light is shining from the\nsurface. ", "Licht scheint von der\nOberfl\xe4""che. "},
    {"Select", "Ausw\xe4hlen"},
    {"Status", "Status"},

    {"Give Item", "Item geben"},
    {"Take Item", "Item nehmen"},
    {"Use Item", "Item nutzen"},
    {"Swap", "Austauschen"},
    {"Pok\xe9"
     "Dex",
     "Pok\xe9"
     "Dex"},

    // 330

    {"Cancel", "Zur\xfc"
               "ck"},
    {"Deselect", "Abw\xe4hlen"},
    {"Select %hhu Pok\xe9mon!", "W\xe4hle %hhu Pok\xe9mon!"},
    {"Select a Pok\xe9mon!", "W\xe4hle ein Pok\xe9mon!"},
    {"Gave %s\nto %s.", "%s an\n%s gegeben."},

    {"Choose these Pok\xe9mon?", "Diese Pok\xe9mon w\xe4hlen?"},
    {"Choose this Pok\xe9mon?", "Dieses Pok\xe9mon w\xe4hlen?"},
    {"Dex No.", "Dex Nr."},
    {"Name", "Name"},
    {"OT", "OT"},

    // 340

    {"ID No.", "Id Nr."},
    {"Exp.", "E.-Pkt."},
    {"Next", "N\xe4""chst."},
    {"Level", "Level"},
    {"Pok\xe9mon Info", "Pok\xe9mon-Info"},

    {"It looks like this Egg", "Was da wohl schl\xfcpfen"},
    {"will take a long time", "wird? Es dauert wohl"},
    {"to hatch.", "noch lange."},
    {"It appears to move", "Hat es sich gerade"},
    {"occasionally. It may", "bewegt? Da tut sich"},

    // 350

    {"be close to hatching.", "wohl bald was."},
    {"Sounds can be heard", "Jetzt macht es schon"},
    {"coming from inside!", "Ger\xe4usche! Bald ist"},
    {"It will hatch soon!", "es wohl soweit!"},
    {"A mysterious Pok\xe9mon", "Ein r\xe4tselhaftes"},

    {"Egg received", "Pok\xe9mon-Ei erhalten"},
    {"from", "vom"},
    {"at", "in/bei"},
    {"from", "von"},
    {"A highly mysterious", "Ein \xe4u\xdf""erst seltsames"},

    // 360

    {"Pok\xe9mon Egg that", "Pok\xe9mon-Ei, das"},
    {"somehow reached you", "irgendwie zu dir fand"},
    {"Item", "Item"},
    {"Ability", "F\xe4hig."},
    {"", "Wesen: "},

    {"nature.", ""},
    {"Egg received.", "Ei erhalten."},
    {"Had a fateful encunter.", "Schicksalhafte Begegnung."},
    {"Egg hatched.", "Ei geschl\xfcpft."},
    {"Egg apparently hatched.", "Ei offenbar geschl\xfcpft."},

    // 370

    {"Met at Lv. %hu.", "Erhalten mit Lv. %hu."},
    {"Apparently met at Lv. %hu.", "Offenbar erhalten mit Lv. %hu."},
    {"Had a fateful enc. at Lv. %hu.", "Schicksalhafte Begeg. mit Lv. %hu."},
    {"App. had a fatef. enc. at Lv. %hu.", "Off. schicks. Begeg. mit Lv. %hu."},
    {"Likes ", "Mag "},

    {"%s ", "%se "},
    {"Pok\xe9""blocks.", "Pok\xe9riegel."},
};
