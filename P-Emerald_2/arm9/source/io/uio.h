#pragma once
#include <nds.h>
#include <string>

#include "font.h"
#include "sprite.h"

namespace IO {

    const u8 SCREEN_TOP = 1;
    const u8 SCREEN_BOTTOM = 0;
    const u8 FONT_WIDTH = 16;
    const u8 FONT_HEIGHT = 16;

    extern font* regularFont;
    extern font* boldFont;
    extern ConsoleFont* consoleFont;

    extern u8 ASpriteOamIndex;

    extern OAMTable *Oam;
    extern SpriteInfo spriteInfo[ SPRITE_COUNT ];

    extern OAMTable *OamTop;
    extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

    extern u8 TEXTSPEED;

    extern int bg2;
    extern int bg3;
    extern int bg2sub;
    extern int bg3sub;
    extern PrintConsole Bottom, Top;


    void updateTime( s8 p_mapMode = 0 );
    void drawSub( );
    void drawSub( u8 p_newIdx );

    struct inputTarget {
        enum inputType {
            BUTTON,
            TOUCH
        };
        inputType       m_inputType;
        KEYPAD_BITS     m_keys;
        u16             m_targetX1 = 0;
        u16             m_targetY1 = 0;
        u16             m_targetX2 = 300;
        u16             m_targetY2 = 300;

        inputTarget( KEYPAD_BITS p_keys )
            : m_inputType( BUTTON ), m_keys( p_keys ) { }
        inputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 )
            : m_inputType( TOUCH ), m_targetX1( p_targetX1 ), m_targetY1( p_targetY1 ), m_targetX2( p_targetX2 ), m_targetY2( p_targetY2 ) { }
    };

    bool waitForTouchUp( bool p_updateTime, bool p_timeParameter = false, u16 p_targetX1 = 0, u16 p_targetY1 = 0, u16 p_targetX2 = 300, u16 p_targetY2 = 300 );
    bool waitForTouchUp( bool p_updateTime, bool p_timeParameter, inputTarget p_inputTarget );

    void waitForKeysUp( bool p_updateTime, bool p_timeParameter, KEYPAD_BITS p_keys );
    void waitForKeysUp( bool p_updateTime, bool p_timeParameter, inputTarget p_inputTarget );

    bool waitForInput( bool p_updateTime, bool p_timeParameter, inputTarget p_inputTarget );

    void initTextField( );

    [deprecated( "Use printRectangle instead." )]
    void putrec( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom = true, bool p_striped = false, u8 p_color = 42 );
    void printRectangle( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom = true, bool p_striped = false, u8 p_color = 42 );

    void printChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_colorIdx, bool p_pressed );

    void printChar( font* p_font, u16 p_ch, s16 p_x, s16 p_y, bool p_bottom );
    void printString( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16 );
    void printStringCenter( font* p_font, const char *p_string, bool p_bottom );
    void printStringD( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom );
    void printStringCenterD( font* p_font, const char *p_string, bool p_bottom );
    void printNumber( font* p_font, s32 p_num, s16 p_x, s16 p_y, bool p_bottom );

    void printString( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_yDistance = 16 );
    void printMBString( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
    void printMBString( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
    void printMBStringD( font* p_font, const char *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
    void printMBStringD( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom, bool p_updateTime = true, s8 p_updateTimePar = 0 );
    void printStringCenter( font* p_font, const wchar_t *p_string, bool p_bottom );
    void printStringD( font* p_font, const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom );
    void printStringCenterD( font* p_font, const wchar_t *p_string, bool p_bottom );

    void topScreenDarken( );
    void btmScreenDarken( );
    void topScreenPlot( u8 p_x, u8 p_y, color p_color );
    void btmScreenPlot( u8 p_x, u8 p_y, color p_color );
}