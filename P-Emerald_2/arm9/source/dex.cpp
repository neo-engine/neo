/*
Pokémon Emerald 2 Version
------------------------------

file        : dex.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dexUI.h"
#include "dex.h"
#include "defines.h"
#include "uio.h"
#include "saveGame.h"

#include <cmath>

namespace DEX {
#define MAX_PAGES 3

#define MAX_PKMN_ALL 32
#define MAX_PKMN_CAUGHT 5
#define MAX_PKMN_PER_PAGE( p_mode ) ( (p_mode == SHOW_CAUGHT) ? MAX_PKMN_CAUGHT : MAX_PKMN_ALL )

    u16 nextEntry( u16 p_startIdx ) {
        if( p_startIdx > MAX_PKMN )
            return MAX_PKMN + 1;
        for( u16 i = p_startIdx + 1; i <= MAX_PKMN; ++i )
            if( IN_DEX( i ) )
                return i;
        return MAX_PKMN + 1;
    }

    u16 previousEntry( u16 p_startIdx ) {
        if( p_startIdx > MAX_PKMN )
            p_startIdx = MAX_PKMN + 1;
        for( u16 i = p_startIdx - 1; p_startIdx > 0 && p_startIdx <= MAX_PKMN; --i )
            if( IN_DEX( i ) )
                return i;
        return 0;
    }

    dex::dex( mode p_mode, u16 p_maxPkmn )
        : _maxPkmn( p_maxPkmn ), _page( 0 ), _mode( p_mode ) {
        _dexUI = new dexUI( p_mode != SHOW_SINGLE, p_maxPkmn );
    }

    void dex::changeMode( mode p_newMode, u16 p_startPkmn ) {
        if( p_newMode == SHOW_CAUGHT && !IN_DEX( p_startPkmn ) )
            p_startPkmn = nextEntry( p_startPkmn );
        if( p_newMode == SHOW_ALL )
            p_startPkmn -= 12;

        memset( _curPkmn, 0, sizeof( _curPkmn ) );
        _curPkmnStart = 0;

        for( u8 i = 0; i < MAX_PKMN_PER_PAGE( p_newMode ); ++i ) {
            _curPkmn[ i ] = p_startPkmn;
            if( p_newMode == SHOW_CAUGHT )
                p_startPkmn = nextEntry( p_startPkmn );
            else
                ++p_startPkmn;
        }

        _mode = p_newMode;
        _dexUI->drawPage( _curPkmn[ ( _curPkmnStart + _selectedIdx ) % MAX_PKMN_PER_PAGE( p_newMode ) ], _page );
        _dexUI->drawSub( p_newMode, _curPkmn, _curPkmnStart, _selectedIdx );
    }

    void dex::select( u8 p_idx ) {
        _selectedIdx = p_idx;

        _dexUI->drawPage( _curPkmn[ p_idx ], _page );
        s8 rs = _dexUI->select( p_idx );
        if( rs == 1 ) rotateForward( );
        else if( rs == -1 ) rotateBackward( );

        if( rs )
            _dexUI->drawSub( _mode, _curPkmn, _curPkmnStart, _selectedIdx );
    }

    void dex::rotateForward( ) {
        if( _mode == SHOW_ALL ) {
            u16 end = _curPkmn[ ( _curPkmnStart + MAX_PKMN_ALL - 1 ) % MAX_PKMN_ALL ];
            for( u8 i = 0; i < 8; ++i )
                _curPkmn[ ( _curPkmnStart + i ) % MAX_PKMN_ALL ] = ++end;
            _curPkmnStart = ( _curPkmnStart + 8 ) % MAX_PKMN_ALL;
        } else if( _mode == SHOW_CAUGHT ) {
            _curPkmn[ _curPkmnStart ] =
                nextEntry( _curPkmn[ ( _curPkmnStart + MAX_PKMN_CAUGHT - 1 ) % MAX_PKMN_CAUGHT ] );
            _curPkmnStart = ( _curPkmnStart + 1 ) % MAX_PKMN_CAUGHT;
        }
    }

    void dex::rotateBackward( ) {
        if( _mode == SHOW_ALL ) {
            u16 start = _curPkmn[ _curPkmnStart ];
            for( u8 i = 0; i < 8; ++i )
                _curPkmn[ ( _curPkmnStart - i + MAX_PKMN_ALL ) % MAX_PKMN_ALL ] = --start;
            _curPkmnStart = ( _curPkmnStart + MAX_PKMN_ALL - 8 ) % MAX_PKMN_ALL;
        } else if( _mode == SHOW_CAUGHT ) {
            _curPkmn[ ( _curPkmnStart + MAX_PKMN_CAUGHT - 1 ) % MAX_PKMN_CAUGHT ] =
                previousEntry( _curPkmn[ _curPkmnStart ] );
            _curPkmnStart = ( _curPkmnStart + MAX_PKMN_CAUGHT - 1 ) % MAX_PKMN_CAUGHT;
        }
    }

    void dex::run( u16 p_pkmnIdx ) {
        FS::SAV->m_lstDex = p_pkmnIdx;

        changeMode( _mode, p_pkmnIdx );
        select( 12 * ( _mode == SHOW_ALL ) );

        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) )
                break;
            /*
            else if( _maxPkmn != u16( -1 ) && GET_AND_WAIT( KEY_DOWN ) ) {
                FS::SAV->m_lstDex = ( FS::SAV->m_lstDex + 1 ) % _maxPkmn;
                _dexUI->drawPage( );
            } else if( _maxPkmn != u16( -1 ) && GET_AND_WAIT( KEY_UP ) ) {
                FS::SAV->m_lstDex = ( FS::SAV->m_lstDex + _maxPkmn - 1 ) % _maxPkmn;
                _dexUI->drawPage( );
            } else if( _maxPkmn != u16( -1 ) && GET_AND_WAIT( KEY_R ) ) {
                FS::SAV->m_lstDex = ( FS::SAV->m_lstDex + 15 ) % _maxPkmn;
                _dexUI->drawPage( );
            } else if( _maxPkmn != u16( -1 ) && GET_AND_WAIT( KEY_L ) ) {
                FS::SAV->m_lstDex = ( FS::SAV->m_lstDex + _maxPkmn - 15 ) % _maxPkmn;
                _dexUI->drawPage( );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                _dexUI->_currPage = ( _dexUI->_currPage + 1 ) % MAX_PAGES;
                _dexUI->drawPage( );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                _dexUI->_currPage = ( _dexUI->_currPage + MAX_PAGES - 1 ) % MAX_PAGES;
                _dexUI->drawPage( );
            } else if( GET_AND_WAIT( KEY_SELECT ) || GET_AND_WAIT( KEY_Y ) ) {
                _dexUI->_currForme++; //Just let it overflow
                _dexUI->drawPage( );
            }
            for( u8 q = 0; q < 5; ++q )
                if( _maxPkmn != u16( -1 ) && GET_AND_WAIT_C( dexsppos[ 0 ][ q ] + 16, dexsppos[ 1 ][ q ] + 16, 16 ) ) {
                    FS::SAV->m_lstDex = ( FS::SAV->m_lstDex + _maxPkmn - 3 + q + ( q > 2 ? 1 : 0 ) ) % _maxPkmn;
                    _dexUI->drawPage( );
                }
            for( u8 q = 5; q < 8; ++q )
                if( GET_AND_WAIT_C( dexsppos[ 0 ][ q ] + 16, dexsppos[ 1 ][ q ] + 16, 16 )
                    && _dexUI->_currPage != ( q + 1 ) % MAX_PAGES ) {
                    _dexUI->_currPage = ( q + 1 ) % MAX_PAGES;
                    _dexUI->drawPage( );
                } */
        }
    }
}