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
#define MAX_PKMN_PER_PAGE( p_mode ) ( (p_mode == dex::SHOW_CAUGHT) ? MAX_PKMN_CAUGHT : MAX_PKMN_ALL )

#define CURR_PKMN _curPkmn[ ( _curPkmnStart + _selectedIdx ) % MAX_PKMN_PER_PAGE( _mode ) ]
#define DRAW_TOP( ) _dexUI->drawPage( CURR_PKMN, _page, _forme )

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
        if( !p_startIdx ) return 0;
        for( u16 i = p_startIdx - 1; i && i <= MAX_PKMN; --i )
            if( IN_DEX( i ) )
                return i;
        return 0;
    }

    dex::dex( mode p_mode, u16 p_maxPkmn )
        : _maxPkmn( p_maxPkmn ), _page( 0 ), _forme( 0 ), _mode( p_mode ) {
        _dexUI = new dexUI( p_mode != SHOW_SINGLE, p_maxPkmn );
    }

    void dex::changeMode( mode p_newMode, u16 p_startPkmn ) {
        IO::NAV->draw( );
        if( p_newMode == SHOW_CAUGHT && !IN_DEX( p_startPkmn ) ) {
            p_startPkmn = nextEntry( p_startPkmn );
            if( p_startPkmn > MAX_PKMN )
                p_startPkmn = previousEntry( p_startPkmn );
        }
        if( p_newMode == SHOW_ALL ) {
            _selectedIdx = 12 + ( ( p_startPkmn + 7 ) % 8 );
            p_startPkmn = ( ( p_startPkmn - 1 ) / 8 * 8 ) - 11;
        } else
            _selectedIdx = 0;

        memset( _curPkmn, 0, sizeof( _curPkmn ) );
        _curPkmnStart = 0;

        for( u8 i = 0; i < MAX_PKMN_PER_PAGE( p_newMode ); ++i ) {
            if( p_newMode == SHOW_SINGLE && i )
                break;
            _curPkmn[ i ] = p_startPkmn;
            if( p_newMode == SHOW_CAUGHT )
                p_startPkmn = nextEntry( p_startPkmn );
            else
                ++p_startPkmn;
        }

        _mode = p_newMode;
        DRAW_TOP( );
        _dexUI->drawSub( p_newMode, _curPkmn, _curPkmnStart, _selectedIdx );
    }

    void dex::select( u8 p_idx ) {
        if( _curPkmn[ ( _curPkmnStart + p_idx ) % MAX_PKMN_PER_PAGE( _mode ) ] > MAX_PKMN
            || !_curPkmn[ ( _curPkmnStart + p_idx ) % MAX_PKMN_PER_PAGE( _mode ) ] )
            return;

        u8 oldIdx = _selectedIdx;
        while( p_idx >= 20 ) {
            rotateForward( );
            p_idx = ( p_idx + MAX_PKMN_ALL - 8 ) % MAX_PKMN_ALL;
        }
        while( p_idx < 12 ) {
            rotateBackward( );
            p_idx = ( p_idx + 8 ) % MAX_PKMN_ALL;
        }
        _selectedIdx = p_idx;
        SAVE::SAV->getActiveFile( ).m_lstDex = CURR_PKMN;

        DRAW_TOP( );
        _dexUI->drawSub( _mode, _curPkmn, _curPkmnStart, _selectedIdx, oldIdx );
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
            for( u8 i = 1; i <= 8; ++i )
                _curPkmn[ ( _curPkmnStart - i + MAX_PKMN_ALL ) % MAX_PKMN_ALL ] = --start;
            _curPkmnStart = ( _curPkmnStart + MAX_PKMN_ALL - 8 ) % MAX_PKMN_ALL;
        } else if( _mode == SHOW_CAUGHT ) {
            _curPkmn[ ( _curPkmnStart + MAX_PKMN_CAUGHT - 1 ) % MAX_PKMN_CAUGHT ] =
                previousEntry( _curPkmn[ _curPkmnStart ] );
            _curPkmnStart = ( _curPkmnStart + MAX_PKMN_CAUGHT - 1 ) % MAX_PKMN_CAUGHT;
        }
    }

    void dex::run( u16 p_pkmnIdx ) {
        SAVE::SAV->getActiveFile( ).m_lstDex = p_pkmnIdx;

        changeMode( _mode, p_pkmnIdx );

        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );

            int pressed = keysCurrent( );
            if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) )
                break;
            else if( GET_AND_WAIT( KEY_SELECT ) || GET_AND_WAIT( KEY_Y ) ) {
                if( _page ) {
                    _forme++;
                    DRAW_TOP( );
                }
            }
            if( CURR_PKMN && IN_DEX( CURR_PKMN ) )
                for( u8 i = 0; i < 3; ++i ) {
                    if( IO::Oam->oamBuffer[ 1 + i ].isHidden )
                        continue;
                    if( GET_AND_WAIT_C( 94 + 32 * i, 4, 14 ) ) {
                        _page = i;
                        DRAW_TOP( );
                        break;
                    }
                }

            if( GET_AND_WAIT( KEY_R ) ) {
                _page = ( _page + 1 ) % MAX_PAGES;
                DRAW_TOP( );
            } else if( GET_AND_WAIT( KEY_L ) ) {
                _page = ( _page + MAX_PAGES - 1 ) % MAX_PAGES;
                DRAW_TOP( );
            }

            if( _mode != SHOW_SINGLE )
                if( GET_AND_WAIT( KEY_START )
                    || GET_AND_WAIT_R( 1, 1, 30 * ( 1 + ( _mode == SHOW_CAUGHT ) ), 16 ) )
                    changeMode( mode( !_mode ), CURR_PKMN );
            if( _mode != SHOW_ALL ) {
                if( GET_AND_WAIT( KEY_RIGHT ) ) {
                    _page = ( _page + 1 ) % MAX_PAGES;
                    DRAW_TOP( );
                } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                    _page = ( _page + MAX_PAGES - 1 ) % MAX_PAGES;
                    DRAW_TOP( );
                }
            }
            if( _mode == SHOW_CAUGHT ) {
                if( GET_AND_WAIT( KEY_DOWN ) ) {
                    if( nextEntry( CURR_PKMN ) < MAX_PKMN + 1 ) {
                        if( _selectedIdx == 4 )
                            rotateForward( );
                        else
                            _selectedIdx++;
                        SAVE::SAV->getActiveFile( ).m_lstDex = CURR_PKMN;
                        DRAW_TOP( );
                        _dexUI->drawSub( _mode, _curPkmn, _curPkmnStart, _selectedIdx );
                    }
                } else if( GET_AND_WAIT( KEY_UP ) ) {
                    if( previousEntry( CURR_PKMN ) ) {
                        if( _selectedIdx == 0 )
                            rotateBackward( );
                        else
                            _selectedIdx--;
                        SAVE::SAV->getActiveFile( ).m_lstDex = CURR_PKMN;
                        DRAW_TOP( );
                        _dexUI->drawSub( _mode, _curPkmn, _curPkmnStart, _selectedIdx );
                    }
                }
                for( u8 i = FRAME_START_2; i < FRAME_START_2 + MAX_PKMN_CAUGHT; ++i )
                    if( !IO::Oam->oamBuffer[ i ].isHidden
                        && GET_AND_WAIT_R( IO::Oam->oamBuffer[ i ].x, IO::Oam->oamBuffer[ i ].y,
                                           IO::Oam->oamBuffer[ i ].x + 32, IO::Oam->oamBuffer[ i ].y + 28 ) ) {
                        _selectedIdx = i - FRAME_START_2;
                        SAVE::SAV->getActiveFile( ).m_lstDex = CURR_PKMN;
                        DRAW_TOP( );
                        _dexUI->drawSub( _mode, _curPkmn, _curPkmnStart, _selectedIdx );
                    }
            } else if( _mode == SHOW_ALL ) {
                if( GET_AND_WAIT( KEY_DOWN ) )
                    select( _selectedIdx + 1 );
                else if( GET_AND_WAIT( KEY_UP ) )
                    select( _selectedIdx - 1 );
                else if( GET_AND_WAIT( KEY_RIGHT ) )
                    select( ( _selectedIdx + 2 * ( 4 - ( ( _selectedIdx + 2 ) % 4 ) ) - 1 ) % MAX_PKMN_ALL );
                else if( GET_AND_WAIT( KEY_LEFT ) )
                    select( ( _selectedIdx + MAX_PKMN_ALL - 2 * ( ( _selectedIdx + 2 ) % 4 ) - 1 ) % MAX_PKMN_ALL );

                for( u8 i = FRAME_START; i < FRAME_START + MAX_PKMN_ALL; ++i )
                    if( !IO::Oam->oamBuffer[ i ].isHidden
                        && GET_AND_WAIT_R( IO::Oam->oamBuffer[ i ].x, IO::Oam->oamBuffer[ i ].y,
                                           IO::Oam->oamBuffer[ i ].x + 32, IO::Oam->oamBuffer[ i ].y + 28 ) )
                        select( i - FRAME_START );
            }
        }
    }
}
