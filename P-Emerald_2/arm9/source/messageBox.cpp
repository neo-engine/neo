/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : messageBox.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality message boxes

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

#include "pokemon.h"
#include "messageBox.h"
#include "sprite.h"
#include "screenLoader.h"
#include "item.h"
#include "fs.h"
#include <cmath>
#include <vector>

#include "print.h"

#include "defines.h"

extern ConsoleFont cfont;
extern FONT::font cust_font;
extern FONT::font cust_font2;

bool back_, save_, main_;
SpriteEntry * back, *save;
u8 TEXTSPEED = 35;

void init( ) {
    for( u8 i = 0; i < 4; ++i )
        Oam->oamBuffer[ 31 + 2 * i ].isHidden = true;
    for( u8 i = 9; i <= 12; ++i ) {
        Oam->oamBuffer[ i ].isHidden = true;
        swiWaitForVBlank( );
    }

    updateOAMSub( Oam );

    cust_font.setColor( 0, 0 );
    cust_font.setColor( BLACK_IDX, 1 );
    cust_font.setColor( GRAY_IDX, 2 );
    cust_font2.setColor( 0, 0 );
    cust_font2.setColor( GRAY_IDX, 1 );
    cust_font2.setColor( WHITE_IDX, 2 );

    BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
    BG_PALETTE_SUB[ GRAY_IDX ] = STEEL;
    BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
    BG_PALETTE_SUB[ RED_IDX ] = RED;
    BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
    FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, WHITE_IDX );

    updateOAMSub( Oam );
}
void dinit( ) {
    drawSub( );

    for( u8 i = 0; i < 4; ++i )
        Oam->oamBuffer[ 31 + 2 * i ].isHidden = false;
}

void messageBox::clear( ) {
    FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
}
void messageBox::clearButName( ) {
    FONT::putrec( (u8)72, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
}

bool waitForTouchUp( bool p_uTime, bool p_tpar, u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 ) {
    while( 1 ) {
        swiWaitForVBlank( );
        scanKeys( );
        if( p_uTime )
            updateTime( p_tpar );
        auto touch = touchReadXY( );
        if( touch.px == 0 && touch.py == 0 )
            return true;
        if( !( touch.px >= p_targetX1 && touch.py >= p_targetY1 && touch.px <= p_targetX2 && touch.py <= p_targetY2 ) )
            return false;
    }
    return false;
}

messageBox::messageBox( ITEMS::item p_item, const u16 p_count ) {
    m_isNamed = NULL;
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    init( );
    u8 a = 0, b = 0;
    u16 c = 0;
    FS::drawItemIcon( Oam, spriteInfo, p_item.m_itemName, 4, 4, a, b, c );
    updateOAMSub( Oam );

    updateTime( s8( 1 ) );

    cust_font.setColor( 253, 3 );
    cust_font.setColor( 254, 4 );

    BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 254 ] = RGB15( 0, 0, 15 );

    char buf[ 40 ];
    sprintf( buf, "%3d %s in der", p_count, p_item.getDisplayName( ).c_str( ) );
    cust_font.printMBStringD( buf, 32, 8, true, true, 1 );
    cust_font.printChar( 489 - 21 + p_item.getItemType( ), 32, 24, true );
    sprintf( buf, "%s-Tasche verstaut.`", bagnames[ p_item.m_itemType ].c_str( ) );
    FONT::ASpriteOamIndex = 8;
    cust_font.printMBStringD( buf, 46, 24, true, true, 1 );

    SAV->m_bag.addItem( p_item.m_itemType, p_item.getItemId( ), p_count );

    initMainSprites( Oam, spriteInfo );
    setMainSpriteVisibility( main_ );
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, false );
    dinit( );
    updateOAMSub( Oam );
}

