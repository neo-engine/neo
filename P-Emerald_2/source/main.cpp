#include <nds.h>
#include <fat.h>
#include "nitrofs.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <fcntl.h>
#include <unistd.h>

//#include <n3d.h>
//#include <n3d\nds\arm9\n3ddevice.h>
//#include "n3dsampleframework.h"
//#include "submesh0_n3dmesh_bin.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "mbox.h"
#include "item.h"
#include "berry.h"
#include "font.h"
#include "print.h"
#include "font_data.h"
#include "battle.h"

#include "scrnloader.h"
#include "PKMN.h"
#include "savgm.h"
#include "Map.h"
#include "keyboard.h"
#include "sprite.h"

#include "Brother.h"
#include "Male.h"
#include "Female.h"

OAMTable *oam = new OAMTable();
SpriteInfo spriteInfo[SPRITE_COUNT];

OAMTable *oamTop = new OAMTable();
SpriteInfo spriteInfoTop[SPRITE_COUNT];

//Centers o t circles.
//PKMN -> ID -> DEX -> Bag -> Opt -> Nav
// X|Y
int mainSpritesPositions[6][2] 
= {{130,60},{160,80},{160,115},{130,135},{100,115},{100,80}};

//Map* acMap;

//extern N3DDEVICE g_device;

//void SetupMatrices();
//void RenderScene(const N3DFLOAT _timeStep);
//void SetupMatrices()
//{   
//    // Build the projection matrix
//    N3DMATRIX projMatrix;
//
//    N3DMatrixPerspectiveFovLH(projMatrix, N3DFLOAT(N3DPI / 4.0f), N3DFLOAT(4.0f / 3.0f), N3DFLOAT(1.0f), N3DFLOAT(100.0f));
//    g_device.SetTransform(N3DTS_PROJECTION, projMatrix);
//
//
//    // Build the view matrix
//    N3DMATRIX   viewMatrix;
//    N3DVECTOR3  eye(N3DFLOAT(0), N3DFLOAT(0), N3DFLOAT(0));
//    N3DVECTOR3  at(N3DFLOAT(1), N3DFLOAT(1.75), N3DFLOAT(4));
//    N3DVECTOR3  up(N3DFLOAT(0), N3DFLOAT(1), N3DFLOAT(0));
//
//    N3DMatrixLookAtLH(viewMatrix, eye, at, up);
//    g_device.SetTransform(N3DTS_VIEW, viewMatrix);
//}

enum GameMod{
    DEVELOPER,
    ALPHA,
    BETA,
    RELEASE,
    EMULATOR
} gMod = DEVELOPER;
std::string CodeName = "Raging Gyarados";

int bg3sub;
int bg2sub;
int bg3;
int bg2;

extern PrintConsole Top,Bottom;
ConsoleFont cfont;
Keyboard* kbd;
font::Font cust_font(font::font_data, font::font_widths);

//extern Map Maps[];

int hours, seconds, minutes, day, month, year;
int achours, acseconds, acminutes, acday, acmonth, acyear;
unsigned int ticks;

savgm SAV;
const std::string sav_nam = "nitro:/SAV";
extern std::map<int,std::string> Locations;
Region acRegion = HOENN;
extern Region acMapRegion;
extern bool showmappointer;
extern void printMapLocation(const touchPosition& t);

scrnloader scrn(-2);	

extern POKEMON::PKMN::BOX_PKMN stored_pkmn[MAXSTOREDPKMN];
extern std::vector<int> box_of_st_pkmn[MAXPKMN];
extern std::vector<int> free_spaces;

void updateTime();

void whoCares(int){	return; }
void progress(int j)
{
    consoleSelect(&Bottom);
    consoleSetWindow(&Bottom,0,0,10,1);
    printf("%3i%%",j);
}

enum ChoiceResult{
    CONTINUE,
    NEW_GAME,
    OPTIONS,
    GEHEIMGESCHEHEN,
    TRANSFER_GAME,
    CANCEL
};
namespace POKEMON{ extern char* getLoc(int ind); }

void fillWeiter()
{
    consoleSetWindow(&Bottom, 1,1,33,22);
    consoleSelect(&Bottom);
    BG_PALETTE_SUB[3] = RGB15(0,0,0);

    std::wstring P(SAV.getName());
    P.insert(P.begin(),22-P.size(),' ');
    P = L"        " + P+L"\n";
    wprintf(P.c_str());	

    std::string W(POKEMON::getLoc(SAV.Loc[0]));
    W.insert(W.begin(),30-W.size(),' ');
    printf(&W[0]);
    printf("\n");

    std::wstringstream ss;
    ss << SAV.PLAYTIME / 60<<":"<<((SAV.PLAYTIME-(SAV.PLAYTIME / 60 * 60)) / 10)<< ((SAV.PLAYTIME-(SAV.PLAYTIME / 60 * 60)) % 10);
    P = ss.str();
    P.insert(P.begin(),20-P.size(),' ');
    P = L"SPIELZEIT "+P;
    wprintf(P.c_str());	
    printf("\n");
    if(SAV.Orden/10!=0)
    printf("ORDEN                       %i""%i\n",SAV.Orden/10,SAV.Orden%10);	
    else
    printf("ORDEN                        %i\n",SAV.Orden%10);	
    printf("POK\x82""DEX                    %3i",SAV.Dex);
}
void killWeiter()
{
    consoleSetWindow(&Bottom, 1,1,30,22);
    consoleSelect(&Bottom);
    consoleClear();
}

