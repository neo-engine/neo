#pragma once

#include <nds.h>

#include "messageBox.h"

namespace IO {
    class yesNoBox {
    public:
        yesNoBox( );
        yesNoBox( const char* p_name );
        yesNoBox( messageBox p_box );

        bool getResult( const char* p_text, bool p_time = true );
        bool getResult( const wchar_t* p_text, bool p_time = true );

        void draw( u8 p_pressedIdx );

    private:
        bool _isNamed;
    };

}