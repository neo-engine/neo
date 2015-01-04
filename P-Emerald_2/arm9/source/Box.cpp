/*
Pokémon Emerald 2 Version
------------------------------

file        : pokemon.h
author      : Philip Wellnitz (RedArceus)
description :

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


//DEPRECATED

#include "screenLoader.h"
#include "pokemon.h"
#include "fs.h"
#include <vector>

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
extern saveGame* SAV;

extern PrintConsole Top, Bottom;

POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
std::vector<int> box_of_st_pkmn[ MAXPKMN ];
std::vector<int> free_spaces;

void dPage( int p_Page ) {

}

const int MAXPKMNINBOXLIST = 7;
int drawBox( u16 p_pkmnId ) {
    --p_pkmnId;
    Oam->oamBuffer[ A_ID ].isHidden = true;
    Oam->oamBuffer[ BACK_ID ].isHidden = true;
    for( u8 i = BORDER_ID; i < BORDER_ID + 10; ++i )
        Oam->oamBuffer[ i ].isHidden = true;
    updateOAMSub( Oam );

    FS::loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PKMNInfoScreen" );

    consoleSetWindow( &Top, 0, 0, SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 );
    consoleSelect( &Top );
    consoleClear( );

    consoleSelect( &Bottom );
    consoleClear( );

    consoleSelect( &Bottom );
    consoleSetWindow( &Bottom, 1, 1, 32, 24 );
    printf( "Pok\x82""mon in dieser Box:\n\n>" );
    consoleSetWindow( &Bottom, 2, 3, 30, 20 );

    int page = 0, pagemax = 3;

    int acin = 0, max_ = std::min( MAXPKMNINBOXLIST, (int)box_of_st_pkmn[ p_pkmnId ].size( ) );
    std::string status_[ ] = { "   ", "BSG", "BRT", "GIF", "SLF", "PAR", "GFR" };
    for( int i = 0; i < max_; i++ ) {
        POKEMON::pokemon::boxPokemon& acPKMN = stored_pkmn[ box_of_st_pkmn[ p_pkmnId ][ i ] ];
        wprintf( &acPKMN.m_name[ 0 ] );
        if( acPKMN.m_pokerus % 16 )
            printf( " PKRS" );
        else if( acPKMN.m_pokerus )
            printf( " *" );
        //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
        //printf(" %s\n",&status_[acPKMN._status][0]);
    }
    dPage( page );

    touchPosition touch;
    while( 1 ) {
        touchRead( &touch );
        swiWaitForVBlank( );
        scanKeys( );
        int pressed = keysDown( );


        if( ( pressed & KEY_DOWN ) || ( ( touch.py > 178 ) && ( touch.px <= 220 ) && ( touch.px > 200 ) ) ) {
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_TOUCH )
                    break;
                if( keysUp( ) & KEY_DOWN )
                    break;
            }
DOWN:

            consoleSelect( &Bottom );
            consoleClear( );
            if( (int)box_of_st_pkmn[ p_pkmnId ].size( ) - acin - 1 )
                ++acin;
            max_ = acin + std::min( MAXPKMNINBOXLIST, (int)box_of_st_pkmn[ p_pkmnId ].size( ) );
            for( int i = acin; i < max_; i++ ) {
                POKEMON::pokemon::boxPokemon& acPKMN = stored_pkmn[ box_of_st_pkmn[ p_pkmnId ][ i ] ];
                wprintf( &acPKMN.m_name[ 0 ] );
                if( acPKMN.m_pokerus % 16 )
                    printf( " PKRS" );
                else if( acPKMN.m_pokerus )
                    printf( " *" );
                //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
                //printf(" %s\n",&status_[acPKMN._status][0]);
            }
        } else if( ( pressed & KEY_UP ) || ( ( touch.py > 178 ) && ( touch.px <= 240 ) && ( touch.px > 220 ) ) ) {
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_TOUCH )
                    break;
                if( keysUp( ) & KEY_UP )
                    break;
            }
UP:

            consoleSelect( &Bottom );
            consoleClear( );
            if( (int)box_of_st_pkmn[ p_pkmnId ].size( ) - acin - 1 )
                --acin;
            max_ = acin + std::min( MAXPKMNINBOXLIST, (int)box_of_st_pkmn[ p_pkmnId ].size( ) );
            for( int i = acin; i < max_; i++ ) {
                POKEMON::pokemon::boxPokemon& acPKMN = stored_pkmn[ box_of_st_pkmn[ p_pkmnId ][ i ] ];
                wprintf( &acPKMN.m_name[ 0 ] );
                if( acPKMN.m_pokerus % 16 )
                    printf( " PKRS" );
                else if( acPKMN.m_pokerus )
                    printf( " *" );
                //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
                //printf(" %s\n",&status_[acPKMN._status][0]);
            }
        } else if( ( pressed & KEY_B ) || ( ( touch.py > 178 ) && ( touch.px > 240 ) ) ) {
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_TOUCH )
                    break;
                if( keysUp( ) & KEY_B )
                    break;
            }
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            swiWaitForVBlank( );
            return pressed | KEY_B;
        } else if( pressed & KEY_RIGHT || ( ( touch.py > 178 ) && ( touch.px > 20 ) && ( touch.px <= 40 ) ) ) {
            if( ++page == pagemax )
                page = 0;
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_TOUCH )
                    break;
                if( keysUp( ) & KEY_RIGHT )
                    break;
            }
            dPage( page );
        } else if( pressed & KEY_LEFT || ( ( touch.py > 178 ) && ( touch.px <= 20 ) ) ) {
            if( --page == -1 )
                page = pagemax - 1;
            while( 1 ) {
                scanKeys( );
                if( keysUp( ) & KEY_TOUCH )
                    break;
                if( keysUp( ) & KEY_LEFT )
                    break;
            }
            dPage( page );
        } else if( pressed & KEY_L ) {
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            return KEY_L;
        } else if( pressed & KEY_R ) {
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            return KEY_R;
        } else if( pressed & KEY_A ) {
            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
            consoleClear( );
            int ret = stored_pkmn[ box_of_st_pkmn[ p_pkmnId ][ acin ] ].draw( );
            /*if(ret & KEY_UP)
            goto UP;
            else if(ret & KEY_DOWN)
            goto DOWN;*/
            consoleSetWindow( &Bottom, 1, 1, 32, 24 );
            consoleSelect( &Bottom );
            printf( "Pok\x82""mon in dieser Box:\n\n>" );
            consoleSetWindow( &Bottom, 2, 3, 30, 20 );
            for( int i = acin; i < max_; i++ ) {
                POKEMON::pokemon::boxPokemon& acPKMN = stored_pkmn[ box_of_st_pkmn[ p_pkmnId ][ i ] ];
                wprintf( &acPKMN.m_name[ 0 ] );
                if( acPKMN.m_pokerus % 16 )
                    printf( " PKRS" );
                else if( acPKMN.m_pokerus )
                    printf( " *" );
                //printf(" %i/%i KP",acPKMN.acHP,acPKMN.maxStats[0]);
                //printf(" %s\n",&status_[acPKMN._status][0]);
            }
        }
    }
}