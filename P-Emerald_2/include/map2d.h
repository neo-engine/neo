#pragma once
#include <nds.h>
#include <nds/ndstypes.h>
#include <vector>

#include <stdio.h>

namespace map2d{
    extern int bgs[4];


    typedef struct { u16 pal[16]; } Palette;
    typedef struct{ u8 tile[32]; } Tile;
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


    };
    
    typedef struct{ Tile blocks[1024]; } TileSet;
    typedef struct{ Block blocks[1024]; } BlockSet;

    typedef struct{
        u16 blockidx      : 10;
        u8 movedata       :  6;  
    }MapBlockAtom;

    class Anbindung{
    public:
        char name[100];
        char direction;
        int move;
        int mapidx;

        int mapsx;
        int mapsy;

        Anbindung() {}
        Anbindung(const char name[100], char dir, int mv, int mpidx) 
            :  direction(dir), move(mv), mapidx(mpidx) {
            memcpy(this->name,name,100);
        }
    };
    class Animation{
    public:
        u16 tileIdx;
        u8 acFrame;
        u8 maxFrame;
        u8 speed;
        std::vector<Tile> animationTiles;
    };

    class Map{
    public: 
        u32 sizex,sizey;

        Palette pals[16];
        std::vector< std::vector <MapBlockAtom> > blocks;
        TileSet t;
        BlockSet b;
        std::vector< Anbindung> anbindungen;
        u16 rand[2][2];
        std::vector< Animation> animations;

        //WildePKMN, Events...
        Map() { }

        Map(const char* Path, const char* Name);

        void draw(int bx,int by,bool init = false);
        void movePlayer(int direction,bool clear = false);
    };
}