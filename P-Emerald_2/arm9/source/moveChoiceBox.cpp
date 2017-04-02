/*
Pokémon Emerald 2 Version
------------------------------

file        : moveChoiceBox.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2017
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

#include "moveChoiceBox.h"
#include "defines.h"
#include "move.h"
#include "saveGame.h"
#include "uio.h"

#include "Back.h"
#include "Backward.h"
#include "Forward.h"

#include <algorithm>
#include <cmath>

namespace IO {

    const u8 CHOICE_POS[ 2 ][ 5 ][ 4 ]
        = {{{24, 40, 124, 96}, {132, 40, 232, 96}, {24, 104, 124, 162}, {132, 104, 232, 162}},
           {{24, 36, 124, 76},
            {132, 36, 232, 76},
            {24, 80, 124, 120},
            {132, 80, 232, 120},
            {78, 124, 178, 164}}};
#define NEW_PAGE 9
    void moveChoiceBox::draw( u8 p_pressedIdx ) {
        if( p_pressedIdx == NEW_PAGE ) {
            if( _drawSub ) NAV->draw( );
            IO::printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 28, true, false, WHITE_IDX );
            regularFont->printString( GET_STRING( 49 ), 8, 8, true );
        }

        BG_PALETTE_SUB[ RED_IDX ] = CHOICE_COLOR;

        for( u8 i = 0; i < 4 + !!_moveToLearn; ++i ) {
            auto moveIdx              = ( i < 4 ) ? _pokemon.m_boxdata.m_moves[ i ] : _moveToLearn;
            auto acMove               = AttackList[ moveIdx ];
            BG_PALETTE_SUB[ 240 + i ] = moveIdx ? IO::getColor( acMove->m_moveType ) : GRAY;

            printChoiceBox(
                CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ], CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ],
                CHOICE_POS[ !!_moveToLearn ][ i ][ 2 ], CHOICE_POS[ !!_moveToLearn ][ i ][ 3 ], 4,
                ( _selectedIdx == i ) ? RED_IDX : 240 + i, i == p_pressedIdx );

            if( !acMove ) continue;

            regularFont->printString(
                acMove->m_moveName.c_str( ), CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ]
                                                 + ( CHOICE_POS[ !!_moveToLearn ][ i ][ 2 ]
                                                     - CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ] )
                                                       / 2
                                                 + 2 * ( p_pressedIdx == i ),
                CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 4 + ( p_pressedIdx == i ), true,
                IO::font::CENTER );
            char buffer[ 30 ];
            u8   curPP = ( i < 4 ) ? _pokemon.m_boxdata.m_acPP[ i ] : acMove->m_movePP;
            snprintf( buffer, 20, "%hhu/%hhu%s", curPP, acMove->m_movePP, GET_STRING( 31 ) );
            regularFont->printString(
                buffer, CHOICE_POS[ !!_moveToLearn ][ i ][ 2 ] - 5 + 2 * ( p_pressedIdx == i ),
                CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 20 + ( p_pressedIdx == i ), true,
                IO::font::RIGHT );

            Oam->oamBuffer[ 3 + 2 * i ].x
                = 2 * ( p_pressedIdx == i ) + CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ] + 6;
            Oam->oamBuffer[ 3 + 2 * i ].y
                = ( p_pressedIdx == i ) + CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 20;

            if( !_moveToLearn ) {
                if( acMove->m_moveBasePower && acMove->m_moveHitType != move::STAT )
                    snprintf( buffer, 20, "%s %hhu", GET_STRING( 184 ), acMove->m_moveBasePower );
                else
                    snprintf( buffer, 20, "%s ---", GET_STRING( 184 ) );
                regularFont->printString(
                    buffer, CHOICE_POS[ !!_moveToLearn ][ i ][ 2 ] - 7 + 2 * ( p_pressedIdx == i ),
                    CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 39 + ( p_pressedIdx == i ), true,
                    IO::font::RIGHT );

                Oam->oamBuffer[ 4 + 2 * i ].x
                    = 2 * ( p_pressedIdx == i ) + CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ] + 6;
                Oam->oamBuffer[ 4 + 2 * i ].y
                    = ( p_pressedIdx == i ) + CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 38;
            }

            swiWaitForVBlank( );
        }
        updateOAM( true );
    }

    moveChoiceBox::moveChoiceBox( pokemon p_pokemon, u16 p_moveToLearn, bool p_drawSub ) {
        _pokemon     = p_pokemon;
        _moveToLearn = p_moveToLearn;
        _drawSub     = p_drawSub;
        _selectedIdx = (u8) -1;

        initOAMTable( true );
        u16 tileCnt = 16;

        tileCnt = loadSprite( BACK_ID, 0, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32,
                              BackPal, BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0,
                              true );

        for( u8 i = 0; i < 4; ++i ) {
            if( !p_pokemon.m_boxdata.m_moves[ i ] ) break;
            tileCnt = loadTypeIcon( AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ]->m_moveType,
                                    CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ] + 6,
                                    CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 20, 3 + 2 * i,
                                    3 + 2 * i, tileCnt, true );
            if( !_moveToLearn )
                tileCnt = loadDamageCategoryIcon(
                    AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ]->m_moveHitType,
                    CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ] + 6,
                    CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ] + 38, 4 + 2 * i, 4 + 2 * i, tileCnt,
                    true );
        }
        if( _moveToLearn ) {
            tileCnt = loadTypeIcon( AttackList[ _moveToLearn ]->m_moveType, 12, 134, 11, 11,
                                    tileCnt, true );
            tileCnt = loadDamageCategoryIcon( AttackList[ _moveToLearn ]->m_moveHitType, 44, 134,
                                              12, 12, tileCnt, true );
        }
    }

    moveChoiceBox::moveChoiceBox( pokemon p_pokemon, bool p_drawSub )
        : moveChoiceBox( p_pokemon, 0, p_drawSub ) {
    }

    int moveChoiceBox::getResult( bool p_backButton, bool p_drawSub, u8 p_initialSelection ) {
        _selectedIdx = p_initialSelection;
        _drawSub     = p_drawSub;

        Oam->oamBuffer[ BACK_ID ].isHidden = !p_backButton;
        updateOAM( true );

        int result = -1;
        draw( NEW_PAGE );
        u8 endIdx = !!_moveToLearn + 4;

        loop( ) {
            swiWaitForVBlank( );
            touchPosition touch;
            touchRead( &touch );
            scanKeys( );
            int pressed = keysCurrent( );

            if( p_backButton && GET_AND_WAIT( KEY_B ) ) return -1;
            if( ( _selectedIdx == 4
                  || ( _selectedIdx < 4 && _pokemon.m_boxdata.m_moves[ _selectedIdx ] ) )
                && GET_AND_WAIT( KEY_A ) )
                return (int) _selectedIdx;
            if( GET_AND_WAIT( KEY_DOWN ) ) {
                if( _selectedIdx == (u8) -1 )
                    _selectedIdx = 0;
                else if( _selectedIdx + 2 >= endIdx ) {
                    if( _moveToLearn ) _selectedIdx = endIdx - 1;
                } else
                    _selectedIdx += 2;
                draw( -1 );
            }
            if( GET_AND_WAIT( KEY_UP ) ) {
                if( _selectedIdx == (u8) -1 )
                    _selectedIdx = 0;
                else if( _selectedIdx >= 2 ) {
                    _selectedIdx -= 2;
                    draw( -1 );
                }
            }

            if( GET_AND_WAIT( KEY_RIGHT ) || GET_AND_WAIT( KEY_LEFT ) ) {
                if( _selectedIdx == (u8) -1 )
                    _selectedIdx = 0;
                else if( _selectedIdx % 2 )
                    _selectedIdx--;
                else if( _selectedIdx < 4 )
                    _selectedIdx++;
                draw( -1 );
            }

            for( u8 i = 0; i < 4 + !!_moveToLearn; ++i )
                if( IN_RANGE_R( CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ],
                                CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ],
                                CHOICE_POS[ !!_moveToLearn ][ i ][ 2 ],
                                CHOICE_POS[ !!_moveToLearn ][ i ][ 3 ] ) ) {
                    if( i < 4 && !_pokemon.m_boxdata.m_moves[ i ] ) continue;

                    draw( i );
                    if( waitForTouchUp( CHOICE_POS[ !!_moveToLearn ][ i ][ 0 ],
                                        CHOICE_POS[ !!_moveToLearn ][ i ][ 1 ],
                                        CHOICE_POS[ !!_moveToLearn ][ i ][ 2 ],
                                        CHOICE_POS[ !!_moveToLearn ][ i ][ 3 ] ) )
                        return i;
                    draw( -1 );
                }
        }
        return result;
    }
}
