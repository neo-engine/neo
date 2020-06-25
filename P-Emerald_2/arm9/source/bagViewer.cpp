/*
Pokémon neo
------------------------------

file        : bagViewer.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
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

#include "animations.h"
#include "bagViewer.h"
#include "choiceBox.h"
#include "defines.h"
#include "itemNames.h"
#include "messageBox.h"
#include "saveGame.h"
#include "uio.h"
#include "yesNoBox.h"

#include <nds.h>

#include <algorithm>

namespace BAG {
#define TRESHOLD 20
    bagViewer::bagViewer( pokemon* p_playerTeam, context p_context ) {
        _context = p_context;
        _playerTeam = p_playerTeam;
        _bagUI           = new bagUI( _playerTeam );

        _currSelectedIdx = 0;
        initView( );
    }

    void bagViewer::initView( ) {
        _currentViewEnd = _currentViewStart = SAVE::SAV.getActiveFile( ).m_lstBagItem;
        _view = std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>>( );

        auto curBg = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;

        if( (bag::bagType) curBg == bag::TM_HM
                || (bag::bagType) curBg == bag::KEY_ITEMS ) {
            if( _context != context::NO_SPECIAL_CONTEXT ) {
                return;
            }
        }

        auto sz = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBg );

        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            for( bool done = false; !done; ) {
                auto ci = SAVE::SAV.getActiveFile( ).m_bag( (bag::bagType) curBg, _currentViewEnd );

                done = isAllowed( ci.first ) ;
                if( done ) {
                    auto idata = ITEM::getItemData( ci.first );
                    _view.push_back( std::pair( ci, idata ) );
                }
                _currentViewEnd = ( _currentViewEnd + 1 ) % sz;
            }
            if( _currentViewEnd == _currentViewStart ) {
                // bag contains less than MAX_ITEMS_PER_PAGE items.
                break;
            }
        }

    }

    bool bagViewer::isAllowed( u16 p_itemId ) {
        if( _context == NO_SPECIAL_CONTEXT ) {
            return true;
        } else if( _context == BATTLE || _context == WILD_BATTLE ) {
            constexpr u16 BATTLE_ITEMS[ 75 ] = {
                I_POTION, I_ANTIDOTE, I_BURN_HEAL, I_ICE_HEAL, I_AWAKENING,
                I_PARALYZE_HEAL, I_FULL_RESTORE, I_MAX_POTION, I_HYPER_POTION, I_SUPER_POTION,
                I_FULL_HEAL, I_REVIVE, I_MAX_REVIVE, I_FRESH_WATER, I_SODA_POP,
                I_LEMONADE, I_MOOMOO_MILK, I_ENERGY_POWDER, I_ENERGY_ROOT, I_HEAL_POWDER,
                I_REVIVAL_HERB, I_ETHER, I_MAX_ETHER, I_ELIXIR, I_MAX_ELIXIR,
                I_LAVA_COOKIE, I_BERRY_JUICE, I_OLD_GATEAU, I_GUARD_SPEC, I_DIRE_HIT,
                I_X_ATTACK, I_X_DEFENSE, I_X_SPEED, I_X_ACCURACY, I_X_SP_ATK,
                I_X_SP_DEF, I_POKE_DOLL, I_FLUFFY_TAIL, I_BLUE_FLUTE, I_YELLOW_FLUTE,
                I_RED_FLUTE, I_SWEET_HEART, I_CHERI_BERRY, I_CHESTO_BERRY, I_PECHA_BERRY,
                I_RAWST_BERRY, I_ASPEAR_BERRY, I_LEPPA_BERRY, I_ORAN_BERRY, I_PERSIM_BERRY,

                I_LUM_BERRY, I_SITRUS_BERRY, I_POKE_TOY, I_CASTELIACONE, I_LUMIOSE_GALETTE,
                I_SHALOUR_SABLE, I_BIG_MALASADA, I_PUMKIN_BERRY, I_DRASH_BERRY, I_CHRO_BERRY,
                I_PEWTER_CRUNCHIES, I_NION_BERRY, I_RIE_BERRY, I_GARC_BERRY,
            };

            for( u8 i = 0; i < 75; ++i ) {
                if( p_itemId && p_itemId == BATTLE_ITEMS[ i ] ) { return true; }
            }
            if( _context == WILD_BATTLE ) {
                constexpr u16 POKEBALLS[ 30 ] = {
                    I_MASTER_BALL, I_ULTRA_BALL, I_GREAT_BALL, I_POKE_BALL, I_SAFARI_BALL,
                    I_NET_BALL, I_DIVE_BALL, I_NEST_BALL, I_REPEAT_BALL, I_TIMER_BALL,
                    I_LUXURY_BALL, I_PREMIER_BALL, I_DUSK_BALL, I_HEAL_BALL, I_QUICK_BALL,
                    I_CHERISH_BALL, I_FAST_BALL, I_LEVEL_BALL, I_LURE_BALL, I_HEAVY_BALL,
                    I_LOVE_BALL, I_FRIEND_BALL, I_MOON_BALL, I_SPORT_BALL, I_PARK_BALL,
                    I_DREAM_BALL, I_BEAST_BALL,
                };

                for( u8 i = 0; i < 30; ++i ) {
                    if( p_itemId && p_itemId == POKEBALLS[ i ] ) { return true; }
                }
            }
            return false;
        } else if( _context == GIVE_TO_PKMN ) {
            constexpr u16 NO_GIVE_ITEMS[ 30 ] = {
                I_TEA, I_MYSTICTICKET, I_AUTOGRAPH, I_BIKE_VOUCHER, I_FAME_CHECKER,
                I_BERRY_POUCH, I_TEACHY_TV, I_POWDER_JAR, I_EXP_SHARE, I_LOCK_CAPSULE,
                I_ENIGMA_STONE, I_LIBERTY_PASS, I_PASS_ORB, I_PROP_CASE, I_DRAGON_SKULL,
                I_HOLO_CASTER, I_PROF_S_LETTER, I_ROLLER_SKATES, I_POKE_FLUTE, I_PRISON_BOTTLE,
                I_MEGA_CUFF, I_ENIGMATIC_CARD, I_SECRET_KEY2, I_SS_TICKET3, I_SILPH_SCOPE,
                I_PARCEL2, I_CARD_KEY2, I_GOLD_TEETH, I_LIFT_KEY,
            };

            for( u8 i = 0; i < 30; ++i ) {
                if( p_itemId == NO_GIVE_ITEMS[ i ] ) { return false; }
            }

            if( p_itemId >= I_TM01 && p_itemId <= I_BLACK_APRICORN ) { return false; }
            if( p_itemId >= I_PHOTO_ALBUM && p_itemId <= I_DATA_CARD_27 ) { return false; }
            if( p_itemId > I_CASTELIACONE && p_itemId <= I_REVEAL_GLASS ) { return false; }
            if( p_itemId >= I_SPRINKLOTAD && p_itemId <= I_TRAVEL_TRUNK ) { return false; }
            if( p_itemId >= I_LOOKER_TICKET && p_itemId <= I_METEORITE2 ) { return false; }
            if( p_itemId >= I_METEORITE3 && p_itemId <= I_PIKANIUM_Z ) { return false; }
            if( p_itemId >= I_Z_RING && p_itemId <= I_RAINBOW_PASS ) { return false; }
            if( p_itemId >= I_RED_NECTAR && p_itemId <= I_MOON_FLUTE ) { return false; }
            if( p_itemId > I_GRASSY_SEED && p_itemId < I_NULL_PLATE ) { return false; }
            if( p_itemId > I_NULL_PLATE && p_itemId < I_PEWTER_CRUNCHIES ) { return false; }
            if( p_itemId >= I_SOLGANIUM_Z && p_itemId < I_HEALTH_CANDY ) { return false; }
            if( p_itemId > I_QUICK_CANDY_0 && p_itemId < I_RUSTED_SWORD ) { return false; }
            if( p_itemId > I_EXP_CANDY_XL && p_itemId < I_LONELY_MINT ) { return false; }
            if( p_itemId >= I_HI_TECH_EARBUDS && p_itemId <= I_ROTOM_CATALOG) { return false; }
        }
        return false;
    }

    void bagViewer::initUI( ) {
        _bagUI->init( );
        _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view );

        if( _view.size( ) ) {
            _bagUI->selectItem( _currSelectedIdx, _view[ _currSelectedIdx ].first,
                    &_view[ _currSelectedIdx ].second );
        }
    }

    bool bagViewer::useItemOnPkmn( pokemon& p_pokemon, u16 p_itemId, ITEM::itemData* p_data ) {
        if( !p_pokemon.m_boxdata.m_speciesId || p_pokemon.isEgg( ) ) return false;
        if( p_data->m_itemType == ITEM::ITEMTYPE_TM ) {
            u16 currMv = p_data->m_param2;

            p_pokemon.learnMove( currMv );

            // 0: HM, 1: TM, 2: TR
            return p_data->m_effect == 2;
        }

        char buffer[ 100 ];
        if( p_data->m_itemType == ITEM::ITEMTYPE_MEDICINE
            || p_data->m_itemType == ITEM::ITEMTYPE_FORMECHANGE ) {

            u8 oldLv = p_pokemon.m_level;

            if( ITEM::use( p_itemId, *p_data, p_pokemon, [&]( u8 p_message ) -> u8 {
                    if( p_message == 1 ) { // player should select 1 move
                        IO::choiceBox cb( p_pokemon, 0 );
                        _bagUI->drawPkmnIcons( );
                        u8 res = cb.getResult( GET_STRING( 49 ), false, false );
                        initUI( );
                        return 1 << res;
                    }
                    if( p_message == 0xFF ) { // Sacred Ash
                        bool change = false;

                    for( u8 i = 0; i < 6; ++i ) {
                        if( !_playerTeam[ i ].m_boxdata.m_speciesId ) {
                            break;
                        }
                        change |= _playerTeam[ i ].heal( );
                    }
                    return change;
                }
                return 0;
            } ) ) {
                snprintf( buffer, 99, GET_STRING( 50 ),
                          ITEM::getItemName( p_itemId, CURRENT_LANGUAGE ).c_str( ),
                          p_pokemon.m_boxdata.m_name );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                IO::messageBox( buffer, false );

                SAVE::SAV.getActiveFile( ).m_bag.erase(
                    (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, p_itemId, 1 );

                // Check for evolution
                if( p_pokemon.m_level != oldLv && p_pokemon.canEvolve( ) ) {
                    u16 oldsp = p_pokemon.getSpecies( );
                    u8 oldfm = p_pokemon.getForme( );
                    p_pokemon.evolve( );
                    u16 newsp = p_pokemon.getSpecies( );
                    u8 newfm = p_pokemon.getForme( );

                    IO::ANIM::evolvePkmn( oldsp, oldfm, newsp, newfm, p_pokemon.isShiny( ),
                            p_pokemon.isFemale( ), false );
                }
                return false;
            }
            IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
            IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
            IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
            IO::messageBox( GET_STRING( 53 ), false );
            return false;
        }

        if( p_data->m_itemType == ITEM::ITEMTYPE_EVOLUTION ) {
            // Use the item on the PKMN
            if( !p_pokemon.canEvolve( p_itemId, EVOMETHOD_ITEM ) ) {
                IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                IO::messageBox( GET_STRING( 53 ), false );
                return false;
            } else {
                    u16 oldsp = p_pokemon.getSpecies( );
                    u8 oldfm = p_pokemon.getForme( );
                    p_pokemon.evolve( p_itemId, EVOMETHOD_ITEM );
                    u16 newsp = p_pokemon.getSpecies( );
                    u8 newfm = p_pokemon.getForme( );

                    IO::ANIM::evolvePkmn( oldsp, oldfm, newsp, newfm, p_pokemon.isShiny( ),
                            p_pokemon.isFemale( ), false );
            }
            return false;
        }
        return false;
    }

    bool bagViewer::giveItemToPkmn( pokemon& p_pokemon, u16 p_itemId ) {
        if( p_pokemon.getItem( ) ) {
            IO::yesNoBox yn( false );
            char         buffer[ 100 ];
            snprintf( buffer, 99, GET_STRING( 54 ), p_pokemon.m_boxdata.m_name,
                      ITEM::getItemName( p_pokemon.getItem( ), CURRENT_LANGUAGE ).c_str( ) );
            if( !yn.getResult( buffer ) ) return false;

            takeItemFromPkmn( p_pokemon );
        }
        p_pokemon.giveItem( p_itemId );
        _bagUI->drawPkmnIcons( );
        return true;
    }

    void bagViewer::takeItemFromPkmn( pokemon& p_pokemon ) {
        if( p_pokemon.isEgg( ) || !p_pokemon.getItem( ) ) return;
        auto currBgType = toBagType( ITEM::getItemData( p_pokemon.getItem( ) ).m_itemType );
        SAVE::SAV.getActiveFile( ).m_bag.insert( currBgType, p_pokemon.takeItem( ), 1 );
        _bagUI->drawPkmnIcons( );
    }

    bool bagViewer::confirmChoice( u16 p_targetItem ) {
        IO::yesNoBox yn( false );
        char         buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( 56 ), ITEM::getItemName( p_targetItem ).c_str( ) );
        if( !yn.getResult( buffer ) ) { return false; }

        return true;
    }

    bool bagViewer::handleSomeInput( bool p_allowSort ) {
        auto curBg = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
        auto curBgsz = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

        if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X )
            || ( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) ) {
            return false;
        } /*else if( p_allowSort && GET_AND_WAIT( KEY_SELECT ) ) {
            SAVE::SAV.getActiveFile( ).m_bag.sort(
                     (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag );
            _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                           SAVE::SAV.getActiveFile( ).m_lstBagItem );
            if( SAVE::SAV.getActiveFile( ).m_bag.size(
                    (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag ) ) {
                auto nw = ITEM::getItemData( CURRENT_ITEM.first );
                _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM, nw );
            }
        } */ else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
             _currSelectedIdx = 0;
            SAVE::SAV.getActiveFile( ).m_lstBag = ( curBg + BAG_CNT - 1 ) % BAG_CNT;
            curBg = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
            curBgsz = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

            if( curBgsz ) {
                SAVE::SAV.getActiveFile( ).m_lstBagItem %= curBgsz;
            } else {
                SAVE::SAV.getActiveFile( ).m_lstBagItem = 0;
            }
            initView( );
            _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                    _view, _currSelectedIdx );

            if( curBgsz ) {
                auto idata =  currentItem( ).second;
                _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
            }
            cooldown = COOLDOWN_COUNT;
        } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
            _currSelectedIdx = 0;
            SAVE::SAV.getActiveFile( ).m_lstBag = ( curBg + 1 ) % BAG_CNT;
            curBg = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
            curBgsz = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

            if( curBgsz ) {
                SAVE::SAV.getActiveFile( ).m_lstBagItem %= curBgsz;
            } else {
                SAVE::SAV.getActiveFile( ).m_lstBagItem = 0;
            }
            initView( );
            _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                    _view, _currSelectedIdx );

            if( curBgsz ) {
                auto idata =  currentItem( ).second;
                _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
            }
            cooldown = COOLDOWN_COUNT;
        } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
            if( !curBgsz ) {
                cooldown = COOLDOWN_COUNT;
                return true;
            }

            if( _currSelectedIdx < _view.size( ) - 1 ) {
                _currSelectedIdx++;
            } else {
                std::rotate( _view.begin( ), _view.begin( ) + 1, _view.end( ) );

                if( _currentViewStart != _currentViewEnd ) {
                    SAVE::SAV.getActiveFile( ).m_lstBagItem
                        = ( SAVE::SAV.getActiveFile( ).m_lstBagItem + 1 ) % curBgsz;
                    _currentViewStart = ( _currentViewStart + 1 ) % curBgsz;

                    for( bool done = false; !done; ) {
                        auto ci = SAVE::SAV.getActiveFile( ).m_bag( (bag::bagType) curBg,
                                _currentViewEnd );

                        done = isAllowed( ci.first ) ;
                        _currentViewEnd = ( _currentViewEnd + 1 ) % curBgsz;
                        if( done ) {
                            auto idata = ITEM::getItemData( ci.first );
                            _view[ _currSelectedIdx ] = std::pair( ci, idata );
                        }
                    }
                }
                _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                               _view, _currSelectedIdx );
            }
            auto idata =  currentItem( ).second;
            _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
            cooldown = COOLDOWN_COUNT;
        } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
            if( !curBgsz ) {
                cooldown = COOLDOWN_COUNT;
                return true;
            }

            if( _currSelectedIdx > 0 ) {
                _currSelectedIdx--;
            } else {
                std::rotate( _view.rbegin( ), _view.rbegin( ) + 1, _view.rend( ) );
                if( _currentViewStart != _currentViewEnd ) {
                    _currentViewEnd = ( _currentViewEnd + curBgsz - 1 ) % curBgsz;
                    for( bool done = false; !done; ) {
                        _currentViewStart = ( _currentViewStart + curBgsz - 1 ) % curBgsz;
                        auto ci = SAVE::SAV.getActiveFile( ).m_bag( (bag::bagType) curBg,
                                _currentViewStart );
                        done = isAllowed( ci.first ) ;

                        SAVE::SAV.getActiveFile( ).m_lstBagItem
                            = ( SAVE::SAV.getActiveFile( ).m_lstBagItem + curBgsz - 1 ) % curBgsz;
                        if( done ) {
                            auto idata = ITEM::getItemData( ci.first );
                            _view[ _currSelectedIdx ] = std::pair( ci, idata );
                        }
                    }
                }
                _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                               _view, _currSelectedIdx );
            }
            auto idata =  currentItem( ).second;
            _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
            cooldown = COOLDOWN_COUNT;
        } else if( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 44, SCREEN_HEIGHT - 10, 16 ) ) {
            if( !curBgsz ) return true;
            SAVE::SAV.getActiveFile( ).m_lstBagItem
                = ( SAVE::SAV.getActiveFile( ).m_lstBagItem + MAX_ITEMS_PER_PAGE - 1 ) % curBgsz;
            initView( );
            _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                               _view, _currSelectedIdx );
            auto idata =  currentItem( ).second;
            _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
        }  else if( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 76, SCREEN_HEIGHT - 10, 16 ) ) {
            if( !curBgsz ) return true;
            SAVE::SAV.getActiveFile( ).m_lstBagItem
                = ( SAVE::SAV.getActiveFile( ).m_lstBagItem + curBgsz - MAX_ITEMS_PER_PAGE + 1 )
                % curBgsz;
            initView( );
            _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                               _view, _currSelectedIdx );
            auto idata =  currentItem( ).second;
            _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
        }

        for( u8 i = 0; i < 5; ++i )
            if( i != SAVE::SAV.getActiveFile( ).m_lstBag && !_hasSprite
                && GET_AND_WAIT_C( 26 * i + 13, 3 + 13, 13 ) ) {
                _currSelectedIdx = 0;
                SAVE::SAV.getActiveFile( ).m_lstBag = i;
                curBg = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
                curBgsz = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

                if( curBgsz ) {
                    SAVE::SAV.getActiveFile( ).m_lstBagItem %= curBgsz;
                } else {
                    SAVE::SAV.getActiveFile( ).m_lstBagItem = 0;
                }
                _currentViewEnd = _currentViewStart = SAVE::SAV.getActiveFile( ).m_lstBagItem;

                initView( );
                _ranges = _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                        _view, _currSelectedIdx );

                if( curBgsz ) {
                    auto idata =  currentItem( ).second;
                    _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
                }
                break;
            }
        return true;
    }

    // returns: 0 if nothing happened; 1 if the item got consumed and 2 if the bag is exited.
    // If an item has yet to be used the 14 highest bits contain its id
    u16 bagViewer::handleSelection( ) {
        return 0;

        /*
        ITEM::itemData data = ITEM::getItemData( CURRENT_ITEM.first );

        if( data.m_itemType == ITEM::ITEMTYPE_TM ) return 0;

        const char* choices[ 5 ] = {GET_STRING( 44 ), GET_STRING( 45 ), GET_STRING( 46 ),
                                    GET_STRING( 47 ), GET_STRING( 48 )};

        IO::choiceBox cb( 1 + ITEM::isUsable( CURRENT_ITEM.first ),
                          choices + 2
                              + ( data.m_itemType != ITEM::ITEMTYPE_KEYITEM
                                  && data.m_itemType != ITEM::ITEMTYPE_FORMECHANGE ),
                          0, true );
        _bagUI->drawPkmnIcons( );
        char buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( 57 ),
                  ITEM::getItemName( CURRENT_ITEM.first, CURRENT_LANGUAGE ).c_str( ) );
        int res = cb.getResult( buffer, true, false );

        u16 ret = data.m_itemType != ITEM::ITEMTYPE_KEYITEM
                  && data.m_itemType != ITEM::ITEMTYPE_FORMECHANGE;
        if( res != -1 ) { res += ret; }

        switch( res ) {
        case 0: // Register
        {
            if( SAVE::SAV.getActiveFile( ).m_registeredItem != CURRENT_ITEM.first )
                SAVE::SAV.getActiveFile( ).m_registeredItem = CURRENT_ITEM.first;
            else
                SAVE::SAV.getActiveFile( ).m_registeredItem = 0;
            break;
        }
        case 1: // Use
        {
            if( !ITEM::isUsable( CURRENT_ITEM.first ) ) {
                initUI( );
                IO::Oam->oamBuffer[ FWD_ID ].isHidden  = true;
                IO::Oam->oamBuffer[ BACK_ID ].isHidden = true;
                IO::Oam->oamBuffer[ BWD_ID ].isHidden  = true;
                IO::messageBox( GET_STRING( 58 ), false );
                ret = 0;
                break;
            }

            SAVE::SAV.getActiveFile( )
                .m_lstUsedItems[ SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx ]
                = CURRENT_ITEM.first;
            SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx
                = ( SAVE::SAV.getActiveFile( ).m_lstUsedItemsIdx + 1 ) % 5;

            if( !ITEM::use( CURRENT_ITEM.first, true ) )
                ret = 2 | ( CURRENT_ITEM.first << 2 );
            else {
                initUI( );
                ITEM::use( CURRENT_ITEM.first );
                if( data.m_itemType != ITEM::ITEMTYPE_KEYITEM
                        && data.m_itemType != ITEM::ITEMTYPE_FORMECHANGE )
                    SAVE::SAV.getActiveFile( ).m_bag.erase(
                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, CURRENT_ITEM.first, 1 );
            }
            break;
        }
        case 2: // Toss everything
            SAVE::SAV.getActiveFile( ).m_bag.erase(
                (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, CURRENT_ITEM.first );
            break;
        default:
            ret = 0;
            break;
        }
        initUI( );
        return ret;
        */
    }

    u16 bagViewer::run( ) {
        _currSelectedIdx = 0;
        initUI( );
        _hasSprite = false;

        u8            curr = -1, start = -1;
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held = keysHeld( );

            if( _hasSprite ) _bagUI->updateSprite( touch );

            /*
            if( _hasSprite && !( touch.px | touch.py ) ) { // Player drops the sprite at hand
                if( !_ranges[ start ].second.m_isHeld ) {
                    _bagUI->dropSprite( start, currentItem( )->first, currentItem( )->second );
                } else {
                    auto data = ITEM::getItemData( _ranges[ start ].second.m_item );
                    _bagUI->dropSprite( MAX_ITEMS_PER_PAGE, {_ranges[ start ].second.m_item, 1},
                                        data );
                }
                _hasSprite = false;

                if( start == curr ) continue; // Nothing happened

                u8 t = 0;
                for( ; !_ranges[ t ].second.m_isHeld; ++t ) {}
                if( curr != (u8) -1 ) {
                    if( _ranges[ start ].second.m_isHeld
                        && _ranges[ curr ].second.m_isHeld ) { // Swap held items
                        if( !_playerTeam[ start - t ].isEgg( )
                            && !_playerTeam[ curr - t ].isEgg( ) ) {
                            auto oldItem = _playerTeam[ start - t ]
                                .getItem( );
                            _playerTeam[ start - t ].giveItem( _playerTeam[ curr - t ].getItem( ) );
                            _playerTeam[ curr - t ].giveItem( oldItem );
                            _bagUI->drawPkmnIcons( );
                        }
                    } else if( !_ranges[ start ].second.m_isHeld
                               && !_ranges[ curr ].second.m_isHeld ) { // Swap bag items
                        SAVE::SAV.getActiveFile( ).m_bag.swap(
                            (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                            SAVE::SAV.getActiveFile( ).m_lstBagItem + start,
                            SAVE::SAV.getActiveFile( ).m_lstBagItem + curr );
                        std::swap( _ranges[ start ].second.m_item, _ranges[ curr ].second.m_item );
                        _currSelectedIdx = curr;

                        auto old = ITEM::getItemData( _ranges[ start ].second.m_item );
                        auto nw  = ITEM::getItemData( CURRENT_ITEM.first );

                        _bagUI->selectItem( start, {_ranges[ start ].second.m_item, 0}, old );
                        _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM, nw );
                        continue;
                    } else if( !_ranges[ start ].second.m_isHeld
                               && _ranges[ curr ].second.m_isHeld ) { // Give/use item
                        pokemon& pkm    = _playerTeam[ curr - t ];

                        auto data   = ITEM::getItemData( _ranges[ start ].second.m_item );
                        bool result = false;
                        if( data.m_itemType == ITEM::ITEMTYPE_TM )
                            useItemOnPkmn( pkm, _ranges[ start ].second.m_item, data );
                        else if( ( data.m_itemType & 15 ) == ITEM::ITEMTYPE_MEDICINE
                                 || data.m_itemType == ITEM::ITEMTYPE_FORMECHANGE
                                 || data.m_itemType == ITEM::ITEMTYPE_EVOLUTION ) {
                            const char* choices[ 5 ]
                                = {GET_STRING( 44 ), GET_STRING( 45 ), GET_STRING( 46 ),
                                   GET_STRING( 47 ), GET_STRING( 48 )};
                            IO::choiceBox cb( 2, choices, 0, true );
                            char          buffer[ 100 ];
                            snprintf( buffer, 99, GET_STRING( 57 ),
                                      ITEM::getItemName( _ranges[ start ].second.m_item,
                                                         CURRENT_LANGUAGE )
                                          .c_str( ) );
                            _bagUI->drawPkmnIcons( );
                            int res = cb.getResult( buffer, true, false );
                            initUI( );
                            switch( res ) {
                            case 0:
                                result = giveItemToPkmn( pkm, _ranges[ start ].second.m_item );
                                break;
                            case 1:
                                result = useItemOnPkmn( pkm, _ranges[ start ].second.m_item, data );
                                break;
                            default:
                                break;
                            }
                        } else
                            result = giveItemToPkmn( pkm, _ranges[ start ].second.m_item );

                        if( result )
                            SAVE::SAV.getActiveFile( ).m_bag.erase(
                                (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                _ranges[ start ].second.m_item, 1 );
                    }
                } else if( _ranges[ start ].second.m_isHeld
                           && ( curr == (u8) -1
                                || !_ranges[ curr ].second.m_isHeld ) ) { // Take item
                    takeItemFromPkmn( _playerTeam[ start - t ] );
                }
                initUI( );
            } else */ if( !handleSomeInput( ) )
                break;
            else if( GET_AND_WAIT( KEY_A ) ) {
                u16 res = handleSelection( );
                if( res & 2 ) return ( res >> 2 );
            }

            /*
            bool rangeChanged = false;
            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE_I( touch, i.first ) ) {
                    if( !_hasSprite ) {
                        u8 c = 0;
                        loop( ) {
                            scanKeys( );
                            swiWaitForVBlank( );
                            touchRead( &touch );
                            if( ( !touch.px && !touch.py ) || c++ == TRESHOLD ) {
                                if( !touch.px && !touch.py && _currSelectedIdx == j ) {
                                    u16 res = handleSelection( );
                                    if( res & 2 ) return ( res >> 2 );
                                    break;
                                }
                                if( !i.second.m_isHeld || i.second.m_item ) {
                                    if( SAVE::SAV.getActiveFile( ).m_bag.size(
                                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag ) ) {
//                                        auto nw = ITEM::getItemData( CURRENT_ITEM.first );
//                                        _bagUI->unselectItem(
//                                            (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
//                                            _currSelectedIdx, CURRENT_ITEM.first, nw );
                                    } else { // the current bag is empty
//                                        ITEM::itemData empty = {0, 0, 0, 0, 0, 0, 0};
//                                        _bagUI->unselectItem(
//                                            (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
//                                            MAX_ITEMS_PER_PAGE, 0, empty );
                                    }
                                }
                                if( !i.second.m_isHeld ) {
                                    _currSelectedIdx = j;
                                    auto nw          = ITEM::getItemData( CURRENT_ITEM.first );
                                    _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM, nw );
                                } else if( i.second.m_item ) {
                                    u8 vl = j;
                                    if( j < MAX_ITEMS_PER_PAGE )
                                        vl += MAX_ITEMS_PER_PAGE
                                              - SAVE::SAV.getActiveFile( ).m_bag.size(
                                                    (bag::bagType) SAVE::SAV.getActiveFile( )
                                                        .m_lstBag );
                                    auto nw = ITEM::getItemData( i.second.m_item );
                                    _bagUI->selectItem( vl, {i.second.m_item, 1}, nw );
                                }
                                if( c >= TRESHOLD ) {
                                    if( !i.second.m_isHeld ) {
                                        auto nw    = ITEM::getItemData( CURRENT_ITEM.first );
                                        _hasSprite = _bagUI->getSprite( j, CURRENT_ITEM, nw );
                                    } else {
                                        u8 vl = j;
                                        if( j < MAX_ITEMS_PER_PAGE )
                                            vl += MAX_ITEMS_PER_PAGE
                                                  - SAVE::SAV.getActiveFile( ).m_bag.size(
                                                        (bag::bagType) SAVE::SAV.getActiveFile( )
                                                            .m_lstBag );
                                        auto nw = ITEM::getItemData( i.second.m_item );
                                        _hasSprite
                                            = _bagUI->getSprite( vl, {i.second.m_item, 1}, nw );
                                    }
                                    if( _hasSprite ) {
                                        _bagUI->updateSprite( touch );
                                        start = curr = j;
                                    }
                                }
                                break;
                            }
                            if( !IN_RANGE_I( touch, i.first ) ) break;
                        }
                    } else {
                        curr         = j;
                        rangeChanged = true;
                    }
                }
            }
    */
      //      if( !rangeChanged ) curr = -1;
            swiWaitForVBlank( );

        }
        return 0;
    }

    u16 bagViewer::getItem( context p_context ) {
        /*
        _currSelectedIdx = 0;
        initUI( );
        _hasSprite = false;

        touchPosition touch;
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( !handleSomeInput( touch, pressed ) ) return 0;
            if( GET_AND_WAIT( KEY_A ) ) {
                if( SAVE::SAV.getActiveFile( ).m_bag.empty(
                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag ) )
                    continue;
                u16 targetItem = CURRENT_ITEM.first;
                if( targetItem && confirmChoice( p_context, targetItem ) ) {
                    if( p_context != context::BATTLE )
                        SAVE::SAV.getActiveFile( ).m_bag.erase(
                            (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, CURRENT_ITEM.first,
                            1 );
                    return targetItem;
                }
                initUI( );
            }

            for( u8 j = 0; j < _ranges.size( ); ++j ) {
                auto i = _ranges[ j ];
                if( IN_RANGE_I( touch, i.first ) ) {
                    u8 c = 0;
                    loop( ) {
                        scanKeys( );
                        swiWaitForVBlank( );
                        touchRead( &touch );
                        if( c++ == TRESHOLD ) { // Ask the player whether he is sure about the
                                                // choice
                            while( touch.px || touch.py ) {
                                scanKeys( );
                                swiWaitForVBlank( );
                                touchRead( &touch );
                            }
                            u16 targetItem = i.second.m_item;
                            if( !i.second.m_isHeld && confirmChoice( p_context, targetItem ) ) {
                                if( p_context != context::BATTLE )
                                    SAVE::SAV.getActiveFile( ).m_bag.erase(
                                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                        CURRENT_ITEM.first, 1 );
                                return targetItem;
                            } else {
                                initUI( );
                                break;
                            }
                        }
                        if( !touch.px && !touch.py ) {
                            if( !i.second.m_isHeld || i.second.m_item ) {
                                if( SAVE::SAV.getActiveFile( ).m_bag.size(
                                    (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag ) ) {
                                    auto nw = ITEM::getItemData( CURRENT_ITEM.first );
//                                    _bagUI->unselectItem(
//                                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
//                                        _currSelectedIdx, CURRENT_ITEM.first, nw );
                                } else { // the current bag is empty
//                                    ITEM::itemData empty = {0, 0, 0, 0, 0, 0, 0};
//                                    _bagUI->unselectItem(
//                                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
//                                        MAX_ITEMS_PER_PAGE, 0, empty );
                                }
                            }
                            if( !i.second.m_isHeld ) {
                                _currSelectedIdx = j;
                                auto nw          = ITEM::getItemData( CURRENT_ITEM.first );
                                _bagUI->selectItem( _currSelectedIdx, CURRENT_ITEM, nw );
                            } else if( i.second.m_item ) {
                                u8 vl = j;
                                if( j < MAX_ITEMS_PER_PAGE )
                                    vl += MAX_ITEMS_PER_PAGE
                                          - SAVE::SAV.getActiveFile( ).m_bag.size(
                                                (bag::bagType) SAVE::SAV.getActiveFile( )
                                                    .m_lstBag );
                                auto nw = ITEM::getItemData( i.second.m_item );
                                _bagUI->selectItem( vl, {i.second.m_item, 1}, nw );
                            }
                            break;
                        }
                        if( !IN_RANGE_I( touch, i.first ) ) break;
                    }
                }
            }
            swiWaitForVBlank( );
        }
    */
        return 0;
    }
} // namespace BAG
