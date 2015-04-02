#pragma once

#include "../ds/pokemon.h"
#include <vector>

namespace STS {
    extern u8 fieldCnt;

    class statusScreenUI {
        friend class statusScreen;
    protected:
        std::vector<pokemon>& _pokemon;
    public:
        u8 m_pagemax;
        statusScreenUI( std::vector<pokemon>& p_pokemon, u8 p_pageMax = 3 );

        virtual void init( u8 p_current ) = 0;
        virtual void draw( u8 p_current, u8 p_page, bool p_newpok ) = 0;
        virtual void draw( u8 p_current ) = 0;
    };

    class regStsScreenUI : public statusScreenUI {
        u8 _current;
        void initTop( );
        void initSub( u16 p_pkmIdx );
    public:
        void init( u8 p_current ) override;
        void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
        void draw( u8 p_current ) override;
    };

    //class battleStsScreenUI : public statusScreenUI {
    //public:
    //    void init( u8 p_current ) override;
    //    void draw( u8 p_current, u8 p_page, bool p_newpok ) override;
    //    void draw( u8 p_current ) override;
    //};
}