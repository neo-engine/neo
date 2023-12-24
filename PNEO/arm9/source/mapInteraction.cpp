/*
Pokémon neo
------------------------------

file        : mapScript.cpp
author      : Philip Wellnitz
description : Map script engine

Copyright (C) 2012 - 2023
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
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "box/boxViewer.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/locationNames.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/uio.h"
#include "map/mapDrawer.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"

namespace MAP {
    void mapDrawer::interactFollowPkmn( ) {
        if( _followPkmnData == nullptr ) { return; }

        // rotate pkmn sprite to face player
        direction d = direction( ( SAVE::SAV.getActiveFile( ).m_player.m_direction + 2 ) % 4 );
        _mapSprites.setFrameD( _playerFollowPkmnSprite, d );

        // if follow pkmn is disguised, undisguise it
        if( _followPkmnIsDisguised ) {
            _followPkmnIsDisguised    = false;
            _followPkmnDisguiseBusted = true;
            updateFollowPkmn( );
            _followPkmnIsDisguised    = true;
            _followPkmnDisguiseBusted = false;
        }

        // play pkmn cry
        SOUND::playCry( _followPkmnData->getSpecies( ), _followPkmnData->getForme( ) );

        u8   emotion       = 0;
        char buffer[ 200 ] = { 0 };

        // first pick a random message type out of:
        //
        // 0 - special map location interaction (TODO)
        // 1  - item found (TODO)
        // 2 - general message
        // 3 - special map tile interaction (standing on sand/water/etc) (TODO)
        // 4 - special pkmn message (TODO)
        // 5 - hp / status condition message
        // 6 - special type interaction (some message specific to the pkmn's type) (TODO)
        // 7 - special weather interaction (TODO)
        // 8 - happiness message
        u8 msgtype = rand( ) % 9;

        switch( msgtype ) {
        case 0: {
            u8 msg = 0;
            switch( getCurrentLocationId( ) ) {
            case L_FIERY_DEN:
            case L_FIERY_PATH:
            case L_MT_PYRE:
            case L_TEAM_MAGMA_HIDEOUT:
            case L_SCORCHED_SLAB:
                if( _followPkmnSpeciesData ) {
                    if( _followPkmnSpeciesData->m_baseForme.m_types[ 0 ] == BATTLE::TYPE_FIRE
                        || _followPkmnSpeciesData->m_baseForme.m_types[ 1 ] == BATTLE::TYPE_FIRE ) {
                        msg     = 50 + ( rand( ) & 1 );
                        emotion = 2;
                        break;
                    }
                    if( _followPkmnSpeciesData->m_baseForme.m_types[ 0 ] == BATTLE::TYPE_WATER
                        || _followPkmnSpeciesData->m_baseForme.m_types[ 1 ]
                               == BATTLE::TYPE_WATER ) {
                        msg     = 52;
                        emotion = 0;
                        break;
                    }
                    if( _followPkmnSpeciesData->m_baseForme.m_types[ 0 ] == BATTLE::TYPE_ICE
                        || _followPkmnSpeciesData->m_baseForme.m_types[ 1 ] == BATTLE::TYPE_ICE ) {
                        msg     = 52 + ( rand( ) & 1 );
                        emotion = 8;
                        break;
                    }
                }
            default: break;
            }
            if( msg ) {
                snprintf( buffer, PKMNPHRS_LEN, FS::getPkmnPhrase( msg ),
                          _followPkmnData->m_boxdata.m_name );
                break;
            }

            [[fallthrough]];
        }
        case 1:
        case 2:
        default: {
            emotion = rand( ) % 12;
            while( emotion == 10 ) { emotion = rand( ) % 12; } // ignore poison
            u8 msg = rand( ) % 3;
            snprintf( buffer, PKMNPHRS_LEN, FS::getPkmnPhrase( 3 * emotion + msg ),
                      _followPkmnData->m_boxdata.m_name );
            break;
        }

        case 3:
        case 4:
        case 5: {
            u8 msg = 0;
            if( _followPkmnData->m_statusint ) {
                if( _followPkmnData->m_status.m_isPoisoned
                    || _followPkmnData->m_status.m_isBadlyPoisoned ) {
                    emotion = 10;
                    msg     = 36;
                } else if( _followPkmnData->m_status.m_isAsleep ) {
                    emotion = 1;
                    msg     = 37;
                } else if( _followPkmnData->m_status.m_isBurned ) {
                    emotion = 11;
                    msg     = 38;
                } else if( _followPkmnData->m_status.m_isFrozen ) {
                    emotion = 4;
                    msg     = 39;
                } else if( _followPkmnData->m_status.m_isParalyzed ) {
                    emotion = 11;
                    msg     = 40;
                }
            } else {
                if( _followPkmnData->m_stats.m_curHP * 8 < _followPkmnData->m_stats.m_maxHP ) {
                    emotion = 11;
                    msg     = 41;
                } else if( _followPkmnData->m_stats.m_curHP * 4
                           < _followPkmnData->m_stats.m_maxHP ) {
                    emotion = 11;
                    msg     = 42;
                } else if( _followPkmnData->m_stats.m_curHP * 2
                           < _followPkmnData->m_stats.m_maxHP ) {
                    emotion = 11;
                    msg     = 40;
                } else if( _followPkmnData->m_stats.m_curHP * 4
                           < _followPkmnData->m_stats.m_maxHP * 3 ) {
                    emotion = 3;
                    msg     = 43;
                } else {
                    emotion = 7;
                    msg     = 44;
                }
            }
            snprintf( buffer, PKMNPHRS_LEN, FS::getPkmnPhrase( msg ),
                      _followPkmnData->m_boxdata.m_name );
            break;
        }

        case 6:
        case 7:
        case 8: {
            u8 msg = 0;
            // happiness
            if( _followPkmnData->m_boxdata.m_steps < 60 ) {
                emotion = 8;
                msg     = 45;
            } else if( _followPkmnData->m_boxdata.m_steps < 120 ) {
                emotion = 1;
                msg     = 46;
            } else if( _followPkmnData->m_boxdata.m_steps < 185 ) {
                emotion = 3;
                msg     = 47;
            } else if( _followPkmnData->m_boxdata.m_steps < 250 ) {
                emotion = 7;
                msg     = 48;
            } else {
                emotion = 6;
                msg     = 49;
            }
            snprintf( buffer, PKMNPHRS_LEN, FS::getPkmnPhrase( msg ),
                      _followPkmnData->m_boxdata.m_name );
            break;
        }
        }

        if( emotion ) {
            // Show the emotion icon
            _mapSprites.showExclamation( _playerFollowPkmnSprite, 2 * emotion );
            for( u8 i = 0; i < 15; ++i ) { swiWaitForVBlank( ); }
            _mapSprites.showExclamation( _playerFollowPkmnSprite, 2 * emotion + 1 );
            for( u8 i = 0; i < 15; ++i ) { swiWaitForVBlank( ); }
            _mapSprites.hideExclamation( );
        }

        // print message
        printMapMessage( buffer, style( 0 ) );
    }

    void mapDrawer::interact( ) {
        u16  px = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posX;
        u16  py = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posY;
        u16  pz = SAVE::SAV.getActiveFile( ).m_player.m_pos.m_posZ;
        auto d  = SAVE::SAV.getActiveFile( ).m_player.m_direction;
        u16  tx = px + dir[ d ][ 0 ];
        u16  ty = py + dir[ d ][ 1 ];

        // Check for follow pkmn
        if( _pkmnFollowsPlayer && _followPkmn.m_pos.m_posX == tx
            && _followPkmn.m_pos.m_posY == ty ) {
            interactFollowPkmn( );
            return;
        }

        // Check for special blocks

        u8 behave = at( tx, ty ).m_bottombehave;

        switch( behave ) {
        case BEH_INTERACT_PC: { // PC
            SOUND::playSoundEffect( SFX_PC_OPEN );
            atom( tx, ty ).m_blockidx = 0x5;
            loadBlock( at( tx, ty ), ( _lastcol + NUM_COLS / 2 ) % NUM_COLS,
                       ( _lastrow + NUM_ROWS / 2 ) % NUM_ROWS );
            bgUpdate( );
            for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }
            IO::printMessage( GET_STRING( 559 ), MSG_INFO );

            BOX::boxViewer bxv;
            ANIMATE_MAP = false;
            videoSetMode( MODE_5_2D );
            bgUpdate( );
            SOUND::dimVolume( );

            bxv.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            IO::initVideoSub( );
            IO::resetScale( true, false );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            IO::init( );
            draw( );

            SOUND::playSoundEffect( SFX_PC_CLOSE );
            atom( tx, ty ).m_blockidx = 0x4;
            loadBlock( at( tx, ty ), ( _lastcol + NUM_COLS / 2 ) % NUM_COLS,
                       ( _lastrow + NUM_ROWS / 2 ) % NUM_ROWS );
            bgUpdate( );
            for( u8 i = 0; i < 3; ++i ) { swiWaitForVBlank( ); }

            return;
        }
        case BEH_INTERACT_MAP: { // Map
            // TODO
            printMapMessage( GET_STRING( 560 ), MSG_NORMAL );
            return;
        }
        case BEH_INTERACT_TV: { // TV
            printMapMessage( GET_MAP_STRING( 26 ), MSG_NORMAL );
            return;
        }

        case BEH_INTERACT_S133: { // picture books
            printMapMessage( GET_MAP_STRING( 133 ), MSG_NORMAL );
            break;
        }
        case BEH_INTERACT_S134: { // picture books
            printMapMessage( GET_MAP_STRING( 134 ), MSG_NORMAL );
            break;
        }
        case BEH_INTERACT_S30: { // PokeCenter magazines
            printMapMessage( GET_MAP_STRING( 30 ), MSG_NORMAL );
            break;
        }
        case BEH_INTERACT_S710: { // Slateport empty vase
            printMapMessage( GET_MAP_STRING( 710 ), MSG_NORMAL );
            break;
        }
        case BEH_INTERACT_TRASH: { // trash bin is empty
            if( !currentData( ).hasEvent( EVENT_ITEM, px, py, pz ) ) {
                printMapMessage( GET_MAP_STRING( 404 ), MSG_NORMAL );
            }
            break;
        }
        case BEH_INTERACT_S127: { // Pokemart shelves
            printMapMessage( GET_MAP_STRING( 127 ), MSG_NORMAL );
            break;
        }
        case BEH_INTERACT_S396: { // blue prints
            printMapMessage( GET_MAP_STRING( 396 ), MSG_NORMAL );
            break;
        }
        case BEH_INTERACT_BEHIND: { // load script one block behind
            handleEvents( tx, ty, pz, d );
            return;
        }
            [[likely]] default : break;
        }
        handleEvents( px, py, pz, d );
    }

} // namespace MAP
