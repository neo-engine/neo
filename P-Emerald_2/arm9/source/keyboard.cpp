#include <nds.h>
#include <cmath>

#include "keyboard.h"
#include "scrnloader.h"
#include "print.h"

extern font::Font cust_font;
extern font::Font cust_font2;
extern SpriteInfo spriteInfo[SPRITE_COUNT];
extern SpriteInfo spriteInfoTop[SPRITE_COUNT];
extern OAMTable *oam,*oamTop;

std::wstring keyboard::getText(int length,const char* msg,bool time)
{
    drawSub();

    for(int i= 0; i< 4; ++i)
        oam->oamBuffer[31+2*i].isHidden = true;
    oam->oamBuffer[A_ID].isHidden = false;
    oam->oamBuffer[BWD_ID].isHidden = false;
    oam->oamBuffer[FWD_ID].isHidden = false;
    oam->oamBuffer[13].x = 256 - 24;
    oam->oamBuffer[13].y = 196 - 28 - 22;
    oam->oamBuffer[14].x = 256 - 28 - 18;
    oam->oamBuffer[14].y = 196 - 28;
    oam->oamBuffer[A_ID].x = 256 - 24;
    oam->oamBuffer[A_ID].y = 196 - 28;

    updateOAMSub(oam);

    cust_font.set_color(0,0);
    cust_font.set_color(251,1);
    cust_font.set_color(252,2);
    cust_font2.set_color(0,0);
    cust_font2.set_color(253,1);
    cust_font2.set_color(254,2);

    BG_PALETTE_SUB[250] = RGB15(31,31,31);
    BG_PALETTE_SUB[251] = RGB15(15,15,15);
    BG_PALETTE_SUB[252] = RGB15(3,3,3);
    BG_PALETTE_SUB[253] = RGB15(15,15,15);
    BG_PALETTE_SUB[254] = RGB15(31,31,31);
    font::putrec(0,0,256,63,true,false,250);

    if(msg != 0)
        cust_font.print_string(msg,8,4,true);

    draw();
    std::wstring out;
    consoleSelect(&Bottom);
    touchPosition t;
    int i = 0;
    while(1)
    {
        scanKeys();
        touchRead(&t);  
        if(time)
            updateTime();
        if(i < length && chars.count(std::pair<int,int>(t.px/8,t.py/8)))
        {
            while(1)
            {
                scanKeys();
                if(time)
                    updateTime();
                t = touchReadXY();
                if(t.px == 0 && t.py == 0)
                    break;
            }
            ++i;
            out += chars[std::pair<int,int>(t.px/8,t.py/8)];

            font::putrec(8*(13-i/3),40,8*(13-i/3+length + 1),56,true,false,250);
            cust_font.print_string(out.c_str(),8*(13-i/3),40,true);
            swiWaitForVBlank();
        }
        else if((sqrt(sq(248-t.px) + sq(162-t.py)) <= 16))
        {
            while(1)
            {
                scanKeys();
                if(time)
                    updateTime();
                t = touchReadXY();
                if(t.px == 0 && t.py == 0)
                    break;
            }
            ind = (ind +1) % MAXKEYBOARDS;
            undraw(); 
            if(msg != 0){                
                font::putrec(0,0,256,63,true,false,250);
                cust_font.print_string(msg,8,4,true);
            }
            draw(); 
            if(time)
                updateTime();

            font::putrec(8*(13-i/3),40,8*(13-i/3+length + 1),56,true,false,250);
            cust_font.print_string(out.c_str(),8*(13-i/3),40,true);
        }
        else if(sqrt(sq(248-t.px) + sq(184-t.py)) <= 16)
        {
            while(1)
            {
                scanKeys();
                if(time)
                    updateTime();
                t = touchReadXY();
                if(t.px == 0 && t.py == 0)
                    break;
            }
            undraw();
            initMainSprites(oam,spriteInfo);
            return out;
        }
        else if((sqrt(sq(220-t.px) + sq(184-t.py)) <= 16))
        {
            while(1)
            {
                scanKeys();
                if(time)
                    updateTime();
                t = touchReadXY();
                if(t.px == 0 && t.py == 0)
                    break;
            }
            out.erase((out.end() - 1));
            font::putrec(8*(13-i/3),40,8*(13-i/3+length + 1),56,true,false,250);
            --i;
            cust_font.print_string(out.c_str(),8*(13-i/3),40,true);
        }
        swiWaitForVBlank();
    }
}

void keyboard::undraw(){
    drawSub();
    BG_PALETTE_SUB[250] = RGB15(31,31,31);
    BG_PALETTE_SUB[251] = RGB15(15,15,15);
    BG_PALETTE_SUB[252] = RGB15(3,3,3);
    BG_PALETTE_SUB[253] = RGB15(15,15,15);
    BG_PALETTE_SUB[254] = RGB15(31,31,31);
    font::putrec(0,0,256,63,true,false,250);
}

void keyboard::draw(){
    consoleSelect(&Bottom);
    consoleSetWindow(&Bottom,4,11,26,13);

    switch (ind)
    {
    case 0://UPPER
        {
            int cnt = 0;
            int rw_cnt = 9,cl_cnt = 3;
            chars.clear();
            for (char i = 'A'; i <= 'Z'; i++)
            {
                if(cnt++ == 13)
                {
                    cnt = 0;
                    cl_cnt = 3;
                    rw_cnt += 3;
                }
                cust_font2.print_char(i,cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = i;
                cl_cnt+=2;
            }
            rw_cnt+= 4;
            cl_cnt = 3;
            char pt[14] = "ÄÖÜ .!?()[]{}";
            for(int i= 0; i < 13; ++i){
                cust_font2.print_char(pt[i],cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = pt[i];
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = pt[i];
                cl_cnt+=2;
            }
            break;
        }
    case 1://LOWER
        {
            int cnt = 0;
            int rw_cnt = 9,cl_cnt = 3;
            for (char i = 'a'; i <= 'z'; i++)
            {
                if(cnt++ == 13)
                {
                    cnt = 0;
                    cl_cnt = 3;
                    rw_cnt += 3;
                }
                cust_font2.print_char(i,cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = i;
                cl_cnt+=2;
            }
            rw_cnt+= 4;
            cl_cnt = 3;
            char pt[14] = "äöüßé §$%&/\\#";
            for(int i= 0; i < 13; ++i){
                cust_font2.print_char(pt[i],cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = pt[i];
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = pt[i];
                cl_cnt+=2;
            }
            break;
        }
    case 2://NUM
        {
            int rw_cnt = 9,cl_cnt = 3;

            char pt[14] = "0123456789,;:";
            for(int i= 0; i < 13; ++i){
                cust_font2.print_char(pt[i],cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = pt[i];
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = pt[i];
                cl_cnt+=2;
            }
            rw_cnt+= 3;
            cl_cnt = 3;
            for(int i= 3*49-10; i < 3*49+3; ++i){
                cust_font2.print_char(i,cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = i;
                cl_cnt+=2;
            }
            rw_cnt+= 4;
            cl_cnt = 3;
            for(int i= 3*49+3; i < 3*49+16; ++i){
                cust_font2.print_char(i,cl_cnt*8,rw_cnt*8,true);
                chars[std::pair<int,int>(cl_cnt,rw_cnt)] = i;
                chars[std::pair<int,int>(cl_cnt,rw_cnt+1)] = i;
                cl_cnt+=2;
            }
            break;
        }
    }
}