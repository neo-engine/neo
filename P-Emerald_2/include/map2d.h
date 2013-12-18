#pragma once
#include <nds.h>
#include <nds/ndstypes.h>
#include <vector>

#include <stdio.h>

namespace map2d{

    class Palette{
    public:
        u16 pal[16];
    };
    typedef struct{
        u8 tile[64];
    } Tile;
    typedef u8 PalNo;
    //typedef struct{
    //    u16 tileidx      : 10;
    //    u8 vflip         :  1;
    //    u8 hflip         :  1;
    //    PalNo palno      :  4;
    //}BlockAtom;
    typedef u16 BlockAtom;

    class Block{
    public:
        BlockAtom top[2][2];
        u8 topbehave;

        BlockAtom bottom[2][2];
        u8 bottombehave;

        void draw(int x, int y);
    };
    
    class TileSet{
    public:
        Tile blocks[1024];
    };
    class BlockSet{
    public:
        Block blocks[1024];
    };

    
    typedef struct{
        u16 blockidx      : 10;
        u8 movedata       :  6;
    }MapBlockAtom;
    class Map{
    public:
        u8 sizex,sizey;

        Palette pals[16];
        std::vector< std::vector <MapBlockAtom> > blocks;
        TileSet t;
        BlockSet b;


        //WildePKMN, Events...
        Map() { }

        Map(int sizex,int sizey,Palette pals[16],u8* blocks,u8* move, TileSet t,BlockSet b) 
            : sizex(sizex), sizey(sizey), t(t), b(b){
                for(int i= 0; i < 16; ++i)
                    this->pals[i] = pals[i];
                this->blocks.assign(sizex,std::vector<MapBlockAtom>(sizey));
                int c = 0;
                for(int i= 0; i < sizex; ++i)
                    for(int j= 0; j < sizey; ++j,++c){
                        this->blocks[i][j].blockidx = blocks[c];
                        this->blocks[i][j].movedata = move[c];
                    }
            }
        Map(const char* Path, const char* Name);

        void draw(int bx,int by);
    };
}