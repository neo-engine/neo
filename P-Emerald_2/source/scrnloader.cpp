#include <fstream>
#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "mbox.h"
#include "scrnloader.h"
#include "savgm.h"
#include "PKMN.h"
#include "sprite.h"
#include "bag.h"
#include "item.h"
#include "battle.h"

#include "DexTop.h"
//Sprites
#include "Back.h"
#include "Save.h"
#include "Option.h"
#include "Pokemon.h"
#include "Id.h"
#include "SPBag.h"
#include "Nav.h"
#include "PokeDex.h"
#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Choice_1.h"
#include "Choice_2.h"
#include "Choice_3.h"
#include "Message.h"
#include "Border_1.h"
#include "Border_2.h"
#include "Border_3.h"
#include "Border_4.h"
#include "Border_5.h"
#include "ChSq_a.h"
#include "ChSq_b.h"
#include "BigCirc1.h"

#include "ItemSpr1.h"
#include "ItemSpr2.h"
#include "ItemSpr3.h"

#include "NoItem.h"
#include "BagSpr.h"
#include "BagSpr2.h"

PrintConsole Top,Bottom;

unsigned int NAV_DATA[12288] = {0};
unsigned short int NAV_DATA_PAL[256] = {0};
BG_set BGs[MAXBG] = {{"Raging_Gyarados",&(BG1Bitmap[0]),&(BG1Pal[0]),false,false},
    {"Sleeping_Eevee",NAV_DATA,NAV_DATA_PAL,true,false},
    {"Mystic_Guardevoir",NAV_DATA,NAV_DATA_PAL,true,true},
    {"Waiting_Suicune",NAV_DATA,NAV_DATA_PAL,true,true},
    {"Fighting_Groudon",NAV_DATA,NAV_DATA_PAL,true,false},
    {"Fighting_Kyogre",NAV_DATA,NAV_DATA_PAL,true,false}};
int BG_ind = 0;
extern POKEMON::PKMN::BOX_PKMN stored_pkmn[MAXSTOREDPKMN];
extern std::vector<int> box_of_st_pkmn[MAXPKMN];
extern std::vector<int> free_spaces;

extern SpriteInfo spriteInfo[SPRITE_COUNT];
extern SpriteInfo spriteInfoTop[SPRITE_COUNT];
extern OAMTable *oam,*oamTop;

extern int drawBox(short);
namespace POKEMON { extern const char* getLoc(int ind); }
Region acMapRegion = NONE;
std::pair<int,int> acMapPoint = std::pair<int,int>(32,24);
bool showfirst = true,showmappointer = false;

#define MAXMAPPOS 75
struct MapRegionPos{int lx,ly,rx,ry,ind;} MapLocations[3][MAXMAPPOS] = {{{90,66,96,73,1010},{115,139,121,146,308},{57,36,66,47,239},{46,53,51,59,298},{37,73,47,89,245},{39,98,45,104,294},{45,104,53,112,242},{47,136,54,142,291},{50,136,59,148,237},{64,103,72,111,236},{64,117,72,125,235},{64,80,72,88,240},{65,75,71,80,295},{69,59,78,67,238},{77,47,85,60,292},{79,60,84,66,312},{89,80,105,96,244},{89,109,98,126,243},{114,36,123,46,246},{140,52,149,62,202},{142,71,147,77,300},{121,94,130,104,1000},{146,109,154,118,241},{168,112,174,116,306},{161,112,167,116,320},{110,112,116,116,313},{179,122,187,131,293},{210,95,219,111,250},{173,88,180,97,249},{192,74,206,82,248},{200,60,204,66,302},{153,59,168,67,247},{93,111,115,117,285},{114,111,130,117,284},{129,111,150,117,283},{149,111,174,117,282},{173,111,191,117,281},{190,111,205,117,280},{187,104,212,112,279},{187,80,205,105,278},{168,80,188,101,277},{168,60,189,81,275},{190,60,205,76,276},{128,60,153,66,272},{142,65,147,83,273},{115,82,147,88,274},{122,87,128,95,1001},{103,82,115,88,269},{109,39,114,83,270},{122,39,128,66,271},{90,39,96,83,262},{76,60,91,66,263},{62,37,96,44,264},{39,54,45,75,266},{46,39,51,53,265},{46,39,60,44,265},{45,75,66,81,267},{39,90,45,109,254},{39,108,45,133,255},{39,132,57,139,256},{57,140,70,146,258},{69,140,91,146,259},{90,122,96,146,260},{90,85,96,116,261},{69,82,94,88,268},{50,103,66,110,252},{64,109,70,119,251},{65,96,91,103,253}}};
void printMapLocation(const touchPosition& t){
    consoleSetWindow(&Bottom,5,0,20,1);
    consoleSelect(&Bottom);
    consoleClear();
    for(int i=0; i < MAXMAPPOS; ++i)
    if(t.px > MapLocations[acMapRegion-1][i].lx && t.px < MapLocations[acMapRegion-1][i].rx &&
            t.py > MapLocations[acMapRegion-1][i].ly &&t.py < MapLocations[acMapRegion-1][i].ry){
        printf(POKEMON::getLoc(MapLocations[acMapRegion-1][i].ind));
        return;
    }
}

void updateTime()
{
    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);

    achours = timeStruct->tm_hour;
    acminutes = timeStruct->tm_min;
    if(acseconds != timeStruct->tm_sec){
        acseconds = timeStruct->tm_sec;
        if(showmappointer){
            if(showfirst){
                showfirst = false;
                oam->oamBuffer[SQCH_ID].isHidden = true;
                oam->oamBuffer[SQCH_ID+1].isHidden = false;
            }
            else{
                showfirst = true;
                oam->oamBuffer[SQCH_ID].isHidden = false;
                oam->oamBuffer[SQCH_ID+1].isHidden = true;
            }
            updateOAMSub(oam);
        }
    }
    acday = timeStruct->tm_mday;
    acmonth = timeStruct->tm_mon;
    acyear = timeStruct->tm_year +1900;
    
    int oX = Bottom.windowX, oY = Bottom.windowY,wX = Bottom.windowWidth,wY = Bottom.windowHeight;
    consoleSetWindow(&Bottom,18,23,8,1);
    consoleSelect(&Bottom);
    consoleClear();
    printf("%02i:%02i:%02i", achours, acminutes,acseconds);
    consoleSetWindow(&Bottom,oX,oY,wX,wY);
}

unsigned int TEMP[12288] = {0};
unsigned short int TEMP_PAL[256] = {0};

bool loadSprite(SpriteInfo* spriteInfo,const std::string& Path, const std::string& Name,const int TileCnt,const int PalCnt){
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
    sizeof(SPRITE_GFX[0]);
    std::stringstream ss;
    ss << Path << Name << ".raw";
    FILE* fd = fopen(&(ss.str()[0]),"rb");

    if(fd == 0){
        fclose(fd);
        return false;
    }
    fread(TEMP,  sizeof(unsigned int),TileCnt, fd);
    fread(TEMP_PAL,  sizeof(unsigned short int),PalCnt, fd);

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[spriteInfo->entry->gfxIndex * OFFSET_MULTIPLIER], 4*TileCnt);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[spriteInfo->entry->palette * COLORS_PER_PALETTE], 2*PalCnt);
    fclose(fd);
    return true;
}
bool loadSpriteSub(SpriteInfo* spriteInfo,const char* Path, const char* Name,const int TileCnt,const int PalCnt){
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
    sizeof(SPRITE_GFX_SUB[0]);
    char pt[100];
    sprintf(pt, "%s%s.raw",Path,Name);
    FILE* fd = fopen(pt,"rb");
    
    if(fd == 0){
        fclose(fd);
        return false;
    }
    fread(TEMP,  sizeof(unsigned int),TileCnt, fd);
    fread(TEMP_PAL,  sizeof(unsigned short int),PalCnt, fd);

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    TEMP,
    &SPRITE_GFX_SUB[spriteInfo->entry->gfxIndex * OFFSET_MULTIPLIER],
    4*TileCnt);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    TEMP_PAL,
    &SPRITE_PALETTE_SUB[spriteInfo->entry->palette * COLORS_PER_PALETTE],
    2*PalCnt);
    fclose(fd);
    return true;
}
bool loadPKMNSprite(OAMTable* oam,SpriteInfo* spriteInfo, const char* Path,const int& pkmn_no,const int posX,
                    const int posY, int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool bottom,bool shiny,bool female,bool flipx){
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX_SUB[0]);
    
    char pt[100];
    if(!female)
        sprintf(pt, "%sSprite_%d.raw",Path,pkmn_no);
    else
        sprintf(pt, "%sSprite_%df.raw",Path,pkmn_no);
    FILE* fd = fopen(pt,"rb");
     
    if(fd == 0){
        fclose(fd); 
        return false;
    }
    int PalCnt = 16;
    for(int i= 0; i< 16; ++i)
        TEMP_PAL[i] = 0;
    fread(TEMP_PAL,  sizeof(unsigned short int),PalCnt, fd);
    for(int i = 0; i< 96*96; ++i)
        TEMP[i] = 0;
    fread(TEMP,  sizeof(unsigned int),96*96, fd);
    fclose(fd);
    if(shiny){
        memset(pt,0,sizeof(pt));
        if(!female)
            sprintf(pt, "%sSprite_%ds.raw",Path,pkmn_no);
        else
            sprintf(pt, "%sSprite_%dsf.raw",Path,pkmn_no);
        fd = fopen(pt,"rb");
        for(int i= 0; i< 16; ++i)
            TEMP_PAL[i] = 0; 
        fread(TEMP_PAL,  sizeof(unsigned short int),PalCnt, fd);
        fclose(fd);
    }
    if(bottom)
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[(++palcnt) * COLORS_PER_PALETTE], 32);
    else
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[(++palcnt) * COLORS_PER_PALETTE], 32);

    SpriteInfo * backInfo = &spriteInfo[++oamIndex];
    SpriteEntry * back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;
    backInfo->width = 64;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL; 
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = flipx ? 32 + posX : posX;
    back->hFlip = flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = palcnt;
    if(bottom)
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER], 96*96/2);
    else
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER], 96*96/2);
    nextAvailableTileIdx += 64;

    backInfo = &spriteInfo[++oamIndex];
    back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;
    backInfo->width = 32;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_TALL;
    back->x = flipx ?  posX : 64 +posX;
    back->hFlip = flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = palcnt; 
    nextAvailableTileIdx += 32;

    backInfo = &spriteInfo[++oamIndex];
    back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;
    backInfo->width = 64;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = posY + 64;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_WIDE;
    back->x = flipx ? 32 + posX : posX;
    back->hFlip = flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = palcnt; 
    nextAvailableTileIdx += 32;

    backInfo = &spriteInfo[++oamIndex];
    back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;  
    backInfo->width = 32; 
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = posY + 64;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = flipx ?  posX : 64 +posX;
    back->hFlip = flipx;
    back->size = OBJSIZE_32;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = palcnt;
    nextAvailableTileIdx += 16;

    ++palcnt;
    if(bottom)
        updateOAMSub(oam);
    else
        updateOAM(oam);
    return true;
}
bool loadPKMNSpriteTop(OAMTable* oam,SpriteInfo* spriteInfo, const char* Path,const int& pkmn_no,const int posX,
                    const int posY, int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool bottom,bool shiny,bool female,bool flipx){
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX_SUB[0]);
    
    char pt[100];
    if(!female)
        sprintf(pt, "%sSprite_%d.raw",Path,pkmn_no);
    else
        sprintf(pt, "%sSprite_%df.raw",Path,pkmn_no);
    FILE* fd = fopen(pt,"rb");
     
    if(fd == 0){
        fclose(fd); 
        return false;
    }
    int PalCnt = 16;
    for(int i= 0; i< 16; ++i)
        TEMP_PAL[i] = 0;
    fread(TEMP_PAL,  sizeof(unsigned short int),PalCnt, fd);
    for(int i = 0; i< 96*96; ++i)
        TEMP[i] = 0;
    fread(TEMP,  sizeof(unsigned int),96*64, fd);
    fclose(fd);
    if(shiny){
        memset(pt,0,sizeof(pt));
        if(!female)
            sprintf(pt, "%sSprite_%ds.raw",Path,pkmn_no);
        else
            sprintf(pt, "%sSprite_%dsf.raw",Path,pkmn_no);
        fd = fopen(pt,"rb");
        for(int i= 0; i< 16; ++i)
            TEMP_PAL[i] = 0; 
        fread(TEMP_PAL,  sizeof(unsigned short int),PalCnt, fd);
        fclose(fd);
    }
    if(bottom)
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE_SUB[(++palcnt) * COLORS_PER_PALETTE], 32);
    else
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP_PAL, &SPRITE_PALETTE[(++palcnt) * COLORS_PER_PALETTE], 32);

    SpriteInfo * backInfo = &spriteInfo[++oamIndex];
    SpriteEntry * back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;
    backInfo->width = 64;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL; 
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = flipx ? 32 + posX : posX;
    back->hFlip = flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = palcnt;
    if(bottom)
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER], 96*96/2);
    else
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL, TEMP, &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER], 96*96/2);
    nextAvailableTileIdx += 64;

    backInfo = &spriteInfo[++oamIndex];
    back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;
    backInfo->width = 32;
    backInfo->height = 64;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = posY;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_TALL;
    back->x = flipx ?  posX : 64 +posX;
    back->hFlip = flipx;
    back->size = OBJSIZE_64;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = palcnt; 
    nextAvailableTileIdx += 32;

    ++palcnt;
    if(bottom)
        updateOAMSub(oam);
    else
        updateOAM(oam);
    return true;
}
bool loadPicture(u16* layer,const char* Path, const char* Name,int palsize){
    char pt[100];
    sprintf(pt, "%s%s.raw",Path,Name);
    FILE* fd = fopen(pt,"rb");

    if(fd == 0){
        fclose(fd);
        return false;
    }    
    
    fread(TEMP,  sizeof(unsigned int),12288, fd);
    fread(TEMP_PAL,  sizeof(unsigned short int),256, fd);

    dmaCopy(TEMP, layer, 256*192 );
    dmaCopy( TEMP_PAL, BG_PALETTE, palsize); 
    fclose(fd);
    
    return true;
}
bool loadPictureSub(u16* layer,const char* Path, const char* Name,int palsize){
    char pt[100];
    sprintf(pt, "%s%s.raw",Path,Name);
    FILE* fd = fopen(pt,"rb");

    if(fd == 0){
        fclose(fd);
        return false;
    }
    
    fread(TEMP,  sizeof(unsigned int),12288, fd);
    fread(TEMP_PAL,  sizeof(unsigned short int),256, fd);

    dmaCopy(TEMP, layer, 256*192 );
    dmaCopy( TEMP_PAL, BG_PALETTE_SUB, palsize); 
    fclose(fd);
    
    
    return true;
}

bool loadNavScreen(u16* layer,const char* Name,int no){
    if(no == BG_ind && NAV_DATA[0]){
        dmaCopy( NAV_DATA, layer, 256*192 );
        dmaCopy( NAV_DATA_PAL, BG_PALETTE_SUB, 256*2); 
        return true;
    }
    
    char pt[100];
    sprintf(pt, "nitro:/PICS/NAV/%s.raw",Name);
    FILE* fd = fopen(pt,"rb");
    
    if(fd == 0){
        fclose(fd);
        return false;
    }
    
    fread(NAV_DATA,  sizeof(unsigned int),12288, fd);
    fread(NAV_DATA_PAL,  sizeof(unsigned short int),256, fd);

    dmaCopy(NAV_DATA, layer, 256*192 );
    dmaCopy( NAV_DATA_PAL, BG_PALETTE_SUB, 256*2); 
    fclose(fd);

    return true;
}

unsigned int BTMs[6][12288];
unsigned short int PALs[6][256];

unsigned short int cachedPKMN[6] = {0};

extern int mainSpritesPositions[6][2];

int ac = 0;
namespace POKEMON{
    bool drawInfoSub(u16* layer,int PKMN) /*--deprecated*/
    //Bilder für InfoScrn von SD laden
    {
        for(int i= 0; i < 6; ++i)
        if(cachedPKMN[i] == PKMN){
            dmaCopy( BTMs[i], layer, 256*256 );
            dmaCopy( PALs[i], BG_PALETTE_SUB, 256*2); 
            ac = (ac+1)%6;  
            BG_PALETTE_SUB[3] = RGB15(0,0,0);
            return true;
        }
        std::stringstream ss;
        ss << "nitro:/PICS/PKMN_SUB_SCREEN/_";
        if(PKMN < 10) ss << "0";
        if(PKMN < 100) ss << "0";
        ss << PKMN << "Info.raw";
        FILE* fd = fopen(&(ss.str()[0]),"rb");

        if(fd == 0){
            return false;
        }
        ac = (ac+1)%6;
    
        fread(&BTMs[ac][0],  sizeof(unsigned int),12288, fd);
        fread(&PALs[ac][0],  sizeof(unsigned short int),256, fd);

        dmaCopy( BTMs[ac], layer, 256*256 );
        dmaCopy( PALs[ac], BG_PALETTE_SUB, 256*2); 
        cachedPKMN[ac] = PKMN;
        fclose(fd);

        for(int i= 9;i <= 12; ++i) 
        oam->oamBuffer[i].isHidden = true;
        updateOAMSub(oam);
    
        BG_PALETTE_SUB[3] = RGB15(0,0,0);
        return true;
    }
}
int positions[6][2] = {
    {14,2}, 	{16,3}, 	{14,9},
    {16,10}, 	{14,17},	{16,18}
};
int borders[6][2] = {
    {4,2},{18,3},
    {4,9},{18,10},
    {4,17},{18,18}
};

extern SpriteInfo spriteInfo[SPRITE_COUNT];
extern OAMTable *oam;

