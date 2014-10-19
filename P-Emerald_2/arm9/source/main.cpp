/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : main.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Main ARM9 entry point

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


#include <nds.h>
#include <fat.h>
#include "nitrofs.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include <fcntl.h>
#include <unistd.h>

#include "as_lib9.h" 

#include "map2d.h"
#include "buffer.h"
#include "hmMoves.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "messageBox.h"
#include "item.h"
#include "berry.h"
#include "font.h"
#include "print.h"
#include "fontData.h"
#include "battle.h"

#include "screenLoader.h"
#include "pokemon.h"
#include "saveGame.h"
//#include "Map.h"
#include "keyboard.h"
#include "sprite.h"

#include "Gen.h"

#include "Brother.h"
#include "Male.h"
#include "Female.h"

#include "BigCirc1.h" 

OAMTable *Oam = new OAMTable( );
SpriteInfo spriteInfo[ SPRITE_COUNT ];

OAMTable *OamTop = new OAMTable( );
SpriteInfo spriteInfoTop[ SPRITE_COUNT ];

#define _EMULATOR

enum GameMod {
    DEVELOPER,
    ALPHA,
    BETA,
    RELEASE,
    EMULATOR
};
#ifndef _EMULATOR
GameMod gMod = DEVELOPER;
#else
GameMod gMod = EMULATOR;
#endif

std::string CodeName = "Reborn Ho-Oh";
SavMod savMod = _NDS;

char acSlot2Game[ 5 ];

int bg3sub;
int bg2sub;
int bg3;
int bg2;

extern PrintConsole Top, Bottom;
ConsoleFont cfont;
Keyboard* kbd;
font::Font cust_font( font::font1::fontData, font::font1::fontWidths, font::font1::shiftchar );
font::Font cust_font2( font::font2::fontData, font::font2::fontWidths, font::font2::shiftchar );

//extern Map Maps[];
map2d::Map* acMap;

int hours, seconds, minutes, day, month, year;
int achours, acseconds, acminutes, acday, acmonth, acyear;
unsigned int ticks;

saveGame SAV;
const std::string sav_nam = "nitro:/SAV";
extern std::map<int, std::string> Locations;
Region acRegion = HOENN;
extern Region acMapRegion;
extern bool showmappointer;
extern void printMapLocation( const touchPosition& t );

screenLoader scrn( -2 );

extern POKEMON::pokemon::boxPokemon stored_pkmn[ MAXSTOREDPKMN ];
extern std::vector<int> box_of_st_pkmn[ MAXPKMN ];
extern std::vector<int> free_spaces;

extern void updateTime( int );

void whoCares( int ) {
    return;
}
void progress( int ) {
    return;
}

enum ChoiceResult {
    CONTINUE,
    NEW_GAME,
    OPTIONS,
    GEHEIMGESCHEHEN,
    TRANSFER_GAME,
    CANCEL
};
namespace POKEMON {
    extern char* getLoc( int p_ind );
}

bool playMp3( const char* p_path, const char* p_name ) {
    sprintf( buffer, "%s%s", p_path, p_name );
    auto f = fopen( buffer, "rb" );
    if( !f ) {
        fclose( f );
        return false;
    }
    fclose( f );
    AS_MP3StreamPlay( buffer );
    return true;
}
#define PLAYMp(path) if(!playMp3("./PERM2/SOUND/",(path)))\
    playMp3("nitro:/SOUND/",(path));


void fillWeiter( ) {
    cust_font.setColor( 0, 0 );
    cust_font.setColor( 251, 1 );
    cust_font.setColor( 252, 2 );

    BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
    if( SAV.m_isMale )
        BG_PALETTE_SUB[ 252 ] = RGB15( 0, 0, 31 );
    else
        BG_PALETTE_SUB[ 252 ] = RGB15( 31, 0, 0 );

    sprintf( buffer, "%ls", SAV.getName( ).c_str( ) );
    cust_font.printString( buffer, 128, 5, true );

    sprintf( buffer, "%s", POKEMON::getLoc( SAV.m_acMapIdx ) );
    cust_font.printString( "Ort:", 16, 23, true );
    cust_font.printString( buffer, 128, 23, true );


    sprintf( buffer, "%d:%02d", SAV.m_pt.m_hours, SAV.m_pt.m_mins );
    cust_font.printString( "Spielzeit:", 16, 37, true );
    cust_font.printString( buffer, 128, 37, true );

    sprintf( buffer, "%i", SAV.m_badges );
    cust_font.printString( "Orden:", 16, 51, true );
    cust_font.printString( buffer, 128, 51, true );

    sprintf( buffer, "%i", SAV.m_dex );
    cust_font.printString( "PokéDex:", 16, 65, true );
    cust_font.printString( buffer, 128, 65, true );
}
void killWeiter( ) {
    consoleSetWindow( &Bottom, 1, 1, 30, 22 );
    consoleSelect( &Bottom );
    consoleClear( );
}

ChoiceResult opScreen( ) {
    consoleSelect( &Top );
    consoleClear( );

    ChoiceResult results[ 5 ] = {
        CONTINUE,
        NEW_GAME,
        GEHEIMGESCHEHEN,
        OPTIONS,
        TRANSFER_GAME,
    };
    int MaxVal;
    std::pair<int, int> ranges[ 5 ] = {
        std::pair<int, int>( 0, 84 ),
        std::pair<int, int>( 87, 108 ),
        std::pair<int, int>( 113, 134 ),
        std::pair<int, int>( 139, 160 ),
        std::pair<int, int>( 165, 186 )
    };

    /* if(gMod == DEVELOPER){
    consoleSelect(&Bottom);
    consoleSetWindow(&Bottom,0,23,30,2);
    printf("Slot 2: %s",acSlot2Game);
    }*/

    switch( SAV.m_savTyp ) {
        case 3:
        {
            loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "MainMenu0" );
            MaxVal = 5;
            fillWeiter( );
            break;
        }
        case 2:
        {
            loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "MainMenu1" );
            MaxVal = 4;
            fillWeiter( );
            break;
        }
        case 1:
        {
            loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "MainMenu2" );
            MaxVal = 3;
            results[ 2 ] = OPTIONS;

            fillWeiter( );
            break;
        }
        case 0:
        {
            loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "MainMenu3" );

            MaxVal = 2;
            ranges[ 0 ] = std::pair<int, int>( 0, 20 );
            ranges[ 1 ] = std::pair<int, int>( 25, 40 );
            results[ 0 ] = NEW_GAME;
            results[ 1 ] = OPTIONS;

            break;
        }
        default:
        {
            killWeiter( );
            return CANCEL;
        }
    }

    //return OPTIONS;

    touchPosition touch;
    consoleSelect( &Bottom );
    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    while( 1 ) {
        swiWaitForVBlank( );

        scanKeys( );
        touch = touchReadXY( );
        //printf("(%d|%d|%d|%d)\n",touch.px,touch.py,touch.rawx,touch.rawy);
        //fflush(stdout);

        scanKeys( );
        int p = keysCurrent( );
        int k = keysHeld( ) | keysDown( );
        if( ( SAV.m_savTyp == 1 ) && ( k & KEY_SELECT ) && ( k & KEY_RIGHT ) && ( k & KEY_L ) && ( k & KEY_R ) ) {
            killWeiter( );
            consoleClear( );
            ++SAV.m_savTyp;
            return opScreen( );
        } else if( ( gMod == DEVELOPER ) && ( SAV.m_savTyp == 2 ) && ( k & KEY_START ) && ( k & KEY_LEFT ) && ( k & KEY_L ) && ( k & KEY_R ) ) {
            killWeiter( );
            consoleClear( );
            ++SAV.m_savTyp;
            return opScreen( );
        } else if( p & KEY_B ) {
            killWeiter( );
            consoleClear( );
            loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD" );
            loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD" );
            for( int i = 1; i < 256; ++i )
                BG_PALETTE_SUB[ i ] = RGB15( 31, 31, 31 );
            return CANCEL;
        }
        for( int i = 0; i < MaxVal; i++ )
            if( ( touch.py > ranges[ i ].first && touch.py < ranges[ i ].second ) ) {
            while( 1 ) {
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            killWeiter( );
            loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD" );
            loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD" );
            for( int j = 1; j < 256; ++j )
                BG_PALETTE_SUB[ j ] = RGB15( 31, 31, 31 );

            return results[ i ];
            }

        if( AS_GetMP3Status( ) != MP3ST_PLAYING )
            return CANCEL;
    }
}


