/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : main.cpp
    author      : Philip Wellnitz
    description : Main ARM9 entry point

    Copyright (C) 2012 - 2015
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


#include <nds.h>
#include <fat.h>
#include <filesystem.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <memory>

#include "defines.h"
#include "saveGame.h"
#include "startScreen.h"

#include "hmMoves.h"
#include "item.h"
#include "berry.h"
#include "pokemon.h"

#include "uio.h"
#include "messageBox.h"
#include "yesNoBox.h"
#include "choiceBox.h"
#include "sprite.h"

#include "bagUI.h"
#include "bagViewer.h"
#include "statusScreen.h"
#include "statusScreenUI.h"
#include "dex.h"
#include "dexUI.h"
#include "mapDrawer.h"
#include "mapSlice.h"
#include "mapObject.h"

#include "battle.h"
#include "Gen.h"

#include "BigCirc1.h" 
#include "consoleFont.h"

#include "Back.h"
#include "Save.h"
#include "Option.h"
#include "PokemonSp.h"
#include "Id.h"
#include "SPBag.h"
#include "Nav.h"
#include "PokeDex.h"

#ifndef _EMULATOR
GameMod gMod = GameMod::DEVELOPER;
#else
GameMod gMod = GameMod::EMULATOR;
#endif

DEX::dexUI dui( true, 1, FS::SAV->m_hasGDex ? 649 : 493 );
DEX::dex dx( FS::SAV->m_hasGDex ? 649 : 493, &dui );
MAP::mapDrawer* curMap = 0;

u8 DayTimes[ 4 ][ 5 ] = {
    { 7, 10, 15, 17, 23 },
    { 6, 9, 12, 18, 23 },
    { 5, 8, 10, 20, 23 },
    { 7, 9, 13, 19, 23 }
};

int hours = 0, seconds = 0, minutes = 0, day = 0, month = 0, year = 0;
int achours = 0, acseconds = 0, acminutes = 0, acday = 0, acmonth = 0, acyear = 0;
int pressed, held, last;
u8 frame = 0;
bool DRAW_TIME = false;
bool UPDATE_TIME = true;
bool ANIMATE_MAP = false;
bool INIT_MAIN_SPRITES = false;

u8 getCurrentDaytime( ) {
    time_t unixTime = time( NULL );
    struct tm* timeStruct = gmtime( (const time_t *)&unixTime );

    u8 t = timeStruct->tm_hour, m = timeStruct->tm_mon;

    for( u8 i = 0; i < 5; ++i )
        if( DayTimes[ m / 4 ][ i ] >= t )
            return i;
    return 254;
}

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
    return false;
    // return ( FS::SAV->m_acMoveMode != MAP::MoveMode::SURF )
    //     && acMap->m_blocks[ FS::SAV->m_acposy / 20 + 10 + dir[ lastdir ][ 0 ] ][ FS::SAV->m_acposx / 20 + 10 + dir[ lastdir ][ 1 ] ].m_movedata == 4;
}

void cut::use( ) { }
void rockSmash::use( ) { }
void fly::use( ) { }
void flash::use( ) { }
void whirlpool::use( ) { }
void surf::use( ) {
    MAP::curMap->changeMoveMode( MAP::SURF );
    //movePlayerOnMap( FS::SAV->m_acposx / 20 + dir[ lastdir ][ 1 ], FS::SAV->m_acposy / 20 + dir[ lastdir ][ 0 ], FS::SAV->m_acposz, false );
}


u8 positions[ 6 ][ 2 ] = {
    { 14, 2 }, { 16, 3 }, { 14, 9 },
    { 16, 10 }, { 14, 17 }, { 16, 18 }
};

