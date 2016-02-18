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
#define TRESHOLD 20
    bagViewer::bagViewer( ) {
        _currCmp = std::less<std::pair<u16, u16>>( );
    }

#define CURRENT_ITEM FS::SAV->m_bag( ( bag::bagType )FS::SAV->m_lstBag, (FS::SAV->m_lstBagItem + _currSelectedIdx) % FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag ) )

    const char* choices[ 5 ] = { "Geben", "Anwenden", "Registrieren", "Einsetzen", "Wegwerfen" };

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
            return false;
        }

        if( ItemList[ p_item ]->m_itemType == item::itemType::MEDICINE ) {
            for( u8 i = 0; i < 2; ++i )
                if( ItemList[ p_item ]->needsInformation( i ) ) {
                    IO::choiceBox cb( p_pokemon, 0 );
                    _bagUI->drawPkmnIcons( );
                    u8 res = 1 + cb.getResult( "Welche Attacke?", false, false );
                    initUI( );

                    u32& newEffect = ItemList[ p_item ]->m_itemData.m_itemEffect;
                    newEffect &= ~( 1 << ( 9 + 16 * !i ) );
                    newEffect |= ( res << ( 9 + 16 * !i ) );
                }
            u8 oldLv = p_pokemon.m_level;
            if( ItemList[ p_item ]->use( p_pokemon ) ) {

                sprintf( buffer, "%s auf %s angewendet.", ItemList[ p_item ]->getDisplayName( true ).c_str( ),
                         p_pokemon.m_boxdata.m_name );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                IO::messageBox( buffer, false );

                FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, p_item, 1 );

                //Check for evolution
                if( p_pokemon.m_level != oldLv && p_pokemon.canEvolve( ) ) {
                    initUI( );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                    sprintf( buffer, "%s entwickelt sich…", p_pokemon.m_boxdata.m_name );
                    IO::messageBox( buffer, false );
                    p_pokemon.evolve( );

                    initUI( );
                    swiWaitForVBlank( );

                    sprintf( buffer, "…und wurde zu einem\n%s!", getDisplayName( p_pokemon.m_boxdata.m_speciesId ) );
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
            IO::messageBox( "Es würde keine Wirkung haben…", false );
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
                IO::messageBox( "Es würde keine\nWirkung haben…", false );
                return false;
            } else if( ItemList[ p_item ]->m_itemType == item::itemType::GOODS ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                sprintf( buffer, "%s entwickelt sich…", p_pokemon.m_boxdata.m_name );
                IO::messageBox( buffer, false );
                p_pokemon.evolve( p_item, 3 );

                initUI( );
                swiWaitForVBlank( );

                sprintf( buffer, "…und wurde zu einem\n%s!", getDisplayName( p_pokemon.m_boxdata.m_speciesId ) );
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
            sprintf( buffer, "%s trägt bereits\ndas Item %s.\nSollen die Items getauscht werden?",
                     p_pokemon.m_boxdata.m_name, ItemList[ p_pokemon.getItem( ) ]->getDisplayName( true ).c_str( ) );
            if( !yn.getResult( buffer ) )
                return false;

            takeItemFromPkmn( p_pokemon );
        }
        p_pokemon.m_boxdata.m_holdItem = p_item;
        return true;
    }

    void bagViewer::takeItemFromPkmn( pokemon& p_pokemon ) {
        if( p_pokemon.isEgg( ) || !p_pokemon.getItem( ) )
            return;
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
            IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
            IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
            IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
            IO::messageBox( "Dieses Item kann nicht\nausgewählt werden.", false );
            return false;
        }
        IO::yesNoBox yn( false );
        sprintf( buffer, "%s auswählen?",
                 ItemList[ p_targetItem ]->getDisplayName( true ).c_str( ) );
        IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
        IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
        IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
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

    bool useable( u16 p_item ) {
        return ( p_item == I_REPEL )
            || ( p_item == I_SUPER_REPEL )
            || ( p_item == I_MAX_REPEL )
            || ( p_item == I_ESCAPE_ROPE )
            || ( p_item == I_HONEY )

            || ( p_item == I_EXP_SHARE )
            || ( p_item == I_POKE_RADAR )
            || ( p_item == I_COIN_CASE )
            || ( p_item == I_OLD_ROD )
            || ( p_item == I_SUPER_ROD )
            || ( p_item == I_GOOD_ROD )
            || ( p_item == I_SQUIRT_BOTTLE )
            || ( p_item == I_SPRAYDUCK )
            || ( p_item == I_WAILMER_PAIL )
            || ( p_item == I_BICYCLE )
            || ( p_item == I_ACRO_BIKE )
            || ( p_item == I_BIKE2 )
            || ( p_item == I_MACH_BIKE );
    }

    //returns: 0 if nothing happend; 1 if the item got consumed and 2 if the bag is exited.
    //If an item has yet to be used the 14 highest bits contain its id
    u16 bagViewer::handleSelection( ) {
        item* itm = ItemList[ CURRENT_ITEM.first ];
        if( itm->m_itemType == item::TM_HM )
            return 0;

        IO::choiceBox cb( 1 + useable( CURRENT_ITEM.first ), choices + 2
                          + ( itm->m_itemType != item::KEY_ITEM ), 0, true );
        _bagUI->drawPkmnIcons( );
        sprintf( buffer, "Was tun mit %s?", itm->getDisplayName( true ).c_str( ) );
        int res = cb.getResult( buffer, true, false );
        if( res != -1 )
            res += ( itm->m_itemType != item::KEY_ITEM );
        u16 ret = ( itm->m_itemType != item::KEY_ITEM );
        switch( res ) {
            case 0: //Registry
            {
                FS::SAV->m_registeredItem = CURRENT_ITEM.first;
                break;
            }
            case 1: //Use
            {
                if( !itm->useable( ) ) {
                    initUI( );
                    IO::Oam->oamBuffer[ FWD_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                    IO::Oam->oamBuffer[ BWD_ID ].isHidden = true;
                    IO::messageBox( "Das kann jetzt nicht\neingesetzt werden.", false );
                    ret = 0;
                    break;
                }

                FS::SAV->m_lstUsedItems[ FS::SAV->m_lstUsedItemsIdx ] = CURRENT_ITEM.first;
                FS::SAV->m_lstUsedItemsIdx = ( FS::SAV->m_lstUsedItemsIdx + 1 ) % 5;

                bool res = itm->use( true );
                if( !res )
                    ret = 2 | ( CURRENT_ITEM.first << 2 );
                else {
                    initUI( );
                    itm->use( );
                    FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, CURRENT_ITEM.first, 1 );
                }
                break;
            }
            case 2: //Toss everything
                FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, CURRENT_ITEM.first );
                break;
            default:
                ret = 0;
                break;
        }
        initUI( );
        return ret;
    }

    u16 bagViewer::run( ) {
        _bagUI = new bagUI( );
        _currSelectedIdx = 0;
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

            if( _hasSprite && !( touch.px | touch.py ) ) { //Player drops the sprite at hand
                if( !_ranges[ start ].second.m_isHeld )
                    _bagUI->dropSprite( ( bag::bagType )FS::SAV->m_lstBag, start, CURRENT_ITEM );
                else
                    _bagUI->dropSprite( ( bag::bagType )FS::SAV->m_lstBag, MAX_ITEMS_PER_PAGE, { _ranges[ start ].second.m_item, 1 } );
                _hasSprite = false;

                if( start == curr ) continue; //Nothing happened

                u8 t = 0; for( ; !_ranges[ t ].second.m_isHeld; ++t ) { }
                if( curr != (u8) -1 ) {
                    if( _ranges[ start ].second.m_isHeld && _ranges[ curr ].second.m_isHeld ) { //Swap held items
                        if( !FS::SAV->m_pkmnTeam[ start - t ].isEgg( )
                            && !FS::SAV->m_pkmnTeam[ curr - t ].isEgg( ) )
                            std::swap( FS::SAV->m_pkmnTeam[ start - t ].m_boxdata.m_holdItem,
                                       FS::SAV->m_pkmnTeam[ curr - t ].m_boxdata.m_holdItem );
                    } else if( !_ranges[ start ].second.m_isHeld && !_ranges[ curr ].second.m_isHeld ) { //Swap bag items
                        FS::SAV->m_bag.swap( ( bag::bagType )FS::SAV->m_lstBag, FS::SAV->m_lstBagItem + start, FS::SAV->m_lstBagItem + curr );
                        std::swap( _ranges[ start ].second.m_item, _ranges[ curr ].second.m_item );
                        _currSelectedIdx = curr;
                        _bagUI->unselectItem( ( bag::bagType )FS::SAV->m_lstBag, start, _ranges[ start ].second.m_item );
                        _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM );
                        continue;
                    } else if( !_ranges[ start ].second.m_isHeld && _ranges[ curr ].second.m_isHeld ) { //Give/use item
                        pokemon& pkm = FS::SAV->m_pkmnTeam[ curr - t ];
                        auto itm = ItemList[ _ranges[ start ].second.m_item ];
                        bool result = false;
                        if( itm->m_itemType == item::TM_HM )
                            useItemOnPkmn( pkm, _ranges[ start ].second.m_item );
                        else if( itm->getEffectType( ) == item::itemEffectType::USE_ON_PKMN
                                 || itm->m_itemType == item::MEDICINE ) {
                            IO::choiceBox cb( 2, choices, 0, true );
                            sprintf( buffer, "Was tun mit %s?", itm->getDisplayName( true ).c_str( ) );
                            _bagUI->drawPkmnIcons( );
                            int res = cb.getResult( buffer, true, false );
                            initUI( );
                            switch( res ) {
                                case 0:
                                    result = giveItemToPkmn( pkm, _ranges[ start ].second.m_item );
                                    break;
                                case 1:
                                    result = useItemOnPkmn( pkm, _ranges[ start ].second.m_item );
                                    break;
                                default:
                                    break;
                            }
                        } else
                            result = giveItemToPkmn( pkm, _ranges[ start ].second.m_item );

                        if( result )
                            FS::SAV->m_bag.erase( ( bag::bagType )FS::SAV->m_lstBag, _ranges[ start ].second.m_item, 1 );
                    }
                } else if( _ranges[ start ].second.m_isHeld && ( curr == (u8) -1 || !_ranges[ curr ].second.m_isHeld ) ) { //Take item
                    takeItemFromPkmn( FS::SAV->m_pkmnTeam[ start - t ] );
                }
                initUI( );
            } else if( !handleSomeInput( touch, pressed ) )
                break;
            else if( GET_AND_WAIT( KEY_A ) ) {
                u16 res = handleSelection( );
                if( res & 2 )
                    return ( res >> 2 );
            }

            bool rangeChanged = false;
            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE( touch, i.first ) ) {
                    if( !_hasSprite ) {
                        u8 c = 0;
                        loop( ) {
                            scanKeys( );
                            swiWaitForVBlank( );
                            touchRead( &touch );
                            if( ( !touch.px && !touch.py ) || c++ == TRESHOLD ) {
                                if( !touch.px && !touch.py && _currSelectedIdx == j ) {
                                    u16 res = handleSelection( );
                                    if( res & 2 )
                                        return ( res >> 2 );
                                    break;
                                }
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
                                if( c >= TRESHOLD ) {
                                    if( !i.second.m_isHeld )
                                        _hasSprite = _bagUI->getSprite( j, CURRENT_ITEM );
                                    else {
                                        u8 vl = j;
                                        if( j < MAX_ITEMS_PER_PAGE )
                                            vl += MAX_ITEMS_PER_PAGE - FS::SAV->m_bag.size( ( bag::bagType )FS::SAV->m_lstBag );
                                        _hasSprite = _bagUI->getSprite( vl, { i.second.m_item, 1 } );
                                    }
                                    if( _hasSprite ) {
                                        _bagUI->updateSprite( touch );
                                        start = curr = j;
                                    }
                                }
                                break;
                            }
                            if( !IN_RANGE( touch, i.first ) )
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
        return 0;
    }

    u16 bagViewer::getItem( context p_context ) {
        _bagUI = new bagUI( );
        _currSelectedIdx = 0;
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