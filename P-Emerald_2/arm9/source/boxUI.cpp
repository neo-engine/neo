#include <nds.h>

#include "boxUI.h"
#include "uio.h"
#include "defines.h"
#include "fs.h"
#include "pokemon.h"

#include "Back.h"
#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Up.h"
#include "Down.h"

namespace BOX {

    void boxUI::init( ) {
        IO::vramSetup( );
        videoSetMode( MODE_5_2D/* | DISPLAY_BG2_ACTIVE*/ | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );
        videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_EXT_PALETTE );
        
        swiWaitForVBlank( );
        IO::Top = *consoleInit( &IO::Top, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true );
        consoleSetFont( &IO::Top, IO::consoleFont );
        FS::readPictureData( bgGetGfxPtr( IO::bg3 ), "nitro:/PICS/", "PKMNInfoScreen" );

        IO::Bottom = *consoleInit( &IO::Bottom, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true );
        consoleSetFont( &IO::Bottom, IO::consoleFont );

        consoleSelect( &IO::Top );
        consoleSetWindow( &IO::Top, 0, 0, 32, 24 );
        consoleClear( );
        consoleSetWindow( &IO::Bottom, 0, 0, 32, 24 );
        consoleSelect( &IO::Bottom );
        consoleClear( );

        bgUpdate( );

        IO::drawSub( );

        IO::initOAMTable( true );
        u16 nextAvailableTileIdx = 0;

        nextAvailableTileIdx = IO::loadSprite( BACK_ID, BACK_ID, 0, nextAvailableTileIdx,
                                               SCREEN_WIDTH - 28, SCREEN_HEIGHT - 28, 32, 32, BackPal,
                                               BackTiles, BackTilesLen, false, false, false, OBJPRIORITY_0, true );

        IO::updateOAM( true );
        IO::initOAMTable( false );

        BG_PALETTE[ WHITE_IDX ] = WHITE;
        BG_PALETTE[ GRAY_IDX ] = GRAY;
        BG_PALETTE[ BLACK_IDX ] = BLACK;

        BG_PALETTE_SUB[ COLOR_IDX ] = GREEN;
        BG_PALETTE_SUB[ WHITE_IDX ] = WHITE;
        BG_PALETTE_SUB[ GRAY_IDX ] = GRAY;
        BG_PALETTE_SUB[ BLACK_IDX ] = BLACK;
        BG_PALETTE_SUB[ RED_IDX ] = RED;
    }

#define POS_X( i ) ( 16 + 32 * ( (i) % 7 ) )
#define POS_Y( i ) ( 57 + 28 * ( (i) / 7 ) )

    std::vector<IO::inputTarget> boxUI::draw( std::pair<u16, u16> p_pokemon[ 30 ], u8 p_pos, box* p_box, u8 p_oldpos, bool p_newPok ) {
        std::vector<IO::inputTarget> res;
        for( u8 i = 0; i < 28; ++i )
            res.push_back( IO::inputTarget( POS_X( i ), POS_Y( i ), POS_X( i ) + 28, POS_Y( i ) + 21 ) );

        pokemonData p; getAll( p_pokemon[ p_pos + 1 ].first, p );

        //SubScreen stuff
        IO::printChoiceBox( 48, 23, 204, 48, 6, COLOR_IDX, false );
        if( p_pokemon[ p_pos + 1 ].first ) {
            char buffer[ 50 ];
            sprintf( buffer, "%s (%u)", p.m_displayName, p_pos/*p_box->count( p_pokemon[ p_pos + 1 ].first )*/ );
            IO::printString( IO::regularFont, buffer, 126 - IO::regularFont->stringWidth( buffer ) / 2, 28, true );
        } else {
            IO::printString( IO::regularFont, "Leer",
                             126 - IO::regularFont->stringWidth( "Leer" ) / 2, 28, true );
        }

        if( p_oldpos == 42 ) {
            for( u8 cnt = 0; cnt < 28; ++cnt ) {
                IO::printChoiceBox( POS_X( cnt ), POS_Y( cnt ),
                                    POS_X( cnt ) + 28, POS_Y( cnt ) + 21, 3, ( cnt == p_pos ) ? RED_IDX : GRAY_IDX, false );
            }
        } else {
            IO::printChoiceBox( POS_X( p_oldpos ), POS_Y( p_oldpos ),
                                POS_X( p_oldpos ) + 28, POS_Y( p_oldpos ) + 21, 3, GRAY_IDX, false );
            IO::printChoiceBox( POS_X( p_pos ), POS_Y( p_pos ),
                                POS_X( p_pos ) + 28, POS_Y( p_pos ) + 21, 3, RED_IDX, false );
        }

        u8 oam = 3;
        u8 pal = 4;
        u16 tileCnt = 192;

        u16 lst = p_pokemon[ 1 ].first;

        for( u8 i = 1; i < 29; ++i ) {
            if( !p_pokemon[ i ].first ) {
                IO::Oam->oamBuffer[ oam++ ].isHidden = true;
                continue;
            }

            if( i > 1 && lst == p_pokemon[ i ].first ) {
                memcpy( &IO::Oam->oamBuffer[ oam ], &IO::Oam->oamBuffer[ oam - 1 ], sizeof( SpriteEntry ) );
                IO::Oam->oamBuffer[ oam ].x = POS_X( i - 1 ) - 3;
                IO::Oam->oamBuffer[ oam++ ].y = POS_Y( i - 1 ) - 10;
            } else {
                lst = p_pokemon[ i ].first;
                tileCnt = IO::loadPKMNIcon( lst, POS_X( i - 1 ) - 3, POS_Y( i - 1 ) - 10, oam++, pal / 16, pal % 16, tileCnt );
                ++pal;
            }
        }
        IO::updateOAM( true );
        return res;
    }
}