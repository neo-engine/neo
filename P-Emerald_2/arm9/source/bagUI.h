#pragma once
#include <nds.h>

#include "bag.h"
#include "uio.h"

namespace BAG {
#define GIVE_ITEM 1
#define TAKE_ITEM 2
#define MOVE_ITEM 3

#define MOVE_BAG  4

#define BAG_CNT 5
    class bagUI {
        friend class bagViewer;
        std::vector<std::pair<IO::inputTarget, u16>> _ranges;
        std::vector<std::pair<u16, u16>> _bag[ BAG_CNT ];

        u8 drawPkmn( item* p_item );
    private:
        void init( );

        void updateAtHand( touchPosition p_touch, u8 p_oamIdx );

        std::vector<IO::inputTarget> drawBagPage( u8 p_page, u16 p_itemIdx );
        u8 getSprite( u8 p_rangeIdx, touchPosition p_currPos );
        u32 acceptDrop( u8 p_startIdx, u8 p_dropIdx, u8 p_oamIdx ); //First 10 bits: type, remaining: value
    };
}