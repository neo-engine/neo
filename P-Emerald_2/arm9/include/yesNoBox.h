/*
Pok�mon neo
------------------------------

file        : yesNoBox.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2020
Philip Wellnitz

This file is part of Pok�mon neo.

Pok�mon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pok�mon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pok�mon neo.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <functional>
#include <vector>
#include <nds.h>

#include "uio.h"
#include "messageBox.h"
#include "saveGame.h"

namespace IO {
    /*
     * @brief: Runs a message box with a YES and a NO choice.
     */
    class yesNoBox {
      public:
        static void DEFAULT_TICK( ) { }

        enum selection {
            YES = 0,
            NO = 1
        };

        yesNoBox( ) { }
        /*
         * @brief: Opens a yesNoBox and returns the player's selection.
         * @param p_drawFunction: Callback used to draw the yesNoBox
         * @param p_selectFunction: Callback used when the player selects a choice.
         */
        selection getResult( std::function<std::vector<std::pair<inputTarget, selection>>()>
                             p_drawFunction, std::function<void(selection)> p_selectFunction,
                             selection p_initialSelection = YES,
                             std::function<void( )> p_tick = DEFAULT_TICK );

        [[deprecated]]
        yesNoBox( bool p_initSprites );

        [[deprecated]]
        yesNoBox( SAVE::language p_language, bool p_initSprites = true );

        [[deprecated]]
        yesNoBox( const char* p_name, bool p_initSprites = true );

        [[deprecated]]
        yesNoBox( messageBox p_box, bool p_initSprites = true );

        [[deprecated]]
        bool getResult( const char* p_text = 0, bool p_textAtOnce = true );

      private:
        bool           _isNamed;
        SAVE::language _language;
        void           draw( u8 p_pressedIdx, u8 p_selectedIdx );
    };

} // namespace IO