messageBox::messageBox( const char* p_text, bool p_time, bool p_remsprites ) {
    m_isNamed = NULL;
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );

    init( );

    if( p_time ) updateTime( s8( 1 ) );

    FONT::ASpriteOamIndex = 8;
    std::string text( p_text );
    cust_font.printMBStringD( ( text + '`' ).c_str( ), 8, 8, true, p_time, 1 );

    setSpriteVisibility( back, back_ );
    setSpriteVisibility( save, save_ );
    setMainSpriteVisibility( main_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    dinit( );
    updateOAMSub( Oam );
    swiWaitForVBlank( );
}
messageBox::messageBox( const wchar_t* p_text, bool p_time, bool p_remsprites ) {
    m_isNamed = NULL;
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );

    init( );

    if( p_time ) updateTime( );

    FONT::ASpriteOamIndex = 8;
    std::wstring text( p_text );
    cust_font.printMBStringD( ( text + L'`' ).c_str( ), 8, 8, true, p_time );

    setSpriteVisibility( back, back_ );
    setSpriteVisibility( save, save_ );
    setMainSpriteVisibility( main_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    for( int i = 9; i <= 12; ++i ) {
        Oam->oamBuffer[ i ].isHidden = p_remsprites;
        swiWaitForVBlank( );
    }
    dinit( );
    updateOAMSub( Oam );
    swiWaitForVBlank( );
}
messageBox::messageBox( const char* p_text, const char* p_name, bool p_time, bool p_a, bool p_remsprites, sprite_type p_sprt, u16 p_sprind ) {
    m_isNamed = p_name;
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    if( p_sprt != no_sprite ) {
        u8 a = 0, b = 0;
        u16 c = 0;
        if( p_sprt == sprite_pkmn ) {
            FS::loadPKMNSpriteTop( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", p_sprind, (u16)-16, 0, a, b, c, true );
        }
        if( p_sprt == sprite_trainer ) {
            FS::loadPKMNSpriteTop( Oam, spriteInfo, "nitro:/PICS/SPRITES/TRAINER/", p_sprind, (u16)-16, 0, a, b, c, true );
        }
    }

    init( );

    if( p_time ) updateTime( );
    if( p_name )
        cust_font.printString( p_name, 8, 8, true );
    if( p_a ) {
        FONT::ASpriteOamIndex = 8;
        std::string text( p_text );
        cust_font.printMBStringD( ( text + '`' ).c_str( ), 64 * ( !!p_name ) + 8, 8, true, p_time );
    } else
        cust_font.printStringD( p_text, ( 64 * !!m_isNamed ) + 8, 8, true );

    if( !p_remsprites )
        return;
    if( p_sprt != no_sprite )
        initMainSprites( Oam, spriteInfo );
    setSpriteVisibility( back, back_ );
    setSpriteVisibility( save, save_ );
    setMainSpriteVisibility( main_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    for( int i = 9; i <= 12; ++i ) {
        Oam->oamBuffer[ i ].isHidden = p_remsprites;
        swiWaitForVBlank( );
    }
    dinit( );
    updateOAMSub( Oam );
    swiWaitForVBlank( );
}
messageBox::messageBox( const wchar_t* p_text, const wchar_t* p_name, bool p_time, bool p_a, bool p_remsprites, sprite_type p_sprt, u16 p_sprind ) {
    m_isNamed = NULL;
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    if( p_sprt != no_sprite ) {
        u8 a = 0, b = 0;
        u16 c = 0;
        if( p_sprt == sprite_pkmn ) {
            FS::loadPKMNSpriteTop( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", p_sprind, (u16)-16, 0, a, b, c, true );
        }
        if( p_sprt == sprite_trainer ) {
            FS::loadPKMNSpriteTop( Oam, spriteInfo, "nitro:/PICS/SPRITES/TRAINER/", p_sprind, (u16)-16, 0, a, b, c, true );
        }
    }
    updateOAMSub( Oam );

    init( );

    if( p_time ) updateTime( );
    if( p_name )
        cust_font.printString( p_name, 8, 8, true );
    if( p_a ) {
        FONT::ASpriteOamIndex = 8;
        std::wstring text( p_text );
        cust_font.printMBStringD( ( text + L'`' ).c_str( ), 64 * ( !!p_name ) + 8, 8, true, p_time );
    } else
        cust_font.printStringD( p_text, ( 64 * !!m_isNamed ) + 8, 8, true );

    if( !p_remsprites )
        return;
    if( p_sprt != no_sprite )
        initMainSprites( Oam, spriteInfo );
    setSpriteVisibility( back, back_ );
    setSpriteVisibility( save, save_ );
    setMainSpriteVisibility( main_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    for( int i = 9; i <= 12; ++i ) {
        Oam->oamBuffer[ i ].isHidden = p_remsprites;
        swiWaitForVBlank( );
    }
    dinit( );
    updateOAMSub( Oam );
    swiWaitForVBlank( );
}

void messageBox::put( const char* p_text, bool p_a, bool p_time ) {
    init( );

    if( p_time ) updateTime( );
    if( m_isNamed )
        cust_font.printString( m_isNamed, 8, 8, true );
    if( p_a ) {
        FONT::ASpriteOamIndex = 8;
        std::string text( p_text );
        cust_font.printMBStringD( ( text + '`' ).c_str( ), ( 64 * !!m_isNamed ) + 8, 8, true, p_time );
    } else
        cust_font.printStringD( p_text, ( 64 * !!m_isNamed ) + 8, 8, true );

    swiWaitForVBlank( );
}

void drawChoiceBox( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, u8 p_borderWidth, u8 p_colorIdx, bool p_pressed ) {
    if( !p_pressed ) {
        FONT::putrec( p_x1 + 1, p_y1 + 1, p_x2, p_y2,
                      true, false, BLACK_IDX );
        FONT::putrec( p_x1, p_y1, p_x2 - 2, p_y2 - 1,
                      true, false, p_colorIdx );
        FONT::putrec( p_x1 + 1 + p_borderWidth, p_y1 + p_borderWidth - 1, p_x2 - p_borderWidth, p_y2 - p_borderWidth + 4,
                      true, false, BLACK_IDX );
        FONT::putrec( p_x1 + p_borderWidth, p_y1 + p_borderWidth - 2, p_x2 - p_borderWidth - 2, p_y2 - p_borderWidth + 3,
                      true, false, WHITE_IDX );
    } else {
        FONT::putrec( p_x1, p_y1, p_x2 - 1, p_y2 - 1,
                      true, false, BLACK_IDX );
        FONT::putrec( p_x1 + 2, p_y1 + 1, p_x2, p_y2,
                      true, false, p_colorIdx );

        FONT::putrec( p_x1 + p_borderWidth, p_y1 + p_borderWidth - 2, p_x2 - p_borderWidth - 1, p_y2 - p_borderWidth + 3,
                      true, false, BLACK_IDX );

        FONT::putrec( p_x1 + 2 + p_borderWidth, p_y1 + p_borderWidth - 1, p_x2 - p_borderWidth, p_y2 - p_borderWidth + 4,
                      true, false, WHITE_IDX );
    }
}

void yesNoBox::draw( u8 p_pressedIdx ) {
    drawChoiceBox( 28, 102, 122, 134, 6, RED_IDX, p_pressedIdx == 0 );
    cust_font.printString( "Ja", 65 + 2 * ( p_pressedIdx == 0 ),
                           110 + ( p_pressedIdx == 0 ), true );
    drawChoiceBox( 134, 102, 228, 134, 6, BLUE_IDX, p_pressedIdx == 1 );
    cust_font.printString( "Nein", 165 + 2 * ( p_pressedIdx == 1 ),
                           110 + ( p_pressedIdx == 1 ), true );
}

yesNoBox::yesNoBox( ) {
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    Oam->oamBuffer[ 8 ].isHidden = true;
    init( );
    updateOAMSub( Oam );

    consoleSetWindow( &Bottom, 1, 1, 30, MAXLINES );
    consoleSelect( &Bottom );
    _isNamed = false;
}
yesNoBox::yesNoBox( const char* p_name ) {
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    Oam->oamBuffer[ 8 ].isHidden = true;

    init( );

    consoleSetWindow( &Bottom, 1, 1, 8, MAXLINES - 1 );
    consoleSelect( &Bottom );
    cust_font.printString( p_name, 8, 8, true );
    consoleSetWindow( &Bottom, 9, 1, 22, MAXLINES );

    swiWaitForVBlank( );
    _isNamed = true;
}
yesNoBox::yesNoBox( messageBox p_box ) {
    back = &Oam->oamBuffer[ 0 ];
    save = &Oam->oamBuffer[ 1 ];
    back_ = back->isHidden;
    save_ = save->isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( back, true );
    setSpriteVisibility( save, true );
    setMainSpriteVisibility( true );
    Oam->oamBuffer[ 8 ].isHidden = true;
    init( );
    updateOAMSub( Oam );
    _isNamed = p_box.m_isNamed;
}

bool yesNoBox::getResult( const char* p_text = 0, bool p_time ) {

    updateOAMSub( Oam );
    consoleSelect( &Bottom );
    if( p_text ) {
        if( _isNamed )
            cust_font.printStringD( p_text, 72, 8, true );
        else
            cust_font.printStringD( p_text, 8, 8, true );
    }
    draw( 2 );
    updateOAMSub( Oam );
    bool result;
    while( 42 ) {
        swiWaitForVBlank( );
        updateOAMSub( Oam );
        if( p_time )
            updateTime( );
        touchPosition t;
        touchRead( &t );
        if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
            draw( 0 );
            if( !waitForTouchUp( p_time, false, 28, 102, 122, 134 ) ) {
                draw( 2 );
                continue;
            }
            result = true;
            break;
        } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
            draw( 1 );
            if( !waitForTouchUp( p_time, false, 134, 102, 228, 134 ) ) {
                draw( 2 );
                continue;
            }
            result = false;
            break;
        }
    }
    setSpriteVisibility( back, back_ );
    setSpriteVisibility( save, save_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    for( u8 i = 9; i <= 12; ++i ) {
        Oam->oamBuffer[ i ].isHidden = true;
    }
    updateOAMSub( Oam );
    consoleSetWindow( &Bottom, 1, 1, 30, 24 );
    consoleClear( );
    dinit( );
    return result;
}
bool yesNoBox::getResult( const wchar_t* p_text = 0, bool p_time ) {

    updateOAMSub( Oam );
    consoleSelect( &Bottom );
    if( p_text ) {
        if( _isNamed )
            cust_font.printStringD( p_text, 72, 8, true );
        else
            cust_font.printStringD( p_text, 8, 8, true );
    }
    draw( 2 );
    updateOAMSub( Oam );
    bool result;
    while( 42 ) {
        swiWaitForVBlank( );
        updateOAMSub( Oam );
        if( p_time )
            updateTime( );
        touchPosition t;
        touchRead( &t );
        if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
            draw( 0 );
            if( !waitForTouchUp( p_time, false, 28, 102, 122, 134 ) ) {
                draw( 2 );
                continue;
            }
            result = true;
            break;
        } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
            draw( 1 );
            if( !waitForTouchUp( p_time, false, 134, 102, 228, 134 ) ) {
                draw( 2 );
                continue;
            }
            result = false;
            break;
        }
    }
    setSpriteVisibility( back, back_ );
    setSpriteVisibility( save, save_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    for( u8 i = 9; i <= 12; ++i ) {
        Oam->oamBuffer[ i ].isHidden = true;
    }
    updateOAMSub( Oam );
    consoleSetWindow( &Bottom, 1, 1, 30, 24 );
    consoleClear( );
    dinit( );
    return result;
}

#define NEW_PAGE 9
void choiceBox::draw( u8 p_pressedIdx ) {
    if( p_pressedIdx == NEW_PAGE ) {
        drawSub( );
        init( );
        if( _text ) {
            if( _name ) {
                cust_font.printString( _name, 8, 8, true );
                cust_font.printString( _text, 72, 8, true );
            } else
                cust_font.printString( _text, 8, 8, true );
        }
    }
    u8 startIdx = ( ( !_big ) * 3 + 3 ) * _acPage;
    u8 endIdx = std::min( _num, u8( ( ( !_big ) * 3 + 3 ) * ( _acPage + 1 ) ) );

    BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;

    if( _big ) {
        for( u8 i = startIdx; i < endIdx; ++i ) {
            u8 acPos = i - startIdx;
            drawChoiceBox( 32, 68 + acPos * 35, 192 + 32, 68 + 32 + acPos * 35, 6, COLOR_IDX, acPos == p_pressedIdx );
            cust_font.printString( _choices[ i ], 40 + 2 * ( p_pressedIdx == acPos ),
                                   78 + acPos * 35 + ( p_pressedIdx == acPos ), true );
            swiWaitForVBlank( );
        }
    } else {
        for( u8 i = startIdx; i < endIdx; ++i ) {
            u8 acPos = i - startIdx;
            drawChoiceBox( ( ( acPos % 2 ) ? 129 : 19 ), 68 + ( acPos / 2 ) * 35,
                           106 + ( ( acPos % 2 ) ? 129 : 19 ), 32 + 68 + ( acPos / 2 ) * 35, 6, COLOR_IDX, acPos == p_pressedIdx );
            cust_font.printString( _choices[ i ], ( ( acPos % 2 ) ? 129 : 19 ) + 8 + 2 * ( p_pressedIdx == acPos ),
                                   78 + ( acPos / 2 ) * 35 + ( p_pressedIdx == acPos ), true );
            swiWaitForVBlank( );
        }
    }
}
choiceBox::choiceBox( int p_num, const char** p_choices, const char* p_name = 0, bool p_big = false ) {
    _num = p_num;
    _choices = p_choices;
    _big = p_big || ( p_num <= 3 );
    _acPage = 0;

    back_ = Oam->oamBuffer[ 0 ].isHidden;
    save_ = Oam->oamBuffer[ 1 ].isHidden;
    main_ = ( Oam->oamBuffer[ BAG_ID ] ).isHidden;
    setSpriteVisibility( &( Oam->oamBuffer[ 0 ] ), true );
    setSpriteVisibility( &( Oam->oamBuffer[ 1 ] ), true );
    setMainSpriteVisibility( true );
    Oam->oamBuffer[ 8 ].isHidden = true;
    init( );

    for( u8 i = 13; i <= 29; ++i ) {
        Oam->oamBuffer[ i ].isHidden = true;
    }
    updateOAMSub( Oam );

    _name = p_name;

    swiWaitForVBlank( );
}

int fwdPos[ 2 ][ 2 ] = { { SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12 }, { SCREEN_WIDTH - 11, SCREEN_HEIGHT - 31 } },
bwdPos[ 2 ][ 2 ] = { { SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12 }, { SCREEN_WIDTH - 31, SCREEN_HEIGHT - 11 } };

choiceBox::~choiceBox( ) {
    dinit( );
    setMainSpriteVisibility( main_ );
    Oam->oamBuffer[ 8 ].isHidden = true;
    for( u8 i = 9; i <= 29; ++i )
        Oam->oamBuffer[ i ].isHidden = true;

    setSpriteVisibility( &( Oam->oamBuffer[ 0 ] ), back_ );
    setSpriteVisibility( &( Oam->oamBuffer[ 1 ] ), save_ );
    updateOAMSub( Oam );
    swiWaitForVBlank( );
}

int choiceBox::getResult( const char* p_text, bool p_time, bool p_backButton ) {
    consoleSelect( &Bottom );
    _text = p_text;
    draw( NEW_PAGE );

    int result = -1;

    if( p_backButton ) {
        ( Oam->oamBuffer[ BACK_ID ] ).isHidden = false;
        ( Oam->oamBuffer[ BACK_ID ] ).x = fwdPos[ 0 ][ 0 ] - 12;
        ( Oam->oamBuffer[ BACK_ID ] ).y = fwdPos[ 0 ][ 1 ] - 12;
        updateOAMSub( Oam );
    }

    if( _num < 1 )
        return -1;
    else if( _num <= 3 || ( _num > 3 && _big ) ) {
        _acPage = 0;
        if( _num > 3 && _big ) {
            ( Oam->oamBuffer[ 13 ] ).isHidden = false;
            ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
            ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
        }
        while( 42 ) {
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            if( p_time )
                updateTime( s8( 1 ) );
            touchPosition t;
            touchRead( &t );

            for( u8 i = 0; i < 3; ++i )
                if( ( i + 3 * _acPage ) < _num && t.px >= 32 && t.py >= 68 + 35 * i && t.px <= 224 && t.py <= 100 + 35 * i ) {
                    draw( i );
                    if( !waitForTouchUp( p_time, true, 32, 68 + 35 * i, 224, 100 + 35 * i ) ) {
                        draw( 8 );
                        break;
                    }
                    result = i + 3 * _acPage;
                    goto END;
                }
            if( p_backButton && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] - 12 ) + sq( t.py - fwdPos[ 0 ][ 1 ] - 12 ) ) < 17 ) { //Back pressed
                result = -1;
                goto END;
            }
            if( _num > 3 && _big ) {
                if( !p_backButton && ( ( _num - 1 ) / 3 && _acPage == 0 && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] ) + sq( t.py - fwdPos[ 0 ][ 1 ] ) ) < 17 )
                    || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - fwdPos[ 1 ][ 0 ] ) + sq( t.py - fwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                    waitForTouchUp( p_time, true );
                    if( ( ++_acPage ) >= ( ( _num - 1 ) / 3 ) ) {
                        ( Oam->oamBuffer[ 13 ] ).isHidden = true;
                        ( Oam->oamBuffer[ 14 ] ).isHidden = !_acPage;
                        ( Oam->oamBuffer[ 14 ] ).x = bwdPos[ p_backButton ][ 0 ] - 16;
                        ( Oam->oamBuffer[ 14 ] ).y = bwdPos[ p_backButton ][ 1 ] - 16;
                        //if( _acPage == ( ( _num - 1 ) / 3 ) )
                        //    draw( NEW_PAGE );
                        _acPage = ( ( _num - 1 ) / 3 );
                    } else {
                        ( Oam->oamBuffer[ 14 ] ).isHidden = false;
                        ( Oam->oamBuffer[ 13 ] ).isHidden = false;
                        ( Oam->oamBuffer[ 14 ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ 14 ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                        ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                        draw( NEW_PAGE );
                    }
                    updateOAMSub( Oam );
                } else if( !p_backButton && ( _acPage && _acPage == ( _num - 1 ) / 3 && sqrt( sq( t.px - bwdPos[ 0 ][ 0 ] ) + sq( t.py - bwdPos[ 0 ][ 1 ] ) ) < 17 )
                           || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - bwdPos[ 1 ][ 0 ] ) + sq( t.py - bwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                    waitForTouchUp( p_time, true );
                    if( ( --_acPage ) <= 0 ) {
                        ( Oam->oamBuffer[ 14 ] ).isHidden = true;
                        ( Oam->oamBuffer[ 13 ] ).isHidden = !( _acPage < ( ( _num - 1 ) / 3 ) );
                        ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                        ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
                        if( _acPage == 0 )
                            draw( NEW_PAGE );
                        _acPage = 0;
                    } else {
                        ( Oam->oamBuffer[ 13 ] ).isHidden = false;
                        ( Oam->oamBuffer[ 14 ] ).isHidden = false;
                        ( Oam->oamBuffer[ 14 ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ 14 ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                        ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                        draw( NEW_PAGE );
                    }
                    updateOAMSub( Oam );
                }
            }
        }
    } else {
        _acPage = 0;
        if( _num > 6 ) {
            ( Oam->oamBuffer[ 13 ] ).isHidden = false;
            ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
            ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
        }
        updateOAMSub( Oam );
        while( 42 ) {
            swiWaitForVBlank( );
            updateOAMSub( Oam );
            if( p_time )
                updateTime( s8( 1 ) );
            touchPosition t;
            touchRead( &t );

            for( u8 i = 0; i < std::min( 6, _num - 6 * _acPage ); ++i ) {
                if( t.px >= ( ( i % 2 ) ? 129 : 19 ) && t.py >= 68 + ( i / 2 ) * 35
                    && t.px <= 106 + ( ( i % 2 ) ? 129 : 19 ) && t.py <= 32 + 68 + ( i / 2 ) * 35 ) {
                    draw( i );

                    if( !waitForTouchUp( p_time, true, ( ( i % 2 ) ? 129 : 19 ),
                        68 + ( i / 2 ) * 35, 106 + ( ( i % 2 ) ? 129 : 19 ), 32 + 68 + ( i / 2 ) * 35 ) ) {
                        draw( 8 );
                        break;
                    }
                    result = i + 6 * _acPage;
                    goto END;
                }
            }

            if( p_backButton && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] - 12 ) + sq( t.py - fwdPos[ 0 ][ 1 ] - 12 ) ) < 17 ) { //Back pressed
                result = -1;
                goto END;
            } else if( !p_backButton && ( ( _num - 1 ) / 6 && _acPage == 0 && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] ) + sq( t.py - fwdPos[ 0 ][ 1 ] ) ) < 17 )
                       || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - fwdPos[ 1 ][ 0 ] ) + sq( t.py - fwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                waitForTouchUp( p_time, true );
                if( ( ++_acPage ) >= ( ( _num - 1 ) / 6 ) ) {
                    ( Oam->oamBuffer[ 13 ] ).isHidden = true;
                    ( Oam->oamBuffer[ 14 ] ).isHidden = !_acPage;
                    ( Oam->oamBuffer[ 14 ] ).x = bwdPos[ p_backButton ][ 0 ] - 16;
                    ( Oam->oamBuffer[ 14 ] ).y = bwdPos[ p_backButton ][ 1 ] - 16;
                    /*if( _acPage == ( _num / 6 ) )
                        draw( NEW_PAGE );*/
                    _acPage = ( _num / 6 );
                } else {
                    ( Oam->oamBuffer[ 14 ] ).isHidden = false;
                    ( Oam->oamBuffer[ 13 ] ).isHidden = false;
                    ( Oam->oamBuffer[ 14 ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                    ( Oam->oamBuffer[ 14 ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                    ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                    ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                    draw( NEW_PAGE );
                }
            } else if( !p_backButton && ( _acPage && _acPage == ( _num - 1 ) / 6 && sqrt( sq( t.px - bwdPos[ 0 ][ 0 ] ) + sq( t.py - bwdPos[ 0 ][ 1 ] ) ) < 17 )
                       || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - bwdPos[ 1 ][ 0 ] ) + sq( t.py - bwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {
                waitForTouchUp( p_time, true );
                if( ( --_acPage ) <= 0 ) {
                    ( Oam->oamBuffer[ 14 ] ).isHidden = true;
                    ( Oam->oamBuffer[ 13 ] ).isHidden = !( _acPage < ( ( _num - 1 ) / 6 ) );
                    ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                    ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
                    if( _acPage == 0 )
                        draw( NEW_PAGE );
                    _acPage = 0;
                } else {
                    ( Oam->oamBuffer[ 13 ] ).isHidden = false;
                    ( Oam->oamBuffer[ 14 ] ).isHidden = false;
                    ( Oam->oamBuffer[ 14 ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                    ( Oam->oamBuffer[ 14 ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                    ( Oam->oamBuffer[ 13 ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                    ( Oam->oamBuffer[ 13 ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                    draw( NEW_PAGE );
                }
            }
        }
    }
END:
    dinit( );
    ( Oam->oamBuffer[ 14 ] ).isHidden = true;
    ( Oam->oamBuffer[ 13 ] ).isHidden = true;
    ( Oam->oamBuffer[ BACK_ID ] ).isHidden = true;
    updateOAMSub( Oam );
    return result;
}