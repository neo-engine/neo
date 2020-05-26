/*
Pokémon neo
------------------------------

file        : pokemonIO.cpp
author      : Philip Wellnitz
description : Methods for pokémon which require user IO

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

#include "choiceBox.h"
#include "defines.h"
#include "messageBox.h"
#include "moveChoiceBox.h"
#include "pokemon.h"
#include "uio.h"
#include "yesNoBox.h"

bool boxPokemon::learnMove( u16 p_move ) {
    char buffer[ 50 ];
    if( p_move == m_moves[ 0 ] || p_move == m_moves[ 1 ] || p_move == m_moves[ 2 ]
        || p_move == m_moves[ 3 ] ) {
        snprintf( buffer, 49, GET_STRING( 102 ), m_name, MOVE::getMoveName( p_move ).c_str( ) );
        IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
        IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
        IO::messageBox a( buffer, false );
        return false;
    } else if( canLearn( m_speciesId, p_move, 4 ) ) {
        auto mdata    = MOVE::getMoveData( p_move );
        bool freeSpot = false;
        for( u8 i = 0; i < 4; ++i )
            if( !m_moves[ i ] ) {
                m_moves[ i ] = p_move;
                m_curPP[ i ] = std::min( m_curPP[ i ], mdata.m_pp );

                snprintf( buffer, 49, GET_STRING( 103 ), m_name,
                          MOVE::getMoveName( p_move ).c_str( ) );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                IO::messageBox a( buffer, false );

                freeSpot = true;
                break;
            }
        if( !freeSpot ) {
            IO::yesNoBox yn( false );
            snprintf( buffer, 49, GET_STRING( 104 ), m_name );
            if( yn.getResult( buffer ) ) {
                u8 res = IO::moveChoiceBox( *this, p_move ).getResult( false, false, (u8) -1 );
                if( res < 4 ) {
                    if( MOVE::isFieldMove( m_moves[ res ] ) ) {
                        snprintf( buffer, 49, GET_STRING( 106 ), m_name,
                                  MOVE::getMoveName( m_moves[ res ] ).c_str( ) );
                        IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                        IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                        IO::messageBox a( buffer, false );
                        return false;
                    } else {
                        m_moves[ res ] = p_move;
                        m_curPP[ res ] = std::min( m_curPP[ res ], mdata.m_pp );
                    }
                }
            } else
                return false;
        }
    } else {
        snprintf( buffer, 49, GET_STRING( 107 ), m_name, MOVE::getMoveName( p_move ).c_str( ) );
        IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
        IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
        IO::messageBox a( buffer, false );
        return false;
    }
    return true;
}
