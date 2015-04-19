#pragma once

#include <nds.h>

#include "messageBox.h"

namespace IO {
    class yesNoBox {
    public:
        yesNoBox( bool p_initSprites = true );
        yesNoBox( const char* p_name, bool p_initSprites = true );
        yesNoBox( messageBox p_box, bool p_initSprites = true );

        bool getResult( const char* p_text );
        bool getResult( const wchar_t* p_text );

        void draw( u8 p_pressedIdx );

    private:
        bool _isNamed;
    };

}