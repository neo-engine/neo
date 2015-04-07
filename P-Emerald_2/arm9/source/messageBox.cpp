/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : messageBox.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality message boxes

    Copyright (C) 2012 - 2015
    Philip Wellnitz (RedArceus)

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

#include "messageBox.h"
#include "sprite.h"
#include "item.h"
#include "fs.h"
#include "saveGame.h"
#include <cmath>
#include <vector>

#include "uio.h"
#include "defines.h"

#include "A.h"

namespace IO {

    void messageBox::clear( ) {
        printRectangle( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
    }
    void messageBox::clearButName( ) {
        printRectangle( (u8)72, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
    }


    messageBox::messageBox( item p_item, const u16 p_count ) {
        m_isNamed = NULL;

        initTextField( );
        initOAMTable( true );
        u16 c = 0;
        c = loadSprite( A_ID, 0, 0,
                        SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                        ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        loadItemIcon( p_item.m_itemName, 4, 4, 0, 1, c );
        updateOAM( true );
        swiWaitForVBlank( );

        regularFont->setColor( 253, 3 );
        regularFont->setColor( 254, 4 );

        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 0, 0, 15 );

        char buf[ 40 ];
        sprintf( buf, "%3d %s in der", p_count, p_item.getDisplayName( ).c_str( ) );
        regularFont->printMBStringD( buf, 32, 8, true );
        regularFont->printChar( 489 - 21 + p_item.getItemType( ), 32, 24, true );
        sprintf( buf, "%s-Tasche verstaut.`", BAG::bagnames[ p_item.m_itemType ].c_str( ) );
        ASpriteOamIndex = A_ID;
        regularFont->printMBStringD( buf, 46, 24, true );

        if( !FS::SAV->m_bag )
            FS::SAV->m_bag = new BAG::bag( );
        FS::SAV->m_bag->insert( BAG::toBagType( p_item.m_itemType ), p_item.getItemId( ), p_count );
    }

    messageBox::messageBox( const char* p_text, bool p_remsprites ) {
        m_isNamed = NULL;
        initTextField( );
        initOAMTable( true );
        loadSprite( A_ID, 0, 0,
                    SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                    ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );

        ASpriteOamIndex = A_ID;
        std::string text( p_text );
        regularFont->printMBStringD( ( text + '`' ).c_str( ), 8, 8, true );

        swiWaitForVBlank( );
    }
    messageBox::messageBox( const wchar_t* p_text, bool p_remsprites ) {
        m_isNamed = NULL;
        initTextField( );
        initOAMTable( true );
        loadSprite( A_ID, 0, 0,
                    SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                    ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );

        ASpriteOamIndex = A_ID;
        std::wstring text( p_text );
        regularFont->printMBStringD( ( text + L'`' ).c_str( ), 8, 8, true );

        swiWaitForVBlank( );
    }
    messageBox::messageBox( const char* p_text, const char* p_name, bool p_a, bool p_remsprites, sprite_type p_sprt, u16 p_sprind ) {
        m_isNamed = p_name;
        initOAMTable( true );
        u16 c = 0;
        c = loadSprite( A_ID, 0, 0,
                        SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                        ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );
        if( p_sprt != no_sprite ) {
            if( p_sprt == sprite_pkmn ) {
                c = loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_sprind, (u16)-16, 0, 0, 1, c, true, false, false, false, true );
            }
            if( p_sprt == sprite_trainer ) {
                c = loadPKMNSprite( "nitro:/PICS/SPRITES/TRAINER/", p_sprind, (u16)-16, 0, 0, 1, c, true, false, true, false, true );
            }
            updateOAM( true );
        }

        initTextField( );

        if( p_name )
            regularFont->printString( p_name, 8, 8, true );
        if( p_a ) {
            ASpriteOamIndex = 8;
            std::string text( p_text );
            regularFont->printMBStringD( ( text + '`' ).c_str( ), 64 * ( !!p_name ) + 8, 8, true );
        } else
            regularFont->printStringD( p_text, ( 64 * !!m_isNamed ) + 8, 8, true );

        if( !p_remsprites )
            return;
        if( p_sprt != no_sprite ) {
            Oam->oamBuffer[ p_sprind ].isHidden = true;
            updateOAM( true );
        }
        swiWaitForVBlank( );
    }
    messageBox::messageBox( const wchar_t* p_text, const wchar_t* p_name, bool p_a, bool p_remsprites, sprite_type p_sprt, u16 p_sprind ) {
        m_isNamed = NULL;
        initOAMTable( true );
        u16 c = 0;
        c = loadSprite( A_ID, 0, 0,
                        SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                        ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );
        if( p_sprt != no_sprite ) {
            if( p_sprt == sprite_pkmn ) {
                c = loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_sprind, (u16)-16, 0, 0, 1, c, true, false, false, false, true );
            }
            if( p_sprt == sprite_trainer ) {
                c = loadPKMNSprite( "nitro:/PICS/SPRITES/TRAINER/", p_sprind, (u16)-16, 0, 0, 1, c, true, false, true, false, true );
            }
            updateOAM( true );
        }

        initTextField( );

        if( p_name )
            regularFont->printString( p_name, 8, 8, true );
        if( p_a ) {
            ASpriteOamIndex = A_ID;
            std::wstring text( p_text );
            regularFont->printMBStringD( ( text + L'`' ).c_str( ), 64 * ( !!p_name ) + 8, 8, true );
        } else
            regularFont->printStringD( p_text, ( 64 * !!m_isNamed ) + 8, 8, true );

        if( !p_remsprites )
            return;
        if( p_sprt != no_sprite ) {
            Oam->oamBuffer[ p_sprind ].isHidden = true;
            updateOAM( true );
        }
        swiWaitForVBlank( );
    }

    void messageBox::put( const char* p_text, bool p_a ) {
        initTextField( );
        initOAMTable( true );
        loadSprite( A_ID, 0, 0,
                    SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                    ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );

        if( m_isNamed )
            regularFont->printString( m_isNamed, 8, 8, true );
        if( p_a ) {
            ASpriteOamIndex = A_ID;
            std::string text( p_text );
            regularFont->printMBStringD( ( text + '`' ).c_str( ), ( 64 * !!m_isNamed ) + 8, 8, true );
        } else
            regularFont->printStringD( p_text, ( 64 * !!m_isNamed ) + 8, 8, true );

        swiWaitForVBlank( );
    }

}