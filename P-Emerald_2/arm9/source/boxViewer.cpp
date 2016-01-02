/*
Pokémon Emerald 2 Version
------------------------------

file        : boxViewer.cpp
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

#include "boxViewer.h"
#include "boxUI.h"
#include "defines.h"
#include "uio.h"
#include "messageBox.h"

#include "statusScreen.h"
#include "statusScreenUI.h"
#include "saveGame.h"

namespace BOX {
#define TRESHOLD 10
#define MAX_PKMN_PRO_PAGE 21
#define MAX_PKMN_PRO_PAGE_WITHOUT_TAKE 21
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

        for( s8 i = 28; i >= 0; --i ) {
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
        _atHandOam = 0;
        _ranges = _boxUI->draw( _currPage, _currPos = 0, _box, '*', true, p_allowTakePkmn );

        u8 mx = p_allowTakePkmn ? MAX_PKMN_PRO_PAGE : MAX_PKMN_PRO_PAGE_WITHOUT_TAKE;

        touchPosition touch;
        u8 curr = -1, start = -1; //Indices the held sprite is covering
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            u8 oldPos = _currPos;
            bool newPok;

            if( _atHandOam ) {
                _boxUI->updateAtHand( touch, _atHandOam );
            }

            if( _atHandOam && !( touch.px | touch.py ) ) { //Player drops the sprite at hand
                _boxUI->acceptDrop( start, curr, _atHandOam );
                _atHandOam = 0;
                //Count the non-fainted team pkmn
                u8 cnt = 0;
                for( u8 i = 0; i < 6; ++i )
                    if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId
                        && FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP )
                        ++cnt;

                if( start < 21 && curr < 21 ) //Boxes auto-sort; no PKMN-Shifting inside Boxes
                    continue;
                else if( start >= 21 && curr >= 21 ) { //Shift Team-Pkmn
                    if( FS::SAV->m_pkmnTeam[ curr - 21 ].m_boxdata.m_speciesId &&
                        FS::SAV->m_pkmnTeam[ start - 21 ].m_boxdata.m_speciesId ) {
                        std::swap( FS::SAV->m_pkmnTeam[ start - 21 ], FS::SAV->m_pkmnTeam[ curr - 21 ] );
                        _ranges = _boxUI->draw( _currPage, _currPos = curr, _box, start, true, true );
                    } else {
                        _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                    }
                } else if( start < 21 && curr >= 21 && _currPage[ start + 1 ].first ) { //withdraw pkmn
                                                                                        //Extract pkmn from the box
                    auto tmp = _box->operator[]( _currPage[ start + 1 ].first );
                    auto it = tmp.begin( );
                    for( u8 i = 0; i < _currPage[ start + 1 ].second; ++i, ++it );
                    pokemon target = pokemon( *it );
                    _box->erase( target );

                    //Swap the extracted Pkmn with the team pkmn
                    u8 rct = 0; //real count: including fainted ones
                    for( u8 i = 0; i < 6; ++i )
                        if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                            ++rct;
                    rct = std::min( rct, u8( curr - 21 ) );

                    std::swap( target, FS::SAV->m_pkmnTeam[ rct ] );

                    //Insert the team pkmn into the box
                    _box->insert( target );

                    //recalc and redraw everything
                    for( u8 i = 0; i < 30; ++i )
                        _currPage[ i ] = { 0, 0 };
                    generateNextPage( );
                    _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                } else if( start >= 21 && curr < 21 ) { //deposit pkmn
                    if( cnt <= 1 ) {
                        _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                        continue;
                    }
                    _box->insert( FS::SAV->m_pkmnTeam[ start - 21 ] );
                    for( u8 i = start - 21; i < 5; ++i )
                        std::swap( FS::SAV->m_pkmnTeam[ i ], FS::SAV->m_pkmnTeam[ i + 1 ] );
                    memset( &FS::SAV->m_pkmnTeam[ 5 ], 0, sizeof( pokemon ) );

                    for( u8 i = 0; i < 30; ++i )
                        _currPage[ i ] = { 0, 0 };
                    generateNextPage( );
                    _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                }
            }

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                if( p_allowTakePkmn && _currPos >= 21 ) {
                    newPok = false;
                    _currPos = ( ( _currPos - 20 ) % 6 ) + 21;
                } else {
                    if( ( newPok = ( ( ( ++_currPos ) %= mx ) < oldPos ) ) )
                        generateNextPage( );
                }
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                if( p_allowTakePkmn && _currPos >= 21 ) {
                    newPok = false;
                    _currPos = ( ( _currPos - 16 ) % 6 ) + 21;
                } else {
                    if( ( newPok = ( ( ( _currPos += ( mx - 1 ) ) %= mx ) > oldPos ) ) )
                        generatePreviousPage( );
                }
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                if( ( newPok = ( ( ( _currPos += ( mx - 7 ) ) %= mx ) > oldPos ) ) )
                    generatePreviousPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                if( ( newPok = ( ( ( _currPos += 7 ) %= mx ) < oldPos ) ) )
                    generateNextPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_A ) ) {

            }

            bool rangeChanged = false;
            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE( touch, i ) ) {
                    if( !_atHandOam ) {
                        u8 c = 0;
                        loop( ) {
                            scanKeys( );
                            swiWaitForVBlank( );
                            touchRead( &touch );
                            if( c++ == TRESHOLD && p_allowTakePkmn ) {
                                _atHandOam = _boxUI->getSprite( _currPos, j );
                                _ranges = _boxUI->draw( _currPage, j, _box, _currPos, false, p_allowTakePkmn );
                                _currPos = j;
                                if( _atHandOam )
                                    start = curr = j;
                                break;
                            }
                            if( !touch.px && !touch.py ) {
                                _boxUI->acceptTouch( _currPos, j, p_allowTakePkmn );
                                _ranges = _boxUI->draw( _currPage, j, _box, _currPos, false, p_allowTakePkmn );
                                _currPos = j;
                                break;
                            }
                            if( !IN_RANGE( touch, i ) )
                                break;
                        }
                    } else {
                        curr = j;
                        rangeChanged = true;
                    }
                }
            }
            if( !rangeChanged )
                curr = -1;
        }
    }
}