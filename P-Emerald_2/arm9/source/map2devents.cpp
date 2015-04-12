/*
Pokémon Emerald 2 Version
------------------------------

file        : map2devents.cpp
author      : Philip Wellnitz (RedArceus)
description :

Copyright (C) 2012 - 2015
Philip Wellnitz (RedArceus)

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <cstdio>

#include "buffer.h"
#include "map2d.h"
#include "map2devents.h"


namespace MAP {
    bool operator<( Map::pos p_a, Map::pos p_b ) {
        if( p_a.m_posX < p_b.m_posX )
            return true;
        else if( p_a.m_posX == p_b.m_posX )
            if( p_a.m_posY < p_b.m_posY )
                return true;
            else if( p_a.m_posY == p_b.m_posY )
                if( p_a.m_posZ < p_b.m_posZ )
                    return true;
        return false;
    }

    map2devents::map2devents( Map* p_map, const char* p_path, const char* p_name )
        : m_map( p_map ) {
        sprintf( buffer, "%s%s.e2t", p_path, p_name );
        FILE* mapF = fopen( buffer, "rb" );
        if( mapF == 0 )
            return;

        fread( &m_personCnt, sizeof( u8 ), 1, mapF );
        fread( &m_warpCnt, sizeof( u8 ), 1, mapF );
        fread( &m_scriptCnt, sizeof( u8 ), 1, mapF );
        fread( &m_signCnt, sizeof( u8 ), 1, mapF );

        //Read Persons
        for( u8 i = 0; i < m_personCnt; ++i ) {
            map2dperson p;
            fread( &p, sizeof( map2dperson ), 1, mapF );
            m_events[ p.m_position ].push_back( p );
        }
        //Read Warps
        for( u8 i = 0; i < m_warpCnt; ++i ) {
            map2dwarp p;
            fread( &p, sizeof( map2dwarp ), 1, mapF );
            m_events[ p.m_position ].push_back( p );
        }
        //Read Scripts
        for( u8 i = 0; i < m_scriptCnt; ++i ) {
            map2dscript p;
            fread( &p, sizeof( map2dscript ), 1, mapF );
            m_events[ p.m_position ].push_back( p );
        }
        //Read Signs
        for( u8 i = 0; i < m_signCnt; ++i ) {
            map2dsign p;
            fread( &p, sizeof( map2dsign ), 1, mapF );
            m_events[ p.m_position ].push_back( p );
        }
    }

    void map2devents::triggerStepOn( Map::pos p_pos, MoveMode p_moveMode ) {
        //Check for warp

        //Check for script

        //Check for trainers
    }

    void map2devents::triggerInteract( Map::pos p_pos, FaceDirection p_faceDirection ) {
        //Check for signs

        //check for persons

    }
}