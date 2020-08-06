/*
Pokémon neo
------------------------------

file        : mapSprite.cpp
author      : Philip Wellnitz
description : Map Sprites.

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

#include "mapSprite.h"
#include "fs.h"
#include "nav.h"
#include "saveGame.h"
#include "uio.h"

#define SPR_MAIN_PLAYER_OAM        0
#define SPR_MAIN_PLAYER_PLAT_OAM   1
#define SPR_SMALL_NPC_OAM( p_idx ) ( 2 + ( p_idx ) )
#define SPR_LARGE_NPC_OAM( p_idx ) ( SPR_SMALL_NPC_OAM( MAX_SMALL_NPC ) + ( p_idx ) )
#define SPR_HM_OAM( p_idx )        ( SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) + ( p_idx ) )

// Strength boulder (16x16)
// Rock Smash rock  (16x16)
// Cut tree         (16x16)
// Map animation (rustling grass, etc)

#define SPR_MAIN_PLAYER_GFX        0
#define SPR_MAIN_PLAYER_PLAT_GFX   16
#define SPR_SMALL_NPC_GFX( p_idx ) ( 32 + 8 * ( p_idx ) )
#define SPR_LARGE_NPC_GFX( p_idx ) ( SPR_SMALL_NPC_GFX( MAX_SMALL_NPC ) + 16 * ( p_idx ) )
#define SPR_HM_GFX( p_idx )        ( SPR_LARGE_NPC_GFX( MAX_LARGE_NPC ) + 4 * ( p_idx ) )

namespace MAP {
    mapSpriteData::mapSpriteData( u16 p_imageId ) {
        FILE* f;
        if( p_imageId < 250 ) {
            f = FS::open( IO::OW_PATH, p_imageId, ".rsd" );
        } else {
            if( p_imageId == 250 ) {
                // load player appearance
                p_imageId = SAVE::SAV.getActiveFile( ).m_appearance;
            } else if( p_imageId == 251 ) {
                // load rival's appearance
                p_imageId = SAVE::SAV.getActiveFile( ).getVar( SAVE::V_RIVAL_APPEARANCE );

            } else {
                p_imageId &= 255;
            }
            f = FS::open( IO::TRAINER_PATH, p_imageId, ".rsd" );
        }

        if( !f ) {
#ifdef DESQUID
            NAV::printMessage( "Sprite failed" );
#endif
        } else {
            FS::read( f, m_palData, sizeof( u16 ), 16 );
            FS::read( f, &m_frameCount, sizeof( u8 ), 1 );
            FS::read( f, &m_width, sizeof( u8 ), 1 );
            FS::read( f, &m_height, sizeof( u8 ), 1 );
            FS::read( f, m_frameData, sizeof( u32 ), m_width * m_height * m_frameCount / 8 );
            FS::close( f );
        }
    }

    mapSprite::mapSprite( u16 p_imageId, u8 p_startFrame ) {
        _data            = mapSpriteData( p_imageId );
        _info.m_picNum   = p_imageId;
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

    void mapSprite::drawFrame( u8 p_oamIdx, u8 p_value, bool p_hFlip ) {
        IO::setOWSpriteFrame( p_value, p_hFlip, p_oamIdx, _data.m_palData, _data.m_frameData );
    }

    void mapSprite::setFrame( u8 p_oamIdx, u8 p_value ) {
        _info.m_curFrame = p_value;
        drawFrame( p_oamIdx, _info.m_curFrame );
    }

    void mapSprite::currentFrame( u8 p_oamIdx ) {
        drawFrame( p_oamIdx, _info.m_curFrame );
    }
    void mapSprite::nextFrame( u8 p_oamIdx ) {
        if( ( ( ++_info.m_curFrame ) % PLAYER_FAST ) % 3 == 0 ) _info.m_curFrame -= 2;
        drawFrame( p_oamIdx, _info.m_curFrame );
    }

    void mapSpriteManager::init( ) {
        // pre load item, hm sprites
        _itemBallData  = mapSpriteData( 256 | 249 );
        _hmBallData    = mapSpriteData( 256 | 250 );
        _strengthData  = mapSpriteData( 256 | 251 );
        _rockSmashData = mapSpriteData( 256 | 252 );
        _cutData       = mapSpriteData( 256 | 253 );

        _playerPlatform.m_sprite = mapSprite( { 256 | 248, 0 }, mapSpriteData( 256 | 248 ) );
    }

    void mapSpriteManager::reset( ) {
        IO::initOAMTable( false );

        destroySprite( SPR_MAIN_PLAYER_OAM, false );
        destroySprite( SPR_MAIN_PLAYER_PLAT_OAM, false );
        for( u8 i = 0; i < MAX_SMALL_NPC; ++i ) { destroySprite( SPR_SMALL_NPC_OAM( i ), false ); }
        for( u8 i = 0; i < MAX_LARGE_NPC; ++i ) { destroySprite( SPR_LARGE_NPC_OAM( i ), false ); }
        for( u8 i = 0; i < MAX_HM_PARTICLE; ++i ) { destroySprite( SPR_HM_OAM( i ), false ); }
        update( );
    }

    /*
     * @brief: Loads the specified sprite at the specified position on the screen.
     */
    void doLoadSprite( u16 p_posX, u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       const mapSpriteData& p_data ) {
        IO::loadSpriteB(
            p_oamIdx, p_tileCnt, p_posX, p_posY, p_data.m_width, p_data.m_height, p_data.m_palData,
            reinterpret_cast<const unsigned int*>( p_data.m_frameData ),
            p_data.m_width * p_data.m_height / 2, false, false, false, OBJPRIORITY_2, false );
    }

    void doLoadSprite( u16 p_posX, u16 p_posY, u8 p_oamIdx, u16 p_tileCnt,
                       const mapSprite& p_sprite ) {
        auto& data = p_sprite.getData( );
        doLoadSprite( p_posX, p_posY, p_oamIdx, p_tileCnt, data );
    }

    const mapSpriteManager::managedSprite&
    mapSpriteManager::getManagedSprite( u8 p_spriteId ) const {
        if( p_spriteId == SPR_MAIN_PLAYER_OAM ) {
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
        if( p_spriteId == SPR_MAIN_PLAYER_OAM ) {
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
        return getManagedSprite( p_spriteId ).m_sprite.getData( );
    }

    u8 mapSpriteManager::loadSprite( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY,
                                     spriteType p_type, const mapSprite& p_sprite ) {

        switch( p_type ) {
        case SPTYPE_PLAYER:
            _player = { p_sprite, { p_posX, p_posY, 0, 0, 0, 0 } };
            doLoadSprite( screenX( p_camX, p_posX, p_sprite.getData( ).m_width ),
                          screenY( p_camY, p_posY, p_sprite.getData( ).m_height ),
                          SPR_MAIN_PLAYER_OAM, SPR_MAIN_PLAYER_GFX, p_sprite );
            return SPR_MAIN_PLAYER_OAM;
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
                                           camShift( p_camY, p_posY ) } } };
                doLoadSprite( screenX( p_camX, p_posX, 32 ), screenY( p_camY, p_posY, 32 ),
                              SPR_LARGE_NPC_OAM( freesp ), SPR_LARGE_NPC_GFX( freesp ), p_sprite );
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
                                             camShift( p_camY, p_posY ) } } };
                doLoadSprite( screenX( p_camX, p_posX, 32 ), screenY( p_camY, p_posY, 32 ),
                              SPR_SMALL_NPC_OAM( freesp ), SPR_SMALL_NPC_GFX( freesp ), p_sprite );
                return SPR_SMALL_NPC_OAM( freesp );
            }
        }
        default: break;
        }

        return 255;
    }

    u8 mapSpriteManager::loadSprite( u16 p_camX, u16 p_camY, u16 p_posX, u16 p_posY,
                                     u8 p_particleId ) {

        u8 nextfree = 255;
        for( u8 i = 0; i < MAX_HM_PARTICLE; ++i ) {
            if( !_hmSpriteInfo[ i ].first ) {
                nextfree = i;
                break;
            }
        }

        if( p_particleId != SPR_PLATFORM && nextfree == 255 ) {
            // No space for an additional particle
            return 255;
        } else if( p_particleId != SPR_PLATFORM ) {
            _hmSpriteInfo[ nextfree ] = {
                p_particleId,
                { p_posX, p_posY, 0, 0, camShift( p_camX, p_posX ), camShift( p_camY, p_posY ) } };
        }

        switch( p_particleId ) {
        case SPR_ITEM:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          SPR_HM_OAM( nextfree ), SPR_HM_GFX( p_particleId ), _itemBallData );
            break;
        case SPR_HMBALL:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          SPR_HM_OAM( nextfree ), SPR_HM_GFX( p_particleId ), _hmBallData );
            break;
        case SPR_STRENGTH:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          SPR_HM_OAM( nextfree ), SPR_HM_GFX( p_particleId ), _strengthData );
            break;
        case SPR_ROCKSMASH:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          SPR_HM_OAM( nextfree ), SPR_HM_GFX( p_particleId ), _rockSmashData );
            break;
        case SPR_CUT:
            doLoadSprite( screenX( p_camX, p_posX, 16 ), screenY( p_camY, p_posY, 16 ),
                          SPR_HM_OAM( nextfree ), SPR_HM_GFX( p_particleId ), _cutData );
            break;
        case SPR_PLATFORM:
            _playerPlatform.m_pos = { p_posX, p_posY, 0, 0, 0, 0 };
            doLoadSprite( screenX( p_camX, p_posX, 32 ), screenY( p_camY, p_posY, 32 ) + 3,
                          SPR_MAIN_PLAYER_PLAT_OAM, SPR_MAIN_PLAYER_PLAT_GFX,
                          _playerPlatform.m_sprite );
            return SPR_MAIN_PLAYER_PLAT_OAM;
        default: break;
        }

        return 255;
    }

    void mapSpriteManager::destroySprite( u8 p_spriteId, bool p_update ) {
        IO::OamTop->oamBuffer[ p_spriteId ].isHidden = true;
        if( p_spriteId >= SPR_SMALL_NPC_OAM( 0 )
            && p_spriteId < SPR_SMALL_NPC_OAM( MAX_SMALL_NPC ) ) {
            _smallNpcs[ p_spriteId - SPR_SMALL_NPC_OAM( 0 ) ].first = false;
        } else if( p_spriteId >= SPR_LARGE_NPC_OAM( 0 )
                   && p_spriteId < SPR_LARGE_NPC_OAM( MAX_LARGE_NPC ) ) {
            _bigNpcs[ p_spriteId - SPR_LARGE_NPC_OAM( 0 ) ].first = false;
        } else if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].first = SPR_UNUSED;
        }
        if( p_update ) { IO::updateOAM( false ); }
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

        update( );
    }

    void mapSpriteManager::moveSprite( u8 p_spriteId, direction p_direction, s16 p_amount,
                                       bool p_update ) {
        moveSprite( p_spriteId, p_amount * dir[ p_direction ][ 0 ],
                    p_amount * dir[ p_direction ][ 1 ], p_update );
    }

    void mapSpriteManager::moveSprite( u8 p_spriteId, s8 p_dx, s8 p_dy, bool p_update ) {
        IO::OamTop->oamBuffer[ p_spriteId ].x += p_dx;
        IO::OamTop->oamBuffer[ p_spriteId ].y += p_dy;

        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.moveSprite( p_dx, p_dy );
        } else {
            getManagedSprite( p_spriteId ).m_pos.moveSprite( p_dx, p_dy );
        }

        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::translateSprite( u8 p_spriteId, s8 p_dx, s8 p_dy, bool p_update ) {
        IO::OamTop->oamBuffer[ p_spriteId ].x += p_dx;
        IO::OamTop->oamBuffer[ p_spriteId ].y += p_dy;

        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) ) {
            _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.translateSprite( p_dx, p_dy );
            IO::OamTop->oamBuffer[ p_spriteId ].isHidden
                = !_hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.isVisible( );

#ifdef DESQUID_MORE
            NAV::printMessage(
                ( std::to_string( p_spriteId )
                  + " x: " + std::to_string( IO::OamTop->oamBuffer[ p_spriteId ].x )
                  + " y : " + std::to_string( IO::OamTop->oamBuffer[ p_spriteId ].y ) + " cam x : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_camDisX )
                  + " cam y : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_camDisY )
                  + "\npos x : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_curX )
                  + " pos y : "
                  + std::to_string( _hmSpriteInfo[ p_spriteId - SPR_HM_OAM( 0 ) ].second.m_curY ) )
                    .c_str( ) );
#endif

        } else {
            getManagedSprite( p_spriteId ).m_pos.translateSprite( p_dx, p_dy );
            IO::OamTop->oamBuffer[ p_spriteId ].isHidden
                = !getManagedSprite( p_spriteId ).m_pos.isVisible( );
        }

        if( p_update ) { IO::updateOAM( false ); }
    }

    ObjPriority mapSpriteManager::getPriority( u8 p_spriteId ) const {
        return IO::OamTop->oamBuffer[ p_spriteId ].priority;
    }

    void mapSpriteManager::setVisibility( u8 p_spriteId, bool p_value, bool p_update ) {
        IO::OamTop->oamBuffer[ p_spriteId ].isHidden = p_value;
        if( p_update ) { IO::updateOAM( false ); }
    }

    bool mapSpriteManager::getVisibility( u8 p_spriteId ) {
        return IO::OamTop->oamBuffer[ p_spriteId ].isHidden;
    }

    void mapSpriteManager::setPriority( u8 p_spriteId, ObjPriority p_value, bool p_update ) {
        IO::OamTop->oamBuffer[ p_spriteId ].priority = p_value;
        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::drawFrame( u8 p_spriteId, u8 p_value, bool p_update ) {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
                return;
            }
        getManagedSprite( p_spriteId ).m_sprite.drawFrame( p_spriteId, p_value );
        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::drawFrame( u8 p_spriteId, u8 p_value, bool p_hflip, bool p_update ) {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
                return;
            }
        getManagedSprite( p_spriteId ).m_sprite.drawFrame( p_spriteId, p_value, p_hflip );
        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::setFrame( u8 p_spriteId, u8 p_value, bool p_update ) {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
                return;
            }
        getManagedSprite( p_spriteId ).m_sprite.setFrame( p_spriteId, p_value );
        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::currentFrame( u8 p_spriteId, bool p_update ) {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
                return;
            }
        getManagedSprite( p_spriteId ).m_sprite.currentFrame( p_spriteId );
        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::nextFrame( u8 p_spriteId, bool p_update ) {
        if( p_spriteId >= SPR_HM_OAM( 0 ) && p_spriteId < SPR_HM_OAM( MAX_HM_PARTICLE ) )
            [[unlikely]] {
                return;
            }
        getManagedSprite( p_spriteId ).m_sprite.nextFrame( p_spriteId );
        if( p_update ) { IO::updateOAM( false ); }
    }

    void mapSpriteManager::update( ) {
        IO::updateOAM( false );
    }
} // namespace MAP
