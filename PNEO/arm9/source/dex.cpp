/*
Pokémon neo
------------------------------

file        : dex.cpp
author      : Philip Wellnitz
description :

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

#include <cmath>

#include "choiceBox.h"
#include "defines.h"
#include "dex.h"
#include "dexUI.h"
#include "saveGame.h"
#include "sound.h"
#include "uio.h"

namespace DEX {
    dex::dex( ) {
        _dexUI = new dexUI( );
    }

    void dex::changeMode( mode p_newMode, u16 p_startPkmn ) {
        _mode = p_newMode;
        _dexUI->changeMode( _mode );

        if( !p_startPkmn ) [[unlikely]] {
            if( _mode == NATIONAL_DEX ) {
                if( !SAVE::SAV.getActiveFile( ).m_lstDex ) {
                    SAVE::SAV.getActiveFile( ).m_lstDex = 1;
                }
                selectNational( SAVE::SAV.getActiveFile( ).m_lstDex, true );
            } else if( _mode == LOCAL_DEX ) {
                selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                             SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot, true );
            }
            return;
        }

        if( _mode == NATIONAL_DEX ) {
            if( !p_startPkmn ) { p_startPkmn = 1; }
            selectNational( p_startPkmn, true );
            return;
        } else if( _mode == LOCAL_DEX ) {
            for( u16 i = 0; i < MAX_LOCAL_DEX_PAGES - 4; ++i ) {
                for( u8 j = 0; j < MAX_LOCAL_DEX_SLOTS; ++j ) {
                    if( LOCAL_DEX_PAGES[ i ][ j ] == p_startPkmn ) {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = i,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = j, true );
                        return;
                    }
                }
            }

            selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = 2,
                         SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = 0, true );
            return;
        }
    }

    void dex::selectNational( u16 p_idx, bool p_forceDraw ) {
        if( !p_idx || p_idx > MAX_PKMN ) [[unlikely]] {
            // out of bounds
            return;
        }

        if( !p_forceDraw && SAVE::SAV.getActiveFile( ).m_lstDex == p_idx ) { return; }

        if( SAVE::SAV.getActiveFile( ).seen( p_idx ) ) { SOUND::playCry( p_idx, 0, false ); }

        SAVE::SAV.getActiveFile( ).m_lstDex = p_idx;
        _dexUI->selectNationalIndex( SAVE::SAV.getActiveFile( ).m_lstDex );
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

    void dex::selectLocal( u16 p_page, u8 p_slot, bool p_forceDraw ) {
        u16 oldpg = SAVE::SAV.getActiveFile( ).m_lstLocalDexPage;
        u8  oldsl = SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot;
        if( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage < 2 ) [[unlikely]] {
            // save uninitialized
            p_page = SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = 2;
            p_slot = SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = 0;
        }
        if( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage > MAX_LOCAL_DEX_PAGES - 5 ) [[unlikely]] {
            // save uninitialized
            p_page = SAVE::SAV.getActiveFile( ).m_lstLocalDexPage = MAX_LOCAL_DEX_PAGES - 5;
            p_slot = SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot = 1;
        }
        if( p_page < 2 || p_page > MAX_LOCAL_DEX_PAGES - 5 ) [[unlikely]] {
            // out of bounds
            return;
        }
        s16 dir = s16( p_page ) - s16( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage );
        if( dir * dir > 1 ) {
            dir         = 0;
            p_forceDraw = true;
        }

        while( p_page >= 2 && p_page <= MAX_LOCAL_DEX_PAGES - 5 ) {
            SAVE::SAV.getActiveFile( ).m_lstLocalDexPage += dir;
            u8 slot = p_slot;
            if( dir >= 0 ) {
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
        if( SAVE::SAV.getActiveFile( ).seen( pkmn % ALOLAN_FORME ) ) {
            SOUND::playCry( pkmn % ALOLAN_FORME, pkmn > ALOLAN_FORME, false );
        }
        _dexUI->selectLocalPageSlot( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot );
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
                                 SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot );
                }

                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _mode == mode::NATIONAL_DEX ) {
                    // prev page
                    // TODO
                } else if( _mode == mode::LOCAL_DEX ) {
                    selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage - 1,
                                 SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot );
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                if( _mode == mode::NATIONAL_DEX ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstDex < MAX_PKMN ) {
                        SOUND::playSoundEffect( SFX_SELECT );
                        selectNational( SAVE::SAV.getActiveFile( ).m_lstDex + 1 );
                    }
                } else if( _mode == mode::LOCAL_DEX ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    if( SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot < MAX_LOCAL_DEX_SLOTS - 1
                        && LOCAL_DEX_PAGES[ SAVE::SAV.getActiveFile( ).m_lstLocalDexPage ]
                                          [ SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot + 1 ] ) {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot + 1 );
                    } else {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage + 1, 0 );
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
                        && LOCAL_DEX_PAGES[ SAVE::SAV.getActiveFile( ).m_lstLocalDexPage ]
                                          [ SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot - 1 ] ) {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage,
                                     SAVE::SAV.getActiveFile( ).m_lstLocalDexSlot - 1 );
                    } else {
                        selectLocal( SAVE::SAV.getActiveFile( ).m_lstLocalDexPage - 1,
                                     MAX_LOCAL_DEX_SLOTS - 1 );
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
                // TODO
                cooldown = COOLDOWN_COUNT;
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
        // TODO
    }
} // namespace DEX
