/*
    Pokémon Emerald 2 Version
    ------------------------------

    file        : battle.cpp
    author      : Philip Wellnitz (RedArceus)
    description : Functionality for Pokémon battles

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


#include <cwchar>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <tuple>
#include <initializer_list>

#include "battle.h"
#include "pokemon.h"
#include "move.h"
#include "item.h"
#include "screenLoader.h"
#include "messageBox.h"
#include "saveGame.h"
#include "bag.h"
#include "buffer.h"
#include "fs.h"
#include "sprite.h"

#include "Message.h"
#include "Border_1.h"
#include "Border_2.h"
#include "Border_3.h"
#include "Border_4.h"
#include "Border_5.h"
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

#include "BattleBall1.h" //Normal
#include "BattleBall2.h" //Statused
#include "BattleBall3.h" //Fainted
#include "BattleBall4.h" //NA

//Test Trainer mugs:
#include "mug_001_1.h"
#include "mug_001_2.h"

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

#include "TestBattleBack.h"
#include "Border.h"

namespace BATTLE {
    const char* trainerclassnames[ ] = { "Pokémon-Trainer" };

    const char* ailmentnames[ ] = {
        "none",
        "wurde paralysiert.",
        "schläft ein.",
        "wurde eingefroren.",
        "fängt an zu brennen.",
        "wurde vergiftet.",
        "wurde verwirrt.",
        "Infatuation",
        "ist gefangen.",
        "wurde in Nachtmahr\ngefangen.",
        "wurde Folterknecht\nunterworfen.",
        "wurde blockiert.",
        "gähnt.",
        "kann nicht mehr\ngeheilt werden.",
        "No_type_immunity",
        "wurde bepflanzt",
        "fällt unter ein\nEmbargo.",
        "hört Abgesang.",
        "Ingrain"
    };

    //////////////////////////////////////////////////////////////////////////
    // BEGIN BATTLE_UI
    //////////////////////////////////////////////////////////////////////////

    void battleUI::displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, bool p_big ) {
        if( p_big )
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 20, 24 );
        else
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 8, 12 );
    }
    void battleUI::displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR ) {
        p_HP = std::max( std::min( (u16)101, p_HP ), u16( 0 ) );
        u16 factor = std::max( 1, p_outerR / 15 );
        if( p_HP > 100 ) {
            BG_PALETTE[ p_freecolor1 ] = GREEN;
            for( u16 i = 0; i < factor * 100; ++i )
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                u8 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor1 ) << 8 ) | (u8)p_freecolor1;
                //printf("%i %i; ",nx,ny);
                }
        } else {
            BG_PALETTE[ p_freecolor2 ] = NORMAL_;
            for( u16 i = factor * 100 - factor*p_HPstart; i < factor*p_HP; ++i ) {
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u8 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                    ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor2 ) << 8 ) | (u8)p_freecolor2;
                    if( i == factor * 50 )
                        BG_PALETTE[ p_freecolor1 ] = YELLOW;
                    if( i == factor * 80 )
                        BG_PALETTE[ p_freecolor1 ] = RED;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }
    void battleUI::displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR ) {
        u16 factor = std::max( 1, p_outerR / 15 );
        if( p_EPstart >= 100 || p_EP > 100 ) {
            BG_PALETTE[ p_freecolor1 ] = NORMAL_;
            for( u16 i = 0; i < factor * 100; ++i )
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                u16 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor1 ) << 8 ) | (u8)p_freecolor1;
                //printf("%i %i; ",nx,ny);
                }
        } else {
            BG_PALETTE[ p_freecolor2 ] = ICE;
            for( u16 i = p_EPstart*factor; i <= p_EP*factor; ++i ) {
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 + j * sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 ), ny = p_y + 16 + j * cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 );
                    ( (color *)BG_BMP_RAM( 1 ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor2 ) << 8 ) | (u8)p_freecolor2;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }

    void battleUI::initLogScreen( ) {
        cust_font.setColor( 0, 0 );
        cust_font.setColor( 251, 1 );
        cust_font.setColor( 252, 2 );
        cust_font2.setColor( 0, 0 );
        cust_font2.setColor( 253, 1 );
        cust_font2.setColor( 254, 2 );

        BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
        BG_PALETTE[ 251 ] = RGB15( 15, 15, 15 );
        BG_PALETTE[ 252 ] = RGB15( 3, 3, 3 );
        BG_PALETTE[ 253 ] = RGB15( 15, 15, 15 );
        BG_PALETTE[ 254 ] = RGB15( 31, 31, 31 );
        FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
    }
    void battleUI::clearLogScreen( ) {
        FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
    }
    void battleUI::setLogTextColor( u16 p_color ) {
        BG_PALETTE_SUB[ 251 ] = BG_PALETTE[ 251 ] = p_color;
    }
    void battleUI::setLogText2Color( u16 p_color ) {
        BG_PALETTE_SUB[ 253 ] = BG_PALETTE[ 253 ] = p_color;
    }
    void battleUI::writeLogText( const std::wstring& p_message ) {
        cust_font.printMBString( p_message.c_str( ), 8, 8, true );
    }

    void battleUI::waitForTouchUp( ) {
        while( 1 ) {
            swiWaitForVBlank( );
            updateTime( false );
            scanKeys( );
            auto t = touchReadXY( );
            if( t.px == 0 && t.py == 0 )
                break;
        }
    }
    void battleUI::waitForKeyUp( int p_key ) {
        while( 1 ) {
            scanKeys( );
            swiWaitForVBlank( );
            updateTime( );
            if( keysUp( ) & p_key )
                break;
        }
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
#define PKMN_PAL_START
#define OPP_PKMN_1_PAL        0-1
#define OPP_PKMN_2_PAL        1-1
#define OWN_PKMN_1_PAL        2-1
#define OWN_PKMN_2_PAL        3-1

#define PB_PAL_TOP(i)         ((i) +4)
#define HP_PAL                7

#define OPP_PKMN_1_TILE      44
#define OPP_PKMN_2_TILE     188
#define OWN_PKMN_1_TILE     332
#define OWN_PKMN_2_TILE     476

#define OWN1_EP_COL         160
#define OWN2_EP_COL         OWN1_EP_COL

#define OWN_HP_COL          150
#define OPP_HP_COL          155

#define HP_COL(a,b) (((a) == OPPONENT )? (OPP_HP_COL + (b)*2 ): (OWN_HP_COL + (b)*2 ))

#define GENDER(a) (a.m_boxdata.m_isFemale? 147 : (a.m_boxdata.m_isGenderless ? ' ' : 141))

    // BOTTOM SCREEN DEFINES
#define PB_PAL_SUB(i) (i)
#define FIGHT_START 0

#define SUB_PKMN_ICON_PAL 5

#define SUB_A_OAM 30
#define SUB_Back_OAM 31

    u16 initStsBalls( bool p_bottom, battle* p_battle, u16& p_tilecnt ) {
        //Own PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = ACPKMNSTS2( *p_battle, i, PLAYER );
            p_tilecnt = loadSprite( p_bottom ? Oam : OamTop, p_bottom ? spriteInfo : spriteInfoTop, p_bottom ? i : ( STSBALL_START + i ),
                                    p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, p_bottom ? ( 16 * i ) : 240 - ( 16 * ( 6 - i ) ),
                                    180, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                    false, false, false, OBJPRIORITY_0, p_bottom );
        }
        //Opps PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = ACPKMNSTS2( *p_battle, i, OPPONENT );
            p_tilecnt = loadSprite( p_bottom ? Oam : OamTop, p_bottom ? spriteInfo : spriteInfoTop, p_bottom ? ( 6 + i ) : ( STSBALL_START + 6 + i ),
                                    p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, !p_bottom ? ( 16 * ( 6 - i ) ) : 240 - ( 16 * i ),
                                    -4, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                    false, false, false, OBJPRIORITY_0, p_bottom );
        }
    }
    void setStsBallVisibility( bool p_opponent, u8 p_pokemonPos, bool p_visibility, bool p_bottom ) {
        if( p_bottom ) {
            Oam[ 6 * p_opponent + p_pokemonPos ].oamBuffer->isHidden = p_visibility;
            updateOAMSub( Oam );
        } else {
            OamTop[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].oamBuffer->isHidden = p_visibility;
            updateOAM( OamTop );
        }
    }
    void setStsBallSts( bool p_opponent, u8 p_pokemonPos, battle::acStatus p_status, bool p_bottom ) {
        u8 idx = 0;
        u16 tileIdx = 0;
        if( p_bottom ) {
            idx = 6 * p_opponent + p_pokemonPos;
            tileIdx = Oam[ idx ].oamBuffer->gfxIndex;
            loadSprite( Oam, spriteInfo, idx, PB_PAL_SUB( p_status ), tileIdx, p_opponent ? ( 240 - ( 16 * p_pokemonPos ) ) : ( 16 * ( 6 - p_pokemonPos ) ),
                        p_opponent ? -4 : 180, 16, 16, PB_PAL( p_status ), PB_TILES( p_status ), PB_TILES_LEN( p_status ),
                        false, false, false, OBJPRIORITY_0, p_bottom );
            updateOAMSub( Oam );
        } else {
            idx = STSBALL_IDX( p_pokemonPos, p_opponent );
            tileIdx = OamTop[ idx ].oamBuffer->gfxIndex;
            loadSprite( Oam, spriteInfo, idx, PB_PAL_SUB( p_status ), tileIdx, !p_opponent ? ( 240 - ( 16 * ( 6 - p_pokemonPos ) ) ) : ( 16 * p_pokemonPos ),
                        p_opponent ? -4 : 180, 16, 16, PB_PAL( p_status ), PB_TILES( p_status ), PB_TILES_LEN( p_status ),
                        false, false, false, OBJPRIORITY_0, p_bottom );
            updateOAM( OamTop );
        }
    }

    void loadSpritesTop( battle* p_battle ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );

        dmaCopy( TestBattleBackBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
        dmaCopy( TestBattleBackPal, BG_PALETTE, 128 * 2 );
        dmaCopy( mug_001_1Bitmap, bgGetGfxPtr( bg2 ), 256 * 192 );
        dmaCopy( mug_001_1Pal, BG_PALETTE, 64 );
        for( u8 i = 0; i < 40; ++i ) {
            swiWaitForVBlank( );
            updateTime( );
        }
        dmaCopy( mug_001_2Bitmap, bgGetGfxPtr( bg2 ), 256 * 192 );
        dmaCopy( mug_001_2Pal, BG_PALETTE, 64 );
        for( u8 i = 0; i < 120; ++i ) {
            swiWaitForVBlank( );
            updateTime( );
        }

        dmaFillWords( 0, bgGetGfxPtr( bg2 ), 256 * 192 );
    }

    void loadSpritesSub( battle* p_battle ) {
        initOAMTableSub( Oam );
        drawSub( );

        u16 tilecnt = 0;
        tilecnt = initStsBalls( true, p_battle, tilecnt );

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
        sprintf( buffer, "Eine Herausforderung von\n%s %s!",
                 trainerclassnames[ p_battle->_opponent->m_trainerClass ],
                 p_battle->_opponent->m_battleTrainerName );
        cust_font.printString( buffer, 16, 80, true );
        updateOAMSub( Oam );
    }

    void loadBattleUITop( battle* p_battle ) {
        initOAMTable( OamTop );

        Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &Top, &cfont );


        u16 tilecnt = 0;
        initStsBalls( false, p_battle, tilecnt );

        updateOAM( OamTop );
    }
    void loadBattleUISub( u16 p_pkmnId, bool p_isWildBattle, bool p_showNav ) {
        u16 tilecnt = 0;
        //Load UI Sprites
        //FIGHT -- 1
        tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START, 0, tilecnt, 64,
                              72, 64, 64, BattleSub1Pal, BattleSub1Tiles, BattleSub1TilesLen,
                              false, false, false, OBJPRIORITY_2, true );
        tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START + 1, 0, tilecnt, 128,
                              72, 64, 64, BattleSub1Pal, BattleSub1Tiles, BattleSub1TilesLen,
                              false, true, false, OBJPRIORITY_2, true );
        //FIGHT-TEXT
        tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START + 2, 1, tilecnt, 96,
                              100, 64, 32, BattleSub2Pal, BattleSub2Tiles, BattleSub2TilesLen,
                              false, false, false, OBJPRIORITY_0, true );
        //RUN / POKENAV
        if( p_isWildBattle ) { //Show Run
            tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START + 3, 2, tilecnt, 91,
                                  150, 64, 32, BattleSub3Pal, BattleSub3Tiles, BattleSub3TilesLen,
                                  false, false, false, OBJPRIORITY_3, true );
        } else if( p_showNav ) { //Show Nav
            tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START + 3, 2, tilecnt, 91,
                                  150, 64, 32, BattleSub6Pal, BattleSub6Tiles, BattleSub6TilesLen,
                                  false, false, false, OBJPRIORITY_3, true );
        }
        //BAG
        tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START + 4, 3, tilecnt, 0,
                              157, 64, 32, BattleSub4Pal, BattleSub4Tiles, BattleSub4TilesLen,
                              false, false, false, OBJPRIORITY_3, true );
        //POKEMON
        tilecnt = loadSprite( Oam, spriteInfo, FIGHT_START + 5, 4, tilecnt, 185,
                              142, 64, 32, BattleSub5Pal, BattleSub5Tiles, BattleSub5TilesLen,
                              false, false, false, OBJPRIORITY_3, true );
        //Load an icon of the PKMN, too
        u8 oamIndex = FIGHT_START + 5;
        u8 palIndex = 5;

        FS::drawPKMNIcon( Oam, spriteInfo, p_pkmnId, 112, 68, oamIndex, palIndex, tilecnt, true );

        //PreLoad A and Back buttons

        tilecnt = loadSprite( Oam, spriteInfo, SUB_A_OAM, 14, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                              ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        FONT::ASpriteOamIndex = SUB_A_OAM;
        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 15, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );

        //That's it
        updateOAMSub( Oam );
    }
    void hideBattleUISub( ) {
        for( u8 i = 0; i < FIGHT_START + 6; ++i )
            Oam->oamBuffer[ i ].isHidden = true;
        updateOAMSub( Oam );
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
    }

    void battleUI::trainerIntro( ) {
        //Use a 2D Map as top background for trainer intro
        loadSpritesSub( _battle );
        loadSpritesTop( _battle ); // This should consume some time

        loadBattleUITop( _battle );
        initOAMTableSub( Oam );
        drawSub( );
        hideBattleUISub( );
        initLogScreen( );
    }

    void battleUI::declareBattleMove( u8 p_pokemonPos ) {
        swprintf( wbuffer, 50, L"Was soll %ls tun?", ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_name );
        writeLogText( wbuffer );

        loadBattleUISub( ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId, true, false );

        while( 1 ) {

        }

        hideBattleUISub( );
    }

    void battleUI::declareBattleMoveChoose( ) {

    }

    void battleUI::chooseAttack( ) {

    }

    void battleUI::chooseItem( ) {

    }

    u8 battleUI::choosePKMN( bool p_firstIsChosen ) {

    }

    void battleUI::useNav( ) {

    }

    void battleUI::showAttack( u8 p_moveNo ) {

    }

    void battleUI::updateHP( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::applyEXPChanges( ) {

    }

    void battleUI::updateStats( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::hidePKMN( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::sendPKMN( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::evolvePKMN( bool p_opponent, u8 p_pokemonPos ) {

    }

    void battleUI::learnMove( u8 p_pokemonPos, u16 p_move ) {

    }

    void battleUI::dinit( ) {

    }

#undef OWN_HP_1
#undef OWN_HP_2              
#undef OPP_HP_1              
#undef OPP_HP_2              
#undef OWN_PB_START          
#undef OPP_PB_START         
#undef OPP_PKMN_1_START     
#undef OPP_PKMN_2_START    
#undef OWN_PKMN_1_START     
#undef OWN_PKMN_2_START     

#undef PB_ANIM             
#undef SHINY_ANIM          

#undef OPP_PKMN_1_PAL        
#undef OPP_PKMN_2_PAL       
#undef OWN_PKMN_1_PAL       
#undef OWN_PKMN_2_PAL        

#undef PB_PAL_START          
#undef HP_PAL                

#undef OPP_PKMN_1_TILE      
#undef OPP_PKMN_2_TILE     
#undef OWN_PKMN_1_TILE     
#undef OWN_PKMN_2_TILE     

#undef OWN1_EP_COL         
#undef OWN2_EP_COL         

#undef OWN_HP_COL          
#undef OPP_HP_COL          

#undef HP_COL

#undef GENDER

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE_UI
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // BEGIN BATTLE
    //////////////////////////////////////////////////////////////////////////
#define C2I(a) ((a) - L'0')
    std::wstring parseLogCmd( battle& p_battle, const std::wstring& p_cmd ) {
        if( p_cmd == L"A" )
            return L"`";
        if( p_cmd == L"CLEAR" )
            return L"";

        if( p_cmd == L"TRAINER" ) {
            std::swprintf( wbuffer, 50, L"%s", p_battle._opponent->m_battleTrainerName );
            return std::wstring( wbuffer );
        }
        if( p_cmd == L"TCLASS" ) {
            std::swprintf( wbuffer, 50, L"%s", p_battle._opponent->m_trainerClass );
            return std::wstring( wbuffer );
        }
        if( p_cmd.substr( 0, 4 ) == L"COLR" ) {
            u8 r, g, b;

            r = 10 * C2I( p_cmd[ 5 ] ) + C2I( p_cmd[ 6 ] );
            g = 10 * C2I( p_cmd[ 8 ] ) + C2I( p_cmd[ 9 ] );
            b = 10 * C2I( p_cmd[ 11 ] ) + C2I( p_cmd[ 12 ] );

            p_battle._battleUI.setLogTextColor( RGB15( r, g, b ) );
            return L"";
        }

        POKEMON::pokemon& target = ACPKMN2( p_battle, 0, PLAYER );
        bool isPkmn = false;
        bool isOpp = false;

        if( p_cmd.substr( 0, 4 ) == L"OWN1" ) {
            target = ACPKMN2( p_battle, 0, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OWN2" ) {
            target = ACPKMN2( p_battle, 1, PLAYER );
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP1" ) {
            target = ACPKMN2( p_battle, 0, OPPONENT );
            isOpp = true;
            isPkmn = true;
        }
        if( p_cmd.substr( 0, 4 ) == L"OPP2" ) {
            target = ACPKMN2( p_battle, 1, OPPONENT );
            isOpp = true;
            isPkmn = true;
        }

        if( isPkmn && p_cmd.length( ) == 4 )
            return target.m_boxdata.m_name + std::wstring( isOpp ? L" (Gegner)" : L"" );

        if( isPkmn ) {
            auto specifier = p_cmd.substr( 5 );
            if( specifier == L"ABILITY" )
                return getWAbilityName( target.m_boxdata.m_ability );
            if( specifier.substr( 0, 4 ) == L"MOVE" ) {
                std::swprintf( wbuffer, 50, L"%s", AttackList[ target.m_boxdata.m_moves[ C2I( specifier[ 4 ] ) - 1 ] ]->m_moveName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"ITEM" ) {
                std::swprintf( wbuffer, 50, L"%s", ITEMS::ItemList[ target.m_boxdata.m_holdItem ].m_itemName.c_str( ) );
                return std::wstring( wbuffer );
            }
            if( specifier == L"LEVEL" )
                return ( L"" + target.m_Level );
        }

        return L"";
    }

    battle::battle( battleTrainer* p_player, battleTrainer* p_opponent, int p_maxRounds, int p_AILevel, battleMode p_battleMode ) {
        _maxRounds = p_maxRounds;
        _AILevel = p_AILevel;
        _player = p_player;
        _opponent = p_opponent;
        m_battleMode = p_battleMode;

        m_distributeEXP = true;
    }


    /**
     *  @brief Write the message p_message to the battle log
     *  @param p_message: The message to be written
     */
    void battle::log( const std::wstring& p_message ) {
        std::wstring msg = L"";
        for( size_t i = 0; i < p_message.length( ); i++ ) {
            if( p_message[ i ] == L'[' ) {
                std::wstring accmd = L"";
                while( p_message[ ++i ] != L']' )
                    accmd += p_message[ i ];
                msg += parseLogCmd( *this, accmd );
                if( accmd == L"CLEAR" ) {
                    _battleUI.writeLogText( msg );
                    _battleUI.clearLogScreen( );
                }
            } else
                msg += p_message[ i ];
        }

        _battleUI.writeLogText( msg );
        _battleUI.clearLogScreen( );
    }

    /**
     *  @brief      Runs the battle.
     *  @returns    -1 if opponent won, 0 if the battle resulted in a tie, 1 otherwise
     */
    s8 battle::start( ) {
        battleEndReason battleEnd;

        initBattle( );

        while( 1 || _round++ < _maxRounds ) {
            registerParticipatedPKMN( );

            if( canMove( PLAYER, 0 ) )
                _battleUI.declareBattleMove( 0 );
            else
                log( L"[OWN1] kann nicht angreifen...[A]" );
            if( m_battleMode == DOUBLE && canMove( PLAYER, 1 ) )
                _battleUI.declareBattleMove( 1 );
            else if( m_battleMode == DOUBLE )
                log( L"[OWN2] kann nicht angreifen...[A]" );

            getAIMoves( );

            doMoves( );

            if( endConditionHit( battleEnd ) ) {
                endBattle( battleEnd );
                break;
            }
            doWeather( );

            refillBattleSpots( true );
        }

        return battleEnd;
    }

    /**
     *  @brief Initialize the battle.
     */
    void battle::initBattle( ) {
        //Some basic initialization stuff
        _battleUI = battleUI( this );
        _battleUI.init( );

        for( u8 i = 0; i < 6; ++i ) {
            ACPOS( i, PLAYER ) = ACPOS( i, OPPONENT ) = i;
            for( u8 o = 0; o < MAX_STATS; ++o )
                ACPKMNSTATCHG( i, PLAYER )[ o ] = ACPKMNSTATCHG( i, OPPONENT )[ o ] = 0;
            if( _player->m_pkmnTeam->size( ) > i ) {
                if( ACPKMN( i, PLAYER ).m_boxdata.m_individualValues.m_isEgg )
                    ACPKMNSTS( i, PLAYER ) = NA;
                else if( ACPKMN( i, PLAYER ).m_stats.m_acHP == 0 )
                    ACPKMNSTS( i, PLAYER ) = KO;
                else if( ACPKMN( i, PLAYER ).m_statusint )
                    ACPKMNSTS( i, PLAYER ) = STS;
                else
                    ACPKMNSTS( i, PLAYER ) = OK;
            } else
                ACPKMNSTS( i, PLAYER ) = NA;
            if( _opponent->m_pkmnTeam->size( ) > i ) {
                if( ACPKMN( i, OPPONENT ).m_boxdata.m_individualValues.m_isEgg )
                    ACPKMNSTS( i, OPPONENT ) = NA;
                else if( ACPKMN( i, OPPONENT ).m_stats.m_acHP == 0 )
                    ACPKMNSTS( i, OPPONENT ) = KO;
                else if( ACPKMN( i, OPPONENT ).m_statusint )
                    ACPKMNSTS( i, OPPONENT ) = STS;
                else ACPKMNSTS( i, OPPONENT ) = OK;
            } else
                ACPKMNSTS( i, OPPONENT ) = NA;
        }

        for( auto p : { PLAYER, OPPONENT } ) {
            _battleSpotOccupied[ 0 ][ p ] = false;
            _battleSpotOccupied[ 1 ][ p ] = !( m_battleMode == DOUBLE );
        }

        refillBattleSpots( false, false );

        _battleUI.trainerIntro( );

        refillBattleSpots( false );

        if( m_weather != NO_WEATHER ) {
            log( _weatherMessage[ m_weather ] );
        }

        doAbilities( ability::BEFORE_BATTLE );
    }

    /**
     *  @brief send in PKMN for fainted ones, if possible
     *  @param p_choice: Specifies whether the player can choose the PKMN which is/are being sent
     */
    void battle::refillBattleSpots( bool p_choice, bool p_send ) {

        for( u8 i = 0; i < 2; ++i )for( auto j : { PLAYER, OPPONENT } ) {
            if( !_battleSpotOccupied[ i ][ j ] ) {
                u8 nextSpot = i;
                if( !p_choice || j )
                    nextSpot = getNextPKMN( j, i );
                else
                    nextSpot = _battleUI.choosePKMN( i );

                if( nextSpot != 7 && nextSpot != i )
                    std::swap( ACPOS( i, j ), ACPOS( nextSpot, j ) );
            }
        }

        //Sort the remaining PKMN according to their status -> No STS, STS, KO, NA
        std::vector<std::pair<u8, u8> > tmp;
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            tmp.push_back( std::pair<u8, u8>( ACPKMNSTS( i, PLAYER ), i ) );
        std::sort( tmp.begin( ), tmp.end( ) );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            ACPOS( i, PLAYER ) = tmp[ i - ( m_battleMode == DOUBLE ? 2 : 1 ) ].second;

        //Sort the remaining PKMN according to their status -> No STS, STS, KO, NA (For the opponent)
        tmp.clear( );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            tmp.push_back( std::pair<u8, u8>( ACPKMNSTS( i, OPPONENT ), i ) );
        std::sort( tmp.begin( ), tmp.end( ) );
        for( u8 i = ( m_battleMode == DOUBLE ? 2 : 1 ); i < 6; ++i )
            ACPOS( i, OPPONENT ) = tmp[ i - ( m_battleMode == DOUBLE ? 2 : 1 ) ].second;

        orderPKMN( );

        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( auto j : { PLAYER, OPPONENT } ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    if( !_battleSpotOccupied[ i ][ j ] ) {
                        if( p_send ) {
                            _battleUI.sendPKMN( j, i );
                            _battleSpotOccupied[ i ][ j ] = true;
                        }
                    }
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
     *  @brief Gets the index of the next PKMN that is not koed.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @returns Minimum i for which ACPKMNSTS(i,p_opponent) != KO and != SELECTED, 7 iff all PKMN fainted
     */
    u8 battle::getNextPKMN( bool p_opponent, u8 p_startIdx ) {
        u8 max = ( p_opponent ? _opponent->m_pkmnTeam->size( ) : _player->m_pkmnTeam->size( ) );

        for( u8 i = p_startIdx; i < max; ++i )
            if( ACPKMNSTS( i, p_opponent ) != KO
                && ACPKMNSTS( i, p_opponent ) != SELECTED )
                return i;
        return 7;
    }

    /**
     *  @brief Orders the PKMN according to their speed.
     *  @param p_includeMovePriority: Determines whether the PKMN's move's priorities should matter.
     */
    void battle::orderPKMN( bool p_includeMovePriority ) {
        _moveOrder[ 0 ][ 0 ]
            = _moveOrder[ 0 ][ 1 ]
            = _moveOrder[ 1 ][ 0 ]
            = _moveOrder[ 1 ][ 1 ] = 42;

        std::vector< std::tuple<s8, u16, u8> > inits;
        for( u8 i = 0; i <= ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); ++i ) {
            for( auto j : { PLAYER, OPPONENT } ) {
                u16 acSpd = ACPKMN( i, j ).m_stats.m_Spd;
                s8 movePr = ( ( _battleMoves[ i ][ j ].m_type == battleMove::ATTACK || _battleMoves[ i ][ j ].m_type == battleMove::MEGA_ATTACK ) ?
                              AttackList[ ACPKMN( i, j ).m_boxdata.m_moves[ 0 ] ]->m_movePriority : 0 );
                inits.push_back( std::tuple<s8, u16, u8>( p_includeMovePriority * movePr, acSpd, 2u * i + ( 1u - j ) ) );
            }
        }
        std::sort( inits.begin( ), inits.end( ), std::greater < std::tuple<s8, u16, u8> >( ) );

        u8 c = 0;
        for( const std::tuple<s8, u16, u8> i : inits ) {
            s8 v;
            u16 _1;
            u8 _2;
            std::tie( v, _1, _2 ) = i;
            bool isOpp = v % 2,
                isSnd = v / 2;

            _moveOrder[ isSnd ][ isOpp ] = c++;
        }
    }

    /**
     *  @brief Applies all possible abilities ordered by their PKMNs speed.
     *  @param p_situation: Current situation, on which an ability may be useable
     */
    void battle::doAbilities( ability::abilityType p_situation ) {
        orderPKMN( );

        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( auto j : { PLAYER, OPPONENT } ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    doAbility( j, i, p_situation );
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
    *  @brief Applies all possible abilities ordered by their PKMNs speed.
    *  @param p_opponent: true iff the next opponent's PKMN is requested.
    *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
    *  @param p_situation: Current situation, on which an ability may be useable
    */
    void battle::doAbility( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation ) {
        auto ab = ability( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability );

        if( ab.m_type & p_situation ) {
            std::swprintf( wbuffer, 50, L"%s von %ls%s wirkt.[A]",
                           ab.m_abilityName.c_str( ),
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? "\n(Gegner)" : "\n" ) );

            log( wbuffer );
            ab.m_effect.execute( *this, &( ACPKMN( p_pokemonPos, p_opponent ) ) );

            for( u8 k = 0; k < 4; ++k ) {
                bool isOpp = k % 2,
                    isSnd = k / 2;
                _battleUI.updateHP( isOpp, isSnd );
                _battleUI.updateStats( isOpp, isSnd );
            }
        }
    }

    /**
     *  @brief Determines whether the specified PKMN can perform a move during the current turn.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    bool battle::canMove( bool p_opponent, u8 p_pokemonPos ) {
        if( m_battleMode != DOUBLE && p_pokemonPos )
            return false;
        return !ACPKMNSTATCHG( p_pokemonPos, p_opponent )[ ATTACK_BLOCKED ];
    }

    /**
     *  @brief Computes the AI's moves and stores them into _battleMoves
     */
    void battle::getAIMoves( ) {
        switch( _AILevel ) {
            default:
            case 0: // Trivial AI
            {
                for( u8 i = 0; i < 2; ++i )if( canMove( OPPONENT, i ) )
                    _battleMoves[ i ][ OPPONENT ] = { battleMove::type::ATTACK, 0, 0 };
                break;
            }
        }
    }

    /**
     *  @brief Does all of the turn's moves.
     */
    void battle::doMoves( ) {
        //Mega evolve all PKMN first
        for( u8 i = 0; i < 4; ++i ) {
            bool isOpp = i % 2,
                isSnd = i / 2;

            if( _battleMoves[ isSnd ][ isOpp ].m_type == battleMove::MEGA_ATTACK ) {
                megaEvolve( isOpp, isSnd );
                _battleMoves[ isSnd ][ isOpp ].m_type = battleMove::ATTACK;
            }
        }

        orderPKMN( true );

        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( auto j : { PLAYER, OPPONENT } ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    doMove( j, i );
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
     *  @brief Mega-evolves the specified PKMN, if possible.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    void battle::megaEvolve( bool p_opponent, u8 p_pokemonPos ) {
        if( !_allowMegaEvolution )
            return;

        //TODO
    }

    /**
     *  @brief Does the specified PKMN's move.
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     */
    void battle::doMove( bool p_opponent, u8 p_pokemonPos ) {
        _acMove = _moveOrder[ p_pokemonPos ][ p_opponent ];

        std::wstring acPkmnStr = L"";
        if( p_opponent )
            acPkmnStr = L"OPP" + ( p_pokemonPos + 1 );
        else
            acPkmnStr = L"OWN" + ( p_pokemonPos + 1 );
        auto& acMove = _battleMoves[ p_pokemonPos ][ p_opponent ];
        auto& acPkmn = ACPKMN( p_pokemonPos, p_opponent );

        switch( acMove.m_type ) {
            case battleMove::ATTACK:
            {
                auto& acAttack = AttackList[ acPkmn.m_boxdata.m_moves[ acMove.m_value ] ];
                if( acMove.m_target == 0 ) {
                    if( !p_opponent )
                        switch( acAttack->m_moveAffectsWhom ) {
                            case move::moveAffectsTypes::USER:
                                acMove.m_target = 1 + p_pokemonPos;
                                break;
                            case move::moveAffectsTypes::OWN_FIELD:
                                acMove.m_target = ( 1 << 4 );
                                break;
                            case move::moveAffectsTypes::OPPONENTS_FIELD:
                                acMove.m_target = ( 1 << 5 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES:
                                acMove.m_target = ( 1 << 2 ) | ( 1 << 3 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES_AND_PARTNER:
                                acMove.m_target = ( 1 << ( p_pokemonPos ) | ( 1 << 2 ) | ( 1 << 3 ) );
                                break;
                            case move::moveAffectsTypes::SELECTED:
                                acMove.m_target = ( 1 << 2 );
                                break;
                            case move::moveAffectsTypes::RANDOM:
                                acMove.m_target = ( 1 << ( rand( ) % 4 ) );
                                if( acMove.m_target == ( 1 << p_pokemonPos ) )
                                    acMove.m_target <<= 1;
                                break;
                            default:
                                break;
                    } else
                        switch( acAttack->m_moveAffectsWhom ) {
                            case move::moveAffectsTypes::USER:
                                acMove.m_target = ( ( 1 + p_pokemonPos ) << 2 );
                                break;
                            case move::moveAffectsTypes::OWN_FIELD:
                                acMove.m_target = ( 1 << 5 );
                                break;
                            case move::moveAffectsTypes::OPPONENTS_FIELD:
                                acMove.m_target = ( 1 << 4 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES:
                                acMove.m_target = ( 1 << 0 ) | ( 1 << 1 );
                                break;
                            case move::moveAffectsTypes::BOTH_FOES_AND_PARTNER:
                                acMove.m_target = ( 1 << 0 ) | ( 1 << 1 ) | ( 1 << ( 2 + p_pokemonPos ) );
                                break;
                            case move::moveAffectsTypes::SELECTED:
                                acMove.m_target = ( 1 << 0 );
                                break;
                            case move::moveAffectsTypes::RANDOM:
                                acMove.m_target = ( 1 << ( rand( ) % 4 ) );
                                if( acMove.m_target == ( 1 << ( 2 + p_pokemonPos ) ) )
                                    acMove.m_target >>= 1;
                                break;
                            default:
                                break;
                    }
                }

                calcDamage( _acMove, rand( ) % 16 );

                doAbilities( ability::BEFORE_ATTACK );
                doItems( ability::BEFORE_ATTACK );

                doAttack( _acMove );

                doAbilities( ability::AFTER_ATTACK );
                doItems( ability::AFTER_ATTACK );

                break;
            }
            case battleMove::SWITCH:
                battle::switchPKMN( p_opponent, p_pokemonPos, acMove.m_value );
                break;
            case battleMove::USE_ITEM:
            {
                if( p_opponent )
                    std::swprintf( wbuffer, 100, L"[TRAINER] ([TCLASS]) setzt\n%s ein.[A]", ITEMS::ItemList[ acMove.m_value ].getDisplayName( true ).c_str( ) );
                doItem( p_opponent, acMove.m_target, ability::abilityType( 0 ) );
                break;
            }
            case battleMove::USE_NAV:
                //TODO
                break;
            default:
                break;
        }
    }

    /**
    *  @brief Calculates the current move's dealt damage.
    *  @param p_moveNo: The number of the attack that shall be done.
    *  @param p_randInt: A random integer between 0 and 15 used to adjust the dealt damage
    */
    void battle::calcDamage( u8 p_moveNo, u8 p_randInt ) {
        for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
            if( _moveOrder[ i ][ j ] == p_moveNo ) {
                auto& bm = _battleMoves[ i ][ j ];

                //Calculate critical hit chance
                int mod = 16;
                switch( _criticalChance[ i ][ j ] ) {
                    case 1:
                        mod = 8;
                        break;
                    case 2:
                        mod = 4;
                        break;
                    case 3:
                        mod = 3;
                        break;
                    case 4:
                        mod = 2;
                        break;
                    default:
                        break;
                }

                for( u8 k = 0; k < 4; ++k ) {
                    bool isOpp = k % 2,
                        isSnd = k / 2;

                    if( !( bm.m_target & ( 1 << k ) ) )
                        continue;

                    auto move = AttackList[ ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] ];

                    if( move->m_moveHitType == move::STAT ) {
                        _acDamage[ isSnd ][ isOpp ] = 0;
                        return;
                    }

                    POKEMON::PKMNDATA::pokemonData pd;
                    POKEMON::PKMNDATA::getAll( ACPKMN( i, j ).m_boxdata.m_speciesId, pd );

                    //Calculate effectivity
                    _effectivity[ isSnd ][ isOpp ] = getEffectiveness( move->m_moveType, pd.m_types[ 0 ] );
                    if( pd.m_types[ 0 ] != pd.m_types[ 1 ] )
                        _effectivity[ isSnd ][ isOpp ] *= getEffectiveness( move->m_moveType, pd.m_types[ 1 ] );

                    //Calculate critical hit
                    if( p_randInt <= 15 )
                        _critical[ isSnd ][ isOpp ] = !( rand( ) % mod );
                    else {
                        _critical[ isSnd ][ isOpp ] = false;

                        if( p_randInt == 16 )
                            p_randInt = 0;
                        else
                            p_randInt = 15;
                    }
                    //STAB
                    float STAB = 1.5f;
                    if( ACPKMN( i, j ).m_boxdata.m_ability == A_ADAPTABILITY )
                        STAB = 2.0f;

                    //Weather
                    float weather = 1.0f;

                    bool weatherPossible = true;
                    for( u8 a = 0; a < 2; ++a ) for( u8 b = 0; b < 2; ++b )
                        weatherPossible &= ( ACPKMN( i, j ).m_boxdata.m_ability != A_AIR_LOCK
                        && ACPKMN( i, j ).m_boxdata.m_ability != A_CLOUD_NINE );

                    if( weatherPossible ) {
                        if( m_weather == SUN && move->m_moveType == WASSER )
                            weather = 0.5f;
                        if( m_weather == HEAVY_SUNSHINE && move->m_moveType == WASSER )
                            weather = 0.0f;

                        if( m_weather == SUN && move->m_moveType == FEUER )
                            weather = 1.5f;
                        if( m_weather == HEAVY_SUNSHINE && move->m_moveType == FEUER )
                            weather = 1.5f;

                        if( m_weather == RAIN && move->m_moveType == FEUER )
                            weather = 0.5f;
                        if( m_weather == HEAVY_RAIN && move->m_moveType == FEUER )
                            weather = 0.0f;

                        if( m_weather == RAIN && move->m_moveType == WASSER )
                            weather = 1.5f;
                        if( m_weather == HEAVY_RAIN && move->m_moveType == WASSER )
                            weather = 1.5f;
                    }

                    //Multi-Target modifier
                    int tcnt = 0;
                    for( u8 o = 0; o < 4; ++o )
                        if( ( bm.m_target & ( 1 << o ) ) )
                            tcnt++;

                    float target = 1.0f;
                    if( tcnt > 1 )
                        target = 0.75f;

                    //Burn
                    float burn = 1.0f;
                    if( ACPKMNAIL( i, j ) == move::ailment::BURN
                        && move->m_moveHitType == move::PHYS
                        && ACPKMN( i, j ).m_boxdata.m_ability != A_GUTS ) {
                        burn = 0.5f;
                    }

                    //Base damage calculation
                    auto moveAtkHitType = move->m_moveHitType;
                    auto moveDefHitType = move->m_moveHitType;

                    if( ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] == M_PSYSHOCK )
                        moveDefHitType = move::PHYS;
                    if( ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] == M_PSYSTRIKE )
                        moveDefHitType = move::PHYS;
                    if( ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] == M_SECRET_SWORD )
                        moveDefHitType = move::PHYS;

                    float atk = ( ( moveAtkHitType == move::PHYS ) ? ACPKMN( i, j ).m_stats.m_Atk : ACPKMN( i, j ).m_stats.m_SAtk );
                    if( ACPKMN( i, j ).m_boxdata.m_ability != A_UNAWARE
                        && ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] != M_FOULPLAY ) {
                        if( moveAtkHitType == move::PHYS ) {
                            if( ACPKMNSTATCHG( i, j )[ ATK ] > 0 )
                                atk = atk * ACPKMNSTATCHG( i, j )[ ATK ] / 2.0;
                            if( ACPKMNSTATCHG( i, j )[ ATK ] < 0 )
                                atk = atk  * 2.0 / ( -ACPKMNSTATCHG( i, j )[ ATK ] );
                        }
                        if( moveAtkHitType == move::SPEC ) {
                            if( ACPKMNSTATCHG( i, j )[ SATK ] > 0 )
                                atk = atk * ACPKMNSTATCHG( i, j )[ SATK ] / 2.0;
                            if( ACPKMNSTATCHG( i, j )[ SATK ] < 0 )
                                atk = atk  * 2.0 / ( -ACPKMNSTATCHG( i, j )[ SATK ] );
                        }
                    }
                    if( ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] == M_FOULPLAY ) {
                        atk = ACPKMN( isSnd, isOpp ).m_stats.m_Atk;

                        if( ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] > 0 )
                            atk = atk * ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] / 2.0;
                        if( ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] < 0 )
                            atk = atk  * 2.0 / ( -ACPKMNSTATCHG( isSnd, isOpp )[ ATK ] );
                    }

                    float def = ( ( moveDefHitType == move::PHYS ) ? ACPKMN( isSnd, isOpp ).m_stats.m_Def : ACPKMN( isSnd, isOpp ).m_stats.m_SDef );
                    if( ACPKMN( isSnd, isOpp ).m_boxdata.m_ability != A_UNAWARE
                        && ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] != M_CHIP_AWAY ) {
                        if( moveDefHitType == move::PHYS ) {
                            if( ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] > 0 )
                                atk = atk * ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] / 2.0;
                            if( ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] < 0 )
                                atk = atk  * 2.0 / ( -ACPKMNSTATCHG( isSnd, isOpp )[ DEF ] );
                        }
                        if( moveDefHitType == move::SPEC ) {
                            if( ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] > 0 )
                                atk = atk * ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] / 2.0;
                            if( ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] < 0 )
                                atk = atk  * 2.0 / ( -ACPKMNSTATCHG( isSnd, isOpp )[ SDEF ] );
                        }
                    }

                    _acDamage[ isSnd ][ isOpp ] = s16( ( ( 2 * ACPKMN( i, j ).m_Level + 10 ) / 250 ) * ( atk / def ) * move->m_moveBasePower + 2 );
                    float modifier = _effectivity[ isSnd ][ isOpp ] * ( _critical[ isSnd ][ isOpp ] ? 2 : 1 ) * STAB * weather * target * burn;
                    _acDamage[ isSnd ][ isOpp ] *= s16( modifier );
                }
            }
        }
    }

    /**
    *  @brief Applies all possible hold Items ordered by their PKMNs speed.
    *  @param p_situation: Current situation, on which an Item may be useable
    */
    void battle::doItems( ability::abilityType p_situation ) {
        orderPKMN( );

        for( u8 p = 0; p < 4; ++p ) {
            for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
                if( _moveOrder[ i ][ j ] == p ) {
                    doItem( j, i, p_situation );
                    goto NEXT;
                }
            }
NEXT:
            ;
        }
    }

    /**
    *  @brief Applies all possible hold items ordered by their PKMNs speed.
    *  @param p_opponent: true iff the next opponent's PKMN is requested.
    *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
    *  @param p_situation: Current situation, on which an item may be useable
    */
    void battle::doItem( bool p_opponent, u8 p_pokemonPos, ability::abilityType p_situation ) {
        auto im = ITEMS::ItemList[ ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_ability ];

        if( ( im.getEffectType( ) & ITEMS::item::itemEffectType::IN_BATTLE ) && ( im.m_inBattleEffect & p_situation ) ) {
            std::swprintf( wbuffer, 50, L"%s von %ls%s wirkt.[A]",
                           im.getDisplayName( ).c_str( ),
                           ( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_name ),
                           ( p_opponent ? "\n(Gegner)" : "\n" ) );

            log( wbuffer );
            im.m_inBattleScript.execute( *this, &( ACPKMN( p_pokemonPos, p_opponent ) ) );
            for( u8 k = 0; k < 4; ++k ) {
                bool isOpp = k % 2,
                    isSnd = k / 2;
                _battleUI.updateHP( isOpp, isSnd );
                _battleUI.updateStats( isOpp, isSnd );
            }
        }
    }

    /**
     *  @brief does the p_moveNo positioned attack of this turn
     *  @param p_moveNo: The number of the attack that shall be done.
     */
    void battle::doAttack( u8 p_moveNo ) {
        bool messagePrinted = false;
        for( u8 i = 0; i < 2; ++i )for( u8 j = 0; j < 2; ++j ) {
            if( _moveOrder[ i ][ j ] == p_moveNo ) {
                auto& bm = _battleMoves[ i ][ j ];
                if( bm.m_type != battleMove::ATTACK )
                    return;
                auto acMove = AttackList[ ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ] ];

                //"Preview" Attacks effect
                if( rand( ) % 100 < acMove->m_moveEffectAccuracy )
                    acMove->m_moveEffect.execute( *this, &ACPKMN( i, j ) );

                for( u8 k = 0; k < 4; ++k ) {
                    bool isOpp = k % 2,
                        isSnd = k / 2;

                    if( !( bm.m_target & ( 1 << k ) ) )
                        continue;
                    if( !_battleSpotOccupied[ isSnd ][ isOpp ] )
                        continue;

                    bool doesChange = !!_acDamage[ isSnd ][ isOpp ];

                    for( u8 s = 0; s < MAX_STATS; s++ )
                        doesChange |= !!_acStatChange[ isSnd ][ isOpp ][ s ];

                    if( !messagePrinted ) {
                        std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]",
                                       ( ACPKMN( i, j ).m_boxdata.m_name ),
                                       ( j ? "\n(Gegner)" : "\n" ),
                                       acMove->m_moveName.c_str( ) );
                        log( wbuffer );
                        messagePrinted = true;
                    }

                    //Check if the attack fails 
                    if( acMove->m_moveAccuracy && rand( ) * 1.0 / RAND_MAX > acMove->m_moveAccuracy / 100.0 ) {
                        std::swprintf( wbuffer, 100, L"%ls%s wich aus.[A]",
                                       ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                                       ( j ? "\n(Gegner)" : "\n" ) );
                        log( wbuffer );
                        goto NEXT;
                    }

                    if( doesChange ) {

                        ACPKMN( isSnd, isOpp ).m_stats.m_acHP = std::max( u16( 0 ),
                                                                          std::min( u16( ACPKMN( isSnd, isOpp ).m_stats.m_acHP - _acDamage[ isSnd ][ isOpp ] ),
                                                                          ACPKMN( isSnd, isOpp ).m_stats.m_maxHP ) );


                        for( u8 s = 0; s < MAX_STATS; s++ )
                            ACPKMNSTATCHG( isSnd, isOpp )[ s ] += _acStatChange[ isSnd ][ isOpp ][ s ];

                        _battleUI.showAttack( p_moveNo );
                        _battleUI.updateHP( isOpp, isSnd );
                        if( _critical[ isSnd ][ isOpp ] )
                            log( L"[COLR:15:15:00]Ein Volltreffer![A][COLR:15:15:15]" );
                        if( _effectivity[ isSnd ][ isOpp ] != 1.0f ) {
                            float effectivity = _effectivity[ isSnd ][ isOpp ];
                            if( effectivity > 3.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:00:31:00]Das ist enorm effektiv\ngegen %ls![A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity > 1.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:00:15:00]Das ist sehr effektiv\ngegen %ls![A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity == 0.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:31:00:00]Hat die Attacke\n%lsgetroffen?[A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity < 0.3f )
                                std::swprintf( wbuffer, 100, L"[COLR:31:00:00]Das ist nur enorm wenig\neffektiv gegen %ls...[A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            else if( effectivity < 1.0f )
                                std::swprintf( wbuffer, 100, L"[COLR:15:00:00]Das ist nicht sehr effektiv\ngegen %ls.[A][COLR:15:15:15]", ACPKMN( isSnd, isOpp ).m_boxdata.m_name );
                            log( wbuffer );
                        }

                        //Check if PKMN fainted
                        if( !ACPKMN( isSnd, isOpp ).m_stats.m_acHP ) {
                            std::swprintf( wbuffer, 100, L"%ls%s wurde besiegt.[A]",
                                           ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                                           ( j ? "\n(Gegner)" : "\n" ) );
                            log( wbuffer );
                            _battleUI.hidePKMN( isOpp, isSnd );
                            _battleSpotOccupied[ isSnd ][ isOpp ] = false;

                            ACPKMNSTS( isSnd, isOpp ) = KO;

                            if( m_distributeEXP )
                                distributeEXP( isSnd, isOpp );
                        } else
                            _battleUI.updateStats( isOpp, isSnd );
                    } else {
                        std::swprintf( wbuffer, 100, L"%ls%s bleibt von\nder Attacke unbeeindruckt...[A]",
                                       ( ACPKMN( isSnd, isOpp ).m_boxdata.m_name ),
                                       ( isOpp ? "\n(Gegner)" : "\n" ) );
                        log( wbuffer );
                    }
                }
                for( u8 k = 4; k < 6; ++k ) {
                    if( !( bm.m_target & ( 1 << k ) ) )
                        continue;
                    if( !messagePrinted ) {
                        std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]",
                                       ( ACPKMN( i, j ).m_boxdata.m_name ),
                                       ( j ? "\n(Gegner)" : "\n" ),
                                       acMove->m_moveName.c_str( ) );
                        log( wbuffer );
                        messagePrinted = true;
                    }
                    //acMove->m_moveEffect.execute( *this, &( ACPKMN( i, j ) ) );
                }

                if( !messagePrinted ) {
                    std::swprintf( wbuffer, 100, L"%ls%s setzt\n%s ein.[A]\nDie Attacke ging\nins leere...",
                                   ( ACPKMN( i, j ).m_boxdata.m_name ),
                                   ( j ? "\n(Gegner)" : "\n" ),
                                   acMove->m_moveName.c_str( ) );
                    log( wbuffer );
                    messagePrinted = true;
                }
