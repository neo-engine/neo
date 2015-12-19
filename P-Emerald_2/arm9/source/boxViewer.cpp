#include "boxViewer.h"
#include "boxUI.h"
#include "defines.h"
#include "uio.h"
#include "statusScreen.h"
#include "statusScreenUI.h"
#include "saveGame.h"

namespace BOX {
#define TRESHOLD 10
    u16 boxViewer::nextNonEmptyBox( u16 p_start ) {
        for( u16 i = p_start + 1; i <= MAX_PKMN + 1; ++i )
            if( !_box->empty( i ) )
                return i;
        return p_start;
    }
    u16 boxViewer::previousNonEmptyBox( u16 p_start ) {
        for( u16 i = p_start - 1; i > 0; --i )
            if( !p_start )
                return p_start;
            else if( !_box->empty( i ) )
                return i;
        return p_start;
    }

    void boxViewer::generateNextPage( ) {

        _currPage[ 0 ] = _currPage[ 28 ];

        u16 pokemon = _currPage[ 0 ].first;
        u16 pos = _currPage[ 0 ].second;

        for( u8 i = 1; i < 30; ++i )
            _currPage[ i ] = { 0, 0 };

        for( u8 i = 1; i < 30; ++i ) {
            if( ++pos < _box->count( pokemon ) )
                _currPage[ i ] = { pokemon, pos };
            else {
                u16 n = nextNonEmptyBox( pokemon );
                if( n == pokemon ) {
                    return;
                } else {
                    _currPage[ i ] = { n, 0 };
                    pos = 0;
                    pokemon = n;
                }
            }
        }
    }
    void boxViewer::generatePreviousPage( ) {

        if( !( _currPage[ 0 ].first || _currPage[ 0 ].second ) ) //It's already the first page
            return;

        _currPage[ 29 ] = _currPage[ 1 ];
        u16 pokemon = _currPage[ 1 ].first;
        u16 pos = _currPage[ 1 ].second;

        for( u8 i = 0; i < 29; ++i )
            _currPage[ i ] = { 0, 0 };

        for( u8 i = 28; i >= 0; --i ) {
            if( pos-- )
                _currPage[ i ] = { pokemon, pos };
            else {
                u16 n = previousNonEmptyBox( pokemon );
                if( n == pokemon ) {
                    _currPage[ 0 ] = { 0, 0 };

                    return;
                } else {
                    _currPage[ i ] = { n, pos = _box->count( n ) - 1 };
                    pokemon = n;
                }
            }
        }
    }

