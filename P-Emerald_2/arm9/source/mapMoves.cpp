/*
Pokémon Emerald 2 Version
------------------------------

file        : mapMoves.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2017
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
#include "uio.h"
#include "messageBox.h"

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
    return ( SAVE::SAV->getActiveFile( ).m_player.m_movement != MAP::SURF )
        && MAP::curMap->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                              SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] ).m_movedata == 0x4
        && MAP::curMap->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX, SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY ).m_movedata == 0xc;
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
    return MAP::curMap->at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] ).m_bottombehave == 0x12;
}
bool waterfall::possible( ) {
    return ( SAVE::SAV->getActiveFile( ).m_player.m_movement == MAP::SURF )
        && MAP::curMap->at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + MAP::dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] ).m_bottombehave == 0x13;
}
bool teleport::possible( ) {
    return false;
}
bool headbutt::possible( ) {
    return false;
}
bool sweetScent::possible( ) {
    u8 curBehave = MAP::curMap->at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX, SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY ).m_bottombehave;
    return curBehave == 0x2 || curBehave == 0x3;
}
bool dig::possible( ) {
    return false;
}

void cut::use( ) { }
void rockSmash::use( ) { }
void fly::use( ) { }
void flash::use( ) { }
void whirlpool::use( ) { }
void surf::use( ) {
    MAP::curMap->sitDownPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction, MAP::SURF );
}
void dive::use( ) { }
void defog::use( ) { }
void strength::use( ) { }
void rockClimb::use( ) {
    MAP::curMap->sitDownPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction, MAP::SURF );
    while( possible( ) )
        MAP::curMap->walkPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
    MAP::curMap->standUpPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
    if( MAP::curMap->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX, SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY ).m_movedata > 4
        && MAP::curMap->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX, SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY ).m_movedata != 0x3c
        && MAP::curMap->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX, SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY ).m_movedata != 0x0a )
        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ = MAP::curMap->atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX, 
                                                                               SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY ).m_movedata / 4;
}
void waterfall::use( ) {
    MAP::curMap->disablePkmn( );
    while( possible( ) )
        MAP::curMap->walkPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
    MAP::curMap->walkPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
    MAP::curMap->enablePkmn( );
}
void teleport::use( ) { }
void headbutt::use( ) { }
void sweetScent::use( ) {
    if( !sweetScent::possible( ) || !MAP::curMap->requestWildPkmn( true ) ) {
        IO::messageBox( GET_STRING( 90 ), true );
        IO::NAV->draw( );
    }
}
void dig::use( ) { }
