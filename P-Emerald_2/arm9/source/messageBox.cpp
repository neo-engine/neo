/*
    Pokémon neo
    ------------------------------

    file        : messageBox.cpp
    author      : Philip Wellnitz
    description : Functionality message boxes

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

#include <cmath>
#include <vector>
#include "messageBox.h"
#include "fs.h"
#include "saveGame.h"
#include "sprite.h"

#include "defines.h"
#include "uio.h"

#include "A.h"

namespace IO {
    s16 curx = 8, cury = 8;

    void messageBox::clear( ) {
        printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true,  (u8) 250 );
        curx = 8;
        cury = 8;
    }
    void messageBox::clearButName( ) {
        printRectangle( (u8) 72, (u8) 0, (u8) 255, (u8) 63, true,  (u8) 250 );
        curx = 64 * ( !!m_isNamed ) + 8;
        cury = 8;
    }

    messageBox::messageBox( u16 p_itemId, ITEM::itemData& p_data, const u16 p_count ) {
        m_isNamed = NULL;

        initTextField( );
        initOAMTable( true );
        swiWaitForVBlank( );

        regularFont->setColor( 253, 3 );
        regularFont->setColor( 254, 4 );

        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 0, 0, 15 );

        char buf[ 40 ];
        if( BAG::toBagType( p_data.m_itemType ) != BAG::bag::bagType::TM_HM ) {
            if( BAG::toBagType( p_data.m_itemType ) != BAG::bag::bagType::KEY_ITEMS )
                sprintf( buf, GET_STRING( 89 ), p_count, ITEM::getItemName( p_itemId,
                            CURRENT_LANGUAGE ).c_str( ) );
            else
                sprintf( buf, GET_STRING( 88 ), ITEM::getItemName( p_itemId )
                        .c_str( ) );
            loadSprite( A_ID, 0, 0, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal, ATiles,
                        ATilesLen, false, false, true, OBJPRIORITY_0, true );
            loadItemIcon( p_itemId, 4, 4, 0, 1, 0 );

        } else {
            sprintf( buf, GET_STRING( 87 ),
                    ITEM::getItemName( p_itemId ).c_str( ),
                    MOVE::getMoveName( p_data.m_param1 ).c_str( ) );
            MOVE::moveData mdata = MOVE::getMoveData( p_data.m_param1 );
            IO::loadTMIcon( mdata.m_type, MOVE::isFieldMove( p_data.m_param1 ), 4, 4, 0, 1, 0 );
        }
        loadSprite( A_ID, 2, 64, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal, ATiles,
                    ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );
        s16 x = 32, y = 8;
        regularFont->printMBStringD( buf, x, y, true );
        regularFont->printChar( ITEM::getItemChar( p_data.m_itemType ), 32, 24, true );
        sprintf( buf, GET_STRING( 86 ), GET_STRING( 11 + BAG::toBagType( p_data.m_itemType ) ) );
        ASpriteOamIndex = A_ID;
        x               = 46;
        y               = 24;
        regularFont->printMBStringD( buf, x, y, true );

        SAVE::SAV.getActiveFile( ).m_bag.insert( BAG::toBagType( p_data.m_itemType ),
                                                  p_itemId, p_count );
    }

    messageBox::messageBox( const char* p_text, bool p_remsprites ) {
        m_isNamed = NULL;
        initTextField( );
        if( p_remsprites ) initOAMTable( true );
        loadSprite( A_ID, 0, 0, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal, ATiles,
                    ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );

        ASpriteOamIndex = A_ID;
        curx            = 8;
        cury            = 8;
        std::string text( p_text );
        regularFont->printMBStringD( ( text + '`' ).c_str( ), curx, cury, true );

        swiWaitForVBlank( );
    }
    messageBox::messageBox( const char* p_text, const char* p_name, bool p_a, bool p_remsprites,
                            sprite_type p_sprt, u16 p_sprind ) {
        m_isNamed = p_name;
        if( p_remsprites ) initOAMTable( true );
        u16 c = 0;
        c     = loadSprite( A_ID, 0, 0, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal, ATiles,
                        ATilesLen, false, false, true, OBJPRIORITY_0, true );
        updateOAM( true );
        if( p_sprt != no_sprite ) {
            if( p_sprt == sprite_pkmn ) {
                c = loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_sprind, (u16) -16, 0, 0, 1, c,
                                    true, false, false, false, true );
            }
            if( p_sprt == sprite_trainer ) {
                c = loadPKMNSprite( "nitro:/PICS/SPRITES/TRAINER/", p_sprind, (u16) -16, 0, 0, 1, c,
                                    true, false, true, false, true );
            }
            updateOAM( true );
        }

        initTextField( );

        curx = 64 * ( !!p_name ) + 8;
        cury = 8;
        if( p_name ) regularFont->printString( p_name, 8, 8, true );
        if( p_a ) {
            ASpriteOamIndex = A_ID;
            std::string text( p_text );
            regularFont->printMBStringD( ( text + '`' ).c_str( ), curx, cury, true );
        } else
            regularFont->printStringD( p_text, curx, cury, true );

        if( !p_remsprites ) return;
        if( p_sprt != no_sprite ) {
            Oam->oamBuffer[ p_sprind ].isHidden = true;
            updateOAM( true );
        }
        swiWaitForVBlank( );
    }

    void messageBox::carriageReturn( ) {
        curx = 64 * ( !!m_isNamed ) + 8;
    }

    void messageBox::put( const char* p_text, bool p_a ) {
        initOAMTable( true );
        if( p_a ) {
            loadSprite( A_ID, 0, 0, SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal, ATiles,
                        ATilesLen, false, false, true, OBJPRIORITY_0, true );
            updateOAM( true );
        }

        if( m_isNamed ) regularFont->printString( m_isNamed, 8, 8, true );
        if( p_a ) {
            ASpriteOamIndex = A_ID;
            std::string text( p_text );
            regularFont->printMBStringD( ( text + '`' ).c_str( ), curx, cury, true );
        } else
            regularFont->printStringD( p_text, curx, cury, true );

        swiWaitForVBlank( );
    }
} // namespace IO
