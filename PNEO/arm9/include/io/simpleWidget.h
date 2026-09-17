/*
Pokémon neo
------------------------------

file        : simpleWidget.h
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

#include <string>
#include <nds.h>

#include <io/choiceBox.h>
#include <io/counter.h>
#include <io/yesNoBox.h>
#include "defines.h"
#include "pokemon.h"

namespace IO {
    /*
     * @brief: Makes the player select one out of a set of different choices.
     */
    class simpleChoiceBox : public choiceBox {
      public:
        simpleChoiceBox( mode p_mode = MODE_UP_DOWN ) : choiceBox{ p_mode } {
        }

        /*
         * @brief: Simplified choice box using the default drawing functions from the NAV
         * namespace. (Works only while the player is in the OW and no menu/application is currently
         * opened.)
         */
        selection getResult( const char* p_message, style p_style,
                             const std::vector<u16>& p_choices, bool p_showExitButton = false );

        /*
         * @brief: Simplified choice box using the default drawing functions from the NAV
         * namespace. (Works only while the player is in the OW and no menu/application is currently
         * opened.)
         */
        selection getResult( const char* p_message, style p_style,
                             const std::vector<std::string>& p_choices,
                             bool                            p_showExitButton = false );

        /*
         * @brief: Makes the player select one from 5 moves.
         */
        selection getResult( const char* p_message, style p_style, u16 p_moves[ 4 ],
                             u16 p_extraMove );
    };

    /*
     * @brief: Makes the player select a numeric value.
     */
    class simpleCounter : public counter {
      public:
        simpleCounter( s32 p_minValue, s32 p_maxValue ) : counter{ p_minValue, p_maxValue } {
        }

        /*
         * @brief: A simplified counter using the default (overworld) message printing
         * system (from the NAV namespace).
         */
        s32 getResult( const char* p_message, style p_style );
    };

    /*
     * @brief: Runs a message box with a YES and a NO choice.
     */
    class simpleYesNoBox : public yesNoBox {
      public:
        simpleYesNoBox( ) : yesNoBox{ } {
        }

        /*
         * @brief: A simplified yes/no-box that uses the message output system (from the NAV
         * namespace) when the player is in the overworld and no special menu is currently visible.
         * @param p_showMoney: Display the current money the player owns; used for shop
         * dialogs (i.e. "Do you want to buy/pay <..>?").
         */
        selection getResult( const char* p_message, style p_style, bool p_showMoney = false );
    };

} // namespace IO
