/*
Pokémon neo
------------------------------

file        : simpleWidget.cpp
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

#include <algorithm>
#include <cmath>

#include "defines.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/simpleWidget.h"
#include "io/util.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace IO {

    choiceBox::selection simpleChoiceBox::getResult( const char* p_message, style p_style,
                                                     const std::vector<u16>& p_choices,
                                                     bool                    p_showExitButton ) {
        return choiceBox::getResult(
            [ & ]( u8 ) {
                return printChoiceMessage( p_message, p_style, p_choices, p_showExitButton );
            },
            [ & ]( u8 p_selection ) { printChoiceMessage( 0, p_style, p_choices, p_selection ); },
            0, choiceBox::DEFAULT_TICK, 0, [ & ]( ) { SOUND::playSoundEffect( SFX_CANCEL ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_SELECT ); } );
    }

    choiceBox::selection simpleChoiceBox::getResult( const char* p_message, style p_style,
                                                     const std::vector<std::string>& p_choices,
                                                     bool p_showExitButton ) {
        return choiceBox::getResult(
            [ & ]( u8 ) {
                return printChoiceMessage( p_message, p_style, p_choices, p_showExitButton );
            },
            [ & ]( u8 p_selection ) { printChoiceMessage( 0, p_style, p_choices, p_selection ); },
            0, choiceBox::DEFAULT_TICK, 0, [ & ]( ) { SOUND::playSoundEffect( SFX_CANCEL ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_SELECT ); } );
    }

    choiceBox::selection simpleChoiceBox::getResult( const char* p_message, style p_style,
                                                     u16 p_moves[ 4 ], u16 p_extraMove ) {
        return choiceBox::getResult(
            [ & ]( u8 ) { return printChoiceMessage( p_message, p_style, p_moves, p_extraMove ); },
            [ & ]( u8 p_selection ) {
                printChoiceMessage( 0, p_style, p_moves, p_extraMove, p_selection );
            },
            0, choiceBox::DEFAULT_TICK, 0, [ & ]( ) { SOUND::playSoundEffect( SFX_CANCEL ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_SELECT ); } );
    }

    s32 simpleCounter::getResult( const char* p_message, style p_style ) {
        u8 mxdg = 0;
        for( s32 i = counter::maxValue( ); i > 0; i /= 10, mxdg++ ) {}

        return counter::getResult(
            [ & ]( ) {
                IO::printMessage( p_message, p_style );
                return IO::drawCounter( counter::minValue( ), counter::maxValue( ) );
            },
            [ & ]( s32 p_value, u8 p_selDigit ) {
                IO::updateCounterValue( p_value, p_selDigit, mxdg );
            },
            [ & ]( s32 p_button ) {
                IO::hoverCounterButton( counter::minValue( ), counter::maxValue( ), p_button );
            },
            counter::minValue( ), counter::DEFAULT_TICK,
            [ & ]( ) { SOUND::playSoundEffect( SFX_CANCEL ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_SELECT ); } );
    }

    yesNoBox::selection simpleYesNoBox::getResult( const char* p_message, style p_style,
                                                   bool p_showMoney ) {
        return yesNoBox::getResult(
            [ & ]( ) { return IO::printYNMessage( p_message, p_style, 255, p_showMoney ); },
            [ & ]( yesNoBox::selection p_selection ) {
                IO::printYNMessage( 0, p_style, p_selection == IO::yesNoBox::NO, p_showMoney );
            },
            IO::yesNoBox::selection::YES, yesNoBox::DEFAULT_TICK,
            [ & ]( ) { SOUND::playSoundEffect( SFX_CANCEL ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_CHOOSE ); },
            [ & ]( ) { SOUND::playSoundEffect( SFX_SELECT ); } );
    }

} // namespace IO
