/*
Pokémon neo
------------------------------

file        : mapDrawer.cpp
author      : Philip Wellnitz
description : Map drawing engine: map object related functions

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

#include <algorithm>

#include "bag/bagViewer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/abilityNames.h"
#include "io/choiceBox.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/gameStart.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace MAP {
#define SPR_PKMN_OAM 100
#define SPR_CIRC_OAM 104

#define SPR_PKMN_GFX 303
#define SPR_CIRC_GFX 447

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

    bool mapDrawer::loadMapObject( std::pair<u8, mapObject>& p_mapObject ) {
        if( !p_mapObject.second.valid( ) ) { return false; }

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

            // check for reflection
            u8 behave = at( p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY + 1 )
                            .m_bottombehave;
            if( isReflective( behave ) ) { _mapSprites.enableReflection( p_mapObject.first ); }
            behave = at( p_mapObject.second.m_pos.m_posX, p_mapObject.second.m_pos.m_posY + 2 )
                         .m_bottombehave;
            if( isReflective( behave ) ) { _mapSprites.enableReflection( p_mapObject.first ); }

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
        return true;
    }

    void mapDrawer::attachMapObjectToPlayer( u8 p_objectId ) {
        removeAttachedObjects( );
        SAVE::SAV.getActiveFile( ).m_objectAttached    = 1;
        SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx = fixMapObject( p_objectId );
    }

    void mapDrawer::removeAttachedObjects( ) {
        if( SAVE::SAV.getActiveFile( ).m_objectAttached ) {
            SAVE::SAV.getActiveFile( ).m_mapObjAttachedIdx = 0;
            SAVE::SAV.getActiveFile( ).m_objectAttached    = 0;
            unfixMapObject( );
        }
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

    u8 mapDrawer::fixMapObject( u8 p_objectId ) {
        // swap mo to position 0
        std::swap( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ],
                   SAVE::SAV.getActiveFile( ).m_mapObjects[ _fixedObjectCount ] );
        return _fixedObjectCount++;
    }

    void mapDrawer::unfixMapObject( ) {
        _fixedObjectCount = 0;
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

            // check for reflection
            u8 behave = at( p_mapObject.m_pos.m_posX + dir[ p_movement.m_direction ][ 0 ],
                            p_mapObject.m_pos.m_posY + dir[ p_movement.m_direction ][ 1 ] + 1 )
                            .m_bottombehave;
            if( isReflective( behave ) ) { _mapSprites.enableReflection( p_spriteId ); }
            behave = at( p_mapObject.m_pos.m_posX + dir[ p_movement.m_direction ][ 0 ],
                         p_mapObject.m_pos.m_posY + dir[ p_movement.m_direction ][ 1 ] + 2 )
                         .m_bottombehave;
            if( isReflective( behave ) ) { _mapSprites.enableReflection( p_spriteId ); }
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

            bool refl = false;
            // check for reflection
            u8 behave = at( p_mapObject.m_pos.m_posX, p_mapObject.m_pos.m_posY + 1 ).m_bottombehave;
            if( isReflective( behave ) ) {
                _mapSprites.enableReflection( p_spriteId );
                refl = true;
            }
            behave = at( p_mapObject.m_pos.m_posX, p_mapObject.m_pos.m_posY + 2 ).m_bottombehave;
            if( isReflective( behave ) ) {
                _mapSprites.enableReflection( p_spriteId );
                refl = true;
            }

            if( !refl ) { _mapSprites.disableReflection( p_spriteId ); }
        }
    }

    void mapDrawer::moveMapObject( u8 p_objectId, movement p_movement, bool p_movePlayer,
                                   direction p_playerMovement, bool p_adjustAnim ) {
        moveMapObject( SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].second,
                       SAVE::SAV.getActiveFile( ).m_mapObjects[ p_objectId ].first, p_movement,
                       p_movePlayer, p_playerMovement, p_adjustAnim );
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
            snprintf( buf, 99, "%02d/%hu%s", p_pkmnId / ITEMS_PER_DIR, p_pkmnId,
                      p_shiny ? "s" : "" );
        } else {
            snprintf( buf, 99, "%02d/%hu%s_%hhu", p_pkmnId / ITEMS_PER_DIR, p_pkmnId,
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
        FS::getPkmnData( species, forme, _followPkmnSpeciesData );

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
            if( loadMapObject( cur ) ) {
                _playerFollowPkmnSprite = cur.first;
                _mapSprites.setFrameD( _playerFollowPkmnSprite, p_direction );
                _pkmnFollowsPlayer = true;
            } else {
                _pkmnFollowsPlayer = false;
                removeFollowPkmn( );
            }
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

            u16 ox = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX - dir[ _lastPlayerMove ][ 0 ];
            u16 oy = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY - dir[ _lastPlayerMove ][ 1 ];
            stepOff( ox, oy );
        }
    }

    void mapDrawer::showPkmn( const pkmnSpriteInfo& p_pkmn, bool p_cry ) {
        _mapSprites.setVisibility( _playerSprite, true, false );
        IO::loadSpriteB( "UI/cc", SPR_CIRC_OAM, SPR_CIRC_GFX, 64, 32, 64, 64, false, false, false,
                         OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 1, SPR_CIRC_GFX, 128, 32, 64, 64, 0, 0, 0, false, true,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 2, SPR_CIRC_GFX, 64, 96, 64, 64, 0, 0, 0, true, false,
                         false, OBJPRIORITY_1, false );
        IO::loadSpriteB( SPR_CIRC_OAM + 3, SPR_CIRC_GFX, 128, 96, 64, 64, 0, 0, 0, true, true,
                         false, OBJPRIORITY_1, false );

        if( p_cry ) { SOUND::playCry( p_pkmn.m_pkmnIdx, p_pkmn.m_forme ); }

        IO::loadPKMNSpriteB( p_pkmn, 80, 48, SPR_PKMN_OAM, SPR_PKMN_GFX, false );
        IO::updateOAM( false );
        for( u8 i = 0; i < 75; ++i ) swiWaitForVBlank( );

        for( u8 i = 0; i < 4; ++i ) {
            IO::OamTop->oamBuffer[ SPR_CIRC_OAM + i ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_PKMN_OAM + i ].isHidden = true;
        }
        IO::updateOAM( false );
    }

    void mapDrawer::usePkmn( const pkmnSpriteInfo& p_pkmn ) {
        u8 basePic = SAVE::SAV.getActiveFile( ).m_player.m_picNum / 10 * 10;
        SAVE::SAV.getActiveFile( ).m_player.m_picNum = basePic + 5;
        auto mmode = SAVE::SAV.getActiveFile( ).m_player.m_movement;

        u16 curx      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury      = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        _playerSprite = _mapSprites.loadSprite( curx, cury, mapSpriteManager::SPTYPE_PLAYER,
                                                SAVE::SAV.getActiveFile( ).m_player.sprite( ) );
        for( u8 i = 0; i < 5; ++i ) {
            _mapSprites.drawFrame( _playerSprite, i, false, true );
            for( u8 j = 0; j < 5; ++j ) swiWaitForVBlank( );
        }

        showPkmn( p_pkmn, true );

        changeMoveMode( mmode );
        swiWaitForVBlank( );
    }

    direction mapDrawer::getFollowPkmnDirection( ) const {
        if( !_pkmnFollowsPlayer || _followPkmnData == nullptr ) { return UP; }

        return _followPkmn.m_direction;
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
        _followPkmn.m_direction = olddir;

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
            SOUND::playBGMOneshot( BGM_OS_BADGE );
        } else if( p_type == 1 ) {
            SOUND::playBGMOneshot( BGM_OS_SYMBOL );
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
            IO::loadSpriteB( ( "BA/b" + std::to_string( p_badge ) ).c_str( ), SPR_PKMN_OAM,
                             SPR_PKMN_GFX, 96, 64, 64, 64, false, false, false, OBJPRIORITY_0,
                             false );
            SAVE::SAV.getActiveFile( ).m_lastAchievementEvent = p_badge;
            SAVE::SAV.getActiveFile( ).m_HOENN_Badges |= ( 1 << ( p_badge - 1 ) );
        } else if( p_type == 1 ) { // Frontier symbol
            IO::loadSpriteB( ( "BA/s" + std::to_string( p_badge ) ).c_str( ), SPR_PKMN_OAM,
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
        IO::printMessage( buffer, MSG_INFO );
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
                IO::buyItem( p_offeredItems, p_paymentMethod );
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
                IO::init( );
                ANIMATE_MAP = true;
            } else if( ( !p_allowItemSell && curMode == 1 ) || curMode == 2 ) {
                break;
            }
        }

        IO::init( );
    }

    void mapDrawer::resetMapSprites( ) {
        for( u16 i = 0; i < 20; ++i ) { swiWaitForVBlank( ); }
        for( u16 i = 0; i < SAVE::MAX_MAPOBJECT; ++i ) {
            SAVE::SAV.getActiveFile( ).m_mapObjects[ i ] = { UNUSED_MAPOBJECT, mapObject( ) };
        }
        SAVE::SAV.getActiveFile( ).m_mapObjectCount = 0;
        unfixMapObject( );
        _mapSprites.reset( );
    }

    void mapDrawer::constructAndAddNewMapObjects( MAP::mapData const& p_data, u8 p_mapX,
                                                  u8 p_mapY ) {
        bool oa     = ANIMATE_MAP;
        ANIMATE_MAP = false;
        std::vector<std::pair<u8, mapObject>> res;
        u16 curx = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16 cury = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;

        std::set<position> eventPositions;

#ifdef DESQUID_MORE
        IO::fadeScreen( IO::UNFADE );
        IO::printMessage( ( std::string( "constructAndAddNewMapObjects " )
                            + std::to_string( p_mapX ) + " " + std::to_string( p_mapY ) )
                              .c_str( ) );
#endif
        // check old objects and purge them if they are not visible anymore
        for( u8 i = _fixedObjectCount; i < SAVE::SAV.getActiveFile( ).m_mapObjectCount; ++i ) {
            auto o = SAVE::SAV.getActiveFile( ).m_mapObjects[ i ];
            if( o.first == UNUSED_MAPOBJECT ) { continue; }

            if( dist( o.second.m_pos.m_posX, o.second.m_pos.m_posY, curx, cury ) > 24 ) {
#ifdef DESQUID_MORE
                IO::printMessage(
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
        for( u8 i = 0; i < MAX_EVENTS_PER_SLICE; ++i ) {
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
                IO::printMessage( ( std::to_string( curx ) + "|" + std::to_string( cury ) + " : "
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

            if( loadMapObject( cur ) ) { res.push_back( cur ); }
        }

        SAVE::SAV.getActiveFile( ).m_mapObjectCount = res.size( ) + _fixedObjectCount;
        for( u8 i = 0; i < res.size( ); ++i ) {
            SAVE::SAV.getActiveFile( ).m_mapObjects[ i + _fixedObjectCount ] = res[ i ];
        }

        // force an update
        _mapSprites.update( );
        ANIMATE_MAP = oa;
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
} // namespace MAP
