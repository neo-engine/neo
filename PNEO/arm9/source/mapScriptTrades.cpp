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

            printMapMessage( GET_MAP_STRING( IO::STR_MAP_ST_INTRO ), MSG_NORMAL );
        } else {
            printMapMessage( GET_MAP_STRING( IO::STR_MAP_ST_MSG1 ), MSG_NORMAL );
        }

        snprintf( buffer, 199, GET_MAP_STRING( IO::STR_MAP_ST_MSG2 ),
                  FS::getDisplayName( SAVE::SAV.getActiveFile( ).m_traderPokemon.getSpecies( ) )
                      .c_str( ) );
        printMapMessage( buffer, MSG_NORMAL );

        if( IO::yesNoBox::YES
            == IO::yesNoBox( ).getResult(
                convertMapString( GET_MAP_STRING( IO::STR_MAP_ST_MSG3 ), MSG_NORMAL ).c_str( ),
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
        printMapMessage( GET_MAP_STRING( IO::STR_MAP_ST_MSG4 ), MSG_NORMAL );

        ANIMATE_MAP = true;
    }

    u8 mapDrawer::ingameTrade( u8 p_tradeIdx, u16 p_targetPkmn, u8 p_targetForme, u16 p_offeredPkmn,
                               u8 p_offeredForme ) {
        ANIMATE_MAP = false;

        STS::partyScreen sts = STS::partyScreen( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                 SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ),
                                                 false, false, false, 1 );

        u8   res       = 2;
        auto selection = sts.run( ).getSelectedPkmn( );
        if( selection < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) ) {
            auto& pkmn = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ selection ];
            if( pkmn.getSpecies( ) != p_targetPkmn || pkmn.getForme( ) != p_targetForme ) {
                res = 2;
            } else {
                auto trpkmn = pokemon( p_offeredPkmn, pkmn.m_level, p_offeredForme,
                                       GET_TRADE_STRING( 2 * p_tradeIdx + 1 ) );
                memcpy( trpkmn.m_boxdata.m_oT, GET_TRADE_STRING( 2 * p_tradeIdx ), OTLENGTH );
                trpkmn.m_boxdata.m_oTId
                    = u16{ trpkmn.m_boxdata.m_oT[ 0 ] } * trpkmn.m_boxdata.m_oT[ 1 ] + p_tradeIdx;
                trpkmn.m_boxdata.m_oTSid = 2 * p_tradeIdx;
                std::swap( SAVE::SAV.getActiveFile( ).m_pkmnTeam[ selection ], trpkmn );

                pokemon::trade( trpkmn, SAVE::SAV.getActiveFile( ).m_pkmnTeam[ selection ],
                                GET_TRADE_STRING( 2 * p_tradeIdx ) );
                res = 0;
            }
        } else {
            res = 1;
        }
        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        IO::resetScale( true, false );
        bgUpdate( );
        IO::init( );
        MAP::curMap->draw( );

        ANIMATE_MAP = true;
        return res;
    }
} // namespace MAP
