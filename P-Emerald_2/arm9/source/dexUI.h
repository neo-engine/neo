#pragma once

#include <nds.h>
#include "pokemon.h"

namespace DEX {
    class dexUI {
        friend class dex;
    private:
        u8* _inDex;
        u16 _currPkmn;
        u16 _maxPkmn;
        u8 _currPage;
        u8 _currForme;

        void init( );
        void drawPage( bool p_newPok );
        void drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName );
    public:
        dexUI( u8* p_inDex, u16 p_currPkmn, u16 p_maxPkmn )
            :_inDex( p_inDex ), _currPkmn( p_currPkmn ), _maxPkmn( p_maxPkmn ), _currPage( 0 ) { }
    };
}