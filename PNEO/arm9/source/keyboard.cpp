/*
Pokémon neo
------------------------------

file        : keyboard.cpp
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

#include <nds.h>

#include <io/inputTarget.h>
#include "fs/data.h"
#include "fs/fs.h"
#include "io/kana.h"
#include "io/keyboard.h"
#include "io/strings.h"
#include "io/util.h"
#include "sound/sound.h"

#include "key.h"

namespace IO {
    constexpr u8 HEIGHT = 16, WIDTH = 12;
    constexpr u8 SX = 25, SY = 38;
    constexpr u8 MARGIN_X      = 4;
    constexpr u8 MARGIN_Y      = 8;
    constexpr u8 CHARS_PER_ROW = 13;
    constexpr u8 NUM_ROWS      = 5;

    constexpr u16 KBD_DAKUTEN          = 0xFF00;
    constexpr u16 KBD_HANDAKUTEN       = 0xFF01;
    constexpr u16 CHAR_DAKUTEN_COMP    = 113;
    constexpr u16 CHAR_HANDAKUTEN_COMP = 130;

    constexpr u16 PAGES[ MAX_KEYBOARD_PAGES ][ NUM_ROWS * CHARS_PER_ROW ] = {
        { 'A',    'B', 'C',    'D',    'E',    'F',    'G',    'H', 'I',    'J',    'K',
          'L',    'M', 'N',    'O',    'P',    'Q',    'R',    'S', 'T',    'U',    'V',
          'W',    'X', 'Y',    'Z',    'a',    'b',    'c',    'd', 'e',    'f',    'g',
          'h',    'i', 'j',    'k',    'l',    'm',    'n',    'o', 'p',    'q',    'r',
          's',    't', 'u',    'v',    'w',    'x',    'y',    'z', '\xc4', '\xc9', '\xd6',
          '\xdc', ' ', '\xe4', '\xe9', '\xf6', '\xfc', '\xdf', ' ', '.',    ',' },

        { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ' ', '+', '-',

          192, 193, 196, 197, ' ', 199, 200, 201, 202, 203, 204, 205, 206,
          207, ' ', 209, 210, 211, 212, 214, 217, 218, 219, 220, ' ', 223,

          224, 225, 226, 228, 229, 231, 232, 233, 234, 235, 236, 237, 238,
          239, 241, 242, 243, 244, 246, 249, 250, 251, 252, ' ', '!', '?' },

        {
            5001, 5010, 5020, 5030, 5041, 5046, 5061, 5067, 5072, 5077, 5000, 5066, KBD_DAKUTEN,
            5003, 5012, 5022, 5032, 5042, 5049, 5062, ' ',  5073, ' ',  5002, 5068, KBD_HANDAKUTEN,
            5005, 5014, 5024, 5035, 5043, 5052, 5063, 5069, 5074, 5078, 5004, 5070, 5245,
            5007, 5016, 5026, 5037, 5044, 5055, 5064, ' ',  5075, ' ',  5006, ' ',  5224,
            5009, 5018, 5028, 5039, 5045, 5058, 5065, 5071, 5076, 5079, 5008, 5034, 5225,
        },

        {
            5081, 5090, 5100, 5110, 5121, 5126, 5141, 5147, 5152, 5157, 5080, 5146, KBD_DAKUTEN,
            5083, 5092, 5102, 5112, 5122, 5129, 5142, ' ',  5153, ' ',  5082, 5148, KBD_HANDAKUTEN,
            5085, 5094, 5104, 5115, 5123, 5132, 5143, 5149, 5154, 5158, 5084, 5150, 5245,
            5087, 5096, 5106, 5117, 5124, 5135, 5144, ' ',  5155, ' ',  5086, ' ',  5224,
            5089, 5098, 5108, 5119, 5125, 5138, 5145, 5151, 5156, 5159, 5088, 5114, 5225,
        },

        {
            5170, 5171, 5172, 5173, 5174, 5175, 5176, 5177, 5178, 5179, 5180, 5181, 5182,
            5183, 5184, 5185, 5186, 5187, 5188, 5189, 5190, 5191, 5192, 5193, 5194, 5195,
            5196, 5197, 5198, 5199, 5200, 5201, 5202, 5203, 5204, 5205, 5206, 5207, 5208,
            5209, 5210, 5211, 5212, 5213, 5214, 5215, 5216, 5217, 5218, 5219, 5220, 5221,
            5160, 5161, 5162, 5163, 5164, 5165, 5166, 5167, 5168, 5169, '.',  ',',  ' ',
        },
        {
            5223, 5224, 5225, 5226, 5227, 5228, 5229, 5230, 5231, 5232, 5233, 5234, 5235,
            5236, 5237, 5238, 5239, 5240, 5241, 5242, 5243, 5244, 5245, 5246, 5247, 5248,
            5249, 5250, 5251, 5252, 5253, 5254, 5255, 5256, 5257, ' ',  5259, 5260, 5261,
            5262, 5263, 5264, 5265, 5266, 5267, 5268, 5269, 5270, 5271, 5272, ' ',  ' ',
            5281, 5282, 5283, 5284, ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
        } };

    std::string guardEmptyString( const std::string& p_string ) {
        for( auto i : p_string )
            if( i != ' ' ) return p_string;
        return "";
    }

    std::string keyboard::getText( u8 p_length ) {
        bool kanaMode = CURRENT_LANGUAGE == SAVE::JAP;
        _page         = pageStart( );
        _lastKanaC1 = _lastKanaC2 = 0;
        init( );
        for( u8 i = 0; i < p_length; i += kanaMode ? 2 : 1 ) { clearChar( i ); }

        std::string res = "";
        u16         c;
        u8          pos  = 0; // offset in res
        u8          cell = 0; // cell offset on screen
        loop( ) {
            c = getNextChar( );
            if( c < maxPages( ) ) {
                _page = c;
                drawPage( );
                continue;
            }

            switch( c ) {
            case '\n': return guardEmptyString( res );
            case '\b':
                if( kanaMode ) {
                    if( pos ) {
                        u8 last = 0, len = 1;
                        for( u8 i = 0; i < pos; i += len ) {
                            last = i;
                            len  = ( res[ i ] == ' ' ) ? 1 : 2;
                        }
                        pos -= len;
                        cell -= 1;
                        clearChar( cell * 2 );
                        res.erase( last );
                        if( pos >= 2 && res[ pos - 1 ] != ' ' ) {
                            _lastKanaC1 = res[ pos - 2 ];
                            _lastKanaC2 = res[ pos - 1 ];
                        } else {
                            _lastKanaC1 = _lastKanaC2 = 0;
                        }
                        drawMarkKeys( );
                    }
                } else if( pos ) {
                    clearChar( --pos );
                    res.pop_back( );
                }
                break;
            case KBD_DAKUTEN:
            case KBD_HANDAKUTEN: {
                bool half = ( c == KBD_HANDAKUTEN );
                char c1 = 0, c2 = 0;
                if( kanaMode && pos >= 2 ) {
                    c1 = res[ pos - 2 ];
                    c2 = res[ pos - 1 ];
                    if( KANA::hasVariant( c1, c2, half )
                        && KANA::toggleVoicedness( c1, c2, half ) ) {
                        res[ pos - 2 ] = c1;
                        _lastKanaC1    = c1;
                        _lastKanaC2    = c2;
                        drawChar( ( cell - 1 ) * 2,
                                  KANA_CODE_START + KANA::digraphToGlyphOrderIndex( c1, c2 ) );
                        drawMarkKeys( );
                        break;
                    }
                }
                break;
            }

            default:
                if( kanaMode ) {
                    char c1 = 0, c2 = 0;
                    u16  drawCode = c;
                    u8   need     = 2;
                    if( c == ' ' ) {
                        c1       = ' ';
                        need     = 1;
                        drawCode = 0;
                    } else if( isKana( c ) ) {
                        c1 = KANA::GLYPH_ORDER[ valToKana( c, 0 ) ][ 0 ];
                        c2 = KANA::GLYPH_ORDER[ valToKana( c, 0 ) ][ 1 ];
                    } else if( isNumeral( c ) || isUpper( c ) || isLower( c ) ) {
                        // full width chars
                        c1       = 'x';
                        c2       = (char) c;
                        drawCode = KANA::digraphToGlyphOrderIndex( c1, c2 );
                    } else if( KANA::digraphToGlyphOrderIndex( 'x', (char) c ) != KANA::NO_CODE ) {
                        // full width punctuation
                        c1 = 'x';
                        c2 = (char) c;
                    }
                    if( pos + need <= p_length ) {
                        res += c1;
                        if( need == 2 ) { res += c2; }
                        if( drawCode ) { drawChar( cell * 2, drawCode ); }
                        pos += need;
                        cell += 1;
                        _lastKanaC1 = c1;
                        _lastKanaC2 = need == 2 ? c2 : 0;
                        drawMarkKeys( );
                    }
                } else if( pos < p_length ) {
                    res += c;
                    drawChar( pos++, c );
                }

                if( pos >= p_length ) { select( _curSel = 254, _frame ); }
                break;
            }
        }
    }

    void keyboard::select( u8 p_idx, u8 p_frame ) {
        auto& oam = Oam->oamBuffer;

        if( p_idx == u8( -1 ) ) {
            oam[ 0 ].isHidden = true;
            oam[ 1 ].isHidden = true;
            oam[ 2 ].isHidden = true;
            oam[ 3 ].isHidden = true;
            updateOAM( true );
            return;
        }

        s8 sml = 2 * !!( ( p_frame & 15 ) <= 7 ) - 1;

        oam[ 0 ].isHidden = false;
        oam[ 1 ].isHidden = false;
        oam[ 2 ].isHidden = false;
        oam[ 3 ].isHidden = false;

        if( p_idx < CHARS_PER_ROW * NUM_ROWS ) {
            auto x  = p_idx % CHARS_PER_ROW;
            auto y  = p_idx / CHARS_PER_ROW;
            u8   px = SX + x * ( WIDTH + MARGIN_X );
            u8   py = SY + y * ( HEIGHT + MARGIN_Y );

            oam[ 0 ].x = px - 2 + sml;
            oam[ 0 ].y = py - 1 + sml;
            oam[ 1 ].x = px - 2 + sml;
            oam[ 1 ].y = py + HEIGHT - 1 - sml;
            oam[ 2 ].x = px + WIDTH - 5 - sml;
            oam[ 2 ].y = py + HEIGHT - 1 - sml;
            oam[ 3 ].x = px + WIDTH - 5 - sml;
            oam[ 3 ].y = py - 1 + sml;
        } else if( p_idx == 254 ) { // enter
            auto x   = 9;
            auto x2  = 12;
            auto y   = 5;
            u8   px  = SX + x * ( WIDTH + MARGIN_X );
            u8   px2 = SX + x2 * ( WIDTH + MARGIN_X );
            u8   py  = SY + y * ( HEIGHT + MARGIN_Y );

            oam[ 0 ].x = px - 2 + sml;
            oam[ 0 ].y = py - 1 + sml;
            oam[ 1 ].x = px - 2 + sml;
            oam[ 1 ].y = py + HEIGHT - 1 - sml;
            oam[ 2 ].x = px2 + WIDTH - 5 - sml;
            oam[ 2 ].y = py + HEIGHT - 1 - sml;
            oam[ 3 ].x = px2 + WIDTH - 5 - sml;
            oam[ 3 ].y = py - 1 + sml;
        } else if( p_idx == 253 ) { // backspace
            auto x   = 5;
            auto x2  = 8;
            auto y   = 5;
            u8   px  = SX + x * ( WIDTH + MARGIN_X );
            u8   px2 = SX + x2 * ( WIDTH + MARGIN_X );
            u8   py  = SY + y * ( HEIGHT + MARGIN_Y );

            oam[ 0 ].x = px - 2 + sml;
            oam[ 0 ].y = py - 1 + sml;
            oam[ 1 ].x = px - 2 + sml;
            oam[ 1 ].y = py + HEIGHT - 1 - sml;
            oam[ 2 ].x = px2 + WIDTH - 5 - sml;
            oam[ 2 ].y = py + HEIGHT - 1 - sml;
            oam[ 3 ].x = px2 + WIDTH - 5 - sml;
            oam[ 3 ].y = py - 1 + sml;
        }

        for( u8 i = 0; i < maxPages( ) - pageStart( ); ++i ) {
            if( p_idx == 240 + i ) {
                auto x  = i;
                auto y  = 5;
                u8   px = SX + x * ( WIDTH + MARGIN_X );
                u8   py = SY + y * ( HEIGHT + MARGIN_Y );

                oam[ 0 ].x = px - 2 + sml;
                oam[ 0 ].y = py - 1 + sml;
                oam[ 1 ].x = px - 2 + sml;
                oam[ 1 ].y = py + HEIGHT - 1 - sml;
                oam[ 2 ].x = px + WIDTH - 5 - sml;
                oam[ 2 ].y = py + HEIGHT - 1 - sml;
                oam[ 3 ].x = px + WIDTH - 5 - sml;
                oam[ 3 ].y = py - 1 + sml;
            }
        }

        updateOAM( true );
    }

    void keyboard::init( ) const {
        FS::readPictureData( bgGetGfxPtr( IO::bg3sub ), "nitro:/PICS/", "keyboard", 240 * 2,
                             256 * 192, true );

        BG_PALETTE_SUB[ IO::WHITE_IDX ] = RGB15( 27, 30, 30 );
        BG_PALETTE_SUB[ IO::GRAY_IDX ]  = RGB15( 12, 19, 20 );
        BG_PALETTE_SUB[ IO::BLACK_IDX ] = IO::WHITE;
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( IO::BLACK_IDX, 2 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::boldFont->setColor( IO::BLACK_IDX, 4 );
        IO::boldFont->setColor( IO::GRAY_IDX, 3 );
        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->setColor( IO::WHITE_IDX, 3 );
        IO::regularFont->setColor( IO::GRAY_IDX, 4 );
        IO::smallFont->setColor( 0, 0 );
        IO::smallFont->setColor( IO::WHITE_IDX, 1 );
        IO::smallFont->setColor( 0, 2 );

        initOAMTable( true );

        u8 py = SY + 5 * ( HEIGHT + MARGIN_Y ) + 1;

        IO::loadSprite( 0, 0, 0, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, false, false, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( 1, 0, 1, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, true, false, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( 2, 0, 2, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, true, true, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( 3, 0, 3, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, false, true, true,
                        OBJPRIORITY_0, true );

        u8   pb   = pageStart( );
        bool kana = CURRENT_LANGUAGE == SAVE::JAP;
        auto tabX = [ & ]( u8 p_page ) {
            return u8( SX - 1 + ( p_page > pb ? p_page - pb : 0 ) * ( WIDTH + MARGIN_X ) );
        };
        auto hidden = [ & ]( u8 p_page ) { return !( pb <= p_page && p_page < maxPages( ) ); };
        IO::loadUIIcon( IO::ICON::PG0_START, 4, 1, 4, tabX( 0 ), py, 16, 32, false, false,
                        hidden( 0 ), OBJPRIORITY_0, true );
        IO::loadUIIcon( IO::ICON::PG1_START, 5, 2, 12, tabX( kana ? 5 : 1 ), py, 16, 32, false,
                        false, kana ? false : hidden( 1 ), OBJPRIORITY_0, true );
        IO::loadUIIcon( IO::ICON::PG2_START, 6, 3, 20, tabX( 2 ), py, 16, 32, false, false,
                        hidden( 2 ), OBJPRIORITY_0, true );
        IO::loadUIIcon( IO::ICON::PG3_START, 7, 4, 28, tabX( 3 ), py, 16, 32, false, false,
                        hidden( 3 ), OBJPRIORITY_0, true );
        IO::loadUIIcon( IO::ICON::PG4_START, 8, 5, 36, tabX( 4 ), py, 16, 32, false, false,
                        hidden( 4 ), OBJPRIORITY_0, true );
        updateOAM( true );

        drawPage( );
    }

    u8 keyboard::maxPages( ) const {
        return CURRENT_LANGUAGE == SAVE::JAP ? u8( MAX_KEYBOARD_PAGES ) : u8( 2 );
    }

    u8 keyboard::pageStart( ) const {
        return CURRENT_LANGUAGE == SAVE::JAP ? u8( 2 ) : u8( 0 );
    }

    void keyboard::drawPage( ) const {
        s8 shx = _page >= 2 ? 4 : 0;
        s8 shy = 0;
        IO::boldFont->setColor( 0, 3 );
        IO::boldFont->setColor( 0, 1 );

        for( u8 y = 0; y < NUM_ROWS; ++y ) {
            for( u8 x = 0; x < CHARS_PER_ROW; ++x ) {
                u16 entry = PAGES[ _page ][ y * CHARS_PER_ROW + x ];
                if( entry == KBD_DAKUTEN || entry == KBD_HANDAKUTEN ) { continue; }

                u8 px = SX + x * ( WIDTH + MARGIN_X ) - shx;
                u8 py = SY + y * ( HEIGHT + MARGIN_Y ) - shy;

                IO::printRectangle( px, py, px + WIDTH + shx, py + HEIGHT + 2, true, 0 );

                u16 glyph = entry;
                if( _page <= 1 && CURRENT_LANGUAGE == SAVE::JAP ) {
                    if( entry == '!' || entry == '?' || entry == '.' || entry == ',' ) {
                        glyph = KANA::digraphToGlyphOrderIndex( 'x', (char) entry );
                    }
                }

                s16 gx = s16( px + 2 + ( glyph >= 5160 ? 3 : glyph >= 5000 ? 2 : 0 ) );
                IO::boldFont->printChar( PAGES[ _page ][ y * CHARS_PER_ROW + x ], gx, py + 2,
                                         true );
            }
        }

        drawMarkKeys( );

        IO::boldFont->setColor( IO::GRAY_IDX, 3 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::smallFont->printString(
            GET_STRING_L( IO::STR_UI_KEY_BACKSPACE,
                          CURRENT_LANGUAGE == SAVE::JAP ? SAVE::EN : CURRENT_LANGUAGE ),
            136, 158, true, IO::font::CENTER );
        IO::smallFont->printString(
            GET_STRING_L( IO::STR_UI_KEY_ENTER,
                          CURRENT_LANGUAGE == SAVE::JAP ? SAVE::EN : CURRENT_LANGUAGE ),
            195, 158, true, IO::font::CENTER );
    }

    void keyboard::drawMarkKeys( ) const {
        IO::boldFont->setColor( 0, 3 );
        IO::boldFont->setColor( 0, 1 );
        for( u8 y = 0; y < NUM_ROWS; ++y ) {
            for( u8 x = 0; x < CHARS_PER_ROW; ++x ) {
                u16 entry = PAGES[ _page ][ y * CHARS_PER_ROW + x ];
                if( entry != KBD_DAKUTEN && entry != KBD_HANDAKUTEN ) { continue; }

                bool half = ( entry == KBD_HANDAKUTEN );
                u8   px   = SX + x * ( WIDTH + MARGIN_X );
                u8   py   = SY + y * ( HEIGHT + MARGIN_Y );
                IO::printRectangle( px, py, px + WIDTH, py + HEIGHT + 2, true, 0 );

                if( !( _lastKanaC1 && KANA::hasVariant( _lastKanaC1, _lastKanaC2, half ) ) ) {
                    continue;
                }

                IO::boldFont->printChar( half ? CHAR_DAKUTEN_COMP : CHAR_HANDAKUTEN_COMP, px + 1,
                                         py + 1, true, 1, false );
            }
        }
        IO::boldFont->setColor( IO::GRAY_IDX, 3 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
    }

    void keyboard::drawChar( u8 p_pos, u16 p_char ) {
        clearChar( p_pos );
        s16 x = ( CURRENT_LANGUAGE == SAVE::JAP ) ? (s16) ( 69 + ( p_pos / 2 ) * ( 1 + WIDTH ) )
                                                  : (s16) ( 72 + p_pos * ( 1 + WIDTH ) );
        IO::boldFont->printChar( p_char, x, 10, true );
    }

    void keyboard::clearChar( u8 p_pos ) {
        bool kanaMode = CURRENT_LANGUAGE == SAVE::JAP;
        s16  x0       = kanaMode ? (s16) ( 69 + ( p_pos / 2 ) * ( 1 + WIDTH ) )
                                 : (s16) ( 70 + p_pos * ( 1 + WIDTH ) );
        s16 x1 = kanaMode ? (s16) ( x0 + WIDTH ) : (s16) ( 70 + ( p_pos + 1 ) * ( 1 + WIDTH ) - 1 );
        IO::printRectangle( x0, 10, x1, 15 + 18, true, 0 );

        s16 ux = kanaMode ? (s16) ( x0 + ( WIDTH / 2 - 6 ) ) : x0;

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->printChar( '_', ux, 15, true );
    }

    std::vector<std::pair<inputTarget, u8>> keyboard::getTouchPositions( ) const {
        auto res = std::vector<std::pair<inputTarget, u8>>( );

        for( u8 y = 0; y < NUM_ROWS; ++y )
            for( u8 x = 0; x < CHARS_PER_ROW; ++x ) {
                u8 px = SX + x * ( WIDTH + MARGIN_X );
                u8 py = SY + y * ( HEIGHT + MARGIN_Y );
                res.push_back(
                    std::pair( inputTarget( px, py - 1, px + WIDTH + 2, py + HEIGHT - 1 ),
                               u8( y * CHARS_PER_ROW + x ) ) );
            }

        for( u8 i = 0; i < maxPages( ) - pageStart( ); ++i ) {
            u8 px = SX + i * ( WIDTH + MARGIN_X );
            u8 py = SY + 5 * ( HEIGHT + MARGIN_Y );
            res.push_back( std::pair(
                inputTarget( px - 2, py - 1, px + WIDTH - 5, py + HEIGHT - 1 ), u8( 240 + i ) ) );
        }

        {
            u8 px  = SX + 5 * ( WIDTH + MARGIN_X );
            u8 px2 = SX + 9 * ( WIDTH + MARGIN_X );
            u8 py  = SY + 5 * ( HEIGHT + MARGIN_Y );
            res.push_back( std::pair(
                inputTarget( px - 2, py - 1, px2 + WIDTH - 5, py + HEIGHT - 1 ), u8( 253 ) ) );
        }
        {
            u8 px  = SX + 10 * ( WIDTH + MARGIN_X );
            u8 px2 = SX + 13 * ( WIDTH + MARGIN_X );
            u8 py  = SY + 5 * ( HEIGHT + MARGIN_Y );
            res.push_back( std::pair(
                inputTarget( px - 2, py - 1, px2 + WIDTH - 5, py + HEIGHT - 1 ), u8( 254 ) ) );
        }

        return res;
    }

    u16 keyboard::getNextChar( ) {
        auto tick = [ & ]( ) { select( _curSel, ++_frame ); };
        GAME_LOOP( tick ) {
            if( GET_KEY_COOLDOWN( KEY_A ) ) {
                if( _curSel == 253 ) {
                    SOUND::playSoundEffect( SFX_CANCEL );
                    return '\b';
                }
                SOUND::playSoundEffect( SFX_CHOOSE );
                if( _curSel == 254 ) { return '\n'; }
                if( _curSel >= 240 && _curSel < 240 + maxPages( ) - pageStart( ) ) {
                    return pageStart( ) + _curSel - 240;
                }

                return PAGES[ _page ][ _curSel ];
            }

            if( GET_AND_WAIT( KEY_START ) ) { select( _curSel = 254, _frame ); }
            if( GET_AND_WAIT( KEY_SELECT ) )
                return pageStart( ) + ( _page + 1 - pageStart( ) ) % ( maxPages( ) - pageStart( ) );
            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return '\b';
            }

            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel < NUM_ROWS * CHARS_PER_ROW - 1 ) {
                    select( ++_curSel, _frame );
                } else if( _curSel == NUM_ROWS * CHARS_PER_ROW - 1 ) {
                    select( _curSel = 240, _frame );
                } else if( _curSel >= 240 && _curSel < 240 + maxPages( ) - pageStart( ) - 1 ) {
                    select( ++_curSel, _frame );
                } else if( _curSel == 240 + maxPages( ) - pageStart( ) - 1 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = 0, _frame );
                }
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel > 0 && _curSel <= NUM_ROWS * CHARS_PER_ROW - 1 ) {
                    select( --_curSel, _frame );
                } else if( _curSel == 0 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel > 240 && _curSel <= 240 + maxPages( ) - pageStart( ) - 1 ) {
                    select( --_curSel, _frame );
                } else if( _curSel == 240 ) {
                    select( _curSel = NUM_ROWS * CHARS_PER_ROW - 1, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = 240 + maxPages( ) - pageStart( ) - 1, _frame );
                }
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel >= CHARS_PER_ROW && _curSel <= NUM_ROWS * CHARS_PER_ROW - 1 ) {
                    select( _curSel -= CHARS_PER_ROW, _frame );
                } else if( _curSel == 0 ) {
                    select( _curSel = 240, _frame );
                } else if( _curSel == 1 ) {
                    select( _curSel = std::min( 241, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel == 2 ) {
                    select( _curSel = std::min( 242, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel == 3 ) {
                    select( _curSel = std::min( 243, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel == 4 ) {
                    select( _curSel = std::min( 244, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel >= 5 && _curSel < 9 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel >= 9 && _curSel < 13 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel >= 240 && _curSel <= 240 + maxPages( ) - pageStart( ) - 1 ) {
                    select( _curSel = ( NUM_ROWS - 1 ) * CHARS_PER_ROW + _curSel - 240, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 9, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 5, _frame );
                }
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel <= ( NUM_ROWS - 1 ) * CHARS_PER_ROW - 1 ) {
                    select( _curSel += CHARS_PER_ROW, _frame );
                } else if( _curSel == ( NUM_ROWS - 1 ) * CHARS_PER_ROW ) {
                    select( _curSel = 240, _frame );
                } else if( _curSel == ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 1 ) {
                    select( _curSel = std::min( 241, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel == ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 2 ) {
                    select( _curSel = std::min( 242, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel == ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 3 ) {
                    select( _curSel = std::min( 243, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel == ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 4 ) {
                    select( _curSel = std::min( 244, 240 + maxPages( ) - pageStart( ) - 1 ),
                            _frame );
                } else if( _curSel >= ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 5
                           && _curSel < ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 9 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel >= ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 9
                           && _curSel < ( NUM_ROWS - 1 ) * CHARS_PER_ROW + 13 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel >= 240 && _curSel <= 240 + maxPages( ) - pageStart( ) - 1 ) {
                    select( _curSel = _curSel - 240, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = 9, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = 5, _frame );
                }
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            }

            for( auto i : getTouchPositions( ) ) {
                if( i.first.inRange( IO::TOUCH ) ) {
                    swiWaitForVBlank( );
                    bool good = true;
                    while( IO::TOUCH.px || IO::TOUCH.py ) {
                        select( _curSel = i.second, _frame++ );
                        swiWaitForVBlank( );
                        scanKeys( );

                        if( !i.first.inRange( IO::TOUCH ) ) {
                            good = false;
                            break;
                        }
                        touchRead( &IO::TOUCH );
                        swiWaitForVBlank( );
                    }

                    if( good ) {
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        if( _curSel == 254 ) { return '\n'; }
                        if( _curSel == 253 ) { return '\b'; }
                        if( _curSel >= 240 && _curSel < 240 + maxPages( ) - pageStart( ) ) {
                            return pageStart( ) + _curSel - 240;
                        }
                        return PAGES[ _page ][ _curSel ];
                    }
                }
            }
        }
        return '\n';
    }
} // namespace IO