ChoiceResult opScreen(int type)
{
    consoleSelect(&Top);
    consoleClear();

    ChoiceResult results[5] = {
        CONTINUE,
        NEW_GAME,
        GEHEIMGESCHEHEN,
        OPTIONS,
        TRANSFER_GAME,
    };
    int MaxVal;
    std::pair<int,int> ranges[5] = {
        std::pair<int,int>(0,80),
        std::pair<int,int>(85,105),
        std::pair<int,int>(112,132),
        std::pair<int,int>(165,185)
    };

    switch (type)
    {
    case 3:
        {
            loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","MainMenu0");
            MaxVal = 5;
            fillWeiter();
            break;
        }
    case 2:
        {
            loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","MainMenu1");
            MaxVal = 4;
            fillWeiter();
            break;
        }
    case 1:
        {
            loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","MainMenu2");
            MaxVal = 3;
            results[2] = OPTIONS;
            
            fillWeiter();
            break;
        }
    case 0:
        {
            loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","MainMenu3");

            MaxVal = 2;
            ranges[0]= std::pair<int,int>(0,20);
            ranges[1]= std::pair<int,int>(25,40);
            results[0] = NEW_GAME;
            results[1] = OPTIONS;

            break;
        }
    default:
        {
            killWeiter();
            return CANCEL;
        }
    }
    
    touchPosition touch;
    while(1)
    {
        scanKeys();
        touchRead(&touch);
        int p = keysUp();
        int k = keysHeld();
        if ((SAV.SavTyp == 1) && (k & KEY_SELECT) && (k & KEY_RIGHT) && (k & KEY_L) && (k & KEY_R))
        {			
            killWeiter();
            consoleClear();
            return opScreen(++SAV.SavTyp);
        }
        else if ((gMod==DEVELOPER)&&(SAV.SavTyp == 2) && (k & KEY_START) && (k & KEY_LEFT) && (k & KEY_L) && (k & KEY_R))
        {			
            killWeiter();
            consoleClear();
            return opScreen(++SAV.SavTyp);
        }
        else if (p & KEY_B)
        {
            killWeiter();
            consoleClear();
            loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");		
            for(int i = 1;i<256;++i)
            BG_PALETTE_SUB[i] = RGB15(31,31,31);
            return CANCEL;
        }
        for (int i = 0; i < MaxVal; i++)
        if(touch.py > ranges[i].first && touch.py < ranges[i].second)
        {
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            killWeiter();
            loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");	
            for(int j = 1;j<256;++j)
            BG_PALETTE_SUB[j] = RGB15(31,31,31);
            
            return results[i];
        }
    }
}

void printText(const char* text){    
    int indx = 0;
    while(text[indx] != '\0'){/*
        if(text[indx] == ' ' || text[indx] == '\n')
            for(int i= 0; i < 120/TEXTSPEED; ++i)
                swiWaitForVBlank();*/
        for(int i= 0; i < 80/TEXTSPEED; ++i)
        swiWaitForVBlank();
        printf("%c",text[indx++]);
    }
    while(1)
    {
        scanKeys();
        if(keysUp() & KEY_TOUCH) break;
        if(keysUp() & KEY_A) break;
    }
}