void initNewGame( ) {
    AS_MP3Stop( );

    SAV = saveGame( );
    SAV.m_activatedPNav = false;
    SAV.m_money = 3000;
    SAV.m_Id = rand( ) % 65536;
    SAV.m_Sid = rand( ) % 65536;

    SAV.m_savTyp = 1;
    SAV.m_playtime = 0;
    SAV.m_HOENN_Badges = 0;
    SAV.m_KANTO_Badges = 0;
    SAV.m_JOHTO_Badges = 0;
    SAV.m_badges = 0;
    SAV.m_dex = 0;

    SAV.m_PkmnTeam.clear( );
    consoleSelect( &Bottom );
    consoleClear( );
    consoleSetWindow( &Bottom, 3, 10, 26, 5 );

    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );

    cust_font.setColor( 0, 0 );
    cust_font.setColor( 251, 1 );
    cust_font.setColor( 252, 2 );
    cust_font2.setColor( 0, 0 );
    cust_font2.setColor( 253, 1 );
    cust_font2.setColor( 254, 2 );

    BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    BG_PALETTE_SUB[ 251 ] = RGB15( 30, 30, 30 );
    BG_PALETTE_SUB[ 252 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
    BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );

    PLAYMp( "1001.mp3" );

    cust_font.printStringD( "Haaaaalt!", 24, 84, true );
    while( 1 ) {
        swiWaitForVBlank( );
        scanKeys( );
        if( keysCurrent( ) & KEY_TOUCH ) break;
        if( keysCurrent( ) & KEY_A ) break;
    }
    cust_font.printStringD( "Hier lang!", 100, 84, true );
    while( 1 ) {
        swiWaitForVBlank( );
        scanKeys( );
        if( keysCurrent( ) & KEY_TOUCH ) break;
        if( keysCurrent( ) & KEY_A ) break;
    }
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD", 16 );
    //loadPicture(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");

    free_spaces.clear( );
    for( int i = 0; i < MAXPKMN; i++ ) {
        SAV.m_inDex[ i ] = false;
        box_of_st_pkmn[ i ].clear( );
        free_spaces.push_back( i );
    }
    for( int i = 0; i < MAXSTOREDPKMN; ++i ) {
        //stored_pkmn[i] = 0;

        free_spaces.push_back( i );
    }

    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "NewGame" );
    cust_font.printStringD( "Hi, ich bin Maike, die\n""Tochter von Prof. Birk.", 24, 76, true );

    while( 1 ) {
        scanKeys( );
        if( keysUp( ) & KEY_TOUCH ) break;
        if( keysUp( ) & KEY_A ) break;
    }
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD", 16 );
    //loadPicture(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");
    //loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");
    cust_font.printStringD( "Da er gerade leider nicht in Hoenn\nist, werde ich euch heute euren\nPokéNav und euren PokéDex\nüberreichen.", 8, 68, true );

    while( 1 ) {
        scanKeys( );
        if( keysUp( ) & KEY_TOUCH ) break;
        if( keysUp( ) & KEY_A ) break;
    }
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD", 16 );
    //loadPicture(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");
    //loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");

    cust_font.printStringD( "So hier ist erstmal der PokéNav!", 8, 84, true );

    while( 1 ) {
        scanKeys( );
        if( keysUp( ) & KEY_TOUCH ) break;
        if( keysUp( ) & KEY_A ) break;
    }
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD", 16 );
    //loadPicture(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");
    //loadPicture(bgGetGfxPtr(bg3),"nitro:/PICS/","NewGame");
    cust_font.printStringD( "Ich gehe dann jetzt mal\ndie Dexe holen.\nIhr könnt solange eure\nPokéNav einrichten.", 24, 68, true );
    while( 1 ) {
        scanKeys( );
        if( keysUp( ) & KEY_TOUCH ) break;
        if( keysUp( ) & KEY_A ) break;
    }
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "ClearD", 16 );
    //loadPicture(bgGetGfxPtr(bg3sub),"nitro:/PICS/","ClearD");

    consoleSelect( &Bottom );
    std::wstring S_;

    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "PokeNav" );
    scrn.init( );
    setMainSpriteVisibility( true );
    Oam->oamBuffer[ 1 ].isHidden = true;
    updateOAMSub( Oam );
    AS_MP3Stop( );
    swiWaitForIRQ( );

    PLAYMp( "KeyItemGet.mp3" );
    AS_SetMP3Loop( false );
    messageBox M( "Du erhälst einen PokéNav." );
    while( AS_GetMP3Status( ) == MP3ST_PLAYING ) {
        swiWaitForIRQ( );
    }
    M.clear( );

    M.put( "Beginne automatische\nInitialisierung.", false );
    for( int i = 0; i < 120; ++i )
        swiWaitForVBlank( );


    AS_MP3Stop( );

    PLAYMp( "1000.mp3" );
    AS_SetMP3Loop( true );
    M.put( "Setze Heimatregion: Hoenn.", false );
    for( int i = 0; i < 120; ++i )
        swiWaitForVBlank( );
    M.put( "Setze Heimatstadt: Klippdelta City.", false );
    for( int i = 0; i < 120; ++i )
        swiWaitForVBlank( );

    char buffer[ 20 ];
    sprintf( buffer, "Setze ID: %05i", SAV.m_Id );
    M.put( buffer, false );

    for( int i = 0; i < 120; ++i )
        swiWaitForVBlank( );

    sprintf( buffer, "Setze SID: %05i", SAV.m_Sid );
    M.put( buffer, false );

    for( int i = 0; i < 120; ++i )
        swiWaitForVBlank( );
    consoleClear( );
    M.clear( );
    M = messageBox( "Automatische Initialisierung\nabgeschlossen." );
    consoleClear( );
    M.clear( );
