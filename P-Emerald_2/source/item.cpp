#include <fstream>
#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sstream>

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
bool berry::_load(){
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
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
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
    if(f == 0)
        return "FEHLER";
    int ac;
    fscanf(f,"%i",&ac);
    fscanf(f,"%i\n",&ac);
    std::string s = readString(f);
    s = readString(f);
    fclose(f);
    return s;
}
std::string item::getDisplayName(){
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
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
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
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
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
    if(f == 0)
        return 0;
    int ac;
    fscanf(f,"%i",&ac);
    fscanf(f,"%i\n",&ac);
    fclose(f);
    return ac;
}

std::string berry::getDescription2(){
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
    if(f == 0)
        return "FEHLER";
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
    std::stringstream FILENAME;
    FILENAME << ITEM_PATH << this->Name << ".data";
    FILE* f = fopen(FILENAME.str().c_str(),"r");
    if(f == 0)
        return "FEHLER";
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
    battle_item("Blaue Floete"),    battle_item("Gelbe Floete"),
    battle_item("Rote Floete"),     battle_item("Schwarze Floete"),
    battle_item("Weisse Floete"),
    
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
    item("Ovaler Stein"),       item("Spiritkern"),
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
    mail("Antwortbrief"),   mail("Brueckbrief H"),
    mail("Brueckbrief M"),  mail("Brueckbrief Z"),
    mail("Brueckbrief D"),  mail("Brueckbrief W"),

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
    key_item("BlaueKugel"),     key_item("Mytokristall")
};

