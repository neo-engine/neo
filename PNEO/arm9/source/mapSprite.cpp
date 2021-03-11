/*
Pokémon neo
------------------------------

file        : mapSprite.cpp
author      : Philip Wellnitz
description : Map Sprites.

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

#include "mapSprite.h"
#include "fs.h"
#include "nav.h"
#include "saveGame.h"
#include "uio.h"

#define SPR_MAPTILE_OAM( p_idx )   ( 0 + ( p_idx ) )
#define SPR_SMALL_NPC_OAM( p_idx ) ( SPR_MAPTILE_OAM( MAX_TILE_ANIM ) + ( p_idx ) )
#define SPR_LARGE_NPC_OAM( p_idx ) ( SPR_SMALL_NPC_OAM( MAX_SMALL_NPC ) + ( p_idx ) )
#define SPR_MAIN_PLAYER_OAM        ( SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) )
#define SPR_HM_OAM( p_idx )        ( 1 + SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) + ( p_idx ) )

#define MAX_OAM ( SPR_HM_OAM( MAX_HM_PARTICLE ) )

#define SPR_MAIN_PLAYER_PLAT_OAM ( SPR_HM_OAM( MAX_HM_PARTICLE ) )
#define SPR_DOOR_OAM             ( SPR_MAIN_PLAYER_PLAT_OAM + 1 )
#define SPR_EXCLM_OAM            SPR_DOOR_OAM + 1

// Strength boulder (16x16)
// Rock Smash rock  (16x16)
// Cut tree         (16x16)
// Map animation (rustling grass, etc)

#define SPR_MAIN_PLAYER_GFX        0
#define SPR_MAIN_PLAYER_PLAT_GFX   16
#define SPR_SMALL_NPC_GFX( p_idx ) ( 32 + 8 * ( p_idx ) )
#define SPR_LARGE_NPC_GFX( p_idx ) ( SPR_SMALL_NPC_GFX( MAX_SMALL_NPC ) + 16 * ( p_idx ) )
#define SPR_HM_GFX( p_idx )        ( SPR_LARGE_NPC_GFX( MAX_LARGE_NPC ) + 4 * ( p_idx ) )
#define SPR_MAPTILE_GFX( p_idx )   ( SPR_HM_GFX( MAX_HM_PARTICLE_GFX_SLOTS ) + 4 * ( p_idx ) )
#define SPR_DOOR_GFX               ( SPR_MAPTILE_GFX( MAX_TILE_ANIM_GFX_SLOTS ) )

#define SPR_EXCLM_GFX 303

namespace MAP {
    void mapSpriteData::readData( FILE* p_f ) {
        if( !p_f ) {
#ifdef DESQUID
            NAV::printMessage( "Sprite failed" );
#endif
        } else {
            FS::read( p_f, m_palData, sizeof( u16 ), 16 );
            FS::read( p_f, &m_frameCount, sizeof( u8 ), 1 );
            FS::read( p_f, &m_width, sizeof( u8 ), 1 );
            FS::read( p_f, &m_height, sizeof( u8 ), 1 );
            FS::read( p_f, m_frameData, sizeof( u32 ), m_width * m_height * m_frameCount / 8 );
            FS::close( p_f );
        }
    }

    void mapSpriteData::updatePalette( u8 p_bgPalIdx ) {
        std::memcpy( m_palData, &BG_PALETTE[ 16 * p_bgPalIdx ], 16 * sizeof( u16 ) );
    }

    mapSpriteData::mapSpriteData( u16 p_imageId, u8 p_forme, bool p_shiny, bool p_female ) {
        FILE* f;
        if( p_imageId > PKMN_SPRITE ) {
            u16  species = p_imageId - PKMN_SPRITE;
            u8   forme   = p_forme;
            bool shiny   = p_shiny;
            bool female  = p_female;

            char buf[ 100 ];
            if( !forme ) {
                snprintf( buf, 99, "%02d/%hu%s%s", species / FS::ITEMS_PER_DIR, species,
                          shiny ? "s" : "", female ? "f" : "" );
            } else {
                snprintf( buf, 99, "%02d/%hu%s%s_%hhu", species / FS::ITEMS_PER_DIR, species,
                          shiny ? "s" : "", female ? "f" : "", forme );
            }
            f = FS::open( IO::OWP_PATH, buf, ".rsd" );
        } else if( p_imageId < 250 ) {
            f = FS::open( IO::OW_PATH, p_imageId, ".rsd" );
        } else {
            if( p_imageId == 250 ) {
                // load player appearance
                p_imageId = SAVE::SAV.getActiveFile( ).m_appearance;
            } else if( p_imageId == 251 ) {
                // load rival's appearance
                p_imageId = SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE );

            } else {
                p_imageId &= 255;
            }
            f = FS::openSplit( IO::TRAINER_PATH, p_imageId, ".rsd", 255 );
        }
        readData( f );
    }

    mapSpriteData::mapSpriteData( u8 p_door, u16 p_palData[ 16 ] ) {
        FILE* f      = FS::openSplit( IO::DOOR_PATH, p_door, ".door", 52 );
        m_width      = 16;
        m_height     = 32;
        m_frameCount = 3;
        if( f ) {
            FS::read( f, m_frameData, sizeof( u32 ), m_width * m_height * m_frameCount / 8 );
            FS::close( f );
        } else {
            std::memset( m_frameData, 0, sizeof( m_frameData ) );
        }
        std::memcpy( m_palData, p_palData, 16 * sizeof( u16 ) );
    }

    mapSprite::mapSprite( u16 p_imageId, u8 p_startFrame, u8 p_forme, bool p_shiny,
                          bool p_female ) {
        _data            = mapSpriteData( p_imageId, p_forme, p_shiny, p_female );
        _info.m_picNum   = p_imageId;
        _info.m_curFrame = p_startFrame;
    }

    mapSprite::mapSprite( FILE* p_f, u8 p_startFrame ) {
        _data.readData( p_f );
        _info.m_picNum   = -1;
        _info.m_curFrame = p_startFrame;
    }

    void mapSprite::drawFrame( u8 p_oamIdx, u8 p_value ) {
        u8 frame = p_value;
        if( frame % PLAYER_FAST >= 9 ) frame -= 3;
        if( p_value % PLAYER_FAST == 15 ) frame--;

        drawFrame( p_oamIdx, frame / PLAYER_FAST * 9 + frame % PLAYER_FAST,
                   ( frame != p_value )
                       && ( p_value % PLAYER_FAST < 12 || p_value % PLAYER_FAST == 15 ) );
    }

    void mapSprite::drawFrameD( u8 p_oamIdx, direction p_direction ) {
        if( _info.m_picNum > PKMN_SPRITE ) {
            drawFrame( p_oamIdx, getOWPKMNFrame( p_direction ), false );
        } else {
            drawFrame( p_oamIdx, getFrame( p_direction ) );
        }
    }

    void mapSprite::drawFrame( u8 p_oamIdx, u8 p_value, bool p_hFlip ) {

#ifdef DESQUID
//        NAV::printMessage( ( std::string( " Draw Frame " ) + std::to_string( p_value ) ).c_str( )
//        );
#endif
        IO::setOWSpriteFrame( p_value, p_hFlip, p_oamIdx, _data.m_palData, _data.m_frameData );
    }

    void mapSprite::setFrame( u8 p_oamIdx, u8 p_value ) {
        _info.m_curFrame = p_value;
        drawFrame( p_oamIdx, _info.m_curFrame );
    }

    void mapSprite::setFrameD( u8 p_oamIdx, direction p_direction ) {
        _info.m_curFrame = getFrameForDir( p_direction );
        drawFrame( p_oamIdx, _info.m_curFrame );
    }

    void mapSprite::currentFrame( u8 p_oamIdx ) {
        drawFrame( p_oamIdx, _info.m_curFrame );
    }
    void mapSprite::nextFrame( u8 p_oamIdx ) {
        if( _info.m_picNum > PKMN_SPRITE ) {
            if( ( ++_info.m_curFrame ) % 2 == 0 ) _info.m_curFrame -= 2;
        } else {
            if( ( ( ++_info.m_curFrame ) % PLAYER_FAST ) % 3 == 0 ) _info.m_curFrame -= 2;
        }
        drawFrame( p_oamIdx, _info.m_curFrame );
    }

    u8 mapSprite::getFrameForDir( direction p_direction ) const {
        if( _info.m_picNum > PKMN_SPRITE ) {
            return MAP::getOWPKMNFrame( p_direction );
        } else {
            return MAP::getFrame( p_direction );
        }
    }

    void mapSpriteManager::init( ) {
        // pre load item, hm sprites
        _itemBallData  = mapSpriteData( 256 | 249 );
        _hmBallData    = mapSpriteData( 256 | 250 );
        _strengthData  = mapSpriteData( 256 | 251 );
        _rockSmashData = mapSpriteData( 256 | 252 );
        _cutData       = mapSpriteData( 256 | 253 );
        _grassData     = mapSpriteData( 256 | 254 );
        _longGrassData = mapSpriteData( 256 | 255 );

        _grassData.updatePalette( 3 );
        _longGrassData.updatePalette( 3 );

        _playerPlatform.m_sprite = mapSprite( { 256 | 248, 0 }, mapSpriteData( 256 | 248 ) );

        for( u8 i = 0; i < 128; ++i ) {
            _oamPosition[ i ]  = i;
            _oamPositionR[ i ] = i;
        }
    }

    void mapSpriteManager::reset( ) {
        IO::initOAMTable( false );

        destroySprite( SPR_MAIN_PLAYER_OAM, false );
        destroySprite( SPR_MAIN_PLAYER_PLAT_OAM, false );
        for( u8 i = 0; i < MAX_SMALL_NPC; ++i ) { destroySprite( SPR_SMALL_NPC_OAM( i ), false ); }
        for( u8 i = 0; i < MAX_LARGE_NPC; ++i ) { destroySprite( SPR_LARGE_NPC_OAM( i ), false ); }
        for( u8 i = 0; i < MAX_HM_PARTICLE; ++i ) { destroySprite( SPR_HM_OAM( i ), false ); }
        for( u8 i = 0; i < MAX_TILE_ANIM; ++i ) { destroySprite( SPR_MAPTILE_OAM( i ), false ); }
        for( u8 i = 0; i < 128; ++i ) {
            _oamPosition[ i ]  = i;
            _oamPositionR[ i ] = i;
        }

        update( );
    }

    /*
     * @brief: Loads the specified sprite at the specified position on the screen.
     */
    void doLoadSprite( u16 p_posX, u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       const mapSpriteData& p_data, bool p_hidden = false ) {
        IO::loadSpriteB( p_oamIdx, p_tileCnt, p_posX, p_posY, p_data.m_width, p_data.m_height,
                         p_data.m_palData,
                         reinterpret_cast<const unsigned int*>( p_data.m_frameData ),
                         p_data.m_width * p_data.m_height / 2, false, false,
                         p_hidden
                             || !mapSpritePos{ 0, 0, 0, 0, s16( 128 - p_posX ), s16( 92 - p_posY ) }
                                     .isVisible( ),
                         OBJPRIORITY_2, false );
    }

    void doLoadSprite( u16 p_posX, u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       const mapSprite& p_sprite, bool p_hidden = false ) {
        auto& data = p_sprite.getData( );
        doLoadSprite( p_posX, p_posY, p_oamIdx, p_tileCnt, data, p_hidden );
    }

    const mapSpriteManager::managedSprite&
    mapSpriteManager::getManagedSprite( u8 p_spriteId ) const {
        if( p_spriteId == SPR_DOOR_OAM ) {
            return _doorAnimation;
        } else if( p_spriteId == SPR_MAIN_PLAYER_OAM ) {
            return _player;
        } else if( p_spriteId == SPR_MAIN_PLAYER_PLAT_OAM ) {
            return _playerPlatform;
        } else if( p_spriteId >= SPR_SMALL_NPC_OAM( 0 )
                   && p_spriteId < SPR_SMALL_NPC_OAM( MAX_SMALL_NPC ) ) {
            if( _smallNpcs[ p_spriteId - SPR_SMALL_NPC_OAM( 0 ) ].first ) {
                return _smallNpcs[ p_spriteId - SPR_SMALL_NPC_OAM( 0 ) ].second;
            }
        } else if( p_spriteId >= SPR_LARGE_NPC_OAM( 0 )
                   && p_spriteId < SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) ) {
            if( _bigNpcs[ p_spriteId - SPR_LARGE_NPC_OAM( 0 ) ].first ) {
                return _bigNpcs[ p_spriteId - SPR_LARGE_NPC_OAM( 0 ) ].second;
            }
        }
        return _player;
    }

    mapSpriteManager::managedSprite& mapSpriteManager::getManagedSprite( u8 p_spriteId ) {
        if( p_spriteId == SPR_DOOR_OAM ) {
            return _doorAnimation;
        } else if( p_spriteId == SPR_MAIN_PLAYER_OAM ) {
            return _player;
        } else if( p_spriteId == SPR_MAIN_PLAYER_PLAT_OAM ) {
            return _playerPlatform;
        } else if( p_spriteId >= SPR_SMALL_NPC_OAM( 0 )
                   && p_spriteId < SPR_SMALL_NPC_OAM( MAX_SMALL_NPC ) ) {
            if( _smallNpcs[ p_spriteId - SPR_SMALL_NPC_OAM( 0 ) ].first ) {
                return _smallNpcs[ p_spriteId - SPR_SMALL_NPC_OAM( 0 ) ].second;
            }
        } else if( p_spriteId >= SPR_LARGE_NPC_OAM( 0 )
                   && p_spriteId < SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) ) {
            if( _bigNpcs[ p_spriteId - SPR_LARGE_NPC_OAM( 0 ) ].first ) {
                return _bigNpcs[ p_spriteId - SPR_LARGE_NPC_OAM( 0 ) ].second;
            }
        }
        return _player;
    }

    const mapSpriteData& mapSpriteManager::getSpriteData( u8 p_spriteId ) const {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            switch( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].first ) {
            case SPR_ITEM: return _itemBallData;
            case SPR_HMBALL: return _hmBallData;
            case SPR_STRENGTH: return _strengthData;
            case SPR_ROCKSMASH: return _rockSmashData;
            case SPR_CUT: return _cutData; [[unlikely]] default : break;
            }
        }
        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            switch( _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].first ) {
            case SPR_GRASS: {
                return _grassData;
            }
            case SPR_LONG_GRASS: {
                return _longGrassData;
            }
                [[unlikely]] default : break;
            }
        }

        return getManagedSprite( p_spriteId ).m_sprite.getData( );
    }

    mapSpriteData& mapSpriteManager::getSpriteData( u8 p_spriteId ) {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            switch( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].first ) {
            case SPR_ITEM: return _itemBallData;
            case SPR_HMBALL: return _hmBallData;
            case SPR_STRENGTH: return _strengthData;
            case SPR_ROCKSMASH: return _rockSmashData;
            case SPR_CUT: return _cutData; [[unlikely]] default : break;
            }
        }
        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            switch( _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].first ) {
            case SPR_GRASS: {
                return _grassData;
            }
            case SPR_LONG_GRASS: return _longGrassData; [[unlikely]] default : break;
            }
        }
        return getManagedSprite( p_spriteId ).m_sprite.getData( );
    }

    void mapSpriteManager::swapSprites( u8 p_spriteId1, u8 p_spriteId2, bool p_update ) {
        std::swap( IO::spriteInfoTop[ p_spriteId1 ], IO::spriteInfoTop[ p_spriteId2 ] );
        std::swap( IO::OamTop->oamBuffer[ p_spriteId1 ], IO::OamTop->oamBuffer[ p_spriteId2 ] );
        std::swap( _oamPositionR[ p_spriteId1 ], _oamPositionR[ p_spriteId2 ] );
        std::swap( _oamPosition[ _oamPositionR[ p_spriteId1 ] ],
                   _oamPosition[ _oamPositionR[ p_spriteId2 ] ] );

        if( p_update ) { IO::updateOAM( false ); }
    }

    s8 cmpSprY( u8 p_idx1, u8 p_idx2 ) {
        u16 val1 = u16( IO::OamTop->oamBuffer[ p_idx1 ].y ) + 1000;
        if( val1 > 1192 ) { val1 -= 256; }
        u16 val2 = u16( IO::OamTop->oamBuffer[ p_idx2 ].y ) + 1000;
        if( val2 > 1192 ) { val2 -= 256; }

        val1 += IO::spriteInfoTop[ p_idx1 ].m_height;
        val2 += IO::spriteInfoTop[ p_idx2 ].m_height;
        /*
                if( debug ) {
                    NAV::printMessage( ( std::to_string( p_idx1 ) + ": " + std::to_string( val1 ) +
           "\n"
                                         + std::to_string( p_idx2 ) + " : " + std::to_string( val2 )
           ) .c_str( ) );
                }
                */

        if( val1 < val2 )
            return -1;
        else if( val1 == val2 )
            return 0;
        return 1;
    }

    void mapSpriteManager::reorderSprites( bool p_update ) {
        static bool reordering = false;

        if( reordering ) { return; }
        reordering = true;

        // sort things via bubble sort; there are only few elements so it should be fast
        // enough
        for( u8 i = 0; i < MAX_OAM; ++i ) {
            bool swp = false;
            for( u8 j = 1; j < MAX_OAM - i; ++j ) {
                //                NAV::printMessage( ( std::to_string( j - 1 ) + " sw " +
                //                std::to_string( j ) + " : "
                //                                     + std::to_string( IO::OamTop->oamBuffer[ j -
                //                                     1 ].y ) + " vs "
                //                                     + std::to_string( IO::OamTop->oamBuffer[ j
                //                                     ].y ) )
                //                                      .c_str( ) );

                // take care of potentially negative coordinates
                auto cmp = cmpSprY( j - 1, j );
                if( cmp < 0 || ( cmp == 0 && _oamPositionR[ j - 1 ] > _oamPositionR[ j ] ) ) {
                    swapSprites( j - 1, j, false );
                    swp = true;
                }
            }
            if( !swp ) { break; }
        }

        if( _oamPosition[ SPR_MAIN_PLAYER_OAM ] > _oamPosition[ SPR_MAIN_PLAYER_PLAT_OAM ] ) {
            //            swapSprites( _oamPosition[ SPR_MAIN_PLAYER_OAM ],
            //                         _oamPosition[ SPR_MAIN_PLAYER_PLAT_OAM ], false );
        }

        if( p_update ) { update( ); }
        reordering = false;
    }

    u8 mapSpriteManager::loadSprite( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY,
                                     spriteType p_type, const mapSprite& p_sprite, bool p_hidden ) {

        switch( p_type ) {
        case SPTYPE_DOOR:
            // At most one door animation at a time; overwrite any existing door animation
            _doorAnimation = {
                p_sprite,
                { p_posX, p_posY, 0, 0, camShift( p_camX, p_posX ), camShift( p_camY, p_posY ) },
                SPTYPE_DOOR };
            doLoadSprite( screenX( p_camX, p_posX, p_sprite.getData( ).m_width ),
                          screenY( p_camY, p_posY, p_sprite.getData( ).m_height ),
                          _oamPosition[ SPR_DOOR_OAM ], SPR_DOOR_GFX, p_sprite, p_hidden );
            return SPR_DOOR_OAM;
        case SPTYPE_PLAYER:
            _player = { p_sprite, { p_posX, p_posY, 0, 0, 0, 0 }, SPTYPE_PLAYER };
            doLoadSprite( screenX( p_camX, p_posX, p_sprite.getData( ).m_width ),
                          screenY( p_camY, p_posY, p_sprite.getData( ).m_height ),
                          _oamPosition[ SPR_MAIN_PLAYER_OAM ], SPR_MAIN_PLAYER_GFX, p_sprite,
                          p_hidden );
            reorderSprites( false );
            return SPR_MAIN_PLAYER_OAM;
        case SPTYPE_BERRYTREE:
        case SPTYPE_NPC: {
            bool isBig = p_sprite.getData( ).m_width == 32;

            if( isBig ) {
                // search for free space
                u8 freesp = 255;
                for( u8 i = 0; i < MAX_LARGE_NPC; ++i ) {
                    if( !_bigNpcs[ i ].first ) {
                        freesp = i;
                        break;
                    }
                }
                if( freesp == 255 ) { return 255; }
                _bigNpcs[ freesp ] = { true,
                                       { p_sprite,
                                         { p_posX, p_posY, 0, 0, camShift( p_camX, p_posX ),
                                           camShift( p_camY, p_posY ) },
                                         p_type } };
                doLoadSprite( screenX( p_camX, p_posX, p_sprite.getData( ).m_width ),
                              screenY( p_camY, p_posY, p_sprite.getData( ).m_height ),
                              _oamPosition[ SPR_LARGE_NPC_OAM( freesp ) ],
                              SPR_LARGE_NPC_GFX( freesp ), p_sprite, p_hidden );
                reorderSprites( false );
                return SPR_LARGE_NPC_OAM( freesp );
            } else {
                u8 freesp = 255;
                for( u8 i = 0; i < MAX_SMALL_NPC; ++i ) {
                    if( !_smallNpcs[ i ].first ) {
                        freesp = i;
                        break;
                    }
                }
                if( freesp == 255 ) { return 255; }
                _smallNpcs[ freesp ] = { true,
                                         { p_sprite,
                                           { p_posX, p_posY, 0, 0, camShift( p_camX, p_posX ),
                                             camShift( p_camY, p_posY ) },
                                           p_type } };
                doLoadSprite( screenX( p_camX, p_posX, p_sprite.getData( ).m_width ),
                              screenY( p_camY, p_posY, p_sprite.getData( ).m_height ),
                              _oamPosition[ SPR_SMALL_NPC_OAM( freesp ) ],
                              SPR_SMALL_NPC_GFX( freesp ), p_sprite, p_hidden );
                reorderSprites( false );
                return SPR_SMALL_NPC_OAM( freesp );
            }
        }
        default: break;
        }

        return 255;
    }

    u8 mapSpriteManager::loadBerryTree( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY,
                                        u8 p_berryIdx, u8 p_stage ) {
        FILE* f;
        u8    fr = 0;
        if( p_stage <= 1 ) { // generic sprite for all berries
            f  = FS::open( IO::BERRY_PATH, 999, ".rsd" );
            fr = 2 * p_stage;
        } else { // custom sprite
            f = FS::open( IO::BERRY_PATH, p_berryIdx, ".rsd" );
            if( !f ) { f = FS::open( IO::BERRY_PATH, u16( 0 ), ".rsd" ); }
            fr = 2 * ( p_stage - 2 );
        }
        return loadSprite( p_camX, p_camY, p_posX, p_posY, SPTYPE_BERRYTREE, mapSprite( f, fr ) );
    }

    mapSpriteData DOOR_DATA;

    u8 mapSpriteManager::loadDoor( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY, u8 p_doorIdx,
                                   u16 p_palette[ 16 ] ) {
        DOOR_DATA          = mapSpriteData( p_doorIdx, p_palette );
        mapSpriteInfo info = { p_doorIdx, 0 };
        return loadSprite( p_camX, p_camY, p_posX, p_posY, SPTYPE_DOOR,
                           mapSprite( info, DOOR_DATA ) );
    }

    u8 mapSpriteManager::loadSprite( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY,
                                     u8 p_particleId, bool p_hidden ) {

        bool isTileA = p_particleId >= TILE_ANIM_START;

        auto mx   = isTileA ? MAX_TILE_ANIM : MAX_HM_PARTICLE;
        auto info = isTileA ? _tileAnimInfo : _hmSpriteInfo;

        u8 nextfree = 255;
        if( p_particleId != SPR_PLATFORM ) {
            for( u8 i = 0; i < mx; ++i ) {
                if( !info[ i ].first ) {
                    nextfree = i;
                    break;
                }
            }
        }

        if( p_particleId != SPR_PLATFORM && nextfree == 255 ) {
            // No space for an additional particle
            return 255;
        } else if( p_particleId != SPR_PLATFORM ) {
            info[ nextfree ] = {
                p_particleId,
                { p_posX, p_posY, 0, 0, camShift( p_camX, p_posX ), camShift( p_camY, p_posY ) } };
        }

#ifdef DESQUID_MORE
        NAV::printMessage( ( std::string( "load hm object " )
                             + std::to_string( screenX( p_camX, p_posX, 16 ) ) + "."
                             + std::to_string( screenY( p_camY, p_posY, 16 ) ) + "." )
                               .c_str( ) );
#endif
        switch( p_particleId ) {
        case SPR_ITEM:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_HM_OAM( nextfree ) ], SPR_HM_GFX( p_particleId ),
                          _itemBallData, p_hidden );
            reorderSprites( false );
            return SPR_HM_OAM( nextfree );
        case SPR_HMBALL:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_HM_OAM( nextfree ) ], SPR_HM_GFX( p_particleId ),
                          _hmBallData, p_hidden );
            reorderSprites( false );
            return SPR_HM_OAM( nextfree );
        case SPR_STRENGTH:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_HM_OAM( nextfree ) ], SPR_HM_GFX( p_particleId ),
                          _strengthData, p_hidden );
            reorderSprites( false );
            return SPR_HM_OAM( nextfree );
        case SPR_ROCKSMASH:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_HM_OAM( nextfree ) ], SPR_HM_GFX( p_particleId ),
                          _rockSmashData, p_hidden );
            reorderSprites( false );
            return SPR_HM_OAM( nextfree );
        case SPR_CUT:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_HM_OAM( nextfree ) ], SPR_HM_GFX( p_particleId ),
                          _cutData, p_hidden );
            reorderSprites( false );
            return SPR_HM_OAM( nextfree );

        case SPR_GRASS:
            _grassData.updatePalette( 2 );
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_MAPTILE_OAM( nextfree ) ],
                          SPR_MAPTILE_GFX( 2 * ( p_particleId % 100 ) ), _grassData, p_hidden );
            setPriority( SPR_MAPTILE_OAM( nextfree ), OBJPRIORITY_2 );
            return SPR_MAPTILE_OAM( nextfree );
        case SPR_LONG_GRASS:
            _longGrassData.updatePalette( 2 );
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          _oamPosition[ SPR_MAPTILE_OAM( nextfree ) ],
                          SPR_MAPTILE_GFX( 2 * ( p_particleId % 100 ) ), _longGrassData, p_hidden );
            setPriority( SPR_MAPTILE_OAM( nextfree ), OBJPRIORITY_2 );
            return SPR_MAPTILE_OAM( nextfree );

        case SPR_PLATFORM:
            _playerPlatform.m_pos = { p_posX, p_posY, 0, 0, 0, 0 };
            doLoadSprite( screenX( p_camX, p_posX, 32 ), screenY( p_camY, p_posY, 32 ) + 3,
                          _oamPosition[ SPR_MAIN_PLAYER_PLAT_OAM ], SPR_MAIN_PLAYER_PLAT_GFX,
                          _playerPlatform.m_sprite, p_hidden );
            reorderSprites( false );
            return SPR_MAIN_PLAYER_PLAT_OAM;
        default: break;
        }

        return 255;
    }

    void mapSpriteManager::destroySprite( u8 p_spriteId, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

#ifdef DESQUID_MORE
        NAV::printMessage( ( std::to_string( p_spriteId ) + " destroy" ).c_str( ) );
#endif

        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden = true;
        if( p_spriteId >= SPR_SMALL_NPC_OAM( 0 )
            && p_spriteId < SPR_SMALL_NPC_OAM( MAX_SMALL_NPC ) ) {
            _smallNpcs[ p_spriteId - SPR_SMALL_NPC_OAM( 0 ) ].first = false;
        } else if( p_spriteId >= SPR_LARGE_NPC_OAM( 0 )
                   && p_spriteId < SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) ) {
            _bigNpcs[ p_spriteId - SPR_LARGE_NPC_OAM( 0 ) ].first = false;
        } else if( p_spriteId >= SPR_MAPTILE_OAM( 0 )
                   && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].first = SPR_UNUSED;
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].first = SPR_UNUSED;
        }
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::moveCamera( direction p_direction, s16 p_amount, bool p_movePlayer ) {

        s8 dx = -p_amount * dir[ p_direction ][ 0 ];
        s8 dy = -p_amount * dir[ p_direction ][ 1 ];

        if( p_movePlayer ) {
            translateSprite( SPR_MAIN_PLAYER_OAM, dx, dy, false );
            translateSprite( SPR_MAIN_PLAYER_PLAT_OAM, dx, dy, false );
        }
        for( u8 i = 0; i < MAX_SMALL_NPC; ++i ) {
            if( _smallNpcs[ i ].first ) {
                translateSprite( SPR_SMALL_NPC_OAM( i ), dx, dy, false );
            }
        }
        for( u8 i = 0; i < MAX_LARGE_NPC; ++i ) {
            if( _bigNpcs[ i ].first ) { translateSprite( SPR_LARGE_NPC_OAM( i ), dx, dy, false ); }
        }
        for( u8 i = 0; i < MAX_HM_PARTICLE; ++i ) {
            if( _hmSpriteInfo[ i ].first ) { translateSprite( SPR_HM_OAM( i ), dx, dy, false ); }
        }
        for( u8 i = 0; i < MAX_TILE_ANIM; ++i ) {
            if( _tileAnimInfo[ i ].first ) {
                translateSprite( SPR_MAPTILE_OAM( i ), dx, dy, false );
            }
        }
        translateSprite( SPR_DOOR_OAM, dx, dy, false );

        reorderSprites( false );
        update( );
    }

    void mapSpriteManager::moveSprite( u8 p_spriteId, direction p_direction, s16 p_amount,
                                       bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        moveSprite( p_spriteId, p_amount * dir[ p_direction ][ 0 ],
                    p_amount * dir[ p_direction ][ 1 ], p_update );
    }

    void mapSpriteManager::moveSprite( u8 p_spriteId, u8 p_targetSpriteId, bool p_update ) {
        if( p_spriteId == 255 ) { return; }
        if( p_targetSpriteId == 255 ) { return; }

        auto tx   = IO::OamTop->oamBuffer[ _oamPosition[ p_targetSpriteId ] ].x;
        auto ty   = IO::OamTop->oamBuffer[ _oamPosition[ p_targetSpriteId ] ].y;
        auto data = getManagedSprite( p_targetSpriteId ).m_sprite.getData( );
        ty += data.m_height - 16;
        if( data.m_width == 32 ) { tx += data.m_width - 8; }

        moveSpriteT( p_spriteId, tx, ty, p_update );
    }

    void mapSpriteManager::moveSpriteT( u8 p_spriteId, u16 p_targetX, u16 p_targetY,
                                        bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        moveSprite( p_spriteId, p_targetX - IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x,
                    p_targetY - IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y, p_update );
    }

    void mapSpriteManager::moveSprite( u8 p_spriteId, s8 p_dx, s8 p_dy, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x += p_dx;
        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y += p_dy;

        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].second.moveSprite( p_dx, p_dy );
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.moveSprite( p_dx, p_dy );
        } else {
            getManagedSprite( p_spriteId ).m_pos.moveSprite( p_dx, p_dy );
        }
        reorderSprites( false );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::translateSprite( u8 p_spriteId, s8 p_dx, s8 p_dy, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x += p_dx;
        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y += p_dy;

        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].second.translateSprite( p_dx, p_dy );
            IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden
                = !_tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].second.isVisible( );
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.translateSprite( p_dx, p_dy );
            IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden
                = !_hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.isVisible( );
