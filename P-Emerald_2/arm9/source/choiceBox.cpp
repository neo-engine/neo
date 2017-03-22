/*
Pokémon Emerald 2 Version
------------------------------

file        : choiceBox.cpp
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

#include "choiceBox.h"
#include "uio.h"
#include "defines.h"

#include "Back.h"
#include "Forward.h"
#include "Backward.h"

#include <cmath>
#include <algorithm>

namespace IO {
#define NEW_PAGE 9
    void choiceBox::draw( u8 p_pressedIdx ) {
        if( p_pressedIdx == NEW_PAGE ) {
            if( _drawSub )
                NAV->draw( );
            initTextField( );
            if( _text ) {
                if( _name ) {
                    regularFont->printString( _name, 8, 8, true );
                    regularFont->printString( _text, 72, 8, true );
                } else
                    regularFont->printString( _text, 8, 8, true );
            }
        }
        u8 startIdx = ( ( !_big ) * 3 + 3 ) * _acPage;
        u8 endIdx = std::min( _num, u8( ( ( !_big ) * 3 + 3 ) * ( _acPage + 1 ) ) );

        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        BG_PALETTE_SUB[ RED_IDX ] = GREEN;

        if( _big ) {
            for( u8 i = startIdx; i < endIdx; ++i ) {
                u8 acPos = i - startIdx;
                printChoiceBox( 32, 68 + acPos * 35, 192 + 32, 68 + 32 + acPos * 35, 6, ( _selectedIdx == acPos ) ? RED_IDX : COLOR_IDX, acPos == p_pressedIdx );
                regularFont->printString( _choices[ i ], 32 + 2 * ( p_pressedIdx == acPos ) + 184 / 2,
                                          78 + acPos * 35 + ( p_pressedIdx == acPos ), true, IO::font::CENTER );
                swiWaitForVBlank( );
            }
        } else {
            for( u8 i = startIdx; i < endIdx; ++i ) {
                u8 acPos = i - startIdx;
                printChoiceBox( ( ( acPos % 2 ) ? 129 : 19 ), 68 + ( acPos / 2 ) * 35,
                                106 + ( ( acPos % 2 ) ? 129 : 19 ), 32 + 68 + ( acPos / 2 ) * 35, 6, ( _selectedIdx == acPos ) ? RED_IDX : COLOR_IDX, acPos == p_pressedIdx );
                regularFont->printString( _choices[ i ], ( ( acPos % 2 ) ? 129 : 19 ) + 2 * ( p_pressedIdx == acPos ) + 102 / 2,
                                          78 + ( acPos / 2 ) * 35 + ( p_pressedIdx == acPos ), true, IO::font::CENTER );
                swiWaitForVBlank( );
            }
        }
    }
    choiceBox::choiceBox( int p_num, const char** p_choices, const char* p_name = 0, bool p_big = false ) {
        _num = p_num;
        _choices = p_choices;
        _big = p_big || ( p_num <= 3 );
        _acPage = 0;
        _drawSub = true;

        initTextField( );

        initOAMTable( true );
        u16 tileCnt = 16;

        tileCnt = loadSprite( BACK_ID, 0, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );
        tileCnt = loadSprite( FWD_ID, 1, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, ForwardPal,
                              ForwardTiles, ForwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        tileCnt = loadSprite( BWD_ID, 2, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackwardPal,
                              BackwardTiles, BackwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        updateOAM( true );

        _name = p_name;

        swiWaitForVBlank( );
    }

    std::vector<const char*> names;
    choiceBox::choiceBox( pokemon p_pokemon, u16 p_move ) {
        _num = 4 + !!p_move;
        names = { AttackList[ p_pokemon.m_boxdata.m_moves[ 0 ] ]->m_moveName.c_str( ),
            AttackList[ p_pokemon.m_boxdata.m_moves[ 1 ] ]->m_moveName.c_str( ),
            AttackList[ p_pokemon.m_boxdata.m_moves[ 2 ] ]->m_moveName.c_str( ),
            AttackList[ p_pokemon.m_boxdata.m_moves[ 3 ] ]->m_moveName.c_str( ),
            AttackList[ p_move ]->m_moveName.c_str( ) };
        _choices = &names[ 0 ];
        _big = false;
        _acPage = 0;
        _name = 0;
        _drawSub = false;

        initTextField( );

        initOAMTable( true );
        u16 tileCnt = 0;

        tileCnt = loadSprite( BACK_ID, 0, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );
        tileCnt = loadSprite( FWD_ID, 1, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, ForwardPal,
                              ForwardTiles, ForwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        tileCnt = loadSprite( BWD_ID, 2, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackwardPal,
                              BackwardTiles, BackwardTilesLen, false, false, true, OBJPRIORITY_1, true );

        for( u8 i = 0; i < 4; ++i ) {
            if( !p_pokemon.m_boxdata.m_moves[ i ] ) {
                _num -= 4 - i;
                break;
            }
            tileCnt = loadTypeIcon( AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ]->m_moveType,
                ( ( i % 2 ) ? 122 : 12 ), 64 + ( i / 2 ) * 35, 3 + 2 * i, 3 + 2 * i, tileCnt, true );
            tileCnt = loadDamageCategoryIcon( AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ]->m_moveHitType,
                ( ( i % 2 ) ? 154 : 44 ), 64 + ( i / 2 ) * 35, 4 + 2 * i, 4 + 2 * i, tileCnt, true );
        }
        if( p_move ) {
            tileCnt = loadTypeIcon( AttackList[ p_move ]->m_moveType,
                                    12, 134, 11, 11, tileCnt, true );
            tileCnt = loadDamageCategoryIcon( AttackList[ p_move ]->m_moveHitType,
                                              44, 134, 12, 12, tileCnt, true );
        }

        updateOAM( true );
        swiWaitForVBlank( );
    }

    int fwdPos[ 2 ][ 2 ] = { { SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12 }, { SCREEN_WIDTH - 11, SCREEN_HEIGHT - 31 } },
        bwdPos[ 2 ][ 2 ] = { { SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12 }, { SCREEN_WIDTH - 31, SCREEN_HEIGHT - 11 } };

    void choiceBox::updateButtons( bool p_backButton ) {
        if( ( _acPage + 1 ) * 3 >= _num ) {
            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = true;
            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = !_acPage;
            ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ p_backButton ][ 0 ] - 16;
            ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ p_backButton ][ 1 ] - 16;
        } else if( !_acPage ) {
            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = true;
            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
            ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
        } else {
            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
            ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
            ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
            ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
        }
        updateOAM( true );
    }

    int choiceBox::getResult( const char* p_text, bool p_backButton, bool p_drawSub, u8 p_initialSelection ) {
        _drawSub = p_drawSub;
        _text = p_text;
        _selectedIdx = p_initialSelection;
        draw( NEW_PAGE );

        int result = -1;
        if( p_backButton ) {
            ( Oam->oamBuffer[ BACK_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ BACK_ID ] ).x = fwdPos[ 0 ][ 0 ] - 16;
            ( Oam->oamBuffer[ BACK_ID ] ).y = fwdPos[ 0 ][ 1 ] - 14;
            updateOAM( true );
        }

        if( _num < 1 )
            return -1;
        else if( _num <= 3 || ( _num > 3 && _big ) ) {
            _acPage = 0;
            if( _num > 3 && _big ) {
                ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
            }
            loop( ) {
                swiWaitForVBlank( );
                updateOAM( true );
                touchPosition t;
                auto& touch = t;
                touchRead( &t );
                scanKeys( );
                int pressed = keysCurrent( );

                for( u8 i = 0; i < 3; ++i )
                    if( ( i + 3 * _acPage ) < _num && t.px >= 32 && t.py >= 68 + 35 * i && t.px <= 224 && t.py <= 100 + 35 * i ) {
                        draw( i );
                        if( !waitForTouchUp( 32, 68 + 35 * i, 224, 100 + 35 * i ) ) {
                            draw( 8 );
                            break;
                        }
                        result = i + 3 * _acPage;
                        goto END;
                    }
                if( GET_AND_WAIT( KEY_DOWN ) ) {
                    ++_selectedIdx;
                    if( _selectedIdx + 3 * _acPage >= _num || _selectedIdx == 3 ) {
                        if( ( _acPage + 1 ) * 3 < _num ) {
                            ++_acPage;
                            _selectedIdx = 0;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        } else
                            --_selectedIdx;
                        continue;
                    }
                    draw( -1 );
                }
                if( GET_AND_WAIT( KEY_UP ) ) {
                    if( !_selectedIdx || _selectedIdx == (u8) -1 ) {
                        if( _acPage ) {
                            _selectedIdx = 2;
                            --_acPage;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        }
                        continue;
                    }
                    --_selectedIdx;
                    draw( -1 );
                }
                if( _selectedIdx != (u8) -1 && GET_AND_WAIT( KEY_A ) ) {
                    result = _selectedIdx + 3 * _acPage;
                    goto END;
                }
                if( p_backButton && ( GET_AND_WAIT_C( fwdPos[ 0 ][ 0 ], fwdPos[ 0 ][ 1 ] + 2, 16 ) || GET_AND_WAIT( KEY_B ) ) ) { //Back pressed
                    result = -1;
                    goto END;
                }
                if( _num > 3 && _big ) {
                    if( ( !p_backButton && ( ( _num - 1 ) / 3 && _acPage == 0 && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] ) + sq( t.py - fwdPos[ 0 ][ 1 ] ) ) < 17 ) )
                        || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - fwdPos[ 1 ][ 0 ] ) + sq( t.py - fwdPos[ 1 ][ 1 ] ) ) < 17 )
                        || GET_AND_WAIT( KEY_RIGHT ) ) {

                        waitForTouchUp( );
                        if( ( _acPage + 1 ) * 3 < _num ) {
                            ++_acPage;
                            while( _selectedIdx + 3 * _acPage >= _num )
                                --_selectedIdx;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        }
                    } else if( ( !p_backButton && ( _acPage && _acPage == ( _num - 1 ) / 3 && sqrt( sq( t.px - bwdPos[ 0 ][ 0 ] ) + sq( t.py - bwdPos[ 0 ][ 1 ] ) ) < 17 ) )
                               || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - bwdPos[ 1 ][ 0 ] ) + sq( t.py - bwdPos[ 1 ][ 1 ] ) ) < 17 )
                               || GET_AND_WAIT( KEY_LEFT ) ) {

                        waitForTouchUp( );
                        if( _acPage ) {
                            --_acPage;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        }
                    }
                }
            }
        } else {
            _acPage = 0;
            if( _num > 6 ) {
                ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
            }
            updateOAM( true );
            loop( ) {
                swiWaitForVBlank( );
                updateOAM( true );
                touchPosition touch;
                auto& t = touch;
                touchRead( &touch );
                scanKeys( );
                int pressed = keysCurrent( );

                for( u8 i = 0; i < std::min( 6, _num - 6 * _acPage ); ++i ) {
                    if( t.px >= ( ( i % 2 ) ? 129 : 19 ) && t.py >= 68 + ( i / 2 ) * 35
                        && t.px <= 106 + ( ( i % 2 ) ? 129 : 19 ) && t.py <= 32 + 68 + ( i / 2 ) * 35 ) {
                        draw( i );

                        if( !waitForTouchUp( ( ( i % 2 ) ? 129 : 19 ),
                                             68 + ( i / 2 ) * 35, 106 + ( ( i % 2 ) ? 129 : 19 ), 32 + 68 + ( i / 2 ) * 35 ) ) {
                            draw( 8 );
                            break;
                        }
                        result = i + 6 * _acPage;
                        goto END;
                    }
                }

                if( p_backButton && ( GET_AND_WAIT_C( fwdPos[ 0 ][ 0 ], fwdPos[ 0 ][ 1 ] + 2, 16 ) || GET_AND_WAIT( KEY_B ) ) ) { //Back pressed
                    result = -1;
                    goto END;
                } else if( ( !p_backButton && ( ( _num - 1 ) / 6 && _acPage == 0 && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] ) + sq( t.py - fwdPos[ 0 ][ 1 ] ) ) < 17 ) )
                           || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - fwdPos[ 1 ][ 0 ] ) + sq( t.py - fwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                    waitForTouchUp( );
                    if( ( ++_acPage ) >= ( ( _num - 1 ) / 6 ) ) {
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = true;
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = !_acPage;
                        ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ p_backButton ][ 0 ] - 16;
                        ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ p_backButton ][ 1 ] - 16;
                        if( _acPage == ( _num / 6 ) )
                            draw( NEW_PAGE );
                        _acPage = ( _num / 6 );
                    } else {
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                        draw( NEW_PAGE );
                    }
                } else if( ( !p_backButton && ( _acPage && _acPage == ( _num - 1 ) / 6 && sqrt( sq( t.px - bwdPos[ 0 ][ 0 ] ) + sq( t.py - bwdPos[ 0 ][ 1 ] ) ) < 17 ) )
                           || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - bwdPos[ 1 ][ 0 ] ) + sq( t.py - bwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {
                    waitForTouchUp( );
                    if( ( --_acPage ) <= 0 ) {
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = true;
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = !( _acPage < ( ( _num - 1 ) / 6 ) );
                        ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
                        if( _acPage == 0 )
                            draw( NEW_PAGE );
                        _acPage = 0;
                    } else {
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                        draw( NEW_PAGE );
                    }
                }
            }
        }
END:
        return result;
    }
}
