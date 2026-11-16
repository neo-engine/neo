/*
Pokémon neo
------------------------------

file        : font.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2026
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

#include <algorithm>
#include <cstring>
#include <nds.h>
#include <nds/ndstypes.h>

#include "io/font.h"
#include "io/fontUtil.h"

namespace IO {
    u8  TEXTSPEED          = 50;
    u16 TMPBUF[ 256 * 64 ] = { 0 };

    font* regularFont = new font( REGULAR_FONT::fontData, REGULAR_FONT::fontWidths,
                                  REGULAR_FONT::shiftchar, REGULAR_FONT::parseKanaDigraph );
    font* boldFont    = new font( BOLD_FONT::fontData, BOLD_FONT::fontWidths, BOLD_FONT::shiftchar,
                                  BOLD_FONT::parseKanaDigraph );
    font* smallFont
        = new font( SMALL_FONT::fontData, SMALL_FONT::fontWidths, SMALL_FONT::shiftchar, nullptr );
    font* brailleFont = new font( BRAILLE_FONT::fontData, BRAILLE_FONT::fontWidths,
                                  BRAILLE_FONT::shiftchar, BRAILLE_FONT::parseKanaDigraph );

    bool kanaModeProcess( const std::pair<u16, u16>& p_token, bool& p_kanaMode,
                          u8& p_kanaCounter ) {
        if( p_token.first == KANA::ON ) {
            p_kanaMode = true;
            p_kanaCounter += p_kanaMode;
            return true;
        }
        if( p_token.first == KANA::OFF ) {
            if( p_kanaCounter ) {
                p_kanaMode = --p_kanaCounter;
            } else {
                p_kanaMode = false;
            }
            return true;
        }
        return false;
    }

    font::font( u8* p_data, u8* p_widths, void ( *p_shiftchar )( u16& ),
                void ( *p_parseKanaDigraph )( const char*, u32&, u16[ 2 ], u8& ) ) {
        _data       = p_data;
        _widths     = p_widths;
        _color[ 0 ] = _color[ 1 ] = _color[ 2 ] = _color[ 3 ] = _color[ 4 ] = WHITE;

        _shiftchar        = p_shiftchar;
        _parseKanaDigraph = p_parseKanaDigraph;
    }

    void font::charDelay( s8 p_textSpeedModifier ) const {
        for( u8 i = 0; i < 80 / ( TEXTSPEED + p_textSpeedModifier ); ++i ) { swiWaitForVBlank( ); }
    }

    u32 font::tokenWidth( const std::pair<u16, u16>& p_token, u8 p_charShift ) const {
        u32 width = 0;
        if( ( _parseKanaDigraph && ( p_token.first == KANA::ON || p_token.first == KANA::OFF ) )
            || p_token.first == '\n' ) {
            return 0;
        }
        u16 c = p_token.first;
        if( c ) {
            if( p_token.second != u16( -1 ) ) { _shiftchar( c ); }
            width += _widths[ c ] - p_charShift;
        }
        c = p_token.second;
        if( c && c != u16( -1 ) ) {
            _shiftchar( c );
            width += _widths[ c ] - p_charShift;
        }
        return width;
    }

    std::pair<u16, u16> font::nextToken( const char* p_string, u32& p_start,
                                         bool p_kanaMode ) const {
        u16 res = 0;
        if( !p_string[ p_start ] ) { return { 0, 0 }; }
        if( _parseKanaDigraph
            && ( p_string[ p_start ] == KANA::ON || p_string[ p_start ] == KANA::OFF ) ) {
            return { u16( p_string[ p_start++ ] ), 0 };
        }
        if( p_string[ p_start ] == '\n' || p_string[ p_start ] == ' ' ) {
            return { u16( p_string[ p_start++ ] ), 0 };
        }
        if( p_string[ p_start ] == '[' ) {
            p_start++;
            while( p_string[ p_start ] && p_string[ p_start ] != ']' ) {
                res *= 10;
                res += ( p_string[ p_start++ ] - '0' );
            }
            if( p_string[ p_start ] ) { p_start++; }
            return { res, u16( -1 ) };
        }
        if( p_kanaMode ) {
            u16 codes[ 2 ] = { 0 };
            u8  n          = 0;
            _parseKanaDigraph( p_string, p_start, codes, n );
            return { codes[ 0 ], codes[ 1 ] };
        }
        return { u16( p_string[ p_start++ ] ), 0 };
    }

    std::vector<std::pair<u16, u16>> font::tokenize( const char* p_string, u16 p_maxWidth,
                                                     char p_breakChar, u8 p_charShift ) const {
        std::vector<std::pair<u16, u16>> outTokens{ };
        std::vector<std::pair<u16, u16>> tmpTokens{ };

        u32  current_char   = 0;
        u32  width          = 0;
        u32  lasttmpwidth   = 0;
        bool kanaModeOn     = false;
        u8   kanaCounter    = 0;
        bool needsBreakChar = false;

        while( p_string[ current_char ] ) {
            const auto tok = nextToken( p_string, current_char, kanaModeOn );
            if( _parseKanaDigraph && kanaModeProcess( tok, kanaModeOn, kanaCounter ) ) { continue; }
            width += tokenWidth( tok, p_charShift );

            if( tok.first == p_breakChar ) {
                // commit all current tokens
                if( needsBreakChar ) { outTokens.push_back( { p_breakChar, 0 } ); }
                needsBreakChar = true;
                outTokens.append_range( tmpTokens );
                tmpTokens.clear( );
                lasttmpwidth = width;
                continue;
            }
            if( tok.first == '\n' ) {
                if( needsBreakChar ) { outTokens.push_back( { p_breakChar, 0 } ); }
                outTokens.append_range( tmpTokens );
                outTokens.push_back( { '\n', 0 } );
                tmpTokens.clear( );
                width          = 0;
                needsBreakChar = false;
                continue;
            }
            tmpTokens.push_back( tok );
            if( needsBreakChar && width >= p_maxWidth ) {
                // insert an extra newline
                if( needsBreakChar ) { outTokens.push_back( { '\n', 0 } ); }
                needsBreakChar = false;
                width -= lasttmpwidth;
                lasttmpwidth = width;
            }
        }
        if( !tmpTokens.empty( ) ) {
            if( needsBreakChar ) { outTokens.push_back( { p_breakChar, 0 } ); }
            outTokens.append_range( tmpTokens );
        }
        return outTokens;
    }

    u32 font::stringWidth( const char* p_string, u8 p_charShift, bool p_startInKanaMode ) const {
        u32  current_char = 0;
        u32  width        = 0;
        bool kanaModeOn   = p_startInKanaMode;
        u8   kanaCounter  = 0;

        while( p_string[ current_char ] ) {
            const auto tok = nextToken( p_string, current_char, kanaModeOn );
            if( tok.first == '\n' ) { break; }
            if( _parseKanaDigraph && kanaModeProcess( tok, kanaModeOn, kanaCounter ) ) { continue; }
            width += tokenWidth( tok, p_charShift );
        }
        return width;
    }

    u32 font::stringWidthC( const char* p_string ) const {
        return stringWidth( p_string, 1 );
    }

    u32 font::stringMaxWidth( const char* p_string, u16 p_maxWidth, char p_breakChar,
                              u8 p_charShift, bool p_startInKanaMode ) const {
        u32  current_char = 0;
        u32  width        = 0;
        u32  lasttmpwidth = 0;
        bool kanaModeOn   = p_startInKanaMode;
        u8   kanaCounter  = 0;

        while( p_string[ current_char ] ) {
            const auto tok = nextToken( p_string, current_char, kanaModeOn );
            if( tok.first == '\n' ) { break; }
            if( _parseKanaDigraph && kanaModeProcess( tok, kanaModeOn, kanaCounter ) ) { continue; }
            width += tokenWidth( tok, p_charShift );
            if( tok.first == p_breakChar ) { lasttmpwidth = width; }
            if( width >= p_maxWidth ) { return lasttmpwidth; }
        }
        return width;
    }

    u32 font::stringMaxWidthC( const char* p_string, u16 p_maxWidth, char p_breakChar ) const {
        return stringMaxWidth( p_string, p_maxWidth, p_breakChar, 1 );
    }

    u16 font::printChar( u16 p_ch, s16 p_x, s16 p_y, bool p_bottom, u8 p_layer,
                         bool p_shift ) const {
        if( p_shift ) { _shiftchar( p_ch ); }

        s16 putX, putY;
        u8  getX, getY;
        u32 offset = p_ch * FONT_WIDTH * FONT_HEIGHT;

        if( p_layer != u8( -1 ) ) {
            for( putY = p_y, getY = 0; putY < p_y + FONT_HEIGHT; ++putY, ++getY ) {
                for( putX = p_x, getX = 0; putX < p_x + _widths[ p_ch ]; putX++, getX++ ) {
                    if( putX >= 0 && putX < SCREEN_WIDTH && putY >= 0 && putY < 256 ) {
                        u8 clr = _color[ _data[ offset + ( getX + getY * FONT_WIDTH ) ] ];
                        if( clr ) { setPixel( putX, putY, p_bottom, clr, p_layer ); }
                    }
                }
            }
        }
        return _widths[ p_ch ];
    }

    u16 font::printCharB( u16 p_ch, const u16* p_palette, u16* p_buffer, u16 p_bufferWidth, s16 p_x,
                          s16 p_y, bool p_shift ) const {
        if( p_shift ) { _shiftchar( p_ch ); }

        s16 putX, putY;
        u16 getX, getY;
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
                printRectangle( p_x + ( p_digits - i - 1 ) * 8, p_y + 2,
                                p_x + ( p_digits - i ) * 8 - 1, p_y + 14, p_bottom, p_highlightBG,
                                p_layer );
                setColor( p_highlightFG, 1 );
            } else {
                printRectangle( p_x + ( p_digits - i - 1 ) * 8, p_y + 2,
                                p_x + ( p_digits - i ) * 8 - 1, p_y + 14, p_bottom, 0, p_layer );
            }
            printChar( '0' + ( p_value % 10 ), p_x + ( p_digits - i - 1 ) * 8, p_y, p_bottom,
                       p_layer );
            if( i == p_highlightDigit ) { setColor( old, 1 ); }
        }
    }

    std::vector<u32> font::lineWidths( const std::vector<std::pair<u16, u16>>& p_tokens,
                                       u8                                      p_charShift ) const {
        std::vector<u32> res{ };
        u32              wd = 0;
        for( const auto& tok : p_tokens ) {
            wd += tokenWidth( tok, p_charShift );
            if( tok.first == '\n' ) {
                res.push_back( wd );
                wd = 0;
            }
        }
        if( wd ) { res.push_back( wd ); }
        return res;
    }

    u16 font::printString( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                           alignment p_alignment, u8 p_yDistance, s8 p_adjustX, u8 p_charShift,
                           bool p_delay, u8 p_layer, s8 p_textSpeedModifier ) const {
        return printBreakingString( p_string, p_x, p_y, -1, p_bottom, p_alignment, p_yDistance, ' ',
                                    p_adjustX, p_charShift, p_delay, p_layer, p_textSpeedModifier );
    }

    u16 font::printStringC( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                            alignment p_alignment, u8 p_yDistance, s8 p_adjustX, bool p_delay,
                            u8 p_layer, s8 p_textSpeedModifier ) const {
        return printString( p_string, p_x, p_y, p_bottom, p_alignment, p_yDistance, p_adjustX, 1,
                            p_delay, p_layer, p_textSpeedModifier );
    }

    u16 font::printStringD( const char* p_string, s16 p_x, s16 p_y, bool p_bottom,
                            alignment p_alignment, u8 p_yDistance, s8 p_adjustX, u8 p_charShift,
                            u8 p_layer, s8 p_textSpeedModifier ) const {
        return printString( p_string, p_x, p_y, p_bottom, p_alignment, p_yDistance, p_adjustX,
                            p_charShift, true, p_layer, p_textSpeedModifier );
    }

    void bitmapScramble( u16* p_bufferIn, u16* p_bufferOut, u16 p_bufferWidth, u8 p_chunkSize,
                         u16 p_bufferHeight ) {
        u32 pos = 0;
        for( u32 i = 0; i < p_bufferWidth / p_chunkSize; ++i ) {
            u32 ub
                = std::min( u32( p_chunkSize ), p_bufferWidth - i * p_bufferWidth / p_chunkSize );
            for( u32 y = 0; y < p_bufferHeight; ++y ) {
                for( u32 x = 0; x < ub; ++x ) {
                    p_bufferOut[ pos++ ] = p_bufferIn[ y * p_bufferWidth + x + i * p_chunkSize ];
                }
            }
        }
    }
    u16 font::printStringB( const char* p_string, const u16* p_palette, u16* p_buffer,
                            u16 p_bufferWidth, alignment p_alignment, u8 p_yDistance,
                            u8 p_charShift, u8 p_chunkSize, u16 p_bufferHeight,
                            u16 p_rightPadding ) const {
        u16        lineSpace = p_bufferWidth - p_rightPadding;
        const auto toks      = tokenize( p_string, lineSpace, ' ', p_charShift );
        return printTokensB( toks, p_palette, p_buffer, p_bufferWidth, p_alignment, p_yDistance,
                             p_charShift, p_chunkSize, p_bufferHeight, p_rightPadding );
    }

    u16 font::printTokensB( const std::vector<std::pair<u16, u16>>& p_tokens, const u16* p_palette,
                            u16* p_buffer, u16 p_bufferWidth, alignment p_alignment, u8 p_yDistance,
                            u8 p_charShift, u8 p_chunkSize, u16 p_bufferHeight,
                            u16 p_rightPadding ) const {

        s16        putX = 0, putY = 0;
        u32        curLine   = 0;
        u16        lineSpace = p_bufferWidth - p_rightPadding;
        const auto linewd    = lineWidths( p_tokens, p_charShift );
        if( p_alignment == RIGHT ) { putX = lineSpace - linewd[ curLine ]; }
        if( p_alignment == CENTER ) { putX = ( lineSpace - linewd[ curLine ] ) / 2; }
        std::memset( TMPBUF, 0, sizeof( TMPBUF ) );

        for( const auto& [ c1, c2 ] : p_tokens ) {
            if( c1 == '\n' ) {
                putY += p_yDistance;
                curLine++;
                if( p_alignment == LEFT ) { putX = 0; }
                if( p_alignment == RIGHT ) { putX = lineSpace - linewd[ curLine ]; }
                if( p_alignment == CENTER ) { putX = ( lineSpace - linewd[ curLine ] ) / 2; }
                continue;
            }
            putX += printCharB( c1, p_palette, TMPBUF, p_bufferWidth, putX, putY, c2 != u16( -1 ) )
                    - p_charShift;
            if( c2 && c2 != u16( -1 ) ) {
                putX += printCharB( c2, p_palette, TMPBUF, p_bufferWidth, putX, putY,
                                    c2 != u16( -1 ) )
                        - p_charShift;
            }
        }

        bitmapScramble( TMPBUF, p_buffer, p_bufferWidth, p_chunkSize, p_bufferHeight );
        return linewd.size( );
    }

    u16 font::printStringBC( const char* p_string, const u16* p_palette, u16* p_buffer,
                             u16 p_bufferWidth, alignment p_alignment, u8 p_yDistance,
                             u8 p_chunkSize, u16 p_bufferHeight, u16 p_rightPadding ) const {
        return printStringB( p_string, p_palette, p_buffer, p_bufferWidth, p_alignment, p_yDistance,
                             1, p_chunkSize, p_bufferHeight, p_rightPadding );
    }

    u16 font::printBreakingString( const char* p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                   bool p_bottom, alignment p_alignment, u8 p_yDistance,
                                   char p_breakChar, s8 p_adjustX, u8 p_charShift, bool p_delay,
                                   u8 p_layer, s8 p_textSpeedModifier ) const {
        s16        putX = p_x, putY = p_y;
        u32        curLine = 0;
        const auto toks    = tokenize( p_string, u16( p_maxWidth ), p_breakChar, p_charShift );
        const auto linewd  = lineWidths( toks, p_charShift );
        if( p_alignment == RIGHT ) { putX = p_x - linewd[ curLine ]; }
        if( p_alignment == CENTER ) { putX = p_x - linewd[ curLine ] / 2; }

        for( const auto& [ c1, c2 ] : toks ) {
            if( c1 == '\n' ) {
                putY += p_yDistance;
                putX = ( p_x -= p_adjustX );
                curLine++;
                if( p_alignment == RIGHT ) { putX = p_x - linewd[ curLine ]; }
                if( p_alignment == CENTER ) { putX = p_x - linewd[ curLine ] / 2; }
                continue;
            }
            putX += printChar( c1, putX, putY, p_bottom, p_layer, c2 != u16( -1 ) ) - p_charShift;
            if( p_delay ) { charDelay( p_textSpeedModifier ); }
            if( c2 && c2 != u16( -1 ) ) {
                putX += printChar( c2, putX, putY, p_bottom, p_layer ) - p_charShift;
                if( p_delay ) { charDelay( p_textSpeedModifier ); }
            }
        }
        return linewd.size( );
    }

    u16 font::printBreakingStringC( const char* p_string, s16 p_x, s16 p_y, s16 p_maxWidth,
                                    bool p_bottom, alignment p_alignment, u8 p_yDistance,
                                    char p_breakChar, s8 p_adjustX, bool p_delay, u8 p_layer,
                                    s8 p_textSpeedModifier ) const {
        return printBreakingString( p_string, p_x, p_y, p_maxWidth, p_bottom, p_alignment,
                                    p_yDistance, p_breakChar, p_adjustX, 1, p_delay, p_layer,
                                    p_textSpeedModifier );
    }

    void font::printMaxString( const char* p_string, s16 p_x, s16 p_y, bool p_bottom, s16 p_maxX,
                               u16 p_breakChar, u8 p_charShift, bool p_delay, u8 p_layer,
                               s8 p_textSpeedModifier ) const {
        s16        putX = p_x, putY = p_y;
        const auto toks = tokenize( p_string, u16( p_maxX - p_x + 1 ), p_breakChar, p_charShift );

        for( const auto& [ c1, c2 ] : toks ) {
            auto wd = tokenWidth( std::pair<u16, u16>( c1, c2 ), p_charShift );

            if( s32( putX + wd ) > p_maxX ) {
                printChar( p_breakChar, putX, putY, p_bottom, p_layer );
                break;
            } else {
                putX += printChar( c1, putX, putY, p_bottom, p_layer, c2 != u16( -1 ) )
                        - p_charShift;
                if( p_delay ) { charDelay( p_textSpeedModifier ); }
                if( c2 && c2 != u16( -1 ) ) {
                    putX += printChar( c2, putX, putY, p_bottom, p_layer ) - p_charShift;
                    if( p_delay ) { charDelay( p_textSpeedModifier ); }
                }
            }
        }
    }

    void font::printMaxStringC( const char* p_string, s16 p_x, s16 p_y, bool p_bottom, s16 p_maxX,
                                u16 p_breakChar, bool p_delay, u8 p_layer,
                                s8 p_textSpeedModifier ) const {
        return printMaxString( p_string, p_x, p_y, p_bottom, p_maxX, p_breakChar, 1, p_delay,
                               p_layer, p_textSpeedModifier );
    }

} // namespace IO