void initNewGame()
{
    SAV = savgm();
    SAV.activatedPNav = false;
    SAV.Money = 3000;
    SAV.ID = rand() % 65536;
    SAV.SID = rand() % 65536;

    SAV.SavTyp = 1;		
    SAV.PLAYTIME = 0;
    SAV.HOENN_Badges=0;
    SAV.KANTO_Badges=0;
    SAV.JOHTO_Badges=0;
    SAV.Orden=0;
    SAV.Dex=0;
    
    SAV.PKMN_team.clear();
    consoleSelect(&Bottom);
    consoleClear();
    consoleSetWindow(&Bottom,3,10,26,5);

    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","ClearD");
    for(int i = 1;i<256;++i)
    BG_PALETTE_SUB[i] = RGB15(31,31,31);
    
    printText("Haaaaalt!\n");
    printText("Hier lang.");
    consoleClear();	
    free_spaces.clear();
    for (int i = 0; i < MAXPKMN; i++){
        SAV.inDex[i] = false;
        box_of_st_pkmn[i].clear();
        free_spaces.push_back(i);
    }
    for(int i = 0; i < MAXSTOREDPKMN; ++i){
        //stored_pkmn[i] = 0;
        
        free_spaces.push_back(i);
    }

    /*dmaCopy( NewGameBitmap, bgGetGfxPtr(bg3), 256*256 );
    dmaCopy( NewGamePal,BG_PALETTE, 256*2); 
    
    while(1)
    {
        scanKeys();
        if(keysUp() & KEY_TOUCH)
            break;
        if(keysUp() & KEY_A)
            break;
    }*/
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");
    printf("\x1b[37m");
    printText("Hi, ich bin Maike, die\n""Tochter von Prof. Birk.");
    consoleClear();
    
    printText("Da er gerade leider nicht in Hoenn ist, werde ich\n""euch heute euren Pok\x82""Nav  und euren Pok\x82""Dex\n\x81""berreichen.");
    consoleClear();
    printText("So hier ist zun\x84""chst\n""der Pok\x82""Nav!");
    consoleClear();
    printText("Ich gehe dann jetzt mal\ndie Dexe holen.\nIhr k\x94""nnt solange eure\nPok\x82""Nav einrichten");	
    consoleClear();
    consoleSelect(&Bottom);
    printf("\x1b[39m");
    //BG_PALETTE_SUB[255] = RGB15(0,0,0);
    std::wstring S_ = L"Du erh\x84""lst einen Pok\x82""Nav.";
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","PokeNav");
    scrn.init();
    setMainSpriteVisibility(true);
    oam->oamBuffer[1].isHidden = true;
    updateOAMSub(oam);
    mbox M(S_.c_str());
    M.clear();
    consoleClear();
    
    for(int i= 9;i <= 12; ++i) {
        oam->oamBuffer[i].isHidden = false;
        updateOAMSub(oam);
    }
    M.put("Beginne automatische\nInitialisierung.",false);
    for(int i = 0; i < 120;++i)
    swiWaitForVBlank();
    M.put("\nSetze Heimatregion: Hoenn.",false);
    for(int i = 0; i < 120;++i)
    swiWaitForVBlank();
    M.put("\nSetze Heimatstadt: Klippdelta City.",false);
    for(int i = 0; i < 120;++i)
    swiWaitForVBlank();
    std::stringstream s2,s3;
    s2<<SAV.ID;
    std::string B = s2.str();
    s3<<SAV.SID;
    std::string C = s3.str();
    B.insert(B.begin(),5-B.length(),'0');
    C.insert(C.begin(),5-C.length(),'0');

    B = "\nSetze ID: " + B;
    M.put(&(B[0]),false);
    for(int i = 0; i < 120;++i)
    swiWaitForVBlank();
    
    C = "\nSetze SID: " + C;
    M.put(&(C[0]),false);
    for(int i = 0; i < 120;++i)
    swiWaitForVBlank();
    consoleClear();
    M.clear();
    M = mbox("Automatische Initialisierung\nabgeschlossen.");
    consoleClear();
    M.clear();
INDIVIDUALISIERUNG:
    M = mbox("Beginne Individualisierung.");
    ynbox yn  = ynbox(M);
    SAV.IsMale = !yn.getResult("Bist du ein M\x84""dchen?");
    consoleClear();
    M.clear();
    keyboard K = keyboard();
    std::wstring Name = K.getText(10,"Gib deinen Namen an!");
    if(Name.empty())
    if(SAV.IsMale)
    SAV.setName(L"Basti");
    else 
    SAV.setName(L"Lari");
    else
    SAV.setName(Name);
    ynbox YN = ynbox();
    std::wstring S = L"Du bist also ";
    if(SAV.IsMale)
    S += L"der\n";
    else
    S += L"die\n";
    S += SAV.getName(); S +=L"?";
    if(!YN.getResult(&(S[0])))
    goto INDIVIDUALISIERUNG;
    
    consoleClear();
    M.clear();
    M = mbox("Individualisierung\nabgeschlossen!");
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","ClearD");
    if(!BGs[BG_ind].load_from_rom){
        dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
    }
    else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
        dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
        BG_ind = 0;
    }
    for (int i = 0; i < 30; i++)
    swiWaitForVBlank();
    dmaCopy( BrotherBitmap, bgGetGfxPtr(bg3), 256*256 );
    dmaCopy( BrotherPal,BG_PALETTE, 256*2); 
    if(SAV.IsMale)
    {
        M = mbox("Mir will sie den Dex\n""zuerst geben!","???",true,true,true,mbox::sprite_pkmn);
        consoleClear();
        dmaCopy( FemaleBitmap, bgGetGfxPtr(bg3), 256*256 );
        dmaCopy( FemalePal,BG_PALETTE, 256*2); 
        M = mbox("Hey, jetzt spiel dich\nnicht so auf.\n","???",true,true,false,mbox::sprite_pkmn);	
        consoleClear();
        M = mbox("Ja, so ist er, mein\nBruder halt...\n"" Nich, Moritz?","???",true,true,true,mbox::sprite_pkmn);
        consoleClear();		
        dmaCopy( BrotherBitmap, bgGetGfxPtr(bg3), 256*256 );
        dmaCopy( BrotherPal,BG_PALETTE, 256*2); 
        M = mbox("Du bist doch\nnur neidisch!","Moritz",true,true,true,mbox::sprite_trainer,0);
        consoleClear();
        dmaCopy( FemaleBitmap, bgGetGfxPtr(bg3), 256*256 );
        dmaCopy( FemalePal,BG_PALETTE, 256*2); 
        M = mbox("Tse...\n","???",true,true,true,mbox::sprite_pkmn);
        consoleClear();
        M = mbox("Hi, ich bin Larissa,\n""aber Lari reicht auch.","Lari",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Das da ist mein\nkleiner Bruder Moritz.","Lari",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Wir kommen aus Azuria","Lari",true,true,true,mbox::sprite_trainer,0);
        loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
        for (int k = 0; k < 30; k++)
        swiWaitForVBlank();
        M = mbox("Das hei\x9D""t eigentlich.","Lari",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Als alle Kanto ver-\nlassen haben, sind wir""nach Klippdelta\ngezogen.","Lari",true,true,true,mbox::sprite_trainer,0);
        consoleClear();
        loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
        for (int k = 0; k < 30; k++)
        swiWaitForVBlank();
        M = mbox("Du kommst auch aus\nKlippdelta?!","Lari",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Oh...\n","Lari",mbox::sprite_trainer,0);
        loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
        for (int k = 0; k < 30; k++)
        swiWaitForVBlank();
        M = mbox("Na dann sehen wir uns ja wahrscheinlich\nnoch \x94""fter...","Lari",true,true,true,mbox::sprite_trainer,0);
        consoleClear();
        loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
    }
    else
    {
        M = mbox("Mir wird sie den Dex\nzuerst geben!","???",true,true,false,mbox::sprite_pkmn,0);
        consoleClear();
        dmaCopy( MaleBitmap, bgGetGfxPtr(bg3), 256*256 );
        dmaCopy( MalePal,BG_PALETTE, 256*2); 
        M = mbox("Und weiter?","???",true,true,false,mbox::sprite_pkmn,0);
        consoleClear();
        dmaCopy( BrotherBitmap, bgGetGfxPtr(bg3), 256*256 );
        dmaCopy( BrotherPal,BG_PALETTE, 256*2); 
        M = mbox("Bist doch\nnur neidisch!","???",true,true,false,mbox::sprite_pkmn,0);
        consoleClear();
        dmaCopy( MaleBitmap, bgGetGfxPtr(bg3), 256*256 );
        dmaCopy( MalePal,BG_PALETTE, 256*2); 
        M = mbox("Wozu...\n","???",true,true,false,mbox::sprite_pkmn,0);
        consoleClear();
        M = mbox("Hi, ich bin Sebastian.","???",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Aber nenn' mich ruhig Basti.","Basti",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Das da ist mein\n""kleiner Bruder Moritz.","Basti",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Wir kommen aus Azuria.","Basti",true,true,true,mbox::sprite_trainer,0);
        loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
        for (int k = 0; k < 30; k++)
        swiWaitForVBlank();
        M = mbox("Das hei\x9D""t eigentlich.","Basti",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Als alle Kanto ver-\nlassen haben, sind wir""nach Klippdelta\ngezogen.","Basti",true,true,true,mbox::sprite_trainer,0);
        consoleClear();
        M = mbox("Du lebst auch in\nKlippdelta, nich? ","Basti",true,true,false,mbox::sprite_trainer,0);	
        consoleClear();
        M = mbox("Als erstes werde ich\nein Trasla fangen.\nDann habe ich bald einGuardevoir.","Basti",true,true,false,mbox::sprite_trainer,0);
        M = mbox("Dann k\x94""nnen wir ja malk\x84""mpfen, ich meine,\ndu wohnst ja nur ein\nHaus weiter.","Basti",true,true,true,mbox::sprite_trainer,0);
        consoleClear();
    }	
    for (int k = 0; k < 45; k++)
    swiWaitForVBlank();
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");
    mbox("So, hier habt ihr das Pok\x82""Dex-Modul f\x81""r den Pok\x82""Nav.","Maike",true,true,false,mbox::sprite_trainer,0);
    S_ = L"Einmal f\x81""r " +SAV.getName();
    mbox(S_.c_str(),L"Maike",true,true,false,mbox::sprite_trainer,0);	
    mbox("Einmal f\x81""r Moritz.","Maike",true,true,false,mbox::sprite_trainer,0);
    if(SAV.IsMale)
    mbox("Und einmal f\x81""r Lari.","Maike",true,true,true,mbox::sprite_trainer,0);
    else
    mbox("Und einmal f\x81""r Basti.","Maike",true,true,true,mbox::sprite_trainer,0);
    consoleClear();
    loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
    S_ = SAV.getName(); S_ += L" erh\x84""lt\ndas Pok\x82""Dex-Modul.";
    M = mbox(S_.c_str());
    
    
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");
    mbox("Der Pok\x82""Nav ist in derLage, euch s\x84""mtliche\nDaten \x81""ber eure\nPok\x82""mon zu liefern.","Maike",true,true,false,mbox::sprite_trainer,0);
    mbox("Auch kann man mit ihm andere Trainer anrufenund er besitzt eine\nKartenfunktion.","Maike",true,true,false,mbox::sprite_trainer,0);
    dmaCopy( BrotherBitmap, bgGetGfxPtr(bg3), 256*256 );
    dmaCopy( BrotherPal,BG_PALETTE, 256*2); 
    mbox("Und er organisiert\neinem auch so den\nBeutel, dass er\noptimal gef\x81""llt ist!","Moritz",true,true,false,mbox::sprite_trainer,0);
    mbox("Moritz!",SAV.getName().c_str());
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");
    mbox("...","Maike",true,false,false,mbox::sprite_trainer,0);
    mbox("Also, vergesst ihn ja nicht bei euch zu\nHause, wenn ihr auf\nReisen geht!","Maike",true,true,false,mbox::sprite_trainer,0);
    mbox("Also dann, bis bald!","Maike",true,true,true,mbox::sprite_trainer,0);
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","Clear");
    
    S_ = SAV.getName(); S_ += L",\n wir seh'n uns noch!";
    M = mbox(&(S_[0]),L"Moritz",true,true,true,mbox::sprite_trainer,0);	
    consoleSelect(&Top);
    SAV.hasPKMN = false;
    
    setMainSpriteVisibility(false);
    oam->oamBuffer[1].isHidden = false;
    updateOAMSub(oam);   
    loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind);
}

void initVideo() {
    
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000);

    vramSetBankE(VRAM_E_MAIN_SPRITE);

    /*  Set the video mode on the main screen. */
    videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
    DISPLAY_BG2_ACTIVE | // Enable BG2 for display
    DISPLAY_BG3_ACTIVE | // Enable BG3 for display
    DISPLAY_SPR_ACTIVE | // Enable sprites for display
    DISPLAY_SPR_1D       // Enable 1D tiled sprites
    );
}
void initVideoSub() {

    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    vramSetBankD(VRAM_D_SUB_SPRITE);


    /*  Set the video mode on the main screen. */
    videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
    DISPLAY_BG2_ACTIVE | // Enable BG2 for display
    DISPLAY_BG3_ACTIVE | // Enable BG3 for display
    DISPLAY_SPR_ACTIVE | // Enable sprites for display
    DISPLAY_SPR_1D       // Enable 1D tiled sprites
    );
}
void vramSetup()
{
    initVideo();
    initVideoSub();
    VRAM_F_CR = VRAM_ENABLE | VRAM_F_BG_EXT_PALETTE | VRAM_OFFSET(1);
    vramSetBankG(VRAM_G_LCD);
    vramSetBankH(VRAM_H_LCD);
}

