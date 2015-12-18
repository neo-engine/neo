#pragma once

#include <nds.h>

#include "box.h"
#include "boxUI.h"

namespace BOX {
    class boxViewer {
    private:
        box* _box;
        boxUI* _boxUI;
        u8 _currPos;
        std::pair<u16, u16> _currPage[ 30 ];

        std::vector<IO::inputTarget> _ranges;
        u8 _atHandOam;

        u16 nextNonEmptyBox( u16 p_start );
        u16 previousNonEmptyBox( u16 p_start );
        void generateNextPage( );
        void generatePreviousPage( );
    public:
        boxViewer( box* p_box, boxUI* p_boxUI, u16 p_currPkmn )
            : _box( p_box ), _boxUI( p_boxUI ), _currPos( 0 ) {
            for( u8 i = 0; i < 30; ++i )
                _currPage[ i ] = { 0, 0 };
            generateNextPage( );
        }

        void run( bool p_allowTakePkmn = false );
    };
}