int initMainSprites(OAMTable * oam, SpriteInfo *spriteInfo){
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
    sizeof(SPRITE_GFX_SUB[0]);

    /* Keep track of the available tiles */
    int nextAvailableTileIdx = 0;
    
    SpriteInfo * backInfo = &spriteInfo[BACK_ID];
    SpriteEntry * back = &oam->oamBuffer[BACK_ID];
    backInfo->oamId = BACK_ID;
    backInfo->width = 32;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = SCREEN_HEIGHT - 28;
    back->isRotateScale = false;
    back->isSizeDouble = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = SCREEN_WIDTH - 28;
    back->rotationIndex = backInfo->oamId;
    back->size = OBJSIZE_32;
    back->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += BackTilesLen / BYTES_PER_16_COLOR_TILE;
    back->priority = OBJPRIORITY_0;
    back->palette = backInfo->oamId;


    SpriteInfo * saveInfo = &spriteInfo[SAVE_ID];
    SpriteEntry * save = &oam->oamBuffer[SAVE_ID];
    saveInfo->oamId = SAVE_ID;
    saveInfo->width = 64;
    saveInfo->height = 64;
    saveInfo->angle = 0;
    saveInfo->entry = save;
    save->y =  - 20;
    save->isRotateScale = false;
    save->isSizeDouble = false;
    save->blendMode = OBJMODE_NORMAL;
    save->isMosaic = false;
    save->colorMode = OBJCOLOR_16;
    save->shape = OBJSHAPE_SQUARE;
    save->x =  - 20;
    save->size = OBJSIZE_64;
    save->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += SaveTilesLen / BYTES_PER_16_COLOR_TILE;
    save->priority = OBJPRIORITY_0;
    save->palette = saveInfo->oamId;


    //MAIN MENU

    SpriteInfo * optsInfo = &spriteInfo[OPTS_ID];
    SpriteEntry * opts = &oam->oamBuffer[OPTS_ID];
    optsInfo->oamId = OPTS_ID;
    optsInfo->width = 32;
    optsInfo->height = 32;
    optsInfo->angle = 0;
    optsInfo->entry = opts;
    opts->y = mainSpritesPositions[OPTS_ID-2][1]- optsInfo->height /2;
    opts->isRotateScale = false;
    opts->isSizeDouble = false;
    opts->blendMode = OBJMODE_NORMAL;
    opts->isMosaic = false;
    opts->colorMode = OBJCOLOR_16;
    opts->shape = OBJSHAPE_SQUARE;
    opts->x = mainSpritesPositions[OPTS_ID-2][0] - optsInfo->width /2;
    opts->size = OBJSIZE_32;
    opts->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += OptionTilesLen / BYTES_PER_16_COLOR_TILE;
    opts->priority = OBJPRIORITY_0;
    opts->palette = optsInfo->oamId;

    
    SpriteInfo * pkmnInfo = &spriteInfo[PKMN_ID];
    SpriteEntry * pkmn = &oam->oamBuffer[PKMN_ID];
    pkmnInfo->oamId = PKMN_ID;
    pkmnInfo->width = 32;
    pkmnInfo->height = 32;
    pkmnInfo->angle = 0;
    pkmnInfo->entry = pkmn;
    pkmn->y = mainSpritesPositions[PKMN_ID-2][1]- pkmnInfo->height /2;
    pkmn->isRotateScale = false;
    pkmn->isSizeDouble = false;
    pkmn->blendMode = OBJMODE_NORMAL;
    pkmn->isMosaic = false;
    pkmn->colorMode = OBJCOLOR_16;
    pkmn->shape = OBJSHAPE_SQUARE;
    pkmn->x = mainSpritesPositions[PKMN_ID-2][0] - pkmnInfo->width /2;
    pkmn->size = OBJSIZE_32;
    pkmn->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += PokemonTilesLen / BYTES_PER_16_COLOR_TILE;
    pkmn->priority = OBJPRIORITY_0;
    pkmn->palette = pkmnInfo->oamId;

    
    SpriteInfo * navInfo = &spriteInfo[NAV_ID];
    SpriteEntry * nav = &oam->oamBuffer[NAV_ID];
    navInfo->oamId = NAV_ID;
    navInfo->width = 32;
    navInfo->height = 32;
    navInfo->angle = 0;
    navInfo->entry = nav;
    nav->y = mainSpritesPositions[NAV_ID-2][1]- navInfo->height /2;
    nav->isRotateScale = false;
    nav->isSizeDouble = false;
    nav->blendMode = OBJMODE_NORMAL;
    nav->isMosaic = false;
    nav->colorMode = OBJCOLOR_16;
    nav->shape = OBJSHAPE_SQUARE;
    nav->x = mainSpritesPositions[NAV_ID-2][0] - navInfo->width /2;
    nav->size = OBJSIZE_32;
    nav->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += NavTilesLen / BYTES_PER_16_COLOR_TILE;
    nav->priority = OBJPRIORITY_0;
    nav->palette = navInfo->oamId;

    
    SpriteInfo * idInfo = &spriteInfo[ID_ID];
    SpriteEntry * id = &oam->oamBuffer[ID_ID];
    idInfo->oamId = ID_ID;
    idInfo->width = 32;
    idInfo->height = 32;
    idInfo->angle = 0;
    idInfo->entry = id;
    id->y = mainSpritesPositions[ID_ID-2][1]- idInfo->height /2;
    id->isRotateScale = false;
    id->isSizeDouble = false;
    id->blendMode = OBJMODE_NORMAL;
    id->isMosaic = false;
    id->colorMode = OBJCOLOR_16;
    id->shape = OBJSHAPE_SQUARE;
    id->x = mainSpritesPositions[ID_ID-2][0] - idInfo->width /2;
    id->size = OBJSIZE_32;
    id->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += IdTilesLen / BYTES_PER_16_COLOR_TILE;
    id->priority = OBJPRIORITY_0;
    id->palette = idInfo->oamId;

    
    SpriteInfo * dexInfo = &spriteInfo[DEX_ID];
    SpriteEntry * dex = &oam->oamBuffer[DEX_ID];
    dexInfo->oamId = DEX_ID;
    dexInfo->width = 32;
    dexInfo->height = 32;
    dexInfo->angle = 0;
    dexInfo->entry = dex;
    dex->y = mainSpritesPositions[DEX_ID-2][1]- dexInfo->height /2;
    dex->isRotateScale = false;
    dex->isSizeDouble = false;
    dex->blendMode = OBJMODE_NORMAL;
    dex->isMosaic = false;
    dex->colorMode = OBJCOLOR_16;
    dex->shape = OBJSHAPE_SQUARE;
    dex->x = mainSpritesPositions[DEX_ID-2][0] - dexInfo->width /2;
    dex->size = OBJSIZE_32;
    dex->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += PokeDexTilesLen / BYTES_PER_16_COLOR_TILE;
    dex->priority = OBJPRIORITY_0;
    dex->palette = dexInfo->oamId;

    
    SpriteInfo * bagInfo = &spriteInfo[BAG_ID];
    SpriteEntry * bag = &oam->oamBuffer[BAG_ID];
    bagInfo->oamId = BAG_ID;
    bagInfo->width = 32;
    bagInfo->height = 32;
    bagInfo->angle = 0;
    bagInfo->entry = bag;
    bag->y = mainSpritesPositions[BAG_ID-2][1]- bagInfo->height /2;
    bag->isRotateScale = false;
    bag->isSizeDouble = false;
    bag->blendMode = OBJMODE_NORMAL;
    bag->isMosaic = false;
    bag->colorMode = OBJCOLOR_16;
    bag->shape = OBJSHAPE_SQUARE;
    bag->x = mainSpritesPositions[BAG_ID-2][0] - bagInfo->width /2;
    bag->size = OBJSIZE_32;
    bag->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += SPBagTilesLen / BYTES_PER_16_COLOR_TILE;
    bag->priority = OBJPRIORITY_0;
    bag->palette = bagInfo->oamId;
    
    //"A"-Button
    SpriteInfo * AInfo = &spriteInfo[A_ID];
    SpriteEntry * A = &oam->oamBuffer[A_ID];
    AInfo->oamId = A_ID;
    AInfo->width = 32;
    AInfo->height = 32;
    AInfo->angle = 0;
    AInfo->entry = A;
    A->y = SCREEN_HEIGHT - 28;
    A->isRotateScale = false;
    A->blendMode = OBJMODE_NORMAL;
    A->isMosaic = false;
    A->colorMode = OBJCOLOR_16;
    A->shape = OBJSHAPE_SQUARE;
    A->isHidden = true;
    A->x = SCREEN_WIDTH - 28;
    A->size = OBJSIZE_32;
    A->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += ATilesLen / BYTES_PER_16_COLOR_TILE;
    A->priority = OBJPRIORITY_0;
    A->palette = AInfo->oamId;

    
    /* Copy over the sprite palettes */
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BackPal,
    &SPRITE_PALETTE_SUB[backInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    SavePal,
    &SPRITE_PALETTE_SUB[saveInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    PokemonPal,
    &SPRITE_PALETTE_SUB[pkmnInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    IdPal,
    &SPRITE_PALETTE_SUB[idInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    PokeDexPal,
    &SPRITE_PALETTE_SUB[dexInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    SPBagPal,
    &SPRITE_PALETTE_SUB[bagInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    OptionPal,
    &SPRITE_PALETTE_SUB[optsInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    NavPal,
    &SPRITE_PALETTE_SUB[navInfo->oamId * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    APal,
    &SPRITE_PALETTE_SUB[AInfo->oamId * COLORS_PER_PALETTE],
    32);


    /* Copy the sprite graphics to sprite graphics memory */
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BackTiles,
    &SPRITE_GFX_SUB[back->gfxIndex * OFFSET_MULTIPLIER],
    BackTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    SaveTiles,
    &SPRITE_GFX_SUB[save->gfxIndex * OFFSET_MULTIPLIER],
    SaveTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    PokemonTiles,
    &SPRITE_GFX_SUB[pkmn->gfxIndex * OFFSET_MULTIPLIER],
    PokemonTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    IdTiles,
    &SPRITE_GFX_SUB[id->gfxIndex * OFFSET_MULTIPLIER],
    IdTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    PokeDexTiles,
    &SPRITE_GFX_SUB[dex->gfxIndex * OFFSET_MULTIPLIER],
    PokeDexTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    SPBagTiles,
    &SPRITE_GFX_SUB[bag->gfxIndex * OFFSET_MULTIPLIER],
    SPBagTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    OptionTiles,
    &SPRITE_GFX_SUB[opts->gfxIndex * OFFSET_MULTIPLIER],
    OptionTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    NavTiles,
    &SPRITE_GFX_SUB[nav->gfxIndex * OFFSET_MULTIPLIER],
    NavTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    ATiles,
    &SPRITE_GFX_SUB[A->gfxIndex * OFFSET_MULTIPLIER],
    ATilesLen);
    
    int palcnt = M_ID;

    //Message
    for(int i= M_ID; i < M_ID + 4; ++i){
        SpriteInfo * MInfo = &spriteInfo[i];
        SpriteEntry * M = &oam->oamBuffer[i];
        MInfo->oamId = i;
        MInfo->width = 64;
        MInfo->height = 64;
        MInfo->angle = 0;
        MInfo->entry = M;
        M->y = 0;
        M->isRotateScale = false;
        M->blendMode = OBJMODE_NORMAL;
        M->isMosaic = false;
        M->colorMode = OBJCOLOR_16;
        M->shape = OBJSHAPE_SQUARE;
        M->isHidden = true;
        M->x = (i-M_ID)*64;
        M->size = OBJSIZE_64;
        M->gfxIndex = nextAvailableTileIdx;
        M->priority = OBJPRIORITY_2;
        M->palette = palcnt;    
    }
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    MessagePal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    MessageTiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    MessageTilesLen);
    nextAvailableTileIdx += MessageTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;

    SpriteInfo * FwdInfo = &spriteInfo[FWD_ID];
    SpriteEntry * Fwd = &oam->oamBuffer[FWD_ID];
    FwdInfo->oamId = FWD_ID;
    FwdInfo->width = 32;
    FwdInfo->height = 32;
    FwdInfo->angle = 0;
    FwdInfo->entry = Fwd;
    Fwd->y = SCREEN_HEIGHT - 28;
    Fwd->isRotateScale = false;
    Fwd->blendMode = OBJMODE_NORMAL;
    Fwd->isMosaic = false;
    Fwd->colorMode = OBJCOLOR_16;
    Fwd->shape = OBJSHAPE_SQUARE;
    Fwd->isHidden = true;
    Fwd->x = SCREEN_WIDTH - 28;
    Fwd->size = OBJSIZE_32;
    Fwd->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += ForwardTilesLen / BYTES_PER_16_COLOR_TILE;
    Fwd->priority = OBJPRIORITY_2;
    Fwd->palette = palcnt;

    
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    ForwardPal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    MessagePalLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    ForwardTiles,
    &SPRITE_GFX_SUB[Fwd->gfxIndex * OFFSET_MULTIPLIER],
    ForwardTilesLen);
    ++palcnt;
    
    SpriteInfo * BwdInfo = &spriteInfo[BWD_ID];
    SpriteEntry * Bwd = &oam->oamBuffer[BWD_ID];
    BwdInfo->oamId = BWD_ID;
    BwdInfo->width = 32;
    BwdInfo->height = 32;
    BwdInfo->angle = 0;
    BwdInfo->entry = Bwd;
    Bwd->y = SCREEN_HEIGHT - 28;
    Bwd->isRotateScale = false;
    Bwd->blendMode = OBJMODE_NORMAL;
    Bwd->isMosaic = false;
    Bwd->colorMode = OBJCOLOR_16;
    Bwd->shape = OBJSHAPE_SQUARE;
    Bwd->isHidden = true;
    Bwd->x = SCREEN_WIDTH - 28;
    Bwd->size = OBJSIZE_32;
    Bwd->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += BackwardTilesLen / BYTES_PER_16_COLOR_TILE;
    Bwd->priority = OBJPRIORITY_2;
    Bwd->palette = palcnt;

    
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BackwardPal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    MessagePalLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BackwardTiles,
    &SPRITE_GFX_SUB[Bwd->gfxIndex * OFFSET_MULTIPLIER],
    BackwardTilesLen);
    ++palcnt;

    //cbox
    int nextnext = nextAvailableTileIdx + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;

    for(int i= 0; i < 6; ++i){
        SpriteInfo * C1Info = &spriteInfo[2*i+CHOICE_ID];
        SpriteEntry * C1 = &oam->oamBuffer[2*i+CHOICE_ID];
        C1Info->oamId = CHOICE_ID;
        C1Info->width = 64;
        C1Info->height = 32;
        C1Info->angle = 0;
        C1Info->entry = C1;
        C1->y = 68+ (i/2)*32;
        C1->isRotateScale = false;
        C1->blendMode = OBJMODE_NORMAL;
        C1->isMosaic = false;
        C1->colorMode = OBJCOLOR_16;
        C1->shape = OBJSHAPE_WIDE;
        C1->isHidden = true;
        C1->x = ((i%2)? 32 : 128);
        C1->size = OBJSIZE_64;
        C1->gfxIndex = nextAvailableTileIdx;
        
        C1->priority = OBJPRIORITY_2;
        C1->palette = palcnt;
        
        SpriteInfo * C3Info = &spriteInfo[2*i+CHOICE_ID+1];
        SpriteEntry * C3 = &oam->oamBuffer[2*i+CHOICE_ID+1];
        C3Info->oamId = CHOICE_ID;
        C3Info->width = 64;
        C3Info->height = 32;
        C3Info->angle = 0;
        C3Info->entry = C3;
        C3->y = 68+ (i/2)*32;
        C3->isRotateScale = false;
        C3->blendMode = OBJMODE_NORMAL;
        C3->isMosaic = false;
        C3->colorMode = OBJCOLOR_16;
        C3->shape = OBJSHAPE_WIDE;
        C3->isHidden = true;
        C3->x = ((i%2)? 62 : 160);
        C3->size = OBJSIZE_64;
        C3->gfxIndex = nextnext;
        C3->priority = OBJPRIORITY_2;
        C3->palette = palcnt;
    }
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Choice_1Pal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    Choice_1PalLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Choice_1Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    Choice_1TilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Choice_3Tiles,
    &SPRITE_GFX_SUB[nextnext * OFFSET_MULTIPLIER],
    Choice_3TilesLen);
    nextAvailableTileIdx = nextnext + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
    for(int i= 0; i <3; ++i){
        SpriteInfo * C2Info = &spriteInfo[i+CHOICE_ID+12];
        SpriteEntry * C2 = &oam->oamBuffer[i+CHOICE_ID+12];
        C2Info->oamId = CHOICE_ID;
        C2Info->width = 64;
        C2Info->height = 32;
        C2Info->angle = 0;
        C2Info->entry = C2;
        C2->y = 68+ (i)*32;
        C2->isRotateScale = false;
        C2->blendMode = OBJMODE_NORMAL;
        C2->isMosaic = false;
        C2->colorMode = OBJCOLOR_16;
        C2->shape = OBJSHAPE_WIDE;
        C2->isHidden = true;
        C2->x = 96;
        C2->size = OBJSIZE_64;
        C2->gfxIndex = nextAvailableTileIdx;
        C2->priority = OBJPRIORITY_2;
        C2->palette = palcnt;
    }
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Choice_2Tiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    Choice_2TilesLen);
    nextAvailableTileIdx += Choice_2TilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;

    SpriteInfo * Bo4Info = &spriteInfo[BORDER_ID];
    SpriteEntry * Bo4 = &oam->oamBuffer[BORDER_ID];
    Bo4Info->oamId = BORDER_ID;
    Bo4Info->width = 64;
    Bo4Info->height = 64;
    Bo4Info->angle = 0;
    Bo4Info->entry = Bo4;
    Bo4->y = 0;
    Bo4->isRotateScale = false;
    Bo4->blendMode = OBJMODE_NORMAL;
    Bo4->isMosaic = true;
    Bo4->colorMode = OBJCOLOR_16;
    Bo4->shape = OBJSHAPE_SQUARE;
    Bo4->isHidden = false;
    Bo4->x = 0;
    Bo4->size = OBJSIZE_64;
    Bo4->gfxIndex = nextAvailableTileIdx;
    Bo4->priority = OBJPRIORITY_2;
    Bo4->palette = palcnt;
    Bo4->vFlip = true;
    Bo4->hFlip = true;

    Bo4 = &oam->oamBuffer[BORDER_ID+1];
    Bo4->y = 128;
    Bo4->isRotateScale = false;
    Bo4->blendMode = OBJMODE_NORMAL;
    Bo4->isMosaic = true;
    Bo4->colorMode = OBJCOLOR_16;
    Bo4->shape = OBJSHAPE_SQUARE;
    Bo4->isHidden = false;
    Bo4->x = 192;
    Bo4->size = OBJSIZE_64;
    Bo4->gfxIndex = nextAvailableTileIdx;
    Bo4->priority = OBJPRIORITY_2;
    Bo4->palette = palcnt;
    Bo4->vFlip = false;
    Bo4->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Border_4Pal,    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Border_4Tiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    Border_4TilesLen);
    nextAvailableTileIdx += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo3Info = &spriteInfo[BORDER_ID+2];
    SpriteEntry * Bo3 = &oam->oamBuffer[BORDER_ID+2];
    Bo3Info->oamId = BORDER_ID;
    Bo3Info->width = 64;
    Bo3Info->height = 64;
    Bo3Info->angle = 0;
    Bo3Info->entry = Bo3;
    Bo3->y = 0;
    Bo3->isRotateScale = false;
    Bo3->blendMode = OBJMODE_NORMAL;
    Bo3->isMosaic = true;
    Bo3->colorMode = OBJCOLOR_16;
    Bo3->shape = OBJSHAPE_SQUARE;
    Bo3->isHidden = false;
    Bo3->x = 64;
    Bo3->size = OBJSIZE_64;
    Bo3->gfxIndex = nextAvailableTileIdx;
    Bo3->priority = OBJPRIORITY_2;
    Bo3->palette = palcnt;
    Bo3->vFlip = true;
    Bo3->hFlip = true;

    Bo3 = &oam->oamBuffer[BORDER_ID+3];
    Bo3->y = 128;
    Bo3->isRotateScale = false;
    Bo3->blendMode = OBJMODE_NORMAL;
    Bo3->isMosaic = true;
    Bo3->colorMode = OBJCOLOR_16;
    Bo3->shape = OBJSHAPE_SQUARE;
    Bo3->isHidden = false;
    Bo3->x = 128;
    Bo3->size = OBJSIZE_64;
    Bo3->gfxIndex = nextAvailableTileIdx;
    Bo3->priority = OBJPRIORITY_2;
    Bo3->palette = palcnt;
    Bo3->vFlip = false;
    Bo3->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Border_3Tiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    Border_3TilesLen);
    nextAvailableTileIdx += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo2Info = &spriteInfo[BORDER_ID+4];
    SpriteEntry * Bo2 = &oam->oamBuffer[BORDER_ID+4];
    Bo2Info->oamId = BORDER_ID;
    Bo2Info->width = 64;
    Bo2Info->height = 64;
    Bo2Info->angle = 0;
    Bo2Info->entry = Bo2;
    Bo2->y = 0;
    Bo2->isRotateScale = false;
    Bo2->blendMode = OBJMODE_NORMAL;
    Bo2->isMosaic = true;
    Bo2->colorMode = OBJCOLOR_16;
    Bo2->shape = OBJSHAPE_SQUARE;
    Bo2->isHidden = false;
    Bo2->x = 128;
    Bo2->size = OBJSIZE_64;
    Bo2->gfxIndex = nextAvailableTileIdx;
    Bo2->priority = OBJPRIORITY_2;
    Bo2->palette = palcnt;
    Bo2->vFlip = true;
    Bo2->hFlip = true;

    Bo2 = &oam->oamBuffer[BORDER_ID+5];
    Bo2->y = 128;
    Bo2->isRotateScale = false;
    Bo2->blendMode = OBJMODE_NORMAL;
    Bo2->isMosaic = true;
    Bo2->colorMode = OBJCOLOR_16;
    Bo2->shape = OBJSHAPE_SQUARE;
    Bo2->isHidden = false;
    Bo2->x = 64;
    Bo2->size = OBJSIZE_64;
    Bo2->gfxIndex = nextAvailableTileIdx;
    Bo2->priority = OBJPRIORITY_2;
    Bo2->palette = palcnt;
    Bo2->vFlip = false;
    Bo2->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Border_2Tiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    Border_2TilesLen);
    nextAvailableTileIdx += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo1Info = &spriteInfo[BORDER_ID+6];
    SpriteEntry * Bo1 = &oam->oamBuffer[BORDER_ID+6];
    Bo1Info->oamId = BORDER_ID;
    Bo1Info->width = 64;
    Bo1Info->height = 64;
    Bo1Info->angle = 0;
    Bo1Info->entry = Bo2;
    Bo1->y = 0;
    Bo1->isRotateScale = false;
    Bo1->blendMode = OBJMODE_NORMAL;
    Bo1->isMosaic = true;
    Bo1->colorMode = OBJCOLOR_16;
    Bo1->shape = OBJSHAPE_SQUARE;
    Bo1->isHidden = false;
    Bo1->x = 192;
    Bo1->size = OBJSIZE_64;
    Bo1->gfxIndex = nextAvailableTileIdx;
    Bo1->priority = OBJPRIORITY_2;
    Bo1->palette = palcnt;
    Bo1->vFlip = true;
    Bo1->hFlip = true;

    Bo1 = &oam->oamBuffer[BORDER_ID+7];
    Bo1->y = 128;
    Bo1->isRotateScale = false;
    Bo1->blendMode = OBJMODE_NORMAL;
    Bo1->isMosaic = true;
    Bo1->colorMode = OBJCOLOR_16;
    Bo1->shape = OBJSHAPE_SQUARE;
    Bo1->isHidden = false;
    Bo1->x = 0;
    Bo1->size = OBJSIZE_64;
    Bo1->gfxIndex = nextAvailableTileIdx;
    Bo1->priority = OBJPRIORITY_2;
    Bo1->palette = palcnt;
    Bo1->vFlip = false;
    Bo1->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Border_1Tiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    Border_1TilesLen);
    nextAvailableTileIdx += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo5Info = &spriteInfo[BORDER_ID+8];
    SpriteEntry * Bo5 = &oam->oamBuffer[BORDER_ID+8];
    Bo5Info->oamId = BORDER_ID;
    Bo5Info->width = 64;
    Bo5Info->height = 64;
    Bo5Info->angle = 0;
    Bo5Info->entry = Bo2;
    Bo5->y = 64;
    Bo5->isRotateScale = false;
    Bo5->blendMode = OBJMODE_NORMAL;
    Bo5->isMosaic = true;
    Bo5->colorMode = OBJCOLOR_16;
    Bo5->shape = OBJSHAPE_SQUARE;
    Bo5->isHidden = false;
    Bo5->x = 0;
    Bo5->size = OBJSIZE_64;
    Bo5->gfxIndex = nextAvailableTileIdx;
    Bo5->priority = OBJPRIORITY_2;
    Bo5->palette = palcnt;
    Bo5->vFlip = true;
    Bo5->hFlip = true;

    Bo5 = &oam->oamBuffer[BORDER_ID+9];
    Bo5->y = 64;
    Bo5->isRotateScale = false;
    Bo5->blendMode = OBJMODE_NORMAL;
    Bo5->isMosaic = true;
    Bo5->colorMode = OBJCOLOR_16;
    Bo5->shape = OBJSHAPE_SQUARE;
    Bo5->isHidden = false;
    Bo5->x = 192;
    Bo5->size = OBJSIZE_64;
    Bo5->gfxIndex = nextAvailableTileIdx;
    Bo5->priority = OBJPRIORITY_2;
    Bo5->palette = palcnt;
    Bo5->vFlip = false;
    Bo5->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    Border_5Tiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    Border_5TilesLen);
    nextAvailableTileIdx += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;

    SpriteInfo * SQCHAInfo = &spriteInfo[SQCH_ID];
    SpriteEntry * SQCHA = &oam->oamBuffer[SQCH_ID];
    SQCHAInfo->oamId = SQCH_ID;
    SQCHAInfo->width = 16;
    SQCHAInfo->height = 16;
    SQCHAInfo->angle = 0;
    SQCHAInfo->entry = SQCHA;
    SQCHA->y = acMapPoint.second;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_SQUARE;
    SQCHA->isHidden = true;
    SQCHA->x = acMapPoint.first;
    SQCHA->size = OBJSIZE_16;
    SQCHA->gfxIndex = nextAvailableTileIdx;
    SQCHA->priority = OBJPRIORITY_0;
    SQCHA->palette = palcnt;


    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    ChSq_aPal,    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,    ChSq_aTiles,    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],    ChSq_aTilesLen);
    nextAvailableTileIdx += ChSq_aTilesLen / BYTES_PER_16_COLOR_TILE;
    
    SQCHA = &oam->oamBuffer[SQCH_ID+1];
    SQCHA->y = acMapPoint.second;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_SQUARE;
    SQCHA->isHidden = true;
    SQCHA->x = acMapPoint.first;
    SQCHA->size = OBJSIZE_16;
    SQCHA->gfxIndex = nextAvailableTileIdx;
    SQCHA->priority = OBJPRIORITY_0;
    SQCHA->palette = palcnt;
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    ChSq_bTiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    ChSq_bTilesLen);

    nextAvailableTileIdx += ChSq_bTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;
    return nextAvailableTileIdx;
}
void setMainSpriteVisibility(bool hidden){
    for(int i = PKMN_ID; i<= NAV_ID; ++i)
    if(i == PKMN_ID && !SAV.hasPKMN)
    setSpriteVisibility(&oam->oamBuffer[i],true);
    else
    setSpriteVisibility(&oam->oamBuffer[i],hidden);
    updateOAMSub(oam);
}