int main(int argc, char** argv) 
{
    //Init
    powerOn(POWER_ALL_2D);
    
    fatInitDefault();
    nitroFSInit(argv[0]);

    //PRE-Intro
    touchPosition touch;
    
    vramSetup();

    videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D);

    // set up our top bitmap background
    bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 1 , 0 );
    //bgSet( bg3, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( bg3, 3 );
    bgUpdate();
    
    // set up our bottom bitmap background
    bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 1 , 0 );
    // bgSet( bg3sub, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( bg3sub, 2 );
    bgUpdate();
    //// set up our bottom bitmap background
    //bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 2 , 0 );
    //// bgSet( bg2sub, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    //bgSetPriority( bg2sub, 3 );
    //bgUpdate();

    Top = *consoleInit(0, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true ,true);
    Bottom = *consoleInit(0,  0, BgType_Text4bpp, BgSize_T_256x256, 2,0, false,true );
    
    cfont.gfx = (u16*)fontTiles;
    cfont.pal = (u16*)fontPal;
    cfont.numChars = 218;
    cfont.numColors =  16;
    cfont.bpp = 8;
    cfont.asciiOffset = 32;
    cfont.convertSingleColor = false;
    
    consoleSetFont(&Top, &cfont);
    consoleSetFont(&Bottom, &cfont);
    
    //ReadSavegame
    if(gMod != EMULATOR){
        SAV = savgm(whoCares);
        SAV.hasPKMN &= SAV.good;
    }
    else{
        SAV.hasPKMN = false;
        SAV.SavTyp = 0;
    }
