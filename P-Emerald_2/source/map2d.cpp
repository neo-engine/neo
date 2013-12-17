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
    inline void readTileSet(FILE* file, TileSet& tileSet, int startidx = 0, int size = 128){
        if(file == 0)
            return;
        fread(&tileSet.blocks[startidx],sizeof(Tile),size,file);
    }
    inline void readNop(FILE* file, int cnt){
        if(file == 0)
            return;
        fread(0,sizeof(u8),cnt,file);
    }
    inline void readBlockSet(FILE* file, BlockSet& tileSet, int startidx = 0, int size = 256){
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
        readTileSet(fopen(buf,"rb"),this->t,128);
        sprintf(buf,"nitro://MAPS/TILESETS/%i.bvd",tsidx2);
        readBlockSet(fopen(buf,"rb"),this->b,512);
        
        readNop(mapF,40);
        this->blocks.assign(this->sizex,std::vector<MapBlockAtom>(this->sizey));

        for(int x = 0; x < sizex; ++x)
            for(int y = 0; y < sizey; ++y)
                fread(&this->blocks[x][y],sizeof(MapBlockAtom),1,mapF);

    }

    void Map::draw(int bx,int by){
        videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);
        int bg[4];
        u16* mapMemory[4];

        for(int i= 0; i< 4; ++i){
            bg[i] = bgInit( i, BgType_Bmp8, BgSize_B8_256x256, i, 1);
            loadPicture(bgGetGfxPtr(bg[i]),"nitro:/PICS/","ClearD");
            mapMemory[i] = (u16*)BG_MAP_RAM(i);
        }
        bgUpdate();

        swiCopy(this->t.blocks,(u8*)BG_TILE_RAM(1),512);
        dmaCopy(this->pals, BG_PALETTE_SUB, 256*2); 

        int c = 0;
        for(int y = by; y < by + 12; y+=2)
            for(int x = bx; x < bx + 16; x+=2){
                int toplayer = 0, bottomlayer = 3;

                mapMemory[toplayer][c] = this->b.blocks[blocks[x][y].blockidx].top[0][0].tileidx;
                mapMemory[toplayer][c+1] = this->b.blocks[blocks[x][y].blockidx].top[0][1].tileidx;
                mapMemory[toplayer][c+16] = this->b.blocks[blocks[x][y].blockidx].top[1][0].tileidx;
                mapMemory[toplayer][c+17] = this->b.blocks[blocks[x][y].blockidx].top[1][1].tileidx;

                mapMemory[bottomlayer][c] = this->b.blocks[blocks[x][y].blockidx].bottom[0][0].tileidx;
                mapMemory[bottomlayer][c+1] = this->b.blocks[blocks[x][y].blockidx].bottom[0][1].tileidx;
                mapMemory[bottomlayer][c+16] = this->b.blocks[blocks[x][y].blockidx].bottom[1][0].tileidx;
                mapMemory[bottomlayer][c+17] = this->b.blocks[blocks[x][y].blockidx].bottom[1][1].tileidx;

                c += 2;
            }
    }
}