INDIVIDUALISIERUNG:
    M = messageBox( "Beginne Individualisierung." );
    yesNoBox yn = yesNoBox( M );
    consoleSetWindow( &Bottom, 1, 1, 22, MAXLINES );
    SAV.m_isMale = !yn.getResult( "Bist du ein Mädchen?" );
    if( SAV.m_isMale )
        SAV.m_overWorldIdx = 0;
    else
        SAV.m_overWorldIdx = 10;
    consoleClear( );
    M.clear( );
    keyboard K = keyboard( );
    std::wstring Name = K.getText( 10, "Gib deinen Namen an!" );
    if( Name.empty( ) )
        if( SAV.m_isMale )
            SAV.setName( L"Basti" );
        else
            SAV.setName( L"Lari" );
    else
        SAV.setName( Name );
    yesNoBox YN = yesNoBox( );
    std::wstring S = L"Du bist also ";
    if( SAV.m_isMale )
        S += L"der\n";
    else
        S += L"die\n";
    S += SAV.getName( ); S += L"?";
    if( !YN.getResult( &( S[ 0 ] ) ) )
        goto INDIVIDUALISIERUNG;

    consoleClear( );
    M.clear( );
    M = messageBox( "Individualisierung\nabgeschlossen!" );
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "ClearD" );
    drawSub( );
    for( int i = 0; i < 30; i++ )
        swiWaitForVBlank( );
    dmaCopy( BrotherBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
    dmaCopy( BrotherPal, BG_PALETTE, 256 * 2 );
    if( SAV.m_isMale ) {
        M = messageBox( "Mir will sie den Dex\n""zuerst geben!", "???", true, true, true, messageBox::sprite_pkmn );
        consoleClear( );
        dmaCopy( FemaleBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( FemalePal, BG_PALETTE, 256 * 2 );
        M = messageBox( "Hey, jetzt spiel dich\nnicht so auf.\n", "???", true, true, false, messageBox::sprite_pkmn );
        consoleClear( );
        M = messageBox( "Ja, so ist er, mein\nBruder halt...\n"" Nich, Moritz?", "???", true, true, true, messageBox::sprite_pkmn );
        consoleClear( );
        dmaCopy( BrotherBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( BrotherPal, BG_PALETTE, 256 * 2 );
        M = messageBox( "Du bist doch\nnur neidisch!", "Moritz", true, true, true, messageBox::sprite_trainer, 0 );
        consoleClear( );
        dmaCopy( FemaleBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( FemalePal, BG_PALETTE, 256 * 2 );
        M = messageBox( "Tse...\n", "???", true, true, true, messageBox::sprite_pkmn );
        consoleClear( );
        M = messageBox( "Hi, ich bin Larissa,\n""aber Lari reicht auch.", "Lari", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Das da ist mein\nkleiner Bruder Moritz.", "Lari", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Wir kommen aus Azuria", "Lari", true, true, true, messageBox::sprite_trainer, 0 );
        loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind );
        for( int k = 0; k < 30; k++ )
            swiWaitForVBlank( );
        M = messageBox( "Das heißt eigentlich.", "Lari", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Als alle Kanto verlassen\nhaben, sind wir nach\nKlippdelta gezogen.", "Lari", true, true, true, messageBox::sprite_trainer, 0 );
        consoleClear( );
        loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind );
        for( int k = 0; k < 30; k++ )
            swiWaitForVBlank( );
        M = messageBox( "Du kommst auch aus\nKlippdelta?!", "Lari", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Oh...\n", "Lari", messageBox::sprite_trainer, 0 );
        loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind );
        for( int k = 0; k < 30; k++ )
            swiWaitForVBlank( );
        M = messageBox( "Na dann sehen wir uns ja\nwahrscheinlich noch öfter...", "Lari", true, true, true, messageBox::sprite_trainer, 0 );
        consoleClear( );
        loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind );
    } else {
        M = messageBox( "Mir wird sie den Dex\nzuerst geben!", "???", true, true, false, messageBox::sprite_pkmn, 0 );
        consoleClear( );
        dmaCopy( MaleBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( MalePal, BG_PALETTE, 256 * 2 );
        M = messageBox( "Und weiter?", "???", true, true, false, messageBox::sprite_pkmn, 0 );
        consoleClear( );
        dmaCopy( BrotherBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( BrotherPal, BG_PALETTE, 256 * 2 );
        M = messageBox( "Bist doch nur neidisch!", "???", true, true, false, messageBox::sprite_pkmn, 0 );
        consoleClear( );
        dmaCopy( MaleBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( MalePal, BG_PALETTE, 256 * 2 );
        M = messageBox( "Wozu...\n", "???", true, true, false, messageBox::sprite_pkmn, 0 );
        consoleClear( );
        M = messageBox( "Hi, ich bin Sebastian.", "???", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Nenn' mich ruhig Basti.", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Das da ist mein\nkleiner Bruder Moritz.", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Wir kommen aus Azuria.", "Basti", true, true, true, messageBox::sprite_trainer, 0 );
        loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind );
        for( int k = 0; k < 30; k++ )
            swiWaitForVBlank( );
        M = messageBox( "Das heißt eigentlich.", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Als alle Kanto verlassen\nhaben, sind wir\nnach Klippdelta.", "Basti", true, true, true, messageBox::sprite_trainer, 0 );
        consoleClear( );
        M = messageBox( "Du lebst auch in\nKlippdelta, nich? ", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        consoleClear( );
        M = messageBox( "Im hohen Gras dort sollen\nja Trasla auftauchen.", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Da muss ich mir dann\ngleich eins fangen!", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Dann habe ich bald ein\nGuardevoir.", "Basti", true, true, false, messageBox::sprite_trainer, 0 );
        M = messageBox( "Dann können wir ja mal\nkämpfen, du wohnst ja\nnur ein Haus weiter.", "Basti", true, true, true, messageBox::sprite_trainer, 0 );
        consoleClear( );
    }
    for( int k = 0; k < 45; k++ )
        swiWaitForVBlank( );
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "NewGame" );
    messageBox( "So, hier habt ihr das\nPokéDex-Modul für den\nPokéNav.", "Maike", true, true, false, messageBox::sprite_trainer, 0 );
    messageBox( "Einmal für dich.", "Maike", true, true, false, messageBox::sprite_trainer, 0 );
    messageBox( "Einmal für Moritz.", "Maike", true, true, false, messageBox::sprite_trainer, 0 );
    if( SAV.m_isMale )
        messageBox( "Und einmal für Lari.", "Maike", true, true, true, messageBox::sprite_trainer, 0 );
    else
        messageBox( "Und einmal für Basti.", "Maike", true, true, true, messageBox::sprite_trainer, 0 );

    M = messageBox( "Du erhälst das\nPokéDex-Modul für den PokéNav!" );


    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "NewGame" );
    messageBox( "Der PokéNav kann euch\nalles über eure\nPokémon liefern.", "Maike", true, true, false, messageBox::sprite_trainer, 0 );
    messageBox( "Man kann mit ihm andere\nTrainer anrufen und er\nhat 'ne Kartenfunktion.", "Maike", true, true, false, messageBox::sprite_trainer, 0 );
    dmaCopy( BrotherBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
    dmaCopy( BrotherPal, BG_PALETTE, 256 * 2 );
    messageBox( "Und er zeigt einem,\nwie man den Beutel\nam Besten packt!", "Moritz", true, true, false, messageBox::sprite_trainer, 0 );
    messageBox( "Moritz!", SAV.getName( ).c_str( ) );
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "NewGame" );
    messageBox( "...", "Maike", true, false, false, messageBox::sprite_trainer, 0 );
    messageBox( "Also, vergesst ihn ja nich'\nbei euch zu Hause, wenn\nihr auf Reisen geht!", "Maike", true, true, false, messageBox::sprite_trainer, 0 );
    messageBox( "Also dann, bis bald!", "Maike", true, true, true, messageBox::sprite_trainer, 0 );
    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "Clear" );

    S_ = SAV.getName( ); S_ += L",\n wir seh'n uns noch!";
    M = messageBox( &( S_[ 0 ] ), L"Moritz", true, true, true, messageBox::sprite_trainer, 0 );
    consoleSelect( &Top );
    SAV.m_hasPKMN = false;
    strcpy( SAV.m_acMapName, "0/98" );
    SAV.m_acMapIdx = 1000;
    SAV.m_acposx = 2 * 20;
    SAV.m_acposy = 25 * 20;
    SAV.m_acposz = 3;
    setMainSpriteVisibility( false );
    Oam->oamBuffer[ 1 ].isHidden = false;
    updateOAMSub( Oam );
    loadNavScreen( bgGetGfxPtr( bg3sub ), BGs[ BG_ind ].m_name.c_str( ), BG_ind );
}

void initVideo( ) {

    vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
    vramSetBankB( VRAM_B_MAIN_BG_0x06020000 );

    vramSetBankE( VRAM_E_MAIN_SPRITE );

    videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

    // set up our top bitmap background
    bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    bgSetPriority( bg3, 3 );
    bgUpdate( );
}
void initVideoSub( ) {

    vramSetBankC( VRAM_C_SUB_BG_0x06200000 );
    vramSetBankD( VRAM_D_SUB_SPRITE );


    /*  Set the video mode on the main screen. */
    videoSetModeSub( MODE_5_2D | // Set the graphics mode to Mode 5
                     DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                     DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                     DISPLAY_SPR_ACTIVE | // Enable sprites for display
                     DISPLAY_SPR_1D       // Enable 1D tiled sprites
                     );
}
void vramSetup( ) {
    initVideo( );
    initVideoSub( );
    VRAM_F_CR = VRAM_ENABLE | VRAM_F_BG_EXT_PALETTE | VRAM_OFFSET( 1 );
    vramSetBankG( VRAM_G_LCD );
    vramSetBankH( VRAM_H_LCD );
}

int lastdir;
int dir[ 5 ][ 2 ] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
enum MoveMode {
    WALK,
    SURF,
    BIKE
};
int MOV = 20;

bool cut::possible( ) {
    return false;
}
bool rockSmash::possible( ) {
    return false;
}
bool fly::possible( ) {
    return false;
}
bool flash::possible( ) {
    return true;
}
bool whirlpool::possible( ) {
    return false;
}
bool surf::possible( ) {
    return SAV.m_acMoveMode != SURF && acMap->m_blocks[ SAV.m_acposy / 20 + 10 + dir[ lastdir ][ 0 ] ][ SAV.m_acposx / 20 + 10 + dir[ lastdir ][ 1 ] ].m_movedata == 4;
}

bool heroIsBig = false;


void startScreen( ) {

    irqInit( );
    irqEnable( IRQ_VBLANK );
    irqSet( IRQ_VBLANK, AS_SoundVBL );    // needed for mp3 streaming

    vramSetup( );

    videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

    // set up our top bitmap background
    bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    //bgSet( bg3, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( bg3, 3 );
    bgUpdate( );

    // set up our bottom bitmap background
    bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
    // bgSet( bg3sub, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( bg3sub, 3 );
    bgUpdate( );
    //// set up our bottom bitmap background
    bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    //// bgSet( bg2sub, 0, 1<<8, 1<<8, 0, 0, 0, 0 );
    bgSetPriority( bg2sub, 2 );
    //bgUpdate();

    Top = *consoleInit( 0, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    Bottom = *consoleInit( 0, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );

    cfont.gfx = (u16*)fontTiles;
    cfont.pal = (u16*)fontPal;
    cfont.numChars = 218;
    cfont.numColors = 16;
    cfont.bpp = 8;
    cfont.asciiOffset = 32;
    cfont.convertSingleColor = false;

    consoleSetFont( &Top, &cfont );
    consoleSetFont( &Bottom, &cfont );

    //ReadSavegame
    if( gMod != EMULATOR ) {
        SAV = saveGame( whoCares );
        SAV.m_hasPKMN &= SAV.m_good;
    } else {
        SAV.m_hasPKMN = false;
        SAV.m_savTyp = 0;
    }
    // init the ASlib
    AS_Init( AS_MODE_MP3 | AS_MODE_16CH );

    // set default sound settings
    AS_SetDefaultSettings( AS_PCM_16BIT, 22050, AS_NO_DELAY );
START:
    //Intro


    if( !playMp3( "./PERM2/SOUND/", "Intro.mp3" ) )
        playMp3( "nitro:/SOUND/", "Intro.mp3" );
    AS_SetMP3Loop( false );
    AS_SetMP3Volume( 127 );

    //StartScreen

    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "Title" );
    if( BGs[ BG_ind ].m_allowsOverlay )
        drawSub( );
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "Clear" );

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );

    BG_PALETTE[ 3 ] = BG_PALETTE_SUB[ 3 ] = RGB15( 0, 0, 0 );

    printf( "@ Philip \"RedArceus\" Wellnitz\n                     2012 - 2014\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
    if( gMod == DEVELOPER )
        printf( "             Developer's version\n" );
    else if( gMod == BETA )
        printf( "                            Beta\n" );
    else if( gMod == ALPHA )
        printf( "                           Alpha\n" );
    else if( gMod == EMULATOR )
        printf( "                        Emulator\n" );
    if( gMod != RELEASE ) {
        std::string s = "\"" + CodeName + "\"";
        s.insert( s.begin( ), 32 - s.length( ), ' ' );
        printf( &( s[ 0 ] ) );
    }

    consoleSetWindow( &Top, 0, 23, 32, 1 );
    consoleSelect( &Top );
    int D0000 = 0;
    touchPosition tp;
    while( 1 ) {
        scanKeys( );
        tp = touchReadXY( );
        swiWaitForVBlank( );

        int pressed = keysCurrent( );
        if( ( pressed & KEY_A ) || ( pressed & KEY_START ) || ( tp.px || tp.py ) )
            break;
        ++D0000;
        if( !( D0000 % 120 ) ) {
            printf( "     BER\x9A""HRE, UM ZU STARTEN" );
            D0000 = 0;
        } else if( ( D0000 % 120 ) == 60 )
            consoleClear( );
        if( AS_GetMP3Status( ) != MP3ST_PLAYING )
            goto START;
    }

    while( tp.px || tp.py ) {
        scanKeys( );
        tp = touchReadXY( );
        swiWaitForVBlank( );
    }

    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    consoleSelect( &Bottom );
    consoleClear( );
    consoleSelect( &Top );

    time_t uTime = time( NULL );
    tm* tStruct = gmtime( (const time_t *)&uTime );

    hours = tStruct->tm_hour;
    minutes = tStruct->tm_min;
    seconds = tStruct->tm_sec;
    day = tStruct->tm_mday;
    month = tStruct->tm_mon + 1;
    year = tStruct->tm_year + 1900;

    ticks = 0;
    timerStart( 0, ClockDivider_1024, 0, NULL );
    ticks += timerElapsed( 0 );

    srand( hours + ( 100 * minutes ) + ( 10000 * seconds ) / ( day + ( 100 * month ) + year ) );
    POKEMON::LastPID = rand( );

    //StartMenu
    AS_SetMP3Volume( 31 );

    switch( opScreen( ) ) {
        case TRANSFER_GAME:
        {
            char cmpgm[ 5 ][ 4 ] = { "BPE", "AXP", "AXV", "BPR", "BPG" };
            int acgame = -1;

            for( int i = 0; i < 5; ++i ) {
                for( int j = 0; j < 3; ++j )
                    if( cmpgm[ i ][ j ] != acSlot2Game[ j ] )
                        goto CONT;
                acgame = i;
CONT:
                ;
            }
            if( acgame == -1 )
                goto START;

            scrn.init( );
            yesNoBox yn = yesNoBox( messageBox( "", 0, false, false, false ) );
            if( yn.getResult( "Möchtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?", false ) ) {
                messageBox( "Solltest du im Folgenden\nspeichern, so werden Daten\nauf das GBA-Modul geschrieben.", false, false );
                messageBox( "Bitte entferne daher das\nGBA-Modul nicht, es könnte\nden Spielstand beschädigen.", false, false );
                messageBox( "Auch das Speichern an sich\nkann den Spielstand\nbeschädigen.", false, false );
                yn = yesNoBox( );
                if( yn.getResult( "Möchtest du fortfahren?", false ) ) {
                    messageBox( "Lade Spielstand...", 0, false, false, false );
                    int loadgame = acgame > 2 ? 1 : 0;

                    gen3::SaveParser* save3 = gen3::SaveParser::Instance( );

                    if( save3->load( loadgame ) == -1 ) {
                        messageBox( "Ein Fehler ist aufgetreten.\nKehre zum Hauptmenü zurück." );
                        goto START;
                    }
                    SAV = saveGame( );
                    wchar_t savname[ 8 ] = { 0 };
                    for( int i = 0; i < 7; ++i )
                        savname[ i ] = gen3::getNText( save3->unpackeddata[ i ] );
                    SAV.setName( savname );

                    SAV.m_isMale = !save3->unpackeddata[ 8 ];

                    SAV.m_Id = ( save3->unpackeddata[ 11 ] << 8 ) | save3->unpackeddata[ 10 ];
                    SAV.m_Sid = ( save3->unpackeddata[ 13 ] << 8 ) | save3->unpackeddata[ 12 ];

                    SAV.m_pt.m_hours = ( save3->unpackeddata[ 15 ] << 8 ) | save3->unpackeddata[ 14 ];
                    SAV.m_pt.m_mins = save3->unpackeddata[ 16 ];
                    SAV.m_pt.m_secs = save3->unpackeddata[ 17 ];

                    SAV.m_gba.m_gameid = ( save3->unpackeddata[ 0xaf ] << 24 ) | ( save3->unpackeddata[ 0xae ] << 16 ) | ( save3->unpackeddata[ 0xad ] << 8 ) | save3->unpackeddata[ 0xac ];

                    POKEMON::PKMNDATA::pokemonData p;
                    for( int i = 0; i < 6; ++i ) {
                        if( save3->pokemon[ i ]->personality ) {
                            SAV.m_PkmnTeam.push_back( POKEMON::pokemon( ) );

                            POKEMON::pokemon &acPkmn = SAV.m_PkmnTeam[ i ];
                            gen3::belt_pokemon_t* &acBeltP = save3->pokemon[ i ];


                            acPkmn.m_boxdata.m_PID = acBeltP->personality;
                            acPkmn.m_boxdata.m_SID = acBeltP->otid >> 16;
                            acPkmn.m_boxdata.m_ID = acBeltP->otid % ( 1 << 16 );
                            for( int i = 0; i < 10; ++i )
                                acPkmn.m_boxdata.m_Name[ i ] = gen3::getNText( acBeltP->name[ i ] );
                            acPkmn.m_boxdata.m_Name[ 10 ] = 0;
                            acPkmn.m_boxdata.m_hometown = acBeltP->language;
                            for( int i = 0; i < 7; ++i )
                                acPkmn.m_boxdata.m_OT[ i ] = gen3::getNText( acBeltP->otname[ i ] );
                            acPkmn.m_boxdata.m_OT[ 7 ] = 0;
                            acPkmn.m_boxdata.m_markings = acBeltP->markint;

                            acPkmn.m_statusint = acBeltP->status;
                            acPkmn.m_Level = acBeltP->level;
                            acPkmn.m_boxdata.m_PKRUS = acBeltP->pokerus;

                            acPkmn.m_stats.m_acHP = acBeltP->currentHP;
                            acPkmn.m_stats.m_maxHP = acBeltP->maxHP;
                            acPkmn.m_stats.m_Atk = acBeltP->move;
                            acPkmn.m_stats.m_Def = acBeltP->defense;
                            acPkmn.m_stats.m_SAtk = acBeltP->spatk;
                            acPkmn.m_stats.m_SDef = acBeltP->spdef;
                            acPkmn.m_stats.m_Spd = acBeltP->speed;

                            gen3::pokemon::pokemon_growth_t* &acBG = save3->pokemon_growth[ i ];
                            acPkmn.m_boxdata.m_SPEC = gen3::getNPKMNIdx( acBG->species );
                            acPkmn.m_boxdata.m_Item = gen3::getNItemIdx( acBG->held );
                            acPkmn.m_boxdata.m_exp = acBG->xp;
                            acPkmn.m_boxdata.m_steps = acBG->happiness;
                            acPkmn.m_boxdata.m_PPUps = acBG->ppbonuses;

                            gen3::pokemon::pokemon_moves_t* &acBA = save3->pokemon_moves[ i ];
                            for( int i = 0; i < 4; ++i ) {
                                acPkmn.m_boxdata.m_Attack[ i ] = acBA->atk[ i ];
                                acPkmn.m_boxdata.m_AcPP[ i ] = acBA->pp[ i ];
                            }

                            gen3::pokemon::pokemon_effort_t* &acBE = save3->pokemon_effort[ i ];
                            for( int i = 0; i < 6; ++i ) {
                                acPkmn.m_boxdata.m_EV[ i ] = acBE->EV[ i ];
                                acPkmn.m_boxdata.m_ConStats[ i ] = acBE->ConStat[ i ];
                            }

                            gen3::pokemon::pokemon_misc_t* &acBM = save3->pokemon_misc[ i ];
                            acPkmn.m_boxdata.m_IVint = acBM->IVint;

                            POKEMON::PKMNDATA::getAll( acPkmn.m_boxdata.m_SPEC, p );
                            acPkmn.m_boxdata.m_ability = p.m_abilities[ acPkmn.m_boxdata.m_IV.m_isEgg ];
                            acPkmn.m_boxdata.m_IV.m_isEgg = acPkmn.m_boxdata.m_IV.m_isNicked;
                            acPkmn.m_boxdata.m_gotPlace = gen3::getNLocation( acBM->locationcaught );

                            acPkmn.m_boxdata.m_gotLevel = acBM->levelcaught;

                            if( acPkmn.m_boxdata.m_IV.m_isEgg || acPkmn.m_boxdata.m_gotLevel ) {
                                acPkmn.m_boxdata.m_hatchPlace = 999;
                                acPkmn.m_boxdata.m_gotLevel = 5;
                                acPkmn.m_boxdata.m_hatchDate[ 0 ] =
                                    acPkmn.m_boxdata.m_hatchDate[ 1 ] =
                                    acPkmn.m_boxdata.m_hatchDate[ 2 ] = 0;
                                acPkmn.m_boxdata.m_gotDate[ 0 ] =
                                    acPkmn.m_boxdata.m_gotDate[ 1 ] =
                                    acPkmn.m_boxdata.m_gotDate[ 2 ] = 1;
                            }
                            acPkmn.m_boxdata.m_OTisFemale = acBM->tgender;
                            acPkmn.m_boxdata.m_Ball = acBM->pokeball;
                            acPkmn.m_boxdata.m_gotDate[ 0 ] =
                                acPkmn.m_boxdata.m_gotDate[ 1 ] =
                                acPkmn.m_boxdata.m_gotDate[ 2 ] = 0;

                            SAV.m_hasPKMN = true;
                        }
                    }
                    savMod = _GBA;
                    //load player to default pos
                    strcpy( SAV.m_acMapName, "0/98" );
                    SAV.m_acMapIdx = 1000;
                    SAV.m_acposx = 2 * 20;
                    SAV.m_acposy = 25 * 20;
                    SAV.m_acposz = 3;

                    SAV.m_overWorldIdx = 20 * ( ( acgame + 1 ) / 2 ) + ( SAV.m_isMale ? 0 : 10 );

                    Oam->oamBuffer[ SAVE_ID ].isHidden = false;

                    messageBox( "Abgeschlossen." );
                    break;
                } else goto START;
            } else goto START;
        }
        case GEHEIMGESCHEHEN:
        case CANCEL:
            //printf("%i",SAV.SavTyp);
            //while(1);
            if( gMod != EMULATOR )
                goto START;
        case OPTIONS:
            scrn.init( );
            SAV = saveGame( );
            SAV.m_activatedPNav = false;
            SAV.m_money = 3000;
            SAV.m_Id = rand( ) % 65536;
            SAV.m_Sid = rand( ) % 65536;

            SAV.m_savTyp = 1;
            SAV.m_playtime = 0;
            SAV.m_HOENN_Badges = 0;
            SAV.m_KANTO_Badges = 0;
            SAV.m_JOHTO_Badges = 0;
            SAV.m_badges = 0;
            SAV.m_dex = 0;
            SAV.m_hasPKMN = false;

            SAV.m_PkmnTeam.clear( );
            free_spaces.clear( );
            for( int i = 0; i < MAXPKMN; i++ ) {
                SAV.m_inDex[ i ] = false;
                box_of_st_pkmn[ i ].clear( );
                free_spaces.push_back( i );
            }
            for( int i = 0; i < MAXSTOREDPKMN; ++i ) {
                //stored_pkmn[i] = 0;
                free_spaces.push_back( i );
            }

            SAV.m_overWorldIdx = 0;
            strcpy( SAV.m_acMapName, "0/98" );
            SAV.m_acMapIdx = 1000;
            SAV.m_acposx = 2 * 20, SAV.m_acposy = 25 * 20, SAV.m_acposz = 3;
            break;
        case CONTINUE:
            scrn.init( );
            break;
        case NEW_GAME:
            initNewGame( );
    }
    swiWaitForVBlank( );
    swiWaitForIRQ( );
}


int mode = -1;
void showNewMap( int p_mapIdx ) {
    AS_MP3Stop( );
    for( int i = 0; i < 3; ++i ) {
        for( int j = 0; j < 75; ++j ) {
            MapRegionPos m = MapLocations[ i ][ j ];
            if( m.m_ind != p_mapIdx )
                continue;
            acMapRegion = Region( i + 1 );
            showmappointer = true;
            scrn.draw( mode = 1 + i );
            printMapLocation( m );
            Oam->oamBuffer[ SQCH_ID ].x = Oam->oamBuffer[ SQCH_ID + 1 ].x = ( m.m_lx + m.m_rx ) / 2 - 8;
            Oam->oamBuffer[ SQCH_ID ].y = Oam->oamBuffer[ SQCH_ID + 1 ].y = ( m.m_ly + m.m_ry ) / 2 - 8;
            Oam->oamBuffer[ SQCH_ID ].isHidden = Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = false;
            updateOAMSub( Oam );

            char buffer[ 120 ] = { 0 };
            sprintf( buffer, "%d.mp3", p_mapIdx );
            if( !playMp3( "./PERM2/SOUND/", buffer ) )
                playMp3( "nitro:/SOUND/", buffer );
            swiWaitForIRQ( );
            swiWaitForVBlank( );
            return;
        }
    }
}

bool left = false;
void loadframe( SpriteInfo* p_si, int p_idx, int p_frame, bool p_big = false ) {
    char buffer[ 50 ];
    sprintf( buffer, "%i/%i", p_idx, p_frame );
    if( !p_big )
        loadSprite( p_si, "nitro://PICS/SPRITES/OW/", buffer, 64, 16 );
    else
        loadSprite( p_si, "nitro://PICS/SPRITES/OW/", buffer, 128, 16 );
}

void animateHero( int p_dir, int p_frame, bool p_runDisable = false ) {
    heroIsBig = false;

    left = !left;
    bool bike = (MoveMode)SAV.m_acMoveMode == BIKE, run = ( keysHeld( ) & KEY_B ) && !p_runDisable;
    if( p_frame == 0 ) {
        switch( p_dir ) {
            case 0:
                OamTop->oamBuffer[ 0 ].hFlip = false;
                loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 0, heroIsBig );
                updateOAM( OamTop );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForIRQ( );
                return;
            case 1:
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                OamTop->oamBuffer[ 0 ].hFlip = true;
                if( !run ) {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 7, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 8, heroIsBig );
                } else {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 16, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 17, heroIsBig );
                }

                updateOAM( OamTop );
                swiWaitForVBlank( );
                swiWaitForIRQ( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            case 2:
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run ) {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 3, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 4, heroIsBig );
                } else {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 12, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 13, heroIsBig );
                }
                updateOAM( OamTop );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            case 3:
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run ) {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 7, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 8, heroIsBig );
                } else {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 16, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 17, heroIsBig );
                }
                updateOAM( OamTop );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            case 4:
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run ) {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 5, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 6, heroIsBig );
                } else {
                    if( left )
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 14, heroIsBig );
                    else
                        loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 15, heroIsBig );
                }
                updateOAM( OamTop );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                return;
            default:
                break;
        }
    }
    if( p_frame == 1 ) {
        switch( p_dir ) {
            case 0:
                OamTop->oamBuffer[ 0 ].hFlip = false;
                loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 0, heroIsBig );
                updateOAM( OamTop );
                return;
            case 1:
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                OamTop->oamBuffer[ 0 ].hFlip = true;
                updateOAM( OamTop );
                if( !run )
                    swiWaitForVBlank( );
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 2, heroIsBig );
                else
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 11, heroIsBig );
                updateOAM( OamTop );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 2, 0 );
                bgUpdate( );
                return;
            case 2:
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                updateOAM( OamTop );
                if( !run )
                    swiWaitForVBlank( );
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 0, heroIsBig );
                else
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 9, heroIsBig );
                updateOAM( OamTop );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, 2 );
                bgUpdate( );
                return;
            case 3:
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                OamTop->oamBuffer[ 0 ].hFlip = false;
                updateOAM( OamTop );
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 2, heroIsBig );
                else
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 11, heroIsBig );
                updateOAM( OamTop );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], -2, 0 );
                bgUpdate( );
                return;
            case 4:
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                if( !run )
                    swiWaitForVBlank( );
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 1, heroIsBig );
                else
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 10, heroIsBig );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                updateOAM( OamTop );
                bgUpdate( );
                swiWaitForVBlank( );
                for( int i = 1; i < 4; ++i )
                    bgScroll( map2d::bgs[ i ], 0, -2 );
                bgUpdate( );
                return;
            default:
                break;
        }
    }
    if( p_frame == 2 ) {
        switch( p_dir ) {
            case 0:
                OamTop->oamBuffer[ 0 ].hFlip = false;
                loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 0 );
                updateOAM( OamTop );
                return;
            case 1:
                OamTop->oamBuffer[ 0 ].hFlip = true;
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 2, heroIsBig );
                //else
                //    loadframe(&spriteInfoTop[0],SAV.owIdx,11,heroIsBig);
                updateOAM( OamTop );
                return;
            case 2:
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 0, heroIsBig );
                //else
                //    loadframe(&spriteInfoTop[0],SAV.owIdx,9,heroIsBig);
                updateOAM( OamTop );
                return;
            case 3:
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 2, heroIsBig );
                //else
                //    loadframe(&spriteInfoTop[0],SAV.owIdx,11,heroIsBig);
                updateOAM( OamTop );
                return;
            case 4:
                OamTop->oamBuffer[ 0 ].hFlip = false;
                if( !run )
                    loadframe( &spriteInfoTop[ 0 ], SAV.m_overWorldIdx, 1, heroIsBig );
                //else
                //    loadframe(&spriteInfoTop[0],SAV.owIdx,10,heroIsBig);
                updateOAM( OamTop );
                return;
            default:
                break;
        }
    }
}

