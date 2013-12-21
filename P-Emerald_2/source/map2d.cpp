#include "map2d.h"

#include "scrnloader.h"

#include <fstream>
#include <vector>
#include <fat.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>


extern PrintConsole Top,Bottom;

namespace map2d{
#define PLAYERSPRITE 0

    inline void readPal(FILE* file, Palette pal[16]){
        if(file == 0)
            return;
        for(int i= 0; i < 16; ++i)
            fread(pal[i].pal,sizeof(u16),16,file);
    }
    inline void readTileSet(FILE* file, TileSet& tileSet, int startidx = 0, int size = 512){
        if(file == 0)
            return;
        fread(&tileSet.blocks[startidx],sizeof(Tile),size,file);
    }
    inline void readNop(FILE* file, int cnt){
        if(file == 0)
            return;
        fread(0,sizeof(u8),cnt,file);
    }
    inline void readBlockSet(FILE* file, BlockSet& tileSet, int startidx = 0, int size = 512){
        if(file == 0)
            return;
        readNop(file,4); 
        for(int i = 0; i < size; ++i){
            fread(tileSet.blocks[startidx+i].bottom,sizeof(BlockAtom),4,file);
            fread(tileSet.blocks[startidx+i].top,sizeof(BlockAtom),4,file);
        }
        for(int i = 0; i < size; ++i){
            fread(&tileSet.blocks[startidx+i].bottombehave,sizeof(u8),1,file);
            fread(&tileSet.blocks[startidx+i].topbehave,sizeof(u8),1,file);
        }
    }

    Map::Map(const char* Path, const char* Name){
        char buf[100]; 
        sprintf(buf,"%s%s.m2p",Path,Name);
        FILE* mapF = fopen(buf,"rb");
        if(mapF == 0)
            return;
        
        fread(&this->sizey,sizeof(u32),1,mapF);
        fread(&this->sizex,sizeof(u32),1,mapF);

        u8 tsidx1, tsidx2;
        fread(&tsidx1,sizeof(u8),1,mapF);  
        readNop(mapF,3);
        fread(&tsidx2,sizeof(u8),1,mapF);
        readNop(mapF,3);
                
        sprintf(buf,"nitro://MAPS/TILESETS/%i.ts",tsidx1);
        readTileSet(fopen(buf,"rb"),this->t);
        sprintf(buf,"nitro://MAPS/TILESETS/%i.bvd",tsidx1);
        readBlockSet(fopen(buf,"rb"),this->b);

        sprintf(buf,"nitro://MAPS/TILESETS/%i.ts",tsidx2);
        readTileSet(fopen(buf,"rb"),this->t,512);
        sprintf(buf,"nitro://MAPS/TILESETS/%i.bvd",tsidx2);
        readBlockSet(fopen(buf,"rb"),this->b,512);
        

        sprintf(buf,"nitro://MAPS/TILESETS/%i.p2l",tsidx1);
        readBlockSet(fopen(buf,"rb"),this->b,512);
        readPal(fopen(buf,"rb"),this->pals);

        readNop(mapF,4);
        this->blocks.assign(this->sizex,std::vector<MapBlockAtom>(this->sizey));

        for(int x = 0; x < sizex; ++x)
            for(int y = 0; y < sizey; ++y)
                fread(&this->blocks[x][y],sizeof(MapBlockAtom),1,mapF);
        
    }

    void Map::draw(int bx,int by){
        videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE |
            DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
        
        REG_BG0CNT = BG_32x32 | BG_COLOR_256 |  BG_MAP_BASE(0) | BG_TILE_BASE(1)|  BG_PRIORITY(0);
        REG_BG1CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(1) | BG_TILE_BASE(1)| BG_PRIORITY(1);
        REG_BG2CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(2) | BG_TILE_BASE(1)| BG_PRIORITY(2);
        REG_BG3CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(3) | BG_TILE_BASE(1)| BG_PRIORITY(3);

        //Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 5, true ,true);

        int bg[4];
        u16* mapMemory[4];

        for(int i= 0; i< 4; ++i){
            //bg[i] = bgInit( i, BgType_Bmp8, BgSize_B8_256x256, i, 1);
            //loadPicture(bgGetGfxPtr(bg[i]),"nitro:/PICS/","ClearD");
            mapMemory[i] = (u16*)BG_MAP_RAM(i);
        }
        bgUpdate();
        u8* tileMemory = (u8*)BG_TILE_RAM(1);
        
        for(int i= 0; i < 1024; ++i)
            swiCopy(this->t.blocks[i].tile, tileMemory + i * 32, 16);

        dmaCopy(this->pals, BG_PALETTE, 512); 
        
        for(int i= 0; i < 32*32; ++i){
            mapMemory[0][i] = 0;
            mapMemory[1][i] =mapMemory[2][i] =mapMemory[3][i] = 0;
        }
          
        int c = 0;
        for(int x = by; x < by + 13; x++){
            for(int y = bx; y < bx + 17; y++){
                int toplayer = 1, bottomlayer = 3;

                Block acBlock = this->b.blocks[blocks[x][y].blockidx];
                if(x < 0 || y < 0 ||x >= this->sizex || y >= this->sizey)
                    acBlock  = this->rand[x%2][y%2];
                else{
                    //consoleSelect(&Top);
                    //consoleSetWindow(&Top,2*(y-bx),2*(x-by),2,2);
                    //printf("%i %i",acBlock.top[0][0]>>12,acBlock.bottom[0][0]>>12);
                }

                if(acBlock.bottombehave == 0x10)
                    toplayer++;

                if(x > by && y > bx)
                    mapMemory[toplayer][c-33] = (acBlock.top[0][0]);
                if(x > by && y < bx + 16)
                    mapMemory[toplayer][c-32] = acBlock.top[0][1];
                if(x < by +12 && y > bx)
                    mapMemory[toplayer][c-1] = acBlock.top[1][0];
                if(x < by +12&& y < bx + 16)
                    mapMemory[toplayer][c] = acBlock.top[1][1];
                
                
                if(x > by && y > bx)
                    mapMemory[bottomlayer][c-33] = acBlock.bottom[0][0];
                if(x > by && y < bx + 16)
                    mapMemory[bottomlayer][c-32] = acBlock.bottom[0][1];
                if(x < by +12 && y > bx)
                    mapMemory[bottomlayer][c-1] = acBlock.bottom[1][0];
                if(x < by +12&& y < bx + 16)
                    mapMemory[bottomlayer][c] = acBlock.bottom[1][1];

                if(y < bx + 16)
                    c+=2;
            }
            c += 32;
        }
        bgUpdate();
        consoleSelect(&Bottom);
        swiWaitForVBlank();
    }
}