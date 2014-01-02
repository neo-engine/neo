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

const char ITEM_PATH[] = "nitro:/ITEMS/";

std::string readString(FILE* fd){
    std::string ret = "";
    char ac; 
    while((ac= fgetc(fd)) == '\n' || ac == '\r');
    if (ac == '*')
        return ret;
    else ret += ac;
    while(((ac = fgetc(fd)) != '*')){
        if(ac == 'ä')
            ret += '\x84';
        else if(ac == 'Ä')
            ret += '\x8E';
        else if(ac == 'ü')
            ret += '\x81';
        else if(ac == 'Ü')
            ret += '\x9A';
        else if(ac == 'ö')
            ret += '\x94';
        else if(ac == 'Ö')
            ret += '\x99';
        else if(ac == 'ß')
            ret += '\x9D';
        else if(ac == 'é')
            ret += '\x82';
        else
            ret += ac;
    }
    return ret;
}
std::wstring readWString(FILE* fd){
    std::wstring ret = L"";
    char ac; 
    while((ac= fgetc(fd)) == '\n' || ac == '\r');
    if (ac == '*')
        return ret;
    else ret += ac;
    while(((ac = fgetc(fd)) != '*')){
        if(ac == 'ä')
            ret += '\x84';
        else if(ac == 'Ä')
            ret += '\x8E';
        else if(ac == 'ü')
            ret += '\x81';
        else if(ac == 'Ü')
            ret += '\x9A';
        else if(ac == 'ö')
            ret += '\x94';
        else if(ac == 'Ö')
            ret += '\x99';
        else if(ac == 'ß')
            ret += '\x9D';
        else if(ac == 'é')
            ret += '\x82';
        else
            ret += ac;
    }
    return ret;
}

int item::getID(){
    for(int i= 0; i <700; ++i)
        if(ItemList[i].Name == this->Name)
            return i;
}

bool item::_load(){
//    std::stringstream FILENAME;
//    FILENAME << ITEM_PATH << this->Name << ".data";
//    FILE* f = fopen(FILENAME.str().c_str(),"r");
//    if(f == 0)
//        return false;
//    //this->itemtype = GOODS;
//    int ac;
//    fscanf(f,"%i",&ac);
//    this->effekt = item::EFFEKT(ac);
//    fscanf(f,"%i\n",&(this->price));
//    this->displayName = readString(f);
//    this->dscrpt = readString(f);
//    this->effekt_script = readString(f);
//    fclose(f);
    return load = true;
}
char buf[100];
bool berry::_load(){
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");

    if(f == 0)
        return load = false;
    //this->itemtype = BERRIES;
    int ac;
    fscanf(f,"%i",&ac);
    //this->effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(this->price));
    fscanf(f,"%i",&ac);
    /*this->displayName = */readString(f);
    /*this->dscrpt = "  "+ */readString(f);
    /*this->effekt_script = */readString(f);

    fscanf(f,"%hi",&(this->size));
    fscanf(f,"%i",&ac);
    this->Guete = berry::Guete_Type(ac);
    fscanf(f,"%i",&ac);
    this->BeerenKr_Type = Type(ac);
    fscanf(f,"%hhu",&(this->BeerenKr_Str));
    for(int i= 0; i< 5; ++i)
        fscanf(f,"%hhu",&(this->Taste[i]));
    fscanf(f,"%hhu",&(this->HoursPerStage));
    fscanf(f,"%hhu",&(this->minBerries));
    fscanf(f,"%hhu\n",&(this->maxBerries));
    fclose(f);
    return load = true;
}

std::string item::getDescription(){
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");
    if(f == 0)
        return "Keine Daten.";
    int ac;
    fscanf(f,"%i",&ac);
    fscanf(f,"%i\n",&ac);
    std::string s = readString(f);
    s = readString(f);
    fclose(f);
    return s;
}
std::string item::getDisplayName(){
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");
    if(f == 0)
        return this->Name;
    int ac;
    fscanf(f,"%i",&ac);
    fscanf(f,"%i\n",&ac);
    std::string s = readString(f);
    fclose(f);
    return s;
}
item::EFFEKT item::getEffekt(){
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");
    if(f == 0)
        return (item::EFFEKT::NONE);
    int ac;
    fscanf(f,"%i",&ac);
    fclose(f);
    return (item::EFFEKT)ac;
}
item::ITEM_TYPE item::getItemType(){
    return this->itemtype;
}
int item::getPrice(){ 
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");
    if(f == 0)
        return 0;
    int ac;
    fscanf(f,"%i",&ac);
    fscanf(f,"%i\n",&ac);
    fclose(f);
    return ac;
}

