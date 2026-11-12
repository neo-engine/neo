/*
    Pokémon neo
    ------------------------------

    file        : voltorbFlipUI.cpp
    author      : Philip Wellnitz
    description : Voltorb Flip mini game, ported from HGSS.

    Copyright (C) 2026
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

#include "spx/voltorbFlipUI.h"

#include <nds.h>

#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "fs/data.h"
#include "fs/fs.h"
#include "io/fontUtil.h"
#include "io/simpleWidget.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/util.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/voltorbFlip.h"
#include "spx/voltorbFlipData.h"
#include "spx/voltorbFlipUI.h"

namespace SPX::VF {
    constexpr u8 VF_TOP_LAYER   = 1;
    constexpr u8 VF_BOARD_LAYER = 5;

    constexpr u8  VF_QUIT_X                     = 192;
    constexpr u8  VF_QUIT_Y                     = 160;
    constexpr u8  VF_QUIT_W                     = 64;
    constexpr u8  VF_QUIT_H                     = 32;
    constexpr u8  VF_MEMO_X                     = 192;
    constexpr u8  VF_MEMO_Y                     = 8;
    constexpr u8  VF_MEMO_W                     = 64;
    constexpr u8  VF_MEMO_H                     = 64;
    constexpr u8  VF_MEMO_LBL_X                 = VF_MEMO_X + 8;
    constexpr u8  VF_MEMO_LBL_Y                 = VF_MEMO_Y + 20;
    constexpr u8  VF_MEMO_LBL_W                 = 48;
    constexpr u8  VF_MEMO_LBL_H                 = 32;
    constexpr u8  VF_PANEL_X[ 4 ]               = { 200 - 4, 224 - 4, 200 - 4, 224 - 4 };
    constexpr u8  VF_PANEL_Y[ 4 ]               = { 80, 80, 104, 104 };
    constexpr u8  VF_PANEL_MEMO[ 4 ]            = { MEMO_VOLTORB, MEMO_ONE, MEMO_TWO, MEMO_THREE };
    constexpr u8  VF_EXPL_FRAMES                = 6;
    constexpr u8  VF_EXPL_PX                    = 64;
    constexpr u8  VF_EXPL_DUR[ VF_EXPL_FRAMES ] = { 4, 4, 6, 6, 6, 4 };
    constexpr int VF_SEL_BORD                   = 4;

    constexpr u8  BAR_X      = 29;
    constexpr u8  BAR_Y      = 80;
    constexpr u8  BAR_PARTS  = 13;
    constexpr int BAR_FLIP_X = BAR_X + ( BAR_PARTS - 1 ) * 16 - 9;
    constexpr int BAR_RIGHT  = BAR_FLIP_X + 16 - 1;

    constexpr s16 YN_YES_X = 29;
    constexpr s16 YN_NO_X  = 131;
    constexpr s16 YN_YES_Y = 116;
    constexpr s16 YN_NO_Y  = 116;
    constexpr int VF_YN_W  = 96;
    constexpr int VF_YN_H  = 32;

    // top
    constexpr u8 VF_COIN_OAM( u8 p_slot ) {
        return (u8) p_slot;
    }

    constexpr u8 VF_COIN_PAL = 3;

    constexpr u16 VF_COIN_TILE = 0;

    // sub
    constexpr u8 VF_BAR_OAM( u8 p_part ) {
        return (u8) p_part;
    }
    constexpr u8 VF_YN_OAM( u8 p_cell, u8 p_part ) {
        return (u8) ( 13 + 6 * p_cell + p_part );
    }
    constexpr u8 VF_EXPL_OAM = 25;
    constexpr u8 VF_QUIT_OAM = 26;
    constexpr u8 VF_SEL_OAM  = 27;
    constexpr u8 VF_MEMO_OAM = 38;
    constexpr u8 VF_PANEL_OAM( u8 p_i ) {
        return (u8) ( 39 + p_i );
    }
    constexpr u8 VF_MEMOCUR_OAM = 43;

    constexpr u8 VF_EXPL_PAL     = 0;
    constexpr u8 VF_SEL_PAL      = 2;
    constexpr u8 VF_BTN_REST_PAL = 2;
    constexpr u8 VF_BTN_PUSH_PAL = 3;
    constexpr u8 VF_BOX_PAL      = 5;
    constexpr u8 VF_BOXSEL_PAL   = 6;
    constexpr u8 VF_PANEL_PAL    = 8;

    constexpr u16 VF_EXPL_TILE         = 0;
    constexpr u16 VF_VOLT_GLYPH_TILES  = 8 * 16 / 64;
    constexpr u16 VF_COIN_DIGIT_TILES  = 16 * 32 / 64;
    constexpr u16 VF_PANEL_FRAME_TILES = 16;
    constexpr u16 VF_SEL_TILE          = 388;
    constexpr u16 VF_BOX_CORNER_TILE   = 412;
    constexpr u16 VF_BOX_EDGE_TILE     = 420;
    constexpr u16 VF_PANEL_TILE        = 512;
    constexpr u16 VF_QUIT_TILE         = 640;
    constexpr u16 VF_MEMO_TILE         = 672;

    u8 SELECTED_COL = 0;
    u8 SELECTED_ROW = 0;

    int memoButtonAt( int p_x, int p_y ) {
        for( u8 i = 0; i < 4; ++i ) {
            if( p_x >= VF_PANEL_X[ i ] && p_x < VF_PANEL_X[ i ] + 24 && p_y >= VF_PANEL_Y[ i ]
                && p_y < VF_PANEL_Y[ i ] + 24 ) {
                return i;
            }
        }
        return -1;
    }
    u8 memoButtonMemo( int p_i ) {
        return p_i < 4 ? VF_PANEL_MEMO[ p_i ] : u8( MEMO_NONE );
    }

    int cellAt( int p_px, int p_py ) {
        int x = p_px - BOARD_X0;
        int y = p_py - BOARD_Y0;
        if( x < 0 || y < 0 ) { return -1; }
        int col = x / BOARD_STRIDE;
        int row = y / BOARD_STRIDE;
        if( col >= NUM_COLS || row >= NUM_ROWS ) { return -1; }
        if( x % BOARD_STRIDE >= CARD_PX || y % BOARD_STRIDE >= CARD_PX ) { return -1; }
        return row * NUM_COLS + col;
    }

    bool inQuitRect( int p_x, int p_y ) {
        return ( p_x >= 192 && p_x < 256 && p_y >= 160 && p_y < 192 );
    }

    bool inMemoRect( int p_x, int p_y ) {
        return ( p_x >= 192 && p_x < 256 && p_y >= 8 && p_y < 72 );
    }

    void snapshotRect( u8* p_dst, bool p_bottom, int p_x, int p_y, int p_w, int p_h ) {
        u8* gfx = (u8*) IO::BG_BMP( p_bottom, p_bottom ? VF_BOARD_LAYER : VF_TOP_LAYER );
        for( int y = 0; y < p_h; ++y ) {
            memcpy( p_dst + y * p_w, gfx + ( p_y + y ) * 256 + p_x, p_w );
        }
    }

    void restoreRect( const u8* p_src, bool p_bottom, int p_x, int p_y, int p_w, int p_h ) {
        u8 layer = p_bottom ? VF_BOARD_LAYER : VF_TOP_LAYER;
        for( int y = 0; y < p_h; ++y ) {
            for( int x = 0; x < p_w; ++x ) {
                IO::setPixel( (u8) ( p_x + x ), (u8) ( p_y + y ), p_bottom, p_src[ y * p_w + x ],
                              layer );
            }
        }
    }

    void clearTextRect( int p_x, int p_y, int p_w, int p_h ) {
        u8* bg = (u8*) IO::BG_BMP( true, 1 );
        for( int y = p_y; y < p_y + p_h; y++ ) { memset( &bg[ y * 256 + p_x ], 0, p_w ); }
    }

    void showSprites( int p_first, int p_count, bool p_visible, bool p_bottom = true ) {
        SpriteEntry* oam = p_bottom ? IO::Oam->oamBuffer : IO::OamTop->oamBuffer;
        for( int i = p_first; i < p_first + p_count; i++ ) {
            if( p_visible ) {
                oam[ i ].attribute[ 0 ] &= ~ATTR0_DISABLED;
            } else {
                oam[ i ].attribute[ 0 ] |= ATTR0_DISABLED;
            }
            oam[ i ].isHidden = !p_visible;
        }
        IO::updateOAM( p_bottom );
    }

    void quitArrow( bool p_visible ) {
        clearTextRect( VF_MEMO_LBL_X, VF_MEMO_LBL_Y, VF_MEMO_LBL_W, VF_MEMO_LBL_H );
        clearTextRect( VF_QUIT_X + 2, VF_QUIT_Y + 3, VF_QUIT_W - 4, VF_QUIT_H - 6 );
        if( p_visible ) {
            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            IO::regularFont->printStringC( GET_STRING( IO::STR_VF_OPEN_MEMO ),
                                           VF_MEMO_X + VF_MEMO_W / 2, VF_MEMO_Y + 20, true,
                                           IO::font::CENTER, 15, 0, false, 1 );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            IO::regularFont->printStringC( GET_STRING( IO::STR_VF_QUIT ), VF_QUIT_X + VF_QUIT_W / 2,
                                           VF_QUIT_Y + 8, true, IO::font::CENTER, 15, 0, false, 1 );
        }
        for( int id : { VF_QUIT_OAM, VF_MEMO_OAM } ) { showSprites( id, 1, p_visible ); }
    }

    void voltorbFlipUI::_drawMessageWindow( const char* p_message, bool p_continue ) {
        showSprites( VF_BAR_OAM( 0 ), BAR_PARTS, true );
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        bool twoline = false;
        for( auto i = 0; p_message[ i ]; ++i ) {
            if( p_message[ i ] == '\n' ) {
                twoline = true;
                break;
            }
        }
        if( twoline ) {
            IO::regularFont->printBreakingStringC( p_message, 128, BAR_Y + 1, 240, true,
                                                   IO::font::CENTER, 14 );
        } else {
            IO::regularFont->printBreakingStringC( p_message, 128, BAR_Y + 8, 240, true,
                                                   IO::font::CENTER, 14 );
        }

        if( p_continue ) { IO::regularFont->drawContinue( BAR_RIGHT - 10, BAR_Y + 18, true, 1 ); }
        quitArrow( false );
        _hideSelection( );
    }

    void voltorbFlipUI::_hideMessageWindow( ) {
        showSprites( VF_BAR_OAM( 0 ), BAR_PARTS, false );
        memset( (void*) BG_BMP_RAM_SUB( 1 ), 0, 256 * 192 );
        quitArrow( true );
        moveSelection( SELECTED_COL, SELECTED_ROW );
    }

    void voltorbFlipUI::_loadBoard( ) {
        FS::readPictureData( IO::BG_BMP( true, VF_BOARD_LAYER ), "nitro:/PICS/", "voltorb_board",
                             BOARD_PAL_BYTES, 256 * 192, true );
        u16* pal   = IO::BG_PAL( true );
        pal[ 255 ] = pal[ 0 ];
    }

    void voltorbFlipUI::_loadTopScreen( ) {
        FS::readPictureData( IO::BG_BMP( false ), "nitro:/PICS/", "voltorb_top", TOP_PAL_BYTES,
                             256 * 192, false );
        u16* pal   = IO::BG_PAL( false );
        pal[ 255 ] = pal[ 0 ];
    }

    void voltorbFlipUI::_loadWindow( ) {
        SpriteEntry* oam = IO::Oam->oamBuffer;
        IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_1_START, VF_BAR_OAM( 0 ), VF_BOX_PAL,
                        VF_BOX_CORNER_TILE, BAR_X, BAR_Y, 16, 32, false, false, true, OBJPRIORITY_3,
                        true, OBJMODE_NORMAL );
        IO::loadUIIcon( IO::ICON::NOSELECTION_96_32_2_START, VF_BAR_OAM( 1 ), VF_BOX_PAL,
                        VF_BOX_EDGE_TILE, BAR_X + 16, BAR_Y, 16, 32, false, false, true,
                        OBJPRIORITY_3, true, OBJMODE_NORMAL );
        u16 corner = oam[ VF_BAR_OAM( 0 ) ].gfxIndex;
        u16 edge   = oam[ VF_BAR_OAM( 1 ) ].gfxIndex;
        for( u8 j = 2; j < BAR_PARTS - 1; ++j ) {
            IO::loadSprite( VF_BAR_OAM( j ), VF_BOX_PAL, edge, BAR_X + j * 16, BAR_Y, 16, 32, 0, 0,
                            0, false, false, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
        }
        IO::loadSprite( VF_BAR_OAM( BAR_PARTS - 1 ), VF_BOX_PAL, corner, BAR_FLIP_X, BAR_Y, 16, 32,
                        0, 0, 0, true, true, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
        for( u8 cell = 0; cell < 2; ++cell ) {
            s16 y = cell ? YN_NO_Y : YN_YES_Y;
            s16 x = cell ? YN_NO_X : YN_YES_X;
            for( int j = 0; j < 5; j++ ) {
                IO::loadSprite( VF_YN_OAM( cell, j ), VF_BOX_PAL, j ? edge : corner, x + j * 16, y,
                                16, 32, 0, 0, 0, false, false, true, OBJPRIORITY_3, true,
                                OBJMODE_NORMAL );
            }
            IO::loadSprite( VF_YN_OAM( cell, 5 ), VF_BOX_PAL, corner, x + 5 * 16, y, 16, 32, 0, 0,
                            0, true, true, true, OBJPRIORITY_3, true, OBJMODE_NORMAL );
        }
        IO::copySpritePal( IO::SELECTED_SPR_PAL, VF_BOXSEL_PAL, 0, sizeof( IO::SELECTED_SPR_PAL ),
                           true );
        IO::updateOAM( true );
    }

    void voltorbFlipUI::_loadExplosion( ) {
        FILE* f = FS::open( "nitro:/PICS/", "voltorb_expl", ".rsd" );
        if( !f ) { return; }
        IO::loadAnimatedSprite( f, 0, 0, VF_EXPL_OAM, VF_EXPL_PAL, VF_EXPL_TILE, OBJPRIORITY_1,
                                true );
        showSprites( VF_EXPL_OAM, 1, false );
    }

    void voltorbFlipUI::_loadCoins( ) {
        FILE* f = FS::open( "nitro:/PICS/", "voltorb_coins", ".rsd" );
        if( !f ) { return; }
        IO::loadAnimatedSprite( f, COIN_X, TOTAL_BAND_Y, VF_COIN_OAM( 0 ), VF_COIN_PAL,
                                VF_COIN_TILE, OBJPRIORITY_1, false );
        for( u8 i = 1; i < 10; ++i ) {
            IO::OamTop->oamBuffer[ VF_COIN_OAM( i ) ] = IO::OamTop->oamBuffer[ VF_COIN_OAM( 0 ) ];
        }
        showSprites( VF_COIN_OAM( 0 ), 10, false, false );
    }

    void voltorbFlipUI::_loadMemoButtons( ) {
        FILE* f = FS::open( "nitro:/PICS/", "voltorb_memo", ".rsd" );
        if( !f ) { return; }
        IO::loadAnimatedSprite( f, VF_PANEL_X[ 0 ], VF_PANEL_Y[ 0 ], VF_PANEL_OAM( 0 ),
                                VF_PANEL_PAL, VF_PANEL_TILE, OBJPRIORITY_3, true );
        for( u8 i = 1; i < 4; ++i ) {
            IO::Oam->oamBuffer[ VF_PANEL_OAM( i ) ] = IO::Oam->oamBuffer[ VF_PANEL_OAM( 0 ) ];
        }
        showSprites( VF_PANEL_OAM( 0 ), 4, false );
    }

    void voltorbFlipUI::_loadQuitButton( ) {
        FILE* f = FS::open( "nitro:/PICS/", "voltorb_qbtn", ".rsd" );
        if( !f ) { return; }
        IO::loadAnimatedSprite( f, VF_QUIT_X, VF_QUIT_Y, VF_QUIT_OAM, VF_BTN_PUSH_PAL, VF_QUIT_TILE,
                                OBJPRIORITY_3, true );
        IO::Oam->oamBuffer[ VF_QUIT_OAM ].palette = VF_BTN_REST_PAL;
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->printStringC( GET_STRING( IO::STR_VF_QUIT ), VF_QUIT_X + VF_QUIT_W / 2,
                                       VF_QUIT_Y + 8, true, IO::font::CENTER, 15, 0, false, 1 );
        IO::updateOAM( true );
    }

    void voltorbFlipUI::_loadSelectionBox( ) {
        FILE* f = FS::open( "nitro:/PICS/", "voltorb_select", ".rsd" );
        if( !f ) { return; }
        IO::loadAnimatedSprite( f, BOARD_X0 - VF_SEL_BORD, BOARD_Y0 - VF_SEL_BORD, VF_SEL_OAM,
                                VF_SEL_PAL, VF_SEL_TILE, OBJPRIORITY_3, true );
        showSprites( VF_MEMOCUR_OAM, 1, false );
    }

    void voltorbFlipUI::_drawBlock( bool p_bottom, const u8* p_src, int p_w, int p_h, int p_x,
                                    int p_y ) {
        int x1 = p_x + p_w;
        if( x1 > 256 ) { x1 = 256; }
        int y1 = p_y + p_h;
        if( y1 > 192 ) { y1 = 192; }
        for( int y = ( p_y < 0 ? 0 : p_y ); y < y1; ++y ) {
            const u8* src = p_src + ( y - p_y ) * p_w;
            for( int x = ( p_x < 0 ? 0 : p_x ); x < x1; ++x ) {
                u8 v = src[ x - p_x ];
                if( !v ) { continue; }
                IO::setPixel( (u8) x, (u8) y, p_bottom, v,
                              p_bottom ? VF_BOARD_LAYER : VF_TOP_LAYER );
            }
        }
    }

    void voltorbFlipUI::_drawCard( int p_cardId, const u8* p_block ) {
        _drawBlock( true, p_block, CARD_PX, CARD_PX, BOARD_X0 + ( p_cardId % 5 ) * BOARD_STRIDE,
                    BOARD_Y0 + ( p_cardId / 5 ) * BOARD_STRIDE );
    }

    void voltorbFlipUI::_drawHints( const voltorbFlip& p_board ) {
        for( u8 r = 0; r < NUM_ROWS; ++r ) {
            int points = p_board.pointsPerRow( r );
            _drawBlock( true, DIGIT[ points / 10 ], 8, 8, HINT_ROW_DIGITS_X, ( r * 4 + 1 ) * 8 );
            _drawBlock( true, DIGIT[ points % 10 ], 8, 8, HINT_ROW_DIGITS_X + 8,
                        ( r * 4 + 1 ) * 8 );

            _drawBlock( true, DIGIT[ p_board.voltPerRow( r ) ], 8, 8, HINT_ROW_DIGITS_X + 8,
                        ( r * 4 + 1 ) * 8 + 13 );
        }
        for( u8 c = 0; c < NUM_COLS; ++c ) {
            int points = p_board.pointsPerCol( c );
            _drawBlock( true, DIGIT[ points / 10 ], 8, 8, ( c * 4 + 2 ) * 8, HINT_COL_DIGITS_Y );
            _drawBlock( true, DIGIT[ points % 10 ], 8, 8, ( c * 4 + 2 ) * 8 + 8,
                        HINT_COL_DIGITS_Y );

            _drawBlock( true, DIGIT[ p_board.voltPerCol( c ) ], 8, 8, ( c * 4 + 2 ) * 8 + 8,
                        HINT_COL_DIGITS_Y + 13 );
        }
    }

    void voltorbFlipUI::_printTopText( const voltorbFlip& p_board ) {
        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        std::array<char, 64> line{ };
        snprintf( line.data( ), line.size( ), "%s%s", GET_STRING( IO::STR_VF_VOLTORB_FLIP ),
                  FS::kanaWrap( p_board.level( ) + 1, CURRENT_LANGUAGE ) );
        IO::regularFont->printStringC( line.data( ), 128, 8, false, IO::font::CENTER );
        IO::regularFont->printStringC( GET_STRING( IO::STR_VF_FLIP_FOR_COINS ), 128, 23, false,
                                       IO::font::CENTER );

        IO::regularFont->printString( GET_STRING( IO::STR_VF_MULT ), 104, 52, false );
        IO::regularFont->printString( GET_STRING( IO::STR_VF_GAME_OVER ), 104, 84, false );

        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        snprintf( line.data( ), line.size( ), GET_STRING( IO::STR_VF_COLLECTED_COINS ),
                  SAVE::CURRENT_FILE->getPlayerName( ) );

        IO::regularFont->printStringC( line.data( ), COIN_X / 2, 112, false, IO::font::CENTER, 16 );
        IO::regularFont->printStringC( GET_STRING( IO::STR_VF_COLLECTED_COINS_THIS_ROUND ),
                                       COIN_X / 2, 154, false, IO::font::CENTER, 16 );

        drawTotal( SAVE::CURRENT_FILE->m_coins );
        drawPayout( p_board.payout( ) );
    }

    void voltorbFlipUI::_hideSelection( ) {
        showSprites( VF_SEL_OAM, 1, false );
        showSprites( VF_MEMOCUR_OAM, 1, false );
    }

    void voltorbFlipUI::_setCounter( int p_bandY, int p_oamBase, u32 p_value ) {
        u32 v = p_value;
        for( int slot = 4; slot >= 0; slot-- ) {
            SpriteEntry* e = &IO::OamTop->oamBuffer[ VF_COIN_OAM( p_oamBase + ( 4 - slot ) ) ];
            e->x           = (s16) ( COIN_X + slot * 16 );
            e->y           = (s16) p_bandY;
            e->gfxIndex    = VF_COIN_TILE + ( v % 10 ) * VF_COIN_DIGIT_TILES;
            v /= 10;
        }
        showSprites( VF_COIN_OAM( p_oamBase ), 5, true, false );
    }

    void voltorbFlipUI::init( const voltorbFlip& p_board ) {
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        _loadBoard( );
        _loadTopScreen( );
        _loadCoins( );
        _loadMemoButtons( );
        _shownTotal  = SAVE::CURRENT_FILE->m_coins;
        _shownPayout = 0;
        _drawHints( p_board );
        _printTopText( p_board );
        _loadExplosion( );
        _loadSelectionBox( );
        _loadQuitButton( );

        for( u8 id = 0; id < NUM_TILES; ++id ) {
            int x = BOARD_X0 + ( id % NUM_COLS ) * BOARD_STRIDE;
            int y = BOARD_Y0 + ( id / NUM_COLS ) * BOARD_STRIDE;
            snapshotRect( _markTags[ id ], true, x, y, CARD_PX, CARD_PX );
        }
        loadMemoButton( false );
        _loadWindow( );
    }

    void voltorbFlipUI::fullRedraw( const voltorbFlip& p_board ) {
        _loadBoard( );
        _loadTopScreen( );
        for( u8 id = 0; id < NUM_TILES; ++id ) {
            const card& c = p_board.cardAt( id );
            if( c.m_flipped && c.m_type >= CARD_ONE && c.m_type <= CARD_VOLTORB ) {
                _drawCard( id, FACE_UP[ c.m_type - CARD_ONE ] );
            }
        }
        _drawHints( p_board );
        _printTopText( p_board );
        for( u8 id = 0; id < NUM_TILES; ++id ) {
            int x = BOARD_X0 + ( id % NUM_COLS ) * BOARD_STRIDE;
            int y = BOARD_Y0 + ( id / NUM_COLS ) * BOARD_STRIDE;
            snapshotRect( _markTags[ id ], true, x, y, CARD_PX, CARD_PX );
            if( !p_board.cardAt( id ).m_flipped && p_board.cardAt( id ).m_memo != MEMO_NONE ) {
                drawCardMark( id, p_board.cardAt( id ).m_memo );
            }
        }
        loadMemoButton( false );
        drawMemoPanel( false, MEMO_NONE );
    }

    void voltorbFlipUI::drawPayout( u32 p_payout ) {
        _shownPayout = p_payout;
        _setCounter( PAYOUT_BAND_Y, 5, p_payout );
    }

    void voltorbFlipUI::drawTotal( u32 p_total ) {
        _shownTotal = p_total;
        _setCounter( TOTAL_BAND_Y, 0, p_total );
    }

    void voltorbFlipUI::countPayoutTo( const char* p_message, u32 p_payout ) {
        _drawMessageWindow( p_message );

        bool down     = true;
        bool complete = false;
        GAME_LOOP( IO::DEFAULT_TICK ) {
            bool isDown = ( IO::TOUCH.px || IO::TOUCH.py
                            || ( IO::BTN_HELD & ( KEY_A | KEY_B | KEY_START ) ) )
                          != 0;
            bool just   = isDown && !down;
            down        = isDown;

            if( _shownPayout >= p_payout ) {
                if( !complete ) {
                    SOUND::playSoundEffect( SFX_VOLTORB_PAYOUT );
                    complete = true;
                }
                if( complete && just ) { break; }
                continue;
            }
            _shownPayout = just ? p_payout : _shownPayout + 1;
            _setCounter( PAYOUT_BAND_Y, 5, _shownPayout );
            if( ( _shownPayout % 4 ) == 0 && !complete ) {
                SOUND::playSoundEffect( SFX_VOLTORB_PAYOUT );
            }
        }
        _hideMessageWindow( );
    }

    void voltorbFlipUI::transferToTotal( const char* p_awardMessage, u32 p_payout ) {
        _drawMessageWindow( p_awardMessage );

        _shownPayout = p_payout;
        bool down    = true;
        GAME_LOOP( IO::DEFAULT_TICK ) {
            bool isDown = ( IO::TOUCH.px || IO::TOUCH.py
                            || ( IO::BTN_HELD & ( KEY_A | KEY_B | KEY_START ) ) )
                          != 0;
            bool just   = isDown && !down;
            down        = isDown;

            if( !_shownPayout ) {
                SOUND::playSoundEffect( SFX_VOLTORB_PAYOUT_END );
                break;
            }
            u32 moved = ( just || _shownTotal >= MAX_PAYOUT ) ? _shownPayout : 1u;
            if( moved == 1u && ( _shownPayout % 4 ) == 0 ) {
                SOUND::playSoundEffect( SFX_VOLTORB_PAYOUT );
            }
            _shownPayout -= moved;
            _shownTotal += moved;
            if( _shownTotal > MAX_PAYOUT ) { _shownTotal = MAX_PAYOUT; }
            _setCounter( TOTAL_BAND_Y, 0, _shownTotal );
            _setCounter( PAYOUT_BAND_Y, 5, _shownPayout );
        }
        _hideMessageWindow( );
    }

    void voltorbFlipUI::animateFlip( int p_cardId, u8 p_type ) {
        SOUND::playSoundEffect( SFX_VOLTORB_FLIP );
        _drawCard( p_cardId, BACK45 );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        _drawCard( p_cardId, EDGE );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        if( p_type >= CARD_ONE && p_type <= CARD_VOLTORB ) {
            _drawCard( p_cardId, FACE45[ p_type - CARD_ONE ] );
        }
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        if( p_type >= CARD_ONE && p_type <= CARD_VOLTORB ) {
            _drawCard( p_cardId, FACE_UP[ p_type - CARD_ONE ] );
        }
    }

    void voltorbFlipUI::explode( int p_cardId ) {
        SOUND::playSoundEffect( SFX_VOLTORB_HIT );
        int cx = BOARD_X0 + ( p_cardId % NUM_COLS ) * BOARD_STRIDE + CARD_PX / 2;
        int cy = BOARD_Y0 + ( p_cardId / NUM_COLS ) * BOARD_STRIDE + CARD_PX / 2;
        int ox = cx - VF_EXPL_PX / 2;
        int oy = cy - VF_EXPL_PX / 2;

        SpriteEntry* e = &IO::Oam->oamBuffer[ VF_EXPL_OAM ];
        e->x           = ox;
        e->y           = oy;
        for( int f = 0; f < VF_EXPL_FRAMES; ++f ) {
            e->gfxIndex = VF_EXPL_TILE + f * ( VF_EXPL_PX * VF_EXPL_PX / 64 );
            showSprites( VF_EXPL_OAM, 1, true );
            for( int i = 0; i < VF_EXPL_DUR[ f ]; i++ ) { swiWaitForVBlank( ); }
        }
        showSprites( VF_EXPL_OAM, 1, false );
        swiWaitForVBlank( );
    }

    void voltorbFlipUI::revealBoard( const voltorbFlip& p_board ) {
        for( int id = 0; id < NUM_TILES; ++id ) {
            u8 type = p_board.cardAt( id ).m_type;
            if( type >= CARD_ONE && type <= CARD_VOLTORB ) {
                _drawCard( id, FACE_UP[ type - CARD_ONE ] );
            }
        }

        SOUND::playSoundEffect( SFX_VOLTORB_FLIP );
        IO::waitForInteract( [ & ]( u8 ) {}, [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); } );
    }

    void voltorbFlipUI::moveSelection( int p_col, int p_row ) {
        SELECTED_COL   = p_col;
        SELECTED_ROW   = p_row;
        SpriteEntry* e = &IO::Oam->oamBuffer[ VF_SEL_OAM ];
        e->x           = BOARD_X0 + p_col * BOARD_STRIDE - VF_SEL_BORD;
        e->y           = BOARD_Y0 + p_row * BOARD_STRIDE - VF_SEL_BORD;
        showSprites( VF_SEL_OAM, 1, true );
    }

    void voltorbFlipUI::showMessage( const char* p_message ) {
        _drawMessageWindow( p_message );
        IO::waitForInteract( [ & ]( u8 ) {}, [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); } );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        _hideMessageWindow( );
    }

    bool voltorbFlipUI::askQuit( u16 p_payout ) {
        std::array<char, 100> msg{ };
        if( p_payout == 1 ) {
            snprintf( msg.data( ), msg.size( ), GET_STRING( IO::STR_VF_QUIT_NOW_1 ),
                      FS::kanaWrap( (unsigned) p_payout, CURRENT_LANGUAGE ) );
        } else if( p_payout > 0 ) {
            snprintf( msg.data( ), msg.size( ), GET_STRING( IO::STR_VF_QUIT_NOW_2 ),
                      FS::kanaWrap( (unsigned) p_payout, CURRENT_LANGUAGE ) );
        } else {
            snprintf( msg.data( ), msg.size( ), GET_STRING( IO::STR_VF_QUIT_NOW_3 ),
                      FS::kanaWrap( (unsigned) p_payout, CURRENT_LANGUAGE ) );
        }

        bool sel
            = IO::simpleYesNoBox( ).getResult(
                  [ & ]( ) {
                      _drawMessageWindow( msg.data( ), false );
                      showSprites( VF_YN_OAM( 0, 0 ), 12, true );
                      IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                      IO::regularFont->printString( GET_STRING( IO::STR_UI_YES ),
                                                    YN_YES_X + VF_YN_W / 2, YN_YES_Y + 8, true,
                                                    IO::font::CENTER, 15, 0, false, 1 );
                      IO::regularFont->printString( GET_STRING( IO::STR_UI_NO ),
                                                    YN_NO_X + VF_YN_W / 2, YN_NO_Y + 8, true,
                                                    IO::font::CENTER, 15, 0, false, 1 );

                      auto res
                          = std::vector<std::pair<IO::inputTarget, IO::yesNoBox::selection>>{ };
                      res.push_back(
                          std::pair( IO::inputTarget( YN_YES_X, YN_YES_Y, YN_YES_X + VF_YN_W,
                                                      YN_YES_Y + VF_YN_H ),
                                     IO::yesNoBox::YES ) );
                      res.push_back( std::pair(
                          IO::inputTarget( YN_NO_X, YN_NO_Y, YN_NO_X + VF_YN_W, YN_NO_Y + VF_YN_H ),
                          IO::yesNoBox::NO ) );
                      return res;
                  },
                  [ & ]( IO::yesNoBox::selection p_selection ) {
                      SpriteEntry* oam = IO::Oam->oamBuffer;
                      for( u8 cell = 0; cell < 2; ++cell ) {
                          u8 pal = ( cell == ( p_selection == IO::yesNoBox::YES ? 0 : 1 ) )
                                       ? VF_BOXSEL_PAL
                                       : VF_BOX_PAL;
                          for( u8 j = 0; j < 6; ++j ) { oam[ VF_YN_OAM( cell, j ) ].palette = pal; }
                      }
                      IO::updateOAM( true );
                  },
                  IO::yesNoBox::NO )
              == IO::yesNoBox::YES;

        showSprites( VF_YN_OAM( 0, 0 ), 12, false );
        _hideMessageWindow( );
        return sel;
    }

    void voltorbFlipUI::loadMemoButton( bool p_open ) {
        FILE* f = FS::open( "nitro:/PICS/", "voltorb_mbtn", ".rsd" );
        if( !f ) { return; }
        IO::loadAnimatedSprite( f, VF_MEMO_X, VF_MEMO_Y, VF_MEMO_OAM, VF_BTN_PUSH_PAL, VF_MEMO_TILE,
                                OBJPRIORITY_3, true );
        IO::Oam->oamBuffer[ VF_MEMO_OAM ].palette = VF_BTN_REST_PAL;
        clearTextRect( VF_MEMO_LBL_X, VF_MEMO_LBL_Y, VF_MEMO_LBL_W, VF_MEMO_LBL_H );

        IO::BG_PAL( true )[ IO::BLACK_IDX ] = IO::RGB( 8, 8, 8 );
        IO::regularFont->setColor( IO::BLACK_IDX, 1 );
        IO::regularFont->printStringC(
            p_open ? GET_STRING( IO::STR_VF_CLOSE_MEMO ) : GET_STRING( IO::STR_VF_OPEN_MEMO ),
            VF_MEMO_X + VF_MEMO_W / 2, VF_MEMO_Y + 21, true, IO::font::CENTER, 15, 0, false, 1 );
        IO::updateOAM( true );
    }

    void voltorbFlipUI::drawMemoPanel( bool p_visible, u8 p_mask ) {
        for( int i = 0; i < 4; ++i ) {
            SpriteEntry* e  = &IO::Oam->oamBuffer[ VF_PANEL_OAM( i ) ];
            bool         on = p_mask & VF_PANEL_MEMO[ i ];
            e->gfxIndex     = VF_PANEL_TILE + ( 2 * i + ( on ? 1 : 0 ) ) * VF_PANEL_FRAME_TILES;
            e->x            = VF_PANEL_X[ i ];
            e->y            = VF_PANEL_Y[ i ];
        }
        showSprites( VF_PANEL_OAM( 0 ), 4, p_visible );
        showSprites( VF_MEMOCUR_OAM, 1, p_visible );
    }

    void voltorbFlipUI::memoCursor( int p_cell ) {
        IO::Oam->oamBuffer[ VF_MEMOCUR_OAM ].x        = VF_PANEL_X[ p_cell ];
        IO::Oam->oamBuffer[ VF_MEMOCUR_OAM ].y        = VF_PANEL_Y[ p_cell ];
        IO::Oam->oamBuffer[ VF_MEMOCUR_OAM ].priority = OBJPRIORITY_0;
        showSprites( VF_MEMOCUR_OAM, 1, true );
    }

    void voltorbFlipUI::drawCardMark( int p_cardId, u8 p_mask ) {
        int x = BOARD_X0 + ( p_cardId % NUM_COLS ) * BOARD_STRIDE;
        int y = BOARD_Y0 + ( p_cardId / NUM_COLS ) * BOARD_STRIDE;
        restoreRect( _markTags[ p_cardId ], true, x, y, CARD_PX, CARD_PX );
        if( p_mask & MEMO_VOLTORB ) {
            const u8* face = FACE_UP[ CARD_VOLTORB - CARD_ONE ];
            for( int dy = 0; dy < 8; dy++ ) {
                for( int dx = 0; dx < 8; dx++ ) {
                    u8 v = face[ ( dy + 4 ) * CARD_PX + 8 + dx ];
                    if( v ) {
                        IO::setPixel( (u8) ( x + dx ), (u8) ( y + dy ), true, v, VF_BOARD_LAYER );
                    }
                }
            }
        }
        if( p_mask & MEMO_ONE ) { _drawBlock( true, DIGIT[ 1 ], 8, 8, x + 16, (s16) y ); }
        if( p_mask & MEMO_TWO ) { _drawBlock( true, DIGIT[ 2 ], 8, 8, x, (s16) ( y + 16 ) ); }
        if( p_mask & MEMO_THREE ) {
            _drawBlock( true, DIGIT[ 3 ], 8, 8, x + 16, (s16) ( y + 16 ) );
        }
    }

    void voltorbFlipUI::buttonHighlight( bool p_quitDown, bool p_memoDown ) {
        SpriteEntry* q = &IO::Oam->oamBuffer[ VF_QUIT_OAM ];
        SpriteEntry* m = &IO::Oam->oamBuffer[ VF_MEMO_OAM ];

        u8 qp = p_quitDown ? VF_BTN_PUSH_PAL : VF_BTN_REST_PAL;
        u8 mp = p_memoDown ? VF_BTN_PUSH_PAL : VF_BTN_REST_PAL;
        if( q->palette != qp || m->palette != mp ) {
            q->palette = qp;
            m->palette = mp;
            IO::updateOAM( true );
        }
    }
} // namespace SPX::VF
