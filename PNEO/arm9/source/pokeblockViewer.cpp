/*
Pokémon neo
------------------------------

file        : pokeblockViewer.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2023 - 2023
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

#include <nds.h>

#include "bag/pokeblockViewer.h"
#include "fs/data.h"
#include "gen/itemNames.h"
#include "io/animations.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/strings.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace BAG {
    pokeblockViewer::pokeblockViewer( pokemon* p_playerTeam, u16* p_pokeblockCount ) {
        _playerTeam     = p_playerTeam;
        _pokeblockCount = p_pokeblockCount;
        _ui             = new pokeblockUI( _playerTeam );

        _selectedPkmn  = 0;
        _selectedBlock = 0;
        _view          = 0;
    }

    void pokeblockViewer::selectView( u8 p_view ) {
        _view = p_view;
        switch( _view ) {
        default:
        case 0:
            _ui->initBlockView( _pokeblockCount );
            _ui->selectBlock( _selectedBlock, _pokeblockCount[ _selectedBlock ] );
            break;
        case 1:
            _ui->initPkmnView( );
            _ui->selectPkmn( _selectedPkmn, _selectedBlock );
            break;
        case 2: {
            _playerTeam[ _selectedPkmn ].eatPokeblock( _selectedBlock );
            _pokeblockCount[ _selectedBlock ]--;
            _ui->animateFeedBlockToPkmn( _selectedPkmn, _selectedBlock );
            _ui->initPkmnView( );
            _ui->selectBlock( _selectedBlock, _pokeblockCount[ _selectedBlock ] );
            _ui->selectPkmn( _selectedPkmn, _selectedBlock );
            _view = 1;
            break;
        }
        }
    }

    void pokeblockViewer::selectBlock( u8 p_blockIdx ) {
        _selectedBlock = p_blockIdx;
        _ui->selectBlock( _selectedBlock, _pokeblockCount[ _selectedBlock ] );
    }

    void pokeblockViewer::selectPkmn( u8 p_pkmnIdx ) {
        _selectedPkmn = p_pkmnIdx;
        _ui->selectPkmn( _selectedPkmn, _selectedBlock );
    }

    u8 pokeblockViewer::handleTouch( ) {
        auto tpos = _ui->getTouchPositions( );

        bool bad = false;

        for( auto t : tpos ) {
            if( ( touch.px || touch.py ) && t.first.inRange( touch ) ) {
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    if( !t.first.inRange( touch ) ) {
                        bad = true;
                        break;
                    }
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                if( !bad ) {
                    if( !t.second ) { return 1; } // Back

                    if( t.second == 50 ) { // select
                        return 2;
                    }

                    if( t.second >= 100 && t.second < 150 ) { // select pokeblock
                        u8 res = 0;
                        if( _selectedBlock == t.second - 100 ) { res = 2; }
                        selectBlock( t.second - 100 );
                        return res;
                    }

                    if( t.second >= 150 ) { // select pkmn
                        u8 res = 0;
                        if( _view && _selectedPkmn == t.second - 150 ) { res = 2; }
                        if( !_view ) { selectView( _view + 1 ); }
                        selectPkmn( t.second - 150 );
                        return res;
                    }
                }
            }
        }
        return 0;
    }

    void pokeblockViewer::run( ) {
        _ui->init( );
        selectView( 0 );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            auto tch = handleTouch( );

            if( GET_AND_WAIT( KEY_X ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return;
            } else if( GET_AND_WAIT( KEY_B ) || tch == 1 ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                if( _view ) {
                    selectView( 0 );
                    cooldown = COOLDOWN_COUNT;
                } else {
                    return;
                }
            } else if( GET_AND_WAIT( KEY_A ) || tch == 2 ) {
                if( !_playerTeam[ 0 ].getSpecies( ) ) { continue; }
                if( _view
                    && ( !_pokeblockCount[ _selectedBlock ]
                         || !_playerTeam[ _selectedPkmn ].canEatPokeblock( _selectedBlock ) ) ) {
                    continue;
                }
                SOUND::playSoundEffect( SFX_CHOOSE );
                selectView( _view + 1 );
                cooldown = COOLDOWN_COUNT;
            } else if( !_view && GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                selectBlock( ( _selectedBlock + SAVE::POKEBLOCK_TYPES - 1 )
                             % SAVE::POKEBLOCK_TYPES );
                cooldown = COOLDOWN_COUNT;
            } else if( !_view && GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                selectBlock( ( _selectedBlock + 1 ) % SAVE::POKEBLOCK_TYPES );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( _view ) {
                    if( _selectedPkmn + 1 < 6 && _playerTeam[ _selectedPkmn + 1 ].getSpecies( ) ) {
                        selectPkmn( _selectedPkmn + 1 );
                    } else {
                        selectPkmn( 0 );
                    }
                } else {
                    selectBlock( ( _selectedBlock + BLOCKS_PER_ROW ) % SAVE::POKEBLOCK_TYPES );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( _view ) {
                    if( _selectedPkmn ) {
                        selectPkmn( _selectedPkmn - 1 );
                    } else {
                        u8 i = 0;
                        for( ; i < 6; ++i ) {
                            if( !_playerTeam[ i ].getSpecies( ) ) { break; }
                        }
                        if( i ) {
                            selectPkmn( i - 1 );
                        } else {
                            selectView( 0 );
                        }
                    }
                } else {
                    selectBlock( ( _selectedBlock + SAVE::POKEBLOCK_TYPES - BLOCKS_PER_ROW )
                                 % SAVE::POKEBLOCK_TYPES );
                }
                cooldown = COOLDOWN_COUNT;
            }
            swiWaitForVBlank( );
        }
        return;
    }
} // namespace BAG
