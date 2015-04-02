#pragma once

#include <vector>

#include "../ds/pokemon.h"
#include "statusScreenUI.h"

namespace STS {
    class statusScreen {
    protected:
        u8 _page;
        u8 _pkmnIdx;
        std::vector<pokemon>& _pokemon;
        statusScreenUI* _stsUI;
    public:
        statusScreen( u8 p_pkmnIdx, statusScreenUI* p_stsUI );

        virtual s16 drawPage( bool p_time, s8 p_timeParameter ) = 0;
        virtual s16 run( bool p_time, s8 p_timeParameter ) = 0;
    };

    class regStsScreen : public statusScreen {
        s16 run( bool _time, s8 p_timeParameter ) override;
        s16 drawPage( bool p_time, s8 p_timeParameter ) override;
    };

    //class battleStsScreen : public statusScreen {
    //    s16 run( bool _time, s8 p_timeParameter ) override;
    //    s16 drawPage( bool p_time, s8 p_timeParameter ) override;
    //};
}