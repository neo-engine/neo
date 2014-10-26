/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : print.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for printing custom fonts

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
#include <string.h>
#include "fontData.h"
#include "print.h"
#include "messageBox.h"

namespace FONT {
    void putrec( u8 p_x1, u8 p_y1, u8 p_x2, u8 p_y2, bool p_bottom, bool p_striped, u8 p_color ) {
        for( u16 x = p_x1; x <= p_x2; ++x ) for( u16 y = p_y1; y < p_y2; ++y )
            if( p_bottom )
                ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( x + y * (u16)SCREEN_WIDTH ) / 2 ] = !p_striped ? ( ( (u8)p_color ) << 8 ) | ( (u8)p_color ) : p_color;
            else
                ( (color *)BG_BMP_RAM( 1 ) )[ ( x + y * (u16)SCREEN_WIDTH ) / 2 ] = !p_striped ? ( ( (u8)p_color ) << 8 ) | ( (u8)p_color ) : p_color;
    }
    Font::Font( u8 *p_data, u8 *p_widths, void( *p_shiftchar )( u16& val ) ) {
        _data = p_data;
        _widths = p_widths;
        _color[ 0 ] = _color[ 1 ] = _color[ 2 ]
            = _color[ 3 ] = _color[ 4 ] = RGB( 31, 31, 31 );
        _shiftchar = p_shiftchar;
    }

    Font::~Font( ) { }


    void Font::printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom ) {
        _shiftchar( p_ch );

        s16 putX, putY;
        u8 getX, getY;
        u32 offset = p_ch * FONT_WIDTH * FONT_HEIGHT;

        for( putY = p_y, getY = 0; putY < p_y + FONT_HEIGHT; ++putY, ++getY ) {
            for( putX = p_x, getX = 0; putX < p_x + _widths[ p_ch ]; putX += 2, getX += 2 ) {
                if( putX >= 0 && putX < SCREEN_WIDTH && putY >= 0 && putY < SCREEN_HEIGHT ) {
                    if( !p_bottom ) {
                        topScreenPlot( putX, putY, ( (u8)( _color[ _data[ 1 + offset + ( getX + getY * FONT_WIDTH ) ] ] ) << 8 ) |
                                       (u8)( _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ] ) );
                    } else {
                        btmScreenPlot( putX, putY, ( (u8)( _color[ _data[ 1 + offset + ( getX + getY * FONT_WIDTH ) ] ] ) << 8 ) |
                                       (u8)( _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ] ) );
                    }
                }
            }
        }
    }

    void Font::printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += FONT_HEIGHT;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16)p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }

    void drawContinue( Font p_font, u8 p_x, u8 p_y ) {
        p_font.printChar( 172, p_x, p_y, true );
    }
    void hideContinue( u8 p_x, u8 p_y ) {
        putrec( p_x, p_y, p_x + 10, p_y + 10, true, false, (u8)250 );
    }

    void Font::printMBString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == L'\n' ) {
                putY += FONT_HEIGHT;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == L'`' ) {
                u8 c = 0;
                bool on = false;
                while( 1 ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 246, 54 );
                        else
                            drawContinue( *this, 246, 54 );
                    }
                    updateTime( 0 );
                    if( keysUp( ) & KEY_A )
                        break;
                    auto t = touchReadXY( );
                    if( t.px == 0 && t.py == 0 )
                        break;
                }
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16)p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }
    void Font::printString( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == L'\n' ) {
                putY += FONT_HEIGHT;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16)p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }
    void Font::printStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += 16;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16)p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0; i < 80 / TEXTSPEED; ++i )
                swiWaitForVBlank( );
            current_char++;
        }
    }
    void Font::printStringD( const wchar_t *p_string, s16 p_x, s16 p_y, bool p_bottom ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += FONT_HEIGHT;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom );

            u16 c = (u16)p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0; i < 80 / TEXTSPEED; ++i )
                swiWaitForVBlank( );
            current_char++;
        }
    }

    void Font::printStringCenter( const char *p_string, bool p_bottom ) {
        s16 x = SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2;
        s16 y = SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2;

        printString( p_string, x, y, p_bottom );
    }
    void Font::printStringCenterD( const char *p_string, bool p_bottom ) {
        s16 x = ( SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2 );
        s16 y = ( SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2 );

        printStringD( p_string, x, y, p_bottom );
    }
    void Font::printStringCenter( const wchar_t *p_string, bool p_bottom ) {
        s16 x = SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2;
        s16 y = SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2;

        printString( p_string, x, y, p_bottom );
    }
    void Font::printStringCenterD( const wchar_t *p_string, bool p_bottom ) {
        s16 x = ( SCREEN_WIDTH / 2 - stringWidth( p_string ) / 2 );
        s16 y = ( SCREEN_HEIGHT / 2 - FONT_HEIGHT / 2 );

        printStringD( p_string, x, y, p_bottom );
    }

    void Font::printNumber( s32 p_num, s16 p_x, s16 p_y, bool p_bottom ) {
        char numstring[ 10 ] = "";
        u32 number = p_num, quotient = 1, remainder = 0;
        char remainder_str[ 3 ] = "";
        u32 current_char = 0, current_char2 = 0;
        static char string[ 100 ];

        if( number == 0 ) {
            strcpy( string, "0" );
        } else {
            while( quotient != 0 ) {
                remainder = number % 10;
                quotient = number / 10;
                remainder_str[ 0 ] = remainder + 48;
                remainder_str[ 1 ] = '\0';
                strcat( numstring, remainder_str );
                number = quotient;
            }

            current_char = strlen( numstring ) - 1;
            while( current_char != 0 ) {
                string[ current_char2 ] = numstring[ current_char ];
                current_char--;
                current_char2++;
            }
            string[ current_char2 ] = numstring[ current_char ];
            string[ current_char2 + 1 ] = '\0';
        }

        printString( string, p_x, p_y, p_bottom );
    }


    u32 Font::stringWidth( const char *p_string ) const {
        u32 current_char = 0;
        u32 width = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' )
                break;
            width += _widths[ (u8)p_string[ current_char ] ] + 1;

            current_char++;
        }

        return width - 1;
    }
    u32 Font::stringWidth( const wchar_t *p_string ) const {
        u32 current_char = 0;
        u32 width = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' )
                break;
            width += _widths[ (u8)p_string[ current_char ] ] + 1;

            current_char++;
        }

        return width - 1;
    }
}

