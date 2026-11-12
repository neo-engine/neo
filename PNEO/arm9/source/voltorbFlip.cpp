/*
    Pokémon neo
    ------------------------------

    file        : voltorbFlip.cpp
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
#include "spx/voltorbFlip.h"

namespace SPX::VF {
#define VF_IS_MULTIPLIER_CARD( p_type ) ( (u32) ( ( p_type ) - 2 ) <= 1 )
#define VF_LEVEL_AT_LEAST( p_boardId, p_level ) \
    ( ( p_boardId ) >= BOARDS_PER_LEVEL * ( ( p_level ) - 1 ) )

    const boardConfig BOARD_CONFIGS[ MAX_LEVEL * BOARDS_PER_LEVEL ] = {
        // Lv. 1
        { 6, 3, 1, 3, 3 },
        { 6, 0, 3, 2, 2 },
        { 6, 5, 0, 3, 4 },
        { 6, 2, 2, 3, 3 },
        { 6, 4, 1, 3, 4 },
        { 6, 3, 1, 3, 3 },
        { 6, 0, 3, 2, 2 },
        { 6, 5, 0, 3, 4 },
        { 6, 2, 2, 3, 3 },
        { 6, 4, 1, 3, 4 },
        // Lv. 2
        { 7, 1, 3, 2, 3 },
        { 7, 6, 0, 3, 4 },
        { 7, 3, 2, 2, 3 },
        { 7, 0, 4, 2, 3 },
        { 7, 5, 1, 3, 4 },
        { 7, 1, 3, 2, 2 },
        { 7, 6, 0, 3, 3 },
        { 7, 3, 2, 2, 2 },
        { 7, 0, 4, 2, 2 },
        { 7, 5, 1, 3, 3 },
        // Lv. 3
        { 8, 2, 3, 2, 3 },
        { 8, 7, 0, 3, 4 },
        { 8, 4, 2, 3, 3 },
        { 8, 1, 4, 2, 3 },
        { 8, 6, 1, 4, 3 },
        { 8, 2, 3, 2, 2 },
        { 8, 7, 0, 3, 3 },
        { 8, 4, 2, 3, 3 },
        { 8, 1, 4, 2, 2 },
        { 8, 6, 1, 3, 3 },
        // Lv. 4
        { 8, 3, 3, 4, 3 },
        { 8, 0, 5, 2, 3 },
        { 10, 8, 0, 4, 5 },
        { 10, 5, 2, 3, 4 },
        { 10, 2, 4, 3, 4 },
        { 8, 3, 3, 3, 3 },
        { 8, 0, 5, 2, 2 },
        { 10, 8, 0, 4, 4 },
        { 10, 5, 2, 3, 3 },
        { 10, 2, 4, 3, 3 },
        // Lv. 5
        { 10, 7, 1, 4, 5 },
        { 10, 4, 3, 3, 4 },
        { 10, 1, 5, 3, 4 },
        { 10, 9, 0, 4, 5 },
        { 10, 6, 2, 4, 5 },
        { 10, 7, 1, 4, 4 },
        { 10, 4, 3, 3, 3 },
        { 10, 1, 5, 3, 3 },
        { 10, 9, 0, 4, 4 },
        { 10, 6, 2, 4, 4 },
        // Lv. 6
        { 10, 3, 4, 3, 4 },
        { 10, 0, 6, 3, 4 },
        { 10, 8, 1, 4, 5 },
        { 10, 5, 3, 4, 5 },
        { 10, 2, 5, 3, 4 },
        { 10, 3, 4, 3, 3 },
        { 10, 0, 6, 3, 3 },
        { 10, 8, 1, 4, 4 },
        { 10, 5, 3, 4, 4 },
        { 10, 2, 5, 3, 3 },
        // Lv. 7
        { 10, 7, 2, 4, 5 },
        { 10, 4, 4, 4, 5 },
        { 13, 1, 6, 3, 4 },
        { 13, 9, 1, 5, 6 },
        { 10, 6, 3, 4, 5 },
        { 10, 7, 2, 4, 4 },
        { 10, 4, 4, 4, 4 },
        { 13, 1, 6, 3, 3 },
        { 13, 9, 1, 5, 5 },
        { 10, 6, 3, 4, 4 },
        // Lv. 8
        { 10, 0, 7, 3, 4 },
        { 10, 8, 2, 5, 6 },
        { 10, 5, 4, 4, 5 },
        { 10, 2, 6, 4, 5 },
        { 10, 7, 3, 5, 6 },
        { 10, 0, 7, 3, 3 },
        { 10, 8, 2, 5, 5 },
        { 10, 5, 4, 4, 4 },
        { 10, 2, 6, 4, 4 },
        { 10, 7, 3, 5, 5 },
    };

    u8 cardValue( cardType p_type ) {
        switch( p_type ) {
        case CARD_ONE: return 1;
        case CARD_TWO: return 2;
        case CARD_THREE: return 3;
        default: return 0;
        }
    }

    roundSummary* voltorbFlip::_historyTop( ) {
        int head = _historyHead;
        return &_boardHistory[ head == 0 ? HISTORY_LENGTH - 1 : head - 1 ];
    }
    const roundSummary* voltorbFlip::_historyTop( ) const {
        int head = _historyHead;
        return &_boardHistory[ head == 0 ? HISTORY_LENGTH - 1 : head - 1 ];
    }

    void voltorbFlip::_pushHistory( ) {
        roundSummary* round   = &_boardHistory[ _historyHead ];
        round->m_outcome      = _roundOutcome;
        round->m_cardsFlipped = _cardsFlipped;
        round->m_boardId      = _boardId;
        round->m_level        = _level;
        _historyHead          = ( _historyHead + 1 ) % HISTORY_LENGTH;
    }

    void voltorbFlip::_selectBoardId( ) {
        _level   = calcNextLevel( );
        _boardId = (u8) ( _level * BOARDS_PER_LEVEL + rand( ) % BOARDS_PER_LEVEL );
    }

    void voltorbFlip::_countPointsInRowCols( ) {
        for( int r = 0; r < NUM_ROWS; ++r ) {
            _pointsPerRow[ r ] = 0;
            for( int c = 0; c < NUM_COLS; ++c ) {
                _pointsPerRow[ r ] += cardValue( _cards[ r ][ c ].m_type );
            }
        }
        for( int c = 0; c < NUM_COLS; ++c ) {
            _pointsPerCol[ c ] = 0;
            for( int r = 0; r < NUM_ROWS; ++r ) {
                _pointsPerCol[ c ] += cardValue( _cards[ r ][ c ].m_type );
            }
        }
    }

    void voltorbFlip::_countVoltorbsInRowCols( ) {
        for( int r = 0; r < NUM_ROWS; r++ ) {
            _voltorbsPerRow[ r ] = 0;
            for( int c = 0; c < NUM_COLS; c++ ) {
                if( _cards[ r ][ c ].m_type == CARD_VOLTORB ) { ++_voltorbsPerRow[ r ]; }
            }
        }
        for( int c = 0; c < NUM_COLS; c++ ) {
            _voltorbsPerCol[ c ] = 0;
            for( int r = 0; r < NUM_ROWS; r++ ) {
                if( _cards[ r ][ c ].m_type == CARD_VOLTORB ) { ++_voltorbsPerCol[ c ]; }
            }
        }
    }

    void voltorbFlip::_calcBoardMaxPayout( ) {
        int payout = 1;
        for( int i = 0; i < NUM_TILES; i++ ) {
            card* c = getCard( i );
            if( c->m_type != CARD_VOLTORB ) { payout *= cardValue( c->m_type ); }
        }
        if( payout > MAX_PAYOUT ) { payout = MAX_PAYOUT; }
        _maxPayout = (u16) payout;
    }

    void voltorbFlip::_countMultiplierCards( ) {
        _numMultipliersOnBoard = 0;
        for( int i = 0; i < NUM_TILES; i++ ) {
            if( VF_IS_MULTIPLIER_CARD( getCard( i )->m_type ) ) { _numMultipliersOnBoard++; }
        }
    }

    void voltorbFlip::_placeCardsOnBoard( cardType p_type, u8 p_n, u16 p_maxAttempts ) {
        bool overrideWithOneCard = p_type == CARD_ONE;
        u16  attempts            = 0;
        for( u8 i = 0; i < p_n; ++i ) {
            u8    cardId = !overrideWithOneCard ? (u8) ( (u32) rand( ) % NUM_TILES ) : (u8) i;
            card* c      = getCard( cardId );
            if( c->m_type == CARD_ONE || overrideWithOneCard ) {
                c->m_type = p_type;
            } else {
                if( ++attempts >= p_maxAttempts ) { break; }
                --i;
            }
        }
    }

    bool voltorbFlip::_boardRegenNeeded( ) const {
        u8 freePerCol[ NUM_COLS ] = { 0 };
        u8 freePerRow[ NUM_ROWS ] = { 0 };
        u8 free                   = 0;

        const boardConfig* config = &BOARD_CONFIGS[ _boardId ];

        for( int i = 0; i < NUM_TILES; ++i ) {
            const card& c = cardAt( i );
            if( VF_IS_MULTIPLIER_CARD( c.m_type ) ) {
                int col = i % NUM_COLS;
                int row = i / NUM_COLS;
                if( _voltorbsPerRow[ row ] == 0 || _voltorbsPerCol[ col ] == 0 ) {
                    freePerCol[ col ]++;
                    freePerRow[ row ]++;
                    free++;
                }
            }
        }

        if( config->m_maxFreeMultipliers <= free ) { return true; }
        for( int c = 0; c < NUM_COLS; ++c ) {
            if( config->m_maxFreeMultipliersPerRowCol <= freePerCol[ c ] ) { return true; }
        }
        for( int r = 0; r < NUM_ROWS; ++r ) {
            if( config->m_maxFreeMultipliersPerRowCol <= freePerRow[ r ] ) { return true; }
        }
        return false;
    }

    void voltorbFlip::_generateBoard( u16 p_maxTries ) {
        u8 voltorbs = BOARD_CONFIGS[ _boardId ].m_voltorbs;
        u8 twos     = BOARD_CONFIGS[ _boardId ].m_twos;
        u8 threes   = BOARD_CONFIGS[ _boardId ].m_threes;

        for( u16 i = 0; i < p_maxTries; ++i ) {
            _placeCardsOnBoard( CARD_ONE, NUM_TILES );
            _placeCardsOnBoard( CARD_VOLTORB, voltorbs );
            _placeCardsOnBoard( CARD_TWO, twos );
            _placeCardsOnBoard( CARD_THREE, threes );
            _countVoltorbsInRowCols( );

            if( !_boardRegenNeeded( ) ) { break; }
        }
    }

    const card& voltorbFlip::cardAt( u8 p_cardId ) const {
        return _cards[ p_cardId / NUM_COLS ][ p_cardId % NUM_COLS ];
    }
    card* voltorbFlip::getCard( u8 p_cardId ) {
        return &_cards[ p_cardId / NUM_COLS ][ p_cardId % NUM_COLS ];
    }

    u8 voltorbFlip::pointsPerRow( u8 p_row ) const {
        return _pointsPerRow[ p_row ];
    }
    u8 voltorbFlip::pointsPerCol( u8 p_col ) const {
        return _pointsPerCol[ p_col ];
    }
    u8 voltorbFlip::voltPerRow( u8 p_row ) const {
        return _voltorbsPerRow[ p_row ];
    }
    u8 voltorbFlip::voltPerCol( u8 p_col ) const {
        return _voltorbsPerCol[ p_col ];
    }
    u16 voltorbFlip::payout( ) const {
        return _payout;
    }
    u16 voltorbFlip::maxPayout( ) const {
        return _maxPayout;
    }
    u8 voltorbFlip::level( ) const {
        return _level;
    }
    u8 voltorbFlip::cardsFlipped( ) const {
        return _cardsFlipped;
    }
    roundOutcome voltorbFlip::outcome( ) const {
        return _roundOutcome;
    }
    void voltorbFlip::setRoundOutcome( roundOutcome p_outcome ) {
        _roundOutcome = p_outcome;
    }

    void voltorbFlip::flipCard( u8 p_cardId ) {
        card* c      = getCard( p_cardId );
        c->m_flipped = true;
        c->m_memo    = 0;
    }

    void voltorbFlip::multiplyPayout( cardType p_type ) {
        int value = cardValue( p_type );
        if( value == 0 ) { return; }
        _cardsFlipped++;
        if( VF_IS_MULTIPLIER_CARD( p_type ) ) { _numMultipliersFlipped++; }
        int payout = _payout;
        if( payout == 0 ) {
            _payout = (u16) value;
            return;
        }
        int next = payout * value;
        if( next > MAX_PAYOUT ) { next = MAX_PAYOUT; }
        _payout = (u16) next;
    }

    void voltorbFlip::newBoard( ) {
        _selectBoardId( );
        _generateBoard( );
        _countPointsInRowCols( );
        _calcBoardMaxPayout( );
        _countMultiplierCards( );
    }

    void voltorbFlip::updateHistoryAndReset( ) {
        _pushHistory( );

        roundSummary bak[ HISTORY_LENGTH ];
        for( int i = 0; i < HISTORY_LENGTH; ++i ) { bak[ i ] = _boardHistory[ i ]; }
        u8 head = _historyHead;

        memset( this, 0, sizeof( voltorbFlip ) );

        for( int i = 0; i < HISTORY_LENGTH; ++i ) { _boardHistory[ i ] = bak[ i ]; }
        _historyHead = head;
    }

    bool voltorbFlip::cleared( ) const {
        return _maxPayout > 1 && _payout >= _maxPayout;
    }

    u8 voltorbFlip::toggleMemo( u8 p_cardId, u8 p_mark ) {
        card* c = getCard( p_cardId );
        if( !c->m_flipped ) { c->m_memo ^= p_mark; }
        return c->m_memo;
    }

    u8 voltorbFlip::calcNextLevel( ) const {
        const roundSummary* prevRound = _historyTop( );
        roundOutcome        outcome   = prevRound->m_outcome;
        u8                  boardId   = prevRound->m_boardId;

        if( outcome == OUT_WON && VF_LEVEL_AT_LEAST( prevRound->m_boardId, 8 ) ) { return 7; }

        if( VF_LEVEL_AT_LEAST( boardId, 5 ) ) {
            u8 i = 0;
            for( ; i < HISTORY_LENGTH; ++i ) {
                const roundSummary* round = &_boardHistory[ i ];
                if( round->m_cardsFlipped < 8 || round->m_outcome == OUT_LOST ) { break; }
            }
            if( i == HISTORY_LENGTH ) { return 7; }
        }

        if( ( VF_LEVEL_AT_LEAST( boardId, 7 ) && prevRound->m_cardsFlipped >= 7 )
            || ( VF_LEVEL_AT_LEAST( boardId, 6 ) && outcome == OUT_WON ) ) {
            return 6;
        }
        if( ( VF_LEVEL_AT_LEAST( boardId, 6 ) && prevRound->m_cardsFlipped >= 6 )
            || ( VF_LEVEL_AT_LEAST( boardId, 5 ) && outcome == OUT_WON ) ) {
            return 5;
        }
        if( ( VF_LEVEL_AT_LEAST( boardId, 5 ) && prevRound->m_cardsFlipped >= 5 )
            || ( VF_LEVEL_AT_LEAST( boardId, 4 ) && outcome == OUT_WON ) ) {
            return 4;
        }
        if( ( VF_LEVEL_AT_LEAST( boardId, 4 ) && prevRound->m_cardsFlipped >= 4 )
            || ( VF_LEVEL_AT_LEAST( boardId, 3 ) && outcome == OUT_WON ) ) {
            return 3;
        }
        if( ( VF_LEVEL_AT_LEAST( boardId, 3 ) && prevRound->m_cardsFlipped >= 3 )
            || ( VF_LEVEL_AT_LEAST( boardId, 2 ) && outcome == OUT_WON ) ) {
            return 2;
        }
        if( ( VF_LEVEL_AT_LEAST( boardId, 2 ) && prevRound->m_cardsFlipped >= 2 )
            || ( outcome == OUT_WON ) ) {
            return 1;
        }
        return 0;
    }

} // namespace SPX::VF
