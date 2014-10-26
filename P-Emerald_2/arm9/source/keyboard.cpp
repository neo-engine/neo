/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : keyboard.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for keyboard input

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */


#include <nds.h>
#include <cmath>

#include "keyboard.h"
#include "screenLoader.h"
#include "print.h"

extern FONT::font cust_font;
extern FONT::font cust_font2;
extern SpriteInfo spriteInfo[ SPRITE_COUNT ];
extern SpriteInfo spriteInfoTop[ SPRITE_COUNT ];
extern OAMTable *Oam, *OamTop;

std::wstring keyboard::getText( u16 p_length, const char* p_msg, bool p_time ) {
    drawSub( );

    for( u8 i = 0; i < 4; ++i )
        Oam->oamBuffer[ 31 + 2 * i ].isHidden = true;
    Oam->oamBuffer[ A_ID ].isHidden = false;
    Oam->oamBuffer[ BWD_ID ].isHidden = false;
    Oam->oamBuffer[ FWD_ID ].isHidden = false;
    Oam->oamBuffer[ 13 ].x = 256 - 24;
    Oam->oamBuffer[ 13 ].y = 196 - 28 - 22;
    Oam->oamBuffer[ 14 ].x = 256 - 28 - 18;
    Oam->oamBuffer[ 14 ].y = 196 - 28;
    Oam->oamBuffer[ A_ID ].x = 256 - 24;
    Oam->oamBuffer[ A_ID ].y = 196 - 28;

    updateOAMSub( Oam );

    cust_font.setColor( 0, 0 );
    cust_font.setColor( 251, 1 );
    cust_font.setColor( 252, 2 );
    cust_font2.setColor( 0, 0 );
    cust_font2.setColor( 253, 1 );
    cust_font2.setColor( 254, 2 );

    BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
    BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
    FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );

    if( p_msg != 0 )
        cust_font.printString( p_msg, 8, 4, true );

    draw( );
    std::wstring out;
    consoleSelect( &Bottom );
    touchPosition t;
    u8 i = 0;
    while( 1 ) {
        scanKeys( );
        touchRead( &t );
        if( p_time )
            updateTime( );
        if( i < p_length && _chars.count( std::pair<int, int>( t.px / 8, t.py / 8 ) ) ) {
            while( 1 ) {
                scanKeys( );
                if( p_time )
                    updateTime( );
                t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            ++i;
            out += _chars[ std::pair<int, int>( t.px / 8, t.py / 8 ) ];

            FONT::putrec( u8( 8 * ( 13 - i / 3 ) ), u8( 40 ), u8( 8 * ( 13 - i / 3 + p_length + 1 ) ), (u8)56, true, false, (u8)250 );
            cust_font.printString( out.c_str( ), 8 * ( 13 - i / 3 ), 40, true );
            swiWaitForVBlank( );
        } else if( ( sqrt( sq( 248 - t.px ) + sq( 162 - t.py ) ) <= 16 ) ) {
            while( 1 ) {
                scanKeys( );
                if( p_time )
                    updateTime( );
                t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            _ind = ( _ind + 1 ) % MAXKEYBOARDS;
            undraw( );
            if( p_msg != 0 ) {
                FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
                cust_font.printString( p_msg, 8, 4, true );
            }
            draw( );
            if( p_time )
                updateTime( );

            FONT::putrec( u8( 8 * ( 13 - i / 3 ) ), u8( 40 ), u8( 8 * ( 13 - i / 3 + p_length + 1 ) ), u8( 56 ), true, false, u8( 250 ) );
            cust_font.printString( out.c_str( ), 8 * ( 13 - i / 3 ), 40, true );
        } else if( sqrt( sq( 248 - t.px ) + sq( 184 - t.py ) ) <= 16 ) {
            while( 1 ) {
                scanKeys( );
                if( p_time )
                    updateTime( );
                t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            undraw( );
            initMainSprites( Oam, spriteInfo );
            return out;
        } else if( ( sqrt( sq( 220 - t.px ) + sq( 184 - t.py ) ) <= 16 ) ) {
            while( 1 ) {
                scanKeys( );
                if( p_time )
                    updateTime( );
                t = touchReadXY( );
                if( t.px == 0 && t.py == 0 )
                    break;
            }
            out.erase( ( out.end( ) - 1 ) );
            FONT::putrec( u8( 8 * ( 13 - i / 3 ) ), (u8)40, u8( 8 * ( 13 - i / 3 + p_length + 1 ) ), (u8)56, true, false, (u8)250 );
            --i;
            cust_font.printString( out.c_str( ), 8 * ( 13 - i / 3 ), 40, true );
        }
        swiWaitForVBlank( );
    }
}

void keyboard::undraw( ) {
    drawSub( );
    BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
    BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
    FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
}

void keyboard::draw( ) {
    consoleSelect( &Bottom );
    consoleSetWindow( &Bottom, 4, 11, 26, 13 );

    switch( _ind ) {
        case 0://UPPER
        {
            u8 cnt = 0;
            u8 rw_cnt = 9, cl_cnt = 3;
            _chars.clear( );
            for( char i = 'A'; i <= 'Z'; i++ ) {
                if( cnt++ == 13 ) {
                    cnt = 0;
                    cl_cnt = 3;
                    rw_cnt += 3;
                }
                cust_font2.printChar( i, cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = i;
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = i;
                cl_cnt += 2;
            }
            rw_cnt += 4;
            cl_cnt = 3;
            char pt[ 14 ] = "ÄÖÜ .!?()[]{}";
            for( u8 i = 0; i < 13; ++i ) {
                cust_font2.printChar( pt[ i ], cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = pt[ i ];
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = pt[ i ];
                cl_cnt += 2;
            }
            break;
        }
        case 1://LOWER
        {
            u8 cnt = 0;
            u8 rw_cnt = 9, cl_cnt = 3;
            for( char i = 'a'; i <= 'z'; i++ ) {
                if( cnt++ == 13 ) {
                    cnt = 0;
                    cl_cnt = 3;
                    rw_cnt += 3;
                }
                cust_font2.printChar( i, cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = i;
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = i;
                cl_cnt += 2;
            }
            rw_cnt += 4;
            cl_cnt = 3;
            char pt[ 14 ] = "äöüßé §$%&/\\#";
            for( u8 i = 0; i < 13; ++i ) {
                cust_font2.printChar( pt[ i ], cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = pt[ i ];
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = pt[ i ];
                cl_cnt += 2;
            }
            break;
        }
        case 2://NUM
        {
            u8 rw_cnt = 9, cl_cnt = 3;

            char pt[ 14 ] = "0123456789,;:";
            for( u8 i = 0; i < 13; ++i ) {
                cust_font2.printChar( pt[ i ], cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = pt[ i ];
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = pt[ i ];
                cl_cnt += 2;
            }
            rw_cnt += 3;
            cl_cnt = 3;
            for( u8 i = 3 * 49 - 10; i < 3 * 49 + 3; ++i ) {
                cust_font2.printChar( i, cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = i;
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = i;
                cl_cnt += 2;
            }
            rw_cnt += 4;
            cl_cnt = 3;
            for( u8 i = 3 * 49 + 3; i < 3 * 49 + 16; ++i ) {
                cust_font2.printChar( i, cl_cnt * 8, rw_cnt * 8, true );
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt ) ] = i;
                _chars[ std::pair<int, int>( cl_cnt, rw_cnt + 1 ) ] = i;
                cl_cnt += 2;
            }
            break;
        }
    }
}