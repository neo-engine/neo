#pragma once

#include <nds.h>

#include "BG1.h"
#include "sprite.h"
#include <string>

#ifndef __SCRN__
#define __SCRN__

#define sq(a) ((a)*(a))
#define TIMER_SPEED (BUS_CLOCK/1024)

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
class savgm;
extern savgm SAV;

extern PrintConsole Top,Bottom;
extern ConsoleFont cfont;
extern int achours, acseconds, acminutes, acday, acmonth, acyear;
extern int hours, seconds, minutes, day, month, year;
extern unsigned int ticks;

namespace POKEMON{bool drawInfoSub(u16* layer,int PKMN);}

enum Region{
    NONE = 0,
    HOENN = 1,
    KANTO = 2,
    JOHTO = 3
};

#define BACK_ID  0
#define SAVE_ID  1
#define PKMN_ID  2
#define ID_ID  3
#define DEX_ID  4
#define BAG_ID  5
#define OPTS_ID  6
#define NAV_ID  7
#define A_ID  8
#define M_ID  9
// 10 - used
// 11 - used
// 12 - used
#define FWD_ID  13
#define BWD_ID  14
#define CHOICE_ID  15
// 14 additional spaces used
#define BORDER_ID 31
// 9 additional spaces used
#define SQCH_ID 41
// 42 used

#define MAXBG 6
struct BG_set {
    std::string Name;
    const unsigned int *MainMenu;
    const unsigned short* MainMenuPal;
    bool load_from_rom;
    bool allowsOverlay;
};
extern BG_set BGs[MAXBG];
extern int BG_ind;

void vramSetup();

void updateTime();

void initVideoSub();

void animateBack();
void setMainSpriteVisibility(bool hidden);

void drawItem(OAMTable* oam,SpriteInfo* spriteInfo,const std::string& item_name,const int posX,const int posY,const int cnt,
int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen,bool showcnt);
void drawItemIcon(OAMTable* oam,SpriteInfo* spriteInfo,const std::string& item_name,const int posX,const int posY,
int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen = true);
void drawPKMNIcon(OAMTable* oam,SpriteInfo* spriteInfo,const int& pkmn_no,const int posX,const int posY,
int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen);

bool loadNavScreen(u16* layer,const char* Name,int no);
bool loadPicture(u16* layer,const char* Path, const char* Name,int palsize = 512);
bool loadPictureSub(u16* layer,const char* Path, const char* Name,int palsize = 512);
bool laodSprite(SpriteInfo* spriteInfo,const char* Path, const char* Name);
bool laodSpriteSub(SpriteInfo* spriteInfo,const char* Path, const char* Name);
bool loadPKMNSprite(OAMTable* oam,SpriteInfo* spriteInfo, const char* Path,const int& pkmn_no,const int posX,
                    const int posY, int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool bottom,bool shiny = false,bool female = false,bool flipx = false);
bool loadPKMNSpriteTop(OAMTable* oam,SpriteInfo* spriteInfo, const char* Path,const int& pkmn_no,const int posX,
                    const int posY, int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool bottom,bool shiny = false,bool female = false,bool flipx = false);

void drawTypeIcon (OAMTable *oam, SpriteInfo * spriteInfo, int& oamIndex, int& palcnt, int & nextTile, Type t ,int x,int y, bool bottom);

extern std::string bagnames[8];
class scrnloader{
private:
    int pos;

public:
    scrnloader(int p):pos(p){}
    
    void draw(int m);
    void init();

    void run_bag();
    void run_pkmn();
    void run_dex(int num = 0);
};

#endif