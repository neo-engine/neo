/*
Pokémon neo
------------------------------

file        : mapMessage.cpp
author      : Philip Wellnitz
description : Map script engine (message boxes etc)

Copyright (C) 2012 - 2023
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

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "box/boxViewer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/locationNames.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "map/mapScript.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace MAP {
    std::string convertMapString( const std::string& p_text, style p_style ) {
        std::string res = "";
        for( size_t i = 0; i < p_text.size( ); ++i ) {
            if( p_text[ i ] == '[' ) {
                std::string accmd = "";
                while( p_text[ ++i ] != ']' ) { accmd += p_text[ i ]; }
                res += IO::parseLogCmd( accmd );
                continue;
            } else if( p_text[ i ] == '\r' ) {
                if( p_style == MSG_NORMAL ) {
                    IO::printMessage( res.c_str( ), MSG_NORMAL_CONT );
                } else if( p_style == MSG_INFO ) {
                    IO::printMessage( res.c_str( ), MSG_INFO_CONT );
                } else {
                    IO::printMessage( res.c_str( ), p_style );
                }
                res = "";
                continue;
            }
            res += p_text[ i ];
        }
        return res;
    }

    void printMapMessage( const std::string& p_text, style p_style ) {
        if( p_style == MSG_SIGN ) {
            // TODO: properly implement signs
            p_style = MSG_INFO;
        }
        IO::printMessage( convertMapString( p_text, p_style ).c_str( ), p_style );
    }

    void printMapYNMessage( const std::string& p_text, style p_style ) {

        if( p_style == MSG_SIGN ) {
            // TODO: properly implement signs
            p_style = MSG_INFO;
        }
        IO::printMessage( convertMapString( p_text, p_style ).c_str( ), p_style );
    }
} // namespace MAP
