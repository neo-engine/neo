/*
Pokémon neo
------------------------------

file        : font.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <nds.h>
#include <nds/ndstypes.h>

#include "defines.h"
#include "font.h"
#include "saveGame.h"
#include "uio.h"

namespace IO {
    font::font( u8 *p_data, u8 *p_widths, void ( *p_shiftchar )( u16 &val ) ) {
        _data       = p_data;
        _widths     = p_widths;
        _color[ 0 ] = _color[ 1 ] = _color[ 2 ] = _color[ 3 ] = _color[ 4 ] = WHITE;
        _shiftchar                                                          = p_shiftchar;
    }

    void font::printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom, u8 p_layer ) {
        _shiftchar( p_ch );

        s16 putX, putY;
        u8  getX, getY;
        u32 offset = p_ch * FONT_WIDTH * FONT_HEIGHT;

        for( putY = p_y, getY = 0; putY < p_y + FONT_HEIGHT; ++putY, ++getY ) {
            for( putX = p_x, getX = 0; putX < p_x + _widths[ p_ch ]; putX++, getX++ ) {
                if( putX >= 0 && putX < SCREEN_WIDTH && putY >= 0 && putY < SCREEN_HEIGHT ) {
                    u8 clr = _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ];
                    if( clr ) setPixel( putX, putY, p_bottom, clr, p_layer );
                }
            }
        }
    }

    void font::printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                            alignment p_alignment, u8 p_yDistance, s8 p_adjustX, u8 p_layer ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;
        if( p_alignment == RIGHT ) putX = p_x - stringWidth( p_string );
        if( p_alignment == CENTER ) putX = p_x - stringWidth( p_string ) / 2;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += p_yDistance;
                putX = ( p_x -= p_adjustX );
                if( p_alignment == RIGHT ) putX = p_x - stringWidth( p_string + current_char + 1 );
                if( p_alignment == CENTER )
                    putX = p_x - stringWidth( p_string + current_char + 1 ) / 2;

                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }

    void font::printMaxString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, s16 p_maxX,
                               u16 p_breakChar, u8 p_layer ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            if( putX + _widths[ c ] > p_maxX ) {
                printChar( p_breakChar, putX, putY, p_bottom, p_layer );
                break;
            } else
                printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );

            putX += _widths[ c ];
            current_char++;
        }
    }

    void font::printStringD( const char *p_string, s16 &p_x, s16 &p_y, bool p_bottom, u8 p_layer ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += 16;
                putX = p_x;
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0;
                 i < 80 / ( TEXTSPEED + SAVE::SAV->getActiveFile( ).m_options.m_textSpeedModifier );
                 ++i )
                swiWaitForVBlank( );
            current_char++;
        }
        p_x = putX;
        p_y = putY;
    }

    void drawContinue( font p_font, u8 p_x, u8 p_y, u8 p_layer ) {
        p_font.printChar( /*'@'*/ u16( 172 ), p_x, p_y, true, p_layer );
    }
    void hideContinue( u8 p_x, u8 p_y, u8 p_layer ) {
        BG_PALETTE_SUB[ 250 ] = WHITE;
        printRectangle( p_x, p_y, p_x + 5, p_y + 9, true, (u8) 250, p_layer );
    }

    void font::printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_layer ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += FONT_HEIGHT - 2;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == '`' ) {
                u8   c  = 0;
                bool on = false;
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    IO::updateOAM( false );
                }

                loop( ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    int pressed = keysCurrent( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 243, 51, p_layer );
                        else
                            drawContinue( *this, 243, 51, p_layer );
                        on = !on;
                    }
                    if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_B ) ) break;
                    touchPosition t;
                    touchRead( &t );
                    if( t.px > 224 && t.py > 164 && waitForTouchUp( 224, 164 ) ) { break; }
                }
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    IO::updateOAM( false );
                }
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            current_char++;
        }
    }
    void font::printMBStringD( const char *p_string, s16 &p_x, s16 &p_y, bool p_bottom,
                               u8 p_layer ) {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += FONT_HEIGHT - 2;
                putX = p_x;
                current_char++;
                continue;
            }
            if( p_string[ current_char ] == '`' ) {
                u8   c  = 0;
                bool on = false;
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = false;
                    IO::updateOAM( false );
                }

                loop( ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    int pressed = keysCurrent( );
                    if( ++c == 45 ) {
                        c = 0;
                        if( on )
                            hideContinue( 243, 51, p_layer );
                        else
                            drawContinue( *this, 243, 51, p_layer );
                        on = !on;
                    }
                    if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_B ) ) break;
                    touchPosition t;
                    touchRead( &t );
                    if( t.px > 224 && t.py > 164 && waitForTouchUp( 224, 164 ) ) { break; }
                }
                if( p_bottom ) {
                    Oam->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    updateOAM( true );
                } else {
                    OamTop->oamBuffer[ ASpriteOamIndex ].isHidden = true;
                    IO::updateOAM( false );
                }
                current_char++;
                continue;
            }
            printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );

            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            putX += _widths[ c ];

            for( u8 i = 0;
                 i < 80 / ( TEXTSPEED + SAVE::SAV->getActiveFile( ).m_options.m_textSpeedModifier );
                 ++i )
                swiWaitForVBlank( );
            current_char++;
        }
        p_x = putX;
        p_y = putY;
    }

    u32 font::stringWidth( const char *p_string ) const {
        u32 current_char = 0;
        u32 width        = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) break;
            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            width += _widths[ c ];

            current_char++;
        }

        return width;
    }
} // namespace IO
