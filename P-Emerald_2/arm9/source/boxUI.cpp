#include <nds.h>

#include "boxUI.h"
#include "uio.h"
#include "defines.h"
#include "fs.h"

#include "Back.h"
#include "A.h"
#include "Forward.h"
#include "Backward.h"
#include "Up.h"
#include "Down.h"

namespace BOX {

    void boxUI::init( ) {
        IO::vramSetup( );
        videoSetMode( MODE_5_2D/* | DISPLAY_BG2_ACTIVE*/ | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D );
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

    void boxUI::draw( u16 p_pokemon, u8 p_pos, box* p_box ) {
        pokemonData p; getAll( p_pokemon, p );

        //SubScreen stuff
        IO::printChoiceBox( 48, 23, 204, 48, 6, COLOR_IDX, false );
        IO::printString( IO::regularFont, "TEST", 126 - IO::regularFont->stringWidth( "TEST" ) / 2, 28, true );

        u16 cnt = 0;
        for( u8 i = 0; i < 4; ++i )
            for( u8 j = 0; j < 7; ++j )
                IO::printChoiceBox( 16 + 32 * j, 57 + 28 * i,
                44 + 32 * j, 78 + 28 * i, 3, ( cnt++ == p_pos ) ? RED_IDX : GRAY_IDX, false );
    }
}