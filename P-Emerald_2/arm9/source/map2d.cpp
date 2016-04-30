///*
//Pokémon Emerald 2 Version
//------------------------------
//
//file        : map2d.cpp
//author      : Philip Wellnitz 
//description :
//
//Copyright (C) 2012 - 2016
//Philip Wellnitz 
//
//This file is part of Pokémon Emerald 2 Version.
//
//Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
//*/
//
//
//
//void animateMap( u8 p_frame ) {
//    u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
//    for( size_t i = 0; i < acMap->m_animations.size( ); ++i ) {
//        MAP::Animation& a = acMap->m_animations[ i ];
//        if( ( p_frame ) % ( a.m_speed ) == 0 || a.m_speed == 1 ) {
//            a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
//            swiCopy( &a.m_animationTiles[ a.m_acFrame ], tileMemory + a.m_tileIdx * 32, 16 );
//        }
//    }
//}
