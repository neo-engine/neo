#pragma once

#include <nds.h>
#include "pokemon.h"

namespace DEX {
    class dexUI {
        friend class dex;
    private:
        bool _useInDex;
        u16 _currPkmn;
        u16 _maxPkmn;
        u8 _currPage;
        u8 _currForme;

        void init( );
        void drawPage( bool p_newPok, bool p_newPage = false );
        void drawFormes( u16 p_formeIdx, bool p_hasGenderDifference, const std::string& p_formeName );
        void undrawFormes( u16 p_formeIdx );
    public:
        dexUI( bool p_useInDex, u16 p_currPkmn, u16 p_maxPkmn )
            : _useInDex( p_useInDex ), _currPkmn( p_currPkmn ), _maxPkmn( p_maxPkmn ), _currPage( 0 ) { }
        u16 currPkmn( ) {
            return _currPkmn;
        }
    };
}