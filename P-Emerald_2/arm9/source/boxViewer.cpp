#include "boxViewer.h"
#include "boxUI.h"
#include "defines.h"
#include "uio.h"

namespace BOX {
    void boxViewer::run( bool p_allowTakePkmn ) {
        _boxUI->init( );
        _boxUI->draw( _currPkmn, _currPos, _box );
        touchPosition touch;
        loop( ) {
            swiWaitForVBlank( );
            scanKeys( );
            touchRead( &touch );
            int pressed = keysCurrent( );

            if( GET_AND_WAIT( KEY_B )
                || GET_AND_WAIT( KEY_X )
                || GET_AND_WAIT_C( SCREEN_WIDTH - 12, SCREEN_HEIGHT - 10, 16 ) ) {
                break;
            } 
        }
    }
}