START:
    //Intro
    //StartScreen
    
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","Title");
    loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","Clear");
    
    consoleSetWindow(&Bottom, 0,0,32,24);
    consoleSelect(&Bottom);

    BG_PALETTE[3] = BG_PALETTE_SUB[3] = RGB15(0,0,0);

    printf("@ RedArceus 2012 - 2013\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    if(gMod == DEVELOPER)
    printf("             Developer's version\n");
    else if(gMod == BETA)
    printf("                            Beta\n");
    else if(gMod == ALPHA)
    printf("                           Alpha\n");
    else if(gMod == EMULATOR)
    printf("                        Emulator\n");
    if(gMod != RELEASE)
    {
        std::string s = "\""+CodeName+"\"";
        s.insert(s.begin(),32-s.length(),' ');
        printf(&(s[0]));
    }

    consoleSetWindow(&Top, 0,23,32,1);
    consoleSelect(&Top);
    int D0000 = 0;
    while(1)
    {
        scanKeys();
        swiWaitForVBlank();
        int pressed = keysUp();
        if ((pressed & KEY_A)||(pressed & KEY_START||(pressed & KEY_TOUCH)) )
        break;
        ++D0000;
        if (!(D0000 % 120))
        {
            printf("     BER\x9A""HRE, UM ZU STARTEN");
            D0000 = 0;
        }
        else if ((D0000 % 120) == 60)
        consoleClear();
    }

    consoleSetWindow(&Bottom, 0,0,32,24);
    consoleSelect(&Bottom);
    consoleClear();
    consoleSelect(&Top);

    time_t uTime = time(NULL);
    tm* tStruct = gmtime((const time_t *)&uTime);

    hours = tStruct->tm_hour;
    minutes = tStruct->tm_min;
    seconds = tStruct->tm_sec;
    day = tStruct->tm_mday;
    month = tStruct->tm_mon+1;
    year = tStruct->tm_year +1900;

    ticks = 0;
    timerStart(0, ClockDivider_1024, 0, NULL);
    ticks += timerElapsed(0);

    srand(hours+(100*minutes)+(10000*seconds)/(day+(100*month)+year));
    POKEMON::LastPID = rand();

    //StartMenu
    switch(opScreen(SAV.SavTyp))
    {
    case CANCEL:
        //printf("%i",SAV.SavTyp);
        //while(1);
        goto START;
    case GEHEIMGESCHEHEN:
    case TRANSFER_GAME:

        //goto START;
    case CONTINUE:
        scrn.init();
        break;
    case OPTIONS:
        scrn.init();
        SAV = savgm();
        SAV.activatedPNav = false;
        SAV.Money = 3000;
        SAV.ID = rand() % 65536;
        SAV.SID = rand() % 65536;

        SAV.SavTyp = 1;		
        SAV.PLAYTIME = 0;
        SAV.HOENN_Badges=0;
        SAV.KANTO_Badges=0;
        SAV.JOHTO_Badges=0;
        SAV.Orden=0;
        SAV.Dex=0;
        
        SAV.PKMN_team.clear();
        free_spaces.clear();
        for (int i = 0; i < MAXPKMN; i++){
            SAV.inDex[i] = false;
            box_of_st_pkmn[i].clear();
            free_spaces.push_back(i);
        }
        for(int i = 0; i < MAXSTOREDPKMN; ++i){
            //stored_pkmn[i] = 0;
            free_spaces.push_back(i);
        }
        break;
    case NEW_GAME:
        initNewGame();
    }
    swiWaitForVBlank();
    int mode = -1;
    scrn.draw(mode);

    powerOn(POWER_ALL);
    videoSetMode(MODE_0_3D | DISPLAY_BG0_ACTIVE);
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankF(VRAM_F_TEX_PALETTE);
    
    //N3DSampleFrameworkInit();

    //g_device.Init();
    //SetupMatrices();
    
    cust_font.set_color(RGB(0,31,31),0);

    std::pair<int,int> dirs[4] = {std::pair<int,int>(0,1),std::pair<int,int>(1,0),std::pair<int,int>(0,-1),std::pair<int,int>(-1,0)};
    int acDir = 0;
    //acMap = Maps;
    
    int HILFSCOUNTER = 251;
    //N3DMATRIX ro;
    //N3DMatrixRotationY(ro, DegreeToRadian(0));
    //g_device.SetTransform(N3DTS_VIEW,ro);
    oam->oamBuffer[PKMN_ID].isHidden = !(SAV.hasPKMN && SAV.PKMN_team.size());
    updateOAMSub(oam);

    SAV.hasGDex = true;

    while(42) 
    {
        updateTime();
        swiWaitForVBlank();
        updateOAMSub(oam);
        scanKeys();
        touchRead(&touch);
        int pressed = keysUp(),held = keysHeld();
        
        //Moving
        /*if(held & KEY_R){
            if (pressed & KEY_RIGHT)
            {
                N3DMATRIX rot;
                acMap->rotation = acMap->rotation+90 == 360 ? 0:(acMap->rotation+90);
                N3DMatrixRotationY(rot, DegreeToRadian(acMap->rotation));
                g_device.SetTransform(N3DTS_VIEW,rot);
                acMap->render();
                //acMap->setRot(New);			
                N3DSampleFrameworkWaitForVBlank();
            }
            else if (pressed & KEY_LEFT)
            {				
                N3DMATRIX rot;
                acMap->rotation = acMap->rotation-90 == -90 ? 270:(acMap->rotation-90);
                N3DMatrixRotationY(rot, DegreeToRadian(acMap->rotation));
                g_device.SetTransform(N3DTS_VIEW,rot);
                acMap->render();
                //acMap->setRot(New);			
                N3DSampleFrameworkWaitForVBlank();
            }
            continue;
        }
        else if(held & KEY_DOWN)
        {
            int newX = acMap->acPos.x + dirs[(acDir)%4].first;
            int newY = acMap->acPos.y + dirs[(acDir)%4].second;
            int newZ = acMap->moveData[((acMap->acPos.x/10)+(acMap->SX/2)) * acMap->SY + (acMap->acPos.y/10)] == 0 ? 
                (acMap->moveData[((newX/10)+(acMap->SX/2)) * acMap->SY + (newY/10)]-2)/8.0 : acMap->acPos.z;
            Point newPos = Point(newX,newY,newZ);
            acMap->setPos(newPos);
            N3DSampleFrameworkWaitForVBlank();
            continue;
        }
        else if (held & KEY_LEFT)
        {
            int newX = acMap->acPos.x + dirs[(acDir+1)%4].first;
            int newY = acMap->acPos.y + dirs[(acDir+1)%4].second;
            int newZ = acMap->moveData[((acMap->acPos.x/10)+(acMap->SX/2)) * acMap->SY + (acMap->acPos.y/10)] == 0 ? 
                (acMap->moveData[((newX/10)+(acMap->SX/2)) * acMap->SY + (newY/10)]-2)/8.0 : acMap->acPos.z;
            Point newPos = Point(newX,newY,newZ);
            acMap->setPos(newPos);
            N3DSampleFrameworkWaitForVBlank();
            continue;
        }
        else if (held & KEY_RIGHT)
        {
            int newX = acMap->acPos.x + dirs[(acDir+3)%4].first;
            
            int newY = acMap->acPos.y + dirs[(acDir+3)%4].second;
            int newZ = acMap->moveData[((acMap->acPos.x/10)+(acMap->SX/2)) * acMap->SY + (acMap->acPos.y/10)] == 0 ? 
                (acMap->moveData[((newX/10)+(acMap->SX/2)) * acMap->SY + (newY/10)]-2)/8.0 : acMap->acPos.z;
            Point newPos = Point(newX,newY,newZ);
            acMap->setPos(newPos);
            N3DSampleFrameworkWaitForVBlank();
            continue;
        }
        else if (held & KEY_UP)
        {
            int newX = acMap->acPos.x + dirs[(acDir+2)%4].first;
            int newY = acMap->acPos.y + dirs[(acDir+2)%4].second;
            int newZ = acMap->moveData[((acMap->acPos.x/10)+(acMap->SX/2)) * acMap->SY + (acMap->acPos.y/10)] == 0 ? 
                (acMap->moveData[((newX/10)+(acMap->SX/2)) * acMap->SY + (newY/10)]-2)/8.0 : acMap->acPos.z;
            Point newPos = Point(newX,newY,newZ);
            acMap->setPos(newPos);
            N3DSampleFrameworkWaitForVBlank();
            continue;
        }*/
        //StartBag
        //Centers o t circles.
        //PKMN -> ID -> DEX -> Bag -> Opt -> Nav
        // X|Y
        //int mainSpritesPositions[6][2] 
        //= {{130,60},{160,80},{160,115},{130,135},{100,115},{100,80}};
        if (sqrt(sq(mainSpritesPositions[3][0]-touch.px) + sq(mainSpritesPositions[3][1]-touch.py)) <= 16 && mode == -1)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            //scrn.run_bag
            //g_device.EndScene();
            SAV.Bag.draw();
            
            powerOn(POWER_ALL);
            videoSetMode(MODE_0_3D | DISPLAY_BG0_ACTIVE);
            vramSetBankA(VRAM_A_TEXTURE);
            vramSetBankB(VRAM_B_TEXTURE);
            vramSetBankF(VRAM_F_TEX_PALETTE);
        }
        //StartPkmn
        else if (SAV.PKMN_team.size() && (sqrt(sq(mainSpritesPositions[0][0]-touch.px) + sq(mainSpritesPositions[0][1]-touch.py)) <= 16 )&& mode == -1)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }

            //g_device.EndScene();

            scrn.run_pkmn();

            powerOn(POWER_ALL);
            videoSetMode(MODE_0_3D | DISPLAY_BG0_ACTIVE);
            vramSetBankA(VRAM_A_TEXTURE);
            vramSetBankB(VRAM_B_TEXTURE);
            vramSetBankF(VRAM_F_TEX_PALETTE);

            scrn.draw(mode);
        }
        //StartDex
        else if (sqrt(sq(mainSpritesPositions[2][0]-touch.px) + sq(mainSpritesPositions[2][1]-touch.py)) <= 16 && mode == -1)
        {  			
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            //g_device.EndScene();
            scrn.run_dex();
            scrn.draw(mode);
            
            powerOn(POWER_ALL);
            videoSetMode(MODE_0_3D | DISPLAY_BG0_ACTIVE);
            vramSetBankA(VRAM_A_TEXTURE);
            vramSetBankB(VRAM_B_TEXTURE);
            vramSetBankF(VRAM_F_TEX_PALETTE);
        }
        //StartOptions
        else if (sqrt(sq(mainSpritesPositions[4][0]-touch.px) + sq(mainSpritesPositions[4][1]-touch.py)) <= 16 && mode == -1)
        {  			
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            //g_device.EndScene();
            //vramSetup();
            initVideo();
            videoSetMode(MODE_5_2D |DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
            VRAM_F_CR = VRAM_ENABLE | VRAM_OFFSET(1);
            BATTLE::battle_trainer me("TEST",0,0,0,0,&SAV.PKMN_team,0);
            std::vector<POKEMON::PKMN> cpy = SAV.PKMN_team;
            BATTLE::battle_trainer opp("TEST_OPP","DER TEST ISt DER BESTE MSG1","DER TEST VERLIERT GERADE... MSG2","DER TEST GEWINNT HAHA! MSG3","DER TEST VERLIERT... MSG4",&(cpy),0);

            BATTLE::battle test_battle(&me,&opp,100,5,BATTLE::battle::DOUBLE);
            test_battle.start(100,BATTLE::battle::NONE);

            //for(s16 i= 0; i<256;i++)
            //    for(s16 j = 0; j< 192; ++j)
            //        ((Color *)BG_BMP_RAM(1))[(i + j * SCREEN_WIDTH)/2] = (u8)1;
            //scrn.draw(mode);
            //
            //powerOn(POWER_ALL);
            //videoSetMode(MODE_0_3D | DISPLAY_BG0_ACTIVE);
            //vramSetBankA(VRAM_A_TEXTURE);
            //vramSetBankB(VRAM_B_TEXTURE);
            //vramSetBankF(VRAM_F_TEX_PALETTE);
        }
        //StartID
        else if (sqrt(sq(mainSpritesPositions[1][0]-touch.px) + sq(mainSpritesPositions[1][1]-touch.py)) <= 16 && mode == -1)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            const char *someText[7]= {"\n     PKMN-Spawn","\n    Item-Spawn","\n 1-Item_Test","  Nicht\n  jetzt.","\nWie bitte?","\n   ...","\n    42"};
            cbox test(3,&someText[0],0,false);
            int res = test.getResult("...",true);
            switch(res)
            {
            case 0:
                {
                    SAV.PKMN_team.clear();
                    char A []= {1,2,3,4};
                    for(int i = 0;i<6;++i)
                    {
                        POKEMON::PKMN a(A,HILFSCOUNTER,0,
                        1+rand()%100,SAV.ID,SAV.SID,L"TEST"/*SAV.getName()*/,false,true,rand()%2,true,rand()%2,false,HILFSCOUNTER,i+1,i);
                        stored_pkmn[*free_spaces.rbegin()] = a.boxdata;
                        a.stats.acHP = i*a.stats.maxHP/5;
                        SAV.PKMN_team.push_back(a);
                            
                        SAV.inDex[a.boxdata.SPEC-1] = true;
                        box_of_st_pkmn[a.boxdata.SPEC-1].push_back(*free_spaces.rbegin());
                        //printf("%i",(*free_spaces.rbegin()));
                        free_spaces.pop_back();
                        HILFSCOUNTER= 1+((HILFSCOUNTER)%649);
                    }
                    for(int i= 0; i< 649; ++i)
                        SAV.inDex[i] = true;
                    SAV.hasPKMN = true;
                    swiWaitForVBlank();
                    setMainSpriteVisibility(false);
                    break;
                }
            case 1:
                for(int j = 0; j< 700; ++j)
                    SAV.Bag.addItem(ItemList[j].itemtype,j,(rand()%999) +1);
                break;
            case 2:
                SAV.PKMN_team[0].boxdata.exp += 100;
                mbox(item(),42);
                mbox(berry("Ginemabeere"),31);
            }
            scrn.draw(mode);
        }
        //StartPok\x82""nav
        else if (sqrt(sq(mainSpritesPositions[5][0]-touch.px) + sq(mainSpritesPositions[5][1]-touch.py)) <= 16 && mode == -1)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            mode = 0;
            scrn.draw(mode);
        }
        //StartMaps
        else if (sqrt(sq(mainSpritesPositions[0][0]-touch.px) + sq(mainSpritesPositions[0][1]-touch.py)) <= 16 && mode == 0)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            if(acMapRegion == NONE)
            acMapRegion = acRegion;
            mode = acMapRegion;
            showmappointer = true;
            scrn.draw(mode);
        }
        //Nav->StartScrn
        else if ( touch.px>224 && touch.py>164 && mode == 0)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                    break;
            }
            mode = -1;
            scrn.draw(mode);
        }
        //Map->NavScrn
        else if ( touch.px>224 && touch.py>164 && mode > 0)
        {  
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                    break;
            }
            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,4,0,20,3);
            consoleClear();
            showmappointer = false;
            oam->oamBuffer[SQCH_ID].isHidden = true;
            oam->oamBuffer[SQCH_ID+1].isHidden = true;
            updateOAMSub(oam);
            mode = 0;
            scrn.draw(mode);
        }
        //SwitchMap
        else if ((pressed & KEY_SELECT )&& mode > 0)
        {  
            mode = ((mode+1) % 3)+1;
            consoleSetWindow(&Bottom,5,0,20,1);
            consoleSelect(&Bottom);
            consoleClear();
            scrn.draw(mode);
        }
        //MapCourser
        else if(touch.px > 39 && touch.px < SCREEN_WIDTH-39 && touch.py > 31 && touch.py < SCREEN_HEIGHT-31 && mode > 0){
            oam->oamBuffer[SQCH_ID].x = oam->oamBuffer[SQCH_ID + 1].x = touch.px-8;
            oam->oamBuffer[SQCH_ID].y = oam->oamBuffer[SQCH_ID + 1].y = touch.py-8;
            printMapLocation(touch);
            updateOAMSub(oam);
        }
        else if(touch.px != 0 && touch.py != 0 && sqrt(sq(touch.px-8)+sq(touch.py-12)) <= 17)
        {
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            bool sqa = oam->oamBuffer[SQCH_ID].isHidden,
            sqb = oam->oamBuffer[SQCH_ID+1].isHidden;
            oam->oamBuffer[SQCH_ID].isHidden = true;
            oam->oamBuffer[SQCH_ID+1].isHidden = true;
            bool mappy = showmappointer;
            showmappointer = false;
            updateOAMSub(oam);
            ynbox Save("Pok\x82""Nav ");
            if(Save.getResult("M\x94""chtest du deinen\nFortschritt sichern?\n"))
            { 
                if(gMod == EMULATOR)
                    mbox Succ("Speichern?\nIn einem Emulator?","Pok\x82""Nav");
                else if(SAV.save(progress))
                    mbox Succ("Fortschritt\nerfolgreich gesichert!","Pok\x82""Nav");
                else
                    mbox Succ("Es trat ein Fehler aufSpiel nicht gesichert","Pok\x82""Nav");
            }
            consoleClear();
            oam->oamBuffer[SQCH_ID].isHidden = sqa;
            oam->oamBuffer[SQCH_ID+1].isHidden = sqb;
            showmappointer = mappy;
            updateOAMSub(oam);
            scrn.draw(mode);
        }
        //drawMap		
        //acMap->render();
        //N3DSampleFrameworkWaitForVBlank();
        //End 

    }
    return 0;
}