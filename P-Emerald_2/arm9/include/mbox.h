#include <nds.h>
#include <string>
#include <cstdio>


#ifndef __MBOX__
#define __MBOX__
extern int bg3sub;
extern PrintConsole Bottom;

extern void updateTime(bool mapMode);

bool operator==(touchPosition R,touchPosition L);

#define MAXLINES 6
extern int TEXTSPEED;

class item;
class mbox
{
public:
    enum sprite_type{
        no_sprite = 0,
        sprite_pkmn = 1,
        sprite_trainer = 2
    };

    mbox(item item, const int count);

    mbox(const char* text,bool time = true,bool remsprites = true);
    mbox(const wchar_t* text,bool time = true,bool remsprites = true);
    mbox(const char* text,const char* name,bool time = true,bool a = true,bool remsprites = true,sprite_type sprt = no_sprite,int sprind = 0);
    mbox(const wchar_t* text,const wchar_t* name,bool time = true,bool a = true,bool remsprites = true,sprite_type sprt = no_sprite,int sprind = 0);
    ~mbox()
    {
    }
    void put(const char* text,bool a = true,bool time = true);
    void clear();
    void clearButName();

    const char* isNamed;
};

class ynbox
{
public:
    ynbox();
    ynbox(const char* name);
    ynbox(mbox Box);

    bool getResult(const char* Text,bool time = true);
    bool getResult(const wchar_t* Text,bool time = true);

    ~ynbox()
    {
        consoleSetWindow(&Bottom, 9,1,22,MAXLINES);	
        consoleSelect(&Bottom);
        consoleClear();
        consoleSetWindow(&Bottom,1,1,8,MAXLINES-1);
        consoleClear();
        consoleSetWindow(&Bottom, 1,1,30,MAXLINES);
        consoleClear();
    }
private:
    bool isNamed;
};


#define MAX_CHOICES_PER_PAGE 3
#define MAX_CHOICES_PER_SMALL_PAGE 6
class cbox
{
public:
    cbox(int num,const char** choices,const char* name,bool big);
    ~cbox();

    int getResult(const char* Text,bool time);
    void kill();
private:
    const char** choices_;
    bool big;
    int num;
    bool name;
};

#endif