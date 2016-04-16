/*
Pokémon Emerald 2 Version
------------------------------

file        : battleUI.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2016
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

#include <cwchar>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <tuple>
#include <initializer_list>

#include "battle.h"
#include "battleUI.h"
#include "battleTrainer.h"
#include "defines.h"
#include "pokemon.h"
#include "move.h"
#include "item.h"
#include "yesNoBox.h"
#include "choiceBox.h"
#include "saveGame.h"
#include "buffer.h"
#include "fs.h"
#include "sprite.h"
#include "uio.h"
#include "bagViewer.h"
#include "keyboard.h"

//Sprites
#include "Back.h"
#include "A.h"

#include "Battle1.h"

#include "BattleSub1.h"
#include "BattleSub2.h"
#include "BattleSub3.h"
#include "BattleSub4.h"
#include "BattleSub5.h"
#include "BattleSub6.h"

#include "BattleBall1.h" //Normal
#include "BattleBall2.h" //Statused
#include "BattleBall3.h" //Fainted
#include "BattleBall4.h" //NA

#include "memo.h"
#include "atks.h"
#include "Up.h"
#include "Down.h"

#include "PokeBall1.h"
#include "PokeBall2.h"
#include "PokeBall3.h"
#include "PokeBall4.h"
#include "PokeBall5.h"
#include "PokeBall6.h"
#include "PokeBall7.h"
#include "PokeBall8.h"
#include "PokeBall9.h"
#include "PokeBall10.h"
#include "PokeBall11.h"
#include "PokeBall12.h"
#include "PokeBall13.h"
#include "PokeBall14.h"
#include "PokeBall15.h"
#include "PokeBall16.h"
#include "PokeBall17.h"

#include "Shiny1.h"
#include "Shiny2.h"


namespace BATTLE {
    //////////////////////////////////////////////////////////////////////////
    // BEGIN BATTLE_UI
    //////////////////////////////////////////////////////////////////////////

    void initColors( ) {

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = STEEL_COLOR;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
        BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
    }
    void battleUI::initLogScreen( ) {
        initColors( );
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = STEEL_COLOR;
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ RED_IDX ] = RED;
        BG_PALETTE[ BLUE_IDX ] = BLUE;
        IO::printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true, false, WHITE_IDX );
    }
    void battleUI::clearLogScreen( ) {
        IO::printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 63, true, false, WHITE_IDX );
    }
    void battleUI::setLogTextColor( u16 p_color ) {
        BG_PALETTE_SUB[ COLOR_IDX ] = p_color;
    }
    void battleUI::writeLogText( const std::string& p_message ) {
        IO::regularFont->printMBString( p_message.c_str( ), 8, 8, true );
    }

#define PB_PAL( i ) ( ( ( i ) == 0 ) ? BattleBall1Pal : ( ( ( i ) == 1 ) ? BattleBall2Pal : ( ( ( i ) == 2 ) ? BattleBall3Pal : BattleBall4Pal ) ) )
#define PB_TILES( i ) ( ( ( i ) == 0 ) ? BattleBall1Tiles : ( ( ( i ) == 1 ) ? BattleBall2Tiles : ( ( ( i ) == 2 ) ? BattleBall3Tiles : BattleBall4Tiles ) ) )
#define PB_TILES_LEN( i ) BattleBall1TilesLen

    // TOP SCREEN DEFINES

    //Some defines of indices in the OAM for the used sprites
#define HP_START              1
#define HP_IDX( p_opponent, p_pokemonPos) ( HP_START + ( ( p_opponent )+ ( p_pokemonPos ) * 2  ) )

#define STSBALL_START         5
#define STSBALL_IDX( p_pokemonPos, p_opponent ) ( STSBALL_START + ( ( p_opponent ) * 6 + ( p_pokemonPos ) ) )

#define PKMN_START           17
#define PKMN_IDX( p_pokemonPos, p_opponent ) ( PKMN_START + 4 * ( ( p_opponent ) * 2 + 1 - ( p_pokemonPos ) ) )

#define PLATFORM_START       34

#define PB_ANIM             127
#define SHINY_ANIM          127

    //Some analogous defines for their pal indices
#define PKMN_PAL_START        0
#define PKMN_PAL_IDX( p_pokemonPos, p_opponent ) ( PKMN_PAL_START + ( ( p_opponent ) * 2 + ( p_pokemonPos ) ) )

#define PB_PAL_TOP(i)         ( (i) + 4 )
#define HP_PAL                8
#define PLAT_PAL              9

#define PB_ANIM_TILES         0
#define PKMN_TILE_START      64
#define PKMN_TILE_IDX( p_pokemonPos, p_opponent ) ( PKMN_TILE_START + 144 * ( ( p_opponent ) * 2 + ( p_pokemonPos ) ) )
    u16 TILESTART = ( PKMN_TILE_START + 4 * 144 );

#define OWN1_EP_COL         200
#define OWN2_EP_COL         OWN1_EP_COL

#define OWN_HP_COL          210
#define OPP_HP_COL          215

#define HP_COL(a,b) (((a) == OPPONENT )? (OPP_HP_COL + (b)*2 ): (OWN_HP_COL + (b)*2 ))

#define GENDER(a) (a.m_boxdata.m_isFemale? 147 : (a.m_boxdata.m_isGenderless ? ' ' : 141))

    // BOTTOM SCREEN DEFINES
#define PB_PAL_SUB(i) (i)
#define SUB_FIGHT_START 0

#define SUB_PKMN_ICON_PAL 5

#define SUB_CHOICE_START 10

#define SUB_A_OAM 30
#define SUB_Back_OAM 31

    u16 SUB_TILESTART = 0;
    u8 SUB_PALSTART = 0;

    void drawGender( u8 p_x, u8 p_y, s8 p_gender, bool p_bottom = false ) {
        auto pal = BG_PAL( p_bottom );
        pal[ RED_IDX ] = RED;
        pal[ BLUE_IDX ] = BLUE;
        pal[ RED2_IDX ] = RED2;
        pal[ BLUE2_IDX ] = BLUE2;
        pal[ WHITE_IDX ] = WHITE;
        if( p_gender == 1 ) {
            IO::regularFont->setColor( BLUE_IDX, 1 );
            IO::regularFont->setColor( BLUE2_IDX, 2 );
            IO::regularFont->printChar( '{', p_x, p_y, p_bottom );
        } else if( p_gender == -1 ) {
            IO::regularFont->setColor( RED_IDX, 1 );
            IO::regularFont->setColor( RED2_IDX, 2 );
            IO::regularFont->printChar( '}', p_x, p_y, p_bottom );
        }
    }
    void undrawPkmnInfo1( u8 p_hpx, u8 p_hpy ) {
        IO::printRectangle( p_hpx - 88, p_hpy + 4, p_hpx, p_hpy + 30, false, false, 0 );
    }
    void undrawPkmnInfo2( u8 p_hpx, u8 p_hpy ) {
        IO::printRectangle( p_hpx + 32, p_hpy + 4, p_hpx + 120, p_hpy + 30, false, false, 0 );
    }
    void drawPkmnInfo1( u8 p_hpx, u8 p_hpy, pokemon p_pokemon, u8 p_hpCol ) {
        undrawPkmnInfo1( p_hpx, p_hpy );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->printString( p_pokemon.m_boxdata.m_name, p_hpx - 12, p_hpy + 2, false, IO::font::RIGHT );
        if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32 )
            drawGender( p_hpx - 10, p_hpy + 2, p_pokemon.gender( ) );
        IO::regularFont->setColor( WHITE_IDX, 2 );
        IO::regularFont->setColor( GRAY_IDX, 1 );
        char bf2[ 20 ] = { 0 };
        sprintf( buffer, "Lv%hhu", p_pokemon.m_level );
        sprintf( bf2, " %huKP", p_pokemon.m_stats.m_acHP );
        IO::regularFont->printString( buffer, p_hpx - IO::regularFont->stringWidth( bf2 ) - 2, p_hpy + 15, false, IO::font::RIGHT );
        IO::regularFont->setColor( WHITE_IDX, 1 );
        IO::regularFont->setColor( p_hpCol, 2 );
        IO::regularFont->printString( bf2, p_hpx - 2, p_hpy + 15, false, IO::font::RIGHT );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
    }
    void drawPkmnInfo2( u8 p_hpx, u8 p_hpy, pokemon p_pokemon, u8 p_hpCol ) {
        undrawPkmnInfo2( p_hpx, p_hpy );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::regularFont->printString( p_pokemon.m_boxdata.m_name, p_hpx + 34, p_hpy + 2, false );
        if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32 )
            drawGender( p_hpx + IO::regularFont->stringWidth( p_pokemon.m_boxdata.m_name ) + 34, p_hpy + 2, p_pokemon.gender( ) );
        IO::regularFont->setColor( WHITE_IDX, 2 );
        IO::regularFont->setColor( GRAY_IDX, 1 );
        char bf2[ 20 ] = { 0 };
        sprintf( buffer, "Lv%hhu", p_pokemon.m_level );
        sprintf( bf2, " %huKP", p_pokemon.m_stats.m_acHP );
        IO::regularFont->printString( buffer, p_hpx + 34, p_hpy + 15, false );
        IO::regularFont->setColor( WHITE_IDX, 1 );
        IO::regularFont->setColor( p_hpCol, 2 );
        IO::regularFont->printString( bf2, p_hpx + IO::regularFont->stringWidth( buffer ) + 34, p_hpy + 15, false );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
    }

    u16 battleUI::initStsBalls( bool p_bottom, u16& p_tilecnt ) {
        //Own PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = CUR_PKMN_STS_2( *_battle, i, PLAYER );
            p_tilecnt = IO::loadSprite( p_bottom ? i : ( STSBALL_START + i ),
                                        p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, p_bottom ? ( 16 * i ) : 240 - ( 16 * i ),
                                        180, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                        false, false, false, OBJPRIORITY_0, p_bottom );
        }
        //Opps PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = CUR_PKMN_STS_2( *_battle, i, OPPONENT );
            if( _battle->m_isWildBattle )
                acStat = IN_DEX( _battle->_wildPokemon.m_pokemon->m_boxdata.m_speciesId ) ? acStatus::OK : acStatus::NA;
            p_tilecnt = IO::loadSprite( p_bottom ? ( 6 + i ) : ( STSBALL_START + 6 + i ),
                                        p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, !p_bottom ? ( 16 * i ) : 240 - ( 16 * i ),
                                        -4, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                        false, false, _battle->m_isWildBattle, OBJPRIORITY_0, p_bottom );
        }
        return p_tilecnt;
    }
    void setStsBallVisibility( bool p_opponent, u8 p_pokemonPos, bool p_isHidden, bool p_bottom ) {
        if( p_bottom ) {
            IO::Oam->oamBuffer[ 6 * p_opponent + p_pokemonPos ].isHidden = p_isHidden;
            IO::updateOAM( true );
        } else {
            IO::OamTop->oamBuffer[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].isHidden = p_isHidden;
            IO::updateOAM( false );
        }
    }
    void setStsBallPosition( bool p_opponent, u8 p_pokemonPos, u8 p_x, u8 p_y, bool p_bottom ) {
        if( p_bottom ) {
            IO::Oam->oamBuffer[ 6 * p_opponent + p_pokemonPos ].x = p_x;
            IO::Oam->oamBuffer[ 6 * p_opponent + p_pokemonPos ].y = p_y;
            IO::updateOAM( true );
        } else {
            IO::OamTop->oamBuffer[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].x = p_x;
            IO::OamTop->oamBuffer[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].y = p_y;
            IO::updateOAM( false );
        }
    }
    void setStsBallSts( bool p_opponent, u8 p_pokemonPos, acStatus p_status, bool p_bottom ) {
        u8 idx = 0;
        u16 tileIdx = 0;
        if( p_bottom ) {
            idx = 6 * p_opponent + p_pokemonPos;
            tileIdx = IO::Oam->oamBuffer[ idx ].gfxIndex;
            IO::loadSprite( idx, PB_PAL_SUB( p_status ), tileIdx, p_opponent ? ( 240 - ( 16 * p_pokemonPos ) ) : ( 16 * ( 6 - p_pokemonPos ) ),
                            p_opponent ? -4 : 180, 16, 16, PB_PAL( p_status ), PB_TILES( p_status ), PB_TILES_LEN( p_status ),
                            false, false, false, OBJPRIORITY_0, p_bottom );
            IO::updateOAM( p_bottom );
        } else {
            idx = STSBALL_IDX( p_pokemonPos, p_opponent );
            tileIdx = IO::OamTop->oamBuffer[ idx ].gfxIndex;
            IO::loadSprite( idx, PB_PAL_TOP( p_status ), tileIdx, !p_opponent ? ( 240 - ( 16 * p_pokemonPos ) ) : ( 16 * ( p_pokemonPos ) ),
                            p_opponent ? -4 : 180, 16, 16, PB_PAL( p_status ), PB_TILES( p_status ), PB_TILES_LEN( p_status ),
                            false, false, false, OBJPRIORITY_0, p_bottom );
            IO::updateOAM( false );
        }
    }

    void animatePokeBall( u8 p_x, u8 p_y, u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt ) {

        IO::SpriteInfo * type1Info = &IO::spriteInfoTop[ p_oamIndex ];
        SpriteEntry * type1 = &IO::OamTop->oamBuffer[ p_oamIndex ];
        type1Info->m_oamId = p_oamIndex;
        type1Info->m_width = 16;
        type1Info->m_height = 16;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = p_y;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = p_x;
        type1->size = OBJSIZE_16;
        type1->gfxIndex = p_tileCnt;
        type1->priority = OBJPRIORITY_0;
        type1->palette = p_palCnt;

        IO::copySpritePal( PokeBall1Pal, p_palCnt, false );
        IO::copySpriteData( PokeBall1Tiles, p_tileCnt, PokeBall1TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall2Tiles, p_tileCnt, PokeBall2TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall3Tiles, p_tileCnt, PokeBall3TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall4Tiles, p_tileCnt, PokeBall4TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall5Tiles, p_tileCnt, PokeBall5TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall6Tiles, p_tileCnt, PokeBall6TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall7Tiles, p_tileCnt, PokeBall7TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall8Tiles, p_tileCnt, PokeBall8TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall9Tiles, p_tileCnt, PokeBall9TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall10Tiles, p_tileCnt, PokeBall10TilesLen, false );;
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall11Tiles, p_tileCnt, PokeBall11TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 3; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;

        type1Info->m_oamId = p_oamIndex;
        type1Info->m_width = 64;
        type1Info->m_height = 64;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = p_y - 22;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = p_x - 22;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = p_tileCnt;
        type1->priority = OBJPRIORITY_0;
        type1->palette = p_palCnt;

        IO::copySpritePal( Shiny1Pal, p_palCnt, false );
        IO::copySpriteData( Shiny1Tiles, p_tileCnt, Shiny1TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        IO::copySpritePal( Shiny2Pal, p_palCnt, false );
        IO::copySpriteData( Shiny2Tiles, p_tileCnt, Shiny2TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        IO::updateOAM( false );
    }

    void animateShiny( u8 p_x, u8 p_y, u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt ) {
        IO::SpriteInfo * type1Info = &IO::spriteInfoTop[ p_oamIndex ];
        SpriteEntry * type1 = &IO::OamTop->oamBuffer[ p_oamIndex ];
        type1Info->m_oamId = p_oamIndex;
        type1Info->m_width = 64;
        type1Info->m_height = 64;
        type1Info->m_angle = 0;
        type1Info->m_entry = type1;
        type1->y = p_y;
        type1->isRotateScale = false;
        type1->isHidden = false;
        type1->blendMode = OBJMODE_NORMAL;
        type1->isMosaic = false;
        type1->colorMode = OBJCOLOR_16;
        type1->shape = OBJSHAPE_SQUARE;
        type1->x = p_x;
        type1->size = OBJSIZE_64;
        type1->gfxIndex = p_tileCnt;
        type1->priority = OBJPRIORITY_0;
        type1->palette = p_palCnt;

        IO::copySpritePal( Shiny1Pal, p_palCnt, false );
        IO::copySpriteData( Shiny1Tiles, p_tileCnt, Shiny1TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->priority = OBJPRIORITY_1;
        IO::copySpritePal( Shiny2Pal, p_palCnt, false );
        IO::copySpriteData( Shiny2Tiles, p_tileCnt, Shiny2TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->priority = OBJPRIORITY_0;
        IO::copySpritePal( Shiny1Pal, p_palCnt, false );
        IO::copySpriteData( Shiny1Tiles, p_tileCnt, Shiny1TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->priority = OBJPRIORITY_1;
        IO::copySpritePal( Shiny2Pal, p_palCnt, false );
        IO::copySpriteData( Shiny2Tiles, p_tileCnt, Shiny2TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->priority = OBJPRIORITY_0;
        IO::copySpritePal( Shiny1Pal, p_palCnt, false );
        IO::copySpriteData( Shiny1Tiles, p_tileCnt, Shiny1TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->priority = OBJPRIORITY_1;
        IO::copySpritePal( Shiny2Pal, p_palCnt, false );
        IO::copySpriteData( Shiny2Tiles, p_tileCnt, Shiny2TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        IO::updateOAM( false );
    }

    void battleUI::loadSpritesTop( ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( IO::OamTop );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        REG_BLDCNT = BLEND_FADE_BLACK | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3 | BLEND_SRC_SPRITE;
        REG_BLDY = 0x1F;
        bgUpdate( );

        sprintf( buffer, "%d", _battle->m_backgroundId );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/BATTLE_BACK/", buffer, 512, 49152 );

        u16 val = 0x1F;
        for( s8 i = 4; i >= 0; --i ) {
            for( u8 j = 0; j < 5; ++j )
                swiWaitForVBlank( );
            val &= ~( 1 << u8( i ) );
            REG_BLDY = val;
        }
        swiWaitForVBlank( );

        if( !_battle->m_isWildBattle ) {
            sprintf( buffer, "%03d_1", _battle->_opponent->m_trainerClass );
            FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/BATTLE_MUG/", buffer, 128, 49152 );
            for( u8 i = 0; i < 40; ++i )
                swiWaitForVBlank( );
            sprintf( buffer, "%03d_2", _battle->_opponent->m_trainerClass );
            FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/BATTLE_MUG/", buffer, 128, 49152 );
            for( u8 i = 0; i < 120; ++i )
                swiWaitForVBlank( );

            dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        } else {
            auto acPkmn = *_battle->_wildPokemon.m_pokemon;
            u16 x = 128;
            u8  y = 19;

            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                                     PKMN_IDX( 0, OPPONENT ), PKMN_PAL_IDX( 0, OPPONENT ), PKMN_TILE_IDX( 0, OPPONENT ),
                                     false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                                         acPkmn.m_boxdata.m_speciesId, x, y,
                                         PKMN_IDX( 0, OPPONENT ), PKMN_PAL_IDX( 0, OPPONENT ), PKMN_TILE_IDX( 0, OPPONENT ), false,
                                         acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
            IO::updateOAM( false );
            for( u8 i = 0; i < 112 - 96; ++i ) {
                x++;
                IO::OamTop->oamBuffer[ PKMN_IDX( 0, OPPONENT ) ].x++;
                IO::OamTop->oamBuffer[ PKMN_IDX( 0, OPPONENT ) + 1 ].x++;
                IO::OamTop->oamBuffer[ PKMN_IDX( 0, OPPONENT ) + 2 ].x++;
                IO::OamTop->oamBuffer[ PKMN_IDX( 0, OPPONENT ) + 3 ].x++;
                IO::updateOAM( false );
                swiWaitForVBlank( );
            }
            if( acPkmn.m_boxdata.isShiny( ) )
                animateShiny( x + 16, y + 16, SHINY_ANIM, 15, TILESTART );
        }
    }

    void battleUI::redrawBattle( ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( IO::OamTop );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        REG_BLDCNT = BLEND_FADE_BLACK | BLEND_SRC_BG0 | BLEND_SRC_BG1 | BLEND_SRC_BG2 | BLEND_SRC_BG3 | BLEND_SRC_SPRITE;
        REG_BLDY = 0x1F;
        bgUpdate( );

        sprintf( buffer, "%d", _battle->m_backgroundId );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/BATTLE_BACK/", buffer, 512, 49152 );

        loadBattleUITop( );
        for( u8 i = 0; i < 4; ++i )
            sendPKMN( i % 2, i / 2, true );
        if( _battle->m_isWildBattle ) {
            auto acPkmn = *_battle->_wildPokemon.m_pokemon;
            u16 x = 144;
            u8  y = 19;

            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                                     PKMN_IDX( 0, OPPONENT ), PKMN_PAL_IDX( 0, OPPONENT ), PKMN_TILE_IDX( 0, OPPONENT ),
                                     false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                                         acPkmn.m_boxdata.m_speciesId, x, y,
                                         PKMN_IDX( 0, OPPONENT ), PKMN_PAL_IDX( 0, OPPONENT ), PKMN_TILE_IDX( 0, OPPONENT ), false,
                                         acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
            IO::updateOAM( false );
        }

        REG_BLDY = 0;
        swiWaitForVBlank( );
    }

    void battleUI::loadSpritesSub( ) {
        IO::initOAMTable( true );
        IO::NAV->draw( );

        u16 tilecnt = 0;
        tilecnt = initStsBalls( true, tilecnt );
        initColors( );

        sprintf( buffer, "Eine Herausforderung von\n%s %s!",
                 trainerClassNames[ _battle->_opponent->m_trainerClass ].c_str( ),
                 _battle->_opponent->m_battleTrainerName.c_str( ) );
        IO::regularFont->printString( buffer, 16, 80, true );
        IO::updateOAM( true );
    }

    void battleUI::loadBattleUITop( ) {
        if( !_battle->m_isWildBattle )
            IO::initOAMTable( false );


        TILESTART = initStsBalls( false, TILESTART = ( PKMN_TILE_START + 4 * 144 ) );

        for( u8 i = 0; i < 4; ++i ) {
            TILESTART = IO::loadSprite( HP_IDX( i % 2, ( i / 2 ) ), HP_PAL,
                                        TILESTART, 0, 0, 32, 32, Battle1Pal,
                                        Battle1Tiles, Battle1TilesLen, false,
                                        false, true, OBJPRIORITY_1, false );
        }
        TILESTART = IO::loadSprite( PLATFORM_START, PLAT_PAL,
                                    TILESTART, 128, 56, 64, 64, IO::PlatformPals[ _battle->m_platform2Id ],
                                    IO::PlatformTiles[ 2 * _battle->m_platform2Id ], 2048, false,
                                    false, false, OBJPRIORITY_3, false );
        TILESTART = IO::loadSprite( PLATFORM_START + 1, PLAT_PAL,
                                    TILESTART, 192, 56, 64, 64, IO::PlatformPals[ _battle->m_platform2Id ],
                                    IO::PlatformTiles[ 2 * _battle->m_platform2Id + 1 ], 2048, false,
                                    false, false, OBJPRIORITY_3, false );
        TILESTART = IO::loadSprite( PLATFORM_START + 2, PLAT_PAL + 1,
                                    TILESTART, -52, 152 - 32, 64, 64, IO::PlatformPals[ _battle->m_platformId ],
                                    IO::PlatformTiles[ 2 * _battle->m_platformId ], 2048, false,
                                    false, false, OBJPRIORITY_3, false );
        TILESTART = IO::loadSprite( PLATFORM_START + 3, PLAT_PAL + 1,
                                    TILESTART, 80 - 16, 152 - 32, 64, 64, IO::PlatformPals[ _battle->m_platformId ],
                                    IO::PlatformTiles[ 2 * _battle->m_platformId + 1 ], 2048, false,
                                    false, false, OBJPRIORITY_3, false );
        IO::OamTop->oamBuffer[ PLATFORM_START + 2 ].isRotateScale = true;
        IO::OamTop->oamBuffer[ PLATFORM_START + 2 ].isSizeDouble = true;
        IO::OamTop->oamBuffer[ PLATFORM_START + 2 ].rotationIndex = 0;
        IO::OamTop->oamBuffer[ PLATFORM_START + 3 ].isRotateScale = true;
        IO::OamTop->oamBuffer[ PLATFORM_START + 3 ].isSizeDouble = true;
        IO::OamTop->oamBuffer[ PLATFORM_START + 3 ].rotationIndex = 0;
        IO::OamTop->matrixBuffer[ 0 ].hdx = 140;
        IO::OamTop->matrixBuffer[ 0 ].vdy = 130;
        IO::OamTop->matrixBuffer[ 1 ].hdx = 154;
        IO::OamTop->matrixBuffer[ 1 ].vdy = 154;

        IO::updateOAM( false );
        if( _battle->m_isWildBattle ) {
            u8 hpx = 88, hpy = 40;
            auto& acPkmn = *_battle->_wildPokemon.m_pokemon;
            setStsBallPosition( OPPONENT, 0, hpx + 8, hpy + 8, false );
            IO::OamTop->oamBuffer[ HP_IDX( OPPONENT, 0 ) ].isHidden = false;
            IO::OamTop->oamBuffer[ HP_IDX( OPPONENT, 0 ) ].x = hpx;
            IO::OamTop->oamBuffer[ HP_IDX( OPPONENT, 0 ) ].y = hpy;
            setStsBallVisibility( OPPONENT, 0, false, false );
            IO::updateOAM( false );

            pokemonData p;
            getAll( acPkmn.m_boxdata.m_speciesId, p );

            drawPkmnInfo1( hpx, hpy, acPkmn, HP_COL( OPPONENT, 0 ) );

            IO::displayHP( 100, 101, hpx, hpy, HP_COL( OPPONENT, 0 ), HP_COL( OPPONENT, 0 ) + 1, false );
            IO::displayHP( 100, 100 - acPkmn.m_stats.m_acHP * 100 / acPkmn.m_stats.m_maxHP,
                           hpx, hpy, HP_COL( OPPONENT, 0 ), HP_COL( OPPONENT, 0 ) + 1, false );

            IO::displayEP( 0, ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ) * 100 /
                           ( EXP[ acPkmn.m_level ][ p.m_expType ] - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ),
                           hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, false );
        }
    }

    void loadBattleUISub( u16 p_pkmnId, bool p_isWildBattle, bool p_showNav ) {
        u16 tilecnt = 0;
        //Load UI Sprites
        //FIGHT -- 1
        tilecnt = IO::loadSprite( SUB_FIGHT_START, 0, tilecnt, 64,
                                  72, 64, 64, BattleSub1Pal, BattleSub1Tiles, BattleSub1TilesLen,
                                  false, false, false, OBJPRIORITY_2, true );
        tilecnt = IO::loadSprite( SUB_FIGHT_START + 1, 0, tilecnt, 128,
                                  72, 64, 64, BattleSub1Pal, BattleSub1Tiles, BattleSub1TilesLen,
                                  false, true, false, OBJPRIORITY_2, true );
        //FIGHT-TEXT
        tilecnt = IO::loadSprite( SUB_FIGHT_START + 2, 1, tilecnt, 96,
                                  100, 64, 32, BattleSub2Pal, BattleSub2Tiles, BattleSub2TilesLen,
                                  false, false, false, OBJPRIORITY_0, true );
        //RUN / POKENAV
        if( p_isWildBattle ) { //Show Run
            tilecnt = IO::loadSprite( SUB_FIGHT_START + 3, 2, tilecnt, 91,
                                      150, 64, 32, BattleSub3Pal, BattleSub3Tiles, BattleSub3TilesLen,
                                      false, false, false, OBJPRIORITY_3, true );
        } else if( p_showNav ) { //Show Nav
            tilecnt = IO::loadSprite( SUB_FIGHT_START + 3, 2, tilecnt, 91,
                                      150, 64, 32, BattleSub6Pal, BattleSub6Tiles, BattleSub6TilesLen,
                                      false, false, false, OBJPRIORITY_3, true );
        }
        //BAG
        tilecnt = IO::loadSprite( SUB_FIGHT_START + 4, 3, tilecnt, 0,
                                  157, 64, 32, BattleSub4Pal, BattleSub4Tiles, BattleSub4TilesLen,
                                  false, false, false, OBJPRIORITY_3, true );
        //POKEMON
        tilecnt = IO::loadSprite( SUB_FIGHT_START + 5, 4, tilecnt, 185,
                                  142, 64, 32, BattleSub5Pal, BattleSub5Tiles, BattleSub5TilesLen,
                                  false, false, false, OBJPRIORITY_3, true );
        //Load an icon of the PKMN, too
        u8 oamIndex = SUB_FIGHT_START + 6;
        u8 palIndex = 5;

        tilecnt = IO::loadPKMNIcon( p_pkmnId, 112, 68, oamIndex++, palIndex++, tilecnt, true );

        //PreLoad A and Back buttons

        tilecnt = IO::loadSprite( SUB_A_OAM, 6, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                                  ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        IO::ASpriteOamIndex = SUB_A_OAM;
        tilecnt = IO::loadSprite( SUB_Back_OAM, 7, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );

        SUB_TILESTART = tilecnt;
        SUB_PALSTART = 8;
        IO::updateOAM( true );
    }

    void setBattleUISubVisibility( bool p_isHidden = false ) {
        for( u8 i = 0; i <= SUB_FIGHT_START + 6; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = p_isHidden;
        IO::updateOAM( true );
    }

    void loadA( ) {
        IO::loadSprite( SUB_A_OAM, 6, IO::Oam->oamBuffer[ SUB_A_OAM ].gfxIndex,
                        SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                        ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        IO::ASpriteOamIndex = SUB_A_OAM;
        IO::updateOAM( true );
    }

    void setDeclareBattleMoveSpriteVisibility( bool p_showBack, bool p_isHidden = true ) {
        if( p_showBack )
            IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = p_isHidden;
        setBattleUISubVisibility( p_isHidden );
    }

    void battleUI::drawPkmnChoicePkmn( u8 p_index, bool p_firstIsChosen, bool p_pressed ) {
        u8 x = 8 + ( p_index % 2 ) * 120 - ( p_index / 2 ) * 4,
            y = 32 + ( p_index / 2 ) * 48;

        if( !p_index || ( p_firstIsChosen && ( p_index == 1 ) ) || p_index == firstMoveSwitchTarget )
            IO::printChoiceBox( x + 8, y, x + 120, y + 44, 5, RED_IDX, p_pressed );
        else
            IO::printChoiceBox( x + 8, y, x + 120, y + 44, 5, GRAY_IDX, p_pressed );

        if( p_index >= _battle->_player->m_pkmnTeam.size( ) )
            return;

        auto& acPkmnStr = CUR_PKMN_STR_2( *_battle, p_index, PLAYER );
        auto& acPkmn = *acPkmnStr.m_pokemon;

        u8 dx = p_pressed * 2;
        u8 dy = p_pressed;

        if( !acPkmn.m_boxdata.m_individualValues.m_isEgg ) {
            u8 gn = !acPkmn.gender( ) ? 8 : 0;
            IO::regularFont->printString( acPkmn.m_boxdata.m_name, gn + dx + x + 104, dy + y + 2, true, IO::font::RIGHT );
            if( !gn )
                drawGender( dx + x + 104, dy + y + 2, acPkmn.gender( ), true );
            IO::regularFont->setColor( GRAY_IDX, 1 );
            IO::regularFont->setColor( WHITE_IDX, 2 );
            IO::regularFont->printString( ItemList[ acPkmn.m_boxdata.m_holdItem ]->getDisplayName( true ).c_str( ),
                                          dx + x + 113, dy + y + 15, true, IO::font::RIGHT );
            sprintf( buffer, "Lv%d", acPkmn.m_level );
            IO::regularFont->printString( buffer, dx + x + 12, dy + y + 27, true );
            if( acPkmn.m_stats.m_acHP ) {
                switch( acPkmnStr.m_ailments ) {
                    case FREEZE:
                        sprintf( buffer, "Eingfr" );
                        break;
                    case PARALYSIS:
                        sprintf( buffer, "Prlyse" );
                        break;
                    case BURN:
                        sprintf( buffer, "Verbrng" );
                        break;
                    case SLEEP:
                        sprintf( buffer, "Schlaf" );
                        break;
                    case POISONED:
                        sprintf( buffer, "Gift" );
                    default:
                        if( 5 * acPkmn.m_stats.m_acHP <= acPkmn.m_stats.m_maxHP )
                            IO::regularFont->setColor( RED_IDX, 1 );
                        else if( 2 * acPkmn.m_stats.m_acHP <= acPkmn.m_stats.m_maxHP )
                            IO::regularFont->setColor( 241, 1 );
                        else
                            IO::regularFont->setColor( 240, 1 );
                        sprintf( buffer, "%d/%d", acPkmn.m_stats.m_acHP, acPkmn.m_stats.m_maxHP );
                }
            } else {
                IO::regularFont->setColor( RED_IDX, 1 );
                sprintf( buffer, "Besiegt" );
            }
            IO::regularFont->printString( buffer, dx + x + 113, dy + y + 27, true, IO::font::RIGHT );

            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        } else
            IO::regularFont->printString( "Ei", dx + x + 112, dy + y + 2, true, IO::font::RIGHT );
    }

    void battleUI::drawPKMNChoiceScreen( bool p_firstIsChosen ) {
        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &IO::Bottom );

        if( _battle->m_battleMode == battle::DOUBLE && _battle->_battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::SWITCH )
            firstMoveSwitchTarget = _battle->_battleMoves[ 0 ][ PLAYER ].m_value;

        BG_PAL( true )[ 240 ] = GREEN;
        BG_PAL( true )[ 241 ] = YELLOW;
        for( u8 i = 0; i < 6; ++i ) {
            u8 x = 8 + ( i % 2 ) * 120 - ( i / 2 ) * 4,
                y = 32 + ( i / 2 ) * 48;

            drawPkmnChoicePkmn( i, p_firstIsChosen, false );

            if( i >= _battle->_player->m_pkmnTeam.size( ) )
                continue;

            auto& acPkmnStr = CUR_PKMN_STR_2( *_battle, i, PLAYER );
            auto& acPkmn = *acPkmnStr.m_pokemon;
            if( !acPkmn.m_boxdata.m_individualValues.m_isEgg )
                tilecnt = IO::loadPKMNIcon( acPkmn.m_boxdata.m_speciesId, x - 4, y - 12, ++oamIndex, palIndex++, tilecnt, true );
            else
                tilecnt = IO::loadEggIcon( x - 4, y - 12, ++oamIndex, palIndex++, tilecnt, true );
        }

        IO::updateOAM( true );
    }

    void undrawPKMNChoiceScreen( ) {
        IO::NAV->draw( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( true );
    }

    void drawPkmnConfirmation( pokemon p_pokemon, bool p_alreadySent, bool p_alreadyChosen, u8 p_x, u8 p_y, bool p_pressed ) {
        IO::printChoiceBox( p_x - 8, p_y, p_x + 136, p_y + 64, 6, GRAY_IDX, p_pressed );
        bool dead = !p_pokemon.m_stats.m_acHP;
        u8 dx = p_pressed * 2, dy = p_pressed;

        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            if( !p_alreadySent && !p_alreadyChosen && !dead )
                IO::regularFont->printString( "Aussenden", p_x + dx + 64, dy + 52, true, IO::font::CENTER );
            else if( !p_alreadyChosen  && !dead )
                IO::regularFont->printString( "Bereits im Kampf", p_x + dx + 64, dy + 52, true, IO::font::CENTER );
            else if( dead )
                IO::regularFont->printString( "Schon besiegt…", p_x + dx + 64, dy + 52, true, IO::font::CENTER );
            else
                IO::regularFont->printString( "Schon ausgewählt", p_x + dx + 64, dy + 52, true, IO::font::CENTER );

            IO::regularFont->printString( p_pokemon.m_boxdata.m_name, p_x + dx + 58, dy + 66, true, IO::font::CENTER );
            drawGender( p_x + dx + 62 + IO::regularFont->stringWidth( p_pokemon.m_boxdata.m_name ) / 2, dy + 66, p_pokemon.gender( ), true );

            IO::regularFont->setColor( GRAY_IDX, 1 );
            IO::regularFont->setColor( WHITE_IDX, 2 );

            IO::regularFont->printString( ItemList[ p_pokemon.m_boxdata.m_holdItem ]->getDisplayName( true ).c_str( ),
                                          p_x + dx + 64, dy + 80, true, IO::font::CENTER );

            sprintf( buffer, "Lv%d", p_pokemon.m_level );
            IO::regularFont->printString( buffer, dx + p_x, dy + 94, true );
            sprintf( buffer, "%d/%dKP", p_pokemon.m_stats.m_acHP, p_pokemon.m_stats.m_maxHP );
            IO::regularFont->printString( buffer, dx + p_x + 128, dy + 94, true, IO::font::RIGHT );

            IO::regularFont->setColor( BLACK_IDX, 1 );
            IO::regularFont->setColor( GRAY_IDX, 2 );
        } else {
            consoleSetWindow( &IO::Bottom, 8, 11, 16, 10 );
            IO::regularFont->printString( "Ein Ei kann", p_x + dx + 64, dy + 80, true, IO::font::CENTER );
            IO::regularFont->printString( "nicht kämpfen!", p_x + dx + 64, dy + 94, true, IO::font::CENTER );
        }
    }

    /**
    *  @returns 0 if the Pokemon shall be sent, 1 if further information was requested, 2 if the moves should be displayed, 3 if the previous screen shall be shown
    */
    u8 showConfirmation( pokemon& p_pokemon, bool p_alreadySent, bool p_alreadyChosen ) {
        IO::NAV->draw( );
        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        u16 tilecnt = 0;
        u8  palIndex = 4;
        u8 oamIndex = SUB_Back_OAM + 1;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

        consoleSelect( &IO::Bottom );

        bool dead = !p_pokemon.m_stats.m_acHP;
        u8 x = 104, y = 48;
        u16 t2;
        //Switch
        BG_PALETTE_SUB[ GRAY_IDX ] = NORMAL_COLOR;

        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            //Status
            tilecnt = IO::loadSprite( ++oamIndex, 3, tilecnt,
                                      20, 128, 32, 32, memoPal, memoTiles,
                                      memoTilesLen, false, false, false, OBJPRIORITY_1, true );
            IO::printChoiceBox( 36, 132, 124, 156, 6, GRAY_IDX, false );
            IO::regularFont->printString( "Bericht", 56, 137, true );

            //Moves
            tilecnt = IO::loadSprite( ++oamIndex, 4, tilecnt,
                                      200, 128, 32, 32, atksPal, atksTiles,
                                      atksTilesLen, false, false, false, OBJPRIORITY_1, true );
            IO::printChoiceBox( 132, 132, 216, 156, 6, GRAY_IDX, false );
            IO::regularFont->printString( "Attacken", 138, 137, true );
        } else {
            x = 64;
            y = 64;
        }
        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            if( !( t2 = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                                            16, 32, ++oamIndex, ++palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale ) ) )
                t2 = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                                         16, 32, oamIndex, palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale );
            oamIndex += 3;
            tilecnt = t2;
        }

        drawPkmnConfirmation( p_pokemon, p_alreadySent, p_alreadyChosen, x, y, false );

        IO::updateOAM( true );
        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) )  //Back
                return 3;
            if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
                if( !p_alreadySent && !p_alreadyChosen && !dead
                    && IN_RANGE_R( x, y, x + 128, y + 64 ) ) { //Send
                    drawPkmnConfirmation( p_pokemon, p_alreadySent, p_alreadyChosen, x, y, true );
                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP )
                            return 0;
                        if( !IN_RANGE_R( x, y, x + 128, y + 64 ) ) {
                            drawPkmnConfirmation( p_pokemon, p_alreadySent, p_alreadyChosen, x, y, false );
                            break;
                        }
                    }
                }
                if( IN_RANGE_R( 20, 132, 124, 156 ) ) { //Info
                    IO::printChoiceBox( 36, 132, 124, 156, 6, GRAY_IDX, true );
                    IO::regularFont->printString( "Bericht", 58, 138, true );
                    IO::Oam->oamBuffer[ SUB_Back_OAM + 2 ].x += 2;
                    IO::Oam->oamBuffer[ SUB_Back_OAM + 2 ].y += 1;
                    IO::updateOAM( true );
                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP )
                            return 1;
                        if( !IN_RANGE_R( 20, 132, 124, 156 ) ) {
                            IO::printChoiceBox( 36, 132, 124, 156, 6, GRAY_IDX, false );
                            IO::regularFont->printString( "Bericht", 56, 137, true );
                            IO::Oam->oamBuffer[ SUB_Back_OAM + 2 ].x -= 2;
                            IO::Oam->oamBuffer[ SUB_Back_OAM + 2 ].y -= 1;
                            IO::updateOAM( true );
                            break;
                        }
                    }
                }
                if( IN_RANGE_R( 132, 132, 232, 156 ) ) { //Moves
                    IO::printChoiceBox( 132, 132, 216, 156, 6, GRAY_IDX, true );
                    IO::regularFont->printString( "Attacken", 140, 138, true );
                    IO::Oam->oamBuffer[ SUB_Back_OAM + 3 ].x += 2;
                    IO::Oam->oamBuffer[ SUB_Back_OAM + 3 ].y += 1;
                    IO::updateOAM( true );
                    loop( ) {
                        swiWaitForVBlank( );
                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP )
                            return 2;
                        if( !IN_RANGE_R( 132, 132, 232, 156 ) ) {
                            IO::printChoiceBox( 132, 132, 216, 156, 6, GRAY_IDX, false );
                            IO::regularFont->printString( "Attacken", 138, 137, true );
                            IO::Oam->oamBuffer[ SUB_Back_OAM + 3 ].x -= 2;
                            IO::Oam->oamBuffer[ SUB_Back_OAM + 3 ].y -= 1;
                            IO::updateOAM( true );
                            break;
                        }
                    }
                }
            }
        }
    }

    /**
    *  @param p_page: 1 show moves, 0 show status
    *  @returns 0: return to prvious screen, 1 view next pokémon, 2 view previous pokémon, 3 switch screen
    */
    u8 showDetailedInformation( pokemon& p_pokemon, u8 p_page ) {
        IO::NAV->draw( );
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        initColors( );
        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        // ^ Sprite
        tilecnt = IO::loadSprite( SUB_Back_OAM - 2, 1, tilecnt,
                                  SCREEN_WIDTH - 22, SCREEN_HEIGHT - 28 - 24, 32, 32, UpPal,
                                  UpTiles, UpTilesLen, false, false, false, OBJPRIORITY_1, true );
        // v Sprite
        tilecnt = IO::loadSprite( SUB_Back_OAM - 3, 2, tilecnt,
                                  SCREEN_WIDTH - 28 - 24, SCREEN_HEIGHT - 22, 32, 32, DownPal,
                                  DownTiles, DownTilesLen, false, false, false, OBJPRIORITY_1, true );
        if( !p_page ) {
            tilecnt = IO::loadSprite( SUB_Back_OAM - 4, 3, tilecnt,
                                      SCREEN_WIDTH - 20, SCREEN_HEIGHT - 28 - 48, 32, 32, atksPal,
                                      atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_2, true );
        } else {
            tilecnt = IO::loadSprite( SUB_Back_OAM - 5, 3, tilecnt,
                                      SCREEN_WIDTH - 20, SCREEN_HEIGHT - 28 - 48, 32, 32, memoPal,
                                      memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_2, true );
        }

        pokemonData data;
        getAll( p_pokemon.m_boxdata.m_speciesId, data );

        IO::updateOAM( true );

        u16 exptype = data.m_expType;

        u16 t2;
        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            if( !( t2 = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                                            16, 8, ++oamIndex, ++palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale ) ) ) {
                t2 = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                                         16, 8, oamIndex, palIndex, tilecnt,
                                         true, p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale );
            }
            oamIndex += 4;
            tilecnt = t2;

            consoleSetWindow( &IO::Bottom, 4, 0, 12, 2 );
            printf( "EP(%3lu%%)\nKP(%3i%%)", ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][ exptype ] )
                    * 100 / ( EXP[ p_pokemon.m_level ][ exptype ] - EXP[ p_pokemon.m_level - 1 ][ exptype ] ),
                    p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP );
            IO::displayHP( 100, 101, 46, 40, 236, 237, false, 50, 56, true );
            IO::displayHP( 100, 100 - p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP, 46, 40, 236, 237, false, 50, 56, true );

            IO::displayEP( 100, 101, 46, 40, 238, 239, false, 59, 62, true );
            IO::displayEP( 0, ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_level - 1 ][ exptype ] )
                           * 100 / ( EXP[ p_pokemon.m_level ][ exptype ] - EXP[ p_pokemon.m_level - 1 ][ exptype ] ),
                           46, 40, 238, 239, false, 59, 62, true );
            IO::boldFont->setColor( GRAY_IDX, 1 );
            IO::boldFont->setColor( BLACK_IDX, 2 );

            consoleSetWindow( &IO::Bottom, 2, 1, 13, 2 );

            std::sprintf( buffer, "%s /", p_pokemon.m_boxdata.m_name );
            IO::boldFont->printString( buffer, 16, 96, true );
            s8 G = p_pokemon.m_boxdata.gender( );

            if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32 ) {
                if( G == 1 ) {
                    IO::boldFont->setColor( BLUE_IDX, 2 );
                    IO::boldFont->printChar( '{', 100, 102, true );
                } else {
                    IO::boldFont->setColor( RED_IDX, 2 );
                    IO::boldFont->printChar( '}', 100, 102, true );
                }
            }
            IO::boldFont->setColor( GRAY_IDX, 1 );
            IO::boldFont->setColor( BLACK_IDX, 2 );


            IO::boldFont->printString( getDisplayName( p_pokemon.m_boxdata.m_speciesId ), 24, 110, true );

            if( p_pokemon.m_boxdata.getItem( ) ) {
                IO::boldFont->printString( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ),
                                           24, 122, true );
                tilecnt = IO::loadItemIcon( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->m_itemName, 0, 116, ++oamIndex, ++palIndex, tilecnt, true );
            } else {
                ++oamIndex;
                ++palIndex;
                IO::regularFont->printString( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ), 24, 122, true );
            }
            IO::boldFont->setColor( GRAY_IDX, 1 );
            IO::boldFont->setColor( BLACK_IDX, 2 );

            if( data.m_types[ 0 ] == data.m_types[ 1 ] ) {
                tilecnt = IO::loadTypeIcon( data.m_types[ 0 ], 224, 0, ++oamIndex, ++palIndex, tilecnt, true );
                oamIndex++;
                ++palIndex;
            } else {
                tilecnt = IO::loadTypeIcon( data.m_types[ 0 ], 192, 0, ++oamIndex, ++palIndex, tilecnt, true );
                tilecnt = IO::loadTypeIcon( data.m_types[ 1 ], 224, 0, ++oamIndex, ++palIndex, tilecnt, true );
            }

        } else {
            tilecnt = IO::loadEggSprite( 26, 24, ++oamIndex, ++palIndex, tilecnt, true );
            IO::boldFont->setColor( GRAY_IDX, 1 );
            IO::boldFont->setColor( BLACK_IDX, 2 );

            IO::boldFont->printString( "Ei /", 16, 96, true );
            IO::boldFont->printString( "Ei", 24, 110, true );
        }

        //Here starts the page specific stuff
        if( p_pokemon.m_boxdata.m_individualValues.m_isEgg )
            p_page = 0;

        if( p_page == 1 ) { //Moves
            for( u8 i = 0; i < 4; ++i ) {
                if( p_pokemon.m_boxdata.m_moves[ i ] ) {
                    auto acMove = AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ];

                    BG_PALETTE_SUB[ 240 + i ] = IO::getColor( acMove->m_moveType );

                    u8 w = 104, h = 32;
                    u8 x = 144 - 8 * i, y = 18 + ( h + 8 ) * i;

                    IO::printRectangle( x + 1, y + 1, x + w + 2, y + h + 1,
                                        true, false, BLACK_IDX );
                    IO::printRectangle( x, y, x + w, y + h,
                                        true, false, 240 + i );
                    IO::printRectangle( x + 7, y + 5, x + w - 4, y + h - 1,
                                        true, false, BLACK_IDX );
                    IO::printRectangle( x + 6, y + 4, x + w - 6, y + h - 2,
                                        true, false, WHITE_IDX );

                    IO::regularFont->printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                    tilecnt = IO::loadTypeIcon( acMove->m_moveType, x - 10, y - 7, ++oamIndex, ++palIndex, tilecnt, true );
                    tilecnt = IO::loadDamageCategoryIcon( acMove->m_moveHitType, x + 22, y - 7, ++oamIndex, ++palIndex, tilecnt, true );
                    consoleSelect( &IO::Bottom );
                    consoleSetWindow( &IO::Bottom, x / 8, 5 + 5 * i, 20, 2 );
                    printf( "%6hhu/%2hhu AP", p_pokemon.m_boxdata.m_acPP[ 0 ],
                            AttackList[ p_pokemon.m_boxdata.m_moves[ 0 ] ]->m_movePP * ( ( 5 + p_pokemon.m_boxdata.m_ppup.m_Up1 ) / 5 ) );
                }
            }
        } else { //Status
            if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
                IO::boldFont->setColor( GRAY_IDX, 1 );
                IO::boldFont->setColor( BLACK_IDX, 2 );
                sprintf( buffer, "KP                     %3i", p_pokemon.m_stats.m_maxHP );
                IO::boldFont->printString( buffer, 130, 16, true );

#define ADJUST_COLOR( p_val )   if( p_val == 1.1 ) IO::boldFont->setColor( RED_IDX, 2 ); \
                                else if( p_val == 0.9 ) IO::boldFont->setColor( BLUE_IDX, 2 ); \
                                else IO::boldFont->setColor( BLACK_IDX, 2 );

                ADJUST_COLOR( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 0 ] );
                sprintf( buffer, "ANG                   %3i", p_pokemon.m_stats.m_Atk );
                IO::boldFont->printString( buffer, 126, 41, true );

                ADJUST_COLOR( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 1 ] );
                sprintf( buffer, "VER                   %3i", p_pokemon.m_stats.m_Def );
                IO::boldFont->printString( buffer, 124, 58, true );

                ADJUST_COLOR( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 3 ] );
                sprintf( buffer, "SAN                   %3i", p_pokemon.m_stats.m_SAtk );
                IO::boldFont->printString( buffer, 122, 75, true );

                ADJUST_COLOR( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 4 ] );
                sprintf( buffer, "SVE                   %3i", p_pokemon.m_stats.m_SDef );
                IO::boldFont->printString( buffer, 120, 92, true );

                ADJUST_COLOR( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 2 ] );
                sprintf( buffer, "INI                   \xC3\xC3""%3i", p_pokemon.m_stats.m_Spd );
                IO::boldFont->printString( buffer, 118, 109, true );

                IO::printRectangle( (u8) 158, (u8) 18, u8( 158 + 68 ), u8( 18 + 12 ), true, false, WHITE_IDX );

                IO::printRectangle( (u8) 158, (u8) 18, u8( 158 + ( 68.0*p_pokemon.m_boxdata.IVget( 0 ) / 31 ) ), u8( 18 + 6 ), true, false, GRAY_IDX );
                IO::printRectangle( (u8) 158, u8( 18 + 6 ), u8( 158 + ( 68.0*p_pokemon.m_boxdata.m_effortValues[ 0 ] / 252 ) ), u8( 18 + 12 ), true, false, GRAY_IDX );

                for( int i = 1; i < 6; ++i ) {
                    IO::printRectangle( u8( 156 - 2 * i ), u8( 26 + ( 17 * i ) ),
                                        u8( 156 - 2 * i + 68 ), u8( 26 + 12 + ( 17 * i ) ),
                                        true, false, WHITE_IDX );
                    IO::printRectangle( u8( 156 - 2 * i ), u8( 26 + ( 17 * i ) ),
                                        u8( 156 - 2 * i + ( 68.0*p_pokemon.m_boxdata.IVget( i ) / 31 ) ),
                                        u8( 26 + 6 + ( 17 * i ) ),
                                        true, false, GRAY_IDX );
                    IO::printRectangle( u8( 156 - 2 * i ), u8( 26 + 6 + ( 17 * i ) ),
                                        u8( 156 - 2 * i + ( 68.0*p_pokemon.m_boxdata.m_effortValues[ i ] / 252 ) ),
                                        u8( 26 + 12 + ( 17 * i ) ), true, false, GRAY_IDX );
                }

                //Ability
                auto acAbility = ability( p_pokemon.m_boxdata.m_ability );

                IO::printRectangle( u8( 0 ), u8( 138 ), u8( 255 ), u8( 192 ), true, false, WHITE_IDX );
                IO::regularFont->setColor( WHITE_IDX, 2 );
                IO::regularFont->setColor( BLACK_IDX, 1 );
                u8 nlCnt = 0;
                auto nStr = FS::breakString( acAbility.m_flavourText, IO::regularFont, 250 );
                for( auto c : nStr )
                    if( c == '\n' )
                        nlCnt++;
                IO::regularFont->printString( nStr.c_str( ), 0, 138, true, IO::font::LEFT, u8( 16 - 2 * nlCnt ) );
                IO::regularFont->printString( acAbility.m_abilityName.c_str( ), 5, 176, true );
                IO::regularFont->setColor( GRAY_IDX, 1 );
                IO::regularFont->setColor( BLACK_IDX, 2 );
            } else {
                IO::boldFont->setColor( WHITE_IDX, 1 );
                IO::boldFont->setColor( BLACK_IDX, 2 );
                if( p_pokemon.m_boxdata.m_steps > 10 ) {
                    IO::boldFont->printString( "Was da wohl", 16 * 8, 50, true );
                    IO::boldFont->printString( "schlüpfen wird?", 16 * 8, 70, true );
                    IO::boldFont->printString( "Es dauert wohl", 16 * 8, 100, true );
                    IO::boldFont->printString( "noch lange.", 16 * 8, 120, true );
                } else if( p_pokemon.m_boxdata.m_steps > 5 ) {
                    IO::boldFont->printString( "Hat es sich", 16 * 8, 50, true );
                    IO::boldFont->printString( "gerade bewegt?", 16 * 8, 70, true );
                    IO::boldFont->printString( "Da tut sich", 16 * 8, 100, true );
                    IO::boldFont->printString( "wohl bald was.", 16 * 8, 120, true );
                } else {
                    IO::boldFont->printString( "Jetzt macht es", 16 * 8, 50, true );
                    IO::boldFont->printString( "schon Geräusche!", 16 * 8, 70, true );
                    IO::boldFont->printString( "Bald ist es", 16 * 8, 100, true );
                    IO::boldFont->printString( "wohl soweit.", 16 * 8, 120, true );
                }
                IO::boldFont->setColor( GRAY_IDX, 1 );
                IO::boldFont->setColor( BLACK_IDX, 2 );
            }
        }

        IO::updateOAM( true );
        touchPosition touch;
        loop( ) {

            scanKeys( );
            touchRead( &touch );
            u32 pressed = keysHeld( );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) )  //Back
                return 0;
            else if( GET_AND_WAIT( KEY_UP ) )
                return 2;
            else if( GET_AND_WAIT( KEY_DOWN ) )
                return 1;
            else if( GET_AND_WAIT( KEY_RIGHT ) )
                return 3;
            else if( GET_AND_WAIT( KEY_LEFT ) )
                return 3;
            else if( GET_AND_WAIT_C( 16 + SCREEN_WIDTH - 22, 16 + SCREEN_HEIGHT - 28 - 24, 16 ) )
                return 2;
            else if( GET_AND_WAIT_C( 16 + SCREEN_WIDTH - 28 - 24, 16 + SCREEN_HEIGHT - 22, 16 ) )
                return 1;
            else if( GET_AND_WAIT_C( 16 + SCREEN_WIDTH - 20, 16 + SCREEN_HEIGHT - 28 - 48, 16 ) )
                return 3;
        }
    }

    void getSpritePos( bool p_opponent, u8 p_pokemonPos, bool p_double, s16& p_x, s16& p_y, u8& p_hpx, u8& p_hpy ) {
        if( p_opponent ) {
            p_hpx = 88; p_hpy = 40;
            p_x = 176; p_y = 20;
            if( p_pokemonPos ) {
                p_hpx -= 88; p_hpy -= 32;
                p_x = 112; p_y = 14;
            }
            if( !p_double ) {
                p_x -= 32;
                p_y += 3;
            }
        } else {
            p_hpx = 220; p_hpy = 152;
            p_x = 40; p_y = 100;
            if( !p_pokemonPos ) {
                p_hpx -= 88; p_hpy -= 32;
                p_x = -40; p_y = 84;
            }
            if( !p_double ) {
                p_x += 36;
                p_y += 8;
            }
        }
    }

    void adjustSprite( bool p_opponent, u8 p_pokemonPos ) {
        for( u8 k = 0; k < 4; ++k ) {
            IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) + k ].isRotateScale = true;
            IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) + k ].isSizeDouble = true;
            IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) + k ].rotationIndex = 1;
            IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) + k ].priority = OBJPRIORITY_3;
        }
        IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) ].y -= 48;
        IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) ].x -= 48;
        IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) + 1 ].y -= 48;
        IO::OamTop->oamBuffer[ PKMN_IDX( p_pokemonPos, p_opponent ) + 2 ].x -= 48;
    }

    /**
    *  @brief Loads all the required graphics and sprites
    */
    void battleUI::init( ) {
        //Copy the current PKMNStatus
        for( u8 i = 0; i < 6; ++i )
            for( u8 p = 0; p < 2; ++p )
                for( u8 s = 0; s < MAX_STATS; ++s )
                    _oldPKMNStats[ CUR_POS_2( *_battle, i, p ) ][ p ][ s ] = CUR_PKMN_STS_2( *_battle, i, p );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::Oam->oamBuffer[ SUB_A_OAM ].gfxIndex = 0;
        loadA( );
    }

    void battleUI::trainerIntro( ) {
        loadSpritesSub( );
        loadSpritesTop( ); // This should consume some time

        loadBattleUITop( );
        IO::initOAMTable( true );
        IO::NAV->draw( );
        initLogScreen( );
    }
    void battleUI::pokemonIntro( ) {
        loadSpritesTop( ); // This should consume some time

        loadBattleUITop( );
        IO::initOAMTable( true );
        IO::NAV->draw( );
        initLogScreen( );
        IO::loadSprite( SUB_A_OAM, 6, 0,
                        SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                        ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        IO::ASpriteOamIndex = SUB_A_OAM;
        _battle->log( std::string( "Ein wildes " ) + _battle->_wildPokemon.m_pokemon->m_boxdata.m_name + " erscheint![A]" );
    }

    u8 firstMoveSwitchTarget = 0;
    bool battleUI::declareBattleMove( u8 p_pokemonPos, bool p_showBack ) {
        char buffer[ 100 ];
        std::sprintf( buffer, "Was soll %s tun?", CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_name );
        writeLogText( buffer );

        loadBattleUISub( CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                         _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
        if( p_showBack ) {
            IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
            IO::updateOAM( true );
        }

        touchPosition touch;
        auto& result = _battle->_battleMoves[ p_pokemonPos ][ PLAYER ];
        loop( ) {

            scanKeys( );
            touchRead( &touch );

            //Accept touches that are almost on the sprite
            if( p_showBack && GET_AND_WAIT_R( 224, 164, 300, 300 ) ) {
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                return false;
            } else if( GET_AND_WAIT_R( 74, 81, 181, 127 ) ) { //Attacks
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::ATTACK;
SHOW_ATTACK:
                //Check if the PKMN still has AP to use, if not, the move becomes struggle
                u16 apCnt = 0;
                for( u8 i = 0; i < 4; ++i )
                    apCnt += CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_acPP[ i ];

                if( !apCnt ) {
                    result.m_value = M_STRUGGLE;
                    result.m_target = 0;
                    loadA( );
                    return true;
                }

                result.m_value = chooseAttack( p_pokemonPos );
                if( result.m_value ) {
                    if( _battle->m_battleMode == battle::DOUBLE ) {
                        result.m_target = chooseAttackTarget( p_pokemonPos, result.m_value );
                        if( result.m_target ) {
                            loadA( );
                            return true;
                        }
                        goto SHOW_ATTACK;
                    } else {
                        result.m_target = 0;
                        loadA( );
                        return true;
                    }
                }
                loadBattleUISub( CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( buffer );

            } else if( GET_AND_WAIT_R( 0, 162, 58, 300 ) ) { //Bag

                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::USE_ITEM;
                result.m_value = chooseItem( );
                result.m_target = 0;
                loadBattleUISub( CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
                if( result.m_value ) {
                    if( ItemList[ result.m_value ]->m_itemType == item::MEDICINE ) {
                        u8 res = choosePKMN( p_pokemonPos + ( _battle->m_battleMode == battle::DOUBLE ), true, true );
                        if( res != (u8) -1 ) {
                            result.m_target |= ( 1 << res );
                            result.m_newItemEffect = ItemList[ result.m_value ]->m_itemData.m_itemEffect;
                            for( u8 i = 0; i < 2; ++i )
                                if( ItemList[ result.m_value ]->needsInformation( i ) ) {
                                    IO::choiceBox cb( CUR_PKMN_2( *_battle, res, PLAYER ), 0 );
                                    u8 res = 1 + cb.getResult( "Welche Attacke?", false, false );

                                    result.m_newItemEffect &= ~( 1 << ( 9 + 16 * !i ) );
                                    result.m_newItemEffect |= ( res << ( 9 + 16 * !i ) );
                                }
                        } else {
                            loadBattleUISub( CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                             _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
                            goto NEXT_TRY;
                        }
                    } else if( ItemList[ result.m_value ]->m_itemType == item::POKE_BALLS )
                        result.m_target |= ( 1 << 2 );
                    IO::initOAMTable( true );
                    IO::NAV->draw( );
                    loadBattleUISub( CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                     _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );

                    setDeclareBattleMoveSpriteVisibility( p_showBack, true );
                    initLogScreen( );
                    loadA( );
                    return true;
                }
NEXT_TRY:
                initLogScreen( );
                std::sprintf( buffer, "Was soll %s tun?", CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_name );
                writeLogText( buffer );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );

            } else if( !_battle->m_isWildBattle && FS::SAV->m_activatedPNav && GET_AND_WAIT_R( 95, 152, 152, 178 ) ) {

                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::USE_NAV;
                useNav( );
                if( result.m_value ) {
                    loadA( );
                    return true;
                }
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( buffer );

            } else if( _battle->m_isWildBattle && GET_AND_WAIT_R( 97, 162, 153, 180 ) ) { //Run
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::RUN;
                if( _battle->run( ) ) {
                    _battle->log( "Du bist entkommen.[A]" );
                    return true;
                } else
                    _battle->log( "Flucht gescheitert…[A]" );
                if( result.m_value ) {
                    loadA( );
                    return true;
                }
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( buffer );

            } else if( GET_AND_WAIT_R( 195, 148, 238, 176 ) ) { // Switch Pkmn
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::SWITCH;
                result.m_value = choosePKMN( p_pokemonPos + ( _battle->m_battleMode == battle::DOUBLE ) );
                if( result.m_value != (u8) -1 && result.m_value ) {
                    loadA( );
                    return true;
                }
                loadBattleUISub( CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( buffer );
            }
        }
    }

    u16 battleUI::chooseAttack( u8 p_pokemonPos ) {
        u16 result = 0;

        writeLogText( "Welche Attacke?" );

        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;

        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &IO::Bottom );

        auto acPkmn = CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER );

        for( u8 i = 0; i < 4; ++i ) {
            if( acPkmn.m_boxdata.m_moves[ i ] ) {
                auto acMove = AttackList[ acPkmn.m_boxdata.m_moves[ i ] ];

                BG_PALETTE_SUB[ 240 + i ] = IO::getColor( acMove->m_moveType );

                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

                IO::printChoiceBox( x, y, x + w + 2, y + h + 1, 6, 240 + i, false );

                IO::regularFont->printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                tilecnt = IO::loadTypeIcon( acMove->m_moveType, x - 7, y - 7, ++oamIndex, palIndex++, tilecnt, true );
                tilecnt = IO::loadDamageCategoryIcon( acMove->m_moveHitType, x + 25, y - 7, ++oamIndex, palIndex++, tilecnt, true );
                consoleSelect( &IO::Bottom );
                consoleSetWindow( &IO::Bottom, x / 8, 12 + ( i / 2 ) * 6, 20, 2 );
                printf( "%6hhu/%2hhu AP",
                        acPkmn.m_boxdata.m_acPP[ i ],
                        s8( AttackList[ acPkmn.m_boxdata.m_moves[ i ] ]->m_movePP * ( ( 5 + acPkmn.m_boxdata.PPupget( i ) ) / 5.0 ) ) );
            }
        }

        IO::updateOAM( true );

        touchPosition touch;
        loop( ) {
NEXT:
            scanKeys( );
            touchRead( &touch );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) ) { //Back
                result = 0;
                break;
            }
            for( u8 i = 0; i < 4; ++i ) {
                if( !acPkmn.m_boxdata.m_moves[ i ] )
                    break;
                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );
                if( IN_RANGE_R( x, y, x + w, y + h ) && acPkmn.m_boxdata.m_acPP[ i ] ) {
                    auto acMove = AttackList[ acPkmn.m_boxdata.m_moves[ i ] ];

                    IO::printChoiceBox( x, y, x + w + 2, y + h + 1, 6, 240 + i, true );

                    IO::regularFont->printString( acMove->m_moveName.c_str( ), x + 9, y + 8, true );

                    loop( ) {
                        swiWaitForVBlank( );

                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP )
                            break;
                        if( !IN_RANGE_R( x, y, x + w, y + h ) ) {
                            IO::printChoiceBox( x, y, x + w + 2, y + h + 1, 6, 240 + i, false );

                            IO::regularFont->printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                            goto NEXT;
                        }
                    }

                    result = acPkmn.m_boxdata.m_moves[ i ];
                    goto END;
                }
            }
        }
