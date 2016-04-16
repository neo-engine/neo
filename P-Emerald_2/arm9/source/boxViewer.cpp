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
#include "keyboard.h"

#include "statusScreen.h"
#include "statusScreenUI.h"
#include "saveGame.h"

namespace BOX {
#define TRESHOLD 20

#define HAS_SELECTION( no, yes ) do if( _selectedIdx == (u8) -1 ) { no; } else { yes; } while (false)
    void boxViewer::run( bool p_allowTakePkmn ) {
#define CLEAN ( _topScreenDirty = false  )
        _ranges = _boxUI.draw( p_allowTakePkmn );
        CLEAN;
        _curPage = 0;
        _selectedIdx = (u8) -1;
        memset( &_heldPkmn, 0, sizeof( pokemon ) );

        _showTeam = p_allowTakePkmn;

        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                if( !_heldPkmn.m_boxdata.m_speciesId )
                    break;
            } else if( GET_AND_WAIT( KEY_L ) ) {
                FS::SAV->m_curBox = ( FS::SAV->m_curBox + MAX_BOXES - 1 ) % MAX_BOXES;
                CLEAN;
                _ranges = _boxUI.draw( p_allowTakePkmn );
                select( _selectedIdx );
            } else if( IN_RANGE_R( 24, 23, 48, 48 ) ) {
                _boxUI.buttonChange( boxUI::BUTTON_LEFT, true );
                loop( ) {
                    swiWaitForVBlank( );

                    scanKeys( );
                    touchRead( &touch );
                    if( TOUCH_UP ) {
                        FS::SAV->m_curBox = ( FS::SAV->m_curBox + MAX_BOXES - 1 ) % MAX_BOXES;
                        CLEAN;
                        _ranges = _boxUI.draw( p_allowTakePkmn );
                        select( _selectedIdx );
                        break;
                    }
                    if( !IN_RANGE_R( 24, 23, 48, 48 ) ) {
                        _boxUI.buttonChange( boxUI::BUTTON_LEFT, false );
                        break;
                    }
                }
            } else if( GET_AND_WAIT( KEY_R ) ) {
                FS::SAV->m_curBox = ( FS::SAV->m_curBox + 1 ) % MAX_BOXES;
                CLEAN;
                _ranges = _boxUI.draw( p_allowTakePkmn );
                select( _selectedIdx );
            } else if( IN_RANGE_R( 208, 23, 232, 48 ) ) {
                _boxUI.buttonChange( boxUI::BUTTON_RIGHT, true );
                loop( ) {
                    swiWaitForVBlank( );

                    scanKeys( );
                    touchRead( &touch );
                    if( TOUCH_UP ) {
                        FS::SAV->m_curBox = ( FS::SAV->m_curBox + 1 ) % MAX_BOXES;
                        CLEAN;
                        _ranges = _boxUI.draw( p_allowTakePkmn );
                        select( _selectedIdx );
                        break;
                    }
                    if( !IN_RANGE_R( 208, 23, 232, 48 ) ) {
                        _boxUI.buttonChange( boxUI::BUTTON_RIGHT, false );
                        break;
                    }
                }
            } else if( IN_RANGE_R( 49, 23, 207, 48 ) ) {
                _boxUI.buttonChange( boxUI::BUTTON_BOX_NAME, true );
                loop( ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    if( TOUCH_UP ) {
                        _boxUI.buttonChange( boxUI::BUTTON_BOX_NAME, false );
                        IO::swapScreens( );
                        IO::OamTop->oamBuffer[ 0 ].isHidden = true;
                        IO::updateOAM( false );
                        IO::printRectangle( 144, 192 - 14, 255, 192, false, false, WHITE_IDX );

                        IO::keyboard kb;
                        sprintf( buffer, "Name für Box „%s“", FS::SAV->getCurrentBox( )->m_name );
                        strcpy( FS::SAV->getCurrentBox( )->m_name, kb.getText( 14, buffer ).c_str( ) );
                        IO::swapScreens( );
                        IO::OamTop->oamBuffer[ 0 ].isHidden = false;
                        IO::updateOAM( false );

                        _ranges = _boxUI.draw( p_allowTakePkmn );
                        select( _selectedIdx );
                        break;
                    }
                    if( !IN_RANGE_R( 49, 23, 207, 48 ) ) {
                        _boxUI.buttonChange( boxUI::BUTTON_BOX_NAME, false );
                        break;
                    }
                }
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                if( _selectedIdx >= MAX_PKMN_PER_BOX && _selectedIdx < MAX_PKMN_PER_BOX + 6 )
                    _selectedIdx = MAX_PKMN_PER_BOX + 6;
                else if( _selectedIdx >= MAX_PKMN_PER_BOX + 6 )
                    _selectedIdx = 0;
                else
                    HAS_SELECTION( _selectedIdx = 0,
                                   _selectedIdx = ( _selectedIdx + 6 ) % ( MAX_PKMN_PER_BOX + 8 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                if( _selectedIdx < 6 )
                    _selectedIdx = MAX_PKMN_PER_BOX + 6;
                else if( _selectedIdx >= MAX_PKMN_PER_BOX + 6 )
                    _selectedIdx = MAX_PKMN_PER_BOX;
                else
                    HAS_SELECTION( _selectedIdx = 0,
                                   _selectedIdx = ( _selectedIdx + MAX_PKMN_PER_BOX + 2 ) % ( MAX_PKMN_PER_BOX + 8 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                HAS_SELECTION( _selectedIdx = 0,
                               _selectedIdx = ( _selectedIdx + 1 ) % ( MAX_PKMN_PER_BOX + 8 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                HAS_SELECTION( _selectedIdx = 0,
                               _selectedIdx = ( _selectedIdx + MAX_PKMN_PER_BOX + 7 ) % ( MAX_PKMN_PER_BOX + 8 ) );
                select( _selectedIdx );
            } else if( GET_AND_WAIT( KEY_A ) ) {
                HAS_SELECTION( , takePkmn( _selectedIdx ) );
            } else if( GET_AND_WAIT( KEY_SELECT ) ) {
                _curPage = ( _curPage + 1 ) % 5;
                select( _selectedIdx );
            }
            for( u8 i = 0; i < 5; ++i ) {
                if( IO::OamTop->oamBuffer[ PAGE_ICON_START + i ].isHidden )
                    continue;
                if( GET_AND_WAIT_C( 62 + 32 * i, 4, 14 ) ) {
                    _curPage = i;
                    select( _selectedIdx );
                }
            }

            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE_I( touch, i ) ) {
                    u8 c = 0;
                    loop( ) {
                        scanKeys( );
                        swiWaitForVBlank( );
                        touchRead( &touch );
                        if( c++ == TRESHOLD ) {
                            _selectedIdx = j;
                            takePkmn( _selectedIdx );
                            IO::waitForTouchUp( i );
                            break;
                        }
                        if( TOUCH_UP ) {
                            _selectedIdx = j;
                            select( _selectedIdx );
                            break;
                        }
                        if( !IN_RANGE_I( touch, i ) )
                            break;
                    }
                }
            }
        }
    }
    void boxViewer::select( u8 p_index ) {
        if( p_index == (u8) -1 ) {
            _boxUI.select( p_index );
            return;
        }
        pokemon selection;
        if( p_index < MAX_PKMN_PER_BOX )
            selection = ( *FS::SAV->getCurrentBox( ) )[ p_index ];
        else if( p_index < MAX_PKMN_PER_BOX + 6 ) {
            if( _showTeam )
                selection = FS::SAV->m_pkmnTeam[ p_index - MAX_PKMN_PER_BOX ];
            else
                selection = FS::SAV->m_clipboard[ p_index - MAX_PKMN_PER_BOX ];
        }
        _boxUI.select( p_index );
        if( p_index >= MAX_PKMN_PER_BOX + 6 )
            return;
        if( selection.m_boxdata.m_speciesId ) {
            if( !_topScreenDirty )
                _stsUI->init( );
            _stsUI->draw( selection, _curPage, true );
            _topScreenDirty = true;
        } else if( _topScreenDirty ) {
            _boxUI.drawAllBoxStatus( );
            _topScreenDirty = false;
        }
    }

    void boxViewer::takePkmn( u8 p_index ) {
        if( p_index >= MAX_PKMN_PER_BOX + 6 ) {
            if( p_index >= MAX_PKMN_PER_BOX + 8 )
                return;
            if( p_index == MAX_PKMN_PER_BOX + 6 ) // <
                FS::SAV->m_curBox = ( FS::SAV->m_curBox + MAX_BOXES - 1 ) % MAX_BOXES;
            else if( p_index == MAX_PKMN_PER_BOX + 7 ) // >
                FS::SAV->m_curBox = ( FS::SAV->m_curBox + 1 ) % MAX_BOXES;

            CLEAN;
            _ranges = _boxUI.draw( _showTeam );
            select( _selectedIdx );
            return;
        }

        pokemon::boxPokemon hld = _heldPkmn.m_boxdata;
        if( p_index < MAX_PKMN_PER_BOX )
            std::swap( hld, FS::SAV->getCurrentBox( )->operator[]( p_index ) );
        if( p_index >= MAX_PKMN_PER_BOX && !_showTeam )
            std::swap( hld, FS::SAV->m_clipboard[ p_index - MAX_PKMN_PER_BOX ] );
        if( p_index >= MAX_PKMN_PER_BOX && _showTeam ) {
            std::swap( _heldPkmn, FS::SAV->m_pkmnTeam[ p_index - MAX_PKMN_PER_BOX ] );
            if( !updateTeam( ) ) {
                std::swap( _heldPkmn, FS::SAV->m_pkmnTeam[ p_index - MAX_PKMN_PER_BOX ] );
                return;
            } else
                _boxUI.updateTeam( );
        } else
            _heldPkmn = pokemon( hld );
        _boxUI.takePkmn( p_index, _heldPkmn.m_boxdata.m_speciesId, _heldPkmn.m_boxdata.m_individualValues.m_isEgg );
    }

    //Remove gaps in party pkmn
    bool boxViewer::updateTeam( ) {
        u8 nxt = 1;
        for( u8 i = 0; i < 6; ++i, ++nxt )
            if( !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                for( ; nxt < 6; ++nxt )
                    if( FS::SAV->m_pkmnTeam[ nxt ].m_boxdata.m_speciesId ) {
                        std::swap( FS::SAV->m_pkmnTeam[ i ], FS::SAV->m_pkmnTeam[ nxt ] );
                        break;
                    }

        //Check if the party is safe to walk with
        for( u8 i = 0; i < 6; ++i )
            if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId
                && FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP
                && !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_individualValues.m_isEgg )
                return true;
        return false;
    }
}