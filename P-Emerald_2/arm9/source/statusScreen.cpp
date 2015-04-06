#include "statusScreen.h"
#include "uio.h"
#include "messageBox.h"
#include "defines.h"
#include "item.h"
#include "move.h"
#include "saveGame.h"
#include "dex.h"

namespace STS {
    regStsScreen::regStsScreen( u8 p_pkmnIdx, statusScreenUI* p_stsUI ) {
        _pokemon = p_stsUI->_pokemon;
        _page = 0;
        _pkmnIdx = p_pkmnIdx;
        _stsUI = p_stsUI;
    }

    void regStsScreen::run( bool p_time, s8 p_timeParameter ) {
        _stsUI->init( _pkmnIdx );
        touchPosition touch;

        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            if( p_time )
                IO::updateTime( p_timeParameter );
            int pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_X ) || GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_A ) || GET_AND_WAIT_C( 128, 96, 16 ) ) {
                auto res = drawPage( p_time, p_timeParameter );
                if( res & KEY_X )
                    break;
                else if( res & KEY_B ) {
                    _stsUI->init( _pkmnIdx );
                    continue;
                }
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                _pkmnIdx = ( _pkmnIdx + 1 ) % _pokemon->size( );
                _stsUI->init( _pkmnIdx, false );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                _pkmnIdx = ( _pkmnIdx + _pokemon->size( ) - 1 ) % _pokemon->size( );
                _stsUI->init( _pkmnIdx, false );
            } else if( _stsUI->_showTakeItem &&
                       GET_AND_WAIT_R( 152, !!_stsUI->_showMoveCnt * ( -7 + 24 * _stsUI->_showMoveCnt ), 300, ( 17 + 24 * _stsUI->_showMoveCnt ) ) ) {
                char buffer[ 50 ];
                item acI = *ItemList[ ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_holdItem ];
                ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_holdItem = 0;

                sprintf( buffer, "%s von %ls\nim Beutel verstaut.", acI.getDisplayName( ).c_str( ), ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_name );
                IO::messageBox( buffer, p_time );
                FS::SAV->m_bag->insert( BAG::toBagType( acI.getItemType( ) ), acI.getItemId( ), 1 );
                _stsUI->init( _pkmnIdx, false );
            } else if( GET_AND_WAIT_R( 152, !!( _stsUI->_showTakeItem + _stsUI->_showMoveCnt ) * ( -7 + 24 * ( _stsUI->_showTakeItem + _stsUI->_showMoveCnt ) ),
                300, ( 17 + 24 * ( _stsUI->_showMoveCnt + _stsUI->_showTakeItem ) ) ) ) {

                DEX::dex( -1, 0 ).run( ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_speciesId, p_time, p_timeParameter );

                _stsUI->init( _pkmnIdx );
            }
            for( u8 i = 0; i < _stsUI->_showMoveCnt; ++i )
                if( GET_AND_WAIT_R( 152, ( !!i * ( -7 + 24 * i ) ), 256, ( 17 + 24 * i ) ) ) {
                    u8 u = 0, o;
                    for( o = 0; o < 4 && u <= i; ++o )
                        if( AttackList[ ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->m_isFieldAttack )
                            u++;
                    o--;
                    consoleSelect( &IO::Bottom );
                    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    if( AttackList[ ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->possible( ) ) {

                        char buffer[ 50 ];
                        sprintf( buffer, "%ls setzt %s\nein!", ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_name,
                                 AttackList[ ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->m_moveName.c_str( ) );
                        IO::messageBox( buffer, p_time );
                        IO::drawSub( );
                        


                        //shoUseAttack( (*_pokemon)[_pkmnIdx ].m_boxdata.m_speciesId,
                        //              (*_pokemon)[_pkmnIdx ].m_boxdata.m_isFemale, (*_pokemon)[_pkmnIdx ].m_boxdata.isShiny( ) );

                        AttackList[ ( *_pokemon )[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->use( );
                        return;
                    } else {
                        IO::messageBox( "Diese Attacke kann jetzt\nnicht eingesetzt werden.", "PokéNav", p_time );
                        _stsUI->init( _pkmnIdx, false );
                    }
                    break;
                }

        }

    }
    s16 regStsScreen::drawPage( bool p_time, s8 p_timeParameter ) {
        _stsUI->draw( _pkmnIdx, _page, true );

        touchPosition touch;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            if( p_time )
                IO::updateTime( p_timeParameter );
            int pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X ) || GET_AND_WAIT_C( 248, 184, 16 ) ) {
                return ( pressed & KEY_X ) | KEY_B;
            } else if( GET_AND_WAIT( KEY_DOWN ) || GET_AND_WAIT_C( 220, 184, 16 ) ) {
                _pkmnIdx = ( _pkmnIdx + 1 ) % _pokemon->size( );
                _stsUI->draw( _pkmnIdx, _page, true );
            } else if( GET_AND_WAIT( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                _pkmnIdx = ( _pkmnIdx + _pokemon->size( ) - 1 ) % _pokemon->size( );
                _stsUI->draw( _pkmnIdx, _page, true );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                _page = ( _page + 1 ) % _stsUI->m_pagemax;
                _stsUI->draw( _pkmnIdx, _page, false );
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                _page = ( _page + _stsUI->m_pagemax - 1 ) % _stsUI->m_pagemax;
                _stsUI->draw( _pkmnIdx, _page, false );
            }

            else if( _page != 0 && GET_AND_WAIT_C( 12, 12, 16 ) ) {
                _page = 0;
                _stsUI->draw( _pkmnIdx, _page, false );
            } else if( _page != 1 && GET_AND_WAIT_C( 8, 34, 16 ) ) {
                _page = 1;
                _stsUI->draw( _pkmnIdx, _page, false );
            } else if( _page != 2 && GET_AND_WAIT_C( 34, 10, 16 ) ) {
                _page = 2;
                _stsUI->draw( _pkmnIdx, _page, false );
            }
        }

    }
}