void scrnloader::draw(int m){
    pos = m;
    SpriteEntry * back = &oam->oamBuffer[BACK_ID];
    setSpriteVisibility(back,false);
    if(m== 0){
        if(!BGs[BG_ind].load_from_rom){
            dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
            dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
        }
        else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
            dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
            dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
            BG_ind = 0;
        }
        setMainSpriteVisibility(true);
    }
    else if(m== 1){
        loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","BottomScreen2");
        consoleSelect(&Bottom);
        consoleSetWindow(&Bottom,4,1,12,1);
        printf(" Hoenn");
        acMapRegion = HOENN;
        setMainSpriteVisibility(true);
    }
    else if(m== 2){
        loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","BottomScreen3");
        consoleSelect(&Bottom);
        consoleSetWindow(&Bottom,4,1,12,1);
        printf(" Kanto");
        acMapRegion = KANTO;
        setMainSpriteVisibility(true);
    }
    else if(m== 3){
        loadPictureSub(bgGetGfxPtr(bg3sub),"nitro:/PICS/","BottomScreen2_BG3_KJ");
        consoleSelect(&Bottom);
        consoleSetWindow(&Bottom,4,1,12,1);
        printf(" Johto");
        acMapRegion = JOHTO;
        setMainSpriteVisibility(true);
    }
    else{		 
        if(!BGs[BG_ind].load_from_rom){
            dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
            dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
        }
        else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
            dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
            dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
            BG_ind = 0;
        }
        setSpriteVisibility(back,true);
        setMainSpriteVisibility(false);
        /*initOAMTableSub(oam);
        initMainSprites(oam,spriteInfo);*/
    }
    updateOAMSub(oam);
}
void scrnloader::init(){	
    //initVideoSub();
    initOAMTableSub(oam);
    initMainSprites(oam,spriteInfo);
    this->draw(this->pos);
}

void drawPKMNIcon(OAMTable* oam,SpriteInfo* spriteInfo,const int& pkmn_no,const int posX,const int posY,
int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen = true){
    
    static const int OFFSET_MULTIPLIER = 32 / sizeof(SPRITE_GFX_SUB[0]);
    SpriteInfo *ItemInfo = &spriteInfo[++oamIndex];
    SpriteEntry *Item = &oam->oamBuffer[oamIndex];
    ItemInfo->oamId = oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = nextAvailableTileIdx;
    Item->priority = subScreen? OBJPRIORITY_1:OBJPRIORITY_0;
    Item->palette = palcnt;
    Item->x = posX;
    Item->y = posY;
    char pt[100];
    sprintf(pt, "Icon_%d",pkmn_no);
    if(subScreen){
        if(!loadSpriteSub(ItemInfo,"nitro:/PICS/ICONS/",pt,128,16)){
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemPal,
            &SPRITE_PALETTE_SUB[palcnt * 16],
            32);
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemTiles,
            &SPRITE_GFX_SUB[ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER],
            NoItemTilesLen);
        }
    }
    else{
        if(!loadSprite(ItemInfo,"nitro:/PICS/ICONS/",pt,128,16)){
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemPal,
            &SPRITE_PALETTE[palcnt * 16],
            32);
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemTiles,
            &SPRITE_GFX[ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER],
            NoItemTilesLen);
        }
    }
    nextAvailableTileIdx +=  512 / 32;
    ++palcnt;
}
void drawItemIcon(OAMTable* oam,SpriteInfo* spriteInfo,const std::string& item_name,const int posX,const int posY,
int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen){
    
    static const int OFFSET_MULTIPLIER = 32 / sizeof(SPRITE_GFX_SUB[0]);
    SpriteInfo *ItemInfo = &spriteInfo[++oamIndex];
    SpriteEntry *Item = &oam->oamBuffer[oamIndex];
    ItemInfo->oamId = oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = nextAvailableTileIdx;
    Item->priority = subScreen? OBJPRIORITY_1:OBJPRIORITY_0;
    Item->palette = palcnt;
    Item->x = posX;
    Item->y = posY;
    if(subScreen){
        if(!loadSpriteSub(ItemInfo,"nitro:/PICS/SPRITES/ITEMS/",item_name.c_str(),128,16)){
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemPal,
            &SPRITE_PALETTE_SUB[palcnt * 16],
            32);
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemTiles,
            &SPRITE_GFX_SUB[ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER],
            NoItemTilesLen);
        }
    }
    else{
        if(!loadSprite(ItemInfo,"nitro:/PICS/SPRITES/ITEMS/",item_name.c_str(),128,16)){
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemPal,
            &SPRITE_PALETTE[palcnt * 16],
            32);
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
            NoItemTiles,
            &SPRITE_GFX[ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER],
            NoItemTilesLen);
        }
    }
    nextAvailableTileIdx +=  512 / 32;
    ++palcnt;
}
void initTop()
{	
    consoleSelect(&Top);
    printf("\x1b[39m");	int a = 0,b= 0,c = 0;
    initOAMTable(oamTop);
    for (size_t i = 0; i < SAV.PKMN_team.size(); i++)
    {
        if(i%2 == 0){
            drawPKMNIcon(oamTop,spriteInfoTop,SAV.PKMN_team[i].boxdata.SPEC,borders[i][0]*8-28,borders[i][1]*8,a,b,c,false);
            BATTLE::displayHP(100,101,borders[i][0]*8-13,borders[i][1]*8+8-(i!= 2 ? 4:0),142+2*i,143+2*i,false,true);   
            BATTLE::displayHP(100,100-SAV.PKMN_team[i].stats.acHP*100/SAV.PKMN_team[i].stats.maxHP,borders[i][0]*8-13,borders[i][1]*8+8-(i!= 2? 4:0),142+2*i,143+2*i,false,true); 
        }
        else{
            drawPKMNIcon(oamTop,spriteInfoTop,SAV.PKMN_team[i].boxdata.SPEC,borders[i][0]*8+76,borders[i][1]*8,a,b,c,false);
            BATTLE::displayHP(100,101,borders[i][0]*8+63,borders[i][1]*8+8-(i!= 3 ? 4:0),142+2*i,143+2*i,false,true);   
            BATTLE::displayHP(100,100-SAV.PKMN_team[i].stats.acHP*100/SAV.PKMN_team[i].stats.maxHP,borders[i][0]*8+63,borders[i][1]*8+8-(i!= 3 ? 4:0),142+2*i,143+2*i,false,true); 
        }
        updateOAM(oamTop);
        consoleSetWindow(&Top,borders[i][0],borders[i][1],12,6);		
        wprintf(SAV.PKMN_team[i].boxdata.Name);

        //if(SAV.PKMN_team[i].Name.length < 10)
        printf("\n");
        printf(POKEMON::PKMNDATA::getDisplayName(SAV.PKMN_team[i].boxdata.SPEC));

        printf("\n\n");
        printf("\n""%hi""/%hi KP\n",SAV.PKMN_team[i].stats.acHP,SAV.PKMN_team[i].stats.maxHP);
        printf(ItemList[SAV.PKMN_team[i].boxdata.getItem()].getDisplayName().c_str());
    }
    updateOAM(oamTop);
}
void clearTop()
{
    consoleSelect(&Top);
    for (size_t i = 0; i < SAV.PKMN_team.size(); i++)
    {
        consoleSetWindow(&Top,borders[i][0],borders[i][1],12,6);		
        consoleClear();
    }
    initOAMTable(oamTop);
}

