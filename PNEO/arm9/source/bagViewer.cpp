/*
Pokémon neo
------------------------------

file        : bagViewer.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2021
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

#include "bagViewer.h"
#include "animations.h"
#include "choiceBox.h"
#include "defines.h"
#include "itemNames.h"
#include "saveGame.h"
#include "sound.h"
#include "uio.h"
#include "yesNoBox.h"

#include <nds.h>

#include <algorithm>

namespace BAG {
    // #define THRESHOLD 20
    bagViewer::bagViewer( pokemon* p_playerTeam, context p_context ) {
        _context    = p_context;
        _playerTeam = p_playerTeam;
        _bagUI      = new bagUI( _playerTeam );

        _currSelectedIdx = 0;
        initView( );
    }

    void bagViewer::initView( ) {
        auto curBg = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
        if( _context == CHOOSE_BERRY ) {
            curBg = ( bag::bagType )( ( SAVE::SAV.getActiveFile( ).m_lstBag = 3 ) );
        }
        if( _context == MOCK_BATTLE ) {
            curBg      = (bag::bagType) 0;
            auto idata = ITEM::getItemData( I_POKE_BALL );
            _view      = std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>>( );
            _view.push_back( std::pair( std::pair<u16, u16>{ I_POKE_BALL, 1 }, idata ) );
            return;
        }
        _currentViewEnd = _currentViewStart = SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ];
        _view = std::vector<std::pair<std::pair<u16, u16>, ITEM::itemData>>( );

        if( (bag::bagType) curBg == bag::TM_HM || (bag::bagType) curBg == bag::KEY_ITEMS ) {
            if( _context != context::NO_SPECIAL_CONTEXT && _context != context::SELL_ITEM ) {
                return;
            }
        }

        auto sz = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBg );
        if( !sz ) { return; }

        for( u8 i = 0; i < MAX_ITEMS_PER_PAGE; ++i ) {
            for( bool done = false; !done; ) {
                auto ci = SAVE::SAV.getActiveFile( ).m_bag( (bag::bagType) curBg, _currentViewEnd );

                done = isAllowed( ci.first );
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

    bool canGive( u16 p_itemId ) {
        constexpr u16 NO_GIVE_ITEMS[ 30 ] = {
            I_TEA,          I_MYSTIC_TICKET,  I_AUTOGRAPH,     I_BIKE_VOUCHER, I_FAME_CHECKER,
            I_BERRY_POUCH,  I_TEACHY_TV,      I_POWDER_JAR,    I_EXP_ALL,      I_LOCK_CAPSULE,
            I_ENIGMA_STONE, I_LIBERTY_PASS,   I_PASS_ORB,      I_PROP_CASE,    I_DRAGON_SKULL,
            I_HOLO_CASTER,  I_PROF_S_LETTER,  I_ROLLER_SKATES, I_POKE_FLUTE,   I_PRISON_BOTTLE,
            I_MEGA_CUFF,    I_ENIGMATIC_CARD, I_SECRET_KEY2,   I_SS_TICKET3,   I_SILPH_SCOPE,
            I_PARCEL2,      I_CARD_KEY2,      I_GOLD_TEETH,    I_LIFT_KEY,
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
        if( p_itemId >= I_HI_TECH_EARBUDS && p_itemId <= I_ROTOM_CATALOG ) { return false; }

        return true;
    }

    bool bagViewer::isAllowed( u16 p_itemId ) {
        if( _context == MOCK_BATTLE ) { return p_itemId == I_POKE_BALL; }
        if( _context == NO_SPECIAL_CONTEXT || _context == SELL_ITEM ) {
            return true;
        } else if( _context == BATTLE || _context == WILD_BATTLE ) {
            constexpr u16 BATTLE_ITEMS[ 75 ] = {
                I_POTION,          I_ANTIDOTE,      I_BURN_HEAL,        I_ICE_HEAL,
                I_AWAKENING,       I_PARALYZE_HEAL, I_FULL_RESTORE,     I_MAX_POTION,
                I_HYPER_POTION,    I_SUPER_POTION,  I_FULL_HEAL,        I_REVIVE,
                I_MAX_REVIVE,      I_FRESH_WATER,   I_SODA_POP,         I_LEMONADE,
                I_MOOMOO_MILK,     I_ENERGY_POWDER, I_ENERGY_ROOT,      I_HEAL_POWDER,
                I_REVIVAL_HERB,    I_ETHER,         I_MAX_ETHER,        I_ELIXIR,
                I_MAX_ELIXIR,      I_LAVA_COOKIE,   I_BERRY_JUICE,      I_OLD_GATEAU,
                I_GUARD_SPEC,      I_DIRE_HIT,      I_X_ATTACK,         I_X_DEFENSE,
                I_X_SPEED,         I_X_ACCURACY,    I_X_SP_ATK,         I_X_SP_DEF,
                I_POKE_DOLL,       I_FLUFFY_TAIL,   I_BLUE_FLUTE,       I_YELLOW_FLUTE,
                I_RED_FLUTE,       I_SWEET_HEART,   I_CHERI_BERRY,      I_CHESTO_BERRY,
                I_PECHA_BERRY,     I_RAWST_BERRY,   I_ASPEAR_BERRY,     I_LEPPA_BERRY,
                I_ORAN_BERRY,      I_PERSIM_BERRY,

                I_LUM_BERRY,       I_SITRUS_BERRY,  I_POKE_TOY,         I_CASTELIACONE,
                I_LUMIOSE_GALETTE, I_SHALOUR_SABLE, I_BIG_MALASADA,     I_PUMKIN_BERRY,
                I_DRASH_BERRY,     I_CHRO_BERRY,    I_PEWTER_CRUNCHIES, I_NION_BERRY,
                I_RIE_BERRY,       I_GARC_BERRY,
            };

            for( u8 i = 0; i < 75; ++i ) {
                if( p_itemId && p_itemId == BATTLE_ITEMS[ i ] ) { return true; }
            }
            if( _context == WILD_BATTLE ) {
                constexpr u16 POKEBALLS[ 30 ] = {
                    I_MASTER_BALL,  I_ULTRA_BALL,   I_GREAT_BALL, I_POKE_BALL,   I_SAFARI_BALL,
                    I_NET_BALL,     I_DIVE_BALL,    I_NEST_BALL,  I_REPEAT_BALL, I_TIMER_BALL,
                    I_LUXURY_BALL,  I_PREMIER_BALL, I_DUSK_BALL,  I_HEAL_BALL,   I_QUICK_BALL,
                    I_CHERISH_BALL, I_FAST_BALL,    I_LEVEL_BALL, I_LURE_BALL,   I_HEAVY_BALL,
                    I_LOVE_BALL,    I_FRIEND_BALL,  I_MOON_BALL,  I_SPORT_BALL,  I_PARK_BALL,
                    I_DREAM_BALL,   I_BEAST_BALL,
                };

                for( u8 i = 0; i < 30; ++i ) {
                    if( p_itemId && p_itemId == POKEBALLS[ i ] ) { return true; }
                }
            }
            return false;
        } else if( _context == GIVE_TO_PKMN ) {
            return canGive( p_itemId );
        } else if( _context == CHOOSE_BERRY ) {
            if( p_itemId >= I_CHERI_BERRY && p_itemId <= I_ROWAP_BERRY ) { return true; }
            if( p_itemId >= I_ROSELI_BERRY && p_itemId <= I_MARANGA_BERRY ) { return true; }
            if( p_itemId == I_NION_BERRY ) { return true; }
            if( p_itemId >= I_PUMKIN_BERRY && p_itemId <= I_EGGANT_BERRY ) { return true; }
        }
        return false;
    }

    void bagViewer::initUI( ) {
        _bagUI->init( );
        _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                             _currSelectedIdx );
    }

    u8 bagViewer::chooseMove( const boxPokemon* p_pokemon, u16 p_extraMove ) {
        IO::choiceBox cb  = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        auto          res = cb.getResult(
            [ & ]( u8 ) { return _bagUI->drawMoveChoice( p_pokemon, p_extraMove ); },
            [ & ]( u8 p_selection ) { _bagUI->selectMoveChoice( p_selection ); } );
        _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                             _currSelectedIdx );

        return res;
    }

    bool bagViewer::useItemOnPkmn( pokemon& p_pokemon, u16 p_itemId, ITEM::itemData* p_data ) {
        if( !p_pokemon.m_boxdata.m_speciesId || p_pokemon.isEgg( ) ) return false;
        if( p_data->m_itemType == ITEM::ITEMTYPE_TM ) {
            u16 currMv = p_data->m_param2;
            return p_pokemon.learnMove(
                       currMv,
                       [ & ]( const char* p_message ) {
                           _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                                _view, _currSelectedIdx );
                           _bagUI->printMessage( p_message );
                           waitForInteract( );
                       },
                       [ & ]( boxPokemon* p_pok, u16 p_extraMove ) {
                           return chooseMove( p_pok, p_extraMove );
                       },
                       [ & ]( const char* p_message ) {
                           IO::yesNoBox yn;
                           return yn.getResult(
                                      [ & ]( ) { return _bagUI->printYNMessage( p_message, 254 ); },
                                      [ & ]( IO::yesNoBox::selection p_sel ) {
                                          _bagUI->printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                                      } )
                                  == IO::yesNoBox::YES;
                       } )
                   && p_data->m_effect == 2;
        }

        char buffer[ 100 ];
        if( ( p_data->m_itemType & 15 ) == ITEM::ITEMTYPE_MEDICINE
            || p_data->m_itemType == ITEM::ITEMTYPE_FORMECHANGE ) {

            u8 oldLv = p_pokemon.m_level;

            if( ITEM::use( p_itemId, *p_data, p_pokemon, [ & ]( u8 p_message ) -> u8 {
                    if( p_message == 1 ) { // player should select 1 move
                        auto res = chooseMove( &p_pokemon.m_boxdata );
                        if( res < 4 ) { return 1 << res; }
                        return 0;
                    }
                    if( p_message == 0xFF ) { // Sacred Ash
                        bool change = false;

                        for( u8 i = 0; i < 6; ++i ) {
                            if( !_playerTeam[ i ].m_boxdata.m_speciesId ) { break; }
                            change |= _playerTeam[ i ].heal( );
                        }
                        return change;
                    }
                    return 0;
                } ) ) {
                snprintf( buffer, 99, GET_STRING( 50 ), ITEM::getItemName( p_itemId ).c_str( ) );
                _bagUI->printMessage( buffer );
                waitForInteract( );

                auto lstBg = SAVE::SAV.getActiveFile( ).m_lstBag;
                SAVE::SAV.getActiveFile( ).m_bag.erase( (bag::bagType) lstBg, p_itemId, 1 );
                if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ lstBg ]
                    == SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) lstBg ) ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ lstBg ] > 0 ) {
                        SAVE::SAV.getActiveFile( ).m_lstViewedItem[ lstBg ]--;
                    }
                }
                initView( );

                // Check for evolution
                if( p_pokemon.m_level != oldLv && p_pokemon.canEvolve( ) ) {
                    u16 oldsp = p_pokemon.getSpecies( );
                    u8  oldfm = p_pokemon.getForme( );
                    p_pokemon.evolve( );
                    u16 newsp = p_pokemon.getSpecies( );
                    u8  newfm = p_pokemon.getForme( );

                    IO::ANIM::evolvePkmn( oldsp, oldfm, newsp, newfm, p_pokemon.isShiny( ),
                                          p_pokemon.isFemale( ), false );
                    initUI( );
                }
                return _context == BATTLE || _context == WILD_BATTLE;
            }
            _bagUI->printMessage( GET_STRING( 53 ) );
            waitForInteract( );
            return false;
        }

        if( p_data->m_itemType == ITEM::ITEMTYPE_EVOLUTION ) {
            // Use the item on the PKMN
            if( !p_pokemon.canEvolve( p_itemId, EVOMETHOD_ITEM ) ) {
                _bagUI->printMessage( GET_STRING( 53 ) );
                waitForInteract( );
                return false;
            } else {
                u16 oldsp = p_pokemon.getSpecies( );
                u8  oldfm = p_pokemon.getForme( );
                p_pokemon.evolve( p_itemId, EVOMETHOD_ITEM );
                u16 newsp = p_pokemon.getSpecies( );
                u8  newfm = p_pokemon.getForme( );

                IO::ANIM::evolvePkmn( oldsp, oldfm, newsp, newfm, p_pokemon.isShiny( ),
                                      p_pokemon.isFemale( ), false );

                auto lstBg = SAVE::SAV.getActiveFile( ).m_lstBag;
                SAVE::SAV.getActiveFile( ).m_bag.erase( (bag::bagType) lstBg, p_itemId, 1 );
                if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ lstBg ]
                    == SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) lstBg ) ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ lstBg ] > 0 ) {
                        SAVE::SAV.getActiveFile( ).m_lstViewedItem[ lstBg ]--;
                    }
                }
                initView( );
                initUI( );
            }
            return false;
        }
        return false;
    }

    bool bagViewer::giveItemToPkmn( pokemon& p_pokemon, u16 p_itemId ) {
        if( p_pokemon.isEgg( ) ) { return false; }

        if( p_pokemon.getItem( ) ) {
            IO::yesNoBox yn;
            char         buffer[ 100 ];
            snprintf( buffer, 99, GET_STRING( 54 ), p_pokemon.m_boxdata.m_name );
            if( yn.getResult( [ & ]( ) { return _bagUI->printYNMessage( buffer, 254 ); },
                              [ & ]( IO::yesNoBox::selection p_sel ) {
                                  _bagUI->printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                              } )
                == IO::yesNoBox::NO )
                return false;

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

    u8 bagViewer::confirmChoice( u16 p_targetItem, ITEM::itemData* p_data ) {
        if( _context == BATTLE || _context == WILD_BATTLE ) {
            // Check if the item needs to be used on a pkmn
            if( ( p_data->m_itemType & 15 ) == ITEM::ITEMTYPE_MEDICINE ) {
                IO::choiceBox cb2    = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
                auto          tgpkmn = cb2.getResult(
                    [ & ]( u8 ) {
                        _bagUI->drawPkmnChoice( );
                        auto tmp = _bagUI->getPkmnInputTarget( );
                        tmp.push_back( _bagUI->getButtonInputTarget( IO::choiceBox::BACK_CHOICE ) );
                        return tmp;
                    },
                    [ & ]( u8 p_selection ) { _bagUI->selectPkmn( p_selection ); } );
                _bagUI->undrawPkmnChoice( );

                if( tgpkmn == IO::choiceBox::BACK_CHOICE ) { return false; }

                if( !useItemOnPkmn( _playerTeam[ tgpkmn ], p_targetItem, p_data ) ) {
                    _bagUI->selectPkmn( -1 );
                    return false;
                }
                _bagUI->selectPkmn( -1 );
                return 2;
            }
        }

        IO::yesNoBox yn;
        char         buffer[ 100 ];
        snprintf( buffer, 99, GET_STRING( 56 ), ITEM::getItemName( p_targetItem ).c_str( ) );

        if( _context == MOCK_BATTLE ) {
            // Just wait a couple of frames and return
            _bagUI->printYNMessage( buffer, 254 );
            _bagUI->printYNMessage( 0, false );
            for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }
            return true;
        }

        if( yn.getResult( [ & ]( ) { return _bagUI->printYNMessage( buffer, 254 ); },
                          [ & ]( IO::yesNoBox::selection p_sel ) {
                              _bagUI->printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                          } )
            == IO::yesNoBox::NO )
            return false;

        return true;
    }

    void bagViewer::selectItem( s8 p_index ) {
        auto curBg   = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
        auto curBgsz = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

        if( p_index >= 0 && p_index < s8( _view.size( ) ) ) {
            _currSelectedIdx = u8( p_index );
            auto idata       = currentItem( ).second;
            _bagUI->selectItem( _currSelectedIdx, currentItem( ).first, &idata );
        } else if( p_index < 0 ) {
            _currSelectedIdx = 0;

            while( p_index++ < 0 ) {
                std::rotate( _view.rbegin( ), _view.rbegin( ) + 1, _view.rend( ) );
                if( _currentViewStart != _currentViewEnd ) {
                    _currentViewEnd = ( _currentViewEnd + curBgsz - 1 ) % curBgsz;
                    for( bool done = false; !done; ) {
                        _currentViewStart = ( _currentViewStart + curBgsz - 1 ) % curBgsz;
                        auto ci           = SAVE::SAV.getActiveFile( ).m_bag( (bag::bagType) curBg,
                                                                    _currentViewStart );
                        done              = isAllowed( ci.first );
                        SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ]
                            = ( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ] + curBgsz - 1 )
                              % curBgsz;
                        if( done ) {
                            auto idata                = ITEM::getItemData( ci.first );
                            _view[ _currSelectedIdx ] = std::pair( ci, idata );
                        }
                    }
                }
            }
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );
        } else if( p_index >= s8( _view.size( ) ) ) {
            _currSelectedIdx = _view.size( ) - 1;

            while( p_index-- >= s8( _view.size( ) ) ) {
                std::rotate( _view.begin( ), _view.begin( ) + 1, _view.end( ) );
                if( _currentViewStart != _currentViewEnd ) {
                    SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ]
                        = ( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ] + 1 ) % curBgsz;
                    _currentViewStart = ( _currentViewStart + 1 ) % curBgsz;
                    for( bool done = false; !done; ) {
                        auto ci         = SAVE::SAV.getActiveFile( ).m_bag( (bag::bagType) curBg,
                                                                    _currentViewEnd );
                        done            = isAllowed( ci.first );
                        _currentViewEnd = ( _currentViewEnd + 1 ) % curBgsz;
                        if( done ) {
                            auto idata                = ITEM::getItemData( ci.first );
                            _view[ _currSelectedIdx ] = std::pair( ci, idata );
                        }
                    }
                }
            }
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );
        }
    }

    void bagViewer::selectPage( u8 p_page ) {
        SAVE::SAV.getActiveFile( ).m_lstBag = p_page;
        auto curBg                          = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
        auto curBgsz                        = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

        if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ] >= curBgsz ) {
            SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBg ] = 0;
            _currSelectedIdx                                    = 0;
        }
        initView( );
        _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                             _currSelectedIdx );
    }

    u8 bagViewer::handleTouch( ) {
        auto tpos = _bagUI->getTouchPositions( );

        bool bad = false;

        for( auto t : tpos ) {
            if( ( touch.px || touch.py ) && t.first.inRange( touch ) ) {
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    if( !t.first.inRange( touch ) ) {
                        bad = true;
                        break;
                    }
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                if( !bad ) {
                    if( !t.second ) { return 1; } // Back

                    if( t.second < 6 ) { // select page
                        _currSelectedIdx = 0;
                        selectPage( t.second - 1 );
                        return 0;
                    }

                    if( t.second == 90 ) { // forward
                        selectItem( _currSelectedIdx + 1 );
                        return 0;
                    }
                    if( t.second == 91 ) { // backward
                        selectItem( _currSelectedIdx - 1 );
                        return 0;
                    }

                    if( t.second >= 100 ) { // select item
                        selectItem( t.second - 100 );
                        return 0;
                    }
                }
            }
        }

        /*else if( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 44, SCREEN_HEIGHT - 10, 16 ) ) {
            if( !curBgsz ) return true;
            SOUND::playSoundEffect( SFX_CHOOSE );
            SAVE::SAV.getActiveFile( ).m_lstBagItem
                = ( SAVE::SAV.getActiveFile( ).m_lstBagItem + MAX_ITEMS_PER_PAGE - 1 ) % curBgsz;
            initView( );
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                               _view, _currSelectedIdx );
        }

        else if( !_hasSprite && GET_AND_WAIT_C( SCREEN_WIDTH - 76, SCREEN_HEIGHT - 10, 16 ) ) {
            if( !curBgsz ) return true;
            SOUND::playSoundEffect( SFX_CHOOSE );
            SAVE::SAV.getActiveFile( ).m_lstBagItem
                = ( SAVE::SAV.getActiveFile( ).m_lstBagItem + curBgsz - MAX_ITEMS_PER_PAGE + 1 )
                % curBgsz;
            initView( );
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag,
                                               _view, _currSelectedIdx );
        }

           */
        /*
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
                                                     )
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
                    } elsen
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
        } else */
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
                           n }
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
        return 0;
    }

    bool bagViewer::handleSomeInput( bool p_allowSort ) {
        auto curBg   = (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag;
        auto curBgsz = SAVE::SAV.getActiveFile( ).m_bag.size( curBg );

        if( GET_AND_WAIT( KEY_B ) || GET_AND_WAIT( KEY_X ) || handleTouch( ) ) {
            SOUND::playSoundEffect( SFX_CANCEL );
            return false;
        } else if( p_allowSort && GET_AND_WAIT( KEY_SELECT ) ) {
            SOUND::playSoundEffect( SFX_CHOOSE );
            SAVE::SAV.getActiveFile( ).m_bag.sort(
                (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag );

            initView( );
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );
        } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
            SOUND::playSoundEffect( SFX_SELECT );
            _currSelectedIdx = 0;
            selectPage( SAVE::SAV.getActiveFile( ).m_lstBag = ( curBg + BAG_CNT - 1 ) % BAG_CNT );
            cooldown = COOLDOWN_COUNT;
        } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
            SOUND::playSoundEffect( SFX_SELECT );
            _currSelectedIdx = 0;
            selectPage( SAVE::SAV.getActiveFile( ).m_lstBag = ( curBg + 1 ) % BAG_CNT );

            cooldown = COOLDOWN_COUNT;
        } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
            if( !curBgsz ) {
                cooldown = COOLDOWN_COUNT;
                return true;
            }
            SOUND::playSoundEffect( SFX_SELECT );
            selectItem( _currSelectedIdx + 1 );
            cooldown = COOLDOWN_COUNT;
        } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
            if( !curBgsz ) {
                cooldown = COOLDOWN_COUNT;
                return true;
            }
            SOUND::playSoundEffect( SFX_SELECT );
            selectItem( _currSelectedIdx - 1 );
            cooldown = COOLDOWN_COUNT;
        }

        return true;
    }

    std::vector<bagViewer::choice> bagViewer::getItemChoices( u16             p_itemId,
                                                              ITEM::itemData* p_data ) {
        auto res = std::vector<bagViewer::choice>( );

        if( _context != SELL_ITEM ) {
            if( canGive( p_itemId ) ) { res.push_back( GIVE_ITEM ); }

            if( p_data->m_itemType & ITEM::ITEMTYPE_BERRY ) { res.push_back( VIEW_DETAILS ); }

            if( ( p_data->m_itemType & 15 ) == ITEM::ITEMTYPE_MEDICINE
                || p_data->m_itemType == ITEM::ITEMTYPE_FORMECHANGE
                || p_data->m_itemType == ITEM::ITEMTYPE_EVOLUTION ) {
                res.push_back( APPLY_ITEM );
            }

            if( ITEM::isUsable( p_itemId ) ) { res.push_back( USE_ITEM ); }

            if( p_data->m_itemType == ITEM::ITEMTYPE_KEYITEM ) {
                if( ITEM::isUsable( p_itemId ) ) {
                    if( SAVE::SAV.getActiveFile( ).m_registeredItem != p_itemId ) {
                        res.push_back( REGISTER_ITEM );
                    } else {
                        res.push_back( DEREGISTER_ITEM );
                    }
                }
            } else if( p_data->m_itemType == ITEM::ITEMTYPE_TM ) {
                res.push_back( USE_TM );
                if( p_data->m_effect == 2 ) { // TR
                    res.push_back( TOSS_ITEM );
                }
            } else {
                res.push_back( TOSS_ITEM );
            }
        } else {
            res.push_back( SELL );
        }

        res.push_back( BACK );

        return res;
    }

    void bagViewer::waitForInteract( ) {
        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_A ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }
                SOUND::playSoundEffect( SFX_CHOOSE );
                cooldown = COOLDOWN_COUNT;
                break;
            }
        }
    }

    u16 bagViewer::executeChoice( bagViewer::choice p_choice ) {
        u16  targetItem = currentItem( ).first.first;
        auto idata      = currentItem( ).second;

        auto curBag     = SAVE::SAV.getActiveFile( ).m_lstBag;
        auto curBagSize = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBag );

        char buffer[ 100 ];

        switch( p_choice ) {
        case SELL: {
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );
            if( !idata.m_sellPrice ) { // item cannot be sold
                _bagUI->printMessage( GET_STRING( 482 ) );
                waitForInteract( );
                return 0;
            }

            auto cnt = SAVE::SAV.getActiveFile( ).m_bag.count( (bag::bagType) curBag, targetItem );

            if( cnt > 1 ) {
                // make player choose how many items they want to sell

                // TODO
            }

            u32 sellprice = cnt * idata.m_sellPrice;
            snprintf( buffer, 99, GET_STRING( 484 ), sellprice );

            IO::yesNoBox yn;
            if( yn.getResult( [ & ]( ) { return _bagUI->printYNMessage( buffer, 254 ); },
                              [ & ]( IO::yesNoBox::selection p_sel ) {
                                  _bagUI->printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                              } )
                == IO::yesNoBox::NO ) {
                return 0;
            }

            SAVE::SAV.getActiveFile( ).m_money += sellprice;
            if( SAVE::SAV.getActiveFile( ).m_money > 999'999'999 ) {
                SAVE::SAV.getActiveFile( ).m_money = 999'999'999;
            }
            SAVE::SAV.getActiveFile( ).m_bag.erase( (bag::bagType) curBag, targetItem, cnt );
            curBagSize = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBag );
            if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] == curBagSize ) {
                if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] > 0 ) {
                    SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ]--;
                }
            }
            initView( );
            return 0;
        }
        case DEREGISTER_ITEM: SAVE::SAV.getActiveFile( ).m_registeredItem = 0; return 0;
        case REGISTER_ITEM: SAVE::SAV.getActiveFile( ).m_registeredItem = targetItem; return 0;
        case TOSS_ITEM: {
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );
            IO::yesNoBox yn;
            if( yn.getResult( [ & ]( ) { return _bagUI->printYNMessage( GET_STRING( 485 ), 254 ); },
                              [ & ]( IO::yesNoBox::selection p_sel ) {
                                  _bagUI->printYNMessage( 0, p_sel == IO::yesNoBox::NO );
                              } )
                == IO::yesNoBox::NO ) {
                return 0;
            }

            SAVE::SAV.getActiveFile( ).m_bag.erase( (bag::bagType) curBag, targetItem );
            curBagSize = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBag );
            if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] == curBagSize ) {
                if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] > 0 ) {
                    SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ]--;
                }
            }
            initView( );
            return 0;
        }
        case USE_ITEM: {
            SAVE::SAV.getActiveFile( ).m_lstUsedItem = targetItem;

            if( !ITEM::use(
                    targetItem, []( const char* ) {}, true ) ) {
                return 2 | ( targetItem << 2 );
            } else {
                _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                     _currSelectedIdx );
                ITEM::use( targetItem, [ & ]( const char* p_message ) {
                    _bagUI->printMessage( p_message );
                    waitForInteract( );
                } );
                if( idata.m_itemType != ITEM::ITEMTYPE_KEYITEM
                    && idata.m_itemType != ITEM::ITEMTYPE_FORMECHANGE ) {
                    SAVE::SAV.getActiveFile( ).m_bag.erase(
                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, targetItem, 1 );
                }

                curBagSize = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBag );
                if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] == curBagSize ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] > 0 ) {
                        SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ]--;
                    }
                }
                initView( );
            }
            return 0;
        }
        case USE_TM:
        case GIVE_ITEM:
        case APPLY_ITEM: {
            // select a pkmn
            u8   tgpkmn     = 255;
            bool removeItem = false;
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );

            IO::choiceBox cb2 = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN );
            tgpkmn            = cb2.getResult(
                [ & ]( u8 ) {
                    _bagUI->drawPkmnChoice( );
                    auto tmp = _bagUI->getPkmnInputTarget( );
                    tmp.push_back( _bagUI->getButtonInputTarget( IO::choiceBox::BACK_CHOICE ) );
                    return tmp;
                },
                [ & ]( u8 p_selection ) { _bagUI->selectPkmn( p_selection ); } );
            _bagUI->undrawPkmnChoice( );

            if( tgpkmn == IO::choiceBox::BACK_CHOICE ) { return 0; }

            if( p_choice == GIVE_ITEM ) {
                if( giveItemToPkmn( _playerTeam[ tgpkmn ], targetItem ) ) { removeItem = true; }
            } else {
                if( useItemOnPkmn( _playerTeam[ tgpkmn ], targetItem, &idata ) ) {
                    removeItem = true;
                }
            }
            _bagUI->selectPkmn( -1 );

            // remove item from bag
            if( removeItem ) {
                SAVE::SAV.getActiveFile( ).m_bag.erase( (bag::bagType) curBag, targetItem, 1 );
                curBagSize = SAVE::SAV.getActiveFile( ).m_bag.size( (bag::bagType) curBag );
                if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] == curBagSize ) {
                    if( SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ] > 0 ) {
                        SAVE::SAV.getActiveFile( ).m_lstViewedItem[ curBag ]--;
                    }
                }
                initView( );
            }
            return 0;
        }
        case VIEW_DETAILS:
            // TODO
            return 0;
        default: break;
        }
        return 0;
    }

    u16 bagViewer::handleSelection( ) {
        u16  targetItem = currentItem( ).first.first;
        auto idata      = currentItem( ).second;

        _choices               = getItemChoices( targetItem, &idata );
        std::vector<u16> texts = std::vector<u16>( );

        for( u8 i = 0; i < _choices.size( ); ++i ) {
            texts.push_back( getTextForChoice( _choices[ i ] ) );
        }

        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );

        auto res
            = cb.getResult( [ & ]( u8 ) { return _bagUI->drawChoice( targetItem, &idata, texts ); },
                            [ & ]( u8 p_selection ) { _bagUI->selectChoice( p_selection ); } );

        if( res == IO::choiceBox::EXIT_CHOICE ) [[unlikely]] {
                return 2;
            }
        else if( res == IO::choiceBox::BACK_CHOICE || _choices[ res ] == BACK ) {
            _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                 _currSelectedIdx );
            return 0;
        } else {
            auto rs = executeChoice( _choices[ res ] );
            if( !( rs & 2 ) ) {
                _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                     _currSelectedIdx );
            }
            return rs;
        }
    }

    u16 bagViewer::run( ) {
        _currSelectedIdx = 0;
        initUI( );

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( !handleSomeInput( ) )
                break;
            else if( GET_AND_WAIT( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                u16 res = handleSelection( );
                if( res & 2 ) return ( res >> 2 );
            }
            swiWaitForVBlank( );
        }
        return 0;
    }

    u16 bagViewer::getItem( ) {
        _currSelectedIdx = 0;
        initUI( );

        if( _context == MOCK_BATTLE ) {
            // Just wait a couple of frames and return
            for( u8 i = 0; i < 60; ++i ) { swiWaitForVBlank( ); }
            SOUND::playSoundEffect( SFX_CHOOSE );
            u16  targetItem = currentItem( ).first.first;
            auto itemData   = currentItem( ).second;
            confirmChoice( targetItem, &itemData );
            return targetItem;
        }

        cooldown = COOLDOWN_COUNT;
        loop( ) {
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( !handleSomeInput( ) ) return 0;
            if( GET_AND_WAIT( KEY_A ) ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                if( SAVE::SAV.getActiveFile( ).m_bag.empty(
                        (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag ) )
                    continue;

                u16  targetItem = currentItem( ).first.first;
                auto itemData   = currentItem( ).second;
                u8   res        = 0;
                if( targetItem && ( res = confirmChoice( targetItem, &itemData ) ) ) {
                    return targetItem;
                }
                _bagUI->drawBagPage( (bag::bagType) SAVE::SAV.getActiveFile( ).m_lstBag, _view,
                                     _currSelectedIdx );
            }
            swiWaitForVBlank( );
        }
        return 0;
    }
} // namespace BAG
