/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : item.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Main item engine.

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


#include <fstream>
#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "item.h"
#include "berry.h"
#include "buffer.h"

const char ITEM_PATH[ ] = "nitro:/ITEMS/";

std::string readString( FILE* p_file, bool p_new ) {
    std::string ret = "";
    char ac;

    while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );

    if( ac == '*' ) {
        ret += '\0';
        return ret;
    } else ret += ac;

    while( ( ac = fgetc( p_file ) ) != '*' ) {
        if( !p_new ) {
            if( ac == 'ä' )
                ret += '\x84';
            else if( ac == 'Ä' )
                ret += '\x8E';
            else if( ac == 'ü' )
                ret += '\x81';
            else if( ac == 'Ü' )
                ret += '\x9A';
            else if( ac == 'ö' )
                ret += '\x94';
            else if( ac == 'Ö' )
                ret += '\x99';
            else if( ac == 'ß' )
                ret += '\x9D';
            else if( ac == 'é' )
                ret += '\x82';
            else if( ac == '%' )
                ret += ' ';
            else if( ac == '|' )
                ret += (char)136;
            else if( ac == '#' )
                ret += (char)137;
            else
                ret += ac;
            continue;
        }
        if( ac == '|' )
            ret += (char)136;
        else if( ac == '#' )
            ret += (char)137;
        else
            ret += ac;
    }
    ret += '\0';
    return ret;
}

std::wstring readWString( FILE* p_file, bool p_new ) {
    std::wstring ret = L"";
    char ac;
    while( ( ac = fgetc( p_file ) ) == '\n' || ac == '\r' );
    if( ac == '*' ) {
        ret += L'\0';
        return ret;
    } else ret += ac;
    while( ( ac = fgetc( p_file ) ) != '*' ) {
        if( !p_new ) {
            if( ac == 'ä' )
                ret += '\x84';
            else if( ac == 'Ä' )
                ret += '\x8E';
            else if( ac == 'ü' )
                ret += '\x81';
            else if( ac == 'Ü' )
                ret += '\x9A';
            else if( ac == 'ö' )
                ret += '\x94';
            else if( ac == 'Ö' )
                ret += '\x99';
            else if( ac == 'ß' )
                ret += '\x9D';
            else if( ac == 'é' )
                ret += '\x82';
            else if( ac == '%' )
                ret += ' ';
            else
                ret += ac;
            continue;
        }
        if( ac == '|' )
            ret += (char)136;
        else if( ac == '#' )
            ret += (char)137;
        else
            ret += ac;
    }
    ret += L'\0';
    return ret;
}

int item::getItemId( ) {
    for( int i = 0; i < 700; ++i )
        if( ItemList[ i ].m_itemName == m_itemName )
            return i;
    return 0;
}

bool item::load( ) {
    //    std::stringstream FILENAME;
    //    FILENAME << ITEM_PATH << Name << ".data";
    //    FILE* f = fopen(FILENAME.str().c_str(),"r");
    //    if(f == 0)
    //        return false;
    //    //itemtype = GOODS;
    //    int ac;
    //    fscanf(f,"%i",&ac);
    //    effekt = item::EFFEKT(ac);
    //    fscanf(f,"%i\n",&(price));
    //    displayName = readString(f);
    //    dscrpt = readString(f);
    //    effekt_script = readString(f);
    //    fclose(f);
    return m_loaded = true;
}

bool berry::load( ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );

    if( f == 0 )
        return m_loaded = false;
    //itemtype = BERRIES;
    int ac;
    fscanf( f, "%i", &ac );
    //effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(price));
    fscanf( f, "%i", &ac );
    /*displayName = */readString( f, false );
    /*dscrpt = "  "+ */readString( f, false );
    /*effekt_script = */readString( f, false );

    fscanf( f, "%hi", &( m_berrySize ) );

    fscanf( f, "%i", &ac );
    m_berryGuete = berry::berryGueteType( ac );

    fscanf( f, "%i", &ac );
    m_naturalGiftType = Type( ac );

    fscanf( f, "%hhu", &( m_naturalGiftStrength ) );

    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &( m_berryTaste[ i ] ) );

    fscanf( f, "%hhu", &( m_hoursPerGrowthStage ) );

    fscanf( f, "%hhu", &( m_minBerries ) );

    fscanf( f, "%hhu\n", &( m_maxBerries ) );

    fclose( f );
    return m_loaded = true;
}