NEXT:
                _lstMove = ACPKMN( i, j ).m_boxdata.m_moves[ bm.m_value ];
                if( j )
                    _lstOppMove = _lstMove;
                else
                    _lstOwnMove = _lstMove;
                return;
            }
        }
    }

    /**
     *  @brief Registers the current participating PKMN
     */
    void battle::registerParticipatedPKMN( ) {
        if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
            && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
            _participatedPKMN[ &ACPKMN( 0, PLAYER ) ].insert( &ACPKMN( 0, OPPONENT ) );
        if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
            && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
            _participatedPKMN[ &ACPKMN( 0, OPPONENT ) ].insert( &ACPKMN( 0, PLAYER ) );

        if( m_battleMode == DOUBLE ) {
            if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 0, PLAYER ) ].insert( &ACPKMN( 1, OPPONENT ) );
            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 0, OPPONENT ) ].insert( &ACPKMN( 1, PLAYER ) );

            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 0, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 0, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, PLAYER ) ].insert( &ACPKMN( 0, OPPONENT ) );
            if( ACPKMNSTS( 0, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 0, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, OPPONENT ) ].insert( &ACPKMN( 0, PLAYER ) );

            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, PLAYER ) ].insert( &ACPKMN( 1, OPPONENT ) );
            if( ACPKMNSTS( 1, PLAYER ) != KO && ACPKMNSTS( 1, OPPONENT ) != KO
                && ACPKMNSTS( 1, PLAYER ) != NA && ACPKMNSTS( 1, OPPONENT ) != NA )
                _participatedPKMN[ &ACPKMN( 1, OPPONENT ) ].insert( &ACPKMN( 1, PLAYER ) );
        }
    }

    /**
    *  @brief Distrobutes EXP.
    *  @param p_opponent: true iff the opponent's PKMN fainted.
    *  @param p_pokemonPos: Position of the fainted PKMN (0 or 1)
    */
    void battle::distributeEXP( bool p_opponent, u8 p_pokemonPos ) {
        auto& receivingPKMN = _participatedPKMN[ &ACPKMN( p_pokemonPos, p_opponent ) ];

        float wildModifer = m_isWildBattle ? 1 : 1.5;
        POKEMON::PKMNDATA::pokemonData p;
        POKEMON::PKMNDATA::getAll( ACPKMN( p_pokemonPos, p_opponent ).m_boxdata.m_speciesId, p );
        u16 b = p.m_EXPYield;

        for( auto i : receivingPKMN ) {
            if( i->m_stats.m_acHP ) {
                if( i->m_Level == 100 )
                    continue;
                float e = ( i->m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                u8 L = i->m_Level;

                float t = ( i->m_boxdata.m_oTId == SAV.m_Id && i->m_boxdata.m_oTSid == SAV.m_Sid ? 1 : 1.5 );

                u32 exp = u32( ( wildModifer * t* b* e* L ) / 7 );

                i->m_boxdata.m_experienceGained += exp;

                //Distribute EV
                auto acItem = i->m_boxdata.m_holdItem;
                auto hasPKRS = i->m_boxdata.m_pokerus;

                u8 multiplier = ( 1 << ( hasPKRS + ( acItem == I_MACHO_BRACE ) ) );

                i->m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2 + 4 * ( acItem == I_POWER_WEIGHT ) ) );
                i->m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4 * ( acItem == I_POWER_BRACER ) ) );
                i->m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * ( acItem == I_POWER_BELT ) ) );
                i->m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2 + 4 * ( acItem == I_POWER_ANKLET ) ) );
                i->m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4 * ( acItem == I_POWER_LENS ) ) );
                i->m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2 + 4 * ( acItem == I_POWER_BAND ) ) );
            }
        }

        _battleUI.applyEXPChanges( ); // Checks also for level-advancement of 1st (and in Doubles 2nd) PKMN

        if( SAV.m_EXPShareEnabled && !p_opponent ) {
            log( L"Der EP-Teiler wirkt![A]" );
            for( u8 i = ( ( m_battleMode == DOUBLE ) ? 2 : 1 ); i < 6; ++i )if( ACPKMNSTS( i, PLAYER ) != KO &&
                                                                                ACPKMNSTS( i, PLAYER ) != NA ) {

                if( ACPKMN( i, PLAYER ).m_Level == 100 )
                    continue;

                auto& acPkmn = ACPKMN( i, PLAYER );

                float e = ( acPkmn.m_boxdata.m_holdItem == I_LUCKY_EGG ) ? 1.5 : 1;

                u8 L = acPkmn.m_Level;

                float t = ( acPkmn.m_boxdata.m_oTId == SAV.m_Id && acPkmn.m_boxdata.m_oTSid == SAV.m_Sid ? 1 : 1.5 );

                u32 exp = u32( ( wildModifer * t* b* e* L ) / 7 );

                //Half values through EXP-Share
                acPkmn.m_boxdata.m_experienceGained += exp / 2;

                //Distribute EV
                //Check for EV-enhancing stuff
                auto acItem = acPkmn.m_boxdata.m_holdItem;
                auto hasPKRS = acPkmn.m_boxdata.m_pokerus;

                u8 multiplier = ( 1 << ( hasPKRS + ( acItem == I_MACHO_BRACE ) ) );

                acPkmn.m_boxdata.m_effortValues[ 0 ] += ( multiplier * ( p.m_EVYield[ 0 ] / 2 + 4 * ( acItem == I_POWER_WEIGHT ) ) );
                acPkmn.m_boxdata.m_effortValues[ 1 ] += ( multiplier * ( p.m_EVYield[ 1 ] / 2 + 4 * ( acItem == I_POWER_BRACER ) ) );
                acPkmn.m_boxdata.m_effortValues[ 2 ] += ( multiplier * ( p.m_EVYield[ 2 ] / 2 + 4 * ( acItem == I_POWER_BELT ) ) );
                acPkmn.m_boxdata.m_effortValues[ 3 ] += ( multiplier * ( p.m_EVYield[ 3 ] / 2 + 4 * ( acItem == I_POWER_ANKLET ) ) );
                acPkmn.m_boxdata.m_effortValues[ 4 ] += ( multiplier * ( p.m_EVYield[ 4 ] / 2 + 4 * ( acItem == I_POWER_LENS ) ) );
                acPkmn.m_boxdata.m_effortValues[ 5 ] += ( multiplier * ( p.m_EVYield[ 5 ] / 2 + 4 * ( acItem == I_POWER_BAND ) ) );

                //Check for level-advancing

                POKEMON::PKMNDATA::getAll( acPkmn.m_boxdata.m_speciesId, p );

                bool newLevel = POKEMON::EXP[ L ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                u16 HPdif = acPkmn.m_stats.m_maxHP - acPkmn.m_stats.m_acHP;

                while( newLevel ) {
                    acPkmn.m_Level++;

                    if( acPkmn.m_boxdata.m_speciesId != 292 ) //Check for Ninjatom
                        acPkmn.m_stats.m_maxHP = ( ( acPkmn.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPkmn.m_Level / 100 ) + 10;
                    else
                        acPkmn.m_stats.m_maxHP = 1;
                    POKEMON::pkmnNatures nature = acPkmn.m_boxdata.getNature( );

                    acPkmn.m_stats.m_Atk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_attack + 2 * p.m_bases[ ATK + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ ATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 ) * POKEMON::NatMod[ nature ][ ATK ];
                    acPkmn.m_stats.m_Def = ( ( ( acPkmn.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ DEF + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ DEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ DEF ];
                    acPkmn.m_stats.m_Spd = ( ( ( acPkmn.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ SPD + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SPD + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ SPD ];
                    acPkmn.m_stats.m_SAtk = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ SATK + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SATK + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ SATK ];
                    acPkmn.m_stats.m_SDef = ( ( ( acPkmn.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ SDEF + 1 ]
                        + ( acPkmn.m_boxdata.m_effortValues[ SDEF + 1 ] >> 2 ) )*acPkmn.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ SDEF ];

                    acPkmn.m_stats.m_acHP = acPkmn.m_stats.m_maxHP - HPdif;

                    std::swprintf( wbuffer, 50, L"%ls erreicht Level %d.[A]", acPkmn.m_boxdata.m_name, acPkmn.m_Level );
                    log( wbuffer );

                    u8 oldSpec = acPkmn.m_boxdata.m_speciesId;
                    checkForAttackLearn( i );
                    checkForEvolution( PLAYER, i );
                    if( oldSpec != acPkmn.m_boxdata.m_speciesId )
                        checkForAttackLearn( i );

                    newLevel = acPkmn.m_Level < 100 && POKEMON::EXP[ acPkmn.m_Level ][ p.m_expType ] <= acPkmn.m_boxdata.m_experienceGained;
                }
            }
        }
    }

    /**
     *  @brief Checks if the own PKMN at position p_pokemonPos
     *  @param p_pokemonPos: Position of the PKMN which coul learn new moves (0 to 5)
     */
    void battle::checkForAttackLearn( u8 p_pokemonPos ) {
        const int MAX_ATTACKS_PER_LEVEL = 10;
        u16 learnable[ MAX_ATTACKS_PER_LEVEL ];
        auto& acPkmn = ACPKMN( p_pokemonPos, PLAYER );

        POKEMON::PKMNDATA::getLearnMoves( acPkmn.m_boxdata.m_speciesId, acPkmn.m_Level, acPkmn.m_Level, 1, MAX_ATTACKS_PER_LEVEL, learnable );

        for( u8 i = 0; i < MAX_ATTACKS_PER_LEVEL; ++i ) {
            if( !learnable[ i ] )
                break;
            _battleUI.learnMove( p_pokemonPos, learnable[ i ] );
        }

    }

    /**
    *  @brief Tests if the specified PKMN can perform an evolution, and performs it, if possible.
    *  @param p_opponent: true iff the opponent's PKMN shall be tested.
    *  @param p_pokemonPos: Position of the PKMN to be tested (0 or 1)
    */
    void battle::checkForEvolution( bool p_opponent, u8 p_pokemonPos ) {
        if( !SAV.m_evolveInBattle )
            return;

        if( ACPKMN( p_pokemonPos, p_opponent ).canEvolve( ) ) {
            auto& acPkmn = ACPKMN( p_pokemonPos, p_opponent );

            std::swprintf( wbuffer, 50, L"%ls entwickelt sich[A]", acPkmn.m_boxdata.m_name );
            log( wbuffer );

            acPkmn.evolve( );
            _battleUI.evolvePKMN( p_opponent, p_pokemonPos );

            std::swprintf( wbuffer, 50, L"und wurde zu einem %ls![A]", acPkmn.m_boxdata.m_name );
            log( wbuffer );
        }
    }

    /**
     *  @brief Checks if the battle has to end
     *  @param p_battleEndReason [in/out]: Stores the reason for an end of the battle.
     *  @returns True iff the battle has to end
     */
    bool battle::endConditionHit( battleEndReason& p_battleEndReason ) {
        //Check round limit
        if( _round >= _maxRounds ) {
            p_battleEndReason = battleEndReason::ROUND_LIMIT;
            return true;
        }

        //Check amount of non-koed PKMN
        //PLAYER
        u8 pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( _player->m_pkmnTeam->size( ) > i ) {
                if( ACPKMNSTS( i, PLAYER ) != KO )
                    pkmnCnt++;
            } else
                break;
        }
        if( !pkmnCnt ) {
            p_battleEndReason = battleEndReason::OPPONENT_WON;
            return true;
        }

        //OPPONENT
        pkmnCnt = 0;
        for( u8 i = 0; i < 6; ++i ) {
            if( _opponent->m_pkmnTeam->size( ) > i ) {
                if( ACPKMNSTS( i, OPPONENT ) != KO )
                    pkmnCnt++;
            } else
                break;
        }
        if( !pkmnCnt ) {
            p_battleEndReason = battleEndReason::PLAYER_WON;
            return true;
        }

        return false;
    }

    /**
     *  @brief Ends the battle.
     *  @param p_battleEndReason: Reason for the end of battle.
     */
    void battle::endBattle( battleEndReason p_battleEndReason ) {
        switch( p_battleEndReason ) {
            case BATTLE::battle::ROUND_LIMIT:
                log( L"Das Rundenlimit dieses\nKampfes wurde erreicht.[A][CLEAR]Der Kampf endet in einem\nUnentschieden![A]" );
                break;
            case BATTLE::battle::OPPONENT_WON:
            {
                std::swprintf( wbuffer, 100, L"[TRAINER] [TCLASS] gewinnt...[A][CLEAR]%s[A]",
                               _opponent->getWinMsg( ) );
                log( wbuffer );
                break;
            }
            case BATTLE::battle::PLAYER_WON:
            {
                std::swprintf( wbuffer, 100, L"Du besiegst [TRAINER]\n[TCLASS]![A][CLEAR]%s[A]",
                               _opponent->getLooseMsg( ) );
                log( wbuffer );
                std::swprintf( wbuffer, 100, L"Du gewinnst %d$.[A]",
                               _opponent->getLooseMoney( ) );
                log( wbuffer );
                break;
            }
            default:
                log( L"Der Kampf endet.[A]" );
                break;
        }

        _battleUI.dinit( );
    }

    /**
     *  @brief Swithes the PKMN
     *  @param p_opponent: true iff the next opponent's PKMN is requested.
     *  @param p_pokemonPos: Position of the target PKMN (0 or 1)
     *  @param p_newPokemonPos: The new PKMNs current Pos
     */
    void battle::switchPKMN( bool p_opponent, u8 p_pokemonPos, u8 p_newPokemonPos ) {

        std::wstring acPkmnStr = L"";
        if( p_opponent )
            acPkmnStr = L"OPP" + ( p_pokemonPos + 1 );
        else
            acPkmnStr = L"OWN" + ( p_pokemonPos + 1 );

        if( p_opponent )
            std::swprintf( wbuffer, 100, L"[%ls] wurde von [TRAINER]\n([TCLASS]) auf die Bank geschickt.[A]", acPkmnStr.c_str( ) );
        else
            std::swprintf( wbuffer, 100, L"Auf die Bank [%ls]![A]", acPkmnStr.c_str( ) );
        log( wbuffer );

        _battleUI.hidePKMN( p_opponent, p_pokemonPos );

        std::swap( ACPOS( p_pokemonPos, p_opponent ), ACPOS( p_newPokemonPos, p_opponent ) );

        if( p_opponent )
            std::swprintf( wbuffer, 100, L"[TRAINER] ([TCLASS]) schickt\n[%s] in den Kampf.[A]", acPkmnStr.c_str( ) );
        else
            std::swprintf( wbuffer, 100, L"Los [%ls]![A]", acPkmnStr.c_str( ) );
        log( wbuffer );

        _battleUI.sendPKMN( p_opponent, p_pokemonPos );
    }

    /**
     *  @brief Applys weather effects.
     */
    void battle::doWeather( ) {
        if( m_weather != NO_WEATHER ) {
            if( --_weatherLength ) {
                log( _weatherMessage[ m_weather ] );
                _weatherEffects[ m_weather ].execute( *this, this );
            } else {
                log( _weatherEndMessage[ m_weather ] );
                m_weather = NO_WEATHER;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE
    //////////////////////////////////////////////////////////////////////////



























































    //OLD STUFF -> deprcated

    //void init( ) {
    //    for( int i = 5; i <= 8; ++i ) {
    //        Oam->oamBuffer[ i ].isHidden = true;
    //        swiWaitForVBlank( );
    //    }
    //    for( int i = 0; i < 4; ++i ) {
    //        Oam->oamBuffer[ 9 + 2 * i ].isHidden = true;
    //        swiWaitForVBlank( );
    //    }

    //    Oam->oamBuffer[ 20 ].isHidden = true;
    //    for( int i = 0; i < 20; ++i )
    //        Oam->oamBuffer[ 36 + i ].isHidden = true;
    //    //oamIndexS = 36;
    //    //nextAvailableTileIdxS = p_oam->oamBuffer[36].gfxIndex;
    //    //palcntS = p_oam->oamBuffer[36].palette;

    //    cust_font.setColor( 0, 0 );
    //    cust_font.setColor( 251, 1 );
    //    cust_font.setColor( 252, 2 );
    //    cust_font2.setColor( 0, 0 );
    //    cust_font2.setColor( 253, 1 );
    //    cust_font2.setColor( 254, 2 );

    //    BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    //    BG_PALETTE_SUB[ 251 ] = RGB15( 15, 15, 15 );
    //    BG_PALETTE_SUB[ 252 ] = RGB15( 3, 3, 3 );
    //    BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
    //    BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
    //    BG_PALETTE[ 250 ] = RGB15( 31, 31, 31 );
    //    BG_PALETTE[ 251 ] = RGB15( 15, 15, 15 );
    //    BG_PALETTE[ 252 ] = RGB15( 3, 3, 3 );
    //    BG_PALETTE[ 253 ] = RGB15( 15, 15, 15 );
    //    BG_PALETTE[ 254 ] = RGB15( 31, 31, 31 );
    //    FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );

    //    updateOAMSub( Oam );
    //}
    //void clear( ) {
    //    FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, (u8)250 );
    //}
    //void dinit( ) {

    //    drawSub( );

    //    for( int i = 5; i < 8; ++i )
    //        Oam->oamBuffer[ 31 + 2 * i ].isHidden = false;
    //    for( int i = 0; i < 4; ++i ) {
    //        Oam->oamBuffer[ 9 + 2 * i ].isHidden = false;
    //        swiWaitForVBlank( );
    //    }
    //    updateOAMSub( Oam );
    //}



    //
    //    void initinitBattleScrnSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, int p_ownPok, int p_oppPok ) {
    //        oamIndex = palcnt = nextAvailableTileIdx = 0;
    //
    //        SpriteInfo * type1Info = &p_spriteInfo[ oamIndex ];
    //        SpriteEntry * type1 = &p_oam->oamBuffer[ oamIndex ];
    //        type1Info->m_oamId = oamIndex;
    //        type1Info->m_width = 16;
    //        type1Info->m_height = 16;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = 0;
    //        type1->isRotateScale = false;
    //        type1->isHidden = false;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = 0;
    //        type1->size = OBJSIZE_16;
    //        type1->gfxIndex = nextAvailableTileIdx;
    //        type1->priority = OBJPRIORITY_0;
    //        type1->palette = palcnt;
    //
    //        for( int i = 0; i < 11; ++i ) {
    //            p_spriteInfo[ ++oamIndex ] = *type1Info;
    //            p_oam->oamBuffer[ oamIndex ] = *type1;
    //            p_oam->oamBuffer[ oamIndex ].x = i < 5 ? 16 + i * 16 : 256 - ( i - 4 ) * 16;
    //            p_oam->oamBuffer[ oamIndex ].isHidden = i < 5 ? p_oppPok < i : p_ownPok + 5 < i;
    //            p_oam->oamBuffer[ oamIndex ].y = i >= 5 ? 192 - 16 : 0;
    //        }
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Pal, &SPRITE_PALETTE[ ( palcnt++ ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall1TilesLen );
    //        nextAvailableTileIdx += BattleBall1TilesLen / BYTES_PER_16_COLOR_TILE;
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Pal, &SPRITE_PALETTE[ ( palcnt++ ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall2TilesLen );
    //        nextAvailableTileIdx += BattleBall2TilesLen / BYTES_PER_16_COLOR_TILE;
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Pal, &SPRITE_PALETTE[ ( palcnt++ ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall3TilesLen );
    //        nextAvailableTileIdx += BattleBall3TilesLen / BYTES_PER_16_COLOR_TILE;
    //}

    //    void drawTopBack( );
    //
    //    void battle::initBattleScreen( ) {
    //        sprintf( buffer, "%i.raw", _opponent->m_trainerClass );
    //
    //        FS::loadPicture( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD" );
    //        dmaCopy( BorderBitmap, bgGetGfxPtr( bg2sub ), 256 * 192 );
    //        dmaCopy( BorderPal, BG_PALETTE_SUB, 256 * 2 );
    //        drawTopBack( );
    //        initinitBattleScrnSprites( OamTop, spriteInfoTop, 6, 6 );
    //
    //        for( int i = 0; i < 6; ++i )
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 32;
    //                    OamTop->oamBuffer[ i ].palette++;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 16;
    //                    OamTop->oamBuffer[ i ].palette += 2;
    //                    break;
    //                default:
    //                    break;
    //        }
    //        for( int i = 6; i < 12; ++i )
    //            switch( _acPkmnStatus[ _acPkmnPosition[ i - 6 ][ PLAYER ] ][ PLAYER ] ) {
    //                case NA:
    //                    OamTop->oamBuffer[ i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 32;
    //                    OamTop->oamBuffer[ i ].palette++;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ i ].gfxIndex += BattleBall2TilesLen / 16;
    //                    OamTop->oamBuffer[ i ].palette += 2;
    //                    break;
    //                default:
    //                    break;
    //        }
    //        updateOAM( OamTop );
    //
    //
    //        consoleSetWindow( &Top, 0, 0, 32, 24 );
    //        consoleSelect( &Top );
    //        consoleClear( );
    //        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //        consoleClear( );
    //        consoleSetWindow( &Bottom, 2, 11, 32, 24 );
    //        consoleSelect( &Bottom );
    //
    //        cust_font.setColor( 0, 0 );
    //        cust_font.setColor( 251, 1 );
    //        cust_font.setColor( 252, 2 );
    //        cust_font2.setColor( 0, 0 );
    //        cust_font2.setColor( 253, 1 );
    //        cust_font2.setColor( 254, 2 );
    //
    //        BG_PALETTE_SUB[ 250 ] = RGB15( 31, 31, 31 );
    //        BG_PALETTE_SUB[ 251 ] = RGB15( 30, 30, 30 );
    //        BG_PALETTE_SUB[ 252 ] = RGB15( 15, 15, 15 );
    //        BG_PALETTE_SUB[ 253 ] = RGB15( 15, 15, 15 );
    //        BG_PALETTE_SUB[ 254 ] = RGB15( 31, 31, 31 );
    //        sprintf( buffer, "Eine Herausforderung von\n %s %s!", trainerclassnames[ _opponent->m_trainerClass ], _opponent->m_battleTrainerName );
    //        cust_font.printString( buffer, 16, 80, true );
    //
    //        FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n", 144, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 8; ++i )
    //            swiWaitForVBlank( );
    //        oamIndex -= 4;
    //        --palcnt;
    //        --palcnt;
    //        nextAvailableTileIdx -= 144;
    //        FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n2", 144, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 8; ++i )
    //            swiWaitForVBlank( );
    //        oamIndex -= 4;
    //        --palcnt;
    //        --palcnt;
    //        nextAvailableTileIdx -= 144;
    //        FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n3", 144, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
    //        updateOAM( OamTop );
    //        oamIndex -= 4;
    //        --palcnt;
    //        --palcnt;
    //        nextAvailableTileIdx -= 144;
    //        for( int i = 0; i < 20; ++i )
    //            swiWaitForVBlank( );
    //
    //        for( int l = 0; l < 25; ++l ) {
    //            FS::loadTrainerSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/TRAINER/", "n3", 144 + 4 * l, 16, oamIndex, palcnt, nextAvailableTileIdx, false );
    //            updateOAM( OamTop );
    //            oamIndex -= 4;
    //            --palcnt;
    //            --palcnt;
    //            nextAvailableTileIdx -= 144;
    //            for( int i = 0; i < 3; ++i )
    //                swiWaitForVBlank( );
    //        }
    //
    //        FS::loadPicture( bgGetGfxPtr( bg3sub ), "nitro:/PICS/", "ClearD" );
    //        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //        consoleClear( );
    //    }

    //    void initBattleScreenSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo ) {
    //        oamIndex = palcnt = nextAvailableTileIdx = 0;
    //
    //        SpriteInfo * type1Info = &p_spriteInfo[ OWN_HP_2 ];
    //        SpriteEntry * type1 = &p_oam->oamBuffer[ OWN_HP_2 ];
    //        type1Info->m_oamId = OWN_HP_2;
    //        type1Info->m_width = 32;
    //        type1Info->m_height = 32;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = 192 - 32 - 8;
    //        type1->isRotateScale = false;
    //        type1->isHidden = false;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = 256 - 36;
    //        type1->size = OBJSIZE_32;
    //        type1->gfxIndex = nextAvailableTileIdx;
    //        type1->priority = OBJPRIORITY_2;
    //        type1->palette = HP_PAL;
    //
    //        p_spriteInfo[ OWN_HP_1 ] = *type1Info;
    //        p_oam->oamBuffer[ OWN_HP_1 ] = *type1;
    //        p_oam->oamBuffer[ OWN_HP_1 ].x -= 88;
    //        p_oam->oamBuffer[ OWN_HP_1 ].y -= 32;
    //        p_oam->oamBuffer[ OWN_HP_1 ].priority = OBJPRIORITY_2;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle1Pal, &SPRITE_PALETTE[ (HP_PAL)* COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle1Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Battle1TilesLen );
    //        nextAvailableTileIdx += Battle1TilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //        type1Info = &p_spriteInfo[ OPP_HP_2 ];
    //        type1 = &p_oam->oamBuffer[ OPP_HP_2 ];
    //        type1Info->m_oamId = OPP_HP_2;
    //        type1Info->m_width = 32;
    //        type1Info->m_height = 32;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = 8;
    //        type1->isRotateScale = false;
    //        type1->isHidden = false;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = 0;
    //        type1->size = OBJSIZE_32;
    //        type1->gfxIndex = nextAvailableTileIdx;
    //        type1->priority = OBJPRIORITY_2;
    //        type1->palette = HP_PAL + 1;
    //
    //        p_spriteInfo[ OPP_HP_1 ] = *type1Info;
    //        p_oam->oamBuffer[ OPP_HP_1 ] = *type1;
    //        p_oam->oamBuffer[ OPP_HP_1 ].x += 88;
    //        p_oam->oamBuffer[ OPP_HP_1 ].y += 24;
    //        p_oam->oamBuffer[ OPP_HP_1 ].priority = OBJPRIORITY_2;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle2Pal, &SPRITE_PALETTE[ ( HP_PAL + 1 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Battle2Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], Battle2TilesLen );
    //        nextAvailableTileIdx += Battle2TilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //
    //        type1Info = &p_spriteInfo[ OWN_PB_START ];
    //        type1 = &p_oam->oamBuffer[ OWN_PB_START ];
    //        type1Info->m_oamId = OWN_PB_START;
    //        type1Info->m_width = 16;
    //        type1Info->m_height = 16;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = 0;
    //        type1->isRotateScale = false;
    //        type1->isHidden = true;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = 0;
    //        type1->size = OBJSIZE_16;
    //        type1->gfxIndex = nextAvailableTileIdx;
    //        type1->priority = OBJPRIORITY_0;
    //        type1->palette = PB_PAL_START;
    //
    //        for( int i = 1; i < 12; ++i ) {
    //            p_spriteInfo[ OWN_PB_START + i ] = *type1Info;
    //            p_oam->oamBuffer[ OWN_PB_START + i ] = *type1;
    //        }
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Pal, &SPRITE_PALETTE[ (PB_PAL_START)* COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall1Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall1TilesLen );
    //        nextAvailableTileIdx += BattleBall1TilesLen / BYTES_PER_16_COLOR_TILE;
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Pal, &SPRITE_PALETTE[ ( PB_PAL_START + 1 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall2Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall2TilesLen );
    //        nextAvailableTileIdx += BattleBall2TilesLen / BYTES_PER_16_COLOR_TILE;
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Pal, &SPRITE_PALETTE[ ( PB_PAL_START + 2 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleBall3Tiles, &SPRITE_GFX[ nextAvailableTileIdx * OFFSET_MULTIPLIER ], BattleBall3TilesLen );
    //        nextAvailableTileIdx += BattleBall3TilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //
    //    }

    //    void initBattleSubScreenSprites( OAMTable* p_oam, SpriteInfo* p_spriteInfo, bool p_isWild, bool p_hasPokeNav ) {
    //        oamIndexS = palcntS = nextAvailableTileIdxS = 0;
    //
    //        type1Info = &p_spriteInfo[ ++oamIndexS ];
    //        type1 = &p_oam->oamBuffer[ oamIndexS ];
    //        type1Info->m_oamId = oamIndexS;
    //        type1Info->m_width = 64;
    //        type1Info->m_height = 32;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = 144;
    //        type1->isRotateScale = false;
    //        type1->isHidden = true;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_WIDE;
    //        type1->x = 176;
    //        type1->size = OBJSIZE_64;
    //        type1->gfxIndex = nextAvailableTileIdxS;
    //        type1->priority = OBJPRIORITY_1;
    //        type1->palette = palcntS;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub5Pal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BattleSub5Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BattleSub5TilesLen );
    //        nextAvailableTileIdxS += BattleSub5TilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //        for( int i = 0; i < 4; ++i ) {
    //            SpriteInfo * MInfo = &p_spriteInfo[ ++oamIndexS ];
    //            SpriteEntry * M = &p_oam->oamBuffer[ oamIndexS ];
    //            MInfo->m_oamId = oamIndexS;
    //            MInfo->m_width = 64;
    //            MInfo->m_height = 64;
    //            MInfo->m_angle = 0;
    //            MInfo->m_entry = M;
    //            M->y = 0;
    //            M->isRotateScale = false;
    //            M->blendMode = OBJMODE_NORMAL;
    //            M->isMosaic = false;
    //            M->colorMode = OBJCOLOR_16;
    //            M->shape = OBJSHAPE_SQUARE;
    //            M->isHidden = true;
    //            M->x = (i)* 64;
    //            M->size = OBJSIZE_64;
    //            M->gfxIndex = nextAvailableTileIdxS;
    //            M->priority = OBJPRIORITY_2;
    //            M->palette = palcntS;
    //        }
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, MessagePal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, MessageTiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], MessageTilesLen );
    //        nextAvailableTileIdxS += MessageTilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //        ++palcntS;
    //
    //        Bo5 = &p_oam->oamBuffer[ ++oamIndexS ];
    //        Bo5->y = 192;
    //        Bo5->isRotateScale = false;
    //        Bo5->blendMode = OBJMODE_NORMAL;
    //        Bo5->isMosaic = false;
    //        Bo5->colorMode = OBJCOLOR_16;
    //        Bo5->shape = OBJSHAPE_SQUARE;
    //        Bo5->isHidden = true;
    //        Bo5->x = 256;
    //        Bo5->size = OBJSIZE_64;
    //        Bo5->gfxIndex = nextAvailableTileIdxS;
    //        Bo5->priority = OBJPRIORITY_2;
    //        Bo5->palette = palcntS;
    //        Bo5->vFlip = false;
    //        Bo5->hFlip = false;
    //        SpriteInfo * backInfo = &p_spriteInfo[ ++oamIndexS ];
    //        SpriteEntry * back = &p_oam->oamBuffer[ oamIndexS ];
    //        backInfo->m_oamId = oamIndexS;
    //        backInfo->m_width = 32;
    //        backInfo->m_height = 32;
    //        backInfo->m_angle = 0;
    //        backInfo->m_entry = back;
    //        back->y = SCREEN_HEIGHT - 28;
    //        back->isRotateScale = false;
    //        back->blendMode = OBJMODE_NORMAL;
    //        back->isMosaic = false;
    //        back->isHidden = true;
    //        back->colorMode = OBJCOLOR_16;
    //        back->shape = OBJSHAPE_SQUARE;
    //        back->x = SCREEN_WIDTH - 28;
    //        back->size = OBJSIZE_32;
    //        back->gfxIndex = nextAvailableTileIdxS;
    //        back->priority = OBJPRIORITY_1;
    //        back->palette = palcntS;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BackTiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], BackTilesLen );
    //        nextAvailableTileIdxS += BackTilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, BackPal, &SPRITE_PALETTE_SUB[ ( palcntS++ ) * COLORS_PER_PALETTE ], 32 );
    //
    //        ++oamIndexS;
    //        int nextnext = nextAvailableTileIdxS + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
    //        for( int i = 0; i < 6; ++i ) {
    //            SpriteInfo * C1Info = &p_spriteInfo[ 2 * i + oamIndexS ];
    //            SpriteEntry * C1 = &p_oam->oamBuffer[ 2 * i + oamIndexS ];
    //            C1Info->m_oamId = oamIndexS;
    //            C1Info->m_width = 64;
    //            C1Info->m_height = 32;
    //            C1Info->m_angle = 0;
    //            C1Info->m_entry = C1;
    //            C1->y = 68 + ( i / 2 ) * 32;
    //            C1->isRotateScale = false;
    //            C1->blendMode = OBJMODE_NORMAL;
    //            C1->isMosaic = false;
    //            C1->colorMode = OBJCOLOR_16;
    //            C1->shape = OBJSHAPE_WIDE;
    //            C1->isHidden = true;
    //            C1->x = ( ( i % 2 ) ? 32 : 128 );
    //            C1->size = OBJSIZE_64;
    //            C1->gfxIndex = nextAvailableTileIdxS;
    //
    //            C1->priority = OBJPRIORITY_2;
    //            C1->palette = palcntS;
    //
    //            SpriteInfo * C3Info = &p_spriteInfo[ 2 * i + oamIndexS + 1 ];
    //            SpriteEntry * C3 = &p_oam->oamBuffer[ 2 * i + oamIndexS + 1 ];
    //            C3Info->m_oamId = oamIndexS;
    //            C3Info->m_width = 64;
    //            C3Info->m_height = 32;
    //            C3Info->m_angle = 0;
    //            C3Info->m_entry = C3;
    //            C3->y = 68 + ( i / 2 ) * 32;
    //            C3->isRotateScale = false;
    //            C3->blendMode = OBJMODE_NORMAL;
    //            C3->isMosaic = false;
    //            C3->colorMode = OBJCOLOR_16;
    //            C3->shape = OBJSHAPE_WIDE;
    //            C3->isHidden = true;
    //            C3->x = ( ( i % 2 ) ? 62 : 160 );
    //            C3->size = OBJSIZE_64;
    //            C3->gfxIndex = nextnext;
    //            C3->priority = OBJPRIORITY_2;
    //            C3->palette = palcntS;
    //        }
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_1Pal, &SPRITE_PALETTE_SUB[ palcntS * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_1Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], Choice_1TilesLen );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_3Tiles, &SPRITE_GFX_SUB[ nextnext * OFFSET_MULTIPLIER ], Choice_3TilesLen );
    //        nextAvailableTileIdxS = nextnext + Choice_1TilesLen / BYTES_PER_16_COLOR_TILE;
    //
    //        for( int i = 0; i < 3; ++i ) {
    //            SpriteInfo * C2Info = &p_spriteInfo[ i + oamIndexS + 12 ];
    //            SpriteEntry * C2 = &p_oam->oamBuffer[ i + oamIndexS + 12 ];
    //            C2Info->m_oamId = oamIndexS;
    //            C2Info->m_width = 64;
    //            C2Info->m_height = 32;
    //            C2Info->m_angle = 0;
    //            C2Info->m_entry = C2;
    //            C2->y = 68 + (i)* 32;
    //            C2->isRotateScale = false;
    //            C2->blendMode = OBJMODE_NORMAL;
    //            C2->isMosaic = false;
    //            C2->colorMode = OBJCOLOR_16;
    //            C2->shape = OBJSHAPE_WIDE;
    //            C2->isHidden = true;
    //            C2->x = 96;
    //            C2->size = OBJSIZE_64;
    //            C2->gfxIndex = nextAvailableTileIdxS;
    //            C2->priority = OBJPRIORITY_2;
    //            C2->palette = palcntS;
    //        }
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Choice_2Tiles, &SPRITE_GFX_SUB[ nextAvailableTileIdxS * OFFSET_MULTIPLIER ], Choice_2TilesLen );
    //        nextAvailableTileIdxS += Choice_2TilesLen / BYTES_PER_16_COLOR_TILE;
    //        ++palcntS;
    //        oamIndexS += 15;
    //    }
    //
    //#define PB_ANIM_TILES 700
    //    void animatePB( int p_x, int p_y ) {
    //        p_x += 8; p_y += 8;
    //        SpriteInfo * type1Info = &spriteInfoTop[ PB_ANIM ];
    //        SpriteEntry * type1 = &OamTop->oamBuffer[ PB_ANIM ];
    //        type1Info->m_oamId = PB_ANIM;
    //        type1Info->m_width = 16;
    //        type1Info->m_height = 16;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = p_y;
    //        type1->isRotateScale = false;
    //        type1->isHidden = false;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = p_x;
    //        type1->size = OBJSIZE_16;
    //        type1->gfxIndex = PB_ANIM_TILES;
    //        type1->priority = OBJPRIORITY_0;
    //        type1->palette = 15;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall1TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall2TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall3Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall3TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall4Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall4TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall5Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall5TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall6Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall6TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall7Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall7TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall8Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall8TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall9Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall9TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall10Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall10TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall11Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], PokeBall11TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 3; ++i )
    //            swiWaitForVBlank( );
    //        type1->isHidden = true;
    //
    //        type1Info->m_oamId = PB_ANIM;
    //        type1Info->m_width = 64;
    //        type1Info->m_height = 64;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = p_y - 22;
    //        type1->isRotateScale = false;
    //        type1->isHidden = false;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = p_x - 22;
    //        type1->size = OBJSIZE_64;
    //        type1->gfxIndex = PB_ANIM_TILES;
    //        type1->priority = OBJPRIORITY_0;
    //        type1->palette = 15;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        type1->isHidden = true;
    //        updateOAM( OamTop );
    //    }
    //
    //    void animateShiny( int p_x, int p_y ) {
    //        SpriteInfo * type1Info = &spriteInfoTop[ SHINY_ANIM ];
    //        SpriteEntry * type1 = &OamTop->oamBuffer[ SHINY_ANIM ];
    //        type1Info->m_oamId = SHINY_ANIM;
    //        type1Info->m_width = 64;
    //        type1Info->m_height = 64;
    //        type1Info->m_angle = 0;
    //        type1Info->m_entry = type1;
    //        type1->y = p_y;
    //        type1->isRotateScale = false;
    //        type1->isHidden = false;
    //        type1->blendMode = OBJMODE_NORMAL;
    //        type1->isMosaic = false;
    //        type1->colorMode = OBJCOLOR_16;
    //        type1->shape = OBJSHAPE_SQUARE;
    //        type1->x = p_x;
    //        type1->size = OBJSIZE_64;
    //        type1->gfxIndex = PB_ANIM_TILES;
    //        type1->priority = OBJPRIORITY_0;
    //        type1->palette = 15;
    //
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny1TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ ( 15 ) * COLORS_PER_PALETTE ], 32 );
    //        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ PB_ANIM_TILES * OFFSET_MULTIPLIER ], Shiny2TilesLen );
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 2; ++i )
    //            swiWaitForVBlank( );
    //        type1->isHidden = true;
    //        updateOAM( OamTop );
    //    }
    ////
    //    void battle::switchOppPkmn( int p_newPok, int p_toSwitch ) {
    //        if( ACPKMN( p_newPok, OPPONENT ).m_stats.m_acHP == 0 )
    //            return;
    //
    //        init( );
    //
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //        if( ACPKMN( p_toSwitch, OPPONENT ).m_stats.m_acHP ) {
    //            clear( );
    //            sprintf( buffer, "%ls wird von\n%s %s\nauf die Bank geschickt. ", ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_Name,
    //                     trainerclassnames[ _opponent->m_trainerClass ], _opponent->m_battleTrainerName );
    //            cust_font.printString( buffer, 8, 8, true );
    //        }
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ); i < 6; ++i )
    //            if( i == p_newPok || i == p_toSwitch ) {
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case KO:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette--;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette -= 2;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //            }
    //
    //        std::swap( ACPOS( p_newPok, OPPONENT ), ACPOS( p_toSwitch, OPPONENT ) );
    //
    //        for( int i = 0; i < 150; ++i )
    //            swiWaitForVBlank( );
    //
    //        clear( );
    //        sprintf( buffer, "%s %s\nschickt %ls in den Kampf.",
    //                 trainerclassnames[ _opponent->m_trainerClass ],
    //                 _opponent->m_battleTrainerName,
    //                 ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //
    //        consoleSelect( &Top );
    //
    //        if( p_toSwitch == 0 ) {
    //            oamIndex = OPP_PKMN_1_START;
    //            palcnt = OPP_PKMN_1_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //
    //            OamTop->oamBuffer[ OPP_PB_START ].isHidden = true;
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_1_START + i ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 206, 50 );
    //
    //            OamTop->oamBuffer[ OPP_PB_START ].isHidden = false;
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop,
    //                "nitro:/PICS/SPRITES/PKMN/",
    //                ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //                176,
    //                20,
    //                oamIndex,
    //                palcnt,
    //                nextAvailableTileIdx,
    //                false,
    //                ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //                ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //                oamIndex = OPP_PKMN_1_START;
    //                palcnt = OPP_PKMN_1_PAL;
    //                nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //                loadPKMNSprite( OamTop,
    //                                spriteInfoTop,
    //                                "nitro:/PICS/SPRITES/PKMN/",
    //                                ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //                                176,
    //                                20,
    //                                oamIndex,
    //                                palcnt,
    //                                nextAvailableTileIdx,
    //                                false,
    //                                ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //                                !ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale );
    //            }
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_1_START + i ].isHidden = false;
    //            updateOAM( OamTop );
    //
    //            if( ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ) )
    //                animateShiny( 176 + 16, 36 );
    //
    //            displayHP( 100, 101, 88, 32, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP,
    //                       88, 32, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            OamTop->oamBuffer[ OPP_PB_START ].x = 96;
    //            OamTop->oamBuffer[ OPP_PB_START ].y = 41;
    //            consoleSetWindow( &Top, 0, 5, 20, 2 );
    //            consoleClear( );
    //
    //            printf( "%10ls%c\n",
    //                    ACPKMN( 0, OPPONENT ).m_boxdata.m_Name,
    //                    GENDER( ( *_player->m_pkmnTeam )[ ACPOS( 0, OPPONENT ) ] ) );
    //
    //            if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //                printf( " " );
    //            if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //                printf( " " );
    //            printf( "Lv%d%4dKP", ACPKMN( 0, OPPONENT ).m_Level,
    //                    ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //        } else {
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //            consoleSelect( &Top );
    //            oamIndex = OPP_PKMN_2_START;
    //            palcnt = OPP_PKMN_2_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_2_START + i ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 142, 34 );
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = false;
    //
    //            if( !loadPKMNSprite( OamTop,
    //                spriteInfoTop,
    //                "nitro:/PICS/SPRITES/PKMN/",
    //                ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC,
    //                112,
    //                4,
    //                oamIndex,
    //                palcnt,
    //                nextAvailableTileIdx,
    //                false,
    //                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ),
    //                ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //                oamIndex = OPP_PKMN_2_START;
    //                palcnt = OPP_PKMN_2_PAL;
    //                nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //                loadPKMNSprite( OamTop,
    //                                spriteInfoTop,
    //                                "nitro:/PICS/SPRITES/PKMN/",
    //                                ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC,
    //                                112,
    //                                4,
    //                                oamIndex,
    //                                palcnt,
    //                                nextAvailableTileIdx,
    //                                false,
    //                                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ),
    //                                !ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale );
    //            }
    //
    //            for( int i = 1; i <= 4; ++i )
    //                OamTop->oamBuffer[ OPP_PKMN_2_START + i ].isHidden = false;
    //            updateOAM( OamTop );
    //
    //            if( ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ) )
    //                animateShiny( 112 + 16, 20 );
    //
    //            updateOAM( OamTop );
    //
    //            displayHP( 100, 101, 0, 8, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP, 0, 8, HP_COL( 1, p_toSwitch ), HP_COL( 1, p_toSwitch ) + 1, false );
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].x = 8;
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].y = 17;
    //            consoleSetWindow( &Top, 4, 2, 20, 2 );
    //            consoleClear( );
    //            printf( "%ls%c\nLv%d%4dKP",
    //                    ACPKMN( 1, OPPONENT ).m_boxdata.m_Name,
    //                    GENDER( ACPKMN( 1, OPPONENT ) ),
    //                    ACPKMN( 1, OPPONENT ).m_Level,
    //                    ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //        }
    //
    //        consoleSelect( &Bottom );
    //
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ); i < 6; ++i )
    //            if( i == p_newPok || i == p_toSwitch ) {
    //            OamTop->oamBuffer[ OPP_PB_START + i ].x = -4 + 18 * i;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].y = -4;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette++;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette += 2;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //            }
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 200; ++i )
    //            swiWaitForVBlank( );
    //
    //        if( abilities[ ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls (Gegn.) wirkt!\n",
    //                     abilities[ ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( p_toSwitch, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //    }
    //
    //    void battle::switchOwnPkmn( int p_newPok, int p_toSwitch ) {
    //        if( ACPKMN( p_newPok, PLAYER ).m_stats.m_acHP == 0 )
    //            return;
    //        init( );
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ); i < 6; ++i ) {
    //            if( i == p_newPok || i == p_toSwitch ) {
    //                OamTop->oamBuffer[ OWN_PB_START + i ].x = 236 - 18 * i;
    //                OamTop->oamBuffer[ OWN_PB_START + i ].y = 196 - 16;
    //                OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = false;
    //                switch( ACPKMNSTS( i, PLAYER ) ) {
    //                    case KO:
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].palette--;
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 32;
    //                        break;
    //                    case STS:
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].palette -= 2;
    //                        OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex -= BattleBall1TilesLen / 16;
    //                        break;
    //                    default:
    //                        break;
    //                }
    //            }
    //        }
    //        updateOAM( OamTop );
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //        if( ACPKMN( p_toSwitch, PLAYER ).m_stats.m_acHP ) {
    //            clear( );
    //            sprintf( buffer, "Auf die Bank,\n %ls!", ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //        }
    //        std::swap( ACPOS( p_newPok, PLAYER ), ACPOS( p_toSwitch, PLAYER ) );
    //
    //        for( int i = 0; i < 100; ++i )
    //            swiWaitForVBlank( );
    //
    //
    //        consoleSelect( &Bottom );
    //        clear( );
    //        sprintf( buffer, "Los %ls!", ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //
    //        consoleSelect( &Top );
    //        if( p_toSwitch == 0 ) {
    //            oamIndex = OWN_PKMN_1_START;
    //            palcnt = OWN_PKMN_1_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //
    //            OamTop->oamBuffer[ OWN_PB_START ].isHidden = true;
    //            for( int i = 0; i < 4; ++i )
    //                OamTop->oamBuffer[ OWN_PKMN_1_START + i + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 80, 170 );
    //
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale ) )
    //                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale );
    //
    //            OamTop->oamBuffer[ OWN_PB_START ].isHidden = false;
    //
    //            updateOAM( OamTop );
    //
    //            if( ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ) )
    //                animateShiny( 6, 116 );
    //
    //            displayHP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP,
    //                       256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayEP( 100, 100, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //
    //            POKEMON::PKMNDATA::getAll( ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //            displayEP( 0, ( ACPKMN( 0, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                       ( POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                       256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //            OamTop->oamBuffer[ OWN_PB_START ].x = 256 - 88 - 32 + 4;
    //            OamTop->oamBuffer[ OWN_PB_START ].y = 192 - 31 - 32;
    //            consoleSetWindow( &Top, 21, 16, 20, 4 );
    //            consoleClear( );
    //            printf( "%ls%c\nLv%d%4dKP",
    //                    ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                    GENDER( ACPKMN( 0, PLAYER ) ),
    //                    ACPKMN( 0, PLAYER ).m_Level,
    //                    ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //
    //            //OamTop->oamBuffer[OWN_PB_START + 1].isHidden = OamTop->oamBuffer[2].isHidden = false;
    //            updateOAM( OamTop );
    //        } else {
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //            consoleSelect( &Top );
    //            oamIndex = OWN_PKMN_2_START;
    //            palcnt = OWN_PKMN_2_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //
    //            for( int i = 0; i < 4; ++i ) //hide pokemon sprite
    //                OamTop->oamBuffer[ OWN_PKMN_2_START + i + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //
    //            animatePB( 142, 34 );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].isHidden = false;
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale ) )
    //                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale );
    //
    //            if( ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ) )
    //                animateShiny( 50 + 16, 136 );
    //
    //            updateOAM( OamTop );
    //
    //            displayHP( 100, 101, 256 - 36, 192 - 40, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                       256 - 36, 192 - 40, HP_COL( 0, p_toSwitch ), HP_COL( 0, p_toSwitch ) + 1, false );
    //            displayEP( 100, 100, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //            POKEMON::PKMNDATA::getAll( ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //            displayEP( 0,
    //                       ( ACPKMN( 1, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                       ( POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                       256 - 36,
    //                       192 - 40,
    //                       OWN2_EP_COL,
    //                       OWN2_EP_COL,
    //                       false );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].x = 256 - 32 + 4;
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].y = 192 - 31;
    //            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //            consoleClear( );
    //            printf( "%10ls%c\n",
    //                    ACPKMN( 1, PLAYER ).m_boxdata.m_Name,
    //                    GENDER( ACPKMN( 1, PLAYER ) ) );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                printf( " " );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                printf( " " );
    //            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //            //OamTop->oamBuffer[5].isHidden = OamTop->oamBuffer[1].isHidden = false;
    //            updateOAM( OamTop );
    //        }
    //
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ); i < 6; ++i )
    //            if( i == p_newPok || i == p_toSwitch ) {
    //            OamTop->oamBuffer[ OWN_PB_START + i ].x = 236 - 18 * i;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].y = 196 - 16;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, PLAYER ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette++;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette += 2;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //            }
    //
    //        for( int i = 0; i < 100; ++i )
    //            swiWaitForVBlank( );
    //        updateOAM( OamTop );
    //
    //        if( abilities[ ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls wirkt!\n",
    //                     abilities[ ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( p_toSwitch, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //    }
    //
    //    void setMainBattleVisibility( bool p_hidden ) {
    //        for( int i = 0; i <= 4; ++i )
    //            Oam->oamBuffer[ i ].isHidden = p_hidden;
    //        Oam->oamBuffer[ 2 ].isHidden |= !p_hidden & !SAV.m_activatedPNav;
    //        for( int i = 19; i <= 19; ++i )
    //            Oam->oamBuffer[ i ].isHidden = p_hidden;
    //        updateOAMSub( Oam );
    //    }
    //
    //
    //    void drawTopBack( ) {
    //        dmaCopy( TestBattleBackBitmap, bgGetGfxPtr( bg3 ), 256 * 256 );
    //        dmaCopy( TestBattleBackPal, BG_PALETTE, 128 * 2 );
    //    }
    //


    //    void battle::initBattleScene( int p_battleBack, weather p_weather ) {
    //        for( size_t i = 0; i < 6; ++i ) {
    //            ACPOS( i, PLAYER ) = ACPOS( i, OPPONENT ) = i;
    //            if( _player->m_pkmnTeam->size( ) > i ) {
    //                if( ACPKMN( i, PLAYER ).m_stats.m_acHP == 0 || ACPKMN( i, PLAYER ).m_boxdata.m_IV.m_isEgg )
    //                    ACPKMNSTS( i, PLAYER ) = KO;
    //                else if( ACPKMN( i, PLAYER ).m_statusint )
    //                    ACPKMNSTS( i, PLAYER ) = STS;
    //                else
    //                    ACPKMNSTS( i, PLAYER ) = OK;
    //            } else
    //                ACPKMNSTS( i, PLAYER ) = NA;
    //            if( _opponent->m_pkmnTeam->size( ) > i ) {
    //                if( ACPKMN( i, OPPONENT ).m_stats.m_acHP == 0 || ACPKMN( i, OPPONENT ).m_boxdata.m_IV.m_isEgg )
    //                    ACPKMNSTS( i, OPPONENT ) = KO;
    //                else if( ACPKMN( i, OPPONENT ).m_statusint )
    //                    ACPKMNSTS( i, OPPONENT ) = STS;
    //                else ACPKMNSTS( i, OPPONENT ) = OK;
    //            } else
    //                ACPKMNSTS( i, OPPONENT ) = NA;
    //        }
    //        if( ACPKMNSTS( 0, PLAYER ) == KO ) {
    //            for( size_t i = 1 + ( m_battleMode == DOUBLE ); i < _player->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                    std::swap( ACPOS( 0, PLAYER ), ACPOS( i, PLAYER ) );
    //                    break;
    //                }
    //            }
    //        }
    //        if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, PLAYER ) == KO ) {
    //            for( size_t i = 2; i < _player->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                    std::swap( ACPOS( 1, PLAYER ), ACPOS( i, PLAYER ) );
    //                    break;
    //                }
    //            }
    //        }
    //        if( ACPKMNSTS( 0, OPPONENT ) == KO ) {
    //            for( size_t i = 1 + ( m_battleMode == DOUBLE ); i < _opponent->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                    std::swap( ACPOS( 0, OPPONENT ), ACPOS( i, OPPONENT ) );
    //                    break;
    //                }
    //            }
    //        }
    //        if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, OPPONENT ) == KO ) {
    //            for( size_t i = 2; i < _opponent->m_pkmnTeam->size( ); ++i ) {
    //                if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                    std::swap( ACPOS( 1, OPPONENT ), ACPOS( i, OPPONENT ) );
    //                    break;
    //                }
    //            }
    //        }
    //        vramSetup( );
    //        videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    //        drawTopBack( );
    //        Top = *consoleInit( &Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
    //        consoleSetFont( &Top, &cfont );
    //
    //        Bottom = *consoleInit( &Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
    //        consoleSetFont( &Bottom, &cfont );
    //
    //        //touchPosition t;
    //
    //        initOAMTableSub( Oam );
    //        initOAMTable( OamTop );
    //        _round = _maxRounds;
    //        initBattleScreen( );
    //        drawTopBack( );
    //
    //        drawSub( );
    //
    //        drawTopBack( );
    //
    //        initOAMTableSub( Oam );
    //        initOAMTable( OamTop );
    //        initBattleScreenSprites( OamTop, spriteInfoTop );
    //
    //        initBattleSubScreenSprites( Oam, spriteInfo, false, SAV.m_activatedPNav );
    //        for( int i = 5; i <= 19; ++i )
    //            Oam->oamBuffer[ i ].isHidden = false;
    //        updateOAMSub( Oam );
    //
    //        for( int i = 1; i <= 4; ++i )
    //            OamTop->oamBuffer[ i ].isHidden = true;
    //        updateOAM( OamTop );
    //
    //        init( );
    //        //Opp's Side
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //
    //        if( ( m_battleMode == DOUBLE ) && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO )
    //            sprintf( buffer, "%s %s\nschickt %ls ",
    //            trainerclassnames[ _opponent->m_trainerClass ],
    //            _opponent->m_battleTrainerName,
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //        else
    //            sprintf( buffer, "%s %s\nschickt %ls in den Kampf.",
    //            trainerclassnames[ _opponent->m_trainerClass ],
    //            _opponent->m_battleTrainerName,
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //        consoleSelect( &Top );
    //
    //        animatePB( 206, 50 );
    //
    //        OamTop->oamBuffer[ OPP_HP_1 ].isHidden = OamTop->oamBuffer[ OPP_PB_START ].isHidden = false;
    //        oamIndex = OPP_PKMN_1_START;
    //        palcnt = OPP_PKMN_1_PAL;
    //        nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //
    //        if( !loadPKMNSprite( OamTop,
    //            spriteInfoTop,
    //            "nitro:/PICS/SPRITES/PKMN/",
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //            176,
    //            20,
    //            oamIndex,
    //            palcnt,
    //            nextAvailableTileIdx,
    //            false,
    //            ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //            ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //
    //            oamIndex = OPP_PKMN_1_START;
    //            palcnt = OPP_PKMN_1_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_1_TILE;
    //            loadPKMNSprite( OamTop,
    //                            spriteInfoTop,
    //                            "nitro:/PICS/SPRITES/PKMN/",
    //                            ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC,
    //                            176,
    //                            20,
    //                            oamIndex,
    //                            palcnt,
    //                            nextAvailableTileIdx,
    //                            false,
    //                            ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ),
    //                            !ACPKMN( 0, OPPONENT ).m_boxdata.m_isFemale );
    //        }
    //        if( ACPKMN( 0, OPPONENT ).m_boxdata.isShiny( ) )
    //            animateShiny( 176 + 16, 36 );
    //
    //        displayHP( 100, 101, 88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, false );
    //        displayHP( 100, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP, 88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, false );
    //        OamTop->oamBuffer[ OPP_PB_START ].x = 96;
    //        OamTop->oamBuffer[ OPP_PB_START ].y = 41;
    //        consoleSetWindow( &Top, 0, 5, 20, 5 );
    //
    //        printf( "%10ls%c\n", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name, GENDER( ( *_player->m_pkmnTeam )[ ACPOS( 0, OPPONENT ) ] ) );
    //        if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //            printf( " " );
    //        if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //            printf( " " );
    //        printf( "Lv%d%4dKP", ACPKMN( 0, OPPONENT ).m_Level,
    //                ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //
    //        if( ( m_battleMode == DOUBLE ) && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ) {
    //            OamTop->oamBuffer[ OPP_HP_2 ].isHidden = OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = true;
    //            updateOAM( OamTop );
    //            for( int i = 0; i < 80; ++i )
    //                swiWaitForVBlank( );
    //            consoleSelect( &Bottom );
    //            clear( );
    //            sprintf( buffer, "und %ls in den Kampf.", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            consoleSelect( &Top );
    //
    //            animatePB( 142, 34 );
    //            OamTop->oamBuffer[ OPP_HP_2 ].isHidden = OamTop->oamBuffer[ OPP_PB_START + 1 ].isHidden = false;
    //            oamIndex = OPP_PKMN_2_START;
    //            palcnt = OPP_PKMN_2_PAL;
    //            nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //
    //            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC, 112, 4, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ), ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale ) ) {
    //                oamIndex = OPP_PKMN_2_START;
    //                palcnt = OPP_PKMN_2_PAL;
    //                nextAvailableTileIdx = OPP_PKMN_2_TILE;
    //                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", ACPKMN( 1, OPPONENT ).m_boxdata.m_SPEC, 112, 4, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ), !ACPKMN( 1, OPPONENT ).m_boxdata.m_isFemale );
    //            }
    //            if( ACPKMN( 1, OPPONENT ).m_boxdata.isShiny( ) )
    //                animateShiny( 112 + 16, 20 );
    //
    //
    //            displayHP( 100, 101, 0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP, 0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, false );
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].x = 8;
    //            OamTop->oamBuffer[ OPP_PB_START + 1 ].y = 17;
    //            consoleSetWindow( &Top, 4, 2, 20, 5 );
    //            printf( "%ls%c\nLv%d%4dKP", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name, GENDER( ( *_player->m_pkmnTeam )[ ACPOS( 1, OPPONENT ) ] ),
    //                    ACPKMN( 1, OPPONENT ).m_Level, ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //        }
    //
    //        consoleSelect( &Bottom );
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _opponent->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, OPPONENT ) != KO ); i < 6; ++i ) {
    //            OamTop->oamBuffer[ OPP_PB_START + i ].x = -4 + 18 * i;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].y = -4;
    //            OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, OPPONENT ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette = PB_PAL_START + 1;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].palette = PB_PAL_START + 2;
    //                    OamTop->oamBuffer[ OPP_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //        }
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 80; ++i )
    //            swiWaitForVBlank( );
    //
    //        if( abilities[ ACPKMN( 0, OPPONENT ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls (Gegn.) wirkt!\n",
    //                     abilities[ ACPKMN( 0, OPPONENT ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //        if( ( m_battleMode == DOUBLE ) && abilities[ ACPKMN( 1, OPPONENT ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls (Gegn.) wirkt!\n",
    //                     abilities[ ACPKMN( 1, OPPONENT ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 1, OPPONENT ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //
    //
    //        //Own Side
    //        consoleSelect( &Bottom );
    //        clear( );
    //        sprintf( buffer, "Los %ls! ", ACPKMN( 0, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //        consoleSelect( &Top );
    //
    //        animatePB( 20, 150 );
    //        oamIndex = OWN_PKMN_1_START;
    //        palcnt = OWN_PKMN_1_PAL;
    //        nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //        if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //            ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale ) ) {
    //            oamIndex = OWN_PKMN_1_START;
    //            palcnt = OWN_PKMN_1_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //            loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                            ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale );
    //        }
    //        if( ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ) )
    //            animateShiny( 6, 116 );
    //
    //        displayHP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //        displayHP( 100, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP,
    //                   256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //        displayEP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //
    //        POKEMON::PKMNDATA::getAll( ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, p );
    //        displayEP( 0, ( ACPKMN( 0, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                   ( POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                   256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //        OamTop->oamBuffer[ OWN_PB_START ].x = 256 - 88 - 32 + 4;
    //        OamTop->oamBuffer[ OWN_PB_START ].y = 192 - 31 - 32;
    //        consoleSetWindow( &Top, 21, 16, 20, 5 );
    //
    //        printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //
    //
    //        OamTop->oamBuffer[ OWN_PB_START ].isHidden = OamTop->oamBuffer[ OWN_HP_1 ].isHidden = false;
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 80; ++i )
    //            swiWaitForVBlank( );
    //
    //        if( ( m_battleMode == DOUBLE ) && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ) {
    //            consoleSelect( &Bottom );
    //            clear( );
    //            sprintf( buffer, "Auf in den Kampf %ls! ", ACPKMN( 1, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            consoleSelect( &Top );
    //
    //            animatePB( 80, 170 );
    //
    //            oamIndex = OWN_PKMN_2_START;
    //            palcnt = OWN_PKMN_2_PAL;
    //            nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //
    //            if( !loadPKMNSprite( OamTop,
    //                spriteInfoTop,
    //                "nitro:/PICS/SPRITES/PKMNBACK/",
    //                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC,
    //                50,
    //                120,
    //                oamIndex,
    //                palcnt,
    //                nextAvailableTileIdx,
    //                false,
    //                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ),
    //                ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale ) ) {
    //
    //                oamIndex = OWN_PKMN_2_START;
    //                palcnt = OWN_PKMN_2_PAL;
    //                nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //                loadPKMNSprite( OamTop,
    //                                spriteInfoTop,
    //                                "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC,
    //                                50,
    //                                120,
    //                                oamIndex,
    //                                palcnt,
    //                                nextAvailableTileIdx,
    //                                false,
    //                                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ),
    //                                !ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale );
    //            }
    //            if( ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ) )
    //                animateShiny( 50 + 16, 136 );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].isHidden = OamTop->oamBuffer[ OWN_HP_2 ].isHidden = false;
    //            updateOAM( OamTop );
    //
    //            displayHP( 100, 101, 256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //            displayHP( 100, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                       256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //            displayEP( 100, 100, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //            POKEMON::PKMNDATA::getAll( ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, p );
    //            displayEP( 0, ( ACPKMN( 1, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                       ( POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level ][ p.m_expType ] - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                       256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].x = 256 - 32 + 4;
    //            OamTop->oamBuffer[ OWN_PB_START + 1 ].y = 192 - 31;
    //            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //
    //            printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                printf( " " );
    //            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                printf( " " );
    //            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //        }
    //
    //        for( int i = 1 + ( m_battleMode == DOUBLE && _player->m_pkmnTeam->size( ) > 1 && ACPKMNSTS( 1, PLAYER ) != KO ); i < 6; ++i ) {
    //            OamTop->oamBuffer[ OWN_PB_START + i ].x = 236 - 18 * i;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].y = 196 - 16;
    //            OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = false;
    //            switch( ACPKMNSTS( i, PLAYER ) ) {
    //                case NA:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].isHidden = true;
    //                    break;
    //                case KO:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette = PB_PAL_START + 1;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 32;
    //                    break;
    //                case STS:
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].palette = PB_PAL_START + 2;
    //                    OamTop->oamBuffer[ OWN_PB_START + i ].gfxIndex += BattleBall1TilesLen / 16;
    //                    break;
    //                default:
    //                    break;
    //            }
    //        }
    //        updateOAM( OamTop );
    //        for( int i = 0; i < 80; ++i )
    //            swiWaitForVBlank( );
    //
    //        consoleSelect( &Bottom );
    //        if( abilities[ ACPKMN( 0, PLAYER ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls wirkt!\n",
    //                     abilities[ ACPKMN( 0, PLAYER ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 0, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //        if( ( m_battleMode == DOUBLE ) && abilities[ ACPKMN( 1, PLAYER ).m_boxdata.m_ability ].m_type & ability::BEFORE_BATTLE ) {
    //            clear( );
    //            sprintf( buffer, "%s von\n %ls wirkt!\n",
    //                     abilities[ ACPKMN( 1, PLAYER ).m_boxdata.m_ability ].m_abilityName.c_str( ),
    //                     ACPKMN( 1, PLAYER ).m_boxdata.m_Name );
    //            cust_font.printString( buffer, 8, 8, true );
    //            //abilities[(*_opponent->m_pkmnTeam)[_acPkmnPosition[0][1]].m_boxdata.m_ability].run();
    //            for( int i = 0; i < 100; ++i )
    //                swiWaitForVBlank( );
    //        }
    //
    //        //OamTop->oamBuffer[4].isHidden = OamTop->oamBuffer[12].isHidden = false;
    //
    //        updateOAM( OamTop );
    //
    //        for( int i = 0; i <= 4; ++i )
    //            Oam->oamBuffer[ i ].isHidden = false;
    //        Oam->oamBuffer[ 2 ].isHidden = !SAV.m_activatedPNav;
    //        updateOAMSub( Oam );
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //    }
    //
    //    float criticalChances[ 5 ] = { 0.0625, 0.125, 0.25, 0.3333, 0.5 };
    //    bool criticalOccured = false;
    //    float eff = 1;
    //    bool missed = false;
    //    int calcDamage( const move& p_move, const POKEMON::pokemon& p_attackingPkmn, const POKEMON::pokemon& p_defendingPkmn, int p_randomValue ) {
    //        if( p_move.m_moveHitType == move::moveHitTypes::STAT )
    //            return 0;
    //        eff = 1;
    //        missed = false;
    //        if( p_move.m_moveAccuracy && rand( ) * 1.0 / RAND_MAX > p_move.m_moveAccuracy / 100.0 ) {
    //            missed = true;
    //            return 0;
    //        }
    //
    //        if( p_defendingPkmn.m_stats.m_acHP == 0 ) {
    //            missed = true;
    //            return 0;
    //        }
    //
    //        int atkval = ( p_move.m_moveHitType == move::moveHitTypes::SPEC ? p_attackingPkmn.m_stats.m_SAtk : p_attackingPkmn.m_stats.m_Atk );
    //        int defval = ( p_move.m_moveHitType == move::moveHitTypes::SPEC ? p_defendingPkmn.m_stats.m_SDef : p_defendingPkmn.m_stats.m_Def );
    //
    //        int baseDmg = ( ( ( ( 2 * p_attackingPkmn.m_Level ) / 5 + 2 ) * p_move.m_moveBasePower * atkval ) / defval ) / 50 + 2;
    //
    //        POKEMON::PKMNDATA::pokemonData p1, p2;
    //        POKEMON::PKMNDATA::getAll( p_attackingPkmn.m_boxdata.m_speciesId, p1 );
    //        POKEMON::PKMNDATA::getAll( p_defendingPkmn.m_boxdata.m_speciesId, p2 );
    //
    //        int vs = 1;
    //        criticalOccured = false;
    //
    //        if( p_randomValue <= 15 )
    //            if( p_randomValue >= 0 && rand( ) * 1.0 / RAND_MAX <= criticalChances[ vs ] ) {
    //            baseDmg <<= 1;
    //            criticalOccured = true;
    //            }
    //
    //        if( p_randomValue == -1 )
    //            p_randomValue = 0;
    //        if( p_randomValue == -2 )
    //            p_randomValue = 15;
    //
    //        baseDmg = ( baseDmg * ( 100 - p_randomValue ) ) / 100;
    //
    //        if( p_move.m_moveType == p1.m_types[ 0 ] || p_move.m_moveType == p1.m_types[ 1 ] )
    //            baseDmg = ( baseDmg * 3 ) / 2;
    //
    //        eff = getEffectiveness( p_move.m_moveType, p2.m_types[ 0 ] );
    //        if( p2.m_types[ 1 ] != p2.m_types[ 0 ] )
    //            eff *= getEffectiveness( p_move.m_moveType, p2.m_types[ 1 ] );
    //        baseDmg = baseDmg * eff;
    //
    //        return std::max( 1, baseDmg );
    //    }
    //
    //    std::pair<int, int> ownAtk[ 2 ]; //AtkIdx; Target 1->opp[0]/2->opp[1]/3->both_opp/4->self/8->partner
    //    std::pair<int, int> oppAtk[ 2 ]; //AtkIdx; Target 1->own[0]/2->own[1]/3->both_opp/4->self/8->partner
    //    int switchWith[ 2 ][ 2 ] = { { 0 } };

    //    void battle::printAttackChoiceScreen( int p_pkmnSlot, int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex ) {
    //        for( int i = 21; i < 29; i += 2 ) {
    //            if( !( ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ) )
    //                continue;
    //
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).y += 14 + 16 * ( ( i - 21 ) / 4 );
    //            ( Oam->oamBuffer[ i ] ).y += 14 + 16 * ( ( i - 21 ) / 4 );
    //            if( ( i / 2 ) % 2 )
    //                ( Oam->oamBuffer[ i ] ).x -= 16;
    //            else
    //                ( Oam->oamBuffer[ i + 1 ] ).x += 16;
    //            updateOAMSub( Oam );
    //
    //            consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 17, 5 );
    //            consoleClear( );
    //            drawTypeIcon( Oam, spriteInfo, p_oamIndex, p_paletteIndex, p_tileIndex,
    //                          AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveType,
    //                          ( Oam->oamBuffer[ i ] ).x + 4, ( Oam->oamBuffer[ i ] ).y - 10, true );
    //            printf( "    %s\n    AP %2hhu""/""%2hhu ",
    //                    &( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveName[ 0 ] ),
    //                    ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ ( i - 21 ) / 2 ],
    //                    AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_movePP *
    //                    ( ( 5 + ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.PPupget( ( i - 21 ) / 2 ) ) / 5 ) );
    //            switch( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveHitType ) {
    //                case move::PHYS:
    //                    printf( "PHS" );
    //                    break;
    //                case move::SPEC:
    //                    printf( "SPC" );
    //                    break;
    //                case move::STAT:
    //                    printf( "STS" );
    //                    break;
    //            }
    //            printf( "\n    S " );
    //            if( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveBasePower )
    //                printf( "%3i", AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveBasePower );
    //            else
    //                printf( "---" );
    //            printf( " G " );
    //            if( AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveAccuracy )
    //                printf( "%3i", AttackList[ ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] ]->m_moveAccuracy );
    //            else
    //                printf( "---" );
    //        }
    //    }

    //    void battle::printTargetChoiceScreen( int p_pkmnSlot, int p_move, int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex ) {
    //        for( int i = 0; i < 5; ++i )
    //            Oam->oamBuffer[ p_oamIndex + i ].isHidden = true;
    //        for( int i = 21; i < 29; i += 2 ) {
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //        }
    //        updateOAMSub( Oam );
    //
    //        int poss = AttackList[ p_move ]->m_moveAffectsWhom;
    //        //Opp1, Opp2, ME, Partner, 
    //        bool validTrg[ 4 ] = {
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 16 || poss == 2 || poss == 4,
    //            poss == 0 || poss == 2 || poss == 32
    //        };
    //        if( !p_pkmnSlot )
    //            std::swap( validTrg[ 2 ], validTrg[ 3 ] );
    //        POKEMON::pokemon& p_attackingPkmn = ACPKMN( p_pkmnSlot, PLAYER );
    //
    //
    //        for( int i = 21; i < 29; i += 2 ) {
    //            int u = ( i - 21 ) / 2;
    //            if( m_battleMode != DOUBLE && ( u == 2 || u == 1 ) ) {
    //                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //                consoleClear( );
    //                continue;
    //            }
    //
    //            POKEMON::pokemon &acPK = ( u / 2 ? ACPKMN( 1 - u % 2, PLAYER ) : ACPKMN( u % 2, OPPONENT ) );
    //
    //            if( acPK.m_stats.m_acHP == 0 ) {
    //                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //                consoleClear( );
    //                continue;
    //            }
    //
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //            updateOAMSub( Oam );
    //
    //            consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //            consoleClear( );
    //
    //
    //            drawPKMNIcon( Oam, spriteInfo, acPK.m_boxdata.m_speciesId, ( Oam->oamBuffer[ i ] ).x - 4, ( Oam->oamBuffer[ i ] ).y - 16, p_oamIndex, p_paletteIndex, p_tileIndex, true );
    //
    //            updateOAMSub( Oam );
    //
    //            printf( "      %ls", acPK.m_boxdata.m_name );
    //            if( validTrg[ u ] ) {
    //                if( AttackList[ p_move ]->m_moveHitType != move::moveHitTypes::STAT )
    //                    printf( "\n      %3d-%2d KP\n        Schaden",
    //                    std::max( 1, calcDamage( *AttackList[ p_move ], p_attackingPkmn, acPK, -2 ) ),
    //                    std::max( 1, calcDamage( *AttackList[ p_move ], p_attackingPkmn, acPK, -1 ) ) );
    //                else
    //                    printf( "\n       Keinen\n        Schaden" );
    //            }
    //        }
    //    }

    //    void battle::printPKMNSwitchScreen( int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex ) {
    //        setMainBattleVisibility( true );
    //        consoleClear( );
    //        dinit( );
    //        for( int i = 0; i <= 8; ++i )
    //            ( Oam->oamBuffer[ i ] ).isHidden = true;
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = false;
    //        updateOAMSub( Oam );
    //
    //        int num = (int)_player->m_pkmnTeam->size( );
    //        consoleSelect( &Bottom );
    //
    //        for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) ) {
    //            if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                break;
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).isHidden = false;
    //            ( Oam->oamBuffer[ i + 1 ] ).y -= 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //            ( Oam->oamBuffer[ i ] ).y -= 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //            updateOAMSub( Oam );
    //            consoleSetWindow( &Bottom, ( ( Oam->oamBuffer[ i ] ).x + 6 ) / 8, ( ( Oam->oamBuffer[ i ] ).y + 6 ) / 8, 12, 3 );
    //            if( !ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_IV.m_isEgg ) {
    //                printf( "   %3i/%3i\n ",
    //                        ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_stats.m_acHP,
    //                        ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_stats.m_maxHP );
    //                wprintf( ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_Name );
    //                printf( "\n" );
    //                printf( "%11s",
    //                        ItemList[ ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_Item ].getDisplayName( ).c_str( ) );
    //                drawPKMNIcon( Oam,
    //                              spriteInfo,
    //                              ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_SPEC,
    //                              ( Oam->oamBuffer[ i ] ).x - 4,
    //                              ( Oam->oamBuffer[ i ] ).y - 20,
    //                              p_oamIndex,
    //                              p_paletteIndex,
    //                              p_tileIndex,
    //                              true );
    //            } else {
    //                printf( "\n Ei" );
    //                drawEggIcon( Oam,
    //                             spriteInfo,
    //                             ( Oam->oamBuffer[ i ] ).x - 4,
    //                             ( Oam->oamBuffer[ i ] ).y - 20,
    //                             p_oamIndex,
    //                             p_paletteIndex,
    //                             p_tileIndex,
    //                             true );
    //            }
    //            updateOAMSub( Oam );
    //        }
    //    }

    //    int battle::getSwitchPkmn( int& p_oamIndex, int& p_paletteIndex, int& p_tileIndex, bool p_retA ) {
    //        int res = -1;
    //        int num = (int)_player->m_pkmnTeam->size( );
    //        touchPosition t;
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = !p_retA;
    //        while( 42 ) {
    //            swiWaitForVBlank( );
    //            updateOAMSub( Oam );
    //            updateTime( );
    //            touchRead( &t );
    //
    //            if( p_retA && t.px > 224 && t.py > 164 ) {
    //                waitForTouchUp( );
    //
    //                consoleSelect( &Bottom );
    //                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                consoleClear( );
    //                for( int i = 5; i <= 8; ++i )
    //                    ( Oam->oamBuffer[ i ] ).isHidden = false;
    //                for( int i = 20; i <= p_oamIndex; ++i )
    //                    ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                updateOAMSub( Oam );
    //
    //                for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) ) {
    //                    if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                        break;
    //                    ( Oam->oamBuffer[ i + 1 ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //                    ( Oam->oamBuffer[ i ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //                }
    //                init( );
    //                return -1;
    //            }
    //            for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) )
    //                if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                    break;
    //                else if( t.px > Oam->oamBuffer[ i ].x && t.py > Oam->oamBuffer[ i ].y && t.px - 64 < Oam->oamBuffer[ i + 1 ].x && t.py - 32 < Oam->oamBuffer[ i ].y ) {
    //                    if( ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_boxdata.m_IV.m_isEgg ||
    //                        ACPKMN( ( ( i - 21 ) / 2 ) ^ 1, PLAYER ).m_stats.m_acHP == 0 ||
    //                        ( ( ( i - 21 ) / 2 ) ^ 1 ) < ( 1 + ( m_battleMode == DOUBLE ) ) )
    //                        continue;
    //
    //                    ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                    ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                    ( Oam->oamBuffer[ 3 + ( ( ( i - 21 ) / 2 ) ^ 1 ) ] ).isHidden = true;
    //                    updateOAMSub( Oam );
    //
    //                    waitForTouchUp( );
    //
    //                    res = ( ( ( i - 21 ) / 2 ) ^ 1 );
    //                    goto OUT2;
    //                }
    //        }
    //OUT2:
    //        consoleSelect( &Bottom );
    //        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //        consoleClear( );
    //        for( int i = 5; i <= 8; ++i )
    //            ( Oam->oamBuffer[ i ] ).isHidden = false;
    //        for( int i = 20; i <= p_oamIndex; ++i )
    //            ( Oam->oamBuffer[ i ] ).isHidden = true;
    //        for( int i = 23; i < 32; i += ( ( ( i - 21 ) / 2 ) % 2 ? -2 : +6 ) ) {
    //            if( ( ( ( i - 21 ) / 2 ) ^ 1 ) >= num )
    //                break;
    //            ( Oam->oamBuffer[ i + 1 ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //            ( Oam->oamBuffer[ i ] ).y += 16 * ( 2 - ( ( i - 21 ) / 4 ) );
    //        }
    //        updateOAMSub( Oam );
    //        init( );
    //
    //        return res;
    //    }

    //#define BATTLE_END  -1
    //#define RETRY       +1
    //#define RETRY2       -2
    //#define SUCCESS     +2
    //    int battle::getChoice( int p_pkmnSlot ) {
    //        touchPosition t;
    //
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = !p_pkmnSlot;
    //        updateOAMSub( Oam );
    //
    //        setMainBattleVisibility( false );
    //        int oamIndex = oamIndexS, paletteIndex = palcntS, tileIndex = nextAvailableTileIdxS;
    //        drawPKMNIcon( Oam, spriteInfo, ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_SPEC, 112, 64, oamIndex, paletteIndex, tileIndex, true );
    //        clear( );
    //        sprintf( buffer, "Was soll %ls tun?", ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //        updateOAMSub( Oam );
    //
    //        int aprest = 0;
    //
    //        while( 42 ) {
    //            updateTime( );
    //            swiWaitForVBlank( );
    //            touchRead( &t );
    //            if( p_pkmnSlot && t.px > 224 && t.py > 164 ) {
    //                waitForTouchUp( );
    //
    //                return RETRY2;
    //            }
    //            //BEGIN FIGHT
    //            else if( t.px > 64 && t.px < 64 + 128 && t.py > 72 && t.py < 72 + 64 ) {
    //                waitForTouchUp( );
    //                setMainBattleVisibility( true );
    //                Oam->oamBuffer[ oamIndexS + 1 ].isHidden = true;
    //                consoleClear( );
    //
    //                aprest = 0;
    //                for( int i = 0; i < 4; ++i )
    //                    aprest += ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ i ];
    //                if( aprest == 0 ) {
    //                    clear( );
    //                    printf( "%ls hat keine\n restlichen Attacken...", ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //                    ownAtk[ p_pkmnSlot ] = std::pair<int, int>( 165, 1 | 2 | 4 | 8 );
    //                } else {
    //                    ( Oam->oamBuffer[ 20 ] ).isHidden = false;
    //
    //                    oamIndex = oamIndexS; paletteIndex = palcntS; tileIndex = nextAvailableTileIdxS;
    //                    printAttackChoiceScreen( p_pkmnSlot, oamIndex, paletteIndex, tileIndex );
    //
    //                    updateOAMSub( Oam );
    //                    consoleSetWindow( &Bottom, 0, 0, 32, 5 );
    //                    clear( );
    //                    sprintf( buffer, "Welchen Angriff?" );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    while( 42 ) {
    //                        updateTime( );
    //                        swiWaitForVBlank( );
    //                        touchRead( &t );
    //                        if( t.px > 224 && t.py > 164 ) { //Back  
    //                            waitForTouchUp( );
    //
    //                            consoleSelect( &Bottom );
    //                            consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                            consoleClear( );
    //                            for( int i = 20; i <= oamIndex; ++i )
    //                                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                            setMainBattleVisibility( false );
    //
    //
    //                            for( int i = 21; i < 29; i += 2 ) {
    //                                ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                                ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                                ( Oam->oamBuffer[ i + 1 ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //                                ( Oam->oamBuffer[ i ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //
    //                                if( ( i / 2 ) % 2 )
    //                                    ( Oam->oamBuffer[ i ] ).x += 16;
    //                                else
    //                                    ( Oam->oamBuffer[ i + 1 ] ).x -= 16;
    //                                updateOAMSub( Oam );
    //                            }
    //
    //                            return RETRY;
    //                        }
    //
    //                        for( int i = 21; i < 29; i += 2 )
    //                            if( t.px>( Oam->oamBuffer[ i ].x ) && t.px < ( Oam->oamBuffer[ i + 1 ].x + 64 ) &&
    //                                t.py>( Oam->oamBuffer[ i ] ).y && t.py < ( Oam->oamBuffer[ i ].y + 32 ) ) {
    //                            if( !ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] )
    //                                continue;
    //                            if( !ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ ( i - 21 ) / 2 ] )
    //                                continue;
    //
    //                            while( 1 ) {
    //                                swiWaitForVBlank( );
    //                                updateTime( false );
    //                                scanKeys( );
    //                                t = touchReadXY( );
    //                                if( t.px == 0 && t.py == 0 )
    //                                    break;
    //                            }
    //                            int trg = getTarget( p_pkmnSlot, ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ] );
    //                            if( trg == 0 ) {
    //                                oamIndex = oamIndexS; paletteIndex = palcntS; tileIndex = nextAvailableTileIdxS;
    //                                for( int i = 21; i < 29; i += 2 ) {
    //                                    ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                                    ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                                    ( Oam->oamBuffer[ i + 1 ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //                                    ( Oam->oamBuffer[ i ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //
    //                                    if( ( i / 2 ) % 2 )
    //                                        ( Oam->oamBuffer[ i ] ).x += 16;
    //                                    else
    //                                        ( Oam->oamBuffer[ i + 1 ] ).x -= 16;
    //
    //                                    updateOAMSub( Oam );
    //                                }
    //                                printAttackChoiceScreen( p_pkmnSlot, oamIndex, paletteIndex, tileIndex );
    //
    //                                updateOAMSub( Oam );
    //                                consoleSetWindow( &Bottom, 0, 0, 32, 5 );
    //                                clear( );
    //                                sprintf( buffer, "Welchen Angriff?" );
    //                                cust_font.printString( buffer, 8, 8, true );
    //                                break;
    //                            }
    //
    //                            ownAtk[ p_pkmnSlot ] = std::pair<int, int>( ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Attack[ ( i - 21 ) / 2 ], trg );
    //                            ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_AcPP[ ( i - 21 ) / 2 ]--;
    //                            goto ATTACKCHOSEN;
    //                            }
    //                    }
    //ATTACKCHOSEN:
    //                    for( int i = 21; i < 29; i += 2 ) {
    //                        ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                        ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                        ( Oam->oamBuffer[ i + 1 ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //                        ( Oam->oamBuffer[ i ] ).y -= 14 + 16 * ( ( i - 21 ) / 4 );
    //
    //                        if( ( i / 2 ) % 2 )
    //                            ( Oam->oamBuffer[ i ] ).x += 16;
    //                        else
    //                            ( Oam->oamBuffer[ i + 1 ] ).x -= 16;
    //
    //                        updateOAMSub( Oam );
    //                    }
    //                    for( int i = oamIndex; i >= oamIndex - 3; --i )
    //                        ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                    updateOAMSub( Oam );
    //                    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                    consoleClear( );
    //                    consoleSetWindow( &Bottom, 0, 0, 32, 5 );
    //                }
    //                return SUCCESS;
    //            }
    //            //END FIGHT
    //            //BEGIN BAG
    //            else if( t.px > 16 && t.px < 16 + 64 && t.py > 144 && t.py < 144 + 32 ) {
    //                waitForTouchUp( );
    //                return BATTLE_END;
    //            }
    //            //END BAG
    //            //BEGIN POKENAV
    //            else if( SAV.m_activatedPNav && t.px > 96 && t.px < 96 + 64 && t.py > 152 && t.py < 152 + 32 ) {
    //                waitForTouchUp( );
    //                printf( "TEST3" );
    //            }
    //            //END POKENAV
    //            //BEGIN pokemon
    //            else if( t.px > 176 && t.px < 176 + 64 && t.py > 144 && t.py < 144 + 32 ) {
    //                waitForTouchUp( );
    //
    //                oamIndex = oamIndexS; paletteIndex = palcntS; tileIndex = nextAvailableTileIdxS;
    //                printPKMNSwitchScreen( oamIndex, paletteIndex, tileIndex );
    //
    //                int res = getSwitchPkmn( oamIndex, paletteIndex, tileIndex, true );
    //
    //                if( res == -1 )
    //                    return RETRY;
    //
    //                switchWith[ p_pkmnSlot ][ PLAYER ] = res;
    //
    //                return SUCCESS;
    //            }
    //            //END pokemon
    //        }
    //    }

    //#define OPP_1   1
    //#define OPP_2   2
    //#define ME      4
    //#define PARTNER 8
    //#define OWN_FIELD 16
    //#define OPP_FIELD 32
    //    int battle::getTarget( int p_pkmnSlot, int p_move ) {
    //        int ret = 0;
    //
    //        touchPosition t;
    //
    //        ( Oam->oamBuffer[ 20 ] ).isHidden = false;
    //        updateOAMSub( Oam );
    //
    //        clear( );
    //        sprintf( buffer, "Wen soll %ls angreifen?", ACPKMN( p_pkmnSlot, PLAYER ).m_boxdata.m_Name );
    //        cust_font.printString( buffer, 8, 8, true );
    //
    //        int oamIndex = oamIndexS, paletteIndex = palcntS, tileIndex = nextAvailableTileIdxS;
    //        printTargetChoiceScreen( p_pkmnSlot, p_move, oamIndex, paletteIndex, tileIndex );
    //
    //        int poss = AttackList[ p_move ]->m_moveAffectsWhom;
    //        //Opp1, Opp2, ME, Partner
    //        bool validTrg[ 4 ] = {
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 0 || poss == 8 || poss == 32 || poss == 64,
    //            poss == 16 || poss == 2 || poss == 4,
    //            poss == 0 || poss == 2 || poss == 32
    //        };
    //        if( !p_pkmnSlot )
    //            std::swap( validTrg[ 2 ], validTrg[ 3 ] );
    //
    //        int change = 0;
    //
    //        while( 42 ) {
    //            if( ++change == 60 ) {
    //                bool changeA[ 4 ] = { false };
    //
    //                if( poss & 2 )
    //                    changeA[ 2 ] = changeA[ 3 ] = true;
    //                if( ( poss & 8 ) || ( poss & 32 ) || ( poss & 64 ) )
    //                    changeA[ 0 ] = changeA[ 1 ] = true;
    //                if( poss & 32 )
    //                    changeA[ 2 + ( p_pkmnSlot ) ] = true;
    //
    //                for( int i = 21; i < 29; i += 2 ) {
    //                    if( m_battleMode == DOUBLE && changeA[ ( i - 21 ) / 2 ] ) {
    //                        int u = ( i - 21 ) / 2;
    //
    //                        POKEMON::pokemon &acPK = ( ( u / 2 ) ? ACPKMN( 1 - u % 2, PLAYER ) : ACPKMN( u % 2, OPPONENT ) );
    //
    //                        if( acPK.m_stats.m_acHP == 0 )
    //                            continue;
    //
    //                        Oam->oamBuffer[ i ].isHidden = !Oam->oamBuffer[ i ].isHidden;
    //                        Oam->oamBuffer[ i + 1 ].isHidden = !Oam->oamBuffer[ i + 1 ].isHidden;
    //                    }
    //                }
    //                updateOAMSub( Oam );
    //                change = 0;
    //            }
    //            updateTime( );
    //            swiWaitForVBlank( );
    //            touchRead( &t );
    //            if( t.px>224 && t.py > 164 ) { //Back  
    //                waitForTouchUp( );
    //                return 0;
    //            }
    //
    //            for( int i = 21; i < 29; i += 2 ) {
    //                if( t.px>( Oam->oamBuffer[ i ].x ) && t.px < ( Oam->oamBuffer[ i + 1 ].x + 64 ) &&
    //                    t.py>( Oam->oamBuffer[ i ] ).y && t.py < ( Oam->oamBuffer[ i ].y + 32 ) ) {
    //                    int u = ( i - 21 ) / 2;
    //                    if( m_battleMode != DOUBLE && ( u == 2 || u == 1 ) )
    //                        continue;
    //                    if( !validTrg[ u ] )
    //                        continue;
    //
    //                    POKEMON::pokemon &acPK = ( ( u / 2 ) ? ACPKMN( 1 - u % 2, PLAYER ) : ACPKMN( u % 2, OPPONENT ) );
    //
    //                    if( acPK.m_stats.m_acHP == 0 ) {
    //                        ( Oam->oamBuffer[ i ] ).isHidden = true;
    //                        ( Oam->oamBuffer[ i + 1 ] ).isHidden = true;
    //                        consoleSetWindow( &Bottom, ( Oam->oamBuffer[ i ] ).x / 8 - 3, ( Oam->oamBuffer[ i ] ).y / 8 + 1, 20, 5 );
    //                        consoleClear( );
    //                        continue;
    //                    }
    //
    //                    while( 1 ) {
    //                        swiWaitForVBlank( );
    //                        updateTime( false );
    //                        scanKeys( );
    //                        t = touchReadXY( );
    //                        if( t.px == 0 && t.py == 0 )
    //                            break;
    //                    }
    //                    ret = ( 1 << u );
    //                    if( u == 2 && p_pkmnSlot == 0 )
    //                        ret = PARTNER;
    //                    if( poss & 2 )
    //                        ret = OWN_FIELD;
    //                    if( poss & 4 )
    //                        ret = ( 2 << ( rand( ) % 3 ) );
    //                    if( ( poss & 8 ) || ( poss & 32 ) )
    //                        ret |= OPP_1 | OPP_2;
    //                    if( poss & 32 )
    //                        ret |= PARTNER;
    //                    if( poss & 64 )
    //                        ret = OPP_FIELD;
    //
    //                    return ret;
    //                }
    //            }
    //        }
    //    }

    //    bool participated[ 6 ] = { false };
    //#define LUCKY_EGG_EFFEKT     42
    //    int calcEXP( const POKEMON::pokemon& p_move, int p_atkind, const POKEMON::pokemon& p_defendingPkmn, bool p_wild ) {
    //        if( !SAV.m_EXPShareEnabled && !participated[ p_atkind ] )
    //            return 0;
    //
    //        float a = p_wild ? 1 : 1.5;
    //        POKEMON::PKMNDATA::getAll( p_defendingPkmn.m_boxdata.m_speciesId, p );
    //        int b = p.m_EXPYield;
    //
    //        float e = ItemList[ p_move.m_boxdata.m_holdItem ].getEffectType( ) == LUCKY_EGG_EFFEKT ? 1.5 : 1;
    //
    //        int L = p_move.m_Level;
    //
    //        int s = 1;
    //        if( SAV.m_EXPShareEnabled && !participated[ p_atkind ] )
    //            s = 2;
    //
    //        float t = ( p_move.m_boxdata.m_oTId == SAV.m_Id && p_move.m_boxdata.m_oTSid == SAV.m_Sid ? 1 : 1.5 );
    //
    //        return int( ( a * t* b* e* L ) / ( 7 * s ) );
    //    }

    //    void printEFFLOG( const POKEMON::pokemon& P, int p_move ) {
    //        if( AttackList[ p_move ]->m_moveHitType == move::moveHitTypes::STAT )
    //            return;
    //
    //        if( missed ) {
    //            clear( );
    //            if( P.m_stats.m_acHP )
    //                sprintf( buffer, "%ls wich der Attacke aus.", P.m_boxdata.m_name );
    //            else
    //                sprintf( buffer, "Die Attacke ging daneben..." );
    //            cust_font.printString( buffer, 8, 8, true );
    //            for( int i = 0; i < 75; ++i )
    //                swiWaitForVBlank( );
    //            return;
    //        }
    //        if( eff != 1 )
    //            clear( );
    //        if( eff > 3 )
    //            sprintf( buffer, "Das ist enorm effektiv\ngegen %ls!", P.m_boxdata.m_name );
    //        else if( eff > 1 )
    //            sprintf( buffer, "Das ist sehr effektiv\ngegen %ls!", P.m_boxdata.m_name );
    //        else if( eff == 0 )
    //            sprintf( buffer, "Hat die Attacke\n%lsgetroffen?", P.m_boxdata.m_name );
    //        else if( eff < 0.3 )
    //            sprintf( buffer, "Das ist nur enorm wenig\neffektiv gegen %ls...", P.m_boxdata.m_name );
    //        else if( eff < 1 )
    //            sprintf( buffer, "Das ist nicht sehr effektiv\ngegen %ls.", P.m_boxdata.m_name );
    //
    //        if( eff != 1 ) {
    //            cust_font.printString( buffer, 8, 8, true );
    //            for( int i = 0; i < 75; ++i )
    //                swiWaitForVBlank( );
    //        }
    //        if( criticalOccured ) {
    //            clear( );
    //            cust_font.printString( "Ein Volltreffer!", 8, 8, true );
    //            for( int i = 0; i < 75; ++i )
    //                swiWaitForVBlank( );
    //        }
    //    }

    //    int battle::start( int p_battleBack, weather p_weather ) {
    //        drawSub( );
    //
    //        for( int i = 0; i < 6; ++i )
    //            participated[ i ] = false;
    //
    //        videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
    //        drawTopBack( );
    //        initBattleScene( p_battleBack, p_weather );
    //        //touchPosition t;
    //
    //        while( ( _round-- ) != 0 ) {
    //            switchWith[ 0 ][ OPPONENT ] = switchWith[ 0 ][ PLAYER ] =
    //                switchWith[ 1 ][ OPPONENT ] = switchWith[ 1 ][ PLAYER ] = 0;
    //
    //            //Switch Out KOed pokemon
    //
    //            //Own
    //            if( ACPKMNSTS( 0, PLAYER ) == KO ) {
    //                int pcnt = 0, at = 0;
    //                for( int i = 1 + ( m_battleMode == DOUBLE ); i < 6; ++i )
    //                    if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                    //switchOwnPkmn(i,0);
    //                    pcnt++;
    //                    at = i;
    //                    }
    //                if( pcnt == 1 )
    //                    switchOwnPkmn( at, 0 );
    //                else if( pcnt > 1 ) {
    //                    int oamIndex = oamIndexS,
    //                        paletteIndex = palcntS,
    //                        tileIndex = nextAvailableTileIdxS;
    //                    printPKMNSwitchScreen( oamIndex, paletteIndex, tileIndex );
    //
    //                    int res = getSwitchPkmn( oamIndex, paletteIndex, tileIndex, false );
    //                    switchOwnPkmn( res, 0 );
    //                }
    //            }
    //            if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, PLAYER ) == KO ) {
    //                int pcnt = 0, at = 0;
    //                for( int i = 2; i < 6; ++i ) {
    //                    if( ACPKMNSTS( i, PLAYER ) != KO ) {
    //                        //switchOwnPkmn(i,0);
    //                        pcnt++;
    //                        at = i;
    //                    }
    //                }
    //                if( pcnt == 1 )
    //                    switchOwnPkmn( at, 1 );
    //                else if( pcnt > 1 ) {
    //                    int oamIndex = oamIndexS,
    //                        paletteIndex = palcntS,
    //                        tileIndex = nextAvailableTileIdxS;
    //                    printPKMNSwitchScreen( oamIndex, paletteIndex, tileIndex );
    //
    //                    int res = getSwitchPkmn( oamIndex, paletteIndex, tileIndex, false );
    //                    switchOwnPkmn( res, 1 );
    //                }
    //            }
    //
    //            if( ACPKMNSTS( 0, PLAYER ) == KO && ( m_battleMode != DOUBLE || ACPKMNSTS( 1, PLAYER ) == KO || ACPKMNSTS( 1, PLAYER ) == NA ) ) {
    //                //Player lost
    //                clear( );
    //                cust_font.printString( _opponent->getWinMsg( ), 8, 8, true );
    //                for( int i = 0; i < 75; ++i )
    //                    swiWaitForVBlank( );
    //
    //                dinit( );
    //                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                consoleSelect( &Bottom );
    //                consoleClear( );
    //                initOAMTableSub( Oam );
    //                initMainSprites( Oam, spriteInfo );
    //                setMainSpriteVisibility( false );
    //                Oam->oamBuffer[ 8 ].isHidden = true;
    //                Oam->oamBuffer[ 0 ].isHidden = true;
    //                Oam->oamBuffer[ 1 ].isHidden = false;
    //                return -1;
    //            }
    //
    //            //Opp
    //            if( ACPKMNSTS( 0, OPPONENT ) == KO )
    //                for( size_t i = 1 + ( m_battleMode == DOUBLE ); i < _opponent->m_pkmnTeam->size( ); ++i )
    //                    if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                switchOppPkmn( i, 0 );
    //                for( int i = 0; i < 6; ++i )
    //                    participated[ i ] = false;
    //                break;
    //                    }
    //            if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, OPPONENT ) == KO )
    //                for( size_t i = 2; i < _opponent->m_pkmnTeam->size( ); ++i )
    //                    if( ACPKMNSTS( i, OPPONENT ) != KO ) {
    //                switchOppPkmn( i, 1 );
    //                for( int i = 0; i < 6; ++i )
    //                    participated[ i ] = false;
    //                break;
    //                    }
    //            if( ACPKMNSTS( 0, OPPONENT ) == KO && ( m_battleMode != DOUBLE || ACPKMNSTS( 1, OPPONENT ) == KO || ACPKMNSTS( 1, OPPONENT ) == NA ) ) {
    //                //Opp lost
    //                clear( );
    //                cust_font.printString( _opponent->getLooseMsg( ), 8, 8, true );
    //                for( int i = 0; i < 75; ++i )
    //                    swiWaitForVBlank( );
    //
    //                SAV.m_money += _opponent->getLooseMoney( );
    //                clear( );
    //                sprintf( buffer, "Du gewinnst %dP!", _opponent->getLooseMoney( ) );
    //                cust_font.printString( buffer, 8, 8, true );
    //                for( int i = 0; i < 75; ++i )
    //                    swiWaitForVBlank( );
    //
    //                dinit( );
    //                consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //                consoleSelect( &Bottom );
    //                consoleClear( );
    //                initOAMTableSub( Oam );
    //                initMainSprites( Oam, spriteInfo );
    //                setMainSpriteVisibility( false );
    //                Oam->oamBuffer[ 8 ].isHidden = true;
    //                Oam->oamBuffer[ 0 ].isHidden = true;
    //                Oam->oamBuffer[ 1 ].isHidden = false;
    //                return 1;
    //            }
    //            //End of Switch Out
    //
    //            participated[ ACPOS( 0, PLAYER ) ] = true;
    //            if( ( m_battleMode == DOUBLE ) )
    //                participated[ ACPOS( 1, PLAYER ) ] = true;
    //
    //            consoleSelect( &Bottom );
    //
    //BEFORE_0:
    //            if( ACPKMNSTS( 0, PLAYER ) != KO ) {
    //                switch( getChoice( 0 ) ) {
    //                    case SUCCESS:
    //                        goto BEFORE_1;
    //                    case RETRY:
    //                        switchWith[ 0 ][ PLAYER ] = 0;
    //                        goto BEFORE_0;
    //                    case BATTLE_END:
    //                        goto END;
    //                }
    //            }
    //BEFORE_1:
    //            if( ( m_battleMode == DOUBLE ) && ACPKMNSTS( 1, PLAYER ) != KO )
    //                switch( getChoice( 1 ) ) {
    //                    case RETRY:
    //                        switchWith[ 1 ][ PLAYER ] = 0;
    //                        goto BEFORE_1;
    //                    case RETRY2:
    //                        switchWith[ 0 ][ PLAYER ] = switchWith[ 1 ][ PLAYER ] = 0;
    //                        goto BEFORE_0;
    //                    case BATTLE_END:
    //                        goto END;
    //            }
    //
    //            ( Oam->oamBuffer[ 20 ] ).isHidden = true;
    //            updateOAMSub( Oam );
    //            //OPP'S ACTIONS
    //            switchWith[ 0 ][ OPPONENT ] = switchWith[ 0 ][ PLAYER ];
    //            switchWith[ 1 ][ OPPONENT ] = switchWith[ 1 ][ PLAYER ];
    //            oppAtk[ 0 ] = std::pair<int, int>( ACPKMN( 0, OPPONENT ).m_boxdata.m_Attack[ 0 ], 1 );
    //            oppAtk[ 1 ] = std::pair<int, int>( ACPKMN( 1, OPPONENT ).m_boxdata.m_Attack[ 0 ], 2 );
    //
    //
    //            consoleClear( );
    //
    //            int inits[ 4 ] = { 0 }, ranking[ 4 ] = { 0 };
    //
    //            int maxst = ( m_battleMode == DOUBLE ) ? 4 : 2;
    //
    //            for( int i = 0; i < maxst; ++i )
    //                if( i % 2 )
    //                    inits[ i ] = ( *_opponent->m_pkmnTeam )[ _acPkmnPosition[ i / 2 ][ OPPONENT ] ].m_stats.m_Spd;
    //                else
    //                    inits[ i ] = ( *_player->m_pkmnTeam )[ _acPkmnPosition[ i / 2 ][ PLAYER ] ].m_stats.m_Spd;
    //
    //            bool ko[ 2 ][ 2 ] = { { false } };
    //
    //            for( int i = 0; i < maxst; ++i ) {
    //                for( int j = 0, max = -1; j < maxst; ++j )
    //                    if( inits[ j ] > max && ( !i || ( ranking[ i - 1 ] != j && inits[ ranking[ i - 1 ] ] >= inits[ j ] ) ) ) {
    //                    max = inits[ j ];
    //                    ranking[ i ] = j;
    //                    }
    //                for( int i = 0; i < maxst; ++i ) {
    //                    int prio = 0;
    //                    if( i % 2 )
    //                        prio = AttackList[ oppAtk[ i / 2 ].first ]->m_movePriority;
    //                    else
    //                        prio = AttackList[ ownAtk[ i / 2 ].first ]->m_movePriority;
    //                    ranking[ i ] -= 4 * prio;
    //                }
    //            }
    //            for( int i = 0; i < maxst; ++i ) {
    //                int acin = 0;
    //                for( int j = 0, min = 100; j < maxst; ++j )
    //                    if( ranking[ j ] < min ) {
    //                    acin = j;
    //                    min = ranking[ j ];
    //                    }
    //                ranking[ acin ] = 42442;
    //
    //                bool opp = acin % 2;
    //
    //                if( opp ) {
    //                    if( ACPKMN( acin / 2, OPPONENT ).m_stats.m_acHP == 0 )
    //                        continue;
    //                } else
    //                    if( ACPKMN( acin / 2, PLAYER ).m_stats.m_acHP == 0 )
    //                        continue;
    //
    //                if( switchWith[ acin / 2 ][ opp ] ) {
    //                    if( opp )
    //                        switchOppPkmn( switchWith[ acin / 2 ][ opp ], acin / 2 );
    //                    else
    //                        switchOwnPkmn( switchWith[ acin / 2 ][ opp ], acin / 2 );
    //                    continue;
    //                }
    //
    //                ko[ 0 ][ PLAYER ] = ko[ 0 ][ OPPONENT ] = ko[ 1 ][ PLAYER ] = ko[ 1 ][ OPPONENT ] = false;
    //                consoleSelect( &Top );
    //
    //                if( opp )
    //                    sprintf( buffer,
    //                    "%ls (Gegner)\nsetzt %s ein!",
    //                    ACPKMN( acin / 2, OPPONENT ).m_boxdata.m_Name,
    //                    AttackList[ oppAtk[ acin / 2 ].first ]->m_moveName.c_str( ) );
    //                else
    //                    sprintf( buffer,
    //                    "%ls setzt\n%s ein!",
    //                    ACPKMN( acin / 2, PLAYER ).m_boxdata.m_Name,
    //                    AttackList[ ownAtk[ acin / 2 ].first ]->m_moveName.c_str( ) );
    //                clear( );
    //                cust_font.printString( buffer, 8, 8, true );
    //                for( int i = 0; i < 70; ++i )
    //                    swiWaitForVBlank( );
    //                if( opp ) {
    //                    int tg = oppAtk[ acin / 2 ].second;
    //                    int dmg1 = 0, dmg2 = 0, dmg3 = 0;
    //                    if( tg & 1 ) {
    //                        missed = false;
    //                        dmg1 = calcDamage( *AttackList[ oppAtk[ acin / 2 ].first ], ACPKMN( acin / 2, OPPONENT ), ACPKMN( 0, PLAYER ), rand( ) % 15 );
    //
    //                        if( !missed ) {
    //                            int old = ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP;
    //                            ACPKMN( 0, PLAYER ).m_stats.m_acHP = std::max( 0, ACPKMN( 0, PLAYER ).m_stats.m_acHP - dmg1 );
    //
    //                            ko[ 0 ][ PLAYER ] = ( ACPKMN( 0, PLAYER ).m_stats.m_acHP == 0 );
    //                            if( ko[ 0 ][ PLAYER ] )
    //                                ACPKMNSTS( 0, PLAYER ) = KO;
    //
    //                            displayHP( old, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP,
    //                                       256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, true );
    //                            consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                            consoleClear( );
    //                            printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                    GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                    ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //                        }
    //                        printEFFLOG( ( ACPKMN( 0, PLAYER ) ), oppAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 2 ) ) {
    //                        missed = false;
    //                        dmg2 = calcDamage( *AttackList[ oppAtk[ acin / 2 ].first ], ACPKMN( acin / 2, OPPONENT ), ACPKMN( 1, PLAYER ), rand( ) % 15 );
    //
    //                        if( !missed ) {
    //                            int old = ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP;
    //                            ACPKMN( 1, PLAYER ).m_stats.m_acHP = std::max( 0, ACPKMN( 1, PLAYER ).m_stats.m_acHP - dmg2 );
    //
    //                            ko[ 1 ][ PLAYER ] = ( ACPKMN( 1, PLAYER ).m_stats.m_acHP == 0 );
    //                            if( ko[ 1 ][ PLAYER ] )
    //                                ACPKMNSTS( 1, PLAYER ) = KO;
    //                            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                            consoleClear( );
    //                            printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                printf( " " );
    //                            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                printf( " " );
    //                            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //                            displayHP( old, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                                       256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, true );
    //
    //                        }
    //                        printEFFLOG( ( ACPKMN( 1, PLAYER ) ), oppAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 8 ) ) {
    //                        missed = false;
    //                        dmg3 = calcDamage( *AttackList[ oppAtk[ acin / 2 ].first ], ACPKMN( acin / 2, OPPONENT ),
    //                                           ACPKMN( 1 - acin / 2, OPPONENT ), rand( ) % 15 );
    //
    //                        if( !missed ) {
    //                            int old = 0;
    //                            if( acin / 2 == 0 )
    //                                old = ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP;
    //                            else
    //                                old = ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP;
    //
    //                            ACPKMN( 1 - acin / 2, OPPONENT ).m_stats.m_acHP = std::max( 0, ACPKMN( 1 - acin / 2, OPPONENT ).m_stats.m_acHP - dmg3 );
    //                            if( !ACPKMN( 1 - acin / 2, OPPONENT ).m_stats.m_acHP ) {
    //                                ko[ 1 - ( acin / 2 ) ][ OPPONENT ] = true;
    //                                ACPKMNSTS( 1 - ( acin / 2 ), OPPONENT ) = KO;
    //                            }
    //                            if( acin / 2 == 0 ) {
    //                                consoleSetWindow( &Top, 0, 5, 20, 2 );
    //                                consoleClear( );
    //                                printf( "%10ls%c\n", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name, GENDER( ACPKMN( 0, OPPONENT ) ) );
    //                                if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //                                    printf( " " );
    //                                if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //                                    printf( " " );
    //                                printf( "Lv%d%4dKP", ACPKMN( 0, OPPONENT ).m_Level,
    //                                        ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //                                displayHP( old, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP,
    //                                           88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, true );
    //                            } else {
    //                                consoleSetWindow( &Top, 4, 2, 20, 2 );
    //                                consoleClear( );
    //                                printf( "%ls%c\nLv%d%4dKP", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name, GENDER( ACPKMN( 1, OPPONENT ) ),
    //                                        ACPKMN( 1, OPPONENT ).m_Level, ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //                                displayHP( old, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP,
    //                                           0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, true );
    //                            }
    //                        }
    //                        printEFFLOG( ACPKMN( 1 - acin / 2, OPPONENT ), oppAtk[ acin / 2 ].first );
    //                    }
    //                } else {
    //                    int tg = ownAtk[ acin / 2 ].second;
    //                    int dmg1 = 0, dmg2 = 0, dmg3 = 0;
    //                    if( tg & 1 ) {
    //                        missed = false;
    //                        dmg1 = calcDamage( *AttackList[ ownAtk[ acin / 2 ].first ], ACPKMN( acin / 2, PLAYER ), ACPKMN( 0, OPPONENT ), rand( ) % 15 );
    //                        if( !missed ) {
    //                            int old = ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP;
    //                            ACPKMN( 0, OPPONENT ).m_stats.m_acHP = std::max( 0, ACPKMN( 0, OPPONENT ).m_stats.m_acHP - dmg1 );
    //                            if( !ACPKMN( 0, OPPONENT ).m_stats.m_acHP ) {
    //                                ko[ 0 ][ OPPONENT ] = true;
    //                                ACPKMNSTS( 0, OPPONENT ) = KO;
    //                            }
    //                            consoleSetWindow( &Top, 0, 5, 20, 2 );
    //                            consoleClear( );
    //                            printf( "%10ls%c\n", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name, GENDER( ACPKMN( 0, OPPONENT ) ) );
    //
    //                            if( ACPKMN( 0, OPPONENT ).m_Level < 10 )
    //                                printf( " " );
    //                            if( ACPKMN( 0, OPPONENT ).m_Level < 100 )
    //                                printf( " " );
    //
    //                            printf( "Lv%d%4dKP",
    //                                    ACPKMN( 0, OPPONENT ).m_Level,
    //                                    ACPKMN( 0, OPPONENT ).m_stats.m_acHP );
    //
    //                            displayHP( old, 100 - ACPKMN( 0, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 0, OPPONENT ).m_stats.m_maxHP,
    //                                       88, 32, HP_COL( 1, 0 ), HP_COL( 1, 0 ) + 1, true );
    //                        }
    //                        printEFFLOG( ACPKMN( 0, OPPONENT ), ownAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 2 ) ) {
    //                        missed = false;
    //                        dmg2 = calcDamage( *AttackList[ ownAtk[ acin / 2 ].first ],
    //                                           ACPKMN( acin / 2, PLAYER ),
    //                                           ACPKMN( 1, OPPONENT ),
    //                                           rand( ) % 15 );
    //                        if( !missed ) {
    //                            int old = ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP;
    //
    //                            ACPKMN( 1, OPPONENT ).m_stats.m_acHP = std::max( 0, ACPKMN( 1, OPPONENT ).m_stats.m_acHP - dmg2 );
    //
    //                            if( !ACPKMN( 1, OPPONENT ).m_stats.m_acHP ) {
    //                                ko[ 1 ][ OPPONENT ] = true;
    //                                ACPKMNSTS( 1, OPPONENT ) = KO;
    //                            }
    //
    //                            consoleSetWindow( &Top, 4, 2, 20, 2 );
    //                            consoleClear( );
    //                            printf( "%ls%c\nLv%d%4dKP",
    //                                    ACPKMN( 1, OPPONENT ).m_boxdata.m_Name,
    //                                    GENDER( ACPKMN( 1, OPPONENT ) ),
    //                                    ACPKMN( 1, OPPONENT ).m_Level,
    //                                    ACPKMN( 1, OPPONENT ).m_stats.m_acHP );
    //                            displayHP( old, 100 - ACPKMN( 1, OPPONENT ).m_stats.m_acHP * 100 / ACPKMN( 1, OPPONENT ).m_stats.m_maxHP,
    //                                       0, 8, HP_COL( 1, 1 ), HP_COL( 1, 1 ) + 1, true );
    //
    //                        }
    //                        printEFFLOG( ACPKMN( 1, OPPONENT ), ownAtk[ acin / 2 ].first );
    //                    }
    //                    if( ( m_battleMode == DOUBLE ) && ( tg & 8 ) ) {
    //                        missed = false;
    //                        dmg3 = calcDamage( *AttackList[ ownAtk[ acin / 2 ].first ],
    //                                           ACPKMN( acin / 2, PLAYER ),
    //                                           ACPKMN( 1 - acin / 2, PLAYER ),
    //                                           rand( ) % 15 );
    //                        if( !missed ) {
    //                            int old = 0;
    //                            if( acin / 2 == 1 )
    //                                old = ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 0, PLAYER ).m_stats.m_maxHP;
    //                            else
    //                                old = ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP;
    //
    //                            ACPKMN( 1 - acin / 2, PLAYER ).m_stats.m_acHP = std::max( 0, ACPKMN( 1 - acin / 2, PLAYER ).m_stats.m_acHP - dmg3 );
    //                            if( !ACPKMN( 1 - acin / 2, PLAYER ).m_stats.m_acHP ) {
    //                                ko[ 1 - ( acin / 2 ) ][ PLAYER ] = true;
    //                                ACPKMNSTS( 1 - ( acin / 2 ), PLAYER ) = KO;
    //                            }
    //                            if( acin / 2 == 1 ) {
    //                                displayHP( old, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100 /
    //                                           ACPKMN( 0, PLAYER ).m_stats.m_maxHP, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, true );
    //                                consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                                consoleClear( );
    //                                printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                        GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                        ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //                            } else {
    //                                displayHP( old, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 / ACPKMN( 1, PLAYER ).m_stats.m_maxHP,
    //                                           256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, true );
    //                                consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                                consoleClear( );
    //                                printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                                if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                    printf( " " );
    //                                if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                    printf( " " );
    //                                printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                        ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //                            }
    //                        }
    //                        printEFFLOG( ACPKMN( 1 - acin / 2, PLAYER ), ownAtk[ acin / 2 ].first );
    //                    }
    //                }
    //
    //
    //                if( ko[ 0 ][ PLAYER ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls wurde besiegt.", ACPKMN( 0, PLAYER ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OWN_PKMN_1_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    consoleSelect( &Bottom );
    //
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //                }
    //                if( ko[ 1 ][ PLAYER ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls wurde besiegt.", ACPKMN( 1, PLAYER ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OWN_PKMN_2_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    consoleSelect( &Bottom );
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //                }
    //
    //                if( ko[ 0 ][ OPPONENT ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls (Gegner)\nwurde besiegt.", ACPKMN( 0, OPPONENT ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //                    consoleSetWindow( &Top, 0, 5, 20, 2 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OPP_PKMN_1_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //
    //                }
    //                if( ko[ 1 ][ OPPONENT ] ) {
    //                    clear( );
    //                    sprintf( buffer, "%ls (Gegner)\nwurde besiegt.", ACPKMN( 1, OPPONENT ).m_boxdata.m_Name );
    //                    cust_font.printString( buffer, 8, 8, true );
    //
    //                    consoleSetWindow( &Top, 4, 2, 20, 2 );
    //                    consoleClear( );
    //                    for( int i = 0; i < 4; ++i )
    //                        OamTop->oamBuffer[ OPP_PKMN_2_START + i + 1 ].isHidden = true;
    //                    updateOAM( OamTop );
    //
    //                    for( int i = 0; i < 75; ++i )
    //                        swiWaitForVBlank( );
    //                }
    //
    //
    //                consoleSelect( &Bottom );
    //
    //                if( ko[ 0 ][ OPPONENT ] || ko[ 1 ][ OPPONENT ] ) {
    //                    for( int i = 0; i < 6; ++i ) {
    //                        if( m_distributeEXP && ACPKMNSTS( i, PLAYER ) != acStatus::NA && ACPKMNSTS( i, PLAYER ) != acStatus::KO ) {
    //                            POKEMON::pokemon& acPK = ( *_player->m_pkmnTeam )[ ACPOS( i, PLAYER ) ];
    //                            POKEMON::pokemon& acDF = ACPKMN( 0, OPPONENT );
    //
    //                            POKEMON::PKMNDATA::getAll( ACPKMN( 0, OPPONENT ).m_boxdata.m_SPEC, p );
    //                            int exp = calcEXP( acPK, ACPOS( i, PLAYER ), acDF, false );
    //                            if( exp && acPK.m_Level < 100 ) {
    //                                int evsum = 0;
    //
    //                                for( int j = 0; j < 6; ++j ) {
    //                                    evsum += p.m_EVYield[ j ];
    //                                    acPK.m_boxdata.m_effortValues[ j ] += p.m_EVYield[ j ];
    //                                }
    //
    //                                clear( );
    //                                sprintf( buffer, "%ls erhält %d EV\nund %d E.-Punkte.", acPK.m_boxdata.m_name, evsum, exp );
    //                                cust_font.printString( buffer, 8, 8, true );
    //
    //                                POKEMON::PKMNDATA::getAll( ( *_player->m_pkmnTeam )[ ACPOS( i, PLAYER ) ].m_boxdata.m_SPEC, p );
    //                                int old = ( acPK.m_boxdata.m_experienceGained - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                    ( POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] );
    //
    //                                acPK.m_boxdata.m_experienceGained += exp;
    //
    //                                int nw = std::min( 100u, ( acPK.m_boxdata.m_experienceGained - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                   ( POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) );
    //
    //
    //                                if( i == 0 )
    //                                    displayEP( old, nw, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, true );
    //
    //                                if( ( m_battleMode == DOUBLE ) && i == 1 )
    //                                    displayEP( old, nw, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, true );
    //                                for( int i = 0; i < 75; ++i )
    //                                    swiWaitForVBlank( );
    //                                bool newLevel = acPK.m_Level < 100 && POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] <= acPK.m_boxdata.m_experienceGained;
    //                                bool nL = newLevel;
    //
    //                                u16 HPdif = acPK.m_stats.m_maxHP - acPK.m_stats.m_acHP;
    //
    //                                while( newLevel ) {
    //                                    acPK.m_Level++;
    //
    //                                    if( acPK.m_boxdata.m_speciesId != 292 )
    //                                        acPK.m_stats.m_maxHP = ( ( acPK.m_boxdata.m_individualValues.m_hp + 2 * p.m_bases[ 0 ] + ( acPK.m_boxdata.m_effortValues[ 0 ] / 4 ) + 100 )* acPK.m_Level / 100 ) + 10;
    //                                    else
    //                                        acPK.m_stats.m_maxHP = 1;
    //                                    POKEMON::pkmnNatures nature = acPK.m_boxdata.getNature( );
    //                                    acPK.m_stats.m_Atk = ( ( ( acPK.m_boxdata.m_individualValues.m_moves + 2 * p.m_bases[ 1 ] + ( acPK.m_boxdata.m_effortValues[ 1 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ PLAYER ];
    //                                    acPK.m_stats.m_Def = ( ( ( acPK.m_boxdata.m_individualValues.m_defense + 2 * p.m_bases[ 2 ] + ( acPK.m_boxdata.m_effortValues[ 2 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ OPPONENT ];
    //                                    acPK.m_stats.m_Spd = ( ( ( acPK.m_boxdata.m_individualValues.m_speed + 2 * p.m_bases[ 3 ] + ( acPK.m_boxdata.m_effortValues[ 3 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ 2 ];
    //                                    acPK.m_stats.m_SAtk = ( ( ( acPK.m_boxdata.m_individualValues.m_sAttack + 2 * p.m_bases[ 4 ] + ( acPK.m_boxdata.m_effortValues[ 4 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ 3 ];
    //                                    acPK.m_stats.m_SDef = ( ( ( acPK.m_boxdata.m_individualValues.m_sDefense + 2 * p.m_bases[ 5 ] + ( acPK.m_boxdata.m_effortValues[ 5 ] >> 2 ) )*acPK.m_Level / 100.0 ) + 5 )*POKEMON::NatMod[ nature ][ 4 ];
    //
    //                                    acPK.m_stats.m_acHP = acPK.m_stats.m_maxHP - HPdif;
    //
    //                                    clear( );
    //                                    sprintf( buffer, "%ls erreicht Level %d.", acPK.m_boxdata.m_name, acPK.m_Level );
    //                                    cust_font.printString( buffer, 8, 8, true );
    //                                    for( int i = 0; i < 75; ++i )
    //                                        swiWaitForVBlank( );
    //
    //                                    nw = std::min( 100u, ( acPK.m_boxdata.m_experienceGained - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                   ( POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPK.m_Level - 1 ][ p.m_expType ] ) );
    //
    //                                    if( i == 0 ) {
    //                                        displayEP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //                                        displayEP( 0, nw, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, true );
    //                                        consoleSelect( &Top );
    //                                        consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                                        consoleClear( );
    //                                        printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                                GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                                ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //                                    }
    //                                    if( ( m_battleMode == DOUBLE ) && i == 1 ) {
    //                                        displayEP( 100, 101, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //                                        displayEP( 0, nw, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, true );
    //                                        consoleSelect( &Top );
    //                                        consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                                        consoleClear( );
    //                                        printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                                        if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                            printf( " " );
    //                                        if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                            printf( " " );
    //                                        printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                                ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //                                    }
    //                                    newLevel = acPK.m_Level < 100 && (unsigned)POKEMON::EXP[ acPK.m_Level ][ p.m_expType ] <= acPK.m_boxdata.m_experienceGained;
    //                                }
    //
    //                                if( nL && SAV.m_evolveInBattle ) {
    //                                    consoleSelect( &Top );
    //                                    if( acPK.canEvolve( ) ) {
    //                                        clear( );
    //                                        sprintf( buffer, "%ls entwickelt sich!", acPK.m_boxdata.m_name );
    //                                        cust_font.printString( buffer, 8, 8, true );
    //                                        for( int i = 0; i < 75; ++i )
    //                                            swiWaitForVBlank( );
    //
    //                                        acPK.evolve( );
    //
    //                                        if( i == 0 ) {
    //
    //                                            oamIndex = OWN_PKMN_1_START;
    //                                            palcnt = OWN_PKMN_1_PAL;
    //                                            nextAvailableTileIdx = OWN_PKMN_1_TILE;
    //
    //                                            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale ) )
    //                                                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, -10, 100, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 0, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 0, PLAYER ).m_boxdata.m_isFemale );
    //
    //                                            updateOAM( OamTop );
    //
    //
    //                                            displayHP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //                                            displayHP( 100, 100 - ACPKMN( 0, PLAYER ).m_stats.m_acHP * 100
    //                                                       / ACPKMN( 0, PLAYER ).m_stats.m_maxHP, 256 - 96 - 28, 192 - 32 - 8 - 32, HP_COL( 0, 0 ), HP_COL( 0, 0 ) + 1, false );
    //                                            displayEP( 100, 101, 256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //
    //                                            POKEMON::PKMNDATA::getAll( ACPKMN( 0, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //                                            displayEP( 0, ( ACPKMN( 0, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                       ( POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level ][ p.m_expType ] -
    //                                                       POKEMON::EXP[ ACPKMN( 0, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                                                       256 - 96 - 28, 192 - 32 - 8 - 32, OWN1_EP_COL, OWN1_EP_COL + 1, false );
    //                                            consoleSelect( &Top );
    //                                            consoleSetWindow( &Top, 21, 16, 20, 4 );
    //                                            consoleClear( );
    //                                            printf( "%ls%c\nLv%d%4dKP", ACPKMN( 0, PLAYER ).m_boxdata.m_Name,
    //                                                    GENDER( ACPKMN( 0, PLAYER ) ), ACPKMN( 0, PLAYER ).m_Level,
    //                                                    ACPKMN( 0, PLAYER ).m_stats.m_acHP );
    //
    //                                        }
    //                                        if( ( m_battleMode == DOUBLE ) && i == 1 ) {
    //                                            oamIndex = OWN_PKMN_2_START;
    //                                            palcnt = OWN_PKMN_2_PAL;
    //                                            nextAvailableTileIdx = OWN_PKMN_2_TILE;
    //
    //                                            if( !loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale ) )
    //                                                loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, 50, 120, oamIndex, palcnt, nextAvailableTileIdx, false,
    //                                                ACPKMN( 1, PLAYER ).m_boxdata.isShiny( ), !ACPKMN( 1, PLAYER ).m_boxdata.m_isFemale );
    //
    //                                            updateOAM( OamTop );
    //
    //                                            displayHP( 100, 101, 256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //                                            displayHP( 100, 100 - ACPKMN( 1, PLAYER ).m_stats.m_acHP * 100 /
    //                                                       ACPKMN( 1, PLAYER ).m_stats.m_maxHP, 256 - 36, 192 - 40, HP_COL( 0, 1 ), HP_COL( 0, 1 ) + 1, false );
    //                                            displayEP( 100, 100, 256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //                                            POKEMON::PKMNDATA::getAll( ACPKMN( 1, PLAYER ).m_boxdata.m_SPEC, p );
    //
    //                                            displayEP( 0, ( ACPKMN( 1, PLAYER ).m_boxdata.m_exp - POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ) * 100 /
    //                                                       ( POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level ][ p.m_expType ] -
    //                                                       POKEMON::EXP[ ACPKMN( 1, PLAYER ).m_Level - 1 ][ p.m_expType ] ),
    //                                                       256 - 36, 192 - 40, OWN2_EP_COL, OWN2_EP_COL + 1, false );
    //
    //                                            consoleSelect( &Top );
    //                                            consoleSetWindow( &Top, 16, 20, 20, 5 );
    //                                            consoleClear( );
    //                                            printf( "%10ls%c\n", ACPKMN( 1, PLAYER ).m_boxdata.m_Name, GENDER( ACPKMN( 1, PLAYER ) ) );
    //                                            if( ACPKMN( 1, PLAYER ).m_Level < 10 )
    //                                                printf( " " );
    //                                            if( ACPKMN( 1, PLAYER ).m_Level < 100 )
    //                                                printf( " " );
    //                                            printf( "Lv%d%4dKP", ACPKMN( 1, PLAYER ).m_Level,
    //                                                    ACPKMN( 1, PLAYER ).m_stats.m_acHP );
    //
    //                                        }
    //
    //                                        clear( );
    //                                        sprintf( buffer, "Und wurde zu einem\n%ls.", acPK.m_boxdata.m_name );
    //                                        cust_font.printString( buffer, 8, 8, true );
    //
    //                                        for( int i = 0; i < 75; ++i )
    //                                            swiWaitForVBlank( );
    //                                    }
    //                                }
    //                            }
    //                        }
    //                    }
    //                }
    //
    //            }
    //
    //
    //
    //        }
    //END:
    //        dinit( );
    //        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
    //        consoleSelect( &Bottom );
    //        consoleClear( );
    //        initOAMTableSub( Oam );
    //        initMainSprites( Oam, spriteInfo );
    //        setMainSpriteVisibility( false );
    //        Oam->oamBuffer[ 8 ].isHidden = true;
    //        Oam->oamBuffer[ 0 ].isHidden = true;
    //        Oam->oamBuffer[ 1 ].isHidden = false;
    //        return 0;
    //    }
}