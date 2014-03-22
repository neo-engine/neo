/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : ability.cpp
    author      : Philip Wellnitz (RedArceus)
    description : The abilities

    Copyright (C) 2012 - 2014
        Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        is required.

    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
        distribution.
*/

#include "ability.h"

ablty abilities[180]={
    ablty("Energieaufladung"   ,"Sch\x81""tzt vor L\x84""rmattacken.",1),
    ablty("Duftnote"           ,"Macht Begegnungen mit wilden\nPKMN unwahrscheinlicher.",8),
    ablty("Niesel"             ,"Betritt das PKMN den Kampf,\nbeginntes zu regnen.",18),
    ablty("Temposchub"         ,"Erh\x94""ht im Kampf kontinuierlich\ndie Initiative.",32),
    ablty("Kampfpanzer"        ,"Sch\x81""tzt vor Volltreffern.",1),
    ablty("Robustheit"         ,"Sch\x81""tzt vor K.O.-Attacken.",1),
    ablty("Feuchtigkeit"       ,"Verhindert Selbstzerst\x94""rung\nvon PKMN im Kampf.",1),
    ablty("Flexibilit\x84""t" ,"Sch\x81""tzt vor Paralyse.",1),
    ablty("Sandschleier"       ,"Macht Fehlschlag gegn.\nAttacken im Sandsturm wahr-\nscheinlicher.",1),
    ablty("Statik"             ,"Paralysiert bei Ber\x81""hrung.",1),
    ablty("Volt-Absorber"      ,"Wandelt Schaden von\x9e\x9f\xa0\xa1\nAttacken in KP um.",1),
    ablty("H2O-Absorber"       ,"Wandelt Schaden\nvon\x89\x8a\x8b\x8c\nAttacken in KP um.",1),
    ablty("D\x94""sigkeit"    ,"Sch\x81""tzt vor Anziehung.",1),
    ablty("Wolke Sieben"       ,"Sch\x81""tzt vor Wetter-Effekten.",1),
    ablty("Facettenauge"       ,"Macht Treffer eigener Attacken\nwahrscheinlicher.",1),
    ablty("Insomnia"           ,"Sch\x81""tzt vor Schlaf.",1),
    ablty("Farbwechsel"        ,"Passt eigenen Typ der letzten\ngegn. Attacke an.",32),
    ablty("Immunit\x84""t"    ,"Sch\x81""tzt vor Vergiftung.",1),
    ablty("Feuerf\x84""nger"  ,"Verst\x84""rkt eigene \x85\x86\x87\x88 Attacken nach Treffer gegn.\n\x85\x86\x87\x88 Atttacke.",1),
    ablty("Puderabwehr"        ,"Sch\x81""tzt vor Zusatzeffekten\ngegn. Attacken.",1),
    ablty("Tempomacher"        ,"Sch\x81""tzt vor Verwirrung.",1),
    ablty("Saugnapf"           ,"Sch\x81""tzt vor ungewolltem\nAustausch.",1),
    ablty("Bedroher"           ,"Betritt das PKMN den Kampf,\nwird der gegn. Angriff\nverringert.",2),
    ablty("Wegsperre"          ,"Verhindert Austausch oder\nFluchtgegn. PKMN.",16),
    ablty("Rauhaut"           ,"Schadet gegn. PKMN""nach direktem\nTreffer.",32),
    ablty("Wunderwache"       ,"Nur sehr effektive""Attacken k\x94""nnen\nSchaden zuf\x81""gen.",1),
    ablty("Schwebe"           ,"Sch\x81""tzt vor\n\xd2\xd3\xd4\xd5 Attacken.",1),
    ablty("Sporenwirt"        ,"Verursacht evtl.\nSp. Zustand bei\ngegn. PKMN nach\ndirektem Treffer.",32),
    ablty("Synchro"           ,"Gibt Sp. Zustand\nan gegn. PKMN\nweiter.",32),
    ablty("Neutraltorso"      ,"Sch\x81""tzt vor\nStatusver\x84""nd.",1),
    ablty("Innere Kraft"      ,"Sch\x81""tzt vor\nStatusver\x84""nd.",1),
    ablty("Blitzf\x84""nger" ,"Sch\x81""tzt vor\n\x9e\x9f\xa0\xa1 Attacken.",1),
    ablty("Edelmut"           ,"Macht Zusatz-\neffekte von eig.\nAttacken wahr-\nscheinlicher.",1),
    ablty("Wassertempo"       ,"Erh\x94""ht eigene\nInitiative bei\nRegen.",16),
    ablty("Chlorophyll"       ,"Erh\x94""ht eigene\nInitiative bei\nstarkem Sonnen-\nschein.",16),
    ablty("Erleuchtung"       ,"Macht Begegnungen\nmit wilden PKMN\nwahrscheinlicher.",8),
    ablty("F\x84""hrte"      ,"Kopiert die F\x84""hig-\nkeit eines gegn.\nPKMN.",2),
    ablty("Kraftkollos"       ,"Erh\x94""ht eigenen\nAngriff.",8),
    ablty("Giftdorn"          ,"Vergiftet bei\nBer\x81""hrung.",32),
    ablty("Konzentrator"      ,"Sch\x81""tzt vor\nZur\x81""ckschrecken.",1),
    ablty("Magmapanzer"       ,"Sch\x81""tzt vor\nEinfrieren.\nBeschleunigt Brut-vorg\x84""nge.",9),
    ablty("Aquah\x81""lle"   ,"Sch\x81""tzt vor\nVerbrennungen.",1),
    ablty("Magnetfalle"       ,"Verhindert Flucht\noder Austausch von""gegn. \xb2\xb3\xb4\xb5 PKMN.",16),
    ablty("L\x84""rmschutz","Sch\x81""tzt vor L\x84""rmattacken.",1),
    ablty("Regengenuss","Regeneriert KP bei Regen.",16),
    ablty("Sandsturm","Betritt das PKMN den Kampf, so tritt ein Sandsturm auf.",16),
    ablty("Erzwinger","Gegn. PKMN verbrauchen 2AP pro erfolgreicher Attacke.",32),
    ablty("Speckschicht","Reduziert Schaden von \x8f\x90\x91\x92 und \x85\x86\x87\x88 Attacken.",1),
    ablty("Fr\x81""hwecker","Schnelles Aufwachen.",16),
    ablty("Flammk\x94""rper","Verbrennt bei Ber\x81""hrung. Beschleunigt Brutvorg\x84""nge",9),
    ablty("Angsthase","Garantiert das Entkommen von wilden PKMN.",16),
    ablty("Adlerauge","Sch\x81""tzt vor Absinken der Genauigkeit.",1),
    ablty("Scherenmacht","Sch\x81""tzt vor Absinken des Angriffs.",1),
    ablty("Mitnahme","Sammelt Gegenst\x84""nde.",8),
    ablty("Schnarchnase","PKMN kann nicht in zwei aufeinanderfolgenden Z\x81""gen angreifen.",16),
    ablty("\x9A""bereifer","Erh\x94""ht Angriff, senkt Genauigkeit.",8),
    ablty("Charmebolzen","Bet\x94""rt bei Kontakt.",32),
    ablty("Plus","Erh\x94""ht Sp. Angriff sofern ein PKMN mit Minus am Kampf teilnimmt.",1),
    ablty("Minus","Erh\x94""ht Sp. Angriff sofern ein PKMN mit Plus am Kampf teilnimmt.",1),
    ablty("Prognose","Passt \x84""u\x9D""eres Erscheinungsbild dem Wetter an.",32),
    ablty("Wertehalter","Sch\x81""tzt vor Item-Diebstahl.",1),
    ablty("Expidermis","PKMN heilt evtl. eigene Sp. Zust\x84""nde.",16),
    ablty("Adrenalin","Erh\x94""ht Angriff bei Statusver\x84""nderungen.",16),
    ablty("Notschutz","Erh\x94""ht Verteidigung bei Statusver\x84""nderungen.",16),
    ablty("Kloakenso\x9D""e","Schadet Anwendern von Saugattacken.",32),
    ablty("Notd\x81""nger","Erh\x94""ht Schaden durch PFLANZEN-Attacken in einer Notlage.",16),
    ablty("Gro\x9D""brand","Erh\x94""ht Schaden durch FEUER-Attacken in einer Notlage.",16),
    ablty("Sturzbach","Erh\x94""ht Schaden durch WASSER-Attacken in einer Notlage.",16),
    ablty("Hexaplaga","Erh\x94""ht Schaden durch \xb6\xb7\xb8\xb9""-Attacken in einer Notlage.",16),
    ablty("Steinhaupt","Sch\x81""tzt vor R\x81""cksto\x9D""schaden.",1),
    ablty("D\x81""rre","Betritt das PKMN den Kampf, intensiviert sich die Sonneneinstrahlung.",16),
    ablty("Ausweglos","Verhindert Austausch oder Flucht gegn. PKMN.",16),
    ablty("Munterkeit","Sch\x81""tzt vor Einschlafen.",1),
    ablty("Pulverrauch","Sch\x81""tzt vor Absenken von Statuswerten.",1),
    ablty("Mentalkraft","Erh\x94""ht Schaden durch physische Attacken.",1),
    ablty("Panzerhaut","Sch\x81""tzt vor Volltreffern.",1),
    ablty("Klimaschutz","Neutralisiert Wettereffekte.",16),
    //GEN 4
    ablty("Fu\x9D""angel","Ist das PKMN verwirrt, sind Treffer gegn. PKMN unwahrscheinlicher.",1),
    ablty("Starthilfe","Erh\x94""ht eigene Initiative, wenn von ELEKTRO-Attacke getroffen.",32),
    ablty("Rivalit\x84""t","Eigener Angriff steigt bei gleichem Geschlecht des Gegners.",16),
    ablty("Felsenfest","Erh\x94""ht eigene Initiative, wenn das PKMN zur\x81""ckschreckt.",32),
    ablty("Schneemantel","Macht Fehlschlag gegn. Attacken bei Hagel wahrscheinlicher.",1),
    ablty("V\x94""llerei","Das PKMN benutzt Beeren fr\x81""hzeitig.",32),
    ablty("Kurzschluss","Maximiert Angriff nach erlittenem Volltreffer.",32),
    ablty("Entlastung","Erh\x94""ht eigene Initiative nach Benutzung eines getragenen Items.",32),
    ablty("Hitzeschutz","Schw\x84""cht gegn. FEUER-Attacken.",1),
    ablty("Wankelmut","Verdoppelt den Effekt von Statusver\x84""nderungen.",1),
    ablty("Trockenheit","Hei\x9D""es Wetter reduziert KP, Wasser erh\x94""ht KP.",1),
    ablty("Download","Passt Statuswerte entsprechend denen gegn. PKMN an.",16),
    ablty("Eisenfaust","Verst\x84""rkt schlag-basierte Attacken.",1),
    ablty("Aufheber","Regeneriert KP bei Vergiftung.",1),
    ablty("Anpassung","Steigert Attacken des selben Typs.",1),
    ablty("Wertelink","Mehrfach ausgef\x81""hrte Attacken treffen \x81""ber die max. Anz. an Z\x81""gen.",1),
    ablty("Hydration","Heilt Sp. Zust\x84""nde bei Regen.",16),
    ablty("Solarkraft","Erh\x94""ht Sp. Angriff und senkt KP bei Sonne.",1),
    ablty("Rasanz","Erh\x94""ht Initiative wenn das PKMN von einem Sp. Zustand betroffen ist.",1),
    ablty("Regulierung","Alle Attacken des PKMN werden zu NORMAL-Attacken.",1),
    ablty("Supersch\x81""tze","Steigert Attacken nach Volltreffern.",1),
    ablty("Magieschild","Nur physische Attacken schaden dem PKMN.",1),
    ablty("Schildlos","Alle Attacken treffen garantiert.",1),
    ablty("Zeitspiel","Das PKMN agiert stets zuletzt.",16),
    ablty("Techniker","Steigert Effektivit\x84""t von schw\x84""cheren Attacken.",1),
    ablty("Floraschild","Heilt Sp. Zust\x84""nde bei Regen.",16),
    ablty("Tollpatsch","Das PKMN kann keine Items verwenden.",16),
    ablty("\x9A""berbr\x81""ckung","Greift unabh\x84""ngig anderer F\x84""higkeiten an.",1),
    ablty("Gl\x81""ckspilz","Macht Volltreffer wahrscheinlicher.",1),
    ablty("Finalschlag","Schadet dem PKMN, das finale Attacke landet.",32),
    ablty("Vorahnung","Ahnt gef\x84""hrliche Attacke eines gegn. PKMN.",2),
    ablty("Vorwarnung","Ahnt st\x84""rkste Attacke eines gegn. PKMN.",2),
    ablty("Unkenntnis","Ignoriert gegn. Statusver\x84""nderungen.",1),
    ablty("Aufwertung","Wertet \"weniger effektive\" Attacken auf.",1),
    ablty("Filter","Senkt St\x84""rke sehr effektiver Attacken.",1),
    ablty("Saumselig","Halbiert Angriff und Initiative zu Kampfbeginn.",16),
    ablty("Rauflust","Erm\x94""glicht gegnerischen \x95\x96\x97\x98""-PKMN zu schaden.",1),
    ablty("Sturmsog","Zieht offensive \xa6\xa7\xa8\xa9""-Attacken auf sich.",1),
    ablty("Eishaut","Regeneriert KP bei Hagel.",16),
    ablty("Felskern","Senkt St\x84""rke sehr effektiver Attacken.",1),
    ablty("Hagelalarm","Betritt das PKMN den Kampf, beginnt es zu hageln.",16),
    ablty("Honigmaul","Das PKMN sammelt von Zeit zu Zeit Honig.",8),
    ablty("Schnüffler","Erkennt Items gegnerischer PKMN.",16),
    ablty("Achtlos","Verst\x84""rkt Attacken mit R\x81""cksto\x9D"".",1),
    ablty("Variabilit\x84""t","Passt Typ der getragenen Tafel an.",8),
    ablty("Pflanzengabe","Steigert Effektivit\x84""t von Team-PKMN bei Sonne.",1),
    ablty("Alptraum","Schadet schlafenden gegnerischen PKMN.",1),
    //Gen 5
    ablty("Langfinger","Stiehlt Item bei Kontakt.",1),
    ablty("Rohe Gewalt","Wandelt Zusatzeffekte von Attacken in zus\x84""tzliche St\x84""rke.",1),
    ablty("Umkehrung","Kehrt Statusver\x84""nderungen um.",1),
    ablty("Anspannung","Verhindert gegn. Einsatz von Beeren.",1),
    ablty("Rohe Gewalt","Wandelt Zusatzeffekte von Attacken in zus\x84""tzliche St\x84""rke.",1),
    ablty("Siegeswille","Erh\x94""ht Angriff stark, wenn Statuswerte absinken.",1),
    ablty("Schw\x84""chling","Halbiert Angr. und SpAngr. bei halben KP.",1),
    ablty("Tastfluch","Blockiert letzte gegn. Attacke bei Kontakt.",1),
    ablty("Heilherz","Heilt Statusprobleme von PKMN im Team.",1),
    ablty("Freundeshut","Weniger Schaden bei PKMN im Team.",1),
    ablty("Bruchr\x81""stung","Erh\x94""ht Initiative, senkt Verteidigung nach Treffer.",1),
    ablty("Schwermetall","Verdoppelt Gewicht.",1),
    ablty("Leichtmetall","Halbiert Gewicht.",1),
    ablty("Multischuppe","Halber Schaden bei vollen KP.",1),
    ablty("Giftwahn","Erh\x94""ht Angriff, wenn vergiftet.",1),
    ablty("Hitzewahn","Erh\x94""ht Sp.Angr., wenn verbrannt.",1),
    ablty("Reiche Ernte","Regeneriert benutzte Beeren.",1),
    ablty("Telepathie","Immun gegen Attaken von PKMN im Team.",1),
    ablty("Gef\x81""hlswippe","Erh\x94""ht und senkt zuf\x84""llige Statuswerte pro Runde.",1),
    ablty("Wetterfest","Immun gegen Wetter.",1),
    ablty("Giftgriff","Vergiftet bei Kontakt.",1),
    ablty("Belebekraft","Heilt sich bei Austausch.",1),
    ablty("Brustbieter","Macht Volltreffer wahrscheinlicher.",1),
    ablty("Sandscharrer","Erh\x94""ht Initiative bei Sandsturm.",1),
    ablty("Wunderhaut","Gegn. Status-Attacken fehlen manchmal.",1),
    ablty("Analyse","Erh\x94""ht St\x84"", wenn zuletzt agierend.",1),
    ablty("Illusion","Nimmt Gestalt des letzten PKMN im Team an.",2),
    ablty("Doppelg\x84""nger","Nimmt Gestalt des Gegners an.",2),
    ablty("Schwebedurch","Ignoriert Reflektor, Lichtschild und Bodyguard.",1),
    ablty("Mumie","Verteilt Mumie bei Kontakt.",1),
    ablty("Hochmut","Erh\x94""ht Angriff, wenn gegn. PKMN besiegt.",1),
    ablty("Redlichkeit","Erh\x94""ht Angriff, wenn von gegn. \xbe\xbf\xc0\xc1 Attacke getroffen.",1),
    ablty("Hasenfu\x9D","Erh\x94""ht Init., wenn von gegn. \x95\x96\x97\x98"", \xb6\xb7\xb8\xb9 oder \xbe\xbf\xc0\xc1 Attacke getroffen.",1),
    ablty("Magiespiegel","Kann Statusver\x84""nderungen zur\x91""ckwerfen.",1),
    ablty("Vegetarier","Immun gegen \xaa\xab\xac\xad""-Attacken.",1),
    ablty("Strolch","Erh\x94""ht Priorit\x84 von Status-Attacken.",1),
    ablty("Sandgewalt","St\x84""rkt \xd2\xd3\xd4\xd5"", \xb6\xb7\xb8\xb9 und \x9e\x9f\xa0\xa1 Attacken w\x84""hrend eines Sandsturms.",1),
    ablty("Eisenstachel","Schadet bei Kontakt.",1),
    ablty("Zen-Modus","Wandelt Form bei niedriegen KP.",1),
    ablty("Triumphstern","Erh\x94""ht Genauigkeit von Attacken von PKMN im Team.",1),
    ablty("Turbobrand","Greift unabh\x84""ngig anderer F\x84""higkeiten an.",1),
    ablty("Teravolt","Greift unabh\x84""ngig anderer F\x84""higkeiten an.",1),

    //Gen *
    ablty("Klon","Das PKMN kann sich weder entwickeln, noch kann es TM oder VM erlernen.",1),
    ablty("Unantastbar","Immun gegen alle Attacken, Wetter und Items.",1),
    ablty("Fischauge","Erh\x94""ht Genauigkeit nach direktem Treffer.",1),
    ablty("Gleichheit","Erh\x94""ht Effektivit\x84""t eigener Attacken, bei Gegnern selbem Typs.",1),
    ablty("Schillerschuppe","Macht Begegnungen mit schillernden PKMN wahrscheinlicher.",8),
    ablty("Nebelwand","Betritt das PKMN den Kampf, zieht starker Nebel auf.",16),
    ablty("K\x94""nigsheiler","Heilt alle PKMN im Team am Ende einer Runde ein wenig.",1),
    ablty("Einsch\x91""chterer","Betritt das PKMN den Kampf, tauscht der Gegner sein PKMN aus.",1),
    ablty("Effizienz","Verdoppelt Schw\x84""en und Resistenzen.",1),
    ablty("\x9A""berfluss","Verbraucht keine AP.",1)
};

/*	Ability:Cloned
Cloned PKMN can neither evolve nor use TMs
They've got a (kind of) shiny sprite and a black star instead of a red one
They've got doubled base stats and cannot, of course, be traded.
*/