std::string item::getDescription( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return "Keine Daten.";
    int ac;
    fscanf( f, "%i", &ac );
    fscanf( f, "%i\n", &ac );
    std::string s = readString( f, p_new );
    s = readString( f, p_new );
    fclose( f );
    return s;
}

std::string item::getDisplayName( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return m_itemName;
    int ac;
    fscanf( f, "%i", &ac );
    fscanf( f, "%i\n", &ac );
    std::string s = readString( f, p_new );
    fclose( f );
    return s;
}

item::itemEffectType item::getEffectType( ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return ( item::itemEffectType::NONE );
    int ac;
    fscanf( f, "%i", &ac );
    fclose( f );
    return ( item::itemEffectType )ac;
}

item::itemType item::getItemType( ) {
    return m_itemType;
}

int item::getPrice( ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return 0;
    int ac;
    fscanf( f, "%i", &ac );
    fscanf( f, "%i\n", &ac );
    fclose( f );
    return ac;
}

std::string berry::getDescription2( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return "Keine Daten.";
    //itemtype = BERRIES;
    int ac;
    fscanf( f, "%i", &ac );
    //effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(price));
    fscanf( f, "%i", &ac );
    /*displayName = */readString( f, p_new );
    /*dscrpt = "  "+ */readString( f, p_new );
    /*effekt_script = */readString( f, p_new );

    fscanf( f, "%hi", &( ac ) );
    fscanf( f, "%i", &ac );
    //Guete = berry::Guete_Type(ac);
    fscanf( f, "%i", &ac );
    //BeerenKr_Type = Type(ac);
    fscanf( f, "%hhu", &( ac ) );
    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu\n", &( ac ) );

    std::string s = readString( f, p_new );
    fclose( f );
    return s;
}

std::string item::getShortDescription( bool p_new ) {
    sprintf( buffer, "%s%s.data", ITEM_PATH, m_itemName.c_str( ) );
    FILE* f = fopen( buffer, "r" );
    if( f == 0 )
        return "Keine Daten.";
    int ac;
    fscanf( f, "%i", &ac );
    //effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(price));
    fscanf( f, "%i", &ac );
    /*displayName = */readString( f, p_new );
    /*dscrpt = "  "+ */readString( f, p_new );
    /*effekt_script = */readString( f, p_new );

    if( fscanf( f, "%hi", &( ac ) ) == EOF )
        return "Keine Daten.";
    fscanf( f, "%i", &ac );
    //Guete = berry::Guete_Type(ac);
    fscanf( f, "%i", &ac );
    //BeerenKr_Type = Type(ac);
    fscanf( f, "%hhu", &( ac ) );
    for( int i = 0; i < 5; ++i )
        fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu", &( ac ) );
    fscanf( f, "%hhu\n", &( ac ) );

    readString( f, p_new );
    std::string s = readString( f, p_new );
    fclose( f );
    return s;
}