std::string berry::getDescription2(){
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");
    if(f == 0)
        return "Keine Daten.";
    //this->itemtype = BERRIES;
    int ac;
    fscanf(f,"%i",&ac);
    //this->effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(this->price));
    fscanf(f,"%i",&ac);
    /*this->displayName = */readString(f);
    /*this->dscrpt = "  "+ */readString(f);
    /*this->effekt_script = */readString(f);

    fscanf(f,"%hi",&(ac));
    fscanf(f,"%i",&ac);
    //this->Guete = berry::Guete_Type(ac);
    fscanf(f,"%i",&ac);
    //this->BeerenKr_Type = Type(ac);
    fscanf(f,"%hhu",&(ac));
    for(int i= 0; i< 5; ++i)
        fscanf(f,"%hhu",&(ac));
    fscanf(f,"%hhu",&(ac));
    fscanf(f,"%hhu",&(ac));
    fscanf(f,"%hhu\n",&(ac));
    
    std::string s = readString(f);
    fclose(f);
    return s;
}

std::string item::getShortDescription(){
    sprintf(buf,"%s%s.data",ITEM_PATH,this->Name.c_str());
    FILE* f = fopen(buf,"r");
    if(f == 0)
        return "Keine Daten.";
    int ac;
    fscanf(f,"%i",&ac);
    //this->effekt = item::EFFEKT(ac);
    //fscanf(f,"%i\n",&(this->price));
    fscanf(f,"%i",&ac);
    /*this->displayName = */readString(f);
    /*this->dscrpt = "  "+ */readString(f);
    /*this->effekt_script = */readString(f);

    fscanf(f,"%hi",&(ac));
    fscanf(f,"%i",&ac);
    //this->Guete = berry::Guete_Type(ac);
    fscanf(f,"%i",&ac);
    //this->BeerenKr_Type = Type(ac);
    fscanf(f,"%hhu",&(ac));
    for(int i= 0; i< 5; ++i)
        fscanf(f,"%hhu",&(ac));
    fscanf(f,"%hhu",&(ac));
    fscanf(f,"%hhu",&(ac));
    fscanf(f,"%hhu\n",&(ac));
    
    readString(f);
    std::string s = readString(f);
    fclose(f);
    return s;
}

