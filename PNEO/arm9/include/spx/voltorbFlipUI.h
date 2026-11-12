/*
    Pokémon neo
    ------------------------------

    file        : voltorbFlipUI.h
    author      : Philip Wellnitz
    description : Voltorb Flip UI definitions.

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

#pragma once
#include <nds.h>
#include "voltorbFlip.h"

namespace SPX::VF {
    class voltorbFlipUI {
      private:
        static constexpr int COIN_X        = 168;
        static constexpr int TOTAL_BAND_Y  = 112;
        static constexpr int PAYOUT_BAND_Y = 154;

        u8 _markTags[ NUM_TILES ][ 24 * 24 ];

        u32 _shownPayout = 0;
        u32 _shownTotal  = 0;

        void _drawMessageWindow( const char* p_message, bool p_continue = true );
        void _hideMessageWindow( );

        void _loadBoard( );
        void _loadTopScreen( );
        void _loadWindow( );
        void _loadExplosion( );
        void _loadCoins( );
        void _loadMemoButtons( );
        void _loadQuitButton( );
        void _loadSelectionBox( );

        void _drawBlock( bool p_bottom, const u8* p_src, int p_w, int p_h, int p_x, int p_y );
        void _drawCard( int p_cardId, const u8* p_block );
        void _drawHints( const voltorbFlip& p_board );
        void _printTopText( const voltorbFlip& p_board );
        void _hideSelection( );

        void _setCounter( int p_bandY, int p_oamBase, u32 p_value );

      public:
        void init( const voltorbFlip& p_board );
        void fullRedraw( const voltorbFlip& p_board );

        void drawPayout( u32 p_payout );
        void drawTotal( u32 p_total );
        void countPayoutTo( const char* p_message, u32 p_payout );
        void transferToTotal( const char* p_awardMessage, u32 p_payout );

        void animateFlip( int p_cardId, u8 p_type );
        void explode( int p_cardId );

        void revealBoard( const voltorbFlip& p_board );

        void moveSelection( int p_col, int p_row );

        void showMessage( const char* p_message );

        bool askQuit( u16 p_payout );

        void loadMemoButton( bool p_open );
        void drawMemoPanel( bool p_visible, u8 p_mask );
        void memoCursor( int p_cell );
        void drawCardMark( int p_cardId, u8 p_mask );

        void buttonHighlight( bool p_quitDown, bool p_memoDown );
    };

    int  memoButtonAt( int p_x, int p_y );
    u8   memoButtonMemo( int p_i );
    int  cellAt( int p_px, int p_py );
    bool inQuitRect( int p_x, int p_y );
    bool inMemoRect( int p_x, int p_y );
} // namespace SPX::VF
