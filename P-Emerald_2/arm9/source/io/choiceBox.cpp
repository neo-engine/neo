#include "choiceBox.h"
#include "uio.h"
#include "../defines.h"
#include "../ds/pokemon.h"

#include "Back.h"
#include "Forward.h"
#include "Backward.h"

namespace IO {
#define NEW_PAGE 9
    void choiceBox::draw( u8 p_pressedIdx ) {
        if( p_pressedIdx == NEW_PAGE ) {
            drawSub( );
            initTextField( );
            if( _text ) {
                if( _name ) {
                    regularFont->printString( _name, 8, 8, true );
                    regularFont->printString( _text, 72, 8, true );
                } else
                    regularFont->printString( _text, 8, 8, true );
            }
        }
        u8 startIdx = ( ( !_big ) * 3 + 3 ) * _acPage;
        u8 endIdx = std::min( _num, u8( ( ( !_big ) * 3 + 3 ) * ( _acPage + 1 ) ) );

        BG_PALETTE_SUB[ COLOR_IDX ] = CHOICE_COLOR;

        if( _big ) {
            for( u8 i = startIdx; i < endIdx; ++i ) {
                u8 acPos = i - startIdx;
                printChoiceBox( 32, 68 + acPos * 35, 192 + 32, 68 + 32 + acPos * 35, 6, COLOR_IDX, acPos == p_pressedIdx );
                regularFont->printString( _choices[ i ], 40 + 2 * ( p_pressedIdx == acPos ),
                                          78 + acPos * 35 + ( p_pressedIdx == acPos ), true );
                swiWaitForVBlank( );
            }
        } else {
            for( u8 i = startIdx; i < endIdx; ++i ) {
                u8 acPos = i - startIdx;
                printChoiceBox( ( ( acPos % 2 ) ? 129 : 19 ), 68 + ( acPos / 2 ) * 35,
                               106 + ( ( acPos % 2 ) ? 129 : 19 ), 32 + 68 + ( acPos / 2 ) * 35, 6, COLOR_IDX, acPos == p_pressedIdx );
                regularFont->printString( _choices[ i ], ( ( acPos % 2 ) ? 129 : 19 ) + 8 + 2 * ( p_pressedIdx == acPos ),
                                          78 + ( acPos / 2 ) * 35 + ( p_pressedIdx == acPos ), true );
                swiWaitForVBlank( );
            }
        }
    }
    choiceBox::choiceBox( int p_num, const char** p_choices, const char* p_name = 0, bool p_big = false ) {
        _num = p_num;
        _choices = p_choices;
        _big = p_big || ( p_num <= 3 );
        _acPage = 0;

        initTextField( );

        initOAMTableSub( Oam );
        u16 nextAvailableTileIdx = 16;

        nextAvailableTileIdx = loadSprite( Oam, spriteInfo, BACK_ID, 0, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                           BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = loadSprite( Oam, spriteInfo, FWD_ID, 1, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, ForwardPal,
                                           ForwardTiles, ForwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        nextAvailableTileIdx = loadSprite( Oam, spriteInfo, BWD_ID, 2, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackwardPal,
                                           BackwardTiles, BackwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        updateOAMSub( Oam );

        _name = p_name;

        swiWaitForVBlank( );
    }

    int fwdPos[ 2 ][ 2 ] = { { SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12 }, { SCREEN_WIDTH - 11, SCREEN_HEIGHT - 31 } },
        bwdPos[ 2 ][ 2 ] = { { SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12 }, { SCREEN_WIDTH - 31, SCREEN_HEIGHT - 11 } };

    int choiceBox::getResult( const char* p_text, bool p_time, bool p_backButton ) {
        _text = p_text;
        draw( NEW_PAGE );
        initOAMTableSub( Oam );
        u16 nextAvailableTileIdx = 16;

        nextAvailableTileIdx = loadSprite( Oam, spriteInfo, BACK_ID, 0, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                           BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        nextAvailableTileIdx = loadSprite( Oam, spriteInfo, FWD_ID, 1, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, ForwardPal,
                                           ForwardTiles, ForwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        nextAvailableTileIdx = loadSprite( Oam, spriteInfo, BWD_ID, 2, nextAvailableTileIdx,
                                           SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackwardPal,
                                           BackwardTiles, BackwardTilesLen, false, false, true, OBJPRIORITY_1, true );
        updateOAMSub( Oam );

        int result = -1;

        if( p_backButton ) {
            ( Oam->oamBuffer[ BACK_ID ] ).isHidden = false;
            ( Oam->oamBuffer[ BACK_ID ] ).x = fwdPos[ 0 ][ 0 ] - 12;
            ( Oam->oamBuffer[ BACK_ID ] ).y = fwdPos[ 0 ][ 1 ] - 12;
            updateOAMSub( Oam );
        }

        if( _num < 1 )
            return -1;
        else if( _num <= 3 || ( _num > 3 && _big ) ) {
            _acPage = 0;
            if( _num > 3 && _big ) {
                ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
            }
            while( 42 ) {
                swiWaitForVBlank( );
                updateOAMSub( Oam );
                if( p_time )
                    updateTime( s8( 1 ) );
                touchPosition t;
                touchRead( &t );

                for( u8 i = 0; i < 3; ++i )
                    if( ( i + 3 * _acPage ) < _num && t.px >= 32 && t.py >= 68 + 35 * i && t.px <= 224 && t.py <= 100 + 35 * i ) {
                        draw( i );
                        if( !waitForTouchUp( p_time, true, 32, 68 + 35 * i, 224, 100 + 35 * i ) ) {
                            draw( 8 );
                            break;
                        }
                        result = i + 3 * _acPage;
                        goto END;
                    }
                if( p_backButton && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] - 12 ) + sq( t.py - fwdPos[ 0 ][ 1 ] - 12 ) ) < 17 ) { //Back pressed
                    result = -1;
                    goto END;
                }
                if( _num > 3 && _big ) {
                    if( !p_backButton && ( ( _num - 1 ) / 3 && _acPage == 0 && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] ) + sq( t.py - fwdPos[ 0 ][ 1 ] ) ) < 17 )
                        || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - fwdPos[ 1 ][ 0 ] ) + sq( t.py - fwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                        waitForTouchUp( p_time, true );
                        if( ( ++_acPage ) >= ( ( _num - 1 ) / 3 ) ) {
                            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = true;
                            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = !_acPage;
                            ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ p_backButton ][ 0 ] - 16;
                            ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ p_backButton ][ 1 ] - 16;
                            //if( _acPage == ( ( _num - 1 ) / 3 ) )
                            //    draw( NEW_PAGE );
                            _acPage = ( ( _num - 1 ) / 3 );
                        } else {
                            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
                            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                            ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                            ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                            ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                            ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                            draw( NEW_PAGE );
                        }
                        updateOAMSub( Oam );
                    } else if( !p_backButton && ( _acPage && _acPage == ( _num - 1 ) / 3 && sqrt( sq( t.px - bwdPos[ 0 ][ 0 ] ) + sq( t.py - bwdPos[ 0 ][ 1 ] ) ) < 17 )
                               || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - bwdPos[ 1 ][ 0 ] ) + sq( t.py - bwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                        waitForTouchUp( p_time, true );
                        if( ( --_acPage ) <= 0 ) {
                            ( Oam->oamBuffer[ 14 ] ).isHidden = true;
                            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = !( _acPage < ( ( _num - 1 ) / 3 ) );
                            ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                            ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
                            if( _acPage == 0 )
                                draw( NEW_PAGE );
                            _acPage = 0;
                        } else {
                            ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                            ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
                            ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                            ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                            ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                            ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                            draw( NEW_PAGE );
                        }
                        updateOAMSub( Oam );
                    }
                }
            }
        } else {
            _acPage = 0;
            if( _num > 6 ) {
                ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
            }
            updateOAMSub( Oam );
            while( 42 ) {
                swiWaitForVBlank( );
                updateOAMSub( Oam );
                if( p_time )
                    updateTime( s8( 1 ) );
                touchPosition t;
                touchRead( &t );

                for( u8 i = 0; i < std::min( 6, _num - 6 * _acPage ); ++i ) {
                    if( t.px >= ( ( i % 2 ) ? 129 : 19 ) && t.py >= 68 + ( i / 2 ) * 35
                        && t.px <= 106 + ( ( i % 2 ) ? 129 : 19 ) && t.py <= 32 + 68 + ( i / 2 ) * 35 ) {
                        draw( i );

                        if( !waitForTouchUp( p_time, true, ( ( i % 2 ) ? 129 : 19 ),
                            68 + ( i / 2 ) * 35, 106 + ( ( i % 2 ) ? 129 : 19 ), 32 + 68 + ( i / 2 ) * 35 ) ) {
                            draw( 8 );
                            break;
                        }
                        result = i + 6 * _acPage;
                        goto END;
                    }
                }

                if( p_backButton && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] - 12 ) + sq( t.py - fwdPos[ 0 ][ 1 ] - 12 ) ) < 17 ) { //Back pressed
                    result = -1;
                    goto END;
                } else if( !p_backButton && ( ( _num - 1 ) / 6 && _acPage == 0 && sqrt( sq( t.px - fwdPos[ 0 ][ 0 ] ) + sq( t.py - fwdPos[ 0 ][ 1 ] ) ) < 17 )
                           || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - fwdPos[ 1 ][ 0 ] ) + sq( t.py - fwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {

                    waitForTouchUp( p_time, true );
                    if( ( ++_acPage ) >= ( ( _num - 1 ) / 6 ) ) {
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = true;
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = !_acPage;
                        ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ p_backButton ][ 0 ] - 16;
                        ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ p_backButton ][ 1 ] - 16;
                        /*if( _acPage == ( _num / 6 ) )
                        draw( NEW_PAGE );*/
                        _acPage = ( _num / 6 );
                    } else {
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                        draw( NEW_PAGE );
                    }
                } else if( !p_backButton && ( _acPage && _acPage == ( _num - 1 ) / 6 && sqrt( sq( t.px - bwdPos[ 0 ][ 0 ] ) + sq( t.py - bwdPos[ 0 ][ 1 ] ) ) < 17 )
                           || ( ( p_backButton || _acPage ) && sqrt( sq( t.px - bwdPos[ 1 ][ 0 ] ) + sq( t.py - bwdPos[ 1 ][ 1 ] ) ) < 17 ) ) {
                    waitForTouchUp( p_time, true );
                    if( ( --_acPage ) <= 0 ) {
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = true;
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = !( _acPage < ( ( _num - 1 ) / 6 ) );
                        ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ p_backButton ][ 0 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ p_backButton ][ 1 ] - 16;
                        if( _acPage == 0 )
                            draw( NEW_PAGE );
                        _acPage = 0;
                    } else {
                        ( Oam->oamBuffer[ FWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ BWD_ID ] ).isHidden = false;
                        ( Oam->oamBuffer[ BWD_ID ] ).x = bwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ BWD_ID ] ).y = bwdPos[ 1 ][ 1 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).x = fwdPos[ 1 ][ 0 ] - 16;
                        ( Oam->oamBuffer[ FWD_ID ] ).y = fwdPos[ 1 ][ 1 ] - 16;
                        draw( NEW_PAGE );
                    }
                }
            }
        }
END:
        return result;
    }
}