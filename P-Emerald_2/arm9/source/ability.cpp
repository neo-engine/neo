/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : ability.cpp
    author      : Philip Wellnitz (RedArceus)
    description : The abilities

    Copyright (C) 2012 2014
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

ability abilities[ 180 ] = {
    ability( "Energieaufladung", "Sch\x81""tzt vor L\x84""rmattacken.", 1 ),
    ability( "Duftnote", "Macht Begegnungen mit wilden PKMN unwahrscheinlicher.", 8 ),
    ability( "Niesel", "Betritt das PKMN den Kampf, beginntes zu regnen.", 18 ),
    ability( "Temposchub", "Erh\x94""ht im Kampf kontinuierlich die Initiative.", 32 ),
    ability( "Kampfpanzer", "Sch\x81""tzt vor Volltreffern.", 1 ),
    ability( "Robustheit", "Sch\x81""tzt vor K.O.-Attacken.", 1 ),
    ability( "Feuchtigkeit", "Verhindert Selbstzerst\x94""rung von PKMN im Kampf.", 1 ),
    ability( "Flexibilit\x84""t", "Sch\x81""tzt vor Paralyse.", 1 ),
    ability( "Sandschleier", "Macht Fehlschlag gegn. Attacken im Sandsturm wahrscheinlicher.", 1 ),
    ability( "Statik", "Paralysiert bei Ber\x81""hrung.", 1 ),
    ability( "Volt-Absorber", "Wandelt Schaden von\x9e\x9f\xa0\xa1 Attacken in KP um.", 1 ),
    ability( "H2O-Absorber", "Wandelt Schaden von\x89\x8a\x8b\x8c Attacken in KP um.", 1 ),
    ability( "D\x94""sigkeit", "Sch\x81""tzt vor Anziehung.", 1 ),
    ability( "Wolke Sieben", "Sch\x81""tzt vor Wetter-Effekten.", 1 ),
    ability( "Facettenauge", "Macht Treffer eigener Attacken wahrscheinlicher.", 1 ),
    ability( "Insomnia", "Sch\x81""tzt vor Schlaf.", 1 ),
    ability( "Farbwechsel", "Passt eigenen Typ der letzten gegn. Attacke an.", 32 ),
    ability( "Immunit\x84""t", "Sch\x81""tzt vor Vergiftung.", 1 ),
    ability( "Feuerf\x84""nger", "Verst\x84""rkt eigene \x85\x86\x87\x88 Attacken nach Treffer gegn. \x85\x86\x87\x88 Atttacke.", 1 ),
    ability( "Puderabwehr", "Sch\x81""tzt vor Zusatzeffekten gegn. Attacken.", 1 ),
    ability( "Tempomacher", "Sch\x81""tzt vor Verwirrung.", 1 ),
    ability( "Saugnapf", "Sch\x81""tzt vor ungewolltem Austausch.", 1 ),
    ability( "Bedroher", "Betritt das PKMN den Kampf, wird der gegn. Angriff verringert.", 2 ),
    ability( "Wegsperre", "Verhindert Austausch oder Fluchtgegn. PKMN.", 16 ),
    ability( "Rauhaut", "Schadet gegn. PKMN""nach direktem Treffer.", 32 ),
    ability( "Wunderwache", "Nur sehr effektive""Attacken k\x94""nnen Schaden zuf\x81""gen.", 1 ),
    ability( "Schwebe", "Sch\x81""tzt vor \xd2\xd3\xd4\xd5 Attacken.", 1 ),
    ability( "Sporenwirt", "Verursacht evtl. Sp. Zustand bei gegn. PKMN nach direktem Treffer.", 32 ),
    ability( "Synchro", "Gibt Sp. Zustand an gegn. PKMN weiter.", 32 ),
    ability( "Neutraltorso", "Sch\x81""tzt vor Statusver\x84""nd.", 1 ),
    ability( "Innere Kraft", "Sch\x81""tzt vor Statusver\x84""nd.", 1 ),
    ability( "Blitzf\x84""nger", "Sch\x81""tzt vor \x9e\x9f\xa0\xa1 Attacken.", 1 ),
    ability( "Edelmut", "Macht Zusatzeffekte von eig. Attacken wahrscheinlicher.", 1 ),
    ability( "Wassertempo", "Erh\x94""ht eigene Initiative bei Regen.", 16 ),
    ability( "Chlorophyll", "Erh\x94""ht eigene Initiative bei starkem Sonnenschein.", 16 ),
    ability( "Erleuchtung", "Macht Begegnungen mit wilden PKMN wahrscheinlicher.", 8 ),
    ability( "F\x84""hrte", "Kopiert die F\x84""higkeit eines gegn. PKMN.", 2 ),
    ability( "Kraftkollos", "Erh\x94""ht eigenen Angriff.", 8 ),
    ability( "Giftdorn", "Vergiftet bei Ber\x81""hrung.", 32 ),
    ability( "Konzentrator", "Sch\x81""tzt vor Zur\x81""ckschrecken.", 1 ),
    ability( "Magmapanzer", "Sch\x81""tzt vor Einfrieren. Beschleunigt Brut-vorg\x84""nge.", 9 ),
    ability( "Aquah\x81""lle", "Sch\x81""tzt vor Verbrennungen.", 1 ),
    ability( "Magnetfalle", "Verhindert Flucht oder Austausch von gegn. \xb2\xb3\xb4\xb5 PKMN.", 16 ),
    ability( "L\x84""rmschutz", "Sch\x81""tzt vor L\x84""rmattacken.", 1 ),
    ability( "Regengenuss", "Regeneriert KP bei Regen.", 16 ),
    ability( "Sandsturm", "Betritt das PKMN den Kampf, so tritt ein Sandsturm auf.", 16 ),
    ability( "Erzwinger", "Gegn. PKMN verbrauchen 2AP pro erfolgreicher Attacke.", 32 ),
    ability( "Speckschicht", "Reduziert Schaden von \x8f\x90\x91\x92 und \x85\x86\x87\x88 Attacken.", 1 ),
    ability( "Fr\x81""hwecker", "Schnelles Aufwachen.", 16 ),
    ability( "Flammk\x94""rper", "Verbrennt bei Ber\x81""hrung. Beschleunigt Brutvorg\x84""nge", 9 ),
    ability( "Angsthase", "Garantiert das Entkommen von wilden PKMN.", 16 ),
    ability( "Adlerauge", "Sch\x81""tzt vor Absinken der Genauigkeit.", 1 ),
    ability( "Scherenmacht", "Sch\x81""tzt vor Absinken des Angriffs.", 1 ),
    ability( "Mitnahme", "Sammelt Gegenst\x84""nde.", 8 ),
    ability( "Schnarchnase", "PKMN kann nicht in zwei aufeinanderfolgenden Z\x81""gen angreifen.", 16 ),
    ability( "\x9A""bereifer", "Erh\x94""ht Angriff, senkt Genauigkeit.", 8 ),
    ability( "Charmebolzen", "Bet\x94""rt bei Kontakt.", 32 ),
    ability( "Plus", "Erh\x94""ht Sp. Angriff sofern ein PKMN mit Minus am Kampf teilnimmt.", 1 ),
    ability( "Minus", "Erh\x94""ht Sp. Angriff sofern ein PKMN mit Plus am Kampf teilnimmt.", 1 ),
    ability( "Prognose", "Passt \x84""u\x9D""eres Erscheinungsbild dem Wetter an.", 32 ),
    ability( "Wertehalter", "Sch\x81""tzt vor Item-Diebstahl.", 1 ),
    ability( "Expidermis", "PKMN heilt evtl. eigene Sp. Zust\x84""nde.", 16 ),
    ability( "Adrenalin", "Erh\x94""ht Angriff bei Statusver\x84""nderungen.", 16 ),
    ability( "Notschutz", "Erh\x94""ht Verteidigung bei Statusver\x84""nderungen.", 16 ),
    ability( "Kloakenso\x9D""e", "Schadet Anwendern von Saugattacken.", 32 ),
    ability( "Notd\x81""nger", "Erh\x94""ht Schaden durch PFLANZEN-Attacken in einer Notlage.", 16 ),
    ability( "Gro\x9D""brand", "Erh\x94""ht Schaden durch FEUER-Attacken in einer Notlage.", 16 ),
    ability( "Sturzbach", "Erh\x94""ht Schaden durch WASSER-Attacken in einer Notlage.", 16 ),
    ability( "Hexaplaga", "Erh\x94""ht Schaden durch \xb6\xb7\xb8\xb9""-Attacken in einer Notlage.", 16 ),
    ability( "Steinhaupt", "Sch\x81""tzt vor R\x81""cksto\x9D""schaden.", 1 ),
    ability( "D\x81""rre", "Betritt das PKMN den Kampf, intensiviert sich die Sonneneinstrahlung.", 16 ),
    ability( "Ausweglos", "Verhindert Austausch oder Flucht gegn. PKMN.", 16 ),
    ability( "Munterkeit", "Sch\x81""tzt vor Einschlafen.", 1 ),
    ability( "Pulverrauch", "Sch\x81""tzt vor Absenken von Statuswerten.", 1 ),
    ability( "Mentalkraft", "Erh\x94""ht Schaden durch physische Attacken.", 1 ),
    ability( "Panzerhaut", "Sch\x81""tzt vor Volltreffern.", 1 ),
    ability( "Klimaschutz", "Neutralisiert Wettereffekte.", 16 ),
    //GEN 4
    ability( "Fu\x9D""angel", "Ist das PKMN verwirrt, sind Treffer gegn. PKMN unwahrscheinlicher.", 1 ),
    ability( "Starthilfe", "Erh\x94""ht eigene Initiative, wenn von ELEKTRO-Attacke getroffen.", 32 ),
    ability( "Rivalit\x84""t", "Eigener Angriff steigt bei gleichem Geschlecht des Gegners.", 16 ),
    ability( "Felsenfest", "Erh\x94""ht eigene Initiative, wenn das PKMN zur\x81""ckschreckt.", 32 ),
    ability( "Schneemantel", "Macht Fehlschlag gegn. Attacken bei Hagel wahrscheinlicher.", 1 ),
    ability( "V\x94""llerei", "Das PKMN benutzt Beeren fr\x81""hzeitig.", 32 ),
    ability( "Kurzschluss", "Maximiert Angriff nach erlittenem Volltreffer.", 32 ),
    ability( "Entlastung", "Erh\x94""ht eigene Initiative nach Benutzung eines getragenen Items.", 32 ),
    ability( "Hitzeschutz", "Schw\x84""cht gegn. FEUER-Attacken.", 1 ),
    ability( "Wankelmut", "Verdoppelt den Effekt von Statusver\x84""nderungen.", 1 ),
    ability( "Trockenheit", "Hei\x9D""es Wetter reduziert KP, Wasser erh\x94""ht KP.", 1 ),
    ability( "Download", "Passt Statuswerte entsprechend denen gegn. PKMN an.", 16 ),
    ability( "Eisenfaust", "Verst\x84""rkt schlag-basierte Attacken.", 1 ),
    ability( "Aufheber", "Regeneriert KP bei Vergiftung.", 1 ),
    ability( "Anpassung", "Steigert Attacken des selben Typs.", 1 ),
    ability( "Wertelink", "Mehrfach ausgef\x81""hrte Attacken treffen \x81""ber die max. Anz. an Z\x81""gen.", 1 ),
    ability( "Hydration", "Heilt Sp. Zust\x84""nde bei Regen.", 16 ),
    ability( "Solarkraft", "Erh\x94""ht Sp. Angriff und senkt KP bei Sonne.", 1 ),
    ability( "Rasanz", "Erh\x94""ht Initiative wenn das PKMN von einem Sp. Zustand betroffen ist.", 1 ),
    ability( "Regulierung", "Alle Attacken des PKMN werden zu NORMAL-Attacken.", 1 ),
    ability( "Supersch\x81""tze", "Steigert Attacken nach Volltreffern.", 1 ),
    ability( "Magieschild", "Nur physische Attacken schaden dem PKMN.", 1 ),
    ability( "Schildlos", "Alle Attacken treffen garantiert.", 1 ),
    ability( "Zeitspiel", "Das PKMN agiert stets zuletzt.", 16 ),
    ability( "Techniker", "Steigert Effektivit\x84""t von schw\x84""cheren Attacken.", 1 ),
    ability( "Floraschild", "Heilt Sp. Zust\x84""nde bei Regen.", 16 ),
    ability( "Tollpatsch", "Das PKMN kann keine Items verwenden.", 16 ),
    ability( "\x9A""berbr\x81""ckung", "Greift unabh\x84""ngig anderer F\x84""higkeiten an.", 1 ),
    ability( "Gl\x81""ckspilz", "Macht Volltreffer wahrscheinlicher.", 1 ),
    ability( "Finalschlag", "Schadet dem PKMN, das finale Attacke landet.", 32 ),
    ability( "Vorahnung", "Ahnt gef\x84""hrliche Attacke eines gegn. PKMN.", 2 ),
    ability( "Vorwarnung", "Ahnt st\x84""rkste Attacke eines gegn. PKMN.", 2 ),
    ability( "Unkenntnis", "Ignoriert gegn. Statusver\x84""nderungen.", 1 ),
    ability( "Aufwertung", "Wertet \"weniger effektive\" Attacken auf.", 1 ),
    ability( "Filter", "Senkt St\x84""rke sehr effektiver Attacken.", 1 ),
    ability( "Saumselig", "Halbiert Angriff und Initiative zu Kampfbeginn.", 16 ),
    ability( "Rauflust", "Erm\x94""glicht gegnerischen \x95\x96\x97\x98""-PKMN zu schaden.", 1 ),
    ability( "Sturmsog", "Zieht offensive \xa6\xa7\xa8\xa9""-Attacken auf sich.", 1 ),
    ability( "Eishaut", "Regeneriert KP bei Hagel.", 16 ),
    ability( "Felskern", "Senkt St\x84""rke sehr effektiver Attacken.", 1 ),
    ability( "Hagelalarm", "Betritt das PKMN den Kampf, beginnt es zu hageln.", 16 ),
    ability( "Honigmaul", "Das PKMN sammelt von Zeit zu Zeit Honig.", 8 ),
    ability( "Schnüffler", "Erkennt Items gegnerischer PKMN.", 16 ),
    ability( "Achtlos", "Verst\x84""rkt Attacken mit R\x81""cksto\x9D"".", 1 ),
    ability( "Variabilit\x84""t", "Passt Typ der getragenen Tafel an.", 8 ),
    ability( "Pflanzengabe", "Steigert Effektivit\x84""t von Team-PKMN bei Sonne.", 1 ),
    ability( "Alptraum", "Schadet schlafenden gegnerischen PKMN.", 1 ),
    //Gen 5
    ability( "Langfinger", "Stiehlt Item bei Kontakt.", 1 ),
    ability( "Rohe Gewalt", "Wandelt Zusatzeffekte von Attacken in zus\x84""tzliche St\x84""rke.", 1 ),
    ability( "Umkehrung", "Kehrt Statusver\x84""nderungen um.", 1 ),
    ability( "Anspannung", "Verhindert gegn. Einsatz von Beeren.", 1 ),
    ability( "Rohe Gewalt", "Wandelt Zusatzeffekte von Attacken in zus\x84""tzliche St\x84""rke.", 1 ),
    ability( "Siegeswille", "Erh\x94""ht Angriff stark, wenn Statuswerte absinken.", 1 ),
    ability( "Schw\x84""chling", "Halbiert Angr. und SpAngr. bei halben KP.", 1 ),
    ability( "Tastfluch", "Blockiert letzte gegn. Attacke bei Kontakt.", 1 ),
    ability( "Heilherz", "Heilt Statusprobleme von PKMN im Team.", 1 ),
    ability( "Freundeshut", "Weniger Schaden bei PKMN im Team.", 1 ),
    ability( "Bruchr\x81""stung", "Erh\x94""ht Initiative, senkt Verteidigung nach Treffer.", 1 ),
    ability( "Schwermetall", "Verdoppelt Gewicht.", 1 ),
    ability( "Leichtmetall", "Halbiert Gewicht.", 1 ),
    ability( "Multischuppe", "Halber Schaden bei vollen KP.", 1 ),
    ability( "Giftwahn", "Erh\x94""ht Angriff, wenn vergiftet.", 1 ),
    ability( "Hitzewahn", "Erh\x94""ht Sp.Angr., wenn verbrannt.", 1 ),
    ability( "Reiche Ernte", "Regeneriert benutzte Beeren.", 1 ),
    ability( "Telepathie", "Immun gegen Attaken von PKMN im Team.", 1 ),
    ability( "Gef\x81""hlswippe", "Erh\x94""ht und senkt zuf\x84""llige Statuswerte pro Runde.", 1 ),
    ability( "Wetterfest", "Immun gegen Wetter.", 1 ),
    ability( "Giftgriff", "Vergiftet bei Kontakt.", 1 ),
    ability( "Belebekraft", "Heilt sich bei Austausch.", 1 ),
    ability( "Brustbieter", "Macht Volltreffer wahrscheinlicher.", 1 ),
    ability( "Sandscharrer", "Erh\x94""ht Initiative bei Sandsturm.", 1 ),
    ability( "Wunderhaut", "Gegn. Status-Attacken fehlen manchmal.", 1 ),
    ability( "Analyse", "Erh\x94""ht St\x84"", wenn zuletzt agierend.", 1 ),
    ability( "Illusion", "Nimmt Gestalt des letzten PKMN im Team an.", 2 ),
    ability( "Doppelg\x84""nger", "Nimmt Gestalt des Gegners an.", 2 ),
    ability( "Schwebedurch", "Ignoriert Reflektor, Lichtschild und Bodyguard.", 1 ),
    ability( "Mumie", "Verteilt Mumie bei Kontakt.", 1 ),
    ability( "Hochmut", "Erh\x94""ht Angriff, wenn gegn. PKMN besiegt.", 1 ),
    ability( "Redlichkeit", "Erh\x94""ht Angriff, wenn von gegn. \xbe\xbf\xc0\xc1 Attacke getroffen.", 1 ),
    ability( "Hasenfu\x9D", "Erh\x94""ht Init., wenn von gegn. \x95\x96\x97\x98"", \xb6\xb7\xb8\xb9 oder \xbe\xbf\xc0\xc1 Attacke getroffen.", 1 ),
    ability( "Magiespiegel", "Kann Statusver\x84""nderungen zur\x91""ckwerfen.", 1 ),
    ability( "Vegetarier", "Immun gegen \xaa\xab\xac\xad""-Attacken.", 1 ),
    ability( "Strolch", "Erh\x94""ht Priorit\x84 von Status-Attacken.", 1 ),
    ability( "Sandgewalt", "St\x84""rkt \xd2\xd3\xd4\xd5"", \xb6\xb7\xb8\xb9 und \x9e\x9f\xa0\xa1 Attacken w\x84""hrend eines Sandsturms.", 1 ),
    ability( "Eisenstachel", "Schadet bei Kontakt.", 1 ),
    ability( "Zen-Modus", "Wandelt Form bei niedriegen KP.", 1 ),
    ability( "Triumphstern", "Erh\x94""ht Genauigkeit von Attacken von PKMN im Team.", 1 ),
    ability( "Turbobrand", "Greift unabh\x84""ngig anderer F\x84""higkeiten an.", 1 ),
    ability( "Teravolt", "Greift unabh\x84""ngig anderer F\x84""higkeiten an.", 1 ),



    //Gen *
    ability( "Klon", "Das PKMN kann sich weder entwickeln, noch kann es TM oder VM erlernen.", 1 ),
    ability( "Unantastbar", "Immun gegen alle Attacken, Wetter und Items.", 1 ),
    ability( "Fischauge", "Erh\x94""ht Genauigkeit nach direktem Treffer.", 1 ),
    ability( "Gleichheit", "Erh\x94""ht Effektivit\x84""t eigener Attacken, bei Gegnern selbem Typs.", 1 ),
    ability( "Schillerschuppe", "Macht Begegnungen mit schillernden PKMN wahrscheinlicher.", 8 ),
    ability( "Nebelwand", "Betritt das PKMN den Kampf, zieht starker Nebel auf.", 16 ),
    ability( "K\x94""nigsheiler", "Heilt alle PKMN im Team am Ende einer Runde ein wenig.", 1 ),
    ability( "Einsch\x91""chterer", "Betritt das PKMN den Kampf, tauscht der Gegner sein PKMN aus.", 1 ),
    ability( "Effizienz", "Verdoppelt Schw\x84""en und Resistenzen.", 1 ),
    ability( "\x9A""berfluss", "Verbraucht keine AP.", 1 )
};

/*	Ability:Cloned
Cloned PKMN can neither evolve nor use TMs
They've got a (kind of) shiny sprite and a black star instead of a red one
They've got doubled base stats and, of course, cannot be traded.
*/