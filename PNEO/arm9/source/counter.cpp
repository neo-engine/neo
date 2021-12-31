/*
Pokémon neo
------------------------------

file        : counter.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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

#include "counter.h"
#include "defines.h"
#include "nav.h"
#include "saveGame.h"
#include "sound.h"
#include "uio.h"

#include <algorithm>
#include <cmath>

namespace IO {
    s32 counter::getResult( const char* p_message, style p_style ) {
        u8 mxdg = 0;
        for( s32 i = _maxValue; i > 0; i /= 10, mxdg++ ) {}

        return getResult(
            [ & ]( ) {
                NAV::printMessage( p_message, p_style );
                return NAV::drawCounter( _minValue, _maxValue );
            },
            [ & ]( s32 p_value, u8 p_selDigit ) {
                NAV::updateCounterValue( p_value, p_selDigit, mxdg );
            },
            [ & ]( s32 p_button ) { NAV::hoverCounterButton( _minValue, _maxValue, p_button ); },
            _minValue );
    }

    s32
    counter::getResult( std::function<std::vector<std::pair<inputTarget, s32>>( )> p_drawFunction,
                        std::function<void( s32, u8 )>                             p_updateValue,
                        std::function<void( s32 )> p_hoverButton, s32 p_initialValue,
                        std::function<void( )> p_tick ) {

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

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            p_tick( );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_A ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                p_hoverButton( _minValue - 2 );
                break;
            }
            if( back && ( pressed & KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown = COOLDOWN_COUNT;
                p_updateValue( value = 0, dig = 0 );
                p_hoverButton( 0 );
                break;
            }
            if( exit && ( pressed & KEY_X ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown = COOLDOWN_COUNT;
                p_updateValue( value = 0, dig = 0 );
                p_hoverButton( value = _minValue - 3 );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_LEFT ) ) { // move to pre digit
                SOUND::playSoundEffect( SFX_SELECT );

                if( dig > 0 ) {
                    dig--;
                    df *= 10;
                }
                p_updateValue( value, dig );
                cooldown = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( dig + 1 < mxdg ) {
                    dig++;
                    df /= 10;
                }
                p_updateValue( value, dig );
                cooldown = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( value <= _maxValue - df ) {
                    p_updateValue( value += df, dig );
                } else if( value == _maxValue ) {
                    p_updateValue( value = _minValue, dig );
                } else {
                    p_updateValue( value = _maxValue, dig );
                }
                cooldown = COOLDOWN_COUNT;
            }
            if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( value >= _minValue + df ) {
                    p_updateValue( value -= df, dig );
                } else if( value == _minValue ) {
                    p_updateValue( value = _maxValue, dig );
                } else {
                    p_updateValue( value = _minValue, dig );
                }
                cooldown = COOLDOWN_COUNT;
            }

            for( auto i : choices ) {
                if( i.first.inRange( touch ) ) {
                    s32 df2 = i.second;
                    p_hoverButton( df2 );
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
                        if( !df2 || df2 == _minValue - 3 ) {
                            SOUND::playSoundEffect( SFX_CANCEL );
                            return 0;
                        } else if( df2 == _minValue - 2 ) {
                            SOUND::playSoundEffect( SFX_CHOOSE );
                            return value;
                        } else {
                            SOUND::playSoundEffect( SFX_SELECT );
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

            swiWaitForVBlank( );
        }
        return value;
    }
} // namespace IO