/*

537	 0x0219	Prism Scale	Prism Scale	Bag Items pocket icon.png	 Items pocket
538	 0x021A	Eviolite	Eviolite	Bag Items pocket icon.png	 Items pocket
539	 0x021B	Float Stone	Float Stone	Bag Items pocket icon.png	 Items pocket
540	 0x021C	Rocky Helmet	Rocky Helmet	Bag Items pocket icon.png	 Items pocket
541	 0x021D	Air Balloon	Air Balloon	Bag Items pocket icon.png	 Items pocket
542	 0x021E	Red Card	Red Card	Bag Items pocket icon.png	 Items pocket
543	 0x021F	Ring Target	Ring Target	Bag Items pocket icon.png	 Items pocket
544	 0x0220	Binding Band	Binding Band	Bag Items pocket icon.png	 Items pocket
545	 0x0221	Absorb Bulb	Absorb Bulb	Bag Items pocket icon.png	 Items pocket
546	 0x0222	Cell Battery	Cell Battery	Bag Items pocket icon.png	 Items pocket
547	 0x0223	Eject Button	Eject Button	Bag Items pocket icon.png	 Items pocket
548	 0x0224	Fire Gem	Fire Gem	Bag Items pocket icon.png	 Items pocket
549	 0x0225	Water Gem	Water Gem	Bag Items pocket icon.png	 Items pocket
550	 0x0226	Electric Gem	Electric Gem	Bag Items pocket icon.png	 Items pocket
551	 0x0227	Grass Gem	Grass Gem	Bag Items pocket icon.png	 Items pocket
552	 0x0228	Ice Gem	Ice Gem	Bag Items pocket icon.png	 Items pocket
553	 0x0229	Fighting Gem	Fighting Gem	Bag Items pocket icon.png	 Items pocket
554	 0x022A	Poison Gem	Poison Gem	Bag Items pocket icon.png	 Items pocket
555	 0x022B	Ground Gem	Ground Gem	Bag Items pocket icon.png	 Items pocket
556	 0x022C	Flying Gem	Flying Gem	Bag Items pocket icon.png	 Items pocket
557	 0x022D	Psychic Gem	Psychic Gem	Bag Items pocket icon.png	 Items pocket
558	 0x022E	Bug Gem	Bug Gem	Bag Items pocket icon.png	 Items pocket
559	 0x022F	Rock Gem	Rock Gem	Bag Items pocket icon.png	 Items pocket
560	 0x0230	Ghost Gem	Ghost Gem	Bag Items pocket icon.png	 Items pocket
561	 0x0231	Dragon Gem	Dragon Gem	Bag Items pocket icon.png	 Items pocket
562	 0x0232	Dark Gem	Dark Gem	Bag Items pocket icon.png	 Items pocket
563	 0x0233	Steel Gem	Steel Gem	Bag Items pocket icon.png	 Items pocket
564	 0x0234	Normal Gem	Normal Gem	Bag Items pocket icon.png	 Items pocket

565	 0x0235	Health Wing	Health Wing	Bag Medicine pocket icon.png	 Medicine pocket
566	 0x0236	Muscle Wing	Muscle Wing	Bag Medicine pocket icon.png	 Medicine pocket
567	 0x0237	Resist Wing	Resist Wing	Bag Medicine pocket icon.png	 Medicine pocket
568	 0x0238	Genius Wing	Genius Wing	Bag Medicine pocket icon.png	 Medicine pocket
569	 0x0239	Clever Wing	Clever Wing	Bag Medicine pocket icon.png	 Medicine pocket
570	 0x023A	Swift Wing	Swift Wing	Bag Medicine pocket icon.png	 Medicine pocket

571	 0x023B	Pretty Wing	Pretty Wing	Bag Items pocket icon.png	 Items pocket
572	 0x023C	Cover Fossil	Cover Fossil	Bag Items pocket icon.png	 Items pocket
573	 0x023D	Plume Fossil	Plume Fossil	Bag Items pocket icon.png	 Items pocket

574	 0x023E	Liberty Pass	Liberty Pass	Bag Key items pocket icon.png	 Key items pocket

575	 0x023F	Pass Orb	Pass Orb	Bag Items pocket icon.png	 Items pocket

576	 0x0240	Dream Ball	Dream Ball	Bag Poké Balls pocket icon.png	 Poké Balls pocket

577	 0x0241	Poké Toy	Poké Toy	Bag Items pocket icon.png	 Items pocket

578	 0x0242	Prop Case	Prop Case	Bag Key items pocket icon.png	 Key items pocket
579	 0x0243	Dragon Skull	Dragon Skull	Bag Key items pocket icon.png	 Key items pocket

580	 0x0244	BalmMushroom	BalmMushroom	Bag Items pocket icon.png	 Items pocket
581	 0x0245	Big Nugget	Big Nugget	Bag Items pocket icon.png	 Items pocket
582	 0x0246	Pearl String	Pearl String	Bag Items pocket icon.png	 Items pocket
583	 0x0247	Comet Shard	Comet Shard	Bag Items pocket icon.png	 Items pocket
584	 0x0248	Relic Copper	Relic Copper	Bag Items pocket icon.png	 Items pocket
585	 0x0249	Relic Silver	Relic Silver	Bag Items pocket icon.png	 Items pocket
586	 0x024A	Relic Gold	Relic Gold	Bag Items pocket icon.png	 Items pocket
587	 0x024B	Relic Vase	Relic Vase	Bag Items pocket icon.png	 Items pocket
588	 0x024C	Relic Band	Relic Band	Bag Items pocket icon.png	 Items pocket
589	 0x024D	Relic Statue	Relic Statue	Bag Items pocket icon.png	 Items pocket
590	 0x024E	Relic Crown	Relic Crown	Bag Items pocket icon.png	 Items pocket

591	 0x024F	Casteliacone	Casteliacone	Bag Medicine pocket icon.png	 Medicine pocket

592	 0x0250	Dire Hit 2	Dire Hit 2	Bag Unknown pocket icon.png	 Unknown pocket
593	 0x0251	X Speed 2	X Speed 2	Bag Unknown pocket icon.png	 Unknown pocket
594	 0x0252	X Special 2	X Special 2	Bag Unknown pocket icon.png	 Unknown pocket
595	 0x0253	X Sp. Def 2	X Sp. Def 2	Bag Unknown pocket icon.png	 Unknown pocket
596	 0x0254	X Defend 2	X Defend 2	Bag Unknown pocket icon.png	 Unknown pocket
597	 0x0255	X Attack 2	X Attack 2	Bag Unknown pocket icon.png	 Unknown pocket
598	 0x0256	X Accuracy 2	X Accuracy 2	Bag Unknown pocket icon.png	 Unknown pocket
599	 0x0257	X Speed 3	X Speed 3	Bag Unknown pocket icon.png	 Unknown pocket
600	 0x0258	X Special 3	X Special 3	Bag Unknown pocket icon.png	 Unknown pocket
601	 0x0259	X Sp. Def 3	X Sp. Def 3	Bag Unknown pocket icon.png	 Unknown pocket
602	 0x025A	X Defend 3	X Defend 3	Bag Unknown pocket icon.png	 Unknown pocket
603	 0x025B	X Attack 3	X Attack 3	Bag Unknown pocket icon.png	 Unknown pocket
604	 0x025C	X Accuracy 3	X Accuracy 3	Bag Unknown pocket icon.png	 Unknown pocket
605	 0x025D	X Speed 6	X Speed 6	Bag Unknown pocket icon.png	 Unknown pocket
606	 0x025E	X Special 6	X Special 6	Bag Unknown pocket icon.png	 Unknown pocket
607	 0x025F	X Sp. Def 6	X Sp. Def 6	Bag Unknown pocket icon.png	 Unknown pocket
608	 0x0260	X Defend 6	X Defend 6	Bag Unknown pocket icon.png	 Unknown pocket
609	 0x0261	X Attack 6	X Attack 6	Bag Unknown pocket icon.png	 Unknown pocket
610	 0x0262	X Accuracy 6	X Accuracy 6	Bag Unknown pocket icon.png	 Unknown pocket
611	 0x0263	Ability Urge	Ability Urge	Bag Unknown pocket icon.png	 Unknown pocket
612	 0x0264	Item Drop	Item Drop	Bag Unknown pocket icon.png	 Unknown pocket
613	 0x0265	Item Urge	Item Urge	Bag Unknown pocket icon.png	 Unknown pocket
614	 0x0266	Reset Urge	Reset Urge	Bag Unknown pocket icon.png	 Unknown pocket
615	 0x0267	Dire Hit 3	Dire Hit 3	Bag Unknown pocket icon.png	 Unknown pocket

616	 0x0268	Light Stone	Light Stone	Bag Key items pocket icon.png	 Key items pocket
617	 0x0269	Dark Stone	Dark Stone	Bag Key items pocket icon.png	 Key items pocket
618	 0x026A	TM Electric	TM93	Bag TMs and HMs pocket icon.png	 TMs and HMs pocket
619	 0x026B	TM Fighting	TM94	Bag TMs and HMs pocket icon.png	 TMs and HMs pocket
620	 0x026C	TM Dark	TM95	Bag TMs and HMs pocket icon.png	 TMs and HMs pocket
621	 0x026D	Xtransceiver-m	Xtransceiver	Bag Key items pocket icon.png	 Key items pocket
622	 0x026E	God Stone	God Stone	Bag Key items pocket icon.png	 Key items pocket
623	 0x026F	Gram	Gram 1	Bag Key items pocket icon.png	 Key items pocket
624	 0x0270	Gram	Gram 2	Bag Key items pocket icon.png	 Key items pocket
625	 0x0271	Gram	Gram 3	Bag Key items pocket icon.png	 Key items pocket
626	 0x0272	Xtransceiver-f	Xtransceiver	Bag Key items pocket icon.png	 Key items pocket
627	 0x0273	Medal Box	Medal Box*	Bag Key items pocket icon.png	 Key items pocket
628	 0x0274	DNA Splicers	DNA Splicers*	Bag Key items pocket icon.png	 Key items pocket
629	 0x0275	DNA Splicers	DNA Splicers*	Bag Key items pocket icon.png	 Key items pocket
630	 0x0276	Permit	Permit*	Bag Key items pocket icon.png	 Key items pocket
631	 0x0277	Oval Charm	Oval Charm*	Bag Key items pocket icon.png	 Key items pocket
632	 0x0278	Shiny Charm	Shiny Charm*	Bag Key items pocket icon.png	 Key items pocket
633	 0x0279	Plasma Card	Plasma Card*	Bag Key items pocket icon.png	 Key items pocket
634	 0x027A	Grubby Hanky	Grubby Hanky*	Bag Key items pocket icon.png	 Key items pocket
635	 0x027B	Colress MCHN	Colress MCHN*	Bag Key items pocket icon.png	 Key items pocket
636	 0x027C	Dropped Item-m	Dropped Item*	Bag Key items pocket icon.png	 Key items pocket
637	 0x027D	Dropped Item-f	Dropped Item*	Bag Key items pocket icon.png	 Key items pocket
638	 0x027E	Reveal Glass	Reveal Glass*	Bag Key items pocket icon.png	 Key items pocket
*/