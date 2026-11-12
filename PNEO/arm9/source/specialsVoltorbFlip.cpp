/*
    Pokémon neo
    ------------------------------

    file        : specialsVoltorbFlip.cpp
    author      : Philip Wellnitz
    description : Voltorb Flip mini game, ported from HGSS.

    Copyright (C) 2026
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

#include <nds.h>

#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "io/menuUI.h"
#include "io/screenFade.h"
#include "io/util.h"
#include "map/mapDrawer.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "spx/voltorbFlip.h"
#include "spx/voltorbFlipData.h"
#include "spx/voltorbFlipUI.h"

namespace SPX::VF {
    void realizePayout( u16 p_payout ) {
        u32 coins = (u32) SAVE::CURRENT_FILE->m_coins + (u32) p_payout;
        if( coins > MAX_PAYOUT ) { coins = MAX_PAYOUT; }
        SAVE::CURRENT_FILE->m_coins = (u16) coins;
    }

    void runVoltorbFlip( ) {
        IO::initVideo( );
        IO::initVideoSub( );

        IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bgSetPriority( IO::bg2sub, 2 );
        IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3sub, 3 );

        memset( (void*) BG_BMP_RAM_SUB( 1 ), 0, 256 * 192 );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::initColors( true, true, true );

        voltorbFlip game;
        memset( &game, 0, sizeof( game ) );
        voltorbFlipUI drawer;

        game.newBoard( );
        drawer.init( game );
        drawer.moveSelection( 0, 0 );
        IO::fadeScreen( IO::UNFADE_IMMEDIATE, true, true );

        auto askQuit = [ & ]( ) -> bool {
            bool yes = drawer.askQuit( game.payout( ) );
            drawer.fullRedraw( game );
            return yes;
        };

        bool memoOpen     = false;
        int  memoCell     = 0;
        int  memoPrevMark = 0;

        auto flipAt = [ & ]( int p_cardId ) {
            if( p_cardId < 0 || p_cardId >= NUM_TILES || game.cardAt( p_cardId ).m_flipped ) {
                return;
            }
            int type         = game.cardAt( p_cardId ).m_type;
            u32 payoutBefore = game.payout( );

            drawer.animateFlip( p_cardId, type );
            game.flipCard( p_cardId );

            if( type == CARD_VOLTORB ) {
                drawer.explode( p_cardId );
                game.setRoundOutcome( OUT_LOST );
                drawer.showMessage( GET_STRING( IO::STR_VF_0_COINS ) );

                for( int i = 0; i < NUM_TILES; i++ ) { game.flipCard( i ); }
                drawer.revealBoard( game );
                game.updateHistoryAndReset( );
                game.newBoard( );
                memoOpen = false;
                drawer.fullRedraw( game );
                return;
            }

            game.multiplyPayout( (cardType) type );
            u32 payoutAfter = game.payout( );

            std::array<char, 48> msg{ };
            if( payoutBefore == 0u ) {
                if( payoutAfter == 1 ) {
                    snprintf( msg.data( ), msg.size( ), GET_STRING( IO::STR_VF_1_COIN ),
                              FS::kanaWrap( (int) type, CURRENT_LANGUAGE ),
                              FS::kanaWrap( (unsigned) payoutAfter, CURRENT_LANGUAGE ) );
                } else {
                    snprintf( msg.data( ), msg.size( ), GET_STRING( IO::STR_VF_MULT_COINS ),
                              FS::kanaWrap( (int) type, CURRENT_LANGUAGE ),
                              FS::kanaWrap( (unsigned) payoutAfter, CURRENT_LANGUAGE ) );
                }
            } else if( type != CARD_ONE ) {
                snprintf( msg.data( ), msg.size( ), GET_STRING( IO::STR_VF_MULT_COINS ),
                          FS::kanaWrap( (int) type, CURRENT_LANGUAGE ),
                          FS::kanaWrap( (unsigned) payoutAfter, CURRENT_LANGUAGE ) );
            }
            if( msg[ 0 ] != '\0' ) { drawer.countPayoutTo( msg.data( ), payoutAfter ); }

            if( game.cleared( ) ) {
                SOUND::playSoundEffect( SFX_VOLTORB_WIN );
                drawer.showMessage( GET_STRING( IO::STR_VF_GAME_CLEAR ) );

                std::array<char, 64> award{ };
                if( payoutAfter == 1 ) {
                    snprintf( award.data( ), award.size( ), GET_STRING( IO::STR_VF_OBT_1_COIN ),
                              SAVE::CURRENT_FILE->getPlayerName( ) );
                } else {
                    snprintf( award.data( ), award.size( ), GET_STRING( IO::STR_VF_OBT_MULT_COINS ),
                              SAVE::CURRENT_FILE->getPlayerName( ),
                              FS::kanaWrap( (unsigned) payoutAfter, CURRENT_LANGUAGE ) );
                }
                drawer.transferToTotal( award.data( ), payoutAfter );

                realizePayout( (u16) payoutAfter );
                game.setRoundOutcome( OUT_WON );
                game.updateHistoryAndReset( );
                game.newBoard( );
                memoOpen = false;
                drawer.fullRedraw( game );
            }
        };

        int cursor = 0;

        auto refreshPanel = [ & ]( ) {
            drawer.drawMemoPanel( memoOpen, game.cardAt( cursor ).m_memo );
            if( memoOpen ) {
                drawer.memoCursor( memoCell );
            } else {
                drawer.moveSelection( cursor % NUM_COLS, cursor / NUM_COLS );
            }
        };
        auto toggleMemo = [ & ]( ) {
            memoOpen = !memoOpen;
            if( memoOpen ) {
                memoCell     = 0;
                memoPrevMark = 0;
            }
            drawer.loadMemoButton( memoOpen );
            refreshPanel( );
        };
        auto closeMemo = [ & ]( ) {
            if( memoOpen ) {
                memoOpen = false;
                drawer.loadMemoButton( false );
                refreshPanel( );
            }
        };
        auto moveMemoCursor = [ & ]( int p_cell ) {
            p_cell = p_cell < 0 ? 3 : ( p_cell > 3 ? 0 : p_cell );
            if( p_cell == memoCell ) { return; }
            memoPrevMark = memoCell;
            memoCell     = p_cell;
            SOUND::playSoundEffect( SFX_SELECT );
            drawer.memoCursor( memoCell );
        };
        auto markCell = [ & ]( int p_cell ) {
            if( game.cardAt( cursor ).m_flipped ) { return; }
            drawer.drawCardMark( cursor, game.toggleMemo( cursor, memoButtonMemo( p_cell ) ) );
            refreshPanel( );
        };

        bool touchDown = false;
        GAME_LOOP( IO::DEFAULT_TICK ) {
            bool down = IO::TOUCH.px || IO::TOUCH.py;
            bool just = down && !touchDown;
            touchDown = down;

            drawer.buttonHighlight( inQuitRect( IO::TOUCH.px, IO::TOUCH.py ),
                                    inMemoRect( IO::TOUCH.px, IO::TOUCH.py ) );

            bool leave = false;

            if( just && inMemoRect( IO::TOUCH.px, IO::TOUCH.py ) ) {
                toggleMemo( );
            } else if( just && memoOpen && memoButtonAt( IO::TOUCH.px, IO::TOUCH.py ) >= 0 ) {
                int cell = memoButtonAt( IO::TOUCH.px, IO::TOUCH.py );
                if( cell < 4 && memoCell < 4 ) { memoPrevMark = memoCell; }
                memoCell = cell;
                drawer.memoCursor( memoCell );
                markCell( cell );
            } else if( just && inQuitRect( IO::TOUCH.px, IO::TOUCH.py ) ) {
                closeMemo( );
                leave = askQuit( );
            } else if( just ) {
                int id = cellAt( IO::TOUCH.px, IO::TOUCH.py );
                if( id >= 0 ) {
                    cursor = id;
                    drawer.moveSelection( id % NUM_COLS, id / NUM_COLS );
                    if( memoOpen ) {
                        refreshPanel( );
                    } else {
                        flipAt( id );
                    }
                }
            } else if( ( IO::BTN_PRESSED & KEY_START )
                       || ( ( IO::BTN_PRESSED & KEY_B ) && !memoOpen ) ) {
                leave = askQuit( );
            } else if( IO::BTN_PRESSED & KEY_B ) {
                closeMemo( );
            } else if( IO::BTN_PRESSED & KEY_X ) {
                toggleMemo( );
            } else if( memoOpen ) {
                if( IO::BTN_PRESSED & KEY_LEFT ) {
                    moveMemoCursor( memoCell - 1 );
                } else if( IO::BTN_PRESSED & KEY_RIGHT ) {
                    moveMemoCursor( memoCell + 1 );
                } else if( IO::BTN_PRESSED & KEY_DOWN ) {
                    moveMemoCursor( memoCell + 2 );
                } else if( IO::BTN_PRESSED & KEY_UP ) {
                    if( memoCell == 4 && memoPrevMark >= 2 ) {
                        moveMemoCursor( memoPrevMark );
                    } else {
                        moveMemoCursor( memoCell - 2 );
                    }
                }
                if( IO::BTN_PRESSED & KEY_A ) { markCell( memoCell ); }
            } else {
                int  col   = cursor % NUM_COLS;
                int  row   = cursor / NUM_COLS;
                bool moved = false;
                if( ( IO::BTN_PRESSED & KEY_UP ) && row > 0 ) {
                    row--;
                    moved = true;
                } else if( ( IO::BTN_PRESSED & KEY_DOWN ) && row < 4 ) {
                    row++;
                    moved = true;
                } else if( ( IO::BTN_PRESSED & KEY_LEFT ) && col > 0 ) {
                    col--;
                    moved = true;
                } else if( ( IO::BTN_PRESSED & KEY_RIGHT ) && col < 4 ) {
                    col++;
                    moved = true;
                }
                if( moved ) {
                    SOUND::playSoundEffect( SFX_SELECT );
                    cursor = row * NUM_COLS + col;
                    drawer.moveSelection( col, row );
                }
                if( IO::BTN_PRESSED & KEY_A ) { flipAt( cursor ); }
            }

            if( leave ) {
                if( game.payout( ) > 0 ) {
                    std::array<char, 64> award{ };
                    if( game.payout( ) == 1 ) {
                        snprintf( award.data( ), award.size( ), GET_STRING( IO::STR_VF_OBT_1_COIN ),
                                  SAVE::CURRENT_FILE->m_playername );
                    } else {
                        snprintf( award.data( ), award.size( ),
                                  GET_STRING( IO::STR_VF_OBT_MULT_COINS ),
                                  SAVE::CURRENT_FILE->m_playername,
                                  FS::kanaWrap( (unsigned) game.payout( ), CURRENT_LANGUAGE ) );
                    }
                    drawer.transferToTotal( award.data( ), game.payout( ) );
                    realizePayout( (u16) game.payout( ) );
                }
                game.setRoundOutcome( OUT_QUIT );
                break;
            }
        }
    }
} // namespace SPX::VF
