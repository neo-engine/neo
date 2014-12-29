


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

#include "Back.h"
#include "A.h"
#include "map2d.h"
#include "messageBox.h"

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

#define BG_PAL( p_sub ) ( ( p_sub ) ? BG_PALETTE_SUB : BG_PALETTE )
#define BG_BMP( p_sub ) ( ( p_sub ) ? BG_BMP_RAM_SUB( 1 ) : BG_BMP_RAM( 1 ) )

    void battleUI::displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, bool p_big ) {
        if( p_big )
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 20, 24 );
        else
            displayHP( p_HPstart, p_HP, p_x, p_y, p_freecolor1, p_freecolor2, p_delay, 8, 12 );
    }
    void battleUI::displayHP( u16 p_HPstart, u16 p_HP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        p_HP = std::max( std::min( (u16)101, p_HP ), u16( 0 ) );
        u16 factor = std::max( 1, p_outerR / 15 );
        if( p_HP > 100 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = GREEN;
            for( u16 i = 0; i < factor * 100; ++i )
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 + j * ( sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) ),
                        ny = p_y + 16 + j * ( cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) );
                    ( (color *)( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor1 ) << 8 ) | (u8)p_freecolor1;
                    //printf("%i %i; ",nx,ny);
                }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = NORMAL_;
            for( u16 i = factor * 100 - factor*p_HPstart; i < factor*p_HP; ++i ) {
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 + j * ( sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) ),
                        ny = p_y + 16 + j * ( cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) );
                    ( (color *)( BG_BMP( p_sub ) ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor2 ) << 8 ) | (u8)p_freecolor2;
                    if( i == factor * 50 )
                        BG_PAL( p_sub )[ p_freecolor1 ] = YELLOW;
                    if( i == factor * 80 )
                        BG_PAL( p_sub )[ p_freecolor1 ] = RED;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }
    void battleUI::displayEP( u16 p_EPstart, u16 p_EP, u8 p_x, u8 p_y, u8 p_freecolor1, u8 p_freecolor2, bool p_delay, u8 p_innerR, u8 p_outerR, bool p_sub ) {
        u16 factor = std::max( 1, p_outerR / 15 );
        if( p_EPstart >= 100 || p_EP > 100 ) {
            BG_PAL( p_sub )[ p_freecolor1 ] = NORMAL_;
            for( u16 i = 0; i < factor * 100; ++i )
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 + j * ( sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) ),
                        ny = p_y + 16 + j * ( cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) );
                    ( (color *)BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor1 ) << 8 ) | (u8)p_freecolor1;
                    //printf("%i %i; ",nx,ny);
                }
        } else {
            BG_PAL( p_sub )[ p_freecolor2 ] = ICE;
            for( u16 i = p_EPstart*factor; i <= p_EP*factor; ++i ) {
                for( u16 j = p_innerR; j <= p_outerR; ++j ) {
                    u16 nx = p_x + 16 + j * ( sin( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) ),
                        ny = p_y + 16 + j * ( cos( ( 50 - i / ( 1.0*factor ) )*acos( 0 ) / 30 + 0.001f ) );
                    ( (color *)BG_BMP( p_sub ) )[ ( nx + ny * SCREEN_WIDTH ) / 2 ] = ( ( (u8)p_freecolor2 ) << 8 ) | (u8)p_freecolor2;
                }
                if( p_delay )
                    swiWaitForVBlank( );
            }
        }
    }
    void initColors( ) {

        cust_font.setColor( 0, 0 );
        cust_font.setColor( BLACK_IDX, 1 );
        cust_font.setColor( GRAY_IDX, 2 );
        cust_font2.setColor( 0, 0 );
        cust_font2.setColor( GRAY_IDX, 1 );
        cust_font2.setColor( WHITE_IDX, 2 );

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
        FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, WHITE_IDX );
    }
    void battleUI::clearLogScreen( ) {
        FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)63, true, false, WHITE_IDX );
    }
    void battleUI::setLogTextColor( u16 p_color ) {
        BG_PALETTE_SUB[ COLOR_IDX ] = BG_PALETTE[ COLOR_IDX ] = p_color;
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
            p_tilecnt = loadSprite( p_bottom ? Oam : OamTop, p_bottom ? spriteInfo : spriteInfoTop, p_bottom ? i : ( STSBALL_START + i ),
                                    p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, p_bottom ? ( 16 * i ) : 240 - ( 16 * i ),
                                    180, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                    false, false, false, OBJPRIORITY_0, p_bottom );
        }
        //Opps PKMNs PBs
        for( u8 i = 0; i < 6; ++i ) {
            auto acStat = ACPKMNSTS2( *p_battle, i, OPPONENT );
            p_tilecnt = loadSprite( p_bottom ? Oam : OamTop, p_bottom ? spriteInfo : spriteInfoTop, p_bottom ? ( 6 + i ) : ( STSBALL_START + 6 + i ),
                                    p_bottom ? PB_PAL_SUB( acStat ) : PB_PAL_TOP( acStat ), p_tilecnt, !p_bottom ? ( 16 * i ) : 240 - ( 16 * i ),
                                    -4, 16, 16, PB_PAL( acStat ), PB_TILES( acStat ), PB_TILES_LEN( acStat ),
                                    false, false, false, OBJPRIORITY_0, p_bottom );
        }
        return p_tilecnt;
    }
    void setStsBallVisibility( bool p_opponent, u8 p_pokemonPos, bool p_isHidden, bool p_bottom ) {
        if( p_bottom ) {
            Oam->oamBuffer[ 6 * p_opponent + p_pokemonPos ].isHidden = p_isHidden;
            updateOAMSub( Oam );
        } else {
            OamTop->oamBuffer[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].isHidden = p_isHidden;
            updateOAM( OamTop );
        }
    }
    void setStsBallPosition( bool p_opponent, u8 p_pokemonPos, u8 p_x, u8 p_y, bool p_bottom ) {
        if( p_bottom ) {
            Oam->oamBuffer[ 6 * p_opponent + p_pokemonPos ].x = p_x;
            Oam->oamBuffer[ 6 * p_opponent + p_pokemonPos ].y = p_y;
            updateOAMSub( Oam );
        } else {
            OamTop->oamBuffer[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].x = p_x;
            OamTop->oamBuffer[ STSBALL_IDX( p_pokemonPos, p_opponent ) ].y = p_y;
            updateOAM( OamTop );
        }
    }
    void setStsBallSts( bool p_opponent, u8 p_pokemonPos, battle::acStatus p_status, bool p_bottom ) {
        u8 idx = 0;
        u16 tileIdx = 0;
        if( p_bottom ) {
            idx = 6 * p_opponent + p_pokemonPos;
            tileIdx = Oam->oamBuffer[ idx ].gfxIndex;
            loadSprite( Oam, spriteInfo, idx, PB_PAL_SUB( p_status ), tileIdx, p_opponent ? ( 240 - ( 16 * p_pokemonPos ) ) : ( 16 * ( 6 - p_pokemonPos ) ),
                        p_opponent ? -4 : 180, 16, 16, PB_PAL( p_status ), PB_TILES( p_status ), PB_TILES_LEN( p_status ),
                        false, false, false, OBJPRIORITY_0, p_bottom );
            updateOAMSub( Oam );
        } else {
            idx = STSBALL_IDX( p_pokemonPos, p_opponent );
            tileIdx = OamTop->oamBuffer[ idx ].gfxIndex;
            loadSprite( OamTop, spriteInfoTop, idx, PB_PAL_TOP( p_status ), tileIdx, !p_opponent ? ( 240 - ( 16 * p_pokemonPos ) ) : ( 16 * ( p_pokemonPos ) ),
                        p_opponent ? -4 : 180, 16, 16, PB_PAL( p_status ), PB_TILES( p_status ), PB_TILES_LEN( p_status ),
                        false, false, false, OBJPRIORITY_0, p_bottom );
            updateOAM( OamTop );
        }
    }

    void loadSpritesTop( battle* p_battle ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        bg2 = bgInit( 2, BgType_Bmp8, BgSize_B8_256x256, 1, 0 );
        bg3 = bgInit( 3, BgType_Bmp8, BgSize_B8_256x256, 5, 0 );
        bgSetPriority( bg3, 3 );
        bgSetPriority( bg2, 2 );
        initOAMTable( OamTop );
        dmaFillWords( 0, bgGetGfxPtr( bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( bg3 ), 256 * 192 );
        bgUpdate( );


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
        initColors( );

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

        TILESTART = initStsBalls( false, p_battle, TILESTART = ( PKMN_TILE_START + 4 * 144 ) );

        for( u8 i = 0; i < 4; ++i ) {
            TILESTART = loadSprite( OamTop, spriteInfoTop, HP_IDX( i % 2, ( i / 2 ) ), HP_PAL,
                                    TILESTART, 0, 0, 32, 32, Battle1Pal,
                                    Battle1Tiles, Battle1TilesLen, false,
                                    false, true, OBJPRIORITY_2, false );
        }

        updateOAM( OamTop );
    }
    void loadBattleUISub( u16 p_pkmnId, bool p_isWildBattle, bool p_showNav ) {
        u16 tilecnt = 0;
        //Load UI Sprites
        //FIGHT -- 1
        tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START, 0, tilecnt, 64,
                              72, 64, 64, BattleSub1Pal, BattleSub1Tiles, BattleSub1TilesLen,
                              false, false, false, OBJPRIORITY_2, true );
        tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START + 1, 0, tilecnt, 128,
                              72, 64, 64, BattleSub1Pal, BattleSub1Tiles, BattleSub1TilesLen,
                              false, true, false, OBJPRIORITY_2, true );
        //FIGHT-TEXT
        tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START + 2, 1, tilecnt, 96,
                              100, 64, 32, BattleSub2Pal, BattleSub2Tiles, BattleSub2TilesLen,
                              false, false, false, OBJPRIORITY_0, true );
        //RUN / POKENAV
        if( p_isWildBattle ) { //Show Run
            tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START + 3, 2, tilecnt, 91,
                                  150, 64, 32, BattleSub3Pal, BattleSub3Tiles, BattleSub3TilesLen,
                                  false, false, false, OBJPRIORITY_3, true );
        } else if( p_showNav ) { //Show Nav
            tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START + 3, 2, tilecnt, 91,
                                  150, 64, 32, BattleSub6Pal, BattleSub6Tiles, BattleSub6TilesLen,
                                  false, false, false, OBJPRIORITY_3, true );
        }
        //BAG
        tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START + 4, 3, tilecnt, 0,
                              157, 64, 32, BattleSub4Pal, BattleSub4Tiles, BattleSub4TilesLen,
                              false, false, false, OBJPRIORITY_3, true );
        //POKEMON
        tilecnt = loadSprite( Oam, spriteInfo, SUB_FIGHT_START + 5, 4, tilecnt, 185,
                              142, 64, 32, BattleSub5Pal, BattleSub5Tiles, BattleSub5TilesLen,
                              false, false, false, OBJPRIORITY_3, true );
        //Load an icon of the PKMN, too
        u8 oamIndex = SUB_FIGHT_START + 5;
        u8 palIndex = 5;

        FS::drawPKMNIcon( Oam, spriteInfo, p_pkmnId, 112, 68, oamIndex, palIndex, tilecnt, true );

        //PreLoad A and Back buttons

        tilecnt = loadSprite( Oam, spriteInfo, SUB_A_OAM, 6, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                              ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        FONT::ASpriteOamIndex = SUB_A_OAM;
        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 7, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, true, OBJPRIORITY_0, true );

        SUB_TILESTART = tilecnt;
        SUB_PALSTART = 8;
        updateOAMSub( Oam );
    }
    void setBattleUISubVisibility( bool p_isHidden = false ) {
        for( u8 i = 0; i <= SUB_FIGHT_START + 6; ++i )
            Oam->oamBuffer[ i ].isHidden = p_isHidden;
        updateOAMSub( Oam );
    }

    void loadA( ) {
        loadSprite( Oam, spriteInfo, SUB_A_OAM, 6, Oam->oamBuffer[ SUB_A_OAM ].gfxIndex,
                    SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, APal,
                    ATiles, ATilesLen, false, false, true, OBJPRIORITY_0, true );
        FONT::ASpriteOamIndex = SUB_A_OAM;
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
        initOAMTableSub( Oam );
        Oam->oamBuffer[ SUB_A_OAM ].gfxIndex = 0;
        loadA( );
    }

    void battleUI::trainerIntro( ) {
        //Use a 2D Map as top background for trainer intro
        loadSpritesSub( _battle );
        loadSpritesTop( _battle ); // This should consume some time

        loadBattleUITop( _battle );
        initOAMTableSub( Oam );
        drawSub( );
        setBattleUISubVisibility( );
        initLogScreen( );
    }

    void setDeclareBattleMoveSpriteVisibility( bool p_showBack, bool p_isHidden = true ) {
        if( p_showBack )
            Oam->oamBuffer[ SUB_Back_OAM ].isHidden = p_isHidden;
        setBattleUISubVisibility( p_isHidden );
    }


    u8 firstMoveSwitchTarget = 0;
    bool battleUI::declareBattleMove( u8 p_pokemonPos, bool p_showBack ) {
        wchar_t wbuffer[ 100 ];
        swprintf( wbuffer, 50, L"Was soll %ls tun?", ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_name );
        writeLogText( wbuffer );

        loadBattleUISub( ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                         _battle->m_isWildBattle, !_battle->m_isWildBattle && SAV.m_activatedPNav );
        if( p_showBack ) {
            Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
            updateOAMSub( Oam );
        }

        touchPosition t;
        auto& result = _battle->_battleMoves[ p_pokemonPos ][ PLAYER ];
        while( 1 ) {
            updateTime( false );
            scanKeys( );
            t = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( p_showBack && t.px > 224 && t.py > 164 ) {
                waitForTouchUp( );
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                return false;

            } else if( t.px > 74 && t.px < 181 && t.py > 81 && t.py < 125 ) {//Attacks
                waitForTouchUp( );
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
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && SAV.m_activatedPNav );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( wbuffer );
            } else if( t.px < 58 && t.py > 162 && t.py <= 192 ) {//Bag
                waitForTouchUp( );
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
            } else if( !_battle->m_isWildBattle && SAV.m_activatedPNav
                       && t.px > 95 && t.px < 152 && t.py > 152 && t.py < 178 ) {//Nav
                waitForTouchUp( );
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

            } else if( _battle->m_isWildBattle && t.px > 97 && t.px < 153 && t.py > 162 && t.py < 180 ) {//Run
                waitForTouchUp( );
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

            } else if( t.px > 195 && t.px < 238 && t.py > 148 && t.py < 176 ) {//Pokémon
                waitForTouchUp( );
                setDeclareBattleMoveSpriteVisibility( p_showBack );
                clearLogScreen( );
                result.m_type = battle::battleMove::SWITCH;
                result.m_value = choosePKMN( p_pokemonPos + ( _battle->m_battleMode == battle::DOUBLE ) );
                if( result.m_value ) {
                    loadA( );
                    return true;
                }
                loadBattleUISub( ACPKMN2( *_battle, p_pokemonPos, PLAYER ).m_boxdata.m_speciesId,
                                 _battle->m_isWildBattle, !_battle->m_isWildBattle && SAV.m_activatedPNav );
                setDeclareBattleMoveSpriteVisibility( p_showBack, false );
                writeLogText( wbuffer );
            }
        }
    }

    u16 battleUI::chooseAttack( u8 p_pokemonPos ) {
        u16 result = 0;

        writeLogText( L"Welche Attacke?" );

        Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;

        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 0, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &Bottom );

        auto acPkmn = ACPKMN2( *_battle, p_pokemonPos, PLAYER );

        for( u8 i = 0; i < 4; ++i ) {
            if( acPkmn.m_boxdata.m_moves[ i ] ) {
                auto acMove = AttackList[ acPkmn.m_boxdata.m_moves[ i ] ];

                BG_PALETTE_SUB[ 240 + i ] = POKEMON::PKMNDATA::getColor( acMove->m_moveType );

                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

                FONT::putrec( x + 1, y + 1, x + w + 2, y + h + 1,
                              true, false, BLACK_IDX );
                FONT::putrec( x, y, x + w, y + h,
                              true, false, 240 + i );
                FONT::putrec( x + 7, y + 5, x + w - 4, y + h - 1,
                              true, false, BLACK_IDX );
                FONT::putrec( x + 6, y + 4, x + w - 6, y + h - 2,
                              true, false, WHITE_IDX );

                cust_font.printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                drawTypeIcon( Oam, spriteInfo, oamIndex, palIndex, tilecnt, acMove->m_moveType, x - 7, y - 7, true );
                consoleSelect( &Bottom );
                consoleSetWindow( &Bottom, x / 8, 12 + ( i / 2 ) * 6, 20, 2 );
                printf( "%6hhu/%2hhu AP",
                        acPkmn.m_boxdata.m_acPP[ i ],
                        AttackList[ acPkmn.m_boxdata.m_moves[ i ] ]->m_movePP * ( ( 5 + acPkmn.m_boxdata.m_ppup.m_Up1 ) / 5 ) );
            }
        }

        updateOAMSub( Oam );

        touchPosition t;
        while( 42 ) {
NEXT:
            updateTime( false );
            scanKeys( );
            t = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( t.px > 224 && t.py > 164 ) { //Back
                waitForTouchUp( );
                result = 0;
                break;
            }
            for( u8 i = 0; i < 4; ++i ) {
                if( !acPkmn.m_boxdata.m_moves[ i ] )
                    break;
                u8 w = 104, h = 32;
                u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );
                if( t.px >= x && t.py >= y && t.px <= x + w && t.py <= y + h
                    && acPkmn.m_boxdata.m_acPP[ i ] ) {
                    auto acMove = AttackList[ acPkmn.m_boxdata.m_moves[ i ] ];

                    FONT::putrec( x, y, x + w, y + h,
                                  true, false, 0 );
                    FONT::putrec( x + 1, y + 1, x + w + 2, y + h + 1,
                                  true, false, 240 + i );
                    FONT::putrec( x + 8, y + 6, x + w - 2, y + h,
                                  true, false, WHITE_IDX );

                    cust_font.printString( acMove->m_moveName.c_str( ), x + 9, y + 9, true );

                    while( 1 ) {
                        swiWaitForVBlank( );
                        updateTime( false );
                        scanKeys( );
                        auto t = touchReadXY( );
                        if( t.px == 0 && t.py == 0 )
                            break;
                        if( !( t.px >= x && t.py >= y && t.px <= x + w && t.py <= y + h ) ) {
                            FONT::putrec( x + 1, y + 1, x + w + 2, y + h + 1,
                                          true, false, BLACK_IDX );
                            FONT::putrec( x, y, x + w, y + h,
                                          true, false, 240 + i );
                            FONT::putrec( x + 7, y + 5, x + w - 4, y + h - 1,
                                          true, false, BLACK_IDX );
                            FONT::putrec( x + 6, y + 4, x + w - 6, y + h - 2,
                                          true, false, WHITE_IDX );

                            cust_font.printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                            goto NEXT;
                        }
                    }

                    result = acPkmn.m_boxdata.m_moves[ i ];
                    goto END;
                }
            }
        }