inline void movePlayer( int p_direction ) {
    acMap->movePlayer( p_direction );
}

bool movePlayerOnMap( int p_x, int p_y, int p_z, bool p_init /*= true*/ ) {
    bool WTW = ( gMod == DEVELOPER ) && ( keysHeld( ) & KEY_R );

    MoveMode playermoveMode = (MoveMode)SAV.m_acMoveMode;

    p_x += 10;
    p_y += 10;

    if( p_x < 0 )
        return false;
    if( p_y < 0 )
        return false;
    if( p_x >= (int)acMap->m_sizey + 20 )
        return false;
    if( p_y >= (int)acMap->m_sizex + 20 )
        return false;

    int lastmovedata = acMap->m_blocks[ SAV.m_acposy / 20 + 10 ][ SAV.m_acposx / 20 + 10 ].m_movedata;
    int acmovedata = acMap->m_blocks[ p_y ][ p_x ].m_movedata;
    map2d::Block acBlock = acMap->m_blockSets.m_blocks[ acMap->m_blocks[ p_y ][ p_x ].m_blockidx ],
        lastBlock = acMap->m_blockSets.m_blocks[ acMap->m_blocks[ SAV.m_acposy / 20 + 10 ][ SAV.m_acposx / 20 + 10 ].m_blockidx ];

    int verhalten = acBlock.m_bottombehave, hintergrund = acBlock.m_topbehave;
    int lstverhalten = lastBlock.m_bottombehave, lsthintergrund = lastBlock.m_topbehave;
    if( verhalten == 0xa0 && playermoveMode != WALK ) //nur normales laufen möglich
        return false;

    if( verhalten == 0xc1 && p_y != SAV.m_acposy / 20 + 10 ) //Rechts-Links-Blockung
        return false;
    if( verhalten == 0xc0 && p_x != SAV.m_acposx / 20 + 10 ) //Oben-Unten-Blockung
        return false;
    if( verhalten >= 0xd3 && verhalten <= 0xd7 ) //fester block
        return false;

    if( !WTW ) {
        if( acmovedata == 1 )
            return false;
        if( ( acmovedata == 4 && playermoveMode != SURF ) )
            return false;
    }
    if( acmovedata == 0xc && playermoveMode == SURF ) {
        SAV.m_acMoveMode = WALK;

    }

    int movedir = 0;
    int oldx = SAV.m_acposy / 20 + 10, oldy = SAV.m_acposx / 20 + 10;
    if( oldy < p_x )
        movedir = 1;
    else if( oldy > p_x )
        movedir = 3;
    else if( oldx < p_y )
        movedir = 2;
    else if( oldx > p_y )
        movedir = 4;

    if( lastmovedata == 0 && acmovedata % 4 == 0 )
        SAV.m_acposz = p_z = acmovedata / 4;

    OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_2;
    if( ( verhalten == 0x70 || lstverhalten == 0x70 ) && p_z >= 4 )
        OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_1;
    if( acmovedata == 60 ) {
        if( p_z <= 3 )
            OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_3;
        else
            OamTop->oamBuffer[ 0 ].priority = OBJPRIORITY_1;
    }
    if( WTW || ( acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) )
        animateHero( movedir, 0 );
    else {
        animateHero( movedir, 2 );
        bgUpdate( );
        return false;
    }

    if( p_x < 10 ) {
        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'W' && p_y >= a.m_move + 10 && p_y < a.m_move + a.m_mapsx + 10 ) {
                    showNewMap( a.m_mapidx );
                    free( acMap );
                    strcpy( SAV.m_acMapName, a.m_name );
                    SAV.m_acMapIdx = a.m_mapidx;
                    acMap = new map2d::Map( "nitro://MAPS/", a.m_name );
                    p_y -= a.m_move;
                    p_x = a.m_mapsy + 10;
                    SAV.m_acposx = 20 * ( p_x - 10 );
                    SAV.m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 17, p_y - 18, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_y < 10 ) {
        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'N' && p_x >= a.m_move + 10 && p_x < a.m_move + a.m_mapsy + 10 ) {
                    showNewMap( a.m_mapidx );
                    free( acMap );
                    strcpy( SAV.m_acMapName, a.m_name );
                    SAV.m_acMapIdx = a.m_mapidx;
                    acMap = new map2d::Map( "nitro://MAPS/", a.m_name );
                    p_x -= a.m_move;
                    p_y = a.m_mapsx + 10;
                    SAV.m_acposx = 20 * ( p_x - 10 );
                    SAV.m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 16, p_y - 19, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_x >= (int)acMap->m_sizey + 10 ) {
        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'E' && p_y >= a.m_move + 10 && p_y < a.m_move + a.m_mapsx + 10 ) {
                    showNewMap( a.m_mapidx );
                    free( acMap );
                    strcpy( SAV.m_acMapName, a.m_name );
                    SAV.m_acMapIdx = a.m_mapidx;
                    acMap = new map2d::Map( "nitro://MAPS/", a.m_name );
                    p_y -= a.m_move;
                    p_x = 9;
                    SAV.m_acposx = 20 * ( p_x - 10 );
                    SAV.m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 15, p_y - 18, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_y >= (int)acMap->m_sizex + 10 ) {

        if( WTW || acmovedata == 4 || ( acmovedata % 4 == 0 && acmovedata / 4 == p_z ) || acmovedata == 0 || acmovedata == 60 ) {
            for( auto a : acMap->m_anbindungen ) {
                if( a.m_direction == 'S'  && p_x >= a.m_move + 10 && p_x < a.m_move + a.m_mapsy + 10 ) {
                    showNewMap( a.m_mapidx );
                    free( acMap );
                    strcpy( SAV.m_acMapName, a.m_name );
                    SAV.m_acMapIdx = a.m_mapidx;
                    acMap = new map2d::Map( "nitro://MAPS/", a.m_name );
                    p_x -= a.m_move;
                    p_y = 9;
                    SAV.m_acposx = 20 * ( p_x - 10 );
                    SAV.m_acposy = 20 * ( p_y - 10 );
                    animateHero( movedir, 1 );
                    acMap->draw( p_x - 16, p_y - 17, true );
                    animateHero( movedir, 2 );
                    return true;
                }
            }
        }
        return false;
    }
    if( p_init )
        acMap->draw( p_x - 16, p_y - 18, p_init );
    else
        movePlayer( movedir );

    animateHero( movedir, 1 );
    animateHero( movedir, 2 );
    if( p_init )
        animateHero( lastdir, 2 );

    updateOAM( OamTop );

    return true;
}

