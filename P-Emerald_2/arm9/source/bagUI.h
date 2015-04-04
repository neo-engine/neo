#pragma once
#include <nds.h>

#include "bag.h"

namespace BAG {
    class bagUI {
    private:
        bag* _bag;
        bag::bagType _currentPage;
        u8 _itemIdx[ 5 ];

        void init( );
        void drawBagPage( bag::bagType p_page );
    public:
        bagUI( bag* _bag, bag::bagType p_page, u8 p_itemIdx[ 5 ] );

        void run( );
    };
}