END:
        IO::NAV->draw( );
        initColors( );
        clearLogScreen( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( true );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        return result;
    }


    void battleUI::drawAttackTargetChoice( bool p_selected[ 4 ], bool p_neverTarget[ 4 ], u8 p_pokemonPos ) {
        if( p_selected[ 2 ] && p_selected[ 3 ] )
            IO::printRectangle( 112 + 1, 130 + 1, 112 + 16 + 2, 146 + 1, true, false, BLACK_IDX );
        if( p_selected[ 0 ] && p_selected[ 1 ] )
            IO::printRectangle( 120 + 1, 82 + 1, 120 + 16 + 2, 98 + 1, true, false, BLACK_IDX );

        if( p_selected[ 1 ] && p_selected[ 2 ] )
            IO::printRectangle( 56 + 1, 106 + 1, 56 + 16 + 2, 122 + 1, true, false, BLACK_IDX );
        if( p_selected[ 3 ] && p_selected[ 0 ] )
            IO::printRectangle( 176 + 1, 106 + 1, 176 + 16 + 2, 122 + 1, true, false, BLACK_IDX );

        _battle->_battleMoves[ p_pokemonPos ][ PLAYER ].m_target = 1 | 2 | 4 | 8;

        for( u8 i = 0; i < 4; ++i ) {
            u8 aI = i % 2;
            if( 1 - ( i / 2 ) )
                aI = ( 1 - aI );

            auto acPkmn = CUR_PKMN_2( *_battle, aI, 1 - ( i / 2 ) );

            u8 w = 104, h = 32;
            u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

            IO::printChoiceBox( x, y, x + w + 2, y + h + 1, 6, ( p_selected[ i ] && !p_neverTarget[ i ] ) ? RED_IDX : GRAY_IDX, false );

            if( p_neverTarget[ i ] )
                continue;

            if( acPkmn.m_stats.m_acHP )
                IO::regularFont->printString( acPkmn.m_boxdata.m_name, x + 7, y + 7, true );
        }

        if( p_selected[ 2 ] && p_selected[ 3 ] )
            IO::printRectangle( 112, 130, 112 + 16, 146, true, false, RED_IDX );
        if( p_selected[ 0 ] && p_selected[ 1 ] )
            IO::printRectangle( 120, 82, 120 + 16, 98, true, false, RED_IDX );

        if( p_selected[ 1 ] && p_selected[ 2 ] )
            IO::printRectangle( 56, 106, 56 + 16, 122, true, false, RED_IDX );
        if( p_selected[ 3 ] && p_selected[ 0 ] )
            IO::printRectangle( 176, 106, 176 + 16, 122, true, false, RED_IDX );
    }
    u8 battleUI::chooseAttackTarget( u8 p_pokemonPos, u16 p_move ) {
        u8 result = 0;

        writeLogText( "Welches PKMN angreifen?" );

        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;

        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &IO::Bottom );

        bool selected[ 4 ] = { false };
        bool neverTarget[ 4 ] = { false };
        bool selectionExists = true;

        auto acMove = AttackList[ p_move ];

        switch( acMove->m_moveAffectsWhom ) {
            case move::BOTH_FOES:
            case move::OPPONENTS_FIELD:
                neverTarget[ 2 ] = neverTarget[ 3 ] = true;
                selected[ 0 ] = selected[ 1 ] = true;
                break;
            case move::BOTH_FOES_AND_PARTNER:
                neverTarget[ p_pokemonPos + 2 ] = true;
                selected[ 0 ] = selected[ 1 ]
                    = selected[ 3 - p_pokemonPos ] = true;
                break;
            case move::OWN_FIELD:
                neverTarget[ 0 ] = neverTarget[ 1 ] = true;
                selected[ 1 ] = selected[ 0 ] = true;
                break;
            case move::SELECTED:
                neverTarget[ 2 + p_pokemonPos ] = true;
                selectionExists = false;
                break;
            case move::USER:
                selected[ 2 + p_pokemonPos ] = true;
                neverTarget[ 0 ] = neverTarget[ 1 ]
                    = neverTarget[ 3 - p_pokemonPos ] = true;
                break;
            default:
            case move::RANDOM:
                selected[ 2 ] = selected[ 3 ] = true;
                selected[ 0 ] = selected[ 1 ] = true;
                break;
        }

        drawAttackTargetChoice( selected, neverTarget, p_pokemonPos );
        for( u8 i = 0; i < 4; ++i ) {
            u8 aI = i % 2;
            if( 1 - ( i / 2 ) )
                aI = ( 1 - aI );

            auto acPkmn = CUR_PKMN_2( *_battle, aI, 1 - ( i / 2 ) );
            if( neverTarget[ i ] )
                continue;

            u8 w = 104, h = 32;
            u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

            if( acPkmn.m_stats.m_acHP )
                tilecnt = IO::loadPKMNIcon( acPkmn.m_boxdata.m_speciesId,
                                            x - 10, y - 23, ++oamIndex, palIndex++, tilecnt );
        }
        IO::updateOAM( true );

        touchPosition touch;
        loop( ) {
NEXT:
            scanKeys( );
            touchRead( &touch );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) ) { //Back
                result = 0;
                break;
            }

            for( u8 i = 0; i < 4; ++i ) {
                u8 aI = i % 2;
                if( 1 - ( i / 2 ) )
                    aI = ( 1 - aI );

                auto acPkmn = CUR_PKMN_2( *_battle, aI, 1 - ( i / 2 ) );
                if( neverTarget[ i ] || !acPkmn.m_stats.m_acHP )
                    continue;
                if( !selected[ i ] && selectionExists )
                    continue;

                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );
                if( touch.px >= x && touch.py >= y && touch.px <= x + w && touch.py <= y + h ) {

                    if( selected[ 2 ] && selected[ 3 ] )
                        IO::printRectangle( 112, 130, 112 + 16, 132, true, false, 0 );
                    if( selected[ 0 ] && selected[ 1 ] )
                        IO::printRectangle( 120, 82, 120 + 16, 84, true, false, 0 );

                    if( selected[ 1 ] && selected[ 2 ] )
                        IO::printRectangle( 56, 106, 56 + 2, 122, true, false, 0 );
                    if( selected[ 3 ] && selected[ 0 ] )
                        IO::printRectangle( 176, 106, 176 + 2, 122, true, false, 0 );

                    for( u8 j = 0; j < 4; ++j ) {
                        if( !selected[ j ] && j != i )
                            continue;

                        u8 aJ = j % 2;
                        if( 1 - ( j / 2 ) )
                            aJ = ( 1 - aJ );

                        auto acPkmnJ = CUR_PKMN_2( *_battle, aJ, 1 - ( j / 2 ) );

                        u8 nx = 16 - 8 * ( j / 2 ) + ( w + 16 ) * ( j % 2 ), ny = 74 + ( h + 16 ) * ( j / 2 );
                        IO::printChoiceBox( nx, ny, nx + w + 2, ny + h + 1, 6, RED_IDX, true );
                        if( neverTarget[ j ] || !acPkmnJ.m_stats.m_acHP )
                            continue;
                        IO::regularFont->printString( acPkmnJ.m_boxdata.m_name, nx + 9, ny + 8, true );
                    }
                    if( selected[ 2 ] && selected[ 3 ] )
                        IO::printRectangle( 112 + 1, 130 + 1, 112 + 16 + 2, 146 + 1, true, false, RED_IDX );
                    if( selected[ 0 ] && selected[ 1 ] )
                        IO::printRectangle( 120 + 1, 82 + 1, 120 + 16 + 2, 98 + 1, true, false, RED_IDX );

                    if( selected[ 1 ] && selected[ 2 ] )
                        IO::printRectangle( 56 + 2, 106 + 1, 56 + 16 + 2, 122 + 2, true, false, RED_IDX );
                    if( selected[ 3 ] && selected[ 0 ] )
                        IO::printRectangle( 176 + 2, 106 + 1, 176 + 16 + 2, 122 + 2, true, false, RED_IDX );

                    loop( ) {
                        swiWaitForVBlank( );

                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP )
                            break;
                        if( !( touch.px >= x && touch.py >= y && touch.px <= x + w && touch.py <= y + h ) ) {
                            drawAttackTargetChoice( selected, neverTarget, p_pokemonPos );
                            goto NEXT;
                        }
                    }

                    selected[ i ] = true;

                    if( acMove->m_moveAffectsWhom == move::RANDOM
                        || ( acMove->m_moveAffectsWhom & move::OWN_FIELD )
                        || ( acMove->m_moveAffectsWhom & move::OPPONENTS_FIELD )
                        || ( acMove->m_moveAffectsWhom == move::DEPENDS_ON_ATTACK ) )
                        result = 0;
                    result = selected[ 2 ] | ( selected[ 3 ] << 1 ) | ( selected[ 1 ] << 2 ) | ( selected[ 0 ] << 3 );
                    goto END;
                }
            }
        }
