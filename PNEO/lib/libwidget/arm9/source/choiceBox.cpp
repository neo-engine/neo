/*
Pokémon neo
------------------------------

file        : choiceBox.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2026
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

#include <algorithm>
#include <cmath>

#include "io/choiceBox.h"

namespace IO {

#define UPDATE_PAGE_STATS                                                               \
    do {                                                                                \
        back = ext = prvpg = nxtpg = false;                                             \
        mxchoice                   = 0;                                                 \
        for( auto g : choices ) {                                                       \
            if( g.second == BACK_CHOICE ) { back = true; }                              \
            if( g.second == EXIT_CHOICE ) { ext = true; }                               \
            if( g.second == PREV_PAGE_CHOICE ) { prvpg = true; }                        \
            if( g.second == NEXT_PAGE_CHOICE ) { nxtpg = true; }                        \
            if( g.second < 6 ) { mxchoice = std::max( u8( g.second + 1 ), mxchoice ); } \
        }                                                                               \
    } while( false )

    choiceBox::selection choiceBox::getResult(
        std::function<std::vector<std::pair<inputTarget, selection>>( u8 )> p_drawFunction,
        std::function<void( selection )> p_selectFunction, selection p_initialSelection,
        std::function<void( )> p_tick, u8 p_initialPage, std::function<void( )> p_sfxCancel,
        std::function<void( )> p_sfxChoose, std::function<void( )> p_sfxSelect ) {
        u8   page    = p_initialPage;
        auto choices = p_drawFunction( page );
        if( !choices.size( ) ) [[unlikely]] { return BACK_CHOICE; }

        auto sel = p_initialSelection;
        p_selectFunction( sel );

        bool prvpg = false, nxtpg = false, back = false, ext = false;
        u8   mxchoice = 0;
        UPDATE_PAGE_STATS;

        BTN_COOLDOWN = COOLDOWN_COUNT;
        while( 1 ) {
            swiWaitForVBlank( );
            p_tick( );
            scanKeys( );
            touchRead( &TOUCH );
            BTN_PRESSED = keysUp( );
            BTN_HELD    = keysHeld( );

            // key controls
            if( BTN_PRESSED & KEY_A ) {
                if( sel < choices.size( ) ) {
                    if( choices[ sel ].second == choiceBox::BACK_CHOICE
                        || choices[ sel ].second == choiceBox::EXIT_CHOICE ) {
                        p_sfxCancel( );
                        break;
                    } else if( choices[ sel ].second == choiceBox::DISABLED_CHOICE ) {
                        // Choice is disabled, nothing happens
                    } else {
                        p_sfxChoose( );
                        break;
                    }
                } else {
                    p_sfxCancel( );
                    break;
                }
                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( back && ( BTN_PRESSED & KEY_B ) ) {
                p_sfxCancel( );
                BTN_COOLDOWN = COOLDOWN_COUNT;
                sel          = choiceBox::BACK_CHOICE;
                if( _mode != MODE_UP_DOWN_LEFT_RIGHT_CANCEL ) { p_selectFunction( sel ); }
                break;
            }
            if( ext && ( BTN_PRESSED & KEY_X ) ) {
                p_sfxCancel( );
                BTN_COOLDOWN = COOLDOWN_COUNT;
                sel          = choiceBox::EXIT_CHOICE;
                p_selectFunction( sel );
                break;
            }

            if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                p_sfxSelect( );

                if( _mode == MODE_LEFT_RIGHT ) {
                    sel = ( sel + mxchoice - 1 ) % mxchoice;
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT_CANCEL ) {
                    if( ( sel ^ 1 ) < mxchoice ) { sel ^= 1; }
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    if( !( sel & 1 ) && prvpg ) {
                        // Switch to previous page
                        choices = p_drawFunction( --page );
                        UPDATE_PAGE_STATS;
                    }
                    if( ( sel ^ 1 ) < mxchoice ) { sel ^= 1; }
                } else if( _mode == MODE_UP_DOWN && prvpg ) {
                    choices = p_drawFunction( --page );
                    UPDATE_PAGE_STATS;
                } else {
                    BTN_COOLDOWN = COOLDOWN_COUNT;
                    continue;
                }
                p_selectFunction( sel );

                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                p_sfxSelect( );

                if( _mode == MODE_LEFT_RIGHT ) {
                    sel = ( sel + 1 ) % mxchoice;
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT_CANCEL ) {
                    if( ( sel ^ 1 ) < mxchoice ) {
                        sel ^= 1;
                    } else if( sel > 0 ) {
                        sel = ( ( mxchoice - 1 ) >> 1 ) << 1;
                    }
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    if( ( sel & 1 ) && nxtpg ) {
                        // Switch to next page
                        choices = p_drawFunction( ++page );
                        UPDATE_PAGE_STATS;
                    }
                    if( ( sel ^ 1 ) < mxchoice ) {
                        sel ^= 1;
                    } else if( sel > 0 ) {
                        sel = ( ( mxchoice - 1 ) >> 1 ) << 1;
                    }
                } else if( _mode == MODE_UP_DOWN && nxtpg ) {
                    choices = p_drawFunction( ++page );
                    UPDATE_PAGE_STATS;
                    sel = std::min( sel, mxchoice );
                } else {
                    BTN_COOLDOWN = COOLDOWN_COUNT;
                    continue;
                }
                p_selectFunction( sel );

                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                p_sfxSelect( );

                if( _mode == MODE_UP_DOWN ) {
                    sel = ( sel + mxchoice - 1 ) % mxchoice;
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT_CANCEL ) {
                    if( sel < 2 ) {
                        sel = 4;
                    } else if( sel < 5 ) {
                        sel -= 2;
                    }
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    // move to the pre option in the current column
                    if( sel < 2 ) {
                        sel = ( mxchoice - 1 - ( sel & 1 ) ) / 2 * 2 + ( sel & 1 );
                    } else {
                        sel -= 2;
                    }
                }
                p_selectFunction( sel );

                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                p_sfxSelect( );

                if( _mode == MODE_UP_DOWN ) {
                    sel = ( sel + 1 ) % mxchoice;
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT_CANCEL ) {
                    if( sel < 3 ) {
                        sel += 2;
                    } else if( sel < 4 ) {
                        sel = 4;
                    } else if( sel == 4 ) {
                        sel = 0;
                    }
                } else if( _mode == MODE_UP_DOWN_LEFT_RIGHT ) {
                    // Move to the next option in the current column
                    if( sel + 2 >= mxchoice ) {
                        sel &= 1;
                    } else {
                        sel += 2;
                    }
                }
                p_selectFunction( sel );

                BTN_COOLDOWN = COOLDOWN_COUNT;
            }

            // touch controls
            for( auto i : choices ) {
                if( i.first.inRange( TOUCH ) ) {
                    sel = i.second;
                    p_selectFunction( sel );
                    bool bad = false;
                    while( TOUCH.px || TOUCH.py ) {
                        if( !i.first.inRange( TOUCH ) ) {
                            bad = true;
                            break;
                        }
                        swiWaitForVBlank( );
                        p_tick( );
                        scanKeys( );
                        touchRead( &TOUCH );
                    }
                    if( !bad && sel != DISABLED_CHOICE ) {
                        if( sel == EXIT_CHOICE || sel == BACK_CHOICE ) {
                            p_sfxCancel( );
                        } else {
                            p_sfxChoose( );
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
        }
        return sel;
    }
} // namespace IO
