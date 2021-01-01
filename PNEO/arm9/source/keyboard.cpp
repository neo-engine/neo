/*
Pokémon neo
------------------------------

file        : keyboard.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2021
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

#include <cmath>
#include <string>
#include <vector>
#include <nds.h>

#include "defines.h"
#include "fs.h"
#include "keyboard.h"
#include "nav.h"
#include "sound.h"
#include "uio.h"

#include "key.h"

namespace IO {
    constexpr u8 height = 16, width = 12;
    constexpr u8 sx = 25, sy = 38;
    constexpr u8 marginx     = 4;
    constexpr u8 marginy     = 8;
    constexpr u8 charsPerRow = 13;
    constexpr u8 numRows     = 5;

    constexpr u16 pages[ MAX_KEYBOARD_PAGES ][ numRows * charsPerRow ]
        = { { 'A',    'B', 'C',    'D',    'E',    'F',    'G',    'H', 'I',    'J',    'K',
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
                5001, 5010, 5020, 5030, 5041, 5046, 5061, 5067, 5072, 5077, 5000, 5066, '"',
                5003, 5012, 5022, 5032, 5042, 5049, 5062, ' ',  5073, ' ',  5002, 5068, '°',
                5005, 5014, 5024, 5035, 5043, 5052, 5063, 5069, 5074, 5078, 5004, 5070, '-',
                5007, 5016, 5026, 5037, 5044, 5055, 5064, ' ',  5075, ' ',  5006, ' ',  5224,
                5009, 5018, 5028, 5039, 5045, 5058, 5065, 5071, 5076, 5079, 5008, 5034, 5225,
            },

            {
                5081, 5090, 5100, 5110, 5121, 5126, 5141, 5147, 5152, 5157, 5080, 5146, '"',
                5083, 5092, 5102, 5112, 5122, 5129, 5142, ' ',  5153, ' ',  5082, 5148, '°',
                5085, 5094, 5104, 5115, 5123, 5132, 5143, 5149, 5154, 5158, 5084, 5150, '-',
                5087, 5096, 5106, 5117, 5124, 5135, 5144, ' ',  5155, ' ',  5086, ' ',  5224,
                5089, 5098, 5108, 5119, 5125, 5138, 5145, 5151, 5156, 5159, 5088, 5114, 5225,
            },

            {
                5170, 5171, 5172, 5173, 5174, 5175, 5176, 5177, 5178, 5179, 5180, 5181, 5182,
                5183, 5184, 5185, 5186, 5187, 5188, 5189, 5190, 5191, 5192, 5193, 5194, 5195,
                5196, 5197, 5198, 5199, 5200, 5201, 5202, 5203, 5204, 5205, 5206, 5207, 5208,
                5209, 5210, 5211, 5212, 5213, 5214, 5215, 5216, 5217, 5218, 5219, 5220, 5221,
                5160, 5161, 5162, 5163, 5164, 5165, 5166, 5167, 5168, 5169, '.',  ',',  ' ',
            } };

    std::string guardEmptyString( const std::string& p_string ) {
        for( auto i : p_string )
            if( i != ' ' ) return p_string;
        return "";
    }

    std::string keyboard::getText( u8 p_length ) {
        _page = 0;
        init( );
        for( u8 i = 0; i < p_length; ++i ) clearChar( i );

        std::string res = "";
        u16         c;
        u8          pos = 0;
        loop( ) {
            c = getNextChar( );
            if( c < maxPages( ) ) {
                _page = c;
                drawPage( );
                continue;
            }

            switch( c ) {
            case '\n':
                return guardEmptyString( res );
            case '\b':
                if( pos ) {
                    clearChar( --pos );
                    res.pop_back( );
                }
                break;
            default:
                if( pos < p_length ) {
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

        if( p_idx < charsPerRow * numRows ) {
            auto x  = p_idx % charsPerRow;
            auto y  = p_idx / charsPerRow;
            u8   px = sx + x * ( width + marginx );
            u8   py = sy + y * ( height + marginy );

            oam[ 0 ].x = px - 2 + sml;
            oam[ 0 ].y = py - 1 + sml;
            oam[ 1 ].x = px - 2 + sml;
            oam[ 1 ].y = py + height - 1 - sml;
            oam[ 2 ].x = px + width - 5 - sml;
            oam[ 2 ].y = py + height - 1 - sml;
            oam[ 3 ].x = px + width - 5 - sml;
            oam[ 3 ].y = py - 1 + sml;
        } else if( p_idx == 254 ) { // enter
            auto x   = 9;
            auto x2  = 12;
            auto y   = 5;
            u8   px  = sx + x * ( width + marginx );
            u8   px2 = sx + x2 * ( width + marginx );
            u8   py  = sy + y * ( height + marginy );

            oam[ 0 ].x = px - 2 + sml;
            oam[ 0 ].y = py - 1 + sml;
            oam[ 1 ].x = px - 2 + sml;
            oam[ 1 ].y = py + height - 1 - sml;
            oam[ 2 ].x = px2 + width - 5 - sml;
            oam[ 2 ].y = py + height - 1 - sml;
            oam[ 3 ].x = px2 + width - 5 - sml;
            oam[ 3 ].y = py - 1 + sml;
        } else if( p_idx == 253 ) { // backspace
            auto x   = 5;
            auto x2  = 8;
            auto y   = 5;
            u8   px  = sx + x * ( width + marginx );
            u8   px2 = sx + x2 * ( width + marginx );
            u8   py  = sy + y * ( height + marginy );

            oam[ 0 ].x = px - 2 + sml;
            oam[ 0 ].y = py - 1 + sml;
            oam[ 1 ].x = px - 2 + sml;
            oam[ 1 ].y = py + height - 1 - sml;
            oam[ 2 ].x = px2 + width - 5 - sml;
            oam[ 2 ].y = py + height - 1 - sml;
            oam[ 3 ].x = px2 + width - 5 - sml;
            oam[ 3 ].y = py - 1 + sml;
        }

        for( u8 i = 0; i < maxPages( ); ++i ) {
            if( p_idx == 240 + i ) {
                auto x  = i;
                auto y  = 5;
                u8   px = sx + x * ( width + marginx );
                u8   py = sy + y * ( height + marginy );

                oam[ 0 ].x = px - 2 + sml;
                oam[ 0 ].y = py - 1 + sml;
                oam[ 1 ].x = px - 2 + sml;
                oam[ 1 ].y = py + height - 1 - sml;
                oam[ 2 ].x = px + width - 5 - sml;
                oam[ 2 ].y = py + height - 1 - sml;
                oam[ 3 ].x = px + width - 5 - sml;
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

        u8 py = sy + 5 * ( height + marginy ) + 1;

        IO::loadSprite( 0, 0, 0, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, false, false, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( 1, 0, 1, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, true, false, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( 2, 0, 2, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, true, true, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( 3, 0, 3, 0, 0, 8, 8, keyPal, keyTiles, keyTilesLen, false, true, true,
                        OBJPRIORITY_0, true );
        IO::loadSprite( "KB/pg0", 4, 1, 4, sx - 1 + 0 * ( width + marginx ), py, 16, 32, false,
                        false, false, OBJPRIORITY_0, true );
        IO::loadSprite( "KB/pg1", 5, 2, 12, sx - 1 + 1 * ( width + marginx ), py, 16, 32, false,
                        false, false, OBJPRIORITY_0, true );
        IO::loadSprite( "KB/pg2", 6, 3, 20, sx - 1 + 2 * ( width + marginx ), py, 16, 32, false,
                        false, maxPages( ) <= 2, OBJPRIORITY_0, true );
        IO::loadSprite( "KB/pg3", 7, 4, 28, sx - 1 + 3 * ( width + marginx ), py, 16, 32, false,
                        false, maxPages( ) <= 3, OBJPRIORITY_0, true );
        IO::loadSprite( "KB/pg4", 8, 5, 36, sx - 1 + 4 * ( width + marginx ), py, 16, 32, false,
                        false, maxPages( ) <= 4, OBJPRIORITY_0, true );

        updateOAM( true );

        drawPage( );
    }

    u8 keyboard::maxPages( ) const {
        switch( CURRENT_LANGUAGE ) {
        default:
            return 2;
        }
    }

    void keyboard::drawPage( ) const {

        s8 shx = ( _page >= 2 && _page <= 3 ) * 2;
        if( _page == 4 ) shx++;
        s8 shy = 0;
        IO::boldFont->setColor( 0, 3 );
        IO::boldFont->setColor( 0, 1 );

        for( u8 y = 0; y < numRows; ++y )
            for( u8 x = 0; x < charsPerRow; ++x ) {
                u8 px = sx + x * ( width + marginx ) - shx;
                u8 py = sy + y * ( height + marginy ) - shy;

                IO::printRectangle( px, py, px + width + shx, py + height + 2, true, 0 );
                IO::boldFont->printChar( pages[ _page ][ y * charsPerRow + x ], px + 2, py + 2,
                                         true );
            }

        IO::boldFont->setColor( IO::GRAY_IDX, 3 );
        IO::boldFont->setColor( IO::GRAY_IDX, 1 );
        IO::smallFont->printString( GET_STRING( 439 ), 136, 158, true, IO::font::CENTER );
        IO::smallFont->printString( GET_STRING( 440 ), 195, 158, true, IO::font::CENTER );
    }

    void keyboard::drawChar( u8 p_pos, u16 p_char ) {
        IO::boldFont->printChar( p_char, 72 + p_pos * width, 10, true );
    }
    void keyboard::clearChar( u8 p_pos ) {
        IO::printRectangle( 70 + p_pos * width, 10, 70 + ( p_pos + 1 ) * width - 1, 15 + 18, true,
                            0 );
        IO::regularFont->printChar( '_', 70 + p_pos * width, 15, true );
    }

    std::vector<std::pair<inputTarget, u8>> keyboard::getTouchPositions( ) const {
        auto res = std::vector<std::pair<inputTarget, u8>>( );

        for( u8 y = 0; y < numRows; ++y )
            for( u8 x = 0; x < charsPerRow; ++x ) {
                u8 px = sx + x * ( width + marginx );
                u8 py = sy + y * ( height + marginy );
                res.push_back(
                    std::pair( inputTarget( px - 2, py - 1, px + width - 5, py + height - 1 ),
                               u8( y * charsPerRow + x ) ) );
            }

        for( u8 i = 0; i < maxPages( ); ++i ) {
            u8 px = sx + i * ( width + marginx );
            u8 py = sy + 5 * ( height + marginy );
            res.push_back( std::pair(
                inputTarget( px - 2, py - 1, px + width - 5, py + height - 1 ), u8( 240 + i ) ) );
        }

        {
            u8 px  = sx + 5 * ( width + marginx );
            u8 px2 = sx + 9 * ( width + marginx );
            u8 py  = sy + 5 * ( height + marginy );
            res.push_back( std::pair(
                inputTarget( px - 2, py - 1, px2 + width - 5, py + height - 1 ), u8( 253 ) ) );
        }
        {
            u8 px  = sx + 10 * ( width + marginx );
            u8 px2 = sx + 13 * ( width + marginx );
            u8 py  = sy + 5 * ( height + marginy );
            res.push_back( std::pair(
                inputTarget( px - 2, py - 1, px2 + width - 5, py + height - 1 ), u8( 254 ) ) );
        }

        return res;
    }

    u16 keyboard::getNextChar( ) {
        touchPosition touch;
        int           pressed;
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            select( _curSel, ++_frame );
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            pressed = keysUp( );
            held    = keysHeld( );

            if( GET_KEY_COOLDOWN( KEY_A ) ) {
                if( _curSel == 253 ) {
                    SOUND::playSoundEffect( SFX_CANCEL );
                    return '\b';
                }
                SOUND::playSoundEffect( SFX_CHOOSE );
                if( _curSel == 254 ) { return '\n'; }
                if( _curSel >= 240 && _curSel < 240 + maxPages( ) ) { return _curSel - 240; }

                return pages[ _page ][ _curSel ];
            }

            if( GET_AND_WAIT( KEY_START ) ) { select( _curSel = 254, _frame ); }
            if( GET_AND_WAIT( KEY_SELECT ) ) return ( _page + 1 ) % maxPages( );
            if( GET_KEY_COOLDOWN( KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return '\b';
            }

            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel < numRows * charsPerRow - 1 ) {
                    select( ++_curSel, _frame );
                } else if( _curSel == numRows * charsPerRow - 1 ) {
                    select( _curSel = 240, _frame );
                } else if( _curSel >= 240 && _curSel < 240 + maxPages( ) - 1 ) {
                    select( ++_curSel, _frame );
                } else if( _curSel == 240 + maxPages( ) - 1 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = 0, _frame );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel > 0 && _curSel <= numRows * charsPerRow - 1 ) {
                    select( --_curSel, _frame );
                } else if( _curSel == 0 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel > 240 && _curSel <= 240 + maxPages( ) - 1 ) {
                    select( --_curSel, _frame );
                } else if( _curSel == 240 ) {
                    select( _curSel = numRows * charsPerRow - 1, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = 240 + maxPages( ) - 1, _frame );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel >= charsPerRow && _curSel <= numRows * charsPerRow - 1 ) {
                    select( _curSel -= charsPerRow, _frame );
                } else if( _curSel == 0 ) {
                    select( _curSel = 240, _frame );
                } else if( _curSel == 1 ) {
                    select( _curSel = std::min( 241, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel == 2 ) {
                    select( _curSel = std::min( 242, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel == 3 ) {
                    select( _curSel = std::min( 243, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel == 4 ) {
                    select( _curSel = std::min( 244, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel >= 5 && _curSel < 9 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel >= 9 && _curSel < 13 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel >= 240 && _curSel <= 240 + maxPages( ) - 1 ) {
                    select( _curSel = ( numRows - 1 ) * charsPerRow + _curSel - 240, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = ( numRows - 1 ) * charsPerRow + 9, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = ( numRows - 1 ) * charsPerRow + 5, _frame );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _curSel <= ( numRows - 1 ) * charsPerRow - 1 ) {
                    select( _curSel += charsPerRow, _frame );
                } else if( _curSel == ( numRows - 1 ) * charsPerRow ) {
                    select( _curSel = 240, _frame );
                } else if( _curSel == ( numRows - 1 ) * charsPerRow + 1 ) {
                    select( _curSel = std::min( 241, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel == ( numRows - 1 ) * charsPerRow + 2 ) {
                    select( _curSel = std::min( 242, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel == ( numRows - 1 ) * charsPerRow + 3 ) {
                    select( _curSel = std::min( 243, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel == ( numRows - 1 ) * charsPerRow + 4 ) {
                    select( _curSel = std::min( 244, 240 + maxPages( ) - 1 ), _frame );
                } else if( _curSel >= ( numRows - 1 ) * charsPerRow + 5
                           && _curSel < ( numRows - 1 ) * charsPerRow + 9 ) {
                    select( _curSel = 253, _frame );
                } else if( _curSel >= ( numRows - 1 ) * charsPerRow + 9
                           && _curSel < ( numRows - 1 ) * charsPerRow + 13 ) {
                    select( _curSel = 254, _frame );
                } else if( _curSel >= 240 && _curSel <= 240 + maxPages( ) - 1 ) {
                    select( _curSel = _curSel - 240, _frame );
                } else if( _curSel == 254 ) {
                    select( _curSel = 9, _frame );
                } else if( _curSel == 253 ) {
                    select( _curSel = 5, _frame );
                }
                cooldown = COOLDOWN_COUNT;
            }

            for( auto i : getTouchPositions( ) ) {
                if( i.first.inRange( touch ) ) {
                    swiWaitForVBlank( );
                    bool good = true;
                    while( touch.px || touch.py ) {
                        select( _curSel = i.second, _frame++ );
                        swiWaitForVBlank( );
                        scanKeys( );

                        if( !i.first.inRange( touch ) ) {
                            good = false;
                            break;
                        }
                        touchRead( &touch );
                        swiWaitForVBlank( );
                    }

                    if( good ) {
                        SOUND::playSoundEffect( SFX_CHOOSE );
                        if( _curSel == 254 ) { return '\n'; }
                        if( _curSel == 253 ) { return '\b'; }
                        if( _curSel >= 240 && _curSel < 240 + maxPages( ) ) {
                            return _curSel - 240;
                        }
                        return pages[ _page ][ _curSel ];
                    }
                }
            }
            swiWaitForVBlank( );
        }
        return '\n';
    }
} // namespace IO
