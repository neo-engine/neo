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

#define HAS_SELECTION( no, yes ) do if( _selectedIdx == (u8) -1 ) { no; } else { yes; } while (false)
    void boxViewer::run( bool p_allowTakePkmn ) {
        _atHandOam = 0;
        _selectedIdx = (u8) -1;
        _ranges = _boxUI.draw( p_allowTakePkmn );

        touchPosition touch;
        u8 curr = -1, start = -1; //Indices the held sprite is covering
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            bool newPok;

            if( _atHandOam ) {
                _boxUI.updateAtHand( touch, _atHandOam );
            }

            if( _atHandOam && !( touch.px | touch.py ) ) { //Player drops the sprite at hand
                //boxUI.acceptDrop( start, curr, _atHandOam );
                _atHandOam = 0;
                //Count the non-fainted team pkmn
                u8 cnt = 0;
                for( u8 i = 0; i < 6; ++i )
                    if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId
                        && FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP )
                        ++cnt;
                /*
                                sprintf( buffer, "%d -> %d", start, curr );
                                IO::messageBox m( buffer );*/
                                /*
                                if( start == 255 || curr == 255 ) //Something went wrong
                                    continue;
                                else if( start < 21 && curr < 21 ) //Boxes auto-sort; no PKMN-Shifting inside Boxes
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
                                    generatePreviousPage( );
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

                                    generatePreviousPage( );
                                    generateNextPage( );
                                    _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                                }*/
            }

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_L ) ) {
                FS::SAV->m_curBox = ( FS::SAV->m_curBox + MAX_BOXES - 1 ) % MAX_BOXES;
                select( _selectedIdx = (u8) -1 );
                _boxUI.draw( p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_R ) ) {
                FS::SAV->m_curBox = ( FS::SAV->m_curBox + 1 ) % MAX_BOXES;
                select( _selectedIdx = (u8) -1 );
                _boxUI.draw( p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                HAS_SELECTION( _selectedIdx = 0,
                               _selectedIdx = ( _selectedIdx + 6 ) % ( MAX_PKMN_PER_BOX + 6 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                HAS_SELECTION( _selectedIdx = 0,
                               _selectedIdx = ( _selectedIdx + MAX_PKMN_PER_BOX ) % ( MAX_PKMN_PER_BOX + 6 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                HAS_SELECTION( _selectedIdx = 0,
                               _selectedIdx = ( _selectedIdx + 1 ) % ( MAX_PKMN_PER_BOX + 6 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                HAS_SELECTION( _selectedIdx = 0,
                               _selectedIdx = ( _selectedIdx + MAX_PKMN_PER_BOX + 5 ) % ( MAX_PKMN_PER_BOX + 6 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) ) {
                HAS_SELECTION( , takePkmn( _selectedIdx ) );
            }

            /*else if( GET_AND_WAIT( KEY_RIGHT ) ) {
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
                curr = -1; */
        }
    }
    void boxViewer::select( u8 p_index ) {
        if( p_index == (u8) -1 ) {
            dropPkmn( p_index );
            _boxUI.select( p_index );
            return;
        }

        _boxUI.select( p_index );
    }

    void boxViewer::takePkmn( u8 p_index ) {
        if( p_index > MAX_PKMN_PER_BOX + 6 )
            return;
        //New spot is empty -> drop every held pkmn
        if( p_index < MAX_PKMN_PER_BOX
            && !FS::SAV->currentBox( )->operator[]( p_index ).m_speciesId ) {
            dropPkmn( p_index );
            return;
        }
        if( p_index > MAX_PKMN_PER_BOX && _showTeam
            && !FS::SAV->m_pkmnTeam[ p_index - MAX_PKMN_PER_BOX ].m_boxdata.m_speciesId ) {
            dropPkmn( p_index );
            return;
        }
        if( p_index > MAX_PKMN_PER_BOX && !_showTeam
            && !FS::SAV->m_clipboard[ p_index - MAX_PKMN_PER_BOX ].m_speciesId ) {
            dropPkmn( p_index );
            return;
        }
        //Current spot is non-empty

        _boxUI.takePkmn( p_index );
    }
    void boxViewer::dropPkmn( u8 p_index ) {
        _boxUI.takePkmn( p_index );
    }
}