void scrnloader::run_pkmn()
{		
    vramSetup();
    videoSetMode(MODE_5_2D |DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );	
    Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2,0, true ,true);
    consoleSetFont(&Top, &cfont);

    Bottom = *consoleInit(&Bottom,  0, BgType_Text4bpp, BgSize_T_256x256, 2,0, false,true );
    consoleSetFont(&Bottom, &cfont);

    touchPosition touch;
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","PKMNScreen");
    if(!BGs[BG_ind].load_from_rom){
        dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
    }
    else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
        dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
        BG_ind = 0;
    }
    
    int acIn= 0,max = SAV.PKMN_team.size();
    consoleSelect(&Top);
    consoleClear();
    initTop();
    consoleSetWindow(&Top,positions[acIn][0],positions[acIn][1],2,2);
    if(acIn&1)
    printf(">");
    else
    printf("<");
    
    SpriteEntry * back = &oam->oamBuffer[BACK_ID];
    SpriteEntry * save = &oam->oamBuffer[SAVE_ID];
    setSpriteVisibility(back,false);
    setSpriteVisibility(save,true);
    setMainSpriteVisibility(true);
    oam->oamBuffer[8].isHidden = false;
    oam->oamBuffer[8].x = SCREEN_WIDTH / 2 - 16;
    oam->oamBuffer[8].y = SCREEN_HEIGHT / 2 - 16;
    updateOAMSub(oam);

    while(1)
    {		
        updateTime();
        consoleSelect(&Top);
        updateOAMSub(oam);

        swiWaitForVBlank();
        scanKeys();	
        int pressed = keysUp();
        touchRead(&touch);

        if ( touch.px>224 && touch.py>164)
        {
            while(1)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            break;
        }
        else if(pressed & KEY_A || (sqrt(sq(touch.px-128)+sq(touch.py-96)) <= 16))
        {
            while(2)
            {
                scanKeys();
                if(keysUp() & KEY_TOUCH || keysUp() & KEY_A)
                break;
            }
            consoleClear();
            clearTop();
            oam->oamBuffer[8].isHidden = true;
            updateOAMSub(oam);
            swiWaitForVBlank();
            while (1)
            {
                int p = SAV.PKMN_team[acIn].draw();
                if(p & KEY_B)
                {
                    initOAMTableSub(oam);
                    initMainSprites(oam,spriteInfo);
                    setSpriteVisibility(back,false);
                    setSpriteVisibility(save,true);
                    setMainSpriteVisibility(true);
                    oam->oamBuffer[8].isHidden = false;
                    oam->oamBuffer[8].x = SCREEN_WIDTH / 2 - 16;
                    oam->oamBuffer[8].y = SCREEN_HEIGHT / 2 - 16;
                    
                    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","PKMNScreen");
                    if(!BGs[BG_ind].load_from_rom){
                        dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
                        dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
                    }
                    else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
                        dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
                        dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
                        BG_ind = 0;
                    }

                    acIn %= max;
                    consoleSelect(&Top);
                    consoleClear();
                    initTop();
                    consoleSetWindow(&Top,positions[acIn][0],positions[acIn][1],2,2);
                    if(acIn&1)
                    printf(">");
                    else
                    printf("<");

                    break;
                }
                else if(p & KEY_UP)
                {
                    /*initOAMTableSub(oam);
                    initMainSprites(oam,spriteInfo);
                    setSpriteVisibility(back,true);
                    setSpriteVisibility(save,true);
                    setMainSpriteVisibility(true);*/
                    if(--acIn <= -1)
                    acIn = max-1;
                }
                else if(p & KEY_DOWN)
                {
                    /* initOAMTableSub(oam);
                    initMainSprites(oam,spriteInfo);
                    setSpriteVisibility(back,true);
                    setSpriteVisibility(save,true);
                    setMainSpriteVisibility(true);*/
                    if(++acIn >= max)
                    acIn = 0;
                }

            }
        }
        //else if (touch.px>117 && touch.py>52&&touch.px<141 && touch.py<75 && !(touch==lstTch) && bbb > 20){
        //	//Item Geben
        //	mbox("Nicht Implementiert!");			
        //	dmaCopy( BGs[BG_ind].PKMNMenu, bgGetGfxPtr(bg3sub), 256*256 );
        //	dmaCopy( BGs[BG_ind].PKMNMenuPal,BG_PALETTE_SUB, 256*2);
        //}
        //else if (touch.px>116 && touch.py>119&&touch.px<140 && touch.py<143 && !(touch==lstTch) && bbb > 20)
        //{  
        //	//Item nehmen			
        //	mbox("Nicht Implementiert!");			
        //	dmaCopy( BGs[BG_ind].PKMNMenu, bgGetGfxPtr(bg3sub), 256*256 );
        //	dmaCopy( BGs[BG_ind].PKMNMenuPal,BG_PALETTE_SUB, 256*2);
        //}
        else if (pressed & KEY_UP)
        {
            if(--acIn <= -1)
            acIn = max-1;
            consoleClear();
            consoleSetWindow(&Top,positions[acIn][0],positions[acIn][1],2,2);
            if(acIn&1)
            printf(">");
            else
            printf("<");
        }
        else if (pressed & KEY_DOWN)
        {
            if(++acIn >= max)
            acIn = 0;
            consoleClear();
            consoleSetWindow(&Top,positions[acIn][0],positions[acIn][1],2,2);
            if(acIn&1)
            printf(">");
            else
            printf("<");
        }
    }
    for (int i = 0; i < max; i++)
    {
        consoleSetWindow(&Top,positions[i][0],positions[i][1],2,2);
        consoleClear();
    }
    clearTop();

    initOAMTableSub(oam);
    initMainSprites(oam,spriteInfo);
    setSpriteVisibility(back,true);
    setSpriteVisibility(save,false);
    setMainSpriteVisibility(false);
    oam->oamBuffer[8].isHidden = true;
}
const int dexsppos[2][9] = {{160,128,96,19,6,120,158,196,8},{-16,0,24,138,173,108,126,144,32}};
void initDexSprites(OAMTable* oam, SpriteInfo* spriteInfo,int& oamIndex,int& palcnt, int& nextAvailableTileIdx){
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX_SUB[0]);
    /* Keep track of the available tiles */
    nextAvailableTileIdx = 0;
    oamIndex = 0;
    palcnt = 0;
    SpriteInfo * backInfo = &spriteInfo[oamIndex];
    SpriteEntry * back = &oam->oamBuffer[oamIndex];
    backInfo->oamId = oamIndex;
    backInfo->width = 32;
    backInfo->height = 32;
    backInfo->angle = 0;
    backInfo->entry = back;
    back->y = SCREEN_HEIGHT - 28;
    back->isRotateScale = false;
    back->blendMode = OBJMODE_NORMAL;
    back->isMosaic = false;
    back->isHidden = false;
    back->colorMode = OBJCOLOR_16;
    back->shape = OBJSHAPE_SQUARE;
    back->x = SCREEN_WIDTH - 28;
    back->rotationIndex = backInfo->oamId;
    back->size = OBJSIZE_32;
    back->gfxIndex = nextAvailableTileIdx;
    back->priority = OBJPRIORITY_0;
    back->palette = backInfo->oamId;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BackTiles,
    &SPRITE_GFX_SUB[back->gfxIndex * OFFSET_MULTIPLIER],
    BackTilesLen);
    nextAvailableTileIdx += BackTilesLen / BYTES_PER_16_COLOR_TILE;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BackPal,
    &SPRITE_PALETTE_SUB[backInfo->oamId * COLORS_PER_PALETTE],
    32);
    palcnt = backInfo->oamId + 1;

    SpriteInfo * Bo4Info = &spriteInfo[++oamIndex];
    SpriteEntry * Bo4 = &oam->oamBuffer[oamIndex];
    Bo4Info->oamId = oamIndex;
    Bo4Info->width = 64;
    Bo4Info->height = 64;
    Bo4Info->angle = 0;
    Bo4Info->entry = Bo4;
    Bo4->y = 0;
    Bo4->isRotateScale = false;
    Bo4->blendMode = OBJMODE_NORMAL;
    Bo4->isMosaic = false;
    Bo4->colorMode = OBJCOLOR_16;
    Bo4->shape = OBJSHAPE_SQUARE;
    Bo4->isHidden = false;
    Bo4->x = 0;
    Bo4->size = OBJSIZE_64;
    Bo4->gfxIndex = nextAvailableTileIdx;
    Bo4->priority = OBJPRIORITY_2;
    Bo4->palette = palcnt;
    Bo4->vFlip = true;
    Bo4->hFlip = true;

    Bo4 = &oam->oamBuffer[++oamIndex];
    Bo4->y = 128;
    Bo4->isRotateScale = false;
    Bo4->blendMode = OBJMODE_NORMAL;
    Bo4->isMosaic = false;
    Bo4->colorMode = OBJCOLOR_16;
    Bo4->shape = OBJSHAPE_SQUARE;
    Bo4->isHidden = false;
    Bo4->x = 192;
    Bo4->size = OBJSIZE_64;
    Bo4->gfxIndex = nextAvailableTileIdx;
    Bo4->priority = OBJPRIORITY_2;
    Bo4->palette = palcnt;
    Bo4->vFlip = false;
    Bo4->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Border_4Pal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Border_4Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    Border_4TilesLen);
    
    nextAvailableTileIdx += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo3Info = &spriteInfo[++oamIndex];
    SpriteEntry * Bo3 = &oam->oamBuffer[oamIndex];
    Bo3Info->oamId = 3;
    Bo3Info->width = 64;
    Bo3Info->height = 64;
    Bo3Info->angle = 0;
    Bo3Info->entry = Bo3;
    Bo3->y = 0;
    Bo3->isRotateScale = false;
    Bo3->blendMode = OBJMODE_NORMAL;
    Bo3->isMosaic = false;
    Bo3->colorMode = OBJCOLOR_16;
    Bo3->shape = OBJSHAPE_SQUARE;
    Bo3->isHidden = false;
    Bo3->x = 64;
    Bo3->size = OBJSIZE_64;
    Bo3->gfxIndex = nextAvailableTileIdx;
    Bo3->priority = OBJPRIORITY_2;
    Bo3->palette = palcnt;
    Bo3->vFlip = true;
    Bo3->hFlip = true;

    Bo3 = &oam->oamBuffer[++oamIndex];
    Bo3->y = 128;
    Bo3->isRotateScale = false;
    Bo3->blendMode = OBJMODE_NORMAL;
    Bo3->isMosaic = false;
    Bo3->colorMode = OBJCOLOR_16;
    Bo3->shape = OBJSHAPE_SQUARE;
    Bo3->isHidden = false;
    Bo3->x = 128;
    Bo3->size = OBJSIZE_64;
    Bo3->gfxIndex = nextAvailableTileIdx;
    Bo3->priority = OBJPRIORITY_2;
    Bo3->palette = palcnt;
    Bo3->vFlip = false;
    Bo3->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Border_3Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    Border_3TilesLen);
    
    nextAvailableTileIdx += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo2Info = &spriteInfo[++oamIndex];
    SpriteEntry * Bo2 = &oam->oamBuffer[oamIndex];
    Bo2Info->oamId = 3;
    Bo2Info->width = 64;
    Bo2Info->height = 64;
    Bo2Info->angle = 0;
    Bo2Info->entry = Bo2;
    Bo2->y = 0;
    Bo2->isRotateScale = false;
    Bo2->blendMode = OBJMODE_NORMAL;
    Bo2->isMosaic = false;
    Bo2->colorMode = OBJCOLOR_16;
    Bo2->shape = OBJSHAPE_SQUARE;
    Bo2->isHidden = false;
    Bo2->x = 128;
    Bo2->size = OBJSIZE_64;
    Bo2->gfxIndex = nextAvailableTileIdx;
    Bo2->priority = OBJPRIORITY_2;
    Bo2->palette = palcnt;
    Bo2->vFlip = true;
    Bo2->hFlip = true;

    Bo2 = &oam->oamBuffer[++oamIndex];
    Bo2->y = 128;
    Bo2->isRotateScale = false;
    Bo2->blendMode = OBJMODE_NORMAL;
    Bo2->isMosaic = false;
    Bo2->colorMode = OBJCOLOR_16;
    Bo2->shape = OBJSHAPE_SQUARE;
    Bo2->isHidden = false;
    Bo2->x = 64;
    Bo2->size = OBJSIZE_64;
    Bo2->gfxIndex = nextAvailableTileIdx;
    Bo2->priority = OBJPRIORITY_2;
    Bo2->palette = palcnt;
    Bo2->vFlip = false;
    Bo2->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Border_2Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    Border_2TilesLen);
    nextAvailableTileIdx += Border_2TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo1Info = &spriteInfo[++oamIndex];
    SpriteEntry * Bo1 = &oam->oamBuffer[oamIndex];
    Bo1Info->oamId = 3;
    Bo1Info->width = 64;
    Bo1Info->height = 64;
    Bo1Info->angle = 0;
    Bo1Info->entry = Bo1;
    Bo1->y = 0;
    Bo1->isRotateScale = false;
    Bo1->blendMode = OBJMODE_NORMAL;
    Bo1->isMosaic = false;
    Bo1->colorMode = OBJCOLOR_16;
    Bo1->shape = OBJSHAPE_SQUARE;
    Bo1->isHidden = false;
    Bo1->x = 192;
    Bo1->size = OBJSIZE_64;
    Bo1->gfxIndex = nextAvailableTileIdx;
    Bo1->priority = OBJPRIORITY_2;
    Bo1->palette = palcnt;
    Bo1->vFlip = true;
    Bo1->hFlip = true;

    Bo1 = &oam->oamBuffer[++oamIndex];
    Bo1->y = 128;
    Bo1->isRotateScale = false;
    Bo1->blendMode = OBJMODE_NORMAL;
    Bo1->isMosaic = false;
    Bo1->colorMode = OBJCOLOR_16;
    Bo1->shape = OBJSHAPE_SQUARE;
    Bo1->isHidden = false;
    Bo1->x = 0;
    Bo1->size = OBJSIZE_64;
    Bo1->gfxIndex = nextAvailableTileIdx;
    Bo1->priority = OBJPRIORITY_2;
    Bo1->palette = palcnt;
    Bo1->vFlip = false;
    Bo1->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Border_1Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    Border_1TilesLen);
    nextAvailableTileIdx += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;
    
    SpriteInfo * Bo5Info = &spriteInfo[++oamIndex];
    SpriteEntry * Bo5 = &oam->oamBuffer[oamIndex];
    Bo5Info->oamId = 3;
    Bo5Info->width = 64;
    Bo5Info->height = 64;
    Bo5Info->angle = 0;
    Bo5Info->entry = Bo5;
    Bo5->y = 64;
    Bo5->isRotateScale = false;
    Bo5->blendMode = OBJMODE_NORMAL;
    Bo5->isMosaic = false;
    Bo5->colorMode = OBJCOLOR_16;
    Bo5->shape = OBJSHAPE_SQUARE;
    Bo5->isHidden = false;
    Bo5->x = 0;
    Bo5->size = OBJSIZE_64;
    Bo5->gfxIndex = nextAvailableTileIdx;
    Bo5->priority = OBJPRIORITY_2;
    Bo5->palette = palcnt;
    Bo5->vFlip = true;
    Bo5->hFlip = true;

    Bo5 = &oam->oamBuffer[++oamIndex];
    Bo5->y = 64;
    Bo5->isRotateScale = false;
    Bo5->blendMode = OBJMODE_NORMAL;
    Bo5->isMosaic = false;
    Bo5->colorMode = OBJCOLOR_16;
    Bo5->shape = OBJSHAPE_SQUARE;
    Bo5->isHidden = false;
    Bo5->x = 192;
    Bo5->size = OBJSIZE_64;
    Bo5->gfxIndex = nextAvailableTileIdx;
    Bo5->priority = OBJPRIORITY_2;
    Bo5->palette = palcnt;
    Bo5->vFlip = false;
    Bo5->hFlip = false;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    Border_5Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    Border_5TilesLen);
    nextAvailableTileIdx += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;

    ++palcnt;
    oamIndex = 10;
    for(int i= 0; i< 8; ++i){
        SpriteInfo * BagSprInfo = &spriteInfo[++oamIndex];
        SpriteEntry * BagSpr = &oam->oamBuffer[oamIndex];
        BagSprInfo->oamId = 11;
        BagSprInfo->width = 32;
        BagSprInfo->height = 32;
        BagSprInfo->angle = 0;
        BagSprInfo->entry = BagSpr;
        BagSpr->y = dexsppos[1][i];
        BagSpr->isRotateScale = false;
        BagSpr->blendMode = OBJMODE_NORMAL;
        BagSpr->isMosaic = false;
        BagSpr->colorMode = OBJCOLOR_16;
        BagSpr->shape = OBJSHAPE_SQUARE;
        BagSpr->isHidden = false;
        BagSpr->x = dexsppos[0][i];
        BagSpr->size = OBJSIZE_32;
        BagSpr->gfxIndex = nextAvailableTileIdx;
        BagSpr->priority = OBJPRIORITY_2;
        BagSpr->palette = palcnt;
    }
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BagSprPal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BagSprTiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    BagSprTilesLen); 
    
    nextAvailableTileIdx += BagSprTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;
    SpriteInfo * B2Info = &spriteInfo[++oamIndex];
    SpriteEntry * B2 = &oam->oamBuffer[oamIndex];
    B2Info->oamId = 12;
    B2Info->width = 64;
    B2Info->height = 64;
    B2Info->angle = 0;
    B2Info->entry = B2;
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = dexsppos[0][8];
    B2->y = dexsppos[1][8];
    
    B2 = &oam->oamBuffer[++oamIndex];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = dexsppos[0][8]+64;
    B2->y = dexsppos[1][8];
    B2->hFlip = true;
    
    B2 = &oam->oamBuffer[++oamIndex];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = dexsppos[0][8];
    B2->y = dexsppos[1][8]+64;
    B2->hFlip = false;
    B2->vFlip = true;

    B2 = &oam->oamBuffer[++oamIndex];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = dexsppos[0][8]+64;
    B2->y = dexsppos[1][8]+64;
    B2->hFlip = true;
    B2->vFlip = true;
    
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BigCirc1Pal,
    &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BigCirc1Tiles,
    &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    BigCirc1TilesLen);
    nextAvailableTileIdx += BigCirc1TilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;
    updateOAMSub(oam);
}

void drawTypeIcon (OAMTable *oam, SpriteInfo * spriteInfo, int& oamIndex, int& palcnt, int & nextTile, Type t ,int x,int y, bool bottom){
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX_SUB[0]);

    SpriteInfo * type1Info = &spriteInfo[++oamIndex];
    SpriteEntry * type1 = &oam->oamBuffer[oamIndex];
    type1Info->oamId = oamIndex;
    type1Info->width = 32;
    type1Info->height = 16;
    type1Info->angle = 0;
    type1Info->entry = type1;
    type1->y = y;
    type1->isRotateScale = false;
    type1->isHidden = false;
    type1->blendMode = OBJMODE_NORMAL;
    type1->isMosaic = false;
    type1->colorMode = OBJCOLOR_16;
    type1->shape = OBJSHAPE_WIDE;
    type1->x = x;
    type1->size = OBJSIZE_32;
    type1->gfxIndex = nextTile;
    type1->priority = OBJPRIORITY_0;
    type1->palette = palcnt;
    
    if(bottom){
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        TypePals[t],
        &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
        32);
        /* Copy the sprite graphics to sprite graphics memory */
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        TypeTiles[t],
        &SPRITE_GFX_SUB[nextTile * OFFSET_MULTIPLIER],
        KampfTilesLen);
    }
    else{
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        TypePals[t],
        &SPRITE_PALETTE[palcnt * COLORS_PER_PALETTE],
        32);
        /* Copy the sprite graphics to sprite graphics memory */
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        TypeTiles[t],
        &SPRITE_GFX[nextTile * OFFSET_MULTIPLIER],
        KampfTilesLen);
    }
    nextTile += KampfTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;
}

