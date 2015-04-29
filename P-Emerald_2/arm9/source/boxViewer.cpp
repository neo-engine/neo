#include "boxViewer.h"
#include "boxUI.h"
#include "defines.h"
#include "uio.h"
#include "statusScreen.h"
#include "statusScreenUI.h"

namespace BOX {
    u16 boxViewer::nextNonEmptyBox( u16 p_start ) {
        for( u16 i = p_start + 1; i <= MAX_PKMN + 1; ++i )
            if( !_box->empty( i ) )
                return i;
        return p_start;
    }
    u16 boxViewer::previousNonEmptyBox( u16 p_start ) {
        for( u16 i = p_start - 1; i > 0; --i )
            if( !p_start )
                return p_start;
            else if( !_box->empty( i ) )
                return i;
        return p_start;
    }

    void boxViewer::generateNextPage( ) {

        _currPage[ 0 ] = _currPage[ 28 ];

        u16 pokemon = _currPage[ 0 ].first;
        u16 pos = _currPage[ 0 ].second;

        for( u8 i = 1; i < 30; ++i )
            _currPage[ i ] = { 0, 0 };

        for( u8 i = 1; i < 30; ++i ) {
            if( ++pos < _box->count( pokemon ) )
                _currPage[ i ] = { pokemon, pos };
            else {
                u16 n = nextNonEmptyBox( pokemon );
                if( n == pokemon ) {
                    return;
                } else {
                    _currPage[ i ] = { n, 0 };
                    pos = 0;
                    pokemon = n;
                }
            }
        }
    }
    void boxViewer::generatePreviousPage( ) {

        if( !( _currPage[ 0 ].first || _currPage[ 0 ].second ) ) //It's already the first page
            return;

        _currPage[ 29 ] = _currPage[ 1 ];
        u16 pokemon = _currPage[ 1 ].first;
        u16 pos = _currPage[ 1 ].second;

        for( u8 i = 0; i < 29; ++i )
            _currPage[ i ] = { 0, 0 };

        for( u8 i = 28; i >= 0; --i ) {
            if( pos-- )
                _currPage[ i ] = { pokemon, pos };
            else {
                u16 n = previousNonEmptyBox( pokemon );
                if( n == pokemon ) {
                    _currPage[ 0 ] = { 0, 0 };

                    return;
                } else {
                    _currPage[ i ] = { n, pos = _box->count( n ) - 1 };
                    pokemon = n;
                }
            }
        }
    }

    void boxViewer::run( bool p_allowTakePkmn ) {
        _boxUI->init( );
        _ranges = _boxUI->draw( _currPage, _currPos = 0, _box, '*', true, p_allowTakePkmn );

        u8 mx = p_allowTakePkmn ? 21 : 28;

        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            u8 oldPos = _currPos;
            bool newPok;

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                if( newPok = ( ( ( ++_currPos ) %= mx ) < oldPos ) )
                    generateNextPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                if( newPok = ( ( ( _currPos += ( mx - 1 ) ) %= mx ) > oldPos ) )
                    generatePreviousPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                if( newPok = ( ( ( _currPos += ( mx - 7 ) ) %= mx ) > oldPos ) )
                    generatePreviousPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                if( newPok = ( ( ( _currPos += 7 ) %= mx ) < oldPos ) )
                    generateNextPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_A ) ) {

            }
        }
    }
}