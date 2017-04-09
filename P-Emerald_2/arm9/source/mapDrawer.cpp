/*
Pokémon Emerald 2 Version
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine

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

#include <algorithm>

#include "battle.h"
#include "battleTrainer.h"
#include "battleWeather.h"
#include "defines.h"
#include "mapDrawer.h"
#include "messageBox.h"
#include "saveGame.h"
#include "screenFade.h"
#include "sprite.h"
#include "uio.h"

#include "BigCirc1.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace MAP {
#define NUM_ROWS 16
#define NUM_COLS 32
#define MAP_BORDER 0x3f
    mapDrawer* curMap = 0;
#define CUR_SLICE _slices[ _curX ][ _curY ]
    constexpr s8 currentHalf( u16 p_pos ) {
        return s8( ( p_pos % SIZE >= SIZE / 2 ) ? 1 : -1 );
    }

    mapBlockAtom& mapDrawer::atom( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE->m_x ), y = ( p_y / SIZE != CUR_SLICE->m_y );
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
            ->m_blocks[ p_y % SIZE ][ p_x % SIZE ];
    }
    block& mapDrawer::at( u16 p_x, u16 p_y ) const {
        bool x = ( p_x / SIZE != CUR_SLICE->m_x ), y = ( p_y / SIZE != CUR_SLICE->m_y );
        u16  blockidx = _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]
                           ->m_blocks[ p_y % SIZE ][ p_x % SIZE ]
                           .m_blockidx;
        return _slices[ ( _curX + x ) & 1 ][ ( _curY + y ) & 1 ]->m_blockSet.m_blocks[ blockidx ];
    }

    u16 lastrow,      // Row to be filled when extending the map to the top
        lastcol;      // Column to be filled when extending the map to the left
    u16       cx, cy; // Cameras's pos
    u16*      mapMemory[ 4 ];
    s8        fastBike     = false;
    mapObject surfPlatform = {MAP::mapObject::SURF_PLATFORM,
                              {0, 0, 0},
                              240,
                              MAP::moveMode::NO_MOVEMENT,
                              0,
                              1,
                              MAP::direction::UP};

    inline void loadBlock( block p_curblock, u32 p_memPos ) {
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
    inline void loadBlock( block p_curblock, u8 p_scrnX, u8 p_scrnY ) {
        u32 c = 64 * u32( p_scrnY ) + 2 * ( u32( p_scrnX ) % 16 );
        c += ( u32( p_scrnX ) / 16 ) * 1024;
        loadBlock( p_curblock, c );
    }

    // Drawing of Maps and stuff

    void mapDrawer::draw( u16 p_globX, u16 p_globY, bool p_init ) {
        if( p_init ) {
            videoSetMode( MODE_0_2D /* | DISPLAY_BG0_ACTIVE*/ | DISPLAY_BG1_ACTIVE
                          | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE
                          | DISPLAY_SPR_1D );
            vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
            FADE_TOP_DARK( );
            bgUpdate( );

            u16 mx = SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                my = SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY;
            constructSlice( SAVE::SAV->getActiveFile( ).m_currentMap, mx / SIZE, my / SIZE,
                            _slices[ _curX ][ _curY ], _slices );
            constructSlice( SAVE::SAV->getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                            my / SIZE, _slices[ _curX ^ 1 ][ _curY ], _slices );
            constructSlice( SAVE::SAV->getActiveFile( ).m_currentMap, mx / SIZE,
                            my / SIZE + currentHalf( my ), _slices[ _curX ][ _curY ^ 1 ], _slices );
            constructSlice( SAVE::SAV->getActiveFile( ).m_currentMap, mx / SIZE + currentHalf( mx ),
                            my / SIZE + currentHalf( my ), _slices[ _curX ^ 1 ][ _curY ^ 1 ],
                            _slices );

            for( u8 i = 1; i < 4; ++i ) {
                bgInit( i, BgType_Text4bpp, BgSize_T_512x256, 2 * i - 1, 1 );
                bgSetScroll( i, 120, 40 );
            }
            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );

            // for( u16 i = 0; i < MAX_TILES_PER_TILE_SET * 2; ++i )
            //    swiCopy( CUR_SLICE->m_tileSet.m_tiles[ i ].m_tile, tileMemory + i * 32, 16 );
            dmaCopy( CUR_SLICE->m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );
            dmaCopy( CUR_SLICE->m_pals, BG_PALETTE, 512 );
            for( u8 i = 1; i < 4; ++i ) {
                mapMemory[ i ] = (u16*) BG_MAP_RAM( 2 * i - 1 );
                bgSetPriority( i, i );
            }
        }

        lastrow = NUM_ROWS - 1;
        lastcol = NUM_COLS - 1;

        cx = p_globX;
        cy = p_globY;

        u16 mny = p_globY - 8;
        u16 mnx = p_globX - 15;

        for( u16 y = 0; y < NUM_ROWS; y++ )
            for( u16 x = 0; x < NUM_COLS; x++ ) {
#ifdef ___DEBUG
                if( ( mnx + x ) % SIZE == 0 || ( mny + y ) % SIZE == 0 )
                    loadBlock( CUR_SLICE->m_blockSet.m_blocks[ 0 ], x, y );
                else
#endif
                    loadBlock( at( mnx + x, mny + y ), x, y );
            }
        bgUpdate( );
    }

    void mapDrawer::draw( ObjPriority p_playerPrio ) {
        draw( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
              SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY, true ); // Draw the map

        IO::initOAMTable( false );
        drawPlayer( p_playerPrio ); // Draw the player
        drawObjects( );             // Draw NPCs / stuff

        IO::fadeScreen( IO::UNFADE );
    }

    void mapDrawer::drawPlayer( ObjPriority p_playerPrio ) {
        _sprites[ 0 ] = SAVE::SAV->getActiveFile( ).m_player.show( 128 - 8, 96 - 24, 0, 0, 0 );
        _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] = 0;
        _entriesUsed |= ( 1 << 0 );
        changeMoveMode( SAVE::SAV->getActiveFile( ).m_player.m_movement );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setPriority(
            p_playerPrio );
        if( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF ) {
            surfPlatform.m_id               = 1;
            _spritePos[ surfPlatform.m_id ] = 1;
            _sprites[ _spritePos[ surfPlatform.m_id ] ]
                = surfPlatform.show( 128 - 16, 96 - 20, 1, 1, 192 );
            _entriesUsed |= ( 1 << 1 );
            _sprites[ _spritePos[ surfPlatform.m_id ] ].setFrame(
                getFrame( SAVE::SAV->getActiveFile( ).m_player.m_direction ) );
        }
    }

    void mapDrawer::drawObjects( ) {
    }

    void mapDrawer::animateField( u16 p_globX, u16 p_globY ) {
        u8 behave = at( p_globX, p_globY ).m_bottombehave;

        switch( behave ) {
        default:
            break;
        }

        // TODO
    }

    void mapDrawer::stepOn( u16 p_globX, u16 p_globY, u8 p_z, bool p_allowWildPkmn ) {
        animateField( p_globX, p_globY );
        handleEvents( p_globX, p_globY, p_z );
        IO::NAV->updateMap( getCurrentLocationId( ) );
        if( p_allowWildPkmn ) handleWildPkmn( p_globX, p_globY );
    }

    void mapDrawer::loadNewRow( direction p_direction, bool p_updatePlayer ) {
        cx += dir[ p_direction ][ 0 ];
        cy += dir[ p_direction ][ 1 ];
        //#ifdef DEBUG
        //        assert( cx != SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX || cy !=
        //        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY );
        //#endif
        if( p_updatePlayer ) {
            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX = cx;
            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY = cy;

            SAVE::SAV->getActiveFile( ).stepIncrease( );
        }

        // Check if a new slice should be loaded
        if( ( dir[ p_direction ][ 0 ] == 1 && cx % 32 == 15 )
            || ( dir[ p_direction ][ 0 ] == -1 && cx % 32 == 14 )
            || ( dir[ p_direction ][ 1 ] == 1 && cy % 32 == 16 )
            || ( dir[ p_direction ][ 1 ] == -1 && cy % 32 == 15 ) ) {
            loadSlice( p_direction );
#ifdef __DEBUG
            IO::messageBox m( "Load Slice" );
            IO::NAV->draw( );
#endif
        }
        // Check if a new slice got stepped onto
        if( ( dir[ p_direction ][ 0 ] == 1 && cx % 32 == 0 )
            || ( dir[ p_direction ][ 0 ] == -1 && cx % 32 == 31 )
            || ( dir[ p_direction ][ 1 ] == 1 && cy % 32 == 0 )
            || ( dir[ p_direction ][ 1 ] == -1 && cy % 32 == 31 ) ) {
            _curX = ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1;
            _curY = ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1;
            // Update tileset, block and palette data
            u8* tileMemory = (u8*) BG_TILE_RAM( 1 );

            // for( u16 i = 0; i < MAX_TILES_PER_TILE_SET * 2; ++i )
            //    swiCopy( CUR_SLICE->m_tileSet.m_tiles[ i ].m_tile, tileMemory + i * 32, 16 );
            dmaCopy( CUR_SLICE->m_tileSet.m_tiles, tileMemory, MAX_TILES_PER_TILE_SET * 2 * 32 );
            dmaCopy( CUR_SLICE->m_pals, BG_PALETTE, 512 );

#ifdef __DEBUG
            sprintf( buffer, "Switch Slice to (%d, %d)", _curX, _curY );
            IO::messageBox m( buffer );
            IO::NAV->draw( );
#endif
        }

        switch( p_direction ) {
        case UP: {
            u16 ty  = cy - 8;
            u16 mnx = cx - 15;
            for( u16 x = ( lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, lastrow );
            lastrow = ( lastrow + NUM_ROWS - 1 ) % NUM_ROWS;
            break;
        }
        case LEFT: {
            u16 tx  = cx - 15;
            u16 mny = cy - 8;
            for( u16 y = ( lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), lastcol, y );
            lastcol = ( lastcol + NUM_COLS - 1 ) % NUM_COLS;
            break;
        }
        case DOWN: {
            lastrow = ( lastrow + 1 ) % NUM_ROWS;
            u16 ty  = cy + 7;
            u16 mnx = cx - 15;
            for( u16 x = ( lastcol + 1 ) % NUM_COLS, xp = mnx; xp < mnx + NUM_COLS;
                 x = ( x + 1 ) % NUM_COLS, ++xp )
                loadBlock( at( xp, ty ), x, lastrow );
            break;
        }
        case RIGHT: {
            lastcol = ( lastcol + 1 ) % NUM_COLS;
            u16 tx  = cx + 16;
            u16 mny = cy - 8;
            for( u16 y = ( lastrow + 1 ) % NUM_ROWS, yp = mny; yp < mny + NUM_ROWS;
                 y = ( y + 1 ) % NUM_ROWS, ++yp )
                loadBlock( at( tx, yp ), lastcol, y );
            break;
        }
        }
        bgUpdate( );
    }

    void mapDrawer::moveCamera( direction p_direction, bool p_updatePlayer, bool p_autoLoadRows ) {
        for( u8 i = 1; i < 4; ++i ) bgScroll( i, dir[ p_direction ][ 0 ], dir[ p_direction ][ 1 ] );
        bgUpdate( );

        if( p_autoLoadRows
            && ( ( dir[ p_direction ][ 0 ]
                   && ( ( bgState[ 1 ].scrollX >> 8 ) - dir[ p_direction ][ 0 ] + 16 ) % 16 == 0 )
                 || ( dir[ p_direction ][ 1 ]
                      && ( ( bgState[ 1 ].scrollY >> 8 ) - dir[ p_direction ][ 1 ] + 16 ) % 16
                             == 0 ) ) )
            loadNewRow( p_direction, p_updatePlayer );
    }

    void mapDrawer::loadSlice( direction p_direction ) {
        constructSlice( SAVE::SAV->getActiveFile( ).m_currentMap,
                        CUR_SLICE->m_x + dir[ p_direction ][ 0 ],
                        CUR_SLICE->m_y + dir[ p_direction ][ 1 ],
                        _slices[ ( 2 + _curX + dir[ p_direction ][ 0 ] ) & 1 ]
                               [ ( 2 + _curY + dir[ p_direction ][ 1 ] ) & 1 ],
                        _slices );
        auto& neigh = _slices[ ( _curX + !dir[ p_direction ][ 0 ] ) & 1 ]
                             [ ( _curY + !dir[ p_direction ][ 1 ] ) & 1 ];
        constructSlice( SAVE::SAV->getActiveFile( ).m_currentMap,
                        neigh->m_x + dir[ p_direction ][ 0 ], neigh->m_y + dir[ p_direction ][ 1 ],
                        _slices[ _curX ^ 1 ][ _curY ^ 1 ], _slices );
    }

    void mapDrawer::disablePkmn( s16 p_steps ) {
        SAVE::SAV->getActiveFile( ).m_repelSteps = p_steps;
    }
    void mapDrawer::enablePkmn( ) {
        SAVE::SAV->getActiveFile( ).m_repelSteps = 0;
    }

    void mapDrawer::handleWarp( warpType p_type ) {
        warpPos current = warpPos{SAVE::SAV->getActiveFile( ).m_currentMap,
                                  SAVE::SAV->getActiveFile( ).m_player.m_pos};
        if( p_type == LAST_VISITED ) {
            warpPos target = SAVE::SAV->getActiveFile( ).m_lastWarp;
            if( !target.first && !target.second.m_posX && !target.second.m_posY
                && !target.second.m_posZ )
                return;
            SAVE::SAV->getActiveFile( ).m_lastWarp = current;

            warpPlayer( p_type, target );
        } else {
            handleWarp( p_type, current );
        }
    }
    void mapDrawer::handleWildPkmn( u16 p_globX, u16 p_globY ) {
        u8 moveData = atom( p_globX, p_globY ).m_movedata;
        u8 behave   = at( p_globX, p_globY ).m_bottombehave;

        if( SAVE::SAV->getActiveFile( ).m_repelSteps ) return;
        // handle Pkmn stuff
        if( moveData == 0x04 && behave != 0x13 )
            handleWildPkmn( WATER );
        else if( behave == 0x02 )
            handleWildPkmn( GRASS );
        else if( behave == 0x03 )
            handleWildPkmn( HIGH_GRASS );
        else if( ( CURRENT_BANK.m_mapType >> 4 ) & CAVE )
            handleWildPkmn( GRASS );
    }
    pokemon wildPkmn;
    bool mapDrawer::handleWildPkmn( wildPkmnType p_type, u8 p_rodType, bool p_forceEncounter ) {

        u16 rn = rand( ) % ( 512 + SAVE::SAV->getActiveFile( ).m_options.m_encounterRateModifier );
        if( p_type == FISHING_ROD ) rn /= 8;
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
        u8 level = SAVE::SAV->getActiveFile( ).getEncounterLevel( tier );

        if( rn > 40 || !level ) {
            if( p_type == FISHING_ROD ) {
                IO::messageBox m( GET_STRING( 5 ), true );
                _playerIsFast = false;
                IO::NAV->draw( true );
            }
            return false;
        }
        if( p_type == FISHING_ROD ) {
            IO::messageBox m( GET_STRING( 6 ), true );
            _playerIsFast = false;
            IO::NAV->draw( true );
        } else if( SAVE::SAV->getActiveFile( ).m_repelSteps && !p_forceEncounter )
            return false;
        u8 arridx = u8( p_type ) * 15 + tier * 3;
        if( p_type != FISHING_ROD )
            while( level > CUR_SLICE->m_pokemon[ arridx ].second && ( arridx + 1 ) % 3 ) ++arridx;
        else
            arridx += p_rodType;

        if( !CUR_SLICE->m_pokemon[ arridx ].first ) return false;

        IO::fadeScreen( IO::BATTLE );
        wildPkmn             = pokemon( CUR_SLICE->m_pokemon[ arridx ].first, level );
        BATTLE::weather weat = BATTLE::weather::NO_WEATHER;
        switch( _weather ) {
        case SUNNY:
            weat = BATTLE::weather::SUN;
            break;
        case RAINY:
        case THUNDERSTORM:
            weat = BATTLE::weather::RAIN;
            break;
        case SNOW:
        case BLIZZARD:
            weat = BATTLE::weather::HAIL;
            break;
        case SANDSTORM:
            weat = BATTLE::weather::SANDSTORM;
            break;
        case FOG:
            weat = BATTLE::weather::FOG;
            break;
        case HEAVY_SUNLIGHT:
            weat = BATTLE::weather::HEAVY_SUNSHINE;
            break;
        case HEAVY_RAIN:
            weat = BATTLE::weather::HEAVY_RAIN;
            break;
        default:
            break;
        }

        u8 platform = 0, plat2 = 0;
        u8 battleBack = CURRENT_BANK.m_battleBg;
        if( p_type == GRASS || p_type == HIGH_GRASS || p_type == FISHING_ROD ) {
            if( ( CURRENT_BANK.m_mapType >> 4 ) == OUTSIDE
                && SAVE::SAV->getActiveFile( ).m_player.m_movement != SURF ) {
                plat2 = platform = 1;
            } else if( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF ) {
                plat2 = platform = 0;
            } else if( ( CURRENT_BANK.m_mapType >> 4 ) & DARK ) {
                plat2 = platform = 6;
            } else if( ( CURRENT_BANK.m_mapType >> 4 ) & CAVE ) {
                plat2 = platform = 4;
            }

            if( p_type == FISHING_ROD ) plat2 = 0;
        }

        if( !battleBack ) {
            if( p_type == GRASS ) battleBack      = 1;
            if( p_type == HIGH_GRASS ) battleBack = 3;
            if( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF )
                battleBack = 4;
            else if( ( CURRENT_BANK.m_mapType >> 4 ) & CAVE )
                battleBack = 19;
        }

        auto playerPrio
            = _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].getPriority( );
        ANIMATE_MAP = false;
        swiWaitForVBlank( );
        IO::NAV->draw( );
        BATTLE::battleTrainer* bt = SAVE::SAV->getActiveFile( ).getBattleTrainer( );
        BATTLE::battle( bt, &wildPkmn, weat, platform, plat2, battleBack ).start( );
        SAVE::SAV->getActiveFile( ).updateTeam( bt );
        delete bt;
        FADE_TOP_DARK( );
        ANIMATE_MAP = true;
        IO::NAV->draw( true );
        draw( playerPrio );

        return true;
    }
    void mapDrawer::handleTrainer( ) {
    }

    bool mapDrawer::requestWildPkmn( bool p_forceHighGrass ) {
        u8 moveData = atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                          .m_movedata;
        u8 behave = at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                        .m_bottombehave;

        if( moveData == 0x04 && behave != 0x13 )
            return handleWildPkmn( WATER, 0, true );
        else if( behave == 0x02 && !p_forceHighGrass )
            return handleWildPkmn( GRASS, 0, true );
        else if( behave == 0x03 || p_forceHighGrass )
            return handleWildPkmn( HIGH_GRASS, 0, true );
        else if( ( CURRENT_BANK.m_mapType >> 4 ) & CAVE )
            return handleWildPkmn( GRASS, 0, true );
        return false;
    }

    void mapDrawer::animateMap( u8 p_frame ) {
        if( !CUR_SLICE ) return;
        u8* tileMemory = (u8*) BG_TILE_RAM( 1 );
        if( !CUR_SLICE->m_tileSet.m_animationCount1 ) return;
        for( u8 i = 0; i < CUR_SLICE->m_tileSet.m_animationCount1; ++i ) {
            auto& a = CUR_SLICE->m_tileSet.m_animations[ i ];
            if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                dmaCopy( a.m_tiles + a.m_acFrame, tileMemory + a.m_tileIdx * 32, sizeof( tile ) );
            }
        }
        if( !CUR_SLICE->m_tileSet.m_animationCount2 ) return;
        for( u8 i = 0; i < CUR_SLICE->m_tileSet.m_animationCount2; ++i ) {
            auto& a = CUR_SLICE->m_tileSet.m_animations[ i + MAX_ANIM_PER_TILE_SET ];
            if( a.m_speed <= 1 || p_frame % a.m_speed == 0 ) {
                a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
                dmaCopy( a.m_tiles + a.m_acFrame,
                         tileMemory + ( a.m_tileIdx + MAX_TILES_PER_TILE_SET ) * 32,
                         sizeof( tile ) );
            }
        }
    }

    mapDrawer::mapDrawer( ) : _curX( 0 ), _curY( 0 ), _playerIsFast( false ), _entriesUsed( 0 ) {
    }

    // Movement stuff
    bool mapDrawer::canMove( position p_start, direction p_direction, moveMode p_moveMode ) {
        u16 nx = p_start.m_posX + dir[ p_direction ][ 0 ];
        u16 ny = p_start.m_posY + dir[ p_direction ][ 1 ];

        // Gather data about the source block
        u8 lstMoveData, lstBehave;
        if( nx / SIZE != p_start.m_posX / SIZE || ny / SIZE != p_start.m_posY / SIZE ) {
            lstMoveData = 0;
            lstBehave   = 0;
        } else {
            lstMoveData = atom( p_start.m_posX, p_start.m_posY ).m_movedata;

            auto lstblock = at( p_start.m_posX, p_start.m_posY );
            lstBehave     = lstblock.m_bottombehave;
        }

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
            if( p_direction % 2 ) return false;
            break;
        case 0xc1:
            if( p_direction % 2 == 0 ) return false;
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
        default:
            break;
        }
        switch( curBehave ) {
        // Jumpy stuff
        case 0x38:
        case 0x35:
            return p_direction == RIGHT;
        case 0x39:
        case 0x34:
            return p_direction == LEFT;
        case 0x3a:
            return p_direction == UP;
        case 0x3b:
            return p_direction == DOWN;

        case 0xa0:
            if( !( p_moveMode & WALK ) ) return false;
            break;
        case 0xc0:
            if( p_direction % 2 ) return false;
            break;
        case 0xc1:
            if( p_direction % 2 == 0 ) return false;
            break;
        case 0x13:
        case 0xd3:
        case 0xd4:
        case 0xd5:
        case 0xd6:
        case 0xd7:
            return false;
        default:
            break;
        }

        // Check for movedata stuff
        if( curMoveData % 4 == 1 ) return false;
        if( lstMoveData == 0x0a ) // Stand up (only possible for the player)
            return p_direction == SAVE::SAV->getActiveFile( ).m_player.m_direction;
        if( curMoveData == 0x0a ) // Sit down
            return ( p_moveMode == WALK );
        if( curMoveData == 4 && !( p_moveMode & SURF ) )
            return false;
        else if( curMoveData == 4 )
            return true;
        if( curMoveData == 0x0c && lstMoveData == 4 ) return true;
        if( !curMoveData || !lstMoveData ) return true;
        if( curMoveData == 0x3c ) return true;
        return curMoveData % 4 == 0 && curMoveData / 4 == p_start.m_posZ;
    }
    void mapDrawer::movePlayer( direction p_direction, bool p_fast ) {
        u8 newMoveData
            = atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                    SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                  .m_movedata;
        u8 lstMoveData = atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                               SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                             .m_movedata;

        u8 newBehave
            = at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                  .m_bottombehave;
        u8 lstBehave = at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                           SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                           .m_bottombehave;

        if( SAVE::SAV->getActiveFile( ).m_player.m_movement != moveMode::WALK )
            p_fast = false; // Running is only possible when the player is actually walking

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
                IO::messageBox m( GET_STRING( 7 ), POKE_NAV );
                _playerIsFast = false;
                IO::NAV->draw( true );
                return;
            }
            // Check for end of surf, stand up and sit down
            if( lstMoveData == 0x0a
                && newMoveData != 0x0a ) { // Stand up (only possible for the player)
                if( p_direction != SAVE::SAV->getActiveFile( ).m_player.m_direction ) return;

                standUpPlayer( p_direction );
                stepOn( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ );
                return;
            } else if( lstMoveData == 0x0a ) {
                fastBike = false;
                return;
            }
            if( newMoveData == 0x0a ) { // Sit down
                if( SAVE::SAV->getActiveFile( ).m_player.m_movement != WALK ) return;
                SAVE::SAV->getActiveFile( ).m_player.m_direction
                    = direction( ( u8( p_direction ) + 2 ) % 4 );
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setFrame(
                    getFrame( SAVE::SAV->getActiveFile( ).m_player.m_direction ) );
                sitDownPlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction, SIT );
                fastBike = false;
                return;
            }
            if( newMoveData == 0x0c && lstMoveData == 4 ) { // End of surf
                standUpPlayer( p_direction );
                stepOn( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY,
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ );
                fastBike = false;
                return;
            }

            // Check for jumps/slides/…
            if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, p_direction,
                          SAVE::SAV->getActiveFile( ).m_player.m_movement ) ) {
                stopPlayer( p_direction );
                fastBike = false;
                return;
            }

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
            // Warpy stuff
            case 0x60:
                walkPlayer( p_direction, p_fast );
                handleWarp( NO_SPECIAL );
                hadjump = false;
                break;
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
                case 0x48:
                    slidePlayer( p_direction );
                    break;
                // These change the direction of movement
                case 0x40:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case 0x41:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case 0x42:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP );
                    p_direction = UP;
                    break;
                case 0x60:
                case 0x43:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0x44:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( RIGHT );
                    p_direction = RIGHT;
                    break;
                case 0x45:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( LEFT );
                    p_direction = LEFT;
                    break;
                case 0x46:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( UP );
                    p_direction = UP;
                    break;
                case 0x47:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0xd0:
                    if( fastBike > 9 && p_direction != DOWN ) goto NEXT_PASS;
                    slidePlayer( DOWN );
                    p_direction = DOWN;
                    break;

                case 0x50:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, RIGHT,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( RIGHT, true );
                    p_direction = RIGHT;
                    break;
                case 0x51:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, LEFT,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( LEFT, true );
                    p_direction = LEFT;
                    break;
                case 0x52:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, UP,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
                        goto NEXT_PASS;
                    walkPlayer( UP, true );
                    p_direction = UP;
                    break;
                case 0x53:
                    if( !canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos, DOWN,
                                  SAVE::SAV->getActiveFile( ).m_player.m_movement ) )
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
                        break;
                    }
                    goto NEXT_PASS;
                NEXT_PASS:
                default:
                    if( reinit ) {
                        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ]
                            .setFrame( getFrame( p_direction ) );
                        fastBike = false;
                        return;
                    }
                    switch( newBehave ) {
                    case 0x20:
                    case 0x48:
                        walkPlayer( p_direction, p_fast );
                        break;

                    // Check for warpy stuff
                    case 0x60:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( CAVE_ENTRY );
                        break;
                    case 0x61:
                    case 0x68:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( NO_SPECIAL );
                        break;
                    case 0x66:
                    case 0x6e:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( LAST_VISITED );
                        break;
                    case 0x69:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( DOOR );
                        break;
                    case 0x6C:
                        walkPlayer( p_direction, p_fast );
                        handleWarp( EMERGE_WATER );
                        break;
                    case 0x70:
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
                    default:
                        moving = false;
                        continue;
                    }
                }
            }
            reinit = true;
            newMoveData
                = atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY
                            + dir[ p_direction ][ 1 ] )
                      .m_movedata;
            lstMoveData = atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata;

            newBehave
                = at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                      SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                      .m_bottombehave;
            lstBehave = at( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                            .m_bottombehave;
        }
        walkPlayer( p_direction, p_fast );
        if( atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                  SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                    .m_movedata
                > 4
            && atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                     SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                       .m_movedata
                   != 0x3c
            && atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                     SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                       .m_movedata
                   != 0x0a )
            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ
                = atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                        SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                      .m_movedata
                  / 4;
    }

    void mapDrawer::warpPlayer( warpType p_type, warpPos p_target ) {
        u8 oldMapType = CURRENT_BANK.m_mapType >> 4;
        loadNewBank( p_target.first );
        u8 newMapType = CURRENT_BANK.m_mapType >> 4;

        bool entryCave = ( !( oldMapType & CAVE ) && ( newMapType & CAVE ) );
        bool exitCave  = ( ( oldMapType & CAVE ) && !( newMapType & CAVE ) );
        switch( p_type ) {
        case DOOR:
            break;
        case TELEPORT:
            break;
        case EMERGE_WATER:
            break;
        case CAVE_ENTRY:
            if( entryCave ) {
                IO::fadeScreen( IO::CAVE_ENTRY );
                break;
            }
            if( exitCave ) {
                IO::fadeScreen( IO::CAVE_EXIT );
                break;
            }
        case LAST_VISITED:
        case NO_SPECIAL:
        default:
            IO::fadeScreen( IO::CLEAR_DARK );
            break;
        }
        swiWaitForVBlank( );
        swiWaitForVBlank( );

        SAVE::SAV->getActiveFile( ).m_player.m_pos = p_target.second;
        if( SAVE::SAV->getActiveFile( ).m_currentMap != p_target.first ) {
            SAVE::SAV->getActiveFile( ).m_currentMap = p_target.first;
            draw( );
            IO::NAV->showNewMap( SAVE::SAV->getActiveFile( ).m_currentMap );
        }

        if( exitCave ) movePlayer( DOWN );
    }

    void mapDrawer::redirectPlayer( direction p_direction, bool p_fast ) {
        // Check if the player's direction changed
        if( p_direction != SAVE::SAV->getActiveFile( ).m_player.m_direction ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setFrame(
                ( p_fast * 20 ) + getFrame( p_direction ) );
            SAVE::SAV->getActiveFile( ).m_player.m_direction = p_direction;

            if( _entriesUsed & ( 1 << 1 ) )
                _sprites[ _spritePos[ surfPlatform.m_id ] ].setFrame( getFrame( p_direction ) );
        }
    }

    void mapDrawer::standUpPlayer( direction p_direction ) {
        redirectPlayer( p_direction, false );
        bool remPlat = SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF;

        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( DOWN, 1 );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
            getFrame( p_direction ) / 3 + 12 );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        if( remPlat ) {
            _sprites[ _spritePos[ surfPlatform.m_id ] ].setVisibility( false );
            _entriesUsed ^= ( 1 << 1 );
        }
        moveCamera( p_direction, true );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( DOWN, 2 );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        swiWaitForVBlank( );
        moveCamera( p_direction, true );
        moveCamera( p_direction, true );

        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( DOWN, 1 );
        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }

        changeMoveMode( WALK );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( p_direction, true );
            swiWaitForVBlank( );
        }
    }
    void mapDrawer::sitDownPlayer( direction p_direction, moveMode p_newMoveMode ) {
        direction di = ( ( p_newMoveMode == SIT ) ? direction( ( u8( p_direction ) + 2 ) % 4 )
                                                  : p_direction );

        if( p_newMoveMode == SURF ) {
            // Load the Pkmn
            surfPlatform.m_id               = 1;
            _spritePos[ surfPlatform.m_id ] = 1;
            _sprites[ _spritePos[ surfPlatform.m_id ] ]
                = surfPlatform.show( 128 - 16, 96 - 20, 1, 1, 192 );
            _entriesUsed |= ( 1 << 1 );
            _sprites[ _spritePos[ surfPlatform.m_id ] ].setFrame( getFrame( p_direction ) );
        }

        for( u8 i = 0; i < 7; ++i ) {
            if( i == 3 )
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( UP, 2 );
            moveCamera( di, true );
            swiWaitForVBlank( );
        }
        changeMoveMode( p_newMoveMode );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
            getFrame( p_direction ) / 3 + 12 );

        for( u8 i = 0; i < 4; ++i ) {
            moveCamera( di, true );
            if( i % 2 ) swiWaitForVBlank( );
        }
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( UP, 2 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );
        moveCamera( di, true );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
            getFrame( p_direction ) );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( UP, 1 );
        swiWaitForVBlank( );
        moveCamera( di, true );
        swiWaitForVBlank( );
        moveCamera( di, true );
    }

    void mapDrawer::slidePlayer( direction p_direction ) {
        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setFrame(
                getFrame( p_direction ) );
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].nextFrame( );
        }
        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 )
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].currentFrame( );
            swiWaitForVBlank( );
        }
        stepOn( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ );
    }
    void mapDrawer::walkPlayer( direction p_direction, bool p_fast ) {
        if( SAVE::SAV->getActiveFile( ).m_player.m_movement != WALK ) p_fast = false;
        redirectPlayer( p_direction, p_fast );

        if( atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == 0x3c
            && SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ > 3
            && SAVE::SAV->getActiveFile( ).m_player.m_movement != SURF ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setPriority(
                OBJPRIORITY_1 );
        }

        if( p_fast != _playerIsFast ) {
            _playerIsFast = p_fast;
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setFrame(
                ( p_fast * 20 ) + getFrame( p_direction ) );
        }
        for( u8 i = 0; i < 16; ++i ) {
            moveCamera( p_direction, true );
            if( i == 8 )
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].nextFrame( );
            if( ( !p_fast || i % 3 ) && SAVE::SAV->getActiveFile( ).m_player.m_movement != BIKE )
                swiWaitForVBlank( );
            if( i % ( fastBike / 3 + 2 ) == 0
                && SAVE::SAV->getActiveFile( ).m_player.m_movement == BIKE )
                swiWaitForVBlank( );
        }
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
            ( p_fast * 20 ) + getFrame( p_direction ) );
        if( SAVE::SAV->getActiveFile( ).m_player.m_movement == BIKE )
            fastBike = std::min( fastBike + 1, 12 );
        else
            fastBike = false;

        if( atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                  SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                    .m_movedata
                == 0x3c
            && ( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ <= 3
                 || SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF ) ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setPriority(
                OBJPRIORITY_3 );
            if( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF )
                _sprites[ _spritePos[ surfPlatform.m_id ] ].setPriority( OBJPRIORITY_3 );
        } else if( atom(
                       SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX + dir[ p_direction ][ 0 ],
                       SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY + dir[ p_direction ][ 1 ] )
                           .m_movedata
                       != 0x3c
                   && atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                            SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY )
                              .m_movedata
                          != 0x3c ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setPriority(
                OBJPRIORITY_2 );
            if( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF )
                _sprites[ _spritePos[ surfPlatform.m_id ] ].setPriority( OBJPRIORITY_2 );
        }
        stepOn( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ );
    }
    void mapDrawer::jumpPlayer( direction p_direction ) {
        redirectPlayer( p_direction, false );
        if( _playerIsFast ) {
            _playerIsFast = false;
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setFrame(
                getFrame( p_direction ) );
        }
        for( u8 i = 0; i < 32; ++i ) {
            moveCamera( p_direction, true );
            if( i < 6 && i % 2 )
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( UP, 2 );
            if( i % 8 == 0 )
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].nextFrame( );
            if( i > 28 && i % 2 )
                _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].move( DOWN, 3 );
            if( i % 4 ) swiWaitForVBlank( );
        }
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
            getFrame( p_direction ) );
        stepOn( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX,
                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY,
                SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posZ );
    }

    void mapDrawer::stopPlayer( ) {
        while( fastBike ) {
            fastBike = std::max( 0, (s8) fastBike - 3 );
            if( canMove( SAVE::SAV->getActiveFile( ).m_player.m_pos,
                         SAVE::SAV->getActiveFile( ).m_player.m_direction, BIKE ) )
                movePlayer( SAVE::SAV->getActiveFile( ).m_player.m_direction );
            fastBike = std::max( 0, (s8) fastBike - 1 );
        }
        _playerIsFast = false;
        fastBike      = false;
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
            getFrame( SAVE::SAV->getActiveFile( ).m_player.m_direction ) );
    }
    void mapDrawer::stopPlayer( direction p_direction ) {
        if( SAVE::SAV->getActiveFile( ).m_player.m_movement == SIT
            && ( ( p_direction % 2 == SAVE::SAV->getActiveFile( ).m_player.m_direction % 2 )
                 || atom( SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX
                              + dir[ p_direction ][ 0 ],
                          SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY
                              + dir[ p_direction ][ 1 ] )
                            .m_movedata
                        != atom(
                               SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posX
                                   + dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 0 ],
                               SAVE::SAV->getActiveFile( ).m_player.m_pos.m_posY
                                   + dir[ SAVE::SAV->getActiveFile( ).m_player.m_direction ][ 1 ] )
                               .m_movedata ) ) {
            return;
        }
        fastBike = false;
        redirectPlayer( p_direction, false );
        _playerIsFast = false;
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].nextFrame( );
    }

    void mapDrawer::changeMoveMode( moveMode p_newMode ) {
        u8   basePic  = SAVE::SAV->getActiveFile( ).m_player.m_picNum / 10 * 10;
        bool newIsBig = false;
        fastBike      = false;
        u8 ydif       = 0;
        SAVE::SAV->getActiveFile( ).m_player.m_movement = p_newMode;
        switch( p_newMode ) {
        case WALK:
            SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic;
            break;
        case SURF:
            SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic + 3;
            newIsBig                                      = basePic == 0 || basePic == 10;
            ydif                                          = 2;
            break;
        case BIKE:
            SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic + 1;
            newIsBig                                      = true;
            break;
        case ACRO_BIKE:
            SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic + 2;
            newIsBig                                      = true;
            break;
        case SIT:
            SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic + 3;
            newIsBig                                      = basePic == 0 || basePic == 10;
            ydif                                          = 2;
            break;
        default:
            break;
        }
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ]
            = SAVE::SAV->getActiveFile( ).m_player.show( 128 - 8 - 8 * newIsBig, 96 - 24 - ydif, 0,
                                                         0, 0 );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].setFrame(
            getFrame( SAVE::SAV->getActiveFile( ).m_player.m_direction ) );
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

        u8 basePic = SAVE::SAV->getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic + 6;
        bool surfing = ( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF );
        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ]
            = SAVE::SAV->getActiveFile( ).m_player.show( 128 - 16 + 8 * dir[ p_direction ][ 0 ],
                                                         96 - 24 + 8 * ( p_direction == DOWN ), 0,
                                                         0, 0 );

        u8 frame                        = 0;
        if( p_direction == UP ) frame   = 4;
        if( p_direction == DOWN ) frame = 8;

        for( u8 i = 0; i < 4; ++i ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
                frame + i, p_direction == RIGHT );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        u8             rounds = rand( ) % 5;
        bool           failed = false;
        IO::messageBox fish( "", 0, false );
        for( u8 i = 0; i < rounds + 1; ++i ) {
            u8 cr = rand( ) % 7;
            fish.clear( );
            for( u8 j = 0; j < cr + 5; ++j ) {
                fish.put( " .", false );
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
            fish.carriageReturn( );
            fish.put( GET_STRING( 8 ) );
            if( FRAME_COUNT > 60 ) {
                failed = true;
                break;
            }
        }

    OUT:
        fish.clear( );
        if( failed ) fish.put( GET_STRING( 9 ) );
        IO::NAV->draw( true );
        for( s8 i = 2; i >= 0; --i ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame(
                frame + i, p_direction == RIGHT );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
        }
        changeMoveMode( surfing ? SURF : WALK );
        if( !failed ) {
            // Check if the player's leading Pokémon has sucion cups or sticky hold
            bool forceEncounter
                = ( !SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 0 ].isEgg( )
                    && ( SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                             == A_SUCTION_CUPS
                         || SAVE::SAV->getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_ability
                                == A_STICKY ) );

            // Start wild PKMN battle here
            handleWildPkmn( FISHING_ROD, p_rodType, forceEncounter );
        }

        PLAYER_IS_FISHING = false;
    }

    void mapDrawer::usePkmn( u16 p_pkmIdx, bool p_female, bool p_shiny ) {
        u8 basePic = SAVE::SAV->getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV->getActiveFile( ).m_player.m_picNum = basePic + 5;
        bool surfing = ( SAVE::SAV->getActiveFile( ).m_player.m_movement == SURF );

        _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ]
            = SAVE::SAV->getActiveFile( ).m_player.show(
                128 - 8 - 8 * ( basePic == 0 || basePic == 10 ), 96 - 24 - 3 * surfing, 0, 0, 0 );
        for( u8 i = 0; i < 5; ++i ) {
            _sprites[ _spritePos[ SAVE::SAV->getActiveFile( ).m_player.m_id ] ].drawFrame( i,
                                                                                           false );
            for( u8 j = 0; j < 5; ++j ) swiWaitForVBlank( );
        }
        u16 tl = IO::loadSprite( 124, 0, 0, 64, 32, 64, 64, BigCirc1Pal, BigCirc1Tiles,
                                 BigCirc1TilesLen, false, false, false, OBJPRIORITY_1, false );
        IO::loadSprite( 125, 0, 0, 128, 32, 64, 64, 0, 0, 0, false, true, false, OBJPRIORITY_1,
                        false );
        IO::loadSprite( 126, 0, 0, 64, 96, 64, 64, 0, 0, 0, true, false, false, OBJPRIORITY_1,
                        false );
        IO::loadSprite( 127, 0, 0, 128, 96, 64, 64, 0, 0, 0, true, true, false, OBJPRIORITY_1,
                        false );

        if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 120, 1, tl, false,
                                 p_shiny, p_female ) )
            IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, 120, 1, tl, false,
                                p_shiny, !p_female );
        IO::updateOAM( false );
        for( u8 i = 0; i < 75; ++i ) swiWaitForVBlank( );

        for( u8 i = 120; i < 128; ++i ) IO::OamTop->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( false );
        changeMoveMode( surfing ? SURF : WALK );
        if( surfing ) {
            _sprites[ _spritePos[ surfPlatform.m_id ] ]
                = surfPlatform.show( 128 - 16, 96 - 20, 1, 1, 192 );
            _sprites[ _spritePos[ surfPlatform.m_id ] ].setFrame(
                getFrame( SAVE::SAV->getActiveFile( ).m_player.m_direction ) );
        }
        swiWaitForVBlank( );
    }

    u16 mapDrawer::getCurrentLocationId( ) const {
        return getCurrentLocationId( SAVE::SAV->m_activeFile );
    }
    u16 mapDrawer::getCurrentLocationId( u8 p_file ) const {
        u8 currentBank = CURRENT_BANK.m_bank;
        if( currentBank != SAVE::SAV->m_saveFile[ p_file ].m_currentMap )
            loadNewBank( SAVE::SAV->m_saveFile[ p_file ].m_currentMap );
        u16 res = CURRENT_BANK.m_locationId;
        for( u8 i = 0; i < MAX_MAP_LOCATIONS; ++i ) {
            if( SAVE::SAV->m_saveFile[ p_file ].m_player.m_pos.m_posX
                    >= CURRENT_BANK.m_data[ i ].m_upperLeftX
                && SAVE::SAV->m_saveFile[ p_file ].m_player.m_pos.m_posX
                       <= CURRENT_BANK.m_data[ i ].m_lowerRightX
                && SAVE::SAV->m_saveFile[ p_file ].m_player.m_pos.m_posY
                       >= CURRENT_BANK.m_data[ i ].m_upperLeftY
                && SAVE::SAV->m_saveFile[ p_file ].m_player.m_pos.m_posY
                       <= CURRENT_BANK.m_data[ i ].m_lowerRightY ) {
                res = CURRENT_BANK.m_data[ i ].m_locationId;
                break;
            }
        }
        if( currentBank != SAVE::SAV->m_saveFile[ p_file ].m_currentMap )
            loadNewBank( currentBank );
        return res;
    }
}
