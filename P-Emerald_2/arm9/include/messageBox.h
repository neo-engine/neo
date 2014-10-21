#include <nds.h>
#include <string>
#include <cstdio>


#ifndef __MBOX__
#define __MBOX__
extern int bg3sub;
extern PrintConsole Bottom;

extern void updateTime( s8 p_mapMode );

bool operator==( touchPosition p_r, touchPosition p_l );

#define MAXLINES 6
extern u8 TEXTSPEED;

class item;
class messageBox {
public:
    enum sprite_type {
        no_sprite = 0,
        sprite_pkmn = 1,
        sprite_trainer = 2
    };

    messageBox( ITEMS::item p_item, const int p_count );

    messageBox( const char* p_text, bool p_time = true, bool p_remsprites = true );
    messageBox( const wchar_t* p_text, bool p_time = true, bool p_remsprites = true );
    messageBox( const char* p_text, const char* p_name, bool p_time = true, bool p_a = true, bool p_remsprites = true, sprite_type p_sprt = no_sprite, u8 p_sprind = 0 );
    messageBox( const wchar_t* p_text, const wchar_t* p_name, bool p_time = true, bool p_a = true, bool p_remsprites = true, sprite_type p_sprt = no_sprite, u8 p_sprind = 0 );
    ~messageBox( ) { }
    void put( const char* p_text, bool p_a = true, bool p_time = true );
    void clear( );
    void clearButName( );

    const char* m_isNamed;
};

class yesNoBox {
public:
    yesNoBox( );
    yesNoBox( const char* p_name );
    yesNoBox( messageBox p_box );

    bool getResult( const char* p_text, bool p_time = true );
    bool getResult( const wchar_t* p_text, bool p_time = true );

    ~yesNoBox( ) {
        consoleSetWindow( &Bottom, 9, 1, 22, MAXLINES );
        consoleSelect( &Bottom );
        consoleClear( );
        consoleSetWindow( &Bottom, 1, 1, 8, MAXLINES - 1 );
        consoleClear( );
        consoleSetWindow( &Bottom, 1, 1, 30, MAXLINES );
        consoleClear( );
    }
private:
    bool _isNamed;
};


#define MAX_CHOICES_PER_PAGE 3
#define MAX_CHOICES_PER_SMALL_PAGE 6
class choiceBox {
public:
    choiceBox( int p_num, const char** p_choices, const char* p_name, bool p_big );
    ~choiceBox( );

    int getResult( const char* p_text, bool p_time );
    void kill( );
private:
    const char** _choices;
    bool _big;
    u8 _num;
    bool _name;
};

#endif