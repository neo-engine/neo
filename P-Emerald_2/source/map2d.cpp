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

    inline void readPal(FILE* file, Palette* pal){
        if(file == 0)
            return;
        for(int i= 0; i < 6; ++i)
            fread(pal[i].pal,sizeof(u16),16,file);
        fclose(file);
    }
    inline void readTileSet(FILE* file, TileSet& tileSet, int startidx = 0, int size = 512){
        if(file == 0)
            return;
        fread(&tileSet.blocks[startidx],sizeof(Tile),size,file);
        fclose(file);
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
        fclose(file);
    }
    inline void readAnimations(FILE* file, std::vector<Animation>& animations){
        if(file == 0)
            return;
        u8 N; 
        fread(&N,sizeof (u8),1,file);
        for(int i= 0; i < N; ++i){
            Animation a;            
            fread(&a.tileIdx,sizeof (u16),1,file);
            fread(&a.speed,sizeof (u8),1,file);
            fread(&a.maxFrame,sizeof (u8),1,file);
            a.acFrame = 0;
            a.animationTiles.assign(a.maxFrame, Tile());
            for(int i = 0; i < a.maxFrame; ++i)
                fread(&a.animationTiles[i],sizeof(Tile),1,file);
            animations.push_back(a);
        }
        fclose(file);
    }
    Map::Map(const char* Path, const char* Name){
        consoleSelect(&Bottom);
        consoleSetWindow(&Bottom,4,4,25,20);
        consoleClear();

        char buf[100]; 
        sprintf(buf,"%s%s.m2p",Path,Name);
        FILE* mapF = fopen(buf,"rb");
        if(mapF == 0)
            return;

        //printf("TEST1\n");        
        
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
        sprintf(buf,"nitro://MAPS/TILESETS/%i.p2l",tsidx1);
        readPal(fopen(buf,"rb"),this->pals);
        sprintf(buf,"nitro://MAPS/TILESETS/%i.anm",tsidx1);
        readAnimations(fopen(buf,"rb"),this->animations);

        sprintf(buf,"nitro://MAPS/TILESETS/%i.ts",tsidx2);
        readTileSet(fopen(buf,"rb"),this->t,512); 
        sprintf(buf,"nitro://MAPS/TILESETS/%i.bvd",tsidx2);
        readBlockSet(fopen(buf,"rb"),this->b,512);
        sprintf(buf,"nitro://MAPS/TILESETS/%i.p2l",tsidx2);
        readPal(fopen(buf,"rb"),this->pals + 6);
        sprintf(buf,"nitro://MAPS/TILESETS/%i.anm",tsidx2);
        readAnimations(fopen(buf,"rb"),this->animations);
        

        readNop(mapF,4);
        this->blocks.assign(this->sizex+20,std::vector<MapBlockAtom>(this->sizey+20));
             
        //printf("TEST2\n");

        sprintf(buf,"%s%s.anb",Path,Name);
        FILE* A = fopen(buf,"r");
        if(A){ 
            int N; fscanf(A,"%d",&N); 
            this->anbindungen = std::vector<Anbindung>(N);
            //printf("TEST3 : %i\n",N);
            for(int i= 0; i < N; ++i){
                Anbindung& ac = this->anbindungen[i];
                fscanf(A,"%s %c %d %d",ac.name, &ac.direction,&ac.move,&ac.mapidx);
                //printf("TEST4 : %s %c %d %d\n",ac.name, ac.direction,ac.move,ac.mapidx);

                sprintf(buf,"%s%s.m2p",Path,ac.name);
                FILE* mapF2 = fopen(buf,"rb");
                if(mapF2 == 0)
                    continue;
                fread(&ac.mapsy,sizeof(u32),1,mapF2);
                fread(&ac.mapsx,sizeof(u32),1,mapF2);
                
                readNop(mapF2,12);
                for(int x = 0; x < ac.mapsx; ++x)
                    for(int y = 0; y < ac.mapsy; ++y){
                        if(ac.direction == 'W' && y >= ac.mapsy - 10 && x + ac.move + 10 >= 0 && x + ac.move < (int)sizex + 10)
                            fread(&(blocks[x + ac.move + 10][y - ac.mapsy + 10]),sizeof(MapBlockAtom),1,mapF2);

                        else if(ac.direction == 'N' && x - ac.mapsx + 10 >= 0 && y + ac.move >= -10 && y + ac.move < (int)sizey + 10)
                            fread(&(blocks[x - ac.mapsx + 10][y + ac.move + 10]),sizeof(MapBlockAtom),1,mapF2);
                        
                        else if(ac.direction == 'E' && y < 10 && x + ac.move +10 >= 0 && x + ac.move < (int)sizex + 10)
                            fread(&(blocks[x + ac.move + 10][y+sizey+10]),sizeof(MapBlockAtom),1,mapF2);

                        else if(ac.direction == 'S' && x < 10 && y + ac.move +10 >= 0 && y + ac.move < (int)sizey + 10)
                            fread(&(blocks[x+sizex+10][y + ac.move + 10]),sizeof(MapBlockAtom),1,mapF2);
                        else{
                            readNop(mapF2,sizeof(MapBlockAtom));
                        }
                    } 
                fclose(mapF2);
            }
            consoleSelect(&Bottom);
            for(int i= 0; i< 2; ++i)
                for(int j= 0; j < 2; ++j){
                    if(fscanf(A,"%hu",&rand[i][j]) == EOF)
                        rand[i][j] = 0;
                }
            fclose(A);
        }

        for(int x = 0; x < (int)sizex; ++x)
            for(int y = 0; y < (int)sizey; ++y)
                fread(&this->blocks[x+10][y+10],sizeof(MapBlockAtom),1,mapF);
        //printf("Test5 %i",this->anbindungen.size());
        fclose(mapF);
    }

    int tcnt = 0;
    int bgs[4];

    int lastrow, lastcol;
    int lastbx, lastby;

    void Map::fill(u16* mapMemory[4],int xmin,int x,int xmax,int ymin,int y, int ymax, int c){
        int toplayer = 1,betw = 2, bottomlayer = 3;

        Block acBlock;
        if(x < 0 || y < 0 ||x >= (int)this->sizex + 20 || y >= (int)this->sizey + 20)
            acBlock = this->b.blocks[rand[x%2][y%2]];                    
        else if((x < 10 || y < 10 ||x >= (int)this->sizex + 10 || y >= (int)this->sizey + 10) && blocks[x][y].blockidx == 0)
            acBlock = this->b.blocks[rand[x%2][y%2]];                    
        else
            acBlock = this->b.blocks[blocks[x][y].blockidx];
 
        if(acBlock.topbehave != 0x10)
            std::swap(toplayer,betw);

        if(x > xmin && y > ymin){
            mapMemory[toplayer][c-33] = 0;
            mapMemory[betw][c-33] = (acBlock.top[0][0]);
            mapMemory[bottomlayer][c-33] = acBlock.bottom[0][0];
        }
        if(x > xmin && y < ymax - 1){
            mapMemory[toplayer][c-32] = 0;
            mapMemory[betw][c-32] = acBlock.top[0][1];
            mapMemory[bottomlayer][c-32] = acBlock.bottom[0][1];
        }
        if(x < xmax - 1 && y > ymin){
            mapMemory[toplayer][c-1] = 0;
            mapMemory[betw][c-1] = acBlock.top[1][0];
            mapMemory[bottomlayer][c-1] = acBlock.bottom[1][0];
        }
        if(x < xmax -1 && y < ymax - 1){
            mapMemory[toplayer][c] = 0;
            mapMemory[betw][c] = acBlock.top[1][1];
            mapMemory[bottomlayer][c] = acBlock.bottom[1][1];
        }
    }

    void Map::movePlayer(int direction) {
        int c = 0;
        u16* mapMemory[4];
        for(int i= 1; i< 4; ++i)  mapMemory[i] = (u16*)BG_MAP_RAM(2*i-1);

        int bx = lastbx, by = lastby;

        int xmin,xmax,ymin,ymax,plsval;
        int lc = lastcol, lr = lastrow;

        switch(direction) 
        {
        case 1: 
            lastrow = (lastrow + 1) % 32;
            c = lastrow * 2;
            lastbx++;
            bx += 16;

            xmin = lastby;
            xmax = lastby + 17;
            ymin = lastbx+15;
            ymax = lastbx + 17;
            plsval = 60;
            
            break;
        case 3:
            c = lastrow * 2;
            lastrow = (lastrow + 31) % 32;
            lastbx--;
            bx += 14;

            xmin = lastby;
            xmax = lastby + 17;
            ymin = lastbx ;
            ymax = lastbx + 2;
            plsval = 60;
            
            break;
        case 2:
            lastcol = (lastcol + 1) % 16;
            c = lastcol * 64;
            lastby++;
            by += 16;

            xmin = lastby + 15;
            xmax = lastby + 17;
            ymin = lastbx;
            ymax = lastbx + 17;
            plsval = 32;
            
            break;
        case 4:
            c = lastcol * 64;
            lastcol = (lastcol + 15) % 16;

            lastby--;
            by += 14;

            xmin = lastby;
            xmax = lastby + 2;
            ymin = lastbx;
            ymax = lastbx + 17;
            plsval = 32;  
            
        }
        int c2 = c;
        if(direction == 1){
            ymin += 16;
            ymax += 16;
        }
        for(int i= 0; i < 2; ++i){
            if((direction % 2 == 0) || (i == ((lastrow+(direction == 3 ? 1:0))%32) / 16))
                for(int x = xmin; x < xmax; x++){
                    for(int y = ymin; y < ymax; y++){
                        
                        fill(mapMemory,xmin,x,xmax,ymin,y,ymax,c);
                
                        if((direction % 2 == 1 && y < bx + 16 * ( 2+ i)) || (direction % 2 == 0 && y < bx + 16 * (1 + i)))
                            c+=2;
                    }
                    c += plsval;
                }
            c = c2 + 1024 - 32;
            if(direction % 2 == 0){
                c += 32;
                ymin += 16;
                ymax += 16;
            }
        }
        switch(direction) {
        case 2: case 4:
            for(int i = 1; i < 4; ++i){
                u16 q[64];
                for(int g = 0; g < 32; ++g)
                    q[g] = mapMemory[i][c2 + g];
                for(int g = 0; g < 32; ++g)
                    q[g + 32] = mapMemory[i][c2 + g + 1024];
                for(int o = 0; o < 32; ++o)
                    mapMemory[i][c2 + o] = q[(o + 64 - 2*((lr+1)%32))%64];
                for(int o = 0; o < 32; ++o)
                    mapMemory[i][c2 + o + 1024] = q[(o + 96 - 2*((lr+1)%32))%64];
            }
            c2 += 32;
            for(int i = 1; i < 4; ++i){
                u16 q[64];
                for(int g = 0; g < 32; ++g)
                    q[g] = mapMemory[i][c2 + g];
                for(int g = 0; g < 32; ++g)
                    q[g + 32] = mapMemory[i][c2 + g + 1024];
                for(int o = 0; o < 32; ++o)
                    mapMemory[i][c2 + o] = q[(o + 64 - 2*((lr+1)%32))%64];
                for(int o = 0; o < 32; ++o)
                    mapMemory[i][c2 + o + 1024] = q[(o + 96 - 2*((lr+1)%32))%64];
            }
            break;
        case 1: case 3:
            c2 += (((lastrow+(direction == 3 ? 1:0))%32) / 16) * (1024 - 32);

            for(int i = 1; i < 4; ++i){
                u16 q[32];
                for(int g = 0; g < 32; ++g)
                    q[g] = mapMemory[i][c2 + 32*g];
                for(int o = 0; o < 32; ++o)
                    mapMemory[i][c2 + 32*o] = q[(o + 32 - 2*(lc+1))%32];
            }
            c2++;
            for(int i = 1; i < 4; ++i){
                u16 q[32];
                for(int g = 0; g < 32; ++g)
                    q[g] = mapMemory[i][c2 + 32*g];
                for(int o = 0; o < 32; ++o)
                    mapMemory[i][c2 + 32*o] = q[(o + 32 - 2*(lc+1))%32];
            }
            break;
        }
    }
    
    void Map::draw(int bx,int by,bool init){
        if(init){
            videoSetMode(MODE_0_2D /*| DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE |
                DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
            vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
        
            //REG_BG0CNT = BG_32x32 | BG_COLOR_16 |  BG_MAP_BASE(0) | BG_TILE_BASE(1)|  BG_PRIORITY(0);
            //REG_BG1CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(1) | BG_TILE_BASE(1)| BG_PRIORITY(1);
            //REG_BG2CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(2) | BG_TILE_BASE(1)| BG_PRIORITY(2);
            //REG_BG3CNT = BG_32x32 | BG_COLOR_16 |   BG_MAP_BASE(3) | BG_TILE_BASE(1)| BG_PRIORITY(3);

            for(int i= 1; i < 4; ++i){
                bgs[i] = bgInit(i, BgType_Text4bpp, BgSize_T_512x256, 2*i - 1, 1);
                bgSetPriority(bgs[i],i);
                bgScroll(bgs[i],128,32);
            }

            //Top = *consoleInit(&Top, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 5, true ,true);

            u8* tileMemory = (u8*)BG_TILE_RAM(1);
        
            for(int i= 0; i < 1024; ++i)
                swiCopy(this->t.blocks[i].tile, tileMemory + i * 32, 16);

            dmaCopy(this->pals, BG_PALETTE, 512); 
        }
        u16* mapMemory[4];
        for(int i= 1; i< 4; ++i)  mapMemory[i] = (u16*)BG_MAP_RAM(2*i-1);
        
        lastrow = 31;
        lastcol = 15;

        int c = 0;
        bx += 0;
        by += 10;
        
        
        lastbx = bx;
        lastby = by;
        for(int i= 0; i < 2; ++i){
            for(int x = by; x < by + 17; x++){
                for(int y = bx; y < bx + 17; y++){
                    int toplayer = 1, bottomlayer = 3;

                    Block acBlock = this->b.blocks[blocks[x][y].blockidx];

                    //if(tile_deb_test){
                    //    acBlock = this->b.blocks[tcnt];
                    //    tcnt = (tcnt +1)%1024;
                    //}
                    if(x < 0 || y < 0 ||x >= (int)this->sizex + 20 || y >= (int)this->sizey + 20){
                        acBlock = this->b.blocks[rand[x%2][y%2]];
                    }
                    else if((x < 10 || y < 10 ||x >= (int)this->sizex + 10 || y >= (int)this->sizey + 10) && blocks[x][y].blockidx == 0){
                        acBlock = this->b.blocks[rand[x%2][y%2]];
                    }
                    else{
                        //consoleSelect(&Top);
                        //consoleSetWindow(&Top,2*(y-bx)-1,2*(x-by)-1,2,2);
                        //printf("%i",acBlock.topbehave);
                    }
                
                    if(acBlock.topbehave == 0x10){                    
                        if(x > by && y > bx)
                            mapMemory[toplayer + 1][c-33] = (acBlock.top[0][0]);
                        if(x > by && y < bx + 16)
                            mapMemory[toplayer + 1][c-32] = acBlock.top[0][1];
                        if(x < by +16 && y > bx)
                            mapMemory[toplayer + 1][c-1] = acBlock.top[1][0];
                        if(x < by +16&& y < bx + 16)
                            mapMemory[toplayer + 1][c] = acBlock.top[1][1];

                        if(x > by && y > bx)
                            mapMemory[toplayer][c-33] = 0;
                        if(x > by && y < bx + 16)
                            mapMemory[toplayer][c-32] = 0;
                        if(x < by +16 && y > bx)
                            mapMemory[toplayer][c-1] = 0;
                        if(x < by +16&& y < bx + 16)
                            mapMemory[toplayer][c] = 0;
                    }
                    else{
                        if(x > by && y > bx)
                            mapMemory[toplayer][c-33] = (acBlock.top[0][0]);
                        if(x > by && y < bx + 16)
                            mapMemory[toplayer][c-32] = acBlock.top[0][1];
                        if(x < by +16 && y > bx)
                            mapMemory[toplayer][c-1] = acBlock.top[1][0];
                        if(x < by +16&& y < bx + 16)
                            mapMemory[toplayer][c] = acBlock.top[1][1];
                    
                        if(x > by && y > bx)
                            mapMemory[toplayer+1][c-33] = 0;
                        if(x > by && y < bx + 16)
                            mapMemory[toplayer+1][c-32] = 0;
                        if(x < by +16 && y > bx)
                            mapMemory[toplayer+1][c-1] = 0;
                        if(x < by +16&& y < bx + 16)
                            mapMemory[toplayer+1][c] = 0;
                    }
                
                    if(x > by && y > bx)
                        mapMemory[bottomlayer][c-33] = acBlock.bottom[0][0];
                    if(x > by && y < bx + 16)
                        mapMemory[bottomlayer][c-32] = acBlock.bottom[0][1];
                    if(x < by +16 && y > bx)
                        mapMemory[bottomlayer][c-1] = acBlock.bottom[1][0];
                    if(x < by +16&& y < bx + 16)
                        mapMemory[bottomlayer][c] = acBlock.bottom[1][1];
                
                    if(y < bx + 16)
                        c+=2;
                }
                c += 32;
            }
            c -= 64;
            bx += 16;
        }
        bgUpdate();
        consoleSelect(&Bottom);
        //swiWaitForVBlank();
    }
}