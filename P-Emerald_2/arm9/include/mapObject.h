/*
Pokémon neo
------------------------------

file        : mapObject.h
author      : Philip Wellnitz
description : Header file. Consult the corresponding source file for details.

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
#pragma once
#include "mapDefines.h"
#include "mapSprite.h"

namespace MAP {
    class mapSprite;

    constexpr u8 getFrame( direction p_direction ) {
        switch( p_direction ) {
        case MAP::UP: return 3;
        case MAP::RIGHT: return 9;
        case MAP::DOWN: return 0;
        case MAP::LEFT: return 6;
        }
        return 0;
    }

    struct mapPlayer {
        u8        m_unused1;
        position  m_pos;
        u16       m_picNum;
        moveMode  m_movement;
        u16       m_unused2;
        direction m_direction;

        constexpr mapPlayer( position p_pos, u16 p_picNum, moveMode p_movement = moveMode::WALK )
            : m_pos( p_pos ), m_picNum( p_picNum ), m_movement( p_movement ),
              m_direction( direction::DOWN ) {
        }

        constexpr mapPlayer( )
            : m_pos( { 0, 0, 0 } ), m_picNum( 0 ), m_movement( moveMode::WALK ),
              m_direction( direction::DOWN ) {
        }

        /*
         * @brief: Constructs a map sprite for the player.
         */
        inline mapSprite sprite( ) const {
            u8 frameStart = getFrame( m_direction );
            return mapSprite( m_picNum, frameStart );
        }
    };

    struct mapObject {
        position       m_pos;
        u16            m_picNum;
        moveMode       m_movement;
        u16            m_range;
        direction      m_direction;
        mapData::event m_event;

        inline mapSprite sprite( ) const {
            u8 frameStart = getFrame( m_direction );
            return mapSprite( m_picNum | 256, frameStart );
        }

        void interact( );

        constexpr mapObject( const mapPlayer& p_player )
            : m_pos( p_player.m_pos ), m_picNum( p_player.m_picNum ),
              m_movement( p_player.m_movement ), m_range( 0 ), m_direction( p_player.m_direction ),
              m_event( mapData::event( ) ) {
        }

        constexpr mapObject( )
            : m_pos( { 0, 0, 0 } ), m_picNum( 0 ), m_movement( moveMode::WALK ), m_range( 0 ),
              m_direction( direction::DOWN ), m_event( mapData::event( ) ) {
        }
    };
} // namespace MAP
