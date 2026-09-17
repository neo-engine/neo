/*
Pokémon neo
------------------------------

file        : counter.cpp
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

#include "io/counter.h"

namespace IO {
    s32
    counter::getResult( std::function<std::vector<std::pair<inputTarget, s32>>( )> p_drawFunction,
                        std::function<void( s32, u8 )>                             p_updateValue,
                        std::function<void( s32 )> p_hoverButton, s32 p_initialValue,
                        std::function<void( )> p_tick, std::function<void( )> p_sfxCancel,
                        std::function<void( )> p_sfxChoose, std::function<void( )> p_sfxSelect ) {

        s32 value = p_initialValue;
        u8  dig   = 0;
        s32 df    = 1;
        u8  mxdg  = 0;
        for( s32 i = _maxValue; i > 0; i /= 10, mxdg++, df *= 10 ) {}
        df /= 10;

        auto choices = p_drawFunction( );
        if( !choices.size( ) ) [[unlikely]] { return 0; }

        bool back = false, exit = false;
        for( auto i : choices ) {
            if( !i.second ) { back = true; }
            if( i.second == _minValue - 3 ) { exit = true; }
        }

        p_updateValue( value, 0 );

        BTN_COOLDOWN = COOLDOWN_COUNT;
        while( 1 ) {
            swiWaitForVBlank( );
            p_tick( );
            scanKeys( );
            touchRead( &TOUCH );
            BTN_PRESSED = keysUp( );
            BTN_HELD    = keysHeld( );

            if( BTN_PRESSED & KEY_A ) {
                p_sfxChoose( );
                BTN_COOLDOWN = COOLDOWN_COUNT;
                p_hoverButton( _minValue - 2 );
                break;
            }
            if( back && ( BTN_PRESSED & KEY_B ) ) {
                p_sfxCancel( );
                BTN_COOLDOWN = COOLDOWN_COUNT;
                p_updateValue( value = 0, dig = 0 );
                p_hoverButton( 0 );
                break;
            }
            if( exit && ( BTN_PRESSED & KEY_X ) ) {
                p_sfxCancel( );
                BTN_COOLDOWN = COOLDOWN_COUNT;
                p_updateValue( value = 0, dig = 0 );
                p_hoverButton( value = _minValue - 3 );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_LEFT ) ) { // move to pre digit
                p_sfxSelect( );

                if( dig > 0 ) {
                    dig--;
                    df *= 10;
                }
                p_updateValue( value, dig );
                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                p_sfxSelect( );

                if( dig + 1 < mxdg ) {
                    dig++;
                    df /= 10;
                }
                p_updateValue( value, dig );
                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                p_sfxSelect( );

                if( value <= _maxValue - df ) {
                    p_updateValue( value += df, dig );
                } else if( value == _maxValue ) {
                    p_updateValue( value = _minValue, dig );
                } else {
                    p_updateValue( value = _maxValue, dig );
                }
                BTN_COOLDOWN = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                p_sfxSelect( );
                if( value >= _minValue + df ) {
                    p_updateValue( value -= df, dig );
                } else if( value == _minValue ) {
                    p_updateValue( value = _maxValue, dig );
                } else {
                    p_updateValue( value = _minValue, dig );
                }
                BTN_COOLDOWN = COOLDOWN_COUNT;
            }

            // touch controls
            for( auto i : choices ) {
                if( i.first.inRange( TOUCH ) ) {
                    s32 df2 = i.second;
                    p_hoverButton( df2 );
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
                    if( !bad ) {
                        if( !df2 || df2 == _minValue - 3 ) {
                            p_sfxCancel( );
                            return 0;
                        } else if( df2 == _minValue - 2 ) {
                            p_sfxChoose( );
                            return value;
                        } else {
                            p_sfxSelect( );
                        }

                        dig = 0;
                        for( s32 tmp = std::abs( df2 ); tmp > 0; tmp /= 10, ++dig ) {}
                        dig = mxdg - dig;

                        if( df2 > 0 ) {
                            if( value <= _maxValue - df2 ) {
                                p_updateValue( value += df2, dig );
                            } else if( value == _maxValue ) {
                                p_updateValue( value = _minValue, dig );
                            } else {
                                p_updateValue( value = _maxValue, dig );
                            }
                        } else if( df2 < 0 ) {
                            if( value >= _minValue - df2 ) {
                                p_updateValue( value += df2, dig );
                            } else if( value == _minValue ) {
                                p_updateValue( value = _maxValue, dig );
                            } else {
                                p_updateValue( value = _minValue, dig );
                            }
                        }
                    }
                    p_hoverButton( _maxValue + 1 );
                }
            }
        }
        return value;
    }
} // namespace IO
