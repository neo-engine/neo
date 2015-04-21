#pragma once

#include <nds.h>

#include "box.h"
#include "boxUI.h"

namespace BOX {
    class boxViewer {
    private:
        box* _box;
        boxUI* _boxUI;
        u16 _currPkmn;
        u8 _currPos;

    public:
        boxViewer( box* p_box, boxUI* p_boxUI, u16 p_currPkmn )
            : _box( p_box ), _boxUI( p_boxUI ), _currPkmn( p_currPkmn ), _currPos( 0 ) {

        }

        void run( bool p_allowTakePkmn = false );
    };
}