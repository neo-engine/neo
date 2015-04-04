#pragma once

#include <vector>

#include "pokemon.h"
#include "statusScreenUI.h"

namespace STS {
    class statusScreen {
    protected:
        u8 _page;
        u8 _pkmnIdx;
        std::vector<pokemon>* _pokemon;
        statusScreenUI* _stsUI;
    public:

        virtual s16 drawPage( bool p_time, s8 p_timeParameter ) = 0;
        virtual void run( bool p_time, s8 p_timeParameter ) = 0;
    };

    class regStsScreen : public statusScreen {
    public:
        regStsScreen( u8 p_pkmnIdx, statusScreenUI* p_stsUI );

        void run( bool p_time, s8 p_timeParameter ) override;
        s16 drawPage( bool p_time, s8 p_timeParameter ) override;
    };

    //class battleStsScreen : public statusScreen {
    //    s16 run( bool _time, s8 p_timeParameter ) override;
    //    s16 drawPage( bool p_time, s8 p_timeParameter ) override;
    //};
}