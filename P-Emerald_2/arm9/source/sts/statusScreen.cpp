#include "statusScreen.h"
#include "../io/uio.h"
#include "../io/messageBox.h"
#include "../defines.h"
#include "../ds/item.h"
#include "../ds/move.h"
#include "../fs/saveGame.h"

namespace STS {
    statusScreen::statusScreen( u8 p_pkmnIdx, statusScreenUI* p_stsUI )
        : _pokemon( p_stsUI->_pokemon ) {
        _page = 0;
        _pkmnIdx = p_pkmnIdx;
        _stsUI = p_stsUI;
    }

    s16 regStsScreen::run( bool p_time, s8 p_timeParameter ) {
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
                _pkmnIdx = ( _pkmnIdx + 1 ) % _pokemon.size( );
                _stsUI->init( _pkmnIdx );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                _pkmnIdx = ( _pkmnIdx + _pokemon.size( ) - 1 ) % _pokemon.size( );
                _stsUI->init( _pkmnIdx );
            } else if( _pokemon[ _pkmnIdx ].m_boxdata.m_holdItem &&
                       GET_AND_WAIT_R( 152, ( -7 + 24 * fieldCnt ), 300, ( 17 + 24 * fieldCnt ) ) ) {
                char buffer[ 50 ];
                item acI = *ItemList[ _pokemon[ _pkmnIdx ].m_boxdata.m_holdItem ];
                _pokemon[ _pkmnIdx ].m_boxdata.m_holdItem = 0;

                sprintf( buffer, "%s von %ls\nim Beutel verstaut.", acI.getDisplayName( ).c_str( ), _pokemon[ _pkmnIdx ].m_boxdata.m_name );
                IO::messageBox( buffer, p_time );
                FS::SAV->m_bag.addItem( acI.getItemType( ), acI.getItemId( ), 1 );
                _stsUI->init( _pkmnIdx );
            } else if( GET_AND_WAIT_R( 152, ( -7 + 24 * ( !!_pokemon[ _pkmnIdx ].m_boxdata.m_holdItem + fieldCnt ) ),
                300, ( 17 + 24 * ( fieldCnt + !!_pokemon[ _pkmnIdx ].m_boxdata.m_holdItem ) ) ) ) {

                //run the PokeDex from here

                _stsUI->init( _pkmnIdx );
            }
            for( u8 i = 0; i < fieldCnt; ++i )
                if( GET_AND_WAIT_R( 152, ( -7 + 24 * i ), 256, ( 17 + 24 * i ) ) ) {
                    u8 u = 0, o;
                    for( o = 0; o < 4 && u <= i; ++o )
                        if( AttackList[ _pokemon[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->m_isFieldAttack )
                            u++;
                    o--;
                    if( AttackList[ _pokemon[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->possible( ) ) {

                        char buffer[ 50 ];
                        sprintf( buffer, "%ls setzt %s\nein!", _pokemon[ _pkmnIdx ].m_boxdata.m_name,
                                 AttackList[ _pokemon[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->m_moveName.c_str( ) );
                        IO::messageBox( buffer, p_time );

                        //shoUseAttack( _pokemon[ _pkmnIdx ].m_boxdata.m_speciesId,
                        //              _pokemon[ _pkmnIdx ].m_boxdata.m_isFemale, _pokemon[ _pkmnIdx ].m_boxdata.isShiny( ) );

                        AttackList[ _pokemon[ _pkmnIdx ].m_boxdata.m_moves[ o ] ]->use( );
                        return;
                    } else {
                        IO::messageBox( "Diese Attacke kann jetzt\nnicht eingesetzt werden.", "PokéNav", p_time );
                        _stsUI->init( _pkmnIdx );
                    }
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
                _pkmnIdx = ( _pkmnIdx + 1 ) % _pokemon.size( );
                _stsUI->draw( _pkmnIdx, _page, true );
            } else if( GET_AND_WAIT( KEY_UP ) || GET_AND_WAIT_C( 248, 162, 16 ) ) {
                _pkmnIdx = ( _pkmnIdx + _pokemon.size( ) - 1 ) % _pokemon.size( );
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