void animateMap( u8 p_frame ) {
    u8* tileMemory = (u8*)BG_TILE_RAM( 1 );
    for( size_t i = 0; i < acMap->m_animations.size( ); ++i ) {
        map2d::Animation& a = acMap->m_animations[ i ];
        if( ( p_frame ) % ( a.m_speed ) == 0 || a.m_speed == 1 ) {
            a.m_acFrame = ( a.m_acFrame + 1 ) % a.m_maxFrame;
            swiCopy( &a.m_animationTiles[ a.m_acFrame ], tileMemory + a.m_tileIdx * 32, 16 );
        }
    }
}

void initMapSprites( ) {
    initOAMTable( OamTop );
    SpriteInfo * SQCHAInfo = &spriteInfoTop[ 0 ];
    SpriteEntry * SQCHA = &OamTop->oamBuffer[ 0 ];
    SQCHAInfo->oamId = 0;
    SQCHAInfo->width = 16;
    SQCHAInfo->height = 32;
    SQCHAInfo->angle = 0;
    SQCHAInfo->entry = SQCHA;
    SQCHA->y = 72;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_TALL;
    SQCHA->isHidden = false;
    SQCHA->x = 120;
    SQCHA->size = OBJSIZE_32;
    SQCHA->gfxIndex = 0;
    SQCHA->priority = OBJPRIORITY_2;
    SQCHA->palette = 0;

    loadframe( SQCHAInfo, SAV.m_overWorldIdx, 0 );

    SQCHAInfo = &spriteInfoTop[ 1 ];
    SQCHA = &OamTop->oamBuffer[ 1 ];
    SQCHAInfo->oamId = 1;
    SQCHAInfo->width = 32;
    SQCHAInfo->height = 32;
    SQCHAInfo->angle = 0;
    SQCHAInfo->entry = SQCHA;
    SQCHA->y = 72;
    SQCHA->isRotateScale = false;
    SQCHA->blendMode = OBJMODE_NORMAL;
    SQCHA->isMosaic = true;
    SQCHA->colorMode = OBJCOLOR_16;
    SQCHA->shape = OBJSHAPE_SQUARE;
    SQCHA->isHidden = true;
    SQCHA->x = 112;
    SQCHA->size = OBJSIZE_32;
    SQCHA->gfxIndex = 16;
    SQCHA->priority = OBJPRIORITY_2;
    SQCHA->palette = 0;

    SpriteInfo * B2Info = &spriteInfoTop[ 2 ];
    SpriteEntry * B2 = &OamTop->oamBuffer[ 2 ];
    B2Info->oamId = 2;
    B2Info->width = 64;
    B2Info->height = 64;
    B2Info->angle = 0;
    B2Info->entry = B2;
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 64;
    B2->y = 32;

    B2 = &OamTop->oamBuffer[ 3 ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 128;
    B2->y = 32;
    B2->hFlip = true;

    B2 = &OamTop->oamBuffer[ 4 ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 64;
    B2->y = 96;
    B2->hFlip = false;
    B2->vFlip = true;

    B2 = &OamTop->oamBuffer[ 5 ];
    B2->isRotateScale = false;
    B2->blendMode = OBJMODE_NORMAL;
    B2->isMosaic = false;
    B2->colorMode = OBJCOLOR_16;
    B2->shape = OBJSHAPE_SQUARE;
    B2->isHidden = true;
    B2->size = OBJSIZE_64;
    B2->gfxIndex = 32;
    B2->priority = OBJPRIORITY_1;
    B2->palette = 1;
    B2->x = 128;
    B2->y = 96;
    B2->hFlip = true;
    B2->vFlip = true;

    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BigCirc1Pal, &SPRITE_PALETTE[ 16 ], 32 );
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BigCirc1Tiles, &SPRITE_GFX[ 32 * 32 / sizeof( SPRITE_GFX[ 0 ] ) ], BigCirc1TilesLen );
}