item ItemList[700] = {
    item("Null"),
    ball("Meisterball"),    ball("Hyperball"),
    ball("Superball"),      ball("Pokeball"),
    ball("Safariball"),     ball("Netzball"),
    ball("Tauchball"),      ball("Nestball"),
    ball("Wiederball"),     ball("Timerball"),
    ball("Luxusball"),      ball("Premierball"),
    ball("Finsterball"),    ball("Heilball"),
    ball("Flottball"),      ball("Jubelball"),
    medicine("Trank"),          medicine("Gegengift"),
    medicine("Feuerheiler"),    medicine("Eisheiler"),
    medicine("Aufwecker"),      medicine("Para-Heiler"),
    medicine("Top-Genesung"),   medicine("Top-Trank"),
    medicine("Hypertrank"),     medicine("Supertrank"),
    medicine("Hyperheiler"),    medicine("Beleber"),
    medicine("Top-Beleber"),    medicine("Mineralwasser"),
    medicine("Sprudel"),        medicine("Limonade"),
    medicine("KuhMuh-Milch"),   medicine("Energiestaub"),
    medicine("Kraftwurzel"),    medicine("Heilpuder"),
    medicine("Vitalkraut"),     medicine("AEther"),
    medicine("Top-AEther"),     medicine("Elixir"),
    medicine("Top-Elixir"),     medicine("Lavakeks"), 
    medicine("Beerensaft"),     medicine("Zauberasche"),
    medicine("KP-Plus"),        medicine("Protein"),
    medicine("Eisen"),          medicine("Carbon"),
    medicine("Kalzium"),        medicine("Sonderbonbon"),
    medicine("AP-Plus"),        medicine("Zink"),
    medicine("AP-Top"),         medicine("Spezialitaet"),
    
    battle_item("Megablock"),       battle_item("Angriffsplus"),
    battle_item("X-Angriff"),       battle_item("X-Abwehr"),
    battle_item("X-Tempo"),         battle_item("X-Treffer"),
    battle_item("X-Spezial"),       battle_item("X-SpezialVer"),
    battle_item("Pokepuppe"),       battle_item("Eneco-Rute"),
    battle_item("BlaueFloete"),    battle_item("GelbeFloete"),
    battle_item("RoteFloete"),     battle_item("SchwarzeFloete"),
    battle_item("WeisseFloete"),
    
    item("Kuestensalz"),        item("Kuestenschale"),
    item("Purpurstueck"),       item("Indigostueck"),
    item("Gelbstueck"),         item("Gruenstueck"),
    item("Superschutz"),        item("Top-Schutz"),
    item("Fluchtseil"),         item("Schutz"),
    item("Sonnenstein"),        item("Mondstein"),
    item("Feuerstein"),         item("Donnerstein"),
    item("Wasserstein"),        item("Blattstein"),
    item("Minipilz"),           item("Riesenpilz"),
    item("Perle"),              item("Riesenperle"),
    item("Sternenstaub"),       item("Sternenstueck"),
    item("Nugget"),             item("Herzschuppe"),
    item("Honig"),              item("Wachsmulch"), 
    item("Feuchtmulch"),        item("Stabilmulch"),
    item("Neumulch"),           item("Wurzelfossil"),
    item("Klauenfossil"),       item("Helixfossil"),
    item("Domfossil"),          item("Altbernstein"),
    item("Schildfossil"),       item("Kopffossil"),
    item("Steinknochen"),       item("Funkelstein"),
    item("Finsterstein"),       item("Leuchtstein"),
    item("OvalerStein"),       item("Spiritkern"),
    item("Platin-Orb"),         

    item("Null"),
    item("Null"),
    item("Null"),

    item("Aquamodul"),      item("Blitzmodul"),
    item("Flammenmodul"),   item("Gefriermodul"),

    mail("Wiesenbrief"),    mail("Feuerbrief"),
    mail("Wasserbrief"),    mail("Bluetenbrief"),
    mail("Minenbrief"),     mail("Stahlbrief"),
    mail("Rosabrief"),      mail("Schneebrief"),
    mail("Sternbrief"),     mail("Luftbrief"),
    mail("Mosaikbrief"),    mail("Ziegelbrief"),

    item("Null"),
    item("Null"),

    item("Herzkonfekt"),    item("Adamant-Orb"),
    item("Weiss-Orb"),  

    mail("Grussbrief"),     mail("Faiblebrief"), 
    mail("Einladebrief"),   mail("Dankesbrief"),
    mail("Fragebrief"),     mail("Insiderbrief"),
    mail("Antwortbrief"),   mail("BrueckbriefH"),
    mail("BrueckbriefM"),  mail("BrueckbriefZ"),
    mail("BrueckbriefD"),  mail("BrueckbriefW"),

    berry("Amrenabeere"),   berry("Maronbeere"),
    berry("Pirsifbeere"),	berry("Fragiabeere"),
    berry("Wilbirbeere"),	berry("Jonagobeere"),
    berry("Sinelbeere"),	berry("Persimbeere"),
    berry("Prunusbeere"),	berry("Tsitrubeere"),
    berry("Giefebeere"),	berry("Wikibeere"),
    berry("Magobeere"), 	berry("Gauvebeere"),
    berry("Yapabeere"), 	berry("Himmihbeere"),
    berry("Morbbeere"), 	berry("Nanabbeere"),
    berry("Nirbebeere"),	berry("Sananabeere"),
    berry("Granabeere"),	berry("Setangbeere"),
    berry("Qualotbeere"),	berry("Honmelbeere"),
    berry("Labrusbeere"),	berry("Tamotbeere"),
    berry("Saimbeere"), 	berry("Magostbeere"),
    berry("Rabutabeere"),	berry("Tronzibeere"),
    berry("Kiwanbeere"),	berry("Pallmbeere"),
    berry("Wasmelbeere"),	berry("Durinbeere"),
    berry("Myrtilbeere"),	berry("Koakobeere"),
    berry("Foepasbeere"),	berry("Kerzalbeere"),
    berry("Grindobeere"),	berry("Kiroyabeere"),
    berry("Rospelbeere"),	berry("Grarzbeere"),
    berry("Schukebeere"),	berry("Kobabeere"),
    berry("Pyapabeere"),	berry("Tanigabeere"),
    berry("Chiaribeere"),	berry("Zitarzbeere"),
    berry("Terirobeere"),	berry("Burleobeere"),
    berry("Babiribeere"),	berry("Latchibeere"),
    berry("Lydzibeere"),	berry("Linganbeere"),
    berry("Salkabeere"),	berry("Tahaybeere"),
    berry("Apikobeere"),	berry("Lansatbeere"),
    berry("Krambobeere"),	berry("Enigmabeere"),
    berry("Wunfrubeere"),	berry("Eipfelbeere"),
    berry("Jabocabeere"),	berry("Roselbeere"),
    
    item("Blendpuder"),     item("Schlohkraut"),
    item("Machoband"),      item("EP-Teiler"),
    item("Flinkklaue"),     item("Sanftglocke"),
    item("Mentalkraut"),    item("Wahlband"),
    item("KingsRock"),      item("Silberstaub"),
    item("Muenzamulett"),   item("Schutzband"),
    item("Seelentau"),      item("Abysszahn"),
    item("Abyssplatte"),    item("Rauchball"),
    item("Ewigstein"),      item("Fokusband"),
    item("Gluecksei"),      item("Scope-Linse"),
    item("Metallmantel"),   item("UEberreste"),
    item("Drachenhaut"),    item("Kugelblitz"),
    item("Pudersand"),      item("Granitstein"),
    item("Wundersaat"),     item("Schattenglas"),
    item("Schwarzgurt"),    item("Magnet"),
    item("Zauberwasser"),   item("Hackattack"),
    item("Giftstich"),      item("EwigesEis"),
    item("Bannsticker"),    item("Krummloeffel"),
    item("Holzkohle"),      item("Drachenzahn"),
    item("Seidenschal"),    item("Up-Grade"),
    item("Seegesang"),      item("Seerauch"),
    item("Laxrauch"),       item("LuckyPunch"),
    item("Metallstaub"),    item("Kampfknochen"),
    item("Lauchstange"),    item("RoterSchal"),
    item("BlauerSchal"),    item("PinkerSchal"),
    item("GruenerSchal"),   item("GelberSchal"),
    item("Grosslinse"),     item("Muskelband"),
    item("Schlauglas"),     item("Expertengurt"),
    item("Lichtlehm"),      item("Leben-Orb"),
    item("Energiekraut"),   item("Toxik-Orb"),
    item("Heiss-Orb"),      item("Flottstaub"),
    item("Fokusgurt"),      item("Zoomlinse"),
    item("Metronom"),       item("Eisenkugel"),
    item("Schwerschweif"),  item("Fatumknoten"),
    item("Giftschleim"),    item("Eisbrocken"),
    item("Glattbrocken"),   item("Heissbrocken"),
    item("Nassbrocken"),    item("Griffklaue"),
    item("Wahlschal"),      item("Klettdorn"),
    item("Machtreif"),      item("Machtgurt"),
    item("Machtlinse"),     item("Machtband"),
    item("Machtkette"),     item("Machtgewicht"),
    item("Wechselhuelle"),  item("Grosswurzel"),
    item("Wahlglas"),       item("Feuertafel"),
    item("Wassertafel"),    item("Blitztafel"),
    item("Wiesentafel"),    item("Frosttafel"),
    item("Fausttafel"),     item("Gifttafel"),
    item("Erdtafel"),       item("Wolkentafel"),
    item("Hirntafel"),      item("Kaefertafel"),
    item("Steintafel"),     item("Spuktafel"),
    item("Drakotafel"),     item("Furchttafel"),
    item("Eisentafel"),     item("Schraegrauch"),
    item("Steinrauch"),     item("Lahmrauch"),
    item("Wellenrauch"),    item("Rosenrauch"),
    item("Gluecksrauch"),   item("Scheurauch"),
    item("Schuetzer"),      item("Elektrisierer"),
    item("Magmaisierer"),   item("Dubiosdisk"),
    item("Finsterumhang"),  item("Scharfklaue"),
    item("Scharfzahn"),
    
    TM("TM01"),    TM("TM02"),    TM("TM03"),    TM("TM04"),
    TM("TM05"),    TM("TM06"),    TM("TM07"),    TM("TM08"),
    TM("TM09"),    TM("TM10"),    TM("TM11"),    TM("TM12"),
    TM("TM13"),    TM("TM14"),    TM("TM15"),    TM("TM16"),
    TM("TM17"),    TM("TM18"),    TM("TM19"),    TM("TM20"),
    TM("TM21"),    TM("TM22"),    TM("TM23"),    TM("TM24"),
    TM("TM25"),    TM("TM26"),    TM("TM27"),    TM("TM28"),
    TM("TM29"),    TM("TM30"),    TM("TM31"),    TM("TM32"),
    TM("TM33"),    TM("TM34"),    TM("TM35"),    TM("TM36"),
    TM("TM37"),    TM("TM38"),    TM("TM39"),    TM("TM40"),
    TM("TM41"),    TM("TM42"),    TM("TM43"),    TM("TM44"),
    TM("TM45"),    TM("TM46"),    TM("TM47"),    TM("TM48"),
    TM("TM49"),    TM("TM50"),    TM("TM51"),    TM("TM52"),
    TM("TM53"),    TM("TM54"),    TM("TM55"),    TM("TM56"),
    TM("TM57"),    TM("TM58"),    TM("TM59"),    TM("TM60"),
    TM("TM61"),    TM("TM62"),    TM("TM63"),    TM("TM64"),
    TM("TM65"),    TM("TM66"),    TM("TM67"),    TM("TM68"),
    TM("TM69"),    TM("TM70"),    TM("TM71"),    TM("TM72"),
    TM("TM73"),    TM("TM74"),    TM("TM75"),    TM("TM76"),
    TM("TM77"),    TM("TM78"),    TM("TM79"),    TM("TM80"),
    TM("TM81"),    TM("TM82"),    TM("TM83"),    TM("TM84"),
    TM("TM85"),    TM("TM86"),    TM("TM87"),    TM("TM88"),
    TM("TM89"),    TM("TM90"),    TM("TM91"),    TM("TM92"),
    TM("VM01"),    TM("VM02"),    TM("VM03"),    TM("VM04"),
    TM("VM05"),    TM("VM06"),    TM("VM07"),    TM("VM08"),

    key_item("Forschersack"),   key_item("Beutesack"),
    key_item("Regelbuch"),      key_item("Poke-Radar"),
    key_item("Punktekarte"),    key_item("Tagebuch"),
    key_item("StickKoffer"),    key_item("Modekoffer"),
    key_item("Stickertuete"),   key_item("Adressbuch"),
    key_item("K-Schluessel"),   key_item("Talisman"),
    key_item("G-Schluessel"),   key_item("RoteKette"),
    key_item("Karte"),          key_item("Kampffahnder"),
    key_item("Muenzkorb"),      key_item("Angel"),
    key_item("Profiangel"),     key_item("Superangel"),
    key_item("Entonkanne"),     key_item("Knurspbox"),
    key_item("Fahrrad"),        key_item("B-Schluessel"),
    key_item("EichsBrief"),     key_item("Lunarfeder"),
    key_item("Mitglkarte"),     key_item("Azurfloete"),
    key_item("Bootsticket"),    key_item("Wettb-karte"),
    key_item("Magmastein"),     key_item("Paket"),
    key_item("Coupon1"),        key_item("Coupon2"),
    key_item("Coupon3"),        key_item("L-Schlussel"),
    key_item("Geheimtrank"),    key_item("Kampfkamera"),
    key_item("Gracidea"),       key_item("F-OEffner"),
    key_item("Aprikokobox"),    key_item("Icognitoheft"),
    key_item("Pflanzset"),      key_item("Itemradar"),
    key_item("BlaueKarte"),     key_item("Flegmon-Rute"),
    key_item("Klarglocke"),     key_item("Tueroeffner"),
    key_item("Kelleroeffner"),  key_item("Schiggykanne"),
    key_item("RoteHaut"),       key_item("Fundsache"),
    key_item("Fahrkarte"),      key_item("Spule"),
    key_item("Silberfluegel"),  key_item("Buntschwinge"),
    key_item("Kurios-Ei"),      key_item("Devon-Scope"), 
    key_item("Auroraticket"),   key_item("AlteSeekarte"),
 
    TM("VM09"),    TM("VM10"),
    TM("VM11"),    TM("VM12"),

    ball("Turboball"),      ball("Levelball"),
    ball("Koederball"),     ball("Schwerball"),
    ball("Sympaball"),      ball("Freundesball"),
    ball("Mondball"),       ball("Turnierball"),
    ball("Parkball"),

    key_item("Fotoalbum"),      key_item("GB-Player"),
    key_item("Gischtglocke"),   medicine("Wutkeks"),

    berry("Pumkinbeere"),	berry("Drashbeere"),
    berry("Eggantbeere"),	berry("Stribbeere"),
    berry("Chilianbeere"),	berry("Nutpeabeere"),
    berry("Ginemabeere"),	berry("Kuobeere"),
    berry("Yagobeere"), 	berry("Tougabeere"),
    berry("Ninikubeere"),	berry("Topobeere"),
  
    key_item("K1-Schluessel"),  key_item("K2-Schluessel"),
    key_item("K4-Schluessel"),  key_item("K6-Schluessel"),
    key_item("Meteorit"),       key_item("Wuestenglas"),
    key_item("AEon-Ticket"),    key_item("Scanner"),
    key_item("PokeRiegelBox"),  key_item("Devon-Waren"),
    key_item("Kunstrad"),       key_item("Eilrad"),
    key_item("Aschetasche"),    key_item("Wailmerkanne"),
    key_item("Sphaerensegm"),   key_item("GrueneKugel"),
    key_item("Tresorkapsel"),   key_item("RoteKugel"),
    key_item("BlaueKugel"),     key_item("Mytokristall"),

    item("Schoenschuppe"),      item("Evolith"),
    item("Leichtstein"),        item("Beulenhelm"),
    item("Luftballon"),         item("RoteKarte"),
    item("Zielscheibe"),        item("Klammerband"),
    item("Knolle"),             item("Akku"),
    item("Fluchtknopf"),   

    item("Feuerjuwel"),         item("Wasserjuwel"),
    item("Elektrojuwel"),       item("Pflanzjuwel"),
    item("Eisjuwel"),           item("Kampfjuwel"),
    item("Giftjuwel"),          item("Bodenjuwel"),
    item("Flugjuwel"),          item("Psychojuwel"),
    item("Kaeferjuwel"),        item("Gesteinjuwel"),
    item("Geistjuwel"),         item("Drakojuwel"),
    item("Unlichtjuwel"),       item("Stahljuwel"),
    item("Normaljuwel"),

    medicine("Heilfeder"),      medicine("Kraftfeder"),
    medicine("Abwehrfeder"),    medicine("Geniefeder"),
    medicine("Espritfeder"),    medicine("Flinkfeder"),
    item("Prachtfeder"),

    item("Schildfossil"),       item("Federfossil"),

    key_item("Gartenpass"),
    item("Transferorb"),        item("Traumball"),
    key_item("Deko-Box"),       key_item("Drakoschaedel"),

    item("Duftpilz"),           item("Riesennugget"),
    item("Triperle"),           item("Kometstück"),
    item("AlterHeller"),        item("AlterTaler"),
    item("AlterDukat"),         item("AlteVase"),
    item("AlterReif"),          item("AlteStatue"),
    item("AlteKrone"),          
    
    medicine("Stratos-Eis"),

    item("Null"),    item("Null"),    item("Null"),    item("Null"),
    item("Null"),    item("Null"),    item("Null"),    item("Null"),
    item("Null"),    item("Null"),    item("Null"),    item("Null"),
    item("Null"),    item("Null"),    item("Null"),    item("Null"),
    item("Null"),    item("Null"),    item("Null"),    item("Null"),
    item("Null"),    item("Null"),    item("Null"),    item("Null"),

    key_item("Lichtstein"),
    key_item("Dunkelstein"),
    TM("TM93"),
    TM("TM94"),
    TM("TM95"), 
    key_item("Viso-Caster"),
    key_item("Briefpost"),
    key_item("Briefpost"),
    key_item("Briefpost"),
    key_item("Medaillenbox"),
    key_item("DNS-Keil"),
    key_item("Genehmigung"),
    key_item("Ovalpin"),
    key_item("Schillerpin"),
    key_item("Plasmakarte"),
    key_item("Schnaeuztuch"),
    key_item("Achromat"),
    key_item("Fundsache"),
    key_item("Wahrspiegel")
};