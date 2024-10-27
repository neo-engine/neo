/*
Pokémon neo
------------------------------

file        : navApp.cpp
author      : Philip Wellnitz
description : Implementation of applications for the PNav.

Copyright (C) 2012 - 2022
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

#include "fs/data.h"
#include "fs/fs.h"
#include "gen/moveNames.h"
#include "io/menuUI.h"
#include "io/navApp.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDefines.h"
#include "map/mapDrawer.h"

namespace IO {

    void mapNavApp::drawIcon( u8 p_oamSlot, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        IO::loadSprite( "NV/app01", p_oamSlot, oam[ p_oamSlot ].palette, oam[ p_oamSlot ].gfxIndex,
                        oam[ p_oamSlot ].x, oam[ p_oamSlot ].y, 64, 64, false, false, false,
                        OBJPRIORITY_1, p_bottom );
    }

    void mapNavApp::load( bool p_bottom ) {
        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        auto ptr3 = !p_bottom ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        auto pal  = !p_bottom ? BG_PALETTE : BG_PALETTE_SUB;

        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 i = 0; i < MAX_NAV_APPS; ++i ) { oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden = true; }

        oam[ SPR_NAV_APP_RSV_SUB + 2 ]          = oam[ SPR_X_OAM_SUB ];
        oam[ SPR_NAV_APP_RSV_SUB + 2 ].isHidden = false;
        oam[ SPR_NAV_APP_RSV_SUB + 2 ].x        = 200;
        oam[ SPR_NAV_APP_RSV_SUB + 2 ].y        = 166;

        computePlayerPosition( );
        _cursorX = _playerX;
        _cursorY = _playerY;
        computeCursorLocationId( );

        // load player icon
        u16 tileCnt = 0;

        if( !SAVE::SAV.getActiveFile( ).m_appearance ) {
            tileCnt
                = IO::loadSprite( "NV/player0", SPR_NAV_APP_RSV_SUB + 1, SPR_NAV_APP_RSV1_PAL_SUB,
                                  tileCnt, _playerX - 8, _playerY + _mapTopY - 8, 16, 16, false,
                                  false, false, OBJPRIORITY_3, p_bottom );
        } else {
            tileCnt
                = IO::loadSprite( "NV/player1", SPR_NAV_APP_RSV_SUB + 1, SPR_NAV_APP_RSV1_PAL_SUB,
                                  tileCnt, _playerX - 8, _playerY + _mapTopY - 8, 16, 16, false,
                                  false, false, OBJPRIORITY_3, p_bottom );
        }

        // load cursor icon
        tileCnt = IO::loadUIIcon(
            IO::ICON::BOX_ARROW_START, SPR_NAV_APP_RSV_SUB, SPR_NAV_APP_RSV2_PAL_SUB, tileCnt,
            _cursorX, _cursorY + _mapTopY - 14, 16, 16, false, false, false, OBJPRIORITY_3, true );

        // load bg
        // use last OW map as dafault
        snprintf( buffer, TMP_BUFFER_SIZE, "map%hhu", _OWMap );
        FS::readPictureData( ptr3, "nitro:/PICS/NAV_APP/", buffer, 192 * 2, 192 * 256, p_bottom );
        pal[ 0 ] = IO::BLACK;

        if( _cursorLocationId ) {
            snprintf( buffer, TMP_BUFFER_SIZE, "%s: %s",
                      FS::getLocation( MAP::MAP_LOCATIONS.m_defaultLocation ).c_str( ),
                      FS::getLocation( _cursorLocationId ).c_str( ) );
        } else {
            snprintf( buffer, TMP_BUFFER_SIZE, "%s",
                      FS::getLocation( MAP::MAP_LOCATIONS.m_defaultLocation ).c_str( ) );
        }

        IO::regularFont->printStringC( buffer, 12, 10, p_bottom, IO::font::LEFT );

        IO::updateOAM( p_bottom );
    }

    void mapNavApp::computePlayerPosition( ) {
        if( FSDATA.isOWMap( SAVE::SAV.getActiveFile( ).m_currentMap ) ) {
            if( MAP::MAP_LOCATIONS.m_bank != SAVE::SAV.getActiveFile( ).m_currentMap
                || !MAP::MAP_LOCATIONS.m_good ) {
                FS::loadLocationData( SAVE::SAV.getActiveFile( ).m_currentMap );
            }
            // player is in OW
            _playerX = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                       / MAP::MAP_LOCATIONS.m_mapImageRes;
            _playerY = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                       / MAP::MAP_LOCATIONS.m_mapImageRes;
            _OWMap = SAVE::SAV.getActiveFile( ).m_currentMap;

            _mapTopX = MAP::MAP_LOCATIONS.m_mapImageShiftX;
            _mapTopY = MAP::MAP_LOCATIONS.m_mapImageShiftY;
            _mapBotX = _mapTopX
                       + ( MAP::mapLocation::MAP_LOCATION_RES / MAP::MAP_LOCATIONS.m_mapImageRes )
                             * ( MAP::MAP_LOCATIONS.m_owMapSizeX );
            _mapBotY = _mapTopY
                       + ( MAP::mapLocation::MAP_LOCATION_RES / MAP::MAP_LOCATIONS.m_mapImageRes )
                             * ( MAP::MAP_LOCATIONS.m_owMapSizeY + 1 );
        } else if( FSDATA.isOWMap( SAVE::SAV.getActiveFile( ).m_lastOWPos.first ) ) {
            if( MAP::MAP_LOCATIONS.m_bank != SAVE::SAV.getActiveFile( ).m_lastOWPos.first
                || !MAP::MAP_LOCATIONS.m_good ) {
                FS::loadLocationData( SAVE::SAV.getActiveFile( ).m_lastOWPos.first );
            }

            _playerX = SAVE::SAV.getActiveFile( ).m_lastOWPos.second.m_posX
                       / MAP::MAP_LOCATIONS.m_mapImageRes;
            _playerY = SAVE::SAV.getActiveFile( ).m_lastOWPos.second.m_posY
                       / MAP::MAP_LOCATIONS.m_mapImageRes;
            _OWMap = SAVE::SAV.getActiveFile( ).m_lastOWPos.first;

            _mapTopX = MAP::MAP_LOCATIONS.m_mapImageShiftX;
            _mapTopY = MAP::MAP_LOCATIONS.m_mapImageShiftY;
            _mapBotX = _mapTopX
                       + ( MAP::mapLocation::MAP_LOCATION_RES / MAP::MAP_LOCATIONS.m_mapImageRes )
                             * ( MAP::MAP_LOCATIONS.m_owMapSizeX );
            _mapBotY = _mapTopY
                       + ( MAP::mapLocation::MAP_LOCATION_RES / MAP::MAP_LOCATIONS.m_mapImageRes )
                             * ( MAP::MAP_LOCATIONS.m_owMapSizeY + 1 );
        } else {
            _playerX = 0;
            _playerY = 0;
            _OWMap   = 0;

            _mapTopX = 0;
            _mapTopY = 0;
            _mapBotX = 0;
            _mapBotY = 0;
        }
    }

    void mapNavApp::computeCursorLocationId( ) {
        _cursorLocationId = MAP::MAP_LOCATIONS.get( _cursorY / 2, _cursorX / 2 );
    }

    bool mapNavApp::tick( bool p_bottom ) {
        BG_PALETTE_SUB[ IO::BLUE_IDX ] = IO::BLUE2;

        constexpr u8 TMP_BUFFER_SIZE = 100;
        char         buffer[ TMP_BUFFER_SIZE + 10 ];

        // update player position
        auto oldx = _playerX, oldy = _playerY;
        auto oldl = _cursorLocationId;
        computePlayerPosition( );
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        bool change    = _playerX != oldx || _playerY != oldy;
        bool locChange = false;

        // update player sprite

        oam[ SPR_NAV_APP_RSV_SUB + 1 ].x = _playerX - 8;
        oam[ SPR_NAV_APP_RSV_SUB + 1 ].y = _playerY + _mapTopY - 8;
        if( change ) {
            locChange |= ( _playerX != _cursorX || _playerY != _cursorY );
            _cursorX = _playerX;
            _cursorY = _playerY;
        }

        // Check for fly
        bool flyusable = false;
        for( u8 i = 0; i < 6; ++i ) {
            if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ].m_boxdata.m_speciesId ) { break; }
            auto a = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];
            if( a.isEgg( ) ) { continue; }
            for( u8 j = 0; j < 4; ++j ) {
                if( a.m_boxdata.m_moves[ j ] == M_FLY && BATTLE::possible( M_FLY, 0 ) ) {
                    flyusable = true;
                    break;
                }
            }
        }

        // check for (touch) input
        if( touch.px > _mapTopX && touch.px < _mapBotX && touch.py > _mapTopY
            && touch.py < _mapBotY ) {
            // move cursor
            locChange |= ( touch.px != _cursorX || touch.py != _cursorY );

            _cursorX = touch.px;
            _cursorY = touch.py - _mapTopY;
        }

        if( locChange ) {
            oam[ SPR_NAV_APP_RSV_SUB ].x = _cursorX;
            oam[ SPR_NAV_APP_RSV_SUB ].y = _cursorY + _mapTopY - 14;

            computeCursorLocationId( );
            if( _cursorLocationId != oldl ) {
                if( _cursorLocationId
                    && MAP::MAP_LOCATIONS.m_defaultLocation != _cursorLocationId ) {
                    snprintf( buffer, TMP_BUFFER_SIZE, "%s: %s",
                              FS::getLocation( MAP::MAP_LOCATIONS.m_defaultLocation ).c_str( ),
                              FS::getLocation( _cursorLocationId ).c_str( ) );
                } else {
                    snprintf( buffer, TMP_BUFFER_SIZE, "%s",
                              FS::getLocation( MAP::MAP_LOCATIONS.m_defaultLocation ).c_str( ) );
                }
                IO::printRectangle( 43, 10, 200, 30, p_bottom, 0 );
                IO::regularFont->printStringC( buffer, 12, 10, p_bottom, IO::font::LEFT );
            }
        }

        auto nm     = FS::getMoveName( M_FLY );
        auto wd     = IO::regularFont->stringWidth( nm.c_str( ) );
        bool canfly = false;
        if( flyusable && _cursorLocationId ) {
            // check if current location has a registered fly position
            auto fpos = SAVE::SAV.getActiveFile( ).getFlyPosForLocation( _cursorLocationId );
            if( fpos.location( ) == _cursorLocationId ) {
                IO::printRectangle( oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - wd - 4, 10,
                                    oam[ SPR_NAV_APP_RSV_SUB + 2 ].x, 26, p_bottom, IO::BLUE_IDX );
                IO::regularFont->printStringC( nm.c_str( ),
                                               oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - ( wd / 2 ) - 3,
                                               10, p_bottom, IO::font::CENTER );
                canfly = true;
            }
        } else {
            IO::printRectangle( oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - wd - 4, 10,
                                oam[ SPR_NAV_APP_RSV_SUB + 2 ].x, 26, p_bottom, 0 );
        }

        if( touch.px >= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x
            && touch.px <= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x + 20
            && touch.py >= oam[ SPR_NAV_APP_RSV_SUB + 2 ].y
            && touch.py <= oam[ SPR_NAV_APP_RSV_SUB + 2 ].y + 20 ) {
            bool suc = true;
            while( touch.px || touch.py ) {
                swiWaitForVBlank( );
                scanKeys( );

                if( !( touch.px >= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x
                       && touch.px <= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x + 20
                       && touch.py >= oam[ SPR_NAV_APP_RSV_SUB + 2 ].y
                       && touch.py <= oam[ SPR_NAV_APP_RSV_SUB + 2 ].y + 20 ) ) {
                    suc = false;
                    break;
                }
                touchRead( &touch );
                swiWaitForVBlank( );
            }
            if( suc ) { return true; }
        }

        if( canfly ) {
            if( touch.px >= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - wd - 4
                && touch.px <= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - 1 && touch.py >= 10
                && touch.py <= 26 ) {
                bool suc = true;
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );

                    if( !( touch.px >= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - wd - 4
                           && touch.px <= oam[ SPR_NAV_APP_RSV_SUB + 2 ].x - 1 && touch.py >= 10
                           && touch.py <= 26 ) ) {
                        suc = false;
                        break;
                    }
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }
                if( suc ) {
                    // execute fly
                    auto fpos
                        = SAVE::SAV.getActiveFile( ).getFlyPosForLocation( _cursorLocationId );
                    auto target = MAP::warpPos{
                        fpos.m_targetBank,
                        MAP::position{ fpos.m_targetX, fpos.m_targetY, fpos.m_targetZ } };
                    MAP::curMap->flyPlayer( target );
                    return false;
                }
            }
        }

        if( change || locChange ) { IO::updateOAM( p_bottom ); }
        return false;
    }
} // namespace IO
