#pragma once
#include "box.h"

namespace BOX {
    class boxUI {
    public:
        void init( );
        void draw( u16 p_pokemon, u8 p_pos, box* p_box );
    };
}