void formes(OAMTable *oam, SpriteInfo * spriteInfo, int& oamIndex, int& palcnt, int & nextAvailableTileIdx, int pkmn, bool female,POKEMON::Gender_Type g){
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; 
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE / sizeof(SPRITE_GFX[0]);
    
    SpriteInfo * B2Info = &spriteInfo[++oamIndex];
    SpriteEntry * B2 = &oam->oamBuffer[oamIndex];
    B2Info->oamId = 12;
    B2Info->width = 64;
    B2Info->height = 64;
    B2Info->angle = 0;
    B2Info->entry = B2;
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = 64;
    B2->y = 48;
    spriteInfo[++oamIndex] = *B2Info;
    oam->oamBuffer[oamIndex] = *B2;
    
    B2 = &oam->oamBuffer[++oamIndex];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = 64+64;
    B2->y = 48;
    B2->hFlip = true;
    spriteInfo[++oamIndex] = *B2Info;
    oam->oamBuffer[oamIndex] = *B2;
    
    B2 = &oam->oamBuffer[++oamIndex];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = 64;
    B2->y = 48+64;
    B2->hFlip = false;
    B2->vFlip = true;
    spriteInfo[++oamIndex] = *B2Info;
    oam->oamBuffer[oamIndex] = *B2;

    B2 = &oam->oamBuffer[++oamIndex];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = false;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = nextAvailableTileIdx;
    B2->priority = OBJPRIORITY_1;
    B2->palette = palcnt;
    B2->x = 64+64;
    B2->y = 48+64;
    B2->hFlip = true;
    B2->vFlip = true;
    spriteInfo[++oamIndex] = *B2Info;
    oam->oamBuffer[oamIndex] = *B2;
    
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BigCirc1Pal,
    &SPRITE_PALETTE[palcnt * COLORS_PER_PALETTE],
    32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
    BigCirc1Tiles,
    &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER],
    BigCirc1TilesLen);
    nextAvailableTileIdx += BigCirc1TilesLen / BYTES_PER_16_COLOR_TILE;
    if(pkmn == 0){
        updateOAM(oam);
        loadPKMNSprite(oamTop,spriteInfoTop,"nitro:/PICS/SPRITES/PKMN/",pkmn,80,64,oamIndex,palcnt,nextAvailableTileIdx,false,false,female);
        return;
    }
    else{
        for(int i = oamIndex; i > oamIndex - 8; --i)
            if(i%2)
                oam->oamBuffer[i].x -= 50;
            else
                oam->oamBuffer[i].x += 50;
        updateOAM(oam);
        if(!loadPKMNSprite(oamTop,spriteInfoTop,"nitro:/PICS/SPRITES/PKMN/",pkmn,80 - 50,64,oamIndex,palcnt,nextAvailableTileIdx,false,false,female,true))
            loadPKMNSprite(oamTop,spriteInfoTop,"nitro:/PICS/SPRITES/PKMN/",pkmn,80 - 50,64,oamIndex,palcnt,nextAvailableTileIdx,false,false,!female,true);
        --palcnt;
        if(!loadPKMNSprite(oamTop,spriteInfoTop,"nitro:/PICS/SPRITES/PKMN/",pkmn,80 + 50,64,oamIndex,palcnt,nextAvailableTileIdx,false,true,female))
            loadPKMNSprite(oamTop,spriteInfoTop,"nitro:/PICS/SPRITES/PKMN/",pkmn,80 + 50,64,oamIndex,palcnt,nextAvailableTileIdx,false,true,!female);
    }
    SpriteInfo * C1Info = &spriteInfo[++oamIndex];
    SpriteEntry * C1 = &oam->oamBuffer[oamIndex];
    C1Info->oamId = oamIndex;
    C1Info->width = 64;
    C1Info->height = 32;
    C1Info->angle = 0;
    C1Info->entry = C1;
    C1->y = 168;
    C1->isRotateScale = false;
    C1->blendMode = OBJMODE_NORMAL;
    C1->isMosaic = false;
    C1->colorMode = OBJCOLOR_16;
    C1->shape = OBJSHAPE_WIDE;
    C1->isHidden = false;
    C1->x = -8;
    C1->size = OBJSIZE_64;
    C1->gfxIndex = nextAvailableTileIdx;
        
    C1->priority = OBJPRIORITY_2;
    C1->palette = palcnt;
    spriteInfo[++oamIndex] = *C1Info;
    oam->oamBuffer[oamIndex] = *C1;
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, Choice_1Tiles, &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER], Choice_1TilesLen);
    nextAvailableTileIdx += Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
        
    SpriteInfo * C3Info = &spriteInfo[++oamIndex];
    SpriteEntry * C3 = &oam->oamBuffer[oamIndex];
    C3Info->oamId = oamIndex;
    C3Info->width = 64;
    C3Info->height = 32;
    C3Info->angle = 0;
    C3Info->entry = C3;
    C3->y = 168;
    C3->isRotateScale = false;
    C3->blendMode = OBJMODE_NORMAL;
    C3->isMosaic = false;
    C3->colorMode = OBJCOLOR_16;
    C3->shape = OBJSHAPE_WIDE;
    C3->isHidden = false;
    C3->x = 64-8;
    C3->size = OBJSIZE_64;
    C3->gfxIndex = nextAvailableTileIdx;
    C3->priority = OBJPRIORITY_2;
    C3->palette = palcnt;
    spriteInfo[++oamIndex] = *C3Info;
    oam->oamBuffer[oamIndex] = *C3;

    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, Choice_1Pal, &SPRITE_PALETTE[palcnt * COLORS_PER_PALETTE], 32);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL, Choice_3Tiles, &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER], Choice_3TilesLen);
    nextAvailableTileIdx += Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
    if(pkmn == 0 || g == POKEMON::GENDERLESS)
        for(int i = oamIndex; i > oamIndex - 4; --i)
            oam->oamBuffer[i].isHidden = true;
    else{
        oam->oamBuffer[oamIndex].x += 272-128;
        oam->oamBuffer[oamIndex-2].x += 272-128;
        consoleSetWindow(&Top,2,22,31,10);
        if((female || g == POKEMON::FEMALE) && g != POKEMON::MALE)
            printf(" weiblich          weiblich\n                 (schillernd)");
        else
            printf(" m\x84""nnlich          m\x84""nnlich\n                 (schillernd)");
    }
    updateOAM(oam);
}
void drawTopDexPage(int page, int pkmn,int forme = 0){
    initOAMTable(oamTop);
    consoleSetWindow(&Top,0,0,32,24);
    consoleSelect(&Top);
    consoleClear();
    int a = 0, b = 0,c = 0;
    POKEMON::PKMNDATA::PKMNDATA acpkmndata;
    POKEMON::Gender_Type acG = POKEMON::GENDERLESS;
    int newformepkmn = pkmn;
    if(page < 4){
        POKEMON::PKMNDATA::getAll(pkmn,acpkmndata);
        dmaCopy(DexTopBitmap, bgGetGfxPtr(bg3), 256*192);
        dmaCopy(DexTopPal, BG_PALETTE, 32); 
        if(page == 2)
            memset(bgGetGfxPtr(bg3)+7168,1,256*192-14336);
        if(SAV.inDex[pkmn - 1]){
            BG_PALETTE[1] = POKEMON::PKMNDATA::getColor(acpkmndata.Types[0]);

            bool isFixed = (acpkmndata.gender == POKEMON::GENDERLESS) || (acpkmndata.gender == POKEMON::MALE) || (acpkmndata.gender == POKEMON::FEMALE);
            forme %= acpkmndata.formecnt ? ((isFixed ? 1 : 2)*acpkmndata.formecnt) : 2;
            newformepkmn = acpkmndata.formecnt ? POKEMON::PKMNDATA::getForme(pkmn,forme/(isFixed ? 1 : 2) ,0): pkmn;
            acG = acpkmndata.gender;
            if(acpkmndata.formecnt)
                POKEMON::PKMNDATA::getAll(newformepkmn,acpkmndata);

            BG_PALETTE[1] = POKEMON::PKMNDATA::getColor(acpkmndata.Types[0]);
            drawPKMNIcon(oamTop,spriteInfoTop,(pkmn == 493 || pkmn == 649) ? pkmn : newformepkmn,0,8,a,b,c,false);
            drawTypeIcon(oamTop,spriteInfoTop,a,b,c,acpkmndata.Types[0],33,35,false);
            if(acpkmndata.Types[0] != acpkmndata.Types[1])
                drawTypeIcon(oamTop,spriteInfoTop,a,b,c,acpkmndata.Types[1],65,35,false);
            updateOAM(oamTop);
            printf("\n    Du hast %i dieser PKMN.",box_of_st_pkmn[pkmn-1].size());
            printf("\n\n     %s - %s",POKEMON::PKMNDATA::getDisplayName(pkmn),POKEMON::PKMNDATA::getSpecies(pkmn));
            printf("\n\n %03i",pkmn);
        }
        else{
            printf("\n    Keine Daten vorhanden."); 
            printf("\n\n     ???????????? - %s",POKEMON::PKMNDATA::getSpecies(0));
            printf("\n\n %03i",pkmn);
        }
    }
    switch(page){
    case 0:{
            printf("\x1b[37m");
            if(SAV.inDex[pkmn - 1]){
                BG_PALETTE[42] = POKEMON::PKMNDATA::getColor(acpkmndata.Types[1]);
                for(int i= 0; i< 6; ++i){
                    font::putrec(17 + 40 * i,std::max(56, 103- acpkmndata.Bases[i] / 3),39 + 40 * i,103,false,true);
                    //font::putrec(17 + 40 * i,std::min(103, 56 + acpkmndata.Bases[i] / 3),(6* (acpkmndata.Bases[i] % 3)) + 16 + 40 * i,std::min(103, 58 + acpkmndata.Bases[i] / 3),statColor[i],false);
                }
                printf("\n\n  KP   ANG  DEF  SAN  SDF  INT");
                printf("\n\n\n\n\n\n\n GW %5.1fkg GR %6.1fm\n\n", 42 / 10.0, 4200/ 10.0);
                consoleSetWindow(&Top, 1,16,30,24);
                printf(POKEMON::PKMNDATA::getDexEntry(pkmn));
            }
            else{
                printf("\n\n  KP   ANG  DEF  SAN  SDF  INT");
                printf("\n\n\n\n\n\n\n GW ???.?kg GR ???.?m\n");
                consoleSetWindow(&Top, 1,16,30,24); 
                printf(POKEMON::PKMNDATA::getDexEntry(0));
            }
            break;
        }
    case 2:{
            if(SAV.inDex[pkmn - 1]){
                formes(oamTop,spriteInfoTop,a,b,c, newformepkmn ,forme%2,acG);
            }
            else
                formes(oamTop,spriteInfoTop,a,b,c,0,0,POKEMON::GENDERLESS);
            break;
        }
    case 4:{
            loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","BottomScreen2");
            break;
        }
    case 5:{
            loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","BottomScreen3");
            break;
        }
    case 6:{
            loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","BottomScreen2_BG3_KJ");
            break;
        }
    }
    swiWaitForVBlank();
}
void scrnloader::run_dex(int num){
    vramSetup();
    videoSetMode(MODE_5_2D |DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );		
    Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256,2,0, true ,true);
    consoleSetFont(&Top,&cfont);
    
    Bottom = *consoleInit(&Bottom,  0, BgType_Text4bpp, BgSize_T_256x256, 2,0, false,true );
    consoleSetFont(&Bottom, &cfont);
    
    touchPosition t;	int acForme = 0;
    dmaCopy(DexTopBitmap, bgGetGfxPtr(bg3), 256*256);
    dmaCopy(DexTopPal, BG_PALETTE, 32); 

    if(!BGs[BG_ind].load_from_rom){
        dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
    }
    else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
        dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
        BG_ind = 0;
    }
    initOAMTableSub(oam);
    initOAMTable(oamTop);
    int palcnt = 0,tilecnt = 0,oamInd = 0;
    initDexSprites(oam,spriteInfo,oamInd,palcnt,tilecnt);
    
    consoleSetWindow(&Bottom,0,0,32,24);
    consoleSelect(&Bottom);
    consoleClear();
    printf("Gefangen: %3i",SAV.Dex);

    int acNum = num,maxn = SAV.hasGDex ? 649 : 493,o2=oamInd,p2=palcnt,t2=tilecnt,acPage = 0,acMap = acMapRegion;

    consoleSetWindow(&Top,0,0,32,24);
    consoleSelect(&Top);
    consoleClear();
    for(int i = (acNum + maxn - 3)%maxn,j = 0; j < 5; i = (i+1)%maxn,++j)
    if(i == acNum){
        drawTopDexPage(acPage,i+1,acForme);
        loadPKMNSprite(oam,spriteInfo,"nitro:/PICS/SPRITES/PKMN/",SAV.inDex[i]?i + 1:0,dexsppos[0][8]+16,dexsppos[1][8]+16,o2,p2,t2,true);
        --j; 
        continue;
    }
    else
        drawPKMNIcon(oam,spriteInfo,SAV.inDex[i]?i + 1:0,dexsppos[0][j],dexsppos[1][j],o2,p2,t2,true);
    
    spriteInfo[16].entry->isHidden = true;
    updateOAMSub(oam);
    while(42){
        updateTime();
        swiWaitForVBlank();
        updateOAMSub(oam);
        scanKeys();
        touchRead(&t);
        int pressed = keysUp(),held = keysHeld();
        if ( (t.px>224 && t.py>164))
        {  
            while((t.px>224 && t.py>164))
            {
                if((keysUp() & KEY_TOUCH))
                break;
                scanKeys();
                swiWaitForVBlank();
                updateTime();
            }
            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,4,0,20,3);
            consoleClear();
            break;
        }
        else if((pressed & KEY_DOWN)){
            while(1)
            {
                if(keysUp() & KEY_DOWN)
                break;
                swiWaitForVBlank();
                updateTime();
                scanKeys();
            }
            o2=oamInd;p2=palcnt;t2=tilecnt;
            acNum = (acNum + 1) % maxn;
            for(int i = (acNum + maxn - 3)%maxn,j = 0; j < 5; i = (i+1)%maxn,++j)
            if(i == acNum){drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,i+1,acForme);--j;
        loadPKMNSprite(oam,spriteInfo,"nitro:/PICS/SPRITES/PKMN/",SAV.inDex[i]?i + 1:0,dexsppos[0][8]+16,dexsppos[1][8]+16,o2,p2,t2,true); continue;}
            else
            drawPKMNIcon(oam,spriteInfo,SAV.inDex[i]?i + 1:0,dexsppos[0][j],dexsppos[1][j],o2,p2,t2,true);
        }
        else if((pressed & KEY_UP)){
            while(1)
            {
                if(keysUp() & KEY_UP)
                break;
                scanKeys();
                swiWaitForVBlank();
                updateTime();
            }
            o2=oamInd;p2=palcnt;t2=tilecnt;
            acNum = (acNum + maxn - 1) % maxn;
            for(int i = (acNum + maxn - 3)%maxn,j = 0; j < 5; i = (i+1)%maxn,++j)
            if(i == acNum){ drawTopDexPage(acPage == 1 ?  4 + acMap: acPage,i+1,acForme);--j;
        loadPKMNSprite(oam,spriteInfo,"nitro:/PICS/SPRITES/PKMN/",SAV.inDex[i]?i + 1:0,dexsppos[0][8]+16,dexsppos[1][8]+16,o2,p2,t2,true); continue;}
            else
            drawPKMNIcon(oam,spriteInfo,SAV.inDex[i]?i + 1:0,dexsppos[0][j],dexsppos[1][j],o2,p2,t2,true);
        }
        else if((pressed & KEY_R)){
            while(1)
            {
                if(keysUp() & KEY_R)
                break;
                swiWaitForVBlank();
                updateTime();
                scanKeys();
            }
            o2=oamInd;p2=palcnt;t2=tilecnt;
            acNum = (acNum + 15) % maxn;
            for(int i = (acNum + maxn - 3)%maxn,j = 0; j < 5; i = (i+1)%maxn,++j)
            if(i == acNum){drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,i+1,acForme);--j;
        loadPKMNSprite(oam,spriteInfo,"nitro:/PICS/SPRITES/PKMN/",SAV.inDex[i]?i + 1:0,dexsppos[0][8]+16,dexsppos[1][8]+16,o2,p2,t2,true); continue;}
            else
            drawPKMNIcon(oam,spriteInfo,SAV.inDex[i]?i + 1:0,dexsppos[0][j],dexsppos[1][j],o2,p2,t2,true);
        }
        else if((pressed & KEY_L)){
            while(1)
            {
                if(keysUp() & KEY_L)
                break;
                scanKeys();
                swiWaitForVBlank();
                updateTime();
            }
            o2=oamInd;p2=palcnt;t2=tilecnt;
            acNum = (acNum + maxn - 15) % maxn;
            for(int i = (acNum + maxn - 3)%maxn,j = 0; j < 5; i = (i+1)%maxn,++j)
            if(i == acNum){drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,i+1,acForme);--j;
        loadPKMNSprite(oam,spriteInfo,"nitro:/PICS/SPRITES/PKMN/",SAV.inDex[i]?i + 1:0,dexsppos[0][8]+16,dexsppos[1][8]+16,o2,p2,t2,true); continue;}
            else
            drawPKMNIcon(oam,spriteInfo,SAV.inDex[i]?i + 1:0,dexsppos[0][j],dexsppos[1][j],o2,p2,t2,true);
        }
        else if((pressed & KEY_LEFT)){
            while(1)
            {
                if(keysUp() & KEY_LEFT) 
                break;
                scanKeys();
                swiWaitForVBlank();
                updateTime();
            }
            if(acPage == 1)
                loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","ClearD");
            spriteInfo[16+acPage].entry->isHidden = false;
            acPage = (acPage + 2) %3;
            spriteInfo[16+acPage].entry->isHidden = true;
            updateOAMSub(oam);
            drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,acNum+1,acForme);
        }
        else if((pressed & KEY_RIGHT)){
            while(1)
            {
                if(keysUp() & KEY_RIGHT)
                break;
                scanKeys();
                swiWaitForVBlank();
                updateTime();
            }
            if(acPage == 1)
                loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","ClearD");
            spriteInfo[16+acPage].entry->isHidden = false;
            acPage = (acPage + 1) %3;
            spriteInfo[16+acPage].entry->isHidden = true;
            updateOAMSub(oam);
            drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,acNum+1,acForme);
        }
        else if((pressed & KEY_SELECT)){
            if(acPage > 1)
                while(1)
                {
                    if(keysUp() & KEY_SELECT)
                    break;
                    scanKeys();
                    swiWaitForVBlank();
                    updateTime();
                }
            if(acPage == 1)
                acMap = (acMap + 1) %3;
            else
                acForme = (acForme + 1)%60;
            drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,acNum+1,acForme);
        }
        for(int q = 0; q < 5; ++q)
            if(sqrt(sq(dexsppos[0][q]-t.px+16) + sq(dexsppos[1][q]-t.py+16)) <= 16){
                while(1)
                {
                    if(keysUp() & KEY_TOUCH) break;
                    scanKeys();
                    swiWaitForVBlank();
                    updateTime();
                }
                o2=oamInd;p2=palcnt;t2=tilecnt;
                acNum = (acNum + maxn - 3 + q + (q > 2 ? 1: 0)) % maxn;
                for(int i = (acNum + maxn - 3)%maxn,j = 0; j < 5; i = (i+1)%maxn,++j)
                if(i == acNum){drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,i+1,acForme);--j;
        loadPKMNSprite(oam,spriteInfo,"nitro:/PICS/SPRITES/PKMN/",SAV.inDex[i]?i + 1:0,dexsppos[0][8]+16,dexsppos[1][8]+16,o2,p2,t2,true); continue;}
                else
                    drawPKMNIcon(oam,spriteInfo,SAV.inDex[i]?i + 1:0,dexsppos[0][j],dexsppos[1][j],o2,p2,t2,true);
        }
        for(int q = 5; q < 8; ++q)
            if(sqrt(sq(dexsppos[0][q]-t.px+16) + sq(dexsppos[1][q]-t.py+16)) <= 16){
                while(1)
                {
                    if(keysUp() & KEY_TOUCH) break;
                    scanKeys();
                    swiWaitForVBlank();
                    updateTime();
                }
                if(acPage == 1)
                    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","ClearD");
                spriteInfo[16+acPage].entry->isHidden = false;
                acPage = (q + 1) %3;
                spriteInfo[16+acPage].entry->isHidden = true;
                updateOAMSub(oam);
                drawTopDexPage(acPage == 1 ? 4 + acMap: acPage,acNum+1,acForme);
        }
    }
    consoleSetWindow(&Bottom,0,0,32,24);
    consoleSelect(&Bottom);
    consoleClear();
    initOAMTableSub(oam);
    initMainSprites(oam,spriteInfo);
    setMainSpriteVisibility(false);
    oam->oamBuffer[8].isHidden = true;
    oam->oamBuffer[0].isHidden = true;
    oam->oamBuffer[1].isHidden = false;
}

