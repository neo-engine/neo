#pragma once
#include "box.h"
#include "uio.h"
#include <vector>

namespace BOX {
    class boxUI {
        friend class boxViewer;

        std::vector<std::pair<u8, IO::inputTarget>> _ranges;

        void init( );

        void updateAtHand( touchPosition p_touch, u8 p_oamIdx );

        std::vector<IO::inputTarget> draw( std::pair<u16, u16> p_pokemon[ 30 ], u8 p_pos, box* p_box, u8 p_oldpos = 42, bool p_newPok = true, bool p_showTeam = false );
        u8 getSprite( u8 p_oldIdx, u8 p_rangeIdx );
        u32 acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ); //First 10 bits: type, remaining: value
        u8 acceptTouch( u8 p_oldIdx, u8 p_rangeIdx, bool p_allowTakePkmn );
    };
}