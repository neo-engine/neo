#include "bagViewer.h"
#include "defines.h"
#include "uio.h"
#include "saveGame.h"
#include "yesNoBox.h"
#include "messageBox.h"

#include <nds.h>

#include <algorithm>

namespace BAG {
#define TRESHOLD 20
    bagViewer::bagViewer( bag* p_bag, bagUI* p_bagUI )
        :_bagUI( p_bagUI ), _origBag( p_bag ) {
        _currPage = 0;
        _currItem = 0;
        _currCmp = std::less<std::pair<u16, u16>>( );
        for( u8 i = 0; i < BAG_CNT; ++i ) {
            _bagUI->_bag[ i ] = std::vector<std::pair<u16, u16>>( );
            _bagUI->_bag[ i ].clear( );
            if( !p_bag->element( bag::bagType( i ) ).empty( ) ) {
                for( auto j : p_bag->element( bag::bagType( i ) ) )
                    _bagUI->_bag[ i ].push_back( j );
                //std::stable_sort( _bagUI->_bag[ i ].begin( ), _bagUI->_bag[ i ].end( ), _currCmp );
            }

        }
        _atHandOam = 0;
    }

    void bagViewer::run( u8 p_startPage, u16 p_startIdx ) {
        _currPage = p_startPage;
        _currItem = p_startIdx;

        _bagUI->init( );
        _ranges = _bagUI->drawBagPage( _currPage, _currItem );

        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            u8 curr = 0;

            //if( _atHandOam && !( touch.px || touch.py ) ) { //Player drops the sprite at hand
            //    _atHandOam = 0;
            //    u32 res = _bagUI->acceptDrop( curr );
            //    u8 targetPkmn = u8( res >> 16 );
            //    auto& acPkmn = FS::SAV->m_pkmnTeam[ targetPkmn ];

            //    u16 targetItem = u16( res );
            //    std::pair<u16, u16> currItem = { targetItem, _origBag->element( bag::bagType( _currPage ) )[ targetItem ] };

            //    switch( res >> 24 ) {
            //        case GIVE_ITEM:{
            //            if( !acPkmn.m_boxdata.m_speciesId || acPkmn.m_boxdata.m_individualValues.m_isEgg )
            //                break;
            //            if( acPkmn.m_boxdata.m_holdItem ) {
            //                IO::yesNoBox yn;
            //                sprintf( buffer, "%ls trägt bereits\ndas Item %s.\nSollen die Items getauscht werden?",
            //                         acPkmn.m_boxdata.m_name, ItemList[ acPkmn.m_boxdata.m_holdItem ]->getDisplayName( true ).c_str( ) );
            //                if( !yn.getResult( buffer ) )
            //                    break;

            //                auto currBgType = toBagType( ItemList[ acPkmn.m_boxdata.m_holdItem ]->getItemType( ) );
            //                _origBag->insert( currBgType, acPkmn.m_boxdata.m_holdItem, 1 );
            //                auto bgI = std::find_if( _bag[ (u8)currBgType ].begin( ), _bag[ (u8)currBgType ].end( ), [ acPkmn ]( std::pair<u16, u16> p_item ) {
            //                    return p_item.first == acPkmn.m_boxdata.m_holdItem;
            //                } );
            //                if( bgI != _bag[ currBgType ].end( ) )
            //                    ++bgI->second;
            //                else
            //                    _bag[ currBgType ].push_back( { acPkmn.m_boxdata.m_holdItem, 1 } );
            //            }
            //            _origBag->erase( bag::bagType( _currPage ), targetItem, 1 );
            //            auto bgI = std::find( _bag[ _currPage ].begin( ), _bag[ _currPage ].end( ), currItem );
            //            if( !--bgI->second )
            //                _bag[ _currPage ].erase( bgI );

            //            acPkmn.m_boxdata.m_holdItem = targetItem;
            //            sprintf( buffer, "%ls trägt nun\ndas Item %s.",
            //                     acPkmn.m_boxdata.m_name, ItemList[ targetItem ]->getDisplayName( true ).c_str( ) );
            //            IO::messageBox m( buffer );
            //            _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            //        }
            //        case TAKE_ITEM:{
            //            if( !acPkmn.m_boxdata.m_speciesId || acPkmn.m_boxdata.m_individualValues.m_isEgg )
            //                break;
            //            if( acPkmn.m_boxdata.m_holdItem ) {
            //                auto currBgType = toBagType( ItemList[ acPkmn.m_boxdata.m_holdItem ]->getItemType( ) );
            //                _origBag->insert( currBgType, acPkmn.m_boxdata.m_holdItem, 1 );
            //                auto bgI = std::find_if( _bag[ (u8)currBgType ].begin( ), _bag[ (u8)currBgType ].end( ), [ acPkmn ]( std::pair<u16, u16> p_item ) {
            //                    return ( p_item.first == acPkmn.m_boxdata.m_holdItem );
            //                } );
            //                if( bgI != _bag[ currBgType ].end( ) )
            //                    ++bgI->second;
            //                else
            //                    _bag[ currBgType ].push_back( { acPkmn.m_boxdata.m_holdItem, 1 } );

            //                acPkmn.m_boxdata.m_holdItem = 0;
            //                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            //            }
            //            break;
            //        }
            //        case MOVE_ITEM:{
            //            if( !acPkmn.m_boxdata.m_speciesId || acPkmn.m_boxdata.m_individualValues.m_isEgg )
            //                break;
            //            if( acPkmn.m_boxdata.m_holdItem && targetItem != targetPkmn ) {
            //                //In this case, currItem is actually the index of the targetted PKMN
            //                auto& acPkmn2 = FS::SAV->m_pkmnTeam[ targetItem ];
            //                if( !acPkmn2.m_boxdata.m_speciesId || acPkmn2.m_boxdata.m_individualValues.m_isEgg )
            //                    break;
            //                std::swap( acPkmn.m_boxdata.m_holdItem, acPkmn2.m_boxdata.m_holdItem );
            //                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            //            }
            //            break;
            //        }

            //        case MOVE_BAG:
            //            break;

            //        default:
            //            break;
            //    }
            //} else 
            if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X ) || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } else if( GET_AND_WAIT( KEY_LEFT ) ) {
                _currPage = ( _currPage + BAG_CNT - 1 ) % BAG_CNT;
                _currItem %= _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
                _currPage = ( _currPage + 1 ) % BAG_CNT;
                _currItem %= _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            } else if( GET_AND_WAIT( KEY_DOWN ) ) {
                _currItem = ( _currItem + 1 ) % _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            } else if( GET_AND_WAIT( KEY_UP ) ) {
                _currItem = ( _currItem + _bagUI->_bag[ _currPage ].size( ) - 1 ) % _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            } else if( GET_AND_WAIT_C( SCREEN_WIDTH - 44, SCREEN_HEIGHT - 10, 16 ) ) {
                _currItem = ( _currItem + 8 ) % _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            } else if( GET_AND_WAIT_C( SCREEN_WIDTH - 76, SCREEN_HEIGHT - 10, 16 ) ) {
                _currItem = ( _currItem + _bagUI->_bag[ _currPage ].size( ) - 8 ) % _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            }
            for( u8 i = 0; i < 5; ++i )
                if( i != _currPage && GET_AND_WAIT_C( 26 * i + 13, 3 + 13, 13 ) ) {
                    _currPage = i;
                    _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                    break;
                }
            //for( u8 j = 0; !_atHandOam && j < _ranges.size( ); ++j ) {
            //    auto i = _ranges[ j ];
            //    if( IN_RANGE( touch, IO::inputTarget( i.first.first, i.first.second, i.second.first, i.second.second ) ) ) {
            //        u8 c = 0;
            //        loop( ) {
            //            scanKeys( );
            //            swiWaitForVBlank( );
            //            touchRead( &touch );
            //            if( c++ == TRESHOLD ) {
            //                _atHandOam = _bagUI->getSprite( j );
            //                if( _atHandOam )
            //                    curr = j;
            //                break;
            //            }
            //            if( !( touch.px || touch.py ) )
            //                break;
            //        }
            //    }
            //}
        }
    }
}