#pragma once
#include "box.h"
#include "uio.h"
#include <vector>

namespace BOX {
    class boxUI {
    public:
        void init( );
        std::vector<IO::inputTarget> draw( std::pair<u16, u16> p_pokemon[ 30 ], u8 p_pos, box* p_box, u8 p_oldpos = 42, bool p_newPok = true );
    };
}