void initMainSprites( ) {
    IO::initOAMTable( true );
    u16 tileCnt = 0;

    tileCnt = IO::loadSprite( BACK_ID, BACK_ID, tileCnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );

    tileCnt = IO::loadSprite( SAVE_ID, SAVE_ID, tileCnt,
                              -20, -20, 64, 64, SavePal,
                              SaveTiles, SaveTilesLen, false, false, false, OBJPRIORITY_0, true );

    //Main menu sprites

    tileCnt = IO::loadSprite( OPTS_ID, OPTS_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( OPTS_ID - 2 ) + 1 ] - 16,
                              32, 32, OptionPal, OptionTiles, OptionTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( PKMN_ID, PKMN_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( PKMN_ID - 2 ) + 1 ] - 16,
                              32, 32, PokemonSpPal, PokemonSpTiles, PokemonSpTilesLen,
                              false, false, !FS::SAV->m_pkmnTeam[ 0 ].m_boxdata.m_speciesId, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( NAV_ID, NAV_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( NAV_ID - 2 ) + 1 ] - 16,
                              32, 32, NavPal, NavTiles, NavTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( ID_ID, ID_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( ID_ID - 2 ) + 1 ] - 16,
                              32, 32, IdPal, IdTiles, IdTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( DEX_ID, DEX_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( DEX_ID - 2 ) + 1 ] - 16,
                              32, 32, PokeDexPal, PokeDexTiles, PokeDexTilesLen,
                              false, false, false, OBJPRIORITY_0, true );
    tileCnt = IO::loadSprite( BAG_ID, BAG_ID, tileCnt,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) ] - 16,
                              IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 * ( BAG_ID - 2 ) + 1 ] - 16,
                              32, 32, SPBagPal, SPBagTiles, SPBagTilesLen,
                              false, false, false, OBJPRIORITY_0, true );

    IO::updateOAM( true );
}
void initGraphics( ) {
    IO::vramSetup( );
    videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );

    IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    IO::bg3sub = bgInitSub( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
    IO::bg2sub = bgInitSub( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
    for( u8 i = 0; i < 4; ++i )
        bgSetPriority( i, i );
    for( u8 i = 0; i < 4; ++i )
        bgSetPriority( 4 + i, i );
    bgUpdate( );

    IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );

    IO::consoleFont->gfx = (u16*)consoleFontTiles;
    IO::consoleFont->pal = (u16*)consoleFontPal;
    IO::consoleFont->numChars = 218;
    IO::consoleFont->numColors = 16;
    IO::consoleFont->bpp = 8;
    IO::consoleFont->asciiOffset = 32;
    IO::consoleFont->convertSingleColor = false;

    consoleSetFont( &IO::Top, IO::consoleFont );
    consoleSetFont( &IO::Bottom, IO::consoleFont );
}
void initTimeAndRnd( ) {
    time_t uTime = time( NULL );
    tm* tStruct = gmtime( (const time_t *)&uTime );

    hours = tStruct->tm_hour;
    month = tStruct->tm_min;
    seconds = tStruct->tm_sec;
    day = tStruct->tm_mday;
    month = tStruct->tm_mon + 1;
    year = tStruct->tm_year + 1900;

    srand( hours ^ ( 100 * minutes ) ^ ( 10000 * seconds ) ^ ( day ^ ( 100 * month ) ^ year ) );
    LastPID = rand( );
}

void defaultScrns( ) {
    consoleSelect( &IO::Top );
    consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
    consoleClear( );
    consoleSelect( &IO::Bottom );
    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
    consoleClear( );
}

