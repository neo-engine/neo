#include "dexUI.h"
#include "dex.h"
#include "defines.h"
#include "uio.h"

#include <cmath>

namespace DEX {
#define MAX_PAGES 3
    const u8 dexsppos[ 2 ][ 9 ] = { { 160, 128, 96, 19, 6, 120, 158, 196, 8 }, { -16, 0, 24, 138, 173, 108, 126, 144, 32 } };

    void dex::run( u16 p_pkmnIdx, bool p_time, s8 p_timeParameter ) {
        if( !_dexUI ) {
            _dexUI = new dexUI( false, p_pkmnIdx, _maxPkmn ); //Only the current Pkmn shall be shown
        }

        _dexUI->init( );
        _dexUI->_currPkmn = p_pkmnIdx;
        _dexUI->drawPage( true, true );

        touchPosition touch;
        loop( ) {
            if( p_time )
                IO::updateTime( p_timeParameter );
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) )
                break;
            else if( _maxPkmn != -1 && GET_AND_WAIT( KEY_DOWN ) ) {
                _dexUI->_currPkmn = ( _dexUI->_currPkmn + 1 ) % _maxPkmn;
                _dexUI->drawPage( true );
            } else if( _maxPkmn != -1 && GET_AND_WAIT( KEY_UP ) ) {
                _dexUI->_currPkmn = ( _dexUI->_currPkmn + _maxPkmn - 1 ) % _maxPkmn;
                _dexUI->drawPage( true );
            } else if( _maxPkmn != -1 && GET_AND_WAIT( KEY_R ) ) {
                _dexUI->_currPkmn = ( _dexUI->_currPkmn + 15 ) % _maxPkmn;
                _dexUI->drawPage( true );
            } else if( _maxPkmn != -1 && GET_AND_WAIT( KEY_L ) ) {
                _dexUI->_currPkmn = ( _dexUI->_currPkmn + _maxPkmn - 15 ) % _maxPkmn;
                _dexUI->drawPage( true );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                _dexUI->_currPage = ( _dexUI->_currPage + 1 ) % MAX_PAGES;
                _dexUI->drawPage( false, true );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                _dexUI->_currPage = ( _dexUI->_currPage + MAX_PAGES - 1 ) % MAX_PAGES;
                _dexUI->drawPage( false, true );
            } else if( GET_AND_WAIT( KEY_SELECT ) ) {
                _dexUI->_currForme++; //Just let it overflow
                _dexUI->drawPage( false );
            }
            for( u8 q = 0; q < 5; ++q )
                if( _maxPkmn != -1 && GET_AND_WAIT_C( dexsppos[ 0 ][ q ] + 16, dexsppos[ 1 ][ q ] + 16, 16 ) ) {
                    _dexUI->_currPkmn = ( _dexUI->_currPkmn + _maxPkmn - 3 + q + ( q > 2 ? 1 : 0 ) ) % _maxPkmn;
                    _dexUI->drawPage( true );
                }
            for( u8 q = 5; q < 8; ++q )
                if( GET_AND_WAIT_C( dexsppos[ 0 ][ q ] + 16, dexsppos[ 1 ][ q ] + 16, 16 )
                    && _dexUI->_currPage != q + 1 ) {
                    _dexUI->_currPage = ( q + 1 ) % MAX_PAGES;
                    _dexUI->drawPage( false, true );
                }
        }
    }
}