void topScreenDarken( ) {
    u16 i;
    color pixel;

    for( i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ ) {
        pixel = ( (color *)BG_BMP_RAM( 1 ) )[ i ];
        ( (color *)BG_BMP_RAM( 1 ) )[ i ] = ( ( pixel & 0x1F ) >> 1 ) |
            ( ( ( ( pixel >> 5 ) & 0x1F ) >> 1 ) << 5 ) |
            ( ( ( ( pixel >> 10 ) & 0x1F ) >> 1 ) << 10 ) |
            ( 1 << 15 );
    }
}
void topScreenPlot( u8 p_x, u8 p_y, color p_color ) {
    if( ( p_color >> 8 ) != 0 && ( p_color % ( 1 << 8 ) ) != 0 )
        ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color;
    else if( ( p_color >> 8 ) != 0 )
        ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] % ( 1 << 8 ) );
    else if( ( p_color % ( 1 << 8 ) ) != 0 )
        ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] << 8 );
}
void btmScreenDarken( ) {
    u16 i;
    color pixel;

    for( i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ ) {
        pixel = ( (color *)BG_BMP_RAM_SUB( 1 ) )[ i ];
        ( (color *)BG_BMP_RAM_SUB( 1 ) )[ i ] = ( ( pixel & 0x1F ) >> 1 ) |
            ( ( ( ( pixel >> 5 ) & 0x1F ) >> 1 ) << 5 ) |
            ( ( ( ( pixel >> 10 ) & 0x1F ) >> 1 ) << 10 ) |
            ( 1 << 15 );
    }
}
void btmScreenPlot( u8 p_x, u8 p_y, color p_color ) {
    if( ( p_color >> 8 ) != 0 && ( p_color % ( 1 << 8 ) ) != 0 )
        ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color;
    else if( ( p_color >> 8 ) != 0 )
        ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] % ( 1 << 8 ) );
    else if( ( p_color % ( 1 << 8 ) ) != 0 )
        ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] = p_color | ( ( (color *)BG_BMP_RAM_SUB( 1 ) )[ ( p_x + p_y * SCREEN_WIDTH ) / 2 ] << 8 );
}