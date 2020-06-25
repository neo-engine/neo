/*
Pokémon neo
------------------------------

file        : yesNoBox.cpp
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

#include "yesNoBox.h"
#include "defines.h"
#include "uio.h"
#include "sound.h"

namespace IO {
    yesNoBox::selection yesNoBox::getResult(
            std::function<std::vector<std::pair<inputTarget, selection>>()> p_drawFunction,
            std::function<void(selection)> p_selectFunction,
            selection p_initialSelection, std::function<void( )> p_tick ) {
        auto choices = p_drawFunction( );

        auto sel = p_initialSelection;
        p_selectFunction( sel );

        cooldown     = COOLDOWN_COUNT;
        loop( ) {
            p_tick( );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_A ) {
                if( sel == yesNoBox::YES ) { SOUND::playSoundEffect( SFX_CHOOSE ); }
                else { SOUND::playSoundEffect( SFX_CANCEL ); }
                cooldown = COOLDOWN_COUNT;
                break;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                cooldown  = COOLDOWN_COUNT;
                sel = yesNoBox::NO;
                p_selectFunction( sel );
                break;
            }
            if( GET_KEY_COOLDOWN( KEY_RIGHT ) || GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( sel == yesNoBox::YES ) { sel = yesNoBox::NO; }
                else if( sel == yesNoBox::NO ) { sel = yesNoBox::YES; }

                p_selectFunction( sel );

                cooldown = COOLDOWN_COUNT;
            }

            for( auto i : choices ) {
                if( IN_RANGE_I( touch, i.first ) ) {
                    sel = i.second;
                    p_selectFunction( sel );
                    bool bad = false;
                    while( touch.px || touch.py ) {
                        swiWaitForVBlank( );
                        if( !IN_RANGE_I( touch, i.first ) ) {
                            bad = true;
                            break;
                        }
                        p_tick( );
                        scanKeys( );
                        touchRead( &touch );
                        swiWaitForVBlank( );
                    }
                    if( !bad ) {
                        if( sel == yesNoBox::YES ) { SOUND::playSoundEffect( SFX_CHOOSE ); }
                        else { SOUND::playSoundEffect( SFX_CANCEL ); }
                        return sel;
                    }
                }
            }

            swiWaitForVBlank( );
        }
        return sel;
    }

    void yesNoBox::draw( u8 p_pressedIdx, u8 p_selectedIdx ) {
        if( p_selectedIdx == 1 )
            BG_PALETTE_SUB[ COLOR_IDX ] = BLUE2;
        else if( p_selectedIdx == 0 )
            BG_PALETTE_SUB[ COLOR_IDX ] = RED2;

        printChoiceBox( 28, 102, 122, 134, 6, ( p_selectedIdx == 1 ) ? COLOR_IDX : BLUE_IDX,
                        p_pressedIdx == 0 );
        regularFont->printString( STRINGS[ 80 ][ _language ], 28 + 47 + 2 * ( p_pressedIdx == 0 ),
                                  110 + ( p_pressedIdx == 0 ), true, IO::font::CENTER );
        printChoiceBox( 134, 102, 228, 134, 6, ( p_selectedIdx == 0 ) ? COLOR_IDX : RED_IDX,
                        p_pressedIdx == 1 );
        regularFont->printString( STRINGS[ 81 ][ _language ], 134 + 47 + 2 * ( p_pressedIdx == 1 ),
                                  110 + ( p_pressedIdx == 1 ), true, IO::font::CENTER );
    }

    yesNoBox::yesNoBox( bool p_initSprites ) {
        initTextField( );
        if( p_initSprites ) initOAMTable( true );
        _isNamed  = false;
        _language = SAVE::SAV.getActiveFile( ).m_options.m_language;
    }
    yesNoBox::yesNoBox( SAVE::language p_language, bool p_initSprites ) {
        initTextField( );
        if( p_initSprites ) initOAMTable( true );
        _isNamed  = false;
        _language = p_language;
    }
    yesNoBox::yesNoBox( const char* p_name, bool p_initSprites ) {
        initTextField( );
        if( p_initSprites ) initOAMTable( true );
        regularFont->printString( p_name, 8, 8, true );

        swiWaitForVBlank( );
        _isNamed  = true;
        _language = SAVE::SAV.getActiveFile( ).m_options.m_language;
    }
    yesNoBox::yesNoBox( messageBox p_box, bool p_initSprites ) {
        initTextField( );
        if( p_initSprites ) initOAMTable( true );
        _isNamed  = p_box.m_isNamed;
        _language = SAVE::SAV.getActiveFile( ).m_options.m_language;
    }

    bool yesNoBox::getResult( const char* p_text, bool p_textAtOnce ) {
        s16 x = 8 + 64 * !!_isNamed;
        s16 y = 8;
        if( p_text && !p_textAtOnce )
            regularFont->printStringD( p_text, x, y, true );
        else if( p_text )
            regularFont->printString( p_text, x, y, true );

        u8 selIdx = (u8) -1;
        draw( 2, selIdx );
        bool result;
        loop( ) {
            swiWaitForVBlank( );
            touchPosition t;
            touchRead( &t );
            scanKeys( );
            int pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_LEFT ) ) {
                selIdx = 0;
                draw( 2, selIdx );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                selIdx = 1;
                draw( 2, selIdx );
            } else if( selIdx != (u8) -1 && GET_AND_WAIT( KEY_A ) ) {
                result = !selIdx;
                break;
            }

            if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
                draw( 0, selIdx );
                if( !waitForTouchUp( 28, 102, 122, 134 ) ) {
                    draw( 2, selIdx );
                    continue;
                }
                draw( 2, selIdx );
                swiWaitForVBlank( );
                result = true;
                break;
            } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
                draw( 1, selIdx );
                if( !waitForTouchUp( 134, 102, 228, 134 ) ) {
                    draw( 2, selIdx );
                    continue;
                }
                draw( 2, selIdx );
                swiWaitForVBlank( );
                result = false;
                break;
            }
        }
        return result;
    }
} // namespace IO
