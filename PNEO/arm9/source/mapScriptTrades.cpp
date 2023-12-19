/*
Pokémon neo
------------------------------

file        : mapScriptTrades.cpp
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

#include <algorithm>

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "defines.h"
#include "dex/dex.h"
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
    char buffer[ 200 ];
    void mapDrawer::selfTrader( ) {
        ANIMATE_MAP = false;

        // check if self-trader has a pkmn of the player
        if( !SAVE::SAV.getActiveFile( ).m_traderPokemon.getSpecies( ) ) {
            SAVE::SAV.getActiveFile( ).m_traderPokemon = boxPokemon( PKMN_DITTO, 5, 0, 0, 1, true );
            SAVE::SAV.getActiveFile( ).m_traderPokemon.m_oTId  = SAVE::SAV.getActiveFile( ).m_sid;
            SAVE::SAV.getActiveFile( ).m_traderPokemon.m_oTSid = SAVE::SAV.getActiveFile( ).m_id;
            SAVE::SAV.getActiveFile( ).m_traderPokemon.m_ball  = 1;

            memcpy( SAVE::SAV.getActiveFile( ).m_traderPokemon.m_oT, GET_TRADE_STRING( 0 ),
                    OTLENGTH );
            memcpy( SAVE::SAV.getActiveFile( ).m_traderPokemon.m_name, GET_TRADE_STRING( 1 ),
                    PKMN_NAMELENGTH );

            printMapMessage( "Hello, I am Trador.\nI like to trade Pokemon.", style{ 0 } );
        } else {
            printMapMessage( "Trador: Hello [PLAYER]!", style{ 0 } );
        }

        snprintf( buffer, 199, "I will trade any Pokemon\nfor my %s.",
                  FS::getDisplayName( SAVE::SAV.getActiveFile( ).m_traderPokemon.getSpecies( ) )
                      .c_str( ) );
        printMapMessage( buffer, style{ 0 } );

        if( IO::yesNoBox::YES
            == IO::yesNoBox( ).getResult(
                convertMapString( "Would you like to trade\nPokemon with me?", style{ 0 } )
                    .c_str( ),
                MSG_NOCLOSE, false ) ) {

            STS::partyScreen sts = STS::partyScreen( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                     SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ),
                                                     false, false, false, 1 );

            auto selection = sts.run( ).getSelectedPkmn( );

            if( selection < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) ) {
                auto trpkmn = pokemon( SAVE::SAV.getActiveFile( ).m_traderPokemon );

                std::swap( SAVE::SAV.getActiveFile( ).m_pkmnTeam[ selection ], trpkmn );
                SAVE::SAV.getActiveFile( ).m_traderPokemon = trpkmn.m_boxdata;

                pokemon::trade( trpkmn, SAVE::SAV.getActiveFile( ).m_pkmnTeam[ selection ],
                                GET_TRADE_STRING( 0 ) );
            }
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
        printMapMessage( "Come back whenever you\nwant to trade with me!", style{ 0 } );

        ANIMATE_MAP = true;
    }

    void mapDrawer::ingameTrade( u8 p_tradeIdx, u16 p_targetPkmn, u8 p_targetForme,
                                 u16 p_offeredPkmn, u8 p_offeredForme ) {
        // load pkmn from rom
        // ask player for specified pkmn
        // trade
    }
} // namespace MAP
