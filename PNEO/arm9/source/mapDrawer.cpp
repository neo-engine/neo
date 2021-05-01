/*
Pokémon neo
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine

Copyright (C) 2012 - 2021
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

#include "abilityNames.h"
#include "bagViewer.h"
#include "battle.h"
#include "battleDefines.h"
#include "battleTrainer.h"
#include "choiceBox.h"
#include "defines.h"
#include "fs.h"
#include "gameStart.h"
#include "mapDrawer.h"
#include "nav.h"
#include "pokemonNames.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sound.h"
#include "sprite.h"
#include "uio.h"

#ifdef DESQUID_MORE
#include <cassert>
#endif

namespace MAP {
#define MAP_BORDER 0x3f

#define SPR_PKMN_OAM 100
#define SPR_CIRC_OAM 104

#define SPR_PKMN_GFX 303
#define SPR_CIRC_GFX 447

    constexpr direction movement2Direction( u8 p_move ) {
        switch( p_move ) {
        case 0: return UP;
        case 1: return DOWN;
        case 2: return RIGHT;
        case 3: return LEFT;

        default: return DOWN;
        }
    }

    direction getRandomLookDirection( moveMode p_movement ) {
        u8 st = rand( ) % 4;

        for( u8 i = 0; i < 4; ++i ) {
            if( ( p_movement == WALK_AROUND_LEFT_RIGHT ) || ( p_movement == WALK_AROUND_UP_DOWN )
                || ( p_movement == WALK_AROUND_SQUARE )
                || ( p_movement & ( 1 << ( ( st + i ) % 4 ) ) ) ) {
                return movement2Direction( ( st + i ) % 4 );
            }
        }

        return DOWN;
    }

    mapDrawer* curMap = nullptr;
#define CUR_SLICE _slices[ _curX ][ _curY ]
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    void mapDrawer::loadMapObject( std::pair<u8, mapObject>& p_mapObject ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        switch( p_mapObject.second.m_event.m_type ) {
        default:
        case EVENT_OW_PKMN:
        case EVENT_TRAINER:
        case EVENT_NPC_MESSAGE:
        case EVENT_NPC: {
            p_mapObject.first = _mapSprites.loadSprite(
                curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                mapSpriteManager::SPTYPE_NPC, p_mapObject.second.sprite( ) );

            _mapSprites.setFrameD( p_mapObject.first, p_mapObject.second.m_direction, false );
            break;
        }
        case EVENT_HMOBJECT: {
            if( p_mapObject.second.m_event.m_data.m_hmObject.m_hmType ) {
                p_mapObject.first = _mapSprites.loadSprite(
                    curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                    p_mapObject.second.m_event.m_data.m_hmObject.m_hmType );
            } else {
                // HM object got destroyed already
                p_mapObject.first = 255;
            }
            break;
        }
        case EVENT_BERRYTREE: {
            // Check the growth of the specified berry tree
            if( SAVE::SAV.getActiveFile( ).berryIsAlive(
                    p_mapObject.second.m_event.m_data.m_berryTree.m_treeIdx ) ) {
                u8 stage = SAVE::SAV.getActiveFile( ).getBerryStage(
                    p_mapObject.second.m_event.m_data.m_berryTree.m_treeIdx );
                u8 berryType = SAVE::SAV.getActiveFile( ).getBerry(
                    p_mapObject.second.m_event.m_data.m_berryTree.m_treeIdx );
                p_mapObject.first = _mapSprites.loadBerryTree(
                    curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                    berryType, stage );
            } else {
                p_mapObject.first = 255;
            }
            break;
        }
        case EVENT_ITEM: {
            if( p_mapObject.second.m_event.m_data.m_item.m_itemType ) {
                p_mapObject.first = _mapSprites.loadSprite(
                    curx, cury, p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY,
                    p_mapObject.second.m_event.m_data.m_item.m_itemType == 1
                        ? mapSpriteManager::SPR_ITEM
                        : mapSpriteManager::SPR_HMBALL );
            } else {
                // No item icon for hidden items (otherwise the "hidden" part would be
                // pointless, right)
                p_mapObject.first = 255;
            }
        }
        }
        if( p_mapObject.first != 255 ) {
            animateField( p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY );
        }
    }

    const mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            .m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    const block& mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           .m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ].m_blockSet.m_blocks[ blockidx ];
    }

    mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            .m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    block& mapDrawer::at( u16 p_x, u16 p_y ) {
        bool x = ( p_x / SIZE != CUR_SLICE.m_x ), y = ( p_y / SIZE != CUR_SLICE.m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           .m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ].m_blockSet.m_blocks[ blockidx ];
    }

    const mapData& mapDrawer::currentData( ) const {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        return currentData( curx, cury );
    }
    const mapData& mapDrawer::currentData( u16 p_x, u16 p_y ) const {
        u16 curx = p_x;
        u16 cury = p_y;

        bool x = ( curx / SIZE != CUR_SLICE.m_x ), y = ( cury / SIZE != CUR_SLICE.m_y );
        return _data[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ];
    }

#define CUR_DATA currentData( )
    u16* mapMemory[ 4 ];
    s8   fastBike = false;

    void mapDrawer::loadBlock( block p_curblock, u32 p_memPos ) {
        u8   toplayer = 1, bottomlayer = 3;
        bool elevateTopLayer = p_curblock.m_topbehave == 0x10;

        mapMemory[ toplayer ][ p_memPos ]      = !elevateTopLayer * p_curblock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 1 ]  = !elevateTopLayer * p_curblock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer ][ p_memPos + 32 ] = !elevateTopLayer * p_curblock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer ][ p_memPos + 33 ] = !elevateTopLayer * p_curblock.m_top[ 1 ][ 1 ];

        mapMemory[ toplayer + 1 ][ p_memPos ]      = elevateTopLayer * p_curblock.m_top[ 0 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 1 ]  = elevateTopLayer * p_curblock.m_top[ 0 ][ 1 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 32 ] = elevateTopLayer * p_curblock.m_top[ 1 ][ 0 ];
        mapMemory[ toplayer + 1 ][ p_memPos + 33 ] = elevateTopLayer * p_curblock.m_top[ 1 ][ 1 ];

        mapMemory[ bottomlayer ][ p_memPos ]      = p_curblock.m_bottom[ 0 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 1 ]  = p_curblock.m_bottom[ 0 ][ 1 ];
        mapMemory[ bottomlayer ][ p_memPos + 32 ] = p_curblock.m_bottom[ 1 ][ 0 ];
        mapMemory[ bottomlayer ][ p_memPos + 33 ] = p_curblock.m_bottom[ 1 ][ 1 ];
    }
    void mapDrawer::loadBlock( block p_curblock, u8 p_scrnX, u8 p_scrnY ) {
        u32 c = 64 * u32( p_scrnY ) + 2 * ( u32( p_scrnX ) % 16 );
        c += ( u32( p_scrnX ) / 16 ) * 1024;
        loadBlock( p_curblock, c );
    }

    void mapDrawer::registerOnBankChangedHandler( std::function<void( u8 )> p_handler ) {
        _newBankCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnLocationChangedHandler( std::function<void( u16 )> p_handler ) {
        _newLocationCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnMoveModeChangedHandler( std::function<void( moveMode )> p_handler ) {
        _newMoveModeCallbacks.push_back( p_handler );
    }
    void mapDrawer::registerOnWeatherChangedHandler( std::function<void( mapWeather )> p_handler ) {
        _newWeatherCallbacks.push_back( p_handler );
    }
    // Drawing of Maps and stuff

    void mapDrawer::attachMapObjectToPlayer( u8 p_objectId ) {
        removeAttachedObjects( );
        SAVE::SAV.getActiveFile( ).m_objectAttached    = 1;
        SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx = p_objectId;
        _fixedMapObjects.insert( p_objectId );
    }

    void mapDrawer::removeAttachedObjects( ) {
        if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            _fixedMapObjects.erase( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx );
            SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx = 0;
            SAVE::SAV.getActiveFile( ).m_objectAttached    = 0;
        }
    }

    constexpr auto WEATHER_BLEND = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG0 | BLEND_DST_BG2
                                   | BLEND_DST_BG1 | BLEND_DST_SPRITE;

    void mapDrawer::initWeather( ) {
        _weatherScrollX = 0;
        _weatherScrollY = 0;
        REG_BLDALPHA    = 0;
        switch( getWeather( ) ) {
        case CLOUDY:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "clouds",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );
            _weatherScrollX = 2;
            _weatherScrollY = 0;
            REG_BLDALPHA    = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case FOREST_CLOUDS:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "forestcloud",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );
            _weatherScrollX = 0;
            _weatherScrollY = 0;
            REG_BLDALPHA    = 0xff | ( 0x08 << 8 );
            _weatherFollow  = true;
            break;

        case ASH_RAIN:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "ashrain",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );
            _weatherScrollX = 2;
            _weatherScrollY = -4;
            _weatherFollow  = true;
            break;
        case SANDSTORM: {
            bool goggles = SAVE::SAV.getActiveFile( ).m_bag.count(
                BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_GO_GOGGLES );

            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            bgWrapOn( IO::bg3 );

            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "sandstorm",
                                                        256 * 256 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            bgSetScroll( IO::bg3, 0, 0 );

            if( goggles ) { REG_BLDALPHA = 0xff | ( 0x05 << 8 ); }

            _weatherScrollX = 40;
            _weatherScrollY = 10;
            _weatherFollow  = false;
            break;
        }
        case DARK_FLASHABLE:
        case DARK_PERMANENT:
        case DARK_FLASH_USED:
        case DARK_FLASH_1:
        case DARK_FLASH_2:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            FS::readData<unsigned int, unsigned short>( "nitro:/PICS/WEATHER/", "flash",
                                                        256 * 192 / 4, TEMP, 256, TEMP_PAL );
            dmaCopy( TEMP, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
            dmaCopy( TEMP_PAL, BG_PALETTE + 240, 32 );
            if( getWeather( ) == DARK_FLASH_USED ) {
                bgSetScale( IO::bg3, 1 << 7, 1 << 7 );
                bgSetScroll( IO::bg3, 64, 48 );
            }
            if( getWeather( ) == DARK_FLASH_1 ) {
                bgSetScale( IO::bg3, 1 << 7 | 1 << 6 | 1 << 5, 1 << 7 | 1 << 6 | 1 << 5 );
                bgSetScroll( IO::bg3, 112 - 96, 84 - 72 );
            }
            if( getWeather( ) == DARK_FLASH_2 ) {
                bgSetScale( IO::bg3, 1 << 7 | 1 << 6, 1 << 7 | 1 << 6 );
                bgSetScroll( IO::bg3, 96 - 64, 72 - 48 );
            }
            _weatherFollow = false;
            break;
        default:
            IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 3, 0 );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
            break;
        }
        bgSetPriority( IO::bg3, 0 );
    }

    void mapDrawer::changeWeather( mapWeather p_newWeather ) {
        if( getWeather( ) != p_newWeather ) {
            SAVE::SAV.getActiveFile( ).m_currentMapWeather = p_newWeather;
            for( auto fn : _newWeatherCallbacks ) { fn( getWeather( ) ); }
            initWeather( );
            if( REG_BLDALPHA ) {
                REG_BLDCNT = WEATHER_BLEND;
            } else {
                REG_BLDCNT = BLEND_NONE;
            }
        }
    }

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            ANIMATE_MAP = false;

            videoSetMode( MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE
                          | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                          | ( ( DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_128 | DISPLAY_SPR_1D_BMP
                                | DISPLAY_SPR_1D_BMP_SIZE_128 | ( 5 << 28 ) | 2 )
                              & 0xffffff0 ) );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );

            FADE_TOP_DARK( );
            dmaFillHalfWords( 0, BG_PALETTE, 512 );
            bgUpdate( );

            _curX = _curY = 0;

            u16 mx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                my = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE, my / SIZE,
                            &_slices[ _curX ][ _curY ], _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE, my / SIZE,
                             _data[ _curX ][ _curY ] );
            runLevelScripts( _data[ _curX ][ _curY ], mx / SIZE, my / SIZE );
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                            my / SIZE, &_slices[ _curX ^ 1 ][ _curY ], _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                             my / SIZE, _data[ _curX ^ 1 ][ _curY ] );
            runLevelScripts( _data[ _curX ^ 1 ][ _curY ], mx / SIZE + currentHalf( mx ),
                             my / SIZE );
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE,
                            my / SIZE + currentHalf( my ), &_slices[ _curX ][ _curY ^ 1 ],
                            _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE,
                             my / SIZE + currentHalf( my ), _data[ _curX ][ _curY ^ 1 ] );
            runLevelScripts( _data[ _curX ][ _curY ^ 1 ], mx / SIZE,
                             my / SIZE + currentHalf( my ) );
            constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                            my / SIZE + currentHalf( my ), &_slices[ _curX ^ 1 ][ _curY ^ 1 ],
                            _slices );
            FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                             my / SIZE + currentHalf( my ), _data[ _curX ^ 1 ][ _curY ^ 1 ] );
            runLevelScripts( _data[ _curX ^ 1 ][ _curY ^ 1 ], mx / SIZE + currentHalf( mx ),
                             my / SIZE + currentHalf( my ) );

            for( u8 i = 1; i < 4; ++i ) {
                bgInit( i - 1, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetScroll( i - 1, 120, 40 );
            }
            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
            dmaCopy( CUR_SLICE.m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );

            // for palettes, the unchanged day-time pal comes first
            u8 currDT = ( getCurrentDaytime( ) + 3 ) % 5;
            if( ( CUR_DATA.m_mapType & INSIDE ) || ( CUR_DATA.m_mapType & CAVE ) ) { currDT = 0; }
            BG_PALETTE[ 0 ] = 0;

            for( u8 i = 1; i < 4; ++i ) {
                mapMemory[ i ] = (u16*) BG_MAP_RAM( 2 * i - 1 );
                bgSetPriority( i - 1, i );
            }
            // reset frame animation of objects
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto& o                            = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
                o.second.m_currentMovement.m_frame = 0;
            }

            for( u8 i = 0; i < 4; ++i ) {
                constructAndAddNewMapObjects( _data[ i % 2 ][ i / 2 ],
                                              _slices[ i % 2 ][ i / 2 ].m_x,
                                              _slices[ i % 2 ][ i / 2 ].m_y );
            }

            dmaCopy( CUR_SLICE.m_pals + currDT * 16, BG_PALETTE, 512 - 32 );
            initWeather( );
            BG_PALETTE[ 0 ] = 0;

            _mapSprites.reset( );
            // Restore the map objects
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                loadMapObject( SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] );
            }
            if( _pkmnFollowsPlayer ) {
                if( updateFollowPkmn( ) ) {
                    std::pair<u8, mapObject> cur = { _playerFollowPkmnSprite, _followPkmn };
                    loadMapObject( cur );
                    _followPkmn             = cur.second;
                    _playerFollowPkmnSprite = cur.first;
                } else {
                    removeFollowPkmn( );
                }
            }
            if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                attachMapObjectToPlayer( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx );
            }
        }

        _lastrow = NUM_ROWS - 1;
        _lastcol = NUM_COLS - 1;

        _cx = p_globX;
        _cy = p_globY;

        u16 mny = p_globY - 8;
        u16 mnx = p_globX - 15;

        ANIMATE_MAP = false;
        for( u16 y = 0; y < NUM_ROWS; y++ )
            for( u16 x = 0; x < NUM_COLS; x++ ) { loadBlock( at( mnx + x, mny + y ), x, y ); }

        bgUpdate( );
        ANIMATE_MAP = true;
    }

    void mapDrawer::draw( ObjPriority, bool p_playerHidden ) {
        draw( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
              SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY, true ); // Draw the map

        drawPlayer( SAVE::SAV.getActiveFile( ).m_playerPriority,
                    p_playerHidden ); // Draw the player

        for( auto fn : _newBankCallbacks ) { fn( SAVE::SAV.getActiveFile( ).m_currentMap ); }
        auto curLocId = getCurrentLocationId( );
        for( auto fn : _newLocationCallbacks ) { fn( curLocId ); }

        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ, false, true );
    }

    void mapDrawer::drawPlayer( ObjPriority p_playerPrio, bool p_playerHidden ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ),
                                                p_playerHidden );
        changeMoveMode( SAVE::SAV.getActiveFile( ).m_player.m_movement );
        _mapSprites.setPriority(
            _playerSprite, SAVE::SAV.getActiveFile( ).m_playerPriority = p_playerPrio, false );
        _mapSprites.setVisibility( _playerSprite, p_playerHidden );
    }

    void mapDrawer::fixMapObject( u8 p_objectId ) {
        _fixedMapObjects.insert( p_objectId );
    }
    void mapDrawer::unfixMapObject( u8 p_objectId ) {
        _fixedMapObjects.erase( p_objectId );
    }

    void mapDrawer::showExclamationAboveMapObject( u8 p_objectId ) {
        auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ];
        _mapSprites.showExclamation( o.first );
        SOUND::playSoundEffect( SFX_EXMARK );
        for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }
        _mapSprites.hideExclamation( );
    }

    void mapDrawer::moveMapObject( mapObject& p_mapObject, u8 p_spriteId, movement p_movement,
                                   bool p_movePlayer, direction p_playerMovement,
                                   bool p_adjustAnim ) {
        // redirect object
        if( p_movement.m_frame == 0 ) {
            _mapSprites.setFrameD( p_spriteId, p_movement.m_direction );
            if( p_movePlayer ) { _mapSprites.setFrameD( _playerSprite, p_playerMovement ); }
        }
        if( p_movement.m_frame == 15 ) {
            _mapSprites.drawFrameD( p_spriteId, p_movement.m_direction );

            if( p_movePlayer ) { _mapSprites.drawFrameD( _playerSprite, p_playerMovement ); }
        }

        for( u8 i = 0; i < 16; ++i ) {
            if( i == p_movement.m_frame ) {
                if( p_movePlayer ) { moveCamera( p_playerMovement, true ); }
                _mapSprites.moveSprite( p_spriteId, p_movement.m_direction, 1 );
                if( i == 8 ) {
                    _mapSprites.nextFrame( p_spriteId );
                    if( p_movePlayer ) { _mapSprites.nextFrame( _playerSprite ); }
                }
            }
        }
        if( p_movement.m_frame == 0 ) {
            if( p_movePlayer ) {
                animateField( p_mapObject.m_pos.m_posX, p_mapObject.m_pos.m_posY );

                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX = p_mapObject.m_pos.m_posX;
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY = p_mapObject.m_pos.m_posY;
            }
            p_mapObject.m_pos.m_posX += dir[ p_movement.m_direction ][ 0 ];
            p_mapObject.m_pos.m_posY += dir[ p_movement.m_direction ][ 1 ];

            auto px = p_mapObject.m_pos.m_posX;
            auto py = p_mapObject.m_pos.m_posY;

            animateField( px, py );

            if( p_adjustAnim && _tileAnimations.count( { px, py, 0 } ) ) {
                // this function may get called while the player is moving, so the player may
                // be at a fractional grid point and we need to fix this shift by hand
                // This is extremely hacky, I know
                _mapSprites.moveSprite( _tileAnimations[ { px, py, 0 } ], p_spriteId, false );
                _mapSprites.moveSprite( _tileAnimations[ { px, py, 0 } ],
                                        15 * dir[ p_movement.m_direction ][ 0 ],
                                        15 * dir[ p_movement.m_direction ][ 1 ], true );
            }
        }
        if( p_movement.m_frame == 15 ) {
            // clear remnants of field animation on old tile
            clearFieldAnimation( p_mapObject.m_pos.m_posX - dir[ p_movement.m_direction ][ 0 ],
                                 p_mapObject.m_pos.m_posY - dir[ p_movement.m_direction ][ 1 ] );
        }
    }

    void mapDrawer::moveMapObject( u8 p_objectId, movement p_movement, bool p_movePlayer,
                                   direction p_playerMovement, bool p_adjustAnim ) {
        moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second,
                       SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].first, p_movement,
                       p_movePlayer, p_playerMovement, p_adjustAnim );
    }

    void mapDrawer::clearFieldAnimation( u16 p_globX, u16 p_globY ) {
        position p = { p_globX, p_globY, 0 };
        if( !_tileAnimations.count( p ) ) {
            return;
        } else {
            _mapSprites.destroySprite( _tileAnimations[ p ] );
            _tileAnimations.erase( p );
        }
    }

    u8 mapDrawer::getTileAnimation( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        switch( behave ) {
        case 0x2:
        case 0x24: return mapSpriteManager::SPR_GRASS;
        case 0x3: return mapSpriteManager::SPR_LONG_GRASS;
        default: return 0;
        }
    }

    u8 mapDrawer::animateField( u16 p_globX, u16 p_globY, u8 p_animation, u8 p_frame ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( !p_frame ) {
            auto res = _mapSprites.loadSprite( curx, cury, p_globX, p_globY, p_animation );
            _mapSprites.drawFrame( res, 1 );
            if( p_animation == mapSpriteManager::SPR_LONG_GRASS ) {
                _mapSprites.setPriority( res, OBJPRIORITY_1 );
            }
            return res;
        } else {
            _mapSprites.drawFrame( p_animation, p_frame );
            _mapSprites.setPriority( p_animation, OBJPRIORITY_1 );
            return p_animation;
        }
    }

    void mapDrawer::animateField( u16 p_globX, u16 p_globY, u8 p_animation ) {
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( _tileAnimations.count( { p_globX, p_globY, 0 } ) ) {
            clearFieldAnimation( p_globX, p_globY );
        }
        _tileAnimations[ { p_globX, p_globY, 0 } ]
            = _mapSprites.loadSprite( curx, cury, p_globX, p_globY, p_animation );
    }

    void mapDrawer::setBlock( u16 p_globX, u16 p_globY, u16 p_newBlock ) {
        if( p_newBlock > 2 * MAX_BLOCKS_PER_TILE_SET ) [[unlikely]] { return; }
        atom( p_globX, p_globY ).m_blockidx = p_newBlock;

        auto curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        auto cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        if( std::abs( curx - p_globX ) > NUM_COLS / 2
            || std::abs( cury - p_globY ) > NUM_ROWS / 2 ) {
            // A non-visible block got changed, we should not draw it on-screen.
            return;
        }

        loadBlock( at( p_globX, p_globY ), ( _lastcol + NUM_COLS / 2 ) % NUM_COLS,
                   ( _lastrow + NUM_ROWS / 2 + 1 ) % NUM_ROWS );
        bgUpdate( );
    }

    void mapDrawer::animateField( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;
        u8 anim   = getTileAnimation( p_globX, p_globY );

        switch( behave ) {
        case 0x2:
        case 0x3: {
            // tall grass
            animateField( p_globX, p_globY, anim );
            break;
        }
        case 0x24: {
            // ashen grass
            animateField( p_globX, p_globY, anim );
            if( CUR_SLICE.m_tIdx2 == 6 ) {
                setBlock( p_globX, p_globY, 0x206 );
            } else if( CUR_SLICE.m_tIdx2 == 7 ) {
                setBlock( p_globX, p_globY, 0x212 );
            }
            break;
        }
        case 0xd2: {
            // breakable floor
            // breaks on step on
            setBlock( p_globX, p_globY, 0x206 );
        }
        default: break;
        }

        // TODO
    }

    void mapDrawer::stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn,
                            bool p_unfade ) {
        animateField( p_globX, p_globY );
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        auto curLocId = getCurrentLocationId( );
        for( auto fn : _newLocationCallbacks ) { fn( curLocId ); }

        // Check for things that activate upon stepping on a tile

        switch( behave ) {
        case 0x24: { // Add ash to the soot bag
            if( SAVE::SAV.getActiveFile( ).m_bag.count( BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ),
                                                        I_SOOT_SACK ) ) {
                SAVE::SAV.getActiveFile( ).m_ashCount++;
                if( SAVE::SAV.getActiveFile( ).m_ashCount > 999'999'999 ) {
                    SAVE::SAV.getActiveFile( ).m_ashCount = 999'999'999;
                }
            }
            break;
        }
        default: break;
        }

        if( p_allowWildPkmn && !_scriptRunning ) {
            bool hadBattle = false;
            // Check for trainer
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

                if( o.second.m_event.m_type == EVENT_TRAINER ) [[unlikely]] {
                    // Check if trainer can see player

                    if( std::abs( p_globX - o.second.m_pos.m_posX ) > o.second.m_range )
                        [[likely]] {
                        continue;
                    }
                    if( std::abs( p_globY - o.second.m_pos.m_posY ) > o.second.m_range )
                        [[likely]] {
                        continue;
                    }
                    if( std::abs( p_globY - o.second.m_pos.m_posY )
                        && std::abs( p_globX - o.second.m_pos.m_posX ) ) [[likely]] {
                        continue;
                    }

                    direction trainerDir = UP;
                    direction playerDir  = DOWN;
                    if( p_globY > o.second.m_pos.m_posY ) {
                        trainerDir = DOWN;
                        playerDir  = UP;
                    }
                    if( p_globX < o.second.m_pos.m_posX ) {
                        trainerDir = LEFT;
                        playerDir  = RIGHT;
                    }
                    if( p_globX > o.second.m_pos.m_posX ) {
                        trainerDir = RIGHT;
                        playerDir  = LEFT;
                    }

                    if( trainerDir != o.second.m_direction ) { continue; }

                    // Check for exclamation mark / music change
                    if( !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_TRAINER_BATTLED(
                            o.second.m_event.m_data.m_trainer.m_trainerId ) ) ) [[likely]] {
                        // player did not defeat the trainer yet
                        auto tr = BATTLE::getBattleTrainer(
                            o.second.m_event.m_data.m_trainer.m_trainerId );

                        // Check if the battle would be a double battle; if so and if the
                        // player has only a single pkmn, the battle is optional
                        if( !BATTLE::isDoubleBattleTrainerClass( tr.m_data.m_trainerClass )
                            || SAVE::SAV.getActiveFile( ).countAlivePkmn( ) >= 2 ) {

                            SAVE::SAV.getActiveFile( ).m_mapObjects[ i ].second.m_movement
                                = NO_MOVEMENT;
                            showExclamationAboveMapObject( i );
                            SOUND::playBGM(
                                SOUND::BGMforTrainerEncounter( tr.m_data.m_trainerClass ) );

                            // walk trainer to player
                            redirectPlayer( playerDir, false );

                            while( dist( p_globX, p_globY, o.second.m_pos.m_posX,
                                         o.second.m_pos.m_posY )
                                   > 1 ) {
                                for( u8 j = 0; j < 16; ++j ) {
                                    moveMapObject( i, { trainerDir, j } );
                                    swiWaitForVBlank( );
                                }
                            }
                            runEvent( o.second.m_event, i );
                        }
                    }
                }
            }

            if( !hadBattle ) { handleWildPkmn( p_globX, p_globY ); }
        }

        if( p_unfade ) {
            IO::fadeScreen( IO::UNFADE );
            if( REG_BLDALPHA ) {
                REG_BLDCNT = WEATHER_BLEND;
            } else {
                REG_BLDCNT = BLEND_NONE;
            }
            bgUpdate( );
        }
        handleEvents( p_globX, p_globY, p_z );
    }

    void mapDrawer::loadNewRow( direction p_direction, bool p_updatePlayer ) {
        _cx += dir[ p_direction ][ 0 ];
        _cy += dir[ p_direction ][ 1 ];
#ifdef DESQUID_MORE
        assert( _cx != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                || _cy != SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY );
#endif
        if( p_updatePlayer ) { updatePlayer( ); }

        // Check if a new slice should be loaded
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 32 == 16 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 32 == 15 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 32 == 16 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 32 == 15 ) ) {

            // Hacky optimization: Don't load new slices on inside maps.
            if( ( CUR_DATA.m_mapType & CAVE ) || !( CUR_DATA.m_mapType & INSIDE ) ) {
                ANIMATE_MAP = false;
                DRAW_TIME   = false;
                loadSlice( p_direction );
                ANIMATE_MAP = true;
                DRAW_TIME   = true;
            }
#ifdef DESQUID_MORE
            NAV::printMessage( "Load Slice" );
#endif
        }

        // Check if map objects should be updated
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 16 == 8 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 16 == 7 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 16 == 8 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 16 == 7 ) ) {
            ANIMATE_MAP = false;
            for( u8 i = 0; i < 4; ++i ) {
                constructAndAddNewMapObjects( _data[ i % 2 ][ i / 2 ],
                                              _slices[ i % 2 ][ i / 2 ].m_x,
                                              _slices[ i % 2 ][ i / 2 ].m_y );
            }
            ANIMATE_MAP = true;
        }

        // Check if a new slice got stepped onto
        if( ( dir[ p_direction ][ 0 ] == 1 && _cx % 32 == 0 )
            || ( dir[ p_direction ][ 0 ] == -1 && _cx % 32 == 31 )
            || ( dir[ p_direction ][ 1 ] == 1 && _cy % 32 == 0 )
            || ( dir[ p_direction ][ 1 ] == -1 && _cy % 32 == 31 ) ) {

            u8 oldts1 = CUR_SLICE.m_tIdx1;
            u8 oldts2 = CUR_SLICE.m_tIdx2;

            _curX = ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1;
            _curY = ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1;
            // Update tileset, block and palette data

            u8 newts1 = CUR_SLICE.m_tIdx1;
            u8 newts2 = CUR_SLICE.m_tIdx2;

            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );

            ANIMATE_MAP = false;
            if( oldts1 != newts1 && oldts2 != newts2 ) {
                dmaCopy( CUR_SLICE.m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );
            } else if( oldts2 != newts2 ) {
                dmaCopy( &CUR_SLICE.m_tileSet.m_tiles[ MAX_TILES_PER_TILE_SET ],
                         &tileMemory[ MAX_TILES_PER_TILE_SET * 32 ], MAX_TILES_PER_TILE_SET * 32 );
            }

            // for palettes, the unchanged day-time pal comes first
            u8 currDT = ( getCurrentDaytime( ) + 3 ) % 5;
            if( ( CUR_DATA.m_mapType & INSIDE ) || ( CUR_DATA.m_mapType & CAVE ) ) { currDT = 0; }
            dmaCopy( CUR_SLICE.m_pals + currDT * 16, BG_PALETTE, 512 - 32 );
            BG_PALETTE[ 0 ] = 0;
            ANIMATE_MAP     = true;

#ifdef DESQUID_MORE
            char buffer[ 100 ];
            snprintf( buffer, 99, "Switch Slice to (%d, %d)", _curX, _curY );
            NAV::printMessage( buffer );
#endif
        }

        switch( p_direction ) {
        case UP: {
            u16 ty  = _cy - 8;
            u16 mnx = _cx - 15;
            for( u16 x = ( _lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, _lastrow );
            _lastrow = ( _lastrow + NUM_ROWS - 1 ) % NUM_ROWS;
            break;
        }
        case LEFT: {
            u16 tx  = _cx - 15;
            u16 mny = _cy - 8;
            for( u16 y = ( _lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), _lastcol, y );
            _lastcol = ( _lastcol + NUM_COLS - 1 ) % NUM_COLS;
            break;
        }
        case DOWN: {
            _lastrow = ( _lastrow + 1 ) % NUM_ROWS;
            u16 ty   = _cy + 7;
            u16 mnx  = _cx - 15;
            for( u16 x = ( _lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, _lastrow );
            break;
        }
        case RIGHT: {
            _lastcol = ( _lastcol + 1 ) % NUM_COLS;
            u16 tx   = _cx + 16;
            u16 mny  = _cy - 8;
            for( u16 y = ( _lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), _lastcol, y );
            break;
        }
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows ) {
        for( u8 i = 0; i < 4; ++i ) {
            if( i == IO::bg3 && !_weatherFollow ) { continue; }
            bgScroll( i, dir[ p_direction ][ 0 ], dir[ p_direction ][ 1 ] );
        }
        bgUpdate( );
        _mapSprites.moveCamera( p_direction, 1, !p_updatePlayer );
        if( p_autoLoadRows
            && ( ( dir[ p_direction ][ 0 ]
                   && ( ( bgState[ 1 ].scrollX >> 8 ) - !!dir[ p_direction ][ 0 ] + 9 ) % 16 == 0 )
                 || ( dir[ p_direction ][ 1 ]
                      && ( ( bgState[ 1 ].scrollY >> 8 ) - !!dir[ p_direction ][ 1 ] + 9 ) % 16
                             == 0 ) ) ) {
            loadNewRow( p_direction, p_updatePlayer );
        }
    }

    void mapDrawer::loadSlice( direction p_direction ) {
        auto mx = CUR_SLICE.m_x + dir[ p_direction ][ 0 ],
             my = CUR_SLICE.m_y + dir[ p_direction ][ 1 ];

        constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                        &_slices[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                                [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                        _slices );
        FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                         _data[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                              [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ] );
        runLevelScripts( _data[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                              [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                         mx, my );

        auto& neigh = _slices[ ( _curX + !dir[ p_direction ][ 0 ] ) & 1 ]
                             [ ( _curY + !dir[ p_direction ][ 1 ] ) & 1 ];
        mx = neigh.m_x + dir[ p_direction ][ 0 ];
        my = neigh.m_y + dir[ p_direction ][ 1 ];
        constructSlice( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                        &_slices[ _curX ^ 1 ][ _curY ^ 1 ], _slices );
        FS::readMapData( SAVE::SAV.getActiveFile( ).m_currentMap, mx, my,
                         _data[ _curX ^ 1 ][ _curY ^ 1 ] );
        runLevelScripts( _data[ _curX ^ 1 ][ _curY ^ 1 ], mx, my );
    }

    void mapDrawer::disablePkmn( s16 p_steps ) {
        SAVE::SAV.getActiveFile( ).m_repelSteps = p_steps;
    }

    void mapDrawer::enablePkmn( ) {
        SAVE::SAV.getActiveFile( ).m_repelSteps = 0;
    }

    void mapDrawer::animateDoor( u16 p_globX, u16 p_globY, u8 p_z, bool p_close ) {
        u16  curx  = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  cury  = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        auto wdata = getWarpData( p_globX, p_globY, p_z );
        if( !wdata.first ) { return; }

        if( wdata.second.m_warp.m_warpType == DOOR ) {
            SOUND::playSoundEffect( SFX_ENTER_DOOR );
        } else if( wdata.second.m_warp.m_warpType == SLIDING_DOOR ) {
            SOUND::playSoundEffect( SFX_SLIDING_DOOR );
        } else {
            // Not a door?
            return;
        }

        auto block = atom( p_globX, p_globY );
        u8   ts    = 0;

        if( block.m_blockidx < MAX_BLOCKS_PER_TILE_SET ) {
            ts = CUR_SLICE.m_tIdx1;
        } else {
            ts = CUR_SLICE.m_tIdx2;
        }

        for( u8 i = 0; i < DOOR_ANIMATION_COUNT; ++i ) {
            auto d = DOOR_ANIMATIONS[ i ];
            if( d.m_tileset != ts || d.m_blockIdx != block.m_blockidx ) { continue; }

            u8 door = _mapSprites.loadDoor( curx, cury, p_globX, p_globY, d.m_doorIdx,
                                            &BG_PALETTE[ 16 * d.m_palette ] );
            if( !p_close ) {
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 1 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 2 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
            } else {
                _mapSprites.drawFrame( door, 2 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 1 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.drawFrame( door, 0 );
                for( u8 j = 0; j < 8; ++j ) { swiWaitForVBlank( ); }
                _mapSprites.destroySprite( door );
            }
            return;
        }

        // no animation found :/
    }

    void mapDrawer::openDoor( u16 p_globX, u16 p_globY, u8 p_z ) {
        animateDoor( p_globX, p_globY, p_z, false );
    }

    void mapDrawer::closeDoor( u16 p_globX, u16 p_globY, u8 p_z ) {
        animateDoor( p_globX, p_globY, p_z, true );
    }

    std::pair<bool, mapData::event::data> mapDrawer::getWarpData( u16 p_globX, u16 p_globY,
                                                                  u8 p_z ) {
        auto data = currentData( p_globX, p_globY );
        p_globX %= SIZE;
        p_globY %= SIZE;

        for( u8 i = 0; i < data.m_eventCount; ++i ) {
            if( data.m_events[ i ].m_type == EVENT_WARP && data.m_events[ i ].m_posX == p_globX
                && data.m_events[ i ].m_posY == p_globY && data.m_events[ i ].m_posZ == p_z ) {
                if( data.m_events[ i ].m_activateFlag
                    && !SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_activateFlag ) ) {
                    continue;
                }
                if( data.m_events[ i ].m_deactivateFlag
                    && SAVE::SAV.getActiveFile( ).checkFlag(
                        data.m_events[ i ].m_deactivateFlag ) ) {
                    continue;
                }

                return { true, data.m_events[ i ].m_data };
            }
        }
        return { false, mapData::event::data( ) };
    }

    void mapDrawer::handleWarp( warpType p_type, warpPos p_source ) {
        warpPos tg;
        u16     curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16     cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16     curz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;

        auto wdata = getWarpData( curx, cury, curz );
        curx %= SIZE;
        cury %= SIZE;
        if( !wdata.first ) { return; }

        if( wdata.second.m_warp.m_warpType != NO_SPECIAL ) {
            p_type = wdata.second.m_warp.m_warpType;
            if( wdata.second.m_warp.m_warpType == LAST_VISITED ) {
                tg = SAVE::SAV.getActiveFile( ).m_lastWarp;
            } else {
                tg = warpPos(
                    wdata.second.m_warp.m_bank,
                    position( wdata.second.m_warp.m_mapX * SIZE + wdata.second.m_warp.m_posX,
                              wdata.second.m_warp.m_mapY * SIZE + wdata.second.m_warp.m_posY,
                              +wdata.second.m_warp.m_posZ ) );
            }
        }

        if( tg.first == 0xFF ) tg = SAVE::SAV.getActiveFile( ).m_lastWarp;
        if( !tg.first && !tg.second.m_posY && !tg.second.m_posZ && !tg.second.m_posX ) return;

        SAVE::SAV.getActiveFile( ).m_lastWarp = p_source;
        warpPlayer( p_type, tg );
    }

    void mapDrawer::handleWarp( warpType p_type ) {
        warpPos current = warpPos{ SAVE::SAV.getActiveFile( ).m_currentMap,
                                   SAVE::SAV.getActiveFile( ).m_player.m_pos };
        if( p_type == LAST_VISITED ) {
            warpPos target = SAVE::SAV.getActiveFile( ).m_lastWarp;
            if( !target.first && !target.second.m_posX && !target.second.m_posY
                && !target.second.m_posZ )
                return;
            SAVE::SAV.getActiveFile( ).m_lastWarp = current;

            warpPlayer( p_type, target );
        } else {
            handleWarp( p_type, current );
        }
    }

    void mapDrawer::handleWildPkmn( u16 p_globX, u16 p_globY ) {
        u8 moveData = atom( p_globX, p_globY ).m_movedata;
        u8 behave   = at( p_globX, p_globY ).m_bottombehave;

        if( SAVE::SAV.getActiveFile( ).m_repelSteps ) return;
        // handle Pkmn stuff
        if( moveData == 0x04 && behave != 0x13 )
            handleWildPkmn( WATER );
        else if( behave == 0x02 || behave == 0x24 || behave == 0x06 || behave == 0x08 )
            handleWildPkmn( GRASS );
        else if( behave == 0x03 )
            handleWildPkmn( HIGH_GRASS );
        //        else if( CUR_DATA.m_mapType & CAVE )
        //            handleWildPkmn( GRASS );
    }

    pokemon wildPkmn;
    bool    mapDrawer::handleWildPkmn( wildPkmnType p_type, bool p_forceEncounter ) {
        u16 rn
            = rand( ) % ( 512 + 3 * SAVE::SAV.getActiveFile( ).m_options.m_encounterRateModifier );
        if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) rn /= 8;
        if( p_forceEncounter ) rn %= 40;

        u8 tier;
        if( rn < 2 )
            tier = 4;
        else if( rn < 6 )
            tier = 3;
        else if( rn < 14 )
            tier = 2;
        else if( rn < 26 )
            tier = 1;
        else
            tier = 0;
        u8 level = SAVE::SAV.getActiveFile( ).getEncounterLevel( tier );

        if( rn > 40 || !level ) {
            if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) {
                _playerIsFast = false;
                NAV::printMessage( GET_STRING( 5 ) );
            }
            return false;
        }
        if( p_type == OLD_ROD || p_type == GOOD_ROD || p_type == SUPER_ROD ) {
            _playerIsFast = false;
            NAV::printMessage( GET_STRING( 6 ) );
        } else if( SAVE::SAV.getActiveFile( ).m_repelSteps && !p_forceEncounter ) {
            return false;
        }

        s8 availmod = ( SAVE::SAV.getActiveFile( ).m_options.getDifficulty( ) - 3 ) / 3;

        u8 total = 0;
        for( u8 i = 0; i < CUR_DATA.m_pokemonDescrCount; ++i ) {
            if( CUR_DATA.m_pokemon[ i ].m_encounterType == p_type ) {
                s8 ownedbadge = SAVE::SAV.getActiveFile( ).getBadgeCount( ) + availmod;
                if( ownedbadge < 0 ) { ownedbadge = 0; }

                if( ownedbadge >= CUR_DATA.m_pokemon[ i ].m_slot ) {

                    if( CUR_DATA.m_pokemon[ i ].m_daytime
                        & ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) {
                        total += CUR_DATA.m_pokemon[ i ].m_encounterRate;
                    } else {
#ifdef DESQUID_MORE
                        NAV::printMessage(
                            ( std::string( "Ignoring pkmn due to wrong time: " )
                              + std::to_string( i ) + " "
                              + std::to_string( CUR_DATA.m_pokemon[ i ].m_daytime ) + " vs "
                              + std::to_string( ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) )
                                .c_str( ),
                            MSG_INFO );
#endif
                    }
                } else {
#ifdef DESQUID_MORE
                    NAV::printMessage( ( std::string( "Ignoring pkmn due to insufficient badges: " )
                                         + std::to_string( i ) )
                                           .c_str( ),
                                       MSG_INFO );
#endif
                }
            }
        }
        if( !total ) {
#ifdef DESQUID_MORE
            NAV::printMessage( "No pkmn", MSG_INFO );
#endif
            return false;
        }

#ifdef DESQUID_MORE
        NAV::printMessage( ( std::to_string( total ) ).c_str( ), MSG_INFO );
#endif
        u16 pkmnId      = 0;
        u16 backup      = 0;
        u8  pkmnForme   = 0;
        u8  backupForme = 0;

        u8 res = rand( ) % total;
        total  = 0;
        for( u8 i = 0; i < CUR_DATA.m_pokemonDescrCount; ++i ) {
            if( CUR_DATA.m_pokemon[ i ].m_encounterType == p_type ) {
                s8 ownedbadge = SAVE::SAV.getActiveFile( ).getBadgeCount( ) + availmod;
                if( ownedbadge < 0 ) { ownedbadge = 0; }

                if( ownedbadge >= CUR_DATA.m_pokemon[ i ].m_slot
                    && ( CUR_DATA.m_pokemon[ i ].m_daytime
                         & ( 1 << ( getCurrentDaytime( ) % 4 ) ) ) ) {
                    total += CUR_DATA.m_pokemon[ i ].m_encounterRate;

                    // if the player hasn't obtained the nat dex yet, they should only see
                    // pkmn that are in the local dex
                    if( SAVE::SAV.getActiveFile( ).getPkmnDisplayDexId(
                            CUR_DATA.m_pokemon[ i ].m_speciesId )
                        != u16( -1 ) ) {
                        backup      = CUR_DATA.m_pokemon[ i ].m_speciesId;
                        backupForme = CUR_DATA.m_pokemon[ i ].m_forme;
                    } else {
                        continue;
                    }

                    if( total > res ) {
                        pkmnId    = CUR_DATA.m_pokemon[ i ].m_speciesId;
                        pkmnForme = CUR_DATA.m_pokemon[ i ].m_forme;
#ifdef DESQUID_MORE
                        NAV::printMessage(
                            ( std::to_string( pkmnId ) + " " + std::to_string( pkmnForme ) )
                                .c_str( ),
                            MSG_INFO );
#endif
                        break;
                    }
                }
            }
        }

        if( !pkmnId ) {
            pkmnId    = backup;
            pkmnForme = backupForme;
        }
        if( !pkmnId ) { return false; }

        if( pkmnId == PKMN_PIKACHU && !( rand( ) & 0xff ) ) { pkmnId = PKMN_MIMIKYU; }

        ANIMATE_MAP = false;
        DRAW_TIME   = false;

        bool luckyenc = SAVE::SAV.getActiveFile( ).m_bag.count(
                            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_WISHING_CHARM )
                            ? !( rand( ) & 127 )
                            : !( rand( ) & 2047 );
        bool charm    = SAVE::SAV.getActiveFile( ).m_bag.count(
            BAG::toBagType( ITEM::ITEMTYPE_KEYITEM ), I_SHINY_CHARM );

        if( luckyenc ) {
            SOUND::playBGM( MOD_BATTLE_WILD_ALT );
        } else {
            SOUND::playBGM( SOUND::BGMforWildBattle( pkmnId ) );
        }
        _playerIsFast = false;
        fastBike      = false;
        _mapSprites.setFrameD( _playerSprite, SAVE::SAV.getActiveFile( ).m_player.m_direction );

        IO::fadeScreen( IO::BATTLE );
        IO::BG_PAL( true )[ 0 ] = 0;
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3sub ), 256 * 192 );

        wildPkmn = pokemon( pkmnId, level, pkmnForme, 0, luckyenc ? 255 : ( charm ? 3 : 0 ),
                            luckyenc, false, 0, 0, luckyenc );

        u8 platform = 0, plat2 = 0;
        u8 battleBack = p_type == WATER ? CUR_DATA.m_surfBattleBG : CUR_DATA.m_battleBG;
        switch( p_type ) {
        case WATER:
            platform = CUR_DATA.m_surfBattlePlat1;
            plat2    = CUR_DATA.m_surfBattlePlat2;
            break;
        case OLD_ROD:
        case GOOD_ROD:
        case SUPER_ROD:
            platform = CUR_DATA.m_battlePlat1;
            plat2    = CUR_DATA.m_surfBattlePlat2;
            break;

        default:
            platform = CUR_DATA.m_battlePlat1;
            plat2    = CUR_DATA.m_battlePlat2;
            break;
        }

        auto playerPrio = _mapSprites.getPriority( _playerSprite );
        swiWaitForVBlank( );
        if( BATTLE::battle( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                            SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ), wildPkmn, platform,
                            plat2, battleBack, getBattlePolicy( true ) )
                .start( )
            == BATTLE::battle::BATTLE_OPPONENT_WON ) {
            faintPlayer( );
            return true;
        }
        SOUND::restartBGM( );
        FADE_TOP_DARK( );
        draw( playerPrio );
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = playerPrio );
        NAV::init( );

        ANIMATE_MAP = true;
        DRAW_TIME   = true;
        return true;
    }

    BATTLE::battlePolicy mapDrawer::getBattlePolicy( bool p_isWildBattle, BATTLE::battleMode p_mode,
                                                     bool p_distributeEXP ) {
        BATTLE::battlePolicy res = p_isWildBattle
                                       ? BATTLE::battlePolicy( BATTLE::DEFAULT_WILD_POLICY )
                                       : BATTLE::battlePolicy( BATTLE::DEFAULT_TRAINER_POLICY );

        res.m_mode               = p_mode;
        res.m_distributeEXP      = p_distributeEXP;
        res.m_allowMegaEvolution = SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_MEGA_EVOLUTION );

        res.m_weather = BATTLE::weather::NO_WEATHER;
        switch( getWeather( ) ) {
        case SUNNY: res.m_weather = BATTLE::weather::SUN; break;
        case RAINY:
        case THUNDERSTORM: res.m_weather = BATTLE::weather::RAIN; break;
        case SNOW:
        case BLIZZARD: res.m_weather = BATTLE::weather::HAIL; break;
        case SANDSTORM: res.m_weather = BATTLE::weather::SANDSTORM; break;
        case FOG: res.m_weather = BATTLE::weather::FOG; break;
        case HEAVY_SUNLIGHT: res.m_weather = BATTLE::weather::HEAVY_SUNSHINE; break;
        case HEAVY_RAIN: res.m_weather = BATTLE::weather::HEAVY_RAIN; break;
        default: break;
        }

        return res;
    }

    bool mapDrawer::requestWildPkmn( bool p_forceHighGrass ) {
        u8 moveData = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        u8 behave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                        .m_bottombehave;

        if( moveData == 0x04 && behave != 0x13 )
            return handleWildPkmn( WATER, true );
        else if( behave == 0x02 && !p_forceHighGrass )
            return handleWildPkmn( GRASS, true );
        else if( ( behave == 0x24 || behave == 0x06 ) && !p_forceHighGrass )
            return handleWildPkmn( GRASS, true );
        else if( behave == 0x03 || p_forceHighGrass )
            return handleWildPkmn( HIGH_GRASS, true );
        else if( CUR_DATA.m_mapType & CAVE )
            return handleWildPkmn( GRASS, true );
        return false;
    }

    void mapDrawer::animateMap( u8 p_frame ) {
        // animate weather
        if( _weatherScrollX || _weatherScrollY ) {
            bgScrollf( IO::bg3, ( _weatherScrollX << 8 ) / 10, ( _weatherScrollY << 8 ) / 10 );
            bgUpdate( );
        }

        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u16 cx2 = curx, cy2 = cury;

        if( _pkmnFollowsPlayer ) {
            cx2 = _followPkmn.m_pos.m_posX;
            cy2 = _followPkmn.m_pos.m_posY;
        }

        // animate map objects

        bool change = false;
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.first == 255 ) { continue; }

            if( o.second.m_event.m_type == EVENT_BERRYTREE ) {
                if( ( p_frame & 31 ) == 15 ) { _mapSprites.nextFrame( o.first ); }
                continue;
            }

            if( o.second.m_movement == NO_MOVEMENT ) { continue; }

            if( o.second.m_movement <= 15 || o.second.m_movement == WALK_AROUND_LEFT_RIGHT
                || o.second.m_movement == WALK_AROUND_UP_DOWN
                || o.second.m_movement == WALK_AROUND_SQUARE ) {
                if( ( p_frame & 127 ) == 127 ) {
                    o.second.m_direction = getRandomLookDirection( o.second.m_movement );
                    _mapSprites.setFrameD( o.first, o.second.m_direction, false );
                    change = true;
                }
            }

            if( change ) { continue; }

            if( o.second.m_currentMovement.m_frame ) {
                moveMapObject( i, o.second.m_currentMovement );
                o.second.m_currentMovement.m_frame
                    = ( o.second.m_currentMovement.m_frame + 1 ) & 15;
                continue;
            }

            u8 rndir = rand( ) & 1;

            bool movemnt
                = ( p_frame & 127 ) == 63
                  || ( ( p_frame & 15 ) == 15 && o.second.m_movement == WALK_CONT_LEFT_RIGHT )
                  || ( ( p_frame & 15 ) == 15 && o.second.m_movement == WALK_CONT_UP_DOWN )
                  || ( ( p_frame & 15 ) == 15 && o.second.m_movement == WALK_CONT_FOLLOW_OBJECT );

            if( o.second.m_movement == WALK_CONT_FOLLOW_OBJECT ) {
                if( movemnt ) {
                    auto curdir = o.second.m_currentMovement.m_direction;
                    auto nxdir  = direction( ( curdir + 1 ) % 4 );

                    // check if the object could do a right turn (ignoring any events)
                    if( canMove( o.second.m_pos, nxdir, WALK, false ) ) {
                        o.second.m_currentMovement = { nxdir, 0 };
                    }

                    auto nox = o.second.m_pos.m_posX
                               + dir[ o.second.m_currentMovement.m_direction ][ 0 ];
                    auto noy = o.second.m_pos.m_posY
                               + dir[ o.second.m_currentMovement.m_direction ][ 1 ];

                    // Don't glitch through the player or other objects
                    if( canMove( o.second.m_pos, o.second.m_currentMovement.m_direction, WALK )
                        && ( nox != curx || noy != cury ) && ( nox != cx2 || noy != cy2 ) ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    } else {
                        o.second.m_currentMovement = { curdir, 0 };
                    }
                }
            }

            if( o.second.m_movement == WALK_AROUND_LEFT_RIGHT
                || o.second.m_movement == WALK_LEFT_RIGHT
                || o.second.m_movement == WALK_CONT_LEFT_RIGHT
                || ( o.second.m_movement == WALK_AROUND_SQUARE && rndir ) ) {
                if( movemnt ) {
                    bool nomove = false;

                    auto nxl = o.second.m_pos.m_posX + dir[ LEFT ][ 0 ];
                    auto nyl = o.second.m_pos.m_posY + dir[ LEFT ][ 1 ];
                    auto nxr = o.second.m_pos.m_posX + dir[ RIGHT ][ 0 ];
                    auto nyr = o.second.m_pos.m_posY + dir[ RIGHT ][ 1 ];
                    auto nox = o.second.m_pos.m_posX
                               + dir[ o.second.m_currentMovement.m_direction ][ 0 ];
                    auto noy = o.second.m_pos.m_posY
                               + dir[ o.second.m_currentMovement.m_direction ][ 1 ];

                    if( o.second.m_pos.m_posX % SIZE == ( o.second.m_event.m_posX + 1 ) % SIZE ) {

                        if( canMove( o.second.m_pos, LEFT, WALK ) && ( nxl != curx || nyl != cury )
                            && ( nxl != cx2 || nyl != cy2 ) ) {
                            o.second.m_currentMovement = { LEFT, 0 };
                        } else {
                            nomove = true;
                        }
                    } else if( ( o.second.m_pos.m_posX + 1 ) % SIZE
                               == o.second.m_event.m_posX % SIZE ) {
                        if( canMove( o.second.m_pos, RIGHT, WALK ) && ( nxr != curx || nyr != cury )
                            && ( nxr != cx2 || nyr != cy2 ) ) {
                            o.second.m_currentMovement = { RIGHT, 0 };
                        } else {
                            nomove = true;
                        }
                    } else {
                        if( o.second.m_currentMovement.m_direction != LEFT
                            && o.second.m_currentMovement.m_direction != RIGHT ) {
                            if( canMove( o.second.m_pos, RIGHT, WALK )
                                && ( nxr != curx || nyr != cury )
                                && ( nxr != cx2 || nyr != cy2 ) ) {
                                o.second.m_currentMovement = { RIGHT, 0 };
                            } else {
                                nomove = true;
                            }
                        } else if( ( nox == curx && noy == cury )
                                   || ( nox == cx2 && noy == cy2 ) ) {
                            nomove = true;
                        }
                    }
                    if( !nomove ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    }
                }
            }
            if( o.second.m_movement == WALK_AROUND_UP_DOWN || o.second.m_movement == WALK_UP_DOWN
                || o.second.m_movement == WALK_CONT_UP_DOWN
                || ( o.second.m_movement == WALK_AROUND_SQUARE && !rndir ) ) {
                if( movemnt ) {
                    bool nomove = false;

                    auto nxu = o.second.m_pos.m_posX + dir[ UP ][ 0 ];
                    auto nyu = o.second.m_pos.m_posY + dir[ UP ][ 1 ];
                    auto nxd = o.second.m_pos.m_posX + dir[ DOWN ][ 0 ];
                    auto nyd = o.second.m_pos.m_posY + dir[ DOWN ][ 1 ];
                    auto nox = o.second.m_pos.m_posX
                               + dir[ o.second.m_currentMovement.m_direction ][ 0 ];
                    auto noy = o.second.m_pos.m_posY
                               + dir[ o.second.m_currentMovement.m_direction ][ 1 ];

                    if( o.second.m_pos.m_posY % SIZE == ( o.second.m_event.m_posY + 1 ) % SIZE ) {
                        if( canMove( o.second.m_pos, UP, WALK ) && ( nxu != curx || nyu != cury )
                            && ( nxu != cx2 || nyu != cy2 ) ) {
                            o.second.m_currentMovement = { UP, 0 };
                        } else {
                            nomove = true;
                        }
                    } else if( ( o.second.m_pos.m_posY + 1 ) % SIZE
                               == o.second.m_event.m_posY % SIZE ) {
                        if( canMove( o.second.m_pos, DOWN, WALK ) && ( nxd != curx || nyd != cury )
                            && ( nxd != cx2 || nyd != cy2 ) ) {
                            o.second.m_currentMovement = { DOWN, 0 };
                        } else {
                            nomove = true;
                        }
                    } else {
                        if( o.second.m_currentMovement.m_direction != DOWN
                            && o.second.m_currentMovement.m_direction != UP ) {
                            if( canMove( o.second.m_pos, DOWN, WALK )
                                && ( nxd != curx || nyd != cury )
                                && ( nxd != cx2 || nyd != cy2 ) ) {
                                o.second.m_currentMovement = { DOWN, 0 };
                            } else {
                                nomove = true;
                            }
                        } else if( ( nox == curx && noy == cury )
                                   || ( nox == cx2 && noy == cy2 ) ) {
                            nomove = true;
                        }
                    }
                    if( !nomove ) {
                        moveMapObject( i, o.second.m_currentMovement );
                        o.second.m_currentMovement.m_frame++;
                    }
                }
            }
        }

        if( change ) { _mapSprites.update( ); }

        loadAnimatedTiles( p_frame );
    }

    void mapDrawer::loadAnimatedTiles( u8 p_frame ) {
        u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
        for( u8 i = 0; i < TILE_ANIMATION_COUNT; ++i ) {
            if( !TILE_ANIMATIONS[ i ].m_size ) { break; }
            if( CUR_SLICE.m_tIdx1 == TILE_ANIMATIONS[ i ].m_tileSetIdx ) {
                auto& a = TILE_ANIMATIONS[ i ];
                if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                    a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                    dmaCopy( a.m_tileData + 32 * a.m_size * a.m_acFrame,
                             tileMemory + a.m_tileIdx * 32, sizeof( tile ) * a.m_size );
                }
            }
            if( CUR_SLICE.m_tIdx2 == TILE_ANIMATIONS[ i ].m_tileSetIdx ) {
                auto& a = TILE_ANIMATIONS[ i ];
                if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                    a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                    dmaCopy( a.m_tileData + 32 * a.m_size * a.m_acFrame,
                             tileMemory + ( a.m_tileIdx + MAX_TILES_PER_TILE_SET ) * 32,
                             sizeof( tile ) * a.m_size );
                }
            }
        }
    }

    mapDrawer::mapDrawer( ) : _curX( 0 ), _curY( 0 ), _playerIsFast( false ) {
        _mapSprites.init( );
    }

    // Movement stuff
    bool mapDrawer::canMove( position p_start, direction p_direction, moveMode p_moveMode,
                             bool p_events ) {
        u16 nx = p_start.m_posX + dir[ p_direction ][ 0 ];
        u16 ny = p_start.m_posY + dir[ p_direction ][ 1 ];

#ifdef DESQUID
        // Walk through walls for desquid purposes.
        if( keysHeld( ) & KEY_R ) { return true; }
#endif

        if( p_events ) {
            // Check if any event is occupying the target block
            for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
                auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
                if( o.second.m_pos.m_posX == nx && o.second.m_pos.m_posY == ny ) {
                    switch( o.second.m_event.m_type ) {
                    case EVENT_HMOBJECT:
                        if( o.second.m_event.m_data.m_hmObject.m_hmType
                            == mapSpriteManager::SPR_STRENGTH ) {
                            // Check if the boulder could be moved by using strength
                            if( p_moveMode == STRENGTH
                                || !canMove( { nx, ny, p_start.m_posZ }, p_direction, STRENGTH ) ) {
                                return false;
                            }
                            // Check if the player has actually used strength
                            if( !_strengthUsed ) { return false; }
                            break;
                        }
                        if( o.second.m_event.m_data.m_hmObject.m_hmType ) { return false; }
                        break;
                    case EVENT_ITEM:
                        if( o.second.m_event.m_data.m_item.m_itemType ) {
                            return false;
                        } // item is not hidden
                        break;
                    case EVENT_NPC:
                    case EVENT_NPC_MESSAGE:
                    case EVENT_TRAINER:
                    case EVENT_OW_PKMN:
                    case EVENT_BERRYTREE: return false;
                    case EVENT_GENERIC:
                        if( o.second.m_event.m_trigger & TRIGGER_INTERACT ) { return false; }
                    default: break;
                    }
                }
            }
        }

        // Gather data about the source block
        u8 lstMoveData, lstBehave;
        lstMoveData = atom( p_start.m_posX, p_start.m_posY ).m_movedata;

        auto lstblock = at( p_start.m_posX, p_start.m_posY );
        lstBehave     = lstblock.m_bottombehave;

        // Gather data about the destination block
        u8 curMoveData, curBehave;
        curMoveData = atom( nx, ny ).m_movedata;

        auto curblock = at( nx, ny );
        curBehave     = curblock.m_bottombehave;

        // Check for special block attributes
        switch( lstBehave ) {
        case 0x30:
            if( p_direction == RIGHT ) return false;
            break;
        case 0x31:
            if( p_direction == LEFT ) return false;
            break;
        case 0x32:
            if( p_direction == UP ) return false;
            break;
        case 0x33:
            if( p_direction == DOWN ) return false;
            break;
        case 0x36:
            if( p_direction == DOWN || p_direction == LEFT ) return false;
            break;
        case 0x37:
            if( p_direction == DOWN || p_direction == RIGHT ) return false;
            break;
        case 0xa0:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case 0xc0:
            if( ( p_direction & 1 ) == 0 ) return false;
            break;
        case 0xc1:
            if( p_direction & 1 ) return false;
            break;
        case 0x62:
            if( p_direction == RIGHT ) return true;
            break;
        case 0x63:
            if( p_direction == LEFT ) return true;
            break;
        case 0x64:
            if( p_direction == UP ) return true;
            break;
        case 0x65:
        case 0x6d:
            if( p_direction == DOWN ) return true;
            break;

        case 0xd3: // Bike stuff
        case 0xd5:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == LEFT || p_direction == RIGHT ) {
                if( ( curBehave != 0xd3 && curBehave != 0xd5 ) || p_moveMode != ACRO_BIKE
                    || !( held & KEY_B ) ) {
                    return false;
                }
            }
            break;
        case 0xd6:
        case 0xd4:
            if( !( p_moveMode & BIKE ) ) { return false; }
            if( p_direction == DOWN || p_direction == UP ) {
                if( ( curBehave != 0xd4 && curBehave != 0xd6 ) || p_moveMode != ACRO_BIKE
                    || !( held & KEY_B ) ) {
                    return false;
                }
            }
            break;

        default: break;
        }

        switch( curBehave ) {
        // Jumpy stuff
        case 0x38:
        case 0x35: return p_direction == RIGHT;
        case 0x39:
        case 0x34: return p_direction == LEFT;
        case 0x3a: return p_direction == UP;
        case 0x3b: return p_direction == DOWN;

        case 0xa0:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case 0xc0:
            if( ( p_direction & 1 ) == 0 ) return false;
            break;
        case 0xc1:
            if( p_direction & 1 ) return false;
            break;
        case 0x13: return false;
        case 0xd7: return false;

        // door-y stuff
        case 0x69: // Player can move to a door if there is a corresponding warp at the target
                   // position
            if( currentData( nx, ny ).hasEvent( EVENT_WARP, nx % SIZE, ny % SIZE,
                                                p_start.m_posZ ) ) {
                return true;
            } else {
                return false;
            }

        case 0xd3: // Bike stuff
        case 0xd5:
            if( !( p_moveMode & BIKE ) ) { return false; }
            break;
        case 0xd6:
        case 0xd4:
            if( !( p_moveMode & BIKE ) ) { return false; }
            break;

        default: break;
        }

        if( ( p_moveMode & BIKE ) && !canBike( { nx, ny, p_start.m_posZ } ) ) { return false; }

        // Check for movedata stuff
        if( curMoveData % 4 == 1 ) { return false; }
        if( lstMoveData == 0x0a ) { // Stand up (only possible for the player)
            return p_direction == SAVE::SAV.getActiveFile( ).m_player.m_direction;
        }
        if( curMoveData == 0x0a ) { // Sit down
            return ( p_moveMode == WALK );
        }
        if( curMoveData == 4 && !( p_moveMode & SURF ) ) {
            return false;
        } else if( curMoveData == 4 ) {
            return true;
        }

        if( curMoveData == 0x0c && lstMoveData == 4 ) { return true; }
        if( !curMoveData || !lstMoveData ) { return true; }
        if( curMoveData == 0x3c ) { return true; }

        return curMoveData % 4 == 0 && curMoveData / 4 == p_start.m_posZ;
    }
    void mapDrawer::movePlayer( direction p_direction, bool p_fast ) {

        u16 curx        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u8  curz        = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        u16 nx          = curx + dir[ p_direction ][ 0 ];
        u16 ny          = cury + dir[ p_direction ][ 1 ];
        u8  newMoveData = atom( nx, ny ).m_movedata;
        u8  lstMoveData = atom( curx, cury ).m_movedata;
        u8  newBehave   = at( nx, ny ).m_bottombehave;
        u8  lstBehave   = at( curx, cury ).m_bottombehave;

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement != moveMode::WALK ) {
            p_fast = false; // Running is only possible when the player is actually walking
        }

        // Check if any event is occupying the target block and push it if necessary
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.second.m_pos.m_posX == nx && o.second.m_pos.m_posY == ny ) {
                switch( o.second.m_event.m_type ) {
                case EVENT_HMOBJECT:
                    if( o.second.m_event.m_data.m_hmObject.m_hmType
                        == mapSpriteManager::SPR_STRENGTH ) {
                        // Check if the boulder could be moved by using strength
                        if( !canMove( { nx, ny, curz }, p_direction, STRENGTH ) ) { continue; }
                        // Check if the player has actually used strength
                        if( !_strengthUsed ) { continue; }

                        // push the boulder one block in the current direction
                        SOUND::playSoundEffect( SFX_HM_STRENGTH );
                        for( u8 f = 0; f < 16; ++f ) {
                            _mapSprites.moveSprite( o.first, p_direction, 1 );
                            swiWaitForVBlank( );
                        }
                        o.second.m_pos.m_posX += dir[ p_direction ][ 0 ];
                        o.second.m_pos.m_posY += dir[ p_direction ][ 1 ];
                    }
                    break;
                default: break;
                }
            }
        }

        bool reinit = false, moving = true, hadjump = false;
        while( moving ) {
            if( newMoveData == MAP_BORDER ) {
                fastBike = false;
                stopPlayer( direction( ( u8( p_direction ) + 2 ) % 4 ) );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                stopPlayer( );
                NAV::printMessage( GET_STRING( 7 ), MSG_INFO );
                _playerIsFast = false;
                return;
            }
            // Check for end of surf, stand up and sit down
            if( lstMoveData == 0x0a
                && newMoveData != 0x0a ) { // Stand up (only possible for the player)
                if( p_direction != SAVE::SAV.getActiveFile( ).m_player.m_direction ) return;

                standUpPlayer( p_direction );
                stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
                return;
            } else if( lstMoveData == 0x0a ) {
                fastBike = false;
                return;
            }
            if( newMoveData == 0x0a ) { // Sit down
                if( SAVE::SAV.getActiveFile( ).m_player.m_movement != WALK ) return;
                SAVE::SAV.getActiveFile( ).m_player.m_direction
                    = direction( ( u8( p_direction ) + 2 ) % 4 );
                _mapSprites.setFrameD( _playerSprite,
                                       SAVE::SAV.getActiveFile( ).m_player.m_direction );
                sitDownPlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction, SIT );
                fastBike = false;
                return;
            }
            if( newMoveData == 0x0c && lstMoveData == 4 ) { // End of surf
                standUpPlayer( p_direction );
                stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
                fastBike = false;
                return;
            }

            if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, p_direction,
                          SAVE::SAV.getActiveFile( ).m_player.m_movement ) ) {
                fastBike = false;
                moving   = false;
                stopPlayer( p_direction );
                return;
            }

            // Check for jumps/slides/...
            switch( newBehave ) {
            // First check for jumps
            case 0x38:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( RIGHT );
                p_direction = RIGHT;
                break;
            case 0x39:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( LEFT );
                p_direction = LEFT;
                break;
            case 0x3a:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( UP );
                p_direction = UP;
                break;
            case 0x3b:
                if( hadjump ) goto NO_BREAK;
                hadjump = true;
                jumpPlayer( DOWN );
                p_direction = DOWN;
                break;

            case 0xd3: // Bike stuff
            case 0xd5:
                if( p_direction == LEFT || p_direction == RIGHT ) {
                    bikeJumpPlayer( p_direction );
                    stopPlayer( p_direction );
                    return;
                }
                goto NO_BREAK;
            case 0xd6:
            case 0xd4:
                if( p_direction == UP || p_direction == DOWN ) {
                    bikeJumpPlayer( p_direction );
                    stopPlayer( p_direction );
                    return;
                }
                goto NO_BREAK;

            case 0xd2: {
                // breakable floor
                if( fastBike > 9 ) goto NEXT_PASS;
                walkPlayer( p_direction, p_fast );
                stopPlayer( p_direction );
                return;
            }

            case 0xd0:
                if( p_direction == DOWN ) {
                    removeFollowPkmn( );
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    reinit      = true;
                    break;
                } else {
                    if( fastBike > 9 ) goto NEXT_PASS;
                    walkPlayer( p_direction, p_fast );
                    removeFollowPkmn( );
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    reinit      = true;
                    break;
                }

            // Warpy stuff
            case 0x62:
                if( p_direction == RIGHT ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case 0x63:
                if( p_direction == LEFT ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case 0x64:
                if( p_direction == UP ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            case 0x65:
            case 0x6d:
                if( p_direction == DOWN ) {
                    walkPlayer( p_direction, p_fast );
                    handleWarp( NO_SPECIAL );
                    hadjump = false;
                    break;
                }
                goto NO_BREAK;
            NO_BREAK:
            default:
                // If no jump has to be done, check for other stuff
                hadjump = false;
                switch( lstBehave ) {
                case 0x20:
                case 0x48: slidePlayer( p_direction ); break;
                // These change the direction of movement
                case 0x40:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case 0x41:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case 0x42:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP );
                    p_direction = UP;
                    break;
                case 0x43:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0x44:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case 0x45:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case 0x46:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( UP );
                    p_direction = UP;
                    break;
                case 0x47:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0x50:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT, true );
                    p_direction = RIGHT;
                    break;
                case 0x51:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT, true );
                    p_direction = LEFT;
                    break;
                case 0x52:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP, true );
                    p_direction = UP;
                    break;
                case 0x53:
                    if( !canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV.getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN, true );
                    p_direction = DOWN;
                    break;
                case 0x62:
                    if( p_direction == RIGHT ) {
                        redirectPlayer( RIGHT, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case 0x63:
                    if( p_direction == LEFT ) {
                        redirectPlayer( LEFT, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case 0x64:
                    if( p_direction == UP ) {
                        redirectPlayer( UP, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    }
                    goto NEXT_PASS;
                case 0x65:
                case 0x6d:
                    if( p_direction == DOWN ) {
                        redirectPlayer( DOWN, p_fast );
                        handleWarp( NO_SPECIAL );
                        return;
                    }
                    goto NEXT_PASS;

                case 0xd0:
                    if( fastBike > 9 && p_direction != DOWN ) goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                NEXT_PASS:
                default:
                    if( reinit ) {
                        _mapSprites.setFrameD( _playerSprite, p_direction );
                        fastBike = false;
                        return;
                    }
                    switch( newBehave ) {
                    case 0x20:
                    case 0x48: walkPlayer( p_direction, p_fast ); break;

                    // Check for warpy stuff
                    case 0x60:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( CAVE_ENTRY );
                        return;
                    case 0x61:
                    case 0x68:
                    case 0x6e:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( NO_SPECIAL );
                        return;
                    case 0x66:
                        walkPlayer( p_direction, p_fast );
                        fastBike = false;
                        stopPlayer( p_direction );
                        return;
                    case 0x69:
                        redirectPlayer( p_direction, p_fast );
                        openDoor( nx, ny );
                        walkPlayer( p_direction, p_fast );
                        handleWarp( DOOR );
                        break;
                    case 0x6C:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( EMERGE_WATER );
                        break;
                    case 0x29:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( TELEPORT );
                        break;

                    // These change the direction of movement
                    case 0x40:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case 0x41:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case 0x42:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case 0x43:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;

                    case 0x44:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case 0x45:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case 0x46:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case 0x47:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;

                    case 0x50:
                        walkPlayer( p_direction, p_fast );
                        p_direction = RIGHT;
                        break;
                    case 0x51:
                        walkPlayer( p_direction, p_fast );
                        p_direction = LEFT;
                        break;
                    case 0x52:
                        walkPlayer( p_direction, p_fast );
                        p_direction = UP;
                        break;
                    case 0x53:
                        walkPlayer( p_direction, p_fast );
                        p_direction = DOWN;
                        break;
                    default: moving = false; continue;
                    }
                }
            }
            reinit = true;
            newMoveData
                = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_movedata;
            lstMoveData = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata;

            newBehave
                = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                      SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_bottombehave;
            lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                            .m_bottombehave;
        }
        walkPlayer( p_direction, p_fast );
        auto movedt = atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        if( movedt > 4 && movedt != 0x3c && movedt != 0xa ) {
            SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ = movedt / 4;
        }
    }

    void mapDrawer::fallthroughPlayer( ) {
        removeFollowPkmn( );

        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        _mapSprites.setVisibility( _playerSprite, true );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        IO::fadeScreen( IO::CLEAR_DARK );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        redirectPlayer( DOWN, false );
        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY += 32;
        draw( );
    }

    void mapDrawer::warpPlayer( warpType p_type, warpPos p_target ) {
        u8   oldMapType = u8( CUR_DATA.m_mapType );
        bool checkPos   = false;
        fastBike        = 0;
        _playerIsFast   = false;
        removeFollowPkmn( );

        if( p_target.first != SAVE::SAV.getActiveFile( ).m_currentMap ) {
            SAVE::SAV.getActiveFile( ).m_mapObjectCount = 0;
        }
        if( p_target.first != OW_MAP && SAVE::SAV.getActiveFile( ).m_currentMap == OW_MAP ) {
            SAVE::SAV.getActiveFile( ).m_lastOWPos = { SAVE::SAV.getActiveFile( ).m_currentMap,
                                                       SAVE::SAV.getActiveFile( ).m_player.m_pos };

            checkPos = true;
        }

        loadNewBank( p_target.first );

        mapData ndata;
        FS::readMapData( p_target.first, p_target.second.m_posX / SIZE,
                         p_target.second.m_posY / SIZE, ndata );

        u8 newMapType = u8( ndata.m_mapType );

        if( checkPos ) {
            auto curL   = ndata.m_baseLocationId;
            auto tmpPos = getOWPosForLocation( curL );
            if( tmpPos != DUMMY_POSITION ) {
                SAVE::SAV.getActiveFile( ).m_lastOWPos
                    = { SAVE::SAV.getActiveFile( ).m_currentMap, tmpPos };
            }
        }

        bool entryCave
            = ( !( oldMapType & CAVE ) && ( newMapType & CAVE ) && !( newMapType & INSIDE ) );
        if( entryCave ) {
            SAVE::SAV.getActiveFile( ).m_lastCaveEntry
                = { SAVE::SAV.getActiveFile( ).m_currentMap,
                    SAVE::SAV.getActiveFile( ).m_player.m_pos };
        }
        bool hidePlayer = true;
        bool exitCave
            = ( ( oldMapType & CAVE ) && !( oldMapType & INSIDE ) && !( newMapType & CAVE ) );
        if( exitCave ) { SAVE::SAV.getActiveFile( ).m_lastCaveEntry = { 255, { 0, 0, 0 } }; }

        if( !( oldMapType & INSIDE ) && ( newMapType & INSIDE ) ) { hidePlayer = false; }
        if( newMapType & CAVE ) { hidePlayer = false; }
        if( exitCave && p_type != DOOR && p_type != SLIDING_DOOR ) { hidePlayer = false; }

        switch( p_type ) {
        case TELEPORT:
            SOUND::playSoundEffect( SFX_WARP );
            for( u8 j = 0; j < 2; ++j ) {
                redirectPlayer( RIGHT, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( UP, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( LEFT, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
                redirectPlayer( DOWN, false );
                for( u8 i = 0; i < 2; ++i ) { swiWaitForVBlank( ); }
            }
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        case EMERGE_WATER: break;
        case CAVE_ENTRY:
            SOUND::playSoundEffect( SFX_CAVE_WARP );
            if( entryCave ) {
                IO::fadeScreen( IO::CAVE_ENTRY );
            } else if( exitCave ) {
                IO::fadeScreen( IO::CAVE_EXIT );
            } else {
                IO::fadeScreen( IO::CLEAR_DARK );
            }
            break;
        case LAST_VISITED:
        case DOOR:
        case SLIDING_DOOR:
        default:
            SOUND::playSoundEffect( SFX_CAVE_WARP );
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        case NO_SPECIAL: break;
        }
        swiWaitForVBlank( );
        swiWaitForVBlank( );

        if( ( ( oldMapType & INSIDE ) && ( newMapType & INSIDE ) && p_type == CAVE_ENTRY ) ) {
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            stopPlayer( DOWN );
            stopPlayer( );
        }

        SAVE::SAV.getActiveFile( ).m_player.m_pos = p_target.second;
        //        if( SAVE::SAV.getActiveFile( ).m_currentMap != p_target.first ) {
        SAVE::SAV.getActiveFile( ).m_currentMap = p_target.first;

        auto oldw = SAVE::SAV.getActiveFile( ).m_currentMapWeather;
        if( ndata.m_mapType & mapType::DARK ) {
            if( ndata.m_mapType & mapType::FLASHABLE ) {
                if( SAVE::SAV.getActiveFile( ).m_currentMapWeather != DARK_FLASH_USED ) {
                    SAVE::SAV.getActiveFile( ).m_currentMapWeather = DARK_FLASHABLE;
                }
            } else {
                SAVE::SAV.getActiveFile( ).m_currentMapWeather = DARK_PERMANENT;
            }
        } else {
            SAVE::SAV.getActiveFile( ).m_currentMapWeather = ndata.m_weather;
        }
        if( oldw != SAVE::SAV.getActiveFile( ).m_currentMapWeather ) { initWeather( ); }

        // hide player, may need to open a door first
        draw( OBJPRIORITY_2, hidePlayer );
        for( auto fn : _newBankCallbacks ) { fn( SAVE::SAV.getActiveFile( ).m_currentMap ); }
        auto curLocId = getCurrentLocationId( );

        if( curLocId == L_POKEMON_CENTER && oldMapType != newMapType && p_type == SLIDING_DOOR ) {
            // Register a new faint position (only if the PC was just entered)
            SAVE::SAV.getActiveFile( ).m_lastPokeCenter = p_target;
            SAVE::SAV.getActiveFile( ).m_lastPokeCenter.second.m_posY -= 4;
        }

        for( auto fn : _newLocationCallbacks ) { fn( curLocId ); }

        auto posx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        auto posy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u8 behave = at( posx, posy ).m_bottombehave;

        if( behave == 0x69 ) {
            // a door, open it
            openDoor( posx, posy );
        }
        if( hidePlayer ) { drawPlayer( OBJPRIORITY_2 ); }

        if( ( currentData( ).m_mapType & INSIDE )
            && ( SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::MACH_BIKE
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::ACRO_BIKE
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == MAP::BIKE ) ) {
            // Don't bike in buildings
            changeMoveMode( MAP::WALK );
        }

        if( ( ( oldMapType & INSIDE ) && ( newMapType & INSIDE ) && p_type == CAVE_ENTRY ) ) {
            stopPlayer( DOWN );
        }

        bool oldforce  = _forceNoFollow;
        _forceNoFollow = true;
        switch( behave ) {
        case 0x6e: walkPlayer( UP, false ); break;
        case 0x60: walkPlayer( DOWN, false ); break;
        case 0x69: {
            walkPlayer( DOWN, false );
            closeDoor( posx, posy );
            break;
        }

        default: break;
        }
        _forceNoFollow = oldforce;
    }

    void mapDrawer::redirectPlayer( direction p_direction, bool p_fast, bool p_force ) {
        // Check if redirecting is allowed
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;

        if( lstBehave >= 0xd3 && lstBehave <= 0xd6
            && p_direction % 2 != SAVE::SAV.getActiveFile( ).m_player.m_direction % 2 )
            return;

        // Check if the player's direction changed
        if( p_direction != SAVE::SAV.getActiveFile( ).m_player.m_direction || p_force ) {
            if( !_mapSprites.getVisibility( _playerPlatSprite ) ) {
                _mapSprites.setFrameD( _playerPlatSprite, p_direction, false );
            }
            _mapSprites.setFrame( _playerSprite, ( p_fast * 20 ) + getFrame( p_direction ) );
            SAVE::SAV.getActiveFile( ).m_player.m_direction = p_direction;
        }
    }

    void mapDrawer::standUpPlayer( direction p_direction ) {
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            clearFieldAnimation( gx, gy );
        }

        redirectPlayer( p_direction, false );
        bool remPlat = SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                       || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB;

        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        _mapSprites.moveSprite( _playerSprite, DOWN, 1 );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) / 3 + 12 );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        if( remPlat ) { _mapSprites.destroySprite( _playerPlatSprite ); }
        moveCamera( p_direction, true );
        _mapSprites.moveSprite( _playerSprite, DOWN, 2 );

        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        moveCamera( p_direction, true );

        _mapSprites.moveSprite( _playerSprite, DOWN, 1 );
        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }

        changeMoveMode( WALK );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }

        clearFieldAnimation( gx, gy );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::updatePlayer( ) {
        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX = _cx;
        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY = _cy;
    }

    void mapDrawer::sitDownPlayer( direction p_direction, moveMode p_newMoveMode ) {
        direction di = ( ( p_newMoveMode == SIT ) ? direction( ( u8( p_direction ) + 2 ) % 4 )
                                                  : p_direction );

        removeFollowPkmn( );
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;
        u16 gx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            clearFieldAnimation( gx, gy );
        }

        if( p_newMoveMode == SURF || p_newMoveMode == ROCK_CLIMB ) {
            // Load the Pkmn
            _playerPlatSprite
                = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPR_PLATFORM );
            _mapSprites.setFrame( _playerPlatSprite, getFrame( p_direction ) );
        }

        for( u8 i = 0; i < 7; ++i ) {
            if( i == 3 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            moveCamera( di, true );
            swiWaitForVBlank( );
        }
        changeMoveMode( p_newMoveMode );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) / 3 + 12 );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( di, true );
            if( i % 2 ) swiWaitForVBlank( );
        }
        _mapSprites.moveSprite( _playerSprite, UP, 2 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );
        moveCamera( di, true );
        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) );
        _mapSprites.moveSprite( _playerSprite, UP, 1 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );

        clearFieldAnimation( gx, gy );
    }

    void mapDrawer::slidePlayer( direction p_direction ) {
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            clearFieldAnimation( gx, gy );
        }

        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
            _mapSprites.nextFrame( _playerSprite );
        }
        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 ) { _mapSprites.currentFrame( _playerSprite ); }
            swiWaitForVBlank( );
        }
        clearFieldAnimation( gx, gy );
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    /*
     * @brief: Returns which pkmn should be allowed to follow the player.
     * Currently, a pkmn needs to have a 16x32, 16x16 or 32x32 ow sprite.
     */
    bool canFollowPlayer( u16 p_pkmnId, bool p_shiny, u8 p_forme ) {
        static u16  lstid  = 0;
        static bool lstsh  = 0;
        static u8   fm     = 0;
        static bool lstres = false;

        if( lstid == p_pkmnId && lstsh == p_shiny && fm == p_forme ) { return lstres; }
        lstid = p_pkmnId;
        lstsh = p_shiny;
        fm    = p_forme;

        char buf[ 100 ];
        if( !p_forme ) {
            snprintf( buf, 99, "%02d/%hu%s", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId,
                      p_shiny ? "s" : "" );
        } else {
            snprintf( buf, 99, "%02d/%hu%s_%hhu", p_pkmnId / FS::ITEMS_PER_DIR, p_pkmnId,
                      p_shiny ? "s" : "", p_forme );
        }
        FILE* f = FS::open( IO::OWP_PATH, buf, ".rsd" );
        if( !f ) { return lstres = false; }

        FS::readNop( f, 16 * sizeof( u16 ) );
        FS::readNop( f, sizeof( u8 ) );

        u8 sx = 0, sy = 0;
        FS::read( f, &sx, sizeof( u8 ), 1 );
        FS::read( f, &sy, sizeof( u8 ), 1 );
        FS::close( f );
        if( sx > 32 || sy > 32 ) { return lstres = false; }
        return lstres = true;
    }

    bool mapDrawer::updateFollowPkmn( ) {
        _followPkmnData        = nullptr;
        _followPkmnSpeciesData = nullptr;
        if( !SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) ) { return false; }
        if( _forceNoFollow ) { return false; }

        // only if first pkmn is not ko, it will follow the player.
        if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].canBattle( ) ) { return false; }

        _followPkmnData = &SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ];

        if( _followPkmnData == nullptr ) { return false; }

        u16  species = _followPkmnData->getSpecies( );
        bool shiny   = _followPkmnData->isShiny( );
        //        bool female  = _followPkmnData->isFemale( );
        u8 forme = _followPkmnData->getForme( );
        getPkmnData( species, forme, _followPkmnSpeciesData );

        if( species > MAX_PKMN ) { return false; }
        if( !canFollowPlayer( species, shiny, forme ) ) { return false; }

        _followPkmn.m_picNum = species + PKMN_SPRITE;
        _followPkmn.m_range  = ( forme << 1 ) | shiny;
        return true;
    }

    void mapDrawer::spawnFollowPkmn( u16 p_globX, u16 p_globY, u8 p_z, direction p_direction ) {
        _followPkmn = mapObject( );
        if( SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ) && updateFollowPkmn( ) ) {
            _followPkmn.m_pos          = { p_globX, p_globY, p_z };
            _followPkmn.m_movement     = NO_MOVEMENT;
            _followPkmn.m_direction    = p_direction;
            _followPkmn.m_event.m_type = EVENT_OW_PKMN;

            std::pair<u8, mapObject> cur = { 0, _followPkmn };
            loadMapObject( cur );
            _playerFollowPkmnSprite = cur.first;
            _mapSprites.setFrameD( _playerFollowPkmnSprite, p_direction );
            _pkmnFollowsPlayer = true;
        } else {
            _pkmnFollowsPlayer = false;
            removeFollowPkmn( );
        }
    }

    void mapDrawer::removeFollowPkmn( ) {
        if( _pkmnFollowsPlayer ) {
            _mapSprites.destroySprite( _playerFollowPkmnSprite );
            _pkmnFollowsPlayer     = false;
            _followPkmnData        = nullptr;
            _followPkmnSpeciesData = nullptr;
        }
    }

    void mapDrawer::walkPlayer( direction p_direction, bool p_fast ) {
        u16 gx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16 gz   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        u16 nx   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ];
        u16 ny   = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ];
        u8  anim = getTileAnimation( nx, ny );
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            if( !_pkmnFollowsPlayer && !SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                clearFieldAnimation( gx, gy );
            }
        }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement != WALK ) p_fast = false;

        // movement for attached objects
        auto olddir         = _lastPlayerMove;
        auto oldprio        = _lastPlayerPriority;
        _lastPlayerMove     = p_direction;
        _lastPlayerPriority = _mapSprites.getPriority( _playerSprite );
        _mapSprites.setPriority( _playerFollowPkmnSprite, oldprio );

        redirectPlayer( p_direction, p_fast );

        if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ > 3
            && SAVE::SAV.getActiveFile( ).m_player.m_movement != SURF ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_1 );
        }

        if( p_fast != _playerIsFast ) {
            _playerIsFast = p_fast;
            _mapSprites.setFrame( _playerSprite,
                                  ( p_fast * PLAYER_FAST ) + getFrame( p_direction ) );
        }
        u8 sid = 255;
        for( u8 i = 0; i < 16; ++i ) {
            if( anim && i == 0 ) { sid = animateField( nx, ny, anim, 0 ); }
            if( anim && sid < 255 && i == 5 ) { animateField( nx, ny, sid, 2 ); }
            if( anim && sid < 255 && i == 12 ) { animateField( nx, ny, sid, 3 ); }

            moveCamera( p_direction, true );
            if( i == 8 ) { _mapSprites.nextFrame( _playerSprite ); }
            if( ( !p_fast || i % 3 ) && !fastBike ) swiWaitForVBlank( );
            if( i % ( fastBike / 3 + 2 ) == 0 && fastBike ) swiWaitForVBlank( );

            if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx, { olddir, i }, false,
                               DOWN, false );
            } else if( _pkmnFollowsPlayer ) {
                moveMapObject( _followPkmn, _playerFollowPkmnSprite, { olddir, i }, false, DOWN,
                               false );
            }
        }

        // check if the object following the player got somehow detached (due to a jump,  etc)
        if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            auto pos = SAVE::SAV.getActiveFile( )
                           .m_mapObjects[ SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx ]
                           .second.m_pos;
            if( std::abs( pos.m_posX - nx ) + std::abs( pos.m_posY - ny ) > 1 ) {
                for( u8 i = 0; i < 16; ++i ) {
                    moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx, { olddir, i },
                                   false, DOWN, false );
                    if( i % 3 ) swiWaitForVBlank( );
                }
            }
        } else if( _pkmnFollowsPlayer ) {
            if( std::abs( _followPkmn.m_pos.m_posX - nx )
                    + std::abs( _followPkmn.m_pos.m_posY - ny )
                > 1 ) {
                for( u8 i = 0; i < 16; ++i ) {
                    moveMapObject( _followPkmn, _playerFollowPkmnSprite, { olddir, i }, false, DOWN,
                                   false );
                    if( i % 3 ) swiWaitForVBlank( );
                }
            }
        }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement == WALK && !_pkmnFollowsPlayer ) {
            spawnFollowPkmn( gx, gy, gz, olddir );
        }

        if( sid < 255 ) { _mapSprites.destroySprite( sid, false ); }
        _mapSprites.drawFrame( _playerSprite, ( p_fast * PLAYER_FAST ) + getFrame( p_direction ) );
        if( ( SAVE::SAV.getActiveFile( ).m_player.m_movement & BIKE )
            || SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF ) {
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == ACRO_BIKE ) {
                fastBike = std::min( fastBike + 1, 4 );
            } else if( SAVE::SAV.getActiveFile( ).m_player.m_movement == MACH_BIKE ) {
                fastBike = std::min( fastBike + 1, 12 );
            } else if( SAVE::SAV.getActiveFile( ).m_player.m_movement == BIKE ) {
                fastBike = std::min( fastBike + 1, 8 );
            } else {
                fastBike = std::min( fastBike + 1, 6 );
            }
        } else
            fastBike = false;

        if( atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == 0x3c
            && ( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ <= 3
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                 || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_3 );

            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) {
                _mapSprites.setPriority( _playerPlatSprite, OBJPRIORITY_3 );
            }
        } else if( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ <= 3 ) {
            _mapSprites.setPriority( _playerSprite,
                                     SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_2 );
            if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF
                || SAVE::SAV.getActiveFile( ).m_player.m_movement == ROCK_CLIMB ) {
                _mapSprites.setPriority( _playerPlatSprite, OBJPRIORITY_2 );
            }
        }
        if( !_pkmnFollowsPlayer && !SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            clearFieldAnimation( gx, gy );
        }
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );

        SAVE::SAV.getActiveFile( ).stepIncrease( );
        _mapSprites.reorderSprites( true );
    }

    void mapDrawer::bikeJumpPlayer( direction p_direction ) {
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            clearFieldAnimation( gx, gy );
        }

        SOUND::playSoundEffect( SFX_JUMP );

        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i < 3 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            if( i > 13 ) { _mapSprites.moveSprite( _playerSprite, DOWN, 3 ); }
            if( i % 2 ) swiWaitForVBlank( );
        }
        clearFieldAnimation( gx, gy );
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::jumpPlayer( direction p_direction ) {
        SOUND::playSoundEffect( SFX_JUMP );
        u16 gx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 gy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16 nx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + 2 * dir[ p_direction ][ 0 ];
        u16 ny = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + 2 * dir[ p_direction ][ 1 ];
        if( p_direction == DOWN
            && getTileAnimation( gx, gy ) != mapSpriteManager::SPR_LONG_GRASS ) {
            clearFieldAnimation( gx, gy );
        }

        // movement for attached objects
        auto olddir         = _lastPlayerMove;
        auto oldprio        = _lastPlayerPriority;
        _lastPlayerMove     = p_direction;
        _lastPlayerPriority = _mapSprites.getPriority( _playerSprite );
        _mapSprites.setPriority( _playerFollowPkmnSprite, oldprio );

        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _mapSprites.setFrame( _playerSprite, getFrame( p_direction ) );
        }

        bool followlong = false;
        if( _pkmnFollowsPlayer
            && std::abs( _followPkmn.m_pos.m_posX - nx ) + std::abs( _followPkmn.m_pos.m_posY - ny )
                   > 3 ) {
            followlong = true;
        }

        for( u8 i = 0; i < 32; ++i ) {
            moveCamera( p_direction, true );
            if( i < 6 && i % 2 ) { _mapSprites.moveSprite( _playerSprite, UP, 2 ); }
            if( i % 8 == 0 ) { _mapSprites.nextFrame( _playerSprite ); }
            if( i > 28 && i % 2 ) { _mapSprites.moveSprite( _playerSprite, DOWN, 3 ); }
            if( i % 4 ) swiWaitForVBlank( );

            if( i < 16 || followlong ) {
                if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
                    moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx,
                                   { olddir, u8( i & 15 ) }, false, DOWN, false );
                } else if( _pkmnFollowsPlayer ) {
                    moveMapObject( _followPkmn, _playerFollowPkmnSprite, { olddir, u8( i & 15 ) },
                                   false, DOWN, false );
                }
            }
        }

        _mapSprites.drawFrame( _playerSprite, getFrame( p_direction ) );
        clearFieldAnimation( gx, gy );
        stepOn( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ );
        SAVE::SAV.getActiveFile( ).stepIncrease( );
    }

    void mapDrawer::stopPlayer( ) {
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;
        if( lstBehave == 0x66 ) {
            // fall through
            _playerIsFast = false;
            fastBike      = false;
            _mapSprites.setFrame( _playerSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            fallthroughPlayer( );
            return;
        }

        while( fastBike ) {
            fastBike = std::max( 0, (s8) fastBike - 3 );
            if( canMove( SAVE::SAV.getActiveFile( ).m_player.m_pos,
                         SAVE::SAV.getActiveFile( ).m_player.m_direction, BIKE ) )
                movePlayer( SAVE::SAV.getActiveFile( ).m_player.m_direction );
            fastBike = std::max( 0, (s8) fastBike - 1 );
        }
        _playerIsFast = false;
        fastBike      = false;
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
    }
    void mapDrawer::stopPlayer( direction p_direction ) {
        u8 lstBehave = at( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;
        if( lstBehave == 0x66 ) {
            // fall through
            _playerIsFast = false;
            fastBike      = false;
            _mapSprites.setFrame( _playerSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            fallthroughPlayer( );
            return;
        }

        if( SAVE::SAV.getActiveFile( ).m_player.m_movement == SIT
            && ( ( p_direction % 2 == SAVE::SAV.getActiveFile( ).m_player.m_direction % 2 )
                 || atom(
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                            .m_movedata
                        != atom( SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                                     + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 0 ],
                                 SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                                     + dir[ SAVE::SAV.getActiveFile( ).m_player.m_direction ][ 1 ] )
                               .m_movedata ) ) {
            return;
        }
        fastBike = false;
        redirectPlayer( p_direction, false );
        _playerIsFast = false;
        _mapSprites.nextFrame( _playerSprite );
    }

    void mapDrawer::changeMoveMode( moveMode p_newMode, bool p_hidden ) {
        bool change  = SAVE::SAV.getActiveFile( ).m_player.m_movement != p_newMode;
        u8   basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        fastBike     = false;
        bool surfing = false;
        u8   ydif    = 0;
        SAVE::SAV.getActiveFile( ).m_player.m_movement = p_newMode;
        switch( p_newMode ) {
        case WALK: SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic; break;
        case SURF:
        case ROCK_CLIMB:
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 3;
            surfing                                      = true;
            break;
        case BIKE:
        case MACH_BIKE:
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 1;
            break;
        case ACRO_BIKE:
            //    SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 2;
            //    TODO
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 1;
            break;
        case SIT:
            removeFollowPkmn( );
            SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 3;
            ydif                                         = 2;
            break;
        default: break;
        }

        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite
            = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                      SAVE::SAV.getActiveFile( ).m_player.sprite( ), p_hidden );
        if( ydif ) { _mapSprites.moveSprite( _playerSprite, UP, ydif, true ); }
        _mapSprites.setFrame( _playerSprite,
                              getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );

        if( surfing ) {
            _playerPlatSprite
                = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPR_PLATFORM, p_hidden );
            _mapSprites.setFrame( _playerPlatSprite,
                                  getFrame( SAVE::SAV.getActiveFile( ).m_player.m_direction ) );
            if( !change ) { _mapSprites.moveSprite( _playerSprite, UP, 3 ); }
        }

        for( auto fn : _newMoveModeCallbacks ) { fn( p_newMode ); }
    }

    bool mapDrawer::canFish( position p_start, direction p_direction ) {
        return atom( p_start.m_posX + dir[ p_direction ][ 0 ],
                     p_start.m_posY + dir[ p_direction ][ 1 ] )
                       .m_movedata
                   == 0x04
               && atom( p_start.m_posX, p_start.m_posY ).m_movedata != 0x3c;
    }
    void mapDrawer::fishPlayer( direction p_direction, u8 p_rodType ) {
        PLAYER_IS_FISHING = true;
        u16 curx          = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury          = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 6;
        bool surfing  = ( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF );
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        _mapSprites.moveSprite( _playerSprite, 8 * dir[ p_direction ][ 0 ],
                                8 * ( p_direction == DOWN ), true );

        u8 frame = 0;
        if( p_direction == UP ) frame = 4;
        if( p_direction == DOWN ) frame = 8;

        for( u8 i = 0; i < 4; ++i ) {
            _mapSprites.drawFrame( _playerSprite, frame + i, p_direction == RIGHT, true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        u8   rounds = rand( ) % 5;
        bool failed = false;
        NAV::printMessage( 0, MSG_NOCLOSE );
        for( u8 i = 0; i < rounds + 1; ++i ) {
            u8 cr = rand( ) % 7;
            NAV::printMessage( 0, MSG_NOCLOSE );
            for( u8 j = 0; j < cr + 5; ++j ) {
                NAV::printMessage( " .", MSG_NOCLOSE );
                for( u8 k = 0; k < 30; ++k ) {
                    scanKeys( );
                    swiWaitForVBlank( );
                    int pressed = keysDown( );
                    if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT( KEY_B ) ) {
                        failed = true;
                        goto OUT;
                    }
                }
            }
            FRAME_COUNT = 0;
            NAV::printMessage( "\n", MSG_NOCLOSE );
            NAV::printMessage( GET_STRING( 8 ), MSG_NOCLOSE );
            if( FRAME_COUNT > 60 ) {
                failed = true;
                break;
            }
        }

    OUT:
        NAV::printMessage( 0, MSG_NOCLOSE );
        if( failed ) {
            NAV::printMessage( GET_STRING( 9 ) );
        } else {
            NAV::printMessage( 0 );
        }
        for( s8 i = 2; i >= 0; --i ) {
            _mapSprites.drawFrame( _playerSprite, frame + i, p_direction == RIGHT, true );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        changeMoveMode( surfing ? SURF : WALK );
        if( !failed ) {
            // Check if the player's leading Pokémon has sucion cups or sticky hold
            bool forceEncounter
                = ( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].isEgg( )
                    && ( SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                             == A_SUCTION_CUPS
                         || SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                                == A_STICKY_HOLD ) );

            // Start wild PKMN battle here
            switch( p_rodType ) {
            default:
            case 0: handleWildPkmn( OLD_ROD, forceEncounter ); break;
            case 1: handleWildPkmn( GOOD_ROD, forceEncounter ); break;
            case 2: handleWildPkmn( SUPER_ROD, forceEncounter ); break;
            }
        }

        PLAYER_IS_FISHING = false;
    }

    void mapDrawer::usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny, u8 p_forme ) {
        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 5;
        bool surfing = ( SAVE::SAV.getActiveFile( ).m_player.m_movement == SURF );

        u16 curx      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        for( u8 i = 0; i < 5; ++i ) {
            _mapSprites.drawFrame( _playerSprite, i, false, true );
            for( u8 j = 0; j < 5; ++j ) swiWaitForVBlank( );
        }
        _mapSprites.setVisibility( _playerSprite, true, false );
        IO::loadSpriteB( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_GFX, 64, 32, 64, 64, false, false, false,
                         OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 1, SPR_CIRC_GFX, 128, 32, 64, 64, 0, 0, 0, false, true,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 2, SPR_CIRC_GFX, 64, 96, 64, 64, 0, 0, 0, true, false,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 3, SPR_CIRC_GFX, 128, 96, 64, 64, 0, 0, 0, true, true,
                         false, OBJPRIORITY_1, false );

        SOUND::playCry( p_pkmIdx, p_forme );
        IO::loadPKMNSpriteB( p_pkmIdx, 80, 48, SPR_PKMN_OAM, SPR_PKMN_GFX, false, p_shiny, p_female,
                             false, false, p_forme );
        IO::updateOAM( false );
        for( u8 i = 0; i < 75; ++i ) swiWaitForVBlank( );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_PKMN_OAM + i ].isHidden = true;
        }
        IO::updateOAM( false );
        changeMoveMode( surfing ? SURF : WALK );
        swiWaitForVBlank( );
    }

    bool mapDrawer::useFollowPkmn( ) {
        if( !_pkmnFollowsPlayer || _followPkmnData == nullptr ) { return false; }

        // store direction the player is currently facing
        direction olddir = SAVE::SAV.getActiveFile( ).m_player.m_direction;

        // make one step in the opposite direction of the last player move, this shoul
        // make the follow pkmn and the player switch places
        direction oldplaydir = _lastPlayerMove;
        walkPlayer( direction( ( _lastPlayerMove + 2 ) % 4 ), false );
        redirectPlayer( oldplaydir, false );

        // make the pkmn face the player's old direction
        _mapSprites.setFrameD( _playerFollowPkmnSprite, olddir, false );

        // play cry
        SOUND::playCry( _followPkmnData->getSpecies( ), _followPkmnData->getForme( ) );

        return true;
    }

    void mapDrawer::awardBadge( u8 p_type, u8 p_badge ) {

        if( p_type == 0
            && ( SAVE::SAV.getActiveFile( ).m_HOENN_Badges & ( 1 << ( p_badge - 1 ) ) ) ) {
            // player already has this badge/symbol.
            return;
        } else if( p_type == 1 ) {
            auto sym = ( p_badge / 10 ) - 1;
            auto tp  = ( p_badge % 10 ) - 1;

            if( SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges & ( 1 << ( 7 * tp + sym ) ) ) {
                return;
            }
        }

        if( p_type == 0 ) {
            SOUND::playBGMOneshot( MOD_OS_BADGE );
        } else if( p_type == 1 ) {
            SOUND::playBGMOneshot( MOD_OS_SYMBOL );
        }

        IO::loadSpriteB( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_GFX, 64, 32, 64, 64, false, false, false,
                         OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 1, SPR_CIRC_GFX, 128, 32, 64, 64, 0, 0, 0, false, true,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 2, SPR_CIRC_GFX, 64, 96, 64, 64, 0, 0, 0, true, false,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 3, SPR_CIRC_GFX, 128, 96, 64, 64, 0, 0, 0, true, true,
                         false, OBJPRIORITY_1, false );

        if( p_type == 0 ) { // Hoenn badge
            IO::loadSpriteB( ( "ba/b" + std::to_string( p_badge ) ).c_str( ), SPR_PKMN_OAM,
                             SPR_PKMN_GFX, 96, 64, 64, 64, false, false, false, OBJPRIORITY_0,
                             false );
            SAVE::SAV.getActiveFile( ).m_lastAchievementEvent = p_badge;
            SAVE::SAV.getActiveFile( ).m_HOENN_Badges |= ( 1 << ( p_badge - 1 ) );
        } else if( p_type == 1 ) { // Frontier symbol
            IO::loadSpriteB( ( "ba/s" + std::to_string( p_badge ) ).c_str( ), SPR_PKMN_OAM,
                             SPR_PKMN_GFX, 96, 64, 64, 64, false, false, false, OBJPRIORITY_0,
                             false );

            auto sym = ( p_badge / 10 ) - 1;
            auto tp  = ( p_badge % 10 ) - 1;

            SAVE::SAV.getActiveFile( ).m_lastAchievementEvent = 10 + 2 * sym + tp;
            SAVE::SAV.getActiveFile( ).m_FRONTIER_Badges |= ( 1 << ( 7 * tp + sym ) );
        }

        SAVE::SAV.getActiveFile( ).m_lastAchievementDate = SAVE::CURRENT_DATE;

        IO::updateOAM( false );
        for( u16 i = 0; i < 330; ++i ) swiWaitForVBlank( );

        for( u8 i = 0; i < 4; ++i ) { IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden = true; }
        IO::OamTop->oamBuffer[ SPR_PKMN_OAM ].isHidden = true;
        IO::updateOAM( false );

        char buffer[ 140 ];
        snprintf( buffer, 139, GET_STRING( 436 ), SAVE::SAV.getActiveFile( ).m_playername,
                  getBadgeName( p_type, p_badge ) );
        NAV::printMessage( buffer, MSG_INFO );
        SOUND::restartBGM( );
    }

    void mapDrawer::runPokeMart( const std::vector<std::pair<u16, u32>>& p_offeredItems,
                                 const char* p_message, bool p_allowItemSell, u8 p_paymentMethod ) {

        // Select mode (buy/sell/cancel)

        u8  curMode = 0;
        u16 buystr  = p_paymentMethod == 0 ? 468 : 673;

        loop( ) {

            if( p_allowItemSell ) {
                curMode = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT )
                              .getResult( p_message ? p_message : GET_STRING( 470 ), MSG_NOCLOSE,
                                          { buystr, 469, 387 } );
            } else {
                curMode = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT )
                              .getResult( p_message ? p_message : GET_STRING( 470 ), MSG_NOCLOSE,
                                          { buystr, 387 } );
            }

            if( curMode == 0 ) {
                NAV::buyItem( p_offeredItems, p_paymentMethod );
            } else if( p_allowItemSell && curMode == 1 ) {
                BAG::bagViewer bv = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam,
                                                    BAG::bagViewer::SELL_ITEM );
                ANIMATE_MAP       = false;
                SOUND::dimVolume( );

                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                bv.run( );

                FADE_TOP_DARK( );
                FADE_SUB_DARK( );
                IO::clearScreen( false );
                videoSetMode( MODE_5_2D );
                bgUpdate( );

                draw( );
                SOUND::restoreVolume( );
                NAV::init( );
                ANIMATE_MAP = true;
            } else if( ( !p_allowItemSell && curMode == 1 ) || curMode == 2 ) {
                break;
            }
        }

        NAV::init( );
    }

    u16 mapDrawer::getCurrentLocationId( ) const {
        if( SAVE::SAV.getActiveFile( ).m_currentMap == OW_MAP ) [[likely]] {
            return BANK_10_MAP_LOCATIONS[ SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY
                                          / MAP_LOCATION_RES ]
                                        [ SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX
                                          / MAP_LOCATION_RES ];
        }

        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX % SIZE;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY % SIZE;

        const mapData& mdata = currentData( );

        u16 res = mdata.m_baseLocationId;
        for( u8 i = 0; i < mdata.m_extraLocationCount; ++i ) {
            if( mdata.m_extraLocations[ i ].m_left <= curx
                && mdata.m_extraLocations[ i ].m_right >= curx
                && mdata.m_extraLocations[ i ].m_top <= cury
                && mdata.m_extraLocations[ i ].m_bottom >= cury ) {

                res = mdata.m_extraLocations[ i ].m_locationId;
                break;
            }
        }
        return res;
    }
    u16 mapDrawer::getCurrentLocationId( u8 p_file ) const {
        if( SAVE::SAV.m_saveFile[ p_file ].m_currentMap == OW_MAP ) [[likely]] {
            return BANK_10_MAP_LOCATIONS[ SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posY
                                          / MAP_LOCATION_RES ]
                                        [ SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posX
                                          / MAP_LOCATION_RES ];
        }

        u16 curx = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posX % SIZE;
        u16 cury = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posY % SIZE;
        u16 mapx = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posX / SIZE;
        u16 mapy = SAVE::SAV.m_saveFile[ p_file ].m_player.m_pos.m_posY / SIZE;

        mapData mdata;
        FS::readMapData( SAVE::SAV.m_saveFile[ p_file ].m_currentMap, mapx, mapy, mdata );

        u16 res = mdata.m_baseLocationId;
        for( u8 i = 0; i < mdata.m_extraLocationCount; ++i ) {
            if( mdata.m_extraLocations[ i ].m_left <= curx
                && mdata.m_extraLocations[ i ].m_right >= curx
                && mdata.m_extraLocations[ i ].m_top <= cury
                && mdata.m_extraLocations[ i ].m_bottom >= cury ) {

                res = mdata.m_extraLocations[ i ].m_locationId;
                break;
            }
        }
        return res;
    }

    void mapDrawer::constructAndAddNewMapObjects( MAP::mapData const& p_data, u8 p_mapX,
                                                  u8 p_mapY ) {
        std::vector<std::pair<u8, mapObject>> res;
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        std::set<position> eventPositions;

#ifdef DESQUID_MORE
        IO::fadeScreen( IO::UNFADE );
        NAV::printMessage( ( std::string( "constructAndAddNewMapObjects " )
                             + std::to_string( p_mapX ) + " " + std::to_string( p_mapY ) )
                               .c_str( ) );
#endif

        // check old objects and purge them if they are not visible anymore
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.first == UNUSED_MAPOBJECT ) { continue; }
            if( _fixedMapObjects.count( i ) ) {
#ifdef DESQUID_MORE
                NAV::printMessage(
                    ( std::string( "skip " ) + std::to_string( o.first ) ).c_str( ) );
#endif
                continue;
            }

            if( dist( o.second.m_pos.m_posX, o.second.m_pos.m_posY, curx, cury ) > 24 ) {
#ifdef DESQUID_MORE
                NAV::printMessage(
                    ( std::string( "Destroying " ) + std::to_string( i ) + " "
                      + std::to_string( o.first ) + " : " + std::to_string( o.second.m_pos.m_posX )
                      + " " + std::to_string( curx ) + " " + std::to_string( o.second.m_pos.m_posY )
                      + " " + std::to_string( cury ) )
                        .c_str( ) );
#endif
                _mapSprites.destroySprite( o.first, false );
            } else if( o.second.m_event.m_activateFlag
                       && !SAVE::SAV.getActiveFile( ).checkFlag(
                           o.second.m_event.m_activateFlag ) ) {
                _mapSprites.destroySprite( o.first, false );
            } else if( o.second.m_event.m_deactivateFlag
                       && SAVE::SAV.getActiveFile( ).checkFlag(
                           o.second.m_event.m_deactivateFlag ) ) {
                _mapSprites.destroySprite( o.first, false );
            } else {
                res.push_back( o );
                eventPositions.insert(
                    { o.second.m_event.m_posX, o.second.m_event.m_posY, o.second.m_event.m_posZ } );
            }
        }

        //        bool loadingNewObjectFailed = false;

        // add new objects
        for( u8 i = 0; i < p_data.m_eventCount; ++i ) {
            if( p_data.m_events[ i ].m_activateFlag
                && !SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_activateFlag ) ) {
                continue;
            }
            if( p_data.m_events[ i ].m_deactivateFlag
                && SAVE::SAV.getActiveFile( ).checkFlag( p_data.m_events[ i ].m_deactivateFlag ) ) {
                continue;
            }

            // check if there is an event that is already loaded and has the same base
            // coordinates as the current event
            if( eventPositions.count( { p_data.m_events[ i ].m_posX, p_data.m_events[ i ].m_posY,
                                        p_data.m_events[ i ].m_posZ } ) ) {
                continue;
            }

            if( p_data.m_events[ i ].m_type == EVENT_NPC_MESSAGE
                || p_data.m_events[ i ].m_type == EVENT_NPC ) {
                if( p_data.m_events[ i ].m_data.m_npc.m_scriptType == 11
                    && SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
                if( p_data.m_events[ i ].m_data.m_npc.m_scriptType == 10
                    && !SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                    continue;
                }
            }

            std::pair<u8, mapObject> cur;

            switch( p_data.m_events[ i ].m_type ) {
            case EVENT_HMOBJECT: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum    = (u16) -1;
                obj.m_movement  = NO_MOVEMENT;
                obj.m_range     = 0;
                obj.m_direction = UP;
                obj.m_event     = p_data.m_events[ i ];
#ifdef DESQUID_MORE
                NAV::printMessage( ( std::to_string( curx ) + "|" + std::to_string( cury ) + " : "
                                     + std::to_string( obj.m_pos.m_posX ) + " , "
                                     + std::to_string( obj.m_pos.m_posY ) )
                                       .c_str( ) );
#endif
                cur = { 0, obj };
                break;
            }
            case EVENT_BERRYTREE: {
                if( !SAVE::SAV.getActiveFile( ).berryIsAlive(
                        p_data.m_events[ i ].m_data.m_berryTree.m_treeIdx ) ) {
                    SAVE::SAV.getActiveFile( ).harvestBerry(
                        p_data.m_events[ i ].m_data.m_berryTree.m_treeIdx );
                    continue;
                } else {
                    // Check the growth of the specified berry tree
                    mapObject obj   = mapObject( );
                    obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                                  u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                                  p_data.m_events[ i ].m_posZ };
                    obj.m_picNum    = (u16) -1;
                    obj.m_movement  = NO_MOVEMENT;
                    obj.m_range     = 0;
                    obj.m_direction = UP;
                    obj.m_event     = p_data.m_events[ i ];

                    cur = { 0, obj };
                }
                break;
            }
            case EVENT_ITEM: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum    = (u16) -1;
                obj.m_movement  = NO_MOVEMENT;
                obj.m_range     = 0;
                obj.m_direction = UP;
                obj.m_event     = p_data.m_events[ i ];

                cur = { 0, obj };
                break;
            }
            case EVENT_TRAINER: {
                mapObject obj = mapObject( );
                obj.m_pos     = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum  = p_data.m_events[ i ].m_data.m_trainer.m_spriteId;
                obj.m_movement
                    = (MAP::moveMode) p_data.m_events[ i ].m_data.m_trainer.m_movementType;
                obj.m_range     = (MAP::moveMode) p_data.m_events[ i ].m_data.m_trainer.m_sight;
                obj.m_direction = getRandomLookDirection( obj.m_movement );
                obj.m_event     = p_data.m_events[ i ];
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                cur = { 0, obj };
                break;
            }

            case EVENT_OW_PKMN: {
                mapObject obj  = mapObject( );
                obj.m_pos      = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum   = p_data.m_events[ i ].m_data.m_owPkmn.m_speciesId + PKMN_SPRITE;
                obj.m_movement = NO_MOVEMENT;
                obj.m_range    = ( ( p_data.m_events[ i ].m_data.m_owPkmn.m_forme & 0x3f ) << 1 )
                              | !!( p_data.m_events[ i ].m_data.m_owPkmn.m_shiny & 0x3f );
                obj.m_direction = DOWN;
                obj.m_event     = p_data.m_events[ i ];

                cur = { 0, obj };
                break;
            }

            case EVENT_NPC:
            case EVENT_NPC_MESSAGE: {
                mapObject obj   = mapObject( );
                obj.m_pos       = { u16( p_mapX * SIZE + p_data.m_events[ i ].m_posX ),
                              u16( p_mapY * SIZE + p_data.m_events[ i ].m_posY ),
                              p_data.m_events[ i ].m_posZ };
                obj.m_picNum    = p_data.m_events[ i ].m_data.m_npc.m_spriteId;
                obj.m_movement  = (MAP::moveMode) p_data.m_events[ i ].m_data.m_npc.m_movementType;
                obj.m_range     = 0;
                obj.m_direction = getRandomLookDirection( obj.m_movement );
                obj.m_event     = p_data.m_events[ i ];
                obj.m_currentMovement = movement{ obj.m_direction, 0 };

                cur = { 0, obj };
                break;
            }
            default: continue;
            }

            loadMapObject( cur );
            res.push_back( cur );
        }

        SAVE::SAV.getActiveFile( ).m_mapObjectCount = res.size( ) + _fixedMapObjects.size( );
        for( u8 i = 0, shift = 0; i < res.size( ) + _fixedMapObjects.size( ); ++i ) {
            if( _fixedMapObjects.count( i ) ) {
                ++shift;
                continue;
            }
            if( size_t( i - shift ) < res.size( ) ) {
                SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] = res[ i - shift ];
            } else {
                SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] = { UNUSED_MAPOBJECT, mapObject( ) };
            }
        }

        // force an update
        _mapSprites.update( );
    }

    void mapDrawer::destroyHMObject( u16 p_globX, u16 p_globY ) {
        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto& o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];

            if( o.second.m_pos.m_posX != p_globX || o.second.m_pos.m_posY != p_globY ) { continue; }
            if( o.second.m_event.m_type == MAP::EVENT_HMOBJECT ) {
                if( o.second.m_event.m_data.m_hmObject.m_hmType
                    == mapSpriteManager::SPR_ROCKSMASH ) {
                    SOUND::playSoundEffect( SFX_HM_ROCKSMASH );
                    for( u8 g = 1; g <= 4; ++g ) {
                        for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                        _mapSprites.drawFrame( o.first, g, false, true );
                    }
                    for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                    _mapSprites.destroySprite( o.first );

                    o.first                                     = 255;
                    o.second.m_event.m_data.m_hmObject.m_hmType = 0;
                }
                if( o.second.m_event.m_data.m_hmObject.m_hmType == mapSpriteManager::SPR_CUT ) {
                    SOUND::playSoundEffect( SFX_HM_CUT );
                    for( u8 g = 1; g <= 4; ++g ) {
                        for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                        _mapSprites.drawFrame( o.first, g, false, true );
                    }
                    for( u8 f = 0; f < 4; ++f ) { swiWaitForVBlank( ); }
                    _mapSprites.destroySprite( o.first );

                    o.first                                     = 255;
                    o.second.m_event.m_data.m_hmObject.m_hmType = 0;
                }
            }
        }
    }

    void mapDrawer::faintPlayer( ) {
        removeFollowPkmn( );
        SAVE::SAV.getActiveFile( ).increaseVar( SAVE::V_NUM_FAINTED );
        IO::fadeScreen( IO::CLEAR_DARK_IMMEDIATE, true, true );
        ANIMATE_MAP = false;
        videoSetMode( MODE_5_2D );
        IO::initVideo( true );
        IO::clearScreen( true, true, true );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        SOUND::setVolume( 0 );
        bgUpdate( );

        auto tgpos = SAVE::SAV.getActiveFile( ).m_lastPokeCenter;
        if( !SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first
            || SAVE::SAV.getActiveFile( ).m_lastPokeCenter.first == 255 ) {
            SAVE::printTextAndWait( GET_STRING( 562 ) );
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                tgpos = { 21, { 0x2b, 0x29, 3 } };
            } else {
                tgpos = { 21, { 0x31, 0x49, 3 } };
            }
        } else {
            SAVE::printTextAndWait( GET_STRING( 561 ) );
        }
        _mapSprites.setPriority( _playerSprite,
                                 SAVE::SAV.getActiveFile( ).m_playerPriority = OBJPRIORITY_2 );

        FADE_TOP_DARK( );
        FADE_SUB_DARK( );
        IO::clearScreen( false );
        videoSetMode( MODE_5_2D );
        bgUpdate( );

        for( u8 i = 0; i < SAVE::SAV.getActiveFile( ).getTeamPkmnCount( ); ++i ) {
            SAVE::SAV.getActiveFile( ).getTeamPkmn( i )->heal( );
        }

        changeMoveMode( MAP::WALK );
        SOUND::setVolume( 0 );
        IO::initVideoSub( );
        IO::resetScale( true, false );
        NAV::init( );
        redirectPlayer( DOWN, false );
        warpPlayer( NO_SPECIAL, tgpos );
        ANIMATE_MAP = true;
    }
} // namespace MAP
