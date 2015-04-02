#include "yesNoBox.h"
#include "uio.h"
#include "../defines.h"

namespace IO {

    void yesNoBox::draw( u8 p_pressedIdx ) {
        printChoiceBox( 28, 102, 122, 134, 6, RED_IDX, p_pressedIdx == 0 );
        regularFont->printString( "Ja", 65 + 2 * ( p_pressedIdx == 0 ),
                                  110 + ( p_pressedIdx == 0 ), true );
        printChoiceBox( 134, 102, 228, 134, 6, BLUE_IDX, p_pressedIdx == 1 );
        regularFont->printString( "Nein", 165 + 2 * ( p_pressedIdx == 1 ),
                                  110 + ( p_pressedIdx == 1 ), true );
    }

    yesNoBox::yesNoBox( ) {
        initTextField( );
        initOAMTable( true );
        _isNamed = false;
    }
    yesNoBox::yesNoBox( const char* p_name ) {
        initTextField( );
        initOAMTable( true );
        regularFont->printString( p_name, 8, 8, true );

        swiWaitForVBlank( );
        _isNamed = true;
    }
    yesNoBox::yesNoBox( messageBox p_box ) {
        initTextField( );
        initOAMTable( true );
        _isNamed = p_box.m_isNamed;
    }

    bool yesNoBox::getResult( const char* p_text = 0, bool p_time ) {
        if( p_text ) {
            if( _isNamed )
                regularFont->printStringD( p_text, 72, 8, true );
            else
                regularFont->printStringD( p_text, 8, 8, true );
        }
        draw( 2 );
        bool result;
        loop( ) {
            swiWaitForVBlank( );
            if( p_time )
                updateTime( );
            touchPosition t;
            touchRead( &t );
            if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
                draw( 0 );
                if( !waitForTouchUp( p_time, false, 28, 102, 122, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = true;
                break;
            } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
                draw( 1 );
                if( !waitForTouchUp( p_time, false, 134, 102, 228, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = false;
                break;
            }
        }
        return result;
    }
    bool yesNoBox::getResult( const wchar_t* p_text = 0, bool p_time ) {
        if( p_text ) {
            if( _isNamed )
                regularFont->printStringD( p_text, 72, 8, true );
            else
                regularFont->printStringD( p_text, 8, 8, true );
        }
        draw( 2 );
        bool result;
        loop( ) {
            swiWaitForVBlank( );
            if( p_time )
                updateTime( );
            touchPosition t;
            touchRead( &t );
            if( t.px >= 28 && t.py >= 102 && t.px <= 122 && t.py <= 134 ) {
                draw( 0 );
                if( !waitForTouchUp( p_time, false, 28, 102, 122, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = true;
                break;
            } else if( t.px >= 134 && t.py >= 102 && t.px <= 228 && t.py <= 134 ) {
                draw( 1 );
                if( !waitForTouchUp( p_time, false, 134, 102, 228, 134 ) ) {
                    draw( 2 );
                    continue;
                }
                result = false;
                break;
            }
        }
        return result;
    }

}