item ItemList[ 700 ] = {
    item( "Null" ),
    ball( "Meisterball" ), ball( "Hyperball" ),
    ball( "Superball" ), ball( "Pokeball" ),
    ball( "Safariball" ), ball( "Netzball" ),
    ball( "Tauchball" ), ball( "Nestball" ),
    ball( "Wiederball" ), ball( "Timerball" ),
    ball( "Luxusball" ), ball( "Premierball" ),
    ball( "Finsterball" ), ball( "Heilball" ),
    ball( "Flottball" ), ball( "Jubelball" ),
    medicine( "Trank" ), medicine( "Gegengift" ),
    medicine( "Feuerheiler" ), medicine( "Eisheiler" ),
    medicine( "Aufwecker" ), medicine( "Para-Heiler" ),
    medicine( "Top-Genesung" ), medicine( "Top-Trank" ),
    medicine( "Hypertrank" ), medicine( "Supertrank" ),
    medicine( "Hyperheiler" ), medicine( "Beleber" ),
    medicine( "Top-Beleber" ), medicine( "Tafelwasser" ),
    medicine( "Sprudel" ), medicine( "Limonade" ),
    medicine( "KuhMuh-Milch" ), medicine( "Energiestaub" ),
    medicine( "Kraftwurzel" ), medicine( "Heilpuder" ),
    medicine( "Vitalkraut" ), medicine( "AEther" ),
    medicine( "Top-AEther" ), medicine( "Elixir" ),
    medicine( "Top-Elixir" ), medicine( "Lavakeks" ),
    medicine( "Beerensaft" ), medicine( "Zauberasche" ),
    medicine( "KP-Plus" ), medicine( "Protein" ),
    medicine( "Eisen" ), medicine( "Carbon" ),
    medicine( "Kalzium" ), medicine( "Sonderbonbon" ),
    medicine( "AP-Plus" ), medicine( "Zink" ),
    medicine( "AP-Top" ), medicine( "Spezialitaet" ),

    battleItem( "Megablock" ), battleItem( "Angriffsplus" ),
    battleItem( "X-Angriff" ), battleItem( "X-Abwehr" ),
    battleItem( "X-Tempo" ), battleItem( "X-Treffer" ),
    battleItem( "X-Spezial" ), battleItem( "X-SpezialVer" ),
    battleItem( "Pokepuppe" ), battleItem( "Eneco-Rute" ),
    battleItem( "BlaueFloete" ), battleItem( "GelbeFloete" ),
    battleItem( "RoteFloete" ), battleItem( "SchwarzeFloete" ),
    battleItem( "WeisseFloete" ),

    item( "Kuestensalz" ), item( "Kuestenschale" ),
    item( "Purpurstueck" ), item( "Indigostueck" ),
    item( "Gelbstueck" ), item( "Gruenstueck" ),
    item( "Superschutz" ), item( "Top-Schutz" ),
    item( "Fluchtseil" ), item( "Schutz" ),
    item( "Sonnenstein" ), item( "Mondstein" ),
    item( "Feuerstein" ), item( "Donnerstein" ),
    item( "Wasserstein" ), item( "Blattstein" ),
    item( "Minipilz" ), item( "Riesenpilz" ),
    item( "Perle" ), item( "Riesenperle" ),
    item( "Sternenstaub" ), item( "Sternenstueck" ),
    item( "Nugget" ), item( "Herzschuppe" ),
    item( "Honig" ), item( "Wachsmulch" ),
    item( "Feuchtmulch" ), item( "Stabilmulch" ),
    item( "Neumulch" ), item( "Wurzelfossil" ),
    item( "Klauenfossil" ), item( "Helixfossil" ),
    item( "Domfossil" ), item( "Altbernstein" ),
    item( "Schildfossil" ), item( "Kopffossil" ),
    item( "Steinknochen" ), item( "Funkelstein" ),
    item( "Finsterstein" ), item( "Leuchtstein" ),
    item( "OvalerStein" ), item( "Spiritkern" ),
    item( "Platinum-Orb" ),

    item( "Null" ),
    item( "Null" ),
    item( "Null" ),

    item( "Aquamodul" ), item( "Blitzmodul" ),
    item( "Flammenmodul" ), item( "Gefriermodul" ),

    mail( "Wiesenbrief" ), mail( "Feuerbrief" ),
    mail( "Wasserbrief" ), mail( "Bluetenbrief" ),
    mail( "Minenbrief" ), mail( "Stahlbrief" ),
    mail( "Rosabrief" ), mail( "Schneebrief" ),
    mail( "Sternbrief" ), mail( "Luftbrief" ),
    mail( "Mosaikbrief" ), mail( "Ziegelbrief" ),

    item( "Null" ),
    item( "Null" ),

    item( "Herzkonfekt" ), item( "Adamant-Orb" ),
    item( "Weiss-Orb" ),

    mail( "Grussbrief" ), mail( "Faiblebrief" ),
    mail( "Einladebrief" ), mail( "Dankesbrief" ),
    mail( "Fragebrief" ), mail( "Insiderbrief" ),
    mail( "Rueckbrief" ), mail( "BrueckbriefH" ),
    mail( "BrueckbriefM" ), mail( "BrueckbriefZ" ),
    mail( "BrueckbriefD" ), mail( "BrueckbriefW" ),

    berry( "Amrenabeere" ), berry( "Maronbeere" ),
    berry( "Pirsifbeere" ), berry( "Fragiabeere" ),
    berry( "Wilbirbeere" ), berry( "Jonagobeere" ),
    berry( "Sinelbeere" ), berry( "Persimbeere" ),
    berry( "Prunusbeere" ), berry( "Tsitrubeere" ),
    berry( "Giefebeere" ), berry( "Wikibeere" ),
    berry( "Magobeere" ), berry( "Gauvebeere" ),
    berry( "Yapabeere" ), berry( "Himmihbeere" ),
    berry( "Morbbeere" ), berry( "Nanabbeere" ),
    berry( "Nirbebeere" ), berry( "Sananabeere" ),
    berry( "Granabeere" ), berry( "Setangbeere" ),
    berry( "Qualotbeere" ), berry( "Honmelbeere" ),
    berry( "Labrusbeere" ), berry( "Tamotbeere" ),
    berry( "Saimbeere" ), berry( "Magostbeere" ),
    berry( "Rabutabeere" ), berry( "Tronzibeere" ),
    berry( "Kiwanbeere" ), berry( "Pallmbeere" ),
    berry( "Wasmelbeere" ), berry( "Durinbeere" ),
    berry( "Myrtilbeere" ), berry( "Koakobeere" ),
    berry( "Foepasbeere" ), berry( "Kerzalbeere" ),
    berry( "Grindobeere" ), berry( "Kiroyabeere" ),
    berry( "Rospelbeere" ), berry( "Grarzbeere" ),
    berry( "Schukebeere" ), berry( "Kobabeere" ),
    berry( "Pyapabeere" ), berry( "Tanigabeere" ),
    berry( "Chiaribeere" ), berry( "Zitarzbeere" ),
    berry( "Terirobeere" ), berry( "Burleobeere" ),
    berry( "Babiribeere" ), berry( "Latchibeere" ),
    berry( "Lydzibeere" ), berry( "Linganbeere" ),
    berry( "Salkabeere" ), berry( "Tahaybeere" ),
    berry( "Apikobeere" ), berry( "Lansatbeere" ),
    berry( "Krambobeere" ), berry( "Enigmabeere" ),
    berry( "Wunfrubeere" ), berry( "Eipfelbeere" ),
    berry( "Jabocabeere" ), berry( "Roselbeere" ),

    item( "Blendpuder" ), item( "Schlohkraut" ),
    item( "Machoband" ), keyItem( "EP-Teiler" ),
    item( "Flinkklaue" ), item( "Sanftglocke" ),
    item( "Mentalkraut" ), item( "Wahlband" ),
    item( "King-Stein" ), item( "Silberstaub" ),
    item( "Muenzamulett" ), item( "Schutzband" ),
    item( "Seelentau" ), item( "Abysszahn" ),
    item( "Abyssplatte" ), item( "Rauchball" ),
    item( "Ewigstein" ), item( "Fokus-Band" ),
    item( "Gluecks-Ei" ), item( "Scope-Linse" ),
    item( "Metallmantel" ), item( "UEberreste" ),
    item( "Drachenhaut" ), item( "Kugelblitz" ),
    item( "Pudersand" ), item( "Granitstein" ),
    item( "Wundersaat" ), item( "Schattenglas" ),
    item( "Schwarzgurt" ), item( "Magnet" ),
    item( "Zauberwasser" ), item( "Hackmove" ),
    item( "Giftstich" ), item( "EwigesEis" ),
    item( "Bannsticker" ), item( "Krummloeffel" ),
    item( "Holzkohle" ), item( "Drachenzahn" ),
    item( "Seidenschal" ), item( "Up-Grade" ),
    item( "Seegesang" ), item( "Seerauch" ),
    item( "Laxrauch" ), item( "LuckyPunch" ),
    item( "Metallstaub" ), item( "Kampfknochen" ),
    item( "Lauchstange" ), item( "RoterSchal" ),
    item( "BlauerSchal" ), item( "RosaSchal" ),
    item( "GruenerSchal" ), item( "GelberSchal" ),
    item( "Grosslinse" ), item( "Muskelband" ),
    item( "Schlauglas" ), item( "Expertengurt" ),
    item( "Lichtlehm" ), item( "Leben-Orb" ),
    item( "Energiekraut" ), item( "Toxik-Orb" ),
    item( "Heiss-Orb" ), item( "Flottstaub" ),
    item( "Fokusgurt" ), item( "Zoomlinse" ),
    item( "Metronom" ), item( "Eisenkugel" ),
    item( "Schwerschweif" ), item( "Fatumknoten" ),
    item( "Giftschleim" ), item( "Eisbrocken" ),
    item( "Glattbrocken" ), item( "Heissbrocken" ),
    item( "Nassbrocken" ), item( "Griffklaue" ),
    item( "Wahlschal" ), item( "Klettdorn" ),
    item( "Machtreif" ), item( "Machtgurt" ),
    item( "Machtlinse" ), item( "Machtband" ),
    item( "Machtkette" ), item( "Machtgewicht" ),
    item( "Wechselhuelle" ), item( "Grosswurzel" ),
    item( "Wahlglas" ), item( "Feuertafel" ),
    item( "Wassertafel" ), item( "Blitztafel" ),
    item( "Wiesentafel" ), item( "Frosttafel" ),
    item( "Fausttafel" ), item( "Gifttafel" ),
    item( "Erdtafel" ), item( "Wolkentafel" ),
    item( "Hirntafel" ), item( "Kaefertafel" ),
    item( "Steintafel" ), item( "Spuktafel" ),
    item( "Dracotafel" ), item( "Furchttafel" ),
    item( "Eisentafel" ), item( "Schraegrauch" ),
    item( "Steinrauch" ), item( "Lahmrauch" ),
    item( "Wellenrauch" ), item( "Rosenrauch" ),
    item( "Gluecksrauch" ), item( "Scheurauch" ),
    item( "Schuetzer" ), item( "Elektrisierer" ),
    item( "Magmaisierer" ), item( "Dubiosdisk" ),
    item( "Duesterumhang" ), item( "Scharfklaue" ),
    item( "Scharfzahn" ),

    TM( "TM01" ), TM( "TM02" ), TM( "TM03" ), TM( "TM04" ),
    TM( "TM05" ), TM( "TM06" ), TM( "TM07" ), TM( "TM08" ),
    TM( "TM09" ), TM( "TM10" ), TM( "TM11" ), TM( "TM12" ),
    TM( "TM13" ), TM( "TM14" ), TM( "TM15" ), TM( "TM16" ),
    TM( "TM17" ), TM( "TM18" ), TM( "TM19" ), TM( "TM20" ),
    TM( "TM21" ), TM( "TM22" ), TM( "TM23" ), TM( "TM24" ),
    TM( "TM25" ), TM( "TM26" ), TM( "TM27" ), TM( "TM28" ),
    TM( "TM29" ), TM( "TM30" ), TM( "TM31" ), TM( "TM32" ),
    TM( "TM33" ), TM( "TM34" ), TM( "TM35" ), TM( "TM36" ),
    TM( "TM37" ), TM( "TM38" ), TM( "TM39" ), TM( "TM40" ),
    TM( "TM41" ), TM( "TM42" ), TM( "TM43" ), TM( "TM44" ),
    TM( "TM45" ), TM( "TM46" ), TM( "TM47" ), TM( "TM48" ),
    TM( "TM49" ), TM( "TM50" ), TM( "TM51" ), TM( "TM52" ),
    TM( "TM53" ), TM( "TM54" ), TM( "TM55" ), TM( "TM56" ),
    TM( "TM57" ), TM( "TM58" ), TM( "TM59" ), TM( "TM60" ),
    TM( "TM61" ), TM( "TM62" ), TM( "TM63" ), TM( "TM64" ),
    TM( "TM65" ), TM( "TM66" ), TM( "TM67" ), TM( "TM68" ),
    TM( "TM69" ), TM( "TM70" ), TM( "TM71" ), TM( "TM72" ),
    TM( "TM73" ), TM( "TM74" ), TM( "TM75" ), TM( "TM76" ),
    TM( "TM77" ), TM( "TM78" ), TM( "TM79" ), TM( "TM80" ),
    TM( "TM81" ), TM( "TM82" ), TM( "TM83" ), TM( "TM84" ),
    TM( "TM85" ), TM( "TM86" ), TM( "TM87" ), TM( "TM88" ),
    TM( "TM89" ), TM( "TM90" ), TM( "TM91" ), TM( "TM92" ),
    TM( "VM01" ), TM( "VM02" ), TM( "VM03" ), TM( "VM04" ),
    TM( "VM05" ), TM( "VM06" ), TM( "VM07" ), TM( "VM08" ),

    keyItem( "Forschersack" ), keyItem( "Beutesack" ),
    keyItem( "Regelbuch" ), keyItem( "Poke-Radar" ),
    keyItem( "Punktekarte" ), keyItem( "Tagebuch" ),
    keyItem( "StickKoffer" ), keyItem( "Modekoffer" ),
    keyItem( "Stickertuete" ), keyItem( "Adressbuch" ),
    keyItem( "K-Schluessel" ), keyItem( "Talisman" ),
    keyItem( "G-Schluessel" ), keyItem( "RoteKette" ),
    keyItem( "Karte" ), keyItem( "Kampffahnder" ),
    keyItem( "Muenzkorb" ), keyItem( "Angel" ),
    keyItem( "Profiangel" ), keyItem( "Superangel" ),
    keyItem( "Entonkanne" ), keyItem( "Knurspbox" ),
    keyItem( "Fahrrad" ), keyItem( "B-Schluessel" ),
    keyItem( "EichsBrief" ), keyItem( "Lunarfeder" ),
    keyItem( "Mitglkarte" ), keyItem( "Azurfloete" ),
    keyItem( "Bootsticket" ), keyItem( "Wettb-karte" ),
    keyItem( "Magmastein" ), keyItem( "Paket" ),
    keyItem( "Kupon1" ), keyItem( "Kupon2" ),
    keyItem( "Kupon3" ), keyItem( "L-Schluessel" ),
    keyItem( "Geheimtrank" ), keyItem( "Kampfkamera" ),
    keyItem( "Gracidea" ), keyItem( "F-OEffner" ),
    keyItem( "Aprikokobox" ), keyItem( "Icognitoheft" ),
    keyItem( "Pflanzset" ), keyItem( "Itemradar" ),
    keyItem( "BlaueKarte" ), keyItem( "Flegmon-Rute" ),
    keyItem( "Klarglocke" ), keyItem( "Tueroeffner" ),
    keyItem( "Kelleroeffner" ), keyItem( "Schiggykanne" ),
    keyItem( "RoteHaut" ), keyItem( "Fundsache" ),
    keyItem( "Fahrschein" ), keyItem( "Spule" ),
    keyItem( "Silberfluegel" ), keyItem( "Buntschwinge" ),
    keyItem( "Raetsel-Ei" ), keyItem( "Devon-Scope" ),
    keyItem( "Auroraticket" ), keyItem( "AlteKarte" ),

    TM( "VM09" ), TM( "VM10" ),
    TM( "VM11" ), TM( "VM12" ),

    ball( "Turboball" ), ball( "Levelball" ),
    ball( "Koederball" ), ball( "Schwerball" ),
    ball( "Sympaball" ), ball( "Freundesball" ),
    ball( "Mondball" ), ball( "Turnierball" ),
    ball( "Parkball" ),

    keyItem( "Fotoalbum" ), keyItem( "GB-Player" ),
    keyItem( "Gischtglocke" ), medicine( "Wutkeks" ),

    berry( "Pumkinbeere" ), berry( "Drashbeere" ),
    berry( "Eggantbeere" ), berry( "Stribbeere" ),
    berry( "Chilianbeere" ), berry( "Nutpeabeere" ),
    berry( "Ginemabeere" ), berry( "Kuobeere" ),
    berry( "Yagobeere" ), berry( "Tougabeere" ),
    berry( "Ninikubeere" ), berry( "Topobeere" ),

    keyItem( "K1-Schluessel" ), keyItem( "K2-Schluessel" ),
    keyItem( "K4-Schluessel" ), keyItem( "K6-Schluessel" ),
    keyItem( "Meteorit" ), keyItem( "Wuestenglas" ),
    keyItem( "AEon-Ticket" ), keyItem( "Scanner" ),
    keyItem( "PokeRiegelBox" ), keyItem( "Devon-Waren" ),
    keyItem( "Kunstrad" ), keyItem( "Eilrad" ),
    keyItem( "Aschetasche" ), keyItem( "Wailmerkanne" ),
    keyItem( "Sphaerensegm" ), keyItem( "GrueneKugel" ),
    keyItem( "Tresorkapsel" ), keyItem( "RoteKugel" ),
    keyItem( "BlaueKugel" ), keyItem( "Mytokristall" ),

    item( "Schoenschuppe" ), item( "Evolith" ),
    item( "Leichtstein" ), item( "Beulenhelm" ),
    item( "Luftballon" ), item( "RoteKarte" ),
    item( "Zielscheibe" ), item( "Klammerband" ),
    item( "Knolle" ), item( "Akku" ),
    item( "Fluchtknopf" ),

    item( "Feuerjuwel" ), item( "Wasserjuwel" ),
    item( "Elektrojuwel" ), item( "Pflanzjuwel" ),
    item( "Eisjuwel" ), item( "Kampfjuwel" ),
    item( "Giftjuwel" ), item( "Bodenjuwel" ),
    item( "Flugjuwel" ), item( "Psychojuwel" ),
    item( "Kaeferjuwel" ), item( "Gesteinjuwel" ),
    item( "Geistjuwel" ), item( "Drakojuwel" ),
    item( "Unlichtjuwel" ), item( "Stahljuwel" ),
    item( "Normaljuwel" ),

    medicine( "Heilfeder" ), medicine( "Kraftfeder" ),
    medicine( "Abwehrfeder" ), medicine( "Geniefeder" ),
    medicine( "Espritfeder" ), medicine( "Flinkfeder" ),
    item( "Prachtfeder" ),

    item( "Schildfossil" ), item( "Federfossil" ),

    keyItem( "Gartenpass" ),
    item( "Transferorb" ), item( "Traumball" ),
    keyItem( "Deko-Box" ), keyItem( "Drakoschaedel" ),

    item( "Duftpilz" ), item( "Riesennugget" ),
    item( "Triperle" ), item( "Kometstueck" ),
    item( "AlterHeller" ), item( "AlterTaler" ),
    item( "AlterDukat" ), item( "AlteVase" ),
    item( "AlterReif" ), item( "AlteStatue" ),
    item( "AlteKrone" ),

    medicine( "Stratos-Eis" ),

    item( "Null" ), item( "Null" ), item( "Null" ), item( "Null" ),
    item( "Null" ), item( "Null" ), item( "Null" ), item( "Null" ),
    item( "Null" ), item( "Null" ), item( "Null" ), item( "Null" ),
    item( "Null" ), item( "Null" ), item( "Null" ), item( "Null" ),
    item( "Null" ), item( "Null" ), item( "Null" ), item( "Null" ),
    item( "Null" ), item( "Null" ), item( "Null" ), item( "Null" ),

    keyItem( "Lichtstein" ),
    keyItem( "Dunkelstein" ),
    TM( "TM93" ),
    TM( "TM94" ),
    TM( "TM95" ),
    keyItem( "Viso-Casterm" ),
    keyItem( "Nebelstein" ),
    keyItem( "Briefpost" ),
    keyItem( "Briefpost" ),
    keyItem( "Briefpost" ),
    keyItem( "Viso-Caster" ),
    keyItem( "Medaillenbox" ),
    keyItem( "DNS-Keil" ),
    keyItem( "Genehmigung" ),
    keyItem( "Ovalpin" ),
    keyItem( "Schillerpin" ),
    keyItem( "Plasmakarte" ),
    keyItem( "Schnaeuztuch" ),
    keyItem( "Achromat" ),
    keyItem( "Fundsache" ),
    keyItem( "Wahrspiegel" )
};