int stepcnt = 0;
void stepincrease( ) {
    stepcnt = ( stepcnt + 1 ) % 256;
    if( stepcnt == 0 ) {
        for( size_t s = 0; s < SAV.m_PkmnTeam.size( ); ++s ) {
            POKEMON::pokemon& ac = SAV.m_PkmnTeam[ s ];

            if( ac.m_boxdata.m_IV.m_isEgg ) {
                ac.m_boxdata.m_steps--;
                if( ac.m_boxdata.m_steps == 0 ) {
                    ac.m_boxdata.m_IV.m_isEgg = false;
                    ac.m_boxdata.m_hatchPlace = SAV.m_acMapIdx;
                    ac.m_boxdata.m_hatchDate[ 0 ] = acday;
                    ac.m_boxdata.m_hatchDate[ 1 ] = acmonth + 1;
                    ac.m_boxdata.m_hatchDate[ 2 ] = ( acyear + 1900 ) % 100;
                    char buffer[ 50 ];
                    sprintf( buffer, "%ls schüpfte\naus dem Ei!", ac.m_boxdata.m_Name );
                    messageBox M( buffer );
                }
            } else
                ac.m_boxdata.m_steps = std::min( 255, ac.m_boxdata.m_steps + 1 );
        }
    }
}

void cut::use( ) { }
void rockSmash::use( ) { }
void fly::use( ) { }
void flash::use( ) { }
void whirlpool::use( ) { }
void surf::use( ) {
    //heroIsBig = true;
    SAV.m_acMoveMode = SURF;
    movePlayerOnMap( SAV.m_acposx / 20 + dir[ lastdir ][ 1 ], SAV.m_acposy / 20 + dir[ lastdir ][ 0 ], SAV.m_acposz, false );
    SAV.m_acposx += 20 * dir[ lastdir ][ 1 ];
    SAV.m_acposy += 20 * dir[ lastdir ][ 0 ];
}


