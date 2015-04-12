#pragma once

#include <nds.h>
#include "dexUI.h"

namespace DEX {
    extern const u8 dexsppos[ 2 ][ 9 ];
    class dex {
    private:
        dexUI* _dexUI;
        u16 _maxPkmn; //set this to -1 to show only a single dex entry

        u32 runPKMNPage( );
    public:
        dex( u16 p_maxPkmn, dexUI* p_dexUI )
            : _maxPkmn( p_maxPkmn ), _dexUI( p_dexUI ) { }
        ~dex( ) {
            delete _dexUI;
        }

        void run( u16 p_pkmnIdx );
    };
}