int main( int, char** p_argv ) {
    //Init
    powerOn( POWER_ALL_2D );

    fatInitDefault( );
    nitroFSInit( p_argv );


    irqEnable( IRQ_VBLANK );
    initGraphics( );
    initTimeAndRnd( );

    //Read the savegame
    if( gMod != EMULATOR ) {
        FS::SAV = FS::readSave( );
    } else {
        FS::SAV->m_savTyp = 0;
    }
    startScreen( ).run( );

    int HILFSCOUNTER = 252;

    FS::SAV->m_hasGDex = true;
    FS::SAV->m_evolveInBattle = true;


    irqSet( IRQ_VBLANK, [ ]( ) {
        scanKeys( );

        if( ANIMATE_MAP ) {
            //animateMap( ++frame );
        }
        if( INIT_MAIN_SPRITES ) {
            INIT_MAIN_SPRITES = false;
            initMainSprites( );
        }
        if( !UPDATE_TIME )
            return;

        IO::boldFont->setColor( 0, 0 );
        u8 oldC1 = IO::boldFont->getColor( 1 );
        u8 oldC2 = IO::boldFont->getColor( 2 );
        IO::boldFont->setColor( 0, 1 );
        IO::boldFont->setColor( BLACK_IDX, 2 );
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        time_t unixTime = time( NULL );
        struct tm* timeStruct = gmtime( (const time_t *)&unixTime );

        if( acseconds != timeStruct->tm_sec || DRAW_TIME ) {
            DRAW_TIME = false;
            BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
            IO::boldFont->setColor( WHITE_IDX, 1 );
            IO::boldFont->setColor( WHITE_IDX, 2 );

            char buffer[ 50 ];
            sprintf( buffer, "%02i:%02i:%02i", achours, acminutes, acseconds );
            IO::boldFont->printString( buffer, 18 * 8, 192 - 16, true );

            achours = timeStruct->tm_hour;
            acminutes = timeStruct->tm_min;
            acseconds = timeStruct->tm_sec;

            IO::boldFont->setColor( 0, 1 );
            IO::boldFont->setColor( BLACK_IDX, 2 );
            sprintf( buffer, "%02i:%02i:%02i", achours, acminutes, acseconds );
            IO::boldFont->printString( buffer, 18 * 8, 192 - 16, true );
        }
        achours = timeStruct->tm_hour;
        acminutes = timeStruct->tm_min;
        acday = timeStruct->tm_mday;
        acmonth = timeStruct->tm_mon;
        acyear = timeStruct->tm_year + 1900;

        IO::boldFont->setColor( oldC1, 1 );
        IO::boldFont->setColor( oldC2, 2 );
    } );
    IO::drawSub( true );

    curMap = new MAP::mapDrawer( FS::SAV->m_currentMap, FS::SAV->m_player );
    curMap->draw( );
    ANIMATE_MAP = true;

    touchPosition touch;
    bool stopped = true;
    u8 bmp = false;
    loop( ) {
        swiWaitForVBlank( );
        touchRead( &touch );
        pressed = keysUp( );
        last = held;
        held = keysHeld( );

#ifdef DEBUG
        if( held & KEY_L && gMod == DEVELOPER ) {
            std::sprintf( buffer, "Currently at (%hu,%hu,%hu).",
                          FS::SAV->m_player.m_pos.m_posX,
                          FS::SAV->m_player.m_pos.m_posY,
                          FS::SAV->m_player.m_pos.m_posZ );
            IO::messageBox m( buffer );

            IO::drawSub( true );
        }
#endif

        if( pressed & KEY_A ) {
            for( u8 i = 0; i < 6; ++i ) {
                if( !FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                    break;
                auto a = FS::SAV->m_pkmnTeam[ i ];
                if( !a.m_boxdata.m_individualValues.m_isEgg ) {
                    for( u8 i = 0; i < 4; ++i ) {
                        if( AttackList[ a.m_boxdata.m_moves[ i ] ]->m_isFieldAttack
                            && AttackList[ a.m_boxdata.m_moves[ i ] ]->possible( ) ) {

                            char buffer[ 50 ];
                            sprintf( buffer, "%s\nMöchtest du %s nutzen?", AttackList[ a.m_boxdata.m_moves[ i ] ]->text( ), AttackList[ a.m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ) );
                            IO::yesNoBox yn;
                            if( yn.getResult( buffer ) ) {
                                IO::drawSub( );
                                sprintf( buffer, "%ls setzt %s\nein!", a.m_boxdata.m_name, AttackList[ a.m_boxdata.m_moves[ i ] ]->m_moveName.c_str( ) );
                                IO::messageBox( buffer, true );
                                //shoUseAttack( a.m_boxdata.m_speciesId, a.m_boxdata.m_isFemale, a.m_boxdata.isShiny( ) );
                                AttackList[ a.m_boxdata.m_moves[ i ] ]->use( );
                            }
                            IO::drawSub( true );
                            goto OUT;
                        }
                    }
                }
            }
OUT:
            ;
        }
        //Movement
        if( held & KEY_Y ) {
            IO::waitForKeysUp( KEY_Y );
            if( FS::SAV->m_player.m_movement == MAP::WALK )
                curMap->changeMoveMode( MAP::BIKE );
            else if( FS::SAV->m_player.m_movement == MAP::BIKE )
                curMap->changeMoveMode( MAP::WALK );
            else {
                IO::messageBox( "Das kann jetzt\nnicht eingesetzt werden.", "PokéNav" );
                IO::drawSub( true );
            }
            swiWaitForVBlank( );
            scanKeys( );
            continue;
        }
        if( held & ( KEY_DOWN | KEY_UP | KEY_LEFT | KEY_RIGHT ) ) {
            MAP::direction curDir = GET_DIR( held );
            scanKeys( );

            stopped = false;
            if( curMap->canMove( FS::SAV->m_player.m_pos, curDir, FS::SAV->m_player.m_movement ) || ( ( held & KEY_R ) && gMod == DEVELOPER ) ) {
                curMap->movePlayer( curDir, ( held & KEY_B ) );
            } else if( !bmp ) {
                //Play "Bump" sound
                curMap->stopPlayer( curDir );
                swiWaitForVBlank( );
                bmp = true;
            } else if( bmp < 2 ) {
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                swiWaitForVBlank( );
                curMap->stopPlayer( );
                bmp = 2;
            }
            continue;
        }
        if( !stopped ) {
            curMap->stopPlayer( );
            stopped = true;
            bmp = false;
        }
        //StartBag
        if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 6 ],
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 7 ], 16 ) ) {
            BAG::bagUI bui;
            BAG::bagViewer bv( FS::SAV->m_bag, &bui );
            ANIMATE_MAP = false;
            UPDATE_TIME = false;
            INIT_MAIN_SPRITES = false;

            bv.run( FS::SAV->m_lstBag, FS::SAV->m_lstBagItem );

            defaultScrns( );
            IO::drawSub( true );
            UPDATE_TIME = true;
            curMap->draw( );
            ANIMATE_MAP = true;
        } else if( FS::SAV->m_pkmnTeam[ 0 ].m_boxdata.m_speciesId     //StartPkmn
                   && ( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 0 ],
                   IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 1 ], 16 ) ) ) {

            std::vector<pokemon> tmp;
            for( u8 i = 0; i < 6; ++i )
                if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                    tmp.push_back( FS::SAV->m_pkmnTeam[ i ] );
                else
                    break;
            STS::regStsScreenUI rsUI( &tmp );
            STS::regStsScreen sts( 0, &rsUI );
            ANIMATE_MAP = false;

            sts.run( );

            for( u8 i = 0; i < tmp.size( ); ++i )
                FS::SAV->m_pkmnTeam[ i ] = tmp[ i ];

            defaultScrns( );
            IO::drawSub( true );
            curMap->draw( );
            ANIMATE_MAP = true;
        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 4 ],        //StartDex
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 5 ], 16 ) ) {
            ANIMATE_MAP = false;
            dx.run( dui.currPkmn( ) );

            defaultScrns( );
            initMainSprites( );
            curMap->draw( );
            ANIMATE_MAP = true;
        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 8 ],        //StartOptions
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 9 ], 16 ) ) {


        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 2 ],        //StartID
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 3 ], 16 ) ) {

            const char *someText[ 7 ] = { "PKMN-Spawn", "Item-Spawn", "1-Item-Test", "Dbl Battle", "Sgl Battle", "Chg NavScrn", " ... " };
            IO::choiceBox test( 6, &someText[ 0 ], 0, false );
            int res = test.getResult( "Tokens of god-being..." );
            IO::drawSub( );
            switch( res ) {
                case 0:
                {
                    memset( FS::SAV->m_pkmnTeam, 0, sizeof( FS::SAV->m_pkmnTeam ) );
                    for( int i = 0; i < 3; ++i ) {
                        pokemon& a = FS::SAV->m_pkmnTeam[ i ];
                        a = pokemon( 0, 133, 0,
                                     50, FS::SAV->m_id + i, FS::SAV->m_sid, FS::SAV->m_playername,
                                     !FS::SAV->m_isMale, false, false, rand( ) % 2, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        a.m_stats.m_acHP *= i / 5.0;
                        a.m_boxdata.m_moves[ 3 ] = 0;
                        if( canLearn( 133, 19, 4 ) )
                            a.m_boxdata.m_moves[ 1 ] = 19;
                        a.m_boxdata.m_experienceGained += 750;

                        //Hand out some ribbons
                        for( u8 i = 0; i < 4; ++i ) {
                            a.m_boxdata.m_ribbons0[ i ] = rand( ) % 255;
                            a.m_boxdata.m_ribbons1[ i ] = rand( ) % 255;
                            a.m_boxdata.m_ribbons2[ i ] = rand( ) % 255;
                        }
                        a.m_boxdata.m_ribbons1[ 2 ] = rand( ) % 63;
                        a.m_boxdata.m_ribbons1[ 3 ] = 0;
                        a.m_boxdata.m_holdItem = I_CELL_BATTERY + i;

                        FS::SAV->m_inDex[ ( a.m_boxdata.m_speciesId - 1 ) / 8 ] |= ( 1 << ( ( a.m_boxdata.m_speciesId - 1 ) % 8 ) );

                        HILFSCOUNTER = 3 + ( ( HILFSCOUNTER ) % 649 );
                    }
                    for( u16 i = 0; i < 649 / 8; ++i )
                        FS::SAV->m_inDex[ i ] = 255;

                    swiWaitForVBlank( );
                    break;
                }
                case 1:
                    if( !FS::SAV->m_bag )
                        FS::SAV->m_bag = new BAG::bag( );
                    for( u16 j = 1; j < 637; ++j )
                        if( ItemList[ j ]->m_itemName != "Null" )
                            FS::SAV->m_bag->insert( BAG::toBagType( ItemList[ j ]->m_itemType ), j, 1 );
                    break;
                case 2: {
                    item* curr = ItemList[ rand( ) % 638 ];
                    while( curr->m_itemName == "Null" )
                        curr = ItemList[ rand( ) % 638 ];
                    IO::messageBox( curr, 31 );
                    break;
                }
                case 3:{
                    std::vector<pokemon> tmp, cpy;
                    for( u8 i = 0; i < 6; ++i )
                        if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                            tmp.push_back( FS::SAV->m_pkmnTeam[ i ] );
                        else
                            break;
                    BATTLE::battleTrainer me( "TEST", "", "", "", "", tmp, FS::SAV->m_bag->getBattleItems( ) );

                    for( u8 i = 0; i < 3; ++i ) {
                        pokemon a( 0, HILFSCOUNTER, 0,
                                   30, FS::SAV->m_id + 1, FS::SAV->m_sid, L"Heiko"/*FS::SAV->getName()*/, i % 2, true, rand( ) % 2, true, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }
                    std::vector<item> itms;
                    BATTLE::battleTrainer opp( "Heiko", "Auf in den Kampf!", "Hm... Du bist gar nicht so schlecht...",
                                               "Yay gewonnen!", "Das war wohl eine Niederlage...", cpy, itms );

                    BATTLE::battle test_battle( me, opp, 100, 5, BATTLE::battle::DOUBLE );
                    ANIMATE_MAP = false;
                    test_battle.start( );
                    for( u8 i = 0; i < tmp.size( ); ++i )
                        FS::SAV->m_pkmnTeam[ i ] = tmp[ i ];
                    break;
                }
                case 4:{
                    std::vector<pokemon> tmp, cpy;
                    for( u8 i = 0; i < 6; ++i )
                        if( FS::SAV->m_pkmnTeam[ i ].m_boxdata.m_speciesId )
                            tmp.push_back( FS::SAV->m_pkmnTeam[ i ] );
                        else
                            break;
                    BATTLE::battleTrainer me( "TEST", "", "", "", "", tmp, FS::SAV->m_bag->getBattleItems( ) );

                    for( u8 i = 0; i < 6; ++i ) {
                        pokemon a( 0, HILFSCOUNTER, 0,
                                   15, FS::SAV->m_id + 1, FS::SAV->m_sid, L"Heiko"/*FS::SAV->getName()*/, i % 2, true, rand( ) % 2, true, rand( ) % 2, i == 3, HILFSCOUNTER, i + 1, i );
                        //a.stats.acHP = i*a.stats.maxHP/5;
                        cpy.push_back( a );
                        HILFSCOUNTER = 1 + ( ( HILFSCOUNTER ) % 649 );
                    }
                    std::vector<item> itms;
                    BATTLE::battleTrainer opp( "Heiko", "Auf in den Kampf!", "Hm... Du bist gar nicht so schlecht...",
                                               "Yay gewonnen!", "Das war wohl eine Niederlage...", cpy, itms );

                    BATTLE::battle test_battle( me, opp, 100, 5, BATTLE::battle::SINGLE );
                    ANIMATE_MAP = false;
                    test_battle.start( );
                    for( u8 i = 0; i < tmp.size( ); ++i )
                        FS::SAV->m_pkmnTeam[ i ] = tmp[ i ];
                    break;
                }
                case 5:{
                    const char *bgNames[ MAXBG ];
                    for( u8 o = 0; o < MAXBG; ++o )
                        bgNames[ o ] = IO::BGs[ o ].m_name.c_str( );

                    IO::choiceBox scrnChoice( MAXBG, bgNames, 0, true );
                    IO::drawSub( scrnChoice.getResult( "Welcher Hintergrund\nsoll dargestellt werden?" ) );
                }
            }

            IO::drawSub( true );
            swiWaitForVBlank( );
            if( res == 3 || res == 4 ) {
                curMap->draw( );
            }
        } else if( GET_AND_WAIT_C( IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 10 ],  //Start Pokénav
            IO::BGs[ FS::SAV->m_bgIdx ].m_mainMenuSpritePoses[ 11 ], 16 ) ) {

        } else if( touch.px != 0 && touch.py != 0 && GET_AND_WAIT_C( 8, 12, 17 ) ) {
            IO::yesNoBox Save( "PokéNav " );
            if( Save.getResult( "Möchtest du deinen\nFortschritt sichern?\n" ) ) {
                IO::drawSub( );
                if( gMod == EMULATOR )
                    IO::messageBox Succ( "Speichern?\nIn einem Emulator?!", "PokéNav" );
                else if( FS::writeSave( FS::SAV ) )
                    IO::messageBox Succ( "Fortschritt\nerfolgreich gesichert!", "PokéNav" );
                else
                    IO::messageBox Succ( "Es trat ein Fehler auf\nSpiel nicht gesichert.", "PokéNav" );
            }
            IO::drawSub( true );
        }
        //End 

        scanKeys( );
    }
    delete curMap;
    return 0;
}