/*
    Pokémon neo
    ------------------------------

    file        : voltorbFlip.h
    author      : Philip Wellnitz
    description : Voltorb Flip, ported from HGSS.

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

#include <stdlib.h>
#include <string.h>

namespace SPX::VF {
    constexpr u8 NUM_COLS       = 5;
    constexpr u8 NUM_ROWS       = 5;
    constexpr u8 NUM_TILES      = NUM_COLS * NUM_ROWS;
    constexpr u8 HISTORY_LENGTH = 5;

    constexpr u16 MAX_PAYOUT = 50000;

    constexpr u8 MAX_LEVEL        = 8;
    constexpr u8 BOARDS_PER_LEVEL = 10;

    enum cardType {
        CARD_NONE,
        CARD_ONE,
        CARD_TWO,
        CARD_THREE,
        CARD_VOLTORB,
    };

    enum roundOutcome {
        OUT_NONE,
        OUT_QUIT,
        OUT_WON,
        OUT_LOST,
    };

    enum memoMark {
        MEMO_NONE    = 0,
        MEMO_VOLTORB = 1 << 0,
        MEMO_ONE     = 1 << 1,
        MEMO_TWO     = 1 << 2,
        MEMO_THREE   = 1 << 3,
    };

    struct card {
        cardType m_type;
        u8       m_memo;
        bool     m_flipped;
    };

    struct roundSummary {
        roundOutcome m_outcome;
        u8           m_cardsFlipped;
        u8           m_boardId;
        u8           m_level;
    };

    struct boardConfig {
        u8 m_voltorbs;
        u8 m_twos;
        u8 m_threes;
        u8 m_maxFreeMultipliersPerRowCol : 4;
        u8 m_maxFreeMultipliers : 4;
    };

    extern const boardConfig BOARD_CONFIGS[ MAX_LEVEL * BOARDS_PER_LEVEL ];

    u8 cardValue( cardType p_type );

    class voltorbFlip {
      private:
        card         _cards[ NUM_ROWS ][ NUM_COLS ];
        u8           _pointsPerCol[ NUM_COLS ];
        u8           _pointsPerRow[ NUM_ROWS ];
        u8           _voltorbsPerCol[ NUM_COLS ];
        u8           _voltorbsPerRow[ NUM_ROWS ];
        roundOutcome _roundOutcome;
        u16          _payout;
        u16          _maxPayout;
        u16          _numMultipliersOnBoard;
        u16          _numMultipliersFlipped;
        u8           _cardsFlipped;
        u8           _boardId;
        u8           _level;
        u8           _historyHead;
        roundSummary _boardHistory[ HISTORY_LENGTH ];

        roundSummary*       _historyTop( );
        const roundSummary* _historyTop( ) const;
        void                _pushHistory( );
        void                _selectBoardId( );
        void                _countPointsInRowCols( );
        void                _countVoltorbsInRowCols( );
        void                _calcBoardMaxPayout( );
        void                _countMultiplierCards( );
        void                _placeCardsOnBoard( cardType p_type, u8 p_n, u16 p_maxAttempts = 100 );
        bool                _boardRegenNeeded( ) const;
        void                _generateBoard( u16 p_maxAttempts = 1000 );

      public:
        voltorbFlip( ) {
        }

        const card&  cardAt( u8 p_cardId ) const;
        card*        getCard( u8 p_cardId );
        u8           pointsPerRow( u8 p_row ) const;
        u8           pointsPerCol( u8 p_col ) const;
        u8           voltPerRow( u8 p_row ) const;
        u8           voltPerCol( u8 p_col ) const;
        u16          payout( ) const;
        u16          maxPayout( ) const;
        u8           level( ) const;
        u8           cardsFlipped( ) const;
        roundOutcome outcome( ) const;
        void         setRoundOutcome( roundOutcome p_outcome );
        void         flipCard( u8 p_cardId );
        void         multiplyPayout( cardType p_type );
        void         newBoard( );
        void         updateHistoryAndReset( );
        bool         cleared( ) const;
        u8           toggleMemo( u8 p_cardId, u8 p_mark );
        u8           calcNextLevel( ) const;
    };
} // namespace SPX::VF
