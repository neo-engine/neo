#include "bagViewer.h"
#include "defines.h"
#include "uio.h"
#include "saveGame.h"
#include "yesNoBox.h"
#include "messageBox.h"
#include "choiceBox.h"

#include <nds.h>

#include <algorithm>

namespace BAG {
#define TRESHOLD 10
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
        u8 curr = -1, start = -1;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );

            if( _atHandOam ) {
                _bagUI->updateAtHand( touch, _atHandOam );
            }

            if( _atHandOam && !( touch.px | touch.py ) ) { //Player drops the sprite at hand
                u32 res = _bagUI->acceptDrop( start, curr, _atHandOam );
                _atHandOam = 0;
                u8 targetPkmn = u8( res >> 16 );
                pokemon& acPkmn = FS::SAV->m_pkmnTeam[ targetPkmn ];

                u16 targetItem = res % ( 1 << 16 );
                std::pair<u16, u16> currItem = { targetItem, _origBag->element( bag::bagType( _currPage ) )[ targetItem ] };

                switch( res >> 24 ) {
                    case GIVE_ITEM:{
                        if( !acPkmn.m_boxdata.m_speciesId || acPkmn.m_boxdata.m_individualValues.m_isEgg )
                            break;
                        if( ItemList[ targetItem ]->getItemType( ) == item::itemType::KEY_ITEM )
                            break;
                        if( ItemList[ targetItem ]->getItemType( ) == item::itemType::TM_HM ) {
                            TM* currTm = static_cast<TM*>( ItemList[ targetItem ] );
                            u16 currMv = currTm->m_moveIdx;

                            if( currMv == acPkmn.m_boxdata.m_moves[ 0 ]
                                || currMv == acPkmn.m_boxdata.m_moves[ 1 ]
                                || currMv == acPkmn.m_boxdata.m_moves[ 2 ]
                                || currMv == acPkmn.m_boxdata.m_moves[ 3 ] ) {
                                sprintf( buffer, "%ls beherrscht\n%s bereits!", acPkmn.m_boxdata.m_name, AttackList[ currMv ]->m_moveName.c_str( ) );
                                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                                IO::messageBox a( buffer, false );
                            } else if( canLearn( acPkmn.m_boxdata.m_speciesId, currMv, 4 ) ) {
                                bool freeSpot = false;
                                for( u8 i = 0; i < 4; ++i )
                                    if( !acPkmn.m_boxdata.m_moves[ i ] ) {
                                        acPkmn.m_boxdata.m_moves[ i ] = currMv;

                                        sprintf( buffer, "%ls erlernt\n%s!", acPkmn.m_boxdata.m_name, AttackList[ currMv ]->m_moveName.c_str( ) );
                                        IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                                        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                                        IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                                        IO::messageBox a( buffer, false );

                                        freeSpot = true;
                                        break;
                                    }
                                if( !freeSpot ) {
                                    IO::yesNoBox yn( false );
                                    sprintf( buffer, "%ls beherrscht\nbereits 4 Attacken.\nSoll eine verlernt werden?", acPkmn.m_boxdata.m_name );
                                    if( yn.getResult( buffer ) ) {
                                        u8 res = IO::choiceBox( acPkmn, currMv ).getResult( "Welche Attacke?", false );
                                        if( res < 4 )
                                            acPkmn.m_boxdata.m_moves[ res ] = currMv;
                                    }
                                }
                            } else {
                                sprintf( buffer, "%ls kann\n%s nicht erlernen!", acPkmn.m_boxdata.m_name, AttackList[ currMv ]->m_moveName.c_str( ) );
                                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                                IO::messageBox a( buffer, false );
                            }
                            _bagUI->init( );
                            _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                            break;
                        }
                        if( acPkmn.m_boxdata.m_holdItem ) {
                            IO::yesNoBox yn( false );
                            sprintf( buffer, "%ls trägt bereits\ndas Item %s.\nSollen die Items getauscht werden?",
                                     acPkmn.m_boxdata.m_name, ItemList[ acPkmn.m_boxdata.m_holdItem ]->getDisplayName( true ).c_str( ) );
                            if( !yn.getResult( buffer ) ) {
                                _bagUI->init( );
                                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                                break;
                            }
                            auto currBgType = toBagType( ItemList[ acPkmn.m_boxdata.m_holdItem ]->getItemType( ) );
                            _origBag->insert( currBgType, acPkmn.m_boxdata.m_holdItem, 1 );
                            auto bgI = std::find_if( _bagUI->_bag[ (u8)currBgType ].begin( ), _bagUI->_bag[ (u8)currBgType ].end( ), [ acPkmn ]( std::pair<u16, u16> p_item ) {
                                return p_item.first == acPkmn.m_boxdata.m_holdItem;
                            } );
                            if( bgI != _bagUI->_bag[ currBgType ].end( ) )
                                ++bgI->second;
                            else
                                _bagUI->_bag[ currBgType ].push_back( { acPkmn.m_boxdata.m_holdItem, 1 } );
                        }
                        _origBag->erase( bag::bagType( _currPage ), targetItem, 1 );
                        auto bgI = std::find( _bagUI->_bag[ _currPage ].begin( ), _bagUI->_bag[ _currPage ].end( ), currItem );
                        if( !--bgI->second )
                            _bagUI->_bag[ _currPage ].erase( bgI );

                        acPkmn.m_boxdata.m_holdItem = targetItem;
                        //sprintf( buffer, "%ls trägt nun\ndas Item %s.",
                        //         acPkmn.m_boxdata.m_name, ItemList[ targetItem ]->getDisplayName( true ).c_str( ) );
                        //IO::messageBox m( buffer );
                        _bagUI->init( );
                        _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                        break;
                    }
                    case TAKE_ITEM:{
                        if( !acPkmn.m_boxdata.m_speciesId || acPkmn.m_boxdata.m_individualValues.m_isEgg )
                            break;
                        if( acPkmn.m_boxdata.m_holdItem ) {
                            auto currBgType = toBagType( ItemList[ acPkmn.m_boxdata.m_holdItem ]->getItemType( ) );
                            _origBag->insert( currBgType, acPkmn.m_boxdata.m_holdItem, 1 );
                            auto bgI = std::find_if( _bagUI->_bag[ (u8)currBgType ].begin( ), _bagUI->_bag[ (u8)currBgType ].end( ), [ acPkmn ]( std::pair<u16, u16> p_item ) {
                                return ( p_item.first == acPkmn.m_boxdata.m_holdItem );
                            } );
                            if( bgI != _bagUI->_bag[ currBgType ].end( ) )
                                ++bgI->second;
                            else
                                _bagUI->_bag[ currBgType ].push_back( { acPkmn.m_boxdata.m_holdItem, 1 } );

                            acPkmn.m_boxdata.m_holdItem = 0;
                            _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                        }
                        break;
                    }
                    case MOVE_ITEM:{
                        if( !acPkmn.m_boxdata.m_speciesId || acPkmn.m_boxdata.m_individualValues.m_isEgg )
                            break;
                        if( acPkmn.m_boxdata.m_holdItem && targetItem != targetPkmn ) {
                            //In this case, currItem is actually the index of the targetted PKMN
                            auto& acPkmn2 = FS::SAV->m_pkmnTeam[ targetItem ];
                            if( !acPkmn2.m_boxdata.m_speciesId || acPkmn2.m_boxdata.m_individualValues.m_isEgg )
                                break;
                            std::swap( acPkmn.m_boxdata.m_holdItem, acPkmn2.m_boxdata.m_holdItem );
                            _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                        }
                        break;
                    }

                    case MOVE_BAG:
                        break;

                    default:
                        break;
                }
            } else if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X ) || ( !_atHandOam &&  GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) ) {
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
            } else if( !_atHandOam && GET_AND_WAIT_C( SCREEN_WIDTH - 44, SCREEN_HEIGHT - 10, 16 ) ) {
                _currItem = ( _currItem + 8 ) % _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            } else if( !_atHandOam && GET_AND_WAIT_C( SCREEN_WIDTH - 76, SCREEN_HEIGHT - 10, 16 ) ) {
                _currItem = ( _currItem + _bagUI->_bag[ _currPage ].size( ) - 8 ) % _bagUI->_bag[ _currPage ].size( );
                _ranges = _bagUI->drawBagPage( _currPage, _currItem );
            }
            for( u8 i = 0; i < 5; ++i )
                if( i != _currPage &&  !_atHandOam && GET_AND_WAIT_C( 26 * i + 13, 3 + 13, 13 ) ) {
                    _currPage = i;
                    _ranges = _bagUI->drawBagPage( _currPage, _currItem );
                    break;
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
                            if( c++ == TRESHOLD ) {
                                _atHandOam = _bagUI->getSprite( j, touch );
                                if( _atHandOam )
                                    start = curr = j;
                                break;
                            }
                            if( !( touch.px || touch.py ) || !IN_RANGE( touch, i ) )
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