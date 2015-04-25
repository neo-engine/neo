#include "boxViewer.h"
#include "boxUI.h"
#include "defines.h"
#include "uio.h"

namespace BOX {
    u16 boxViewer::nextNonEmptyBox( u16 p_start ) {
        for( u16 i = p_start + 1; i < MAX_PKMN + 1; ++i )
            if( !_box->empty( i ) )
                return i;
        return p_start;
    }
    u16 boxViewer::previousNonEmptyBox( u16 p_start ) {
        for( u16 i = p_start; i > 0; --i )
            if( !i )
                return p_start;
            else if( !_box->empty( i ) )
                return i;
        return p_start;
    }

    void boxViewer::generatePage( u16 p_pokemon, u16 p_pos ) {
        u16 fst = p_pokemon;
        u16 fpos = p_pos;

        for( u8 i = 0; i < 30; ++i )
            _currPage[ i ] = { 0, 0 };

        if( p_pos < _box->count( p_pokemon ) )
            _currPage[ 1 ] = { p_pokemon, p_pos };
        else {
            u16 n = nextNonEmptyBox( p_pokemon );
            if( n == p_pokemon )
                return;
            else {
                _currPage[ 1 ] = { n, 0 };
                p_pos = 0;
                fst = p_pokemon = n;
            }
        }

        if( p_pos )
            _currPage[ 0 ] = { p_pokemon, p_pos - 1 };
        else {
            u16 n = previousNonEmptyBox( p_pokemon );
            if( n != p_pokemon )
                _currPage[ 0 ] = { n, _box->count( n ) - 1 };
        }

        for( u8 i = 2; i < 30; ++i ) {
            if( ++p_pos < _box->count( p_pokemon ) )
                _currPage[ i ] = { p_pokemon, p_pos };
            else {
                u16 n = nextNonEmptyBox( p_pokemon );
                if( n == p_pokemon || n == fst ) {
                    _currPage[ 29 ] = { p_pokemon, p_pos };
                    _currPage[ 0 ] = { 0, 0 };

                    return;
                } else {
                    _currPage[ i ] = { n, 0 };
                    p_pos = 0;
                    p_pokemon = n;
                }
            }
        }
    }

    void boxViewer::run( bool p_allowTakePkmn ) {
        _boxUI->init( );
        _ranges = _boxUI->draw( _currPage, _currPos = 0, _box );
        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            u8 oldPos = _currPos;

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                if( ( ( ++_currPos ) %= 28 ) < oldPos )
                    generatePage( _currPage[ 29 ].first, _currPage[ 29 ].second );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                if( ( ( _currPos += 27 ) %= 28 ) > oldPos )
                    generatePage( _currPage[ 0 ].first, _currPage[ 0 ].second );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                if( ( ( _currPos += 21 ) %= 28 ) > oldPos )
                    generatePage( _currPage[ 0 ].first, _currPage[ 0 ].second );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos );
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                if( ( ( _currPos += 7 ) %= 28 ) < oldPos )
                    generatePage( _currPage[ 29 ].first, _currPage[ 29 ].second );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos );
            }
        }
    }
}