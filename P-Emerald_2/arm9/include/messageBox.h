/*
Pokémon Emerald 2 Version
------------------------------

file        : messageBox.h
author      : Philip Wellnitz (RedArceus)
description : Header file. See corresponding source file for details.

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <nds.h>
#include <string>
#include <cstdio>


#ifndef __MBOX__
#define __MBOX__
extern int bg3sub;
extern PrintConsole Bottom;

extern void updateTime( s8 p_mapMode );

bool operator==( touchPosition p_r, touchPosition p_l );
bool waitForTouchUp( bool p_uTime, bool p_tpar = false, u16 p_targetX1 = 0, u16 p_targetY1 = 0, u16 p_targetX2 = 300, u16 p_targetY2 = 300 );

#define MAXLINES 6
extern u8 TEXTSPEED;

namespace ITEMS {
    class item;
}
class messageBox {
public:
    enum sprite_type {
        no_sprite = 0,
        sprite_pkmn = 1,
        sprite_trainer = 2
    };

    messageBox( ITEMS::item p_item, const u16 p_count );

    messageBox( const char* p_text, bool p_time = true, bool p_remsprites = true );
    messageBox( const wchar_t* p_text, bool p_time = true, bool p_remsprites = true );
    messageBox( const char* p_text, const char* p_name, bool p_time = true, bool p_a = true, bool p_remsprites = true, sprite_type p_sprt = no_sprite, u16 p_sprind = 0 );
    messageBox( const wchar_t* p_text, const wchar_t* p_name, bool p_time = true, bool p_a = true, bool p_remsprites = true, sprite_type p_sprt = no_sprite, u16 p_sprind = 0 );
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

    void draw( u8 p_pressedIdx );

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

    void draw( u8 p_pressedIdx );
    int getResult( const char* p_text = 0, bool p_time = true, bool p_backButton = false );
    void kill( );
private:
    const char** _choices;
    bool _big;
    u8 _num;
    const char* _name;
    const char* _text;
    u8 _acPage;
};

#endif