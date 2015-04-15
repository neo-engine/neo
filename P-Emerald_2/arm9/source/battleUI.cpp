/*
Pokémon Emerald 2 Version
------------------------------

file        : battleUI.cpp
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


#include <cwchar>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <tuple>
#include <initializer_list>

#include "battle.h"
#include "battleUI.h"
#include "defines.h"
#include "pokemon.h"
#include "move.h"
#include "item.h"
#include "yesNoBox.h"
#include "saveGame.h"
#include "buffer.h"
#include "fs.h"
#include "sprite.h"
#include "uio.h"

#include "Back.h"
#include "A.h"
#include "map2d.h"

#include "Battle1.h"
#include "Battle2.h"

#include "BattleSub1.h"
#include "BattleSub2.h"
#include "BattleSub3.h"
#include "BattleSub4.h"
#include "BattleSub5.h"
#include "BattleSub6.h"

#include "Choice_1.h"
#include "Choice_2.h"
#include "Choice_3.h"
#include "Choice_4.h"
#include "Choice_5.h"

#include "BattleBall1.h" //Normal
#include "BattleBall2.h" //Statused
#include "BattleBall3.h" //Fainted
#include "BattleBall4.h" //NA

//Test Trainer mugs:
#include "mug_001_1.h"
#include "mug_001_2.h"
#include "TestBattleBack.h"
#include "Border.h"


#include "memo.h"
#include "atks.h"
#include "Up.h"
#include "Down.h"

#include "BattlePkmnChoice1.h"
#include "BattlePkmnChoice2.h"
#include "BattlePkmnChoice3.h"
#include "BattlePkmnChoice4.h"

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

    extern char* trainerclassnames[ ];

    void initColors( ) {

        IO::regularFont->setColor( 0, 0 );
        IO::regularFont->setColor( BLACK_IDX, 1 );
        IO::regularFont->setColor( GRAY_IDX, 2 );
        IO::boldFont->setColor( 0, 0 );
        IO::boldFont->setColor( GRAY_IDX, 1 );
        IO::boldFont->setColor( WHITE_IDX, 2 );

        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = STEEL;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
        BG_PALETTE_SUB[ BLUE_IDX ] = BLUE;
    }
    void battleUI::initLogScreen( ) {
        initColors( );
        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = STEEL;
        BG_PALETTE[ BLACK_IDX ] = BLACK;
        BG_PALETTE[ RED_IDX ] = RED;
        BG_PALETTE[ BLUE_IDX ] = BLUE;
        IO::printRectangle( (u8)0, (u8)0, (u8)255, (u8)63, true, false, WHITE_IDX );
    }
    void battleUI::clearLogScreen( ) {
        IO::printRectangle( (u8)0, (u8)0, (u8)255, (u8)63, true, false, WHITE_IDX );
    }
    void battleUI::setLogTextColor( u16 p_color ) {
        BG_PALETTE_SUB[ COLOR_IDX ] = BG_PALETTE[ COLOR_IDX ] = p_color;
    }
    void battleUI::writeLogText( const std::wstring& p_message ) {
        IO::regularFont->printMBString( p_message.c_str( ), 8, 8, true );
    }

#define PB_PAL( i ) ( ( ( i ) == 0 ) ? BattleBall1Pal : ( ( ( i ) == 1 ) ? BattleBall2Pal : ( ( ( i ) == 2 ) ? BattleBall3Pal : BattleBall4Pal ) ) )
#define PB_TILES( i ) ( ( ( i ) == 0 ) ? BattleBall1Tiles : ( ( ( i ) == 1 ) ? BattleBall2Tiles : ( ( ( i ) == 2 ) ? BattleBall3Tiles : BattleBall4Tiles ) ) )
#define PB_TILES_LEN( i ) BattleBall1TilesLen

    // TOP SCREEN DEFINES

    //Some defines of indices in the OAM for the used sprites
#define HP_START                 1
#define HP_IDX( p_pokemonPos, p_opponent ) ( HP_START + ( ( p_opponent ) * 2 + ( p_pokemonPos ) ) )

#define STSBALL_START            5
#define STSBALL_IDX( p_pokemonPos, p_opponent ) ( STSBALL_START + ( ( p_opponent ) * 6 + ( p_pokemonPos ) ) )

#define PKMN_START               17
#define PKMN_IDX( p_pokemonPos, p_opponent ) ( PKMN_START + 4 * ( ( p_opponent ) * 2 + ( p_pokemonPos ) ) )

#define PB_ANIM             127
#define SHINY_ANIM          127

    //Some analogous defines for their pal indices
#define PKMN_PAL_START          0
#define PKMN_PAL_IDX( p_pokemonPos, p_opponent ) ( PKMN_PAL_START + ( ( p_opponent ) * 2 + ( p_pokemonPos ) ) )

#define PB_PAL_TOP(i)         ( (i) + 4 )
#define HP_PAL                8

#define PKMN_TILE_START       0
#define PKMN_TILE_IDX( p_pokemonPos, p_opponent ) ( PKMN_TILE_START + 144 * ( ( p_opponent ) * 2 + ( p_pokemonPos ) ) )
    u16 TILESTART = ( PKMN_TILE_START + 4 * 144 );

#define OWN1_EP_COL         160
#define OWN2_EP_COL         OWN1_EP_COL

#define OWN_HP_COL          150
#define OPP_HP_COL          155

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

    u16 initStsBalls( bool p_bottom, battle* p_battle, u16& p_tilecnt ) {
        //Own PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = ACPKMNSTS2( *p_battle, i, PLAYER );
            p_tilecnt = IO::loadSprite( p_bottom ? i : ( STSBALL_START + i ),
                                        p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, p_bottom ? ( 16 * i ) : 240 - ( 16 * i ),
                                        180, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                        false, false, false, OBJPRIORITY_0, p_bottom );
        }
        //Opps PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = ACPKMNSTS2( *p_battle, i, OPPONENT );
            p_tilecnt = IO::loadSprite( p_bottom ? ( 6 + i ) : ( STSBALL_START + 6 + i ),
                                        p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, !p_bottom ? ( 16 * i ) : 240 - ( 16 * i ),
                                        -4, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                        false, false, false, OBJPRIORITY_0, p_bottom );
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
    void setStsBallSts( bool p_opponent, u8 p_pokemonPos, battle::acStatus p_status, bool p_bottom ) {
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

    void loadSpritesTop( battle* p_battle ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        IO::bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        IO::bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( IO::bg3, 3 );
        bgSetPriority( IO::bg2, 2 );
        IO::initOAMTable( IO::OamTop );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( IO::bg3 ), 256 * 192 );
        bgUpdate( );


        dmaCopy( TestBattleBackBitmap, bgGetGfxPtr( IO::bg3 ), 256 * 256 );
        dmaCopy( TestBattleBackPal, BG_PALETTE, 128 * 2 );
        dmaCopy( mug_001_1Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaCopy( mug_001_1Pal, BG_PALETTE, 64 );
        for( u8 i = 0; i < 40; ++i )
            swiWaitForVBlank( );
        dmaCopy( mug_001_2Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaCopy( mug_001_2Pal, BG_PALETTE, 64 );
        for( u8 i = 0; i < 120; ++i )
            swiWaitForVBlank( );

        dmaFillWords( 0, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
    }

    void loadSpritesSub( battle* p_battle ) {
        IO::initOAMTable( true );
        IO::drawSub( );

        u16 tilecnt = 0;
        tilecnt = initStsBalls( true, p_battle, tilecnt );
        initColors( );

        sprintf( buffer, "Eine Herausforderung von\n%s %s!",
                 trainerclassnames[ p_battle->_opponent.m_trainerClass ],
                 p_battle->_opponent.m_battleTrainerName.c_str( ) );
        IO::regularFont->printString( buffer, 16, 80, true );
        IO::updateOAM( true );
    }

    void loadBattleUITop( battle* p_battle ) {
        IO::initOAMTable( false );

        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );

        TILESTART = initStsBalls( false, p_battle, TILESTART = ( PKMN_TILE_START + 4 * 144 ) );

        for( u8 i = 0; i < 4; ++i ) {
            TILESTART = IO::loadSprite( HP_IDX( i % 2, ( i / 2 ) ), HP_PAL,
                                        TILESTART, 0, 0, 32, 32, Battle1Pal,
                                        Battle1Tiles, Battle1TilesLen, false,
                                        false, true, OBJPRIORITY_2, false );
        }

        IO::updateOAM( false );
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

    /**
    *  @brief Loads all the required graphics and sprites
    */
    void battleUI::init( ) {
        //Copy the current PKMNStatus
        for( u8 i = 0; i < 6; ++i )
            for( u8 p = 0; p < 2; ++p )
                for( u8 s = 0; s < MAX_STATS; ++s )
                    _oldPKMNStats[ ACPOS2( *_battle, i, p ) ][ p ][ s ] = ACPKMNSTS2( *_battle, i, p );
        IO::initOAMTable( true );
        IO::initOAMTable( false );
        IO::Oam->oamBuffer[ SUB_A_OAM ].gfxIndex = 0;
        loadA( );
    }

    void battleUI::trainerIntro( ) {
        loadSpritesSub( _battle );
        loadSpritesTop( _battle ); // This should consume some time

        loadBattleUITop( _battle );
        IO::initOAMTable( true );
        IO::drawSub( );
        setBattleUISubVisibility( );
        initLogScreen( );
    }

    void setDeclareBattleMoveSpriteVisibility( bool p_showBack, bool p_isHidden = true ) {
        if( p_showBack )
            IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = p_isHidden;
        setBattleUISubVisibility( p_isHidden );
    }

