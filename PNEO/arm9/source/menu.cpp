/*
Pokémon neo
------------------------------

file        : menu.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2022
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

#include <map>
#include <vector>

#include "bag/bagViewer.h"
#include "bag/item.h"
#include "battle/battle.h"
#include "battle/battleTrainer.h"
#include "box/boxViewer.h"
#include "defines.h"
#include "dex/dex.h"
#include "fs/fs.h"
#include "gen/itemNames.h"
#include "gen/locationNames.h"
#include "gen/moveNames.h"
#include "io/choiceBox.h"
#include "io/counter.h"
#include "io/keyboard.h"
#include "io/menu.h"
#include "io/menuUI.h"
#include "io/message.h"
#include "io/navApp.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "io/yesNoBox.h"
#include "map/mapDrawer.h"
#include "map/mapObject.h"
#include "map/mapSlice.h"
#include "pokemon.h"
#include "sound/sound.h"
#include "spx/specials.h"
#include "sts/partyScreen.h"
#include "sts/statusScreen.h"

namespace IO {

    mapNavApp MAP_NAV_APP = mapNavApp( );
#ifndef NO_SOUND
    jboxNavApp JBOX_NAV_APP = jboxNavApp( );
#endif
    std::vector<navApp*> NAV_APPS    = std::vector<navApp*>( );
    navApp*              CUR_NAV_APP = nullptr;

    bool NAV_NEEDS_REDRAW = false;

    /*
     * @brief: Checks which nav apps should be displayed
     */
    void recomputeNavApps( ) {
        NAV_APPS.clear( );
        NAV_APPS.push_back( &MAP_NAV_APP );
#ifndef NO_SOUND
        if( SAVE::SAV.getActiveFile( ).m_options.m_enableBGM ) {
            NAV_APPS.push_back( &JBOX_NAV_APP );
        } else {
            if( CUR_NAV_APP == &JBOX_NAV_APP ) {
                CUR_NAV_APP      = nullptr;
                NAV_NEEDS_REDRAW = true;
            }
        }
#endif
    }

    void handleInput( const char* p_path ) {
        auto& oam = IO::Oam->oamBuffer;

        if( CUR_NAV_APP != nullptr ) {
            if( CUR_NAV_APP->tick( true ) ) {
                // nav app exited
                CUR_NAV_APP = nullptr;
                redraw( );
            }
        }

        if( NAV_NEEDS_REDRAW ) { redraw( ); }

        if( pressed & KEY_Y ) {
            // registered item
            IO::waitForKeysUp( KEY_Y );
            if( SAVE::SAV.getActiveFile( ).m_registeredItem ) {
                if( BAG::isUsable( SAVE::SAV.getActiveFile( ).m_registeredItem ) ) {
                    BAG::use( SAVE::SAV.getActiveFile( ).m_registeredItem,
                              []( const char* p_msg ) { printMessage( p_msg ); } );
                    //  updateItems( );
                } else {
                    printMessage( GET_STRING( IO::STR_UI_CANNOT_USE_FIELD_ITEM ) );
                }
            } else {
                printMessage( GET_STRING( IO::STR_UI_CAN_REGISTER_ITEM_TO_Y ) );
            }
            return;
        }

        if( ( pressed & KEY_X ) || ( pressed & KEY_START ) ) {
            // Open menu
            focusMenu( p_path );
            return;
        }

        for( auto c : getTouchPositions( ) ) {
            if( c.first.inRange( touch ) ) {
                if( c.second < NAV_APP_START ) {
                    oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = false;
                    oam[ SPR_MENU_SEL_OAM_SUB ].x = oam[ SPR_MENU_OAM_SUB( u8( c.second ) ) ].x - 2;
                    oam[ SPR_MENU_SEL_OAM_SUB ].y = oam[ SPR_MENU_OAM_SUB( u8( c.second ) ) ].y - 2;
                    IO::updateOAM( true );
                }

                bool change = true;
                while( touch.px || touch.py ) {
                    swiWaitForVBlank( );
                    scanKeys( );

                    if( !c.first.inRange( touch ) ) {
                        change                               = 0;
                        oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;
                        IO::updateOAM( true );
                        break;
                    }
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                if( change ) {
                    handleMenuSelection( c.second, p_path );
                    oam[ SPR_MENU_SEL_OAM_SUB ].isHidden = true;
                    IO::updateOAM( true );
                }
            }
        }

#ifdef DESQUID
        if( pressed & KEY_SELECT ) {
            focusDesquidMenu( p_path );
            return;
        }
#endif
    }

#ifdef DESQUID
    void handleDesquidMenuSelection( desquidMenuOption p_selection, const char* ) {
        switch( p_selection ) {
        case DSQ_SPAWN_DEFAULT_TEAM: {
            init( );
            IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
            auto          res  = menu.getResult(
                GET_STRING( FS::DESQUID_STRING + 46 ), MSG_NOCLOSE,
                std::vector<u16>{ FS::DESQUID_STRING + 61, FS::DESQUID_STRING + 64,
                                            FS::DESQUID_STRING + 62, FS::DESQUID_STRING + 63 },
                true );
            switch( res ) {
            case 0: { // default team
                memset( SAVE::SAV.getActiveFile( ).m_pkmnTeam, 0,
                        sizeof( SAVE::SAV.getActiveFile( ).m_pkmnTeam ) );
                std::vector<u16> tmp = { PKMN_SCEPTILE, PKMN_BLAZIKEN, PKMN_SWAMPERT };
                for( int i = 0; i < 3; ++i ) {
                    pokemon& a = SAVE::SAV.getActiveFile( ).m_pkmnTeam[ i ];

                    a = pokemon( tmp[ i ], 50, 0, 0, i );

                    // Hand out some ribbons
                    for( u8 j = 0; j < 4; ++j ) {
                        a.m_boxdata.m_ribbons0[ j ]     = rand( ) % 255;
                        a.m_boxdata.m_ribbons1[ j ]     = rand( ) % 255;
                        a.m_boxdata.m_ribbons2[ j ]     = rand( ) % 255;
                        a.m_boxdata.m_contestStats[ j ] = rand( );
                    }
                    a.m_boxdata.m_contestStats[ 4 ] = 255;
                    a.m_boxdata.m_contestStats[ 5 ] = rand( );
                    a.m_boxdata.m_ribbons1[ 2 ]     = rand( ) % 63;
                    a.m_boxdata.m_ribbons1[ 3 ]     = 0;
                    if( a.m_boxdata.m_speciesId == 493 ) {
                        u8 plate = rand( ) % 17;
                        if( plate < 16 )
                            a.giveItem( I_FLAME_PLATE + plate );
                        else
                            a.giveItem( I_PIXIE_PLATE );
                    } else {
                        a.m_boxdata.m_heldItem = 1 + rand( ) % 400;
                    }
                }
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_moves[ 0 ] = M_ROCK_CLIMB;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_moves[ 1 ] = M_FLASH;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_moves[ 2 ] = M_SWEET_SCENT;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 0 ].m_boxdata.m_moves[ 3 ] = M_CUT;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 0 ] = M_DIG;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 1 ] = M_ROCK_SMASH;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 2 ] = M_STRENGTH;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 1 ].m_boxdata.m_moves[ 3 ] = M_FLY;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 0 ] = M_SURF;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 1 ] = M_WATERFALL;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 2 ] = M_DIVE;
                SAVE::SAV.getActiveFile( ).m_pkmnTeam[ 2 ].m_boxdata.m_moves[ 3 ] = M_SPLASH;

                break;
            }
            case 1: { // fill boxes
                memset( SAVE::SAV.getActiveFile( ).m_caughtPkmn, 0,
                        sizeof( SAVE::SAV.getActiveFile( ).m_caughtPkmn ) );
                memset( SAVE::SAV.getActiveFile( ).m_seenPkmn, 0,
                        sizeof( SAVE::SAV.getActiveFile( ).m_seenPkmn ) );
                SAVE::SAV.getActiveFile( ).setFlag( SAVE::F_NAT_DEX_OBTAINED, 1 );
                for( u16 pid = 1; pid <= MAX_PKMN; ++pid ) {
                    auto box    = ( pid - 1 ) / 30;
                    auto boxpos = ( pid - 1 ) % 30;

                    SAVE::SAV.getActiveFile( ).m_storedPokemon[ box ][ boxpos ]
                        = boxPokemon( pid, 75, 0, 0, 255 );
                    SAVE::SAV.getActiveFile( ).registerCaughtPkmn( pid );
                }

                break;
            }
            case 2: { // repel 9999
                SAVE::SAV.getActiveFile( ).m_repelSteps = 9999;
                break;
            }
            case 3: { // repel off
                SAVE::SAV.getActiveFile( ).m_repelSteps = 0;
                break;
            }
            default: break;
            }

            init( );
            swiWaitForVBlank( );
            break;
        }
        case DSQ_SPAWN_DEFAULT_ITEMS:
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::ITEMS, I_MAX_REPEL, 999 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::ITEMS, I_MASTER_BALL, 999 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::MEDICINE, I_MAX_REVIVE, 999 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_ACRO_BIKE, 1 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_MACH_BIKE, 1 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_SUPER_ROD, 1 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_GO_GOGGLES, 1 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::KEY_ITEMS, I_DEVON_SCOPE, 1 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::ITEMS, I_RED_SHARD, 999 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::ITEMS, I_BLUE_SHARD, 999 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::ITEMS, I_YELLOW_SHARD, 999 );
            SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::ITEMS, I_GREEN_SHARD, 999 );

            for( u16 i = 0; i < I_EGGANT_BERRY; ++i ) {
                if( BAG::itemToBerry( i ) ) {
                    SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::bag::BERRIES, i, 1 );
                }
            }
            init( );
            break;
        case DSQ_EDIT_FLAGS: {
            init( );
            IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
            auto          res  = menu.getResult( GET_STRING( FS::DESQUID_STRING + 46 ), MSG_NOCLOSE,
                                                 std::vector<u16>{ FS::DESQUID_STRING + 53,
                                                                   FS::DESQUID_STRING + 54,
                                                                   FS::DESQUID_STRING + 55 },
                                                 true );
            switch( res ) {
            case 0: { // edit badges
                init( );
                MAP::curMap->awardBadge( 0, 1 + rand( ) % 8 );
                break;
            }
            case 1: { // all badges
                init( );
                for( u8 i = 1; i <= 8; ++i ) { MAP::curMap->awardBadge( 0, i ); }
                break;
            }
            case 2: { // edit route
                init( );
                IO::choiceBox menu3 = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
                SAVE::SAV.getActiveFile( ).m_route = menu3.getResult(
                    GET_STRING( FS::DESQUID_STRING + 46 ), MSG_NOCLOSE,
                    std::vector<u16>{ FS::DESQUID_STRING + 65, FS::DESQUID_STRING + 66,
                                      FS::DESQUID_STRING + 67, FS::DESQUID_STRING + 68,
                                      FS::DESQUID_STRING + 69 } );
                break;
            }
            default: break;
            }

            init( );
            break;
        }
        case DSQ_WARP: {
            init( );
            break;
        }
        case DSQ_TIME: {
            init( );
            IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
            auto          res  = menu.getResult(
                GET_STRING( FS::DESQUID_STRING + 46 ), MSG_NOCLOSE,
                std::vector<u16>{ FS::DESQUID_STRING + 56, FS::DESQUID_STRING + 57,
                                            FS::DESQUID_STRING + 58, FS::DESQUID_STRING + 59,
                                            FS::DESQUID_STRING + 60 },
                true );
            switch( res ) {
            case 0: { // daytime to dawn
                SAVE::CURRENT_TIME.m_hours
                    = DAY_TIMES[ SAVE::CURRENT_DATE.m_month / 4 ][ DAYTIME_MORNING ];
                break;
            }
            case 1: { // daytime to day
                SAVE::CURRENT_TIME.m_hours
                    = DAY_TIMES[ SAVE::CURRENT_DATE.m_month / 4 ][ DAYTIME_DAY ];
                break;
            }
            case 2: { // daytime to dusk
                SAVE::CURRENT_TIME.m_hours
                    = DAY_TIMES[ SAVE::CURRENT_DATE.m_month / 4 ][ DAYTIME_DUSK ];
                break;
            }
            case 3: { // daytime to night
                SAVE::CURRENT_TIME.m_hours
                    = DAY_TIMES[ SAVE::CURRENT_DATE.m_month / 4 ][ DAYTIME_NIGHT ];
                break;
            }
            case 4: { // playtime +10h
                if( SAVE::SAV.getActiveFile( ).m_playTime.m_hours + 10 <= 999 ) {
                    SAVE::SAV.getActiveFile( ).m_playTime.m_hours += 10;
                } else {
                    SAVE::SAV.getActiveFile( ).m_playTime.m_hours = 999;
                }
                break;
            }
            default: break;
            }

            init( );
            break;
        }
        case DSQ_BATTLE_TRAINER: {
            init( );
            break;
        }
        }
    }

    void focusDesquidMenu( const char* p_path ) {
        SOUND::playSoundEffect( SFX_MENU );

        std::vector<u16> choices = {
            FS::DESQUID_STRING + 47, FS::DESQUID_STRING + 48, FS::DESQUID_STRING + 49,
            FS::DESQUID_STRING + 50, FS::DESQUID_STRING + 51, FS::DESQUID_STRING + 52,
        };

        IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        auto          res
            = menu.getResult( GET_STRING( FS::DESQUID_STRING + 46 ), MSG_NOCLOSE, choices, true );

        if( res != IO::choiceBox::BACK_CHOICE ) {
            handleDesquidMenuSelection( desquidMenuOption( res ), p_path );
        }
        init( );
    }