    void boxViewer::run( bool p_allowTakePkmn ) {
        _boxUI->init( );
        _atHandOam = 0;
        _ranges = _boxUI->draw( _currPage, _currPos = 0, _box, '*', true, p_allowTakePkmn );

        u8 mx = p_allowTakePkmn ? 21 : 28;

        touchPosition touch;
        u8 curr = -1, start = -1;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            u8 oldPos = _currPos;
            bool newPok;

            if( _atHandOam ) {
                _boxUI->updateAtHand( touch, _atHandOam );
            }

            if( _atHandOam && !( touch.px | touch.py ) ) { //Player drops the sprite at hand
                u32 res = _boxUI->acceptDrop( start, curr, _atHandOam );
                _atHandOam = 0;

                if( start < 21 && curr < 21 )
                    continue;

                else if( start >= 21 && curr >= 21 ) {
                    //Count the team pokemon
                    u8 cnt = 0;
                    for( u8 i = 0; i < 6; ++i )
                        if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId
                            && FS::SAV->m_pkmnTeam[ i ].m_stats.m_acHP )
                            ++cnt;
                    if( !FS::SAV->m_pkmnTeam[ start - 21 ].m_stats.m_acHP )
                        ++cnt;

                    if( curr == u8( -1 ) && cnt > 1 ) { //Deposit the pokemon
                        _box->insert( FS::SAV->m_pkmnTeam[ start - 21 ] );

                        for( u8 i = start - 21; i < 5; ++i )
                            std::swap( FS::SAV->m_pkmnTeam[ i ], FS::SAV->m_pkmnTeam[ i + 1 ] );
                        memset( &FS::SAV->m_pkmnTeam[ 5 ], 0, sizeof( pokemon ) );

                        for( u8 i = 0; i < 30; ++i )
                            _currPage[ i ] = { 0, 0 };
                        generateNextPage( );
                        _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                    } else if( curr != u8( -1 ) && FS::SAV->m_pkmnTeam[ curr - 21 ].m_boxdata.m_speciesId &&
                               FS::SAV->m_pkmnTeam[ start - 21 ].m_boxdata.m_speciesId ) {
                        std::swap( FS::SAV->m_pkmnTeam[ start - 21 ], FS::SAV->m_pkmnTeam[ curr - 21 ] );

                        _ranges = _boxUI->draw( _currPage, _currPos = curr, _box, start, true, true );
                    } else {
                        _ranges = _boxUI->draw( _currPage, _currPos = start, _box, start, true, true );
                    }
                } else if( start < 21 && curr >= 21 && _currPage[ start ].first ) {
                    //if( FS::SAV->m_pkmnTeam[ curr - 21 ].m_boxdata.m_speciesId ) {
                    //    _box->insert( FS::SAV->m_pkmnTeam[ curr - 21 ] );
                    //}

                    //FS::SAV->m_pkmnTeam[ curr - 21 ].m_boxdata =

                } else if( start >= 21 && curr < 21 ) {

                    }
            }

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                if( p_allowTakePkmn && _currPos >= 21 ) {
                    newPok = false;
                    _currPos = ( ( _currPos - 20 ) % 6 ) + 21;
                } else {
                    if( newPok = ( ( ( ++_currPos ) %= mx ) < oldPos ) )
                        generateNextPage( );
                }
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                if( p_allowTakePkmn && _currPos >= 21 ) {
                    newPok = false;
                    _currPos = ( ( _currPos - 16 ) % 6 ) + 21;
                } else {
                    if( newPok = ( ( ( _currPos += ( mx - 1 ) ) %= mx ) > oldPos ) )
                        generatePreviousPage( );
                }
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                if( newPok = ( ( ( _currPos += ( mx - 7 ) ) %= mx ) > oldPos ) )
                    generatePreviousPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                if( newPok = ( ( ( _currPos += 7 ) %= mx ) < oldPos ) )
                    generateNextPage( );
                _ranges = _boxUI->draw( _currPage, _currPos, _box, oldPos, newPok, p_allowTakePkmn );
            } else if( GET_AND_WAIT( KEY_A ) ) {

            }

            bool rangeChanged = false;
            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE( touch, i ) ) {
                    if( !_atHandOam ) {
                        u8 c = 0;
                        loop( ) {
                            scanKeys( );
                            swiWaitForVBlank( );
                            touchRead( &touch );
                            if( c++ == TRESHOLD && p_allowTakePkmn ) {
                                _atHandOam = _boxUI->getSprite( _currPos, j );
                                _ranges = _boxUI->draw( _currPage, j, _box, _currPos, false, p_allowTakePkmn );
                                _currPos = j;
                                if( _atHandOam )
                                    start = curr = j;
                                break;
                            }
                            if( !touch.px && !touch.py ) {
                                u8 res = _boxUI->acceptTouch( _currPos, j, p_allowTakePkmn );
                                _ranges = _boxUI->draw( _currPage, j, _box, _currPos, false, p_allowTakePkmn );
                                _currPos = j;
                                break;
                            }
                            if( !IN_RANGE( touch, i ) )
                                break;
                        }
                    } else {
                        curr = j;
                        rangeChanged = true;
                    }
                }
            }
            if( !rangeChanged )
                curr = -1;
        }
    }
}