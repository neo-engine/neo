/*
Pokémon Emerald 2 Version
------------------------------

file        : bagViewer.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
Philip Wellnitz

This file is part of Pokémon Emerald 2 Version.

Pokémon Emerald 2 Version is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon Emerald 2 Version is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon Emerald 2 Version.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    bagViewer::bagViewer( ) {
        _currCmp = std::less<std::pair<u16, u16>>( );
    }

#define CURRENT_ITEM FS::SAV->m_bag( ( bag::bagType )FS::SAV->m_lstBag, (FS::SAV->m_lstBagItem + _currSelectedIdx) % FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
    void bagViewer::initUI( ) {
        _bagUI->init( );
        _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
        if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
            _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
    }

    /*
     *  Returns true if the item needs to be removed from the bag, false otherwise
     */
    bool bagViewer::useItemOnPkmn( pokemon& p_pokemon, u16 p_item ) {
        if( !p_pokemon.m_boxdata.m_speciesId || p_pokemon.isEgg( ) )
            return false;
        if( ItemList[ p_item ]->m_itemType == item::itemType::KEY_ITEM )
            return false;
        if( ItemList[ p_item ]->m_itemType == item::itemType::TM_HM ) {
            TM* currTm = static_cast<TM*>( ItemList[ p_item ] );
            u16 currMv = currTm->m_moveIdx;

            p_pokemon.learnMove( currMv );
            return true;
        }

        if( ItemList[ p_item ]->m_itemType == item::itemType::MEDICINE ) {
            for( u8 i = 0; i < 2; ++i )
                if( ItemList[ p_item ]->needsInformation( i ) ) {
                    u8 res = 1 + IO::choiceBox( p_pokemon, 0 ).getResult( "Welche Attacke?", false );
                    initUI( );

                    u32& newEffect = ItemList[ p_item ]->m_itemData.m_itemEffect;
                    newEffect &= ~( 1 << ( 9 + 16 * !i ) );
                    newEffect |= ( res << ( 9 + 16 * !i ) );
                }
            u8 oldLv = p_pokemon.m_Level;
            if( ItemList[ p_item ]->use( p_pokemon ) ) {

                sprintf( buffer, "%s auf %ls angewendet.", ItemList[ p_item ]->getDisplayName( true ).c_str( ),
                         p_pokemon.m_boxdata.m_name );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( buffer, false );

                FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, p_item, 1 );

                //Check for evolution
                if( p_pokemon.m_Level != oldLv && p_pokemon.canEvolve( ) ) {
                    initUI( );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                    sprintf( buffer, "%ls entwickelt sich...", p_pokemon.m_boxdata.m_name );
                    IO::messageBox( buffer, false );
                    p_pokemon.evolve( );

                    initUI( );
                    swiWaitForVBlank( );

                    sprintf( buffer, "...und wurde zu einem\n%ls!", getWDisplayName( p_pokemon.m_boxdata.m_speciesId ) );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                    IO::messageBox( buffer, false );
                }
                return false;
            }
            IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
            IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
            IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
            IO::messageBox( "Es würde keine Wirkung haben...", false );
            return false;
        }

        if( ItemList[ p_item ]->getEffectType( ) == item::itemEffectType::USE_ON_PKMN ) {
            //Use the item on the PKMN
            //If it is an ordinary item, it could be only an evolutionary item
            if( ItemList[ p_item ]->m_itemType == item::itemType::GOODS
                && !p_pokemon.canEvolve( p_item, 3 ) ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( "Es würde keine\nWirkung haben...", false );
                return false;
            } else if( ItemList[ p_item ]->m_itemType == item::itemType::GOODS ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                sprintf( buffer, "%ls entwickelt sich...", p_pokemon.m_boxdata.m_name );
                IO::messageBox( buffer, false );
                p_pokemon.evolve( p_item, 3 );

                initUI( );
                swiWaitForVBlank( );

                sprintf( buffer, "...und wurde zu einem\n%ls!", getWDisplayName( p_pokemon.m_boxdata.m_speciesId ) );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( buffer, false );
            }
            return false;
        }
        return false;
    }

    /*
    *  Returns true if the item needs to be removed from the bag, false otherwise
    */
    bool bagViewer::giveItemToPkmn( pokemon& p_pokemon, u16 p_item ) {
        if( p_pokemon.getItem( ) ) {
            IO::yesNoBox yn( false );
            sprintf( buffer, "%ls trägt bereits\ndas Item %s.\nSollen die Items getauscht werden?",
                     p_pokemon.m_boxdata.m_name, ItemList[ p_pokemon.getItem( ) ]->getDisplayName( true ).c_str( ) );
            if( !yn.getResult( buffer ) )
                return false;

            takeItemFromPkmn( p_pokemon );
        }
        p_pokemon.m_boxdata.m_holdItem = p_item;
        return true;
    }

    void bagViewer::takeItemFromPkmn( pokemon& p_pokemon ) {
        auto currBgType = toBagType( ItemList[ p_pokemon.getItem( ) ]->m_itemType );
        FS::SAV->m_bag.insert( currBgType, p_pokemon.getItem( ), 1 );
        p_pokemon.m_boxdata.m_holdItem = 0;
    }

    /*
    *  Returns true if the item needs to be removed from the bag, false otherwise
    */
    bool bagViewer::confirmChoice( context p_context, u16 p_targetItem ) {
        bool possible = true;

        //Check if the item can be returned
        switch( p_context ) {
            case BAG::bagViewer::BATTLE:
                possible &= ( ItemList[ p_targetItem ]->getEffectType( )
                              & ( item::itemEffectType::IN_BATTLE | item::itemEffectType::USE_ON_PKMN ) );
                break;
            case BAG::bagViewer::GIVE_TO_PKMN:
                possible &= ( ItemList[ p_targetItem ]->m_itemType != item::itemType::KEY_ITEM );
                possible &= ( ItemList[ p_targetItem ]->m_itemType != item::itemType::TM_HM );
                possible &= ( ItemList[ p_targetItem ]->m_itemType != item::itemType::MAILS );
                break;
            default:
                break;
        }

        if( !possible ) {
            IO::messageBox( "Dieses Item kann nicht\nausgewählt werden." );
            return false;
        }
        IO::yesNoBox yn( false );
        sprintf( buffer, "%s auswählen?",
                 ItemList[ p_targetItem ]->getDisplayName( true ).c_str( ) );
        if( !yn.getResult( buffer ) ) {
            return false;
        }

        return true;
    }

    bool bagViewer::handleSomeInput( touchPosition p_touch, int p_pressed ) {
        auto& touch = p_touch;
        auto& pressed = p_pressed;

        auto curBgsz = FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag );

        if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X ) || ( !_hasSprite &&  GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) ) {
            return false;
        } else if( GET_AND_WAIT( KEY_LEFT ) ) {
            _currSelectedIdx = 0;
            FS::SAV->m_lstBag = ( FS::SAV->m_lstBag + BAG_CNT - 1 ) % BAG_CNT;
            if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
                FS::SAV->m_lstBagItem %= FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag );
            else
                FS::SAV->m_lstBagItem = 0;
            _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
                _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
        } else if( GET_AND_WAIT( KEY_RIGHT ) ) {
            _currSelectedIdx = 0;
            FS::SAV->m_lstBag = ( FS::SAV->m_lstBag + 1 ) % BAG_CNT;
            if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
                FS::SAV->m_lstBagItem %= FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag );
            else
                FS::SAV->m_lstBagItem = 0;
            _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
                _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
        } else if( GET_AND_WAIT( KEY_DOWN ) ) {
            if( !curBgsz ) return true;
            _bagUI->unselectItem( ( bag::bagType )FS::SAV->m_lstBag, _currSelectedIdx, CURRENT_ITEM.first );
            u8 mx = std::min( 9u, curBgsz );
            if( ++_currSelectedIdx == mx ) {
                _currSelectedIdx = 0;
                FS::SAV->m_lstBagItem = ( FS::SAV->m_lstBagItem + mx ) % curBgsz;
                _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            }
            _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
        } else if( GET_AND_WAIT( KEY_UP ) ) {
            if( !curBgsz ) return true;
            _bagUI->unselectItem( ( bag::bagType )FS::SAV->m_lstBag, _currSelectedIdx, CURRENT_ITEM.first );
            u8 mx = std::min( 9u, curBgsz );
            if( _currSelectedIdx-- == 0 ) {
                _currSelectedIdx = mx - 1;
                FS::SAV->m_lstBagItem = ( FS::SAV->m_lstBagItem + curBgsz - mx ) % curBgsz;
                _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            }
            _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
        } else if( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 44, SCREEN_HEIGHT - 10, 16 ) ) {
            if( !curBgsz ) return true;
            FS::SAV->m_lstBagItem = ( FS::SAV->m_lstBagItem + 8 ) % curBgsz;
            _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
        } else if( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 76, SCREEN_HEIGHT - 10, 16 ) ) {
            if( !curBgsz ) return true;
            FS::SAV->m_lstBagItem = ( FS::SAV->m_lstBagItem + curBgsz - 8 ) % curBgsz;
            _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
            _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
        }

        for( u8 i = 0; i < 5; ++i )
            if( i != FS::SAV->m_lstBag &&  !_hasSprite && GET_AND_WAIT_C( 26 * i + 13, 3 + 13, 13 ) ) {
                FS::SAV->m_lstBag = i;
                _ranges = _bagUI->drawBagPage( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );
                if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
                    _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
                break;
            }
        return true;
    }


    void bagViewer::run( ) {
        _bagUI = new bagUI( );
        initUI( );
        _hasSprite = false;

        touchPosition touch;
        u8 curr = -1, start = -1;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );

            if( _hasSprite )
                _bagUI->updateSprite( touch );

            /*/
            if( _atHandOam && !( touch.px | touch.py ) ) { //Player drops the sprite at hand
                u32 res = _bagUI->acceptDrop( start, curr, _atHandOam );
                _atHandOam = 0;
                u8 targetPkmn = u8( res >> 16 );
                pokemon& acPkmn = FS::SAV->m_pkmnTeam[ targetPkmn ];

                u16 targetItem = res % ( 1 << 16 );
                std::pair<u16, u16> currItem = { targetItem, _origBag->element( bag::bagType( _currPage ) )[ targetItem ] };

            } else */ if( !handleSomeInput( touch, pressed ) )
                break;
            /*
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
                            if( !touch.px && !touch.py ) {
                                u8 res = _bagUI->acceptTouch( j );
                                switch( res ) {
                                    default:
                                        break;
                                }
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
                curr = -1; */
        }
    }

    u16 bagViewer::getItem( context p_context ) {
        _bagUI = new bagUI( );
        initUI( );
        _hasSprite = false;

        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );
            if( !handleSomeInput( touch, pressed ) )
                return 0;
            if( GET_AND_WAIT( KEY_A ) ) {
                if( FS::SAV->m_bag.empty( ( bag::bagType )FS::SAV->m_lstBag ) )
                    continue;
                u16 targetItem = CURRENT_ITEM.first;
                if( targetItem && confirmChoice( p_context, targetItem ) ) {
                    FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, CURRENT_ITEM.first, 1 );
                    return targetItem;
                }
                initUI( );
            }

            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE( touch, i.first ) ) {
                    u8 c = 0;
                    loop( ) {
                        scanKeys( );
                        swiWaitForVBlank( );
                        touchRead( &touch );
                        if( c++ == TRESHOLD ) { //Ask the player whether he is sure about the choice
                            while( touch.px || touch.py ) {
                                scanKeys( );
                                swiWaitForVBlank( );
                                touchRead( &touch );
                            }
                            u16 targetItem = i.second.m_item;
                            if( !i.second.m_isHeld && confirmChoice( p_context, targetItem ) ) {
                                FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, CURRENT_ITEM.first, 1 );
                                return targetItem;
                            } else {
                                initUI( );
                                break;
                            }
                        }
                        if( !touch.px && !touch.py ) {
                            if( !i.second.m_isHeld || i.second.m_item ) {
                                if( FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )
                                    _bagUI->unselectItem( ( bag::bagType )FS::SAV->m_lstBag, _currSelectedIdx, CURRENT_ITEM.first );
                                else //the current bag is empty
                                    _bagUI->unselectItem( ( bag::bagType )FS::SAV->m_lstBag, MAX_ITEMS_PER_PAGE, 0 );
                            }
                            if( !i.second.m_isHeld ) {
                                _currSelectedIdx = j;
                                _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
                            } else if( i.second.m_item ) {
                                u8 vl = j;
                                if( j < MAX_ITEMS_PER_PAGE )
                                    vl += MAX_ITEMS_PER_PAGE - FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag );
                                _bagUI->selectItem( vl, { i.second.m_item, 1 } );
                            }
                            break;
                        }
                        if( !IN_RANGE( touch, i.first ) )
                            break;
                    }
                }
            }
        }
        return 0;
    }
}