#endif
    void _printMessage( const char* p_message ) {
        printMessage( p_message );
    }

    void handleMenuSelection( menuOption p_selection, const char* p_path ) {
        if( p_selection >= NAV_APP_START ) {
            // start the nav app
            u8 app = p_selection - NAV_APP_START;
            if( app < NAV_APPS.size( ) && NAV_APPS[ app ] != nullptr ) {
                CUR_NAV_APP = NAV_APPS[ app ];
                CUR_NAV_APP->load( );
            }
            return;
        }

        switch( p_selection ) {
        case VIEW_PARTY: {
            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            u8 teamSize = 0;
            for( ; teamSize < SAVE::NUM_PARTY_SLOTS; ++teamSize ) {
                if( !SAVE::SAV.getActiveFile( ).m_pkmnTeam[ teamSize ].m_boxdata.m_speciesId ) {
                    break;
                }
            }
            STS::partyScreen sts
                = STS::partyScreen( SAVE::SAV.getActiveFile( ).m_pkmnTeam, teamSize );

            SOUND::dimVolume( );

            auto res = sts.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            IO::resetScale( true, false );
            bgUpdate( );

            ANIMATE_MAP = true;
            SOUND::restoreVolume( );

            init( );
            MAP::curMap->draw( );
            if( res.m_selectedMove ) {
                // check if selected move is a move that triggers some map event
                bool maptg = false;
                for( auto m : MAP::curMap->getTriggerMovesForCurPos( ) ) {
                    maptg |= res.m_selectedMove == m;
                }
                if( maptg ) {
                    MAP::curMap->executeMoveTriggerScript( res.m_selectedMove );
                    return;
                }

                if( res.m_selectedMove == M_FLY ) {
                    auto fpos = SAVE::SAV.getActiveFile( ).getFlyPosForLocation(
                        res.m_selectedMoveTarget );
                    if( res.m_selectedMoveTarget != fpos.m_targetLocation
                        || !fpos.m_targetLocation ) {
                        return;
                    }

                    auto target = MAP::warpPos{
                        fpos.m_targetBank,
                        MAP::position{ fpos.m_targetX, fpos.m_targetY, fpos.m_targetZ } };
                    MAP::curMap->flyPlayer( target );
                    return;
                }
                for( u8 j = 0; j < 2; ++j ) {
                    if( BATTLE::possible( res.m_selectedMove, j ) ) {
                        BATTLE::use( res.m_selectedMove, j );
                        break;
                    }
                }
            }
            return;
        }
        case VIEW_DEX: {
            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            DEX::dex dx = DEX::dex( );
            SOUND::dimVolume( );

            dx.run( );
            // DEX::dex::SHOW_CAUGHT, MAX_PKMN ).run( SAVE::SAV.getActiveFile( ).m_lstDex );

            IO::initVideoSub( );
            IO::resetScale( true, false );
            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            MAP::curMap->draw( );
            init( );
            return;
        }
        case VIEW_BAG: {
            ANIMATE_MAP = false;
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            BAG::bagViewer bv = BAG::bagViewer( SAVE::SAV.getActiveFile( ).m_pkmnTeam );
            SOUND::dimVolume( );
            u16 res = bv.run( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );

            if( res ) { BAG::use( res, _printMessage ); }
            return;
        }
        case VIEW_ID: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            SPX::runIDViewer( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );
            return;
        }
        case SAVE: {
            IO::yesNoBox yn;
            if( yn.getResult( GET_STRING( IO::STR_UI_WOULD_YOU_LIKE_TO_SAVE ), MSG_INFO_NOCLOSE )
                == IO::yesNoBox::YES ) {
                init( );
                ANIMATE_MAP = false;
                u16 lst     = -1;
                if( FS::writeSave( p_path, [ & ]( u16 p_perc, u16 p_total ) {
                        u16 stat = p_perc * 18 / p_total;
                        if( stat != lst ) {
                            lst = stat;
                            printMessage( 0, MSG_INFO_NOCLOSE );
                            char        buffer[ 100 ];
                            std::string buf2 = "";
                            for( u8 i = 0; i < stat; ++i ) {
                                buf2 += "\x03";
                                if( i % 3 == 2 ) { buf2 += " "; }
                            }
                            for( u8 i = stat; i < 18; ++i ) {
                                buf2 += "\x04";
                                if( i % 3 == 2 ) { buf2 += " "; }
                            }
                            snprintf( buffer, 99, GET_STRING( IO::STR_UI_SAVING_A_LOT_OF_DATA ),
                                      buf2.c_str( ) );
                            doPrintMessage( buffer, MSG_INFO_NOCLOSE, 0, 0, true );
                        }
                    } ) ) {
                    printMessage( 0, MSG_INFO_NOCLOSE );
                    SOUND::playSoundEffect( SFX_SAVE );
                    printMessage( GET_STRING( IO::STR_UI_SAVING_COMPLETE ), MSG_INFO );
                } else {
                    printMessage( 0, MSG_INFO_NOCLOSE );
                    printMessage( GET_STRING( IO::STR_UI_SAVING_FAILED ), MSG_INFO );
                }
                ANIMATE_MAP = true;
            } else {
                init( );
            }

            return;
        }
        case SETTINGS: {
            ANIMATE_MAP = false;
            SOUND::dimVolume( );

            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            SAVE::runSettings( );

            FADE_TOP_DARK( );
            FADE_SUB_DARK( );
            IO::clearScreen( false );
            videoSetMode( MODE_5_2D );
            bgUpdate( );

            MAP::curMap->draw( );
            ANIMATE_MAP = true;
            SOUND::restoreVolume( );
            init( );

            return;
        }
        default: return;
        }
    }

    void focusMenu( const char* p_path ) {
        SOUND::playSoundEffect( SFX_MENU );

        IO::choiceBox menu = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        auto          res  = menu.getResult( [ & ]( u8 ) { return drawMenu( ); }, selectMenuItem );

        init( );
        if( res != IO::choiceBox::BACK_CHOICE ) {
            handleMenuSelection( menuOption( res ), p_path );
        }
    }

    s32 getItemCount( std::pair<u16, u32> p_item, const BAG::itemData& p_itemData,
                      const std::string& p_name, u8 p_paymentMethod ) {

        auto& oam = IO::Oam->oamBuffer;
        IO::printRectangle( 0, 40, 255, 192, true, 0 );

        // Compute max amount of the selected item the player can buy

        s32 mx = 0;
        if( p_paymentMethod == 0 ) {
            mx = SAVE::SAV.getActiveFile( ).m_money / p_item.second;
        } else if( p_paymentMethod == 1 ) {
            mx = SAVE::SAV.getActiveFile( ).m_battlePoints / p_item.second;
        } else if( p_paymentMethod == 2 ) {
            mx = SAVE::SAV.getActiveFile( ).m_coins / p_item.second;
        } else if( p_paymentMethod == 3 ) {
            mx = SAVE::SAV.getActiveFile( ).m_ashCount / p_item.second;
        }

        mx = std::max( s32( 0 ),
                       (s32) std::min( mx, s32( 999
                                                - SAVE::SAV.getActiveFile( ).m_bag.count(
                                                    BAG::toBagType( p_itemData.m_itemType ),
                                                    p_item.first ) ) ) );

        if( p_paymentMethod == 3 ) { mx = 1; }

        char buffer[ 100 ];
        s32  res = 0;
        if( mx == 0 ) {
            IO::printRectangle( 0, 40, 255, 192, true, 0 );
            init( true );
            for( u8 i = 0; i < 128; ++i ) { oam[ i ].isHidden = true; }
            for( u8 i = 0; i < 10; ++i ) {
                oam[ SPR_MSGBOX_OAM_SUB + i ].isHidden = false;
                oam[ SPR_MSGBOX_OAM_SUB + i ].y        = 32;
            }

            IO::updateOAM( true );

            IO::regularFont->setColor( IO::BLACK_IDX, 1 );
            if( p_paymentMethod < 3 ) {
                IO::regularFont->printBreakingStringC(
                    GET_STRING( IO::STR_UI_NOT_ENOUGH_MONEYTYPE_START + p_paymentMethod ), 40, 38,
                    256 - 80, true );
            } else if( p_paymentMethod == 3 ) {
                IO::regularFont->printBreakingStringC(
                    GET_STRING( IO::STR_UI_NOT_ENOUGH_MONEYTYPE_ASH ), 40, 38, 256 - 80, true );
            }

            waitForInteract( );
            IO::regularFont->setColor( IO::WHITE_IDX, 1 );
            return res;
        } else if( mx > 0 ) {
            snprintf( buffer, 100, GET_STRING( IO::STR_UI_MART_CLERK_HOW_MANY ), p_name.c_str( ) );

            for( u8 i = 0; i < 10; ++i ) { oam[ SPR_MSGBOX_OAM_SUB + i ].isHidden = false; }

            IO::counter c   = IO::counter( 0, mx );
            u8          mdg = 0;
            for( auto tmp = mx; tmp > 0; tmp /= 10, ++mdg ) {}

            res = c.getResult(
                [ & ]( ) {
                    auto res2 = drawCounter( 0, mx );
                    IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                    IO::regularFont->printBreakingStringC( buffer, 40, 38, 256 - 80, true );
                    IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                    return res2;
                },
                [ & ]( u32 p_newValue, u8 p_selDig ) {
                    updateCounterValue( p_newValue, p_selDig, mdg );
                },
                [ & ]( s32 p_hoveredButton ) { hoverCounterButton( 0, mx, p_hoveredButton ); }, 1 );
            if( !res ) { return res; }
            if( res == -3 ) { return -1; }
        } else {
            res = 1;
        }

        // make the player confirm the choice
        IO::printRectangle( 0, 40, 255, 192, true, 0 );
        init( true );
        for( u8 i = 0; i < 128; ++i ) { oam[ i ].isHidden = true; }
        for( u8 i = 0; i < 10; ++i ) {
            oam[ SPR_MSGBOX_OAM_SUB + i ].isHidden = false;
            oam[ SPR_MSGBOX_OAM_SUB + i ].y        = 16;
        }

        u32 cost = res * p_item.second;

        if( p_paymentMethod < 3 ) {
            snprintf( buffer, 99,
                      GET_STRING( IO::STR_UI_MART_CLERK_TOTAL_MONEYTYPE_START + p_paymentMethod ),
                      p_name.c_str( ), res, cost );
        } else if( p_paymentMethod == 3 ) {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_MART_CLERK_TOTAL_MONEYTYPE_ASH ),
                      p_name.c_str( ), cost );
        }
        IO::yesNoBox yn;
        auto         conf = yn.getResult(
            [ & ]( ) {
                auto tmpres = printYNMessage( 0, MSG_NORMAL, 253 );
                IO::regularFont->setColor( IO::BLACK_IDX, 1 );
                IO::regularFont->printBreakingStringC( buffer, 40, 22, 256 - 80, true );
                IO::regularFont->setColor( IO::WHITE_IDX, 1 );
                return tmpres;
            },
            [ & ]( IO::yesNoBox::selection p_selection ) {
                printYNMessage( 0, MSG_NORMAL, p_selection == IO::yesNoBox::NO );
            } );
        if( conf == IO::yesNoBox::NO ) { return 0; }
        SOUND::playSoundEffect( SFX_BUY_SUCCESSFUL );
        return res;
    }

    void selectDaycarePkmn( u8 p_daycare, u8 p_selection ) {
        boxPokemon* dcstart = &SAVE::SAV.getActiveFile( ).m_dayCarePkmn[ p_daycare * 2 ];
        if( p_selection == IO::choiceBox::EXIT_CHOICE
            || p_selection == IO::choiceBox::BACK_CHOICE ) {
            // empty!
        } else {
            if( p_selection < 2 && dcstart[ p_selection ].getSpecies( ) ) {
                selectMenuItem( p_selection );
            }
        }
    }

    u8 chooseDaycarePkmn( u8 p_daycare ) {
        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        return cb.getResult(
            [ & ]( u8 ) { return drawDaycareChoice( p_daycare ); },
            [ & ]( u8 p_selection ) { selectDaycarePkmn( p_daycare, p_selection ); } );
    }

    void buyItem( const std::vector<std::pair<u16, u32>>& p_offeredItems, u8 p_paymentMethod ) {
        std::vector<std::string>   names = std::vector<std::string>( );
        std::vector<std::string>   descr = std::vector<std::string>( );
        std::vector<BAG::itemData> data  = std::vector<BAG::itemData>( );

        for( auto i : p_offeredItems ) {
            data.push_back( FS::getItemData( i.first ) );
            auto nm = FS::getItemName( i.first );
            if( data.back( ).m_itemType == BAG::ITEMTYPE_TM ) {
                nm = FS::getMoveName( data.back( ).m_param2 );
                descr.push_back( FS::getMoveDescr( data.back( ).m_param2 ) );
            } else {
                descr.push_back( FS::getItemDescr( i.first ) );
            }
            names.push_back( nm );
        }

        IO::choiceBox cb = IO::choiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT );
        hideMessageBox( );

        u8 curPg = 0, oldsel = 0;
        loop( ) {
            // Make player select an item
            auto curItm = cb.getResult(
                [ & ]( u8 p_page ) {
                    curPg = p_page;
                    return drawItemChoice( p_offeredItems, names, data, p_paymentMethod,
                                           NUM_CB_CHOICES * p_page );
                },
                [ & ]( u8 p_selection ) {
                    selectItem( p_offeredItems[ NUM_CB_CHOICES * curPg + p_selection ],
                                data[ NUM_CB_CHOICES * curPg + p_selection ],
                                descr[ NUM_CB_CHOICES * curPg + p_selection ],
                                NUM_CB_CHOICES * curPg + p_selection );
                },
                oldsel, IO::choiceBox::DEFAULT_TICK, curPg );

            oldsel = curItm;

            if( curItm == IO::choiceBox::BACK_CHOICE || curItm == IO::choiceBox::EXIT_CHOICE ) {
                dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
                init( true );
                hideMessageBox( );
                break;
            }

            curItm += NUM_CB_CHOICES * curPg;

            // Make player choose how many of the chosen item they want to buy

            s32 cnt = getItemCount( p_offeredItems[ curItm ], data[ curItm ], names[ curItm ],
                                    p_paymentMethod );
            dmaFillWords( 0, bgGetGfxPtr( IO::bg2sub ), 256 * 192 );
            init( true );

            if( cnt == -1 ) {
                hideMessageBox( );
                break;
            }

            if( cnt > 0 ) {
                if( p_paymentMethod == 0 ) {
                    SAVE::SAV.getActiveFile( ).m_money -= p_offeredItems[ curItm ].second * cnt;
                } else if( p_paymentMethod == 1 ) {
                    SAVE::SAV.getActiveFile( ).m_battlePoints
                        -= p_offeredItems[ curItm ].second * cnt;
                } else if( p_paymentMethod == 2 ) {
                    SAVE::SAV.getActiveFile( ).m_coins -= p_offeredItems[ curItm ].second * cnt;
                } else if( p_paymentMethod == 3 ) {
                    SAVE::SAV.getActiveFile( ).m_ashCount -= p_offeredItems[ curItm ].second * cnt;
                }
                SAVE::SAV.getActiveFile( ).m_bag.insert(
                    BAG::toBagType( data[ curItm ].m_itemType ), p_offeredItems[ curItm ].first,
                    cnt );
            }
        }
    }

} // namespace IO
