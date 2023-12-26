/*
Pokémon neo
------------------------------

file        : mapScriptPokeBlockBlender.cpp
author      : Philip Wellnitz
description : Map script engine (ingame trades)

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

#include "defines.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "map/mapDrawer.h"
#include "spx/specials.h"

namespace MAP {
    void mapDrawer::pokeblockBlender( u8 p_numNPC ) {
        char buffer[ 200 ];
        ANIMATE_MAP = false;

        printMapMessage( GET_MAP_STRING( 736 ), MSG_INFO );

        // check for pokeblock kit

        if( !SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( BAG::ITEMTYPE_KEYITEM ),
                                                     I_POKEBLOCK_KIT ) ) {
            printMapMessage( GET_MAP_STRING( 735 ), MSG_INFO );

            ANIMATE_MAP = true;
            return;
        }

        if( !p_numNPC ) {
            printMapMessage( GET_MAP_STRING( 737 ), MSG_INFO );

            // check if rotom (base forme) is following the player, if so ask if player
            // would like to let rotom enter the machine
            bool canUseRotom = _pkmnFollowsPlayer && _followPkmnData->getSpecies( ) == PKMN_ROTOM
                               && _followPkmnData->getForme( ) == 0;

            if( canUseRotom ) {
                snprintf( buffer, 199, GET_MAP_STRING( 738 ), _followPkmnData->m_boxdata.m_name );
                printMapMessage( buffer, MSG_INFO );

                snprintf( buffer, 199, GET_MAP_STRING( 739 ), _followPkmnData->m_boxdata.m_name );

                if( IO::yesNoBox::YES
                    == IO::yesNoBox( ).getResult( convertMapString( buffer, MSG_INFO ).c_str( ),
                                                  MSG_INFO_NOCLOSE, false ) ) {

                    direction olddir = SAVE::SAV.getActiveFile( ).m_player.m_direction;
                    useFollowPkmn( );
                    removeFollowPkmn( );
                    walkPlayer( direction( ( _lastPlayerMove + 2 ) % 4 ), false );
                    redirectPlayer( olddir, false );

                    // start minigame
                    ANIMATE_MAP = false;
                    SPX::runPokeblockBlender( 3, true );

                    FADE_TOP_DARK( );
                    FADE_SUB_DARK( );
                    IO::clearScreen( false );
                    videoSetMode( MODE_5_2D );
                    IO::resetScale( true, false );
                    bgUpdate( );
                    IO::init( );
                    MAP::curMap->draw( );
                } else {
                    IO::init( );
                }
            }
            ANIMATE_MAP = true;
            return;
        }

        if( IO::yesNoBox::YES
            == IO::yesNoBox( ).getResult(
                convertMapString( GET_MAP_STRING( 740 ), MSG_INFO ).c_str( ), MSG_INFO_NOCLOSE,
                false ) ) {
            // start minigame
            SPX::runPokeblockBlender( p_numNPC, false );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            IO::resetScale( true, false );
            bgUpdate( );
            IO::init( );
            MAP::curMap->draw( );
        } else {
            IO::init( );
        }
        ANIMATE_MAP = true;
    }
} // namespace MAP
