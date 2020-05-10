/*
Pokémon neo
------------------------------

file        : battleUI.cpp
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

#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <functional>
#include <initializer_list>
#include <tuple>

#include "ability.h"
#include "bagViewer.h"
#include "battle.h"
#include "battleTrainer.h"
#include "battleUI.h"
#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "item.h"
#include "keyboard.h"
#include "move.h"
#include "moveNames.h"
#include "moveChoiceBox.h"
#include "pokemon.h"
#include "pokemonNames.h"
#include "saveGame.h"
#include "sprite.h"
#include "uio.h"
#include "yesNoBox.h"

// Sprites
#include "A.h"
#include "Back.h"

#include "Battle1.h"

#include "BattleBall1.h" //Normal
#include "BattleBall2.h" //Statused
#include "BattleBall3.h" //Fainted
#include "BattleBall4.h" //NA

#include "Down.h"
#include "Up.h"
#include "atks.h"
#include "memo.h"

#include "PokeBall1.h"
#include "PokeBall10.h"
#include "PokeBall11.h"
#include "PokeBall12.h"
#include "PokeBall13.h"
#include "PokeBall14.h"
#include "PokeBall15.h"
#include "PokeBall16.h"
#include "PokeBall17.h"
#include "PokeBall2.h"
#include "PokeBall3.h"
#include "PokeBall4.h"
#include "PokeBall5.h"
#include "PokeBall6.h"
#include "PokeBall7.h"
#include "PokeBall8.h"
#include "PokeBall9.h"

#include "Shiny1.h"
#include "Shiny2.h"

namespace BATTLE {
    //////////////////////////////////////////////////////////////////////////
    // BEGIN BATTLE_UI
    //////////////////////////////////////////////////////////////////////////
#define C2I( a ) ( ( a ) - '0' )
#if false
    std::string battleUI::parseLogCmd( const std::string& p_cmd ) {
        if( p_cmd == "A" ) return "`";
        if( p_cmd == "CLEAR" ) return "";

        if( p_cmd == "TRAINER" ) { return _opponent->m_battleTrainerName[ CURRENT_LANGUAGE ]; }
        if( p_cmd == "TCLASS" ) { return trainerClassNames[ _opponent->m_trainerClass ]; }
        if( p_cmd == "OPPONENT" ) {
            if( m_isWildBattle )
                return GET_STRING( 310 );
            else
                return GET_STRING( 311 );
        }
        if( p_cmd.substr( 0, 4 ) == "COLR" ) {
            u8 r, g, b;

            r = 10 * C2I( p_cmd[ 5 ] ) + C2I( p_cmd[ 6 ] );
            g = 10 * C2I( p_cmd[ 8 ] ) + C2I( p_cmd[ 9 ] );
            b = 10 * C2I( p_cmd[ 11 ] ) + C2I( p_cmd[ 12 ] );

            _battleUI.setLogTextColor( RGB15( r, g, b ) );
            if( r != 15 || g != 15 || b != 15 )
                IO::regularFont->setColor( COLOR_IDX, 1 );
            else
                IO::regularFont->setColor( GRAY_IDX, 1 );
            return "";
        }

        pokemon target = CUR_PKMN( 0, PLAYER );
        bool    isPkmn = false;
        bool    isOpp  = false;

        if( p_cmd.substr( 0, 4 ) == "OWN1" ) {
            target = CUR_PKMN( 0, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == "OWN2" ) {
            target = CUR_PKMN( 1, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == "OPP1" ) {
            target = CUR_PKMN( 0, OPPONENT );
            isOpp  = true;
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == "OPP2" ) {
            target = CUR_PKMN( 1, OPPONENT );
            isOpp  = true;
            isPkmn = true;
        }

        if( isPkmn && p_cmd.length( ) == 4 )
            return target.m_boxdata.m_name
                   + std::string( isOpp ? " " + parseLogCmd( "OPPONENT" ) : "" );

        if( isPkmn ) {
            auto specifier = p_cmd.substr( 5 );
            if( specifier == "ABILITY" ) return getAbilityName( target.m_boxdata.m_ability,
                    CURRENT_LANGUAGE );
            if( specifier.substr( 0, 4 ) == "MOVE" )
                return getMoveName( target.m_boxdata.m_moves[ C2I( specifier[ 4 ] ) - 1 ],
                    CURRENT_LANGUAGE );
            if( specifier == "ITEM" ) return ITEM::getItemName( target.m_boxdata.m_holdItem,
                    CURRENT_LANGUAGE );
            if( specifier == "LEVEL" ) return ( "" + target.m_level );
        }

        return "";
    }

    /**
     *  @brief Write the message p_message to the battle log
     *  @param p_message: The message to be written
     */
    void battle::log( const std::string& p_message ) {
        std::string msg = "";
        for( size_t i = 0; i < p_message.length( ); i++ ) {
            if( p_message[ i ] == L'[' ) {
                std::string accmd = "";
                while( p_message[ ++i ] != L']' ) accmd += p_message[ i ];
                if( accmd == "END" ) {
                    _battleUI.writeLogText( msg );
                    return;
                }
                msg += parseLogCmd( accmd );
                if( accmd == "CLEAR" ) {
                    _battleUI.writeLogText( msg );
                    _battleUI.clearLogScreen( );
                    msg = "";
                }
            } else
                msg += p_message[ i ];
        }

        if( msg != "" ) _battleUI.writeLogText( msg );
        _battleUI.clearLogScreen( );
    }
#endif

    void initColors( ) {

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ]  = STEEL_COLOR;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ]   = RED;
        BG_PALETTE_SUB[ BLUE_IDX ]  = BLUE;
        BG_PALETTE_SUB[ RED2_IDX ]  = RED2;
        BG_PALETTE_SUB[ BLUE2_IDX ] = BLUE2;
        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;
    }
    void battleUI::initLogScreen( ) {
        initColors( );
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ]  = STEEL_COLOR;
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ RED_IDX ]   = RED;
        BG_PALETTE[ BLUE_IDX ]  = BLUE;
        IO::printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true, WHITE_IDX );
    }
    void battleUI::clearLogScreen( ) {
        IO::printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true, WHITE_IDX );
    }
    void battleUI::setLogTextColor( u16 p_color ) {
        BG_PALETTE_SUB[ COLOR_IDX ] = p_color;
    }
    void battleUI::writeLogText( const std::string& p_message ) {
        IO::regularFont->printMBString( p_message.c_str( ), 8, 8, true );
    }

}