END:
        IO::NAV->draw( );
        initColors( );
        clearLogScreen( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( true );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );

        return result;
    }

    u16 battleUI::chooseItem( ) {
        if( !_battle->_player->m_items && _battle->_player->m_itemCount == MAX_ITEMS_IN_BAG ) {
            BAG::bagViewer bv;
            UPDATE_TIME = false;
            u16 itm = bv.getItem( BAG::bagViewer::BATTLE );
            IO::initOAMTable( true );
            IO::initOAMTable( false );
            IO::NAV->draw( );
            UPDATE_TIME = true;
            DRAW_TIME = true;
            redrawBattle( );
            initLogScreen( );
            return itm;
        }
        return 0;
    }

    u8 battleUI::choosePKMN( bool p_firstIsChosen, bool p_back, bool p_noRestrict ) {
START:
        consoleSelect( &IO::Bottom );
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        u8 result = 0;
        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = !p_back;
        IO::updateOAM( true );
        IO::NAV->draw( );
        initColors( );
        IO::printRectangle( (u8) 0, (u8) 0, (u8) 255, (u8) 28, true, false, WHITE_IDX );

        writeLogText( "Welches PKMN?" );

        drawPKMNChoiceScreen( p_firstIsChosen );
        touchPosition touch;
        loop( ) {
NEXT:
            IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = !p_back;
            IO::updateOAM( true );


            scanKeys( );
            touchRead( &touch );

            //Accept touches that are almost on the sprite
            if( p_back && GET_AND_WAIT_R( 224, 164, 300, 300 ) ) { //Back
                result = -1;
                break;
            }
            auto teamSz = _battle->_player->m_pkmnTeam.size( );
            for( u8 i = 0; i < teamSz; ++i ) {
                if( p_noRestrict && CUR_PKMN_2( *_battle, i, PLAYER ).isEgg( ) )
                    continue;
                u8 x = 8 + ( i % 2 ) * 120 - ( i / 2 ) * 4,
                    y = 32 + ( i / 2 ) * 48;

                if( touch.px >= x + 8 && touch.py >= y && touch.px <= x + 120 && touch.py <= y + 44 ) {
                    drawPkmnChoicePkmn( i, p_firstIsChosen, true );
                    loop( ) {
                        swiWaitForVBlank( );

                        scanKeys( );
                        touchRead( &touch );
                        if( TOUCH_UP )
                            break;
                        if( !( touch.px >= x + 8 && touch.py >= y && touch.px <= x + 120 && touch.py <= y + 44 ) ) {
                            drawPkmnChoicePkmn( i, p_firstIsChosen, false );
                            goto NEXT;
                        }
                    }
                    result = i;
                    u8 tmp = 1;
                    if( !p_noRestrict ) {
                        auto acPkmn = CUR_PKMN_2( *_battle, result, PLAYER );
                        undrawPKMNChoiceScreen( );
                        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                        consoleClear( );
                        while( ( tmp = showConfirmation( acPkmn, result <= p_firstIsChosen, result == firstMoveSwitchTarget && result > 1 ) ) ) {
                            if( tmp == 3 )
                                break;
                            u8 oldtmp = tmp - 1;
                            while( ( tmp = showDetailedInformation( acPkmn, tmp - 1 ) ) ) {
                                if( tmp == 1 ) {
                                    result = ( result + 1 + oldtmp ) % teamSz;
                                    acPkmn = CUR_PKMN_2( *_battle, result, PLAYER );
                                    tmp = 1 + oldtmp;
                                }
                                if( tmp == 2 ) {
                                    result = ( result + teamSz - 1 ) % teamSz;
                                    acPkmn = CUR_PKMN_2( *_battle, result, PLAYER );
                                    tmp = 1 + oldtmp;
                                }
                                if( tmp == 3 ) {
                                    tmp = 1 + ( 1 - oldtmp );
                                    oldtmp = tmp - 1;
                                }
                            }
                            acPkmn = CUR_PKMN_2( *_battle, result, PLAYER );
                            undrawPKMNChoiceScreen( );
                            consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                            consoleClear( );
                        }
                    }
                    if( !tmp || p_noRestrict )
                        goto CLEAR;
                    goto START;
                }
            }
        }

CLEAR:
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        clearLogScreen( );
        initColors( );
        return result;
    }

    void battleUI::useNav( ) {
        _battle->log( "Use Nav[A]" );
    }

    void battleUI::showAttack( bool p_opponent, u8 p_pokemonPos ) {
        // Attack animation here

        auto acMove = _battle->_battleMoves[ p_pokemonPos ][ p_opponent ];

        if( acMove.m_type != battle::battleMove::ATTACK )
            return;
    }

    void battleUI::updateHP( bool p_opponent, u8 p_pokemonPos, u16 p_oldHP, u16 p_oldHPmax ) {
        if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
            return;
        if( p_oldHPmax == u16( -1 ) )
            p_oldHPmax = CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_maxHP;

        u8 hpx = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
            hpy = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y;

        IO::displayHP( p_oldHP * 100 / p_oldHPmax,
                       100 - CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP * 100
                       / CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_maxHP,
                       hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, true );

        consoleSelect( &IO::Top );
        auto acPkmn = CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent );
        if( p_opponent == p_pokemonPos )
            drawPkmnInfo2( hpx, hpy, acPkmn, HP_COL( p_opponent, p_pokemonPos ) );
        else
            drawPkmnInfo1( hpx, hpy, acPkmn, HP_COL( p_opponent, p_pokemonPos ) );
    }

    void battleUI::applyEXPChanges( bool p_opponent, u8 p_pokemonPos, u32 p_gainedExp ) {

        if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
            return;

        u8 hpx = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
            hpy = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y;

        pokemonData p;
        auto& acPkmn = CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent );

        if( !acPkmn.m_stats.m_acHP )
            return;

        getAll( acPkmn.m_boxdata.m_speciesId, p );

        u16 expStart = ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] - p_gainedExp ) * 100 /
            ( EXP[ acPkmn.m_level ][ p.m_expType ] - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] );
        u16 expEnd = std::min( u16( 100 ), u16( ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ) * 100 /
                                                ( EXP[ acPkmn.m_level ][ p.m_expType ] - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ) ) );

        std::sprintf( buffer, "%s gewinnt %lu E.-Punkte.[A]", acPkmn.m_boxdata.m_name, p_gainedExp );
        _battle->log( buffer );
        IO::displayEP( expStart, expEnd, hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, true );


        bool newLevel = EXP[ acPkmn.m_level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
        u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;
        u16 oldHP = acPkmn.m_stats.m_acHP;
        u16 oldHPmax = acPkmn.m_stats.m_maxHP;

        while( newLevel ) {
            acPkmn.m_level++;

            if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ]
                                             + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPkmn.m_level / 100 ) + 10;
            else
                acPkmn.m_stats.m_maxHP = 1;
            pkmnNatures nature = acPkmn.m_boxdata.getNature( );

            acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2 * p.m_bases[ ATK + 1 ]
                                         + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 ) * NatMod[ nature ][ ATK ];
            acPkmn.m_stats.m_Def = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                                         + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ DEF ];
            acPkmn.m_stats.m_Spd = ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                                         + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SPD ];
            acPkmn.m_stats.m_SAtk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                                          + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SATK ];
            acPkmn.m_stats.m_SDef = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                                          + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ] >> 2 ) )*acPkmn.m_level / 100.0 ) + 5 )*NatMod[ nature ][ SDEF ];

            acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

            std::sprintf( buffer, "%s erreicht Level %d.[A]", acPkmn.m_boxdata.m_name, acPkmn.m_level );
            _battle->log( buffer );

            updateHP( p_opponent, p_pokemonPos, oldHP, oldHPmax );
            oldHP = acPkmn.m_stats.m_acHP;
            oldHPmax = acPkmn.m_stats.m_maxHP;

            u8 oldSpec = acPkmn.m_boxdata.m_speciesId;
            _battle->checkForAttackLearn( p_pokemonPos );
            _battle->checkForEvolution( PLAYER, p_pokemonPos );
            if( oldSpec != acPkmn.m_boxdata.m_speciesId )
                _battle->checkForAttackLearn( p_pokemonPos );
            newLevel = acPkmn.m_level < 100 && EXP[ acPkmn.m_level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;

            expStart = 0;
            expEnd = std::min( u16( 100 ), u16( ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ) * 100 /
                                                ( EXP[ acPkmn.m_level ][ p.m_expType ] - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ) ) );

            IO::displayEP( 101, 101, hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, false );
            IO::displayEP( expStart, expEnd, hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, true );
        }
    }

    void battleUI::updateStats( bool p_opponent, u8 p_pokemonPos, bool p_move ) {
        //if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
        //    return;
        if( _battle->m_isWildBattle && p_opponent )
            return;
        setStsBallSts( p_opponent, p_pokemonPos, CUR_PKMN_STS_2( *_battle, p_pokemonPos, p_opponent ), false );
        if( p_pokemonPos <= ( _battle->m_battleMode == battle::DOUBLE ) && p_move
            && CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP ) {
            u8 hpx = 0, hpy = 0;
            s16 x, y;
            getSpritePos( p_opponent, p_pokemonPos, _battle->m_battleMode == battle::DOUBLE, x, y, hpx, hpy );
            setStsBallPosition( p_opponent, p_pokemonPos, hpx + 8, hpy + 8, false );
        }
        IO::updateOAM( false );
    }

    void battleUI::updateStatus( bool p_opponent, u8 p_pokemonPos ) {
        (void) p_opponent;
        (void) p_pokemonPos;
    }

    void battleUI::showStatus( bool p_opponent, u8 p_pokemonPos ) {
        (void) p_opponent;
        (void) p_pokemonPos;
    }

    void battleUI::hidePKMN( bool p_opponent, u8 p_pokemonPos ) {
        if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
            return;

        //Hide PKMN sprite
        for( u8 i = PKMN_IDX( p_pokemonPos, p_opponent ); i < PKMN_IDX( p_pokemonPos, p_opponent ) + 4; ++i ) {
            IO::OamTop->oamBuffer[ i ].isRotateScale = false;
            IO::OamTop->oamBuffer[ i ].isHidden = true;
        }

        //Hide HP Bar
        //OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].isHidden = true;
        IO::displayHP( 100, 100, IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
                       IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y,
                       HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );
        IO::displayEP( 100, 100, IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
                       IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y, OWN1_EP_COL, OWN1_EP_COL + 1, false );

        if( !_battle->m_isWildBattle )
            setStsBallSts( p_opponent, p_pokemonPos, CUR_PKMN_STS_2( *_battle, p_pokemonPos, p_opponent ), false );
        IO::updateOAM( false );

        //Clear text
        u8 hpx = 0, hpy = 0;
        s16 x, y;
        getSpritePos( p_opponent, p_pokemonPos, _battle->m_battleMode == battle::DOUBLE, x, y, hpx, hpy );

        consoleSelect( &IO::Top );
        if( p_opponent == p_pokemonPos )
            undrawPkmnInfo2( hpx, hpy );
        else
            undrawPkmnInfo1( hpx, hpy );
    }

    void battleUI::sendPKMN( bool p_opponent, u8 p_pokemonPos, bool p_silent ) {
        if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
            return;
        if( _battle->m_isWildBattle && p_opponent )
            return;
        loadA( );

        s16 x = 0, y = 0;
        u8 hpx = 0, hpy = 0;
        getSpritePos( p_opponent, p_pokemonPos, _battle->m_battleMode == battle::DOUBLE, x, y, hpx, hpy );

        auto acPkmn = CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent );
        if( !acPkmn.m_stats.m_acHP )
            return;
        //Lets do some animation stuff here
        if( !p_silent ) {
            if( p_opponent )
                std::sprintf( buffer, "[TRAINER] ([TCLASS]) schickt\n%s in den Kampf![A]",
                              CUR_PKMN_2( *_battle, p_pokemonPos, OPPONENT ).m_boxdata.m_name );
            else
                std::sprintf( buffer, "Los [OWN%d]![A]", p_pokemonPos + 1 );
            _battle->log( buffer );

            setStsBallVisibility( p_opponent, p_pokemonPos, true, false );
            IO::updateOAM( false );

            animatePokeBall( x + 40 + 24 * ( !p_opponent ), y + 40 + 24 * ( !p_opponent ), PB_ANIM, 15, TILESTART );
        }
        //Load the PKMN sprite
        if( p_opponent ) {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                                     PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                                     false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                                         acPkmn.m_boxdata.m_speciesId, x, y,
                                         PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                                         acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
        } else {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/", acPkmn.m_boxdata.m_speciesId, x + 12, y + 12,
                                     PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                                     false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/",
                                         acPkmn.m_boxdata.m_speciesId, x + 12, y + 12,
                                         PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                                         acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
            adjustSprite( p_opponent, p_pokemonPos );
        }
        if( acPkmn.m_boxdata.isShiny( ) && !p_silent )
            animateShiny( x + 16 + 24 * ( !p_opponent ), y + 16 + 24 * ( !p_opponent ), SHINY_ANIM, 15, TILESTART );

        setStsBallPosition( p_opponent, p_pokemonPos, hpx + 8, hpy + 8, false );
        IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].isHidden = false;
        IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x = hpx;
        IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y = hpy;
        setStsBallVisibility( p_opponent, p_pokemonPos, false, false );
        IO::updateOAM( false );

        pokemonData p;
        getAll( acPkmn.m_boxdata.m_speciesId, p );

        consoleSelect( &IO::Top );
        if( p_opponent == p_pokemonPos )
            drawPkmnInfo2( hpx, hpy, acPkmn, HP_COL( p_opponent, p_pokemonPos ) );
        else
            drawPkmnInfo1( hpx, hpy, acPkmn, HP_COL( p_opponent, p_pokemonPos ) );

        IO::displayHP( 100, 101, hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );
        IO::displayHP( 100, 100 - acPkmn.m_stats.m_acHP * 100 / acPkmn.m_stats.m_maxHP,
                       hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );

        IO::displayEP( 0, ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ) * 100 /
                       ( EXP[ acPkmn.m_level ][ p.m_expType ] - EXP[ acPkmn.m_level - 1 ][ p.m_expType ] ),
                       hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, false );

    }

    void battleUI::evolvePKMN( bool p_opponent, u8 p_pokemonPos ) {
        if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
            return;
        s16 x = 0, y = 0;
        u8 hpx, hpy;
        getSpritePos( p_opponent, p_pokemonPos, _battle->m_battleMode == battle::DOUBLE, x, y, hpx, hpy );

        auto acPkmn = CUR_PKMN_2( *_battle, p_pokemonPos, p_opponent );
        if( p_opponent ) {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                                     PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                                     false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                                         acPkmn.m_boxdata.m_speciesId, x, y,
                                         PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                                         acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
        } else {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/", acPkmn.m_boxdata.m_speciesId, x + 12, y + 12,
                                     PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                                     false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/",
                                         acPkmn.m_boxdata.m_speciesId, x + 12, y + 12,
                                         PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                                         acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
            adjustSprite( p_opponent, p_pokemonPos );
        }
        updateHP( p_opponent, p_pokemonPos, 1, 1 );
    }

    void battleUI::learnMove( u8 p_pokemonPos, u16 p_move ) {
        if( _battle->m_battleMode != battle::DOUBLE && p_pokemonPos )
            return;
        if( !p_move )
            return;

        //Check if the PKMN already knows this move
        auto& acPkmn = CUR_PKMN_2( *_battle, p_pokemonPos, PLAYER );

        for( u8 i = 0; i < 4; ++i )
            if( acPkmn.m_boxdata.m_moves[ i ] == p_move )
                return;

        if( acPkmn.m_boxdata.m_moves[ 3 ] ) {
            std::sprintf( buffer, "%s kann nun\n%s erlernen![A][CLEAR]Aber %s kennt\nbereits 4 Attacken.[A]",
                          acPkmn.m_boxdata.m_name,
                          AttackList[ p_move ]->m_moveName.c_str( ),
                          acPkmn.m_boxdata.m_name );
            _battle->log( buffer );
            IO::yesNoBox yn;
ST:
            if( yn.getResult( "Soll eine Attacke\nvergessen werden?" ) ) {
                initLogScreen( );
                auto res = chooseAttack( p_pokemonPos );
                for( u8 u = 0; u < 50; ++u )
                    IO::Oam->oamBuffer[ u ].isHidden = true;
                IO::updateOAM( true );

                if( !res ) {
                    std::sprintf( buffer, "Aufgeben %s zu erlernen?", AttackList[ p_move ]->m_moveName.c_str( ) );
                    if( !yn.getResult( buffer ) ) {
                        for( u8 u = 0; u < 50; ++u )
                            IO::Oam->oamBuffer[ u ].isHidden = true;
                        IO::updateOAM( true );
                        initLogScreen( );
                        goto ST;
                    }
                } else {
                    initLogScreen( );
                    loadA( );
                    std::sprintf( buffer, "%s vergisst %s[A]\nund erlernt %s![A]",
                                  acPkmn.m_boxdata.m_name,
                                  AttackList[ res ]->m_moveName.c_str( ),
                                  AttackList[ p_move ]->m_moveName.c_str( ) );
                    _battle->log( buffer );

                    for( u8 i = 0; i < 4; ++i )
                        if( acPkmn.m_boxdata.m_moves[ i ] == res ) {
                            acPkmn.m_boxdata.m_moves[ i ] = p_move;
                            acPkmn.m_boxdata.m_acPP[ i ] = std::min( acPkmn.m_boxdata.m_acPP[ i ], AttackList[ p_move ]->m_movePP );
                        }
                }
            } else {
                initLogScreen( );
                loadA( );
                for( u8 u = 0; u < 50; ++u )
                    IO::Oam->oamBuffer[ u ].isHidden = true;
                IO::updateOAM( true );

                std::sprintf( buffer, "Aufgeben %s zu erlernen?", AttackList[ p_move ]->m_moveName.c_str( ) );
                if( !yn.getResult( buffer ) ) {
                    initLogScreen( );
                    for( u8 u = 0; u < 50; ++u )
                        IO::Oam->oamBuffer[ u ].isHidden = true;
                    IO::updateOAM( true );
                    goto ST;
                }
            }
        } else {
            for( u8 i = 0; i < 4; ++i ) {
                if( !acPkmn.m_boxdata.m_moves[ i ] ) {
                    acPkmn.m_boxdata.m_moves[ i ] = p_move;
                    acPkmn.m_boxdata.m_acPP[ i ] = std::min( acPkmn.m_boxdata.m_acPP[ i ], AttackList[ p_move ]->m_movePP );
                    std::sprintf( buffer, "%s erlernt %s![A]",
                                  acPkmn.m_boxdata.m_name,
                                  AttackList[ p_move ]->m_moveName.c_str( ) );
                    _battle->log( buffer );
                    break;
                }
            }
        }

        initLogScreen( );
        loadA( );
    }

    void battleUI::showEndScreen( ) {

        IO::initOAMTable( false );
        consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
        consoleSelect( &IO::Top );
        consoleClear( );

        sprintf( buffer, "%03d_1", _battle->_opponent->m_trainerClass );
        FS::readPictureData( bgGetGfxPtr( IO::bg2 ), "nitro:/PICS/BATTLE_MUG/", buffer, 128, 49152 );
    }

    void battleUI::capture( u16 p_pokeBall, u8 p_ticks ) {
        (void) p_pokeBall;

        IO::OamTop->oamBuffer[ 0 ].isHidden = true;
        IO::loadSprite( PB_ANIM, 15, PB_ANIM_TILES, 72, 100, 16, 16,
                        PokeBall1Pal, PokeBall1Tiles, PokeBall1TilesLen,
                        false, false, false, OBJPRIORITY_0, false );
        IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall1TilesLen, false );
        IO::updateOAM( false );

        u8 y;
        for( u8 x = 92; x <= 184; x += 2 ) {
            y = u8( .01 * x * x - 3 * x + 236 );
            IO::OamTop->oamBuffer[ PB_ANIM ].x = x;
            IO::OamTop->oamBuffer[ PB_ANIM ].y = y;
            u8 f = x - 92;
            if( f % 36 == 0 )
                IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall1TilesLen, false );
            if( f % 36 == 4 )
                IO::copySpriteData( PokeBall2Tiles, PB_ANIM_TILES, PokeBall2TilesLen, false );
            if( f % 36 == 8 )
                IO::copySpriteData( PokeBall3Tiles, PB_ANIM_TILES, PokeBall3TilesLen, false );
            if( f % 36 == 12 )
                IO::copySpriteData( PokeBall4Tiles, PB_ANIM_TILES, PokeBall4TilesLen, false );
            if( f % 36 == 16 )
                IO::copySpriteData( PokeBall5Tiles, PB_ANIM_TILES, PokeBall5TilesLen, false );
            if( f % 36 == 20 )
                IO::copySpriteData( PokeBall6Tiles, PB_ANIM_TILES, PokeBall6TilesLen, false );
            if( f % 36 == 24 )
                IO::copySpriteData( PokeBall7Tiles, PB_ANIM_TILES, PokeBall7TilesLen, false );
            if( f % 36 == 28 )
                IO::copySpriteData( PokeBall8Tiles, PB_ANIM_TILES, PokeBall8TilesLen, false );
            if( f % 36 == 32 )
                IO::copySpriteData( PokeBall9Tiles, PB_ANIM_TILES, PokeBall9TilesLen, false );
            IO::updateOAM( false );
            swiWaitForVBlank( );
            if( x % 4 == 2 )
                swiWaitForVBlank( );
        }
        IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );

        if( p_ticks == (u8) -1 ) {
            IO::OamTop->oamBuffer[ PB_ANIM ].isHidden = true;
            IO::updateOAM( false );
            _battle->log( "Der Ball wurde abgeblockt.\nSei kein Dieb![A]" );
            return;
        }
        for( int i = 0; i < 4; ++i )
            swiWaitForVBlank( );

        IO::copySpriteData( PokeBall11Tiles, PB_ANIM_TILES, PokeBall11TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 8; ++i )
            swiWaitForVBlank( );
        IO::updateOAM( false );

        for( u8 i = 0; i < 4; ++i )
            IO::OamTop->oamBuffer[ PKMN_IDX( 0, OPPONENT ) + i ].isHidden = true;
        IO::updateOAM( false );

        for( int i = 0; i < 3; ++i )
            swiWaitForVBlank( );

        IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );

        for( ; y <= 76; y += 2 ) {
            IO::OamTop->oamBuffer[ PB_ANIM ].y = y;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        for( ; y >= 64; y-- ) {
            IO::OamTop->oamBuffer[ PB_ANIM ].y = y;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        for( ; y <= 76; y++ ) {
            IO::OamTop->oamBuffer[ PB_ANIM ].y = y;
            IO::updateOAM( false );
            swiWaitForVBlank( );
        }
        for( int i = 0; i < 60; ++i )
            swiWaitForVBlank( );

        if( !p_ticks ) goto BREAK;

        IO::copySpriteData( PokeBall12Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall13Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall14Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall13Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall12Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 60; ++i )
            swiWaitForVBlank( );

        if( p_ticks == 1 ) goto BREAK;

        IO::copySpriteData( PokeBall15Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall16Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall17Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall16Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall15Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 60; ++i )
            swiWaitForVBlank( );

        if( p_ticks == 2 ) goto BREAK;
        IO::copySpriteData( PokeBall12Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall13Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall14Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall13Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall12Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 5; ++i )
            swiWaitForVBlank( );
        IO::copySpriteData( PokeBall1Tiles, PB_ANIM_TILES, PokeBall12TilesLen, false );
        IO::updateOAM( false );
        for( int i = 0; i < 50; ++i )
            swiWaitForVBlank( );

        if( p_ticks == 3 ) goto BREAK;

        for( int i = 0; i < 20; ++i )
            swiWaitForVBlank( );

        std::sprintf( buffer, "Toll!\n%s wurde gefangen![A]", _battle->_wildPokemon.m_pokemon->m_boxdata.m_name );
        _battle->log( buffer );
        return;
BREAK:
        IO::copySpriteData( PokeBall11Tiles, PB_ANIM_TILES, PokeBall11TilesLen, false );
        IO::updateOAM( false );
        for( u8 i = 0; i < 4; ++i )
            IO::OamTop->oamBuffer[ PKMN_IDX( 0, OPPONENT ) + i ].isHidden = false;
        IO::OamTop->oamBuffer[ PB_ANIM ].isHidden = true;
        IO::updateOAM( false );
        if( p_ticks == 0 ) _battle->log( "Mist!\nEs hat sich befreit…[A]" );
        else if( p_ticks == 1 ) _battle->log( "Oh.\nFast hätte es geklappt…[A]" );
        else if( p_ticks == 2 ) _battle->log( "Mist!\nDas war knapp…[A]" );
        else if( p_ticks == 3 ) _battle->log( "Verflixt!\nEs war doch fast gefangen…[A]" );
        return;
    }

    void battleUI::handleCapture( ) {
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::NAV->draw( );
        UPDATE_TIME = true;
        DRAW_TIME = true;

        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( IO::OamTop );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        bgUpdate( );

        sprintf( buffer, "%d", _battle->m_backgroundId );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/BATTLE_BACK/", buffer, 512, 49152 );

        auto& acPkmn = *_battle->_wildPokemon.m_pokemon;
        u16 x = 80;
        u8  y = 48;

        if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                                 PKMN_IDX( 0, OPPONENT ), PKMN_PAL_IDX( 0, OPPONENT ), PKMN_TILE_IDX( 0, OPPONENT ),
                                 false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                                     acPkmn.m_boxdata.m_speciesId, x, y,
                                     PKMN_IDX( 0, OPPONENT ), PKMN_PAL_IDX( 0, OPPONENT ), PKMN_TILE_IDX( 0, OPPONENT ), false,
                                     acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                _battle->log( "Sprite failed!\n(That's a bad thing, btw.)[A]" );
            }
        }
        IO::updateOAM( false );

        IO::NAV->draw( true );
        initLogScreen( );

        IO::yesNoBox yn;
        sprintf( buffer, "Möchtest du dem %s\neinen Spitznamen geben?", acPkmn.m_boxdata.m_name );
        if( yn.getResult( buffer ) ) {
            IO::keyboard kbd;
            auto nick = kbd.getText( 10, "Wähle einen Spitznamen!" );
            if( strcmp( nick.c_str( ), acPkmn.m_boxdata.m_name )
                && strcmp( "", nick.c_str( ) ) ) {
                strcpy( acPkmn.m_boxdata.m_name, nick.c_str( ) );
                acPkmn.m_boxdata.m_individualValues.m_isNicked = true;
            }
        }
        IO::NAV->draw( );
        initLogScreen( );
    }

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE_UI
    //////////////////////////////////////////////////////////////////////////
}
