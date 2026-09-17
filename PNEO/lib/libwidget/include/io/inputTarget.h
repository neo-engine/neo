/*
Pokémon neo
------------------------------

file        : inputTarget.h
author      : Philip Wellnitz
description : Consult corresponding source file.

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
#pragma once
#include <functional>
#include <nds.h>

namespace IO {
    // num frames between button input
    constexpr u8 COOLDOWN_COUNT = 6;

    extern int           BTN_PRESSED, BTN_HELD, BTN_LAST;
    extern touchPosition TOUCH;
    extern u8            BTN_COOLDOWN;

#define TOUCH_UP ( !IO::TOUCH.px && !IO::TOUCH.py )
#define GET_KEY_COOLDOWN( p_key )     \
    ( ( IO::BTN_PRESSED & ( p_key ) ) \
      || ( ( IO::BTN_HELD & ( p_key ) ) && !( --IO::BTN_COOLDOWN ) ) )
#define GET_AND_WAIT( p_key ) \
    ( ( IO::BTN_PRESSED & ( p_key ) ) && IO::waitForInput( IO::inputTarget( p_key ) ) )

    constexpr u32 sq( s32 a ) {
        return u32( a * a );
    }

    /*
     * @brief: Returns a predicate to detect a rhombus with two edges parallel to the y
     * axis.
     *
     * (p_topX, p_topY)
     * |\   \
     * |_\  | 2 * p_height
     * \ |  |
     *  \|  /
     *
     * \_/
     * p_width
     */
    inline std::function<bool( touchPosition& )> touchVerticalRhombus( u16 p_topX, u16 p_topY,
                                                                       u16 p_width, u16 p_height ) {
        return [ = ]( touchPosition& p_touch ) {
            return ( p_touch.py >= p_topY && p_touch.py < p_topY + p_height && p_touch.px >= p_topX
                     && p_touch.px < p_topX + p_touch.py - p_topY )
                   || ( p_touch.py >= p_topY + p_height && p_touch.py < p_topY + 2 * p_height
                        && p_touch.px >= p_topX + p_touch.py - p_topY - p_height
                        && p_touch.px < p_topX + p_width );
        };
    }

    inline std::function<bool( touchPosition& )> touchCircle( u16 p_targetX1, u16 p_targetY1,
                                                              u16 p_targetR ) {
        return [ = ]( touchPosition& p_touch ) {
            return sq( p_touch.px - p_targetX1 ) + sq( p_touch.py - p_targetY1 ) <= sq( p_targetR );
        };
    }

    inline std::function<bool( touchPosition& )> touchRectangle( u16 p_targetX1, u16 p_targetY1,
                                                                 u16 p_targetX2, u16 p_targetY2 ) {
        return [ = ]( touchPosition& p_touch ) {
            return p_touch.px >= p_targetX1 && p_touch.py >= p_targetY1 && p_touch.px <= p_targetX2
                   && p_touch.py <= p_targetY2;
        };
    }

    struct touchInputTarget {
        std::function<bool( touchPosition& )> m_customTouch = 0;

        touchInputTarget( ) {
        }
        touchInputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 ) {
            m_customTouch = touchRectangle( p_targetX1, p_targetY1, p_targetX2, p_targetY2 );
        }
        touchInputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetR ) {
            m_customTouch = touchCircle( p_targetX1, p_targetY1, p_targetR );
        }
        touchInputTarget( std::function<bool( touchPosition& )> p_customTouch )
            : m_customTouch( p_customTouch ) {
        }

        inline bool inRange( touchPosition& p_touch ) const {
            if( m_customTouch ) { return m_customTouch( p_touch ); }
            return false;
        }
    };

    struct inputTarget {
        enum inputType { BUTTON, TOUCH, TOUCH_CIRCLE };
        inputType   m_inputType;
        KEYPAD_BITS m_keys;
        u16         m_targetX1 = 0;
        u16         m_targetY1 = 0;
        u16         m_targetX2 = 300;
        u16         m_targetY2 = 300;
        u16         m_targetR  = 16;

        inputTarget( KEYPAD_BITS p_keys ) : m_inputType( BUTTON ), m_keys( p_keys ) {
        }
        inputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetX2, u16 p_targetY2 )
            : m_inputType( TOUCH ), m_targetX1( p_targetX1 ), m_targetY1( p_targetY1 ),
              m_targetX2( p_targetX2 ), m_targetY2( p_targetY2 ) {
        }
        inputTarget( u16 p_targetX1, u16 p_targetY1, u16 p_targetR )
            : m_inputType( TOUCH_CIRCLE ), m_targetX1( p_targetX1 ), m_targetY1( p_targetY1 ),
              m_targetX2( 0 ), m_targetY2( 0 ), m_targetR( p_targetR ) {
        }

        constexpr bool inRange( touchPosition& p_touch ) const {
            if( !m_targetX1 && !m_targetY1 && !m_targetR && !m_targetX2 && !m_targetY2 ) {
                return false;
            }

            if( m_inputType == TOUCH ) {
                return p_touch.px >= m_targetX1 && p_touch.py >= m_targetY1
                       && p_touch.px <= m_targetX2 && p_touch.py <= m_targetY2;
            } else if( m_inputType == TOUCH_CIRCLE ) {
                return sq( p_touch.px - m_targetX1 ) + sq( p_touch.py - m_targetY1 )
                       <= sq( m_targetR );
            } else {
                return false;
            }
        }
    };

    bool waitForTouchUp( u16 p_targetX1 = 1, u16 p_targetY1 = 1, u16 p_targetX2 = 300,
                         u16 p_targetY2 = 300 );
    bool waitForTouchUp( inputTarget p_inputTarget );

    bool waitForKeysUp( KEYPAD_BITS p_keys );
    bool waitForKeysUp( inputTarget p_inputTarget );

    bool waitForInput( inputTarget p_inputTarget );

    /*
     * @brief: Waits until the player presses either A or B.
     */
    void waitForInteractAB( );

    void waitForInteract( std::function<void( u8 )> p_tick, std::function<void( )> p_sfxChoose );

    /*
     * @brief: Finishes the current frame and checks whether the given buttons were
     * IO::BTN_PRESSED.
     */
    bool interruptableFrame( int p_inter = 0 );

} // namespace IO