#define MAXITEMSPERPAGE 12
void initBagSprites(OAMTable* oam,SpriteInfo* spriteInfo,int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen = true){
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                        * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
    (subScreen?sizeof(SPRITE_GFX_SUB[0]):sizeof(SPRITE_GFX[0]));
    /* Keep track of the available tiles */
    nextAvailableTileIdx = 0;
    oamIndex = 0;
    palcnt = 0;
    if(subScreen){
        SpriteInfo * backInfo = &spriteInfo[oamIndex];
        SpriteEntry * back = &oam->oamBuffer[oamIndex];
        backInfo->oamId = oamIndex;
        backInfo->width = 32;
        backInfo->height = 32;
        backInfo->angle = 0;
        backInfo->entry = back;
        back->y = SCREEN_HEIGHT - 28;
        back->isRotateScale = false;
        back->blendMode = OBJMODE_NORMAL;
        back->isMosaic = false;
        back->isHidden = false;
        back->colorMode = OBJCOLOR_16;
        back->shape = OBJSHAPE_SQUARE;
        back->x = SCREEN_WIDTH - 28;
        back->rotationIndex = backInfo->oamId;
        back->size = OBJSIZE_32;
        back->gfxIndex = nextAvailableTileIdx;
        back->priority = OBJPRIORITY_0;
        back->palette = backInfo->oamId;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BackTiles,
        &SPRITE_GFX_SUB[back->gfxIndex * OFFSET_MULTIPLIER],
        BackTilesLen);
        nextAvailableTileIdx += BackTilesLen / BYTES_PER_16_COLOR_TILE;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BackPal,
        &SPRITE_PALETTE_SUB[backInfo->oamId * COLORS_PER_PALETTE],
        32);
        palcnt = backInfo->oamId + 1;

        SpriteInfo * Bo4Info = &spriteInfo[++oamIndex];
        SpriteEntry * Bo4 = &oam->oamBuffer[oamIndex];
        Bo4Info->oamId = oamIndex;
        Bo4Info->width = 64;
        Bo4Info->height = 64;
        Bo4Info->angle = 0;
        Bo4Info->entry = Bo4;
        Bo4->y = 0;
        Bo4->isRotateScale = false;
        Bo4->blendMode = OBJMODE_NORMAL;
        Bo4->isMosaic = false;
        Bo4->colorMode = OBJCOLOR_16;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->isHidden = false;
        Bo4->x = 0;
        Bo4->size = OBJSIZE_64;
        Bo4->gfxIndex = nextAvailableTileIdx;
        Bo4->priority = OBJPRIORITY_2;
        Bo4->palette = palcnt;
        Bo4->vFlip = true;
        Bo4->hFlip = true;

        Bo4 = &oam->oamBuffer[++oamIndex];
        Bo4->y = 128;
        Bo4->isRotateScale = false;
        Bo4->blendMode = OBJMODE_NORMAL;
        Bo4->isMosaic = false;
        Bo4->colorMode = OBJCOLOR_16;
        Bo4->shape = OBJSHAPE_SQUARE;
        Bo4->isHidden = false;
        Bo4->x = 192;
        Bo4->size = OBJSIZE_64;
        Bo4->gfxIndex = nextAvailableTileIdx;
        Bo4->priority = OBJPRIORITY_2;
        Bo4->palette = palcnt;
        Bo4->vFlip = false;
        Bo4->hFlip = false;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_4Pal,
        &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
        32);
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_4Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        Border_4TilesLen);
        
        nextAvailableTileIdx += Border_4TilesLen / BYTES_PER_16_COLOR_TILE;
        
        SpriteInfo * Bo3Info = &spriteInfo[++oamIndex];
        SpriteEntry * Bo3 = &oam->oamBuffer[oamIndex];
        Bo3Info->oamId = 3;
        Bo3Info->width = 64;
        Bo3Info->height = 64;
        Bo3Info->angle = 0;
        Bo3Info->entry = Bo3;
        Bo3->y = 0;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = false;
        Bo3->x = 64;
        Bo3->size = OBJSIZE_64;
        Bo3->gfxIndex = nextAvailableTileIdx;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = palcnt;
        Bo3->vFlip = true;
        Bo3->hFlip = true;

        Bo3 = &oam->oamBuffer[++oamIndex];
        Bo3->y = 128;
        Bo3->isRotateScale = false;
        Bo3->blendMode = OBJMODE_NORMAL;
        Bo3->isMosaic = false;
        Bo3->colorMode = OBJCOLOR_16;
        Bo3->shape = OBJSHAPE_SQUARE;
        Bo3->isHidden = false;
        Bo3->x = 128;
        Bo3->size = OBJSIZE_64;
        Bo3->gfxIndex = nextAvailableTileIdx;
        Bo3->priority = OBJPRIORITY_2;
        Bo3->palette = palcnt;
        Bo3->vFlip = false;
        Bo3->hFlip = false;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_3Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        Border_3TilesLen);
        
        nextAvailableTileIdx += Border_3TilesLen / BYTES_PER_16_COLOR_TILE;
        
        SpriteInfo * Bo2Info = &spriteInfo[++oamIndex];
        SpriteEntry * Bo2 = &oam->oamBuffer[oamIndex];
        Bo2Info->oamId = 3;
        Bo2Info->width = 64;
        Bo2Info->height = 64;
        Bo2Info->angle = 0;
        Bo2Info->entry = Bo2;
        Bo2->y = 0;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = false;
        Bo2->x = 128;
        Bo2->size = OBJSIZE_64;
        Bo2->gfxIndex = nextAvailableTileIdx;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = palcnt;
        Bo2->vFlip = true;
        Bo2->hFlip = true;

        Bo2 = &oam->oamBuffer[++oamIndex];
        Bo2->y = 128;
        Bo2->isRotateScale = false;
        Bo2->blendMode = OBJMODE_NORMAL;
        Bo2->isMosaic = false;
        Bo2->colorMode = OBJCOLOR_16;
        Bo2->shape = OBJSHAPE_SQUARE;
        Bo2->isHidden = false;
        Bo2->x = 64;
        Bo2->size = OBJSIZE_64;
        Bo2->gfxIndex = nextAvailableTileIdx;
        Bo2->priority = OBJPRIORITY_2;
        Bo2->palette = palcnt;
        Bo2->vFlip = false;
        Bo2->hFlip = false;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_2Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        Border_2TilesLen);
        nextAvailableTileIdx += Border_2TilesLen / BYTES_PER_16_COLOR_TILE;
        
        SpriteInfo * Bo1Info = &spriteInfo[++oamIndex];
        SpriteEntry * Bo1 = &oam->oamBuffer[oamIndex];
        Bo1Info->oamId = 3;
        Bo1Info->width = 64;
        Bo1Info->height = 64;
        Bo1Info->angle = 0;
        Bo1Info->entry = Bo1;
        Bo1->y = 0;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = false;
        Bo1->x = 192;
        Bo1->size = OBJSIZE_64;
        Bo1->gfxIndex = nextAvailableTileIdx;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = palcnt;
        Bo1->vFlip = true;
        Bo1->hFlip = true;

        Bo1 = &oam->oamBuffer[++oamIndex];
        Bo1->y = 128;
        Bo1->isRotateScale = false;
        Bo1->blendMode = OBJMODE_NORMAL;
        Bo1->isMosaic = false;
        Bo1->colorMode = OBJCOLOR_16;
        Bo1->shape = OBJSHAPE_SQUARE;
        Bo1->isHidden = false;
        Bo1->x = 0;
        Bo1->size = OBJSIZE_64;
        Bo1->gfxIndex = nextAvailableTileIdx;
        Bo1->priority = OBJPRIORITY_2;
        Bo1->palette = palcnt;
        Bo1->vFlip = false;
        Bo1->hFlip = false;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_1Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        Border_1TilesLen);
        nextAvailableTileIdx += Border_1TilesLen / BYTES_PER_16_COLOR_TILE;
        
        SpriteInfo * Bo5Info = &spriteInfo[++oamIndex];
        SpriteEntry * Bo5 = &oam->oamBuffer[oamIndex];
        Bo5Info->oamId = 3;
        Bo5Info->width = 64;
        Bo5Info->height = 64;
        Bo5Info->angle = 0;
        Bo5Info->entry = Bo5;
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 0;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdx;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcnt;
        Bo5->vFlip = true;
        Bo5->hFlip = true;

        Bo5 = &oam->oamBuffer[++oamIndex];
        Bo5->y = 64;
        Bo5->isRotateScale = false;
        Bo5->blendMode = OBJMODE_NORMAL;
        Bo5->isMosaic = false;
        Bo5->colorMode = OBJCOLOR_16;
        Bo5->shape = OBJSHAPE_SQUARE;
        Bo5->isHidden = false;
        Bo5->x = 192;
        Bo5->size = OBJSIZE_64;
        Bo5->gfxIndex = nextAvailableTileIdx;
        Bo5->priority = OBJPRIORITY_2;
        Bo5->palette = palcnt;
        Bo5->vFlip = false;
        Bo5->hFlip = false;

        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        Border_5Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        Border_5TilesLen);
        nextAvailableTileIdx += Border_5TilesLen / BYTES_PER_16_COLOR_TILE;
    }
    else{
        SpriteInfo * BoxInfo = &spriteInfo[++oamIndex];
        SpriteEntry * Box = &oam->oamBuffer[oamIndex];
        BoxInfo->oamId = 1;
        BoxInfo->width = 64;
        BoxInfo->height = 64;
        BoxInfo->angle = 0;
        BoxInfo->entry = Box;
        Box->y = 0;
        Box->isRotateScale = false;
        Box->blendMode = OBJMODE_NORMAL;
        Box->isMosaic = false;
        Box->colorMode = OBJCOLOR_16;
        Box->shape = OBJSHAPE_SQUARE;
        Box->isHidden = true;
        Box->x = 0;
        Box->size = OBJSIZE_64;
        Box->gfxIndex = nextAvailableTileIdx;
        Box->priority = OBJPRIORITY_3;
        Box->palette = palcnt;
        
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        ItemSpr1Tiles,
        &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        ItemSpr1TilesLen);
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        ItemSpr1Pal,
        &SPRITE_PALETTE[palcnt * COLORS_PER_PALETTE],
        32);
        nextAvailableTileIdx += ItemSpr1TilesLen / BYTES_PER_16_COLOR_TILE;
        
        Box = &oam->oamBuffer[++oamIndex];
        Box->y = 0;
        Box->isRotateScale = false;
        Box->blendMode = OBJMODE_NORMAL;
        Box->isMosaic = false;
        Box->colorMode = OBJCOLOR_16;
        Box->shape = OBJSHAPE_SQUARE;
        Box->isHidden = true;
        Box->x = 0;
        Box->size = OBJSIZE_64;
        Box->gfxIndex = nextAvailableTileIdx;
        Box->priority = OBJPRIORITY_3;
        Box->palette = palcnt;
        
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        ItemSpr2Tiles,
        &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        ItemSpr2TilesLen);
        nextAvailableTileIdx += ItemSpr2TilesLen / BYTES_PER_16_COLOR_TILE;
        
        Box = &oam->oamBuffer[++oamIndex];
        Box->y = 0;
        Box->isRotateScale = false;
        Box->blendMode = OBJMODE_NORMAL;
        Box->isMosaic = false;
        Box->colorMode = OBJCOLOR_16;
        Box->shape = OBJSHAPE_SQUARE;
        Box->isHidden = true;
        Box->x = 0;
        Box->size = OBJSIZE_64;
        Box->gfxIndex = nextAvailableTileIdx;
        Box->priority = OBJPRIORITY_3;
        Box->palette = palcnt;
        
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        ItemSpr3Tiles,
        &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        ItemSpr3TilesLen);
        nextAvailableTileIdx += ItemSpr3TilesLen / BYTES_PER_16_COLOR_TILE;
    }
    ++palcnt;
    oamIndex = 10;
    for(int i= 0; i< MAXITEMSPERPAGE + 2; ++i){
        SpriteInfo * BagSprInfo = &spriteInfo[++oamIndex];
        SpriteEntry * BagSpr = &oam->oamBuffer[oamIndex];
        BagSprInfo->oamId = 11;
        BagSprInfo->width = 32;
        BagSprInfo->height = 32;
        BagSprInfo->angle = 0;
        BagSprInfo->entry = BagSpr;
        BagSpr->y = 0;
        BagSpr->isRotateScale = false;
        BagSpr->blendMode = OBJMODE_NORMAL;
        BagSpr->isMosaic = false;
        BagSpr->colorMode = OBJCOLOR_16;
        BagSpr->shape = OBJSHAPE_SQUARE;
        BagSpr->isHidden = true;
        BagSpr->x = 0;
        BagSpr->size = OBJSIZE_32;
        BagSpr->gfxIndex = nextAvailableTileIdx;
        BagSpr->priority = BGs[BG_ind].allowsOverlay? OBJPRIORITY_3 : OBJPRIORITY_2;
        if(!subScreen)
        BagSpr->priority = OBJPRIORITY_2;
        BagSpr->palette = palcnt;
    }
    if(subScreen){
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSprPal,
        &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
        32);
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSprTiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        BagSprTilesLen);
    }
    else{
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSprPal,
        &SPRITE_PALETTE[palcnt * COLORS_PER_PALETTE],
        32);
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSprTiles,
        &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        BagSprTilesLen);
    }
    nextAvailableTileIdx += BagSprTilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;
    for(int i= 0; i< MAXITEMSPERPAGE; ++i){
        SpriteInfo * BagSprInfo = &spriteInfo[++oamIndex];
        SpriteEntry * BagSpr = &oam->oamBuffer[oamIndex];
        BagSprInfo->oamId = 13 +MAXITEMSPERPAGE;
        BagSprInfo->width = 32;
        BagSprInfo->height = 16;
        BagSprInfo->angle = 0;
        BagSprInfo->entry = BagSpr;
        BagSpr->y = 0;
        BagSpr->isRotateScale = false;
        BagSpr->blendMode = OBJMODE_NORMAL;
        BagSpr->isMosaic = false;
        BagSpr->colorMode = OBJCOLOR_16;
        BagSpr->shape = OBJSHAPE_WIDE;
        BagSpr->isHidden = true;
        BagSpr->x = 0;
        BagSpr->size = OBJSIZE_32;
        BagSpr->gfxIndex = nextAvailableTileIdx;
        BagSpr->priority = OBJPRIORITY_1;
        BagSpr->palette = palcnt;
    }
    if(subScreen){
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSpr2Pal,
        &SPRITE_PALETTE_SUB[palcnt * COLORS_PER_PALETTE],
        32);
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSpr2Tiles,
        &SPRITE_GFX_SUB[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        BagSpr2TilesLen);
    }
    else{
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSpr2Pal,
        &SPRITE_PALETTE[palcnt * COLORS_PER_PALETTE],
        32);
        dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
        BagSpr2Tiles,
        &SPRITE_GFX[nextAvailableTileIdx * OFFSET_MULTIPLIER],
        BagSpr2TilesLen);
    }
    nextAvailableTileIdx += BagSpr2TilesLen / BYTES_PER_16_COLOR_TILE;
    ++palcnt;
}

