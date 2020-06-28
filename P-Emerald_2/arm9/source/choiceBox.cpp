/*
Pokémon neo
------------------------------

file        : choiceBox.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "defines.h"
#include "choiceBox.h"
#include "uio.h"
#include "saveGame.h"
#include "sound.h"

#include "Back.h"
#include "Backward.h"
#include "Forward.h"

#include <algorithm>
#include <cmath>

namespace IO {

#define UPDATE_PAGE_STATS do { \
    for( auto g : choices ) { \
        if( g.second == BACK_CHOICE ) { back = true; } \
        if( g.second == EXIT_CHOICE ) { ext = true; } \
        if( g.second == PREV_PAGE_CHOICE ) { prvpg = true; } \
        if( g.second == NEXT_PAGE_CHOICE ) { nxtpg = true; } \
        if( g.second < 6 ) { mxchoice = std::max( u8( g.second + 1 ), mxchoice ); } \
    } \
} while( false )


    choiceBox::selection choiceBox::getResult(
            std::function<std::vector<std::pair<inputTarget, selection>>(u8)> p_drawFunction,
            std::function<void(selection)> p_selectFunction,
            selection p_initialSelection, std::function<void( )> p_tick ) {
        u8 page = 0;
        auto choices = p_drawFunction( page );
        if( !choices.size( ) ) [[unlikely]] { return BACK_CHOICE; }

        auto sel = p_initialSelection;
        p_selectFunction( sel );

        bool prvpg = false, nxtpg = false, back = false, ext = false;
        u8 mxchoice = 0;
        UPDATE_PAGE_STATS;

        cooldown     = COOLDOWN_COUNT;
        loop( ) {
            p_tick( );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_A ) {
                if( sel < choices.size( ) ) { SOUND::playSoundEffect( SFX_CHOOSE ); }
                else  { SOUND::playSoundEffect( SFX_CANCEL ); }
                cooldown = COOLDOWN_COUNT;
                break;
            }
            if( back && ( pressed & KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown  = COOLDOWN_COUNT;
                sel = choiceBox::BACK_CHOICE;
                p_selectFunction( sel );
                break;
            }
            if( ext && ( pressed & KEY_X ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown  = COOLDOWN_COUNT;
                sel = choiceBox::EXIT_CHOICE;
                p_selectFunction( sel );
                break;
            }

            if( _mode != MODE_UP_DOWN_SINGLE && GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    if( !( sel & 1 ) && prvpg ) {
                        // Switch to previous page
                        choices = p_drawFunction( --page );
                        UPDATE_PAGE_STATS;
                    }
                    sel ^= 1;
                }
                if( _mode == MODE_UP_DOWN && prvpg ) {
                    choices = p_drawFunction( --page );
                    UPDATE_PAGE_STATS;
                }
                p_selectFunction( sel );

                cooldown = COOLDOWN_COUNT;
            }
            if( _mode != MODE_UP_DOWN_SINGLE && GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    if( ( sel & 1 ) && nxtpg ) {
                        // Switch to previous page
                        choices = p_drawFunction( ++page );
                        UPDATE_PAGE_STATS;
                    }
                    if( ( sel ^ 1 ) < mxchoice ) {
                        sel ^= 1;
                    } else if( sel > 0 ) {
                        sel--;
                    }
                }
                if( _mode == MODE_UP_DOWN && nxtpg ) {
                    choices = p_drawFunction( ++page );
                    UPDATE_PAGE_STATS;
                    sel = std::min( sel, mxchoice );
                }
                p_selectFunction( sel );

                cooldown = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( _mode == MODE_UP_DOWN ) {
                    sel = ( sel + mxchoice - 1 ) % mxchoice;
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    // move to the pre option in the current column
                    if( sel < 2 ) {
                        sel = ( mxchoice - 1 - ( sel & 1 ) ) / 2 * 2 + ( sel & 1 );
                    } else {
                        sel -= 2;
                    }
                }
                p_selectFunction( sel );

                cooldown = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( _mode == MODE_UP_DOWN ) {
                    sel = ( sel + 1 ) % mxchoice;
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    // Move to the next option in the current column
                    if( sel + 2 >= mxchoice ) {
                        sel &= 1;
                    } else {
                        sel += 2;
                    }
                }
                p_selectFunction( sel );

                cooldown = COOLDOWN_COUNT;
            }

            for( auto i : choices ) {
                if( i.first.inRange( touch ) ) {
                    sel = i.second;
                    p_selectFunction( sel );
                    bool bad = false;
                    while( touch.px || touch.py ) {
                        swiWaitForVBlank( );
                        if( !i.first.inRange( touch ) ) {
                            bad = true;
                            break;
                        }
                        p_tick( );
                        scanKeys( );
                        touchRead( &touch );
                        swiWaitForVBlank( );
                    }
                    if( !bad ) {
                        if( sel == EXIT_CHOICE || sel == BACK_CHOICE ) {
                            SOUND::playSoundEffect( SFX_CANCEL );
                        } else {
                            SOUND::playSoundEffect( SFX_CHOOSE );
                        }
                        if( sel == NEXT_PAGE_CHOICE ) {
                            choices = p_drawFunction( ++page );
                            UPDATE_PAGE_STATS;
                            p_selectFunction( ( sel = 0 ) );
                            break;
                        } else if( sel == PREV_PAGE_CHOICE ) {
                            choices = p_drawFunction( --page );
                            UPDATE_PAGE_STATS;
                            p_selectFunction( ( sel = 0 ) );
                            break;
                        }
                        return sel;
                    }
                }
            }

            swiWaitForVBlank( );
        }
        return sel;
    }

    constexpr u8 NEW_PAGE = 9;
    void choiceBox::draw( u8 p_pressedIdx ) {
        if( p_pressedIdx == NEW_PAGE ) {
            if( _drawSub ) NAV::draw( );
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
        u8 endIdx   = std::min( _num, u8( ( ( !_big ) * 3 + 3 ) * ( _acPage + 1 ) ) );

        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
        BG_PALETTE_SUB[ RED_IDX ]   = GREEN;

        if( _big ) {
            for( u8 i = startIdx; i < endIdx; ++i ) {
                u8 acPos = i - startIdx;
                printChoiceBox( 32, 68 + acPos * 35, 192 + 32, 68 + 32 + acPos * 35, 6,
                                ( _selectedIdx == acPos ) ? RED_IDX : COLOR_IDX,
                                acPos == p_pressedIdx );
                regularFont->printString(
                    _choices[ i ], 32 + 2 * ( p_pressedIdx == acPos ) + 184 / 2,
                    78 + acPos * 35 + ( p_pressedIdx == acPos ), true, IO::font::CENTER );
                swiWaitForVBlank( );
            }
        } else {
            for( u8 i = startIdx; i < endIdx; ++i ) {
                u8 acPos = i - startIdx;
                printChoiceBox( ( ( acPos % 2 ) ? 129 : 19 ), 68 + ( acPos / 2 ) * 35,
                                106 + ( ( acPos % 2 ) ? 129 : 19 ), 32 + 68 + ( acPos / 2 ) * 35, 6,
                                ( _selectedIdx == acPos ) ? RED_IDX : COLOR_IDX,
                                acPos == p_pressedIdx );
                regularFont->printString(
                    _choices[ i ],
                    ( ( acPos % 2 ) ? 129 : 19 ) + 2 * ( p_pressedIdx == acPos ) + 102 / 2,
                    78 + ( acPos / 2 ) * 35 + ( p_pressedIdx == acPos ), true, IO::font::CENTER );
                swiWaitForVBlank( );
            }
        }
    }
    choiceBox::choiceBox( int p_num, const char** p_choices, const char* p_name = 0,
                          bool p_big = false ) {
        _num     = p_num;
        _choices = p_choices;
        _big     = p_big || ( p_num <= 3 );
        _acPage  = 0;
        _drawSub = true;

        initTextField( );

        initOAMTable( true );
        u16 tileCnt = 16;

        tileCnt = loadSprite( BACK_ID, 0, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32,
                              BackPal, BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0,
                              true );
        tileCnt = loadSprite( FWD_ID, 1, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32,
                              ForwardPal, ForwardTiles, ForwardTilesLen, false, false, true,
                              OBJPRIORITY_1, true );
        tileCnt = loadSprite( BWD_ID, 2, tileCnt, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32,
                              BackwardPal, BackwardTiles, BackwardTilesLen, false, false, true,
                              OBJPRIORITY_1, true );
        updateOAM( true );

        _name = p_name;

        swiWaitForVBlank( );
    }

    int fwdPos[ 2 ][ 2 ]
        = {{SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12}, {SCREEN_WIDTH - 11, SCREEN_HEIGHT - 31}},
        bwdPos[ 2 ][ 2 ]
        = {{SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12}, {SCREEN_WIDTH - 31, SCREEN_HEIGHT - 11}};

    void choiceBox::updateButtons( bool p_backButton ) {
        u8 pp = ( _num <= 3 || ( _num > 3 && _big ) ) ? 3 : 6;

        if( p_backButton ) {
            ( Oam->oamBuffer[ BACK_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ BACK_ID ] ).x        = fwdPos[ 0 ][ 0 ] - 16;
            ( Oam->oamBuffer[ BACK_ID ] ).y        = fwdPos[ 0 ][ 1 ] - 14;
        }
        if( ( _acPage + 1 ) * pp >= _num ) {
            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = true;
            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = !_acPage;
            ( Oam->oamBuffer[ BWD_ID ] ).x        = bwdPos[ p_backButton ][ 0 ] - 16;
            ( Oam->oamBuffer[ BWD_ID ] ).y        = bwdPos[ p_backButton ][ 1 ] - 16;
        } else if( !_acPage ) {
            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = true;
            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ FWD_ID ] ).x        = fwdPos[ p_backButton ][ 0 ] - 16;
            ( Oam->oamBuffer[ FWD_ID ] ).y        = fwdPos[ p_backButton ][ 1 ] - 16;
        } else {
            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ BWD_ID ] ).x        = bwdPos[ 1 ][ 0 ] - 16;
            ( Oam->oamBuffer[ BWD_ID ] ).y        = bwdPos[ 1 ][ 1 ] - 16;
            ( Oam->oamBuffer[ FWD_ID ] ).x        = fwdPos[ 1 ][ 0 ] - 16;
            ( Oam->oamBuffer[ FWD_ID ] ).y        = fwdPos[ 1 ][ 1 ] - 16;
        }
        updateOAM( true );
    }

    int choiceBox::getResult( const char* p_text, bool p_backButton, bool p_drawSub,
                              u8 p_initialSelection ) {
        _drawSub     = p_drawSub;
        _text        = p_text;
        _selectedIdx = p_initialSelection;
        int result   = -1;
        _acPage      = 0;
        updateButtons( p_backButton );
        draw( NEW_PAGE );

        if( _num < 1 )
            return -1;
        else if( _num <= 3 || ( _num > 3 && _big ) ) {
            loop( ) {
                swiWaitForVBlank( );
                updateOAM( true );
                touchPosition t;
                auto&         touch = t;
                touchRead( &t );
                scanKeys( );
                int pressed = keysCurrent( );

                for( u8 i = 0; i < 3; ++i )
                    if( ( i + 3 * _acPage ) < _num && t.px >= 32 && t.py >= 68 + 35 * i
                        && t.px <= 224 && t.py <= 100 + 35 * i ) {
                        draw( i );
                        if( !waitForTouchUp( 32, 68 + 35 * i, 224, 100 + 35 * i ) ) {
                            draw( 8 );
                            break;
                        }
                        result = i + 3 * _acPage;
                        goto END;
                    }
                if( GET_AND_WAIT( KEY_DOWN ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
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
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( _selectedIdx == (u8) -1 )
                        _selectedIdx = 0;
                    else if( !_selectedIdx ) {
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
                if( p_backButton
                    && (
                        /* GET_AND_WAIT_C( fwdPos[ 0 ][ 0 ], fwdPos[ 0 ][ 1 ] + 2, 16 )
                         || */ GET_AND_WAIT( KEY_B ) ) ) { // Back pressed
                    result = -1;
                    goto END;
                }
                if( _num > 3 ) {
                    if( /*( !IO::Oam->oamBuffer[ FWD_ID ].isHidden
                          && GET_AND_WAIT_C( IO::Oam->oamBuffer[ FWD_ID ].x + 16,
                                             IO::Oam->oamBuffer[ FWD_ID ].y + 16, 16 ) )
                        || */ GET_AND_WAIT( KEY_RIGHT ) ) {
                        if( ( _acPage + 1 ) * 3 < _num ) {
                            ++_acPage;
                            while( _selectedIdx + 3 * _acPage >= _num ) --_selectedIdx;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        }
                    } else if( /*( !IO::Oam->oamBuffer[ BWD_ID ].isHidden
                                 && GET_AND_WAIT_C( IO::Oam->oamBuffer[ BWD_ID ].x + 16,
                                                    IO::Oam->oamBuffer[ BWD_ID ].y + 16, 16 ) )
                               ||*/ GET_AND_WAIT( KEY_LEFT ) ) {
                        if( _acPage ) {
                            --_acPage;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        }
                    }
                }
            }
        } else {
            loop( ) {
                swiWaitForVBlank( );
                updateOAM( true );
                touchPosition touch;
                auto&         t = touch;
                touchRead( &touch );
                scanKeys( );
                int pressed = keysCurrent( );

                for( u8 i = 0; i < std::min( 6, _num - 6 * _acPage ); ++i ) {
                    if( t.px >= ( ( i % 2 ) ? 129 : 19 ) && t.py >= 68 + ( i / 2 ) * 35
                        && t.px <= 106 + ( ( i % 2 ) ? 129 : 19 )
                        && t.py <= 32 + 68 + ( i / 2 ) * 35 ) {
                        draw( i );

                        if( !waitForTouchUp( ( ( i % 2 ) ? 129 : 19 ), 68 + ( i / 2 ) * 35,
                                             106 + ( ( i % 2 ) ? 129 : 19 ),
                                             32 + 68 + ( i / 2 ) * 35 ) ) {
                            draw( 8 );
                            break;
                        }
                        result = i + 6 * _acPage;
                        goto END;
                    }
                }
                if( _selectedIdx != (u8) -1 && GET_AND_WAIT( KEY_A ) ) {
                    result = _selectedIdx + 3 * _acPage;
                    goto END;
                }
                if( GET_AND_WAIT( KEY_DOWN ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( _selectedIdx == (u8) -1 )
                        _selectedIdx = 0;
                    else if( _selectedIdx >= 4 || _selectedIdx + 2 + _acPage * 6 >= _num ) {
                        if( _selectedIdx % 2 ) {
                            if( ( _acPage + 1 ) * 6 < _num ) {
                                ++_acPage;
                                _selectedIdx = 0;
                                draw( NEW_PAGE );
                                updateButtons( p_backButton );
                            }
                            continue;
                        } else
                            _selectedIdx = 1;
                    } else
                        _selectedIdx += 2;
                    draw( -1 );
                }
                if( GET_AND_WAIT( KEY_UP ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( _selectedIdx == (u8) -1 )
                        _selectedIdx = 0;
                    else if( _selectedIdx < 2 ) {
                        if( !_selectedIdx ) {
                            if( _acPage ) {
                                --_acPage;
                                _selectedIdx = 5;
                                draw( NEW_PAGE );
                                updateButtons( p_backButton );
                                continue;
                            }
                        } else
                            _selectedIdx = 4;
                    } else
                        _selectedIdx -= 2;
                    draw( -1 );
                }

                if( GET_AND_WAIT( KEY_RIGHT ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( _selectedIdx == (u8) -1 )
                        _selectedIdx = 0;
                    else if( _selectedIdx % 2 ) {
                        if( ( _acPage + 1 ) * 6 < _num ) {
                            ++_acPage;
                            _selectedIdx--;
                            while( _selectedIdx + 6 * _acPage >= _num ) _selectedIdx -= 2;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                        }
                        continue;
                    } else {
                        _selectedIdx++;
                        if( _selectedIdx + 6 * _acPage >= _num ) _selectedIdx--;
                    }
                    draw( -1 );
                }
                if( GET_AND_WAIT( KEY_LEFT ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( _selectedIdx == (u8) -1 )
                        _selectedIdx = 0;
                    else if( _selectedIdx % 2 == 0 ) {
                        if( _acPage ) {
                            --_acPage;
                            _selectedIdx++;
                            draw( NEW_PAGE );
                            updateButtons( p_backButton );
                            continue;
                        }
                    } else
                        _selectedIdx--;
                    draw( -1 );
                }

                /*
                if( p_backButton
                    &&  (  GET_AND_WAIT_C( fwdPos[ 0 ][ 0 ], fwdPos[ 0 ][ 1 ] + 2, 16 )
                         || GET_AND_WAIT( KEY_B ) ) ) { // Back pressed
                    result = -1;
                    goto END;
                } else if( !IO::Oam->oamBuffer[ FWD_ID ].isHidden
                           && GET_AND_WAIT_C( IO::Oam->oamBuffer[ FWD_ID ].x + 16,
                                              IO::Oam->oamBuffer[ FWD_ID ].y + 16, 16 ) ) {
                    if( ( _acPage + 1 ) * 6 < _num ) {
                        ++_acPage;
                        while( _selectedIdx + 6 * _acPage >= _num ) --_selectedIdx;
                        draw( NEW_PAGE );
                        updateButtons( p_backButton );
                    }
                } else if( !IO::Oam->oamBuffer[ BWD_ID ].isHidden
                           && GET_AND_WAIT_C( IO::Oam->oamBuffer[ BWD_ID ].x + 16,
                                              IO::Oam->oamBuffer[ BWD_ID ].y + 16, 16 ) ) {
                    waitForTouchUp( );
                    if( _acPage ) {
                        --_acPage;
                        draw( NEW_PAGE );
                        updateButtons( p_backButton );
                    }
                }
                         */
            }
        }
    END:
        if( result >= 0 ) {
            SOUND::playSoundEffect( SFX_CHOOSE );
        } else {
            SOUND::playSoundEffect( SFX_CANCEL );
        }
        return result;
    }
}
