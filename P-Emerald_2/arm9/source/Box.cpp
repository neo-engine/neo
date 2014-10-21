/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : Box.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for Pokémon storage

    Copyright (C) 2012 - 2014
    Philip Wellnitz (RedArceus)

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:


    1.	The origin of this software must not be misrepresented; you
    must not claim that you wrote the original software. If you use
    this software in a product, an acknowledgment in the product
    is required.

    2.	Altered source versions must be plainly marked as such, and
    must not be misrepresented as being the original software.

    3.	This notice may not be removed or altered from any source
    distribution.
    */


#include "screenLoader.h"
#include "pokemon.h"
#include "fs.h"
#include <vector>

class berry;

extern int bg3sub;
extern int bg3;
extern int bg2sub;
extern int bg2;
extern saveGame SAV;

extern PrintConsole Top, Bottom;

POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
std::vector<int> box_of_st_pkmn[ MAXPKMN ];
std::vector<int> free_spaces;

extern PrintConsole Top, Bottom;
void dPage( int p_Page ) {

}

extern OAMTable *Oam;
extern SpriteInfo* spriteInfo;
const int MAXPKMNINBOXLIST = 7;
int drawBox( u16 p_pkmnId ) {
    --p_pkmnId;
    Oam->oamBuffer[ A_ID ].isHidden = true;
    Oam->oamBuffer[ BACK_ID ].isHidden = true;
    for( int i = BORDER_ID; i < BORDER_ID + 10; ++i )
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