#define p_time true
#define p_timeParameter false
    u8 firstMoveSwitchTarget = 0;
    bool battleUI::declareBattleMove( u8 p_pokemonPos, bool p_showBack ) {
        wchar_t wbuffer[ 100 ];
        swprintf( wbuffer, 50, L"Was soll %ls tun?", ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_name );
        writeLogText( wbuffer );

        loadBattleUISub( ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                         _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
        if( p_showBack ) {
            IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
            IO::updateOAM( true );
        }

        touchPosition touch;
        auto& result = _battle->_battleMoves[ p_pokemonPos ][ PLAYER ];
        loop( ) {

            scanKeys( );
            touch = touchReadXY( );

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
                    apCnt += ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_acPP[ i ];

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
                loadBattleUISub( ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( wbuffer );

            } else if( GET_AND_WAIT_R( 0, 162, 58, 300 ) ) { //Bag

                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::USE_ITEM;
                result.m_value = chooseItem( p_pokemonPos );
                if( result.m_value ) {
                    loadA( );
                    return true;
                }
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( wbuffer );

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
                writeLogText( wbuffer );

            } else if( _battle->m_isWildBattle && GET_AND_WAIT_R( 97, 162, 153, 180 ) ) { //Run
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::RUN;
                if( _battle->run( ) ) {
                    _battle->log( L"Du entkommst...[A]" );
                    return true;
                } else
                    _battle->log( L"Flucht gescheitert![A]" );
                if( result.m_value ) {
                    loadA( );
                    return true;
                }
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( wbuffer );

            } else if( GET_AND_WAIT_R( 195, 148, 238, 176 ) ) { // Switch Pkmn
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::SWITCH;
                result.m_value = choosePKMN( p_pokemonPos + ( _battle->m_battleMode == battle::DOUBLE ) );
                if( result.m_value ) {
                    loadA( );
                    return true;
                }
                loadBattleUISub( ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && FS::SAV->m_activatedPNav );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( wbuffer );
            }
        }
    }

    u16 battleUI::chooseAttack( u8 p_pokemonPos ) {
        u16 result = 0;

        writeLogText( L"Welche Attacke?" );

        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;

        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &IO::Bottom );

        auto acPkmn = ACPKMN2( *_battle, p_pokemonPos, PLAYER );

        for( u8 i = 0; i < 4; ++i ) {
            if( acPkmn.m_boxdata.m_moves[ i ] ) {
                auto acMove = AttackList[ acPkmn.m_boxdata.m_moves[ i ] ];

                BG_PALETTE_SUB[ 240 + i ] = IO::getColor( acMove->m_moveType );

                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

                IO::printRectangle( x + 1, y + 1, x + w + 2, y + h + 1,
                                    true, false, BLACK_IDX );
                IO::printRectangle( x, y, x + w, y + h,
                                    true, false, 240 + i );
                IO::printRectangle( x + 7, y + 5, x + w - 4, y + h - 1,
                                    true, false, BLACK_IDX );
                IO::printRectangle( x + 6, y + 4, x + w - 6, y + h - 2,
                                    true, false, WHITE_IDX );

                IO::regularFont->printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                tilecnt = IO::loadTypeIcon( acMove->m_moveType, x - 7, y - 7, ++oamIndex, palIndex++, tilecnt, true );
                tilecnt = IO::loadDamageCategoryIcon( acMove->m_moveHitType, x + 25, y - 7, ++oamIndex, palIndex++, tilecnt, true );
                consoleSelect( &IO::Bottom );
                consoleSetWindow( &IO::Bottom, x / 8, 12 + ( i / 2 ) * 6, 20, 2 );
                printf( "%6hhu/%2hhu AP",
                        acPkmn.m_boxdata.m_acPP[ i ],
                        AttackList[ acPkmn.m_boxdata.m_moves[ i ] ]->m_movePP * ( ( 5 + acPkmn.m_boxdata.m_ppup.m_Up1 ) / 5 ) );
            }
        }

        IO::updateOAM( true );

        touchPosition touch;
        loop( ) {
NEXT:

            scanKeys( );
            touch = touchReadXY( );

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
                if( touch.px >= x && touch.py >= y && touch.px <= x + w && touch.py <= y + h
                    && acPkmn.m_boxdata.m_acPP[ i ] ) {
                    auto acMove = AttackList[ acPkmn.m_boxdata.m_moves[ i ] ];

                    IO::printRectangle( x, y, x + w, y + h,
                                        true, false, 0 );
                    IO::printRectangle( x + 1, y + 1, x + w + 2, y + h + 1,
                                        true, false, 240 + i );
                    IO::printRectangle( x + 8, y + 6, x + w - 2, y + h,
                                        true, false, WHITE_IDX );

                    IO::regularFont->printString( acMove->m_moveName.c_str( ), x + 9, y + 9, true );

                    loop( ) {
                        swiWaitForVBlank( );

                        scanKeys( );
                        auto touch = touchReadXY( );
                        if( touch.px == 0 && touch.py == 0 )
                            break;
                        if( !( touch.px >= x && touch.py >= y && touch.px <= x + w && touch.py <= y + h ) ) {
                            IO::printRectangle( x + 1, y + 1, x + w + 2, y + h + 1,
                                                true, false, BLACK_IDX );
                            IO::printRectangle( x, y, x + w, y + h,
                                                true, false, 240 + i );
                            IO::printRectangle( x + 7, y + 5, x + w - 4, y + h - 1,
                                                true, false, BLACK_IDX );
                            IO::printRectangle( x + 6, y + 4, x + w - 6, y + h - 2,
                                                true, false, WHITE_IDX );

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
        IO::drawSub( );
        initColors( );
        clearLogScreen( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( true );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        return result;
    }

    u8 battleUI::chooseAttackTarget( u8 p_pokemonPos, u16 p_move ) {
        u8 result = 0;

        writeLogText( L"Welches PKMN angreifen?" );

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

        if( selected[ 2 ] && selected[ 3 ] )
            IO::printRectangle( 112 + 1, 130 + 1, 112 + 16 + 2, 146 + 1, true, false, BLACK_IDX );
        if( selected[ 0 ] && selected[ 1 ] )
            IO::printRectangle( 120 + 1, 82 + 1, 120 + 16 + 2, 98 + 1, true, false, BLACK_IDX );

        if( selected[ 1 ] && selected[ 2 ] )
            IO::printRectangle( 56 + 1, 106 + 1, 56 + 16 + 2, 122 + 1, true, false, BLACK_IDX );
        if( selected[ 3 ] && selected[ 0 ] )
            IO::printRectangle( 176 + 1, 106 + 1, 176 + 16 + 2, 122 + 1, true, false, BLACK_IDX );

        _battle->_battleMoves[ p_pokemonPos ][ PLAYER ].m_target = 1 | 2 | 4 | 8;

        for( u8 i = 0; i < 4; ++i ) {
            u8 aI = i % 2;
            if( 1 - ( i / 2 ) )
                aI = ( 1 - aI );

            auto acPkmn = ACPKMN2( *_battle, aI, 1 - ( i / 2 ) );

            u8 w = 104, h = 32;
            u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

            IO::printRectangle( x + 1, y + 1, x + w + 2, y + h + 1,
                                true, false, BLACK_IDX );
            IO::printRectangle( x, y, x + w, y + h,
                                true, false, selected[ i ] ? RED_IDX : GRAY_IDX );
            IO::printRectangle( x + 7, y + 5, x + w - 4, y + h - 1,
                                true, false, BLACK_IDX );
            IO::printRectangle( x + 6, y + 4, x + w - 6, y + h - 2,
                                true, false, WHITE_IDX );

            if( neverTarget[ i ] )
                continue;

            if( acPkmn.m_stats.m_acHP ) {
                IO::regularFont->printString( acPkmn.m_boxdata.m_name, x + 7, y + 7, true );
                tilecnt = IO::loadPKMNIcon( acPkmn.m_boxdata.m_speciesId,
                                            x - 10, y - 23, ++oamIndex, palIndex++, tilecnt );
            }
        }

        if( selected[ 2 ] && selected[ 3 ] )
            IO::printRectangle( 112, 130, 112 + 16, 146, true, false, RED_IDX );
        if( selected[ 0 ] && selected[ 1 ] )
            IO::printRectangle( 120, 82, 120 + 16, 98, true, false, RED_IDX );

        if( selected[ 1 ] && selected[ 2 ] )
            IO::printRectangle( 56, 106, 56 + 16, 122, true, false, RED_IDX );
        if( selected[ 3 ] && selected[ 0 ] )
            IO::printRectangle( 176, 106, 176 + 16, 122, true, false, RED_IDX );

        IO::updateOAM( true );

        touchPosition touch;
        loop( ) {
NEXT:

            scanKeys( );
            touch = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) ) { //Back
                result = 0;
                break;
            }

            for( u8 i = 0; i < 4; ++i ) {
                u8 aI = i % 2;
                if( 1 - ( i / 2 ) )
                    aI = ( 1 - aI );

                auto acPkmn = ACPKMN2( *_battle, aI, 1 - ( i / 2 ) );
                if( neverTarget[ i ] || !acPkmn.m_stats.m_acHP )
                    continue;
                if( !selected[ i ] && selectionExists )
                    continue;

                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );
                if( touch.px >= x && touch.py >= y && touch.px <= x + w && touch.py <= y + h ) {

                    for( u8 j = 0; j < 4; ++j ) {
                        if( !selected[ j ] && j != i )
                            continue;

                        u8 aJ = j % 2;
                        if( 1 - ( j / 2 ) )
                            aJ = ( 1 - aJ );

                        auto acPkmnJ = ACPKMN2( *_battle, aJ, 1 - ( i / 2 ) );

                        u8 nx = 16 - 8 * ( j / 2 ) + ( w + 16 ) * ( j % 2 ), ny = 74 + ( h + 16 ) * ( j / 2 );
                        IO::printRectangle( nx, ny, x + w, y + h,
                                            true, false, 0 );
                        IO::printRectangle( nx + 1, ny + 1, nx + w + 2, ny + h + 1,
                                            true, false, RED_IDX );
                        IO::printRectangle( nx + 8, ny + 6, nx + w - 2, ny + h,
                                            true, false, WHITE_IDX );
                        if( neverTarget[ j ] || !acPkmnJ.m_stats.m_acHP )
                            continue;
                        IO::regularFont->printString( acPkmnJ.m_boxdata.m_name, nx + 9, ny + 9, true );
                    }

                    loop( ) {
                        swiWaitForVBlank( );

                        scanKeys( );
                        auto touch = touchReadXY( );
                        if( touch.px == 0 && touch.py == 0 )
                            break;
                        if( !( touch.px >= x && touch.py >= y && touch.px <= x + w && touch.py <= y + h ) ) {
                            for( u8 j = 0; j < 4; ++j ) {
                                if( !selected[ j ] && j != i )
                                    continue;

                                u8 aJ = j % 2;
                                if( 1 - ( j / 2 ) )
                                    aJ = ( 1 - aJ );

                                auto acPkmnJ = ACPKMN2( *_battle, aJ, 1 - ( i / 2 ) );

                                u8 nx = 16 - 8 * ( j / 2 ) + ( w + 16 ) * ( j % 2 ), ny = 74 + ( h + 16 ) * ( j / 2 );

                                IO::printRectangle( nx + 1, ny + 1, nx + w + 2, ny + h + 1,
                                                    true, false, BLACK_IDX );
                                IO::printRectangle( nx, ny, nx + w, ny + h,
                                                    true, false, selected[ i ] ? RED_IDX : GRAY_IDX );
                                IO::printRectangle( nx + 7, ny + 5, nx + w - 4, ny + h - 1,
                                                    true, false, BLACK_IDX );
                                IO::printRectangle( nx + 6, ny + 4, nx + w - 6, ny + h - 2,
                                                    true, false, WHITE_IDX );
                                if( neverTarget[ j ] || !acPkmnJ.m_stats.m_acHP )
                                    continue;
                                IO::regularFont->printString( acPkmnJ.m_boxdata.m_name, nx + 7, ny + 7, true );
                            }
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
        IO::drawSub( );
        initColors( );
        clearLogScreen( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( true );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );

        return result;
    }

    u16 battleUI::chooseItem( u8 p_pokemonPos ) {
        u8 result = 0;

        // Make this a debug battle end

        _battle->_round = 0;
        _battle->_maxRounds = -1;


        touchPosition touch;
        loop( ) {

            scanKeys( );
            touch = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) ) { //Back
                result = 0;
                break;
            }
        }

        clearLogScreen( );
        return result;
    }

    void drawPKMNChoiceScreen( battle* p_battle, bool p_firstIsChosen ) {
        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = IO::loadSprite( SUB_Back_OAM, 0, tilecnt,
                                  SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                  BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &IO::Bottom );

        if( p_battle->m_battleMode == battle::DOUBLE && p_battle->_battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::SWITCH )
            firstMoveSwitchTarget = p_battle->_battleMoves[ 0 ][ PLAYER ].m_value;

        for( u8 i = 0; i < 6; ++i ) {
            u8 x = 8 + ( i % 2 ) * 120 - ( i / 2 ) * 4,
                y = 32 + ( i / 2 ) * 48;

            if( !i || ( p_firstIsChosen && ( i == 1 ) ) || i == firstMoveSwitchTarget ) {
                tilecnt = IO::loadSprite( SUB_CHOICE_START + 2 * i, 1, tilecnt,
                                          x, y, 64, 64, BattlePkmnChoice1Pal, BattlePkmnChoice1Tiles,
                                          BattlePkmnChoice1TilesLen, false, false, false, OBJPRIORITY_2, true );
                tilecnt = IO::loadSprite( SUB_CHOICE_START + 2 * i + 1, 1, tilecnt,
                                          x + 64, y, 64, 64, BattlePkmnChoice2Pal, BattlePkmnChoice2Tiles,
                                          BattlePkmnChoice2TilesLen, false, false, false, OBJPRIORITY_2, true );
            } else {
                tilecnt = IO::loadSprite( SUB_CHOICE_START + 2 * i, 2, tilecnt,
                                          x, y, 64, 64, BattlePkmnChoice3Pal, BattlePkmnChoice3Tiles,
                                          BattlePkmnChoice1TilesLen, false, false, false, OBJPRIORITY_2, true );
                tilecnt = IO::loadSprite( SUB_CHOICE_START + 2 * i + 1, 2, tilecnt,
                                          x + 64, y, 64, 64, BattlePkmnChoice4Pal, BattlePkmnChoice4Tiles,
                                          BattlePkmnChoice4TilesLen, false, false, false, OBJPRIORITY_2, true );
            }

            if( i >= p_battle->_player.m_pkmnTeam.size( ) )
                continue;

            auto& acPkmn = ACPKMN2( *p_battle, i, PLAYER );

            consoleSetWindow( &IO::Bottom, ( x + 6 ) / 8, ( y + 6 ) / 8, 20, 8 );
            if( !acPkmn.m_boxdata.m_individualValues.m_isEgg ) {
                printf( "       Lv.%3d", acPkmn.m_Level );
                printf( "\n%14ls\n", acPkmn.m_boxdata.m_name );
                printf( "%14s\n\n",
                        ItemList[ acPkmn.m_boxdata.m_holdItem ]->getDisplayName( ).c_str( ) );
                printf( "   %3i/%3i",
                        acPkmn.m_stats.m_acHP,
                        acPkmn.m_stats.m_maxHP );
                tilecnt = IO::loadPKMNIcon( acPkmn.m_boxdata.m_speciesId,
                                            x + 4,
                                            y - 12,
                                            ++oamIndex,
                                            palIndex++,
                                            tilecnt,
                                            true );
            } else {
                printf( "\n            Ei" );
                tilecnt = IO::loadEggIcon( x + 4,
                                           y - 12,
                                           ++oamIndex,
                                           palIndex++,
                                           tilecnt,
                                           true );
            }
        }

        IO::updateOAM( true );
    }

    void undrawPKMNChoiceScreen( ) {
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            IO::Oam->oamBuffer[ i ].isHidden = true;
        IO::updateOAM( true );
    }

    /**
    *  @returns 0 if the Pokemon shall be sent, 1 if further information was requested, 2 if the moves should be displayed, 3 if the previous screen shall be shown
    */
    u8 showConfirmation( pokemon& p_pokemon, bool p_alreadySent, bool p_alreadyChosen ) {
        IO::drawSub( );
        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
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
        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            if( !( t2 = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                32, 32, ++oamIndex, ++palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale ) ) )
                t2 = IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                32, 32, oamIndex, palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale );
            oamIndex += 3;
            tilecnt = t2;

            consoleSetWindow( &IO::Bottom, 13, 7, 20, 8 );
            if( !p_alreadySent && !p_alreadyChosen )
                printf( "   AUSSENDEN" );
            else if( !p_alreadyChosen )
                printf( "Bereits im Kampf" );
            else if( dead )
                printf( "Schon besiegt..." );
            else
                printf( "Schon ausgew\x84""hlt" );
            printf( "\n----------------\n%11ls %c\n%11s\n\nLv.%3i %3i/%3iKP",
                    p_pokemon.m_boxdata.m_name,
                    GENDER( p_pokemon ),
                    ItemList[ p_pokemon.m_boxdata.m_holdItem ]->getDisplayName( ).c_str( ),
                    p_pokemon.m_Level,
                    p_pokemon.m_stats.m_acHP,
                    p_pokemon.m_stats.m_maxHP );
        } else {
            consoleSetWindow( &IO::Bottom, 8, 11, 16, 10 );
            printf( "  Ein Ei kann\n nicht k\x84""mpfen!" );
            x = 64;
            y = 64;
        }
        //Switch
        tilecnt = IO::loadSprite( ++oamIndex, 2, tilecnt,
                                  x, y, 64, 64, Choice_4Pal, Choice_4Tiles,
                                  Choice_4TilesLen, false, false, false, OBJPRIORITY_2, true );
        tilecnt = IO::loadSprite( ++oamIndex, 2, tilecnt,
                                  x + 64, y, 64, 64, Choice_4Pal, Choice_5Tiles,
                                  Choice_5TilesLen, false, false, false, OBJPRIORITY_2, true );

        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            //Status
            tilecnt = IO::loadSprite( ++oamIndex, 2, tilecnt,
                                      28, 128, 64, 32, Choice_1Pal, Choice_1Tiles,
                                      Choice_1TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = IO::loadSprite( ++oamIndex, 2, tilecnt,
                                      60, 128, 64, 32, Choice_3Pal, Choice_3Tiles,
                                      Choice_3TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = IO::loadSprite( ++oamIndex, 3, tilecnt,
                                      20, 128, 32, 32, memoPal, memoTiles,
                                      memoTilesLen, false, false, false, OBJPRIORITY_1, true );
            consoleSetWindow( &IO::Bottom, 7, 17, 20, 8 );
            printf( "BERICHT" );

            //Moves
            tilecnt = IO::loadSprite( ++oamIndex, 2, tilecnt,
                                      132, 128, 64, 32, Choice_1Pal, Choice_1Tiles,
                                      Choice_1TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = IO::loadSprite( ++oamIndex, 2, tilecnt,
                                      164, 128, 64, 32, Choice_3Pal, Choice_3Tiles,
                                      Choice_3TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = IO::loadSprite( ++oamIndex, 4, tilecnt,
                                      200, 128, 32, 32, atksPal, atksTiles,
                                      atksTilesLen, false, false, false, OBJPRIORITY_1, true );
            consoleSetWindow( &IO::Bottom, 17, 17, 20, 8 );
            printf( "ATTACKEN" );
        }
        IO::updateOAM( true );

        touchPosition touch;
        loop( ) {

            scanKeys( );
            touch = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( GET_AND_WAIT_R( 224, 164, 300, 300 ) )  //Back
                return 3;
            if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
                if( !p_alreadySent && !p_alreadyChosen && !dead
                    && GET_AND_WAIT_R( x, y, x + 128, y + 64 ) )  //Send
                    return 0;
                if( GET_AND_WAIT_R( 20, 128, 124, 160 ) )  //Info
                    return 1;
                if( GET_AND_WAIT_R( 132, 128, 232, 160 ) )  //Moves
                    return 2;
            }
        }
    }

    /**
    *  @param p_page: 1 show moves, 0 show status
    *  @returns 0: return to prvious screen, 1 view next pokémon, 2 view previous pokémon, 3 switch screen
    */
    u8 showDetailedInformation( pokemon& p_pokemon, u8 p_page ) {
        IO::drawSub( );
        initColors( );
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
        u16 tilecnt = 0;
        u8  palIndex = 4;
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
                                         16, 8, oamIndex, palIndex, IO::Oam->oamBuffer[ oamIndex ].gfxIndex,
                                         true, p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale );
            }
            oamIndex += 4;
            tilecnt = t2;

            consoleSetWindow( &IO::Bottom, 4, 0, 12, 2 );
            printf( "EP(%3lu%%)\nKP(%3i%%)", ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_Level - 1 ][ exptype ] )
                    * 100 / ( EXP[ p_pokemon.m_Level ][ exptype ] - EXP[ p_pokemon.m_Level - 1 ][ exptype ] ),
                    p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP );
            IO::displayHP( 100, 101, 46, 40, 245, 246, false, 50, 56, true );
            IO::displayHP( 100, 100 - p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP, 46, 40, 245, 246, false, 50, 56, true );

            IO::displayEP( 100, 101, 46, 40, 247, 248, false, 59, 62, true );
            IO::displayEP( 0, ( p_pokemon.m_boxdata.m_experienceGained - EXP[ p_pokemon.m_Level - 1 ][ exptype ] )
                           * 100 / ( EXP[ p_pokemon.m_Level ][ exptype ] - EXP[ p_pokemon.m_Level - 1 ][ exptype ] ),
                           46, 40, 247, 248, false, 59, 62, true );
            IO::regularFont->setColor( WHITE_IDX, 1 );

            consoleSetWindow( &IO::Bottom, 2, 1, 13, 2 );

            std::swprintf( wbuffer, 20, L"%ls /", p_pokemon.m_boxdata.m_name );
            IO::regularFont->printString( wbuffer, 16, 96, true );
            s8 G = p_pokemon.m_boxdata.gender( );

            if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32 ) {
                if( G == 1 ) {
                    IO::regularFont->setColor( BLUE_IDX, 1 );
                    IO::regularFont->printChar( 136, 100, 102, true );
                } else {
                    IO::regularFont->setColor( RED_IDX, 1 );
                    IO::regularFont->printChar( 137, 100, 102, true );
                }
            }
            IO::regularFont->setColor( WHITE_IDX, 1 );

            IO::regularFont->printString( getDisplayName( p_pokemon.m_boxdata.m_speciesId ), 24, 110, true );

            if( p_pokemon.m_boxdata.getItem( ) ) {
                IO::regularFont->printString( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( true ).c_str( ),
                                              24, 124, true );
                tilecnt = IO::loadItemIcon( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->m_itemName, 0, 116, ++oamIndex, ++palIndex, tilecnt, true );
            } else {
                IO::regularFont->setColor( BLACK_IDX, 1 );
                IO::regularFont->setColor( GRAY_IDX, 2 );
                IO::regularFont->printString( ItemList[ p_pokemon.m_boxdata.getItem( ) ]->getDisplayName( ).c_str( ), 24, 124, true );
            }
            IO::regularFont->setColor( GRAY_IDX, 1 );
            IO::regularFont->setColor( BLACK_IDX, 2 );

            if( data.m_types[ 0 ] == data.m_types[ 1 ] ) {
                tilecnt = IO::loadTypeIcon( data.m_types[ 0 ], 224, 0, ++oamIndex, ++palIndex, tilecnt, true );
                oamIndex++;
                ++palIndex;
            } else {
                tilecnt = IO::loadTypeIcon( data.m_types[ 0 ], 192, 0, ++oamIndex, ++palIndex, tilecnt, true );
                tilecnt = IO::loadTypeIcon( data.m_types[ 1 ], 224, 0, ++oamIndex, ++palIndex, tilecnt, true );
            }

        } else {
            IO::regularFont->setColor( WHITE_IDX, 1 );
            IO::regularFont->printString( "Ei /", 16, 96, true );
            IO::regularFont->printString( "Ei", 24, 110, true );
            IO::regularFont->setColor( GRAY_IDX, 1 );
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
                    printf( "%6hhu/%2hhu AP",
                            p_pokemon.m_boxdata.m_acPP[ 0 ],
                            AttackList[ p_pokemon.m_boxdata.m_moves[ 0 ] ]->m_movePP * ( ( 5 + p_pokemon.m_boxdata.m_ppup.m_Up1 ) / 5 ) );
                }
            }
        } else { //Status
            if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
                IO::regularFont->setColor( WHITE_IDX, 1 );
                sprintf( buffer, "KP                     %3i", p_pokemon.m_stats.m_maxHP );
                IO::regularFont->printString( buffer, 130, 16, true );

                if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 0 ] == 1.1 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( RED_IDX, 2 );
                } else if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 0 ] == 0.9 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( BLUE_IDX, 2 );
                } else {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "ANG                   %3i", p_pokemon.m_stats.m_Atk );
                IO::regularFont->printString( buffer, 126, 41, true );

                if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 1 ] == 1.1 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( RED_IDX, 2 );
                } else if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 1 ] == 0.9 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( BLUE_IDX, 2 );
                } else {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "VER                   %3i", p_pokemon.m_stats.m_Def );
                IO::regularFont->printString( buffer, 124, 58, true );

                if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 3 ] == 1.1 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( RED_IDX, 2 );
                } else if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 3 ] == 0.9 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( BLUE_IDX, 2 );
                } else {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "SAN                   %3i", p_pokemon.m_stats.m_SAtk );
                IO::regularFont->printString( buffer, 122, 75, true );

                if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 4 ] == 1.1 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( RED_IDX, 2 );
                } else if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 4 ] == 0.9 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( BLUE_IDX, 2 );
                } else {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "SVE                   %3i", p_pokemon.m_stats.m_SDef );
                IO::regularFont->printString( buffer, 120, 92, true );

                if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 2 ] == 1.1 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( RED_IDX, 2 );
                } else if( NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 2 ] == 0.9 ) {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( BLUE_IDX, 2 );
                } else {
                    IO::regularFont->setColor( WHITE_IDX, 1 );
                    IO::regularFont->setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "INI                   \xC3\xC3""%3i", p_pokemon.m_stats.m_Spd );
                IO::regularFont->printString( buffer, 118, 109, true );

                IO::printRectangle( (u8)158, (u8)18, u8( 158 + 68 ), u8( 18 + 12 ), true, false, WHITE_IDX );

                IO::printRectangle( (u8)158, (u8)18, u8( 158 + ( 68.0*p_pokemon.m_boxdata.IVget( 0 ) / 31 ) ), u8( 18 + 6 ), true, false, GRAY_IDX );
                IO::printRectangle( (u8)158, u8( 18 + 6 ), u8( 158 + ( 68.0*p_pokemon.m_boxdata.m_effortValues[ 0 ] / 252 ) ), u8( 18 + 12 ), true, false, GRAY_IDX );

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
                IO::regularFont->printString( nStr.c_str( ), 0, 138, true, u8( 16 - 2 * nlCnt ) );
                IO::regularFont->printString( acAbility.m_abilityName.c_str( ), 5, 176, true );
                IO::regularFont->setColor( GRAY_IDX, 1 );
                IO::regularFont->setColor( BLACK_IDX, 2 );
            } else {
                IO::regularFont->setColor( WHITE_IDX, 1 );
                IO::regularFont->setColor( BLACK_IDX, 2 );
                if( p_pokemon.m_boxdata.m_steps > 10 ) {
                    IO::regularFont->printString( "Was da wohl", 16 * 8, 50, true );
                    IO::regularFont->printString( "schlüpfen wird?", 16 * 8, 70, true );
                    IO::regularFont->printString( "Es dauert wohl", 16 * 8, 100, true );
                    IO::regularFont->printString( "noch lange.", 16 * 8, 120, true );
                } else if( p_pokemon.m_boxdata.m_steps > 5 ) {
                    IO::regularFont->printString( "Hat es sich", 16 * 8, 50, true );
                    IO::regularFont->printString( "gerade bewegt?", 16 * 8, 70, true );
                    IO::regularFont->printString( "Da tut sich", 16 * 8, 100, true );
                    IO::regularFont->printString( "wohl bald was.", 16 * 8, 120, true );
                } else {
                    IO::regularFont->printString( "Jetzt macht es", 16 * 8, 50, true );
                    IO::regularFont->printString( "schon Geräusche!", 16 * 8, 70, true );
                    IO::regularFont->printString( "Bald ist es", 16 * 8, 100, true );
                    IO::regularFont->printString( "wohl soweit.", 16 * 8, 120, true );
                }
                IO::regularFont->setColor( GRAY_IDX, 1 );
                IO::regularFont->setColor( BLACK_IDX, 2 );
            }
        }

        IO::updateOAM( true );
        touchPosition touch;
        loop( ) {

            scanKeys( );
            touch = touchReadXY( );
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

    u8 battleUI::choosePKMN( bool p_firstIsChosen, bool p_back ) {
START:
        consoleSelect( &IO::Bottom );
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleClear( );
        u8 result = 0;
        IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = !p_back;
        IO::updateOAM( true );
        drawPKMNChoiceScreen( _battle, p_firstIsChosen );
        IO::drawSub( );
        initColors( );
        IO::printRectangle( (u8)0, (u8)0, (u8)255, (u8)28, true, false, WHITE_IDX );

        writeLogText( L"Welches PKMN?" );

        touchPosition touch;
        loop( ) {
            IO::Oam->oamBuffer[ SUB_Back_OAM ].isHidden = !p_back;
            IO::updateOAM( true );


            scanKeys( );
            touch = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( p_back && GET_AND_WAIT_R( 224, 164, 300, 300 ) ) { //Back
                result = 0;
                break;
            }
            auto teamSz = _battle->_player.m_pkmnTeam.size( );
            for( u8 i = 0; i < teamSz; ++i ) {
                u8 x = IO::Oam->oamBuffer[ SUB_CHOICE_START + 2 * i ].x;
                u8 y = IO::Oam->oamBuffer[ SUB_CHOICE_START + 2 * i ].y;

                if( GET_AND_WAIT_R( x, y, x + 96, y + 42 ) ) {
                    result = i;
                    u8 tmp = 1;
                    auto acPkmn = ACPKMN2( *_battle, result, PLAYER );
                    undrawPKMNChoiceScreen( );
                    consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    while( ( tmp = showConfirmation( acPkmn, !result || ( result == p_firstIsChosen ), result == firstMoveSwitchTarget ) ) ) {
                        if( tmp == 3 )
                            break;
                        u8 oldtmp = tmp - 1;
                        while( ( tmp = showDetailedInformation( acPkmn, tmp - 1 ) ) ) {
                            if( tmp == 1 ) {
                                result = ( result + 1 + oldtmp ) % teamSz;
                                acPkmn = ACPKMN2( *_battle, result, PLAYER );
                                tmp = 1 + oldtmp;
                            }
                            if( tmp == 2 ) {
                                result = ( result + teamSz - 1 ) % teamSz;
                                acPkmn = ACPKMN2( *_battle, result, PLAYER );
                                tmp = 1 + oldtmp;
                            }
                            if( tmp == 3 ) {
                                tmp = 1 + ( 1 - oldtmp );
                                oldtmp = tmp - 1;
                            }
                        }
                        acPkmn = ACPKMN2( *_battle, result, PLAYER );
                        undrawPKMNChoiceScreen( );
                        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
                        consoleClear( );
                    }
                    if( !tmp )
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
        _battle->log( L"Use Nav[A]" );
    }

    void battleUI::showAttack( bool p_opponent, u8 p_pokemonPos ) {
        // Attack animation here

        auto acMove = _battle->_battleMoves[ p_pokemonPos ][ p_opponent ];

        if( acMove.m_type != battle::battleMove::ATTACK )
            return;
    }

    void battleUI::updateHP( bool p_opponent, u8 p_pokemonPos ) {
        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;

        u8 hpx = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
            hpy = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y;

        IO::displayHP( 100, 100 - ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP * 100 / ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_maxHP,
                       hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, true );

        consoleSelect( &IO::Top );
        if( p_opponent == p_pokemonPos ) {
            consoleSetWindow( &IO::Top, ( hpx + 40 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
            printf( "%10ls%c\n",
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_boxdata.m_name,
                    GENDER( ACPKMN2( *_battle, p_pokemonPos, p_opponent ) ) );
            printf( "Lv%3d%4dKP\n", ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_Level,
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP );
        } else {
            consoleSetWindow( &IO::Top, ( hpx - 88 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
            printf( "%10ls%c\n",
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_boxdata.m_name,
                    GENDER( ACPKMN2( *_battle, p_pokemonPos, p_opponent ) ) );
            printf( "Lv%3d%4dKP\n", ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_Level,
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP );
        }
    }

    void battleUI::applyEXPChanges( bool p_opponent, u8 p_pokemonPos, u32 p_gainedExp ) {

        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;

        u8 hpx = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
            hpy = IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y;

        pokemonData p;
        auto& acPkmn = ACPKMN2( *_battle, p_pokemonPos, p_opponent );

        if( !acPkmn.m_stats.m_acHP )
            return;

        getAll( acPkmn.m_boxdata.m_speciesId, p );

        u16 expStart = ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] - p_gainedExp ) * 100 /
            ( EXP[ acPkmn.m_Level ][ p.m_expType ] - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] );
        u16 expEnd = std::min( u16( 100 ), u16( ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) * 100 /
            ( EXP[ acPkmn.m_Level ][ p.m_expType ] - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) ) );

        std::swprintf( wbuffer, 50, L"%ls gewinnt %d E.-Punkte.[A]", acPkmn.m_boxdata.m_name, p_gainedExp );
        _battle->log( wbuffer );
        IO::displayEP( expStart, expEnd, hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, true );


        bool newLevel = EXP[ acPkmn.m_Level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
        u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;

        while( newLevel ) {
            acPkmn.m_Level++;

            if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ]
                + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPkmn.m_Level / 100 ) + 10;
            else
                acPkmn.m_stats.m_maxHP = 1;
            pkmnNatures nature = acPkmn.m_boxdata.getNature( );

            acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2 * p.m_bases[ ATK + 1 ]
                + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 ) * NatMod[ nature ][ ATK ];
            acPkmn.m_stats.m_Def = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ DEF ];
            acPkmn.m_stats.m_Spd = ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SPD ];
            acPkmn.m_stats.m_SAtk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SATK ];
            acPkmn.m_stats.m_SDef = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*NatMod[ nature ][ SDEF ];

            acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

            std::swprintf( wbuffer, 50, L"%ls erreicht Level %d.[A]", acPkmn.m_boxdata.m_name, acPkmn.m_Level );
            _battle->log( wbuffer );

            updateHP( p_opponent, p_pokemonPos );

            u8 oldSpec = acPkmn.m_boxdata.m_speciesId;
            _battle->checkForAttackLearn( p_pokemonPos );
            _battle->checkForEvolution( PLAYER, p_pokemonPos );
            if( oldSpec != acPkmn.m_boxdata.m_speciesId )
                _battle->checkForAttackLearn( p_pokemonPos );
            newLevel = acPkmn.m_Level < 100 && EXP[ acPkmn.m_Level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;

            expStart = 0;
            expEnd = std::min( u16( 100 ), u16( ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) * 100 /
                ( EXP[ acPkmn.m_Level ][ p.m_expType ] - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) ) );

            IO::displayEP( 101, 101, hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, false );
            IO::displayEP( expStart, expEnd, hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, true );
        }
    }

    void battleUI::updateStats( bool p_opponent, u8 p_pokemonPos, bool p_move ) {
        //if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
        //    return;
        setStsBallSts( p_opponent, p_pokemonPos, ACPKMNSTS2( *_battle, p_pokemonPos, p_opponent ), false );
        if( p_pokemonPos <= ( _battle->m_battleMode == battle::DOUBLE ) && p_move
            && ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP ) {
            u8 hpx = 0, hpy = 0;
            if( p_opponent ) {
                hpx = 88;
                hpy = 40;

                if( p_pokemonPos ) {
                    hpx -= 88;
                    hpy -= 32;
                }
            } else {
                hpx = 220;
                hpy = 152;

                if( !p_pokemonPos ) {
                    hpx -= 88;
                    hpy -= 32;
                }
            }
            setStsBallPosition( p_opponent, p_pokemonPos, hpx + 8, hpy + 8, false );
        }
        IO::updateOAM( false );
    }

    void battleUI::updateStatus( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::showStatus( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::hidePKMN( bool p_opponent, u8 p_pokemonPos ) {
        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;

        //Hide PKMN sprite
        for( u8 i = PKMN_IDX( p_pokemonPos, p_opponent ); i < PKMN_IDX( p_pokemonPos, p_opponent ) + 4; ++i )
            IO::OamTop->oamBuffer[ i ].isHidden = true;

        //Hide HP Bar
        //OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].isHidden = true;
        IO::displayHP( 100, 100, IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
                       IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y,
                       HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );
        IO::displayEP( 100, 100, IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
                       IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y, OWN1_EP_COL, OWN1_EP_COL + 1, false );
        //setStsBallVisibility( p_opponent, p_pokemonPos, true, false );
        setStsBallSts( p_opponent, p_pokemonPos, ACPKMNSTS2( *_battle, p_pokemonPos, p_opponent ), false );
        IO::updateOAM( false );

        //Clear text
        u8 hpx = 0, hpy = 0;

        if( p_opponent ) {
            hpx = 88;
            hpy = 40;

            if( p_pokemonPos ) {
                hpx -= 88;
                hpy -= 32;
            }
        } else {
            hpx = 220;
            hpy = 152;

            if( !p_pokemonPos ) {
                hpx -= 88;
                hpy -= 32;
            }
        }
        consoleSelect( &IO::Top );
        if( p_opponent == p_pokemonPos ) {
            consoleSetWindow( &IO::Top, ( hpx + 40 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
        } else {
            consoleSetWindow( &IO::Top, ( hpx - 88 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
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

        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall1Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall1Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall1TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall2Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall2TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall3Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall3TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall4Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall4TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall5Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall5TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall6Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall6TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall7Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall7TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall8Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall8TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall9Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall9TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall10Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall10TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, PokeBall11Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], PokeBall11TilesLen );
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

        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny1TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny2TilesLen );
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

        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny1TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny2TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny1TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny2TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny1TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* IO::COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( IO::SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * IO::OFFSET_MULTIPLIER ], Shiny2TilesLen );
        IO::updateOAM( false );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        IO::updateOAM( false );
    }

    void battleUI::sendPKMN( bool p_opponent, u8 p_pokemonPos ) {
        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;
        loadA( );

        s16 x = 0, y = 0;
        u8 hpx = 0, hpy = 0;

        if( p_opponent ) {
            hpx = 88;
            hpy = 40;

            x = 176;
            y = 19;

            if( p_pokemonPos ) {
                hpx -= 88;
                hpy -= 32;

                x = 112;
                y = 14;
            }
        } else {
            hpx = 220;
            hpy = 152;

            x = 60;
            y = 120;

            if( !p_pokemonPos ) {
                hpx -= 88;
                hpy -= 32;

                x = 0;
                y = 115;
            }
        }

        auto acPkmn = ACPKMN2( *_battle, p_pokemonPos, p_opponent );
        if( !acPkmn.m_stats.m_acHP )
            return;
        //Lets do some animation stuff here

        if( p_opponent )
            std::swprintf( wbuffer, 200, L"[TRAINER] ([TCLASS]) schickt\n%ls in den Kampf![A]",
            ACPKMN2( *_battle, p_pokemonPos, OPPONENT ).m_boxdata.m_name );
        else
            std::swprintf( wbuffer, 50, L"Los [OWN%d]![A]", p_pokemonPos + 1 );
        _battle->log( wbuffer );

        setStsBallVisibility( p_opponent, p_pokemonPos, true, false );
        IO::updateOAM( false );

        animatePokeBall( x + 40, y + 40, PB_ANIM, 15, TILESTART );

        //Load the PKMN sprite
        if( p_opponent ) {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                    acPkmn.m_boxdata.m_speciesId, x, y,
                    PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                    acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( L"Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
        } else {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/", acPkmn.m_boxdata.m_speciesId, x, y,
                PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/",
                    acPkmn.m_boxdata.m_speciesId, x, y,
                    PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                    acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( L"Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
        }
        if( acPkmn.m_boxdata.isShiny( ) )
            animateShiny( x + 16, y + 16, SHINY_ANIM, 15, TILESTART );

        setStsBallPosition( p_opponent, p_pokemonPos, hpx + 8, hpy + 8, false );
        IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].isHidden = false;
        IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x = hpx;
        IO::OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y = hpy;
        setStsBallVisibility( p_opponent, p_pokemonPos, false, false );
        IO::updateOAM( false );

        pokemonData p;
        getAll( acPkmn.m_boxdata.m_speciesId, p );

        IO::displayHP( 100, 101, hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );
        IO::displayHP( 100, 100 - acPkmn.m_stats.m_acHP * 100 / acPkmn.m_stats.m_maxHP,
                       hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );

        IO::displayEP( 0, ( acPkmn.m_boxdata.m_experienceGained - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) * 100 /
                       ( EXP[ acPkmn.m_Level ][ p.m_expType ] - EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ),
                       hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, false );

        consoleSelect( &IO::Top );
        if( p_opponent == p_pokemonPos ) {
            consoleSetWindow( &IO::Top, ( hpx + 40 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            printf( "%10ls%c\n",
                    acPkmn.m_boxdata.m_name,
                    GENDER( acPkmn ) );
            printf( "Lv%3d%4dKP\n", acPkmn.m_Level,
                    acPkmn.m_stats.m_acHP );
        } else {
            consoleSetWindow( &IO::Top, ( hpx - 88 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            printf( "%10ls%c\n",
                    acPkmn.m_boxdata.m_name,
                    GENDER( acPkmn ) );
            printf( "Lv%3d%4dKP\n", acPkmn.m_Level,
                    acPkmn.m_stats.m_acHP );
        }
    }

    void battleUI::evolvePKMN( bool p_opponent, u8 p_pokemonPos ) {
        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;
        s16 x = 0, y = 0;
        if( p_opponent ) {
            x = 176;
            y = 19;
            if( p_pokemonPos ) {
                x = 112;
                y = 14;
            }
        } else {
            x = 60;
            y = 120;
            if( !p_pokemonPos ) {
                x = 0;
                y = 115;
            }
        }
        auto acPkmn = ACPKMN2( *_battle, p_pokemonPos, p_opponent );
        if( p_opponent ) {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMN/",
                    acPkmn.m_boxdata.m_speciesId, x, y,
                    PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                    acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( L"Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
        } else {
            if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/", acPkmn.m_boxdata.m_speciesId, x, y,
                PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ),
                false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !IO::loadPKMNSprite( "nitro:/PICS/SPRITES/PKMNBACK/",
                    acPkmn.m_boxdata.m_speciesId, x, y,
                    PKMN_IDX( p_pokemonPos, p_opponent ), PKMN_PAL_IDX( p_pokemonPos, p_opponent ), PKMN_TILE_IDX( p_pokemonPos, p_opponent ), false,
                    acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( L"Sprite failed!\n(That's a bad thing, btw.)[A]" );
                }
            }
        }
        updateHP( p_opponent, p_pokemonPos );
    }

    void battleUI::learnMove( u8 p_pokemonPos, u16 p_move ) {
        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;
        if( !p_move )
            return;

        //Check if the PKMN already knows this move
        auto& acPkmn = ACPKMN2( *_battle, p_pokemonPos, PLAYER );

        for( u8 i = 0; i < 4; ++i )
            if( acPkmn.m_boxdata.m_moves[ i ] == p_move )
                return;

        if( acPkmn.m_boxdata.m_moves[ 3 ] ) {
            std::swprintf( wbuffer, 50, L"%ls kann nun\n%s erlernen![A][CLEAR]Aber %ls kennt\nbereits 4 Attacken.[A]",
                           acPkmn.m_boxdata.m_name,
                           AttackList[ p_move ]->m_moveName.c_str( ),
                           acPkmn.m_boxdata.m_name );
            _battle->log( wbuffer );
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
                    std::swprintf( wbuffer, 100, L"%ls vergisst %s[A]\nund erlernt %s![A]",
                                   acPkmn.m_boxdata.m_name,
                                   AttackList[ res ]->m_moveName.c_str( ),
                                   AttackList[ p_move ]->m_moveName.c_str( ) );
                    _battle->log( wbuffer );

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
                    std::swprintf( wbuffer, 50, L"%ls erlernt %s![A]",
                                   acPkmn.m_boxdata.m_name,
                                   AttackList[ p_move ]->m_moveName.c_str( ) );
                    _battle->log( wbuffer );
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

        dmaCopy( mug_001_1Bitmap, bgGetGfxPtr( IO::bg2 ), 256 * 192 );
        dmaCopy( mug_001_1Pal, BG_PALETTE, 64 );
    }

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE_UI
    //////////////////////////////////////////////////////////////////////////
}