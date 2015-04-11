#pragma once
#include <nds.h>
#include <vector>
#include <functional>

#include "bag.h"
#include "bagUI.h"
#include "item.h"
#include "uio.h"

namespace BAG {
    class bagViewer {
    private:
        u8 _currPage;
        u16 _currItem;

        std::function<bool( std::pair<u16, u16>, std::pair<u16, u16> )> _currCmp;

        bag* _origBag;

        std::vector<IO::inputTarget> _ranges;
        u8 _atHandOam;

        bagUI* _bagUI;

    public:
        bagViewer( bag* p_bag, bagUI* p_bagUI );
        void run( u8 p_startPage, u16 p_startIdx );
    };
}