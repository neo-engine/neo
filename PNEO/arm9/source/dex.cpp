/*
Pokémon neo
------------------------------

file        : dex.cpp
author      : Philip Wellnitz
description :

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

#include <cmath>

#include "defines.h"
#include "dex/dex.h"
#include "dex/dexUI.h"
#include "gen/pokemonFormes.h"
#include "io/choiceBox.h"
#include "io/uio.h"
#include "save/saveGame.h"
#include "sound/sound.h"

namespace DEX {
    u16 getFirstPkmnInNatDex( ) {
        for( u16 i = 1; i <= MAX_PKMN; ++i ) {
            if( SAVE::SAV.getActiveFile( ).seen( i ) || SAVE::SAV.getActiveFile( ).caught( i ) ) {
                return i;
            }
        }
        return 0;
    }

    u16 getLastPkmnInNatDex( ) {
        for( u16 i = MAX_PKMN; i; --i ) {
            if( SAVE::SAV.getActiveFile( ).seen( i ) || SAVE::SAV.getActiveFile( ).caught( i ) ) {
                return i;
            }
        }
        return 0;
    }

    u16 getFirstPkmnInLocDex( ) {
        for( u16 i = 0; i < LOCAL_DEX_SIZE; ++i ) {
            if( SAVE::SAV.getActiveFile( ).seen( LOCAL_DEX[ i ] )
                || SAVE::SAV.getActiveFile( ).caught( LOCAL_DEX[ i ] ) ) {
                return LOCAL_DEX[ i ];
            }
        }
        return 0;
    }

    u16 getLastPkmnInLocDex( ) {
        for( u16 i = 0; i < LOCAL_DEX_SIZE; ++i ) {
            if( SAVE::SAV.getActiveFile( ).seen( LOCAL_DEX[ LOCAL_DEX_SIZE - i - 1 ] )
                || SAVE::SAV.getActiveFile( ).caught( LOCAL_DEX[ LOCAL_DEX_SIZE - i - 1 ] ) ) {
                return LOCAL_DEX[ LOCAL_DEX_SIZE - i - 1 ];
            }
        }
        return 0;
    }

    dex::dex( ) {
        _dexUI        = new dexUI( );
        _natDexUB     = getLastPkmnInNatDex( );
        _natDexLB     = getFirstPkmnInNatDex( );
        _locDexUB     = getLastPkmnInLocDex( );
        _locDexLB     = getFirstPkmnInLocDex( );
        _locFirstPage = getLocSlotForNat( _locDexLB );
        _locLastPage  = getLocSlotForNat( _locDexUB );
    }

    void dex::changeMode( mode p_newMode, u16 p_startPkmn ) {
        _mode = p_newMode;
        _dexUI->changeMode( _mode );

        if( !p_startPkmn ) [[unlikely]] {
            if( _mode == NATIONAL_DEX ) {
                if( !SAVE::SAV.getActiveFile( ).m_lstDex ) {
                    SAVE::SAV.getActiveFile( ).m_lstDex = _natDexLB;
                }
                selectNational( SAVE::SAV.getActiveFile( ).m_lstDex, true );
            } else if( _mode == LOCAL_DEX ) {
                if( !SAVE::SAV.getActiveFile( ).m_lstLocalDexPage ) [[unlikely]] {
                    SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = _locFirstPage.first;
                    SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = _locFirstPage.second;
                }
                selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                             SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, 0, true );
            }
            return;
        }

        if( _mode == NATIONAL_DEX ) {
            if( !p_startPkmn || p_startPkmn > _natDexUB ) { p_startPkmn = _natDexLB; }
            selectNational( p_startPkmn, true );
            return;
        } else if( _mode == LOCAL_DEX ) {
            auto sl = getLocSlotForNat( p_startPkmn );
            if( sl.first && sl.second < MAX_LOCAL_DEX_SLOTS ) {
                selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = sl.first,
                             SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = sl.second, 0, true );
                return;
            }

            SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = _locFirstPage.first;
            SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = _locFirstPage.second;
            selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                         SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, 0, true );
            return;
        }
    }

    void dex::selectNational( u16 p_idx, bool p_forceDraw, u8 p_forme, bool p_shiny,
                              bool p_female ) {
        if( !p_idx || p_idx > MAX_PKMN ) [[unlikely]] {
            // out of bounds
            return;
        }

        if( !p_forceDraw && SAVE::SAV.getActiveFile( ).m_lstDex == p_idx ) { return; }

        if( SAVE::SAV.getActiveFile( ).seen( p_idx ) ) { SOUND::playCry( p_idx, 0, false ); }

        SAVE::SAV.getActiveFile( ).m_lstDex = p_idx;
        if( formeIdx( p_idx, p_forme ) == -1 ) { p_forme = _currentForme = 0; }
        _dexUI->nationalSelectIndex( SAVE::SAV.getActiveFile( ).m_lstDex, _natDexUB, true, p_forme,
                                     p_shiny, p_female );
    }

    u8 getNextEntryInRow( u16 p_row, u8 p_slot = 0 ) {
        for( u8 i = p_slot; i < MAX_LOCAL_DEX_SLOTS; ++i ) {
            if( LOCAL_DEX_PAGES[ p_row ][ i ] ) { return i; }
        }
        for( u8 i = p_slot; i; --i ) {
            if( LOCAL_DEX_PAGES[ p_row ][ i - 1 ] ) { return i - 1; }
        }
        return 255;
    }

    u8 getPrevEntryInRow( u16 p_row, u8 p_slot = 0 ) {
        for( u8 i = p_slot + 1; i; --i ) {
            if( LOCAL_DEX_PAGES[ p_row ][ i - 1 ] ) { return i - 1; }
        }
        for( u8 i = p_slot + 1; i < MAX_LOCAL_DEX_SLOTS; ++i ) {
            if( LOCAL_DEX_PAGES[ p_row ][ i ] ) { return i; }
        }
        return 255;
    }

    void dex::selectLocal( u16 p_page, u8 p_slot, s8 p_dir, bool p_forceDraw, u8 p_forme,
                           bool p_shiny, bool p_female ) {
        u16 oldpg = SAVE::SAV.getActiveFile( ).m_lstLocalDexPage;
        u8  oldsl = SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot;
        if( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage < 1 ) [[unlikely]] {
            // save uninitialized
            p_page = SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = 1;
            p_slot = SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = 0;
        }
        if( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage > MAX_LOCAL_DEX_PAGES - 4 ) [[unlikely]] {
            // save uninitialized
            p_page = SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = MAX_LOCAL_DEX_PAGES - 4;
            p_slot = SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = 1;
        }
        if( p_page < 1 || p_page > MAX_LOCAL_DEX_PAGES - 4 ) [[unlikely]] {
            // out of bounds
            return;
        }
        s16 dir = s16( p_page ) - s16( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage );
        if( dir * dir > 1 ) {
            dir         = 0;
            p_forceDraw = true;
        }

        while( p_page >= 1 && p_page <= _locLastPage.first ) {
            SAVE::SAV.getActiveFile( ).m_lstLocalDexPage += dir;
            u8 slot = p_slot;
            if( p_dir >= 0 ) {
                slot = getNextEntryInRow( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage, p_slot );
            } else {
                slot = getPrevEntryInRow( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage, p_slot );
            }
            if( slot < 255 ) {
                SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = slot;
                break;
            }
        }

        if( !p_forceDraw && SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot == oldsl
            && SAVE::SAV.getActiveFile( ).m_lstLocalDexPage == oldpg ) {
            // no need to redraw the stuff that is on the screen already
            return;
        }

        u16 pkmn = LOCAL_DEX_PAGES[ SAVE::SAV.getActiveFile( ).m_lstLocalDexPage ]
                                  [ SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot ];

        if( formeIdx( pkmn, p_forme ) == -1 ) { p_forme = _currentForme = 0; }

        if( SAVE::SAV.getActiveFile( ).seen( pkmn ) ) { SOUND::playCry( pkmn, p_forme, false ); }
        _dexUI->localSelectPageSlot( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot,
                                     _locLastPage.first, true, p_forme, p_shiny, p_female );
    }

    bool dex::runModeChoice( ) {
        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );

        auto res = cb.getResult(
            [ & ]( u8 ) {
                return _dexUI->drawModeChoice(
                    SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_DEX_OBTAINED ),
                    SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_NAT_DEX_OBTAINED ) );
            },
            [ & ]( u8 p_newSel ) { _dexUI->selectMode( p_newSel ); }, 0 );

        if( res == IO::choiceBox::EXIT_CHOICE || res == IO::choiceBox::BACK_CHOICE ) {
            return true;
        }
        _mode = mode( res );
        return false;
    }

    void dex::runDex( ) {
        _dexUI->init( );
        changeMode( _mode, 0 );
        cooldown = COOLDOWN_COUNT;
        u8 sl    = 255;

        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) || ( pressed & KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _mode == mode::NATIONAL_DEX ) {
                    // Next page
                    // TODO
                } else if( _mode == mode::LOCAL_DEX ) {
                    selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage + 1,
                                 SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, 1, false,
                                 _currentForme );
                }

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _mode == mode::NATIONAL_DEX ) {
                    // prev page
                    // TODO
                } else if( _mode == mode::LOCAL_DEX ) {
                    selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage - 1,
                                 SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, -1, false,
                                 _currentForme );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                if( _mode == mode::NATIONAL_DEX ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstDex < _natDexUB ) {
                        SOUND::playSoundEffect( SFX_SELECT );
                        selectNational( SAVE::SAV.getActiveFile( ).m_lstDex + 1 );
                    }
                } else if( _mode == mode::LOCAL_DEX ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot < MAX_LOCAL_DEX_SLOTS - 1
                        && ( sl = getNextEntryInRow( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot
                                                         + 1 ),
                             sl != 255 && sl > SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot ) ) {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot + 1, 1, false,
                                     _currentForme );
                    } else {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage + 1, 0, 1, false,
                                     _currentForme );
                    }
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                if( _mode == mode::NATIONAL_DEX ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstDex > 1 ) {
                        SOUND::playSoundEffect( SFX_SELECT );
                        selectNational( SAVE::SAV.getActiveFile( ).m_lstDex - 1 );
                    }
                } else if( _mode == mode::LOCAL_DEX ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot
                        && ( sl = getPrevEntryInRow( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot
                                                         - 1 ),
                             sl != 255 && sl < SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot ) ) {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot - 1, -1, false,
                                     _currentForme );
                    } else {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage - 1,
                                     MAX_LOCAL_DEX_SLOTS - 1, -1, false, _currentForme );
                    }
                }
                cooldown = COOLDOWN_COUNT;
            } /* else if( GET_KEY_COOLDOWN( KEY_L ) ) {
                // switch to prev page
                SOUND::playSoundEffect( SFX_SELECT );
                // TODO
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_R ) ) {
                // switch to next page
                SOUND::playSoundEffect( SFX_SELECT );
                // TODO
                cooldown = COOLDOWN_COUNT;
            } */
            else if( GET_KEY_COOLDOWN( KEY_SELECT ) ) {
                // switch mode local/national dex
                if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_DEX_OBTAINED )
                    && SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_NAT_DEX_OBTAINED ) ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( _mode == NATIONAL_DEX ) {
                        changeMode( mode::LOCAL_DEX, SAVE::SAV.getActiveFile( ).m_lstDex );
                    } else if( _mode == mode::LOCAL_DEX ) {
                        changeMode(
                            mode::NATIONAL_DEX,
                            LOCAL_DEX_PAGES[ SAVE::SAV.getActiveFile( ).m_lstLocalDexPage ]
                                           [ SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot ] );
                    }
                    cooldown = COOLDOWN_COUNT;
                }
            } else if( GET_KEY_COOLDOWN( KEY_Y ) || GET_KEY_COOLDOWN( KEY_A ) ) {
                // switch info on current page (next forme, etc)
                SOUND::playSoundEffect( SFX_SELECT );

                u16 pkmn = LOCAL_DEX_PAGES[ SAVE::SAV.getActiveFile( ).m_lstLocalDexPage ]
                                          [ SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot ];

                int nforme = _currentForme;
                if( formeIdx( pkmn, _currentForme + 1 ) != -1 ) {
                    nforme = _currentForme + 1;
                } else {
                    nforme = 0;
                }
                if( nforme != _currentForme ) {
                    _currentForme = nforme;
                    if( _mode == mode::NATIONAL_DEX ) {
                        selectNational( SAVE::SAV.getActiveFile( ).m_lstDex, true, _currentForme );
                    } else if( _mode == mode::LOCAL_DEX ) {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, 0, true,
                                     _currentForme );
                    }
                }

                cooldown = COOLDOWN_COUNT;
            }

            for( auto t : _dexUI->getTouchPositions( _mode ) ) {
                if( ( touch.px || touch.py ) && t.first.inRange( touch ) ) {
                    bool bad = false;
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
                        if( !t.second ) {
                            // Back
                            SOUND::playSoundEffect( SFX_CANCEL );
                            return;
                        }

                        if( _mode == NATIONAL_DEX ) {
                            SOUND::playSoundEffect( SFX_SELECT );
                            selectNational( t.second, true );
                            break;
                        } else if( _mode == LOCAL_DEX ) {
                            SOUND::playSoundEffect( SFX_SELECT );
                            auto slot = getLocSlotForNat( t.second );
                            SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = slot.first;
                            SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = slot.second;
                            selectLocal( slot.first, slot.second, 0, true, _currentForme );
                            break;
                        }
                    }
                }
            }

            swiWaitForVBlank( );
        }
    }

    void dex::run( ) {
        // run "welcome screen"/mode selection
        if( runModeChoice( ) ) { return; }

        // run actual dex
        runDex( );
    }

    void dex::run( u16 p_pkmnIdx, u8 p_forme, bool p_shiny, bool p_female ) {
        _mode = SHOW_SINGLE;
        _dexUI->init( );

        if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_DEX_OBTAINED )
            && SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_NAT_DEX_OBTAINED ) ) {
            SAVE::SAV.getActiveFile( ).m_lstDex = p_pkmnIdx;
            _dexUI->changeMode( NATIONAL_DEX );
            selectNational( SAVE::SAV.getActiveFile( ).m_lstDex, true, p_forme, p_shiny, p_female );

        } else {
            auto sl = getLocSlotForNat( p_pkmnIdx );

            SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = sl.first;
            SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = sl.second;
            _dexUI->changeMode( LOCAL_DEX );
            selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                         SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, 0, true, p_forme, p_shiny,
                         p_female );
        }

        cooldown = COOLDOWN_COUNT;

        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_X ) || ( pressed & KEY_B ) || ( pressed & KEY_A ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                return;
            }

            for( auto t : _dexUI->getTouchPositions( _mode ) ) {
                if( ( touch.px || touch.py ) && t.first.inRange( touch ) ) {
                    bool bad = false;
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
                        if( !t.second ) {
                            // Back
                            SOUND::playSoundEffect( SFX_CANCEL );
                            return;
                        }
                    }
                }
            }
        }
    }
} // namespace DEX