void drawItem(OAMTable* oam,SpriteInfo* spriteInfo,const std::string& item_name,const int posX,const int posY,const int cnt,
int& oamIndex,int& palcnt, int& nextAvailableTileIdx,bool subScreen = true,bool showcnt = false){
    
    static const int OFFSET_MULTIPLIER = 32 / sizeof(SPRITE_GFX_SUB[0]);
    SpriteInfo *ItemInfo = &spriteInfo[++oamIndex];
    SpriteEntry *Item = &oam->oamBuffer[oamIndex];
    ItemInfo->oamId = oamIndex;
    ItemInfo->width = ItemInfo->height = 32;
    ItemInfo->angle = 0;
    ItemInfo->entry = Item;
    oam->oamBuffer[oamIndex-2*MAXITEMSPERPAGE].x = Item->x = posX;
    oam->oamBuffer[oamIndex-MAXITEMSPERPAGE].x = posX + 8;
    oam->oamBuffer[oamIndex-2*MAXITEMSPERPAGE].y = Item->y = posY;
    oam->oamBuffer[oamIndex-MAXITEMSPERPAGE].y = posY + 28;
    oam->oamBuffer[oamIndex-2*MAXITEMSPERPAGE].isHidden = false;
    oam->oamBuffer[oamIndex-MAXITEMSPERPAGE].isHidden = showcnt;
    Item->isRotateScale = false;
    Item->blendMode = OBJMODE_NORMAL;
    Item->isMosaic = false;
    Item->colorMode = OBJCOLOR_16;
    Item->shape = OBJSHAPE_SQUARE;
    Item->isHidden = false;
    Item->size = OBJSIZE_32;
    Item->gfxIndex = nextAvailableTileIdx;
    Item->priority = subScreen? OBJPRIORITY_1:OBJPRIORITY_0;
    Item->palette = palcnt;
    if(subScreen){
        if(!loadSpriteSub(ItemInfo,"nitro:/PICS/SPRITES/ITEMS/",item_name.c_str(),128,16)){
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL, NoItemPal, &SPRITE_PALETTE_SUB[palcnt * 16], 32);
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL, NoItemTiles, &SPRITE_GFX_SUB[ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER], NoItemTilesLen);
        }
    }
    else{
        if(!loadSprite(ItemInfo,"nitro:/PICS/SPRITES/ITEMS/",item_name.c_str(),128,16)){
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL, NoItemPal, &SPRITE_PALETTE[palcnt * 16], 32);
            dmaCopyHalfWords(SPRITE_DMA_CHANNEL, NoItemTiles, &SPRITE_GFX[ItemInfo->entry->gfxIndex * OFFSET_MULTIPLIER], NoItemTilesLen);
        }
    }
    nextAvailableTileIdx +=  spriteInfo->height * spriteInfo->width / 32;
    ++palcnt;
    if(subScreen){
        updateOAMSub(oam);
        consoleSelect(&Bottom);
        consoleSetWindow(&Bottom,(posX+10)/8,(posY+32)/8,3,1);
    }
    else{
        updateOAM(oam);
        consoleSelect(&Top);
        consoleSetWindow(&Top,(posX+10)/8,(posY+32)/8,3,1);
    }
    if(!showcnt)
        printf("%3i",cnt);
}
const int MAXPERM = 14;
void getRanPerm(int* arr,int* out){
    int used = 0;
    for(int i=0; i < MAXPERM; ++i){
        int newInd = rand() % MAXPERM;
        while(used & (1 << newInd))
        newInd = rand() % MAXPERM;
        out[newInd] = arr[i];
        used |= (1 << newInd);
    }
}