#ifdef DESQUID_MORE
            NAV::printMessage(
                ( std::to_string( p_spriteId )
                  + " x: " + std::to_string( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x )
                  + " y : "
                  + std::to_string( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y )
                  + " cam x : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_camDisX )
                  + " cam y : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_camDisY )
                  + "\npos x : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_curX )
                  + " pos y : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_curY ) )
                    .c_str( ) );
#endif

        } else if( p_spriteId != SPR_DOOR_OAM ) {
            getManagedSprite( p_spriteId ).m_pos.translateSprite( p_dx, p_dy );
            IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden
                = !getManagedSprite( p_spriteId ).m_pos.isVisible( );
#ifdef DESQUID_MORE
            NAV::printMessage(
                ( std::to_string( p_spriteId ) + " hidden? "
                  + std::to_string( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden ) )
                    .c_str( ) );
#endif
        }

        if( p_update ) { update( ); }
    }

    ObjPriority mapSpriteManager::getPriority( u8 p_spriteId ) const {
        if( p_spriteId == 255 ) { return OBJPRIORITY_0; }

        return IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].priority;
    }

    void mapSpriteManager::setVisibility( u8 p_spriteId, bool p_value, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden = p_value;
        if( p_update ) { update( ); }
    }

    bool mapSpriteManager::getVisibility( u8 p_spriteId ) {
        if( p_spriteId == 255 ) { return true; }

        return IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].isHidden;
    }

    void mapSpriteManager::setPriority( u8 p_spriteId, ObjPriority p_value, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].priority = p_value;
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::drawFrame( u8 p_spriteId, u8 p_value, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            auto data = getSpriteData( p_spriteId );
            auto pid  = _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].first;
            doLoadSprite( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x,
                          IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y,
                          _oamPosition[ p_spriteId ], SPR_MAPTILE_GFX( 2 * ( pid % 100 ) + 1 ),
                          data );
            IO::setOWSpriteFrame( p_value, false, _oamPosition[ p_spriteId ], data.m_palData,
                                  data.m_frameData );
            return;
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
            return;
        }
        getManagedSprite( p_spriteId ).m_sprite.drawFrame( _oamPosition[ p_spriteId ], p_value );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::drawFrameD( u8 p_spriteId, direction p_direction, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) )
            [[unlikely]] {
            return;
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
            return;
        }
        getManagedSprite( p_spriteId )
            .m_sprite.drawFrameD( _oamPosition[ p_spriteId ], p_direction );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::drawFrame( u8 p_spriteId, u8 p_value, bool p_hflip, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_MAPTILE_OAM( 0 ) && p_spriteId < SPR_MAPTILE_OAM( MAX_TILE_ANIM ) ) {
            auto data = getSpriteData( p_spriteId );
            auto pid  = _tileAnimInfo[ p_spriteId - SPR_MAPTILE_OAM( 0 ) ].first;
            doLoadSprite( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x,
                          IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y,
                          _oamPosition[ p_spriteId ], SPR_MAPTILE_GFX( 2 * ( pid % 100 ) + 1 ),
                          data );
            IO::setOWSpriteFrame( p_value, p_hflip, _oamPosition[ p_spriteId ], data.m_palData,
                                  data.m_frameData );
            return;
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            switch( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].first ) {
            case SPR_ROCKSMASH:
                doLoadSprite( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x,
                              IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y,
                              _oamPosition[ p_spriteId ], SPR_HM_GFX( 0 ), _rockSmashData );
                IO::setOWSpriteFrame( p_value, p_hflip, _oamPosition[ p_spriteId ],
                                      _rockSmashData.m_palData, _rockSmashData.m_frameData );
                break;
            case SPR_CUT:
                doLoadSprite( IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x,
                              IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y,
                              _oamPosition[ p_spriteId ], SPR_HM_GFX( 0 ), _cutData );
                IO::setOWSpriteFrame( p_value, p_hflip, _oamPosition[ p_spriteId ],
                                      _cutData.m_palData, _cutData.m_frameData );
                break;
            default: return;
            }

            if( p_update ) { update( ); }
            return;
        }
        getManagedSprite( p_spriteId )
            .m_sprite.drawFrame( _oamPosition[ p_spriteId ], p_value, p_hflip );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::setFrame( u8 p_spriteId, u8 p_value, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
            return;
        }
        getManagedSprite( p_spriteId ).m_sprite.setFrame( _oamPosition[ p_spriteId ], p_value );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::setFrameD( u8 p_spriteId, direction p_direction, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
            return;
        }
        getManagedSprite( p_spriteId )
            .m_sprite.setFrameD( _oamPosition[ p_spriteId ], p_direction );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::currentFrame( u8 p_spriteId, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
            return;
        }
        getManagedSprite( p_spriteId ).m_sprite.currentFrame( _oamPosition[ p_spriteId ] );
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::nextFrame( u8 p_spriteId, bool p_update ) {
        if( p_spriteId == 255 ) { return; }

        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
            return;
        }
        auto& mspr = getManagedSprite( p_spriteId );
        if( mspr.m_type != SPTYPE_BERRYTREE ) {
            mspr.m_sprite.nextFrame( _oamPosition[ p_spriteId ] );
        } else {
            mspr.m_sprite.setFrame( _oamPosition[ p_spriteId ],
                                    mspr.m_sprite.getCurrentFrame( ) ^ 1 );
        }
        if( p_update ) { update( ); }
    }

    void mapSpriteManager::update( ) {
        IO::updateOAM( false );
    }

    void mapSpriteManager::showExclamation( u8 p_spriteId, u8 p_emote ) {
        auto spr = getManagedSprite( p_spriteId );
        char buffer[ 10 ];
        snprintf( buffer, 9, "EMO/%hhu", p_emote );
        IO::loadSpriteB( buffer, SPR_EXCLM_OAM, SPR_EXCLM_GFX,
                         IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].x
                             + ( spr.m_sprite.getData( ).m_width == 32 ? 8 : 0 ),
                         IO::OamTop->oamBuffer[ _oamPosition[ p_spriteId ] ].y - 8, 16, 16, false,
                         false, false, OBJPRIORITY_0, false );
        update( );
    }

    void mapSpriteManager::hideExclamation( ) {
        IO::OamTop->oamBuffer[ _oamPosition[ SPR_EXCLM_OAM ] ].isHidden = true;
        update( );
    }
} // namespace MAP
