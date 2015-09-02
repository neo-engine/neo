/*
Pokémon Emerald 2 Version
------------------------------

file        : mapMoves.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2015
Philip Wellnitz

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

#include "defines.h"
#include "mapDrawer.h"
#include "mapSlice.h"
#include "hmMoves.h"
#include "saveGame.h"

bool cut::possible( ) {
    return false;
}
bool rockSmash::possible( ) {
    return false;
}
bool fly::possible( ) {
    return false;
}
bool flash::possible( ) {
    return false;
}
bool whirlpool::possible( ) {
    return false;
}
bool surf::possible( ) {
    return ( FS::SAV->m_player.m_movement != MAP::SURF )
        && MAP::curMap->atom( FS::SAV->m_player.m_pos.m_posX + MAP::dir[ FS::SAV->m_player.m_direction ][ 0 ],
        FS::SAV->m_player.m_pos.m_posY + MAP::dir[ FS::SAV->m_player.m_direction ][ 1 ] ).m_movedata == 0x4;
}
bool dive::possible( ) {
    return false;
}
bool defog::possible( ) {
    return false;
}
bool strength::possible( ) {
    return false;
}
bool rockClimb::possible( ) {
    return MAP::curMap->at( FS::SAV->m_player.m_pos.m_posX + MAP::dir[ FS::SAV->m_player.m_direction ][ 0 ],
                            FS::SAV->m_player.m_pos.m_posY + MAP::dir[ FS::SAV->m_player.m_direction ][ 1 ] ).m_bottombehave == 0x12;
}
bool waterfall::possible( ) {
    return ( FS::SAV->m_player.m_movement == MAP::SURF )
        && MAP::curMap->at( FS::SAV->m_player.m_pos.m_posX + MAP::dir[ FS::SAV->m_player.m_direction ][ 0 ],
        FS::SAV->m_player.m_pos.m_posY + MAP::dir[ FS::SAV->m_player.m_direction ][ 1 ] ).m_bottombehave == 0x13;
}
bool teleport::possible( ) {
    return false;
}
bool headbutt::possible( ) {
    return false;
}
bool sweetScent::possible( ) {
    u8 curBehave = MAP::curMap->at( FS::SAV->m_player.m_pos.m_posX, FS::SAV->m_player.m_pos.m_posY ).m_bottombehave;
    return curBehave == 0x2 || curBehave == 0x3;
}

void cut::use( ) { }
void rockSmash::use( ) { }
void fly::use( ) { }
void flash::use( ) { }
void whirlpool::use( ) { }
void surf::use( ) {
    MAP::curMap->sitDownPlayer( FS::SAV->m_player.m_direction, MAP::SURF );
}
void dive::use( ) { }
void defog::use( ) { }
void strength::use( ) { }
void rockClimb::use( ) {
    MAP::curMap->sitDownPlayer( FS::SAV->m_player.m_direction, MAP::SURF );
    while( possible( ) )
        MAP::curMap->walkPlayer( FS::SAV->m_player.m_direction );
    MAP::curMap->standUpPlayer( FS::SAV->m_player.m_direction );
    if( MAP::curMap->atom( FS::SAV->m_player.m_pos.m_posX, FS::SAV->m_player.m_pos.m_posY ).m_movedata > 4
        && MAP::curMap->atom( FS::SAV->m_player.m_pos.m_posX, FS::SAV->m_player.m_pos.m_posY ).m_movedata != 0x3c
        && MAP::curMap->atom( FS::SAV->m_player.m_pos.m_posX, FS::SAV->m_player.m_pos.m_posY ).m_movedata != 0x0a )
        FS::SAV->m_player.m_pos.m_posZ = MAP::curMap->atom( FS::SAV->m_player.m_pos.m_posX, FS::SAV->m_player.m_pos.m_posY ).m_movedata / 4;
}
void waterfall::use( ) {
    while( possible( ) )
        MAP::curMap->walkPlayer( FS::SAV->m_player.m_direction );
    MAP::curMap->walkPlayer( FS::SAV->m_player.m_direction );
}
void teleport::use( ) { }
void headbutt::use( ) { }
void sweetScent::use( ) { }