const std::string choi[6] = {"\nEinsetzen.","\nEinem PKMN geben.","\nRegistrieren.","\nWeitere Daten ansehen."};
int getAnswer(item::ITEM_TYPE bagtype){
    touchPosition t;
    if(bagtype != item::BERRIES)
    {
        (oam->oamBuffer[20]).isHidden = false;
        (oam->oamBuffer[21]).isHidden = false;
        (oam->oamBuffer[29]).isHidden = false;
        (oam->oamBuffer[24]).isHidden = false;
        (oam->oamBuffer[25]).isHidden = false;
        (oam->oamBuffer[28]).isHidden = false;
        (oam->oamBuffer[20]).y -= 32;
        (oam->oamBuffer[21]).y -= 32;
        (oam->oamBuffer[28]).y -= 32;
        (oam->oamBuffer[24]).y -= 32;
        (oam->oamBuffer[25]).y -= 32;
        (oam->oamBuffer[29]).y -= 32;
        updateOAMSub(oam);
        
        consoleSetWindow(&Bottom,5,9,22,3);
        printf(choi[0].c_str());

        consoleSetWindow(&Bottom,5,13,22,3);
        if(bagtype != item::KEY_ITEM )
        printf(choi[1].c_str());
        else
        printf(choi[2].c_str());

        while(42)
        {
            updateTime();
            swiWaitForVBlank();
            updateOAMSub(oam);
            scanKeys();
            touchRead(&t);
            if ( t.px>224 && t.py>164)
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
                consoleSetWindow(&Bottom,0,0,32,24);
                consoleClear();
                return 4;
            }

            if (t.px > 31 && t.py > 67 && t.px < 225 && t.py <101)
            {           
                (oam->oamBuffer[20]).isHidden = true;
                (oam->oamBuffer[21]).isHidden = true;
                (oam->oamBuffer[28]).isHidden = true;
                updateOAMSub(oam);
                while(1)
                {
                    swiWaitForVBlank();
                    scanKeys();
                    updateTime();
                    if(keysUp() & KEY_TOUCH)
                    break;
                }
                (oam->oamBuffer[24]).isHidden = true;
                (oam->oamBuffer[25]).isHidden = true;
                (oam->oamBuffer[29]).isHidden = true;
                (oam->oamBuffer[20]).y += 32;
                (oam->oamBuffer[21]).y += 32;
                (oam->oamBuffer[28]).y += 32;
                (oam->oamBuffer[24]).y += 32;
                (oam->oamBuffer[25]).y += 32;
                (oam->oamBuffer[29]).y += 32;
                updateOAMSub(oam);
                consoleSetWindow(&Bottom, 1,1,30,24);   
                consoleClear();

                return 0;
            }
            else if (t.px > 31 && t.py > 99 && t.px < 225 && t.py <143)
            {           
                (oam->oamBuffer[24]).isHidden = true;
                (oam->oamBuffer[25]).isHidden = true;
                (oam->oamBuffer[29]).isHidden = true;
                updateOAMSub(oam);
                while(1)
                {
                    swiWaitForVBlank();
                    scanKeys();
                    updateTime();
                    if(keysUp() & KEY_TOUCH)
                    break;
                }
                (oam->oamBuffer[20]).isHidden = true;
                (oam->oamBuffer[21]).isHidden = true;
                (oam->oamBuffer[28]).isHidden = true;
                (oam->oamBuffer[20]).y += 32;
                (oam->oamBuffer[21]).y += 32;
                (oam->oamBuffer[28]).y += 32;
                (oam->oamBuffer[24]).y += 32;
                (oam->oamBuffer[25]).y += 32;
                (oam->oamBuffer[29]).y += 32;
                updateOAMSub(oam);
                consoleSetWindow(&Bottom, 1,1,30,24);   
                consoleClear();

                return 1;
            }
        }
    }
    else{
        for(int i= 0; i<3; ++i)
        {
            (oam->oamBuffer[17+4*i]).isHidden = false;
            (oam->oamBuffer[17+4*i]).y -= 16;
            (oam->oamBuffer[16+4*i]).isHidden = false;
            (oam->oamBuffer[16+4*i]).y -= 16;
            (oam->oamBuffer[27+i]).isHidden = false;
            (oam->oamBuffer[27+i]).y -= 16;
            updateOAMSub(oam); 
            consoleSetWindow(&Bottom,5,7+4*i,22,3);
            if(i == 2)
            ++i;
            printf(choi[i].c_str());
        }
        while(42)
        {
            updateTime();
            swiWaitForVBlank();
            updateOAMSub(oam);
            scanKeys();
            touchRead(&t);
            if ( t.px>224 && t.py>164)
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
                consoleSetWindow(&Bottom,0,0,32,24);
                consoleClear();
                return 4;
            }
            if (t.px > 31 && t.py > 51 && t.px < 225 && t.py <85)
            {           
                oam->oamBuffer[17].isHidden = true;
                oam->oamBuffer[16].isHidden = true;
                oam->oamBuffer[27].isHidden = true;
                updateOAMSub(oam);
                while(1)
                {
                    swiWaitForVBlank();
                    scanKeys();
                    updateTime();
                    if(keysUp() & KEY_TOUCH)
                    break;
                }
                consoleSetWindow(&Bottom, 1,1,30,24);   
                consoleClear();
                for(int i= 0; i<3; ++i)
                {
                    (oam->oamBuffer[17+4*i]).isHidden = true;
                    (oam->oamBuffer[17+4*i]).y += 16;
                    (oam->oamBuffer[16+4*i]).isHidden = true;
                    (oam->oamBuffer[16+4*i]).y += 16;
                    (oam->oamBuffer[27+i]).isHidden = true;
                    (oam->oamBuffer[27+i]).y += 16;
                    updateOAMSub(oam);
                }
                return 0;
            }
            else if (t.px > 31 && t.py > 83 && t.px < 225 && t.py <116)
            {           
                oam->oamBuffer[20].isHidden = true;
                oam->oamBuffer[21].isHidden = true;
                oam->oamBuffer[28].isHidden = true;
                updateOAMSub(oam);
                while(1)
                {
                    swiWaitForVBlank();
                    scanKeys();
                    updateTime();
                    if(keysUp() & KEY_TOUCH)
                    break;
                }
                consoleSetWindow(&Bottom, 1,1,30,24);   
                consoleClear();
                for(int i= 0; i<3; ++i)
                {
                    (oam->oamBuffer[17+4*i]).isHidden = true;
                    (oam->oamBuffer[17+4*i]).y += 16;
                    (oam->oamBuffer[16+4*i]).isHidden = true;
                    (oam->oamBuffer[16+4*i]).y += 16;
                    (oam->oamBuffer[27+i]).isHidden = true;
                    (oam->oamBuffer[27+i]).y += 16;
                    updateOAMSub(oam);
                }
                return 1;
            }
            else if (t.px > 31 && t.py > 115 && t.px < 225 && t.py <148)
            {           
                oam->oamBuffer[24].isHidden = true;
                oam->oamBuffer[25].isHidden = true;
                oam->oamBuffer[29].isHidden = true;
                updateOAMSub(oam);
                while(1)
                {
                    swiWaitForVBlank();
                    scanKeys();
                    updateTime();
                    if(keysUp() & KEY_TOUCH)
                    break;
                }
                consoleSetWindow(&Bottom, 1,1,30,24);   
                consoleClear();
                
                for(int i= 0; i<3; ++i)
                {
                    (oam->oamBuffer[17+4*i]).isHidden = true;
                    (oam->oamBuffer[17+4*i]).y += 16;
                    (oam->oamBuffer[16+4*i]).isHidden = true;
                    (oam->oamBuffer[16+4*i]).y += 16;
                    (oam->oamBuffer[27+i]).isHidden = true;
                    (oam->oamBuffer[27+i]).y += 16;
                    updateOAMSub(oam);
                }
                return 2;
            }
        }
    }
}
std::string bagnames[8] = {"Items","Schl\x81""sselitems","TM/VM","Briefe","Medizin","Beeren","Pok\x82""b\x84""lle","Kampfitems"};
void drawBagPage(int page,int* pos,int &oamIndex,int& palcnt,int& nextAvTileIdx,int &oamIndexT,int& palcntT,int& nextAvTileIdxT){
    consoleSetWindow(&Bottom,0,0,32,24);
    consoleSelect(&Bottom);
    consoleClear();
    for(int j= 11; j< 19; ++j){
        oam->oamBuffer[j+2].isHidden = true;
        updateOAMSub(oam);
    }
    int ind = 214 + 4 * (page), maxpage = 1 + ((int(SAV.Bag.size(bag::BAGTYPE(page))))-1) / 12;
    printf("%c%c %s\n%c%c  1/%i",ind,ind+1,bagnames[page].c_str(),ind +2,ind +3, maxpage);
    
    oam->oamBuffer[11].x = SCREEN_WIDTH - 22;
    oam->oamBuffer[11].y = SCREEN_HEIGHT - 49;
    oam->oamBuffer[11].priority = OBJPRIORITY_1;
    oam->oamBuffer[12].x = SCREEN_WIDTH - 49;
    oam->oamBuffer[12].y = SCREEN_HEIGHT - 22;
    oam->oamBuffer[12].priority = OBJPRIORITY_1;

    if(maxpage > 1){
        oam->oamBuffer[11].isHidden = false;
        updateOAMSub(oam);        
        consoleSetWindow(&Bottom,31,19,2,2);
        printf("%c\n%c",246,247);
    }
    
    int cpy[MAXPERM];
    getRanPerm(pos,cpy);
    for(int i = 0; i < MAXPERM; ++i)
    pos[i] = cpy[i];

    int acpage = 0,oam2 = oamIndex,pal2 = palcnt,tile2 = nextAvTileIdx;
    touchPosition t;
    for(int i= acpage * 12; i< acpage * 12+ std::min(int(SAV.Bag.size(bag::BAGTYPE(page))) - acpage * 12,12);++i){
        updateTime();
        scanKeys();
        touchRead(&t);
        if(sqrt(sq(SCREEN_WIDTH - 6 -t.px) + sq(SCREEN_HEIGHT - 33-t.py)) <= 16 && acpage < maxpage - 1)
        goto NEXT;
        if(sqrt(sq(SCREEN_WIDTH - 33 -t.px) + sq(SCREEN_HEIGHT - 6-t.py)) <= 16 && acpage > 0)
        goto PREV;
        if ( t.px>224 && t.py>164)
        goto BACK;
        std::pair<int,int> acElem = SAV.Bag.element_at(bag::BAGTYPE(page),i);
        drawItem(oam,spriteInfo,ItemList[acElem.first].Name,38 + (pos[i - (acpage*12)]%4*48),
        18 + (pos[i - (acpage*12)]/4)*40,acElem.second,oam2,pal2,tile2,true,page == item::KEY_ITEM);
    }
    while(42){
        updateTime();
        swiWaitForVBlank();
        updateOAMSub(oam);
        scanKeys();
        touchRead(&t);
        //int pressed = keysUp(),held = keysHeld();
        if ( t.px>224 && t.py>164)
        {  
BACK:
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            return;
        }
        else if(sqrt(sq(SCREEN_WIDTH - 6 -t.px) + sq(SCREEN_HEIGHT - 33-t.py)) <= 16 && acpage < maxpage - 1){
NEXT:
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            acpage++;
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            printf("%c%c %s\n%c%c %2i/%i",ind,ind+1,bagnames[page].c_str(),ind +2,ind +3,acpage+1, maxpage);
            
            oam->oamBuffer[12].isHidden = false;
            consoleSetWindow(&Bottom,27,22,2,2);
            printf("%c\n%c",248,249);
            consoleSetWindow(&Bottom,31,19,2,2);
            printf("%c\n%c",246,247);
            if(maxpage == acpage +1){
                oam->oamBuffer[11].isHidden = true;
                consoleSetWindow(&Bottom,31,19,2,2);
                consoleClear();
            }
            updateOAMSub(oam);        
            getRanPerm(pos,cpy);
            for(int i = 0; i < MAXPERM; ++i)
            pos[i] = cpy[i];
            oam2 = oamIndex;
            pal2 = palcnt;
            tile2 = nextAvTileIdx;
            for(int i= 13; i< oam2 + 13; ++i) oam->oamBuffer[i].isHidden = true;
            updateOAMSub(oam);
            for(int i= acpage * 12; i< acpage * 12+ std::min(int(SAV.Bag.size(bag::BAGTYPE(page))) - acpage * 12,12);++i){
                updateTime();
                scanKeys();
                touchRead(&t);
                if(sqrt(sq(SCREEN_WIDTH - 6 -t.px) + sq(SCREEN_HEIGHT - 33-t.py)) <= 16 && acpage < maxpage - 1)
                goto NEXT;
                if(sqrt(sq(SCREEN_WIDTH - 33 -t.px) + sq(SCREEN_HEIGHT - 6-t.py)) <= 16 && acpage > 0)
                goto PREV;
                if ( t.px>224 && t.py>164)
                goto BACK;
                std::pair<int,int> acElem = SAV.Bag.element_at(bag::BAGTYPE(page),i);
                drawItem(oam,spriteInfo,ItemList[acElem.first].Name,38 + (pos[i - (acpage*12)]%4*48),
                18 + (pos[i - (acpage*12)]/4)*40,acElem.second,oam2,pal2,tile2,true,page == item::KEY_ITEM);
            }
        }
        else if(sqrt(sq(SCREEN_WIDTH - 33 -t.px) + sq(SCREEN_HEIGHT - 6-t.py)) <= 16 && acpage > 0){
PREV:
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            acpage--;
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            printf("%c%c %s\n%c%c %2i/%i",ind,ind+1,bagnames[page].c_str(),ind +2,ind +3,acpage+1, maxpage);
            
            oam->oamBuffer[11].isHidden = false;
            consoleSetWindow(&Bottom,31,19,2,2);
            printf("%c\n%c",246,247);
            consoleSetWindow(&Bottom,27,22,2,2);
            printf("%c\n%c",248,249);
            if(0 == acpage){
                oam->oamBuffer[12].isHidden = true;
                consoleSetWindow(&Bottom,27,22,2,2);
                consoleClear();
            }
            updateOAMSub(oam);        
            getRanPerm(pos,cpy);
            for(int i = 0; i < MAXPERM; ++i)
            pos[i] = cpy[i];
            oam2 = oamIndex;
            pal2 = palcnt;
            tile2 = nextAvTileIdx;
            for(int i= 13; i< oam2 + 13; ++i) oam->oamBuffer[i].isHidden = true;
            updateOAMSub(oam);
            for(int i= acpage * 12; i< acpage * 12+ std::min(int(SAV.Bag.size(bag::BAGTYPE(page))) - acpage * 12,12);++i){
                updateTime();
                scanKeys();
                touchRead(&t);
                if((sqrt(sq(SCREEN_WIDTH - 6 -t.px) + sq(SCREEN_HEIGHT - 33-t.py)) <= 16) && acpage < maxpage - 1)
                goto NEXT;
                if(sqrt(sq(SCREEN_WIDTH - 33 -t.px) + sq(SCREEN_HEIGHT - 6-t.py)) <= 16 && acpage > 0)
                goto PREV;
                if ( t.px>224 && t.py>164)
                goto BACK;
                std::pair<int,int> acElem = SAV.Bag.element_at(bag::BAGTYPE(page),i);
                drawItem(oam,spriteInfo,ItemList[acElem.first].Name,38 + (pos[i - (acpage*12)]%4*48),
                18 + (pos[i - (acpage*12)]/4)*40,acElem.second,oam2,pal2,tile2,true,page == item::KEY_ITEM);
            }
        }
        else
        for(int io= 0; io< std::min(int(SAV.Bag.size(bag::BAGTYPE(page))) - acpage * 12,12);++io)
        if(sqrt(sq((54 + (pos[io]%4*48)) -t.px) + sq((34 + (pos[io]/4)*40)-t.py)) <= 16){
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            oam2 = oamIndex;
            pal2 = palcnt;
            tile2 = nextAvTileIdx;
            consoleSelect(&Top);
            consoleSetWindow(&Top,0,0,32,24);
            consoleClear();
            loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","ClearD");
            oamTop->oamBuffer[1].isHidden = oamTop->oamBuffer[2].isHidden = oamTop->oamBuffer[3].isHidden = false;
            oamTop->oamBuffer[1].x = 36;
            oamTop->oamBuffer[2].x = 100;
            oamTop->oamBuffer[3].x = 164;
            oamTop->oamBuffer[1].y = oamTop->oamBuffer[2].y = oamTop->oamBuffer[3].y = 69;
            updateOAM(oamTop);	

            std::pair<int,int> acElem = SAV.Bag.element_at(bag::BAGTYPE(page),acpage * 12 + io);
            drawItem(oamTop,spriteInfo,ItemList[acElem.first].Name,22,
            50,acElem.second,oam2,pal2,tile2,false,page == item::KEY_ITEM);
            consoleSetWindow(&Top,7,9,13,1);
            consoleSelect(&Top);
            printf(ItemList[acElem.first].getDisplayName().c_str());
            consoleSetWindow(&Top,6,11,23,10);
            printf(("  "+ItemList[acElem.first].getDescription()).c_str());
            //printf("   %i  %i  %i",ItemList[acElem.first].getItemType(),ItemList[acElem.first].itemtype,ItemList[SAV.Bag.element_at((bag::BAGTYPE)page,acpage * 12 +io).first].getItemType());   

            for(int i= acpage * 12; i< acpage * 12+ std::min(int(SAV.Bag.size(bag::BAGTYPE(page))) - acpage * 12,12);++i){
                acElem = SAV.Bag.element_at(bag::BAGTYPE(page),i);
                if(i-acpage * 12 == io)
                continue;
                else if(pos[i-acpage * 12] < 6)
                drawItem(oamTop,spriteInfo,ItemList[acElem.first].Name,6 + pos[i-acpage * 12]*40,
                10,acElem.second,oam2,pal2,tile2,false,page == item::KEY_ITEM);
                else if(pos[i-acpage * 12] < 12)
                drawItem(oamTop,spriteInfo,ItemList[acElem.first].Name,6 + (pos[i-acpage * 12]-6)*40,
                138,acElem.second,oam2,pal2,tile2,false,page == item::KEY_ITEM);
                else if(pos[i-acpage * 12] == 12)
                drawItem(oamTop,spriteInfo,ItemList[acElem.first].Name,238,
                82,acElem.second,oam2,pal2,tile2,false,page == item::KEY_ITEM);
                else if(pos[i-acpage * 12] == 13)
                drawItem(oamTop,spriteInfo,ItemList[acElem.first].Name,-10,
                82,acElem.second,oam2,pal2,tile2,false,page == item::KEY_ITEM);

                updateTime();
            }


            initOAMTableSub(oam);
            initMainSprites(oam,spriteInfo);
            setMainSpriteVisibility(true);
            oam->oamBuffer[8].isHidden = true;
            oam->oamBuffer[0].isHidden = false;
            oam->oamBuffer[1].isHidden = true;
            for(int i= 9;i <= 12; ++i) 
            oam->oamBuffer[i].isHidden = true;

            consoleSelect(&Bottom);
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleClear();
            consoleSetWindow(&Bottom, 1,1,30,MAXLINES);	
            swiWaitForVBlank();
            updateOAMSub(oam);
            
            int acItem = SAV.Bag.element_at((bag::BAGTYPE)page,acpage * 12 +io).first;
            int ret =  getAnswer(ItemList[acItem].getItemType());
            
            if(ret == 0){

            }
            else if(ret == 1 &&
                    ItemList[SAV.Bag.element_at((bag::BAGTYPE)page,acpage * 12 +io).first].getItemType() != item::KEY_ITEM){
                int a = 2,b = 2,c = 81;
                int num = (int)SAV.PKMN_team.size();
                consoleSelect(&Bottom);
                for(int i = 17; i < 26; i+=(((i-15)/2)%2?-2:+6)){
                    if((((i-15)/2)^1) >= num)
                    break;
                    (oam->oamBuffer[i]).isHidden = false;
                    (oam->oamBuffer[i+1]).isHidden = false;
                    (oam->oamBuffer[i+1]).y -= 16 * (2-((i-15)/4));
                    (oam->oamBuffer[i]).y -= 16 * (2-((i-15)/4));
                    updateOAMSub(oam); 
                    consoleSetWindow(&Bottom,((oam->oamBuffer[i]).x+6)/8,((oam->oamBuffer[i]).y+6)/8,12,3);
                    printf("   %3i/%3i\n ",SAV.PKMN_team[((i-15)/2)^1].stats.acHP,SAV.PKMN_team[((i-15)/2)^1].stats.maxHP);
                    wprintf(SAV.PKMN_team[((i-15)/2)^1].boxdata.Name); printf("\n");
                    printf("%11s",ItemList[SAV.PKMN_team[((i-15)/2)^1].boxdata.Item].getDisplayName().c_str());
                    drawPKMNIcon(oam,spriteInfo,SAV.PKMN_team[((i-15)/2)^1].boxdata.SPEC,(oam->oamBuffer[i]).x-4,(oam->oamBuffer[i]).y-20,a,b,c);
                    updateOAMSub(oam); 
                }
                updateOAMSub(oam);
                while(42){
                    swiWaitForVBlank();
                    updateOAMSub(oam);
                    updateTime();
                    touchRead(&t);

                    if ( t.px>224 && t.py>164)
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
                        consoleSetWindow(&Bottom,0,0,32,24);
                        consoleClear();
                        break;
                    }
                    for(int i = 17; i < 26; i+=(((i-15)/2)%2?-2:+6)){
                        if((((i-15)/2)^1) >= num)
                        break;
                        else if (t.px > oam->oamBuffer[i].x && t.py > oam->oamBuffer[i].y && t.px-64 < oam->oamBuffer[i+1].x && t.py-32 < oam->oamBuffer[i].y){  
                            (oam->oamBuffer[i]).isHidden = true;
                            (oam->oamBuffer[i+1]).isHidden = true;
                            (oam->oamBuffer[3 + (((i-15)/2)^1)]).isHidden = true;
                            updateOAMSub(oam);
                            while(1)
                            {
                                swiWaitForVBlank();
                                scanKeys();
                                updateTime();
                                if(keysUp() & KEY_TOUCH)
                                break;
                            }
                            consoleSetWindow(&Bottom, 1,1,30,24);   
                            consoleClear();
                            {
                                POKEMON::PKMN& ac = SAV.PKMN_team[(((i-15)/2)^1)];
                                initOAMTableSub(oam);
                                initMainSprites(oam,spriteInfo);
                                setMainSpriteVisibility(true);
                                oam->oamBuffer[8].isHidden = true;
                                oam->oamBuffer[0].isHidden = false;
                                oam->oamBuffer[1].isHidden = true;
                                if(ac.boxdata.Item != 0){//PKMN hat schon Item 
                                    std::wstring ws(ac.boxdata.Name);
                                    ws += L" h\x84""lt bereits\ndas Item ";
                                    mbox M = mbox(ws.c_str(),0,true,false,false);
                                    M.put((ItemList[ac.boxdata.Item].getDisplayName()+".").c_str(),false);
                                    if(ynbox(M).getResult("\n\nSollen die Items ausgetauscht werden?")){
                                        int pkmnOldItem = ac.boxdata.Item;
                                        ac.boxdata.Item = acItem;
                                        SAV.Bag.removeItem(bag::BAGTYPE(page),acItem,1);
                                        SAV.Bag.addItem(ItemList[pkmnOldItem].itemtype,pkmnOldItem,1);
                                    }
                                    else{
                                        goto OUT;
                                    }
                                }
                                else{
                                    ac.boxdata.Item = acItem;
                                    SAV.Bag.removeItem(bag::BAGTYPE(page),acItem,1);
                                }
                                
                                goto OUT;
                            }
                        }
                    }
                }
OUT:
                ;
            }
            else if( ret == 2){}
            else if(ret == 3) {}

            //todo: some code's still missing
            
            initOAMTable(oamTop);
            initOAMTableSub(oam);
            
            loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","BagTop");	
            
            consoleSelect(&Top);
            consoleSetWindow(&Top,0,0,32,24);
            consoleClear();
            for(int i= 0; i< 8; ++i){
                consoleSetWindow(&Top,0,3*i,12,3);
                int acIn = 214 + 4*i;
                int s = SAV.Bag.size((bag::BAGTYPE)i);
                printf("%c%c\n%c%c ",(acIn),(acIn+1),(acIn+2),(acIn+3));
                if(s == 0)      printf("  0 Items");
                else if( s==1)  printf("  1 Item");
                else            printf("%3i Items", s);
            }
            oam2 = oamIndex;
            pal2 = palcnt;
            tile2 = nextAvTileIdx;
            
            initBagSprites(oamTop,spriteInfoTop,oamIndexT,palcntT,nextAvTileIdxT,false); 
            updateOAM(oamTop);
            initBagSprites(oam,spriteInfo,oamIndex,palcnt,nextAvTileIdx);
            swiWaitForVBlank();
            updateOAMSub(oam);
            
            oam->oamBuffer[11].x = SCREEN_WIDTH - 22;
            oam->oamBuffer[11].y = SCREEN_HEIGHT - 49;
            oam->oamBuffer[11].priority = OBJPRIORITY_1;
            oam->oamBuffer[12].x = SCREEN_WIDTH - 49;
            oam->oamBuffer[12].y = SCREEN_HEIGHT - 22;
            oam->oamBuffer[12].priority = OBJPRIORITY_1;
            consoleSelect(&Bottom); 
            consoleSetWindow(&Bottom,0,0,32,24);
            printf("%c%c %s        \n%c%c %2i/%i",ind,ind+1,bagnames[page].c_str(),ind +2,ind +3,acpage+1, maxpage);
            
            oam->oamBuffer[11].isHidden = false;
            oam->oamBuffer[12].isHidden = false;
            swiWaitForVBlank();
            updateOAMSub(oam);

            consoleSetWindow(&Bottom,31,19,2,2);
            printf("%c\n%c",246,247);
            consoleSetWindow(&Bottom,27,22,2,2);
            printf("%c\n%c",248,249);
            if(0 == acpage){
                oam->oamBuffer[12].isHidden = true;
                updateOAMSub(oam);  
                consoleSetWindow(&Bottom,27,22,2,2);
                consoleClear();
            }
            if(maxpage == acpage +1){
                oam->oamBuffer[11].isHidden = true;
                updateOAMSub(oam);
                consoleSetWindow(&Bottom,31,19,2,2);
                consoleClear();
            }       
            getRanPerm(pos,cpy);
            for(int i = 0; i < MAXPERM; ++i)
            pos[i] = cpy[i];
            
            for(int j = 0; j <= 10; ++j)
            oam->oamBuffer[j].isHidden = false;
            swiWaitForVBlank();
            updateOAMSub(oam);
            for(int i= acpage * 12; i< acpage * 12+ std::min(int(SAV.Bag.size(bag::BAGTYPE(page))) - acpage * 12,12);++i){
                updateTime();
                scanKeys();
                touchRead(&t);
                if((sqrt(sq(SCREEN_WIDTH - 6 -t.px) + sq(SCREEN_HEIGHT - 33-t.py)) <= 16) && acpage < maxpage - 1)
                goto NEXT;
                if(sqrt(sq(SCREEN_WIDTH - 33 -t.px) + sq(SCREEN_HEIGHT - 6-t.py)) <= 16 && acpage > 0)
                goto PREV;
                if ( t.px>224 && t.py>164)
                goto BACK;
                auto acElem = SAV.Bag.element_at(bag::BAGTYPE(page),i);
                drawItem(oam,spriteInfo,ItemList[acElem.first].Name,38 + (pos[i - (acpage*12)]%4*48),
                18 + (pos[i - (acpage*12)]/4)*40,acElem.second,oam2,pal2,tile2,true,page == item::KEY_ITEM);
                updateTime();
            }
        }
    }
}
void bag::draw(){
    vramSetup();
    videoSetMode(MODE_5_2D |DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );		
    Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256,2,0, true ,true);
    consoleSetFont(&Top,&cfont);
    
    Bottom = *consoleInit(&Bottom,  0, BgType_Text4bpp, BgSize_T_256x256, 2,0, false,true );
    consoleSetFont(&Bottom, &cfont);
    
    touchPosition t;
    loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","BagTop");	

    consoleSelect(&Top);
    for(int i= 0; i< 8; ++i){
        consoleSetWindow(&Top,0,3*i,12,3);
        int acIn = 214 + 4*i;
        int s = (int)this->bags[i].size();
        printf("%c%c\n%c%c ",(acIn),(acIn+1),(acIn+2),(acIn+3));
        if(s == 0)
        printf("  0 Items");
        else if( s==1)
        printf("  1 Item");
        else
        printf("%3i Items", s);
    }
    if(!BGs[BG_ind].load_from_rom){
        dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
    }
    else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name.c_str(),BG_ind)){
        dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
        dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
        BG_ind = 0;
    }
    initOAMTableSub(oam);
    int palcnt = 0,tilecnt = 0,oamInd = 0;
    initBagSprites(oam,spriteInfo,oamInd,palcnt,tilecnt);


    initOAMTable(oamTop);
    int palcntT = 0,tilecntT = 0,oamIndT = 0;
    initBagSprites(oamTop,spriteInfoTop,oamIndT,palcntT,tilecntT,false);    
    
    consoleSetWindow(&Bottom,0,0,32,24);
    consoleSelect(&Bottom);
    consoleClear();

    int positions[8][2] = {{112,32},{144,48},{160,80},{144,112},{112,128},{80,112},{64,80},{80,48}};
    //Goods,Keys,TMs,Mails,Medicine,Berries,PokéBalls,BattleItems
    for(int i= 11; i< 19; ++i){
        oam->oamBuffer[i+2].x = positions[i-11][0];
        oam->oamBuffer[i+2].y = positions[i-11][1];
        oam->oamBuffer[i+2].isHidden = false;
        updateOAMSub(oam);
        
        int nx = positions[i-11][0]/8 + 1, ny = positions[i-11][1]/ 8 +1,ind = 214 + 4 * (i-11);
        consoleSetWindow(&Bottom,nx,ny,2,2);
        for(int p= 0; p < 4; ++p)
        printf("%c",ind+p);
    }
    int pos[MAXPERM],cpy[MAXPERM] = {0};
    for(int i= 0; i< MAXPERM; ++i)
    pos[i] = i;
    getRanPerm(pos,cpy);
    for(int i = 0; i < MAXPERM; ++i)
    pos[i] = cpy[i];

    while(42){
        updateTime();
        swiWaitForVBlank();
        updateOAMSub(oam);
        scanKeys();
        touchRead(&t);
        //int pressed = keysUp(),held = keysHeld();
        if ( t.px>224 && t.py>164)
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
            break;
        }
        for(int i = 0; i < 8; ++i)
        if(sqrt(sq(16+positions[i][0]-t.px) + sq(16+positions[i][1]-t.py)) <= 16){
            while(1)
            {
                scanKeys();
                swiWaitForVBlank();
                updateTime();
                if(keysUp() & KEY_TOUCH)
                break;
            }
            drawBagPage(i,pos,oamInd,palcnt,tilecnt,oamIndT,palcntT,tilecntT);
            
            //initOAMTableSub(oam);
            /*
                initBagSprites(oam,spriteInfo,oamInd,palcnt,tilecnt);
                initBagSprites(oamTop,spriteInfoTop,oamIndT,palcntT,tilecntT,false);  

                loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","BagTop");
                consoleSelect(&Top);
                for(int i= 0; i< 8; ++i){
                    consoleSetWindow(&Top,0,3*i,12,3);
                    int acIn = 214 + 4*i;
                    int s = (int)this->bags[i].size();
                    printf("%c%c\n%c%c ",(acIn),(acIn+1),(acIn+2),(acIn+3));
                    if(s == 0)
                        printf("  0 Items");
                    else if( s==1)
                        printf("  1 Item");
                    else
                        printf("%3i Items", s);
                }
                if(!BGs[BG_ind].load_from_rom){
                    dmaCopy(BGs[BG_ind].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
                    dmaCopy(BGs[BG_ind].MainMenuPal, BG_PALETTE_SUB, 256*2); 
                }
                else if(!loadNavScreen(bgGetGfxPtr(bg3sub),BGs[BG_ind].Name,BG_ind)){
                    dmaCopy(BGs[0].MainMenu, bgGetGfxPtr(bg3sub), 256*256);
                    dmaCopy(BGs[0].MainMenuPal, BG_PALETTE_SUB, 256*2); 
                    BG_ind = 0;
                }*/
            
            consoleSetWindow(&Bottom,0,0,32,24);
            consoleSelect(&Bottom);
            consoleClear();
            for(int j = 0; j <= 10; ++j)
            oam->oamBuffer[j].isHidden = false;
            for(int j = 11; j <= 60; ++j)
            oam->oamBuffer[j].isHidden = true;
            for(int j= 11; j< 19; ++j){
                oam->oamBuffer[j+2].x = positions[j-11][0];
                oam->oamBuffer[j+2].y = positions[j-11][1];
                oam->oamBuffer[j+2].isHidden = false;
                updateOAMSub(oam);
                
                int nx = positions[j-11][0]/8 + 1, ny = positions[j-11][1]/ 8 +1,ind = 214 + 4 * (j-11);
                consoleSetWindow(&Bottom,nx,ny,2,2);
                for(int p= 0; p < 4; ++p)
                printf("%c",ind+p);
            }
            updateOAMSub(oam);
        }
    }
    
    consoleSetWindow(&Bottom,0,0,32,24);
    consoleSelect(&Bottom);
    consoleClear();
    initOAMTableSub(oam);
    initMainSprites(oam,spriteInfo);
    setMainSpriteVisibility(false);
    oam->oamBuffer[8].isHidden = true;
    oam->oamBuffer[0].isHidden = true;
    oam->oamBuffer[1].isHidden = false;
}