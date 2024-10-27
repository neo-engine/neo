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

#ifndef NO_SOUND
#include "fs/data.h"
#include "fs/fs.h"
#include "gen/pokemonNames.h"
#include "io/menuUI.h"
#include "io/navApp.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "sound/sound.h"
#include "sound/sseqData.h"

namespace IO {
    void jboxNavApp::drawIcon( u8 p_oamSlot, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        IO::loadSprite( "NV/app02", p_oamSlot, oam[ p_oamSlot ].palette, oam[ p_oamSlot ].gfxIndex,
                        oam[ p_oamSlot ].x, oam[ p_oamSlot ].y, 64, 64, false, false, false,
                        OBJPRIORITY_1, p_bottom );
    }

    void setSongChoiceVis( u8 p_idx, bool p_vis, bool p_bottom = true ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 j = 0; j < 5; ++j ) {
            auto idx            = SPR_NAV_APP_RSV_SUB + 7 + 5 * p_idx + j;
            oam[ idx ].isHidden = p_vis;
        }
    }

    void setSongChoicePal( u8 p_idx, u8 p_pal, bool p_bottom = true ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 j = 0; j < 5; ++j ) {
            auto idx           = SPR_NAV_APP_RSV_SUB + 7 + 5 * p_idx + j;
            oam[ idx ].palette = p_pal;
        }
    }

    void jboxNavApp::load( bool p_bottom ) {
        char buffer[ 100 ];

        auto ptr3 = !p_bottom ? bgGetGfxPtr( IO::bg3 ) : bgGetGfxPtr( IO::bg3sub );
        auto pal  = !p_bottom ? BG_PALETTE : BG_PALETTE_SUB;

        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        for( u8 i = 0; i < MAX_NAV_APPS; ++i ) { oam[ SPR_NAV_APP_ICON_SUB( i ) ].isHidden = true; }

        // next / prev page

        u16 tileCnt = 0;

        oam[ SPR_NAV_APP_RSV_SUB + 2 ]          = oam[ SPR_X_OAM_SUB ];
        oam[ SPR_NAV_APP_RSV_SUB + 2 ].isHidden = false;
        oam[ SPR_NAV_APP_RSV_SUB + 2 ].x        = 200;
        oam[ SPR_NAV_APP_RSV_SUB + 2 ].y        = 166;

        oam[ SPR_NAV_APP_RSV_SUB + 3 ] = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ];
        oam[ SPR_NAV_APP_RSV_SUB + 4 ] = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 7 ];
        oam[ SPR_NAV_APP_RSV_SUB + 5 ] = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) ];
        oam[ SPR_NAV_APP_RSV_SUB + 6 ] = oam[ SPR_CHOICE_START_OAM_SUB( 0 ) + 7 ];

        oam[ SPR_NAV_APP_RSV_SUB + 5 ].x = 180;
        oam[ SPR_NAV_APP_RSV_SUB + 3 ].x = 14;
        oam[ SPR_NAV_APP_RSV_SUB + 6 ].x = oam[ SPR_NAV_APP_RSV_SUB + 5 ].x + 16;
        oam[ SPR_NAV_APP_RSV_SUB + 4 ].x = oam[ SPR_NAV_APP_RSV_SUB + 3 ].x + 16;
        oam[ SPR_NAV_APP_RSV_SUB + 4 ].y = oam[ SPR_NAV_APP_RSV_SUB + 3 ].y = 87 - 16;
        oam[ SPR_NAV_APP_RSV_SUB + 6 ].y = oam[ SPR_NAV_APP_RSV_SUB + 5 ].y
            = oam[ SPR_NAV_APP_RSV_SUB + 3 ].y;

        oam[ SPR_NAV_APP_RSV_SUB + 3 ].isHidden = false;
        oam[ SPR_NAV_APP_RSV_SUB + 4 ].isHidden = false;
        oam[ SPR_NAV_APP_RSV_SUB + 5 ].isHidden = false;
        oam[ SPR_NAV_APP_RSV_SUB + 6 ].isHidden = false;

        tileCnt = IO::loadUIIcon( IO::ICON::ARROW_START, SPR_NAV_APP_RSV_SUB, SPR_X_PAL_SUB,
                                  tileCnt, oam[ SPR_NAV_APP_RSV_SUB + 3 ].x + 8,
                                  oam[ SPR_NAV_APP_RSV_SUB + 3 ].y + 8, 16, 16, false, false, false,
                                  OBJPRIORITY_1, p_bottom, OBJMODE_NORMAL );

        oam[ SPR_NAV_APP_RSV_SUB + 1 ]   = oam[ SPR_NAV_APP_RSV_SUB ];
        oam[ SPR_NAV_APP_RSV_SUB + 1 ].x = oam[ SPR_NAV_APP_RSV_SUB + 5 ].x + 8;
        oam[ SPR_NAV_APP_RSV_SUB + 1 ].y = oam[ SPR_NAV_APP_RSV_SUB + 5 ].y + 8;

        oam[ SPR_NAV_APP_RSV_SUB + 1 ].hFlip = true;

        IO::copySpritePal( ARR_X_SPR_PAL, SPR_X_PAL_SUB, 0, 2 * 7, p_bottom );

        // load bg

        tileCnt = IO::loadSprite( "SEL/noselection_64_20", SPR_NAV_APP_RSV_SUB + 7, SPR_BOX_PAL_SUB,
                                  tileCnt, 0, 0, 32, 32, false, false, false, OBJPRIORITY_3,
                                  p_bottom, OBJMODE_BLENDED );

        for( u8 i = 0; i < SONGS_PER_PAGE + 1; ++i ) {
            for( u8 j = 0; j < 5; ++j ) {
                auto idx = SPR_NAV_APP_RSV_SUB + 7 + 5 * i + j;

                oam[ idx ]   = oam[ SPR_NAV_APP_RSV_SUB + 7 ];
                oam[ idx ].x = 49 + 24 * j;
                oam[ idx ].y = 50 + 22 * i - 16;

                if( i == SONGS_PER_PAGE ) { oam[ idx ].y += 10; }

                if( j == 4 ) {
                    oam[ idx ].vFlip = oam[ idx ].hFlip = true;
                    oam[ idx ].y -= 12;
                }
            }
        }

        FS::readPictureData( ptr3, "nitro:/PICS/NAV_APP/", "app02bg", 192 * 2, 192 * 256,
                             p_bottom );
        pal[ 0 ] = IO::BLACK;

        setMode( _currentMode, p_bottom );

        auto s1 = std::string( GET_STRING( IO::STR_UI_JBOX_LUDICOBOX ) );
        if( _currentSong && SOUND::SSEQ::existsBGM( _currentSong ) ) {
            snprintf( buffer, 99, "%s: %s", s1.c_str( ), FS::getBGMName( _currentSong ).c_str( ) );
        } else {
            snprintf( buffer, 99, "%s: %s", s1.c_str( ),
                      GET_STRING( _currentMode == JMODE_SHOW_RECORD
                                      ? IO::STR_UI_JBOX_CHOOSE_RECORD
                                      : IO::STR_UI_JBOX_CHOOSE_SONG ) );
        }
        IO::regularFont->printStringC( buffer, 12, 10, p_bottom, IO::font::LEFT );
    }

    void jboxNavApp::hoverButton( u16 p_btn, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;

        if( p_btn == FWD_CHOICE ) {
            oam[ SPR_NAV_APP_RSV_SUB + 5 ].palette = oam[ SPR_NAV_APP_RSV_SUB + 6 ].palette
                = SPR_BOX_SEL_PAL_SUB;
        } else {
            oam[ SPR_NAV_APP_RSV_SUB + 5 ].palette = oam[ SPR_NAV_APP_RSV_SUB + 6 ].palette
                = SPR_BOX_PAL_SUB;
        }
        if( p_btn == BWD_CHOICE ) {
            oam[ SPR_NAV_APP_RSV_SUB + 3 ].palette = oam[ SPR_NAV_APP_RSV_SUB + 4 ].palette
                = SPR_BOX_SEL_PAL_SUB;
        } else {
            oam[ SPR_NAV_APP_RSV_SUB + 3 ].palette = oam[ SPR_NAV_APP_RSV_SUB + 4 ].palette
                = SPR_BOX_PAL_SUB;
        }

        if( _currentMode == JMODE_SHOW_RECORD ) {
            for( u16 i = 0; i < SONGS_PER_PAGE; ++i ) {
                if( p_btn == NUM_SPECIAL_TGS + i * MAX_SONGS_PER_RECORD ) {
                    setSongChoicePal( i, SPR_BOX_SEL_PAL_SUB, p_bottom );
                } else {
                    setSongChoicePal( i, SPR_BOX_PAL_SUB, p_bottom );
                }
            }
        } else {
            for( u8 i = 0; i < SONGS_PER_PAGE; ++i ) {
                if( p_btn == NUM_SPECIAL_TGS + i ) {
                    setSongChoicePal( i, SPR_BOX_SEL_PAL_SUB, p_bottom );
                } else {
                    setSongChoicePal( i, SPR_BOX_PAL_SUB, p_bottom );
                }
            }
        }

        if( p_btn == STOP_PLAYBACK ) {
            setSongChoicePal( SONGS_PER_PAGE, SPR_BOX_SEL_PAL_SUB, p_bottom );
        } else {
            setSongChoicePal( SONGS_PER_PAGE, SPR_BOX_PAL_SUB, p_bottom );
        }

        IO::updateOAM( p_bottom );
    }

    void jboxNavApp::selectSong( u16 p_idx, bool p_bottom ) {
        SOUND::playCry( PKMN_LUDICOLO );

        char buffer[ 100 ];
        if( !p_idx || !SOUND::SSEQ::existsBGM( _currentSong ) ) {
            auto s1 = std::string( GET_STRING( IO::STR_UI_JBOX_LUDICOBOX ) );
            SOUND::setJBoxBGM( SOUND::JBOX_DISABLED );
            snprintf( buffer, 99, "%s: %s", s1.c_str( ),
                      GET_STRING( _currentMode == JMODE_SHOW_RECORD
                                      ? IO::STR_UI_JBOX_CHOOSE_RECORD
                                      : IO::STR_UI_JBOX_CHOOSE_SONG ) );
            IO::printRectangle( 45, 10, 200, 30, p_bottom, 0 );
            IO::regularFont->printStringC( buffer, 12, 10, p_bottom, IO::font::LEFT );
            return;
        }

        if( _currentMode == JMODE_SHOW_RECORD ) {
            _currentRecord   = p_idx;
            _currentSelStart = _currentRecord + 1;
            setMode( JMODE_SHOW_SONG );
        } else {
            _currentSong = p_idx;
            auto s1      = std::string( GET_STRING( IO::STR_UI_JBOX_LUDICOBOX ) );
            SOUND::setJBoxBGM( _currentSong );
            snprintf( buffer, 99, "%s: %s", s1.c_str( ), FS::getBGMName( _currentSong ).c_str( ) );
            IO::printRectangle( 45, 10, 200, 30, p_bottom, 0 );
            IO::regularFont->printStringC( buffer, 12, 10, p_bottom, IO::font::LEFT );
        }
    }

    void jboxNavApp::drawSongList( u16 p_startIdx, bool p_listRecords, bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        IO::printRectangle( 45, 30, 200, 180, p_bottom, 0 );

        for( u8 i = 0; i < SONGS_PER_PAGE; ++i ) {
            auto next = p_listRecords ? p_startIdx + MAX_SONGS_PER_RECORD * i : p_startIdx + i;

            if( SOUND::SSEQ::existsBGM( next ) ) {
                auto idx = SPR_NAV_APP_RSV_SUB + 7 + 5 * i;

                setSongChoiceVis( i, false, p_bottom );
                IO::regularFont->printStringC( FS::getBGMName( next ).c_str( ), oam[ idx ].x + 64,
                                               oam[ idx ].y + 2, p_bottom, IO::font::CENTER );
            } else {
                setSongChoiceVis( i, true, p_bottom );
            }
        }

        auto ub = p_listRecords ? p_startIdx + MAX_SONGS_PER_RECORD * SONGS_PER_PAGE
                                : p_startIdx + SONGS_PER_PAGE;
        auto lb = p_listRecords ? p_startIdx / MAX_SONGS_PER_RECORD : p_startIdx - _currentRecord;

        oam[ SPR_NAV_APP_RSV_SUB + 1 ].isHidden = !SOUND::SSEQ::existsBGM( ub );
        oam[ SPR_NAV_APP_RSV_SUB ].isHidden     = lb <= s16( SONGS_PER_PAGE );

        auto idx = SPR_NAV_APP_RSV_SUB + 7 + 5 * SONGS_PER_PAGE;
        IO::regularFont->printStringC( GET_STRING( IO::STR_UI_JBOX_STOP_PLAYBACK ),
                                       oam[ idx ].x + 64, oam[ idx ].y + 2, p_bottom,
                                       IO::font::CENTER );
    }

    std::vector<std::pair<IO::inputTarget, u16>> jboxNavApp::touchPositions( bool p_bottom ) {
        SpriteEntry* oam = ( p_bottom ? IO::Oam : IO::OamTop )->oamBuffer;
        std::vector<std::pair<IO::inputTarget, u16>> res
            = std::vector<std::pair<IO::inputTarget, u16>>( );

        // prev / next page
        res.push_back( std::pair( IO::inputTarget( oam[ SPR_NAV_APP_RSV_SUB + 3 ].x,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 3 ].y,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 3 ].x + 32,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 3 ].y + 32 ),
                                  BWD_CHOICE ) );

        res.push_back( std::pair( IO::inputTarget( oam[ SPR_NAV_APP_RSV_SUB + 5 ].x,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 5 ].y,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 5 ].x + 32,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 5 ].y + 32 ),
                                  FWD_CHOICE ) );

        // select song
        for( u8 i = 0; i < SONGS_PER_PAGE; ++i ) {
            auto next = _currentMode == JMODE_SHOW_RECORD
                            ? _currentRecSelStart + i * MAX_SONGS_PER_RECORD
                            : _currentSelStart + i;

            if( SOUND::SSEQ::existsBGM( next ) ) {
                auto idx = SPR_NAV_APP_RSV_SUB + 7 + 5 * i;
                res.push_back( std::pair( IO::inputTarget( oam[ idx ].x, oam[ idx ].y,
                                                           oam[ idx ].x + 128, oam[ idx ].y + 20 ),
                                          next + NUM_SPECIAL_TGS ) );
            }
        }

        // stop playback
        auto idx = SPR_NAV_APP_RSV_SUB + 7 + 5 * SONGS_PER_PAGE;
        res.push_back( std::pair(
            IO::inputTarget( oam[ idx ].x, oam[ idx ].y, oam[ idx ].x + 128, oam[ idx ].y + 20 ),
            STOP_PLAYBACK ) );

        // back
        res.push_back( std::pair( IO::inputTarget( oam[ SPR_NAV_APP_RSV_SUB + 2 ].x,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 2 ].y,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 2 ].x + 20,
                                                   oam[ SPR_NAV_APP_RSV_SUB + 2 ].y + 20 ),
                                  EXIT_CHOICE ) );

        return res;
    }

    void jboxNavApp::redraw( bool p_bottom ) {
        if( _currentMode == JMODE_SHOW_RECORD ) {
            drawSongList( _currentRecSelStart, true, p_bottom );
        } else {
            drawSongList( _currentSelStart, false, p_bottom );
        }
        IO::updateOAM( p_bottom );
    }

    void jboxNavApp::setMode( mode p_newMode, bool p_bottom ) {
        _currentMode = p_newMode;
        redraw( p_bottom );
    }

    bool jboxNavApp::tick( bool p_bottom ) {
        for( auto c : touchPositions( ) ) {
            if( c.first.inRange( touch ) ) {
                if( c.second >= NUM_SPECIAL_TGS ) {
                    if( _currentMode == JMODE_SHOW_RECORD ) {
                        hoverButton( c.second - _currentRecSelStart, p_bottom );
                    } else {
                        hoverButton( c.second - _currentSelStart, p_bottom );
                    }
                } else {
                    hoverButton( c.second, p_bottom );
                }
                bool suc = true;
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );

                    if( !c.first.inRange( touch ) ) {
                        suc = false;
                        break;
                    }
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }
                hoverButton( 0, p_bottom );
                if( suc ) {
                    if( c.second == EXIT_CHOICE ) {
                        // if currently showing songs, go back to record overview;
                        // otherwise exit
                        if( _currentMode == JMODE_SHOW_RECORD ) {
                            return true;
                        } else {
                            setMode( JMODE_SHOW_RECORD, p_bottom );
                            break;
                        }
                    } else if( c.second == FWD_CHOICE ) {
                        if( _currentMode == JMODE_SHOW_RECORD ) {
                            if( SOUND::SSEQ::existsBGM( _currentRecSelStart
                                                        + SONGS_PER_PAGE
                                                              * MAX_SONGS_PER_RECORD ) ) {
                                SOUND::playCry( PKMN_BELLOSSOM );
                                _currentRecSelStart += SONGS_PER_PAGE * MAX_SONGS_PER_RECORD;
                                redraw( p_bottom );
                                break;
                            }
                        } else {
                            if( SOUND::SSEQ::existsBGM( _currentSelStart + SONGS_PER_PAGE ) ) {
                                SOUND::playCry( PKMN_BELLOSSOM );
                                _currentSelStart += SONGS_PER_PAGE;
                                redraw( p_bottom );
                                break;
                            }
                        }
                    } else if( c.second == BWD_CHOICE ) {
                        if( _currentMode == JMODE_SHOW_RECORD ) {
                            if( _currentRecSelStart > SONGS_PER_PAGE * MAX_SONGS_PER_RECORD ) {
                                SOUND::playCry( PKMN_BELLOSSOM );
                                _currentRecSelStart -= SONGS_PER_PAGE * MAX_SONGS_PER_RECORD;
                                redraw( p_bottom );
                                break;
                            }
                        } else {
                            if( _currentSelStart > _currentRecord + SONGS_PER_PAGE ) {
                                SOUND::playCry( PKMN_BELLOSSOM );
                                _currentSelStart -= SONGS_PER_PAGE;
                                redraw( p_bottom );
                                break;
                            }
                        }
                    } else if( c.second == STOP_PLAYBACK ) {
                        selectSong( 0 );
                    } else {
                        selectSong( c.second - NUM_SPECIAL_TGS );
                    }
                }
            }
        }

        return false;
    }

} // namespace IO
#endif