void shoUseAttack( int p_pkmIdx, bool p_female, bool p_shiny ) {
    OamTop->oamBuffer[ 0 ].isHidden = true;
    OamTop->oamBuffer[ 1 ].isHidden = false;
    for( int i = 0; i < 5; ++i ) {
        loadframe( &spriteInfoTop[ 1 ], SAV.m_overWorldIdx + 4, i, true );
        updateOAM( OamTop );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
    }
    for( int i = 0; i < 4; ++i )
        OamTop->oamBuffer[ 2 + i ].isHidden = false;
    int a = 5, b = 2, c = 96;
    if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, a, b, c, false, p_shiny, p_female ) ) {
        loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", p_pkmIdx, 80, 48, a, b, c, false, p_shiny, !p_female );
    }
    updateOAM( OamTop );

    for( int i = 0; i < 40; ++i )
        swiWaitForVBlank( );

    //animateHero(lastdir,2);
    OamTop->oamBuffer[ 0 ].isHidden = false;
    OamTop->oamBuffer[ 1 ].isHidden = true;
    for( int i = 0; i < 8; ++i )
        OamTop->oamBuffer[ 2 + i ].isHidden = true;
    updateOAM( OamTop );
}


int main( int p_argc, char** p_argv ) {
    //Init
    powerOn( POWER_ALL_2D );

    fatInitDefault( );
    nitroFSInit( p_argv[ 0 ] );

    //PRE-Intro
    touchPosition touch;

    sysSetBusOwners( true, true );
    memcpy( acSlot2Game, (char*)0x080000AC, 4 );

    startScreen( );

    AS_SetMP3Volume( 127 );

    AS_MP3Stop( );

    heroIsBig = SAV.m_acMoveMode != WALK;

    loadPictureSub( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "Clear" );
    loadPictureSub( bgGetGfxPtr( bg2sub ), "nitro:/PICS/", "Clear" );
    scrn.draw( mode );

    loadPicture( bgGetGfxPtr( bg3 ), "nitro:/PICS/", "Clear" );
    acMap = new map2d::Map( "nitro://MAPS/", SAV.m_acMapName );

    movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );
    lastdir = 0;

    cust_font.setColor( RGB( 0, 31, 31 ), 0 );

    int HILFSCOUNTER = 252;
    Oam->oamBuffer[ PKMN_ID ].isHidden = !( SAV.m_hasPKMN && SAV.m_PkmnTeam.size( ) );
    updateOAMSub( Oam );

    SAV.m_hasGDex = true;
    SAV.m_evolveInBattle = true;

    initMapSprites( );
    updateOAM( OamTop );


    char buffer[ 120 ] = { 0 };
    sprintf( buffer, "%d.mp3", SAV.m_acMapIdx );
    PLAYMp( buffer );
    AS_SetMP3Loop( true );
    swiWaitForIRQ( );
    swiWaitForVBlank( );

    while( 42 ) {
        updateTime( true );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForVBlank( );
        swiWaitForIRQ( );
        updateOAMSub( Oam );
        scanKeys( );
        touchRead( &touch );
        int pressed = keysUp( ), held = keysHeld( );

        //jump to MainSCrn immediatly
        if( pressed & KEY_X ) {
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 4, 0, 20, 3 );
            consoleClear( );
            showmappointer = false;
            Oam->oamBuffer[ SQCH_ID ].isHidden = true;
            Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = true;
            setMainSpriteVisibility( false );
            Oam->oamBuffer[ SAVE_ID ].isHidden = false;
            Oam->oamBuffer[ PKMN_ID ].isHidden = !( SAV.m_hasPKMN && SAV.m_PkmnTeam.size( ) );
            updateOAMSub( Oam );
            mode = -1;
            scrn.draw( mode );
        }

        if( held & KEY_L && gMod == DEVELOPER ) {
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 4, 4, 20, 10 );
            consoleClear( );
            printf( "%3i %3i\nx: %3i y: %3i z: %3i\n", acMap->m_sizex, acMap->m_sizey, SAV.m_acposx / 20, ( SAV.m_acposy ) / 20, SAV.m_acposz );
            printf( "%s %i\n", SAV.m_acMapName, SAV.m_acMapIdx );
            printf( "topbehave %i;\n bottombehave %i", acMap->m_blockSets.m_blocks[ acMap->m_blocks[ SAV.m_acposy / 20 + 10 ][ SAV.m_acposx / 20 + 10 ].m_blockidx ].m_topbehave,
                    acMap->m_blockSets.m_blocks[ acMap->m_blocks[ SAV.m_acposy / 20 + 10 ][ SAV.m_acposx / 20 + 10 ].m_blockidx ].m_bottombehave );
        }
        if( pressed & KEY_A ) {
            for( auto a : SAV.m_PkmnTeam )
                if( !a.m_boxdata.m_IV.m_isEgg )
                    for( int i = 0; i < 4; ++i )
                        if( AttackList[ a.m_boxdata.m_Attack[ i ] ]->m_isFieldAttack && AttackList[ a.m_boxdata.m_Attack[ i ] ]->possible( ) ) {
                consoleSelect( &Bottom );
                consoleSetWindow( &Bottom, 4, 0, 20, 3 );
                consoleClear( );
                showmappointer = false;
                Oam->oamBuffer[ SQCH_ID ].isHidden = true;
                Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = true;
                updateOAMSub( Oam );
                scrn.draw( mode = -1 );
                char buffer[ 50 ];
                sprintf( buffer, "%s\nMöchtest du %s nutzen?", AttackList[ a.m_boxdata.m_Attack[ i ] ]->text( ), AttackList[ a.m_boxdata.m_Attack[ i ] ]->m_moveName.c_str( ) );
                yesNoBox yn;
                if( yn.getResult( buffer ) ) {
                    sprintf( buffer, "%ls setzt %s\nein!", a.m_boxdata.m_Name, AttackList[ a.m_boxdata.m_Attack[ i ] ]->m_moveName.c_str( ) );
                    messageBox( buffer, true, true );
                    shoUseAttack( a.m_boxdata.m_SPEC, a.m_boxdata.m_isFemale, a.m_boxdata.isShiny( ) );
                    AttackList[ a.m_boxdata.m_Attack[ i ] ]->use( );
                }
                goto OUT;
                        }
OUT:
            scrn.draw( mode );
        }
        //Moving
        if( pressed & KEY_DOWN ) {
            animateHero( 2, 2, true );
            lastdir = 2;
            continue;
        }
        if( pressed & KEY_RIGHT ) {
            animateHero( 1, 2, true );
            lastdir = 1;
            continue;
        }
        if( pressed & KEY_UP ) {
            animateHero( 4, 2, true );
            lastdir = 4;
            continue;
        }
        if( pressed & KEY_LEFT ) {
            animateHero( 3, 2, true );
            lastdir = 3;
            continue;
        }

        if( held & KEY_DOWN ) {
            scanKeys( );
            if( movePlayerOnMap( SAV.m_acposx / 20, ( SAV.m_acposy + MOV ) / 20, SAV.m_acposz, false ) ) {
                SAV.m_acposy += MOV;
                stepincrease( );
                lastdir = 2;
            }
            if( SAV.m_acMoveMode != BIKE )
                continue;
        }
        if( held & KEY_LEFT ) {
            scanKeys( );
            if( movePlayerOnMap( ( SAV.m_acposx - MOV ) / 20, SAV.m_acposy / 20, SAV.m_acposz, false ) ) {
                SAV.m_acposx -= MOV;
                stepincrease( );
                lastdir = 3;
            }
            if( SAV.m_acMoveMode != BIKE )
                continue;
        }
        if( held & KEY_RIGHT ) {
            scanKeys( );
            if( movePlayerOnMap( ( SAV.m_acposx + MOV ) / 20, SAV.m_acposy / 20, SAV.m_acposz, false ) ) {
                SAV.m_acposx += MOV;
                stepincrease( );
                lastdir = 1;
            }
            if( SAV.m_acMoveMode != BIKE )
                continue;
        }
        if( held & KEY_UP ) {
            scanKeys( );
            if( movePlayerOnMap( SAV.m_acposx / 20, ( SAV.m_acposy - MOV ) / 20, SAV.m_acposz, false ) ) {
                SAV.m_acposy -= MOV;
                stepincrease( );
                lastdir = 4;
            }
            if( SAV.m_acMoveMode != BIKE )
                continue;
        }
        //StartBag
        //Centers o t circles.
        //pokemon -> ID -> DEX -> Bag -> Opt -> Nav
        // X|Y
        //int mainSpritesPositions[6][2] 
        //= {{130,60},{160,80},{160,115},{130,135},{100,115},{100,80}};
        if( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 6 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 7 ] - touch.py ) ) <= 16 && mode == -1 ) {

            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            SAV.m_bag.draw( );
            initMapSprites( );
            movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );
        }
        //StartPkmn
        else if( SAV.m_PkmnTeam.size( ) && ( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 0 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 1 ] - touch.py ) ) <= 16 ) && mode == -1 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }

            scrn.run_pkmn( );

            scrn.draw( mode );
            initMapSprites( );
            movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );
        }
        //StartDex
        else if( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 4 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 5 ] - touch.py ) ) <= 16 && mode == -1 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }

            scrn.run_dex( );
            scrn.draw( mode );
            initMapSprites( );
            movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );
        }
        //StartOptions
        else if( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 8 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 9 ] - touch.py ) ) <= 16 && mode == -1 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
        }
        //StartID
        else if( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 2 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 3 ] - touch.py ) ) <= 16 && mode == -1 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            const char *someText[ 7 ] = { "\n     pokemon-Spawn", "\n    Item-Spawn", "\n 1-Item_Test", "\n  Battle SPWN.", "\n   Battle SPWN 2", "\n    42" };
            choiceBox test( 5, &someText[ 0 ], 0, true );
            int res = test.getResult( "Tokens of god-being...", true );
            switch( res ) {
                case 0:
                {
                    SAV.m_PkmnTeam.clear( );
                    for( int i = 0; i < 5; ++i ) {
                        POKEMON::pokemon a( 0, HILFSCOUNTER, 0,
                                            20, SAV.m_Id, SAV.m_Sid, L"TEST"/*SAV.getName().c_str()*/, !SAV.m_isMale, false, rand( ) % 2, rand( ) % 2, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        stored_pkmn[ *free_spaces.rbegin( ) ] = a.m_boxdata;
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        if( POKEMON::PKMNDATA::canLearn( HILFSCOUNTER, 57, 4 ) )
                            a.m_boxdata.m_Attack[ 2 ] = 57;
                        if( POKEMON::PKMNDATA::canLearn( HILFSCOUNTER, 19, 4 ) )
                            a.m_boxdata.m_Attack[ 1 ] = 19;
                        a.m_boxdata.m_exp += 750;
                        SAV.m_PkmnTeam.push_back( a );

                        SAV.m_inDex[ a.m_boxdata.m_SPEC - 1 ] = true;
                        box_of_st_pkmn[ a.m_boxdata.m_SPEC - 1 ].push_back( *free_spaces.rbegin( ) );
                        //printf("%i",(*free_spaces.rbegin()));
                        free_spaces.pop_back( );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }
                    for( int i = 0; i < 649; ++i )
                        SAV.m_inDex[ i ] = true;
                    SAV.m_hasPKMN = true;
                    swiWaitForVBlank( );
                    setMainSpriteVisibility( false );
                    break;
                }
                case 1:
                    for( int j = 1; j < 700; ++j )
                        if( ItemList[ j ].m_itemName != "Null" )
                            SAV.m_bag.addItem( ItemList[ j ].m_itemType, j, 1 );
                    break;
                case 2:
                    messageBox( berry( "Ginemabeere" ), 31 );
                    setMainSpriteVisibility( false );
                    break;
                case 3:{
                    BATTLE::battleTrainer me( "TEST", 0, 0, 0, 0, &SAV.m_PkmnTeam, 0 );
                    std::vector<POKEMON::pokemon> cpy;

                    for( int i = 0; i < 3; ++i ) {
                        POKEMON::pokemon a( 0, HILFSCOUNTER, 0,
                                            30, SAV.m_Id, SAV.m_Sid, L"TEST"/*SAV.getName()*/, i % 2, true, rand( ) % 2, true, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }

                    BATTLE::battleTrainer opp( "TEST-OPP", "DeR TeST iST DeR BeSTe MSG1", "DeR TeST VeRLieRT GeRaDe... MSG2", "DeR TeST GEWiNNT HaHa! MSG3", "DeR TeST VeRLieRT... MSG4", &( cpy ), 0 );

                    BATTLE::battle test_battle( &me, &opp, 100, 5, BATTLE::battle::DOUBLE );
                    test_battle.start( 100, BATTLE::battle::NONE );
                    initMapSprites( );
                    movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );
                    break;
                }
                case 4:{
                    BATTLE::battleTrainer me( "TEST", 0, 0, 0, 0, &SAV.m_PkmnTeam, 0 );
                    std::vector<POKEMON::pokemon> cpy;

                    for( int i = 0; i < 6; ++i ) {
                        POKEMON::pokemon a( 0, HILFSCOUNTER, 0,
                                            15, SAV.m_Id, SAV.m_Sid, L"TEST"/*SAV.getName()*/, i % 2, true, rand( ) % 2, true, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }

                    BATTLE::battleTrainer opp( "TEST-OPP", "DeR TeST iST DeR BeSTe MSG1", "DeR TeST VeRLieRT GeRaDe... MSG2", "DeR TeST GEWiNNT HaHa! MSG3", "DeR TeST VeRLieRT... MSG4", &( cpy ), 0 );

                    BATTLE::battle test_battle( &me, &opp, 100, 5, BATTLE::battle::SINGLE );
                    test_battle.start( 100, BATTLE::battle::NONE );
                    initMapSprites( );
                    movePlayerOnMap( SAV.m_acposx / 20, SAV.m_acposy / 20, SAV.m_acposz, true );
                    break;
                }
            }
            setMainSpriteVisibility( false );
            scrn.draw( mode );

        }
        //StartPok\x82""nav
        else if( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 10 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 11 ] - touch.py ) ) <= 16 && mode == -1 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            mode = 0;
            scrn.draw( mode );
            //movePlayerOnMap(SAV.m_acposx/20,SAV.m_acposy/20,SAV.m_acposz,false);
        }
        //StartMaps
        else if( sqrt( sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 0 ] - touch.px ) + sq( BGs[ BG_ind ].m_mainMenuSpritePoses[ 1 ] - touch.py ) ) <= 16 && mode == 0 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            if( acMapRegion == NONE )
                acMapRegion = acRegion;
            mode = acMapRegion;
            showmappointer = true;
            scrn.draw( mode );
        }
        //Nav->StartScrn
        else if( touch.px > 224 && touch.py > 164 && mode == 0 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            mode = -1;
            scrn.draw( mode );
        }
        //Map->NavScrn
        else if( touch.px > 224 && touch.py > 164 && mode > 0 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 4, 0, 20, 3 );
            consoleClear( );
            showmappointer = false;
            Oam->oamBuffer[ SQCH_ID ].isHidden = true;
            Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = true;
            updateOAMSub( Oam );
            mode = 0;
            scrn.draw( mode );
        }
        //SwitchMap
        else if( ( pressed & KEY_SELECT ) && mode > 0 ) {
            mode = ( ( mode + 1 ) % 3 ) + 1;
            consoleSetWindow( &Bottom, 5, 0, 20, 1 );
            consoleSelect( &Bottom );
            consoleClear( );
            scrn.draw( mode );
        }
        //MapCourser
        else if( touch.px > 39 && touch.px < SCREEN_WIDTH - 39 && touch.py > 31 && touch.py < SCREEN_HEIGHT - 31 && mode > 0 ) {
            Oam->oamBuffer[ SQCH_ID ].x = Oam->oamBuffer[ SQCH_ID + 1 ].x = touch.px - 8;
            Oam->oamBuffer[ SQCH_ID ].y = Oam->oamBuffer[ SQCH_ID + 1 ].y = touch.py - 8;
            printMapLocation( touch );
            updateOAMSub( Oam );
            updateTime( true );
        } else if( touch.px != 0 && touch.py != 0 && sqrt( sq( touch.px - 8 ) + sq( touch.py - 12 ) ) <= 17 ) {
            while( 1 ) {
                swiWaitForVBlank( );
                updateTime( true );
                scanKeys( );
                touch = touchReadXY( );
                if( touch.px == 0 && touch.py == 0 )
                    break;
            }
            bool sqa = Oam->oamBuffer[ SQCH_ID ].isHidden,
                sqb = Oam->oamBuffer[ SQCH_ID + 1 ].isHidden;
            Oam->oamBuffer[ SQCH_ID ].isHidden = true;
            Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = true;
            bool mappy = showmappointer;
            showmappointer = false;
            updateOAMSub( Oam );
            consoleSelect( &Bottom );
            consoleSetWindow( &Bottom, 0, 0, 32, 5 );
            consoleClear( );
            yesNoBox Save( "PokéNav " );
            if( Save.getResult( "Möchtest du deinen\nFortschritt sichern?\n" ) ) {
                if( gMod == EMULATOR )
                    messageBox Succ( "Speichern?\nIn einem Emulator?", "PokéNav" );
                else if( SAV.save( progress ) )
                    messageBox Succ( "Fortschritt\nerfolgreich gesichert!", "PokéNav" );
                else
                    messageBox Succ( "Es trat ein Fehler auf\nSpiel nicht gesichert.", "PokéNav" );
            }
            Oam->oamBuffer[ SQCH_ID ].isHidden = sqa;
            Oam->oamBuffer[ SQCH_ID + 1 ].isHidden = sqb;
            showmappointer = mappy;
            updateOAMSub( Oam );
            scrn.draw( mode );
        }
        //End 

    }
    free( acMap );
    return 0;
}