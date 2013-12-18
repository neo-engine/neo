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


namespace map2d{
    inline void readPal(FILE* file, Palette& pal){
        if(file == 0)
            return;
        fread(pal.pal,sizeof(u16),16,file);
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
        
        fread(&this->sizex,sizeof(u8),1,mapF);
        readNop(mapF,3);
        fread(&this->sizey,sizeof(u8),1,mapF);
        readNop(mapF,3);

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
        
        int pal[16] = {1,0x7fff,0x779B,0x7337,0x6ED4,0x4610,0x3D8C,0x3528,
            0x42F7,0x7AF1,0x76AF,0x7AF3,0x6E8F,0x622D,0x55CC,0x4315};
        
        for(int j= 0; j < 16; ++j)
        for(int i= 0; i < 16; ++i)
            this->pals[j].pal[i] = pal[i];
        pals[0].pal[0] = 0;


        readNop(mapF,40);
        this->blocks.assign(this->sizex,std::vector<MapBlockAtom>(this->sizey));

        for(int x = 0; x < sizex; ++x)
            for(int y = 0; y < sizey; ++y)
                fread(&this->blocks[x][y],sizeof(MapBlockAtom),1,mapF);

    }

    void Map::draw(int bx,int by){
        videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
        vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
        
        REG_BG0CNT = BG_32x32 | BG_COLOR_256 |  BG_MAP_BASE(0) | BG_TILE_BASE(1)|  BG_PRIORITY(0);
        REG_BG1CNT = BG_32x32 | BG_COLOR_256 |   BG_MAP_BASE(1) | BG_TILE_BASE(1)| BG_PRIORITY(1);
        REG_BG2CNT = BG_32x32 | BG_COLOR_256 |   BG_MAP_BASE(2) | BG_TILE_BASE(1)| BG_PRIORITY(2);
        REG_BG3CNT = BG_32x32 | BG_COLOR_256 |   BG_MAP_BASE(3) | BG_TILE_BASE(1)| BG_PRIORITY(3);

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
            swiCopy(this->t.blocks[i].tile, tileMemory + i * 64, 32);
        dmaCopy(this->pals, BG_PALETTE, 256*2); 
        
        for(int i= 0; i < 32*32; ++i){
            mapMemory[0][i] = 0;
            mapMemory[1][i] =mapMemory[2][i] =mapMemory[3][i] = 0;
        }

        int c = 0;
        for(int x = by; x < by + 12; x++){
            for(int y = bx; y < bx + 16; y++){
                int toplayer = 0, bottomlayer = 3;

                mapMemory[toplayer][c] = (this->b.blocks[blocks[x][y].blockidx].top[0][0]);
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(toplayer))[(8*x +u + (v+8*y) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[0][0].palno;

                mapMemory[toplayer][c+1] = this->b.blocks[blocks[x][y].blockidx].top[0][1];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(toplayer))[(8*(x+1) +u + (v+8*(y)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[0][1].palno;

                mapMemory[toplayer][c+32] = this->b.blocks[blocks[x][y].blockidx].top[1][0];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(toplayer))[(8*(x) +u + (v+8*(y+1)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[1][0].palno;

                mapMemory[toplayer][c+33] = this->b.blocks[blocks[x][y].blockidx].top[1][1];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(toplayer))[(8*(x+1) +u + (v+8*(y+1)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[1][1].palno;


                mapMemory[bottomlayer][c] = this->b.blocks[blocks[x][y].blockidx].bottom[0][0];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(bottomlayer))[(8*(x) +u + (v+8*(y)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[0][0].palno;

                mapMemory[bottomlayer][c+1] = this->b.blocks[blocks[x][y].blockidx].bottom[0][1];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(bottomlayer))[(8*(x+1) +u + (v+8*(y)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[0][1].palno;

                mapMemory[bottomlayer][c+32] = this->b.blocks[blocks[x][y].blockidx].bottom[1][0];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(bottomlayer))[(8*(x) +u + (v+8*(y+1)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[1][0].palno;

                mapMemory[bottomlayer][c+33] = this->b.blocks[blocks[x][y].blockidx].bottom[1][1];
                //for(int u = 0; u < 8; ++u) for(int v = 0; v < 8; ++v)
                //    ((u16 *)BG_BMP_RAM(bottomlayer))[(8*(x+1) +u + (v+8*(y+1)) * SCREEN_WIDTH)/2] <<= this->b.blocks[blocks[x][y].blockidx].top[1][1].palno;

                c += 2;
            }
            c += 32;
        }
        bgUpdate();
        swiWaitForVBlank();
    }
}