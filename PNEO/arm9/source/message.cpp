/*
Pokémon neo
------------------------------

file        : message.cpp
author      : Philip Wellnitz
description : Implementations for displaying messages on the top screen in th OW.

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

#include "bag/item.h"
#include "battle/battle.h"
#include "defines.h"
#include "fs/fs.h"
#include "gen/itemNames.h"
#include "gen/locationNames.h"
#include "gen/moveNames.h"
#include "io/message.h"
#include "io/screenFade.h"
#include "io/sprite.h"
#include "io/strings.h"
#include "io/uio.h"
#include "pokemon.h"
#include "sound/sound.h"
#include "spx/specials.h"

namespace IO {
    char TMP_TEXT_BUF[ 512 ]  = { 0 };
    u16  CONT_BUF[ 16 * 16 ]  = { 0 };
    u16  TEXT_BUF[ 64 * 256 ] = { 0 };
    u16  TEXT_PAL[ 16 ]       = { 0, IO::BLACK, IO::GRAY, IO::WHITE, IO::BLUE, IO::BLUE };
    char TEXT_CACHE_1[ 256 ]  = { 0 }; // top line
    char TEXT_CACHE_2[ 256 ]  = { 0 }; // bottom line

    std::string parseLogCmd( const std::string& p_cmd ) {
        u16 tmp = -1;

        if( p_cmd == "PLAYER" ) { return SAVE::SAV.getActiveFile( ).m_playername; }
        if( p_cmd == "RIVAL" ) {
            if( SAVE::SAV.getActiveFile( ).checkFlag( SAVE::F_RIVAL_APPEARANCE ) ) {
                return std::string( GET_STRING( 461 ) );
            } else {
                return std::string( GET_STRING( 460 ) );
            }
        }
        if( sscanf( p_cmd.c_str( ), "CRY:%hu", &tmp ) && tmp != u16( -1 ) ) {
            SOUND::playCry( tmp );
            return "";
        }
        if( sscanf( p_cmd.c_str( ), "VAR:%hu", &tmp ) && tmp != u16( -1 ) ) {
            return std::to_string( SAVE::SAV.getActiveFile( ).getVar( tmp ) );
        }
        if( sscanf( p_cmd.c_str( ), "TEAM:%hu", &tmp ) && tmp != u16( -1 ) ) {
            return FS::getDisplayName(
                SAVE::SAV.getActiveFile( ).getTeamPkmn( tmp )->getSpecies( ) );
        }
        return std::string( "[" ) + p_cmd + "]";
    }

    void hideMessageBox( ) {
        for( u8 i = 0; i < SPR_MSGBOX_OAM_LEN; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MSGBOX_OAM + i ].isHidden = true;
        }
        IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;

        for( u8 i = 0; i < SPR_MSGTEXT_OAM_LEN; ++i ) {
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + i ].isHidden = true;
        }
        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
        std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
        IO::updateOAM( false );
    }

    void animateMB( u8 p_frame ) {
        if( ( p_frame & 15 ) == 0 ) {
            auto& oam                       = IO::OamTop->oamBuffer;
            oam[ SPR_MSGCONT_OAM ].isHidden = !oam[ SPR_MSGCONT_OAM ].isHidden;
            IO::updateOAM( false );
        }
    }

    void waitForInteract( ) {
        scanKeys( );
        cooldown = COOLDOWN_COUNT;
        u8 frame = 0;
        loop( ) {
            animateMB( ++frame );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_A ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    animateMB( ++frame );
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

    void animateMBS( u8 p_frame ) {
        if( ( p_frame & 31 ) == 0 ) {
            IO::regularFont->drawContinue( 254 - 12, 192 - 40 + 24, false );
        }
        if( ( p_frame & 31 ) == 15 ) {
            IO::regularFont->hideContinue( 254 - 12, 192 - 40 + 24, 0, false );
        }
    }

    void waitForInteractS( ) {
        scanKeys( );
        cooldown = COOLDOWN_COUNT;
        u8 frame = 0;
        loop( ) {
            animateMBS( ++frame );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( ( pressed & KEY_A ) || ( pressed & KEY_B ) || touch.px || touch.py ) {
                while( touch.px || touch.py ) {
                    animateMBS( ++frame );
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

    void constructMBSprite( style p_style ) {
        switch( p_style ) {
        case MSG_NORMAL:
        case MSG_NOCLOSE:
        case MSG_NORMAL_CONT:
        case MSG_BRAILLE: {
            // construct mb 1, use default gfx addresses
            IO::loadSpriteB( "UI/mbox1", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 0, 192 - 46, 32, 64, false,
                             false, false, OBJPRIORITY_0, false );
            // TODO: 1 sprite for each corner (flipped)
            // TODO: 1 sprite for each edge (stretched/flipped)
            // TODO: 1 sprite for interior, stretched to fill area
            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 32 + 16 * i, 192 - 46, 32,
                                 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
            }
            break;
        }
        case MSG_INFO:
        case MSG_INFO_NOCLOSE:
        case MSG_ITEM:
        case MSG_INFO_CONT:
        case MSG_SIGN: {
            // construct mb 2, use default gfx addresses
            IO::loadSpriteB( "UI/mbox2", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX, 0, 192 - 46, 32, 64, false,
                             false, false, OBJPRIORITY_0, false );
            // TODO: 1 sprite for each corner (flipped)
            // TODO: 1 sprite for each edge (stretched/flipped)
            // TODO: 1 sprite for interior, stretched to fill area
            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSpriteB( SPR_MSGBOX_OAM + 13 - i, SPR_MSGBOX_GFX, 32 + 16 * i, 192 - 46, 32,
                                 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
            }
            break;
        }
        case MSG_MART_ITEM: {
            // mart uses more elaborate mb design, needs more gxf data
            IO::loadSpriteB( "UI/mboxmart", SPR_MSGBOX_OAM, SPR_MSGBOX_GFX_MART, 0, 192 - 51, 32,
                             64, false, false, false, OBJPRIORITY_0, false );
            for( u8 i = 0; i < 13; ++i ) {
                IO::loadSpriteB( SPR_MSGBOX_OAM + i + 1, SPR_MSGBOX_GFX_MART, 32 + 16 * i, 192 - 51,
                                 32, 64, 0, 0, 0, false, true, false, OBJPRIORITY_0, false );
            }
            break;
        }
        default: break;
        }
    }

    void doPrintMessage( const char* p_message, style p_style, u16 p_item,
                         const BAG::itemData* p_data, bool p_noDelay ) {
        IO::regularFont->setColor( 4, 3 );
        IO::regularFont->setColor( 5, 4 );
        u16 x = 12, y = 192 - 40, hg = 32;
        if( p_message ) {
            if( LOCATION_TIMER ) { hideLocation( ); }

            constructMBSprite( p_style );

            if( p_style == MSG_NORMAL || p_style == MSG_NOCLOSE || p_style == MSG_NORMAL_CONT ) {
                IO::regularFont->setColor( 1, 1 );
                IO::regularFont->setColor( 2, 2 );
            } else if( p_style == MSG_BRAILLE ) {
                IO::regularFont->setColor( 1, 1 );
                IO::regularFont->setColor( 2, 2 );
                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 0, 2 );
                IO::brailleFont->setColor( 0, 0 );
                IO::brailleFont->setColor( 1, 1 );
                IO::brailleFont->setColor( 2, 2 );
            } else if( p_style == MSG_INFO || p_style == MSG_INFO_NOCLOSE || p_style == MSG_ITEM
                       || p_style == MSG_INFO_CONT || p_style == MSG_SIGN ) {
                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 2, 2 );
                u16 lns = IO::regularFont->printBreakingStringC(
                    p_message, 0, 0, 192 - 20, true, IO::font::LEFT, 12, ' ', 0, false, -1 );
                if( p_style == MSG_ITEM ) {
                    x += 48;
                    if( lns == 1 ) {
                        y += 8;
                    } else {
                        y += 2;
                    }
                }
            } else if( p_style == MSG_MART_ITEM ) {
                p_noDelay = true;
                std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
                std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
                std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );

                IO::regularFont->setColor( 3, 1 );
                IO::regularFont->setColor( 0, 2 );
                x = 54, y = 192 - 50, hg = 64;

                u16 lns = IO::regularFont->printBreakingStringC(
                    p_message, 0, 0, 192 - 20, true, IO::font::LEFT, 12, ' ', 0, false, -1 );
                if( lns == 3 ) { y = 192 - 44; }
                if( lns <= 2 ) { y = 192 - 38; }
            }
        }

        if( !p_message ) {
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
            std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
        } else {
            u16  cpos    = 0;
            u16  tileCnt = p_style == MSG_MART_ITEM ? SPR_MSG_GFX_MART : SPR_MSG_GFX;
            bool sp      = false;
            IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;
            std::memset( TMP_TEXT_BUF, 0, sizeof( TMP_TEXT_BUF ) );

            u8   tmppos         = 0;
            char shortbuf[ 20 ] = { 0 };

            auto fnt = ( p_style == MSG_BRAILLE ) ? IO::brailleFont : IO::regularFont;

            while( p_message[ cpos ] ) {
                if( !p_noDelay ) {
                    // Check for special escaped characters ([escape sequence]), that need
                    // to be treated as a single character.
                    if( p_message[ cpos ] == '[' ) {
                        std::memset( shortbuf, 0, sizeof( shortbuf ) );
                        tmppos               = 0;
                        sp                   = true;
                        shortbuf[ tmppos++ ] = p_message[ cpos ];
                        ++cpos;
                        continue;
                    }
                    if( p_message[ cpos ] == ']' ) {
                        sp                   = false;
                        shortbuf[ tmppos++ ] = p_message[ cpos ];
                    } else if( !sp ) {
                        std::memset( shortbuf, 0, sizeof( shortbuf ) );
                        shortbuf[ tmppos = 0 ] = p_message[ cpos ];
                    }
                    if( sp ) {
                        shortbuf[ tmppos++ ] = p_message[ cpos ];
                        ++cpos;
                        continue;
                    }
                }

                tileCnt = p_style == MSG_MART_ITEM ? SPR_MSG_GFX_MART : SPR_MSG_GFX;
                u8 ln   = 1;
                if( p_noDelay ) {
                    std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
                    ln = fnt->printStringBC( p_message, TEXT_PAL, TEXT_BUF, 256 - ( 64 * !!p_item ),
                                             IO::font::LEFT, 16 - ( 4 * !!p_item ), 64, hg, 20 );
                } else {
                    std::strncat( TMP_TEXT_BUF, shortbuf, 20 );
                    if( !cpos ) { std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) ); }
                    ln = fnt->printStringBC( TMP_TEXT_BUF, TEXT_PAL, TEXT_BUF,
                                             256 - ( 64 * !!p_item ), IO::font::LEFT,
                                             16 - ( 4 * !!p_item ), 64, hg, 20 );
                }

                if( !p_noDelay ) {
                    if( ln == 1 || ( ln == 2 && p_message[ cpos ] == '\n' ) ) {
                        std::strncat( TEXT_CACHE_1, shortbuf, 20 );
                    } else if( ln == 2 || ( ln > 2 && p_message[ cpos ] == '\n' ) ) {
                        std::strncat( TEXT_CACHE_2, shortbuf, 20 );
                    } else {
                        waitForInteract( );
                        std::strncpy( TEXT_CACHE_1, TEXT_CACHE_2, 256 );
                        std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
                        std::strncat( TEXT_CACHE_2, shortbuf, 20 );
                        std::memset( TMP_TEXT_BUF, 0, sizeof( TMP_TEXT_BUF ) );
                        std::strncat( TMP_TEXT_BUF, TEXT_CACHE_1, 256 );
                        std::strncat( TMP_TEXT_BUF, shortbuf, 20 );
                    }
                }

                tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM, tileCnt, x, y, 64, hg, TEXT_BUF,
                                           64 * hg / 2, false, false, false, OBJPRIORITY_0, false );
                tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM + 1, tileCnt, x + 64, y, 64, hg,
                                           TEXT_BUF + 64 * hg, 64 * hg / 2, false, false, false,
                                           OBJPRIORITY_0, false );
                tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM + 2, tileCnt, x + 128, y, 64, hg,
                                           TEXT_BUF + 2 * 64 * hg, 64 * hg / 2, false, false, false,
                                           OBJPRIORITY_0, false );
                if( !p_item ) {
                    tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM + 3, tileCnt, x + 64 + 128, y, 64,
                                               hg, TEXT_BUF + 3 * 64 * hg, 64 * hg / 2, false,
                                               false, false, OBJPRIORITY_0, false );
                } else if( !cpos ) {
                    if( !p_data || p_data->m_itemType != BAG::ITEMTYPE_TM ) {
                        tileCnt = IO::loadItemIconB( p_item, 16, 192 - 40, SPR_MSGTEXT_OAM + 3,
                                                     tileCnt, false );
                    } else if( p_data && p_data->m_itemType == BAG::ITEMTYPE_TM ) {
                        BATTLE::moveData move = FS::getMoveData( p_data->m_param2 );

                        u8 tmtype = p_data->m_effect;
                        if( tmtype == 1 && BATTLE::isFieldMove( p_data->m_param2 ) ) { tmtype = 0; }
                        tileCnt = IO::loadTMIconB( move.m_type, tmtype, 16, 192 - 40,
                                                   SPR_MSGTEXT_OAM + 3, tileCnt, false );
                    }
                }
                IO::updateOAM( false );
                for( u8 i = 0;
                     i < 80
                             / ( IO::TEXTSPEED
                                 + SAVE::SAV.getActiveFile( ).m_options.m_textSpeedModifier );
                     ++i ) {
                    swiWaitForVBlank( );
                }
                if( p_noDelay ) { break; }
                cpos++;
            }

            if( p_style == MSG_NORMAL || p_style == MSG_INFO || p_style == MSG_NORMAL_CONT
                || p_style == MSG_INFO_CONT || p_style == MSG_ITEM || p_style == MSG_BRAILLE ) {
                // "Continue" char
                IO::regularFont->printCharB( 172, TEXT_PAL, CONT_BUF, 16, 0, 0 );
                tileCnt = IO::loadSpriteB( SPR_MSGCONT_OAM, SPR_MSGCONT_GFX, 254 - 12,
                                           192 - 40 + 24, 16, 16, CONT_BUF, 16 * 16 / 2, false,
                                           false, false, OBJPRIORITY_0, false );
            } else {
                IO::OamTop->oamBuffer[ SPR_MSGCONT_OAM ].isHidden = true;
            }
        }

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::regularFont->setColor( IO::BLUE_IDX, 3 );
        IO::regularFont->setColor( IO::BLUE2_IDX, 4 );
        IO::updateOAM( false );
    }

    void printMessage( const char* p_message, style p_style, bool p_noDelay ) {
        doPrintMessage( p_message, p_style, 0, 0, p_noDelay );

        if( p_style == MSG_NORMAL_CONT || p_style == MSG_INFO_CONT ) {
            waitForInteract( );
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
            std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
            IO::updateOAM( false );
        }
        if( p_style == MSG_NORMAL || p_style == MSG_INFO || p_style == MSG_BRAILLE ) {
            waitForInteract( );
            hideMessageBox( );
        }
    }

    void useItemFromPlayer( u16 p_itemId, u16 p_amount ) {
        auto data = FS::getItemData( p_itemId );
        auto cnt  = std::min( p_amount, SAVE::SAV.getActiveFile( ).m_bag.count(
                                           BAG::toBagType( data.m_itemType ), p_itemId ) );
        SAVE::SAV.getActiveFile( ).m_bag.erase( BAG::toBagType( data.m_itemType ), p_itemId, cnt );
        char buffer[ 100 ];
        auto iname = FS::getItemName( p_itemId );

        if( data.m_itemType == BAG::ITEMTYPE_TM ) {
            iname += " " + FS::getMoveName( data.m_param2 );
        }

        if( cnt > 1 ) {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_USED_ITEM_X_TIMES ), cnt, iname.c_str( ) );
        } else {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_USED_ITEM ), iname.c_str( ) );
        }
        doPrintMessage( buffer, MSG_ITEM, p_itemId, &data );
        waitForInteract( );
        hideMessageBox( );
    }

    void takeItemFromPlayer( u16 p_itemId, u16 p_amount ) {
        auto data = FS::getItemData( p_itemId );
        auto cnt  = std::min( p_amount, SAVE::SAV.getActiveFile( ).m_bag.count(
                                           BAG::toBagType( data.m_itemType ), p_itemId ) );
        SAVE::SAV.getActiveFile( ).m_bag.erase( BAG::toBagType( data.m_itemType ), p_itemId, cnt );
        char buffer[ 100 ];
        auto iname = FS::getItemName( p_itemId );

        if( data.m_itemType == BAG::ITEMTYPE_TM ) {
            iname += " " + FS::getMoveName( data.m_param2 );
        }

        if( cnt > 1 ) {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_HANDED_OVER_ITEM_X_TIMES ), cnt,
                      iname.c_str( ) );
        } else {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_HANDED_OVER_ITEM ), iname.c_str( ) );
        }
        doPrintMessage( buffer, MSG_ITEM, p_itemId, &data );
        waitForInteract( );
        hideMessageBox( );
    }

    void giveItemToPlayer( u16 p_itemId, u16 p_amount ) {
        auto data = FS::getItemData( p_itemId );
        SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::toBagType( data.m_itemType ), p_itemId,
                                                 p_amount );
        char buffer[ 100 ];
        auto iname = FS::getItemName( p_itemId );

        if( data.m_itemType == BAG::ITEMTYPE_TM ) {
            iname += " " + FS::getMoveName( data.m_param2 );
        }

        if( p_amount > 1 ) {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_OBTAINED_ITEM_X_TIMES ), p_amount,
                      iname.c_str( ) );
        } else {
            snprintf( buffer, 99, GET_STRING( IO::STR_UI_OBTAINED_ITEM ), iname.c_str( ) );
        }
        switch( data.m_itemType ) {
        case BAG::ITEMTYPE_KEYITEM: SOUND::playSoundEffect( SFX_OBTAIN_KEY_ITEM ); break;
        case BAG::ITEMTYPE_TM: SOUND::playSoundEffect( SFX_OBTAIN_TM ); break;
        default: SOUND::playSoundEffect( SFX_OBTAIN_ITEM ); break;
        }
        doPrintMessage( buffer, MSG_ITEM, p_itemId, &data );
        waitForInteract( );
        auto fmt = std::string( GET_STRING( IO::STR_UI_PUT_ITEM_INTO_BAG ) );
        snprintf(
            buffer, 99, fmt.c_str( ), iname.c_str( ), BAG::getItemChar( data.m_itemType ),
            GET_STRING( IO::STR_UI_BAG_PAGE_NAME_START + BAG::toBagType( data.m_itemType ) ) );
        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        std::memset( TEXT_CACHE_1, 0, sizeof( TEXT_CACHE_1 ) );
        std::memset( TEXT_CACHE_2, 0, sizeof( TEXT_CACHE_2 ) );
        doPrintMessage( buffer, MSG_ITEM, p_itemId, &data );
        waitForInteract( );
        hideMessageBox( );
    }

    u16  CURRENT_LOCATION = 0;
    u8   LOCATION_TIMER   = 0;
    void showNewLocation( u16 p_newLocation, bool ) {
        if( p_newLocation == CURRENT_LOCATION ) { return; }

        if( p_newLocation == L_POKEMON_CENTER || p_newLocation == L_POKEMON_MART ) { return; }

        CURRENT_LOCATION = p_newLocation;
        LOCATION_TIMER   = 120;

        std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
        if( FS::frameForLocation( p_newLocation ) == 3 || FS::frameForLocation( p_newLocation ) == 6
            || FS::frameForLocation( p_newLocation ) == 0
            || FS::frameForLocation( p_newLocation ) == 8
            || FS::frameForLocation( p_newLocation ) == 9
            || FS::frameForLocation( p_newLocation ) == 10 ) {
            IO::regularFont->setColor( 1, 1 );
        } else {
            IO::regularFont->setColor( 3, 1 );
        }
        IO::regularFont->setColor( 2, 2 );
        IO::regularFont->printStringBC( ( FS::getLocation( p_newLocation ) + " " ).c_str( ),
                                        TEXT_PAL, TEXT_BUF, 128, IO::font::CENTER );
        u16 tileCnt = SPR_MSG_GFX;
        u16 x = 0, y = 8;
        tileCnt = IO::loadSpriteB( SPR_MSGTEXT_OAM, tileCnt, x, y, 64, 32, TEXT_BUF, 64 * 32 / 2,
                                   false, false, false, OBJPRIORITY_0, false );
        tileCnt
            = IO::loadSpriteB( SPR_MSGTEXT_OAM + 1, tileCnt, x + 64, y, 64, 32, TEXT_BUF + 64 * 32,
                               64 * 32 / 2, false, false, false, OBJPRIORITY_0, false );
        tileCnt = IO::loadLocationBackB( FS::frameForLocation( p_newLocation ), 1, 1,
                                         SPR_MSGTEXT_OAM + 2, tileCnt, false );

        IO::regularFont->setColor( IO::WHITE_IDX, 1 );
        IO::regularFont->setColor( IO::GRAY_IDX, 2 );
        IO::updateOAM( false );
    }

    void hideLocation( u8 p_remTime ) {
        if( !p_remTime ) {
            LOCATION_TIMER = 0;
            std::memset( TEXT_BUF, 0, sizeof( TEXT_BUF ) );
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM ].isHidden     = true;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 1 ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 2 ].isHidden = true;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 3 ].isHidden = true;
        } else {
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM ].y -= 2;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 1 ].y -= 2;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 2 ].y -= 2;
            IO::OamTop->oamBuffer[ SPR_MSGTEXT_OAM + 3 ].y -= 2;
        }
        IO::updateOAM( false );
    }

} // namespace IO