END:
        drawSub( );
        initColors( );
        clearLogScreen( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            Oam->oamBuffer[ i ].isHidden = true;
        updateOAMSub( Oam );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );
        return result;
    }

    u8 battleUI::chooseAttackTarget( u8 p_pokemonPos, u16 p_move ) {
        u8 result = 0;

        writeLogText( L"Welches PKMN angreifen?" );

        Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;

        u16 tilecnt = 0;
        u8  palIndex = 3;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 0, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &Bottom );

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
            FONT::putrec( 112 + 1, 130 + 1, 112 + 16 + 2, 146 + 1, true, false, BLACK_IDX );
        if( selected[ 0 ] && selected[ 1 ] )
            FONT::putrec( 120 + 1, 82 + 1, 120 + 16 + 2, 98 + 1, true, false, BLACK_IDX );

        if( selected[ 1 ] && selected[ 2 ] )
            FONT::putrec( 56 + 1, 106 + 1, 56 + 16 + 2, 122 + 1, true, false, BLACK_IDX );
        if( selected[ 3 ] && selected[ 0 ] )
            FONT::putrec( 176 + 1, 106 + 1, 176 + 16 + 2, 122 + 1, true, false, BLACK_IDX );

        _battle->_battleMoves[ p_pokemonPos ][ PLAYER ].m_target = 1 | 2 | 4 | 8;

        for( u8 i = 0; i < 4; ++i ) {
            u8 aI = i % 2;
            if( 1 - ( i / 2 ) )
                aI = ( 1 - aI );

            auto acPkmn = ACPKMN2( *_battle, aI, 1 - ( i / 2 ) );

            u8 w = 104, h = 32;
            u8 x = 16 - 8 * ( i / 2 ) + ( w + 16 ) * ( i % 2 ), y = 74 + ( h + 16 ) * ( i / 2 );

            FONT::putrec( x + 1, y + 1, x + w + 2, y + h + 1,
                          true, false, BLACK_IDX );
            FONT::putrec( x, y, x + w, y + h,
                          true, false, selected[ i ] ? RED_IDX : GRAY_IDX );
            FONT::putrec( x + 7, y + 5, x + w - 4, y + h - 1,
                          true, false, BLACK_IDX );
            FONT::putrec( x + 6, y + 4, x + w - 6, y + h - 2,
                          true, false, WHITE_IDX );

            if( neverTarget[ i ] )
                continue;

            if( acPkmn.m_stats.m_acHP ) {
                cust_font.printString( acPkmn.m_boxdata.m_name, x + 7, y + 7, true );
                FS::drawPKMNIcon( Oam, spriteInfo, acPkmn.m_boxdata.m_speciesId,
                                  x - 10, y - 23, oamIndex, palIndex, tilecnt );
            }
        }

        if( selected[ 2 ] && selected[ 3 ] )
            FONT::putrec( 112, 130, 112 + 16, 146, true, false, RED_IDX );
        if( selected[ 0 ] && selected[ 1 ] )
            FONT::putrec( 120, 82, 120 + 16, 98, true, false, RED_IDX );

        if( selected[ 1 ] && selected[ 2 ] )
            FONT::putrec( 56, 106, 56 + 16, 122, true, false, RED_IDX );
        if( selected[ 3 ] && selected[ 0 ] )
            FONT::putrec( 176, 106, 176 + 16, 122, true, false, RED_IDX );

        updateOAMSub( Oam );

        touchPosition t;
        while( 42 ) {
NEXT:
            updateTime( false );
            scanKeys( );
            t = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( t.px > 224 && t.py > 164 ) { //Back
                waitForTouchUp( );
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
                if( t.px >= x && t.py >= y && t.px <= x + w && t.py <= y + h ) {

                    for( u8 j = 0; j < 4; ++j ) {
                        if( !selected[ j ] && j != i )
                            continue;

                        u8 aJ = j % 2;
                        if( 1 - ( j / 2 ) )
                            aJ = ( 1 - aJ );

                        auto acPkmnJ = ACPKMN2( *_battle, aJ, 1 - ( i / 2 ) );

                        u8 nx = 16 - 8 * ( j / 2 ) + ( w + 16 ) * ( j % 2 ), ny = 74 + ( h + 16 ) * ( j / 2 );
                        FONT::putrec( nx, ny, x + w, y + h,
                                      true, false, 0 );
                        FONT::putrec( nx + 1, ny + 1, nx + w + 2, ny + h + 1,
                                      true, false, RED_IDX );
                        FONT::putrec( nx + 8, ny + 6, nx + w - 2, ny + h,
                                      true, false, WHITE_IDX );
                        if( neverTarget[ j ] || !acPkmnJ.m_stats.m_acHP )
                            continue;
                        cust_font.printString( acPkmnJ.m_boxdata.m_name, nx + 9, ny + 9, true );
                    }

                    while( 1 ) {
                        swiWaitForVBlank( );
                        updateTime( false );
                        scanKeys( );
                        auto t = touchReadXY( );
                        if( t.px == 0 && t.py == 0 )
                            break;
                        if( !( t.px >= x && t.py >= y && t.px <= x + w && t.py <= y + h ) ) {
                            for( u8 j = 0; j < 4; ++j ) {
                                if( !selected[ j ] && j != i )
                                    continue;

                                u8 aJ = j % 2;
                                if( 1 - ( j / 2 ) )
                                    aJ = ( 1 - aJ );

                                auto acPkmnJ = ACPKMN2( *_battle, aJ, 1 - ( i / 2 ) );

                                u8 nx = 16 - 8 * ( j / 2 ) + ( w + 16 ) * ( j % 2 ), ny = 74 + ( h + 16 ) * ( j / 2 );

                                FONT::putrec( nx + 1, ny + 1, nx + w + 2, ny + h + 1,
                                              true, false, BLACK_IDX );
                                FONT::putrec( nx, ny, nx + w, ny + h,
                                              true, false, selected[ i ] ? RED_IDX : GRAY_IDX );
                                FONT::putrec( nx + 7, ny + 5, nx + w - 4, ny + h - 1,
                                              true, false, BLACK_IDX );
                                FONT::putrec( nx + 6, ny + 4, nx + w - 6, ny + h - 2,
                                              true, false, WHITE_IDX );
                                if( neverTarget[ j ] || !acPkmnJ.m_stats.m_acHP )
                                    continue;
                                cust_font.printString( acPkmnJ.m_boxdata.m_name, nx + 7, ny + 7, true );
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
        drawSub( );
        initColors( );
        clearLogScreen( );
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            Oam->oamBuffer[ i ].isHidden = true;
        updateOAMSub( Oam );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );

        return result;
    }

    u16 battleUI::chooseItem( u8 p_pokemonPos ) {
        u8 result = 0;

        // Make this a debug battle end

        _battle->_round = 0;
        _battle->_maxRounds = -1;


        touchPosition t;
        while( 42 ) {
            updateTime( false );
            scanKeys( );
            t = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( t.px > 224 && t.py > 164 ) { //Back
                waitForTouchUp( );
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

        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 0, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        consoleSelect( &Bottom );

        if( p_battle->m_battleMode == battle::DOUBLE && p_battle->_battleMoves[ 0 ][ PLAYER ].m_type == battle::battleMove::SWITCH )
            firstMoveSwitchTarget = p_battle->_battleMoves[ 0 ][ PLAYER ].m_value;

        for( u8 i = 0; i < 6; ++i ) {
            u8 x = 8 + ( i % 2 ) * 120 - ( i / 2 ) * 4,
                y = 32 + ( i / 2 ) * 48;

            if( !i || ( p_firstIsChosen && ( i == 1 ) ) || i == firstMoveSwitchTarget ) {
                tilecnt = loadSprite( Oam, spriteInfo, SUB_CHOICE_START + 2 * i, 1, tilecnt,
                                      x, y, 64, 64, BattlePkmnChoice1Pal, BattlePkmnChoice1Tiles,
                                      BattlePkmnChoice1TilesLen, false, false, false, OBJPRIORITY_2, true );
                tilecnt = loadSprite( Oam, spriteInfo, SUB_CHOICE_START + 2 * i + 1, 1, tilecnt,
                                      x + 64, y, 64, 64, BattlePkmnChoice2Pal, BattlePkmnChoice2Tiles,
                                      BattlePkmnChoice2TilesLen, false, false, false, OBJPRIORITY_2, true );
            } else {
                tilecnt = loadSprite( Oam, spriteInfo, SUB_CHOICE_START + 2 * i, 2, tilecnt,
                                      x, y, 64, 64, BattlePkmnChoice3Pal, BattlePkmnChoice3Tiles,
                                      BattlePkmnChoice1TilesLen, false, false, false, OBJPRIORITY_2, true );
                tilecnt = loadSprite( Oam, spriteInfo, SUB_CHOICE_START + 2 * i + 1, 2, tilecnt,
                                      x + 64, y, 64, 64, BattlePkmnChoice4Pal, BattlePkmnChoice4Tiles,
                                      BattlePkmnChoice4TilesLen, false, false, false, OBJPRIORITY_2, true );
            }

            if( i >= p_battle->_player->m_pkmnTeam->size( ) )
                continue;

            auto& acPkmn = ACPKMN2( *p_battle, i, PLAYER );

            consoleSetWindow( &Bottom, ( x + 6 ) / 8, ( y + 6 ) / 8, 20, 8 );
            if( !acPkmn.m_boxdata.m_individualValues.m_isEgg ) {
                printf( "       Lv.%3d", acPkmn.m_Level );
                printf( "\n%14ls\n", acPkmn.m_boxdata.m_name );
                printf( "%14s\n\n",
                        ITEMS::ItemList[ acPkmn.m_boxdata.m_holdItem ].getDisplayName( ).c_str( ) );
                printf( "   %3i/%3i",
                        acPkmn.m_stats.m_acHP,
                        acPkmn.m_stats.m_maxHP );
                FS::drawPKMNIcon( Oam,
                                  spriteInfo,
                                  acPkmn.m_boxdata.m_speciesId,
                                  x + 4,
                                  y - 12,
                                  oamIndex,
                                  palIndex,
                                  tilecnt,
                                  true );
            } else {
                printf( "\n            Ei" );
                FS::drawEggIcon( Oam,
                                 spriteInfo,
                                 x + 4,
                                 y - 12,
                                 oamIndex,
                                 palIndex,
                                 tilecnt,
                                 true );
            }
        }

        updateOAMSub( Oam );
    }

    void undrawPKMNChoiceScreen( ) {
        for( u8 i = 0; i <= 3 * SUB_Back_OAM; ++i )
            Oam->oamBuffer[ i ].isHidden = true;
        updateOAMSub( Oam );
    }

    /**
    *  @returns 0 if the Pokemon shall be sent, 1 if further information was requested, 2 if the moves should be displayed, 3 if the previous screen shall be shown
    */
    u8 showConfirmation( POKEMON::pokemon& p_pokemon, bool p_alreadySent, bool p_alreadyChosen ) {
        drawSub( );
        Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
        u16 tilecnt = 0;
        u8  palIndex = 4;
        u8 oamIndex = SUB_Back_OAM + 1;

        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 0, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

        consoleSelect( &Bottom );

        bool dead = !p_pokemon.m_stats.m_acHP;
        u8 x = 104, y = 48;

        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            if( !FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                32, 32, oamIndex, palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale ) )
                FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                32, 32, oamIndex, palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale );

            consoleSetWindow( &Bottom, 13, 7, 20, 8 );
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
                    ITEMS::ItemList[ p_pokemon.m_boxdata.m_holdItem ].getDisplayName( ).c_str( ),
                    p_pokemon.m_Level,
                    p_pokemon.m_stats.m_acHP,
                    p_pokemon.m_stats.m_maxHP );
        } else {
            consoleSetWindow( &Bottom, 8, 11, 16, 10 );
            printf( "  Ein Ei kann\n nicht k\x84""mpfen!" );
            x = 64;
            y = 64;
        }
        //Switch
        tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 2, tilecnt,
                              x, y, 64, 64, Choice_4Pal, Choice_4Tiles,
                              Choice_4TilesLen, false, false, false, OBJPRIORITY_2, true );
        tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 2, tilecnt,
                              x + 64, y, 64, 64, Choice_4Pal, Choice_5Tiles,
                              Choice_5TilesLen, false, false, false, OBJPRIORITY_2, true );

        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            //Status
            tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 2, tilecnt,
                                  28, 128, 64, 32, Choice_1Pal, Choice_1Tiles,
                                  Choice_1TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 2, tilecnt,
                                  60, 128, 64, 32, Choice_3Pal, Choice_3Tiles,
                                  Choice_3TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 3, tilecnt,
                                  20, 128, 32, 32, memoPal, memoTiles,
                                  memoTilesLen, false, false, false, OBJPRIORITY_1, true );
            consoleSetWindow( &Bottom, 7, 17, 20, 8 );
            printf( "BERICHT" );

            //Moves
            tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 2, tilecnt,
                                  132, 128, 64, 32, Choice_1Pal, Choice_1Tiles,
                                  Choice_1TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 2, tilecnt,
                                  164, 128, 64, 32, Choice_3Pal, Choice_3Tiles,
                                  Choice_3TilesLen, false, false, false, OBJPRIORITY_2, true );
            tilecnt = loadSprite( Oam, spriteInfo, ++oamIndex, 4, tilecnt,
                                  200, 128, 32, 32, atksPal, atksTiles,
                                  atksTilesLen, false, false, false, OBJPRIORITY_1, true );
            consoleSetWindow( &Bottom, 17, 17, 20, 8 );
            printf( "ATTACKEN" );
        }
        updateOAMSub( Oam );

        touchPosition t;
        while( 42 ) {
            updateTime( false );
            scanKeys( );
            t = touchReadXY( );

            //Accept touches that are almost on the sprite
            if( t.px > 224 && t.py > 164 ) { //Back
                battleUI::waitForTouchUp( );
                return 3;
            }
            if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
                if( !p_alreadySent && !p_alreadyChosen && !dead && t.px > x && t.px < x + 128 && t.py > y && t.py < y + 64 ) { //Send
                    battleUI::waitForTouchUp( );
                    return 0;
                }
                if( t.px > 20 && t.px < 124 && t.py > 128 && t.py < 160 ) { //Info
                    battleUI::waitForTouchUp( );
                    return 1;
                }
                if( t.px > 132 && t.px < 232 && t.py > 128 && t.py < 160 ) { //Moves
                    battleUI::waitForTouchUp( );
                    return 2;
                }
            }
        }
    }

    /**
    *  @param p_page: 1 show moves, 0 show status
    *  @returns 0: return to prvious screen, 1 view next pokémon, 2 view previous pokémon, 3 switch screen
    */
    u8 showDetailedInformation( POKEMON::pokemon& p_pokemon, u8 p_page ) {
        drawSub( );
        initColors( );
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );
        Oam->oamBuffer[ SUB_Back_OAM ].isHidden = false;
        u16 tilecnt = 0;
        u8  palIndex = 4;
        u8 oamIndex = SUB_Back_OAM;

        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM, 0, tilecnt,
                              SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                              BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );
        // ^ Sprite
        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM - 2, 1, tilecnt,
                              SCREEN_WIDTH - 22, SCREEN_HEIGHT - 28 - 24, 32, 32, UpPal,
                              UpTiles, UpTilesLen, false, false, false, OBJPRIORITY_1, true );
        // v Sprite
        tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM - 3, 2, tilecnt,
                              SCREEN_WIDTH - 28 - 24, SCREEN_HEIGHT - 22, 32, 32, DownPal,
                              DownTiles, DownTilesLen, false, false, false, OBJPRIORITY_1, true );
        if( !p_page ) {
            tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM - 4, 3, tilecnt,
                                  SCREEN_WIDTH - 20, SCREEN_HEIGHT - 28 - 48, 32, 32, atksPal,
                                  atksTiles, atksTilesLen, false, false, false, OBJPRIORITY_2, true );
        } else {
            tilecnt = loadSprite( Oam, spriteInfo, SUB_Back_OAM - 5, 3, tilecnt,
                                  SCREEN_WIDTH - 20, SCREEN_HEIGHT - 28 - 48, 32, 32, memoPal,
                                  memoTiles, memoTilesLen, false, false, false, OBJPRIORITY_2, true );
        }

        POKEMON::PKMNDATA::pokemonData data;
        POKEMON::PKMNDATA::getAll( p_pokemon.m_boxdata.m_speciesId, data );

        updateOAMSub( Oam );

        u16 exptype = data.m_expType;

        if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
            if( !FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                16, 8, oamIndex, palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), p_pokemon.m_boxdata.m_isFemale ) )
                FS::loadPKMNSprite( Oam, spriteInfo, "nitro:/PICS/SPRITES/PKMN/", p_pokemon.m_boxdata.m_speciesId,
                16, 8, oamIndex, palIndex, tilecnt, true, p_pokemon.m_boxdata.isShiny( ), !p_pokemon.m_boxdata.m_isFemale );
            consoleSetWindow( &Bottom, 4, 0, 12, 2 );
            printf( "EP(%3i%%)\nKP(%3i%%)", ( p_pokemon.m_boxdata.m_experienceGained - POKEMON::EXP[ p_pokemon.m_Level - 1 ][ exptype ] )
                    * 100 / ( POKEMON::EXP[ p_pokemon.m_Level ][ exptype ] - POKEMON::EXP[ p_pokemon.m_Level - 1 ][ exptype ] ),
                    p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP );
            BATTLE::battleUI::displayHP( 100, 101, 46, 40, 245, 246, false, 50, 56, true );
            BATTLE::battleUI::displayHP( 100, 100 - p_pokemon.m_stats.m_acHP * 100 / p_pokemon.m_stats.m_maxHP, 46, 40, 245, 246, false, 50, 56, true );

            BATTLE::battleUI::displayEP( 100, 101, 46, 40, 247, 248, false, 59, 62, true );
            BATTLE::battleUI::displayEP( 0, ( p_pokemon.m_boxdata.m_experienceGained - POKEMON::EXP[ p_pokemon.m_Level - 1 ][ exptype ] )
                                         * 100 / ( POKEMON::EXP[ p_pokemon.m_Level ][ exptype ] - POKEMON::EXP[ p_pokemon.m_Level - 1 ][ exptype ] ),
                                         46, 40, 247, 248, false, 59, 62, true );
            cust_font.setColor( WHITE_IDX, 1 );

            consoleSetWindow( &Bottom, 2, 1, 13, 2 );

            std::swprintf( wbuffer, 20, L"%ls /", p_pokemon.m_boxdata.m_name );
            cust_font.printString( wbuffer, 16, 96, true );
            s8 G = p_pokemon.m_boxdata.gender( );

            if( p_pokemon.m_boxdata.m_speciesId != 29 && p_pokemon.m_boxdata.m_speciesId != 32 ) {
                if( G == 1 ) {
                    cust_font.setColor( BLUE_IDX, 1 );
                    cust_font.printChar( 136, 100, 102, true );
                } else {
                    cust_font.setColor( RED_IDX, 1 );
                    cust_font.printChar( 137, 100, 102, true );
                }
            }
            cust_font.setColor( WHITE_IDX, 1 );

            cust_font.printString( POKEMON::PKMNDATA::getDisplayName( p_pokemon.m_boxdata.m_speciesId ), 24, 110, true );

            if( p_pokemon.m_boxdata.getItem( ) ) {
                cust_font.printString( ITEMS::ItemList[ p_pokemon.m_boxdata.getItem( ) ].getDisplayName( true ).c_str( ),
                                       24, 124, true );
                FS::drawItemIcon( Oam, spriteInfo, ITEMS::ItemList[ p_pokemon.m_boxdata.getItem( ) ].m_itemName, 0, 116, oamIndex, palIndex, tilecnt, true );
            } else {
                cust_font.setColor( BLACK_IDX, 1 );
                cust_font.setColor( GRAY_IDX, 2 );
                cust_font.printString( ITEMS::ItemList[ p_pokemon.m_boxdata.getItem( ) ].getDisplayName( ).c_str( ), 24, 124, true );
            }
            cust_font.setColor( GRAY_IDX, 1 );
            cust_font.setColor( BLACK_IDX, 2 );

            if( data.m_types[ 0 ] == data.m_types[ 1 ] )
                drawTypeIcon( Oam, spriteInfo, oamIndex, palIndex, tilecnt, data.m_types[ 0 ], 224, 0, true );
            else {
                drawTypeIcon( Oam, spriteInfo, oamIndex, palIndex, tilecnt, data.m_types[ 0 ], 192, 0, true );
                drawTypeIcon( Oam, spriteInfo, oamIndex, palIndex, tilecnt, data.m_types[ 1 ], 224, 0, true );
            }

        } else {
            cust_font.setColor( WHITE_IDX, 1 );
            cust_font.printString( "Ei /", 16, 96, true );
            cust_font.printString( "Ei", 24, 110, true );
            cust_font.setColor( GRAY_IDX, 1 );
        }

        //Here starts the page specific stuff


        if( p_pokemon.m_boxdata.m_individualValues.m_isEgg )
            p_page = 0;

        if( p_page == 1 ) { //Moves
            for( u8 i = 0; i < 4; ++i ) {
                if( p_pokemon.m_boxdata.m_moves[ i ] ) {
                    auto acMove = AttackList[ p_pokemon.m_boxdata.m_moves[ i ] ];

                    BG_PALETTE_SUB[ 240 + i ] = POKEMON::PKMNDATA::getColor( acMove->m_moveType );

                    u8 w = 104, h = 32;
                    u8 x = 144 - 8 * i, y = 18 + ( h + 8 ) * i;

                    FONT::putrec( x + 1, y + 1, x + w + 2, y + h + 1,
                                  true, false, BLACK_IDX );
                    FONT::putrec( x, y, x + w, y + h,
                                  true, false, 240 + i );
                    FONT::putrec( x + 7, y + 5, x + w - 4, y + h - 1,
                                  true, false, BLACK_IDX );
                    FONT::putrec( x + 6, y + 4, x + w - 6, y + h - 2,
                                  true, false, WHITE_IDX );

                    cust_font.printString( acMove->m_moveName.c_str( ), x + 7, y + 7, true );
                    drawTypeIcon( Oam, spriteInfo, oamIndex, palIndex, tilecnt, acMove->m_moveType, x - 10, y - 7, true );
                    consoleSelect( &Bottom );
                    consoleSetWindow( &Bottom, x / 8, 5 + 5 * i, 20, 2 );
                    printf( "%6hhu/%2hhu AP",
                            p_pokemon.m_boxdata.m_acPP[ 0 ],
                            AttackList[ p_pokemon.m_boxdata.m_moves[ 0 ] ]->m_movePP * ( ( 5 + p_pokemon.m_boxdata.m_ppup.m_Up1 ) / 5 ) );
                }
            }
        } else { //Status
            if( !( p_pokemon.m_boxdata.m_individualValues.m_isEgg ) ) {
                cust_font.setColor( WHITE_IDX, 1 );
                sprintf( buffer, "KP                     %3i", p_pokemon.m_stats.m_maxHP );
                cust_font.printString( buffer, 130, 16, true );

                if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 0 ] == 1.1 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( RED_IDX, 2 );
                } else if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 0 ] == 0.9 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( BLUE_IDX, 2 );
                } else {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "ANG                   %3i", p_pokemon.m_stats.m_Atk );
                cust_font.printString( buffer, 126, 41, true );

                if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 1 ] == 1.1 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( RED_IDX, 2 );
                } else if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 1 ] == 0.9 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( BLUE_IDX, 2 );
                } else {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "VER                   %3i", p_pokemon.m_stats.m_Def );
                cust_font.printString( buffer, 124, 58, true );

                if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 3 ] == 1.1 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( RED_IDX, 2 );
                } else if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 3 ] == 0.9 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( BLUE_IDX, 2 );
                } else {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "SAN                   %3i", p_pokemon.m_stats.m_SAtk );
                cust_font.printString( buffer, 122, 75, true );

                if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 4 ] == 1.1 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( RED_IDX, 2 );
                } else if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 4 ] == 0.9 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( BLUE_IDX, 2 );
                } else {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "SVE                   %3i", p_pokemon.m_stats.m_SDef );
                cust_font.printString( buffer, 120, 92, true );

                if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 2 ] == 1.1 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( RED_IDX, 2 );
                } else if( POKEMON::NatMod[ p_pokemon.m_boxdata.getNature( ) ][ 2 ] == 0.9 ) {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( BLUE_IDX, 2 );
                } else {
                    cust_font.setColor( WHITE_IDX, 1 );
                    cust_font.setColor( GRAY_IDX, 2 );
                }
                sprintf( buffer, "INI                   \xC3\xC3""%3i", p_pokemon.m_stats.m_Spd );
                cust_font.printString( buffer, 118, 109, true );

                FONT::putrec( (u8)158, (u8)18, u8( 158 + 68 ), u8( 18 + 12 ), true, false, WHITE_IDX );

                FONT::putrec( (u8)158, (u8)18, u8( 158 + ( 68.0*p_pokemon.m_boxdata.IVget( 0 ) / 31 ) ), u8( 18 + 6 ), true, false, GRAY_IDX );
                FONT::putrec( (u8)158, u8( 18 + 6 ), u8( 158 + ( 68.0*p_pokemon.m_boxdata.m_effortValues[ 0 ] / 252 ) ), u8( 18 + 12 ), true, false, GRAY_IDX );

                for( int i = 1; i < 6; ++i ) {
                    FONT::putrec( u8( 156 - 2 * i ), u8( 26 + ( 17 * i ) ),
                                  u8( 156 - 2 * i + 68 ), u8( 26 + 12 + ( 17 * i ) ),
                                  true, false, WHITE_IDX );
                    FONT::putrec( u8( 156 - 2 * i ), u8( 26 + ( 17 * i ) ),
                                  u8( 156 - 2 * i + ( 68.0*p_pokemon.m_boxdata.IVget( i ) / 31 ) ),
                                  u8( 26 + 6 + ( 17 * i ) ),
                                  true, false, GRAY_IDX );
                    FONT::putrec( u8( 156 - 2 * i ), u8( 26 + 6 + ( 17 * i ) ),
                                  u8( 156 - 2 * i + ( 68.0*p_pokemon.m_boxdata.m_effortValues[ i ] / 252 ) ),
                                  u8( 26 + 12 + ( 17 * i ) ), true, false, GRAY_IDX );
                }

                //Ability
                auto acAbility = ability( p_pokemon.m_boxdata.m_ability );

                FONT::putrec( u8( 0 ), u8( 138 ), u8( 255 ), u8( 192 ), true, false, WHITE_IDX );
                cust_font.setColor( WHITE_IDX, 2 );
                cust_font.setColor( BLACK_IDX, 1 );
                u8 nlCnt = 0;
                auto nStr = FS::breakString( acAbility.m_flavourText, cust_font, 250 );
                for( auto c : nStr )
                    if( c == '\n' )
                        nlCnt++;
                cust_font.printString( nStr.c_str( ), 0, 138, true, u8( 16 - 2 * nlCnt ) );
                cust_font.printString( acAbility.m_abilityName.c_str( ), 5, 176, true );
                cust_font.setColor( GRAY_IDX, 1 );
                cust_font.setColor( BLACK_IDX, 2 );
            } else {
                cust_font.setColor( WHITE_IDX, 1 );
                cust_font.setColor( BLACK_IDX, 2 );
                if( p_pokemon.m_boxdata.m_steps > 10 ) {
                    cust_font.printString( "Was da wohl", 16 * 8, 50, true );
                    cust_font.printString( "schlüpfen wird?", 16 * 8, 70, true );
                    cust_font.printString( "Es dauert wohl", 16 * 8, 100, true );
                    cust_font.printString( "noch lange.", 16 * 8, 120, true );
                } else if( p_pokemon.m_boxdata.m_steps > 5 ) {
                    cust_font.printString( "Hat es sich", 16 * 8, 50, true );
                    cust_font.printString( "gerade bewegt?", 16 * 8, 70, true );
                    cust_font.printString( "Da tut sich", 16 * 8, 100, true );
                    cust_font.printString( "wohl bald was.", 16 * 8, 120, true );
                } else {
                    cust_font.printString( "Jetzt macht es", 16 * 8, 50, true );
                    cust_font.printString( "schon Geräusche!", 16 * 8, 70, true );
                    cust_font.printString( "Bald ist es", 16 * 8, 100, true );
                    cust_font.printString( "wohl soweit.", 16 * 8, 120, true );
                }
                cust_font.setColor( GRAY_IDX, 1 );
                cust_font.setColor( BLACK_IDX, 2 );
            }
        }

        touchPosition t;
        while( 42 ) {
            updateTime( false );
            scanKeys( );
            t = touchReadXY( );
            u32 p = keysHeld( );

            //Accept touches that are almost on the sprite
            if( t.px > 224 && t.py > 164 ) { //Back
                battleUI::waitForTouchUp( );
                return 0;
            } else if( p & KEY_UP ) {
                battleUI::waitForKeyUp( KEY_UP );
                return 2;
            } else if( p & KEY_DOWN ) {
                battleUI::waitForKeyUp( KEY_DOWN );
                return 1;
            } else if( p & KEY_RIGHT ) {
                battleUI::waitForKeyUp( KEY_RIGHT );
                return 3;
            } else if( p & KEY_LEFT ) {
                battleUI::waitForKeyUp( KEY_LEFT );
                return 3;
            } else if( ( sqrt( sq( 16 + SCREEN_HEIGHT - 28 - 24 - t.py ) + sq( 16 + SCREEN_WIDTH - 22 - t.px ) ) <= 16 ) ) {
                battleUI::waitForTouchUp( );
                return 2;
            } else if( ( sqrt( sq( 16 + SCREEN_WIDTH - 28 - 24 - t.px ) + sq( 16 + SCREEN_HEIGHT - 22 - t.py ) ) <= 16 ) ) {
                battleUI::waitForTouchUp( );
                return 1;
            } else if( ( sqrt( sq( 16 + SCREEN_HEIGHT - 28 - 48 - t.py ) + sq( 16 + SCREEN_WIDTH - 20 - t.px ) ) <= 16 ) ) {
                battleUI::waitForTouchUp( );
                return 3;
            }
        }
    }

    u8 battleUI::choosePKMN( bool p_firstIsChosen, bool p_back ) {
START:
        consoleSelect( &Bottom );
        undrawPKMNChoiceScreen( );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleClear( );
        u8 result = 0;
        Oam->oamBuffer[ SUB_Back_OAM ].isHidden = !p_back;
        updateOAMSub( Oam );
        drawPKMNChoiceScreen( _battle, p_firstIsChosen );
        drawSub( );
        initColors( );
        FONT::putrec( (u8)0, (u8)0, (u8)255, (u8)28, true, false, WHITE_IDX );

        writeLogText( L"Welches PKMN?" );

        touchPosition t;
        while( 42 ) {
            Oam->oamBuffer[ SUB_Back_OAM ].isHidden = !p_back;
            updateOAMSub( Oam );

            updateTime( false );
            scanKeys( );
            t = touchReadXY( );

            //Accept touches that are almost on the sprite
            if(p_back && t.px > 224 && t.py > 164 ) { //Back
                waitForTouchUp( );
                result = 0;
                break;
            }
            auto teamSz = _battle->_player->m_pkmnTeam->size( );
            for( u8 i = 0; i < teamSz; ++i ) {
                u8 x = Oam->oamBuffer[ SUB_CHOICE_START + 2 * i ].x;
                u8 y = Oam->oamBuffer[ SUB_CHOICE_START + 2 * i ].y;

                if( t.px > x && t.px < x + 96 && t.py > y && t.py < y + 42 ) {
                    waitForTouchUp( );
                    result = i;
                    u8 tmp = 1;
                    auto acPkmn = ACPKMN2( *_battle, result, PLAYER );
                    undrawPKMNChoiceScreen( );
                    consoleSetWindow( &Bottom, 0, 0, 32, 24 );
                    consoleClear( );
                    while( tmp = showConfirmation( acPkmn, !result || ( result == p_firstIsChosen ), result == firstMoveSwitchTarget ) ) {
                        if( tmp == 3 )
                            break;
                        u8 oldtmp = tmp - 1;
                        while( tmp = showDetailedInformation( acPkmn, tmp - 1 ) ) {
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
                        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
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
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
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

        u8 hpx = OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
            hpy = OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y;

        displayHP( 100, 100 - ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP * 100 / ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_maxHP,
                   hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, true );

        consoleSelect( &Top );
        if( p_opponent == p_pokemonPos ) {
            consoleSetWindow( &Top, ( hpx + 40 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
            printf( "%10ls%c\n",
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_boxdata.m_name,
                    GENDER( ACPKMN2( *_battle, p_pokemonPos, p_opponent ) ) );
            printf( "Lv%3d%4dKP\n", ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_Level,
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP );
        } else {
            consoleSetWindow( &Top, ( hpx - 88 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
            printf( "%10ls%c\n",
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_boxdata.m_name,
                    GENDER( ACPKMN2( *_battle, p_pokemonPos, p_opponent ) ) );
            printf( "Lv%3d%4dKP\n", ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_Level,
                    ACPKMN2( *_battle, p_pokemonPos, p_opponent ).m_stats.m_acHP );
        }
    }

    void battleUI::applyEXPChanges( ) {
        for( u8 i = 0; i < 4; ++i ) {
            bool opponent = i % 2;
            bool pokemonPos = i / 2;

            if( !_battle->m_battleMode == battle::DOUBLE && pokemonPos )
                continue;

            u8 hpx = OamTop->oamBuffer[ HP_IDX( opponent, pokemonPos ) ].x,
                hpy = OamTop->oamBuffer[ HP_IDX( opponent, pokemonPos ) ].y;

            POKEMON::PKMNDATA::pokemonData p;
            auto acPkmn = ACPKMN2( *_battle, pokemonPos, opponent );

            if( !acPkmn.m_stats.m_acHP )
                continue;

            POKEMON::PKMNDATA::getAll( acPkmn.m_boxdata.m_speciesId, p );

            displayEP( 0, ( acPkmn.m_boxdata.m_experienceGained - POKEMON::EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) * 100 /
                       ( POKEMON::EXP[ acPkmn.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ),
                       hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, true );
        }
    }

    void battleUI::updateStats( bool p_opponent, u8 p_pokemonPos, bool p_move ) {
        //if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
        //    return;
        setStsBallSts( p_opponent, p_pokemonPos, ACPKMNSTS2( *_battle, p_pokemonPos, p_opponent ), false );
        if( p_pokemonPos <= ( _battle->m_battleMode == battle::DOUBLE)  && p_move
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
        updateOAM( OamTop );
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
            OamTop->oamBuffer[ i ].isHidden = true;

        //Hide HP Bar
        //OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].isHidden = true;
        displayHP( 100, 100, OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
                   OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y,
                   HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );
        displayEP( 100, 100, OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x,
                   OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y, OWN1_EP_COL, OWN1_EP_COL + 1, false );
        //setStsBallVisibility( p_opponent, p_pokemonPos, true, false );
        setStsBallSts( p_opponent, p_pokemonPos, ACPKMNSTS2( *_battle, p_pokemonPos, p_opponent ), false );
        updateOAM( OamTop );

        //Clear text
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
        consoleSelect( &Top );
        if( p_opponent == p_pokemonPos ) {
            consoleSetWindow( &Top, ( hpx + 40 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
        } else {
            consoleSetWindow( &Top, ( hpx - 88 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            consoleClear( );
        }
    }

    void animatePokeBall( u8 p_x, u8 p_y, u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt ) {

        SpriteInfo * type1Info = &spriteInfoTop[ p_oamIndex ];
        SpriteEntry * type1 = &OamTop->oamBuffer[ p_oamIndex ];
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

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall1Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall1Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall2Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall3Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall3TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall4Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall4TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall5Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall5TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall6Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall6TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall7Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall7TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall8Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall8TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall9Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall9TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall10Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall10TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, PokeBall11Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], PokeBall11TilesLen );
        updateOAM( OamTop );
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

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        updateOAM( OamTop );
    }

    void animateShiny( u8 p_x, u8 p_y, u8 p_oamIndex, u8 p_palCnt, u16 p_tileCnt ) {
        SpriteInfo * type1Info = &spriteInfoTop[ p_oamIndex ];
        SpriteEntry * type1 = &OamTop->oamBuffer[ p_oamIndex ];
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

        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny1Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny1TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Pal, &SPRITE_PALETTE[ (p_palCnt)* COLORS_PER_PALETTE ], 32 );
        dmaCopyHalfWords( SPRITE_DMA_CHANNEL, Shiny2Tiles, &SPRITE_GFX[ p_tileCnt * OFFSET_MULTIPLIER ], Shiny2TilesLen );
        updateOAM( OamTop );
        for( int i = 0; i < 2; ++i )
            swiWaitForVBlank( );
        type1->isHidden = true;
        updateOAM( OamTop );
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
        updateOAM( OamTop );

        animatePokeBall( x + 40, y + 40, PB_ANIM, 15, TILESTART );

        //Load the PKMN sprite
        u8 oamIdx = PKMN_IDX( p_pokemonPos, p_opponent ) - 1;
        u8 palIdx = PKMN_PAL_IDX( p_pokemonPos, p_opponent ) - 1;
        u16 tileCnt = PKMN_TILE_IDX( p_pokemonPos, p_opponent );

        if( p_opponent ) {
            if( !FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/", acPkmn.m_boxdata.m_speciesId, x, y,
                oamIdx, palIdx, tileCnt, false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMN/",
                    acPkmn.m_boxdata.m_speciesId, x, y,
                    oamIdx, palIdx, tileCnt, false,
                    acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( L"Sprite failed[A]" );
                }
            }
        } else {
            if( !FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/", acPkmn.m_boxdata.m_speciesId, x, y,
                oamIdx, palIdx, tileCnt, false, acPkmn.m_boxdata.isShiny( ), acPkmn.m_boxdata.m_isFemale, false ) ) {
                if( !FS::loadPKMNSprite( OamTop, spriteInfoTop, "nitro:/PICS/SPRITES/PKMNBACK/",
                    acPkmn.m_boxdata.m_speciesId, x, y,
                    oamIdx, palIdx, tileCnt, false,
                    acPkmn.m_boxdata.isShiny( ), !acPkmn.m_boxdata.m_isFemale, false ) ) {
                    _battle->log( L"Sprite failed[A]" );
                }
            }
        }
        if( acPkmn.m_boxdata.isShiny( ) )
            animateShiny( x + 16, y + 16, SHINY_ANIM, 15, TILESTART );

        setStsBallPosition( p_opponent, p_pokemonPos, hpx + 8, hpy + 8, false );
        OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].isHidden = false;
        OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].x = hpx;
        OamTop->oamBuffer[ HP_IDX( p_opponent, p_pokemonPos ) ].y = hpy;
        setStsBallVisibility( p_opponent, p_pokemonPos, false, false );
        updateOAM( OamTop );

        POKEMON::PKMNDATA::pokemonData p;
        POKEMON::PKMNDATA::getAll( acPkmn.m_boxdata.m_speciesId, p );

        displayHP( 100, 101, hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );
        displayHP( 100, 100 - acPkmn.m_stats.m_acHP * 100 / acPkmn.m_stats.m_maxHP,
                   hpx, hpy, HP_COL( p_opponent, p_pokemonPos ), HP_COL( p_opponent, p_pokemonPos ) + 1, false );

        displayEP( 0, ( acPkmn.m_boxdata.m_experienceGained - POKEMON::EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ) * 100 /
                   ( POKEMON::EXP[ acPkmn.m_Level ][ p.m_expType ] - POKEMON::EXP[ acPkmn.m_Level - 1 ][ p.m_expType ] ),
                   hpx, hpy, OWN1_EP_COL, OWN1_EP_COL + 1, false );

        consoleSelect( &Top );
        if( p_opponent == p_pokemonPos ) {
            consoleSetWindow( &Top, ( hpx + 40 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
            printf( "%10ls%c\n",
                    acPkmn.m_boxdata.m_name,
                    GENDER( acPkmn ) );
            printf( "Lv%3d%4dKP\n", acPkmn.m_Level,
                    acPkmn.m_stats.m_acHP );
        } else {
            consoleSetWindow( &Top, ( hpx - 88 ) / 8, ( hpy + 8 ) / 8, 20, 3 );
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

    }

    void battleUI::learnMove( u8 p_pokemonPos, u16 p_move ) {
        if( !_battle->m_battleMode == battle::DOUBLE && p_pokemonPos )
            return;


        auto& acPkmn = ACPKMN2( *_battle, p_pokemonPos, PLAYER );
        if( acPkmn.m_boxdata.m_moves[ 3 ] ) {
            std::swprintf( wbuffer, 50, L"%ls kann nun\n%s erlernen![A][CLEAR]Aber %ls kennt\nbereits 4 Attacken.[A]",
                           acPkmn.m_boxdata.m_name,
                           AttackList[ p_move ]->m_moveName.c_str( ),
                           acPkmn.m_boxdata.m_name );
            _battle->log( wbuffer );
            yesNoBox yn;
ST:
            if( yn.getResult( "Soll eine Attacke\nvergessen werden?" ) ) {
                auto res = chooseAttack( p_pokemonPos );
                if( !res ) {
                    std::sprintf( buffer, "Aufgeben %s zu erlernen?", AttackList[ p_move ]->m_moveName.c_str( ) );
                    if( !yn.getResult( buffer ) )
                        goto ST;
                } else {
                    std::swprintf( wbuffer, 100, L"%ls vergisst %s[A]\nund erlernt %s![A]",
                                   acPkmn.m_boxdata.m_name,
                                   AttackList[ res ]->m_moveName.c_str( ),
                                   AttackList[ p_move ]->m_moveName.c_str( ) );
                    _battle->log( wbuffer );

                    for( u8 i = 0; i < 4; ++i )
                        if( acPkmn.m_boxdata.m_moves[ i ] == res )
                            acPkmn.m_boxdata.m_moves[ i ] = p_move;
                }
            } else {
                std::sprintf( buffer, "Aufgeben %s zu erlernen?", AttackList[ p_move ]->m_moveName.c_str( ) );
                if( !yn.getResult( buffer ) )
                    goto ST;
            }
        } else {
            for( u8 i = 0; i < 4; ++i ) {
                if( !acPkmn.m_boxdata.m_moves[ i ] ) {
                    acPkmn.m_boxdata.m_moves[ i ] = p_move;
                    std::swprintf( wbuffer, 50, L"%ls erlernt %s![A]",
                                   acPkmn.m_boxdata.m_name,
                                   AttackList[ p_move ]->m_moveName.c_str( ) );
                    _battle->log( wbuffer );
                    break;
                }
            }
        }
    }

    void battleUI::showEndScreen( ) {

        initOAMTable( OamTop );
        consoleSetWindow( &Top, 0, 0, 32, 24 );
        consoleSelect( &Top );
        consoleClear( );

        dmaCopy( mug_001_1Bitmap, bgGetGfxPtr( bg2 ), 256 * 192 );
        dmaCopy( mug_001_1Pal, BG_PALETTE, 64 );
    }

    void battleUI::dinit( ) {
        videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
        dmaFillWords( 0, bgGetGfxPtr( bg2 ), 256 * 192 );
        dmaFillWords( 0, bgGetGfxPtr( bg3 ), 256 * 192 );
        consoleSetWindow( &Bottom, 0, 0, 32, 24 );
        consoleSelect( &Bottom );
        consoleClear( );
        drawSub( );
        initOAMTableSub( Oam );
        initOAMTable( OamTop );
        initMainSprites( Oam, spriteInfo );
        setMainSpriteVisibility( false );
        Oam->oamBuffer[ 8 ].isHidden = true;
        Oam->oamBuffer[ 0 ].isHidden = true;
        Oam->oamBuffer[ 1 ].isHidden = false;
    }

    //////////////////////////////////////////////////////////////////////////
    // END BATTLE_UI
    //////////////////////////////////////////////////////////////////////////
}