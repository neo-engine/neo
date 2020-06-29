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

    void font::_runMB( bool p_bottom, u8 p_layer ) const {
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
                    drawContinue( 243, 51, p_layer );
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
    }

    void font::_charDelay( ) const {
        for( u8 i = 0;
             i < 80 / ( TEXTSPEED + SAVE::SAV.getActiveFile( ).m_options.m_textSpeedModifier );
             ++i )
            swiWaitForVBlank( );
    }

    u32 font::stringWidth( const char *p_string, u8 p_charShift ) const {
        u32 current_char = 0;
        u32 width        = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) break;
            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            width += _widths[ c ] - p_charShift;

            current_char++;
        }

        return width;
    }

    u32 font::stringWidthC( const char *p_string ) const {
        return stringWidth( p_string, 1 );
    }

    u32 font::stringMaxWidth( const char *p_string, u16 p_maxWidth, char p_breakChar,
                              u8 p_charShift ) const {
        u32 current_char = 0;
        u32 width        = 0;
        u32 lasttmpwidth = 0;

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) { return width; }
            if( p_string[ current_char ] == p_breakChar ) { lasttmpwidth = width; }
            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            width += _widths[ c ] - p_charShift;
            if( width >= p_maxWidth ) { return lasttmpwidth; }
            current_char++;
        }

        return width;
    }

    u32 font::stringMaxWidthC( const char *p_string, u16 p_maxWidth, char p_breakChar ) const {
        return stringMaxWidth( p_string, p_maxWidth, p_breakChar, 1 );
    }

    u16 font::printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom, u8 p_layer ) const {
        _shiftchar( p_ch );

        s16 putX, putY;
        u8  getX, getY;
        u32 offset = p_ch * FONT_WIDTH * FONT_HEIGHT;

        if( p_layer != u8( -1 ) ) {
            for( putY = p_y, getY = 0; putY < p_y + FONT_HEIGHT; ++putY, ++getY ) {
                for( putX = p_x, getX = 0; putX < p_x + _widths[ p_ch ]; putX++, getX++ ) {
                    if( putX >= 0 && putX < SCREEN_WIDTH && putY >= 0 && putY < 256 ) {
                        u8 clr = _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ];
                        if( clr ) setPixel( putX, putY, p_bottom, clr, p_layer );
                    }
                }
            }
        }
        return _widths[ p_ch ];
    }

    u16 font::printCharB( u16 p_ch, u16 *p_palette, u16 *p_buffer, u16 p_bufferWidth, s16 p_x,
                          s16 p_y ) const {
        _shiftchar( p_ch );

        s16 putX, putY;
        u8  getX, getY;
        u32 offset = p_ch * FONT_WIDTH * FONT_HEIGHT;

        for( putY = p_y, getY = 0; putY < p_y + FONT_HEIGHT; ++putY, ++getY ) {
            for( putX = p_x, getX = 0; putX < p_x + _widths[ p_ch ]; putX++, getX++ ) {
                if( putX >= 0 && putX < p_bufferWidth ) {
                    u8 clr = _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ];
                    if( clr ) {
                        p_buffer[ p_bufferWidth * putY + putX ] = ( 1 << 15 ) | p_palette[ clr ];
                    }
                }
            }
        }
        return _widths[ p_ch ];
    }

    void font::drawContinue( u8 p_x, u8 p_y, bool p_bottom, u8 p_layer ) const {
        printChar( /*'@'*/ u16( 172 ), p_x, p_y, p_bottom, p_layer );
    }

    void font::hideContinue( u8 p_x, u8 p_y, u8 p_color, bool p_bottom, u8 p_layer ) const {
        printRectangle( p_x, p_y, p_x + 5, p_y + 9, p_bottom, p_color, p_layer );
    }

    void font::printCounter( u32 p_value, u8 p_digits, u16 p_x, u16 p_y, u8 p_highlightDigit,
                             u8 p_highlightBG, u8 p_highlightFG, bool p_bottom, u8 p_layer ) {
        for( u8 i = 0; i < p_digits; ++i, p_value /= 10 ) {
            auto old = getColor( 1 );
            if( i == p_highlightDigit ) {
                IO::printRectangle( p_x + ( p_digits - i - 1 ) * 8, p_y + 2,
                                    p_x + ( p_digits - i ) * 8 - 1, p_y + 14, p_bottom,
                                    p_highlightBG, p_layer );
                setColor( p_highlightFG, 1 );
            } else {
                IO::printRectangle( p_x + ( p_digits - i - 1 ) * 8, p_y + 2,
                                    p_x + ( p_digits - i ) * 8 - 1, p_y + 14, p_bottom, 0,
                                    p_layer );
            }
            printChar( '0' + ( p_value % 10 ), p_x + ( p_digits - i - 1 ) * 8, p_y, p_bottom,
                       p_layer );
            if( i == p_highlightDigit ) { setColor( old, 1 ); }
        }
    }

    u16 font::printString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                           alignment p_alignment, u8 p_yDistance, s8 p_adjustX, u8 p_charShift,
                           bool p_delay, bool p_mb, u8 p_layer ) const {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;
        u16 lines = 1;
        if( p_alignment == RIGHT ) { putX = p_x - stringWidth( p_string, p_charShift ); }
        if( p_alignment == CENTER ) { putX = p_x - stringWidth( p_string, p_charShift ) / 2; }

        while( p_string[ current_char ] ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += p_yDistance;
                putX = ( p_x -= p_adjustX );
                if( p_alignment == RIGHT ) {
                    putX = p_x - stringWidth( p_string + current_char + 1, p_charShift );
                }
                if( p_alignment == CENTER ) {
                    putX = p_x - stringWidth( p_string + current_char + 1, p_charShift ) / 2;
                }

                current_char++;
                lines++;
                continue;
            }
            if( p_mb && p_string[ current_char ] == '`' ) {
                _runMB( p_bottom );
                current_char++;
                continue;
            }

            putX += printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer )
                    - p_charShift;

            if( p_delay ) { _charDelay( ); }

            current_char++;
        }
        return lines;
    }

    u16 font::printStringC( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                            alignment p_alignment, u8 p_yDistance, s8 p_adjustX, bool p_delay,
                            bool p_mb, u8 p_layer ) const {
        return printString( p_string, p_x, p_y, p_bottom, p_alignment, p_yDistance, p_adjustX, 1,
                            p_delay, p_mb, p_layer );
    }

    u16 font::printStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom,
                            alignment p_alignment, u8 p_yDistance, s8 p_adjustX, u8 p_charShift,
                            bool p_mb, u8 p_layer ) const {
        return printString( p_string, p_x, p_y, p_bottom, p_alignment, p_yDistance, p_adjustX,
                            p_charShift, true, p_mb, p_layer );
    }

    u16 font::printMBString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_charShift,
                             bool p_delay, u8 p_layer ) const {
        return printString( p_string, p_x, p_y, p_bottom, font::LEFT, FONT_HEIGHT - 2, 0,
                            p_charShift, p_delay, true, p_layer );
    }

    u16 font::printMBStringD( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, u8 p_charShift,
                              u8 p_layer ) const {
        return printMBString( p_string, p_x, p_y, p_bottom, p_charShift, true, p_layer );
    }

    u16 font::printStringB( const char *p_string, u16 *p_palette, u16 *p_buffer, u16 p_bufferWidth,
                            alignment p_alignment, u8 p_yDistance, u8 p_charShift ) const {
        u32 current_char = 0;
        s16 putX = 0, putY = 0;
        u16 lines = 1;
        if( p_alignment == RIGHT ) { putX = p_bufferWidth - stringWidth( p_string, p_charShift ); }
        if( p_alignment == CENTER ) {
            putX = ( p_bufferWidth - stringWidth( p_string, p_charShift ) ) / 2;
        }

        while( p_string[ current_char ] && putX < p_bufferWidth ) {
            if( p_string[ current_char ] == '\n' ) {
                putY += p_yDistance;
                if( p_alignment == RIGHT ) {
                    putX = p_bufferWidth - stringWidth( p_string + current_char + 1, p_charShift );
                }
                if( p_alignment == CENTER ) {
                    putX = ( p_bufferWidth
                             - stringWidth( p_string + current_char + 1, p_charShift ) )
                           / 2;
                }

                current_char++;
                lines++;
                continue;
            }
            putX += printCharB( p_string[ current_char ], p_palette, p_buffer, p_bufferWidth, putX,
                                putY )
                    - p_charShift;
            current_char++;
        }
        return lines;
    }

    u16 font::printStringBC( const char *p_string, u16 *p_palette, u16 *p_buffer, u16 p_bufferWidth,
                             alignment p_alignment, u8 p_yDistance ) const {
        return printStringB( p_string, p_palette, p_buffer, p_bufferWidth, p_alignment, p_yDistance,
                             1 );
    }

    u16 font::printBreakingString( const char *p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                   bool p_bottom, alignment p_alignment, u8 p_yDistance,
                                   char p_breakChar, s8 p_adjustX, u8 p_charShift, bool p_delay,
                                   bool p_mb, u8 p_layer ) const {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;
        u16 lines = 1;

        s16 lineWd = stringMaxWidth( p_string, p_maxWidth, p_breakChar, p_charShift );
        if( p_alignment == RIGHT ) putX = p_x - lineWd;
        if( p_alignment == CENTER ) putX = p_x - lineWd / 2;

        while( p_string[ current_char ] ) {
            if( lineWd <= 0 || p_string[ current_char ] == '\n' ) {
                putY += p_yDistance;
                putX   = ( p_x -= p_adjustX );
                lineWd = stringMaxWidth( p_string + current_char + 1, p_maxWidth, p_breakChar,
                                         p_charShift );
                if( p_alignment == RIGHT ) putX = p_x - lineWd;
                if( p_alignment == CENTER ) putX = p_x - lineWd / 2;

                current_char++;
                lines++;
                continue;
            }
            if( p_mb && p_string[ current_char ] == '`' ) {
                _runMB( p_bottom );
                current_char++;
                continue;
            }

            u16 wd = printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );
            putX += wd - p_charShift;
            lineWd -= wd - p_charShift;

            if( p_delay ) { _charDelay( ); }

            current_char++;
        }
        return lines;
    }

    u16 font::printBreakingStringC( const char *p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                    bool p_bottom, alignment p_alignment, u8 p_yDistance,
                                    char p_breakChar, s8 p_adjustX, bool p_delay, bool p_mb,
                                    u8 p_layer ) const {
        return printBreakingString( p_string, p_x, p_y, p_maxWidth, p_bottom, p_alignment,
                                    p_yDistance, p_breakChar, p_adjustX, 1, p_delay, p_mb,
                                    p_layer );
    }

    void font::printMaxString( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, s16 p_maxX,
                               u16 p_breakChar, u8 p_charShift, bool p_delay, u8 p_layer ) const {
        u32 current_char = 0;
        s16 putX = p_x, putY = p_y;

        while( p_string[ current_char ] ) {
            u16 c = (u16) p_string[ current_char ];
            _shiftchar( c );
            u16 wd = _widths[ c ] - p_charShift;
            if( putX + wd > p_maxX ) {
                printChar( p_breakChar, putX, putY, p_bottom, p_layer );
                break;
            } else
                printChar( p_string[ current_char ], putX, putY, p_bottom, p_layer );

            if( p_delay ) { _charDelay( ); }

            putX += wd;
            current_char++;
        }
    }

    void font::printMaxStringC( const char *p_string, s16 p_x, s16 p_y, bool p_bottom, s16 p_maxX,
                                u16 p_breakChar, bool p_delay, u8 p_layer ) const {
        return printMaxString( p_string, p_x, p_y, p_bottom, p_maxX, p_breakChar, 1, p_delay,
                               p_layer );
    }

} // namespace IO
